/*******************************************************************************
 * @file bod_button_example.c
 * @brief BOD Button example
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

/* Includes ------------------------------------------------------------------*/
#include "bod_button_example.h"
#include "rsi_debug.h"
#include "sl_si91x_bod.h"

/*******************************************************************************
***************************  Defines / Macros  ********************************
******************************************************************************/

/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
uint8_t button_max_value;  // Variable to store the maximum button value
uint8_t button1_min_value; // Variable to store the minimum value for button 1
uint8_t button2_min_value; // Variable to store the minimum value for button 2
uint8_t button3_min_value; // Variable to store the minimum value for button 3

float vbatt     = 0; // Variable to store the current battery voltage
float lastVbatt = 0; // Variable to store the last read battery voltage
// Define voltage ranges for buttons
// User configuration parameters for BOD button voltage thresholds
sl_bod_button_uc_config_param_t sl_bod_button_uc_config_param = {
  .button_wakeup_enable            = Enable,                                // Enable wakeup on button press
  .button_1_min_voltage_percentage = SL_BOD_BUTTON1_MIN_VOLTAGE_PERCENTAGE, // Min voltage % for button 1
  .button_1_max_voltage_percentage = SL_BOD_BUTTON1_MAX_VOLTAGE_PERCENTAGE, // Max voltage % for button 1
  .button_2_min_voltage_percentage = SL_BOD_BUTTON2_MIN_VOLTAGE_PERCENTAGE, // Min voltage % for button 2
  .button_2_max_voltage_percentage = SL_BOD_BUTTON2_MAX_VOLTAGE_PERCENTAGE, // Max voltage % for button 2
  .button_3_min_voltage_percentage = SL_BOD_BUTTON3_MIN_VOLTAGE_PERCENTAGE, // Min voltage % for button 3
  .button_3_max_voltage_percentage = SL_BOD_BUTTON3_MAX_VOLTAGE_PERCENTAGE  // Max voltage % for button 3
};
/******************************************************************************
**************************   GLOBAL VARIABLES   *******************************
******************************************************************************/
static volatile uint8_t button_number = 0;

/******************************************************************************
**************************   GLOBAL FUNCTIONS   *******************************
******************************************************************************/

/******************************************************************************
**************************   BOD Callback Function   **************************
******************************************************************************/
void user_bod_button_callback(void)
{
  button_number = sl_si91x_bod_button_value_read();
  /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */
  SL_PRINT_STRING_ERROR("BOD Button number: %d \r\n", button_number);
}
/******************************************************************************
**************************   BoD Button example init  *************************
******************************************************************************/
void bod_button_example_init(void)
{
  sl_status_t status = SL_STATUS_ABORT;
  do {
    // Set BOD (Brown-Out Detector) to manual mode
    sl_si91x_bod_set_mode(SL_BOD_MANUAL_MODE);
    // Configure button voltage thresholds and get min/max values for each button
    sl_si91x_bod_button_set_configuration(sl_bod_button_uc_config_param,
                                          &button_max_value,
                                          &button1_min_value,
                                          &button2_min_value,
                                          &button3_min_value);

    // Apply the button configuration using the calculated min/max values
    sl_si91x_bod_button_configuration(button_max_value, button1_min_value, button2_min_value, button3_min_value);

    SL_PRINT_STRING_ERROR("BOD Button Configuration successful \r\n");

    // Enable wakeup on button press if configured
    if (sl_bod_button_uc_config_param.button_wakeup_enable == ENABLE) {
      status = sl_si91x_bod_button_wakeup_enable_v2(sl_bod_button_uc_config_param.button_wakeup_enable);
      if (status == SL_STATUS_OK)
        SL_PRINT_STRING_ERROR("BOD Button Wakeup enabled successful \r\n");
      else {
        SL_PRINT_STRING_ERROR("BOD Button Wakeup enable Fail \r\n");
        break;
      }
    }

    // Register the user callback for button events
    status = sl_si91x_bod_button_register_callback(user_bod_button_callback);
    if (!status)
      SL_PRINT_STRING_ERROR("BOD Button call back registration successful \r\n");
    else {
      SL_PRINT_STRING_ERROR("BOD Button call back registration Failed \r\n");
      break;
    }

    /* Enable button interrupt */
    sl_si91x_bod_button_enable_interrupt();
    SL_PRINT_STRING_ERROR("BOD Button interrupt enabled successful \r\n");
  } while (false);
}
/******************************************************************************
**************************   BDD Button Process Action  ***********************
******************************************************************************/
void bod_button_process_action(void)
{
  // Get the current battery voltage
  sl_status_t status = sl_si91x_bod_get_battery_status(&vbatt);
  if (status != SL_STATUS_OK) {
    // Print error if reading battery voltage fails
    SL_PRINT_STRING_ERROR("Vbat read failed\r\n");
  } else if (lastVbatt != vbatt) {
    // If battery voltage has changed, reconfigure button thresholds
    sl_si91x_bod_button_set_configuration(sl_bod_button_uc_config_param,
                                          &button_max_value,
                                          &button1_min_value,
                                          &button2_min_value,
                                          &button3_min_value);
    // Apply the new button configuration
    sl_si91x_bod_button_configuration(button_max_value, button1_min_value, button2_min_value, button3_min_value);

    // Enable wakeup on button press if configured

    status = sl_si91x_bod_button_wakeup_enable_v2(sl_bod_button_uc_config_param.button_wakeup_enable);
    if (status == SL_STATUS_OK)
      SL_PRINT_STRING_ERROR("BOD Button Wakeup enabled successful \r\n");
    else
      SL_PRINT_STRING_ERROR("BOD Button Wakeup enable Fail \r\n");

    // Enable button interrupt
    sl_si91x_bod_button_enable_interrupt();
    // Update last read battery voltage
    lastVbatt = vbatt;
  }
}
