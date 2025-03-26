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

#include "cmsis_os2.h"
#include "errno.h"
#include "rsi_debug.h"
#include "sl_board_configuration.h"
#include "sl_net.h"
#include "sl_si91x_fw_fallback.h"
#include "sl_si91x_driver.h"
#include "sl_utility.h"
#include "sl_wifi.h"
#include "socket.h"
#include <string.h>

#ifdef SLI_SI91X_MCU_INTERFACE
#include "sl_si91x_hal_soc_soft_reset.h"
#endif

/******************************************************
  *                      Macros
  ******************************************************/

/******************************************************
  *                    Constants
  ******************************************************/
//! Server IP address - The IP address of the server where the OTA update will be fetched.
#define SERVER_IP_ADDRESS "192.168.0.158"

//! Server port number - The port on which the server is listening for connections.
#define SERVER_PORT 5005

//! Receive data length - The buffer size for receiving data packets.
#define RECV_BUFFER_SIZE 1027

//! Number of remaining bytes to be processed in the current OTA update.
uint16_t number_of_remaining_bytes = 0;

//! Chunk index - Tracks the current chunk number being processed.
uint16_t chunk = 1;

//! OTA image start address - The memory address where the OTA update will be stored.
static uint32_t ota_image_start_address = 0;

//! OTA image size - The total size of the OTA update in bytes.
static uint32_t ota_image_size = 0;

//! M4 OTA image - The memory location or reference for the M4 core's OTA update.
static uint32_t m4_ota_image = 0;

//! NWP OTA image - The memory location or reference for the NWP OTA update.
static uint32_t ta_ota_image = 0;

//! Buffer to store received data from the server during the firmware update process.
uint8_t recv_buffer[RECV_BUFFER_SIZE];

//! Enumeration representing the different states of the firmware update process.
typedef enum {
  STATE_SENDING_REQUEST,     //!< Sending the request to the server for the firmware update.
  STATE_RECEIVING_RESPONSE,  //!< Receiving the response (firmware data) from the server.
  STATE_PROCESSING_RESPONSE, //!< Processing the received firmware data (e.g., storing, verifying).
  STATE_COMPLETED,           //!< Firmware update process completed successfully.
  STATE_ERROR                //!< An error occurred during the firmware update process.
} firmware_update_state_t;

/* Extern symbols from the linker script */
extern uint32_t __rom_start;
extern uint32_t __rom_length;

/** @brief Default flash address for NWP Slot A. */
#define DEFAULT_NWP_SLOT_A_OFFSET 0x4011000

/** @brief Example firmware image size. */
#define DEFAULT_NWP_IMAGE_SIZE 0x2EFE0

#define SL_APP_TOGGLE_SLOT_INFO 0
/******************************************************
  *               Function Declarations
  ******************************************************/

//! Starts the application, initializing necessary components and handling the main process.
static void application_start(void *argument);

//! Manages the overall firmware update process by handling communication with the server.
static sl_status_t firmware_update_process(int client_socket);

//! Sends a request to the server for a firmware update, specifying the chunk to be received.
static sl_status_t sl_app_sending_request(int client_socket, uint16_t chunk, uint16_t chunk_max_count);

//! Receives a response from the server, extracting the firmware update chunk type and length.
static sl_status_t sl_app_receiving_response(int client_socket, uint8_t *fwup_chunk_type, uint16_t *fwup_chunk_length);

//! Processes the received firmware update chunk, verifying and storing the data appropriately.
static sl_status_t sl_app_processing_response(int client_socket,
                                              uint8_t fwup_chunk_type,
                                              uint16_t fwup_chunk_length,
                                              uint16_t *chunk,
                                              uint16_t *chunk_max_count);
void sl_app_display_ab_slot_info(const sl_si91x_fw_ab_slot_management_t *slot_info);

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
  .boot_option         = NWP_FW_LOAD_CMD,
  .nwp_fw_image_number = NWP_FW_IMAGE_NUMBER_1,
  .mac_address         = NULL,
  .band                = SL_SI91X_WIFI_BAND_2_4GHZ,
  .region_code         = US,
  .boot_config         = { .oper_mode                  = SL_SI91X_CLIENT_MODE,
                           .coex_mode                  = SL_SI91X_WLAN_ONLY_MODE,
                           .feature_bit_map            = (SL_SI91X_FEAT_SECURITY_PSK | SL_SI91X_FEAT_AGGREGATION),
                           .tcp_ip_feature_bit_map     = (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT),
                           .custom_feature_bit_map     = (SL_SI91X_CUSTOM_FEAT_EXTENTION_VALID),
                           .ext_custom_feature_bit_map = (SL_SI91X_EXT_FEAT_XTAL_CLK
#ifndef SLI_SI91X_MCU_INTERFACE
                                                  | SL_SI91X_RAM_LEVEL_NWP_ALL_AVAILABLE
#else
                                                  | SL_SI91X_RAM_LEVEL_NWP_ADV_MCU_BASIC
#endif
#if defined(SLI_SI917) || defined(SLI_SI915)
                                                  | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
                                                  ),
                   .bt_feature_bit_map         = 0,
                   .ext_tcp_ip_feature_bit_map = 0,
                   .ble_feature_bit_map        = 0,
                   .ble_ext_feature_bit_map    = 0,
                   .config_feature_bit_map     = 0 }
};

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
  sl_status_t status                 = SL_STATUS_FAIL;
  struct sockaddr_in server_address  = { 0 }; // Server address structure
  socklen_t socket_length            = sizeof(struct sockaddr_in);
  int client_socket                  = -1;           // Client socket descriptor
  int socket_return_value            = 0;            // Return value for socket operations
  sl_wifi_firmware_version_t version = { 0 };        // Structure to store firmware version
  sl_si91x_fw_ab_slot_management_t app_ab_slot_info; // Structure to manage firmware slots

  UNUSED_PARAMETER(argument); // Avoids compiler warning for unused parameter

  // Get the start address of the ROM from the linker script
  uint32_t rom_address = (uint32_t)&__rom_start;
  // Get the length of the ROM from the linker script
  uint32_t rom_length = (uint32_t)&__rom_length;
  // Initialize the Wi-Fi client interface

  DEBUGOUT("\r\nFW FallBack App is Running:%lX\r\n", rom_address);
  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &sl_wifi_firmware_update_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    DEBUGOUT("Failed to start Wi-Fi client interface: 0x%lx\r\n", status);
    while (1)
      ; // Halt execution in case of failure
  }
  DEBUGOUT("\r\nWi-Fi Init Success\r\n");

  // Retrieve the current firmware version
  status = sl_wifi_get_firmware_version(&version);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to fetch firmware version: 0x%lx\r\n", status);
  } else {
    print_firmware_version(&version);
  }

  // Retrieve the current slot information
  status = sl_si91x_ab_get_slot_info(&app_ab_slot_info);
  sl_app_display_ab_slot_info(&app_ab_slot_info);
  if (status == SL_SI91X_AB_ERR_INVALID_SLOT_INFO) {
    // If getting slot info fails, print the error and current ROM address and length
    DEBUGOUT("\r\n get_slot info fail loading default slot info :%lX %lX %lX \r\n", status, rom_address, rom_length);

    // Set the slot info for the M4 core using the current linker address and length
    status = sl_si91x_ab_upgrade_set_slot_info(rom_address - SLI_SI91X_CHUNK_LENGTH,
                                               rom_length,
                                               SL_SI91X_AB_OTA_IMAGE_TYPE_M4);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("Failed to update M4 slot, error: %lu\n", status);
    } else {
      DEBUGOUT("Successfully updated M4 slot with address: %lX and length: %lX\n", rom_address, rom_length);
    }

    // Set the slot info for the NWP core using predefined offset and image size
    status = sl_si91x_ab_upgrade_set_slot_info(DEFAULT_NWP_SLOT_A_OFFSET,
                                               DEFAULT_NWP_IMAGE_SIZE,
                                               SL_SI91X_AB_OTA_IMAGE_TYPE_NWP);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("Failed to update NWP slot, error: %lu\n", status);
    } else {
      DEBUGOUT(" \r\n Successfully updated NWP slot with offset: %X and size: %X \r\n",
               DEFAULT_NWP_SLOT_A_OFFSET,
               DEFAULT_NWP_IMAGE_SIZE);
    }
  }

  // Bring up the Wi-Fi client interface
  status = sl_net_up(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID);
  if (status != SL_STATUS_OK) {
#if SL_APP_TOGGLE_SLOT_INFO
    status = sl_si91x_toggle_slot_info(1, 1);
    if (status != SL_STATUS_OK) {
      while (1)
        ;
    }
    sl_si91x_soc_nvic_reset();
#endif
    DEBUGOUT("Failed to bring Wi-Fi client interface up: 0x%lx\r\n", status);
    osThreadExit(); // Exit thread on failure
  }
  DEBUGOUT("\r\nWi-Fi Client Connected\r\n");

  // Configure the server address
  server_address.sin_family = AF_INET;
  server_address.sin_port   = SERVER_PORT;
  sl_net_inet_addr(SERVER_IP_ADDRESS, &server_address.sin_addr.s_addr);

  // Create a TCP socket
  client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (client_socket < 0) {
    DEBUGOUT("\r\nSocket creation failed with BSD error: %d\r\n", errno);
    osThreadExit(); // Exit thread on failure
  }
  DEBUGOUT("\r\nClient socket : %d\r\n", client_socket);

  // Connect to the firmware update server
  socket_return_value = connect(client_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    DEBUGOUT("\r\nSocket Connect failed with BSD error: %d\r\n", errno);
    close(client_socket);
    osThreadExit(); // Exit thread on failure
  }
  DEBUGOUT("\r\nConnect to TCP Server Success\r\n");

  // **Start Firmware Update Process**
  status = firmware_update_process(client_socket);
  if (status == SL_STATUS_OK) {
    DEBUGOUT("\r\nFirmware update was successful. Task will continue monitoring.\r\n");
    // Optionally reset the system after firmware update
    sl_si91x_soc_nvic_reset();
  } else {
    DEBUGOUT("\r\nFirmware update failed. Retrying...\r\n");
    osThreadExit();
  }
}

static sl_status_t firmware_update_process(int client_socket)
{
  uint16_t data_chunk           = 0;                     // Tracks the current firmware data chunk
  uint16_t chunk_max_count      = 0;                     // Total number of chunks expected
  uint16_t fwup_chunk_length    = 0;                     // Length of the received firmware chunk
  uint8_t fwup_chunk_type       = 0;                     // Type of received firmware chunk
  firmware_update_state_t state = STATE_SENDING_REQUEST; // Initial state
  uint32_t status               = SL_STATUS_FAIL;        // Default failure status

  while (1) {
    switch (state) {
      case STATE_SENDING_REQUEST:
        // Send a request to the server for firmware update
        state = sl_app_sending_request(client_socket, data_chunk, chunk_max_count);
        break;

      case STATE_RECEIVING_RESPONSE:
        // Receive the firmware update response from the server
        state = sl_app_receiving_response(client_socket, &fwup_chunk_type, &fwup_chunk_length);
        break;

      case STATE_PROCESSING_RESPONSE:
        // Process the received firmware chunk and update state accordingly
        state =
          sl_app_processing_response(client_socket, fwup_chunk_type, fwup_chunk_length, &data_chunk, &chunk_max_count);
        break;

      case STATE_COMPLETED:
        // Firmware update process completed successfully
        DEBUGOUT("\r\nFirmware Update Completed Successfully:%lX\r\n", ota_image_start_address);

        // Close the client socket
        close(client_socket);

        // Perform integrity check on the downloaded firmware
        status = sl_si91x_verify_image(ota_image_start_address);
        if (status != SL_STATUS_SI91X_FW_UPDATE_DONE) {
          DEBUGOUT("\r\nFirmware integrity check fail:%lX\r\n", status);
          return STATE_ERROR;
        } else {
          DEBUGOUT("\r\nFirmware integrity check success:%lX\r\n", status);
          // Updating M4 firmware slot
          if (m4_ota_image) {
            status =
              sl_si91x_ab_upgrade_set_slot_info(ota_image_start_address, ota_image_size, SL_SI91X_AB_OTA_IMAGE_TYPE_M4);
            if (status != SL_STATUS_OK) {
              DEBUGOUT("Failed to update M4 slot, error: %lu\n", status);
            }
          }

          // Updating NWP firmware slot
          if (ta_ota_image) {
            status = sl_si91x_ab_upgrade_set_slot_info(ota_image_start_address,
                                                       ota_image_size,
                                                       SL_SI91X_AB_OTA_IMAGE_TYPE_NWP);
            if (status != SL_STATUS_OK) {
              DEBUGOUT("Failed to update NWP slot, error: %lu\n", status);
            }
          }
        }
        return SL_STATUS_OK; // Return success

      case STATE_ERROR:
        // Handle errors encountered during the firmware update process
        DEBUGOUT("\r\nError occurred during firmware update process\r\n");
        close(client_socket);
        return SL_STATUS_FAIL; // Return failure

      default:
        // Handle unexpected states
        DEBUGOUT("\r\nUnknown state encountered\r\n");
        return SL_STATUS_FAIL; // Return failure
    }
  }
}
static sl_status_t sl_app_sending_request(int client_socket, uint16_t data_chunk, uint16_t chunk_max_count)
{
  uint8_t send_buffer[3] = { 0 }; // Buffer to hold the request message

  // Check if the requested chunk exceeds the allowed maximum count
  if (chunk_max_count > 0 && (data_chunk > chunk_max_count)) {
    DEBUGOUT("\r\nChunk %d exceeded chunk_max_count %d. Firmware update failed.\r\n", data_chunk, chunk_max_count);
    return STATE_ERROR;
  }

  // Determine the request type: first chunk (header) or subsequent chunks (content)
  send_buffer[0] = (chunk == 1) ? SL_FWUP_RPS_HEADER : SL_FWUP_RPS_CONTENT;

  // Copy the chunk number into the request buffer (little-endian format)
  memcpy(&send_buffer[1], &chunk, 2);

  // Send the request to the server
  int data_length = send(client_socket, (uint8_t *)send_buffer, 3, 0);
  if (data_length < 0) {
    // If there's a buffer issue, retry sending the request
    if (errno == ENOBUFS)
      return STATE_SENDING_REQUEST; // Retry sending

    // Log other socket errors and return failure
    DEBUGOUT("\r\nFailed to send data, BSD Error Code: %d\r\n", errno);
    return STATE_ERROR;
  }

  // Successfully sent the request, move to the next state (receiving response)
  return STATE_RECEIVING_RESPONSE;
}

static sl_status_t sl_app_receiving_response(int client_socket, uint8_t *fwup_chunk_type, uint16_t *fwup_chunk_length)
{
  uint8_t recv_buffer[3] = { 0 };
  int recv_size          = 3;

  int data_length = recv(client_socket, recv_buffer, recv_size, 0);
  if (data_length < 0) {
    DEBUGOUT("\r\nFailed to receive data, BSD Error Code: %d\r\n", errno);
    return STATE_ERROR;
  }

  *fwup_chunk_type = recv_buffer[0];
  memcpy(fwup_chunk_length, &recv_buffer[1], 2);
  return STATE_PROCESSING_RESPONSE;
}

static sl_status_t sl_app_processing_response(int client_socket,
                                              uint8_t fwup_chunk_type,
                                              uint16_t fwup_chunk_length,
                                              uint16_t *data_chunk,
                                              uint16_t *chunk_max_count)
{
  uint8_t recv_buffer[fwup_chunk_length]; // Buffer to hold received firmware data chunk
  int recv_size = fwup_chunk_length;
  sl_status_t status;
  ota_image_info_t ota_config;
  static uint32_t current_flash_offset = 0; // Track current flash write offset

  if (recv_size > 0) {
    DEBUGOUT("\r\nImage size: %x\r\n", recv_size);

    // Receive the firmware chunk from the server
    int data_length = recv(client_socket, recv_buffer, recv_size, 0);
    if (data_length < 0) {
      DEBUGOUT("\r\nFailed to receive data from remote peer, BSD Error Code: %d\r\n", errno);
      return STATE_ERROR; // Fatal error: Cannot proceed without valid firmware data
    }

    // Check if received chunk is the firmware header
    if (fwup_chunk_type == SL_SI91X_HEADER_PACKET) {

      // Extract firmware metadata from the received header
      status = sl_si91x_ab_upgrade_get_rps_configs(recv_buffer, &ota_config);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("\r\nError: OTA config extraction failed.\r\n");
        return STATE_ERROR; // Fatal error: Unable to parse firmware metadata
      }

      // Calculate total number of chunks required for complete firmware transfer
      *chunk_max_count =
        (ota_config.ota_image_size + SL_SI91X_MAX_OTA_IMAGE_CHUNK_SIZE - 1) / SL_SI91X_MAX_OTA_IMAGE_CHUNK_SIZE;
      number_of_remaining_bytes = (ota_config.ota_image_size % SL_SI91X_MAX_OTA_IMAGE_CHUNK_SIZE);

      ota_image_start_address = ota_config.ota_image_offset;
      ota_image_size          = ota_config.ota_image_size;

      // Identify firmware type (M4 or NWP)
      if (ota_config.ota_image_type == SL_SI91X_M4_RPS_BIT) {
        m4_ota_image = 1;
      } else if (ota_config.ota_image_type == SL_SI91X_NWP_RPS_BIT) {
        ta_ota_image = 1;
      }

      DEBUGOUT("\r\nAddr: %lx, Size: %lx type:%u\r\n",
               ota_config.ota_image_offset,
               ota_config.ota_image_size,
               ota_config.ota_image_type);
      DEBUGOUT("\r\n max_count: %x, remaining_bytes: %x\r\n", *chunk_max_count, number_of_remaining_bytes);

      current_flash_offset = ota_config.ota_image_offset;

      // Erase flash memory before writing new firmware
      status = sl_si91x_flash_erase(ota_config.ota_image_offset, ota_config.ota_image_size);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("\r\nFirmware flash erase fail:%lX\r\n", status);
        return STATE_ERROR; // Fatal error: Cannot write new firmware without erasing flash
      } else {
        DEBUGOUT("\r\nFirmware flash erase success:%lu\r\n", status);
      }

    } else {
      // Determine size of the current firmware chunk to write
      fwup_chunk_length =
        (*data_chunk < *chunk_max_count)
          ? SL_SI91X_MAX_OTA_IMAGE_CHUNK_SIZE
          : (number_of_remaining_bytes == 0 ? SL_SI91X_MAX_OTA_IMAGE_CHUNK_SIZE : number_of_remaining_bytes);

      DEBUGOUT("\r\nChunk: %d, fwup_chunk_length: %d, chunk_max_count: %d\r\n",
               *data_chunk,
               fwup_chunk_length,
               *chunk_max_count);
      DEBUGOUT("\r\ncurrent_flash_offset: %lx", current_flash_offset);

      // Write firmware chunk to flash memory
      status = sl_si91x_flash_write(current_flash_offset, recv_buffer, fwup_chunk_length);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("\r\nFirmware flash Write fail:%lu\r\n", status);
        return STATE_ERROR; // Fatal error: Firmware update cannot proceed without writing to flash
      }
      current_flash_offset += fwup_chunk_length;
    }

    if (status != SL_STATUS_OK) {
      DEBUGOUT("\r\nFirmware update operation failed.\r\n");
      return STATE_ERROR; // Fatal error: Firmware update process failed
    }

    // Check if all chunks have been processed
    if (*data_chunk >= (*chunk_max_count)) {
      return STATE_COMPLETED; // Firmware update completed successfully
    } else {
      (*data_chunk)++; // Move to the next chunk
      chunk++;
      return STATE_SENDING_REQUEST; // Request the next firmware chunk from the server
    }
  }

  return STATE_COMPLETED; // Firmware update completed successfully
}
void sl_app_display_ab_slot_info(const sl_si91x_fw_ab_slot_management_t *slot_info)
{
  if (slot_info == NULL) {
    DEBUGOUT("\r\nError: NULL pointer passed for slot_info!\n");
    return;
  }

  DEBUGOUT("\r\n========= Firmware Slot Information =========");
  DEBUGOUT("\r\nMagic Word: 0x%lX", (unsigned long)slot_info->slot_magic_word);

  // M4 Slot Information
  DEBUGOUT("\r\n---- M4 Slot Information ----");
  DEBUGOUT("\r\nM4 Slot A:");
  DEBUGOUT("\r\n  Slot ID: %u", slot_info->m4_slot_info.m4_slot_A.slot_id);
  DEBUGOUT("\r\n  Image Offset: 0x%lX", (unsigned long)slot_info->m4_slot_info.m4_slot_A.slot_image_offset);
  DEBUGOUT("\r\n  Image Size: %lu bytes", (unsigned long)slot_info->m4_slot_info.m4_slot_A.image_size);

  DEBUGOUT("\r\nM4 Slot B:");
  DEBUGOUT("\r\n  Slot ID: %u", slot_info->m4_slot_info.m4_slot_B.slot_id);
  DEBUGOUT("\r\n  Image Offset: 0x%lX", (unsigned long)slot_info->m4_slot_info.m4_slot_B.slot_image_offset);
  DEBUGOUT("\r\n  Image Size: %lu bytes", (unsigned long)slot_info->m4_slot_info.m4_slot_B.image_size);

  DEBUGOUT("\r\nCurrently Active M4 Slot: %c", slot_info->m4_slot_info.current_active_M4_slot == SLOT_A ? 'A' : 'B');

  // NWP Slot Information
  DEBUGOUT("\r\n---- NWP Slot Information ----");
  DEBUGOUT("\r\nNWP Slot A:");
  DEBUGOUT("\r\n  Slot ID: %u", slot_info->nwp_slot_info.nwp_slot_A.slot_id);
  DEBUGOUT("\r\n  Image Offset: 0x%lX", (unsigned long)slot_info->nwp_slot_info.nwp_slot_A.slot_image_offset);
  DEBUGOUT("\r\n  Image Size: %lu bytes", (unsigned long)slot_info->nwp_slot_info.nwp_slot_A.image_size);

  DEBUGOUT("\r\nNWP Slot B:");
  DEBUGOUT("\r\n  Slot ID: %u", slot_info->nwp_slot_info.nwp_slot_B.slot_id);
  DEBUGOUT("\r\n  Image Offset: 0x%lX", (unsigned long)slot_info->nwp_slot_info.nwp_slot_B.slot_image_offset);
  DEBUGOUT("\r\n  Image Size: %lu bytes", (unsigned long)slot_info->nwp_slot_info.nwp_slot_B.image_size);

  DEBUGOUT("\r\nCurrently Active NWP Slot: %c", slot_info->nwp_slot_info.current_active_nwp_slot == SLOT_A ? 'A' : 'B');

  DEBUGOUT("\r\nSlot Structure CRC: 0x%lX", (unsigned long)slot_info->slot_struct_crc);
  DEBUGOUT("\r\n=============================================\r\n");
}

// End of file