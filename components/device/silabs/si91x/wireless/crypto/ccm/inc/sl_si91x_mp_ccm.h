/***************************************************************************/ /**
 * @file
 * @brief SL SI91X Multipart CCM Header file
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
#include "sl_si91x_ccm.h"
#include "sl_si91x_protocol_types.h"

/******************************************************
 *                Function Declarations
*******************************************************/

#if defined(SLI_SI917B0) && defined(SL_SI91X_SIDE_BAND_CRYPTO)
/***************************************************************************/
/**
 * @brief
 *   Copy B0 key configuration from @ref sl_si91x_ccm_config_t into an
 *   @ref sli_si91x_ccm_mp_request_t. This is used by both single-shot
 *   sideband CCM and multipart CCM paths.
 * @param[in,out] request
 *   Pointer to the multipart CCM request whose key_info fields will be
 *   populated.
 * @param[in] config
 *   Configuration object of type @ref sl_si91x_ccm_config_t containing the
 *   B0 key configuration to copy from.
 ******************************************************************************/
void sl_si91x_ccm_mp_get_key_info(sli_si91x_ccm_mp_request_t *request, const sl_si91x_ccm_config_t *config);
#endif
/**
 * @addtogroup CRYPTO_CCM_FUNCTIONS
 * @{
 */

/***************************************************************************/
/**
 * @brief
 *   To initialize a multipart CCM operation by sending the first-packet command
 *   (FIRST_CHUNK) to the firmware. This resets and initializes the firmware CCM
 *   context with all required configuration: key, nonce, AD, tag length,
 *   total message length, and encrypt/decrypt direction. No plaintext or
 *   ciphertext payload is sent in this call. This is a blocking API.
 * @param[in] config
 *   Configuration object of type @ref sl_si91x_ccm_config_t.
 * @return
 *   sl_status_t.
 * For more information on status codes, refer to
 * [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 ******************************************************************************/
sl_status_t sl_si91x_mp_ccm_init(sl_si91x_ccm_config_t *config);

/***************************************************************************/
/**
 * @brief
 *   To send a middle-packet CCM command (MIDDLE_CHUNK) carrying a chunk of
 *   message data to the firmware for encryption or decryption. This is a blocking API.
 * @param[in] config
 *   Configuration object of type @ref sl_si91x_ccm_config_t. The msg pointer
 *   should be advanced to the current chunk.
 * @param[in] chunk_length
 *   Number of bytes in this chunk.
 * @param[out] output
 *   Buffer to store the encrypted or decrypted output for this chunk.
 * @return
 *   sl_status_t.
 * For more information on status codes, refer to
 * [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 ******************************************************************************/
sl_status_t sl_si91x_mp_ccm_update(sl_si91x_ccm_config_t *config, uint16_t chunk_length, uint8_t *output);

/***************************************************************************/
/**
 * @brief
 *   To send the last-packet CCM command (LAST_CHUNK) to the firmware, which
 *   finalizes the CCM operation. For encryption, the firmware produces the
 *   authentication tag. For decryption, the firmware verifies the tag. This is a blocking API.
 * @param[in] config
 *   Configuration object of type @ref sl_si91x_ccm_config_t. The msg pointer
 *   should be at the final chunk (may be NULL if chunk_length is 0).
 * @param[in] chunk_length
 *   Number of bytes in this final chunk (can be 0).
 * @param[out] output
 *   Buffer to store the firmware output.
 * @return
 *   sl_status_t.
 * For more information on status codes, refer to
 * [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 ******************************************************************************/
sl_status_t sl_si91x_mp_ccm_final(sl_si91x_ccm_config_t *config, uint16_t chunk_length, uint8_t *output);

/** @} */
