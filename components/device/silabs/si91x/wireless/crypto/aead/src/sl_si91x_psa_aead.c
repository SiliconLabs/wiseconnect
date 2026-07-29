/*******************************************************************************
 * @file  sl_si91x_psa_aead.c
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include "sli_si91x_crypto_driver_functions.h"
#if defined(SLI_PSA_DRIVER_FEATURE_CCM)
#include "sl_si91x_ccm.h"
#if defined(SL_SI91X_SIDE_BAND_CRYPTO)
#include "sl_si91x_mp_ccm.h"
#endif
#endif
#if defined(SLI_PSA_DRIVER_FEATURE_GCM)
#include "sl_si91x_gcm.h"
#endif
#if defined(SLI_PSA_DRIVER_FEATURE_CHACHAPOLY)
#include "sl_si91x_chachapoly.h"
#endif
#if defined(SLI_SECURE_KEY_STORAGE_DEVICE_SI91X)
#include "sl_si91x_psa_wrap.h"
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
static psa_status_t sli_si91x_check_aead_parameters(const psa_key_attributes_t *attributes,
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
      if ((tag_length < SLI_SI91X_CCM_TAG_MIN_SIZE) || (tag_length > SLI_SI91X_AEAD_TAG_MAX_SIZE)
          || (tag_length % 2 != 0) || (nonce_length < SLI_SI91X_CCM_IV_MIN_SIZE)
          || (nonce_length > SLI_SI91X_CCM_IV_MAX_SIZE)) {
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
      if ((tag_length < 4) || (tag_length > 16) || nonce_length != SLI_SI91X_GCM_IV_SIZE) {
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
      if (tag_length != SL_SI91X_TAG_SIZE || nonce_length != SLI_SI91X_CHACHAPOLY_IV_SIZE) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
      break;
#endif // SLI_PSA_DRIVER_FEATURE_CHACHAPOLY
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

#if !defined(SLI_PSA_DRIVER_FEATURE_CCM) && !defined(SLI_PSA_DRIVER_FEATURE_GCM) \
  && !defined(SLI_PSA_DRIVER_FEATURE_CHACHAPOLY)
  UNUSED_PARAMETER(nonce_length);
  UNUSED_VARIABLE(tag_length);
#endif
  UNUSED_PARAMETER(additional_data_length);

  return PSA_SUCCESS;
}

#if defined(SLI_PSA_DRIVER_FEATURE_CCM)
static void sli_si91x_set_input_config_ccm(const psa_key_attributes_t *attributes,
                                           sl_si91x_ccm_config_t *config_ccm,
                                           const uint8_t *key_buffer,
                                           size_t key_buffer_size)
{
#if defined(SLI_SI917B0)
  /* Fetch key type from attributes */
  psa_key_location_t location_ccm = PSA_KEY_LIFETIME_GET_LOCATION(psa_get_key_lifetime(attributes));
  if (location_ccm == 0) {
    config_ccm->key_config.b0.key_type = SL_SI91X_TRANSPARENT_KEY;
  } else {
    config_ccm->key_config.b0.key_type = SL_SI91X_WRAPPED_KEY;
  }

  /* Set key_size from key_buffer_size */
  if (key_buffer_size == 16) {
    config_ccm->key_config.b0.key_size = SL_SI91X_CCM_KEY_SIZE_128;
  }
  if (key_buffer_size == 24) {
    config_ccm->key_config.b0.key_size = SL_SI91X_CCM_KEY_SIZE_192;
  }
  if (key_buffer_size == 32) {
    config_ccm->key_config.b0.key_size = SL_SI91X_CCM_KEY_SIZE_256;
  }

  config_ccm->key_config.b0.key_slot     = 0;
  config_ccm->key_config.b0.wrap_iv_mode = SL_SI91X_WRAP_IV_CBC_MODE;
  if (config_ccm->key_config.b0.wrap_iv_mode == SL_SI91X_WRAP_IV_CBC_MODE) {
    memcpy(config_ccm->key_config.b0.wrap_iv, WRAP_IV, SL_SI91X_IV_SIZE);
  }
  memcpy(config_ccm->key_config.b0.key_buffer, key_buffer, config_ccm->key_config.b0.key_size);
#else
  config_ccm->key_config.a0.key        = (uint8_t *)malloc(key_buffer_size);
  config_ccm->key_config.a0.key_length = key_buffer_size;
  memcpy(config_ccm->key_config.a0.key, key_buffer, config_ccm->key_config.a0.key_length);
#endif
}
#endif

#if defined(SLI_PSA_DRIVER_FEATURE_GCM)
static void sli_si91x_set_input_config_gcm(const psa_key_attributes_t *attributes,
                                           sl_si91x_gcm_config_t *config_gcm,
                                           const uint8_t *key_buffer,
                                           size_t key_buffer_size)
{
#if defined(SLI_SI917B0)
  /* Fetch key type from attributes */
  psa_key_location_t location_gcm = PSA_KEY_LIFETIME_GET_LOCATION(psa_get_key_lifetime(attributes));
  if (location_gcm == 0) {
    config_gcm->key_config.b0.key_type = SL_SI91X_TRANSPARENT_KEY;
  } else {
    config_gcm->key_config.b0.key_type = SL_SI91X_WRAPPED_KEY;
  }

  /* Set key_size from key_buffer_size */
  if (key_buffer_size == 16) {
    config_gcm->key_config.b0.key_size = SL_SI91X_GCM_KEY_SIZE_128;
  }
  if (key_buffer_size == 24) {
    config_gcm->key_config.b0.key_size = SL_SI91X_GCM_KEY_SIZE_192;
  }
  if (key_buffer_size == 32) {
    config_gcm->key_config.b0.key_size = SL_SI91X_GCM_KEY_SIZE_256;
  }

  config_gcm->gcm_mode                   = SL_SI91X_GCM_MODE;
  config_gcm->key_config.b0.key_slot     = 0;
  config_gcm->key_config.b0.wrap_iv_mode = SL_SI91X_WRAP_IV_CBC_MODE;
  if (config_gcm->key_config.b0.wrap_iv_mode == SL_SI91X_WRAP_IV_CBC_MODE) {
    memcpy(config_gcm->key_config.b0.wrap_iv, WRAP_IV, SL_SI91X_IV_SIZE);
  }
  memcpy(config_gcm->key_config.b0.key_buffer, key_buffer, config_gcm->key_config.b0.key_size);
#else
  config_gcm->key_config.a0.key        = (uint8_t *)malloc(key_buffer_size);
  config_gcm->key_config.a0.key_length = key_buffer_size;
  memcpy(config_gcm->key_config.a0.key, key_buffer, config_gcm->key_config.a0.key_length);
#endif
}
#endif

#if defined(SLI_PSA_DRIVER_FEATURE_CHACHAPOLY)
static void sli_si91x_set_input_config_chachapoly(const psa_key_attributes_t *attributes,
                                                  sl_si91x_chachapoly_config_t *config_chachapoly,
                                                  const uint8_t *key_buffer)
{
#if defined(SLI_SI917B0)
  /* Fetch key type from attributes */
  psa_key_location_t location_chachapoly = PSA_KEY_LIFETIME_GET_LOCATION(psa_get_key_lifetime(attributes));
  if (location_chachapoly == 0) {
    config_chachapoly->key_config.b0.key_type = SL_SI91X_TRANSPARENT_KEY;
  } else {
    config_chachapoly->key_config.b0.key_type = SL_SI91X_WRAPPED_KEY;
  }

  config_chachapoly->key_config.b0.key_size     = SL_SI91X_CHACHAPOLY_KEY_SIZE_256;
  config_chachapoly->key_config.b0.key_slot     = 0;
  config_chachapoly->key_config.b0.wrap_iv_mode = SL_SI91X_WRAP_IV_CBC_MODE;
  if (config_chachapoly->key_config.b0.wrap_iv_mode == SL_SI91X_WRAP_IV_CBC_MODE) {
    memcpy(config_chachapoly->key_config.b0.wrap_iv, WRAP_IV, SL_SI91X_IV_SIZE);
  }
  memcpy(config_chachapoly->key_config.b0.key_buffer, key_buffer, config_chachapoly->key_config.b0.key_size);
#else
  memcpy(config_chachapoly->key_config.a0.key_chacha, key_buffer, SL_SI91X_CHACHAPOLY_KEY_SIZE_256);
#endif
}
#endif

/*****************************************************************************
 * Encrypto a message using AEAD algorithms.
******************************************************************************/
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
  status = sli_si91x_check_aead_parameters(attributes, alg, nonce_length, additional_data_length);
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
#if defined(SLI_PSA_DRIVER_FEATURE_CCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 0): {
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

      sli_si91x_set_input_config_ccm(attributes, &config_ccm, key_buffer, key_buffer_size);

      /* Calling sl_si91x_ccm() for CCM encryption */
      si91x_status = sl_si91x_ccm(&config_ccm, ciphertext);

#if !defined(SLI_SI917B0)
      free(config_ccm.key_config.a0.key);
#endif
      /* gets the si91x error codes and returns its equivalent psa_status codes */
      status = convert_si91x_error_code_to_psa_status(si91x_status);

      break;
    }
#endif /* SLI_PSA_DRIVER_FEATURE_CCM */
#if defined(SLI_PSA_DRIVER_FEATURE_GCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 0): {
      sl_si91x_gcm_config_t config_gcm;
      config_gcm.encrypt_decrypt = SL_SI91X_GCM_ENCRYPT;
      config_gcm.dma_use         = SL_SI91X_GCM_DMA_ENABLE;
      config_gcm.msg             = plaintext;
      config_gcm.msg_length      = plaintext_length;
      config_gcm.nonce           = nonce;
      config_gcm.nonce_length    = nonce_length;
      config_gcm.ad              = additional_data;
      config_gcm.ad_length       = additional_data_length;

      sli_si91x_set_input_config_gcm(attributes, &config_gcm, key_buffer, key_buffer_size);

      /* Calling sl_si91x_gcm() for GCM encryption */
      si91x_status = sl_si91x_gcm(&config_gcm, ciphertext);

#if !defined(SLI_SI917B0)
      free(config_gcm.key_config.a0.key);
#endif
      /* gets the si91x error codes and returns its equivalent psa_status codes */
      status = convert_si91x_error_code_to_psa_status(si91x_status);

      break;
    }
#endif /* SLI_PSA_DRIVER_FEATURE_GCM */
#if defined(SLI_PSA_DRIVER_FEATURE_CHACHAPOLY)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CHACHA20_POLY1305, 0): {
      uint8_t temp_nonce[16] = { 0 };
      temp_nonce[0]          = 0x01;
      memcpy(temp_nonce + 4, nonce, 12);
      sl_si91x_chachapoly_config_t config_chachapoly;
      config_chachapoly.encrypt_decrypt = SL_SI91X_CHACHAPOLY_ENCRYPT;
      config_chachapoly.dma_use         = SL_SI91X_CHACHAPOLY_DMA_ENABLE;
      config_chachapoly.chachapoly_mode = SL_SI91X_CHACHA20POLY1305_MODE;
      config_chachapoly.msg             = plaintext;
      config_chachapoly.msg_length      = plaintext_length;
      config_chachapoly.nonce           = temp_nonce;
      config_chachapoly.ad              = additional_data;
      config_chachapoly.ad_length       = additional_data_length;

      sli_si91x_set_input_config_chachapoly(attributes, &config_chachapoly, key_buffer);

      /* Calling sl_si91x_chachapoly() for CHACHAPOLY encryption */
      si91x_status = sl_si91x_chachapoly(&config_chachapoly, ciphertext);

      /* gets the si91x error codes and returns its equivalent psa_status codes */
      status = convert_si91x_error_code_to_psa_status(si91x_status);

      break;
    }
#endif /* SLI_PSA_DRIVER_FEATURE_CHACHAPOLY */
    default:
      (void)status;
      (void)key_buffer;
#if !defined(SLI_PSA_DRIVER_FEATURE_CCM) && !defined(SLI_PSA_DRIVER_FEATURE_GCM) \
  && !defined(SLI_PSA_DRIVER_FEATURE_CHACHAPOLY)
      UNUSED_VARIABLE(si91x_status);
#endif
      status = PSA_ERROR_NOT_SUPPORTED;
      break;
  }
  if (status == PSA_SUCCESS) {
    *ciphertext_length = plaintext_length + tag_length;
  }
  return status;
}

/*****************************************************************************
 * Decrypt a message using AEAD algorithms.
******************************************************************************/
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
  status = sli_si91x_check_aead_parameters(attributes, alg, nonce_length, additional_data_length);
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
#if defined(SLI_PSA_DRIVER_FEATURE_CCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 0): {
      sl_si91x_ccm_config_t config_ccm = { 0 };
      config_ccm.encrypt_decrypt       = SL_SI91X_CCM_DECRYPT;
      config_ccm.msg                   = ciphertext;
      config_ccm.msg_length            = ciphertext_length - tag_length;
      config_ccm.nonce                 = nonce;
      config_ccm.nonce_length          = nonce_length;
      config_ccm.ad                    = additional_data;
      config_ccm.ad_length             = additional_data_length;
      config_ccm.tag                   = (uint8_t *)ciphertext + (ciphertext_length - tag_length);
      config_ccm.tag_length            = tag_length;

      sli_si91x_set_input_config_ccm(attributes, &config_ccm, key_buffer, key_buffer_size);

      /* Calling sl_si91x_ccm() for CCM decryption */
      si91x_status = sl_si91x_ccm(&config_ccm, plaintext);

#if !defined(SLI_SI917B0)
      free(config_ccm.key_config.a0.key);
#endif
      status = convert_si91x_error_code_to_psa_status(si91x_status);
      break;
    }
#endif /* SLI_PSA_DRIVER_FEATURE_CCM */
#if defined(SLI_PSA_DRIVER_FEATURE_GCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 0): {
      sl_si91x_gcm_config_t config_gcm = { 0 };
      config_gcm.encrypt_decrypt       = SL_SI91X_GCM_DECRYPT;
      config_gcm.dma_use               = SL_SI91X_GCM_DMA_ENABLE;
      config_gcm.msg                   = ciphertext;
      config_gcm.msg_length            = ciphertext_length - tag_length;
      config_gcm.nonce                 = nonce;
      config_gcm.nonce_length          = nonce_length;
      config_gcm.ad                    = additional_data;
      config_gcm.ad_length             = additional_data_length;

      sli_si91x_set_input_config_gcm(attributes, &config_gcm, key_buffer, key_buffer_size);

      /* Calling sl_si91x_gcm() for GCM decryption */
      si91x_status = sl_si91x_gcm(&config_gcm, plaintext);

#if !defined(SLI_SI917B0)
      free(config_gcm.key_config.a0.key);
#endif
      status = convert_si91x_error_code_to_psa_status(si91x_status);
      break;
    }
#endif /* SLI_PSA_DRIVER_FEATURE_GCM */
#if defined(SLI_PSA_DRIVER_FEATURE_CHACHAPOLY)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CHACHA20_POLY1305, 0): {
      uint8_t temp_nonce[16] = { 0 };
      temp_nonce[0]          = 0x01;
      memcpy(temp_nonce + 4, nonce, 12);
      sl_si91x_chachapoly_config_t config_chachapoly;
      config_chachapoly.encrypt_decrypt = SL_SI91X_CHACHAPOLY_DECRYPT;
      config_chachapoly.dma_use         = SL_SI91X_CHACHAPOLY_DMA_ENABLE;
      config_chachapoly.chachapoly_mode = SL_SI91X_CHACHA20POLY1305_MODE;
      config_chachapoly.msg             = ciphertext;
      config_chachapoly.msg_length      = ciphertext_length - tag_length;
      config_chachapoly.nonce           = temp_nonce;
      config_chachapoly.ad              = additional_data;
      config_chachapoly.ad_length       = additional_data_length;

      sli_si91x_set_input_config_chachapoly(attributes, &config_chachapoly, key_buffer);

      /* Calling sl_si91x_chachapoly() for CHACHAPOLY decryption */
      si91x_status = sl_si91x_chachapoly(&config_chachapoly, plaintext);

      /* gets the si91x error codes and returns its equivalent psa_status codes */
      status = convert_si91x_error_code_to_psa_status(si91x_status);

      break;
    }
#endif /* SLI_PSA_DRIVER_FEATURE_CHACHAPOLY */
    default:
      (void)status;
      (void)key_buffer;
#if !defined(SLI_PSA_DRIVER_FEATURE_CCM) && !defined(SLI_PSA_DRIVER_FEATURE_GCM) \
  && !defined(SLI_PSA_DRIVER_FEATURE_CHACHAPOLY)
      UNUSED_VARIABLE(si91x_status);
#endif
      status = PSA_ERROR_NOT_SUPPORTED;
      break;
  }
  if (status == PSA_SUCCESS) {
    *plaintext_length = ciphertext_length - tag_length;
  }
  return status;
}

#if defined(SLI_AEAD_DEVICE_SI91X) && defined(SLI_PSA_DRIVER_FEATURE_CCM)

#ifndef SL_SI91X_SIDE_BAND_CRYPTO
static inline sl_status_t sl_si91x_mp_ccm_init(sl_si91x_ccm_config_t *config)
{
  UNUSED_VARIABLE(config);
  return SL_STATUS_NOT_SUPPORTED;
}
static inline sl_status_t sl_si91x_mp_ccm_update(sl_si91x_ccm_config_t *config, uint16_t chunk_length, uint8_t *output)
{
  UNUSED_VARIABLE(config);
  UNUSED_VARIABLE(chunk_length);
  UNUSED_VARIABLE(output);
  return SL_STATUS_NOT_SUPPORTED;
}
static inline sl_status_t sl_si91x_mp_ccm_final(sl_si91x_ccm_config_t *config, uint16_t chunk_length, uint8_t *output)
{
  UNUSED_VARIABLE(config);
  UNUSED_VARIABLE(chunk_length);
  UNUSED_VARIABLE(output);
  return SL_STATUS_NOT_SUPPORTED;
}
#endif /* !SL_SI91X_SIDE_BAND_CRYPTO */

#ifdef SL_SI91X_SIDE_BAND_CRYPTO
static psa_status_t sli_si91x_validate_ccm_key_for_setup(const psa_key_attributes_t *attributes, psa_algorithm_t alg)
{
  psa_status_t status = PSA_SUCCESS;

  if (PSA_ALG_AEAD_WITH_SHORTENED_TAG(alg, 0) != PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 0)) {
    status = PSA_ERROR_NOT_SUPPORTED;
    return status;
  }
  if (psa_get_key_type(attributes) != PSA_KEY_TYPE_AES) {
    status = PSA_ERROR_NOT_SUPPORTED;
    return status;
  }
  switch (psa_get_key_bits(attributes)) {
    case 128:
    case 192:
    case 256:
      break;
    default:
      status = PSA_ERROR_INVALID_ARGUMENT;
      return status;
  }
  return status;
}
#endif /* SL_SI91X_SIDE_BAND_CRYPTO */

/*****************************************************************************
 * Common setup for multipart AEAD encrypt/decrypt operations.
 *
 * Validates key type, key size, and CCM tag-length constraints, then
 * initializes the operation context with key material and algorithm info.
 *
 * @param[in,out] ctx             Operation context to initialize.
 * @param[in]     attributes      Attributes of the key to use.
 * @param[in]     key_buffer      Buffer containing the key material.
 * @param         key_buffer_size Size of key_buffer in bytes.
 * @param         alg             The AEAD algorithm (e.g. PSA_ALG_CCM).
 * @param         is_encrypt      1 for encryption, 0 for decryption.
 *
 * @return PSA_SUCCESS on success, or an appropriate PSA error code.
******************************************************************************/
static psa_status_t sli_si91x_aead_common_setup(sli_si91x_crypto_aead_operation_t *ctx,
                                                const psa_key_attributes_t *attributes,
                                                const uint8_t *key_buffer,
                                                size_t key_buffer_size,
                                                psa_algorithm_t alg,
                                                uint8_t is_encrypt)
{
  psa_status_t status = PSA_ERROR_INVALID_ARGUMENT;

#ifndef SL_SI91X_SIDE_BAND_CRYPTO
  UNUSED_VARIABLE(ctx);
  UNUSED_VARIABLE(attributes);
  UNUSED_VARIABLE(key_buffer);
  UNUSED_VARIABLE(key_buffer_size);
  UNUSED_VARIABLE(alg);
  UNUSED_VARIABLE(is_encrypt);
  status = PSA_ERROR_NOT_SUPPORTED;
  return status;
#else

  if (ctx == NULL || attributes == NULL || key_buffer == NULL || key_buffer_size == 0) {
    status = PSA_ERROR_INVALID_ARGUMENT;
    return status;
  }

  status = sli_si91x_validate_ccm_key_for_setup(attributes, alg);
  if (status != PSA_SUCCESS) {
    return status;
  }

  if (key_buffer_size > SL_SI91X_KEY_BUFFER_SIZE) {
    status = PSA_ERROR_INVALID_ARGUMENT;
    return status;
  }

  psa_key_type_t key_type = psa_get_key_type(attributes);
  uint32_t key_bits       = (uint32_t)psa_get_key_bits(attributes);
  uint16_t tag_len        = (uint16_t)PSA_AEAD_TAG_LENGTH(key_type, key_bits, alg);

  /* CCM valid tag lengths are {4, 6, 8, 10, 12, 14, 16} per RFC 3610 §2.
   * Reject values outside this set to match the one-shot path validation
   * and prevent ctx->tag[] overflow from an oversized shortened-tag alg. */
  if (tag_len < SLI_SI91X_CCM_TAG_MIN_SIZE || tag_len > SLI_SI91X_AEAD_TAG_MAX_SIZE || tag_len % 2 != 0) {
    status = PSA_ERROR_INVALID_ARGUMENT;
    return status;
  }

  memset(ctx, 0, sizeof(*ctx));
  memcpy(ctx->key_buffer, key_buffer, key_buffer_size);
  ctx->key_buffer_size = key_buffer_size;
  ctx->alg             = alg;
  ctx->key_type        = key_type;
  ctx->key_lifetime    = psa_get_key_lifetime(attributes);
  ctx->key_bits        = key_bits;
  ctx->is_encrypt      = is_encrypt;
  ctx->tag_length      = tag_len;

  status = PSA_SUCCESS;
  return status;
#endif /* SL_SI91X_SIDE_BAND_CRYPTO */
}

/*****************************************************************************
 * Set up a multipart authenticated encryption operation.
 *
 * @param[in,out] ctx             Operation context to initialize.
 * @param[in]     attributes      Attributes of the key to use.
 * @param[in]     key_buffer      Buffer containing the key material.
 * @param         key_buffer_size Size of key_buffer in bytes.
 * @param         alg             The AEAD algorithm (e.g. PSA_ALG_CCM).
 *
 * @return PSA_SUCCESS on success, or an appropriate PSA error code.
******************************************************************************/
psa_status_t sli_si91x_psa_aead_encrypt_setup(sli_si91x_crypto_aead_operation_t *ctx,
                                              const psa_key_attributes_t *attributes,
                                              const uint8_t *key_buffer,
                                              size_t key_buffer_size,
                                              psa_algorithm_t alg)
{
  return sli_si91x_aead_common_setup(ctx, attributes, key_buffer, key_buffer_size, alg, 1U);
}

/*****************************************************************************
 * Set up a multipart authenticated decryption operation.
 *
 * @param[in,out] ctx             Operation context to initialize.
 * @param[in]     attributes      Attributes of the key to use.
 * @param[in]     key_buffer      Buffer containing the key material.
 * @param         key_buffer_size Size of key_buffer in bytes.
 * @param         alg             The AEAD algorithm (e.g. PSA_ALG_CCM).
 *
 * @return PSA_SUCCESS on success, or an appropriate PSA error code.
******************************************************************************/
psa_status_t sli_si91x_psa_aead_decrypt_setup(sli_si91x_crypto_aead_operation_t *ctx,
                                              const psa_key_attributes_t *attributes,
                                              const uint8_t *key_buffer,
                                              size_t key_buffer_size,
                                              psa_algorithm_t alg)
{
  return sli_si91x_aead_common_setup(ctx, attributes, key_buffer, key_buffer_size, alg, 0U);
}

/*****************************************************************************
 * Set the nonce (IV) for a multipart AEAD operation.
 *
 * @param[in,out] ctx          Operation context.
 * @param[in]     nonce        Buffer containing the nonce / IV.
 * @param         nonce_length Size of nonce in bytes. Must be within CCM
 *                             bounds (SLI_SI91X_CCM_IV_MIN_SIZE ..
 *                             SLI_SI91X_CCM_IV_MAX_SIZE, i.e. 7-13).
 *
 * @return PSA_SUCCESS on success, or PSA_ERROR_INVALID_ARGUMENT.
******************************************************************************/
psa_status_t sli_si91x_psa_aead_set_nonce(sli_si91x_crypto_aead_operation_t *ctx,
                                          const uint8_t *nonce,
                                          size_t nonce_length)
{
  psa_status_t status = PSA_SUCCESS;

  if (ctx == NULL || nonce == NULL || nonce_length < SLI_SI91X_CCM_IV_MIN_SIZE
      || nonce_length > SLI_SI91X_CCM_IV_MAX_SIZE) {
    status = PSA_ERROR_INVALID_ARGUMENT;
    return status;
  }

  memcpy(ctx->nonce, nonce, nonce_length);
  ctx->nonce_length = (uint16_t)nonce_length;
  return status;
}

/*****************************************************************************
 * Declare the lengths of the AD and plaintext for a multipart AEAD operation.
 *
 * @param[in,out] ctx              Operation context.
 * @param         ad_length        Length of the additional data in bytes.
 * @param         plaintext_length Length of the plaintext / ciphertext in bytes.
 *
 * @return PSA_SUCCESS on success, or PSA_ERROR_INVALID_ARGUMENT /
 *         PSA_ERROR_NOT_SUPPORTED if lengths exceed driver limits.
******************************************************************************/
psa_status_t sli_si91x_psa_aead_set_lengths(sli_si91x_crypto_aead_operation_t *ctx,
                                            size_t ad_length,
                                            size_t plaintext_length)
{
  psa_status_t status = PSA_SUCCESS;

  if (ctx == NULL) {
    status = PSA_ERROR_INVALID_ARGUMENT;
    return status;
  }
  if (ad_length > SLI_SI91X_AEAD_AD_MAX_SIZE) {
    status = PSA_ERROR_INVALID_ARGUMENT;
    return status;
  }
  if (plaintext_length > UINT16_MAX) {
    status = PSA_ERROR_NOT_SUPPORTED;
    return status;
  }

  ctx->ad_length        = (uint16_t)ad_length;
  ctx->total_msg_length = (uint16_t)plaintext_length;
  return status;
}

/*****************************************************************************
 * Feed additional data (AD) into a multipart AEAD operation.
 *
 * May be called multiple times; the AD bytes are accumulated internally.
 *
 * @param[in,out] ctx          Operation context.
 * @param[in]     input        Buffer containing the AD fragment.
 * @param         input_length Size of input in bytes.
 *
 * @return PSA_SUCCESS on success, or PSA_ERROR_INVALID_ARGUMENT if
 *         cumulative AD exceeds the declared ad_length.
******************************************************************************/
psa_status_t sli_si91x_psa_aead_update_ad(sli_si91x_crypto_aead_operation_t *ctx,
                                          const uint8_t *input,
                                          size_t input_length)
{
  psa_status_t status = PSA_SUCCESS;

  if (ctx == NULL) {
    status = PSA_ERROR_INVALID_ARGUMENT;
    return status;
  }
  if (input_length == 0) {
    return status;
  }
  if (input == NULL) {
    status = PSA_ERROR_INVALID_ARGUMENT;
    return status;
  }
  if ((size_t)ctx->ad_offset + input_length > ctx->ad_length) {
    status = PSA_ERROR_INVALID_ARGUMENT;
    return status;
  }

  memcpy(ctx->ad + ctx->ad_offset, input, input_length);
  ctx->ad_offset += (uint16_t)input_length;
  return status;
}

/*****************************************************************************
 * Initialize firmware CCM context on first data update.
 *
 * Builds a sl_si91x_ccm_config_t from the accumulated operation context
 * and sends the FIRST_CHUNK packet to firmware via sl_si91x_mp_ccm_init.
 * Subsequent calls are a no-op once fw_initialized is set.
 *
 * @param[in,out] ctx  Operation context (must have nonce, AD, key set).
 *
 * @return PSA_SUCCESS on success, or an appropriate PSA error code.
******************************************************************************/
static psa_status_t sli_si91x_aead_ensure_fw_init(sli_si91x_crypto_aead_operation_t *ctx)
{
  psa_status_t status = PSA_SUCCESS;

  if (ctx->fw_initialized) {
    return status;
  }

  /* All declared AD must have been supplied before the first data update. */
  if (ctx->ad_offset != ctx->ad_length) {
    status = PSA_ERROR_INVALID_ARGUMENT;
    return status;
  }

  sl_si91x_ccm_config_t *config = (sl_si91x_ccm_config_t *)malloc(sizeof(sl_si91x_ccm_config_t));
  if (config == NULL) {
    status = PSA_ERROR_INSUFFICIENT_MEMORY;
    return status;
  }
  memset(config, 0, sizeof(*config));

  config->encrypt_decrypt = ctx->is_encrypt ? SL_SI91X_CCM_ENCRYPT : SL_SI91X_CCM_DECRYPT;
  config->msg             = NULL;
  config->msg_length      = ctx->total_msg_length;
  config->nonce           = ctx->nonce;
  config->nonce_length    = ctx->nonce_length;
  config->ad              = (ctx->ad_offset > 0) ? ctx->ad : NULL;
  config->ad_length       = ctx->ad_offset;
  config->tag             = ctx->tag;
  config->tag_length      = ctx->tag_length;

#if defined(SLI_SI917B0)
  psa_key_location_t location        = PSA_KEY_LIFETIME_GET_LOCATION(ctx->key_lifetime);
  config->key_config.b0.key_type     = (location == 0) ? SL_SI91X_TRANSPARENT_KEY : SL_SI91X_WRAPPED_KEY;
  config->key_config.b0.key_size     = (sl_si91x_ccm_key_size_t)ctx->key_buffer_size;
  config->key_config.b0.key_slot     = 0;
  config->key_config.b0.wrap_iv_mode = SL_SI91X_WRAP_IV_CBC_MODE;
  memcpy(config->key_config.b0.wrap_iv, WRAP_IV, SL_SI91X_IV_SIZE);
  memcpy(config->key_config.b0.key_buffer, ctx->key_buffer, ctx->key_buffer_size);
#else
  config->key_config.a0.key        = ctx->key_buffer;
  config->key_config.a0.key_length = (uint16_t)ctx->key_buffer_size;
#endif

  sl_status_t sl_status = sl_si91x_mp_ccm_init(config);
  free(config);
  config = NULL;
  if (sl_status != SL_STATUS_OK) {
    status = convert_si91x_error_code_to_psa_status(sl_status);
    return status;
  }

  ctx->fw_initialized = 1U;
  return status;
}

/*****************************************************************************
 * Feed message data (plaintext or ciphertext) into a multipart AEAD operation.
 *
 * Each call produces output of the same length as the input.  Firmware
 * writes to an internal buffer; each chunk is copied back to the caller's
 * output buffer after the firmware call.  The internal buffer is freed
 * during finish(), verify(), or abort().
 *
 * @param[in,out] ctx           Operation context.
 * @param[in]     input         Buffer containing the message fragment.
 * @param         input_length  Size of input in bytes.
 * @param[out]    output        Output buffer for encrypted / decrypted data.
 * @param         output_size   Size of output in bytes (>= input_length).
 * @param[out]    output_length On success, number of bytes written to output.
 *
 * @return PSA_SUCCESS on success, or an appropriate PSA error code.
******************************************************************************/
psa_status_t sli_si91x_psa_aead_update(sli_si91x_crypto_aead_operation_t *ctx,
                                       const uint8_t *input,
                                       size_t input_length,
                                       uint8_t *output,
                                       size_t output_size,
                                       size_t *output_length)
{
  psa_status_t status = PSA_ERROR_INVALID_ARGUMENT;

  if (ctx == NULL || output_length == NULL) {
    status = PSA_ERROR_INVALID_ARGUMENT;
    return status;
  }
  *output_length = 0;

  /* Zero-length update is a true no-op: no state change, no firmware call.
   * Must be checked before fw_output_buf allocation so that a no-op call
   * does not trigger a buffer allocation or firmware interaction. */
  if (input_length == 0) {
    status = PSA_SUCCESS;
    return status;
  }

  /* Validate inputs before any irreversible state change (fw_init,
   * buffer allocation). */
  if (input_length > UINT16_MAX) {
    status = PSA_ERROR_NOT_SUPPORTED;
    return status;
  }
  if (input == NULL || output == NULL || output_size < input_length) {
    status = PSA_ERROR_INVALID_ARGUMENT;
    return status;
  }
  if (ctx->processed_length + (uint16_t)input_length > ctx->total_msg_length) {
    status = PSA_ERROR_INVALID_ARGUMENT;
    return status;
  }

  /* The firmware caches the first output pointer (output_base) and writes
   * all subsequent chunks at output_base + offset.  The PSA API is chunk-
   * oriented: callers may supply a different output buffer per update call
   * (e.g. a reused scratch buffer).  To decouple firmware expectations from
   * the caller's buffer management, both encrypt and decrypt go through an
   * internal buffer (fw_output_buf) sized for the full message.  Each
   * chunk's output is copied back to the caller's buffer after the firmware
   * call.  For encryption the buffer is oversized by tag_length so the
   * firmware's tag-append at total_msg_length stays in bounds. */
  if (ctx->fw_output_buf == NULL) {
    size_t buf_size = ctx->is_encrypt ? (size_t)ctx->total_msg_length + ctx->tag_length : (size_t)ctx->total_msg_length;
    ctx->fw_output_buf = (uint8_t *)malloc(buf_size);
    if (ctx->fw_output_buf == NULL) {
      status = PSA_ERROR_INSUFFICIENT_MEMORY;
      return status;
    }
  }

  status = sli_si91x_aead_ensure_fw_init(ctx);
  if (status != PSA_SUCCESS) {
    return status;
  }

  sl_si91x_ccm_config_t *config = (sl_si91x_ccm_config_t *)malloc(sizeof(sl_si91x_ccm_config_t));
  if (config == NULL) {
    status = PSA_ERROR_INSUFFICIENT_MEMORY;
    return status;
  }
  memset(config, 0, sizeof(*config));
  config->encrypt_decrypt = ctx->is_encrypt ? SL_SI91X_CCM_ENCRYPT : SL_SI91X_CCM_DECRYPT;
  config->msg             = input;
  config->msg_length      = ctx->total_msg_length;
  config->nonce_length    = ctx->nonce_length;
  config->ad_length       = ctx->ad_offset;
  config->tag_length      = ctx->tag_length;

  sl_status_t sl_status =
    sl_si91x_mp_ccm_update(config, (uint16_t)input_length, ctx->fw_output_buf + ctx->processed_length);
  free(config);
  config = NULL;
  if (sl_status != SL_STATUS_OK) {
    status = convert_si91x_error_code_to_psa_status(sl_status);
    return status;
  }

  memcpy(output, ctx->fw_output_buf + ctx->processed_length, input_length);

  ctx->processed_length += (uint16_t)input_length;
  *output_length = input_length;
  status         = PSA_SUCCESS;
  return status;
}

/*****************************************************************************
 * Finish a multipart authenticated encryption operation.
 *
 * Finalizes the encryption and returns the authentication tag.  For
 * AD-only operations (no update calls), ciphertext may be NULL.
 *
 * @param[in,out] ctx               Operation context.
 * @param[out]    ciphertext        Buffer for any remaining ciphertext.
 * @param         ciphertext_size   Size of ciphertext buffer in bytes.
 * @param[out]    ciphertext_length On success, remaining ciphertext bytes.
 * @param[out]    tag               Buffer to receive the authentication tag.
 * @param         tag_size          Size of tag buffer in bytes.
 * @param[out]    tag_length        On success, tag length in bytes.
 *
 * @return PSA_SUCCESS on success, or an appropriate PSA error code.
******************************************************************************/
psa_status_t sli_si91x_psa_aead_finish(sli_si91x_crypto_aead_operation_t *ctx,
                                       uint8_t *ciphertext,
                                       size_t ciphertext_size,
                                       size_t *ciphertext_length,
                                       uint8_t *tag,
                                       size_t tag_size,
                                       size_t *tag_length)
{
  psa_status_t status = PSA_ERROR_INVALID_ARGUMENT;

  UNUSED_VARIABLE(ciphertext);
  UNUSED_VARIABLE(ciphertext_size);

  if (ctx == NULL || tag == NULL || tag_length == NULL || ciphertext_length == NULL) {
    status = PSA_ERROR_INVALID_ARGUMENT;
    return status;
  }
  *ciphertext_length = 0;
  *tag_length        = 0;

  status = sli_si91x_aead_ensure_fw_init(ctx);
  if (status != PSA_SUCCESS) {
    return status;
  }

  /* All declared plaintext must have been supplied via update() before
   * finalization; otherwise the firmware state is incomplete and the
   * fw_output_buf allocation below may be undersized. */
  if (ctx->processed_length != ctx->total_msg_length) {
    status = PSA_ERROR_INVALID_ARGUMENT;
    return status;
  }

  if (tag_size < ctx->tag_length) {
    status = PSA_ERROR_BUFFER_TOO_SMALL;
    return status;
  }

  /* The encrypt path uses an internal buffer (fw_output_buf) so the
   * firmware's tag-append at output_base + total_msg_length stays in bounds.
   * For AD-only (total_msg_length == 0, no update calls), allocate just
   * tag_length bytes — the tag is written at offset 0. */
  if (ctx->fw_output_buf == NULL) {
    ctx->fw_output_buf = (uint8_t *)malloc(ctx->tag_length);
    if (ctx->fw_output_buf == NULL) {
      status = PSA_ERROR_INSUFFICIENT_MEMORY;
      return status;
    }
  }

  sl_si91x_ccm_config_t *config = (sl_si91x_ccm_config_t *)malloc(sizeof(sl_si91x_ccm_config_t));
  if (config == NULL) {
    status = PSA_ERROR_INSUFFICIENT_MEMORY;
    return status;
  }
  memset(config, 0, sizeof(*config));
  config->encrypt_decrypt = SL_SI91X_CCM_ENCRYPT;
  config->msg             = NULL;
  config->msg_length      = ctx->total_msg_length;
  config->nonce_length    = ctx->nonce_length;
  config->ad_length       = ctx->ad_offset;
  config->tag             = ctx->tag;
  config->tag_length      = ctx->tag_length;

  /* LAST_CHUNK with chunk_length=0: firmware finalizes and writes the
   * computed tag at its internal output_base + total_msg_length.  Pass the
   * internal buffer so sl_sb_ccm_set_output_base captures a safe address
   * (for AD-only this is the first time output_base is set). */
  sl_status_t sl_status = sl_si91x_mp_ccm_final(config, 0, ctx->fw_output_buf + ctx->processed_length);
  free(config);
  config = NULL;
  if (sl_status != SL_STATUS_OK) {
    status = convert_si91x_error_code_to_psa_status(sl_status);
    return status;
  }

  /* Tag is safely within fw_output_buf at offset total_msg_length. */
  memcpy(tag, ctx->fw_output_buf + ctx->total_msg_length, ctx->tag_length);
  *tag_length = ctx->tag_length;

  free(ctx->fw_output_buf);
  ctx->fw_output_buf = NULL;
  status             = PSA_SUCCESS;
  return status;
}

/*****************************************************************************
 * Finish a multipart authenticated decryption and verify the tag.
 *
 * Finalizes decryption and verifies the authentication tag against the
 * expected value.  For AD-only operations (no update calls), plaintext
 * may be NULL.
 *
 * @param[in,out] ctx              Operation context.
 * @param[out]    plaintext        Buffer for any remaining plaintext.
 * @param         plaintext_size   Size of plaintext buffer in bytes.
 * @param[out]    plaintext_length On success, remaining plaintext bytes.
 * @param[in]     tag              Buffer containing the expected tag.
 * @param         tag_length       Size of tag in bytes.  Must equal the tag
 *                                 length established during setup.
 *
 * @return PSA_SUCCESS on success, PSA_ERROR_INVALID_SIGNATURE on tag
 *         mismatch, or an appropriate PSA error code.
******************************************************************************/
psa_status_t sli_si91x_psa_aead_verify(sli_si91x_crypto_aead_operation_t *ctx,
                                       uint8_t *plaintext,
                                       size_t plaintext_size,
                                       size_t *plaintext_length,
                                       const uint8_t *tag,
                                       size_t tag_length)
{
  psa_status_t status = PSA_ERROR_INVALID_ARGUMENT;

  UNUSED_VARIABLE(plaintext_size);

  if (ctx == NULL || plaintext_length == NULL) {
    status = PSA_ERROR_INVALID_ARGUMENT;
    return status;
  }
  *plaintext_length = 0;

  status = sli_si91x_aead_ensure_fw_init(ctx);
  if (status != PSA_SUCCESS) {
    return status;
  }

  if (tag == NULL || tag_length != ctx->tag_length) {
    status = PSA_ERROR_INVALID_ARGUMENT;
    return status;
  }

  /* All declared ciphertext must have been supplied via update() before
   * verification; otherwise the firmware state is incomplete. */
  if (ctx->processed_length != ctx->total_msg_length) {
    status = PSA_ERROR_INVALID_ARGUMENT;
    return status;
  }

  /* Copy the expected tag into the context so that LAST_CHUNK can pass it
   * to firmware for comparison via sl_sb_ccm_set_expected_tag. */
  memcpy(ctx->tag, tag, tag_length);

  sl_si91x_ccm_config_t *config = (sl_si91x_ccm_config_t *)malloc(sizeof(sl_si91x_ccm_config_t));
  if (config == NULL) {
    status = PSA_ERROR_INSUFFICIENT_MEMORY;
    return status;
  }
  memset(config, 0, sizeof(*config));
  config->encrypt_decrypt = SL_SI91X_CCM_DECRYPT;
  config->msg             = NULL;
  config->msg_length      = ctx->total_msg_length;
  config->nonce_length    = ctx->nonce_length;
  config->ad_length       = ctx->ad_offset;
  config->tag             = ctx->tag;
  config->tag_length      = ctx->tag_length;

  /* LAST_CHUNK for decrypt: firmware verifies the computed tag against the
   * expected tag.  Pass fw_output_buf (already set from update) so the
   * firmware's cached output_base remains valid; for AD-only (no update
   * was called), fw_output_buf is NULL and plaintext may also be NULL. */
  uint8_t *fw_out       = (ctx->fw_output_buf != NULL) ? ctx->fw_output_buf : plaintext;
  sl_status_t sl_status = sl_si91x_mp_ccm_final(config, 0, fw_out);
  free(config);
  config = NULL;

  if (ctx->fw_output_buf != NULL) {
    free(ctx->fw_output_buf);
    ctx->fw_output_buf = NULL;
  }

  if (sl_status != SL_STATUS_OK) {
    status = convert_si91x_error_code_to_psa_status(sl_status);
    return status;
  }

  status = PSA_SUCCESS;
  return status;
}

/*****************************************************************************
 * Abort a multipart AEAD operation and release all resources.
 *
 * Resets the firmware state (if initialized) and zeroes the context.
 * Safe to call on an already-aborted or zero-initialized context.
 *
 * @param[in,out] ctx  Operation context to abort.
 *
 * @return PSA_SUCCESS on success, or PSA_ERROR_INSUFFICIENT_MEMORY if the
 *         firmware teardown allocation failed (local state is still cleaned).
******************************************************************************/
psa_status_t sli_si91x_psa_aead_abort(sli_si91x_crypto_aead_operation_t *ctx)
{
  psa_status_t status = PSA_SUCCESS;

  if (ctx != NULL) {
#ifdef SL_SI91X_SIDE_BAND_CRYPTO
    if (ctx->fw_initialized) {
      sl_si91x_ccm_config_t *config = (sl_si91x_ccm_config_t *)malloc(sizeof(sl_si91x_ccm_config_t));
      if (config != NULL) {
        memset(config, 0, sizeof(*config));
        config->encrypt_decrypt = ctx->is_encrypt ? SL_SI91X_CCM_ENCRYPT : SL_SI91X_CCM_DECRYPT;
        config->msg_length      = ctx->total_msg_length;
        config->nonce_length    = ctx->nonce_length;
        config->ad_length       = ctx->ad_offset;
        config->tag_length      = ctx->tag_length;
        (void)sl_si91x_mp_ccm_final(config, 0, NULL);
        free(config);
        config = NULL;
      } else {
        /* Firmware CCM context could not be torn down; signal the failure
         * but still clean up all host-side state below. */
        status = PSA_ERROR_INSUFFICIENT_MEMORY;
      }
    }
    if (ctx->fw_output_buf != NULL) {
      free(ctx->fw_output_buf);
      ctx->fw_output_buf = NULL;
    }
#endif
    memset(ctx, 0, sizeof(*ctx));
  }
  return status;
}
#elif defined(SLI_AEAD_DEVICE_SI91X)
/* CCM multipart not available in this build — provide stubs so the
 * auto-generated PSA driver wrapper (guarded by SLI_AEAD_DEVICE_SI91X alone)
 * compiles and receives PSA_ERROR_NOT_SUPPORTED, triggering the software
 * fallback path. */

psa_status_t sli_si91x_psa_aead_encrypt_setup(sli_si91x_crypto_aead_operation_t *ctx,
                                              const psa_key_attributes_t *attributes,
                                              const uint8_t *key_buffer,
                                              size_t key_buffer_size,
                                              psa_algorithm_t alg)
{
  UNUSED_VARIABLE(ctx);
  UNUSED_VARIABLE(attributes);
  UNUSED_VARIABLE(key_buffer);
  UNUSED_VARIABLE(key_buffer_size);
  UNUSED_VARIABLE(alg);
  return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t sli_si91x_psa_aead_decrypt_setup(sli_si91x_crypto_aead_operation_t *ctx,
                                              const psa_key_attributes_t *attributes,
                                              const uint8_t *key_buffer,
                                              size_t key_buffer_size,
                                              psa_algorithm_t alg)
{
  UNUSED_VARIABLE(ctx);
  UNUSED_VARIABLE(attributes);
  UNUSED_VARIABLE(key_buffer);
  UNUSED_VARIABLE(key_buffer_size);
  UNUSED_VARIABLE(alg);
  return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t sli_si91x_psa_aead_set_nonce(sli_si91x_crypto_aead_operation_t *ctx,
                                          const uint8_t *nonce,
                                          size_t nonce_length)
{
  UNUSED_VARIABLE(ctx);
  UNUSED_VARIABLE(nonce);
  UNUSED_VARIABLE(nonce_length);
  return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t sli_si91x_psa_aead_set_lengths(sli_si91x_crypto_aead_operation_t *ctx,
                                            size_t ad_length,
                                            size_t plaintext_length)
{
  UNUSED_VARIABLE(ctx);
  UNUSED_VARIABLE(ad_length);
  UNUSED_VARIABLE(plaintext_length);
  return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t sli_si91x_psa_aead_update_ad(sli_si91x_crypto_aead_operation_t *ctx,
                                          const uint8_t *input,
                                          size_t input_length)
{
  UNUSED_VARIABLE(ctx);
  UNUSED_VARIABLE(input);
  UNUSED_VARIABLE(input_length);
  return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t sli_si91x_psa_aead_update(sli_si91x_crypto_aead_operation_t *ctx,
                                       const uint8_t *input,
                                       size_t input_length,
                                       uint8_t *output,
                                       size_t output_size,
                                       size_t *output_length)
{
  UNUSED_VARIABLE(ctx);
  UNUSED_VARIABLE(input);
  UNUSED_VARIABLE(input_length);
  UNUSED_VARIABLE(output);
  UNUSED_VARIABLE(output_size);
  UNUSED_VARIABLE(output_length);
  return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t sli_si91x_psa_aead_finish(sli_si91x_crypto_aead_operation_t *ctx,
                                       uint8_t *ciphertext,
                                       size_t ciphertext_size,
                                       size_t *ciphertext_length,
                                       uint8_t *tag,
                                       size_t tag_size,
                                       size_t *tag_length)
{
  UNUSED_VARIABLE(ctx);
  UNUSED_VARIABLE(ciphertext);
  UNUSED_VARIABLE(ciphertext_size);
  UNUSED_VARIABLE(ciphertext_length);
  UNUSED_VARIABLE(tag);
  UNUSED_VARIABLE(tag_size);
  UNUSED_VARIABLE(tag_length);
  return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t sli_si91x_psa_aead_verify(sli_si91x_crypto_aead_operation_t *ctx,
                                       uint8_t *plaintext,
                                       size_t plaintext_size,
                                       size_t *plaintext_length,
                                       const uint8_t *tag,
                                       size_t tag_length)
{
  UNUSED_VARIABLE(ctx);
  UNUSED_VARIABLE(plaintext);
  UNUSED_VARIABLE(plaintext_size);
  UNUSED_VARIABLE(plaintext_length);
  UNUSED_VARIABLE(tag);
  UNUSED_VARIABLE(tag_length);
  return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t sli_si91x_psa_aead_abort(sli_si91x_crypto_aead_operation_t *ctx)
{
  UNUSED_VARIABLE(ctx);
  return PSA_ERROR_NOT_SUPPORTED;
}

#endif /* SLI_AEAD_DEVICE_SI91X */
