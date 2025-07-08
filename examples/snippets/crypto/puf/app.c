/***************************************************************************/ /**
 * @file app.c
 * @brief PUF Example Application
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

#include <string.h>
#include "cmsis_os2.h"
#include "sl_net.h"
#include "sl_net_wifi_types.h"
#include "sl_wifi.h"
#include "sl_status.h"
#include "sl_si91x_constants.h"
#include "sl_si91x_types.h"
#include "sl_si91x_puf.h"
#include "sl_si91x_crypto_utility.h"

/******************************************************
 *                    Constants
 ******************************************************/
//! User Key to be used to create keycode(Key length can be 16 or 32 bytes)
#define USER_KEY "123456abcdefghij"
//! User input data to be encrypted
#define AES_PLAIN_TXT "Silabs"
//! AES CBC mode Initailization Vector
#define AES_CBC_IV "abcdefghijklmnop"
//! Buffer size for output data. Adjust based on input message length.
#define AES_BUFFER_SIZE 256
//! PUF Demo type
#define PUF_GET_KEY_DEMO 0
//! Block size for alignment
#define AES_BLOCK_SIZE 16
//! Input message padding for block alignment
#define PKCS_7_PADDING 1

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
#ifdef SLI_SI91X_MCU_INTERFACE
                                       | SL_SI91X_FEAT_WPS_DISABLE
#endif
                                       ),
                   .tcp_ip_feature_bit_map     = (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT),
                   .custom_feature_bit_map     = (SL_SI91X_CUSTOM_FEAT_EXTENTION_VALID),
                   .ext_custom_feature_bit_map = (MEMORY_CONFIG | SL_SI91X_EXT_FEAT_PUF
#ifdef SLI_SI917
                                                  | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
                                                  ),
                   .bt_feature_bit_map         = 0,
                   .ext_tcp_ip_feature_bit_map = 0,
                   .ble_feature_bit_map        = 0,
                   .ble_ext_feature_bit_map    = 0,
                   .config_feature_bit_map     = 0 }
};

//! AES Encrypted and Decrypted data
uint8_t aes_encry_data[AES_BUFFER_SIZE];
uint8_t aes_decry_data[AES_BUFFER_SIZE];

#if PKCS_7_PADDING
size_t padded_data_length = 0;
uint8_t decrypted_unpadded_data_buffer[AES_BUFFER_SIZE];
#endif

/******************************************************
 *               Function Declarations
 ******************************************************/
static void application_start(void *argument);
static sl_status_t puf_aes_encrypt_decrypt_app(void);

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
  printf("\r\n Wi-Fi Init Success\r\n");

  status = puf_aes_encrypt_decrypt_app();
  if (status != SL_STATUS_OK) {
    return;
  }
  printf("\r\n Example Demonstration Completed\r\n");
}

sl_status_t puf_aes_encrypt_decrypt_app(void)
{
  sl_status_t status;

  uint8_t keycode[52];

  status = sl_si91x_puf_start_req();
  if (status != SL_STATUS_OK) {
    printf("\r\n PUF Start Fail Status 0x%lx\r\n", status);
    return status;
  }
  printf("\r\n PUF Start Success \n");

#if PUF_GET_KEY_DEMO
  /* PUF Get key demo operation of 32 Bytes(SL_SI91X_PUF_KEY_SIZE_256) Key */

  uint8_t get_key[32];

  // Set Key(32Bytes) operation with PUF
  status = sl_si91x_puf_set_key_req(1, SL_SI91X_PUF_KEY_SIZE_256, (uint8_t *)USER_KEY, keycode);
  if (status != SL_STATUS_OK) {
    printf("\r\n Set Key Fail Status 0x%lx\r\n", status);
    return status;
  }
  printf("\r\n PUF set key Success 32Bytes Size \n");

  // Get Key(16Bytes) operation with PUF
  status = sl_si91x_puf_get_key_req(keycode, get_key);
  if (status != SL_STATUS_OK) {
    printf("\r\n Get Key Fail Status 0x%lx\r\n", status);
    return status;
  }
  printf("\r\n PUF Get key Success 32Bytes Size \n");

  return status;
#endif

  /* PUF operation with 16 Bytes(SL_SI91X_PUF_KEY_SIZE_128) Key */

  // Set Key(16Bytes) operation with PUF
  status = sl_si91x_puf_set_key_req(0, SL_SI91X_PUF_KEY_SIZE_128, (uint8_t *)USER_KEY, keycode);
  if (status != SL_STATUS_OK) {
    printf("\r\n Set Key Fail Status 0x%lx\r\n", status);
    return status;
  }
  printf("\r\n PUF set key Success 16Bytes Size \n");

  // Set Intrinsic Key operation with PUF
  uint8_t intr_key_resp[52];

  for (uint32_t i = 0; i < 5; i++) {
    status = sl_si91x_puf_set_intr_key_req(0, SL_SI91X_PUF_KEY_SIZE_128, intr_key_resp);
    if (status != SL_STATUS_OK) {
      printf("\r\n Set Intrinsic Key Fail Status 0x%lx\r\n", status);
      return status;
    }
    printf("\r\n PUF set Intrinsic key Success 16Bytes Size \n");

    // Print the response
    printf("\r\nRandom Intrinsic Key Response %ld: ", i + 1);
    for (uint32_t j = 0; j < sizeof(intr_key_resp); j++) {
      printf("%02x", intr_key_resp[j]);
    }
    printf("\n");
  }
  // Load Key(16Bytes) operation with PUF
  status = sl_si91x_puf_load_key_req(keycode);
  if (status != SL_STATUS_OK) {
    printf("\r\n Load Key to PUF/AES Fail Status 0x%lx\r\n", status);
    return status;
  }
  printf("\r\n PUF Load key Success 16Bytes Size \n");

  //!Encrypt using AES ECB mode

#if PKCS_7_PADDING
  size_t padded_data_length = 0;
  uint8_t *padded_data =
    pkcs7_padding((uint8_t *)AES_PLAIN_TXT, strlen(AES_PLAIN_TXT), AES_BLOCK_SIZE, &padded_data_length);
  if (!padded_data) {
    return SL_STATUS_FAIL;
  }
#endif

  memset(aes_encry_data, 0, AES_BUFFER_SIZE);
  status = sl_si91x_puf_aes_encrypt_req(SL_SI91X_AES_ECB_MODE,
                                        SL_SI91X_PUF_AS_KEY_SOURCE,
                                        SL_SI91X_PUF_KEY_SIZE_128,
                                        NULL,
#if PKCS_7_PADDING
                                        padded_data_length,
                                        padded_data,
#else
                                        AES_BUFFER_SIZE,
                                        (uint8_t *)AES_PLAIN_TXT,
#endif
                                        0,
                                        NULL,
                                        aes_encry_data);
  if (status != SL_STATUS_OK) {
    printf("\r\n AES Encryption Fail Status 0x%lx\r\n", status);
#if PKCS_7_PADDING
    free(padded_data);
#endif
    return status;
  }

  printf("\r\n Encryption Done with AES ECB 16bytes key Data Size 256\n");

  //! Decryption of data AES ECB mode
  memset(aes_decry_data, 0, AES_BUFFER_SIZE);
  status = sl_si91x_puf_aes_decrypt_req(SL_SI91X_AES_ECB_MODE,
                                        SL_SI91X_PUF_AS_KEY_SOURCE,
                                        SL_SI91X_PUF_KEY_SIZE_128,
                                        NULL,
#if PKCS_7_PADDING
                                        padded_data_length,
#else
                                        AES_BUFFER_SIZE,
#endif
                                        aes_encry_data,
                                        0,
                                        NULL,
                                        aes_decry_data);
  if (status != SL_STATUS_OK) {
    printf("\r\n AES Decryption Fail Status 0x%lx\r\n", status);
#if PKCS_7_PADDING
    free(padded_data);
#endif
    return status;
  }

#if PKCS_7_PADDING
  int32_t unpadded_data_length = pkcs7_unpad(aes_decry_data, padded_data_length);
  if (unpadded_data_length < 0) {
    free(padded_data);
    return SL_STATUS_FAIL;
  }
  memset(decrypted_unpadded_data_buffer, 0, AES_BUFFER_SIZE);
  memcpy(decrypted_unpadded_data_buffer, aes_decry_data, unpadded_data_length);
#endif

  printf("\r\n Decryption Done with AES ECB 16bytes key Size\n");

#if PKCS_7_PADDING
  free(padded_data);
#endif

  //!Encrypt using AES CBC mode 128bit Key

#if PKCS_7_PADDING
  padded_data = pkcs7_padding((uint8_t *)AES_PLAIN_TXT, strlen(AES_PLAIN_TXT), AES_BLOCK_SIZE, &padded_data_length);
  if (!padded_data) {
    return SL_STATUS_FAIL;
  }
#endif

  memset(aes_encry_data, 0, AES_BUFFER_SIZE);
  status = sl_si91x_puf_aes_encrypt_req(SL_SI91X_AES_CBC_MODE,
                                        SL_SI91X_PUF_AS_KEY_SOURCE,
                                        SL_SI91X_PUF_KEY_SIZE_128,
                                        NULL,
#if PKCS_7_PADDING
                                        padded_data_length,
                                        padded_data,
#else
                                        AES_BUFFER_SIZE,
                                        (uint8_t *)AES_PLAIN_TXT,
#endif
                                        SL_SI91X_PUF_IV_SIZE_128,
                                        (uint8_t *)AES_CBC_IV,
                                        aes_encry_data);
  if (status != SL_STATUS_OK) {
    printf("\r\n AES Encryption Fail Status 0x%lx\r\n", status);
#if PKCS_7_PADDING
    free(padded_data);
#endif
    return status;
  }
  printf("\r\n Encryption Done with AES CBC 16bytes key Data Size 256 \n");

  //!Decrypt using AES CBC mode 128bit Key
  memset(aes_decry_data, 0, AES_BUFFER_SIZE);
  status = sl_si91x_puf_aes_decrypt_req(SL_SI91X_AES_CBC_MODE,
                                        SL_SI91X_PUF_AS_KEY_SOURCE,
                                        SL_SI91X_PUF_KEY_SIZE_128,
                                        NULL,
#if PKCS_7_PADDING
                                        padded_data_length,
#else
                                        AES_BUFFER_SIZE,
#endif
                                        aes_encry_data,
                                        0,
                                        (uint8_t *)AES_CBC_IV,
                                        aes_decry_data);
  if (status != SL_STATUS_OK) {
    printf("\r\n AES Decryption Fail Status 0x%lx\r\n", status);
#if PKCS_7_PADDING
    free(padded_data);
#endif
    return status;
  }

#if PKCS_7_PADDING
  unpadded_data_length = pkcs7_unpad(aes_decry_data, padded_data_length);
  if (unpadded_data_length < 0) {
    free(padded_data);
    return SL_STATUS_FAIL;
  }
  memset(decrypted_unpadded_data_buffer, 0, AES_BUFFER_SIZE);
  memcpy(decrypted_unpadded_data_buffer, aes_decry_data, unpadded_data_length);
#endif

  printf("\r\n Decryption Done with AES CBC 16bytes key Size \n");

#if PKCS_7_PADDING
  if (memcmp((uint8_t *)AES_PLAIN_TXT, decrypted_unpadded_data_buffer, unpadded_data_length) == 0) {
    printf("\r\n AES CBC Encryption and Decryption Success \n");
  } else {
    printf("\r\n AES CBC Encryption and Decryption Mismatch \n");
  }
#else
  if (memcmp((uint8_t *)AES_PLAIN_TXT, aes_decry_data, strlen(AES_PLAIN_TXT)) == 0) {
    printf("\r\n AES CBC Encryption and Decryption Success \n");
  } else {
    printf("\r\n AES CBC Encryption and Decryption Mismatch \n");
  }
#endif

  //! Calculate MAC using AES CBC mode 128bit Key
  uint8_t aes_mac_resp[16]; // Buffer to store the MAC response

  status = sl_si91x_puf_aes_mac_req(SL_SI91X_PUF_AS_KEY_SOURCE,
                                    SL_SI91X_PUF_KEY_SIZE_128,
                                    NULL,
#if PKCS_7_PADDING
                                    padded_data_length,
                                    padded_data,
#else
                                    AES_BUFFER_SIZE,
                                    (uint8_t *)AES_PLAIN_TXT,
#endif
                                    SL_SI91X_PUF_IV_SIZE_128,
                                    (uint8_t *)AES_CBC_IV,
                                    aes_mac_resp);
  if (status != SL_STATUS_OK) {
    printf("\r\n AES MAC Calculation Fail Status 0x%lx\r\n", status);
#if PKCS_7_PADDING
    free(padded_data);
#endif
    return status;
  }

  printf("\r\n AES MAC Calculation Done with AES CBC 16bytes key Size \n");

  // Print the MAC response
  printf("\r\n AES MAC Response: ");
  for (uint32_t i = 0; i < sizeof(aes_mac_resp); i++) {
    printf("%02x", aes_mac_resp[i]);
  }
  printf("\n");

#if PKCS_7_PADDING
  free(padded_data);
#endif

  return status;
}
