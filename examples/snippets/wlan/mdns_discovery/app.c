/***************************************************************************/ /**
 * @file
 * @brief Mdns Disovery Example Application
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
#include "errno.h"
#include "netdb.h"
#include "sl_wifi_callback_framework.h"
#include "sl_status.h"
#include "cmsis_os2.h"
#include "sl_wifi.h"
#include "sl_utility.h"
#include "sl_net.h"
#include "sl_net_si91x.h"
#include "sl_net_wifi_types.h"
#include <string.h>
#include <stdint.h>
#include "sl_mdns.h"
#include "sl_net_default_values.h"

/******************************************************
 *                    Constants
 ******************************************************/
#define MDNSV6_ENABLE 0

#if MDNSV6_ENABLE
#define SERVER_IP  "2401:4901:1290:10de::1004"
#define IP_VERSION SL_IPV6_VERSION
#else
#define SERVER_IP  "192.168.50.131"
#define IP_VERSION SL_IPV4_VERSION
#endif

#define CLIENT_MESSAGE "WIFISDK"

#define MDNS_HOST_NAME       "wiseconnect.local."
#define MDNS_SERVICE_TYPE    "_http._tcp.local."
#define MDNS_SERVICE_NAME    "wiseconnect._http._tcp.local"
#define MDNS_SERVICE_MESSAGE "HTTP service"
#define MDNS_SERVICE_PORT    80
#define MDNS_SERVICE_TTL     300

volatile bool mdns_discovery_done = false;

/******************************************************
 *               Variable Definitions
 ******************************************************/
const osThreadAttr_t thread_attributes = {
  .name       = "app",
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = 3072,
  .priority   = osPriorityLow,
  .tz_module  = 0,
};

sl_ip_address_t ip_address           = { 0 };
sl_net_wifi_client_profile_t profile = { 0 };
/******************************************************
 *               Function Declarations
 ******************************************************/
static void application_start(void *argument);
static sl_status_t mdns_event_handler(sl_net_event_t event, sl_status_t status, void *data, uint32_t data_length);

/******************************************************
 *               Function Definitions
 ******************************************************/

static sl_status_t mdns_event_handler(sl_net_event_t event, sl_status_t status, void *data, uint32_t data_length)
{
  UNUSED_PARAMETER(data_length);

  if (event == SL_NET_MDNS_STOP_EVENT) {
    SL_DEBUG_LOG_V2(WARN, "Discovery operation timed out.\r\n");
    mdns_discovery_done = true;
    return SL_STATUS_OK;
  }

  if (event != SL_NET_MDNS_EVENT) {
    return SL_STATUS_OK;
  }

  SL_DEBUG_LOG_V2(INFO, "Received mDNS Discovery Event with status: 0x%lX\r\n", status);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "mDNS Discovery failed!\r\n");
    return status;
  }

  sl_mdns_response_t *result = (sl_mdns_response_t *)data;

  switch ((sl_mdns_query_type_t)result->query_type) {
    case SL_MDNS_QUERY_TYPE_PTR:
      SL_DEBUG_LOG_V2(INFO, "=== PTR Record ===\r\n");
      if (result->instance_name) {
        SL_DEBUG_LOG_V2(INFO, "Instance Name : %s\r\n", (uintptr_t)result->instance_name);
      }
      if (result->service_type) {
        SL_DEBUG_LOG_V2(INFO, "Service Type  : %s\r\n", (uintptr_t)result->service_type);
      }
      if (result->proto) {
        SL_DEBUG_LOG_V2(INFO, "Protocol      : %s\r\n", (uintptr_t)result->proto);
      }
      SL_DEBUG_LOG_V2(INFO, "TTL           : %lu\r\n", result->ttl);
      break;

    case SL_MDNS_QUERY_TYPE_SRV:
      SL_DEBUG_LOG_V2(INFO, "=== SRV Record ===\r\n");
      if (result->instance_name) {
        SL_DEBUG_LOG_V2(INFO, "Service Name  : %s\r\n", (uintptr_t)result->instance_name);
      }
      if (result->hostname) {
        SL_DEBUG_LOG_V2(INFO, "Target Host   : %s\r\n", (uintptr_t)result->hostname);
      }
      SL_DEBUG_LOG_V2(INFO, "Port          : %u\r\n", result->port);
      SL_DEBUG_LOG_V2(INFO, "TTL           : %lu\r\n", result->ttl);
      break;

    case SL_MDNS_QUERY_TYPE_A:
      SL_DEBUG_LOG_V2(INFO, "=== A Record ===\r\n");
      if (result->hostname) {
        SL_DEBUG_LOG_V2(INFO, "Hostname      : %s\r\n", (uintptr_t)result->hostname);
      }
      for (size_t i = 0; i < result->addr.addr_count; i++) {
        sl_ip_address_t *ip = &result->addr.addr[i];
        if (ip->type == SL_IPV4) {
          SL_DEBUG_LOG_V2(INFO, "IPv4 Address  : ");
          print_sl_ip_address(ip);
        }
      }
      SL_DEBUG_LOG_V2(INFO, "TTL           : %lu\r\n", result->ttl);
      break;

    case SL_MDNS_QUERY_TYPE_AAAA:
      SL_DEBUG_LOG_V2(INFO, "=== AAAA Record ===\r\n");
      if (result->hostname) {
        SL_DEBUG_LOG_V2(INFO, "Hostname      : %s\r\n", (uintptr_t)result->hostname);
      }
      for (size_t i = 0; i < result->addr.addr_count; i++) {
        sl_ip_address_t *ip = &result->addr.addr[i];
        if (ip->type == SL_IPV6) {
          SL_DEBUG_LOG_V2(INFO, "IPv6 Address  : ");
          print_sl_ip_address(ip);
        }
      }
      SL_DEBUG_LOG_V2(INFO, "TTL           : %lu\r\n", result->ttl);
      break;

    case SL_MDNS_QUERY_TYPE_TXT:
      SL_DEBUG_LOG_V2(INFO, "=== TXT Record ===\r\n");
      if (result->instance_name) {
        SL_DEBUG_LOG_V2(INFO, "Service Name  : %s\r\n", (uintptr_t)result->instance_name);
      }
      for (size_t i = 0; i < result->txt.txt_count; i++) {
        if (result->txt.txt[i]) {
          SL_DEBUG_LOG_V2(INFO, "TXT Record    : %s\r\n", (uintptr_t)result->txt.txt[i]);
        }
      }
      SL_DEBUG_LOG_V2(INFO, "TTL           : %lu\r\n", result->ttl);
      break;

    default:
      SL_DEBUG_LOG_V2(WARN, "Unknown query type: %u\r\n", result->query_type);
      break;
  }

  return SL_STATUS_OK;
}

void app_init(const void *unused)
{
  UNUSED_PARAMETER(unused);
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

static void application_start(void *argument)
{
  UNUSED_PARAMETER(argument);
  sl_status_t status             = SL_STATUS_FAIL;
  sl_mdns_t mdns                 = { 0 };
  sl_mdns_configuration_t config = { .protocol = SL_MDNS_PROTO_UDP, .type = IP_VERSION, .host_name = MDNS_HOST_NAME };
  sl_mdns_service_t service      = { .instance_name   = MDNS_SERVICE_NAME,
                                     .service_type    = MDNS_SERVICE_TYPE,
                                     .service_message = MDNS_SERVICE_MESSAGE,
                                     .port            = MDNS_SERVICE_PORT,
                                     .ttl             = MDNS_SERVICE_TTL };

  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &sl_wifi_default_client_configuration, NULL, mdns_event_handler);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to start Wi-Fi Client interface: 0x%lx\r\n", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Wi-Fi Init Success \r\n");

  profile = DEFAULT_WIFI_CLIENT_PROFILE;
#if MDNSV6_ENABLE
  profile.ip.type = SL_IPV6;
#else
  profile.ip.type = SL_IPV4;
#endif
  status = sl_net_set_profile(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID, &profile);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to set client profile: 0x%lx", status);
    return;
  }

  status = sl_net_up(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to bring Wi-Fi client interface up: 0x%lx", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Wi-Fi client connected");

  status = sl_net_get_profile(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID, &profile);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to get client profile: 0x%lx", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Client profile is fetched successfully.");

  if (profile.ip.type == SL_IPV4) {
    ip_address.type = SL_IPV4;
    memcpy(&ip_address.ip.v4.bytes, &profile.ip.ip.v4.ip_address.bytes, sizeof(sl_ipv4_address_t));
    print_sl_ip_address(&ip_address);
  } else if (profile.ip.type == SL_IPV6) {
    sl_ip_address_t link_local_address = { 0 };
    memcpy(&link_local_address.ip.v6, &profile.ip.ip.v6.link_local_address, SL_IPV6_ADDRESS_LENGTH);
    link_local_address.type = SL_IPV6;
    SL_DEBUG_LOG_V2(INFO, "Link Local Address: ");
    print_sl_ip_address(&link_local_address);

    sl_ip_address_t global_address = { 0 };
    memcpy(&global_address.ip.v6, &profile.ip.ip.v6.global_address, SL_IPV6_ADDRESS_LENGTH);
    global_address.type = SL_IPV6;
    SL_DEBUG_LOG_V2(INFO, "Global Address: ");
    print_sl_ip_address(&global_address);

    sl_ip_address_t gateway = { 0 };
    memcpy(&gateway.ip.v6, &profile.ip.ip.v6.gateway, SL_IPV6_ADDRESS_LENGTH);
    gateway.type = SL_IPV6;
    SL_DEBUG_LOG_V2(INFO, "Gateway Address: ");
    print_sl_ip_address(&gateway);
  }

  // Initialize MDNS service
  status = sl_mdns_init(&mdns, (const sl_mdns_configuration_t *)&config, NULL);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to initialize MDNS : 0x%lx\r\n", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "MDNS initialized\r\n");

  // Add an interface to MDNS Instance
  status = sl_mdns_add_interface(&mdns, SL_NET_WIFI_CLIENT_INTERFACE);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to add interface to MDNS : 0x%lx\r\n", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Interface Added to MDNS\r\n");

  // Add a service to MDNS Instance
  status = sl_mdns_register_service(&mdns, SL_NET_WIFI_CLIENT_INTERFACE, &service);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to register service to MDNS: 0x%lx\r\n", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Service Added to MDNS\r\n");

  sl_mdns_service_query_t service_query = { .service_type = "_http._tcp.local.", // Query for HTTP services
                                            .query_type   = SL_MDNS_QUERY_TYPE_PTR,
                                            .timeout      = 5000 };

  status = sl_mdns_service_discovery_start(&mdns, SL_NET_WIFI_CLIENT_INTERFACE, &service_query);
  if (status != SL_STATUS_IN_PROGRESS) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to send mDNS service discovery request: 0x%lx\r\n", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "mDNS Service Discovery request sent successfully\r\n");

  while (!mdns_discovery_done) {
    osDelay(100); // Wait in 100ms intervals
  }
  // Deinitialize MDNS service
  status = sl_mdns_deinit(&mdns);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to deinitialize MDNS : 0x%lx\r\n", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Deinitialize MDNS successfully\r\n");
}
