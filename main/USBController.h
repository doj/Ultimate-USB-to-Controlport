// -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil; mode: c++ -*-

#pragma once
#include "ControlPortDeviceHandler.h"
#include "timer.h"
#include "config.h"

class USBController : public ControlPortDeviceHandler
{
#if USE_SERIAL
  int8_t m_x = 0x7f;
  int8_t m_y = 0x7f;
  void debugAxes() const;
#endif

  const uint8_t m_num;

  /// bit mask of button states.
  /// bit off: button not pressed.
  /// bit on:  button pressedn.
  uint16_t m_button_state = 0;

  Timer_t::Task m_autoFireTask = 0;
  uint8_t m_autoFireState = 0;

  uint8_t m_autoFireAfreq = AUTO_FIRE_A_FREQ;
  uint8_t m_autoFireYfreq = AUTO_FIRE_Y_FREQ;

  uint8_t m_lefty = 0;

  uint16_t m_oldButtons = 0;
  uint8_t m_oldX = 0;
  uint8_t m_oldY = 0;

  uint8_t direction(uint8_t pin);

protected:
  ///@name SNES joystick button names
  ///@{
  static const uint8_t BUT_X = 1;  ///< Sony triangle
  static const uint8_t BUT_A = 2;  ///< Sony circle
  static const uint8_t BUT_B = 3;  ///< Sony cross
  static const uint8_t BUT_Y = 4;  ///< Sony square
  static const uint8_t BUT_L2 = 5; ///< NES L
  static const uint8_t BUT_R2 = 6; ///< NES R
  static const uint8_t BUT_L1 = 7; ///< only Sony
  static const uint8_t BUT_R1 = 8; ///< only Sony
  static const uint8_t BUT_SELECT = 9;
  static const uint8_t BUT_START  = 10;
  ///@}

  uint8_t m_but_a = BUT_A;
  uint8_t m_but_b = BUT_B;
  uint8_t m_but_x = BUT_X;
  uint8_t m_but_y = BUT_Y;

public:
  USBController(uint8_t num, ControlPortDevice *cpd) :
    ControlPortDeviceHandler(cpd),
    m_num(num)
  {
    init();
  }
  ~USBController();
  void init() const;
  void OnX(uint8_t x);
  void OnY(uint8_t y);
  void OnButtonUp(uint8_t but_id);
  void OnButtonDn(uint8_t but_id);
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

  void parse(const uint8_t *buf, const uint8_t len) override;
};
