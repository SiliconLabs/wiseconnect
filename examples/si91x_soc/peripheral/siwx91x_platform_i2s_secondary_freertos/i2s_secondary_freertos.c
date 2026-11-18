/***************************************************************************/ /**
 * @file i2s_secondary_freertos.c
 * @brief I2S Secondary FreeRTOS example functions
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
#include "i2s_secondary_freertos.h"
#include "rsi_debug.h"
#include "rsi_rom_table_si91x.h"
#include "rsi_rom_clks.h"
#include "cmsis_os2.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define I2S_SECONDARY_BUFFER_SIZE 1024 // Transmit/Receive buffer size
#define I2S_INSTANCE              0    // I2S instance

#define I2S_EVENT_SEND_COMPLETE    (1U << 0)
#define I2S_EVENT_RECEIVE_COMPLETE (1U << 1)
/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
static uint16_t i2s_secondary_data_in[I2S_SECONDARY_BUFFER_SIZE];
static uint16_t i2s_secondary_data_out[I2S_SECONDARY_BUFFER_SIZE];
static sl_i2s_handle_t i2s_driver_handle    = NULL;
static sl_i2s_xfer_config_t i2s_xfer_config = { 0 };
static osEventFlagsId_t i2s_event_flags;
static sl_status_t i2s_secondary_init_function(void);
static void i2s_secondary_task(void *argument);
static const osThreadAttr_t i2s_thread_attributes = {
  .name       = "i2s_secondary",
  .stack_size = 2048,
  .priority   = osPriorityLow1,
};

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/

static void i2s_secondary_event_callback_handler(uint32_t event);
static sl_status_t i2s_app_compare_data(void);
/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
/*******************************************************************************
 * I2S example initialization function
 *
 * @brief Creates the I2S secondary FreeRTOS task thread.
 * @return None
 ******************************************************************************/
void i2s_example_init(void)
{
  osThreadId_t thread_id = osThreadNew((osThreadFunc_t)i2s_secondary_task, NULL, &i2s_thread_attributes);
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
 * I2S secondary initialization function
 *
 * @brief Initializes the I2S peripheral, configures power mode, creates event
 *        flags (with guard against double-init), and registers the event callback.
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_ALLOCATION_FAILED on
 *         event flags failure, or the relevant status code on other failures.
 ******************************************************************************/
static sl_status_t i2s_secondary_init_function(void)
{
  sl_status_t status = 0;
  sl_i2s_version_t i2s_version;

  // Pre-fill transmit buffer with sample data for loopback comparison
  for (uint32_t i = 0; i < I2S_SECONDARY_BUFFER_SIZE; i++) {
    i2s_secondary_data_out[i] = (uint16_t)i;
  }

  // Fetch and display I2S driver version
  i2s_version = sl_si91x_i2s_get_version();
  SL_PRINT_STRING_ERROR("I2S version is fetched successfully\r\n");
  SL_PRINT_STRING_ERROR("API version is %d.%d.%d\r\n", i2s_version.release, i2s_version.major, i2s_version.minor);

  // Initialize I2S peripheral and store driver handle
  status = sl_si91x_i2s_init(I2S_INSTANCE, &i2s_driver_handle);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("I2S Initialization fail\r\n");
    return status;
  }
  SL_PRINT_STRING_ERROR("I2S Initialization success\r\n");

  // Configure ARM full power mode
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

  // Register user callback handler for I2S events
  status = sl_si91x_i2s_register_event_callback(i2s_driver_handle, i2s_secondary_event_callback_handler);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("I2S user callback register fail\r\n");
    return status;
  }
  SL_PRINT_STRING_ERROR("I2S user callback register success\r\n");

  return SL_STATUS_OK;
}
/*******************************************************************************
 * I2S secondary task
 *
 * @brief Single-shot send -> wait -> receive -> wait -> compare, then exit.
 * @param argument Unused thread argument (required by CMSIS-RTOS API)
 * @return None
 ******************************************************************************/
static void i2s_secondary_task(void *argument)
{
  (void)argument;
  sl_status_t status = 0;
  uint32_t flags     = 0U;

  status = i2s_secondary_init_function();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("I2S initialization failed: Error Code : %lu, exiting task\r\n", status);
    osThreadExit();
  }

  /* Transfer parameters for slave mode */
  i2s_xfer_config.mode          = SL_I2S_SLAVE;
  i2s_xfer_config.protocol      = SL_I2S_PROTOCOL;
  i2s_xfer_config.resolution    = SL_I2S0_RESOLUTION;
  i2s_xfer_config.sampling_rate = SL_I2S0_SAMPLING_RATE;
  i2s_xfer_config.sync          = SL_I2S_ASYNC;
  i2s_xfer_config.data_size     = SL_I2S_DATA_SIZE16;

  /* Transmit phase */
  i2s_xfer_config.transfer_type = SL_I2S_TRANSMIT;
  status                        = sl_si91x_i2s_config_transmit_receive(i2s_driver_handle, &i2s_xfer_config);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("I2S transmit config fail: Error Code : %lu\r\n", status);
    osThreadExit();
  }
  SL_PRINT_STRING_ERROR("I2S transmit config success\r\n");

  status = sl_si91x_i2s_transmit_data(i2s_driver_handle, i2s_secondary_data_out, I2S_SECONDARY_BUFFER_SIZE);
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

  /* Receive phase */
  i2s_xfer_config.transfer_type = SL_I2S_RECEIVE;
  status                        = sl_si91x_i2s_config_transmit_receive(i2s_driver_handle, &i2s_xfer_config);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("I2S receive config fail: Error Code : %lu\r\n", status);
    osThreadExit();
  }
  SL_PRINT_STRING_ERROR("I2S receive config success\r\n");

  status = sl_si91x_i2s_receive_data(i2s_driver_handle, i2s_secondary_data_in, I2S_SECONDARY_BUFFER_SIZE);
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

  if (i2s_app_compare_data() != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("I2S secondary data comparison failed\r\n");
  }

  osThreadExit();
}

/*******************************************************************************
 * Function to compare the loop back data
 *
 * @brief Compares the transmitted and received buffer contents after I2S
 *        loopback transfer. Reports success or failure via debug output.
 * @param none
 * @return sl_status_t SL_STATUS_OK on success (data match), SL_STATUS_FAIL on
 *         mismatch.
 ******************************************************************************/
static sl_status_t i2s_app_compare_data(void)
{
  uint16_t data_index = 0;
  for (data_index = 0; data_index < I2S_SECONDARY_BUFFER_SIZE; data_index++) {
    if (i2s_secondary_data_in[data_index] != i2s_secondary_data_out[data_index]) {
      return SL_STATUS_FAIL;
    }
  }
  SL_PRINT_STRING_ERROR("Data comparison successful\r\n");
  return SL_STATUS_OK;
}

/*******************************************************************************
 * I2S event callback handler
 *
 * @brief Handles events triggered by the I2S interface. Sets the corresponding
 *        event flags to signal completion to the waiting task.
 * @param event Interrupt/event type (e.g. SL_I2S_SEND_COMPLETE, SL_I2S_RECEIVE_COMPLETE)
 * @return none
 ******************************************************************************/
static void i2s_secondary_event_callback_handler(uint32_t event)
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
