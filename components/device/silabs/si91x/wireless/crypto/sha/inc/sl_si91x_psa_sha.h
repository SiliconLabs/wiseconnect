/*******************************************************************************
 * @file  sl_si91x_psa_sha.h
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

#ifndef SL_SI91X_PSA_SHA_H
#define SL_SI91X_PSA_SHA_H

#include "psa/crypto.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/**
 * Multipart hash operation state for the Si91x PSA SHA driver.
 *
 * The standard PSA streaming API (\c psa_hash_update, \c psa_hash_finish, etc.) does not pass
 * \c psa_algorithm_t on each call; the algorithm is fixed at \c psa_hash_setup and carried in
 * this operation object.
 *
 * A separate \c alg (\c psa_algorithm_t) member could be added, but it would add little value:
 * the side-band path sends commands to the TA, and the TA firmware holds the SHA context. The
 * host still needs a single field that both identifies the active algorithm for follow-up calls
 * and can be forced into an error state if something fails mid-stream. \p digest_size serves that
 * role: it is the expected digest length for the selected SHA mode, and on any intermediate
 * failure the implementation sets it to \c SL_SI91X_SHA_LEN_INVALID. The next \c update /
 * \c finish / \c abort then checks \p digest_size first and returns an error without relying on
 * a stale TA session. (Storing \c alg in addition would not remove that invalidation pattern.)
 *
 * The TA must still be given a concrete SHA mode (\c SL_SI91X_SHA_*) on \c update and \c finish.
 * That value can be derived from \p digest_size (unique per supported SHA variant here); the
 * \c switch in the \c .c file maps digest length → \c sha_mode. Keeping an \c alg member would be
 * optional and redundant for that mapping—the \c switch would remain—and was therefore omitted.
 */
typedef struct {
  size_t
    digest_size; ///< Expected digest length for the active hash, or \c SL_SI91X_SHA_LEN_INVALID if the operation is invalid / failed
} sli_si91x_crypto_sha_operation_t;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

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
 * \retval PSA_SUCCESS Success.
 * \retval PSA_ERROR_NOT_SUPPORTED
 *         \p alg is not supported.
 * \retval PSA_ERROR_INSUFFICIENT_MEMORY
 * \retval PSA_ERROR_BUFFER_TOO_SMALL
 *         ciphertext_size is too small.
 * \retval PSA_ERROR_CORRUPTION_DETECTED
 */
psa_status_t sli_si91x_crypto_hash_compute(psa_algorithm_t alg,
                                           const uint8_t *input,
                                           size_t input_length,
                                           uint8_t *hash,
                                           size_t hash_size,
                                           size_t *hash_length);

/**
 * \brief Set up a multipart hash operation.
 *
 * \note The signature of this function is that of a PSA driver
 *       hash_setup entry point as defined in the PSA driver interface
 *       specification for transparent drivers.
 *
 * \param[in,out] operation       Operation object to initialise. Must be
 *                                zero-initialized or previously aborted.
 * \param[in]     alg             The SHA algorithm to use
 *                                (\c PSA_ALG_SHA_1, \c PSA_ALG_SHA_224, etc.).
 *
 * \retval PSA_SUCCESS                  Operation set up successfully.
 * \retval PSA_ERROR_INVALID_ARGUMENT   \p operation is \c NULL or \p alg is
 *                                      not a hash algorithm.
 * \retval PSA_ERROR_BAD_STATE          \p alg is not a supported SHA variant.
 * \retval PSA_ERROR_NOT_SUPPORTED      No SHA algorithm is enabled.
 */
psa_status_t sli_si91x_crypto_hash_setup(sli_si91x_crypto_sha_operation_t *operation, psa_algorithm_t alg);

/**
 * \brief Feed input data into an ongoing multipart hash operation.
 *
 * \note The signature of this function is that of a PSA driver
 *       hash_update entry point as defined in the PSA driver interface
 *       specification for transparent drivers.
 *
 * \param[in,out] operation       Active operation object returned by a
 *                                successful \c sli_si91x_crypto_hash_setup.
 * \param[in]     input           Buffer containing the input data.
 *                                May be \c NULL if \p input_length is 0.
 * \param[in]     input_length    Size of the \p input buffer in bytes.
 *                                Must not exceed \c UINT16_MAX.
 *                                A zero-length update is a valid no-op.
 *
 * \retval PSA_SUCCESS                  Data accepted successfully.
 * \retval PSA_ERROR_INVALID_ARGUMENT   One or more arguments are invalid.
 * \retval PSA_ERROR_BAD_STATE          The operation is not active or has
 *                                      been invalidated by a prior failure.
 * \retval PSA_ERROR_NOT_SUPPORTED      No SHA algorithm is enabled.
 */
psa_status_t sli_si91x_crypto_hash_update(sli_si91x_crypto_sha_operation_t *operation,
                                          const uint8_t *input,
                                          size_t input_length);

/**
 * \brief Finish a multipart hash operation and produce the digest.
 *
 * \note The signature of this function is that of a PSA driver
 *       hash_finish entry point as defined in the PSA driver interface
 *       specification for transparent drivers.
 *
 * \param[in,out] operation       Active operation object to finalise.
 * \param[out]    hash            Buffer where the hash value will be written.
 * \param[in]     hash_size       Size of the \p hash buffer in bytes.
 * \param[out]    hash_length     On success, the number of bytes written
 *                                to \p hash.
 *
 * \retval PSA_SUCCESS                  Hash produced successfully.
 * \retval PSA_ERROR_INVALID_ARGUMENT   One or more arguments are invalid.
 * \retval PSA_ERROR_BUFFER_TOO_SMALL   \p hash_size is smaller than the
 *                                      digest length for the active algorithm.
 * \retval PSA_ERROR_BAD_STATE          The operation is not active or has
 *                                      been invalidated by a prior failure.
 * \retval PSA_ERROR_NOT_SUPPORTED      No SHA algorithm is enabled.
 */
psa_status_t sli_si91x_crypto_hash_finish(sli_si91x_crypto_sha_operation_t *operation,
                                          uint8_t *hash,
                                          size_t hash_size,
                                          size_t *hash_length);

/**
 * \brief Abort a multipart hash operation and release associated resources.
 *
 * \note The signature of this function is that of a PSA driver
 *       hash_abort entry point as defined in the PSA driver interface
 *       specification for transparent drivers.
 *
 * This function is idempotent: calling it on a zero-initialized,
 * already-finished, or previously-aborted operation succeeds with no effect.
 * No firmware command is sent because the firmware resets its SHA context
 * automatically on completion or failure.
 *
 * \param[in,out] operation       Operation object to abort. May be in any
 *                                state, including zero-initialized.
 *
 * \retval PSA_SUCCESS                  Operation aborted (or was already
 *                                      inactive).
 * \retval PSA_ERROR_INVALID_ARGUMENT   \p operation is \c NULL.
 * \retval PSA_ERROR_NOT_SUPPORTED      No SHA algorithm is enabled.
 */
psa_status_t sli_si91x_crypto_hash_abort(sli_si91x_crypto_sha_operation_t *operation);

#endif /* SL_SI91X_PSA_SHA_H */
