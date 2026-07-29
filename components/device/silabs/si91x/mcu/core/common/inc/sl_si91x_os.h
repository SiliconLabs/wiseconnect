/***************************************************************************/ /**
 * @file  sl_si91x_os.h
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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
#pragma once

#ifdef __FREERTOS_OS_WISECONNECT
#include "FreeRTOSConfig.h"
#endif

#ifdef __FREERTOS_OS_WISECONNECT
/*
 * Default NVIC priority used by the Si91x device init code when an OS is
 * enabled. For FreeRTOS, keep IRQ priorities at or below the max syscall
 * priority to avoid configASSERT() failures when calling ISR-safe APIs.
 */
#define SL_SI91X_OS_DEFAULT_IRQ_PRIORITY (configMAX_SYSCALL_INTERRUPT_PRIORITY >> (8 - configPRIO_BITS))
#endif

#ifdef __FREERTOS_OS_WISECONNECT
#define SL_OS_SYSTEM_TICK_RATE configTICK_RATE_HZ
#endif

#ifdef __ZEPHYR__
#define SL_OS_SYSTEM_TICK_RATE CONFIG_SYS_CLOCK_TICKS_PER_SEC
#endif

/* Fallback for non-FreeRTOS OS ports (can be overridden by the OS layer). */
#ifndef SL_SI91X_OS_DEFAULT_IRQ_PRIORITY
#define SL_SI91X_OS_DEFAULT_IRQ_PRIORITY (0U)
#endif
