/*******************************************************************************
 * @file  sl_si91x_psa_ecdh.c
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
#include "sl_si91x_ecdh.h"
#include "sl_si91x_trng.h"
#include "sl_si91x_psa_ecdh.h"
#include "sli_si91x_crypto_driver_functions.h"
#include <stdio.h>

#define BUFFER_SIZE 32

#ifdef ECC_SEPR1_256
static uint8_t ecc_p256_gx[ECC_P256_LEN] = { 0x6B, 0x17, 0xD1, 0xF2, 0xE1, 0x2C, 0x42, 0x47, 0xF8, 0xBC, 0xE6,
                                             0xE5, 0x63, 0xA4, 0x40, 0xF2, 0x77, 0x03, 0x7D, 0x81, 0x2D, 0xEB,
                                             0x33, 0xA0, 0xF4, 0xA1, 0x39, 0x45, 0xD8, 0x98, 0xC2, 0x96 };
static uint8_t ecc_p256_gy[ECC_P256_LEN] = { 0x4F, 0xE3, 0x42, 0xE2, 0xFE, 0x1A, 0x7F, 0x9B, 0x8E, 0xE7, 0xEB,
                                             0x4A, 0x7C, 0x0F, 0x9E, 0x16, 0x2B, 0xCE, 0x33, 0x57, 0x6B, 0x31,
                                             0x5E, 0xCE, 0xCB, 0xB6, 0x40, 0x68, 0x37, 0xBF, 0x51, 0xF5 };
static uint8_t ecc_p256_z[ECC_P256_LEN]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1 };
#endif //ECC_SEPR1_256

#ifdef ECC_SEPR1_192
static uint8_t ecc_p192_gx[ECC_P192_LEN] = { 0x18, 0x8D, 0xA8, 0x0E, 0xB0, 0x30, 0x90, 0xF6, 0x7C, 0xBF, 0x20, 0xEB,
                                             0x43, 0xA1, 0x88, 0x00, 0xF4, 0xFF, 0x0A, 0xFD, 0x82, 0xFF, 0x10, 0x12 };
static uint8_t ecc_p192_gy[ECC_P192_LEN] = { 0x71, 0x92, 0xB9, 0x5F, 0xFC, 0x8D, 0xA7, 0x86, 0x31, 0x01, 0x1E, 0xD6,
                                             0xB2, 0x4C, 0xDD, 0x57, 0x3F, 0x97, 0x7A, 0x11, 0xE7, 0x94, 0x81, 0x1 };
static uint8_t ecc_p192_z[ECC_P192_LEN]  = { 0x1 };
#endif //ECC_SEPR1_192

psa_status_t sli_si91x_psa_generate_key_ecdh(const psa_key_attributes_t *attributes,
                                             const uint8_t *key_buffer,
                                             size_t key_buffer_size,
                                             size_t *key_buffer_length)
{
  (void)attributes;
  psa_status_t status = PSA_ERROR_GENERIC_ERROR;

#ifdef ECC_SEPR1_256
  if (key_buffer_size > 32)
    return PSA_ERROR_INVALID_ARGUMENT;

  status =
    convert_si91x_error_code_to_psa_status(sl_si91x_trng_get_random_num((uint32_t *)key_buffer, key_buffer_size));

  if (status != PSA_SUCCESS) {
    *key_buffer_length = 0;
    printf("\r\n Private key generation failed! Err code: %ld\r\n", status);
    return status;
  } else {
    *key_buffer_length = key_buffer_size;
    printf("\r\n Private key generation succeeded! ");
  }
  return status;
#endif

#ifdef ECC_SEPR1_192
  if (key_buffer_size > 24)
    return PSA_ERROR_INVALID_ARGUMENT;

  status =
    convert_si91x_error_code_to_psa_status(sl_si91x_trng_get_random_num((uint32_t *)key_buffer, key_buffer_size));

  if (status != PSA_SUCCESS) {
    *key_buffer_length = 0;
    printf("\r\n Private key generation failed! Err code: %ld\r\n", status);
    return status;
  } else {
    *key_buffer_length = key_buffer_size;
    printf("\r\n Private key generation succeeded! ");
  }
  return status;
#endif

  status = PSA_ERROR_NOT_SUPPORTED;
  return status;
}

psa_status_t sli_si91x_psa_export_public_key_ecdh(const psa_key_attributes_t *attributes,
                                                  const uint8_t *key_buffer,
                                                  size_t key_buffer_size,
                                                  uint8_t *data,
                                                  size_t data_size,
                                                  size_t *data_length)
{
  (void)attributes;
  psa_status_t status = PSA_ERROR_GENERIC_ERROR;

#ifdef ECC_SEPR1_256
  //! Buffers to store responses
  uint8_t ecc_p256_rx[ECC_P256_LEN] = { 0 };
  uint8_t ecc_p256_ry[ECC_P256_LEN] = { 0 };
  uint8_t ecc_p256_rz[ECC_P256_LEN] = { 0 };

  uint8_t ecc_p256_d[ECC_P256_LEN] = { 0 };
#endif //ECC_SEPR1_256

#ifdef ECC_SEPR1_192
  //! Buffers to store responses
  uint8_t ecc_p192_rx[ECC_P192_LEN] = { 0 };
  uint8_t ecc_p192_ry[ECC_P192_LEN] = { 0 };
  uint8_t ecc_p192_rz[ECC_P192_LEN] = { 0 };

  uint8_t ecc_p192_d[ECC_P192_LEN] = { 0 };
#endif //ECC_SEPR1_192

#ifdef ECC_SEPR1_256
  memcpy(ecc_p256_d, key_buffer, key_buffer_size);
  //!ECDH point multiplication
  status = convert_si91x_error_code_to_psa_status(sl_si91x_ecdh_point_multiplication(SL_SI91X_ECDH_256,
                                                                                     ecc_p256_d,
                                                                                     ecc_p256_gx,
                                                                                     ecc_p256_gy,
                                                                                     ecc_p256_z,
                                                                                     2,
                                                                                     ecc_p256_rx,
                                                                                     ecc_p256_ry,
                                                                                     ecc_p256_rz,
                                                                                     1));
  if (status != PSA_SUCCESS) {
    printf("\r\n Public key generation failed! Err code: %ld\r\n", status);
    return status;
  } else {
    *data_length = data_size;
    memcpy(data, ecc_p256_rx, sizeof(ecc_p256_rx));
    memcpy((data + (sizeof(ecc_p256_rx) / sizeof(ecc_p256_rx[0]))), ecc_p256_ry, sizeof(ecc_p256_ry));
    printf("\r\n Public key generation success! \r\n");
  }

  return status;
#endif //ECC_SEPR1_256

#ifdef ECC_SEPR1_192
  memcpy(ecc_p192_d, key_buffer, key_buffer_size);
  //!ECDH point multiplication
  status = convert_si91x_error_code_to_psa_status(sl_si91x_ecdh_point_multiplication(SL_SI91X_ECDH_192,
                                                                                     ecc_p192_d,
                                                                                     ecc_p192_gx,
                                                                                     ecc_p192_gy,
                                                                                     ecc_p192_z,
                                                                                     2,
                                                                                     ecc_p192_rx,
                                                                                     ecc_p192_ry,
                                                                                     ecc_p192_rz,
                                                                                     1));
  if (status != PSA_SUCCESS) {
    printf("\r\n Public key generation failed! Err code: %ld\r\n", status);
    return status;
  } else {
    *data_length = data_size;
    memcpy(data, ecc_p192_rx, sizeof(ecc_p192_rx));
    memcpy((data + (sizeof(ecc_p192_rx) / sizeof(ecc_p192_rx[0]))), ecc_p192_ry, sizeof(ecc_p192_rx));
    printf("\r\n Public key generation success! \r\n");
  }
  return status;
#endif //ECC_SEPR1_192

  // Make status as PSA_ERROR_NOT_SUPPORTED in case the curve type is not P-256 or P-192. This will do a software fallback
  status = PSA_ERROR_NOT_SUPPORTED;
  return status;
}

psa_status_t sli_si91x_psa_ecdh_key_agreement(psa_algorithm_t alg,
                                              const psa_key_attributes_t *attributes,
                                              const uint8_t *private_key,
                                              size_t private_key_len,
                                              const uint8_t *peer_key,
                                              size_t peer_key_length,
                                              uint8_t *shared_secret,
                                              size_t shared_secret_size,
                                              size_t *shared_secret_length)
{
  (void)alg;
  (void)attributes;
  (void)peer_key_length;
  psa_status_t status = PSA_ERROR_GENERIC_ERROR;

#ifdef ECC_SEPR1_256
  //! Buffers to store responses
  uint8_t ecc_p256_rx[ECC_P256_LEN] = { 0 };
  uint8_t ecc_p256_ry[ECC_P256_LEN] = { 0 };
  uint8_t ecc_p256_rz[ECC_P256_LEN] = { 0 };

  uint8_t ecc_p256_d[ECC_P256_LEN] = { 0 };
  uint8_t ecc_p256_px[BUFFER_SIZE] = { 0 };
  uint8_t ecc_p256_py[BUFFER_SIZE] = { 0 };
#endif //ECC_SEPR1_256

#ifdef ECC_SEPR1_192
  //! Buffers to store responses
  uint8_t ecc_p192_rx[ECC_P192_LEN] = { 0 };
  uint8_t ecc_p192_ry[ECC_P192_LEN] = { 0 };
  uint8_t ecc_p192_rz[ECC_P192_LEN] = { 0 };

  uint8_t ecc_p192_d[ECC_P192_LEN] = { 0 };
  uint8_t ecc_p192_px[BUFFER_SIZE] = { 0 };
  uint8_t ecc_p192_py[BUFFER_SIZE] = { 0 };
#endif //ECC_SEPR1_192

#ifdef ECC_SEPR1_256
  memcpy(ecc_p256_d, private_key, private_key_len);

  memcpy(ecc_p256_px, peer_key, sizeof(ecc_p256_px));
  memcpy(ecc_p256_py, ((peer_key + (sizeof(ecc_p256_px) / sizeof(ecc_p256_px[0])))), sizeof(ecc_p256_py));

  //!ECDH point multiplication
  status = convert_si91x_error_code_to_psa_status(sl_si91x_ecdh_point_multiplication(SL_SI91X_ECDH_256,
                                                                                     ecc_p256_d,
                                                                                     ecc_p256_px,
                                                                                     ecc_p256_py,
                                                                                     ecc_p256_z,
                                                                                     2,
                                                                                     ecc_p256_rx,
                                                                                     ecc_p256_ry,
                                                                                     ecc_p256_rz,
                                                                                     1));

  if (status != PSA_SUCCESS) {
    *shared_secret_length = 0;
    printf("\r\n Shared secret generation failed! Err code: %ld\r\n", status);
    return status;
  } else {
    memcpy(shared_secret, ecc_p256_rx, sizeof(ecc_p256_rx));
    memcpy((shared_secret + (sizeof(ecc_p256_rx) / sizeof(ecc_p256_rx[0]))), ecc_p256_ry, sizeof(ecc_p256_ry));
    *shared_secret_length = shared_secret_size;
    printf("\r\n Shared secret generation success! \r\n");
  }

  return status;
#endif //ECC_SEPR1_256

#ifdef ECC_SEPR1_192
  memcpy(ecc_p192_d, private_key, private_key_len);

  memcpy(ecc_p192_px, peer_key, ECC_P192_LEN);
  memcpy(ecc_p192_ry, peer_key + ECC_P192_LEN, ECC_P192_LEN);

  //!ECDH point multiplication
  status = convert_si91x_error_code_to_psa_status(sl_si91x_ecdh_point_multiplication(SL_SI91X_ECDH_192,
                                                                                     ecc_p192_d,
                                                                                     ecc_p192_px,
                                                                                     ecc_p192_py,
                                                                                     ecc_p192_z,
                                                                                     2,
                                                                                     ecc_p192_rx,
                                                                                     ecc_p192_ry,
                                                                                     ecc_p192_rz,
                                                                                     1));

  if (status != PSA_SUCCESS) {
    *shared_secret_length = 0;
    printf("\r\n Shared secret generation failed! Err code: %ld\r\n", status);
    return status;
  } else {
    memcpy(shared_secret, ecc_p192_px, ECC_P192_LEN);
    memcpy(shared_secret + ECC_P192_LEN, ecc_p192_py, ECC_P192_LEN);
    *shared_secret_length = shared_secret_size;
    printf("\r\n Shared secret generation success! \r\n");
  }

  return status;
#endif //ECC_SEPR1_192

  // Make status as PSA_ERROR_NOT_SUPPORTED in case the curve type is not P-256 or P-192. This will do a software fallback
  status = PSA_ERROR_NOT_SUPPORTED;
  return status;
}
