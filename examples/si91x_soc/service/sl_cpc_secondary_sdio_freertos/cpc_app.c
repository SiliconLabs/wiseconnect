/***************************************************************************/ /**
 * @file cpc_app.c
 * @brief Secondary firmware for the CPC sample application
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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
#include <stdio.h>
#include <string.h>

#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "rsi_os.h"

#include "sl_assert.h"
#include "sl_constants.h"
#include "sl_cpc.h"

#include "sl_cpc_drv_secondary_sdio_config.h"
#include "cpc_app.h"
#include "rsi_debug.h"

/*******************************************************************************
 *******************************  DEFINES   ************************************
 ******************************************************************************/
#ifndef THREAD_STACK_SIZE
#define THREAD_STACK_SIZE 128 // in words
#endif

#ifndef THREAD_PRIO
#define THREAD_PRIO osPriorityLow
#endif

#define ENDPOINT_0_CONNECTED    0x1
#define ENDPOINT_1_CONNECTED    0x2
#define ENDPOINT_0_DISCONNECTED 0X4
#define ENDPOINT_1_DISCONNECTED 0x8

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/
static uint8_t static_write_array[SL_CPC_RX_PAYLOAD_MAX_LENGTH];
static sl_cpc_endpoint_handle_t user_endpoint_handle_0;
static sl_cpc_endpoint_handle_t user_endpoint_handle_1;

const osThreadAttr_t cpc_app_thread_endpoint_0_attributes = {
  .name       = "cpc thread endpoint 0", // Name of thread
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = (THREAD_STACK_SIZE * 4), // Stack size of cpc_thread_endpoint_0
  .priority   = THREAD_PRIO,             // Priority of Sensor task
  .tz_module  = 0,
  .reserved   = 0,
};

const osThreadAttr_t cpc_app_thread_endpoint_1_attributes = {
  .name       = "cpc thread endpoint 1", // Name of thread
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = (THREAD_STACK_SIZE * 4), // Stack size of cpc_thread_endpoint_1
  .priority   = THREAD_PRIO,             // Priority of cpc_thread_endpoint_1 task
  .tz_module  = 0,
  .reserved   = 0,
};

const osThreadAttr_t cpc_app_thread_connections_monitor_attributes = {
  .name       = "cpc thread connection monitor", // Name of thread
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = (THREAD_STACK_SIZE * 4), // Stack size of cpc_thread_endpoint_1
  .priority   = THREAD_PRIO,             // Priority of cpc_thread_endpoint_1 task
  .tz_module  = 0,
  .reserved   = 0,
};

const osEventFlagsAttr_t endpoint_connected_flags_attributes = {
  .name      = "endpoint connected flags",
  .attr_bits = 0,
  .cb_mem    = NULL,
  .cb_size   = 0,
};

const osThreadAttr_t thread_attributes = {
  .name       = "app",
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = 128,
  .priority   = osPriorityLow,
  .tz_module  = 0,
  .reserved   = 0,
};

static osThreadId_t cpc_app_ep_0_thread_id;
static osThreadId_t cpc_app_ep_1_thread_id;
static osThreadId_t cpc_app_thread_connections_monitor_thread_id;

osSemaphoreId_t tx_complete_semaphore;
osSemaphoreAttr_t tx_complete_semaphore_attr_st;

osEventFlagsId_t endpoint_connected_flags_id;

/*******************************************************************************
 *************************  LOCAL FUNCTIONS   **********************************
 ******************************************************************************/

// Callback triggered on error
static void on_endpoint_error(uint8_t endpoint_id, void *arg)
{
  (void)arg;
  uint32_t flags;

  if ((endpoint_id == SL_CPC_ENDPOINT_USER_ID_0)
      && (sl_cpc_get_endpoint_state(&user_endpoint_handle_0) == SL_CPC_STATE_ERROR_DESTINATION_UNREACHABLE)) {
    // clear connected flag
    flags = osEventFlagsClear(endpoint_connected_flags_id, ENDPOINT_0_CONNECTED);
    if (flags == osFlagsError) {
      DEBUGOUT("Error Clearing for flags: %ld\n", flags);
      SL_ASSERT(false);
    }
    flags = osEventFlagsSet(endpoint_connected_flags_id, ENDPOINT_0_DISCONNECTED);
    if (flags == osFlagsError) {
      DEBUGOUT("Error Setting for flags: %ld\n", flags);
      SL_ASSERT(false);
    }

  } else if ((endpoint_id == SL_CPC_ENDPOINT_USER_ID_1)
             && (sl_cpc_get_endpoint_state(&user_endpoint_handle_1) == SL_CPC_STATE_ERROR_DESTINATION_UNREACHABLE)) {
    // clear connected flag
    flags = osEventFlagsClear(endpoint_connected_flags_id, ENDPOINT_1_CONNECTED);
    if (flags == osFlagsError) {
      DEBUGOUT("Error Clearing for flags: %ld\n", flags);
      SL_ASSERT(false);
    }
    flags = osEventFlagsSet(endpoint_connected_flags_id, ENDPOINT_1_DISCONNECTED);
    if (flags == osFlagsError) {
      DEBUGOUT("Error Setting for flags: %ld\n", flags);
      SL_ASSERT(false);
    }
  } else {
    SL_ASSERT(0);
  }
}

// Callback triggered on write complete
static void on_write_completed(sl_cpc_user_endpoint_id_t endpoint_id, void *buffer, void *arg, sl_status_t status)
{
  (void)arg;
  (void)buffer;
  osStatus_t sem;

  SL_ASSERT(status == SL_STATUS_OK);

  if (endpoint_id == SL_CPC_ENDPOINT_USER_ID_0) {
    // user endpoint 0 uses a dynamically allocated buffer
    // which can be safely freed in the on_write_completed callback.
    free(buffer);
  } else if (endpoint_id == SL_CPC_ENDPOINT_USER_ID_1) {
    // user endpoint 1 uses a static buffer, so a semaphore is used
    // to ensure that the buffer is available for the next write
    sem = osSemaphoreRelease(tx_complete_semaphore);
    if (sem != osOK) {
      DEBUGOUT("Failed to release Semaphore\n");
      SL_ASSERT(false);
    }
  } else {
    SL_ASSERT(0);
  }
}

/***************************************************************************/ /**
 * Open and configure endpoint
 ******************************************************************************/
static void open_endpoint(sl_cpc_endpoint_handle_t *ep, uint8_t endpoint_id)
{
  sl_status_t status;

  status = sl_cpc_open_user_endpoint(ep, endpoint_id, 0, 1);
  SL_ASSERT(status == SL_STATUS_OK);

  // the same callback can be used for multiple endpoints, as the endpoint_id is passed
  // to it
  status = sl_cpc_set_endpoint_option(ep, SL_CPC_ENDPOINT_ON_IFRAME_WRITE_COMPLETED, (void *)on_write_completed);
  SL_ASSERT(status == SL_STATUS_OK);

  status = sl_cpc_set_endpoint_option(ep, SL_CPC_ENDPOINT_ON_ERROR, (void *)on_endpoint_error);
  SL_ASSERT(status == SL_STATUS_OK);
}

/***************************************************************************/ /**
 * CPC task for user endpoint 0
 ******************************************************************************/
static void cpc_app_task_endpoint_0(void *arg)
{
  sl_status_t status;
  void *read_array;
  uint16_t size;
  uint8_t *dynamic_write_array;
  uint32_t flags;
  (void)&arg;

  while (1) {
    flags = osEventFlagsWait(endpoint_connected_flags_id,
                             ENDPOINT_0_CONNECTED,
                             osFlagsWaitAny | osFlagsNoClear,
                             osWaitForever);
    if (flags == osFlagsError) {
      DEBUGOUT("Error waiting for flags: %ld\n", flags);
      SL_ASSERT(false);
    }

    if (flags & ENDPOINT_0_CONNECTED) {
      // read is blocking
      status = sl_cpc_read(&user_endpoint_handle_0, &read_array, &size, 0, 0u);
      if (size > 0) {
        // allocate a buffer which will be freed in the on_write_completed callback
        dynamic_write_array = (uint8_t *)malloc(size * sizeof(uint8_t));

        SL_ASSERT(status == SL_STATUS_OK);
        memcpy(dynamic_write_array, read_array, size);
        // return the buffer to CPC
        sl_cpc_free_rx_buffer(read_array);
        // echo the data back to the host. Note that with security enabled, the write buffer
        // will be modified.
        status = sl_cpc_write(&user_endpoint_handle_0, dynamic_write_array, size, 0u, NULL);
        SL_ASSERT(status == SL_STATUS_OK);
      }
    }
  }
}

/***************************************************************************/ /**
 * CPC task for user endpoint 1
 ******************************************************************************/
static void cpc_app_task_endpoint_1(void *arg)
{
  sl_status_t status;
  void *read_array;
  uint16_t size;
  uint32_t flags;
  (void)&arg;

  while (1) {
    flags = osEventFlagsWait(endpoint_connected_flags_id,
                             ENDPOINT_1_CONNECTED,
                             osFlagsWaitAny | osFlagsNoClear,
                             osWaitForever);
    if (flags == osFlagsError) {
      DEBUGOUT("Error waiting for flags: %ld\n", flags);
      SL_ASSERT(false);
    }

    if (flags & ENDPOINT_1_CONNECTED) {
      // read is blocking
      status = sl_cpc_read(&user_endpoint_handle_1, &read_array, &size, 0u, 0u);
      if (size > 0) {
        SL_ASSERT(size <= sizeof(static_write_array));
        memcpy(static_write_array, read_array, size);
        // return the buffer to CPC
        sl_cpc_free_rx_buffer(read_array);
        // echo the data back to the host. Note that with security enabled, the write buffer
        // will be modified.
        status = sl_cpc_write(&user_endpoint_handle_1, static_write_array, size, 0, NULL);
        SL_ASSERT(status == SL_STATUS_OK);
        // wait for tx to complete
        osStatus_t sem_status = osSemaphoreAcquire(tx_complete_semaphore, osWaitForever);
        if (sem_status != osOK) {
          DEBUGOUT("Failed to acquire tx_complete Semaphore\n");
        }
      }
    }
  }
}

/***************************************************************************/ /**
 * CPC task for monitoring connections
 ******************************************************************************/
static void cpc_app_task_connections(void *arg)
{
  uint32_t flags;
  sl_status_t status;
  osStatus_t delay_status = osError;
  (void)&arg;

  // Open user endpoints and set flags
  open_endpoint(&user_endpoint_handle_0, SL_CPC_ENDPOINT_USER_ID_0);
  // set connected flag
  flags = osEventFlagsSet(endpoint_connected_flags_id, ENDPOINT_0_CONNECTED);
  if (flags == osFlagsError) {
    DEBUGOUT("Error Setting for flags: %ld\n", flags);
    SL_ASSERT(false);
  }

  flags = osEventFlagsClear(endpoint_connected_flags_id, ENDPOINT_0_DISCONNECTED);
  if (flags == osFlagsError) {
    DEBUGOUT("Error Clearing for flags: %ld\n", flags);
    SL_ASSERT(false);
  }

  DEBUGOUT("EP0 Connected!\n");

  open_endpoint(&user_endpoint_handle_1, SL_CPC_ENDPOINT_USER_ID_1);
  // set connected flag
  flags = osEventFlagsSet(endpoint_connected_flags_id, ENDPOINT_1_CONNECTED);
  if (flags == osFlagsError) {
    DEBUGOUT("Error Setting for flags: %ld\n", flags);
    SL_ASSERT(false);
  }

  flags = osEventFlagsClear(endpoint_connected_flags_id, ENDPOINT_1_DISCONNECTED);
  if (flags == osFlagsError) {
    DEBUGOUT("Error Clearing for flags: %ld\n", flags);
    SL_ASSERT(false);
  }
  DEBUGOUT("EP1 Connected!\n");
  // enter the main loop of the task
  while (1) {
    // Pend on the endpoint connected flag group. If any flag is cleared,
    // the corresponding endpoint is in error and the endpoint
    // must be reopened
    flags = osEventFlagsWait(endpoint_connected_flags_id,
                             ENDPOINT_0_DISCONNECTED | ENDPOINT_1_DISCONNECTED,
                             osFlagsWaitAny | osFlagsNoClear,
                             osWaitForever);

    if (flags == osFlagsError) {
      DEBUGOUT("Error waiting for flags: %ld\n", flags);
      SL_ASSERT(false);
    }

    if (flags & ENDPOINT_0_DISCONNECTED) {
      // endpoint 0 has disconnected
      DEBUGOUT("EP0 Disconnected!\n");
      status = sl_cpc_close_endpoint(&user_endpoint_handle_0);
      // status will be SL_STATUS_BUSY if the endpoint is in the process of closing
      SL_ASSERT(status == SL_STATUS_OK || status == SL_STATUS_BUSY);

      if (sl_cpc_get_endpoint_state(&user_endpoint_handle_0) == SL_CPC_STATE_FREED) {
        // User endpoint ids are defined in sl_cpc.h
        open_endpoint(&user_endpoint_handle_0, SL_CPC_ENDPOINT_USER_ID_0);
        // set connected flag
        flags = osEventFlagsSet(endpoint_connected_flags_id, ENDPOINT_0_CONNECTED);
        if (flags == osFlagsError) {
          DEBUGOUT("Error Setting for flags: %ld\n", flags);
          SL_ASSERT(false);
        }
        flags = osEventFlagsClear(endpoint_connected_flags_id, ENDPOINT_0_DISCONNECTED);
        if (flags == osFlagsError) {
          DEBUGOUT("Error Clearing for flags: %ld\n", flags);
          SL_ASSERT(false);
        }
        DEBUGOUT("EP0 Connected!\n");
      } else {
        // allow core thread to free endpoint
        if (delay_status != osOK) {
          DEBUGOUT("Failed to delay\n");
        }
      }
    }

    if (flags & ENDPOINT_1_DISCONNECTED) {
      // endpoint 1 has disconnected
      DEBUGOUT("EP1 Disconnected!\n");
      status = sl_cpc_close_endpoint(&user_endpoint_handle_1);
      // status will be SL_STATUS_BUSY if the endpoint is in the process of closing
      SL_ASSERT(status == SL_STATUS_OK || status == SL_STATUS_BUSY);

      if (sl_cpc_get_endpoint_state(&user_endpoint_handle_1) == SL_CPC_STATE_FREED) {
        // User endpoint ids are defined in sl_cpc.h
        open_endpoint(&user_endpoint_handle_1, SL_CPC_ENDPOINT_USER_ID_1);
        // set connected flag
        flags = osEventFlagsSet(endpoint_connected_flags_id, ENDPOINT_1_CONNECTED);
        if (flags == osFlagsError) {
          DEBUGOUT("Error Setting for flags: %ld\n", flags);
          SL_ASSERT(false);
        }
        flags = osEventFlagsClear(endpoint_connected_flags_id, ENDPOINT_1_DISCONNECTED);
        if (flags == osFlagsError) {
          DEBUGOUT("Error Clearing for flags: %ld\n", flags);
          SL_ASSERT(false);
        }
        DEBUGOUT("EP1 Connected!\n");
      } else {
        // allow core thread to free endpoint
        delay_status = osDelay(1);
        if (delay_status != osOK) {
          DEBUGOUT("Failed to delay\n");
        }
      }
    }
  }
}
/*******************************************************************************
 ************************  GLOBAL FUNCTIONS   **********************************
 ******************************************************************************/

/***************************************************************************/ /**
 * Initialize CPC application
 ******************************************************************************/
void cpc_app_init(void)
{
  uint32_t flags;

  // Create semaphore for transmission complete
  tx_complete_semaphore_attr_st.attr_bits = 0U;
  tx_complete_semaphore_attr_st.cb_mem    = NULL;
  tx_complete_semaphore_attr_st.cb_size   = 0U;
  tx_complete_semaphore_attr_st.name      = NULL;
  tx_complete_semaphore                   = osSemaphoreNew(1U, 0U, &tx_complete_semaphore_attr_st);
  if (tx_complete_semaphore == 0) {
    SL_ASSERT(false);
  }

// Create flag group for endpoint disconnect events
// Define event flags for each endpoint connection (assuming 8 endpoints)
#define ENDPOINT_CONNECTED_FLAGS (1 << 8) - 1

  // Create event flags object with all flags initially cleared
  endpoint_connected_flags_id = osEventFlagsNew(&endpoint_connected_flags_attributes);

  if (endpoint_connected_flags_id == NULL) {
    // Error handling - Event flags creation failed
    SL_ASSERT(false);
  }

  // Set the ENDPOINT_*_DISCONNECTED flags
  flags = osEventFlagsSet(endpoint_connected_flags_id, ENDPOINT_0_DISCONNECTED);
  if (flags == osFlagsError) {
    DEBUGOUT("Error waiting for flags: %ld\n", flags);
  }
  flags = osEventFlagsSet(endpoint_connected_flags_id, ENDPOINT_1_DISCONNECTED);
  if (flags == osFlagsError) {
    DEBUGOUT("Error waiting for flags: %ld\n", flags);
  }

  // Create Threads
  cpc_app_ep_0_thread_id =
    osThreadNew((osThreadFunc_t)cpc_app_task_endpoint_0, NULL, &cpc_app_thread_endpoint_0_attributes);
  if (cpc_app_ep_0_thread_id == 0) {
    SL_ASSERT(false);
  }

  cpc_app_ep_1_thread_id =
    osThreadNew((osThreadFunc_t)cpc_app_task_endpoint_1, NULL, &cpc_app_thread_endpoint_1_attributes);
  if (cpc_app_ep_1_thread_id == 0) {
    SL_ASSERT(false);
  }

  cpc_app_thread_connections_monitor_thread_id =
    osThreadNew((osThreadFunc_t)cpc_app_task_connections, NULL, &cpc_app_thread_connections_monitor_attributes);
  if (cpc_app_thread_connections_monitor_thread_id == 0) {
    SL_ASSERT(false);
  }

  osThreadNew((osThreadFunc_t)cpc_app_process_action, NULL, &thread_attributes);
}

void cpc_app_process_action()
{
  while (1) {
  }
}
