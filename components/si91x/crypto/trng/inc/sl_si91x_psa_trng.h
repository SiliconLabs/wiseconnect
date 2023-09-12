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
#include "sl_status.h"
/* Include file for error code conversion */
#include "sl_si91x_crypto.h"

sl_status_t sl_si91x_psa_get_random(uint8_t *output, size_t len, size_t *out_len);
sl_status_t sl_si91x_psa_trng_init();

#endif /* SL_SI91X_PSA_TRNG_H */