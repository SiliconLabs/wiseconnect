/*******************************************************************************
 * @file  sl_si91x_puf.h
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#pragma once
#include "sl_si91x_constants.h"
#include "sl_status.h"

/******************************************************
 *                      Macros
 *****************************************************/
/******************************************************
 *                    Constants
 *****************************************************/
/** @addtogroup CRYPTO_PUF_TYPES
 * @{
 */

/******************************************************
 *                   Enumerations
 *****************************************************/
/**
 * @brief Key size identifiers.
 */
typedef enum {
  SL_SI91X_PUF_KEY_SIZE_128 = 0, ///< 128-bit key size.
  SL_SI91X_PUF_KEY_SIZE_256 = 1  ///< 256-bit key size.
} sl_si91x_puf_key_size_t;

/**
 * @brief AES mode identifiers.
 */
typedef enum {
  SL_SI91X_AES_ECB_MODE = 0, ///< AES mode for ECB.
  SL_SI91X_AES_CBC_MODE = 1  ///< AES mode for CBC.
} sl_si91x_puf_aes_mode_t;

/**
 * @brief Key source identifiers.
 */
typedef enum {
  SL_SI91X_PUF_AS_KEY_SOURCE = 0, ///< PUF as key source to AES.
  SL_SI91X_AES_AS_KEY_SOURCE = 1  ///< Input key as key source to AES.
} sl_si91x_puf_key_source_t;

/**
 * @brief Initialization Vector (IV) size identifiers.
 */
typedef enum {
  SL_SI91X_PUF_IV_SIZE_128 = 0, ///< 128-bit IV size.
  SL_SI91X_PUF_IV_SIZE_256 = 1  ///< 256-bit IV size.
} sl_si91x_puf_iv_size_t;

/** @} */

/******************************************************
 *                  Type Definitions
 *****************************************************/

/******************************************************
 *                    Structures
 *****************************************************/

/******************************************************
 *                 Global Variables
 *****************************************************/
/******************************************************
 *               Function Declarations
 *****************************************************/
/**
 * @addtogroup CRYPTO_PUF_FUNCTIONS
 * @{
 */

/**
 * @brief Starts PUF process.
 * 
 * @details 
 *  Device should contain a valid activation code (AC) in Flash. The activation code and the startup data are used to reconstruct the digital fingerprint. 
 *  The activation code is generated by the manufacturer and flashed into the device.
 *  To generate and flash the activation code using Simplicity Commander, follow these steps:
 *  1. Connect the device to the host using a USB cable.
 *  2. Open terminal and enter the following command: 
 *   ```sh
 *   commander manufacturing init --mbr ta_mbr_{OPN}.bin
 *   ```
 *
 * @return 
 *  sl_status_t. See [Status Codes](https://docs.silabs.com/gecko-platform/latest/platform-common/status) and [WiSeConnect Status Codes](../wiseconnect-api-reference-guide-err-codes/wiseconnect-status-codes) for details.
 * 
 * @note
 *  This function needs to be called after every power-up and reset.
 */
sl_status_t sl_si91x_puf_start_req(void);

/**
 * @brief Sets key for the PUF.
 * 
 * @details
 *  The digital fingerprint generated during the PUF start operations and a User Key (UK) 
 *  provided by the client design are used to generate a Key Code (KC).
 * 
 * @param[in]  key_index    Key index ( valid range is 0 - 15 )
 * @param[in]  key_size     Key size of type @ref sl_si91x_puf_key_size_t
 * @param[in]  key_ptr      Pointer to key of size 16 or 32 bytes based on the key size
 * @param[out] key_code_ptr Pointer to key code with size of 52 bytes.
 * 
 * @return 
 *  sl_status_t. See [Status Codes](https://docs.silabs.com/gecko-platform/latest/platform-common/status) and [WiSeConnect Status Codes](../wiseconnect-api-reference-guide-err-codes/wiseconnect-status-codes) for details.
 * 
 * @note
 *  This operation needs to be done only once for each user key.
 */
sl_status_t sl_si91x_puf_set_key_req(uint8_t key_index,
                                     uint8_t key_size,
                                     const uint8_t *key_ptr,
                                     uint8_t *key_code_ptr);

/**
 * @brief Blocks further set key operation on PUF.
 * 
 * @return 
 *  sl_status_t. See [Status Codes](https://docs.silabs.com/gecko-platform/latest/platform-common/status) and [WiSeConnect Status Codes](../wiseconnect-api-reference-guide-err-codes/wiseconnect-status-codes) for details.
 */
sl_status_t sl_si91x_puf_set_key_disable_req(void);

/**
 * @brief Performs a get key operation with the provided key code.
 * 
 * @details
 *  The digital fingerprint generated during the start operation and the KC generated 
 *  during a set key operation @ref sl_si91x_puf_set_key_req, is used to retrieve a stored key.
 * 
 * @param[in]  key_code_ptr Pointer to the Key Code with size of 52 bytes.
 * @param[out] key_ptr  Pointer to stored key response. The size of the response can be 16 or 32 bytes based on the key size.
 *  
 * @pre Pre-conditions:
 *  To retrieve the `key_ptr` (UK), user must set the `key_index` in @ref sl_si91x_puf_set_key_req to a value between 1 and 15 (inclusive).
 *  
 * @return 
 *  sl_status_t. See [Status Codes](https://docs.silabs.com/gecko-platform/latest/platform-common/status) and [WiSeConnect Status Codes](../wiseconnect-api-reference-guide-err-codes/wiseconnect-status-codes) for details.
 * 
 * @note
 *  This operation needs to be done every time a key is needed.
 */
sl_status_t sl_si91x_puf_get_key_req(uint8_t *key_code_ptr, uint8_t *key_ptr);

/**
 * @brief Disables further key retrieval operations on PUF.
 * 
 * @return 
 *  sl_status_t. See [Status Codes](https://docs.silabs.com/gecko-platform/latest/platform-common/status) and [WiSeConnect Status Codes](../wiseconnect-api-reference-guide-err-codes/wiseconnect-status-codes) for details.
 */
sl_status_t sl_si91x_puf_get_key_disable_req(void);

/**
 * @brief Loads key code using PUF to AES engine.
 * 
 * @details
 *  This is same as get key operation, but after retrieving key using @ref sl_si91x_puf_set_key_req, it loads into AES engine. 
 *  
 * @param[in] key_code_ptr Pointer to the key code with size of 52 bytes.
 * 
 * @pre Pre-conditions:
 *  The user must set the `key_index` in @ref sl_si91x_puf_set_key_req to `0`, 
 *  indicating that only the Master Key can be loaded into the AES engine.
 * 
 * @return 
 *  sl_status_t. See [Status Codes](https://docs.silabs.com/gecko-platform/latest/platform-common/status) and [WiSeConnect Status Codes](../wiseconnect-api-reference-guide-err-codes/wiseconnect-status-codes) for details.
 * 
 * @note
 *  This operation needs to be done for different keys to be loaded into AES engine.
 */
sl_status_t sl_si91x_puf_load_key_req(uint8_t *key_code_ptr);

/**
 * @brief Sets intrinsic key operation
 * 
 * @details
 *  The digital fingerprint generated during the Enroll/Start operations @ref sl_si91x_puf_start_req is used to generate a key code (KC) that defines a unique intrinsic key. 
 *  This operation needs to be done only once for each intrinsic key. Each time a Set Intrinsic Key operation is executed a new unique key is generated.
 * 
 * @param[in]  key_index     Key index ( valid range is 0 - 15 )
 * @param[in]  key_size      Key size of type @ref sl_si91x_puf_key_size_t
 * @param[out] intr_key_resp Pointer to response
 * 
 * @return 
 *  sl_status_t. See [Status Codes](https://docs.silabs.com/gecko-platform/latest/platform-common/status) and [WiSeConnect Status Codes](../wiseconnect-api-reference-guide-err-codes/wiseconnect-status-codes) for details.
 * 
 * @note 
 *  Intrinsic keys are randomly generated. They cannot be used for cryptographic algorithms that require specific keys, which is typical for public key schemes like RSA. In such cases, user keys should be used.
 */
sl_status_t sl_si91x_puf_set_intr_key_req(uint8_t key_index, uint8_t key_size, uint8_t *intr_key_resp);

/**
 * @brief Encrypts data using the provided initialization vector and key, depending on the mode.
 * 
 * @details
 *  This operation generates encrypted data for the provided plain input data based on AES mode, key input and initialization vector.
 * 
 * @param[in]  mode           AES mode of type @ref sl_si91x_puf_aes_mode_t
 * @param[in]  key_source     Key source of type @ref sl_si91x_puf_key_source_t
 * @param[in]  key_size       Key size of type @ref sl_si91x_puf_key_size_t
 * @param[in]  key_ptr        Pointer to key of size 16 or 32 bytes based on key size.(valid only if key_source is @ref sl_si91x_puf_key_source_t::SL_SI91X_AES_AS_KEY_SOURCE)
 * @param[in]  data_size      Size of data in bytes (value that should be multiples of AES block size in range 16 - 1408)
 * @param[in]  data_ptr       Pointer to Data
 * @param[in]  iv_size        IV size of type @ref sl_si91x_puf_iv_size_t
 * @param[in]  iv_ptr         Pointer to Initialization Vector(IV) of size 16 or 32 bytes based on IV size. (valid only if mode is @ref sl_si91x_puf_aes_mode_t::SL_SI91X_AES_CBC_MODE)
 * @param[out] aes_encry_resp Pointer to response
 * 
 * @return 
 *  sl_status_t. See [Status Codes](https://docs.silabs.com/gecko-platform/latest/platform-common/status) and [WiSeConnect Status Codes](../wiseconnect-api-reference-guide-err-codes/wiseconnect-status-codes) for details.
 */
sl_status_t sl_si91x_puf_aes_encrypt_req(uint8_t mode,
                                         uint8_t key_source,
                                         uint16_t key_size,
                                         const uint8_t *key_ptr,
                                         uint16_t data_size,
                                         uint8_t *data_ptr,
                                         uint16_t iv_size,
                                         uint8_t *iv_ptr,
                                         uint8_t *aes_encry_resp);

/**
 * @brief Decrypts data using the provided initialization vector and key, depending on the mode.
 * 
 * @details
 *  This operation generates decrypted data for the provided encrypted data based on AES mode, key input and initialization vector.
 * 
 * @param[in]  mode           AES mode of type @ref sl_si91x_puf_aes_mode_t
 * @param[in]  key_source     Key source of type @ref sl_si91x_puf_key_source_t
 * @param[in]  key_size       Key size of type @ref sl_si91x_puf_key_size_t
 * @param[in]  key_ptr        Pointer to key of size 16 or 32 bytes based on key size.(valid only if key_source is @ref sl_si91x_puf_key_source_t::SL_SI91X_AES_AS_KEY_SOURCE)
 * @param[in]  data_size      Size of data in bytes (value that should be multiples of AES block size in range 16 - 1408)
 * @param[in]  data_ptr       Pointer to Data
 * @param[in]  iv_size        IV size of type @ref sl_si91x_puf_iv_size_t
 * @param[in]  iv_ptr         Pointer to Initialization Vector(IV) of size 16 or 32 bytes based on IV size.
 * @param[out] aes_decry_resp Pointer to response
 * 
 * @return 
 *  sl_status_t. See [Status Codes](https://docs.silabs.com/gecko-platform/latest/platform-common/status) and [WiSeConnect Status Codes](../wiseconnect-api-reference-guide-err-codes/wiseconnect-status-codes) for details.
 */
sl_status_t sl_si91x_puf_aes_decrypt_req(uint8_t mode,
                                         uint8_t key_source,
                                         uint16_t key_size,
                                         const uint8_t *key_ptr,
                                         uint16_t data_size,
                                         uint8_t *data_ptr,
                                         uint16_t iv_size,
                                         uint8_t *iv_ptr,
                                         uint8_t *aes_decry_resp);

/**
 * @brief Calculates Message Authentication Check (MAC) for given data using AES-CBC mode.
 * 
 * @details
 *  This operation generates Message Authentication Check (MAC) for the provided plain input data based on key input and initialization vector (IV). 
 *  This operation is only used in AES-CBC mode.
 * 
 * @param[in]  key_source   Key source of type @ref sl_si91x_puf_key_source_t
 * @param[in]  key_size     Key size of type @ref sl_si91x_puf_key_size_t
 * @param[in]  key_ptr      Pointer to key of size 16 or 32 bytes based on key size. (valid only if key_source is @ref sl_si91x_puf_key_source_t::SL_SI91X_AES_AS_KEY_SOURCE)
 * @param[in]  data_size    Size of data in bytes (value that should be multiples of AES block size in range 16 - 1408)
 * @param[in]  data_ptr     Pointer to Data
 * @param[in]  iv_size      IV size of type @ref sl_si91x_puf_iv_size_t
 * @param[in]  iv_ptr       Pointer to IV of size 16 or 32 bytes based on IV size.
 * @param[out] aes_mac_resp Pointer to response
 * 
 * @return 
 *  sl_status_t. See [Status Codes](https://docs.silabs.com/gecko-platform/latest/platform-common/status) and [WiSeConnect Status Codes](../wiseconnect-api-reference-guide-err-codes/wiseconnect-status-codes) for details.
 */
sl_status_t sl_si91x_puf_aes_mac_req(uint8_t key_source,
                                     uint16_t key_size,
                                     const uint8_t *key_ptr,
                                     uint16_t data_size,
                                     uint8_t *data_ptr,
                                     uint16_t iv_size,
                                     uint8_t *iv_ptr,
                                     uint8_t *aes_mac_resp);

/** @} */
