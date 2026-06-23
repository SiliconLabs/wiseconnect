/***************************************************************************/ /**
 * @file
 * @brief Powersave Standby Associated Example Application
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

#include "sl_status.h"
#include "sl_board_configuration.h"
#include "cmsis_os2.h"
#include "errno.h"
#include "sl_wifi.h"
#include "sl_net.h"
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
#define SERVER_IP_ADDRESS    "192.168.50.40"
#define DATA                 "HellofromUDPclient!!!"
#define SERVER_PORT          5001
#define NUMBER_OF_PACKETS    1000
#define ENABLE_DATA_TRANSFER 0

/* Defaults migrated from sl_wifi_filter_broadcast(5000, 1, 1). */
#define BCAST_FILTER_ENABLE      (1U)
#define MCAST_FILTER_ENABLE      (1U)
#define FILTER_MODE              (0U)
#define BEACON_DROP_THRESHOLD_MS (5000U)

/******************************************************
 *                    Constants
 ******************************************************/
#ifndef SL_SI91X_WC_DEVICE_CONFIGURATION
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
                   .tcp_ip_feature_bit_map =
                     (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID),
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
#else
#include "sl_si91x_wc_device_configuration.h"

sl_mac_address_t mac = { .octet = SL_SI91X_WC_MAC_ADDRESS };
static const sl_wifi_device_configuration_t station_init_configuration = {
  .boot_option = SL_SI91X_WC_BOOT_OPTION,
#if SL_SI91X_WC_SET_MAC_ADDRESS
  .mac_address = &mac,
#else
  .mac_address = NULL,
#endif
  .band        = SL_SI91X_WC_WIFI_BAND,
  .region_code = SL_SI91X_WC_REGION,
  .boot_config = {
                .oper_mode                  = SL_SI91X_WC_OPERMODE,
                .coex_mode                  = SL_SI91X_WC_COEXMODE,
                .feature_bit_map            = SL_SI91X_WC_FEATURE_BITMAP,
                .tcp_ip_feature_bit_map     = SL_SI91X_WC_TCP_IP_FEATURE_BITMAP,
                .custom_feature_bit_map     = SL_SI91X_WC_CUSTOM_FEATURE_BITMAP,
                .ext_custom_feature_bit_map = SL_SI91X_WC_EXTENDED_CUSTOM_FEATURE_BITMAP,
                .bt_feature_bit_map         = SL_SI91X_WC_BT_FEATURE_BITMAP,
                .ext_tcp_ip_feature_bit_map = SL_SI91X_WC_EXTENDED_TCPIP_FEATURE_BITMAP,
                .ble_feature_bit_map        = SL_SI91X_WC_BLE_FEATURE_BITMAP,
                .ble_ext_feature_bit_map    = SL_SI91X_WC_EXTENDED_BLE_CUSTOM_FEATURE_BITMAP,
                .config_feature_bit_map     = SL_SI91X_WC_CONFIG_FEATURE_BITMAP,
              },
  .ta_pool = { .tx_ratio_in_buffer_pool     = SL_SI91X_WC_TX_POOL_RATIO,
                .rx_ratio_in_buffer_pool     = SL_SI91X_WC_RX_POOL_RATIO,
                .global_ratio_in_buffer_pool = SL_SI91X_WC_GLOBAL_POOL_RATIO },
  .efuse_data_type = SL_SI91X_WC_EFUSE_DATA_TYPE,
  .nwp_fw_image_number = SL_SI91X_WC_NWP_FW_IMAGE_NUMBER
};
#endif

/******************************************************
 *               Function Declarations
 ******************************************************/
static void application_start(void *argument);
sl_status_t send_data(void);

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
    SL_DEBUG_LOG_V2(ERROR, "Failed to start Wi-Fi Client interface: 0x%" PRIx32, (uint32_t)status);
    return;
  }
  status = sl_wifi_get_mac_address(SL_WIFI_CLIENT_INTERFACE, &mac_addr);
  if (status == SL_STATUS_OK) {
    char ps_assoc_mac_log[80];
    snprintf(ps_assoc_mac_log,
             sizeof(ps_assoc_mac_log),
             "Device MAC address: %x:%x:%x:%x:%x:%x",
             mac_addr.octet[0],
             mac_addr.octet[1],
             mac_addr.octet[2],
             mac_addr.octet[3],
             mac_addr.octet[4],
             mac_addr.octet[5]);
    SL_DEBUG_LOG_V2(INFO, "%s", (uintptr_t)ps_assoc_mac_log);
  } else {
    SL_DEBUG_LOG_V2(ERROR, "Failed to get mac address: 0x%" PRIx32, (uint32_t)status);
  }
  status = sl_wifi_get_firmware_version(&version);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to fetch firmware version: 0x%" PRIx32, (uint32_t)status);
  } else {
    print_firmware_version(&version);
  }

  status = sl_net_up(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to bring Wi-Fi client interface up: 0x%" PRIx32, (uint32_t)status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Wi-Fi client connected\r\n");

  sl_wifi_groupcast_filter_config_t groupcast_filter_config = { 0 };
  groupcast_filter_config.enable_bcast_filter               = (uint8_t)BCAST_FILTER_ENABLE;
  groupcast_filter_config.enable_mcast_filter               = (uint8_t)MCAST_FILTER_ENABLE;
  groupcast_filter_config.filter_mode                       = (uint8_t)FILTER_MODE;

  status = sl_wifi_set_groupcast_filter_config(&groupcast_filter_config);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "sl_wifi_set_groupcast_filter_config failed, Error Code : 0x%" PRIx32, (uint32_t)status);
    return;
  }

  status = sl_wifi_set_beacon_drop_threshold(SL_WIFI_CLIENT_INTERFACE, (uint16_t)BEACON_DROP_THRESHOLD_MS);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "sl_wifi_set_beacon_drop_threshold failed, Error Code : 0x%" PRIx32, (uint32_t)status);
    return;
  }

  // set performance profile
  status = sl_wifi_set_performance_profile_v2(&performance_profile);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Power save configuration Failed, Error Code : 0x%" PRIx32, (uint32_t)status);
    return;
  }

#if ENABLE_DATA_TRANSFER
  status = send_data();
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Send data failed with status 0x%" PRIx32, (uint32_t)status);
    return;
  }
#endif
  SL_DEBUG_LOG_V2(INFO, "Example Demonstration Completed");

#ifdef SLI_SI91X_MCU_INTERFACE
  osSemaphoreId_t wait_semaphore;
  wait_semaphore = osSemaphoreNew(1, 0, NULL);
  if (wait_semaphore == NULL) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to create semaphore");
    return;
  }
  // Waiting forever using semaphore to put M4 to sleep in tick less mode
  osSemaphoreAcquire(wait_semaphore, osWaitForever);
#endif
}

sl_status_t send_data(void)
{
  int32_t bytes_sent          = -1;
  int32_t total_num_of_bytes  = 0;
  int32_t packet_count        = 0;
  struct sockaddr_in address  = { 0 };
  sl_ipv4_address_t server_ip = { 0 };

  //create UDP socket
  int32_t socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (socket_fd < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket Create failed with bsd error: %d\r\n", errno);
    return SL_STATUS_FAIL;
  }
  SL_DEBUG_LOG_V2(INFO, "UDP Client Socket Creation Success\r\n");

  sl_net_inet_addr((char *)SERVER_IP_ADDRESS, (uint32_t *)&server_ip);

  //send data to server
  address.sin_port        = SERVER_PORT;
  address.sin_addr.s_addr = server_ip.value;

  while (packet_count < NUMBER_OF_PACKETS) {
    bytes_sent =
      sendto(socket_fd, (uint8_t *)DATA, (sizeof(DATA) - 1), 0, (const struct sockaddr *)&address, sizeof(address));
    if (bytes_sent < 0) {
      if (errno == ENOBUFS)
        continue;
      SL_DEBUG_LOG_V2(ERROR, "Send failed with bsd error: %d\r\n", errno);
      close(socket_fd);
      return SL_STATUS_FAIL;
    }
    total_num_of_bytes += bytes_sent;
    packet_count++;
  }

  SL_DEBUG_LOG_V2(INFO, "Total number of bytes sent: %ld\r\n", total_num_of_bytes);
  close(socket_fd);

  return SL_STATUS_OK;
}