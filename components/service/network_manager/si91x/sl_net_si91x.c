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
#include "sl_wifi_types.h"
#include "sl_net.h"
#include "stddef.h"
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
#include "sl_wifi_callback_framework.h"

static sl_status_t sli_si91x_send_multicast_request(sl_wifi_interface_t interface,
                                                    const sl_ip_address_t *ip_address,
                                                    uint8_t command_type);
sl_status_t sl_net_host_get_by_name(const char *host_name,
                                    const uint32_t timeout,
                                    const sl_net_dns_resolution_ip_type_t dns_resolution_ip,
                                    sl_ip_address_t *sl_ip_address);

extern bool device_initialized;

sl_status_t sl_net_wifi_client_init(sl_net_interface_t interface,
                                    const void *configuration,
                                    void *context,
                                    sl_net_event_handler_t event_handler)
{
  UNUSED_PARAMETER(interface);
  UNUSED_PARAMETER(context);
  sl_status_t status = SL_STATUS_FAIL;

  // Set the user-defined event handler for client mode
  sl_si91x_register_event_handler(event_handler);

  status = sl_wifi_init(configuration, NULL, sl_wifi_default_event_handler);
  // Verify the initialization status and return it
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_net_wifi_client_deinit(sl_net_interface_t interface)
{
  UNUSED_PARAMETER(interface);
  return sl_wifi_deinit();
}

sl_status_t sl_net_wifi_client_up(sl_net_interface_t interface, sl_net_profile_id_t profile_id)
{
  UNUSED_PARAMETER(interface);
  sl_status_t status;
  sl_net_wifi_client_profile_t profile;

  // Get the client profile using the provided profile_id
  status = sl_net_get_profile(SL_NET_WIFI_CLIENT_INTERFACE, profile_id, &profile);
  VERIFY_STATUS_AND_RETURN(status);

  // Connect to the Wi-Fi network
  status = sl_wifi_connect(SL_WIFI_CLIENT_INTERFACE, &profile.config, 18000);
  VERIFY_STATUS_AND_RETURN(status);

  // Configure the IP address settings
  status = sl_si91x_configure_ip_address(&profile.ip, SL_SI91X_WIFI_CLIENT_VAP_ID);
  VERIFY_STATUS_AND_RETURN(status);

  // Set the client profile
  status = sl_net_set_profile(SL_NET_WIFI_CLIENT_INTERFACE, profile_id, &profile);
  return status;
}

sl_status_t sl_net_wifi_client_down(sl_net_interface_t interface)
{
  UNUSED_PARAMETER(interface);

  // Disconnect from the Wi-Fi network
  return sl_wifi_disconnect(SL_WIFI_CLIENT_INTERFACE);
}

sl_status_t sl_net_wifi_ap_init(sl_net_interface_t interface,
                                const void *configuration,
                                void *workspace,
                                sl_net_event_handler_t event_handler)
{
  UNUSED_PARAMETER(interface);
  UNUSED_PARAMETER(workspace);
  sl_status_t status = SL_STATUS_FAIL;

  // Set the user-defined event handler for AP mode
  sl_si91x_register_event_handler(event_handler);

  status = sl_wifi_init(configuration, NULL, sl_wifi_default_event_handler);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_net_wifi_ap_deinit(sl_net_interface_t interface)
{
  UNUSED_PARAMETER(interface);
  return sl_wifi_deinit();
}

sl_status_t sl_net_wifi_ap_up(sl_net_interface_t interface, sl_net_profile_id_t profile_id)
{
  UNUSED_PARAMETER(interface);
  sl_status_t status;
  sl_net_wifi_ap_profile_t profile;

  status = sl_net_get_profile(SL_NET_WIFI_AP_INTERFACE, profile_id, &profile);
  VERIFY_STATUS_AND_RETURN(status);

  // Validate if profile configuration is valid
  // AP + DHCP client not supported
  // AP + link local not supported
  if (profile.ip.mode != SL_IP_MANAGEMENT_STATIC_IP) {
    return SL_STATUS_INVALID_CONFIGURATION;
  }
  status = sl_si91x_configure_ip_address(&profile.ip, SL_SI91X_WIFI_AP_VAP_ID);
  VERIFY_STATUS_AND_RETURN(status);

  status = sl_wifi_start_ap(SL_WIFI_AP_2_4GHZ_INTERFACE, &profile.config);
  VERIFY_STATUS_AND_RETURN(status);

  return status;
}

sl_status_t sl_net_wifi_ap_down(sl_net_interface_t interface)
{
  UNUSED_PARAMETER(interface);
  return sl_wifi_stop_ap(SL_WIFI_AP_INTERFACE);
}

sl_status_t sl_net_join_multicast_address(sl_net_interface_t interface, const sl_ip_address_t *ip_address)
{
  return sli_si91x_send_multicast_request((sl_wifi_interface_t)interface, ip_address, SL_WIFI_MULTICAST_JOIN);
}

sl_status_t sl_net_leave_multicast_address(sl_net_interface_t interface, const sl_ip_address_t *ip_address)
{
  return sli_si91x_send_multicast_request((sl_wifi_interface_t)interface, ip_address, SL_WIFI_MULTICAST_LEAVE);
}

static sl_status_t sli_si91x_send_multicast_request(sl_wifi_interface_t interface,
                                                    const sl_ip_address_t *ip_address,
                                                    uint8_t command_type)
{
  UNUSED_PARAMETER(interface);
  si91x_req_multicast_t multicast = { 0 };
  sl_status_t status              = SL_STATUS_OK;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  //Fill IP version and IP address
  if (ip_address->type == SL_IPV6) {
    multicast.ip_version[0] = 6;
    memcpy(multicast.multicast_address.ipv6_address, ip_address->ip.v6.bytes, RSI_IP_ADDRESS_LEN * 4);
  } else {
    multicast.ip_version[0] = 4;
    memcpy(multicast.multicast_address.ipv4_address, ip_address->ip.v4.bytes, RSI_IP_ADDRESS_LEN);
  }
  multicast.type[0] = command_type;

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_MULTICAST,
                                        SI91X_NETWORK_CMD_QUEUE,
                                        &multicast,
                                        sizeof(multicast),
                                        SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                        NULL,
                                        NULL);

  VERIFY_STATUS_AND_RETURN(status);
  return SL_STATUS_OK;
}

// Resolve a host name to an IP address using DNS
sl_status_t sl_net_host_get_by_name(const char *host_name,
                                    const uint32_t timeout,
                                    const sl_net_dns_resolution_ip_type_t dns_resolution_ip,
                                    sl_ip_address_t *sl_ip_address)
{
  // Check for a NULL pointer for sl_ip_address
  SL_WIFI_ARGS_CHECK_NULL_POINTER(sl_ip_address);

  sl_status_t status;
  sl_si91x_packet_t *packet;
  sl_wifi_buffer_t *buffer                       = NULL;
  sl_si91x_dns_response_t *dns_response          = { 0 };
  sl_si91x_dns_query_request_t dns_query_request = { 0 };

  // Determine the wait period based on the timeout value
  sl_si91x_wait_period_t wait_period = timeout == 0 ? SL_SI91X_RETURN_IMMEDIATELY : SL_SI91X_WAIT_FOR_RESPONSE(timeout);
  // Determine the IP version to be used (IPv4 or IPv6)
  dns_query_request.ip_version[0] = (dns_resolution_ip == SL_NET_DNS_TYPE_IPV4) ? 4 : 6;
  memcpy(dns_query_request.url_name, host_name, sizeof(dns_query_request.url_name));

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_DNS_QUERY,
                                        SI91X_NETWORK_CMD_QUEUE,
                                        &dns_query_request,
                                        sizeof(dns_query_request),
                                        wait_period,
                                        NULL,
                                        &buffer);

  // Check if the command failed and free the buffer if it was allocated
  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sl_si91x_host_free_buffer(buffer);
  }
  VERIFY_STATUS_AND_RETURN(status);

  // Extract the DNS response from the SI91X packet buffer
  packet       = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  dns_response = (sl_si91x_dns_response_t *)packet->data;

  // Convert the SI91X DNS response to the sl_ip_address format
  convert_si91x_dns_response(sl_ip_address, dns_response);
  sl_si91x_host_free_buffer(buffer);
  return SL_STATUS_OK;
}
