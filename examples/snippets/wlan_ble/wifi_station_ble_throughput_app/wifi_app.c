/*******************************************************************************
* @file  wifi_app.c
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
/*************************************************************************
 *
 */

/*================================================================================
 * @brief : This file contains example application for WLAN Station BLE Throughput
 * @section Description :
 * This application demonstrates how to configure SiLabs module in UDP client/
 * server or TCP client/server to measure throughput.
 =================================================================================*/

/*=======================================================================*/
//  ! INCLUDES
/*=======================================================================*/

//! SL Wi-Fi SDK includes
#include "sl_constants.h"
#include "sl_wifi.h"
#include "sl_wifi_callback_framework.h"
#include "sl_net.h"
#include "sl_net_si91x.h"
#include "sl_utility.h"
#include "sl_si91x_driver.h"
#include "sl_si91x_socket_support.h"

#include "errno.h"
#include "socket.h"
#include "sl_si91x_socket_support.h"

#include "cmsis_os2.h"
#include <rsi_common_apis.h>
#include <string.h>
#include "stdlib.h"
#include "wifi_config.h"

#include "sl_si91x_socket_utility.h"
#include "sl_si91x_socket_constants.h"
#include "sl_si91x_socket.h"
#include "sl_si91x_socket_types.h"
#include "sl_si91x_core_utilities.h"
#ifdef SLI_SI91X_MCU_INTERFACE
#include "rsi_rom_clks.h"
#endif

/*=======================================================================*/
//   ! MACROS
/*=======================================================================*/
#define DHCP_HOST_NAME    NULL
#define TIMEOUT_MS        25000
#define WIFI_SCAN_TIMEOUT 10000

#define BYTES_TO_RECEIVE           (1 << 28) //256MB
#define TEST_TIMEOUT               (30000)   //30sec
#define BACK_LOG                   1
#define SL_HIGH_PERFORMANCE_SOCKET BIT(7)

#if ENABLE_TLS
#include "cacert.pem.h" //! Include TLS CA certificate
// Load certificate to device flash :
// Certificate could be loaded once and need not be loaded for every boot up
#endif

/*=======================================================================*/
//   ! GLOBAL VARIABLES
/*=======================================================================*/
//! Throughput parameters
uint8_t data_buffer[BUFFER_SIZE];
volatile uint8_t wlan_data_rcv;
volatile uint8_t wlan_completed;

/*=======================================================================*/
//   ! EXTERN VARIABLES
/*=======================================================================*/
extern osSemaphoreId_t ble_conn_sem;

/*=======================================================================*/
//   ! PROCEDURES
/*=======================================================================*/

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

uint8_t has_data_received = 0;
uint32_t bytes_read       = 0;
uint32_t start            = 0;
uint32_t now              = 0;
uint8_t first_call        = 1;
uint8_t enable_callback   = 0;

#if SOCKET_ASYNC_FEATURE
static uint8_t tcp_async_socket_config_done = 0;
#endif

#if SL_BLE_DYNAMIC_DISABLE_THROUGHPUT_DEMO && SOCKET_ASYNC_FEATURE
static void reset_wlan_async_rx_state_between_passes(void)
{
  has_data_received = 0;
  bytes_read        = 0;
  first_call        = 1;
  enable_callback   = 0;
  start             = 0;
  now               = 0;
}
#endif
void data_callback(uint32_t sock_no,
                   uint8_t *buffer,
                   uint32_t length,
                   const sl_si91x_socket_metadata_t *firmware_socket_response)
{
  UNUSED_PARAMETER(sock_no);
  UNUSED_PARAMETER(buffer);
  UNUSED_PARAMETER(firmware_socket_response);
  if (enable_callback) {
    if (first_call) {
      first_call = 0;
      start      = osKernelGetTickCount();
    }

    bytes_read += length;
    now = osKernelGetTickCount();
    if ((bytes_read > BYTES_TO_RECEIVE) || ((now - start) > TEST_TIMEOUT)) {
      first_call      = 1;
      enable_callback = 0;
      // bytes_read = 0;
      has_data_received = 1;
    }
  }
}

void wlan_throughput_task(void);
static void measure_and_print_throughput(uint32_t total_num_of_bytes, uint32_t test_timeout)
{
  float duration = ((test_timeout) / 1000);                    // ms to sec
  float result   = ((float)total_num_of_bytes * 8) / duration; // bytes to bps
  result         = (result / 1000000);                         // bps to Mbps
  SL_DEBUG_LOG_V2(INFO, "Throughput achieved @ %0.02f Mbps in %0.03f sec successfully\r\n", result, duration);
  bytes_read = 0;
}

#if ENABLE_TLS && LOAD_CERTIFICATE
sl_status_t clear_and_load_certificates_in_flash(void)
{
  sl_status_t status = SL_STATUS_OK;

  // Load TLS CA certificate
  status =
    sl_net_set_credential(SL_NET_TLS_SERVER_CREDENTIAL_ID(0), SL_NET_SIGNING_CERTIFICATE, cacert, sizeof(cacert) - 1);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Loading TLS CA certificate in to FLASH Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  SL_DEBUG_LOG_V2(INFO, "Load TLS CA certificate at index %d Success\r\n", 0);

  return status;
}
#endif

sl_status_t join_callback_handler(sl_wifi_event_t event,
                                  sl_status_t status_code,
                                  char *result,
                                  uint32_t result_length,
                                  void *arg)
{
  UNUSED_PARAMETER(result);
  UNUSED_PARAMETER(arg);

  if (SL_WIFI_CHECK_IF_EVENT_FAILED(event)) {
    SL_DEBUG_LOG_V2(ERROR, "F: Join Event received with %lu bytes payload\r\n", result_length);
    return status_code;
  }

  return SL_STATUS_OK;
}

void wifi_app_callbacks_init(void)
{
  sl_wifi_set_join_callback_v2(join_callback_handler, NULL);
}

#if SL_BLE_DYNAMIC_DISABLE_THROUGHPUT_DEMO
/**
 * @brief Reconnect Wi-Fi and configure IP after BLE re-enable (dynamic demo path).
 *        Wi-Fi stack stays initialized; caller disconnects station before BLE re-enable, so this only
 *        sets credential, connects, and runs DHCP — no sl_wifi_init(). Uses SSID, PSK, SECURITY_TYPE from wifi_config.h.
 * @return 0 on success, -1 on failure.
 */
static int32_t wifi_app_init_and_reconnect(void)
{
  sl_status_t status;
  sl_net_ip_configuration_t ip_address        = { 0 };
  sl_wifi_client_configuration_t access_point = { 0 };
  sl_wifi_credential_id_t id                  = SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID;

  wifi_app_callbacks_init();

  status = sl_net_set_credential(id, SL_NET_WIFI_PSK, PSK, strlen((char *)PSK));
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Init+reconnect: set credential failed: 0x%lX\r\n", status);
    return -1;
  }

  access_point.ssid.length = strlen((char *)SSID);
  memcpy(access_point.ssid.value, SSID, access_point.ssid.length);
  access_point.security      = SECURITY_TYPE;
  access_point.encryption    = SL_WIFI_DEFAULT_ENCRYPTION;
  access_point.credential_id = id;

  status = sl_wifi_connect(SL_WIFI_CLIENT_2_4GHZ_INTERFACE, &access_point, TIMEOUT_MS);
  if (status != RSI_SUCCESS) {
    SL_DEBUG_LOG_V2(ERROR, "Init+reconnect: sl_wifi_connect failed: 0x%lX\r\n", status);
    return -1;
  }

  ip_address.type      = SL_IPV4;
  ip_address.mode      = SL_IP_MANAGEMENT_DHCP;
  ip_address.host_name = DHCP_HOST_NAME;
  status               = sl_si91x_configure_ip_address(&ip_address, SL_SI91X_WIFI_CLIENT_VAP_ID);
  if (status != RSI_SUCCESS) {
    SL_DEBUG_LOG_V2(ERROR, "Init+reconnect: IP config failed: 0x%lX\r\n", status);
    return -1;
  }

  return 0;
}
#endif

/*====================================================*/
/**
 * @fn         int32_t application(void)
 * @brief      This function explains how to scan, connect and tx/rx wlan packets from remote device
 * @param[in]  void
 * @return     int32_t
 * @section description
 *====================================================*/
int32_t rsi_wlan_app_task()
{
  int32_t status                              = RSI_SUCCESS;
  sl_net_ip_configuration_t ip_address        = { 0 };
  sl_wifi_client_configuration_t access_point = { 0 };

  wifi_app_callbacks_init();

#if TX_RX_RATIO_ENABLE // FixMe
  status = rsi_wlan_buffer_config();
  if (status != RSI_SUCCESS) {
    return status;
  }
#endif

  osSemaphoreAcquire(ble_conn_sem, osWaitForever);

  // Connect to an Access point
  sl_wifi_credential_t cred  = { 0 };
  sl_wifi_credential_id_t id = SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID;

  cred.type = SL_WIFI_PSK_CREDENTIAL;
  memcpy(cred.psk.value, PSK, strlen((char *)PSK));

  status = sl_net_set_credential(id, SL_NET_WIFI_PSK, PSK, strlen((char *)PSK));
  if (SL_STATUS_OK == status) {
    SL_DEBUG_LOG_V2(INFO, "Credentials set, id : %lu\r\n", id);

    access_point.ssid.length = strlen((char *)SSID);
    memcpy(access_point.ssid.value, SSID, access_point.ssid.length);
    access_point.security      = SECURITY_TYPE;
    access_point.encryption    = SL_WIFI_DEFAULT_ENCRYPTION;
    access_point.credential_id = id;

    SL_DEBUG_LOG_V2(INFO, "SSID %s\r\n", (uintptr_t)access_point.ssid.value);
    status = sl_wifi_connect(SL_WIFI_CLIENT_2_4GHZ_INTERFACE, &access_point, TIMEOUT_MS);
  }
  if (status != RSI_SUCCESS) {
    SL_DEBUG_LOG_V2(ERROR, "WLAN connection failed %lx\r\n", status);
    return status;
  } else {
    SL_DEBUG_LOG_V2(INFO, "WLAN connected\r\n");
  }

  //! Configure IP
  ip_address.type      = SL_IPV4;
  ip_address.mode      = SL_IP_MANAGEMENT_DHCP;
  ip_address.host_name = DHCP_HOST_NAME;

  status = sl_si91x_configure_ip_address(&ip_address, SL_SI91X_WIFI_CLIENT_VAP_ID);
  if (status != RSI_SUCCESS) {
    SL_DEBUG_LOG_V2(ERROR, "IP Config failed %lx\r\n", status);
    return status;
  }
  sl_ip_address_t ip = { 0 };
  ip.type            = ip_address.type;
  ip.ip.v4.value     = ip_address.ip.v4.ip_address.value;
  print_sl_ip_address(&ip);

  wlan_throughput_task();

#if SL_BLE_DYNAMIC_DISABLE_THROUGHPUT_DEMO
  /* If dynamic disable demo enabled: disable BLE -> throughput again -> disconnect station -> enable BLE -> reconnect + DHCP */
  {
    int32_t ble_result = RSI_FAILURE;
    if (rsi_ble_app_request_disable() != RSI_SUCCESS) {
      SL_DEBUG_LOG_V2(ERROR, "BLE disable request failed\r\n");
      return -1;
    }
    if (osMessageQueueGet(ble_disable_done_queue, &ble_result, NULL, osWaitForever) != osOK
        || ble_result != RSI_SUCCESS) {
      SL_DEBUG_LOG_V2(ERROR, "BLE disable failed: %ld\r\n", (long)ble_result);
      return -1;
    }

    SL_DEBUG_LOG_V2(INFO, "BLE disabled\r\n");
#if SL_BLE_DYNAMIC_DISABLE_THROUGHPUT_DEMO && SOCKET_ASYNC_FEATURE
    reset_wlan_async_rx_state_between_passes();
#endif

    wlan_throughput_task();
    status = sl_wifi_disconnect(SL_WIFI_CLIENT_2_4GHZ_INTERFACE);
    if (status != SL_STATUS_OK) {
      SL_DEBUG_LOG_V2(ERROR, "Dynamic demo: sl_wifi_disconnect failed: 0x%lX\r\n", (unsigned long)status);
      return -1;
    }
    rsi_ble_app_request_enable();
    if (osMessageQueueGet(ble_enable_done_queue, &ble_result, NULL, osWaitForever) != osOK
        || ble_result != RSI_SUCCESS) {
      SL_DEBUG_LOG_V2(ERROR, "BLE enable failed: %ld\r\n", (long)ble_result);
      return -1;
    }

    SL_DEBUG_LOG_V2(INFO, "BLE re-enabled\r\n");

    if (wifi_app_init_and_reconnect() != 0) {
      SL_DEBUG_LOG_V2(ERROR, "Init+reconnect after BLE enable failed\r\n");
      return -1;
    }
  }
#endif

  SL_DEBUG_LOG_V2(INFO, "WLAN throughput test finished\r\n");
  return 0;
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
  uint32_t packets                  = 0;

  server_address.sin_family = AF_INET;
  server_address.sin_port   = SERVER_PORT;
  sl_net_inet_addr(SERVER_IP_ADDRESS, &server_address.sin_addr.s_addr);

  client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket creation failed with BSD error: %d\r\n", errno);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Socket ID : %d\r\n", client_socket);

  SL_DEBUG_LOG_V2(INFO, "UDP_TX Throughput test start\r\n");
  start = osKernelGetTickCount();
  while (1) {
    sent_bytes =
      sendto(client_socket, data_buffer, UDP_BUFFER_SIZE, 0, (struct sockaddr *)&server_address, socket_length);
    if (sent_bytes < 0) {
      if (errno == ENOBUFS)
        continue;
      SL_DEBUG_LOG_V2(ERROR, "sendto failed with bsd error:%d\r\n", errno);
      close(client_socket);
      return;
    } else if (sent_bytes > 0) {
      total_bytes_sent = total_bytes_sent + sent_bytes;
      packets++;
    }

    now = osKernelGetTickCount();
    if ((now - start) >= TEST_TIMEOUT) {
      SL_DEBUG_LOG_V2(INFO, "Number of packets sent : %ld\r\n", packets);
      SL_DEBUG_LOG_V2(INFO, "Total bytes sent : %ld\r\n", total_bytes_sent);
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

  SL_DEBUG_LOG_V2(INFO, "UDP_TX Throughput test finished\r\n");
  close(client_socket);
}
#if SOCKET_ASYNC_FEATURE
void receive_data_from_udp_client(void)
{
  int client_socket = -1;

  int socket_return_value           = 0;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);
  client_socket                     = sl_si91x_socket_async(AF_INET, SOCK_DGRAM, IPPROTO_UDP, &data_callback);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket creation failed with bsd error: %d\r\n", errno);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Socket ID : %d\r\n", client_socket);

  server_address.sin_family = AF_INET;
  server_address.sin_port   = DEVICE_PORT;

  socket_return_value = sl_si91x_bind(client_socket, (struct sockaddr *)&server_address, socket_length);

  if (socket_return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket bind failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Listening on Local Port %d\r\n", DEVICE_PORT);

  SL_DEBUG_LOG_V2(INFO, "UDP_RX Async Throughput test start\r\n");

#if CONTINUOUS_THROUGHPUT
  // Continuous throughput mode - loop forever with interval reporting
  while (1) {
    // Reset for each interval; re-arm callback each interval (data_callback is gated on enable_callback)
    has_data_received = 0;
    bytes_read        = 0;
    first_call        = 1;
    enable_callback   = 1;
    start             = osKernelGetTickCount();

    // Wait for TEST_TIMEOUT interval or max bytes
    while (!has_data_received) {
      osThreadYield();
    }

    now = osKernelGetTickCount();
    SL_DEBUG_LOG_V2(INFO, "Total bytes received : %ld\r\n", bytes_read);
    measure_and_print_throughput(bytes_read, (now - start));
  }
#else
  // Single measurement mode

  enable_callback = 1;
  while (!has_data_received) {
    osThreadYield();
  }

  SL_DEBUG_LOG_V2(INFO, "UDP_RX Async Throughput test finished\r\n");
  SL_DEBUG_LOG_V2(INFO, "Total bytes received : %ld\r\n", bytes_read);

  measure_and_print_throughput(bytes_read, (now - start));
  has_data_received = 0;

  close(client_socket);
#endif
}
#else
void receive_data_from_udp_client(void)
{
  sl_status_t status = SL_STATUS_OK;
  int client_socket = -1;
  uint32_t total_bytes_received = 0;
  int socket_return_value = 0;
  int read_bytes = 1;
  uint32_t start = 0;
  uint32_t now = 0;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length = sizeof(struct sockaddr_in);

  client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket creation failed with bsd error: %d\r\n", errno);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Socket ID : %d\r\n", client_socket);

  server_address.sin_family = AF_INET;
  server_address.sin_port = DEVICE_PORT;

  socket_return_value = bind(client_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket bind failed with BSD error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Listening on Local Port %d\r\n", DEVICE_PORT);

  SL_DEBUG_LOG_V2(INFO, "UDP_RX Throughput test start\r\n");
  start = osKernelGetTickCount();
  while (1) {
    read_bytes = recvfrom(client_socket, data_buffer, sizeof(data_buffer), 0, NULL, NULL);
    if (read_bytes < 0) {
      if (errno == 0) {
        // get the error code returned by the firmware
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

    now = osKernelGetTickCount();
    if ((now - start) > TEST_TIMEOUT) {
      SL_DEBUG_LOG_V2(INFO, "Total bytes received : %ld\r\n", total_bytes_received);
      measure_and_print_throughput(total_bytes_received, (now - start));
#if CONTINUOUS_THROUGHPUT
      total_bytes_received = 0;
      start = osKernelGetTickCount();
#else
      break;
#endif
    }
  }

  SL_DEBUG_LOG_V2(INFO, "UDP_RX Throughput test finished\r\n");
  close(client_socket);
  return;
}
#endif
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
    SL_DEBUG_LOG_V2(ERROR, "Socket Create failed with BSD error: %d\r\n", errno);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Socket ID : %d\r\n", client_socket);

  socket_return_value = connect(client_socket, (struct sockaddr *)&server_address, sizeof(struct sockaddr_in));
  if (socket_return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket Connect failed with BSD error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Socket connected to TCP server\r\n");

  SL_DEBUG_LOG_V2(INFO, "TCP_TX Throughput test start\r\n");
  start = osKernelGetTickCount();
  while (1) {
    sent_bytes = send(client_socket, data_buffer, TCP_BUFFER_SIZE, 0);
    if (sent_bytes < 0) {
      if (errno == ENOBUFS)
        continue;
      SL_DEBUG_LOG_V2(ERROR, "TCP send failed\r\n");
      break;
    } else if (sent_bytes > 0) {
      total_bytes_sent = total_bytes_sent + sent_bytes;
      packets++;
    }
    now = osKernelGetTickCount();
    if ((now - start) >= TEST_TIMEOUT) {
      SL_DEBUG_LOG_V2(INFO, "Number of packets sent : %ld\r\n", packets);
      SL_DEBUG_LOG_V2(INFO, "Total bytes sent : %ld\r\n", total_bytes_sent);
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

  SL_DEBUG_LOG_V2(INFO, "TCP_TX Throughput test finished\r\n");
  close(client_socket);

  return;
}
#if SOCKET_ASYNC_FEATURE
void receive_data_from_tcp_client(void)
{
  int server_socket                 = -1;
  int client_socket                 = -1;
  int socket_return_value           = 0;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);
  uint32_t high_performance_socket  = SL_HIGH_PERFORMANCE_SOCKET;

  if (!tcp_async_socket_config_done) {
    sl_status_t status = sl_si91x_config_socket(socket_config);
    if (status != SL_STATUS_OK) {
      SL_DEBUG_LOG_V2(ERROR, "Socket config failed: %ld\r\n", status);
      return;
    }
    tcp_async_socket_config_done = 1;
  }
  SL_DEBUG_LOG_V2(INFO, "Socket config Done\r\n");

  server_socket = sl_si91x_socket_async(AF_INET, SOCK_STREAM, IPPROTO_TCP, &data_callback);
  if (server_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket creation failed with bsd error: %d\r\n", errno);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Server Socket ID : %d\r\n", server_socket);

  /* HP socket mode is per-socket; apply on every new async TCP socket */
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
  server_address.sin_family = AF_INET;
  server_address.sin_port   = DEVICE_PORT;

  socket_return_value = sl_si91x_bind(server_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket bind failed with bsd error: %d\r\n", errno);
    close(server_socket);
    return;
  }

  socket_return_value = sl_si91x_listen(server_socket, BACK_LOG);
  if (socket_return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket listen failed with bsd error: %d\r\n", errno);
    close(server_socket);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Listening on Local Port : %d\r\n", DEVICE_PORT);

  client_socket = sl_si91x_accept(server_socket, NULL, 0);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket accept failed with bsd error: %d\r\n", errno);
    close(server_socket);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Client Socket ID : %d\r\n", client_socket);

  SL_DEBUG_LOG_V2(INFO, "TCP_RX Async Throughput test start\r\n");

#if CONTINUOUS_THROUGHPUT
  // Continuous throughput mode - loop forever with interval reporting
  while (1) {
    // Reset for each interval; re-arm callback each interval (data_callback is gated on enable_callback)
    has_data_received = 0;
    bytes_read        = 0;
    first_call        = 1;
    enable_callback   = 1;
    start             = osKernelGetTickCount();

    // Wait for TEST_TIMEOUT interval or max bytes
    while (!has_data_received) {
      osThreadYield();
    }

    now = osKernelGetTickCount();
    SL_DEBUG_LOG_V2(INFO, "Total bytes received : %ld\r\n", bytes_read);
    measure_and_print_throughput(bytes_read, (now - start));
  }
#else
  // Single measurement mode

  enable_callback = 1;
  while (!has_data_received) {
    osThreadYield();
  }

  SL_DEBUG_LOG_V2(INFO, "TCP_RX  Async Throughput test finished\r\n");
  SL_DEBUG_LOG_V2(INFO, "Total bytes received : %ld\r\n", bytes_read);

  close(server_socket);
  close(client_socket);

  measure_and_print_throughput(bytes_read, (now - start));
  has_data_received = 0;
#endif
}
#else
void receive_data_from_tcp_client(void)
{
  int server_socket = -1;
  int client_socket = -1;
  int socket_return_value = 0;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length = sizeof(struct sockaddr_in);
  uint8_t high_performance_socket = SL_HIGH_PERFORMANCE_SOCKET;
  uint32_t total_bytes_received = 0;
  uint32_t start = 0;
  uint32_t now = 0;
  int read_bytes = 1;
  sl_status_t status = SL_STATUS_FAIL;

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
    close(client_socket);
    return;
  }
  server_address.sin_family = AF_INET;
  server_address.sin_port = DEVICE_PORT;

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
  SL_DEBUG_LOG_V2(INFO, "Listening on Local Port : %d\r\n", DEVICE_PORT);

  client_socket = accept(server_socket, NULL, NULL);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket accept failed with bsd error: %d\r\n", errno);
    close(server_socket);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Client Socket ID : %d\r\n", client_socket);

  SL_DEBUG_LOG_V2(INFO, "TCP_RX Throughput test start\r\n");
  start = osKernelGetTickCount();
  while (1) {
    read_bytes = recv(client_socket, data_buffer, sizeof(data_buffer), 0);
    if (read_bytes < 0) {
      if (errno == 0) {
        // get the error code returned by the firmware
        status = sl_wifi_get_saved_firmware_status();
        if (status == SL_STATUS_SI91X_MEMORY_FAILED_FROM_MODULE) {
          continue;
        } else {
          SL_DEBUG_LOG_V2(ERROR, "TCP recv failed with BSD error = %d and status = 0x%lx\r\n", errno, status);
        }
      } else if (errno == ENOTCONN) {
        SL_DEBUG_LOG_V2(INFO, "Remote server terminated\r\n");
      } else {
        SL_DEBUG_LOG_V2(ERROR, "TLS recv failed with BSD error = %d\r\n", errno);
      }
      close(client_socket);
      close(server_socket);
      return;
    }

    total_bytes_received = total_bytes_received + read_bytes;
    now = osKernelGetTickCount();
    if ((now - start) > TEST_TIMEOUT) {
      SL_DEBUG_LOG_V2(INFO, "Total bytes received : %ld\r\n", total_bytes_received);
      measure_and_print_throughput(total_bytes_received, (now - start));
#if CONTINUOUS_THROUGHPUT
      total_bytes_received = 0;
      start = osKernelGetTickCount();
#else
      break;
#endif
    }
  }

  SL_DEBUG_LOG_V2(INFO, "TCP_RX Throughput test finished\r\n");
  close(client_socket);
  close(server_socket);
  return;
}
#endif

void send_data_to_tls_server(void)
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
    SL_DEBUG_LOG_V2(ERROR, "Socket creation failed with bsd error: %d\r\n", errno);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Socket ID : %d\r\n", client_socket);

  socket_return_value = setsockopt(client_socket, SOL_TCP, TCP_ULP, TLS, sizeof(TLS));
  if (socket_return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Set Socket option failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }

  server_address.sin_family = AF_INET;
  server_address.sin_port   = TLS_SERVER_PORT;
  sl_net_inet_addr(SERVER_IP_ADDRESS, &server_address.sin_addr.s_addr);

  socket_return_value = connect(client_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket Connect failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Socket connected to TLS server\r\n");

  SL_DEBUG_LOG_V2(INFO, "TLS_TX Throughput test start\r\n");
  start = osKernelGetTickCount();
  while (1) {
    sent_bytes = send(client_socket, data_buffer, TLS_BUFFER_SIZE, 0);
    if (sent_bytes < 0) {
      if (errno == ENOBUFS)
        continue;
      break;
    } else if (sent_bytes > 0) {
      total_bytes_sent = total_bytes_sent + sent_bytes;
      packets++;
    }
    now = osKernelGetTickCount();
    if ((now - start) > TEST_TIMEOUT) {
      SL_DEBUG_LOG_V2(INFO, "Total bytes sent : %ld\r\n", total_bytes_sent);
      SL_DEBUG_LOG_V2(INFO, "Number of packets sent : %ld\r\n", packets);
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

  SL_DEBUG_LOG_V2(INFO, "TLS_TX Throughput test finished\r\n");
  close(client_socket);

  return;
}

void receive_data_from_tls_client(void)
{
  int client_socket                 = -1;
  int socket_return_value           = 0;
  uint32_t total_bytes_received     = 0;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);
  uint8_t high_performance_socket   = SL_HIGH_PERFORMANCE_SOCKET;
  sl_status_t status                = SL_STATUS_FAIL;

  client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket creation failed with bsd error: %d\r\n", errno);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Socket ID : %d\r\n", client_socket);

  socket_return_value = setsockopt(client_socket, SOL_TCP, TCP_ULP, TLS, sizeof(TLS));
  if (socket_return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Set Socket option failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }

  socket_return_value = setsockopt(client_socket,
                                   SOL_SOCKET,
                                   SL_SO_HIGH_PERFORMANCE_SOCKET,
                                   &high_performance_socket,
                                   sizeof(high_performance_socket));
  if (socket_return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Set Socket option failed with BSD error: %d\r\n", errno);
    close(client_socket);
    return;
  }

  server_address.sin_family = AF_INET;
  server_address.sin_port   = TLS_SERVER_PORT;
  sl_net_inet_addr(SERVER_IP_ADDRESS, &server_address.sin_addr.s_addr);

  socket_return_value = connect(client_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket Connect failed with BSD error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Socket connected to TLS server\r\n");

  SL_DEBUG_LOG_V2(INFO, "TLS_RX Throughput test start\r\n");
  uint32_t start = osKernelGetTickCount();
  uint32_t now   = start;
  int read_bytes = 1;
  while (1) {
    read_bytes = recv(client_socket, data_buffer, sizeof(data_buffer), 0);
    if (read_bytes < 0) {
      if (errno == 0) {
        // get the error code returned by the firmware
        status = sl_wifi_get_saved_firmware_status();
        if (status == SL_STATUS_SI91X_MEMORY_FAILED_FROM_MODULE) {
          continue;
        } else {
          SL_DEBUG_LOG_V2(ERROR, "TLS recv failed with BSD error = %d and status = 0x%lx\r\n", errno, status);
        }
      } else if (errno == ENOTCONN) {
        SL_DEBUG_LOG_V2(INFO, "Remote server terminated\r\n");
      } else {
        SL_DEBUG_LOG_V2(ERROR, "TLS recv failed with BSD error = %d\r\n", errno);
      }
      close(client_socket);
      return;
    }

    total_bytes_received = total_bytes_received + read_bytes;
    now                  = osKernelGetTickCount();
    if ((now - start) > TEST_TIMEOUT) {
      SL_DEBUG_LOG_V2(INFO, "Total bytes received : %ld\r\n", total_bytes_received);
      measure_and_print_throughput(total_bytes_received, (now - start));
#if CONTINUOUS_THROUGHPUT
      total_bytes_received = 0;
      start                = osKernelGetTickCount();
#else
      break;
#endif
    }
  }

  SL_DEBUG_LOG_V2(INFO, "TLS_RX Throughput test finished\r\n");
  close(client_socket);
  return;
}

void wlan_throughput_task(void)
{

  for (size_t i = 0; i < sizeof(data_buffer); i++)
    data_buffer[i] = 'A' + (i % 26);
  SL_DEBUG_LOG_V2(INFO, "in wlan_throughput_task\r\n");
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
    case TLS_TX: {
      send_data_to_tls_server();
    } break;
    case TLS_RX: {
      receive_data_from_tls_client();
    } break;

      return;

      SL_DEBUG_LOG_V2(INFO, "In wlan task\r\n");
      osDelay(2000);
  }
}
