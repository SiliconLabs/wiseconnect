/*******************************************************************************
 * @file  sl_si91x_psa_aead.c
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include "sli_si91x_crypto_driver_functions.h"
#if defined(SLI_PSA_DRIVER_FEATURE_CCM)
#include "sl_si91x_ccm.h"
#endif
#if defined(SLI_PSA_DRIVER_FEATURE_GCM)
#include "sl_si91x_gcm.h"
#endif
#if defined(SLI_PSA_DRIVER_FEATURE_CHACHAPOLY)
#include "sl_si91x_chachapoly.h"
#endif
#include "sl_si91x_crypto.h"
#include "sl_status.h"
#include "sl_constants.h"
#include "sl_si91x_protocol_types.h"
#include "sl_si91x_driver.h"
#include <string.h>

/**
 * \brief Verify the parameters of authenticated encryption and decryption operations.
 *
 * \param[in]  attributes         The attributes of the key to use for the
 *                                operation.
 * \param      alg                The AEAD algorithm to compute.
 * \param      nonce_length       Size of the nonce buffer in bytes. This must
 *                                be appropriate for the selected algorithm.
 *                                The default nonce size is
 *                                PSA_AEAD_NONCE_LENGTH(key_type, alg) where
 *                                key_type is the type of key.
 * \param      additional_data_length  Size of additional_data in bytes.
 *
 * \retval #PSA_SUCCESS Success.
 * \retval #PSA_ERROR_NOT_SUPPORTED
 *         \p alg is not supported.
 * \retval #PSA_ERROR_INVALID_ARGUMENT
 */
static psa_status_t check_aead_parameters(const psa_key_attributes_t *attributes,
                                          psa_algorithm_t alg,
                                          size_t nonce_length,
                                          size_t additional_data_length)
{
  size_t tag_length = PSA_AEAD_TAG_LENGTH(psa_get_key_type(attributes), psa_get_key_bits(attributes), alg);

  switch (PSA_ALG_AEAD_WITH_SHORTENED_TAG(alg, 0)) {
#if defined(SLI_PSA_DRIVER_FEATURE_CCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 0):
      // verify key type
      if (psa_get_key_type(attributes) != PSA_KEY_TYPE_AES) {
        return PSA_ERROR_NOT_SUPPORTED;
      }
      switch (psa_get_key_bits(attributes)) {
        case 128: // Fallthrough
        case 192: // Fallthrough
        case 256:
          break;
        default:
          return PSA_ERROR_INVALID_ARGUMENT;
      }
      // verify nonce and tag lengths
      if ((tag_length < 4) || (tag_length > 16) || (tag_length % 2 != 0) || (nonce_length < SL_SI91X_CCM_IV_MIN_SIZE)
          || (nonce_length > SL_SI91X_CCM_IV_MAX_SIZE)) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
      break;
#endif // SLI_PSA_DRIVER_FEATURE_CCM
#if defined(SLI_PSA_DRIVER_FEATURE_GCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 0):
      // verify key type
      if (psa_get_key_type(attributes) != PSA_KEY_TYPE_AES) {
        return PSA_ERROR_NOT_SUPPORTED;
      }
      switch (psa_get_key_bits(attributes)) {
        case 128: // Fallthrough
        case 192: // Fallthrough
        case 256:
          break;
        default:
          return PSA_ERROR_INVALID_ARGUMENT;
      }
      // verify nonce and tag lengths
      if ((tag_length < 4) || (tag_length > 16) || nonce_length != SL_SI91X_GCM_IV_SIZE) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
      break;
#endif // SLI_PSA_DRIVER_FEATURE_GCM
#if defined(SLI_PSA_DRIVER_FEATURE_CHACHAPOLY)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CHACHA20_POLY1305, 0):
      // verify key type
      if (psa_get_key_type(attributes) != PSA_KEY_TYPE_CHACHA20) {
        return PSA_ERROR_NOT_SUPPORTED;
      }
      switch (psa_get_key_bits(attributes)) {
        case 256:
          break;
        default:
          return PSA_ERROR_INVALID_ARGUMENT;
      }
      // verify nonce and tag lengths
      if (tag_length != SL_SI91X_TAG_SIZE || nonce_length != SL_SI91X_CHACHAPOLY_IV_SIZE) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
      break;
#endif // SLI_PSA_DRIVER_FEATURE_CHACHAPOLY
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  return PSA_SUCCESS;
}

/**
 * \brief Process an authenticated encryption operation.
 *
 * \note The signature of this function is that of a PSA driver
 *       aead_encrypt entry point. This function behaves as an aead_encrypt
 *       entry point as defined in the PSA driver interface specification for
 *       transparent drivers.
 *
 * \param[in]  attributes         The attributes of the key to use for the
 *                                operation.
 * \param[in]  key_buffer         The buffer containing the key context.
 * \param      key_buffer_size    Size of the \p key_buffer buffer in bytes.
 * \param      alg                The AEAD algorithm to compute.
 * \param[in]  nonce              Nonce or IV to use.
 * \param      nonce_length       Size of the nonce buffer in bytes. This must
 *                                be appropriate for the selected algorithm.
 *                                The default nonce size is
 *                                PSA_AEAD_NONCE_LENGTH(key_type, alg) where
 *                                key_type is the type of key.
 * \param[in]  additional_data    Additional data that will be authenticated
 *                                but not encrypted.
 * \param      additional_data_length  Size of additional_data in bytes.
 * \param[in]  plaintext          Data that will be authenticated and encrypted.
 * \param      plaintext_length   Size of plaintext in bytes.
 * \param[out] ciphertext         Output buffer for the authenticated and
 *                                encrypted data. The additional data is not
 *                                part of this output. For algorithms where the
 *                                encrypted data and the authentication tag are
 *                                defined as separate outputs, the
 *                                authentication tag is appended to the
 *                                encrypted data.
 * \param      ciphertext_size    Size of the ciphertext buffer in bytes. This
 *                                must be appropriate for the selected algorithm
 *                                and key:
 *                                - A sufficient output size is
 *                                  PSA_AEAD_ENCRYPT_OUTPUT_SIZE(key_type, alg,
 *                                  plaintext_length) where key_type is the type
 *                                  of key.
 *                                - PSA_AEAD_ENCRYPT_OUTPUT_MAX_SIZE(
 *                                  plaintext_length) evaluates to the maximum
 *                                  ciphertext size of any supported AEAD
 *                                  encryption.
 * \param[out] ciphertext_length  On success, the size of the output in the
 *                                ciphertext buffer.
 *
 * \retval #PSA_SUCCESS Success.
 * \retval #PSA_ERROR_NOT_SUPPORTED
 *         \p alg is not supported.
 * \retval #PSA_ERROR_INSUFFICIENT_MEMORY
 * \retval #PSA_ERROR_BUFFER_TOO_SMALL
 *         ciphertext_size is too small.
 * \retval #PSA_ERROR_CORRUPTION_DETECTED
 */

psa_status_t sli_si91x_crypto_aead_encrypt(const psa_key_attributes_t *attributes,
                                           const uint8_t *key_buffer,
                                           size_t key_buffer_size,
                                           psa_algorithm_t alg,
                                           const uint8_t *nonce,
                                           size_t nonce_length,
                                           const uint8_t *additional_data,
                                           size_t additional_data_length,
                                           const uint8_t *plaintext,
                                           size_t plaintext_length,
                                           uint8_t *ciphertext,
                                           size_t ciphertext_size,
                                           size_t *ciphertext_length)
{
  psa_status_t status = PSA_ERROR_GENERIC_ERROR;

  // Input check
  if ((key_buffer == NULL) || (key_buffer_size <= 0) || (attributes == NULL) || (nonce == NULL)
      || (additional_data == NULL && additional_data_length > 0) || (plaintext == NULL && plaintext_length > 0)
      || (plaintext_length > 0 && (ciphertext == NULL || ciphertext_size == 0)) || (ciphertext_length == NULL)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Verify that the driver supports the given parameters
  status = check_aead_parameters(attributes, alg, nonce_length, additional_data_length);
  if (status != PSA_SUCCESS) {
    return status;
  }

  psa_key_type_t key_type = psa_get_key_type(attributes);
  size_t key_bits         = psa_get_key_bits(attributes);
  uint8_t tag_length      = PSA_AEAD_TAG_LENGTH(key_type, key_bits, alg);

  // Check sufficient output buffer size.
  if (ciphertext_size < (plaintext_length + tag_length)) {
    return PSA_ERROR_BUFFER_TOO_SMALL;
  }
  *ciphertext_length = 0;

  sl_status_t si91x_status;
  switch (PSA_ALG_AEAD_WITH_SHORTENED_TAG(alg, 0)) {
#if defined(PSA_WANT_ALG_CCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 0):;
      sl_si91x_ccm_config_t config_ccm;
      config_ccm.encrypt_decrypt = SL_SI91X_CCM_ENCRYPT;
      config_ccm.msg             = plaintext;
      config_ccm.msg_length      = plaintext_length;
      config_ccm.nonce           = nonce;
      config_ccm.nonce_length    = nonce_length;
      config_ccm.ad              = additional_data;
      config_ccm.ad_length       = additional_data_length;
      config_ccm.tag             = ciphertext + plaintext_length;
      config_ccm.tag_length      = tag_length;

#ifdef CHIP_917B0
      /* Fetch key type from attributes */
      psa_key_location_t location_ccm = PSA_KEY_LIFETIME_GET_LOCATION(psa_get_key_lifetime(attributes));
      if (location_ccm == 0) {
        config_ccm.key_config.b0.key_type = SL_SI91X_TRANSPARENT_KEY;
      } else {
        config_ccm.key_config.b0.key_type = SL_SI91X_WRAPPED_KEY;
      }

      /* Set key_size from key_buffer_size */
      if (key_buffer_size == 16) {
        config_ccm.key_config.b0.key_size = SL_SI91X_CCM_KEY_SIZE_128;
      }
      if (key_buffer_size == 24) {
        config_ccm.key_config.b0.key_size = SL_SI91X_CCM_KEY_SIZE_192;
      }
      if (key_buffer_size == 32) {
        config_ccm.key_config.b0.key_size = SL_SI91X_CCM_KEY_SIZE_256;
      }

      config_ccm.key_config.b0.key_slot     = 0;
      config_ccm.key_config.b0.wrap_iv_mode = SL_SI91X_WRAP_IV_CBC_MODE;
      if (config_ccm.key_config.b0.wrap_iv_mode == SL_SI91X_WRAP_IV_CBC_MODE) {
        memcpy(config_ccm.key_config.b0.wrap_iv, WRAP_IV, SL_SI91X_IV_SIZE);
      }
      memcpy(config_ccm.key_config.b0.key_buffer, key_buffer, config_ccm.key_config.b0.key_size);
#else
      config_ccm.key_config.a0.key_length = key_buffer_size;
      memcpy(config_ccm.key_config.a0.key, key_buffer, config_ccm.key_config.a0.key_length);
#endif

      if (status != PSA_SUCCESS) {
        return status;
      }
      /* Calling sl_si91x_ccm() for CCM encryption */
      si91x_status = sl_si91x_ccm(&config_ccm, ciphertext);

      /* gets the si91x error codes and returns its equivalent psa_status codes */
      status = convert_si91x_error_code_to_psa_status(si91x_status);

      break;
#endif /* PSA_WANT_ALG_CCM */
#if defined(PSA_WANT_ALG_GCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 0):;
      sl_si91x_gcm_config_t config_gcm;
      config_gcm.encrypt_decrypt = SL_SI91X_GCM_ENCRYPT;
      config_gcm.dma_use         = SL_SI91X_DMA_ENABLE;
      config_gcm.msg             = plaintext;
      config_gcm.msg_length      = plaintext_length;
      config_gcm.nonce           = nonce;
      config_gcm.nonce_length    = nonce_length;
      config_gcm.ad              = additional_data;
      config_gcm.ad_length       = additional_data_length;

#ifdef CHIP_917B0
      /* Fetch key type from attributes */
      psa_key_location_t location_gcm = PSA_KEY_LIFETIME_GET_LOCATION(psa_get_key_lifetime(attributes));
      if (location_gcm == 0) {
        config_gcm.key_config.b0.key_type = SL_SI91X_TRANSPARENT_KEY;
      } else {
        config_gcm.key_config.b0.key_type = SL_SI91X_WRAPPED_KEY;
      }

      /* Set key_size from key_buffer_size */
      if (key_buffer_size == 16) {
        config_gcm.key_config.b0.key_size = SL_SI91X_GCM_KEY_SIZE_128;
      }
      if (key_buffer_size == 24) {
        config_gcm.key_config.b0.key_size = SL_SI91X_GCM_KEY_SIZE_192;
      }
      if (key_buffer_size == 32) {
        config_gcm.key_config.b0.key_size = SL_SI91X_GCM_KEY_SIZE_256;
      }

      config_gcm.gcm_mode                   = SL_SI91X_GCM_MODE;
      config_gcm.key_config.b0.key_slot     = 0;
      config_gcm.key_config.b0.wrap_iv_mode = SL_SI91X_WRAP_IV_CBC_MODE;
      if (config_gcm.key_config.b0.wrap_iv_mode == SL_SI91X_WRAP_IV_CBC_MODE) {
        memcpy(config_gcm.key_config.b0.wrap_iv, WRAP_IV, SL_SI91X_IV_SIZE);
      }
      memcpy(config_gcm.key_config.b0.key_buffer, key_buffer, config_gcm.key_config.b0.key_size);
#else
      config_gcm.key_config.a0.key_length = key_buffer_size;
      memcpy(config_gcm.key_config.a0.key, key_buffer, config_gcm.key_config.a0.key_length);
#endif

      if (status != PSA_SUCCESS) {
        return status;
      }
      /* Calling sl_si91x_gcm() for GCM encryption */
      si91x_status = sl_si91x_gcm(&config_gcm, ciphertext);

      /* gets the si91x error codes and returns its equivalent psa_status codes */
      status = convert_si91x_error_code_to_psa_status(si91x_status);

      break;
#endif /* PSA_WANT_ALG_GCM */
#if defined(PSA_WANT_ALG_CHACHA20_POLY1305)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CHACHA20_POLY1305, 0):;
      sl_si91x_chachapoly_config_t config_chachapoly;
      config_chachapoly.encrypt_decrypt = SL_SI91X_CHACHAPOLY_ENCRYPT;
      config_chachapoly.dma_use         = SL_SI91X_DMA_ENABLE;
      config_chachapoly.chachapoly_mode = SL_SI91X_CHACHA20POLY1305_MODE;
      config_chachapoly.msg             = plaintext;
      config_chachapoly.msg_length      = plaintext_length;
      config_chachapoly.nonce           = nonce;
      config_chachapoly.ad              = additional_data;
      config_chachapoly.ad_length       = additional_data_length;

#ifdef CHIP_917B0
      /* Fetch key type from attributes */
      psa_key_location_t location_chachapoly = PSA_KEY_LIFETIME_GET_LOCATION(psa_get_key_lifetime(attributes));
      if (location_chachapoly == 0) {
        config_chachapoly.key_config.b0.key_type = SL_SI91X_TRANSPARENT_KEY;
      } else {
        config_chachapoly.key_config.b0.key_type = SL_SI91X_WRAPPED_KEY;
      }

      config_chachapoly.key_config.b0.key_size     = SL_SI91X_CHACHAPOLY_KEY_SIZE_256;
      config_chachapoly.key_config.b0.key_slot     = 0;
      config_chachapoly.key_config.b0.wrap_iv_mode = SL_SI91X_WRAP_IV_CBC_MODE;
      if (config_chachapoly.key_config.b0.wrap_iv_mode == SL_SI91X_WRAP_IV_CBC_MODE) {
        memcpy(config_chachapoly.key_config.b0.wrap_iv, WRAP_IV, SL_SI91X_IV_SIZE);
      }
      memcpy(config_chachapoly.key_config.b0.key_buffer, key_buffer, config_chachapoly.key_config.b0.key_size);
#else
      memcpy(config_chachapoly.key_config.a0.key_chacha, key_buffer, SL_SI91X_CHACHAPOLY_KEY_SIZE_256);
#endif

      if (status != PSA_SUCCESS) {
        return status;
      }
      /* Calling sl_si91x_chachapoly() for CHACHAPOLY encryption */
      si91x_status = sl_si91x_chachapoly(&config_chachapoly, ciphertext);

      /* gets the si91x error codes and returns its equivalent psa_status codes */
      status = convert_si91x_error_code_to_psa_status(si91x_status);

      break;
#endif /* PSA_WANT_ALG_CHACHAPOLY */
    default:
      (void)status;
      (void)key_buffer;
      status = PSA_ERROR_NOT_SUPPORTED;
      break;
  }
  if (status == PSA_SUCCESS) {
    *ciphertext_length = plaintext_length + tag_length;
  }
  return status;
}

/**
 * \brief Process an authenticated decryption operation.
 *
 * \note The signature of this function is that of a PSA driver
 *       aead_decrypt entry point. This function behaves as an aead_decrypt
 *       entry point as defined in the PSA driver interface specification for
 *       transparent drivers.
 *
 * \param[in]  attributes         The attributes of the key to use for the
 *                                operation.
 * \param[in]  key_buffer         The buffer containing the key context.
 * \param      key_buffer_size    Size of the \p key_buffer buffer in bytes.
 * \param      alg                The AEAD algorithm to compute.
 * \param[in]  nonce              Nonce or IV to use.
 * \param      nonce_length       Size of the nonce buffer in bytes. This must
 *                                be appropriate for the selected algorithm.
 *                                The default nonce size is
 *                                PSA_AEAD_NONCE_LENGTH(key_type, alg) where
 *                                key_type is the type of key.
 * \param[in]  additional_data    Additional data that has been authenticated
 *                                but not encrypted.
 * \param      additional_data_length  Size of additional_data in bytes.
 * \param[in]  ciphertext         Data that has been authenticated and
 *                                encrypted. For algorithms where the encrypted
 *                                data and the authentication tag are defined
 *                                as separate inputs, the buffer contains
 *                                encrypted data followed by the authentication
 *                                tag.
 * \param      ciphertext_length  Size of ciphertext in bytes.
 * \param[out] plaintext          Output buffer for the decrypted data.
 * \param      plaintext_size     Size of the plaintext buffer in bytes. This
 *                                must be appropriate for the selected algorithm
 *                                and key:
 *                                - A sufficient output size is
 *                                  PSA_AEAD_DECRYPT_OUTPUT_SIZE(key_type, alg,
 *                                  ciphertext_length) where key_type is the
 *                                  type of key.
 *                                - PSA_AEAD_DECRYPT_OUTPUT_MAX_SIZE(
 *                                  ciphertext_length) evaluates to the maximum
 *                                  plaintext size of any supported AEAD
 *                                  decryption.
 * \param[out] plaintext_length   On success, the size of the output in the
 *                                plaintext buffer.
 *
 * \retval #PSA_SUCCESS Success.
 * \retval #PSA_ERROR_INVALID_SIGNATURE
 *         The cipher is not authentic.
 * \retval #PSA_ERROR_NOT_SUPPORTED
 *         \p alg is not supported.
 * \retval #PSA_ERROR_INSUFFICIENT_MEMORY
 * \retval #PSA_ERROR_BUFFER_TOO_SMALL
 *         plaintext_size is too small.
 * \retval #PSA_ERROR_CORRUPTION_DETECTED
 */
psa_status_t sli_si91x_crypto_aead_decrypt(const psa_key_attributes_t *attributes,
                                           const uint8_t *key_buffer,
                                           size_t key_buffer_size,
                                           psa_algorithm_t alg,
                                           const uint8_t *nonce,
                                           size_t nonce_length,
                                           const uint8_t *additional_data,
                                           size_t additional_data_length,
                                           const uint8_t *ciphertext,
                                           size_t ciphertext_length,
                                           uint8_t *plaintext,
                                           size_t plaintext_size,
                                           size_t *plaintext_length)
{
  psa_status_t status = PSA_ERROR_GENERIC_ERROR;

  //! Input check
  if ((attributes == NULL) || (key_buffer == NULL) || (key_buffer_size <= 0) || (nonce == NULL)
      || (additional_data == NULL && additional_data_length > 0) || (ciphertext == NULL && ciphertext_length > 0)
      || (plaintext == NULL && plaintext_size > 0) || (plaintext_length == NULL)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Verify that the driver supports the given parameters
  status = check_aead_parameters(attributes, alg, nonce_length, additional_data_length);
  if (status != PSA_SUCCESS) {
    return status;
  }
  *plaintext_length = 0;

  psa_key_type_t key_type = psa_get_key_type(attributes);
  size_t key_bits         = psa_get_key_bits(attributes);
  uint8_t tag_length      = PSA_AEAD_TAG_LENGTH(key_type, key_bits, alg);

  // Check sufficient output buffer size.
  if (plaintext_size < (ciphertext_length - tag_length)) {
    return PSA_ERROR_BUFFER_TOO_SMALL;
  }

  int32_t si91x_status;
  switch (PSA_ALG_AEAD_WITH_SHORTENED_TAG(alg, 0)) {
#if defined(PSA_WANT_ALG_CCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 0):;
      sl_si91x_ccm_config_t config_ccm = { 0 };
      config_ccm.encrypt_decrypt       = SL_SI91X_CCM_DECRYPT;
      config_ccm.msg                   = ciphertext;
      config_ccm.msg_length            = ciphertext_length - tag_length;
      config_ccm.nonce                 = nonce;
      config_ccm.nonce_length          = nonce_length;
      config_ccm.ad                    = additional_data;
      config_ccm.ad_length             = additional_data_length;
      config_ccm.tag                   = ciphertext + (ciphertext_length - tag_length);
      config_ccm.tag_length            = tag_length;

#ifdef CHIP_917B0
      /* Fetch key type from attributes */
      psa_key_location_t location_ccm = PSA_KEY_LIFETIME_GET_LOCATION(psa_get_key_lifetime(attributes));
      if (location_ccm == 0) {
        config_ccm.key_config.b0.key_type = SL_SI91X_TRANSPARENT_KEY;
      } else {
        config_ccm.key_config.b0.key_type = SL_SI91X_WRAPPED_KEY;
      }

      /* Set key_size from key_buffer_size */
      if (key_buffer_size == 16) {
        config_ccm.key_config.b0.key_size = SL_SI91X_CCM_KEY_SIZE_128;
      }
      if (key_buffer_size == 24) {
        config_ccm.key_config.b0.key_size = SL_SI91X_CCM_KEY_SIZE_192;
      }
      if (key_buffer_size == 32) {
        config_ccm.key_config.b0.key_size = SL_SI91X_CCM_KEY_SIZE_256;
      }

      config_ccm.key_config.b0.key_slot     = 0;
      config_ccm.key_config.b0.wrap_iv_mode = SL_SI91X_WRAP_IV_CBC_MODE;
      if (config_ccm.key_config.b0.wrap_iv_mode == SL_SI91X_WRAP_IV_CBC_MODE) {
        memcpy(config_ccm.key_config.b0.wrap_iv, WRAP_IV, SL_SI91X_IV_SIZE);
      }
      memcpy(config_ccm.key_config.b0.key_buffer, key_buffer, config_ccm.key_config.b0.key_size);
#else
      config_ccm.key_config.a0.key_length = key_buffer_size;
      memcpy(config_ccm.key_config.a0.key, key_buffer, config_ccm.key_config.a0.key_length);
#endif

      if (status != PSA_SUCCESS) {
        return status;
      }
      /* Calling sl_si91x_ccm() for CCM decryption */
      si91x_status = sl_si91x_ccm(&config_ccm, plaintext);

      status = convert_si91x_error_code_to_psa_status(si91x_status);
      break;
#endif /* PSA_WANT_ALG_CCM */
#if defined(PSA_WANT_ALG_GCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 0):;
      sl_si91x_gcm_config_t config_gcm = { 0 };
      config_gcm.encrypt_decrypt       = SL_SI91X_GCM_DECRYPT;
      config_gcm.dma_use               = SL_SI91X_DMA_ENABLE;
      config_gcm.msg                   = ciphertext;
      config_gcm.msg_length            = ciphertext_length - tag_length;
      config_gcm.nonce                 = nonce;
      config_gcm.nonce_length          = nonce_length;
      config_gcm.ad                    = additional_data;
      config_gcm.ad_length             = additional_data_length;

#ifdef CHIP_917B0
      /* Fetch key type from attributes */
      psa_key_location_t location_gcm = PSA_KEY_LIFETIME_GET_LOCATION(psa_get_key_lifetime(attributes));
      if (location_gcm == 0) {
        config_gcm.key_config.b0.key_type = SL_SI91X_TRANSPARENT_KEY;
      } else {
        config_gcm.key_config.b0.key_type = SL_SI91X_WRAPPED_KEY;
      }

      /* Set key_size from key_buffer_size */
      if (key_buffer_size == 16) {
        config_gcm.key_config.b0.key_size = SL_SI91X_GCM_KEY_SIZE_128;
      }
      if (key_buffer_size == 24) {
        config_gcm.key_config.b0.key_size = SL_SI91X_GCM_KEY_SIZE_192;
      }
      if (key_buffer_size == 32) {
        config_gcm.key_config.b0.key_size = SL_SI91X_GCM_KEY_SIZE_256;
      }

      config_gcm.gcm_mode                   = SL_SI91X_GCM_MODE;
      config_gcm.key_config.b0.key_slot     = 0;
      config_gcm.key_config.b0.wrap_iv_mode = SL_SI91X_WRAP_IV_CBC_MODE;
      if (config_gcm.key_config.b0.wrap_iv_mode == SL_SI91X_WRAP_IV_CBC_MODE) {
        memcpy(config_gcm.key_config.b0.wrap_iv, WRAP_IV, SL_SI91X_IV_SIZE);
      }
      memcpy(config_gcm.key_config.b0.key_buffer, key_buffer, config_gcm.key_config.b0.key_size);
#else
      config_gcm.key_config.a0.key_length = key_buffer_size;
      memcpy(config_gcm.key_config.a0.key, key_buffer, config_gcm.key_config.a0.key_length);
#endif

      if (status != PSA_SUCCESS) {
        return status;
      }
      /* Calling sl_si91x_gcm() for GCM decryption */
      si91x_status = sl_si91x_gcm(&config_gcm, plaintext);

      status = convert_si91x_error_code_to_psa_status(si91x_status);
      break;
#endif /* PSA_WANT_ALG_GCM */
#if defined(PSA_WANT_ALG_CHACHA20_POLY1305)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CHACHA20_POLY1305, 0):;
      sl_si91x_chachapoly_config_t config_chachapoly;
      config_chachapoly.encrypt_decrypt = SL_SI91X_CHACHAPOLY_DECRYPT;
      config_chachapoly.dma_use         = SL_SI91X_DMA_ENABLE;
      config_chachapoly.chachapoly_mode = SL_SI91X_CHACHA20POLY1305_MODE;
      config_chachapoly.msg             = ciphertext;
      config_chachapoly.msg_length      = ciphertext_length - tag_length;
      config_chachapoly.nonce           = nonce;
      config_chachapoly.ad              = additional_data;
      config_chachapoly.ad_length       = additional_data_length;

#ifdef CHIP_917B0
      /* Fetch key type from attributes */
      psa_key_location_t location_chachapoly = PSA_KEY_LIFETIME_GET_LOCATION(psa_get_key_lifetime(attributes));
      if (location_chachapoly == 0) {
        config_chachapoly.key_config.b0.key_type = SL_SI91X_TRANSPARENT_KEY;
      } else {
        config_chachapoly.key_config.b0.key_type = SL_SI91X_WRAPPED_KEY;
      }

      config_chachapoly.key_config.b0.key_size     = SL_SI91X_CHACHAPOLY_KEY_SIZE_256;
      config_chachapoly.key_config.b0.key_slot     = 0;
      config_chachapoly.key_config.b0.wrap_iv_mode = SL_SI91X_WRAP_IV_CBC_MODE;
      if (config_chachapoly.key_config.b0.wrap_iv_mode == SL_SI91X_WRAP_IV_CBC_MODE) {
        memcpy(config_chachapoly.key_config.b0.wrap_iv, WRAP_IV, SL_SI91X_IV_SIZE);
      }
      memcpy(config_chachapoly.key_config.b0.key_buffer, key_buffer, config_chachapoly.key_config.b0.key_size);
#else
      memcpy(config_chachapoly.key_config.a0.key_chacha, key_buffer, SL_SI91X_CHACHAPOLY_KEY_SIZE_256);
#endif

      if (status != PSA_SUCCESS) {
        return status;
      }
      /* Calling sl_si91x_chachapoly() for CHACHAPOLY decryption */
      si91x_status = sl_si91x_chachapoly(&config_chachapoly, plaintext);

      /* gets the si91x error codes and returns its equivalent psa_status codes */
      status = convert_si91x_error_code_to_psa_status(si91x_status);

      break;
#endif /* PSA_WANT_ALG_CHACHAPOLY */
    default:
      (void)status;
      (void)key_buffer;
      status = PSA_ERROR_NOT_SUPPORTED;
      break;
  }
  if (status == PSA_SUCCESS) {
    *plaintext_length = ciphertext_length - tag_length;
  }
  return status;
}
