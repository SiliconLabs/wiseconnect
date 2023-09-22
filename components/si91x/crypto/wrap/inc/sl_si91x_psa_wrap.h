/*******************************************************************************
 * @file  sl_si91x_psa_wrap.h
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

#ifndef SL_SI91X_PSA_WRAP_H
#define SL_SI91X_PSA_WRAP_H

#include "sli_si91x_crypto_driver_functions.h"
#include "string.h"
#include "sl_status.h"
#include "sl_si91x_crypto.h"

#define PSA_KEY_VOLATILE_PERSISTENT_WRAPPED ((psa_key_location_t)0x000001)

/**
 * \brief Wrap a key
 *
 * \param[in]  key_buffer         Input key buffer.
 * \param[in]  key_buffer_size    Key size.
 * \param[in]  wrap_mode          Wrapping method(ECB/CBC)
 * \param[in]  wrap_iv_var        IV for AES CBC mode
 *
 * \retval #PSA_SUCCESS Success.
 * \retval #PSA_ERROR
 */
psa_status_t sli_si91x_crypto_wrap_key(uint8_t *key_buffer,
                                       size_t key_buffer_size,
                                       uint32_t wrap_mode,
                                       uint8_t *wrap_iv_var);

/**
 * \brief Generate and wrap a symmetric key
 *
 * \param[in]  key_buffer         Input key buffer.
 * \param[in]  key_buffer_size    Key size.
 * \param[in]  wrap_mode          Wrapping method(ECB/CBC)
 * \param[in]  wrap_iv_var        IV for AES CBC mode
 *
 * \retval #PSA_SUCCESS Success.
 * \retval #PSA_ERROR
 */
psa_status_t sli_si91x_crypto_generate_symm_key(uint8_t *key_buffer,
                                                size_t key_buffer_size,
                                                uint32_t wrap_mode,
                                                uint8_t *wrap_iv_var);

#endif /* SL_SI91X_PSA_WRAP_H */
