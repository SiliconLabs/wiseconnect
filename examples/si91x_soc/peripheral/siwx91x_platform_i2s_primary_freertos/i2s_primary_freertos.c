/***************************************************************************/ /**
 * @file i2s_primary_freertos.c
 * @brief I2S examples functions
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
#include "sl_si91x_i2s.h"
#include "sl_si91x_i2s_config.h"
#include "i2s_primary_freertos.h"
#include "rsi_debug.h"
#include "rsi_rom_table_si91x.h"
#include "rsi_rom_clks.h"
#include "cmsis_os2.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define I2S_PRIMARY_BUFFER_SIZE 1024 // Transmit/Receive buffer size
#define I2S_INSTANCE            0    // I2S instance

#define I2S_EVENT_SEND_COMPLETE    (1U << 0)
#define I2S_EVENT_RECEIVE_COMPLETE (1U << 1)
/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
static uint16_t i2s_primary_data_in[I2S_PRIMARY_BUFFER_SIZE];
static uint16_t i2s_primary_data_out[I2S_PRIMARY_BUFFER_SIZE];
static sl_i2s_handle_t i2s_driver_handle    = NULL;
static sl_i2s_xfer_config_t i2s_xfer_config = { 0 };
static osEventFlagsId_t i2s_event_flags;
static sl_status_t i2s_primary_init_function(void);
static void i2s_primary_task(void *argument);
static const osThreadAttr_t i2s_thread_attributes = {
  .name       = "i2s_primary",
  .stack_size = 2048,
  .priority   = osPriorityLow1,
};

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/

static void i2s_primary_event_callback_handler(uint32_t event);
static sl_status_t i2s_app_compare_data(void);
/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
/***************************************************************************/ /**
 * @brief Initialize the I2S example by creating the FreeRTOS primary task.
 *
 * Creates a new thread that runs i2s_primary_task to perform I2S loop-back
 * transfer operations.
 *
 * @param none
 * @return none
 ******************************************************************************/
void i2s_example_init(void)
{
  osThreadId_t thread_id = osThreadNew((osThreadFunc_t)i2s_primary_task, NULL, &i2s_thread_attributes);
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

/***************************************************************************/ /**
 * @brief Initialize I2S driver, power mode, event flags, and callback.
 *
 * Sets up the I2S peripheral for primary mode operation: fetches driver version,
 * initializes the driver, configures power mode, creates event flags for
 * synchronization, and registers the event callback handler.
 *
 * @param none
 * @return SL_STATUS_OK on success; SL_STATUS_ALLOCATION_FAILED on event flags
 *         creation failure; or the status code from driver init/config/callback
 *         on other failures.
 ******************************************************************************/
static sl_status_t i2s_primary_init_function(void)
{
  sl_status_t status = 0;
  sl_i2s_version_t i2s_version;

  // Filling the data out array with integer values
  for (uint32_t i = 0; i < I2S_PRIMARY_BUFFER_SIZE; i++) {
    i2s_primary_data_out[i] = (uint16_t)i;
  }

  // Driver init: fetch I2S driver version
  i2s_version = sl_si91x_i2s_get_version();
  SL_PRINT_STRING_ERROR("I2S version is fetched successfully\r\n");
  SL_PRINT_STRING_ERROR("API version is %d.%d.%d\r\n", i2s_version.release, i2s_version.major, i2s_version.minor);

  // Initialize I2S peripheral and store driver handle in i2s_driver_handle
  status = sl_si91x_i2s_init(I2S_INSTANCE, &i2s_driver_handle);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("I2S Initialization fail\r\n");
    return status;
  }
  SL_PRINT_STRING_ERROR("I2S Initialization success\r\n");

  // Power mode config: configure ARM full power mode
  status = sl_si91x_i2s_configure_power_mode(i2s_driver_handle, SL_I2S_FULL_POWER);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("I2S power mode config fail\r\n");
    return status;
  }
  SL_PRINT_STRING_ERROR("I2S power mode config success\r\n");

  // Event flags creation (NULL guard is defensive in case init is ever called more than once)
  if (i2s_event_flags == NULL) {
    i2s_event_flags = osEventFlagsNew(NULL);
    if (i2s_event_flags == NULL) {
      SL_PRINT_STRING_ERROR("I2S event flags create failed\r\n");
      return SL_STATUS_ALLOCATION_FAILED;
    }
  }

  // Callback registration: register user callback handler
  status = sl_si91x_i2s_register_event_callback(i2s_driver_handle, i2s_primary_event_callback_handler);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("I2S user callback register fail\r\n");
    return status;
  }
  SL_PRINT_STRING_ERROR("I2S user callback register success\r\n");

  return SL_STATUS_OK;
}
/***************************************************************************/ /**
 * @brief I2S primary FreeRTOS task (single shot): receive -> wait -> compare ->
 *        send -> wait, then exit.
 *
 * Initializes I2S, configures receive mode, waits for receive complete, compares
 * loop-back data, then configures transmit mode and waits for send complete.
 *
 * @param argument Unused task argument (NULL passed from i2s_example_init)
 * @return none
 ******************************************************************************/
static void i2s_primary_task(void *argument)
{
  (void)argument;
  sl_status_t status = 0;
  uint32_t flags     = 0U;

  status = i2s_primary_init_function();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("I2S initialization failed: Error Code : %lu, exiting task\r\n", status);
    osThreadExit();
  }

  i2s_xfer_config.mode          = SL_I2S_MASTER;
  i2s_xfer_config.protocol      = SL_I2S_PROTOCOL;
  i2s_xfer_config.resolution    = SL_I2S0_RESOLUTION;
  i2s_xfer_config.sampling_rate = SL_I2S0_SAMPLING_RATE;
  i2s_xfer_config.sync          = SL_I2S_ASYNC;
  i2s_xfer_config.data_size     = SL_I2S_DATA_SIZE16;

  /* Receive phase: configure receive, start receive, wait for event */
  i2s_xfer_config.transfer_type = SL_I2S_RECEIVE;
  status                        = sl_si91x_i2s_config_transmit_receive(i2s_driver_handle, &i2s_xfer_config);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("I2S receive config fail: Error Code : %lu\r\n", status);
    osThreadExit();
  }
  SL_PRINT_STRING_ERROR("I2S receive config success\r\n");
  status = sl_si91x_i2s_receive_data(i2s_driver_handle, i2s_primary_data_in, I2S_PRIMARY_BUFFER_SIZE);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("I2S receive start fail: Error Code : %lu\r\n", status);
    osThreadExit();
  }
  SL_PRINT_STRING_ERROR("I2S receive start success\r\n");

  flags = osEventFlagsWait(i2s_event_flags, I2S_EVENT_RECEIVE_COMPLETE, osFlagsWaitAll, osWaitForever);
  if ((flags & osFlagsError) != 0U) {
    SL_PRINT_STRING_ERROR("I2S receive event wait failed\r\n");
    osThreadExit();
  }
  SL_PRINT_STRING_ERROR("Data received successfully\r\n");

  if (i2s_app_compare_data() != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("I2S primary data comparison failed\r\n");
  }

  /* Send phase: configure transmit, start transmit, wait for event */
  i2s_xfer_config.transfer_type = SL_I2S_TRANSMIT;
  status                        = sl_si91x_i2s_config_transmit_receive(i2s_driver_handle, &i2s_xfer_config);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("I2S transmit config fail: Error Code : %lu\r\n", status);
    osThreadExit();
  }
  SL_PRINT_STRING_ERROR("I2S transmit config success\r\n");
  status = sl_si91x_i2s_transmit_data(i2s_driver_handle, i2s_primary_data_out, I2S_PRIMARY_BUFFER_SIZE);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("I2S transmit start fail: Error Code : %lu\r\n", status);
    osThreadExit();
  }
  SL_PRINT_STRING_ERROR("I2S transmit start success\r\n");

  flags = osEventFlagsWait(i2s_event_flags, I2S_EVENT_SEND_COMPLETE, osFlagsWaitAll, osWaitForever);
  if ((flags & osFlagsError) != 0U) {
    SL_PRINT_STRING_ERROR("I2S send event wait failed\r\n");
    osThreadExit();
  }
  SL_PRINT_STRING_ERROR("Data send successfully\r\n");

  osThreadExit();
}

/***************************************************************************/ /**
 * @brief Compare loop-back data after receive phase.
 *
 * Compares i2s_primary_data_in with i2s_primary_data_out; returns status based
 * on whether all samples match.
 *
 * @param none
 * @return SL_STATUS_OK on match; SL_STATUS_FAIL on mismatch.
 ******************************************************************************/
static sl_status_t i2s_app_compare_data(void)
{
  for (uint16_t data_index = 0; data_index < I2S_PRIMARY_BUFFER_SIZE; data_index++) {
    if (i2s_primary_data_in[data_index] != i2s_primary_data_out[data_index]) {
      return SL_STATUS_FAIL;
    }
  }
  SL_PRINT_STRING_ERROR("Data comparison successful\r\n");
  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 * @brief I2S event callback handler - sets event flags for send/receive complete.
 *
 * Invoked by the I2S driver when transfer events occur. Sets the appropriate
 * event flag (I2S_EVENT_SEND_COMPLETE or I2S_EVENT_RECEIVE_COMPLETE) to unblock
 * the waiting task.
 *
 * @param event I2S interrupt/event type (e.g. SL_I2S_SEND_COMPLETE,
 *              SL_I2S_RECEIVE_COMPLETE)
 * @return none
 ******************************************************************************/
static void i2s_primary_event_callback_handler(uint32_t event)
{
  switch (event) {
    case SL_I2S_SEND_COMPLETE:
      (void)osEventFlagsSet(i2s_event_flags, I2S_EVENT_SEND_COMPLETE);
      break;
    case SL_I2S_RECEIVE_COMPLETE:
      (void)osEventFlagsSet(i2s_event_flags, I2S_EVENT_RECEIVE_COMPLETE);
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
