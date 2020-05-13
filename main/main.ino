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
  - [X] support two USB joysticks with both control ports
  - [ ] fix device mapping in USB Host Shield Library
  - [ ] connect to both control ports and switch joystick by magic button press
  - [ ] reconfigure any USB button to any Commodore button
  - [ ] save configuration to Arduino EEPROM https://www.arduino.cc/en/Reference/EEPROM
  - [ ] support PlayStation Classic USB controller
  - [ ] support PlayStation 3 controller
  - [ ] support PlayStation 4 controller
  - [ ] use analog joystick of a PlayStation 3 controller for POT X and POT Y
  - [ ] support Xbox controller
  - [ ] use USB mouse for POT X and POT Y with 1351 protocol
  - [ ] support mouse wheel

  /Applications/Arduino.app/Contents/Java/hardware/arduino/avr/cores/arduino/Arduino.h
*/

#include <usbhid.h>
#include <hiduniversal.h>
#include <usbhub.h>
#include <hidboot.h>
#include "innext_snes.h"
#include "debug.h"
#include "timer.h"

#if USE_SERIAL
void debug(const char *str)
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

#if defined(JOY1_UP) || defined(JOY2_UP)

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

#ifdef JOY1_UP
static USBHub HubJoy1(&Usb);
static HIDUniversal HidUniJoy1(&Usb);
static iNNEXT innext1(JOY1_UP,JOY1_DOWN,JOY1_LEFT,JOY1_RIGHT,JOY1_FIRE,JOY1_POTX,JOY1_POTY);
static iNNEXTparser innext_parser1(&innext1);
#endif
#ifdef JOY2_UP
static USBHub HubJoy2(&Usb);
static HIDUniversal HidUniJoy2(&Usb);
static iNNEXT innext2(JOY2_UP,JOY2_DOWN,JOY2_LEFT,JOY2_RIGHT,JOY2_FIRE,JOY2_POTX,JOY2_POTY);
static iNNEXTparser innext_parser2(&innext2);
#endif

#endif

////////////////////////////////////////////////////////////////////

#if defined(MOUSE1_UP) || defined(MOUSE2_UP)

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

#if defined(MOUSE1_UP)
static USBHub HubMouse1(&Usb);
HIDBoot<USB_HID_PROTOCOL_MOUSE> HidMouse1(&Usb);
Commodore1351 mouse1(1, MOUSE1_UP, MOUSE1_DOWN, MOUSE1_LEFT, MOUSE1_RIGHT, MOUSE1_FIRE, MOUSE1_POTX, MOUSE1_POTY);
#endif
#if defined(MOUSE2_UP)
static USBHub HubMouse2(&Usb);
HIDBoot<USB_HID_PROTOCOL_MOUSE> HidMouse2(&Usb);
Commodore1351 mouse2(2, MOUSE2_UP, MOUSE2_DOWN, MOUSE2_LEFT, MOUSE2_RIGHT, MOUSE2_FIRE, MOUSE2_POTX, MOUSE2_POTY);
#endif

static void mouse1_irq();
static void mouse2_irq();

static void
mouse1_irq()
{
#if defined(MOUSE1_UP)
  mouse1.irq();
#endif
}

static void
mouse2_irq()
{
#if defined(MOUSE2_UP)
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

  if (Usb.Init() == -1)
    {
      debug("! USB init");
    }

  delay(200);

#if defined(MOUSE1_UP)
  if (! HidMouse1.SetReportParser(0, &mouse1))
    {
      debug("!m1");
    }
#endif
#if defined(MOUSE2_UP)
  if (! HidMouse2.SetReportParser(0, &mouse2))
    {
      debug("!m2");
    }
#endif
#ifdef JOY1_UP
  if (! HidUniJoy1.SetReportParser(1, &innext_parser1))
    {
      debug("!joy1");
    }
#endif
#ifdef JOY2_UP
  if (! HidUniJoy2.SetReportParser(2, &innext_parser2))
    {
      debug("!joy2");
    }
#endif
}

// the loop function runs over and over again forever
void loop()
{
  Usb.Task();
  timer.tick();
}
