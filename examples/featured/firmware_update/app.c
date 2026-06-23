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

#ifdef SLI_SI91X_MCU_INTERFACE
#include "sl_si91x_hal_soc_soft_reset.h"
#endif

/******************************************************
 *                      Macros
 ******************************************************/
/******************************************************
 *                    Constants
 ******************************************************/
#ifdef SLI_SI91X_MCU_INTERFACE
//! Set 1 for combined image
#define COMBINED_IMAGE 0
#endif

//! Server IP address.
#define SERVER_IP_ADDRESS "192.168.0.213"

//! Server port number
#define SERVER_PORT 5001

// Receive data length
#define RECV_BUFFER_SIZE 1024

#define FW_HEADER_SIZE 64
#define CHUNK_SIZE     1024

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
};

static const sl_wifi_device_configuration_t firmware_update_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .region_code = US,
  .boot_config = { .oper_mode              = SL_SI91X_CLIENT_MODE,
                   .coex_mode              = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map        = (SL_WIFI_FEAT_SECURITY_PSK | SL_WIFI_FEAT_AGGREGATION),
                   .tcp_ip_feature_bit_map = (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT),
                   .custom_feature_bit_map = (SL_WIFI_SYSTEM_CUSTOM_FEAT_EXTENSION_VALID),
                   .ext_custom_feature_bit_map =
                     (SL_SI91X_EXT_FEAT_XTAL_CLK | SL_SI91X_EXT_FEAT_UART_SEL_FOR_DEBUG_PRINTS | MEMORY_CONFIG
#ifdef SLI_SI917
                      | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
                      ),
                   .bt_feature_bit_map         = 0,
                   .ext_tcp_ip_feature_bit_map = SL_SI91X_CONFIG_FEAT_EXTENSION_VALID,
                   .ble_feature_bit_map        = 0,
                   .ble_ext_feature_bit_map    = 0,
                   .config_feature_bit_map     = 0 }
};

uint8_t recv_buffer[RECV_BUFFER_SIZE];

/******************************************************
 *               Function Declarations
 ******************************************************/
static void application_start(void *argument);
sl_status_t update_firmware(void);

/******************************************************
 *               Function Definitions
 ******************************************************/

void app_init(void)
{
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

static void application_start(void *argument)
{
  UNUSED_PARAMETER(argument); // Avoid compiler warnings when parameter is not used
  sl_status_t status;

  SL_DEBUG_LOG_V2(INFO, "Application Started\r\n");
  // Initialize the Wi-Fi client interface with the configuration specified by firmware_update_configuration
  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &firmware_update_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to start Wi-Fi client interface: 0x%lx\r\n", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Wi-Fi Init Success\r\n");

  // Bring up the Wi-Fi client interface
  status = sl_net_up(SL_NET_WIFI_CLIENT_INTERFACE, 0);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to bring Wi-Fi client interface up: 0x%lx\r\n", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Wi-Fi Client Connected\r\n");

  // Update device firmware by downloading firmware file from remote TCP server
  status = update_firmware();
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, " Update Firmware failed with status 0x%lx\r\n", status);
    return;
  }

  SL_DEBUG_LOG_V2(INFO, "Firmware Upgrade Completed\r\n");
}

sl_status_t update_firmware()
{
  sl_wifi_firmware_version_t version = { 0 };
  sl_status_t status                 = SL_STATUS_FAIL;
  struct sockaddr_in server_address  = { 0 };
  socklen_t socket_length            = sizeof(struct sockaddr_in);
  int client_socket                  = -1;
  int socket_return_value            = 0;
  uint32_t start                     = 0;
  uint32_t finish                    = 0;
  uint16_t chunk                     = 1;
  uint16_t chunk_max_count           = 1; // for header
  uint16_t fwup_chunk_length         = 0;
  int16_t recv_size                  = 0;
  uint8_t send_buffer[3]             = { 0 };
  uint8_t fwup_chunk_type            = 0;
  uint32_t fw_image_size             = 0;

  // Gets firmware version from device
  status = sl_wifi_get_firmware_version(&version);
  VERIFY_STATUS_AND_RETURN(status);
  print_firmware_version(&version);

  // Set up the server IP address and port number
  server_address.sin_family = AF_INET;
  server_address.sin_port   = SERVER_PORT;
  sl_net_inet_addr(SERVER_IP_ADDRESS, &server_address.sin_addr.s_addr);

  // Create TCP client socket
  client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  SL_DEBUG_LOG_V2(INFO, "client_socket : %d\r\n", client_socket);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket creation failed with bsd error: %d\r\n", errno);
    return SL_STATUS_FAIL;
  }

  // Connect to the TCP server
  socket_return_value = connect(client_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket Connect failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return SL_STATUS_FAIL;
  }
  SL_DEBUG_LOG_V2(INFO, "Connect to TCP Server Success\r\n");

  SL_DEBUG_LOG_V2(INFO, "Firmware update start\r\n");
  start = osKernelGetTickCount();
  while (1) {

    if (chunk > (chunk_max_count + 1)) {
      SL_DEBUG_LOG_V2(ERROR, " chunk : %d > chunk_max_count %d. Firmware update failed.\r\n", chunk, chunk_max_count);
      finish = osKernelGetTickCount();
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
      if (errno == ENOBUFS)
        continue;
      SL_DEBUG_LOG_V2(ERROR, "Failed to Send data, Error Code : 0x%X\r\n", data_length);
      finish = osKernelGetTickCount();
      sl_si91x_fwup_abort();
      close(client_socket);
      return SL_STATUS_FAIL;
    }

    // Get first 3 bytes from remote peer
    recv_size   = 3;
    data_length = recv(client_socket, recv_buffer, recv_size, 0);
    if (data_length < 0) {
      SL_DEBUG_LOG_V2(ERROR, "Failed to Receive data, Error Code : 0x%d\r\n", data_length);
      finish = osKernelGetTickCount();
      sl_si91x_fwup_abort();
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
        SL_DEBUG_LOG_V2(ERROR, "Failed to Receive data from remote peer, Error Code : 0x%d\r\n", data_length);
        finish = osKernelGetTickCount();
        sl_si91x_fwup_abort();
        close(client_socket);
        return SL_STATUS_FAIL;
      }

      // Call corresponding firmware upgrade API based on the chunk type
      if (fwup_chunk_type == SL_FWUP_RPS_HEADER) {
        //! Send the first chunk to extract OTA image size
        status = sl_wifi_get_firmware_size((void *)recv_buffer, &fw_image_size);
        if (status != SL_STATUS_OK) {
          SL_DEBUG_LOG_V2(ERROR, "Unable to fetch firmware size. Status: 0x%lx\r\n", status);
          close(client_socket);
          return SL_STATUS_FAIL;
        }
        SL_DEBUG_LOG_V2(INFO, " Image size = 0x%lx\r\n", fw_image_size);
        chunk_max_count += ((fw_image_size - FW_HEADER_SIZE) / CHUNK_SIZE) + 1;
        // Send RPS header which is received as first chunk
        status = sl_si91x_fwup_start(recv_buffer);
      } else {
        // Send RPS content
        status = sl_si91x_fwup_load(recv_buffer, fwup_chunk_length);
      }
    }

    if (status != SL_STATUS_OK) { // Check if firmware update is completed
      if (status == SL_STATUS_SI91X_FW_UPDATE_DONE) {
        finish = osKernelGetTickCount();
        // Close the socket
        close(client_socket);
        osDelay(3000);
        SL_DEBUG_LOG_V2(INFO, "Firmware update complete\r\n");
        SL_DEBUG_LOG_V2(INFO, "FW update duration : %ld\r\n", finish - start);

#ifdef SLI_SI91X_MCU_INTERFACE
//! Perform SOC soft reset for combined Image
#if COMBINED_IMAGE
        SL_DEBUG_LOG_V2(INFO, "SoC Soft Reset initiated!\r\n");
        sl_si91x_soc_nvic_reset();
#endif
#endif

        // De-initialize the client network interface
        status = sl_net_deinit(SL_NET_WIFI_CLIENT_INTERFACE);
        SL_DEBUG_LOG_V2(INFO, "Wi-Fi Deinit status : %lx\r\n", status);
        VERIFY_STATUS_AND_RETURN(status);

#ifdef SL_NCP_UART_INTERFACE
        SL_DEBUG_LOG_V2(INFO, "Waiting for firmware upgrade to complete\r\n");
        osDelay(40000);
        SL_DEBUG_LOG_V2(INFO, "Waiting Done\r\n");
#endif

        // Initialize the client interface again to check if firmware update is done properly
        status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &firmware_update_configuration, NULL, NULL);
        SL_DEBUG_LOG_V2(INFO, "Wi-Fi Init status : %lx\r\n", status);
        VERIFY_STATUS_AND_RETURN(status);

        status = sl_wifi_get_firmware_version(&version);
        VERIFY_STATUS_AND_RETURN(status);
        print_firmware_version(&version);

        return SL_STATUS_OK;
      } else {
        SL_DEBUG_LOG_V2(ERROR, "Firmware update failed : %lx\r\n", status);
        finish = osKernelGetTickCount();
        close(client_socket);
        SL_DEBUG_LOG_V2(INFO, "FW update duration : %ld\r\n", finish - start);
        return SL_STATUS_FAIL;
      }
    }
    chunk++;
  }

  return SL_STATUS_OK;
}
