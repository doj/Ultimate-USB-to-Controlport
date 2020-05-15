// -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil; mode: c++ -*-

#pragma once
#include <stdint.h>

class ControlPortDevice;

class ControlPortDeviceHandler
{
protected:
  ControlPortDevice *m_cpd;
public:
  explicit ControlPortDeviceHandler(ControlPortDevice *cpd) :
    m_cpd(cpd)
  {}
  virtual ~ControlPortDeviceHandler() {}
  virtual void parse(const uint8_t *buf, const uint8_t len) = 0;
};
