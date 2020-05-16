// -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil; mode: c++ -*-

#include "USBKeyboard.h"
#include "debug.h"
#include <usbhid.h>

bool
contains(const uint8_t *arr, const uint8_t sc)
{
  for(int i = 0; i < 6; ++i)
    {
      if (arr[i] == sc)
	return true;
    }
  return false;
}

void
USBKeyboard::parse(const uint8_t *buf, const uint8_t len, USBHID *hid, const uint8_t bAddress, const uint8_t epAddress)
{
  m_hid = hid;
  (void)bAddress;
  (void)epAddress;

#if 0
  for(int i = 0; i < len; ++i)
    {
      debugs(" ");
      debugv(buf[i]);
    }
  debugnl();
#endif

  if (len < 8)
    return;

  for(int i = 0; i < 6; ++i)
    {
      const uint8_t k = m_currentKeys[i];
      if (k == 0)
	continue;
      if (! contains(buf+2, k))
	key(k, false);
    }

  for(int i = 2; i < len; ++i)
    {
      const uint8_t k = buf[i];
      if (k == 0)
	continue;
      if (! contains(m_currentKeys, k))
	key(k, true);
    }

  memcpy(m_currentKeys, buf+2, sizeof(m_currentKeys));
}

void
USBKeyboard::key(const uint8_t sc, const bool down)
{
  uint8_t but = 0;
  switch(sc)
    {
    case 0x52: // cursor up
    case 0x60: // num 8
    case 0x1a: // w
    case 0x0c: // i
      OnY(down ? 0 : 0x7f);
      return;

    case 0x5a: // num 2
    case 0x51: // cursor down
    case 0x16: // s
    case 0x0e: // k
      OnY(down ? 0xff : 0x7f);
      return;

    case 0x50: // cursor left
    case 0x5c: // num 4
    case 0x04: // a
    case 0x0d: // j
      OnX(down ? 0 : 0x7f);
      return;

    case 0x4f: // cursor right
    case 0x5e: // num 6
    case 0x07: // d
    case 0x0f: // l
      OnX(down ? 0xff : 0x7f);
      return;

    case 0x62: // ins
    case 0x2c: // space
    case 0x35: // ~
    case 0x28: // enter
    case 0x3a: // F1
    case 0x05: // b
      but = m_but_b;
      break;

    case 0x1b: // x
      but = m_but_x;
      break;

    case 0x1c: // y
      but = m_but_y;
      break;

    case 0x3b: // F2
      but = BUT_SELECT;
      break;

    case 0x3c: // F3
      but = BUT_START;
      break;

    default:
      return;
  }
  if (down)
    OnButtonDn(but);
  else
    OnButtonUp(but);
}

void
USBKeyboard::setLED()
{
  if (! m_hid)
    return;
  m_hid->SetReport(/*ep=*/0,
		   /*iface=*/0/*hid->GetIface()*/,
		   /*report_type=*/2,
		   /*report_id=*/0,
		   /*nbytes=*/1,
		   &m_led);
}

void
USBKeyboard::fireCB(bool on)
{
  if (on)
    m_led |= 1;
  else
    m_led &= ~1;
  setLED();
}
