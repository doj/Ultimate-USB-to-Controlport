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

Timer_t timer;

class NULLUSBReadParser : public USBReadParser
{
public:
  void Parse(const uint16_t len, const uint8_t *pbuf, const uint16_t &offset) override
  {
    (void)len;
    (void)pbuf;
    (void)offset;
  }
};

class HIDUniversalWithDump : public HIDUniversal
{
public:
  HIDUniversalWithDump(USB *usb) : HIDUniversal(usb) {}
protected:
  uint8_t OnInitSuccessful() override
  {
    uint8_t rcode;
    NULLUSBReadParser p;
    // TODO: something is happening in GetReportDescr() that makes the P4-5N controller start up correctly.
    if ((rcode = GetReportDescr(0, &p)))
      {
        debug("!GetReportDescr");
      }
    return rcode;
  }
};

static USB Usb;
static USBHub UsbHub(&Usb);
#define X(n) \
static HIDUniversalWithDump uni ## n (&Usb); \
static ControlPortDevice cpd ## n (n);
// support up to 5 devices: 2*(joystick+keyboard)+mouse
X(1);
X(2);
X(3);
X(4);
X(5);
#undef X

// the setup function runs once upon startup
void
setup()
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
#undef X
}

// the loop function runs over and over again forever
void
loop()
{
  Usb.Task();
  timer.tick();
}

// swap the control ports of all devices
void
swapControlPorts()
{
#define X(n) cpd ## n.swapPort()
  X(1);
  X(2);
  X(3);
  X(4);
  X(5);
#undef X
}
