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
/******************************************************
 *                    Constants
 ******************************************************/
#define SL_SI91X_HMAC_PADDING              BIT(0)
#define SL_SI91X_HMAC_512_BLOCK_SIZE       128
#define SL_SI91X_HMAC_256_BLOCK_SIZE       64
#define SL_SI91X_HMAC_512_BLOCK_SIZE_ALIGN 127
#define SL_SI91X_HMAC_256_BLOCK_SIZE_ALIGN 63
#define SL_SI91X_DEFAULT_16BYTE_ALIGN      15

/**
 * @addtogroup CRYPTO_WRAP_TYPES 
 * @{ 
 */

/**
 * @brief Structure defining WRAP configuration.
 * 
 * The structure defines the WRAP configuration required for the operation, which includes 
 * key type, key size, key buffer, and so on.
 */
typedef struct {
  uint32_t key_type;                        ///< Type of the key
  uint16_t padding;                         ///< BIT(0) is set: HMAC padding, else PKCS7 padding
  uint16_t hmac_sha_mode;                   ///< To determine HMAC SHA mode if HMAC padding is set, else reserved
  uint32_t key_size;                        ///< Size of the key
  sl_si91x_crypto_wrap_mode_t wrap_iv_mode; ///< Wrap IV mode
  uint8_t wrap_iv[SL_SI91X_IV_SIZE];        ///< Buffer to store the IV
  uint8_t key_buffer[SL_SI91X_WRAP_KEY_BUFFER_SIZE]; ///< Key data wrapped or plain text
} sl_si91x_wrap_config_t;

typedef enum {
  SL_SI91X_WRAP_HMAC_SHA_1 = 1, ///< HMAC SHA 1 mode used for padding during wrap
  SL_SI91X_WRAP_HMAC_SHA_256,   ///< HMAC SHA 256 used for padding during wrap
  SL_SI91X_WRAP_HMAC_SHA_384,   ///< HMAC SHA 384 mode used for padding during wrap
  SL_SI91X_WRAP_HMAC_SHA_512    ///< HMAC SHA 512 mode used for padding during wrap
} sl_si91x_wrap_mode_t;

/** @} */

/******************************************************
 *                Function Declarations
*******************************************************/
/**
 * @addtogroup CRYPTO_WRAP_FUNCTIONS
 * @{ 
 */

/***************************************************************************/
/**
 * @brief
 *   To get the wrapped version of the plain key.
 * @param[in] config 
 *   Configuration object of type @ref sl_si91x_wrap_config_t
 * @param[out] output 
 *   Buffer to store the output.
 * @return
 *   sl_status_t.
 * For more information on status codes, refer to 
 * [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
******************************************************************************/
sl_status_t sl_si91x_wrap(sl_si91x_wrap_config_t *config, uint8_t *output);

/** @} */
