/***************************************************************************/ /**
 * @file  sl_si91x_wdt_config.h
 * @brief Watchdog-Timer configuration file.
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licenser of this software is Silicon Laboratories Inc.
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
#ifndef SL_SI91X_WDT_CONFIG_H
#define SL_SI91X_WDT_CONFIG_H

#include "sl_si91x_watchdog_timer.h"
#include "sl_si91x_wdt_manager.h"
#include "sl_component_catalog.h"

// <<< Use Configuration Wizard in Context Menu >>>

//  <e> WatchDog Timer UC Configuration
//  <i> Enable: Peripheral configuration is taken straight from the
//  configuration set in the universal configuration (UC).
//  <i> Disable: If the application demands it to be modified during runtime, use the
//  sl_si91x_watchdog_set_configuration API to modify the peripheral
//  configuration.
//  <i> Default: 1
#define WDT_UC 1

// <h> Watchdog Timer Configuration

// <o SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_PERIOD> WDT Timeout Interval
//   <SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_1=> 0.0625 ms
//   <SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_2=> 0.125 ms
//   <SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_3=> 0.25 ms
//   <SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_4=> 0.5 ms
//   <SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_5=> 1 ms
//   <SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_6=> 2 ms
//   <SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_7=> 4 ms
//   <SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_8=> 8 ms
//   <SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_9=> 16 ms
//   <SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_10=> 32 ms
//   <SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_11=> 64 ms
//   <SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_12=> 128 ms
//   <SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_13=> 256 ms
//   <SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_14=> 512 ms
//   <SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_15=> 1024 ms
//   <SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_16=> 2048 ms
//   <SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_17=> 4096 ms
//   <SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_18=> 8192 ms
//   <SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_19=> 16384 ms
//   <SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_20=> 32768 ms
//   <SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_21=> 65536 ms
//   <SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_22=> 131072 ms
//   <SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_23=> 262144 ms
//   <SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_24=> 524288 ms
//   <SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_25=> 1048576 ms
//   <SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_26=> 2097152 ms
//   <SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_27=> 4194304 ms
//   <SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_28=> 8388608 ms
//   <SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_29=> 16777216 ms
//   <SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_30=> 33554432 ms
//   <SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_31=> 67108864 ms
// <i> Selection of Interrupt Time
#define SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_PERIOD SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_20

// </h>
// </e>
// <<< end of configuration section >>>

#endif // SL_SI91X_WDT_CONFIG_H
