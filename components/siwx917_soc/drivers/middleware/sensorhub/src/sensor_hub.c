/*******************************************************************************
* @file  sensor_hub.c
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <string.h>
#include "sensor_hub.h"
#include "sensor_type.h"
#include "rsi_board.h"
#include "rsi_os.h"
#include "hub_hal_intf.h"

uint8_t sl_i2c_Init();
uint8_t i2c_sensors_scan(uint16_t address);

extern sl_sensor_impl_type sensor_impls[];
extern ARM_DRIVER_I2C Driver_I2C2;
extern sl_sensor_info_t sensor_info[MAX_NUM_SENSORS];
extern bus_intf_config_t bus_intf_info;

const char *SL_SENSOR_TYPE_STRING[] = { "NULL", "HUMITURE", "IMU", "LIGHTSENSOR" };
const char *SL_SENSOR_MODE_STRING[] = { "MODE_DEFAULT", "MODE_POLLING", "MODE_INTERRUPT" };

sl_sensor_cb_info_t cb_info;
QueueHandle_t event_queue;
SemaphoreHandle_t event_queue_mutex;
EventGroupHandle_t sl_event_group = NULL;
uint32_t sl_sensor_wait_flags     = 0x1;
SemaphoreHandle_t sl_sensor_mutex = NULL;

/*Handles for sensor hub tasks*/
rsi_task_handle_t sensor_task_handle = NULL;
rsi_task_handle_t em_task_handle     = NULL;

ARM_DRIVER_I2C *I2Cdrv = &Driver_I2C2;

/*Sensor Data RAM*/
/*TODO: the sensor_data_ram must be mapped to ULP RAM*/
uint8_t sensor_data_ram[1024];
uint32_t free_ram_index = 0;

/*Sensor List, to maintain info of sensors created*/
sensor_list_t sensor_list;
/*Map table for interrupt vs sensor*/
intr_list_map_t int_list_map;

/** @addtogroup SOC26
* @{
*/

/*==============================================*/
/**
 *  @fn          sl_sensor_impl_type* sl_get_sensor_impl(int32_t sensor_id)
 *  @brief       To get the appropriate sensor driver implementation.
 *  @param[in]   sensor_id     ID of the target sensor
 *  @return      Reference to the sensor driver
 */
sl_sensor_impl_type *sl_get_sensor_impl(int32_t sensor_id)
{
  int count = get_impl_size();

  for (int i = 0; i < count; i++) {
    if (sensor_impls[i].type == sensor_id) {
      return &sensor_impls[i];
    }
  }
  return NULL;
}

/*==============================================*/
/**
 *  @fn          int32_t create_sensor_list_index()
 *  @brief       To create a sensor index in the list.
 *  @return      sensor index in the list.
 */
int32_t create_sensor_list_index()
{
  uint8_t sensor_index;

  for (sensor_index = 0; sensor_index < MAX_NUM_SENSORS; sensor_index++) {
    if (SENSOR_INVALID == sensor_list.sensors[sensor_index].sensor_status) {
      break;
    }
  }
  return sensor_index;
}

/*==============================================*/
/**
 *  @fn          int32_t get_sensor_index(sl_sensor_id_en sensor_id)
 *  @brief       To get the index of the sensor present in the list
 *  @param[in]   sensor_id     id of the target sensor
 *  @return      index of the sensor
 */
int32_t get_sensor_index(sl_sensor_id_en sensor_id)
{
  uint8_t i;
  for (i = 0; i < MAX_NUM_SENSORS; i++) {
    if ((sensor_list.sensors[i].sensor_id == sensor_id) && (sensor_list.sensors[i].sensor_status != SENSOR_INVALID)) {
      return i;
    }
  }
  return 0xff;
}

/*==============================================*/
/**
 *  @fn          int32_t delete_sensor_list_index(sl_sensor_id_en sensor_id)
 *  @brief       To delete the sensor index from the list
 *  @param[in]   sensor_id     id of the target sensor
 *  @return      index of the sensor
 */
int32_t delete_sensor_list_index(sl_sensor_id_en sensor_id)
{
  uint8_t i;
  for (i = 0; i < MAX_NUM_SENSORS; i++) {
    if (sensor_list.sensors[i].sensor_id == sensor_id) {
      sensor_list.sensors[i].sensor_id = 0;
      return i;
    }
  }
  return 0xff;
}
/*==============================================*/
/**
 *  @fn          sl_sensor_info_t* get_sensor_info(sl_sensor_id_en sensor_id)
 *  @brief       To get the info on the configured sensor
 *  @param[in]   sensor_id     id of the target sensor
 *  @return      reference to the sensor info
 */
sl_sensor_info_t *get_sensor_info(sl_sensor_id_en sensor_id)
{
  uint8_t sens_index = 0;
  for (sens_index = 0; sens_index < MAX_NUM_SENSORS; sens_index++) {
    if (sensor_info[sens_index].sensor_id == sensor_id) {
      return &sensor_info[sens_index];
    }
  }
  return NULL;
}

/*==============================================*/
/**
 *  @fn          void sl_sensorhub_notify_cb_register(sl_sensor_signalEvent_t cb_event, sl_sensor_id_en *cb_ack)
 *  @brief       To register the application callback handler
 *  @param[in]   cb_event     pointer to the event
 *  @param[in]   cb_ack       Pointer to the application handler
 */
void sl_sensorhub_notify_cb_register(sl_sensor_signalEvent_t cb_event, sl_sensor_id_en *cb_ack)
{
  //!register call back handler for application
  cb_info.cb_event     = cb_event;
  cb_info.cb_event_ack = cb_ack;
}

/*==============================================*/
/**
 *  @fn          void sl_sensors_timer_cb(TimerHandle_t xTimer)
 *  @brief       software timer callback
 *  @param[in]   xTimer     handle to the timer
 */
void sl_sensors_timer_cb(TimerHandle_t xTimer)
{
  uint32_t event_bit = (uint32_t)pvTimerGetTimerID(xTimer);
  xEventGroupSetBits(sl_event_group, (0x01 << event_bit));
}

/*==============================================*/
/**
 *  @fn          void NPSS_GPIO_IRQHandler(void)
 *  @brief       NPSS GPIO IRQ handler
 */
void NPSS_GPIO_IRQHandler(void)
{
  volatile uint32_t intrStatus = 0;

  intrStatus = RSI_NPSSGPIO_GetIntrStatus();
  for (uint8_t idx = 0; idx < int_list_map.map_index; idx++) {
    if (BIT(int_list_map.map_table[idx].intr) == intrStatus) {
      RSI_NPSSGPIO_ClrIntr(BIT(int_list_map.map_table[idx].intr));
      xEventGroupSetBitsFromISR(sl_event_group, (0x01 << int_list_map.map_table[idx].sensor_list_index), 64);
    }
  }
}

/*==============================================*/
/**
 *  @fn          void Interrupt_config(uint16_t gpio_pin,sl_gpio_intr_type intr_type)
 *  @brief       Configuring the different types of GPIO interrupts.
 *  @param[in]   gpio_pin     GPIO pin number
 *  @param[in]   intr_type    type of interrupt.
 */
void Interrupt_config(uint16_t gpio_pin, sl_gpio_intr_type intr_type)
{
  /* Enable NPSS GPIO input buffer */
  RSI_NPSSGPIO_InputBufferEn(gpio_pin, 1);

  /* Set the NPSS GPIO pin MUX */
  RSI_NPSSGPIO_SetPinMux(gpio_pin, 2);

  /* Set the direction of the NPSS GPIO */
  RSI_NPSSGPIO_SetDir(gpio_pin, NPSS_GPIO_DIR_INPUT);

  /*Program interrupt type */
  switch (intr_type) {
    case RISE_EDGE:
      RSI_NPSSGPIO_SetIntRiseEdgeEnable(BIT(gpio_pin));
      break;
    case FALL_EDGE:
      RSI_NPSSGPIO_SetIntFallEdgeEnable(BIT(gpio_pin));
      break;
    case LOW_LEVEL:
      RSI_NPSSGPIO_SetIntLevelLowEnable(BIT(gpio_pin));
      break;
    case HIGH_LEVEL:
      RSI_NPSSGPIO_SetIntLevelHighEnable(BIT(gpio_pin));
      break;
  }
}

/*==============================================*/
/**
 *  @fn          void Interrupt_Start(uint16_t gpio_pin)
 *  @brief       Enable and set the priority of GPIO interrupt.
 *  @param[in]   gpio_pin     GPIO pin number
 */
void Interrupt_Start(uint16_t gpio_pin)
{
  /* unmask the NPSS GPIO interrupt */
  RSI_NPSSGPIO_IntrUnMask(BIT(gpio_pin));

  /* Set NVIC priority less than syscall priority */
  NVIC_SetPriority(NPSS_TO_MCU_GPIO_INTR_IRQn, configMAX_SYSCALL_INTERRUPT_PRIORITY - 1);

  /*  NVIC Enable */
  NVIC_EnableIRQ(NPSS_TO_MCU_GPIO_INTR_IRQn);
}

/*==============================================*/
/**
 *  @fn          void Interrupt_Stop(uint16_t gpio_pin)
 *  @brief       Mask and Disable the GPIO interrupt.
 *  @param[in]   gpio_pin     GPIO pin number
 */
void Interrupt_Stop(uint16_t gpio_pin)
{
  /*mask the NPSS GPIO interrupt */
  RSI_NPSSGPIO_IntrMask(BIT(gpio_pin));

  /*  NVIC Disable */
  NVIC_DisableIRQ(NPSS_TO_MCU_GPIO_INTR_IRQn);
}

/*==============================================*/
/**
 *  @fn          void Interrupt_Delete(void)
 *  @brief       Dissable the GPIO interrupt.
 */
void Interrupt_Delete(void)
{
  /*  NVIC Disable */
  NVIC_DisableIRQ(NPSS_TO_MCU_GPIO_INTR_IRQn);
}

/*==============================================*/
/**
 *  @fn          uint8_t sl_spi_Init(void)
 *  @brief       Initialize the SPI Interface based on the configuration.
 */
uint8_t sl_spi_Init(void)
{
  //!TODO: This API needs to be implemented
  return 0;
}

/*==============================================*/
/**
 *  @fn          uint8_t sl_uart_Init(void)
 *  @brief       Initialize the UART Interface based on the configuration.
 */
uint8_t sl_uart_Init(void)
{
  //!TODO: This API needs to be implemented
  return 0;
}

/*==============================================*/
/**
 *  @fn           uint8_t sl_adc_Init(void)
 *  @brief       Initialize the ADC Interface based on the configuration.
 */
uint8_t sl_adc_Init(void)
{
  //!TODO: This API needs to be implemented
  return 0;
}

/*==============================================*/
/**
 *  @fn          uint8_t sl_sensorhub_init()
 *  @brief       To initialize the sensor hub
 *  @return      execution status
 */
uint8_t sl_sensorhub_init()
{
  uint32_t status = 0;

  status = sl_i2c_Init();
  if (status != RSI_OK) {
    return RSI_FAIL;
  }

  /*TODO: SPI, ADC, and UART initializations should be handled */
  status = sl_spi_Init();
  if (status != RSI_OK) {
    return RSI_FAIL;
  }

  status = sl_uart_Init();
  if (status != RSI_OK) {
    return RSI_FAIL;
  }

  status = sl_adc_Init();
  if (status != RSI_OK) {
    return RSI_FAIL;
  }

  return RSI_OK;
}

/*==============================================*/
/**
 *  @fn          int32_t sensor_hub_start()
 *  @brief       Start the sensor hub operations
 *  @return      execution status
 */
int32_t sensor_hub_start()
{
  int8_t status = 0;
  //!create sensor and event tasks
  status = rsi_task_create((void *)Sensor_Task,
                           (uint8_t *)"sensor_task",
                           SENSOR_TASK_STACK_SIZE,
                           NULL,
                           SENSOR_TASK_PRIORITY,
                           &sensor_task_handle);
  if (status != RSI_OK) {
    return RSI_FAIL;
  }

  rsi_task_create((void *)EM_Task, (uint8_t *)"em_task", EM_TASK_STACK_SIZE, NULL, EM_TASK_PRIORITY, &em_task_handle);
  if (status != RSI_OK) {
    return RSI_FAIL;
  }

  return RSI_OK;
}

/*==============================================*/
/**
 *  @fn          uint8_t sl_detect_sensors(sl_sensor_id_en sensor_id_info[],uint8_t num_of_sensors)
 *  @brief       To Scan the available sensors in the system.
 *  @param[in]   sensor_id_info    reference to the detected sensors info
 *  @param[in]   num_of_sensors    number of sensors to be scanned
 *  @return      execution status
 */
uint8_t sl_detect_sensors(sl_sensor_id_en sensor_id_info[], uint8_t num_of_sensors)
{
  uint8_t cnt, status = 0, sensors_detected = 0;
  for (cnt = 0; cnt < num_of_sensors; cnt++) {
    switch (sensor_info[cnt].bus) {
      case I2C:
        status = i2c_sensors_scan(BH1750_I2C_ADDRESS_DEFAULT);
        if (status != RSI_OK) {
          return RSI_FAIL;
        }
        break;

      case SPI:
        break;

      case ADC:
        break;

      case GPIO:
        break;

      default:
        break;
    }

    if (status == 0) {
      sensor_id_info[sensors_detected++] = sensor_info[cnt].sensor_id;
    }
  }
  return sensors_detected;
}

/*==============================================*/
/**
 *  @fn          int32_t sl_sensorhub_create_sensor(sl_sensor_id_en sensor_id)
 *  @brief       To Create a sensor instance in the sensor hub.
 *  @param[in]   sensor_id     ID of the sensor
 *  @return      execution status
 */
int32_t sl_sensorhub_create_sensor(sl_sensor_id_en sensor_id)
{
  sl_sensor_info_t *local_info;
  uint8_t sensor_index;
  uint8_t status             = 0;
  uint32_t ramAllocationSize = 0;
  char sl_sensor_timer_name[16];

  local_info = get_sensor_info(sensor_id);
  if (NULL == local_info) {
    /*If there is no config found for the given sensor then return an error*/
    return RSI_FAIL;
  }

  /*Add the sensor to the sensor list*/
  sensor_index = create_sensor_list_index();
  if (sensor_index == MAX_NUM_SENSORS) {
    /* Invalid sensor index */
    return RSI_FAIL;
  }
  sensor_list.sensors[sensor_index].sensor_id = sensor_id;
  sensor_list.sensors[sensor_index].event_bit = sensor_index;
  sensor_list.sensors[sensor_index].config    = local_info;

  /*Allocate a data ram for the sensor to store the sampling data*/
  // if (sensor_list.sensors[sensor_index].config->sens_data_ptr == NULL) {

  switch (sensor_list.sensors[sensor_index].config->data_deliver.mode) {
    case THRESHOLD:
      ramAllocationSize = 2 + sizeof(sensor_data_t);
      break;

    case TIMEOUT:
      ramAllocationSize = 2
                          + (sizeof(sensor_data_t)
                             * ((sensor_list.sensors[sensor_index].config->data_deliver.timeout
                                 / sensor_list.sensors[sensor_index].config->sampling_interval)
                                + 1));
      break;

    case NUM_OF_SAMPLES:
      ramAllocationSize =
        2 + (sizeof(sensor_data_t) * (sensor_list.sensors[sensor_index].config->data_deliver.numOfSamples));
      break;

    default:
      /*Do nothing*/
      break;
  }

  /*Check if Sensor RAM is available*/
  if ((free_ram_index + ramAllocationSize) > sizeof(sensor_data_ram)) {
    return RSI_FAIL;
  }

  sensor_list.sensors[sensor_index].config->sens_data_ptr = &sensor_data_ram[free_ram_index];
  free_ram_index += ramAllocationSize;
  sensor_list.sensors[sensor_index].max_samples = (ramAllocationSize - 2) / sizeof(sensor_data_t);
  //}

  /*Find the Sensor HAL Implementation*/
  sensor_list.sensors[sensor_index].sensor_impl = sl_get_sensor_impl((sensor_id & SENSOR_ID_MASK) >> SENSOR_ID_OFFSET);
  if (sensor_list.sensors[sensor_index].sensor_impl == NULL) {
    return RSI_FAIL;
  }
  /*Create a sensor*/
  sensor_list.sensors[sensor_index].driver_handle =
    sensor_list.sensors[sensor_index].sensor_impl->create(sensor_list.sensors[sensor_index].config->bus,
                                                          (sensor_list.sensors[sensor_index].config->addr));
  if (sensor_list.sensors[sensor_index].driver_handle == NULL) {
    return RSI_FAIL;
  }

  sensor_list.sensors[sensor_index].ctrl_handle =
    sensor_list.sensors[sensor_index].sensor_impl->control(&sensor_list.sensors[sensor_index].config->sensor_id,
                                                           COMMAND_SET_POWER,
                                                           NULL);
  if (sensor_list.sensors[sensor_index].ctrl_handle != RSI_OK) {
    return RSI_FAIL;
  }

  sensor_list.sensors[sensor_index].ctrl_handle =
    sensor_list.sensors[sensor_index].sensor_impl->control(&sensor_list.sensors[sensor_index].config->sensor_id,
                                                           COMMAND_SET_RANGE,
                                                           NULL);
  if (sensor_list.sensors[sensor_index].ctrl_handle != RSI_OK) {
    return RSI_FAIL;
  }

  sensor_list.sensors[sensor_index].ctrl_handle =
    sensor_list.sensors[sensor_index].sensor_impl->control(&sensor_list.sensors[sensor_index].config->sensor_id,
                                                           COMMAND_SELF_TEST,
                                                           NULL);
  if (sensor_list.sensors[sensor_index].ctrl_handle != RSI_OK) {
    return RSI_FAIL;
  }

  snprintf(sl_sensor_timer_name,
           sizeof(sl_sensor_timer_name) - 1,
           "%s%02x",
           SL_SENSOR_TYPE_STRING[sensor_list.sensors[sensor_index].sensor_impl->type],
           sensor_id);

  /*Check the sensor mode. poll/interrupt and create the timer*/
  switch (local_info->mode) {
    case POLLING_MODE:
      sensor_list.sensors[sensor_index].timer_handle =
        xTimerCreate(sl_sensor_timer_name,
                     (sensor_list.sensors[sensor_index].config->sampling_interval / portTICK_RATE_MS),
                     1,
                     (void *)(sensor_list.sensors[sensor_index].event_bit),
                     sl_sensors_timer_cb);
      if (sensor_list.sensors[sensor_index].timer_handle == NULL) {
        return RSI_FAIL;
      }
      break;

    case INTERRUPT_MODE:
      Interrupt_config(sensor_list.sensors[sensor_index].config->sampling_intr_req_pin,
                       sensor_list.sensors[sensor_index].config->intr_type);
      int_list_map.map_table[int_list_map.map_index].intr =
        sensor_list.sensors[sensor_index].config->sampling_intr_req_pin;
      int_list_map.map_table[int_list_map.map_index].sensor_list_index = sensor_index;
      int_list_map.map_index++;
      break;

    default:
      break;
  }

  /* update sensor status into sensor list */
  sensor_list.sensors[sensor_index].sensor_status = SENSOR_VALID;

  return RSI_OK;
}

/*==============================================*/
/**
 *  @fn          int32_t sl_sensorhub_delete_sensor(sl_sensor_id_en sensor_id)
 *  @brief       To delete a sensor from the sensor hub.
 *  @param[in]   sensor_id     ID of the sensor
 *  @return      execution status
 */
int32_t sl_sensorhub_delete_sensor(sl_sensor_id_en sensor_id)
{

  uint8_t sensor_index;
  uint8_t status = 0;

  /*Delete the sensor to the sensor list*/
  sensor_index = delete_sensor_list_index(sensor_id);
  if (sensor_index == MAX_NUM_SENSORS) {
    /* Invalid sensor index */
    return RSI_FAIL;
  }

  /*Check the sensor mode. poll/interrupt and create the timer*/
  switch (sensor_list.sensors[sensor_index].config->mode) {
    case POLLING_MODE:
      status = xTimerDelete(sensor_list.sensors[sensor_index].timer_handle, portMAX_DELAY);
      if (status != pdPASS) {
        /* Post-event as SL_SENSOR_START_FAILED */
        EM_post_event(sensor_id, SL_SENSOR_DELETE_FAILED, NULL, 50);
        return RSI_FAIL;
      }
      break;

    case INTERRUPT_MODE:
      Interrupt_Delete();
      break;

    default:
      break;
  }

  sensor_list.sensors[sensor_index].ctrl_handle =
    sensor_list.sensors[sensor_index].sensor_impl->control(&sensor_list.sensors[sensor_index].config->sensor_id,
                                                           COMMAND_SET_POWER,
                                                           (void *)POWER_MODE_SLEEP);
  if (status != RSI_OK) {
    return RSI_FAIL;
  }

  //!delete sensor
  sensor_list.sensors[sensor_index].ctrl_handle =
    sensor_list.sensors[sensor_index].sensor_impl->delete (&sensor_list.sensors[sensor_index].config->sensor_id);
  if (status != RSI_OK) {
    return RSI_FAIL;
  }

  /* update sensor status into sensor list */
  sensor_list.sensors[sensor_index].sensor_status = SENSOR_INVALID;

  //!post SENSOR_STARTED event to application
  EM_post_event(sensor_id, SL_SENSOR_DELETED, NULL, 50);

  return RSI_OK;
}

/*==============================================*/
/**
 *  @fn          int32_t sl_sensorhub_start_sensor(sl_sensor_id_en sensor_id)
 *  @brief       To start the sensor operation for the given sensor
 *  @param[in]   sensor_id     ID of the sensor
 *  @return      execution status
 */
int32_t sl_sensorhub_start_sensor(sl_sensor_id_en sensor_id)
{
  uint8_t sensor_index, status = 0;
  sensor_index = get_sensor_index(sensor_id);
  if (sensor_index == 0xff) {
    EM_post_event(sensor_id, SL_SENSOR_START_FAILED, NULL, 50);
    return RSI_FAIL;
  }

  switch (sensor_list.sensors[sensor_index].config->mode) {
    case POLLING_MODE:
      status = xTimerStart(sensor_list.sensors[sensor_index].timer_handle, portMAX_DELAY);
      if (status != pdPASS) {
        /* Post event as SL_SENSOR_START_FAILED */
        EM_post_event(sensor_id, SL_SENSOR_START_FAILED, NULL, 50);
        return RSI_FAIL;
      }
      break;

    case INTERRUPT_MODE:
      Interrupt_Start(sensor_list.sensors[sensor_index].config->sampling_intr_req_pin);

      break;
    default:
      //!Post-event to the application as sensor config invalid
      EM_post_event(sensor_id, SL_SENSOR_CNFG_INVALID, NULL, 50);
      return RSI_FAIL;
  }

  sl_sensor_wait_flags |= 0x1 << sensor_list.sensors[sensor_index].event_bit;

  /* update sensor status into sensor list */
  sensor_list.sensors[sensor_index].sensor_status = SENSOR_START;

  //!post SENSOR_STARTED event to application
  EM_post_event(sensor_id, SL_SENSOR_STARTED, NULL, 50);

  sensor_list.sensors[sensor_index].ctrl_handle =
    sensor_list.sensors[sensor_index].sensor_impl->control(&sensor_list.sensors[sensor_index].config->sensor_id,
                                                           COMMAND_SET_POWER,
                                                           NULL);
  if (status != RSI_OK) {
    return RSI_FAIL;
  }

  return RSI_OK;
}

/*==============================================*/
/**
 *  @fn          int32_t sl_sensorhub_stop_sensor(sl_sensor_id_en sensor_id)
 *  @brief       To stop the sensor operation for the given sensor
 *  @param[in]   sensor_id     ID of the sensor
 *  @return      execution status
 */
int32_t sl_sensorhub_stop_sensor(sl_sensor_id_en sensor_id)
{
  uint8_t sensor_index, status = 0;
  sensor_index = get_sensor_index(sensor_id);
  if (sensor_index == 0xff) {
    EM_post_event(sensor_id, SL_SENSOR_STOP_FAILED, NULL, 50);
    return RSI_FAIL;
  }
  switch (sensor_list.sensors[sensor_index].config->mode) {
    case POLLING_MODE:
      status = xTimerStop(sensor_list.sensors[sensor_index].timer_handle, portMAX_DELAY);
      if (status != pdPASS) {
        /* Post event as SL_SENSOR_STOP_FAILED */
        EM_post_event(sensor_id, SL_SENSOR_STOP_FAILED, NULL, 50);
        return RSI_FAIL;
      }
      break;

    case INTERRUPT_MODE:
      Interrupt_Stop(sensor_list.sensors[sensor_index].config->sampling_intr_req_pin);

      break;
    default:
      //!Post-event to the application as sensor config invalid
      EM_post_event(sensor_id, SL_SENSOR_CNFG_INVALID, NULL, 50);
      return RSI_FAIL;
  }

  sl_sensor_wait_flags &= ~(0x1 << sensor_list.sensors[sensor_index].event_bit);

  /* update sensor status into sensor list */
  sensor_list.sensors[sensor_index].sensor_status = SENSOR_STOP;

  //!post SENSOR_STOPPED event to application
  EM_post_event(sensor_id, SL_SENSOR_STOPPED, NULL, 50);

  sensor_list.sensors[sensor_index].ctrl_handle =
    sensor_list.sensors[sensor_index].sensor_impl->control(&sensor_list.sensors[sensor_index].config->sensor_id,
                                                           COMMAND_SET_POWER,
                                                           NULL);

  return RSI_OK;
}

/*==============================================*/
/**
 *  @fn          void EM_post_event(sl_sensor_id_en sensor_id,sl_sensorhub_event_en event, void* dataPtr, TickType_t ticks_to_wait)
 *  @brief       To post the events/notifications to event manager(EM) to be notified to the application
 *  @param[in]   sensor_id      id of the  sensor
 *  @param[in]   event          event/notifications
 *  @param[in]   dataPtr        pointer to the data
 *  @param[in]   ticks_to_wait  max time to wait for the post
 */
void EM_post_event(sl_sensor_id_en sensor_id, sl_sensorhub_event_en event, void *dataPtr, TickType_t ticks_to_wait)
{
  sl_em_event_st em_event;
  em_event.sensor_id   = sensor_id;
  em_event.event       = event;
  em_event.sensor_data = dataPtr;

  BaseType_t mutex_result = pdFALSE;
  mutex_result            = xSemaphoreTakeRecursive(event_queue_mutex, ticks_to_wait);

  if (mutex_result == pdTRUE) {
    xSemaphoreGiveRecursive(event_queue_mutex);
    mutex_result = xQueueSendToBack(event_queue, &em_event, 0);
  }
}

/*==============================================*/
/**
 *  @fn          void EM_Task(void)
 *  @brief       Task to handle the operations of the Event Manager(EM)
 */
void EM_Task(void)
{

  sl_em_event_st em_event;

  /*Create an Event Queue*/
  event_queue = xQueueCreate(20, sizeof(sl_em_event_st));
  if (event_queue == NULL) {
    printf("create event queue failed");
    while (1)
      ;
  }

  /*Create a recursive Mutex to safe guard Event Queue*/
  event_queue_mutex = xSemaphoreCreateRecursiveMutex();
  if (event_queue_mutex == NULL) {
    printf("create event mutex failed");
    while (1)
      ;
  }

  while (1) {
    /*Wait for the Event on Queue*/
    while ((xQueueReceive(event_queue, &em_event, EM_TASK_RUN_TICKS) == pdTRUE)) {
      /*Acquire the mutex*/
      xSemaphoreTakeRecursive(event_queue_mutex, portMAX_DELAY);

      /*Call the handler and notify the event */
      cb_info.cb_event(em_event.sensor_id, em_event.event, em_event.sensor_data);

      if (*cb_info.cb_event_ack != NULL) {
        for (uint8_t i = 0; i < MAX_NUM_SENSORS; i++) {
          if (sensor_list.sensors[i].sensor_id == *cb_info.cb_event_ack) {
            sensor_list.sensors[i].config->sens_data_ptr->number = 0;
            sensor_list.sensors[i].event_ack                     = 0;
            *cb_info.cb_event_ack                                = (sl_sensor_id_en)NULL;
          }
        }
      }

      /*Release the mutex*/
      xSemaphoreGiveRecursive(event_queue_mutex);
    }

    if (*cb_info.cb_event_ack != NULL) {
      for (uint8_t i = 0; i < MAX_NUM_SENSORS; i++) {
        if (sensor_list.sensors[i].sensor_id == *cb_info.cb_event_ack) {
          sensor_list.sensors[i].config->sens_data_ptr->number = 0;
          sensor_list.sensors[i].event_ack                     = 0;
          *cb_info.cb_event_ack                                = (sl_sensor_id_en)NULL;
        }
      }
    }
  }
}

/*==============================================*/
/**
 *  @fn          void Sensor_Task(void)
 *  @brief       Task to handle the sensor operations.
 */
void Sensor_Task(void)
{
  sl_event_group  = xEventGroupCreate();
  sl_sensor_mutex = xSemaphoreCreateMutex();
  uint32_t event_flags;
  uint8_t i;
  if (sl_sensor_mutex == NULL) {
    printf("create event mutex failed");
    while (1)
      ;
  }

  while (1) {

    event_flags = xEventGroupWaitBits(sl_event_group, sl_sensor_wait_flags, 1, 0, portMAX_DELAY);

    xSemaphoreTake(sl_sensor_mutex, portMAX_DELAY);
    i = 0;
    while (event_flags) {

      if ((event_flags & 0x1) == 0x1) {

        if (sensor_list.sensors[i].event_ack == 0) {
          if (sensor_list.sensors[i].config->data_deliver.mode == THRESHOLD) {
            sensor_list.sensors[i].config->sens_data_ptr->number = 0;
          }
          if (sensor_list.sensors[i].config->data_deliver.mode != NO_DATA)
            sensor_list.sensors[i].sensor_impl->sample(
              sensor_list.sensors[i].driver_handle,
              (sensor_data_group_t *)sensor_list.sensors[i].config->sens_data_ptr);
        }

        switch (sensor_list.sensors[i].config->data_deliver.mode) {
          case THRESHOLD:
            if (sensor_list.sensors[i]
                  .config->sens_data_ptr->sensor_data[sensor_list.sensors[i].config->sens_data_ptr->number - 1]
                  .light
                >= sensor_list.sensors[i].config->data_deliver.threshold) {
              if (sensor_list.sensors[i].event_ack == 0) {
                sensor_list.sensors[i].event_ack = 1;
                EM_post_event(sensor_list.sensors[i].sensor_id,
                              SL_SENSOR_DATA_READY,
                              sensor_list.sensors[i].config->sens_data_ptr,
                              50);
              }
            }
            break;

          case TIMEOUT:
            if (sensor_list.sensors[i].config->sens_data_ptr->number
                == (sensor_list.sensors[i].config->data_deliver.timeout
                    / sensor_list.sensors[i].config->sampling_interval)) {
              if (sensor_list.sensors[i].event_ack == 0) {
                sensor_list.sensors[i].event_ack = 1;
                EM_post_event(sensor_list.sensors[i].sensor_id,
                              SL_SENSOR_DATA_READY,
                              sensor_list.sensors[i].config->sens_data_ptr,
                              50);
              }
            }
            break;

          case NUM_OF_SAMPLES:
            if (sensor_list.sensors[i].config->sens_data_ptr->number
                == (sensor_list.sensors[i].config->data_deliver.numOfSamples)) {
              if (sensor_list.sensors[i].event_ack == 0) {
                sensor_list.sensors[i].event_ack = 1;
                EM_post_event(sensor_list.sensors[i].sensor_id,
                              SL_SENSOR_DATA_READY,
                              sensor_list.sensors[i].config->sens_data_ptr,
                              50);
              }
            }
            break;

          case NO_DATA:
            if (sensor_list.sensors[i].event_ack == 0) {
              sensor_list.sensors[i].event_ack = 1;
              EM_post_event(sensor_list.sensors[i].sensor_id,
                            SL_SENSOR_DATA_READY,
                            sensor_list.sensors[i].config->sens_data_ptr,
                            50);
            }
            break;

          default:
            /*Do nothing*/
            break;
        }
      }
      i++;
      event_flags = event_flags >> 1;
    }
    xSemaphoreGive(sl_sensor_mutex);
  }
}

/*==============================================*/
/**
 * @fn         void ARM_I2C_SignalEvent(uint32_t event)
 * @brief      I2C event handler
 * @param[in]  event I2C transmit and receive events
 * @return     None
 */
void ARM_I2C_SignalEvent(uint32_t event)
{
  switch (event) {
    case ARM_I2C_EVENT_TRANSFER_DONE:
      break;
    case ARM_I2C_EVENT_TRANSFER_INCOMPLETE:
      break;
    case ARM_I2C_EVENT_SLAVE_TRANSMIT:
      break;
    case ARM_I2C_EVENT_SLAVE_RECEIVE:
      break;
    case ARM_I2C_EVENT_ADDRESS_NACK:
      break;
    case ARM_I2C_EVENT_GENERAL_CALL:
      break;
    case ARM_I2C_EVENT_ARBITRATION_LOST:
      break;
    case ARM_I2C_EVENT_BUS_ERROR:
      break;
    case ARM_I2C_EVENT_BUS_CLEAR:
      break;
  }
  return;
}

/*==============================================*/
/**
 *  @fn          uint8_t sl_i2c_Init()
 *  @brief       Initialize I2C Interface based on configuration
 *  @return      execution status
 */
uint8_t sl_i2c_Init()
{
  int32_t status = ARM_DRIVER_OK;

  status = I2Cdrv->Initialize(ARM_I2C_SignalEvent);
  if (status != ARM_DRIVER_OK) {
    DEBUGOUT("\r\nI2C Initialization Failed, Error Code : %d\r\n", status);
    return status;
  } else {
    DEBUGOUT("\r\n I2C Initialization Success \r\n");
  }

  status = I2Cdrv->PowerControl(bus_intf_info.i2c_config.i2c_power_state);
  if (status != ARM_DRIVER_OK) {
    DEBUGOUT("\r\nFailed to Set Power to I2C, Error Code : %d\r\n", status);
    return status;
  } else {
    DEBUGOUT("\r\n Set Power mode to I2C is Success \r\n");
  }

  status = I2Cdrv->Control(bus_intf_info.i2c_config.i2c_control_mode, bus_intf_info.i2c_config.i2c_bus_speed);
  if (status != ARM_DRIVER_OK) {
    DEBUGOUT("\r\nFailed to Set Configuration Parameters to I2C, Error Code : %d\r\n", status);
    return status;
  } else {
    DEBUGOUT("\r\n Set Control mode to I2C is Success\r\n");
  }
  return status;
}

/*==============================================*/
/**
 *  @fn          uint8_t i2c_sensors_scan(uint16_t address)
 *  @brief       Scan the i2c sensors based on the address
 *  @param[in]   address Sensor address
 *  @return      execution status
 */
uint8_t i2c_sensors_scan(uint16_t address)
{
  uint8_t a[2] = "\0", len = 0;
  uint8_t rd_buf[] = "\0";

  a[0] = (uint8_t)(address & 0xFF);

  I2Cdrv->MasterTransmit(address, a, 1, false);
  while (I2Cdrv->GetStatus().busy)
    ;
  return RSI_OK;
}
/** @} */