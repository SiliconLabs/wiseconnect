/*******************************************************************************
 * @file  sl_si91x_psa_trng.h
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

#ifndef SL_SI91X_PSA_TRNG_H
#define SL_SI91X_PSA_TRNG_H

#include "string.h"
/* Include file for error code conversion */
#include "sl_si91x_crypto.h"
#include "psa/crypto.h"

/***************************************************************************/ /**
 * @fn          psa_status_t sl_si91x_psa_get_random()
 * @brief
 *   This API attempts to read the requested amount of conditioned entropy from the TRNG peripheral.
 * @param[in]   len
 *   Amount of bytes to read from the FIFO into the output
 * @param[out]  output
 *   Output buffer to contain the entropy
 * @param[out]  out_len
 *   Amount of bytes actually written into the output buffer
 * @return
 *   returns psa_status_t PSA_SUCCESS  on success, on failure return error code
******************************************************************************/
psa_status_t sl_si91x_psa_get_random(uint8_t *output, size_t len, size_t *out_len);

/***************************************************************************/ /**
 * @fn          psa_status_t sli_si91x_crypto_trng_init()
 * @brief
 *   This API Checks entropy and initializes key which needs to be programmed to TRNG hardware engine.
 * @return
 *   returns psa_status_t PSA_SUCCESS  on success, on failure return error code
******************************************************************************/
psa_status_t sli_si91x_crypto_trng_init();

#endif /* SL_SI91X_PSA_TRNG_H */
