/***************************************************************************/ /**
 * @file adc_sensor_hal.c
 * @brief adc sensor hal handles ADC based sensors
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "rsi_debug.h"
#include "sensor_hub.h"
#include "sensor_type.h"
#include "sl_si91x_adc.h"
#include "adc_sensor_driver.h"

// For unused parameter
#define UNUSED_PARAM __attribute__((unused))

/* Rx buffer to store ADC data in ram*/
#ifdef SL_SH_ADC_CHANNEL0
uint16_t adc_data_ram_ch0[SL_SH_ADC_SENSOR0_NUM_OF_SAMPLES * SL_SH_ADC_CH0_NUM_SAMPLES] __attribute__((aligned(2)));
#endif
#ifdef SL_SH_ADC_CHANNEL1
uint16_t adc_data_ram_ch1[SL_SH_ADC_CH1_NUM_SAMPLES] __attribute__((aligned(2)));
#endif

#ifdef SL_SH_ADC_CHANNEL0
uint16_t *adc_data_ptrs[] = {
  adc_data_ram_ch0,
#ifdef SL_SH_ADC_CHANNEL1
  adc_data_ram_ch1,
#endif
};
#endif

/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
static bool is_channel_init = false;
static sl_adc_cfg_t *adc_config;

sl_adc_error_t sl_si91x_guva_s12d_sample(uint8_t channel, bool is_static, uint16_t *adc_value);
sl_adc_error_t sl_si91x_joystick_sample(uint8_t channel, bool is_static, uint16_t *adc_value);

sl_adc_sensor_data_t adc_sensor_info[] = {
  { .channel = SL_SH_ADC_CH0_CHANNEL, .sample = sl_si91x_guva_s12d_sample, .sleep = NULL, .wakeup = NULL },

  { .channel = SL_SH_ADC_CH0_CHANNEL, .sample = sl_si91x_joystick_sample, .sleep = NULL, .wakeup = NULL }
};

sl_adc_error_t sl_si91x_guva_s12d_sample(uint8_t channel, bool is_static, uint16_t *adc_value)
{
  if (is_static) {
    sl_si91x_adc_sensor_sample_static(adc_value);
  } else {
    sl_si91x_sensor_channel_sample(channel);
  }
  return SL_STATUS_OK;
}

sl_adc_error_t sl_si91x_joystick_sample(uint8_t channel, bool is_static, uint16_t *adc_value)
{
  if (is_static) {
    sl_si91x_adc_sensor_sample_static(adc_value);
  } else {
    sl_si91x_sensor_channel_sample(channel);
  }
  return SL_STATUS_OK;
}

/*******************************************************************************
 * @fn        static const adc_sensor_impl_t *get_implementation(int channel)
 * @brief     ADC get sensor implementation
 *
 * @return    pointer to adc sensor implementation
*******************************************************************************/

static sl_adc_sensor_data_t *get_implementation(uint8_t channel)
{
  uint8_t i;
  uint8_t size = sizeof(adc_sensor_info) / sizeof(sl_adc_sensor_data_t);
  for (i = 0; i < size; i++) {
    if (adc_sensor_info[i].channel == channel) {
      return &adc_sensor_info[i];
    }
  }
  return NULL;
}

/*******************************************************************************
 * @fn        static sl_status_t adc_stop(sl_adc_config_t *adc_cfg)
 * @brief     ADC stop
 *            This function stops ADC peripheral
 *
 * @param[in] adc_cfg: ADC configuration
 *
 * @return    respective sl error code (SL_STATUS_OK, SL_STATUS_FAIL, ETC)
*******************************************************************************/
static sl_status_t adc_stop(sl_adc_config_t *adc_cfg)
{
  sl_status_t sl_status;
  sl_status = sl_si91x_adc_stop(*adc_cfg);

  return sl_status;
}

/*******************************************************************************
 * @fn        sl_sensor_adc_handle_t sl_si91x_adc_sensor_create(sl_adc_bus_handle_t bus, int channel)
 * @brief     ADC sensor create
 *            This function will create ADC sensor by initializing respective sensor and
 *            then starts the ADC peripheral if not started already. We will keep is_init
 *            flag to know if particular sensor is initialized.
 *
 * @param[in] bus: ADC bus value
 * @param[in] channel: ADC channel (used to find respective sensor implementations)
 *
 * @return    respective sl error code (SL_STATUS_OK, SL_STATUS_FAIL, ETC)
*******************************************************************************/
sl_sensor_adc_handle_t sl_si91x_adc_sensor_create(UNUSED_PARAM sl_sensor_bus_t bus, int channel)
{
  if (!is_channel_init) {
    adc_config = sl_si91x_fetch_adc_bus_intf_info();
  }
  sl_adc_sensor_data_t *p_sensor = (sl_adc_sensor_data_t *)pvPortMalloc(sizeof(sl_adc_sensor_data_t));
  if (p_sensor == NULL) {
    DEBUGOUT("\r\n ADC sensor create failed while memory allocation:%u \r\n", sizeof(sl_adc_sensor_data_t));
    return NULL;
  }
  p_sensor->channel = (uint8_t)channel;

  p_sensor = get_implementation(p_sensor->channel);
  /* All other adc sensor handles will be called only after sensor_create, so we are storing the pointer to adc bus interface info here */

  if (!is_channel_init) {
    sl_status_t ret = sl_si91x_adc_channel_init(&adc_config->adc_ch_cfg, &adc_config->adc_cfg);
    if (ret != SL_STATUS_OK) {
      DEBUGOUT("\r\n ADC sensor channels init failed, channel \r\n");
      return NULL;
    }
    is_channel_init = true;
  }
  DEBUGOUT("\r\n ADC - sensor created, channels %d\r\n", channel);
  p_sensor->is_init = true;

  return (sl_sensor_adc_handle_t)p_sensor;
}

/*******************************************************************************
 * @fn        inline sl_status_t sl_si91x_adc_sensor_enable(sl_adc_sensor_data_t *p_sensor, UNUSED_PARAM int channel)
 * @brief     ADC sensor enable
 *            This function will enable the sensor by enabling respective ADC channel. Note
 *            that ADC peripheral is started as part of sensor init. And this should be 
 *            used only if you are using multiple channels
 *
 * @param[in] p_sensor: ADC sensor pointer which contains implementations, bus, etc.
 * @param[in] channel: ADC channel number
 *
 * @return    respective sl error code (SL_STATUS_OK, SL_STATUS_FAIL, ETC)
*******************************************************************************/
inline sl_status_t sl_si91x_adc_sensor_enable(sl_adc_sensor_data_t *p_sensor, UNUSED_PARAM int channel)
{
  if (p_sensor == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  for (uint8_t ch_no = 0; ch_no < MAX_CHNL_NO; ch_no++) {
    if (p_sensor->channel & BIT(ch_no)) {
      sl_status_t ret = sl_si91x_adc_chnl_enable(ch_no);
      if (ret != SL_STATUS_OK) {
        DEBUGOUT("\r\n ADC sensor enable failed \r\n");
        return ret;
      }
    }
  }
  return SL_STATUS_OK;
}

/*******************************************************************************
 * @fn        inline sl_status_t sl_si91x_adc_sensor_disable(sl_adc_sensor_data_t *p_sensor, UNUSED_PARAM int channel)
 * @brief     ADC sensor disable
 *            This function will disables the sensor by disabling respective ADC channel
 *            Note that ADC peripheral will not be deinitialized here. And this should be 
 *            used only if you are using multiple channels
 *
 * @param[in] sensor: ADC sensor pointer which contains implementations, bus, etc.
 * @param[in] channel: ADC channel number
 *
 * @return    respective sl error code (SL_STATUS_OK, SL_STATUS_FAIL, ETC)
*******************************************************************************/
inline sl_status_t sl_si91x_adc_sensor_disable(sl_adc_sensor_data_t *p_sensor, UNUSED_PARAM int channel)
{
  if (p_sensor == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  for (uint8_t ch_no = 0; ch_no < MAX_CHNL_NO; ch_no++) {
    if (p_sensor->channel & BIT(ch_no)) {
      sl_status_t ret = sl_si91x_adc_chnl_disable(ch_no);
      if (ret != SL_STATUS_OK) {
        DEBUGOUT("\r\n ADC sensor disable failed \r\n");
        return ret;
      }
    }
  }
  return SL_STATUS_OK;
}

/******************************************************************************
 * @fn        sl_status_t sl_si91x_adc_sensor_delete(sl_sensor_adc_handle_t *sensor)
 * @brief     ADC sensor delete
 *            This function will delete the sensor by freeing the adc sensor configurations
 *            and ADC peripheral will be stopped if no sensor is running.
 *
 * @param[in] sensor: ADC sensor pointer which contains implementations, bus, etc.
 *
 * @return    respective sl error code (SL_STATUS_OK, SL_STATUS_FAIL, ETC)
*******************************************************************************/
sl_adc_error_t sl_si91x_adc_sensor_delete(sl_sensor_adc_handle_t *sensor)
{
  if (sensor == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  sl_adc_sensor_data_t *p_sensor = (sl_adc_sensor_data_t *)(*sensor);

  if (!p_sensor->is_init) {
    free(p_sensor);
    return SL_STATUS_OK;
  }

  p_sensor->is_init = false;
  sl_status_t ret   = sl_si91x_adc_de_init(&adc_config->adc_cfg);
  if (ret != SL_STATUS_OK) {
    DEBUGOUT("\r\n ADC sensor deinitialization failed\r\n");
    return ret;
  }

  // TODO: Create a variable to know how many ADC sensors are created
  // Stop ADC only when no ADC sensors are present
  ret = adc_stop(&adc_config->adc_cfg);
  if (ret != SL_STATUS_OK) {
    DEBUGOUT("\r\n ADC sensor stop failed while deinitialization \r\n");
    return ret;
  }

  free(p_sensor);
  *sensor = NULL;
  return SL_STATUS_OK;
}

/*******************************************************************************
 * @fn        sl_status_t sl_si91x_adc_sensor_sample_static(sl_sensor_adc_handle_t *sensor, uint16_t *adc_value)
 * @brief     ADC sensor read static data
 *            This function will sample static ADC data from the ADC_DATA register.
 *
 * @param[in] sensor: ADC sensor pointer which contains implementations, bus, etc.
 * @param[in] adc_value: data pointer to store the ADC static value
 * 
 * @return    respective sl error code (SL_STATUS_OK, SL_STATUS_FAIL, ETC)
*******************************************************************************/
sl_status_t sl_si91x_adc_sensor_sample_static(uint16_t *adc_value)
{
  sl_status_t ret = sl_si91x_adc_read_static_sample(&adc_config->adc_ch_cfg, &adc_config->adc_cfg, adc_value);
  return ret;
}

/*******************************************************************************
 * @fn        sl_status_t sl_si91x_adc_sensor_sleep(sl_sensor_adc_handle_t sensor)
 * @brief     ADC sensor sleep
 *            This function will put ADC sensor into sleep
 *
 * @param[in] sensor: ADC sensor pointer which contains implementations, bus, etc.
 * 
 * @return    respective sl error code (SL_STATUS_OK, SL_STATUS_FAIL, ETC)
*******************************************************************************/
sl_status_t sl_si91x_adc_sensor_sleep(sl_sensor_adc_handle_t sensor)
{
  if (sensor == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  sl_adc_sensor_data_t *p_sensor = (sl_adc_sensor_data_t *)(sensor);
  sl_status_t ret                = p_sensor->sleep();
  return ret;
}

/*******************************************************************************
 * @fn        sl_status_t sl_si91x_adc_sensor_wakeup(sl_sensor_adc_handle_t sensor)
 * @brief     ADC sensor wakeup
 *            This function will wakeup ADC sensor from sleep
 *
 * @param[in] sensor: ADC sensor pointer which contains implementations, bus, etc.
 * 
 * @return    respective sl error code (SL_STATUS_OK, SL_STATUS_FAIL, ETC)
*******************************************************************************/
sl_status_t sl_si91x_adc_sensor_wakeup(sl_sensor_adc_handle_t sensor)
{
  if (sensor == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  sl_adc_sensor_data_t *p_sensor = (sl_adc_sensor_data_t *)(sensor);
  sl_status_t ret                = p_sensor->wakeup();
  return ret;
}

/*******************************************************************************
 * @fn        sl_status_t sl_si91x_adc_sensor_set_power(sl_sensor_adc_handle_t sensor, sl_sensor_power_mode_t power_mode)
 * @brief     ADC sensor set power mode
 *            This function will put sensor into requested power mode
 * 
 * @param[in] sensor: ADC sensor pointer which contains implementations, bus, etc.
 * @param[in] power_mode: power mode for sensor
 * 
 * @return    respective sl error code (SL_STATUS_OK, SL_STATUS_FAIL, ETC)
*******************************************************************************/
sl_status_t sl_si91x_adc_sensor_set_power(sl_sensor_adc_handle_t sensor, sl_sensor_power_mode_t power_mode)
{
  sl_adc_sensor_data_t *p_sensor = (sl_adc_sensor_data_t *)(sensor);
  sl_status_t ret                = 0;

  if (sensor == NULL) {
    return SL_STATUS_NULL_POINTER;
  } else if (power_mode > SL_SENSOR_POWER_MAX) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  switch (power_mode) {
    case SL_SENSOR_POWER_MODE_WAKEUP:
      ret = p_sensor->wakeup();
      break;
    case SL_SENSOR_POWER_MODE_SLEEP:
      ret = p_sensor->sleep();
      break;
    default:
      ret = SL_STATUS_INVALID_PARAMETER;
      break;
  }
  return ret;
}

/*******************************************************************************
 * @fn        sl_status_t sl_si91x_sensor_sample(uint8_t channel)
 * @brief     ADC read the sample
 *            This function will read sample data from a particular sample
 *
 * @param[in] sensor: ADC channel number from which to sample
 *
 * @return    respective sl error code (SL_STATUS_OK, SL_STATUS_FAIL, ETC)
*******************************************************************************/
sl_adc_error_t sl_si91x_sensor_channel_sample(uint8_t channel)
{
  sl_status_t status;
  status = sl_si91x_adc_channel_read_sample(&adc_config->adc_ch_cfg, channel);
  if (status != SL_STATUS_OK) {
    return status;
  }
  return SL_STATUS_OK;
}

/*******************************************************************************
 * @fn        sl_status_t sl_si91x_adc_sensor_sample(sl_sensor_adc_handle_t *sensor, sl_sensor_data_group_t *data_group)
 * @brief     ADC sensor sample
 *            This function will sample the respective channel ADC data.
 *
 * @param[in] sensor: ADC sensor pointer which contains implementations, bus, etc.
 * @param[in] data_group: contains memory to store ADC data and handle
 *            number of samples
 * 
 * @return    respective sl error code (SL_STATUS_OK, SL_STATUS_FAIL, ETC)
*******************************************************************************/
sl_adc_error_t sl_si91x_adc_sensor_sample(sl_sensor_adc_handle_t sensor, sl_sensor_data_group_t *data_group)
{
  sl_adc_sensor_data_t *p_sensor = (sl_adc_sensor_data_t *)(sensor);
  sl_status_t ret                = 0;
  uint8_t ch_no                  = p_sensor->channel;
  bool is_static                 = false;

  if (p_sensor == NULL) {
    return SL_STATUS_NULL_POINTER;
  } else if (p_sensor->is_init == false) {
    return SL_STATUS_NOT_INITIALIZED;
  } else if (adc_config->adc_cfg.operation_mode == SL_ADC_FIFO_MODE) {
    if ((adc_config->adc_data_ready & BIT(ch_no)) != 1) {
      return SL_STATUS_NOT_READY;
    }
  }

  if (adc_config->adc_cfg.operation_mode == SL_ADC_STATIC_MODE) {
    is_static = true;
    ret       = p_sensor->sample(p_sensor->channel, is_static, (uint16_t *)&adc_data_ptrs[0][data_group->number]);
    data_group->sensor_data[0].adc = (uint16_t *)adc_data_ptrs[0];
    if (SL_STATUS_OK != ret) {
      return ret;
    }
    data_group->number++;
  } else {
    if (adc_config->adc_data_ready & BIT(ch_no)) {
      adc_config->adc_data_ready &= ~(BIT(ch_no));
      if (p_sensor->sample != NULL) {
        p_sensor->sample(ch_no, is_static, NULL);
        for (uint16_t sample_length = 0; sample_length < adc_config->adc_ch_cfg.num_of_samples[ch_no];
             sample_length++) {
          if (adc_config->adc_ch_cfg.rx_buf[ch_no][sample_length] & BIT(11)) {
            adc_config->adc_ch_cfg.rx_buf[ch_no][sample_length] =
              (adc_config->adc_ch_cfg.rx_buf[ch_no][sample_length] & (ADC_MASK_VALUE));
          } else {
            adc_config->adc_ch_cfg.rx_buf[ch_no][sample_length] = adc_config->adc_ch_cfg.rx_buf[ch_no][sample_length]
                                                                  | BIT(11);
          }
        }
        data_group->sensor_data[0].adc = (uint16_t *)adc_config->adc_ch_cfg.rx_buf[ch_no];
        data_group->number             = adc_config->adc_ch_cfg.num_of_samples[ch_no];
      }
    }
  }

  return SL_STATUS_OK;
}

/*******************************************************************************
 * @fn        sl_status_t sl_si91x_adc_sensor_control(sl_sensor_adc_handle_t sensor, sl_sensor_command_t cmd, UNUSED_PARAM void *args)
 * @brief     ADC sensor control
 *            This function will control the different sensor modes
 *
 * @param[in] sensor: ADC sensor pointer which contains implementations, bus, etc.
 * @param[in] cmd: put the sensor into respective mode
 * 
 * @return    respective sl error code (SL_STATUS_OK, SL_STATUS_FAIL, ETC)
*******************************************************************************/
sl_adc_error_t sl_si91x_adc_sensor_control(sl_sensor_adc_handle_t sensor,
                                           sl_sensor_command_t cmd,
                                           UNUSED_PARAM void *args)
{
  sl_status_t ret = SL_STATUS_OK;

  if (sensor == NULL) {
    return SL_STATUS_NULL_POINTER;
  } else if (cmd > SL_COMMAND_MAX) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  switch (cmd) {
    case SL_COMMAND_SET_MODE:
      ret = SL_STATUS_OK;
      break;
    case SL_COMMAND_SET_RANGE:
      ret = SL_STATUS_OK;
      break;
    case SL_COMMAND_SET_ODR:
      ret = SL_STATUS_OK;
      break;
    case SL_COMMAND_SET_POWER:
      ret = SL_STATUS_OK;
      break;
    case SL_COMMAND_SELF_TEST:
      ret = SL_STATUS_OK;
      break;
    default:
      ret = SL_STATUS_OK;
      break;
  }
  return ret;
}
