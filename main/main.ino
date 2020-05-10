// -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil; mode: c++ -*-
/*
  USB-snes-to-digital
  https://github.com/doj/USB-snes-to-digital

  The following Arduino Uno pins are used:
  pin 9-13 used by the USB host shield (library)

  pin 2 - up
  pin 3 - down
  pin 4 - left
  pin 5 - right
  pin 6 - fire
  pin 7 - fire 2
  pin 8 - fire 3
  you can change the pin assignment with the pin* variables below

  Features:
  - [ ] use digital x-pad for joystick directions.
  - [ ] use B button for fire
  - [ ] use X button for up direction
  - [ ] use L button for left
  - [ ] use R button for right
  - [ ] use A button for auto fire with 5 Hz
  - [ ] use Y button for auto fire with 10 Hz
  - [ ] use start button for fire 2 on POT X
  - [ ] use select button for fire 3 on POT Y
  - [ ] use analog joystick of a PlayStation 3 controller for POT X and POT Y

  /Applications/Arduino.app/Contents/Java/hardware/arduino/avr/cores/arduino/Arduino.h
*/

#define USE_LCD 1
#define USE_SERIAL 0

#include <usbhid.h>
#include <hiduniversal.h>
#include <usbhub.h>
#include "innext_snes.h"

void debug(const char *str, int8_t line = 0);

#if USE_SERIAL
#include <SPI.h>
#endif

#if USE_LCD
#include <LiquidCrystal.h>
LiquidCrystal lcd(/*RS=*/2, /*E=*/3, /*D4..D7*/4,5,6,7);

#else
#define pinUp    2
#define pinDown  3
#define pinLeft  4
#define pinRight 5
#define pinFire  6
#define pinFire2 7
#define pinFire3 8
#endif

USB Usb;
USBHub Hub(&Usb);
HIDUniversal Hid(&Usb);

void debug(const char *str, int8_t line)
{
#if USE_SERIAL
  Serial.println(str);
#endif
#if USE_LCD
  if (line >= 0)
  {
    lcd.setCursor(0,line);
  }
  lcd.print(str);
#endif
}

const char *hexdigit = "0123456789abcdef";

void debug(const uint8_t v)
{
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
}

////////////////////////////////////////////////////////////////////

class iNNEXT : public iNNEXTevents
{
  int8_t m_x = 0x7f;
  int8_t m_y = 0x7f;
  void debugAxes() const;
public:
  void OnX(int8_t x) override;
  void OnY(int8_t y) override;
  void OnButtonUp(uint8_t but_id) override;
  void OnButtonDn(uint8_t but_id) override;
};

void
iNNEXT::debugAxes() const
{
  debug("x ", 0);debug(m_x);
  debug(" y ",-1);debug(m_y);
}

void
iNNEXT::OnX(int8_t x)
{
  m_x = x;
  debugAxes();
}

void
iNNEXT::OnY(int8_t y)
{
  m_y = y;
  debugAxes();
}

void
iNNEXT::OnButtonUp(uint8_t but_id)
{
  debug("Up: ",1);
  debug(but_id);
}

void
iNNEXT::OnButtonDn(uint8_t but_id)
{
  debug("Dn: ",1);
  debug(but_id);
}

iNNEXT innext;
iNNEXTparser innext_parser(&innext);

////////////////////////////////////////////////////////////////////

// the setup function runs once when you press reset or power the board
void setup()
{
#if USE_SERIAL
  Serial.begin(115200);
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  Serial.println("Start");
#endif

#if USE_LCD
  lcd.begin(/*columns=*/16, /*rows=*/2);
#else
  pinMode(pinUp, OUTPUT);
  pinMode(pinDown, OUTPUT);
  pinMode(pinLeft, OUTPUT);
  pinMode(pinRight, OUTPUT);
  pinMode(pinFire, OUTPUT);
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
char blink = LOW;
char blinkState = 0;
char togglePin = 0;
char autofire = 1;
char dirState = 0;
void loop()
{
  Usb.Task();

#if 0
  // blink LED on pin 13
  if (++blinkState > 10)
  {
    digitalWrite(LED_BUILTIN, blink);
    blink = ! blink;
    blinkState = 0;

#if USE_LCD
    lcd.setCursor(0,1);
    lcd.print(blink ? "on " : "off");
#endif
  }

#if !USE_LCD
  if (++dirState > 10)
  {
    dirState = 0;
    digitalWrite(togglePin, LOW);
    if (++togglePin >= pinFire)
    {
      togglePin = pinUp;
    }
    digitalWrite(togglePin, HIGH);
  }

  if (autofire == 1)
  {
    digitalWrite(pinFire, HIGH);
    autofire = 2;
  }
  else if (autofire == 2)
  {
    digitalWrite(pinFire, LOW);
    autofire = 1;
  }
#endif

  delay(100);
#endif

}
