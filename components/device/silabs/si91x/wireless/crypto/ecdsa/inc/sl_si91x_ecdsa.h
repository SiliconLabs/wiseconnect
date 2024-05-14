/***************************************************************************/ /**
 * @file
 * @brief SL SI91X ECDSA Header file
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

#pragma once
#include "sl_si91x_crypto.h"
#include "sl_status.h"

/******************************************************
 *                    Constants
 ******************************************************/
/**
 * @addtogroup CRYPTO_ECDSA_CONSTANTS
 * @{ 
 */

typedef enum {
  SL_SI91X_ECDSA_PRIV_KEY_SIZE_224 = 28, ///< private key size of 224 bits
  SL_SI91X_ECDSA_PRIV_KEY_SIZE_256 = 32, ///< private key size of 256 bits
  SL_SI91X_ECDSA_PUB_KEY_SIZE_224  = 57, ///< public key size of 57 bytes
  SL_SI91X_ECDSA_PUB_KEY_SIZE_256  = 65  ///< public key size of 65 bytes
} sl_si91x_ecdsa_key_size_t;

typedef enum {
  SL_SI91X_GENERATE_ECC_KEY_PAIR = 1, ///< Generate ECC private public key pair
  SL_SI91X_ECDSA_GENERATE_SIGN,       ///< Generate signature
  SL_SI91X_ECDSA_VERIFY_SIGN          ///< Verify signature
} sl_si91x_crypto_ecdsa_mode_t;

typedef enum {
  SL_SI91X_ECDSA_SIGN_HALF_SIZE_P224R1 = 28,  ///< signature size of 224 bits divided by 2
  SL_SI91X_ECDSA_SIGN_HALF_SIZE_P256R1 = 32,  ///< signature size of 256 bits divided by 2
  SL_SI91X_ECDSA_SIGN_MAX_SIZE         = 128, ///< maximum signature size supported
} sl_si91x_ecdsa_signature_size_t;

typedef enum {
  SL_SI91X_ECC_SECP224R1 = 14, ///< Values as provided by firmware
  SL_SI91X_ECC_SECP256R1 = 7
} sl_si91x_crypto_ecc_curve_t;

typedef enum {
  SL_SI91X_ECDSA_SHA_256 = 1,
  SL_SI91X_ECDSA_SHA_384,
  SL_SI91X_ECDSA_SHA_512,
} sl_si91x_crypto_ecdsa_sha_mode_t;

/** @} */

/******************************************************
 *                   Type Definitions
 ******************************************************/
/**
 * @addtogroup CRYPTO_ECDSA_TYPES 
 * @{ 
 */

typedef struct {
  sl_si91x_crypto_key_type_t key_type;      ///< Key type - wrapped or plain
  sl_si91x_ecdsa_key_size_t key_size;       ///< ecc key size
  sl_si91x_crypto_key_slot_t key_slot;      ///< Key slot
  sl_si91x_crypto_wrap_mode_t wrap_iv_mode; ///< Wrap mode - ECB or CBC
  uint8_t wrap_iv[SL_SI91X_IV_SIZE];        ///< IV used in SL_SI91X_AES_CBC and SL_SI91X_AES_CTR modes
  uint32_t reserved;                        ///< Reserved for future use
} sl_si91x_ecdsa_key_config_b0_t;

typedef struct {
  uint16_t key_length; ///< Length of the key
} sl_si91x_ecdsa_key_config_a0_t;

typedef union {
  sl_si91x_ecdsa_key_config_a0_t a0; ///< Key configuration for non-B0 chip versions
  sl_si91x_ecdsa_key_config_b0_t b0; ///< Key configuration for B0 chip versions
} sl_si91x_ecdsa_key_config_t;

typedef struct {
  sl_si91x_crypto_ecdsa_mode_t ecdsa_operation; ///< genereate ecc key pair, sign or verify
  sl_si91x_crypto_ecc_curve_t curve_id;         ///< curve ID for ecdsa
  sl_si91x_crypto_ecdsa_sha_mode_t sha_mode;    ///<SHA hashing of input message
  const uint8_t *private_key;                   ///< Pointer to the private key
  uint8_t private_key_length;                   ///< Length of the private key
  const uint8_t *public_key;                    ///< Pointer to the public key
  uint8_t public_key_length;                    ///< Length of the public key
  const uint8_t *signature;                     ///< Pointer to the signature
  uint8_t signature_length;                     ///< Length of the signature
  const uint8_t *msg;                           ///< Pointer to the input message
  uint32_t msg_length;                          ///< Length of the message
  sl_si91x_ecdsa_key_config_t key_config;       ///< Key configuration
} sl_si91x_ecdsa_config_t;

/** @} */

/**
 * @addtogroup CRYPTO_ECDSA_FUNCTIONS
 * @{ 
 */

/***************************************************************************/ /**
 * @brief 
 *   This API is used to sign/verify the message according to the given configuration. This is a blocking API.
 * @param[in] config 
 *   Configuration object of type @ref sl_si91x_ecdsa_config_t
 * @param[out] output 
 *   Buffer to store the output.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
******************************************************************************/
sl_status_t sl_si91x_ecdsa(sl_si91x_ecdsa_config_t *config, uint8_t *output);

/** @} */
