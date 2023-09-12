/*
 * hub_hal_intf.h
 *
 *  Created on: Mar 14, 2023
 *      Author: kokuncha
 */

#ifndef HUB_HAL_INTF_H_
#define HUB_HAL_INTF_H_

#include "sensor_type.h"
#include "lightsensor_hal.h"

#define CONFIG_SENSOR_INCLUDED_LIGHT

typedef enum { I2C, SPI, UART, ADC, GPIO } sensor_bus_en;

typedef struct {
  sensor_type_t type;                                                           /* sensor type  */
  sensor_driver_handle_t (*create)(sensor_bus_en, int sensor_id);               /* create a sensor  */
  int32_t (*delete)(sensor_driver_handle_t *);                                  /*delete a sensor  */
  int32_t (*sample)(sensor_driver_handle_t, sensor_data_group_t *);             /* acquire a group of sensor data  */
  int32_t (*control)(sensor_driver_handle_t, sensor_command_t cmd, void *args); /* modify the sensor configuration  */
} sl_sensor_impl_type;

uint32_t get_impl_size();

#endif /* HUB_HAL_INTF_H_ */
