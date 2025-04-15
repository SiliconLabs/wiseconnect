/*
 * bg_compat.c
 *
 *  Created on: 9.4.2015
 *      Author: jeknaapp
 */
#include "bg_compat.h"

uint32_t bg_compat_reverse32(uint32_t x)
{
  x = ((x & 0x55555555) << 1) | ((x & 0xAAAAAAAA) >> 1);
  x = ((x & 0x33333333) << 2) | ((x & 0xCCCCCCCC) >> 2);
  x = ((x & 0x0F0F0F0F) << 4) | ((x & 0xF0F0F0F0) >> 4);
  x = ((x & 0x00FF00FF) << 8) | ((x & 0xFF00FF00) >> 8);
  x = ((x & 0x0000FFFF) << 16) | ((x & 0xFFFF0000) >> 16);
  return x;
}

uint32_t bg_compat_reverse32_bytes(uint32_t x)
{
  uint32_t r;
  r = (x >> 24) & 0x000000ff;
  r |= (x >> 8) & 0x0000ff00;
  r |= (x << 8) & 0x00ff0000;
  r |= (x << 24) & 0xff000000;

  return r;
}

//Weak helper functions to hook to external test libraries
WEAK void bg_compat_int_disable(uint8_t *state, const char *file, int line)
{
  (void)state;
  (void)file;
  (void)line;
}

WEAK void bg_compat_int_enable(uint8_t *state, const char *file, int line)
{
  (void)state;
  (void)file;
  (void)line;
}

WEAK bool bg_compat_int_disabled()
{
  return false;
}

WEAK bool bg_compat_int_in_irq()
{
  return false;
}
