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
#ifndef RSI_M4_INTERFACE
#include "sl_uart.h"
#endif
#include "sl_board_configuration.h"
#include "cmsis_os2.h"
#include "sl_utility.h"
#include <stdbool.h>

/******************************************************
 *                    Constants
 ******************************************************/

#define UART_BUFFER_SIZE 128

#define MY_ARG_TYPE uart, spi, i2c, tcp

#define OFFSETS(a, b, c, d) uint8_t offsets[] = { 0, sizeof(#a), sizeof(#b), sizeof(#c) }

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

sl_status_t help_command_handler(console_args_t *arguments);
sl_status_t rtt_command_handler(console_args_t *arguments);

static void print_command_args(const console_descriptive_command_t *command);

extern sl_status_t process_buffer_line(const console_database_t *command_database,
                                       console_args_t *args,
                                       const console_descriptive_command_t **command);

void application_start(const void *unused);
void print_status(sl_status_t status, uint32_t duration);
extern sl_status_t sl_board_enable_vcom(void);

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
  .stack_size = 3072,
  .priority   = 0,
  .tz_module  = 0,
  .reserved   = 0,
};

void app_init(const void *unused)
{

  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

void application_start(const void *unused)
{
  UNUSED_PARAMETER(unused);
  console_args_t args;
  const console_descriptive_command_t *command;

  SL_DEBUG_LOG("app start\n");

  printf("Ready\r\n");

  console_line_ready = 0;

  while (1) {
    printf("\r\n> \r\n");
    while (!console_line_ready) {
      console_process_uart_data();
      osDelay(20);
    }

    sl_status_t result = process_buffer_line(&console_command_database, &args, &command);

    if (result == SL_STATUS_OK) {
      SL_DEBUG_LOG("Processing command\n");
      if (command->handler) {
        printf("\r\n");
        uint32_t start_time = osKernelGetTickCount();
        result              = command->handler(&args);
        uint32_t duration   = osKernelGetTickCount() - start_time;
        print_status(result, duration);
      }
    } else if (result == SL_STATUS_COMMAND_IS_INVALID) {
      printf("\r\nArgs: ");
      print_command_args(command);
      print_status(SL_STATUS_INVALID_PARAMETER, 0);
    } else {
      printf("\r\nNot supported\r\n");
    }
    console_line_ready = 0;
  }
}

void print_status(sl_status_t status, uint32_t duration)
{
  printf("\r\n0x%05lX: (%lums) %s\r\n", status, duration, (status == SL_STATUS_OK) ? "Success" : "");
}

sl_status_t help_command_handler(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  for (uint8_t a = 0; a < console_command_database.length; ++a) {
    printf("\r\n");
    printf("%s", console_command_database.entries[a].key);
    printf("  ");
    print_command_args((console_descriptive_command_t *)console_command_database.entries[a].value);
    printf("\r\n   ");
    printf(((console_descriptive_command_t *)console_command_database.entries[a].value)->description);
  }
  return SL_STATUS_OK;
}

typedef struct {
  void *next;
  uint8_t stack[];
} console_threaded_command_t;

static console_threaded_command_t *threaded_commands_list_head;
sl_status_t thread_command_handler(console_args_t *arguments)
{
  console_args_t args;
  const console_descriptive_command_t *command;
  uint32_t stack_size = (uint32_t)arguments->arg[0];
  sl_status_t result  = console_parse_command((char *)arguments->arg[1], &console_command_database, &args, &command);

  if (result == SL_STATUS_OK) {
    if (command->handler) {
      // Create thread
      console_threaded_command_t *new_thread = malloc(sizeof(console_threaded_command_t) + stack_size);
      if (new_thread == NULL) {
        return SL_STATUS_ALLOCATION_FAILED;
      }
      new_thread->next = 0;
      if (threaded_commands_list_head == NULL) {
        threaded_commands_list_head = new_thread;
      } else {
        new_thread->next            = threaded_commands_list_head;
        threaded_commands_list_head = new_thread->next;
      }
      uint32_t start_time = osKernelGetTickCount();
      result              = command->handler(&args);
      uint32_t duration   = osKernelGetTickCount() - start_time;
      print_status(result, duration);
    }
  }
  return result;
}

sl_status_t rtt_command_handler(console_args_t *arguments)
{
  return SL_STATUS_OK;
}

extern const arg_list_t console_argument_types[];
static void print_command_args(const console_descriptive_command_t *command)
{
  bool is_optional = false;
  for (int a = 0; command->argument_list[a] != CONSOLE_ARG_END; ++a) {
    if (command->argument_list[a] & CONSOLE_ARG_OPTIONAL) {
      char option_char[2] = { (char)command->argument_list[a] & CONSOLE_ARG_OPTIONAL_CHARACTER_MASK, 0 };
      printf("[-");
      printf("%s", option_char);
      printf(" ");
      is_optional = true;
      continue;
    } else if (command->argument_list[a] & CONSOLE_ARG_ENUM) {
      printf("{");
      uint8_t enum_index = command->argument_list[a] & CONSOLE_ARG_ENUM_INDEX_MASK;
      for (int b = 0; console_argument_types[enum_index][b] != NULL;
           /* Increment occurs in internal logic */) {
        printf("%s", console_argument_types[enum_index][b]);
        if (console_argument_types[enum_index][++b]) {
          printf("|");
        }
      }
      printf("}");
    } else {
      printf("<");
      if (command->argument_help && command->argument_help[a]) {
        printf("%s", command->argument_help[a]);
      } else {
        printf(console_argument_type_strings[command->argument_list[a] & CONSOLE_ARG_ENUM_INDEX_MASK]);
      }
      printf(">");
    }
    if (is_optional) {
      printf("] ");
      is_optional = false;
    } else {
      printf(" ");
    }
  }
}
