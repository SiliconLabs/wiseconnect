/***************************************************************************/ /**
* @file bjt_temperature_sensor_example.c
* @brief BJT Temperature Sensor example - demonstrates die temperature measurement
*        using the internal BJT temperature-dependent voltage source
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
#include "rsi_debug.h"
#include "sl_si91x_bjt_temperature_sensor.h"
#include "bjt_temperature_sensor_example.h"

/*******************************************************************************
***************************  Defines / Macros  ********************************
******************************************************************************/

/*******************************************************************************
**********************  Local Function prototypes   ***************************
******************************************************************************/

/*******************************************************************************
 *************************** GLOBAL VARIABLES   *******************************
 ******************************************************************************/

// Temperature sensor configuration structures
// These are defined in the driver with specific values required for temperature measurement:
// - ADC Channel 0 (hardwired to BJT sensor output)
// - OPAMP1 unity gain (required for accurate voltage buffering)
// - Static ADC mode (single conversion per reading)
extern adc_config_t sl_bjt_config;
extern adc_ch_config_t sl_bjt_channel_config;

/*******************************************************************************
**************************   GLOBAL FUNCTIONS   *******************************
******************************************************************************/
/*******************************************************************************
 * ADC example initialization function
 ******************************************************************************/
void bjt_temperature_sensor_example_init(void)
{
  sl_status_t status;

  status = sl_si91x_bjt_temperature_sensor_init(sl_bjt_channel_config, sl_bjt_config);
  if (status != SL_STATUS_OK) {
    /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */
    SL_PRINT_STRING_ERROR("sl_si91x_bjt_temp_init: Error Code : %lu \n", status);
    return;
  }
  /* Calling sl_si91x_bjt_temperature_sensor_init() changes the frequency of
   * the M4SS_REF_CLK during eFuse calibration data access. This clock is also 
   * used by the debug subsystem for SWV (Serial Wire Viewer) output timing.
   * It is necessary to call DEBUGINIT() a second time to reconfigure the debug
   * subsystem's clock prescaler to match the new clock frequency, otherwise 
   * DEBUGOUT() prints will have corrupted characters due to incorrect bit timing.
   */
  DEBUGINIT();
  SL_PRINT_STRING_ERROR("BJT temperature sensor Initialization Success\n");
}

/*******************************************************************************
 * Function will run continuously and will wait for trigger
 ******************************************************************************/
void bjt_temperature_sensor_example_process_action(void)
{
  sl_status_t status;
  double temperature = 0;
  // ADC operation mode if STATIC then it will execute, here it will give
  // equivalent voltage of 12 bit adc output.
  status = sl_si91x_bjt_temperature_sensor_read_data(&temperature);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("BJT Temperature sensor read failed, Error Code: 0x%ld \n", status);
  }
  SL_PRINT_STRING_ERROR("BJT Temperature = %ld\n", (int32_t)temperature);
}
