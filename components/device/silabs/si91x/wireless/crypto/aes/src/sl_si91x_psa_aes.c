/*******************************************************************************
 * @file  sl_si91x_psa_aes.c
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_si91x_aes.h"
#ifdef SL_SI91X_SIDE_BAND_CRYPTO
#include "sl_si91x_mp_aes.h"
#endif
#include "sl_si91x_crypto.h"
#include "sl_status.h"
#include "sl_constants.h"
#include "sl_si91x_protocol_types.h"
#include "sl_si91x_driver.h"
#include <string.h>

#define SLI_SI91X_MAX_DATA_SIZE_IN_BYTES_FOR_AES 1408

/*
 * The si91x firmware sideband AES multipart wrapper supports CBC, CTR and ECB.
 * Each PSA algorithm gate is checked individually so a build can opt into any
 * subset; the multipart driver entry points are compiled in if at least one
 * of the three modes is enabled.
 */
#if (defined(PSA_WANT_KEY_TYPE_AES)) \
  && ((defined(PSA_WANT_ALG_CTR)) || (defined(PSA_WANT_ALG_CBC_NO_PADDING)) || (defined(PSA_WANT_ALG_ECB_NO_PADDING)))
#define SLI_SI91X_PSA_AES_MULTIPART_ENABLED 1
#endif

/* True when the requested PSA algorithm is one we support for multipart AES. */
static inline bool sli_si91x_psa_alg_is_supported_multipart(psa_algorithm_t alg)
{
  switch (alg) {
#if defined(PSA_WANT_ALG_CTR)
    case PSA_ALG_CTR:
      return true;
#endif
#if defined(PSA_WANT_ALG_CBC_NO_PADDING)
    case PSA_ALG_CBC_NO_PADDING:
      return true;
#endif
#if defined(PSA_WANT_ALG_ECB_NO_PADDING)
    case PSA_ALG_ECB_NO_PADDING:
      return true;
#endif
    default:
      return false;
  }
}

static psa_status_t sli_si91x_psa_multipart_check_input_parameters(sli_si91x_crypto_cipher_operation_t *operation,
                                                                   const psa_key_attributes_t *attributes,
                                                                   const uint8_t *key_buffer,
                                                                   size_t key_buffer_size,
                                                                   psa_algorithm_t alg)
{
#if defined(SLI_SI91X_PSA_AES_MULTIPART_ENABLED)

  if (operation == NULL || attributes == NULL || key_buffer == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  if (key_buffer_size < (psa_get_key_bits(attributes) / 8)
      || !(psa_get_key_bits(attributes) == 128 || psa_get_key_bits(attributes) == 192
           || psa_get_key_bits(attributes) == 256)) {
    return PSA_ERROR_NOT_SUPPORTED;
  }

  if (psa_get_key_type(attributes) != PSA_KEY_TYPE_AES) {
    return PSA_ERROR_NOT_SUPPORTED;
  }

  if (!sli_si91x_psa_alg_is_supported_multipart(alg)) {
    return PSA_ERROR_NOT_SUPPORTED;
  }

  return PSA_SUCCESS;

#else // SLI_SI91X_PSA_AES_MULTIPART_ENABLED

  (void)operation;
  (void)attributes;
  (void)key_buffer;
  (void)key_buffer_size;
  (void)alg;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // SLI_SI91X_PSA_AES_MULTIPART_ENABLED
}

/*
 * Map a PSA algorithm to the SI91X AES sub-mode and indicate whether the mode
 * uses an IV. ECB has no IV; CBC and CTR do. Returns PSA_ERROR_NOT_SUPPORTED
 * if the algorithm is not one of the supported multipart modes.
 */
static psa_status_t sli_si91x_psa_alg_to_aes_mode(psa_algorithm_t alg, sl_si91x_aes_mode_t *aes_mode, bool *needs_iv)
{
  switch (alg) {
#if defined(PSA_WANT_ALG_CTR)
    case PSA_ALG_CTR:
      *aes_mode = SL_SI91X_AES_CTR;
      *needs_iv = true;
      return PSA_SUCCESS;
#endif
#if defined(PSA_WANT_ALG_CBC_NO_PADDING)
    case PSA_ALG_CBC_NO_PADDING:
      *aes_mode = SL_SI91X_AES_CBC;
      *needs_iv = true;
      return PSA_SUCCESS;
#endif
#if defined(PSA_WANT_ALG_ECB_NO_PADDING)
    case PSA_ALG_ECB_NO_PADDING:
      *aes_mode = SL_SI91X_AES_ECB;
      *needs_iv = false;
      return PSA_SUCCESS;
#endif
    default:
      (void)aes_mode;
      (void)needs_iv;
      return PSA_ERROR_NOT_SUPPORTED;
  }
}

/*
 * Build a transient sl_si91x_aes_config_t from the operation context for a
 * sideband multipart send. The caller passes chunk_length, aes_flags, msg
 * and output explicitly; mode/direction/key/iv are taken from the operation.
 *
 * Avoids a full memset of sl_si91x_aes_config_t (~1.5 KB on B0 due to the
 * embedded key_buffer) on every cipher_update by setting the small scalar
 * fields explicitly and letting the bulky key_config copy itself overwrite
 * the struct.  msg / msg_length are zeroed here so init-only / finalize-only
 * callers (which don't touch msg) don't ship stack garbage on the wire.
 */
static void sli_si91x_psa_build_aes_config(const sli_si91x_crypto_cipher_operation_t *operation,
                                           sl_si91x_aes_config_t *config_out)
{
  config_out->aes_mode        = operation->config.aes_mode;
  config_out->encrypt_decrypt = operation->config.encrypt_decrypt;
  /* ECB ignores IV; passing the pointer through is harmless. */
  config_out->iv         = operation->config.iv;
  config_out->msg        = NULL;
  config_out->msg_length = 0;
  config_out->key_config = operation->config.key_config;
}

/*
 * Release the plaintext key buffer that sli_si91x_configure_key_settings
 * malloc'd for the multipart operation on a0 (non-B0) builds. The one-shot
 * encrypt/decrypt paths free the same buffer inline; the multipart path
 * owns it for the whole operation lifetime and must release it from every
 * terminal entry point (finish / abort) and from every setup-time rollback
 * before the operation context is wiped, otherwise the pointer is zeroed
 * by memset and the allocation is leaked.
 *
 * Idempotent and safe on a zero-initialized operation: free(NULL) is a
 * no-op, and we clear the pointer after free so a double call is harmless.
 * On B0 builds the function is a no-op (the key lives inside the b0 key
 * buffer, no allocation is involved).
 */
static inline void sli_si91x_psa_release_multipart_key(sli_si91x_crypto_cipher_operation_t *operation)
{
#if !defined(SLI_SI917B0)
  if (operation != NULL) {
    free(operation->config.key_config.a0.key);
    operation->config.key_config.a0.key = NULL;
  }
#else
  (void)operation;
#endif
}

#ifdef SL_SI91X_SIDE_BAND_CRYPTO
/*
 * Sideband-only helpers.  The non-sideband (legacy NWP-command) path does
 * not need separate FIRST/LAST control-packet sends from PSA: its multipart
 * state machine is driven entirely from cipher_update via sl_si91x_aes_multipart
 * with FIRST/MIDDLE/LAST flags, and we deliberately do not change that path.
 *
 * These helpers exist purely to drive the sideband firmware context
 * lifecycle (sl_sb_aes_init / sl_sb_aes_final + sl_sb_aes_reset).
 */

/*
 * Send a FIRST_CHUNK control packet (chunk_length == 0) so the sideband
 * firmware AES context is fully initialized with key + IV.  Subsequent
 * calls only need to ship MIDDLE/LAST chunks; the host operation is moved
 * to MIDDLE state on success so cipher_update never re-sends FIRST.
 */
static psa_status_t sli_si91x_psa_send_first_chunk(sli_si91x_crypto_cipher_operation_t *operation)
{
  sl_si91x_aes_config_t config;
  sli_si91x_psa_build_aes_config(operation, &config);

  /*
   * Init-only send via the public multipart API.  The wire builder inside
   * sl_si91x_mp_aes_init() uses our config (mode, direction, key, iv) and
   * sends FIRST_CHUNK with chunk_length == 0 so the firmware runs
   * sl_sb_aes_reset + sl_sb_aes_init only.
   */
  sl_status_t si91x_status = sl_si91x_mp_aes_init(&config);
  psa_status_t status      = convert_si91x_error_code_to_psa_status(si91x_status);
  if (status == PSA_SUCCESS) {
    /* FW context is now initialized; future updates send MIDDLE_CHUNK. */
    operation->config.chunk_flag = SL_SI91X_AES_MIDDLE_CHUNK;
  }
  return status;
}

/*
 * Send a LAST_CHUNK control packet (chunk_length == 0) so the sideband
 * firmware finalizes and tears the multipart AES context down.  Used by
 * cipher_finish after the host has streamed all data via MIDDLE chunks.
 */
static psa_status_t sli_si91x_psa_send_last_chunk(const sli_si91x_crypto_cipher_operation_t *operation)
{
  sl_si91x_aes_config_t config;
  sli_si91x_psa_build_aes_config(operation, &config);
  sl_status_t si91x_status = sl_si91x_mp_aes_final(&config, 0, NULL);
  return convert_si91x_error_code_to_psa_status(si91x_status);
}
#endif /* SL_SI91X_SIDE_BAND_CRYPTO */

static void sli_si91x_configure_key_settings(const psa_key_attributes_t *attributes,
                                             void *key_config,
                                             const uint8_t *key_buffer,
                                             size_t key_buffer_size,
                                             bool is_multipart)
{
#if defined(SLI_SI917B0)
  sl_si91x_aes_key_config_b0_t *config_b0;
  if (is_multipart) {
    config_b0 = &((sli_si91x_psa_aes_multipart_config_t *)key_config)->key_config.b0;
  } else {
    config_b0 = &((sl_si91x_aes_config_t *)key_config)->key_config.b0;
  }

  /* Fetch key type from attributes */
  psa_key_location_t location = PSA_KEY_LIFETIME_GET_LOCATION(psa_get_key_lifetime(attributes));

  /* Determine key type based on storage location - different validation for different operation types */
  if (is_multipart) {
    config_b0->key_type = (location == PSA_KEY_LOCATION_LOCAL_STORAGE) ? SL_SI91X_TRANSPARENT_KEY
                                                                       : SL_SI91X_WRAPPED_KEY;
  } else {
    config_b0->key_type = (location == 0) ? SL_SI91X_TRANSPARENT_KEY : SL_SI91X_WRAPPED_KEY;
  }

  /* Set key_size from key_buffer_size */
  switch (key_buffer_size) {
    case 16:
      config_b0->key_size = SL_SI91X_AES_KEY_SIZE_128;
      break;
    case 24:
      config_b0->key_size = SL_SI91X_AES_KEY_SIZE_192;
      break;
    case 32:
      config_b0->key_size = SL_SI91X_AES_KEY_SIZE_256;
      break;
  }

  config_b0->key_slot = 0;
  memcpy(config_b0->key_buffer, key_buffer, config_b0->key_size);
#else
  sl_si91x_aes_key_config_a0_t *config_a0;
  if (is_multipart) {
    config_a0 = &((sli_si91x_psa_aes_multipart_config_t *)key_config)->key_config.a0;
  } else {
    config_a0 = &((sl_si91x_aes_config_t *)key_config)->key_config.a0;
  }

  config_a0->key        = (uint8_t *)malloc(key_buffer_size);
  config_a0->key_length = key_buffer_size;
  memcpy(config_a0->key, key_buffer, config_a0->key_length);
#endif
}

static void sli_si91x_set_input_config(const psa_key_attributes_t *attributes,
                                       sl_si91x_aes_config_t *config,
                                       const uint8_t *key_buffer,
                                       size_t key_buffer_size)
{
  sli_si91x_configure_key_settings(attributes, config, key_buffer, key_buffer_size, false);
}

static void sli_si91x_psa_multipart_set_input_config(const psa_key_attributes_t *attributes,
                                                     sli_si91x_psa_aes_multipart_config_t *config,
                                                     const uint8_t *key_buffer,
                                                     size_t key_buffer_size)
{
  sli_si91x_configure_key_settings(attributes, config, key_buffer, key_buffer_size, true);
}

// Configures AES mode, IV usage, and optional wrap-IV settings based on a PSA algorithm.
static psa_status_t sli_si91x_configure_aes_mode(psa_algorithm_t alg,
                                                 uint16_t *aes_mode,
                                                 sl_si91x_aes_config_t *config,
                                                 const uint8_t **aes_iv,
                                                 const uint8_t *iv)
{
  switch (alg) {
    case PSA_ALG_ECB_NO_PADDING:
      /* Setting mode as ECB_MODE */
      *aes_mode = SL_SI91X_AES_ECB;
#if defined(SLI_SI917B0)
      /* Setting wrap iv mode as WRAP_IV_ECB_MODE */
      config->key_config.b0.wrap_iv_mode = SL_SI91X_WRAP_IV_ECB_MODE;
#endif
      break;
    case PSA_ALG_CBC_NO_PADDING:
      /* Setting mode as CBC_MODE */
      *aes_mode = SL_SI91X_AES_CBC;
      /* setting aes_iv with iv */
      *aes_iv = iv;
#if defined(SLI_SI917B0)
      /* Setting wrap iv mode as WRAP_IV_CBC_MODE */
      config->key_config.b0.wrap_iv_mode = SL_SI91X_WRAP_IV_CBC_MODE;
#endif
      break;
    case PSA_ALG_CTR:
      /* Setting mode as CTR_MODE */
      *aes_mode = SL_SI91X_AES_CTR;
      /* setting aes_iv with iv */
      *aes_iv = iv;
      break;
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }
  return PSA_SUCCESS;
}

/*****************************************************************************
* Encrypt a message using a AES cipher.
*****************************************************************************/
psa_status_t sli_si91x_crypto_cipher_encrypt(const psa_key_attributes_t *attributes,
                                             const uint8_t *key_buffer,
                                             size_t key_buffer_size,
                                             psa_algorithm_t alg,
                                             const uint8_t *iv,
                                             size_t iv_length,
                                             const uint8_t *input,
                                             size_t input_length,
                                             uint8_t *output,
                                             size_t output_size,
                                             size_t *output_length)
{
  (void)iv_length;
  (void)output_size;
  /* Local variables */
  psa_status_t status          = PSA_SUCCESS;
  sl_status_t si91x_status     = SL_STATUS_OK;
  sl_si91x_aes_config_t config = { 0 };
  uint16_t aes_mode;
  const uint8_t *aes_iv = NULL;

  //! Input check
  if ((attributes == NULL) || (key_buffer == NULL) || (input == NULL) || (output == NULL) || (output_length == NULL)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  if (((alg == PSA_ALG_CTR) || (alg == PSA_ALG_CBC_NO_PADDING)) && (iv == NULL)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Configure AES mode (ECB/CBC/CTR) and IV pointer based on algorithm; return on unsupported algorithm
  status = sli_si91x_configure_aes_mode(alg, &aes_mode, &config, &aes_iv, iv);
  if (status != PSA_SUCCESS) {
    return status;
  }

  config.aes_mode        = aes_mode;
  config.encrypt_decrypt = SL_SI91X_AES_ENCRYPT;
  config.msg             = input;
  config.msg_length      = input_length;
  config.iv              = aes_iv;

  sli_si91x_set_input_config(attributes, &config, key_buffer, key_buffer_size);

  /* Calling sl_si91x_aes() for AES encryption */
  si91x_status = sl_si91x_aes(&config, output);

#if !defined(SLI_SI917B0)
  free(config.key_config.a0.key);
#endif

  /* gets the si91x error codes and returns its equivalent psa_status codes */
  status = convert_si91x_error_code_to_psa_status(si91x_status);

  if (status == PSA_SUCCESS) {
    *output_length = input_length;
  }

  return status;
}

/*****************************************************************************
 * Decrypt a message using a AES cipher.
******************************************************************************/
psa_status_t sli_si91x_crypto_cipher_decrypt(const psa_key_attributes_t *attributes,
                                             const uint8_t *key_buffer,
                                             size_t key_buffer_size,
                                             psa_algorithm_t alg,
                                             const uint8_t *input,
                                             size_t input_length,
                                             uint8_t *output,
                                             size_t output_size,
                                             size_t *output_length)
{
  (void)output_size;
  /* Local variables */
  psa_status_t status          = PSA_SUCCESS;
  sl_status_t si91x_status     = SL_STATUS_OK;
  sl_si91x_aes_config_t config = { 0 };
  uint16_t aes_mode;
  const uint8_t *aes_iv = NULL;

  //! Input pointer check
  if ((attributes == NULL) || (key_buffer == NULL) || (input == NULL) || (output == NULL) || (output_length == NULL)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // For decrypt, IV is extracted from input for CBC and CTR modes
  const uint8_t *iv = (alg == PSA_ALG_CBC_NO_PADDING || alg == PSA_ALG_CTR) ? input : NULL;

  status = sli_si91x_configure_aes_mode(alg, &aes_mode, &config, &aes_iv, iv);
  if (status != PSA_SUCCESS) {
    return status;
  }

  // Adjust input pointer and length for CBC and CTR modes
  if (alg == PSA_ALG_CBC_NO_PADDING || alg == PSA_ALG_CTR) {
    input += SL_SI91X_IV_SIZE;
    input_length -= SL_SI91X_IV_SIZE;
  }

  config.aes_mode        = aes_mode;
  config.encrypt_decrypt = SL_SI91X_AES_DECRYPT;
  config.msg             = input;
  config.msg_length      = input_length;
  config.iv              = aes_iv;

  sli_si91x_set_input_config(attributes, &config, key_buffer, key_buffer_size);

  /* Calling sl_si91x_aes() for AES decryption */
  si91x_status = sl_si91x_aes(&config, output);

#if !defined(SLI_SI917B0)
  free(config.key_config.a0.key);
#endif

  /* gets the si91x error codes and returns its equivalent psa_status codes */
  status = convert_si91x_error_code_to_psa_status(si91x_status);

  if (status == PSA_SUCCESS) {
    *output_length = input_length;
  }

  return status;
}

/*****************************************************************************
* Encrypt a message using a AES cipher multipart.
*****************************************************************************/
/*
 * Common setup helper that maps the PSA algorithm to the SI91X AES sub-mode
 * and primes the operation context. The encrypt/decrypt direction is the only
 * thing the public setup wrappers need to provide.
 */
static psa_status_t sli_si91x_crypto_cipher_setup_common(sli_si91x_crypto_cipher_operation_t *operation,
                                                         const psa_key_attributes_t *attributes,
                                                         const uint8_t *key_buffer,
                                                         size_t key_buffer_size,
                                                         psa_algorithm_t alg,
                                                         sl_si91x_aes_type_t direction)
{
  psa_status_t status =
    sli_si91x_psa_multipart_check_input_parameters(operation, attributes, key_buffer, key_buffer_size, alg);

  if (status != PSA_SUCCESS) {
    return status;
  }

  sl_si91x_aes_mode_t aes_mode = SL_SI91X_AES_CTR;
  bool needs_iv                = false;
  status                       = sli_si91x_psa_alg_to_aes_mode(alg, &aes_mode, &needs_iv);
  if (status != PSA_SUCCESS) {
    return status;
  }

  // Reset context.
  memset(operation, 0, sizeof(sli_si91x_crypto_cipher_operation_t));

  // Set up context.
  memcpy(&operation->alg, &alg, sizeof(alg));

  operation->config.aes_mode        = aes_mode;
  operation->config.encrypt_decrypt = direction;
  /*
   * chunk_flag tracks the host-side state machine for the firmware ctx:
   *   FIRST_CHUNK  -> FW ctx is NOT yet initialized; the next outbound
   *                   request must carry FIRST_CHUNK to drive sl_sb_aes_init.
   *   MIDDLE_CHUNK -> FW ctx is initialized; updates send MIDDLE_CHUNK and
   *                   finish sends LAST_CHUNK.
   * iv_flag is the "operation is in a valid streaming state" gate
   * consumed by cipher_update and cipher_finish.  At setup time it
   * encodes "is the IV present?": cleared for CBC/CTR (caller must
   * still call set_iv) and pre-set for ECB (no IV is needed for the
   * mode to be ready to stream).  cipher_update clears it back to 0
   * on any non-SUCCESS path so a buggy caller who finishes instead of
   * aborting after a failed update is rejected with BAD_STATE.
   */
  operation->config.chunk_flag = SL_SI91X_AES_FIRST_CHUNK;
  operation->config.iv_flag    = needs_iv ? 0 : 1;

  sli_si91x_psa_multipart_set_input_config(attributes, &operation->config, key_buffer, key_buffer_size);

#if defined(SLI_SI917B0)
  /*
   * Pre-set the wrap-iv mode for built-in/wrapped keys so the per-chunk
   * sideband request carries the correct wrap_iv_mode for the AES sub-mode.
   * This mirrors sli_si91x_configure_aes_mode() used by the one-shot path.
   */
  switch (alg) {
#if defined(PSA_WANT_ALG_CBC_NO_PADDING)
    case PSA_ALG_CBC_NO_PADDING:
      operation->config.key_config.b0.wrap_iv_mode = SL_SI91X_WRAP_IV_CBC_MODE;
      break;
#endif
#if defined(PSA_WANT_ALG_ECB_NO_PADDING)
    case PSA_ALG_ECB_NO_PADDING:
      operation->config.key_config.b0.wrap_iv_mode = SL_SI91X_WRAP_IV_ECB_MODE;
      break;
#endif
    default:
      /* CTR uses the default; nothing to do. */
      break;
  }
#endif

  /*
   * No NWP round-trip is issued from setup.  FIRST_CHUNK is deferred to
   * the first call that actually has the information the firmware needs
   * to initialize sb_aes_ctx and carries useful payload:
   *
   *   CBC/CTR -> cipher_set_iv() sends FIRST_CHUNK after staging the IV
   *              (sl_sb_aes_init seeds running-IV / counter from it).
   *
   *   ECB     -> the first cipher_update() lazily emits FIRST_CHUNK
   *              (chunk_length == 0, control-only) before sending its
   *              MIDDLE_CHUNK payload.  Doing it here in setup_common
   *              would burn one host->NWP round-trip and reserve
   *              sb_aes_ctx for callers that may follow up with
   *              cipher_abort() and never send any data, starving
   *              concurrent crypto users (Wi-Fi MAC/MGMT, TLS) for no
   *              useful work.
   *
   * Non-sideband path drives FIRST/MIDDLE/LAST from cipher_update via
   * sl_si91x_aes_multipart and does not need a separate init send here
   * either.
   */

  return PSA_SUCCESS;
}

psa_status_t sli_si91x_crypto_cipher_encrypt_setup(sli_si91x_crypto_cipher_operation_t *operation,
                                                   const psa_key_attributes_t *attributes,
                                                   const uint8_t *key_buffer,
                                                   size_t key_buffer_size,
                                                   psa_algorithm_t alg)
{
  return sli_si91x_crypto_cipher_setup_common(operation,
                                              attributes,
                                              key_buffer,
                                              key_buffer_size,
                                              alg,
                                              SL_SI91X_AES_ENCRYPT);
}

psa_status_t sli_si91x_crypto_cipher_decrypt_setup(sli_si91x_crypto_cipher_operation_t *operation,
                                                   const psa_key_attributes_t *attributes,
                                                   const uint8_t *key_buffer,
                                                   size_t key_buffer_size,
                                                   psa_algorithm_t alg)
{
  return sli_si91x_crypto_cipher_setup_common(operation,
                                              attributes,
                                              key_buffer,
                                              key_buffer_size,
                                              alg,
                                              SL_SI91X_AES_DECRYPT);
}

psa_status_t sli_si91x_crypto_cipher_update(sli_si91x_crypto_cipher_operation_t *operation,
                                            const uint8_t *input,
                                            size_t input_length,
                                            uint8_t *output,
                                            size_t output_size,
                                            size_t *output_length)
{
#if defined(SLI_SI91X_PSA_AES_MULTIPART_ENABLED)

  psa_status_t status      = PSA_ERROR_CORRUPTION_DETECTED;
  sl_status_t si91x_status = SL_STATUS_OK;

  if (output_length == NULL) {
    status = PSA_ERROR_INVALID_ARGUMENT;
    return status;
  }
  *output_length = 0;

  if (operation == NULL || (input == NULL && input_length > 0) || (output == NULL && output_size > 0)) {
    status = PSA_ERROR_INVALID_ARGUMENT;
    return status;
  }

  /*
   * Gate on iv_flag, which doubles as the "operation is in a valid
   * streaming state" check:
   *   - For CBC/CTR, set by cipher_set_iv after a successful IV stage.
   *   - For ECB, pre-set by setup_common (no IV required).
   *   - Cleared by *this function* on any prior failure return below,
   *     so a buggy caller who retries cipher_update after a failed
   *     one is rejected here rather than corrupting FW state further.
   * No further unconditional state changes happen before the
   * input-validation block; the failure-path policy that clears
   * iv_flag begins at the size-cap checks below.
   */
  if (operation->config.iv_flag == 0) {
    status = PSA_ERROR_BAD_STATE;
    return status;
  }

  /*
   * Sideband:    FW context is initialized via FIRST_CHUNK from
   *              cipher_set_iv (CBC/CTR) or lazily on the first
   *              cipher_update for ECB (see lazy-init block below). Cap
   *              is the uint16 wire-field width (firmware does its own
   *              per-chunk HW windowing).
   * Non-sideband: chunk_flag is FIRST_CHUNK on the first update by
   *              design; cap is the wire descriptor's embedded msg[]
   *              size (SLI_SI91X_MAX_DATA_SIZE_IN_BYTES_FOR_AES).
   */
  /*
   * Failure-path policy for the rest of this function.
   *
   * PSA semantics: any non-SUCCESS return from cipher_update puts the
   * operation into an error state and the caller is required to call
   * cipher_abort.  We harden against callers that violate that contract
   * (e.g. call cipher_finish instead of cipher_abort) by clearing
   * iv_flag on every post-state-machine failure path below.  Once
   * iv_flag is 0, this function will refuse subsequent updates with
   * BAD_STATE and cipher_finish will refuse to send LAST_CHUNK to a
   * potentially-wedged firmware multipart context.
   *
   * Zero-length update (PSA no-op) is intentionally excluded from this
   * policy: it does not touch FW state and PSA semantics allow it to
   * succeed without leaving the operation in an error state.
   */
#ifdef SL_SI91X_SIDE_BAND_CRYPTO
  if (input_length > UINT16_MAX) {
    operation->config.iv_flag = 0;
    status                    = PSA_ERROR_INVALID_ARGUMENT;
    return status;
  }
#else
  if (input_length > SLI_SI91X_MAX_DATA_SIZE_IN_BYTES_FOR_AES) {
    operation->config.iv_flag = 0;
    status                    = PSA_ERROR_INVALID_ARGUMENT;
    return status;
  }
#endif

  /* Zero-length update is a documented no-op (PSA semantics). */
  if (input_length == 0) {
    status = PSA_SUCCESS;
    return status;
  }

  /*
   * Raw block modes (CBC/ECB) must receive 16-byte aligned chunks. CTR is a
   * stream cipher mode and accepts arbitrary byte lengths in the firmware
   * multipart wrapper.
   */
  if ((operation->config.aes_mode == SL_SI91X_AES_CBC || operation->config.aes_mode == SL_SI91X_AES_ECB)
      && (input_length % SL_SI91X_AES_BLOCK_SIZE != 0)) {
    operation->config.iv_flag = 0;
    status                    = PSA_ERROR_INVALID_ARGUMENT;
    return status;
  }

  if (output_size < input_length) {
    operation->config.iv_flag = 0;
    status                    = PSA_ERROR_BUFFER_TOO_SMALL;
    return status;
  }

  operation->config.msg = input;

  // Populating sl_si91x_aes_config_t structure with sli_si91x_psa_aes_multipart_config_t structure
  sl_si91x_aes_config_t config;
  sli_si91x_psa_build_aes_config(operation, &config);
  config.msg        = operation->config.msg;
  config.msg_length = (uint16_t)input_length;

#ifdef SL_SI91X_SIDE_BAND_CRYPTO
  /*
   * Lazy FIRST_CHUNK send for ECB (deferred from setup_common to avoid
   * burning an NWP round-trip on a setup-then-abort caller).
   *
   * For CBC/CTR, cipher_set_iv has already issued FIRST_CHUNK and moved
   * chunk_flag to MIDDLE; reaching this point with chunk_flag still at
   * FIRST_CHUNK after the iv_flag guard above is only possible for ECB.
   * We still gate on aes_mode == ECB as defense in depth so an internal
   * state machine bug does not silently re-init a live CBC/CTR session.
   *
   * sli_si91x_psa_send_first_chunk flips chunk_flag to MIDDLE_CHUNK on
   * success, so the subsequent sl_si91x_mp_aes_update sends MIDDLE
   * without any second transition.  If FIRST fails we clear iv_flag and
   * bail out: no FW data state was changed (sl_sb_aes_init may have run
   * but no MIDDLE/LAST data was streamed), so an honest caller now sees
   * an errored op and is expected to call cipher_abort per PSA
   * semantics.  If they instead call cipher_finish, the iv_flag == 0
   * guard in finish will refuse to send LAST_CHUNK to that ambiguous
   * FW context.
   */
  if (operation->config.chunk_flag == SL_SI91X_AES_FIRST_CHUNK) {
    if (operation->config.aes_mode != SL_SI91X_AES_ECB) {
      operation->config.iv_flag = 0;
      return PSA_ERROR_BAD_STATE;
    }
    psa_status_t init_status = sli_si91x_psa_send_first_chunk(operation);
    if (init_status != PSA_SUCCESS) {
      operation->config.iv_flag = 0;
      return init_status;
    }
  }

  /*
   * Sideband: always send MIDDLE_CHUNK via the public multipart API. The
   * very first chunk's FIRST_CHUNK is handled either by
   * sli_si91x_psa_send_first_chunk() during set_iv (CBC/CTR) or by the
   * lazy-init block just above (ECB), so the firmware context is
   * already initialized here.
   */
  si91x_status = sl_si91x_mp_aes_update(&config, (uint16_t)input_length, output);
#else
  si91x_status = sl_si91x_aes_multipart(&config, (uint16_t)input_length, operation->config.chunk_flag, output);
#endif
  status = convert_si91x_error_code_to_psa_status(si91x_status);

  if (status == PSA_SUCCESS) {
#ifndef SL_SI91X_SIDE_BAND_CRYPTO
    if (operation->config.chunk_flag == SL_SI91X_AES_FIRST_CHUNK) {
      operation->config.chunk_flag = SL_SI91X_AES_MIDDLE_CHUNK;
    }
#endif
    /*
     * Track cumulative bytes streamed so cipher_finish can validate the
     * CBC/ECB block-alignment invariant at the finalize boundary.
     * Saturate at UINT32_MAX rather than wrapping: a wrap would silently
     * defeat the alignment check, while saturation surfaces nothing worse
     * than a misleading-but-aligned cumulative count and the per-update
     * alignment guard above still independently rejects any individual
     * mis-sized chunk. 4 GiB of multipart AES through a single PSA
     * operation is not a realistic embedded workload but the saturate
     * is cheap defense-in-depth.
     */
    if (operation->config.processed_length > UINT32_MAX - input_length) {
      operation->config.processed_length = UINT32_MAX;
    } else {
      operation->config.processed_length += (uint32_t)input_length;
    }
    *output_length = input_length;
  } else {
    /*
     * Wire send failed.  The firmware multipart context may or may not
     * have advanced past this chunk: for sideband, the MIDDLE_CHUNK
     * request either never reached the NWP, was rejected during
     * descriptor parse, or was accepted with partial side effects
     * (running IV / counter advance) -- we cannot tell from the host
     * side.  Clear iv_flag to mark the op as errored so a buggy caller
     * that runs cipher_finish instead of cipher_abort is rejected
     * with BAD_STATE rather than triggering a LAST_CHUNK send against
     * an indeterminate FW state.
     */
    operation->config.iv_flag = 0;
  }

  return status;

#else // SLI_SI91X_PSA_AES_MULTIPART_ENABLED

  (void)operation;
  (void)input;
  (void)input_length;
  (void)output;
  (void)output_size;
  (void)output_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // SLI_SI91X_PSA_AES_MULTIPART_ENABLED
}

psa_status_t sli_si91x_crypto_cipher_set_iv(sli_si91x_crypto_cipher_operation_t *operation,
                                            const uint8_t *iv,
                                            size_t iv_length)
{
#if defined(SLI_SI91X_PSA_AES_MULTIPART_ENABLED)

  if (operation == NULL || iv == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  if (iv_length != SL_SI91X_IV_SIZE) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  /*
   * Sanity-check that setup_common() actually populated the key descriptor
   * before we let an IV be staged. The "has a key" probe lives in different
   * union members on each chip variant and the two members do not overlap
   * 1:1 in the union layout, so this must be macro-gated rather than
   * relying on accidental field aliasing.
   *
   *   B0: key_size is the populated discriminator (16 / 24 / 32 bytes);
   *       sli_si91x_configure_key_settings() writes it from key_buffer_size
   *       and leaves it zero if the descriptor was never populated.
   *   a0: the populated discriminator is key_length; the plaintext key is
   *       heap-allocated and key_length records its size.
   */
#if defined(SLI_SI917B0)
  if (operation->config.key_config.b0.key_size == 0) {
    return PSA_ERROR_BAD_STATE;
  }
#else
  if (operation->config.key_config.a0.key == NULL || operation->config.key_config.a0.key_length == 0) {
    return PSA_ERROR_BAD_STATE;
  }
#endif

  /*
   * ECB has no IV. PSA core normally won't call set_iv for ECB, but if it
   * does we treat it as a benign no-op rather than a hard error so callers
   * that share code paths across modes don't have to special-case ECB.
   * ECB's FIRST_CHUNK is sent lazily by the first cipher_update; we
   * deliberately do not send it here even if an IV was passed, because
   * the FW ignores IV for ECB and emitting FIRST_CHUNK now would just
   * burn an NWP round-trip and reserve sb_aes_ctx for a caller that may
   * still abort without any data.
   */
  if (operation->config.aes_mode == SL_SI91X_AES_ECB) {
    return PSA_SUCCESS;
  }

  /*
   * set_iv must run before the first cipher_update. Re-running set_iv after
   * the firmware context is already initialized would silently leave the
   * old IV in place inside the FW, so we forbid it explicitly.
   */
  if (operation->config.chunk_flag != SL_SI91X_AES_FIRST_CHUNK) {
    return PSA_ERROR_BAD_STATE;
  }

  // Stage IV in the operation context and mark it as set.
  memcpy(operation->config.iv, iv, iv_length);
  operation->config.iv_flag = 1;

#ifdef SL_SI91X_SIDE_BAND_CRYPTO
  /*
   * Sideband-only.  This is the FIRST_CHUNK send for CBC/CTR.  ECB takes
   * a different path: setup_common does not send FIRST_CHUNK eagerly,
   * and cipher_update lazily emits FIRST_CHUNK on its first call with
   * non-zero data, so the two modes never reach this site concurrently.
   * We defer init for CBC/CTR until here because
   * sl_sb_aes_init seeds its running-IV (CBC) / counter (CTR) state
   * from the IV, and the IV only becomes available via cipher_set_iv
   * (PSA contract: set_iv runs after setup, before update).
   *
   * The chunk_flag != FIRST_CHUNK guard above also makes this idempotent
   * against a buggy PSA caller invoking set_iv twice: a second set_iv
   * is rejected with BAD_STATE, so we never re-init the FW context and
   * never re-send FIRST_CHUNK.
   *
   * Non-sideband path drives FIRST from cipher_update via
   * sl_si91x_aes_multipart and does not need an early init send here.
   */
  psa_status_t init_status = sli_si91x_psa_send_first_chunk(operation);
  if (init_status != PSA_SUCCESS) {
    /*
     * FIRST_CHUNK send failed.  Clear iv_flag so the op presents as
     * errored to cipher_update / cipher_finish (both of which gate on
     * iv_flag != 0); per PSA semantics the caller is required to call
     * cipher_abort.  chunk_flag is still FIRST_CHUNK (send_first_chunk
     * only flips it on success), so an honest caller that retries
     * set_iv would also pass the chunk_flag guard above -- we do not
     * forbid retry here because no FW data state was ever advanced and
     * a recoverable transient (e.g. NWP busy) is a plausible cause.
     */
    operation->config.iv_flag = 0;
    return init_status;
  }
#endif

  return PSA_SUCCESS;

#else // SLI_SI91X_PSA_AES_MULTIPART_ENABLED

  (void)operation;
  (void)iv;
  (void)iv_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // SLI_SI91X_PSA_AES_MULTIPART_ENABLED
}

psa_status_t sli_si91x_crypto_cipher_abort(sli_si91x_crypto_cipher_operation_t *operation)
{
#if defined(SLI_SI91X_PSA_AES_MULTIPART_ENABLED)

  /*
   * abort() only wipes the host-side context. We deliberately do NOT send a
   * LAST_CHUNK control packet here: that would call sl_sb_aes_final() on
   * the firmware, which for CBC/ECB validates that processed_length is
   * 16-byte aligned and would surface spurious errors when aborting an
   * in-progress stream.
   *
   * On sideband, the firmware multipart context (sb_aes_ctx) is global
   * (single-stream) and is left in active=1 state until the next
   * FIRST_CHUNK arrives; sl_aes_handler unconditionally invokes
   * sl_sb_aes_reset before sl_sb_aes_init on FIRST, so an abandoned
   * session cannot wedge the next one.  Concurrent multipart sessions
   * from independent callers are NOT supported on a single device:
   * each FIRST replaces whatever context was previously active.
   */
  if (operation != NULL) {
    sli_si91x_psa_release_multipart_key(operation);
    memset(operation, 0, sizeof(*operation));
  }

  return PSA_SUCCESS;

#else // SLI_SI91X_PSA_AES_MULTIPART_ENABLED

  (void)operation;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // SLI_SI91X_PSA_AES_MULTIPART_ENABLED
}

psa_status_t sli_si91x_crypto_cipher_finish(sli_si91x_crypto_cipher_operation_t *operation,
                                            uint8_t *output,
                                            size_t output_size,
                                            size_t *output_length)
{
#if defined(SLI_SI91X_PSA_AES_MULTIPART_ENABLED)

  psa_status_t status = PSA_ERROR_GENERIC_ERROR;
  (void)output;
  (void)output_size;

  if (output_length == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }
  *output_length = 0;

  if (operation == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  /* Validate that finish() was reached for a supported algorithm. */
  switch (operation->alg) {
#if defined(PSA_WANT_ALG_CTR)
    case PSA_ALG_CTR:
      status = PSA_SUCCESS;
      break;
#endif
#if defined(PSA_WANT_ALG_CBC_NO_PADDING)
    case PSA_ALG_CBC_NO_PADDING:
      status = PSA_SUCCESS;
      break;
#endif
#if defined(PSA_WANT_ALG_ECB_NO_PADDING)
    case PSA_ALG_ECB_NO_PADDING:
      status = PSA_SUCCESS;
      break;
#endif
    default:
      /* Wipe and bail out on unsupported algorithm. */
      sli_si91x_psa_release_multipart_key(operation);
      memset(operation, 0, sizeof(sli_si91x_crypto_cipher_operation_t));
      return PSA_ERROR_BAD_STATE;
  }

  /*
   * State-machine validation: cipher_finish must not succeed on an
   * operation that is not in a usable state.  iv_flag carries two
   * meanings now and we treat them uniformly here:
   *
   *   1. "IV has been staged."  Set by cipher_set_iv for CBC/CTR;
   *      pre-set to 1 by setup_common for ECB (which has no IV).
   *   2. "Operation is in a valid streaming state."  cipher_update
   *      clears iv_flag on any non-SUCCESS failure path (including
   *      ECB lazy-init failure and MIDDLE_CHUNK wire-send failure)
   *      so a buggy caller that runs cipher_finish instead of
   *      cipher_abort on an errored op is rejected here.
   *
   * Both meanings collapse to "iv_flag == 0 => caller must abort, not
   * finish".  The check is therefore not mode-gated: it correctly
   * rejects CBC/CTR-without-set_iv, CBC/CTR-with-failed-update, and
   * ECB-with-failed-update all the same.  We wipe state before
   * returning so the operation handle cannot be re-used in a
   * half-initialized configuration.
   */
  if (operation->config.iv_flag == 0) {
    sli_si91x_psa_release_multipart_key(operation);
    memset(operation, 0, sizeof(sli_si91x_crypto_cipher_operation_t));
    return PSA_ERROR_BAD_STATE;
  }

  /*
   * Structural validation: CBC/ECB are raw block modes and the
   * cumulative bytes streamed through cipher_update must be a multiple
   * of the AES block size at finish time (PSA_ALG_*_NO_PADDING contract).
   *
   * Today the per-update guard in cipher_update already enforces 16-byte
   * alignment on every individual chunk, so the sum is aligned by
   * induction and this check is a no-op in well-formed code paths.  We
   * keep it because (a) it makes the finalize-time contract explicit at
   * the validation point rather than only inductively at the streaming
   * point, (b) it remains correct if the per-update gate is ever
   * relaxed to support driver-side partial-block buffering (a real
   * possibility given PSA permits update(15) + update(17) = 32 even
   * though we currently reject the first half), and (c) it costs a
   * single modulo on a path that already pays for a NWP round-trip.
   *
   * CTR is a stream mode and accepts arbitrary cumulative byte lengths;
   * we deliberately do not gate it here.
   */
  if ((operation->config.aes_mode == SL_SI91X_AES_CBC || operation->config.aes_mode == SL_SI91X_AES_ECB)
      && (operation->config.processed_length % SL_SI91X_AES_BLOCK_SIZE) != 0) {
    sli_si91x_psa_release_multipart_key(operation);
    memset(operation, 0, sizeof(sli_si91x_crypto_cipher_operation_t));
    return PSA_ERROR_INVALID_ARGUMENT;
  }

#ifdef SL_SI91X_SIDE_BAND_CRYPTO
  /*
   * Sideband-only: drive the firmware finalize step.  We only send
   * LAST_CHUNK if the FW context was actually initialized (chunk_flag
   * transitioned to MIDDLE via cipher_set_iv for CBC/CTR or via the
   * lazy ECB FIRST_CHUNK send inside cipher_update).
   *
   * After the iv_flag check above, the only path that reaches here
   * with chunk_flag still at FIRST_CHUNK is an ECB operation that was
   * set up but never streamed any non-zero update (PSA setup -> finish
   * with no intervening cipher_update); there is nothing to finalize
   * on the firmware in that case and we just wipe the host context.
   *
   * Non-sideband path drives LAST_CHUNK from cipher_update via
   * sl_si91x_aes_multipart and does not need a separate finalize send here.
   */
  if (operation->config.chunk_flag == SL_SI91X_AES_MIDDLE_CHUNK) {
    psa_status_t last_status = sli_si91x_psa_send_last_chunk(operation);
    if (last_status != PSA_SUCCESS) {
      /*
       * Even if LAST failed, wipe local context so the operation handle
       * cannot be reused.  Surface the failure to the caller; the next
       * FIRST_CHUNK on a fresh operation will reset the FW context.
       */
      sli_si91x_psa_release_multipart_key(operation);
      memset(operation, 0, sizeof(sli_si91x_crypto_cipher_operation_t));
      return last_status;
    }
  }
#endif

  // Wipe context.
  sli_si91x_psa_release_multipart_key(operation);
  memset(operation, 0, sizeof(sli_si91x_crypto_cipher_operation_t));

  return status;

#else // SLI_SI91X_PSA_AES_MULTIPART_ENABLED

  (void)operation;
  (void)output;
  (void)output_size;
  (void)output_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // SLI_SI91X_PSA_AES_MULTIPART_ENABLED
}
