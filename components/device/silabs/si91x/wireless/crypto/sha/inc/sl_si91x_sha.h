/*******************************************************************************
 * @file  sl_si91x_sha.h
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#pragma once
#include "sl_si91x_crypto.h"
#include "sl_status.h"

/******************************************************
 *                    Constants
 ******************************************************/
/**
 * @addtogroup CRYPTO_SHA_CONSTANTS
 * @{ 
 */

#define SL_SI91X_SHA_LEN_INVALID 0 ///< SHA length is invalid
/**
 * @brief Enumeration defining SHA modes supported by the SI91X device.
 *
 * This enumeration defines different SHA modes supported by the SI91X device,
 * such as SHA1, SHA224, SHA256, SHA384, and SHA512 modes.
 */
typedef enum {
  SL_SI91X_SHA_1 = 1, ///< SHA 1 mode
  SL_SI91X_SHA_256,   ///< SHA 256 mode
  SL_SI91X_SHA_384,   ///< SHA 384 mode
  SL_SI91X_SHA_512,   ///< SHA 512 mode
  SL_SI91X_SHA_224    ///< SHA 224 mode
} sl_si91x_crypto_sha_mode_t;

/**
 * @brief Enumeration defining digest lengths of SHA modes supported by the SI91X device.
 *
 * This enumeration defines digest lengths of different SHA modes supported by the SI91X device,
 * including SHA1, SHA224, SHA256, SHA384, and SHA512 modes.
 */
typedef enum {
  SL_SI91X_SHA_1_DIGEST_LEN   = 20, ///< Digest length for SHA 1
  SL_SI91X_SHA_256_DIGEST_LEN = 32, ///< Digest length for SHA 256
  SL_SI91X_SHA_384_DIGEST_LEN = 48, ///< Digest length for SHA 384
  SL_SI91X_SHA_512_DIGEST_LEN = 64, ///< Digest length for SHA 512
  SL_SI91X_SHA_224_DIGEST_LEN = 28  ///< Digest length for SHA 224
} sl_si91x_sha_length_t;

/** @} */

/******************************************************
 *                Function Declarations
*******************************************************/
/**
 * @addtogroup CRYPTO_SHA_FUNCTIONS
 * @{ 
 */

/***************************************************************************/
/**
 * @brief 
 *   To provide the SHA output for the given configuration. This is a blocking API.
 * @param[in] sha_mode 
 *   1 – For SHA1 
 *   2 – For SHA256 
 *   3 – For SHA384 
 *   4 – For SHA512 
 *   5 – For SHA224 
 * @param[in]  msg 
 *   Pointer to the message.
 * @param[in]  msg_length 
 *   Total message length.
 * @param[out] digest 
 *   Buffer to store the output.
 * @return
 *   sl_status_t.
 * For more information on status codes, see 
 * [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
******************************************************************************/
sl_status_t sl_si91x_sha(uint8_t sha_mode, uint8_t *msg, uint16_t msg_length, uint8_t *digest);

/** @} */
