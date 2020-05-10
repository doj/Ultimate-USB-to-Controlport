// -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil; mode: c++ -*-

#pragma once
#include <usbhid.h>

class iNNEXTevents
{
public:
  virtual void OnX(uint8_t x) = 0;
  virtual void OnY(uint8_t y) = 0;
  virtual void OnButtonUp(uint8_t but_id) = 0;
  virtual void OnButtonDn(uint8_t but_id) = 0;
};

class iNNEXTparser : public HIDReportParser
{
  iNNEXTevents *joyEvents;
  uint8_t oldX;
  uint8_t oldY;
  uint16_t oldButtons;

public:
  iNNEXTparser(iNNEXTevents *evt);
  void setEventHandler(iNNEXTevents *e) { joyEvents = e; }
  void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) override;
};
