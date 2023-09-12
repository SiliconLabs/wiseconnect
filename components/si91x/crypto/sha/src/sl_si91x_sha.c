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
#include "sl_si91x_driver.h"
#include <string.h>
/*==============================================*/
/**
 * @brief      Compute the SHA digest. This is a blocking API.
 * @param[in]  sha_mode     1 – For SHA1 \n 2 – For SHA256 \n 3 – For SHA384 \n 4 – For SHA512 \n 5 – For SHA224 \n
 * @param[in]  msg          - Pointer to message
 * @param[in]  msg_length   - Total message length
 * @param[in]  chunk_length - Current chunk length
 * @param[in]  pending_flag - BIT(0) - 1st chunk \n BIT(1) - Middle chunk \n BIT(2) - Last chunk \n
 * @param[out]  digest      - Output parameter to hold computed digest from SHA
 * @return     0              - Success \n
 *             Non-Zero Value - Failure
 * @note Refer Error Codes section for above error codes \ref error-codes.
 *
 */
static sl_status_t sha_pending(uint8_t sha_mode,
                               uint8_t *msg,
                               uint16_t msg_length,
                               uint16_t chunk_len,
                               uint8_t pending_flag,
                               uint8_t *digest)
{
  sl_status_t status = SL_STATUS_OK;
  SL_PRINTF(SL_SHA_PEN_ENTRY, CRYPTO, LOG_INFO);
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  //  uint8_t digest_len = 0;
  sl_wifi_buffer_t *buffer;
  sl_si91x_packet_t *packet;
  sl_si91x_sha_request_t *request = (sl_si91x_sha_request_t *)malloc(sizeof(sl_si91x_sha_request_t));

  if (request == NULL) {
    return SL_STATUS_ALLOCATION_FAILED;
  }

  // Input pointer check
  if (msg == NULL) {
    free(request);
    return SL_STATUS_INVALID_PARAMETER;
  }

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

  // Data
  // Memset before filling
  memset(&request->msg[0], 0, SL_SI91X_MAX_DATA_SIZE_IN_BYTES);

  // Copy Data
  memcpy(&request->msg[0], msg, chunk_len);

  send_size = sizeof(sl_si91x_sha_request_t) - SL_SI91X_MAX_DATA_SIZE_IN_BYTES + chunk_len;

  status = sl_si91x_driver_send_command(RSI_COMMON_REQ_ENCRYPT_CRYPTO,
                                        SI91X_COMMON_CMD_QUEUE,
                                        request,
                                        send_size,
                                        SL_SI91X_WAIT_FOR_RESPONSE(32000),
                                        NULL,
                                        &buffer);
  if (status != SL_STATUS_OK) {
    free(request);
    sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
    return status;
  }

  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  memcpy(digest, packet->data, request->total_msg_length);

  free(request);
  sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);

  return status;
}

/** @} */

/** @addtogroup CRYPTO
* @{
*/
/*==============================================*/
/**
 * @brief      Decide whether the SHA message can be sent once or requires multiple calls to send. This is a blocking API.
 * @param[in]  sha_mode   1 – For SHA1 \n 2 – For SHA256 \n 3 – For SHA384 \n 4 – For SHA512 \n  5– For SHA224 \n
 * @param[in]  msg        - Pointer to message
 * @param[in]  msg_length - Total message length
 * @param[out]  digest    - Output parameter to hold computed digest from SHA
 * @return     0              - Success \n
 *             Non-Zero Value - Failure
 *
 * @note Refer Error Codes section for above error codes \ref error-codes.
 *
 */
sl_status_t sl_si91x_sha(uint8_t sha_mode, uint8_t *msg, uint16_t msg_length, uint8_t *digest)
{
  sl_status_t status = SL_STATUS_OK;
  SL_PRINTF(SL_SHA_ENTRY, CRYPTO, LOG_INFO);
  uint16_t total_len = 0;
  uint16_t chunk_len = 0;
  uint16_t offset    = 0;
  uint8_t sha_flags  = 0;

  total_len = msg_length;

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
    status = sha_pending(sha_mode, msg, msg_length, chunk_len, sha_flags, digest);

    if (status != SL_STATUS_OK) {
      SL_PRINTF(SL_SHA_CHUNK_LENGTH_MSG_ERROR, CRYPTO, LOG_ERROR, "status: %4x", status);
      return status;
    }

    // Increment the offset value
    offset += chunk_len;
    msg += chunk_len;

    // Decrement the total message lenth
    total_len -= chunk_len;
  }
  SL_PRINTF(SL_SHA_EXIT, CRYPTO, LOG_INFO, "status: %4x", status);
  return status;
}
