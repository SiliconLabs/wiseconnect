/***************************************************************************/ /**
 * @file psa_driver_mac_test.h
 * @brief Top level application functions.
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

#ifndef PSA_DRIVER_MAC_TEST_H
#define PSA_DRIVER_MAC_TEST_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "psa/crypto.h"
#include "unity.h"

#include "sli_si91x_crypto_driver_functions.h"
#include <string.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

typedef enum { plaintext = 0, invalid_key_type } test_key_type_t;

#define TRANSPARENT_MAC_COMPUTE sli_si91x_crypto_mac_compute

int test_psa_driver_mac(void);

#endif //PSA_DRIVER_MAC_TEST_DATA_H
