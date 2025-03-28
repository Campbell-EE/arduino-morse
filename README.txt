****************************************************************

README.txt - by Brady Campbell
created: 2/20/2025     last edited: 2/18/2025
Arduino Nano - Morse Code Project V5

This version ()
****************************************************************

This device is an automatic morse code keyer (AMK). It 
functions on an arduino nano with several other components. Two 
AMK units are required for communication, both units 
independently encode and decode messages. The first function 
encodes serial input from a PC into morse and the second decodes 
recieved messages back into legible alphanumberic text printed 
to the serial viewer. 

IMPORTANT - this design has not been stress tested with two AMKs
yet. There may be some errors that arise that I haven't noted. 

QUICK MORSE FACTS FOR USERS: 

dot 1 unit                    '.'
dash 3 units                  '-'
intra-character space 1 unit  'na' //built into dot & dash
inter-character space 3 units ' '
word space 7 units            '|'
UNIT: ___ms - NOTE, THIS VALUE ABSOLUTELY MUST MATCH ACROSS BOTH
                    UNITS OR IT WILL NOT WORK. 

****************************************************************
DIAGRAM - ARDUINO NANO 
****************************************************************
                     ┌───┐ ┌─────┐(SENDING LED)                  
                     │[R]├─┤[LED]├───────────────┐       
                     └─▲─┘ └─────┘(RECEIVING LED)│       
                       │  ┌───┐ ┌─────┐          │       
                       │  │[R]├─┤[LED]├──────────┤       
┌──────┌─────┐──────┐  │  └─▲─┘ └─────┘          │       
│[D13] │{USB}│ [D12]┼──┘    │                    │       
│[3v3] └─────┘ [D11]┼───────┘                    │       
│[REF] ARDUINO [D10]│  ┌───────────SIGNAL IN     │       
│[A00]  -NANO  [D09]│  │┌────────────►SIGNAL OUT │       
│[A01] AT-mega [D08]│  ││  ┌────────┐            │       
│[A02]  328P   [D07]│  ││┌─►[SWITCH]├────────────┤       
│[A03]  MICRO  [D06]│  │││ └────────┘            │       
│[A04]  CNTRL  [D05]│◄─┘││    ┌───┐ ┌─────┐      │       
│[A05]         [D04]┼───┘│    │[R]├─┤[LED]├──────┤       
│[A06]  {RST}  [D03]┼────┘    └─▲─┘ └─────┘      │       
│[A07]   BTN   [D02]┼───────────┘ (Signal LED)   │       
│[5 V]         [GND]│◄───────────────────────────┘       
│[RST] P P R T [RST]│                                    
│[GND] 1 W X X [RXD]│                                    
│[VIN] 3 R     [TXD]│                                    
└───────────────────┘                                    
                                                         
 PIN LAYOUT:                                             
  D12 - SENDING MODE INDICATOR                           
  D11 - RECEIVING MODE INDICATOR                         
  D05 - SIGNAL IN PIN  (COMING FROM OTHER UNIT)          
  D04 - SIGNAL OUT PIN (GOING TO OTHER UNIT)             
  D03 - MODE SWITCH                                      
  D02 - SIGNAL INDICATOR - FLASHES TO SENT/RECIEVED MORSE
