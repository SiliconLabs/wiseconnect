//#include "console_variable_commands.h"
#include "console.h"
//#include "console_variables.h"
//#include "sl_nvm.h"
//#include "sl_wifi_types.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/******************************************************
 *                      Macros
 ******************************************************/

#define member_size(type, member) sizeof(((type *)0)->member)
#define MIN(a, b)                 ((a < b) ? a : b)
#define UNUSED_PARAMETER(x) (void)(x)

/******************************************************
 *                    Constants
 ******************************************************/

#define KEY_SEPARATOR "."

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

const structure_descriptor_entry_t *find_structure_entry(const char *key,
                                                         const structure_descriptor_entry_t *table,
                                                         uint32_t table_size);
static void print_variable(const structure_descriptor_entry_t *entry, const void *object);

sl_status_t console_variable_get(console_args_t *arguments);
sl_status_t console_variable_set(console_args_t *arguments);

/******************************************************
 *               Variable Definitions
 ******************************************************/

//extern nvm3_Handle_t nvm3_handle;

//sl_status_t wlan_client_profile_handler(console_variable_action_t action, console_args_t* arguments, const console_variable_node_t* node );
//sl_status_t wlan_credential_handler(console_variable_action_t action, console_args_t* arguments, const console_variable_node_t* node );
//sl_status_t wlan_client_scan_handler(console_variable_action_t action, console_args_t* arguments, const console_variable_node_t* node );

extern const unsigned int first_group_end;
extern const console_variable_node_t nodes[];

/******************************************************
 *               Function Definitions
 ******************************************************/

//sl_status_t process_variable_request(console_variable_action_t action, console_args_t* arguments)
static sl_status_t find_variable_node(char **key, const console_variable_node_t **node)
{
  const char *token  = strtok(*key, KEY_SEPARATOR);
  uint32_t iter      = 0;
  uint32_t table_end = first_group_end;

  // Process key by traversing through the CLI variable table
  while (token != NULL) {
    // Check for a key match
    if (strcmp(token, nodes[iter].key) == 0) {
      switch (nodes[iter].type) {
        case SL_CONSOLE_VARIABLE_GROUP_NODE:
          table_end = nodes[iter].data.group.last_index;
          iter      = nodes[iter].data.group.first_index;
          break;

        default:
          *key  = strtok(NULL, KEY_SEPARATOR);
          *node = &nodes[iter];
          return SL_STATUS_OK;
          //
          //        case SL_CONSOLE_VARIABLE_RAM_STRUCT_NODE:
          //          return nodes[iter].data.ram_structure.handler(action, arguments, &nodes[iter]);
          //          break;
          //
          //        case SL_CONSOLE_VARIABLE_CUSTOM_STRUCT_NODE:
          //          return nodes[iter].data.custom_structure.handler(action, arguments, &nodes[iter]);
          //          break;
          //
          //        case SL_CONSOLE_VARIABLE_VARIABLE_NODE:
          //          break;
      }
    } else {
      ++iter;
      if (iter > table_end)
        return SL_STATUS_ABORT;
      continue;
    }

    token = strtok(NULL, KEY_SEPARATOR);
  }

  return SL_STATUS_ABORT;
}

#ifdef subcommand_FEATURE_REQUIRED
/*
 * get <variable>
 * set <variable> [item=value] [item2=value2]
 */
static sl_status_t process_structure_args(console_variable_action_t action,
                                          console_args_t *arguments,
                                          void *object,
                                          const structure_descriptor_entry_t *table,
                                          uint32_t table_size)
{
  // Check if no sub-sections are named. If so, print the full structure
  if (!IS_CONSOLE_ARG_VALID(arguments, 2)) {
    printf("\n");
    for (unsigned int a = 0; a < table_size; ++a) {
      printf("  %s: ", table[a].key);
      print_variable(&table[a], object);
    }
    return SL_STATUS_OK;
  }

  for (uint32_t index = 1; IS_CONSOLE_ARG_VALID(arguments, index); ++index) {
    char *temp = (char *)arguments->arg[index];

    // Find the key
    const char *key = strtok(temp, "=");

    // Find entry in the structure
    const structure_descriptor_entry_t *entry = find_structure_entry(key, table, table_size);

    if (entry == NULL) {
      printf("Struct does not contain %s\n", key);
      return SL_STATUS_ABORT;
    }

    // Do stuff
    switch (action) {
      case SL_CONSOLE_VARIABLE_GET:
        printf("\t%s: ", entry->key);
        print_variable(entry, object);
        break;

      case SL_CONSOLE_VARIABLE_SET:
        temp = strtok(NULL, " ");
        if (temp != NULL) {
          //          uint32_t memory;
          //          void *argv[1];
          //          int mem_index = 0;
          //          int arg_index = 0;
          //          if (sli_cli_validate_argument(entry->type, temp)) {
          //            sli_cli_arguments_convert_single(entry->type, temp, argv, &memory, &arg_index, &mem_index);
          //            if (entry->type == CONSOLE_ARG_STRING) {
          //              memcpy(object + entry->offset, argv[0], MIN(entry->size, strlen((char*)argv[0])));
          //            } else {
          //              memcpy(object + entry->offset, argv[0], entry->size);
          //            }
          //          }
        }
        break;

      default:
        break;
    }
  }
  return SL_STATUS_OK;
}
#endif

static void print_variable(const structure_descriptor_entry_t *entry, const void *object)
{
  switch (entry->type) {
    case CONSOLE_VARIABLE_UINT:
      switch (entry->size) {
        case 1:
          printf("0x%X\n", *(uint8_t *)(object + entry->offset));
          break;
        case 2:
          printf("0x%X\n", *(uint16_t *)(object + entry->offset));
          break;
        case 4:
          printf("0x%lX\n", *(uint32_t *)(object + entry->offset));
          break;
        default:
          break;
      }
      break;

    case CONSOLE_VARIABLE_INT:
      switch (entry->size) {
        case 1:
          printf("%d\n", *(int8_t *)(object + entry->offset));
          break;
        case 2:
          printf("%d\n", *(int16_t *)(object + entry->offset));
          break;
        case 4:
          printf("%ld\n", *(int32_t *)(object + entry->offset));
          break;
        default:
          break;
      }
      break;

    case CONSOLE_VARIABLE_STRING:
      printf("%s\n", (char *)(object + entry->offset));
      break;

    default:
      break;
  }
}

const structure_descriptor_entry_t *find_structure_entry(const char *key,
                                                         const structure_descriptor_entry_t *table,
                                                         uint32_t table_size)
{
  for (unsigned int a = 0; a < table_size; ++a) {
    if (strcmp(key, table[a].key) == 0)
      return &table[a];
  }

  return NULL;
}

sl_status_t console_variable_get(console_args_t *arguments)
{
  const console_variable_node_t *node;
  char *key          = (char *)arguments->arg[0];
  sl_status_t status = find_variable_node(&key, &node);
  if (status != SL_STATUS_OK)
    return status;

  //  if (node->type == SL_CONSOLE_VARIABLE_RAM_STRUCT_NODE) {
  //    find_structure_entry(key, nodes[iter].data.ram_structure.table, nodes[iter].data.ram_structure.table_size );
  //  }
  //  print_variable(entry, object)

  //  process_variable_request(SL_CONSOLE_VARIABLE_GET, arguments);
  return status;
}

sl_status_t console_variable_set(console_args_t *arguments)
{
  const console_variable_node_t *node;
  char *key          = (char *)arguments->arg[0];
  sl_status_t status = find_variable_node(&key, &node);
  if (status != SL_STATUS_OK)
    return status;

  if (node->type == SL_CONSOLE_VARIABLE_RAM_STRUCT_NODE) {
    if (key == NULL)
      return SL_STATUS_COMMAND_IS_INVALID;

    const structure_descriptor_entry_t *entry =
      find_structure_entry(key, node->data.ram_structure.table, node->data.ram_structure.table_size);
    if (entry == NULL)
      return SL_STATUS_COMMAND_IS_INVALID;

    uint32_t arg_result;
    status = console_parse_arg(entry->type, (char *)arguments->arg[1], &arg_result);
    if (status != SL_STATUS_OK)
      return status;

    switch (entry->type) {
      case CONSOLE_VARIABLE_UINT:
      case CONSOLE_VARIABLE_INT:
        memcpy(node->data.ram_structure.object + entry->offset, &arg_result, entry->size);
        break;

      case CONSOLE_VARIABLE_STRING:
      case CONSOLE_VARIABLE_IP_ADDRESS:
      case CONSOLE_VARIABLE_MAC_ADDRESS:
        memcpy(node->data.ram_structure.object + entry->offset, (void *)arg_result, entry->size);
        break;

      default:
        return SL_STATUS_COMMAND_IS_INVALID;
    }
  }
  return status;
}

sl_status_t console_variable_list(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  static const char spaces[] = "                ";
#define MAX_LEVEL_DEPTH ((sizeof(spaces) - 1) / 2)

  uint8_t level = 0;
  uint32_t table_index[MAX_LEVEL_DEPTH];
  uint32_t table_end[MAX_LEVEL_DEPTH];
  memset(table_index, 0, sizeof(table_index));

  table_end[0]   = first_group_end;
  table_index[0] = 0;

  uint32_t iter = 0;

  printf("\nConfigurable variables\n");

  while (1) {
    switch (nodes[iter].type) {
      case SL_CONSOLE_VARIABLE_GROUP_NODE:
        printf("%.*s%s:\n", level * 2, spaces, nodes[iter].key);
        ++table_index[level];
        ++level;
        table_end[level]   = nodes[iter].data.group.last_index;
        table_index[level] = nodes[iter].data.group.first_index;
        break;

      case SL_CONSOLE_VARIABLE_RAM_STRUCT_NODE:
        printf("%.*s%s:\n", level * 2, spaces, nodes[iter].key);
        ++table_index[level];
        ++level;
        const structure_descriptor_entry_t *table = nodes[iter].data.ram_structure.table;
        const uint32_t table_size                 = nodes[iter].data.ram_structure.table_size;
        for (unsigned int a = 0; a < table_size; ++a) {
          printf("%.*s%s: ", level * 2, spaces, table[a].key);
          print_variable(&table[a], nodes[iter].data.ram_structure.object);
        }
        --level;
        break;

#ifdef nvm3_FEATURE_REQUIRED
      case SL_CONSOLE_VARIABLE_NVM_STRUCT_NODE:
        printf("%.*s%s: ", level * 2, spaces, nodes[iter].key);
        uint32_t type;
        size_t length;
        sl_status_t status =
          sl_nvm_get_object_info(&nvm3_handle, nodes[iter].data.structure.data.nvm_id, &type, &length);
        if (status == SL_STATUS_OK) {
          printf("Data available\n");
        } else {
          printf("No data\n");
        }
        ++table_index[level];
        break;
#endif

      case SL_CONSOLE_VARIABLE_VARIABLE_NODE:
        printf("%.*s%s:\n", level * 2, spaces, nodes[iter].key);
        ++table_index[level];
        break;
    }

    // Make sure we have a valid node, otherwise go up the tree until we do
    while (table_index[level] > table_end[level]) {
      if (level == 0) {
        return SL_STATUS_OK;
      }
      --level;
    }
    iter = table_index[level];
  }

  return SL_STATUS_OK;
}
