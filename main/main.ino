// -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil; mode: c++ -*-
/*
  USB-snes-to-digital
  https://github.com/doj/USB-snes-to-digital

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
  - [X] use start button for fire 2 on POT Y
  - [X] use select button for fire 3 on POT X
  - [ ] support two USB joysticks with both control ports
        https://stackoverflow.com/questions/40934344/how-to-communicate-with-the-devices-behind-a-usb-hub
  - [ ] connect to both control ports and switch joystick by magic button press
  - [ ] reconfigure any USB button to any Commodore button
  - [ ] save configuration to Arduino EEPROM https://www.arduino.cc/en/Reference/EEPROM
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

/// set an Arduino pin to LOW or HIGH for a Commodore control port digital input.
/// the Commodore control port pin is either GND (state==LOW) or high-z (state==HIGH).
/// @param pin Arduino pin
/// @param state LOW or HIGH. use LOW if the corresponding button or direction is pressed.
/// \sa https://www.arduino.cc/en/Tutorial/DigitalPins
static void
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

/// set an Arduino pin to LOW or HIGH for a Commodore control port POT input.
/// the Commodore control port pin is either +5V (state==LOW) or high-z (state==HIGH).
/// @param pin Arduino pin
/// @param state LOW or HIGH. Use LOW is the button is pressed.
/// \note do not use this function to set digital joystick pins! Use joystick() for that purpose.
static void
pot(const uint8_t pin, const uint8_t state)
{
  if (state == LOW)
    {
      digitalWrite(pin, HIGH); // +5V
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
static bool
autoFireCB(void *arg)
{
  uint8_t *state = (uint8_t*)arg;
  *state ^= 0x80;
  joystick(*state & 0x7f, *state & 0x80);
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

  Timer_t::Task m_autoFireTask = 0;
  uint8_t m_autoFireState = 0;

  uint8_t m_pinUp;
  uint8_t m_pinDown;
  uint8_t m_pinLeft;
  uint8_t m_pinRight;
  uint8_t m_pinFire;
  uint8_t m_pinPotX;
  uint8_t m_pinPotY;

public:
  iNNEXT(uint8_t pinUp, uint8_t pinDown, uint8_t pinLeft, uint8_t pinRight, uint8_t pinFire, uint8_t pinPotX, uint8_t pinPotY) :
    m_pinUp(pinUp),
    m_pinDown(pinDown),
    m_pinLeft(pinLeft),
    m_pinRight(pinRight),
    m_pinFire(pinFire),
    m_pinPotX(pinPotX),
    m_pinPotY(pinPotY)
  {}
  void init() const;
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
  void cancelAutoFire();
  void startAutoFire(uint8_t freq);
};

void
iNNEXT::init() const
{
  joystick(m_pinUp,    HIGH);
  joystick(m_pinDown,  HIGH);
  joystick(m_pinLeft,  HIGH);
  joystick(m_pinRight, HIGH);
  joystick(m_pinFire,  HIGH);
  pot(m_pinPotX, HIGH);
  pot(m_pinPotX, HIGH);
}

void
iNNEXT::cancelAutoFire()
{
  joystick(m_pinFire, HIGH);
  timer.cancel(m_autoFireTask);
}

void
iNNEXT::startAutoFire(uint8_t freq)
{
  timer.cancel(m_autoFireTask);
  m_autoFireState = m_pinFire;
  joystick(m_pinFire, LOW);
  m_autoFireTask = timer.every(1000/2/freq, autoFireCB, &m_autoFireState);
}

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
      joystick(m_pinLeft,   LOW);
      joystick(m_pinRight, HIGH);
    }
  else if (x > AXIS_CENTER + AXIS_SENSITIVITY)
    {
      joystick(m_pinLeft,  HIGH);
      joystick(m_pinRight, LOW);
    }
  else
    {
      joystick(m_pinLeft,  HIGH);
      joystick(m_pinRight, HIGH);
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
      joystick(m_pinUp,   LOW);
      joystick(m_pinDown, HIGH);

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
      joystick(m_pinUp,  HIGH);
      joystick(m_pinDown, LOW);

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
      joystick(m_pinUp,   HIGH);
      joystick(m_pinDown, HIGH);
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
      joystick(m_pinFire, HIGH);
    }
  else if (but_id == BUT_X)
    {
      joystick(m_pinUp, HIGH);
    }
  else if (but_id == BUT_L)
    {
      joystick(m_pinLeft, HIGH);
    }
  else if (but_id == BUT_R)
    {
      joystick(m_pinRight, HIGH);
    }
  else if (but_id == BUT_A)
    {
      cancelAutoFire();
    }
  else if (but_id == BUT_Y)
    {
      cancelAutoFire();
    }
  else if (but_id == BUT_SELECT)
    {
      pot(m_pinPotX, HIGH);
    }
  else if (but_id == BUT_START)
    {
      pot(m_pinPotY, HIGH);
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
      joystick(m_pinFire, LOW);
    }
  else if (but_id == BUT_X)
    {
      joystick(m_pinUp, LOW);
    }
  else if (but_id == BUT_L)
    {
      joystick(m_pinLeft, LOW);
    }
  else if (but_id == BUT_R)
    {
      joystick(m_pinRight, LOW);
    }
  else if (but_id == BUT_A)
    {
      startAutoFire(AUTO_FIRE_A_FREQ);
    }
  else if (but_id == BUT_Y)
    {
      startAutoFire(AUTO_FIRE_Y_FREQ);
    }
  else if (but_id == BUT_SELECT)
    {
      pot(m_pinPotX, LOW);
    }
  else if (but_id == BUT_START)
    {
      pot(m_pinPotY, LOW);
    }

  uint16_t mask = 1 << but_id;
  m_button_state |= mask;

#if DO_DEBUG
  debug("Dn: ",1);
  debug(but_id);
#endif
}

#ifdef JOY1_UP
static iNNEXT innext1(JOY1_UP,JOY1_DOWN,JOY1_LEFT,JOY1_RIGHT,JOY1_FIRE,JOY1_POTX,JOY1_POTY);
static iNNEXTparser innext_parser1(&innext1);
#endif
#ifdef JOY2_UP
static iNNEXT innext2(JOY2_UP,JOY2_DOWN,JOY2_LEFT,JOY2_RIGHT,JOY2_FIRE,JOY2_POTX,JOY2_POTY);
static iNNEXTparser innext_parser2(&innext2);
#endif

////////////////////////////////////////////////////////////////////

// the setup function runs once when you press reset or power the board
void setup()
{
#ifdef JOY1_UP
  innext1.init();
#endif
#ifdef JOY2_UP
  innext2.init();
#endif

#if USE_SERIAL
  Serial.begin(SERIAL_BAUD);
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  Serial.println("\nStart");
#endif

#if USE_LCD
  lcd.begin(LCD_COLS, LCD_ROWS);
#endif

  if (Usb.Init() == -1)
  {
    debug("! USB init");
  }

  delay(200);

#ifdef JOY1_UP
  if (!Hid.SetReportParser(0, &innext_parser1))
  {
    debug("!inext1");
  }
#endif
#ifdef JOY2_UP
  if (!Hid.SetReportParser(1, &innext_parser2))
  {
    debug("!inext2");
  }
#endif
}

// the loop function runs over and over again forever
void loop()
{
  Usb.Task();
  timer.tick();
}
