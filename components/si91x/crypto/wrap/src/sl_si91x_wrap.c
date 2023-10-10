/***************************************************************************/ /**
 * @file
 * @brief SL SI91X Wrap source file
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

#include "sl_si91x_wrap.h"
#include "sl_si91x_crypto.h"
#include "sl_status.h"
#include "sl_constants.h"
#include "sl_si91x_protocol_types.h"
#include "sl_si91x_driver.h"
#include "string.h"

static sl_status_t wrap_pending(sl_si91x_wrap_config_t *config,
                                uint8_t wrap_flags,
                                uint16_t chunk_length,
                                uint8_t *output)
{
  sl_status_t status               = SL_STATUS_FAIL;
  sl_wifi_buffer_t *buffer         = NULL;
  sl_si91x_packet_t *packet        = NULL;
  sl_si91x_wrap_request_t *request = (sl_si91x_wrap_request_t *)malloc(sizeof(sl_si91x_wrap_request_t));
  SL_VERIFY_POINTER_OR_RETURN(request, SL_STATUS_ALLOCATION_FAILED);

  memset(request, 0, sizeof(sl_si91x_wrap_request_t));

  request->algorithm_type        = WRAP;
  request->current_chunk_length  = chunk_length;
  request->wrap_flags            = wrap_flags;
  request->key_info.key_size     = config->key_size;
  request->key_info.key_type     = config->key_type;
  request->key_info.reserved     = config->reserved;
  request->key_info.wrap_iv_mode = config->wrap_iv_mode;
  memcpy(request->key_info.key_buffer, config->key_buffer, config->key_size);

  if (config->wrap_iv_mode == SL_SI91X_WRAP_IV_CBC_MODE) {
    memcpy(request->key_info.wrap_iv, config->wrap_iv, SL_SI91X_IV_SIZE);
  }

  status =
    sl_si91x_driver_send_command(RSI_COMMON_REQ_ENCRYPT_CRYPTO,
                                 SI91X_COMMON_CMD_QUEUE,
                                 request,
                                 (sizeof(sl_si91x_wrap_request_t) - SL_SI91X_MAX_DATA_SIZE_IN_BYTES + chunk_length),
                                 SL_SI91X_WAIT_FOR_RESPONSE(32000),
                                 NULL,
                                 &buffer);
  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
  }
  VERIFY_STATUS_AND_RETURN(status);

  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  memcpy(output, packet->data, config->key_size);

  free(request);
  sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);

  return status;
}

sl_status_t sl_si91x_wrap(sl_si91x_wrap_config_t *config, uint8_t *output)
{
  sl_status_t status    = SL_STATUS_FAIL;
  uint16_t chunk_len    = 0;
  uint16_t offset       = 0;
  uint8_t wrap_flags    = 0;
  uint32_t total_length = config->key_size;

  SL_VERIFY_POINTER_OR_RETURN(config->key_buffer, SL_STATUS_NULL_POINTER);
  uint8_t *key_buffer = config->key_buffer;

  while (total_length) {
    // Check total length
    if (total_length > SL_SI91X_MAX_DATA_SIZE_IN_BYTES) {
      chunk_len = SL_SI91X_MAX_DATA_SIZE_IN_BYTES;
      if (offset == 0) {
        // Make wrap_flags as first chunk
        wrap_flags |= FIRST_CHUNK;
      } else {
        // Make wrap_flags as Last chunk
        wrap_flags = MIDDLE_CHUNK;
      }
    } else {
      chunk_len  = total_length;
      wrap_flags = LAST_CHUNK;
      if (offset == 0) {
        // If the total length is less than 1400 and offset is zero, make wrap_flags as both first chunk as well as last chunk
        wrap_flags |= FIRST_CHUNK;
      }
    }

    // Send the current chunk length message
    status = wrap_pending(config, wrap_flags, chunk_len, output);
    if (status != SL_STATUS_OK) {
      return status;
    }

    // Increment the offset value
    offset += chunk_len;
    key_buffer += chunk_len;

    // Decrement the total message length
    total_length -= chunk_len;
  }

  return status;
}
