// -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil; mode: c++ -*-

#pragma once
#include "USBController.h"

class USBKeyboard : public USBController
{
  USBHID *m_hid = NULL;
  uint8_t m_led = 0;
  uint8_t m_currentKeys[6];
  void setLED();
  void fireCB(bool on) override;

public:
  USBKeyboard(uint8_t num, ControlPortDevice *cpd) :
    USBController(num,cpd,Generic)
  {
    memset(m_currentKeys, 0, sizeof(m_currentKeys));
  }
  void parse(const uint8_t *buf, const uint8_t len, USBHID *hid, const uint8_t bAddress, const uint8_t epAddress) override;
  void key(const uint8_t sc, const bool down);
};
