// -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil; mode: c++ -*-

#pragma once
#include "USBController.h"

class USBKeyboard : public USBController
{
  uint8_t m_currentKeys[6];
public:
  USBKeyboard(uint8_t num, ControlPortDevice *cpd) :
    USBController(num,cpd)
  {
    memset(m_currentKeys, 0, sizeof(m_currentKeys));
    init();
  }
  void parse(const uint8_t *buf, const uint8_t len) override;
  void key(const uint8_t sc, const bool down);
};
