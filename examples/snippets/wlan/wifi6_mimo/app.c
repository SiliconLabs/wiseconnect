
/***************************************************************************/ /**
 * @file
 * @brief Access point Example Application
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
#include "sl_status.h"
#include "cmsis_os2.h"
#include "sl_wifi.h"
#include "sl_net.h"
#include "socket.h"
#include "sl_ip_types.h"
#include "sl_utility.h"
#include "sl_net_si91x.h"
#include "errno.h"
#include "sl_net_wifi_types.h"
#ifdef RSI_M4_INTERFACE
#include "rsi_rom_clks.h"
#endif

/******************************************************
*                    Constants
******************************************************/
#define SERVER_IP_ADDRESS "192.168.0.107"
#define SERVER_PORT       5003
#define BYTES_TO_RECEIVE  (1 << 20) // 1MB
#define BYTES_TO_SEND     (1 << 29) // 512MB
#define TEST_TIMEOUT      10000     // 10sec
#define BACKLOG_COUNT     1

#define UDP_TX 0
#define UDP_RX 1
#define TCP_RX 2

#define THROUGHPUT_TYPE UDP_TX

#define TCP_BUFFER_SIZE 1460
#define UDP_BUFFER_SIZE 1470

#define SL_HIGH_PERFORMANCE_SOCKET BIT(7)

#if ((THROUGHPUT_TYPE == UDP_RX) || (THROUGHPUT_TYPE == UDP_TX))
#define BUFFER_SIZE UDP_BUFFER_SIZE
#elif (THROUGHPUT_TYPE == TCP_RX)
#define BUFFER_SIZE TCP_BUFFER_SIZE
#endif

#ifdef RSI_M4_INTERFACE
#define SOC_PLL_REF_FREQUENCY 40000000  /*<! PLL input REFERENCE clock 40MHZ */
#define PS4_SOC_FREQ          119000000 /*<! PLL out clock 100MHz            */
#endif

/******************************************************
*               Variable Definitions
******************************************************/
uint8_t send_buffer[BUFFER_SIZE];
uint8_t receive_buffer[BUFFER_SIZE];

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

static const sl_wifi_device_configuration_t client_init_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .boot_config = { .oper_mode              = SL_SI91X_CLIENT_MODE,
                   .coex_mode              = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map        = (SL_SI91X_FEAT_SECURITY_PSK | SL_SI91X_FEAT_AGGREGATION),
                   .tcp_ip_feature_bit_map = (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID
                                              | SL_SI91X_TCP_IP_FEAT_SSL),
                   .custom_feature_bit_map =
                     (SL_SI91X_FEAT_CUSTOM_FEAT_EXTENTION_VALID | SL_SI91X_CUSTOM_FEAT_SOC_CLK_CONFIG_160MHZ),
                   .ext_custom_feature_bit_map = (
#ifndef RSI_M4_INTERFACE
                     RAM_LEVEL_NWP_ALL_MCU_ZERO
#else
                     RAM_LEVEL_NWP_MEDIUM_MCU_MEDIUM
#endif
                     ),
                   .bt_feature_bit_map = 0,
                   .ext_tcp_ip_feature_bit_map =
                     (SL_SI91X_EXT_TCP_IP_WINDOW_DIV | SL_SI91X_CONFIG_FEAT_EXTENTION_VALID),
                   .ble_feature_bit_map     = 0,
                   .ble_ext_feature_bit_map = 0,
                   .config_feature_bit_map  = 0 }
};

/******************************************************
*               Function Declarations
******************************************************/
void receive_data_from_tcp_client();
void receive_data_from_udp_socket();
void send_data_to_udp_socket();
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
  float duration = (test_timeout) / 1000;               // ms to sec
  float result   = (total_num_of_bytes * 8) / duration; // bytes to bits
  result         = (result / 1000000);                  // bps to Mbps
  printf("\r\nThroughput achieved @ %0.02f Mbps in %0.03f sec successfully\r\n", result, duration);
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
  sl_net_wifi_client_profile_t profile = { 0 };
  sl_ip_address_t ip_address           = { 0 };

  sl_status_t status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &client_init_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    printf("\r\nError while initialization: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nWiFi init success\r\n");

  status = sl_net_up(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID);
  if (status != SL_STATUS_OK) {
    printf("Failed to bring Wi-Fi interface up: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nWiFi connection success\r\n");

#ifdef RSI_M4_INTERFACE
  switch_m4_frequency();
  SysTick_Config(SystemCoreClock / (1000));
#endif

  status = sl_net_get_profile(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID, &profile);
  if (status != SL_STATUS_OK) {
    printf("Failed to get client profile: 0x%lx\r\n", status);
    return;
  }
  printf("\r\n Success to get client profile \r\n");

  ip_address.type = SL_IPV4;
  memcpy(&ip_address.ip.v4.bytes, &profile.ip.ip.v4.ip_address.bytes, sizeof(sl_ipv4_address_t));
  print_sl_ip_address(&ip_address);

  for (int i = 0; i < BUFFER_SIZE; i++) {
    send_buffer[i] = 'A' + (i % 26);
  }

  switch (THROUGHPUT_TYPE) {
    case TCP_RX:
      receive_data_from_tcp_client();
      break;
    case UDP_RX:
      receive_data_from_udp_socket();
      break;
    case UDP_TX:
      send_data_to_udp_socket();
      break;
    default:
      printf("\r\nInvalid throughput type\r\n");
  }
}

void receive_data_from_tcp_client()
{
  int32_t read_bytes                = 0;
  uint32_t total_bytes_read         = 0;
  int server_socket                 = -1;
  int client_socket                 = -1;
  int socket_return_value           = 0;
  uint32_t start                    = 0;
  uint32_t now                      = 0;
  uint8_t high_performance_socket   = SL_HIGH_PERFORMANCE_SOCKET;
  sl_ipv4_address_t ip_address      = { 0 };
  struct sockaddr_in server_address = { 0 };

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
  server_address.sin_port   = SERVER_PORT;
  sl_net_inet_addr(SERVER_IP_ADDRESS, (uint32_t *)&ip_address);
  server_address.sin_addr.s_addr = ip_address.value;

  if ((bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address))) < 0) {
    printf("\r\nSocket bind failed with bsd error: %d\r\n", errno);
    return;
  }

  if ((listen(server_socket, BACKLOG_COUNT)) < 0) {
    printf("\r\nSocket listen failed with bsd error: %d\r\n", errno);
    return;
  }
  printf("\r\nListening on port %d \r\n", SERVER_PORT);

  client_socket = accept(server_socket, NULL, NULL);
  if (client_socket < 0) {
    printf("\r\nAccept failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  printf("\r\nClient Socket ID : %d\r\n", client_socket);

  printf("\r\nTCP_RX Throughput test start\r\n");

  uint32_t recv_size = BUFFER_SIZE;
  start              = osKernelGetTickCount();
  while (total_bytes_read < BYTES_TO_RECEIVE) {
    read_bytes = recv(client_socket, &receive_buffer, recv_size, 0);
    if (read_bytes < 0) {
      printf("\r\nReceive failed with bsd error: %d\r\n", errno);
      close(client_socket);
      return;
    }
    total_bytes_read += read_bytes;
    now = osKernelGetTickCount();

    if ((now - start) > TEST_TIMEOUT) {
      printf("\r\nTest Time Out: %ldms\r\n", (now - start));
      break;
    }
  }

  printf("\r\nTCP_RX Throughput test finished\r\n");
  printf("\r\nTotal bytes received : %ld\r\n", total_bytes_read);

  measure_and_print_throughput(total_bytes_read, (now - start));

  close(client_socket);
  close(server_socket);
}

void receive_data_from_udp_socket()
{
  int32_t read_bytes                = 0;
  uint32_t total_bytes_read         = 0;
  int module_socket                 = -1;
  uint32_t start                    = 0;
  uint32_t now                      = 0;
  struct sockaddr_in socket_address = { 0 };
  struct sockaddr_in remote_address = { 0 };
  socklen_t remote_address_length   = sizeof(remote_address);

  module_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (module_socket < 0) {
    printf("\r\nSocket creation failed with bsd error: %d\r\n", errno);
    return;
  }
  printf("\r\nSocket ID : %d\r\n", module_socket);

  socket_address.sin_family = AF_INET;
  socket_address.sin_port   = SERVER_PORT;

  if ((bind(module_socket, (struct sockaddr *)&socket_address, sizeof(socket_address))) < 0) {
    printf("\r\nSocket bind failed with bsd error: %d\r\n", errno);
    return;
  }
  printf("\r\nListening on Local Port %d\r\n", SERVER_PORT);

  printf("\r\nUDP_RX Throughput test start\r\n");
  start = osKernelGetTickCount();
  while (total_bytes_read < BYTES_TO_RECEIVE) {
    read_bytes = recvfrom(module_socket,
                          &receive_buffer,
                          sizeof(receive_buffer),
                          0,
                          (struct sockaddr *)&remote_address,
                          &remote_address_length);
    if (read_bytes < 0) {
      printf("\r\nReceive failed with bsd error: %d\r\n", errno);
      close(module_socket);
      return;
    }
    total_bytes_read += read_bytes;
    now = osKernelGetTickCount();
    if ((now - start) > TEST_TIMEOUT) {
      printf("\r\nTest Time Out: %ldms\r\n", (now - start));
      break;
    }
  }
  printf("\r\nUDP_RX Throughput test finished\r\n");
  printf("\r\nTotal bytes received : %ld\r\n", total_bytes_read);

  measure_and_print_throughput(total_bytes_read, (now - start));

  close(module_socket);
}

void send_data_to_udp_socket()
{
  int32_t bytes_sent                       = 0;
  uint32_t total_bytes_sent                = 0;
  int module_socket                        = -1;
  uint32_t start                           = 0;
  uint32_t now                             = 0;
  struct sockaddr_in remote_socket_address = { 0 };
  sl_ipv4_address_t remote_ip_address      = { 0 };

  module_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (module_socket < 0) {
    printf("\r\nSocket creation failed with bsd error: %d\r\n", errno);
    return;
  }
  printf("\r\nSocket ID : %d\r\n", module_socket);

  remote_socket_address.sin_family = AF_INET;
  remote_socket_address.sin_port   = SERVER_PORT;
  sl_net_inet_addr(SERVER_IP_ADDRESS, (uint32_t *)&remote_ip_address);
  remote_socket_address.sin_addr.s_addr = remote_ip_address.value;

  printf("\r\nUDP_TX Throughput test start\r\n");
  start = osKernelGetTickCount();
  while (total_bytes_sent < BYTES_TO_SEND) {
    bytes_sent = sendto(module_socket,
                        send_buffer,
                        sizeof(send_buffer),
                        0,
                        (struct sockaddr *)&remote_socket_address,
                        sizeof(remote_socket_address));
    now        = osKernelGetTickCount();
    if ((now - start) > TEST_TIMEOUT) {
      printf("\r\nTime Out: %ld\r\n", (now - start));
      break;
    }
    if (bytes_sent > 0)
      total_bytes_sent += bytes_sent;
  }
  printf("\r\nUDP_TX Throughput test finished\r\n");
  printf("\r\nTotal bytes sent : %ld\r\n", total_bytes_sent);

  measure_and_print_throughput(total_bytes_sent, (now - start));

  close(module_socket);
}
