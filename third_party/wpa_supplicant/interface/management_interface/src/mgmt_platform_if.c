/*******************************************************************************
* @file  mgmt_platform_if.h
* @brief This file acts as interface between management interface and
*        platform interface
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

#include "sl_wlan_data_types.h"
#include "sli_supp_event_loop_thread.h"
#include "mgmt_platform_if.h"

/***************************************************************************/ /**
 * @brief  Set trigger eloop event to supplicant event loop thread.
 *
 * @param void.
 * @return sl_status_t Status of the operation. Returns SL_STATUS_OK on success,
 *                     or an appropriate error code on failure.
 ******************************************************************************/
sl_status_t sli_set_event_supplicant_eloop()
{
  uint32_t os_status = osOK;

  // Set event flag
  os_status = osEventFlagsSet(supp_eloop_thread_ctx->event_flags_id, SLI_EVENT_SUPPLICANT_LOOP_TRIGGER);
  if (os_status & osFlagsError) {
    SL_DEBUG_LOG("\r\nFailed to set event flag: 0x%lX\r\n", os_status);
    return SL_STATUS_FAIL;
  }
  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 * @brief  Clears trigger eloop event to supplicant event loop thread.
 *
 * @param void.
 * @return sl_status_t Status of the operation. Returns SL_STATUS_OK on success,
 *                     or an appropriate error code on failure.
 ******************************************************************************/
sl_status_t sli_clear_event_supplicant_eloop()
{
  uint32_t os_status = osOK;

  // clear event flag
  os_status = osEventFlagsClear(supp_eloop_thread_ctx->event_flags_id, SLI_EVENT_SUPPLICANT_LOOP_TRIGGER);
  if (os_status & osFlagsError) {
    SL_DEBUG_LOG("\r\nFailed to clear event flag: 0x%lX\r\n", os_status);
    return SL_STATUS_FAIL;
  }
  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 * @brief  Set event to signal that thread is ready to be terminated.
 *
 * @param void.
 * @return sl_status_t Status of the operation. Returns SL_STATUS_OK on success,
 *                     or an appropriate error code on failure.
 ******************************************************************************/
sl_status_t sli_notify_eloop_thread_ready_to_terminate()
{
  uint32_t os_status =
    osEventFlagsSet(supp_eloop_thread_ctx->event_flags_id, SLI_EVENT_SUPPLICANT_LOOP_NOTIFY_READY_TO_TERMINATE);

  if (os_status & osFlagsError) {
    SL_DEBUG_LOG("\r\nFailed to set event flag: 0x%lX\r\n", os_status);
    return SL_STATUS_FAIL;
  }
  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 * @brief  Clear eloop-thread-stop event flag.
 *
 * @param void.
 * @return sl_status_t Status of the operation. Returns SL_STATUS_OK on success,
 *                     or an appropriate error code on failure.
 ******************************************************************************/
sl_status_t sli_clear_eloop_thread_terminate_event(void)
{
  uint32_t os_status =
    osEventFlagsClear(supp_eloop_thread_ctx->event_flags_id, SLI_EVENT_SUPPLICANT_LOOP_SIGNAL_THREAD_TERMINATE);

  if (os_status & osFlagsError) {
    SL_DEBUG_LOG("\r\nFailed to set event flag: 0x%lX\r\n", os_status);
    return SL_STATUS_FAIL;
  }
  return SL_STATUS_OK;
}
