// -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil; mode: c++ -*-

#include "debug.h"

#if USE_SERIAL
void debugs(const char *str)
{
  Serial.print(str);
}

void debugv(uint8_t v, const uint8_t mode)
{
  static const char *hexdigit = "0123456789abcdef";
  char buf[4];
  if (mode == HEX)
    {
      buf[0] = hexdigit[v >> 4];
      buf[1] = hexdigit[v & 15];
      buf[2] = 0;
      Serial.print(buf);
    }
  else
    {
      buf[3] = 0;
      buf[2] = hexdigit[v % 10];
      if (v < 10)
        {
          Serial.print(buf+2);
          return;
        }
      v /= 10;
      buf[1] = hexdigit[v % 10];
      if (v < 10)
        {
          Serial.print(buf+1);
          return;
        }
      buf[0] = hexdigit[v % 10];
      Serial.print(buf);
    }
}
#endif
