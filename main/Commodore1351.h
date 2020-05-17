// -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil; mode: c++ -*-

#pragma once
#include "ControlPortDeviceHandler.h"

class Commodore1351 : public ControlPortDeviceHandler
{
  struct MouseData {
    struct button_t {
      uint8_t left : 1;
      uint8_t right : 1;
      uint8_t middle : 1;
      uint8_t side_left : 1;
      uint8_t side_right : 1;
      uint8_t unused : 3;
    } button;
    int8_t dX;
    int8_t dY;
    int8_t wUD; ///< up/down wheel for regular mouse
    int8_t wLR; ///< left/right wheel for regular mouse
    int8_t unknown; ///< used by apple mouse
  };
  static_assert(sizeof(MouseData) == 6, "unexpected MouseData size");

  const uint8_t m_num;
  uint8_t m_x = 127;
  uint8_t m_y = 127;
  MouseData::button_t m_oldButton;

  void init();
  void move(const int8_t x, const int8_t y);
public:
  Commodore1351(uint8_t num, ControlPortDevice *cpd) :
    ControlPortDeviceHandler(cpd),
    m_num(num)
  {
    init();
  }
  ~Commodore1351();
  void parse(const uint8_t *buf, const uint8_t len, USBHID *hid, const uint8_t bAddress, const uint8_t epAddress) override;
};
