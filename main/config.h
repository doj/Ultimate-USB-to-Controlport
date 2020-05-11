#pragma once

/// define to 1 if you want to use a serial output for debuggin on digital pins 0 and (?)
#define USE_SERIAL 0

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

/// set the Arduino pin for Joystick Up on DB9 pin 1
#define pinUp    PIN_A0
/// set the Arduino pin for Joystick Down on DB9 pin 2
#define pinDown  PIN_A1
/// set the Arduino pin for Joystick Left on DB9 pin 3
#define pinLeft  PIN_A2
/// set the Arduino pin for Joystick Right on DB9 pin 4
#define pinRight PIN_A3
/// set the Arduino pin for Joystick Fire on DB9 pin 6
#define pinFire  PIN_A5
/// set the Arduino pin for Pot X on DB9 pin 9
#define pinPotX  PIN_D0
/// set the Arduino pin for Pot Y on DB9 pin 5
#define pinPotY  PIN_A4
/// set the Arduino pin for Fire 2
#define pinFire2 pinPotY
/// set the Arduino pin for Fire 3
#define pinFire3 pinPotX

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
