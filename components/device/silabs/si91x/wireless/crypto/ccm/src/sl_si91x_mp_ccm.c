/***************************************************************************/ /**
 * @file
 * @brief SL SI91X Multipart CCM – side-band first/middle/last packet helpers
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

#include "sl_si91x_mp_ccm.h"
#include "sl_si91x_crypto.h"
#include "sl_status.h"
#include "sl_constants.h"
#include "sl_si91x_protocol_types.h"
#include "sl_si91x_driver.h"
#include <string.h>

#ifdef SL_SI91X_SIDE_BAND_CRYPTO

#if defined(SLI_SI917B0)
/*****************************************************************************
 * Copy B0 key configuration into a multipart CCM request.
 *
 * @param[in,out] request  Multipart CCM request whose key_info fields will
 *                         be populated.
 * @param[in]     config   CCM configuration containing the B0 key material.
******************************************************************************/
void sl_si91x_ccm_mp_get_key_info(sli_si91x_ccm_mp_request_t *request, const sl_si91x_ccm_config_t *config)
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
 * Build and send a side-band CCM request to firmware.
 *
 * Allocates an sli_si91x_ccm_mp_request_t, populates it from config and
 * the chunk parameters, then sends it via the side-band crypto interface.
 *
 * @param[in]  config       CCM configuration (key, nonce, AD, tag, etc.).
 * @param      ccm_flags    Chunk flags (FIRST_CHUNK, MIDDLE_CHUNK, LAST_CHUNK).
 * @param      chunk_length Number of message bytes in this chunk.
 * @param[out] output       Buffer for firmware output (may be NULL for
 *                          FIRST_CHUNK / zero-length LAST_CHUNK).
 *
 * @return sl_status_t indicating success or failure.
******************************************************************************/
static sl_status_t sli_mp_ccm_send(sl_si91x_ccm_config_t *config,
                                   uint8_t ccm_flags,
                                   uint16_t chunk_length,
                                   uint8_t *output)
{
  sl_status_t status = SL_STATUS_FAIL;

  sli_si91x_ccm_mp_request_t *request = (sli_si91x_ccm_mp_request_t *)malloc(sizeof(sli_si91x_ccm_mp_request_t));
  if (request == NULL) {
    status = SL_STATUS_ALLOCATION_FAILED;
    return status;
  }

  memset(request, 0, sizeof(*request));

  request->algorithm_type       = CCM;
  request->ccm_flags            = ccm_flags;
  request->nonce_length         = (uint8_t)config->nonce_length;
  request->encrypt_decryption   = config->encrypt_decrypt;
  request->total_msg_length     = config->msg_length;
  request->current_chunk_length = chunk_length;
  request->ad_length            = config->ad_length;
  request->tag_length           = config->tag_length;

  if (ccm_flags & FIRST_CHUNK) {
    request->nonce = (uint8_t *)config->nonce;
    request->tag   = config->tag;
    if (config->ad_length > 0) {
      request->ad = (uint8_t *)config->ad;
    }
#if defined(SLI_SI917B0)
    sl_si91x_ccm_mp_get_key_info(request, config);
#endif
  }

  if ((ccm_flags & LAST_CHUNK) && config->tag != NULL) {
    request->tag = config->tag;
  }

  if (chunk_length > 0 && config->msg != NULL) {
    request->msg = (uint8_t *)config->msg;
  }
  if (output != NULL) {
    request->output = output;
  }

  status = sl_si91x_driver_send_side_band_crypto(SLI_COMMON_REQ_ENCRYPT_CRYPTO,
                                                 request,
                                                 sizeof(sli_si91x_ccm_mp_request_t),
                                                 SLI_WIFI_WAIT_FOR_RESPONSE(SLI_COMMON_RSP_ENCRYPT_CRYPTO_WAIT_TIME));

  free(request);
  request = NULL;
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

#endif /* SL_SI91X_SIDE_BAND_CRYPTO */

/*****************************************************************************
 * Initialize a multipart CCM operation (FIRST_CHUNK).
 *
 * Sends the first-packet command to firmware with key, nonce, AD, and tag
 * configuration.  No plaintext or ciphertext is sent in this call.
 *
 * @param[in] config  CCM configuration (key, nonce, AD, tag, direction).
 *
 * @return sl_status_t indicating success or failure.
******************************************************************************/
sl_status_t sl_si91x_mp_ccm_init(sl_si91x_ccm_config_t *config)
{
  sl_status_t status = SL_STATUS_FAIL;

#ifdef SL_SI91X_SIDE_BAND_CRYPTO
  if (config == NULL || config->nonce == NULL || config->tag == NULL) {
    status = SL_STATUS_INVALID_PARAMETER;
    return status;
  }
  if (config->ad == NULL && config->ad_length != 0) {
    status = SL_STATUS_INVALID_PARAMETER;
    return status;
  }

  status = sli_mp_ccm_send(config, FIRST_CHUNK, 0, NULL);
  return status;
#else
  UNUSED_VARIABLE(config);
  status = SL_STATUS_NOT_SUPPORTED;
  return status;
#endif
}

/*****************************************************************************
 * Send a middle-chunk CCM command (MIDDLE_CHUNK).
 *
 * Sends one chunk of message data to firmware for encryption or decryption.
 *
 * @param[in]  config       CCM configuration; msg must point to the chunk.
 * @param      chunk_length Number of bytes in this chunk (must be > 0).
 * @param[out] output       Buffer for encrypted / decrypted output.
 *
 * @return sl_status_t indicating success or failure.
******************************************************************************/
sl_status_t sl_si91x_mp_ccm_update(sl_si91x_ccm_config_t *config, uint16_t chunk_length, uint8_t *output)
{
  sl_status_t status = SL_STATUS_FAIL;

#ifdef SL_SI91X_SIDE_BAND_CRYPTO
  if (config == NULL || config->msg == NULL || chunk_length == 0 || output == NULL) {
    status = SL_STATUS_INVALID_PARAMETER;
    return status;
  }

  status = sli_mp_ccm_send(config, MIDDLE_CHUNK, chunk_length, output);
  return status;
#else
  UNUSED_VARIABLE(config);
  UNUSED_VARIABLE(chunk_length);
  UNUSED_VARIABLE(output);
  status = SL_STATUS_NOT_SUPPORTED;
  return status;
#endif
}

/*****************************************************************************
 * Send the last-chunk CCM command (LAST_CHUNK) to finalize the operation.
 *
 * For encryption the firmware produces the authentication tag.  For
 * decryption the firmware verifies the tag.  chunk_length may be 0 if
 * all data was sent via update().
 *
 * @param[in]  config       CCM configuration; tag must be set for decrypt.
 * @param      chunk_length Number of bytes in the final chunk (can be 0).
 * @param[out] output       Buffer for firmware output (may be NULL when
 *                          chunk_length is 0).
 *
 * @return sl_status_t indicating success or failure.
******************************************************************************/
sl_status_t sl_si91x_mp_ccm_final(sl_si91x_ccm_config_t *config, uint16_t chunk_length, uint8_t *output)
{
  sl_status_t status = SL_STATUS_FAIL;

#ifdef SL_SI91X_SIDE_BAND_CRYPTO
  if (config == NULL) {
    status = SL_STATUS_INVALID_PARAMETER;
    return status;
  }
  /* When the final chunk carries data, apply the same validation as
   * sl_si91x_mp_ccm_update to prevent firmware null-pointer access. */
  if (chunk_length > 0 && (config->msg == NULL || output == NULL)) {
    status = SL_STATUS_INVALID_PARAMETER;
    return status;
  }

  status = sli_mp_ccm_send(config, LAST_CHUNK, chunk_length, output);
  return status;
#else
  UNUSED_VARIABLE(config);
  UNUSED_VARIABLE(chunk_length);
  UNUSED_VARIABLE(output);
  status = SL_STATUS_NOT_SUPPORTED;
  return status;
#endif
}
