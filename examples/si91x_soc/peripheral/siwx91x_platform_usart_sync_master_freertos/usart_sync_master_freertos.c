/***************************************************************************/ /**
 * @file usart_sync_master_freertos.c
 * @brief USART Synchronous Master FreeRTOS example
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

#include "usart_sync_master_freertos.h"
#include "cmsis_os2.h"
#include "rsi_debug.h"
#include "sl_si91x_usart.h"
#include "sl_status.h"
#include <stdbool.h>
#include <string.h>

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define USART_BUFFER_SIZE     1024   // Data send and receive length
#define USART_BAUDRATE        115200 // Baud rate <9600-7372800>
#define NON_UC_DEFAULT_CONFIG 0    // Enable this macro to set default config in non-UC case (useful when not using UC)
#define MASTER_STARTUP_DELAY  5000 // ms — wait for slave to be ready before first transfer

/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
static uint8_t usart_data_in[USART_BUFFER_SIZE];  // RX buffer
static uint8_t usart_data_out[USART_BUFFER_SIZE]; // TX buffer
static sl_usart_handle_t usart_handle;            // USART driver handle

/* Released from driver ISR on SL_USART_EVENT_TRANSFER_COMPLETE (ISR-safe give on this port). */
static osSemaphoreId_t usart_transfer_sem;

/* FreeRTOS task attributes for the USART sync master task */
static const osThreadAttr_t usart_sync_thread_attributes = {
  .name       = "usart_sync_master_task",
  .stack_size = 2048,
  .priority   = osPriorityLow1,
};

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static sl_status_t usart_sync_init_function(void);
static void usart_sync_task(void *argument);
static void usart_callback_event(uint32_t event);
static sl_status_t usart_app_compare_data(void);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
/*******************************************************************************
 * @brief  Entry point: creates the USART sync master FreeRTOS task.
 * @return None
 ******************************************************************************/
void usart_sync_example_init(void)
{
  osThreadId_t thread_id = osThreadNew((osThreadFunc_t)usart_sync_task, NULL, &usart_sync_thread_attributes);
  if (thread_id == NULL) {
    /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */
    SL_PRINT_STRING_ERROR("Failed to create USART sync master task\r\n");
  }
}

/*******************************************************************************
 * @brief  Initializes the USART peripheral in synchronous master mode.
 * @return sl_status_t SL_STATUS_OK on success, or an error code.
 ******************************************************************************/
static sl_status_t usart_sync_init_function(void)
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

  /* Initialize the USART peripheral */
  status = sl_si91x_usart_init(USART_0, &usart_handle);
  DEBUGINIT();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_usart_init: Error Code : %lu\r\n", status);
    return status;
  }
  SL_PRINT_STRING_ERROR("USART MASTER initialization is successful\r\n");

  /* Apply USART configuration (baud rate, data bits, parity, etc.) */
  status = sl_si91x_usart_set_configuration(usart_handle, &usart_config);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_usart_set_configuration: Error Code : %lu\r\n", status);
    return status;
  }

  /* Register event callback; the callback releases the transfer semaphore */
  status = sl_si91x_usart_multiple_instance_register_event_callback(USART_0, usart_callback_event);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_usart_multiple_instance_register_event_"
                          "callback: Error Code : %lu\r\n",
                          status);
    return status;
  }

  return SL_STATUS_OK;
}

/*******************************************************************************
 * @brief  FreeRTOS task (single shot): one sync transfer, compare, exit.
 * @param  argument  Unused task argument.
 ******************************************************************************/
static void usart_sync_task(void *argument)
{
  (void)argument;
  sl_status_t status = 0;

  /* Initialize USART peripheral in sync master mode */
  status = usart_sync_init_function();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("USART sync master init failed\r\n");
    osThreadExit();
  }

  /* Create semaphore before starting transfer; callback will release it */
  usart_transfer_sem = osSemaphoreNew(1U, 0U, NULL);
  if (usart_transfer_sem == NULL) {
    SL_PRINT_STRING_ERROR("Failed to create USART transfer semaphore\r\n");
    osThreadExit();
  }

  /* Fill TX buffer with master's test pattern */
  for (uint16_t i = 0; i < USART_BUFFER_SIZE; i++) {
    usart_data_out[i] = (uint8_t)(i + 1);
  }

  osDelay(MASTER_STARTUP_DELAY);

  memset(usart_data_in, 0, sizeof(usart_data_in));

  status = sl_si91x_usart_transfer_data(usart_handle, usart_data_out, usart_data_in, sizeof(usart_data_out));
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_usart_transfer_data: Error Code : %lu\r\n", status);
    osThreadExit();
  }
  if (osSemaphoreAcquire(usart_transfer_sem, osWaitForever) != osOK) {
    osThreadExit();
  }
  SL_PRINT_STRING_ERROR("USART transfer completed\r\n");

  if (usart_app_compare_data() != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Data comparison failed, Loop Back Test Failed\r\n");
  }

  osThreadExit();
}

/*******************************************************************************
 * @brief  USART event callback — invoked from the driver interrupt path.
 *         Releases semaphore; CMSIS-RTOS2 uses ISR-safe give when IS_IRQ().
 * @param  event  USART event (SL_USART_EVENT_TRANSFER_COMPLETE, etc.)
 ******************************************************************************/
static void usart_callback_event(uint32_t event)
{
  if (event == SL_USART_EVENT_TRANSFER_COMPLETE) {
    osSemaphoreRelease(usart_transfer_sem);
  }
}

/*******************************************************************************
 * @brief  Validates RX buffer against the slave's expected TX pattern.
 *         Slave sends (USART_BUFFER_SIZE - i), which differs from master's
 *         (i + 1) pattern — this prevents false positives when no slave is
 *         connected and the master reads back its own TX data.
 * @return sl_status_t SL_STATUS_OK if all bytes match, SL_STATUS_FAIL otherwise.
 ******************************************************************************/
static sl_status_t usart_app_compare_data(void)
{
  for (uint16_t i = 0; i < USART_BUFFER_SIZE; i++) {
    if (usart_data_in[i] != (uint8_t)(USART_BUFFER_SIZE - i)) {
      return SL_STATUS_FAIL;
    }
  }
  SL_PRINT_STRING_ERROR("Data comparison successful, Loop Back Test Passed\r\n");
  return SL_STATUS_OK;
}
