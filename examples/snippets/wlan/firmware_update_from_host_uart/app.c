/*******************************************************************************
 * @file
 * @brief Firmware Update Example using Host MCU UART
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
#include "em_usart.h"

#if defined _SILICON_LABS_32B_SERIES_2_CONFIG_5
#include "sl_iostream_eusart_vcom_config.h"
#elif defined _SILICON_LABS_32B_SERIES_2_CONFIG_1
#include "sl_iostream_usart_vcom_config.h"
#endif

/******************************************************
 *                      Macros
 ******************************************************/
/******************************************************
 *                    Constants
 ******************************************************/

// Receive data length
#define RECV_BUFFER_SIZE 16384 // 16k

#define FW_RPS_FILE_SIZE 1870912
#define FW_HEADER_SIZE   64
#define CHUNK_SIZE       RECV_BUFFER_SIZE //Update this value depending upon 'chunk_size_kb' passed in python script
#define MAX_CHUNK_SIZE   1024

#define SL_STATUS_FW_UPDATE_DONE SL_STATUS_SI91X_NO_AP_FOUND

/******************************************************
 *               Global Variable
 ******************************************************/
uint32_t start, end;

const osThreadAttr_t thread_attributes = {
  .name       = "app",
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = 18432,
  .priority   = osPriorityLow,
  .tz_module  = 0,
  .reserved   = 0,
};

static const sl_wifi_device_configuration_t sl_wifi_firmware_update_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
#ifdef TRANSCEIVER_OPER_MODE
  .region_code = JP,
  .boot_config = { .oper_mode = SL_SI91X_TRANSCEIVER_MODE,
#else
  .region_code = US,
  .boot_config = { .oper_mode = SL_SI91X_CLIENT_MODE,
#endif
                   .coex_mode              = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map        = (SL_SI91X_FEAT_SECURITY_PSK | SL_SI91X_FEAT_AGGREGATION),
                   .tcp_ip_feature_bit_map = (SL_SI91X_TCP_IP_FEAT_BYPASS),
                   .custom_feature_bit_map = (SL_SI91X_CUSTOM_FEAT_EXTENTION_VALID),
                   .ext_custom_feature_bit_map =
                     (SL_SI91X_EXT_FEAT_XTAL_CLK | SL_SI91X_EXT_FEAT_UART_SEL_FOR_DEBUG_PRINTS | MEMORY_CONFIG
#ifdef SLI_SI917
                      | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
                      ),
                   .bt_feature_bit_map         = 0,
                   .ext_tcp_ip_feature_bit_map = SL_SI91X_CONFIG_FEAT_EXTENTION_VALID,
                   .ble_feature_bit_map        = 0,
                   .ble_ext_feature_bit_map    = 0,
                   .config_feature_bit_map     = 0 }
};

static sl_wifi_transceiver_configuration_t transceiver_config = {
  .channel.chan_info = { .channel = SL_CHANNEL_NO, .band = SL_WIFI_BAND_2_4GHZ, .bandwidth = SL_WIFI_BANDWIDTH_20MHz },
  .channel.tx_power  = SL_TX_POWER,
  .parameters        = { .set              = 1,
                         .retransmit_count = DEFAULT_RETRANSMIT_COUNT,
                         .cw_params[0]     = { .cwmin = DEFAULT_QOS_BE_CWMIN,
                                               .cwmax = DEFAULT_QOS_BE_CWMAX,
                                               .aifsn = DEFAULT_QOS_BE_AIFSN },
                         .cw_params[1]     = { .cwmin = DEFAULT_QOS_BK_CWMIN,
                                               .cwmax = DEFAULT_QOS_BK_CWMAX,
                                               .aifsn = DEFAULT_QOS_BK_AIFSN },
                         .cw_params[2]     = { .cwmin = DEFAULT_QOS_VI_CWMIN,
                                               .cwmax = DEFAULT_QOS_VI_CWMAX,
                                               .aifsn = DEFAULT_QOS_VI_AIFSN },
                         .cw_params[3]     = { .cwmin = DEFAULT_QOS_VO_CWMIN,
                                               .cwmax = DEFAULT_QOS_VO_CWMAX,
                                               .aifsn = DEFAULT_QOS_VO_AIFSN } },
};

typedef struct fw_version_s {
  uint8_t build_num;
  uint8_t security_version;
  uint8_t minor;
  uint8_t major;
} fw_version_t;

typedef struct fw_version_ext_s {
  uint8_t patch_num;
  uint8_t customer_id : 4;
  uint8_t build_number_extension : 4;
  uint8_t rom_id;
  uint8_t chip_id;
} fw_version_ext_t;

typedef struct sl_si91x_firmware_header_s {
  uint16_t control_flags;
  uint16_t sha_type;
  uint32_t magic_no;
  uint32_t image_size;
  fw_version_t fw_version;
  uint32_t flash_location;
  uint32_t crc;
  uint32_t mic[4];
  uint32_t counter;
  fw_version_ext_t fw_version_ext;
  uint32_t reserved1[4];
} sl_si91x_firmware_header_t;

/******************************************************
 *               Function Declarations
 ******************************************************/
static void application_start(void *argument);
sl_status_t update_firmware(void);
static sl_status_t sl_wifi_transceiver_initialize(void);
uint32_t get_fw_size(char *buffer);
sl_status_t send_1k_chunks(uint8_t *buffer, size_t total_length);
int16_t com_port_send(char *buffer, uint16_t buffer_length);
int16_t com_port_receive(uint8_t *buffer, uint16_t buffer_length);

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

#ifdef TRANSCEIVER_OPER_MODE
  status = sl_wifi_transceiver_initialize();
  if (status != SL_STATUS_OK) {
    return;
  }
#else
  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &sl_wifi_firmware_update_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    printf("Failed to start Wi-Fi interface: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nWi-Fi Init Success\r\n");
#endif

  status = update_firmware();
  printf("FW UP duration : %ld\n", end - start);
  if (status != SL_STATUS_OK) {
    printf("\r\n Update Firmware failed with status 0x%lx\r\n", status);
    return;
  }

  printf("\r\nFirmware Upgrade Completed\r\n");
  osDelay(1000);
  com_port_send("done", strlen("done"));
}

//! This function gets the size of the firmware
uint32_t get_fw_size(char *buffer)
{
  sl_si91x_firmware_header_t *fw = (sl_si91x_firmware_header_t *)buffer;
  return (fw->image_size + sizeof(sl_si91x_firmware_header_t));
}

// wrapper function to send 1k chunks to firmware
sl_status_t send_1k_chunks(uint8_t *buffer, size_t total_length)
{
  size_t chunk_size       = MAX_CHUNK_SIZE; // 1k chunk size
  size_t remaining_length = total_length;
  uint8_t *current_buffer = buffer;

  while (remaining_length > 0) {
    size_t send_length = (remaining_length > chunk_size) ? chunk_size : remaining_length;

    // Call sl_si91x_fwup_load with the current 1k chunk
    sl_status_t result = sl_si91x_fwup_load(current_buffer, send_length);

    // SL_STATUS_FW_UPDATE_DONE is a valid status code for completion
    if (result == SL_STATUS_FW_UPDATE_DONE) {
      return result;
    }

    if (result != SL_STATUS_OK) {
      printf("\r\nFirmware update failed : %lx\n", result);
      return result;
    }

    // Move to the next chunk
    current_buffer += send_length;
    remaining_length -= send_length;
  }

  // All chunks sent successfully
  return 0;
}

/*==================================================================*/
/**
 * @fn         int16_t com_port_send(uint8_t *ptrBuf,uint16_t buffer_length)
 * @param[in]  uint8 *ptrBuf, pointer to the buffer with the data to be sent
 * @param[in]  uint16 buffer_length, number of bytes to send
 * @param[out] None
 * @return     0, 0=success
 * @section description
 * This API is used to send data to the Si91x module through the UART interface.
 */
int16_t com_port_send(char *buffer, uint16_t buffer_length)
{
  for (int txCount = 0; txCount < buffer_length; txCount++) {
#if defined _SILICON_LABS_32B_SERIES_2_CONFIG_5
    EUSART_Tx(SL_IOSTREAM_EUSART_VCOM_PERIPHERAL, *buffer++);
#elif defined _SILICON_LABS_32B_SERIES_2_CONFIG_1
    USART_Tx(SL_IOSTREAM_USART_VCOM_PERIPHERAL, *buffer++);
#else
    //! Depends upon MCU Tx port
#endif
  }
  return 0;
}

/*==================================================================*/
/**
 * @fn         int16_t com_port_receive(uint8_t *ptrBuf,uint16_t bufLen)
 * @param[in]  uint8_t *ptrBuf, pointer to the buffer with the data to be received
 * @param[in]  uint16_t bufLen, number of bytes to send
 * @param[out] None
 * @return     0, 0=success
 * @description
 * This API is used to receive data from Si91x module through the UART interface.
 */

int16_t com_port_receive(uint8_t *buffer, uint16_t buffer_length)
{
  for (; buffer_length > 0; --buffer_length) {
#if (defined _SILICON_LABS_32B_SERIES_2_CONFIG_5 || defined _SILICON_LABS_32B_SERIES_2_CONFIG_1)
    *buffer++ = getchar();
#else
    //! Depends upon MCU Rx port
#endif
  }
  return 0;
}

#ifdef TRANSCEIVER_OPER_MODE
static sl_status_t sl_wifi_transceiver_initialize(void)
{
  sl_status_t status;

  status = sl_wifi_init(&sl_wifi_firmware_update_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    printf("\r\nTransceiver initialization failed: 0x%lX\r\n", status);
    return status;
  }
  printf("\r\nTransceiver initialization successful\r\n");

  status = sl_wifi_transceiver_up(SL_WIFI_TRANSCEIVER_INTERFACE, &transceiver_config);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to bring up Transceiver interface: 0x%lX\r\n", status);
    return status;
  }
  printf("\r\nTransceiver interface up\r\n");

  return status;
}
#endif

sl_status_t update_firmware()
{
  sl_wifi_firmware_version_t version = { 0 };
  sl_status_t status                 = SL_STATUS_FAIL;
  uint16_t fwup_chunk_length         = 0;
  uint8_t fwup_chunk_type            = 1; //Set to Header
  uint32_t no_of_chunk               = 0;
  uint32_t no_of_remaining_bytes     = 0;
  uint32_t fw_image_size             = 0;
  uint16_t i                         = 1;
  uint8_t recv_buffer[RECV_BUFFER_SIZE];

  status = sl_wifi_get_firmware_version(&version);
  VERIFY_STATUS_AND_RETURN(status);
  print_firmware_version(&version);

  printf("\r\nFirmware update start\r\n");
  start = osKernelGetTickCount();
  while (1) {

    // Call corresponding firmware upgrade API based on the chunk type
    if (fwup_chunk_type == SL_FWUP_RPS_HEADER) {
      osDelay(1000);
      memset(recv_buffer, 0, sizeof(recv_buffer));
      com_port_send("header", strlen("header"));
      com_port_receive(recv_buffer, sizeof(sl_si91x_firmware_header_t));

      //! Send the first chunk to extract header
      fw_image_size         = get_fw_size((char *)recv_buffer);
      no_of_chunk           = (fw_image_size / CHUNK_SIZE);
      no_of_remaining_bytes = (fw_image_size % CHUNK_SIZE);

      // Send RPS header which is received as first chunk
      status = sl_si91x_fwup_start(recv_buffer);

      //update chunk_type flag
      fwup_chunk_type = SL_FWUP_RPS_CONTENT;
    }

    else {
      memset(recv_buffer, 0, sizeof(recv_buffer));
      fwup_chunk_length = (i <= no_of_chunk) ? CHUNK_SIZE : no_of_remaining_bytes;

      com_port_send("data", strlen("data"));
      com_port_receive(recv_buffer, fwup_chunk_length);

      // Send RPS content
      status = send_1k_chunks(recv_buffer, fwup_chunk_length);
      i++;
    }

    if (status != SL_STATUS_OK) {
      if (status == SL_STATUS_FW_UPDATE_DONE) {
        end = osKernelGetTickCount();

        printf("\r\nFirmware update complete\r\n");

#ifdef TRANSCEIVER_OPER_MODE
        status = sl_wifi_deinit();
#else
        status = sl_net_deinit(SL_NET_WIFI_CLIENT_INTERFACE);
#endif
        printf("\r\nWi-Fi Deinit status : %lx\r\n", status);
        VERIFY_STATUS_AND_RETURN(status);

        printf("\r\nWaiting for init response...\r\n");

#ifdef TRANSCEIVER_OPER_MODE
        status = sl_wifi_transceiver_initialize();
        VERIFY_STATUS_AND_RETURN(status);
#else
        status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &sl_wifi_firmware_update_configuration, NULL, NULL);
        printf("\r\nWi-Fi Init status : %lx\r\n", status);
        VERIFY_STATUS_AND_RETURN(status);
#endif
        status = sl_wifi_get_firmware_version(&version);
        VERIFY_STATUS_AND_RETURN(status);

        print_firmware_version(&version);

        return SL_STATUS_OK;
      } else {
        printf("\r\nFirmware update failed : %lx\n", status);
        end = osKernelGetTickCount();

        return SL_STATUS_FAIL;
      }
    }
  }
  return SL_STATUS_OK;
}
