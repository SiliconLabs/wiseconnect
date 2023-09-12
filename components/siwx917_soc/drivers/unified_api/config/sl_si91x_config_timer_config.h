/***************************************************************************/ /**
 * @file sl_si91x_config_timer_config.h
 * @brief Config Timer configuration file.
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

#ifndef SL_SI91X_CONFIG_TIMER_CONFIG_H
#define SL_SI91X_CONFIG_TIMER_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

/***************************** CT CONFIG SELECTION ****************************/

// <q SET_TIMER_CONFIG> Enable Timer-Config
// <i> Default: 1
#define SET_TIMER_CONFIG 1

// <q SET_WFG_CONFIG> Enable WFG-Config
// <i> Default: 0
#define SET_WFG_CONFIG 0

// <q SET_OCU_CONFIG> Enable OCU-Config
// <i> Default: 0
#define SET_OCU_CONFIG 1

/******************************************************************************/
/******************************* CT Configuration **************************/
// <h> CT Configuration
// <e> SET_TIMER_CONFIG

// <o SL_CT_MODE_32BIT_ENABLE_MACRO> CT Mode
//   <SL_COUNTER_16BIT=> 16-Bit Mode
//   <SL_COUNTER_32BIT=> 32-Bit Mode
// <i> Default: SL_COUNTER_16BIT
#define SL_CT_MODE_32BIT_ENABLE_MACRO SL_COUNTER_16BIT

// <o SL_COUNTER0_DIRECTION_MACRO> Counter-0 : Direction
//   <SL_COUNTER0_UP=> Up Counter
//   <SL_COUNTER0_DOWN=> Down Counter
//   <SL_COUNTER0_UP_DOWN=> Up-Down Counter
// <i> Default: SL_COUNTER1_UP
#define SL_COUNTER0_DIRECTION_MACRO SL_COUNTER0_UP

// <o SL_COUNTER1_DIRECTION_MACRO> Counter-1 : Direction
//   <SL_COUNTER1_UP=> Up Counter
//   <SL_COUNTER1_DOWN=> Down Counter
//   <SL_COUNTER1_UP_DOWN=> Up-Down Counter
// <i> Default: SL_COUNTER1_UP
#define SL_COUNTER1_DIRECTION_MACRO SL_COUNTER1_UP

// <q SL_COUNTER0_PERIODIC_ENABLE_MACRO> Counter0 : Enable Periodic mode
// <i> Default: 1
#define SL_COUNTER0_PERIODIC_ENABLE_MACRO 1

// <q SL_COUNTER1_PERIODIC_ENABLE_MACRO> Counter1 : Enable Periodic mode
// <i> Default: 1
#define SL_COUNTER1_PERIODIC_ENABLE_MACRO 1

// <q SL_COUNTER0_SOFT_RESET_ENABLE_MACRO> Counter0 : Enable soft reset
// <i> Default: 0
#define SL_COUNTER0_SOFT_RESET_ENABLE_MACRO 0

// <q SL_COUNTER1_SOFT_RESET_ENABLE_MACRO> Counter1 : Enable soft reset
// <i> Default: 0
#define SL_COUNTER1_SOFT_RESET_ENABLE_MACRO 0

// <q SL_COUNTER0_TRIGGER_ENABLE_MACRO> Counter0 : Enable software trigger
// <i> Default: 0
#define SL_COUNTER0_TRIGGER_ENABLE_MACRO 0

// <q SL_COUNTER1_TRIGGER_ENABLE_MACRO> Counter1 : Enable software trigger
// <i> Default: 0
#define SL_COUNTER1_TRIGGER_ENABLE_MACRO 0

// <q SL_COUNTER0_SYNC_TRIGGER_ENABLE_MACRO> Counter0 : Enable sync trigger
// <i> Default: 1
#define SL_COUNTER0_SYNC_TRIGGER_ENABLE_MACRO 1

// <q SL_COUNTER1_SYNC_TRIGGER_ENABLE_MACRO> Counter1 : Enable sync trigger
// <i> Default: 1
#define SL_COUNTER1_SYNC_TRIGGER_ENABLE_MACRO 1

// <q SL_COUNTER0_BUFFER_ENABLE_MACRO> Counter0 : Enable buffer
// <i> Default: 0
#define SL_COUNTER0_BUFFER_ENABLE_MACRO 0

// <q SL_COUNTER1_BUFFER_ENABLE_MACRO> Counter1 : Enable buffer
// <i> Default: 0
#define SL_COUNTER1_BUFFER_ENABLE_MACRO 0

// </e>
// </h>
/******************************************************************************/
/***************************** OCU Configuration ******************************/
// <h> OCU Configuration
// <e> SET_OCU_CONFIG

// <q SL_COUNTER0_OCU_OUTPUT_ENABLE_MACRO> Counter0 : Enable OCU Output
// <i> Default: 1
#define SL_COUNTER0_OCU_OUTPUT_ENABLE_MACRO 0

// <q SL_COUNTER0_OCU_DMA_ENABLE_MACRO> Counter0 : Enable OCU DMA
// <i> Default: 0
#define SL_COUNTER0_OCU_DMA_ENABLE_MACRO 0

// <q SL_COUNTER0_OCU_8BIT_MODE_ENABLE_MACRO> Counter0 : Enable OCU 8-bit mode
// <i> Default: 0
#define SL_COUNTER0_OCU_8BIT_MODE_ENABLE_MACRO 0

// <q SL_COUNTER0_OCU_SYNC_ENABLE_MACRO> Counter0 : Enable OCU sync
// <i> Default: 1
#define SL_COUNTER0_OCU_SYNC_ENABLE_MACRO 1

// <q SL_COUNTER1_OCU_OUTPUT_ENABLE_MACRO> Counter1 : Enable OCU Output
// <i> Default: 1
#define SL_COUNTER1_OCU_OUTPUT_ENABLE_MACRO 1

// <q SL_COUNTER1_OCU_DMA_ENABLE_MACRO> Counter1 : Enable OCU DMA
// <i> Default: 0
#define SL_COUNTER1_OCU_DMA_ENABLE_MACRO 0

// <q SL_COUNTER1_OCU_8BIT_ENABLE_MACRO> Counter1 : Enable OCU 8-bit mode
// <i> Default: 0
#define SL_COUNTER1_OCU_8BIT_ENABLE_MACRO 0

// <q SL_COUNTER1_OCU_SYNC_ENABLE_MACRO> Counter1 : Enable OCU sync
// <i> Default: 1
#define SL_COUNTER1_OCU_SYNC_ENABLE_MACRO 1

// <q SL_OCU_OUTPUT0_TOGGLE_HIGH_MACRO> OCU output-0 : Toggle HIGH
// <i> Default: 1
#define SL_OCU_OUTPUT0_TOGGLE_HIGH_MACRO 1

// <q SL_OCU_OUTPUT0_TOGGLE_LOW_MACRO> OCU output-0 : Toggle LOW
// <i> Default: 1
#define SL_OCU_OUTPUT0_TOGGLE_LOW_MACRO 1

// <q SL_OCU_OUTPUT1_TOGGLE_HIGH_MACRO> OCU output-1 : Toggle HIGH
// <i> Default: 1
#define SL_OCU_OUTPUT1_TOGGLE_HIGH_MACRO 1

// <q SL_OCU_OUTPUT1_TOGGLE_LOW_MACRO> OCU output-1 : Toggle LOW
// <i> Default: 1
#define SL_OCU_OUTPUT1_TOGGLE_LOW_MACRO 1

// </e>
// </h>
/******************************************************************************/
/***************************** WFG Configuration ******************************/

// <h> WFG Configuration
// <e> SET_WFG_CONFIG

// <q SL_OUTPUT0_TOGGLE0_MACRO> Select output-0 toggle-LOW
// <i> Default: 0
#define SL_OUTPUT0_TOGGLE0_MACRO 0

// <q SL_OUTPUT0_TOGGLE1_MACRO> Select output-0 toggle-HIGH
// <i> Default: 0
#define SL_OUTPUT0_TOGGLE1_MACRO 0

// <q SL_TOGGLE_COUNTER0_PEAK_MACRO> Toggle counter-0 peak
// <i> Default: 0
#define SL_TOGGLE_COUNTER0_PEAK_MACRO 0

// <q SL_OUTPUT1_TOGGLE0_MACRO> Select output-1 toggle-LOW
// <i> Default: 0
#define SL_OUTPUT1_TOGGLE0_MACRO 0

// <q SL_OUTPUT1_TOGGLE1_MACRO> Select output-1 toggle-HIGH
// <i> Default: 0
#define SL_OUTPUT1_TOGGLE1_MACRO 0

// <q SL_TOGGLE_COUNTER1_PEAK_MACRO> Toggle counter-1 peak
// <i> Default: 0
#define SL_TOGGLE_COUNTER1_PEAK_MACRO 0

// </e>
// </h>

/******************************************************************************/
// <<< end of configuration section >>>

sl_config_timer_config_t ct_configuration = {
  .is_counter_mode_32bit_enabled    = SL_CT_MODE_32BIT_ENABLE_MACRO,
  .is_counter0_soft_reset_enabled   = SL_COUNTER0_SOFT_RESET_ENABLE_MACRO,
  .is_counter0_periodic_enabled     = SL_COUNTER0_PERIODIC_ENABLE_MACRO,
  .is_counter0_trigger_enabled      = SL_COUNTER0_TRIGGER_ENABLE_MACRO,
  .is_counter0_sync_trigger_enabled = SL_COUNTER0_SYNC_TRIGGER_ENABLE_MACRO,
  .is_counter0_buffer_enabled       = SL_COUNTER0_BUFFER_ENABLE_MACRO,
  .is_counter1_soft_reset_enabled   = SL_COUNTER1_SOFT_RESET_ENABLE_MACRO,
  .is_counter1_periodic_enabled     = SL_COUNTER1_PERIODIC_ENABLE_MACRO,
  .is_counter1_trigger_enabled      = SL_COUNTER1_TRIGGER_ENABLE_MACRO,
  .is_counter1_sync_trigger_enabled = SL_COUNTER1_SYNC_TRIGGER_ENABLE_MACRO,
  .is_counter1_buffer_enabled       = SL_COUNTER1_BUFFER_ENABLE_MACRO,
  .counter0_direction               = SL_COUNTER0_DIRECTION_MACRO,
  .counter1_direction               = SL_COUNTER1_DIRECTION_MACRO,
};

sl_config_timer_wfg_config_t ct_wfg_configuration = {
  .output0_tgl0_sel = SL_OUTPUT0_TOGGLE0_MACRO,
  .output0_tgl1_sel = SL_OUTPUT0_TOGGLE1_MACRO,
  .tgl_cnt0_peak    = SL_TOGGLE_COUNTER0_PEAK_MACRO,
  .output1_tgl0_sel = SL_OUTPUT1_TOGGLE0_MACRO,
  .output1_tgl1_sel = SL_OUTPUT1_TOGGLE1_MACRO,
  .tgl_cnt1_peak    = SL_TOGGLE_COUNTER1_PEAK_MACRO,
};

sl_config_timer_ocu_config_t ct_ocu_configuration = {
  .is_counter0_ocu_output_enabled         = SL_COUNTER0_OCU_OUTPUT_ENABLE_MACRO,
  .is_counter0_ocu_dma_enabled            = SL_COUNTER0_OCU_DMA_ENABLE_MACRO,
  .is_counter0_ocu_8bit_mode_enabled      = SL_COUNTER0_OCU_8BIT_MODE_ENABLE_MACRO,
  .is_counter0_ocu_sync_enabled           = SL_COUNTER0_OCU_SYNC_ENABLE_MACRO,
  .is_counter1_ocu_output_enabled         = SL_COUNTER1_OCU_OUTPUT_ENABLE_MACRO,
  .is_counter1_ocu_dma_enabled            = SL_COUNTER1_OCU_DMA_ENABLE_MACRO,
  .is_counter1_ocu_mode_enabled           = SL_COUNTER1_OCU_8BIT_ENABLE_MACRO,
  .is_counter1_ocu_sync_enabled           = SL_COUNTER1_OCU_SYNC_ENABLE_MACRO,
  .is_counter0_toggle_output_high_enabled = SL_OCU_OUTPUT0_TOGGLE_HIGH_MACRO,
  .is_counter0_toggle_output_low_enabled  = SL_OCU_OUTPUT0_TOGGLE_LOW_MACRO,
  .is_counter1_toggle_output_high_enabled = SL_OCU_OUTPUT1_TOGGLE_HIGH_MACRO,
  .is_counter1_toggle_output_low_enabled  = SL_OCU_OUTPUT1_TOGGLE_LOW_MACRO,
};

#endif /* SL_SI91X_CONFIG_TIMER_CONFIG_H */
