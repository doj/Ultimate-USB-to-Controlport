// -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil; mode: c++ -*-
/*
  Ultimate USB to Controlport
  https://github.com/doj/Ultimate-USB-to-Controlport

  /Applications/Arduino.app/Contents/Java/hardware/arduino/avr/cores/arduino/Arduino.h
*/

#include <usbhid.h>
#include <hiduniversal.h>
#include <usbhub.h>
#include <hidboot.h>
#include "innext_snes.h"
#include "debug.h"
#include "timer.h"
#include "utility.h"

#if USE_SERIAL
void debugs(const char *str)
{
  Serial.print(str);
}

void debugv(uint8_t v, const uint8_t mode)
{
  static const char *hexdigit = "0123456789abcdef";
  char buf[4];
  if (mode == HEX)
    {
      buf[0] = hexdigit[v >> 4];
      buf[1] = hexdigit[v & 15];
      buf[2] = 0;
      Serial.print(buf);
    }
  else
    {
      buf[3] = 0;
      buf[2] = hexdigit[v % 10];
      if (v < 10)
        {
          Serial.print(buf+2);
          return;
        }
      v /= 10;
      buf[1] = hexdigit[v % 10];
      if (v < 10)
        {
          Serial.print(buf+1);
          return;
        }
      buf[0] = hexdigit[v % 10];
      Serial.print(buf);
    }
}
#endif

static USB Usb;

////////////////////////////////////////////////////////////////////

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

///@name auto fire feature
///@{
typedef Timer<2> Timer_t;
static Timer_t timer;
/// \todo see how to move this into the ControlPort class, then we can move joystick() in there as well.
bool
autoFireCB(void *arg)
{
  uint8_t *state = (uint8_t*)arg;
  *state ^= 0x80;
  joystick(*state & 0x7f, *state & 0x80);
  return true;
}
///@}

////////////////////////////////////////////////////////////////////

class ControlPortDevice
{
protected:
  uint8_t m_pinUp;
  uint8_t m_pinDown;
  uint8_t m_pinLeft;
  uint8_t m_pinRight;
  uint8_t m_pinFire;
  uint8_t m_pinPotX;
  uint8_t m_pinPotY;

  ControlPortDevice(uint8_t pinUp, uint8_t pinDown, uint8_t pinLeft, uint8_t pinRight, uint8_t pinFire, uint8_t pinPotX, uint8_t pinPotY) :
    m_pinUp(pinUp),
    m_pinDown(pinDown),
    m_pinLeft(pinLeft),
    m_pinRight(pinRight),
    m_pinFire(pinFire),
    m_pinPotX(pinPotX),
    m_pinPotY(pinPotY)
  {}

  /// set an Arduino pin to LOW or HIGH for a Commodore control port POT input.
  /// the Commodore control port pin is either +5V (state==LOW) or high-z (state==HIGH).
  /// @param pin Arduino pin
  /// @param state LOW or HIGH. Use LOW is the button is pressed.
  /// \note do not use this function to set digital joystick pins! Use joystick() for that purpose.
  void
  pot(const uint8_t pin, const uint8_t state) const
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
};

////////////////////////////////////////////////////////////////////

#if defined(JOYSTICK1) || defined(JOYSTICK2)

class iNNEXT : public iNNEXTevents, public ControlPortDevice
{
#if USE_SERIAL
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

  uint8_t m_autoFireAfreq = AUTO_FIRE_A_FREQ;
  uint8_t m_autoFireYfreq = AUTO_FIRE_Y_FREQ;

  ///@name SNES joystick button names
  ///@{
  uint8_t BUT_X = 1; ///< Sony triangle
  uint8_t BUT_A = 2; ///< Sony circle
  uint8_t BUT_B = 3; ///< Sony cross
  uint8_t BUT_Y = 4; ///< Sony square
  uint8_t BUT_L = 5; ///< Sony L2
  uint8_t BUT_R = 6; ///< Sony R2
  uint8_t BUT_L1 = 7; ///< only Sony
  uint8_t BUT_R1 = 8; ///< only Sony
  uint8_t BUT_SELECT = 9;
  uint8_t BUT_START  = 10;
  ///@}

public:
  iNNEXT(uint8_t pinUp, uint8_t pinDown, uint8_t pinLeft, uint8_t pinRight, uint8_t pinFire, uint8_t pinPotX, uint8_t pinPotY) :
    ControlPortDevice(pinUp, pinDown, pinLeft, pinRight, pinFire, pinPotX, pinPotY)
  {}
  void init() const;
  void OnX(uint8_t x) override;
  void OnY(uint8_t y) override;
  void OnButtonUp(uint8_t but_id) override;
  void OnButtonDn(uint8_t but_id) override;
  bool isAutoFireAConfig() const
  {
    static const uint16_t mask = _BV(BUT_SELECT) | _BV(BUT_START) | _BV(BUT_A);
    return (m_button_state & mask) == mask;
  }
  bool isAutoFireYConfig() const
  {
    static const uint16_t mask = _BV(BUT_SELECT) | _BV(BUT_START) | _BV(BUT_Y);
    return (m_button_state & mask) == mask;
  }
  bool isDirectionSwitchConfig() const
  {
    static const uint16_t mask = _BV(BUT_SELECT) | _BV(BUT_START) | _BV(BUT_B);
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

#if USE_SERIAL
void
iNNEXT::debugAxes() const
{
  debug("x "); debugv(m_x);
  debug(" y ");debugv(m_y);
  debug("\n");
}
#endif

void
iNNEXT::OnX(uint8_t x)
{
  if (x < AXIS_CENTER - AXIS_SENSITIVITY)
    {
      if (isDirectionSwitchConfig())
        {
          std::swap(m_pinLeft,m_pinRight);
          std::swap(m_pinUp,m_pinDown);
          std::swap(BUT_A,BUT_Y);
          std::swap(BUT_B,BUT_X);
          std::swap(BUT_L,BUT_R);
          std::swap(BUT_L1,BUT_R1);
          std::swap(BUT_SELECT,BUT_START);
          debug("lefty\n");
        }
      else
        {
          joystick(m_pinLeft,   LOW);
          joystick(m_pinRight, HIGH);
        }
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
#if USE_SERIAL
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
      joystick(m_pinUp,  HIGH);
      joystick(m_pinDown, LOW);

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
      joystick(m_pinUp,   HIGH);
      joystick(m_pinDown, HIGH);
    }
#if USE_SERIAL
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

#if USE_SERIAL
  debug("Up: ");
  debugv(but_id);
  debug("\n");
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
      startAutoFire(m_autoFireAfreq);
    }
  else if (but_id == BUT_Y)
    {
      startAutoFire(m_autoFireYfreq);
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

#if USE_SERIAL
  debug("Dn: ");
  debugv(but_id);
  debug(" ");
  debugv(m_pinUp);
  debug("\n");
#endif
}

#if defined(JOYSTICK1)
static USBHub HubJoy1(&Usb);
static HIDUniversal HidUniJoy1(&Usb);
#if JOYSTICK1 == 1
static iNNEXT innext1(CP1_UP,CP1_DOWN,CP1_LEFT,CP1_RIGHT,CP1_FIRE,CP1_POTX,CP1_POTY);
#elif JOYSTICK1 == 2
static iNNEXT innext1(CP2_UP,CP2_DOWN,CP2_LEFT,CP2_RIGHT,CP2_FIRE,CP2_POTX,CP2_POTY);
#else
#error unknown value for JOYSTICK1
#endif
static iNNEXTparser innext_parser1(&innext1);
#endif

#if defined(JOYSTICK2)
static USBHub HubJoy2(&Usb);
static HIDUniversal HidUniJoy2(&Usb);
#if JOYSTICK2 == 1
static iNNEXT innext2(CP1_UP,CP1_DOWN,CP1_LEFT,CP1_RIGHT,CP1_FIRE,CP1_POTX,CP1_POTY);
#elif JOYSTICK2 == 2
static iNNEXT innext2(CP2_UP,CP2_DOWN,CP2_LEFT,CP2_RIGHT,CP2_FIRE,CP2_POTX,CP2_POTY);
#else
#error unknown value for JOYSTICK2
#endif
static iNNEXTparser innext_parser2(&innext2);
#endif

#endif // JOYSTICK1 || JOYSTICK2

////////////////////////////////////////////////////////////////////

#if defined(MOUSE1) || defined(MOUSE2)

class Commodore1351 : public HIDReportParser , public ControlPortDevice
{
  struct MouseData {
    struct button_t {
      uint8_t left : 1;
      uint8_t right : 1;
      uint8_t middle : 1;
      uint8_t unused : 5;
    } button;
    int8_t dX;
    int8_t dY;
    int8_t wheel;
  };
  static_assert(sizeof(MouseData) == 4, "unexpected MouseData size");

  uint8_t m_init = 0;
  const uint8_t m_num;
  uint8_t m_x = 127;
  uint8_t m_y = 127;
  MouseData::button_t m_oldButton;

  void init();
  void move(const int8_t x, const int8_t y);
  void mouse(const uint8_t pin, const uint8_t state);
  void enableInterrupt();
  void disableInterrupt();
public:
  void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) override;
  Commodore1351(uint8_t num, uint8_t pinUp, uint8_t pinDown, uint8_t pinLeft, uint8_t pinRight, uint8_t pinFire, uint8_t pinPotX, uint8_t pinPotY) :
    ControlPortDevice(pinUp, pinDown, pinLeft, pinRight, pinFire, pinPotX, pinPotY),
    m_num(num)
  {}
  void irq();
  void irq2();
};

#if defined(MOUSE1)
static USBHub HubMouse1(&Usb);
static HIDBoot<USB_HID_PROTOCOL_MOUSE> HidMouse1(&Usb);
#if MOUSE1 == 1
static Commodore1351 mouse1(1, CP1_UP, CP1_DOWN, CP1_LEFT, CP1_RIGHT, CP1_FIRE, CP1_POTX, CP1_POTY);
#elif MOUSE1 == 2
static Commodore1351 mouse1(1, CP2_UP, CP2_DOWN, CP2_LEFT, CP2_RIGHT, CP2_FIRE, CP2_POTX, CP2_POTY);
#else
#error unknown value for MOUSE 1
#endif
#endif

#if defined(MOUSE2)
static USBHub HubMouse2(&Usb);
static HIDBoot<USB_HID_PROTOCOL_MOUSE> HidMouse2(&Usb);
#if MOUSE2 == 1
static Commodore1351 mouse2(2, CP1_UP, CP1_DOWN, CP1_LEFT, CP1_RIGHT, CP1_FIRE, CP1_POTX, CP1_POTY);
#elif MOUSE2 == 2
static Commodore1351 mouse2(2, CP2_UP, CP2_DOWN, CP2_LEFT, CP2_RIGHT, CP2_FIRE, CP2_POTX, CP2_POTY);
#else
#error unknown value for MOUSE 2
#endif
#endif

static void mouse1_irq();
static void mouse2_irq();

static void
mouse1_irq()
{
#if defined(MOUSE1)
  mouse1.irq();
#endif
}

static void
mouse2_irq()
{
#if defined(MOUSE2)
  mouse2.irq();
#endif
}

void
Commodore1351::init()
{
  joystick(m_pinUp,    HIGH);
  joystick(m_pinDown,  HIGH);
  joystick(m_pinLeft,  HIGH);
  joystick(m_pinRight, HIGH);
  joystick(m_pinFire,  HIGH);
  mouse(m_pinPotX, HIGH);
  mouse(m_pinPotY, HIGH);
  enableInterrupt();
  m_init = 1;
}

void
Commodore1351::enableInterrupt()
{
  if (m_num == 1)
    {
      attachInterrupt(digitalPinToInterrupt(m_pinPotX), mouse1_irq, FALLING);
    }
  else if (m_num == 2)
    {
      attachInterrupt(digitalPinToInterrupt(m_pinPotX), mouse2_irq, FALLING);
    }
}

void
Commodore1351::disableInterrupt()
{
  if (m_num == 1)
    {
      detachInterrupt(digitalPinToInterrupt(m_pinPotX));
    }
  else if (m_num == 2)
    {
      detachInterrupt(digitalPinToInterrupt(m_pinPotX));
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

  mouse(m_pinPotY, LOW);
  sei();//allow interrupts
}

SIGNAL(TIMER1_COMPA_vect)
{
  cli();
  // disable interrupt
  TIMSK1 &= ~_BV(OCIE1A);

  mouse1.irq2();
  sei();
}

void
Commodore1351::irq2()
{
  mouse(m_pinPotY, HIGH);
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

void Commodore1351::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf)
{
  (void)hid;
  (void)is_rpt_id;

#if 0
  for(int i = 0; i < len; ++i)
    {
      debugv(buf[i]);
      debug(" ");
    }
  debug("\n");
#endif

  if (len < 3)
    {
      debug("mouse usb len:");
      debugv(len);
      debug("\n");
      return;
    }

  const MouseData *d = reinterpret_cast<MouseData*>(buf);

  if (m_oldButton.left != d->button.left)
    {
      joystick(m_pinFire, d->button.left);
    }
  if (m_oldButton.right != d->button.right)
    {
      joystick(m_pinUp, d->button.right);
    }
  if (m_oldButton.middle != d->button.middle)
    {
      joystick(m_pinDown, d->button.middle);
    }
  m_oldButton = d->button;

  // TODO scroll wheel.
  // Scroll wheel(s), are not part of the spec, but we could support it.
  // Logitech wireless keyboard and mouse combo reports scroll wheel in byte 4
  // We wouldn't even need to save this information.
  //if(len > 3) {
  //}

  if (d->dX || d->dY)
    {
      move(d->dX, d->dY);
    }
}

void Commodore1351::move(const int8_t x, const int8_t y)
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

  debug("num=");
  debugv(m_num);
  debug(" dx=");
  debugv(x, DEC);
  debug(" dy=");
  debugv(y, DEC);
  debug(" m_x=");
  debugv(m_x, DEC);
  debug(" m_y=");
  debugv(m_y, DEC);
  debug("\n");

  if (! m_init)
    {
      init();
    }
};

#endif

////////////////////////////////////////////////////////////////////

// the setup function runs once when you press reset or power the board
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

#if defined(MOUSE1)
  if (! HidMouse1.SetReportParser(0, &mouse1))
    {
      debug("!m1");
    }
#endif
#if defined(MOUSE2)
  if (! HidMouse2.SetReportParser(0, &mouse2))
    {
      debug("!m2");
    }
#endif
#if defined(JOYSTICK1)
  innext1.init();
  if (! HidUniJoy1.SetReportParser(1, &innext_parser1))
    {
      debug("!j1");
    }
#endif
#if defined(JOYSTICK2)
  innext2.init();
  if (! HidUniJoy2.SetReportParser(2, &innext_parser2))
    {
      debug("!j2");
    }
#endif
}

// the loop function runs over and over again forever
void loop()
{
  Usb.Task();
  timer.tick();
}
