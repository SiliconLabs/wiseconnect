/***************************************************************************/ /**
 * @file sl_si91x_adc_init_inst_config.h
 * @brief ADC configuration file.
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

#ifndef SL_SI91X_ADC_CHANNEL_2_CONFIG_H
#define SL_SI91X_ADC_CHANNEL_2_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************/
/**************************** ADC Channel Configuration ***********************/

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
#if USER_CONFIGURATION_ENABLE
// <<< sl:start pin_tool >>>
// <adc_ch2 signal=P,N> SL_ADC_CH2
// $[ADC_CH2_SL_ADC_CH2]
#ifndef SL_ADC_CH2_PERIPHERAL
#define SL_ADC_CH2_PERIPHERAL ADC_CH2
#endif
#warning "ADC peripheral is not configured. Please configure the ADC pins according to the board connections."
// ADC_CH2 P on GPIO_27
#ifndef SL_ADC_CH2_P_PORT
#define SL_ADC_CH2_P_PORT HP
#endif
#ifndef SL_ADC_CH2_P_PIN
#define SL_ADC_CH2_P_PIN 27
#endif
#ifndef SL_ADC_CH2_P_LOC
#define SL_ADC_CH2_P_LOC 26
#endif

// ADC_CH2 N on GPIO_30
#ifndef SL_ADC_CH2_N_PORT
#define SL_ADC_CH2_N_PORT HP
#endif
#ifndef SL_ADC_CH2_N_PIN
#define SL_ADC_CH2_N_PIN 30
#endif
#ifndef SL_ADC_CH2_N_LOC
#define SL_ADC_CH2_N_LOC 360
#endif
// [ADC_CH2_SL_ADC_CH2]$
// <<< sl:end pin_tool >>>
#endif
// Positive Input Channel Selection
#ifdef SL_ADC_CH2_P_PIN
#define SL_ADC_CHANNEL_2_POS_INPUT_CHNL_SEL \
  ((SL_ADC_CH2_P_PIN == 0)    ? 0           \
   : (SL_ADC_CH2_P_PIN == 2)  ? 1           \
   : (SL_ADC_CH2_P_PIN == 4)  ? 2           \
   : (SL_ADC_CH2_P_PIN == 6)  ? 3           \
   : (SL_ADC_CH2_P_PIN == 8)  ? 4           \
   : (SL_ADC_CH2_P_PIN == 10) ? 5           \
   : (SL_ADC_CH2_P_PIN == 25) ? 6           \
   : (SL_ADC_CH2_P_PIN == 27) ? 7           \
   : (SL_ADC_CH2_P_PIN == 29) ? 8           \
   : (SL_ADC_CH2_P_PIN == 1)  ? 10          \
   : (SL_ADC_CH2_P_PIN == 3)  ? 11          \
   : (SL_ADC_CH2_P_PIN == 5)  ? 12          \
   : (SL_ADC_CH2_P_PIN == 11) ? 13          \
   : (SL_ADC_CH2_P_PIN == 9)  ? 14          \
   : (SL_ADC_CH2_P_PIN == 7)  ? 15          \
   : (SL_ADC_CH2_P_PIN == 26) ? 16          \
   : (SL_ADC_CH2_P_PIN == 28) ? 17          \
   : (SL_ADC_CH2_P_PIN == 30) ? 18          \
                              : -1)
#else
#define SL_ADC_CHANNEL_2_POS_INPUT_CHNL_SEL 7
#endif
// Negative Input Channel Selection
#ifdef SL_ADC_CH2_N_PIN
#define SL_ADC_CHANNEL_2_NEG_INPUT_CHNL_SEL \
  ((SL_ADC_CH2_N_PIN == 1)    ? 0           \
   : (SL_ADC_CH2_N_PIN == 3)  ? 1           \
   : (SL_ADC_CH2_N_PIN == 5)  ? 2           \
   : (SL_ADC_CH2_N_PIN == 11) ? 3           \
   : (SL_ADC_CH2_N_PIN == 9)  ? 4           \
   : (SL_ADC_CH2_N_PIN == 7)  ? 5           \
   : (SL_ADC_CH2_N_PIN == 26) ? 6           \
   : (SL_ADC_CH2_N_PIN == 28) ? 7           \
   : (SL_ADC_CH2_N_PIN == 30) ? 8           \
                              : -1)
#else
#define SL_ADC_CHANNEL_2_NEG_INPUT_CHNL_SEL 8
#endif

#ifdef __cplusplus
}
#endif // SL_ADC
#endif /* SL_SI91X_ADC_CHANNEL_2_CONFIG_H */
