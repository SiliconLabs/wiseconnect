/*******************************************************************************
 * @file  network_commands.c
 * @brief Network AT Commands
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

#include "sl_board_configuration.h"
#include "console.h"
#include "sl_net.h"
#include "sl_net_dns.h"
#include "sl_utility.h"
#include "sl_wifi.h"
#include "sl_net_default_values.h"
#include "sl_si91x_driver.h"
#include <string.h>
#include "sl_string.h"
#include "at_utility.h"

/******************************************************
 *                    Constants
 ******************************************************/

#ifdef SLI_SI91X_OFFLOAD_NETWORK_STACK
#define IP_ADDRESS "192.168.10.2"
#define GATEWAY    "192.168.10.2"
#define NETMASK    "255.255.255.0"
#define AP_PSK     "123456789"
#endif

#ifdef SLI_SI91X_LWIP_NETWORK_INTERFACE
#include "sl_net_ethernet_types.h"
extern sl_net_ethernet_api_t ethernet_interface;

sl_ethernet_context_t ethernet_context;
const sl_ethernet_config_t ethernet_config = {
  .mac_address       = { { 0xD6, 0x15, 0x2C, 0x98, 0x1E, 0x29 } },
  .phy_bus_mode      = SL_ETHERNET_PHY_BUS_MODE_RMII,
  .duplex_mode       = SL_ETHERNET_DUPLEX_MODE_FULL,
  .speed             = SL_ETHERNET_SPEED_100_MBPS,
  .phy_address       = 0xFFu,
  .pin_configuration = &ethernet_pin_configuration,
};
#endif

#ifdef SL_WIFI_COMPONENT_INCLUDED
#include "sl_net_wifi_types.h"
#include "sl_wifi_callback_framework.h"

#ifdef SLI_SI91X_LWIP_HOSTED_NETWORK_STACK
#include "sl_net_for_lwip.h"
static sl_net_wifi_lwip_context_t wifi_client_context;
static sl_net_wifi_lwip_context_t wifi_ap_context;
#elif SLI_SI91X_OFFLOAD_NETWORK_STACK
#include "sl_net_si91x.h"
#include "sl_wifi_callback_framework.h"
#include "sl_net_ping.h"

extern char *strtok_r(char *, const char *, char **);
static sl_status_t convert_string_to_sl_ipv6_address(char *line, uint16_t *ipv6);

sl_status_t ping_response_callback_handler(sl_net_event_t event, sl_status_t status, void *data, uint32_t user_data);
static sl_status_t ap_connected_event_handler(sl_wifi_event_t event, void *data, uint32_t data_length, void *arg)
{
  UNUSED_PARAMETER(data_length);
  UNUSED_PARAMETER(event);
  UNUSED_PARAMETER(arg);

  printf("at+WIFI_AP_CLIENT_CONNECTED=");
  print_mac_address((sl_mac_address_t *)data);
  printf("\r\n>\r\n");

  return SL_STATUS_OK;
}

static sl_status_t ap_disconnected_event_handler(sl_wifi_event_t event, void *data, uint32_t data_length, void *arg)
{
  UNUSED_PARAMETER(data_length);
  UNUSED_PARAMETER(arg);
  UNUSED_PARAMETER(event);

  printf("at+WIFI_AP_CLIENT_DISCONNECTED=");
  print_mac_address((sl_mac_address_t *)data);
  printf("\r\n>\r\n");

  return SL_STATUS_OK;
}

#endif

#endif

#define MAX_WIFI_PROFILES MAX_WIFI_CLIENT_PROFILES
#define OTAF_DEFAULT_PORT 80

static sl_net_wifi_client_profile_t *wifi_client_profiles[MAX_WIFI_PROFILES] = { NULL };
static sl_net_ip_configuration_t ip_config                                   = { 0 };

extern sl_wifi_device_configuration_t si91x_init_configuration;
void display_wifi_client_profile(const sl_net_wifi_client_profile_t *profile);

void initialize_wifi_profiles(sl_net_profile_id_t profile_id)
{
  if (profile_id <= MAX_WIFI_PROFILES) {
    wifi_client_profiles[profile_id] = (sl_net_wifi_client_profile_t *)malloc(sizeof(sl_net_wifi_client_profile_t));
    if (wifi_client_profiles[profile_id] != NULL) {
      memcpy(wifi_client_profiles[profile_id], &DEFAULT_WIFI_CLIENT_PROFILE, sizeof(sl_net_wifi_client_profile_t));
    }
  }
}

void free_wifi_profiles()
{
  for (int i = 0; i < MAX_WIFI_PROFILES; i++) {
    if (wifi_client_profiles[i] != NULL) {
      free(wifi_client_profiles[i]);
      wifi_client_profiles[i] = NULL;
    }
  }
}

sl_net_wifi_client_profile_t *get_wifi_profile(sl_net_profile_id_t profile_id)
{
  if (profile_id >= MAX_WIFI_PROFILES) {
    return NULL; // Invalid profile ID
  }
  if (wifi_client_profiles[profile_id] == NULL) {
    initialize_wifi_profiles(profile_id);
  }
  return wifi_client_profiles[profile_id];
}

sl_status_t net_init_check_command_handler(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  PRINT_AT_CMD_SUCCESS;
  printf("%d\r\n", sl_si91x_is_device_initialized());
  return SL_STATUS_OK;
}

sl_status_t net_init_command_handler(console_args_t *arguments)
{
  CHECK_ARGUMENT_BITMAP(arguments, 0x01);
  sl_status_t status;
  sl_net_interface_t interface = (sl_net_interface_t)GET_OPTIONAL_COMMAND_ARG(arguments, 0, 0, sl_net_interface_t);
  sl_net_event_handler_t event_handler = NULL;
  const void *config                   = NULL;

  if (si91x_init_configuration.boot_option != 0) {
    config = &si91x_init_configuration;
  }

  switch (SL_NET_INTERFACE_TYPE(interface)) {
#ifdef SLI_SI91X_LWIP_NETWORK_INTERFACE
    case SL_NET_ETHERNET_INTERFACE:
      status = sl_net_init(SL_NET_ETHERNET_INTERFACE, &ethernet_config, &ethernet_context, 0);
      VERIFY_STATUS_AND_RETURN(status);
      break;
#endif

#ifdef SL_WIFI_COMPONENT_INCLUDED
    case SL_NET_WIFI_CLIENT_INTERFACE:
#ifdef SLI_SI91X_OFFLOAD_NETWORK_STACK
      event_handler = ping_response_callback_handler;
      status        = sl_net_init(interface, config, NULL, event_handler);
#elif SLI_SI91X_LWIP_HOSTED_NETWORK_STACK
      status = sl_net_init(interface, config, &wifi_client_context, event_handler);
#endif
      VERIFY_STATUS_AND_RETURN(status);
      break;
    case SL_NET_WIFI_AP_INTERFACE:
#ifdef SLI_SI91X_OFFLOAD_NETWORK_STACK
      status = sl_net_init(interface, (const void *)config, NULL, NULL);
#elif SLI_SI91X_LWIP_HOSTED_NETWORK_STACK
      status = sl_net_init(interface, (const void *)config, (void *)&wifi_ap_context, NULL);
#endif
      VERIFY_STATUS_AND_RETURN(status);
      sl_wifi_set_callback(SL_WIFI_CLIENT_CONNECTED_EVENTS, ap_connected_event_handler, NULL);
      sl_wifi_set_callback(SL_WIFI_CLIENT_DISCONNECTED_EVENTS, ap_disconnected_event_handler, NULL);
      break;
#endif
    default:
      return SL_STATUS_INVALID_PARAMETER;
  }

  PRINT_AT_CMD_SUCCESS;
  return status;
}

sl_status_t net_deinit_command_handler(console_args_t *arguments)
{
  CHECK_ARGUMENT_BITMAP(arguments, 0x01);
  sl_status_t status;
  sl_net_interface_t interface = (sl_net_interface_t)GET_OPTIONAL_COMMAND_ARG(arguments, 0, 0, sl_net_interface_t);

  memset(&si91x_init_configuration, 0, sizeof(si91x_init_configuration));
  switch (SL_NET_INTERFACE_TYPE(interface)) {
#ifdef SL_WIFI_COMPONENT_INCLUDED
    case SL_NET_WIFI_CLIENT_INTERFACE:
      status = sl_net_wifi_client_deinit(interface);
      VERIFY_STATUS_AND_RETURN(status);
      break;

    case SL_NET_WIFI_AP_INTERFACE:
      status = sl_net_wifi_ap_deinit(interface);
      VERIFY_STATUS_AND_RETURN(status);
      break;
#endif
    default:
      return SL_STATUS_INVALID_PARAMETER;
  }
  PRINT_AT_CMD_SUCCESS;
  return status;
}

sl_status_t net_up_command_handler(console_args_t *arguments)
{
  CHECK_ARGUMENT_BITMAP(arguments, 0x03);
  sl_status_t status;
  sl_net_interface_t interface   = (sl_net_interface_t)GET_OPTIONAL_COMMAND_ARG(arguments, 0, 0, sl_net_interface_t);
  sl_net_profile_id_t profile_id = GET_OPTIONAL_COMMAND_ARG(arguments, 1, SL_NET_PROFILE_ID_0, sl_net_profile_id_t);

  switch (SL_NET_INTERFACE_TYPE(interface)) {
#ifdef SL_WIFI_COMPONENT_INCLUDED
    case SL_NET_WIFI_CLIENT_INTERFACE: {

      if (profile_id == SL_NET_AUTO_JOIN) {
        status = sl_net_wifi_client_up(interface, profile_id);
        VERIFY_STATUS_AND_RETURN(status);
        break;
      }
      // Fetch the profile and print some information about it
      sl_net_wifi_client_profile_t *profile = get_wifi_profile(profile_id);
      if (profile == NULL) {
        return SL_STATUS_INVALID_PARAMETER; // Invalid profile ID
      }
      SL_DEBUG_LOG("Connecting to '%s'\r\n", profile->config.ssid.value);
      status = sl_net_wifi_client_up(interface, profile_id);
      VERIFY_STATUS_AND_RETURN(status);

      status = sl_net_get_profile(SL_NET_WIFI_CLIENT_INTERFACE, profile_id, profile);
      if (status == SL_STATUS_OK) {
        display_wifi_client_profile(profile);
      }

    } break;

    case SL_NET_WIFI_AP_INTERFACE: {
      sl_net_wifi_ap_profile_t profile;

      // Fetch the profile and print some information about it
      status = sl_net_get_profile(SL_NET_WIFI_AP_INTERFACE, profile_id, &profile);
      if (status != SL_STATUS_OK) {
        printf("Failed to load profile with id: %u\r\n", profile_id);
        return status;
      }
      printf("Starting AP with SSID '%s'\r\n", profile.config.ssid.value);
      status = sl_net_wifi_ap_up(interface, profile_id);
      VERIFY_STATUS_AND_RETURN(status);
    } break;
#endif
    default:
      return SL_STATUS_INVALID_PARAMETER;
  }
  PRINT_AT_CMD_SUCCESS;
  return status;
}

sl_status_t net_down_command_handler(console_args_t *arguments)
{
  CHECK_ARGUMENT_BITMAP(arguments, 0x01);
  sl_status_t status;
  sl_net_interface_t interface = (sl_net_interface_t)GET_OPTIONAL_COMMAND_ARG(arguments, 0, 0, sl_net_interface_t);

  switch (SL_NET_INTERFACE_TYPE(interface)) {
#ifdef SL_WIFI_COMPONENT_INCLUDED
    case SL_NET_WIFI_CLIENT_INTERFACE:
      status = sl_net_wifi_client_down(interface);
      VERIFY_STATUS_AND_RETURN(status);
      break;

    case SL_NET_WIFI_AP_INTERFACE:
      status = sl_net_wifi_ap_down(interface);
      VERIFY_STATUS_AND_RETURN(status);
      break;
#endif

    default:
      return SL_STATUS_INVALID_PARAMETER;
  }

  PRINT_AT_CMD_SUCCESS;
  return SL_STATUS_OK;
}

sl_status_t net_cred_wifipsk_command_handler(console_args_t *arguments)
{
  CHECK_ARGUMENT_BITMAP(arguments, 0x03);
  sl_net_credential_id_t cred_id = GET_OPTIONAL_COMMAND_ARG(arguments, 0, 0, sl_net_credential_id_t);
  const char *password           = GET_OPTIONAL_COMMAND_ARG(arguments, 1, NULL, const char *);

  if ((password == NULL) || (cred_id == 0))
    return SL_STATUS_INVALID_PARAMETER;

  sl_status_t status = sl_net_set_credential(cred_id, SL_NET_WIFI_PSK, password, strlen(password));
  VERIFY_STATUS_AND_RETURN(status);
  PRINT_AT_CMD_SUCCESS;

  return SL_STATUS_OK;
}

void display_wifi_client_profile(const sl_net_wifi_client_profile_t *profile)
{
  UNUSED_PARAMETER(profile);
#ifdef AT_DEBUG_ENABLE
  if (profile == NULL) {
    printf("Wi-Fi client profile is NULL.\n");
    return;
  }

  printf("Wi-Fi Client Profile Details:\n");
  printf("---------------------------------\n");

  // Display SSID
  printf("SSID: %s\n", profile->config.ssid.value);
  printf("SSID Length: %d\n", profile->config.ssid.length);

  // Display Channel Information
  printf("Channel: %d\n", profile->config.channel.channel);
  printf("Band: %d\n", profile->config.channel.band);
  printf("Bandwidth: %d\n", profile->config.channel.bandwidth);

  // Display BSSID
  printf("BSSID: ");
  for (uint8_t i = 0; i < sizeof(profile->config.bssid.octet); i++) {
    printf("%02X", profile->config.bssid.octet[i]);
    if (i < sizeof(profile->config.bssid.octet) - 1) {
      printf(":");
    }
  }
  printf("\n BSS type : %d\r\n", profile->config.bss_type);

  // Display Security and Encryption
  printf("Security: %d\n", profile->config.security);
  printf("Encryption: %d\n", profile->config.encryption);

  // Display Client Options
  printf("Client Options: %d\n", profile->config.client_options);

  // Display Credential ID
  printf("Credential ID: %ld\n", profile->config.credential_id);

  // Display IP Configuration
  printf("IP Mode: %d\n", profile->ip.mode);
  printf("IP Type: %d\n", profile->ip.type);
  if (profile->ip.host_name != NULL) {
    printf("Host Name: %s\n", profile->ip.host_name);
  } else {
    printf("Host Name: NULL\n");
  }

  if (profile->ip.type == SL_IPV4) {
    printf("IPv4 Address: %d.%d.%d.%d\n",
           profile->ip.ip.v4.ip_address.bytes[0],
           profile->ip.ip.v4.ip_address.bytes[1],
           profile->ip.ip.v4.ip_address.bytes[2],
           profile->ip.ip.v4.ip_address.bytes[3]);

    printf("Gateway: %d.%d.%d.%d\n",
           profile->ip.ip.v4.gateway.bytes[0],
           profile->ip.ip.v4.gateway.bytes[1],
           profile->ip.ip.v4.gateway.bytes[2],
           profile->ip.ip.v4.gateway.bytes[3]);

    printf("Netmask: %d.%d.%d.%d\n",
           profile->ip.ip.v4.netmask.bytes[0],
           profile->ip.ip.v4.netmask.bytes[1],
           profile->ip.ip.v4.netmask.bytes[2],
           profile->ip.ip.v4.netmask.bytes[3]);
  } else if (profile->ip.type == SL_IPV6) {
    printf("IPv6 Global Address: ");
    for (int i = 0; i < SL_IPV6_ADDRESS_LENGTH; i++) {
      printf("%02X", profile->ip.ip.v6.global_address.bytes[i]);
      if (i < SL_IPV6_ADDRESS_LENGTH - 1) {
        printf(":");
      }
    }
    printf("\n");

    printf("IPv6 Link Local Address: ");
    for (int i = 0; i < SL_IPV6_ADDRESS_LENGTH; i++) {
      printf("%02X", profile->ip.ip.v6.link_local_address.bytes[i]);
      if (i < SL_IPV6_ADDRESS_LENGTH - 1) {
        printf(":");
      }
    }
    printf("\n");

    printf("IPv6 Gateway: ");
    for (int i = 0; i < SL_IPV6_ADDRESS_LENGTH; i++) {
      printf("%02X", profile->ip.ip.v6.gateway.bytes[i]);
      if (i < SL_IPV6_ADDRESS_LENGTH - 1) {
        printf(":");
      }
    }
    printf("\n");
  }

  // Print DHCP configuration
  printf("DHCP Configuration:\n");
  printf("  Min Discover Retry Interval: %d ms\n", profile->ip.dhcp_config.min_discover_retry_interval);
  printf("  Max Discover Retry Interval: %d ms\n", profile->ip.dhcp_config.max_discover_retry_interval);
  printf("  Min Request Retry Interval: %d ms\n", profile->ip.dhcp_config.min_request_retry_interval);
  printf("  Max Request Retry Interval: %d ms\n", profile->ip.dhcp_config.max_request_retry_interval);
  printf("  Max Discover Retries: %d\n", profile->ip.dhcp_config.max_discover_retries);
  printf("  Max Request Retries: %d\n", profile->ip.dhcp_config.max_request_retries);

  printf("---------------------------------\n");
#endif
}

sl_status_t net_sta_credential_command_handler(console_args_t *arguments)
{
  CHECK_ARGUMENT_BITMAP(arguments, 0x07);
  sl_net_profile_id_t profile_id = GET_OPTIONAL_COMMAND_ARG(arguments, 0, SL_NET_PROFILE_ID_0, sl_net_profile_id_t);
  char *ssid                     = GET_OPTIONAL_COMMAND_ARG(arguments, 1, NULL, char *);
  sl_net_wifi_client_profile_t *profile = get_wifi_profile(profile_id);

  if (profile == NULL) {
    return SL_STATUS_INVALID_PARAMETER; // Invalid profile ID
  }

  profile->config.credential_id =
    GET_OPTIONAL_COMMAND_ARG(arguments, 2, SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID, sl_net_credential_id_t);
  profile->config.security   = GET_OPTIONAL_COMMAND_ARG(arguments, 3, SL_WIFI_WPA2, sl_wifi_security_t);
  profile->config.encryption = GET_OPTIONAL_COMMAND_ARG(arguments, 4, SL_WIFI_DEFAULT_ENCRYPTION, sl_wifi_encryption_t);

  if (ssid == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  profile->config.ssid.length = sl_strlen(ssid);
  memcpy(profile->config.ssid.value, ssid, profile->config.ssid.length);
  profile->config.ssid.value[profile->config.ssid.length] = 0;

  display_wifi_client_profile(profile);

  sl_status_t status = sl_net_set_profile(SL_NET_WIFI_CLIENT_INTERFACE, profile_id, profile);
  VERIFY_STATUS_AND_RETURN(status);
  PRINT_AT_CMD_SUCCESS;

  return SL_STATUS_OK;
}

sl_status_t net_sta_channel_command_handler(console_args_t *arguments)
{
  CHECK_ARGUMENT_BITMAP(arguments, 0x03);
  sl_net_profile_id_t profile_id = GET_OPTIONAL_COMMAND_ARG(arguments, 0, SL_NET_PROFILE_ID_0, sl_net_profile_id_t);
  sl_net_wifi_client_profile_t *profile = get_wifi_profile(profile_id);

  if (profile == NULL) {
    return SL_STATUS_INVALID_PARAMETER; // Invalid profile ID
  }

  profile->config.channel.channel   = GET_OPTIONAL_COMMAND_ARG(arguments, 1, 0, uint16_t);
  profile->config.channel.band      = GET_OPTIONAL_COMMAND_ARG(arguments, 2, SL_WIFI_AUTO_BAND, uint8_t);
  profile->config.channel.bandwidth = GET_OPTIONAL_COMMAND_ARG(arguments, 3, SL_WIFI_AUTO_BANDWIDTH, uint8_t);

  display_wifi_client_profile(profile);

  sl_status_t status = sl_net_set_profile(SL_NET_WIFI_CLIENT_INTERFACE, profile_id, profile);
  VERIFY_STATUS_AND_RETURN(status);
  PRINT_AT_CMD_SUCCESS;

  return SL_STATUS_OK;
}

sl_status_t net_sta_bss_command_handler(console_args_t *arguments)
{
  CHECK_ARGUMENT_BITMAP(arguments, 0x03);
  sl_net_profile_id_t profile_id = GET_OPTIONAL_COMMAND_ARG(arguments, 0, SL_NET_PROFILE_ID_0, sl_net_profile_id_t);
  sl_net_wifi_client_profile_t *profile = get_wifi_profile(profile_id);

  if (profile == NULL) {
    return SL_STATUS_INVALID_PARAMETER; // Invalid profile ID
  }

  char *bssid              = GET_OPTIONAL_COMMAND_ARG(arguments, 1, NULL, char *);
  profile->config.bss_type = GET_OPTIONAL_COMMAND_ARG(arguments, 2, SL_WIFI_BSS_TYPE_INFRASTRUCTURE, uint16_t);

  memset(&profile->config.bssid, 0, sizeof(sl_mac_address_t));
  if (bssid != NULL) {
    convert_string_to_mac_address(bssid, &profile->config.bssid);
  }
  display_wifi_client_profile(profile);

  sl_status_t status = sl_net_set_profile(SL_NET_WIFI_CLIENT_INTERFACE, profile_id, profile);
  VERIFY_STATUS_AND_RETURN(status);

  status = sl_si91x_set_join_configuration(SL_WIFI_CLIENT_INTERFACE,
                                           SL_SI91X_JOIN_FEAT_LISTEN_INTERVAL_VALID | SL_SI91X_JOIN_FEAT_BSSID_BASED);
  if (status != SL_STATUS_OK) {
    printf("Failed to start set join configuration: 0x%lx\r\n", status);
    return SL_STATUS_FAIL;
  }
  PRINT_AT_CMD_SUCCESS;

  return SL_STATUS_OK;
}

sl_status_t net_sta_options_command_handler(console_args_t *arguments)
{
  CHECK_ARGUMENT_BITMAP(arguments, 0x01);
  sl_net_profile_id_t profile_id = GET_OPTIONAL_COMMAND_ARG(arguments, 0, SL_NET_PROFILE_ID_0, sl_net_profile_id_t);
  sl_net_wifi_client_profile_t *profile = get_wifi_profile(profile_id);

  if (profile == NULL) {
    return SL_STATUS_INVALID_PARAMETER; // Invalid profile ID
  }

  profile->config.client_options                    = GET_OPTIONAL_COMMAND_ARG(arguments, 1, 0, uint8_t);
  profile->config.channel_bitmap.channel_bitmap_2_4 = GET_OPTIONAL_COMMAND_ARG(arguments, 2, 0, uint16_t);
  profile->config.channel_bitmap.channel_bitmap_5   = GET_OPTIONAL_COMMAND_ARG(arguments, 3, 0, uint32_t);
  profile->priority                                 = GET_OPTIONAL_COMMAND_ARG(arguments, 4, 0, uint32_t);

  display_wifi_client_profile(profile);

  sl_status_t status = sl_net_set_profile(SL_NET_WIFI_CLIENT_INTERFACE, profile_id, profile);
  VERIFY_STATUS_AND_RETURN(status);
  PRINT_AT_CMD_SUCCESS;

  return SL_STATUS_OK;
}

sl_status_t net_sta_ip_command_handler(console_args_t *arguments)
{
  CHECK_ARGUMENT_BITMAP(arguments, 0x07);
  sl_net_profile_id_t profile_id = GET_OPTIONAL_COMMAND_ARG(arguments, 0, SL_NET_PROFILE_ID_0, sl_net_profile_id_t);
  sl_net_wifi_client_profile_t *profile = get_wifi_profile(profile_id);

  if (profile == NULL) {
    return SL_STATUS_INVALID_PARAMETER; // Invalid profile ID
  }

  profile->ip.mode      = GET_OPTIONAL_COMMAND_ARG(arguments, 1, SL_IP_MANAGEMENT_DHCP, uint8_t);
  profile->ip.type      = GET_OPTIONAL_COMMAND_ARG(arguments, 2, SL_IPV4, uint8_t);
  profile->ip.host_name = GET_OPTIONAL_COMMAND_ARG(arguments, 3, NULL, char *);

  if (profile->ip.mode == SL_IP_MANAGEMENT_STATIC_IP) {
    if (profile->ip.type == SL_IPV4) {
      char *sta_ip  = GET_OPTIONAL_COMMAND_ARG(arguments, 4, NULL, char *);
      char *gateway = GET_OPTIONAL_COMMAND_ARG(arguments, 6, NULL, char *);
      char *netmask = GET_OPTIONAL_COMMAND_ARG(arguments, 7, NULL, char *);

      if ((NULL == sta_ip) && (NULL == gateway) && (NULL == netmask))
        return SL_STATUS_INVALID_PARAMETER;

      convert_string_to_sl_ipv4_address(sta_ip, &profile->ip.ip.v4.ip_address);
      convert_string_to_sl_ipv4_address(gateway, &profile->ip.ip.v4.gateway);
      convert_string_to_sl_ipv4_address(netmask, &profile->ip.ip.v4.netmask);
    } else if (profile->ip.type == SL_IPV6) {
      char *v6_global_ip = GET_OPTIONAL_COMMAND_ARG(arguments, 5, NULL, char *);

      if (v6_global_ip == NULL) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      convert_string_to_sl_ipv6_address(v6_global_ip, (uint16_t *)profile->ip.ip.v6.global_address.bytes);
    }
  }

  display_wifi_client_profile(profile);

  sl_status_t status = sl_net_set_profile(SL_NET_WIFI_CLIENT_INTERFACE, profile_id, profile);
  VERIFY_STATUS_AND_RETURN(status);
  PRINT_AT_CMD_SUCCESS;

  return SL_STATUS_OK;
}

sl_status_t net_dhcp_config_command_handler(console_args_t *arguments)
{
  CHECK_ARGUMENT_BITMAP(arguments, 0x3f);
  ip_config.dhcp_config.min_discover_retry_interval = GET_OPTIONAL_COMMAND_ARG(arguments, 0, 5, uint16_t);
  ip_config.dhcp_config.max_discover_retry_interval = GET_OPTIONAL_COMMAND_ARG(arguments, 1, 100, uint16_t);
  ip_config.dhcp_config.min_request_retry_interval  = GET_OPTIONAL_COMMAND_ARG(arguments, 2, 5, uint16_t);
  ip_config.dhcp_config.max_request_retry_interval  = GET_OPTIONAL_COMMAND_ARG(arguments, 3, 10, uint16_t);
  ip_config.dhcp_config.min_discover_retries        = GET_OPTIONAL_COMMAND_ARG(arguments, 4, 5, uint8_t);
  ip_config.dhcp_config.max_request_retries         = GET_OPTIONAL_COMMAND_ARG(arguments, 5, 10, uint8_t);

  PRINT_AT_CMD_SUCCESS;
  return SL_STATUS_OK;
}

sl_status_t net_configure_ip_command_handler(console_args_t *arguments)
{
  CHECK_ARGUMENT_BITMAP(arguments, 0x07);
  sl_net_interface_t interface =
    (sl_net_interface_t)GET_OPTIONAL_COMMAND_ARG(arguments, 0, SL_NET_WIFI_CLIENT_INTERFACE, sl_net_interface_t);
  uint32_t timeout = GET_OPTIONAL_COMMAND_ARG(arguments, 10, SLI_SI91X_WAIT_FOR_EVER, uint32_t);

  ip_config.mode      = GET_OPTIONAL_COMMAND_ARG(arguments, 1, SL_IP_MANAGEMENT_DHCP, uint8_t);
  ip_config.type      = GET_OPTIONAL_COMMAND_ARG(arguments, 2, SL_IPV4, uint8_t);
  ip_config.host_name = GET_OPTIONAL_COMMAND_ARG(arguments, 3, NULL, char *);

  if (ip_config.mode == SL_IP_MANAGEMENT_STATIC_IP) {
    if (ip_config.type == SL_IPV4) {
      char *sta_ip  = GET_OPTIONAL_COMMAND_ARG(arguments, 4, NULL, char *);
      char *gateway = GET_OPTIONAL_COMMAND_ARG(arguments, 5, NULL, char *);
      char *netmask = GET_OPTIONAL_COMMAND_ARG(arguments, 6, NULL, char *);

      if ((NULL == sta_ip) && (NULL == gateway) && (NULL == netmask))
        return SL_STATUS_INVALID_PARAMETER;

      convert_string_to_sl_ipv4_address(sta_ip, &ip_config.ip.v4.ip_address);
      convert_string_to_sl_ipv4_address(gateway, &ip_config.ip.v4.gateway);
      convert_string_to_sl_ipv4_address(netmask, &ip_config.ip.v4.netmask);
    } else if (ip_config.type == SL_IPV6) {
      char *v6_link_local = GET_OPTIONAL_COMMAND_ARG(arguments, 7, NULL, char *);
      char *v6_global_ip  = GET_OPTIONAL_COMMAND_ARG(arguments, 8, NULL, char *);
      char *v6_gateway    = GET_OPTIONAL_COMMAND_ARG(arguments, 9, NULL, char *);

      if ((v6_link_local == NULL) && (v6_global_ip == NULL) && (v6_gateway == NULL)) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      convert_string_to_sl_ipv6_address(v6_link_local, (uint16_t *)ip_config.ip.v6.link_local_address.bytes);
      convert_string_to_sl_ipv6_address(v6_global_ip, (uint16_t *)ip_config.ip.v6.global_address.bytes);
      convert_string_to_sl_ipv6_address(v6_gateway, (uint16_t *)ip_config.ip.v6.gateway.bytes);
    }
  }

  sl_status_t status = sl_net_configure_ip(interface, &ip_config, timeout);
  VERIFY_STATUS_AND_RETURN(status);
  PRINT_AT_CMD_SUCCESS;

  return SL_STATUS_OK;
}

static sl_status_t convert_string_to_sl_ipv6_address(char *line, uint16_t *ipv6)
{
  uint8_t double_colon = 0xFF;
  uint8_t i;
  char *lasts = NULL;
  char *token = strtok_r(line, ":", &lasts);

  for (i = 0; token != NULL; i++, token = strtok_r(NULL, ":", &lasts)) {
    ipv6[i] = (uint16_t)strtoul(token, 0, 16);
    if (lasts[0] == ':') {
      //      ipv6[i] = 0;
      // Skip the ':'
      ++lasts;
      double_colon = i;
    }
  }
  // Check if we need to move the data after the double-colon to the end
  if (i != 8 && double_colon != 0xFF) {
    --i;
    uint8_t end = 7;
    for (; i != double_colon; --i, --end) {
      ipv6[end] = ipv6[i];
    }
    for (; end > double_colon; --end) {
      ipv6[end] = 0;
    }
  }
  return SL_STATUS_OK;
}

// This is used before calling net_up as NVM3 storage isn't implemented yet to store the profile configuration.
sl_status_t set_nvm_profile_command_handler(console_args_t *arguments)
{
  sl_status_t status             = SL_STATUS_OK;
  sl_net_interface_t interface   = (sl_net_interface_t)GET_OPTIONAL_COMMAND_ARG(arguments, 0, 0, sl_net_interface_t);
  sl_net_profile_id_t profile_id = GET_OPTIONAL_COMMAND_ARG(arguments, 1, SL_NET_PROFILE_ID_0, sl_net_profile_id_t);

  if (profile_id == SL_NET_PROFILE_ID_0) {
    profile_id = (interface == SL_NET_WIFI_CLIENT_INTERFACE) ? SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID
                                                             : SL_NET_DEFAULT_WIFI_AP_PROFILE_ID;
  }

  switch (interface) {
#ifdef SL_WIFI_COMPONENT_INCLUDED
    case SL_NET_WIFI_CLIENT_INTERFACE: {
#ifdef SLI_SI91X_OFFLOAD_NETWORK_STACK
      sl_net_wifi_client_profile_t wifi_client_profile = {
          .config = {
              .ssid.value = DEFAULT_WIFI_CLIENT_PROFILE_SSID,
              .ssid.length = sizeof(DEFAULT_WIFI_CLIENT_PROFILE_SSID)-1,
              .channel.channel = SL_WIFI_AUTO_CHANNEL,
              .channel.band = SL_WIFI_AUTO_BAND,
              .channel.bandwidth = SL_WIFI_AUTO_BANDWIDTH,
              .bssid = {{0}},
              .bss_type = SL_WIFI_BSS_TYPE_INFRASTRUCTURE,
              .security = SL_WIFI_WPA_WPA2_MIXED,
              .encryption = SL_WIFI_DEFAULT_ENCRYPTION,
              .client_options = 0,
              .credential_id = SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID,
          },
          .ip = {
              .mode = SL_IP_MANAGEMENT_DHCP,
              .type = SL_IPV4,
              .host_name = NULL,
              .ip = {{{0}}},
          }
      };

      status = sl_net_set_profile(interface, profile_id, &wifi_client_profile);
      VERIFY_STATUS_AND_RETURN(status);

      status = sl_net_set_credential(SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID,
                                     SL_NET_WIFI_PSK,
                                     DEFAULT_WIFI_CLIENT_CREDENTIAL,
                                     strlen(DEFAULT_WIFI_CLIENT_CREDENTIAL));
      VERIFY_STATUS_AND_RETURN(status);
#endif
    } break;
#ifdef SLI_SI91X_OFFLOAD_NETWORK_STACK
    case SL_NET_WIFI_AP_INTERFACE: {
      sl_net_wifi_ap_profile_t wifi_ap_profile = {
    		    .config = {
    		        .ssid.value = DEFAULT_WIFI_AP_PROFILE_SSID,
    		        .ssid.length = sizeof(DEFAULT_WIFI_AP_PROFILE_SSID)-1,
                .channel.channel = SL_WIFI_AUTO_CHANNEL,
                .channel.band = SL_WIFI_AUTO_BAND,
                .channel.bandwidth = SL_WIFI_AUTO_BANDWIDTH,
                .security = SL_WIFI_WPA_WPA2_MIXED,
    		        .encryption = SL_WIFI_CCMP_ENCRYPTION,
    		        .rate_protocol = SL_WIFI_RATE_PROTOCOL_AUTO,
    		        .options = 0,
    		        .credential_id = SL_NET_DEFAULT_WIFI_AP_CREDENTIAL_ID,
    		        .keepalive_type = SL_SI91X_AP_NULL_BASED_KEEP_ALIVE,
    		        .beacon_interval = 100,
    		        .client_idle_timeout = 0xFF,
    		        .dtim_beacon_count = 3,
    		        .maximum_clients = 3,
    		        .beacon_stop = 0,
    		    },
    		    .ip = {
    		      .mode = SL_IP_MANAGEMENT_DHCP,
    		      .type = SL_IPV4,
              .host_name = NULL,
              .ip = {{{0}}},
    		    }
      };

      status = sl_net_set_profile(interface, profile_id, &wifi_ap_profile);
      VERIFY_STATUS_AND_RETURN(status);

      status = sl_net_set_credential(SL_NET_DEFAULT_WIFI_AP_CREDENTIAL_ID,
                                     SL_NET_WIFI_PSK,
                                     DEFAULT_WIFI_AP_CREDENTIAL,
                                     strlen(DEFAULT_WIFI_AP_CREDENTIAL));
      VERIFY_STATUS_AND_RETURN(status);
    } break;
#endif
#endif
    default:
      printf("Unsupported interface\n");
      return SL_STATUS_FAIL;
  }

  return status;
}

sl_status_t dns_hostgetbyname_command_handler(console_args_t *arguments)
{
#ifdef SLI_SI91X_OFFLOAD_NETWORK_STACK
  sl_ip_address_t ip_address = { 0 };
  sl_status_t status;

  const char *host_name  = (const char *)arguments->arg[0];
  const uint32_t timeout = GET_OPTIONAL_COMMAND_ARG(arguments, 1, 20000, const uint32_t);
  sl_net_dns_resolution_ip_type_t ip_type =
    GET_OPTIONAL_COMMAND_ARG(arguments, 2, SL_NET_DNS_TYPE_IPV4, sl_net_dns_resolution_ip_type_t);

  status = sl_net_dns_resolve_hostname(host_name, timeout, ip_type, &ip_address);
  if (status == SL_STATUS_IN_PROGRESS) {
    return SL_STATUS_OK;
  }
  print_sl_ip_address(&ip_address);

  VERIFY_STATUS_AND_RETURN(status);
  return status;
#else
  return SL_STATUS_NOT_SUPPORTED;
#endif
}

#ifdef SLI_SI91X_OFFLOAD_NETWORK_STACK
sl_status_t ping_response_callback_handler(sl_net_event_t event, sl_status_t status, void *data, uint32_t user_data)
{
  UNUSED_PARAMETER(user_data);
  sl_si91x_ping_response_t *response = (sl_si91x_ping_response_t *)data;

  if (SL_NET_PING_RESPONSE_EVENT == event) {
    if (status != SL_STATUS_OK) {
      printf("\n Ping request failed! \n");
      return status;
    } else {
      printf(" Ping reply received from %u.%u.%u.%u \n\n",
             response->ping_address.ipv4_address[0],
             response->ping_address.ipv4_address[1],
             response->ping_address.ipv4_address[2],
             response->ping_address.ipv4_address[3]);
    }
  }

  return status;
}

sl_status_t sl_net_ping_command_handler(console_args_t *arguments)
{
  sl_status_t status         = SL_STATUS_OK;
  sl_ip_address_t ip_address = { .type = SL_IPV4 };

  sl_net_inet_addr((char *)arguments->arg[0], (uint32_t *)&ip_address.ip.v4);
  ip_address.type    = (sl_ip_address_type_t)GET_OPTIONAL_COMMAND_ARG(arguments, 1, SL_IPV4, sl_ip_address_type_t);
  uint16_t ping_size = (uint16_t)arguments->arg[2];

  status = sl_si91x_send_ping(ip_address, ping_size);

  if (SL_STATUS_IN_PROGRESS == status) {
    status = SL_STATUS_OK;
  }

  return status;
}

sl_status_t http_otaf_upgrade_command_handler(const console_args_t *arguments)
{

  sl_status_t status                      = SL_STATUS_OK;
  sl_si91x_http_otaf_params_t http_params = {
    .flags           = GET_OPTIONAL_COMMAND_ARG(arguments, 0, 0, uint16_t),
    .ip_address      = (uint8_t *)GET_OPTIONAL_COMMAND_ARG(arguments, 1, NULL, char *),
    .port            = GET_OPTIONAL_COMMAND_ARG(arguments, 2, OTAF_DEFAULT_PORT, uint16_t),
    .resource        = (uint8_t *)GET_OPTIONAL_COMMAND_ARG(arguments, 3, NULL, char *),
    .host_name       = (uint8_t *)GET_OPTIONAL_COMMAND_ARG(arguments, 4, NULL, char *),
    .extended_header = (uint8_t *)GET_OPTIONAL_COMMAND_ARG(arguments, 5, NULL, char *),
    .user_name       = (uint8_t *)GET_OPTIONAL_COMMAND_ARG(arguments, 6, NULL, char *),
    .password        = (uint8_t *)GET_OPTIONAL_COMMAND_ARG(arguments, 7, NULL, char *),
  };

  status = sl_si91x_http_otaf_v2(&http_params);
  VERIFY_STATUS_AND_RETURN(status);
  PRINT_AT_CMD_SUCCESS;

  return status;
}

sl_status_t start_dhcp_command_handler(console_args_t *arguments)
{
  sl_status_t status                   = SL_STATUS_OK;
  sl_net_ip_configuration_t ip_address = { 0 };
  sl_ip_address_type_t ip_version =
    (sl_ip_address_type_t)GET_OPTIONAL_COMMAND_ARG(arguments, 0, SL_IPV4, sl_ip_address_type_t);
  sl_ip_management_t ip_mode =
    (sl_ip_management_t)GET_OPTIONAL_COMMAND_ARG(arguments, 1, SL_IP_MANAGEMENT_DHCP, sl_ip_management_t);

  if (ip_version == SL_IPV4) {
    ip_address.type = SL_IPV4;
  } else if (ip_version == SL_IPV6) {
    ip_address.type = SL_IPV6;
  } else {
    return SL_STATUS_INVALID_PARAMETER;
  }

  ip_address.mode = (ip_mode == SL_IP_MANAGEMENT_DHCP) ? SL_IP_MANAGEMENT_DHCP : SL_IP_MANAGEMENT_STATIC_IP;

  status = sl_si91x_configure_ip_address(&ip_address, SL_SI91X_WIFI_CLIENT_VAP_ID);
  VERIFY_STATUS_AND_RETURN(status);
  if (ip_address.type == SL_IPV4) {
    {
      sl_ip_address_t temp;
      temp.type  = ip_address.type;
      temp.ip.v4 = ip_address.ip.v4.ip_address;
      print_sl_ip_address(&temp);
    }
  } else if (ip_address.type == SL_IPV6) {
    sl_ip_address_t link_local_address;
    memcpy(&link_local_address.ip.v6, &ip_address.ip.v6.link_local_address, SL_IPV6_ADDRESS_LENGTH);
    link_local_address.type = SL_IPV6;
    printf("Link Local Address: ");
    print_sl_ip_address(&link_local_address);

    sl_ip_address_t global_address;
    memcpy(&global_address.ip.v6, &ip_address.ip.v6.global_address, SL_IPV6_ADDRESS_LENGTH);
    global_address.type = SL_IPV6;
    printf("Global Address: ");
    print_sl_ip_address(&global_address);

    sl_ip_address_t gateway;
    gateway.ip.v6 = ip_address.ip.v6.gateway;
    memcpy(&gateway.ip.v6, &ip_address.ip.v6.gateway, SL_IPV6_ADDRESS_LENGTH);
    gateway.type = SL_IPV6;
    printf("Gateway Address: ");
    print_sl_ip_address(&gateway);
  }

  return status;
}
#elif SLI_SI91X_LWIP_HOSTED_NETWORK_STACK
sl_status_t sl_net_ping_command_handler(console_args_t *arguments)
{
  return SL_STATUS_NOT_SUPPORTED;
}
sl_status_t start_dhcp_command_handler(console_args_t *arguments)
{
  return SL_STATUS_NOT_SUPPORTED;
}
#endif

sl_status_t net_join_multicast_address_command_handler(console_args_t *arguments)
{
  sl_ip_address_t ip_address = { 0 };

  sl_net_inet_addr((char *)arguments->arg[1], (uint32_t *)&ip_address.ip.v4);
  ip_address.type = SL_IPV4;
#ifdef SLI_SI91X_OFFLOAD_NETWORK_STACK
  sl_net_interface_t interface =
    GET_OPTIONAL_COMMAND_ARG(arguments, 0, SL_NET_WIFI_CLIENT_INTERFACE, sl_net_interface_t);
  return sl_net_join_multicast_address(interface, &ip_address);
#else
  return SL_STATUS_NOT_SUPPORTED;
#endif
}

sl_status_t net_leave_multicast_address_command_handler(console_args_t *arguments)
{
  sl_ip_address_t ip_address = { 0 };

  sl_net_inet_addr((char *)arguments->arg[1], (uint32_t *)&ip_address.ip.v4);
  ip_address.type = SL_IPV4;
#ifdef SLI_SI91X_OFFLOAD_NETWORK_STACK
  sl_net_interface_t interface =
    GET_OPTIONAL_COMMAND_ARG(arguments, 0, SL_NET_WIFI_CLIENT_INTERFACE, sl_net_interface_t);
  return sl_net_leave_multicast_address(interface, &ip_address);
#else
  return SL_STATUS_NOT_SUPPORTED;
#endif
}
