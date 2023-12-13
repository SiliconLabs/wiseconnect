/***************************************************************************/ /**
 * @file
 * @brief SL SI91X HMAC Header file
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
#include "sl_si91x_crypto.h"
#include "sl_status.h"

/******************************************************
 *                    Constants
 ******************************************************/
/**
 * @addtogroup CRYPTO_HMAC_CONSTANTS 
 * @{ 
 */

typedef enum {
  SL_SI91X_HMAC_SHA_1 = 1, ///< HMAC SHA 1 mode
  SL_SI91X_HMAC_SHA_256,   ///< HMAC SHA 256 mode
  SL_SI91X_HMAC_SHA_384,   ///< HMAC SHA 384 mode
  SL_SI91X_HMAC_SHA_512    ///< HMAC SHA 512 mode
} sl_si91x_hmac_mode_t;

typedef enum {
  SL_SI91X_HMAC_SHA_1_DIGEST_LEN   = 20,
  SL_SI91X_HMAC_SHA_256_DIGEST_LEN = 32,
  SL_SI91X_HMAC_SHA_384_DIGEST_LEN = 48,
  SL_SI91X_HMAC_SHA_512_DIGEST_LEN = 64
} sl_si91x_hmac_digest_len_t;

/** @} */

/******************************************************
 *                   Type Definitions
 ******************************************************/
/**
 * @addtogroup CRYPTO_HMAC_TYPES 
 * @{ 
 */

typedef struct {
  uint8_t *key;        ///< Pointer to the key
  uint32_t key_length; ///< Length of the key
} sl_si91x_hmac_key_config_A0_t;

typedef struct {
  sl_si91x_crypto_key_type_t key_type;      ///< Key type
  uint32_t key_size;                        ///< Key size
  sl_si91x_crypto_key_slot_t key_slot;      ///< Key slot
  sl_si91x_crypto_wrap_mode_t wrap_iv_mode; ///< Wrap mode
  uint8_t wrap_iv[SL_SI91X_IV_SIZE];        ///< Wrap IV
  uint8_t *key;                             ///< Pointer to the key
  uint32_t reserved;                        ///< Reserved for future use
} sl_si91x_hmac_key_config_B0_t;

typedef union {
  sl_si91x_hmac_key_config_A0_t A0; ///< Key configuration for non-B0 chip versions
  sl_si91x_hmac_key_config_B0_t B0; ///< Key configuration for B0 chip versions
} sl_si91x_hmac_key_config_t;

typedef struct {
  sl_si91x_hmac_mode_t hmac_mode;        ///< HMAC Mode
  const uint8_t *msg;                    ///< Pointer to the input message
  uint32_t msg_length;                   ///< Length of the message
  sl_si91x_hmac_key_config_t key_config; ///< Key configuration
} sl_si91x_hmac_config_t;

/** @} */

/**
 * @addtogroup CRYPTO_HMAC_FUNCTIONS
 * @{ 
 */

/***************************************************************************/ /**
 * @brief This API will provide the HMAC output for the given configuration. This is a blocking API.
 * @param[in] config 
 *   Configuration object of type @ref sl_si91x_hmac_config_t
 * @param[out] output 
 *   Buffer to store the output.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
******************************************************************************/
sl_status_t sl_si91x_hmac(sl_si91x_hmac_config_t *config, uint8_t *output);

/** @} */
