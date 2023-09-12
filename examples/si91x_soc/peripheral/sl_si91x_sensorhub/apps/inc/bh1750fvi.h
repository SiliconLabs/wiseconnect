/*
 * bh1750.h
 *
 *  Created on: Mar 12, 2023
 *      Author: kokuncha
 */

#ifndef BH1750FVI_H_
#define BH1750FVI_H_
#include <stdbool.h>
#include "rsi_data_types.h"
typedef void *sl_err_t;

typedef enum {
  BH1750_ID = 0x01, /*!< BH1750 light sensor id*/
  VEML6040_ID,      /*!< VEML6040 light sensor id*/
  VEML6075_ID,      /*!< VEML6075 light sensor id*/
  LIGHT_MAX_ID,     /*!< max light sensor id*/
} light_sensor_id_t;

typedef void *bh1750_handle_t;
typedef void *i2c_bus_handle_t;
typedef void *i2c_bus_device_handle_t;
typedef enum {
  BH1750_CONTINUE_1LX_RES    = 0x10, /*!< Command to set measure mode as Continuously H-Resolution mode*/
  BH1750_CONTINUE_HALFLX_RES = 0x11, /*!< Command to set measure mode as Continuously H-Resolution mode2*/
  BH1750_CONTINUE_4LX_RES    = 0x13, /*!< Command to set measure mode as Continuously L-Resolution mode*/
  BH1750_ONETIME_1LX_RES     = 0x20, /*!< Command to set measure mode as One Time H-Resolution mode*/
  BH1750_ONETIME_HALFLX_RES  = 0x21, /*!< Command to set measure mode as One Time H-Resolution mode2*/
  BH1750_ONETIME_4LX_RES     = 0x23, /*!< Command to set measure mode as One Time L-Resolution mode*/
} bh1750_cmd_measure_t;

/**
 * @brief light sensor id, used for light_sensor_create
 *
 */

#define BH1750_I2C_ADDRESS_DEFAULT (0x23)

static bh1750_handle_t bh1750 = NULL;
static bool is_init           = false;
#define BH_1750_MEASUREMENT_ACCURACY 1.2 /*!< the typical measurement accuracy of  BH1750 sensor */

typedef struct {
  i2c_bus_device_handle_t i2c_dev;
  uint8_t dev_addr;
} bh1750_dev_t;

#ifdef __cplusplus
extern "C" {
#endif

bh1750_handle_t bh1750_create(i2c_bus_handle_t bus, uint8_t dev_addr);

sl_err_t bh1750_delete(bh1750_handle_t *sensor);

sl_err_t bh1750_power_down(bh1750_handle_t sensor);

sl_err_t bh1750_power_on(bh1750_handle_t sensor);

sl_err_t bh1750_reset_data_register(bh1750_handle_t sensor);

sl_err_t bh1750_set_measure_mode(bh1750_handle_t sensor, bh1750_cmd_measure_t cmd_measure);

sl_err_t bh1750_get_data(bh1750_handle_t sensor, float *data);

sl_err_t bh1750_get_light_intensity(bh1750_handle_t sensor, bh1750_cmd_measure_t cmd_measure, float *data);

sl_err_t bh1750_change_measure_time(bh1750_handle_t sensor, uint8_t measure_time);

sl_err_t bh1750_init(i2c_bus_handle_t handle);

sl_err_t bh1750_deinit(void);

sl_err_t bh1750_sample_light(float *);
//sl_err_t bh1750_sample_light(unsigned int * );

sl_err_t bh1750_test(void);

#ifdef __cplusplus
}
#endif

#endif /* BH1750FVI_H_ */
