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
#include "cmsis_os2.h"
#include "sl_net.h"
#include "sl_si91x_aes.h"
#include "sl_wifi.h"
#include <string.h>

#ifdef SL_SI91X_SIDE_BAND_CRYPTO
/*
 * Sideband multipart AES uses the init/update/final entry points declared
 * in sl_si91x_mp_aes.h.  The legacy sl_si91x_aes_multipart symbol is
 * intentionally absent under SL_SI91X_SIDE_BAND_CRYPTO; this example
 * dispatches between the two surfaces below at compile time.
 */
#include "sl_si91x_mp_aes.h"
#endif
#include "rsi_debug.h"
#include "sl_si91x_crypto_utility.h"
#include "sl_si91x_wrap.h"

/******************************************************
 *                    Constants
 ******************************************************/
#define BUFFER_SIZE      1024 // Update the buffer size based on the message size.
#define USE_WRAPPED_KEYS 0
#define USE_MULTIPART    0
#define PKCS_7_PADDING   0

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

static const sl_wifi_device_configuration_t client_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .region_code = US,
  .boot_config = { .oper_mode       = SL_SI91X_CLIENT_MODE,
                   .coex_mode       = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map = (SL_WIFI_FEAT_SECURITY_PSK | SL_WIFI_FEAT_AGGREGATION
#ifdef SLI_SI91X_MCU_INTERFACE
                                       | SL_WIFI_FEAT_WPS_DISABLE
#endif
                                       ),
                   .tcp_ip_feature_bit_map     = (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT),
                   .custom_feature_bit_map     = (SL_WIFI_SYSTEM_CUSTOM_FEAT_EXTENSION_VALID),
                   .ext_custom_feature_bit_map = (MEMORY_CONFIG
#ifdef SLI_SI917
                                                  | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
                                                  ),
                   .bt_feature_bit_map         = 0,
                   .ext_tcp_ip_feature_bit_map = 0,
                   .ble_feature_bit_map        = 0,
                   .ble_ext_feature_bit_map    = 0,
                   .config_feature_bit_map     = 0 },
  .ta_pool         = { .tx_ratio_in_buffer_pool = 0, .rx_ratio_in_buffer_pool = 0, .global_ratio_in_buffer_pool = 0 },
  .efuse_data_type = SL_SI91X_EFUSE_MFG_SW_VERSION,
  .nwp_fw_image_number = SL_SI91X_NWP_FW_IMAGE_NUMBER_0
};

uint8_t encrypted_buffer[BUFFER_SIZE];
uint8_t decrypted_buffer[BUFFER_SIZE];

uint8_t msg[1024] = { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a };

uint8_t key[SL_SI91X_AES_KEY_SIZE_256] = { 0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae,
                                           0xf0, 0x85, 0x7d, 0x77, 0x81, 0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61,
                                           0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4 };

uint8_t wrapped_key[SL_SI91X_AES_KEY_SIZE_256];

// IV used for SL_SI91X_AES_CBC and SL_SI91X_AES_CTR modes.
uint8_t iv[SL_SI91X_IV_SIZE] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };

sl_si91x_wrap_config_t wrap_config = { 0 };

int sent_data = 0;

#if PKCS_7_PADDING
size_t padded_data_length = 0;
uint8_t decrypted_unpadded_data_buffer[BUFFER_SIZE];
#endif

/******************************************************
 *               Function Declarations
 ******************************************************/
static void application_start(void *argument);
#if USE_MULTIPART
static sl_status_t aes_multipart_encryption(void);
static sl_status_t aes_multipart_decryption(void);
/*
 * Drive the multipart path across ECB / CBC / CTR with an encrypt ->
 * decrypt -> memcmp round-trip per case.  Defined at the bottom of the
 * file under the same USE_MULTIPART guard.
 */
static void run_aes_multipart_matrix(void);
#else
static sl_status_t aes_encryption(void);
static sl_status_t aes_decryption(void);
#endif

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

  sl_status_t status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &client_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    DEBUGOUT("Failed to start Wi-Fi client interface: 0x%lx\r\n", (unsigned long)status);
    return;
  }
  DEBUGOUT("\r\nWi-Fi Init Success\r\n");

#if USE_MULTIPART
  status = aes_multipart_encryption();
  if (status != SL_STATUS_OK) {
    return;
  }

  status = aes_multipart_decryption();
  if (status != SL_STATUS_OK) {
    return;
  }

  /* Multi-mode multipart round-trip matrix (ECB / CBC / CTR). */
  run_aes_multipart_matrix();
#else
  status = aes_encryption();
  if (status != SL_STATUS_OK) {
    return;
  }

  status = aes_decryption();
  if (status != SL_STATUS_OK) {
    return;
  }
#endif
}

#if USE_MULTIPART
sl_status_t aes_multipart_encryption(void)
{
  sl_status_t status;
  sl_si91x_aes_config_t config;
  memset(&config, 0, sizeof(sl_si91x_aes_config_t));

  uint8_t aes_flags = 0;

  config.aes_mode                   = SL_SI91X_AES_CTR;
  config.encrypt_decrypt            = SL_SI91X_AES_ENCRYPT;
  config.msg                        = msg;
  config.msg_length                 = sizeof(msg);
  config.iv                         = iv;
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
    DEBUGOUT("\r\nWrap failed, Error Code : 0x%lX\r\n", (unsigned long)status);
    return status;
  }
  DEBUGOUT("\r\nWrap success\r\n");

  config.key_config.b0.key_type = SL_SI91X_WRAPPED_KEY;
  // for 128 bits key, wrap key size is 128 bits,
  // for 192 and 256 bits keys, wrap key size is 256 bits
  if (config.key_config.b0.key_size == SL_SI91X_AES_KEY_SIZE_128) {
    memcpy(config.key_config.b0.key_buffer, &wrapped_key, SL_SI91X_AES_KEY_SIZE_128);
  } else if (config.key_config.b0.key_size == SL_SI91X_AES_KEY_SIZE_192
             || config.key_config.b0.key_size == SL_SI91X_AES_KEY_SIZE_256) {
    memcpy(config.key_config.b0.key_buffer, &wrapped_key, SL_SI91X_AES_KEY_SIZE_256);
  } else {
    DEBUGOUT("\r\n Invalid key length \r\n");
    return SL_STATUS_INVALID_PARAMETER;
  }
#endif

  uint32_t total_msg_length = config.msg_length;
  int chunk_len             = 0;
  int chunk_sizes[]         = { 128, 256, 128, 384, 128 }; // Make sure that sum of the chunk sizes are equal
                                                           // to total message length.
  int num_chunks = sizeof(chunk_sizes) / sizeof(chunk_sizes[0]);

#ifdef SL_SI91X_SIDE_BAND_CRYPTO
  /*
   * Sideband multipart: one-time init programs the FW context with key, IV,
   * mode and direction.  Subsequent chunks go through update() and the final
   * chunk through final(), which finalizes and tears the FW context down.
   */
  status = sl_si91x_mp_aes_init(&config);
  if (status != SL_STATUS_OK) {
    DEBUGOUT("\r\nAES multipart init failed, Error Code : 0x%lX\r\n", (unsigned long)status);
    return status;
  }
  (void)aes_flags;
#endif

  for (int i = 0; i < num_chunks; i++) {
    chunk_len = chunk_sizes[i];
#ifndef SL_SI91X_SIDE_BAND_CRYPTO
    if (i == 0) {
      aes_flags = SL_SI91X_AES_FIRST_CHUNK; // FIRST_CHUNK
    } else if (i == num_chunks - 1) {
      aes_flags = SL_SI91X_AES_LAST_CHUNK; // LAST_CHUNK
    } else {
      aes_flags = SL_SI91X_AES_MIDDLE_CHUNK; // MIDDLE_CHUNK
    }
#endif
    // Call AES multipart encryption function
    if (chunk_len % SL_SI91X_AES_BLOCK_SIZE == 0) {
#ifdef SL_SI91X_SIDE_BAND_CRYPTO
      if (i == num_chunks - 1) {
        status = sl_si91x_mp_aes_final(&config, (uint16_t)chunk_len, encrypted_buffer + sent_data);
      } else {
        status = sl_si91x_mp_aes_update(&config, (uint16_t)chunk_len, encrypted_buffer + sent_data);
      }
#else
      status = sl_si91x_aes_multipart(&config, chunk_len, aes_flags, encrypted_buffer + sent_data);
#endif
      if (status != SL_STATUS_OK) {
        DEBUGOUT("\r\nAES encryption failed, Error Code : 0x%lX\r\n", (unsigned long)status);
        return status;
      }
      DEBUGOUT("\r\nAES encryption success\r\n");
      sent_data += chunk_len;
      DEBUGOUT("Encrypted %d plain text bytes out of %ld bytes\n", sent_data, total_msg_length);
      // Update pointer for the next chunk
      config.msg += chunk_len;
    } else {
      DEBUGOUT("Error: Chunk size %d is not 16-byte aligned. Skipping.\n", chunk_len);
    }
  }
#if PKCS_7_PADDING
  uint8_t *padded_data = NULL;
  if (chunk_len % SL_SI91X_AES_BLOCK_SIZE != 0) {
    padded_data = pkcs7_padding((const uint8_t *)config.msg,
                                chunk_sizes[num_chunks - 1],
                                SL_SI91X_AES_BLOCK_SIZE,
                                &padded_data_length);
    if (padded_data && padded_data_length % SL_SI91X_AES_BLOCK_SIZE == 0) {
      config.msg = padded_data;
#ifdef SL_SI91X_SIDE_BAND_CRYPTO
      /*
       * Note: this branch only fires when the last loop chunk was unaligned
       * and therefore skipped above (so sl_si91x_mp_aes_final has not been
       * called yet); the padded data here becomes the actual final chunk.
       */
      sl_si91x_mp_aes_final(&config, (uint16_t)padded_data_length, encrypted_buffer + sent_data);
#else
      sl_si91x_aes_multipart(&config, padded_data_length, SL_SI91X_AES_LAST_CHUNK, encrypted_buffer + sent_data);
#endif
      sent_data += padded_data_length;
      DEBUGOUT("Final padded chunk sent, total: %d bytes\n", sent_data);
    } else {
      DEBUGOUT("Error: Padded data length %d is not 16-byte aligned. Skipping.\n", padded_data_length);
    }
  }

#if PKCS_7_PADDING
  SL_CLEANUP_MALLOC(padded_data);
#endif
#endif

  for (int i = 0; i < sent_data; i++) {
    if (i % 16 == 0) {
      DEBUGOUT("\n %d: ", i);
    }
    DEBUGOUT("0x%.2x ", encrypted_buffer[i]);
  }
  DEBUGOUT("\n");

  return status;
}
#else
sl_status_t aes_encryption(void)
{
  sl_status_t status;
  sl_si91x_aes_config_t config;
  memset(&config, 0, sizeof(sl_si91x_aes_config_t));

  config.aes_mode                   = SL_SI91X_AES_CTR;
  config.encrypt_decrypt            = SL_SI91X_AES_ENCRYPT;
  config.msg                        = msg;
  config.msg_length                 = sizeof(msg);
  config.iv                         = iv;
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
    DEBUGOUT("\r\nWrap failed, Error Code : 0x%lX\r\n", (unsigned long)status);
    return status;
  }
  DEBUGOUT("\r\nWrap success\r\n");

  config.key_config.b0.key_type = SL_SI91X_WRAPPED_KEY;
  // for 128 bits key, wrap key size is 128 bits,
  // for 192 and 256 bits keys, wrap key size is 256 bits
  if (config.key_config.b0.key_size == SL_SI91X_AES_KEY_SIZE_128) {
    memcpy(config.key_config.b0.key_buffer, &wrapped_key, SL_SI91X_AES_KEY_SIZE_128);
  } else if (config.key_config.b0.key_size == SL_SI91X_AES_KEY_SIZE_192
             || config.key_config.b0.key_size == SL_SI91X_AES_KEY_SIZE_256) {
    memcpy(config.key_config.b0.key_buffer, &wrapped_key, SL_SI91X_AES_KEY_SIZE_256);
  } else {
    DEBUGOUT("\r\n Invalid key length \r\n");
    return SL_STATUS_INVALID_PARAMETER;
  }
#endif

#if PKCS_7_PADDING
  uint8_t *padded_data =
    pkcs7_padding((const uint8_t *)config.msg, config.msg_length, SL_SI91X_AES_BLOCK_SIZE, &padded_data_length);
  SL_VERIFY_POINTER_OR_RETURN(padded_data, SL_STATUS_NULL_POINTER);

  config.msg        = padded_data;
  config.msg_length = padded_data_length;
#endif

  status = sl_si91x_aes(&config, encrypted_buffer);
  if (status != SL_STATUS_OK) {
    DEBUGOUT("\r\nAES encryption failed, Error Code : 0x%lX\r\n", (unsigned long)status);
    return status;
  }
  DEBUGOUT("\r\nAES encryption success\r\n");

  for (unsigned int i = 0; i < sizeof(msg); i++) {
    if (i % 16 == 0) {
      DEBUGOUT("\n %d: ", i);
    }
    DEBUGOUT("0x%.2x ", encrypted_buffer[i]);
  }
  DEBUGOUT("\n");

#if PKCS_7_PADDING
  SL_CLEANUP_MALLOC(padded_data);
#endif

  return status;
}
#endif

#if USE_MULTIPART
sl_status_t aes_multipart_decryption(void)
{
  sl_status_t status;
  sl_si91x_aes_config_t config;
  memset(&config, 0, sizeof(sl_si91x_aes_config_t));

  uint8_t aes_flags = 0;

  config.aes_mode                   = SL_SI91X_AES_CTR;
  config.encrypt_decrypt            = SL_SI91X_AES_DECRYPT;
  config.msg                        = encrypted_buffer;
  config.msg_length                 = sizeof(msg);
  config.iv                         = iv;
  config.key_config.b0.key_size     = SL_SI91X_AES_KEY_SIZE_256;
  config.key_config.b0.key_slot     = 0;
  config.key_config.b0.wrap_iv_mode = SL_SI91X_WRAP_IV_ECB_MODE;
  config.key_config.b0.key_type     = SL_SI91X_TRANSPARENT_KEY;
  memcpy(config.key_config.b0.key_buffer, &key, config.key_config.b0.key_size);

#if USE_WRAPPED_KEYS
  config.key_config.b0.key_type = SL_SI91X_WRAPPED_KEY;
  // for 128 bits key, wrap key size is 128 bits,
  // for 192 and 256 bits keys, wrap key size is 256 bits
  if (config.key_config.b0.key_size == SL_SI91X_AES_KEY_SIZE_128) {
    memcpy(config.key_config.b0.key_buffer, &wrapped_key, SL_SI91X_AES_KEY_SIZE_128);
  } else if (config.key_config.b0.key_size == SL_SI91X_AES_KEY_SIZE_192
             || config.key_config.b0.key_size == SL_SI91X_AES_KEY_SIZE_256) {
    memcpy(config.key_config.b0.key_buffer, &wrapped_key, SL_SI91X_AES_KEY_SIZE_256);
  } else {
    DEBUGOUT("\r\n Invalid key length \r\n");
    return SL_STATUS_INVALID_PARAMETER;
  }
#endif

#if PKCS_7_PADDING
  config.msg_length = sent_data;
#endif

  uint32_t total_msg_length = config.msg_length;
  int received_data         = 0;
  int chunk_sizes[]         = { 128, 256, 128, 384, 128 };
  int num_chunks            = sizeof(chunk_sizes) / sizeof(chunk_sizes[0]);

#ifdef SL_SI91X_SIDE_BAND_CRYPTO
  /*
   * Sideband multipart decryption mirrors the encryption path: init() once
   * to set up the FW context (key, IV, mode, decrypt direction), then
   * update() per chunk, and final() on the last chunk to drain and tear the
   * context down.
   */
  status = sl_si91x_mp_aes_init(&config);
  if (status != SL_STATUS_OK) {
    DEBUGOUT("\r\nAES multipart init failed, Error Code : 0x%lX\r\n", (unsigned long)status);
    return status;
  }
  (void)aes_flags;
#endif

  for (int i = 0; i < num_chunks; i++) {
    int chunk_len = chunk_sizes[i];
#ifndef SL_SI91X_SIDE_BAND_CRYPTO
    if (i == 0) {
      aes_flags = SL_SI91X_AES_FIRST_CHUNK; // FIRST_CHUNK
    } else if (i == num_chunks - 1) {
      aes_flags = SL_SI91X_AES_LAST_CHUNK; // LAST_CHUNK
    } else {
      aes_flags = SL_SI91X_AES_MIDDLE_CHUNK; // MIDDLE_CHUNK
    }
#endif
    // Call AES multipart decryption function
    if (chunk_len % SL_SI91X_AES_BLOCK_SIZE == 0) {
#ifdef SL_SI91X_SIDE_BAND_CRYPTO
      if (i == num_chunks - 1) {
        status = sl_si91x_mp_aes_final(&config, (uint16_t)chunk_len, decrypted_buffer + received_data);
      } else {
        status = sl_si91x_mp_aes_update(&config, (uint16_t)chunk_len, decrypted_buffer + received_data);
      }
#else
      status = sl_si91x_aes_multipart(&config, chunk_len, aes_flags, decrypted_buffer + received_data);
#endif

      if (status != SL_STATUS_OK) {
        DEBUGOUT("\r\nAES Multipart decryption failed, Error Code : 0x%lX\r\n", (unsigned long)status);
        return status;
      }
      DEBUGOUT("\r\nAES Multipart decryption success\r\n");
      received_data += chunk_len;
      // Update pointer for the next chunk
      config.msg += chunk_len;
      DEBUGOUT("Decrypted %d cipher text bytes out of %ld bytes\n", received_data, total_msg_length);
    } else {
      DEBUGOUT("Error: Chunk size %d is not 16-byte aligned. Skipping.\n", chunk_len);
    }
  }
#if PKCS_7_PADDING
  int32_t unpadded_data_length = pkcs7_unpad(decrypted_buffer, received_data);
  if (unpadded_data_length < 0) {
    DEBUGOUT("Error: Padding removal failed.\n");
    return SL_STATUS_FAIL;
  }
  memset(decrypted_unpadded_data_buffer, 0, sent_data);
  memcpy(decrypted_unpadded_data_buffer, decrypted_buffer, unpadded_data_length);
  received_data = unpadded_data_length;
  DEBUGOUT("Decryption completed. Unpadded data length: %ld bytes\n", unpadded_data_length);
#endif

  for (int i = 0; i < received_data; i++) {
    if (i % 16 == 0) {
      DEBUGOUT("\n %d: ", i);
    }
    DEBUGOUT("0x%.2x ", decrypted_buffer[i]);
  }
  DEBUGOUT("\n");

  return status;
}

#else
sl_status_t aes_decryption(void)
{
  sl_status_t status;
  sl_si91x_aes_config_t config;
  memset(&config, 0, sizeof(sl_si91x_aes_config_t));

  config.aes_mode                   = SL_SI91X_AES_CTR;
  config.encrypt_decrypt            = SL_SI91X_AES_DECRYPT;
  config.msg                        = encrypted_buffer;
  config.msg_length                 = sizeof(msg);
  config.iv                         = iv;
  config.key_config.b0.key_size     = SL_SI91X_AES_KEY_SIZE_256;
  config.key_config.b0.key_slot     = 0;
  config.key_config.b0.wrap_iv_mode = SL_SI91X_WRAP_IV_ECB_MODE;
  config.key_config.b0.key_type     = SL_SI91X_TRANSPARENT_KEY;
  memcpy(config.key_config.b0.key_buffer, &key, config.key_config.b0.key_size);

#if USE_WRAPPED_KEYS
  config.key_config.b0.key_type = SL_SI91X_WRAPPED_KEY;
  // for 128 bits key, wrap key size is 128 bits,
  // for 192 and 256 bits keys, wrap key size is 256 bits
  if (config.key_config.b0.key_size == SL_SI91X_AES_KEY_SIZE_128) {
    memcpy(config.key_config.b0.key_buffer, &wrapped_key, SL_SI91X_AES_KEY_SIZE_128);
  } else if (config.key_config.b0.key_size == SL_SI91X_AES_KEY_SIZE_192
             || config.key_config.b0.key_size == SL_SI91X_AES_KEY_SIZE_256) {
    memcpy(config.key_config.b0.key_buffer, &wrapped_key, SL_SI91X_AES_KEY_SIZE_256);
  } else {
    DEBUGOUT("\r\n Invalid key length \r\n");
    return SL_STATUS_INVALID_PARAMETER;
  }
#endif

#if PKCS_7_PADDING
  config.msg_length = padded_data_length;
#endif

  status = sl_si91x_aes(&config, decrypted_buffer);
  if (status != SL_STATUS_OK) {
    DEBUGOUT("\r\nAES decryption failed, Error Code : 0x%lX\r\n", (unsigned long)status);
    return status;
  }
  DEBUGOUT("\r\nAES decryption success\r\n");

  for (unsigned int i = 0; i < sizeof(msg); i++) {
    if (i % 16 == 0) {
      DEBUGOUT("\n %d: ", i);
    }
    DEBUGOUT("0x%.2x ", decrypted_buffer[i]);
  }
  DEBUGOUT("\n");

#if PKCS_7_PADDING
  int32_t unpadded_data_length = pkcs7_unpad(decrypted_buffer, padded_data_length);
  if (unpadded_data_length < 0) {
    return SL_STATUS_FAIL;
  }
  memset(decrypted_unpadded_data_buffer, 0, BUFFER_SIZE);
  memcpy(decrypted_unpadded_data_buffer, decrypted_buffer, unpadded_data_length);
#endif

  return status;
}
#endif

/* =========================================================================
 *  Multi-mode multipart round-trip matrix (ECB / CBC / CTR)
 *
 *  Table-driven encrypt -> decrypt -> memcmp verification driven through
 *  the same compile-time sideband / non-sideband dispatch as the rest of
 *  this file.  Output is routed through DEBUGOUT() from rsi_debug.h.
 * ========================================================================= */
#if USE_MULTIPART

#include <stdbool.h>

/* Sized to hold the largest matrix case (single-chunk 1424-byte case that
 * forces the sideband firmware to split into two HW windows). */
#define MP_MATRIX_BUFFER_SIZE 1424
#define MP_MATRIX_MAX_CHUNKS  8

typedef struct {
  const char *name;
  uint8_t aes_mode; /* SL_SI91X_AES_ECB / _CBC / _CTR */
  uint8_t needs_iv; /* CBC + CTR: 1; ECB: 0 */
  uint16_t chunk_sizes[MP_MATRIX_MAX_CHUNKS];
  uint8_t num_chunks;
  uint16_t total_len;
} mp_demo_case_t;

/*
 * Cases:
 *  - ECB block-aligned : 5 chunks, all 16-aligned (ECB requires alignment).
 *  - CBC block-aligned : 5 chunks, exercises FW IV-chain across chunks.
 *  - CTR block-aligned : 5 chunks, exercises FW counter advance across chunks.
 *  - CTR byte-tail     : last chunk is non-block-aligned (117 bytes), drives
 *                        the firmware Phase-1 / Phase-3 keystream carry-over.
 *  - CTR HW-windowing  : sideband-only.  Single 1424-byte chunk forces the
 *                        firmware to split into two MAX_DATA_LENGTH_FOR_AES
 *                        (1408-byte) HW windows, releasing the AES/SHA HW
 *                        lock between them.
 */
static const mp_demo_case_t mp_matrix_cases[] = {
  { "ECB block-aligned", SL_SI91X_AES_ECB, 0, { 128, 256, 128, 384, 128 }, 5, 1024 },
  { "CBC block-aligned", SL_SI91X_AES_CBC, 1, { 128, 256, 128, 384, 128 }, 5, 1024 },
  { "CTR block-aligned", SL_SI91X_AES_CTR, 1, { 128, 256, 128, 384, 128 }, 5, 1024 },
  { "CTR byte-tail", SL_SI91X_AES_CTR, 1, { 128, 256, 128, 384, 117 }, 5, 1013 },
#ifdef SL_SI91X_SIDE_BAND_CRYPTO
  { "CTR FW HW-windowing (single 1424B chunk)", SL_SI91X_AES_CTR, 1, { 1424 }, 1, 1424 },
#endif
};

#define MP_NUM_MATRIX_CASES (sizeof(mp_matrix_cases) / sizeof(mp_matrix_cases[0]))

static uint8_t mp_msg[MP_MATRIX_BUFFER_SIZE];
static uint8_t mp_ct[MP_MATRIX_BUFFER_SIZE];
static uint8_t mp_pt[MP_MATRIX_BUFFER_SIZE];

static void mp_fill_pattern(uint8_t *buf, size_t n)
{
  /* Reproducible non-trivial pattern so a roundtrip mismatch is conspicuous. */
  for (size_t i = 0; i < n; i++) {
    buf[i] = (uint8_t)((i * 31u + 0xA5u) & 0xFFu);
  }
}

static const char *mp_mode_name(uint8_t mode)
{
  switch (mode) {
    case SL_SI91X_AES_ECB:
      return "ECB";
    case SL_SI91X_AES_CBC:
      return "CBC";
    case SL_SI91X_AES_CTR:
      return "CTR";
    default:
      return "?";
  }
}

static const char *mp_dir_name(uint8_t encrypt_decrypt)
{
  return (encrypt_decrypt == SL_SI91X_AES_ENCRYPT) ? "encrypt" : "decrypt";
}

static void mp_apply_key_to_config(sl_si91x_aes_config_t *cfg)
{
  cfg->key_config.b0.key_size     = SL_SI91X_AES_KEY_SIZE_256;
  cfg->key_config.b0.key_slot     = 0;
  cfg->key_config.b0.wrap_iv_mode = SL_SI91X_WRAP_IV_ECB_MODE;
#if USE_WRAPPED_KEYS
  cfg->key_config.b0.key_type = SL_SI91X_WRAPPED_KEY;
  memcpy(cfg->key_config.b0.key_buffer, wrapped_key, SL_SI91X_AES_KEY_SIZE_256);
#else
  cfg->key_config.b0.key_type = SL_SI91X_TRANSPARENT_KEY;
  memcpy(cfg->key_config.b0.key_buffer, key, SL_SI91X_AES_KEY_SIZE_256);
#endif
}

static void mp_build_base_config(sl_si91x_aes_config_t *cfg,
                                 const mp_demo_case_t *c,
                                 uint8_t encrypt_decrypt,
                                 uint8_t *msg_in)
{
  memset(cfg, 0, sizeof(*cfg));
  cfg->aes_mode        = c->aes_mode;
  cfg->encrypt_decrypt = encrypt_decrypt;
  cfg->msg             = msg_in;
  cfg->msg_length      = c->total_len;
  cfg->iv              = c->needs_iv ? iv : NULL;
  mp_apply_key_to_config(cfg);
}

/*
 * Drive a full multipart session (init + update*N + final on sideband, or
 * FIRST/MIDDLE/LAST chunked sl_si91x_aes_multipart on the legacy path).
 * Direction is taken from cfg->encrypt_decrypt.  Slides cfg->msg through
 * chunk boundaries and restores it before returning.
 */
static sl_status_t mp_run_session(sl_si91x_aes_config_t *cfg, const mp_demo_case_t *c, uint8_t *out)
{
  sl_status_t status = SL_STATUS_OK;
  uint16_t offset    = 0;
  uint8_t *msg_base  = cfg->msg;

#ifdef SL_SI91X_SIDE_BAND_CRYPTO
  status = sl_si91x_mp_aes_init(cfg);
  if (status != SL_STATUS_OK) {
    DEBUGOUT("  [%s] init failed: 0x%lX\r\n", mp_dir_name(cfg->encrypt_decrypt), (unsigned long)status);
    return status;
  }

  for (uint8_t i = 0; i < c->num_chunks; i++) {
    uint16_t len = c->chunk_sizes[i];
    if (i == c->num_chunks - 1) {
      status = sl_si91x_mp_aes_final(cfg, len, out + offset);
    } else {
      status = sl_si91x_mp_aes_update(cfg, len, out + offset);
    }
    if (status != SL_STATUS_OK) {
      DEBUGOUT("  [%s] chunk %u (len %u) failed: 0x%lX\r\n",
               mp_dir_name(cfg->encrypt_decrypt),
               (unsigned)i,
               (unsigned)len,
               (unsigned long)status);
      cfg->msg = msg_base;
      return status;
    }
    cfg->msg += len;
    offset = (uint16_t)(offset + len);
  }
#else
  for (uint8_t i = 0; i < c->num_chunks; i++) {
    uint16_t len = c->chunk_sizes[i];
    uint8_t flags = (i == 0) ? SL_SI91X_AES_FIRST_CHUNK
                             : ((i == c->num_chunks - 1) ? SL_SI91X_AES_LAST_CHUNK : SL_SI91X_AES_MIDDLE_CHUNK);

    status = sl_si91x_aes_multipart(cfg, len, flags, out + offset);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("  [%s] chunk %u (len %u) failed: 0x%lX\r\n",
               mp_dir_name(cfg->encrypt_decrypt),
               (unsigned)i,
               (unsigned)len,
               (unsigned long)status);
      cfg->msg = msg_base;
      return status;
    }
    cfg->msg += len;
    offset = (uint16_t)(offset + len);
  }
#endif

  cfg->msg = msg_base;
  return status;
}

static sl_status_t mp_run_case(const mp_demo_case_t *c)
{
  sl_status_t status;
  sl_si91x_aes_config_t cfg;

  /* Encrypt mp_msg -> mp_ct */
  mp_build_base_config(&cfg, c, SL_SI91X_AES_ENCRYPT, mp_msg);
  status = mp_run_session(&cfg, c, mp_ct);
  if (status != SL_STATUS_OK) {
    return status;
  }

  /* Decrypt mp_ct -> mp_pt with the original IV */
  mp_build_base_config(&cfg, c, SL_SI91X_AES_DECRYPT, mp_ct);
  status = mp_run_session(&cfg, c, mp_pt);
  if (status != SL_STATUS_OK) {
    return status;
  }

  if (memcmp(mp_pt, mp_msg, c->total_len) != 0) {
    /* Surface the first mismatching byte so a window-stitch / IV-chain bug
     * is locatable from the log alone (e.g., offset 1408 = FW HW window
     * boundary, offset 0 = key/IV setup, offset == chunk boundary, etc.). */
    uint16_t off = 0;
    while (off < c->total_len && mp_pt[off] == mp_msg[off]) {
      off++;
    }
    DEBUGOUT("  round-trip mismatch at byte %u (got 0x%02x, want 0x%02x)\r\n", (unsigned)off, mp_pt[off], mp_msg[off]);
    return SL_STATUS_FAIL;
  }
  DEBUGOUT("  round-trip OK (%u bytes)\r\n", (unsigned)c->total_len);
  return SL_STATUS_OK;
}

static void run_aes_multipart_matrix(void)
{
  size_t passed = 0;
  size_t failed = 0;

  mp_fill_pattern(mp_msg, sizeof(mp_msg));

  DEBUGOUT("\r\n=== Multi-mode multipart matrix (%u cases) ===\r\n", (unsigned)MP_NUM_MATRIX_CASES);
  DEBUGOUT("path: %s, key: %s, AES-256\r\n",
#ifdef SL_SI91X_SIDE_BAND_CRYPTO
           "SIDEBAND",
#else
           "NON-SIDEBAND",
#endif
#if USE_WRAPPED_KEYS
           "WRAPPED");
#else
           "TRANSPARENT");
#endif

  for (size_t i = 0; i < MP_NUM_MATRIX_CASES; i++) {
    const mp_demo_case_t *c = &mp_matrix_cases[i];

    DEBUGOUT("\r\n[Case %u/%u] %s\r\n", (unsigned)(i + 1), (unsigned)MP_NUM_MATRIX_CASES, c->name);

    /* Per-case parameter line so the log is self-contained. */
    DEBUGOUT("  mode=%s, IV=%s, chunks=%u, total=%u, sizes={",
             mp_mode_name(c->aes_mode),
             c->needs_iv ? "yes" : "no",
             (unsigned)c->num_chunks,
             (unsigned)c->total_len);
    for (uint8_t k = 0; k < c->num_chunks; k++) {
      DEBUGOUT("%s%u", (k == 0) ? "" : ",", (unsigned)c->chunk_sizes[k]);
    }
    DEBUGOUT("}\r\n");

    if (mp_run_case(c) == SL_STATUS_OK) {
      DEBUGOUT("  PASS\r\n");
      passed++;
    } else {
      DEBUGOUT("  FAIL\r\n");
      failed++;
    }
  }

  DEBUGOUT("\r\n=== Matrix Summary: %u passed, %u failed ===\r\n", (unsigned)passed, (unsigned)failed);
  /* Single-token verdict line so CI / log scrapers can grep. */
  DEBUGOUT("RESULT: %s\r\n", (failed == 0) ? "PASS" : "FAIL");
}

#endif /* USE_MULTIPART */
