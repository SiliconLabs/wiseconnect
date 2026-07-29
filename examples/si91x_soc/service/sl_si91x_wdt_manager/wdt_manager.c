/*******************************************************************************
 * @file  wdt_manager.c
 * @brief WDT Manager example implementation
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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
 * This file contains example code to demonstrate WDT manager integration
 * with LED blinking functionality. The WDT manager ensures system reliability
 * by monitoring the application and resetting the system if it becomes
 * unresponsive.
 ============================================================================**/

// Include Files
#include "sl_sleeptimer.h"
#include "sl_si91x_led_instances.h"
#include "rsi_debug.h"
#include "sl_si91x_led.h"
#include "wdt_manager.h"
#include "sl_log_helper.h"
/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#ifndef LED_INSTANCE
#define LED_INSTANCE led_led0
#endif

#ifndef TOGGLE_DELAY_MS
#define TOGGLE_DELAY_MS 1000
#endif

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

sl_sleeptimer_timer_handle_t timer;
/* Volatile: set in sleeptimer callback (ISR), read in main loop. Required for LTO. */
static volatile bool toggle_timeout = false;
static uint8_t led_toggle_count     = 0;

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

static void on_timeout(sl_sleeptimer_timer_handle_t *handle, void *data);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************/ /**
 * Initialize WDT Manager example.
 * 
 * This function initializes the WDT manager example with LED toggling functionality.
 * The WDT manager is automatically initialized and started by the WDT Manager
 * component to ensure system reliability.
 * 
 * The WDT manager will:
 * - Monitor the application execution
 * - Reset the system if the application becomes unresponsive
 * - Provide system reliability and recovery mechanisms
 ******************************************************************************/
void wdt_manager_init(void)
{
  /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */
  SL_PRINT_STRING_ERROR("WDT Manager Example Started\r\n");
  SL_PRINT_STRING_ERROR("LED will toggle every %d ms\r\n", TOGGLE_DELAY_MS);
  SL_PRINT_STRING_ERROR("After exactly 10 LED toggles, system will hang and WDT "
                        "will reset it\r\n");
  SL_PRINT_STRING_ERROR("WDT manager is active for system reliability\r\n");

  // Reset LED toggle counter
  led_toggle_count = 0;

  // Create timer for waking up the system periodically to toggle LED
  sl_sleeptimer_start_periodic_timer_ms(&timer,
                                        TOGGLE_DELAY_MS,
                                        on_timeout,
                                        NULL,
                                        0,
                                        SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);

  // Toggle LED once to indicate initialization
  sl_si91x_led_toggle(LED_INSTANCE.pin);
  led_toggle_count++;
  SL_PRINT_STRING_ERROR("LED toggled - Count: %d\r\n", led_toggle_count);
}

/***************************************************************************/ /**
 * WDT Manager process action function.
 * 
 * This function handles the LED toggling based on timer timeout events.
 * It should be called continuously in the main loop.
 * 
 * The WDT manager runs in the background and will automatically:
 * - Kick the watchdog timer to prevent system reset
 * - Monitor application health
 * - Reset the system if the application becomes unresponsive
 ******************************************************************************/
void wdt_manager_process_action(void)
{
  if (toggle_timeout == true) {
    sl_si91x_led_toggle(LED_INSTANCE.pin);
    led_toggle_count++;
    toggle_timeout = false;

    // Print debug message to show LED toggle count
    SL_PRINT_STRING_ERROR("LED toggled - Count: %d\r\n", led_toggle_count);

    // After exactly 10 LED toggles, create system hang to trigger WDT reset
    if (led_toggle_count >= 10) {
      SL_PRINT_STRING_ERROR("System hanging now - WDT will reset the system!\r\n");
      SL_PRINT_STRING_ERROR("Watch for system reset in a few seconds...\r\n");

      // Stop the timer to prevent further LED toggles
      sl_sleeptimer_stop_timer(&timer);

      // Disable interrupts to prevent WDT from being kicked
      __asm volatile("cpsid i" : : : "memory");

      // Create infinite loop to hang the system
      // With interrupts disabled, WDT cannot be kicked and will reset the system
      while (1) {
        // Infinite loop with interrupts disabled
        // WDT timer will expire and reset the system
      }
    }
  }
}

/***************************************************************************/ /**
 * Sleeptimer timeout callback.
 * 
 * This callback is called when the periodic timer expires, indicating
 * that it's time to toggle the LED.
 ******************************************************************************/
static void on_timeout(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  (void)&handle;
  (void)&data;
  toggle_timeout = true;
}
