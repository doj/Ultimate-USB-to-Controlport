// -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil; mode: c++ -*-

#pragma once

#include <usbhid.h>
#include "ControlPortDeviceHandler.h"

class ControlPortDevice : public HIDReportParser
{
private:
  static const uint8_t JOYSTICK1 = 1;
  static const uint8_t JOYSTICK2 = 2;
  static const uint8_t MOUSE1 = 4;
  static const uint8_t MOUSE2 = 8;
  static uint8_t s_used;

  const uint8_t m_num;
  ControlPortDeviceHandler *m_handler = NULL;
  uint8_t m_used = 0;
public:
  uint8_t m_pinUp;
  uint8_t m_pinDown;
  uint8_t m_pinLeft;
  uint8_t m_pinRight;
  uint8_t m_pinFire;
  uint8_t m_pinPotX;
  uint8_t m_pinPotY;

  ControlPortDevice(uint8_t num) :
    m_num(num)
  {
  }

  void initMouse();
  void initJoystick();

#if defined(USB_HOST_SHIELD_VERSION) && (USB_HOST_SHIELD_VERSION >= 0x010303)
  void Release() override;
#endif
  void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) override;

  void pot(const uint8_t pin, const uint8_t state) const;
  void joystick(const uint8_t pin, const uint8_t state) const;
};
