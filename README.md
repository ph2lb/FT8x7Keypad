# FT8x7Keypad
A Arduino based keypad for the Yaesu FT8x7 series.

This is just the prove of concept and there is still a lot of things to do.

Tested it on my Yaesu FT897D with a Arduino R3 Uno and 4x4 keypad.
FT897D is configured to use CAT at 9600 baud and FT857D library has SSoftwareSerial configured on 12 (RX) and 11 (TX).
3 leds on A0..A2 to indicate current mode (Direct frequency, mode select & band select).
Keypad is configured rows 2..5 and column 6..9.

Function implemented : 

- Direct frequency entry (in Khz) (button 1..0 = freq, * = clear, # = enter, A = VFO A/B, B = Split toogle)
- Mode switching (button 1 = USB, 2 = LSB, 3 = AM, 4 = CW, 5 = CWR, 6 = DIG, 7 = FM, 8 = FMN, 9 = PKT)
- Band switching (button 1 = 160m, 2 = 80m, 3 = 60m, A = 40m, 4 = 30m, 5 = 20m, 6 = 17m, B = 15m, 7 = 12m, 8 = 10m, 9 = 6m, C = 2m, 0 = 70cm)
- Button D = switching between layouts (directfrequency entry, mode switching, band switching).


Some credits to James Buck, VE3BUX.

FT857D.cpp and FT857D.h are taken from the : 

Arduino library for controlling a Yaesu FT857D radio via CAT commands.
 
 Version:  0.1
 Created:  2012.08.16
Released:  2012.08.17
  Author:  James Buck, VE3BUX
     Web:  http://www.ve3bux.com
   
    
     
Todo : 
- Add  CAT pass through
- Aditional keypad support (2 x 4x4)
- and lots more.


