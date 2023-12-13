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
#include "sl_wifi.h"
#include "sl_wifi_callback_framework.h"
#include "firmware_upgradation.h"
#include "sl_net_dns.h"
#include "sl_utility.h"

// include certificates
#include "aws_starfield_ca.pem.h"
#include "azure_baltimore_ca.pem.h"
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
#define M4_FW_UPDATE 0
#define TA_FW_UPDATE 1

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

#define DNS_TIMEOUT         20000
#define MAX_DNS_RETRY_COUNT 5
#define OTAF_TIMEOUT        600000
#ifdef AWS_ENABLE
//! for example select required flag bits,  Eg:(HTTPS_SUPPORT | HTTPV6 | HTTP_USER_DEFINED_CONTENT_TYPE)
#define FLAGS HTTPS_SUPPORT
//! Server port number
#define HTTP_PORT 443
//! Server URL
#if FW_UPDATE_TYPE
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
#if FW_UPDATE_TYPE
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
  .reserved   = 0,
};

static const sl_wifi_device_configuration_t station_init_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .region_code = US,
  .boot_config = { .oper_mode              = SL_SI91X_CLIENT_MODE,
                   .coex_mode              = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map        = (SL_SI91X_FEAT_SECURITY_PSK | SL_SI91X_FEAT_AGGREGATION),
                   .tcp_ip_feature_bit_map = (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT | SL_SI91X_TCP_IP_FEAT_HTTP_CLIENT
                                              | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID | SL_SI91X_TCP_IP_FEAT_SSL
                                              | SL_SI91X_TCP_IP_FEAT_DNS_CLIENT),
                   .custom_feature_bit_map = SL_SI91X_CUSTOM_FEAT_EXTENTION_VALID,
                   .ext_custom_feature_bit_map =
                     (SL_SI91X_EXT_FEAT_XTAL_CLK | SL_SI91X_EXT_FEAT_UART_SEL_FOR_DEBUG_PRINTS | MEMORY_CONFIG
#ifdef SLI_SI917
                      | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
                      ),
                   .bt_feature_bit_map = 0,
                   .ext_tcp_ip_feature_bit_map =
                     (SL_SI91X_EXT_TCP_IP_FEAT_SSL_HIGH_PERFORMANCE | SL_SI91X_EXT_TCP_IP_SSL_16K_RECORD
                      | SL_SI91X_CONFIG_FEAT_EXTENTION_VALID),
                   .ble_feature_bit_map     = 0,
                   .ble_ext_feature_bit_map = 0,
                   .config_feature_bit_map  = 0 }
};

volatile bool response               = false;
volatile sl_status_t callback_status = SL_STATUS_OK;

/******************************************************
 *               Function Declarations
 ******************************************************/
void application_start(const void *unused);
sl_status_t http_otaf_app();
static sl_status_t http_fw_update_response_handler(sl_wifi_event_t event,
                                                   uint16_t *data,
                                                   uint32_t data_length,
                                                   void *arg);
static sl_status_t clear_and_load_certificates_in_flash(void);
void soft_reset(void);
void on_timeout_callback(void);
void watchdog_timer_init(void);

/******************************************************
 *               Function Definitions
 ******************************************************/

void soft_reset(void)
{
  watchdog_timer_init();
}

void on_timeout_callback(void)
{
  SL_DEBUG_LOG("\r\nIn handler\r\n");
  return;
}

void app_init(const void *unused)
{
  UNUSED_PARAMETER(unused);
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

void application_start(const void *unused)
{
  UNUSED_PARAMETER(unused);
  sl_status_t status = SL_STATUS_OK;

  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &station_init_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    printf("Failed to start Wi-Fi client interface: 0x%lX\r\n", status);
    return;
  }
  printf("\r\nWi-Fi Init is successful\r\n");

#if LOAD_CERTIFICATE
  if (FLAGS & HTTPS_SUPPORT) {
    status = clear_and_load_certificates_in_flash();
    if (status != SL_STATUS_OK) {
      printf("\r\nUnexpected error while loading certificate: 0x%lX\r\n", status);
      return;
    }
  }
#endif

  status = sl_net_up(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to bring Wi-Fi client interface up: 0x%lX\r\n", status);
    return;
  }
  printf("\r\nConnected to Wi-Fi\r\n");

  status = http_otaf_app();
  if (status != SL_STATUS_OK) {
    printf("\r\nFirmware update failed: 0x%lX\r\n", status);
    return;
  }
  printf("\r\nFirmware update is successful\r\n");
}

sl_status_t clear_and_load_certificates_in_flash(void)
{
  sl_status_t status;
  void *cert           = NULL;
  uint32_t cert_length = 0;

#ifdef AWS_ENABLE
  cert        = (void *)aws_starfield_ca;
  cert_length = (sizeof(aws_starfield_ca) - 1);
#elif AZURE_ENABLE
  cert        = (void *)azure_baltimore_ca;
  cert_length = (sizeof(azure_baltimore_ca) - 1);
#else
  cert        = (uint8_t *)cacert;
  cert_length = (sizeof(cacert) - 1);
#endif

  // Load SSL CA certificate
  status = sl_net_set_credential(SL_NET_TLS_SERVER_CREDENTIAL_ID(0), SL_NET_SIGNING_CERTIFICATE, cert, cert_length);
  if (status != SL_STATUS_OK) {
    printf("\r\nLoading TLS CA certificate in to FLASH Failed, Error Code : 0x%lX\r\n", status);
  } else {
    printf("\r\nLoad TLS CA certificate at index %d Success\r\n", 0);
  }

  return status;
}

sl_status_t http_otaf_app()
{
  sl_status_t status = SL_STATUS_OK;
  uint8_t flags      = FLAGS;
  char server_ip[16];

#if FW_UPDATE_TYPE
  sl_wifi_firmware_version_t version = { 0 };
  status                             = sl_wifi_get_firmware_version(&version);
  VERIFY_STATUS_AND_RETURN(status);
  print_firmware_version(&version);
#endif

  sl_wifi_set_callback(SL_WIFI_HTTP_OTA_FW_UPDATE_EVENTS,
                       (sl_wifi_callback_function_t)&http_fw_update_response_handler,
                       NULL);

#if defined(AWS_ENABLE) || defined(AZURE_ENABLE)
  sl_ip_address_t dns_query_rsp = { 0 };
  uint32_t server_address;
  int32_t dns_retry_count = MAX_DNS_RETRY_COUNT;
  do {
    //! Getting IP address of the AWS server using DNS request
    status = sl_net_host_get_by_name((const char *)hostname, DNS_TIMEOUT, SL_NET_DNS_TYPE_IPV4, &dns_query_rsp);
    dns_retry_count--;
  } while ((dns_retry_count != 0) && (status != SL_STATUS_OK));

  if (status != SL_STATUS_OK) {
    printf("\r\nUnexpected error while resolving dns, Error 0x%lX\r\n", status);
    return status;
  }
  printf("\r\nResolving dns Success\r\n");

  server_address = dns_query_rsp.ip.v4.value;
  sprintf((char *)server_ip,
          "%ld.%ld.%ld.%ld",
          server_address & 0x000000ff,
          (server_address & 0x0000ff00) >> 8,
          (server_address & 0x00ff0000) >> 16,
          (server_address & 0xff000000) >> 24);

#ifdef AWS_ENABLE
  printf("\nResolved AWS S3 Bucket IP address = %s\n", server_ip);
  printf("\r\nFirmware download from AWS S3 Bucket is in progress...\r\n");
#elif AZURE_ENABLE
  printf("\nResolved AZURE Blob Storage IP address = %s\n", server_ip);
  printf("\r\nFirmware download from AZURE Blob Storage is in progress...\r\n");
#endif

#else
  strcpy(server_ip, HTTP_SERVER_IP_ADDRESS);
  printf("\r\nLocal Apache Server IP Address: %s\r\n", HTTP_HOSTNAME);
  printf("\r\nFirmware download from Local Apache Server is in progress...\r\n");
#endif
  status = sl_si91x_http_otaf(HTTP_OTAF,
                              (uint8_t)flags,
                              (uint8_t *)server_ip,
                              (uint16_t)HTTP_PORT,
                              (uint8_t *)HTTP_URL,
                              (uint8_t *)hostname,
                              (uint8_t *)HTTP_EXTENDED_HEADER,
                              (uint8_t *)USERNAME,
                              (uint8_t *)PASSWORD,
                              NULL,
                              0);

  printf("\r\nFirmware update status: 0x%lX\r\n", status);

  if (status != SL_STATUS_OK) {
    printf("\r\nFirmware update FAILED with error: 0x%lX\r\n", status);
    return status;
  } else {
#ifdef AWS_ENABLE
    printf("\r\nCompleted firmware download using AWS\r\n");
#elif AZURE_ENABLE
    printf("\r\nCompleted firmware download using AZURE\r\n");
#else
    printf("\r\nCompleted firmware download using Local Apache Server\r\n");
#endif
    printf("\r\nUpdating the firmware...\r\n");
  }

#if FW_UPDATE_TYPE
  status = sl_net_deinit(SL_NET_WIFI_CLIENT_INTERFACE);
  if (status != SL_STATUS_OK) {
    printf("\r\nError while wifi deinit: 0x%lX \r\n", status);
    return status;
  }
  printf("\r\nWi-Fi Deinit is successful\r\n");

  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &station_init_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    printf("Failed to start Wi-Fi client interface: 0x%lX\r\n", status);
    return status;
  }
  printf("\r\nWi-Fi Init success\r\n");

  status = sl_wifi_get_firmware_version(&version);
  VERIFY_STATUS_AND_RETURN(status);
  print_firmware_version(&version);
#else
  printf("\r\nSoC Soft Reset initiated!\r\n");
  sl_si91x_soc_soft_reset();
#endif

  return status;
}

static sl_status_t http_fw_update_response_handler(sl_wifi_event_t event,
                                                   uint16_t *data,
                                                   uint32_t data_length,
                                                   void *arg)
{
  UNUSED_PARAMETER(data);
  UNUSED_PARAMETER(data_length);
  UNUSED_PARAMETER(arg);
  if (SL_WIFI_CHECK_IF_EVENT_FAILED(event)) {
    response = false;
    return SL_STATUS_FAIL;
  }
  response = true;
  return SL_STATUS_OK;
}
