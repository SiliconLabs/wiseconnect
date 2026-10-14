/***************************************************************************/ /**
 * @file uart_freertos.c
 * @brief UART FreeRTOS example — sends data, receives it back, and compares.
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
#include "uart_freertos.h"
#include "cmsis_os2.h"
#include "rsi_debug.h"
#include "sl_si91x_usart.h"
#include "sl_status.h"
#include <string.h>

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define UART_BUFFER_SIZE      1024   // Data send and receive length
#define UART_BAUDRATE         115200 // Baud rate <9600-7372800>
#define NON_UC_DEFAULT_CONFIG 0      // Enable this macro to set the default configurations in non-UC case

#define UART_SEND_COMPLETE_FLAG    (1 << 0)
#define UART_RECEIVE_COMPLETE_FLAG (1 << 1)
#define UART_IO_COMPLETE_FLAGS     (UART_SEND_COMPLETE_FLAG | UART_RECEIVE_COMPLETE_FLAG)

/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
static uint8_t uart_data_in[UART_BUFFER_SIZE];  // Buffer for received data
static uint8_t uart_data_out[UART_BUFFER_SIZE]; // Buffer for transmitted data
static sl_usart_handle_t uart_handle;           // UART driver handle

/* Event flags set from the driver ISR callback (osEventFlagsSet is ISR-safe on this CMSIS-RTOS2 port). */
static osEventFlagsId_t uart_event_flags;

/* FreeRTOS task attributes for the UART task */
static const osThreadAttr_t uart_thread_attributes = {
  .name       = "uart_task",
  .stack_size = 2048,
  .priority   = osPriorityLow1,
};

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static sl_status_t uart_init_function(void);
static void uart_task(void *argument);
static void uart_event_callback_handler(uint32_t event);
static sl_status_t uart_app_compare_data(void);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
/*******************************************************************************
 * @brief  Entry point: creates the UART FreeRTOS task.
 * @return None
 ******************************************************************************/
void uart_example_init(void)
{
  osThreadId_t thread_id = osThreadNew((osThreadFunc_t)uart_task, NULL, &uart_thread_attributes);
  if (thread_id == NULL) {
    /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */
    SL_PRINT_STRING_ERROR("Failed to create UART task\r\n");
  }
}

/*******************************************************************************
 * @brief  Initializes the UART peripheral and registers the event callback.
 * @return sl_status_t SL_STATUS_OK on success, or an error code.
 ******************************************************************************/
static sl_status_t uart_init_function(void)
{
  sl_status_t status                          = 0;
  sl_si91x_usart_control_config_t uart_config = { 0 };

#if NON_UC_DEFAULT_CONFIG
  uart_config.baudrate      = UART_BAUDRATE;
  uart_config.mode          = SL_USART_MODE_ASYNCHRONOUS;
  uart_config.parity        = SL_USART_NO_PARITY;
  uart_config.stopbits      = SL_USART_STOP_BITS_1;
  uart_config.hwflowcontrol = SL_USART_FLOW_CONTROL_NONE;
  uart_config.databits      = SL_USART_DATA_BITS_8;
  uart_config.misc_control  = SL_USART_MISC_CONTROL_NONE;
  uart_config.usart_module  = UART_1;
  uart_config.config_enable = ENABLE;
  uart_config.synch_mode    = DISABLE;
#endif

  sl_si91x_usart_control_config_t get_config;

  /* Initialize the UART peripheral */
  status = sl_si91x_usart_init(UART_1, &uart_handle);
  DEBUGINIT();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_usart_init: Error Code : %lu\r\n", status);
    return status;
  }
  SL_PRINT_STRING_ERROR("UART initialization is successful\r\n");

  /* Apply UART configuration (baud rate, data bits, parity, etc.) */
  status = sl_si91x_usart_set_configuration(uart_handle, &uart_config);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_usart_set_configuration: Error Code : %lu\r\n", status);
    return status;
  }
  SL_PRINT_STRING_ERROR("UART configuration is successful\r\n");

  /* Register event callback; the ISR will set event flags to wake the task */
  status = sl_si91x_usart_multiple_instance_register_event_callback(UART_1, uart_event_callback_handler);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_usart_register_event_callback: Error Code : %lu\r\n", status);
    return status;
  }
  SL_PRINT_STRING_ERROR("UART user event callback registered successfully\r\n");

  /* Print configured baud rate for verification */
  sl_si91x_usart_get_configurations(UART_1, &get_config);
  SL_PRINT_STRING_ERROR("Baud Rate = %ld\r\n", get_config.baudrate);

  return SL_STATUS_OK;
}

/*******************************************************************************
 * @brief  FreeRTOS task (single shot): send, receive, compare, exit.
 * @param  argument  Unused task argument.
 ******************************************************************************/
static void uart_task(void *argument)
{
  (void)argument;
  sl_status_t status = 0;
  uint32_t flags     = 0U;

  /* Event object must exist before init registers the ISR callback. */
  uart_event_flags = osEventFlagsNew(NULL);
  if (uart_event_flags == NULL) {
    SL_PRINT_STRING_ERROR("Failed to create UART event flags\r\n");
    osThreadExit();
  }

  status = uart_init_function();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("UART init failed\r\n");
    osThreadExit();
  }

  /* Fill the TX buffer with a test pattern */
  for (uint16_t i = 0; i < UART_BUFFER_SIZE; i++) {
    uart_data_out[i] = (uint8_t)(i + 1);
  }

  memset(uart_data_in, 0, sizeof(uart_data_in));

  (void)osEventFlagsClear(uart_event_flags, UART_IO_COMPLETE_FLAGS);

  /* Start receive soon after send begins so TX/RX overlap on loopback. Waiting for send-complete
   * before receive can starve RX and hang receive. */
  status = sl_si91x_usart_send_data(uart_handle, uart_data_out, sizeof(uart_data_out));
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_usart_send_data: Error Code : %lu\r\n", status);
    osThreadExit();
  }

  status = sl_si91x_usart_receive_data(uart_handle, uart_data_in, sizeof(uart_data_in));
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_usart_receive_data: Error Code : %lu\r\n", status);
    osThreadExit();
  }

  flags = osEventFlagsWait(uart_event_flags, UART_IO_COMPLETE_FLAGS, osFlagsWaitAll, osWaitForever);
  if ((flags & osFlagsError) != 0U) {
    SL_PRINT_STRING_ERROR("UART send/receive event wait failed\r\n");
    osThreadExit();
  }
  SL_PRINT_STRING_ERROR("UART send and receive completed\r\n");

  if (uart_app_compare_data() != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("UART data comparison failed\r\n");
  }

  osThreadExit();
}

/*******************************************************************************
 * @brief  UART event callback — invoked from the USART driver interrupt path.
 *         Sets event flags; CMSIS-RTOS2 uses ISR-safe primitives when IS_IRQ().
 * @param  event  UART event (send complete, receive complete, etc.)
 ******************************************************************************/
static void uart_event_callback_handler(uint32_t event)
{
  if (uart_event_flags == NULL) {
    return;
  }
  switch (event) {
    case SL_USART_EVENT_SEND_COMPLETE:
      osEventFlagsSet(uart_event_flags, UART_SEND_COMPLETE_FLAG);
      break;
    case SL_USART_EVENT_RECEIVE_COMPLETE:
      osEventFlagsSet(uart_event_flags, UART_RECEIVE_COMPLETE_FLAG);
      break;
    default:
      break;
  }
}

/*******************************************************************************
 * @brief  Compares the TX and RX buffers and prints the loopback test result.
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on mismatch.
 ******************************************************************************/
static sl_status_t uart_app_compare_data(void)
{
  uint16_t data_index = 0;
  for (data_index = 0; data_index < UART_BUFFER_SIZE; data_index++) {
    if (uart_data_in[data_index] != uart_data_out[data_index]) {
      return SL_STATUS_FAIL;
    }
  }
  SL_PRINT_STRING_ERROR("Data comparison successful, Loop Back Test Passed\r\n");
  return SL_STATUS_OK;
}
