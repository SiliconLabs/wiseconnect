/***************************************************************************/ /**
 * @file sl_si91x_puf.c
 * @brief SL SI91X PUF source file
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

#include "sl_si91x_puf.h"
#include "sl_si91x_driver.h"

/**
 * @brief PUF state machine control block.
 */
typedef enum {
  SLI_SI91X_PUF_STATE_NONE = 0,         ///< PUF default state
  SLI_SI91X_PUF_STATE_STARTED,          ///< PUF process started
  SLI_SI91X_PUF_STATE_SET_KEY,          ///< Key set for PUF
  SLI_SI91X_PUF_STATE_KEY_LOADED,       ///< Key loaded into PUF
  SLI_SI91X_PUF_STATE_SET_KEY_DISABLED, ///< PUF set key disabled
  SLI_SI91X_PUF_STATE_GET_KEY_DISABLED  ///< PUF get key disabled
} sli_si91x_puf_state_t;

sli_si91x_puf_state_t puf_state = SLI_SI91X_PUF_STATE_NONE;

/**
 * @brief Size of the key code in bytes.
 */
#define SLI_SI91X_PUF_KEY_CODE_SIZE_BYTES 52
/** @} */

/**
 * @brief Size of PUF data in units.
 */
#define SLI_SI91X_PUF_DATA_SIZE 2

/**
 * @brief Maximum size of PUF data in bytes.
 */
#define SLI_SI91X_MAX_PUF_DATA_SIZE_BYTES 1408

/**
 * @brief Maximum size of key in bytes.
 */
#define SLI_SI91X_MAX_PUF_KEY_SIZE_BYTES 32

/**
 * @brief Maximum size of PUF Initialization Vector in bytes.
 */
#define SLI_SI91X_MAX_PUF_IV_SIZE_BYTES 32

/**
 * @brief Maximum key index value. Valid range of index is 0 - 15
 */
#define SLI_SI91X_MAX_PUF_KEY_INDEX 15

/**
 * @brief Block size for AES encryption in the PUF module.
 */
#define SLI_SI91X_PUF_BLOCK_SIZE 16

/**
 * @brief Structure for PUF Set Key Request.
 */
typedef struct {
  uint8_t key_index;                             ///< Index of the key
  uint8_t key_size;                              ///< Size of the key
  uint8_t key[SLI_SI91X_MAX_PUF_KEY_SIZE_BYTES]; ///< Key data
} sli_si91x_req_puf_set_key_t;

/**
 * @brief Structure for PUF Get Key Request.
 */
typedef struct {
  uint8_t key_code[SLI_SI91X_PUF_KEY_CODE_SIZE_BYTES]; ///< Key code data
} sli_si91x_req_puf_get_key_t;

/**
 * @brief Structure for PUF Load Key Request.
 */
typedef struct {
  uint8_t key_code[SLI_SI91X_PUF_KEY_CODE_SIZE_BYTES]; ///< Key code data
} sli_si91x_req_puf_load_key_t;

/**
 * @brief Structure for PUF Intrinsic Key Request.
 */
typedef struct {
  uint8_t key_index; ///< Index of the key
  uint8_t key_size;  ///< Size of the key
} sli_si91x_req_puf_intr_key_t;

/**
 * @brief Structure for AES Encryption Request.
 */
typedef struct {
  uint8_t mode;                                    ///< AES mode (0 - ECB, 1 - CBC)
  uint8_t key[SLI_SI91X_MAX_PUF_KEY_SIZE_BYTES];   ///< Key data
  uint8_t iv[SLI_SI91X_MAX_PUF_IV_SIZE_BYTES];     ///< Initialization Vector
  uint8_t data_size[SLI_SI91X_PUF_DATA_SIZE];      ///< Size of data
  uint8_t data[SLI_SI91X_MAX_PUF_DATA_SIZE_BYTES]; ///< Data to encrypt
} sli_si91x_req_aes_encrypt_t;

/**
 * @brief Structure for AES Decryption Request.
 */
typedef struct {
  uint8_t mode;                                    ///< AES mode (0 - ECB, 1 - CBC)
  uint8_t key[SLI_SI91X_MAX_PUF_KEY_SIZE_BYTES];   ///< Key data
  uint8_t iv[SLI_SI91X_MAX_PUF_IV_SIZE_BYTES];     ///< Initialization Vector
  uint8_t data_size[SLI_SI91X_PUF_DATA_SIZE];      ///< Size of data
  uint8_t data[SLI_SI91X_MAX_PUF_DATA_SIZE_BYTES]; ///< Data to decrypt
} sli_si91x_req_aes_decrypt_t;

/**
 * @brief Structure for AES MAC Request.
 */
typedef struct {
  uint8_t mode;                                    ///< AES mode (0 - ECB, 1 - CBC)
  uint8_t key[SLI_SI91X_MAX_PUF_KEY_SIZE_BYTES];   ///< Key data
  uint8_t iv[SLI_SI91X_MAX_PUF_IV_SIZE_BYTES];     ///< Initialization Vector
  uint8_t data_size[SLI_SI91X_PUF_DATA_SIZE];      ///< Size of data
  uint8_t data[SLI_SI91X_MAX_PUF_DATA_SIZE_BYTES]; ///< Data for MAC calculation
} sli_si91x_req_aes_mac_t;

// Converts a 16-bit unsigned integer to a 2-byte array.
/**
 * @brief Converts a 16-bit unsigned integer to a 2-byte array.
 *
 * @details
 *  This function takes a 16-bit unsigned integer as input and converts it into
 *  a 2-byte array. The conversion stores the least significant byte in the first
 *  element and the most significant byte in the second element.
 *
 * @param[in]  val  The 16-bit unsigned integer to be converted.
 * @param[out] dBuf The 2-byte array where the converted value will be stored.
 *
 */
static void sli_uint16_to_2bytes_util(uint16_t val, uint8_t *dBuf)
{
  // Fill the buffer with the 16-bit value in little-endian format
  dBuf[0] = val & 0x00ff;
  dBuf[1] = (val >> 8) & 0x00ff;
}

sl_status_t sl_si91x_puf_start_req(void)
{
  // initial checks
  if (!sl_si91x_is_device_initialized()) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  sl_status_t status = SL_STATUS_OK;
  // driver send
  status = sli_si91x_driver_send_command(SLI_COMMON_REQ_PUF_START,
                                         SI91X_COMMON_CMD,
                                         NULL,
                                         0,
                                         SLI_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                         NULL,
                                         NULL);
  VERIFY_STATUS_AND_RETURN(status);
  // change puf_state
  puf_state = SLI_SI91X_PUF_STATE_STARTED;
  return status;
}

sl_status_t sl_si91x_puf_set_key_req(uint8_t key_index, uint8_t key_size, const uint8_t *key_ptr, uint8_t *key_code_ptr)
{
  // initial checks
  if (!sl_si91x_is_device_initialized()) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  if (puf_state < SLI_SI91X_PUF_STATE_STARTED || puf_state == SLI_SI91X_PUF_STATE_SET_KEY_DISABLED) {
    return SL_STATUS_INVALID_STATE;
  }
  if ((key_index > SLI_SI91X_MAX_PUF_KEY_INDEX) || (key_size > SL_SI91X_PUF_KEY_SIZE_256) || (key_ptr == NULL)
      || (key_code_ptr == NULL)) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if ((size_t)(key_size + 1) * SLI_SI91X_PUF_BLOCK_SIZE != strlen((const char *)key_ptr)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_status_t status                      = SL_STATUS_OK;
  sli_si91x_req_puf_set_key_t puf_set_key = { 0 };
  sl_wifi_buffer_t *buffer                = NULL;
  sl_wifi_system_packet_t *packet         = NULL;

  // Assign key_index
  puf_set_key.key_index = key_index;

  // Assign Key size
  puf_set_key.key_size = key_size;

  // Copy data to be sent
  memcpy(&puf_set_key.key[0], key_ptr, (key_size + 1) * SLI_SI91X_PUF_BLOCK_SIZE);

  // driver send
  status = sli_si91x_driver_send_command(SLI_COMMON_REQ_PUF_SET_KEY,
                                         SI91X_COMMON_CMD,
                                         &puf_set_key,
                                         sizeof(sli_si91x_req_puf_set_key_t),
                                         SL_SI91X_WAIT_FOR_RESPONSE(1000),
                                         NULL,
                                         &buffer);

  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sli_si91x_host_free_buffer(buffer);
  }
  VERIFY_STATUS_AND_RETURN(status);

  // change puf_state
  puf_state = SLI_SI91X_PUF_STATE_SET_KEY;

  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  if (packet->length > 0) {
    memcpy(key_code_ptr, packet->data, packet->length);
  }
  sli_si91x_host_free_buffer(buffer);
  return status;
}

sl_status_t sl_si91x_puf_set_key_disable_req(void)
{
  // initial checks
  if (!sl_si91x_is_device_initialized()) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  sl_status_t status = SL_STATUS_OK;
  // driver send
  status = sli_si91x_driver_send_command(SLI_COMMON_REQ_PUF_DIS_SET_KEY,
                                         SI91X_COMMON_CMD,
                                         NULL,
                                         0,
                                         SLI_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                         NULL,
                                         NULL);
  VERIFY_STATUS_AND_RETURN(status);
  // change puf_state
  puf_state = SLI_SI91X_PUF_STATE_SET_KEY_DISABLED;
  return status;
}

sl_status_t sl_si91x_puf_get_key_req(const uint8_t *key_code_ptr, uint8_t *key_ptr)
{
  // initial checks
  if (!sl_si91x_is_device_initialized()) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  if (puf_state < SLI_SI91X_PUF_STATE_STARTED || puf_state == SLI_SI91X_PUF_STATE_GET_KEY_DISABLED) {
    return SL_STATUS_INVALID_STATE;
  }
  if ((key_code_ptr == NULL) || (key_ptr == NULL)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_status_t status                      = SL_STATUS_OK;
  sli_si91x_req_puf_get_key_t puf_get_key = { 0 };
  sl_wifi_buffer_t *buffer                = NULL;
  sl_wifi_system_packet_t *packet         = NULL;

  // Copy data to be sent
  memcpy(&puf_get_key.key_code[0], key_code_ptr, SLI_SI91X_PUF_KEY_CODE_SIZE_BYTES);

  // driver send
  status = sli_si91x_driver_send_command(SLI_COMMON_REQ_PUF_GET_KEY,
                                         SI91X_COMMON_CMD,
                                         &puf_get_key,
                                         sizeof(sli_si91x_req_puf_get_key_t),
                                         SL_SI91X_WAIT_FOR_RESPONSE(1000),
                                         NULL,
                                         &buffer);

  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sli_si91x_host_free_buffer(buffer);
  }
  VERIFY_STATUS_AND_RETURN(status);

  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  if (packet->length > 0) {
    memcpy(key_ptr, packet->data, packet->length);
  }
  sli_si91x_host_free_buffer(buffer);
  return status;
}

sl_status_t sl_si91x_puf_get_key_disable_req(void)
{
  // initial checks
  if (!sl_si91x_is_device_initialized()) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  sl_status_t status = SL_STATUS_OK;
  // driver send
  status = sli_si91x_driver_send_command(SLI_COMMON_REQ_PUF_DIS_GET_KEY,
                                         SI91X_COMMON_CMD,
                                         NULL,
                                         0,
                                         SLI_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                         NULL,
                                         NULL);
  VERIFY_STATUS_AND_RETURN(status);
  // change puf_state
  puf_state = SLI_SI91X_PUF_STATE_GET_KEY_DISABLED;

  return status;
}

sl_status_t sl_si91x_puf_load_key_req(const uint8_t *key_code_ptr)
{
  // initial checks
  if (!sl_si91x_is_device_initialized()) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  if (puf_state < SLI_SI91X_PUF_STATE_SET_KEY) {
    return SL_STATUS_INVALID_STATE;
  }
  if (key_code_ptr == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_status_t status                        = SL_STATUS_OK;
  sli_si91x_req_puf_load_key_t puf_load_key = { 0 };

  // Copy data to be sent
  memcpy(&puf_load_key.key_code[0], key_code_ptr, SLI_SI91X_PUF_KEY_CODE_SIZE_BYTES);

  // driver send
  status = sli_si91x_driver_send_command(SLI_COMMON_REQ_PUF_LOAD_KEY,
                                         SI91X_COMMON_CMD,
                                         &puf_load_key,
                                         sizeof(sli_si91x_req_puf_load_key_t),
                                         SL_SI91X_WAIT_FOR_RESPONSE(1000),
                                         NULL,
                                         NULL);

  VERIFY_STATUS_AND_RETURN(status);
  // change puf_state
  puf_state = SLI_SI91X_PUF_STATE_KEY_LOADED;

  return status;
}

sl_status_t sl_si91x_puf_set_intr_key_req(uint8_t key_index, uint8_t key_size, uint8_t *intr_key_resp)
{
  // initial checks
  if (!sl_si91x_is_device_initialized()) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  if (puf_state < SLI_SI91X_PUF_STATE_STARTED) {
    return SL_STATUS_INVALID_STATE;
  }
  if ((key_index > SLI_SI91X_MAX_PUF_KEY_INDEX) || (key_size > SL_SI91X_PUF_KEY_SIZE_256) || (intr_key_resp == NULL)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_status_t status                        = SL_STATUS_OK;
  sli_si91x_req_puf_intr_key_t puf_intr_key = { 0 };
  sl_wifi_buffer_t *buffer                  = NULL;
  sl_wifi_system_packet_t *packet           = NULL;

  // Assign key_index
  puf_intr_key.key_index = key_index;

  // Assign Key size
  puf_intr_key.key_size = key_size;

  // driver send
  status = sli_si91x_driver_send_command(SLI_COMMON_REQ_PUF_INTR_KEY,
                                         SI91X_COMMON_CMD,
                                         &puf_intr_key,
                                         sizeof(sli_si91x_req_puf_intr_key_t),
                                         SL_SI91X_WAIT_FOR_RESPONSE(1000),
                                         NULL,
                                         &buffer);

  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sli_si91x_host_free_buffer(buffer);
  }
  VERIFY_STATUS_AND_RETURN(status);

  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  if (packet->length > 0) {
    memcpy(intr_key_resp, packet->data, packet->length);
  }
  sli_si91x_host_free_buffer(buffer);
  return status;
}

// Common parameters check for AES encryption/decryption
static sl_status_t sli_puf_aes_common_checks(uint8_t mode,
                                             uint8_t key_source,
                                             uint16_t key_size,
                                             const uint8_t *key_ptr,
                                             uint16_t data_size,
                                             const uint8_t *data_ptr,
                                             uint16_t iv_size,
                                             const uint8_t *iv_ptr,
                                             const uint8_t *response_ptr)
{
  // initial checks
  if (!sl_si91x_is_device_initialized()) {
    SL_DEBUG_LOG("\r\nDevice not initialized\r\n");
    return SL_STATUS_NOT_INITIALIZED;
  }
  if (puf_state < SLI_SI91X_PUF_STATE_KEY_LOADED) {
    SL_DEBUG_LOG("\r\nInvalid PUF state\r\n");
    return SL_STATUS_INVALID_STATE;
  }
  // Data pointer check
  if (data_ptr == NULL) {
    SL_DEBUG_LOG("\r\nData pointer is NULL\r\n");
    return SL_STATUS_INVALID_PARAMETER;
  }
  // Response pointer/length check
  if (response_ptr == NULL) {
    SL_DEBUG_LOG("\r\nResponse pointer is NULL\r\n");
    return SL_STATUS_INVALID_PARAMETER;
  }
  if ((key_size > SL_SI91X_PUF_KEY_SIZE_256) || (iv_size > SL_SI91X_PUF_IV_SIZE_256)
      || (data_size > SLI_SI91X_MAX_PUF_DATA_SIZE_BYTES) || (data_size < SLI_SI91X_PUF_BLOCK_SIZE)
      || (mode > SL_SI91X_AES_CBC_MODE)) {
    SL_DEBUG_LOG("\r\nInvalid parameter size\r\n");
    return SL_STATUS_INVALID_PARAMETER;
  }
  // Check if key source is AES_AS_KEY_SOURCE and key_ptr is NULL or key length is not proper
  if ((key_source == SL_SI91X_AES_AS_KEY_SOURCE)
      && ((key_ptr == NULL) || (strlen((const char *)key_ptr) != (size_t)(key_size + 1) * SLI_SI91X_PUF_BLOCK_SIZE))) {
    SL_DEBUG_LOG("\r\nInvalid key source or key length\r\n");
    return SL_STATUS_INVALID_PARAMETER;
  }
  // Check if data size is not multiple of block size
  if (data_size % SLI_SI91X_PUF_BLOCK_SIZE != 0 || data_size > SLI_SI91X_MAX_PUF_DATA_SIZE_BYTES) {
    SL_DEBUG_LOG("\r\nInvalid data size\r\n");
    return SL_STATUS_INVALID_PARAMETER;
  }
  // Check if mode is AES_CBC_MODE and iv_ptr is NULL or iv length is not proper
  if ((mode == SL_SI91X_AES_CBC_MODE)
      && ((iv_ptr == NULL) || strlen((const char *)iv_ptr) != (size_t)(iv_size + 1) * SLI_SI91X_PUF_BLOCK_SIZE)) {
    SL_DEBUG_LOG("\r\nInvalid IV for AES CBC mode\r\n");
    return SL_STATUS_INVALID_PARAMETER;
  }
  return SL_STATUS_OK;
}

sl_status_t sl_si91x_puf_aes_encrypt_req(uint8_t mode,
                                         uint8_t key_source,
                                         uint16_t key_size,
                                         const uint8_t *key_ptr,
                                         uint16_t data_size,
                                         const uint8_t *data_ptr,
                                         uint16_t iv_size,
                                         const uint8_t *iv_ptr,
                                         uint8_t *aes_encry_resp)
{
  sl_status_t status = sli_puf_aes_common_checks(mode,
                                                 key_source,
                                                 key_size,
                                                 key_ptr,
                                                 data_size,
                                                 data_ptr,
                                                 iv_size,
                                                 iv_ptr,
                                                 aes_encry_resp);
  if (status != SL_STATUS_OK) {
    return status;
  }

  sli_si91x_req_aes_encrypt_t aes_encry = { 0 };
  sl_wifi_buffer_t *buffer              = NULL;
  sl_wifi_system_packet_t *packet       = NULL;

  if (mode == SL_SI91X_AES_CBC_MODE) {
    // Copy IV
    memcpy(&aes_encry.iv[0], iv_ptr, ((iv_size + 1) * SLI_SI91X_PUF_BLOCK_SIZE));
  }
  // Copy Key
  if (key_source == SL_SI91X_AES_AS_KEY_SOURCE) {
    memcpy(&aes_encry.key[0], key_ptr, (key_size + 1) * SLI_SI91X_PUF_BLOCK_SIZE);
  }
  // Fill AES Encry Mode
  aes_encry.mode = (mode | (key_size << 1) | (key_source << 2) | (iv_size << 3));
  // Copy Data
  memcpy(&aes_encry.data, data_ptr, data_size);

  //Fill length
  sli_uint16_to_2bytes_util(data_size, aes_encry.data_size);

  // driver send
  status = sli_si91x_driver_send_command(SLI_COMMON_REQ_AES_ENCRYPT,
                                         SI91X_COMMON_CMD,
                                         &aes_encry,
                                         sizeof(sli_si91x_req_aes_encrypt_t),
                                         SL_SI91X_WAIT_FOR_RESPONSE(1000),
                                         NULL,
                                         &buffer);

  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sli_si91x_host_free_buffer(buffer);
  }
  VERIFY_STATUS_AND_RETURN(status);

  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  if (packet->length > 0) {
    memcpy(aes_encry_resp, packet->data, packet->length);
  }
  sli_si91x_host_free_buffer(buffer);
  return status;
}

sl_status_t sl_si91x_puf_aes_decrypt_req(uint8_t mode,
                                         uint8_t key_source,
                                         uint16_t key_size,
                                         const uint8_t *key_ptr,
                                         uint16_t data_size,
                                         const uint8_t *data_ptr,
                                         uint16_t iv_size,
                                         const uint8_t *iv_ptr,
                                         uint8_t *aes_decry_resp)
{
  sl_status_t status = sli_puf_aes_common_checks(mode,
                                                 key_source,
                                                 key_size,
                                                 key_ptr,
                                                 data_size,
                                                 data_ptr,
                                                 iv_size,
                                                 iv_ptr,
                                                 aes_decry_resp);
  if (status != SL_STATUS_OK) {
    return status;
  }

  sli_si91x_req_aes_decrypt_t aes_decry = { 0 };
  sl_wifi_buffer_t *buffer              = NULL;
  sl_wifi_system_packet_t *packet       = NULL;

  if (mode == SL_SI91X_AES_CBC_MODE) {
    // Copy IV
    memcpy(&aes_decry.iv[0], iv_ptr, ((iv_size + 1) * SLI_SI91X_PUF_BLOCK_SIZE));
  }
  if (key_source == SL_SI91X_AES_AS_KEY_SOURCE) {
    // Copy Key
    memcpy(&aes_decry.key[0], key_ptr, (key_size + 1) * SLI_SI91X_PUF_BLOCK_SIZE);
  }
  //Fill AES Encry Mode
  aes_decry.mode = (mode | (key_size << 1) | (key_source << 2) | (iv_size << 3));

  // Copy Data
  memcpy(&aes_decry.data[0], data_ptr, data_size);

  //Fill length
  sli_uint16_to_2bytes_util(data_size, aes_decry.data_size);

  // driver send
  status = sli_si91x_driver_send_command(SLI_COMMON_REQ_AES_DECRYPT,
                                         SI91X_COMMON_CMD,
                                         &aes_decry,
                                         sizeof(sli_si91x_req_aes_decrypt_t),
                                         SL_SI91X_WAIT_FOR_RESPONSE(1000),
                                         NULL,
                                         &buffer);

  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sli_si91x_host_free_buffer(buffer);
  }
  VERIFY_STATUS_AND_RETURN(status);

  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  if (packet->length > 0) {
    memcpy(aes_decry_resp, packet->data, packet->length);
  }
  sli_si91x_host_free_buffer(buffer);
  return status;
}

sl_status_t sl_si91x_puf_aes_mac_req(uint8_t key_source,
                                     uint16_t key_size,
                                     const uint8_t *key_ptr,
                                     uint16_t data_size,
                                     const uint8_t *data_ptr,
                                     uint16_t iv_size,
                                     const uint8_t *iv_ptr,
                                     uint8_t *aes_mac_resp)
{
  sl_status_t status =
    sli_puf_aes_common_checks(0, key_source, key_size, key_ptr, data_size, data_ptr, iv_size, iv_ptr, aes_mac_resp);
  if (status != SL_STATUS_OK) {
    return status;
  }

  sli_si91x_req_aes_mac_t aes_mac = { 0 };
  sl_wifi_buffer_t *buffer        = NULL;
  sl_wifi_system_packet_t *packet = NULL;

  //Fill AES Encry Mode
  aes_mac.mode = ((key_size << 1) | (key_source << 2) | (iv_size << 3));

  //Fill length
  sli_uint16_to_2bytes_util(data_size, aes_mac.data_size);

  // Data
  memcpy(&aes_mac.data[0], data_ptr, data_size);

  // Key
  if (key_source == SL_SI91X_AES_AS_KEY_SOURCE) {
    memcpy(&aes_mac.key[0], key_ptr, (key_size + 1) * SLI_SI91X_PUF_BLOCK_SIZE);
  }
  // IV
  memcpy(&aes_mac.iv[0], iv_ptr, (iv_size + 1) * SLI_SI91X_PUF_BLOCK_SIZE);

  // driver send
  status = sli_si91x_driver_send_command(SLI_COMMON_REQ_AES_MAC,
                                         SI91X_COMMON_CMD,
                                         &aes_mac,
                                         sizeof(sli_si91x_req_aes_mac_t),
                                         SL_SI91X_WAIT_FOR_RESPONSE(1000),
                                         NULL,
                                         &buffer);

  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sli_si91x_host_free_buffer(buffer);
  }
  VERIFY_STATUS_AND_RETURN(status);

  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  if (packet->length > 0) {
    memcpy(aes_mac_resp, packet->data, packet->length);
  }
  sli_si91x_host_free_buffer(buffer);
  return status;
}
