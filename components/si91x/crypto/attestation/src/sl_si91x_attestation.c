/***************************************************************************/ /**
 * @file
 * @brief SL SI91x ATTESTATION API
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

#include "sl_si91x_crypto.h"
#include "sl_status.h"
#include "sl_constants.h"
#include "sl_si91x_protocol_types.h"
#include "sl_si91x_driver.h"
#include <string.h>
#include "sl_si91x_attestation.h"

/**
 * @fn    sl_status_t sl_si91x_attestation_get_token(uint8_t *token, uint16_t length, uint32_t *nonce)
 * @brief \Process the nonce for attestation token
 * @param[in] length
 * @param[in] nonce
 * @param[out] token
 * @return return sl_status code 
 *      
 */
sl_status_t sl_si91x_attestation_get_token(uint8_t *token, uint16_t length, uint32_t *nonce)
{
  sl_status_t status;
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  sl_wifi_buffer_t *buffer;
  sl_si91x_packet_t *packet;

  sl_si91x_rsi_token_req_t *attest = (sl_si91x_rsi_token_req_t *)malloc(sizeof(sl_si91x_rsi_token_req_t));

  if (attest == NULL) {
    return SL_STATUS_ALLOCATION_FAILED;
  };

  if ((token == NULL) && (length == 0)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Memset before filling
  memset(attest, 0, sizeof(sl_si91x_rsi_token_req_t));

  // Fill Algorithm type ATTESTATION - 30
  attest->algorithm_type = ATTESTATION;

  // Update the length
  attest->total_msg_length = length;

  // Send 32 byte nonce
  //! Memset before filling
  memset(&attest->msg[0], 0, NONCE_DATA_SIZE);

  //! Copy Data
  memcpy(&attest->msg[0], nonce, NONCE_DATA_SIZE);

  status = sl_si91x_driver_send_command(RSI_COMMON_REQ_ENCRYPT_CRYPTO,
                                        SI91X_COMMON_CMD_QUEUE,
                                        attest,
                                        sizeof(sl_si91x_rsi_token_req_t),
                                        SL_SI91X_WAIT_FOR_RESPONSE(600000),
                                        NULL,
                                        &buffer);
  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
    return status;
  }
  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);

  memcpy(token, packet->data, packet->length);

  free(attest);
  sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
  return status;
}