#include "console.h"
#include "threaded_commands.h"
#include "printf.h"
#include "malloc.h"
#include "sl_constants.h"
#include <stdlib.h>
#include <string.h>

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/

extern void print_status(sl_status_t status, uint32_t duration);
void print_buffered_data(char prefix, unsigned int length, const char *data);

/******************************************************
 *               Variable Definitions
 ******************************************************/

console_threaded_command_t threaded_commands[MAX_THREADED_COMMANDS] = { 0 };
char global_print_buffer[GLOBAL_PRINT_BUFFER_SIZE]                  = { 0 };
uint16_t global_print_start                                         = 0;
uint16_t global_print_end                                           = 0;

/******************************************************
 *               Function Definitions
 ******************************************************/

static void thread_command_thread(void *args)
{
  const console_descriptive_command_t *command;
  console_threaded_command_t *thread = (console_threaded_command_t *)args;

  while (1) {
    console_args_t arguments;
    char *command_line = NULL;
    char *message_data = NULL;
    osStatus_t status  = osMessageQueueGet(thread->queue, &message_data, 0, 0xFFFFFFFF);
    if (status != osOK) {
      continue;
    }

    command_line = message_data;

    // Extract host ID from command line
    if ((*command_line >= '0' && *command_line <= '9') || (*command_line >= 'A' && *command_line <= 'Z')) {
      thread->host_id = *command_line;
      while (*(++command_line) != ' ') {
      }
      *command_line++ = 0;
    } else {
      thread->host_id = '0' + thread->index;
    }

    sl_status_t result  = console_parse_command(command_line, &console_command_database, &arguments, &command);
    uint32_t start_time = osKernelGetTickCount();
    if (result == SL_STATUS_OK) {
      result = command->handler(&arguments);
    }
    uint32_t duration = osKernelGetTickCount() - start_time;
    print_status(result, duration);
    free(message_data);
  }
  osThreadExit();
}

sl_status_t start_command_thread(uint32_t index, uint32_t uart_buffer_size, uint32_t stack_size, uint32_t priority)
{
  if (threaded_commands[index].thread_id != 0) {
    return SL_STATUS_OK;
  }
  console_threaded_command_t *new_thread = &threaded_commands[index];
  new_thread->print_start                = 0;
  new_thread->print_end                  = 0;
  new_thread->print_buffer_size          = uart_buffer_size;
  new_thread->print_buffer               = malloc(uart_buffer_size);
  new_thread->index                      = index;
  new_thread->stack_size                 = stack_size;
  if (new_thread->print_buffer == NULL) {
    return SL_STATUS_ALLOCATION_FAILED;
  }
  memset(new_thread->print_buffer, 0, uart_buffer_size);

  const osMessageQueueAttr_t queue_attributes = {
    .name      = "",
    .attr_bits = 0,
    .cb_mem    = NULL,
    .cb_size   = 0,
    .mq_mem    = 0,
    .mq_size   = 0,
  };
  new_thread->queue = osMessageQueueNew(MAX_THREADED_COMMAND_QUEUE_COUNT, sizeof(uint32_t), &queue_attributes);
  if (new_thread->queue == 0) {
    free(new_thread->print_buffer);
    return SL_STATUS_ALLOCATION_FAILED;
  }

  const osThreadAttr_t thread_attributes = {
    .name       = "",
    .attr_bits  = 0,
    .cb_mem     = 0,
    .cb_size    = 0,
    .stack_mem  = 0,
    .stack_size = stack_size,
    .priority   = priority,
    .tz_module  = 0,
    .reserved   = 0,
  };

  new_thread->thread_id = osThreadNew((osThreadFunc_t)thread_command_thread, new_thread, &thread_attributes);
  if (new_thread->thread_id == 0) {
    free(new_thread->print_buffer);
    return SL_STATUS_ALLOCATION_FAILED;
  }
  return SL_STATUS_OK;
}

sl_status_t push_threaded_task(uint32_t index, char *command)
{
  sl_status_t status    = SL_STATUS_OK;
  size_t command_length = strlen(command);
  char *command_buffer  = malloc(command_length + 1); // +1 for NULL character
  if (command_buffer == NULL) {
    return SL_STATUS_ALLOCATION_FAILED;
  }
  memcpy(command_buffer, command, command_length + 1);
  osThreadSuspend(threaded_commands[index].thread_id);
  osStatus_t os_status = osMessageQueuePut(threaded_commands[index].queue, &command_buffer, 0, 0);
  if (os_status != osOK) {
    status = SL_STATUS_FAIL;
  }
  print_status(status, 1);
  osThreadResume(threaded_commands[index].thread_id);
  return 0xFFFFFFFF;
}

sl_status_t thread_command_handler(console_args_t *arguments)
{
  const uint32_t index = (uint32_t)arguments->arg[0];
  return push_threaded_task(index, (char *)arguments->arg[1]);
}

sl_status_t start_thread_command_handler(console_args_t *arguments)
{
  const uint8_t index             = (uint8_t)arguments->arg[0];
  const uint32_t stack_size       = (uint32_t)arguments->arg[1];
  const uint32_t uart_buffer_size = (uint32_t)arguments->arg[2];
  const uint32_t priority         = GET_OPTIONAL_COMMAND_ARG(arguments, 3, osPriorityNormal, uint32_t);

  return start_command_thread(index, uart_buffer_size, stack_size, priority);
}

sl_status_t thread_terminate_command_handler(console_args_t *arguments)
{
  char *message;
  const uint32_t index = (uint32_t)arguments->arg[0];

  osThreadTerminate(threaded_commands[index].thread_id);

  while (osMessageQueueGet(threaded_commands[index].queue, &message, 0, 0) == osOK) {
    free(message);
  }

  osMessageQueueDelete(threaded_commands[index].queue);
  free(threaded_commands[index].print_buffer);

  memset(&threaded_commands[index], 0, sizeof(threaded_commands[index]));

  return SL_STATUS_OK;
}

sl_status_t thread_info_command_handler(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  for (int a = 0; a < MAX_THREADED_COMMANDS; ++a) {
    if (threaded_commands[a].thread_id != NULL) {
      printf("%d-%d-%d\r\n", a, threaded_commands[a].stack_size, threaded_commands[a].print_buffer_size);
    }
  }
  return SL_STATUS_OK;
}

sl_status_t mallinfo_command_handler(console_args_t *arguments)
{
  (void)(arguments);
  struct mallinfo info = mallinfo();
  printf("arena: %u\r\n", info.arena);
  printf("ordblks: %u\r\n", info.ordblks);
  printf("smblks: %u\r\n", info.smblks);
  printf("hblks: %u\r\n", info.hblks);
  printf("hblkhd: %u\r\n", info.hblkhd);
  printf("used: %u\r\n", info.uordblks);
  printf("not used: %u\r\n", info.fordblks);
  printf("keepcost: %u\r\n", info.keepcost);
  return SL_STATUS_OK;
}

void process_threaded_commands(void)
{
  // Take snapshot of global start and end values
  uint16_t start = global_print_start;
  uint16_t end   = global_print_end;

  if (end != start) {
    print_buffered_data('F', end - start, &global_print_buffer[start]);
    global_print_start = end;
  }
  for (int a = 0; a < MAX_THREADED_COMMANDS; ++a) {
    osThreadId_t id = threaded_commands[a].thread_id;
    if (id != 0) {
      // Take snapshot of start and end values
      start = threaded_commands[a].print_start;
      end   = threaded_commands[a].print_end;
      if (end != start) {
        const unsigned int length = end - start;
        print_buffered_data(threaded_commands[a].host_id, length, &threaded_commands[a].print_buffer[start]);
        threaded_commands[a].print_start = end;
      }
    }
  }
}

void print_buffered_data(char prefix, unsigned int length, const char *data)
{
  printf("%c%.4X%.*s\r\n", prefix, length, length, data);
}

sl_status_t mem_alloc_command_handler(console_args_t *arguments)
{
  uint32_t size = (uint32_t)GET_COMMAND_ARG(arguments, 0);
  void *buffer  = malloc(size);
  if (buffer != NULL) {
    memset(buffer, '\0', size);
    printf("0x%lX\r\n", (uint32_t)buffer);
    return SL_STATUS_OK;
  } else {
    return SL_STATUS_ALLOCATION_FAILED;
  }
}

sl_status_t mem_free_command_handler(console_args_t *arguments)
{
  void *address = (void *)GET_COMMAND_ARG(arguments, 0);
  free(address);
  return SL_STATUS_OK;
}

sl_status_t mem_write_command_handler(console_args_t *arguments)
{
  void *address       = (void *)GET_COMMAND_ARG(arguments, 0);
  uint32_t size       = (uint32_t)GET_COMMAND_ARG(arguments, 1);
  uint32_t start_time = osKernelGetTickCount();
  uint32_t duration   = osKernelGetTickCount() - start_time;
  print_status(SL_STATUS_OK, duration);
  while (size > 0) {
    uint32_t amount_read = console_read_data_from_cache(address, size);
    size -= amount_read;
    address += amount_read;
    if (size > 0) {
      osDelay(10);
    }
  }
  return SL_STATUS_OK;
}
