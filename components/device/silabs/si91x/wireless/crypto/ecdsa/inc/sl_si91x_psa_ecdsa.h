/*******************************************************************************
 * @file  sl_si91x_psa_ecdsa.h
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_SI91X_PSA_ECDSA_H
#define SL_SI91X_PSA_ECDSA_H

#include "sli_si91x_crypto_driver_functions.h"
#include "string.h"
#include "sl_status.h"
#include "sl_si91x_crypto.h"

/**
 * \brief Generate a signature with input message using ECC private key
 *
 * \note The signature of this function is that of a PSA driver
 *       sign_message entry point. This function behaves as a sign_message
 *       entry point as defined in the PSA driver interface specification for
 *       transparent drivers.
 *
 * \param[in]  attributes         The attributes of the key to use for the
 *                                operation.
 * \param[in]  key_buffer         The buffer containing the key context.
 * \param      key_buffer_size    Size of the \p key_buffer buffer in bytes.
 * \param      alg                The ECDSA algorithm to compute.
 * \param[in]  input              Data is encryted and authenticated.
 * \param      input_length       Size of input in bytes.
 * \param[out] signature          Output buffer for the generated signature.
 * \param      signature_size     Size of the signature buffer in bytes. This
 *                                must be appropriate for the selected algorithm
 *                                and key:
 *                                - A sufficient output size is
 *                                  PSA_SIGN_OUTPUT_SIZE(key_type, key_bits, alg) 
 *                                  where key_type is the type
 *                                  of asymmetric key.
 *                                - PSA_VENDOR_ECDSA_SIGNATURE_MAX_SIZE 
 *                                  evaluates to the maximum
 *                                  signature size of any supported curve.
 * \param[out] signature_length   On success, the size of the output in the
 *                                signature buffer.
 *
 * \return
 * psa_status_t. See https://docs.silabs.com/gecko-platform/4.1/service/api/group-error for details.
 * \retval PSA_SUCCESS Success.
 * \retval PSA_ERROR_NOT_SUPPORTED
 *         \p alg is not supported.
 * \retval PSA_ERROR_INSUFFICIENT_MEMORY
 * \retval PSA_ERROR_BUFFER_TOO_SMALL
 *         signature buffer is too small.
 * \retval PSA_ERROR_CORRUPTION_DETECTED
 */

psa_status_t sli_si91x_crypto_sign_message(const psa_key_attributes_t *attributes,
                                           const uint8_t *key_buffer,
                                           size_t key_buffer_size,
                                           psa_algorithm_t alg,
                                           const uint8_t *input,
                                           size_t input_length,
                                           uint8_t *signature,
                                           size_t signature_size,
                                           size_t *signature_length);

/**
 * \brief * Verify a signature with input message using ECC public key.
 *
 * \note The signature of this function is that of a PSA driver
 *       verify_message entry point. This function behaves as a verify_message
 *       entry point as defined in the PSA driver interface specification for
 *       transparent drivers.
 *
 * \param[in]  attributes         The attributes of the key to use for the
 *                                operation.
 * \param[in]  key_buffer         The buffer containing the key context.
 * \param      key_buffer_size    Size of the \p key_buffer buffer in bytes.
 * \param      alg                The ECDSA algorithm to compute.
 * \param[in]  input              Data is authenticated.
 * \param      input_length       Size of input in bytes.
 * \param[in]  signature          The buffer containing signature.

 * \param[in] signature_length    The size of the signature buffer.
 *
 * \return
 * psa_status_t. See https://docs.silabs.com/gecko-platform/4.1/service/api/group-error for details.
 * \retval PSA_SUCCESS Success.
 * \retval PSA_ERROR_INVALID_SIGNATURE
 *         Failure, The signature is invald.
 * \retval PSA_ERROR_NOT_SUPPORTED
 *         \p alg is not supported.
 * \retval PSA_ERROR_INSUFFICIENT_MEMORY
 * \retval PSA_ERROR_CORRUPTION_DETECTED
 */

psa_status_t sli_si91x_crypto_verify_message(const psa_key_attributes_t *attributes,
                                             const uint8_t *key_buffer,
                                             size_t key_buffer_size,
                                             psa_algorithm_t alg,
                                             const uint8_t *input,
                                             size_t input_length,
                                             const uint8_t *signature,
                                             size_t signature_length);

#endif /* SL_SI91X_PSA_ECDSA_H */
