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

extern osSemaphoreId_t crypto_aes_mutex;
extern osSemaphoreId_t crypto_ccm_mutex;
extern osSemaphoreId_t crypto_sha_mutex;
extern osSemaphoreId_t crypto_gcm_mutex;
extern osSemaphoreId_t crypto_chachapoly_mutex;
extern osSemaphoreId_t crypto_attestation_mutex;
extern osSemaphoreId_t crypto_ecdh_mutex;
extern osSemaphoreId_t crypto_hmac_mutex;
extern osSemaphoreId_t crypto_trng_mutex;
extern osStatus_t mutex_result;

/******************************************************
 *                Function Declarations
*******************************************************/

osSemaphoreId_t sl_si91x_crypto_threadsafety_init(osSemaphoreId_t *mutex);
osStatus_t sl_si91x_crypto_mutex_acquire(osSemaphoreId_t *mutex);
osStatus_t sl_si91x_crypto_mutex_release(osSemaphoreId_t *mutex);
