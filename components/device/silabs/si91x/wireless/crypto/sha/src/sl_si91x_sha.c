/*******************************************************************************
 * @file  sl_si91x_sha.c
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

#include "rsi_driver.h"
#include "sl_si91x_crypto.h"
#include "sl_status.h"
#include "sl_constants.h"
#include "sl_si91x_protocol_types.h"
#if defined(SLI_MULTITHREAD_DEVICE_SI91X)
#include "sl_si91x_crypto_thread.h"
#endif
#include "sl_si91x_driver.h"
#include "sl_si91x_sha.h"
#include <string.h>

#ifndef SL_SI91X_SIDE_BAND_CRYPTO
static const uint8_t sha_digest_len_table[] = { [SL_SI91X_SHA_1]   = SL_SI91X_SHA_1_DIGEST_LEN,
                                                [SL_SI91X_SHA_256] = SL_SI91X_SHA_256_DIGEST_LEN,
                                                [SL_SI91X_SHA_384] = SL_SI91X_SHA_384_DIGEST_LEN,
                                                [SL_SI91X_SHA_512] = SL_SI91X_SHA_512_DIGEST_LEN,
                                                [SL_SI91X_SHA_224] = SL_SI91X_SHA_224_DIGEST_LEN };

static sl_status_t sli_si91x_sha_pending(uint8_t sha_mode,
                                         uint8_t *msg,
                                         uint16_t msg_length,
                                         uint16_t chunk_len,
                                         uint8_t pending_flag,
                                         uint8_t *digest)
{
  // Input pointer check
  if ((msg == NULL) && (msg_length != 0)) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_status_t status = SL_STATUS_OK;
  SL_PRINTF(SL_SHA_PEN_ENTRY, CRYPTO, LOG_INFO);
  uint16_t send_size = 0;
  sl_wifi_buffer_t *buffer;
  sl_si91x_packet_t *packet;
  sl_si91x_sha_request_t *request = (sl_si91x_sha_request_t *)malloc(sizeof(sl_si91x_sha_request_t));

  SL_VERIFY_POINTER_OR_RETURN(request, SL_STATUS_ALLOCATION_FAILED);

  memset(request, 0, sizeof(sl_si91x_sha_request_t));

  // Fill Algorithm type SHA - 4
  request->algorithm_type = SHA;

  request->algorithm_sub_type = sha_mode;

  // Fill sha_flags BIT(0) - 1st chunk BIT(2) - Last chunk
  request->sha_flags = pending_flag;

  // Fill total msg length
  request->total_msg_length = msg_length;

  // Fill current chunk length
  request->current_chunk_length = chunk_len;

  // Memset before filling
  memset(&request->msg[0], 0, SL_SI91X_MAX_DATA_SIZE_IN_BYTES);

  // Copy Data
  memcpy(&request->msg[0], msg, chunk_len);

  send_size = sizeof(sl_si91x_sha_request_t) - SL_SI91X_MAX_DATA_SIZE_IN_BYTES + chunk_len;

  status = sl_si91x_driver_send_command(RSI_COMMON_REQ_ENCRYPT_CRYPTO,
                                        SI91X_COMMON_CMD,
                                        request,
                                        send_size,
                                        SL_SI91X_WAIT_FOR_RESPONSE(32000),
                                        NULL,
                                        &buffer);
  if (status != SL_STATUS_OK) {
    free(request);
    if (buffer != NULL)
      sl_si91x_host_free_buffer(buffer);
  }
  VERIFY_STATUS_AND_RETURN(status);

  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);

  if (pending_flag == (LAST_CHUNK | FIRST_CHUNK) || pending_flag == LAST_CHUNK) {
    SL_ASSERT(packet->length == sha_digest_len_table[sha_mode]);
  }
  memcpy(digest, packet->data, sha_digest_len_table[sha_mode]);

  free(request);
  if (buffer != NULL)
    sl_si91x_host_free_buffer(buffer);

  return status;
}

#else
static sl_status_t sli_si91x_sha_side_band(uint8_t sha_mode, uint8_t *msg, uint16_t msg_length, uint8_t *digest)
{
  // Input pointer check
  if (msg == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_status_t status              = SL_STATUS_OK;
  sl_si91x_sha_request_t *request = (sl_si91x_sha_request_t *)malloc(sizeof(sl_si91x_sha_request_t));

  if (request == NULL) {
    return SL_STATUS_ALLOCATION_FAILED;
  }

  memset(request, 0, sizeof(sl_si91x_sha_request_t));

  // Fill Algorithm type SHA - 4
  request->algorithm_type = SHA;

  request->algorithm_sub_type = sha_mode;

  // Fill total msg length
  request->total_msg_length = msg_length;

  // Fill msg ptr
  request->msg = msg;

  // Fill msg ptr
  request->output = digest;

  status = sl_si91x_driver_send_side_band_crypto(RSI_COMMON_REQ_ENCRYPT_CRYPTO,
                                                 request,
                                                 (sizeof(sl_si91x_sha_request_t)),
                                                 SL_SI91X_WAIT_FOR_RESPONSE(32000));
  free(request);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}
#endif

sl_status_t sl_si91x_sha(uint8_t sha_mode, uint8_t *msg, uint16_t msg_length, uint8_t *digest)
{
  sl_status_t status = SL_STATUS_OK;
  SL_PRINTF(SL_SHA_ENTRY, CRYPTO, LOG_INFO);

#if defined(SLI_MULTITHREAD_DEVICE_SI91X)
  if (crypto_sha_mutex == NULL) {
    crypto_sha_mutex = sl_si91x_crypto_threadsafety_init(crypto_sha_mutex);
  }
  mutex_result = sl_si91x_crypto_mutex_acquire(crypto_sha_mutex);
#endif

#ifdef SL_SI91X_SIDE_BAND_CRYPTO
  status = sli_si91x_sha_side_band(sha_mode, msg, msg_length, digest);
  return status;
#else
  uint16_t total_len = 0;
  uint16_t chunk_len = 0;
  uint16_t offset    = 0;
  uint8_t sha_flags  = 0;

  total_len = msg_length;
  if (total_len != 0) {
    while (total_len) {
      // Check total length
      if (total_len > SL_SI91X_MAX_DATA_SIZE_IN_BYTES) {
        chunk_len = SL_SI91X_MAX_DATA_SIZE_IN_BYTES;
        if (offset == 0) {
          // Make sha_flag as first chunk
          sha_flags |= FIRST_CHUNK;
        } else {
          // Make sha_flag as Middle chunk
          sha_flags = MIDDLE_CHUNK;
        }
      } else {
        chunk_len = total_len;

        // Make sha_flag as Last chunk
        sha_flags = LAST_CHUNK;
        if (offset == 0) {
          /* If the total length is less than 1400 and offset is zero,
             make sha_flag as both first chunk as well as last chunk*/
          sha_flags |= FIRST_CHUNK;
        }
      }

      // Send the current chunk length message
      status = sli_si91x_sha_pending(sha_mode, msg, msg_length, chunk_len, sha_flags, digest);

      if (status != SL_STATUS_OK) {
        SL_PRINTF(SL_SHA_CHUNK_LENGTH_MSG_ERROR, CRYPTO, LOG_ERROR, "status: %4x", status);
#if defined(SLI_MULTITHREAD_DEVICE_SI91X)
        mutex_result = sl_si91x_crypto_mutex_release(crypto_sha_mutex);
#endif
        return status;
      }

      // Increment the offset value
      offset += chunk_len;
      msg += chunk_len;

      // Decrement the total message length
      total_len -= chunk_len;
    }
  } else {
    sha_flags = LAST_CHUNK | FIRST_CHUNK;
    status    = sli_si91x_sha_pending(sha_mode, msg, msg_length, chunk_len, sha_flags, digest);
    if (status != SL_STATUS_OK) {
      SL_PRINTF(SL_SHA_CHUNK_LENGTH_MSG_ERROR, CRYPTO, LOG_ERROR, "status: %4x", status);
#if defined(SLI_MULTITHREAD_DEVICE_SI91X)
      mutex_result = sl_si91x_crypto_mutex_release(crypto_sha_mutex);
#endif
      return status;
    }
  }

#if defined(SLI_MULTITHREAD_DEVICE_SI91X)
  mutex_result = sl_si91x_crypto_mutex_release(crypto_sha_mutex);
#endif

  SL_PRINTF(SL_SHA_EXIT, CRYPTO, LOG_INFO, "status: %4x", status);
  return status;
#endif
}
