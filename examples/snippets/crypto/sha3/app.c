/***************************************************************************/ /**
 * @file app.c
 * @brief SHA3 Example Application
 *******************************************************************************
 * # License
 * <b>Copyright 2026 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_si91x_sha.h"
#include "sl_si91x_sha3.h"

/******************************************************
  *                    Constants
  ******************************************************/
// NIST SHA3 short test vector (length = 56 bytes)
char message[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";

/*
  * SHA3 multi-rate padding (pad10*1) with the SHA3 domain separator 0x06.
  *
  * The si91x firmware does NOT apply SHA3 padding internally, so the
  * message must be pre-padded by the application before calling
  * sl_si91x_sha() with a SHA3 mode. Without this padding the resulting
  * digest will not match the standard NIST SHA3 vectors.
  *
  * Rate (in bytes) per mode (rate = (1600 - 2 * digest_bits) / 8):
  *   SHA3-224 -> 144
  *   SHA3-256 -> 136
  *   SHA3-384 -> 104
  *   SHA3-512 ->  72
  */
#define SHA3_PAD_DOMAIN 0x06

#define SHA3_224_RATE 144
#define SHA3_256_RATE 136
#define SHA3_384_RATE 104
#define SHA3_512_RATE 72

// Worst-case padded buffer size = original message + max rate (144).
#define SHA3_PAD_BUF_SIZE 256

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

// Buffer to store response (sized for the largest SHA3 digest)
uint8_t digest[SL_SI91X_SHA3_512_DIGEST_LEN];

// Expected SHA3-512 digest for the NIST short test vector above
uint8_t digest_out[SL_SI91X_SHA3_512_DIGEST_LEN] = { 0x04, 0xa3, 0x71, 0xe8, 0x4e, 0xcf, 0xb5, 0xb8, 0xb7, 0x7c, 0xb4,
                                                     0x86, 0x10, 0xfc, 0xa8, 0x18, 0x2d, 0xd4, 0x57, 0xce, 0x6f, 0x32,
                                                     0x6a, 0x0f, 0xd3, 0xd7, 0xec, 0x2f, 0x1e, 0x91, 0x63, 0x6d, 0xee,
                                                     0x69, 0x1f, 0xbe, 0x0c, 0x98, 0x53, 0x02, 0xba, 0x1b, 0x0d, 0x8d,
                                                     0xc7, 0x8c, 0x08, 0x63, 0x46, 0xb5, 0x33, 0xb4, 0x9c, 0x03, 0x0d,
                                                     0x99, 0xa2, 0x7d, 0xaf, 0x11, 0x39, 0xd6, 0xe7, 0x5e };

/******************************************************
  *               Function Declarations
  ******************************************************/
static void application_start(void *argument);
static sl_status_t sha3_process(void);
static int sha3_pad(const uint8_t *msg,
                    size_t msg_len,
                    size_t rate,
                    uint8_t *out_buf,
                    size_t out_buf_size,
                    size_t *out_padded_len);

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
    printf("Failed to start Wi-Fi client interface: 0x%lx\r\n", (unsigned long)status);
    return;
  }
  printf("\r\nWi-Fi Init Success\r\n");

  status = sha3_process();
  if (status != SL_STATUS_OK) {
    return;
  }
}

static int sha3_pad(const uint8_t *msg,
                    size_t msg_len,
                    size_t rate,
                    uint8_t *out_buf,
                    size_t out_buf_size,
                    size_t *out_padded_len)
{
  if (rate == 0 || out_buf == NULL || out_padded_len == NULL) {
    return -1;
  }

  size_t q          = rate - (msg_len % rate); // bytes to append: 1..rate
  size_t padded_len = msg_len + q;

  if (padded_len > out_buf_size) {
    return -2;
  }

  if (msg_len > 0 && msg != NULL) {
    memcpy(out_buf, msg, msg_len);
  }

  if (q == 1) {
    // Single pad byte: 0x06 | 0x80 = 0x86
    out_buf[msg_len] = SHA3_PAD_DOMAIN | 0x80;
  } else {
    out_buf[msg_len] = SHA3_PAD_DOMAIN;
    if (q > 2) {
      memset(&out_buf[msg_len + 1], 0x00, q - 2);
    }
    out_buf[padded_len - 1] = 0x80;
  }

  *out_padded_len = padded_len;
  return 0;
}

static sl_status_t sha3_process(void)
{
  sl_status_t status;
  uint8_t padded_msg[SHA3_PAD_BUF_SIZE] = { 0 };
  size_t padded_len                     = 0;

  // Apply SHA3-512 pad10*1 padding with domain separator 0x06.
  // When changing the SHA3 mode below, also update the rate passed here
  // to match (SHA3_224_RATE / SHA3_256_RATE / SHA3_384_RATE / SHA3_512_RATE).
  if (sha3_pad((const uint8_t *)message, strlen(message), SHA3_512_RATE, padded_msg, sizeof(padded_msg), &padded_len)
      != 0) {
    printf("\r\nSHA3 padding failed\r\n");
    return SL_STATUS_FAIL;
  }
  printf("\r\nSHA3 padded_len=%u (orig=%u)\r\n", (unsigned)padded_len, (unsigned)strlen(message));

  // Compute digest using SHA3. Change SL_SI91X_SHA3_512 to one of
  // SL_SI91X_SHA3_224 / SL_SI91X_SHA3_256 / SL_SI91X_SHA3_384 / SL_SI91X_SHA3_512
  // to select a different SHA3 digest size; remember to also update the
  // rate used in sha3_pad() above and digest_out[] with the matching
  // expected hash.
#ifdef SL_SI91X_SIDE_BAND_CRYPTO
  status =
    sl_si91x_mp_sha(SL_SI91X_SHA3_512, padded_msg, (uint16_t)padded_len, (uint8_t)(FIRST_CHUNK | LAST_CHUNK), digest);
#else
  status = sl_si91x_sha(SL_SI91X_SHA3_512, padded_msg, (uint16_t)padded_len, digest);
#endif
  if (status != SL_STATUS_OK) {
    printf("\r\n SHA3 Failed, Error Code : 0x%lX\r\n", (unsigned long)status);
    return status;
  }
  printf("\r\nSHA3 success\r\n");

  for (int i = 0; i < SL_SI91X_SHA3_512_DIGEST_LEN; i++) {
    printf(" 0x%02x", digest[i]);
  }

  // By default SHA3 mode is SHA3_512. So compare the resultant hash with digest_out
  // Change the expected sha in the comparison
  if (memcmp(digest, digest_out, SL_SI91X_SHA3_512_DIGEST_LEN)) {
    printf("\r\nSHA3 Compare with expected value failed\r\n");
  } else {
    printf("\r\nSHA3 Compare with expected value is Successful\r\n");
  }

  return status;
}
