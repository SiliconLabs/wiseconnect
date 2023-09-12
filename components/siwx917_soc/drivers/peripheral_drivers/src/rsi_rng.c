/*******************************************************************************
* @file  rsi_rng.c
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

// Include Files

#include "rsi_rom_rng.h"

/*==============================================*/
/**
 * @fn           uint32_t rng_start(HWRNG_Type *pRNG, uint8_t rngMode)     
 * @brief        This API is used to start the Random Number Generation
 * @param[in]    pRNG        :   pointer to the control register
 * @param[in]    rngMode     :   mode of random number generation
 * @return       return RSI_OK if successful execution
 */
uint32_t rng_start(HWRNG_Type *pRNG, uint8_t rngMode)
{
  if (rngMode == 0 || rngMode == 1) {
    if (rngMode == RSI_RNG_TRUE_RANDOM) {
      pRNG->HWRNG_CTRL_REG_b.HWRNG_RNG_ST = 1;
    } else {
      pRNG->HWRNG_CTRL_REG_b.HWRNG_PRBS_ST = 1;
    }
  } else {
    return ERROR_RNG_INVALID_ARG;
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           void rng_stop(HWRNG_Type *pRNG)    
 * @brief        This API is used to stop the Random Number Generation
 * @param[in]    pRNG        :   pointer to the control register
 * @return       none
 */
void rng_stop(HWRNG_Type *pRNG)
{
  //Disable clock
  pRNG->HWRNG_CTRL_REG = 0;
}

/*==============================================*/
/**
 * @fn           void rng_get_bytes(HWRNG_Type *pRNG, uint32_t *randomBytes, uint32_t numberOfBytes)      
 * @brief        This API is used to get the random number bytes
 * @param[in]    pRNG           :   pointer to the control register
 * @param[in]    randomBytes    :   random number register
 * @param[in]    numberOfBytes  :   number of bytes
 * @return       none
 */
void rng_get_bytes(HWRNG_Type *pRNG, uint32_t *randomBytes, uint32_t numberOfBytes)
{
  uint32_t i;

  for (i = 0; i < numberOfBytes; i++) {
    randomBytes[i] = pRNG->HWRNG_RAND_NUM_REG;
  }
}
const ROM_RNG_API_T rng_api = { &rng_start, &rng_stop, &rng_get_bytes };
