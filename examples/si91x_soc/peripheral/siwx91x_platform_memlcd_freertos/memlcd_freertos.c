/***************************************************************************/ /**
 * @file memlcd_freertos.c
 * @brief MEMLCD under FreeRTOS: one task initializes the display, then in a
 *        loop uses osDelay() and refreshes a line with the kernel tick count.
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 ******************************************************************************/
#include "cmsis_os2.h"
#include "memlcd_app.h"
#include "memlcd_freertos.h"
#include "rsi_debug.h"
#include "sl_log_helper.h"

static void memlcd_task(void *argument)
{
  (void)argument;

  memlcd_app_init();

  while (1) {
    osDelay(1000);
    update_memlcd_app();
  }
}

void memlcd_freertos_init(void)
{
  static const osThreadAttr_t attr = {
    .name       = "memlcd",
    .stack_size = 4096,
    .priority   = osPriorityLow1,
  };

  if (osThreadNew((osThreadFunc_t)memlcd_task, NULL, &attr) == NULL) {
    SL_PRINT_STRING_ERROR("Failed to create memlcd thread\r\n");
    /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */
  }
}
