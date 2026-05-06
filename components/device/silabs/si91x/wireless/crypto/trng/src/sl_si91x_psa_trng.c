/*******************************************************************************
 * @file  sl_si91x_psa_trng.c
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
#include "sli_si91x_crypto_driver_functions.h"
#include "sl_si91x_psa_trng.h"
#include "sl_si91x_constants.h"
#include "sl_status.h"
#include "sl_si91x_trng.h"
#include <stdio.h>
#include "rsi_driver.h"

/* TRNG key */
extern uint32_t trng_key[TRNG_KEY_SIZE];

psa_status_t sli_si91x_crypto_trng_init()
{
  psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
  sl_status_t sl_status;

  /* This API checks the Entropy of TRNG i.e source for TRNG */
  sl_status = sl_si91x_trng_entropy();
  /* Convert the error code from si91x to psa */
  status = convert_si91x_error_code_to_psa_status(sl_status);

  if (status != PSA_SUCCESS) {
    SL_PRINTF(SL_TRNG_EXIT, CRYPTO, LOG_INFO, "status: %4x", status);
    return status;
  }

  /* This API Initializes key which needs to be programmed to TRNG hardware engine */
  sl_status = sl_si91x_trng_program_key(trng_key, TRNG_KEY_SIZE);
  /* Convert the error code from si91x to psa */
  status = convert_si91x_error_code_to_psa_status(sl_status);

  if (status != PSA_SUCCESS) {
    SL_PRINTF(SL_TRNG_EXIT, CRYPTO, LOG_INFO, "status: %4x", status);
    return status;
  }

  return status;
}

psa_status_t sl_si91x_psa_get_random(uint8_t *output, size_t len, size_t *out_len)
{
  if (output == NULL || len == 0 || out_len == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }
  psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
  sl_status_t sl_status;

  //! Memset the buffer to zero
  memset(output, 0, len);
  //! Get Random number of desired length
  sl_status = sl_si91x_trng_get_random_num((uint32_t *)output, len);

  /* Convert the error code from si91x to psa */
  status = convert_si91x_error_code_to_psa_status(sl_status);

  if (status != PSA_SUCCESS) {
    SL_PRINTF(SL_TRNG_EXIT, CRYPTO, LOG_INFO, "status: %4x", status);
    return status;
  }

  *out_len = len;
  return status;
}
