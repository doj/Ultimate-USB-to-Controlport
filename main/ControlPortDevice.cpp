// -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil; mode: c++ -*-

#include "ControlPortDevice.h"
#include "Commodore1351.h"
#include "USBKeyboard.h"
#include "debug.h"

void joystick(const uint8_t pin, const uint8_t state);

uint8_t ControlPortDevice::s_used = 0;

void
ControlPortDevice::initMouse()
{
#if 0
  debugp(this);
  debug("initMouse");
  debugnl();
#endif
  if (! (s_used & MOUSE1))
    {
      m_used  = MOUSE1;
      s_used |= MOUSE1;
      m_handler = new Commodore1351(1,this);
      initPins();
      return;
    }
  if (! (s_used & MOUSE2))
    {
      m_used  = MOUSE2;
      s_used |= MOUSE2;
      m_handler = new Commodore1351(2,this);
      initPins();
      return;
    }
  debug("!initMouse");
}

void
ControlPortDevice::initJoystick(const USBController::type_t t)
{
#if 0
  debugp(this);
  debug("initJoy");
  debugnl();
#endif
  if (! (s_used & JOYSTICK1))
    {
      m_used  = JOYSTICK1;
      s_used |= JOYSTICK1;
      m_handler = new USBController(1,this,t);
      initPins();
      return;
    }
  if (! (s_used & JOYSTICK2))
    {
      m_used  = JOYSTICK2;
      s_used |= JOYSTICK2;
      m_handler = new USBController(2,this,t);
      initPins();
      return;
    }
  debug("!initJoy");
}

void
ControlPortDevice::initPins()
{
  if (m_used == JOYSTICK1 ||
      m_used == MOUSE1 ||
      m_used == KEYBOARD1)
    {
      m_pinUp    = CP1_UP;
      m_pinDown  = CP1_DOWN;
      m_pinLeft  = CP1_LEFT;
      m_pinRight = CP1_RIGHT;
      m_pinFire  = CP1_FIRE;
      m_pinPotX  = CP1_POTX;
      m_pinPotY  = CP1_POTY;
    }
  else if (m_used == JOYSTICK2 ||
           m_used == MOUSE2 ||
           m_used == KEYBOARD2)
    {
      m_pinUp    = CP2_UP;
      m_pinDown  = CP2_DOWN;
      m_pinLeft  = CP2_LEFT;
      m_pinRight = CP2_RIGHT;
      m_pinFire  = CP2_FIRE;
      m_pinPotX  = CP2_POTX;
      m_pinPotY  = CP2_POTY;
    }
}


void
ControlPortDevice::initKeyboard()
{
#if 0
  debugp(this);
  debug("initKey");
  debugnl();
#endif
  if (! (s_used & KEYBOARD1))
    {
      m_used  = KEYBOARD1;
      s_used |= KEYBOARD1;
      m_handler = new USBKeyboard(1,this);
      initPins();
      return;
    }
  if (! (s_used & KEYBOARD2))
    {
      m_used  = KEYBOARD2;
      s_used |= KEYBOARD2;
      m_handler = new USBKeyboard(2,this);
      initPins();
      return;
    }
  debug("!initKey");
}

#if defined(USB_HOST_SHIELD_VERSION) && (USB_HOST_SHIELD_VERSION >= 0x020303)
void
ControlPortDevice::Release()
{
  s_used &= ~m_used;
  m_used = 0;
  delete m_handler;
}
#endif

void
#if defined(USB_HOST_SHIELD_VERSION) && (USB_HOST_SHIELD_VERSION >= 0x020303)
ControlPortDevice::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf, uint8_t bAddress, uint8_t epAddress)
{
#else
ControlPortDevice::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf)
{
  uint8_t bAddress = 0;
  uint8_t epAddress = 0;
#endif
  (void)is_rpt_id;

#if 0
  debug("parse");
  for(int i = 0; i < len; ++i)
    {
      debug(" ");
      debugu(buf[i]);
    }
  debugnl();
#endif

  if (! m_handler)
    {
      if (len >= 3 && len <= 6)
        {
          initMouse();
        }
      else if (len == 2)
        {
          initJoystick(USBController::SonyPS1);
        }
      if (len == 8)
        {
          if (buf[0] == 0x01 &&
              buf[1] == 0x7f &&
              buf[2] == 0x7f)
            {
              initJoystick(USBController::iNNEXT);
            }
          else if (buf[0] == 0x80 &&
                   buf[1] == 0x80 &&
                   buf[2] == 0x80 &&
                   buf[3] == 0x80)
            {
              initJoystick(USBController::P4_5N);
            }
          else if (buf[1] == 0x00)
            {
              initKeyboard();
            }
        }
    }
  if (m_handler)
    {
      m_handler->parse(buf, len, hid, bAddress, epAddress);
    }
}

/// set an Arduino pin to LOW or HIGH for a Commodore control port POT input.
/// the Commodore control port pin is either +5V (state==LOW) or high-z (state==HIGH).
/// @param pin Arduino pin
/// @param state LOW or HIGH. Use LOW is the button is pressed.
/// \note do not use this function to set digital joystick pins! Use joystick() for that purpose.
void
ControlPortDevice::pot(const uint8_t pin, const uint8_t state) const
{
  if (! (pin == m_pinPotX || pin == m_pinPotY))
    return;

  if (state == LOW)
    {
      digitalWrite(pin, HIGH); // +5V
      pinMode(pin, OUTPUT);
      debug("pot ");debugu(pin);debugnl();
    }
  else
    {
      // configure the pin for input. This will make it high impedance (high-z).
      pinMode(pin, INPUT);
      digitalWrite(pin, LOW);
    }
}

/// set an Arduino pin to LOW or HIGH for a Commodore control port digital input.
/// the Commodore control port pin is either GND (state==LOW) or high-z (state==HIGH).
/// @param pin Arduino pin
/// @param state LOW or HIGH. use LOW if the corresponding button or direction is pressed.
/// \sa https://www.arduino.cc/en/Tutorial/DigitalPins
void
ControlPortDevice::joystick(const uint8_t pin, const uint8_t state) const
{
  if (pin == m_pinPotX || pin == m_pinPotY)
    return;

  if (state == LOW)
    {
      digitalWrite(pin, LOW); // GND
      pinMode(pin, OUTPUT);
      debug("joy ");debugu(pin);debugnl();
    }
  else
    {
      // configure the pin for input. This will make it high impedance (high-z).
      pinMode(pin, INPUT);
      digitalWrite(pin, LOW);
    }
}

void
ControlPortDevice::swapPort()
{
  if (m_used == 0)
    return;

  s_used &= ~m_used;
  if (m_used < 0x10)
    m_used <<= 4;
  else
    m_used >>= 4;
  s_used |= m_used;

  initPins();
}
