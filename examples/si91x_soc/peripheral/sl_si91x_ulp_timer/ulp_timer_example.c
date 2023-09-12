/***************************************************************************/ /**
* @file ulp_timer_example.c
* @brief ULP Timer example
*******************************************************************************
* # License
* <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* The licenser of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/
/*==================================================================================
 * @brief : This file contains example application for ULP-Timer and LED1 toggle
 * @section Description :
 * This example shows how to use ULP timer to toggle LED at 1sec periodic rate,
   configured with default parameters.
 * Timer is configured to generate interrupts upon expiration and toggles LED.
 * After toggling LED for five times, timer stops and configured with new parameters and toggles LED again five times.
 * After that timer is stopped and timeout-callbacks are unregistered.
 ===================================================================================*/
#include "sl_si91x_ulp_timer.h"
#include "ulp_timer_example.h"
#include "sl_si91x_ulp_timer_init.h"
#include "rsi_board.h"
#include "rsi_chip.h"
#include <stdint.h>

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define ONE_SEC                       1000000 // 1 second in microseconds
#define TIME_IN_MICROSECONDS          ONE_SEC // timer timeout in seconds
#define CLOCKS_PER_MICROSECONDS_20MHZ 20      // clock cycles per microsecond for 20 mhz clock frequency
/*****************************************************************************
 * ULP Timer-match value calculation :
 * For one microsecond type, match-value = number of microseconds
 * For 256 microsecond type, match-value = (time in microseconds)/256
 * For normal up/down counter type, timer =  (number of clock cycles per microseconds * time in microseconds)
 ******************************************************************************/
#define SL_TIMER_MATCH_VALUE \
  ((CLOCKS_PER_MICROSECONDS_20MHZ) * (TIME_IN_MICROSECONDS)) // Timer match value for down-counter type with 20mhz clock
#define RESERVED_IRQ_COUNT             16                    // Reserved IRQ count
#define EXT_IRQ_COUNT                  98                    // External IRQ count
#define VECTOR_TABLE_ENTRIES           (RESERVED_IRQ_COUNT + EXT_IRQ_COUNT) // Vector table entries
#define LED1                           1                                    // For On-board LED-1
#define ONE_SEC_MATCH_VALUE_256US_TYPE 3906 // Timer match value for 1-sec, in 256us type
#define FIFTH_INTERRUPT_COUNT          5    // Count for fifth timeout interrupt
#define TENTH_INTERRUPT_COUNT          10   // Count for tenth timeout interrupt

// Macros used to construct ulp-timer instance
#define ULP_TIMER_INSTANCE \
  SL_ULP_TIMER_TIMER0 // ulp-timer instance to be used, user can pass selected timer-number in place of '0'
#define _CONCAT_TWO_TOKENS(token_1, token_2) token_1##token_2
#define CONCAT_TWO_TOKENS(token_1, token_2)  _CONCAT_TWO_TOKENS(token_1, token_2)
#define SL_ULP_TIMER_HANDLE                  CONCAT_TWO_TOKENS(sl_timer_handle_timer, ULP_TIMER_INSTANCE)
#define SL_ULP_TIMER_CALLBACK                CONCAT_TWO_TOKENS(on_timeout_callback, ULP_TIMER_INSTANCE)

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void SL_ULP_TIMER_CALLBACK(void);

/*******************************************************************************
 **********************  Local variables   ***************************
 ******************************************************************************/
static bool state                          = false;
static bool timer_parameter_change_flag    = false;
static bool timer_callback_unregister_flag = false;
static uint32_t timer_count_value;
static uint32_t timer_direction;
sl_status_t status;
uint32_t ramVector[VECTOR_TABLE_ENTRIES] __attribute__((aligned(256)));
extern void hardware_setup(void);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
void ulp_timer_example_init(void)
{
  /* At this stage the micro-controller clock setting is already configured,
   * this is done through SystemInit() function which is called from startup
   * file (startup_rs1xxxx.s) before to branch to application main.
   * To reconfigure the default setting of SystemInit() function, refer to
   * startup_rs1xxxx.c file
   */
  //copying the vector table from flash to ram
  memcpy(ramVector, (uint32_t *)SCB->VTOR, sizeof(uint32_t) * VECTOR_TABLE_ENTRIES);
  // Assigning the ram vector address to VTOR register
  SCB->VTOR = (uint32_t)ramVector;
  // Switching MCU from PS4 to PS2 state(low power state)
  // In this mode, whatever be the timer clock source value, it will run with 20MHZ only,
  // as it trims higher clock frequencies to 20MHZ.
  // To use timer in high power mode, don't call hardware_setup()
  hardware_setup();
  DEBUGINIT();
  do {
    // Timer Peripheral input clock source configuration, common for all ulp-timer instances
    status = sl_si91x_ulp_timer_init(&sl_timer_clk_handle);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_ulp_timer_init : Invalid Parameters, Error Code : %lu \n", status);
      break;
    }
    DEBUGOUT("Successfully Configured ULP-timer clock input source \n");
    // Updating default timer match-value
    SL_ULP_TIMER_HANDLE.timer_match_value = SL_TIMER_MATCH_VALUE;
    // Configuring timer instance parameters: mode-periodic, type-1us, match-value: 1second
    status = sl_si91x_ulp_timer_set_configuration(&(SL_ULP_TIMER_HANDLE));
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_ulp_timer_set_configuration : Invalid Parameters Error Code : %lu \n", status);
      break;
    }
    DEBUGOUT("Successfully Configured ULP-timer parameters with default parameters \n");
    // Registering timeout callback for the selected timer instance, which will also enable its interrupt
    status = sl_si91x_ulp_timer_register_timeout_callback(ULP_TIMER_INSTANCE, &(SL_ULP_TIMER_CALLBACK));
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_ulp_timer_timeout_callback_register : Invalid Parameters Error Code : %lu \n", status);
      break;
    }
    DEBUGOUT("Successfully Registered timer instance timeout callback \n");
    // Starting Timer instance with default parameters
    status = sl_si91x_ulp_timer_start(ULP_TIMER_INSTANCE);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_ulp_timer_start : Invalid Parameters Error Code : %lu \n", status);
      break;
    }
    DEBUGOUT("Successfully started ulp-timer instance with default parameters \n");
  } while (false);
}

/*******************************************************************************
 * Function will run continuously and will wait for trigger
 ******************************************************************************/
void ulp_timer_example_process_action(void)
{
  //Checking timer callback unregister flag value
  if (timer_callback_unregister_flag) {
    // Resetting callback-unregister flags, on timer operation completion
    timer_callback_unregister_flag = false;
    timer_parameter_change_flag    = false;
    // Unregistering timer instance callback and disabling interrupt
    status = sl_si91x_ulp_timer_unregister_timeout_callback(ULP_TIMER_INSTANCE);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_ulp_timer_timeout_callback_unregister : Invalid Parameters Error Code : %lu \n", status);
    } else {
      DEBUGOUT("Unregistered timer timeout callback, on timer operation completion\n");
    }
  }
  // Checking parameter-change flag, for changing timer-instance default parameters to new parameters
  if (timer_parameter_change_flag) {
    // Resetting parameter-change flag
    timer_parameter_change_flag = false;

    // Changing timer-type to 256US mode
    status = sl_si91x_ulp_timer_set_type(ULP_TIMER_INSTANCE, ULP_TIMER_TYP_256US);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_ulp_timer_set_type : Invalid Parameters Error Code : %lu \n", status);
    } else {
      DEBUGOUT("Successfully changed timer-type to 256US mode \n");
    }
    // Resetting timer direction
    status = sl_si91x_ulp_timer_set_direction(ULP_TIMER_INSTANCE, UP_COUNTER);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_ulp_timer_set_direction : Invalid Parameters Error Code : %lu \n", status);
    } else {
      DEBUGOUT("Successfully changed timer-direction to up-counter \n");
    }
    // Changing timer-instance match value to number of ticks required for 1sec time-out, in 256us mode ( 1000000/256)
    status = sl_si91x_ulp_timer_set_count(ULP_TIMER_INSTANCE, ONE_SEC_MATCH_VALUE_256US_TYPE);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_ulp_timer_set_count : Invalid Parameters Error Code : %lu \n", status);
    } else {
      DEBUGOUT("Successfully changed the timer match value\n");
    }

    // Starting the timer instance in one-shot mode with 256us type and 1-second time-out value
    status = sl_si91x_ulp_timer_start(ULP_TIMER_INSTANCE);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_ulp_timer_start : Invalid Parameters Error Code : %lu \n", status);
    } else {
      DEBUGOUT("Successfully started timer instance with new parameters \n");
    }
    // Reading timer count after starting timer instance
    status = sl_si91x_ulp_timer_get_count(ULP_TIMER_INSTANCE, &timer_count_value);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_ulp_timer_get_count : Invalid Parameters Error Code : %lu \n", status);
    } else {
      DEBUGOUT("Timer count value = %lu, after starting timer\n", timer_count_value);
    }
    // Checking counter direction
    status = sl_si91x_ulp_timer_get_direction(ULP_TIMER_INSTANCE, &timer_direction);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_ulp_timer_get_count : Invalid Parameters Error Code : %lu \n", status);
    } else {
      if (timer_direction) {
        DEBUGOUT("Timer running as up-counter\n");
      } else {
        DEBUGOUT("Timer running as down-counter\n");
      }
    }
  }
}

/*******************************************************************************
* Callback function of ulp-timer instance
*
* @param none
* @return none
******************************************************************************/
static void SL_ULP_TIMER_CALLBACK(void)
{
  static uint8_t interrupt_count = 0;
  DEBUGOUT("On timer%d callback \n", ULP_TIMER_INSTANCE);
  // Incrementing interrupt count
  interrupt_count++;
  // To toggle LED1
  state = !state;
  RSI_Board_LED_Set(LED1, state);
  // Checking interrupt count
  if (interrupt_count == FIFTH_INTERRUPT_COUNT) {
    //Stopping the timer instance, after five LED toggles.
    status = sl_si91x_ulp_timer_stop(ULP_TIMER_INSTANCE);
    if (status != SL_STATUS_OK) {
      return;
    }
    // Setting parameter-change flag to true, so that APIs to change timer parameter will get called
    timer_parameter_change_flag = true;
  }
  // Checking interrupt count
  if (interrupt_count == TENTH_INTERRUPT_COUNT) {
    // Setting unregister flag true, so that after timer operation callback-unregister API should get called
    timer_callback_unregister_flag = true;
    // Stopping the timer instance, after five more LED toggles.
    status = sl_si91x_ulp_timer_stop(ULP_TIMER_INSTANCE);
    if (status != SL_STATUS_OK) {
      return;
    }
  }
}
