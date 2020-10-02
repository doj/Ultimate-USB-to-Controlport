// -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil; mode: c++ -*-

#include "USBController.h"
#include "debug.h"
#include "ControlPortDevice.h"
#include "utility.h"
#include "hiduniversal.h"

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
  set(0, 0, 0x20, 0x20, 0xff);
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
  set(0x80, 0x80, 0xff, 0x20, 0x20);
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
  m_hid = hid;
  m_bAddress = bAddress;
  m_epAddress = epAddress;

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

  if (len == 8 && m_type == iNNEXT)
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
  else if (len == 8 && m_type == P4_5N)
    {
      // left analog stick
      x = buf[2];
      y = buf[3];

      // check d-pad
      switch(buf[5] & 0xF)
        {
          // up
        case 0: x=0x7f, y=0; break;
          // up+right
        case 1: x=0xff, y=0; break;
          // right
        case 2: x=0xff, y=0x7f; break;
          // down+right
        case 3: x=0xff, y=0xff; break;
          // down
        case 4: x=0x7f, y=0xff; break;
          // down+left
        case 5: x=0,    y=0xff; break;
          // left
        case 6: x=0,    y=0x7f; break;
          // up+left
        case 7: x=0,    y=0; break;
        }

      buttons = 0;
      // triangle
      if (buf[5] & 0x80)
        buttons |= 1 << (BUT_X-1);
      // circle
      if (buf[5] & 0x40)
        buttons |= 1 << (BUT_A-1);
      // cross
      if (buf[5] & 0x20)
        buttons |= 1 << (BUT_B-1);
      // square
      if (buf[5] & 0x10)
        buttons |= 1 << (BUT_Y-1);

      if (buf[6] & 1)
        buttons |= 1 << (BUT_L1-1);
      if (buf[6] & 2)
        buttons |= 1 << (BUT_R1-1);
      if (buf[6] & 4)
        buttons |= 1 << (BUT_L2-1);
      if (buf[6] & 8)
        buttons |= 1 << (BUT_R2-1);

      if (buf[6] & 0x10)
        buttons |= 1 << (BUT_SHARE-1);
      // OPTIONS
      if (buf[6] & 0x20)
        buttons |= 1 << (BUT_SELECT-1);
      // analog left
      if (buf[6] & 0x40)
        buttons |= 1 << (BUT_B-1);
      // analog right
      if (buf[6] & 0x80)
        buttons |= 1 << (BUT_B-1);

      // PS
      if (buf[7] & 1)
        buttons |= 1 << (BUT_START-1);
      // center
      if (buf[7] & 2)
        buttons |= 1 << (BUT_B-1);

      // right analog stick, buf[0], buf[1]
      // TODO: set the pot values
    }
  else if (len == 2 && m_type == SonyPS1)
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

void
USBController::set(const uint8_t rumbleLo, const uint8_t rumbleHi, const uint8_t r, const uint8_t g, const uint8_t b) const
{
  (void)rumbleLo;
  (void)rumbleHi;
  (void)r;
  (void)g;
  (void)b;
#if defined(USB_HOST_SHIELD_VERSION) && USB_HOST_SHIELD_VERSION >= 0x010304 && 0
  if (m_type == P4_5N)
    {
      uint8_t buf[32];
      memset(buf, 0, sizeof(buf));

      buf[0] = 0x05; // Report ID
      buf[1] = 0xFF;

      buf[4] = rumbleLo; // Small Rumble
      buf[5] = rumbleHi; // Big rumble

      buf[6] = r; // Red
      buf[7] = g; // Green
      buf[8] = b; // Blue

      //buf[9] = 0; // Time to flash bright (255 = 2.5 seconds)
      //buf[10] = 0; // Time to flash dark (255 = 2.5 seconds)

      const uint8_t ret = m_hid->SndRpt(sizeof(buf), buf);
      debug(" ret ");debugu(ret);
      debugnl();
    }
  else
    {
      debug("set, no type ");
      debugu(m_type);
      debugnl();
    }
#endif
}
