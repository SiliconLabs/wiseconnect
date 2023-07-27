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
#include "sl_net_wifi_types.h"
#include "sl_tls.h"
#include "cmsis_os2.h"
#include "sl_ip_types.h"
#include "wifiuser.pem.h"
#include "sl_wifi.h"

/******************************************************
 *               Function Declarations
 ******************************************************/
static void application_start(void *argument);

/******************************************************
 *                    Constants
 ******************************************************/
#define WIFI_ACCESS_POINT_SSID     "enterprise"
#define WIFI_CLIENT_CERTIFICATE_ID 1

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

static const sl_net_wifi_client_profile_t wifi_client_enterprise_eap_profile = {
    .config = {
        .ssid.value = WIFI_ACCESS_POINT_SSID,
        .ssid.length = sizeof(WIFI_ACCESS_POINT_SSID)-1,
        .channel.channel = SL_WIFI_AUTO_CHANNEL,
        .channel.band = SL_WIFI_AUTO_BAND,
        .channel.bandwidth = SL_WIFI_AUTO_BANDWIDTH,
        .bssid = {{0}},
        .bss_type = SL_WIFI_BSS_TYPE_INFRASTRUCTURE,
        .security = SL_WIFI_WPA2_ENTERPRISE,
        .encryption = SL_WIFI_EAP_TLS_ENCRYPTION,
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

static const sl_net_wifi_eap_credential_entry_t wifi_client_enterprise_eap_credential = {
  .type                = SL_NET_WIFI_PSK,
  .data_length         = sizeof(sl_wifi_eap_credential_t),
  .data.username       = "user1",
  .data.password       = "password1",
  .data.certificate_id = WIFI_CLIENT_CERTIFICATE_ID
};

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
  sl_status_t status                             = SL_STATUS_OK;
  sl_tls_store_configuration_t tls_configuration = { 0 };

  // Set the custom Wi-Fi client profile
  sl_net_set_profile(SL_NET_DEFAULT_WIFI_CLIENT_INTERFACE,
                     SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID,
                     &wifi_client_enterprise_eap_profile);

  // Set the custom Wi-Fi client enterprise credential
  sl_net_set_credential(SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID,
                        SL_NET_WIFI_EAP,
                        &(wifi_client_enterprise_eap_credential.data),
                        sizeof(sl_wifi_eap_credential_t));

  // Initialize the network interface
  status =
    sl_net_init(SL_NET_DEFAULT_WIFI_CLIENT_INTERFACE, &sl_wifi_default_enterprise_client_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    printf("Failed to start Wi-Fi client interface: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nWi-Fi client interface up Success\r\n");

  tls_configuration.cacert             = (uint8_t *)wifiuser;
  tls_configuration.cacert_length      = (sizeof(wifiuser) - 1);
  tls_configuration.cacert_type        = SL_TLS_EAP_CLIENT;
  tls_configuration.use_secure_element = false;

  // Set the Wi-Fi client certificate
  status = sl_tls_set_global_ca_store(tls_configuration);
  if (status != SL_STATUS_OK) {
    printf("\r\nLoading SSL EAP certificate in to FLASH Failed, Error Code : 0x%lX\r\n", status);
    return;
  }

  printf("\r\nLoad SSL EAP certificate Success\r\n");

  // Bring up network interface
  status = sl_net_up(SL_NET_DEFAULT_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID);
  if (status != SL_STATUS_OK) {
    printf("Failed to bring Wi-Fi client interface up: 0x%lx\r\n", status);
    return;
  }
  printf("Wi-Fi client connected\r\n");
}
