/***************************************************************************/ /**
 * @file
 * @brief  Twt Use Case Remote App
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_si91x_socket.h"

#ifdef SLI_SI91X_MCU_INTERFACE
#include "rsi_power_save.h"
#include "rsi_wisemcu_hardware_setup.h"
#endif
/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
#define TCP_RECEIVE        1
#define BACK_LOG           1
#define TCP_LISTENING_PORT 5001
#define UDP_LISTENING_PORT 5002

#if TCP_RECEIVE
#define EXPECTED_DATA_SIZE (sizeof("Door lock opened") - 1)
#else
#define EXPECTED_DATA_SIZE 1470
#endif
#define RSI_MAX_TCP_RETRIES  10
#define RECEIVE_DATA_TIMEOUT 20000 // command interval in milli seconds

static const sl_wifi_device_configuration_t twt_client_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .boot_config = { .oper_mode = SL_SI91X_CLIENT_MODE,
                   .coex_mode = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map =
                     (SL_WIFI_FEAT_SECURITY_OPEN | SL_WIFI_FEAT_AGGREGATION | SL_SI91X_FEAT_ULP_GPIO_BASED_HANDSHAKE
#ifdef SLI_SI91X_MCU_INTERFACE
                      | SL_WIFI_FEAT_WPS_DISABLE
#endif
                      ),
                   .tcp_ip_feature_bit_map     = (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT | SL_SI91X_TCP_IP_FEAT_DNS_CLIENT
                                              | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID),
                   .custom_feature_bit_map     = (SL_WIFI_SYSTEM_CUSTOM_FEAT_EXTENSION_VALID),
                   .ext_custom_feature_bit_map = (SL_WIFI_SYSTEM_EXT_FEAT_LOW_POWER_MODE | SL_SI91X_EXT_FEAT_XTAL_CLK
                                                  | SL_SI91X_EXT_FEAT_DISABLE_DEBUG_PRINTS | MEMORY_CONFIG
#ifdef SLI_SI917
                                                  | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
                                                  ),
                   .bt_feature_bit_map         = 0,
                   .ext_tcp_ip_feature_bit_map = SL_SI91X_CONFIG_FEAT_EXTENSION_VALID,
                   .ble_feature_bit_map        = 0,
                   .ble_ext_feature_bit_map    = 0,
                   .config_feature_bit_map     = SL_SI91X_FEAT_SLEEP_GPIO_SEL_BITMAP },
  .ta_pool         = { .tx_ratio_in_buffer_pool = 0, .rx_ratio_in_buffer_pool = 0, .global_ratio_in_buffer_pool = 0 },
  .efuse_data_type = SL_SI91X_EFUSE_MFG_SW_VERSION,
  .nwp_fw_image_number = SL_SI91X_NWP_FW_IMAGE_NUMBER_0
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
};

sl_ip_address_t ip_address           = { 0 };
sl_net_wifi_client_profile_t profile = { 0 };
int tcp_server_socket = -1, tcp_client_socket = -1, udp_server_socket = -1;

uint32_t start_rx = 0, start_rtt = 0, end_rtt = 0;
volatile uint32_t num_pkts = 0;

volatile uint8_t data_sent                 = 0;
volatile uint8_t data_recvd                = 0;
volatile uint64_t num_bytes                = 0;
volatile int8_t rxBuff[EXPECTED_DATA_SIZE] = { 0 };
bool is_remote_terminated                  = 0;
/******************************************************
  *               Function Declarations
  ******************************************************/
void application_start();
sl_status_t create_tcp_server(void);
sl_status_t create_udp_server(void);
sl_status_t send_and_receive_data(void);
void remote_terminate_callback(int socket_id, uint16_t port_number, uint32_t bytes_sent);

/******************************************************
  *               Function Definitions
  ******************************************************/
void remote_terminate_callback(int socket_id, uint16_t port_number, uint32_t bytes_sent)
{
  is_remote_terminated = 1;
  SL_DEBUG_LOG_V2(INFO,
                  "Remote client terminated on socket %d, port %d , bytes_sent %ld",
                  socket_id,
                  port_number,
                  bytes_sent);
}
void data_callback(uint32_t sock_no,
                   uint8_t *buffer,
                   uint32_t length,
                   const sl_si91x_socket_metadata_t *firmware_socket_response)
{
  UNUSED_PARAMETER(sock_no);
  UNUSED_PARAMETER(firmware_socket_response);
  uint16_t i = 0;
  num_bytes += length;
  num_pkts++;
  memcpy((void *)rxBuff, buffer, length);
  SL_DEBUG_LOG_V2(INFO, "Received %ld bytes\r\n", length);
  SL_DEBUG_LOG_V2(DEBUG, "\"");
  for (i = 0; i < length; i++) {
    SL_DEBUG_LOG_V2(DEBUG, "%c", buffer[i]);
  }
  SL_DEBUG_LOG_V2(DEBUG, "\"");
  if (data_recvd == 0) {
    end_rtt = osKernelGetTickCount();
    SL_DEBUG_LOG_V2(INFO, "Overall RTT : 0x%lX\r\n", (end_rtt - start_rtt));
    data_recvd = 1;
  }
}

void app_init(void)
{
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

void application_start()
{
  sl_status_t status;

  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &twt_client_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to start Wi-Fi client interface: 0x%lx\r\n", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Wi-Fi Init Done\r\n");

  status = sl_net_up(SL_NET_WIFI_CLIENT_INTERFACE, 0);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to bring Wi-Fi client interface up: 0x%lx\r\n", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Wi-Fi Client Connected\r\n");

  status = sl_net_get_profile(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID, &profile);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to get client profile: 0x%lx\r\n", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Success to get client profile\r\n");

  ip_address.type = SL_IPV4;
  memcpy(&ip_address.ip.v4.bytes, &profile.ip.ip.v4.ip_address.bytes, sizeof(sl_ipv4_address_t));
  print_sl_ip_address(&ip_address);

  status = create_tcp_server();
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Error while creating TCP server: 0x%lx \r\n", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "TCP Server is running\r\n");

#if !TCP_RECEIVE
  status = create_udp_server();
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Error while creating UDP server: 0x%lx \r\n", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "UDP Server is running\r\n");
#endif

  status = send_and_receive_data();
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Send and Receive Data fail: 0x%lx \r\n", status);
    return;
  }
}

sl_status_t create_tcp_server(void)
{
  uint8_t max_tcp_retry             = RSI_MAX_TCP_RETRIES;
  struct sockaddr_in server_address = { 0 };

  int socket_return_value = 0;

  sl_si91x_set_remote_termination_callback(remote_terminate_callback);

  tcp_server_socket = sl_si91x_socket_async(AF_INET, SOCK_STREAM, IPPROTO_TCP, &data_callback);
  if (tcp_server_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "TCP Socket creation failed with BSD error: %d\r\n", errno);
    return SL_STATUS_FAIL;
  }
  SL_DEBUG_LOG_V2(INFO, "TCP Server Socket ID : %d\r\n", tcp_server_socket);

  socket_return_value =
    sl_si91x_setsockopt(tcp_server_socket, SOL_SOCKET, SL_SI91X_SO_MAXRETRY, &max_tcp_retry, sizeof(max_tcp_retry));
  if (socket_return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "TCP Set Socket option failed with BSD error: %d\r\n", errno);
    close(tcp_server_socket);
    return SL_STATUS_FAIL;
  }
  SL_DEBUG_LOG_V2(INFO, "TCP Set Sock Option: Max retry set : %d\r\n", max_tcp_retry);

  server_address.sin_family = AF_INET;
  server_address.sin_port   = TCP_LISTENING_PORT;

  socket_return_value =
    sl_si91x_bind(tcp_server_socket, (struct sockaddr *)&server_address, sizeof(struct sockaddr_in));
  if (socket_return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "TCP Socket bind failed with BSD error: %d\r\n", errno);
    close(tcp_server_socket);
    return SL_STATUS_FAIL;
  }
  SL_DEBUG_LOG_V2(INFO, "TCP Bind Success\r\n");

  socket_return_value = sl_si91x_listen(tcp_server_socket, BACK_LOG);
  if (socket_return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "TCP Socket listen failed with BSD error: %d\r\n", errno);
    close(tcp_server_socket);
    return SL_STATUS_FAIL;
  }
  SL_DEBUG_LOG_V2(INFO, "TCP Listening on Local Port : %d\r\n", TCP_LISTENING_PORT);

  tcp_client_socket = sl_si91x_accept(tcp_server_socket, NULL, 0);
  if (tcp_client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket accept failed with BSD error: %d\r\n", errno);
    close(tcp_server_socket);
    return SL_STATUS_FAIL;
  }
  SL_DEBUG_LOG_V2(INFO, "TCP Socket Accept Success\r\n");
  SL_DEBUG_LOG_V2(INFO, "TCP Client Socket ID : %d\r\n", tcp_client_socket);
  return SL_STATUS_OK;
}

sl_status_t create_udp_server(void)
{
  struct sockaddr_in server_address = { 0 };
  int socket_return_value           = 0;

  udp_server_socket = sl_si91x_socket_async(AF_INET, SOCK_DGRAM, IPPROTO_UDP, &data_callback);
  if (udp_server_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "UDP Socket creation failed with BSD error: %d\r\n", errno);
    return SL_STATUS_FAIL;
  }
  SL_DEBUG_LOG_V2(INFO, "UDP Server Socket ID : %d\r\n", udp_server_socket);

  server_address.sin_family = AF_INET;
  server_address.sin_port   = UDP_LISTENING_PORT;

  socket_return_value =
    sl_si91x_bind(udp_server_socket, (struct sockaddr *)&server_address, sizeof(struct sockaddr_in));
  if (socket_return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "UDP Socket bind failed with BSD error: %d\r\n", errno);
    close(udp_server_socket);
    return SL_STATUS_FAIL;
  }
  SL_DEBUG_LOG_V2(INFO, "UDP Bind Success\r\n");

  return SL_STATUS_OK;
}

sl_status_t send_and_receive_data(void)
{
  int status = SL_STATUS_OK;
  while (1) {
    if (data_sent == 0) {
      start_rtt  = osKernelGetTickCount();
      data_sent  = 1;
      data_recvd = 0;
      status     = sl_si91x_send(tcp_client_socket, (uint8_t *)"Stream Data", (sizeof("Stream Data") - 1), 0);
      SL_DEBUG_LOG_V2(INFO, "Sending Command\r\n");
      if (status < 0) {
        data_sent = 0;
        sl_si91x_shutdown(tcp_client_socket, SHUTDOWN_BY_ID);
        SL_DEBUG_LOG_V2(ERROR, "Failed to Send data to TCP Server, Error Code : 0x%x\r\n", (unsigned int)status);
        return SL_STATUS_FAIL;
      }
    }

    SL_DEBUG_LOG_V2(INFO, "Command Sent. Listening for data\r\n");
    start_rx = osKernelGetTickCount();
    SL_DEBUG_LOG_V2(DEBUG, "Start time TX: 0x%lX\r\n", start_rx);

    do {
      osThreadYield();
    } while (((osKernelGetTickCount() - start_rx) < RECEIVE_DATA_TIMEOUT) && !(is_remote_terminated));

    start_rtt = 0;
    SL_DEBUG_LOG_V2(INFO, "Number of packets received : 0x%lX\r\n", num_pkts);
    SL_DEBUG_LOG_V2(INFO, "Number of bytes received : 0x%lx\r\n", (long)num_bytes);
    num_bytes = 0;
    num_pkts  = 0;
    SL_DEBUG_LOG_V2(INFO, "Data Reception Completed\r\n");
    data_sent = 0;
  }
  return SL_STATUS_OK;
}
