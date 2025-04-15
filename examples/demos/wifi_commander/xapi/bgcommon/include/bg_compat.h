/***************************************************************************/ /**
 * @brief Compatibility macros for bgcommon
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#ifndef COMMON_INCLUDE_BG_COMPAT_H_
#define COMMON_INCLUDE_BG_COMPAT_H_
#include <stdint.h>
#include <stdbool.h>

/*
   IAR requires that macros are given before variable definition.
   For example:
   WEAK void func() {}
   PACKSTRUCT(struct foo { int bar; });
   ALIGNED(256) struct foo bar;
 */

#if defined(__IAR_SYSTEMS_ICC__)
/* IAR ICC */
#define __STRINGIFY(a) #a
#ifndef ALIGNED
#define ALIGNED(alignment) _Pragma(__STRINGIFY(data_alignment = alignment))
#endif
#ifndef PACKSTRUCT
#define PACKSTRUCT(decl) __packed decl
#endif
#define WEAK        __weak
#define KEEP_SYMBOL __root
#ifdef NDEBUG
#define OPTIMIZE_SPEED _Pragma(__STRINGIFY(optimize = speed high))
#else
/* Increasing optimization is not allowed */
#define OPTIMIZE_SPEED
#endif
#elif defined(__GNUC__)
/* GNU GCC */
#ifndef ALIGNED
#define ALIGNED(alignment) __attribute__((aligned(alignment)))
#endif
#ifndef PACKSTRUCT
#if defined(_WIN32)
#define PACKSTRUCT(decl) decl __attribute__((packed, gcc_struct))
#else
#define PACKSTRUCT(decl) decl __attribute__((packed))
#endif
#endif
#define WEAK        __attribute__((weak))
#define KEEP_SYMBOL __attribute__((used))
#ifdef __clang__
// clang does not support optimize attribute
#define OPTIMIZE_SPEED
#else
#define OPTIMIZE_SPEED __attribute__((optimize("O3")))
#endif
#else
/* Unknown */
#ifndef PACKSTRUCT
#define PACKSTRUCT(decl) decl
#endif
#define WEAK
#define KEEP_SYMBOL
#endif

#ifndef EFR32
uint32_t bg_compat_reverse32(uint32_t x);

uint32_t bg_compat_reverse32_bytes(uint32_t x);

void bg_compat_int_disable(uint8_t *state, const char *file, int line);
void bg_compat_int_enable(uint8_t *state, const char *file, int line);
bool bg_compat_int_disabled();
bool bg_compat_int_in_irq();

//declare interrupt state
#define BG_INT_DECLARE_STATE uint8_t bg_compat_irq_state
//disable interrupts
#define BG_INT_DISABLE() bg_compat_int_disable(&bg_compat_irq_state, __FILE__, __LINE__)
//enable interrupts
#define BG_INT_ENABLE() bg_compat_int_enable(&bg_compat_irq_state, __FILE__, __LINE__)
//check if IRQ is disabled
#define BG_INT_DISABLED() bg_compat_int_disabled()
//check if in IRQ context
#define BG_INT_IN_IRQ() bg_compat_int_in_irq()

//Stub implementations, mainly for running utests
#define BG_COUNT_LEADING_ZEROS(bits) __builtin_clz(bits)

#define BG_INVERT_WORD(bits) bg_compat_reverse32(bits)

#define BG_REVERSE_BYTES32(bits) bg_compat_reverse32_bytes(bits)

#define BG_ATOMIC_BIT_SET(bitmask, bit) \
  {                                     \
    *bitmask |= BIT(bit);               \
  }                                     \
  while (0)

#define BG_ATOMIC_BIT_CLR(bitmask, bit) \
  do {                                  \
    *bitmask &= ~BIT(bit);              \
  } while (0)

#define BG_BIT_SET_EXCLUSIVE(bitmask, bitset) \
  do {                                        \
    *bitmask |= bitset;                       \
  } while (0)

#define BG_BIT_CLR_EXCLUSIVE(bitmask, bitset) \
  do {                                        \
    *bitmask &= ~(bitset);                    \
  } while (0)

#define BG_SWAP_EXCLUSIVE(bitmask, newbits) \
  unsigned long oldbits;                    \
  oldbits    = *(bitmask);                  \
  *(bitmask) = newbits;                     \
  return oldbits;

#else

// EFR platform

#include "cmsis_compiler.h"
#include "em_core_generic.h"
#define BG_INT_DECLARE_STATE CORE_DECLARE_IRQ_STATE
#define BG_INT_DISABLE()     CORE_ENTER_ATOMIC()
#define BG_INT_ENABLE()      CORE_EXIT_ATOMIC()
#define BG_INT_DISABLED()    CORE_IRQ_DISABLED()
#define BG_INT_IN_IRQ()      CORE_IN_IRQ_CONTEXT()

#define BG_COUNT_LEADING_ZEROS(bits) __CLZ(bits)

#define BG_INVERT_WORD(bits) __RBIT(bits)

#define BG_REVERSE_BYTES32(bytes) __REV(bytes)

#define BG_ATOMIC_BIT_SET(bitmask, bit) BG_BIT_SET_EXCLUSIVE(bitmask, BIT(bit))

#define BG_ATOMIC_BIT_CLR(bitmask, bit) BG_BIT_CLR_EXCLUSIVE(bitmask, BIT(bit))

#define BG_BIT_SET_EXCLUSIVE(bitmask, setbits) \
  do {                                         \
  } while (__STREXW(__LDREXW(bitmask) | (setbits), bitmask))

#define BG_BIT_CLR_EXCLUSIVE(bitmask, setbits) \
  do {                                         \
  } while (__STREXW(__LDREXW(bitmask) & ~(setbits), bitmask))

#define BG_SWAP_EXCLUSIVE(bitmask, newbits) \
  unsigned long oldbits;                    \
  do {                                      \
    oldbits = __LDREXW(bitmask);            \
  } while (__STREXW(newbits, bitmask));     \
  return oldbits;

#endif

#ifndef PACKED
#ifdef __GNUC__ //GNU Packed definition
#define PACKED __attribute__((packed))
#elif defined(__CWCC__) //Codewarrior
#define PACKED
#pragma options align = packed
#elif defined(__IAR_SYSTEMS_ICC__)
#define PACKED
#else
#define PACKED
#endif
#endif

#ifndef ALIGNED_WORD_SIZE
#define ALIGNED_WORD_SIZE (4)
#endif

#ifndef PACKSTRUCT
#ifdef __GNUC__
#ifdef _WIN32
#define PACKSTRUCT(decl) decl __attribute__((__packed__, gcc_struct))
#else
#define PACKSTRUCT(decl) decl __attribute__((__packed__))
#endif
#define ALIGNED __attribute__((aligned(ALIGNED_WORD_SIZE)))
#elif defined(__IAR_SYSTEMS_ICC__)
#define PACKSTRUCT(decl) __packed decl
#define ALIGNED
#elif _MSC_VER //msvc
#define PACKSTRUCT(decl) __pragma(pack(push, 1)) decl __pragma(pack(pop))
#define ALIGNED
#else
#define PACKSTRUCT(a) a PACKED
#endif
#endif

#ifndef WEAK
#if defined(__IAR_SYSTEMS_ICC__)
/* IAR ICC*/
#define WEAK __weak
#elif defined(__GNUC__)
/* GNU GCC */
#define WEAK __attribute__((weak))
#else
/* Unknown */
#define WEAK
#endif
#endif

#endif /* COMMON_INCLUDE_BG_COMPAT_H_ */
