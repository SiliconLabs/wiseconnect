/*
 * lightsensor_hal.c
 *
 *  Created on: Mar 12, 2023
 *      Author: kokuncha
 */
#include "bh1750fvi.h"
#include "sensor_hub.h"
#include "rsi_board.h"

#define BH_1750_MEASUREMENT_ACCURACY 1.2  /*!< the typical measurement accuracy of  BH1750 sensor */
#define BH1750_POWER_DOWN            0x00 /*!< Command to set Power Down*/
#define BH1750_POWER_ON              0x01 /*!< Command to set Power On*/
#define BH1750_DATA_REG_RESET        0x07 /*!< Command to reset data register, not acceptable in power down mode*/

extern ARM_DRIVER_I2C *I2Cdrv;

#define MAX_TIME_TO_WAIT 5000;
TickType_t xTicksToWait = MAX_TIME_TO_WAIT;
TimeOut_t xTimeOut;

bh1750_handle_t bh1750_create(i2c_bus_handle_t bus, uint8_t dev_addr)
{
  bh1750_dev_t *sensor = (bh1750_dev_t *)pvPortMalloc(sizeof(bh1750_dev_t));
  uint8_t a[5]         = "\0";

  a[5]            = "\0";
  a[0]            = (uint8_t)(0x01 & 0xFF);
  sensor->i2c_dev = I2Cdrv->MasterTransmit(BH1750_I2C_ADDRESS_DEFAULT, a, 1, false);

  DEBUGOUT("\r\n I2C sensor create \r\n");
  while (I2Cdrv->GetStatus().busy)
    ;
  /*TODO: check the time out */
  //  vTaskSetTimeOutState( &xTimeOut );
  //  while (I2Cdrv->GetStatus().busy) {
  //      if( xTaskCheckForTimeOut( &xTimeOut, &xTicksToWait ) != pdFALSE ) {
  //          DEBUGOUT("\r\n I2C Status TimeOut \r\n");
  //          return RSI_FAIL;
  //      }
  //  }

  sensor->dev_addr = dev_addr;
  return (bh1750_handle_t)sensor;
}

sl_err_t bh1750_delete(bh1750_handle_t *sensor)
{
  if (*sensor == NULL) {
    return RSI_OK;
  }
  free(sensor);
  *sensor = NULL;
  return RSI_OK;
}

sl_err_t bh1750_power_down(bh1750_handle_t sensor)
{
  uint8_t ret        = 0;
  bh1750_dev_t *sens = (bh1750_dev_t *)sensor;
  uint8_t a[5]       = "\0";

  a[0] = (uint8_t)(BH1750_POWER_DOWN & 0xFF);
  ret  = I2Cdrv->MasterTransmit(BH1750_I2C_ADDRESS_DEFAULT, a, 1, false);

  DEBUGOUT("\r\n I2C sensor create \r\n");
  while (I2Cdrv->GetStatus().busy)
    ;

  return ret;
}

sl_err_t bh1750_power_on(bh1750_handle_t sensor)
{
  uint8_t ret = 0, a[5] = "\0";
  bh1750_dev_t *sens = (bh1750_dev_t *)sensor;

  a[0] = (uint8_t)(BH1750_POWER_ON & 0xFF);
  ret  = I2Cdrv->MasterTransmit(BH1750_I2C_ADDRESS_DEFAULT, a, 1, false); //sens->i2c_dev
  while (I2Cdrv->GetStatus().busy)
    ;

  return ret;
}

sl_err_t bh1750_reset_data_register(bh1750_handle_t sensor)
{
  uint8_t ret        = 0;
  bh1750_dev_t *sens = (bh1750_dev_t *)sensor;
  bh1750_power_on(sensor);
  uint8_t a[5] = "\0";

  a[0] = (uint8_t)(BH1750_DATA_REG_RESET & 0xFF);
  ret  = I2Cdrv->MasterTransmit(BH1750_I2C_ADDRESS_DEFAULT, a, 1, false);
  while (I2Cdrv->GetStatus().busy)
    ;

  return ret;
}

sl_err_t bh1750_change_measure_time(bh1750_handle_t sensor, uint8_t measure_time)
{
  uint8_t ret        = 0;
  bh1750_dev_t *sens = (bh1750_dev_t *)sensor;
  uint8_t buf[2]     = { 0x40, 0x60 };
  buf[0] |= measure_time >> 5;
  buf[1] |= measure_time & 0x1F;

  ret = I2Cdrv->MasterTransmit(BH1750_I2C_ADDRESS_DEFAULT, buf, 2, false);
  while (I2Cdrv->GetStatus().busy)
    ;
  return ret;
}

sl_err_t bh1750_set_measure_mode(bh1750_handle_t sensor, bh1750_cmd_measure_t cmd_measure)
{
  uint8_t ret = 0, a[0] = "\0";
  bh1750_dev_t *sens = (bh1750_dev_t *)sensor;

  a[0] = (uint8_t)(cmd_measure & 0xFF);
  I2Cdrv->MasterTransmit(BH1750_I2C_ADDRESS_DEFAULT, a, 1, false);
  /* TODO: Add timeout and PASS error*/
  while (I2Cdrv->GetStatus().busy)
    ;
  return ret;
}

sl_err_t bh1750_get_data(bh1750_handle_t sensor, float *data)
{
  uint8_t ret = 0, len = 0;
  bh1750_dev_t *sens     = (bh1750_dev_t *)sensor;
  uint8_t bh1750_data[2] = { 0 };
  I2Cdrv->MasterReceive(BH1750_I2C_ADDRESS_DEFAULT, bh1750_data, 2, false);
  /* TODO: Add timeout and PASS error*/
  while (I2Cdrv->GetStatus().busy)
    ;
  *data = ((bh1750_data[0] << 8 | bh1750_data[1]) / BH_1750_MEASUREMENT_ACCURACY);
  return ret;
}

sl_err_t bh1750_get_light_intensity(bh1750_handle_t sensor, bh1750_cmd_measure_t cmd_measure, float *data)
{
  sl_err_t ret = bh1750_set_measure_mode(sensor, cmd_measure);

  if (ret != RSI_OK) {
    return ret;
  }

  if ((cmd_measure == BH1750_CONTINUE_4LX_RES) || (cmd_measure == BH1750_ONETIME_4LX_RES)) {
    vTaskDelay(30 / portTICK_RATE_MS);
  } else {
    vTaskDelay(180 / portTICK_RATE_MS);
  }

  ret = bh1750_get_data(sensor, data);
  return ret;
}

sl_err_t bh1750_init(i2c_bus_handle_t i2c_bus)
{
  bh1750 = bh1750_create(i2c_bus, BH1750_I2C_ADDRESS_DEFAULT);

  if (!bh1750) {
    return RSI_FAIL;
  }
  is_init = true;
  return RSI_OK;
}

sl_err_t bh1750_deinit(void)
{
  if (!is_init) {
    return RSI_FAIL;
  }

  sl_err_t ret = bh1750_delete(&bh1750);

  if (ret == RSI_OK) {
    is_init = false;
  }

  return RSI_OK;
}

sl_err_t bh1750_test(void)
{
  if (!is_init) {
    return RSI_FAIL;
  }

  return RSI_OK;
}

sl_err_t bh1750_sample_light(float *light_data)
{
  if (!is_init) {
    return RSI_FAIL;
  }

  bh1750_cmd_measure_t cmd_measure;
  float bh1750_data;
  sl_err_t ret = bh1750_power_on(bh1750);
  cmd_measure  = BH1750_CONTINUE_4LX_RES;
  ret          = bh1750_set_measure_mode(bh1750, cmd_measure);
  vTaskDelay(30 / portTICK_RATE_MS); /*TODO:check if need or change*/
  ret = bh1750_get_data(bh1750, &bh1750_data);

  if (ret == RSI_OK) {
    *light_data = bh1750_data;
    return RSI_OK;
  }

  *light_data = 0;
  return RSI_FAIL;
}
