/*******************************************************************************
 * @file  sl_si91x_psa_sha.c
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
#include "sl_si91x_sha.h"
#include "sli_si91x_crypto_driver_functions.h"
#include "sl_si91x_psa_sha.h"

#if defined(PSA_WANT_ALG_SHA_1) || defined(PSA_WANT_ALG_SHA_224) || defined(PSA_WANT_ALG_SHA_256) \
  || defined(PSA_WANT_ALG_SHA_384) || defined(PSA_WANT_ALG_SHA_512)
static psa_status_t sli_si91x_get_sha_params_from_alg(psa_algorithm_t alg, uint8_t *sha_algo, size_t *digest_size)
{
  switch (alg) {
#if defined(PSA_WANT_ALG_SHA_1)
    case PSA_ALG_SHA_1:
      *sha_algo    = SL_SI91X_SHA_1;
      *digest_size = SL_SI91X_SHA_1_DIGEST_LEN;
      break;
#endif
#if defined(PSA_WANT_ALG_SHA_224)
    case PSA_ALG_SHA_224:
      *sha_algo    = SL_SI91X_SHA_224;
      *digest_size = SL_SI91X_SHA_224_DIGEST_LEN;
      break;
#endif
#if defined(PSA_WANT_ALG_SHA_256)
    case PSA_ALG_SHA_256:
      *sha_algo    = SL_SI91X_SHA_256;
      *digest_size = SL_SI91X_SHA_256_DIGEST_LEN;
      break;
#endif
#if defined(PSA_WANT_ALG_SHA_384)
    case PSA_ALG_SHA_384:
      *sha_algo    = SL_SI91X_SHA_384;
      *digest_size = SL_SI91X_SHA_384_DIGEST_LEN;
      break;
#endif
#if defined(PSA_WANT_ALG_SHA_512)
    case PSA_ALG_SHA_512:
      *sha_algo    = SL_SI91X_SHA_512;
      *digest_size = SL_SI91X_SHA_512_DIGEST_LEN;
      break;
#endif
    default:
      return PSA_ERROR_BAD_STATE;
  }
  return PSA_SUCCESS;
}

static psa_status_t sli_si91x_get_sha_algo_from_digest_size(size_t digest_size, uint8_t *sha_algo)
{
  switch (digest_size) {
#if defined(PSA_WANT_ALG_SHA_1)
    case SL_SI91X_SHA_1_DIGEST_LEN:
      *sha_algo = SL_SI91X_SHA_1;
      break;
#endif
#if defined(PSA_WANT_ALG_SHA_224)
    case SL_SI91X_SHA_224_DIGEST_LEN:
      *sha_algo = SL_SI91X_SHA_224;
      break;
#endif
#if defined(PSA_WANT_ALG_SHA_256)
    case SL_SI91X_SHA_256_DIGEST_LEN:
      *sha_algo = SL_SI91X_SHA_256;
      break;
#endif
#if defined(PSA_WANT_ALG_SHA_384)
    case SL_SI91X_SHA_384_DIGEST_LEN:
      *sha_algo = SL_SI91X_SHA_384;
      break;
#endif
#if defined(PSA_WANT_ALG_SHA_512)
    case SL_SI91X_SHA_512_DIGEST_LEN:
      *sha_algo = SL_SI91X_SHA_512;
      break;
#endif
    default:
      return PSA_ERROR_BAD_STATE;
  }
  return PSA_SUCCESS;
}
#endif

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
      || ((hash_length == NULL) && (hash_size > 0)) || (input_length > UINT16_MAX)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  status = sli_si91x_get_sha_params_from_alg(alg, &sha_algo, hash_length);
  if (status != PSA_SUCCESS) {
    *hash_length = SL_SI91X_SHA_LEN_INVALID;
    return status;
  }

#ifdef SL_SI91X_SIDE_BAND_CRYPTO
  status = convert_si91x_error_code_to_psa_status(
    sl_si91x_mp_sha(sha_algo, (uint8_t *)input, (uint16_t)input_length, (FIRST_CHUNK | LAST_CHUNK), hash));
  if (status != PSA_SUCCESS) {
    *hash_length = SL_SI91X_SHA_LEN_INVALID;
  }
#else
  status =
    convert_si91x_error_code_to_psa_status(sl_si91x_sha(sha_algo, (uint8_t *)input, (uint16_t)input_length, hash));
  if (status != PSA_SUCCESS) {
    *hash_length = SL_SI91X_SHA_LEN_INVALID;
  }
#endif

#else
  // Suppress unused parameter warnings when no SHA algorithms are enabled
  (void)alg;
  (void)input;
  (void)input_length;
  (void)hash;
  (void)hash_size;
  (void)hash_length;
  (void)sha_algo;
#endif
  return status;
}

psa_status_t sli_si91x_crypto_hash_setup(sli_si91x_crypto_sha_operation_t *operation, psa_algorithm_t alg)
{
#if defined(PSA_WANT_ALG_SHA_1) || defined(PSA_WANT_ALG_SHA_224) || defined(PSA_WANT_ALG_SHA_256) \
  || defined(PSA_WANT_ALG_SHA_384) || defined(PSA_WANT_ALG_SHA_512)

  psa_status_t status = PSA_ERROR_GENERIC_ERROR;
  uint8_t sha_algo    = 0;

  if (operation == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  if (!PSA_ALG_IS_HASH(alg)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Reset context.
  memset(operation, 0, sizeof(sli_si91x_crypto_sha_operation_t));

  status = sli_si91x_get_sha_params_from_alg(alg, &sha_algo, &operation->digest_size);
  if (status != PSA_SUCCESS) {
    operation->digest_size = SL_SI91X_SHA_LEN_INVALID;
    return status;
  }

  status = convert_si91x_error_code_to_psa_status(sl_si91x_mp_sha(sha_algo, NULL, 0, FIRST_CHUNK, NULL));
  if (status != PSA_SUCCESS) {
    operation->digest_size = SL_SI91X_SHA_LEN_INVALID;
  }
  return status;

#else // PSA_WANT_ALG_SHA_*

  (void)operation;
  (void)alg;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_SHA_*
}

psa_status_t sli_si91x_crypto_hash_update(sli_si91x_crypto_sha_operation_t *operation,
                                          const uint8_t *input,
                                          size_t input_length)
{
#if defined(PSA_WANT_ALG_SHA_1) || defined(PSA_WANT_ALG_SHA_224) || defined(PSA_WANT_ALG_SHA_256) \
  || defined(PSA_WANT_ALG_SHA_384) || defined(PSA_WANT_ALG_SHA_512)

  psa_status_t status = PSA_ERROR_GENERIC_ERROR;
  uint8_t sha_algo    = 0;

  if ((operation == NULL) || ((input == NULL) && (input_length > 0)) || (input_length > UINT16_MAX)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  /*
   * Per the PSA Crypto specification, a zero-length update is a valid no-op
   * and must succeed regardless of the input pointer value. Return early to
   * avoid forwarding this to sl_si91x_mp_sha, which rejects a non-NULL msg
   * with msg_length == 0 via its (msg == NULL) != (msg_length == 0) check.
   */
  if (input_length == 0) {
    return PSA_SUCCESS;
  }

  status = sli_si91x_get_sha_algo_from_digest_size(operation->digest_size, &sha_algo);
  if (status != PSA_SUCCESS) {
    return status;
  }

  status = convert_si91x_error_code_to_psa_status(
    sl_si91x_mp_sha(sha_algo, input, (uint16_t)input_length, MIDDLE_CHUNK, NULL));
  if (status != PSA_SUCCESS) {
    /*
     * Mark the operation as invalid so that subsequent update/finish calls
     * fail immediately. No internal abort is performed here; the PSA core
     * calls the driver's abort after a failed update before returning to the
     * application, and the application is also expected to call
     * psa_hash_abort() on any error per the PSA Crypto specification.
     */
    operation->digest_size = SL_SI91X_SHA_LEN_INVALID;
  }
  return status;

#else // PSA_WANT_ALG_SHA_*

  (void)operation;
  (void)input;
  (void)input_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_SHA_*
}

psa_status_t sli_si91x_crypto_hash_finish(sli_si91x_crypto_sha_operation_t *operation,
                                          uint8_t *hash,
                                          size_t hash_size,
                                          size_t *hash_length)
{
#if defined(PSA_WANT_ALG_SHA_1) || defined(PSA_WANT_ALG_SHA_224) || defined(PSA_WANT_ALG_SHA_256) \
  || defined(PSA_WANT_ALG_SHA_384) || defined(PSA_WANT_ALG_SHA_512)

  psa_status_t status = PSA_ERROR_GENERIC_ERROR;
  uint8_t sha_algo    = 0;

  if ((operation == NULL) || ((hash == NULL) && (hash_size > 0)) || (hash_length == NULL)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  if (hash_size < operation->digest_size) {
    return PSA_ERROR_BUFFER_TOO_SMALL;
  }

  status = sli_si91x_get_sha_algo_from_digest_size(operation->digest_size, &sha_algo);
  if (status != PSA_SUCCESS) {
    return status;
  }

  status = convert_si91x_error_code_to_psa_status(sl_si91x_mp_sha(sha_algo, NULL, 0, LAST_CHUNK, hash));
  if (status != PSA_SUCCESS) {
    /*
     * Mark the operation as invalid so that a repeated finish call fails
     * immediately. No internal abort is performed here; the PSA core calls
     * the driver's abort after a failed finish before returning to the
     * application, and the application is also expected to call
     * psa_hash_abort() on any error per the PSA Crypto specification.
     */
    operation->digest_size = SL_SI91X_SHA_LEN_INVALID;
  }
  *hash_length = operation->digest_size;
  return status;

#else // PSA_WANT_ALG_SHA_*

  (void)operation;
  (void)hash;
  (void)hash_size;
  (void)hash_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_SHA_*
}

psa_status_t sli_si91x_crypto_hash_abort(sli_si91x_crypto_sha_operation_t *operation)
{
#if defined(PSA_WANT_ALG_SHA_1) || defined(PSA_WANT_ALG_SHA_224) || defined(PSA_WANT_ALG_SHA_256) \
  || defined(PSA_WANT_ALG_SHA_384) || defined(PSA_WANT_ALG_SHA_512)

  if (operation == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  /*
   * No explicit command is needed to clear the firmware SHA context:
   *   1. On success, sha_final internally resets the SHA context.
   *   2. On failure, the firmware automatically resets the SHA context.
   *   3. If abort is called without a preceding init, the firmware SHA
   *      context was never allocated, so there is nothing to clear.
   * In all cases only the local operation context needs to be wiped.
   */
  memset(operation, 0, sizeof(sli_si91x_crypto_sha_operation_t));
  return PSA_SUCCESS;

#else // PSA_WANT_ALG_SHA_*

  (void)operation;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_SHA_*
}