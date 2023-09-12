/***************************************************************************/ /**
 * @file sensorhub_app.c
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
/*
 * sensorhub_app.c
 *
 *  Created on: Mar 1, 2023
 *      Author: ratanner
 */

// Include files
#include "rsi_ccp_user_config.h"
#include "sensor_hub.h"
#include <stdio.h>
#include "rsi_chip.h"
#include "rsi_os.h"
#include "rsi_board.h"
#include "cmsis_os2.h"

#define SENSOR_PERIOD       CONFIG_SENSOR_EXAMPLE_PERIOD
#define APP_TASK_STACK_SIZE 4096
#define APP_TASK_PRIORITY   1

const osThreadAttr_t app_thread_attributes = {
  .name       = "APP",
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = APP_TASK_STACK_SIZE,
  .priority   = osPriorityLow,
  .tz_module  = 0,
  .reserved   = 0,
};

extern sl_sensor_info_t sensor_info[MAX_NUM_SENSORS];

rsi_task_handle_t app_task_handle = NULL;
uint32_t event_ack                = 0;
static uint32_t sensor_scan_cnt   = 0;
void sensor_event_handler(uint8_t sensor_id, uint8_t event, void *rx_sensor_data)
{
  uint8_t sens_ind;
  for (sens_ind = 0; sens_ind < MAX_NUM_SENSORS; sens_ind++) {
    if (sensor_info[sens_ind].sensor_id == sensor_id) {
      break;
    }
  }

  if (MAX_NUM_SENSORS == sens_ind) {
    DEBUGOUT("Sensor not Found!");
    return;
  }

  switch (event) {
    case SL_SENSOR_STARTED:
      DEBUGOUT("Sensor Started! \r\n", sensor_id);
      break;
    case SL_SENSOR_STOPPED:
      DEBUGOUT("Sensor Stopped! \r\n", sensor_id);
      break;
    case SL_SENSOR_DATA_READY:

      DEBUGOUT("%s ", sensor_info[sens_ind].name);
      DEBUGOUT("Sensor data Ready: \t", sensor_id);

      if (sensor_info[sens_ind].data_deliver.mode == TIMEOUT) {
        for (int i = 0; i < sensor_info[sens_ind].data_deliver.timeout / sensor_info[sens_ind].sampling_interval; i++) {
          DEBUGOUT("%d \t", (int)sensor_info[sens_ind].sens_data_ptr->sensor_data[i].light);
        }
      }
      if (sensor_info[sens_ind].data_deliver.mode == NUM_OF_SAMPLES) {
        for (int i = 0; i < sensor_info[sens_ind].data_deliver.numOfSamples; i++) {
          DEBUGOUT("%d \t", (int)sensor_info[sens_ind].sens_data_ptr->sensor_data[i].light);
        }
      }

      if (sensor_info[sens_ind].data_deliver.mode == THRESHOLD) {
        DEBUGOUT("%d \t", (int)sensor_info[sens_ind].sens_data_ptr->sensor_data[0].light);
      }
      DEBUGOUT("\r\n");

      /*Acknowledge data reception*/
      event_ack = sensor_id;
      break;

    case SL_SENSOR_CNFG_INVALID:
      DEBUGOUT(" SL_SENSOR_CNFG_INVALID \r\n", sensor_id);

      break;
    case SL_SENSOR_START_FAILED:
      DEBUGOUT("Sensor START failed. \r\n", sensor_id);

      break;
    case SL_SENSOR_STOP_FAILED:
      DEBUGOUT("Sensor STOP failed. \r\n", sensor_id);

      break;
    case SL_SENSOR_DELETED:
      DEBUGOUT("Sensor deleted. \r\n", sensor_id);
      break;

    case SL_SENSOR_DELETE_FAILED:
      DEBUGOUT("Sensor deleted failed\r\n", sensor_id);
      break;
    default:
      break;
  }
}

void APP_Task()
{

  uint32_t status = 0;
  uint8_t cnt = 0, delay = 0;
  sl_sensor_id_en sl_sensor_scan_info[MAX_NUM_SENSORS];

  DEBUGOUT("\r\n Start Sensor HUB APP Task \r\n");

  //!Register callback handler for getting different events from sensor hub
  sl_sensorhub_notify_cb_register(sensor_event_handler, &event_ack);

  //!Initialize sensor interface/s//!
  status = sl_sensorhub_init();
  if (status != RSI_OK) {
    DEBUGOUT("\r\n Sensor Hub Init failed \r\n");
    while (1)
      ;
  }

  //! sensor hub scan
  sensor_scan_cnt = sl_detect_sensors(&sl_sensor_scan_info, MAX_NUM_SENSORS);
  if (sensor_scan_cnt == 0) {
    DEBUGOUT("\r\n No sensor is detected \r\n");
    while (1)
      ;
  }

  //!create sensors for scanned sensors
  for (cnt = 0; cnt < sensor_scan_cnt; cnt++) {
    sl_sensorhub_create_sensor(sl_sensor_scan_info[cnt]);
  }
  status = sensor_hub_start();
  if (status != RSI_OK) {
    DEBUGOUT("\r\n  Sensor HUB start failed \r\n");
    while (1)
      ;
  }

  //!Start the sensors
  for (cnt = 0; cnt < sensor_scan_cnt; cnt++) {
    /*start a sensor, data ready events will be posted once data acquired successfully*/
    sl_sensorhub_start_sensor(sl_sensor_scan_info[cnt]);
  }

  while (1)
    ;

  /*  while (1) {
    delay++;
    vTaskDelay(1000);
    if (delay > 100) {
      for (cnt = 0; cnt < sensor_scan_cnt; cnt++) {
        Stop the sensor
        sl_sensorhub_stop_sensor(sl_sensor_scan_info[cnt]);
      }
      while (1)
        ;
    }
  }*/
}

void app_main()
{
  osThreadNew((osThreadFunc_t)APP_Task, NULL, &app_thread_attributes);
}
