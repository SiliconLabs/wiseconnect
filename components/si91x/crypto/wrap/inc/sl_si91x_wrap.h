/***************************************************************************/ /**
 * @file
 * @brief SL SI91X Wrap Header file
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

#pragma once
#include "sl_si91x_protocol_types.h"
#include "sl_si91x_crypto.h"
#include "sl_status.h"

/******************************************************
 *                   Type Definitions
 ******************************************************/
typedef struct {
  uint32_t key_type;                            ///< Key type
  uint32_t reserved;                            ///< Reserved for future use
  uint32_t key_size;                            ///< Key size
  sl_si91x_crypto_wrap_mode_t wrap_iv_mode;     ///< Wrap IV mode
  uint8_t wrap_iv[SL_SI91X_IV_SIZE];            ///< Buffer to store IV
  uint8_t key_buffer[SL_SI91X_KEY_BUFFER_SIZE]; ///< Key data wrapped/ Plain text
} sl_si91x_wrap_config_t;

/***************************************************************************/ /**
 * @brief 
 *   This API is used to get wrap version of the plain key.
 * @param config 
 *   Configuration object of type @ref sl_si91x_wrap_config_t
 * @param output 
 *   Buffer to store the output.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
******************************************************************************/
sl_status_t sl_si91x_wrap(sl_si91x_wrap_config_t *config, uint8_t *output);
