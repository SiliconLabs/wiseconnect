/***************************************************************************/ /**
 * @file
 * @brief Concurrent Mode Example Application
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

/******************************************************
 *                      Macros
 ******************************************************/
#define WIFI_CLIENT_PROFILE_SSID "YOUR_AP_SSID"
#define WIFI_CLIENT_CREDENTIAL   "YOUR_AP_PASSPHRASE"

#define WIFI_AP_PROFILE_SSID "MY_AP_SSID"
#define WIFI_AP_CREDENTIAL   "MY_AP_PASSPHRASE"

#define WIFI_CLIENT_SECURITY_TYPE   SL_WIFI_WPA_WPA2_MIXED
#define WIFI_CLIENT_ENCRYPTION_TYPE SL_WIFI_CCMP_ENCRYPTION

//! IP address of the module
//! E.g: 0x0A0AA8C0 == 192.168.10.10
#define DEFAULT_WIFI_MODULE_IP_ADDRESS 0x0A0AA8C0

//! IP address of netmask
//! E.g: 0x00FFFFFF == 255.255.255.0
#define DEFAULT_WIFI_SN_MASK_ADDRESS 0x00FFFFFF

//! IP address of Gateway
//! E.g: 0x0A0AA8C0 == 192.168.10.10
#define DEFAULT_WIFI_GATEWAY_ADDRESS 0x0A0AA8C0

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

static const sl_net_wifi_client_profile_t wifi_client_profile = {
    .config = {
        .ssid.value = WIFI_CLIENT_PROFILE_SSID,
        .ssid.length = sizeof(WIFI_CLIENT_PROFILE_SSID)-1,
        .channel.channel = SL_WIFI_AUTO_CHANNEL,
        .channel.band = SL_WIFI_AUTO_BAND,
        .channel.bandwidth = SL_WIFI_AUTO_BANDWIDTH,
        .bssid = {{0}},
        .bss_type = SL_WIFI_BSS_TYPE_INFRASTRUCTURE,
        .security = WIFI_CLIENT_SECURITY_TYPE,
        .encryption = WIFI_CLIENT_ENCRYPTION_TYPE,
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
        .type = SL_IPV4,
        .host_name = NULL,
        .ip = {
        		.v4.ip_address.value = DEFAULT_WIFI_MODULE_IP_ADDRESS,
				.v4.gateway.value = DEFAULT_WIFI_GATEWAY_ADDRESS,
				.v4.netmask.value = DEFAULT_WIFI_SN_MASK_ADDRESS
        },
    }
};

sl_net_wifi_psk_credential_entry_t wifi_client_credential = { .type        = SL_NET_WIFI_PSK,
                                                              .data_length = sizeof(WIFI_CLIENT_CREDENTIAL) - 1,
                                                              .data        = WIFI_CLIENT_CREDENTIAL };

sl_net_wifi_psk_credential_entry_t wifi_ap_credential = { .type        = SL_NET_WIFI_PSK,
                                                          .data_length = sizeof(WIFI_AP_CREDENTIAL) - 1,
                                                          .data        = WIFI_AP_CREDENTIAL };
/******************************************************
 *               Function Declarations
 ******************************************************/

static sl_status_t ap_event_handler(uint32_t event, sl_status_t status, void *data, uint32_t data_length);
static sl_status_t client_event_handler(uint32_t event, sl_status_t status, void *data, uint32_t data_length);
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
  sl_wifi_channel_t client_channel = { 0 };

  status =
    sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &sl_wifi_default_concurrent_configuration, NULL, client_event_handler);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to start Wi-Fi client interface: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nWi-Fi Client interface init\r\n");

  status = sl_net_set_profile(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_PROFILE_ID_1, &wifi_client_profile);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to set client profile: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nWi-Fi set client profile success\r\n");

  status = sl_net_set_credential(SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID,
                                 wifi_client_credential.type,
                                 &wifi_client_credential.data,
                                 wifi_client_credential.data_length);
  if (status != SL_STATUS_OK) {
    printf("Failed to set credentials: 0x%lx\r\n", status);
    return;
  }
  printf("\nWi-Fi set credential success\n");

  status = sl_net_up(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_PROFILE_ID_1);
  if (status != SL_STATUS_OK) {
    printf("Failed to bring Wi-Fi client interface up: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nWi-Fi Client interface up\r\n");

  status = sl_net_init(SL_NET_WIFI_AP_INTERFACE, &sl_wifi_default_concurrent_configuration, NULL, ap_event_handler);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to start Wi-Fi AP interface: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nWi-Fi AP interface init\r\n");

  status = sl_wifi_get_channel(SL_WIFI_CLIENT_2_4GHZ_INTERFACE, &client_channel);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to get client channel: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nSuccess to get client channel\r\n");

  wifi_ap_profile.config.channel.channel = client_channel.channel;
  status = sl_net_set_profile(SL_NET_WIFI_AP_INTERFACE, SL_NET_PROFILE_ID_1, &wifi_ap_profile);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to set AP profile: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nSuccess to set AP profile \r\n");

  status = sl_net_set_credential(SL_NET_DEFAULT_WIFI_AP_CREDENTIAL_ID,
                                 wifi_ap_credential.type,
                                 &wifi_ap_credential.data,
                                 wifi_ap_credential.data_length);
  if (status != SL_STATUS_OK) {
    printf("Failed to set credentials: 0x%lx\r\n", status);
    return;
  }
  printf("\nWi-Fi set credential success\n");

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

static sl_status_t client_event_handler(uint32_t event, sl_status_t status, void *data, uint32_t data_length)
{
  UNUSED_PARAMETER(status);
  UNUSED_PARAMETER(data);
  UNUSED_PARAMETER(data_length);
  switch (event) {
    case SL_NET_INTERFACE_UP_EVENT:
      printf("Wi-Fi client interface connected\n");
      break;
    case SL_NET_IP_UP_EVENT:
      printf("Wi-Fi client interface IP ready\n");
      break;
  }
  return SL_STATUS_OK;
}
