/***************************************************************************/ /**
 * @file
 * @brief SNTP client Example Application
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
#include "sl_tls.h"
#include "sl_utility.h"
#include "cmsis_os2.h"
#include "sl_constants.h"
#include "sl_sntp.h"
#include "sl_wifi.h"
#include "sl_net_dns.h"
#include "string.h"

/******************************************************
 *                    Constants
 ******************************************************/

#define SNTP_METHOD        SL_SNTP_UNICAST_MODE
#define FLAGS              0
#define NTP_SERVER_IP      "0.pool.ntp.org" // Mostly "162.159.200.123"
#define DATA_BUFFER_LENGTH 30
#define SNTP_TIMEOUT       50
#define SNTP_API_TIMEOUT   0
#define ASYNC_WAIT_TIMEOUT 10000

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

static const sl_wifi_device_configuration_t sl_wifi_sntp_client_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .boot_config = { .oper_mode       = SL_SI91X_CLIENT_MODE,
                   .coex_mode       = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map = (SL_SI91X_FEAT_SECURITY_PSK | SL_SI91X_FEAT_AGGREGATION),
                   .tcp_ip_feature_bit_map =
                     (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT | SL_SI91X_TCP_IP_FEAT_DNS_CLIENT | SL_SI91X_TCP_IP_FEAT_SSL
                      | SL_SI91X_TCP_IP_FEAT_SNTP_CLIENT | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID),
                   .custom_feature_bit_map     = (SL_SI91X_FEAT_CUSTOM_FEAT_EXTENTION_VALID),
                   .ext_custom_feature_bit_map = (SL_SI91X_EXT_FEAT_SSL_VERSIONS_SUPPORT | SL_SI91X_EXT_FEAT_XTAL_CLK
                                                  | SL_SI91X_EXT_FEAT_UART_SEL_FOR_DEBUG_PRINTS |
#ifndef RSI_M4_INTERFACE
                                                  RAM_LEVEL_NWP_ALL_MCU_ZERO
#else
                                                  RAM_LEVEL_NWP_ADV_MCU_BASIC
#endif
                                                  ),
                   .bt_feature_bit_map = 0,
                   .ext_tcp_ip_feature_bit_map =
                     (SL_SI91X_EXT_TCP_IP_WINDOW_SCALING | SL_SI91X_EXT_TCP_IP_TOTAL_SELECTS(10)
                      | SL_SI91X_EXT_TCP_IP_FEAT_SSL_THREE_SOCKETS | SL_SI91X_EXT_TCP_IP_FEAT_SSL_MEMORY_CLOUD),
                   .ble_feature_bit_map     = 0,
                   .ble_ext_feature_bit_map = 0,
                   .config_feature_bit_map  = 0 }
};

static uint8_t callback_event = 0;
static sl_status_t cb_status  = SL_STATUS_FAIL;
static uint8_t exec_status    = 0;
static char *event_type[]     = { [SL_SNTP_CLIENT_START]           = "SNTP Client Start",
                                  [SL_SNTP_CLIENT_GET_TIME_DATE]   = "SNTP Client Get Time and Date",
                                  [SL_SNTP_CLIENT_GET_SERVER_INFO] = "SNTP Client Get Server Info",
                                  [SL_SNTP_CLIENT_STOP]            = "SNTP Client Stop" };

/******************************************************
 *               Function Declarations
 ******************************************************/
sl_status_t embedded_sntp_client(void);
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

  printf("SNTP client execution Started \r\n");

  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &sl_wifi_sntp_client_configuration, NULL, NULL);
  if (status != SL_STATUS_OK && status != SL_STATUS_ALREADY_INITIALIZED) {
    printf("Failed to start Wi-Fi client interface: 0x%lx\r\n", status);
    return;
  }

  status = sl_net_up(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID);
  if (status != SL_STATUS_OK) {
    printf("Failed to bring Wi-Fi client interface up: 0x%lx\r\n", status);
    return;
  }

  printf("Wi-Fi client connected\r\n");

  embedded_sntp_client();

  while (1) {
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
    // Let the CPU go to sleep if the system allows it.
    sl_power_manager_sleep();
#else
    osDelay(osWaitForever);
#endif
  }
}

static void print_char_buffer(char *buffer, uint32_t buffer_length)
{
  uint32_t i = 0;

  for (i = 0; i < buffer_length; i++) {
    printf("%c", buffer[i]);
  }

  printf("\r\n");
  return;
}

static void print_server_info(char *buffer, uint32_t buffer_length)
{
  uint32_t i = 0;

  for (i = 0; i < buffer_length; i++) {
    if ((i > 1) && (i < 5)) {
      printf("%u.", buffer[i]);
    } else {
      printf("%u ", buffer[i]);
    }
  }

  printf("\r\n");
  return;
}

static void sntp_client_event_handler(sl_sntp_client_response_t *response,
                                      uint8_t *user_data,
                                      uint16_t user_data_length)
{
  UNUSED_PARAMETER(user_data);
  UNUSED_PARAMETER(user_data_length);

  printf("\nReceived %s SNTP event with status %s\n",
         event_type[response->event_type],
         (0 == response->status) ? "Success" : "Failed");
  if ((SL_SNTP_CLIENT_GET_TIME_DATE == response->event_type) && (0 == response->status)) {
    printf("Response %u bytes data : ", response->data_length);
    print_char_buffer((char *)response->data, response->data_length);
  } else if ((SL_SNTP_CLIENT_GET_SERVER_INFO == response->event_type) && (0 == response->status)) {
    printf("Response %u bytes data : ", response->data_length);
    print_server_info((char *)response->data, response->data_length);
  }

  callback_event = response->event_type;
  cb_status      = response->status;

  if (SL_SNTP_CLIENT_STOP == response->event_type) {
    exec_status = 1;
  }
  return;
}

sl_status_t embedded_sntp_client(void)
{
  uint32_t start = 0;
  sl_status_t status;
  sl_ip_address_t address          = { 0 };
  sl_sntp_client_config_t config   = { 0 };
  uint8_t data[DATA_BUFFER_LENGTH] = { 0 };

  sl_dns_host_get_by_name(NTP_SERVER_IP, 10000, SL_NET_DNS_TYPE_IPV4, &address);
  printf("Ip Address : %u.%u.%u.%u\n",
         address.ip.v4.bytes[0],
         address.ip.v4.bytes[1],
         address.ip.v4.bytes[2],
         address.ip.v4.bytes[3]);

  config.server_host_name = address.ip.v4.bytes;
  config.sntp_method      = SNTP_METHOD;
  config.sntp_timeout     = SNTP_TIMEOUT;
  config.event_handler    = sntp_client_event_handler;
  config.flags            = FLAGS;

  cb_status = SL_STATUS_FAIL;
  status    = sl_sntp_client_start(&config, SNTP_API_TIMEOUT);
  if ((SNTP_API_TIMEOUT == 0) && (SL_STATUS_IN_PROGRESS == status)) {
    start = osKernelGetTickCount();

    while ((SL_SNTP_CLIENT_START != callback_event) && (osKernelGetTickCount() - start) <= ASYNC_WAIT_TIMEOUT) {
      osThreadYield();
    }

    if (cb_status != SL_STATUS_OK) {
      printf("Failed to start Async SNTP client: 0x%lx\r\n", status);
      return cb_status;
    }
  } else {
    if (status != SL_STATUS_OK) {
      printf("Failed to start SNTP client: 0x%lx\r\n", status);
      return status;
    }
  }

  cb_status = SL_STATUS_FAIL;
  status    = sl_sntp_client_get_time_date(data, DATA_BUFFER_LENGTH, SNTP_API_TIMEOUT);
  if ((SNTP_API_TIMEOUT == 0) && (SL_STATUS_IN_PROGRESS == status)) {
    start = osKernelGetTickCount();

    while ((SL_SNTP_CLIENT_GET_TIME_DATE != callback_event) && (osKernelGetTickCount() - start) <= ASYNC_WAIT_TIMEOUT) {
      osThreadYield();
    }

    if (cb_status != SL_STATUS_OK) {
      printf("Failed to get Async date and time from ntp server : 0x%lx\r\n", status);
      return cb_status;
    }
  } else {
    if (status != SL_STATUS_OK) {
      printf("Failed to get date and time from ntp server : 0x%lx\r\n", status);
      return status;
    }
  }

  cb_status = SL_STATUS_FAIL;
  status    = sl_sntp_client_get_server_info(data, DATA_BUFFER_LENGTH, SNTP_API_TIMEOUT);
  if ((SNTP_API_TIMEOUT == 0) && (SL_STATUS_IN_PROGRESS == status)) {
    start = osKernelGetTickCount();

    while ((SL_SNTP_CLIENT_GET_SERVER_INFO != callback_event)
           && (osKernelGetTickCount() - start) <= ASYNC_WAIT_TIMEOUT) {
      osThreadYield();
    }

    if (cb_status != SL_STATUS_OK) {
      printf("Failed to get async ntp server info : 0x%lx\r\n", status);
      return cb_status;
    }
  } else {
    if (status != SL_STATUS_OK) {
      printf("Failed to get ntp server info : 0x%lx\r\n", status);
      return status;
    }
  }

  cb_status = SL_STATUS_FAIL;
  status    = sl_sntp_client_stop(SNTP_API_TIMEOUT);
  if ((SNTP_API_TIMEOUT == 0) && (SL_STATUS_IN_PROGRESS == status)) {
    start = osKernelGetTickCount();

    while ((SL_SNTP_CLIENT_STOP != callback_event) && (osKernelGetTickCount() - start) <= ASYNC_WAIT_TIMEOUT) {
      osThreadYield();
    }

    if (cb_status != SL_STATUS_OK) {
      printf("Failed to stop Async SNTP client: 0x%lx\r\n", status);
      return cb_status;
    }
  } else {
    if (status != SL_STATUS_OK) {
      printf("Failed to stop SNTP client: 0x%lx\r\n", status);
      return status;
    }

    exec_status = 1;
  }

  printf("Done\r\n");

  while (0 == exec_status) {
    osThreadYield();
  }

  printf("SNTP client execution completed \r\n");

  return SL_STATUS_OK;
}
