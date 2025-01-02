/***************************************************************************/ /**
 * @file sl_si91x_dac_config.h
 * @brief DAC configuration file.
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

#ifndef SL_SI91X_DAC_CONFIG_H
#define SL_SI91X_DAC_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************/
/******************** DAC Peripheral Configuration **********************/

// <<< Use Configuration Wizard in Context Menu >>>
// <h>DAC Peripheral Configuration

// <o SL_DAC_OPERATION_MODE> DAC operation mode
//   <SL_DAC_FIFO_MODE=> FIFO mode
//   <SL_DAC_STATIC_MODE=> Static mode
// <i> Selection of DAC operation mode.
#define SL_DAC_OPERATION_MODE SL_DAC_STATIC_MODE

// <o SL_DAC_SAMPLE_RATE> Sample rate <63-5000000>
// <i> Default: 5000000
#define SL_DAC_SAMPLE_RATE 5000000

// <o SL_DAC_FIFO_THRESHOLD> FIFO Threshold <0-7>
// <i> Default: 3
#define SL_DAC_FIFO_THRESHOLD 3

// </h>

// <<< end of configuration section >>>
#if USER_CONFIGURATION_ENABLE
// <<< sl:start pin_tool >>>
// <dac0 signal=OUT> SL_DAC0
// $[DAC0_SL_DAC0]
#ifndef SL_DAC0_PERIPHERAL
#define SL_DAC0_PERIPHERAL DAC0
#endif
#warning "DAC peripheral is not configured. Please configure the DAC pins according to the board connections."
// DAC0 OUT on ULP_GPIO_4/GPIO_68
#ifndef SL_DAC0_OUT_PORT
#define SL_DAC0_OUT_PORT ULP
#endif
#ifndef SL_DAC0_OUT_PIN
#define SL_DAC0_OUT_PIN 4
#endif
#ifndef SL_DAC0_OUT_LOC
#define SL_DAC0_OUT_LOC 0
#endif
// [DAC0_SL_DAC0]$
// <<< sl:end pin_tool >>>

// Some boards do not have the DAC output pin ULP_GPIO_4. Therefore, the output
// is internally redirected to OPAMP pin GPIO_30.
#ifdef SLI_SI91X_MCU_CONFIG_RADIO_BOARD_BASE_VER
#define SL_DAC_OUTPUT_PORT 0
#define SL_DAC_OUTPUT_PIN  30
#else
#define SL_DAC_OUTPUT_PORT SL_DAC0_OUT_PORT
#define SL_DAC_OUTPUT_PIN  SL_DAC0_OUT_PIN
#endif

sl_dac_config_t sl_dac_config = { .operating_mode     = SL_DAC_OPERATION_MODE,
                                  .dac_fifo_threshold = SL_DAC_FIFO_THRESHOLD,
                                  .dac_sample_rate    = SL_DAC_SAMPLE_RATE,
                                  .adc_channel        = 0,
                                  .dac_pin            = SL_DAC_OUTPUT_PIN,
                                  .dac_port           = SL_DAC_OUTPUT_PORT };
#endif
#ifdef __cplusplus
}
#endif // SL_DAC
#endif /* SL_SI91X_DAC_CONFIG_H */