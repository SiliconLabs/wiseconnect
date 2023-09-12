#include "sl_board_configuration.h"
#include "console.h"
#include "sl_net.h"
#include "sl_net_dns.h"
#include "sl_utility.h"
#include "sl_wifi.h"
#include "sl_net_default_values.h"
#include "sl_si91x_driver.h"
#include <string.h>

/******************************************************
 *                    Constants
 ******************************************************/

#ifdef si91x_internal_stack_FEATURE_REQUIRED
#define IP_ADDRESS "192.168.10.2"
#define GATEWAY    "192.168.10.2"
#define NETMASK    "255.255.255.0"
#define AP_PSK     "123456789"
#endif

#ifdef ethernet_FEATURE_REQUIRED
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

#ifdef wifi_FEATURE_REQUIRED
#include "sl_net_wifi_types.h"
#include "sl_wifi_callback_framework.h"

#ifdef si91x_lwip_stack_FEATURE_REQUIRED
#include "sl_net_for_lwip.h"
static sl_net_wifi_lwip_context_t wifi_client_context;
static sl_net_wifi_lwip_context_t wifi_ap_context;
#elif si91x_internal_stack_FEATURE_REQUIRED
#include "sl_net_si91x.h"
#include "sl_wifi_callback_framework.h"
#include "sl_net_ping.h"
sl_wifi_context_t wifi_client_context;
sl_wifi_context_t wifi_ap_context;
#endif

#endif

sl_status_t net_init_command_handler(console_args_t *arguments)
{
  sl_status_t status;
  sl_net_interface_t interface = (sl_net_interface_t)GET_COMMAND_ARG(arguments, 0);

  switch (SL_NET_INTERFACE_TYPE(interface)) {
#ifdef ethernet_FEATURE_REQUIRED
    case SL_NET_ETHERNET_INTERFACE:
      status = sl_net_init(SL_NET_ETHERNET_INTERFACE, &ethernet_config, &ethernet_context, 0);
      VERIFY_STATUS_AND_RETURN(status);
      break;
#endif

#ifdef wifi_FEATURE_REQUIRED
    case SL_NET_WIFI_CLIENT_INTERFACE:
      status = sl_net_init(interface, &sl_wifi_default_client_configuration, &wifi_client_context, NULL);
      VERIFY_STATUS_AND_RETURN(status);
      break;

    case SL_NET_WIFI_AP_INTERFACE:
      status = sl_net_init(interface,
                           (const void *)&sl_wifi_default_ap_configuration,
                           (void *)&wifi_ap_context,
                           (sl_net_event_handler_t)default_wifi_event_handler);
      VERIFY_STATUS_AND_RETURN(status);
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
#ifdef wifi_FEATURE_REQUIRED
    case SL_NET_WIFI_CLIENT_INTERFACE:
      status = sl_net_wifi_client_deinit(interface, &wifi_client_context);
      VERIFY_STATUS_AND_RETURN(status);
      break;

    case SL_NET_WIFI_AP_INTERFACE:
      status = sl_net_wifi_ap_deinit(interface, &wifi_ap_context);
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
#ifdef wifi_FEATURE_REQUIRED
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
#endif
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
#ifdef wifi_FEATURE_REQUIRED
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
      printf("Unsupported interface\n");
      return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

// This is used before calling net_up as NVM3 storage isn't implemented yet to store the profile configuration.
sl_status_t set_nvm_profile_command_handler(console_args_t *arguments)
{
  sl_status_t status             = SL_STATUS_OK;
  sl_net_interface_t interface   = (sl_net_interface_t)GET_COMMAND_ARG(arguments, 0);
  sl_net_profile_id_t profile_id = GET_OPTIONAL_COMMAND_ARG(arguments, 1, SL_NET_PROFILE_ID_0, sl_net_profile_id_t);

  if (profile_id == SL_NET_PROFILE_ID_0) {
    profile_id = (interface == SL_NET_WIFI_CLIENT_INTERFACE) ? SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID
                                                             : SL_NET_DEFAULT_WIFI_AP_PROFILE_ID;
  }

  switch (interface) {
#ifdef wifi_FEATURE_REQUIRED
    case SL_NET_WIFI_CLIENT_INTERFACE: {
#ifdef si91x_internal_stack_FEATURE_REQUIRED
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
#ifdef si91x_internal_stack_FEATURE_REQUIRED
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
    		        .keepalive_type = SI91X_AP_NULL_BASED_KEEP_ALIVE,
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
#endif
    default:
      printf("Unsupported interface\n");
      return SL_STATUS_FAIL;
  }

  return status;
}

sl_status_t sl_dns_hostgetbyname_command_handler(console_args_t *arguments)
{
#ifdef si91x_internal_stack_FEATURE_REQUIRED
  sl_ip_address_t ip_address = { 0 };
  sl_status_t status;

  const char *host_name  = (const char *)arguments->arg[0];
  const uint32_t timeout = GET_OPTIONAL_COMMAND_ARG(arguments, 1, 20000, const uint32_t);
  sl_net_dns_resolution_ip_type_t ip_type =
    GET_OPTIONAL_COMMAND_ARG(arguments, 2, SL_NET_DNS_TYPE_IPV4, sl_net_dns_resolution_ip_type_t);

  status = sl_dns_host_get_by_name(host_name, timeout, ip_type, &ip_address);
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

#ifdef si91x_internal_stack_FEATURE_REQUIRED
sl_status_t ping_response_callback_handler(uint32_t event, sl_status_t status, void *data, uint32_t user_data)
{
  UNUSED_PARAMETER(event);
  UNUSED_PARAMETER(user_data);
  sl_si91x_ping_response_t *response = (sl_si91x_ping_response_t *)data;
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

  return status;
}

sl_status_t sl_net_ping_command_handler(console_args_t *arguments)
{
  sl_status_t status         = SL_STATUS_OK;
  sl_ip_address_t ip_address = { .type = SL_IPV4 };

  sl_net_inet_addr((char *)arguments->arg[0], (uint32_t *)&ip_address.ip.v4);
  ip_address.type    = (uint16_t)arguments->arg[1];
  uint16_t ping_size = (uint16_t)arguments->arg[2];

  sl_si91x_register_callback(SL_NET_PING_RESPONSE_EVENT, (sl_net_event_handler_t)ping_response_callback_handler);

  status = sl_si91x_send_ping(ip_address, ping_size);

  if (SL_STATUS_IN_PROGRESS == status) {
    status = SL_STATUS_OK;
  }

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

  status = sl_si91x_configure_ip_address(&ip_address, CLIENT_MODE);
  VERIFY_STATUS_AND_RETURN(status);
  if (ip_address.type == SL_IPV4) {
    print_sl_ip_address((sl_ip_address_t *)&(ip_address.ip.v4.ip_address));
  } else if (ip_address.type == SL_IPV6) {
    sl_ip_address_t link_local_address;
    link_local_address.ip.v6 = ip_address.ip.v6.link_local_address;
    link_local_address.type  = SL_IPV6;
    printf("Link Local Address: ");
    print_sl_ip_address(&link_local_address);

    sl_ip_address_t global_address;
    global_address.ip.v6 = ip_address.ip.v6.global_address;
    global_address.type  = SL_IPV6;
    printf("Global Address: ");
    print_sl_ip_address(&global_address);

    sl_ip_address_t gateway;
    gateway.ip.v6 = ip_address.ip.v6.gateway;
    gateway.type  = SL_IPV6;
    printf("Gateway Address: ");
    print_sl_ip_address(&gateway);
  }

  return status;
}
#elif si91x_lwip_stack_FEATURE_REQUIRED
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
#ifdef si91x_internal_stack_FEATURE_REQUIRED
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
#ifdef si91x_internal_stack_FEATURE_REQUIRED
  sl_net_interface_t interface =
    GET_OPTIONAL_COMMAND_ARG(arguments, 0, SL_NET_WIFI_CLIENT_INTERFACE, sl_net_interface_t);
  return sl_net_leave_multicast_address(interface, &ip_address);
#else
  return SL_STATUS_NOT_SUPPORTED;
#endif
}
