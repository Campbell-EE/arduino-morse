/****************************************************************
Design by Brady Campbell
created: 1/6/2025     last edited: 2/19/2025
Arduino Nano - Morse Code Project 

This version: V5 - 1st restructure 
****************************************************************/


/****************************************************************
KEY MORSE CODE FACTS
****************************************************************/
//    dot 1 unit                    '.'
//    dash 3 unit                   '-'
//    intra-character space 1 unit  'na' //built into dot & dash
//    inter-character space 3 unit  ' '
//    word space 7 unit             '|'
//    UNIT: 200ms 


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

/****************************************************************
ENCODE ALPHANUMBERIC TO MORSE
****************************************************************/
String translateAlphaToMorse(char alpha) {
  for (int i = 0; i < dictionarySize; i++) {
    if (dictionary[i].alpha == alpha) {

      //Serial.println(dictionary[i].morse); 
      return dictionary[i].morse;

    }
  }
  return "!";  // Return Error Character
}

/****************************************************************
TRANSLATE MORSE - PARSE AND CALL DECODE FUNCTION FOR EACH CHAR
****************************************************************/
    String translateMorseToAlpha(String morseMessage){
        String translatedMessage = ""; 
        String morseSubString = "";

        for(int i=0; i< morseMessage.length(); i++){
            char currentMorseChar = morseMessage.charAt(i);

            if(currentMorseChar != ' '){
                morseSubString += currentMorseChar; 
            }
            else{
                translatedMessage += translateMorseToAlpha(morseSubString);
                morseSubString = ""; //Reset for next Char 
            }
        }
        if (morseSubString.length() > 0) {
            translatedMessage += translateMorseToAlpha(morseSubString);
        }

        return translatedMessage; 

    }
/****************************************************************
DECODE MORSE TO ALPHANUMBERIC
****************************************************************/
    char decodeMorseToAlpha(String morseSubString) {
        for (int i = 0; i < dictionarySize; i++) {
            if (dictionary[i].morse == morseSubString) {
                return dictionary[i].alpha;
            }
        }
    return '?';  // Return Error Character 
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
//WIP WIP WIP WIP WIP WIP 
void morseSignalScribe(){

	if(scribeMode == HIGH){
		if(duration >= (0.5 * unit) && duration <= (2 * unit)){
			messageMorseIn += '.';
      Serial.print(".");

		}
	
		else if(duration > (2 * unit) && duration <= (4 * unit)){
			messageMorseIn += '-';
      Serial.print("-");
		}
	
		else{
      //add nothing to the string
      Serial.print("?");
			//alert user that dot/dash too short/long
		}
	}
	  
	if(scribeMode == LOW){
		if(duration >= (2 * unit) && duration <= ( 6 * unit)){
			messageMorseIn += ' ';
      Serial.print(" ");
		}
	
		else if(duration > (6 * unit) && duration <= (10 * unit)){
			messageMorseIn += '|';
		}
		else if(duration > (10 * unit)){
      //add nothing, looks like end of message 
      //handle end of message somehow
    }
		else{
			//do nothing, gap too short
		}


	}
}
	  
  //END SCRIBE

/****************************************************************
SERIAL INPUT HANDLING
****************************************************************/
void serialHandler(){
    while (Serial.available() > 0){
        serialIn += (char)Serial.read();
        delay(10); 
    }
    serialInLength = serialIn.length(); 
}

/****************************************************************
MODE MANAGER
****************************************************************/
void modeManager(){
    bool modeSwitch = digitalRead(ModeSwitchInputPIN);
    //delay(10); 
    if(modeSwitch == HIGH){
        //Set Mode to sending (and indicate)
        sendingMode = HIGH; 
        digitalWrite(SendingModeIndicatorPIN, HIGH); 
        receivingMode = LOW;
        digitalWrite(ReceivingModeIndicatorPIN, LOW); 
        //Serial.println("S");

    }
    else if(modeSwitch == LOW){
        //Set Mode to Receiving (and indicate)
        sendingMode = LOW; 
        digitalWrite(SendingModeIndicatorPIN, LOW); 
        receivingMode = HIGH;
        digitalWrite(ReceivingModeIndicatorPIN, HIGH); 
        //Serial.println("R");
 
    }
}

/****************************************************************
SETUP - RUNS JUST ONCE, LIGHT USE IN THIS PROGRAM
****************************************************************/
//SET BAUD
//SET PIN MODES 
//GIVE USER INSTRUCTIONS
void setup(){

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
void loop(){

    modeManager();
    /************************************************************
    SENDING 
    ************************************************************/
    if(receivingMode == HIGH){ 

        //SERIAL INPUT HANDLING
        serialHandler(); 

        //TRANSLATE ALPHANUMBERIC TO MORSE
        for(int i = 0; i < serialInLength; i++){
            messageMorseOut += translateAlphaToMorse(serialIn[i]);
        }

      

      if(serialIn != ""){
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
    else if(sendingMode == HIGH){ //CHECK IF RECEIVING 

        //DIGITAL INPUT HANDLING


        currentState = digitalRead(MorseInPin);
	
        if (lastState == LOW && currentState == HIGH){
          scribeMode = LOW; 
          currentEdge = millis();
          duration = currentEdge - lastEdge; 
          morseSignalScribe(); //pass duration and mode to scribe 
          lastEdge = millis();
        }

        if (lastState == HIGH && currentState == LOW){
          scribeMode = HIGH; 
          currentEdge = millis();
          duration = currentEdge - lastEdge;
          morseSignalScribe(); //pass duration and mode to scribe
          lastEdge = millis();
        }

        If(duration >= 10 * units){
          
        }
  
         lastState = currentState; //update state

        //TRANSLATE MORSE TO ALPHANUMBERIC

        //OUTPUT TO SERIAL
        
    }
}