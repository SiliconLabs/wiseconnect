/*
 * bg_bit.h
 *
 *  Created on: 9.4.2015
 *      Author: jeknaapp
 */

#ifndef COMMON_INCLUDE_BG_BIT_H_
#define COMMON_INCLUDE_BG_BIT_H_
#include <stdint.h>
#include <stdbool.h>
#include "bg_compat.h"

typedef uint32_t bg_bitmap_t;
typedef uint8_t bg_bit_t;

/**
 * Maximum bit which can be used in bitmap
 */
#define BG_BIT_MAX (sizeof(bg_bitmap_t) * 8 - 1)

/**
 * Invalid bit index
 */
#define BG_BIT_INVALID (BG_BIT_MAX + 1)

/**
 * Convert bit index to bit mask
 * @param bit bit to convert
 * @return bitmask
 */
#define BIT(bit) (1U << (bit))

/**
 * Check if any bit is set in bitmap
 *
 * @param bitmap bitmap to check against
 * @param bits bitmap of bits to compare
 * @return bg_bitmap_t bitmap of matching bits
 */
static inline bg_bitmap_t bg_bitmap_isset(bg_bitmap_t bitmap, bg_bitmap_t bits)
{
  return bitmap & bits;
}

/**
 * Check if bit is set in bitmask
 * @param bitmask bitmask to check
 * @param bit bit index to check
 * @return nonzero if bit is set
 */
static inline bg_bitmap_t bg_bit_isset(bg_bitmap_t bitmask, bg_bit_t bit)
{
  return bitmask & BIT(bit);
}

/**
 * Check bitmap state
 *
 * @param bitmask bitmap to check against
 * @param set these bits must be 1 in bitmap
 * @param clr these bits must be 0 in bitmap, overlapping bits with set are ignored
 *
 * @return true if only bits in set-variable are 1
 */
static inline bool bg_bit_check_mask(bg_bitmap_t bitmap, bg_bitmap_t set, bg_bitmap_t clr)
{
  return (bitmap & (set | clr)) == set;
}

/**
 * Set bit in bitmask
 * @param bitmask bitmask to modify
 * @param bit bit index to set
 */
static inline void bg_bit_set(bg_bitmap_t *bitmask, bg_bit_t bit)
{
  //cstat !ATH-shift-bounds
  *bitmask |= BIT(bit);
}

/**
 * Clear bit in bitmask
 * @param bitmask bitmask to modify
 * @param bitnum bit index to clear
 */
static inline void bg_bit_clr(bg_bitmap_t *bitmask, bg_bit_t bit)
{
  *bitmask &= ~BIT(bit);
}

/**
 * Find lowest bit not set in bitmask
 * @param bitmap
 * @return index, or BG_BIT_INVALID if not found
 */
static inline bg_bit_t bg_bit_lowest_not_set(bg_bitmap_t bitmap)
{
  return BG_COUNT_LEADING_ZEROS(BG_INVERT_WORD(~bitmap));
}

/**
 * Find lowest bit set in bitmask
 * @param bitmap
 * @return index, or BG_BIT_INVALID if not found
 */
static inline bg_bit_t bg_bit_lowest_set(bg_bitmap_t bitmap)
{
  return BG_COUNT_LEADING_ZEROS(BG_INVERT_WORD(bitmap));
}

/**
 * Set bit in bitmask atomically
 * @param bitmask bitmask to modify
 * @param bit bit index to set
 */
static inline void bg_bit_set_atomic(bg_bitmap_t *bitmask, bg_bit_t bit)
{
  BG_ATOMIC_BIT_SET(bitmask, bit);
}

/**
 * Clear bit in bitmask atomically
 * @param bitmask bitmask to modify
 * @param bitnum bit index to clear
 */
static inline void bg_bit_clr_atomic(bg_bitmap_t *bitmask, bg_bit_t bit)
{
  BG_ATOMIC_BIT_CLR(bitmask, bit);
}

/**
 * Set multiple bits at same time using exclusive access
 * @param bitmap bitmap to set
 * @param bits to set
 */
static inline void bg_bit_set_exclusive(bg_bitmap_t *bitmap, bg_bitmap_t bits)
{
  BG_BIT_SET_EXCLUSIVE((uint32_t *)bitmap, (uint32_t)bits);
}

/**
 * Clear multiple bits at same time using exclusive access
 * @param bitmap bitmap to clear
 * @param bits to clear
 */
static inline void bg_bit_clr_exclusive(bg_bitmap_t *bitmap, bg_bitmap_t bits)
{
  BG_BIT_CLR_EXCLUSIVE((uint32_t *)bitmap, (uint32_t)bits);
}

/**
 * Swap bitmap and return old using exclusive access
 * @param bitmap bitmap to exchange
 * @param bits new bitmap to set to
 * @return old bitmap
 */
static inline bg_bitmap_t bg_bit_swap_exclusive(bg_bitmap_t *bitmap, bg_bitmap_t bits)
{
  BG_SWAP_EXCLUSIVE((uint32_t *)bitmap, (uint32_t)bits);
}

/**
 * Calculate the number of set bits from given bitmap
 * @param examine bitmap
 * @return number of set bits
 */
unsigned int bg_bit_popcount(bg_bitmap_t bitmap);

#endif /* COMMON_INCLUDE_BG_BIT_H_ */
