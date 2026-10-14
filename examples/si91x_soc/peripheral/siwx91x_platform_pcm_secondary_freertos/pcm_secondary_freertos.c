/***************************************************************************/ /**
 * @file pcm_secondary_freertos.c
 * @brief PCM secondary under FreeRTOS: `pcm_secondary_example_init()` only creates
 *        the task; the task performs driver init, GPIO sync with primary, then one
 *        transmit (wait) / delay / receive (wait) / compare cycle using event flags,
 *        then exits the task.
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 ******************************************************************************/
#include "sl_si91x_pcm.h"
#include "sl_si91x_pcm_config.h"
#include "pcm_secondary_freertos.h"
#include "rsi_debug.h"
#include "sl_si91x_driver_gpio.h"
#include "sl_si91x_clock_manager.h"
#include "cmsis_os2.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define PCM_SECONDARY_BUFFER_SIZE 1024
#define PCM_INSTANCE              SL_SI91X_PCM0_INSTANCE
#define FRAME_SIZE_ALIGNMENT      4
#define FRAME_OFFSET              1

#if PCM_INSTANCE == PCM0
#define PCM_INSTANCE_CONFIG_(config) SL_PCM0##_##config
#else
#define PCM_INSTANCE_CONFIG_(config) SL_ULP_PCM##_##config
#endif

#define PRIMARY_SECONDARY_SYNC_PIN RTE_UULP_GPIO_2_PIN

#define PCM_SECONDARY_EVENT_SEND_COMPLETE    (1U << 0)
#define PCM_SECONDARY_EVENT_RECEIVE_COMPLETE (1U << 1)

/*******************************************************************************
 *************************** LOCAL TYPES / DATA ********************************
 ******************************************************************************/
typedef uint16_t pcm_data_size_t;

static pcm_data_size_t pcm_secondary_data_in[PCM_SECONDARY_BUFFER_SIZE + FRAME_SIZE_ALIGNMENT];
static pcm_data_size_t pcm_secondary_data_out[PCM_SECONDARY_BUFFER_SIZE];
static sl_i2s_handle_t pcm_handle       = NULL;
static osEventFlagsId_t pcm_event_flags = NULL;
static uint8_t pcm_resolution;
static int pcm_sampling_frequency;
static uint16_t mode;

static const osThreadAttr_t pcm_thread_attributes = {
  .name       = "pcm_secondary",
  .stack_size = 4096,
  .priority   = osPriorityLow1,
};

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void pcm_event_callback(uint32_t event);
static sl_status_t pcm_secondary_driver_init(void);
static void pcm_secondary_task(void *argument);
static void compare_loop_back_data(void);
static void remove_pcm_frame_offset(pcm_data_size_t data_buffer[PCM_SECONDARY_BUFFER_SIZE + FRAME_OFFSET]);
static sl_status_t secondary_sync_wait(bool first_sync);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
void pcm_secondary_example_init(void)
{
  osThreadId_t thread_id = osThreadNew((osThreadFunc_t)pcm_secondary_task, NULL, &pcm_thread_attributes);
  if (thread_id == NULL) {
    SL_PRINT_STRING_ERROR("Failed to create pcm_secondary thread\r\n");
    /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */
    return;
  }
}

static sl_status_t pcm_secondary_driver_init(void)
{
  sl_status_t status;

  pcm_sampling_frequency = PCM_INSTANCE_CONFIG_(SAMPLING_RATE);
  pcm_resolution         = PCM_INSTANCE_CONFIG_(RESOLUTION);
  mode                   = PCM_INSTANCE_CONFIG_(MODE);

  for (uint16_t i = 0; i < PCM_SECONDARY_BUFFER_SIZE; i++) {
    pcm_secondary_data_out[i] = i;
  }

  status = sl_si91x_pcm_init(PCM_INSTANCE, &pcm_handle);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("PCM Initialization fail\r\n");
    return status;
  }
  SL_PRINT_STRING_ERROR("PCM Initialization success\r\n");

  pcm_event_flags = osEventFlagsNew(NULL);
  if (pcm_event_flags == NULL) {
    SL_PRINT_STRING_ERROR("PCM event flags create failed\r\n");
    return SL_STATUS_ALLOCATION_FAILED;
  }

  status = sl_si91x_pcm_register_event_callback(pcm_handle, pcm_event_callback);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("PCM user callback register fail\r\n");
    return status;
  }
  SL_PRINT_STRING_ERROR("PCM user callback register success\r\n");

  status = sl_si91x_pcm_set_configuration(pcm_handle, pcm_sampling_frequency, pcm_resolution, mode);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("PCM configuration set fail\r\n");
    return status;
  }
  SL_PRINT_STRING_ERROR("PCM configuration set success\r\n");

  return SL_STATUS_OK;
}

static void pcm_secondary_task(void *argument)
{
  (void)argument;
  sl_status_t status = pcm_secondary_driver_init();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("PCM driver init failed: 0x%lx, exiting task\r\n", status);
    osThreadExit();
  }

  status = secondary_sync_wait(true);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Secondary device synchronization failed with error code: 0x%lx\r\n", status);
    osThreadExit();
  }
  SL_PRINT_STRING_ERROR("Secondary device synchronized successfully with primary device\r\n");

  (void)osEventFlagsClear(pcm_event_flags, PCM_SECONDARY_EVENT_SEND_COMPLETE | PCM_SECONDARY_EVENT_RECEIVE_COMPLETE);

  status = sl_si91x_pcm_transmit_data(pcm_handle, pcm_secondary_data_out, PCM_SECONDARY_BUFFER_SIZE);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("PCM transmit start fail\r\n");
    osThreadExit();
  }
  SL_PRINT_STRING_ERROR("PCM transmit start success\r\n");

  if ((osEventFlagsWait(pcm_event_flags, PCM_SECONDARY_EVENT_SEND_COMPLETE, osFlagsWaitAny, osWaitForever)
       & osFlagsError)
      != 0U) {
    SL_PRINT_STRING_ERROR("PCM send event wait failed\r\n");
    osThreadExit();
  }

  (void)osEventFlagsClear(pcm_event_flags, PCM_SECONDARY_EVENT_RECEIVE_COMPLETE);

  status = secondary_sync_wait(false);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Secondary device synchronization failed with error code: 0x%lx\r\n", status);
    osThreadExit();
  }
  status =
    sl_si91x_pcm_receive_data(pcm_handle, pcm_secondary_data_in, (PCM_SECONDARY_BUFFER_SIZE + FRAME_SIZE_ALIGNMENT));
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("PCM receive start fail\r\n");
    osThreadExit();
  }
  SL_PRINT_STRING_ERROR("PCM receive start success\r\n");

  if ((osEventFlagsWait(pcm_event_flags, PCM_SECONDARY_EVENT_RECEIVE_COMPLETE, osFlagsWaitAny, osWaitForever)
       & osFlagsError)
      != 0U) {
    SL_PRINT_STRING_ERROR("PCM receive event wait failed\r\n");
    osThreadExit();
  }

  compare_loop_back_data();
  SL_PRINT_STRING_ERROR("PCM secondary single-shot transfer complete.\r\n");
  osThreadExit();
}

static void compare_loop_back_data(void)
{
  uint16_t data_index = 0;

  remove_pcm_frame_offset(pcm_secondary_data_in);

  for (data_index = 0; data_index < PCM_SECONDARY_BUFFER_SIZE; data_index++) {
    if (pcm_secondary_data_in[data_index] != pcm_secondary_data_out[data_index]) {
      break;
    }
  }

  if (data_index == PCM_SECONDARY_BUFFER_SIZE) {
    SL_PRINT_STRING_ERROR("Data comparison successful\r\n");
  } else {
    SL_PRINT_STRING_ERROR("Data comparison failed\r\n");
  }
}

static void pcm_event_callback(uint32_t event)
{
  switch (event) {
    case SL_I2S_SEND_COMPLETE:
      if (pcm_event_flags != NULL) {
        (void)osEventFlagsSet(pcm_event_flags, PCM_SECONDARY_EVENT_SEND_COMPLETE);
      }
      break;
    case SL_I2S_RECEIVE_COMPLETE:
      if (pcm_event_flags != NULL) {
        (void)osEventFlagsSet(pcm_event_flags, PCM_SECONDARY_EVENT_RECEIVE_COMPLETE);
      }
      break;
    case SL_I2S_TX_UNDERFLOW:
      break;
    case SL_I2S_RX_OVERFLOW:
      break;
    case SL_I2S_FRAME_ERROR:
      break;
    default:
      break;
  }
}

static void remove_pcm_frame_offset(pcm_data_size_t data_buffer[PCM_SECONDARY_BUFFER_SIZE + FRAME_OFFSET])
{
  for (int i = 0; i < PCM_SECONDARY_BUFFER_SIZE; i++) {
    data_buffer[i] = data_buffer[i + FRAME_OFFSET];
  }
}

static sl_status_t secondary_sync_wait(bool first_sync)
{
  uint8_t pin_value  = 0;
  sl_status_t status = SL_STATUS_OK;
  if (first_sync) {
    status = sl_si91x_gpio_driver_enable_clock((sl_si91x_gpio_select_clock_t)ULPCLK_GPIO);
    if (status != SL_STATUS_OK) {
      return status;
    }

    status = sl_si91x_gpio_driver_select_uulp_npss_receiver(PRIMARY_SECONDARY_SYNC_PIN, GPIO_RECEIVER_EN);
    if (status != SL_STATUS_OK) {
      return status;
    }

    status = sl_si91x_gpio_driver_set_uulp_npss_pin_mux(PRIMARY_SECONDARY_SYNC_PIN, NPSS_GPIO_PIN_MUX_MODE0);
    if (status != SL_STATUS_OK) {
      return status;
    }

    status =
      sl_si91x_gpio_driver_set_uulp_npss_direction(PRIMARY_SECONDARY_SYNC_PIN, (sl_si91x_gpio_direction_t)GPIO_INPUT);
    if (status != SL_STATUS_OK) {
      return status;
    }
  }
  SL_PRINT_STRING_ERROR("Waiting for primary button 0 press to sync with primary.\r\n");

  for (;;) {
    pin_value = sl_si91x_gpio_driver_get_uulp_npss_pin(PRIMARY_SECONDARY_SYNC_PIN);

    if (pin_value == 0) {
      SL_PRINT_STRING_ERROR("Button press detected, synchronization completed\r\n");
      sl_si91x_delay_ms(10);
      break;
    }
    sl_si91x_delay_ms(10);
  }
  while (pin_value == 0) {
    pin_value = sl_si91x_gpio_driver_get_uulp_npss_pin(PRIMARY_SECONDARY_SYNC_PIN);
    sl_si91x_delay_ms(10);
    ;
  }
  sl_si91x_delay_ms(10);

  return status;
}
