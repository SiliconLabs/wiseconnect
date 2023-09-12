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
#include <string.h>

#ifdef SLI_TRNG_DEVICE_SI91X
/* TRNG key */
uint32_t trng_key[TRNG_KEY_SIZE] = { 0x16157E2B, 0xA6D2AE28, 0x8815F7AB, 0x3C4FCF09 };
#endif //SLI_TRNG_DEVICE_SI91X
/*==============================================*/
/**
 * @fn          sl_status_t sl_si91x_trng_init()
 * @brief       This API Initializes the TRNG hardware engine
 * @return     0              - Success \n
 *             Non-Zero Value - Failure
 * @note Refer Error Codes section for above error codes \ref error-codes.
 *
 * @section description
 * This API checks the Entropy of TRNG and verifies TRNG functioning
 */
/*==============================================*/

sl_status_t sl_si91x_trng_init(sl_si91x_trng_config_t *config, uint32_t *output)
{
  sl_wifi_buffer_t *buffer;
  sl_si91x_packet_t *packet;
  sl_status_t status = SL_STATUS_OK;

  if ((config->trng_key == NULL) && (config->input_length == 0)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_si91x_trng_request_t *request = (sl_si91x_trng_request_t *)malloc(sizeof(sl_si91x_trng_request_t));

  if (request == NULL) {
    return SL_STATUS_ALLOCATION_FAILED;
  }

  memset(request, 0, sizeof(sl_si91x_trng_request_t));

  request->algorithm_type     = TRNG;
  request->algorithm_sub_type = TRNG_INIT;
  request->total_msg_length   = config->input_length;
  memcpy(request->trng_key, config->trng_key, TRNG_KEY_SIZE * 4);
  memcpy(request->msg, config->trng_test_data, config->input_length * 4);
  status = sl_si91x_driver_send_command(RSI_COMMON_REQ_ENCRYPT_CRYPTO,
                                        SI91X_COMMON_CMD_QUEUE,
                                        request,
                                        sizeof(sl_si91x_trng_request_t),
                                        SL_SI91X_WAIT_FOR_RESPONSE(32000),
                                        NULL,
                                        &buffer);
  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
  }
  VERIFY_STATUS_AND_RETURN(status);

  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  memcpy(output, packet->data, packet->length);

  free(request);
  return status;
}

/*==============================================*/
/**
 * @fn          sl_status_t sl_si91x_trng_entropy()
 * @brief       This API check the health of Trng
 * @return     0              - Success \n
 *             Non-Zero Value - Failure
 * @note Refer Error Codes section for above error codes \ref error-codes.
 *
 * @section description
 * This API checks the Entropy of TRNG
 */
/*==============================================*/

sl_status_t sl_si91x_trng_entropy(void)
{
  sl_status_t status;
  sl_si91x_packet_t *packet;
  sl_si91x_trng_request_t *request = (sl_si91x_trng_request_t *)malloc(sizeof(sl_si91x_trng_request_t));

  if (request == NULL) {
    return SL_STATUS_ALLOCATION_FAILED;
  }

  memset(request, 0, sizeof(sl_si91x_trng_request_t));

  request->algorithm_type     = TRNG;
  request->algorithm_sub_type = TRNG_ENTROPY;

  status = sl_si91x_driver_send_command(RSI_COMMON_REQ_ENCRYPT_CRYPTO,
                                        SI91X_COMMON_CMD_QUEUE,
                                        request,
                                        sizeof(sl_si91x_trng_request_t),
                                        SL_SI91X_WAIT_FOR(32000),
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);

  free(request);
  return status;
}

/*==============================================*/
/**
 * @fn          sl_status_t sl_si91x_trng_program_key(uint32_t *trng_key, uint16_t key_length)
 * @brief       This API initializes and programs the key required for TRNG hardware engine
 * @param[in]   trng_key - Pointer to trng_key
 * @param[in]   key_length - key_length - key length in Dwords (uint32_t)
 * @return     0              - Success \n
 *             Non-Zero Value - Failure
 * @note Refer Error Codes section for above error codes \ref error-codes.
 *
 * @section description
 * This API initializes and programs the key required for TRNG hardware engine
 */
/*==============================================*/

sl_status_t sl_si91x_trng_program_key(uint32_t *trng_key, uint16_t key_length)
{
  sl_status_t status;
  sl_wifi_buffer_t *buffer;
  sl_si91x_packet_t *packet;

  if ((trng_key == NULL) && (key_length != TRNG_KEY_SIZE)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_si91x_trng_request_t *request = (sl_si91x_trng_request_t *)malloc(sizeof(sl_si91x_trng_request_t));

  if (request == NULL) {
    return SL_STATUS_ALLOCATION_FAILED;
  }

  memset(request, 0, sizeof(sl_si91x_trng_request_t));

  request->algorithm_type     = TRNG;
  request->algorithm_sub_type = TRNG_KEY;
  memcpy(request->trng_key[0], trng_key, TRNG_KEY_SIZE * 4);

  status = sl_si91x_driver_send_command(RSI_COMMON_REQ_ENCRYPT_CRYPTO,
                                        SI91X_COMMON_CMD_QUEUE,
                                        request,
                                        sizeof(sl_si91x_trng_request_t),
                                        SL_SI91X_WAIT_FOR_RESPONSE(32000),
                                        NULL,
                                        &buffer);
  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
  }
  VERIFY_STATUS_AND_RETURN(status);

  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  memcpy(trng_key, packet->data, packet->length);

  free(request);
  return status;
}

/*==============================================*/
/**
 * @fn          sl_status_t sl_si91x_trng_get_random_num(uint32_t *random_number, uint16_t length)
 * @brief       This API generated random number of desired length
 * @param[in]   random_number - Address for Random number
 * @param[in]   length - length of random number in Dwords (uint32_t)
 * @return     0              - Success \n
 *             Non-Zero Value - Failure
 * @note Refer Error Codes section for above error codes \ref error-codes.
 *
 * @section description
 * This API generates random number of desired length
 */
/*==============================================*/

sl_status_t sl_si91x_trng_get_random_num(uint32_t *random_number, uint16_t length)
{
  sl_status_t status;
  sl_wifi_buffer_t *buffer;
  sl_si91x_packet_t *packet;

  if ((random_number == NULL) && (length == 0)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_si91x_trng_request_t *request = (sl_si91x_trng_request_t *)malloc(sizeof(sl_si91x_trng_request_t));

  if (request == NULL) {
    return SL_STATUS_ALLOCATION_FAILED;
  }

  memset(request, 0, sizeof(sl_si91x_trng_request_t));

  request->algorithm_type     = TRNG;
  request->algorithm_sub_type = TRNG_GENERATION;
  request->total_msg_length   = length;

  status = sl_si91x_driver_send_command(RSI_COMMON_REQ_ENCRYPT_CRYPTO,
                                        SI91X_COMMON_CMD_QUEUE,
                                        request,
                                        sizeof(sl_si91x_trng_request_t),
                                        SL_SI91X_WAIT_FOR_RESPONSE(32000),
                                        NULL,
                                        &buffer);
  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
  }
  VERIFY_STATUS_AND_RETURN(status);

  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  memcpy(random_number, packet->data, packet->length);

  free(request);

  //! Check for any duplicate elements
  status = sl_si91x_duplicate_element((uint32_t *)random_number, length / 4);
  if (status != SL_STATUS_OK) {
    printf("\r\n Duplicate elements present \r\n");
  }
  return status;
}

/*==============================================*/
/**
 * @fn          sl_status_t duplicate_element(uint32 *a,uint32_t length)
 * @brief       This API checks if there are any repeating elements in the Array.
 * @param[in]   a, Array
 * @param[in]   key_length, length of the Array
 * \retval SL_STATUS_OK Success
 * \retval SL_STATUS_TRNG_DUPLICATE_ENTROPY Failure
 *
 * @section description
 *       This API verifies that all the random numbers generated are unique
 */
/*==============================================*/

sl_status_t sl_si91x_duplicate_element(uint32_t *a, uint32_t length)
{
  uint32_t i, j;
  for (i = 0; i < (length - 1); i++) {
    for (j = i + 1; j < length; j++) {
      if (a[i] == a[j]) {
        return SL_STATUS_TRNG_DUPLICATE_ENTROPY;
      }
    }
  }
  return SL_STATUS_OK;
}
