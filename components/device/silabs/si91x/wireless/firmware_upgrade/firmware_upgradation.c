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
#include <sl_string.h>

/******************************************************
 *                      Macros
 ******************************************************/
// Macro to check if malloc failed
#define VERIFY_MALLOC_AND_RETURN(ptr)     \
  do {                                    \
    if (ptr == NULL) {                    \
      return SL_STATUS_ALLOCATION_FAILED; \
    }                                     \
  } while (0)

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define IP_VERSION_6 BIT(1)

/******************************************************
 *                 Global Variables
 ******************************************************/
extern bool device_initialized;

/***************************************************************************/ /**
 * @brief  
 *   Helper function for actual APIs
 * @param[in] type   
 *   Firmware upgrade chunk type
 * @param[in] content 
 *   Firmware content
 * @param[in] length  
 *   Length of the content
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 ******************************************************************************/
static sl_status_t sl_si91x_fwup(uint16_t type, const uint8_t *content, uint16_t length);

static sl_status_t sl_si91x_fwup(uint16_t type, const uint8_t *content, uint16_t length)
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
                                        SI91X_WLAN_CMD,
                                        &fwup,
                                        sizeof(sl_si91x_req_fwup_t),
                                        SL_SI91X_WAIT_FOR_RESPONSE(5000),
                                        NULL,
                                        NULL);

  // Return status if error in sending command occurs
  return status;
}

sl_status_t sl_si91x_fwup_start(const uint8_t *rps_header)
{
  sl_status_t status = sl_si91x_fwup(SL_FWUP_RPS_HEADER, rps_header, SL_RPS_HEADER_SIZE);
  return status;
}

sl_status_t sl_si91x_fwup_load(const uint8_t *content, uint16_t length)
{
  sl_status_t status = sl_si91x_fwup(SL_FWUP_RPS_CONTENT, content, length);
  return status;
}

sl_status_t sl_si91x_fwup_abort()
{
  sl_status_t status = sl_si91x_fwup(SL_FWUP_ABORT, NULL, 0);
  return status;
}

sl_status_t sl_si91x_http_otaf(uint8_t type,
                               uint16_t flags,
                               uint8_t *ip_address,
                               uint16_t port,
                               uint8_t *resource,
                               uint8_t *host_name,
                               uint8_t *extended_header,
                               uint8_t *user_name,
                               uint8_t *password,
                               const uint8_t *post_data,
                               uint32_t post_data_length)
{
  sl_status_t status                            = SL_STATUS_FAIL;
  sl_si91x_http_client_request_t http_client    = { 0 };
  uint32_t send_size                            = 0;
  uint16_t http_length                          = 0;
  uint16_t length                               = 0;
  uint16_t https_enable                         = 0;
  uint8_t packet_identifier                     = 0;
  sl_si91x_http_client_request_t *packet_buffer = NULL;
  uint16_t offset                               = 0;
  uint16_t rem_length                           = 0;
  uint16_t chunk_size                           = 0;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  http_client.ip_version = (flags & IP_VERSION_6) ? SL_IPV6_VERSION : SL_IPV4_VERSION;

  // Set https feature
  https_enable = (flags & SL_SI91X_ENABLE_TLS) ? SL_SI91X_ENABLE_TLS : https_enable;

  // Set default by NULL delimiter
  https_enable |= SL_SI91X_ENABLE_NULL_DELIMETER;

  //ssl versions
  https_enable = (flags & SL_SI91X_TLS_V_1_0) ? (https_enable | SL_SI91X_TLS_V_1_0) : https_enable;

  https_enable = (flags & SL_SI91X_TLS_V_1_1) ? (https_enable | SL_SI91X_TLS_V_1_1) : https_enable;

  https_enable = (flags & SL_SI91X_TLS_V_1_2) ? (https_enable | SL_SI91X_TLS_V_1_2) : https_enable;

  // Set HTTP version 1.1 feature bit
  https_enable = (flags & SL_SI91X_HTTP_V_1_1) ? (https_enable | SL_SI91X_HTTP_V_1_1) : https_enable;

  https_enable = (flags & SL_SI91X_HTTPS_CERTIFICATE_INDEX_1) ? (https_enable | SL_SI91X_HTTPS_CERTIFICATE_INDEX_1)
                                                              : https_enable;

  https_enable = (flags & SL_SI91X_HTTPS_CERTIFICATE_INDEX_2) ? (https_enable | SL_SI91X_HTTPS_CERTIFICATE_INDEX_2)
                                                              : https_enable;

  // Fill https features parameters
  http_client.https_enable = https_enable;

  // Fill port no
  http_client.port_number = port;
  memset(http_client.buffer, 0, sizeof(http_client.buffer));

  // Fill username
  if (strlen((char *)user_name) < sizeof(http_client.buffer)) {
    length = (uint16_t)strlen((char *)user_name);
    memcpy(http_client.buffer, user_name, length);
    http_length += length + 1;
  } else {
    return status;
  }

  // Fill password
  if (strlen((char *)password) < (sizeof(http_client.buffer) - 1 - http_length)) {
    length = (uint16_t)strlen((char *)password);
    memcpy(((http_client.buffer) + http_length), password, length);
    http_length += length + 1;
  } else {
    return status;
  }

  // Check for HTTP_V_1.1 and Empty host name
  host_name = ((flags & SL_SI91X_HTTP_V_1_1) && (strlen((char *)host_name) == 0)) ? ip_address : host_name;

  // Copy  Host name
  if (strlen((char *)host_name) < (sizeof(http_client.buffer) - 1 - http_length)) {
    length = (uint16_t)strlen((char *)host_name);
    memcpy(((http_client.buffer) + http_length), host_name, length);
    http_length += length + 1;
  } else {
    return status;
  }

  // Copy IP address
  if (strlen((char *)ip_address) < (sizeof(http_client.buffer) - 1 - http_length)) {
    length = (uint16_t)strlen((char *)ip_address);
    memcpy(((http_client.buffer) + http_length), ip_address, length);
    http_length += length + 1;
  } else {
    return status;
  }

  // Copy URL resource
  if (strlen((char *)resource) < (sizeof(http_client.buffer) - 1 - http_length)) {
    length = (uint16_t)strlen((char *)resource);
    memcpy(((http_client.buffer) + http_length), resource, length);
    http_length += length + 1;
  } else {
    return status;
  }

  // Copy Extended header
  if (extended_header != NULL) {
    if (strlen((char *)extended_header) < (sizeof(http_client.buffer) - 1 - http_length)) {
      length = (uint16_t)strlen((char *)extended_header);
      memcpy(((http_client.buffer) + http_length), extended_header, length);
      http_length += length;
    } else {
      return status;
    }
  }

  // Copy Httppost data
  if (post_data_length < (sizeof(http_client.buffer) - 1 - http_length)) {
    if (type) {
      memcpy((http_client.buffer) + http_length + 1, post_data, post_data_length);
      http_length += (post_data_length + 1);
    }
  } else {
    return status;
  }

  // Check if request buffer is overflowed or resource length is overflowed
  if (http_length > SI91X_HTTP_BUFFER_LEN
      || sl_strnlen(((char *)resource), SI91X_MAX_HTTP_URL_SIZE + 1) > SI91X_MAX_HTTP_URL_SIZE)
    return SL_STATUS_HAS_OVERFLOWED;

  send_size = sizeof(sl_si91x_http_client_request_t) - SI91X_HTTP_BUFFER_LEN + http_length;
  send_size &= 0xFFF;

  rem_length = http_length;
  if (http_length <= SI91X_MAX_HTTP_CHUNK_SIZE) {
    status = sl_si91x_driver_send_command(RSI_WLAN_REQ_HTTP_OTAF,
                                          SI91X_WLAN_CMD,
                                          &http_client,
                                          send_size,
                                          SL_SI91X_WAIT_FOR_OTAF_RESPONSE,
                                          NULL,
                                          NULL);
  } else {
    packet_buffer = malloc(sizeof(sl_si91x_http_client_request_t));
    VERIFY_MALLOC_AND_RETURN(packet_buffer);

    while (rem_length) {
      if (rem_length > SI91X_MAX_HTTP_CHUNK_SIZE) {
        packet_identifier = (offset == 0) ? HTTP_GET_FIRST_PKT : HTTP_GET_MIDDLE_PKT;
        chunk_size        = SI91X_MAX_HTTP_CHUNK_SIZE;
      } else {
        packet_identifier = HTTP_GET_LAST_PKT;
        chunk_size        = rem_length;
      }

      packet_buffer->ip_version   = http_client.ip_version;
      packet_buffer->https_enable = http_client.https_enable;
      packet_buffer->port_number  = http_client.port_number;

      memcpy(packet_buffer->buffer, (http_client.buffer + offset), chunk_size);

      status = sl_si91x_custom_driver_send_command(
        RSI_WLAN_REQ_HTTP_OTAF,
        SI91X_WLAN_CMD,
        packet_buffer,
        (sizeof(sl_si91x_http_client_request_t) - SI91X_HTTP_BUFFER_LEN + chunk_size),
        (rem_length == chunk_size) ? SL_SI91X_WAIT_FOR_OTAF_RESPONSE : SL_SI91X_WAIT_FOR_COMMAND_RESPONSE,
        NULL,
        NULL,
        packet_identifier);

      if ((status != SL_STATUS_OK) && (status != SL_STATUS_SI91X_HTTP_GET_CMD_IN_PROGRESS))
        break;

      offset += chunk_size;
      rem_length -= chunk_size;
    }

    // Free packet buffer structure memory
    free(packet_buffer);
  }
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}
