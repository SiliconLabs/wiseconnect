/***************************************************************************/ /**
* @file sl_si91x_pcm_init_ulp_pcm_config.h
* @brief ULP_PCM driver configuration file.
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

#ifndef SL_SI91X_PCM_ULP_PCM_CONFIG_H
#define SL_SI91X_PCM_ULP_PCM_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>
#ifdef __cplusplus
extern "C" {
#endif

#include "sl_si91x_pcm.h"
#include "sl_si91x_i2s.h"

/******************************************************************************/
/******************************* PCM1(ULP_PCM) Configuration **************************/
//  <e>ULP PCM UC Configuration
//  <i> Enable: Peripheral configuration is taken straight from the configuration based on the settings specified in the universal configuration (UC)
//  <i> Disable: If the application demands it to be modified during runtime, use the sl_si91x_pcm_config_transmit_receive API to modify the peripheral configuration.
//  <i> Default: 0
#define ULP_PCM_UC 0

// <o SL_ULP_PCM_RESOLUTION> ULP_PCM Resolution
//   <16=> 16 bit
//   <24=> 24 bit
//   <32=> 32 bit
// <i> Default: 16
#define SL_ULP_PCM_RESOLUTION 16

// <o SL_ULP_PCM_SAMPLING_RATE> ULP_PCM Sampling Rate
//   <SL_PCM_SAMPLING_RATE_8000=>   8kHz
//   <SL_PCM_SAMPLING_RATE_11025=>  11.025kHz
//   <SL_PCM_SAMPLING_RATE_16000=>  16kHz
//   <SL_PCM_SAMPLING_RATE_22050=>  22.05kHz
//   <SL_PCM_SAMPLING_RATE_24000=>  24kHz
// <i> Default: SL_PCM_SAMPLING_RATE_8000
#define SL_ULP_PCM_SAMPLING_RATE SL_PCM_SAMPLING_RATE_8000

// <o SL_ULP_PCM_MODE> ULP_PCM Mode
//   <SL_I2S_MASTER=> Primary
//   <SL_I2S_SLAVE=> Secondary
// <i> Default: SL_I2S_MASTER
#define SL_ULP_PCM_MODE SL_I2S_MASTER

// </e>

// <<< sl:start pin_tool >>>
// <i2s_ulp signal=DOUT0,DIN0> SL_ULP_I2S
// $[I2S_ULP_SL_ULP_I2S]
#ifndef SL_ULP_I2S_PERIPHERAL
#define SL_ULP_I2S_PERIPHERAL ULP_I2S
#endif

// ULP_I2S DOUT0 on ULP_GPIO_1/GPIO_65
#ifndef SL_ULP_I2S_DOUT0_PORT
#define SL_ULP_I2S_DOUT0_PORT ULP
#endif
#ifndef SL_ULP_I2S_DOUT0_PIN
#define SL_ULP_I2S_DOUT0_PIN 1
#endif
#ifndef SL_ULP_I2S_DOUT0_LOC
#define SL_ULP_I2S_DOUT0_LOC 4
#endif

// ULP_I2S DIN0 on ULP_GPIO_6/GPIO_70
#ifndef SL_ULP_I2S_DIN0_PORT
#define SL_ULP_I2S_DIN0_PORT ULP
#endif
#ifndef SL_ULP_I2S_DIN0_PIN
#define SL_ULP_I2S_DIN0_PIN 6
#endif
#ifndef SL_ULP_I2S_DIN0_LOC
#define SL_ULP_I2S_DIN0_LOC 8
#endif
// [I2S_ULP_SL_ULP_I2S]$
// <<< sl:end pin_tool >>>

#ifdef __cplusplus
}
#endif

#endif // SL_SI91X_PCM_ULP_PCM_CONFIG_H