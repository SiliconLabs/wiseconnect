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
// <h>Clock Configuration
#define KHZ_RO_CLK_SEL   1
#define KHZ_RC_CLK_SEL   2
#define KHZ_XTAL_CLK_SEL 4
// <o SL_LOW_FREQ_FSM_CLK_SRC> Low Frequency Clock
//   <KHZ_RO_CLK_SEL=> RO CLK
//   <KHZ_RC_CLK_SEL=> RC CLK
//   <KHZ_XTAL_CLK_SEL=> XTAL CLK
// <i> Selection of Low Frequency FSM CLK
#define SL_LOW_FREQ_FSM_CLK_SRC KHZ_RC_CLK_SEL

#define FSM_20MHZ_RO   1
#define FSM_32MHZ_RC   2
#define FSM_40MHZ_XTAL 4
// <o SL_HIGH_FREQ_FSM_CLK_SRC> High Frequency ClocK
//   <FSM_20MHZ_RO=> 20MHZ RO CLK
//   <FSM_32MHZ_RC=> 32MHZ RC CLK
//   <FSM_40MHZ_XTAL=> 40MHZ XTAL CLK
// <i> Selection of High Frequency FSM CLK
#define SL_HIGH_FREQ_FSM_CLK_SRC FSM_32MHZ_RC

#define RO_32KHZ_CLOCK 1
#define MCU_FSM_CLOCK  2
// <o SL_BG_PMU_CLOCK_SRC> BG PMU Clock
//   <RO_32KHZ_CLOCK=> RO 32KHZ CLK
//   <MCU_FSM_CLOCK=> MCU FSM CLK
// <i> Selection of BG PMU Clock Source
#define SL_BG_PMU_CLOCK_SRC RO_32KHZ_CLOCK
// </h>

// <h>Watchdog Timer Configuration

// <o SL_WDT_SYSTEM_RESET_TIME> System-reset time
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
// <<< end of configuration section >>>

// WDT clock sources and parameters initialization

#if defined(SL_LOW_FREQ_FSM_CLK_SRC)
watchdog_timer_clock_config_t sl_watchdog_timer_clk_config_handle = {
  .low_freq_fsm_clock_src  = SL_LOW_FREQ_FSM_CLK_SRC,
  .high_freq_fsm_clock_src = SL_HIGH_FREQ_FSM_CLK_SRC,
  .bg_pmu_clock_source     = SL_BG_PMU_CLOCK_SRC,
};
#endif // SL_LOW_FREQ_FSM_CLK_SRC

#if defined(SL_WDT_INTERRUPT_TIME)
watchdog_timer_config_t sl_watchdog_timer_config_handle = {
  .interrupt_time    = SL_WDT_INTERRUPT_TIME,
  .system_reset_time = SL_WDT_SYSTEM_RESET_TIME,
  .window_time       = SL_WDT_WINDOW_TIME,
};
#endif // SL_WDT_INTERRUPT_TIME

#endif // SL_SI91X_WATCHDOG_TIMER_CONFIG_H
