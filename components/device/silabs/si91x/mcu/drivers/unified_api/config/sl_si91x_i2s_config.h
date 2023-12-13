/***************************************************************************/ /**
 * @file sl_si91x_gspi_config.h
 * @brief GSPI API configuration
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

#ifndef SL_SI91X_I2S_CONFIG_H
#define SL_SI91X_I2S_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_si91x_i2s.h"

// <<< Use Configuration Wizard in Context Menu >>>

//<h>I2S Configuration

// <o SL_I2S_RESOLUTION> I2S Resolution
//   <SL_I2S_RESOLUTION_12=> 12 bit
//   <SL_I2S_RESOLUTION_16=> 16 bit
//   <SL_I2S_RESOLUTION_20=> 20 bit
//   <SL_I2S_RESOLUTION_24=> 24 bit
//   <SL_I2S_RESOLUTION_32=> 32 bit
// <i> Default: SL_I2S_RESOLUTION_16
#define SL_I2S_RESOLUTION SL_I2S_RESOLUTION_16

// <o SL_I2S_SAMPLING_RATE> Sampling Rate
//   <SL_I2S_SAMPLING_RATE_8000=>  	8kHz
//   <SL_I2S_SAMPLING_RATE_11025=>  11.025kHz
//   <SL_I2S_SAMPLING_RATE_16000=>  16kHz
//   <SL_I2S_SAMPLING_RATE_22050=>  22.05kHz
//   <SL_I2S_SAMPLING_RATE_24000=>  24kHz
//   <SL_I2S_SAMPLING_RATE_32000=>  32kHz
//   <SL_I2S_SAMPLING_RATE_44100=>  44.1kHz
//   <SL_I2S_SAMPLING_RATE_48000=>  48kHz
//   <SL_I2S_SAMPLING_RATE_88200=>  88.2kHz
//   <SL_I2S_SAMPLING_RATE_96000=>  96kHz
//   <SL_I2S_SAMPLING_RATE_192000=> 192kHz
// <i> Default: SL_I2S_SAMPLING_RATE_8000
#define SL_I2S_SAMPLING_RATE SL_I2S_SAMPLING_RATE_8000

// </h>
// <<< end of configuration section >>>

#ifdef __cplusplus
}
#endif
#endif // SL_SI91X_I2S_CONFIG_H
