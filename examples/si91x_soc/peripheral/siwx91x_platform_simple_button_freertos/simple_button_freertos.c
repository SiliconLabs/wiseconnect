/***************************************************************************/ /**
 * @file simple_button_freertos.c
 * @brief simple button example functions
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

#include "simple_button_freertos.h"
#include "sl_si91x_button.h"
#include "sl_si91x_button_pin_config.h"
#include "sl_si91x_button_instances.h"
#include "cmsis_os2.h"
#include "stdio.h"
#include "rsi_debug.h"
#include "sl_log_helper.h"
#ifndef SI917_DEVKIT
#include "sl_si91x_led.h"
#include "sl_si91x_led_config.h"
#include "sl_si91x_led_instances.h"
#else
#include "sl_si91x_rgb_led.h"
#include "sl_si91x_rgb_led_config.h"
#include "sl_si91x_rgb_led_instances.h"
#endif

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/
#ifndef BUTTON_INSTANCE
#define BUTTON_INSTANCE button_btn0
#endif

#ifndef LED_INSTANCE
#define LED_INSTANCE led_led0
#endif
/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/
static osSemaphoreId_t button_sem;
/*******************************************************************************
**********************  Local Function prototypes   ***************************
******************************************************************************/
/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
/*******************************************************************************
 **************************   GLOBAL VARIABLES   *******************************
 ******************************************************************************/

static const osThreadAttr_t thread_attributes = {
  .name       = "simple_button_app",
  .stack_size = 1024, // or 2048 if button code uses more stack
  .priority   = osPriorityLow,
};

/***************************************************************************/ /**
 * Initialize example.
 ******************************************************************************/
void button_init(void)
{
  osThreadId_t thread_id = osThreadNew((osThreadFunc_t)simple_button_task, NULL, &thread_attributes);
  if (thread_id == NULL) {
    /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */
    SL_PRINT_STRING_ERROR("Failed to create button thread\r\n");
    return;
  }
}

/***************************************************************************/ /**
 * Button task.
 ******************************************************************************/
void simple_button_task(void *argument)
{
  (void)argument;

  if (button_sem == NULL) {
    button_sem = osSemaphoreNew(1U, 0U, NULL);
    if (button_sem == NULL) {
      SL_PRINT_STRING_ERROR("Failed to create button semaphore\r\n");
      osThreadExit();
    }
  }

  while (1) {
    if (button_sem != NULL) {
      (void)osSemaphoreAcquire(button_sem, osWaitForever);
    }
#ifndef SI917_DEVKIT
    sl_si91x_led_toggle(LED_INSTANCE.pin);
#else
    sl_si91x_simple_rgb_led_toggle(&LED_INSTANCE);
#endif
  }
}

/***************************************************************************/ /**
 * Call back on button change.
 ******************************************************************************/
void sl_si91x_button_isr(uint8_t pin, int8_t state)
{
  if (pin == BUTTON_INSTANCE.pin) {
    if (state == BUTTON_PRESSED) {
      if (button_sem != NULL) {
        (void)osSemaphoreRelease(button_sem);
      }
    }
  }
}
