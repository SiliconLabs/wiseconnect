#pragma once

#include "cmsis_os2.h"
#include "console_types.h"
#include <stdint.h>

#ifndef MAX_THREADED_COMMANDS
#define MAX_THREADED_COMMANDS 8
#endif

#ifndef MAX_THREADED_COMMAND_QUEUE_COUNT
#define MAX_THREADED_COMMAND_QUEUE_COUNT 3
#endif

//#ifndef WIFI_THREAD_INDEX
//#define WIFI_THREAD_INDEX 0
//#endif
//
//#ifndef BLE_THREAD_INDEX
//#define BLE_THREAD_INDEX 1
//#endif

#ifndef GLOBAL_PRINT_BUFFER_SIZE
#define GLOBAL_PRINT_BUFFER_SIZE 256
#endif

typedef struct {
  osThreadId_t thread_id;
  char *print_buffer;
  char host_id;
  uint8_t index;
  uint16_t print_buffer_size;
  uint16_t print_start;
  uint16_t print_end;
  uint16_t stack_size;
  console_args_t args;
  osMessageQueueId_t queue;
} console_threaded_command_t;

extern void process_threaded_commands(void);
sl_status_t push_threaded_task(uint32_t index, char *args);
sl_status_t start_command_thread(uint32_t index, uint32_t uart_buffer_size, uint32_t stack_size, uint32_t priority);

extern console_threaded_command_t threaded_commands[MAX_THREADED_COMMANDS];

extern char global_print_buffer[GLOBAL_PRINT_BUFFER_SIZE];
extern uint16_t global_print_start;
extern uint16_t global_print_end;
