
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
 * Version  : 0.2 (DRAFT)
 * ------------------------------------------------------------------------
 * Description : A Arduino based keypad for the Yaesu FT8x7 series
 * ------------------------------------------------------------------------
 * Revision : 
 *  - 2018-okt-31 0.1 initial version
 *  - 2018-nov-4  0.2 add freq table (mode by freq range)
 * ------------------------------------------------------------------------
 * Hardware used : 
 *  - Arduino Uno R3 (for prototype)
 *  - 4x4 keypad  https://www.adafruit.com/product/3844
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


byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {9, 8, 7, 6}; //connect to the column pinouts of the keypad
 

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


typedef enum { 
  MODEUSB = 0,
  MODELSB = 1,
  MODEAM = 2,
  MODECW = 3,
  MODECWR = 4,
  MODEPKT = 5,
  MODEFM = 6,
  MODEFMN = 7,
  MODEDIG = 8
} ModeEnum;

const char* modeUSB = "USB";
const char* modeLSB = "LSB";
const char* modeAM = "AM";
const char* modeCW = "CW";
const char* modeCWR = "CWR";
const char* modePKT = "PKT";
const char* modeFM = "FM";
const char* modeFMN = "FMN";
const char* modeDIG = "DIG";


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

/*
** Region Mode stuff
*/



char *ModeEnumToString(ModeEnum mode)
{
  switch(mode)
  {
    case MODEUSB : return modeUSB;
    case MODELSB : return modeLSB;
    case MODEAM : return modeAM;
    case MODECW : return modeCW;
    case MODECWR : return modeCWR;
    case MODEPKT : return modePKT;
    case MODEFM : return modeFM;
    case MODEFMN : return modeFMN;
    case MODEDIG : return modeDIG;
  }
  return modeUSB;
}

// note : USB = on position 0 but on keypad on position 1 (button 0 isnt used)
const char *modes[] =
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


/*
** Region Band stuff
*/
// define the bandstruct
typedef struct 
{
  long Freq; // the current frequency on that band (set with default)
  ModeEnum Mode;
} 
BandStruct;

// note : freq in 10Hz steps so 7125.250 Khz = 712525
long bands[] =
{
  
  43200000,  // 70cm = 0
  
    184000, // 160m = 1
    365000, // 80m = 2
    535150, // 60m = 3
  
   1010000, // 30m = 4
   1417500, // 20m = 5
   1806800, // 17m = 6
   2489000, // 12m = 7
   2822500, // 10m = 8
   5010000, // 6m  = 9
  
    710000, // 40m = A
   2122500, // 15m = B
  14500000, //  2m = C
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
      break;    
    case '*' :
    case '#' :
      // unused
      return;
    default : 
      bandIndex = (int)keyPressed - (int)'0';
      break;
  }
  radio.setMode(GetModeStringForFreq(bands[bandIndex]));
  delay(100);
  radio.setFreq(bands[bandIndex]);
       
}


/*
** Region direct frequency
*/
typedef struct 
{
  long Freq; // the current frequency on that band (set with default)
  ModeEnum Mode;
} 
FreqStruct;

FreqStruct freqs[] =
{
// frequencies in 10hz eg 47200 = 472Khz
  { 0, MODEAM },
  { 13570, MODECW },
  { 13780, MODEAM },
  { 47200, MODECW  },
  { 47500, MODEDIG },
  { 47900, MODEAM },

// 160m
  { 181000, MODECW  },
  { 183800, MODEDIG  },
  { 184000, MODEDIG  },
  { 184300, MODELSB },
  { 200000, MODEAM },

// 80m
  { 350000, MODECW },
  { 357000, MODEDIG  },
  { 360000, MODELSB  },
  { 380000, MODEAM },
  
// 60m
  { 535150, MODECW },
  { 535400, MODEUSB },
  { 536600, MODEDIG },
  { 536650, MODEUSB },
  { 550000, MODEAM },

// 40m
  { 700000, MODECW  },
  { 704000, MODEDIG  },
  { 705300, MODELSB  },
  { 720000, MODEAM },

// 30m
  { 1010000, MODECW  },
  { 1013000, MODEDIG },
  { 1015000, MODEAM },

// 20m
  { 1400000, MODECW  },
  { 1407000, MODEDIG },
  { 1410100, MODEUSB },
  { 1435000, MODEAM },

// 17m
  { 1806800, MODECW },
  { 1809500, MODEDIG  },
  { 1811100, MODEUSB },
  { 1816800, MODEAM },

// 15m
  { 2100000, MODECW },
  { 2107000, MODEDIG },
  { 2115100, MODEUSB },
  { 2145000, MODEAM  },

// 12m
  { 2489000, MODECW  },
  { 2491500, MODEDIG },
  { 2494000, MODEUSB },
  { 2499000, MODEAM },

// 10m
  { 2800000, MODECW },
  { 2807000, MODEDIG },
  { 2832000, MODEUSB },
  { 2900000, MODEFM  },

// 6m
  { 5000000, MODECW },
  { 5010000, MODEUSB },
  { 5030000, MODEDIG },
  { 5050000, MODEFM  },

// Airband
  { 10800000, MODEAM },

// 2m
  { 14400000, MODEUSB },
  { 14402500, MODECW },
  { 14415000, MODEUSB },
  { 14440000, MODECW },
  { 14450000, MODEFM },
  { 14600000, MODEFM },

// 70cm
  { 43000000, MODEFM },
  { 43200000, MODECW },
  { 43210000, MODEUSB },
  { 43240000, MODECW },
  { 43250000, MODEFM },
  { 44000000, MODEFM },
  { -1, MODEFM}
};

char * GetModeStringForFreq(long freq)
{
  int i = 0; 
  while(freqs[i].Freq != -1 && freq > freqs[i].Freq)
  {
     i++;
  }  
  if (freq < freqs[i].Freq)
    i--; 
  return ModeEnumToString(freqs[i].Mode);
}


void funcDirectFreq(char keyPressed)
{
  switch(keyPressed)
  {
    case '*' : 
      frequency = 0; 
       break;
    case '#' : 
      radio.setMode(GetModeStringForFreq(frequency*100));
      delay(100);
      radio.setFreq(frequency*100);
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


/*
** Region Main loop
*/
void loop()
{
  char keyPressed = keypad.getKey();
  
  if (keyPressed) 
  {
    if (keyPressed == 'D')
    {
        if (func < FUNCMAX) 
          func = (FuncEnum) func + 1;        
        else 
          func = FUNCMIN;
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
