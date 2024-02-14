/***************************************************************************/ /**
 * @file ro_temperature_sensor_example
 * @brief RO temperature sensor example
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
#include "sl_si91x_ro_temperature_sensor.h"
#include "ro_temperature_sensor_example.h"
#include "rsi_debug.h"
#include "sl_sleeptimer.h"
/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define NOMINAL_TEMP       25   // Calibrated temperature value
#define COUNT_FREEZE       255  // Count of reference clock on which ptat clock counts
#define DELAY_MS1_PERIODIC 2000 //sleeptimer1 periodic timeout in ms
/*******************************************************************************
 ******************************  Data Types  ***********************************
 ******************************************************************************/

/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
sl_sleeptimer_timer_handle_t timer1; //sleeptimer1 handle
boolean_t delay_timeout = false;     //Indicates sleeptimer1 timeout
/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
//Sleeptimer timeout callbacks
static void on_timeout_timer1(sl_sleeptimer_timer_handle_t *handle, void *data);
/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/*******************************************************************************
 * RO temperature sensor example initialization function
 ******************************************************************************/
void ro_temperature_sensor_example_init(void)
{
  sl_status_t status;
  uint32_t ref_clk_cnt = 0, ptat_clk_cnt = 0;
  do {
    //Start 400 ms periodic timer
    sl_sleeptimer_start_periodic_timer_ms(&timer1,
                                          DELAY_MS1_PERIODIC,
                                          on_timeout_timer1,
                                          NULL,
                                          0,
                                          SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
    // RO clock from analog is taken as reference clock
    status = sl_si91x_ro_temperature_select_reference_clock(SL_RO_REF_CLK);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("Reference clock select failed, Error Code: 0x%ld \n", status);
      break;
    }
    DEBUGOUT("Reference clock selection done\n");
    // Calibrated temperature value is set as nominal temperature
    status = sl_si91x_ro_temperature_nominal(NOMINAL_TEMP);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("Nominal temperature value set failed, Error Code: 0x%ld \n", status);
      break;
    }
    DEBUGOUT("Nominal temperature value set done\n");
    // Temperature is updated through RO based calculation
    status = sl_si91x_ro_temperature_based_update(SL_RO_TEMP_BJT_UPDATE_ENABLE);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("Enabling BJT based temp sensor failed, Error Code: 0x%ld \n", status);
      break;
    }
    DEBUGOUT("Enabling BJT based temp sensor done\n");
    // Count of reference clock on which ptat clock counts is set to default value.
    status = sl_si91x_ro_temperature_set_count(COUNT_FREEZE);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("Temperature count freeze failed, Error Code: 0x%ld \n", status);
      break;
    }
    DEBUGOUT("Temperature count freeze done\n");
    // Temperature sensing enable bit is set.
    status = sl_si91x_ro_temperature_enable(SL_RO_TEMPERATURE_ENABLE);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("Temperature sensor enable failed, Error Code: 0x%ld \n", status);
      break;
    }
    DEBUGOUT("Temperature sensor enable done\n");
    // Enables periodic checking of temperature and triggers time for every 1second
    status = sl_si91x_ro_temperature_periodic_update(SL_RO_TEMPERATURE_PERIODIC_ENABLE, SL_RO_TEMPERATURE_TRIGGER_1SEC);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("Temperature updating periodically failed, Error Code: 0x%ld \n", status);
      break;
    }
    DEBUGOUT("Temperature updating periodically done\n");
    // Reads the reference clock count
    status = sl_si91x_ro_temperature_get_reference_clk_count(&ref_clk_cnt);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("Reading reference clock count failed, Error Code: 0x%ld \n", status);
      break;
    }
    DEBUGOUT("Reading reference clock count done\n");
    DEBUGOUT("ref_clk_cnt:%ld\n", ref_clk_cnt);
    // Reads the ptat clock count
    status = sl_si91x_ro_temperature_get_ptat_clk_count(&ptat_clk_cnt);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("Reading ptat clock count failed, Error Code: 0x%ld \n", status);
      break;
    }
    DEBUGOUT("Reading ptat clock count done\n");
    DEBUGOUT("ptat_clk:%ld\n", ptat_clk_cnt);
  } while (false);
}

/*******************************************************************************
 * Function will run continuously in while loop and reads temperature.
 ******************************************************************************/
void ro_temperature_sensor_example_process_action(void)
{
  sl_status_t status;
  int32_t temp = 0;
  if (delay_timeout == true) {
    delay_timeout = false;
    // Temperature sensing enable bit is set.
    status = sl_si91x_ro_temperature_enable(SL_RO_TEMPERATURE_ENABLE);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("Temperature sensor enable failed, Error Code: 0x%ld \n", status);
    }
    // Reads the temperature from RO sensor
    status = sl_si91x_ro_temperature_read(&temp);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("Temperature sensor read failed, Error Code: 0x%ld \n", status);
    }
    DEBUGOUT("RO sensor temperature:%ld\n", temp);
  }
}

/***************************************************************************/ /**
 * Sleeptimer timeout callback.
 ******************************************************************************/
static void on_timeout_timer1(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  (void)&handle;
  (void)&data;
  delay_timeout = true;
}
