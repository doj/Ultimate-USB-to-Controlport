// -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil; mode: c++ -*-

#include "Commodore1351.h"
#include "debug.h"
#include "ControlPortDevice.h"

static void mouse(const uint8_t pin, const uint8_t state);

static void
mouse(const uint8_t pin, const uint8_t state)
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

Commodore1351::~Commodore1351()
{
  detachInterrupt(digitalPinToInterrupt(m_cpd->m_pinPotX));
}

static void mouse1_irq();

static Commodore1351 *mouse1;
static void
mouse1_irq()
{
  mouse1->irq();
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

  // https://www.robotshop.com/community/forum/t/arduino-101-timers-and-interrupts/13072
  // https://gist.github.com/psgs/e7ec757412c0b5cda60f854be57792fd
  // https://www.nongnu.org/avr-libc/user-manual/group__avr__interrupts.html
  // configure timer1 to 2Mhz ticks
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = _BV(WGM12)  // enable timer compare interrupt
           | _BV(CS11) // Set CS11 for 8 divider to result in 2Mhz
    ;
  TCCR1C = 0;

  attachInterrupt(digitalPinToInterrupt(m_cpd->m_pinPotX), mouse1_irq, FALLING);
}

static uint8_t timer1Apin;
static uint8_t timer1Bpin;

void
Commodore1351::irq()
{
  // set up timer 1

  OCR1B = 0xffff;
  OCR1A = 0xffff;
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 2Mhz increments

  // The SID cycles every 512 C64 clock ticks.
  // It keeps the line LOW for 256 cycles, then begins the measurement.
  // this function has about 20 timer ticks latency.
#define OFFSET 200

  // when OCR1A triggers the counter seems to be not functional any more.
  // assign the smaller value to OCR1B.

  if (m_x < m_y)
    {
      timer1Bpin = m_cpd->m_pinPotX;
      OCR1B = (OFFSET+m_x)*2;
      timer1Apin = m_cpd->m_pinPotY;
      OCR1A = (OFFSET+m_y)*2;
    }
  else
    {
      timer1Bpin = m_cpd->m_pinPotY;
      OCR1B = (OFFSET+m_y)*2;
      timer1Apin = m_cpd->m_pinPotX;
      OCR1A = (OFFSET+m_x)*2;
    }
  TIMSK1 |= _BV(OCIE1A)
         |  _BV(OCIE1B)
    ; // turn on CTC mode

  mouse(timer1Apin, LOW);
  mouse(timer1Bpin, LOW);
}

SIGNAL(TIMER1_COMPA_vect)
{
  mouse(timer1Apin, HIGH);
}

SIGNAL(TIMER1_COMPB_vect)
{
  mouse(timer1Bpin, HIGH);
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
      debugu(m_num);
      debug("mouse usb len:");
      debugu(len);
      debugnl();
      return;
    }

  const MouseData *d = reinterpret_cast<const MouseData*>(buf);

  if (m_oldButton.left != d->button.left)
    {
      m_cpd->joystick(m_cpd->m_pinFire, !d->button.left);
    }
  if (m_oldButton.right != d->button.right)
    {
      m_cpd->joystick(m_cpd->m_pinUp, !d->button.right);
    }
  if (m_oldButton.middle != d->button.middle)
    {
      m_cpd->joystick(m_cpd->m_pinDown, !d->button.middle);
    }
  if (m_oldButton.side_left != d->button.side_left)
    {
      m_cpd->joystick(m_cpd->m_pinLeft, !d->button.side_left);
    }
  if (m_oldButton.side_right != d->button.side_right)
    {
      m_cpd->joystick(m_cpd->m_pinRight, !d->button.side_right);
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
  debugu(m_num,DEC);
  debug(" dx=");
  debugi(x);
  debug(" dy=");
  debugi(y);
  debug(" m_x=");
  debugu(m_x,DEC);
  debug(" m_y=");
  debugu(m_y,DEC);
  debugnl();
};
