/***************************************************************************/ /**
 * @file
 * @brief SL SI91X Multipart AES – side-band first/middle/last packet helpers
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

#ifdef SL_SI91X_SIDE_BAND_CRYPTO

#include "sl_si91x_mp_aes.h"
#include "sl_si91x_crypto.h"
#include "sl_status.h"
#include "sl_constants.h"
#include "sl_si91x_protocol_types.h"
#include "sl_si91x_driver.h"
#include <string.h>

#if defined(SLI_SI917B0)
/*****************************************************************************
 * Copy B0 key configuration into a multipart AES request.
 *
 * @param[in,out] request  Multipart AES request whose key_info fields will
 *                         be populated.
 * @param[in]     config   AES configuration containing the B0 key material.
******************************************************************************/
void sli_si91x_aes_mp_get_key_info(sli_si91x_aes_mp_request_t *request, const sl_si91x_aes_config_t *config)
{
  request->key_info.key_type                         = config->key_config.b0.key_type;
  request->key_info.key_detail.key_size              = config->key_config.b0.key_size;
  request->key_info.key_detail.key_spec.key_slot     = config->key_config.b0.key_slot;
  request->key_info.key_detail.key_spec.wrap_iv_mode = config->key_config.b0.wrap_iv_mode;
  request->key_info.reserved                         = config->key_config.b0.reserved;
  if (config->key_config.b0.wrap_iv_mode) {
    memcpy(request->key_info.key_detail.key_spec.wrap_iv, config->key_config.b0.wrap_iv, SL_SI91X_IV_SIZE);
  }
  memcpy(request->key_info.key_detail.key_spec.key_buffer,
         config->key_config.b0.key_buffer,
         config->key_config.b0.key_size);
}
#endif

/*****************************************************************************
 * Build and send a side-band AES request to firmware.
 *
 * Allocates an sli_si91x_aes_mp_request_t, populates it from config and
 * the chunk parameters, then sends it via the side-band crypto interface.
 *
 * @param[in]  config       AES configuration (key, IV, mode, direction).
 * @param      aes_flags    Chunk flags (FIRST_CHUNK, MIDDLE_CHUNK, LAST_CHUNK).
 * @param      chunk_length Number of message bytes in this chunk.
 * @param[out] output       Buffer for firmware output (may be NULL for
 *                          FIRST_CHUNK / zero-length LAST_CHUNK).
 *
 * @return sl_status_t indicating success or failure.
******************************************************************************/
static sl_status_t sli_mp_aes_send(sl_si91x_aes_config_t *config,
                                   uint8_t aes_flags,
                                   uint16_t chunk_length,
                                   uint8_t *output)
{
  sl_status_t status                  = SL_STATUS_FAIL;
  sli_si91x_aes_mp_request_t *request = (sli_si91x_aes_mp_request_t *)malloc(sizeof(sli_si91x_aes_mp_request_t));
  if (request == NULL) {
    status = SL_STATUS_ALLOCATION_FAILED;
    return status;
  }

  memset(request, 0, sizeof(*request));

  request->algorithm_type       = AES;
  request->algorithm_sub_type   = (uint8_t)config->aes_mode;
  request->aes_flags            = aes_flags;
  request->total_msg_length     = config->msg_length;
  request->current_chunk_length = chunk_length;
  request->encrypt_decryption   = config->encrypt_decrypt;

  /*
   * IV is consumed by the firmware on FIRST_CHUNK only. For MIDDLE / LAST
   * the running IV / counter already lives inside the FW context, so we
   * deliberately omit it from the wire after init.
   */
  if ((aes_flags & FIRST_CHUNK) && config->iv != NULL) {
    request->IV = (uint8_t *)config->iv;
  }

  if (chunk_length > 0 && config->msg != NULL) {
    request->msg = (uint8_t *)config->msg;
  }
  if (output != NULL) {
    request->output = output;
  }

#if defined(SLI_SI917B0)
  /*
   * Re-send key descriptor on every chunk. The firmware multipart wrapper
   * reloads the AES key inside the per-chunk hardware lock so unrelated
   * crypto operations between chunks cannot corrupt the multipart state.
   */
  sli_si91x_aes_mp_get_key_info(request, config);
#endif

  status = sl_si91x_driver_send_side_band_crypto(SLI_COMMON_REQ_ENCRYPT_CRYPTO,
                                                 request,
                                                 sizeof(sli_si91x_aes_mp_request_t),
                                                 SLI_WIFI_WAIT_FOR_RESPONSE(SLI_COMMON_RSP_ENCRYPT_CRYPTO_WAIT_TIME));

  free(request);
  request = NULL;
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

/*****************************************************************************
 * Initialize a multipart AES operation (FIRST_CHUNK).
 *
 * Sends the first-packet command to firmware with key, IV (for CBC/CTR),
 * sub-mode and direction.  No plaintext or ciphertext is sent in this call.
 *
 * @param[in] config  AES configuration (key, IV, mode, direction).
 *
 * @return sl_status_t indicating success or failure.
******************************************************************************/
sl_status_t sl_si91x_mp_aes_init(sl_si91x_aes_config_t *config)
{
  sl_status_t status = SL_STATUS_OK;

  if (config == NULL) {
    status = SL_STATUS_INVALID_PARAMETER;
    return status;
  }

  if (config->aes_mode != SL_SI91X_AES_CBC && config->aes_mode != SL_SI91X_AES_CTR
      && config->aes_mode != SL_SI91X_AES_ECB) {
    status = SL_STATUS_NOT_SUPPORTED;
    return status;
  }

  /* CBC and CTR require an IV at init; ECB does not. */
  if ((config->aes_mode == SL_SI91X_AES_CBC || config->aes_mode == SL_SI91X_AES_CTR) && config->iv == NULL) {
    status = SL_STATUS_INVALID_PARAMETER;
    return status;
  }

  /*
   * Host-side key-size sanity check.  Firmware (sli_sb_aes_get_key_size)
   * also rejects bad sizes, but catching them here saves an NWP round-trip.
   */
#if defined(SLI_SI917B0)
  if (config->key_config.b0.key_size != SL_SI91X_AES_KEY_SIZE_128
      && config->key_config.b0.key_size != SL_SI91X_AES_KEY_SIZE_192
      && config->key_config.b0.key_size != SL_SI91X_AES_KEY_SIZE_256) {
    status = SL_STATUS_INVALID_PARAMETER;
    return status;
  }
#else
  if (config->key_config.a0.key_length != SL_SI91X_AES_KEY_SIZE_128
      && config->key_config.a0.key_length != SL_SI91X_AES_KEY_SIZE_192
      && config->key_config.a0.key_length != SL_SI91X_AES_KEY_SIZE_256) {
    status = SL_STATUS_INVALID_PARAMETER;
    return status;
  }
#endif

  status = sli_mp_aes_send(config, FIRST_CHUNK, 0, NULL);
  return status;
}

/*****************************************************************************
 * Send a middle-chunk AES command (MIDDLE_CHUNK).
 *
 * Sends one chunk of message data to firmware for encryption or decryption.
 *
 * @param[in]  config       AES configuration; msg must point to the chunk.
 * @param      chunk_length Number of bytes in this chunk (must be > 0).
 * @param[out] output       Buffer for encrypted / decrypted output.
 *
 * @return sl_status_t indicating success or failure.
******************************************************************************/
sl_status_t sl_si91x_mp_aes_update(sl_si91x_aes_config_t *config, uint16_t chunk_length, uint8_t *output)
{
  sl_status_t status = SL_STATUS_OK;

  if (config == NULL || config->msg == NULL || chunk_length == 0 || output == NULL) {
    status = SL_STATUS_INVALID_PARAMETER;
    return status;
  }

  /* CBC/ECB require 16-byte aligned chunks; CTR accepts arbitrary lengths. */
  if ((config->aes_mode == SL_SI91X_AES_CBC || config->aes_mode == SL_SI91X_AES_ECB)
      && (chunk_length % SL_SI91X_AES_BLOCK_SIZE)) {
    status = SL_STATUS_INVALID_PARAMETER;
    return status;
  }

  status = sli_mp_aes_send(config, MIDDLE_CHUNK, chunk_length, output);
  return status;
}

/*****************************************************************************
 * Send the last-chunk AES command (LAST_CHUNK) to finalize the operation.
 *
 * The firmware runs sl_sb_aes_final + sl_sb_aes_reset, which clears the
 * multipart context.  chunk_length may be 0 if all data was already sent
 * via update().
 *
 * @param[in]  config       AES configuration.
 * @param      chunk_length Number of bytes in the final chunk (can be 0).
 * @param[out] output       Buffer for firmware output (may be NULL when
 *                          chunk_length is 0).
 *
 * @return sl_status_t indicating success or failure.
******************************************************************************/
sl_status_t sl_si91x_mp_aes_final(sl_si91x_aes_config_t *config, uint16_t chunk_length, uint8_t *output)
{
  sl_status_t status = SL_STATUS_OK;

  if (config == NULL) {
    status = SL_STATUS_INVALID_PARAMETER;
    return status;
  }

  /* CBC/ECB require 16-byte aligned chunks; CTR accepts arbitrary lengths. */
  if (chunk_length > 0) {
    if (config->msg == NULL || output == NULL) {
      status = SL_STATUS_INVALID_PARAMETER;
      return status;
    }
    if ((config->aes_mode == SL_SI91X_AES_CBC || config->aes_mode == SL_SI91X_AES_ECB)
        && (chunk_length % SL_SI91X_AES_BLOCK_SIZE)) {
      status = SL_STATUS_INVALID_PARAMETER;
      return status;
    }
  }

  status = sli_mp_aes_send(config, LAST_CHUNK, chunk_length, output);
  return status;
}

#endif /* SL_SI91X_SIDE_BAND_CRYPTO */
