/***************************************************************************/ /**
 * @file
 * @brief WPS Push Button Configuration (PBC) Example Application
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

#include "app.h"
#include "sl_net.h"
#include "sl_wifi.h"
#include "sli_wifi_constants.h"
#include "sl_wifi_credentials.h"
#include "sl_utility.h"
#include "cmsis_os2.h"
#include "sl_wifi_callback_framework.h"
#include "sl_net_si91x.h"
#include <stdio.h>
#include <string.h>

/******************************************************
 *               Constant Definitions
 ******************************************************/
#define SLI_WIFI_CONNECT_TIMEOUT 120000 // 120 seconds in milliseconds

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

static const sl_wifi_device_configuration_t wps_client_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_WIFI_BAND_MODE_2_4GHZ,
  .region_code = SL_WIFI_REGION_US,
  .boot_config = { .oper_mode = SL_SI91X_CLIENT_MODE,
                   .coex_mode = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map =
#ifdef SLI_SI91X_MCU_INTERFACE
                     (SL_WIFI_FEAT_SECURITY_OPEN),
#else
                     (SL_WIFI_FEAT_SECURITY_OPEN | SL_WIFI_FEAT_AGGREGATION),
#endif
                   .tcp_ip_feature_bit_map = (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT | SL_SI91X_TCP_IP_FEAT_DNS_CLIENT
                                              | SL_SI91X_TCP_IP_FEAT_SSL | SL_SI91X_TCP_IP_FEAT_MDNSD
#ifdef SLI_SI91X_ENABLE_IPV6
                                              | SL_SI91X_TCP_IP_FEAT_DHCPV6_CLIENT | SL_SI91X_TCP_IP_FEAT_IPV6
#endif
                                              | SL_SI91X_TCP_IP_FEAT_ICMP | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID),
                   .custom_feature_bit_map = SL_WIFI_SYSTEM_CUSTOM_FEAT_EXTENSION_VALID,
                   .ext_custom_feature_bit_map =
                     (SL_SI91X_EXT_FEAT_XTAL_CLK | SL_SI91X_EXT_FEAT_UART_SEL_FOR_DEBUG_PRINTS | MEMORY_CONFIG
#if defined(SLI_SI917)
                      | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
                      ),
                   .bt_feature_bit_map = 0,
                   .ext_tcp_ip_feature_bit_map =
                     (SL_SI91X_EXT_TCP_IP_WINDOW_SCALING | SL_SI91X_EXT_TCP_IP_TOTAL_SELECTS(10)),
                   .ble_feature_bit_map     = 0,
                   .ble_ext_feature_bit_map = 0,
                   .config_feature_bit_map  = 0 },
  .ta_pool         = { .tx_ratio_in_buffer_pool = 0, .rx_ratio_in_buffer_pool = 0, .global_ratio_in_buffer_pool = 0 },
  .efuse_data_type = SL_SI91X_EFUSE_MFG_SW_VERSION,
  .nwp_fw_image_number = SL_SI91X_NWP_FW_IMAGE_NUMBER_0
};

/******************************************************
 *               Function Declarations
 ******************************************************/
static void application_start(void *argument);
static sl_status_t wps_pbc(void);
static void print_wps_response(const sl_wifi_wps_response_t *response);
static sl_status_t wps_client_connect_from_wps_profile(const sl_wifi_wps_response_t *profile);
static sl_status_t wps_client_configure_dhcp_ipv4(void);

/******************************************************
 *               Function Definitions
 ******************************************************/

void app_init(void)
{
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

static void print_wps_response(const sl_wifi_wps_response_t *response)
{
  if (response == NULL) {
    SL_DEBUG_LOG_V2(WARN, "WPS Response: NULL\r\n");
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "SSID: ");
  if (response->ssid_len > 0 && response->ssid_len <= 32) {
    for (size_t i = 0; i < response->ssid_len; i++) {
      SL_DEBUG_LOG_V2(DEBUG, "%c", response->ssid[i]);
    }
  } else {
    SL_DEBUG_LOG_V2(INFO, "(empty)");
  }
  SL_DEBUG_LOG_V2(INFO, "\r\n");

  SL_DEBUG_LOG_V2(INFO, "SSID Length: %u bytes\r\n", response->ssid_len);
  SL_DEBUG_LOG_V2(INFO, "Security Type: 0x%02X\r\n", response->security_type);
  SL_DEBUG_LOG_V2(INFO, "Status: 0x%08lX\r\n", response->status);
  SL_DEBUG_LOG_V2(INFO, "Key: ");
  for (int i = 0; i < (int)SL_WIFI_WPS_KEY_LENGTH; i++) {
    SL_DEBUG_LOG_V2(DEBUG, "%02X", response->key[i]);
  }
  SL_DEBUG_LOG_V2(INFO, "\r\n");
  SL_DEBUG_LOG_V2(INFO, "remaining_credentials_count: %u", response->remaining_credentials_count);
}

/** Join STA using one WPS credential profile (PMK from WPS key material when not open). */
static sl_status_t wps_client_connect_from_wps_profile(const sl_wifi_wps_response_t *profile)
{
  sl_wifi_client_configuration_t client_config = { 0 };
  sl_status_t status;

  if (profile == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  if (profile->ssid_len == 0U || profile->ssid_len > sizeof(client_config.ssid.value)) {
    SL_DEBUG_LOG_V2(ERROR, "WPS profile has invalid SSID length (%u)", profile->ssid_len);
    return SL_STATUS_INVALID_PARAMETER;
  }

  memcpy(client_config.ssid.value, profile->ssid, profile->ssid_len);
  client_config.ssid.length = profile->ssid_len;
  client_config.security    = profile->security_type;

  if (profile->security_type == SL_WIFI_OPEN) {
    client_config.credential_id = SL_WIFI_NO_CREDENTIAL_ID;
  } else {
    sl_wifi_pmk_credential_t pmk_credential = { 0 };

    memcpy(pmk_credential.value, profile->key, SL_WIFI_WPS_KEY_LENGTH);
    status = sl_net_set_credential(SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID,
                                   SL_NET_WIFI_PMK,
                                   pmk_credential.value,
                                   SL_WIFI_WPS_KEY_LENGTH);
    if (status != SL_STATUS_OK) {
      SL_DEBUG_LOG_V2(ERROR, "Failed to set PMK credential: 0x%lX", status);
      return status;
    }
    client_config.credential_id = SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID;
  }

  SL_DEBUG_LOG_V2(INFO, "Connecting to SSID:");
  if (profile->ssid_len > 0 && profile->ssid_len <= 32) {
    for (size_t i = 0; i < profile->ssid_len; i++) {
      SL_DEBUG_LOG_V2(INFO, "%c", profile->ssid[i]);
    }
  }

  status = sl_wifi_connect(SL_WIFI_CLIENT_INTERFACE, &client_config, SLI_WIFI_CONNECT_TIMEOUT);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to connect to Wi-Fi: 0x%lX", status);
    return status;
  }
  SL_DEBUG_LOG_V2(INFO, "Connected to Wi-Fi successfully!");
  return SL_STATUS_OK;
}

static sl_status_t wps_client_configure_dhcp_ipv4(void)
{
  sl_status_t status;
  sl_net_ip_configuration_t ip_address = { 0 };
  ip_address.type                      = SL_IPV4;
  ip_address.mode                      = SL_IP_MANAGEMENT_DHCP;
  ip_address.host_name                 = NULL;

  status = sl_si91x_configure_ip_address(&ip_address, SL_SI91X_WIFI_CLIENT_VAP_ID);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "IP Configuration failed, error: 0x%lX", status);
    return status;
  }

  sl_ip_address_t ip = { 0 };
  ip.type            = ip_address.type;
  ip.ip.v4.value     = ip_address.ip.v4.ip_address.value;
  SL_DEBUG_LOG_V2(INFO, "IP Configuration successful");
  print_sl_ip_address(&ip);
  return SL_STATUS_OK;
}

static void application_start(void *argument)
{
  UNUSED_PARAMETER(argument);
  sl_status_t status;

  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &wps_client_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to start Wi-Fi Client interface: 0x%lX\r\n", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Wi-Fi client interface up success\r\n");
  status = wps_pbc();
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "WPS connection failed: 0x%lX\r\n", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "WPS connection successful\r\n");

  while (1) {
    osDelay(2000);
  }
}

static sl_status_t wps_pbc(void)
{
  sl_wifi_wps_config_t wps_config     = { 0 };
  sl_wifi_wps_response_t wps_response = { 0 };
  sl_status_t status;
  SL_DEBUG_LOG_V2(INFO, "Starting WPS PBC procedure...\r\n");
  // Configure WPS settings
  wps_config.role         = SL_WIFI_WPS_ENROLLEE_ROLE;
  wps_config.mode         = SL_WIFI_WPS_PUSH_BUTTON_MODE;
  wps_config.auto_connect = true;

  memset(wps_config.optional_pin, 0, sizeof(wps_config.optional_pin));
  status = sl_wifi_start_wps_v2(SL_WIFI_CLIENT_INTERFACE, wps_config, &wps_response);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to start WPS PBC: 0x%lX\r\n", status);
    return status;
  }
  SL_DEBUG_LOG_V2(INFO, "WPS procedure completed successfully!\r\n");
  print_wps_response(&wps_response);

  /* When the AP sends multiple WPS profiles, fetch the rest after the primary response (needed for host join retries). */
  sl_wifi_wps_response_t remaining[SLI_WIFI_MAX_WPS_CREDENTIALS - 1U] = { 0 };
  uint8_t remaining_count                                             = 0;

  if (wps_config.auto_connect == false) {
    if (wps_response.remaining_credentials_count > 0U) {
      status = sl_wifi_wps_get_remaining_credentials(SL_WIFI_CLIENT_INTERFACE,
                                                     remaining,
                                                     wps_response.remaining_credentials_count);
      if (status != SL_STATUS_OK) {
        SL_DEBUG_LOG_V2(ERROR, "Failed to get remaining WPS credentials: 0x%lX", status);
        return status;
      }

      remaining_count = wps_response.remaining_credentials_count;
      SL_DEBUG_LOG_V2(INFO, "Fetched %u additional WPS credential profile(s)", remaining_count);
      for (uint8_t i = 0; i < remaining_count; i++) {
        SL_DEBUG_LOG_V2(INFO, "--- Additional profile %u ---", (unsigned int)(i + 1U));
        print_wps_response(&remaining[i]);
      }
    }
    SL_DEBUG_LOG_V2(INFO, "auto_connect disabled: host-driven join using WPS credential(s)");

    status = wps_client_connect_from_wps_profile(&wps_response);
    for (uint8_t i = 0; (status != SL_STATUS_OK) && (i < remaining_count); i++) {
      SL_DEBUG_LOG_V2(WARN,
                      "Join failed (0x%lX); trying additional WPS profile %u of %u",
                      status,
                      (unsigned int)(i + 1U),
                      (unsigned int)remaining_count);
      status = wps_client_connect_from_wps_profile(&remaining[i]);
    }
    if (status != SL_STATUS_OK) {
      SL_DEBUG_LOG_V2(ERROR, "Join failed for primary and all additional WPS profiles (last status 0x%lX)", status);
      return status;
    }

    status = wps_client_configure_dhcp_ipv4();
    if (status != SL_STATUS_OK) {
      return status;
    }
  }
  return status;
}
