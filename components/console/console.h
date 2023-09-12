#pragma once

#include "console_types.h"
#include "sl_status.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                 Global Variables
 ******************************************************/

// command_database content is generated in cli_command_database.c
extern const console_database_t console_command_database;

extern const char *const console_argument_type_strings[];

extern volatile uint8_t console_line_ready;

/******************************************************
 *               Function Declarations
 ******************************************************/

sl_status_t command_proccessor_init();
sl_status_t console_parse_command(char *command_line,
                                  const console_database_t *db,
                                  console_args_t *args,
                                  const console_descriptive_command_t **output_command);
sl_status_t console_find_command(char **string,
                                 char *string_end,
                                 const console_database_t *db,
                                 const console_database_entry_t **entry,
                                 uint32_t *starting_index);

sl_status_t console_parse_arg(console_argument_type_t type, char *line, uint32_t *arg_result);
void console_add_to_history(const char *line, uint8_t line_length);
void console_process_uart_data(void);
sl_status_t console_process_buffer(const console_database_t *command_database,
                                   console_args_t *args,
                                   const console_descriptive_command_t **command);
void console_print_command_args(const console_descriptive_command_t *command);

char *console_get_command_buffer(void);

#ifdef __cplusplus
} /*extern "C" */
#endif
