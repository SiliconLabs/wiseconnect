/*******************************************************************************
 * @file  sl_si91x_psa_aead.h
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

#ifndef SL_SI91X_PSA_AEAD_H
#define SL_SI91X_PSA_AEAD_H

#include "sli_si91x_crypto_driver_functions.h"
#include "psa/crypto.h"
#include "sl_si91x_crypto.h"
#include "sl_status.h"

#define SLI_SI91X_AEAD_NONCE_MAX_SIZE 16U
#define SLI_SI91X_AEAD_AD_MAX_SIZE    128U
#define SLI_SI91X_AEAD_TAG_MAX_SIZE   16U
#define SLI_SI91X_CCM_TAG_MIN_SIZE    4U

/** Driver multipart AEAD context (Si91x CCM).
 *  Populated progressively by encrypt_setup / set_nonce / set_lengths / update_ad.
 *  The firmware first-packet is sent at the first \c update call. */
typedef struct {
  uint8_t key_buffer[SL_SI91X_KEY_BUFFER_SIZE];
  size_t key_buffer_size;
  psa_algorithm_t alg;
  psa_key_type_t key_type;
  psa_key_lifetime_t key_lifetime;
  uint32_t key_bits;
  uint8_t is_encrypt;
  uint8_t nonce[SLI_SI91X_AEAD_NONCE_MAX_SIZE];
  uint16_t nonce_length;
  uint8_t ad[SLI_SI91X_AEAD_AD_MAX_SIZE];
  uint16_t ad_length;
  uint16_t ad_offset;
  uint16_t total_msg_length;
  uint16_t tag_length;
  uint8_t tag[SLI_SI91X_AEAD_TAG_MAX_SIZE];
  uint8_t fw_initialized;
  uint16_t processed_length;
  uint8_t *fw_output_buf;
} sli_si91x_crypto_aead_operation_t;

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
                                           size_t *ciphertext_length);

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
                                           size_t *plaintext_length);

#if defined(SLI_AEAD_DEVICE_SI91X)

/**
 * \brief Set up a multipart authenticated encryption operation.
 *
 * \param[in,out] ctx             Operation context to initialize.
 * \param[in]     attributes      Attributes of the key to use.
 * \param[in]     key_buffer      Buffer containing the key material.
 * \param         key_buffer_size Size of \p key_buffer in bytes.
 * \param         alg             The AEAD algorithm (e.g. PSA_ALG_CCM).
 *
 * \retval #PSA_SUCCESS
 * \retval #PSA_ERROR_NOT_SUPPORTED      \p alg is not supported.
 * \retval #PSA_ERROR_INVALID_ARGUMENT   Invalid key type or attributes.
 */
psa_status_t sli_si91x_psa_aead_encrypt_setup(sli_si91x_crypto_aead_operation_t *ctx,
                                              const psa_key_attributes_t *attributes,
                                              const uint8_t *key_buffer,
                                              size_t key_buffer_size,
                                              psa_algorithm_t alg);

/**
 * \brief Set up a multipart authenticated decryption operation.
 *
 * \param[in,out] ctx             Operation context to initialize.
 * \param[in]     attributes      Attributes of the key to use.
 * \param[in]     key_buffer      Buffer containing the key material.
 * \param         key_buffer_size Size of \p key_buffer in bytes.
 * \param         alg             The AEAD algorithm (e.g. PSA_ALG_CCM).
 *
 * \retval #PSA_SUCCESS
 * \retval #PSA_ERROR_NOT_SUPPORTED      \p alg is not supported.
 * \retval #PSA_ERROR_INVALID_ARGUMENT   Invalid key type or attributes.
 */
psa_status_t sli_si91x_psa_aead_decrypt_setup(sli_si91x_crypto_aead_operation_t *ctx,
                                              const psa_key_attributes_t *attributes,
                                              const uint8_t *key_buffer,
                                              size_t key_buffer_size,
                                              psa_algorithm_t alg);

/**
 * \brief Set the nonce for a multipart AEAD operation.
 *
 * \param[in,out] ctx          Operation context.
 * \param[in]     nonce        Buffer containing the nonce / IV.
 * \param         nonce_length Size of \p nonce in bytes.
 *
 * \retval #PSA_SUCCESS
 * \retval #PSA_ERROR_INVALID_ARGUMENT   \p nonce_length outside CCM bounds
 *                                       (SLI_SI91X_CCM_IV_MIN_SIZE ..
 *                                        SLI_SI91X_CCM_IV_MAX_SIZE, i.e. 7-13).
 */
psa_status_t sli_si91x_psa_aead_set_nonce(sli_si91x_crypto_aead_operation_t *ctx,
                                          const uint8_t *nonce,
                                          size_t nonce_length);

/**
 * \brief Declare the lengths of the AD and plaintext for a multipart AEAD
 *        operation.
 *
 * \param[in,out] ctx              Operation context.
 * \param         ad_length        Length of the additional data in bytes.
 * \param         plaintext_length Length of the plaintext / ciphertext in bytes.
 *
 * \retval #PSA_SUCCESS
 * \retval #PSA_ERROR_INVALID_ARGUMENT   Lengths exceed driver limits.
 */
psa_status_t sli_si91x_psa_aead_set_lengths(sli_si91x_crypto_aead_operation_t *ctx,
                                            size_t ad_length,
                                            size_t plaintext_length);

/**
 * \brief Feed additional data (AD) into a multipart AEAD operation.
 *
 * May be called multiple times; the AD bytes are accumulated internally.
 *
 * \param[in,out] ctx          Operation context.
 * \param[in]     input        Buffer containing the AD fragment.
 * \param         input_length Size of \p input in bytes.
 *
 * \retval #PSA_SUCCESS
 * \retval #PSA_ERROR_INVALID_ARGUMENT   Total AD exceeds declared ad_length.
 */
psa_status_t sli_si91x_psa_aead_update_ad(sli_si91x_crypto_aead_operation_t *ctx,
                                          const uint8_t *input,
                                          size_t input_length);

/**
 * \brief Feed message data into a multipart AEAD operation.
 *
 * Each call produces output of the same length as the input.  The caller
 * may supply a different output buffer on each call (e.g. a reused scratch
 * buffer).
 *
 * Internally the driver allocates a contiguous buffer for firmware: sized
 * \c total_msg_length + \c tag_length for encryption, or \c total_msg_length
 * for decryption.  Each chunk's output is copied back to the caller's
 * \p output after the firmware call.  The internal buffer is freed during
 * \c finish(), \c verify(), or \c abort().
 *
 * \param[in,out] ctx           Operation context.
 * \param[in]     input         Buffer containing the message fragment.
 * \param         input_length  Size of \p input in bytes.
 * \param[out]    output        Output buffer for encrypted / decrypted data.
 * \param         output_size   Size of \p output in bytes (>= \p input_length).
 * \param[out]    output_length On success, number of bytes written.
 *
 * \retval #PSA_SUCCESS
 * \retval #PSA_ERROR_NOT_SUPPORTED        \p input_length > UINT16_MAX.
 * \retval #PSA_ERROR_INVALID_ARGUMENT     NULL pointers, insufficient size,
 *                                         or cumulative input exceeds
 *                                         declared length.
 * \retval #PSA_ERROR_INSUFFICIENT_MEMORY  Internal allocation failure.
 */
psa_status_t sli_si91x_psa_aead_update(sli_si91x_crypto_aead_operation_t *ctx,
                                       const uint8_t *input,
                                       size_t input_length,
                                       uint8_t *output,
                                       size_t output_size,
                                       size_t *output_length);

/**
 * \brief Finish a multipart authenticated encryption operation.
 *
 * Finalizes the encryption and returns the authentication tag.  For
 * AD-only operations (no update calls), \p ciphertext may be NULL.
 *
 * \param[in,out] ctx               Operation context.
 * \param[out]    ciphertext        Buffer for any remaining ciphertext.
 * \param         ciphertext_size   Size of \p ciphertext in bytes.
 * \param[out]    ciphertext_length On success, remaining ciphertext bytes.
 * \param[out]    tag               Buffer to receive the authentication tag.
 * \param         tag_size          Size of \p tag in bytes.
 * \param[out]    tag_length        On success, tag length in bytes.
 *
 * \retval #PSA_SUCCESS
 * \retval #PSA_ERROR_BUFFER_TOO_SMALL   \p tag_size is too small.
 * \retval #PSA_ERROR_INSUFFICIENT_MEMORY  Internal allocation failure.
 */
psa_status_t sli_si91x_psa_aead_finish(sli_si91x_crypto_aead_operation_t *ctx,
                                       uint8_t *ciphertext,
                                       size_t ciphertext_size,
                                       size_t *ciphertext_length,
                                       uint8_t *tag,
                                       size_t tag_size,
                                       size_t *tag_length);

/**
 * \brief Finish a multipart authenticated decryption and verify the tag.
 *
 * Finalizes decryption and verifies the authentication tag.  For AD-only
 * operations (no update calls), \p plaintext may be NULL.
 *
 * \param[in,out] ctx              Operation context.
 * \param[out]    plaintext        Buffer for any remaining plaintext.
 * \param         plaintext_size   Size of \p plaintext in bytes.
 * \param[out]    plaintext_length On success, remaining plaintext bytes.
 * \param[in]     tag              Buffer containing the expected tag.
 * \param         tag_length       Size of \p tag in bytes.  Must equal the
 *                                 tag length established during setup
 *                                 (derived from the algorithm identifier).
 *
 * \retval #PSA_SUCCESS                   Tag verification succeeded.
 * \retval #PSA_ERROR_INVALID_SIGNATURE   Tag mismatch.
 * \retval #PSA_ERROR_INVALID_ARGUMENT    \p tag is NULL or \p tag_length does
 *                                        not match the algorithm's tag length.
 * \retval #PSA_ERROR_INSUFFICIENT_MEMORY  Internal allocation failure.
 */
psa_status_t sli_si91x_psa_aead_verify(sli_si91x_crypto_aead_operation_t *ctx,
                                       uint8_t *plaintext,
                                       size_t plaintext_size,
                                       size_t *plaintext_length,
                                       const uint8_t *tag,
                                       size_t tag_length);

/**
 * \brief Abort a multipart AEAD operation and release resources.
 *
 * Resets the firmware state (if initialized) and zeroes the context.
 * Safe to call on an already-aborted or zero-initialized context.
 *
 * \param[in,out] ctx  Operation context to abort.
 *
 * \retval #PSA_SUCCESS
 */
psa_status_t sli_si91x_psa_aead_abort(sli_si91x_crypto_aead_operation_t *ctx);

#endif /* SLI_AEAD_DEVICE_SI91X */

#endif /* SL_SI91X_PSA_AEAD_H */