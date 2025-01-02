/***************************************************************************/ /**
 * @file
 * @brief
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
#include "sl_status.h"
#include "sl_utility.h"
#include "sl_net.h"
#include "sl_wifi.h"
#include "sl_net_wifi_types.h"
#include "sl_net_si91x.h"
#include "sl_si91x_host_interface.h"
#include "sl_si91x_driver.h"
#include "sl_rsi_utility.h"
#include "sl_net_rsi_utility.h"
#include "sl_si91x_core_utilities.h"
#include <stdbool.h>
#include <string.h>

// Define a bit mask for DHCP unicast offer
#define SL_SI91X_DHCP_UNICAST_OFFER ((uint32_t)1U << 3)

// Global variable indicating if the device is initialized
extern bool device_initialized;

sl_status_t sli_si91x_configure_ip_address(sl_net_ip_configuration_t *ip_config,
                                           uint8_t virtual_ap_id,
                                           const uint32_t timeout)
{
  sl_status_t status                      = SL_STATUS_INVALID_PARAMETER;
  sl_si91x_req_ipv4_params_t ip_req       = { 0 };
  sl_si91x_req_ipv6_params_t ipv6_request = { 0 };
  sl_si91x_packet_t *packet;
  sl_wifi_buffer_t *buffer = NULL;
  uint32_t wait_time       = (timeout ? SL_SI91X_WAIT_FOR_RESPONSE(timeout) : SL_SI91X_RETURN_IMMEDIATELY);

  // Check if the device is initialized
  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  // Check for NULL ip_config pointer
  if (NULL == ip_config) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Check if timeout is 0
  if (0 == timeout) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (SL_IPV4 & ip_config->type) {
    // Initialize the IPv4 request structure and fill fields accordingly
    memset(&ip_req, 0, sizeof(ip_req));
    ip_req.vap_id = virtual_ap_id;

    if (SL_IP_MANAGEMENT_STATIC_IP == ip_config->mode) {
      ip_req.dhcp_mode = SL_SI91X_STATIC;
      // Fill IP address
      memcpy(ip_req.ipaddress, ip_config->ip.v4.ip_address.bytes, 4);

      // Fill network mask
      memcpy(ip_req.netmask, ip_config->ip.v4.netmask.bytes, 4);

      // Fill gateway
      memcpy(ip_req.gateway, ip_config->ip.v4.gateway.bytes, 4);
    } else {
      ip_req.dhcp_mode = (SL_SI91X_DHCP | SL_SI91X_DHCP_UNICAST_OFFER);
    }

    if (NULL != ip_config->host_name) {
      // Enable DHCP hostname option and copy the hostname
      ip_req.dhcp_mode |= SL_SI91X_DHCP_HOSTNAME;
      memcpy(ip_req.hostname, ip_config->host_name, sizeof(ip_req.hostname));
    }

    ip_req.dhcp_discover_rtr_interval_min = ip_config->dhcp_config.min_discover_retry_interval;
    ip_req.dhcp_request_rtr_interval_min  = ip_config->dhcp_config.min_request_retry_interval;
    ip_req.dhcp_discover_rtr_interval_max = ip_config->dhcp_config.max_discover_retry_interval;
    ip_req.dhcp_request_rtr_interval_max  = ip_config->dhcp_config.max_request_retry_interval;
    ip_req.dhcp_discover_max_retries      = ip_config->dhcp_config.min_discover_retries;
    ip_req.dhcp_request_max_retries       = ip_config->dhcp_config.max_request_retries;

    status = sl_si91x_driver_send_command(RSI_WLAN_REQ_IPCONFV4,
                                          SI91X_NETWORK_CMD,
                                          &ip_req,
                                          sizeof(sl_si91x_req_ipv4_params_t),
                                          wait_time,
                                          NULL,
                                          &buffer);

    // Check if the command failed and free the buffer if it was allocated
    if ((status != SL_STATUS_OK) && (buffer != NULL)) {
      sl_si91x_host_free_buffer(buffer);
    }

    // Verify the status and return it
    VERIFY_STATUS_AND_RETURN(status);
    packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);

    if (SL_IP_MANAGEMENT_DHCP == ip_config->mode) {
      // Extract DHCP response data if in DHCP mode
      const sl_si91x_rsp_ipv4_params_t *response_data = (sl_si91x_rsp_ipv4_params_t *)packet->data;
      memcpy(ip_config->ip.v4.ip_address.bytes, (const uint8_t *)response_data->ipaddr, sizeof(sl_ipv4_address_t));
      memcpy(ip_config->ip.v4.netmask.bytes, (const uint8_t *)response_data->netmask, sizeof(sl_ipv4_address_t));
      memcpy(ip_config->ip.v4.gateway.bytes, (const uint8_t *)response_data->gateway, sizeof(sl_ipv4_address_t));
    }

    // Free the buffer and return success status
    sl_si91x_host_free_buffer(buffer);
  }

  if (SL_IPV6 & ip_config->type) {
    // Initialize the IPv6 request structure
    memset(&ipv6_request, 0, sizeof(ipv6_request));
    uint16_t prefix_length = 64;
    memcpy(&ipv6_request.prefixLength, &prefix_length, 2);
    ipv6_request.vap_id = virtual_ap_id;

    if (SL_IP_MANAGEMENT_STATIC_IP == ip_config->mode) {
      // Set IPv6 mode to static
      memcpy(&ipv6_request.ipaddr6, ip_config->ip.v6.global_address.bytes, SL_IPV6_ADDRESS_LENGTH);
      memcpy(&ipv6_request.gateway6, ip_config->ip.v6.gateway.bytes, SL_IPV6_ADDRESS_LENGTH);
      ipv6_request.mode[0] = SL_SI91X_STATIC;
    } else {
      // Set IPv6 mode to dynamic
      ipv6_request.mode[0] = (SL_SI91X_DHCP | SL_SI91X_DHCP_UNICAST_OFFER);
    }

    // Send the IPv6 configuration request to SI91X driver
    status = sl_si91x_driver_send_command(RSI_WLAN_REQ_IPCONFV6,
                                          SI91X_NETWORK_CMD,
                                          &ipv6_request,
                                          sizeof(sl_si91x_req_ipv6_params_t),
                                          wait_time,
                                          NULL,
                                          &buffer);

    // Check if the command failed and free the buffer if it was allocated
    if ((status != SL_STATUS_OK) && (buffer != NULL)) {
      sl_si91x_host_free_buffer(buffer);
    }

    VERIFY_STATUS_AND_RETURN(status);

    // Extract the IPv6 configuration response data
    packet                                          = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
    const sl_si91x_rsp_ipv6_params_t *ipv6_response = (sl_si91x_rsp_ipv6_params_t *)packet->data;

    // Copy the IPv6 addresses to the address structure
    memcpy(&ip_config->ip.v6.link_local_address,
           (const uint8_t *)ipv6_response->link_local_address,
           sizeof(ipv6_response->link_local_address));
    memcpy(&ip_config->ip.v6.global_address,
           (const uint8_t *)ipv6_response->global_address,
           sizeof(ipv6_response->global_address));
    memcpy(&ip_config->ip.v6.gateway,
           (const uint8_t *)ipv6_response->gateway_address,
           sizeof(ipv6_response->gateway_address));

    // Free the buffer and return success status
    sl_si91x_host_free_buffer(buffer);
  }

  return status;
}

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

sl_status_t sl_si91x_configure_ip_address(sl_net_ip_configuration_t *address, uint8_t virtual_ap_id)
{
  return sli_si91x_configure_ip_address(address, virtual_ap_id, 150000);
}