/***************************************************************************/ /**
 * @file sl_si91x_sysrtc_config.h
 * @brief SYSRTC configuration file.
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_SI91X_SYSRTC_CONFIG_H
#define SL_SI91X_SYSRTC_CONFIG_H

#include "sl_si91x_sysrtc.h"

// <<< Use Configuration Wizard in Context Menu >>>
// <h>Clock Configuration
#define CLK_RO_1KHZ    1
#define CLK_RO_32KHZ   2
#define CLK_RC_32KHZ   4
#define CLK_32KHZ_XTAL 8
// <o SL_SYSRTC_CLK_SRC> SYSRTC Clock
//   <CLK_RO_1KHZ=> 1Khz RO CLK
//   <CLK_RO_32KHZ=> 32khz RO CLK
//   <CLK_RC_32KHZ=> 32khz RC CLK
//   <CLK_32KHZ_XTAL=> 32khz XTAL CLK
// <i> Selection of SYSRTC CLK
#define SL_SYSRTC_CLK_SRC CLK_RC_32KHZ

#define DIVISION_FACTOR_0  0
#define DIVISION_FACTOR_2  2
#define DIVISION_FACTOR_4  4
#define DIVISION_FACTOR_8  8
#define DIVISION_FACTOR_16 16
#define DIVISION_FACTOR_32 32
// <o SL_SYSRTC_CLOCK_DIVISION_FACTOR> Clock division factor
//   <DIVISION_FACTOR_0=> 0
//   <DIVISION_FACTOR_2=> 2
//   <DIVISION_FACTOR_4=> 4
//   <DIVISION_FACTOR_8=> 8
//   <DIVISION_FACTOR_16=> 16
//   <DIVISION_FACTOR_32=> 32
// <i> Selection of SYSRTC CLK DIVISION FACTOR
#define SL_SYSRTC_CLOCK_DIVISION_FACTOR DIVISION_FACTOR_0
// </h>

// <h>SYSRTC Configuration
// <q SL_SYSRTC_RUN_ENABLE_DURING_DEBUG> Counter0 : Enable SYSRTC run during debug
// <i> Default: 0
#define SL_SYSRTC_RUN_ENABLE_DURING_DEBUG 1

#define GROUP_0 0
#define GROUP_1 1
// <o SL_SYSRTC_GROUP> SYSRTC group
//   <GROUP_0=> sysrtc group-0
//   <GROUP_1=> sysrtc group-1
// <i> Selection of SYSRTC group
#define SL_SYSRTC_GROUP GROUP_0

// <q SL_SYSRTC_COMPARE_CHANNEL0_ENABLE> Counter0 : Enable group compare channel-0
// <i> Default: 1
#define SL_SYSRTC_COMPARE_CHANNEL0_ENABLE 1

// <q SL_SYSRTC_COMPARE_CHANNEL1_ENABLE> Counter0 : Enable group compare channel-1
// <i> Default: 0
#define SL_SYSRTC_COMPARE_CHANNEL1_ENABLE 0

// <q SL_SYSRTC_CAPTURE_CHANNEL0_ENABLE> Counter0 : Enable group capture channel-0
// <i> Default: 0
#define SL_SYSRTC_CAPTURE_CHANNEL0_ENABLE 0

// </h>
// <<< end of configuration section >>>

// SYSRTC clock source initialization
#if defined(SL_SYSRTC_CLK_SRC)
sl_sysrtc_clock_config_t sl_sysrtc_clk_config_handle = {
  .clock_source    = SL_SYSRTC_CLK_SRC,
  .division_factor = SL_SYSRTC_CLOCK_DIVISION_FACTOR,
};
#endif // SL_SYSRTC_CLK_SRC

// SYSRTC config structure initialization
#if defined(SL_SYSRTC_RUN_ENABLE_DURING_DEBUG)
sl_sysrtc_config_t sysrtc_handle = {
  .enable_debug_run = SL_SYSRTC_RUN_ENABLE_DURING_DEBUG,
};
#endif // SL_SYSRTC_RUN_DURING_DEBUG_ENABLE

// SYSRTC group-config structure initialization
#if defined(SL_SYSRTC_COMPARE_CHANNEL0_ENABLE)
sl_sysrtc_group_config_t sysrtc_group_config_handle = {
  .compare_channel0_enable   = SL_SYSRTC_COMPARE_CHANNEL0_ENABLE,
  .compare_channel1_enable   = SL_SYSRTC_COMPARE_CHANNEL1_ENABLE,
  .capture_channel0_enable   = SL_SYSRTC_CAPTURE_CHANNEL0_ENABLE,
  .p_compare_channel0_config = NULL,
  .p_compare_channel1_config = NULL,
  .p_capture_channel0_config = NULL,
};
#endif // SL_SYSRTC_COMPARE_CHANNEL0_ENABLE

#endif // SL_SYSRTC_CONFIG_H
