// -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil; mode: c++ -*-
/*
  USB-snes-to-digital
  https://github.com/doj/USB-snes-to-digital

  The following Arduino Uno pins are used:
  pin 9-13 used by the USB host shield (library)

  use the config.h file to change pin assignments.

  Features:
  - [X] use digital x-pad for joystick directions.
  - [X] use B button for fire
  - [X] use X button for up direction
  - [X] use L button for left
  - [X] use R button for right
  - [X] use start button for fire 2 on POT X
  - [X] use select button for fire 3 on POT Y
  - [ ] use A button for auto fire with 5 Hz
  - [ ] use Y button for auto fire with 10 Hz
  - [ ] use analog joystick of a PlayStation 3 controller for POT X and POT Y
  - [ ] use USB mouse for POT X and POT Y with 1351 protocol

  /Applications/Arduino.app/Contents/Java/hardware/arduino/avr/cores/arduino/Arduino.h
*/

#include <usbhid.h>
#include <hiduniversal.h>
#include <usbhub.h>
#include "innext_snes.h"
#include "pins.h"
#include "config.h"

void debug(const char *str, int8_t line = 0);

#if USE_SERIAL
#include <SPI.h>
#endif

#if USE_LCD
#include <LiquidCrystal.h>
LiquidCrystal lcd(LCD_RS,LCD_E,LCD_D4,LCD_D5,LCD_D6,LCD_D7);
#endif

#if USE_LCD || USE_SERIAL
#define DO_DEBUG 1
#else
#define DO_DEBUG 0
#endif

USB Usb;
USBHub Hub(&Usb);
HIDUniversal Hid(&Usb);

void debug(const char *str, int8_t line)
{
#if USE_SERIAL
  if (line >= 0)
    {
      Serial.print("\n");
    }
  Serial.print(str);
#endif
#if USE_LCD
  if (line >= 0)
  {
    lcd.setCursor(0,line);
  }
  lcd.print(str);
#endif
}

void debug(const uint8_t v)
{
#if DO_DEBUG
  static const char *hexdigit = "0123456789abcdef";
  char buf[3];
  buf[0] = hexdigit[v >> 4];
  buf[1] = hexdigit[v & 15];
  buf[2] = 0;
#if USE_SERIAL
  Serial.print(buf);
#endif
#if USE_LCD
  lcd.print(buf);
#endif
#endif
}

////////////////////////////////////////////////////////////////////

class iNNEXT : public iNNEXTevents
{
#if DO_DEBUG
  int8_t m_x = 0x7f;
  int8_t m_y = 0x7f;
  void debugAxes() const;
#endif
public:
  void OnX(uint8_t x) override;
  void OnY(uint8_t y) override;
  void OnButtonUp(uint8_t but_id) override;
  void OnButtonDn(uint8_t but_id) override;
};

#if DO_DEBUG
void
iNNEXT::debugAxes() const
{
  debug("x ", 0);debug(m_x);
  debug(" y ",-1);debug(m_y);
}
#endif

void
iNNEXT::OnX(uint8_t x)
{
  if (x < AXIS_CENTER - AXIS_SENSITIVITY)
    {
      digitalWrite(pinLeft,   LOW);
      digitalWrite(pinRight, HIGH);
    }
  else if (x > AXIS_CENTER + AXIS_SENSITIVITY)
    {
      digitalWrite(pinLeft,  HIGH);
      digitalWrite(pinRight, LOW);
    }
  else
    {
      digitalWrite(pinLeft,  HIGH);
      digitalWrite(pinRight, HIGH);
    }
#if DO_DEBUG
  m_x = x;
  debugAxes();
#endif
}

void
iNNEXT::OnY(uint8_t y)
{
  if (y < AXIS_CENTER - AXIS_SENSITIVITY)
    {
      digitalWrite(pinUp,   LOW);
      digitalWrite(pinDown, HIGH);
    }
  else if (y > AXIS_CENTER + AXIS_SENSITIVITY)
    {
      digitalWrite(pinUp,  HIGH);
      digitalWrite(pinDown, LOW);
    }
  else
    {
      digitalWrite(pinUp,   HIGH);
      digitalWrite(pinDown, HIGH);
    }
#if DO_DEBUG
  m_y = y;
  debugAxes();
#endif
}

void
iNNEXT::OnButtonUp(uint8_t but_id)
{
  if (but_id == BUT_B)
    {
      digitalWrite(pinFire, HIGH);
    }
  else if (but_id == BUT_SELECT)
    {
      digitalWrite(pinFire2, HIGH);
    }
  else if (but_id == BUT_START)
    {
      digitalWrite(pinFire3, HIGH);
    }
  else if (but_id == BUT_X)
    {
      digitalWrite(pinUp, HIGH);
    }
  else if (but_id == BUT_L)
    {
      digitalWrite(pinLeft, HIGH);
    }
  else if (but_id == BUT_R)
    {
      digitalWrite(pinRight, HIGH);
    }
#if DO_DEBUG
  debug("Up: ",1);
  debug(but_id);
#endif
}

void
iNNEXT::OnButtonDn(uint8_t but_id)
{
  if (but_id == BUT_B)
    {
      digitalWrite(pinFire, LOW);
    }
  else if (but_id == BUT_SELECT)
    {
      digitalWrite(pinFire2, LOW);
    }
  else if (but_id == BUT_START)
    {
      digitalWrite(pinFire3, LOW);
    }
  else if (but_id == BUT_X)
    {
      digitalWrite(pinUp, LOW);
    }
  else if (but_id == BUT_L)
    {
      digitalWrite(pinLeft, LOW);
    }
  else if (but_id == BUT_R)
    {
      digitalWrite(pinRight, LOW);
    }
#if DO_DEBUG
  debug("Dn: ",1);
  debug(but_id);
#endif
}

iNNEXT innext;
iNNEXTparser innext_parser(&innext);

////////////////////////////////////////////////////////////////////

// the setup function runs once when you press reset or power the board
void setup()
{
  pinMode(pinUp,    OUTPUT|INPUT_PULLUP);
  pinMode(pinDown,  OUTPUT|INPUT_PULLUP);
  pinMode(pinLeft,  OUTPUT|INPUT_PULLUP);
  pinMode(pinRight, OUTPUT|INPUT_PULLUP);
  pinMode(pinFire,  OUTPUT|INPUT_PULLUP);
  pinMode(pinFire2, OUTPUT|INPUT_PULLUP);
  pinMode(pinFire3, OUTPUT|INPUT_PULLUP);

#if USE_SERIAL
  Serial.begin(115200);
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  Serial.println("Start");
#endif

#if USE_LCD
  lcd.begin(LCD_COLS, LCD_ROWS);
#endif

  if (Usb.Init() == -1)
  {
    debug("! USB init");
  }

  delay(200);

  if (!Hid.SetReportParser(0, &innext_parser))
  {
    debug("!inext");
  }
}

// the loop function runs over and over again forever
void loop()
{
  Usb.Task();
}
