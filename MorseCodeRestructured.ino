/****************************************************************
Design by Brady Campbell
created: 1/6/2025     last edited: 3/28/2025
Arduino Nano - Morse Code Project 

This version: V1
****************************************************************/

/****************************************************************
KEY MORSE CODE FACTS
****************************************************************/
//review these before making any changes 

//    dot 1 unit                    '.'  
//    dash 3 unit                   '-'
//    intra-character space 1 unit  'na' //built into dot & dash
//    inter-character space 3 unit  ' '
//    word space 7 unit             '|'
//    UNIT: 200ms  <-- IMPORTANT: this value MUST be IDENTICAL
//                                for both units or it won't work!

/****************************************************************
DICTIONARY - ARRAY OF A STRUCT
****************************************************************/
struct DictionaryMorse {
  char alpha;
  String morse;
};

DictionaryMorse dictionary[] = {
  //ALPHABET
  { 'A', ".- " },
  { 'B', "-... " },
  { 'C', "-.-. " },
  { 'D', "-.. " },
  { 'E', ". " },
  { 'F', "..-. " },
  { 'G', "--. " },
  { 'H', ".... " },
  { 'I', ".. " },
  { 'J', ".--- " },
  { 'K', "-.- " },
  { 'L', ".-.. " },
  { 'M', "-- " },
  { 'N', "-. " },
  { 'O', "--- " },
  { 'P', ".--. " },
  { 'Q', "--.- " },
  { 'R', ".-. " },
  { 'S', "... " },
  { 'T', "- " },
  { 'U', "..- " },
  { 'V', "...- " },
  { 'W', ".-- " },
  { 'X', "-..- " },
  { 'Y', "-.-- " },
  { 'Z', "--.. " },
  //NUMBERS
  { '1', ".---- " },
  { '2', "..--- " },
  { '3', "...-- " },
  { '4', "....- " },
  { '5', "..... " },
  { '6', "-.... " },
  { '7', "--... " },
  { '8', "---.. " },
  { '9', "----. " },
  { '0', "----- " },
  //PUNCTUATION
  { '.', ".-.-.- " },  //FULL STOP
  { ' ', "|" }         //SPACE
};

//DICTIONARY SIZE
int dictionarySize = sizeof(dictionary) / sizeof(dictionary[0]);


/****************************************************************
VARIABLES & PINS
****************************************************************/
//PINS:

    //INPUTS 
    int SignalInPIN  = 5;
    int ModeSwitchInputPIN = 3; 

    //OUTPUTS 
    int SignalOutPIN = 4;

    //INDICATORS 
    int SendingModeIndicatorPIN = 12;
    int ReceivingModeIndicatorPIN  = 11;
    int SignalIndicatorPIN = 2;

//VARIABLES: 

    //UNIT OF TIME
    unsigned int unit = 200; //milliseconds
    
    //TRANSLATE MORSE TO ALPHA
    String morseMessage;


    //SERIAL HANDLER
    String serialIn;
    int serialInLength;

    //MODE MANAGER
    bool receivingMode; 
    bool sendingMode; 

    //SIGNAL GENERATOR
    String messageMorseOut;
    unsigned int messageMorseOutLength;

    //SCRIBE & DECODING
    float duration; 
    bool scribeMode;
    String messageMorseIn;
    bool currentState; 
    bool lastState;
    float currentEdge;
    float lastEdge; 


/****************************************************************
ENCODE ALPHANUMBERIC TO MORSE & MORSE TO ALPHA.
****************************************************************/
String translateAlphaToMorse(char alpha) {
  for (int i = 0; i < dictionarySize; i++) {
    if (dictionary[i].alpha == alpha) {
      return dictionary[i].morse;
    }
  }
  return "!";  // Return Error Character
}

char translateMorseCharToAlphaChar(String morseSubString) {
  for (int i = 0; i < dictionarySize; i++) {
    if (dictionary[i].morse == morseSubString) {
      return dictionary[i].alpha;
    }
  }
  return "!";  // Return Error Character
}

/****************************************************************
TRANSLATE MORSE - PARSE AND CALL DECODE FUNCTION FOR EACH CHAR
****************************************************************/
String translateMorseToAlpha(String morseMessage) {
    String translatedMessage = "";
    String morseSubString = "";

    for (int i = 0; i < morseMessage.length(); i++) {
        char currentMorseChar = morseMessage.charAt(i);

        if (currentMorseChar != ' ' && currentMorseChar != '|') {
            morseSubString += currentMorseChar;  // Collecting Morse character
        } 
        else //if (currentMorseChar == ' ' || currentMorseChar == '|') 
        {
            // Process the collected Morse character when a space or word separator is encountered
            if (morseSubString.length() > 0) {
                char translatedChar = translateMorseCharToAlphaChar(morseSubString + ' ');
                translatedMessage += translatedChar;
                morseSubString = ""; // Reset for next character
            }
            if (currentMorseChar == '|') {
                translatedMessage += ' '; // Adding space for word separation
            }
        }
    }
    
    // Ensure the last Morse character is translated
    if (morseSubString.length() > 0) {
        char translatedChar = translateMorseCharToAlphaChar(morseSubString + ' ');
        translatedMessage += translatedChar;
    }

    messageMorseIn = "";
    return translatedMessage; 
    
}

/****************************************************************
MORSE SIGNAL GENERATOR 
****************************************************************/
void morseSignalGenerator() {
  for (int i = 0; i < messageMorseOutLength; i++) {
    switch (messageMorseOut[i]) {
      //DOT
      case '.':
        digitalWrite(SignalOutPIN, HIGH);
        digitalWrite(SignalIndicatorPIN, HIGH);
        delay(unit);  //dot
        digitalWrite(SignalOutPIN, LOW);
        digitalWrite(SignalIndicatorPIN, LOW);
        delay(unit);  //intra character space
        break;

      //DASH
      case '-':
        digitalWrite(SignalOutPIN, HIGH);
        digitalWrite(SignalIndicatorPIN, HIGH);
        delay(3 * unit);  //dash
        digitalWrite(SignalOutPIN, LOW);
        digitalWrite(SignalIndicatorPIN, LOW);
        delay(unit);  //intra character space
        break;

      //INTER CHARACTER SPACE
      case ' ':
        digitalWrite(SignalOutPIN, LOW);
        digitalWrite(SignalIndicatorPIN, LOW);
        delay(2 * unit);
        break;

      //INTER WORD SPACE
      case '|':
        digitalWrite(SignalOutPIN, LOW);
        digitalWrite(SignalIndicatorPIN, LOW);
        delay(6 * unit);
        break;
    }
  }
}

/****************************************************************
MORSE SIGNAL SCRIBE 
****************************************************************/
void morseSignalScribe() {

    if (scribeMode == HIGH) {
        if (duration >= (0.25 * unit) && duration <= (2 * unit)) {
            messageMorseIn += '.';
            Serial.print(".");
        } 
        else if (duration > (2 * unit)) {
            messageMorseIn += '-';
            Serial.print("-");
        }
        else {
            // Add an error handler or alert for invalid signal length.
        }
    }

    if (scribeMode == LOW) {
        if (duration >= (2 * unit) && duration <= (6 * unit)) {
            messageMorseIn += ' ';
            Serial.print(" "); // Intra-character gap
        } 
        else if (duration > (6 * unit)) {
            messageMorseIn += '|'; // Inter-word gap
            Serial.print("|");
        }
    }
}
  
/****************************************************************
SERIAL INPUT HANDLING
****************************************************************/
void serialHandler() {
    while (Serial.available() > 0) {
        serialIn += (char)Serial.read();
        delay(10); 
    }
    serialInLength = serialIn.length(); 
}

/****************************************************************
MODE MANAGER
****************************************************************/
void modeManager() {
    bool modeSwitch = digitalRead(ModeSwitchInputPIN);
    if (modeSwitch == HIGH) {
        //Set Mode to sending (and indicate)
        sendingMode = HIGH; 
        digitalWrite(SendingModeIndicatorPIN, HIGH); 
        receivingMode = LOW;
        digitalWrite(ReceivingModeIndicatorPIN, LOW); 
    }
    else if (modeSwitch == LOW) {
        //Set Mode to Receiving (and indicate)
        sendingMode = LOW; 
        digitalWrite(SendingModeIndicatorPIN, LOW); 
        receivingMode = HIGH;
        digitalWrite(ReceivingModeIndicatorPIN, HIGH); 
    }
}

/****************************************************************
SETUP - RUNS JUST ONCE, LIGHT USE IN THIS PROGRAM
****************************************************************/
void setup() {
    //SETUP 
    Serial.begin(9600);
    
    pinMode(SignalIndicatorPIN, OUTPUT);
    pinMode(SignalInPIN, INPUT);
    pinMode(SignalOutPIN, OUTPUT);

    pinMode(ModeSwitchInputPIN, INPUT);
    pinMode(SendingModeIndicatorPIN, OUTPUT);
    pinMode(ReceivingModeIndicatorPIN, OUTPUT);

    //USER INSTRUCTIONS 
    Serial.println("OPEN SERIAL, AWAITING USER INPUT");
}

/****************************************************************
LOOP - RUNS REPEATEDLY, MAIN BLOCK OF CODE FOR THIS PROGRAM 
****************************************************************/
void loop() {

    modeManager();

    /************************************************************
    SENDING 
    ************************************************************/
    if (sendingMode == HIGH) { 
        serialHandler(); 

        //TRANSLATE ALPHANUMBERIC TO MORSE
        for (int i = 0; i < serialInLength; i++) {
            messageMorseOut += translateAlphaToMorse(serialIn[i]);
        }

        if (serialIn != "") {
            //PRINT TO SERIAL
            Serial.println("SENDING MODE, YOUR MESSAGE: ");
            Serial.println("ALPHANUMBERIC:");
            Serial.println(serialIn);
            Serial.println("MORSE-ENCODED: "); 
            Serial.println(messageMorseOut); 
        }

        //SIGNAL GENERATOR
        messageMorseOutLength = messageMorseOut.length(); 
        morseSignalGenerator(); 
        serialIn = ""; 
        messageMorseOut = ""; 
    }

    /************************************************************
    RECEIVING 
    ************************************************************/
    else if (receivingMode == HIGH) { 

        currentState = digitalRead(SignalInPIN);
    
        if (lastState == LOW && currentState == HIGH) {
            scribeMode = LOW; 
            currentEdge = millis();
            duration = currentEdge - lastEdge; 
            morseSignalScribe(); //pass duration and mode to scribe 
            lastEdge = millis();
        }

        if (lastState == HIGH && currentState == LOW) {
            scribeMode = HIGH; 
            currentEdge = millis();
            duration = currentEdge - lastEdge;
            morseSignalScribe(); //pass duration and mode to scribe
            lastEdge = millis();
        }

        if (lastState == LOW && lastState == currentState) {
            currentEdge = millis();
            duration = currentEdge - lastEdge;

            if(duration >= 20 * unit){
                //TRANSLATE MORSE TO ALPHANUMBERIC
                if(messageMorseIn != ""){
                    Serial.println();
                    Serial.println("MORSE MESSAGE RECEIVED");
                    Serial.println(messageMorseIn);
                    Serial.println("TRANSLATED MESSAGE");
                    Serial.println(translateMorseToAlpha(messageMorseIn));

                }
            }
        }

        lastState = currentState; //update state
    }
}
