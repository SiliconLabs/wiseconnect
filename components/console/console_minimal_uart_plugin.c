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
#include "sl_uart.h"
#include "sl_board_configuration.h"
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#ifndef USER_RX_BUFFER_SIZE
#define USER_RX_BUFFER_SIZE (256)
#endif

#ifndef USER_RX_BUFFER_COUNT
#define USER_RX_BUFFER_COUNT (4)
#endif

#define INVALID_INDEX (0xFF)

// Special characters
#define BACKSPACE_CHARACTER           '\b'
#define DELETE_CHARACTER              (127)
#define LINEFEED_CHARACTER            (10)
#define CARRIAGE_RETURN_CHARACTER     (13)
#define ESCAPE_CHARACTER              (27)
#define SPACE_CHARACTER               (32)
#define TAB_CHARACTER                 (9)
#define OPEN_SQUARE_BRACKET_CHARACTER (91)

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

static void print_command_database(const console_database_t *database, const char *prefix);
static void post_uart_rx_handler(char character);

/******************************************************
 *               Variable Definitions
 ******************************************************/
volatile uint8_t console_line_ready;

static uint8_t user_rx_buffer[USER_RX_BUFFER_COUNT][USER_RX_BUFFER_SIZE];
static uint16_t user_rx_buffer_write_pointer = 0;
static uint8_t current_buffer_index          = 0;
static uint8_t buffer_ready_index            = INVALID_INDEX;
static uint8_t uart_rx_cache[USER_RX_BUFFER_SIZE];
static uint16_t uart_rx_write_iter = 0;
static uint16_t uart_rx_read_iter  = 0;

/******************************************************
 *               Function Definitions
 ******************************************************/

sl_status_t console_process_buffer(const console_database_t *command_database,
                                   console_args_t *args,
                                   const console_descriptive_command_t **command)
{
  sl_status_t result = SL_STATUS_FAIL;

  if (buffer_ready_index != INVALID_INDEX) {
    result = console_parse_command((char *)user_rx_buffer[buffer_ready_index], command_database, args, command);
    buffer_ready_index = INVALID_INDEX;
  }

  return result;
}

void cache_uart_rx_data(const char character)
{
  uart_rx_cache[uart_rx_write_iter] = character;
  uart_rx_write_iter                = (uart_rx_write_iter + 1) % sizeof(uart_rx_cache);
}

void console_process_uart_data(void)
{
  while (uart_rx_read_iter != uart_rx_write_iter) {
    post_uart_rx_handler(uart_rx_cache[uart_rx_read_iter]);
    uart_rx_read_iter = (uart_rx_read_iter + 1) % sizeof(uart_rx_cache);
  }
}

static void post_uart_rx_handler(char character)
{
  static uint8_t escape_sequence = 0;

  // Support for CRLF
  static uint8_t last_char_cr = false;

  if (escape_sequence == 0) {
    switch (character) {
      case DELETE_CHARACTER:
      case BACKSPACE_CHARACTER:
        last_char_cr = false;
        if (user_rx_buffer_write_pointer > 0) {
          user_rx_buffer_write_pointer--;
          printf("%c", character);
        }
        break;

      case TAB_CHARACTER:
        // Do tab
        break;

      case LINEFEED_CHARACTER:
        if (last_char_cr != false) {
          last_char_cr = false;
          break;
        }
        __attribute__((fallthrough));
      case CARRIAGE_RETURN_CHARACTER:
        if (character == '\r') {
          last_char_cr = true;
        }
        //        user_rx_buffer[current_buffer_index][user_rx_buffer_write_pointer] = 0;
        memset(&user_rx_buffer[current_buffer_index][user_rx_buffer_write_pointer],
               0,
               USER_RX_BUFFER_SIZE - user_rx_buffer_write_pointer);
        buffer_ready_index           = current_buffer_index;
        user_rx_buffer_write_pointer = 0;
        current_buffer_index++;

        if (current_buffer_index >= USER_RX_BUFFER_COUNT) {
          current_buffer_index = 0;
        }
        console_line_ready = 1;
        break;

      case ESCAPE_CHARACTER:
        last_char_cr    = false;
        escape_sequence = 1;
        break;

      default:
        last_char_cr                                                       = false;
        user_rx_buffer[current_buffer_index][user_rx_buffer_write_pointer] = character;
        user_rx_buffer_write_pointer++;
        printf("%c", character);
        break;
    }
  } else {
    if (escape_sequence == 1) {
      if (character == OPEN_SQUARE_BRACKET_CHARACTER) {
        escape_sequence = 2;
      } else {
        escape_sequence = 0;
      }
    } else {
      switch (character) {
        case 'A':
          // Up arrow pressed
          current_buffer_index--;

          if (current_buffer_index >= USER_RX_BUFFER_COUNT) {
            current_buffer_index = USER_RX_BUFFER_COUNT - 1;
          }

          printf("%s", (const char *)&user_rx_buffer[current_buffer_index][0]);
          break;
      }

      escape_sequence = 0;
    }
  }
}

extern const arg_list_t console_argument_types[];
void console_print_command_args(const console_descriptive_command_t *command)
{
  bool is_optional         = false;
  int argument_help_offset = 0;
  for (int a = 0; command->argument_list[a] != CONSOLE_ARG_END; ++a) {
    if (command->argument_list[a] & CONSOLE_ARG_OPTIONAL) {
      char option_char[2] = { (char)command->argument_list[a] & CONSOLE_ARG_OPTIONAL_CHARACTER_MASK, 0 };
      sl_uart_print(DEFAULT_UART, "[-");
      sl_uart_print(DEFAULT_UART, option_char);
      sl_uart_print(DEFAULT_UART, " ");
      ++argument_help_offset;
      is_optional = true;
      continue;
    } else if (command->argument_list[a] & CONSOLE_ARG_ENUM) {
      sl_uart_print(DEFAULT_UART, "{");
      uint8_t enum_index = command->argument_list[a] & CONSOLE_ARG_ENUM_INDEX_MASK;
      for (int b = 0; console_argument_types[enum_index][b] != NULL;) {
        sl_uart_print(DEFAULT_UART, console_argument_types[enum_index][b]);
        if (console_argument_types[enum_index][++b]) {
          sl_uart_print(DEFAULT_UART, "|");
        }
      }
      sl_uart_print(DEFAULT_UART, "}");
    } else {
      sl_uart_print(DEFAULT_UART, "<");
      if (command->argument_help && command->argument_help[a - argument_help_offset]) {
        sl_uart_print(DEFAULT_UART, command->argument_help[a - argument_help_offset]);
      } else {
        sl_uart_print(DEFAULT_UART,
                      console_argument_type_strings[command->argument_list[a] & CONSOLE_ARG_ENUM_INDEX_MASK]);
      }
      sl_uart_print(DEFAULT_UART, ">");
    }
    if (is_optional) {
      sl_uart_print(DEFAULT_UART, "] ");
      is_optional = false;
    } else {
      sl_uart_print(DEFAULT_UART, " ");
    }
  }
}

sl_status_t default_help_command_handler(console_args_t *arguments)
{
  const console_database_entry_t *entry;
  uint32_t starting_index = 0;
  char *group             = GET_OPTIONAL_COMMAND_ARG(arguments, 0, NULL, char *);
  char *group_end         = group + strlen(group);

  if (group != NULL && group_end != NULL) {
    sl_status_t status = console_find_command(&group, group_end, &console_command_database, &entry, &starting_index);
    if (status == SL_STATUS_OK) {
      const console_descriptive_command_t *command = entry->value;
      if (command->argument_list[0] == CONSOLE_ARG_SUB_COMMAND) {
        print_command_database((const console_database_t *)command->sub_command_database, group);
      }
      return status;
    }
  }

  print_command_database(&console_command_database, NULL);

  return SL_STATUS_OK;
}

static void print_command_database(const console_database_t *database, const char *prefix)
{
  for (uint8_t a = 0; a < database->length; ++a) {
    const console_descriptive_command_t *command = database->entries[a].value;

    sl_uart_print(DEFAULT_UART, "\r\n");
    if (prefix != NULL) {
      sl_uart_print(DEFAULT_UART, prefix);
      sl_uart_print(DEFAULT_UART, ".");
    }
    sl_uart_print(DEFAULT_UART, database->entries[a].key);

    // Check if this is a sub-command
    if (command->argument_list[0] == CONSOLE_ARG_SUB_COMMAND) {
      const console_database_t *temp = (const console_database_t *)command->sub_command_database;
      sl_uart_print(DEFAULT_UART, ".");
      if (command->description) {
        sl_uart_print(DEFAULT_UART, " : ");
        sl_uart_print(DEFAULT_UART, command->description);
      }
      sl_uart_print(DEFAULT_UART, "\r\n");
      for (int b = 0; b < temp->length; b++) {
        sl_uart_print(DEFAULT_UART, "  - ");
        sl_uart_print(DEFAULT_UART, temp->entries[b].key);
        sl_uart_print(DEFAULT_UART, " : ");
        const console_descriptive_command_t *sub_command = temp->entries[b].value;
        sl_uart_print(DEFAULT_UART, sub_command->description);
        sl_uart_print(DEFAULT_UART, "\r\n");
      }
    } else {
      sl_uart_print(DEFAULT_UART, "  ");
      console_print_command_args((console_descriptive_command_t *)database->entries[a].value);
      sl_uart_print(DEFAULT_UART, "\r\n   ");
      sl_uart_print(DEFAULT_UART, ((console_descriptive_command_t *)database->entries[a].value)->description);
    }
    sl_uart_print(DEFAULT_UART, "\r\n");
  }
}

char *console_get_command_buffer(void)
{
  const int index = (current_buffer_index - 1) % USER_RX_BUFFER_COUNT;
  return (char *)user_rx_buffer[index];
}
