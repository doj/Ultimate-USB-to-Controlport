// -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil; mode: c++ -*-

#include "USBController.h"
#include "debug.h"
#include "ControlPortDevice.h"
#include "utility.h"

///@name auto fire feature
///@{
void joystick(const uint8_t pin, const uint8_t state);
/// \todo move this into the USBController class, then we can move joystick() in there as well.
bool
autoFireCB(void *arg)
{
  uint8_t *state = (uint8_t*)arg;
  *state ^= 0x80;
  joystick(*state & 0x7f, *state & 0x80);
  return true;
}
///@}

USBController::~USBController()
{
  debugv(m_num);
  debug("~contr");
}


void
USBController::init() const
{
  debugv(m_num);
  debug("contr init");
  m_cpd->joystick(m_cpd->m_pinUp,    HIGH);
  m_cpd->joystick(m_cpd->m_pinDown,  HIGH);
  m_cpd->joystick(m_cpd->m_pinLeft,  HIGH);
  m_cpd->joystick(m_cpd->m_pinRight, HIGH);
  m_cpd->joystick(m_cpd->m_pinFire,  HIGH);
  m_cpd->pot(m_cpd->m_pinPotX, HIGH);
  m_cpd->pot(m_cpd->m_pinPotX, HIGH);
}

void
USBController::cancelAutoFire()
{
  m_cpd->joystick(m_cpd->m_pinFire, HIGH);
  timer.cancel(m_autoFireTask);
}

void
USBController::startAutoFire(uint8_t freq)
{
  timer.cancel(m_autoFireTask);
  m_autoFireState = m_cpd->m_pinFire;
  m_cpd->joystick(m_cpd->m_pinFire, LOW);
  m_autoFireTask = timer.every(1000/2/freq, autoFireCB, &m_autoFireState);
}

#if USE_SERIAL
void
USBController::debugAxes() const
{
  debug("x "); debugv(m_x);
  debug(" y ");debugv(m_y);
  debug("\n");
}
#endif

void
USBController::OnX(uint8_t x)
{
  if (x < AXIS_CENTER - AXIS_SENSITIVITY)
    {
      if (isDirectionSwitchConfig())
        {
          std::swap(m_cpd->m_pinLeft, m_cpd->m_pinRight);
          std::swap(m_cpd->m_pinUp, m_cpd->m_pinDown);
          std::swap(BUT_A,BUT_Y);
          std::swap(BUT_B,BUT_X);
          std::swap(BUT_L,BUT_R);
          std::swap(BUT_L1,BUT_R1);
          std::swap(BUT_SELECT,BUT_START);
          debug("lefty\n");
        }
      else
        {
          m_cpd->joystick(m_cpd->m_pinLeft,   LOW);
          m_cpd->joystick(m_cpd->m_pinRight, HIGH);
        }
    }
  else if (x > AXIS_CENTER + AXIS_SENSITIVITY)
    {
      m_cpd->joystick(m_cpd->m_pinLeft,  HIGH);
      m_cpd->joystick(m_cpd->m_pinRight, LOW);
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
      m_cpd->joystick(m_cpd->m_pinUp,   LOW);
      m_cpd->joystick(m_cpd->m_pinDown, HIGH);

      if (isAutoFireAConfig() && m_autoFireAfreq < 255)
        {
          ++m_autoFireAfreq;
          debug("auto fire A "); debugv(m_autoFireAfreq);
        }
      if (isAutoFireYConfig() && m_autoFireYfreq < 255)
        {
          ++m_autoFireYfreq;
          debug("auto fire Y "); debugv(m_autoFireYfreq);
        }
    }
  else if (y > AXIS_CENTER + AXIS_SENSITIVITY)
    {
      m_cpd->joystick(m_cpd->m_pinUp,  HIGH);
      m_cpd->joystick(m_cpd->m_pinDown, LOW);

      if (isAutoFireAConfig() && m_autoFireAfreq > 1)
        {
          --m_autoFireAfreq;
          debug("auto fire A "); debugv(m_autoFireAfreq);
        }
      if (isAutoFireYConfig() && m_autoFireYfreq > 1)
        {
          --m_autoFireYfreq;
          debug("auto fire Y "); debugv(m_autoFireYfreq);
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
USBController::OnButtonUp(uint8_t but_id)
{
  if (but_id == BUT_B)
    {
      m_cpd->joystick(m_cpd->m_pinFire, HIGH);
    }
  else if (but_id == BUT_X)
    {
      m_cpd->joystick(m_cpd->m_pinUp, HIGH);
    }
  else if (but_id == BUT_L)
    {
      m_cpd->joystick(m_cpd->m_pinLeft, HIGH);
    }
  else if (but_id == BUT_R)
    {
      m_cpd->joystick(m_cpd->m_pinRight, HIGH);
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
      m_cpd->pot(m_cpd->m_pinPotX, HIGH);
    }
  else if (but_id == BUT_START)
    {
      m_cpd->pot(m_cpd->m_pinPotY, HIGH);
    }

  uint16_t mask = 1 << but_id;
  m_button_state &= ~mask;

#if USE_SERIAL
  debugv(m_num);
  debug("Up: ");
  debugv(but_id);
  debug("\n");
#endif
}

void
USBController::OnButtonDn(uint8_t but_id)
{
  if (but_id == BUT_B)
    {
      m_cpd->joystick(m_cpd->m_pinFire, LOW);
    }
  else if (but_id == BUT_X)
    {
      m_cpd->joystick(m_cpd->m_pinUp, LOW);
    }
  else if (but_id == BUT_L)
    {
      m_cpd->joystick(m_cpd->m_pinLeft, LOW);
    }
  else if (but_id == BUT_R)
    {
      m_cpd->joystick(m_cpd->m_pinRight, LOW);
    }
  else if (but_id == BUT_A)
    {
      startAutoFire(m_autoFireAfreq);
    }
  else if (but_id == BUT_Y)
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

#if USE_SERIAL
  debugv(m_num);
  debug("Dn: ");
  debugv(but_id);
  debug(" ");
  debugv(m_cpd->m_pinUp);
  debug("\n");
#endif
}

void
USBController::parse(const uint8_t *buf, const uint8_t len)
{
  if (len < 7)
    {
      debugv(len,DEC);
      debug("contr len");
      return;
    }

  if (buf[3] != m_oldX)
    {
      m_oldX = buf[3];
      OnX(m_oldX);
    }

  if (buf[4] != m_oldY)
    {
      m_oldY = buf[4];
      OnY(m_oldY);
    }

#if 0
  // Calling Hat Switch event handler
  const uint8_t hat = (buf[5] & 0xF);
  if (hat != oldHat)
    {
      joyEvents->OnHatSwitch(hat);
      oldHat = hat;
    }
#endif

  uint16_t buttons = (0x0000 | buf[6]);
  buttons <<= 4;
  buttons |= (buf[5] >> 4);
  const uint16_t changes = (buttons ^ m_oldButtons);

  // Calling Button Event Handler for every button changed
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
