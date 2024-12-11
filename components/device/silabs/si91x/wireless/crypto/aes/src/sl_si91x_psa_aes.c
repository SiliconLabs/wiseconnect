/*******************************************************************************
 * @file  sl_si91x_psa_aes.c
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
#include "sl_si91x_aes.h"
#include "sl_si91x_crypto.h"
#include "sl_status.h"
#include "sl_constants.h"
#include "sl_si91x_protocol_types.h"
#include "sl_si91x_driver.h"
#include <string.h>

static void sli_si91x_set_input_config(const psa_key_attributes_t *attributes,
                                       sl_si91x_aes_config_t *config,
                                       const uint8_t *key_buffer,
                                       size_t key_buffer_size)
{
#if defined(SLI_SI917B0) || defined(SLI_SI915)
  /* Fetch key type from attributes */
  psa_key_location_t location = PSA_KEY_LIFETIME_GET_LOCATION(psa_get_key_lifetime(attributes));
  if (location == 0)
    config->key_config.b0.key_type = SL_SI91X_TRANSPARENT_KEY;
  else
    config->key_config.b0.key_type = SL_SI91X_WRAPPED_KEY;

  /* Set key_size from key_buffer_size */
  if (key_buffer_size == 16)
    config->key_config.b0.key_size = SL_SI91X_AES_KEY_SIZE_128;
  if (key_buffer_size == 24)
    config->key_config.b0.key_size = SL_SI91X_AES_KEY_SIZE_192;
  if (key_buffer_size == 32)
    config->key_config.b0.key_size = SL_SI91X_AES_KEY_SIZE_256;

  config->key_config.b0.key_slot = 0;
  memcpy(config->key_config.b0.key_buffer, key_buffer, config->key_config.b0.key_size);
#else
  config->key_config.a0.key        = (uint8_t *)malloc(key_buffer_size);
  config->key_config.a0.key_length = key_buffer_size;
  memcpy(config->key_config.a0.key, key_buffer, config->key_config.a0.key_length);
#endif
}

/*****************************************************************************
* Encrypt a message using a AES cipher.
*****************************************************************************/
psa_status_t sli_si91x_crypto_cipher_encrypt(const psa_key_attributes_t *attributes,
                                             const uint8_t *key_buffer,
                                             size_t key_buffer_size,
                                             psa_algorithm_t alg,
                                             const uint8_t *iv,
                                             size_t iv_length,
                                             const uint8_t *input,
                                             size_t input_length,
                                             uint8_t *output,
                                             size_t output_size,
                                             size_t *output_length)
{
  (void)iv_length;
  (void)output_size;
  /* Local variables */
  psa_status_t status          = PSA_SUCCESS;
  sl_status_t si91x_status     = SL_STATUS_OK;
  sl_si91x_aes_config_t config = { 0 };
  uint16_t aes_mode;
  const uint8_t *aes_iv = NULL;

  //! Input check
  if ((attributes == NULL) || (key_buffer == NULL) || (input == NULL) || (output == NULL) || (output_length == NULL)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  if (((alg == PSA_ALG_CTR) || (alg == PSA_ALG_CBC_NO_PADDING)) && (iv == NULL)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  switch (alg) {
    case (PSA_ALG_ECB_NO_PADDING):
      /* Setting mode as ECB_MODE */
      aes_mode = SL_SI91X_AES_ECB;
#if defined(SLI_SI917B0) || defined(SLI_SI915)
      /* Setting wrap iv mode as WRAP_IV_ECB_MODE */
      config.key_config.b0.wrap_iv_mode = SL_SI91X_WRAP_IV_ECB_MODE;
#endif
      break;
    case (PSA_ALG_CBC_NO_PADDING):
      /* Setting mode as CBC_MODE */
      aes_mode = SL_SI91X_AES_CBC;
      /* setting aes_iv with iv */
      aes_iv = iv;
#if defined(SLI_SI917B0) || defined(SLI_SI915)
      /* Setting wrap iv mode as WRAP_IV_CBC_MODE */
      config.key_config.b0.wrap_iv_mode = SL_SI91X_WRAP_IV_CBC_MODE;
#endif
      break;
    case (PSA_ALG_CTR):
      /* Setting mode as CTR_MODE */
      aes_mode = SL_SI91X_AES_CTR;
      /* setting aes_iv with iv */
      aes_iv = iv;
      break;
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  config.aes_mode        = aes_mode;
  config.encrypt_decrypt = SL_SI91X_AES_ENCRYPT;
  config.msg             = input;
  config.msg_length      = input_length;
  config.iv              = aes_iv;

  sli_si91x_set_input_config(attributes, &config, key_buffer, key_buffer_size);

  /* Calling sl_si91x_aes() for AES encryption */
  si91x_status = sl_si91x_aes(&config, output);

#if !defined(SLI_SI917B0) && !defined(SLI_SI915)
  free(config.key_config.a0.key);
#endif

  /* gets the si91x error codes and returns its equivalent psa_status codes */
  status = convert_si91x_error_code_to_psa_status(si91x_status);

  if (status == PSA_SUCCESS) {
    *output_length = input_length;
  }

  return status;
}

/*****************************************************************************
 * Decrypt a message using a AES cipher.
******************************************************************************/
psa_status_t sli_si91x_crypto_cipher_decrypt(const psa_key_attributes_t *attributes,
                                             const uint8_t *key_buffer,
                                             size_t key_buffer_size,
                                             psa_algorithm_t alg,
                                             const uint8_t *input,
                                             size_t input_length,
                                             uint8_t *output,
                                             size_t output_size,
                                             size_t *output_length)
{
  (void)output_size;
  /* Local variables */
  psa_status_t status          = PSA_SUCCESS;
  sl_status_t si91x_status     = SL_STATUS_OK;
  sl_si91x_aes_config_t config = { 0 };
  uint16_t aes_mode;
  const uint8_t *aes_iv = NULL;

  //! Input pointer check
  if ((attributes == NULL) || (key_buffer == NULL) || (input == NULL) || (output == NULL) || (output_length == NULL)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  switch (alg) {
    case (PSA_ALG_ECB_NO_PADDING):
      /* Setting mode as ECB_MODE */
      aes_mode = SL_SI91X_AES_ECB;
#if defined(SLI_SI917B0) || defined(SLI_SI915)
      /* Setting wrap iv mode as WRAP_IV_ECB_MODE */
      config.key_config.b0.wrap_iv_mode = SL_SI91X_WRAP_IV_ECB_MODE;
#endif
      break;
    case (PSA_ALG_CBC_NO_PADDING):
      /* Setting mode as CBC_MODE */
      aes_mode = SL_SI91X_AES_CBC;
      aes_iv   = input;
      input += SL_SI91X_IV_SIZE;
      input_length -= SL_SI91X_IV_SIZE;
#if defined(SLI_SI917B0) || defined(SLI_SI915)
      /* Setting wrap iv mode as WRAP_IV_CBC_MODE */
      config.key_config.b0.wrap_iv_mode = SL_SI91X_WRAP_IV_CBC_MODE;
#endif
      break;
    case (PSA_ALG_CTR):
      /* Setting mode as CTR_MODE */
      aes_mode = SL_SI91X_AES_CTR;
      aes_iv   = input;
      input += SL_SI91X_IV_SIZE;
      input_length -= SL_SI91X_IV_SIZE;
      break;
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  config.aes_mode        = aes_mode;
  config.encrypt_decrypt = SL_SI91X_AES_DECRYPT;
  config.msg             = input;
  config.msg_length      = input_length;
  config.iv              = aes_iv;

  sli_si91x_set_input_config(attributes, &config, key_buffer, key_buffer_size);

  /* Calling sl_si91x_aes() for AES decryption */
  si91x_status = sl_si91x_aes(&config, output);

#if !defined(SLI_SI917B0) && !defined(SLI_SI915)
  free(config.key_config.a0.key);
#endif

  /* gets the si91x error codes and returns its equivalent psa_status codes */
  status = convert_si91x_error_code_to_psa_status(si91x_status);

  if (status == PSA_SUCCESS) {
    *output_length = input_length;
  }

  return status;
}
