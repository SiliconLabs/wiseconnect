/*******************************************************************************
* @file  wifi_throughput.c
* @brief
*******************************************************************************
* # License
* <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* The licensor of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/
/**
 * @file    wifi_throughput.c
 * @version 0.1
 * @date    01 Feb 2021
 *
 *  @brief : This file manages the WLAN connections to remote server, SSL/TCP/UDP data transfers and throughput measurements
 *
 *  @section Description  This file contains code to handle connections between server and client,
 *  					   handles SSL/TCP/UDP data transfers and throughput measurements
 *
 */

/*=======================================================================*/
//  ! INCLUDES
/*=======================================================================*/

#include "app_common_config.h"

#if RSI_ENABLE_WLAN_TEST
#include "stdlib.h"
#include "wifi_app_config.h"

//! SL Wi-Fi SDK includes
#include "sl_constants.h"
#include "sl_wifi.h"
#include "sl_wifi_callback_framework.h"
#include "sl_net.h"
#include "sl_net_si91x.h"
#include "sl_utility.h"
#include "sl_tls.h"

#include "errno.h"
#include "socket.h"
#include "sl_si91x_socket_support.h"

#include "cmsis_os2.h"
#include <rsi_common_apis.h>
#include <string.h>

/*=======================================================================*/
//   ! MACROS
/*=======================================================================*/

#define BACK_LOG                   1
#define SL_HIGH_PERFORMANCE_SOCKET BIT(7)

#if ((THROUGHPUT_TYPE == UDP_RX) || (THROUGHPUT_TYPE == UDP_TX))
#define BUFFER_SIZE UDP_BUFFER_SIZE
#elif ((THROUGHPUT_TYPE == TCP_RX) || (THROUGHPUT_TYPE == TCP_TX))
#define BUFFER_SIZE TCP_BUFFER_SIZE
#elif ((THROUGHPUT_TYPE == SSL_RX) || (THROUGHPUT_TYPE == SSL_TX))
#define BUFFER_SIZE SSL_BUFFER_SIZE
#else
#define BUFFER_SIZE DEFAULT_BUFFER_SIZE
#endif
/*=======================================================================*/
//   ! GLOBAL VARIABLES
/*=======================================================================*/
//! Throughput parameters
uint8_t data_buffer[BUFFER_SIZE];

/*=======================================================================*/
//   ! EXTERN VARIABLES
/*=======================================================================*/

/*=======================================================================*/
//   ! EXTERN FUNCTIONS
/*=======================================================================*/

/*=======================================================================*/
//   ! PROCEDURES
/*=======================================================================*/
uint32_t tick_count_s = 1;
#define THROUGHPUT_AVG_TIME (20000 * tick_count_s) // 20sec

static void measure_and_print_throughput(uint32_t total_num_of_bytes, uint32_t test_timeout)
{
  float duration = ((test_timeout) / 1000) / tick_count_s; // ms to sec
  float result   = (total_num_of_bytes * 8) / duration;    // bytes to bits
  result         = (result / 1000000);                     // bps to Mbps
  printf("\r\nThroughput achieved @ %0.02f Mbps in %0.03f sec successfully\r\n", result, duration);
}

/*************************************************************************/
//!  CALLBACK FUNCTIONS
/*************************************************************************/
void send_data_to_udp_server(void)
{
  int client_socket                 = -1;
  uint32_t total_bytes_sent         = 0;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);
  int sent_bytes                    = 1;
  uint32_t start                    = 0;
  uint32_t now                      = 0;
  uint32_t packets                  = 0;

  server_address.sin_family = AF_INET;
  server_address.sin_port   = SERVER_PORT;
  sl_net_inet_addr(SERVER_IP_ADDRESS, &server_address.sin_addr.s_addr);

  client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (client_socket < 0) {
    printf("\r\nSocket creation failed with BSD error: %d\r\n", errno);
    return;
  }
  printf("\r\nSocket ID : %d\r\n", client_socket);

  printf("\r\nUDP_TX Throughput test start\r\n");
  start = osKernelGetTickCount();
  while (1) {
    sent_bytes =
      sendto(client_socket, data_buffer, UDP_BUFFER_SIZE, 0, (struct sockaddr *)&server_address, socket_length);
    if (sent_bytes < 0) {
      if (errno == ENOTCONN) {
        printf("\nRemote server terminated\n");
      } else {
        printf("\nUDP send failed with BSD error : %d\n", errno);
      }
      close(client_socket);
      return;
    } else if (sent_bytes > 0) {
      total_bytes_sent = total_bytes_sent + sent_bytes;
      packets++;
    }

    now = osKernelGetTickCount();
    if ((now - start) >= THROUGHPUT_AVG_TIME) {
      printf("\r\nNumber of packets sent : %ld\r\n", packets);
      printf("\r\nTotal bytes sent : %ld\r\n", total_bytes_sent);
      measure_and_print_throughput(total_bytes_sent, (now - start));
#if CONTINUOUS_THROUGHPUT
      total_bytes_sent = 0;
      packets          = 0;
      start            = osKernelGetTickCount();
#else
      break;
#endif
    }
  }

  printf("\r\nUDP_TX Throughput test finished\r\n");
  close(client_socket);
}

void receive_data_from_udp_client(void)
{
  int client_socket                 = -1;
  uint32_t total_bytes_received     = 0;
  int socket_return_value           = 0;
  int read_bytes                    = 1;
  uint32_t start                    = 0;
  uint32_t now                      = 0;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);

  client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (client_socket < 0) {
    printf("\r\nSocket creation failed with bsd error: %d\r\n", errno);
    return;
  }
  printf("\r\nSocket ID : %d\r\n", client_socket);

  server_address.sin_family = AF_INET;
  server_address.sin_port   = DEVICE_PORT;

  socket_return_value = bind(client_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    printf("\r\nSocket bind failed with BSD error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  printf("\r\nListening on Local Port %d\r\n", DEVICE_PORT);

  printf("\r\nUDP_RX Throughput test start\r\n");
  start = osKernelGetTickCount();
  while (1) {
    read_bytes = recvfrom(client_socket, data_buffer, sizeof(data_buffer), 0, NULL, NULL);
    if (read_bytes < 0) {
      if (errno == ENOTCONN) {
        printf("\nRemote server terminated\n");
      } else {
        printf("\nUDP recv failed with BSD error : %d\n", errno);
      }
      close(client_socket);
      return;
    } else if (read_bytes > 0) {
      total_bytes_received = total_bytes_received + read_bytes;
    }

    now = osKernelGetTickCount();
    if ((now - start) > THROUGHPUT_AVG_TIME) {
      printf("\r\nTotal bytes received : %ld\r\n", total_bytes_received);
      measure_and_print_throughput(total_bytes_received, (now - start));
#if CONTINUOUS_THROUGHPUT
      total_bytes_received = 0;
      start                = osKernelGetTickCount();
#else
      break;
#endif
    }
  }

  printf("\r\nUDP_RX Throughput test finished\r\n");
  close(client_socket);
  return;
}

void send_data_to_tcp_server(void)
{
  int client_socket                 = -1;
  uint32_t total_bytes_sent         = 0;
  int socket_return_value           = 0;
  int sent_bytes                    = 1;
  uint32_t start                    = 0;
  uint32_t now                      = 0;
  uint32_t packets                  = 0;
  struct sockaddr_in server_address = { 0 };

  server_address.sin_family = AF_INET;
  server_address.sin_port   = SERVER_PORT;
  sl_net_inet_addr(SERVER_IP_ADDRESS, &server_address.sin_addr.s_addr);

  client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (client_socket < 0) {
    printf("\r\nSocket Create failed with BSD error: %d\r\n", errno);
    return;
  }
  printf("\r\nSocket ID : %d\r\n", client_socket);

  socket_return_value = connect(client_socket, (struct sockaddr *)&server_address, sizeof(struct sockaddr_in));
  if (socket_return_value < 0) {
    printf("\r\nSocket Connect failed with BSD error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  printf("\r\nSocket connected to TCP server\r\n");

  //! while running BLE throughput test, return if only wlan connection is configured
#if (BLE_THROUGHPUT_TEST && WLAN_CONNECT_ONLY)
  return;
#endif

  printf("\r\nTCP_TX Throughput test start\r\n");
  start = osKernelGetTickCount();
  while (1) {
    sent_bytes = send(client_socket, data_buffer, TCP_BUFFER_SIZE, 0);
    if (sent_bytes < 0) {
      if (errno == ENOTCONN) {
        printf("\nRemote server terminated\n");
      } else {
        printf("\nTCP send failed with BSD error : %d\n", errno);
      }
      close(client_socket);
      return;
    } else if (sent_bytes > 0) {
      total_bytes_sent = total_bytes_sent + sent_bytes;
      packets++;
    }

//! while running BLE throughput test, don't measure the wlan throughput
#if (BLE_THROUGHPUT_TEST && WLAN_DATATRANSFER && CONTINUOUS_THROUGHPUT)
    continue;
#endif
    now = osKernelGetTickCount();
    if ((now - start) >= THROUGHPUT_AVG_TIME) {
      printf("\r\nNumber of packets sent : %ld\r\n", packets);
      printf("\r\nTotal bytes sent : %ld\r\n", total_bytes_sent);
      measure_and_print_throughput(total_bytes_sent, (now - start));
#if CONTINUOUS_THROUGHPUT
      total_bytes_sent = 0;
      packets          = 0;
      start            = osKernelGetTickCount();
#else
      break;
#endif
    }
  }

  printf("\r\nTCP_TX Throughput test finished\r\n");
  close(client_socket);

  return;
}

void receive_data_from_tcp_client(void)
{
  int server_socket                 = -1;
  int client_socket                 = -1;
  int socket_return_value           = 0;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);
  uint8_t high_performance_socket   = SL_HIGH_PERFORMANCE_SOCKET;
  uint32_t total_bytes_received     = 0;
  uint32_t start                    = 0;
  uint32_t now                      = 0;
  int read_bytes                    = 1;

  server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (server_socket < 0) {
    printf("\r\nSocket creation failed with bsd error: %d\r\n", errno);
    return;
  }
  printf("\r\nServer Socket ID : %d\r\n", server_socket);

  socket_return_value = sl_si91x_set_custom_sync_sockopt(server_socket,
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
  server_address.sin_port   = DEVICE_PORT;

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
  printf("\r\nListening on Local Port : %d\r\n", DEVICE_PORT);

  client_socket = accept(server_socket, NULL, NULL);
  if (client_socket < 0) {
    printf("\r\nSocket accept failed with bsd error: %d\r\n", errno);
    close(server_socket);
    return;
  }
  printf("\r\nClient Socket ID : %d\r\n", client_socket);

  printf("\r\nTCP_RX Throughput test start\r\n");
  start = osKernelGetTickCount();
  while (1) {
    read_bytes = recv(client_socket, data_buffer, sizeof(data_buffer), 0);
    if (read_bytes < 0) {
      if (errno == ENOTCONN) {
        printf("\nRemote server terminated\n");
      } else {
        printf("\nTCP recv failed with BSD error : %d\n", errno);
      }
      close(server_socket);
      close(client_socket);
      return;
    }

    total_bytes_received = total_bytes_received + read_bytes;
    now                  = osKernelGetTickCount();
    if ((now - start) > THROUGHPUT_AVG_TIME) {
      printf("\r\nTotal bytes received : %ld\r\n", total_bytes_received);
      measure_and_print_throughput(total_bytes_received, (now - start));
#if CONTINUOUS_THROUGHPUT
      total_bytes_received = 0;
      start                = osKernelGetTickCount();
#else
      break;
#endif
    }
  }

  printf("\r\nTCP_RX Throughput test finished\r\n");
  close(client_socket);
  close(server_socket);
  return;
}

void send_data_to_ssl_server(void)
{
  int client_socket                 = -1;
  int socket_return_value           = 0;
  uint32_t total_bytes_sent         = 0;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);
  uint32_t now                      = 0;
  uint32_t start                    = 0;
  uint32_t packets                  = 0;
  int sent_bytes                    = 0;

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
  server_address.sin_port   = SSL_SERVER_PORT;
  sl_net_inet_addr(SERVER_IP_ADDRESS, &server_address.sin_addr.s_addr);

  socket_return_value = connect(client_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    printf("\r\nSocket Connect failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  printf("\r\nSocket connected to SSL server\r\n");

  printf("\r\nSSL_TX Throughput test start\r\n");
  start = osKernelGetTickCount();
  while (1) {
    sent_bytes = send(client_socket, data_buffer, SSL_BUFFER_SIZE, 0);
    if (sent_bytes < 0) {
      if (errno == ENOTCONN) {
        printf("\nRemote server terminated\n");
      } else {
        printf("\nSSL send failed with BSD error : %d\n", errno);
      }
      close(client_socket);
      return;
    } else if (sent_bytes > 0) {
      total_bytes_sent = total_bytes_sent + sent_bytes;
      packets++;
    }

//! while running BLE throughput test, don't measure the wlan throughput
#if (BLE_THROUGHPUT_TEST && WLAN_DATATRANSFER && CONTINUOUS_THROUGHPUT)
    continue;
#endif
    now = osKernelGetTickCount();
    if ((now - start) > THROUGHPUT_AVG_TIME) {
      printf("\r\nTotal bytes sent : %ld\r\n", total_bytes_sent);
      printf("\r\nNumber of packets sent : %ld\r\n", packets);
      measure_and_print_throughput(total_bytes_sent, (now - start));
#if CONTINUOUS_THROUGHPUT
      total_bytes_sent = 0;
      packets          = 0;
      start            = osKernelGetTickCount();
#else
      break;
#endif
    }
  }

  printf("\r\nSSL_TX Throughput test finished\r\n");
  close(client_socket);

  return;
}

void receive_data_from_ssl_client(void)
{
  int client_socket                 = -1;
  int socket_return_value           = 0;
  uint32_t total_bytes_received     = 0;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);
  uint8_t high_performance_socket   = SL_HIGH_PERFORMANCE_SOCKET;

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

  socket_return_value = sl_si91x_set_custom_sync_sockopt(client_socket,
                                                         SOL_SOCKET,
                                                         SO_HIGH_PERFORMANCE_SOCKET,
                                                         &high_performance_socket,
                                                         sizeof(high_performance_socket));
  if (socket_return_value < 0) {
    printf("\r\nSet Socket option failed with BSD error: %d\r\n", errno);
    close(client_socket);
    return;
  }

  server_address.sin_family = AF_INET;
  server_address.sin_port   = SSL_SERVER_PORT;
  sl_net_inet_addr(SERVER_IP_ADDRESS, &server_address.sin_addr.s_addr);

  socket_return_value = connect(client_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    printf("\r\nSocket Connect failed with BSD error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  printf("\r\nSocket connected to SSL server\r\n");

  printf("\r\nSSL_RX Throughput test start\r\n");
  uint32_t start = osKernelGetTickCount();
  uint32_t now   = start;
  int read_bytes = 1;
  while (1) {
    read_bytes = recv(client_socket, data_buffer, sizeof(data_buffer), 0);
    if (read_bytes < 0) {
      if (errno == ENOTCONN) {
        printf("\nRemote server terminated\n");
      } else {
        printf("\nSSL recv failed with BSD error : %d\n", errno);
      }
      close(client_socket);
      return;
    }
    if (read_bytes > 0) {
      total_bytes_received = total_bytes_received + read_bytes;
    }
    now = osKernelGetTickCount();
    if ((now - start) > THROUGHPUT_AVG_TIME) {
      printf("\r\nTotal bytes received : %ld\r\n", total_bytes_received);
      measure_and_print_throughput(total_bytes_received, (now - start));
#if CONTINUOUS_THROUGHPUT
      total_bytes_received = 0;
      start                = osKernelGetTickCount();
#else
      break;
#endif
    }
  }

  printf("\r\nSSL_RX Throughput test finished\r\n");
  close(client_socket);
  return;
}

void wlan_throughput_task(void)
{
  memset(data_buffer, 'A', sizeof(data_buffer));

  switch (THROUGHPUT_TYPE) {
    case UDP_TX: {
      send_data_to_udp_server();
    } break;
    case UDP_RX: {
      receive_data_from_udp_client();
    } break;
    case TCP_TX: {
      send_data_to_tcp_server();
    } break;
    case TCP_RX: {
      receive_data_from_tcp_client();
    } break;
    case SSL_TX: {
      send_data_to_ssl_server();
    } break;
    case SSL_RX: {
      receive_data_from_ssl_client();
    } break;
  }
}
#endif
