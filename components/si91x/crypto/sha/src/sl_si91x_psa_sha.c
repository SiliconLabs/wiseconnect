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

/**
 * \brief Process an Hashing operation.
 *
 * \note The signature of this function is that of a PSA driver
 *       hash_compute entry point. This function behaves as an hash_compute
 *       entry point as defined in the PSA driver interface specification for
 *       transparent drivers.
 *
 * \param[in]  alg                The SHA algorithm to compute.
 * \param[in]  input              Input data.
 * \param[in]  input_length       Size of given input.
 * \param[out] hash               Output buffer for the hash value.
 * \param[in]  hash_size          Size of the ciphertext buffer in bytes. This
 *                                must be appropriate for the selected algorithm
 * \param[out] hash_length        On success, the size of the output in the
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

  if ((input == NULL || input_length > 0) || (hash == NULL) || (hash_length == NULL) || (hash_size > 0)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  switch (alg) {
#if defined(PSA_WANT_ALG_SHA_1)
    case PSA_ALG_SHA_1:
      sha_algo     = SL_SI91x_SHA_1;
      *hash_length = SL_SI91x_SHA_1_DIGEST_LEN;
      break;
#endif // PSA_WANT_ALG_SHA_1
#if defined(PSA_WANT_ALG_SHA_224)
    case PSA_ALG_SHA_224:
      sha_algo     = SL_SI91x_SHA_224;
      *hash_length = SL_SI91x_SHA_224_DIGEST_LEN;
      break;
#endif // PSA_WANT_ALG_SHA_224
#if defined(PSA_WANT_ALG_SHA_256)
    case PSA_ALG_SHA_256:
      sha_algo     = SL_SI91x_SHA_256;
      *hash_length = SL_SI91x_SHA_256_DIGEST_LEN;
      break;
#endif // PSA_WANT_ALG_SHA_256
#if defined(PSA_WANT_ALG_SHA_384)
    case PSA_ALG_SHA_384:
      sha_algo     = SL_SI91x_SHA_384;
      *hash_length = SL_SI91x_SHA_384_DIGEST_LEN;
      break;
#endif // PSA_WANT_ALG_SHA_384
#if defined(PSA_WANT_ALG_SHA_512)
    case PSA_ALG_SHA_512:
      sha_algo     = SL_SI91x_SHA_512;
      *hash_length = SL_SI91x_SHA_512_DIGEST_LEN;
      break;
#endif // PSA_WANT_ALG_SHA_256
    default:
      *hash_length = SL_SI91x_SHA_LEN_INVALID;
      return PSA_ERROR_BAD_STATE;
  }

#endif
  return status;
}
