/***************************************************************************/ /**
 * @file
 * @brief SL SI91X TRNG API
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

#pragma once
#include "sl_si91x_protocol_types.h"
#include "sl_si91x_crypto.h"
#include "sl_status.h"

#ifdef SLI_TRNG_DEVICE_SI91X
#include "sl_si91x_protocol_types.h"
#endif //SLI_TRNG_DEVICE_SI91X

/******************************************************
 *                   Type Definitions
 ******************************************************/
typedef struct {
  uint32_t *trng_test_data;
  uint16_t input_length;
  uint32_t *trng_key;
} sl_si91x_trng_config_t;

/******************************************************
 *                Function Declarations
*******************************************************/
sl_status_t sl_si91x_trng_init(sl_si91x_trng_config_t *config, uint32_t *output);
sl_status_t sl_si91x_trng_entropy(void);
sl_status_t sl_si91x_trng_program_key(uint32_t *trng_key, uint16_t key_length);
sl_status_t sl_si91x_trng_get_random_num(uint32_t *random_number, uint16_t length);
sl_status_t sl_si91x_duplicate_element(uint32_t *a, uint32_t length);