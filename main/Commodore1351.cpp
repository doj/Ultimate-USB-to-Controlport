// -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil; mode: c++ -*-

#include "Commodore1351.h"
#include "debug.h"
#include "ControlPortDevice.h"

Commodore1351::~Commodore1351()
{
}

static void mouse1_irq();
static void mouse2_irq();

static Commodore1351 *mouse1;
static void
mouse1_irq()
{
#if defined(MOUSE1)
  mouse1->irq();
#endif
}

static Commodore1351 *mouse2;
static void
mouse2_irq()
{
#if defined(MOUSE2)
  mouse2->irq();
#endif
}

void
Commodore1351::init()
{
#if 0
  debugv(m_num);
  debug("mouse init\n");
#endif
  if (m_num == 1)
    {
      mouse1 = this;
    }
  else if (m_num == 2)
    {
      mouse2 = this;
    }
  else
    {
      debug("bad mouse num\n");
      return;
    }
  m_cpd->joystick(m_cpd->m_pinUp,    HIGH);
  m_cpd->joystick(m_cpd->m_pinDown,  HIGH);
  m_cpd->joystick(m_cpd->m_pinLeft,  HIGH);
  m_cpd->joystick(m_cpd->m_pinRight, HIGH);
  m_cpd->joystick(m_cpd->m_pinFire,  HIGH);
  mouse(m_cpd->m_pinPotX, HIGH);
  mouse(m_cpd->m_pinPotY, HIGH);
  enableInterrupt();
}

void
Commodore1351::enableInterrupt()
{
  if (m_num == 1)
    {
      attachInterrupt(digitalPinToInterrupt(m_cpd->m_pinPotX), mouse1_irq, FALLING);
    }
  else if (m_num == 2)
    {
      attachInterrupt(digitalPinToInterrupt(m_cpd->m_pinPotX), mouse2_irq, FALLING);
    }
}

void
Commodore1351::disableInterrupt()
{
  if (m_num == 1)
    {
      detachInterrupt(digitalPinToInterrupt(m_cpd->m_pinPotX));
    }
  else if (m_num == 2)
    {
      detachInterrupt(digitalPinToInterrupt(m_cpd->m_pinPotX));
    }
}

void
Commodore1351::irq()
{
  cli();//stop interrupts
  disableInterrupt();

  // https://www.robotshop.com/community/forum/t/arduino-101-timers-and-interrupts/13072
  // configure timer1 to 2Mhz ticks
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = (1 << WGM12) | // enable timer compare interrupt
    (1 << CS11);  // Set CS12 and CS10 bits for 8 prescaler
  // set compare match register for 2Mhz increments
  OCR1A = (200 + m_y) * 2;
  TCNT1  = 0;//initialize counter value to 0
  TIMSK1 |= (1 << OCIE1A); // turn on CTC mode

  mouse(m_cpd->m_pinPotY, LOW);
  sei();//allow interrupts
}

SIGNAL(TIMER1_COMPA_vect)
{
  cli();
  // disable interrupt
  TIMSK1 &= ~_BV(OCIE1A);

  mouse1->irq2();
  sei();
}

void
Commodore1351::irq2()
{
  mouse(m_cpd->m_pinPotY, HIGH);
  enableInterrupt();
}

void
Commodore1351::mouse(const uint8_t pin, const uint8_t state)
{
  if (state == LOW)
    {
      digitalWrite(pin, LOW); // GND
      pinMode(pin, OUTPUT);
    }
  else
    {
      pinMode(pin, INPUT);
      digitalWrite(pin, HIGH);
    }
}

void
Commodore1351::parse(const uint8_t *buf, uint8_t const len, USBHID *hid, const uint8_t bAddress, const uint8_t epAddress)
{
  (void)hid;
  (void)bAddress;
  (void)epAddress;

#if 0
  for(int i = 0; i < len; ++i)
    {
      debugv(buf[i]);
      debug(" ");
    }
  debugnl();
#endif

  if (len < 3 || len > 6)
    {
      debugv(m_num);
      debug("mouse usb len:");
      debugv(len);
      debugnl();
      return;
    }

  const MouseData *d = reinterpret_cast<const MouseData*>(buf);

  if (m_oldButton.left != d->button.left)
    {
      m_cpd->joystick(m_cpd->m_pinFire, d->button.left);
    }
  if (m_oldButton.right != d->button.right)
    {
      m_cpd->joystick(m_cpd->m_pinUp, d->button.right);
    }
  if (m_oldButton.middle != d->button.middle)
    {
      m_cpd->joystick(m_cpd->m_pinDown, d->button.middle);
    }
  if (m_oldButton.side_left != d->button.side_left)
    {
      m_cpd->joystick(m_cpd->m_pinLeft, d->button.side_left);
    }
  if (m_oldButton.side_right != d->button.side_right)
    {
      m_cpd->joystick(m_cpd->m_pinRight, d->button.side_right);
    }
  m_oldButton = d->button;

  if (len == 6)
    {
      // apple mouse
      (void)d->wLR; // is up/down
      (void)d->wUD; // is left/right
    }
  else
    {
      if (len >= 4)
        {
          (void)d->wUD;
          // \todo micromys mouse wheel for up/down on pinLeft,pinRight
        }
      if (len >= 5)
        {
          (void)d->wLR;
          // \todo micromys mouse wheel for left right on pinUp,pinDown
        }
    }

  if (d->dX || d->dY)
    {
      move(d->dX, d->dY);
    }
}

void
Commodore1351::move(const int8_t x, const int8_t y)
{
  m_x += x;
  if (m_x < 63)
    {
      m_x += 128;
    }
  else if (m_x > 192)
    {
      m_x -= 128;
    }

  m_y += y;
  if (m_y < 63)
    {
      m_y += 128;
    }
  else if (m_y > 192)
    {
      m_y -= 128;
    }

  debugs("num=");
  debugv(m_num);
  debug(" dx=");
  debugv(x, DEC);
  debug(" dy=");
  debugv(y, DEC);
  debug(" m_x=");
  debugv(m_x, DEC);
  debug(" m_y=");
  debugv(m_y, DEC);
  debugnl();
};
