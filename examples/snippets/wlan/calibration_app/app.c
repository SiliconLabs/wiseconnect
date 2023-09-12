/*******************************************************************************
 * @file
 * @brief Calibration Application
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_utility.h"
#include <string.h>
#include "sl_si91x_driver.h"

/******************************************************
 *                      Macros
 ******************************************************/
/******************************************************
 *                    Constants
 ******************************************************/
#define BURST_MODE      0
#define CONTINUOUS_MODE 1
#define TX_TEST_POWER   18
#define TX_TEST_RATE    0
#define TX_TEST_LENGTH  30
#define TX_TEST_MODE    BURST_MODE
#define TEST_CHANNEL    1

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

static const sl_wifi_device_configuration_t sl_wifi_calibration_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .region_code = US,
  .boot_config = { .oper_mode              = SL_SI91X_TRANSMIT_TEST_MODE,
                   .coex_mode              = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map        = (SL_SI91X_FEAT_SECURITY_PSK | SL_SI91X_FEAT_AGGREGATION),
                   .tcp_ip_feature_bit_map = (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT),
                   .custom_feature_bit_map = (SL_SI91X_FEAT_CUSTOM_FEAT_EXTENTION_VALID),
                   .ext_custom_feature_bit_map =
                     (SL_SI91X_EXT_FEAT_XTAL_CLK | SL_SI91X_EXT_FEAT_UART_SEL_FOR_DEBUG_PRINTS
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
                   .ext_tcp_ip_feature_bit_map = SL_SI91X_CONFIG_FEAT_EXTENTION_VALID,
                   .ble_feature_bit_map        = 0,
                   .ble_ext_feature_bit_map    = 0,
                   .config_feature_bit_map     = 0 }
};

/******************************************************
 *               Function Declarations
 ******************************************************/
static void application_start(void *argument);
sl_status_t test_calibration(void);

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

  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &sl_wifi_calibration_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    printf("Failed to start Wi-Fi client interface: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nWi-Fi Init Success\r\n");

  status = test_calibration();
  if (status != SL_STATUS_OK) {
    printf("\r\n Calibration test failed with status 0x%lx\r\n", status);
    return;
  }

  printf("\r\n Calibration test Completed\r\n");
}

sl_status_t test_calibration()
{
  sl_status_t status;
  sl_si91x_calibration_read_t target         = { 0 };
  sl_si91x_calibration_read_t calib_read_pkt = { 0 };
  sl_si91x_calibration_write_t calib_pkt     = { 0 };
  sl_si91x_freq_offset_t freq_calib_pkt      = { 0 };
  sl_si91x_evm_offset_t evm_offset_pkt       = { 0 };
  sl_si91x_evm_write_t evm_write_pkt         = { 0 };
  sl_si91x_efuse_read_t efuse_read_pkt       = { 0 };
  uint8_t *efuse_read_buf                    = NULL;

  target.target      = 1;
  calib_pkt.target   = 1;
  calib_pkt.flags    = 6;
  calib_pkt.xo_ctune = 12;

  freq_calib_pkt.frequency_offset_in_khz = 30;

  evm_offset_pkt.evm_offset_val = 12;
  evm_offset_pkt.evm_index      = 1;

  evm_write_pkt.target                              = 1;
  evm_write_pkt.flags                               = 2;
  evm_write_pkt.evm_offset_11G_6M_24M_11N_MCS0_MCS2 = 10;

  efuse_read_pkt.efuse_read_addr_offset = 602;
  efuse_read_pkt.efuse_read_data_len    = 20;

  status = sl_si91x_transmit_test_start(TX_TEST_POWER, TX_TEST_RATE, TX_TEST_LENGTH, TX_TEST_MODE, TEST_CHANNEL);
  if (status != SL_STATUS_OK) {
    printf("Transmit test start failed: 0x%lx\r\n", status);
    return status;
  }

  status = sl_si91x_calibration_write(calib_pkt);
  if (status != SL_STATUS_OK) {
    printf("calib write failed: 0x%lx\r\n", status);
    return status;
  }
  printf("calib write success:0x%lx\r\n", status);

  status = sl_si91x_calibration_read(target, &calib_read_pkt);
  if (status != SL_STATUS_OK) {
    printf("calib read failed: 0x%lx\r\n", status);
    return status;
  }
  printf("target %d, reserved:%d "
         "gain_offset_low:%d, gain_offset_2:%d, gain_offset_3:%d,xo_tune:%d\r\n",
         calib_read_pkt.target,
         calib_read_pkt.reserved0[0],
         calib_read_pkt.gain_offset[0],
         calib_read_pkt.gain_offset[1],
         calib_read_pkt.gain_offset[2],
         calib_read_pkt.xo_ctune);

  printf("calib read success:0x%lx\r\n", status);

  status = sl_si91x_frequency_offset(&freq_calib_pkt);
  if (status != SL_STATUS_OK) {
    printf("freq write failed: 0x%lx\r\n", status);
    return status;
  }
  printf("freq write success:0x%lx\r\n", status);

  status = sl_si91x_evm_offset(&evm_offset_pkt);
  if (status != SL_STATUS_OK) {
    printf("evm offset failed: 0x%lx\r\n", status);
    return status;
  }
  printf("evm offset success:0x%lx\r\n", status);

  status = sl_si91x_evm_write(&evm_write_pkt);
  if (status != SL_STATUS_OK) {
    printf("evm write failed: 0x%lx\r\n", status);
    return status;
  }
  printf("evm write success:0x%lx\r\n", status);

  efuse_read_buf = (uint8_t *)malloc(sizeof(uint8_t) * efuse_read_pkt.efuse_read_data_len);

  status = sl_si91x_efuse_read(&efuse_read_pkt, efuse_read_buf);
  if (status != SL_STATUS_OK) {
    printf("efuse read failed: 0x%lx\r\n", status);
    return status;
  }

  for (int i = 0; i < efuse_read_pkt.efuse_read_data_len; i++) {
    printf("%d\r", efuse_read_buf[i]);
  }
  printf("\r\n efuse read success:0x%lx\r\n", status);

  free(efuse_read_buf);
  return status;
}
