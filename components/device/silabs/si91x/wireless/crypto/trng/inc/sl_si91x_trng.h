/***************************************************************************/ /**
 * @file
 * @brief SL SI91X TRNG API
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

#pragma once
#include "sl_si91x_protocol_types.h"
#include "sl_si91x_crypto.h"
#include "sl_status.h"

#ifdef SLI_TRNG_DEVICE_SI91X
#include "sl_si91x_protocol_types.h"
#endif //SLI_TRNG_DEVICE_SI91X

#ifndef SLI_SI91X_TRNG_DUPLICATE_CHECK
// Enables duplicate check for TRNG
#define SLI_SI91X_TRNG_DUPLICATE_CHECK 1
#endif // SLI_SI91X_TRNG_DUPLICATE_CHECK

/******************************************************
 *                   Type Definitions
 ******************************************************/
/**
 * @addtogroup CRYPTO_TRNG_TYPES 
 * @{ 
 */

typedef struct {
  uint32_t *trng_test_data;
  uint16_t input_length;
  uint32_t *trng_key;
} sl_si91x_trng_config_t;

/** @} */

/******************************************************
 *                Function Declarations
*******************************************************/

/**
 * @addtogroup CRYPTO_TRNG_FUNCTIONS
 * @{ 
 */

/***************************************************************************/ /**
 * @brief
 *   This API Initializes the TRNG hardware engine.
 * @param[in] config
 *   Configuration object of type @ref sl_si91x_trng_config_t
 * @param[out] output
 *   Buffer to store the output.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
******************************************************************************/
sl_status_t sl_si91x_trng_init(sl_si91x_trng_config_t *config, uint32_t *output);

/***************************************************************************/ /**
 * @brief
 *   This API checks the Entropy of TRNG and verifies TRNG functioning.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
******************************************************************************/
sl_status_t sl_si91x_trng_entropy(void);

/***************************************************************************/ /**
 * @brief
 *   This API initializes and programs the key required for TRNG hardware engine.
 * @param[in] trng_key - Pointer to trng_key
 * @param[in] key_length - key_length - key length in Dwords (uint32_t)
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
******************************************************************************/
sl_status_t sl_si91x_trng_program_key(uint32_t *trng_key, uint16_t key_length);

/***************************************************************************/ /**
 * @brief This API generates random number of desired length.
 * @param[in]   random_number - Address for Random number
 * @param[in]   length - length of random number in bytes
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
******************************************************************************/
sl_status_t sl_si91x_trng_get_random_num(uint32_t *random_number, uint16_t length);

#if SLI_SI91X_TRNG_DUPLICATE_CHECK
/***************************************************************************/ /**
 * @brief This API checks if there are any repeating elements in the Array.
 * @param[in,out]  dword            Pointer to the array of elements.
 * @param[in]      length_in_dwords Length of the array in dwords.
 * @param[in]      index            Index of the element to be duplicated.
 * 
 * @return     None
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
******************************************************************************/
sl_status_t sl_si91x_duplicate_element(uint32_t *dword, uint32_t length_in_dwords);
#endif // SLI_SI91X_TRNG_DUPLICATE_CHECK

/** @} */