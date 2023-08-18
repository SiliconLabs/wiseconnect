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

#define SL_SI91X_DHCP_UNICAST_OFFER ((uint32_t)1U << 3)

extern bool device_initialized;

typedef enum {
  SL_WIFI_MULTICAST_LEAVE = 0,
  SL_WIFI_MULTICAST_JOIN  = 1,
} sl_si91x_multicast_address_command_type_t;

typedef enum {
  SL_SI91X_STATIC = 0,
  SL_SI91X_DHCP,
  SL_SI91X_DHCP_RESERVED,
  SL_SI91X_DHCP_HOSTNAME,
  SL_SI91X_DHCP_OPTION81,
  SL_SI91X_DHCP_OPTION77
} sl_si91x_ip_config_mode_t;

static sl_net_event_handler_t user_client_event_handler = NULL;
static sl_net_event_handler_t user_ap_event_handler     = NULL;
extern sl_status_t default_wifi_event_handler(sl_wifi_event_t event, sl_wifi_buffer_t *buffer);

static sl_status_t send_multicast_request(sl_wifi_interface_t interface,
                                          const sl_ip_address_t *ip_address,
                                          uint8_t command_type);

static sl_status_t net_wifi_client_event_handler(sl_wifi_event_t event, sl_wifi_buffer_t *buffer)
{
  default_wifi_event_handler(event, buffer);

  // Check for certain events and process them
  // Sometimes notify the user provided event handler
  if (user_client_event_handler) {
    uint16_t data_length      = 0;
    sl_si91x_packet_t *packet = sl_si91x_host_get_buffer_data(buffer, 0, &data_length);
    user_client_event_handler(event,
                              convert_firmware_status(get_si91x_frame_status(packet)),
                              packet->data,
                              packet->length);
  }
  return SL_STATUS_OK;
}

static sl_status_t net_wifi_ap_event_handler(sl_wifi_event_t event, sl_wifi_buffer_t *buffer)
{
  default_wifi_event_handler(event, buffer);

  // Check for certain events and process them
  // Sometimes notify the user provided event handler
  if (user_ap_event_handler) {
    uint16_t data_length      = 0;
    sl_si91x_packet_t *packet = sl_si91x_host_get_buffer_data(buffer, 0, &data_length);
    user_ap_event_handler(event, 0, packet->data, packet->length);
  }
  return SL_STATUS_OK;
}

// wifi_client APIs
sl_status_t sl_net_wifi_client_init(sl_net_interface_t interface,
                                    const void *configuration,
                                    void *context,
                                    sl_net_event_handler_t event_handler)
{
  UNUSED_PARAMETER(interface);
  UNUSED_PARAMETER(context);
  user_client_event_handler = event_handler;
  sl_status_t status        = sl_wifi_init(configuration, net_wifi_client_event_handler);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_net_wifi_client_deinit(sl_net_interface_t interface, void *wifi_client_context)
{
  UNUSED_PARAMETER(interface);
  UNUSED_PARAMETER(wifi_client_context);

  if (!sl_wifi_is_interface_up(SL_WIFI_CLIENT_INTERFACE)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  return sl_wifi_deinit();
}

sl_status_t sl_net_wifi_client_up(sl_net_interface_t interface, sl_net_profile_id_t profile_id)
{
  UNUSED_PARAMETER(interface);
  sl_status_t status;
  sl_net_wifi_client_profile_t profile;

  status = sl_net_get_profile(SL_NET_WIFI_CLIENT_INTERFACE, profile_id, &profile);
  VERIFY_STATUS_AND_RETURN(status);

  status = sl_wifi_connect(SL_WIFI_CLIENT_INTERFACE, &profile.config, 15000);
  VERIFY_STATUS_AND_RETURN(status);

  status = sl_si91x_configure_ip_address(&profile.ip, CLIENT_MODE);
  VERIFY_STATUS_AND_RETURN(status);

  status = sl_net_set_profile(SL_NET_WIFI_CLIENT_INTERFACE, profile_id, &profile);
  return status;
}

sl_status_t sl_net_wifi_client_down(sl_net_interface_t interface)
{
  UNUSED_PARAMETER(interface);
  return sl_wifi_disconnect(SL_WIFI_CLIENT_INTERFACE);
}

// wifi_softap APIs
sl_status_t sl_net_wifi_ap_init(sl_net_interface_t interface,
                                const void *configuration,
                                void *workspace,
                                sl_net_event_handler_t event_handler)
{
  UNUSED_PARAMETER(interface);
  UNUSED_PARAMETER(workspace);
  user_ap_event_handler = event_handler;
  sl_status_t status    = sl_wifi_init(configuration, net_wifi_ap_event_handler);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_net_wifi_ap_deinit(sl_net_interface_t interface, void *wifi_ap_context)
{
  UNUSED_PARAMETER(interface);
  UNUSED_PARAMETER(wifi_ap_context);

  if (!sl_wifi_is_interface_up(SL_WIFI_AP_INTERFACE)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

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
  status = sl_si91x_configure_ip_address(&profile.ip, AP_MODE);
  VERIFY_STATUS_AND_RETURN(status);

  status = sl_wifi_start_ap(SL_WIFI_AP_2_4GHZ_INTERFACE, &profile.config);
  VERIFY_STATUS_AND_RETURN(status);

  return status;
}

sl_status_t sl_net_wifi_ap_down(sl_net_interface_t interface)
{
  UNUSED_PARAMETER(interface);
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_dns_host_get_by_name(const char *host_name,
                                    const uint32_t timeout,
                                    const sl_net_dns_resolution_ip_type_t dns_resolution_ip,
                                    sl_ip_address_t *sl_ip_address)
{
  ARGS_CHECK_NULL_POINTER(sl_ip_address);

  sl_status_t status;
  sl_si91x_packet_t *packet;
  sl_wifi_buffer_t *buffer;
  sl_si91x_dns_response_t *dns_response          = { 0 };
  sl_si91x_dns_query_request_t dns_query_request = { 0 };
  sl_si91x_wait_period_t wait_period = timeout == 0 ? SL_SI91X_RETURN_IMMEDIATELY : SL_SI91X_WAIT_FOR_RESPONSE(timeout);

  dns_query_request.ip_version[0] = (dns_resolution_ip == SL_NET_DNS_TYPE_IPV4) ? 4 : 6;
  memcpy(dns_query_request.url_name, host_name, strlen(host_name));

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_DNS_QUERY,
                                        SI91X_NETWORK_CMD_QUEUE,
                                        &dns_query_request,
                                        sizeof(dns_query_request),
                                        wait_period,
                                        NULL,
                                        &buffer);
  VERIFY_STATUS_AND_RETURN(status);

  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);

  dns_response = (sl_si91x_dns_response_t *)packet->data;
  convert_si91x_dns_response(sl_ip_address, dns_response);

  sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);

  return SL_STATUS_OK;
}

sl_status_t sl_si91x_ota_firmware_upgradation(sl_ip_address_t server_ip,
                                              uint16_t server_port,
                                              uint16_t chunk_number,
                                              uint16_t timeout,
                                              uint16_t tcp_retry_count,
                                              bool asynchronous)
{
  sl_wifi_buffer_t *buffer;
  sl_status_t status                               = SL_STATUS_FAIL;
  sl_si91x_wait_period_t wait_period               = SL_SI91X_RETURN_IMMEDIATELY;
  sl_si91x_ota_firmware_update_request_t otaf_fwup = { 0 };

  if (asynchronous == false) {
    wait_period = SL_SI91X_WAIT_FOR_RESPONSE(70000);
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

  // Send OTAF command
  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_OTA_FWUP,
                                        SI91X_NETWORK_CMD_QUEUE,
                                        &otaf_fwup,
                                        sizeof(sl_si91x_ota_firmware_update_request_t),
                                        wait_period,
                                        NULL,
                                        &buffer);
  if (asynchronous == false) {
    VERIFY_STATUS_AND_RETURN(status);
    sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
  }
  return status;
}

sl_status_t sl_si91x_configure_ip_address(sl_net_ip_configuration_t *address, uint8_t virtual_ap_id)
{
  sl_status_t status = SL_STATUS_FAIL;
  sl_si91x_req_ipv4_params_t ip_req;
  sl_si91x_req_ipv6_params_t ipv6_request;
  sl_si91x_packet_t *packet;
  sl_wifi_buffer_t *buffer;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (NULL == address) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (SL_IPV4 == address->type) {
    memset(&ip_req, 0, sizeof(ip_req));
    ip_req.vap_id = virtual_ap_id;

    if (SL_IP_MANAGEMENT_STATIC_IP == address->mode) {
      ip_req.dhcp_mode = SL_SI91X_STATIC;
      // Fill IP address
      memcpy(ip_req.ipaddress, address->ip.v4.ip_address.bytes, 4);

      // Fill network mask
      memcpy(ip_req.netmask, address->ip.v4.netmask.bytes, 4);

      // Fill gateway
      memcpy(ip_req.gateway, address->ip.v4.gateway.bytes, 4);
    } else {
      ip_req.dhcp_mode = (SL_SI91X_DHCP | SL_SI91X_DHCP_UNICAST_OFFER);
    }

    if (NULL != address->host_name) {
      ip_req.dhcp_mode |= SL_SI91X_DHCP_HOSTNAME;
      memcpy(ip_req.hostname, address->host_name, strlen(address->host_name));
    }

    status = sl_si91x_driver_send_command(RSI_WLAN_REQ_IPCONFV4,
                                          SI91X_NETWORK_CMD_QUEUE,
                                          &ip_req,
                                          sizeof(sl_si91x_req_ipv4_params_t),
                                          SL_SI91X_WAIT_FOR_RESPONSE(150000),
                                          NULL,
                                          &buffer);
    VERIFY_STATUS_AND_RETURN(status);
    packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);

    if (SL_IP_MANAGEMENT_DHCP == address->mode) {
      status = convert_rsi_ipv4_address_to_sl_ip_address((sl_ip_address_t *)&(address->ip.v4.ip_address),
                                                         (sl_si91x_rsp_ipv4_params_t *)packet->data);
      VERIFY_STATUS_AND_RETURN(status);
    }

    sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
  } else if (SL_IPV6 == address->type) {
    memset(&ipv6_request, 0, sizeof(ipv6_request));
    uint16_t prefix_length = 64;
    memcpy(&ipv6_request.prefixLength, &prefix_length, 2);

    if (SL_IP_MANAGEMENT_STATIC_IP == address->mode) {
      memcpy(&ipv6_request.ipaddr6, address->ip.v6.global_address.bytes, SL_IPV6_ADDRESS_LENGTH);
      memcpy(&ipv6_request.gateway6, address->ip.v6.gateway.bytes, SL_IPV6_ADDRESS_LENGTH);
      ipv6_request.mode[0] = SL_SI91X_STATIC;
    } else {
      ipv6_request.mode[0] = (SL_SI91X_DHCP | SL_SI91X_DHCP_UNICAST_OFFER);
    }

    status = sl_si91x_driver_send_command(RSI_WLAN_REQ_IPCONFV6,
                                          SI91X_NETWORK_CMD_QUEUE,
                                          &ipv6_request,
                                          sizeof(sl_si91x_req_ipv6_params_t),
                                          SL_SI91X_WAIT_FOR_RESPONSE(50000),
                                          NULL,
                                          &buffer);
    VERIFY_STATUS_AND_RETURN(status);

    uint32_t input_buffer[13];
    packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
    memcpy(input_buffer, packet->data, packet->length);

    memcpy(&address->ip.v6.link_local_address, &input_buffer[1], 16);
    memcpy(&address->ip.v6.global_address, &input_buffer[5], 16);
    memcpy(&address->ip.v6.gateway, &input_buffer[9], 16);

    sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);

  } else {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return status;
}

sl_status_t sl_net_join_multicast_address(sl_net_interface_t interface, const sl_ip_address_t *ip_address)
{
  return send_multicast_request((sl_wifi_interface_t)interface, ip_address, SL_WIFI_MULTICAST_JOIN);
}

sl_status_t sl_net_leave_multicast_address(sl_net_interface_t interface, const sl_ip_address_t *ip_address)
{
  return send_multicast_request((sl_wifi_interface_t)interface, ip_address, SL_WIFI_MULTICAST_LEAVE);
}

static sl_status_t send_multicast_request(sl_wifi_interface_t interface,
                                          const sl_ip_address_t *ip_address,
                                          uint8_t command_type)
{
  UNUSED_PARAMETER(interface);
  si91x_req_multicast_t multicast;
  sl_status_t status;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  //Fill IP version and IP address
  if (ip_address->type == SL_IPV6) { //IPV6
    //Fill ipv6 version
    multicast.ip_version[0] = 6;
    //Fill ipv6 ip address
    memcpy(multicast.multicast_address.ipv6_address, ip_address->ip.v6.bytes, RSI_IP_ADDRESS_LEN * 4);
  } else {
    //Fill ipv4 version
    multicast.ip_version[0] = 4;
    //Fill ipv4 ip address
    memcpy(multicast.multicast_address.ipv4_address, ip_address->ip.v4.bytes, RSI_IP_ADDRESS_LEN);
  }
  //Fill command type
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
