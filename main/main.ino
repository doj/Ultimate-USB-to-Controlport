// -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil; mode: c++ -*-
/*
  Ultimate USB to Controlport
  https://github.com/doj/Ultimate-USB-to-Controlport

  /Applications/Arduino.app/Contents/Java/hardware/arduino/avr/cores/arduino/Arduino.h
*/

#include <usbhid.h>
#include <hiduniversal.h>
#include <usbhub.h>
#include "ControlPortDevice.h"
#include "debug.h"
#include "timer.h"

/// set an Arduino pin to LOW or HIGH for a Commodore control port digital input.
/// the Commodore control port pin is either GND (state==LOW) or high-z (state==HIGH).
/// @param pin Arduino pin
/// @param state LOW or HIGH. use LOW if the corresponding button or direction is pressed.
/// \sa https://www.arduino.cc/en/Tutorial/DigitalPins
void
joystick(const uint8_t pin, const uint8_t state)
{
  if (state == LOW)
    {
      digitalWrite(pin, LOW); // GND
      pinMode(pin, OUTPUT);
    }
  else
    {
      // configure the pin for input. This will make it high impedance (high-z).
      pinMode(pin, INPUT);
      digitalWrite(pin, LOW);
    }
}

Timer_t timer;

static USB Usb;
static USBHub UsbHub(&Usb);
#define X(n) \
static HIDUniversal uni ## n (&Usb); \
static ControlPortDevice cpd ## n (n);
// support up to 6 devices: 2*(joystick+keyboard+mouse)
X(1);
X(2);
X(3);
X(4);
X(5);
X(6);
#undef X

// the setup function runs once upon startup
void setup()
{
#if USE_SERIAL
  Serial.begin(SERIAL_BAUD);
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  Serial.println("\nStart");
#endif

  if (Usb.Init() == -1)
    {
      debug("!USBinit");
    }

  delay(200);

#define X(n) \
  if (! uni ## n.SetReportParser(n, & cpd ## n)) { \
    debug("!" #n); \
  }
  X(1);
  X(2);
  X(3);
  X(4);
  X(5);
  X(6);
#undef X
}

// the loop function runs over and over again forever
void loop()
{
  Usb.Task();
  timer.tick();
}
