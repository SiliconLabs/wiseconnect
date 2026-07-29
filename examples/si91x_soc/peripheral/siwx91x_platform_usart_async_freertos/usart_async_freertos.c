/***************************************************************************/ /**
 * @file usart_async_freertos.c
 * @brief USART asynchronous FreeRTOS example — sends data, receives it back,
 *        and compares via loopback.
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
#include "usart_async_freertos.h"
#include "cmsis_os2.h"
#include "rsi_debug.h"
#include "sl_si91x_usart.h"
#include "sl_status.h"
#include <string.h>

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define USART_BUFFER_SIZE     1024   // Data send and receive length
#define USART_BAUDRATE        115200 // Baud rate <9600-7372800>
#define NON_UC_DEFAULT_CONFIG 0      // Enable this macro to set default config in non-UC case

#define USART_SEND_COMPLETE_FLAG    (1 << 0)
#define USART_RECEIVE_COMPLETE_FLAG (1 << 1)
#define USART_IO_COMPLETE_FLAGS     (USART_SEND_COMPLETE_FLAG | USART_RECEIVE_COMPLETE_FLAG)

/*******************************************************************************
 *************************** LOCAL VARIABLES   *********************************
 ******************************************************************************/
static uint8_t usart_data_in[USART_BUFFER_SIZE];  // Buffer for received data
static uint8_t usart_data_out[USART_BUFFER_SIZE]; // Buffer for transmitted data
static sl_usart_handle_t usart_handle;            // USART driver handle

/* Event flags set from the driver ISR callback (osEventFlagsSet is ISR-safe on this CMSIS-RTOS2 port). */
static osEventFlagsId_t usart_event_flags;

/* FreeRTOS task attributes for the USART async task */
static const osThreadAttr_t usart_thread_attributes = {
  .name       = "usart_async_task",
  .stack_size = 2048,
  .priority   = osPriorityLow1,
};

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static sl_status_t usart_async_init_function(void);
static void usart_async_task(void *argument);
static void usart_async_event_callback_handler(uint32_t event);
static sl_status_t usart_app_compare_data(void);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
/*******************************************************************************
 * @brief  Entry point: creates the USART async FreeRTOS task.
 * @return None
 ******************************************************************************/
void usart_async_example_init(void)
{
  osThreadId_t thread_id = osThreadNew((osThreadFunc_t)usart_async_task, NULL, &usart_thread_attributes);
  if (thread_id == NULL) {
    /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */
    SL_PRINT_STRING_ERROR("Failed to create USART async task\r\n");
  }
}

/*******************************************************************************
 * @brief  Initializes the USART peripheral and registers the event callback.
 * @return sl_status_t SL_STATUS_OK on success, or an error code.
 ******************************************************************************/
static sl_status_t usart_async_init_function(void)
{
  sl_status_t status                           = 0;
  sl_si91x_usart_control_config_t usart_config = { 0 };

#if NON_UC_DEFAULT_CONFIG
  usart_config.baudrate      = USART_BAUDRATE;
  usart_config.mode          = SL_USART_MODE_ASYNCHRONOUS;
  usart_config.parity        = SL_USART_NO_PARITY;
  usart_config.stopbits      = SL_USART_STOP_BITS_1;
  usart_config.hwflowcontrol = SL_USART_FLOW_CONTROL_NONE;
  usart_config.databits      = SL_USART_DATA_BITS_8;
  usart_config.misc_control  = SL_USART_MISC_CONTROL_NONE;
  usart_config.usart_module  = USART_0;
  usart_config.config_enable = ENABLE;
  usart_config.synch_mode    = DISABLE;
#endif

  sl_si91x_usart_control_config_t get_config;

  /* Initialize the USART peripheral */
  status = sl_si91x_usart_init(USART_0, &usart_handle);
  DEBUGINIT();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_usart_init: Error Code : %lu\r\n", status);
    return status;
  }
  SL_PRINT_STRING_ERROR("USART initialization is successful\r\n");

  /* Apply USART configuration (baud rate, data bits, parity, etc.) */
  status = sl_si91x_usart_set_configuration(usart_handle, &usart_config);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_usart_set_configuration: Error Code : %lu\r\n", status);
    return status;
  }
  SL_PRINT_STRING_ERROR("USART configuration is successful\r\n");

  /* Register callback (runs on ISR path); sets event flags for the task */
  status = sl_si91x_usart_multiple_instance_register_event_callback(USART_0, usart_async_event_callback_handler);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_usart_multiple_instance_register_event_"
                          "callback: Error Code : %lu\r\n",
                          status);
    return status;
  }
  SL_PRINT_STRING_ERROR("USART user event callback registered successfully\r\n");

  /* Print configured baud rate for verification */
  sl_si91x_usart_get_configurations(USART_0, &get_config);
  SL_PRINT_STRING_ERROR("Baud Rate = %ld\r\n", get_config.baudrate);

  return SL_STATUS_OK;
}

/*******************************************************************************
 * @brief  FreeRTOS task (single shot): async send/receive, compare, exit.
 * @param  argument  Unused task argument.
 ******************************************************************************/
static void usart_async_task(void *argument)
{
  (void)argument;
  sl_status_t status = 0;
  uint32_t flags     = 0U;

  /* Event object must exist before USART init registers the ISR callback. */
  usart_event_flags = osEventFlagsNew(NULL);
  if (usart_event_flags == NULL) {
    SL_PRINT_STRING_ERROR("Failed to create USART event flags\r\n");
    osThreadExit();
  }

  status = usart_async_init_function();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("USART init failed\r\n");
    osThreadExit();
  }

  /* Fill the TX buffer with a test pattern */
  for (uint16_t i = 0; i < USART_BUFFER_SIZE; i++) {
    usart_data_out[i] = (uint8_t)(i + 1);
  }

  memset(usart_data_in, 0, sizeof(usart_data_in));

  (void)osEventFlagsClear(usart_event_flags, USART_IO_COMPLETE_FLAGS);

  /* Start receive while transmit is still in progress (same ordering as the polling example).
   * Waiting for send-complete before receive starves the RX path on loopback and can overflow. */
  status = sl_si91x_usart_async_send_data(usart_handle, usart_data_out, sizeof(usart_data_out));
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_usart_async_send_data: Error Code : %lu\r\n", status);
    osThreadExit();
  }

  status = sl_si91x_usart_async_receive_data(usart_handle, usart_data_in, sizeof(usart_data_in));
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_usart_async_receive_data: Error Code : %lu\r\n", status);
    osThreadExit();
  }

  flags = osEventFlagsWait(usart_event_flags, USART_IO_COMPLETE_FLAGS, osFlagsWaitAll, osWaitForever);
  if ((flags & osFlagsError) != 0U) {
    SL_PRINT_STRING_ERROR("USART send/receive event wait failed\r\n");
    osThreadExit();
  }
  SL_PRINT_STRING_ERROR("USART async send and receive completed\r\n");

  if (usart_app_compare_data() != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("USART data comparison failed\r\n");
  }

  osThreadExit();
}

/*******************************************************************************
 * @brief  USART event callback — invoked from the driver interrupt path.
 *         Sets event flags; CMSIS-RTOS2 uses ISR-safe primitives when IS_IRQ().
 * @param  event  USART event (send complete, receive complete, etc.)
 ******************************************************************************/
static void usart_async_event_callback_handler(uint32_t event)
{
  if (usart_event_flags == NULL) {
    return;
  }
  switch (event) {
    case SL_USART_EVENT_SEND_COMPLETE:
      osEventFlagsSet(usart_event_flags, USART_SEND_COMPLETE_FLAG);
      break;
    case SL_USART_EVENT_RECEIVE_COMPLETE:
      osEventFlagsSet(usart_event_flags, USART_RECEIVE_COMPLETE_FLAG);
      break;
    default:
      break;
  }
}

/*******************************************************************************
 * @brief  Compares the TX and RX buffers and prints the loopback test result.
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on mismatch.
 ******************************************************************************/
static sl_status_t usart_app_compare_data(void)
{
  uint16_t data_index = 0;
  for (data_index = 0; data_index < USART_BUFFER_SIZE; data_index++) {
    if (usart_data_in[data_index] != usart_data_out[data_index]) {
      return SL_STATUS_FAIL;
    }
  }
  SL_PRINT_STRING_ERROR("Data comparison successful, Loop Back Test Passed\r\n");
  return SL_STATUS_OK;
}
