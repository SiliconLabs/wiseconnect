/*******************************************************************************
* @file  rsi_rom_rng.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef __RSI_ROM_RNG_H__
#define __RSI_ROM_RNG_H__

/**
 * \ingroup   RSI_SPECIFIC_DRIVERS
 * \defgroup RSI_RNG_DRIVERS RSI:RS1xxxx RNG 
 *  @{
 *
 */
#include "rsi_ccp_user_config.h"
#include "rsi_packing.h"
#if defined(A11_ROM)
#include "rsi_rom_table_si91x.h"
#else
#include "rsi_rom_table_RS1xxxx.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @fn           STATIC INLINE uint32_t  RSI_RNG_Start(HWRNG_Type *pRNG, uint8_t rngMode)
 * @brief        This API is used to start the Random Number Generation
 * @param[in]    pRNG		 : Random Number Generator handler
 * @param[in]    rngMode : mode of the Random Number Generator
 *               			- \ref RNG_TRUE_RANDOM   - For True RNG
 *               			- \ref RNG_PSEUDO_RANDOM - For Psudo RNG
 * @return       returns 0 \ref RSI_OK on success ,non zero on failure
 *
 */
STATIC INLINE uint32_t RSI_RNG_Start(HWRNG_Type *pRNG, uint8_t rngMode)
{
#if defined(ROMDRIVER_PRESENT)
  return ROMAPI_RNG_API->rng_start(pRNG, rngMode);
#else
  return rng_start(pRNG, rngMode);
#endif
}

/**
 * @fn           STATIC INLINE void  RSI_RNG_Stop(HWRNG_Type *pRNG)
 * @brief        This API is used to stop the Random Number Generation
 * @param[in]    pRNG  : Random Number Generator handler
 * @return       none
 */
STATIC INLINE void RSI_RNG_Stop(HWRNG_Type *pRNG)
{
#if defined(ROMDRIVER_PRESENT)
  ROMAPI_RNG_API->rng_stop(pRNG);
#else
  rng_stop(pRNG);
#endif
}

/**
 * @fn           STATIC INLINE void  RSI_RNG_GetBytes(HWRNG_Type *pRNG, uint32_t *randomBytes, uint32_t numberOfBytes)
 * @brief        This API is used to get the random number bytes
 * @param[in]    pRNG	         : Random Number Generator handler
 * @param[in]    numberOfBytes : Number of bytes to generate
 * @param[out]   randomBytes	 : variable or array to store generated random bytes
 * @return       none
 */
STATIC INLINE void RSI_RNG_GetBytes(HWRNG_Type *pRNG, uint32_t *randomBytes, uint32_t numberOfBytes)
{
#if defined(ROMDRIVER_PRESENT)
  ROMAPI_RNG_API->rng_get_bytes(pRNG, randomBytes, numberOfBytes);
#else
  rng_get_bytes(pRNG, randomBytes, numberOfBytes);
#endif
}

#ifdef __cplusplus
}
#endif

#endif

/* @} end of RSI_RNG_DRIVERS */
