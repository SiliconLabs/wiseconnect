/***************************************************************************/ /**
 * @file
 * @brief SL SI91X CHACHAPOLY Header file
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
 * @addtogroup CRYPTO_CHACHAPOLY_CONSTANTS
 * @{ 
 */

typedef enum {
  SL_SI91X_CHACHAPOLY_ENCRYPT = 0, ///< CHACHAPOLY Encryption
  SL_SI91X_CHACHAPOLY_DECRYPT      ///< CHACHAPOLY Decryption
} sl_si91x_chachapoly_type_t;

typedef enum {
  SL_SI91X_CHACHA20POLY1305_MODE = 0, ///< CHACHA20POLY1305 mode
  SL_SI91X_CHACHA20_MODE,             ///< CHACHA20 Mode
  SL_SI91X_CHACHAPOLY_POLY1305_KEYR_KEYS_MODE,
  SL_SI91X_POLY1305_MODE
} sl_si91x_chachapoly_mode_t;

typedef enum {
  SL_SI91X_DMA_DISABLE = 0, ///< Disable DMA
  SL_SI91X_DMA_ENABLE       ///< Enable DMA
} sl_si91x_chachapoly_dma_use_t;

typedef enum {
  SL_SI91X_CHACHAPOLY_KEY_SIZE_256 = 32 ///< key size of 256 bits
} sl_si91x_chachapoly_key_size_t;

/** @} */

/******************************************************
 *                   Type Definitions
 ******************************************************/
/**
 * @addtogroup CRYPTO_CHACHAPOLY_TYPES 
 * @{ 
 */

typedef struct {
  uint8_t key_chacha[SL_SI91X_KEY_BUFFER_SIZE];
  uint8_t keyr_in[16];
  uint8_t keys_in[16];
} sl_si91x_chachapoly_key_config_a0_t;

typedef struct {
  sl_si91x_crypto_key_type_t key_type;          ///< Key type
  sl_si91x_chachapoly_key_size_t key_size;      ///< Key size
  sl_si91x_crypto_key_slot_t key_slot;          ///< Key slot
  sl_si91x_crypto_wrap_mode_t wrap_iv_mode;     ///< Wrap mode
  uint8_t wrap_iv[SL_SI91X_IV_SIZE];            ///< IV used in SL_SI91X_AES_CBC and SL_SI91X_AES_CTR modes
  uint8_t key_buffer[SL_SI91X_KEY_BUFFER_SIZE]; ///< Buffer to store the key
  uint32_t reserved;                            ///< Reserved for future use
} sl_si91x_chachapoly_key_config_b0_t;

typedef union {
  sl_si91x_chachapoly_key_config_a0_t a0; ///< Key configuration for non-B0 chip versions
  sl_si91x_chachapoly_key_config_b0_t b0; ///< Key configuration for B0 chip versions
} sl_si91x_chachapoly_key_config_t;

typedef struct {
  sl_si91x_chachapoly_type_t encrypt_decrypt; ///< Encryption or decryption
  sl_si91x_chachapoly_mode_t chachapoly_mode;
  sl_si91x_chachapoly_dma_use_t dma_use;       ///< DMA Disable or Enable
  const uint8_t *msg;                          ///< Pointer to the input message
  uint16_t msg_length;                         ///< Length of the message
  const uint8_t *nonce;                        ///< Pointer to the Initialization vector
  const uint8_t *ad;                           ///< Pointer to the additional data
  uint16_t ad_length;                          ///< Length of the additional data
  sl_si91x_chachapoly_key_config_t key_config; ///< Key configuration
} sl_si91x_chachapoly_config_t;

/** @} */

/**
 * @addtogroup CRYPTO_CHACHAPOLY_FUNCTIONS
 * @{ 
 */

/***************************************************************************/ /**
 * @brief
 *   This API is used to encrypt/decrypt the message according to the given configuration. This is a blocking API.
 * @param[in] config 
 *   Configuration object of type @ref sl_si91x_chachapoly_config_t
 * @param[out] output 
 *   Buffer to store the output.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
******************************************************************************/
sl_status_t sl_si91x_chachapoly(sl_si91x_chachapoly_config_t *config, uint8_t *output);

/** @} */