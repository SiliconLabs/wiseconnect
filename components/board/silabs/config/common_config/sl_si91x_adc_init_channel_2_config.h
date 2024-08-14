/***************************************************************************/ /**
 * @file sl_si91x_adc_init_inst_config.h
 * @brief ADC configuration file.
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

#ifndef SL_SI91X_ADC_CHANNEL_2_CONFIG_H
#define SL_SI91X_ADC_CHANNEL_2_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************/
/**************************** ADC Channel Configuration ***********************/

#define P2_START_LOCATION_PINTOOL 19
#define N2_START_LOCATION_PINTOOL 352
// <<< Use Configuration Wizard in Context Menu >>>
// <h>ADC Channel Configuration

// <o SL_ADC_CHANNEL_2_INPUT_TYPE> Input Type
//   <SL_ADC_SINGLE_ENDED=>  Single ended
//   <SL_ADC_DIFFERENTIAL=> Differential
// <i> Selection of the ADC input type.
#define SL_ADC_CHANNEL_2_INPUT_TYPE SL_ADC_SINGLE_ENDED

// <o SL_ADC_CHANNEL_2_SAMPLING_RATE> Sampling Rate <1-2500000>
// <i> Default: 100000
#define SL_ADC_CHANNEL_2_SAMPLING_RATE 100000

// <o SL_ADC_CHANNEL_2_SAMPLE_LENGTH> Sample Length <1-1023>
// <i> Default: 1023
#define SL_ADC_CHANNEL_2_SAMPLE_LENGTH 1023

// </h>
// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>
// <adc_ch2 signal=P2,N2> SL_ADC_CH2
// $[ADC_CH2_SL_ADC_CH2]
#ifndef SL_ADC_CH2_PERIPHERAL
#define SL_ADC_CH2_PERIPHERAL ADC_CH2
#endif

// ADC_CH2 P2 on GPIO_27
#ifndef SL_ADC_CH2_P2_PORT
#define SL_ADC_CH2_P2_PORT 0
#endif
#ifndef SL_ADC_CH2_P2_PIN
#define SL_ADC_CH2_P2_PIN 27
#endif
#ifndef SL_ADC_CH2_P2_LOC
#define SL_ADC_CH2_P2_LOC 26
#endif

// ADC_CH2 N2 on GPIO_30
#ifndef SL_ADC_CH2_N2_PORT
#define SL_ADC_CH2_N2_PORT 0
#endif
#ifndef SL_ADC_CH2_N2_PIN
#define SL_ADC_CH2_N2_PIN 30
#endif
#ifndef SL_ADC_CH2_N2_LOC
#define SL_ADC_CH2_N2_LOC 360
#endif
// [ADC_CH2_SL_ADC_CH2]$
// <<< sl:end pin_tool >>>

#ifdef SL_ADC_CH2_P2_LOC
#define SL_ADC_CHANNEL_2_POS_INPUT_CHNL_SEL (SL_ADC_CH2_P2_LOC - P2_START_LOCATION_PINTOOL)
#else
#define SL_ADC_CHANNEL_2_POS_INPUT_CHNL_SEL 7
#endif
#ifdef SL_ADC_CH2_N2_LOC
#define SL_ADC_CHANNEL_2_NEG_INPUT_CHNL_SEL (SL_ADC_CH2_N2_LOC - N2_START_LOCATION_PINTOOL)
#else
#define SL_ADC_CHANNEL_2_NEG_INPUT_CHNL_SEL 8
#endif

#ifdef __cplusplus
}
#endif // SL_ADC
#endif /* SL_SI91X_ADC_CHANNEL_2_CONFIG_H */
