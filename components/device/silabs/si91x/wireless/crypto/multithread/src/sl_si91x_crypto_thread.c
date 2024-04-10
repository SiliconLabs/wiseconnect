/*******************************************************************************
 * @file  sl_si91x_crypto_thread.c
 * @brief SLI SI91X Crypto thread safety file
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
#include <string.h>
#include "cmsis_os2.h"
#include "sl_net.h"
#include "sl_wifi.h"
#include "sl_status.h"
#include "sl_si91x_crypto_thread.h"

osSemaphoreId_t crypto_aes_mutex         = 0;
osSemaphoreId_t crypto_ccm_mutex         = 0;
osSemaphoreId_t crypto_sha_mutex         = 0;
osSemaphoreId_t crypto_gcm_mutex         = 0;
osSemaphoreId_t crypto_chachapoly_mutex  = 0;
osSemaphoreId_t crypto_attestation_mutex = 0;
osSemaphoreId_t crypto_ecdh_mutex        = 0;
osSemaphoreId_t crypto_ecdsa_mutex       = 0;
osSemaphoreId_t crypto_hmac_mutex        = 0;
osSemaphoreId_t crypto_trng_mutex        = 0;
osStatus_t mutex_result                  = osOK;

osSemaphoreId_t sl_si91x_crypto_threadsafety_init(osSemaphoreId_t *mutex)
{
  if (mutex == NULL) {

    osSemaphoreAttr_t sl_osSemaphoreAttr_t;

    //Set Semaphore flags attributes
    sl_osSemaphoreAttr_t.name      = "crypto_mutex";
    sl_osSemaphoreAttr_t.attr_bits = 0U;
    sl_osSemaphoreAttr_t.cb_mem    = NULL;
    sl_osSemaphoreAttr_t.cb_size   = 0U;

    mutex = osSemaphoreNew(1U, 1U, &sl_osSemaphoreAttr_t);

    osSemaphoreRelease(mutex);
  }
  return mutex;
}

osStatus_t sl_si91x_crypto_mutex_acquire(osSemaphoreId_t *mutex)
{
  mutex_result = osSemaphoreAcquire(mutex, osWaitForever);
  if (mutex_result != osOK) {
    printf("Mutex acquire fail : %d \r\n", mutex_result);
  }
  return mutex_result;
}

osStatus_t sl_si91x_crypto_mutex_release(osSemaphoreId_t *mutex)
{
  mutex_result = osSemaphoreRelease(mutex);
  if (mutex_result != osOK) {
    printf("Mutex release fail : %d \r\n", mutex_result);
  }
  return mutex_result;
}
