// -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil; mode: c++ -*-

#pragma once

#include <usbhid.h>
#include "USBController.h"

void swapControlPorts();

class ControlPortDevice : public HIDReportParser
{
private:
  static const uint8_t JOYSTICK1 = 0x01;
  static const uint8_t JOYSTICK2 = 0x10;
  static const uint8_t MOUSE1 = 0x02;
  static const uint8_t MOUSE2 = 0x20;
  static const uint8_t KEYBOARD1 = 0x04;
  static const uint8_t KEYBOARD2 = 0x40;
  static uint8_t s_used;

  const uint8_t m_num;

  ControlPortDeviceHandler *m_handler = NULL;

  /// JOYSTICK1..KEYBOARD2
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
  void initJoystick(const USBController::type_t t);
  void initKeyboard();
  void swapPort();

#if defined(USB_HOST_SHIELD_VERSION) && (USB_HOST_SHIELD_VERSION >= 0x020303)
  void Release() override;
  void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf, uint8_t bAddress, uint8_t epAddress) override;
#else
  void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) override;
#endif

  void pot(const uint8_t pin, const uint8_t state) const;
  void joystick(const uint8_t pin, const uint8_t state) const;
  void send(const uint8_t *buf, const uint16_t len);

private:
  void initPins();
};
