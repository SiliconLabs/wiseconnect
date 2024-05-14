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
//   <SL_DAC_OUTPUT_REF_VOLTAGE_FOR_ADC=> Reference voltage for ADC
// <i> Selection of DAC operation mode.
#define SL_DAC_OPERATION_MODE SL_DAC_STATIC_MODE

// <o SL_DAC_SAMPLE_RATE> Sample rate <63-5000000>
// <i> Default: 5000000
#define SL_DAC_SAMPLE_RATE 5000000

// <o SL_DAC_FIFO_THRESHOLD> FIFO Threshold <0-7>
// <i> Default: 3
#define SL_DAC_FIFO_THRESHOLD 3

// <o SL_DAC_ADC_CHANNEL> ADC Channel
//   <SL_DAC_ADC_CHANNEL_0=> Channel_0
//   <SL_DAC_ADC_CHANNEL_1=> Channel_1
//   <SL_DAC_ADC_CHANNEL_2=> Channel_2
//   <SL_DAC_ADC_CHANNEL_3=> Channel_3
//   <SL_DAC_ADC_CHANNEL_4=> Channel_4
//   <SL_DAC_ADC_CHANNEL_5=> Channel_5
//   <SL_DAC_ADC_CHANNEL_6=> Channel_6
//   <SL_DAC_ADC_CHANNEL_7=> Channel_7
//   <SL_DAC_ADC_CHANNEL_8=> Channel_8
//   <SL_DAC_ADC_CHANNEL_9=> Channel_9
//   <SL_DAC_ADC_CHANNEL_10=> Channel_10
//   <SL_DAC_ADC_CHANNEL_11=> Channel_11
//   <SL_DAC_ADC_CHANNEL_12=> Channel_12
//   <SL_DAC_ADC_CHANNEL_13=> Channel_13
//   <SL_DAC_ADC_CHANNEL_14=> Channel_14
//   <SL_DAC_ADC_CHANNEL_15=> Channel_15
// <i> Selection of ADC Channel.
#define SL_DAC_ADC_CHANNEL SL_DAC_ADC_CHANNEL_0

// </h>

// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>
// <dac signal=IN1> SL_DAC0
// $[DAC_SL_DAC0]
#ifndef SL_DAC0_PERIPHERAL
#define SL_DAC0_PERIPHERAL DAC0
#endif
#ifndef SL_DAC0_PERIPHERAL_NO
#define SL_DAC0_PERIPHERAL_NO 0
#endif

// DAC0 IN1 on GPIO_30
#ifndef SL_DAC0_IN1_PORT
#define SL_DAC0_IN1_PORT 0
#endif
#ifndef SL_DAC0_IN1_PIN
#define SL_DAC0_IN1_PIN 30
#endif
#ifndef SL_DAC0_IN1_LOC
#define SL_DAC0_IN1_LOC 0
#endif
// [DAC_SL_DAC0]$
// <<< sl:end pin_tool >>>

sl_dac_config_t sl_dac_config = { .operating_mode     = SL_DAC_OPERATION_MODE,
                                  .dac_fifo_threshold = SL_DAC_FIFO_THRESHOLD,
                                  .dac_sample_rate    = SL_DAC_SAMPLE_RATE,
                                  .adc_channel        = SL_DAC_ADC_CHANNEL,
                                  .dac_pin            = SL_DAC0_IN1_PIN,
                                  .dac_port           = SL_DAC0_IN1_PORT };

#ifdef __cplusplus
}
#endif // SL_DAC
#endif /* SL_SI91X_DAC_CONFIG_H */
