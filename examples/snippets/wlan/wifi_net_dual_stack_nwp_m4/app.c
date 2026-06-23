/***************************************************************************/ /**
 * @file
 * @brief Dual Network Stack Example Application
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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

/*================================================================================
 * @brief : This file contains example application for Dual Network Stack

 * @section Description:
 * This application demonstrates the Dual Network Stack feature supporting both
 * IPv4 and IPv6 on offload (TA) and hosted LWIP network stacks simultaneously.
 *
 * Configurable modes:
 *   IPV6_MODE          - 0: IPv4, 1: IPv6
 *   TCP_MODE           - 0: UDP,  1: TCP
 *   TX_MODE            - 0: RX,   1: TX
 *   HOST_DATA_TRANSFER - 0: offload only, 1: offload + LWIP
 =================================================================================*/

#include "cmsis_os2.h"
#include "sl_net.h"
#include "sl_board_configuration.h"
#include "sl_utility.h"
#include "sl_si91x_driver.h"
#include "sl_wifi.h"
#include "sl_net_wifi_types.h"
#include "sl_net_default_values.h"
#include <string.h>

#include "lwip/errno.h"
#include "lwip/sockets.h"
#include "sl_net_for_lwip.h"
#include "sl_si91x_socket.h"
#include "sl_si91x_socket_constants.h"
#include "sl_si91x_socket_utility.h"

/******************************************************
 *                    Constants
 ******************************************************/

#define HOST_DATA_TRANSFER 1
#define IPV6_MODE          1
#define TCP_MODE           0
#define TX_MODE            0

#define LISTENING_PORT 5005
#define SERVER_PORT    5004

#define HOST_LISTENING_PORT 5002
#define HOST_SERVER_PORT    5001

#define LISTENING_PORT_V6 5007
#define SERVER_PORT_V6    5006

#define HOST_LISTENING_PORT_V6 5009
#define HOST_SERVER_PORT_V6    5008

#define BUFFER_SIZE        512
#define TCP_BUFFER_SIZE    1460
#define TCP_V6_BUFFER_SIZE 1024

#define BACKLOG          1
#define TEST_TIMEOUT     20000
#define BYTES_TO_SEND    (1 << 29)
#define BYTES_TO_RECEIVE (1 << 28)

#define SERVER_IP    "10.252.245.81"
#define SERVER_IP_V6 "2409:40f0:d2:8197:ad79:8ee1:799:d1b4"

/******************************************************
 *               Socket Configuration
 ******************************************************/
#if TCP_MODE
#define TOTAL_SOCKETS     3
#define TOTAL_TCP_SOCKETS 3
#define TOTAL_UDP_SOCKETS 0
#if TX_MODE
#define TCP_TX_ONLY_SOCKETS 3
#define TCP_RX_ONLY_SOCKETS 0
#else
#define TCP_TX_ONLY_SOCKETS 0
#define TCP_RX_ONLY_SOCKETS 3
#endif
#define UDP_TX_ONLY_SOCKETS 0
#define UDP_RX_ONLY_SOCKETS 0
#else
#define TOTAL_SOCKETS       3
#define TOTAL_TCP_SOCKETS   0
#define TOTAL_UDP_SOCKETS   3
#define TCP_TX_ONLY_SOCKETS 0
#define TCP_RX_ONLY_SOCKETS 0
#if TX_MODE
#define UDP_TX_ONLY_SOCKETS 3
#define UDP_RX_ONLY_SOCKETS 0
#else
#define UDP_TX_ONLY_SOCKETS 0
#define UDP_RX_ONLY_SOCKETS 3
#endif
#endif
#define TCP_RX_HIGH_PERFORMANCE_SOCKETS 0
#define TCP_RX_WINDOW_SIZE_CAP          10
#define TCP_RX_WINDOW_DIV_FACTOR        10

static sl_si91x_socket_config_t socket_config = { TOTAL_SOCKETS,          TOTAL_TCP_SOCKETS,
                                                  TOTAL_UDP_SOCKETS,      TCP_TX_ONLY_SOCKETS,
                                                  TCP_RX_ONLY_SOCKETS,    UDP_TX_ONLY_SOCKETS,
                                                  UDP_RX_ONLY_SOCKETS,    TCP_RX_HIGH_PERFORMANCE_SOCKETS,
                                                  TCP_RX_WINDOW_SIZE_CAP, TCP_RX_WINDOW_DIV_FACTOR };

/******************************************************
 *               Variable Definitions
 ******************************************************/
uint8_t data_buffer[TCP_BUFFER_SIZE];

const osThreadAttr_t thread_attributes = {
  .name       = "app",
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = 3072,
  .priority   = osPriorityLow,
  .tz_module  = 0,
};

const osThreadAttr_t traffic_thread_attributes = {
  .name       = "traffic_thread",
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = 2048,
  .priority   = osPriorityLow,
  .tz_module  = 0,
};

static const sl_wifi_device_configuration_t client_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .region_code = US,
  .boot_config = { .oper_mode = SL_SI91X_CLIENT_MODE,
                   .coex_mode = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map =
                     (SL_WIFI_FEAT_SECURITY_OPEN | SL_WIFI_FEAT_AGGREGATION | SL_SI91X_FEAT_ULP_GPIO_BASED_HANDSHAKE
#ifdef SLI_SI91X_MCU_INTERFACE
                      | SL_WIFI_FEAT_WPS_DISABLE
#endif
                      ),
                   .tcp_ip_feature_bit_map = (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT | SL_SI91X_TCP_IP_FEAT_DHCPV6_CLIENT
                                              | SL_SI91X_TCP_IP_FEAT_IPV6 | SL_SI91X_TCP_IP_FEAT_DNS_CLIENT
                                              | SL_SI91X_TCP_IP_FEAT_ICMP | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID),
                   .custom_feature_bit_map = SL_WIFI_SYSTEM_CUSTOM_FEAT_EXTENSION_VALID,
                   .ext_custom_feature_bit_map = (SL_WIFI_SYSTEM_EXT_FEAT_LOW_POWER_MODE | SL_SI91X_EXT_FEAT_XTAL_CLK
                                                  | SL_SI91X_EXT_FEAT_UART_SEL_FOR_DEBUG_PRINTS | MEMORY_CONFIG
#ifdef SLI_SI917
                                                  | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
                                                  ),
                   .bt_feature_bit_map = 0,
                   .ext_tcp_ip_feature_bit_map =
                     (SL_SI91X_CONFIG_FEAT_EXTENSION_VALID | SL_SI91X_EXT_TCP_IP_DUAL_MODE_ENABLE),
                   .ble_feature_bit_map     = 0,
                   .ble_ext_feature_bit_map = 0,
                   .config_feature_bit_map  = SL_SI91X_FEAT_SLEEP_GPIO_SEL_BITMAP }
};

static sl_net_wifi_client_profile_t wifi_client_profile = {
    .config =
        {
            .ssid.value = DEFAULT_WIFI_CLIENT_PROFILE_SSID,
            .ssid.length = sizeof(DEFAULT_WIFI_CLIENT_PROFILE_SSID) - 1,
            .channel = {.channel = SL_WIFI_AUTO_CHANNEL,
                        .band = SL_WIFI_AUTO_BAND,
                        .bandwidth = SL_WIFI_AUTO_BANDWIDTH},
            .bssid = {{0}},
            .bss_type = SL_WIFI_BSS_TYPE_INFRASTRUCTURE,
            .security = SL_WIFI_WPA2,
            .encryption = SL_WIFI_DEFAULT_ENCRYPTION,
            .credential_id = SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID,
        },
    .ip = {
        .mode = SL_IP_MANAGEMENT_DHCP,
        .type = (SL_IPV4 | SL_IPV6),
        .host_name = NULL
    }};

static sl_net_wifi_lwip_context_t wifi_client_context;

/******************************************************
 *               Function Declarations
 ******************************************************/
static void application_start(void *argument);

/******************************************************
 *               Helper Functions
 ******************************************************/

static void print_ip_addresses(sl_net_wifi_client_profile_t *profile)
{
  sl_ip_address_t ip_address = { 0 };

  if ((profile->ip.type & SL_IPV4) == SL_IPV4) {
    ip_address.type = SL_IPV4;
    memcpy(&ip_address.ip.v4.bytes, &profile->ip.ip.v4.ip_address.bytes, sizeof(sl_ipv4_address_t));
    SL_DEBUG_LOG_V2(INFO, "IPv4 Address: ");
    print_sl_ip_address(&ip_address);
  }

  if ((profile->ip.type & SL_IPV6) == SL_IPV6) {
    ip_address.type = SL_IPV6;
    memcpy(&ip_address.ip.v6.bytes, &profile->ip.ip.v6.global_address.bytes, sizeof(sl_ipv6_address_t));
    SL_DEBUG_LOG_V2(INFO, "IPv6 Address: ");
    print_sl_ip_address(&ip_address);
  }
}

/******************************************************
 *               Offload Data Callback
 ******************************************************/

volatile uint8_t has_data_received = 0;
volatile uint32_t bytes_read       = 0;
uint8_t first_data_frame           = 1;
uint32_t start                     = 0;
uint32_t now                       = 0;

void data_callback(uint32_t sock_no,
                   uint8_t *buffer,
                   uint32_t length,
                   const sl_si91x_socket_metadata_t *firmware_socket_response)
{
  UNUSED_PARAMETER(sock_no);
  UNUSED_PARAMETER(buffer);
  UNUSED_PARAMETER(firmware_socket_response);

  if (first_data_frame) {
    start            = osKernelGetTickCount();
    first_data_frame = 0;
    if (TCP_MODE) {
      SL_DEBUG_LOG_V2(INFO, "Offload TCP_RX test start\r\n");
    } else {
      SL_DEBUG_LOG_V2(INFO, "Offload UDP_RX test start\r\n");
    }
  }

  bytes_read += length;
  now = osKernelGetTickCount();
  if ((bytes_read > BYTES_TO_RECEIVE) || ((now - start) > TEST_TIMEOUT)) {
    has_data_received = 1;
  }
}

/******************************************************
 *     Offload Stack - IPv4 Functions
 ******************************************************/

void send_data_to_tcp_server(void)
{
  int client_socket                 = -1;
  uint32_t total_bytes_sent         = 0;
  int sent_bytes                    = 1;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);

  server_address.sin_family = AF_INET;
  server_address.sin_port   = SERVER_PORT;
  sl_net_inet_addr(SERVER_IP, &server_address.sin_addr.s_addr);

  client_socket = sl_si91x_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[Offload IPv4] Socket create failed: %d\r\n", errno);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[Offload IPv4] Socket ID: %d\r\n", client_socket);

  if (sl_si91x_connect(client_socket, (struct sockaddr *)&server_address, socket_length) < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[Offload IPv4] Connect failed: %d\r\n", errno);
    sl_si91x_shutdown(client_socket, 0);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[Offload IPv4] Connected to TCP server\r\n");
  SL_DEBUG_LOG_V2(INFO, "[Offload IPv4] TCP_TX test start\r\n");

  start = osKernelGetTickCount();
  while (total_bytes_sent < BYTES_TO_SEND) {
    sent_bytes = sl_si91x_send(client_socket, data_buffer, TCP_BUFFER_SIZE, 0);
    now        = osKernelGetTickCount();
    if (sent_bytes < 0) {
      if (errno == ENOBUFS || errno == EAGAIN || errno == EWOULDBLOCK) {
        osDelay(1);
        continue;
      }
      SL_DEBUG_LOG_V2(ERROR, "[Offload IPv4] Send failed: errno=%d\r\n", errno);
      break;
    }
    total_bytes_sent += sent_bytes;
    if ((now - start) > TEST_TIMEOUT) {
      SL_DEBUG_LOG_V2(INFO, "[Offload IPv4] Timeout\r\n");
      break;
    }
  }

  SL_DEBUG_LOG_V2(INFO, "[Offload IPv4] TCP_TX test finished\r\n");
  SL_DEBUG_LOG_V2(INFO, "[Offload IPv4] Total bytes sent: %lu\r\n", total_bytes_sent);

  sl_si91x_shutdown(client_socket, 0);
}

void receive_data_from_tcp_client(void)
{
  has_data_received = 0;
  bytes_read        = 0;
  first_data_frame  = 1;

  int server_socket                 = -1;
  int client_socket                 = -1;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);

  server_socket = sl_si91x_socket_async(AF_INET, SOCK_STREAM, IPPROTO_TCP, &data_callback);
  if (server_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[Offload IPv4] Socket create failed: %d\r\n", errno);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[Offload IPv4] Server Socket ID: %d\r\n", server_socket);

  server_address.sin_family      = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port        = LISTENING_PORT;

  if (sl_si91x_bind(server_socket, (struct sockaddr *)&server_address, socket_length) < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[Offload IPv4] Bind failed: %d\r\n", errno);
    sl_si91x_shutdown(server_socket, 0);
    return;
  }

  if (sl_si91x_listen(server_socket, BACKLOG) < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[Offload IPv4] Listen failed: %d\r\n", errno);
    sl_si91x_shutdown(server_socket, 0);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[Offload IPv4] Listening on port %d\r\n", LISTENING_PORT);

  client_socket = sl_si91x_accept(server_socket, NULL, 0);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[Offload IPv4] Accept failed: %d\r\n", errno);
    sl_si91x_shutdown(server_socket, 0);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[Offload IPv4] Client Socket ID: %d\r\n", client_socket);

  start = osKernelGetTickCount();
  while (!has_data_received) {
    if ((osKernelGetTickCount() - start) > TEST_TIMEOUT) {
      SL_DEBUG_LOG_V2(WARN, "[Offload IPv4] TCP_RX timeout (no data received)\r\n");
      break;
    }
    osThreadYield();
  }

  SL_DEBUG_LOG_V2(INFO, "[Offload IPv4] TCP_RX test finished\r\n");
  SL_DEBUG_LOG_V2(INFO, "[Offload IPv4] Total bytes received: %lu\r\n", bytes_read);

  sl_si91x_shutdown(client_socket, 0);
  sl_si91x_shutdown(server_socket, 0);
}

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

  client_socket = sl_si91x_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[Offload IPv4] Socket create failed: %d\r\n", errno);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[Offload IPv4] Socket ID: %d\r\n", client_socket);
  SL_DEBUG_LOG_V2(INFO, "[Offload IPv4] UDP_TX test start\r\n");

  start = osKernelGetTickCount();
  while (total_bytes_sent < BYTES_TO_SEND) {
    sent_bytes =
      sl_si91x_sendto(client_socket, data_buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_address, socket_length);
    now = osKernelGetTickCount();
    if ((now - start) > TEST_TIMEOUT) {
      SL_DEBUG_LOG_V2(INFO, "[Offload IPv4] Timeout\r\n");
      break;
    }
    if (sent_bytes < 0) {
      if (errno == ENOBUFS || errno == EAGAIN || errno == EWOULDBLOCK) {
        osDelay(1);
        continue;
      }
      SL_DEBUG_LOG_V2(ERROR, "[Offload IPv4] Send failed: errno=%d\r\n", errno);
      break;
    }
    total_bytes_sent += sent_bytes;
  }

  SL_DEBUG_LOG_V2(INFO, "[Offload IPv4] UDP_TX test finished\r\n");
  SL_DEBUG_LOG_V2(INFO, "[Offload IPv4] Total bytes sent: %lu\r\n", total_bytes_sent);

  sl_si91x_shutdown(client_socket, 0);
}

void receive_data_from_udp_client(void)
{
  has_data_received = 0;
  bytes_read        = 0;
  first_data_frame  = 1;

  int client_socket                 = -1;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);

  client_socket = sl_si91x_socket_async(AF_INET, SOCK_DGRAM, IPPROTO_UDP, &data_callback);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[Offload IPv4] Socket create failed: %d\r\n", errno);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[Offload IPv4] Socket ID: %d\r\n", client_socket);

  server_address.sin_family = AF_INET;
  server_address.sin_port   = LISTENING_PORT;

  if (sl_si91x_bind(client_socket, (struct sockaddr *)&server_address, socket_length) < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[Offload IPv4] Bind failed: %d\r\n", errno);
    sl_si91x_shutdown(client_socket, 0);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[Offload IPv4] Listening on port %d\r\n", LISTENING_PORT);

  start = osKernelGetTickCount();
  while (!has_data_received) {
    if ((osKernelGetTickCount() - start) > TEST_TIMEOUT) {
      SL_DEBUG_LOG_V2(WARN, "[Offload IPv4] UDP_RX timeout (no data received)\r\n");
      break;
    }
    osThreadYield();
  }

  SL_DEBUG_LOG_V2(INFO, "[Offload IPv4] UDP_RX test finished\r\n");
  SL_DEBUG_LOG_V2(INFO, "[Offload IPv4] Total bytes received: %lu\r\n", bytes_read);

  sl_si91x_shutdown(client_socket, 0);
}

/******************************************************
 *     Offload Stack - IPv6 Functions
 ******************************************************/

void send_data_to_tcp_server_v6(void)
{
  int client_socket                  = -1;
  uint32_t total_bytes_sent          = 0;
  int sent_bytes                     = 1;
  struct sockaddr_in6 server_address = { 0 };
  socklen_t socket_length            = sizeof(struct sockaddr_in6);
  uint8_t address_buffer[SL_IPV6_ADDRESS_LENGTH];

  server_address.sin6_family = AF_INET6;
  server_address.sin6_port   = SERVER_PORT_V6;

  if (sl_inet_pton6(SERVER_IP_V6,
                    &SERVER_IP_V6[strlen(SERVER_IP_V6)],
                    address_buffer,
                    (unsigned int *)server_address.sin6_addr.un.u32_addr)
      != 1) {
    SL_DEBUG_LOG_V2(ERROR, "[Offload IPv6] Address conversion failed\r\n");
    return;
  }

  client_socket = sl_si91x_socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[Offload IPv6] Socket create failed: %d\r\n", errno);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[Offload IPv6] Socket ID: %d\r\n", client_socket);

  if (sl_si91x_connect(client_socket, (struct sockaddr *)&server_address, socket_length) < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[Offload IPv6] Connect failed: %d\r\n", errno);
    sl_si91x_shutdown(client_socket, 0);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[Offload IPv6] Connected to TCP server\r\n");
  SL_DEBUG_LOG_V2(INFO, "[Offload IPv6] TCP_TX test start\r\n");

  start = osKernelGetTickCount();
  while (total_bytes_sent < BYTES_TO_SEND) {
    sent_bytes = sl_si91x_send(client_socket, data_buffer, TCP_V6_BUFFER_SIZE, 0);
    now        = osKernelGetTickCount();
    if (sent_bytes < 0) {
      if (errno == ENOBUFS || errno == EAGAIN || errno == EWOULDBLOCK) {
        osDelay(1);
        continue;
      }
      SL_DEBUG_LOG_V2(ERROR, "[Offload IPv6] Send failed: errno=%d\r\n", errno);
      break;
    }
    total_bytes_sent += sent_bytes;
    if ((now - start) > TEST_TIMEOUT) {
      SL_DEBUG_LOG_V2(INFO, "[Offload IPv6] Timeout\r\n");
      break;
    }
  }

  SL_DEBUG_LOG_V2(INFO, "[Offload IPv6] TCP_TX test finished\r\n");
  SL_DEBUG_LOG_V2(INFO, "[Offload IPv6] Total bytes sent: %lu\r\n", total_bytes_sent);

  sl_si91x_shutdown(client_socket, 0);
}

void receive_data_from_tcp_client_v6(void)
{
  has_data_received = 0;
  bytes_read        = 0;
  first_data_frame  = 1;

  int server_socket                  = -1;
  int client_socket                  = -1;
  struct sockaddr_in6 server_address = { 0 };
  socklen_t socket_length            = sizeof(struct sockaddr_in6);

  server_socket = sl_si91x_socket_async(AF_INET6, SOCK_STREAM, IPPROTO_TCP, &data_callback);
  if (server_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[Offload IPv6] Socket create failed: %d\r\n", errno);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[Offload IPv6] Server Socket ID: %d\r\n", server_socket);

  server_address.sin6_family = AF_INET6;
  server_address.sin6_port   = LISTENING_PORT_V6;

  if (sl_si91x_bind(server_socket, (struct sockaddr *)&server_address, socket_length) < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[Offload IPv6] Bind failed: %d\r\n", errno);
    sl_si91x_shutdown(server_socket, 0);
    return;
  }

  if (sl_si91x_listen(server_socket, BACKLOG) < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[Offload IPv6] Listen failed: %d\r\n", errno);
    sl_si91x_shutdown(server_socket, 0);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[Offload IPv6] Listening on port %d\r\n", LISTENING_PORT_V6);

  client_socket = sl_si91x_accept(server_socket, NULL, 0);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[Offload IPv6] Accept failed: %d\r\n", errno);
    sl_si91x_shutdown(server_socket, 0);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[Offload IPv6] Client Socket ID: %d\r\n", client_socket);

  start = osKernelGetTickCount();
  while (!has_data_received) {
    if ((osKernelGetTickCount() - start) > TEST_TIMEOUT) {
      SL_DEBUG_LOG_V2(WARN, "[Offload IPv6] TCP_RX timeout (no data received)\r\n");
      break;
    }
    osThreadYield();
  }

  SL_DEBUG_LOG_V2(INFO, "[Offload IPv6] TCP_RX test finished\r\n");
  SL_DEBUG_LOG_V2(INFO, "[Offload IPv6] Total bytes received: %lu\r\n", bytes_read);

  sl_si91x_shutdown(client_socket, 0);
  sl_si91x_shutdown(server_socket, 0);
}

void send_data_to_udp_server_v6(void)
{
  int client_socket                  = -1;
  uint32_t total_bytes_sent          = 0;
  int sent_bytes                     = 1;
  struct sockaddr_in6 server_address = { 0 };
  socklen_t socket_length            = sizeof(struct sockaddr_in6);
  uint8_t address_buffer[SL_IPV6_ADDRESS_LENGTH];

  server_address.sin6_family = AF_INET6;
  server_address.sin6_port   = SERVER_PORT_V6;

  if (sl_inet_pton6(SERVER_IP_V6,
                    &SERVER_IP_V6[strlen(SERVER_IP_V6)],
                    address_buffer,
                    (unsigned int *)server_address.sin6_addr.un.u32_addr)
      != 1) {
    SL_DEBUG_LOG_V2(ERROR, "[Offload IPv6] Address conversion failed\r\n");
    return;
  }

  client_socket = sl_si91x_socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[Offload IPv6] Socket create failed: %d\r\n", errno);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[Offload IPv6] Socket ID: %d\r\n", client_socket);
  SL_DEBUG_LOG_V2(INFO, "[Offload IPv6] UDP_TX test start\r\n");

  start = osKernelGetTickCount();
  while (total_bytes_sent < BYTES_TO_SEND) {
    sent_bytes =
      sl_si91x_sendto(client_socket, data_buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_address, socket_length);
    now = osKernelGetTickCount();
    if ((now - start) > TEST_TIMEOUT) {
      SL_DEBUG_LOG_V2(INFO, "[Offload IPv6] Timeout\r\n");
      break;
    }
    if (sent_bytes < 0) {
      if (errno == ENOBUFS || errno == EAGAIN || errno == EWOULDBLOCK) {
        osDelay(1);
        continue;
      }
      SL_DEBUG_LOG_V2(ERROR, "[Offload IPv6] Send failed: errno=%d\r\n", errno);
      break;
    }
    total_bytes_sent += sent_bytes;
  }

  SL_DEBUG_LOG_V2(INFO, "[Offload IPv6] UDP_TX test finished\r\n");
  SL_DEBUG_LOG_V2(INFO, "[Offload IPv6] Total bytes sent: %lu\r\n", total_bytes_sent);

  sl_si91x_shutdown(client_socket, 0);
}

void receive_data_from_udp_client_v6(void)
{
  has_data_received = 0;
  bytes_read        = 0;
  first_data_frame  = 1;

  int client_socket                  = -1;
  struct sockaddr_in6 server_address = { 0 };
  socklen_t socket_length            = sizeof(struct sockaddr_in6);

  client_socket = sl_si91x_socket_async(AF_INET6, SOCK_DGRAM, IPPROTO_UDP, &data_callback);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[Offload IPv6] Socket create failed: %d\r\n", errno);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[Offload IPv6] Socket ID: %d\r\n", client_socket);

  server_address.sin6_family = AF_INET6;
  server_address.sin6_port   = LISTENING_PORT_V6;

  if (sl_si91x_bind(client_socket, (struct sockaddr *)&server_address, socket_length) < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[Offload IPv6] Bind failed: %d\r\n", errno);
    sl_si91x_shutdown(client_socket, 0);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[Offload IPv6] Listening on port %d\r\n", LISTENING_PORT_V6);

  start = osKernelGetTickCount();
  while (!has_data_received) {
    if ((osKernelGetTickCount() - start) > TEST_TIMEOUT) {
      SL_DEBUG_LOG_V2(WARN, "[Offload IPv6] UDP_RX timeout (no data received)\r\n");
      break;
    }
    osThreadYield();
  }

  SL_DEBUG_LOG_V2(INFO, "[Offload IPv6] UDP_RX test finished\r\n");
  SL_DEBUG_LOG_V2(INFO, "[Offload IPv6] Total bytes received: %lu\r\n", bytes_read);

  sl_si91x_shutdown(client_socket, 0);
}

/******************************************************
 *     Offload Traffic Router
 ******************************************************/

void send_and_receive_offload_data_traffic(void)
{
  if (IPV6_MODE) {
    if (TCP_MODE) {
      if (TX_MODE) {
        send_data_to_tcp_server_v6();
      } else {
        receive_data_from_tcp_client_v6();
      }
    } else {
      if (TX_MODE) {
        send_data_to_udp_server_v6();
      } else {
        receive_data_from_udp_client_v6();
      }
    }
  } else {
    if (TCP_MODE) {
      if (TX_MODE) {
        send_data_to_tcp_server();
      } else {
        receive_data_from_tcp_client();
      }
    } else {
      if (TX_MODE) {
        send_data_to_udp_server();
      } else {
        receive_data_from_udp_client();
      }
    }
  }
}

/******************************************************
 *     Host Stack (LWIP) - IPv4 Functions
 ******************************************************/

#if HOST_DATA_TRANSFER

void send_lwip_data_to_tcp_server(void)
{
  int client_socket                 = -1;
  uint32_t total_bytes_sent         = 0;
  int sent_bytes                    = 1;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);
  uint32_t start                    = 0;
  uint32_t now                      = 0;

  server_address.sin_family = AF_INET;
  server_address.sin_port   = htons(HOST_SERVER_PORT);
  sl_net_inet_addr(SERVER_IP, &server_address.sin_addr.s_addr);

  client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[LWIP IPv4] Socket create failed: %d\r\n", errno);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv4] Socket ID: %d\r\n", client_socket);

  if (connect(client_socket, (struct sockaddr *)&server_address, socket_length) < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[LWIP IPv4] Connect failed: %d\r\n", errno);
    close(client_socket);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv4] Connected to TCP server\r\n");
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv4] TCP_TX test start\r\n");

  start = osKernelGetTickCount();
  while (total_bytes_sent < BYTES_TO_SEND) {
    sent_bytes = send(client_socket, data_buffer, BUFFER_SIZE, 0);
    now        = osKernelGetTickCount();
    if (sent_bytes < 0) {
      if (errno == ENOBUFS || errno == EAGAIN || errno == EWOULDBLOCK || errno == ENOMEM) {
        osDelay(1);
        continue;
      }
      SL_DEBUG_LOG_V2(ERROR, "[LWIP IPv4] Send failed: errno=%d\r\n", errno);
      break;
    }
    total_bytes_sent += sent_bytes;
    if ((now - start) > TEST_TIMEOUT) {
      SL_DEBUG_LOG_V2(INFO, "[LWIP IPv4] Timeout\r\n");
      break;
    }
  }

  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv4] TCP_TX test finished\r\n");
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv4] Total bytes sent: %lu\r\n", total_bytes_sent);

  close(client_socket);
}

void receive_lwip_data_from_tcp_client(void)
{
  int server_socket                 = -1;
  int client_socket                 = -1;
  int read_bytes                    = 0;
  uint32_t total_bytes_received     = 0;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);
  uint32_t start                    = 0;
  uint32_t now                      = 0;

  server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (server_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[LWIP IPv4] Socket create failed: %d\r\n", errno);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv4] Server Socket ID: %d\r\n", server_socket);

  server_address.sin_family      = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port        = htons(HOST_LISTENING_PORT);

  if (bind(server_socket, (struct sockaddr *)&server_address, socket_length) < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[LWIP IPv4] Bind failed: %d\r\n", errno);
    close(server_socket);
    return;
  }

  if (listen(server_socket, BACKLOG) < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[LWIP IPv4] Listen failed: %d\r\n", errno);
    close(server_socket);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv4] Listening on port %d\r\n", HOST_LISTENING_PORT);

  client_socket = accept(server_socket, NULL, NULL);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[LWIP IPv4] Accept failed: %d\r\n", errno);
    close(server_socket);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv4] Client Socket ID: %d\r\n", client_socket);
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv4] TCP_RX test start\r\n");

  start = osKernelGetTickCount();
  do {
    read_bytes = recv(client_socket, data_buffer, BUFFER_SIZE, 0);
    if (read_bytes < 0) {
      SL_DEBUG_LOG_V2(ERROR, "[LWIP IPv4] Receive failed: %d\r\n", errno);
      break;
    }
    total_bytes_received += read_bytes;
    now = osKernelGetTickCount();
  } while ((read_bytes > 0) && ((now - start) < TEST_TIMEOUT));

  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv4] TCP_RX test finished\r\n");
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv4] Total bytes received: %lu\r\n", total_bytes_received);

  close(client_socket);
  close(server_socket);
}

void send_lwip_data_to_udp_server(void)
{
  int client_socket                 = -1;
  uint32_t total_bytes_sent         = 0;
  int sent_bytes                    = 1;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);
  uint32_t start                    = 0;
  uint32_t now                      = 0;

  server_address.sin_family = AF_INET;
  server_address.sin_port   = htons(HOST_SERVER_PORT);
  sl_net_inet_addr(SERVER_IP, &server_address.sin_addr.s_addr);

  client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[LWIP IPv4] Socket create failed: %d\r\n", errno);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv4] Socket ID: %d\r\n", client_socket);
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv4] UDP_TX test start\r\n");

  start = osKernelGetTickCount();
  while (total_bytes_sent < BYTES_TO_SEND) {
    sent_bytes = sendto(client_socket, data_buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_address, socket_length);
    now        = osKernelGetTickCount();
    if (sent_bytes < 0) {
      if (errno == ENOBUFS || errno == EAGAIN || errno == EWOULDBLOCK || errno == ENOMEM) {
        osDelay(1);
        continue;
      }
      SL_DEBUG_LOG_V2(ERROR, "[LWIP IPv4] Send failed: errno=%d\r\n", errno);
      break;
    }
    total_bytes_sent += sent_bytes;
    if ((now - start) > TEST_TIMEOUT) {
      SL_DEBUG_LOG_V2(INFO, "[LWIP IPv4] Timeout\r\n");
      break;
    }
  }

  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv4] UDP_TX test finished\r\n");
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv4] Total bytes sent: %lu\r\n", total_bytes_sent);

  close(client_socket);
}

void receive_lwip_data_from_udp_client(void)
{
  int client_socket                 = -1;
  int read_bytes                    = 0;
  uint32_t total_bytes_received     = 0;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);
  uint32_t start                    = 0;
  uint32_t now                      = 0;

  client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[LWIP IPv4] Socket create failed: %d\r\n", errno);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv4] Socket ID: %d\r\n", client_socket);

  server_address.sin_family      = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port        = htons(HOST_LISTENING_PORT);

  if (bind(client_socket, (struct sockaddr *)&server_address, socket_length) < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[LWIP IPv4] Bind failed: %d\r\n", errno);
    close(client_socket);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv4] Listening on port %d\r\n", HOST_LISTENING_PORT);
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv4] UDP_RX test start\r\n");

  start = osKernelGetTickCount();
  do {
    read_bytes = recvfrom(client_socket, data_buffer, BUFFER_SIZE, 0, NULL, NULL);
    if (read_bytes < 0) {
      SL_DEBUG_LOG_V2(ERROR, "[LWIP IPv4] Receive failed: %d\r\n", errno);
      break;
    }
    total_bytes_received += read_bytes;
    now = osKernelGetTickCount();
  } while ((total_bytes_received < BYTES_TO_RECEIVE) && ((now - start) < TEST_TIMEOUT));

  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv4] UDP_RX test finished\r\n");
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv4] Total bytes received: %lu\r\n", total_bytes_received);

  close(client_socket);
}

/******************************************************
 *     Host Stack (LWIP) - IPv6 Functions
 ******************************************************/

#if LWIP_IPV6
void send_lwip_data_to_tcp_server_v6(void)
{
  int client_socket                  = -1;
  uint32_t total_bytes_sent          = 0;
  int sent_bytes                     = 1;
  struct sockaddr_in6 server_address = { 0 };
  socklen_t socket_length            = sizeof(struct sockaddr_in6);
  uint32_t start                     = 0;
  uint32_t now                       = 0;

  server_address.sin6_family = AF_INET6;
  server_address.sin6_port   = htons(HOST_SERVER_PORT_V6);

  if (inet_pton(AF_INET6, SERVER_IP_V6, &server_address.sin6_addr) <= 0) {
    SL_DEBUG_LOG_V2(ERROR, "[LWIP IPv6] Invalid server address\r\n");
    return;
  }

  client_socket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[LWIP IPv6] Socket create failed: %d\r\n", errno);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv6] Socket ID: %d\r\n", client_socket);

  if (connect(client_socket, (struct sockaddr *)&server_address, socket_length) < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[LWIP IPv6] Connect failed: %d\r\n", errno);
    close(client_socket);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv6] Connected to TCP server\r\n");
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv6] TCP_TX test start\r\n");

  start = osKernelGetTickCount();
  while (total_bytes_sent < BYTES_TO_SEND) {
    sent_bytes = send(client_socket, data_buffer, BUFFER_SIZE, 0);
    now        = osKernelGetTickCount();
    if (sent_bytes < 0) {
      if (errno == ENOBUFS || errno == EAGAIN || errno == EWOULDBLOCK || errno == ENOMEM) {
        osDelay(1);
        continue;
      }
      SL_DEBUG_LOG_V2(ERROR, "[LWIP IPv6] Send failed: errno=%d\r\n", errno);
      break;
    }
    total_bytes_sent += sent_bytes;
    if ((now - start) > TEST_TIMEOUT) {
      SL_DEBUG_LOG_V2(INFO, "[LWIP IPv6] Timeout\r\n");
      break;
    }
  }

  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv6] TCP_TX test finished\r\n");
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv6] Total bytes sent: %lu\r\n", total_bytes_sent);

  close(client_socket);
}

void receive_lwip_data_from_tcp_client_v6(void)
{
  int server_socket                  = -1;
  int client_socket                  = -1;
  int read_bytes                     = 0;
  uint32_t total_bytes_received      = 0;
  struct sockaddr_in6 server_address = { 0 };
  socklen_t socket_length            = sizeof(struct sockaddr_in6);
  uint32_t start                     = 0;
  uint32_t now                       = 0;

  server_socket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
  if (server_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[LWIP IPv6] Socket create failed: %d\r\n", errno);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv6] Server Socket ID: %d\r\n", server_socket);

  server_address.sin6_family = AF_INET6;
  server_address.sin6_port   = htons(HOST_LISTENING_PORT_V6);

  if (bind(server_socket, (struct sockaddr *)&server_address, socket_length) < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[LWIP IPv6] Bind failed: %d\r\n", errno);
    close(server_socket);
    return;
  }

  if (listen(server_socket, BACKLOG) < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[LWIP IPv6] Listen failed: %d\r\n", errno);
    close(server_socket);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv6] Listening on port %d\r\n", HOST_LISTENING_PORT_V6);

  client_socket = accept(server_socket, NULL, NULL);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[LWIP IPv6] Accept failed: %d\r\n", errno);
    close(server_socket);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv6] Client Socket ID: %d\r\n", client_socket);
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv6] TCP_RX test start\r\n");

  start = osKernelGetTickCount();
  do {
    read_bytes = recv(client_socket, data_buffer, BUFFER_SIZE, 0);
    if (read_bytes < 0) {
      SL_DEBUG_LOG_V2(ERROR, "[LWIP IPv6] Receive failed: %d\r\n", errno);
      break;
    }
    total_bytes_received += read_bytes;
    now = osKernelGetTickCount();
  } while ((read_bytes > 0) && ((now - start) < TEST_TIMEOUT));

  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv6] TCP_RX test finished\r\n");
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv6] Total bytes received: %lu\r\n", total_bytes_received);

  close(client_socket);
  close(server_socket);
}

void send_lwip_data_to_udp_server_v6(void)
{
  int client_socket                  = -1;
  uint32_t total_bytes_sent          = 0;
  int sent_bytes                     = 1;
  struct sockaddr_in6 server_address = { 0 };
  socklen_t socket_length            = sizeof(struct sockaddr_in6);
  uint32_t start                     = 0;
  uint32_t now                       = 0;

  server_address.sin6_family = AF_INET6;
  server_address.sin6_port   = htons(HOST_SERVER_PORT_V6);

  if (inet_pton(AF_INET6, SERVER_IP_V6, &server_address.sin6_addr) <= 0) {
    SL_DEBUG_LOG_V2(ERROR, "[LWIP IPv6] Invalid server address\r\n");
    return;
  }

  client_socket = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[LWIP IPv6] Socket create failed: %d\r\n", errno);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv6] Socket ID: %d\r\n", client_socket);
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv6] UDP_TX test start\r\n");

  start = osKernelGetTickCount();
  while (total_bytes_sent < BYTES_TO_SEND) {
    sent_bytes = sendto(client_socket, data_buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_address, socket_length);
    now        = osKernelGetTickCount();
    if (sent_bytes < 0) {
      if (errno == ENOBUFS || errno == EAGAIN || errno == EWOULDBLOCK || errno == ENOMEM) {
        osDelay(1);
        continue;
      }
      SL_DEBUG_LOG_V2(ERROR, "[LWIP IPv6] Send failed: errno=%d\r\n", errno);
      break;
    }
    total_bytes_sent += sent_bytes;
    if ((now - start) > TEST_TIMEOUT) {
      SL_DEBUG_LOG_V2(INFO, "[LWIP IPv6] Timeout\r\n");
      break;
    }
  }

  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv6] UDP_TX test finished\r\n");
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv6] Total bytes sent: %lu\r\n", total_bytes_sent);

  close(client_socket);
}

void receive_lwip_data_from_udp_client_v6(void)
{
  int client_socket                  = -1;
  int read_bytes                     = 0;
  uint32_t total_bytes_received      = 0;
  struct sockaddr_in6 server_address = { 0 };
  socklen_t socket_length            = sizeof(struct sockaddr_in6);
  uint32_t start                     = 0;
  uint32_t now                       = 0;

  client_socket = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[LWIP IPv6] Socket create failed: %d\r\n", errno);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv6] Socket ID: %d\r\n", client_socket);

  server_address.sin6_family = AF_INET6;
  server_address.sin6_port   = htons(HOST_LISTENING_PORT_V6);

  if (bind(client_socket, (struct sockaddr *)&server_address, socket_length) < 0) {
    SL_DEBUG_LOG_V2(ERROR, "[LWIP IPv6] Bind failed: %d\r\n", errno);
    close(client_socket);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv6] Listening on port %d\r\n", HOST_LISTENING_PORT_V6);
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv6] UDP_RX test start\r\n");

  start = osKernelGetTickCount();
  do {
    read_bytes = recvfrom(client_socket, data_buffer, BUFFER_SIZE, 0, NULL, NULL);
    if (read_bytes < 0) {
      SL_DEBUG_LOG_V2(ERROR, "[LWIP IPv6] Receive failed: %d\r\n", errno);
      break;
    }
    total_bytes_received += read_bytes;
    now = osKernelGetTickCount();
  } while ((total_bytes_received < BYTES_TO_RECEIVE) && ((now - start) < TEST_TIMEOUT));

  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv6] UDP_RX test finished\r\n");
  SL_DEBUG_LOG_V2(INFO, "[LWIP IPv6] Total bytes received: %lu\r\n", total_bytes_received);

  close(client_socket);
}
#endif

/******************************************************
 *     LWIP Traffic Router Thread
 ******************************************************/

void lwip_data_traffic_thread(void *argument)
{
  UNUSED_PARAMETER(argument);

  if (IPV6_MODE) {
#if LWIP_IPV6
    if (TCP_MODE) {
      if (TX_MODE) {
        send_lwip_data_to_tcp_server_v6();
      } else {
        receive_lwip_data_from_tcp_client_v6();
      }
    } else {
      if (TX_MODE) {
        send_lwip_data_to_udp_server_v6();
      } else {
        receive_lwip_data_from_udp_client_v6();
      }
    }
#else
    SL_DEBUG_LOG_V2(WARN, "[LWIP] IPv6 not enabled - skipping\r\n");
#endif
  } else {
    if (TCP_MODE) {
      if (TX_MODE) {
        send_lwip_data_to_tcp_server();
      } else {
        receive_lwip_data_from_tcp_client();
      }
    } else {
      if (TX_MODE) {
        send_lwip_data_to_udp_server();
      } else {
        receive_lwip_data_from_udp_client();
      }
    }
  }

  osThreadTerminate(osThreadGetId());
}
#endif

/******************************************************
 *               Application Start
 ******************************************************/

static void application_start(void *argument)
{
  UNUSED_PARAMETER(argument);
  sl_status_t status;

  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &client_configuration, &wifi_client_context, NULL);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to init Wi-Fi client: 0x%lX\r\n", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Wi-Fi client interface initialized\r\n");

  status = sl_net_set_credential(SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID,
                                 SL_NET_WIFI_PSK,
                                 DEFAULT_WIFI_CLIENT_CREDENTIAL,
                                 strlen(DEFAULT_WIFI_CLIENT_CREDENTIAL));
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to set credentials: 0x%lX\r\n", status);
    return;
  }

  status =
    sl_net_set_profile(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID, &wifi_client_profile);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to set profile: 0x%lX\r\n", status);
    return;
  }

  SL_DEBUG_LOG_V2(INFO, "Connecting to Wi-Fi...\r\n");
  status = sl_net_up(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to bring up Wi-Fi: 0x%lX\r\n", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Wi-Fi client connected\r\n");

  status =
    sl_net_get_profile(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID, &wifi_client_profile);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to get profile: 0x%lX\r\n", status);
    return;
  }

  print_ip_addresses(&wifi_client_profile);

  status = sl_si91x_config_socket(socket_config);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Socket config failed: 0x%lX\r\n", status);
    SL_DEBUG_LOG_V2(WARN, "Offload test skipped.\r\n");
  }

  memset(data_buffer, 'A', sizeof(data_buffer));

  SL_DEBUG_LOG_V2(INFO, "========== Configuration ==========\r\n");
  SL_DEBUG_LOG_V2(INFO, "  IPV6_MODE:          %d (%s)\r\n", IPV6_MODE, (uintptr_t)(IPV6_MODE ? "IPv6" : "IPv4"));
  SL_DEBUG_LOG_V2(INFO, "  TCP_MODE:           %d (%s)\r\n", TCP_MODE, (uintptr_t)(TCP_MODE ? "TCP" : "UDP"));
  SL_DEBUG_LOG_V2(INFO, "  TX_MODE:            %d (%s)\r\n", TX_MODE, (uintptr_t)(TX_MODE ? "TX" : "RX"));
  SL_DEBUG_LOG_V2(INFO,
                  "  HOST_DATA_TRANSFER: %d (%s)",
                  HOST_DATA_TRANSFER,
                  (uintptr_t)(HOST_DATA_TRANSFER ? "Offload+LWIP" : "Offload"));
  SL_DEBUG_LOG_V2(INFO, "===================================");

#if HOST_DATA_TRANSFER
  osThreadId_t lwip_thread_id = NULL;

  lwip_thread_id = osThreadNew((osThreadFunc_t)lwip_data_traffic_thread, NULL, &traffic_thread_attributes);
  if (lwip_thread_id == NULL) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to create LWIP thread");
    return;
  }
#endif

  if (status == SL_STATUS_OK) {
    send_and_receive_offload_data_traffic();
  }

  SL_DEBUG_LOG_V2(INFO, "========== Test Completed ==========\r\n");

  while (1) {
    osDelay(1000);
  }
}

void app_init(void)
{
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}
