/*******************************************************************************
* @file  firmware_upgradation.c
* @brief
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_status.h"
#include "sl_si91x_constants.h"
#include "sl_si91x_protocol_types.h"
#include "sl_si91x_host_interface.h"
#include "sl_si91x_driver.h"
#include <string.h>
#include "firmware_upgradation.h"

#define SI91X_HTTP_BUFFER_LEN 1200

extern bool device_initialized;

sl_status_t sl_si91x_fwup(uint16_t type, uint8_t *content, uint16_t length)
{
  sl_status_t status       = SL_STATUS_FAIL;
  sl_si91x_req_fwup_t fwup = { 0 };

  // Check if length exceeds
  if (length > SL_MAX_FWUP_CHUNK_SIZE) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Fill packet type
  memcpy(&fwup.type, &type, sizeof(fwup.type));
  // Fill packet length
  memcpy(&fwup.length, &length, sizeof(fwup.length));
  // Fill packet content
  memcpy(fwup.content, content, length);

  // Send FW update command
  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_FWUP,
                                        SI91X_WLAN_CMD_QUEUE,
                                        &fwup,
                                        sizeof(sl_si91x_req_fwup_t),
                                        SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                        NULL,
                                        NULL);

  // Return status if error in sending command occurs
  return status;
}

sl_status_t sl_si91x_fwup_start(uint8_t *rps_header)
{
  sl_status_t status = sl_si91x_fwup(SL_FWUP_RPS_HEADER, rps_header, SL_RPS_HEADER_SIZE);
  return status;
}

sl_status_t sl_si91x_fwup_load(uint8_t *content, uint16_t length)
{
  sl_status_t status = sl_si91x_fwup(SL_FWUP_RPS_CONTENT, content, length);
  return status;
}

sl_status_t sl_si91x_http_otaf(uint8_t type,
                               uint8_t flags,
                               uint8_t *ip_address,
                               uint16_t port,
                               uint8_t *resource,
                               uint8_t *host_name,
                               uint8_t *extended_header,
                               uint8_t *user_name,
                               uint8_t *password,
                               uint8_t *post_data,
                               uint32_t post_data_length)
{
  sl_status_t status                         = SL_STATUS_FAIL;
  sl_si91x_http_client_request_t http_client = { 0 };
  uint32_t send_size                         = 0;
  uint16_t http_length                       = 0;
  uint8_t https_enable                       = 0;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (flags & SL_IPV6) {
    http_client.ip_version = SL_IPV6_VERSION;
  } else {
    http_client.ip_version = SL_IPV4_VERSION;
  }

  // Set https feature
  if (flags & SL_SI91X_ENABLE_TLS) {
    https_enable = SL_SI91X_ENABLE_TLS;
  }

  // Set default by NULL delimiter
  https_enable |= SL_SI91X_ENABLE_NULL_DELIMETER;

  //ssl versions
  if (flags & SL_SI91X_TLS_V_1_0) {
    https_enable |= SL_SI91X_TLS_V_1_0;
  }

  if (flags & SL_SI91X_TLS_V_1_1) {
    https_enable |= SL_SI91X_TLS_V_1_1;
  }

  // Set HTTP version 1.1 feature bit
  if (flags & SL_SI91X_HTTP_V_1_1) {
    https_enable |= SL_SI91X_HTTP_V_1_1;
  }
  // Fill https features parameters
  http_client.https_enable = https_enable;

  // Fill port no
  http_client.port_number = port;

  // Fill username
  strcpy((char *)http_client.buffer, (char *)user_name);
  http_length += strlen((char *)user_name) + 1;

  // Fill password
  strcpy((char *)((http_client.buffer) + http_length), (char *)password);
  http_length += strlen((char *)password) + 1;

  // Check for HTTP_V_1.1 and Empty host name
  if ((flags & SL_SI91X_HTTP_V_1_1) && (strlen((char *)host_name) == 0)) {
    host_name = ip_address;
  }

  // Copy  Host name
  strcpy((char *)((http_client.buffer) + http_length), (char *)host_name);
  http_length += strlen((char *)host_name) + 1;

  // Copy IP address
  strcpy((char *)((http_client.buffer) + http_length), (char *)ip_address);
  http_length += strlen((char *)ip_address) + 1;

  // Copy URL resource
  strcpy((char *)((http_client.buffer) + http_length), (char *)resource);
  http_length += strlen((char *)resource) + 1;

  // Copy Extended header
  if (extended_header != NULL) {
    strcpy((char *)((http_client.buffer) + http_length), (char *)extended_header);
    http_length += strlen((char *)extended_header);
  }
  // Copy Httppost data
  if (type) {
    memcpy((http_client.buffer) + http_length + 1, post_data, post_data_length);
    http_length += (post_data_length + 1);
  }

  send_size = sizeof(sl_si91x_http_client_request_t) - SI91X_HTTP_BUFFER_LEN + http_length;
  send_size &= 0xFFF;

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_HTTP_OTAF,
                                        SI91X_WLAN_CMD_QUEUE,
                                        &http_client,
                                        send_size,
                                        SL_SI91X_WAIT_FOR_RESPONSE(600000),
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}
