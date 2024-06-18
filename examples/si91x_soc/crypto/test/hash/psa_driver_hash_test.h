/***************************************************************************/ /**
 * @file psa_driver_hash_test.h
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#ifndef PSA_DRIVER_HASH_TEST_H
#define PSA_DRIVER_HASH_TEST_H

#include "psa/crypto.h"
#include "unity.h"
#include "sl_system_init.h"
#include "sli_si91x_crypto_driver_functions.h"

#include <string.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

#define TRANSPARENT_HASH_COMPUTE_FCT sli_si91x_crypto_hash_compute

//Uncomment the macros one by one to start test for respective SHA

#define TEST_SL_SHA1_SHORT
//#define TEST_SL_SHA1_LONG
#define TEST_SL_SHA224_SHORT
//#define TEST_SL_SHA224_LONG
#define TEST_SL_SHA256_SHORT
//#define TEST_SL_SHA256_LONG
#define TEST_SL_SI91X_SHA384_SHORT
//#define TEST_SL_SI91X_SHA384_LONG
#define TEST_SL_SI91X_SHA512_SHORT
//#define TEST_SL_SI91X_SHA512_LONG

int test_psa_driver_hash(void);

#endif // PSA_DRIVER_HASH_TEST_H