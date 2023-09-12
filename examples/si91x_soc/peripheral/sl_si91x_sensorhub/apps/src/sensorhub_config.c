/*
 * sensorhub_config.c
 *
 *  Created on: Mar 6, 2023
 *      Author: kokuncha
 */
#include "sensor_hub.h"

sl_sensor_info_t sensor_info[MAX_NUM_SENSORS] = {
  {
    .name                 = "LIGHT_SENSOR",
    .bus                  = I2C,
    .sensor_id            = SENSOR_BH1750_ID,
    .mode                 = POLLING_MODE,
    .sampling_interval    = 100,
    .addr                 = BH1750_ID,
    .data_deliver.mode    = TIMEOUT,
    .data_deliver.timeout = 1000,
  },
  { .name                   = "button",
    .bus                    = GPIO,
    .sensor_id              = GPIO_SENSE_BUTTON_ID,
    .mode                   = INTERRUPT_MODE,
    .sampling_intr_req_pin  = BUTTON_0_GPIO_PIN,
    .sampling_interval      = 100,
    .data_deliver.mode      = NO_DATA,
    .data_deliver.threshold = 0 },
};

/* bus configuration */
const bus_intf_config_t bus_intf_info = {
  .i2c_config.i2c_bus_speed    = ARM_I2C_BUS_SPEED_FAST,
  .i2c_config.i2c_power_state  = ARM_POWER_FULL,
  .i2c_config.i2c_control_mode = ARM_I2C_BUS_SPEED,

  .spi_config.spi_bit_width = NULL,
  .spi_config.spi_baud      = NULL,
  .spi_config.spi_mode      = NULL, //!phase and polarity
  .spi_config.spi_cs_mode   = NULL,
};
