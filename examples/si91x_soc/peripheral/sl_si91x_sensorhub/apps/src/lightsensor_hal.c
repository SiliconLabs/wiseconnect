/*
 * lightsensor_hal.c
 *
 *  Created on: Mar 13, 2023
 *      Author: kokuncha
 */

#include <stdio.h>
#include "rsi_board.h"
#include "sensor_hub.h"
#include "sensor_type.h"

typedef struct {
  light_sensor_id_t id;
  sl_error_t (*init)(bus_handle_t);
  sl_error_t (*deinit)(void);
  sl_error_t (*test)(void);
  sl_error_t (*sample_light)(unsigned int *);
  sl_error_t (*sample_rgbw)(float *r, float *g, float *b, float *w);
  sl_error_t (*sample_uv)(float *uv, float *uva, float *uvb);
  sl_error_t (*sleep)(void);
  sl_error_t (*wakeup)(void);
} light_sensor_impl_t;

typedef struct {
  light_sensor_id_t id;
  bus_handle_t bus;
  bool is_init;
  const light_sensor_impl_t *impl;
} sensor_light_t;

static const light_sensor_impl_t light_sensor_implementations[] = {
#ifdef CONFIG_SENSOR_LIGHT_INCLUDED_BH1750
  {
    .id           = BH1750_ID,
    .init         = bh1750_init,
    .deinit       = bh1750_deinit,
    .test         = bh1750_test,
    .sample_light = bh1750_sample_light,
    .sample_rgbw  = NULL,
    .sample_uv    = NULL,
    .sleep        = NULL,
    .wakeup       = NULL,
  },
#endif
};

static const light_sensor_impl_t *find_implementation(int id)
{
  const light_sensor_impl_t *active_driver = NULL;
  int count                                = sizeof(light_sensor_implementations) / sizeof(light_sensor_impl_t);
  for (int i = 0; i < count; i++) {
    if (light_sensor_implementations[i].id == id) {
      active_driver = &light_sensor_implementations[i];
      break;
    }
  }
  return active_driver;
}

sensor_light_handle_t light_sensor_create(bus_handle_t bus, int id)
{
  const light_sensor_impl_t *sensor_impl = find_implementation(id);

  if (sensor_impl == NULL) {
    DEBUGOUT("no driver founded, LIGHT ID = %d", id);
    return NULL;
  }
  sensor_light_t *p_sensor = (sensor_light_t *)pvPortMalloc(sizeof(sensor_light_t));
  if (p_sensor == NULL) {
    DEBUGOUT("\r\n light sensor create failed:%u \r\n", sizeof(sensor_light_t));
    return NULL;
  }
  p_sensor->id   = id;
  p_sensor->bus  = bus;
  p_sensor->impl = sensor_impl;
  sl_error_t ret = p_sensor->impl->init(bus);
  if (ret != RSI_OK) {
    free(p_sensor);
    DEBUGOUT("light sensor init failed");
    return NULL;
  }
  p_sensor->is_init = true;
  return (sensor_light_handle_t)p_sensor;
}

sl_error_t light_sensor_delete(sensor_light_handle_t *sensor)
{
  if (sensor == NULL || *sensor == NULL) {
    return RSI_FAIL;
  }
  sensor_light_t *p_sensor = (sensor_light_t *)(*sensor);

  if (!p_sensor->is_init) {
    free(p_sensor);
    return RSI_OK;
  }

  p_sensor->is_init = false;
  sl_error_t ret    = p_sensor->impl->deinit();
  if (ret != RSI_OK) {
    return RSI_FAIL;
  }

  free(p_sensor);
  *sensor = NULL;
  return RSI_OK;
}

sl_error_t light_sensor_test(sensor_light_handle_t sensor)
{
  if (sensor == NULL) {
    return RSI_FAIL;
  }
  sensor_light_t *p_sensor = (sensor_light_t *)(sensor);
  if (!p_sensor->is_init) {
    return RSI_FAIL;
  }
  sl_error_t ret = p_sensor->impl->test();
  return ret;
}

sl_error_t light_sensor_sample_light(sensor_light_handle_t sensor, float *lux)
{
  if (sensor == NULL || lux == NULL) {
    return RSI_FAIL;
  }
  sensor_light_t *p_sensor = (sensor_light_t *)(sensor);
  sl_error_t ret           = p_sensor->impl->sample_light(lux);
  return ret;
}

sl_error_t light_sensor_sample_rgbw(sensor_light_handle_t sensor, rgbw_t *rgbw)
{
  if (sensor == NULL || rgbw == NULL) {
    return RSI_FAIL;
  }
  sensor_light_t *p_sensor = (sensor_light_t *)(sensor);
  sl_error_t ret           = p_sensor->impl->sample_rgbw(&rgbw->r, &rgbw->g, &rgbw->b, &rgbw->w);
  return ret;
}

sl_error_t light_sensor_sample_uv(sensor_light_handle_t sensor, uv_t *uv)
{
  if (sensor == NULL || uv == NULL) {
    return RSI_FAIL;
  }
  sensor_light_t *p_sensor = (sensor_light_t *)(sensor);
  sl_error_t ret           = p_sensor->impl->sample_uv(&uv->uv, &uv->uva, &uv->uvb);
  return ret;
}

sl_error_t light_sensor_sleep(sensor_light_handle_t sensor)
{
  if (sensor == NULL) {
    return RSI_FAIL;
  }
  sensor_light_t *p_sensor = (sensor_light_t *)(sensor);
  sl_error_t ret           = p_sensor->impl->sleep();
  return ret;
}

sl_error_t light_sensor_wakeup(sensor_light_handle_t sensor)
{
  if (sensor == NULL) {
    return RSI_FAIL;
  }
  sensor_light_t *p_sensor = (sensor_light_t *)(sensor);
  sl_error_t ret           = p_sensor->impl->wakeup();
  return ret;
}

sl_error_t light_sensor_set_power(sensor_light_handle_t sensor, sensor_power_mode_t power_mode)
{
  sensor_light_t *p_sensor = (sensor_light_t *)(sensor);
  sl_error_t ret           = 0;

  if (sensor == NULL || power_mode == NULL) {
    return RSI_FAIL;
  }

  switch (power_mode) {
    case POWER_MODE_WAKEUP:
      ret = p_sensor->impl->wakeup();
      break;
    case POWER_MODE_SLEEP:
      ret = p_sensor->impl->sleep();
      break;
    default:
      ret = NULL;
      break;
  }
  return ret;
}

sl_error_t light_sensor_sample(sensor_light_handle_t sensor, sensor_data_group_t *data_group)
{
  sensor_light_t *p_sensor = (sensor_light_t *)(sensor);
  sl_error_t ret           = 0;

  if (sensor == NULL || data_group == NULL) {
    return RSI_FAIL;
  }

  if ((p_sensor->impl->sample_light) != NULL) {
    ret = p_sensor->impl->sample_light(&data_group->sensor_data[data_group->number].light);
    if (RSI_OK == ret) {
      data_group->number++;
    }
  }
  if ((p_sensor->impl->sample_rgbw) != NULL) {
    ret = p_sensor->impl->sample_rgbw(&data_group->sensor_data[data_group->number].rgbw.r,
                                      &data_group->sensor_data[data_group->number].rgbw.g,
                                      &data_group->sensor_data[data_group->number].rgbw.b,
                                      &data_group->sensor_data[data_group->number].rgbw.w);
    if (RSI_OK == ret) {
      data_group->number++;
    }
  }
  if ((p_sensor->impl->sample_uv) != NULL) {
    ret = p_sensor->impl->sample_uv(&data_group->sensor_data[data_group->number].uv.uv,
                                    &data_group->sensor_data[data_group->number].uv.uva,
                                    &data_group->sensor_data[data_group->number].uv.uvb);
    if (RSI_OK == ret) {
      data_group->number++;
    }
  }
  return RSI_OK;
}

sl_error_t light_sensor_control(sensor_light_handle_t sensor, sensor_command_t cmd, void *args)
{
  sl_error_t ret = RSI_OK;

  if (sensor == NULL || cmd == NULL) {
    return RSI_FAIL;
  }

  switch (cmd) {
    case COMMAND_SET_MODE:
      ret = RSI_OK;
      break;
    case COMMAND_SET_RANGE:
      ret = RSI_OK;
      break;
    case COMMAND_SET_ODR:
      ret = RSI_OK;
      break;
    case COMMAND_SET_POWER:
      //ret = light_sensor_set_power(sensor, (sensor_power_mode_t)args);//if it is required enable
      ret = RSI_OK;
      break;
    case COMMAND_SELF_TEST:
      //ret = light_sensor_test(sensor);//if it is required enable
      ret = RSI_OK;
      break;
    default:
      ret = RSI_OK;
      break;
  }
  return ret;
}
