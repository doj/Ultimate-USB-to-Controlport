#pragma once
#include "config.h"

#if USE_SERIAL
/// print a string @p str to the serial console. @p str is located in SRAM.
void debugs(const char* str);
/// print a string @p str to the serial console. @p str is located in Flash (progmem).
void debugf(const char*);
/// print a value @p v to the serial console.
/// @param mode if HEX, print as 2 hex-digits;
///             if DEC, print as 1-3 digit decimal.
void debugu(uint8_t v, uint8_t mode = HEX);
/// print a decimal value @p v to the serial console.
void debugi(int8_t v);
/// print a newline character to the serial console.
void debugnl();
/// print a hexadecimal value @p u to the serial console.
void debugus(uintptr_t u);
/// print a pointer value @p p to the serial console.
inline void debugp(const void *p) { debugus((uintptr_t)p); }
/// print a string @p str to the serial console. @p str is located in Flash (progmem).
#define debug(str) debugf(PSTR(str))

#else
#define debug(str)
#define debugs(str)
#define debugf(str)
inline void debugnl() {}
inline void debugu(uint8_t, uint8_t mode = 0) {(void)mode;}
#define debugi(v)
#define debugus(v)
#define debugp(v)
#endif
