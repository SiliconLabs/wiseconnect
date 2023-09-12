/***************************************************************************/ /**
 * @file
 * @brief SL SI91X AES source file
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

#include "sl_si91x_aes.h"
#include "sl_si91x_crypto.h"
#include "sl_status.h"
#include "sl_constants.h"
#include "sl_si91x_protocol_types.h"
#include "sl_si91x_driver.h"
#include <string.h>

static sl_status_t aes_pending(sl_si91x_aes_config_t *config, uint16_t chunk_length, uint8_t aes_flags, uint8_t *output)
{
  sl_status_t status              = SL_STATUS_FAIL;
  sl_wifi_buffer_t *buffer        = NULL;
  sl_si91x_packet_t *packet       = NULL;
  sl_si91x_aes_request_t *request = (sl_si91x_aes_request_t *)malloc(sizeof(sl_si91x_aes_request_t));
  SL_VERIFY_POINTER_OR_RETURN(request, SL_STATUS_ALLOCATION_FAILED);

  memset(request, 0, sizeof(sl_si91x_aes_request_t));

  request->algorithm_type       = AES;
  request->algorithm_sub_type   = config->aes_mode;
  request->aes_flags            = aes_flags;
  request->total_msg_length     = config->msg_length;
  request->current_chunk_length = chunk_length;
  request->encrypt_decryption   = config->encrypt_decrypt;
  if (config->iv != NULL) {
    memcpy(request->IV, config->iv, SL_SI91X_IV_SIZE);
  }
  memcpy(request->msg, config->msg, chunk_length);

#ifdef CHIP_917B0
  request->key_info.key_type                         = config->key_config.b0.key_type;
  request->key_info.key_detail.key_size              = config->key_config.b0.key_size;
  request->key_info.key_detail.key_spec.key_slot     = config->key_config.b0.key_slot;
  request->key_info.key_detail.key_spec.wrap_iv_mode = config->key_config.b0.wrap_iv_mode;
  request->key_info.reserved                         = config->key_config.b0.reserved;
  memcpy(request->key_info.key_detail.key_spec.wrap_iv, config->key_config.b0.wrap_iv, SL_SI91X_IV_SIZE);
  memcpy(request->key_info.key_detail.key_spec.key_buffer, config->key_config.b0.key_buffer, SL_SI91X_KEY_BUFFER_SIZE);
#else
  memcpy(request->key, config->key_config.a0.key, request->key_length);
  request->key_length = config->key_config.a0.key_length;
#endif

  status =
    sl_si91x_driver_send_command(RSI_COMMON_REQ_ENCRYPT_CRYPTO,
                                 SI91X_COMMON_CMD_QUEUE,
                                 request,
                                 (sizeof(sl_si91x_aes_request_t) - SL_SI91X_MAX_DATA_SIZE_IN_BYTES + chunk_length),
                                 SL_SI91X_WAIT_FOR_RESPONSE(32000),
                                 NULL,
                                 &buffer);
  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
  }
  VERIFY_STATUS_AND_RETURN(status);

  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  memcpy(output, packet->data, request->total_msg_length);

  free(request);
  sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);

  return status;
}

sl_status_t sl_si91x_aes(sl_si91x_aes_config_t *config, uint8_t *output)
{
  uint16_t chunk_len = 0;
  uint16_t offset    = 0;
  uint8_t aes_flags  = 0;
  sl_status_t status = SL_STATUS_FAIL;

  SL_VERIFY_POINTER_OR_RETURN(config->msg, SL_STATUS_NULL_POINTER);

  if (((config->aes_mode == SL_SI91X_AES_CBC) || (config->aes_mode == SL_SI91X_AES_CTR)) && (config->iv == NULL)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  uint16_t total_length = config->msg_length;

  while (total_length) {
    // Check total length
    if (total_length > SL_SI91X_MAX_DATA_SIZE_IN_BYTES) {
      chunk_len = SL_SI91X_MAX_DATA_SIZE_IN_BYTES;
      if (offset == 0) {
        // Make aes_flags as first chunk
        aes_flags |= FIRST_CHUNK;
      } else {
        // Make aes_flags as last chunk
        aes_flags = MIDDLE_CHUNK;
      }
    } else {
      chunk_len = total_length;
      aes_flags = LAST_CHUNK;
      if (offset == 0) {
        // If the total length is less than 1400 and offset is zero, make aes_flags as both first chunk as well as last chunk
        aes_flags |= FIRST_CHUNK;
      }
    }

    // Send the current chunk length message
    status = aes_pending(config, chunk_len, aes_flags, output);
    if (status != SL_STATUS_OK) {
      return status;
    }

    // Increment the offset value
    offset += chunk_len;
    config->msg += chunk_len;

    // Decrement the total message length
    total_length -= chunk_len;
  }

  return status;
}
