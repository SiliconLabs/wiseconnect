/***************************************************************************/ /**
 * @file sl_si91x_psram_retained_data.h
 * @brief PSRAM retained-data placement (LTO-safe) for SiWx917
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef SL_SI91X_PSRAM_RETAINED_DATA_H_
#define SL_SI91X_PSRAM_RETAINED_DATA_H_

/*
 * SL_SI91X_RETAINED_DATA / SL_SI91X_RETAINED_BSS
 * ----------------------------------------------
 * Tag a global/static variable that must always live in internal RAM (never in
 * PSRAM). Required for variables accessed during the sleep-entry / wakeup-
 * recovery flow, when PSRAM is not yet operational (QSPI/PSRAM/PLL/power/clock
 * driver state, wakeup context buffers).
 *
 *   SL_SI91X_RETAINED_DATA -> initialized variables  (.data  -> ".retained_data")
 *   SL_SI91X_RETAINED_BSS  -> zero-initialized vars   (.bss   -> ".retained_bss")
 *
 * Why a fixed section name collected BY NAME (not the old file-based rule):
 *   The previous approach kept these in RAM via a per-object-file linker rule
 *   (the "psram_ram_data_files" list). That is NOT LTO-safe: with LTO the
 *   compiler merges translation units, the per-file .data boundary disappears,
 *   and the variables fall back into PSRAM -> sleep/wakeup failures. Tagging a
 *   variable with an explicit, stable section name that the linker collects with
 *   *(.retained_data*) is object-file-independent, so it survives LTO on both
 *   GCC and LLVM. 'used' keeps the symbol at compile/LTO time; the linker
 *   script wraps these sections in KEEP() so --gc-sections cannot discard them.
 *
 * Gating: only active when data is actually placed in PSRAM
 * (DATA_SEGMENT_IN_PSRAM). Otherwise these expand to nothing and the variable
 * stays in its normal RAM section - exactly matching the old behavior, which
 * only moved .data to RAM under the same condition.
 *
 * This header is provided by the "psram_core" and "data_segment_in_psram"
 * components (include path is added when either is installed).
 */
#if defined(DATA_SEGMENT_IN_PSRAM) && defined(__GNUC__)
#define SL_SI91X_RETAINED_DATA __attribute__((used, section(".retained_data")))
#define SL_SI91X_RETAINED_BSS  __attribute__((used, section(".retained_bss")))
#else
#define SL_SI91X_RETAINED_DATA
#define SL_SI91X_RETAINED_BSS
#endif

#endif // SL_SI91X_PSRAM_RETAINED_DATA_H_
