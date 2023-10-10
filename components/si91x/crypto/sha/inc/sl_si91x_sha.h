/*******************************************************************************
 * @file  sl_si91x_sha.h
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

#ifndef SL_SI91X_SHA_H
#define SL_SI91X_SHA_H

#include "rsi_driver.h"
#include "sl_si91x_crypto.h"
#include "sl_status.h"
#include "sl_constants.h"
#include "sl_si91x_protocol_types.h"
#include "sl_si91x_driver.h"
#include <string.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#define SL_SI91x_SHA_LEN_INVALID 0

/// SHA modes
typedef enum {
  SL_SI91x_SHA_1 = 1,
  SL_SI91x_SHA_256,
  SL_SI91x_SHA_384,
  SL_SI91x_SHA_512,
  SL_SI91x_SHA_224
} sl_si91x_crypto_sha_mode_t;

/// SHA mode lenghts
typedef enum {
  SL_SI91x_SHA_1_DIGEST_LEN   = 20,
  SL_SI91x_SHA_256_DIGEST_LEN = 32,
  SL_SI91x_SHA_384_DIGEST_LEN = 48,
  SL_SI91x_SHA_512_DIGEST_LEN = 64,
  SL_SI91x_SHA_224_DIGEST_LEN = 28
} sl_si91x_sha_length_t;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/*==============================================*/
/**
 * @brief      Compute the SHA digest. This is a blocking API.
 * @param[in]  sha_mode     1 – For SHA1 \n 2 – For SHA256 \n 3 – For SHA384 \n 4 – For SHA512 \n 5 – For SHA224 \n
 * @param[in]  msg          - Pointer to message
 * @param[in]  msg_length   - Total message length
 * @param[in]  chunk_length - Current chunk length
 * @param[in]  pending_flag - BIT(0) - 1st chunk \n BIT(1) - Middle chunk \n BIT(2) - Last chunk \n
 * @param[out]  digest      - Output parameter to hold computed digest from SHA
 * @return     0              - Success \n
 *             Non-Zero Value - Failure
 * @note Refer Error Codes section for above error codes \ref error-codes.
 *
 */
static sl_status_t sha_pending(uint8_t sha_mode,
                               uint8_t *msg,
                               uint16_t msg_length,
                               uint16_t chunk_len,
                               uint8_t pending_flag,
                               uint8_t *digest);

/*==============================================*/
/**
 * @brief      Decide whether the SHA message can be sent once or requires multiple calls to send. This is a blocking API.
 * @param[in]  sha_mode   1 – For SHA1 \n 2 – For SHA256 \n 3 – For SHA384 \n 4 – For SHA512 \n  5– For SHA224 \n
 * @param[in]  msg        - Pointer to message
 * @param[in]  msg_length - Total message length
 * @param[out]  digest    - Output parameter to hold computed digest from SHA
 * @return     0              - Success \n
 *             Non-Zero Value - Failure
 *
 * @note Refer Error Codes section for above error codes \ref error-codes.
 *
 */
sl_status_t sl_si91x_sha(uint8_t sha_mode, uint8_t *msg, uint16_t msg_length, uint8_t *digest);
#endif /* SL_SI91X_SHA_H */
