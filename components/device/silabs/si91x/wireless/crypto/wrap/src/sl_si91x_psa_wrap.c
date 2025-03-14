/*******************************************************************************
 * @file  sl_si91x_psa_wrap.c
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
#include "sl_si91x_wrap.h"
#include "sli_si91x_crypto_driver_functions.h"
#include "sl_si91x_crypto.h"
#include "sl_status.h"
#include "sl_constants.h"
#include "sl_si91x_protocol_types.h"
#include "sl_si91x_driver.h"
#include "sl_si91x_trng.h"
#include <string.h>

unsigned char WRAP_IV[] = { 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                            0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46 };

static psa_status_t sli_si91x_crypto_get_wrap_config_key_info(uint8_t *key_buffer,
                                                              size_t key_buffer_size,
                                                              uint32_t wrap_mode,
                                                              uint8_t *wrap_iv_var,
                                                              sl_si91x_wrap_config_t *wrap_config)
{
  /* Input pointer check */
  if ((key_buffer == NULL) || (key_buffer_size == 0) || (wrap_iv_var == NULL) || (wrap_config == NULL)
      || ((wrap_mode != SL_SI91X_WRAP_IV_ECB_MODE) && (wrap_mode != SL_SI91X_WRAP_IV_CBC_MODE))) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  /* Fill the key info */
  /* We are passing plain key to get wrap key */
  wrap_config->key_type     = SL_SI91X_TRANSPARENT_KEY;
  wrap_config->key_size     = key_buffer_size;
  wrap_config->wrap_iv_mode = wrap_mode;
  if (wrap_mode == SL_SI91X_WRAP_IV_CBC_MODE) {
    memcpy(wrap_config->wrap_iv, wrap_iv_var, SL_SI91X_IV_SIZE);
  }

  return PSA_SUCCESS;
}

psa_status_t sli_si91x_crypto_wrap_key(uint8_t *key_buffer,
                                       size_t key_buffer_size,
                                       uint32_t wrap_mode,
                                       uint8_t *wrap_iv_var)
{

  psa_status_t status                = PSA_SUCCESS;
  sl_status_t si91x_status           = SL_STATUS_OK;
  sl_si91x_wrap_config_t wrap_config = { 0 };

  status = sli_si91x_crypto_get_wrap_config_key_info(key_buffer, key_buffer_size, wrap_mode, wrap_iv_var, &wrap_config);

  if (status != PSA_SUCCESS) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }
  memcpy(wrap_config.key_buffer, key_buffer, wrap_config.key_size);

  status = sl_si91x_wrap(&wrap_config, key_buffer);

  status = convert_si91x_error_code_to_psa_status(si91x_status);
  if (status == PSA_SUCCESS) {
    printf("\r\nWrap success\r\n");
  } else {
    printf("\r\nWrap failed, Error Code : %ld \r\n", status);
  }

  return status;
}

psa_status_t sli_si91x_crypto_generate_symm_key(uint8_t *key_buffer,
                                                size_t key_buffer_size,
                                                uint32_t wrap_mode,
                                                uint8_t *wrap_iv_var)
{
  psa_status_t status                = PSA_SUCCESS;
  sl_status_t si91x_status           = SL_STATUS_OK;
  sl_si91x_wrap_config_t wrap_config = { 0 };

  status = sli_si91x_crypto_get_wrap_config_key_info(key_buffer, key_buffer_size, wrap_mode, wrap_iv_var, &wrap_config);

  if (status != PSA_SUCCESS) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }
  /* Get Random dwords of desired length */
  si91x_status = sl_si91x_trng_get_random_num((uint32_t *)wrap_config.key_buffer, (wrap_config.key_size));
  status       = convert_si91x_error_code_to_psa_status(si91x_status);

  if (status == PSA_SUCCESS) {
    status = sl_si91x_wrap(&wrap_config, key_buffer);
  } else {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  status = convert_si91x_error_code_to_psa_status(si91x_status);
  return status;
}
