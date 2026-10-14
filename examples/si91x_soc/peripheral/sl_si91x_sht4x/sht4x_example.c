/***************************************************************************/ /**
 * @file sht4x_example.c
 * @brief SHT4x example using I2CSPM compat layer and platform SHT4x driver
 *
 * This example uses the I2CSPM compatibility layer so that the unchanged
 * platform SHT4x driver (sl_sht4x.c/h) runs on WiseConnect over sl_si91x_i2c.
 *******************************************************************************
 * # License
 * <b>Copyright 2026 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <inttypes.h>
#include "rsi_debug.h"
#include "sl_sht4x.h"
#include "sl_i2cspm.h"
#include "sl_si91x_sht4x_instances.h"
#include "sl_sleeptimer.h"
#include "sl_si91x_driver_gpio.h"
#include "sl_si91x_clock_manager.h"

/*******************************************************************************
  ***************************  Defines / Macros  ********************************
  ******************************************************************************/
/* I2C instance for SHT4x sensor.
 * IMPORTANT: This must match the I2C instance configured in the i2c_instance
 * component in the project's .slcp file. By default, i2c2 (ULP_I2C) is used.
 * If you change the instance in UC, update this define accordingly:
 *   - For I2C0: use SL_I2C0
 *   - For I2C1: use SL_I2C1
 *   - For I2C2: use SL_ULP_I2C
 */
#ifndef SHT4X_I2C_INSTANCE
#define SHT4X_I2C_INSTANCE SL_ULP_I2C // ULP I2C (I2C2) - default
#endif
#define TX_THRESHOLD        0                  // tx threshold value
#define RX_THRESHOLD        0                  // rx threshold value
#define I2C                 SHT4X_I2C_INSTANCE // I2C instance
#define SHT4X_INIT_RETRIES  10                 // retries for sl_sht4x_init
#define SHT4X_INIT_RETRY_MS 1                  // delay between init retries
#define MODE_0              0                  // GPIO MODE_0 value
#define OUTPUT_VALUE        1                  // GPIO output value

/*******************************************************************************
  ******************************  Data Types  ***********************************
  ******************************************************************************/
typedef sl_i2c_config_t sl_i2c_configuration_t;

/*******************************************************************************
  * I2CSPM handle for platform SHT4x driver (declared in sl_si91x_sht4x_instances.h)
  ******************************************************************************/
static sl_i2c_instance_t _i2c_sensor = I2C;
sl_i2cspm_t *sl_si91x_sht4x_sensor   = (sl_i2cspm_t *)&_i2c_sensor;

/*******************************************************************************
  **********************  Local Function prototypes   ***************************
  ******************************************************************************/
static void i2c_leader_callback(sl_i2c_instance_t i2c_instance, uint32_t status);

/*******************************************************************************
  **************************   GLOBAL FUNCTIONS   *******************************
  ******************************************************************************/
/*******************************************************************************
  * SHT4x example initialization function
  ******************************************************************************/
void sht4x_example_init(void)
{
  sl_status_t status                = SL_STATUS_OK;
  sl_i2c_configuration_t i2c_config = { 0 };
  uint32_t rh_data                  = 0;
  int32_t temp_data                 = 0;

  i2c_config.mode           = SL_I2C_LEADER_MODE;
  i2c_config.transfer_type  = SL_I2C_USING_NON_DMA;
  i2c_config.operating_mode = SL_I2C_STANDARD_MODE;
  i2c_config.i2c_callback   = i2c_leader_callback;

  do {
#if defined(SENSOR_ENABLE_GPIO_MAPPED_TO_UULP)
    if (sl_si91x_gpio_driver_get_uulp_npss_pin(SENSOR_ENABLE_GPIO_PIN) != 1) {
      status = sl_si91x_gpio_driver_enable_clock((sl_si91x_gpio_select_clock_t)ULPCLK_GPIO);
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_enable_clock, Error code: 0x%lx \n", status);
        break;
      }
      status = sl_si91x_gpio_driver_set_uulp_npss_pin_mux(SENSOR_ENABLE_GPIO_PIN, NPSS_GPIO_PIN_MUX_MODE0);
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_set_uulp_npss_pin_mux, Error code: 0x%lx \n", status);
        break;
      }
      status =
        sl_si91x_gpio_driver_set_uulp_npss_direction(SENSOR_ENABLE_GPIO_PIN, (sl_si91x_gpio_direction_t)GPIO_OUTPUT);
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_set_uulp_npss_direction, Error code: 0x%lx \n", status);
        break;
      }
      status = sl_si91x_gpio_driver_set_uulp_npss_pin_value(SENSOR_ENABLE_GPIO_PIN, SET);
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_set_uulp_npss_pin_value, Error code: 0x%lx \n", status);
        break;
      }
    }
#elif defined(SENSOR_ENABLE_GPIO_PORT)
    sl_gpio_t sensor_enable_port_pin = { SENSOR_ENABLE_GPIO_PORT, SENSOR_ENABLE_GPIO_PIN };
    uint8_t pin_value;

    status = sl_gpio_driver_get_pin(&sensor_enable_port_pin, &pin_value);
    if (status != SL_STATUS_OK) {
      /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */
      SL_PRINT_STRING_ERROR("sl_gpio_driver_get_pin, Error code: 0x%lx \n", status);
      break;
    }
    if (pin_value != 1) {
#ifdef SENSOR_ENABLE_GPIO_MAPPED_TO_ULP
      status = sl_si91x_gpio_driver_enable_clock((sl_si91x_gpio_select_clock_t)ULPCLK_GPIO);
#else
      status = sl_si91x_gpio_driver_enable_clock((sl_si91x_gpio_select_clock_t)M4CLK_GPIO);
#endif
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_enable_clock, Error code: 0x%lx \n", status);
        break;
      }
      status = sl_gpio_driver_set_pin_mode(&sensor_enable_port_pin, MODE_0, OUTPUT_VALUE);
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("sl_gpio_driver_set_pin_mode, Error code: 0x%lx \n", status);
        break;
      }
      status = sl_si91x_gpio_driver_set_pin_direction(SENSOR_ENABLE_GPIO_PORT,
                                                      SENSOR_ENABLE_GPIO_PIN,
                                                      (sl_si91x_gpio_direction_t)GPIO_OUTPUT);
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_set_pin_direction, Error code: 0x%lx \n", status);
        break;
      }
      status = sl_gpio_driver_set_pin(&sensor_enable_port_pin);
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("sl_gpio_driver_set_pin, Error code: 0x%lx \n", status);
        break;
      }
    }
#endif

    (void)sl_i2c_driver_deinit(I2C);

    /* Initialize I2C bus */
    status = (sl_status_t)sl_i2c_driver_init(I2C, &i2c_config);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_i2c_driver_init : Invalid Parameters, Error Code: 0x%lx \n", status);
      break;
    }

    status = (sl_status_t)sl_i2c_driver_configure_fifo_threshold(I2C, TX_THRESHOLD, RX_THRESHOLD);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_i2c_driver_configure_fifo_threshold : Invalid Parameters, Error Code: 0x%lx \n",
                            status);
      break;
    }

    /* Initialize SHT4x with retries (sensor may need time after power-on) */
    for (uint8_t attempt = 0; attempt < SHT4X_INIT_RETRIES; attempt++) {
      if (attempt > 0) {
        sl_si91x_delay_ms(SHT4X_INIT_RETRY_MS);
      }
      status = sl_sht4x_init(sl_si91x_sht4x_sensor, SHT4X_ADDR);
      if (status == SL_STATUS_OK) {
        break;
      }
    }
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("Sensor initialization un-successful after %d tries, Error Code: 0x%lx \n",
                            (int)SHT4X_INIT_RETRIES,
                            status);
      SL_PRINT_STRING_ERROR("  (0x31 = I2C NACK: no response at 0x44 - check SDA/SCL, VDD/GND, pull-ups)\n");
      break;
    }

    /* Read one measurement to verify sensor */
    status = sl_sht4x_measure_rh_and_temp(sl_si91x_sht4x_sensor, SHT4X_ADDR, &rh_data, &temp_data);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("Sensor first measurement failed, Error Code: 0x%lx \n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("Sensor first measurement is successful\n");
    SL_PRINT_STRING_ERROR("sensor humidity : %lu.%03lu %%\n", (rh_data / 1000), (rh_data % 1000));
    SL_PRINT_STRING_ERROR("sensor temperature : %ld.%03ld C\n",
                          (long)(temp_data / 1000),
                          (long)(temp_data < 0 ? (-temp_data) % 1000 : temp_data % 1000));
  } while (false);
}

/*******************************************************************************
  * Process action: reads relative humidity and temperature from sensor
  ******************************************************************************/
void sht4x_example_process_action(void)
{
  sl_status_t status = SL_STATUS_OK;
  uint32_t rh_data   = 0; /* permille (‰), driver returns RH * 1000 */
  int32_t temp_data  = 0; /* millidegrees C */
  status             = sl_sht4x_measure_rh_and_temp(sl_si91x_sht4x_sensor, SHT4X_ADDR, &rh_data, &temp_data);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Sensor temperature read failed, Error Code: 0x%lx \n", status);
    /* Delay even on error to throttle retries and maintain 1-second sampling rate */
    sl_si91x_delay_ms(1000);
    return;
  }
  SL_PRINT_STRING_ERROR("Sensor temperature read is successful\n");
  SL_PRINT_STRING_ERROR("sensor humidity : %lu.%03lu %%\n", (rh_data / 1000), (rh_data % 1000));
  SL_PRINT_STRING_ERROR("sensor temperature : %ld.%03ld C\n",
                        (long)(temp_data / 1000),
                        (long)(temp_data < 0 ? (-temp_data) % 1000 : temp_data % 1000));
  /* Delay 1 s so we read sensor every one second. */
  sl_si91x_delay_ms(1000);
}

/*******************************************************************************
  * I2C leader callback
  ******************************************************************************/
static void i2c_leader_callback(sl_i2c_instance_t i2c_instance, uint32_t status)
{
  (void)i2c_instance;
  switch (status) {
    case SL_I2C_DATA_TRANSFER_COMPLETE:
      break;
    default:
      break;
  }
}
