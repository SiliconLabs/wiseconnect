/*******************************************************************************
* @file  sl_supp_event_loop_thread.c
* @brief This contains Supplicant event loop thread and events initialization.
*******************************************************************************
* # License
* <b>Copyright 2020-2024 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "cmsis_os2.h"
#include "sl_mgmt_if_thread.h"
#include "mgmt_if_header.h"
#include "mgmt_platform_if.h"
#include "os.h"
#include "sli_supp_event_loop_thread.h"

static void supp_event_loop_thread(void *argument);

sl_supp_eloop_thread_ctx_t *supp_eloop_thread_ctx = NULL;

static osThreadId_t supp_eloop_thread_id;

/***************************************************************************/ /**
 * @brief Initializes the Supplicant event loop thread and its resources.
 *
 * This function allocates memory for the Supplicant event loop thread 
 * context, creates event flags, and starts the thread. If any
 * part of the initialization fails, it performs cleanup and returns an
 * appropriate error status.
 * This function should only be called after wpa_supplicant_init()
 *
 * @return SL_STATUS_OK if the initialization is successful, otherwise an
 *         error status.
 ******************************************************************************/
sl_status_t sli_supp_eloop_thread_init()
{
  sl_supp_eloop_thread_ctx_t *ctx = (sl_supp_eloop_thread_ctx_t *)sl_malloc(sizeof(sl_supp_eloop_thread_ctx_t));
  if (ctx == NULL) {
    SL_DEBUG_LOG("\r\nFailed to allocate memory for management interface context\r\n");
    return SL_STATUS_ALLOCATION_FAILED;
  }

  sl_status_t status = SL_STATUS_OK;

  // Initialize event flags
  ctx->event_flags_id = osEventFlagsNew(NULL);
  if (ctx->event_flags_id == NULL) {
    SL_DEBUG_LOG("\r\nFailed to create event flags\r\n");
    status = SL_STATUS_FAIL;
    goto cleanup;
  } else {
    SL_DEBUG_LOG("\r\nEvent flags created successfully\r\n");
  }

  // Define and create the thread
  const osThreadAttr_t supp_eloop_thread_attributes = {
    .name       = "supp_event_loop_thread",
    .attr_bits  = 0,
    .cb_mem     = NULL,
    .cb_size    = 0,
    .stack_mem  = NULL,
    .stack_size = 3072,
    .priority   = osPriorityNormal,
    .tz_module  = 0,
    .reserved   = 0,
  };

  supp_eloop_thread_id = osThreadNew((osThreadFunc_t)supp_event_loop_thread, ctx, &supp_eloop_thread_attributes);
  if (supp_eloop_thread_id == NULL) {
    SL_DEBUG_LOG("\r\nFailed to create thread\r\n");
    status = SL_STATUS_FAIL;
    goto cleanup;
  } else {
    SL_DEBUG_LOG("\r\nthread created successfully\r\n");
  }

  supp_eloop_thread_ctx = ctx; // Set the global context only if initialization succeeds

  SL_DEBUG_LOG("\r\n Thread initialized successfully\r\n");
  return SL_STATUS_OK;

cleanup:
  // Cleanup in case of failure
  if (ctx->event_flags_id)
    osEventFlagsDelete(ctx->event_flags_id);

  sl_free(ctx);

  SL_DEBUG_LOG("\r\ninitialization failed with status: 0x%lX\r\n", status);
  return status;
}

/***************************************************************************/ /**
 * @brief Thread function to handle supplicant event loop thread events and run supplicant 
 *        event loop.
 *
 * This function runs in an infinite loop, waiting for and processing 
 * events based on flags set by the driver. The events include 
 * thread termination. 
 * It processes each event type accordingly and handles errors
 * that occur during event processing.
 *
 * @param argument Pointer to the mgmt interface context. This is cast to the
 *                 appropriate type within the function.
 ******************************************************************************/
static void supp_event_loop_thread(void *argument)
{
  sl_supp_eloop_thread_ctx_t *supp_loop_ctx = (sl_supp_eloop_thread_ctx_t *)argument;
  if (supp_loop_ctx == NULL) {
    return;
  }

  while (1) {
    // Wait for any of the events to occur
    uint32_t pending_events =
      osEventFlagsWait(supp_loop_ctx->event_flags_id, SLI_SUPP_LOOP_EVENTS, osFlagsWaitAny, osWaitForever);
    // Process events
    if (pending_events & SLI_EVENT_SUPPLICANT_LOOP_TRIGGER) {
      trigger_eloop_run();
    }

    if (pending_events & SLI_EVENT_SUPPLICANT_LOOP_SIGNAL_THREAD_TERMINATE) {
      if (sli_clear_eloop_thread_terminate_event() != SL_STATUS_OK) {
        SL_MGMT_ASSERT(0);
      }

      if (sli_notify_eloop_thread_ready_to_terminate() != SL_STATUS_OK) {
        SL_MGMT_ASSERT(0);
      }
      break;
    }
  }

  // To keep task running until terminate is received.
  while (1) {
    OS_DELAY(1000);
  }
}

/***************************************************************************/ /**
 * @brief Deinitialize the Supplicant event loop thread and free all allocated resources.
 *
 * @return sl_status_t Returns SL_STATUS_OK on success or appropriate error code.
 ******************************************************************************/
sl_status_t sli_supp_eloop_thread_deinit()
{
  // Check if the driver context is NULL
  if (supp_eloop_thread_ctx == NULL) {
    SL_DEBUG_LOG("[ERROR] supp thread ctx is NULL, cannot deinitialize.\r\n");
    return SL_STATUS_NULL_POINTER;
  }

  sl_status_t status = SL_STATUS_OK;

  // Set the event flag to stop the Eloop thread
  SL_DEBUG_LOG("[INFO] Signaling Supplicant Eloop thread to stop...\r\n");
  osEventFlagsSet(supp_eloop_thread_ctx->event_flags_id, SLI_EVENT_SUPPLICANT_LOOP_SIGNAL_THREAD_TERMINATE);

  // Wait for the Eloop thread to exit
  SL_DEBUG_LOG("[INFO] Waiting for Supplicant Eloop thread to exit...\r\n");
  uint32_t result = osEventFlagsWait(supp_eloop_thread_ctx->event_flags_id,
                                     SLI_EVENT_SUPPLICANT_LOOP_NOTIFY_READY_TO_TERMINATE,
                                     osFlagsWaitAny,
                                     osWaitForever);

  // Handle event flags wait error
  if (result == (uint32_t)osErrorTimeout || result == (uint32_t)osErrorResource) {
    SL_DEBUG_LOG("[ERROR] Failed to wait for Supplicant Eloop thread exit: 0x%lX\r\n", result);
    return SL_STATUS_FAIL;
  }

  // Terminate the Supplicant event loop  thread
  SL_DEBUG_LOG("[INFO] Terminating SUPP LOOP thread...\r\n");
  osThreadTerminate(supp_eloop_thread_id);

  // Check if the thread was terminated successfully
  osThreadState_t state = osThreadGetState(supp_eloop_thread_id);
  if (state == osThreadTerminated) {
    SL_DEBUG_LOG("[INFO] SUPP LOOP  thread terminated successfully.\r\n");
  } else {
    SL_DEBUG_LOG("[ERROR] SUPP LOOP  thread termination failed. Current state: %d\r\n", state);
    SL_MGMT_ASSERT(0);
  }

  // Delete the event flags
  SL_DEBUG_LOG("[INFO] Deleting SUPP LOOP event flags...\r\n");
  osEventFlagsDelete(supp_eloop_thread_ctx->event_flags_id);

  // Free the SUPP LOOP context memory
  SL_DEBUG_LOG("[INFO] Freeing SUPP LOOP context memory...\r\n");
  sl_free(supp_eloop_thread_ctx);
  supp_eloop_thread_ctx = NULL;

  SL_DEBUG_LOG("[INFO] SUPP LOOP Thread deinitialized successfully.\r\n");
  return SL_STATUS_OK;
}
