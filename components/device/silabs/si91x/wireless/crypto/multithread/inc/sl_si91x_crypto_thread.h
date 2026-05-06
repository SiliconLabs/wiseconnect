/***************************************************************************/ /**
 * @file sl_si91x_crypto_thread.h
 * @brief Top level application functions.
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

/******************************************************
 *                    Includes
*******************************************************/
#include <string.h>
#include "cmsis_os2.h"
#include "sl_net.h"
#include "sl_wifi.h"
#include "sl_status.h"

/******************************************************
 *                Variable Declarations
*******************************************************/
/**
 * @addtogroup CRYPTO_THREAD_VARIABLES
 * @{ 
 */

/**
 * @brief Semaphore for AES crypto operations.
 */
extern osSemaphoreId_t crypto_aes_mutex;

/**
 * @brief Semaphore for CCM crypto operations.
 */
extern osSemaphoreId_t crypto_ccm_mutex;

/**
 * @brief Semaphore for SHA crypto operations.
 */
extern osSemaphoreId_t crypto_sha_mutex;

/**
 * @brief Semaphore for GCM crypto operations.
 */
extern osSemaphoreId_t crypto_gcm_mutex;

/**
 * @brief Semaphore for ChaChaPoly crypto operations.
 */
extern osSemaphoreId_t crypto_chachapoly_mutex;

/**
 * @brief Semaphore for attestation crypto operations.
 */
extern osSemaphoreId_t crypto_attestation_mutex;

/**
 * @brief Semaphore for ECDH crypto operations.
 */
extern osSemaphoreId_t crypto_ecdh_mutex;

/**
 * @brief Semaphore for ECDSA crypto operations.
 */
extern osSemaphoreId_t crypto_ecdsa_mutex;

/**
 * @brief Semaphore for HMAC crypto operations.
 */
extern osSemaphoreId_t crypto_hmac_mutex;

/**
 * @brief Semaphore for TRNG crypto operations.
 */
extern osSemaphoreId_t crypto_trng_mutex;

/**
 * @brief Status of the semaphore operations.
 */
extern osStatus_t mutex_result;

/** @} */

/******************************************************
 *                Function Declarations
*******************************************************/
/**
 * @addtogroup CRYPTO_THREAD_FUNCTIONS
 * @{ 
 */

/***************************************************************************/
/**
 * @brief 
 *   To initialize the crypto semaphore for a particular crypto operation.
 * @param[in] mutex 
 *   The crypto semaphore to be initialized.
 * @return
 *   osSemaphoreId_t Semaphore ID.
******************************************************************************/
osSemaphoreId_t sl_si91x_crypto_threadsafety_init(osSemaphoreId_t *mutex);

/***************************************************************************/
/**
 * @brief 
 *   To acquire the crypto semaphore for a particular crypto operation.
 * @param[in] mutex 
 *   The crypto semaphore to be acquired.
 * @return
 *   osStatus_t Status of the semaphore.
******************************************************************************/
osStatus_t sl_si91x_crypto_mutex_acquire(osSemaphoreId_t *mutex);

/***************************************************************************/
/**
 * @brief 
 *   To release the crypto semaphore for a particular crypto operation.
 * @param[in] mutex 
 *   The crypto semaphore to be released.
 * @return
 *   osStatus_t Status of the semaphore.
******************************************************************************/
osStatus_t sl_si91x_crypto_mutex_release(osSemaphoreId_t *mutex);

/** @} */
