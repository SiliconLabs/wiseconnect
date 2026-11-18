/*******************************************************************************
 * @file  blinky.c
 * @brief
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
/**============================================================================
 * @section Description
 * This files contains example code to demonstrate the GPIO/LED toggle functionality.
 ============================================================================**/

// Include Files
#include "rsi_ccp_user_config.h"
#include "sl_sleeptimer.h"
#include "sl_si91x_led_instances.h"
#include "rsi_debug.h"
#include "sl_si91x_led.h"
#include "cmsis_os2.h"
#include "sl_log_helper.h"
/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#ifndef LED_INSTANCE
#define LED_INSTANCE led_led0
#endif

#ifndef TOGGLE_DELAY_MS
#define TOGGLE_DELAY_MS 500
#endif

/*******************************************************************************
 ***************************  GLOBAL VARIABLES   *******************************
 ******************************************************************************/
/* Written in init, read by threads; volatile ensures threads see it with LTO. */
static volatile osMutexId_t led_mutex_id;

static const osMutexAttr_t led_blink_mutex_attr = {
  "ledBlinkMutex",  // human readable mutex name
  osMutexRecursive, // attr_bits
  NULL,             // memory for control block
  0U                // size for control block
};
/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

sl_sleeptimer_timer_handle_t timer;
/* Volatile: set in sleeptimer callback, read in RTOS threads. Required for LTO. */
static volatile bool toggle_timeout = false;

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

static void on_timeout(sl_sleeptimer_timer_handle_t *handle, void *data);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
void blinky_init(void);
void blinky_process_action(void);
/*******************************************************************************
 * Initialize LED blinky with Mutex example.
 ******************************************************************************/
void blinky_init(void)
{
  // Create timer for waking up the system periodically.
  sl_sleeptimer_start_periodic_timer_ms(&timer,
                                        TOGGLE_DELAY_MS,
                                        on_timeout,
                                        NULL,
                                        0,
                                        SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);

  led_mutex_id = osMutexNew(&led_blink_mutex_attr);
  if (led_mutex_id != NULL) {
    // Mutex object created
  }
}

/*******************************************************************************
 * Function to turn on LED based on Mutex availability.
 ******************************************************************************/
void led_on_process_action(void)
{
  osStatus_t status;
  while (1) {
    status = osMutexAcquire(led_mutex_id, osWaitForever); // try to acquire mutex
    if (status != osOK) {
      /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */

      SL_PRINT_STRING_ERROR("led_on task failed to acquired mutex\r\n");
      break;
    }
    SL_PRINT_STRING_ERROR("led_on task acquired mutex\r\n");

    while (!toggle_timeout)
      ;
    toggle_timeout = false;

    // turn on LED
    sl_si91x_led_set(LED_INSTANCE.pin);
    SL_PRINT_STRING_ERROR("LED turned ON\r\n");

    status = osMutexRelease(led_mutex_id);
    if (status != osOK) {
      SL_PRINT_STRING_ERROR("led_on task failed to release mutex\r\n");
      break;
    }

    osDelay(10U); // wait 10ms until start actual work
  }
}

/*******************************************************************************
 * Function to turn off LED based on Mutex availability.
 ******************************************************************************/
void led_off_process_action(void)
{
  osStatus_t status;
  while (1) {
    status = osMutexAcquire(led_mutex_id, osWaitForever); // try to acquire mutex
    if (status != osOK) {
      SL_PRINT_STRING_ERROR("led_off task failed to acquire mutex\r\n");
      break;
    }
    SL_PRINT_STRING_ERROR("led_off task acquired mutex\r\n");

    while (!toggle_timeout)
      ;
    toggle_timeout = false;

    // turn off LED
    sl_si91x_led_clear(LED_INSTANCE.pin);
    SL_PRINT_STRING_ERROR("LED turned OFF\r\n");

    status = osMutexRelease(led_mutex_id);
    if (status != osOK) {
      SL_PRINT_STRING_ERROR("led_off task failed to release mutex\r\n");
      break;
    }

    osDelay(10U); // wait 10ms until start actual work
  }
}

/***************************************************************************/ /**
 * Sleeptimer timeout callback.
 ******************************************************************************/
static void on_timeout(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  (void)&handle;
  (void)&data;
  toggle_timeout = true;
}
