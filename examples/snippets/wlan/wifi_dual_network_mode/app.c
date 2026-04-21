/***************************************************************************/ /**
 * @file
 * @brief Station Ping Example Application for Dual Network Stack Mode
 * 
 *        Ping using offloaded stack and send ICMP packets using raw data frame API
 *        and receive the packets intended for external stack
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
#include "cmsis_os2.h"
#include "sl_net.h"
#include "sl_board_configuration.h"
#include "sl_net_ping.h"
#include "sl_utility.h"
#include "sl_si91x_driver.h"
#include "sl_wifi.h"
#include "sl_net_wifi_types.h"
#include "sl_net_default_values.h"
#include <string.h>

/******************************************************
 *                    Constants
 ******************************************************/
#define REMOTE_IP_ADDRESS "8.8.8.8"
#define PING_PACKET_SIZE  64

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

const osThreadAttr_t net_thread_attributes = {
  .name       = "net_thread",
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = 2048,
  .priority   = osPriorityLow,
  .tz_module  = 0,
  .reserved   = 0,
};

static const sl_wifi_device_configuration_t client_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .region_code = US,
  .boot_config = { .oper_mode = SL_SI91X_CLIENT_MODE,
                   .coex_mode = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map =
                     (SL_SI91X_FEAT_SECURITY_OPEN | SL_SI91X_FEAT_AGGREGATION | SL_SI91X_FEAT_ULP_GPIO_BASED_HANDSHAKE
#ifdef SLI_SI91X_MCU_INTERFACE
                      | SL_SI91X_FEAT_WPS_DISABLE
#endif
                      ),
                   .tcp_ip_feature_bit_map     = (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT | SL_SI91X_TCP_IP_FEAT_DNS_CLIENT
                                              | SL_SI91X_TCP_IP_FEAT_ICMP | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID),
                   .custom_feature_bit_map     = SL_SI91X_CUSTOM_FEAT_EXTENTION_VALID,
                   .ext_custom_feature_bit_map = (SL_SI91X_EXT_FEAT_LOW_POWER_MODE | SL_SI91X_EXT_FEAT_XTAL_CLK
                                                  | SL_SI91X_EXT_FEAT_UART_SEL_FOR_DEBUG_PRINTS | MEMORY_CONFIG
#if defined(SLI_SI917) || defined(SLI_SI915)
                                                  | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
                                                  ),
                   .bt_feature_bit_map = 0,
                   .ext_tcp_ip_feature_bit_map =
                     (SL_SI91X_EXT_TCP_IP_DUAL_MODE_ENABLE | SL_SI91X_CONFIG_FEAT_EXTENTION_VALID),
                   .ble_feature_bit_map     = 0,
                   .ble_ext_feature_bit_map = 0,
                   .config_feature_bit_map  = SL_SI91X_FEAT_SLEEP_GPIO_SEL_BITMAP }
};

/******************************************************
 *               Function Declarations
 ******************************************************/
static void application_start(void *argument);
static sl_status_t network_event_handler(sl_net_event_t event, sl_status_t status, void *data, uint32_t data_length);

/******************************************************
 *               Function Definitions
 ******************************************************/
static sl_status_t network_event_handler(sl_net_event_t event, sl_status_t status, void *data, uint32_t data_length)
{
  UNUSED_PARAMETER(data_length);
  switch (event) {
    case SL_NET_PING_RESPONSE_EVENT: {
      sl_si91x_ping_response_t *response = (sl_si91x_ping_response_t *)data;
      if (status != SL_STATUS_OK) {
        printf("\r\nPing request failed!\r\n");
        return status;
      }
      printf("\r\n%u bytes received from %u.%u.%u.%u\r\n",
             response->ping_size,
             response->ping_address.ipv4_address[0],
             response->ping_address.ipv4_address[1],
             response->ping_address.ipv4_address[2],
             response->ping_address.ipv4_address[3]);
      break;
    }
    case SL_NET_DHCP_NOTIFICATION_EVENT: {
      printf("\r\nReceived DHCP Notification event with status : 0x%lX\r\n", status);
      break;
    }
    case SL_NET_IP_ADDRESS_CHANGE_EVENT: {
      sl_net_ip_configuration_t *ip_config = (sl_net_ip_configuration_t *)data;
      printf("\r\nReceived Ip Address Change Notification event with status : 0x%lX\r\n", status);
      printf("\t Ip Address : %u.%u.%u.%u\r\n",
             ip_config->ip.v4.ip_address.bytes[0],
             ip_config->ip.v4.ip_address.bytes[1],
             ip_config->ip.v4.ip_address.bytes[2],
             ip_config->ip.v4.ip_address.bytes[3]);
      printf("\t Netmask : %u.%u.%u.%u\r\n",
             ip_config->ip.v4.netmask.bytes[0],
             ip_config->ip.v4.netmask.bytes[1],
             ip_config->ip.v4.netmask.bytes[2],
             ip_config->ip.v4.netmask.bytes[3]);
      printf("\t Gateway : %u.%u.%u.%u\r\n",
             ip_config->ip.v4.gateway.bytes[0],
             ip_config->ip.v4.gateway.bytes[1],
             ip_config->ip.v4.gateway.bytes[2],
             ip_config->ip.v4.gateway.bytes[3]);
      break;
    }
    default:
      break;
  }

  return SL_STATUS_OK;
}

// Send PING packet (External host network packet) */
static void send_ping_raw_data_frame(void)
{
  /* PING packet */
  uint8_t buffer[] = {
    0x54, 0xe1, 0xad, 0x30, 0x77, 0x61, // Destination MAC
    0x08, 0xdd, 0xeb, 0x1a, 0xc1, 0xc8, // Source MAC
    0x08, 0x00,                         // IP Type
    0x45, 0x00,                         // Version and header length
    0x00, 0x54,                         // Total length
    0x3c, 0x1b,                         // Identification
    0x40, 0x00,                         // Flags, Fragment offset
    0x40,                               // Time to live
    0x01,                               // Protocol (ICMP)
    0x7b, 0x0e,                         // Header checksum
    0xc0, 0xa8, 0x01, 0x17,             // Source IP address
    0xc0, 0xa8, 0x01, 0x18,             // Destination IP address
    0x08, 0x00,                         // Ping type (request)
    0xbc, 0x58,                         // Checksum
    0x00, 0x0c,                         //  Identifier be
    0x00, 0x07,                         // Identifier le
    0xed, 0xfb, 0x8d, 0x67, 0x00, 0x00, 0x00, 0x00, 0xf6, 0x5d, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x11, 0x12,
    0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
    0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, // Data
    0xCC, 0x95, 0x09, 0xDD                                                                                      // FCS
  };

  while (1) {
    printf("\r\nSend raw data frame (ping packet) \r\n");
    sl_wifi_send_raw_data_frame(SL_WIFI_CLIENT_INTERFACE, (uint8_t *)buffer, sizeof(buffer));
    osDelay(2000);
  }
}

// Handle the RX packet to forward it to the external network stack
sl_status_t sl_si91x_host_process_data_frame(sl_wifi_interface_t interface, sl_wifi_buffer_t *buffer)
{
  UNUSED_PARAMETER(interface);
  void *packet;
  sl_si91x_packet_t *rx_pkt;

  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  rx_pkt = (sl_si91x_packet_t *)packet;
  printf("\r\nReceived raw data frame, frame length : %d\r\n", rx_pkt->length);

  // Forward the received data packet to external network stack
  // ...

  return SL_STATUS_OK;
}

void app_init(const void *unused)
{
  UNUSED_PARAMETER(unused);
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

static void application_start(void *argument)
{
  UNUSED_PARAMETER(argument);
  sl_status_t status;

  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &client_configuration, NULL, network_event_handler);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to start Wi-Fi Client interface: 0x%lX\r\n", status);
    return;
  }
  printf("\r\nWi-Fi client interface up success\r\n");

  status = sl_net_up(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to bring Wi-Fi client interface up: 0x%lX\r\n", status);
    return;
  }
  printf("\r\nWi-Fi client connected\r\n");

  sl_ip_address_t remote_ip_address = { 0 };
  sl_net_inet_addr(REMOTE_IP_ADDRESS, (uint32_t *)&remote_ip_address.ip.v4);
  remote_ip_address.type = SL_IPV4;

  // Thread to handle TX data functionality to send TX packet from external network stack
  osThreadNew((osThreadFunc_t)send_ping_raw_data_frame, NULL, &net_thread_attributes);

  while (1) {
    // Send ping packet using internal stack
    printf("\r\nSend ping request using offload stack\r\n");
    status = sl_si91x_send_ping(remote_ip_address, PING_PACKET_SIZE);
    if (status != SL_STATUS_IN_PROGRESS) {
      printf("\r\nPing request failed with status 0x%lX\r\n", status);
      return;
    }

    // Sleep for 2 seconds
    osDelay(2000);
  }
}
