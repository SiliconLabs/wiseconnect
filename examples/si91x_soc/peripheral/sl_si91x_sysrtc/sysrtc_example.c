/***************************************************************************/ /**
* @file sysrtc_example.c
* @brief Sysrtc examples functions
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
/*==================================================================================
 * @brief :
 * This file contains example application for demonstrating SYSRTC API
 * usage and toggles LED1 on every interrupt and stops & de-initialized after 10
 * interrupts
 * @section Description :
 * This example shows how to use sysrtc peripheral
 * Timer is configured to generate interrupts on reaching compare value and toggles LED.
 * If compare channel is enabled sysrtc generates 10 interrupts and toggles LED after every
 * second it is stopped, callbacks are unregistered and SYSRTC de-initialized.
 * If capture is enabled, it will generate interrupt after 1 sec and toggles LED once.
 * If no channels enabled, application enables overflow interrupt of selected group, generates
 * interrupt on overflow and toggles LED once
 ===================================================================================*/
#include "sl_si91x_sysrtc.h"
#include "sl_si91x_sysrtc_config.h"
#include "sysrtc_example.h"
#include "rsi_board.h"
#include <stdint.h>

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/

#if (SL_SYSRTC_COMPARE_CHANNEL0_ENABLE == 1)
#define COMPARE_CHANNEL SL_SYSRTC_CHANNEL_0 // Channel Number
#endif

#if (SL_SYSRTC_COMPARE_CHANNEL1_ENABLE == 1)
#define COMPARE_CHANNEL SL_SYSRTC_CHANNEL_1 // Channel Number
#endif

#if (SL_SYSRTC_CLK_SRC == CLK_RO_1KHZ)
#define CLOCKS_PER_MILLISECONDS 1 // clocks per milliseconds for 1khz clock frequency
#else
#define CLOCKS_PER_MILLISECONDS 32 // clocks per milliseconds for 32khz clock frequency
#endif

#define TIME_DELAY_IN_MILLISECONDS 1000 // time delay in milliseconds
#define COMPARE_VALUE_32KHZ  \
  ((CLOCKS_PER_MILLISECONDS) \
   * (TIME_DELAY_IN_MILLISECONDS)) // Channel compare value for 1-sec delay with 32kHZ clock frequency
#define COUNTER_VALUE1   0         // Counter register start value
#define COUNTER_VALUE2   0         // Counter register value for overflow interrupt
#define ZER0_COUNT_VALUE 0         // for count value zero
#define ZER0_INTERRUPT   0         // for interrupt count value zero
#define TENTH_INTERRUPT  10        // for tenth interrupt count
#define LED1             1         // For On-board LED-0

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void sysrtc_callback(void *callback_flag);
static uint32_t counter_value1 = COUNTER_VALUE1;
static uint32_t counter_value2 = COUNTER_VALUE2;
/*******************************************************************************
 **********************  Local variables   *************************************
 ******************************************************************************/
static bool state = false;
static sl_status_t status;
static void *callback_flag_data;
static sl_sysrtc_interrupt_enables_t interrupt_enabled;

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
void sysrtc_example_init(void)
{
#if (SL_SYSRTC_GROUP == 0)
  interrupt_enabled.group0_overflow_interrupt_is_enabled = false;
#if (SL_SYSRTC_CAPTURE_CHANNEL0_ENABLE == 1)
  interrupt_enabled.group0_capture0_interrupt_is_enabled = true;
#else
  interrupt_enabled.group0_capture0_interrupt_is_enabled = false;
#endif
#if (SL_SYSRTC_COMPARE_CHANNEL0_ENABLE == 1)
  interrupt_enabled.group0_compare0_interrupt_is_enabled = true;
#else
  interrupt_enabled.group0_compare0_interrupt_is_enabled = false;
#endif
#if (SL_SYSRTC_COMPARE_CHANNEL1_ENABLE == 1)
  interrupt_enabled.group0_compare1_interrupt_is_enabled = true;
#else
  interrupt_enabled.group0_compare1_interrupt_is_enabled = false;
#endif

#endif
#if (SL_SYSRTC_GROUP == 1)
  interrupt_enabled.group1_overflow_interrupt_is_enabled = false;
#if (SL_SYSRTC_CAPTURE_CHANNEL0_ENABLE == 1)
  interrupt_enabled.group1_capture0_interrupt_is_enabled = true;
#else
  interrupt_enabled.group1_capture0_interrupt_is_enabled = false;
#endif
#if (SL_SYSRTC_COMPARE_CHANNEL0_ENABLE == 1)
  interrupt_enabled.group1_compare0_interrupt_is_enabled = true;
#else
  interrupt_enabled.group1_compare0_interrupt_is_enabled = false;
#endif
#if (SL_SYSRTC_COMPARE_CHANNEL1_ENABLE == 1)
  interrupt_enabled.group1_compare1_interrupt_is_enabled = true;
#else
  interrupt_enabled.group1_compare1_interrupt_is_enabled = false;
#endif
#endif
#if ((SL_SYSRTC_GROUP == 0) && (SL_SYSRTC_CAPTURE_CHANNEL0_ENABLE == 0) && (SL_SYSRTC_COMPARE_CHANNEL1_ENABLE == 0) \
     && (SL_SYSRTC_CAPTURE_CHANNEL0_ENABLE == 0))
  interrupt_enabled.group0_overflow_interrupt_is_enabled = true;
#else
  interrupt_enabled.group0_overflow_interrupt_is_enabled = false;
#endif
#if ((SL_SYSRTC_GROUP == 1)                                                                                          \
     && (SL_SYSRTC_CAPTURE_CHANNEL0_ENABLE == SL_SYSRTC_COMPARE_CHANNEL1_ENABLE == SL_SYSRTC_CAPTURE_CHANNEL0_ENABLE \
         == 0))
  interrupt_enabled.group1_overflow_interrupt_is_enabled = true;
#else
  interrupt_enabled.group1_overflow_interrupt_is_enabled = false;
#endif
  do {
    uint32_t compare_value = COMPARE_VALUE_32KHZ;
    // Configuring SYSRTC clock source
    status = sl_si91x_sysrtc_configure_clock(&sl_sysrtc_clk_config_handle);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_sysrtc_configure_clock, Error code: %lu", status);
      break;
    }
    DEBUGOUT("SYSRTC clock configuration is done successfully \n");
    // Initializing and Enabling SYSRTC.
    status = sl_si91x_sysrtc_init(&sysrtc_handle);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_sysrtc_init, Error code: %lu", status);
      break;
    }
    DEBUGOUT("SYSRTC initialization is done successfully \n");
#if ((SL_SYSRTC_COMPARE_CHANNEL0_ENABLE == 1) || (SL_SYSRTC_COMPARE_CHANNEL1_ENABLE == 1))
    // Configuring sysrtc group0, enabling its compare channel
    status = sl_si91x_sysrtc_configure_group(SL_SYSRTC_GROUP, &sysrtc_group_config_handle);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_sysrtc_configure_group, Error code: %lu", status);
      break;
    }
    DEBUGOUT("SYSRTC group configuration is done successfully \n");
    // Registering SYSRTC callback and enabling interrupts
    status =
      sl_si91x_sysrtc_register_callback(sysrtc_callback, &callback_flag_data, SL_SYSRTC_GROUP, &interrupt_enabled);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_config_timer_register_callback, Error code: %lu", status);
      break;
    }
    DEBUGOUT("SYSRTC callback registered successfully\n");
    // Setting counter start value
    sl_si91x_sysrtc_set_count(counter_value1);
    //Setting compare value.
    status = sl_si91x_sysrtc_set_compare_value(SL_SYSRTC_GROUP, COMPARE_CHANNEL, compare_value);
    DEBUGOUT("\r\nSYSRTC group 0 compare value set successfully\r\n");
    // Starting Sysrtc
    sl_si91x_sysrtc_start();
    DEBUGOUT("\nStarted SYSRTC successfully\n");
#endif
#if (SL_SYSRTC_CAPTURE_CHANNEL0_ENABLE == 1)
    uint32_t current_count = 0;
    const sl_sysrtc_group_capture_channel_input_edge_config_t group_capture_channel_config =
      SYSRTC_GROUP_CHANNEL_CAPTURE_CONFIG_DEFAULT;
    // configuring input edge for capture channel
    sysrtc_group_config_handle.p_capture_channel0_config = &group_capture_channel_config;
    // Disabling GPIO input for capture channel
    sl_si91x_sysrtc_enable_input_output_gpio(false);
    // Configuring sysrtc group0, enabling its compare channel
    status = sl_si91x_sysrtc_configure_group(SL_SYSRTC_GROUP, &sysrtc_group_config_handle);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_sysrtc_configure_group, Error code: %lu", status);
      break;
    }
    DEBUGOUT("SYSRTC group configuration is done successfully \n");
    // Registering SYSRTC callback and enabling interrupts
    status =
      sl_si91x_sysrtc_register_callback(sysrtc_callback, &callback_flag_data, SL_SYSRTC_GROUP, &interrupt_enabled);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_config_timer_register_callback, Error code: %lu", status);
      break;
    }
    DEBUGOUT("SYSRTC callback registered successfully\n");
    // Setting counter start value
    sl_si91x_sysrtc_set_count(counter_value1);
    // Starting Sysrtc
    sl_si91x_sysrtc_start();
    DEBUGOUT("\nStarted SYSRTC successfully\n");
    /* Wait until counter is >= 32000 */
    while (current_count < compare_value) {
      sl_si91x_sysrtc_get_count(&current_count);
    }
    /* Trigger bit in MCU_REG1 and wait. */
    status = sl_si91x_sysrtc_sets_register_capture_input(SL_SYSRTC_GROUP);
    //rsi_sysrtc_set_capture_reg(0u);
#endif
#if ((SL_SYSRTC_COMPARE_CHANNEL0_ENABLE == 0) && (SL_SYSRTC_COMPARE_CHANNEL1_ENABLE == 0) \
     && (SL_SYSRTC_CAPTURE_CHANNEL0_ENABLE == 0))
    // Registering SYSRTC callback and enabling overflow interrupts
    status =
      sl_si91x_sysrtc_register_callback(sysrtc_callback, &callback_flag_data, SL_SYSRTC_GROUP, &interrupt_enabled);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_config_timer_register_callback, Error code: %lu", status);
      break;
    }
    DEBUGOUT("SYSRTC callback registered successfully\n");
    // Setting counter start value
    sl_si91x_sysrtc_set_count(counter_value2);
    // Starting Sysrtc
    sl_si91x_sysrtc_start();
    DEBUGOUT("\nStarted SYSRTC successfully\n");
#endif
  } while (false);
}

/***************************************************************************/ /**
 * Function will run continuously and will wait for trigger
 ******************************************************************************/
void sysrtc_example_process_action(void)
{
}

/*******************************************************************************
* Callback function of ulp-timer instance
*
* @param none
* @return none
******************************************************************************/
void sysrtc_callback(void *callback_flags)
{
  DEBUGOUT("In handler... \n");
  static uint8_t interrupt_count = 0;
  uint32_t compare_value         = COMPARE_VALUE_32KHZ;
  uint32_t current_count         = 0;
  // Incrementing interrupt count
  interrupt_count++;
  // To toggle LED1
  state = !state;
  RSI_Board_LED_Set(LED1, state);
  // Reading current count
#if ((SL_SYSRTC_COMPARE_CHANNEL0_ENABLE == 1) || (SL_SYSRTC_COMPARE_CHANNEL1_ENABLE == 1))
  status = sl_si91x_sysrtc_get_count(&current_count);
  // Setting compare value again after every interrupt, to get next interrupt
  sl_si91x_sysrtc_set_compare_value(SL_SYSRTC_GROUP, COMPARE_CHANNEL, (current_count + compare_value));
  // Checking interrupt count
  if (interrupt_count == TENTH_INTERRUPT) {
    //Stopping the timer instance, after five LED toggles.
    sl_si91x_sysrtc_stop();
    // Unregister callback and disabling interrupts
    status = sl_si91x_sysrtc_unregister_callback(SL_SYSRTC_GROUP, &interrupt_enabled);
    // Deinit sysrtc
    sl_si91x_sysrtc_deinit();
  }
#endif
}
