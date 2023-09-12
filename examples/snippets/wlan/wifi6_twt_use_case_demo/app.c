/***************************************************************************/ /**
 * @file
 * @brief  Twt Use Case Remote App
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
#include <string.h>
#include <stdint.h>
#include "errno.h"
#include "sl_wifi_callback_framework.h"
#include "sl_status.h"
#include "sl_board_configuration.h"
#include "cmsis_os2.h"
#include "string.h"
#include "sl_wifi.h"
#include "socket.h"
#include "sl_utility.h"
#include "sl_net.h"
#include "sl_ip_types.h"
#include "sl_string.h"
#include "sl_net_si91x.h"
#include "sl_net_wifi_types.h"
#include "sl_si91x_socket_utility.h"
#include "sl_si91x_socket_constants.h"
#include "rsi_board.h"
#include "sl_si91x_driver.h"
#include "sl_si91x_socket.h"

#ifdef RSI_M4_INTERFACE
#include "rsi_power_save.h"
#include "rsi_wisemcu_hardware_setup.h"
#include "rsi_m4.h"
#endif
/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
#define SEND_TCP_DATA              1
#define BACK_LOG                   1
#define TCP_SERVER_PORT            5001
#define UDP_SERVER_PORT            5002
#define SERVER_IP_UDP              "192.168.50.32"
#define SERVER_IP_TCP              "192.168.1.5"
#define NUMBER_OF_PACKETS          1
#define SL_HIGH_PERFORMANCE_SOCKET BIT(7)
#define TWT_AUTO_CONFIG            1
#define TWT_SCAN_TIMEOUT           10000
#define ENABLE_POWER_SAVE          1
#define DEBUG_UART                 1

#if SEND_TCP_DATA
#define BUF_SIZE 1460
#else
#define BUF_SIZE 1470
#endif

// Use case based TWT selection params
#define DEVICE_AVG_THROUGHPUT                20000
#define ESTIMATE_EXTRA_WAKE_DURATION_PERCENT 0
#define TWT_TOLERABLE_DEVIATION              10
#define TWT_DEFAULT_WAKE_INTERVAL_MS         1024     // in milli seconds
#define TWT_DEFAULT_WAKE_DURATION_MS         8        // in milli seconds
#define MAX_TX_AND_RX_LATENCY_LIMIT          22118400 // 6hrs in milli seconds
#define MAX_BEACON_WAKE_UP_AFTER_SP \
  2 // The number of beacons after the service period completion for which the module wakes up and listens for any pending RX.

#ifdef RSI_M4_INTERFACE
#ifdef COMMON_FLASH_EN
#ifdef CHIP_917B0
#define IVT_OFFSET_ADDR 0x81C2000 /*<!Application IVT location VTOR offset>          B0 common flash Board*/
#else
#define IVT_OFFSET_ADDR 0x8212000 /*<!Application IVT location VTOR offset>          A0 Common flash Board*/
#endif
#else
#define IVT_OFFSET_ADDR \
  0x8012000 /*<!Application IVT location VTOR offset>          Dual Flash  (both A0 and B0) Board*/
#endif
#ifdef CHIP_917B0
#define WKP_RAM_USAGE_LOCATION \
  0x24061EFC /*<!Bootloader RAM usage location upon wake up  */ // B0 Boards (common flash & Dual flash)
#else
#define WKP_RAM_USAGE_LOCATION \
  0x24061000 /*<!Bootloader RAM usage location upon wake up  */ // A0 Boards (common flash & Dual flash)
#endif
#endif

#define WIRELESS_WAKEUP_IRQHandler           NPSS_TO_MCU_WIRELESS_INTR_IRQn
#define WIRELESS_WAKEUP_IRQHandler_Periority 8

static const sl_wifi_device_configuration_t sl_wifi_twt_client_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .boot_config = { .oper_mode = SL_SI91X_CLIENT_MODE,
                   .coex_mode = SL_SI91X_WLAN_MODE,
                   .feature_bit_map =
                     (SL_SI91X_FEAT_SECURITY_OPEN | SL_SI91X_FEAT_AGGREGATION | SL_SI91X_FEAT_ULP_GPIO_BASED_HANDSHAKE
#ifdef RSI_M4_INTERFACE
                      | SL_SI91X_FEAT_WPS_DISABLE
#endif
                      ),
                   .tcp_ip_feature_bit_map     = (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT | SL_SI91X_TCP_IP_FEAT_DNS_CLIENT
                                              | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID | SL_SI91X_TCP_IP_FEAT_SSL),
                   .custom_feature_bit_map     = (SL_SI91X_FEAT_CUSTOM_FEAT_EXTENTION_VALID),
                   .ext_custom_feature_bit_map = (SL_SI91X_EXT_FEAT_LOW_POWER_MODE | SL_SI91X_EXT_FEAT_XTAL_CLK
                                                  | SL_SI91X_EXT_FEAT_DISABLE_DEBUG_PRINTS |
#ifndef RSI_M4_INTERFACE
                                                  RAM_LEVEL_NWP_ALL_MCU_ZERO
#else
                                                  RAM_LEVEL_NWP_ADV_MCU_BASIC
#endif
#ifdef CHIP_917
                                                  | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
                                                  ),
                   .bt_feature_bit_map = SL_SI91X_BT_RF_TYPE,
                   .ext_tcp_ip_feature_bit_map =
                     SL_SI91X_CONFIG_FEAT_EXTENTION_VALID
                     | (SL_SI91X_EXT_TCP_IP_FEAT_SSL_HIGH_PERFORMANCE | SL_SI91X_EXT_TCP_IP_SSL_16K_RECORD),
                   .ble_feature_bit_map     = 0,
                   .ble_ext_feature_bit_map = 0,
                   .config_feature_bit_map  = SL_SI91X_FEAT_SLEEP_GPIO_SEL_BITMAP }
};

/******************************************************
 *               Variable Definitions
 ******************************************************/
const osThreadAttr_t thread_attributes = {
  .name       = "application_thread",
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = 3072,
  .priority   = 0,
  .tz_module  = 0,
  .reserved   = 0,
};

sl_ip_address_t ip_address           = { 0 };
sl_net_wifi_client_profile_t profile = { 0 };
int tcp_client_socket = -1, udp_client_socket = -1;

uint32_t start_rx = 0, start_rtt = 0, end_rtt = 0;
volatile uint32_t num_pkts  = 0;
int32_t packet_count        = 0;
volatile uint8_t data_sent  = 0;
volatile uint8_t data_recvd = 0;
volatile uint64_t num_bytes = 0;
int8_t send_buf[BUF_SIZE];

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

sl_wifi_twt_selection_t default_twt_selection_configuration = {
  .twt_enable                            = 1,
  .average_tx_throughput                 = 1000,
  .tx_latency                            = 0,
  .rx_latency                            = 5000,
  .device_average_throughput             = DEVICE_AVG_THROUGHPUT,
  .estimated_extra_wake_duration_percent = ESTIMATE_EXTRA_WAKE_DURATION_PERCENT,
  .twt_tolerable_deviation               = TWT_TOLERABLE_DEVIATION,
  .default_wake_interval_ms              = TWT_DEFAULT_WAKE_INTERVAL_MS,
  .default_minimum_wake_duration_ms      = TWT_DEFAULT_WAKE_DURATION_MS,
  .beacon_wake_up_count_after_sp         = MAX_BEACON_WAKE_UP_AFTER_SP
};

volatile bool twt_results_complete   = false;
volatile sl_status_t callback_status = SL_STATUS_OK;

/******************************************************
  *               Function Declarations
  ******************************************************/
void application_start();
sl_status_t create_tcp_socket(void);
sl_status_t send_udp_data(void);
sl_status_t receive_and_send_data(void);
static sl_status_t twt_callback_handler(sl_wifi_event_t event,
                                        sl_si91x_twt_response_t *result,
                                        uint32_t result_length,
                                        void *arg);
void M4_sleep_wakeup(void);

/******************************************************
  *               Function Definitions
  ******************************************************/
void data_callback(uint32_t sock_no, uint8_t *buffer, uint32_t length)
{
  UNUSED_PARAMETER(sock_no);
  int i;
#ifdef DEBUG_UART
  fpuInit();
  /*Initialize UART after wake up*/
  DEBUGINIT();
#endif
  printf("\r\n RX Packet Received\r\n ");
  printf("\r\n RX: LEN %ld\r\n ", length);
  printf("\r\n Data:\r\n ");

  for (i = 0; i < length; i++) {
    printf("%c", buffer[i]);
  }
  printf("\r\n");
  data_sent  = 0;
  data_recvd = 1;
}

void app_init(const void *unused)
{
  UNUSED_PARAMETER(unused);
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

void application_start()
{
  sl_status_t status;
  sl_wifi_performance_profile_t performance_profile = { 0 };
  sl_wifi_version_string_t version                  = { 0 };
  sl_mac_address_t mac_addr                         = { 0 };

  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &sl_wifi_twt_client_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    printf("Failed to start Wi-Fi client interface: 0x%lx\r\n", status);
    return;
  }
  printf("Wi-Fi Init Done\r\n");

#ifdef RSI_M4_INTERFACE
  uint8_t xtal_enable = 1;
  status              = sl_si91x_m4_ta_secure_handshake(SI91X_ENABLE_XTAL, 1, &xtal_enable, 0, NULL);
  if (status != SL_STATUS_OK) {
    printf("Failed to bring m4_ta_secure_handshake: 0x%lx\r\n", status);
    return;
  }
  printf("m4_ta_secure_handshake Success\r\n");
#endif

  status = sl_wifi_get_firmware_version(&version);
  if (status != SL_STATUS_OK) {
    printf("Failed to bring m4_ta_secure_handshake: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nFirmware version before update: %s\r\n", version.version);

  status = sl_net_up(SL_NET_WIFI_CLIENT_INTERFACE, 0);
  if (status != SL_STATUS_OK) {
    printf("Failed to bring Wi-Fi client interface up: 0x%lx\r\n", status);
    return;
  }
  printf("Wi-Fi Client Connected\r\n");

  status = sl_wifi_get_mac_address(SL_WIFI_CLIENT_INTERFACE, &mac_addr);
  if (status != SL_STATUS_OK) {
    printf("Failed to get MAC address: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nMAC Address: %x:%x:%x:%x:%x:%x\r\n",
         mac_addr.octet[0],
         mac_addr.octet[1],
         mac_addr.octet[2],
         mac_addr.octet[3],
         mac_addr.octet[4],
         mac_addr.octet[5]);

  status = sl_net_get_profile(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID, &profile);
  if (status != SL_STATUS_OK) {
    printf("Failed to get firmware version: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nSuccess to get client profile\r\n");

  ip_address.type = SL_IPV4;
  memcpy(&ip_address.ip.v4.bytes, &profile.ip.ip.v4.ip_address.bytes, sizeof(sl_ipv4_address_t));
  print_sl_ip_address(&ip_address);

#if !SEND_TCP_DATA
  status = send_udp_data();
  if (status != SL_STATUS_OK) {
    printf("\r\nError while doing UDP TX: 0x%lx \r\n", status);
    return;
  }
  printf("\r\nUDP TX complete\r\n");
#endif

  status = create_tcp_socket();
  if (status != SL_STATUS_OK) {
    printf("\r\nError while creating TCP Socket: 0x%lx \r\n", status);
    return;
  }
  printf("\r\nTCP Socket Creation done\r\n");

  sl_wifi_set_twt_config_callback(twt_callback_handler, NULL);
  if (TWT_AUTO_CONFIG == 1) {
    performance_profile.twt_selection = default_twt_selection_configuration;
    status                            = sl_wifi_target_wake_time_auto_selection(&performance_profile.twt_selection);
  } else {
    performance_profile.twt_request = default_twt_setup_configuration;
    status                          = sl_wifi_enable_target_wake_time(&performance_profile.twt_request);
  }
  if (SL_STATUS_IN_PROGRESS == status) {
    const uint32_t start = osKernelGetTickCount();

    while (!twt_results_complete && (osKernelGetTickCount() - start) <= TWT_SCAN_TIMEOUT) {
      osThreadYield();
    }

    status = twt_results_complete ? callback_status : SL_STATUS_TIMEOUT;
  }
  if (status != SL_STATUS_OK) {
    printf("\r\nError while configuring TWT: 0x%lx \r\n", status);
    return;
  }

  status = sl_wifi_filter_broadcast(5000, 1, 1);
  if (status != SL_STATUS_OK) {
    printf("\r\nBroadcast Data Filter Failed: 0x%lx \r\n", status);
    return;
  }
  printf("\r\nEnabled Broadcast Data Filter\n");

#if ENABLE_POWER_SAVE
  performance_profile.profile = ASSOCIATED_POWER_SAVE;
  status                      = sl_wifi_set_performance_profile(&performance_profile);
  if (status != SL_STATUS_OK) {
    printf("\r\nPowersave Configuration Failed, Error Code : 0x%lX\r\n", status);
    return;
  }
  printf("\r\nAssociated Power Save Enabled\n");
#endif

  status = receive_and_send_data();
  if (status != SL_STATUS_OK) {
    printf("\r\nSend and Receive Data fail: 0x%lx \r\n", status);
    return;
  }
}

sl_status_t send_udp_data(void)
{
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);
  sl_status_t status                = SL_STATUS_OK;
  int32_t packet_count              = 0;

  server_address.sin_family = AF_INET;
  server_address.sin_port   = UDP_SERVER_PORT;
  sl_net_inet_addr(SERVER_IP_UDP, &server_address.sin_addr.s_addr);

  udp_client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (udp_client_socket < 0) {
    printf("\r\nSocket creation failed with BSD error: %d\r\n", errno);
    return SL_STATUS_FAIL;
  }
  printf("\r\nSocket ID : %d\r\n", udp_client_socket);

  printf("\r\nUDP TX start\r\n");
  while (packet_count < NUMBER_OF_PACKETS) {
    status = sendto(udp_client_socket,
                    (int8_t *)"Hello from UDP client!!!",
                    (sizeof("Hello from UDP client!!!") - 1),
                    0,
                    (struct sockaddr *)&server_address,
                    socket_length);
    if (status < 0) {
      printf("\r\nFailed to send data to UDP Server, Error Code : 0x%lX\r\n", status);
      close(udp_client_socket);
    }
    packet_count++;
  }
  packet_count = 0;
  return SL_STATUS_OK;
}

sl_status_t create_tcp_socket(void)
{
  struct sockaddr_in server_address = { 0 };
  uint8_t high_performance_socket   = SL_HIGH_PERFORMANCE_SOCKET;

  int socket_return_value = 0;

  tcp_client_socket = sl_si91x_socket_async(AF_INET, SOCK_STREAM, IPPROTO_TCP, &data_callback);
  if (tcp_client_socket < 0) {
    printf("\r\nTCP Socket creation failed with BSD error: %d\r\n", errno);
    return SL_STATUS_FAIL;
  }
  printf("\r\nTCP Client Socket ID : %d\r\n", tcp_client_socket);

  socket_return_value = sl_si91x_setsockopt(tcp_client_socket,
                                            SOL_SOCKET,
                                            sl_si91x_SO_HIGH_PERFORMANCE_SOCKET,
                                            &high_performance_socket,
                                            sizeof(high_performance_socket));
  if (socket_return_value < 0) {
    printf("\r\nTCP Set Socket option failed with BSD error: %d\r\n", errno);
    close(tcp_client_socket);
    return SL_STATUS_FAIL;
  }
  printf("\r\nSet Socket Option Success\r\n");

  server_address.sin_family = AF_INET;
  server_address.sin_port   = TCP_SERVER_PORT;
  sl_net_inet_addr(SERVER_IP_TCP, &server_address.sin_addr.s_addr);

  socket_return_value = connect(tcp_client_socket, (struct sockaddr *)&server_address, sizeof(struct sockaddr_in));
  if (socket_return_value < 0) {
    printf("\r\nSocket Connect failed with BSD error: %d\r\n", errno);
    close(tcp_client_socket);
    return SL_STATUS_FAIL;
  }
  printf("\r\nSocket connected to TCP server\r\n");

  return SL_STATUS_OK;
}

static sl_status_t twt_callback_handler(sl_wifi_event_t event,
                                        sl_si91x_twt_response_t *result,
                                        uint32_t result_length,
                                        void *arg)
{
  UNUSED_PARAMETER(result_length);
  UNUSED_PARAMETER(arg);

  if (CHECK_IF_EVENT_FAILED(event)) {
    twt_results_complete = true;
    return SL_STATUS_FAIL;
  }

  switch (event) {
    case SL_WIFI_TWT_RESPONSE_EVENT:
      printf("\r\nTWT Setup success");
      break;
    case SL_WIFI_TWT_UNSOLICITED_SESSION_SUCCESS_EVENT:
      printf("\r\nUnsolicited TWT Setup success");
      break;
    case SL_WIFI_TWT_AP_REJECTED_EVENT:
      printf("\r\nTWT Setup Failed. TWT Setup rejected by AP");
      break;
    case SL_WIFI_TWT_OUT_OF_TOLERANCE_EVENT:
      printf("\r\nTWT Setup Failed. TWT response out of tolerance limits");
      break;
    case SL_WIFI_TWT_RESPONSE_NOT_MATCHED_EVENT:
      printf("\r\nTWT Setup Failed. TWT Response not matched with the request parameters");
      break;
    case SL_WIFI_TWT_UNSUPPORTED_RESPONSE_EVENT:
      printf("\r\nTWT Setup Failed. TWT Response Unsupported");
      break;
    case SL_WIFI_TWT_FAIL_MAX_RETRIES_REACHED_EVENT:
      printf("\r\nTWT Setup Failed. Max retries reached");
      break;
    case SL_WIFI_TWT_INACTIVE_DUE_TO_ROAMING_EVENT:
      printf("\r\nTWT session inactive due to roaming");
      break;
    case SL_WIFI_TWT_INACTIVE_DUE_TO_DISCONNECT_EVENT:
      printf("\r\nTWT session inactive due to wlan disconnection");
      break;
    case SL_WIFI_TWT_TEARDOWN_SUCCESS_EVENT:
      printf("\r\nTWT session teardown success");
      break;
    case SL_WIFI_TWT_AP_TEARDOWN_SUCCESS_EVENT:
      printf("\r\nTWT session teardown from AP");
      break;
    case SL_WIFI_TWT_INACTIVE_NO_AP_SUPPORT_EVENT:
      printf("\r\nConnected AP Does not support TWT");
      break;
    default:
      printf("TWT Setup Failed.");
  }
  if (event < SL_WIFI_TWT_TEARDOWN_SUCCESS_EVENT) {
    printf("\r\n wake duration : 0x%X", result->wake_duration);
    printf("\r\n wake_duration_unit: 0x%X", result->wake_duration_unit);
    printf("\r\n wake_int_exp : 0x%X", result->wake_int_exp);
    printf("\r\n negotiation_type : 0x%X", result->negotiation_type);
    printf("\r\n wake_int_mantissa : 0x%X", result->wake_int_mantissa);
    printf("\r\n implicit_twt : 0x%X", result->implicit_twt);
    printf("\r\n un_announced_twt : 0x%X", result->un_announced_twt);
    printf("\r\n triggered_twt : 0x%X", result->triggered_twt);
    printf("\r\n twt_channel : 0x%X", result->twt_channel);
    printf("\r\n twt_protection : 0x%X", result->twt_protection);
    printf("\r\n twt_flow_id : 0x%X\r\n", result->twt_flow_id);
  } else if (event <= SL_WIFI_TWT_AP_TEARDOWN_SUCCESS_EVENT) {
    printf("\r\n twt_flow_id : 0x%X", result->twt_flow_id);
    printf("\r\n negotiation_type : 0x%X\r\n", result->negotiation_type);
  }
  twt_results_complete = true;
  return SL_STATUS_OK;
}

sl_status_t receive_and_send_data(void)
{
  sl_status_t status = SL_STATUS_OK;
  printf("\r\nTCP RX started \r\n");
  while (1) {

    while (!data_recvd) {
      osThreadYield();
#ifdef RSI_M4_INTERFACE
      if (!(P2P_STATUS_REG & TA_wakeup_M4)) {
        if (!data_recvd) {
          //! Keep M4 in sleep
          M4_sleep_wakeup();
        }
      }
#endif
    }
    printf("\r\nTCP RX completed \r\n");

#if !SEND_TCP_DATA
    struct sockaddr_in server_address = { 0 };
    server_address.sin_family         = AF_INET;
    server_address.sin_port           = UDP_SERVER_PORT;
    sl_net_inet_addr(SERVER_IP_UDP, &server_address.sin_addr.s_addr);
    if (data_recvd) {
      data_sent    = 1;
      packet_count = 0;
      printf("\r\nUDP TX start\r\n");

      while (packet_count < NUMBER_OF_PACKETS) {
        for (uint16_t i = 0; i < BUF_SIZE; i++) {
          send_buf[i] = i;
        }
        status = sendto(udp_client_socket,
                        send_buf,
                        BUF_SIZE,
                        0,
                        (struct sockaddr *)&server_address,
                        sizeof(struct sockaddr_in));
        if (status < 0) {
          data_sent = 0;
          printf("\r\nFailed to send data to UDP Server, Error Code : 0x%lX\r\n", status);
          close(udp_client_socket);
        }
        packet_count++;
      }
    }
#else
    osDelay(1000);
    packet_count = 0;
    if (!data_sent) {
      data_sent = 1;
      while (packet_count < NUMBER_OF_PACKETS) {
        status =
          send(tcp_client_socket, (int8_t *)"Hello from TCP client!!!!", (sizeof("Hello from TCP client!!!!") - 1), 0);
        if (status < 0) {
          data_sent = 0;
          printf("\r\nFailed to send data to TCP Server, Error Code : 0x%lX\r\n", status);
          close(udp_client_socket);
        }
        packet_count++;
      }
    }
#endif
    data_recvd = 0;
  }
  return SL_STATUS_OK;
}

#ifdef RSI_M4_INTERFACE

void M4_sleep_wakeup(void)
{
  RSI_PS_SetWkpSources(WIRELESS_BASED_WAKEUP);
  NVIC_SetPriority(WIRELESS_WAKEUP_IRQHandler, WIRELESS_WAKEUP_IRQHandler_Periority);
  /* Enable NVIC */
  NVIC_EnableIRQ(WIRELESS_WAKEUP_IRQHandler);
#ifndef FLASH_BASED_EXECUTION_ENABLE
  /* LDOSOC Default Mode needs to be disabled */
  sl_si91x_disable_default_ldo_mode();

  /* bypass_ldorf_ctrl needs to be enabled */
  sl_si91x_enable_bypass_ldo_rf();

  sl_si91x_disable_flash_ldo();

  /* Configure RAM Usage and Retention Size */
  sl_si91x_configure_ram_retention(WISEMCU_48KB_RAM_IN_USE, WISEMCU_RETAIN_DEFAULT_RAM_DURING_SLEEP);

  /* Trigger M4 Sleep */
  sl_si91x_trigger_sleep(SLEEP_WITH_RETENTION,
                         DISABLE_LF_MODE,
                         0,
                         (uint32_t)RSI_PS_RestoreCpuContext,
                         0,
                         RSI_WAKEUP_WITH_RETENTION_WO_ULPSS_RAM);

#else
  /* Configure RAM Usage and Retention Size */
  sl_si91x_configure_ram_retention(WISEMCU_192KB_RAM_IN_USE, WISEMCU_RETAIN_DEFAULT_RAM_DURING_SLEEP);

  printf("\r\nM4 in sleep\r\n");

  /* Trigger M4 Sleep*/
  sl_si91x_trigger_sleep(SLEEP_WITH_RETENTION,
                         DISABLE_LF_MODE,
                         WKP_RAM_USAGE_LOCATION,
                         (uint32_t)RSI_PS_RestoreCpuContext,
                         IVT_OFFSET_ADDR,
                         RSI_WAKEUP_FROM_FLASH_MODE);
  /* Enable M4_TA interrupt */
  sli_m4_ta_interrupt_init();

  //  /*Start of M4 init after wake up  */
  printf("\r\nM4 Wake Up\r\n");
#endif
#ifdef DEBUG_UART
  fpuInit();
  DEBUGINIT();
#endif
}
#endif

#ifdef RSI_M4_INTERFACE
void IRQ026_Handler()
{
  RSI_PS_GetWkpUpStatus();
  /*Clear interrupt */
  ps_clr_wkp_up_status(NPSS_TO_MCU_WIRELESS_INTR);
  return;
}
#endif
