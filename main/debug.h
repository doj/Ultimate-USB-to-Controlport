#pragma once
#include "config.h"

#if USE_SERIAL
void debug(const char*);
void debugv(uint8_t, uint8_t mode = HEX);
#else
inline void debug(const char*) {}
inline void debugv(uint8_t, uint8_t mode = 0) {(void)mode;}
#endif
