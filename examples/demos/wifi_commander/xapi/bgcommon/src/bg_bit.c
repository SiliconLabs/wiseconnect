/*
 * bg_bit.c
 *
 *  Created on: 9.4.2015
 *      Author: jeknaapp
 */

#include "bg_bit.h"

unsigned int bg_bit_popcount(bg_bitmap_t bitmap)
{
  // hamming weight algorithm from Wikipedia (popcount_3)
  bitmap = bitmap - ((bitmap >> 1) & 0x55555555);
  bitmap = (bitmap & 0x33333333) + ((bitmap >> 2) & 0x33333333);
  return (((bitmap + (bitmap >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}
