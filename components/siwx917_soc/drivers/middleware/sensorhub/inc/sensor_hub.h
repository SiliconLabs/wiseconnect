
#ifndef SENSOR_HUB_H_
#define SENSOR_HUB_H_
#include <stdint.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include <event_groups.h>
#include <timers.h>
#include "rsi_chip.h"
#include "rsi_os.h"
#include "hub_hal_intf.h"

#define MAX_NUM_SENSORS        2
#define SENSOR_TASK_STACK_SIZE (512 * 2)
#define SENSOR_TASK_PRIORITY   2
#define EM_TASK_STACK_SIZE     (512 * 2)
#define EM_TASK_PRIORITY       3

#define NPSS_GPIO_IRQHandler IRQ021_Handler
#define NPSS_GPIO_NVIC       NPSS_TO_MCU_GPIO_INTR_IRQn

#define EM_TASK_RUN_TICKS 25

extern const char *SENSOR_TYPE_STRING[];
extern const char *SENSOR_MODE_STRING[];

typedef void (*sl_sensor_signalEvent_t)(uint8_t sensor_id,
                                        uint8_t event,
                                        void *data); ///< Pointer to \ref ARM_USART_SignalEvent : Signal USART Event.

typedef enum {
  POLLING_MODE,   ///< POLLING_MODE
  INTERRUPT_MODE, ///< INTERRUPT_MODE
} sensor_mode_t;

typedef enum { GPIO_0, GPIO_1, GPIO_2, GPIO_3 } gpio_sensor_id_t;

typedef enum {
  SL_SENSOR_STARTED,
  SL_SENSOR_STOPPED,
  SL_SENSOR_DATA_READY,
  SL_SENSOR_CNFG_INVALID,
  SL_SENSOR_START_FAILED,
  SL_SENSOR_STOP_FAILED,
  SL_SENSOR_DELETED,
  SL_SENSOR_DELETE_FAILED,
} sl_sensorhub_event_en;

typedef enum {
#ifdef CONFIG_SENSOR_INCLUDED_LIGHT
  SENSOR_BH1750_ID = (LIGHT_SENSOR_ID << SENSOR_ID_OFFSET) | BH1750_ID, /*!< bh1750 sensor id*/
#endif
  GPIO_SENSE_BUTTON_ID = (GPIO_SENSOR_ID << SENSOR_ID_OFFSET) | GPIO_0,
} sl_sensor_id_en;

typedef enum { RISE_EDGE, FALL_EDGE, LOW_LEVEL, HIGH_LEVEL } sl_gpio_intr_type;

typedef enum { THRESHOLD, TIMEOUT, NUM_OF_SAMPLES, NO_DATA } data_deliver_mode_e;

typedef enum { SENSOR_INVALID, SENSOR_VALID, SENSOR_START, SENSOR_STOP } sl_sensor_status_en;

typedef struct {
  TimerHandle_t timer_handle; /*polling mode*/
  int32_t intr_state;         /*interrupt mode*/
} timer_intr_handle_t;

typedef struct {
  data_deliver_mode_e mode;
  union {
    uint32_t threshold;
    uint32_t timeout;
    uint32_t numOfSamples;
  };
} data_deliver_type;

typedef struct sensor_info {
  uint8_t *name;             /*!< sensor name*/
  uint8_t *desc;             /*!< sensor descriptive message*/
  sl_sensor_id_en sensor_id; /*!< sensor id*/
  union {
    uint8_t addr;
    uint8_t c; /* csn for spi and channel for adc */
  };
  sensor_bus_en bus;
  sensor_mode_t mode;
  sensor_range_t range;
  uint32_t sampling_interval;
  int16_t sampling_intr_req_pin;
  int16_t intr_type;
  data_deliver_type data_deliver;
  sensor_data_group_t *sens_data_ptr;
} sl_sensor_info_t;

typedef struct {
  sl_sensor_id_en sensor_id; /*!< sensor id*/
  sl_sensor_status_en sensor_status;
  sl_sensor_info_t *config;
  TimerHandle_t timer_handle;
  sl_sensor_impl_type *sensor_impl;
  void *driver_handle;
  void *ctrl_handle;
  uint8_t event_bit;
  uint16_t max_samples;
  uint8_t event_ack;
} sensor_handle_t;

typedef struct {
  uint8_t sensor_index;
  sensor_handle_t sensors[MAX_NUM_SENSORS];
} sensor_list_t;

typedef struct {
  uint8_t sensor_list_index;
  uint8_t intr;
} intr_list_t;

typedef struct {
  uint8_t map_index;
  intr_list_t map_table[10];
} intr_list_map_t;

typedef struct {
  sl_sensor_id_en sensor_id;
  sl_sensorhub_event_en event;
  void *sensor_data;
} sl_em_event_st;

typedef struct sl_sensor_cb_info {
  sl_sensor_signalEvent_t cb_event; // Event callback
  sl_sensor_id_en *cb_event_ack;
} sl_sensor_cb_info_t;

typedef struct sl_i2c_config {
  uint16_t i2c_bus_speed;
  uint32_t i2c_slave_addr;
  uint8_t i2c_id;
  uint8_t i2c_data_mode;
  uint8_t i2c_power_state;
  uint8_t i2c_control_mode;
} sl_i2c_config_t;

typedef struct sl_spi_config {
  uint8_t spi_bit_width;
  uint32_t spi_baud;
  uint8_t spi_mode; //!phase and polarity
  uint8_t spi_cs_mode;
} sl_spi_config_t;

typedef struct sl_adc_config {
  uint8_t;
  uint32_t;
} sl_adc_config_t;

typedef struct sl_uart_config {
  uint8_t;
  uint32_t;
} sl_uart_config_t;

typedef struct sl_gpio_config {
  uint8_t;
  uint32_t;
} sl_gpio_config_t;

typedef struct bus_intf_config {
  sl_i2c_config_t i2c_config;
  sl_spi_config_t spi_config;
  sl_adc_config_t adc_config;
  sl_gpio_config_t gpio_config;
  sl_uart_config_t uart_config;
} bus_intf_config_t;

uint8_t sl_sensorhub_init();

uint8_t sl_detect_sensors(sl_sensor_id_en sensor_id_info[], uint8_t num_of_sensors);

int32_t sl_sensorhub_delete_sensor(sl_sensor_id_en sensor_id);

int32_t sl_sensorhub_create_sensor(sl_sensor_id_en sensor_id);

int32_t sl_sensorhub_start_sensor(sl_sensor_id_en sensor_id);

int32_t sl_sensorhub_stop_sensor(sl_sensor_id_en sensor_id);

void EM_post_event(sl_sensor_id_en sensor_id, sl_sensorhub_event_en event, void *dataPtr, TickType_t ticks_to_wait);

void Sensor_Task(void);

void EM_Task(void);

#endif
