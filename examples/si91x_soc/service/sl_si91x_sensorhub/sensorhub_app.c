/***************************************************************************/ /**
* @file sensorhub_app.c
* @brief sensorhub_app example
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
 *    claim that you wrote the original software.
 *    If you use this software In in a product,
 *    an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
/***************************************************************************
 * @brief : This file perform the sensor hub application related operations
 * like intilize ,scanning the sensors,and create/start the sensor HUB
 ******************************************************************************/

// Include files
#include "rsi_ccp_user_config.h"
#include "sensor_hub.h"
#include <stdio.h>
#include "rsi_chip.h"
#include "rsi_os.h"
#include "rsi_debug.h"
#include "cmsis_os2.h"

/*******************************************************************************
 **************  Sensor app Task Attributes structure for thread   *************
 ******************************************************************************/
#define SL_APP_TASK_STACK_SIZE 4096
const osThreadAttr_t app_thread_attributes = {
  .name       = "SensorHub_App", // Name of thread
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = SL_APP_TASK_STACK_SIZE, // Stack size of sensor_app task
  .priority   = osPriorityLow,          // Priority of Sensor task
  .tz_module  = 0,
  .reserved   = 0,
};
rsi_task_handle_t app_task_handle = NULL;

/*******************************************************************************
 ********************  Extern variables/structures   ***************************
 ******************************************************************************/
extern sl_sensor_info_t sensor_hub_info_t[SL_MAX_NUM_SENSORS]; // Sensor info structure

/*******************************************************************************
 ********************** Local/global variables  *******************************
 ******************************************************************************/
uint32_t event_ack              = 0; // Sensor event acknowledge
static uint32_t sensor_scan_cnt = 0; // Sensor scan count

/*******************************************************************************
 ************************* Local functions  ************************************
 ******************************************************************************/
void sl_si91x_sensorhub_app_task(void);                               // application task
void sensorhub_app_init(void);                                        // application initialization
void sl_si91x_sensor_event_handler(uint8_t sensor_id, uint8_t event); // application event handler

/**************************************************************************/ /**
 * @fn           void sl_si91x_sensor_event_handler()
 * @brief        This Sensor event handle to the Sensor HUB.
 *
 * @param[in]    sensor_id        :Id of the Sensor
 * @param[in]    events           :Sensor hub event
 * @param[out]   NULL
 *
******************************************************************************/
void sl_si91x_sensor_event_handler(uint8_t sensor_id, uint8_t event)
{
  uint8_t sens_ind;
  for (sens_ind = 0; sens_ind < SL_MAX_NUM_SENSORS; sens_ind++) {
    if (sensor_hub_info_t[sens_ind].sensor_id == sensor_id) {
      break;
    }
  }

  if (SL_MAX_NUM_SENSORS == sens_ind) {
    DEBUGOUT("Sensor not Found!");
    return;
  }

  switch (event) {
    case SL_SENSOR_CREATION_FAILED:
      DEBUGOUT("%s Not created\r\n", sensor_hub_info_t[sens_ind].sensor_name);
      break;
    case SL_SENSOR_STARTED:
      DEBUGOUT("Sensor Started:%u \r\n", sensor_id);
      break;
    case SL_SENSOR_STOPPED:
      DEBUGOUT("Sensor Stopped:%u \r\n", sensor_id);
      break;
    case SL_SENSOR_DATA_READY:

      DEBUGOUT("\r\n %s ", sensor_hub_info_t[sens_ind].sensor_name);
      DEBUGOUT("\r\n Sensor data Ready:%u \t", sensor_id);

      if (SL_SENSOR_ADXL345_ID == sensor_id) {
        if (sensor_hub_info_t[sens_ind].sensor_mode == SL_SH_INTERRUPT_MODE) {
          DEBUGOUT("Axis X = %f, \t",
                   (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[0].accelerometer.x);
          DEBUGOUT("Axis Y = %f, \t",
                   (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[0].accelerometer.y);
          DEBUGOUT("Axis Z = %f \t\n ",
                   (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[0].accelerometer.z);
        } else if (sensor_hub_info_t[sens_ind].sensor_mode == SL_SH_POLLING_MODE) {
          if (sensor_hub_info_t[sens_ind].data_deliver.data_mode == SL_SH_THRESHOLD) {
            DEBUGOUT("Axis X = %f, \t",
                     (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[0].accelerometer.x);
            DEBUGOUT("Axis Y = %f, \t",
                     (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[0].accelerometer.y);
            DEBUGOUT("Axis Z = %f \t\r\n ",
                     (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[0].accelerometer.z);
          }
          if (sensor_hub_info_t[sens_ind].data_deliver.data_mode == SL_SH_TIMEOUT) {
            for (uint32_t i = 0;
                 i < sensor_hub_info_t[sens_ind].data_deliver.timeout / sensor_hub_info_t[sens_ind].sampling_interval;
                 i++) {
              DEBUGOUT("\r\n Axis X = %f, \t",
                       (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[i].accelerometer.x);
              DEBUGOUT("Axis Y = %f, \t",
                       (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[i].accelerometer.y);
              DEBUGOUT("Axis Z = %f \r\n ",
                       (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[i].accelerometer.z);
            }
          }
          if (sensor_hub_info_t[sens_ind].data_deliver.data_mode == SL_SH_NUM_OF_SAMPLES) {
            for (uint32_t i = 0; i < sensor_hub_info_t[sens_ind].data_deliver.numofsamples; i++) {
              DEBUGOUT("\r\n Axis:- X = %f, \t",
                       (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[i].accelerometer.x);
              DEBUGOUT("Y = %f, \t",
                       (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[i].accelerometer.y);
              DEBUGOUT("Z = %f  \t",
                       (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[i].accelerometer.z);
            }
          }
        }
      }
      if (SL_SENSOR_APDS9960_ID == sensor_id) {
        if (sensor_hub_info_t[sens_ind].sensor_mode == SL_SH_INTERRUPT_MODE) {
          DEBUGOUT("R = %f, \t", (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[0].rgbw.r);
          DEBUGOUT("G = %f, \t", (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[0].rgbw.g);
          DEBUGOUT("B = %f \t\n ", (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[0].rgbw.b);
          DEBUGOUT("Proximity = %f ;\t\n",
                   (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[0].rgbw.proximity);
          //   DEBUGOUT("Gesture = %c \t\n", (char)sensor_hub_info_t[sens_ind].sens_data_ptr->sensor_data[0].gesture);

        } else if (sensor_hub_info_t[sens_ind].sensor_mode == SL_SH_POLLING_MODE) {
          if (sensor_hub_info_t[sens_ind].data_deliver.data_mode == SL_SH_THRESHOLD) {
            DEBUGOUT("R = %f, \t", (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[0].rgbw.r);
            DEBUGOUT("G = %f, \t", (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[0].rgbw.g);
            DEBUGOUT("B = %f \t\n ", (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[0].rgbw.b);
            DEBUGOUT("Proximity = %f ;\t\n",
                     (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[0].rgbw.proximity);
            //  DEBUGOUT("Gesture = %c \t\n", (char)sensor_hub_info_t[sens_ind].sens_data_ptr->sensor_data[0].gesture);
          }
          if (sensor_hub_info_t[sens_ind].data_deliver.data_mode == SL_SH_TIMEOUT) {
            for (uint32_t i = 0;
                 i < sensor_hub_info_t[sens_ind].data_deliver.timeout / sensor_hub_info_t[sens_ind].sampling_interval;
                 i++) {
              DEBUGOUT("Proximity = %f ;\t",
                       (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[i].rgbw.proximity);
              DEBUGOUT(" R = %f, \t", (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[i].rgbw.r);
              DEBUGOUT(" G = %f, \t", (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[i].rgbw.g);
              DEBUGOUT(" B = %f \t\n ", (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[i].rgbw.b);
              // DEBUGOUT("Gesture = %c \t\n", (char)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[i].gesture);
            }
          }
          if (sensor_hub_info_t[sens_ind].data_deliver.data_mode == SL_SH_NUM_OF_SAMPLES) {
            for (uint32_t i = 0; i < sensor_hub_info_t[sens_ind].data_deliver.numofsamples; i++) {
              DEBUGOUT("\r\n R = %f, \t", (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[i].rgbw.r);
              DEBUGOUT(" G = %f, \t", (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[i].rgbw.g);
              DEBUGOUT(" B = %f \t; ", (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[i].rgbw.b);
              DEBUGOUT(" Proximity = %f ",
                       (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[i].rgbw.proximity);
              //  DEBUGOUT("Gesture = %c \t\n", (char)sensor_hub_info_t[sens_ind].sens_data_ptr->sensor_data[i].gesture);
            }
          }
        }
      }

      if (SL_SENSOR_LM75_ID == sensor_id) {
        if (sensor_hub_info_t[sens_ind].sensor_mode == SL_SH_INTERRUPT_MODE) {
          DEBUGOUT("%f \t", (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[0].temperature);
        } else if (sensor_hub_info_t[sens_ind].sensor_mode == SL_SH_POLLING_MODE) {
          if (sensor_hub_info_t[sens_ind].data_deliver.data_mode == SL_SH_TIMEOUT) {
            for (uint32_t i = 0;
                 i < sensor_hub_info_t[sens_ind].data_deliver.timeout / sensor_hub_info_t[sens_ind].sampling_interval;
                 i++) {
              DEBUGOUT(" %f \t ", (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[i].temperature);
            }
          }
          if (sensor_hub_info_t[sens_ind].data_deliver.data_mode == SL_SH_NUM_OF_SAMPLES) {
            for (uint32_t i = 0; i < sensor_hub_info_t[sens_ind].data_deliver.numofsamples; i++) {
              DEBUGOUT("%f \t", (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[i].temperature);
            }
          }

          if (sensor_hub_info_t[sens_ind].data_deliver.data_mode == SL_SH_THRESHOLD) {
            DEBUGOUT("%f \t", (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[0].temperature);
          }
        }
      }

      if (SL_SENSOR_BH1750_ID == sensor_id) {
        if (sensor_hub_info_t[sens_ind].sensor_mode == SL_SH_INTERRUPT_MODE) {
          DEBUGOUT("%f \t", (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[0].light);
        } else if (sensor_hub_info_t[sens_ind].sensor_mode == SL_SH_POLLING_MODE) {
          if (sensor_hub_info_t[sens_ind].data_deliver.data_mode == SL_SH_TIMEOUT) {
            for (uint32_t i = 0;
                 i < sensor_hub_info_t[sens_ind].data_deliver.timeout / sensor_hub_info_t[sens_ind].sampling_interval;
                 i++) {
              DEBUGOUT("%f \t", (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[i].light);
            }
          }
          if (sensor_hub_info_t[sens_ind].data_deliver.data_mode == SL_SH_NUM_OF_SAMPLES) {
            for (uint32_t i = 0; i < sensor_hub_info_t[sens_ind].data_deliver.numofsamples; i++) {
              DEBUGOUT("%f \t", (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[i].light);
            }
          }

          if (sensor_hub_info_t[sens_ind].data_deliver.data_mode == SL_SH_THRESHOLD) {
            DEBUGOUT("%f \t", (double)sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[0].light);
          }
        }
      }

      if (SL_SENSOR_ADC_JOYSTICK_ID == sensor_id) {
        float vout = 0;
        if (sensor_hub_info_t[sens_ind].sensor_mode == SL_SH_INTERRUPT_MODE) {
          for (uint32_t i = 0; i < SL_SH_ADC_CH0_NUM_SAMPLES; i++) {
            DEBUGOUT("%dmV \t", sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[0].adc[i]);
          }
        } else if (sensor_hub_info_t[sens_ind].sensor_mode == SL_SH_POLLING_MODE) {
          if (sensor_hub_info_t[sens_ind].data_deliver.data_mode == SL_SH_TIMEOUT) {
            for (uint32_t i = 0;
                 i < sensor_hub_info_t[sens_ind].data_deliver.timeout / sensor_hub_info_t[sens_ind].sampling_interval;
                 i++) {
              DEBUGOUT("%dmV \t", sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[0].adc[i]);
            }
          }
          if (sensor_hub_info_t[sens_ind].data_deliver.data_mode == SL_SH_NUM_OF_SAMPLES) {
            for (uint32_t i = 0; i < sensor_hub_info_t[sens_ind].data_deliver.numofsamples; i++) {
              DEBUGOUT("%dmV \t", sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[0].adc[i]);
            }
          }
          if (sensor_hub_info_t[sens_ind].data_deliver.data_mode == SL_SH_THRESHOLD) {
            DEBUGOUT("%dmV \t", sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[0].adc[0]);
          }
        }
        vout =
          (((float)*(sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[0].adc) / (float)SL_SH_ADC_MAX_OP_VALUE)
           * SL_SH_ADC_VREF_VALUE);
        DEBUGOUT("Single ended input: %lfV \t", (double)vout);
      }

      if (SL_SENSOR_ADC_GUVA_S12D_ID == sensor_id) {
        float vout = 0;
        if (sensor_hub_info_t[sens_ind].sensor_mode == SL_SH_INTERRUPT_MODE) {
          for (uint32_t i = 0; i < SL_SH_ADC_CH0_NUM_SAMPLES; i++) {
            DEBUGOUT("%d \t", sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[0].adc[i]);
          }
        } else if (sensor_hub_info_t[sens_ind].sensor_mode == SL_SH_POLLING_MODE) {
          if (sensor_hub_info_t[sens_ind].data_deliver.data_mode == SL_SH_TIMEOUT) {
            for (uint32_t i = 0;
                 i < sensor_hub_info_t[sens_ind].data_deliver.timeout / sensor_hub_info_t[sens_ind].sampling_interval;
                 i++) {
              DEBUGOUT("%d \t", sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[0].adc[i]);
            }
          }
          if (sensor_hub_info_t[sens_ind].data_deliver.data_mode == SL_SH_NUM_OF_SAMPLES) {
            for (uint32_t i = 0; i < sensor_hub_info_t[sens_ind].data_deliver.numofsamples; i++) {
              DEBUGOUT("%d \t", sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[0].adc[i]);
            }
          }
          if (sensor_hub_info_t[sens_ind].data_deliver.data_mode == SL_SH_THRESHOLD) {
            DEBUGOUT("%dmV \t", sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[0].adc[0]);
          }
        }
        vout =
          (((float)*(sensor_hub_info_t[sens_ind].sensor_data_ptr->sensor_data[0].adc) / (float)SL_SH_ADC_MAX_OP_VALUE)
           * SL_SH_ADC_VREF_VALUE);
        DEBUGOUT("Single ended input: %lfV \t", (double)vout);
      }
      DEBUGOUT(" data_deliver.mode:%d \r\n", sensor_hub_info_t[sens_ind].data_deliver.data_mode);

      // Acknowledge data reception
      event_ack = sensor_id;
      break;

    case SL_SENSOR_CNFG_INVALID:
      DEBUGOUT(" SL_SENSOR_CNFG_INVALID:%u \r\n", sensor_id);

      break;
    case SL_SENSOR_START_FAILED:
      DEBUGOUT("Sensor START failed:%u \r\n", sensor_id);

      break;
    case SL_SENSOR_STOP_FAILED:
      DEBUGOUT("Sensor STOP failed:%u \r\n", sensor_id);

      break;
    case SL_SENSOR_DELETED:
      DEBUGOUT("Sensor deleted:%u \r\n", sensor_id);
      break;

    case SL_SENSOR_DELETE_FAILED:
      DEBUGOUT("Sensor deleted failed:%u \r\n", sensor_id);
      break;
    default:
      break;
  }
}

/**************************************************************************/ /**
 * @fn           void sl_si91x_sensorhub_app_task()
 * @brief        This function perform the all sensor related operations.
 *
 * @param[in]    None
 * @param[out]   None
******************************************************************************/
void sl_si91x_sensorhub_app_task(void)
{

  uint32_t status = 0;
  sl_sensor_id_t sl_sensor_scan_info[SL_MAX_NUM_SENSORS];

  osSemaphoreId_t sl_semaphore_app_task_id;
  osStatus_t sl_semapptaskacq_status;

  osSemaphoreAttr_t sl_app_semaphore_attr_st;
  sl_app_semaphore_attr_st.attr_bits = 0U;
  sl_app_semaphore_attr_st.cb_mem    = NULL;
  sl_app_semaphore_attr_st.cb_size   = 0U;
  sl_app_semaphore_attr_st.name      = NULL;

  DEBUGOUT("\r\n Start Sensor HUB APP Task \r\n");
  sl_semaphore_app_task_id = osSemaphoreNew(1U, 0U, &sl_app_semaphore_attr_st);

  // Register callback handler for getting different events from the sensor hub
  status = sl_si91x_sensorhub_notify_cb_register(sl_si91x_sensor_event_handler, (sl_sensor_id_t *)&event_ack);
  if (status != SL_STATUS_OK) {
    DEBUGOUT("\r\n Unable to create call back info: %lu \r\n", status);
    while (1)
      ;
  }
  // Initialize sensor interface
  status = sl_si91x_sensorhub_init();
  if (status != SL_STATUS_OK) {
    DEBUGOUT("\r\n Sensor Hub Init failed \r\n");
    while (1)
      ;
  }

  // sensor hub scan
  sensor_scan_cnt = sl_si91x_sensorhub_detect_sensors((sl_sensor_id_t *)&sl_sensor_scan_info, SL_MAX_NUM_SENSORS);
  if (sensor_scan_cnt == 0) {
    DEBUGOUT("\r\n No sensor is detected \r\n");
    while (1)
      ;
  }

  // create sensors for scanned sensors
  for (uint32_t sensor_cnt = 0; sensor_cnt < sensor_scan_cnt; sensor_cnt++) {
    status = sl_si91x_sensorhub_create_sensor(sl_sensor_scan_info[sensor_cnt]);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("\r\n Unable to create sensor %d,Error Code:%lu \r\n", sl_sensor_scan_info[sensor_cnt], status);
    }
  }
  // Start the sensor HUb Tasks
  status = sl_si91x_sensor_hub_start();
  if (status != RSI_OK) {
    DEBUGOUT("\r\n  Sensor HUB start failed \r\n");
    while (1)
      ;
  }

  // Start the sensors
  for (uint32_t sensor_cnt = 0; sensor_cnt < sensor_scan_cnt; sensor_cnt++) {
    // start a sensor, data ready events will be posted once data is acquired successfully
    status = sl_si91x_sensorhub_start_sensor(sl_sensor_scan_info[sensor_cnt]);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("\r\n Unable to start sensor %d \r\n", sl_sensor_scan_info[sensor_cnt]);
    }
  }
  while (1) {
    // waiting for the semaphore release
    sl_semapptaskacq_status = osSemaphoreAcquire(sl_semaphore_app_task_id, osWaitForever);
    if (sl_semapptaskacq_status != osOK) {
      DEBUGOUT("\r\n osSemaphoreAcquire failed :%d \r\n", sl_semapptaskacq_status);
    }
  }

  // If you need to delete a specific sensor during runtime, utilize the API listed below.
  /*      while (1) {
          uint32_t delay;
          delay++;
          vTaskDelay(1000);
          if (delay > 100) {
              for (uint32_t sensor_cnt = 0; sensor_cnt < sensor_scan_cnt; sensor_cnt++) {
                  //Stop the sensor
                  sl_si91x_sensorhub_stop_sensor(sl_sensor_scan_info[sensor_cnt]);
              }
        while (1)
          ;
      }
    }*/
}
/**************************************************************************/ /**
 * @fn           void sensorhub_app_init()
 * @brief        This function will update the SCB->VTOR Register with new Vector address.
 *               It will change the core frequency to 20 MHz and create the
 *               thread to the sensor app task.
 * @param[in]    None
 * @param[out]   None
******************************************************************************/
void sensorhub_app_init(void)
{

  // Updating the CPU core clock by 20 MHz to work in PS2 mode
  RSI_IPMU_M20rcOsc_TrimEfuse();
  RSI_PS_FsmHfFreqConfig(20);
  RSI_CLK_M4SocClkConfig(M4CLK, M4_ULPREFCLK, 0);

  // Initializes board UART for Prints
  DEBUGINIT();

  // Create the APP task
  osThreadNew((osThreadFunc_t)sl_si91x_sensorhub_app_task, NULL, &app_thread_attributes);
}
