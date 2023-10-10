/*******************************************************************************
 * @file  sl_si91x_psa_trng.c
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
#include "sl_si91x_psa_trng.h"
#include "sl_si91x_constants.h"
#include "sl_status.h"
#include "sl_si91x_trng.h"

#ifdef SLI_TRNG_DEVICE_SI91X
#include "rsi_driver.h"
#else // SLI_TRNG_DEVICE_SI91X
#include "rsi_rng.h"
#include "rsi_error.h"
#endif // SLI_TRNG_DEVICE_SI91X

#ifdef SLI_TRNG_DEVICE_SI91X
/* TRNG key */
extern uint32_t trng_key[TRNG_KEY_SIZE];
#endif //SLI_TRNG_DEVICE_SI91X

/*==============================================*/
/**
 * @fn          sl_status_t sl_si91x_trng_crypto_init()
 * @brief       This API initializes the TRNG
 * @param[in]   none,
 * @return      returns \ref SL_STATUS_OK  on success, on failure return error code.
 * @section description
 *       This API Checks entropy and initializes key which needs to be programmed to TRNG hardware engine
 */
/*==============================================*/

sl_status_t sl_si91x_psa_trng_init()
{
#ifdef SLI_TRNG_DEVICE_SI91X
  sl_status_t status = SL_STATUS_FAIL;
  /* This API checks the Entropy of TRNG i.e source for TRNG */
  status = sl_si91x_trng_entropy();
  if (status != SL_STATUS_OK) {
    printf("\r\n Improper entropy source of TRNG \r\n");
    return status;
  }

  /* This API Initializes key which needs to be programmed to TRNG hardware engine */
  status = sl_si91x_trng_program_key(trng_key, TRNG_KEY_SIZE);
  if (status != SL_STATUS_OK) {
    printf("\r\n TRNG Key Programming Failed \r\n");
    return status;
  }

#else  // SLI_TRNG_DEVICE_SI91X
  error_t status = SL_STATUS_OK;
  /* Enable the peripheral clocks for SET1 register */
  status = RSI_CLK_PeripheralClkEnable1(M4CLK, HWRNG_PCLK_ENABLE);
#endif // SLI_TRNG_DEVICE_SI91X
  return status;
}

/*==============================================*/
/**
 * \brief TRNG entropy retrieval
 *
 * This function attempts to read the requested amount of conditioned entropy
 * from the TRNG peripheral.
 *
 * \param[out] output   output buffer to contain the entropy
 * \param      len      Amount of bytes to read from the FIFO into the output
 * \param[out] out_len  Amount of bytes actually written into the output buffer
 *
 * \retval SL_STATUS_OK All of the requested entropy was output into the buffer
 * \retval SL_STATUS_INVALID_PARAMETER An error when the inputs are invalid
 * \retval SL_STATUS_SI91X_COMMAND_GIVEN_IN_INVALID_STATE TRNG not initialised correctly
 * \retval SL_STATUS_TRNG_DUPLICATE_ENTROPY An error when the entropy is returning a duplicate entropy
 */
sl_status_t sl_si91x_psa_get_random(uint8_t *output, size_t len, size_t *out_len)
{
  if (output == NULL || len == 0 || out_len == NULL) {
    return SL_STATUS_FAIL;
  }
  int32_t status = SL_STATUS_OK;

  //! Memset the buffer to zero
  memset(output, 0, len);
#ifdef SLI_TRNG_DEVICE_SI91X
  //! Get Random dwords of desired length
  status = sl_si91x_trng_get_random_num((uint32_t *)output, len);

#else  // SLI_TRNG_DEVICE_SI91X
  /* Start the Random Number Generation */
  status = RSI_RNG_Start(HWRNG, RSI_RNG_TRUE_RANDOM);

  /* Get random bytes */
  RSI_RNG_GetBytes(HWRNG, output, len);

  /* Stop the Random Number Generation */
  RSI_RNG_Stop(HWRNG);
#endif // SLI_TRNG_DEVICE_SI91X

  if (status != SL_STATUS_OK) {
    printf("\r\n Random number generation Failed \r\n");
    return status;
  }

  *out_len = len;
  return status;
}
