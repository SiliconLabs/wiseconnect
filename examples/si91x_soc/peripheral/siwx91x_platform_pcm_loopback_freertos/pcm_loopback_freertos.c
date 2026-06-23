/***************************************************************************/ /**
 * @file pcm_loopback_freertos.c
 * @brief PCM FreeRTOS loopback: init starts one TX/RX transfer;
 *        task waits for both completions via event flags, compares data, aborts,
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
#include "pcm_loopback_freertos.h"
#include "rsi_debug.h"
#include "cmsis_os2.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define PCM_BUFFER_SIZE      1024
#define PCM_INSTANCE         SL_SI91X_PCM0_INSTANCE
#define FRAME_SIZE_ALIGNMENT 4
#define FRAME_OFFSET         2

#if PCM_INSTANCE == PCM0
#define PCM_INSTANCE_CONFIG_(config) SL_PCM0##_##config
#else
#define PCM_INSTANCE_CONFIG_(config) SL_ULP_PCM##_##config
#endif

#define PCM_LB_EVENT_SEND_COMPLETE    (1U << 0)
#define PCM_LB_EVENT_RECEIVE_COMPLETE (1U << 1)

/*******************************************************************************
 *************************** LOCAL TYPES / DATA ********************************
 ******************************************************************************/
typedef uint16_t pcm_data_size_t;

static pcm_data_size_t pcm_data_in[PCM_BUFFER_SIZE + FRAME_SIZE_ALIGNMENT];
static pcm_data_size_t pcm_data_out[PCM_BUFFER_SIZE];
static sl_i2s_handle_t pcm_handle           = NULL;
static sl_i2s_xfer_config_t pcm_xfer_config = { 0 };
static osEventFlagsId_t pcm_event_flags     = NULL;

static uint8_t pcm_resolution;
static int pcm_sampling_frequency;
static int mode;

static const osThreadAttr_t pcm_thread_attributes = {
  .name       = "pcm_loopback",
  .stack_size = 4096,
  .priority   = osPriorityLow1,
};

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void pcm_event_callback(uint32_t event);
static sl_status_t pcm_loopback_init_function(void);
static void pcm_loopback_task(void *argument);
static void compare_loop_back_data(void);
static void remove_pcm_frame_offset(pcm_data_size_t data_buffer[PCM_BUFFER_SIZE + FRAME_OFFSET]);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
void pcm_example_init(void)
{
  osThreadId_t thread_id = osThreadNew((osThreadFunc_t)pcm_loopback_task, NULL, &pcm_thread_attributes);
  if (thread_id == NULL) {
    SL_PRINT_STRING_ERROR("Failed to create pcm_loopback thread\r\n");
    /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */
    return;
  }
}

static sl_status_t pcm_loopback_init_function(void)
{
  sl_status_t status;
  sl_i2s_status_t pcm_status;

  pcm_sampling_frequency = PCM_INSTANCE_CONFIG_(SAMPLING_RATE);
  pcm_resolution         = PCM_INSTANCE_CONFIG_(RESOLUTION);
  mode                   = PCM_INSTANCE_CONFIG_(MODE);

  for (uint16_t i = 0; i < PCM_BUFFER_SIZE; i++) {
    pcm_data_out[i] = (uint16_t)i;
  }

  status = sl_si91x_pcm_init(PCM_INSTANCE, &pcm_handle);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("PCM Initialization fail\r\n");
    return status;
  }
  SL_PRINT_STRING_ERROR("PCM Initialization success\r\n");

  pcm_status = sl_si91x_pcm_get_status(pcm_handle);
  SL_PRINT_STRING_ERROR("PCM status is fetched successfully \n");
  SL_PRINT_STRING_ERROR("Frame error: %d\n", pcm_status.frame_error);
  SL_PRINT_STRING_ERROR("Rx status: %d\n", pcm_status.rx_busy);
  SL_PRINT_STRING_ERROR("Rx overflow status: %d\n", pcm_status.rx_overflow);
  SL_PRINT_STRING_ERROR("Tx status: %d\n", pcm_status.tx_busy);
  SL_PRINT_STRING_ERROR("Tx underflow status: %d\n", pcm_status.tx_underflow);

  status = sl_si91x_pcm_set_configuration(pcm_handle, pcm_sampling_frequency, pcm_resolution, mode);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("PCM configuration set fail\r\n");
    return status;
  }
  SL_PRINT_STRING_ERROR("PCM configuration set success\r\n");

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

  pcm_xfer_config.mode          = mode;
  pcm_xfer_config.protocol      = SL_PCM_PROTOCOL;
  pcm_xfer_config.resolution    = pcm_resolution;
  pcm_xfer_config.sampling_rate = pcm_sampling_frequency;
  pcm_xfer_config.sync          = SL_I2S_ASYNC;
  pcm_xfer_config.transfer_type = SL_I2S_TRANSMIT;
  pcm_xfer_config.data_size     = SL_I2S_DATA_SIZE16;
  status                        = sl_si91x_pcm_config_transmit_receive(pcm_handle, &pcm_xfer_config);
  if (status != SL_STATUS_OK) {
    DEBUGOUT("PCM transmit invalid config \r\n");
    return status;
  }
  pcm_xfer_config.transfer_type = SL_I2S_RECEIVE;
  status                        = sl_si91x_pcm_config_transmit_receive(pcm_handle, &pcm_xfer_config);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("PCM receive invalid config \r\n");
    return status;
  }

  status = sl_si91x_pcm_transfer(pcm_handle,
                                 (uint16_t *)pcm_data_in,
                                 (uint16_t *)pcm_data_out,
                                 PCM_BUFFER_SIZE + FRAME_SIZE_ALIGNMENT,
                                 PCM_BUFFER_SIZE);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("PCM transfer start failed, status = 0x%lx\r\n", status);
    return status;
  }
  SL_PRINT_STRING_ERROR("PCM transfer success\r\n");

  return SL_STATUS_OK;
}

static void pcm_loopback_task(void *argument)
{
  (void)argument;
  uint32_t flags;
  sl_status_t status;

  sl_status_t init_status = pcm_loopback_init_function();
  if (init_status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("PCM initialization failed: Error Code : %lu, exiting task\r\n", init_status);
    osThreadExit();
  }

  flags = osEventFlagsWait(pcm_event_flags,
                           PCM_LB_EVENT_SEND_COMPLETE | PCM_LB_EVENT_RECEIVE_COMPLETE,
                           osFlagsWaitAll,
                           osWaitForever);
  if ((flags & osFlagsError) != 0U) {
    SL_PRINT_STRING_ERROR("PCM event wait failed\r\n");
    osThreadExit();
  }

  if ((sl_si91x_pcm_get_transmit_data_count(pcm_handle) != (uint32_t)PCM_BUFFER_SIZE)
      || (sl_si91x_pcm_get_receive_data_count(pcm_handle) != (uint32_t)(PCM_BUFFER_SIZE + FRAME_SIZE_ALIGNMENT))) {
    SL_PRINT_STRING_ERROR("PCM TX/RX data count mismatch\r\n");
    osThreadExit();
  }

  SL_PRINT_STRING_ERROR("PCM transfer complete\r\n");
  compare_loop_back_data();
  status = sl_si91x_pcm_end_transfer(pcm_handle, SL_I2S_SEND_ABORT);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("PCM send abort failed: 0x%lx\r\n", (unsigned long)status);
    osThreadExit();
  }
  status = sl_si91x_pcm_end_transfer(pcm_handle, SL_I2S_RECEIVE_ABORT);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("PCM receive abort failed: 0x%lx\r\n", (unsigned long)status);
    osThreadExit();
  }
  osThreadExit();
}

static void compare_loop_back_data(void)
{
  uint16_t data_index = 0;

  remove_pcm_frame_offset(pcm_data_in);

  for (data_index = 0; data_index < PCM_BUFFER_SIZE; data_index++) {
    if (pcm_data_in[data_index] != pcm_data_out[data_index]) {
      break;
    }
  }

  if (data_index == PCM_BUFFER_SIZE) {
    SL_PRINT_STRING_ERROR("Data comparison successful, Loop Back Test Passed \n");
  } else {
    SL_PRINT_STRING_ERROR("Data comparison failed, Loop Back Test failed \n");
  }
}

static void pcm_event_callback(uint32_t event)
{
  switch (event) {
    case SL_I2S_SEND_COMPLETE:
      if (pcm_event_flags != NULL) {
        (void)osEventFlagsSet(pcm_event_flags, PCM_LB_EVENT_SEND_COMPLETE);
      }
      break;
    case SL_I2S_RECEIVE_COMPLETE:
      if (pcm_event_flags != NULL) {
        (void)osEventFlagsSet(pcm_event_flags, PCM_LB_EVENT_RECEIVE_COMPLETE);
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

static void remove_pcm_frame_offset(pcm_data_size_t data_buffer[PCM_BUFFER_SIZE + FRAME_OFFSET])
{
  for (int i = 0; i < PCM_BUFFER_SIZE; i++) {
    data_buffer[i] = data_buffer[i + FRAME_OFFSET];
  }
}
