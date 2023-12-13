/***************************************************************************/ /**
 * @file si70xx_example.c
 * @brief si70xx example APIs
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
#include "sl_si91x_si70xx.h"
#include "si70xx_example.h"
#include "sl_sleeptimer.h"
/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define TX_THRESHOLD       0       // tx threshold value
#define RX_THRESHOLD       0       // rx threshold value
#define I2C                SL_I2C2 // I2C 2 instance
#define USER_REG_1         0xBA    // writing data into user register
#define DELAY_PERIODIC_MS1 2000    //sleeptimer1 periodic timeout in ms
/*******************************************************************************
 ******************************  Data Types  ***********************************
 ******************************************************************************/
/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
typedef sl_i2c_config_t sl_i2c_configuration_t;
sl_sleeptimer_timer_handle_t timer1; //sleeptimer1 handle
boolean_t delay_timeout = false;     //Indicates sleeptimer1 timeout
/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void i2c_leader_callback(sl_i2c_instance_t i2c_instance, uint32_t status);
//Sleeptimer timeout callbacks
static void on_timeout_timer1(sl_sleeptimer_timer_handle_t *handle, void *data);
/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/*******************************************************************************
 * RHT example initialization function
 ******************************************************************************/
void si70xx_example_init(void)
{
  sl_status_t status;
  uint8_t firm_rev;
  sl_i2c_configuration_t i2c_config;
  uint32_t humidity;
  int32_t temperature;
  uint8_t value;
  i2c_config.mode           = SL_I2C_LEADER_MODE;
  i2c_config.transfer_type  = SL_I2C_USING_INTERRUPT;
  i2c_config.operating_mode = SL_I2C_STANDARD_MODE;
  i2c_config.i2c_callback   = i2c_leader_callback;
  do {
    //Start 2000 ms periodic timer
    sl_sleeptimer_start_periodic_timer_ms(&timer1,
                                          DELAY_PERIODIC_MS1,
                                          on_timeout_timer1,
                                          NULL,
                                          0,
                                          SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
    // Initialize I2C bus
    status = sl_i2c_driver_init(I2C, &i2c_config);
    if (status != SL_I2C_SUCCESS) {
      DEBUGOUT("sl_i2c_driver_init : Invalid Parameters, Error Code: 0x%ld \n", status);
      break;
    } else {
      DEBUGOUT("Successfully initialized and configured i2c leader\n");
    }
    status = sl_i2c_driver_configure_fifo_threshold(I2C, TX_THRESHOLD, RX_THRESHOLD);
    if (status != SL_I2C_SUCCESS) {
      DEBUGOUT("sl_i2c_driver_configure_fifo_threshold : Invalid Parameters, Error Code: 0x%ld \n", status);
      break;
    } else {
      DEBUGOUT("Successfully configured i2c TX & RX FIFO thresholds\n");
    }
    // reset the sensor
    status = sl_si91x_si70xx_reset(I2C, SI7021_ADDR);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("Sensor reset un-successful, Error Code: 0x%ld \n", status);
      break;
    } else {
      DEBUGOUT("Successfully reset sensor\n");
    }
    // Initializes sensor and reads electronic ID 1st byte
    status = sl_si91x_si70xx_init(I2C, SI7021_ADDR, SL_EID_FIRST_BYTE);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("Sensor initialization un-successful, Error Code: 0x%ld \n", status);
      break;
    } else {
      DEBUGOUT("Successfully initialized sensor\n");
    }
    // Initializes sensor and reads electronic ID 2nd byte
    status = sl_si91x_si70xx_init(I2C, SI7021_ADDR, SL_EID_SECOND_BYTE);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("Sensor initialization un-successful, Error Code: 0x%ld \n", status);
      break;
    } else {
      DEBUGOUT("Successfully reset sensor\n");
    }
    // Get sensor internal firmware version of sensor
    status = sl_si91x_si70xx_get_firmware_revision(I2C, SI7021_ADDR, &firm_rev);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("Sensor firmware version un-successful, Error Code: 0x%ld \n", status);
      break;
    } else {
      DEBUGOUT("Successfully firmware version of sensor is read\n");
    }
    DEBUGOUT("firmware version:%x\n", firm_rev);
    // write register data into sensor
    status = sl_si91x_si70xx_write_control_register(I2C, SI7021_ADDR, SL_RH_T_USER_REG, USER_REG_1);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("Sensor user register 1 write data failed, Error Code: 0x%ld \n", status);
      break;
    } else {
      DEBUGOUT("Sensor user register 1 write data is successful\n");
    }
    // Reads register data from sensor
    status = sl_si91x_si70xx_read_control_register(I2C, SI7021_ADDR, SL_RH_T_USER_REG, &value);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("Sensor user register 1 read failed, Error Code: 0x%ld \n", status);
      break;
    } else {
      DEBUGOUT("Sensor user register 1 read is successful\n");
    }
    DEBUGOUT("user register data:%x\n", value);
    // Reads temperature from humidity from sensor
    status = sl_si91x_si70xx_read_temp_from_rh(I2C, SI7021_ADDR, &humidity, &temperature);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("Sensor temperature read failed, Error Code: 0x%ld \n", status);
      break;
    } else {
      DEBUGOUT("Sensor temperature read is successful\n");
    }
    DEBUGOUT("sensor humdity :%ld\n", humidity);
    DEBUGOUT("sensor temperature :%ld\n", temperature);
    // measure humidity data from sensor
    status = sl_si91x_si70xx_measure_humidity(I2C, SI7021_ADDR, &humidity);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("Sensor humidity read failed, Error Code: 0x%ld \n", status);
      break;
    } else {
      DEBUGOUT("Sensor humidity read is successful\n");
    }
    DEBUGOUT("sensor humdity :%ld\n", humidity);
    // measure temperature data from sensor
    status = sl_si91x_si70xx_measure_temperature(I2C, SI7021_ADDR, &temperature);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("Sensor temperature read failed, Error Code: 0x%ld \n", status);
      break;
    } else {
      DEBUGOUT("Sensor temperature read is successful\n");
    }
    DEBUGOUT("sensor temperature :%ld\n", temperature);
  } while (false);
}

/*******************************************************************************
 * Function will run continuously in while loop and reads relative humidity and
 * temperature from sensor
 ******************************************************************************/
void si70xx_example_process_action(void)
{
  sl_status_t status;
  uint32_t humidity   = 0;
  int32_t temperature = 0;
  if (delay_timeout == true) {
    delay_timeout = false;
    // Reads humidity(hold master mode) measurement from sensor
    status = sl_si91x_si70xx_measure_rh_and_temp(I2C, SI7021_ADDR, &humidity, &temperature);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("Sensor temperature read failed, Error Code: 0x%ld \n", status);
    } else {
      DEBUGOUT("Sensor temperature read is successful\n");
    }
    DEBUGOUT("sensor humdity :%ld\n", humidity);
    DEBUGOUT("sensor temperature :%ld\n", temperature);
  }
}

/*******************************************************************************
 * Callback Function
 ******************************************************************************/
void i2c_leader_callback(sl_i2c_instance_t i2c_instance, uint32_t status)
{
  (void)i2c_instance;
  switch (status) {
    case SL_I2C_DATA_TRANSFER_COMPLETE:
      break;
    default:
      break;
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