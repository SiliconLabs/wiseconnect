/***************************************************************************/ /**
 * @file
 * @brief HTTP OTAF Example Application
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
#include "cmsis_os2.h"
#include "sl_board_configuration.h"
#include "sl_net.h"
#include "sl_wifi_types.h"
#include <string.h>
#include <inttypes.h>
#include "sl_wifi.h"
#include "sl_si91x_driver.h"
#include "sl_wifi_callback_framework.h"
#include "firmware_upgradation.h"
#include "sl_net_dns.h"
#include "sl_utility.h"

// include certificates
#include "aws_starfield_ca.pem.h"
#include "silabs_dgcert_ca.pem.h"
#include "cacert.pem.h"

#ifdef SLI_SI91X_MCU_INTERFACE
#include "sl_si91x_hal_soc_soft_reset.h"
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
//! Type of FW update
#define M4_FW_UPDATE       0
#define TA_FW_UPDATE       1
#define TWT_SCAN_TIMEOUT   10000
#define TWT_AUTO_CONFIG    1
#define COMBINED_FW_UPDATE 2

/* Defaults migrated from sl_wifi_filter_broadcast(5000, 1, 1). */
#define BCAST_FILTER_ENABLE      (1U)
#define MCAST_FILTER_ENABLE      (1U)
#define FILTER_MODE              (0U)
#define BEACON_DROP_THRESHOLD_MS (5000U)

#define MAX_TX_AND_RX_LATENCY_LIMIT 22118400 // 6hrs in milli seconds

//! Set FW update type
#define FW_UPDATE_TYPE TA_FW_UPDATE

//! Load certificate to device flash :
//! Certificate should be loaded once and need not be loaded for every boot up
#define LOAD_CERTIFICATE 1

// Macro to set specified bit position
#define BIT(a) ((uint32_t)1U << a)

//! Enable IPv6 set this bit in FLAGS, Default is IPv4
#define HTTPV6 BIT(3)

//! Set HTTPS_SUPPORT to use HTTPS feature
#define HTTPS_SUPPORT BIT(0)

//! Set HTTP_POST_DATA to use HTTP POST LARGE DATA feature
#define HTTP_POST_DATA BIT(5)

//! Set HTTP_V_1_1 to use HTTP version 1.1
#define HTTP_V_1_1 BIT(6)

//! Enable user defined http content type in FLAGS
#define HTTP_USER_DEFINED_CONTENT_TYPE BIT(7)

// HTTP OTAF
#define HTTP_OTAF 2

#define DNS_TIMEOUT         10
#define RETRY_COUNT         1
#define MAX_DNS_RETRY_COUNT 5
#define OTAF_TIMEOUT        600000
#ifdef AWS_ENABLE
//! for example select required flag bits,  Eg:(HTTPS_SUPPORT | HTTPV6 | HTTP_USER_DEFINED_CONTENT_TYPE)
#define FLAGS HTTPS_SUPPORT
//! Server port number
#define HTTP_PORT 443
//! Server URL
#if (FW_UPDATE_TYPE == TA_FW_UPDATE)
#define HTTP_URL "SiWG917-A.2.9.0.0.16.rps"
#else
#define HTTP_URL "wifi_wlan_throughput_isp.bin"
#endif
//! Server Hostname
char *hostname = "otafaws.s3.ap-south-1.amazonaws.com";
//! set HTTP extended header
//! if NULL , driver fills default extended header
#define HTTP_EXTENDED_HEADER NULL
//! set Username
#define USERNAME ""
//! set Password
#define PASSWORD ""

#elif AZURE_ENABLE
//! for example select required flag bits,  Eg:(HTTPS_SUPPORT | HTTPV6 | HTTP_USER_DEFINED_CONTENT_TYPE)
#define FLAGS                HTTPS_SUPPORT
//! Server port number
#define HTTP_PORT            443
//! Server URL
#define HTTP_URL             "rps/firmware.rps"
//! Server Hostname
char *hostname = "si917updates.blob.core.windows.net";
//! set HTTP extended header
#define HTTP_EXTENDED_HEADER NULL
//! set Username
#define USERNAME             ""
//! set Password
#define PASSWORD             ""
#else
#define FLAGS                  0
//! Server port number
#define HTTP_PORT              80
//! HTTP Server IP address.
#define HTTP_SERVER_IP_ADDRESS "192.168.0.100"
//! HTTP resource name
#if (FW_UPDATE_TYPE == TA_FW_UPDATE)
#define HTTP_URL "SiWG917-A.2.9.0.0.16.rps"
#else
#define HTTP_URL "wifi_access_point_isp.bin"
#endif
//! set HTTP hostname
#define HTTP_HOSTNAME        "192.168.0.100"
char *hostname = HTTP_HOSTNAME;
//! set HTTP extended header
//! if NULL , driver fills default extended header
#define HTTP_EXTENDED_HEADER NULL
//! set HTTP hostname
#define USERNAME             "admin"
//! set HTTP hostname
#define PASSWORD             "admin"
#endif

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

static const sl_wifi_device_configuration_t station_init_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .region_code = US,
  .boot_config = { .oper_mode = SL_SI91X_CLIENT_MODE,
                   .coex_mode = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map =
                     (SL_WIFI_FEAT_SECURITY_PSK | SL_WIFI_FEAT_AGGREGATION | SL_SI91X_FEAT_ULP_GPIO_BASED_HANDSHAKE
#ifdef RSI_M4_INTERFACE
                      | SL_WIFI_FEAT_WPS_DISABLE
#endif
                      ),
                   .tcp_ip_feature_bit_map     = (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT | SL_SI91X_TCP_IP_FEAT_HTTP_CLIENT
                                              | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID | SL_SI91X_TCP_IP_FEAT_SSL
                                              | SL_SI91X_TCP_IP_FEAT_DNS_CLIENT),
                   .custom_feature_bit_map     = (SL_WIFI_SYSTEM_CUSTOM_FEAT_EXTENSION_VALID),
                   .ext_custom_feature_bit_map = (SL_WIFI_SYSTEM_EXT_FEAT_LOW_POWER_MODE | SL_SI91X_EXT_FEAT_XTAL_CLK
                                                  | SL_SI91X_EXT_FEAT_UART_SEL_FOR_DEBUG_PRINTS | MEMORY_CONFIG
#ifdef SLI_SI917
                                                  | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
                                                  ),
                   .bt_feature_bit_map = 0,
                   .ext_tcp_ip_feature_bit_map =
                     (SL_SI91X_EXT_FEAT_HTTP_OTAF_SUPPORT | SL_SI91X_EXT_TCP_IP_SSL_16K_RECORD
                      | SL_SI91X_CONFIG_FEAT_EXTENSION_VALID),
                   .ble_feature_bit_map     = 0,
                   .ble_ext_feature_bit_map = 0,
                   .config_feature_bit_map  = (SL_SI91X_FEAT_SLEEP_GPIO_SEL_BITMAP | SL_WIFI_ENABLE_ENHANCED_MAX_PSP) },
  .ta_pool         = { .tx_ratio_in_buffer_pool = 0, .rx_ratio_in_buffer_pool = 0, .global_ratio_in_buffer_pool = 0 },
  .efuse_data_type = SL_SI91X_EFUSE_MFG_SW_VERSION,
  .nwp_fw_image_number = SL_SI91X_NWP_FW_IMAGE_NUMBER_0
};

sl_wifi_twt_request_t default_twt_setup_configuration = {
  .twt_enable              = 1,
  .twt_flow_id             = 1,
  .wake_duration           = 0x60,
  .wake_duration_unit      = 0,
  .wake_duration_tol       = 0x60,
  .wake_int_exp            = 13,
  .wake_int_exp_tol        = 13,
  .wake_int_mantissa       = 0x1D4C,
  .wake_int_mantissa_tol   = 0x1D4C,
  .implicit_twt            = 1,
  .un_announced_twt        = 1,
  .triggered_twt           = 0,
  .twt_channel             = 0,
  .twt_protection          = 0,
  .restrict_tx_outside_tsp = 1,
  .twt_retry_limit         = 6,
  .twt_retry_interval      = 10,
  .req_type                = 1,
  .negotiation_type        = 0,
};

sl_wifi_twt_selection_v2_t default_twt_config = {
  .twt_enable            = 1,
  .average_tx_throughput = 1000,
  .tx_latency            = 0,
  .rx_latency            = 5000,
};

int twt_active_session               = 0;
int power_save_enabled               = 0;
volatile bool response               = false;
volatile sl_status_t callback_status = SL_STATUS_OK;
sl_wifi_twt_response_t twt_response;

/******************************************************
 *               Function Declarations
 ******************************************************/
void application_start(const void *unused);
sl_status_t http_otaf_app();
static sl_status_t http_fw_update_response_handler(sl_wifi_event_t event,
                                                   sl_status_t status_code,
                                                   uint16_t *data,
                                                   uint32_t data_length,
                                                   void *arg);
sl_status_t set_twt(void);
static sl_status_t twt_callback_handler(sl_wifi_event_t event,
                                        sl_status_t status_code,
                                        sl_wifi_twt_response_t *result,
                                        uint32_t result_length,
                                        void *arg);
#if LOAD_CERTIFICATE
static sl_status_t clear_and_load_certificates_in_flash(void);
#endif

/******************************************************
 *               Function Definitions
 ******************************************************/
void app_init(void)
{
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

void application_start(const void *unused)
{
  UNUSED_PARAMETER(unused);
  sl_status_t status = SL_STATUS_OK;

  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &station_init_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to start Wi-Fi client interface: 0x%" PRIx32 "", (uint32_t)status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Wi-Fi Init is successful\r\n");

#if LOAD_CERTIFICATE
  if (FLAGS & HTTPS_SUPPORT) {
    status = clear_and_load_certificates_in_flash();
    if (status != SL_STATUS_OK) {
      SL_DEBUG_LOG_V2(ERROR, "Unexpected error while loading certificate: 0x%" PRIx32 "", (uint32_t)status);
      return;
    }
  }
#endif

  status = sl_net_up(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to bring Wi-Fi client interface up: 0x%" PRIx32 "", (uint32_t)status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Connected to Wi-Fi\r\n");

  status = http_otaf_app();
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Firmware update failed: 0x%" PRIx32 "", (uint32_t)status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Firmware update is successful\r\n");
}

#if LOAD_CERTIFICATE
sl_status_t clear_and_load_certificates_in_flash(void)
{
  sl_status_t status;
  void *cert           = NULL;
  uint32_t cert_length = 0;

#ifdef AWS_ENABLE
  cert        = (void *)aws_starfield_ca;
  cert_length = (sizeof(aws_starfield_ca) - 1);
#elif AZURE_ENABLE
  cert        = (void *)silabs_dgcert_ca;
  cert_length = (sizeof(silabs_dgcert_ca) - 1);
#else
  cert        = (uint8_t *)cacert;
  cert_length = (sizeof(cacert) - 1);
#endif

  // Load SSL CA certificate
  status = sl_net_set_credential(SL_NET_TLS_SERVER_CREDENTIAL_ID(0), SL_NET_SIGNING_CERTIFICATE, cert, cert_length);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR,
                    "Loading TLS CA certificate in to FLASH Failed, Error Code : 0x%" PRIx32 "",
                    (uint32_t)status);
  } else {
    SL_DEBUG_LOG_V2(INFO, "Load TLS CA certificate at index %d Success", 0);
  }

  return status;
}
#endif

sl_status_t http_otaf_app()
{
  sl_status_t status = SL_STATUS_OK;
  uint8_t flags      = FLAGS;
  char server_ip[16];
  sl_wifi_performance_profile_v2_t performance_profile = { 0 };

#if (FW_UPDATE_TYPE == TA_FW_UPDATE)
  sl_si91x_firmware_version_t version = { 0 };
  status                              = sl_si91x_get_firmware_version(&version);
  VERIFY_STATUS_AND_RETURN(status);
  printf("\r\nFirmware version is: %x%x.%d.%d.%d.%d.%d.%d\r\n",
         version.chip_id,
         version.rom_id,
         version.major,
         version.minor,
         version.security_version,
         version.patch_num,
         version.customer_id,
         version.build_num);
#endif

  sl_wifi_set_callback_v2(SL_WIFI_HTTP_OTA_FW_UPDATE_EVENTS,
                          (sl_wifi_callback_function_v2_t)&http_fw_update_response_handler,
                          NULL);

#if defined(AWS_ENABLE) || defined(AZURE_ENABLE)
  sl_ip_address_t dns_query_rsp = { 0 };
  uint32_t server_address;
  int32_t dns_retry_count = MAX_DNS_RETRY_COUNT;
  do {
    //! Getting IP address of the AWS server using DNS request
    status = sl_net_dns_resolve_hostname_v2((const char *)hostname,
                                            DNS_TIMEOUT,
                                            RETRY_COUNT,
                                            SL_NET_DNS_TYPE_IPV4,
                                            &dns_query_rsp);
    dns_retry_count--;
  } while ((dns_retry_count != 0) && (status != SL_STATUS_OK));

  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Unexpected error while resolving dns, Error 0x%" PRIx32 "", (uint32_t)status);
    return status;
  }
  SL_DEBUG_LOG_V2(INFO, "Resolving dns Success\r\n");

  server_address = dns_query_rsp.ip.v4.value;
  sprintf((char *)server_ip,
          "%u.%u.%u.%u",
          (unsigned int)(server_address & 0x000000ff),
          (unsigned int)((server_address & 0x0000ff00) >> 8),
          (unsigned int)((server_address & 0x00ff0000) >> 16),
          (unsigned int)((server_address & 0xff000000) >> 24));

#ifdef AWS_ENABLE
  SL_DEBUG_LOG_V2(INFO, "Resolved AWS S3 Bucket IP address = %s\r\n", (uintptr_t)server_ip);
  SL_DEBUG_LOG_V2(INFO, "Firmware download from AWS S3 Bucket is in progress...\r\n");
#elif AZURE_ENABLE
  SL_DEBUG_LOG_V2(INFO, "Resolved AZURE Blob Storage IP address = %s\r\n", (uintptr_t)server_ip);
  SL_DEBUG_LOG_V2(INFO, "Firmware download from AZURE Blob Storage is in progress...\r\n");
#endif

#else
  strcpy(server_ip, HTTP_SERVER_IP_ADDRESS);
  SL_DEBUG_LOG_V2(INFO, "Local Apache Server IP Address: %s\r\n", (uintptr_t)HTTP_HOSTNAME);
#endif
  status = set_twt();
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Error while configuring TWT parameters: 0x%" PRIx32 " ", (uint32_t)status);
    return status;
  }
  SL_DEBUG_LOG_V2(INFO, "TWT Config Done\r\n");

  if (twt_active_session == 1) {
    status = sl_wifi_reschedule_twt(twt_response.twt_flow_id, SL_WIFI_SUSPEND_INDEFINITELY, 0);
    if (status != SL_STATUS_OK) {
      SL_DEBUG_LOG_V2(ERROR, "Suspending TWT Failed: 0x%" PRIx32 " ", (uint32_t)status);
      return status;
    } else {
      twt_active_session = 0;
      SL_DEBUG_LOG_V2(INFO, "Suspend TWT Done\r\n");
    }
  }

  if (power_save_enabled == 1) {
    performance_profile.profile = HIGH_PERFORMANCE;
    status                      = sl_wifi_set_performance_profile_v2(&performance_profile);
    if (status != SL_STATUS_OK) {
      SL_DEBUG_LOG_V2(ERROR, "Powersave Disabling Failed, Error Code : 0x%" PRIx32 "", (uint32_t)status);
      return status;
    } else {
      power_save_enabled = 0;
      SL_DEBUG_LOG_V2(INFO, "Power Save Disabled\r\n");
    }
  }

#ifdef AWS_ENABLE
  SL_DEBUG_LOG_V2(INFO, "Firmware download from AWS S3 Bucket is in progress...\r\n");
#elif AZURE_ENABLE
  SL_DEBUG_LOG_V2(INFO, "Firmware download from AZURE Blob Storage is in progress...\r\n");
#else
  SL_DEBUG_LOG_V2(INFO, "Firmware download from Local Apache Server is in progress...\r\n");
#endif
  sl_si91x_http_otaf_params_t http_params = { 0 };

  http_params.flags           = (uint8_t)flags;
  http_params.ip_address      = (uint8_t *)server_ip;
  http_params.port            = (uint16_t)HTTP_PORT;
  http_params.resource        = (uint8_t *)HTTP_URL;
  http_params.host_name       = (uint8_t *)hostname;
  http_params.extended_header = (uint8_t *)HTTP_EXTENDED_HEADER;
  http_params.user_name       = (uint8_t *)USERNAME;
  http_params.password        = (uint8_t *)PASSWORD;

  status = sl_si91x_http_otaf_v2(&http_params);

  SL_DEBUG_LOG_V2(INFO, "Firmware update status: 0x%" PRIx32 "", (uint32_t)status);
  if (SL_STATUS_IN_PROGRESS == status) {
    const uint32_t start = osKernelGetTickCount();

    while (!response && (osKernelGetTickCount() - start) <= OTAF_TIMEOUT) {
      osThreadYield();
    }

    status = response ? callback_status : SL_STATUS_TIMEOUT;
  }

  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Firmware update FAILED with error: 0x%" PRIx32 "", (uint32_t)status);
    return status;
  } else {
#ifdef AWS_ENABLE
    SL_DEBUG_LOG_V2(INFO, "Completed firmware download using AWS\r\n");
#elif AZURE_ENABLE
    SL_DEBUG_LOG_V2(INFO, "Completed firmware download using AZURE\r\n");
#else
    SL_DEBUG_LOG_V2(INFO, "Completed firmware download using Local Apache Server\r\n");
#endif
    SL_DEBUG_LOG_V2(INFO, "Updating the firmware...\r\n");
  }

#if (FW_UPDATE_TYPE == TA_FW_UPDATE)
  status = sl_net_deinit(SL_NET_WIFI_CLIENT_INTERFACE);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Error while wifi deinit: 0x%" PRIx32 " ", (uint32_t)status);
    return status;
  }
  SL_DEBUG_LOG_V2(INFO, "Wi-Fi Deinit is successful\r\n");

  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &station_init_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to start Wi-Fi client interface: 0x%" PRIx32 "", (uint32_t)status);
    return status;
  }
  SL_DEBUG_LOG_V2(INFO, "Wi-Fi Init success\r\n");

  status = sl_si91x_get_firmware_version(&version);
  VERIFY_STATUS_AND_RETURN(status);
  printf("\r\nFirmware version is: %x%x.%d.%d.%d.%d.%d.%d\r\n",
         version.chip_id,
         version.rom_id,
         version.major,
         version.minor,
         version.security_version,
         version.patch_num,
         version.customer_id,
         version.build_num);
#else
  SL_DEBUG_LOG_V2(INFO, "SoC Soft Reset initiated!\r\n");
  sl_si91x_soc_nvic_reset();
#endif

  return status;
}

sl_status_t set_twt(void)
{
  sl_wifi_performance_profile_v2_t performance_profile = { 0 };
  sl_status_t status                                   = SL_STATUS_OK;

  //! Set TWT Config
  sl_wifi_set_twt_config_callback_v2(twt_callback_handler, NULL);
  if (TWT_AUTO_CONFIG == 1) {
    status = sl_wifi_target_wake_time_auto_selection_v2(&default_twt_config);
  } else {
    performance_profile.twt_request = default_twt_setup_configuration;
    status                          = sl_wifi_enable_target_wake_time(&performance_profile.twt_request);
  }
  VERIFY_STATUS_AND_RETURN(status);
  // A small delay is added so that the asynchronous response from TWT is printed in correct format.
  osDelay(100);

  //! Enable groupcast filter and beacon drop threshold
  sl_wifi_groupcast_filter_config_t groupcast_filter_config = { 0 };
  groupcast_filter_config.enable_bcast_filter               = (uint8_t)BCAST_FILTER_ENABLE;
  groupcast_filter_config.enable_mcast_filter               = (uint8_t)MCAST_FILTER_ENABLE;
  groupcast_filter_config.filter_mode                       = (uint8_t)FILTER_MODE;

  status = sl_wifi_set_groupcast_filter_config(&groupcast_filter_config);
  VERIFY_STATUS_AND_RETURN(status);
  status = sl_wifi_set_beacon_drop_threshold(SL_WIFI_CLIENT_INTERFACE, (uint16_t)BEACON_DROP_THRESHOLD_MS);
  VERIFY_STATUS_AND_RETURN(status);
  SL_DEBUG_LOG_V2(INFO, "Enabled Broadcast Data Filter\r\n");

  //! Apply power save profile
  performance_profile.profile = ASSOCIATED_POWER_SAVE_LOW_LATENCY;
  status                      = sl_wifi_set_performance_profile_v2(&performance_profile);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Powersave Configuration Failed, Error Code : 0x%" PRIx32 "", (uint32_t)status);
    return status;
  } else {
    power_save_enabled = 1;
    SL_DEBUG_LOG_V2(INFO, "Associated Power Save Enabled\r\n");
  }
  return SL_STATUS_OK;
}

static sl_status_t twt_callback_handler(sl_wifi_event_t event,
                                        sl_status_t status_code,
                                        sl_wifi_twt_response_t *result,
                                        uint32_t result_length,
                                        void *arg)
{
  UNUSED_PARAMETER(result_length);
  UNUSED_PARAMETER(arg);

  if (SL_WIFI_CHECK_IF_EVENT_FAILED(event)) {
    return status_code;
  }

  twt_active_session = 0;
  memcpy(&twt_response, result, sizeof(sl_wifi_twt_response_t));
  switch (event) {
    case SL_WIFI_TWT_RESPONSE_EVENT:
      twt_active_session = 1;
      SL_DEBUG_LOG_V2(INFO, "TWT Setup success");
      break;
    case SL_WIFI_TWT_UNSOLICITED_SESSION_SUCCESS_EVENT:
      twt_active_session = 1;
      SL_DEBUG_LOG_V2(INFO, "Unsolicited TWT Setup success");
      break;
    case SL_WIFI_TWT_AP_REJECTED_EVENT:
      SL_DEBUG_LOG_V2(ERROR, "TWT Setup Failed. TWT Setup rejected by AP");
      break;
    case SL_WIFI_TWT_OUT_OF_TOLERANCE_EVENT:
      SL_DEBUG_LOG_V2(ERROR, "TWT Setup Failed. TWT response out of tolerance limits");
      break;
    case SL_WIFI_TWT_RESPONSE_NOT_MATCHED_EVENT:
      SL_DEBUG_LOG_V2(ERROR, "TWT Setup Failed. TWT Response not matched with the request parameters");
      break;
    case SL_WIFI_TWT_UNSUPPORTED_RESPONSE_EVENT:
      SL_DEBUG_LOG_V2(ERROR, "TWT Setup Failed. TWT Response Unsupported");
      break;
    case SL_WIFI_TWT_FAIL_MAX_RETRIES_REACHED_EVENT:
      SL_DEBUG_LOG_V2(ERROR, "TWT Setup Failed. Max retries reached");
      break;
    case SL_WIFI_TWT_INACTIVE_DUE_TO_ROAMING_EVENT:
      SL_DEBUG_LOG_V2(WARN, "TWT session inactive due to roaming");
      break;
    case SL_WIFI_TWT_INACTIVE_DUE_TO_DISCONNECT_EVENT:
      SL_DEBUG_LOG_V2(WARN, "TWT session inactive due to wlan disconnection");
      break;
    case SL_WIFI_TWT_TEARDOWN_SUCCESS_EVENT:
      SL_DEBUG_LOG_V2(INFO, "TWT session teardown success");
      break;
    case SL_WIFI_TWT_AP_TEARDOWN_SUCCESS_EVENT:
      SL_DEBUG_LOG_V2(INFO, "TWT session teardown from AP");
      break;
    case SL_WIFI_TWT_INACTIVE_NO_AP_SUPPORT_EVENT:
      SL_DEBUG_LOG_V2(WARN, "Connected AP Does not support TWT");
      break;
    case SL_WIFI_RESCHEDULE_TWT_SUCCESS_EVENT:
      SL_DEBUG_LOG_V2(INFO, "TWT rescheduled");
      break;
    case SL_WIFI_TWT_INFO_FRAME_EXCHANGE_FAILED_EVENT:
      SL_DEBUG_LOG_V2(WARN, "TWT rescheduling failed due to a failure in the exchange of TWT information frames.");
      break;
    default:
      SL_DEBUG_LOG_V2(ERROR, "TWT Setup Failed.");
  }
  if (event < SL_WIFI_TWT_TEARDOWN_SUCCESS_EVENT) {
    SL_DEBUG_LOG_V2(DEBUG, " wake duration : 0x%X", result->wake_duration);
    SL_DEBUG_LOG_V2(DEBUG, " wake_duration_unit: 0x%X", result->wake_duration_unit);
    SL_DEBUG_LOG_V2(DEBUG, " wake_int_exp : 0x%X", result->wake_int_exp);
    SL_DEBUG_LOG_V2(DEBUG, " negotiation_type : 0x%X\r\n", result->negotiation_type);
    SL_DEBUG_LOG_V2(DEBUG, " wake_int_mantissa : 0x%X", result->wake_int_mantissa);
    SL_DEBUG_LOG_V2(DEBUG, " implicit_twt : 0x%X", result->implicit_twt);
    SL_DEBUG_LOG_V2(DEBUG, " un_announced_twt : 0x%X", result->un_announced_twt);
    SL_DEBUG_LOG_V2(DEBUG, " triggered_twt : 0x%X", result->triggered_twt);
    SL_DEBUG_LOG_V2(DEBUG, " twt_channel : 0x%X", result->twt_channel);
    SL_DEBUG_LOG_V2(DEBUG, " twt_protection : 0x%X", result->twt_protection);
    SL_DEBUG_LOG_V2(DEBUG, " twt_flow_id : 0x%X\r\n", result->twt_flow_id);
  } else if (event < SL_WIFI_TWT_EVENTS_END) {
    SL_DEBUG_LOG_V2(DEBUG, " twt_flow_id : 0x%X\r\n", result->twt_flow_id);
    SL_DEBUG_LOG_V2(DEBUG, " negotiation_type : 0x%X\r\n", result->negotiation_type);
  }
  return SL_STATUS_OK;
}

static sl_status_t http_fw_update_response_handler(sl_wifi_event_t event,
                                                   sl_status_t status_code,
                                                   uint16_t *data,
                                                   uint32_t data_length,
                                                   void *arg)
{
  UNUSED_PARAMETER(data);
  UNUSED_PARAMETER(data_length);
  UNUSED_PARAMETER(arg);

  if (SL_WIFI_CHECK_IF_EVENT_FAILED(event)) {
    response = false;
    return status_code;
  }
  response = true;
  return SL_STATUS_OK;
}
