/***************************************************************************/ /**
 * @file
 * @brief WLAN Throughput from PSRAM Example Application
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

#include "sl_board_configuration.h"
#include "cmsis_os2.h"
#include "sl_wifi.h"
#include "sl_net.h"
#include "sl_tls.h"
#include "socket.h"
#include "sl_utility.h"
#include "errno.h"
#include <string.h>
#include "sl_si91x_driver.h"
#include "sl_net_wifi_types.h"
#include "sl_si91x_socket_utility.h"
#include "sl_si91x_socket_constants.h"
#include "sl_si91x_socket.h"
#ifdef RSI_M4_INTERFACE
#include "rsi_rom_clks.h"
#endif

/******************************************************
 *                      Macros
 ******************************************************/
// Type of throughput
#define UDP_TX 0
#define UDP_RX 1
#define TCP_TX 2
#define TCP_RX 3
#define TLS_TX 4
#define TLS_RX 5

// Throughput measurement type
#define THROUGHPUT_TYPE UDP_TX

// Type of Socket used. Synchronous = 0, Asynchronous = 1
#define SOCKET_ASYNC_FEATURE 1

// Memory length for send buffer
#define TCP_BUFFER_SIZE 1460
#define UDP_BUFFER_SIZE 1470
#define TLS_BUFFER_SIZE 1370
#if ((THROUGHPUT_TYPE == UDP_RX) || (THROUGHPUT_TYPE == UDP_TX))
#define BUFFER_SIZE UDP_BUFFER_SIZE
#elif ((THROUGHPUT_TYPE == TCP_RX) || (THROUGHPUT_TYPE == TCP_TX))
#define BUFFER_SIZE TCP_BUFFER_SIZE
#elif ((THROUGHPUT_TYPE == TLS_RX) || (THROUGHPUT_TYPE == TLS_TX))
#include "cacert.pem.h"
#define BUFFER_SIZE TLS_BUFFER_SIZE
#endif

#define SERVER_IP "192.168.30.117"

// Server port number
#if ((THROUGHPUT_TYPE == TLS_RX) || (THROUGHPUT_TYPE == TLS_TX))
#define SERVER_PORT 443
#else
#define SERVER_PORT 5000
#endif

// Module port number
#define LISTENING_PORT 5005
#define BACK_LOG       1

#define BYTES_TO_SEND          (1 << 29)              //512MB
#define BYTES_TO_RECEIVE       (1 << 20)              //1MB
#define BYTES_TO_RECEIVE_ASYNC 16                     //16MB
#define TEST_TIMEOUT           (10000 * tick_count_s) //10sec

#define SL_HIGH_PERFORMANCE_SOCKET BIT(7)

#ifdef RSI_M4_INTERFACE
#define SOC_PLL_REF_FREQUENCY 40000000  /*<! PLL input REFERENCE clock 40MHZ */
#define PS4_SOC_FREQ          119000000 /*<! PLL out clock 100MHz            */
#endif

/******************************************************
 *                    Constants
 ******************************************************/

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
uint8_t data_buffer[BUFFER_SIZE];
sl_ip_address_t ip_address           = { 0 };
sl_net_wifi_client_profile_t profile = { 0 };

static const sl_wifi_device_configuration_t sl_wifi_throughput_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .region_code = US,
  .boot_config = { .oper_mode = SL_SI91X_CLIENT_MODE,
                   .coex_mode = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map =
                     (SL_SI91X_FEAT_SECURITY_OPEN | SL_SI91X_FEAT_AGGREGATION | SL_SI91X_FEAT_WPS_DISABLE),
                   .tcp_ip_feature_bit_map = (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT | SL_SI91X_TCP_IP_FEAT_SSL
                                              | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID),
                   .custom_feature_bit_map =
                     (SL_SI91X_FEAT_CUSTOM_FEAT_EXTENTION_VALID | SL_SI91X_CUSTOM_FEAT_SOC_CLK_CONFIG_160MHZ),
                   .ext_custom_feature_bit_map = (
#ifndef RSI_M4_INTERFACE
                     RAM_LEVEL_NWP_ALL_MCU_ZERO
#else
                     RAM_LEVEL_NWP_ADV_MCU_BASIC
#endif
#ifdef CHIP_917
                     | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
                     ),
                   .bt_feature_bit_map = 0,
                   .ext_tcp_ip_feature_bit_map =
                     (SL_SI91X_EXT_TCP_IP_WINDOW_DIV | SL_SI91X_CONFIG_FEAT_EXTENTION_VALID
                      | SL_SI91X_EXT_TCP_IP_FEAT_SSL_THREE_SOCKETS | SL_SI91X_EXT_TCP_IP_WAIT_FOR_SOCKET_CLOSE),
                   .ble_feature_bit_map     = 0,
                   .ble_ext_feature_bit_map = 0,
                   .config_feature_bit_map  = SL_SI91X_FEAT_SLEEP_GPIO_SEL_BITMAP }
};

uint32_t tick_count_s = 1;

static sl_si91x_socket_config_t socket_config = {
  1,  // Total sockets
  1,  // Total TCP sockets
  0,  // Total UDP sockets
  0,  // TCP TX only sockets
  1,  // TCP RX only sockets
  0,  // UDP TX only sockets
  0,  // UDP RX only sockets
  1,  // TCP RX high performance sockets
  10, // TCP RX window size
  10  // TCP RX window division factor
};
/******************************************************
 *               Function Declarations
 ******************************************************/
void receive_data_from_tcp_client(void);
void send_data_to_tcp_server(void);
void receive_data_from_udp_client(void);
void send_data_to_udp_server(void);
void receive_data_from_tls_server(void);
void send_data_to_tls_server(void);
static void application_start(void *argument);
static void measure_and_print_throughput(uint32_t total_num_of_bytes, uint32_t test_timeout);
#ifdef RSI_M4_INTERFACE
void switch_m4_frequency(void);
#endif

/******************************************************
 *               Function Definitions
 ******************************************************/

void app_init(const void *unused)
{
  UNUSED_PARAMETER(unused);
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

static void measure_and_print_throughput(uint32_t total_num_of_bytes, uint32_t test_timeout)
{
  float duration = ((test_timeout) / 1000) / tick_count_s; // ms to sec
  float result   = (total_num_of_bytes * 8) / duration;    // bytes to bits
  result         = (result / 1000000);                     // bps to Mbps
  printf("\r\nThroughput achieved @ %0.02f Mbps in %0.03f sec successfully\r\n", result, duration);
}

uint8_t has_data_received = 0;
uint32_t bytes_read       = 0;
void data_callback(uint32_t sock_no, uint8_t *buffer, uint32_t length)
{
  bytes_read += length;

  if (bytes_read > 1024 * 1024 * BYTES_TO_RECEIVE_ASYNC) {
    has_data_received = 1;
  }
}

#ifdef RSI_M4_INTERFACE
void switch_m4_frequency(void)
{
  /*Switch M4 SOC clock to Reference clock*/
  /*Default keep M4 in reference clock*/
  RSI_CLK_M4SocClkConfig(M4CLK, M4_ULPREFCLK, 0);
  /*Configure the PLL frequency*/
  RSI_CLK_SetSocPllFreq(M4CLK, PS4_SOC_FREQ, SOC_PLL_REF_FREQUENCY);
  /*Switch M4 clock to PLL clock for speed operations*/
  RSI_CLK_M4SocClkConfig(M4CLK, M4_SOCPLLCLK, 0);
}
#endif

static void application_start(void *argument)
{
  UNUSED_PARAMETER(argument);
  sl_status_t status;

  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &sl_wifi_throughput_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to start Wi-Fi Client interface: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nWi-Fi client interface init success\r\n");

  status = sl_net_up(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to connect to AP: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nWi-Fi client connected\r\n");

#ifdef RSI_M4_INTERFACE
  tick_count_s = 10;
  switch_m4_frequency();
  SysTick_Config(SystemCoreClock / (1000 * tick_count_s));
#endif

  status = sl_net_get_profile(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID, &profile);
  if (status != SL_STATUS_OK) {
    printf("Failed to get client profile: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nSuccess to get client profile\r\n");

  ip_address.type = SL_IPV4;
  memcpy(&ip_address.ip.v4.bytes, &profile.ip.ip.v4.ip_address.bytes, sizeof(sl_ipv4_address_t));
  print_sl_ip_address(&ip_address);

#if ((THROUGHPUT_TYPE == TLS_RX) || (THROUGHPUT_TYPE == TLS_TX))
  sl_tls_store_configuration_t tls_configuration = { 0 };

  // Clearing certificate
  status = sl_tls_clear_global_ca_store();
  if (status != SL_STATUS_OK) {
    return;
  }

  tls_configuration.cacert             = (uint8_t *)cacert;
  tls_configuration.cacert_length      = (sizeof(cacert) - 1);
  tls_configuration.cacert_type        = SL_TLS_SSL_CA_CERTIFICATE;
  tls_configuration.use_secure_element = false;

  // Load SSL CA certificate
  status = sl_tls_set_global_ca_store(tls_configuration);
  if (status != SL_STATUS_OK) {
    printf("\r\nLoading SSL CA certificate in to FLASH Failed, Error Code : 0x%lX\r\n", status);
    return;
  }
  printf("\r\nLoad SSL CA certificate Success\r\n");
#endif

  for (size_t i = 0; i < sizeof(data_buffer); i++)
    data_buffer[i] = 'A' + (i % 26);

  switch (THROUGHPUT_TYPE) {
    case TCP_RX:
      receive_data_from_tcp_client();
      break;
    case TCP_TX:
      send_data_to_tcp_server();
      break;
    case UDP_RX:
      receive_data_from_udp_client();
      break;
    case UDP_TX:
      send_data_to_udp_server();
      break;
    case TLS_RX:
      receive_data_from_tls_server();
      break;
    case TLS_TX:
      send_data_to_tls_server();
      // intentional fallthrough
    default:
      printf("Invalid Throughput test");
  }
}

void send_data_to_tcp_server(void)
{
  int client_socket                 = -1;
  uint32_t total_bytes_sent         = 0;
  int socket_return_value           = 0;
  int sent_bytes                    = 1;
  uint32_t start                    = 0;
  uint32_t now                      = 0;
  uint32_t fail                     = 0;
  uint32_t pass                     = 0;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);

  server_address.sin_family = AF_INET;
  server_address.sin_port   = SERVER_PORT;
  sl_net_inet_addr(SERVER_IP, &server_address.sin_addr.s_addr);

  client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (client_socket < 0) {
    printf("\r\nSocket Create failed with bsd error: %d\r\n", errno);
    return;
  }
  printf("\r\nSocket ID : %d\r\n", client_socket);

  socket_return_value = connect(client_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    printf("\r\nSocket Connect failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  printf("\r\nSocket connected to TCP server\r\n");

  printf("\r\nTCP_TX Throughput test start\r\n");
  start = osKernelGetTickCount();
  while (total_bytes_sent < BYTES_TO_SEND) {
    sent_bytes = send(client_socket, data_buffer, TCP_BUFFER_SIZE, 0);
    now        = osKernelGetTickCount();
    if (sent_bytes > 0)
      total_bytes_sent = total_bytes_sent + sent_bytes;

    if ((now - start) > TEST_TIMEOUT) {
      printf("\r\nTime Out: %ld\r\n", (now - start));
      break;
    }
    if (sent_bytes < 0) {
      fail++;
    } else {
      pass++;
    }
  }
  printf("\r\nTCP_TX Throughput test finished\r\n");
  printf("\r\nTotal bytes sent : %ld\r\n", total_bytes_sent);
  printf("\r\nSend fail count : %ld, Send pass count : %ld\r\n", fail, pass);

  measure_and_print_throughput(total_bytes_sent, (now - start));

  close(client_socket);
}

void receive_data_from_tcp_client(void)
{
  int server_socket                 = -1;
  int client_socket                 = -1;
  int socket_return_value           = 0;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);
  uint8_t high_performance_socket   = SL_HIGH_PERFORMANCE_SOCKET;

  uint32_t start = 0;
  uint32_t now   = 0;

  sl_status_t status = sl_si91x_config_socket(socket_config);
  if (status != SL_STATUS_OK) {
    printf("Socket config failed: %ld\r\n", status);
  }
  printf("\r\nSocket config Done\r\n");

#if SOCKET_ASYNC_FEATURE
  server_socket = sl_si91x_socket_async(AF_INET, SOCK_STREAM, IPPROTO_TCP, &data_callback);
  if (server_socket < 0) {
    printf("\r\nSocket creation failed with bsd error: %d\r\n", errno);
    return;
  }
  printf("\r\nServer Socket ID : %d\r\n", server_socket);

  socket_return_value = sl_si91x_setsockopt(server_socket,
                                            SOL_SOCKET,
                                            sl_si91x_SO_HIGH_PERFORMANCE_SOCKET,
                                            &high_performance_socket,
                                            sizeof(high_performance_socket));
  if (socket_return_value < 0) {
    printf("\r\nSet Socket option failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  server_address.sin_family = AF_INET;
  server_address.sin_port   = LISTENING_PORT;

  socket_return_value = sl_si91x_bind(server_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    printf("\r\nSocket bind failed with bsd error: %d\r\n", errno);
    close(server_socket);
    return;
  }

  socket_return_value = sl_si91x_listen(server_socket, BACK_LOG);
  if (socket_return_value < 0) {
    printf("\r\nSocket listen failed with bsd error: %d\r\n", errno);
    close(server_socket);
    return;
  }
  printf("\r\nListening on Local Port : %d\r\n", LISTENING_PORT);

  client_socket = sl_si91x_accept(server_socket, NULL, 0);
  if (client_socket < 0) {
    printf("\r\nSocket accept failed with bsd error: %d\r\n", errno);
    close(server_socket);
    return;
  }
  printf("\r\nClient Socket ID : %d\r\n", client_socket);

  printf("\r\nTCP_RX Async Throughput test start\r\n");
  start = osKernelGetTickCount();

  while (!has_data_received) {
    osThreadYield();
  }

  now = osKernelGetTickCount();

  printf("\r\nTCP_RX Async Throughput test finished\r\n");
  printf("\r\nTotal bytes received : %ld\r\n", bytes_read);

  close(client_socket);
  close(server_socket);
  measure_and_print_throughput(bytes_read, (now - start));
#else
  int read_bytes = 1;
  uint32_t total_bytes_received = 0;
  server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (server_socket < 0) {
    printf("\r\nSocket creation failed with bsd error: %d\r\n", errno);
    return;
  }
  printf("\r\nServer Socket ID : %d\r\n", server_socket);

  socket_return_value = setsockopt(server_socket,
                                   SOL_SOCKET,
                                   SO_HIGH_PERFORMANCE_SOCKET,
                                   &high_performance_socket,
                                   sizeof(high_performance_socket));
  if (socket_return_value < 0) {
    printf("\r\nSet Socket option failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  server_address.sin_family = AF_INET;
  server_address.sin_port = LISTENING_PORT;

  socket_return_value = bind(server_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    printf("\r\nSocket bind failed with bsd error: %d\r\n", errno);
    close(server_socket);
    return;
  }

  socket_return_value = listen(server_socket, BACK_LOG);
  if (socket_return_value < 0) {
    printf("\r\nSocket listen failed with bsd error: %d\r\n", errno);
    close(server_socket);
    return;
  }
  printf("\r\nListening on Local Port : %d\r\n", LISTENING_PORT);

  client_socket = accept(server_socket, NULL, NULL);
  if (client_socket < 0) {
    printf("\r\nSocket accept failed with bsd error: %d\r\n", errno);
    close(server_socket);
    return;
  }
  printf("\r\nClient Socket ID : %d\r\n", client_socket);

  printf("\r\nTCP_RX Throughput test start\r\n");
  start = osKernelGetTickCount();
  while (read_bytes > 0) {
    read_bytes = recv(client_socket, data_buffer, sizeof(data_buffer), 0);
    if (read_bytes < 0) {
      printf("\r\nReceive failed with bsd error:%d\r\n", errno);
      close(client_socket);
      close(server_socket);
      return;
    }
    total_bytes_received = total_bytes_received + read_bytes;
    now = osKernelGetTickCount();

    if ((now - start) > TEST_TIMEOUT) {
      printf("\r\nTest Time Out: %ld ms\r\n", (now - start));
      break;
    }
  }
  printf("\r\nTCP_RX Throughput test finished\r\n");
  printf("\r\nTotal bytes received : %ld\r\n", total_bytes_received);

  measure_and_print_throughput(total_bytes_received, (now - start));

  close(client_socket);
  close(server_socket);
#endif
}

void send_data_to_udp_server(void)
{
  int client_socket                 = -1;
  uint32_t total_bytes_sent         = 0;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);
  int sent_bytes                    = 1;
  uint32_t start                    = 0;
  uint32_t now                      = 0;
  uint32_t fail                     = 0;
  uint32_t pass                     = 0;

  server_address.sin_family = AF_INET;
  server_address.sin_port   = SERVER_PORT;
  sl_net_inet_addr(SERVER_IP, &server_address.sin_addr.s_addr);

  client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (client_socket < 0) {
    printf("\r\nSocket creation failed with bsd error: %d\r\n", errno);
    return;
  }
  printf("\r\nSocket ID : %d\r\n", client_socket);

  printf("\r\nUDP_TX Throughput test start\r\n");
  start = osKernelGetTickCount();
  while (total_bytes_sent < BYTES_TO_SEND) {
    sent_bytes =
      sendto(client_socket, data_buffer, UDP_BUFFER_SIZE, 0, (struct sockaddr *)&server_address, socket_length);
    now = osKernelGetTickCount();
    if ((now - start) > TEST_TIMEOUT) {
      printf("\r\nTime Out: %ld\r\n", (now - start));
      break;
    }
    if (sent_bytes < 0) {
      fail++;
    } else {
      pass++;
    }
    if (sent_bytes > 0)
      total_bytes_sent = total_bytes_sent + sent_bytes;
  }
  printf("\r\nUDP_TX Throughput test finished\r\n");
  printf("\r\nTotal bytes sent : %ld\r\n", total_bytes_sent);
  printf("\r\nSend fail count : %ld, Send pass count : %ld\r\n", fail, pass);

  measure_and_print_throughput(total_bytes_sent, (now - start));

  close(client_socket);
}

void receive_data_from_udp_client(void)
{
  int client_socket = -1;

  int socket_return_value = 0;

  uint32_t start                    = 0;
  uint32_t now                      = 0;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);

#if SOCKET_ASYNC_FEATURE
  client_socket = sl_si91x_socket_async(AF_INET, SOCK_DGRAM, IPPROTO_UDP, &data_callback);
  if (client_socket < 0) {
    printf("\r\nSocket creation failed with bsd error: %d\r\n", errno);
    return;
  }
  printf("\r\nSocket ID : %d\r\n", client_socket);

  server_address.sin_family = AF_INET;
  server_address.sin_port   = LISTENING_PORT;

  socket_return_value = sl_si91x_bind(client_socket, (struct sockaddr *)&server_address, socket_length);

  if (socket_return_value < 0) {
    printf("\r\nSocket bind failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  printf("\r\nListening on Local Port %d\r\n", LISTENING_PORT);

  printf("\r\nUDP_RX Async Throughput test start\r\n");
  start = osKernelGetTickCount();
  while (!has_data_received) {
    osThreadYield();
  }
  now = osKernelGetTickCount();
  printf("\r\nUDP_RX Async Throughput test finished\r\n");
  printf("\r\nTotal bytes received : %ld\r\n", bytes_read);

  measure_and_print_throughput(bytes_read, (now - start));

  close(client_socket);
#else
  int read_bytes = 1;
  uint32_t total_bytes_received = 0;
  client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (client_socket < 0) {
    printf("\r\nSocket creation failed with bsd error: %d\r\n", errno);
    return;
  }
  printf("\r\nSocket ID : %d\r\n", client_socket);

  server_address.sin_family = AF_INET;
  server_address.sin_port = LISTENING_PORT;

  socket_return_value = bind(client_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    printf("\r\nSocket bind failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  printf("\r\nListening on Local Port %d\r\n", LISTENING_PORT);

  printf("\r\nUDP_RX Throughput test start\r\n");
  start = osKernelGetTickCount();
  while (total_bytes_received < BYTES_TO_RECEIVE) {
    read_bytes = recvfrom(client_socket, data_buffer, sizeof(data_buffer), 0, NULL, NULL);
    if (read_bytes < 0) {
      printf("\r\nReceive failed with bsd error: %d\r\n", errno);
      close(client_socket);
      return;
    }
    total_bytes_received = total_bytes_received + read_bytes;
    now = osKernelGetTickCount();
    if ((now - start) > TEST_TIMEOUT) {
      printf("\r\nTest Time Out: %ld ms\r\n", (now - start));
      break;
    }
  }
  printf("\r\nUDP_RX Throughput test finished\r\n");
  printf("\r\nTotal bytes received : %ld\r\n", total_bytes_received);

  measure_and_print_throughput(total_bytes_received, (now - start));

  close(client_socket);
#endif
}

void receive_data_from_tls_server(void)
{
  int client_socket       = -1;
  int socket_return_value = 0;

  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);
  uint8_t high_performance_socket   = SL_HIGH_PERFORMANCE_SOCKET;

  sl_status_t status = sl_si91x_config_socket(socket_config);
  if (status != SL_STATUS_OK) {
    printf("Socket config failed: %ld\r\n", status);
  }
  printf("Socket config Done\r\n");

#if SOCKET_ASYNC_FEATURE
  client_socket = sl_si91x_socket_async(AF_INET, SOCK_STREAM, IPPROTO_TCP, &data_callback);
  if (client_socket < 0) {
    printf("\r\nSocket creation failed with bsd error: %d\r\n", errno);
    return;
  }
  printf("\r\nSocket ID : %d\r\n", client_socket);

  socket_return_value = setsockopt(client_socket, SOL_TCP, TCP_ULP, TLS, sizeof(TLS));
  if (socket_return_value < 0) {
    printf("\r\nSet Socket option failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }

  socket_return_value = sl_si91x_setsockopt(client_socket,
                                            SOL_SOCKET,
                                            sl_si91x_SO_HIGH_PERFORMANCE_SOCKET,
                                            &high_performance_socket,
                                            sizeof(high_performance_socket));
  if (socket_return_value < 0) {
    printf("\r\nSet Socket option failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }

  server_address.sin_family = AF_INET;
  server_address.sin_port   = SERVER_PORT;
  sl_net_inet_addr(SERVER_IP, &server_address.sin_addr.s_addr);

  socket_return_value = connect(client_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    printf("\r\nSocket Connect failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  printf("\r\nSocket connected to TLS server\r\n");

  printf("\r\nTLS_RX Throughput test start\r\n");
  uint32_t start = osKernelGetTickCount();
  uint32_t now   = start;

  while (!has_data_received) {
    osThreadYield();
  }

  now = osKernelGetTickCount();

  printf("\r\nTCP_RX Throughput test finished\r\n");
  printf("\r\nTotal bytes received : %ld\r\n", bytes_read);

  close(client_socket);
  measure_and_print_throughput(bytes_read, (now - start));
#else
  uint32_t total_bytes_received = 0;
  client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (client_socket < 0) {
    printf("\r\nSocket creation failed with bsd error: %d\r\n", errno);
    return;
  }
  printf("\r\nSocket ID : %d\r\n", client_socket);

  socket_return_value = setsockopt(client_socket, SOL_TCP, TCP_ULP, TLS, sizeof(TLS));
  if (socket_return_value < 0) {
    printf("\r\nSet Socket option failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }

  socket_return_value = setsockopt(client_socket,
                                   SOL_SOCKET,
                                   SO_HIGH_PERFORMANCE_SOCKET,
                                   &high_performance_socket,
                                   sizeof(high_performance_socket));
  if (socket_return_value < 0) {
    printf("\r\nSet Socket option failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }

  server_address.sin_family = AF_INET;
  server_address.sin_port = SERVER_PORT;
  sl_net_inet_addr(SERVER_IP, &server_address.sin_addr.s_addr);

  socket_return_value = connect(client_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    printf("\r\nSocket Connect failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  printf("\r\nSocket connected to TLS server\r\n");

  printf("\r\nTLS_RX Throughput test start\r\n");
  uint32_t start = osKernelGetTickCount();
  uint32_t now = start;
  int read_bytes = 1;
  while (read_bytes > 0) {
    read_bytes = recv(client_socket, data_buffer, sizeof(data_buffer), 0);
    if (read_bytes < 0) {
      printf("\r\nReceive failed with bsd error:%d\r\n", errno);
      close(client_socket);
      return;
    }
    total_bytes_received = total_bytes_received + read_bytes;
    now = osKernelGetTickCount();

    if ((now - start) > TEST_TIMEOUT) {
      printf("\r\nTest Time Out: %ld ms\r\n", (now - start));
      break;
    }
  }
  printf("\r\nTLS_RX Throughput test finished\r\n");
  printf("\r\nTotal bytes received : %ld\r\n", total_bytes_received);

  measure_and_print_throughput(total_bytes_received, (now - start));

  close(client_socket);
#endif
}

void send_data_to_tls_server(void)
{
  int client_socket                 = -1;
  int socket_return_value           = 0;
  uint32_t total_bytes_sent         = 0;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);

  client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (client_socket < 0) {
    printf("\r\nSocket creation failed with bsd error: %d\r\n", errno);
    return;
  }
  printf("\r\nSocket ID : %d\r\n", client_socket);

  socket_return_value = setsockopt(client_socket, SOL_TCP, TCP_ULP, TLS, sizeof(TLS));
  if (socket_return_value < 0) {
    printf("\r\nSet Socket option failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }

  server_address.sin_family = AF_INET;
  server_address.sin_port   = SERVER_PORT;
  sl_net_inet_addr(SERVER_IP, &server_address.sin_addr.s_addr);

  socket_return_value = connect(client_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    printf("\r\nSocket Connect failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  printf("\r\nSocket connected to TLS server\r\n");

  printf("\r\nTLS_TX Throughput test start\r\n");
  const uint32_t start = osKernelGetTickCount();
  uint32_t now         = start;
  int sent_bytes       = 0;
  uint32_t fail        = 0;
  uint32_t pass        = 0;
  while (total_bytes_sent < BYTES_TO_SEND) {
    sent_bytes = send(client_socket, data_buffer, TLS_BUFFER_SIZE, 0);
    now        = osKernelGetTickCount();
    if (sent_bytes > 0)
      total_bytes_sent = total_bytes_sent + sent_bytes;

    if ((now - start) > TEST_TIMEOUT) {
      printf("\r\nTime Out: %ld\r\n", (now - start));
      break;
    }
    if (sent_bytes < 0) {
      fail++;
    } else {
      pass++;
    }
  }
  printf("\r\nTLS_TX Throughput test finished\r\n");
  printf("\r\nTotal bytes sent : %ld\r\n", total_bytes_sent);
  printf("\r\nSend fail count : %ld, Send pass count : %ld\r\n", fail, pass);

  measure_and_print_throughput(total_bytes_sent, (now - start));

  close(client_socket);
}