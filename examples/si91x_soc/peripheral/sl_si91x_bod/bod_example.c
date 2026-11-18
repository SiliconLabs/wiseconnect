/*******************************************************************************
 * @file bod_example.c
 * @brief BOD example
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
#include "bod_example.h"
#include "rsi_debug.h"
#include "sl_si91x_bod.h"
#include "sl_si91x_clock_manager.h"

/*******************************************************************************
***************************  Defines / Macros  ********************************
******************************************************************************/

/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
static float vbatt                          = 0;
static float vbat_percentage                = 0;
static volatile boolean_t sl_bod_inter_flag = 0;
sl_bod_uc_param_t usr_config_params         = { .slot_value = SL_BOD_DEFAULT_SLOT_VALUE, .blackout_en = DISABLE };
/******************************************************************************
**************************   GLOBAL VARIABLES   *******************************
******************************************************************************/

/******************************************************************************
**************************   GLOBAL FUNCTIONS   *******************************
******************************************************************************/

/******************************************************************************
**************************   BOD Callback Function   **************************
******************************************************************************/
static void bod_callback(void)
{
  // Set the BOD interrupt flag
  sl_bod_inter_flag = 1;
}

/******************************************************************************
************   Initializes the BOD (Brown-Out Detection) example   ************
******************************************************************************/
void bod_example_init(void)
{
  sl_status_t status    = SL_STATUS_FAIL; // Variable to store the status of BOD operations
  float threshold_value = 0;              // Variable to store the BOD threshold value
  do {
    // Initialize the BOD (Brown-Out Detector)
    status = sl_si91x_bod_init();
    if (status != SL_STATUS_OK) {
      /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */
      SL_PRINT_STRING_ERROR("\r\n BOD Initialization Failed\r\n");
      break;
    } else {
      SL_PRINT_STRING_ERROR("\r\n BOD Initialization is Successful\r\n");
    }
    // Register the BOD callback function
    status = sl_si91x_bod_register_callback(bod_callback);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("\r\n  BOD callback Registered Fail \r\n");
      break;
    } else {
      SL_PRINT_STRING_ERROR("\r\n Successfully Registered  BOD callback \r\n");
    }
    // Set the BOD threshold
    status = sl_si91x_bod_set_threshold(SL_BOD_DEFAULT_THRESHOLD);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("\r\n BOD Threshold Configuration Failed\r\n");
      break; // Return if setting threshold fails
    } else
      SL_PRINT_STRING_ERROR("\r\n BOD Threshold Configuration Successful\r\n");

    // Set the BOD configuration parameters
    status = sl_si91x_bod_set_configuration(usr_config_params);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("\r\n BOD Configurations Failed\r\n");
      break;
    } else {
      SL_PRINT_STRING_ERROR("\r\n BOD Configurations Successful\r\n");
    }

    // Get the calibrated threshold value
    status = sl_si91x_bod_get_threshold(&threshold_value);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("\r\n BOD Configurations Failed\r\n");
      break;
    } else {
      SL_PRINT_STRING_ERROR("\r\n BOD Threshold Value is %ld mV\r\n", (int32_t)(threshold_value * 1000.0f));
    }

    // Enable BOD interrupt
    sl_si91x_bod_enable_interrupt();
    SL_PRINT_STRING_ERROR("\r\n BOD Interrupt Enabled successfully \r\n");
  } while (false);
}

/*****************************************************************************
 ******** Process action for the BOD (Brown-Out Detection) example. **********
 *****************************************************************************/
void bod_example_process_action(void)
{
  sl_status_t status = SL_STATUS_FAIL; // Variable to store the status of BOD operations
  do {
    if (sl_bod_inter_flag) {
      // Get the battery status (voltage level)
      status = sl_si91x_bod_get_battery_status(&vbatt);
      if (status == SL_STATUS_OK) {
        // Get the battery percentage based on the voltage level
        status = sl_si91x_bod_battery_percentage(vbatt, &vbat_percentage);
        if (status == SL_STATUS_OK) {
          // Check if the BOD interrupt flag is set
          if (sl_si91x_bod_get_blackout_status()) {
            // Enable blackout reset mode
            SL_PRINT_STRING_ERROR("Blackout mode is enabled. It may pull down "
                                  "the RESET pin, and the current "
                                  "battery level is %.2f%% \r\n\n",
                                  vbat_percentage);
          } else {
            SL_PRINT_STRING_ERROR(" Your Vbatt status is less than the threshold voltage i.e "
                                  "%ldmV battery Percentage is -- %.3f%% \r\n\n",
                                  (int32_t)(vbatt * 1000.0f),
                                  vbat_percentage);
          }
          // Clear the BOD interrupt flag
          sl_bod_inter_flag = 0;
        } else {
          SL_PRINT_STRING_ERROR(" Battery Percentage read failed \r\n");
          break;
        }
      } else {
        SL_PRINT_STRING_ERROR(" \r\nBattery read fail\r\n");
        break; // Battery Status read failed
      }
      sl_si91x_delay_ms(SL_BOD_500MS_DELAY);
    }
  } while (false);
}