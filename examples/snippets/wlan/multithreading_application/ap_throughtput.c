/***************************************************************************/ /**
 * @file
 * @brief AP-side Throughput Example (runs in the ap_thread)
 *
 * Measures WLAN throughput (TCP or UDP, TX or RX) on the SoftAP VAP.
 * The throughput type and socket mode (sync/async) are compile-time
 * configurable via the THROUGHPUT_TYPE and SOCKET_ASYNC_FEATURE macros.
 *
 * Thread-safety notes
 * -------------------
 *  - data_callback() is invoked from the firmware receive context and writes
 *    to volatile globals (bytes_read, has_data_received, etc.).
 *  - The throughput() function polls has_data_received in a yield loop.
 *  - All console output uses LOG_PRINT (mutex-protected).
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
#include "socket.h"
#include "sl_si91x_socket_support.h"
#include "sl_utility.h"
#include "errno.h"
#include <string.h>
#include "sl_si91x_driver.h"
#include "sl_net_wifi_types.h"
#include "sl_si91x_socket_utility.h"
#include "sl_si91x_socket_constants.h"
#include "sl_si91x_socket.h"
#include "app.h"
#include "sl_si91x_core_utilities.h"
#ifdef SLI_SI91X_MCU_INTERFACE
#include "rsi_rom_clks.h"
#endif
#include "sl_wifi_callback_framework.h"

/******************************************************
 *                      Macros
 ******************************************************/
#define UDP_TX 0
#define UDP_RX 1
#define TCP_TX 2
#define TCP_RX 3

#define THROUGHPUT_TYPE TCP_RX

// 1 = async (Si91x callbacks), 0 = sync (BSD recv/recvfrom loop)
#define SOCKET_ASYNC_FEATURE 1

#define TCP_BUFFER_SIZE 1460
#define UDP_BUFFER_SIZE 1470

#if ((THROUGHPUT_TYPE == UDP_RX) || (THROUGHPUT_TYPE == UDP_TX))
#define BUFFER_SIZE UDP_BUFFER_SIZE
#elif ((THROUGHPUT_TYPE == TCP_RX) || (THROUGHPUT_TYPE == TCP_TX))
#define BUFFER_SIZE TCP_BUFFER_SIZE
#endif

#define SERVER_IP   "172.20.10.3"
#define SERVER_PORT 5000

#define SL_HIGH_PERFORMANCE_SOCKET BIT(7)

#define LISTENING_PORT 5005
#define BACK_LOG       1

#define BYTES_TO_SEND    (1 << 29) // 512 MB
#define BYTES_TO_RECEIVE (1 << 28) // 256 MB
#define TEST_TIMEOUT     (10000)   // 10 sec

#define AP_VAP 1

/*****************************************************
 *                 Socket configuration
 *****************************************************/
#define TOTAL_SOCKETS                   2
#define TOTAL_TCP_SOCKETS               2
#define TOTAL_UDP_SOCKETS               0
#define TCP_TX_ONLY_SOCKETS             0
#define TCP_RX_ONLY_SOCKETS             2
#define UDP_TX_ONLY_SOCKETS             0
#define UDP_RX_ONLY_SOCKETS             0
#define TCP_RX_HIGH_PERFORMANCE_SOCKETS 1
#define TCP_RX_WINDOW_SIZE_CAP          44
#define TCP_RX_WINDOW_DIV_FACTOR        44

/******************************************************
 *               Variable Definitions
 ******************************************************/

uint8_t data_buffer[BUFFER_SIZE];

static sl_si91x_socket_config_t socket_config = { TOTAL_SOCKETS,          TOTAL_TCP_SOCKETS,
                                                  TOTAL_UDP_SOCKETS,      TCP_TX_ONLY_SOCKETS,
                                                  TCP_RX_ONLY_SOCKETS,    UDP_TX_ONLY_SOCKETS,
                                                  UDP_RX_ONLY_SOCKETS,    TCP_RX_HIGH_PERFORMANCE_SOCKETS,
                                                  TCP_RX_WINDOW_SIZE_CAP, TCP_RX_WINDOW_DIV_FACTOR };

/******************************************************
 *               Function Declarations
 ******************************************************/
void receive_data_from_udp_client(void);
void send_data_to_udp_server(void);
void receive_data_from_tcp_client(void);
void send_data_to_tcp_server(void);
void throughput(void);
static void measure_and_print_throughput(uint32_t total_num_of_bytes, uint32_t test_timeout);

/******************************************************
 *               Function Definitions
 ******************************************************/
static void measure_and_print_throughput(uint32_t total_num_of_bytes, uint32_t test_timeout)
{
  float duration = ((test_timeout) / 1000.0f);
  float result   = ((float)total_num_of_bytes * 8) / duration; // bytes to bps
  result         = (result / 1000000);                         // bps to Mbps
  // NOTE: SL_DEBUG_LOG_V2 binary encoder doesn't support %f; keep level INFO and review later.
  SL_DEBUG_LOG_V2(INFO, "Throughput achieved @ %0.02f Mbps in %0.03f sec successfully\r\n", result, duration);
}

// Async callback state -- volatile because updated from firmware context
// and read by the throughput thread.
volatile uint8_t has_data_received = 0;
volatile uint32_t bytes_read       = 0;
volatile uint32_t start            = 0;
volatile uint32_t now              = 0;
volatile uint8_t first_data_frame  = 1;

void data_callback(uint32_t sock_no,
                   uint8_t *buffer,
                   uint32_t length,
                   const sl_si91x_socket_metadata_t *firmware_socket_response)
{
  UNUSED_PARAMETER(buffer);
  UNUSED_PARAMETER(firmware_socket_response);

  if (first_data_frame) {
    start = osKernelGetTickCount();
    SL_DEBUG_LOG_V2(INFO, "Client Socket ID : %ld\r\n", sock_no);
#if ((THROUGHPUT_TYPE == UDP_RX) || (THROUGHPUT_TYPE == TCP_RX))
    switch (THROUGHPUT_TYPE) {
      case UDP_RX:
        SL_DEBUG_LOG_V2(INFO, "UDP_RX Throughput test start\r\n");
        break;
      case TCP_RX:
        SL_DEBUG_LOG_V2(INFO, "TCP_RX Throughput test start\r\n");
        break;
    }
#endif
    first_data_frame = 0;
  }

  bytes_read += length;
  now = osKernelGetTickCount();
  if ((bytes_read > BYTES_TO_RECEIVE) || ((now - start) > TEST_TIMEOUT)) {
    has_data_received = 1;
  }
}

void throughput()
{
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
    default:
      SL_DEBUG_LOG_V2(ERROR, "Invalid Throughput test");
  }

  SL_DEBUG_LOG_V2(INFO, "Throughput test completed\r\n");
  while (true) {
    osDelay(osWaitForever);
  }
}

void send_data_to_tcp_server(void)
{
  int client_socket                 = -1;
  uint32_t total_bytes_sent         = 0;
  int socket_return_value           = 0;
  int sent_bytes                    = 1;
  uint32_t tx_start                 = 0;
  uint32_t tx_now                   = 0;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);
  server_address.sin_family         = AF_INET;
  server_address.sin_port           = SERVER_PORT;

  sl_status_t addr_status = sl_net_inet_addr(SERVER_IP, &server_address.sin_addr.s_addr);
  if (addr_status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Invalid SERVER_IP address");
    return;
  }

  client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket Create failed with bsd error: %d", errno);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Socket ID : %d\r\n", client_socket);

  socket_return_value = connect(client_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket Connect failed with bsd error: %d", errno);
    close(client_socket);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Socket connected to TCP server");

  SL_DEBUG_LOG_V2(INFO, "TCP_TX Throughput test start");
  tx_start = osKernelGetTickCount();
  while (total_bytes_sent < BYTES_TO_SEND) {
    sent_bytes = send(client_socket, data_buffer, TCP_BUFFER_SIZE, 0);
    tx_now     = osKernelGetTickCount();
    if (sent_bytes < 0) {
      if (errno == ENOBUFS)
        continue;
      SL_DEBUG_LOG_V2(ERROR, "Socket send failed with bsd error: %d", errno);
      break;
    }
    total_bytes_sent = total_bytes_sent + sent_bytes;

    if ((tx_now - tx_start) > TEST_TIMEOUT) {
      SL_DEBUG_LOG_V2(INFO, "Time Out: %ld", (tx_now - tx_start));
      break;
    }
  }
  SL_DEBUG_LOG_V2(INFO, "TCP_TX Throughput test finished");
  SL_DEBUG_LOG_V2(INFO, "Total bytes sent : %ld", total_bytes_sent);

  measure_and_print_throughput(total_bytes_sent, (tx_now - tx_start));

  close(client_socket);
}

void receive_data_from_tcp_client(void)
{
  int server_socket                 = -1;
  int client_socket                 = -1;
  int socket_return_value           = 0;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);
  uint32_t high_performance_socket  = SL_HIGH_PERFORMANCE_SOCKET;
  uint8_t ap_vap                    = AP_VAP;

  sl_status_t status = sl_si91x_config_socket(socket_config);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Socket config failed: 0x%lx", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Socket config Done\r\n");

#if SOCKET_ASYNC_FEATURE
  server_socket = sl_si91x_socket_async(AF_INET, SOCK_STREAM, IPPROTO_TCP, &data_callback);
  if (server_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket creation failed with bsd error: %d\r\n", errno);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Server Socket ID : %d\r\n", server_socket);

  socket_return_value = sl_si91x_setsockopt(server_socket,
                                            SOL_SOCKET,
                                            SL_SI91X_SO_HIGH_PERFORMANCE_SOCKET,
                                            &high_performance_socket,
                                            sizeof(high_performance_socket));
  if (socket_return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Set Socket option failed with bsd error: %d\r\n", errno);
    close(server_socket);
    return;
  }

  socket_return_value =
    sl_si91x_setsockopt(server_socket, SOL_SOCKET, SL_SI91X_SO_SOCK_VAP_ID, &ap_vap, sizeof(ap_vap));

  server_address.sin_family = AF_INET;
  server_address.sin_port   = LISTENING_PORT;

  socket_return_value = sl_si91x_bind(server_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket bind failed with bsd error: %d\r\n", errno);
    close(server_socket);
    return;
  }

  socket_return_value = sl_si91x_listen(server_socket, BACK_LOG);
  if (socket_return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket listen failed with bsd error:%d\r\n", errno);
    close(server_socket);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Listening on Local Port : %d\r\n", LISTENING_PORT);

  client_socket = sl_si91x_accept(server_socket, NULL, 0);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket accept failed with bsd error: %d\r\n", errno);
    close(server_socket);
    return;
  }

  while (!has_data_received) {
    osThreadYield();
  }

  now = osKernelGetTickCount();

  SL_DEBUG_LOG_V2(INFO, "TCP_RX Throughput test finished\r\n");
  SL_DEBUG_LOG_V2(INFO, "Total bytes received : %ld\r\n", bytes_read);

  // Close accepted socket before listening socket (required on NCP & SOC)
  close(client_socket);
  close(server_socket);

  measure_and_print_throughput(bytes_read, (now - start));
#else
  int read_bytes                = 1;
  uint32_t total_bytes_received = 0;
  uint32_t rx_start             = 0;
  uint32_t rx_now               = 0;

  server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (server_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket creation failed with bsd error: %d\r\n", errno);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Server Socket ID : %d\r\n", server_socket);

  socket_return_value = setsockopt(server_socket,
                                   SOL_SOCKET,
                                   SL_SO_HIGH_PERFORMANCE_SOCKET,
                                   &high_performance_socket,
                                   sizeof(high_performance_socket));
  if (socket_return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Set Socket option failed with bsd error: %d\r\n", errno);
    close(server_socket);
    return;
  }

  socket_return_value =
    sl_si91x_setsockopt(server_socket, SOL_SOCKET, SL_SI91X_SO_SOCK_VAP_ID, &ap_vap, sizeof(ap_vap));

  server_address.sin_family = AF_INET;
  server_address.sin_port   = LISTENING_PORT;

  socket_return_value = bind(server_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket bind failed with bsd error: %d\r\n", errno);
    close(server_socket);
    return;
  }

  socket_return_value = listen(server_socket, BACK_LOG);
  if (socket_return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket listen failed with bsd error: %d\r\n", errno);
    close(server_socket);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Listening on Local Port : %d\r\n", LISTENING_PORT);

  client_socket = accept(server_socket, NULL, NULL);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket accept failed with bsd error: %d\r\n", errno);
    close(server_socket);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Client Socket ID : %d\r\n", client_socket);

  SL_DEBUG_LOG_V2(INFO, "TCP_RX Throughput test start\r\n");
  rx_start = osKernelGetTickCount();
  while (1) {
    read_bytes = recv(client_socket, data_buffer, sizeof(data_buffer), 0);
    if (read_bytes < 0) {
      if (errno == 0) {
        status = sl_wifi_get_saved_firmware_status();
        if (status == SL_STATUS_SI91X_MEMORY_FAILED_FROM_MODULE) {
          continue;
        } else {
          SL_DEBUG_LOG_V2(ERROR, "recv failed with BSD error = %d and status = 0x%lx\r\n", errno, status);
        }
      } else {
        SL_DEBUG_LOG_V2(ERROR, "recv failed with BSD error = %d\r\n", errno);
      }
      break;
    }
    total_bytes_received = total_bytes_received + read_bytes;
    rx_now               = osKernelGetTickCount();

    if ((rx_now - rx_start) > TEST_TIMEOUT) {
      SL_DEBUG_LOG_V2(INFO, "Test Time Out: %ld ms\r\n", (rx_now - rx_start));
      break;
    }
  }
  SL_DEBUG_LOG_V2(INFO, "TCP_RX Throughput test finished\r\n");
  SL_DEBUG_LOG_V2(INFO, "Total bytes received : %ld\r\n", total_bytes_received);

  measure_and_print_throughput(total_bytes_received, (rx_now - rx_start));

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
  uint32_t tx_start                 = 0;
  uint32_t tx_now                   = 0;
  server_address.sin_family         = AF_INET;
  server_address.sin_port           = SERVER_PORT;

  sl_status_t addr_status = sl_net_inet_addr(SERVER_IP, &server_address.sin_addr.s_addr);
  if (addr_status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Invalid SERVER_IP address");
    return;
  }

  client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket creation failed with bsd error: %d\r\n", errno);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Socket ID : %d\r\n", client_socket);

  SL_DEBUG_LOG_V2(INFO, "UDP_TX Throughput test start");
  tx_start = osKernelGetTickCount();
  while (total_bytes_sent < BYTES_TO_SEND) {
    sent_bytes =
      sendto(client_socket, data_buffer, UDP_BUFFER_SIZE, 0, (struct sockaddr *)&server_address, socket_length);
    tx_now = osKernelGetTickCount();
    if ((tx_now - tx_start) > TEST_TIMEOUT) {
      SL_DEBUG_LOG_V2(INFO, "Time Out: %ld", (tx_now - tx_start));
      break;
    }
    if (sent_bytes < 0) {
      if (errno == ENOBUFS)
        continue;
      SL_DEBUG_LOG_V2(ERROR, "Socket send failed with bsd error: %d", errno);
      close(client_socket);
      break;
    }
    total_bytes_sent = total_bytes_sent + sent_bytes;
  }
  SL_DEBUG_LOG_V2(INFO, "UDP_TX Throughput test finished");
  SL_DEBUG_LOG_V2(INFO, "Total bytes sent : %ld", total_bytes_sent);

  measure_and_print_throughput(total_bytes_sent, (tx_now - tx_start));

  close(client_socket);
}

void receive_data_from_udp_client(void)
{
  int client_socket = -1;

  int socket_return_value           = 0;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);

#if SOCKET_ASYNC_FEATURE
  client_socket = sl_si91x_socket_async(AF_INET, SOCK_DGRAM, IPPROTO_UDP, &data_callback);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket creation failed with bsd error: %d\r\n", errno);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Socket ID : %d\r\n", client_socket);

  server_address.sin_family = AF_INET;
  server_address.sin_port   = LISTENING_PORT;

  socket_return_value = sl_si91x_bind(client_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket bind failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Listening on Local Port %d\r\n", LISTENING_PORT);

  while (!has_data_received) {
    osThreadYield();
  }
  now = osKernelGetTickCount();
  SL_DEBUG_LOG_V2(INFO, "UDP_RX Async Throughput test finished\r\n");
  SL_DEBUG_LOG_V2(INFO, "Total bytes received : %ld\r\n", bytes_read);

  measure_and_print_throughput(bytes_read, (now - start));

  close(client_socket);
#else
  sl_status_t status            = SL_STATUS_OK;
  int read_bytes                = 1;
  uint32_t total_bytes_received = 0;
  uint32_t rx_start             = 0;
  uint32_t rx_now               = 0;

  client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket creation failed with bsd error: %d\r\n", errno);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Socket ID : %d\r\n", client_socket);

  server_address.sin_family = AF_INET;
  server_address.sin_port   = LISTENING_PORT;

  socket_return_value = bind(client_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket bind failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Listening on Local Port %d\r\n", LISTENING_PORT);

  SL_DEBUG_LOG_V2(INFO, "UDP_RX Throughput test start\r\n");
  rx_start = osKernelGetTickCount();
  while (total_bytes_received < BYTES_TO_RECEIVE) {
    read_bytes = recvfrom(client_socket, data_buffer, sizeof(data_buffer), 0, NULL, NULL);
    if (read_bytes < 0) {
      if (errno == 0) {
        status = sl_wifi_get_saved_firmware_status();
        if (status == SL_STATUS_SI91X_MEMORY_FAILED_FROM_MODULE) {
          continue;
        } else {
          SL_DEBUG_LOG_V2(ERROR, "recv failed with BSD error = %d and status = 0x%lx\r\n", errno, status);
        }
      } else {
        SL_DEBUG_LOG_V2(ERROR, "recv failed with BSD error = %d\r\n", errno);
      }
      break;
    }

    total_bytes_received = total_bytes_received + read_bytes;
    rx_now               = osKernelGetTickCount();
    if ((rx_now - rx_start) > TEST_TIMEOUT) {
      SL_DEBUG_LOG_V2(INFO, "Test Time Out: %ld ms\r\n", (rx_now - rx_start));
      break;
    }
  }
  SL_DEBUG_LOG_V2(INFO, "UDP_RX Throughput test finished\r\n");
  SL_DEBUG_LOG_V2(INFO, "Total bytes received : %ld\r\n", total_bytes_received);

  measure_and_print_throughput(total_bytes_received, (rx_now - rx_start));

  close(client_socket);
#endif
}
