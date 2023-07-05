/*******************************************************************************
 * @file
 * @brief OTA Firmware Upgradation Example Application
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
#include "sl_wifi.h"
#include "sl_net.h"
#include "sl_wifi_callback_framework.h"
#include "sl_net_rsi_utility.h"
#include "sl_si91x_driver.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
//! Server IP address.
#define OTAF_SERVER_IP_ADDRESS "192.168.0.213"

#define ASYNCHRONOUS_FW_UPDATE 0

//! OTAF Server port number
#define OTAF_SERVER_PORT 5001

//! OTAF TCP receive timeout
#define OTAF_RX_TIMEOUT 200

//! OTAF TCP retry count
#define OTAF_TCP_RETRY_COUNT 20

//! OTA Firmware upgrade retry count
#define OTAF_RETRY_COUNT 10

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

static const sl_wifi_device_configuration_t station_otaf_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .region_code = US,
  .boot_config = { .oper_mode              = SL_SI91X_CLIENT_MODE,
                   .coex_mode              = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map        = (SL_SI91X_FEAT_SECURITY_PSK | SL_SI91X_FEAT_AGGREGATION),
                   .tcp_ip_feature_bit_map = (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT | SL_SI91X_TCP_IP_FEAT_OTAF
                                              | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID | SL_SI91X_TCP_IP_FEAT_DNS_CLIENT),
                   .custom_feature_bit_map = SL_SI91X_FEAT_CUSTOM_FEAT_EXTENTION_VALID,
                   .ext_custom_feature_bit_map =
                     (SL_SI91X_EXT_FEAT_XTAL_CLK_ENABLE(2) | SL_SI91X_EXT_FEAT_UART_SEL_FOR_DEBUG_PRINTS
#ifndef RSI_M4_INTERFACE
                      | RAM_LEVEL_NWP_ALL_MCU_ZERO
#else
                      | RAM_LEVEL_NWP_ADV_MCU_BASIC
#endif
                      ),
                   .bt_feature_bit_map         = 0,
                   .ext_tcp_ip_feature_bit_map = 0,
                   .ble_feature_bit_map        = 0,
                   .ble_ext_feature_bit_map    = 0,
                   .config_feature_bit_map     = 0 }
};

volatile uint8_t response_received;

//! OTAF retry count
volatile uint16_t otaf_retry_count;
static uint16_t chunk_number = 1;

/******************************************************
 *               Function Declarations
 ******************************************************/
static void application_start(void *argument);
#if ASYNCHRONOUS_FW_UPDATE
static sl_status_t ota_fw_up_response_handler(uint32_t event, sl_status_t status, void *data, uint32_t data_length);
#endif
sl_status_t update_firmware(void);

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

  status = sl_net_init(SL_NET_DEFAULT_WIFI_CLIENT_INTERFACE, &station_otaf_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to start Wi-Fi Client interface: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nWi-Fi Init success\r\n");

  status = sl_net_up(SL_NET_DEFAULT_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to connect to AP: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nWi-Fi client connected\r\n");

  status = update_firmware();
  if (status != SL_STATUS_OK) {
    printf("\r\n Update Firmware failed with status 0x%lx\r\n", status);
  }

  printf("\r\nFirmware Update Demonstration Completed\r\n");
}

sl_status_t update_firmware()
{
  sl_status_t status               = SL_STATUS_FAIL;
  sl_wifi_version_string_t version = { 0 };
  sl_ip_address_t server_ip        = { 0 };

  sl_net_inet_addr(OTAF_SERVER_IP_ADDRESS, &server_ip.ip.v4.value);
  server_ip.type = SL_IPV4;

  status = sl_wifi_get_firmware_version(&version);
  VERIFY_STATUS_AND_RETURN(status);

  printf("\r\nFirmware version before update: %s\r\n", version.version);

  printf("\r\nFirmware update start\r\n");

#if ASYNCHRONOUS_FW_UPDATE
  sl_si91x_register_callback(SL_NET_OTA_FW_UPDATE_EVENT, ota_fw_up_response_handler);
#endif

  do {
    status = sl_si91x_ota_firmware_upgradation(server_ip,
                                               OTAF_SERVER_PORT,
                                               chunk_number,
                                               OTAF_RX_TIMEOUT,
                                               OTAF_TCP_RETRY_COUNT,
#if ASYNCHRONOUS_FW_UPDATE
                                               true);
    if (status != SL_STATUS_IN_PROGRESS) {
      printf("\r\nOTA Firmware Update Failed, Error Code : 0x%lX\r\n", status);
      return status;
    }

    //! wait for the Firmware response
    int i = 0;
    do {
      //! event loop
      i++;
      if (i > 300) {
        break;
      }
      osDelay(1000);
    } while (!response_received);

#else
                                               false);
    if (status != SL_STATUS_OK) {
      printf("\r\nOTA Firmware Update Failed, Error Code : 0x%lX\r\n", status);
      return status;
    }

    response_received = 1;
#endif

    //! Check for FWUP success
    if (response_received == 1) {
      break;
    }

    //! Check for FWUP failure
    if ((response_received == 2) && (otaf_retry_count)) {
      otaf_retry_count--;
    }

    //! Check for FWUP failure  with retry count
    if ((response_received == 2) && (otaf_retry_count == 0)) {
      return SL_STATUS_FAIL;
    }

    //! reset rsp received
    response_received = 0;

  } while (otaf_retry_count);

  printf("\r\nOTA Firmware Update success\r\n");
  osDelay(5000);
  status = sl_net_deinit(SL_NET_DEFAULT_WIFI_CLIENT_INTERFACE, NULL);
  VERIFY_STATUS_AND_RETURN(status);
  printf("\r\nWi-Fi Deinit success\r\n");

  osDelay(5000);
  status = sl_net_init(SL_NET_DEFAULT_WIFI_CLIENT_INTERFACE, &sl_wifi_default_client_configuration, NULL, NULL);
  VERIFY_STATUS_AND_RETURN(status);
  printf("\r\nWi-Fi Init success\r\n");

  status = sl_wifi_get_firmware_version(&version);
  VERIFY_STATUS_AND_RETURN(status);
  printf("\r\nFirmware version after update: %s\r\n", version.version);

  return SL_STATUS_OK;
}

#if ASYNCHRONOUS_FW_UPDATE
//! OTAF respone handler
static sl_status_t ota_fw_up_response_handler(uint32_t event, sl_status_t status, void *data, uint32_t data_length)
{
  UNUSED_PARAMETER(data_length);
  if (status != SL_STATUS_OK) {
    printf("Event %ld failed with status : %lx\n", event, status);
    response_received = 2;
    chunk_number      = *(uint16_t *)data;
    return status;
  }

  response_received = 1;
  printf("Event %ld received successfully\n", event);
  return SL_STATUS_OK;
}
#endif
