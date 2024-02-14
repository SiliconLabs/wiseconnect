/*******************************************************************************
* @file  console_variable_table.c.template
* @brief 
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

#include "console_types.h"
#include "console_constants.h"
#include "nvm_ids.h"
#include "wifi_commands.h"

const structure_descriptor_entry_t config_struct_descriptor[] = {
  SL_CONSOLE_STRUCTURE_ENTRY("oper_mode", oper_mode, sl_si91x_boot_configuration_t, CONSOLE_VARIABLE_UINT),
  SL_CONSOLE_STRUCTURE_ENTRY("coex_mode", coex_mode, sl_si91x_boot_configuration_t, CONSOLE_VARIABLE_UINT),
  SL_CONSOLE_STRUCTURE_ENTRY("feature_bit_map", feature_bit_map, sl_si91x_boot_configuration_t, CONSOLE_VARIABLE_UINT),
  SL_CONSOLE_STRUCTURE_ENTRY("tcp_ip_feature_bit_map",
                             tcp_ip_feature_bit_map,
                             sl_si91x_boot_configuration_t,
                             CONSOLE_VARIABLE_UINT),
  SL_CONSOLE_STRUCTURE_ENTRY("custom_feature_bit_map",
                             custom_feature_bit_map,
                             sl_si91x_boot_configuration_t,
                             CONSOLE_VARIABLE_UINT),
  SL_CONSOLE_STRUCTURE_ENTRY("ext_custom_feature_bit_map",
                             ext_custom_feature_bit_map,
                             sl_si91x_boot_configuration_t,
                             CONSOLE_VARIABLE_UINT),
  SL_CONSOLE_STRUCTURE_ENTRY("bt_feature_bit_map",
                             bt_feature_bit_map,
                             sl_si91x_boot_configuration_t,
                             CONSOLE_VARIABLE_UINT),
  SL_CONSOLE_STRUCTURE_ENTRY("ext_tcp_ip_feature_bit_map",
                             ext_tcp_ip_feature_bit_map,
                             sl_si91x_boot_configuration_t,
                             CONSOLE_VARIABLE_UINT),
  SL_CONSOLE_STRUCTURE_ENTRY("ble_feature_bit_map",
                             ble_feature_bit_map,
                             sl_si91x_boot_configuration_t,
                             CONSOLE_VARIABLE_UINT),
  SL_CONSOLE_STRUCTURE_ENTRY("ble_ext_feature_bit_map",
                             ble_ext_feature_bit_map,
                             sl_si91x_boot_configuration_t,
                             CONSOLE_VARIABLE_UINT),
  SL_CONSOLE_STRUCTURE_ENTRY("config_feature_bit_map",
                             config_feature_bit_map,
                             sl_si91x_boot_configuration_t,
                             CONSOLE_VARIABLE_UINT),
};

console_variable_node_t wifi_group[] = {
  SL_CONSOLE_VARIABLE_RAM_STRUCTURE("config", config_struct_descriptor, si91x_init_configuration.boot_config),
};

const console_variable_node_t console_variable_table[] = {
  SL_CONSOLE_VARIABLE_GROUP("wifi", wifi_group),
};

const uint32_t console_variable_table_size = sizeof(console_variable_table) / sizeof(console_variable_node_t);