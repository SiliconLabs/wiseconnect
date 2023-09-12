/*
 * lightsensor_hal.h
 *
 *  Created on: Mar 13, 2023
 *      Author: kokuncha
 */

#ifndef LIGHTSENSOR_HAL_H_
#define LIGHTSENSOR_HAL_H_
#include "sensor_type.h"

typedef void *sl_error_t;

typedef void *bus_handle_t;

typedef void *sensor_light_handle_t; /*!< light sensor handle*/
#define CONFIG_SENSOR_LIGHT_INCLUDED_BH1750 1

#ifdef CONFIG_SENSOR_LIGHT_INCLUDED_BH1750
#include "bh1750fvi.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a light sensor instance.
 * same series' sensor or sensor with same address can only be created once.
 *
 * @param bus i2c bus handle the sensor attached to
 * @param id id declared in light_sensor_id_t
 * @return sensor_light_handle_t return light sensor handle if succeed, return NULL if failed.
 */
sensor_light_handle_t light_sensor_create(bus_handle_t bus, int id);

/**
 * @brief Delete and release the sensor resource.
 *
 * @param sensor point to light sensor handle, will set to NULL if delete succeed.
 * @return sl_error_t
 *     - RSI_OK Success
 *     - RSI_FAIL Fail
*/
sl_error_t light_sensor_delete(sensor_light_handle_t *sensor);

/**
 * @brief Test if sensor is active.
 *
 * @param sensor light sensor handle to operate.
 * @return sl_error_t
 *     - RSI_OK Success
 *     - RSI_FAIL Fail
*/
sl_error_t light_sensor_test(sensor_light_handle_t sensor);

/**
 * @brief Acquire light sensor illuminance result one time.
 *
 * @param sensor light sensor handle to operate.
 * @param lux result data (unit:lux)
 * @return sl_error_t
 *     - RSI_OK Success
 *     - RSI_FAIL Fail
 *     - RSI_ERR_NOT_SUPPORTED Function not supported on this sensor
*/
sl_error_t light_sensor_sample_light(sensor_light_handle_t sensor, float *lux);

/**
 * @brief Acquire light sensor color result one time.
 * light color includes red green blue and white.
 *
 * @param sensor light sensor handle to operate.
 * @param rgbw result data (unit:lux)
 * @return sl_error_t
 *     - RSI_OK Success
 *     - RSI_FAIL Fail
 *     - RSI_ERR_NOT_SUPPORTED Function not supported on this sensor
*/
sl_error_t light_sensor_sample_rgbw(sensor_light_handle_t sensor, rgbw_t *rgbw);

/**
 * @brief Acquire light sensor ultra violet result one time.
 * light Ultraviolet includes UVA UVB and UV.
 *
 * @param sensor light sensor handle to operate.
 * @param uv result data (unit:lux)
 * @return sl_error_t
 *     - RSI_OK Success
 *     - RSI_FAIL Fail
 *     - RSI_ERR_NOT_SUPPORTED Function not supported on this sensor
*/
sl_error_t light_sensor_sample_uv(sensor_light_handle_t sensor, uv_t *uv);

/**
 * @brief Set sensor to sleep mode.
 *
 * @param sensor light sensor handle to operate.
 * @return sl_error_t
 *     - RSI_OK Success
 *     - RSI_FAIL Fail
 *     - RSI_ERR_NOT_SUPPORTED Function not supported on this sensor
*/
sl_error_t light_sensor_sleep(sensor_light_handle_t sensor);

/**
 * @brief Wakeup sensor from sleep mode.
 *
 * @param sensor light sensor handle to operate.
 * @return sl_error_t
 *     - RSI_OK Success
 *     - RSI_FAIL Fail
 *     - RSI_ERR_NOT_SUPPORTED Function not supported on this sensor
*/
sl_error_t light_sensor_wakeup(sensor_light_handle_t sensor);

/**
 * @brief acquire a group of sensor data
 *
 * @param sensor light sensor handle to operate
 * @param data_group acquired data
 * @return sl_error_t
 *     - RSI_OK Success
 *     - RSI_FAIL Fail
 */
sl_error_t light_sensor_sample(sensor_light_handle_t sensor, sensor_data_group_t *data_group);

/**
 * @brief control sensor mode with control commands and args
 *
 * @param sensor light sensor handle to operate
 * @param cmd control commands detailed in sensor_command_t
 * @param args control commands args
 *     - RSI_OK Success
 *     - RSI_FAIL Fail
 *     - RSI_ERR_NOT_SUPPORTED Function not supported on this sensor
 */
sl_error_t light_sensor_control(sensor_light_handle_t sensor, sensor_command_t cmd, void *args);

#ifdef __cplusplus
extern "C"
}
#endif

#endif /* LIGHTSENSOR_HAL_H_ */
