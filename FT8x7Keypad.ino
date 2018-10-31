
/*  
 * ------------------------------------------------------------------------
 * "PH2LB LICENSE" (Revision 1) : (based on "THE BEER-WARE LICENSE" Rev 42) 
 * <lex@ph2lb.nl> wrote this file. As long as you retain this notice
 * you can do modify it as you please. It's Free for non commercial usage 
 * and education and if we meet some day, and you think this stuff is 
 * worth it, you can buy me a beer in return
 * Lex Bolkesteijn 
 * ------------------------------------------------------------------------ 
 * Filename : FT8x7Keypad.ino  
 * Version  : 0.4 (DRAFT)
 * ------------------------------------------------------------------------
 * Description : A Arduino based keypad for the Yaesu FT8x7 series
 * ------------------------------------------------------------------------
 * Revision : 
 *  - 2018-okt-31 0.1 initial version
 * ------------------------------------------------------------------------
 * Hardware used : 
 *  - Arduino Uno R3 
 *  - 4x4 keypad 
 
 * ------------------------------------------------------------------------
 * Software used : 
 *  - Keypad library 
 *  - FT8x7 cat library
 * ------------------------------------------------------------------------ 
 * TODO LIST : 
 *  - add more sourcode comment
 * ------------------------------------------------------------------------ 
 */

#include <Keypad.h>
#include <SoftwareSerial.h>
#include "FT857D.h"     // the file FT857D.h has a lot of documentation which I've added to make using the library easier

FT857D radio;           // define "radio" so that we may pass CAT commands

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// NORMAL
//byte rowPins[ROWS] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
//byte colPins[COLS] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad

byte rowPins[ROWS] = {2, 3, 4, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 7, 8, 9}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS); 

const int DirFreqLed = A0;
const int ModeLed = A1;
const int BandLed = A2;

typedef enum {
  FUNCMIN = 0,
  FUNCDIR = 0,
  FUNCMODE = 1,
  FUNCBAND = 2,
  FUNCMAX = 2
} FuncEnum;

bool split = false;
FuncEnum func = FUNCDIR;

long frequency = 0;


void setup(){
  Serial.begin(9600);
  pinMode(DirFreqLed, OUTPUT);
  pinMode(ModeLed, OUTPUT);
  pinMode(BandLed, OUTPUT);

  updateLeds(func);
  
  radio.begin(9600);    // as with Serial.begin(9600); we wish to start the software serial port
                        // so that we may control a radio via CAT commands
}




void updateLeds(FuncEnum f)
{
  digitalWrite(DirFreqLed, (f == FUNCDIR) ? HIGH : LOW);
  digitalWrite(ModeLed, (f == FUNCMODE) ? HIGH : LOW);
  digitalWrite(BandLed, (f == FUNCBAND) ? HIGH : LOW);
}


void setFrequency(long freq)
{ 
  Serial.print("setFreq = ");
  Serial.println(freq); 
  radio.setFreq(freq*100);  // set VFO frequency to xx
}

void funcDirectFreq(char keyPressed)
{
  switch(keyPressed)
  {
    case '*' : 
      frequency = 0; 
       break;
    case '#' : 
      setFrequency(frequency); 
      frequency = 0; 
      break;
    // for A-D nothing to do yet
    case 'A' : 
      radio.switchVFO();
      break;
    case 'B' : 
      split = (split == false);
      radio.split(split);
      break;
    case 'C' : break;
    case 'D' : break;
    default : 
      frequency *= 10;
      frequency += (long)keyPressed - (long)'0';
      break;
  }
}

const char* modeUSB = "USB";
const char* modeLSB = "LSB";
const char* modeAM = "AM";
const char* modeCW = "CW";
const char* modeCWR = "CWR";
const char* modePKT = "PKT";
const char* modeFM = "FM";
const char* modeFMN = "FMN";
const char* modeDIG = "DIG";

// note : USB = on position 0 but on keypad on position 1 (button 0 isnt used)
char *modes[] =
{
  modeUSB, modeLSB, modeAM,
  modeCW, modeCWR, modePKT,
  modeFM, modeFMN, modeDIG
};

void funcMode(char keyPressed)
{
  switch(keyPressed)
  {
    case 'A' : 
      radio.switchVFO();
      break;
    case 'B' : 
      split = (split == false);
      radio.split(split);
      break;
    case 'C' :
    case 'D' :
    case '0' :
    case '*' :
    case '#' :
      // unused
      break;
    default : 
      int modeIndex = (int)keyPressed - (int)'0' - 1;
      radio.setMode(modes[modeIndex]);
      break;
  }
}


// define the bandstruct
typedef struct 
{
  long Freq; // the current frequency on that band (set with default)
  char *Mode;
} 
BandStruct;

// note : freq in 10Hz steps so 7125.250 Khz = 712525
BandStruct bands[] =
{
  
  {43200000, modeFM },  // 70cm = 0
  
  {  184000, modeLSB }, // 160m = 1
  {  365000, modeLSB }, // 80m = 2
  {  535150, modeUSB }, // 60m = 3
  
  { 1010000, modeCW  }, // 30m = 4
  { 1417500, modeUSB }, // 20m = 5
  { 1806800, modeUSB }, // 17m = 6
  { 2489000, modeUSB }, // 12m = 7
  { 2822500, modeUSB }, // 10m = 8
  { 5010000, modeUSB }, // 6m  = 9
  
  {  710000, modeLSB }, // 40m = A
  { 2122500, modeUSB }, // 15m = B
  {14500000, modeFM  }, //  2m = C
};

void funcBand(char keyPressed)
{
  int bandIndex = 0; 
  switch(keyPressed)
  {
    case 'A' : 
    case 'B' : 
    case 'C' :
    case 'D' :
      bandIndex = (int)keyPressed - (int)'A' + 10;
      radio.setFreq(bands[bandIndex].Freq); 
      delay(100);
      radio.setMode(bands[bandIndex].Mode);
      break;    
    case '*' :
    case '#' :
      // unused
      break;
    default : 
      bandIndex = (int)keyPressed - (int)'0';
      radio.setFreq(bands[bandIndex].Freq); 
      delay(100);
      radio.setMode(bands[bandIndex].Mode);
      break;
  }
}

void loop()
{
  char keyPressed = keypad.getKey();
  
  if (keyPressed) 
  {
    Serial.println(keyPressed);
    if (keyPressed == 'D')
    {
        if (func < FUNCMAX) 
          func = (FuncEnum) func + 1;        
        else 
          func = FUNCMIN;
        Serial.print("Func now ");
        Serial.println(func);
        updateLeds(func);

    }
    else 
    {
      switch(func)
      {
        case FUNCDIR : 
          funcDirectFreq(keyPressed);
          break;
        case FUNCMODE : 
          funcMode(keyPressed);
          break;
        case FUNCBAND :
          funcBand(keyPressed);
         break;
      }
    }
  }
}
