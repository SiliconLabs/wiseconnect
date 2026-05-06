/*******************************************************************************
 * @file  sl_si91x_psa_sha.c
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
#include "sl_si91x_sha.h"
#include "sl_si91x_psa_sha.h"
#include "sli_si91x_crypto_driver_functions.h"

psa_status_t sli_si91x_crypto_hash_compute(psa_algorithm_t alg,
                                           const uint8_t *input,
                                           size_t input_length,
                                           uint8_t *hash,
                                           size_t hash_size,
                                           size_t *hash_length)
{
  psa_status_t status = PSA_ERROR_GENERIC_ERROR;

  uint8_t sha_algo;

#if defined(PSA_WANT_ALG_SHA_1) || defined(PSA_WANT_ALG_SHA_224) || defined(PSA_WANT_ALG_SHA_256) \
  || defined(PSA_WANT_ALG_SHA_384) || defined(PSA_WANT_ALG_SHA_512)

  if (((input == NULL) && (input_length > 0)) || ((hash == NULL) && (hash_size > 0))
      || ((hash_length == NULL) && (hash_size > 0))) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  switch (alg) {
#if defined(PSA_WANT_ALG_SHA_1)
    case PSA_ALG_SHA_1:
      sha_algo     = SL_SI91X_SHA_1;
      *hash_length = SL_SI91X_SHA_1_DIGEST_LEN;
      break;
#endif // PSA_WANT_ALG_SHA_1
#if defined(PSA_WANT_ALG_SHA_224)
    case PSA_ALG_SHA_224:
      sha_algo     = SL_SI91X_SHA_224;
      *hash_length = SL_SI91X_SHA_224_DIGEST_LEN;
      break;
#endif // PSA_WANT_ALG_SHA_224
#if defined(PSA_WANT_ALG_SHA_256)
    case PSA_ALG_SHA_256:
      sha_algo     = SL_SI91X_SHA_256;
      *hash_length = SL_SI91X_SHA_256_DIGEST_LEN;
      break;
#endif // PSA_WANT_ALG_SHA_256
#if defined(PSA_WANT_ALG_SHA_384)
    case PSA_ALG_SHA_384:
      sha_algo     = SL_SI91X_SHA_384;
      *hash_length = SL_SI91X_SHA_384_DIGEST_LEN;
      break;
#endif // PSA_WANT_ALG_SHA_384
#if defined(PSA_WANT_ALG_SHA_512)
    case PSA_ALG_SHA_512:
      sha_algo     = SL_SI91X_SHA_512;
      *hash_length = SL_SI91X_SHA_512_DIGEST_LEN;
      break;
#endif // PSA_WANT_ALG_SHA_256
    default:
      *hash_length = SL_SI91X_SHA_LEN_INVALID;
      return PSA_ERROR_BAD_STATE;
  }

  status = convert_si91x_error_code_to_psa_status(sl_si91x_sha(sha_algo, (uint8_t *)input, input_length, hash));
  if (status != PSA_SUCCESS) {
    *hash_length = SL_SI91X_SHA_LEN_INVALID;
  }

#endif
  return status;
}
