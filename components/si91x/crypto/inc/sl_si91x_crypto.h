/***************************************************************************/ /**
 * @file
 * @brief SL SI91X Crypto common utility file
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

/******************************************************
 *                    Constants
 ******************************************************/
#define SL_SI91X_MAX_DATA_SIZE_IN_BYTES \
  1400                              ///< Maximum length of the AES message in bytes that can be handled in one go
#define SL_SI91X_IV_SIZE         16 ///< Length of the initialization vector in bytes
#define SL_SI91X_KEY_BUFFER_SIZE 32 ///< Key size in bytes

/// Flags to determine the chunk being handled in the API
#define FIRST_CHUNK  BIT(0)
#define MIDDLE_CHUNK BIT(1)
#define LAST_CHUNK   BIT(2)

/// Key slot for Built in keys
typedef enum {
  SL_SI91X_M4_OTA_KEY_SLOT = 1, ///< OTA key slot
  SL_SI91X_M4_PUBLIC_KEY_SLOT   ///< Public key slot
} sl_si91x_crypto_key_slot_t;

/// Key types
typedef enum {
  SL_SI91X_TRANSPARENT_KEY = 0, ///< Plain key
  SL_SI91X_WRAPPED_KEY, ///< Wrapped version of the plain key. To use the wrapped keys security should be enabled in the module.
  SL_SI91X_BUILT_IN_KEY ///< Firmware built in keys
} sl_si91x_crypto_key_type_t;

/// Wrap IV mode
typedef enum {
  SL_SI91X_WRAP_IV_ECB_MODE = 0, ///< Wrap IV using ECB mode
  SL_SI91X_WRAP_IV_CBC_MODE      ///< Wrap IV using CBC mode
} sl_si91x_crypto_wrap_mode_t;

/// Crypto Algorithms
typedef enum {
  AES      = 2,  ///< AES
  SHA      = 4,  ///< SHA
  HMAC_SHA = 5,  ///< HMAC
  DH       = 14, ///< DH
  ECDH     = 15, ///< ECDH
  SHA3     = 16, ///< SHA3
  GCM      = 18, ///< GCM
  WRAP     = 20, ///< WRAP
  TRNG     = 21  ///< TRNG
} sl_si91x_crypto_algorithm_type_t;
