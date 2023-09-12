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

#ifndef SL_SI91X_ADC_CHANNEL_10_CONFIG_H
#define SL_SI91X_ADC_CHANNEL_10_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************/
/******************************* ADC Configuration **************************/

// <<< Use Configuration Wizard in Context Menu >>>
// <h>ADC Configuration

// <o SL_ADC_CHANNEL_10_INPUT_TYPE> Input Type
//   <SL_ADC_SINGLE_ENDED=>  Single ended
//   <SL_ADC_DIFFERENTIAL_ENDED=> Differential ended
// <i> Selection of the ADC input type.
#define SL_ADC_CHANNEL_10_INPUT_TYPE SL_ADC_SINGLE_ENDED

// <o SL_ADC_CHANNEL_10_SAMPLING_RATE> Sampling Rate <1-5000000>
// <i> Default: 100000
#define SL_ADC_CHANNEL_10_SAMPLING_RATE 100000

// <o SL_ADC_CHANNEL_10_SAMPLE_LENGTH> Sample Length <1-1023>
// <i> Default: 1023
#define SL_ADC_CHANNEL_10_SAMPLE_LENGTH 1023

// <o SL_ADC_CHANNEL_10_POS_INPUT_CHNL_SEL> POS Input Channel Selection <0-15>
// <i> Default: 0
#define SL_ADC_CHANNEL_10_POS_INPUT_CHNL_SEL 5

// <o SL_ADC_CHANNEL_10_NEG_INPUT_CHNL_SEL> NEG Input Channel Selection <0-8>
// <i> Default: 0
#define SL_ADC_CHANNEL_10_NEG_INPUT_CHNL_SEL 5

// </h>
// <<< end of configuration section >>>

#ifdef __cplusplus
}
#endif // SL_ADC
#endif /* SL_SI91X_ADC_CHANNEL_10_CONFIG_H */
