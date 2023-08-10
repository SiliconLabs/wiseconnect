#include "console.h"
#include "sl_ieee802_types.h"
#include "sl_ip_types.h"
#include "sl_constants.h"
#include "sl_string.h"
#include "sl_utility.h"
#include "sl_net.h"
#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
/******************************************************
 *                      Macros
 ******************************************************/

#define MIN(a, b) ((a) < (b) ? (a) : (b))

// Taken from Azure SDK crt_abstractions.c
#define DIGIT_VAL(c)                             \
  (((c >= '0') && (c <= '9'))   ? (c - '0')      \
   : ((c >= 'a') && (c <= 'z')) ? (c - 'a' + 10) \
   : ((c >= 'A') && (c <= 'Z')) ? (c - 'A' + 10) \
                                : -1)

/******************************************************
 *                    Constants
 ******************************************************/

#ifndef COMMAND_HISTORY_BUFFER_SIZE
#define COMMAND_HISTORY_BUFFER_SIZE 128
#endif

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

typedef struct {
  uint8_t length;
  uint8_t data[];
} console_history_entry_t;

/******************************************************
 *               Static Function Declarations
 ******************************************************/

static inline uint8_t parse_enum_arg(const char *line, const char *const *options);

sl_status_t convert_string_to_mac_address(const char *line, sl_mac_address_t *mac);

char *string_token(register char *s, register const char *delim, char **lasts);
sl_status_t console_tokenize(char *start, char *end, char **token, char **token_end);

/******************************************************
 *               Variable Definitions
 ******************************************************/

const char *const console_argument_type_strings[] = {
  [CONSOLE_ARG_NONE]                   = "",
  [CONSOLE_ARG_UINT]                   = "uint",
  [CONSOLE_ARG_INT]                    = "int",
  [CONSOLE_ARG_STRING]                 = "string",
  [CONSOLE_ARG_MAC_ADDRESS]            = "xx:xx:xx:xx:xx",
  [CONSOLE_ARG_IP_ADDRESS]             = "x.x.x.x",
  [CONSOLE_ARG_HEX]                    = "0x...",
  [CONSOLE_ARG_REMAINING_COMMAND_LINE] = "...",
};

char console_history_buffer[COMMAND_HISTORY_BUFFER_SIZE];

uint32_t console_history_first = 0xFFFFFFFF;
uint32_t console_history_last  = 0xFFFFFFFF;
uint32_t console_history_end   = 0;

extern const arg_list_t console_argument_types[];
extern const value_list_t console_argument_values[];

/******************************************************
 *               Function Definitions
 ******************************************************/

sl_status_t console_parse_command(char *command_line,
                                  const console_database_t *db,
                                  console_args_t *args,
                                  const console_descriptive_command_t **output_command)
{
  sl_status_t status                    = SL_STATUS_OK;
  const console_database_entry_t *entry = NULL;
  char *token;
  uint32_t index;
  char *command_line_end = command_line + strlen(command_line); // The end should be passed in by the user

#ifdef CONSOLE_SUB_COMMAND_SUPPORT
start_processing_command_from_new_database:
#endif

  args->bitmap = 0;

  // Find first space
  char *iter = command_line;
  status     = console_tokenize(iter, command_line_end, &token, &iter);
  if (status != SL_STATUS_OK)
    return SL_STATUS_FAIL;

  // Try find matching command
  index  = 0;
  status = console_find_command(command_line, (uint32_t)(iter - command_line), db, &entry, &index);
  if (status != SL_STATUS_OK)
    return status;

  *output_command                              = (const console_descriptive_command_t *)entry->value;
  const console_argument_type_t *argument_list = &((*output_command)->argument_list[0]);

  // Try parse command args
  int arg_index = 0;
  int arg_count = 0;
  console_argument_type_t type;

  while (true) {
    type = argument_list[arg_index];

    SL_ASSERT(arg_count < CONSOLE_MAXIMUM_ARG_COUNT, "Command has too many args");

    status = console_tokenize(iter, command_line_end, &token, &iter);

    // If no more tokens are available, verify there are only optional arguments left
    if (status != SL_STATUS_OK) {
      if (type == CONSOLE_ARG_END) {
        return SL_STATUS_OK;
      } else if (type & CONSOLE_ARG_OPTIONAL) {
        arg_index += 2;
        ++arg_count;
        continue;
      }
      return SL_STATUS_COMMAND_IS_INVALID;
    }

    // Check for optional argument
    if (token[0] == '-' && ((token[1] >= 'a' && token[1] <= 'z') || (token[1] >= 'A' && token[1] <= 'Z'))) {
      // Find optional argument descriptor
      for (uint32_t a = 0, arg_number = 0; argument_list[a] != CONSOLE_ARG_END; ++a, ++arg_number) {
        if (!(argument_list[a] & CONSOLE_ARG_OPTIONAL)) {
          continue;
        }
        if ((argument_list[a] & 0x7F) != token[1]) {
          ++a;
          continue;
        }
        // Found a match!
        args->bitmap |= (1 << arg_number);
        status = console_tokenize(iter, command_line_end, &token, &iter);
        if (status == SL_STATUS_OK) {
          status = console_parse_arg(argument_list[a + 1], token, &args->arg[arg_number]);
        }
        break;
      }
      continue;
    }

#ifdef CONSOLE_SUB_COMMAND_SUPPORT // Note: Sub commands not yet supported
    if (type == CONSOLE_ARG_SUB_COMMAND) {
      command_line = token;        // Not sure if this is right
      db           = (const console_database_t *)((uint32_t)(*output_command)->argument_list[a + 1]
                                        + ((*output_command)->argument_list[a + 2] << 8)
                                        + ((*output_command)->argument_list[a + 3] << 16)
                                        + ((*output_command)->argument_list[a + 4] << 24));
      goto start_processing_command_from_new_database;
    }
#endif

    // Verify current ordered argument is not an optional argument
    for (; (type & CONSOLE_ARG_OPTIONAL);) {
      arg_index += 2;
      ++arg_count;
      type = argument_list[arg_index];
      if (type == CONSOLE_ARG_END) {
        return SL_STATUS_OK;
      }
    }

    status = console_parse_arg(type, token, &args->arg[arg_count]);
    if (status == SL_STATUS_OK) {
      args->bitmap |= (1 << arg_count);
    }
    ++arg_index;
    ++arg_count;
  }

  return status;
}

void console_add_to_history(const char *line, uint8_t line_length)
{
  const uint32_t entry_length = (line_length + sizeof(console_history_entry_t) + 1);
  //    uint32_t end = (console_history_last == 0xFFFFFFFF) ? 0 : (console_history_last + console_history_buffer[console_history_last] + 1) % sizeof( console_history_buffer );

  // Ensure there is enough space between the end and the start
  uint32_t available_space =
    (console_history_first == 0xFFFFFFFF)
      ? sizeof(console_history_buffer)
      : (sizeof(console_history_buffer) + console_history_first - console_history_end) % sizeof(console_history_buffer);

  // Drop items from the start until there is enough available space
  while (available_space <= entry_length) {
    SL_ASSERT(console_history_first != 0xFFFFFFFF, "BAD");
    available_space += console_history_buffer[console_history_first] + sizeof(console_history_entry_t) + 1;
    console_history_first =
      (console_history_first + console_history_buffer[console_history_first] + sizeof(console_history_entry_t) + 1)
      % sizeof(console_history_buffer);
  }

  // Write the entry header. There end should always point to the first available byte
  console_history_last = console_history_end;
  if (console_history_first == 0xFFFFFFFF) {
    console_history_first = console_history_last;
  }
  console_history_buffer[console_history_end] = line_length;

  console_history_end = (console_history_end + 1) % sizeof(console_history_buffer);

  // First contiguous copy
  uint8_t copy_length = (uint8_t)MIN((sizeof(console_history_buffer) - console_history_end), line_length);
  memcpy(&console_history_buffer[console_history_end], line, copy_length);
  console_history_end = (console_history_end + line_length) % sizeof(console_history_buffer);

  // Check if we need to copy more because we need to wrap around the end of the buffer
  if (copy_length != line_length) {
    memcpy(&console_history_buffer[0], line + copy_length, line_length - copy_length);
  }

  console_history_buffer[console_history_end] = line_length;
  console_history_end                         = (console_history_end + 1) % sizeof(console_history_buffer);
}

// Simple linear database search. This function could be replaced with a more complicated version
sl_status_t console_find_command(const char *command_string,
                                 uint32_t command_string_length,
                                 const console_database_t *db,
                                 const console_database_entry_t **entry,
                                 uint32_t *starting_index)
{
  // Validate inputs
  if ((command_string == NULL) || (command_string_length == 0) || (starting_index == NULL))
    return SL_STATUS_FAIL;

  for (uint32_t i = *starting_index; i < db->length; i++) {
    if (strncmp(command_string, db->entries[i].key, command_string_length) == 0) {
      *entry          = &(db->entries[i]);
      *starting_index = i;

      if (strlen(db->entries[i].key) == command_string_length) {
        return SL_STATUS_OK;
      } else {
        return SL_STATUS_COMMAND_INCOMPLETE;
      }
    }
  }

  return SL_STATUS_FAIL;
}

sl_status_t console_parse_arg(console_argument_type_t type, char *line, uint32_t *arg_result)
{
  if (type == CONSOLE_ARG_NONE) {
    *arg_result = 0;
    return SL_STATUS_OK;
  }

  if (type & CONSOLE_ARG_ENUM) {
    uint8_t enum_index  = type & CONSOLE_ARG_ENUM_INDEX_MASK;
    uint8_t value_index = parse_enum_arg(line, console_argument_types[enum_index]);
    if (value_index == 0xFF) {
      return SL_STATUS_COMMAND_IS_INVALID;
    }
    if (console_argument_values[enum_index] == NULL) {
      *arg_result = value_index;
    } else {
      *arg_result = console_argument_values[enum_index][value_index];
    }
    return SL_STATUS_OK;
  }

  // Otherwise parse standard arg types
  switch (type & CONSOLE_ARG_ENUM_INDEX_MASK) {
    case CONSOLE_ARG_UINT:
      *arg_result = strtoul(line, 0, 0);
      break;

    case CONSOLE_ARG_INT:
      *arg_result = strtoul(line, 0, 0);
      break;

    case CONSOLE_ARG_STRING:
      *arg_result = (uint32_t)line;
      break;

    case CONSOLE_ARG_MAC_ADDRESS: {
      sl_mac_address_t *temp_mac = (sl_mac_address_t *)line; // Write the converted value back into the line
      *arg_result                = (uint32_t)temp_mac;
      return convert_string_to_mac_address(line, temp_mac);
    } break;

    case CONSOLE_ARG_IP_ADDRESS: {
      sl_ipv4_address_t *temp_ip = (sl_ipv4_address_t *)line; // Write the converted value back into the line
      *arg_result                = (uint32_t)temp_ip;
      return sl_net_inet_addr(line, (uint32_t *)temp_ip);
    } break;

    case CONSOLE_ARG_HEX: {
      return 0;
    } break;
  }
  return SL_STATUS_OK;
}

static inline uint8_t parse_enum_arg(const char *line, const char *const *options)
{
  uint8_t a;
  for (a = 0; options[a] != NULL; ++a) {
    if (strcmp(line, options[a]) == 0) {
      return a;
    }
  }
  return 0xFF;
}

/** Convert a character string into a sl_mac_address_t
 *
 * @param line  Argument string that is expected to be like 00:11:22:33:44:55
 * @param mac
 * @return
 *    sl_status_t
 */
sl_status_t convert_string_to_mac_address(const char *line, sl_mac_address_t *mac)
{
  // Verify we have the exact number of characters. Basic argument verification
  if (sl_strnlen((char *)line, 18) != 17) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  uint8_t index = 0;
  while (index < 6) {
    // Read all the data and verify validity
    int char1 = DIGIT_VAL(line[0]);
    int char2 = DIGIT_VAL(line[1]);
    if (char1 == -1 || char2 == -1 || (line[2] != '\0' && line[2] != ':')) {
      return SL_STATUS_INVALID_PARAMETER;
    }

    // Store value
    mac->octet[index++] = (uint8_t)((uint8_t)char1 << 4) + (uint8_t)char2;
    line += 3;
  }

  return SL_STATUS_OK;
}

static bool escape(char *i, char *end)
{
  // Check if next character is one of the escaped ones
  if (i[1] == '"' || i[1] == '{' || i[1] == '\\') {
    // Next input matches, so shift array
    memmove(i, i + 1, (size_t)(end - i));
    return true;
  } else {
    // Unknown sequence
    return false;
  }
}

sl_status_t console_tokenize(char *start, char *end, char **token, char **token_end)
{
  // Get size of string and declare variables
  //  uint32_t input_size = strlen( input );
  //  uint32_t i = 0;
  char *i = start;

  // Ignore preceding space or null
  while (*i == ' ' || *i == '\0') {
    ++i;
    if (i >= end) {
      return SL_STATUS_FAIL;
    }
  }
  //  token_v[0] = input + i;
  //  *token_c = 1;
  *token = i;
  while (i < end) {
    if (*i == '{' || *i == '"') {
      // Start of segment where splitting should not be performed
      char end_char;
      if (*i == '{') {
        end_char = '}';
        *token   = i;
      } else {
        end_char = '"';
        // Token ignores " character
        *token = i + 1;
        //        token_v[*token_c - 1] = input + i + 1;
      }
      // Verify that preceding character is space or end of string
      if (i > start && (i[-1] != ' ' && i[-1] != '\0')) {
        return SL_STATUS_INVALID_PARAMETER;
      }

      // Loop through input to find end character
      i++;
      while (*i != end_char) {
        if (*i == '\\') {
          // Next character should be escaped
          if (!escape(i, end)) {
            // Escape error
            return SL_STATUS_INVALID_PARAMETER;
          }
          --end;
        }
        ++i;
        if (i >= end) {
          // End character not found, return error
          return SL_STATUS_INVALID_PARAMETER;
        }
      }
      // Verify that next character is space or end of string
      if (i[1] != ' ' && i[1] != '\0' && &i[1] != end) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      // Remove " from string end
      if (end_char == '"') {
        *i         = '\0';
        *token_end = i;
      } else {
        *token_end = i + 1;
      }
      return SL_STATUS_OK;
    } // Escape symbol encountered
    else if (*i == '\\') {
      if (!escape(i, end)) {
        // Escape error
        return SL_STATUS_INVALID_PARAMETER;
      }
      --end;

      // Ordinary splitting
    } else if (*i == ' ') {
      // Turn space into '\0' to indicate end of string
      *i         = '\0';
      *token_end = i;
      return SL_STATUS_OK;
    }
    ++i;
  }

  *token_end = i;
  return SL_STATUS_OK;
}
