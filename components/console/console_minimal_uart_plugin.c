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
#ifndef RSI_M4_INTERFACE
#include "sl_uart.h"
#endif
//#include "sl_thread.h"
#include "sl_board_configuration.h"
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#ifdef RSI_M4_INTERFACE
extern int stdin_getchar(void);
#endif
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

static void post_uart_rx_handler(char character);

/******************************************************
 *               Variable Definitions
 ******************************************************/
#ifdef RSI_M4_INTERFACE
char tempchar, prevchar;
#endif
volatile uint8_t console_line_ready;

static uint8_t user_rx_buffer[USER_RX_BUFFER_COUNT][USER_RX_BUFFER_SIZE];
static uint16_t user_rx_buffer_write_pointer = 0;
static uint8_t current_buffer_index          = 0;
static uint8_t buffer_ready_index            = INVALID_INDEX;
static uint8_t uart_rx_cache[128];
static uint16_t uart_rx_write_iter = 0;
static uint16_t uart_rx_read_iter  = 0;

/******************************************************
 *               Function Definitions
 ******************************************************/

sl_status_t process_buffer_line(const console_database_t *command_database,
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
#ifdef RSI_M4_INTERFACE
  tempchar = 0;
  prevchar = 0;
  while ((tempchar != '\n') && (prevchar != '\r')) {
    prevchar = tempchar;
    tempchar = stdin_getchar();
    cache_uart_rx_data(tempchar);
  }
#endif
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
        user_rx_buffer[current_buffer_index][user_rx_buffer_write_pointer] = 0;
        buffer_ready_index                                                 = current_buffer_index;
        user_rx_buffer_write_pointer                                       = 0;
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
