/***************************************************************************/ /**
 * @file  sl_si91x_watchdog_timer_config.h
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
#ifndef SL_SI91X_WATCHDOG_TIMER_CONFIG_H
#define SL_SI91X_WATCHDOG_TIMER_CONFIG_H

#include "sl_si91x_watchdog_timer.h"

// <<< Use Configuration Wizard in Context Menu >>>

// <e>WatchDog Timer UC Configuration
//  <i> Enable: Peripheral configuration is taken straight from the
//  configuration set in the universal configuration (UC).
//  <i> Disable: If the application demands it to be modified during runtime, use the
//  sl_si91x_watchdog_set_configuration API to modify the peripheral
//  configuration.
//  <i> Default: 1
#define WDT_TIMER_UC 1

// <h>Watchdog Timer Configuration

// <o SL_WDT_SYSTEM_RESET_TIME> System-reset time
//   <TIME_DELAY_2=> 0.125 ms
//   <TIME_DELAY_3=> 0.25 ms
//   <TIME_DELAY_4=> 0.5 ms
//   <TIME_DELAY_5=> 1 ms
//   <TIME_DELAY_6=> 2 ms
//   <TIME_DELAY_7=> 4 ms
//   <TIME_DELAY_8=> 8 ms
//   <TIME_DELAY_9=> 16 ms
//   <TIME_DELAY_10=> 32 ms
//   <TIME_DELAY_11=> 64 ms
//   <TIME_DELAY_12=> 128 ms
//   <TIME_DELAY_13=> 256 ms
//   <TIME_DELAY_14=> 512 ms
//   <TIME_DELAY_15=> 1024 ms
//   <TIME_DELAY_16=> 2048 ms
//   <TIME_DELAY_17=> 4096 ms
//   <TIME_DELAY_18=> 8192 ms
//   <TIME_DELAY_19=> 16384 ms
//   <TIME_DELAY_20=> 32768 ms
//   <TIME_DELAY_21=> 65536 ms
//   <TIME_DELAY_22=> 131072 ms
//   <TIME_DELAY_23=> 262144 ms
//   <TIME_DELAY_24=> 524288 ms
//   <TIME_DELAY_25=> 1048576 ms
//   <TIME_DELAY_26=> 2097152 ms
//   <TIME_DELAY_27=> 4194304 ms
//   <TIME_DELAY_28=> 838860 ms
//   <TIME_DELAY_29=> 16777216 ms
//   <TIME_DELAY_30=> 33554432 ms
//   <TIME_DELAY_31=> 67108864 ms
// <i> Selection of System-reset time
#define SL_WDT_SYSTEM_RESET_TIME TIME_DELAY_17

// <o SL_WDT_INTERRUPT_TIME> Interrupt Time
//   <TIME_DELAY_1=> 0.0625 ms
//   <TIME_DELAY_2=> 0.125 ms
//   <TIME_DELAY_3=> 0.25 ms
//   <TIME_DELAY_4=> 0.5 ms
//   <TIME_DELAY_5=> 1 ms
//   <TIME_DELAY_6=> 2 ms
//   <TIME_DELAY_7=> 4 ms
//   <TIME_DELAY_8=> 8 ms
//   <TIME_DELAY_9=> 16 ms
//   <TIME_DELAY_10=> 32 ms
//   <TIME_DELAY_11=> 64 ms
//   <TIME_DELAY_12=> 128 ms
//   <TIME_DELAY_13=> 256 ms
//   <TIME_DELAY_14=> 512 ms
//   <TIME_DELAY_15=> 1024 ms
//   <TIME_DELAY_16=> 2048 ms
//   <TIME_DELAY_17=> 4096 ms
//   <TIME_DELAY_18=> 8192 ms
//   <TIME_DELAY_19=> 16384 ms
//   <TIME_DELAY_20=> 32768 ms
//   <TIME_DELAY_21=> 65536 ms
//   <TIME_DELAY_22=> 131072 ms
//   <TIME_DELAY_23=> 262144 ms
//   <TIME_DELAY_24=> 524288 ms
//   <TIME_DELAY_25=> 1048576 ms
//   <TIME_DELAY_26=> 2097152 ms
//   <TIME_DELAY_27=> 4194304 ms
//   <TIME_DELAY_28=> 838860 ms
//   <TIME_DELAY_29=> 16777216 ms
//   <TIME_DELAY_30=> 33554432 ms
//   <TIME_DELAY_31=> 67108864 ms
// <i> Selection of Interrupt Time
#define SL_WDT_INTERRUPT_TIME TIME_DELAY_15

// <o SL_WDT_WINDOW_TIME> Window Time
//   <TIME_DELAY_0=> 0.03125 ms
//   <TIME_DELAY_1=> 0.0625 ms
//   <TIME_DELAY_2=> 0.125 ms
//   <TIME_DELAY_3=> 0.25 ms
//   <TIME_DELAY_4=> 0.5 ms
//   <TIME_DELAY_5=> 1 ms
//   <TIME_DELAY_6=> 2 ms
//   <TIME_DELAY_7=> 4 ms
//   <TIME_DELAY_8=> 8 ms
//   <TIME_DELAY_9=> 16 ms
//   <TIME_DELAY_10=> 32 ms
//   <TIME_DELAY_11=> 64 ms
//   <TIME_DELAY_12=> 128 ms
//   <TIME_DELAY_13=> 256 ms
//   <TIME_DELAY_14=> 512 ms
//   <TIME_DELAY_15=> 1024 ms
// <i> Selection of Window Time
#define SL_WDT_WINDOW_TIME TIME_DELAY_0

// </h>
// </e>
// <<< end of configuration section >>>

#if defined(SL_WDT_INTERRUPT_TIME)
watchdog_timer_config_t sl_watchdog_timer_config_handle = {
  .interrupt_time    = SL_WDT_INTERRUPT_TIME,
  .system_reset_time = SL_WDT_SYSTEM_RESET_TIME,
  .window_time       = SL_WDT_WINDOW_TIME,
};
#endif // SL_WDT_INTERRUPT_TIME

#endif // SL_SI91X_WATCHDOG_TIMER_CONFIG_H
