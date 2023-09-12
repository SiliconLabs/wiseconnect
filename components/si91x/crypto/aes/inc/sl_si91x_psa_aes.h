/*******************************************************************************
 * @file  sl_si91x_psa_aes.h
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

#ifndef SL_SI91X_PSA_AES_H
#define SL_SI91X_PSA_AES_H

#include "sli_si91x_crypto_driver_functions.h"
#include "string.h"
#include "sl_status.h"
/* Include file for error code conversion */
#include "sl_si91x_crypto.h"

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
                                             size_t *output_length);

psa_status_t sli_si91x_crypto_cipher_decrypt(const psa_key_attributes_t *attributes,
                                             const uint8_t *key_buffer,
                                             size_t key_buffer_size,
                                             psa_algorithm_t alg,
                                             const uint8_t *input,
                                             size_t input_length,
                                             uint8_t *output,
                                             size_t output_size,
                                             size_t *output_length)

#endif /* SL_SI91X_PSA_AES_H */