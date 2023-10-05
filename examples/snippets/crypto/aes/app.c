/***************************************************************************/ /**
 * @file
 * @brief Crypto AES Application
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

#include <string.h>
#include "cmsis_os2.h"
#include "sl_net.h"
#include "sl_wifi.h"
#include "sl_si91x_aes.h"
#include "sl_si91x_wrap.h"

/******************************************************
 *                    Constants
 ******************************************************/
#define BUFFER_SIZE      16
#define USE_WRAPPED_KEYS 0

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

static const sl_wifi_device_configuration_t client_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .region_code = US,
  .boot_config = { .oper_mode       = SL_SI91X_CLIENT_MODE,
                   .coex_mode       = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map = (SL_SI91X_FEAT_SECURITY_PSK | SL_SI91X_FEAT_AGGREGATION
#ifdef RSI_M4_INTERFACE
                                       | SL_SI91X_FEAT_WPS_DISABLE
#endif
                                       ),
                   .tcp_ip_feature_bit_map     = (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT),
                   .custom_feature_bit_map     = (SL_SI91X_FEAT_CUSTOM_FEAT_EXTENTION_VALID),
                   .ext_custom_feature_bit_map = (
#ifdef RSI_M4_INTERFACE
                     RAM_LEVEL_NWP_ADV_MCU_BASIC
#else
                     RAM_LEVEL_NWP_ALL_MCU_ZERO
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

uint8_t encrypted_buffer[BUFFER_SIZE];
uint8_t decrypted_buffer[BUFFER_SIZE];

uint8_t msg[BUFFER_SIZE] = { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
                             0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a };

uint8_t key[SL_SI91X_AES_KEY_SIZE_256] = { 0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae,
                                           0xf0, 0x85, 0x7d, 0x77, 0x81, 0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61,
                                           0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4 };

uint8_t wrapped_key[SL_SI91X_AES_KEY_SIZE_256];

// IV used for SL_SI91X_AES_CBC and SL_SI91X_AES_CTR modes.
uint8_t iv[SL_SI91X_IV_SIZE] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };

sl_si91x_wrap_config_t wrap_config = { 0 };

/******************************************************
 *               Function Declarations
 ******************************************************/
static void application_start(void *argument);
static sl_status_t aes_encryption(void);
static sl_status_t aes_decryption(void);

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

  sl_status_t status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &client_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    printf("Failed to start Wi-Fi client interface: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nWi-Fi Init Success\r\n");

  status = aes_encryption();
  if (status != SL_STATUS_OK) {
    return;
  }

  status = aes_decryption();
  if (status != SL_STATUS_OK) {
    return;
  }
}

sl_status_t aes_encryption(void)
{
  sl_status_t status;
  sl_si91x_aes_config_t config = { 0 };

  config.aes_mode                   = SL_SI91X_AES_ECB;
  config.encrypt_decrypt            = SL_SI91X_AES_ENCRYPT;
  config.msg                        = msg;
  config.msg_length                 = sizeof(msg);
  config.iv                         = NULL;
  config.key_config.b0.key_size     = SL_SI91X_AES_KEY_SIZE_256;
  config.key_config.b0.key_slot     = 0;
  config.key_config.b0.wrap_iv_mode = SL_SI91X_WRAP_IV_ECB_MODE;
  config.key_config.b0.key_type     = SL_SI91X_TRANSPARENT_KEY;
  memcpy(config.key_config.b0.key_buffer, &key, config.key_config.b0.key_size);

#if USE_WRAPPED_KEYS
  wrap_config.key_type     = SL_SI91X_TRANSPARENT_KEY;
  wrap_config.key_size     = SL_SI91X_AES_KEY_SIZE_256;
  wrap_config.wrap_iv_mode = SL_SI91X_WRAP_IV_ECB_MODE;
  memcpy(wrap_config.key_buffer, key, wrap_config.key_size);

  status = sl_si91x_wrap(&wrap_config, wrapped_key);
  if (status != SL_STATUS_OK) {
    printf("\r\nWrap failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  printf("\r\nWrap success\r\n");

  config.key_config.b0.key_type = SL_SI91X_WRAPPED_KEY;
  memcpy(config.key_config.b0.key_buffer, &wrapped_key, config.key_config.b0.key_size);
#endif

  status = sl_si91x_aes(&config, encrypted_buffer);
  if (status != SL_STATUS_OK) {
    printf("\r\nAES encryption failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  printf("\r\nAES encryption success\r\n");

  return status;
}

sl_status_t aes_decryption(void)
{
  sl_status_t status;
  sl_si91x_aes_config_t config = { 0 };

  config.aes_mode                   = SL_SI91X_AES_ECB;
  config.encrypt_decrypt            = SL_SI91X_AES_DECRYPT;
  config.msg                        = encrypted_buffer;
  config.msg_length                 = sizeof(msg);
  config.iv                         = NULL;
  config.key_config.b0.key_size     = SL_SI91X_AES_KEY_SIZE_256;
  config.key_config.b0.key_slot     = 0;
  config.key_config.b0.wrap_iv_mode = SL_SI91X_WRAP_IV_ECB_MODE;
  config.key_config.b0.key_type     = SL_SI91X_TRANSPARENT_KEY;
  memcpy(config.key_config.b0.key_buffer, &key, config.key_config.b0.key_size);

#if USE_WRAPPED_KEYS
  config.key_config.b0.key_type = SL_SI91X_WRAPPED_KEY;
  memcpy(config.key_config.b0.key_buffer, &wrapped_key, config.key_config.b0.key_size);
#endif

  status = sl_si91x_aes(&config, decrypted_buffer);
  if (status != SL_STATUS_OK) {
    printf("\r\nAES decryption failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  printf("\r\nAES decryption success\r\n");

  return status;
}
