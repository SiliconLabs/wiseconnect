/***************************************************************************/ /**
 * @file psa_driver_aead_test.h
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

#ifndef PSA_DRIVER_AEAD_TEST_H
#define PSA_DRIVER_AEAD_TEST_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include "psa/crypto.h"
#include "unity.h"

#if defined(UNITY_OUTPUT_SERIAL)
#include "unity_conf.h"
#include "unity_usart.h"
#endif // UNITY_OUTPUT_SERIAL

#include <string.h>

#include "sli_si91x_crypto_driver_functions.h"
#include "sl_si91x_psa_wrap.h"
// -----------------------------------------------------------------------------
// Defines
// -----------------------------------------------------------------------------

typedef enum { plaintext = 0, invalid_key_type } test_key_type_t;

typedef enum { encrypt = 0, decrypt, invalid_mode_type } test_mode_type_t;

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

#define TRANSPARENT_AEAD_ENCRYPT sli_si91x_crypto_aead_encrypt
#define TRANSPARENT_AEAD_DECRYPT sli_si91x_crypto_aead_decrypt
#define FLASH_BASE               M4_FLASH_BASE

#define AES_TEXT_MAX_OFFSET 256
// Uncomment this macro to get more runtime diagnostic messages.
// #define BE_VERBOSE

int test_psa_driver_aead(void);

#endif //PSA_DRIVER_AEAD_TEST_H