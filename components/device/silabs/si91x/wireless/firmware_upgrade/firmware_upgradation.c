/***************************************************************************/ /**
 * @file firmware_upgradation.c
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_status.h"
#include "sl_si91x_constants.h"
#include "sl_si91x_protocol_types.h"
#include "sl_si91x_host_interface.h"
#include "sl_si91x_driver.h"
#include <string.h>
#include "firmware_upgradation.h"
#ifdef SLI_SI91X_OFFLOAD_NETWORK_STACK
#include "sl_si91x_socket_utility.h"
#endif
#include <sl_string.h>

/******************************************************
 *                      Macros
 ******************************************************/

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
  sl_status_t status        = SL_STATUS_FAIL;
  sli_si91x_req_fwup_t fwup = { 0 };

  // Check if length exceeds
  if (length > SLI_MAX_FWUP_CHUNK_SIZE) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Fill packet type
  memcpy(&fwup.type, &type, sizeof(fwup.type));
  // Fill packet length
  memcpy(&fwup.length, &length, sizeof(fwup.length));
  // Fill packet content
  memcpy(fwup.content, content, length);

  // Send FW update command
  status = sli_si91x_driver_send_command(SLI_WLAN_REQ_FWUP,
                                         SLI_SI91X_WLAN_CMD,
                                         &fwup,
                                         sizeof(sli_si91x_req_fwup_t),
                                         SL_SI91X_WAIT_FOR_RESPONSE(5000),
                                         NULL,
                                         NULL);

  // Return status if error in sending command occurs
  return status;
}

sl_status_t sl_si91x_fwup_start(const uint8_t *rps_header)
{
  sl_status_t status = sl_si91x_fwup(SL_FWUP_RPS_HEADER, rps_header, SLI_RPS_HEADER_SIZE);
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
  if (flags & SLI_SI91X_FW_START_OF_FILE) {
    boot_cmd = SLI_HOST_INTERACT_REG_VALID | SLI_PONG_VALID;
  }

  //! check for invalid packet
  if ((fw_image_size % SLI_SI91X_MIN_CHUNK_SIZE != 0) && (!(flags & SLI_SI91X_FW_END_OF_FILE))) {
    return SL_STATUS_FAIL;
  }

  //! loop to execute multiple of 4K chunks
  while (offset < fw_image_size) {
    switch (boot_cmd) {
      case (SLI_HOST_INTERACT_REG_VALID | SLI_PING_VALID):
        boot_insn = SLI_PONG_WRITE;
        poll_resp = SLI_PING_AVAIL;
        boot_cmd  = SLI_HOST_INTERACT_REG_VALID | SLI_PONG_VALID;
        break;

      case (SLI_HOST_INTERACT_REG_VALID | SLI_PONG_VALID):
        boot_insn = SLI_PING_WRITE;
        poll_resp = SLI_PONG_AVAIL;
        boot_cmd  = SLI_HOST_INTERACT_REG_VALID | SLI_PING_VALID;
        break;

      default:
        return SL_STATUS_FAIL;
    }

    retval = sli_si91x_boot_instruction((uint8_t)boot_insn, (uint16_t *)(firmware_image + offset));
    VERIFY_STATUS_AND_RETURN(retval);

    while (1) {
      retval = sli_si91x_boot_instruction(SLI_REG_READ, &read_value);
      VERIFY_STATUS_AND_RETURN(retval);

      if (read_value == (SLI_HOST_INTERACT_REG_VALID | poll_resp)) {
        break;
      }
    }
    offset += SLI_SI91X_MIN_CHUNK_SIZE;
  }

  //! For last chunk set boot cmd as End of file reached
  if (flags & SLI_SI91X_FW_END_OF_FILE) {
    boot_cmd = SLI_HOST_INTERACT_REG_VALID | SLI_EOF_REACHED;

    retval = sli_si91x_boot_instruction(SLI_REG_WRITE, &boot_cmd);
    VERIFY_STATUS_AND_RETURN(retval);

    //! check for successful firmware upgrade
    do {
      retval = sli_si91x_boot_instruction(SLI_REG_READ, &read_value);
      VERIFY_STATUS_AND_RETURN(retval);

    } while (read_value != (SLI_HOST_INTERACT_REG_VALID | SLI_FWUP_SUCCESSFUL));
  }
  return retval;
}

sl_status_t sl_si91x_set_fast_fw_up(void)
{
  uint32_t read_data = 0;
  sl_status_t retval = 0;
  retval             = sl_si91x_bus_read_memory(SLI_SI91X_SAFE_UPGRADE_ADDR, 4, (uint8_t *)&read_data);
  VERIFY_STATUS_AND_RETURN(retval);

  //disabling safe upgradation bit
  if (read_data & SLI_SI91X_SAFE_UPGRADE) {
    read_data &= ~(SLI_SI91X_SAFE_UPGRADE);
    retval = sl_si91x_bus_write_memory(SLI_SI91X_SAFE_UPGRADE_ADDR, 4, (uint8_t *)&read_data);
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
  sl_wifi_buffer_t *buffer            = NULL;
  sl_status_t status                  = SL_STATUS_FAIL;
  sli_si91x_wait_period_t wait_period = SLI_SI91X_RETURN_IMMEDIATELY;

  // Initialize the OTA firmware update request structure
  sli_si91x_ota_firmware_update_request_t otaf_fwup = { 0 };

  // Determine the wait period based on the 'asynchronous' flag
  if (asynchronous == false) {
    wait_period = SLI_SI91X_WAIT_FOR_OTAF_RESPONSE;
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

  status = sli_si91x_driver_send_command(SLI_WLAN_REQ_OTA_FWUP,
                                         SLI_SI91X_NETWORK_CMD,
                                         &otaf_fwup,
                                         sizeof(sli_si91x_ota_firmware_update_request_t),
                                         wait_period,
                                         NULL,
                                         &buffer);

  // Check if the command was synchronous and free the buffer if it was allocated
  if (asynchronous == false) {
    if (status != SL_STATUS_OK && buffer != NULL) {
      sli_si91x_host_free_buffer(buffer);
    }
    VERIFY_STATUS_AND_RETURN(status);
  }
  sli_si91x_host_free_buffer(buffer);
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
  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  UNUSED_PARAMETER(type);
  UNUSED_PARAMETER(post_data);
  UNUSED_PARAMETER(post_data_length);

  sl_status_t status                             = SL_STATUS_FAIL;
  uint32_t send_size                             = 0;
  uint16_t http_length                           = 0;
  uint16_t length                                = 0;
  uint16_t https_enable                          = 0;
  uint8_t packet_identifier                      = 0;
  sli_si91x_http_client_request_t *packet_buffer = NULL;
  uint16_t offset                                = 0;
  uint16_t rem_length                            = 0;
  uint16_t chunk_size                            = 0;
  sli_si91x_http_client_request_t *http_client   = malloc(sizeof(sli_si91x_http_client_request_t));
  SL_VERIFY_POINTER_OR_RETURN(http_client, SL_STATUS_ALLOCATION_FAILED);

  memset(http_client, 0, sizeof(sli_si91x_http_client_request_t));
  http_client->ip_version = (flags & IP_VERSION_6) ? SL_IPV6_VERSION : SL_IPV4_VERSION;

  // Set https feature
  https_enable = (flags & SL_SI91X_ENABLE_TLS) ? SL_SI91X_ENABLE_TLS : https_enable;

  // Set default by NULL delimiter
  https_enable |= SL_SI91X_ENABLE_NULL_DELIMETER;

  //ssl versions
  https_enable = (flags & SL_SI91X_TLS_V_1_0) ? (https_enable | SL_SI91X_TLS_V_1_0) : https_enable;

  https_enable = (flags & SL_SI91X_TLS_V_1_1) ? (https_enable | SL_SI91X_TLS_V_1_1) : https_enable;

  https_enable = (flags & SL_SI91X_TLS_V_1_2) ? (https_enable | SL_SI91X_TLS_V_1_2) : https_enable;

  https_enable = (flags & SL_SI91X_TLS_V_1_3) ? (https_enable | SL_SI91X_TLS_V_1_3) : https_enable;

  // Set HTTP version 1.1 feature bit
  https_enable = (flags & SL_SI91X_HTTP_V_1_1) ? (https_enable | SL_SI91X_HTTP_V_1_1) : https_enable;

  https_enable = (flags & SL_SI91X_HTTPS_CERTIFICATE_INDEX_1) ? (https_enable | SL_SI91X_HTTPS_CERTIFICATE_INDEX_1)
                                                              : https_enable;

  https_enable = (flags & SL_SI91X_HTTPS_CERTIFICATE_INDEX_2) ? (https_enable | SL_SI91X_HTTPS_CERTIFICATE_INDEX_2)
                                                              : https_enable;

  if (flags & SL_SI91X_HTTPS_USE_SNI) {
    if (host_name == NULL) {
      free(http_client);
      return SL_STATUS_NULL_POINTER;
    }

    https_enable = (https_enable | SL_SI91X_HTTPS_USE_SNI);

    sl_si91x_socket_type_length_value_t *set_sni = (sl_si91x_socket_type_length_value_t *)malloc(
      sizeof(sl_si91x_socket_type_length_value_t) + sl_strlen((char *)host_name));
    if (set_sni == NULL) {
      free(http_client);
      return SL_STATUS_ALLOCATION_FAILED;
    }

    // Initialize the SNI structure.
    set_sni->type   = SL_SI91X_TLS_EXTENSION_SNI_TYPE;
    set_sni->length = sl_strlen((char *)host_name);
    memcpy(set_sni->value, host_name, set_sni->length);
    status = sli_si91x_set_sni_for_embedded_socket(set_sni);
    if (status != SL_STATUS_OK) {
      free(http_client);
      free(set_sni);
      return status;
    }
    free(set_sni);
  }

  // Fill https features parameters
  http_client->https_enable = https_enable;

  // Fill port no
  http_client->port_number = port;
  memset(http_client->buffer, 0, sizeof(http_client->buffer));

  // Fill username
  if (sl_strlen((char *)user_name) < sizeof(http_client->buffer)) {
    length = (uint16_t)sl_strlen((char *)user_name);
    memcpy(http_client->buffer, user_name, length);
    http_length += length + 1;
  } else {
    free(http_client);
    return status;
  }

  // Fill password
  if (sl_strlen((char *)password) < (sizeof(http_client->buffer) - 1 - http_length)) {
    length = (uint16_t)sl_strlen((char *)password);
    memcpy(((http_client->buffer) + http_length), password, length);
    http_length += length + 1;
  } else {
    free(http_client);
    return status;
  }

  // Check for HTTP_V_1.1 and Empty host name
  host_name = ((flags & SL_SI91X_HTTP_V_1_1) && (sl_strlen((char *)host_name) == 0)) ? ip_address : host_name;

  // Copy  Host name
  if (sl_strlen((char *)host_name) < (sizeof(http_client->buffer) - 1 - http_length)) {
    length = (uint16_t)sl_strlen((char *)host_name);
    memcpy(((http_client->buffer) + http_length), host_name, length);
    http_length += length + 1;
  } else {
    free(http_client);
    return status;
  }

  // Copy IP address
  if (sl_strlen((char *)ip_address) < (sizeof(http_client->buffer) - 1 - http_length)) {
    length = (uint16_t)sl_strlen((char *)ip_address);
    memcpy(((http_client->buffer) + http_length), ip_address, length);
    http_length += length + 1;
  } else {
    free(http_client);
    return status;
  }

  // Copy URL resource
  if (sl_strlen((char *)resource) < (sizeof(http_client->buffer) - 1 - http_length)) {
    length = (uint16_t)sl_strlen((char *)resource);
    memcpy(((http_client->buffer) + http_length), resource, length);
    http_length += length + 1;
  } else {
    free(http_client);
    return status;
  }

  // Copy Extended header
  if (extended_header != NULL) {
    if (sl_strlen((char *)extended_header) < (sizeof(http_client->buffer) - 1 - http_length)) {
      length = (uint16_t)sl_strlen((char *)extended_header);
      memcpy(((http_client->buffer) + http_length), extended_header, length);
      http_length += length;
    } else {
      free(http_client);
      return status;
    }
  }

  // Check if request buffer is overflowed or resource length is overflowed
  if (http_length > SLI_SI91X_HTTP_BUFFER_LEN
      || sl_strnlen(((char *)resource), SLI_SI91X_MAX_HTTP_URL_SIZE + 1) > SLI_SI91X_MAX_HTTP_URL_SIZE)
    return SL_STATUS_HAS_OVERFLOWED;

  send_size = sizeof(sli_si91x_http_client_request_t) - SLI_SI91X_HTTP_BUFFER_LEN + http_length;
  send_size &= 0xFFF;

  rem_length = http_length;
  if (http_length <= SLI_SI91X_MAX_HTTP_CHUNK_SIZE) {
    status = sli_si91x_driver_send_command(SLI_WLAN_REQ_HTTP_OTAF,
                                           SLI_SI91X_WLAN_CMD,
                                           http_client,
                                           send_size,
                                           SLI_SI91X_WAIT_FOR_OTAF_RESPONSE,
                                           NULL,
                                           NULL);
  } else {
    packet_buffer = malloc(sizeof(sli_si91x_http_client_request_t));
    SLI_VERIFY_MALLOC_AND_RETURN(packet_buffer);

    while (rem_length) {
      if (rem_length > SLI_SI91X_MAX_HTTP_CHUNK_SIZE) {
        packet_identifier = (offset == 0) ? SLI_HTTP_GET_FIRST_PKT : SLI_HTTP_GET_MIDDLE_PKT;
        chunk_size        = SLI_SI91X_MAX_HTTP_CHUNK_SIZE;
      } else {
        packet_identifier = SLI_HTTP_GET_LAST_PKT;
        chunk_size        = rem_length;
      }

      packet_buffer->ip_version   = http_client->ip_version;
      packet_buffer->https_enable = http_client->https_enable;
      packet_buffer->port_number  = http_client->port_number;

      memcpy(packet_buffer->buffer, (http_client->buffer + offset), chunk_size);

      status = sl_si91x_custom_driver_send_command(
        SLI_WLAN_REQ_HTTP_OTAF,
        SLI_SI91X_WLAN_CMD,
        packet_buffer,
        (sizeof(sli_si91x_http_client_request_t) - SLI_SI91X_HTTP_BUFFER_LEN + chunk_size),
        (rem_length == chunk_size) ? SLI_SI91X_WAIT_FOR_OTAF_RESPONSE : SLI_SI91X_WAIT_FOR_COMMAND_RESPONSE,
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

  free(http_client);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_si91x_http_otaf_v2(const sl_si91x_http_otaf_params_t *http_otaf_params)
{
  if (http_otaf_params == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_si91x_http_otaf(0,
                            http_otaf_params->flags,
                            http_otaf_params->ip_address,
                            http_otaf_params->port,
                            http_otaf_params->resource,
                            http_otaf_params->host_name,
                            http_otaf_params->extended_header,
                            http_otaf_params->user_name,
                            http_otaf_params->password,
                            NULL,
                            0);
}
#endif /* SLI_SI91X_OFFLOAD_NETWORK_STACK */
