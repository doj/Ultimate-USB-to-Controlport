// -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil; mode: c++ -*-
#pragma once
#include <stdint.h>
#include "pins.h"

/// define to 1 if you want to use a serial output for debugging on PIN_D0 and PIN_D1.
/// You can't use those pins for control port pins.
#define USE_SERIAL 0
#if USE_SERIAL
#define SERIAL_BAUD 115200
#include <SPI.h>
#endif

/*
 * pin 9-13 are used by the USB host shield.
 * Don't use these for Control Port 1 or 2.
 */

///@name Control Port 1 configuration
///@{
/// set the Arduino pin for Joystick 1 Up on DB9 pin 1
#define CP1_UP    PIN_A0
/// set the Arduino pin for Joystick 1 Down on DB9 pin 2
#define CP1_DOWN  PIN_A1
/// set the Arduino pin for Joystick 1 Left on DB9 pin 3
#define CP1_LEFT  PIN_A2
/// set the Arduino pin for Joystick 1 Right on DB9 pin 4
#define CP1_RIGHT PIN_A3
/// set the Arduino pin for Joystick 1 Fire on DB9 pin 6
#define CP1_FIRE  PIN_A4
/// set the Arduino pin for Pot X 1 on DB9 pin 9, should be PIN_D2 or PIN_D3 if you plan to use the mouse.
#define CP1_POTX  PIN_D2
/// set the Arduino pin for Pot Y 1 on DB9 pin 5
#define CP1_POTY  PIN_A5
///@}


///@name Control Port 2 configuration
///@{
/// set the Arduino pin for Joystick 2 Up on DB9 pin 1
#define CP2_UP    PIN_D4
/// set the Arduino pin for Joystick 2 Down on DB9 pin 2
#define CP2_DOWN  PIN_D5
/// set the Arduino pin for Joystick 2 Left on DB9 pin 3
#define CP2_LEFT  PIN_D6
/// set the Arduino pin for Joystick 2 Right on DB9 pin 4
#define CP2_RIGHT PIN_D7
/// set the Arduino pin for Joystick 2 Fire on DB9 pin 6
#define CP2_FIRE  PIN_D8
/// set the Arduino pin for Pot X 2 on DB9 pin 9, should be PIN_D2 or PIN_D3 if you plan to use the mouse.
#define CP2_POTX  PIN_D3
/// set the Arduino pin for Pot Y 2 on DB9 pin 5
#define CP2_POTY  PIN_D1
///@}

/// set the sensitivity of an axis event.
/// small values make the axis trigger early.
/// large values make the axis trigger late.
#define AXIS_SENSITIVITY 64
/// set the integer value for the center position of X/Y USB joystick axis.
#define AXIS_CENTER 0x7f

/// frequency in Hz of auto fire on button A
#define AUTO_FIRE_A_FREQ 10
/// frequency in Hz of auto fire on button Y
#define AUTO_FIRE_Y_FREQ 5
