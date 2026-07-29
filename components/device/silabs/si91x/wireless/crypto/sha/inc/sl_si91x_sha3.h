/*******************************************************************************
 * @file  sl_si91x_sha3.h
 * @brief SHA3 (Keccak) hash API for Si91x
 *******************************************************************************
 * # License
 * <b>Copyright 2026 Silicon Laboratories Inc. www.silabs.com</b>
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
 * @addtogroup CRYPTO_SHA3_CONSTANTS
 * @{
 */

#define SL_SI91X_SHA3_LEN_INVALID 0 ///< SHA3 length is invalid

/**
 * @brief SHA3 modes supported by the SI91X device (algorithm_sub_type for SHA3).
 *
 * Values align with the SHA API mapping for digest width: 224, 256, 384, and 512-bit.
 */
typedef enum {
  SL_SI91X_SHA3_512 = 9,  ///< SHA3-512
  SL_SI91X_SHA3_384 = 13, ///< SHA3-384
  SL_SI91X_SHA3_256 = 17, ///< SHA3-256
  SL_SI91X_SHA3_224 = 18, ///< SHA3-224
} sl_si91x_crypto_sha3_mode_t;

/**
 * @brief Digest lengths for SHA3 modes (bytes).
 */
typedef enum {
  SL_SI91X_SHA3_224_DIGEST_LEN = 28, ///< SHA3-224
  SL_SI91X_SHA3_256_DIGEST_LEN = 32, ///< SHA3-256
  SL_SI91X_SHA3_384_DIGEST_LEN = 48, ///< SHA3-384
  SL_SI91X_SHA3_512_DIGEST_LEN = 64, ///< SHA3-512
} sl_si91x_sha3_length_t;

/** @} */
