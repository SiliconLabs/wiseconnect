/***************************************************************************/ /**
 * @file
 * @brief Access Point V6 Example Application
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_net.h"
#include "cmsis_os2.h"
#include "sl_wifi_device.h"
#include "sl_utility.h"
#include "sl_wifi.h"
#include "sl_net_wifi_types.h"
#include "sl_constants.h"
#include <string.h>

/******************************************************
 *                      Macros
 ******************************************************/
#define WIFI_AP_PROFILE_SSID "MY_AP_SSID"
#define WIFI_AP_CREDENTIAL   "MY_AP_PASSPHRASE"

// IP address of the module
#define DEVICE_IPV6 "2001:db8:0:1::121"

// IP address of Gateway
#define GATEWAY_IPV6 "2001:db8:0:1::121"

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
  .reserved   = 0,
};

static const sl_wifi_device_configuration_t sl_wifi_acess_point_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .region_code = US,
  .boot_config = { .oper_mode              = SL_SI91X_ACCESS_POINT_MODE,
                   .coex_mode              = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map        = SL_SI91X_FEAT_SECURITY_OPEN | SL_SI91X_FEAT_SECURITY_PSK,
                   .tcp_ip_feature_bit_map = SL_SI91X_TCP_IP_FEAT_DHCPV4_SERVER | SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT
                                             | SL_SI91X_TCP_IP_FEAT_DHCPV6_CLIENT | SL_SI91X_TCP_IP_FEAT_IPV6
                                             | SL_SI91X_TCP_IP_FEAT_DHCPV6_SERVER,
                   .custom_feature_bit_map     = 0,
                   .ext_custom_feature_bit_map = 0,
                   .bt_feature_bit_map         = 0,
                   .ext_tcp_ip_feature_bit_map = 0,
                   .ble_feature_bit_map        = 0,
                   .ble_ext_feature_bit_map    = 0,
                   .config_feature_bit_map     = 0 }
};

static sl_net_wifi_ap_profile_t wifi_ap_profile = {
    .config = {
        .ssid.value = WIFI_AP_PROFILE_SSID,
        .ssid.length = sizeof(WIFI_AP_PROFILE_SSID)-1,
        .channel.channel = SL_WIFI_AUTO_CHANNEL,
        .channel.band = SL_WIFI_AUTO_BAND,
        .channel.bandwidth = SL_WIFI_AUTO_BANDWIDTH,
        .security = SL_WIFI_WPA2,
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
        .mode = SL_IP_MANAGEMENT_STATIC_IP,
        .type = SL_IPV6,
        .host_name = NULL,
        .ip = {{{0}}},
    }
};

sl_net_wifi_psk_credential_entry_t wifi_ap_credential = { .type        = SL_NET_WIFI_PSK,
                                                          .data_length = sizeof(WIFI_AP_CREDENTIAL) - 1,
                                                          .data        = WIFI_AP_CREDENTIAL };

/******************************************************
 *               Function Declarations
 ******************************************************/

static sl_status_t ap_event_handler(uint32_t event, sl_status_t status, void *data, uint32_t data_length);
static void application_start(void *argument);

/******************************************************
 *               Function Definitions
 ******************************************************/

void app_init(const void *unused)
{
  UNUSED_PARAMETER(unused);
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

static void application_start(void *argument)
{
  UNUSED_PARAMETER(argument);
  sl_status_t status;
  unsigned char hex_addr[SL_IPV6_ADDRESS_LENGTH] = { 0 };
  uint32_t ip_addr[4]                            = { 0 };
  uint32_t gateway[4]                            = { 0 };
  int return_value;

  status = sl_net_init(SL_NET_WIFI_AP_INTERFACE, &sl_wifi_acess_point_configuration, NULL, ap_event_handler);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to start Wi-Fi AP interface: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nWi-Fi AP interface init success");

  return_value = sl_inet_pton6(DEVICE_IPV6, DEVICE_IPV6 + strlen(DEVICE_IPV6), hex_addr, (unsigned int *)ip_addr);
  if (return_value != 0x1) {
    printf("\r\nIPv6 conversion failed.\r\n");
    return;
  }
  return_value = sl_inet_pton6(GATEWAY_IPV6, GATEWAY_IPV6 + strlen(GATEWAY_IPV6), hex_addr, (unsigned int *)gateway);
  if (return_value != 0x1) {
    printf("\r\nIPv6 conversion failed.\r\n");
    return;
  }

  //fill ip
  memcpy(wifi_ap_profile.ip.ip.v6.global_address.value, ip_addr, sizeof(sl_ipv6_address_t));
  memcpy(wifi_ap_profile.ip.ip.v6.gateway.value, gateway, sizeof(sl_ipv6_address_t));

  status = sl_net_set_profile(SL_NET_WIFI_AP_INTERFACE, SL_NET_PROFILE_ID_1, &wifi_ap_profile);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to set AP profile: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nSuccess to set AP profile\r\n");

  status = sl_net_set_credential(SL_NET_DEFAULT_WIFI_AP_CREDENTIAL_ID,
                                 wifi_ap_credential.type,
                                 &wifi_ap_credential.data,
                                 wifi_ap_credential.data_length);
  if (status != SL_STATUS_OK) {
    printf("Failed to set credentials: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nWi-Fi set credential success\r\n");

  status = sl_net_up(SL_NET_WIFI_AP_INTERFACE, SL_NET_PROFILE_ID_1);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to bring Wi-Fi AP interface up: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nAP started\r\n");

  while (1) {
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
    // Let the CPU go to sleep if the system allows it.
    sl_power_manager_sleep();
#else
    osDelay(osWaitForever);
#endif
  }
}

static sl_status_t ap_event_handler(uint32_t event, sl_status_t status, void *data, uint32_t data_length)
{
  UNUSED_PARAMETER(status);
  UNUSED_PARAMETER(data_length);
  switch (event) {
    case SL_WIFI_CLIENT_CONNECTED_EVENT:
      printf("Remote client connected: ");
      print_mac_address((sl_mac_address_t *)data);
      printf("\n");
      break;
    case SL_WIFI_CLIENT_DISCONNECTED_EVENT:
      printf("Remote client disconnected: ");
      print_mac_address((sl_mac_address_t *)data);
      printf("\n");
      break;
  }
  return SL_STATUS_OK;
}
