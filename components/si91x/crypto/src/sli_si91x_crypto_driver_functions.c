/*******************************************************************************
 * @file  sli_si91x_crypto_driver_functions.c
 * @brief SLI SI91X PSA Crypto common utility file
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

#include "sli_si91x_crypto_driver_functions.h"
#include "sl_status.h"
#include "sl_additional_status.h"

/*==============================================*/
/**
 * @fn          psa_status_t convert_si91x_error_code_to_psa_status(sl_status_t si91x_status)
 * @brief       This API gets the si91x error codes and returns its equivalent psa_status codes
 * @param[in]   si91x_status, si91x error code
 * @return      returns psa_status error code.
 *
 * @section description
 *       This API gets the si91x error codes and returns its equivalent psa_status codes.
 */
psa_status_t convert_si91x_error_code_to_psa_status(sl_status_t si91x_status)
{
  psa_status_t status = PSA_SUCCESS;
  switch (si91x_status) {
    case SL_STATUS_OK:
      break;
    case SL_STATUS_INVALID_PARAMETER:
      status = PSA_ERROR_INVALID_ARGUMENT;
      break;
    case SL_STATUS_SI91X_COMMAND_GIVEN_IN_INVALID_STATE:
      status = PSA_ERROR_BAD_STATE;
      break;
    case SL_STATUS_ALLOCATION_FAILED:
      status = PSA_ERROR_COMMUNICATION_FAILURE;
      break;
    case SL_STATUS_NOT_SUPPORTED:
      status = PSA_ERROR_NOT_SUPPORTED;
      break;
    case SL_STATUS_TIMEOUT:
      status = PSA_ERROR_COMMUNICATION_FAILURE;
      break;
    default:
      status = PSA_ERROR_GENERIC_ERROR;
      break;
  }
  return status;
}
