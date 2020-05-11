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
  - [X] use A button for auto fire with 5 Hz
  - [X] use Y button for auto fire with 3 Hz
  - [X] configure auto fire frequency
  - [ ] use start button for fire 2 on POT Y
  - [ ] use select button for fire 3 on POT X
  - [ ] use analog joystick of a PlayStation 3 controller for POT X and POT Y
  - [ ] use USB mouse for POT X and POT Y with 1351 protocol
  - [ ] connect to both control ports and switch joystick by magic button press
  - [ ] connect to both control ports and create two joysticks
  - [ ] support two USB joysticks with both control ports
  - [ ] reconfigure any USB button to any Commodore button
  - [ ] save configuration to Arduino EEPROM https://www.arduino.cc/en/Reference/EEPROM

  /Applications/Arduino.app/Contents/Java/hardware/arduino/avr/cores/arduino/Arduino.h
*/

#include <usbhid.h>
#include <hiduniversal.h>
#include <usbhub.h>
#include "innext_snes.h"
#include "pins.h"
#include "config.h"
#include "timer.h"

void debug(const char *str, int8_t line = 0);

#if USE_SERIAL
#include <SPI.h>
#endif

#if USE_LCD
#include <LiquidCrystal.h>
static LiquidCrystal lcd(LCD_RS,LCD_E,LCD_D4,LCD_D5,LCD_D6,LCD_D7);
#endif

#if USE_LCD || USE_SERIAL
#define DO_DEBUG 1
#else
#define DO_DEBUG 0
#endif

static USB Usb;
static USBHub Hub(&Usb);
static HIDUniversal Hid(&Usb);

////////////////////////////////////////////////////////////////////

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

/// set an Arduino pin to LOW or HIGH.
/// do it safely to interface with a Commodore control port digital input.
/// @param pin Arduino pin
/// @param state LOW or HIGH
/// \sa https://www.arduino.cc/en/Tutorial/DigitalPins
static void
joystick(const uint8_t pin, const uint8_t state)
{
  if (state == LOW)
    {
      digitalWrite(pin, LOW);
      pinMode(pin, OUTPUT);
    }
  else
    {
      // configure the pin for input. This will make it high impedance (high-z).
      pinMode(pin, INPUT);
      digitalWrite(pin, LOW);
    }
}

//// auto fire feature
typedef Timer<2> Timer_t;
static Timer_t timer;
static Timer_t::Task autoFireJoy1Task = 0;
static uint8_t autoFireState = 0;
static bool
autoFireCB(void *arg)
{
  uint8_t *state = (uint8_t*)arg;
  *state = ! *state;
  joystick(pinFire, *state);
  return true;
}

////////////////////////////////////////////////////////////////////

#define IDX2BIT(idx) (1 << idx)

class iNNEXT : public iNNEXTevents
{
#if DO_DEBUG
  int8_t m_x = 0x7f;
  int8_t m_y = 0x7f;
  void debugAxes() const;
#endif
  /// bit mask of button states.
  /// bit off: button not pressed.
  /// bit on:  button pressedn.
  uint16_t m_button_state = 0;
public:
  void OnX(uint8_t x) override;
  void OnY(uint8_t y) override;
  void OnButtonUp(uint8_t but_id) override;
  void OnButtonDn(uint8_t but_id) override;
  bool isAutoFireAConfig() const
  {
    static const uint16_t mask = IDX2BIT(BUT_SELECT) | IDX2BIT(BUT_START) | IDX2BIT(BUT_A);
    return (m_button_state & mask) == mask;
  }
  bool isAutoFireYConfig() const
  {
    static const uint16_t mask = IDX2BIT(BUT_SELECT) | IDX2BIT(BUT_START) | IDX2BIT(BUT_Y);
    return (m_button_state & mask) == mask;
  }
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
      joystick(pinLeft,   LOW);
      joystick(pinRight, HIGH);
    }
  else if (x > AXIS_CENTER + AXIS_SENSITIVITY)
    {
      joystick(pinLeft,  HIGH);
      joystick(pinRight, LOW);
    }
  else
    {
      joystick(pinLeft,  HIGH);
      joystick(pinRight, HIGH);
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
      joystick(pinUp,   LOW);
      joystick(pinDown, HIGH);

      if (isAutoFireAConfig() && AUTO_FIRE_A_FREQ < 255)
      {
        ++AUTO_FIRE_A_FREQ;
        debug("auto fire A "); debug(AUTO_FIRE_A_FREQ);
      }
      if (isAutoFireYConfig() && AUTO_FIRE_Y_FREQ < 255)
      {
        ++AUTO_FIRE_Y_FREQ;
        debug("auto fire Y "); debug(AUTO_FIRE_Y_FREQ);
      }
    }
  else if (y > AXIS_CENTER + AXIS_SENSITIVITY)
    {
      joystick(pinUp,  HIGH);
      joystick(pinDown, LOW);

      if (isAutoFireAConfig() && AUTO_FIRE_A_FREQ > 1)
      {
        --AUTO_FIRE_A_FREQ;
       debug("auto fire A "); debug(AUTO_FIRE_A_FREQ);
      }
      if (isAutoFireYConfig() && AUTO_FIRE_Y_FREQ > 1)
      {
        --AUTO_FIRE_Y_FREQ;
        debug("auto fire Y "); debug(AUTO_FIRE_Y_FREQ);
      }
    }
  else
    {
      joystick(pinUp,   HIGH);
      joystick(pinDown, HIGH);
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
      joystick(pinFire, HIGH);
    }
  else if (but_id == BUT_SELECT)
    {
      joystick(pinFire2, HIGH);
    }
  else if (but_id == BUT_START)
    {
      joystick(pinFire3, HIGH);
    }
  else if (but_id == BUT_X)
    {
      joystick(pinUp, HIGH);
    }
  else if (but_id == BUT_L)
    {
      joystick(pinLeft, HIGH);
    }
  else if (but_id == BUT_R)
    {
      joystick(pinRight, HIGH);
    }
  else if (but_id == BUT_A)
  {
    joystick(pinFire, HIGH);
     timer.cancel(autoFireJoy1Task);
  }
  else if (but_id == BUT_Y)
  {
    joystick(pinFire, HIGH);
     timer.cancel(autoFireJoy1Task);
  }

  uint16_t mask = 1 << but_id;
  m_button_state &= ~mask;

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
      joystick(pinFire, LOW);
    }
  else if (but_id == BUT_SELECT)
    {
      joystick(pinFire2, LOW);
    }
  else if (but_id == BUT_START)
    {
      joystick(pinFire3, LOW);
    }
  else if (but_id == BUT_X)
    {
      joystick(pinUp, LOW);
    }
  else if (but_id == BUT_L)
    {
      joystick(pinLeft, LOW);
    }
  else if (but_id == BUT_R)
    {
      joystick(pinRight, LOW);
    }
  else if (but_id == BUT_A)
  {
    timer.cancel(autoFireJoy1Task);
    autoFireState = LOW;
    joystick(pinFire, LOW);
    autoFireJoy1Task = timer.every(1000/2/AUTO_FIRE_A_FREQ, autoFireCB, &autoFireState);
  }
  else if (but_id == BUT_Y)
  {
    timer.cancel(autoFireJoy1Task);
    autoFireState = LOW;
    joystick(pinFire, LOW);
    autoFireJoy1Task = timer.every(1000/2/AUTO_FIRE_Y_FREQ, autoFireCB, &autoFireState);
  }

  uint16_t mask = 1 << but_id;
  m_button_state |= mask;

#if DO_DEBUG
  debug("Dn: ",1);
  debug(but_id);
#endif
}

static iNNEXT innext;
static iNNEXTparser innext_parser(&innext);

////////////////////////////////////////////////////////////////////

// the setup function runs once when you press reset or power the board
void setup()
{
  joystick(pinUp,    HIGH);
  joystick(pinDown,  HIGH);
  joystick(pinLeft,  HIGH);
  joystick(pinRight, HIGH);
  joystick(pinFire,  HIGH);
  joystick(pinFire2, HIGH);
  joystick(pinFire3, HIGH);

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
  timer.tick();
}
