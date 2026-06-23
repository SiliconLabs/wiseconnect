/***************************************************************************/ /**
 * @file
 * @brief Powersave Standby Associated with TCP Client Example Application
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

#include "sl_status.h"
#include "sl_board_configuration.h"
#include "cmsis_os2.h"
#include "errno.h"
#include "sl_wifi.h"
#include "sl_net.h"
#include "sl_net_wifi_types.h"
#include "socket.h"
#include "sl_utility.h"
#include "sl_net_si91x.h"
#include "sl_wifi_callback_framework.h"
#include "sl_si91x_driver.h"
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#ifdef SLI_SI91X_MCU_INTERFACE
#include "sl_si91x_power_manager.h"
#include "sl_si91x_m4_ps.h"
#endif

/******************************************************
 *                      Macros
 ******************************************************/
#define SERVER_IP_ADDRESS   "2401:4901:1221:10eb:ddf3:2995:542c:3f9a"
#define DATA                "HellofromTCPclient!!!"
#define SERVER_PORT         5001
#define NUMBER_OF_PACKETS   1000
#define TCP_KEEP_ALIVE_TIME 240
#define TCP_BUFFER_SIZE     1460
#define SEND_TCP_DATA       0

/* Defaults migrated from sl_wifi_filter_broadcast(5000, 1, 1). */
#define BCAST_FILTER_ENABLE      (1U)
#define MCAST_FILTER_ENABLE      (1U)
#define FILTER_MODE              (0U)
#define BEACON_DROP_THRESHOLD_MS (5000U)

/******************************************************
 *                    Constants
 ******************************************************/
static const sl_wifi_device_configuration_t station_init_configuration = {
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
                   .tcp_ip_feature_bit_map = (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT
#ifdef SLI_SI91X_ENABLE_IPV6
                                              | SL_SI91X_TCP_IP_FEAT_DHCPV6_CLIENT | SL_SI91X_TCP_IP_FEAT_IPV6
#endif
                                              | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID),
                   .custom_feature_bit_map     = (SL_WIFI_SYSTEM_CUSTOM_FEAT_EXTENSION_VALID),
                   .ext_custom_feature_bit_map = (SL_WIFI_SYSTEM_EXT_FEAT_LOW_POWER_MODE | SL_SI91X_EXT_FEAT_XTAL_CLK
                                                  | SL_SI91X_EXT_FEAT_UART_SEL_FOR_DEBUG_PRINTS | MEMORY_CONFIG
#ifdef SLI_SI917
                                                  | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
                                                  ),
                   .bt_feature_bit_map         = 0,
                   .ext_tcp_ip_feature_bit_map = SL_SI91X_CONFIG_FEAT_EXTENSION_VALID,
                   .ble_feature_bit_map        = 0,
                   .ble_ext_feature_bit_map    = 0,
                   .config_feature_bit_map = (SL_SI91X_FEAT_SLEEP_GPIO_SEL_BITMAP | SL_WIFI_ENABLE_ENHANCED_MAX_PSP) }
};

/******************************************************
 *               Function Declarations
 ******************************************************/
static void application_start(void *argument);
sl_status_t send_data_to_tcp_server(void);

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
};

/******************************************************
 *               Function Definitions
 ******************************************************/

void app_init(void)
{
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

static void application_start(void *argument)
{
  UNUSED_PARAMETER(argument);
  sl_status_t status;
  sl_wifi_performance_profile_v2_t performance_profile = { .profile = ASSOCIATED_POWER_SAVE_LOW_LATENCY };
  sl_wifi_firmware_version_t version                   = { 0 };
  sl_mac_address_t mac_addr                            = { 0 };

  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &station_init_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, " Failed to start Wi-Fi Client interface: 0x%" PRIx32, (uint32_t)status);
    return;
  }
  status = sl_wifi_get_mac_address(SL_WIFI_CLIENT_INTERFACE, &mac_addr);
  if (status == SL_STATUS_OK) {
    char ps_tcp_mac_log[80];
    snprintf(ps_tcp_mac_log,
             sizeof(ps_tcp_mac_log),
             " Device MAC address: %x:%x:%x:%x:%x:%x",
             mac_addr.octet[0],
             mac_addr.octet[1],
             mac_addr.octet[2],
             mac_addr.octet[3],
             mac_addr.octet[4],
             mac_addr.octet[5]);
    SL_DEBUG_LOG_V2(INFO, "%s", (uintptr_t)ps_tcp_mac_log);
  } else {
    SL_DEBUG_LOG_V2(ERROR, " Failed to get mac address: 0x%" PRIx32, (uint32_t)status);
  }
  status = sl_wifi_get_firmware_version(&version);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, " Failed to fetch firmware version: 0x%" PRIx32, (uint32_t)status);
  } else {
    print_firmware_version(&version);
  }

  sl_wifi_set_join_configuration(SL_WIFI_CLIENT_INTERFACE, 0);
  sl_wifi_listen_interval_v2_t profile2 = { 0 };
  status                                = sl_wifi_set_listen_interval_v2(SL_WIFI_CLIENT_INTERFACE, profile2);

  status = sl_net_up(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, " Failed to bring Wi-Fi client interface up: 0x%" PRIx32, (uint32_t)status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, " Wi-Fi client connected\r\n");

  sl_wifi_groupcast_filter_config_t groupcast_filter_config = { 0 };
  groupcast_filter_config.enable_bcast_filter               = (uint8_t)BCAST_FILTER_ENABLE;
  groupcast_filter_config.enable_mcast_filter               = (uint8_t)MCAST_FILTER_ENABLE;
  groupcast_filter_config.filter_mode                       = (uint8_t)FILTER_MODE;

  status = sl_wifi_set_groupcast_filter_config(&groupcast_filter_config);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, " sl_wifi_set_groupcast_filter_config failed, Error Code : 0x%" PRIx32, (uint32_t)status);
    return;
  }

  status = sl_wifi_set_beacon_drop_threshold(SL_WIFI_CLIENT_INTERFACE, (uint16_t)BEACON_DROP_THRESHOLD_MS);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, " sl_wifi_set_beacon_drop_threshold failed, Error Code : 0x%" PRIx32, (uint32_t)status);
    return;
  }

  // set performance profile
  status = sl_wifi_set_performance_profile_v2(&performance_profile);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, " Power save configuration Failed, Error Code : 0x%" PRIx32, (uint32_t)status);
    return;
  }

  status = send_data_to_tcp_server();
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, " Send data failed with status 0x%" PRIx32, (uint32_t)status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, " Send data completed successfully 0x%" PRIx32, (uint32_t)status);
  SL_DEBUG_LOG_V2(INFO, " Example Demonstration Completed");

#ifdef SLI_SI91X_MCU_INTERFACE
  osSemaphoreId_t wait_semaphore;
  wait_semaphore = osSemaphoreNew(1, 0, NULL);
  if (wait_semaphore == NULL) {
    SL_DEBUG_LOG_V2(ERROR, " Failed to create semaphore");
    return;
  }
  // Waiting forever using semaphore to put M4 to sleep in tick less mode
  osSemaphoreAcquire(wait_semaphore, osWaitForever);
#endif
}

sl_status_t send_data_to_tcp_server(void)
{
  sl_status_t status                   = SL_STATUS_OK;
  int client_socket                    = -1;
  int return_value                     = 0;
  sl_ip_address_t ip_address           = { 0 };
  sl_net_wifi_client_profile_t profile = { 0 };
  uint16_t tcp_keep_alive              = TCP_KEEP_ALIVE_TIME;

  status = sl_net_get_profile(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID, &profile);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, " Failed to get client profile: 0x%" PRIx32, (uint32_t)status);
    return status;
  }
  SL_DEBUG_LOG_V2(INFO, " Client profile is fetched successfully\r\n");

  if (profile.ip.type == SL_IPV4) { /*IPv4*/
    ip_address.type = SL_IPV4;
    memcpy(&ip_address.ip.v4.bytes, &profile.ip.ip.v4.ip_address.bytes, sizeof(sl_ipv4_address_t));
    print_sl_ip_address(&ip_address);

    struct sockaddr_in server_address = { 0 };
    status                            = sl_net_inet_addr(SERVER_IP_ADDRESS, (uint32_t *)&ip_address);
    if (status != SL_STATUS_OK) {
      SL_DEBUG_LOG_V2(ERROR, " IPv4 conversion failed.\r\n");
      return SL_STATUS_FAIL;
    }

    server_address.sin_family      = AF_INET;
    server_address.sin_port        = SERVER_PORT;
    server_address.sin_addr.s_addr = ip_address.ip.v4.value;

    //!Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SL_DEBUG_LOG_V2(INFO, " Client Socket: %d\r\n", client_socket);
    if (client_socket < 0) {
      SL_DEBUG_LOG_V2(ERROR, " Socket Create failed with bsd error: %d\r\n", errno);
      return SL_STATUS_FAIL;
    }
    //!Keep Alive
    return_value = setsockopt(client_socket, SOL_SOCKET, SO_KEEPALIVE, &tcp_keep_alive, sizeof(tcp_keep_alive));
    SL_DEBUG_LOG_V2(INFO, " Client Socket: %d\r\n", client_socket);
    if (return_value < 0) {
      SL_DEBUG_LOG_V2(ERROR, " TCP Keep Alive configuration failed: %d\r\n", errno);
      return SL_STATUS_FAIL;
    }
    //! Socket connect
    return_value = connect(client_socket, (struct sockaddr *)&server_address, sizeof(struct sockaddr_in));
    if (return_value < 0) {
      SL_DEBUG_LOG_V2(ERROR, " Socket Connect failed with bsd error: %d\r\n", errno);
      close(client_socket);
      return SL_STATUS_FAIL;
    }
    SL_DEBUG_LOG_V2(INFO, " Socket Connected\r\n");

  } else if (profile.ip.type == SL_IPV6) { /*IPv6*/
    sl_ip_address_t link_local_address = { 0 };
    memcpy(&link_local_address.ip.v6, &profile.ip.ip.v6.link_local_address, SL_IPV6_ADDRESS_LENGTH);
    link_local_address.type = SL_IPV6;
    SL_DEBUG_LOG_V2(INFO, " Link Local Address: ");
    print_sl_ip_address(&link_local_address);

    sl_ip_address_t global_address = { 0 };
    memcpy(&global_address.ip.v6, &profile.ip.ip.v6.global_address, SL_IPV6_ADDRESS_LENGTH);
    global_address.type = SL_IPV6;
    SL_DEBUG_LOG_V2(INFO, " Global Address: ");
    print_sl_ip_address(&global_address);

    sl_ip_address_t gateway = { 0 };
    memcpy(&gateway.ip.v6, &profile.ip.ip.v6.gateway, SL_IPV6_ADDRESS_LENGTH);
    gateway.type = SL_IPV6;
    SL_DEBUG_LOG_V2(INFO, " Gateway Address: ");
    print_sl_ip_address(&gateway);

    struct sockaddr_in6 server_address6 = { 0 };
    uint8_t address_buffer[SL_IPV6_ADDRESS_LENGTH];

    status = sl_inet_pton6(SERVER_IP_ADDRESS,
                           &SERVER_IP_ADDRESS[strlen(SERVER_IP_ADDRESS)],
                           address_buffer,
                           (unsigned int *)server_address6.sin6_addr.__u6_addr.__u6_addr32);
    if (status != 0x1) {
      SL_DEBUG_LOG_V2(ERROR, " IPv6 conversion failed.\r\n");
      return SL_STATUS_FAIL;
    }

    server_address6.sin6_family = AF_INET6;
    server_address6.sin6_port   = SERVER_PORT;

    //!Create socket
    client_socket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket < 0) {
      SL_DEBUG_LOG_V2(ERROR, " Socket Create failed with bsd error: %d\r\n", errno);
      return SL_STATUS_FAIL;
    }
    //!Keep alive
    return_value = setsockopt(client_socket, SOL_SOCKET, SO_KEEPALIVE, &tcp_keep_alive, sizeof(tcp_keep_alive));
    SL_DEBUG_LOG_V2(INFO, " Client Socket: %d\r\n", client_socket);
    if (return_value < 0) {
      SL_DEBUG_LOG_V2(ERROR, " TCP KA configuration failed: %d\r\n", errno);
      return SL_STATUS_FAIL;
    }
    //! Socket connect
    return_value = connect(client_socket, (struct sockaddr *)&server_address6, sizeof(struct sockaddr_in6));
    if (return_value < 0) {
      SL_DEBUG_LOG_V2(ERROR, " Socket Connect failed with bsd error: %d\r\n", errno);
      close(client_socket);
      return SL_STATUS_FAIL;
    }
    SL_DEBUG_LOG_V2(INFO, " Socket Connected\r\n");
  }

  //! send data
#if SEND_TCP_DATA
  int sent_bytes   = 0;
  int packet_count = 0;
  while (packet_count < NUMBER_OF_PACKETS) {
    sent_bytes = send(client_socket, DATA, strlen(DATA), 0);
    if (sent_bytes < 0) {
      if (errno == ENOBUFS)
        continue;
      PRINT_STATUS(ERROR_TAG, SL_STATUS_FAIL);
      close(client_socket);
      return SL_STATUS_FAIL;
    }
    packet_count++;
  }
  SL_DEBUG_LOG_V2(INFO, " Data sent successfully\r\n");
#endif

  return SL_STATUS_OK;
}
