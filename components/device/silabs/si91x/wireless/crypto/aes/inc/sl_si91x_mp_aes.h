/***************************************************************************/ /**
 * @file
 * @brief SL SI91X Multipart AES Header file
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

#ifdef SL_SI91X_SIDE_BAND_CRYPTO

#include "sl_si91x_aes.h"
#include "sl_si91x_protocol_types.h"

/******************************************************
 *                Function Declarations
*******************************************************/

//! @cond Doxygen_Suppress
#if defined(SLI_SI917B0)
/*
 * Internal helper: copy B0 key configuration from sl_si91x_aes_config_t
 * into an sli_si91x_aes_mp_request_t. Shared by the single-shot sideband
 * AES path (sl_si91x_aes.c) and the multipart sideband path
 * (sl_si91x_mp_aes.c) so every chunk request carries the correct key
 * descriptor regardless of which entry point originated it. Internal to
 * the AES component; not part of the public SDK API surface.
 */
void sli_si91x_aes_mp_get_key_info(sli_si91x_aes_mp_request_t *request, const sl_si91x_aes_config_t *config);
#endif
//! @endcond

/**
 * @addtogroup CRYPTO_AES_FUNCTIONS
 * @{
 */

/***************************************************************************/
/**
 * @brief
 *   To initialize a multipart AES operation by sending the first-packet
 *   command (FIRST_CHUNK) to the firmware. This resets and initializes the
 *   firmware AES context with all required configuration: key, IV (for
 *   CBC/CTR), AES sub-mode (CBC/CTR/ECB) and encrypt/decrypt direction.
 *   No plaintext or ciphertext payload is sent in this call. This is a
 *   blocking API.
 * @param[in] config
 *   Configuration object of type @ref sl_si91x_aes_config_t. The
 *   @c msg / @c msg_length fields are ignored by this call.
 * @return
 *   sl_status_t.
 * For more information on status codes, refer to
 * [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 * @note
 *   - For CBC and CTR, @c config->iv must be non-NULL.
 *   - For ECB, @c config->iv is ignored.
 *   - Subsequent chunks must be sent via @ref sl_si91x_mp_aes_update and
 *     finalized with @ref sl_si91x_mp_aes_final.
 * @warning
 *   **Caller-owned serialization.** Unlike @ref sl_si91x_aes, the multipart
 *   entry points (@ref sl_si91x_mp_aes_init / @ref sl_si91x_mp_aes_update /
 *   @ref sl_si91x_mp_aes_final) do not acquire any SDK-internal mutex,
 *   even when @c SLI_MULTITHREAD_DEVICE_SI91X is defined. The firmware
 *   side-band AES multipart context (@c sb_aes_ctx) is a global
 *   single-stream resource: a second @ref sl_si91x_mp_aes_init from any
 *   thread unconditionally resets whatever multipart context was previously
 *   active, and concurrent @ref sl_si91x_mp_aes_update calls from
 *   independent sessions will corrupt each other's running IV / counter /
 *   processed-length state. The caller is responsible for serializing
 *   the entire @c init -> @c update* -> @c final sequence across threads,
 *   typically via the caller's own mutex or a higher-level scheduler
 *   (the PSA cipher driver entry points in this SDK do this naturally
 *   because the PSA core serializes operations on a single handle).
 ******************************************************************************/
sl_status_t sl_si91x_mp_aes_init(sl_si91x_aes_config_t *config);

/***************************************************************************/
/**
 * @brief
 *   To send a middle-packet AES command (MIDDLE_CHUNK) carrying a chunk of
 *   message data to the firmware for encryption or decryption. The firmware
 *   maintains the running IV (for CBC), counter (for CTR), and processed
 *   length across calls. This is a blocking API.
 * @param[in] config
 *   Configuration object of type @ref sl_si91x_aes_config_t. The @c msg
 *   pointer should be advanced to the current chunk.
 * @param[in] chunk_length
 *   Number of bytes in this chunk. Must be > 0. For CBC/ECB this must be
 *   a multiple of @ref SL_SI91X_AES_BLOCK_SIZE; CTR allows arbitrary byte
 *   lengths.
 * @param[out] output
 *   Buffer to store the encrypted or decrypted output for this chunk.
 *   Must be non-NULL.
 * @return
 *   sl_status_t.
 * For more information on status codes, refer to
 * [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 * @note
 *   - The host-side cap on @c chunk_length is the uint16 wire-field width
 *     (65535 bytes).  Firmware splits each chunk internally into HW
 *     transactions for shared AES/SHA engine lock fairness with concurrent
 *     crypto users (Wi-Fi MAC/MGMT, TLS), so a single large chunk costs
 *     one host->NWP round-trip but is internally throttled.
 *   - **Caller-owned serialization.** This call does not acquire an
 *     SDK-internal mutex. See @ref sl_si91x_mp_aes_init for the full
 *     threading contract; in short, the entire init -> update* -> final
 *     sequence must be serialized by the caller against other multipart
 *     AES sessions on the same device.
 ******************************************************************************/
sl_status_t sl_si91x_mp_aes_update(sl_si91x_aes_config_t *config, uint16_t chunk_length, uint8_t *output);

/***************************************************************************/
/**
 * @brief
 *   To send the last-packet AES command (LAST_CHUNK) to the firmware,
 *   which finalizes the multipart AES operation. The firmware runs
 *   @c sl_sb_aes_final followed by @c sl_sb_aes_reset and tears the
 *   multipart context down. @c chunk_length may be 0 if all data was
 *   already sent via @ref sl_si91x_mp_aes_update calls. This is a
 *   blocking API.
 * @param[in] config
 *   Configuration object of type @ref sl_si91x_aes_config_t. The @c msg
 *   pointer should be at the final chunk (may be NULL if @c chunk_length
 *   is 0).
 * @param[in] chunk_length
 *   Number of bytes in this final chunk (can be 0). For CBC/ECB the
 *   cumulative processed length across all chunks must be a multiple of
 *   @ref SL_SI91X_AES_BLOCK_SIZE.
 * @param[out] output
 *   Buffer to store the firmware output for this chunk. May be NULL when
 *   @c chunk_length is 0.
 * @return
 *   sl_status_t.
 * For more information on status codes, refer to
 * [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 * @note
 *   - As with @ref sl_si91x_mp_aes_update, the host-side cap on
 *     @c chunk_length is the uint16 wire-field width; firmware splits the
 *     final chunk internally into HW transactions for shared AES/SHA engine
 *     lock fairness.
 *   - **Caller-owned serialization.** This call does not acquire an
 *     SDK-internal mutex. See @ref sl_si91x_mp_aes_init for the full
 *     threading contract.
 ******************************************************************************/
sl_status_t sl_si91x_mp_aes_final(sl_si91x_aes_config_t *config, uint16_t chunk_length, uint8_t *output);

/** @} */

#endif /* SL_SI91X_SIDE_BAND_CRYPTO */
