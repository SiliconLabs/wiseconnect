/***************************************************************************/ /**
 * @file sl_si91x_i2s_init_pcm0_config.h
 * @brief PCM0 driver configuration file.
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

#ifndef SL_SI91X_PCM_PCM0_CONFIG_H
#define SL_SI91X_PCM_PCM0_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>
#ifdef __cplusplus
extern "C" {
#endif

#include "sl_si91x_pcm.h"
#include "sl_si91x_i2s.h"
/******************************************************************************/
/******************************* PCM Configuration **************************/
// <<< Use Configuration Wizard in Context Menu >>>
//  <e>PCM0 UC Configuration
//  <i> Enable: Peripheral configuration is taken straight from the configuration based on the settings specified in the universal configuration (UC)
//  <i> Disable: If the application demands it to be modified during runtime, use the sl_si91x_pcm_config_transmit_receive API to modify the peripheral configuration.
//  <i> Default: 0
#define PCM0_UC 0

// <o SL_PCM0_RESOLUTION> PCM0 Resolution
//   <16=> 16 bit
//   <24=> 24 bit
//   <32=> 32 bit
// <i> Default: 16
#define SL_PCM0_RESOLUTION 16

// <o SL_PCM0_CHANNEL> PCM0 Channel
//   <0=> Channel 0
//   <1=> Channel 1
// <i> Default: 0
#define SL_PCM0_CHANNEL 0

// <o SL_PCM0_SAMPLING_RATE> PCM0 Sampling Rate
//   <SL_PCM_SAMPLING_RATE_8000=>   8kHz
//   <SL_PCM_SAMPLING_RATE_11025=>  11.025kHz
//   <SL_PCM_SAMPLING_RATE_16000=>  16kHz
//   <SL_PCM_SAMPLING_RATE_22050=>  22.05kHz
//   <SL_PCM_SAMPLING_RATE_24000=>  24kHz
// <i> Default: SL_PCM_SAMPLING_RATE_8000
#define SL_PCM0_SAMPLING_RATE SL_PCM_SAMPLING_RATE_8000

// <o SL_PCM0_MODE> PCM0 Mode
//   <SL_I2S_MASTER=> Primary
//   <SL_I2S_SLAVE=> Secondary
// <i> Default: SL_I2S_MASTER
#define SL_PCM0_MODE SL_I2S_MASTER

// </e>
#if USER_CONFIGURATION_ENABLE
// <<< sl:start pin_tool >>>
// <i2s0 signal=SCLK,WSCLK,DOUT0,DIN0> SL_I2S0
// $[I2S0_SL_I2S0]
#ifndef SL_I2S0_PERIPHERAL
#define SL_I2S0_PERIPHERAL I2S0
#endif
#warning "I2S peripheral is not configured. Please configure the I2S pins according to the board connections."
#ifndef SL_I2S0_PERIPHERAL_NO
#define SL_I2S0_PERIPHERAL_NO 0
#endif

// I2S0 SCLK on GPIO_25
#ifndef SL_I2S0_SCLK_PORT
#define SL_I2S0_SCLK_PORT HP
#endif
#ifndef SL_I2S0_SCLK_PIN
#define SL_I2S0_SCLK_PIN 25
#endif
#ifndef SL_I2S0_SCLK_LOC
#define SL_I2S0_SCLK_LOC 1
#endif

// I2S0 WSCLK on GPIO_26
#ifndef SL_I2S0_WSCLK_PORT
#define SL_I2S0_WSCLK_PORT HP
#endif
#ifndef SL_I2S0_WSCLK_PIN
#define SL_I2S0_WSCLK_PIN 26
#endif
#ifndef SL_I2S0_WSCLK_LOC
#define SL_I2S0_WSCLK_LOC 5
#endif

// I2S0 DOUT0 on GPIO_28
#ifndef SL_I2S0_DOUT0_PORT
#define SL_I2S0_DOUT0_PORT HP
#endif
#ifndef SL_I2S0_DOUT0_PIN
#define SL_I2S0_DOUT0_PIN 28
#endif
#ifndef SL_I2S0_DOUT0_LOC
#define SL_I2S0_DOUT0_LOC 9
#endif

// I2S0 DIN0 on GPIO_27
#ifndef SL_I2S0_DIN0_PORT
#define SL_I2S0_DIN0_PORT HP
#endif
#ifndef SL_I2S0_DIN0_PIN
#define SL_I2S0_DIN0_PIN 27
#endif
#ifndef SL_I2S0_DIN0_LOC
#define SL_I2S0_DIN0_LOC 13
#endif
// [I2S0_SL_I2S0]$
// <<< sl:end pin_tool >>>
#endif
#ifdef __cplusplus
}
#endif

#endif // SL_SI91X_PCM_PCM0_CONFIG_H
