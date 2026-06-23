/***************************************************************************/ /**
 * @file
 * @brief Multithread Socket Example Application
 *
 * Demonstrates running multiple concurrent socket operations (TCP TX, TCP RX,
 * UDP TX/RX, TLS TX/RX) on separate RTOS threads using the SiWx91x
 * WiseConnect SDK.  Each socket workload runs in its own thread created by
 * create_newsocket_with_new_osthread().
 *
 * Threading model
 * ---------------
 *   app_init()
 *     |
 *     +-- application_start (RTOS thread, priority Low)
 *           |  -- Wi-Fi / network / credential init
 *           |  -- socket config
 *           +-- creates worker threads via create_newsocket_with_new_osthread()
 *                 +-- send_data_to_tcp_server   (TCP TX)
 *                 +-- receive_data_from_tcp_client (TCP RX, server-side)
 *                 +-- receive_data_from_tcp_server (TCP RX, client-side)
 *                 +-- (optionally) UDP TX/RX, TLS TX/RX
 *
 * Synchronization
 * ---------------
 *   - printf_mutex  : serializes LOG_PRINT output across all threads.
 *   - sock_data[]   : per-socket state updated from async callbacks and read
 *                     by the owning worker thread.  Fields are marked volatile
 *                     to ensure visibility across contexts.
 *
 * Resource management
 * -------------------
 *   Every worker thread owns its socket(s) and is responsible for closing them
 *   on both success and error paths before calling osThreadTerminate().
 *   The sockinfo_t block allocated by create_newsocket_with_new_osthread() is
 *   freed by the worker once it has copied the parameters it needs.
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
#include <stdio.h>
#include "sl_si91x_driver.h"
#include "sl_net_wifi_types.h"
#include "sl_si91x_socket_utility.h"
#include "sl_si91x_socket_constants.h"
#include "sl_si91x_core_utilities.h"
#include "sl_si91x_socket.h"
#ifdef SLI_SI91X_MCU_INTERFACE
#include "rsi_rom_clks.h"
#endif
#include "cacert.pem.h"
#include "app.h"
#include "sl_wifi_callback_framework.h"

/******************************************************
 *                      Macros
 ******************************************************/

// Socket type selector passed to create_newsocket_with_new_osthread()
#define SOCKET_TYPE_NONE  0
#define SOCKET_TYPE_ASYNC 1
#define SOCKET_TYPE_SYNC  2

// Maximum payload sizes per protocol (Si91x limits)
#define TCP_BUFFER_SIZE 1460
#define UDP_BUFFER_SIZE 1470
#define TLS_BUFFER_SIZE 1370

#define BACK_LOG 1

#define BYTES_TO_SEND    (1 << 30) // 1024 MB
#define BYTES_TO_RECEIVE (1 << 30) // 1024 MB
#define TEST_TIMEOUT     (30000)   // 30 sec

#define SL_HIGH_PERFORMANCE_SOCKET BIT(7)

osMutexId_t printf_mutex = 0;

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
 *                 Socket configuration
 *****************************************************/
#define TOTAL_SOCKETS                   3
#define TOTAL_TCP_SOCKETS               3
#define TOTAL_UDP_SOCKETS               0
#define TCP_TX_ONLY_SOCKETS             1
#define TCP_RX_ONLY_SOCKETS             1
#define UDP_TX_ONLY_SOCKETS             0
#define UDP_RX_ONLY_SOCKETS             0
#define TCP_RX_HIGH_PERFORMANCE_SOCKETS 1
#define TCP_RX_WINDOW_SIZE_CAP          44
#define TCP_RX_WINDOW_DIV_FACTOR        44

// Upper bound for socket indices tracked in sock_data[]
#define MAX_SERVER_CLIENT_SOCKETS (TOTAL_SOCKETS + TCP_RX_HIGH_PERFORMANCE_SOCKETS + TCP_RX_ONLY_SOCKETS)

/******************************************************
 *               Type Definitions
 ******************************************************/

/// Per-socket state used by async callbacks and the owning worker thread.
/// Fields modified from callbacks are volatile so the worker thread sees
/// updates without an explicit memory barrier.
typedef struct {
  uint32_t socket_id;
  volatile uint8_t is_data_received;
  volatile uint32_t bytes_read;
  volatile uint32_t start;
  volatile uint8_t first_data_frame;
} sock_data_t;

/// Parameters passed from create_newsocket_with_new_osthread() to worker.
/// The worker is responsible for calling free() on this block once it has
/// copied the fields it needs.
typedef struct {
  uint32_t port_num;
  uint8_t socket_type; // SOCKET_TYPE_NONE / ASYNC / SYNC
  char ipaddress[20];
} sockinfo_t;

/******************************************************
 *               Variable Definitions
 ******************************************************/

sl_net_wifi_client_profile_t profile = { 0 };
sock_data_t sock_data[MAX_SERVER_CLIENT_SOCKETS];

static const sl_wifi_device_configuration_t wifi_throughput_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .region_code = US,
  .boot_config = { .oper_mode = SL_SI91X_CLIENT_MODE,
                   .coex_mode = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map =
                     (SL_WIFI_FEAT_SECURITY_OPEN | SL_WIFI_FEAT_AGGREGATION | SL_WIFI_FEAT_WPS_DISABLE),
                   .tcp_ip_feature_bit_map = (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT | SL_SI91X_TCP_IP_FEAT_SSL
                                              | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID),
                   .custom_feature_bit_map =
                     (SL_WIFI_SYSTEM_CUSTOM_FEAT_EXTENSION_VALID | SL_SI91X_CUSTOM_FEAT_SOC_CLK_CONFIG_160MHZ),
                   .ext_custom_feature_bit_map = (MEMORY_CONFIG
#ifdef SLI_SI917
                                                  | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
                                                  ),
                   .bt_feature_bit_map = 0,
                   .ext_tcp_ip_feature_bit_map =
                     (SL_SI91X_EXT_TCP_IP_WINDOW_DIV | SL_SI91X_CONFIG_FEAT_EXTENSION_VALID
                      | SL_SI91X_EXT_TCP_IP_FEAT_SSL_THREE_SOCKETS | SL_SI91X_EXT_TCP_IP_WAIT_FOR_SOCKET_CLOSE),
                   .ble_feature_bit_map     = 0,
                   .ble_ext_feature_bit_map = 0,
                   .config_feature_bit_map  = SL_SI91X_FEAT_SLEEP_GPIO_SEL_BITMAP },
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
void receive_data_from_tcp_client(void *userinfo);
void send_data_to_tcp_server(void *userinfo);
void receive_data_from_udp_client(void *userinfo);
void send_data_to_udp_server(void *userinfo);
void receive_data_from_tls_server(void *userinfo);
void send_data_to_tls_server(void *userinfo);
void receive_data_from_tcp_server(void *userinfo);
static void application_start(void *argument);
static void measure_and_print_throughput(uint32_t total_num_of_bytes, uint32_t test_timeout, uint32_t socket_id);
void data_callback(uint32_t sock_no,
                   uint8_t *buffer,
                   uint32_t length,
                   const sl_si91x_socket_metadata_t *firmware_socket_response);

typedef void socket_handler(void *sockinfo);
osThreadId_t create_newsocket_with_new_osthread(socket_handler sock_handler,
                                                uint32_t port_number,
                                                char *ip_address,
                                                char *thread_name,
                                                uint8_t thread_priority,
                                                uint32_t thread_size,
                                                uint8_t socket_type);

/// Helper: close a socket and log the result. Returns the close status.
static inline sl_status_t close_and_log(int socket_id, const char *label, const char *threadname)
{
  sl_status_t status = close(socket_id);
  if (status == SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(INFO,
                    "%s Socket ID : %d closed successfully : %s",
                    (uintptr_t)label,
                    socket_id,
                    (uintptr_t)threadname);
  } else {
    SL_DEBUG_LOG_V2(ERROR, "%s Socket ID : %d close failed (0x%lx)", (uintptr_t)label, socket_id, status);
    SL_DEBUG_LOG_V2(ERROR, " : %s", (uintptr_t)threadname);
  }
  return status;
}

/******************************************************
 *               Function Definitions
 ******************************************************/

void app_init(void)
{
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

  osThreadId_t tid = osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
  if (tid == NULL) {
    printf("Failed to create application thread\r\n");
  }
}

static void measure_and_print_throughput(uint32_t total_num_of_bytes, uint32_t test_timeout, uint32_t socket_id)
{
  float duration = (test_timeout / 1000.0f);
  float result   = ((float)total_num_of_bytes * 8) / duration; // bytes to bps
  result         = (result / 1000000);                         // bps to Mbps
  // NOTE: SL_DEBUG_LOG_V2 binary encoder doesn't support %f; review later.
  SL_DEBUG_LOG_V2(INFO,
                  "Throughput achieved @ %0.02f Mbps in %0.03f sec successfully : socket id %lu",
                  result,
                  duration,
                  socket_id);
}

/// Async receive callback invoked by the firmware for each data frame.
/// Updates per-socket state in sock_data[]; the owning worker thread polls
/// is_data_received to know when the test is complete.
void data_callback(uint32_t sock_no,
                   uint8_t *buffer,
                   uint32_t length,
                   const sl_si91x_socket_metadata_t *firmware_socket_response)
{
  UNUSED_PARAMETER(buffer);
  UNUSED_PARAMETER(firmware_socket_response);

  if (sock_no >= MAX_SERVER_CLIENT_SOCKETS) {
    return;
  }
  if (sock_data[sock_no].is_data_received == 1) {
    return;
  }
  if (sock_data[sock_no].first_data_frame == 0) {
    sock_data[sock_no].start = osKernelGetTickCount();
    SL_DEBUG_LOG_V2(INFO, "Throughput test start and Client Socket ID : %lu\r\n", sock_no);
    sock_data[sock_no].first_data_frame = 1;
  }

  sock_data[sock_no].bytes_read += length;
  uint32_t now = osKernelGetTickCount();
  if ((sock_data[sock_no].bytes_read >= BYTES_TO_RECEIVE) || ((now - sock_data[sock_no].start) >= TEST_TIMEOUT)) {
    SL_DEBUG_LOG_V2(INFO, "Throughput test finished : Client Socket ID %lu\r\n", sock_no);
    SL_DEBUG_LOG_V2(INFO,
                    "Total bytes received : %ld : Client Socket ID %lu\r\n",
                    sock_data[sock_no].bytes_read,
                    sock_no);
    measure_and_print_throughput(sock_data[sock_no].bytes_read, (now - sock_data[sock_no].start), sock_no);
    sock_data[sock_no].is_data_received = 1;
    sock_data[sock_no].bytes_read       = 0;
    sock_data[sock_no].start            = 0;
  }
}

/// Called when the remote peer terminates the connection. Signals the worker
/// thread to stop waiting for data.
void remote_socket_termination(int socket, uint16_t port, uint32_t bytes_sent)
{
  UNUSED_PARAMETER(port);
  UNUSED_PARAMETER(bytes_sent);
  if ((socket < 0) || (socket >= MAX_SERVER_CLIENT_SOCKETS)) {
    SL_DEBUG_LOG_V2(WARN, "Received remote termination on Invalid Client Socket ID %d", socket);
    return;
  }
  SL_DEBUG_LOG_V2(WARN, "Received remote termination on Client Socket ID %d", socket);
  sock_data[socket].is_data_received = 1;
}

static void application_start(void *argument)
{
  UNUSED_PARAMETER(argument);
  sl_status_t status;
  sl_mac_address_t mac_addr                   = { 0 };
  sl_wifi_firmware_version_t firmware_version = { 0 };

  printf_mutex = osMutexNew(NULL);
  if (printf_mutex == NULL) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to create printf_mutex\r\n");
    return;
  }

  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &wifi_throughput_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to start Wi-Fi Client interface: 0x%lx\r\n", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Wi-Fi client interface init success\r\n");

  status = sl_wifi_get_mac_address(SL_WIFI_CLIENT_INTERFACE, &mac_addr);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to get mac address: 0x%lx\r\n", status);
    return;
  }
  char mt_sock_mac_log[80];
  snprintf(mt_sock_mac_log,
           sizeof(mt_sock_mac_log),
           "Device MAC address: %x:%x:%x:%x:%x:%x",
           mac_addr.octet[0],
           mac_addr.octet[1],
           mac_addr.octet[2],
           mac_addr.octet[3],
           mac_addr.octet[4],
           mac_addr.octet[5]);
  SL_DEBUG_LOG_V2(INFO, "%s", (uintptr_t)mt_sock_mac_log);

  status = sl_wifi_get_firmware_version(&firmware_version);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to fetch firmware version: 0x%lx\r\n", status);
    return;
  } else {
    print_firmware_version(&firmware_version);
  }

  status = sl_net_up(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to connect to AP: 0x%lx\r\n", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Wi-Fi client connected\r\n");

  status = sl_net_get_profile(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID, &profile);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to get client profile: 0x%lx\r\n", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Success to get client profile\r\n");
  {
    sl_ip_address_t ip_address = { 0 };
    ip_address.type            = SL_IPV4;
    memcpy(&ip_address.ip.v4.bytes, &profile.ip.ip.v4.ip_address.bytes, sizeof(sl_ipv4_address_t));
    print_sl_ip_address(&ip_address);
  }

  // Load SSL CA certificate (needed for TLS socket paths)
  status =
    sl_net_set_credential(SL_NET_TLS_SERVER_CREDENTIAL_ID(0), SL_NET_SIGNING_CERTIFICATE, cacert, sizeof(cacert) - 1);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Loading TLS CA certificate in to FLASH Failed, Error Code : 0x%lX\r\n", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Load SSL CA certificate at index %d Success\r\n", 0);

  status = sl_si91x_config_socket(socket_config);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Socket config failed: 0x%lx", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Socket config Done");

  sli_si91x_set_remote_socket_termination_callback(remote_socket_termination);

  // --- Launch worker threads for each socket workload ---
  // Adjust SERVER_IP and port numbers to match your test environment.

  create_newsocket_with_new_osthread(send_data_to_tcp_server,
                                     5001,
                                     "192.168.0.156",
                                     "TCP_TX",
                                     osPriorityLow,
                                     (1024 * 4),
                                     SOCKET_TYPE_NONE);

  create_newsocket_with_new_osthread(receive_data_from_tcp_client,
                                     5004,
                                     "",
                                     "TCP_RX",
                                     osPriorityLow,
                                     (1024 * 4),
                                     SOCKET_TYPE_SYNC);

  create_newsocket_with_new_osthread(receive_data_from_tcp_server,
                                     5005,
                                     "192.168.8.31",
                                     "TCP_RX1",
                                     osPriorityLow,
                                     (1024 * 4),
                                     SOCKET_TYPE_SYNC);

  // To enable UDP or TLS tests, uncomment the corresponding blocks below and
  // update the socket configuration macros (TOTAL_SOCKETS, etc.) accordingly.
  //
  // create_newsocket_with_new_osthread(send_data_to_udp_server, 5002, "192.168.0.156",
  //                                    "UDP_TX", osPriorityLow, (1024 * 4), SOCKET_TYPE_NONE);
  //
  // create_newsocket_with_new_osthread(receive_data_from_udp_client, 5004, "",
  //                                    "UDP_RX", osPriorityLow, (1024 * 4), SOCKET_TYPE_SYNC);
  //
  // create_newsocket_with_new_osthread(send_data_to_tls_server, 5010, "192.168.0.156",
  //                                    "TLS_TX", osPriorityLow, (1024 * 4), SOCKET_TYPE_NONE);
  //
  // create_newsocket_with_new_osthread(receive_data_from_tls_server, 5011, "192.168.0.156",
  //                                    "TLS_RX", osPriorityLow, (1024 * 4), SOCKET_TYPE_SYNC);

  SL_DEBUG_LOG_V2(INFO, "All worker threads launched, application_start exiting.");
  osThreadTerminate(osThreadGetId());
}

osThreadId_t create_newsocket_with_new_osthread(socket_handler sock_handler,
                                                uint32_t port_number,
                                                char *ip_address,
                                                char *thread_name,
                                                uint8_t thread_priority,
                                                uint32_t thread_size,
                                                uint8_t socket_type)
{
  sockinfo_t *sockinfo = (sockinfo_t *)malloc(sizeof(sockinfo_t));
  if (sockinfo == NULL) {
    SL_DEBUG_LOG_V2(ERROR,
                    "Failed to allocate sockinfo for thread %s",
                    (uintptr_t)(thread_name ? thread_name : "unknown"));
    return NULL;
  }

  memset(sockinfo, 0, sizeof(sockinfo_t));
  sockinfo->port_num    = port_number;
  sockinfo->socket_type = socket_type;

  if (ip_address != NULL && ip_address[0] != '\0') {
    size_t len = strlen(ip_address);
    if (len >= sizeof(sockinfo->ipaddress)) {
      len = sizeof(sockinfo->ipaddress) - 1;
    }
    memcpy(sockinfo->ipaddress, ip_address, len);
    sockinfo->ipaddress[len] = '\0';
  }

  osThreadAttr_t thread_attributes = { 0 };
  if (thread_name != NULL) {
    thread_attributes.name = thread_name;
  }
  thread_attributes.priority   = (uint8_t)thread_priority;
  thread_attributes.stack_size = thread_size;

  osThreadId_t tid = osThreadNew((osThreadFunc_t)sock_handler, sockinfo, &thread_attributes);
  if (tid == NULL) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to create thread: %s", (uintptr_t)(thread_name ? thread_name : "unknown"));
    free(sockinfo);
  }
  return tid;
}

/******************************************************
 *           TCP TX - send_data_to_tcp_server
 ******************************************************/
void send_data_to_tcp_server(void *userinfo)
{
  int client_socket                 = -1;
  uint32_t total_bytes_sent         = 0;
  int socket_return_value           = 0;
  int sent_bytes                    = 1;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);
  sockinfo_t *sockinfo              = (sockinfo_t *)userinfo;
  uint32_t start                    = 0;
  uint32_t now                      = 0;
  uint8_t data_buffer[TCP_BUFFER_SIZE];
  const char *threadname = osThreadGetName(osThreadGetId());

  for (size_t i = 0; i < sizeof(data_buffer); i++)
    data_buffer[i] = 'A';

  server_address.sin_family = AF_INET;
  server_address.sin_port   = sockinfo->port_num;

  if (sockinfo->ipaddress[0] == '\0') {
    SL_DEBUG_LOG_V2(ERROR, "Invalid IP Address : %s\r\n", (uintptr_t)threadname);
    free(userinfo);
    osThreadTerminate(osThreadGetId());
    return; // defensive: osThreadTerminate may not return
  }

  sl_status_t addr_status = sl_net_inet_addr(sockinfo->ipaddress, &server_address.sin_addr.s_addr);
  if (addr_status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR,
                    "Invalid server IP address '%s' : %s",
                    (uintptr_t)sockinfo->ipaddress,
                    (uintptr_t)threadname);
    free(userinfo);
    osThreadTerminate(osThreadGetId());
    return;
  }
  free(userinfo);
  userinfo = NULL;

  client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket Create failed with bsd error: %d : %s\r\n", errno, (uintptr_t)threadname);
    osThreadTerminate(osThreadGetId());
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Socket ID : %d : %s\r\n", client_socket, (uintptr_t)threadname);

  socket_return_value = connect(client_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket Connect failed with bsd error: %d : %s\r\n", errno, (uintptr_t)threadname);
    close_and_log(client_socket, "Client", threadname);
    osThreadTerminate(osThreadGetId());
    return;
  }
  SL_DEBUG_LOG_V2(INFO,
                  "Socket connected to TCP server, Client Socket ID %d : %s",
                  client_socket,
                  (uintptr_t)threadname);

  SL_DEBUG_LOG_V2(INFO, "TCP_TX Throughput test start, Client Socket ID %d : %s", client_socket, (uintptr_t)threadname);
  start = osKernelGetTickCount();
  while (total_bytes_sent < BYTES_TO_SEND) {
    sent_bytes = send(client_socket, data_buffer, TCP_BUFFER_SIZE, 0);
    if (sent_bytes < 0) {
      if (errno == ENOBUFS)
        continue;
      SL_DEBUG_LOG_V2(ERROR, "TCP send failed with BSD error : %d : %s", errno, (uintptr_t)threadname);
      break;
    }
    total_bytes_sent = total_bytes_sent + sent_bytes;
    now              = osKernelGetTickCount();
    if ((now - start) >= TEST_TIMEOUT) {
      SL_DEBUG_LOG_V2(INFO, "Time Out: %ld : %s\r\n", (now - start), (uintptr_t)threadname);
      break;
    }
  }
  SL_DEBUG_LOG_V2(INFO, "TCP_TX Throughput test finished : %s", (uintptr_t)threadname);
  SL_DEBUG_LOG_V2(INFO, "Total bytes sent : %ld : %s\r\n", total_bytes_sent, (uintptr_t)threadname);
  measure_and_print_throughput(total_bytes_sent, (now - start), client_socket);

  close_and_log(client_socket, "Client", threadname);
  osThreadTerminate(osThreadGetId());
}

/******************************************************
 *  TCP RX client-side - receive_data_from_tcp_server
 ******************************************************/
void receive_data_from_tcp_server(void *userinfo)
{
  int client_socket                 = -1;
  int socket_return_value           = 0;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);
  sockinfo_t *sockinfo              = (sockinfo_t *)userinfo;
  const char *threadname            = osThreadGetName(osThreadGetId());

  server_address.sin_family = AF_INET;
  server_address.sin_port   = sockinfo->port_num;
  uint8_t socket_type       = sockinfo->socket_type;

  if (sockinfo->ipaddress[0] == '\0') {
    SL_DEBUG_LOG_V2(ERROR, "Invalid IP Address : %s\r\n", (uintptr_t)threadname);
    free(userinfo);
    osThreadTerminate(osThreadGetId());
    return;
  }

  sl_status_t addr_status = sl_net_inet_addr(sockinfo->ipaddress, &server_address.sin_addr.s_addr);
  if (addr_status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR,
                    "Invalid server IP address '%s' : %s",
                    (uintptr_t)sockinfo->ipaddress,
                    (uintptr_t)threadname);
    free(userinfo);
    osThreadTerminate(osThreadGetId());
    return;
  }
  free(userinfo);
  userinfo = NULL;

  if (socket_type == SOCKET_TYPE_ASYNC) {
    client_socket = sl_si91x_socket_async(AF_INET, SOCK_STREAM, IPPROTO_TCP, &data_callback);
    if (client_socket < 0) {
      SL_DEBUG_LOG_V2(ERROR, "Socket creation failed with bsd error: %d : %s\r\n", errno, (uintptr_t)threadname);
      osThreadTerminate(osThreadGetId());
      return;
    }

    socket_return_value = connect(client_socket, (struct sockaddr *)&server_address, socket_length);
    if (socket_return_value < 0) {
      SL_DEBUG_LOG_V2(ERROR, "Socket Connect failed with bsd error: %d : %s\r\n", errno, (uintptr_t)threadname);
      close_and_log(client_socket, "Client", threadname);
      osThreadTerminate(osThreadGetId());
      return;
    }
    SL_DEBUG_LOG_V2(INFO, "Socket connected to TCP server: %s\r\n", (uintptr_t)threadname);

    if (client_socket < 0 || client_socket >= MAX_SERVER_CLIENT_SOCKETS) {
      SL_DEBUG_LOG_V2(ERROR, "Invalid client socket index %d : %s", client_socket, (uintptr_t)threadname);
      close_and_log(client_socket, "Client", threadname);
      osThreadTerminate(osThreadGetId());
      return;
    }
    while (!sock_data[client_socket].is_data_received) {
      osThreadYield();
    }
  } else {
    uint32_t start = 0;
    uint32_t now   = 0;
    uint8_t data_buffer[TCP_BUFFER_SIZE];
    int32_t read_bytes            = 1;
    uint32_t total_bytes_received = 0;

    client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket < 0) {
      SL_DEBUG_LOG_V2(ERROR, "Socket Create failed with bsd error: %d : %s\r\n", errno, (uintptr_t)threadname);
      osThreadTerminate(osThreadGetId());
      return;
    }
    SL_DEBUG_LOG_V2(INFO, "Socket ID : %d : %s\r\n", client_socket, (uintptr_t)threadname);

    socket_return_value = connect(client_socket, (struct sockaddr *)&server_address, socket_length);
    if (socket_return_value < 0) {
      SL_DEBUG_LOG_V2(ERROR, "Socket Connect failed with bsd error: %d : %s\r\n", errno, (uintptr_t)threadname);
      close_and_log(client_socket, "Client", threadname);
      osThreadTerminate(osThreadGetId());
      return;
    }
    SL_DEBUG_LOG_V2(INFO, "Socket connected to TCP server: %s\r\n", (uintptr_t)threadname);

    SL_DEBUG_LOG_V2(INFO,
                    "TCP_RX Throughput test start, Client Socket ID %d : %s",
                    client_socket,
                    (uintptr_t)threadname);
    start = osKernelGetTickCount();
    while (1) {
      read_bytes = recv(client_socket, data_buffer, sizeof(data_buffer), 0);
      if (read_bytes < 0) {
        if (errno == 0) {
          sl_status_t fw_status = sl_wifi_get_saved_firmware_status();
          if (fw_status == SL_STATUS_SI91X_MEMORY_FAILED_FROM_MODULE) {
            continue;
          } else {
            SL_DEBUG_LOG_V2(ERROR,
                            "TCP recv failed with BSD error = %d and status = 0x%lx : %s",
                            errno,
                            fw_status,
                            (uintptr_t)threadname);
          }
        } else if (errno == ENOTCONN) {
          SL_DEBUG_LOG_V2(WARN,
                          "Remote server terminated on Client Socket ID %d : %s",
                          client_socket,
                          (uintptr_t)threadname);
        } else {
          SL_DEBUG_LOG_V2(ERROR, "TCP recv failed with BSD error = %d : %s", errno, (uintptr_t)threadname);
        }
        break;
      }
      total_bytes_received = total_bytes_received + read_bytes;
      now                  = osKernelGetTickCount();
      if ((now - start) > TEST_TIMEOUT) {
        SL_DEBUG_LOG_V2(INFO, "Test Time Out: %ld ms : %s", (now - start), (uintptr_t)threadname);
        break;
      }
    }
    SL_DEBUG_LOG_V2(INFO, "TCP_RX Throughput test finished : %s", (uintptr_t)threadname);
    SL_DEBUG_LOG_V2(INFO, "Total bytes received : %ld : %s", total_bytes_received, (uintptr_t)threadname);
    measure_and_print_throughput(total_bytes_received, (now - start), client_socket);
  }

  close_and_log(client_socket, "Client", threadname);
  osThreadTerminate(osThreadGetId());
}

/******************************************************
 *  TCP RX server-side - receive_data_from_tcp_client
 ******************************************************/
void receive_data_from_tcp_client(void *userinfo)
{
  int server_socket                 = -1;
  int client_socket                 = -1;
  int socket_return_value           = 0;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);
  uint32_t high_performance_socket  = SL_HIGH_PERFORMANCE_SOCKET;
  sockinfo_t *sockinfo              = (sockinfo_t *)userinfo;
  const char *threadname            = osThreadGetName(osThreadGetId());

  server_address.sin_port = sockinfo->port_num;
  uint8_t socket_type     = sockinfo->socket_type;
  free(userinfo);
  userinfo = NULL;

  if (socket_type == SOCKET_TYPE_ASYNC) {
    server_address.sin_family = AF_INET;
    server_socket             = sl_si91x_socket_async(AF_INET, SOCK_STREAM, IPPROTO_TCP, &data_callback);
    if (server_socket < 0) {
      SL_DEBUG_LOG_V2(ERROR, "Socket creation failed with bsd error: %d : %s\r\n", errno, (uintptr_t)threadname);
      osThreadTerminate(osThreadGetId());
      return;
    }
    SL_DEBUG_LOG_V2(INFO, "Server Socket ID : %d : %s\r\n", server_socket, (uintptr_t)threadname);

    socket_return_value = sl_si91x_setsockopt(server_socket,
                                              SOL_SOCKET,
                                              SL_SI91X_SO_HIGH_PERFORMANCE_SOCKET,
                                              &high_performance_socket,
                                              sizeof(high_performance_socket));
    if (socket_return_value < 0) {
      SL_DEBUG_LOG_V2(ERROR, "Set Socket option failed with bsd error: %d : %s\r\n", errno, (uintptr_t)threadname);
      close_and_log(server_socket, "Server", threadname);
      osThreadTerminate(osThreadGetId());
      return;
    }

    socket_return_value = sl_si91x_bind(server_socket, (struct sockaddr *)&server_address, socket_length);
    if (socket_return_value < 0) {
      SL_DEBUG_LOG_V2(ERROR, "Socket bind failed with bsd error: %d : %s\r\n", errno, (uintptr_t)threadname);
      close_and_log(server_socket, "Server", threadname);
      osThreadTerminate(osThreadGetId());
      return;
    }

    socket_return_value = sl_si91x_listen(server_socket, BACK_LOG);
    if (socket_return_value < 0) {
      SL_DEBUG_LOG_V2(ERROR, "Socket listen failed with bsd error: %d : %s", errno, (uintptr_t)threadname);
      close_and_log(server_socket, "Server", threadname);
      osThreadTerminate(osThreadGetId());
      return;
    }
    SL_DEBUG_LOG_V2(INFO, "Listening on Local Port : %u : %s", server_address.sin_port, (uintptr_t)threadname);

    client_socket = sl_si91x_accept(server_socket, NULL, 0);
    if (client_socket < 0) {
      SL_DEBUG_LOG_V2(ERROR, "Socket accept failed with bsd error: %d : %s", errno, (uintptr_t)threadname);
      close_and_log(server_socket, "Server", threadname);
      osThreadTerminate(osThreadGetId());
      return;
    }
    if (client_socket >= MAX_SERVER_CLIENT_SOCKETS) {
      SL_DEBUG_LOG_V2(ERROR, "Invalid client socket index %d : %s", client_socket, (uintptr_t)threadname);
      close_and_log(client_socket, "Client", threadname);
      close_and_log(server_socket, "Server", threadname);
      osThreadTerminate(osThreadGetId());
      return;
    }
    while (!sock_data[client_socket].is_data_received) {
      osThreadYield();
    }
  } else {
    int read_bytes                = 1;
    uint32_t total_bytes_received = 0;
    uint8_t data_buffer[TCP_BUFFER_SIZE];
    uint32_t start = 0;
    uint32_t now   = 0;

    server_address.sin_family = AF_INET;
    server_socket             = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket < 0) {
      SL_DEBUG_LOG_V2(ERROR, "Socket creation failed with bsd error: %d : %s\r\n", errno, (uintptr_t)threadname);
      osThreadTerminate(osThreadGetId());
      return;
    }
    SL_DEBUG_LOG_V2(INFO, "Server Socket ID : %d : %s\r\n", server_socket, (uintptr_t)threadname);

    socket_return_value = setsockopt(server_socket,
                                     SOL_SOCKET,
                                     SL_SO_HIGH_PERFORMANCE_SOCKET,
                                     &high_performance_socket,
                                     sizeof(high_performance_socket));
    if (socket_return_value < 0) {
      SL_DEBUG_LOG_V2(ERROR, "Set Socket option failed with bsd error: %d : %s\r\n", errno, (uintptr_t)threadname);
      close_and_log(server_socket, "Server", threadname);
      osThreadTerminate(osThreadGetId());
      return;
    }

    socket_return_value = bind(server_socket, (struct sockaddr *)&server_address, socket_length);
    if (socket_return_value < 0) {
      SL_DEBUG_LOG_V2(ERROR, "Socket bind failed with bsd error: %d : %s\r\n", errno, (uintptr_t)threadname);
      close_and_log(server_socket, "Server", threadname);
      osThreadTerminate(osThreadGetId());
      return;
    }

    socket_return_value = listen(server_socket, BACK_LOG);
    if (socket_return_value < 0) {
      SL_DEBUG_LOG_V2(ERROR, "Socket listen failed with bsd error: %d : %s", errno, (uintptr_t)threadname);
      close_and_log(server_socket, "Server", threadname);
      osThreadTerminate(osThreadGetId());
      return;
    }
    SL_DEBUG_LOG_V2(INFO, "Listening on Local Port : %u : %s", server_address.sin_port, (uintptr_t)threadname);

    client_socket = accept(server_socket, NULL, NULL);
    if (client_socket < 0) {
      SL_DEBUG_LOG_V2(ERROR, "Socket accept failed with bsd error: %d : %s", errno, (uintptr_t)threadname);
      close_and_log(server_socket, "Server", threadname);
      osThreadTerminate(osThreadGetId());
      return;
    }
    SL_DEBUG_LOG_V2(INFO, "Client Socket ID : %d : %s", client_socket, (uintptr_t)threadname);

    SL_DEBUG_LOG_V2(INFO,
                    "TCP_RX Throughput test start, Client Socket ID %d : %s",
                    client_socket,
                    (uintptr_t)threadname);
    start = osKernelGetTickCount();
    while (1) {
      read_bytes = recv(client_socket, data_buffer, sizeof(data_buffer), 0);
      if (read_bytes < 0) {
        if (errno == 0) {
          sl_status_t fw_status = sl_wifi_get_saved_firmware_status();
          if (fw_status == SL_STATUS_SI91X_MEMORY_FAILED_FROM_MODULE) {
            continue;
          } else {
            SL_DEBUG_LOG_V2(ERROR,
                            "TCP recv failed with BSD error = %d and status = 0x%lx : %s",
                            errno,
                            fw_status,
                            (uintptr_t)threadname);
          }
        } else {
          SL_DEBUG_LOG_V2(ERROR, "TCP recv failed with BSD error = %d : %s", errno, (uintptr_t)threadname);
        }
        break;
      }
      total_bytes_received = total_bytes_received + read_bytes;
      now                  = osKernelGetTickCount();
      if ((now - start) > TEST_TIMEOUT) {
        SL_DEBUG_LOG_V2(INFO, "Test Time Out: %ld ms : %s", (now - start), (uintptr_t)threadname);
        break;
      }
    }
    SL_DEBUG_LOG_V2(INFO, "TCP_RX Throughput test finished : %s", (uintptr_t)threadname);
    SL_DEBUG_LOG_V2(INFO, "Total bytes received : %ld : %s", total_bytes_received, (uintptr_t)threadname);
    measure_and_print_throughput(total_bytes_received, (now - start), client_socket);
  }

  // Close client (accepted) socket before server (listening) socket on
  // NCP & SOC; otherwise client close fails.
  if (client_socket >= 0) {
    close_and_log(client_socket, "Client", threadname);
    client_socket = -1;
  }
  if (server_socket >= 0) {
    close_and_log(server_socket, "Server", threadname);
    server_socket = -1;
  }
  osThreadTerminate(osThreadGetId());
}

/******************************************************
 *           UDP TX - send_data_to_udp_server
 ******************************************************/
void send_data_to_udp_server(void *userinfo)
{
  int client_socket                 = -1;
  uint32_t total_bytes_sent         = 0;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);
  int sent_bytes                    = 1;
  sockinfo_t *sockinfo              = (sockinfo_t *)userinfo;
  uint32_t start = 0, now = 0;
  uint8_t data_buffer[UDP_BUFFER_SIZE];
  const char *threadname = osThreadGetName(osThreadGetId());

  if (sockinfo->ipaddress[0] == '\0') {
    SL_DEBUG_LOG_V2(ERROR, "Invalid IP Address : %s\r\n", (uintptr_t)threadname);
    free(userinfo);
    osThreadTerminate(osThreadGetId());
    return;
  }

  for (size_t i = 0; i < sizeof(data_buffer); i++)
    data_buffer[i] = 'C';

  server_address.sin_family = AF_INET;
  server_address.sin_port   = sockinfo->port_num;

  sl_status_t addr_status = sl_net_inet_addr(sockinfo->ipaddress, &server_address.sin_addr.s_addr);
  if (addr_status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR,
                    "Invalid server IP address '%s' : %s",
                    (uintptr_t)sockinfo->ipaddress,
                    (uintptr_t)threadname);
    free(userinfo);
    osThreadTerminate(osThreadGetId());
    return;
  }
  free(userinfo);
  userinfo = NULL;

  client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket creation failed with bsd error: %d : %s\r\n", errno, (uintptr_t)threadname);
    osThreadTerminate(osThreadGetId());
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Socket ID : %d : %s\r\n", client_socket, (uintptr_t)threadname);

  SL_DEBUG_LOG_V2(INFO, "UDP_TX Throughput test start : %s\r\n", (uintptr_t)threadname);
  start = osKernelGetTickCount();
  while (total_bytes_sent < BYTES_TO_SEND) {
    sent_bytes =
      sendto(client_socket, data_buffer, UDP_BUFFER_SIZE, 0, (struct sockaddr *)&server_address, socket_length);
    if (sent_bytes < 0) {
      if (errno == ENOBUFS)
        continue;
      SL_DEBUG_LOG_V2(ERROR, "UDP send failed with BSD error : %d : %s", errno, (uintptr_t)threadname);
      break;
    }
    total_bytes_sent = total_bytes_sent + sent_bytes;
    now              = osKernelGetTickCount();
    if ((now - start) >= TEST_TIMEOUT) {
      SL_DEBUG_LOG_V2(INFO, "Time Out: %ld : %s\r\n", (now - start), (uintptr_t)threadname);
      break;
    }
  }
  SL_DEBUG_LOG_V2(INFO, "UDP_TX Throughput test finished : %s\r\n", (uintptr_t)threadname);
  SL_DEBUG_LOG_V2(INFO, "Total bytes sent : %ld : %s\r\n", total_bytes_sent, (uintptr_t)threadname);
  measure_and_print_throughput(total_bytes_sent, (now - start), client_socket);

  close_and_log(client_socket, "Client", threadname);
  osThreadTerminate(osThreadGetId());
}

/******************************************************
 *       UDP RX - receive_data_from_udp_client
 ******************************************************/
void receive_data_from_udp_client(void *userinfo)
{
  int client_socket = -1;

  int socket_return_value           = 0;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);
  sockinfo_t *sockinfo              = (sockinfo_t *)userinfo;
  const char *threadname            = osThreadGetName(osThreadGetId());

  server_address.sin_port = sockinfo->port_num;
  uint8_t socket_type     = sockinfo->socket_type;
  free(userinfo);
  userinfo = NULL;

  if (socket_type == SOCKET_TYPE_ASYNC) {
    server_address.sin_family = AF_INET;
    client_socket             = sl_si91x_socket_async(AF_INET, SOCK_DGRAM, IPPROTO_UDP, &data_callback);
    if (client_socket < 0) {
      SL_DEBUG_LOG_V2(ERROR, "Socket creation failed with bsd error: %d : %s\r\n", errno, (uintptr_t)threadname);
      osThreadTerminate(osThreadGetId());
      return;
    }
    SL_DEBUG_LOG_V2(INFO, "Socket ID : %d : %s\r\n", client_socket, (uintptr_t)threadname);

    socket_return_value = sl_si91x_bind(client_socket, (struct sockaddr *)&server_address, socket_length);
    if (socket_return_value < 0) {
      SL_DEBUG_LOG_V2(ERROR, "Socket bind failed with bsd error: %d : %s\r\n", errno, (uintptr_t)threadname);
      close_and_log(client_socket, "Client", threadname);
      osThreadTerminate(osThreadGetId());
      return;
    }
    SL_DEBUG_LOG_V2(INFO, "Listening on Local Port %u : %s", server_address.sin_port, (uintptr_t)threadname);

    if (client_socket < 0 || client_socket >= MAX_SERVER_CLIENT_SOCKETS) {
      SL_DEBUG_LOG_V2(ERROR, "Invalid client socket index %d : %s", client_socket, (uintptr_t)threadname);
      close_and_log(client_socket, "Client", threadname);
      osThreadTerminate(osThreadGetId());
      return;
    }
    while (!sock_data[client_socket].is_data_received) {
      osThreadYield();
    }
  } else {
    int read_bytes                = 1;
    uint32_t start                = 0;
    uint32_t now                  = 0;
    uint32_t total_bytes_received = 0;
    uint8_t data_buffer[UDP_BUFFER_SIZE];

    server_address.sin_family = AF_INET;
    client_socket             = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (client_socket < 0) {
      SL_DEBUG_LOG_V2(ERROR, "Socket creation failed with bsd error: %d : %s\r\n", errno, (uintptr_t)threadname);
      osThreadTerminate(osThreadGetId());
      return;
    }
    SL_DEBUG_LOG_V2(INFO, "Socket ID : %d : %s\r\n", client_socket, (uintptr_t)threadname);

    socket_return_value = bind(client_socket, (struct sockaddr *)&server_address, socket_length);
    if (socket_return_value < 0) {
      SL_DEBUG_LOG_V2(ERROR, "Socket bind failed with bsd error: %d : %s\r\n", errno, (uintptr_t)threadname);
      close_and_log(client_socket, "Client", threadname);
      osThreadTerminate(osThreadGetId());
      return;
    }
    SL_DEBUG_LOG_V2(INFO, "Listening on Local Port %u : %s", server_address.sin_port, (uintptr_t)threadname);

    SL_DEBUG_LOG_V2(INFO,
                    "UDP_RX Throughput test start, Client Socket ID %d : %s",
                    client_socket,
                    (uintptr_t)threadname);
    start = osKernelGetTickCount();
    while (1) {
      read_bytes = recvfrom(client_socket, data_buffer, sizeof(data_buffer), 0, NULL, NULL);
      if (read_bytes < 0) {
        if (errno == 0) {
          sl_status_t fw_status = sl_wifi_get_saved_firmware_status();
          if (fw_status == SL_STATUS_SI91X_MEMORY_FAILED_FROM_MODULE) {
            continue;
          } else {
            SL_DEBUG_LOG_V2(ERROR,
                            "UDP recv failed with BSD error = %d and status = 0x%lx : %s",
                            errno,
                            fw_status,
                            (uintptr_t)threadname);
          }
        } else {
          SL_DEBUG_LOG_V2(ERROR, "UDP recv failed with BSD error = %d : %s", errno, (uintptr_t)threadname);
        }
        break;
      }
      total_bytes_received = total_bytes_received + read_bytes;
      now                  = osKernelGetTickCount();
      if ((now - start) > TEST_TIMEOUT) {
        SL_DEBUG_LOG_V2(INFO, "Test Time Out: %ld ms : %s", (now - start), (uintptr_t)threadname);
        break;
      }
    }
    SL_DEBUG_LOG_V2(INFO, "UDP_RX Throughput test finished : %s", (uintptr_t)threadname);
    SL_DEBUG_LOG_V2(INFO, "Total bytes received : %ld : %s", total_bytes_received, (uintptr_t)threadname);
    measure_and_print_throughput(total_bytes_received, (now - start), client_socket);
  }

  close_and_log(client_socket, "Client", threadname);
  osThreadTerminate(osThreadGetId());
}

/******************************************************
 *       TLS RX - receive_data_from_tls_server
 ******************************************************/
void receive_data_from_tls_server(void *userinfo)
{
  int client_socket       = -1;
  int socket_return_value = 0;

  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);
  uint32_t high_performance_socket  = SL_HIGH_PERFORMANCE_SOCKET;
  sockinfo_t *sockinfo              = (sockinfo_t *)userinfo;
  const char *threadname            = osThreadGetName(osThreadGetId());

  server_address.sin_port = sockinfo->port_num;
  uint8_t socket_type     = sockinfo->socket_type;

  sl_status_t addr_status = sl_net_inet_addr(sockinfo->ipaddress, &server_address.sin_addr.s_addr);
  if (addr_status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR,
                    "Invalid server IP address '%s' : %s",
                    (uintptr_t)sockinfo->ipaddress,
                    (uintptr_t)threadname);
    free(userinfo);
    osThreadTerminate(osThreadGetId());
    return;
  }
  free(userinfo);
  userinfo = NULL;

  if (socket_type == SOCKET_TYPE_ASYNC) {
    server_address.sin_family = AF_INET;
    client_socket             = sl_si91x_socket_async(AF_INET, SOCK_STREAM, IPPROTO_TCP, &data_callback);
    if (client_socket < 0) {
      SL_DEBUG_LOG_V2(ERROR, "Socket creation failed with bsd error: %d : %s\r\n", errno, (uintptr_t)threadname);
      osThreadTerminate(osThreadGetId());
      return;
    }
    SL_DEBUG_LOG_V2(INFO, "Socket ID : %d : %s\r\n", client_socket, (uintptr_t)threadname);

    socket_return_value = setsockopt(client_socket, SOL_TCP, TCP_ULP, TLS, sizeof(TLS));
    if (socket_return_value < 0) {
      SL_DEBUG_LOG_V2(ERROR, "Set Socket option failed with bsd error: %d : %s\r\n", errno, (uintptr_t)threadname);
      close_and_log(client_socket, "Client", threadname);
      osThreadTerminate(osThreadGetId());
      return;
    }

    socket_return_value = sl_si91x_setsockopt(client_socket,
                                              SOL_SOCKET,
                                              SL_SI91X_SO_HIGH_PERFORMANCE_SOCKET,
                                              &high_performance_socket,
                                              sizeof(high_performance_socket));
    if (socket_return_value < 0) {
      SL_DEBUG_LOG_V2(ERROR, "Set Socket option failed with bsd error: %d : %s\r\n", errno, (uintptr_t)threadname);
      close_and_log(client_socket, "Client", threadname);
      osThreadTerminate(osThreadGetId());
      return;
    }

    socket_return_value = connect(client_socket, (struct sockaddr *)&server_address, socket_length);
    if (socket_return_value < 0) {
      SL_DEBUG_LOG_V2(ERROR, "Socket Connect failed with bsd error: %d : %s\r\n", errno, (uintptr_t)threadname);
      close_and_log(client_socket, "Client", threadname);
      osThreadTerminate(osThreadGetId());
      return;
    }
    SL_DEBUG_LOG_V2(INFO, "Socket connected to TLS server : %s\r\n", (uintptr_t)threadname);

    if (client_socket < 0 || client_socket >= MAX_SERVER_CLIENT_SOCKETS) {
      SL_DEBUG_LOG_V2(ERROR, "Invalid client socket index %d : %s", client_socket, (uintptr_t)threadname);
      close_and_log(client_socket, "Client", threadname);
      osThreadTerminate(osThreadGetId());
      return;
    }
    while (!sock_data[client_socket].is_data_received) {
      osThreadYield();
    }
  } else {
    uint32_t total_bytes_received = 0;
    uint32_t start                = 0;
    uint32_t now                  = 0;
    uint8_t data_buffer[TLS_BUFFER_SIZE];

    server_address.sin_family = AF_INET;
    client_socket             = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket < 0) {
      SL_DEBUG_LOG_V2(ERROR, "Socket creation failed with bsd error: %d : %s\r\n", errno, (uintptr_t)threadname);
      osThreadTerminate(osThreadGetId());
      return;
    }
    SL_DEBUG_LOG_V2(INFO, "Socket ID : %d : %s\r\n", client_socket, (uintptr_t)threadname);

    socket_return_value = setsockopt(client_socket, SOL_TCP, TCP_ULP, TLS, sizeof(TLS));
    if (socket_return_value < 0) {
      SL_DEBUG_LOG_V2(ERROR, "Set Socket option failed with bsd error: %d : %s\r\n", errno, (uintptr_t)threadname);
      close_and_log(client_socket, "Client", threadname);
      osThreadTerminate(osThreadGetId());
      return;
    }

    socket_return_value = setsockopt(client_socket,
                                     SOL_SOCKET,
                                     SL_SO_HIGH_PERFORMANCE_SOCKET,
                                     &high_performance_socket,
                                     sizeof(high_performance_socket));
    if (socket_return_value < 0) {
      SL_DEBUG_LOG_V2(ERROR, "Set Socket option failed with bsd error: %d : %s\r\n", errno, (uintptr_t)threadname);
      close_and_log(client_socket, "Client", threadname);
      osThreadTerminate(osThreadGetId());
      return;
    }

    socket_return_value = connect(client_socket, (struct sockaddr *)&server_address, socket_length);
    if (socket_return_value < 0) {
      SL_DEBUG_LOG_V2(ERROR, "Socket Connect failed with bsd error: %d : %s\r\n", errno, (uintptr_t)threadname);
      close_and_log(client_socket, "Client", threadname);
      osThreadTerminate(osThreadGetId());
      return;
    }
    SL_DEBUG_LOG_V2(INFO, "Socket connected to TLS server : %s\r\n", (uintptr_t)threadname);

    SL_DEBUG_LOG_V2(INFO,
                    "TLS_RX Throughput test start, Client Socket ID %d : %s",
                    client_socket,
                    (uintptr_t)threadname);
    start          = osKernelGetTickCount();
    now            = start;
    int read_bytes = 1;
    while (1) {
      read_bytes = recv(client_socket, data_buffer, sizeof(data_buffer), 0);
      if (read_bytes < 0) {
        if (errno == 0) {
          sl_status_t fw_status = sl_wifi_get_saved_firmware_status();
          if (fw_status == SL_STATUS_SI91X_MEMORY_FAILED_FROM_MODULE) {
            continue;
          } else {
            SL_DEBUG_LOG_V2(ERROR,
                            "SSL recv failed with BSD error = %d and status = 0x%lx : %s",
                            errno,
                            fw_status,
                            (uintptr_t)threadname);
          }
        } else if (errno == ENOTCONN) {
          SL_DEBUG_LOG_V2(WARN,
                          "Remote server terminated, Client Socket ID %d : %s",
                          client_socket,
                          (uintptr_t)threadname);
        } else {
          SL_DEBUG_LOG_V2(ERROR, "SSL recv failed with BSD error = %d : %s", errno, (uintptr_t)threadname);
        }
        break;
      }
      total_bytes_received = total_bytes_received + read_bytes;
      now                  = osKernelGetTickCount();
      if ((now - start) > TEST_TIMEOUT) {
        SL_DEBUG_LOG_V2(INFO, "Test Time Out: %ld ms : %s", (now - start), (uintptr_t)threadname);
        break;
      }
    }
    SL_DEBUG_LOG_V2(INFO, "TLS_RX Throughput test finished : %s", (uintptr_t)threadname);
    SL_DEBUG_LOG_V2(INFO, "Total bytes received : %ld : %s", total_bytes_received, (uintptr_t)threadname);
    measure_and_print_throughput(total_bytes_received, (now - start), client_socket);
  }

  close_and_log(client_socket, "Client", threadname);
  osThreadTerminate(osThreadGetId());
}

/******************************************************
 *           TLS TX - send_data_to_tls_server
 ******************************************************/
void send_data_to_tls_server(void *userinfo)
{
  int client_socket                 = -1;
  int socket_return_value           = 0;
  uint32_t total_bytes_sent         = 0;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);
  sockinfo_t *sockinfo              = (sockinfo_t *)userinfo;
  uint32_t start = 0, now = 0;
  uint8_t data_buffer[TLS_BUFFER_SIZE];
  const char *threadname = osThreadGetName(osThreadGetId());

  for (size_t i = 0; i < sizeof(data_buffer); i++)
    data_buffer[i] = 'F';

  server_address.sin_family = AF_INET;
  server_address.sin_port   = sockinfo->port_num;

  sl_status_t addr_status = sl_net_inet_addr(sockinfo->ipaddress, &server_address.sin_addr.s_addr);
  if (addr_status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR,
                    "Invalid server IP address '%s' : %s",
                    (uintptr_t)sockinfo->ipaddress,
                    (uintptr_t)threadname);
    free(userinfo);
    osThreadTerminate(osThreadGetId());
    return;
  }
  free(userinfo);
  userinfo = NULL;

  client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket creation failed with bsd error: %d : %s\r\n", errno, (uintptr_t)threadname);
    osThreadTerminate(osThreadGetId());
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Socket ID : %d : %s\r\n", client_socket, (uintptr_t)threadname);

  socket_return_value = setsockopt(client_socket, SOL_TCP, TCP_ULP, TLS, sizeof(TLS));
  if (socket_return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Set Socket option failed with bsd error: %d : %s\r\n", errno, (uintptr_t)threadname);
    close_and_log(client_socket, "Client", threadname);
    osThreadTerminate(osThreadGetId());
    return;
  }

  socket_return_value = connect(client_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket Connect failed with bsd error: %d : %s\r\n", errno, (uintptr_t)threadname);
    close_and_log(client_socket, "Client", threadname);
    osThreadTerminate(osThreadGetId());
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Socket connected to TLS server : %s\r\n", (uintptr_t)threadname);

  SL_DEBUG_LOG_V2(INFO, "TLS_TX Throughput test start, Client Socket ID %d : %s", client_socket, (uintptr_t)threadname);
  start          = osKernelGetTickCount();
  now            = start;
  int sent_bytes = 0;
  while (total_bytes_sent < BYTES_TO_SEND) {
    sent_bytes = send(client_socket, data_buffer, TLS_BUFFER_SIZE, 0);
    if (sent_bytes < 0) {
      if (errno == ENOBUFS)
        continue;
      SL_DEBUG_LOG_V2(ERROR, "SSL send failed with BSD error : %d : %s", errno, (uintptr_t)threadname);
      break;
    }
    total_bytes_sent = total_bytes_sent + sent_bytes;
    now              = osKernelGetTickCount();
    if ((now - start) >= TEST_TIMEOUT) {
      SL_DEBUG_LOG_V2(INFO, "Time Out: %ld : %s\r\n", (now - start), (uintptr_t)threadname);
      break;
    }
  }
  SL_DEBUG_LOG_V2(INFO, "TLS_TX Throughput test finished : %s\r\n", (uintptr_t)threadname);
  SL_DEBUG_LOG_V2(INFO, "Total bytes sent : %ld : %s\r\n", total_bytes_sent, (uintptr_t)threadname);
  measure_and_print_throughput(total_bytes_sent, (now - start), client_socket);

  close_and_log(client_socket, "Client", threadname);
  osThreadTerminate(osThreadGetId());
}
