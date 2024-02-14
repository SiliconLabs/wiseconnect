/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

/** @file
 *
 * File Description
 *
 */
#include "console.h"
#include "sl_constants.h"
#include "sl_status.h"
//#include "sl_uart.h"
#include "sl_board_configuration.h"
#include "cmsis_os2.h"
#include "SEGGER_RTT.h"
#include "sl_utility.h"
#include "printf.h"
#include "test_agent.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdbool.h>

/******************************************************
 *                    Constants
 ******************************************************/

#define WIFI_COMMAND_THREAD_STACK_SIZE 2048
#define BLE_COMMAND_THREAD_STACK_SIZE  2048
#define MAIN_THREAD_STACK_SIZE         3584

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/

void application_start(const void *unused);
void print_status(sl_status_t status, uint32_t duration);

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

const osThreadAttr_t thread_attributes = {
  .name       = "application_thread",
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = MAIN_THREAD_STACK_SIZE,
  .priority   = osPriorityLow,
  .tz_module  = 0,
  .reserved   = 0,
};

osThreadId_t application_thread_id;

void app_init(const void *unused)
{
  UNUSED_PARAMETER(unused);
  application_thread_id = osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

void application_start(const void *unused)
{
  UNUSED_PARAMETER(unused);
  console_args_t args;
  const console_descriptive_command_t *command;

  SL_DEBUG_LOG("app start\n");

  //  sl_uart_init(DEFAULT_UART, 115200, DEFAULT_UART_PIN_CONFIG);
  printf("Ready\r\n");

  console_line_ready = 0;

  while (1) {
    printf("\r\n> ");
    while (!console_line_ready) {
      console_process_uart_data();
      process_threaded_commands();
    }
    sl_status_t result = console_process_buffer(&console_command_database, &args, &command);

    if (result == SL_STATUS_OK) {
      SL_DEBUG_LOG("Processing command\n");
      printf("\r\n");
      uint32_t start_time = osKernelGetTickCount();
      result              = command->handler(&args);
      uint32_t duration   = osKernelGetTickCount() - start_time;
      // Check for the special result that indicates the command has printed it's own status
      if (result != 0xFFFFFFFF) {
        print_status(result, duration);
      }
    } else if (result == SL_STATUS_COMMAND_IS_INVALID) {
      printf("\r\nArgs: ");
      console_print_command_args(command);
      print_status(SL_STATUS_INVALID_PARAMETER, 0);
    } else {
      printf("\r\nNot supported\r\n");
    }
    console_line_ready = 0;
  }
}

void print_status(sl_status_t status, uint32_t duration)
{
  printf("\n0x%05lX: (%lums) %s", status, duration, (status == SL_STATUS_OK) ? "" : "");
}

void sl_debug_log(const char *format, ...)
{
  va_list param_list;
  va_start(param_list, format);
  (void)SEGGER_RTT_vprintf(0, format, &param_list);
  va_end(param_list);
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
  UNUSED_PARAMETER(xTask);
  UNUSED_PARAMETER(pcTaskName);
  __asm("bkpt");
}

sl_status_t host_reset_command(console_args_t *arguments)
{
  (void)arguments;
  printf("Resetting MCU...\r\n");
  NVIC_SystemReset();
  return SL_STATUS_OK;
}
