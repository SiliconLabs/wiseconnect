/***************************************************************************/ /**
 * @file
 * @brief Throughput Example in AP interface
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "socket.h"
#include "sl_si91x_socket_support.h"
#include "sl_utility.h"
#include "errno.h"
#include <string.h>
#include "sl_si91x_driver.h"
#include "sl_net_wifi_types.h"
#include "sl_si91x_socket_utility.h"
#include "sl_si91x_socket_constants.h"
#include "sl_wifi_callback_framework.h"
#include "app.h"
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
#define THROUGHPUT_TYPE UDP_TX

// Type of Socket used. Synchronous = 0, Asynchronous = 1
#define SOCKET_ASYNC_FEATURE 1
#if SOCKET_ASYNC_FEATURE
#include "sl_si91x_socket.h"
#endif

// Memory length for send buffer
#define TCP_BUFFER_SIZE 1460
#define UDP_BUFFER_SIZE 1470
#define TLS_BUFFER_SIZE 1370

// Bufer Size
#if ((THROUGHPUT_TYPE == UDP_RX) || (THROUGHPUT_TYPE == UDP_TX))
#define BUFFER_SIZE UDP_BUFFER_SIZE
#elif ((THROUGHPUT_TYPE == TCP_RX) || (THROUGHPUT_TYPE == TCP_TX))
#define BUFFER_SIZE TCP_BUFFER_SIZE
#elif ((THROUGHPUT_TYPE == TLS_RX) || (THROUGHPUT_TYPE == TLS_TX))
#include "cacert.pem.h"
#define BUFFER_SIZE TLS_BUFFER_SIZE
#endif

//Server IP
#define SERVER_IP "192.168.10.11"

// Server port number
#if ((THROUGHPUT_TYPE == TLS_RX) || (THROUGHPUT_TYPE == TLS_TX))
#define SERVER_PORT 443
#else
#define SERVER_PORT 5000
#endif
#define SL_HIGH_PERFORMANCE_SOCKET BIT(7)

// Module port number
#define LISTENING_PORT 5005
#define BACK_LOG       1

#define BYTES_TO_SEND    (1 << 29) //512MB
#define BYTES_TO_RECEIVE (1 << 28) //256MB
#define TEST_TIMEOUT     (30000)   //30sec

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

static const sl_wifi_device_configuration_t throughput_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .region_code = US,
  .boot_config = { .oper_mode = SL_SI91X_ACCESS_POINT_MODE,
                   .coex_mode = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map =
                     (SL_SI91X_FEAT_SECURITY_OPEN | SL_SI91X_FEAT_AGGREGATION | SL_SI91X_FEAT_WPS_DISABLE),
                   .tcp_ip_feature_bit_map = (SL_SI91X_TCP_IP_FEAT_DHCPV4_SERVER | SL_SI91X_TCP_IP_FEAT_SSL
                                              | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID),
                   .custom_feature_bit_map =
                     (SL_SI91X_CUSTOM_FEAT_EXTENTION_VALID | SL_SI91X_CUSTOM_FEAT_SOC_CLK_CONFIG_160MHZ),
                   .ext_custom_feature_bit_map = (MEMORY_CONFIG
#if defined(SLI_SI917) || defined(SLI_SI915)
                                                  | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
                                                  ),
                   .bt_feature_bit_map = 0,
                   .ext_tcp_ip_feature_bit_map =
                     (SL_SI91X_EXT_TCP_IP_WINDOW_DIV | SL_SI91X_EXT_TCP_IP_WAIT_FOR_SOCKET_CLOSE),
                   .ble_feature_bit_map     = 0,
                   .ble_ext_feature_bit_map = 0,
                   .config_feature_bit_map  = SL_SI91X_FEAT_SLEEP_GPIO_SEL_BITMAP },
  .ta_pool = { .tx_ratio_in_buffer_pool     = TX_POOL_RATIO,
               .rx_ratio_in_buffer_pool     = RX_POOL_RATIO,
               .global_ratio_in_buffer_pool = GLOBAL_POOL_RATIO }
};

uint8_t data_buffer[BUFFER_SIZE];
volatile uint8_t has_data_received  = 0;
volatile uint32_t bytes_read        = 0;
uint32_t start                      = 0;
uint32_t now                        = 0;
uint8_t first_data_frame            = 1;
bool ap_client_connected            = false;
sl_ip_address_t ip_address          = { 0 };
sl_net_wifi_ap_profile_t ap_profile = { 0 };
/******************************************************
 *               Function Declarations
 ******************************************************/
void receive_data_from_udp_client(void);
void send_data_to_udp_server(void);
void receive_data_from_tcp_client(void);
void send_data_to_tcp_server(void);
void receive_data_from_tls_server(void);
void send_data_to_tls_server(void);
void ap_throughput(void);
static sl_status_t ap_connected_event_handler(sl_wifi_event_t event, void *data, uint32_t data_length, void *arg);
static sl_status_t ap_disconnected_event_handler(sl_wifi_event_t event, void *data, uint32_t data_length, void *arg);
static void application_start(void *argument);
static void measure_and_print_throughput(uint32_t total_num_of_bytes, uint32_t test_timeout);

/******************************************************
 *               Function Definitions
 ******************************************************/
void app_init(void)
{
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

static void measure_and_print_throughput(uint32_t total_num_of_bytes, uint32_t test_timeout)
{
  float duration = ((test_timeout) / 1000);                    // ms to sec
  float result   = ((float)total_num_of_bytes * 8) / duration; // bytes to bps
  result         = (result / 1000000);                         // bps to Mbps
  printf("\r\nThroughput achieved @ %0.02f Mbps in %0.03f sec successfully\r\n", result, duration);
}

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
  sl_wifi_firmware_version_t firmware_version = { 0 };

  // AP initialization
  status = sl_net_init(SL_NET_WIFI_AP_INTERFACE, &throughput_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    printf("\r\nWi-Fi AP initialization failed: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nWi-Fi AP initialization success");

  sl_wifi_set_callback(SL_WIFI_CLIENT_CONNECTED_EVENTS, ap_connected_event_handler, NULL);
  sl_wifi_set_callback(SL_WIFI_CLIENT_DISCONNECTED_EVENTS, ap_disconnected_event_handler, NULL);

  // Get firmware version
  status = sl_wifi_get_firmware_version(&firmware_version);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to fetch firmware version: 0x%lx\r\n", status);
    return;
  } else {
    print_firmware_version(&firmware_version);
  }

  // Get AP profile
  status = sl_net_get_profile(SL_NET_WIFI_AP_INTERFACE, SL_NET_DEFAULT_WIFI_AP_PROFILE_ID, &ap_profile);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to get AP profile: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nSuccess to get AP profile\r\n");

  ip_address.type = SL_IPV4;
  memcpy(&ip_address.ip.v4.bytes, &ap_profile.ip.ip.v4.ip_address.bytes, sizeof(sl_ipv4_address_t));

  printf("\r\n IP Address of AP:");
  print_sl_ip_address(&ip_address);

  // Load SSL CA certificate
#if ((THROUGHPUT_TYPE == TLS_RX) || (THROUGHPUT_TYPE == TLS_TX))
  status =
    sl_net_set_credential(SL_NET_TLS_SERVER_CREDENTIAL_ID(0), SL_NET_SIGNING_CERTIFICATE, cacert, sizeof(cacert) - 1);
  if (status != SL_STATUS_OK) {
    printf("\r\nLoading TLS CA certificate in to FLASH Failed, Error Code : 0x%lX\r\n", status);
    return;
  }
  printf("\r\nLoad SSL CA certificate at index %d Success\r\n", 0);
#endif

  // Bring up AP interface
  status = sl_net_up(SL_NET_WIFI_AP_INTERFACE, SL_NET_DEFAULT_WIFI_AP_PROFILE_ID);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to bring Wi-Fi AP interface up: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nAP started\r\n");

  // Wait for client to get connect to AP
  while (ap_client_connected != true) {
    printf("waiting for client to connect with APUT : %d\r\n", ap_client_connected);
    osDelay(1000);
  }

  // Start AP throughput
  ap_throughput();
}

void ap_throughput()
{
  for (size_t i = 0; i < sizeof(data_buffer); i++)
    data_buffer[i] = 'A' + (i % 26);

  // Type of throughput
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
      break;
    default:
      printf("Invalid Throughput test");
  }

  printf("Throughput test completed\r\n");

  while (true) {
    osThreadYield();
  }
}

// TCP_TX
void send_data_to_tcp_server(void)
{
  int client_socket                 = -1;
  uint32_t total_bytes_sent         = 0;
  int socket_return_value           = 0;
  int sent_bytes                    = 1;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);
  server_address.sin_family         = AF_INET;
  server_address.sin_port           = SERVER_PORT;
  sl_net_inet_addr(SERVER_IP, &server_address.sin_addr.s_addr);

  // Create client socket
  client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (client_socket < 0) {
    printf("\r\nSocket create failed with BSD error: %d\r\n", errno);
    return;
  }
  printf("\r\nSocket ID : %d\r\n", client_socket);

  // Connect socket
  socket_return_value = connect(client_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    printf("\r\nSocket Connect failed with BSD error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  printf("\r\nSocket connected to TCP server\r\n");

  // Send data
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

  // Close socket
  close(client_socket);
}

// TCP_RX
void receive_data_from_tcp_client(void)
{
  int server_socket                 = -1;
  int client_socket                 = -1;
  int socket_return_value           = 0;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);
  uint8_t high_performance_socket   = SL_HIGH_PERFORMANCE_SOCKET;

  sl_status_t status = sl_si91x_config_socket(socket_config);
  if (status != SL_STATUS_OK) {
    printf("Socket config failed: %ld\r\n", status);
  }
  printf("\r\nSocket config Done\r\n");

#if SOCKET_ASYNC_FEATURE
  // Create Server socket
  server_socket = sl_si91x_socket_async(AF_INET, SOCK_STREAM, IPPROTO_TCP, &data_callback);
  if (server_socket < 0) {
    printf("\r\nSocket creation failed with BSD error: %d\r\n", errno);
    return;
  }
  printf("\r\nServer Socket ID : %d\r\n", server_socket);

  // Set socket
  socket_return_value = sl_si91x_setsockopt(server_socket,
                                            SOL_SOCKET,
                                            SL_SI91X_SO_HIGH_PERFORMANCE_SOCKET,
                                            &high_performance_socket,
                                            sizeof(high_performance_socket));
  if (socket_return_value < 0) {
    printf("\r\nSet Socket option failed with BSD error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  server_address.sin_family = AF_INET;
  server_address.sin_port   = LISTENING_PORT;

  // Bind socket
  socket_return_value = sl_si91x_bind(server_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    printf("\r\nSocket bind failed with BSD error: %d\r\n", errno);
    close(server_socket);
    return;
  }

  // Listen socket
  socket_return_value = sl_si91x_listen(server_socket, BACK_LOG);
  if (socket_return_value < 0) {
    printf("\r\nSocket listen failed with BSD error:%d\r\n", errno);
    close(server_socket);
    return;
  }
  printf("\r\nListening on Local Port : %d\r\n", LISTENING_PORT);

  // Accept socket
  client_socket = sl_si91x_accept(server_socket, NULL, 0);
  if (client_socket < 0) {
    printf("\r\nSocket accept failed with BSD error: %d\r\n", errno);
    close(server_socket);
    return;
  }

  while (!has_data_received) {
    osThreadYield();
  }

  now = osKernelGetTickCount();

  printf("\r\nTCP_RX Throughput test finished\r\n");
  printf("\r\nTotal bytes received : %ld\r\n", bytes_read);

  // Close socket
  close(server_socket);
  close(client_socket);

  measure_and_print_throughput(bytes_read, (now - start));
#else
  int read_bytes                = 1;
  uint32_t total_bytes_received = 0;

  // Create server socket
  server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (server_socket < 0) {
    printf("\r\nSocket creation failed with BSD error: %d\r\n", errno);
    return;
  }
  printf("\r\nServer Socket ID : %d\r\n", server_socket);

  // Set socket
  socket_return_value = setsockopt(server_socket,
                                   SOL_SOCKET,
                                   SL_SO_HIGH_PERFORMANCE_SOCKET,
                                   &high_performance_socket,
                                   sizeof(high_performance_socket));
  if (socket_return_value < 0) {
    printf("\r\nSet Socket option failed with BSD error: %d\r\n", errno);
    close(server_socket);
    return;
  }
  server_address.sin_family = AF_INET;
  server_address.sin_port   = LISTENING_PORT;

  // Bind socket
  socket_return_value = bind(server_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    printf("\r\nSocket bind failed with BSD error: %d\r\n", errno);
    close(server_socket);
    return;
  }

  // Listen socket
  socket_return_value = listen(server_socket, BACK_LOG);
  if (socket_return_value < 0) {
    printf("\r\nSocket listen failed with BSD error: %d\r\n", errno);
    close(server_socket);
    return;
  }
  printf("\r\nListening on Local Port : %d\r\n", LISTENING_PORT);

  // Accept socket
  client_socket = accept(server_socket, NULL, NULL);
  if (client_socket < 0) {
    printf("\r\nSocket accept failed with BSD error: %d\r\n", errno);
    close(server_socket);
    return;
  }
  printf("\r\nClient Socket ID : %d\r\n", client_socket);

  // Recieve data
  printf("\r\nTCP_RX Throughput test start\r\n");
  start = osKernelGetTickCount();
  while (read_bytes > 0) {
    read_bytes = recv(client_socket, data_buffer, sizeof(data_buffer), 0);
    if (read_bytes < 0) {
      printf("\r\nReceive failed with BSD error:%d\r\n", errno);
      close(server_socket);
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
  printf("\r\nTCP_RX Throughput test finished\r\n");
  printf("\r\nTotal bytes received : %ld\r\n", total_bytes_received);

  measure_and_print_throughput(total_bytes_received, (now - start));

  // Close socket
  close(server_socket);
  close(client_socket);

#endif
}

// UDP_TX
void send_data_to_udp_server(void)
{
  int client_socket                 = -1;
  uint32_t total_bytes_sent         = 0;
  int sent_bytes                    = 1;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);

  server_address.sin_family = AF_INET;
  server_address.sin_port   = SERVER_PORT;
  sl_net_inet_addr(SERVER_IP, &server_address.sin_addr.s_addr);

  // Create client socket
  client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (client_socket < 0) {
    printf("\r\nSocket creation failed with BSD error: %d\r\n", errno);
    return;
  }
  printf("\r\nSocket ID : %d\r\n", client_socket);

  // Send data
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
      if (errno == ENOBUFS)
        continue;
      printf("\r\nSocket send failed with bsd error: %d\r\n", errno);
      close(client_socket);
      break;
    }
    total_bytes_sent = total_bytes_sent + sent_bytes;
  }
  printf("\r\nUDP_TX Throughput test finished\r\n");
  printf("\r\nTotal bytes sent : %ld\r\n", total_bytes_sent);

  measure_and_print_throughput(total_bytes_sent, (now - start));

  close(client_socket);
}

// UDP_RX
void receive_data_from_udp_client(void)
{
  int client_socket = -1;

  int socket_return_value           = 0;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);

#if SOCKET_ASYNC_FEATURE
  // Create socket
  client_socket = sl_si91x_socket_async(AF_INET, SOCK_DGRAM, IPPROTO_UDP, &data_callback);
  if (client_socket < 0) {
    printf("\r\nSocket creation failed with BSD error: %d\r\n", errno);
    return;
  }
  printf("\r\nSocket ID : %d\r\n", client_socket);

  server_address.sin_family = AF_INET;
  server_address.sin_port   = LISTENING_PORT;

  // Bind socket
  socket_return_value = sl_si91x_bind(client_socket, (struct sockaddr *)&server_address, socket_length);

  if (socket_return_value < 0) {
    printf("\r\nSocket bind failed with BSD error: %d\r\n", errno);
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

  // Close socket
  close(client_socket);
#else
  int read_bytes                = 1;
  uint32_t total_bytes_received = 0;

  // Create socket
  client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (client_socket < 0) {
    printf("\r\nSocket creation failed with BSD error: %d\r\n", errno);
    return;
  }
  printf("\r\nSocket ID : %d\r\n", client_socket);

  server_address.sin_family = AF_INET;
  server_address.sin_port   = LISTENING_PORT;

  // Bind socket
  socket_return_value = bind(client_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    printf("\r\nSocket bind failed with BSD error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  printf("\r\nListening on Local Port %d\r\n", LISTENING_PORT);

  // Receive data
  printf("\r\nUDP_RX Throughput test start\r\n");
  start = osKernelGetTickCount();
  while (total_bytes_received < BYTES_TO_RECEIVE) {
    read_bytes = recvfrom(client_socket, data_buffer, sizeof(data_buffer), 0, NULL, NULL);
    if (read_bytes < 0) {
      printf("\r\nReceive failed with BSD error: %d\r\n", errno);
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

  // Close socket
  close(client_socket);
#endif
}

// TLS_RX
void receive_data_from_tls_server(void)
{
  int client_socket                 = -1;
  int socket_return_value           = 0;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);
  uint8_t high_performance_socket   = SL_HIGH_PERFORMANCE_SOCKET;

  sl_status_t status = sl_si91x_config_socket(socket_config);
  if (status != SL_STATUS_OK) {
    printf("Socket config failed: %ld\r\n", status);
  }
  printf("Socket config Done\r\n");

#if SOCKET_ASYNC_FEATURE
  // Create socket
  client_socket = sl_si91x_socket_async(AF_INET, SOCK_STREAM, IPPROTO_TCP, &data_callback);
  if (client_socket < 0) {
    printf("\r\nSocket creation failed with BSD error: %d\r\n", errno);
    return;
  }
  printf("\r\nSocket ID : %d\r\n", client_socket);

  // Set socket
  socket_return_value = setsockopt(client_socket, SOL_TCP, TCP_ULP, TLS, sizeof(TLS));
  if (socket_return_value < 0) {
    printf("\r\nSet Socket option failed with BSD error: %d\r\n", errno);
    close(client_socket);
    return;
  }

  socket_return_value = sl_si91x_setsockopt(client_socket,
                                            SOL_SOCKET,
                                            SL_SI91X_SO_HIGH_PERFORMANCE_SOCKET,
                                            &high_performance_socket,
                                            sizeof(high_performance_socket));
  if (socket_return_value < 0) {
    printf("\r\nSet Socket option failed with BSD error: %d\r\n", errno);
    close(client_socket);
    return;
  }

  server_address.sin_family = AF_INET;
  server_address.sin_port   = SERVER_PORT;
  sl_net_inet_addr(SERVER_IP, &server_address.sin_addr.s_addr);

  // Connect socket
  socket_return_value = connect(client_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    printf("\r\nSocket Connect failed with BSD error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  printf("\r\nSocket connected to TLS server\r\n");

  while (!has_data_received) {
    osThreadYield();
  }

  now = osKernelGetTickCount();

  printf("\r\nTLS_RX Throughput test finished\r\n");
  printf("\r\nTotal bytes received : %ld\r\n", bytes_read);

  // Close socket
  close(client_socket);

  measure_and_print_throughput(bytes_read, (now - start));
#else
  uint32_t total_bytes_received = 0;

  // Create socket
  client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (client_socket < 0) {
    printf("\r\nSocket creation failed with BSD error: %d\r\n", errno);
    return;
  }
  printf("\r\nSocket ID : %d\r\n", client_socket);

  // Set socket
  socket_return_value = setsockopt(client_socket, SOL_TCP, TCP_ULP, TLS, sizeof(TLS));
  if (socket_return_value < 0) {
    printf("\r\nSet Socket option failed with BSD error: %d\r\n", errno);
    close(client_socket);
    return;
  }

  socket_return_value = setsockopt(client_socket,
                                   SOL_SOCKET,
                                   SL_SO_HIGH_PERFORMANCE_SOCKET,
                                   &high_performance_socket,
                                   sizeof(high_performance_socket));
  if (socket_return_value < 0) {
    printf("\r\nSet Socket option failed with BSD error: %d\r\n", errno);
    close(client_socket);
    return;
  }

  server_address.sin_family = AF_INET;
  server_address.sin_port   = SERVER_PORT;
  sl_net_inet_addr(SERVER_IP, &server_address.sin_addr.s_addr);

  // Connect socket
  socket_return_value = connect(client_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    printf("\r\nSocket Connect failed with BSD error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  printf("\r\nSocket connected to TLS server\r\n");

  printf("\r\nTLS_RX Throughput test start\r\n");
  start          = osKernelGetTickCount();
  now            = start;
  int read_bytes = 1;

  // Receive data
  while (read_bytes > 0) {
    read_bytes = recv(client_socket, data_buffer, sizeof(data_buffer), 0);
    if (read_bytes < 0) {
      printf("\r\nReceive failed with BSD error:%d\r\n", errno);
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

  // Close socket
  close(client_socket);
#endif
}

// TLS_TX
void send_data_to_tls_server(void)
{
  int client_socket                 = -1;
  int socket_return_value           = 0;
  uint32_t total_bytes_sent         = 0;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);

  // create socket
  client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (client_socket < 0) {
    printf("\r\nSocket creation failed with BSD error: %d\r\n", errno);
    return;
  }
  printf("\r\nSocket ID : %d\r\n", client_socket);

  // Set socket
  socket_return_value = setsockopt(client_socket, SOL_TCP, TCP_ULP, TLS, sizeof(TLS));
  if (socket_return_value < 0) {
    printf("\r\nSet Socket option failed with BSD error: %d\r\n", errno);
    close(client_socket);
    return;
  }

  server_address.sin_family = AF_INET;
  server_address.sin_port   = SERVER_PORT;
  sl_net_inet_addr(SERVER_IP, &server_address.sin_addr.s_addr);

  // Connect socket
  socket_return_value = connect(client_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    printf("\r\nSocket Connect failed with BSD error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  printf("\r\nSocket connected to TLS server\r\n");

  // Send data
  printf("\r\nTLS_TX Throughput test start\r\n");
  start          = osKernelGetTickCount();
  now            = start;
  int sent_bytes = 0;
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

  // Close socket
  close(client_socket);
}

static sl_status_t ap_connected_event_handler(sl_wifi_event_t event, void *data, uint32_t data_length, void *arg)
{
  UNUSED_PARAMETER(data_length);
  UNUSED_PARAMETER(arg);
  UNUSED_PARAMETER(event);

  printf("Remote Client connected: ");
  print_mac_address((sl_mac_address_t *)data);
  printf("\n");

  ap_client_connected = true;
  return SL_STATUS_OK;
}

static sl_status_t ap_disconnected_event_handler(sl_wifi_event_t event, void *data, uint32_t data_length, void *arg)
{
  UNUSED_PARAMETER(data_length);
  UNUSED_PARAMETER(arg);
  UNUSED_PARAMETER(event);

  printf("Remote Client disconnected: ");
  print_mac_address((sl_mac_address_t *)data);
  printf("\n");

  return SL_STATUS_OK;
}
