/***************************************************************************/ /**
 * @file  sl_si91x_psa_hmac.c
 * @brief SL SI91X PSA HMAC source file
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "sli_si91x_crypto_driver_functions.h"
#include "sl_si91x_hmac.h"
#include "sl_si91x_crypto.h"
#include "sl_status.h"
#include "sl_constants.h"
#include "sl_si91x_protocol_types.h"
#include "sl_si91x_driver.h"
#include <string.h>

#if defined(PSA_WANT_ALG_HMAC)
static psa_status_t sli_si91x_set_hash_type(const psa_key_attributes_t *attributes,
                                            psa_algorithm_t alg,
                                            uint8_t *hmac_sha_mode,
                                            size_t *digest_length)
{
  // Check key type and output size
  if (psa_get_key_type(attributes) != PSA_KEY_TYPE_HMAC) {
    // For HMAC, key type is strictly enforced
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  switch (PSA_ALG_HMAC_GET_HASH(alg)) {
    case PSA_ALG_SHA_1:
      *hmac_sha_mode = SL_SI91X_HMAC_SHA_1;
      *digest_length = SL_SI91X_HMAC_SHA_1_DIGEST_LEN;
      break;
    case PSA_ALG_SHA_256:
      *hmac_sha_mode = SL_SI91X_HMAC_SHA_256;
      *digest_length = SL_SI91X_HMAC_SHA_256_DIGEST_LEN;
      break;
    case PSA_ALG_SHA_384:
      *hmac_sha_mode = SL_SI91X_HMAC_SHA_384;
      *digest_length = SL_SI91X_HMAC_SHA_384_DIGEST_LEN;
      break;
    case PSA_ALG_SHA_512:
      *hmac_sha_mode = SL_SI91X_HMAC_SHA_512;
      *digest_length = SL_SI91X_HMAC_SHA_512_DIGEST_LEN;
      break;
    default:
      return PSA_ERROR_NOT_SUPPORTED;
      break;
  }

  return PSA_SUCCESS;
}
#endif // PSA_WANT_ALG_HMAC

/*****************************************************************************
* Compute mac using HMAC/CMAC.
*****************************************************************************/
psa_status_t sli_si91x_crypto_mac_compute(const psa_key_attributes_t *attributes,
                                          const uint8_t *key_buffer,
                                          size_t key_buffer_size,
                                          psa_algorithm_t alg,
                                          const uint8_t *input,
                                          size_t input_length,
                                          uint8_t *mac,
                                          size_t mac_size,
                                          size_t *mac_length)
{
#if defined(PSA_WANT_ALG_HMAC) || defined(PSA_WANT_ALG_CMAC)

  if (key_buffer == NULL || attributes == NULL || mac == NULL || mac_length == NULL || input == NULL
      || input_length <= 0) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  psa_status_t status;
  int32_t si91x_status;
  size_t key_size = psa_get_key_bits(attributes) / 8;

  sl_si91x_hmac_config_t config = { 0 };

#if defined(PSA_WANT_ALG_HMAC)
  if (PSA_ALG_IS_HMAC(alg)) {
    size_t digest_length;
    uint8_t hmac_sha_mode;
    status = sli_si91x_set_hash_type(attributes, alg, &hmac_sha_mode, &digest_length);
    if (status != PSA_SUCCESS) {
      return status;
    }

    if ((PSA_MAC_TRUNCATED_LENGTH(alg) > 0) && (PSA_MAC_TRUNCATED_LENGTH(alg) < digest_length)) {
      digest_length = PSA_MAC_TRUNCATED_LENGTH(alg);
    }

    if (mac_size < digest_length) {
      return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    config.msg_length = input_length;
    config.msg        = input;
    config.hmac_mode  = hmac_sha_mode;
#ifdef CHIP_917B0
    /* Fetch key type from attributes */
    psa_key_location_t location = PSA_KEY_LIFETIME_GET_LOCATION(psa_get_key_lifetime(attributes));
    if (location == 0) {
      config.key_config.B0.key_type = SL_SI91X_TRANSPARENT_KEY;
    } else {
      config.key_config.B0.key_type = SL_SI91X_WRAPPED_KEY;
    }

    /* Set key_size from key_buffer_size */
    config.key_config.B0.key_size = key_buffer_size;

    config.key_config.B0.key_slot     = 0;
    config.key_config.B0.wrap_iv_mode = SL_SI91X_WRAP_IV_CBC_MODE;
    if (config.key_config.B0.wrap_iv_mode == SL_SI91X_WRAP_IV_CBC_MODE) {
      memcpy(config.key_config.B0.wrap_iv, WRAP_IV, SL_SI91X_IV_SIZE);
    }
    memcpy(config.key_config.B0.key, key_buffer, key_buffer_size);
#else
    config.key_config.A0.key_length = key_buffer_size;
    memcpy(config.key_config.A0.key, key_buffer, key_buffer_size);
#endif

    status = sl_si91x_hmac(&config, mac);

    //Convert the error code from si91x to psa
    status = convert_si91x_error_code_to_psa_status(si91x_status);
    if (status != PSA_SUCCESS) {
      *mac_length = 0;
      return status;
    }

    // Report generated hmac length
    *mac_length = digest_length;
    return PSA_SUCCESS;
  }
#endif // PSA_WANT_ALG_HMAC

#else // PSA_WANT_ALG_HMAC || PSA_WANT_ALG_CMAC

  (void)attributes;
  (void)key_buffer;
  (void)key_buffer_size;
  (void)alg;
  (void)input;
  (void)input_length;
  (void)mac;
  (void)mac_size;
  (void)mac_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_HMAC || PSA_WANT_ALG_CMAC
}
