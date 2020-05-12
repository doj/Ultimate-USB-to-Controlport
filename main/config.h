#pragma once

/*
 * pin 9-13 used by the USB host shield.
 * Don't use these for joystick 1 or 2.
 */

/// define to 1 if you want to use a serial output for debuggin on digital pins 0 and (?)
#define USE_SERIAL 1
#if USE_SERIAL
#define SERIAL_BAUD 115200
#endif

/// define to 1 if you want to use a LCD with the Hitachi HD44870
#define USE_LCD 1
#if USE_LCD
/// set the Arduino pin for the LCD R/S pin
#define LCD_RS PIN_D2
/// set the Arduino pin for the LCD E pin
#define LCD_E  PIN_D3
/// set the Arduino pin for the LCD D4 pin
#define LCD_D4 PIN_D4
/// set the Arduino pin for the LCD D5 pin
#define LCD_D5 PIN_D5
/// set the Arduino pin for the LCD D6 pin
#define LCD_D6 PIN_D6
/// set the Arduino pin for the LCD D7 pin
#define LCD_D7 PIN_D7
/// set the number of columns on the LCD
#define LCD_COLS 16
/// set the number of rows on the LCD
#define LCD_ROWS 2
#endif

///@name Joystick 1 configuration
///      to disable joystick one, remove these defines.
///@{
/// set the Arduino pin for Joystick 1 Up on DB9 pin 1
#define JOY1_UP    PIN_A0
/// set the Arduino pin for Joystick 1 Down on DB9 pin 2
#define JOY1_DOWN  PIN_A1
/// set the Arduino pin for Joystick 1 Left on DB9 pin 3
#define JOY1_LEFT  PIN_A2
/// set the Arduino pin for Joystick 1 Right on DB9 pin 4
#define JOY1_RIGHT PIN_A3
/// set the Arduino pin for Joystick 1 Fire on DB9 pin 6
#define JOY1_FIRE  PIN_A5
/// set the Arduino pin for Pot X 1 on DB9 pin 9
#define JOY1_POTX  PIN_D8
/// set the Arduino pin for Pot Y 1 on DB9 pin 5
#define JOY1_POTY  PIN_A4
///@}


#if 0
///@name Joystick 2 configuration
///      to disable joystick one, remove these defines.
///@{
/// set the Arduino pin for Joystick 2 Up on DB9 pin 1
#define JOY2_UP    PIN_D3
/// set the Arduino pin for Joystick 2 Down on DB9 pin 2
#define JOY2_DOWN  PIN_D4
/// set the Arduino pin for Joystick 2 Left on DB9 pin 3
#define JOY2_LEFT  PIN_D5
/// set the Arduino pin for Joystick 2 Right on DB9 pin 4
#define JOY2_RIGHT PIN_D6
/// set the Arduino pin for Joystick 2 Fire on DB9 pin 6
#define JOY2_FIRE  PIN_D7
/// set the Arduino pin for Pot X 2 on DB9 pin 9
#define JOY2_POTX  PIN_D0
/// set the Arduino pin for Pot Y 2 on DB9 pin 5
#define JOY2_POTY  PIN_D1
///@}
#else
#define JOY2_UP    JOY1_UP
#define JOY2_DOWN  JOY1_DOWN
#define JOY2_LEFT  JOY1_LEFT
#define JOY2_RIGHT JOY1_RIGHT
#define JOY2_FIRE  JOY1_FIRE
#define JOY2_POTX  JOY1_POTX
#define JOY2_POTY  JOY1_POTY
#endif

/// set the sensitivity of an axis event.
/// small values make the axis trigger early.
/// large values make the axis trigger late.
#define AXIS_SENSITIVITY 64
/// set the integer value for the center position of X/Y USB joystick axis.
#define AXIS_CENTER 0x7f

/// frequency of auto fire on button A
static uint8_t AUTO_FIRE_A_FREQ = 5;
/// frequency of auto fire on button Y
static uint8_t AUTO_FIRE_Y_FREQ = 3;
