// -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil; mode: c++ -*-

#include "USBController.h"
#include "debug.h"
#include "ControlPortDevice.h"
#include "utility.h"

USBController::~USBController()
{
}

void
USBController::init() const
{
#if 0
  debugu(m_num,DEC);
  debug(" contr init ");
  debugp(this);
  debugnl();
#endif
  resetAllPins();
}

bool
autoFireCB(void *arg)
{
  reinterpret_cast<USBController*>(arg)->autoFireCB();
  return true;
}

void
USBController::autoFireCB()
{
  m_autoFireState = ! m_autoFireState;
  fire(m_autoFireState);
}

void
USBController::cancelAutoFire()
{
  fire(false);
  timer.cancel(m_autoFireTask);
}

inline unsigned long timerInterval(uint8_t freq)
{
  return 1000/2/freq;
}

void
USBController::startAutoFire(uint8_t freq)
{
  timer.cancel(m_autoFireTask);
  m_autoFireState = true;
  fire(true);
  m_autoFireTask = timer.every(timerInterval(freq), ::autoFireCB, this);
}

void
USBController::updateAutoFireFreq(uint8_t freq)
{
  if (m_autoFireTask)
    {
      if (! timer.updateInterval(m_autoFireTask, timerInterval(freq)))
        {
          debug("failed autofire freq ");
          debugu(freq,DEC);
          debugnl();
        }
    }
}

#if USE_SERIAL
void
USBController::debugAxes() const
{
#if 0
  debug("x "); debugu(m_x);
  debug(" y ");debugu(m_y);
  debugnl();
#endif
}
#endif

uint8_t
USBController::direction(uint8_t pin)
{
  if (m_lefty)
    {
      if (pin == m_cpd->m_pinUp)
        pin = m_cpd->m_pinDown;
      else if (pin == m_cpd->m_pinDown)
        pin = m_cpd->m_pinUp;
      else if (pin == m_cpd->m_pinLeft)
        pin = m_cpd->m_pinRight;
      else if (pin == m_cpd->m_pinRight)
        pin = m_cpd->m_pinLeft;
    }
  return pin;
}

void
USBController::OnX(uint8_t x)
{
  if (x < AXIS_CENTER - AXIS_SENSITIVITY)
    {
      if (isDirectionSwitchConfig())
        {
          debug("lefty\n");
          m_lefty = ! m_lefty;
          std::swap(m_but_a,m_but_y);
          std::swap(m_but_b,m_but_x);
        }
      else
        {
          m_cpd->joystick(direction(m_cpd->m_pinLeft),   LOW);
          m_cpd->joystick(direction(m_cpd->m_pinRight), HIGH);
        }
    }
  else if (x > AXIS_CENTER + AXIS_SENSITIVITY)
    {
      m_cpd->joystick(direction(m_cpd->m_pinLeft),  HIGH);
      m_cpd->joystick(direction(m_cpd->m_pinRight), LOW);
    }
  else
    {
      m_cpd->joystick(m_cpd->m_pinLeft,  HIGH);
      m_cpd->joystick(m_cpd->m_pinRight, HIGH);
    }
#if USE_SERIAL
  m_x = x;
  debugAxes();
#endif
}

void
USBController::OnY(uint8_t y)
{
  if (y < AXIS_CENTER - AXIS_SENSITIVITY)
    {
      if (isAutoFireAConfig() && m_autoFireAfreq < 255)
        {
          ++m_autoFireAfreq;
          debug("auto fire A ");
          debugu(m_autoFireAfreq,DEC);
          debugnl();
          updateAutoFireFreq(m_autoFireAfreq);
        }
      else if (isAutoFireYConfig() && m_autoFireYfreq < 255)
        {
          ++m_autoFireYfreq;
          debug("auto fire Y ");
          debugu(m_autoFireYfreq,DEC);
          debugnl();
          updateAutoFireFreq(m_autoFireYfreq);
        }
      else if (isPortSwitchConfig())
        {
          resetAllPins();
          ::swapControlPorts();
        }
      else
        {
          m_cpd->joystick(direction(m_cpd->m_pinUp),   LOW);
          m_cpd->joystick(direction(m_cpd->m_pinDown), HIGH);
        }
    }
  else if (y > AXIS_CENTER + AXIS_SENSITIVITY)
    {
      if (isAutoFireAConfig() && m_autoFireAfreq > 1)
        {
          --m_autoFireAfreq;
          debug("auto fire A ");
          debugu(m_autoFireAfreq,DEC);
          debugnl();
          updateAutoFireFreq(m_autoFireAfreq);
        }
      else if (isAutoFireYConfig() && m_autoFireYfreq > 1)
        {
          --m_autoFireYfreq;
          debug("auto fire Y ");
          debugu(m_autoFireYfreq,DEC);
          debugnl();
          updateAutoFireFreq(m_autoFireYfreq);
        }
      else
        {
          m_cpd->joystick(direction(m_cpd->m_pinUp),  HIGH);
          m_cpd->joystick(direction(m_cpd->m_pinDown), LOW);
        }
    }
  else
    {
      m_cpd->joystick(m_cpd->m_pinUp,   HIGH);
      m_cpd->joystick(m_cpd->m_pinDown, HIGH);
    }
#if USE_SERIAL
  m_y = y;
  debugAxes();
#endif
}

void
USBController::OnButtonDn(uint8_t but_id)
{
  if (but_id == m_but_b)
    {
      fire(true);
    }
  else if (but_id == m_but_x)
    {
      m_cpd->joystick(m_cpd->m_pinUp, LOW);
    }
  else if (but_id == BUT_L1 || but_id == BUT_L2)
    {
      m_cpd->joystick(direction(m_cpd->m_pinLeft), LOW);
    }
  else if (but_id == BUT_R1 || but_id == BUT_R2)
    {
      m_cpd->joystick(direction(m_cpd->m_pinRight), LOW);
    }
  else if (but_id == m_but_a)
    {
      startAutoFire(m_autoFireAfreq);
    }
  else if (but_id == m_but_y)
    {
      startAutoFire(m_autoFireYfreq);
    }
  else if (but_id == BUT_SELECT)
    {
      m_cpd->pot(m_cpd->m_pinPotX, LOW);
    }
  else if (but_id == BUT_START)
    {
      m_cpd->pot(m_cpd->m_pinPotY, LOW);
    }

  uint16_t mask = 1 << but_id;
  m_button_state |= mask;

#if USE_SERIAL && 0
  debugi(m_num,DEC);
  debug("Dn: ");
  debugu(but_id);
  debug(" ");
  debugu(m_cpd->m_pinUp);
  debugnl();
#endif
}

void
USBController::OnButtonUp(uint8_t but_id)
{
  if (but_id == m_but_b)
    {
      fire(false);
    }
  else if (but_id == m_but_x)
    {
      m_cpd->joystick(m_cpd->m_pinUp, HIGH);
    }
  else if (but_id == BUT_L1 || but_id == BUT_L2)
    {
      m_cpd->joystick(direction(m_cpd->m_pinLeft), HIGH);
    }
  else if (but_id == BUT_R1 || but_id == BUT_R2)
    {
      m_cpd->joystick(direction(m_cpd->m_pinRight), HIGH);
    }
  else if (but_id == m_but_a)
    {
      cancelAutoFire();
    }
  else if (but_id == m_but_y)
    {
      cancelAutoFire();
    }
  else if (but_id == BUT_SELECT)
    {
      m_cpd->pot(m_cpd->m_pinPotX, HIGH);
    }
  else if (but_id == BUT_START)
    {
      m_cpd->pot(m_cpd->m_pinPotY, HIGH);
    }

  uint16_t mask = 1 << but_id;
  m_button_state &= ~mask;

#if USE_SERIAL && 0
  debugu(m_num,DEC);
  debug("Up: ");
  debugu(but_id);
  debugnl();
#endif
}

void
USBController::parse(const uint8_t *buf, const uint8_t len, USBHID *hid, const uint8_t bAddress, const uint8_t epAddress)
{
  (void)hid;
  (void)bAddress;
  (void)epAddress;

#if 0
  debugp(this);
  debugs(" parse");
  for(int i = 0; i < len; ++i)
    {
      debugs(" ");
      debugu(buf[i]);
    }
  debugnl();
#endif

  uint16_t buttons = 0;
  uint8_t x = 0;
  uint8_t y = 0;

  // NES
  if (len == 8)
    {
      x = buf[3];
      y = buf[4];

#if 0
      // Calling Hat Switch event handler
      const uint8_t hat = (buf[5] & 0xF);
      if (hat != oldHat)
        {
          joyEvents->OnHatSwitch(hat);
          oldHat = hat;
        }
#endif

      buttons = buf[6] << 4;
      buttons |= (buf[5] >> 4);
    }
  // Sony
  else if (len == 2)
    {
      buttons = ((buf[1] & 3) << 8) | buf[0];

      y = 0x7f;
      if ((buf[1] & 0x30) == 0x00)
        y = 0;
      else if ((buf[1] & 0x30) == 0x20)
        y = 0xff;

      x = 0x7f;
      if ((buf[1] & 0x0C) == 0x00)
        x = 0;
      else if ((buf[1] & 0x0C) == 0x08)
        x = 0xff;
    }
  else
    {
      debugu(len,DEC);
      debug("contr len");
      return;
    }

  if (x != m_oldX)
    {
      OnX(m_oldX = x);
    }

  if (y != m_oldY)
    {
      OnY(m_oldY = y);
    }

  const uint16_t changes = (buttons ^ m_oldButtons);
  if (changes)
    {
      for (uint8_t i = 0; i < 0x0C; i++)
        {
          uint16_t mask = (0x0001 << i);

          if ((mask & changes) > 0)
            {
              if ((buttons & mask) > 0)
                OnButtonDn(i + 1);
              else
                OnButtonUp(i + 1);
            }
        }
      m_oldButtons = buttons;
    }
}

void
USBController::fireCB(bool on)
{
  (void)on;
}

void
USBController::fire(bool on)
{
  m_cpd->joystick(m_cpd->m_pinFire, on ? LOW : HIGH);
  fireCB(on);
}

void
USBController::resetAllPins() const
{
  m_cpd->joystick(m_cpd->m_pinUp,    HIGH);
  m_cpd->joystick(m_cpd->m_pinDown,  HIGH);
  m_cpd->joystick(m_cpd->m_pinLeft,  HIGH);
  m_cpd->joystick(m_cpd->m_pinRight, HIGH);
  m_cpd->joystick(m_cpd->m_pinFire,  HIGH);
  m_cpd->pot(m_cpd->m_pinPotX, HIGH);
  m_cpd->pot(m_cpd->m_pinPotX, HIGH);
}
