/***************************************************************************/ /**
 * @file psa_driver_cipher_test.h
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

#include "psa/crypto.h"
#include "sli_si91x_crypto_driver_functions.h"
#include "unity.h"

#include "em_device.h"
#include "sl_status.h"
#include <string.h>

#if defined(ENABLE_TEST_COVERAGE)
extern void __gcov_flush(void);
#endif

typedef enum { plaintext = 0, invalid_key_type } test_key_type_t;

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

#define TRANSPARENT_CIPHER_ENCRYPT_FCT sli_si91x_crypto_cipher_encrypt
#define TRANSPARENT_CIPHER_DECRYPT_FCT sli_si91x_crypto_cipher_decrypt

#define AES_TEXT_MAX_OFFSET 256

int test_psa_driver_cipher(void);
