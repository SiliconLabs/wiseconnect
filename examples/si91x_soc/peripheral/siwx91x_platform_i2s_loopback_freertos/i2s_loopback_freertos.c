/***************************************************************************/ /**
 * @file i2s_loopback_freertos.c
 * @brief I2S FreeRTOS loopback example (single shot): init starts one TX and one
 *        RX; the task waits for both completions via event flags, compares data,
 *        then exits the task.
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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
/* I2S driver and project config */
#include "sl_si91x_i2s.h"
#include "sl_si91x_i2s_config.h"
#include "i2s_loopback_freertos.h"
#include "rsi_debug.h"
#include "rsi_rom_table_si91x.h"
#include "rsi_rom_clks.h"
/* FreeRTOS / CMSIS-RTOS2 */
#include "cmsis_os2.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define I2S_BUFFER_SIZE 1024 // Transmit/Receive buffer size in bytes
#define I2S_INSTANCE    0    // I2S instance index
/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
/* Loopback buffers: TX data is compared with RX after transfer (file-scope only) */
static uint8_t i2s_data_in[I2S_BUFFER_SIZE];
static uint8_t i2s_data_out[I2S_BUFFER_SIZE];
static sl_i2s_handle_t i2s_driver_handle    = NULL;
static sl_i2s_xfer_config_t i2s_xfer_config = { 0 };
/* Signalled by callback when send/receive complete (I2S_EVENT_SEND/RECEIVE_COMPLETE) */
static osEventFlagsId_t i2s_event_flags;
static sl_status_t i2s_init_function(void);
static void i2s_loopback_task(void *argument);
static const osThreadAttr_t i2s_thread_attributes = {
  .name       = "i2s_loopback",
  .stack_size = 2048,
  .priority   = osPriorityLow1,
};

/* Event flag bits set by i2s_event_callback_handler */
#define I2S_EVENT_SEND_COMPLETE    (1U << 0)
#define I2S_EVENT_RECEIVE_COMPLETE (1U << 1)

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/

static void i2s_event_callback_handler(uint32_t event);
static sl_status_t i2s_app_compare_data(void);
/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
/*******************************************************************************
 * @brief  Entry point: creates the I2S loopback FreeRTOS task.
 * @return None
 ******************************************************************************/
void i2s_example_init(void)
{
  /* Task runs init, waits once for TX+RX complete, compares, then exits */
  osThreadId_t thread_id = osThreadNew((osThreadFunc_t)i2s_loopback_task, NULL, &i2s_thread_attributes);
  if (thread_id == NULL) {
    /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */

    SL_PRINT_STRING_ERROR("Failed to create i2s thread\r\n");
    return;
  }
}
/*******************************************************************************
 * @brief  One-time I2S init: version, init, power mode, event flags, callback,
 *         TX/RX config and start of first transfer (loopback).
 * @return SL_STATUS_OK on success, error code on failure
 ******************************************************************************/
static sl_status_t i2s_init_function(void)
{
  sl_status_t status = 0;
  sl_i2s_version_t i2s_version;
  sl_i2s_status_t i2s_status;
  /* I2S transfer configuration (mode, protocol, resolution, etc.) */
  i2s_xfer_config.mode          = SL_I2S_MASTER;
  i2s_xfer_config.protocol      = SL_I2S_PROTOCOL;
  i2s_xfer_config.resolution    = SL_I2S0_RESOLUTION;
  i2s_xfer_config.sampling_rate = SL_I2S0_SAMPLING_RATE;
  i2s_xfer_config.sync          = SL_I2S_ASYNC;
  i2s_xfer_config.data_size     = SL_I2S_DATA_SIZE8;

  // Filling the data out array with integer values
  for (uint32_t i = 0; i < I2S_BUFFER_SIZE; i++) {
    i2s_data_out[i] = (uint8_t)i;
  }

  //Fetch I2S driver version
  i2s_version = sl_si91x_i2s_get_version();
  SL_PRINT_STRING_ERROR("I2S version is fetched successfully\r\n");
  SL_PRINT_STRING_ERROR("API version is %d.%d.%d\r\n", i2s_version.release, i2s_version.major, i2s_version.minor);

  //Initialize I2S peripheral and store driver handle in i2s_driver_handle
  status = sl_si91x_i2s_init(I2S_INSTANCE, &i2s_driver_handle);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("I2S Initialization fail\r\n");
    return status;
  }
  SL_PRINT_STRING_ERROR("I2S Initialization success\r\n");

  //Get the status of I2S peripheral
  i2s_status = sl_si91x_i2s_get_status(i2s_driver_handle);
  SL_PRINT_STRING_ERROR("I2S status is fetched successfully\r\n");
  SL_PRINT_STRING_ERROR("Frame error: %d\r\n", i2s_status.frame_error);
  SL_PRINT_STRING_ERROR("Rx status: %d\r\n", i2s_status.rx_busy);
  SL_PRINT_STRING_ERROR("Rx overflow status: %d\r\n", i2s_status.rx_overflow);
  SL_PRINT_STRING_ERROR("Tx status: %d\r\n", i2s_status.tx_busy);
  SL_PRINT_STRING_ERROR("Tx underflow status: %d\r\n", i2s_status.tx_underflow);

  //Configure ARM full power mode
  status = sl_si91x_i2s_configure_power_mode(i2s_driver_handle, SL_I2S_FULL_POWER);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("I2S power mode config fail\r\n");
    return status;
  }
  SL_PRINT_STRING_ERROR("I2S power mode config success\r\n");

  //Create new Event Flags
  i2s_event_flags = osEventFlagsNew(NULL);
  if (i2s_event_flags == NULL) {
    SL_PRINT_STRING_ERROR("I2S event flags create failed\r\n");
    return SL_STATUS_ALLOCATION_FAILED;
  }

  //Register user callback handler
  status = sl_si91x_i2s_register_event_callback(i2s_driver_handle, i2s_event_callback_handler);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("I2S user callback register fail\r\n");
    return status;
  }
  SL_PRINT_STRING_ERROR("I2S user callback register success\r\n");

  i2s_xfer_config.transfer_type = SL_I2S_TRANSMIT;
  /* Configure transmitter parameters for I2S transfer */
  status = sl_si91x_i2s_config_transmit_receive(i2s_driver_handle, &i2s_xfer_config);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("I2S transmit config fail\r\n");
    return status;
  }
  SL_PRINT_STRING_ERROR("I2S transmit config success\r\n");

  i2s_xfer_config.transfer_type = SL_I2S_RECEIVE;
  /* Configure receiver parameters for I2S transfer */
  status = sl_si91x_i2s_config_transmit_receive(i2s_driver_handle, &i2s_xfer_config);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("I2S receive config fail\r\n");
    return status;
  }
  SL_PRINT_STRING_ERROR("I2S receive config success\r\n");

  //Configure I2S receive DMA channel
  //Since 8-bit resolution is not supported in Si91x I2S module, configure receive data as 16-bit
  //chunks which contains two bytes of 8-bit data.
  status = sl_si91x_i2s_receive_data(i2s_driver_handle, (uint16_t *)i2s_data_in, I2S_BUFFER_SIZE / 2);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("I2S receive start fail\r\n");
    return status;
  }
  SL_PRINT_STRING_ERROR("I2S receive start success\r\n");

  //Configure I2S transmit DMA channel
  //Since 8-bit resolution is not supported in Si91x I2S module, configure transmit data as 16-bit
  //chunks which contains two bytes of 8-bit data.
  status = sl_si91x_i2s_transmit_data(i2s_driver_handle, (uint16_t *)i2s_data_out, I2S_BUFFER_SIZE / 2);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("I2S transmit start fail\r\n");
    return status;
  }
  SL_PRINT_STRING_ERROR("I2S transmit start success\r\n");

  return SL_STATUS_OK;
}
/*******************************************************************************
 * @brief  I2S loopback FreeRTOS task: init starts one TX and one RX; single wait
 *         for both completions, optional count check, compare, then exit.
 * @param  argument  Unused (NULL)
 * @return None
 ******************************************************************************/
static void i2s_loopback_task(void *argument)
{
  (void)argument;

  sl_status_t init_status = i2s_init_function();
  if (init_status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("I2S initialization failed: Error Code : %lu, exiting task\r\n", init_status);
    osThreadExit();
  }

  /* Single-shot: one loopback transfer; callbacks set flags when TX and RX DMA complete */
  uint32_t flags = osEventFlagsWait(i2s_event_flags,
                                    I2S_EVENT_SEND_COMPLETE | I2S_EVENT_RECEIVE_COMPLETE,
                                    osFlagsWaitAll,
                                    osWaitForever);
  if ((flags & osFlagsError) != 0U) {
    SL_PRINT_STRING_ERROR("I2S event wait failed\r\n");
    osThreadExit();
  }

  if ((sl_si91x_i2s_get_transmit_data_count(i2s_driver_handle) != (uint32_t)(I2S_BUFFER_SIZE / 2))
      || (sl_si91x_i2s_get_receive_data_count(i2s_driver_handle) != (uint32_t)(I2S_BUFFER_SIZE / 2))) {
    SL_PRINT_STRING_ERROR("I2S TX/RX data count mismatch\r\n");
    osThreadExit();
  }

  SL_PRINT_STRING_ERROR("I2S transfer complete\r\n");
  if (i2s_app_compare_data() != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("I2S loopback data comparison failed\r\n");
  }
  osThreadExit();
}

/*******************************************************************************
 * @brief  Compare i2s_data_out (TX) with i2s_data_in (RX); print pass/fail.
 * @param  None
 * @return SL_STATUS_OK if data matches, SL_STATUS_FAIL on first mismatch
 ******************************************************************************/
static sl_status_t i2s_app_compare_data(void)
{
  for (uint16_t data_index = 0; data_index < I2S_BUFFER_SIZE; data_index++) {
    if (i2s_data_in[data_index] != i2s_data_out[data_index]) {
      return SL_STATUS_FAIL;
    }
  }
  SL_PRINT_STRING_ERROR("Data comparison successful, Loop Back Test Passed\r\n");
  return SL_STATUS_OK;
}

/*******************************************************************************
 * @brief  I2S event callback: sets event flags on send/receive complete so
 *         i2s_loopback_task can sync and compare data.
 * @param  event  I2S event (SEND_COMPLETE, RECEIVE_COMPLETE, underflow, etc.)
 * @return None
 ******************************************************************************/
static void i2s_event_callback_handler(uint32_t event)
{
  switch (event) {
    case SL_I2S_SEND_COMPLETE:
      (void)osEventFlagsSet(i2s_event_flags, I2S_EVENT_SEND_COMPLETE);
      break;
    case SL_I2S_RECEIVE_COMPLETE:
      (void)osEventFlagsSet(i2s_event_flags, I2S_EVENT_RECEIVE_COMPLETE);
      break;
    case SL_I2S_TX_UNDERFLOW:
      /* Optional: log or handle TX underflow */
      break;
    case SL_I2S_RX_OVERFLOW:
      /* Optional: log or handle RX overflow */
      break;
    case SL_I2S_FRAME_ERROR:
      /* Optional: log or handle frame error */
      break;
    default:
      break;
  }
}