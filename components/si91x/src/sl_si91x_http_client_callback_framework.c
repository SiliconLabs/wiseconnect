/***************************************************************************/ /**
 * @file
 * @brief sl_si91x_http_client_callback_framework.c
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_si91x_http_client_callback_framework.h"
#include "sl_constants.h"
#include <string.h>

/******************************************************
 *                    Constants
 ******************************************************/
//! HTTP OFFSET
#define SI91X_HTTP_OFFSET 12

//! HTTP PUT OFFSET
#define SI91X_HTTP_PUT_OFFSET 16

/******************************************************
 *                    Structures
 ******************************************************/
/**
 * Callback entry structure for sl_http_client events
 */
typedef struct {
  sl_http_client_t client_handle;                 ///< HTTP client handle
  sl_http_client_event_handler callback_function; ///< User provided callback function pointer
} sl_http_client_callback_entry_t;

/******************************************************
 *                 Global Variables
 ******************************************************/
static sl_http_client_callback_entry_t registered_callback[SL_HTTP_CLIENT_MAX_EVENT] = { 0 };

/******************************************************
 *               Function Declarations
 ******************************************************/
static sl_http_client_callback_entry_t *get_http_client_callback_entry(sl_http_client_event_t event);
static sl_status_t sl_si91x_http_client_put_delete(void);

/******************************************************
 *               Function Definitions
 ******************************************************/
sl_status_t sl_http_client_register_callback(sl_http_client_event_t event,
                                             sl_http_client_t client_handle,
                                             sl_http_client_event_handler function)
{
  sl_http_client_callback_entry_t *entry = get_http_client_callback_entry(event);
  if (entry != NULL) {
    entry->client_handle     = client_handle;
    entry->callback_function = function;
    return SL_STATUS_OK;
  }
  return SL_STATUS_FAIL;
}

sl_status_t sl_si91x_http_client_put_delete(void)
{
  sl_status_t status = SL_STATUS_OK;
  sl_si91x_http_client_put_request_t *request =
    (sl_si91x_http_client_put_request_t *)malloc(sizeof(sl_si91x_http_client_put_request_t));

  // Set command to delete HTTP put client
  request->command_type = SI91X_HTTP_CLIENT_PUT_DELETE;

  // Set packet length
  uint16_t packet_len = sizeof(sl_si91x_http_client_put_request_t) - SI91X_HTTP_CLIENT_PUT_MAX_BUFFER_LENGTH;

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_HTTP_CLIENT_PUT,
                                        SI91X_NETWORK_CMD_QUEUE,
                                        request,
                                        packet_len,
                                        SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                        NULL,
                                        NULL);
  free(request);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

static sl_http_client_callback_entry_t *get_http_client_callback_entry(sl_http_client_event_t event)
{
  return &registered_callback[event];
}

sl_status_t sl_http_client_default_event_handler(sl_http_client_event_t event,
                                                 sl_wifi_buffer_t *buffer,
                                                 void *sdk_context)
{
  sl_http_client_callback_entry_t *entry  = get_http_client_callback_entry(event);
  sl_si91x_packet_t *packet               = (sl_si91x_packet_t *)sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  sl_status_t status                      = convert_firmware_status(get_si91x_frame_status(packet));
  sl_http_client_response_t http_response = { 0 };
  uint16_t end_of_data;
  uint16_t http_server_response;

  http_response.status = status;

  // 917 does not support response header in current release
  http_response.response_headers = NULL;

  if ((entry == NULL) && (entry->callback_function == NULL)) {
    return SL_STATUS_FAIL;
  }

  SL_DEBUG_LOG("\r\n>>> %s : %x <<<\r\n", __func__, status);

  switch (packet->command) {
    case RSI_WLAN_RSP_HTTP_CLIENT_GET:
    case RSI_WLAN_RSP_HTTP_CLIENT_POST:
    case RSI_WLAN_RSP_HTTP_CLIENT_POST_DATA: {
      if (status == SL_STATUS_OK) {
        // Extract http server response from packet
        memcpy(&http_server_response, packet->data + 2, sizeof(uint16_t));

        // Fill http server response in callback response
        http_response.http_response_code = http_server_response;

        // Check http server response and fill other callback response data
        if (http_server_response == 0 || (http_server_response >= 200 && http_server_response <= 299)) {
          // Extract end of data indication from packet
          memcpy(&end_of_data, packet->data, sizeof(uint16_t));

          http_response.end_of_data = end_of_data;
          http_response.data_buffer = &packet->data[SI91X_HTTP_OFFSET];
          http_response.data_length = packet->length - SI91X_HTTP_OFFSET;
        }
      }
      break;
    }

    case RSI_WLAN_RSP_HTTP_CLIENT_PUT: {
      uint8_t http_cmd_type                          = *packet->data;
      sl_si91x_http_client_put_pkt_rsp_t *response   = (sl_si91x_http_client_put_pkt_rsp_t *)&packet->data;
      sl_si91x_http_put_pkt_server_rsp_t *server_rsp = (sl_si91x_http_put_pkt_server_rsp_t *)&packet->data;

      // Delete HTTP PUT client if PUT request fails
      if (status != SL_STATUS_OK) {
        // sl_si91x_http_client_put_delete() is always success
        sl_si91x_http_client_put_delete();
        break;
      }

      // Check for HTTP_CLIENT_PUT_PKT command
      if (http_cmd_type == SI91X_HTTP_CLIENT_PUT_PKT) {
        http_response.data_length = 0;
        http_response.end_of_data = response->end_of_file;
      }
      // Check for HTTP Client PUT response from server
      else if (http_cmd_type == SI91X_HTTP_CLIENT_PUT_OFFSET_PKT) {
        http_response.data_length = server_rsp->data_len;
        http_response.end_of_data = server_rsp->more;
      }

      http_response.data_buffer = &packet->data[SI91X_HTTP_PUT_OFFSET];

      // 917 does not support HTTP Server response for SL_HTTP_PUT request
      http_response.http_response_code = 0;

      // Delete HTTP PUT client if end of data is 1
      if (http_response.end_of_data) {
        // sl_si91x_http_client_put_delete() is always success
        sl_si91x_http_client_put_delete();
      }

      break;
    }
  }

  return entry->callback_function(&entry->client_handle, event, &http_response, sdk_context);
}
