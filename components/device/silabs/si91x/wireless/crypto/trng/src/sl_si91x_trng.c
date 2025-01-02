/*******************************************************************************
 * @file  sl_si91x_trng.c
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

#include "sl_si91x_trng.h"
#include "sl_si91x_crypto.h"
#include "sl_status.h"
#include "sl_constants.h"
#include "sl_si91x_protocol_types.h"
#include "sl_si91x_driver.h"
#if defined(SLI_MULTITHREAD_DEVICE_SI91X)
#include "sl_si91x_crypto_thread.h"
#endif
#include <string.h>

#ifdef SLI_TRNG_DEVICE_SI91X
/* TRNG key */
uint32_t trng_key[TRNG_KEY_SIZE] = { 0x16157E2B, 0xA6D2AE28, 0x8815F7AB, 0x3C4FCF09 };
#endif //SLI_TRNG_DEVICE_SI91X

#ifndef SL_SI91X_SIDE_BAND_CRYPTO
static sl_status_t sli_si91x_trng_send_command(sl_si91x_trng_request_t *request, sl_wifi_buffer_t **buffer)
{
  sl_status_t status = SL_STATUS_OK;

#if defined(SLI_MULTITHREAD_DEVICE_SI91X)
  if (crypto_trng_mutex == NULL) {
    crypto_trng_mutex = sl_si91x_crypto_threadsafety_init(crypto_trng_mutex);
  }
  mutex_result = sl_si91x_crypto_mutex_acquire(crypto_trng_mutex);
#endif

  status = sl_si91x_driver_send_command(RSI_COMMON_REQ_ENCRYPT_CRYPTO,
                                        SI91X_COMMON_CMD,
                                        request,
                                        sizeof(sl_si91x_trng_request_t),
                                        SL_SI91X_WAIT_FOR_RESPONSE(32000),
                                        NULL,
                                        buffer);
  if (status != SL_STATUS_OK) {
    free(request);
    if (*buffer != NULL)
      sl_si91x_host_free_buffer(*buffer);
#if defined(SLI_MULTITHREAD_DEVICE_SI91X)
    mutex_result = sl_si91x_crypto_mutex_release(crypto_trng_mutex);
#endif
  }
  VERIFY_STATUS_AND_RETURN(status);

  return status;
}
#endif

sl_status_t sl_si91x_trng_init(sl_si91x_trng_config_t *config, uint32_t *output)
{
  sl_wifi_buffer_t *buffer = NULL;
#ifndef SL_SI91X_SIDE_BAND_CRYPTO
  sl_si91x_packet_t *packet = NULL;
#endif
  sl_status_t status = SL_STATUS_OK;

  if ((config->trng_key == NULL) || (config->input_length == 0) || (output == NULL)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_si91x_trng_request_t *request = (sl_si91x_trng_request_t *)malloc(sizeof(sl_si91x_trng_request_t));

  SL_VERIFY_POINTER_OR_RETURN(request, SL_STATUS_ALLOCATION_FAILED);

  memset(request, 0, sizeof(sl_si91x_trng_request_t));

  request->algorithm_type     = TRNG;
  request->algorithm_sub_type = TRNG_INIT;
  request->total_msg_length   = config->input_length;

#ifdef SL_SI91X_SIDE_BAND_CRYPTO
  request->trng_key = (uint8_t *)config->trng_key;
  request->msg      = (uint8_t *)config->trng_test_data;
  request->output   = (uint8_t *)output;
  status            = sl_si91x_driver_send_side_band_crypto(RSI_COMMON_REQ_ENCRYPT_CRYPTO,
                                                 request,
                                                 (sizeof(sl_si91x_trng_request_t)),
                                                 SL_SI91X_WAIT_FOR_RESPONSE(32000));
#else
  memcpy(request->trng_key, config->trng_key, TRNG_KEY_SIZE * 4);
  memcpy(request->msg, config->trng_test_data, config->input_length * 4);

  status = sli_si91x_trng_send_command(request, &buffer);

  if (status != SL_STATUS_OK) {
    return status;
  }
  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  memcpy(output, packet->data, packet->length);
#endif

  free(request);
  if (buffer != NULL)
    sl_si91x_host_free_buffer(buffer);
#if defined(SLI_MULTITHREAD_DEVICE_SI91X)
  mutex_result = sl_si91x_crypto_mutex_release(crypto_trng_mutex);
#endif
  return status;
}

sl_status_t sl_si91x_trng_entropy(void)
{
  sl_status_t status;
  sl_si91x_trng_request_t *request = (sl_si91x_trng_request_t *)malloc(sizeof(sl_si91x_trng_request_t));

  SL_VERIFY_POINTER_OR_RETURN(request, SL_STATUS_ALLOCATION_FAILED);

  memset(request, 0, sizeof(sl_si91x_trng_request_t));

  request->algorithm_type     = TRNG;
  request->algorithm_sub_type = TRNG_ENTROPY;

#if defined(SLI_MULTITHREAD_DEVICE_SI91X)
  if (crypto_trng_mutex == NULL) {
    crypto_trng_mutex = sl_si91x_crypto_threadsafety_init(crypto_trng_mutex);
  }
  mutex_result = sl_si91x_crypto_mutex_acquire(crypto_trng_mutex);
#endif
#ifdef SL_SI91X_SIDE_BAND_CRYPTO
  status = sl_si91x_driver_send_side_band_crypto(RSI_COMMON_REQ_ENCRYPT_CRYPTO,
                                                 request,
                                                 (sizeof(sl_si91x_trng_request_t)),
                                                 SL_SI91X_WAIT_FOR_RESPONSE(32000));
#else

  status = sl_si91x_driver_send_command(RSI_COMMON_REQ_ENCRYPT_CRYPTO,
                                        SI91X_COMMON_CMD,
                                        request,
                                        sizeof(sl_si91x_trng_request_t),
                                        SL_SI91X_WAIT_FOR(32000),
                                        NULL,
                                        NULL);
#endif
  free(request);
#if defined(SLI_MULTITHREAD_DEVICE_SI91X)
  mutex_result = sl_si91x_crypto_mutex_release(crypto_trng_mutex);
#endif
  return status;
}

sl_status_t sl_si91x_trng_program_key(uint32_t *trng_key, uint16_t key_length)
{
  sl_status_t status;
  sl_wifi_buffer_t *buffer = NULL;
#ifndef SL_SI91X_SIDE_BAND_CRYPTO
  sl_si91x_packet_t *packet = NULL;
#endif

  if ((trng_key == NULL) || (key_length != TRNG_KEY_SIZE)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_si91x_trng_request_t *request = (sl_si91x_trng_request_t *)malloc(sizeof(sl_si91x_trng_request_t));

  SL_VERIFY_POINTER_OR_RETURN(request, SL_STATUS_ALLOCATION_FAILED);

  memset(request, 0, sizeof(sl_si91x_trng_request_t));

  request->algorithm_type     = TRNG;
  request->algorithm_sub_type = TRNG_KEY;

#ifdef SL_SI91X_SIDE_BAND_CRYPTO
  request->trng_key = (uint8_t *)trng_key;
  request->output   = (uint8_t *)trng_key;
  status            = sl_si91x_driver_send_side_band_crypto(RSI_COMMON_REQ_ENCRYPT_CRYPTO,
                                                 request,
                                                 (sizeof(sl_si91x_trng_request_t)),
                                                 SL_SI91X_WAIT_FOR_RESPONSE(32000));
#else
  memcpy(request->trng_key, trng_key, TRNG_KEY_SIZE * 4);

  status = sli_si91x_trng_send_command(request, &buffer);

  if (status != SL_STATUS_OK) {
    return status;
  }
  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  memcpy(trng_key, packet->data, packet->length);
#endif

  free(request);
  if (buffer != NULL)
    sl_si91x_host_free_buffer(buffer);
#if defined(SLI_MULTITHREAD_DEVICE_SI91X)
  mutex_result = sl_si91x_crypto_mutex_release(crypto_trng_mutex);
#endif
  return status;
}

sl_status_t sl_si91x_trng_get_random_num(uint32_t *random_number, uint16_t length)
{
  sl_status_t status;
  sl_wifi_buffer_t *buffer  = NULL;
  sl_si91x_packet_t *packet = NULL;

  if ((random_number == NULL) || (length == 0) || (length > 1024)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_si91x_trng_request_t *request = (sl_si91x_trng_request_t *)malloc(sizeof(sl_si91x_trng_request_t));

  SL_VERIFY_POINTER_OR_RETURN(request, SL_STATUS_ALLOCATION_FAILED);

  memset(request, 0, sizeof(sl_si91x_trng_request_t));

  request->algorithm_type     = TRNG;
  request->algorithm_sub_type = TRNG_GENERATION;
  request->total_msg_length   = length;

#if defined(SLI_MULTITHREAD_DEVICE_SI91X)
  if (crypto_trng_mutex == NULL) {
    crypto_trng_mutex = sl_si91x_crypto_threadsafety_init(crypto_trng_mutex);
  }
  mutex_result = sl_si91x_crypto_mutex_acquire(crypto_trng_mutex);
#endif
#ifdef SL_SI91X_SIDE_BAND_CRYPTO
  request->output = (uint8_t *)random_number;
  status          = sl_si91x_driver_send_side_band_crypto(RSI_COMMON_REQ_ENCRYPT_CRYPTO,
                                                 request,
                                                 (sizeof(sl_si91x_trng_request_t)),
                                                 SL_SI91X_WAIT_FOR_RESPONSE(32000));
#else

  status = sl_si91x_driver_send_command(RSI_COMMON_REQ_ENCRYPT_CRYPTO,
                                        SI91X_COMMON_CMD,
                                        request,
                                        sizeof(sl_si91x_trng_request_t),
                                        SL_SI91X_WAIT_FOR_RESPONSE(32000),
                                        NULL,
                                        &buffer);
  if (status != SL_STATUS_OK) {
    free(request);
    if (buffer != NULL)
      sl_si91x_host_free_buffer(buffer);
#if defined(SLI_MULTITHREAD_DEVICE_SI91X)
    mutex_result = sl_si91x_crypto_mutex_release(crypto_trng_mutex);
#endif
  }
  VERIFY_STATUS_AND_RETURN(status);

  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  // Verify the response length is converted to bytes since
  // response is in bytes instead of DWORDs as expected in
  // request.
  SL_ASSERT(packet->length == (request->total_msg_length * sizeof(uint32_t)));
  SL_ASSERT(length <= packet->length);
  memcpy(random_number, packet->data, length);
#endif
#if SLI_SI91X_TRNG_DUPLICATE_CHECK
  //! Check for any duplicate elements
  if (packet != NULL)
    status = sl_si91x_duplicate_element((uint32_t *)packet->data, length / sizeof(uint32_t));
#endif // SLI_SI91X_TRNG_DUPLICATE_CHECK
  free(request);
  if (buffer != NULL)
    sl_si91x_host_free_buffer(buffer);
#if defined(SLI_MULTITHREAD_DEVICE_SI91X)
  mutex_result = sl_si91x_crypto_mutex_release(crypto_trng_mutex);
#endif
  return status;
}
#if SLI_SI91X_TRNG_DUPLICATE_CHECK
sl_status_t sl_si91x_duplicate_element(uint32_t *dword, uint32_t length_in_dwords)
{
  if (length_in_dwords == 0)
    return SL_STATUS_OK;
  uint32_t i, j;
  for (i = 0; i < (length_in_dwords - 1); i++) {
    for (j = i + 1; j < length_in_dwords; j++) {
      if (dword[i] == dword[j]) {
        return SL_STATUS_TRNG_DUPLICATE_ENTROPY;
      }
    }
  }
  return SL_STATUS_OK;
}
#endif // SLI_SI91X_TRNG_DUPLICATE_CHECK
