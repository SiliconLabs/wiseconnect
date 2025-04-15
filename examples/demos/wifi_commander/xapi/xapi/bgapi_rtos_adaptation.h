/***************************************************************************/ /**
 * @file bgapi_rtos_adaptation.h
 * @brief Internal interfaces for BGAPI RTOS adaptation
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef BGAPI_RTOS_ADAPTATION_H
#define BGAPI_RTOS_ADAPTATION_H

#include <stdbool.h>
#include "sl_bgapi.h"
#include "bgapi.h"

/***************************************************************************/ /**
 * @addtogroup sli_bgapi_rtos_adaptation
 * @{
 *
 * @brief BGAPI RTOS adaptation
 *
 * One of two mutually exclusive implementations of BGAPI RTOS adaptation are
 * selected at link time depending on the application configuration. If the
 * application build does not include an RTOS kernel and the Bluetooth RTOS
 * adaptation, a stub implementation from `bgapi_rtos_adaptation-stubs.c` is
 * used. The stub implementation does not allocate any synchronization objects
 * or perform any locking.
 *
 * If the application build includes an RTOS kernel, it shall also include the
 * Bluetooth RTOS adaptation that is built into the application. The RTOS
 * adaptation implementation will override the stubs with functions that
 * allocates RTOS synchronization objects and performs proper RTOS
 * synchronization.
 */
/** @} end sli_bt_rtos_adaptation */

/**
 * @brief Start the Bluetooth RTOS adaptation
 *
 * This special case is used for the sake of RTOS builds. It is called when the
 * application has requested starting the Bluetooth stack using @ref
 * sl_bt_system_start_bluetooth(). This function has two mutually exclusive
 * implementations depending on the application configuration:
 *
 * 1. When RTOS is included in the application build, this function is
 *    implemented by the real RTOS adaptation in `sl_bt_rtos_adaptation.c`. The
 *    implementation will allocate the necessary RTOS resources, start the
 *    threads, and finally call @ref sli_bt_system_start_bluetooth() in the
 *    Bluetooth thread context to start the stack.
 *
 * 2. When RTOS is not included in the application build, this function is
 *    implemented by the stub RTOS adaptation in
 *    `bgapi_rtos_adaptation-stubs.c`. The implementation will directly call
 *    @ref sli_bt_system_start_bluetooth() to start the stack.
 *
 * @return SL_STATUS_OK if the request to start the stack was successful,
 *   otherwise an error code. When RTOS is used, the starting will finish
 *   asynchronously. In case of error, the RTOS adaptation will notify the
 *   application with a system error event.
 */
sl_status_t sli_bt_start_rtos_adaptation();

/**
 * @brief Prepare for stopping the Bluetooth RTOS adaptation
 *
 * This special case is used for the sake of RTOS builds. It is called from
 * within the Bluetooth thread when handling the BGAPI command to stop the
 * Bluetooth stack. The BGAPI lock is therefore held already when the function
 * is called.
 *
 * When the user application issues the command to stop the stack, the RTOS
 * adaptation needs to remain operational until stopping the stack has finished.
 * The RTOS adaptation still needs to know that stopping is on-going so that it
 * can start to respond with an error if @ref sli_request_rtos_bluetooth_start()
 * is called while stopping is still on-going.
 *
 * When stopping the Bluetooth stack has finished and the RTOS adaptation can
 * freely delete the threads and other RTOS objects, the stack will call @ref
 * sli_bt_stop_rtos_adaptation.
 */
void sli_bt_prepare_to_stop_rtos_adaptation();

/**
 * @brief Stop the Bluetooth RTOS adaptation
 *
 * This special case is used for the sake of RTOS builds. It is called from
 * within `sl_bt_run()` in the Bluetooth thread when stopping the Bluetooth
 * stack has finished. The BGAPI lock is not held when the call is made.
 *
 * When the RTOS adaptation receives this call, it must signal the main loop in
 * the Bluetooth thread to let the loop know the stack has been stopped. This
 * function must then return gracefully back to the Bluetooth stack code to
 * finish the freeing of resources. After the stack has freed its resources, the
 * call to `sl_bt_run()` returns to the Bluetooth thread in the Bluetooth RTOS
 * adaptation. The loop in that thread will then see that the stack has been
 * stopped, it will stop processing further commands, and will gracefully
 * release all RTOS resources.
 */
void sli_bt_stop_rtos_adaptation();

#endif // BGAPI_RTOS_ADAPTATION_H
