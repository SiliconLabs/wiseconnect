/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */
#include "sli_si3xx_lwip_interface.h"
#include "sl_status.h"
#include "lwip/tcpip.h"
#include "lwip/netif.h"
#include "lwip/netifapi.h"
#include "lwip/ip_addr.h"
#include "lwip/ip6_addr.h"
#include "sl_wifi.h"
#include "sl_utility.h"
#include "sl_wifi_callback_framework.h"
#include "sl_wlan_types.h"
#include <string.h>
#include "sl_net_constants.h"
#include "sl_net_types.h"
#include "sl_net_wifi_types.h"
#include "sl_net.h"
#include "sli_net_common_utility.h"

#define SLI_WIFI_CONNECT_TIMEOUT_MS 18000

sli_wlan_lwip_context_t *wifi_client_context = NULL;
sli_wlan_lwip_context_t *wifi_ap_context     = NULL;

static bool lwip_initialized                = false;
static sl_wifi_interface_t ap_interface     = SL_WIFI_INVALID_INTERFACE;
static sl_net_profile_id_t ap_profile_id    = SL_NET_DEFAULT_WIFI_AP_PROFILE_ID;
static sl_wifi_interface_t client_interface = SL_WIFI_INVALID_INTERFACE;

/** Copy lwIP netif IP addresses into @a ip (IPv4 / IPv6 per lwIP build options). */
static void sli_copy_lwip_netif_to_ip_configuration(const struct netif *netif, sl_net_ip_configuration_t *ip)
{
#if LWIP_IPV4 && LWIP_IPV6
  ip_addr_t *addr;
  addr = &netif->ip_addr;
  if (addr->type == IPADDR_TYPE_V4) {
    memcpy(ip->ip.v4.ip_address.bytes, &addr->u_addr.ip4.addr, sizeof(addr->u_addr.ip4.addr));
  } else if (addr->type == IPADDR_TYPE_V6) {
    for (int i = 0; i < 4; i++) {
      ip->ip.v6.link_local_address.value[i] = ntohl(netif->ip6_addr[0].u_addr.ip6.addr[i]);
    }
  }
  addr = &netif->gw;
  if (addr->type == IPADDR_TYPE_V4) {
    memcpy(ip->ip.v4.gateway.bytes, &addr->u_addr.ip4.addr, sizeof(addr->u_addr.ip4.addr));
  } else if (addr->type == IPADDR_TYPE_V6) {
    for (int i = 0; i < 4; i++) {
      ip->ip.v6.global_address.value[i] = ntohl(netif->ip6_addr[1].u_addr.ip6.addr[i]);
    }
  }
  addr = &netif->netmask;
  if (addr->type == IPADDR_TYPE_V4) {
    memcpy(ip->ip.v4.netmask.bytes, &addr->u_addr.ip4.addr, sizeof(addr->u_addr.ip4.addr));
  } else if (addr->type == IPADDR_TYPE_V6) {
    for (int i = 0; i < 4; i++) {
      ip->ip.v6.gateway.value[i] = ntohl(netif->ip6_addr[2].u_addr.ip6.addr[i]);
    }
  }
#else /* LWIP_IPV4 && LWIP_IPV6 */
#if LWIP_IPV4
  const u32_t *addr;
  addr = &netif->ip_addr.addr;
  memcpy(ip->ip.v4.ip_address.bytes, addr, sizeof(*addr));

  addr = &netif->gw.addr;
  memcpy(ip->ip.v4.gateway.bytes, addr, sizeof(*addr));

  addr = &netif->netmask.addr;
  memcpy(ip->ip.v4.netmask.bytes, addr, sizeof(*addr));
#elif LWIP_IPV6
  for (int i = 0; i < 4; i++) {
    ip->ip.v6.link_local_address.value[i] = lwip_htonl(netif->ip6_addr[0].addr[i]);
    ip->ip.v6.global_address.value[i]     = lwip_htonl(netif->ip6_addr[1].addr[i]);
    ip->ip.v6.gateway.value[i]            = lwip_htonl(netif->ip6_addr[2].addr[i]);
  }
#endif /* LWIP_IPV6 */
#endif /* LWIP_IPV4 && LWIP_IPV6 */
}

sl_status_t sl_net_wifi_client_init(sl_net_interface_t interface,
                                    const void *configuration,
                                    void *context,
                                    sl_net_event_handler_t event_handler)
{
  UNUSED_PARAMETER(event_handler);
  if (!((interface == SL_NET_WIFI_CLIENT_1_INTERFACE) || (interface == SL_NET_WIFI_CLIENT_2_INTERFACE))) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (context == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (event_handler != NULL) {
    sli_net_register_event_handler(event_handler);
  }
  sl_status_t status = SL_STATUS_FAIL;
  status             = sl_wifi_init(configuration, NULL, sl_wifi_default_event_handler);
  if (status != SL_STATUS_OK) {
    return status;
  }
  // Set the context for the Wi-Fi client
  wifi_client_context = (sli_wlan_lwip_context_t *)context;

  if (!lwip_initialized) {
    tcpip_init(NULL, NULL);
    lwip_initialized = true;
  }
  // Configure the station network interface
  sli_sta_netif_config();
  return SL_STATUS_OK;
}

sl_status_t sl_net_wifi_client_up(sl_net_interface_t interface, sl_net_profile_id_t profile_id)
{
  //Checking for client interface
  if (!((interface == SL_NET_WIFI_CLIENT_1_INTERFACE) || (interface == SL_NET_WIFI_CLIENT_2_INTERFACE))) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_status_t status = SL_STATUS_FAIL;

  // Connect to the Wi-Fi network
  if (profile_id == SL_NET_AUTO_JOIN) {
    return sli_network_manager_auto_join_request(interface, profile_id);
  }

  // Load profile and connect here
  sl_net_wifi_client_profile_t profile = { 0 };

  status = sl_net_get_profile(interface, profile_id, &profile);
  VERIFY_STATUS_AND_RETURN(status);
  // Configure the client interface based on the band
  if (profile.config.channel.band == SL_WIFI_BAND_2_4GHZ || profile.config.channel.band == SL_WIFI_AUTO_BAND) {
    client_interface = SL_WIFI_CLIENT_2_4GHZ_INTERFACE;
  } else if (profile.config.channel.band == SL_WIFI_BAND_5GHZ) {
    client_interface = SL_WIFI_CLIENT_5GHZ_INTERFACE;
  } else if (profile.config.channel.band == SL_WIFI_BAND_DUAL) {
    client_interface = SL_WIFI_CLIENT_DUAL_INTERFACE;
  } else {
    return SL_STATUS_INVALID_PARAMETER;
  }

  status = sl_wifi_connect(client_interface, &profile.config, SLI_WIFI_CONNECT_TIMEOUT_MS);
  VERIFY_STATUS_AND_RETURN(status);

  status = set_sta_link_up(&profile);
  if (status != SL_STATUS_OK) {
    // Disconnect WiFi on IP configuration failure
    sl_status_t disconnect_status = sl_wifi_disconnect(client_interface);
    if (disconnect_status != SL_STATUS_OK) {
      SL_DEBUG_LOG("WiFi disconnect failed: 0x%lx\n", disconnect_status);
    }
    return status;
  }
  sli_copy_lwip_netif_to_ip_configuration(&wifi_client_context->netif, &profile.ip);
  // Set the client profile
  status = sl_net_set_profile(interface, profile_id, &profile);
  VERIFY_STATUS_AND_RETURN(status);
  return SL_STATUS_OK;
}

sl_status_t sl_net_wifi_client_down(sl_net_interface_t interface)
{
  //Checking for client interface
  if (!((interface == SL_NET_WIFI_CLIENT_1_INTERFACE) || (interface == SL_NET_WIFI_CLIENT_2_INTERFACE))) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  set_sta_link_down();
  return sl_wifi_disconnect(client_interface);
}

sl_status_t sl_net_wifi_client_deinit(sl_net_interface_t interface)
{
  sl_status_t status = SL_STATUS_FAIL;
  //Checking for client interface
  if (!((interface == SL_NET_WIFI_CLIENT_1_INTERFACE) || (interface == SL_NET_WIFI_CLIENT_2_INTERFACE))) {
    return SL_STATUS_INVALID_PARAMETER;
  }

#if LWIP_TESTMODE
  struct sys_timeo **list_head = NULL;

  //! Free all timers
  for (int i = 0; i < lwip_num_cyclic_timers; i++) {
    list_head = sys_timeouts_get_next_timeout();
    if (list_head != NULL && *list_head != NULL)
      sys_untimeout((*list_head)->h, (*list_head)->arg);
  }
#endif

  // Clean up netif
  if (wifi_client_context != NULL) {
    netif_remove(&(wifi_client_context->netif));
    wifi_client_context = NULL;
  }
  status = sl_wifi_deinit();
  VERIFY_STATUS_AND_RETURN(status);
  return SL_STATUS_OK;
}

sl_status_t sl_net_wifi_ap_init(sl_net_interface_t interface,
                                const void *configuration,
                                const void *workspace,
                                sl_net_event_handler_t event_handler)
{ //Checking for AP interface
  if (!((interface == SL_NET_WIFI_AP_1_INTERFACE) || (interface == SL_NET_WIFI_AP_2_INTERFACE))) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (workspace == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_status_t status = SL_STATUS_FAIL;
  //Set the user-defined event handler for AP mode
  sli_net_register_event_handler(event_handler);

  status = sl_wifi_init(configuration, NULL, sl_wifi_default_event_handler);
  if (status != SL_STATUS_OK) {
    return status;
  }

  //Setting the wifi ap context (workspace is mutable storage for LwIP netif)
  wifi_ap_context = (sli_wlan_lwip_context_t *)(uintptr_t)workspace;

  // Initialize the LwIP stack
  if (!lwip_initialized) {
    tcpip_init(NULL, NULL);
    lwip_initialized = true;
  }
  // Initializes the AP netif
  sli_ap_netif_config();
  return SL_STATUS_OK;
}

sl_status_t sl_net_wifi_ap_up(sl_net_interface_t interface, sl_net_profile_id_t profile_id)
{
  if (!((interface == SL_NET_WIFI_AP_1_INTERFACE) || (interface == SL_NET_WIFI_AP_2_INTERFACE))) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_status_t status               = SL_STATUS_FAIL;
  sl_net_wifi_ap_profile_t profile = { 0 };
  status                           = sl_net_get_profile(interface, profile_id, &profile);
  VERIFY_STATUS_AND_RETURN(status);

  // Validate profile configuration
  // AP only supports static IP (no DHCP client for AP)
  if (profile.ip.mode != SL_IP_MANAGEMENT_STATIC_IP) {
    SL_DEBUG_LOG("AP mode requires static IP configuration\n");
    return SL_STATUS_INVALID_CONFIGURATION;
  }
  // Configure the ap_interface based on the band
  if (profile.config.channel.band == SL_WIFI_BAND_2_4GHZ || profile.config.channel.band == SL_WIFI_AUTO_BAND) {
    ap_interface  = SL_WIFI_AP_2_4GHZ_INTERFACE;
    ap_profile_id = profile_id;
  } else if (profile.config.channel.band == SL_WIFI_BAND_5GHZ) {
    ap_interface  = SL_WIFI_AP_5GHZ_INTERFACE;
    ap_profile_id = profile_id;
  } else if (profile.config.channel.band == SL_WIFI_BAND_DUAL) {
    ap_interface  = SL_WIFI_AP_DUAL_INTERFACE;
    ap_profile_id = profile_id;
  } else {
    return SL_STATUS_INVALID_PARAMETER;
  }
  // Start the AP interface at WiFi layer // This should not be hardcoded..
  status = sl_wifi_start_ap(ap_interface, &profile.config);
  VERIFY_STATUS_AND_RETURN(status);

  // Configure static IP and bring up the LwIP netif
  status = set_ap_link_up(&profile);
  if (status != SL_STATUS_OK) {
    // Stop AP on failure
    sl_wifi_stop_ap(ap_interface);
    return status;
  }

  sli_copy_lwip_netif_to_ip_configuration(&wifi_ap_context->netif, &profile.ip);

  // Save the profile
  status = sl_net_set_profile(interface, profile_id, &profile);
  VERIFY_STATUS_AND_RETURN(status);
  return SL_STATUS_OK;
}

sl_status_t sl_net_wifi_ap_down(sl_net_interface_t interface)
{
  if (!((interface == SL_NET_WIFI_AP_1_INTERFACE) || (interface == SL_NET_WIFI_AP_2_INTERFACE))) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Copy netif IP address, netmask and gateway into profile before stopping AP
  if (wifi_ap_context != NULL) {
    sl_net_wifi_ap_profile_t profile = { 0 };
    sl_status_t status               = sl_net_get_profile(interface, ap_profile_id, &profile);
    if (status == SL_STATUS_OK) {
      sli_copy_lwip_netif_to_ip_configuration(&wifi_ap_context->netif, &profile.ip);
      (void)sl_net_set_profile(interface, ap_profile_id, &profile);
    }
  }

  // Bring down LwIP interface
  set_ap_link_down();

  // Stop AP at WiFi layer
  return sl_wifi_stop_ap(ap_interface);
}

sl_status_t sl_net_wifi_ap_deinit(sl_net_interface_t interface)
{
  sl_status_t status = SL_STATUS_FAIL;
  if (!((interface == SL_NET_WIFI_AP_1_INTERFACE) || (interface == SL_NET_WIFI_AP_2_INTERFACE))) {
    return SL_STATUS_INVALID_PARAMETER;
  }

#if LWIP_TESTMODE
  struct sys_timeo **list_head = NULL;

  //! Free all timers
  for (int i = 0; i < lwip_num_cyclic_timers; i++) {
    list_head = sys_timeouts_get_next_timeout();
    if (list_head != NULL && *list_head != NULL)
      sys_untimeout((*list_head)->h, (*list_head)->arg);
  }
#endif

  // Clean up netif
  if (wifi_ap_context != NULL) {
    netif_remove(&(wifi_ap_context->netif));
    wifi_ap_context = NULL;
  }

  status = sl_wifi_deinit();
  VERIFY_STATUS_AND_RETURN(status);

  return SL_STATUS_OK;
}
