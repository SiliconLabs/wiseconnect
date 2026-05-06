/*******************************************************************************
 * @file  sl_si91x_psa_aead.h
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

#ifndef SL_SI91X_PSA_AEAD_H
#define SL_SI91X_PSA_AEAD_H

#include "sli_si91x_crypto_driver_functions.h"
#include "string.h"
#include "sl_status.h"
#include "sl_si91x_crypto.h"

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

#endif /* SL_SI91X_PSA_AEAD_H */