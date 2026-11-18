/***************************************************************************/ /**
 * @file si70xx_example.c
 * @brief si70xx example APIs
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_si91x_si70xx.h"
#include "si70xx_example.h"
#include "sl_sleeptimer.h"
#include "sl_si91x_driver_gpio.h"
#include "sl_si91x_clock_manager.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/

#define TX_THRESHOLD       0                   // tx threshold value
#define RX_THRESHOLD       0                   // rx threshold value
#define I2C                SI70XX_I2C_INSTANCE // I2C instance
#define USER_REG_1         0xBA                // writing data into user register
#define DELAY_PERIODIC_MS1 2000                //sleeptimer1 periodic timeout in ms
#define MODE_0             0                   // Initializing GPIO MODE_0 value
#define OUTPUT_VALUE       1                   // GPIO output value
/* Si70xx datasheet: 5–15 ms after reset before I2C is ready. Driver does a single
 * attempt (no retry); application retries init until success or timeout. */
#define SI70XX_POST_RESET_READY_MS    15
#define SI70XX_INIT_RETRY_INTERVAL_MS 1

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
void delay(uint32_t idelay);
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
  i2c_config.transfer_type  = SL_I2C_USING_NON_DMA;
  i2c_config.operating_mode = SL_I2C_STANDARD_MODE;
  i2c_config.i2c_callback   = i2c_leader_callback;

  do {
#if defined(SENSOR_ENABLE_GPIO_MAPPED_TO_UULP)
    if (sl_si91x_gpio_driver_get_uulp_npss_pin(SENSOR_ENABLE_GPIO_PIN) != 1) {
      // Enable GPIO ULP_CLK
      status = sl_si91x_gpio_driver_enable_clock((sl_si91x_gpio_select_clock_t)ULPCLK_GPIO);
      if (status != SL_STATUS_OK) {
        /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */
        SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_enable_clock, Error code: %lu\r\n", status);
        break;
      }
      SL_PRINT_STRING_ERROR("GPIO driver clock enable is successful \r\n");
      // Set NPSS GPIO pin MUX
      status = sl_si91x_gpio_driver_set_uulp_npss_pin_mux(SENSOR_ENABLE_GPIO_PIN, NPSS_GPIO_PIN_MUX_MODE0);
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_set_uulp_npss_pin_mux, Error code: %lu\r\n", status);
        break;
      }
      SL_PRINT_STRING_ERROR("GPIO driver uulp pin mux selection is successful \r\n");
      // Set NPSS GPIO pin direction
      status =
        sl_si91x_gpio_driver_set_uulp_npss_direction(SENSOR_ENABLE_GPIO_PIN, (sl_si91x_gpio_direction_t)GPIO_OUTPUT);
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_set_uulp_npss_direction, Error code: %lu\r\n", status);
        break;
      }
      SL_PRINT_STRING_ERROR("GPIO driver uulp pin direction selection is successful \r\n");
      // Set UULP GPIO pin
      status = sl_si91x_gpio_driver_set_uulp_npss_pin_value(SENSOR_ENABLE_GPIO_PIN, SET);
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_set_uulp_npss_pin_value, Error code: %lu\r\n", status);
        break;
      }
      SL_PRINT_STRING_ERROR("GPIO driver set uulp pin value is successful \r\n");
    }
#else
    sl_gpio_t sensor_enable_port_pin = { SENSOR_ENABLE_GPIO_PORT, SENSOR_ENABLE_GPIO_PIN };
    uint8_t pin_value;

    status = sl_gpio_driver_get_pin(&sensor_enable_port_pin, &pin_value);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_gpio_driver_get_pin, Error code: %lu\r\n", status);
      break;
    }
    if (pin_value != 1) {
      // Enable GPIO CLK
#ifdef SENSOR_ENABLE_GPIO_MAPPED_TO_ULP
      status = sl_si91x_gpio_driver_enable_clock((sl_si91x_gpio_select_clock_t)ULPCLK_GPIO);
#else
      status = sl_si91x_gpio_driver_enable_clock((sl_si91x_gpio_select_clock_t)M4CLK_GPIO);
#endif
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_enable_clock, Error code: %lu\r\n", status);
        break;
      }
      SL_PRINT_STRING_ERROR("GPIO driver clock enable is successful \r\n");

      // Set the pin mode for GPIO pins.
      status = sl_gpio_driver_set_pin_mode(&sensor_enable_port_pin, MODE_0, OUTPUT_VALUE);
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("sl_gpio_driver_set_pin_mode, Error code: %lu, line %d\r\n", status, __LINE__);
        break;
      }
      SL_PRINT_STRING_ERROR("GPIO driver pin mode select is successful \r\n");
      // Select the direction of GPIO pin whether Input/ Output
      status = sl_si91x_gpio_driver_set_pin_direction(SENSOR_ENABLE_GPIO_PORT,
                                                      SENSOR_ENABLE_GPIO_PIN,
                                                      (sl_si91x_gpio_direction_t)GPIO_OUTPUT);
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_set_pin_direction, Error code: %lu, line %d\r\n", status, __LINE__);
        break;
      }
      // Set GPIO pin
      status = sl_gpio_driver_set_pin(&sensor_enable_port_pin); // Set ULP GPIO pin
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("sl_gpio_driver_set_pin, Error code: %lu, line %d\r\n", status, __LINE__);
        break;
      }
      SL_PRINT_STRING_ERROR("GPIO driver set pin value is successful \r\n");
    }
#endif

    /* Wait for sensor to become ready */
    delay(80);

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
      SL_PRINT_STRING_ERROR("sl_i2c_driver_init : Invalid Parameters, Error Code: 0x%ld \r\n", status);
      break;
    } else {
      SL_PRINT_STRING_ERROR("Successfully initialized and configured i2c leader\r\n");
    }
    status = sl_i2c_driver_configure_fifo_threshold(I2C, TX_THRESHOLD, RX_THRESHOLD);
    if (status != SL_I2C_SUCCESS) {
      SL_PRINT_STRING_ERROR("sl_i2c_driver_configure_fifo_threshold : Invalid Parameters, Error Code: 0x%ld \r\n",
                            status);
      break;
    } else {
      SL_PRINT_STRING_ERROR("Successfully configured i2c TX & RX FIFO thresholds\r\n");
    }
    // reset the sensor
    status = sl_si91x_si70xx_reset(I2C, SI70XX_SLAVE_ADDR);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("Sensor reset un-successful, Error Code: 0x%ld \r\n", status);
      break;
    } else {
      SL_PRINT_STRING_ERROR("Successfully reset sensor\r\n");
    }
    /* After reset, Si70xx needs 5–15 ms (datasheet). Retry init until success or timeout. */
    uint8_t retry_ms;
    for (retry_ms = 0; retry_ms < SI70XX_POST_RESET_READY_MS; retry_ms += SI70XX_INIT_RETRY_INTERVAL_MS) {
      sl_si91x_delay_ms(SI70XX_INIT_RETRY_INTERVAL_MS);
      status = sl_si91x_si70xx_init(I2C, SI70XX_SLAVE_ADDR, SL_EID_FIRST_BYTE);
      if (status == SL_STATUS_OK) {
        break;
      }
    }
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("Sensor initialization un-successful (no ACK after %u ms), Error Code: 0x%ld, line %d\r\n",
                            retry_ms,
                            status,
                            __LINE__);
      break;
    }
    SL_PRINT_STRING_ERROR("Successfully initialized sensor\r\n");
    // Initializes sensor and reads electronic ID 2nd byte (single attempt)
    status = sl_si91x_si70xx_init(I2C, SI70XX_SLAVE_ADDR, SL_EID_SECOND_BYTE);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("Sensor initialization un-successful, Error Code: 0x%ld, line %d\r\n", status, __LINE__);
      break;
    } else {
      SL_PRINT_STRING_ERROR("Successfully reset sensor\r\n");
    }
    // Get sensor internal firmware version of sensor
    status = sl_si91x_si70xx_get_firmware_revision(I2C, SI70XX_SLAVE_ADDR, &firm_rev);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("Sensor firmware version un-successful, Error Code: 0x%ld, line %d\r\n", status, __LINE__);
      break;
    } else {
      SL_PRINT_STRING_ERROR("Successfully firmware version of sensor is read\r\n");
    }
    SL_PRINT_STRING_ERROR("firmware version:%x\r\n", firm_rev);
    // write register data into sensor
    status = sl_si91x_si70xx_write_control_register(I2C, SI70XX_SLAVE_ADDR, SL_RH_T_USER_REG, USER_REG_1);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("Sensor user register 1 write data failed, Error Code: 0x%ld, line %d\r\n",
                            status,
                            __LINE__);
      break;
    } else {
      SL_PRINT_STRING_ERROR("Sensor user register 1 write data is successful\r\n");
    }
    // Reads register data from sensor
    status = sl_si91x_si70xx_read_control_register(I2C, SI70XX_SLAVE_ADDR, SL_RH_T_USER_REG, &value);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("Sensor user register 1 read failed, Error Code: 0x%ld, line %d\r\n", status, __LINE__);
      break;
    } else {
      SL_PRINT_STRING_ERROR("Sensor user register 1 read is successful\r\n");
    }
    SL_PRINT_STRING_ERROR("user register data:%x\r\n", value);
    // Reads temperature from humidity from sensor
    status = sl_si91x_si70xx_read_temp_from_rh(I2C, SI70XX_SLAVE_ADDR, &humidity, &temperature);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("Sensor temperature read failed, Error Code: 0x%ld, line %d\r\n", status, __LINE__);
      break;
    } else {
      SL_PRINT_STRING_ERROR("Sensor temperature read is successful\r\n");
    }
    SL_PRINT_STRING_ERROR("sensor humidity :%ld\r\n", humidity);
    SL_PRINT_STRING_ERROR("sensor temperature :%ld\r\n", temperature);
    // measure humidity data from sensor
    status = sl_si91x_si70xx_measure_humidity(I2C, SI70XX_SLAVE_ADDR, &humidity);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("Sensor humidity read failed, Error Code: 0x%ld, line %d\r\n", status, __LINE__);
      break;
    } else {
      SL_PRINT_STRING_ERROR("Sensor humidity read is successful\r\n");
    }
    SL_PRINT_STRING_ERROR("sensor humidity :%ld\r\n", humidity);
    // measure temperature data from sensor
    status = sl_si91x_si70xx_measure_temperature(I2C, SI70XX_SLAVE_ADDR, &temperature);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("Sensor temperature read failed, Error Code: 0x%ld, line %d\r\n", status, __LINE__);
      break;
    } else {
      SL_PRINT_STRING_ERROR("Sensor temperature read is successful\r\n");
    }
    SL_PRINT_STRING_ERROR("sensor temperature :%ld\r\n", temperature);
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
    status = sl_si91x_si70xx_measure_rh_and_temp(I2C, SI70XX_SLAVE_ADDR, &humidity, &temperature);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("Sensor temperature read failed, Error Code: 0x%ld, line %d\r\n", status, __LINE__);
    } else {
      SL_PRINT_STRING_ERROR("Sensor temperature read is successful\r\n");
    }
    SL_PRINT_STRING_ERROR("sensor humidity :%ld\r\n", humidity);
    SL_PRINT_STRING_ERROR("sensor temperature :%ld\r\n", temperature);
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
/*******************************************************************************
* Function to provide 1 ms Delay
*******************************************************************************/
void delay(uint32_t idelay)
{
  for (uint32_t x = 0; x < 4600 * idelay; x++) //1.002ms delay
  {
    __NOP();
  }
}
