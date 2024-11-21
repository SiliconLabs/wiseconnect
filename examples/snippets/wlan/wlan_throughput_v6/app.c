/***************************************************************************/ /**
 * @file
 * @brief WLAN Throughput IPv6 Application
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
#include "errno.h"
#include "sl_net.h"
#include "sl_net_wifi_types.h"
#include "sl_utility.h"
#include "sl_wifi.h"
#include "socket.h"
#include "sl_si91x_socket_support.h"
#include "sl_si91x_socket_utility.h"
#include <string.h>

#ifdef SLI_SI91X_MCU_INTERFACE
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
#define THROUGHPUT_TYPE UDP_RX

// Type of Socket used. Synchronous = 0, Asynchronous = 1
#define SOCKET_ASYNC_FEATURE 1
#if SOCKET_ASYNC_FEATURE
#include "sl_si91x_socket.h"
#endif

// Memory length for send buffer
#define TCP_BUFFER_SIZE 1380
#define UDP_BUFFER_SIZE 1390
#define TLS_BUFFER_SIZE 1300
#define SERVER_IP       "2401:4901:1290:10de::1004"

#if ((THROUGHPUT_TYPE == UDP_RX) || (THROUGHPUT_TYPE == UDP_TX))
#define BUFFER_SIZE UDP_BUFFER_SIZE
#elif ((THROUGHPUT_TYPE == TCP_RX) || (THROUGHPUT_TYPE == TCP_TX))
#define BUFFER_SIZE TCP_BUFFER_SIZE
#elif ((THROUGHPUT_TYPE == TLS_RX) || (THROUGHPUT_TYPE == TLS_TX))
#include "cacert.pem.h"
#define BUFFER_SIZE TLS_BUFFER_SIZE
#endif

// Server port number
#if ((THROUGHPUT_TYPE == TLS_RX) || (THROUGHPUT_TYPE == TLS_TX))
#define SERVER_PORT 443
#else
#define SERVER_PORT 5000
#endif

// Module port number
#define LISTENING_PORT 5005
#define BACK_LOG       1

#define BYTES_TO_SEND    (1 << 29) // 512MB
#define BYTES_TO_RECEIVE (1 << 28) // 256MB
#define TEST_TIMEOUT     (30000)   // 30sec

#define SL_HIGH_PERFORMANCE_SOCKET BIT(7)

/*=======================================================================*/
// NWP buffer allocation parameters
/*=======================================================================*/

#ifndef TX_POOL_RATIO
#define TX_POOL_RATIO 1
#endif

#ifndef RX_POOL_RATIO
#define RX_POOL_RATIO 1
#endif

#ifndef GLOBAL_POOL_RATIO
#define GLOBAL_POOL_RATIO 1
#endif

/*****************************************************
 *                      Socket configuration
*****************************************************/
#define TOTAL_SOCKETS                   1  //@ Total number of sockets. TCP TX + TCP RX + UDP TX + UDP RX
#define TOTAL_TCP_SOCKETS               1  //@ Total TCP sockets. TCP TX + TCP RX
#define TOTAL_UDP_SOCKETS               0  //@ Total UDP sockets. UDP TX + UDP RX
#define TCP_TX_ONLY_SOCKETS             0  //@ Total TCP TX only sockets. TCP TX
#define TCP_RX_ONLY_SOCKETS             1  //@ Total TCP RX only sockets. TCP RX
#define UDP_TX_ONLY_SOCKETS             0  //@ Total UDP TX only sockets. UDP TX
#define UDP_RX_ONLY_SOCKETS             0  //@ Total UDP RX only sockets. UDP RX
#define TCP_RX_HIGH_PERFORMANCE_SOCKETS 1  //@ Total TCP RX High Performance sockets
#define TCP_RX_WINDOW_SIZE_CAP          44 //@ TCP RX Window size
#define TCP_RX_WINDOW_DIV_FACTOR        44 //@ TCP RX Window division factor

/******************************************************
 *                    Constants
 ******************************************************/
/******************************************************
 *               Variable Definitions
 ******************************************************/

uint8_t data_buffer[BUFFER_SIZE];
uint8_t address_buffer[SL_IPV6_ADDRESS_LENGTH];

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

static const sl_wifi_device_configuration_t throughput_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .region_code = US,
  .boot_config = { .oper_mode       = SL_SI91X_CLIENT_MODE,
                   .coex_mode       = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map = (SL_SI91X_FEAT_SECURITY_PSK | SL_SI91X_FEAT_AGGREGATION),
                   .tcp_ip_feature_bit_map =
                     (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT | SL_SI91X_TCP_IP_FEAT_DHCPV6_CLIENT
                      | SL_SI91X_TCP_IP_FEAT_IPV6 | SL_SI91X_TCP_IP_FEAT_SSL | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID),
                   .custom_feature_bit_map =
                     (SL_SI91X_CUSTOM_FEAT_EXTENTION_VALID | SL_SI91X_CUSTOM_FEAT_SOC_CLK_CONFIG_160MHZ),
                   .ext_custom_feature_bit_map = (MEMORY_CONFIG
#if defined(SLI_SI917) || defined(SLI_SI915)
                                                  | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
                                                  ),
                   .bt_feature_bit_map = 0,
                   .ext_tcp_ip_feature_bit_map =
                     (SL_SI91X_EXT_TCP_IP_WINDOW_DIV | SL_SI91X_CONFIG_FEAT_EXTENTION_VALID),
                   .ble_feature_bit_map     = 0,
                   .ble_ext_feature_bit_map = 0,
                   .config_feature_bit_map  = 0 },
  .ta_pool = { .tx_ratio_in_buffer_pool     = TX_POOL_RATIO,
               .rx_ratio_in_buffer_pool     = RX_POOL_RATIO,
               .global_ratio_in_buffer_pool = GLOBAL_POOL_RATIO }
};

static sl_si91x_socket_config_t socket_config = {
  TOTAL_SOCKETS,                   // Total sockets
  TOTAL_TCP_SOCKETS,               // Total TCP sockets
  TOTAL_UDP_SOCKETS,               // Total UDP sockets
  TCP_TX_ONLY_SOCKETS,             // TCP TX only sockets
  TCP_RX_ONLY_SOCKETS,             // TCP RX only sockets
  UDP_TX_ONLY_SOCKETS,             // UDP TX only sockets
  UDP_RX_ONLY_SOCKETS,             // UDP RX only sockets
  TCP_RX_HIGH_PERFORMANCE_SOCKETS, // TCP RX high performance sockets
  TCP_RX_WINDOW_SIZE_CAP,          // TCP RX window size
  TCP_RX_WINDOW_DIV_FACTOR         // TCP RX window division factor
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
  float duration = ((test_timeout) / 1000);                    // ms to sec
  float result   = ((float)total_num_of_bytes * 8) / duration; // bytes to bps
  result         = (result / 1000000);                         // bps to Mbps
  printf("\r\nThroughput achieved @ %0.02f Mbps in %0.03f sec successfully\r\n", result, duration);
}

volatile uint8_t has_data_received = 0;
volatile uint32_t bytes_read       = 0;
uint32_t start                     = 0;
uint32_t now                       = 0;
uint8_t first_data_frame           = 1;

void data_callback(uint32_t sock_no,
                   uint8_t *buffer,
                   uint32_t length,
                   const sl_si91x_socket_metadata_t *firmware_socket_response)
{
  UNUSED_PARAMETER(buffer);
  UNUSED_PARAMETER(firmware_socket_response);

  if (first_data_frame) {
    start = osKernelGetTickCount();
    printf("\r\nClient Socket ID : %ld\r\n", sock_no);
    switch (THROUGHPUT_TYPE) {
      case UDP_RX:
        printf("\r\nUDP_RX Throughput test start\r\n");
        break;
      case TCP_RX:
        printf("\r\nTCP_RX Throughput test start\r\n");
        break;
      case TLS_RX:
        printf("\r\nTLS_RX Throughput test start\r\n");
        break;
    }
    first_data_frame = 0;
  }

  bytes_read += length;
  now = osKernelGetTickCount();
  if ((bytes_read > BYTES_TO_RECEIVE) || ((now - start) > TEST_TIMEOUT)) {
    has_data_received = 1;
  }
}

static void application_start(void *argument)
{
  UNUSED_PARAMETER(argument);
  sl_status_t status;

  sl_mac_address_t mac_addr                   = { 0 };
  sl_wifi_firmware_version_t firmware_version = { 0 };

  sl_ip_address_t ip_address           = { 0 };
  sl_net_wifi_client_profile_t profile = { 0 };

  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &throughput_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to start Wi-Fi Client interface: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nWi-Fi client interface init success\r\n");

  status = sl_wifi_get_mac_address(SL_WIFI_CLIENT_INTERFACE, &mac_addr);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to get mac address: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nDevice MAC address: %x:%x:%x:%x:%x:%x\r\n",
         mac_addr.octet[0],
         mac_addr.octet[1],
         mac_addr.octet[2],
         mac_addr.octet[3],
         mac_addr.octet[4],
         mac_addr.octet[5]);

  status = sl_wifi_get_firmware_version(&firmware_version);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to fetch firmware version: 0x%lx\r\n", status);
    return;
  } else {
    print_firmware_version(&firmware_version);
  }

  status = sl_net_up(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to connect to AP: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nWi-Fi client connected\r\n");

  status = sl_net_get_profile(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID, &profile);
  if (status != SL_STATUS_OK) {
    printf("Failed to get client profile: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nClient profile is fetched successfully\r\n");

  if (profile.ip.type == SL_IPV4) {
    ip_address.type = SL_IPV4;
    memcpy(&ip_address.ip.v4.bytes, &profile.ip.ip.v4.ip_address.bytes, sizeof(sl_ipv4_address_t));
    print_sl_ip_address(&ip_address);
  } else if (profile.ip.type == SL_IPV6) {
    sl_ip_address_t link_local_address = { 0 };
    memcpy(&link_local_address.ip.v6, &profile.ip.ip.v6.link_local_address, SL_IPV6_ADDRESS_LENGTH);
    link_local_address.type = SL_IPV6;
    printf("Link Local Address: ");
    print_sl_ip_address(&link_local_address);

    sl_ip_address_t global_address = { 0 };
    memcpy(&global_address.ip.v6, &profile.ip.ip.v6.global_address, SL_IPV6_ADDRESS_LENGTH);
    global_address.type = SL_IPV6;
    printf("Global Address: ");
    print_sl_ip_address(&global_address);

    sl_ip_address_t gateway = { 0 };
    memcpy(&gateway.ip.v6, &profile.ip.ip.v6.gateway, SL_IPV6_ADDRESS_LENGTH);
    gateway.type = SL_IPV6;
    printf("Gateway Address: ");
    print_sl_ip_address(&gateway);
  }

#if ((THROUGHPUT_TYPE == TLS_RX) || (THROUGHPUT_TYPE == TLS_TX))
  // Load SSL CA certificate
  status =
    sl_net_set_credential(SL_NET_TLS_SERVER_CREDENTIAL_ID(0), SL_NET_SIGNING_CERTIFICATE, cacert, sizeof(cacert) - 1);
  if (status != SL_STATUS_OK) {
    printf("\r\nLoading TLS CA certificate in to FLASH Failed, Error Code : 0x%lX\r\n", status);
    return;
  }
  printf("\r\nLoad TLS CA certificate at index %d Success\r\n", 0);
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
  int client_socket                   = -1;
  uint32_t total_bytes_sent           = 0;
  struct sockaddr_in6 server_address6 = { 0 };
  socklen_t socket_length             = sizeof(struct sockaddr_in6);
  int socket_return_value             = 0;
  int sent_bytes                      = 1;
  uint32_t start                      = 0;
  uint32_t now                        = 0;

  client_socket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
  if (client_socket < 0) {
    printf("\r\nSocket Create failed with bsd error: %d\r\n", errno);
    return;
  }
  printf("\r\nSocket ID : %d\r\n", client_socket);

  server_address6.sin6_family = AF_INET6;
  server_address6.sin6_port   = SERVER_PORT;

  int status = sl_inet_pton6(SERVER_IP,
                             SERVER_IP + strlen(SERVER_IP),
                             address_buffer,
                             (unsigned int *)server_address6.sin6_addr.__u6_addr.__u6_addr32);
  if (status != 0x1) {
    printf("\r\nIPv6 conversion failed.\r\n");
    return;
  }

  socket_return_value = connect(client_socket, (struct sockaddr *)&server_address6, socket_length);
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
    if (sent_bytes < 0) {
      if (errno == ENOBUFS)
        continue;
      printf("\r\nSocket send failed with bsd error: %d\r\n", errno);
      close(client_socket);
      break;
    }
    total_bytes_sent = total_bytes_sent + sent_bytes;

    if ((now - start) > TEST_TIMEOUT) {
      printf("\r\nTime Out: %ld\r\n", (now - start));
      break;
    }
  }
  printf("\r\nTCP_TX Throughput test finished\r\n");
  printf("\r\nTotal bytes sent : %ld\r\n", total_bytes_sent);

  measure_and_print_throughput(total_bytes_sent, (now - start));

  close(client_socket);
}

void receive_data_from_tcp_client(void)
{
  int server_socket                   = -1;
  int client_socket                   = -1;
  int socket_return_value             = 0;
  struct sockaddr_in6 server_address6 = { 0 };
  socklen_t socket_length             = sizeof(struct sockaddr_in6);
  uint8_t high_performance_socket     = SL_HIGH_PERFORMANCE_SOCKET;

  sl_status_t status = sl_si91x_config_socket(socket_config);
  if (status != SL_STATUS_OK) {
    printf("Socket config failed: %ld\r\n", status);
  }
  printf("\r\nSocket config Done\r\n");

#if SOCKET_ASYNC_FEATURE
  server_socket = sl_si91x_socket_async(AF_INET6, SOCK_STREAM, IPPROTO_TCP, &data_callback);
  if (server_socket < 0) {
    printf("\r\nSocket creation failed with bsd error: %d\r\n", errno);
    return;
  }
  printf("\r\nServer Socket ID : %d\r\n", server_socket);

  socket_return_value = sl_si91x_setsockopt(server_socket,
                                            SOL_SOCKET,
                                            SL_SI91X_SO_HIGH_PERFORMANCE_SOCKET,
                                            &high_performance_socket,
                                            sizeof(high_performance_socket));
  if (socket_return_value < 0) {
    printf("\r\nSet Socket option failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  server_address6.sin6_family = AF_INET6;
  server_address6.sin6_port   = LISTENING_PORT;

  socket_return_value = sl_si91x_bind(server_socket, (struct sockaddr *)&server_address6, socket_length);
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

  while (!has_data_received) {
    osThreadYield();
  }

  now = osKernelGetTickCount();

  printf("\r\nTCP_RX Throughput test finished\r\n");
  printf("\r\nTotal bytes received : %ld\r\n", bytes_read);

  close(server_socket);
  close(client_socket);

  measure_and_print_throughput(bytes_read, (now - start));
#else

  uint32_t start                = 0;
  uint32_t now                  = 0;
  int read_bytes                = 1;
  uint32_t total_bytes_received = 0;

  server_socket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
  if (server_socket < 0) {
    printf("\r\nSocket creation failed with bsd error: %d\r\n", errno);
    return;
  }
  printf("\r\nServer Socket ID : %d\r\n", server_socket);

  socket_return_value = setsockopt(server_socket,
                                   SOL_SOCKET,
                                   SL_SO_HIGH_PERFORMANCE_SOCKET,
                                   &high_performance_socket,
                                   sizeof(high_performance_socket));
  if (socket_return_value < 0) {
    printf("\r\nSet Socket option failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }

  server_address6.sin6_family = AF_INET6;
  server_address6.sin6_port   = LISTENING_PORT;
  socket_return_value         = bind(server_socket, (struct sockaddr *)&server_address6, socket_length);
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

    total_bytes_received += read_bytes;
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
  int client_socket                   = -1;
  uint32_t total_bytes_sent           = 0;
  struct sockaddr_in6 server_address6 = { 0 };
  socklen_t socket_length             = sizeof(struct sockaddr_in6);
  int sent_bytes                      = 1;
  uint32_t start                      = 0;
  uint32_t now                        = 0;

  client_socket = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
  if (client_socket < 0) {
    printf("\r\nSocket creation failed with bsd error: %d\r\n", errno);
    return;
  }
  printf("\r\nSocket ID : %d\r\n", client_socket);

  server_address6.sin6_family = AF_INET6;
  server_address6.sin6_port   = SERVER_PORT;

  int status = sl_inet_pton6(SERVER_IP,
                             SERVER_IP + strlen(SERVER_IP),
                             address_buffer,
                             (unsigned int *)server_address6.sin6_addr.__u6_addr.__u6_addr32);
  if (status != 0x1) {
    printf("\r\nIPv6 conversion failed.\r\n");
    return;
  }

  printf("\r\nUDP_TX Throughput test start\r\n");
  start = osKernelGetTickCount();
  while (total_bytes_sent < BYTES_TO_SEND) {
    sent_bytes =
      sendto(client_socket, data_buffer, UDP_BUFFER_SIZE, 0, (struct sockaddr *)&server_address6, socket_length);
    now = osKernelGetTickCount();

    if (sent_bytes < 0) {
      if (errno == ENOBUFS)
        continue;
      printf("\r\nSocket send failed with bsd error: %d\r\n", errno);
      close(client_socket);
      break;
    }

    total_bytes_sent = total_bytes_sent + sent_bytes;
    if ((now - start) > TEST_TIMEOUT) {
      printf("\r\nTime Out: %ld\r\n", (now - start));
      break;
    }
  }
  printf("\r\nUDP_TX Throughput test finished\r\n");
  printf("\r\nTotal bytes sent : %ld\r\n", total_bytes_sent);

  measure_and_print_throughput(total_bytes_sent, (now - start));

  close(client_socket);
}

void receive_data_from_udp_client(void)
{
  int client_socket                   = -1;
  int socket_return_value             = 0;
  struct sockaddr_in6 server_address6 = { 0 };
  socklen_t socket_length             = sizeof(struct sockaddr_in6);

#if SOCKET_ASYNC_FEATURE
  client_socket = sl_si91x_socket_async(AF_INET6, SOCK_DGRAM, IPPROTO_UDP, &data_callback);
  if (client_socket < 0) {
    printf("\r\nSocket creation failed with bsd error: %d\r\n", errno);
    return;
  }
  printf("\r\nSocket ID : %d\r\n", client_socket);

  server_address6.sin6_family = AF_INET6;
  server_address6.sin6_port   = LISTENING_PORT;

  socket_return_value = sl_si91x_bind(client_socket, (struct sockaddr *)&server_address6, socket_length);

  if (socket_return_value < 0) {
    printf("\r\nSocket bind failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  printf("\r\nListening on Local Port %d\r\n", LISTENING_PORT);

  while (!has_data_received) {
    osThreadYield();
  }
  now = osKernelGetTickCount();
  printf("\r\nUDP_RX Async Throughput test finished\r\n");
  printf("\r\nTotal bytes received : %ld\r\n", bytes_read);

  measure_and_print_throughput(bytes_read, (now - start));

  close(client_socket);
#else

  uint32_t start                = 0;
  uint32_t now                  = 0;
  int read_bytes                = 1;
  uint32_t total_bytes_received = 0;

  client_socket = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
  if (client_socket < 0) {
    printf("\r\nSocket creation failed with bsd error: %d\r\n", errno);
    return;
  }
  printf("\r\nSocket ID : %d\r\n", client_socket);

  server_address6.sin6_family = AF_INET6;
  server_address6.sin6_port   = LISTENING_PORT;

  socket_return_value = bind(client_socket, (struct sockaddr *)&server_address6, socket_length);
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
    now                  = osKernelGetTickCount();

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
  int client_socket                   = -1;
  int socket_return_value             = 0;
  struct sockaddr_in6 server_address6 = { 0 };
  socklen_t socket_length             = sizeof(struct sockaddr_in6);
  uint8_t high_performance_socket     = SL_HIGH_PERFORMANCE_SOCKET;
  sl_status_t status                  = sl_si91x_config_socket(socket_config);
  if (status != SL_STATUS_OK) {
    printf("Socket config failed: %ld\r\n", status);
  }
  printf("Socket config Done\r\n");

#if SOCKET_ASYNC_FEATURE
  client_socket = sl_si91x_socket_async(AF_INET6, SOCK_STREAM, IPPROTO_TCP, &data_callback);
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
                                            SL_SI91X_SO_HIGH_PERFORMANCE_SOCKET,
                                            &high_performance_socket,
                                            sizeof(high_performance_socket));
  if (socket_return_value < 0) {
    printf("\r\nSet Socket option failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }

  server_address6.sin6_family = AF_INET6;
  server_address6.sin6_port   = SERVER_PORT;
  int ret_status              = sl_inet_pton6(SERVER_IP,
                                 SERVER_IP + strlen(SERVER_IP),
                                 address_buffer,
                                 (unsigned int *)server_address6.sin6_addr.__u6_addr.__u6_addr32);
  if (ret_status != 0x1) {
    printf("\r\nIPv6 conversion failed.\r\n");
    return;
  }

  socket_return_value = connect(client_socket, (struct sockaddr *)&server_address6, socket_length);
  if (socket_return_value < 0) {
    printf("\r\nSocket Connect failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  printf("\r\nSocket connected to TLS server\r\n");

  while (!has_data_received) {
    osThreadYield();
  }

  now = osKernelGetTickCount();

  printf("\r\nTCP_RX Throughput test finished\r\n");
  printf("\r\nTotal bytes received : %ld\r\n", bytes_read);

  close(client_socket);
  measure_and_print_throughput(bytes_read, (now - start));
#else
  uint32_t start                = 0;
  uint32_t now                  = 0;
  int read_bytes                = 1;
  uint32_t total_bytes_received = 0;

  client_socket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
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
                                   SL_SO_HIGH_PERFORMANCE_SOCKET,
                                   &high_performance_socket,
                                   sizeof(high_performance_socket));
  if (socket_return_value < 0) {
    printf("\r\nSet Socket option failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }

  server_address6.sin6_family = AF_INET6;
  server_address6.sin6_port   = SERVER_PORT;

  int ret_status = sl_inet_pton6(SERVER_IP,
                                 SERVER_IP + strlen(SERVER_IP),
                                 address_buffer,
                                 (unsigned int *)server_address6.sin6_addr.__u6_addr.__u6_addr32);
  if (ret_status != 0x1) {
    printf("\r\nIPv6 conversion failed.\r\n");
    return;
  }

  socket_return_value = connect(client_socket, (struct sockaddr *)&server_address6, socket_length);
  if (socket_return_value < 0) {
    printf("\r\nSocket Connect failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  printf("\r\nSocket connected to TLS server\r\n");

  printf("\r\nTLS_RX Throughput test start\r\n");
  start = osKernelGetTickCount();
  while (read_bytes > 0) {
    read_bytes = recv(client_socket, data_buffer, sizeof(data_buffer), 0);
    if (read_bytes < 0) {
      printf("\r\nReceive failed with bsd error:%d\r\n", errno);
      close(client_socket);
      return;
    }

    total_bytes_received = total_bytes_received + read_bytes;
    now                  = osKernelGetTickCount();

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
  int client_socket                   = -1;
  int socket_return_value             = 0;
  struct sockaddr_in6 server_address6 = { 0 };
  socklen_t socket_length             = sizeof(struct sockaddr_in6);
  uint32_t start                      = 0;
  uint32_t now                        = 0;
  int sent_bytes                      = 0;
  uint32_t total_bytes_sent           = 0;

  client_socket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
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

  server_address6.sin6_family = AF_INET6;
  server_address6.sin6_port   = SERVER_PORT;

  int status = sl_inet_pton6(SERVER_IP,
                             SERVER_IP + strlen(SERVER_IP),
                             address_buffer,
                             (unsigned int *)server_address6.sin6_addr.__u6_addr.__u6_addr32);
  if (status != 0x1) {
    printf("\r\nIPv6 conversion failed.\r\n");
    return;
  }

  socket_return_value = connect(client_socket, (struct sockaddr *)&server_address6, socket_length);
  if (socket_return_value < 0) {
    printf("\r\nSocket Connect failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  printf("\r\nSocket connected to TLS server\r\n");

  printf("\r\nTLS_TX Throughput test start\r\n");
  start = osKernelGetTickCount();
  while (total_bytes_sent < BYTES_TO_SEND) {
    sent_bytes = send(client_socket, data_buffer, TLS_BUFFER_SIZE, 0);
    now        = osKernelGetTickCount();

    if (sent_bytes < 0) {
      if (errno == ENOBUFS)
        continue;
      printf("\r\nSocket send failed with bsd error: %d\r\n", errno);
      close(client_socket);
      break;
    }
    total_bytes_sent = total_bytes_sent + sent_bytes;

    if ((now - start) > TEST_TIMEOUT) {
      printf("\r\nTime Out: %ld\r\n", (now - start));
      break;
    }
  }
  printf("\r\nTLS_TX Throughput test finished\r\n");
  printf("\r\nTotal bytes sent : %ld\r\n", total_bytes_sent);

  measure_and_print_throughput(total_bytes_sent, (now - start));

  close(client_socket);
}
