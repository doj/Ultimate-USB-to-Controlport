#pragma once
#include "config.h"

#if USE_SERIAL
void debugs(const char*);
void debugv(uint8_t, uint8_t mode = HEX);
#define debug(str) debugs(str) // todo find out how to use PSTR()
#else
#define debug(str)
#define debugs(str)
inline void debugv(uint8_t, uint8_t mode = HEX) {(void)mode;}
#endif
