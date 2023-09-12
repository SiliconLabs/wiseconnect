/*******************************************************************************
 * @file
 * @brief Firmware Update Example Application
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
#include "sl_utility.h"
#include "errno.h"
#include <string.h>
#include "sl_si91x_driver.h"

#ifdef RSI_M4_INTERFACE
#include "sl_si91x_hal_soc_soft_reset.h"
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
//! Server IP address.
#define SERVER_IP_ADDRESS "192.168.0.100"

//! Server port number
#define SERVER_PORT 5000

// Receive data length
#define RECV_BUFFER_SIZE 1027

#define FW_RPS_FILE_SIZE 1870912
#define FW_HEADER_SIZE   64
#define CHUNK_SIZE       1024

#define SI91X_STATUS_FW_UPDATE_DONE 0x10003

/******************************************************
 *               Global Variable
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

static const sl_wifi_device_configuration_t sl_wifi_firmware_update_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .region_code = US,
  .boot_config = { .oper_mode                  = SL_SI91X_CLIENT_MODE,
                   .coex_mode                  = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map            = (SL_SI91X_FEAT_SECURITY_PSK | SL_SI91X_FEAT_AGGREGATION),
                   .tcp_ip_feature_bit_map     = (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT),
                   .custom_feature_bit_map     = (SL_SI91X_FEAT_CUSTOM_FEAT_EXTENTION_VALID),
                   .ext_custom_feature_bit_map = (SL_SI91X_EXT_FEAT_XTAL_CLK
#ifndef RSI_M4_INTERFACE
                                                  | RAM_LEVEL_NWP_ALL_MCU_ZERO
#else
                                                  | RAM_LEVEL_NWP_ADV_MCU_BASIC
#endif
#ifdef CHIP_917
                                                  | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
                                                  ),
                   .bt_feature_bit_map         = 0,
                   .ext_tcp_ip_feature_bit_map = 0,
                   .ble_feature_bit_map        = 0,
                   .ble_ext_feature_bit_map    = 0,
                   .config_feature_bit_map     = 0 }
};

uint8_t recv_buffer[RECV_BUFFER_SIZE];

/******************************************************
 *               Function Declarations
 ******************************************************/
static void application_start(void *argument);
sl_status_t m4_firmware_update_app(void);
void soft_reset(void);
void on_timeout_callback(void);
void watchdog_timer_init(void);
/******************************************************
 *               Function Definitions
 ******************************************************/

void app_init(const void *unused)
{
  UNUSED_PARAMETER(unused);
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

static void application_start(void *argument)
{
  UNUSED_PARAMETER(argument);
  sl_status_t status;

  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &sl_wifi_firmware_update_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    printf("Failed to start Wi-Fi client interface: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nWi-Fi Init Success\r\n");

  status = sl_net_up(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID);
  if (status != SL_STATUS_OK) {
    printf("Failed to bring Wi-Fi client interface up: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nWi-Fi Client Connected\r\n");

  status = m4_firmware_update_app();
  if (status != SL_STATUS_OK) {
    printf("\r\n Update Firmware failed with status 0x%lx\r\n", status);
    return;
  }
}

sl_status_t m4_firmware_update_app()
{
  sl_status_t status                = SL_STATUS_FAIL;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);
  int client_socket                 = -1;
  int socket_return_value           = 0;
  uint16_t chunk                    = 1;
  uint16_t chunk_max_count          = 1; // for header
  uint16_t fwup_chunk_length        = 0;
  int16_t recv_size                 = 0;
  uint8_t send_buffer[3]            = { 0 };
  uint8_t fwup_chunk_type           = 0;

  chunk_max_count += (FW_RPS_FILE_SIZE - FW_HEADER_SIZE) / CHUNK_SIZE;

  server_address.sin_family = AF_INET;
  server_address.sin_port   = SERVER_PORT;
  sl_net_inet_addr(SERVER_IP_ADDRESS, &server_address.sin_addr.s_addr);

  client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (client_socket < 0) {
    printf("\r\nSocket creation failed with BSD error: %d\r\n", errno);
    return SL_STATUS_FAIL;
  }
  printf("\r\nClient socket : %d\r\n", client_socket);

  socket_return_value = connect(client_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    printf("\r\nSocket Connect failed with BSD error: %d\r\n", errno);
    close(client_socket);
    return SL_STATUS_FAIL;
  }
  printf("\r\nConnect to TCP Server Success\r\n");

  printf("\r\nM4 Firmware update start\r\n");
  while (1) {

    if (chunk > chunk_max_count) {
      printf("\r\n chunk : %d > chunk_max_count %d. Firmware update failed.\r\n", chunk, chunk_max_count);
      close(client_socket);
      return SL_STATUS_FAIL;
    }

    // Fill packet type
    if (chunk == 1) {
      send_buffer[0] = SL_FWUP_RPS_HEADER;
    } else {
      send_buffer[0] = SL_FWUP_RPS_CONTENT;
    }

    // Fill packet number
    memcpy(&send_buffer[1], &chunk, 2);

    // Send firmware upgrade request to remote peer
    int data_length = send(client_socket, (int8_t *)send_buffer, 3, 0);
    if (data_length < 0) {
      printf("\r\nFailed to Send data, BSD Error Code: %d\r\n", errno);
      close(client_socket);
      return SL_STATUS_FAIL;
    }

    // Get first 3 bytes from remote peer
    recv_size   = 3;
    data_length = recv(client_socket, recv_buffer, recv_size, 0);
    if (data_length < 0) {
      printf("\r\nFailed to Receive data, BSD Error Code: %d\r\n", errno);
      close(client_socket);
      return SL_STATUS_FAIL;
    }
    // Get the received chunk type
    fwup_chunk_type = recv_buffer[0];

    // Get the received chunk length
    memcpy(&fwup_chunk_length, &recv_buffer[1], 2);

    // Get packet of chunk length from remote peer
    recv_size = fwup_chunk_length;

    if (recv_size > 0) {
      data_length = recv(client_socket, recv_buffer, recv_size, 0);
      if (data_length < 0) {
        printf("\r\nFailed to Receive data from remote peer, BSD Error Code: %d\r\n", errno);
        close(client_socket);
        return SL_STATUS_FAIL;
      }

      // Call corresponding firmware upgrade API based on the chunk type
      if (fwup_chunk_type == SL_FWUP_RPS_HEADER) {
        // Send RPS header which is received as first chunk
        status = sl_si91x_fwup_start(recv_buffer);
      } else {
        // Send RPS content
        status = sl_si91x_fwup_load(recv_buffer, fwup_chunk_length);
      }
    }

    if (status != SL_STATUS_OK) {
      if (status == SI91X_STATUS_FW_UPDATE_DONE) {
        // Close the socket
        close(client_socket);
        printf("\r\nM4 Firmware update complete\r\n");

        sl_si91x_soc_soft_reset();

        return SL_STATUS_OK;
      } else {
        printf("\r\nFirmware update failed : 0x%lx\n", status);
        close(client_socket);
        return SL_STATUS_FAIL;
      }
    }
    chunk++;
  }

  return SL_STATUS_OK;
}
