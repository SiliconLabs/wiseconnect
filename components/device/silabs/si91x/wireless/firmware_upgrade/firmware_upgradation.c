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

#ifndef SLI_SI91X_MCU_INTERFACE /* Only for NCP mode */
sl_status_t sl_si91x_bl_upgrade_firmware(uint8_t *firmware_image, uint32_t fw_image_size, uint8_t flags)
{
  static uint16_t boot_cmd = 0;
  uint16_t read_value      = 0;
  uint32_t offset          = 0;
  uint32_t retval          = 0;
  uint32_t boot_insn       = 0;
  uint32_t poll_resp       = 0;

  //! If it is a start of file set the boot cmd to pong valid
  if (flags & SL_SI91X_FW_START_OF_FILE) {
    boot_cmd = RSI_HOST_INTERACT_REG_VALID | RSI_PONG_VALID;
  }

  //! check for invalid packet
  if ((fw_image_size % SL_SI91X_MIN_CHUNK_SIZE != 0) && (!(flags & SL_SI91X_FW_END_OF_FILE))) {
    return SL_STATUS_FAIL;
  }

  //! loop to execute multiple of 4K chunks
  while (offset < fw_image_size) {
    switch (boot_cmd) {
      case (RSI_HOST_INTERACT_REG_VALID | RSI_PING_VALID):
        boot_insn = RSI_PONG_WRITE;
        poll_resp = RSI_PING_AVAIL;
        boot_cmd  = RSI_HOST_INTERACT_REG_VALID | RSI_PONG_VALID;
        break;

      case (RSI_HOST_INTERACT_REG_VALID | RSI_PONG_VALID):
        boot_insn = RSI_PING_WRITE;
        poll_resp = RSI_PONG_AVAIL;
        boot_cmd  = RSI_HOST_INTERACT_REG_VALID | RSI_PING_VALID;
        break;

      default:
        return SL_STATUS_FAIL;
    }

    retval = sl_si91x_boot_instruction((uint8_t)boot_insn, (uint16_t *)(firmware_image + offset));
    VERIFY_STATUS_AND_RETURN(retval);

    while (1) {
      retval = sl_si91x_boot_instruction(RSI_REG_READ, &read_value);
      VERIFY_STATUS_AND_RETURN(retval);

      if (read_value == (RSI_HOST_INTERACT_REG_VALID | poll_resp)) {
        break;
      }
    }
    offset += SL_SI91X_MIN_CHUNK_SIZE;
  }

  //! For last chunk set boot cmd as End of file reached
  if (flags & SL_SI91X_FW_END_OF_FILE) {
    boot_cmd = RSI_HOST_INTERACT_REG_VALID | RSI_EOF_REACHED;

    retval = sl_si91x_boot_instruction(RSI_REG_WRITE, &boot_cmd);
    VERIFY_STATUS_AND_RETURN(retval);

    //! check for successful firmware upgrade
    do {
      retval = sl_si91x_boot_instruction(RSI_REG_READ, &read_value);
      VERIFY_STATUS_AND_RETURN(retval);

    } while (read_value != (RSI_HOST_INTERACT_REG_VALID | RSI_FWUP_SUCCESSFUL));
  }
  return retval;
}

sl_status_t sl_si91x_set_fast_fw_up(void)
{
  uint32_t read_data = 0;
  sl_status_t retval = 0;
  retval             = sl_si91x_bus_read_memory(SL_SI91X_SAFE_UPGRADE_ADDR, 4, (uint8_t *)&read_data);
  VERIFY_STATUS_AND_RETURN(retval);

  //disabling safe upgradation bit
  if (read_data & SL_SI91X_SAFE_UPGRADE) {
    read_data &= ~(SL_SI91X_SAFE_UPGRADE);
    retval = sl_si91x_bus_write_memory(SL_SI91X_SAFE_UPGRADE_ADDR, 4, (uint8_t *)&read_data);
    VERIFY_STATUS_AND_RETURN(retval);
  }
  return retval;
}
#endif // !SLI_SI91X_MCU_INTERFACE

#ifdef SLI_SI91X_OFFLOAD_NETWORK_STACK
sl_status_t sl_si91x_ota_firmware_upgradation(sl_ip_address_t server_ip,
                                              uint16_t server_port,
                                              uint16_t chunk_number,
                                              uint16_t timeout,
                                              uint16_t tcp_retry_count,
                                              bool asynchronous)
{
  sl_wifi_buffer_t *buffer           = NULL;
  sl_status_t status                 = SL_STATUS_FAIL;
  sl_si91x_wait_period_t wait_period = SL_SI91X_RETURN_IMMEDIATELY;

  // Initialize the OTA firmware update request structure
  sl_si91x_ota_firmware_update_request_t otaf_fwup = { 0 };

  // Determine the wait period based on the 'asynchronous' flag
  if (asynchronous == false) {
    wait_period = SL_SI91X_WAIT_FOR_OTAF_RESPONSE;
  }

  // Check IP version
  if (server_ip.type == SL_IPV4) {
    // Fill the IP version
    otaf_fwup.ip_version = SL_IPV4;
    memcpy(otaf_fwup.server_ip_address.ipv4_address, server_ip.ip.v4.bytes, SL_IPV4_ADDRESS_LENGTH);
  } else if (server_ip.type == SL_IPV6) {
    otaf_fwup.ip_version = SL_IPV6;
    memcpy(otaf_fwup.server_ip_address.ipv6_address, server_ip.ip.v6.bytes, SL_IPV6_ADDRESS_LENGTH);
  } else {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Fill server port number
  memcpy(otaf_fwup.server_port, &server_port, sizeof(server_port));

  // Fill chunk number
  memcpy(otaf_fwup.chunk_number, &chunk_number, sizeof(otaf_fwup.chunk_number));

  // Fill timeout
  memcpy(otaf_fwup.timeout, &timeout, sizeof(otaf_fwup.timeout));

  // Fill TCP retry count
  memcpy(otaf_fwup.retry_count, &tcp_retry_count, sizeof(otaf_fwup.retry_count));

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_OTA_FWUP,
                                        SI91X_NETWORK_CMD,
                                        &otaf_fwup,
                                        sizeof(sl_si91x_ota_firmware_update_request_t),
                                        wait_period,
                                        NULL,
                                        &buffer);

  // Check if the command was synchronous and free the buffer if it was allocated
  if (asynchronous == false) {
    if (status != SL_STATUS_OK && buffer != NULL) {
      sl_si91x_host_free_buffer(buffer);
    }
    VERIFY_STATUS_AND_RETURN(status);
  }
  sl_si91x_host_free_buffer(buffer);
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
  if (sl_strlen((char *)user_name) < sizeof(http_client.buffer)) {
    length = (uint16_t)sl_strlen((char *)user_name);
    memcpy(http_client.buffer, user_name, length);
    http_length += length + 1;
  } else {
    return status;
  }

  // Fill password
  if (sl_strlen((char *)password) < (sizeof(http_client.buffer) - 1 - http_length)) {
    length = (uint16_t)sl_strlen((char *)password);
    memcpy(((http_client.buffer) + http_length), password, length);
    http_length += length + 1;
  } else {
    return status;
  }

  // Check for HTTP_V_1.1 and Empty host name
  host_name = ((flags & SL_SI91X_HTTP_V_1_1) && (sl_strlen((char *)host_name) == 0)) ? ip_address : host_name;

  // Copy  Host name
  if (sl_strlen((char *)host_name) < (sizeof(http_client.buffer) - 1 - http_length)) {
    length = (uint16_t)sl_strlen((char *)host_name);
    memcpy(((http_client.buffer) + http_length), host_name, length);
    http_length += length + 1;
  } else {
    return status;
  }

  // Copy IP address
  if (sl_strlen((char *)ip_address) < (sizeof(http_client.buffer) - 1 - http_length)) {
    length = (uint16_t)sl_strlen((char *)ip_address);
    memcpy(((http_client.buffer) + http_length), ip_address, length);
    http_length += length + 1;
  } else {
    return status;
  }

  // Copy URL resource
  if (sl_strlen((char *)resource) < (sizeof(http_client.buffer) - 1 - http_length)) {
    length = (uint16_t)sl_strlen((char *)resource);
    memcpy(((http_client.buffer) + http_length), resource, length);
    http_length += length + 1;
  } else {
    return status;
  }

  // Copy Extended header
  if (extended_header != NULL) {
    if (sl_strlen((char *)extended_header) < (sizeof(http_client.buffer) - 1 - http_length)) {
      length = (uint16_t)sl_strlen((char *)extended_header);
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
#endif /* SLI_SI91X_OFFLOAD_NETWORK_STACK */
