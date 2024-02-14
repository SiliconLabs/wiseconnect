#include "sl_board_configuration.h"
#include "console.h"
#include "sl_net.h"
#include "sl_net_dns.h"
#include "sl_si91x_driver.h"
#include "sl_utility.h"
#include "sl_wifi.h"
#include "sl_net_default_values.h"
#include "printf.h"
#include <string.h>
#include "command_identifiers.h"

/******************************************************
 *               Static Function Declarations
 ******************************************************/
#ifdef SLI_SI91X_OFFLOAD_NETWORK_STACK
static sl_status_t ping_callback_handler(sl_net_event_t event, sl_status_t status, void *data, uint32_t user_data);
#endif

/******************************************************
 *                    Constants
 ******************************************************/

#ifdef SLI_SI91X_OFFLOAD_NETWORK_STACK
#define IP_ADDRESS "192.168.10.2"
#define GATEWAY    "192.168.10.2"
#define NETMASK    "255.255.255.0"
#define AP_PSK     "123456789"
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
#endif

static sl_status_t ap_connected_event_handler(sl_wifi_event_t event, void *data, uint32_t data_length, void *arg);
static sl_status_t ap_disconnected_event_handler(sl_wifi_event_t event, void *data, uint32_t data_length, void *arg);

#endif

sl_status_t net_init_command_handler(console_args_t *arguments)
{
  sl_status_t status;
  sl_net_interface_t interface         = (sl_net_interface_t)GET_COMMAND_ARG(arguments, 0);
  sl_net_event_handler_t event_handler = NULL;

  switch (SL_NET_INTERFACE_TYPE(interface)) {
#ifdef SLI_SI91X_LWIP_NETWORK_INTERFACE
    case SL_NET_DEFAULT_ETHERNET_INTERFACE:
      status = sl_net_init(SL_NET_DEFAULT_ETHERNET_INTERFACE, &ethernet_config, &ethernet_context, 0);
      VERIFY_STATUS_AND_RETURN(status);
      break;
#endif

#ifdef SL_WIFI_COMPONENT_INCLUDED
    case SL_NET_WIFI_CLIENT_INTERFACE:
#ifdef SLI_SI91X_OFFLOAD_NETWORK_STACK
      event_handler = ping_callback_handler;
      status        = sl_net_init(interface, &sl_wifi_default_client_configuration, NULL, event_handler);
#elif SLI_SI91X_LWIP_HOSTED_NETWORK_STACK
      status = sl_net_init(interface, &sl_wifi_default_client_configuration, &wifi_client_context, event_handler);
#endif
      VERIFY_STATUS_AND_RETURN(status);
      break;

    case SL_NET_WIFI_AP_INTERFACE:
      status = sl_net_init(interface, NULL, NULL, NULL);
      VERIFY_STATUS_AND_RETURN(status);

      sl_wifi_set_callback(SL_WIFI_CLIENT_CONNECTED_EVENTS, ap_connected_event_handler, NULL);
      sl_wifi_set_callback(SL_WIFI_CLIENT_DISCONNECTED_EVENTS, ap_disconnected_event_handler, NULL);
      break;
#endif
    default:
      printf("Unsupported interface\n");
      return SL_STATUS_FAIL;
  }

  return status;
}

sl_status_t net_deinit_command_handler(console_args_t *arguments)
{
  sl_status_t status;
  sl_net_interface_t interface = (sl_net_interface_t)GET_COMMAND_ARG(arguments, 0);

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
      printf("Unsupported interface\n");
      return SL_STATUS_FAIL;
  }
  return status;
}

sl_status_t net_up_command_handler(console_args_t *arguments)
{
  sl_status_t status;
  sl_net_interface_t interface   = (sl_net_interface_t)GET_COMMAND_ARG(arguments, 0);
  sl_net_profile_id_t profile_id = GET_OPTIONAL_COMMAND_ARG(arguments, 1, SL_NET_PROFILE_ID_0, sl_net_profile_id_t);

  switch (SL_NET_INTERFACE_TYPE(interface)) {
    case SL_NET_WIFI_CLIENT_INTERFACE: {
      sl_net_wifi_client_profile_t profile;

      // Fetch the profile and print some information about it
      status = sl_net_get_profile(SL_NET_WIFI_CLIENT_INTERFACE, profile_id, &profile);
      if (status != SL_STATUS_OK) {
        printf("Failed to load profile with id: %u\r\n", profile_id);
        return status;
      }
      printf("Connecting to '%s'\r\n", profile.config.ssid.value);
      status = sl_net_wifi_client_up(interface, profile_id);
      VERIFY_STATUS_AND_RETURN(status);
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

    default:
      printf("Unsupported interface\n");
      return SL_STATUS_FAIL;
  }
  return status;
}

sl_status_t net_down_command_handler(console_args_t *arguments)
{
  sl_status_t status;
  sl_net_interface_t interface = (sl_net_interface_t)GET_COMMAND_ARG(arguments, 0);

  switch (SL_NET_INTERFACE_TYPE(interface)) {
    case SL_NET_WIFI_CLIENT_INTERFACE:
      status = sl_net_wifi_client_down(interface);
      VERIFY_STATUS_AND_RETURN(status);
      break;

    case SL_NET_WIFI_AP_INTERFACE:
      status = sl_net_wifi_ap_down(interface);
      VERIFY_STATUS_AND_RETURN(status);
      break;

    default:
      printf("Unsupported interface\n");
      return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

// This is used before calling net_up as NVM3 storage isn't implemented yet to store the profile configuration.
sl_status_t set_nvm_profile_command_handler(console_args_t *arguments)
{
  sl_status_t status;
  sl_net_interface_t interface   = (sl_net_interface_t)GET_COMMAND_ARG(arguments, 0);
  sl_net_profile_id_t profile_id = GET_OPTIONAL_COMMAND_ARG(arguments, 1, SL_NET_PROFILE_ID_0, sl_net_profile_id_t);

  if (profile_id == SL_NET_PROFILE_ID_0) {
    profile_id = (interface == SL_NET_WIFI_CLIENT_INTERFACE) ? SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID
                                                             : SL_NET_DEFAULT_WIFI_AP_PROFILE_ID;
  }

  switch (interface) {
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
              .encryption = SL_WIFI_CCMP_ENCRYPTION,
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
    		        .channel = {0,0,0},
    		        .rate_protocol = SL_WIFI_RATE_PROTOCOL_AUTO,
    		        .options = 0,
    		        .credential_id = SL_NET_DEFAULT_WIFI_AP_CREDENTIAL_ID,
    		        .keepalive_type = SL_SI91X_AP_NULL_BASED_KEEP_ALIVE,
    		        .beacon_interval = 100,
    		        .client_idle_timeout = 0xFF,
    		        .dtim_beacon_count = 3,
    		        .maximum_clients = 3,
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
    default:
      printf("Unsupported interface\n");
      return SL_STATUS_FAIL;
  }

  return status;
}

sl_status_t sl_dns_hostgetbyname_command_handler(console_args_t *arguments)
{
  sl_ip_address_t ip_address = { 0 };
  sl_status_t status;

  const char *host_name  = (const char *)arguments->arg[0];
  const uint32_t timeout = GET_OPTIONAL_COMMAND_ARG(arguments, 1, 20000, const uint32_t);
  sl_net_dns_resolution_ip_type_t ip_type =
    GET_OPTIONAL_COMMAND_ARG(arguments, 2, SL_NET_DNS_TYPE_IPV4, sl_net_dns_resolution_ip_type_t);

#ifdef SLI_SI91X_OFFLOAD_NETWORK_STACK
  status = sl_net_host_get_by_name(host_name, timeout, ip_type, &ip_address);
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
static sl_status_t ping_callback_handler(sl_net_event_t event, sl_status_t status, void *data, uint32_t user_data)
{
  UNUSED_PARAMETER(user_data);

  switch (event) {
    case SL_NET_PING_RESPONSE_EVENT: {
      sl_ip_address_t remote_ip_address  = { 0 };
      sl_si91x_ping_response_t *response = (sl_si91x_ping_response_t *)data;
      if (status != SL_STATUS_OK) {
        printf("\r\nPing request failed with status 0x%lX\r\n", status);
        return status;
      }
      if (response->ip_version == SL_IPV4_VERSION) {
        printf("\r\n%u bytes received from %u.%u.%u.%u\r\n",
               response->ping_size,
               response->ping_address.ipv4_address[0],
               response->ping_address.ipv4_address[1],
               response->ping_address.ipv4_address[2],
               response->ping_address.ipv4_address[3]);
      } else if (response->ip_version == SL_IPV6_VERSION) {
        memcpy(&remote_ip_address.ip.v6.bytes, &(response->ping_address.ipv6_address), SL_IPV6_ADDRESS_LENGTH);
        remote_ip_address.type = SL_IPV6;
        printf("\r\n%u bytes received from: ", response->ping_size);
        print_sl_ip_address(&remote_ip_address);
      }
      break;
    }
    default:
      break;
  }

  return SL_STATUS_OK;
}

sl_status_t sl_net_ping_command_handler(console_args_t *arguments)
{
  sl_status_t status         = SL_STATUS_OK;
  sl_ip_address_t ip_address = { .type = (uint16_t)arguments->arg[0] };
  uint8_t address_buffer[SL_IPV6_ADDRESS_LENGTH];
  sl_si91x_register_event_handler(ping_callback_handler);

  if (ip_address.type == SL_IPV4) {
    convert_string_to_sl_ipv4_address((char *)arguments->arg[1], &ip_address.ip.v4);
  } else {
    status = sl_inet_pton6((char *)arguments->arg[1],
                           (char *)arguments->arg[1] + strlen((char *)arguments->arg[1]),
                           address_buffer,
                           (unsigned int *)ip_address.ip.v6.value);
  }
  uint16_t ping_size = (uint16_t)arguments->arg[2];

  status = sl_si91x_send_ping(ip_address, ping_size);

  if (SL_STATUS_IN_PROGRESS == status) {
    status = SL_STATUS_OK;
  }

  return status;
}

sl_status_t configure_ip_command_handler(console_args_t *arguments)
{
  sl_status_t status;
  uint8_t virtual_ap_id                = 0;
  sl_net_ip_configuration_t ip_address = {
    .type = (uint16_t)arguments->arg[0],
    .mode =
      (IS_CONSOLE_ARG_VALID(arguments, 0) && IS_CONSOLE_ARG_VALID(arguments, 1) && IS_CONSOLE_ARG_VALID(arguments, 2))
        ? SL_IP_MANAGEMENT_STATIC_IP
        : SL_IP_MANAGEMENT_DHCP,
    .host_name              = NULL,
    .ip.v4.ip_address.value = GET_OPTIONAL_COMMAND_ARG(arguments, 1, 0, uint32_t),
    .ip.v4.gateway.value    = GET_OPTIONAL_COMMAND_ARG(arguments, 2, 0, uint32_t),
    .ip.v4.netmask.value    = GET_OPTIONAL_COMMAND_ARG(arguments, 3, 0, uint32_t),
  };
  virtual_ap_id = GET_OPTIONAL_COMMAND_ARG(arguments, 4, 0, uint32_t);

  status = sl_si91x_configure_ip_address(&ip_address, virtual_ap_id);
  VERIFY_STATUS_AND_RETURN(status);
  if (ip_address.type == SL_IPV4) {
    print_sl_ipv4_address(&(ip_address.ip.v4.ip_address));
    printf(", ");
    print_sl_ipv4_address(&(ip_address.ip.v4.netmask));
    printf(", ");
    print_sl_ipv4_address(&(ip_address.ip.v4.gateway));
  } else {
    print_sl_ipv6_address(&(ip_address.ip.v6.link_local_address));
    printf(", ");
    print_sl_ipv6_address(&(ip_address.ip.v6.global_address));
    printf(", ");
    print_sl_ipv6_address(&(ip_address.ip.v6.gateway));
  }
  return status;
}
#else
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
  sl_net_interface_t interface =
    GET_OPTIONAL_COMMAND_ARG(arguments, 0, SL_NET_WIFI_CLIENT_INTERFACE, sl_net_interface_t);

  convert_string_to_sl_ipv4_address((char *)arguments->arg[1], &ip_address.ip.v4);
  ip_address.type = SL_IPV4;
#ifdef SLI_SI91X_OFFLOAD_NETWORK_STACK
  return sl_net_join_multicast_address(interface, &ip_address);
#else
  return SL_STATUS_NOT_SUPPORTED;
#endif
}

sl_status_t net_leave_multicast_address_command_handler(console_args_t *arguments)
{
  sl_ip_address_t ip_address = { 0 };
  sl_net_interface_t interface =
    GET_OPTIONAL_COMMAND_ARG(arguments, 0, SL_NET_WIFI_CLIENT_INTERFACE, sl_net_interface_t);

  convert_string_to_sl_ipv4_address((char *)arguments->arg[1], &ip_address.ip.v4);
  ip_address.type = SL_IPV4;
#ifdef SLI_SI91X_OFFLOAD_NETWORK_STACK
  return sl_net_leave_multicast_address(interface, &ip_address);
#else
  return SL_STATUS_NOT_SUPPORTED;
#endif
}

static sl_status_t ap_connected_event_handler(sl_wifi_event_t event, void *data, uint32_t data_length, void *arg)
{
  UNUSED_PARAMETER(data_length);
  UNUSED_PARAMETER(arg);
  PRINT_EVENT_START(event, WIFI)

  printf("Client connected: ");
  print_mac_address((sl_mac_address_t *)data);
  printf("\n");

  PRINT_EVENT_END();
  return SL_STATUS_OK;
}

static sl_status_t ap_disconnected_event_handler(sl_wifi_event_t event, void *data, uint32_t data_length, void *arg)
{
  UNUSED_PARAMETER(data_length);
  UNUSED_PARAMETER(arg);
  PRINT_EVENT_START(event, WIFI)

  printf("Client disconnected: ");
  print_mac_address((sl_mac_address_t *)data);
  printf("\n");

  PRINT_EVENT_END();
  return SL_STATUS_OK;
}
