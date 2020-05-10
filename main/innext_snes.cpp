// -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil; mode: c++ -*-

#include "innext_snes.h"

void debug(const char*, int8_t line = 0);
void debug(const uint8_t);

iNNEXTparser::iNNEXTparser(iNNEXTevents *evt) :
  joyEvents(evt),
  oldX(0),
  oldY(0),
  oldButtons(0)
{
}

void
iNNEXTparser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf)
{
  (void)hid;
  (void)is_rpt_id;
  (void)len;

  if (! joyEvents)
    {
      debug("no joyEvents");
      return;
    }

  // debug incoming USB data packets
#if 0
  uint8_t i = 0;
  debug("",0);
  while(i < len && i < 8)
  {
    debug(buf[i++]);
  }
  if (i < len)
  {
    debug("",1);
    while(i < len && i < 16)
    {
      debug(buf[i++]);
    }
  }
  return;
#endif

  if (len < 7)
    {
      debug("usb len:");
      debug(len);
      return;
    }

#define X_INDEX 3
  if (buf[X_INDEX] != oldX)
    {
      oldX = buf[X_INDEX];
      joyEvents->OnX(oldX);
    }

#define Y_INDEX 4
  if (buf[Y_INDEX] != oldY)
    {
      oldY = buf[Y_INDEX];
      joyEvents->OnY(oldY);
    }

#if 0
  // Calling Hat Switch event handler
  const uint8_t hat = (buf[5] & 0xF);
  if (hat != oldHat)
    {
      joyEvents->OnHatSwitch(hat);
      oldHat = hat;
    }
#endif

  uint16_t buttons = (0x0000 | buf[6]);
  buttons <<= 4;
  buttons |= (buf[5] >> 4);
  const uint16_t changes = (buttons ^ oldButtons);

  // Calling Button Event Handler for every button changed
  if (changes)
    {
      for (uint8_t i = 0; i < 0x0C; i++)
        {
          uint16_t mask = (0x0001 << i);

          if ((mask & changes) > 0)
            {
              if ((buttons & mask) > 0)
                joyEvents->OnButtonDn(i + 1);
              else
                joyEvents->OnButtonUp(i + 1);
            }
        }
      oldButtons = buttons;
    }
}
