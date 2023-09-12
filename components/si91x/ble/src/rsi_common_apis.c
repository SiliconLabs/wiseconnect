/*******************************************************************************
* @file  rsi_common_apis.c
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

#include "rsi_common.h"
#include "rsi_timer.h"

#include "rsi_ble.h"

#include "sl_si91x_status.h"

/*
  Global Variables
 * */
rsi_driver_cb_t *rsi_driver_cb = NULL;

int32_t rsi_driver_memory_estimate(void);

rsi_driver_cb_non_rom_t *rsi_driver_cb_non_rom = NULL;

/** @addtogroup COMMON 
* @{
*/
/*==============================================*/
/**
 *
 * @brief      Provide the memory required by the application. This is a non-blocking API.
 * @param[in]  Void
 * @return     Driver pool size
 *
 */

int32_t rsi_driver_memory_estimate(void)
{
  uint32_t actual_length = 0;

  // Calculate the Memory length of the application
  actual_length += RSI_DRIVER_POOL_SIZE;
  return actual_length;
}

/*==============================================*/
/**
 *
 * @brief      Initialize WiSeConnect ble driver.  This is a non-blocking API.
 *             Designate memory to all driver components from the buffer provided by the application.
 * @param[in]  buffer      -    Pointer to buffer from application. \n Driver uses this buffer to hold driver control for its operation.
 * @param[in]  length      -    Length of the buffer.
 * @return     **Success** -    Returns the memory used, which is less than or equal to buffer length provided. \n
 *             **Failure** -    Non-Zero values\n
 *
 *             			**RSI_ERROR_TIMEOUT**         -    If UART initialization fails in SPI / UART mode   \n
 */

/** @} */
uint8_t *buffer_addr = NULL;
int32_t rsi_ble_driver_init(uint8_t *buffer, uint32_t length)
{
  uint32_t actual_length = 0;

  // If (((uint32_t)buffer & 3) != 0)
  if (((uintptr_t)buffer & 3) != 0) // To avoid compiler warning, replace uint32_t with uintptr_t
  {
    // Making buffer 4 byte aligned
    // Length -= (4 - ((uint32_t)buffer & 3));
    // To avoid compiler warning, replace uint32_t with uintptr_t
    length -= (4 - ((uintptr_t)buffer & 3));
    // Buffer = (uint8_t *)(((uint32_t)buffer + 3) & ~3);// To avoid compiler warning, replace uint32_t with uintptr_t
    buffer = (uint8_t *)(((uintptr_t)buffer + 3) & ~3);
  }

  // Memset user buffer
  memset(buffer, 0, length);

  actual_length += rsi_driver_memory_estimate();
  // If length is not sufficient
  if (length < actual_length) {
    return actual_length;
  }
  buffer_addr = buffer;

  // Store length minus any alignment bytes to first 32-bit address in buffer.
  *(uint32_t *)buffer = length;
  buffer += sizeof(uint32_t);

  // Designate memory for driver cb
  rsi_driver_cb = (rsi_driver_cb_t *)buffer;
  buffer += sizeof(rsi_driver_cb_t);

#ifdef RSI_LITTLE_ENDIAN
  rsi_driver_cb->endian = IS_LITTLE_ENDIAN;
#else
  rsi_driver_cb->endian = IS_BIG_ENDIAN;
#endif

  // Designate memory for common_cb
  rsi_driver_cb_non_rom = (rsi_driver_cb_non_rom_t *)buffer;
  buffer += sizeof(rsi_driver_cb_non_rom_t);

#if defined(RSI_DEBUG_PRINTS) || defined(FW_LOGGING_ENABLE)
  // Creates debug prints mutex
  rsi_driver_cb_non_rom->debug_prints_mutex = osMutexNew(NULL);
#endif

  // Designate memory for bt_common_cb
  rsi_driver_cb->bt_common_cb = (rsi_bt_cb_t *)buffer;
  buffer += ((sizeof(rsi_bt_cb_t) + 3) & ~3);

  // Initialize bt_common_cb
  rsi_bt_cb_init(rsi_driver_cb->bt_common_cb, RSI_PROTO_BT_COMMON);

  // Designate memory for ble_cb
  rsi_driver_cb->ble_cb = (rsi_bt_cb_t *)buffer;
  buffer += ((sizeof(rsi_bt_cb_t) + 3) & ~3);

  // Initialize ble_cb
  rsi_bt_cb_init(rsi_driver_cb->ble_cb, RSI_PROTO_BLE);

  // Designate memory for bt_common_cb
  rsi_driver_cb->bt_global_cb = (rsi_bt_global_cb_t *)buffer;
  buffer += sizeof(rsi_bt_global_cb_t);

  // Fill in bt_global_cb
  buffer += rsi_bt_global_cb_init(rsi_driver_cb, buffer);

  if (length < (uint32_t)(buffer - buffer_addr)) {
    SL_PRINTF(SL_DRIVER_INIT_INSUFFICIENT_BUFFER_2, COMMON, LOG_ERROR, "length: %4x", (uint32_t)(buffer - buffer_addr));
    return buffer - buffer_addr;
  }

  // Update state
  rsi_driver_cb_non_rom->device_state = RSI_DRIVER_INIT_DONE;

  SL_PRINTF(SL_DRIVER_INIT_EXIT, COMMON, LOG_INFO, "actual_length=%4x", actual_length);
  return actual_length;
}

//======================================================
/**
 *
 * @brief       De-Initialize driver components. Clear all the memory given for driver operations in \ref rsi_ble_driver_init() API.
 * In OS case,  User need to take care of OS variables initialized in \ref rsi_ble_driver_init(). This is a non-blocking API.
 * This API must be called by the thread/task/Master thread that it is not dependent on.
 * OS variables allocated/initialized in \ref rsi_ble_driver_init() API.
 * @pre 		Need to call after the driver initialization
 * @param[in]   Void
 * @return      0              - Success \n
 *              Non-Zero Value - Failure
 */

int32_t rsi_ble_driver_deinit(void)
{
  SL_PRINTF(SL_DRIVER_DEINIT_ENTRY, COMMON, LOG_INFO);
  uint32_t actual_length = 0;

  if ((rsi_driver_cb_non_rom->device_state < RSI_DRIVER_INIT_DONE)) {
    // Command given in wrong state
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  // Check if buffer is enough for driver components
  actual_length += rsi_driver_memory_estimate();
  if (buffer_addr == NULL) {
    return RSI_FAILURE;
  }

  if (rsi_driver_cb_non_rom->tx_mutex) {
    osMutexDelete(rsi_driver_cb_non_rom->tx_mutex);
  }

#ifdef FW_LOGGING_ENABLE
  sl_fw_log_deinit();
#endif
#if defined(RSI_DEBUG_PRINTS) || defined(FW_LOGGING_ENABLE)
  if (rsi_driver_cb_non_rom->debug_prints_mutex) {
    osMutexDelete(rsi_driver_cb_non_rom->debug_prints_mutex);
  }
#endif

  if (rsi_driver_cb_non_rom->common_cmd_send_sem) {
    osSemaphoreDelete(rsi_driver_cb_non_rom->common_cmd_send_sem);
  }

  if (rsi_driver_cb_non_rom->common_cmd_sem) {
    osSemaphoreDelete(rsi_driver_cb_non_rom->common_cmd_sem);
  }

  // Create BT semaphore
  if (rsi_driver_cb->bt_common_cb->bt_cmd_sem) {
    osSemaphoreDelete(rsi_driver_cb->bt_common_cb->bt_cmd_sem);
  }

  if (rsi_driver_cb->bt_common_cb->bt_sem) {
    osSemaphoreDelete(rsi_driver_cb->bt_common_cb->bt_sem);
  }

  if (rsi_driver_cb->ble_cb->bt_cmd_sem) {
    osSemaphoreDelete(rsi_driver_cb->ble_cb->bt_cmd_sem);
  }

  if (rsi_driver_cb->ble_cb->bt_sem) {
    osSemaphoreDelete(rsi_driver_cb->ble_cb->bt_sem);
  }
  //  rsi_vport_exit_critical();

  rsi_driver_cb_non_rom->device_state = RSI_DEVICE_STATE_NONE;
  SL_PRINTF(SL_DRIVER_DEINIT_SEMAPHORE_DESTROY_FAILED_26, COMMON, LOG_INFO);
  return RSI_SUCCESS;
}

/** @} */
