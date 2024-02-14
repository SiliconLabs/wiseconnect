/*******************************************************************************
* @file  console_command_database.c.template
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

#include "console.h"
#include "console_argument_types.h"

extern sl_status_t aws_iot_finalize_json_document_command(console_args_t *arguments);
static const char *_aws_iot_finalize_json_document_arg_help[] = {};

static const console_descriptive_command_t _aws_iot_finalize_json_document_command = {
  .description   = "aws iot finalize json document",
  .argument_help = _aws_iot_finalize_json_document_arg_help,
  .handler       = aws_iot_finalize_json_document_command,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t aws_iot_mqtt_autoreconnect_set_status_command(console_args_t *arguments);
static const char *_aws_iot_mqtt_autoreconnect_set_status_arg_help[] = {};

static const console_descriptive_command_t _aws_iot_mqtt_autoreconnect_set_status_command = {
  .description   = "aws iot mqtt autoreconnect set status",
  .argument_help = _aws_iot_mqtt_autoreconnect_set_status_arg_help,
  .handler       = aws_iot_mqtt_autoreconnect_set_status_command,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t aws_iot_mqtt_connect_command(console_args_t *arguments);
static const char *_aws_iot_mqtt_connect_arg_help[] = {};

static const console_descriptive_command_t _aws_iot_mqtt_connect_command = { .description = "aws iot mqtt connect",
                                                                             .argument_help =
                                                                               _aws_iot_mqtt_connect_arg_help,
                                                                             .handler = aws_iot_mqtt_connect_command,
                                                                             .argument_list = { CONSOLE_ARG_END } };

extern sl_status_t aws_iot_mqtt_disconnect_command(console_args_t *arguments);
static const char *_aws_iot_mqtt_disconnect_arg_help[] = {};

static const console_descriptive_command_t _aws_iot_mqtt_disconnect_command = {
  .description   = "aws iot mqtt disconnect command",
  .argument_help = _aws_iot_mqtt_disconnect_arg_help,
  .handler       = aws_iot_mqtt_disconnect_command,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t aws_iot_mqtt_init_command(console_args_t *arguments);
static const char *_aws_iot_mqtt_init_arg_help[] = {
  0, 0, 0, 0, 0,
};

static const console_descriptive_command_t _aws_iot_mqtt_init_command = { .description   = "Initializes AWS Mqtt",
                                                                          .argument_help = _aws_iot_mqtt_init_arg_help,
                                                                          .handler       = aws_iot_mqtt_init_command,
                                                                          .argument_list = { CONSOLE_ARG_STRING,
                                                                                             CONSOLE_ARG_UINT,
                                                                                             CONSOLE_ARG_STRING,
                                                                                             CONSOLE_ARG_UINT,
                                                                                             CONSOLE_ARG_UINT,
                                                                                             CONSOLE_ARG_END } };

extern sl_status_t aws_iot_mqtt_publish_command(console_args_t *arguments);
static const char *_aws_iot_mqtt_publish_arg_help[] = {
  0,
  0,
  0,
};

static const console_descriptive_command_t _aws_iot_mqtt_publish_command = {
  .description   = "aws iot mqtt   publish",
  .argument_help = _aws_iot_mqtt_publish_arg_help,
  .handler       = aws_iot_mqtt_publish_command,
  .argument_list = { CONSOLE_ARG_STRING, CONSOLE_ARG_UINT, CONSOLE_ARG_STRING, CONSOLE_ARG_END }
};

extern sl_status_t aws_iot_mqtt_publish_command(console_args_t *arguments);
static const char *_aws_iot_mqtt_publish_more_arg_help[] = {
  0,
  0,
  0,
};

static const console_descriptive_command_t _aws_iot_mqtt_publish_more_command = {
  .description   = "aws iot mqtt   publish",
  .argument_help = _aws_iot_mqtt_publish_more_arg_help,
  .handler       = aws_iot_mqtt_publish_command,
  .argument_list = { CONSOLE_ARG_STRING, CONSOLE_ARG_UINT, CONSOLE_ARG_HEX, CONSOLE_ARG_END }
};

extern sl_status_t aws_iot_mqtt_subscribe_command(console_args_t *arguments);
static const char *_aws_iot_mqtt_subscribe_arg_help[] = {
  0,
  0,
};

static const console_descriptive_command_t _aws_iot_mqtt_subscribe_command = {
  .description   = "aws iot mqtt subscribe",
  .argument_help = _aws_iot_mqtt_subscribe_arg_help,
  .handler       = aws_iot_mqtt_subscribe_command,
  .argument_list = { CONSOLE_ARG_UINT, CONSOLE_ARG_STRING, CONSOLE_ARG_END }
};

extern sl_status_t aws_iot_mqtt_unsubscribe_command(console_args_t *arguments);
static const char *_aws_iot_mqtt_unsubscribe_arg_help[] = {
  0,
};

static const console_descriptive_command_t _aws_iot_mqtt_unsubscribe_command = {
  .description   = "aws iot mqtt unsubscribe",
  .argument_help = _aws_iot_mqtt_unsubscribe_arg_help,
  .handler       = aws_iot_mqtt_unsubscribe_command,
  .argument_list = { CONSOLE_ARG_STRING, CONSOLE_ARG_END }
};

extern sl_status_t aws_iot_shadow_add_reported_command(console_args_t *arguments);
static const char *_aws_iot_shadow_add_reported_arg_help[] = {};

static const console_descriptive_command_t _aws_iot_shadow_add_reported_command = {
  .description   = "aws iot shadow add reported",
  .argument_help = _aws_iot_shadow_add_reported_arg_help,
  .handler       = aws_iot_shadow_add_reported_command,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t aws_iot_shadow_connect_command(console_args_t *arguments);
static const char *_aws_iot_shadow_connect_arg_help[] = {};

static const console_descriptive_command_t _aws_iot_shadow_connect_command = {
  .description   = "Connect AWS device shadow",
  .argument_help = _aws_iot_shadow_connect_arg_help,
  .handler       = aws_iot_shadow_connect_command,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t aws_iot_shadow_init_command(console_args_t *arguments);
static const char *_aws_iot_shadow_init_arg_help[] = {
  0,
  0,
  0,
  0,
};

static const console_descriptive_command_t _aws_iot_shadow_init_command = {
  .description   = "Initializes AWS device shadow",
  .argument_help = _aws_iot_shadow_init_arg_help,
  .handler       = aws_iot_shadow_init_command,
  .argument_list = { CONSOLE_ARG_STRING, CONSOLE_ARG_UINT, CONSOLE_ARG_STRING, CONSOLE_ARG_STRING, CONSOLE_ARG_END }
};

extern sl_status_t aws_iot_shadow_init_json_document_command(console_args_t *arguments);
static const char *_aws_iot_shadow_init_json_document_arg_help[] = {};

static const console_descriptive_command_t _aws_iot_shadow_init_json_document_command = {
  .description   = "aws iot shadow init json document",
  .argument_help = _aws_iot_shadow_init_json_document_arg_help,
  .handler       = aws_iot_shadow_init_json_document_command,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t aws_iot_shadow_register_delta_command(console_args_t *arguments);
static const char *_aws_iot_shadow_register_delta_arg_help[] = {};

static const console_descriptive_command_t _aws_iot_shadow_register_delta_command = {
  .description   = "aws iot shadow register delta",
  .argument_help = _aws_iot_shadow_register_delta_arg_help,
  .handler       = aws_iot_shadow_register_delta_command,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t aws_iot_shadow_update_command(console_args_t *arguments);
static const char *_aws_iot_shadow_update_arg_help[] = {};

static const console_descriptive_command_t _aws_iot_shadow_update_command = { .description = "aws iot shadow update",
                                                                              .argument_help =
                                                                                _aws_iot_shadow_update_arg_help,
                                                                              .handler = aws_iot_shadow_update_command,
                                                                              .argument_list = { CONSOLE_ARG_END } };

extern sl_status_t aws_iot_shadow_yield_command(console_args_t *arguments);
static const char *_aws_iot_shadow_yield_arg_help[] = {};

static const console_descriptive_command_t _aws_iot_shadow_yield_command = { .description = "aws iot shadow yield",
                                                                             .argument_help =
                                                                               _aws_iot_shadow_yield_arg_help,
                                                                             .handler = aws_iot_shadow_yield_command,
                                                                             .argument_list = { CONSOLE_ARG_END } };

extern sl_status_t aws_select_socket_command(console_args_t *arguments);
static const char *_aws_select_socket_arg_help[] = {};

static const console_descriptive_command_t _aws_select_socket_command = { .description   = "aws select connect command",
                                                                          .argument_help = _aws_select_socket_arg_help,
                                                                          .handler       = aws_select_socket_command,
                                                                          .argument_list = { CONSOLE_ARG_END } };

extern sl_status_t rsi_ble_add_char_serv_att_command(console_args_t *arguments);
static const char *_ble_add_char_serv_att_arg_help[] = {
  "uuid of service",
  "READ-0x02 WRITE-0x08 NOTIFY-0x10",
  "Server handle number",
  "Value handle number",
};

static const console_descriptive_command_t _ble_add_char_serv_att_command = {
  .description   = "BLE add characteristic service attribute to the service for writing",
  .argument_help = _ble_add_char_serv_att_arg_help,
  .handler       = rsi_ble_add_char_serv_att_command,
  .argument_list = { CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_add_char_val_att_command(console_args_t *arguments);
static const char *_ble_add_char_val_att_arg_help[] = {
  "uuid of service", "READ-0x02 WRITE-0x08 NOTIFY-0x10", "Value handle number", "Data to set", "enable_custom_feature",
  "custom_feature",
};

static const console_descriptive_command_t _ble_add_char_val_att_command = {
  .description   = "BLE add characteristic value attribute to the service for writing",
  .argument_help = _ble_add_char_val_att_arg_help,
  .handler       = rsi_ble_add_char_val_att_command,
  .argument_list = { CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_STRING,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_add_service_command(console_args_t *arguments);
static const char *_ble_add_service_arg_help[] = {
  "uuid of service",
};

static const console_descriptive_command_t _ble_add_service_command = { .description   = "BLE add new service",
                                                                        .argument_help = _ble_add_service_arg_help,
                                                                        .handler       = rsi_ble_add_service_command,
                                                                        .argument_list = { CONSOLE_ARG_INT,
                                                                                           CONSOLE_ARG_END } };

extern sl_status_t rsi_ble_add_white_list_comamnd(console_args_t *arguments);
static const char *_ble_add_white_list_arg_help[] = {
  "remote_dev_addr",
  "remote_addr_type",
};

static const console_descriptive_command_t _ble_add_white_list_command = {
  .description   = "add ble device to white list",
  .argument_help = _ble_add_white_list_arg_help,
  .handler       = rsi_ble_add_white_list_comamnd,
  .argument_list = { CONSOLE_ARG_STRING, CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_white_list_using_adv_data_comamnd(console_args_t *arguments);
static const char *_ble_add_white_list_adv_data_arg_help[] = {
  "enable",
  "data_compare_index",
  "compare_data_length",
  "payload",
};

static const console_descriptive_command_t _ble_add_white_list_adv_data_command = {
  .description   = "start adv with white list",
  .argument_help = _ble_add_white_list_adv_data_arg_help,
  .handler       = rsi_ble_white_list_using_adv_data_comamnd,
  .argument_list = { CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_STRING, CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_ae_dev_to_periodic_list_command(console_args_t *arguments);
static const char *_ble_ae_dev_to_periodic_list_arg_help[] = {
  "remote_dev_addr",
  "adv_addr_type",
  "type",
};

static const console_descriptive_command_t _ble_ae_dev_to_periodic_list_command = {
  .description   = "add device to periodic list",
  .argument_help = _ble_ae_dev_to_periodic_list_arg_help,
  .handler       = rsi_ble_ae_dev_to_periodic_list_command,
  .argument_list = { CONSOLE_ARG_STRING, CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_ae_read_periodic_adv_list_size_command(console_args_t *arguments);
static const char *_ble_ae_read_periodic_adv_list_size_arg_help[] = {
  "size",
};

static const console_descriptive_command_t _ble_ae_read_periodic_adv_list_size_command = {
  .description   = "read size of payload",
  .argument_help = _ble_ae_read_periodic_adv_list_size_arg_help,
  .handler       = rsi_ble_ae_read_periodic_adv_list_size_command,
  .argument_list = { CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_ae_set_periodic_sync_command(console_args_t *arguments);
static const char *_ble_ae_set_periodic_sync_arg_help[] = {
  "remote_dev_addr", "adv_addr_type", "adv_sid", "fil_policy", "skip", "sync_timeout", "BLE_AE_PER_SYNC",
};

static const console_descriptive_command_t _ble_ae_set_periodic_sync_command = {
  .description   = "set periodic async",
  .argument_help = _ble_ae_set_periodic_sync_arg_help,
  .handler       = rsi_ble_ae_set_periodic_sync_command,
  .argument_list = { CONSOLE_ARG_STRING,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_ae_set_scan_enable_command(console_args_t *arguments);
static const char *_ble_ae_set_scan_enable_arg_help[] = {
  "enable",
  "filter_duplicates",
  "duration",
  "period",
};

static const console_descriptive_command_t _ble_ae_set_scan_enable_command = {
  .description   = "enable ae scan",
  .argument_help = _ble_ae_set_scan_enable_arg_help,
  .handler       = rsi_ble_ae_set_scan_enable_command,
  .argument_list = { CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_ae_set_scan_params_command(console_args_t *arguments);
static const char *_ble_ae_set_scan_params_arg_help[] = {
  "addr_type", "scanning_filter_policy", "scanning_phys", "ScanInterval", "ScanWindow", "scan_type",
};

static const console_descriptive_command_t _ble_ae_set_scan_params_command = {
  .description   = "set ae scan params",
  .argument_help = _ble_ae_set_scan_params_arg_help,
  .handler       = rsi_ble_ae_set_scan_params_command,
  .argument_list = { CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_clear_acceptlist_command(console_args_t *arguments);
static const char *_ble_clear_white_list_arg_help[] = {};

static const console_descriptive_command_t _ble_clear_white_list_command = {
  .description   = "clear the accept list",
  .argument_help = _ble_clear_white_list_arg_help,
  .handler       = rsi_ble_clear_acceptlist_command,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_connect_comamnd(console_args_t *arguments);
static const char *_ble_connect_arg_help[] = {
  "device address <xx:xx:xx:xx:xx:xx>",
  "scan interval",
  "scan window",
  "connection interval min",
  "connection interval max",
  "connection latency",
  "supervision timeout",
};

static const console_descriptive_command_t _ble_connect_command = { .description   = "Initiate the connection",
                                                                    .argument_help = _ble_connect_arg_help,
                                                                    .handler       = rsi_ble_connect_comamnd,
                                                                    .argument_list = { CONSOLE_ARG_STRING,
                                                                                       CONSOLE_ARG_UINT,
                                                                                       CONSOLE_ARG_UINT,
                                                                                       CONSOLE_ARG_UINT,
                                                                                       CONSOLE_ARG_UINT,
                                                                                       CONSOLE_ARG_UINT,
                                                                                       CONSOLE_ARG_UINT,
                                                                                       CONSOLE_ARG_END } };

extern sl_status_t rsi_ble_connect_cancel_command(console_args_t *arguments);
static const char *_ble_connect_cancel_arg_help[] = {
  "device_address",
};

static const console_descriptive_command_t _ble_connect_cancel_command = {
  .description   = "cancels the connection to remote ble device",
  .argument_help = _ble_connect_cancel_arg_help,
  .handler       = rsi_ble_connect_cancel_command,
  .argument_list = { CONSOLE_ARG_STRING, CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_deletefrom_acceptlist_command(console_args_t *arguments);
static const char *_ble_deletefrom_white_list_arg_help[] = {
  "remote_dev_bd_addr",
  "dev_addr_type",
};

static const console_descriptive_command_t _ble_deletefrom_white_list_command = {
  .description   = "delete address from accept_list",
  .argument_help = _ble_deletefrom_white_list_arg_help,
  .handler       = rsi_ble_deletefrom_acceptlist_command,
  .argument_list = { CONSOLE_ARG_STRING, CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_disconnect_comamnd(console_args_t *arguments);
static const char *_ble_disconnect_arg_help[] = {
  "device address <xx:xx:xx:xx:xx:xx>",
};

static const console_descriptive_command_t _ble_disconnect_command = { .description   = "Disconnect",
                                                                       .argument_help = _ble_disconnect_arg_help,
                                                                       .handler       = rsi_ble_disconnect_comamnd,
                                                                       .argument_list = { CONSOLE_ARG_STRING,
                                                                                          CONSOLE_ARG_END } };

extern sl_status_t rsi_ble_execute_write_async_command(console_args_t *arguments);
static const char *_ble_execute_write_async_arg_help[] = {
  "remote_dev_addr",
  "exe_flag",
};

static const console_descriptive_command_t _ble_execute_write_async_command = {
  .description   = "ble execute write async",
  .argument_help = _ble_execute_write_async_arg_help,
  .handler       = rsi_ble_execute_write_async_command,
  .argument_list = { CONSOLE_ARG_STRING, CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_extended_connect_with_params_command(console_args_t *arguments);
static const char *_ble_extended_connect_with_params_arg_help[] = {
  "remote_dev_addr", "remote_addr_type", "initiator_filter_policy", "init_phys",   "ScanInterval",
  "ScanWindow",      "ConnIntervalMin",  "ConnIntervalMax",         "ConnLatency", "ConnSTO",
  "MinCELen",        "MaxCELen",         "own_addr_type",
};

static const console_descriptive_command_t _ble_extended_connect_with_params_command = {
  .description   = "initiates AE connection",
  .argument_help = _ble_extended_connect_with_params_arg_help,
  .handler       = rsi_ble_extended_connect_with_params_command,
  .argument_list = { CONSOLE_ARG_STRING,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_get_att_descriptors_async_command(console_args_t *arguments);
static const char *_ble_get_att_desc_arg_help[] = {
  "device address",
  "start_handle",
  "end_handle",
};

static const console_descriptive_command_t _ble_get_att_desc_command = {
  .description   = "characteristic descriptors list from the remote device",
  .argument_help = _ble_get_att_desc_arg_help,
  .handler       = rsi_ble_get_att_descriptors_async_command,
  .argument_list = { CONSOLE_ARG_STRING, CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_get_att_value_async_command(console_args_t *arguments);
static const char *_ble_get_att_value_async_arg_help[] = {
  "device address <xx:xx:xx:xx:xx:xx>",
  "handle",
};

static const console_descriptive_command_t _ble_get_att_value_async_command = {
  .description   = "BLE get characteristics descriptor",
  .argument_help = _ble_get_att_value_async_arg_help,
  .handler       = rsi_ble_get_att_value_async_command,
  .argument_list = { CONSOLE_ARG_STRING, CONSOLE_ARG_INT, CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_get_char_services_async_command(console_args_t *arguments);
static const char *_ble_get_char_services_async_arg_help[] = {
  "device address <xx:xx:xx:xx:xx:xx>",
  "start handle of profile",
  "end handle of profile",
};

static const console_descriptive_command_t _ble_get_char_services_async_command = {
  .description   = "BLE get service characteristics",
  .argument_help = _ble_get_char_services_async_arg_help,
  .handler       = rsi_ble_get_char_services_async_command,
  .argument_list = { CONSOLE_ARG_STRING, CONSOLE_ARG_INT, CONSOLE_ARG_INT, CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_get_char_value_by_uuid_async_command(console_args_t *arguments);
static const char *_ble_get_char_value_by_uuid_arg_help[] = {
  "remote_dev_addr",
  "start handle of devices",
  "end handle of devices",
  "uuid",
};

static const console_descriptive_command_t _ble_get_char_value_by_uuid_command = {
  .description   = "get char value by uuid",
  .argument_help = _ble_get_char_value_by_uuid_arg_help,
  .handler       = rsi_ble_get_char_value_by_uuid_async_command,
  .argument_list = { CONSOLE_ARG_STRING, CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_get_device_state_command(console_args_t *arguments);
static const char *_ble_get_device_state_arg_help[] = {};

static const console_descriptive_command_t _ble_get_device_state_command = {
  .description   = "Gets the local device state BIT(0) Advertising state, BIT(1) Scanning state, BIT(2) Initiating "
                   "state, BIT(3) Connected state",
  .argument_help = _ble_get_device_state_arg_help,
  .handler       = rsi_ble_get_device_state_command,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_get_max_adv_data_len_command(console_args_t *arguments);
static const char *_ble_get_max_adv_data_len_arg_help[] = {
  "max_data_len",
};

static const console_descriptive_command_t _ble_get_max_adv_data_len_command = {
  .description   = "get max_data_length",
  .argument_help = _ble_get_max_adv_data_len_arg_help,
  .handler       = rsi_ble_get_max_adv_data_len_command,
  .argument_list = { CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_get_max_no_of_supp_adv_sets_command(console_args_t *arguments);
static const char *_ble_get_max_no_of_supp_adv_sets_arg_help[] = {
  "max_no_of_supp_adv_sets",
};

static const console_descriptive_command_t _ble_get_max_no_of_supp_adv_sets_command = {
  .description   = "set no of adv_sets",
  .argument_help = _ble_get_max_no_of_supp_adv_sets_arg_help,
  .handler       = rsi_ble_get_max_no_of_supp_adv_sets_command,
  .argument_list = { CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_get_profile_async_command(console_args_t *arguments);
static const char *_ble_get_profile_async_arg_help[] = {
  "device address <xx:xx:xx:xx:xx:xx>",
  "uuid of service",
};

static const console_descriptive_command_t _ble_get_profile_async_command = {
  .description   = "BLE get gatt profile",
  .argument_help = _ble_get_profile_async_arg_help,
  .handler       = rsi_ble_get_profile_async_command,
  .argument_list = { CONSOLE_ARG_STRING, CONSOLE_ARG_INT, CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_get_profiles_async_command(console_args_t *arguments);
static const char *_ble_get_profiles_async_arg_help[] = {
  "device address <xx:xx:xx:xx:xx:xx>",
};

static const console_descriptive_command_t _ble_get_profiles_async_command = {
  .description   = "BLE get profiles as part of multi connection gatt",
  .argument_help = _ble_get_profiles_async_arg_help,
  .handler       = rsi_ble_get_profiles_async_command,
  .argument_list = { CONSOLE_ARG_STRING, CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_custom_notify_value_command(console_args_t *arguments);
static const char *_ble_notify_custom_value_arg_help[] = {
  "configure the data",
};

static const console_descriptive_command_t _ble_notify_custom_value_command = {
  .description   = "BLE notify with custom value",
  .argument_help = _ble_notify_custom_value_arg_help,
  .handler       = rsi_ble_custom_notify_value_command,
  .argument_list = { CONSOLE_ARG_STRING, CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_notify_value_command(console_args_t *arguments);
static const char *_ble_notify_value_arg_help[] = {};

static const console_descriptive_command_t _ble_notify_value_command = { .description   = "BLE notify the value",
                                                                         .argument_help = _ble_notify_value_arg_help,
                                                                         .handler       = rsi_ble_notify_value_command,
                                                                         .argument_list = { CONSOLE_ARG_END } };

extern sl_status_t rsi_ble_per_rx_command(console_args_t *arguments);
static const char *_ble_per_rx_arg_help[] = {
  "cmd_ix",         "receive_enable",  "access_addr", "phy_rate", "Multiple arguments bundled together",
  "ant_sel",        "pll_mode",        "rf_type",     "rf_chain", "ext_data_len_indication",
  "loop_back_mode", "duty_cycling_en",
};

static const console_descriptive_command_t _ble_per_rx_command = { .description =
                                                                     "configure the per receive parameters",
                                                                   .argument_help = _ble_per_rx_arg_help,
                                                                   .handler       = rsi_ble_per_rx_command,
                                                                   .argument_list = { CONSOLE_ARG_UINT,
                                                                                      CONSOLE_ARG_UINT,
                                                                                      CONSOLE_ARG_UINT,
                                                                                      CONSOLE_ARG_UINT,
                                                                                      CONSOLE_ARG_STRING,
                                                                                      CONSOLE_ARG_UINT,
                                                                                      CONSOLE_ARG_UINT,
                                                                                      CONSOLE_ARG_UINT,
                                                                                      CONSOLE_ARG_UINT,
                                                                                      CONSOLE_ARG_UINT,
                                                                                      CONSOLE_ARG_UINT,
                                                                                      CONSOLE_ARG_UINT,
                                                                                      CONSOLE_ARG_END } };

extern sl_status_t rsi_ble_per_tx_command(console_args_t *arguments);
static const char *_ble_per_tx_arg_help[] = {
  "cmd_ix",
  "transmit_enable",
  "access_addr",
  "phy_rate",
  "Multiple arguments bundled together",
  "Multiple arguments bundled together",
  "pkt_len",
  "payload_type",
  "tx_power",
  "transmit_mode",
  "inter_pkt_gap",
  "num_pkts",
};

static const console_descriptive_command_t _ble_per_tx_command = { .description =
                                                                     "configure the per transmit parameters",
                                                                   .argument_help = _ble_per_tx_arg_help,
                                                                   .handler       = rsi_ble_per_tx_command,
                                                                   .argument_list = { CONSOLE_ARG_UINT,
                                                                                      CONSOLE_ARG_UINT,
                                                                                      CONSOLE_ARG_UINT,
                                                                                      CONSOLE_ARG_UINT,
                                                                                      CONSOLE_ARG_STRING,
                                                                                      CONSOLE_ARG_STRING,
                                                                                      CONSOLE_ARG_UINT,
                                                                                      CONSOLE_ARG_UINT,
                                                                                      CONSOLE_ARG_UINT,
                                                                                      CONSOLE_ARG_UINT,
                                                                                      CONSOLE_ARG_UINT,
                                                                                      CONSOLE_ARG_UINT,
                                                                                      CONSOLE_ARG_END } };

extern sl_status_t rsi_ble_prepare_write_async_command(console_args_t *arguments);
static const char *_ble_prepare_write_async_arg_help[] = {
  "remote_dev_addr",
  "handle",
  "offset",
  "data len",
};

static const console_descriptive_command_t _ble_prepare_write_async_command = {
  .description   = "ble get long attribute value",
  .argument_help = _ble_prepare_write_async_arg_help,
  .handler       = rsi_ble_prepare_write_async_command,
  .argument_list = { CONSOLE_ARG_STRING, CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_read_phy_command(console_args_t *arguments);
static const char *_ble_read_phy_arg_help[] = {
  "device_addr",
};

static const console_descriptive_command_t _ble_read_phy_command = { .description   = "read ble phy",
                                                                     .argument_help = _ble_read_phy_arg_help,
                                                                     .handler       = rsi_ble_read_phy_command,
                                                                     .argument_list = { CONSOLE_ARG_STRING,
                                                                                        CONSOLE_ARG_END } };

extern sl_status_t rsi_ble_remove_gatt_attribute_command(console_args_t *arguments);
static const char *_ble_remove_gatt_att_arg_help[] = {
  "service_handler",
  "att_handle",
};

static const console_descriptive_command_t _ble_remove_gatt_att_command = {
  .description   = "remove gatt attribute value",
  .argument_help = _ble_remove_gatt_att_arg_help,
  .handler       = rsi_ble_remove_gatt_attribute_command,
  .argument_list = { CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_remove_gatt_service_command(console_args_t *arguments);
static const char *_ble_remove_gatt_service_arg_help[] = {
  "service_handler",
};

static const console_descriptive_command_t _ble_remove_gatt_service_command = {
  .description   = "remove gatt service",
  .argument_help = _ble_remove_gatt_service_arg_help,
  .handler       = rsi_ble_remove_gatt_service_command,
  .argument_list = { CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_remove_service_command(console_args_t *arguments);
static const char *_ble_remove_service_arg_help[] = {};

static const console_descriptive_command_t _ble_remove_service_command = { .description = "remove the service",
                                                                           .argument_help =
                                                                             _ble_remove_service_arg_help,
                                                                           .handler = rsi_ble_remove_service_command,
                                                                           .argument_list = { CONSOLE_ARG_END } };

extern sl_status_t rsi_ble_set_adv_data_command(console_args_t *arguments);
static const char *_ble_set_adv_data_arg_help[] = {
  "data",
  "data_length",
};

static const console_descriptive_command_t _ble_set_adv_data_command = {
  .description   = "set ble adv data",
  .argument_help = _ble_set_adv_data_arg_help,
  .handler       = rsi_ble_set_adv_data_command,
  .argument_list = { CONSOLE_ARG_STRING, CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_set_ae_data_command(console_args_t *arguments);
static const char *_ble_set_ae_data_arg_help[] = {
  "type",
  "adv_handle",
  "data_len",
};

static const console_descriptive_command_t _ble_set_ae_data_command = {
  .description   = "sets AE data",
  .argument_help = _ble_set_ae_data_arg_help,
  .handler       = rsi_ble_set_ae_data_command,
  .argument_list = { CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_set_ae_params_command(console_args_t *arguments);
static const char *_ble_set_ae_params_arg_help[] = {
  "adv_handle",
  "adv_event_prop",
  "primary_adv_intterval_min",
  "primary_adv_intterval_max",
  "primary_adv_chnl_map",
  "own_addr_type",
  "peer_addr_type",
  "remote_dev_addr",
  "adv_filter_policy",
  "adv_tx_power",
  "primary_adv_phy",
  "sec_adv_max_skip",
  "sec_adv_phy",
  "adv_sid",
  "scan_req_notify_enable",
};

static const console_descriptive_command_t _ble_set_ae_params_command = { .description   = "set ae scan params",
                                                                          .argument_help = _ble_set_ae_params_arg_help,
                                                                          .handler = rsi_ble_set_ae_params_command,
                                                                          .argument_list = { CONSOLE_ARG_UINT,
                                                                                             CONSOLE_ARG_UINT,
                                                                                             CONSOLE_ARG_UINT,
                                                                                             CONSOLE_ARG_UINT,
                                                                                             CONSOLE_ARG_UINT,
                                                                                             CONSOLE_ARG_UINT,
                                                                                             CONSOLE_ARG_UINT,
                                                                                             CONSOLE_ARG_STRING,
                                                                                             CONSOLE_ARG_UINT,
                                                                                             CONSOLE_ARG_UINT,
                                                                                             CONSOLE_ARG_UINT,
                                                                                             CONSOLE_ARG_STRING,
                                                                                             CONSOLE_ARG_UINT,
                                                                                             CONSOLE_ARG_STRING,
                                                                                             CONSOLE_ARG_UINT,
                                                                                             CONSOLE_ARG_END } };

extern sl_status_t rsi_ble_set_att_cmd_command(console_args_t *arguments);
static const char *_ble_set_att_cmd_arg_help[] = {
  "device address <xx:xx:xx:xx:xx:xx>",
  "handle",
  "data to set",
};

static const console_descriptive_command_t _ble_set_att_cmd_command = {
  .description   = "BLE set attribute cmd",
  .argument_help = _ble_set_att_cmd_arg_help,
  .handler       = rsi_ble_set_att_cmd_command,
  .argument_list = { CONSOLE_ARG_STRING, CONSOLE_ARG_INT, CONSOLE_ARG_INT, CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_set_att_value_async_command(console_args_t *arguments);
static const char *_ble_set_att_value_async_arg_help[] = {
  "device address <xx:xx:xx:xx:xx:xx>",
  "handle",
  "data to set",
};

static const console_descriptive_command_t _ble_set_att_value_async_command = {
  .description   = "BLE set attribute value",
  .argument_help = _ble_set_att_value_async_arg_help,
  .handler       = rsi_ble_set_att_value_async_command,
  .argument_list = { CONSOLE_ARG_STRING, CONSOLE_ARG_INT, CONSOLE_ARG_INT, CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_set_phy_command(console_args_t *arguments);
static const char *_ble_set_phy_arg_help[] = {
  "device_addr",
  "tx_phy",
  "rx_phy",
  "coded_phy",
};

static const console_descriptive_command_t _ble_set_phy_command = {
  .description   = "set ble phy",
  .argument_help = _ble_set_phy_arg_help,
  .handler       = rsi_ble_set_phy_command,
  .argument_list = { CONSOLE_ARG_STRING, CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_set_scan_response_data_command(console_args_t *arguments);
static const char *_ble_set_scan_response_data_arg_help[] = {
  "data",
};

static const console_descriptive_command_t _ble_set_scan_response_data_command = {
  .description   = "sets the scan response data",
  .argument_help = _ble_set_scan_response_data_arg_help,
  .handler       = rsi_ble_set_scan_response_data_command,
  .argument_list = { CONSOLE_ARG_STRING, CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_start_advertising_command(console_args_t *arguments);
static const char *_ble_start_advertising_arg_help[] = {
  "data",
  "ble adv type",
  "ble filter type",
  "ble addr type",
  "ble dir addr",
  "ble adv int min",
  "ble adv int max",
  "ble own addr type",
  "ble channel map",
  "ibeacon_info",
};

static const console_descriptive_command_t _ble_start_advertising_command = {
  .description   = "Start advertising with data",
  .argument_help = _ble_start_advertising_arg_help,
  .handler       = rsi_ble_start_advertising_command,
  .argument_list = { CONSOLE_ARG_STRING,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_STRING,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_start_ae_advertising_command(console_args_t *arguments);
static const char *_ble_start_ae_advertising_arg_help[] = {
  "enable",
  "adv_handle",
  "max_ae_events",
};

static const console_descriptive_command_t _ble_start_ae_advertising_command = {
  .description   = "starts AE advertising",
  .argument_help = _ble_start_ae_advertising_arg_help,
  .handler       = rsi_ble_start_ae_advertising_command,
  .argument_list = { CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_start_notify_value_command(console_args_t *arguments);
static const char *_ble_start_notify_value_arg_help[] = {
  "time_out_ms timeout between each notification",
  "count - no. of times notification should be issued. 0 for forever",
};

static const console_descriptive_command_t _ble_start_notify_value_command = {
  .description   = "BLE start notifying",
  .argument_help = _ble_start_notify_value_arg_help,
  .handler       = rsi_ble_start_notify_value_command,
  .argument_list = { CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_start_scanning_command(console_args_t *arguments);
static const char *_ble_start_scanning_arg_help[] = {
  "device address <xx:xx:xx:xx:xx:xx>",
  "scan type",
  "scan filter type",
  "scan interval",
  "scan window",
  "le public address",
};

static const console_descriptive_command_t _ble_start_scanning_command = { .description = "BLE start scanning",
                                                                           .argument_help =
                                                                             _ble_start_scanning_arg_help,
                                                                           .handler = rsi_ble_start_scanning_command,
                                                                           .argument_list = { CONSOLE_ARG_STRING,
                                                                                              CONSOLE_ARG_UINT,
                                                                                              CONSOLE_ARG_UINT,
                                                                                              CONSOLE_ARG_UINT,
                                                                                              CONSOLE_ARG_UINT,
                                                                                              CONSOLE_ARG_UINT,
                                                                                              CONSOLE_ARG_END } };

extern sl_status_t rsi_ble_stop_scanning_command(console_args_t *arguments);
static const char *_ble_stop_scanning_arg_help[] = {};

static const console_descriptive_command_t _ble_stop_scanning_command = { .description   = "BLE stop scanning",
                                                                          .argument_help = _ble_stop_scanning_arg_help,
                                                                          .handler = rsi_ble_stop_scanning_command,
                                                                          .argument_list = { CONSOLE_ARG_END } };

extern sl_status_t rsi_ble_update_conn_params_comamnd(console_args_t *arguments);
static const char *_ble_update_conn_params_arg_help[] = {
  "device address <xx:xx:xx:xx:xx:xx>", "min interval", "max interval", "latency", "timeout",
};

static const console_descriptive_command_t _ble_update_conn_params_command = {
  .description   = "update connection parameters",
  .argument_help = _ble_update_conn_params_arg_help,
  .handler       = rsi_ble_update_conn_params_comamnd,
  .argument_list = { CONSOLE_ARG_STRING,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_uuid_32bit_to_128bit_command(console_args_t *arguments);
static const char *_ble_uuid_32bit_to_128bit_arg_help[] = {};

static const console_descriptive_command_t _ble_uuid_32bit_to_128bit_command = {
  .description   = "used to convert 32 bit uuid to 128 bit",
  .argument_help = _ble_uuid_32bit_to_128bit_arg_help,
  .handler       = rsi_ble_uuid_32bit_to_128bit_command,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t wifi_bsd_socket_accept_handler(console_args_t *arguments);
static const char *_bsd_socket_accept_arg_help[] = {
  0,
  0,
};

static const console_descriptive_command_t _bsd_socket_accept_command = {
  .description   = "BSD socket listen",
  .argument_help = _bsd_socket_accept_arg_help,
  .handler       = wifi_bsd_socket_accept_handler,
  .argument_list = { CONSOLE_ARG_INT, CONSOLE_OPTIONAL_ARG('j', CONSOLE_ARG_INT), CONSOLE_ARG_END }
};

extern sl_status_t wifi_bsd_async_socket_close_handler(console_args_t *arguments);
static const char *_bsd_socket_async_close_arg_help[] = {
  0,
};

static const console_descriptive_command_t _bsd_socket_async_close_command = {
  .description   = "BSD socket close",
  .argument_help = _bsd_socket_async_close_arg_help,
  .handler       = wifi_bsd_async_socket_close_handler,
  .argument_list = { CONSOLE_ARG_INT, CONSOLE_ARG_END }
};

extern sl_status_t wifi_bsd_async_socket_create_handler(console_args_t *arguments);
static const char *_bsd_socket_async_create_arg_help[] = {
  0,
  0,
  0,
};

static const console_descriptive_command_t _bsd_socket_async_create_command = {
  .description   = "BSD socket creation",
  .argument_help = _bsd_socket_async_create_arg_help,
  .handler       = wifi_bsd_async_socket_create_handler,
  .argument_list = { CONSOLE_ENUM_ARG(bsd_socket_family),
                     CONSOLE_ENUM_ARG(bsd_socket_type),
                     CONSOLE_ENUM_ARG(bsd_socket_protocol),
                     CONSOLE_ARG_END }
};

extern sl_status_t async_event_handler(console_args_t *arguments);
static const char *_bsd_socket_async_event_arg_help[] = {
  0,
};

static const console_descriptive_command_t _bsd_socket_async_event_command = {
  .description   = "Async Event",
  .argument_help = _bsd_socket_async_event_arg_help,
  .handler       = async_event_handler,
  .argument_list = { CONSOLE_ARG_INT, CONSOLE_ARG_END }
};

extern sl_status_t wifi_bsd_async_select_handler(console_args_t *arguments);
static const char *_bsd_socket_async_select_arg_help[] = {
  0,
  0,
};

static const console_descriptive_command_t _bsd_socket_async_select_command = {
  .description   = "Async Select",
  .argument_help = _bsd_socket_async_select_arg_help,
  .handler       = wifi_bsd_async_select_handler,
  .argument_list = { CONSOLE_ARG_INT, CONSOLE_ARG_INT, CONSOLE_ARG_END }
};

extern sl_status_t wifi_bsd_socket_bind_handler(console_args_t *arguments);
static const char *_bsd_socket_bind_arg_help[] = {
  0, 0, 0, 0, 0,
};

static const console_descriptive_command_t _bsd_socket_bind_command = { .description   = "BSD socket bind",
                                                                        .argument_help = _bsd_socket_bind_arg_help,
                                                                        .handler       = wifi_bsd_socket_bind_handler,
                                                                        .argument_list = {
                                                                          CONSOLE_ARG_INT,
                                                                          CONSOLE_ARG_STRING,
                                                                          CONSOLE_ARG_UINT,
                                                                          CONSOLE_ARG_UINT,
                                                                          CONSOLE_OPTIONAL_ARG('j', CONSOLE_ARG_INT),
                                                                          CONSOLE_ARG_END } };

extern sl_status_t wifi_bsd_socket_close_handler(console_args_t *arguments);
static const char *_bsd_socket_close_arg_help[] = {
  0,
};

static const console_descriptive_command_t _bsd_socket_close_command = { .description   = "BSD socket close",
                                                                         .argument_help = _bsd_socket_close_arg_help,
                                                                         .handler       = wifi_bsd_socket_close_handler,
                                                                         .argument_list = { CONSOLE_ARG_INT,
                                                                                            CONSOLE_ARG_END } };

extern sl_status_t wifi_bsd_socket_connect_handler(console_args_t *arguments);
static const char *_bsd_socket_connect_arg_help[] = {
  0,
  0,
  0,
  0,
};

static const console_descriptive_command_t _bsd_socket_connect_command = {
  .description   = "BSD socket connect",
  .argument_help = _bsd_socket_connect_arg_help,
  .handler       = wifi_bsd_socket_connect_handler,
  .argument_list = { CONSOLE_ARG_INT, CONSOLE_ARG_STRING, CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t wifi_bsd_socket_create_handler(console_args_t *arguments);
static const char *_bsd_socket_create_arg_help[] = {
  0,
  0,
  0,
};

static const console_descriptive_command_t _bsd_socket_create_command = { .description   = "BSD socket creation",
                                                                          .argument_help = _bsd_socket_create_arg_help,
                                                                          .handler = wifi_bsd_socket_create_handler,
                                                                          .argument_list = {
                                                                            CONSOLE_ENUM_ARG(bsd_socket_family),
                                                                            CONSOLE_ENUM_ARG(bsd_socket_type),
                                                                            CONSOLE_ENUM_ARG(bsd_socket_protocol),
                                                                            CONSOLE_ARG_END } };

extern sl_status_t wifi_bsd_get_host_by_name_handler(console_args_t *arguments);
static const char *_bsd_socket_get_host_by_name_arg_help[] = {
  0,
};

static const console_descriptive_command_t _bsd_socket_get_host_by_name_command = {
  .description   = "BSD socket listen",
  .argument_help = _bsd_socket_get_host_by_name_arg_help,
  .handler       = wifi_bsd_get_host_by_name_handler,
  .argument_list = { CONSOLE_ARG_STRING, CONSOLE_ARG_END }
};

extern sl_status_t wifi_bsd_socket_get_opt_handler(console_args_t *arguments);
static const char *_bsd_socket_get_opt_arg_help[] = {
  0,
  0,
};

static const console_descriptive_command_t _bsd_socket_get_opt_command = {
  .description   = "BSD get the socket options",
  .argument_help = _bsd_socket_get_opt_arg_help,
  .handler       = wifi_bsd_socket_get_opt_handler,
  .argument_list = { CONSOLE_ARG_INT, CONSOLE_ENUM_ARG(option_name), CONSOLE_ARG_END }
};

extern sl_status_t wifi_bsd_get_peer_name(console_args_t *arguments);
static const char *_bsd_socket_get_peer_name_arg_help[] = {
  0,
};

static const console_descriptive_command_t _bsd_socket_get_peer_name_command = {
  .description   = "BSD get remote node address",
  .argument_help = _bsd_socket_get_peer_name_arg_help,
  .handler       = wifi_bsd_get_peer_name,
  .argument_list = { CONSOLE_ARG_INT, CONSOLE_ARG_END }
};

extern sl_status_t wifi_bsd_get_sock_name(console_args_t *arguments);
static const char *_bsd_socket_get_socket_name_arg_help[] = {
  0,
};

static const console_descriptive_command_t _bsd_socket_get_socket_name_command = {
  .description   = "BSD get local address of socket",
  .argument_help = _bsd_socket_get_socket_name_arg_help,
  .handler       = wifi_bsd_get_sock_name,
  .argument_list = { CONSOLE_ARG_INT, CONSOLE_ARG_END }
};

extern sl_status_t wifi_bsd_socket_listen_handler(console_args_t *arguments);
static const char *_bsd_socket_listen_arg_help[] = {
  0,
  0,
  0,
};

static const console_descriptive_command_t _bsd_socket_listen_command = {
  .description   = "BSD socket listen",
  .argument_help = _bsd_socket_listen_arg_help,
  .handler       = wifi_bsd_socket_listen_handler,
  .argument_list = { CONSOLE_ARG_INT, CONSOLE_ARG_INT, CONSOLE_OPTIONAL_ARG('j', CONSOLE_ARG_INT), CONSOLE_ARG_END }
};

extern sl_status_t wifi_bsd_socket_receive_handler(console_args_t *arguments);
static const char *_bsd_socket_receive_arg_help[] = {
  0,
};

static const console_descriptive_command_t _bsd_socket_receive_command = {
  .description   = "BSD socket receive from",
  .argument_help = _bsd_socket_receive_arg_help,
  .handler       = wifi_bsd_socket_receive_handler,
  .argument_list = { CONSOLE_ARG_INT, CONSOLE_ARG_END }
};

extern sl_status_t wifi_bsd_socket_receive_from_handler(console_args_t *arguments);
static const char *_bsd_socket_receive_from_arg_help[] = {
  0,
};

static const console_descriptive_command_t _bsd_socket_receive_from_command = {
  .description   = "BSD socket receive from",
  .argument_help = _bsd_socket_receive_from_arg_help,
  .handler       = wifi_bsd_socket_receive_from_handler,
  .argument_list = { CONSOLE_ARG_INT, CONSOLE_ARG_END }
};

extern sl_status_t wifi_bsd_select_handler(console_args_t *arguments);
static const char *_bsd_socket_select_arg_help[] = {
  0,
  0,
};

static const console_descriptive_command_t _bsd_socket_select_command = {
  .description   = "Select",
  .argument_help = _bsd_socket_select_arg_help,
  .handler       = wifi_bsd_select_handler,
  .argument_list = { CONSOLE_ARG_INT, CONSOLE_ARG_INT, CONSOLE_ARG_END }
};

extern sl_status_t wifi_bsd_socket_send_handler(console_args_t *arguments);
static const char *_bsd_socket_send_arg_help[] = {
  0,
  0,
};

static const console_descriptive_command_t _bsd_socket_send_command = {
  .description   = "BSD socket send",
  .argument_help = _bsd_socket_send_arg_help,
  .handler       = wifi_bsd_socket_send_handler,
  .argument_list = { CONSOLE_ARG_INT, CONSOLE_ARG_STRING, CONSOLE_ARG_END }
};

extern sl_status_t wifi_bsd_socket_send_to_handler(console_args_t *arguments);
static const char *_bsd_socket_send_to_arg_help[] = {
  0, 0, 0, 0, 0,
};

static const console_descriptive_command_t _bsd_socket_send_to_command = { .description = "BSD socket send",
                                                                           .argument_help =
                                                                             _bsd_socket_send_to_arg_help,
                                                                           .handler = wifi_bsd_socket_send_to_handler,
                                                                           .argument_list = { CONSOLE_ARG_INT,
                                                                                              CONSOLE_ARG_STRING,
                                                                                              CONSOLE_ARG_STRING,
                                                                                              CONSOLE_ARG_UINT,
                                                                                              CONSOLE_ARG_UINT,
                                                                                              CONSOLE_ARG_END } };

extern sl_status_t wifi_si91x_set_custom_sync_sockopt(console_args_t *arguments);
static const char *_bsd_socket_set_custom_sock_opt_arg_help[] = {
  0,
  0,
  0,
  0,
};

static const console_descriptive_command_t _bsd_socket_set_custom_sock_opt_command = {
  .description   = "Set custom sockopt",
  .argument_help = _bsd_socket_set_custom_sock_opt_arg_help,
  .handler       = wifi_si91x_set_custom_sync_sockopt,
  .argument_list = { CONSOLE_ARG_INT,
                     CONSOLE_ENUM_ARG(option_level),
                     CONSOLE_ENUM_ARG(option_name),
                     CONSOLE_ARG_INT,
                     CONSOLE_ARG_END }
};

extern sl_status_t wifi_bsd_socket_set_opt_handler(console_args_t *arguments);
static const char *_bsd_socket_set_opt_arg_help[] = {
  0,
  0,
  0,
  0,
};

static const console_descriptive_command_t _bsd_socket_set_opt_command = {
  .description   = "BSD get the socket options",
  .argument_help = _bsd_socket_set_opt_arg_help,
  .handler       = wifi_bsd_socket_set_opt_handler,
  .argument_list = { CONSOLE_ARG_INT,
                     CONSOLE_ENUM_ARG(option_level),
                     CONSOLE_ENUM_ARG(option_name),
                     CONSOLE_ARG_STRING,
                     CONSOLE_ARG_END }
};

extern sl_status_t wifi_si91x_setsockopt_async(console_args_t *arguments);
static const char *_bsd_socket_set_sock_opt_async_arg_help[] = {
  0,
  0,
  0,
  0,
};

static const console_descriptive_command_t _bsd_socket_set_sock_opt_async_command = {
  .description   = "Set socket options",
  .argument_help = _bsd_socket_set_sock_opt_async_arg_help,
  .handler       = wifi_si91x_setsockopt_async,
  .argument_list = { CONSOLE_ARG_INT,
                     CONSOLE_ENUM_ARG(option_level),
                     CONSOLE_ENUM_ARG(option_name),
                     CONSOLE_ARG_INT,
                     CONSOLE_ARG_END }
};

const console_database_t _bsd_socket_command_database = { CONSOLE_DATABASE_ENTRIES(
  { "accept", &_bsd_socket_accept_command },
  { "async_close", &_bsd_socket_async_close_command },
  { "async_create", &_bsd_socket_async_create_command },
  { "async_event", &_bsd_socket_async_event_command },
  { "async_select", &_bsd_socket_async_select_command },
  { "bind", &_bsd_socket_bind_command },
  { "close", &_bsd_socket_close_command },
  { "connect", &_bsd_socket_connect_command },
  { "create", &_bsd_socket_create_command },
  { "get_host_by_name", &_bsd_socket_get_host_by_name_command },
  { "get_opt", &_bsd_socket_get_opt_command },
  { "get_peer_name", &_bsd_socket_get_peer_name_command },
  { "get_socket_name", &_bsd_socket_get_socket_name_command },
  { "listen", &_bsd_socket_listen_command },
  { "receive", &_bsd_socket_receive_command },
  { "receive_from", &_bsd_socket_receive_from_command },
  { "select", &_bsd_socket_select_command },
  { "send", &_bsd_socket_send_command },
  { "send_to", &_bsd_socket_send_to_command },
  { "set_custom_sock_opt", &_bsd_socket_set_custom_sock_opt_command },
  { "set_opt", &_bsd_socket_set_opt_command },
  { "set_sock_opt_async", &_bsd_socket_set_sock_opt_async_command }, ) };

static const console_descriptive_command_t _bsd_socket_command = { .description = "",
                                                                   0,
                                                                   .sub_command_database =
                                                                     &_bsd_socket_command_database,
                                                                   { CONSOLE_ARG_SUB_COMMAND, CONSOLE_ARG_END } };

extern sl_status_t rsi_bt_deinit_command_handler(console_args_t *arguments);
static const char *_bt_deinit_arg_help[] = {};

static const console_descriptive_command_t _bt_deinit_command = { .description   = "deinitializes BT",
                                                                  .argument_help = _bt_deinit_arg_help,
                                                                  .handler       = rsi_bt_deinit_command_handler,
                                                                  .argument_list = { CONSOLE_ARG_END } };

extern sl_status_t rsi_bt_get_local_device_address_command(console_args_t *arguments);
static const char *_bt_get_local_address_arg_help[] = {};

static const console_descriptive_command_t _bt_get_local_address_command = {
  .description   = "Get the local address",
  .argument_help = _bt_get_local_address_arg_help,
  .handler       = rsi_bt_get_local_device_address_command,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t rsi_bt_get_local_name_command(console_args_t *arguments);
static const char *_bt_get_local_name_arg_help[] = {};

static const console_descriptive_command_t _bt_get_local_name_command = { .description   = "Get the local name",
                                                                          .argument_help = _bt_get_local_name_arg_help,
                                                                          .handler = rsi_bt_get_local_name_command,
                                                                          .argument_list = { CONSOLE_ARG_END } };

extern sl_status_t rsi_bt_power_save_profile_command(console_args_t *arguments);
static const char *_bt_power_save_profile_arg_help[] = {
  "mode",
  "type",
};

static const console_descriptive_command_t _bt_power_save_profile_command = {
  .description   = "Selects the power save profile mode for BT / BLE",
  .argument_help = _bt_power_save_profile_arg_help,
  .handler       = rsi_bt_power_save_profile_command,
  .argument_list = { CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t rsi_bt_set_local_name_command(console_args_t *arguments);
static const char *_bt_set_local_name_arg_help[] = {
  "name",
};

static const console_descriptive_command_t _bt_set_local_name_command = { .description   = "Set the local name",
                                                                          .argument_help = _bt_set_local_name_arg_help,
                                                                          .handler = rsi_bt_set_local_name_command,
                                                                          .argument_list = { CONSOLE_ARG_STRING,
                                                                                             CONSOLE_ARG_END } };

extern sl_status_t rsi_ble_stop_advertising_command(console_args_t *arguments);
static const char *_bt_stop_advertising_arg_help[] = {};

static const console_descriptive_command_t _bt_stop_advertising_command = { .description = "Stop advertising",
                                                                            .argument_help =
                                                                              _bt_stop_advertising_arg_help,
                                                                            .handler = rsi_ble_stop_advertising_command,
                                                                            .argument_list = { CONSOLE_ARG_END } };

extern sl_status_t sl_dns_hostgetbyname_command_handler(console_args_t *arguments);
static const char *_dns_get_host_by_name_arg_help[] = {
  0,
  0,
  0,
};

static const console_descriptive_command_t _dns_get_host_by_name_command = {
  .description   = "DNS resolution",
  .argument_help = _dns_get_host_by_name_arg_help,
  .handler       = sl_dns_hostgetbyname_command_handler,
  .argument_list = { CONSOLE_ARG_STRING,
                     CONSOLE_OPTIONAL_ARG('i', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('j', CONSOLE_ENUM_ARG(sl_net_dns_resolution_ip_type_t)),
                     CONSOLE_ARG_END }
};

const console_database_t _dns_command_database = { CONSOLE_DATABASE_ENTRIES(
  { "get_host_by_name", &_dns_get_host_by_name_command }, ) };

static const console_descriptive_command_t _dns_command = { .description = "",
                                                            0,
                                                            .sub_command_database = &_dns_command_database,
                                                            { CONSOLE_ARG_SUB_COMMAND, CONSOLE_ARG_END } };

extern sl_status_t console_variable_get(console_args_t *arguments);
static const char *_get_arg_help[] = {
  0,
};

static const console_descriptive_command_t _get_command = { .description   = "Get",
                                                            .argument_help = _get_arg_help,
                                                            .handler       = console_variable_get,
                                                            .argument_list = { CONSOLE_ARG_STRING, CONSOLE_ARG_END } };

extern sl_status_t socket_data_receive_handler(console_args_t *arguments);
static const char *_get_socket_rx_stats_arg_help[] = {
  "socket_id", "data_length", "time_in_sec", "No_of_pkts", "sock_type",
};

static const console_descriptive_command_t _get_socket_rx_stats_command = { .description = "recv data and get stats",
                                                                            .argument_help =
                                                                              _get_socket_rx_stats_arg_help,
                                                                            .handler = socket_data_receive_handler,
                                                                            .argument_list = { CONSOLE_ARG_UINT,
                                                                                               CONSOLE_ARG_UINT,
                                                                                               CONSOLE_ARG_UINT,
                                                                                               CONSOLE_ARG_UINT,
                                                                                               CONSOLE_ARG_UINT,
                                                                                               CONSOLE_ARG_END } };

extern sl_status_t default_help_command_handler(console_args_t *arguments);
static const char *_help_arg_help[] = {
  0,
};

static const console_descriptive_command_t _help_command = { .description   = "Print help",
                                                             .argument_help = _help_arg_help,
                                                             .handler       = default_help_command_handler,
                                                             .argument_list = { CONSOLE_ARG_REMAINING_COMMAND_LINE,
                                                                                CONSOLE_ARG_END } };

extern sl_status_t host_reset_command(console_args_t *arguments);
static const char *_host_reset_arg_help[] = {};

static const console_descriptive_command_t _host_reset_command = { .description   = "reset host",
                                                                   .argument_help = _host_reset_arg_help,
                                                                   .handler       = host_reset_command,
                                                                   .argument_list = { CONSOLE_ARG_END } };

extern sl_status_t http_client_deinit_command_handler(console_args_t *arguments);
static const char *_http_client_deinit_arg_help[] = {};

static const console_descriptive_command_t _http_client_deinit_command = {
  .description   = "Deinitialize the HTTP Client",
  .argument_help = _http_client_deinit_arg_help,
  .handler       = http_client_deinit_command_handler,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t http_client_get_start_command_handler(console_args_t *arguments);
static const char *_http_client_get_arg_help[] = {
  0,
  0,
  0,
};

static const console_descriptive_command_t _http_client_get_command = {
  .description   = "Request Data from a resource",
  .argument_help = _http_client_get_arg_help,
  .handler       = http_client_get_start_command_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ARG_STRING),
                     CONSOLE_OPTIONAL_ARG('p', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('r', CONSOLE_ARG_STRING),
                     CONSOLE_ARG_END }
};

extern sl_status_t http_client_init_command_handler(console_args_t *arguments);
static const char *_http_client_init_arg_help[] = {
  0, 0, 0, 0, 0, 0, 0, 0,
};

static const console_descriptive_command_t _http_client_init_command = {
  .description   = "HTTP Client Init",
  .argument_help = _http_client_init_arg_help,
  .handler       = http_client_init_command_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ARG_STRING),
                     CONSOLE_OPTIONAL_ARG('j', CONSOLE_ARG_STRING),
                     CONSOLE_OPTIONAL_ARG('k', CONSOLE_ENUM_ARG(net_interface)),
                     CONSOLE_OPTIONAL_ARG('l', CONSOLE_ENUM_ARG(sl_ip_address_type_t)),
                     CONSOLE_OPTIONAL_ARG('m', CONSOLE_ENUM_ARG(http_client_version)),
                     CONSOLE_OPTIONAL_ARG('n', CONSOLE_ENUM_ARG(boolean)),
                     CONSOLE_OPTIONAL_ARG('o', CONSOLE_ENUM_ARG(http_client_tls_version)),
                     CONSOLE_OPTIONAL_ARG('p', CONSOLE_ARG_UINT),
                     CONSOLE_ARG_END }
};

extern sl_status_t http_client_post_start_command_handler(console_args_t *arguments);
static const char *_http_client_post_arg_help[] = {
  0, 0, 0, 0, 0,
};

static const console_descriptive_command_t _http_client_post_command = {
  .description   = "HTTP POST",
  .argument_help = _http_client_post_arg_help,
  .handler       = http_client_post_start_command_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ARG_STRING),
                     CONSOLE_OPTIONAL_ARG('p', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('r', CONSOLE_ARG_STRING),
                     CONSOLE_OPTIONAL_ARG('b', CONSOLE_ARG_STRING),
                     CONSOLE_OPTIONAL_ARG('l', CONSOLE_ARG_UINT),
                     CONSOLE_ARG_END }
};

extern sl_status_t http_client_put_start_command_handler(console_args_t *arguments);
static const char *_http_client_put_arg_help[] = {
  0,
  0,
  0,
  0,
};

static const console_descriptive_command_t _http_client_put_command = {
  .description   = "Replace or create a new resource",
  .argument_help = _http_client_put_arg_help,
  .handler       = http_client_put_start_command_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ARG_STRING),
                     CONSOLE_OPTIONAL_ARG('p', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('r', CONSOLE_ARG_STRING),
                     CONSOLE_OPTIONAL_ARG('b', CONSOLE_ARG_UINT),
                     CONSOLE_ARG_END }
};

const console_database_t _http_client_command_database = { CONSOLE_DATABASE_ENTRIES(
  { "deinit", &_http_client_deinit_command },
  { "get", &_http_client_get_command },
  { "init", &_http_client_init_command },
  { "post", &_http_client_post_command },
  { "put", &_http_client_put_command }, ) };

static const console_descriptive_command_t _http_client_command = { .description = "",
                                                                    0,
                                                                    .sub_command_database =
                                                                      &_http_client_command_database,
                                                                    { CONSOLE_ARG_SUB_COMMAND, CONSOLE_ARG_END } };

extern sl_status_t init_ble_command(console_args_t *arguments);
static const char *_init_ble_arg_help[] = {
  "capabilities type",
  "bond_info",
  "set_smp",
};

static const console_descriptive_command_t _init_ble_command = {
  .description   = "Initializes the BLE task",
  .argument_help = _init_ble_arg_help,
  .handler       = init_ble_command,
  .argument_list = { CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t init_ble_gatt_command(console_args_t *arguments);
static const char *_init_ble_gatt_arg_help[] = {
  "set_smp",
};

static const console_descriptive_command_t _init_ble_gatt_command = {
  .description   = "Initializes the BLE with GATT task",
  .argument_help = _init_ble_gatt_arg_help,
  .handler       = init_ble_gatt_command,
  .argument_list = { CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t wifi_iot_socket_accept_handler(console_args_t *arguments);
static const char *_iot_socket_accept_arg_help[] = {
  0,
  0,
};

static const console_descriptive_command_t _iot_socket_accept_command = {
  .description   = "IOT socket listen",
  .argument_help = _iot_socket_accept_arg_help,
  .handler       = wifi_iot_socket_accept_handler,
  .argument_list = { CONSOLE_ARG_INT, CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t wifi_iot_socket_bind_handler(console_args_t *arguments);
static const char *_iot_socket_bind_arg_help[] = {
  0,
  0,
  0,
  0,
};

static const console_descriptive_command_t _iot_socket_bind_command = { .description   = "IOT socket bind",
                                                                        .argument_help = _iot_socket_bind_arg_help,
                                                                        .handler       = wifi_iot_socket_bind_handler,
                                                                        .argument_list = {
                                                                          CONSOLE_ARG_INT,
                                                                          CONSOLE_ARG_IP_ADDRESS,
                                                                          CONSOLE_ARG_UINT,
                                                                          CONSOLE_OPTIONAL_ARG('l', CONSOLE_ARG_UINT),
                                                                          CONSOLE_ARG_END } };

extern sl_status_t wifi_iot_socket_close_handler(console_args_t *arguments);
static const char *_iot_socket_close_arg_help[] = {
  0,
};

static const console_descriptive_command_t _iot_socket_close_command = { .description   = "IOT closing socket",
                                                                         .argument_help = _iot_socket_close_arg_help,
                                                                         .handler       = wifi_iot_socket_close_handler,
                                                                         .argument_list = { CONSOLE_ARG_INT,
                                                                                            CONSOLE_ARG_END } };

extern sl_status_t wifi_iot_socket_connect_handler(console_args_t *arguments);
static const char *_iot_socket_connect_arg_help[] = {
  0,
  0,
  0,
  0,
};

static const console_descriptive_command_t _iot_socket_connect_command = {
  .description   = "IOT socket connect",
  .argument_help = _iot_socket_connect_arg_help,
  .handler       = wifi_iot_socket_connect_handler,
  .argument_list = { CONSOLE_ARG_INT,
                     CONSOLE_ARG_IP_ADDRESS,
                     CONSOLE_ARG_UINT,
                     CONSOLE_OPTIONAL_ARG('l', CONSOLE_ARG_UINT),
                     CONSOLE_ARG_END }
};

extern sl_status_t wifi_iot_socket_create_handler(console_args_t *arguments);
static const char *_iot_socket_create_arg_help[] = {
  0,
  0,
  0,
};

static const console_descriptive_command_t _iot_socket_create_command = {
  .description   = "IOT socket creation",
  .argument_help = _iot_socket_create_arg_help,
  .handler       = wifi_iot_socket_create_handler,
  .argument_list = { CONSOLE_ENUM_ARG(socket_type),
                     CONSOLE_ENUM_ARG(ip_protocol),
                     CONSOLE_OPTIONAL_ARG('j', CONSOLE_ENUM_ARG(ipv4_or_ipv6)),
                     CONSOLE_ARG_END }
};

extern sl_status_t wifi_iot_get_host_by_name_handler(console_args_t *arguments);
static const char *_iot_socket_get_host_by_name_arg_help[] = {
  0,
  0,
  0,
};

static const console_descriptive_command_t _iot_socket_get_host_by_name_command = {
  .description   = "IOT socket get host by name",
  .argument_help = _iot_socket_get_host_by_name_arg_help,
  .handler       = wifi_iot_get_host_by_name_handler,
  .argument_list = { CONSOLE_ARG_STRING,
                     CONSOLE_ENUM_ARG(ipv4_or_ipv6),
                     CONSOLE_OPTIONAL_ARG('l', CONSOLE_ARG_UINT),
                     CONSOLE_ARG_END }
};

extern sl_status_t wifi_iot_socket_get_opt_handler(console_args_t *arguments);
static const char *_iot_socket_get_opt_arg_help[] = {
  0,
  0,
};

static const console_descriptive_command_t _iot_socket_get_opt_command = {
  .description   = "IOT socket get opt",
  .argument_help = _iot_socket_get_opt_arg_help,
  .handler       = wifi_iot_socket_get_opt_handler,
  .argument_list = { CONSOLE_ARG_INT, CONSOLE_ENUM_ARG(get_option_id), CONSOLE_ARG_END }
};

extern sl_status_t wifi_iot_socket_get_peer_name_handler(console_args_t *arguments);
static const char *_iot_socket_get_peer_name_arg_help[] = {
  0,
};

static const console_descriptive_command_t _iot_socket_get_peer_name_command = {
  .description   = "IOT socket get peer name",
  .argument_help = _iot_socket_get_peer_name_arg_help,
  .handler       = wifi_iot_socket_get_peer_name_handler,
  .argument_list = { CONSOLE_ARG_INT, CONSOLE_ARG_END }
};

extern sl_status_t wifi_iot_socket_get_socket_name_handler(console_args_t *arguments);
static const char *_iot_socket_get_sock_name_arg_help[] = {
  0,
};

static const console_descriptive_command_t _iot_socket_get_sock_name_command = {
  .description   = "IOT socket get socket name",
  .argument_help = _iot_socket_get_sock_name_arg_help,
  .handler       = wifi_iot_socket_get_socket_name_handler,
  .argument_list = { CONSOLE_ARG_INT, CONSOLE_ARG_END }
};

extern sl_status_t wifi_iot_socket_listen_handler(console_args_t *arguments);
static const char *_iot_socket_listen_arg_help[] = {
  0,
  0,
};

static const console_descriptive_command_t _iot_socket_listen_command = {
  .description   = "IOT socket listen",
  .argument_help = _iot_socket_listen_arg_help,
  .handler       = wifi_iot_socket_listen_handler,
  .argument_list = { CONSOLE_ARG_INT, CONSOLE_ARG_INT, CONSOLE_ARG_END }
};

extern sl_status_t wifi_iot_socket_receive_handler(console_args_t *arguments);
static const char *_iot_socket_receive_arg_help[] = {
  0,
};

static const console_descriptive_command_t _iot_socket_receive_command = {
  .description   = "IOT socket receive",
  .argument_help = _iot_socket_receive_arg_help,
  .handler       = wifi_iot_socket_receive_handler,
  .argument_list = { CONSOLE_ARG_INT, CONSOLE_ARG_END }
};

extern sl_status_t wifi_iot_socket_receive_from_handler(console_args_t *arguments);
static const char *_iot_socket_receive_from_arg_help[] = {
  0,
};

static const console_descriptive_command_t _iot_socket_receive_from_command = {
  .description   = "IOT socket receive from",
  .argument_help = _iot_socket_receive_from_arg_help,
  .handler       = wifi_iot_socket_receive_from_handler,
  .argument_list = { CONSOLE_ARG_INT, CONSOLE_ARG_END }
};

extern sl_status_t wifi_iot_socket_send_handler(console_args_t *arguments);
static const char *_iot_socket_send_arg_help[] = {
  0,
  0,
};

static const console_descriptive_command_t _iot_socket_send_command = {
  .description   = "IOT socket receieve",
  .argument_help = _iot_socket_send_arg_help,
  .handler       = wifi_iot_socket_send_handler,
  .argument_list = { CONSOLE_ARG_INT, CONSOLE_ARG_STRING, CONSOLE_ARG_END }
};

extern sl_status_t wifi_iot_socket_send_to_handler(console_args_t *arguments);
static const char *_iot_socket_send_to_arg_help[] = {
  0, 0, 0, 0, 0,
};

static const console_descriptive_command_t _iot_socket_send_to_command = {
  .description   = "IOT socket send to",
  .argument_help = _iot_socket_send_to_arg_help,
  .handler       = wifi_iot_socket_send_to_handler,
  .argument_list = { CONSOLE_ARG_INT,
                     CONSOLE_ARG_STRING,
                     CONSOLE_ARG_IP_ADDRESS,
                     CONSOLE_ARG_UINT,
                     CONSOLE_OPTIONAL_ARG('l', CONSOLE_ARG_UINT),
                     CONSOLE_ARG_END }
};

extern sl_status_t wifi_iot_socket_set_opt_handler(console_args_t *arguments);
static const char *_iot_socket_set_opt_arg_help[] = {
  0,
  0,
  0,
};

static const console_descriptive_command_t _iot_socket_set_opt_command = {
  .description   = "IOT socket set opt",
  .argument_help = _iot_socket_set_opt_arg_help,
  .handler       = wifi_iot_socket_set_opt_handler,
  .argument_list = { CONSOLE_ARG_INT, CONSOLE_ENUM_ARG(set_option_id), CONSOLE_ARG_INT, CONSOLE_ARG_END }
};

const console_database_t _iot_socket_command_database = { CONSOLE_DATABASE_ENTRIES(
  { "accept", &_iot_socket_accept_command },
  { "bind", &_iot_socket_bind_command },
  { "close", &_iot_socket_close_command },
  { "connect", &_iot_socket_connect_command },
  { "create", &_iot_socket_create_command },
  { "get_host_by_name", &_iot_socket_get_host_by_name_command },
  { "get_opt", &_iot_socket_get_opt_command },
  { "get_peer_name", &_iot_socket_get_peer_name_command },
  { "get_sock_name", &_iot_socket_get_sock_name_command },
  { "listen", &_iot_socket_listen_command },
  { "receive", &_iot_socket_receive_command },
  { "receive_from", &_iot_socket_receive_from_command },
  { "send", &_iot_socket_send_command },
  { "send_to", &_iot_socket_send_to_command },
  { "set_opt", &_iot_socket_set_opt_command }, ) };

static const console_descriptive_command_t _iot_socket_command = { .description = "",
                                                                   0,
                                                                   .sub_command_database =
                                                                     &_iot_socket_command_database,
                                                                   { CONSOLE_ARG_SUB_COMMAND, CONSOLE_ARG_END } };

extern sl_status_t iperf_command(console_args_t *arguments);
static const char *_iperf_arg_help[] = {
  "protocol_family", "type",     "protocol", "local_port",      "local_ip", "remote_ip",
  "remote_port",     "data_len", "ttl_sec",  "throughput_type", "qos",      "async",
};

static const console_descriptive_command_t _iperf_command = { .description   = "send to iperf server",
                                                              .argument_help = _iperf_arg_help,
                                                              .handler       = iperf_command,
                                                              .argument_list = { CONSOLE_ARG_UINT,
                                                                                 CONSOLE_ARG_UINT,
                                                                                 CONSOLE_ARG_UINT,
                                                                                 CONSOLE_ARG_UINT,
                                                                                 CONSOLE_ARG_STRING,
                                                                                 CONSOLE_ARG_STRING,
                                                                                 CONSOLE_ARG_UINT,
                                                                                 CONSOLE_ARG_UINT,
                                                                                 CONSOLE_ARG_UINT,
                                                                                 CONSOLE_ARG_UINT,
                                                                                 CONSOLE_ARG_UINT,
                                                                                 CONSOLE_ARG_UINT,
                                                                                 CONSOLE_ARG_END } };

extern sl_status_t console_variable_list(console_args_t *arguments);
static const char *_list_arg_help[] = {};

static const console_descriptive_command_t _list_command = { .description   = "List variables",
                                                             .argument_help = _list_arg_help,
                                                             .handler       = console_variable_list,
                                                             .argument_list = { CONSOLE_ARG_END } };

extern sl_status_t m4_powersave_command_handler(console_args_t *arguments);
static const char *_m4_powersave_arg_help[] = {};

static const console_descriptive_command_t _m4_powersave_command = { .description   = "Set m4 powersave ",
                                                                     .argument_help = _m4_powersave_arg_help,
                                                                     .handler       = m4_powersave_command_handler,
                                                                     .argument_list = { CONSOLE_ARG_END } };

extern sl_status_t m4_ta_secure_handshake_command_handler(console_args_t *arguments);
static const char *_m4_ta_secure_handshake_arg_help[] = {
  0,
  0,
};

static const console_descriptive_command_t _m4_ta_secure_handshake_command = {
  .description   = "Initializes m4-ta secure handshake",
  .argument_help = _m4_ta_secure_handshake_arg_help,
  .handler       = m4_ta_secure_handshake_command_handler,
  .argument_list = { CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t mallinfo_command_handler(console_args_t *arguments);
static const char *_mallinfo_arg_help[] = {};

static const console_descriptive_command_t _mallinfo_command = { .description   = "Print mallinfo",
                                                                 .argument_help = _mallinfo_arg_help,
                                                                 .handler       = mallinfo_command_handler,
                                                                 .argument_list = { CONSOLE_ARG_END } };

extern sl_status_t mem_alloc_command_handler(console_args_t *arguments);
static const char *_mem_alloc_arg_help[] = {
  "size",
};

static const console_descriptive_command_t _mem_alloc_command = { .description   = "Malloc",
                                                                  .argument_help = _mem_alloc_arg_help,
                                                                  .handler       = mem_alloc_command_handler,
                                                                  .argument_list = { CONSOLE_ARG_UINT,
                                                                                     CONSOLE_ARG_END } };

extern sl_status_t mem_free_command_handler(console_args_t *arguments);
static const char *_mem_free_arg_help[] = {
  "address",
};

static const console_descriptive_command_t _mem_free_command = { .description   = "Free",
                                                                 .argument_help = _mem_free_arg_help,
                                                                 .handler       = mem_free_command_handler,
                                                                 .argument_list = { CONSOLE_ARG_HEX,
                                                                                    CONSOLE_ARG_END } };

extern sl_status_t mem_write_command_handler(console_args_t *arguments);
static const char *_mem_write_arg_help[] = {
  "address",
  "size",
};

static const console_descriptive_command_t _mem_write_command = {
  .description   = "Write to memory",
  .argument_help = _mem_write_arg_help,
  .handler       = mem_write_command_handler,
  .argument_list = { CONSOLE_ARG_HEX, CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

const console_database_t _mem_command_database = { CONSOLE_DATABASE_ENTRIES({ "alloc", &_mem_alloc_command },
                                                                            { "free", &_mem_free_command },
                                                                            { "write", &_mem_write_command }, ) };

static const console_descriptive_command_t _mem_command = { .description = "",
                                                            0,
                                                            .sub_command_database = &_mem_command_database,
                                                            { CONSOLE_ARG_SUB_COMMAND, CONSOLE_ARG_END } };

extern sl_status_t mqtt_client_connect_to_server_command_handler(console_args_t *arguments);
static const char *_mqtt_connect_arg_help[] = {
  0, 0, 0, 0, 0,
};

static const console_descriptive_command_t _mqtt_connect_command = {
  .description   = "Start MQTT Init",
  .argument_help = _mqtt_connect_arg_help,
  .handler       = mqtt_client_connect_to_server_command_handler,
  .argument_list = { CONSOLE_ARG_STRING,
                     CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(boolean)),
                     CONSOLE_OPTIONAL_ARG('j', CONSOLE_ARG_STRING),
                     CONSOLE_OPTIONAL_ARG('k', CONSOLE_ARG_STRING),
                     CONSOLE_OPTIONAL_ARG('l', CONSOLE_ARG_UINT),
                     CONSOLE_ARG_END }
};

extern sl_status_t mqtt_client_deinit_command_handler(console_args_t *arguments);
static const char *_mqtt_deinit_arg_help[] = {};

static const console_descriptive_command_t _mqtt_deinit_command = { .description   = "Start MQTT deinit",
                                                                    .argument_help = _mqtt_deinit_arg_help,
                                                                    .handler       = mqtt_client_deinit_command_handler,
                                                                    .argument_list = { CONSOLE_ARG_END } };

extern sl_status_t mqtt_client_disconnect_command_handler(console_args_t *arguments);
static const char *_mqtt_disconnect_arg_help[] = {
  0,
};

static const console_descriptive_command_t _mqtt_disconnect_command = {
  .description   = "Disconnect from the MQTT broker",
  .argument_help = _mqtt_disconnect_arg_help,
  .handler       = mqtt_client_disconnect_command_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('e', CONSOLE_ARG_UINT), CONSOLE_ARG_END }
};

extern sl_status_t mqtt_client_publish_command_handler(console_args_t *arguments);
static const char *_mqtt_publish_arg_help[] = {
  0, 0, 0, 0, 0, 0, 0, 0,
};

static const console_descriptive_command_t _mqtt_publish_command = {
  .description   = "publish over a given topic",
  .argument_help = _mqtt_publish_arg_help,
  .handler       = mqtt_client_publish_command_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('m', CONSOLE_ENUM_ARG(mqtt_qos_levels)),
                     CONSOLE_OPTIONAL_ARG('n', CONSOLE_ENUM_ARG(boolean)),
                     CONSOLE_OPTIONAL_ARG('o', CONSOLE_ENUM_ARG(boolean)),
                     CONSOLE_OPTIONAL_ARG('p', CONSOLE_ARG_STRING),
                     CONSOLE_OPTIONAL_ARG('q', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('r', CONSOLE_ARG_HEX),
                     CONSOLE_OPTIONAL_ARG('s', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('t', CONSOLE_ARG_UINT),
                     CONSOLE_ARG_END }
};

extern sl_status_t sl_wifi_set_advanced_mqtt_configuration_command_handler(console_args_t *arguments);
static const char *_mqtt_set_advanced_client_configuration_arg_help[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

static const console_descriptive_command_t _mqtt_set_advanced_client_configuration_command = {
  .description   = "Set MQTT Client Configuration",
  .argument_help = _mqtt_set_advanced_client_configuration_arg_help,
  .handler       = sl_wifi_set_advanced_mqtt_configuration_command_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('j', CONSOLE_ENUM_ARG(boolean)),
                     CONSOLE_OPTIONAL_ARG('k', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('l', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('m', CONSOLE_ENUM_ARG(boolean)),
                     CONSOLE_OPTIONAL_ARG('n', CONSOLE_ARG_STRING),
                     CONSOLE_OPTIONAL_ARG('o', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('p', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('q', CONSOLE_ENUM_ARG(boolean)),
                     CONSOLE_OPTIONAL_ARG('r', CONSOLE_ENUM_ARG(mqtt_qos_levels)),
                     CONSOLE_OPTIONAL_ARG('s', CONSOLE_ARG_STRING),
                     CONSOLE_OPTIONAL_ARG('t', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('u', CONSOLE_ARG_STRING),
                     CONSOLE_OPTIONAL_ARG('v', CONSOLE_ARG_UINT),
                     CONSOLE_ARG_END }
};

extern sl_status_t mqtt_client_subscribe_command_handler(console_args_t *arguments);
static const char *_mqtt_subscribe_arg_help[] = {
  0,
  0,
  0,
  0,
};

static const console_descriptive_command_t _mqtt_subscribe_command = {
  .description   = "Subscribe to the MQTT Client",
  .argument_help = _mqtt_subscribe_arg_help,
  .handler       = mqtt_client_subscribe_command_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('a', CONSOLE_ARG_STRING),
                     CONSOLE_OPTIONAL_ARG('b', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('c', CONSOLE_ENUM_ARG(mqtt_qos_levels)),
                     CONSOLE_OPTIONAL_ARG('d', CONSOLE_ARG_UINT),
                     CONSOLE_ARG_END }
};

extern sl_status_t mqtt_client_unsubscribe_command_handler(console_args_t *arguments);
static const char *_mqtt_unsubscribe_arg_help[] = {
  0,
  0,
  0,
};

static const console_descriptive_command_t _mqtt_unsubscribe_command = {
  .description   = "unsubscribe to the given topic",
  .argument_help = _mqtt_unsubscribe_arg_help,
  .handler       = mqtt_client_unsubscribe_command_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('s', CONSOLE_ARG_STRING),
                     CONSOLE_OPTIONAL_ARG('u', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('p', CONSOLE_ARG_UINT),
                     CONSOLE_ARG_END }
};

const console_database_t _mqtt_command_database = { CONSOLE_DATABASE_ENTRIES(
  { "connect", &_mqtt_connect_command },
  { "deinit", &_mqtt_deinit_command },
  { "disconnect", &_mqtt_disconnect_command },
  { "publish", &_mqtt_publish_command },
  { "set_advanced_client_configuration", &_mqtt_set_advanced_client_configuration_command },
  { "subscribe", &_mqtt_subscribe_command },
  { "unsubscribe", &_mqtt_unsubscribe_command }, ) };

static const console_descriptive_command_t _mqtt_command = { .description = "",
                                                             0,
                                                             .sub_command_database = &_mqtt_command_database,
                                                             { CONSOLE_ARG_SUB_COMMAND, CONSOLE_ARG_END } };

extern sl_status_t configure_ip_command_handler(console_args_t *arguments);
static const char *_net_configure_ip_arg_help[] = {
  0, 0, 0, 0, 0,
};

static const console_descriptive_command_t _net_configure_ip_command = {
  .description   = "Start dhcp and get ip address",
  .argument_help = _net_configure_ip_arg_help,
  .handler       = configure_ip_command_handler,
  .argument_list = { CONSOLE_ENUM_ARG(sl_ip_address_type_t),
                     CONSOLE_OPTIONAL_ARG('i', CONSOLE_ARG_IP_ADDRESS),
                     CONSOLE_OPTIONAL_ARG('g', CONSOLE_ARG_IP_ADDRESS),
                     CONSOLE_OPTIONAL_ARG('n', CONSOLE_ARG_IP_ADDRESS),
                     CONSOLE_OPTIONAL_ARG('v', CONSOLE_ARG_UINT),
                     CONSOLE_ARG_END }
};

extern sl_status_t net_deinit_command_handler(console_args_t *arguments);
static const char *_net_deinit_arg_help[] = {
  0,
};

static const console_descriptive_command_t _net_deinit_command = { .description   = "Deinit the network interface",
                                                                   .argument_help = _net_deinit_arg_help,
                                                                   .handler       = net_deinit_command_handler,
                                                                   .argument_list = { CONSOLE_ENUM_ARG(net_interface),
                                                                                      CONSOLE_ARG_END } };

extern sl_status_t net_down_command_handler(console_args_t *arguments);
static const char *_net_down_arg_help[] = {
  0,
};

static const console_descriptive_command_t _net_down_command = { .description   = "Down the network interface",
                                                                 .argument_help = _net_down_arg_help,
                                                                 .handler       = net_down_command_handler,
                                                                 .argument_list = { CONSOLE_ENUM_ARG(net_interface),
                                                                                    CONSOLE_ARG_END } };

extern sl_status_t net_init_command_handler(console_args_t *arguments);
static const char *_net_init_arg_help[] = {
  0,
};

static const console_descriptive_command_t _net_init_command = { .description   = "Init the network interface",
                                                                 .argument_help = _net_init_arg_help,
                                                                 .handler       = net_init_command_handler,
                                                                 .argument_list = { CONSOLE_ENUM_ARG(net_interface),
                                                                                    CONSOLE_ARG_END } };

extern sl_status_t net_join_multicast_address_command_handler(console_args_t *arguments);
static const char *_net_join_multicast_address_arg_help[] = {
  0,
  0,
};

static const console_descriptive_command_t _net_join_multicast_address_command = {
  .description   = "Join multicast address",
  .argument_help = _net_join_multicast_address_arg_help,
  .handler       = net_join_multicast_address_command_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(net_interface)),
                     CONSOLE_OPTIONAL_ARG('p', CONSOLE_ARG_STRING),
                     CONSOLE_ARG_END }
};

extern sl_status_t net_leave_multicast_address_command_handler(console_args_t *arguments);
static const char *_net_leave_multicast_address_arg_help[] = {
  0,
  0,
};

static const console_descriptive_command_t _net_leave_multicast_address_command = {
  .description   = "Leave multicast address",
  .argument_help = _net_leave_multicast_address_arg_help,
  .handler       = net_leave_multicast_address_command_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(net_interface)),
                     CONSOLE_OPTIONAL_ARG('p', CONSOLE_ARG_STRING),
                     CONSOLE_ARG_END }
};

extern sl_status_t sl_net_ping_command_handler(console_args_t *arguments);
static const char *_net_ping_arg_help[] = {
  0,
  0,
  0,
};

static const console_descriptive_command_t _net_ping_command = {
  .description   = "Send ping request",
  .argument_help = _net_ping_arg_help,
  .handler       = sl_net_ping_command_handler,
  .argument_list = { CONSOLE_ENUM_ARG(sl_ip_address_type_t), CONSOLE_ARG_STRING, CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t net_up_command_handler(console_args_t *arguments);
static const char *_net_up_arg_help[] = {
  0,
  0,
};

static const console_descriptive_command_t _net_up_command = {
  .description   = "Up the network interface",
  .argument_help = _net_up_arg_help,
  .handler       = net_up_command_handler,
  .argument_list = { CONSOLE_ENUM_ARG(net_interface), CONSOLE_OPTIONAL_ARG('i', CONSOLE_ARG_UINT), CONSOLE_ARG_END }
};

const console_database_t _net_command_database = { CONSOLE_DATABASE_ENTRIES(
  { "configure_ip", &_net_configure_ip_command },
  { "deinit", &_net_deinit_command },
  { "down", &_net_down_command },
  { "init", &_net_init_command },
  { "join_multicast_address", &_net_join_multicast_address_command },
  { "leave_multicast_address", &_net_leave_multicast_address_command },
  { "ping", &_net_ping_command },
  { "up", &_net_up_command }, ) };

static const console_descriptive_command_t _net_command = { .description = "",
                                                            0,
                                                            .sub_command_database = &_net_command_database,
                                                            { CONSOLE_ARG_SUB_COMMAND, CONSOLE_ARG_END } };

extern sl_status_t set_nvm_profile_command_handler(console_args_t *arguments);
static const char *_nvm_set_profile_arg_help[] = {
  0,
  0,
};

static const console_descriptive_command_t _nvm_set_profile_command = {
  .description   = "set profile and credential in nvm",
  .argument_help = _nvm_set_profile_arg_help,
  .handler       = set_nvm_profile_command_handler,
  .argument_list = { CONSOLE_ENUM_ARG(net_interface), CONSOLE_OPTIONAL_ARG('i', CONSOLE_ARG_UINT), CONSOLE_ARG_END }
};

const console_database_t _nvm_command_database = { CONSOLE_DATABASE_ENTRIES(
  { "set_profile", &_nvm_set_profile_command }, ) };

static const console_descriptive_command_t _nvm_command = { .description = "",
                                                            0,
                                                            .sub_command_database = &_nvm_command_database,
                                                            { CONSOLE_ARG_SUB_COMMAND, CONSOLE_ARG_END } };

extern sl_status_t http_otaf_upgrade_command_handler(console_args_t *arguments);
static const char *_otaf_arg_help[] = {
  "flags",           "ip_address", "port",     "resource",  "host_name",
  "extended_header", "username",   "password", "post_data", "post_data_length",
};

static const console_descriptive_command_t _otaf_command = { .description   = "OTAF Upgrade",
                                                             .argument_help = _otaf_arg_help,
                                                             .handler       = http_otaf_upgrade_command_handler,
                                                             .argument_list = { CONSOLE_ARG_UINT,
                                                                                CONSOLE_ARG_STRING,
                                                                                CONSOLE_ARG_UINT,
                                                                                CONSOLE_ARG_STRING,
                                                                                CONSOLE_ARG_STRING,
                                                                                CONSOLE_ARG_STRING,
                                                                                CONSOLE_ARG_STRING,
                                                                                CONSOLE_ARG_STRING,
                                                                                CONSOLE_ARG_STRING,
                                                                                CONSOLE_ARG_UINT,
                                                                                CONSOLE_ARG_END } };

extern sl_status_t rsi_ble_read_max_data_len_command(console_args_t *arguments);
static const char *_read_max_data_length_arg_help[] = {};

static const console_descriptive_command_t _read_max_data_length_command = {
  .description   = "event reads the data length",
  .argument_help = _read_max_data_length_arg_help,
  .handler       = rsi_ble_read_max_data_len_command,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t sl_send_data_config_pkts(console_args_t *arguments);
static const char *_send_data_config_pkts_arg_help[] = {
  "socket_id", "data_length", "no_of_pkts", "time_in_sec", "server_ip", "server_port",
};

static const console_descriptive_command_t _send_data_config_pkts_command = {
  .description   = "send configure no of pkts",
  .argument_help = _send_data_config_pkts_arg_help,
  .handler       = sl_send_data_config_pkts,
  .argument_list = { CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_STRING,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_END }
};

extern sl_status_t console_variable_set(console_args_t *arguments);
static const char *_set_arg_help[] = {
  0,
  0,
};

static const console_descriptive_command_t _set_command = {
  .description   = "Set",
  .argument_help = _set_arg_help,
  .handler       = console_variable_set,
  .argument_list = { CONSOLE_ARG_STRING, CONSOLE_ARG_STRING, CONSOLE_ARG_END }
};

extern sl_status_t rsi_ble_set_ae_set_random_address_command(console_args_t *arguments);
static const char *_set_ae_set_random_address_arg_help[] = {
  "adv_handle",
  "random_address",
};

static const console_descriptive_command_t _set_ae_set_random_address_command = {
  .description   = "sets random address",
  .argument_help = _set_ae_set_random_address_arg_help,
  .handler       = rsi_ble_set_ae_set_random_address_command,
  .argument_list = { CONSOLE_ARG_UINT, CONSOLE_ARG_STRING, CONSOLE_ARG_END }
};

extern sl_status_t rsi_set_ble_duty_cycle_command(console_args_t *arguments);
static const char *_set_ble_duty_cycle_arg_help[] = {
  "set_duty_cycle",
};

static const console_descriptive_command_t _set_ble_duty_cycle_command = {
  .description   = "configure duty cycle",
  .argument_help = _set_ble_duty_cycle_arg_help,
  .handler       = rsi_set_ble_duty_cycle_command,
  .argument_list = { CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t set_ble_params_command(console_args_t *arguments);
static const char *_set_ble_params_arg_help[] = {
  "power value",
};

static const console_descriptive_command_t _set_ble_params_command = { .description   = "set ble params",
                                                                       .argument_help = _set_ble_params_arg_help,
                                                                       .handler       = set_ble_params_command,
                                                                       .argument_list = { CONSOLE_ARG_UINT,
                                                                                          CONSOLE_ARG_END } };

extern sl_status_t rsi_ble_set_data_len_command(console_args_t *arguments);
static const char *_set_data_length_arg_help[] = {
  "device_addr",
  "tx_len",
  "tx_time",
};

static const console_descriptive_command_t _set_data_length_command = {
  .description   = "set ble data length",
  .argument_help = _set_data_length_arg_help,
  .handler       = rsi_ble_set_data_len_command,
  .argument_list = { CONSOLE_ARG_STRING, CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t sl_net_join_multicast_address_handler(console_args_t *arguments);
static const char *_sl_net_join_multicast_address_arg_help[] = {
  0,
  0,
};

static const console_descriptive_command_t _sl_net_join_multicast_address_command = {
  .description   = "joins multicast group of provided IP address",
  .argument_help = _sl_net_join_multicast_address_arg_help,
  .handler       = sl_net_join_multicast_address_handler,
  .argument_list = { CONSOLE_ARG_INT, CONSOLE_ARG_STRING, CONSOLE_ARG_END }
};

extern sl_status_t sl_net_leave_multicast_address_handler(console_args_t *arguments);
static const char *_sl_net_leave_multicast_address_arg_help[] = {
  0,
  0,
};

static const console_descriptive_command_t _sl_net_leave_multicast_address_command = {
  .description   = "leaves multicast group of provided IP address",
  .argument_help = _sl_net_leave_multicast_address_arg_help,
  .handler       = sl_net_leave_multicast_address_handler,
  .argument_list = { CONSOLE_ARG_INT, CONSOLE_ARG_STRING, CONSOLE_ARG_END }
};

extern sl_status_t sl_wifi_assert_command_handler(console_args_t *arguments);
static const char *_sl_si91x_wireless_assert_arg_help[] = {};

static const console_descriptive_command_t _sl_si91x_wireless_assert_command = {
  .description   = "Trigger an assert",
  .argument_help = _sl_si91x_wireless_assert_arg_help,
  .handler       = sl_wifi_assert_command_handler,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t sl_si91x_get_ram_log_command_handler(console_args_t *arguments);
static const char *_sl_si91x_wireless_get_ram_log_arg_help[] = {
  0,
  0,
};

static const console_descriptive_command_t _sl_si91x_wireless_get_ram_log_command = {
  .description   = "read the mentioned lenght of data from frimware RAM",
  .argument_help = _sl_si91x_wireless_get_ram_log_arg_help,
  .handler       = sl_si91x_get_ram_log_command_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('a', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('l', CONSOLE_ARG_UINT),
                     CONSOLE_ARG_END }
};

const console_database_t _sl_si91x_wireless_command_database = { CONSOLE_DATABASE_ENTRIES(
  { "assert", &_sl_si91x_wireless_assert_command },
  { "get_ram_log", &_sl_si91x_wireless_get_ram_log_command }, ) };

static const console_descriptive_command_t _sl_si91x_wireless_command = {
  .description = "",
  0,
  .sub_command_database = &_sl_si91x_wireless_command_database,
  { CONSOLE_ARG_SUB_COMMAND, CONSOLE_ARG_END }
};

extern sl_status_t sntp_client_get_server_info(console_args_t *arguments);
static const char *_sntp_get_server_info_arg_help[] = {};

static const console_descriptive_command_t _sntp_get_server_info_command = { .description = "get server info",
                                                                             .argument_help =
                                                                               _sntp_get_server_info_arg_help,
                                                                             .handler = sntp_client_get_server_info,
                                                                             .argument_list = { CONSOLE_ARG_END } };

extern sl_status_t sntp_client_get_time_data_command_handler(console_args_t *arguments);
static const char *_sntp_get_time_data_arg_help[] = {};

static const console_descriptive_command_t _sntp_get_time_data_command = {
  .description   = "get time date",
  .argument_help = _sntp_get_time_data_arg_help,
  .handler       = sntp_client_get_time_data_command_handler,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t sntp_client_start_command_handler(console_args_t *arguments);
static const char *_sntp_start_arg_help[] = {
  0,
  0,
  0,
  0,
};

static const console_descriptive_command_t _sntp_start_command = { .description   = "start sntp client",
                                                                   .argument_help = _sntp_start_arg_help,
                                                                   .handler       = sntp_client_start_command_handler,
                                                                   .argument_list = {
                                                                     CONSOLE_ARG_STRING,
                                                                     CONSOLE_OPTIONAL_ARG('m', CONSOLE_ARG_UINT),
                                                                     CONSOLE_OPTIONAL_ARG('t', CONSOLE_ARG_UINT),
                                                                     CONSOLE_OPTIONAL_ARG('f', CONSOLE_ARG_UINT),
                                                                     CONSOLE_ARG_END } };

extern sl_status_t sntp_client_stop_command_handler(console_args_t *arguments);
static const char *_sntp_stop_arg_help[] = {};

static const console_descriptive_command_t _sntp_stop_command = { .description   = "stop sntp client",
                                                                  .argument_help = _sntp_stop_arg_help,
                                                                  .handler       = sntp_client_stop_command_handler,
                                                                  .argument_list = { CONSOLE_ARG_END } };

const console_database_t _sntp_command_database = { CONSOLE_DATABASE_ENTRIES(
  { "get_server_info", &_sntp_get_server_info_command },
  { "get_time_data", &_sntp_get_time_data_command },
  { "start", &_sntp_start_command },
  { "stop", &_sntp_stop_command }, ) };

static const console_descriptive_command_t _sntp_command = { .description = "",
                                                             0,
                                                             .sub_command_database = &_sntp_command_database,
                                                             { CONSOLE_ARG_SUB_COMMAND, CONSOLE_ARG_END } };

extern sl_status_t switch_host_frequency_handler(console_args_t *arguments);
static const char *_switch_m4_freq_arg_help[] = {};

static const console_descriptive_command_t _switch_m4_freq_command = { .description   = "switch m4 frequency",
                                                                       .argument_help = _switch_m4_freq_arg_help,
                                                                       .handler       = switch_host_frequency_handler,
                                                                       .argument_list = { CONSOLE_ARG_END } };

extern sl_status_t thread_command_handler(console_args_t *arguments);
static const char *_t_arg_help[] = {
  "index",
  "command",
};

static const console_descriptive_command_t _t_command = {
  .description   = "Run command in thread",
  .argument_help = _t_arg_help,
  .handler       = thread_command_handler,
  .argument_list = { CONSOLE_ARG_UINT, CONSOLE_ARG_REMAINING_COMMAND_LINE, CONSOLE_ARG_END }
};

extern sl_status_t thread_terminate_command_handler(console_args_t *arguments);
static const char *_terminate_arg_help[] = {
  "index",
};

static const console_descriptive_command_t _terminate_command = { .description   = "Terminate thread",
                                                                  .argument_help = _terminate_arg_help,
                                                                  .handler       = thread_terminate_command_handler,
                                                                  .argument_list = { CONSOLE_ARG_UINT,
                                                                                     CONSOLE_ARG_END } };

extern sl_status_t start_thread_command_handler(console_args_t *arguments);
static const char *_thread_arg_help[] = {
  "index",
  "stack size",
  "uart buffer size",
  "priority",
};

static const console_descriptive_command_t _thread_command = { .description   = "Start a thread",
                                                               .argument_help = _thread_arg_help,
                                                               .handler       = start_thread_command_handler,
                                                               .argument_list = {
                                                                 CONSOLE_ARG_UINT,
                                                                 CONSOLE_ARG_UINT,
                                                                 CONSOLE_ARG_UINT,
                                                                 CONSOLE_OPTIONAL_ARG('p', CONSOLE_ARG_UINT),
                                                                 CONSOLE_ARG_END } };

extern sl_status_t thread_info_command_handler(console_args_t *arguments);
static const char *_thread_info_arg_help[] = {};

static const console_descriptive_command_t _thread_info_command = { .description   = "Print thread information",
                                                                    .argument_help = _thread_info_arg_help,
                                                                    .handler       = thread_info_command_handler,
                                                                    .argument_list = { CONSOLE_ARG_END } };

extern sl_status_t get_rtc_timer_command_handler(console_args_t *arguments);
static const char *_time_get_arg_help[] = {};

static const console_descriptive_command_t _time_get_command = { .description   = "get real time counter",
                                                                 .argument_help = _time_get_arg_help,
                                                                 .handler       = get_rtc_timer_command_handler,
                                                                 .argument_list = { CONSOLE_ARG_END } };

extern sl_status_t set_rtc_timer_command_handler(console_args_t *arguments);
static const char *_time_set_arg_help[] = {
  0, 0, 0, 0, 0, 0, 0,
};

static const console_descriptive_command_t _time_set_command = { .description   = "set real time counter",
                                                                 .argument_help = _time_set_arg_help,
                                                                 .handler       = set_rtc_timer_command_handler,
                                                                 .argument_list = { CONSOLE_ARG_UINT,
                                                                                    CONSOLE_ARG_UINT,
                                                                                    CONSOLE_ARG_UINT,
                                                                                    CONSOLE_ARG_UINT,
                                                                                    CONSOLE_ARG_UINT,
                                                                                    CONSOLE_ARG_UINT,
                                                                                    CONSOLE_ARG_UINT,
                                                                                    CONSOLE_ARG_END } };

const console_database_t _time_command_database = { CONSOLE_DATABASE_ENTRIES({ "get", &_time_get_command },
                                                                             { "set", &_time_set_command }, ) };

static const console_descriptive_command_t _time_command = { .description = "",
                                                             0,
                                                             .sub_command_database = &_time_command_database,
                                                             { CONSOLE_ARG_SUB_COMMAND, CONSOLE_ARG_END } };

extern sl_status_t send_data_on_multiple_sockets(console_args_t *arguments);
static const char *_tx_data_on_multiple_sockets_arg_help[] = {
  "socket_list",
  "data_length",
  "time_in_sec",
  "No_of_pkts",
};

static const console_descriptive_command_t _tx_data_on_multiple_sockets_command = {
  .description   = "Send data on multiple sockets",
  .argument_help = _tx_data_on_multiple_sockets_arg_help,
  .handler       = send_data_on_multiple_sockets,
  .argument_list = { CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t wifi_disconnect_ap_client_command_handler(console_args_t *arguments);
static const char *_wifi_ap_disconnect_ap_client_arg_help[] = {
  0,
  0,
};

static const console_descriptive_command_t _wifi_ap_disconnect_ap_client_command = {
  .description   = "Disconnect client connected to AP",
  .argument_help = _wifi_ap_disconnect_ap_client_arg_help,
  .handler       = wifi_disconnect_ap_client_command_handler,
  .argument_list = { CONSOLE_ARG_MAC_ADDRESS,
                     CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(ap_client_deauth)),
                     CONSOLE_ARG_END }
};

extern sl_status_t wifi_get_ap_client_count_command_handler(console_args_t *arguments);
static const char *_wifi_ap_get_client_count_arg_help[] = {};

static const console_descriptive_command_t _wifi_ap_get_client_count_command = {
  .description   = "Client count for get ap",
  .argument_help = _wifi_ap_get_client_count_arg_help,
  .handler       = wifi_get_ap_client_count_command_handler,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t wifi_get_ap_client_info_command_handler(console_args_t *arguments);
static const char *_wifi_ap_get_client_info_arg_help[] = {};

static const console_descriptive_command_t _wifi_ap_get_client_info_command = {
  .description   = "Fetch the client info that are currently connected to AP",
  .argument_help = _wifi_ap_get_client_info_arg_help,
  .handler       = wifi_get_ap_client_info_command_handler,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t wifi_get_ap_client_list_command_handler(console_args_t *arguments);
static const char *_wifi_ap_get_client_list_arg_help[] = {};

static const console_descriptive_command_t _wifi_ap_get_client_list_command = {
  .description   = "Client list for ap",
  .argument_help = _wifi_ap_get_client_list_arg_help,
  .handler       = wifi_get_ap_client_list_command_handler,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t wifi_get_ap_configuration_command_handler(console_args_t *arguments);
static const char *_wifi_ap_get_configuration_arg_help[] = {
  0,
};

static const console_descriptive_command_t _wifi_ap_get_configuration_command = {
  .description   = "Get ap configuration",
  .argument_help = _wifi_ap_get_configuration_arg_help,
  .handler       = wifi_get_ap_configuration_command_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(wifi_interface)), CONSOLE_ARG_END }
};

extern sl_status_t wifi_set_ap_configuration_command_handler(console_args_t *arguments);
static const char *_wifi_ap_set_configuration_arg_help[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

static const console_descriptive_command_t _wifi_ap_set_configuration_command = {
  .description = "set ap configuration -s ssid -c channel -o option -i credential_id -t client_timeout -d becon_count "
                 "-m max_clients",
  .argument_help = _wifi_ap_set_configuration_arg_help,
  .handler       = wifi_set_ap_configuration_command_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('s', CONSOLE_ARG_STRING),
                     CONSOLE_OPTIONAL_ARG('p', CONSOLE_ENUM_ARG(wifi_security)),
                     CONSOLE_OPTIONAL_ARG('e', CONSOLE_ENUM_ARG(wifi_encryption)),
                     CONSOLE_OPTIONAL_ARG('c', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('r', CONSOLE_ENUM_ARG(rate_protocol)),
                     CONSOLE_OPTIONAL_ARG('o', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('i', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('t', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('d', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('m', CONSOLE_ARG_UINT),
                     CONSOLE_ARG_END }
};

extern sl_status_t wifi_start_ap_command_handler(console_args_t *arguments);
static const char *_wifi_ap_start_arg_help[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

static const console_descriptive_command_t _wifi_ap_start_command = {
  .description   = "Start in AP mode. Check sl_wifi_ap_configuration_t for parameters description",
  .argument_help = _wifi_ap_start_arg_help,
  .handler       = wifi_start_ap_command_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('a', CONSOLE_ARG_STRING),
                     CONSOLE_OPTIONAL_ARG('b', CONSOLE_ENUM_ARG(wifi_security)),
                     CONSOLE_OPTIONAL_ARG('c', CONSOLE_ENUM_ARG(wifi_encryption)),
                     CONSOLE_OPTIONAL_ARG('d', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('e', CONSOLE_ENUM_ARG(wifi_band)),
                     CONSOLE_OPTIONAL_ARG('f', CONSOLE_ENUM_ARG(wifi_bandwidth)),
                     CONSOLE_OPTIONAL_ARG('g', CONSOLE_ENUM_ARG(rate_protocol)),
                     CONSOLE_OPTIONAL_ARG('h', CONSOLE_ENUM_ARG(wifi_ap_flag)),
                     CONSOLE_OPTIONAL_ARG('i', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('j', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('k', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('l', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('m', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('n', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('p', CONSOLE_ARG_STRING),
                     CONSOLE_ARG_END }
};

extern sl_status_t wifi_start_wps(console_args_t *arguments);
static const char *_wifi_ap_start_wps_arg_help[] = {
  0,
  0,
  0,
};

static const console_descriptive_command_t _wifi_ap_start_wps_command = {
  .description   = "Enable WPS on access point",
  .argument_help = _wifi_ap_start_wps_arg_help,
  .handler       = wifi_start_wps,
  .argument_list = { CONSOLE_ENUM_ARG(wifi_interface),
                     CONSOLE_ENUM_ARG(wps_mode),
                     CONSOLE_OPTIONAL_ARG('p', CONSOLE_ARG_STRING),
                     CONSOLE_ARG_END }
};

const console_database_t _wifi_ap_command_database = { CONSOLE_DATABASE_ENTRIES(
  { "disconnect_ap_client", &_wifi_ap_disconnect_ap_client_command },
  { "get_client_count", &_wifi_ap_get_client_count_command },
  { "get_client_info", &_wifi_ap_get_client_info_command },
  { "get_client_list", &_wifi_ap_get_client_list_command },
  { "get_configuration", &_wifi_ap_get_configuration_command },
  { "set_configuration", &_wifi_ap_set_configuration_command },
  { "start", &_wifi_ap_start_command },
  { "start_wps", &_wifi_ap_start_wps_command }, ) };

static const console_descriptive_command_t _wifi_ap_command = { .description = "Wi-Fi AP commands",
                                                                0,
                                                                .sub_command_database = &_wifi_ap_command_database,
                                                                { CONSOLE_ARG_SUB_COMMAND, CONSOLE_ARG_END } };

extern sl_status_t wifi_clear_ca_store_handler(console_args_t *arguments);
static const char *_wifi_clear_ca_store_arg_help[] = {
  0,
  0,
  0,
};

static const console_descriptive_command_t _wifi_clear_ca_store_command = {
  .description   = "clear ca store",
  .argument_help = _wifi_clear_ca_store_arg_help,
  .handler       = wifi_clear_ca_store_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('c', CONSOLE_ENUM_ARG(cert_type)),
                     CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(credential_id)),
                     CONSOLE_OPTIONAL_ARG('n', CONSOLE_ARG_UINT),
                     CONSOLE_ARG_END }
};

extern sl_status_t wifi_clear_tls_certs_command_handler(console_args_t *arguments);
static const char *_wifi_clear_tls_certs_arg_help[] = {};

static const console_descriptive_command_t _wifi_clear_tls_certs_command = {
  .description   = "clear all certs",
  .argument_help = _wifi_clear_tls_certs_arg_help,
  .handler       = wifi_clear_tls_certs_command_handler,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t wifi_connect_command_handler(console_args_t *arguments);
static const char *_wifi_client_connect_arg_help[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

static const console_descriptive_command_t _wifi_client_connect_command = {
  .description   = "Start connecting to specified AP",
  .argument_help = _wifi_client_connect_arg_help,
  .handler       = wifi_connect_command_handler,
  .argument_list = { CONSOLE_ARG_STRING,
                     CONSOLE_ARG_STRING,
                     CONSOLE_OPTIONAL_ARG('s', CONSOLE_ENUM_ARG(wifi_security)),
                     CONSOLE_OPTIONAL_ARG('u', CONSOLE_ARG_STRING),
                     CONSOLE_OPTIONAL_ARG('e', CONSOLE_ENUM_ARG(wifi_encryption)),
                     CONSOLE_OPTIONAL_ARG('t', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('l', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('c', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('b', CONSOLE_ENUM_ARG(wifi_band)),
                     CONSOLE_OPTIONAL_ARG('w', CONSOLE_ENUM_ARG(wifi_bandwidth)),
                     CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(wifi_interface)),
                     CONSOLE_OPTIONAL_ARG('p', CONSOLE_ENUM_ARG(boolean)),
                     CONSOLE_ARG_END }
};

extern sl_status_t wifi_disconnect_command_handler(console_args_t *arguments);
static const char *_wifi_client_disconnect_arg_help[] = {
  0,
};

static const console_descriptive_command_t _wifi_client_disconnect_command = {
  .description   = "Disconnect",
  .argument_help = _wifi_client_disconnect_arg_help,
  .handler       = wifi_disconnect_command_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(wifi_interface)), CONSOLE_ARG_END }
};

extern sl_status_t wifi_set_advanced_client_config(console_args_t *arguments);
static const char *_wifi_client_set_advanced_config_arg_help[] = {
  0, 0, 0, 0, 0,
};

static const console_descriptive_command_t _wifi_client_set_advanced_config_command = {
  .description   = "Set advanced client config",
  .argument_help = _wifi_client_set_advanced_config_arg_help,
  .handler       = wifi_set_advanced_client_config,
  .argument_list = { CONSOLE_OPTIONAL_ARG('e', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('r', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('i', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('b', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('f', CONSOLE_ARG_UINT),
                     CONSOLE_ARG_END }
};

const console_database_t _wifi_client_command_database = { CONSOLE_DATABASE_ENTRIES(
  { "connect", &_wifi_client_connect_command },
  { "disconnect", &_wifi_client_disconnect_command },
  { "set_advanced_config", &_wifi_client_set_advanced_config_command }, ) };

static const console_descriptive_command_t _wifi_client_command = { .description = "Wi-Fi Client commands",
                                                                    0,
                                                                    .sub_command_database =
                                                                      &_wifi_client_command_database,
                                                                    { CONSOLE_ARG_SUB_COMMAND, CONSOLE_ARG_END } };

extern sl_status_t wifi_deinit_command_handler(console_args_t *arguments);
static const char *_wifi_deinit_arg_help[] = {};

static const console_descriptive_command_t _wifi_deinit_command = { .description   = "Deinit Wi-Fi interface",
                                                                    .argument_help = _wifi_deinit_arg_help,
                                                                    .handler       = wifi_deinit_command_handler,
                                                                    .argument_list = { CONSOLE_ARG_END } };

extern sl_status_t sl_wifi_disable_twt(console_args_t *arguments);
static const char *_wifi_disable_target_wake_time_arg_help[] = {};

static const console_descriptive_command_t _wifi_disable_target_wake_time_command = {
  .description   = "Configures TWT parameters. Disables a TWT session. This is blocking API",
  .argument_help = _wifi_disable_target_wake_time_arg_help,
  .handler       = sl_wifi_disable_twt,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t sl_wifi_enable_twt(console_args_t *arguments);
static const char *_wifi_enable_target_wake_time_arg_help[] = {};

static const console_descriptive_command_t _wifi_enable_target_wake_time_command = {
  .description   = "Configures TWT parameters. Enables a TWT session. This is blocking API",
  .argument_help = _wifi_enable_target_wake_time_arg_help,
  .handler       = sl_wifi_enable_twt,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t sl_wifi_filter_broadcast_command_handler(console_args_t *arguments);
static const char *_wifi_filter_broadcast_arg_help[] = {};

static const console_descriptive_command_t _wifi_filter_broadcast_command = {
  .description   = "control the amount of broadcast and multicast data that is transmitted",
  .argument_help = _wifi_filter_broadcast_arg_help,
  .handler       = sl_wifi_filter_broadcast_command_handler,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t wifi_generate_wps_pin_command_handler(console_args_t *arguments);
static const char *_wifi_generate_wps_pin_arg_help[] = {};

static const console_descriptive_command_t _wifi_generate_wps_pin_command = {
  .description   = "Generate 8 digit wps pin",
  .argument_help = _wifi_generate_wps_pin_arg_help,
  .handler       = wifi_generate_wps_pin_command_handler,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t sl_wifi_get_advanced_scan_configuration_command_handler(console_args_t *arguments);
static const char *_wifi_get_advanced_scan_configuration_arg_help[] = {};

static const console_descriptive_command_t _wifi_get_advanced_scan_configuration_command = {
  .description   = "Get advanced scan configuration",
  .argument_help = _wifi_get_advanced_scan_configuration_arg_help,
  .handler       = sl_wifi_get_advanced_scan_configuration_command_handler,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t wifi_get_channel_command_handler(console_args_t *arguments);
static const char *_wifi_get_channel_arg_help[] = {
  0,
};

static const console_descriptive_command_t _wifi_get_channel_command = {
  .description   = "Fetch channel info",
  .argument_help = _wifi_get_channel_arg_help,
  .handler       = wifi_get_channel_command_handler,
  .argument_list = { CONSOLE_ENUM_ARG(wifi_interface), CONSOLE_ARG_END }
};

extern sl_status_t wifi_get_default_interface_command_handler(console_args_t *arguments);
static const char *_wifi_get_default_interface_arg_help[] = {};

static const console_descriptive_command_t _wifi_get_default_interface_command = {
  .description   = "Fetch default interface",
  .argument_help = _wifi_get_default_interface_arg_help,
  .handler       = wifi_get_default_interface_command_handler,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t wifi_get_firmware_version_command_handler(console_args_t *arguments);
static const char *_wifi_get_fw_version_arg_help[] = {};

static const console_descriptive_command_t _wifi_get_fw_version_command = {
  .description   = "Fetch firmware version",
  .argument_help = _wifi_get_fw_version_arg_help,
  .handler       = wifi_get_firmware_version_command_handler,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t wifi_get_mac_address_command_handler(console_args_t *arguments);
static const char *_wifi_get_mac_address_arg_help[] = {
  0,
};

static const console_descriptive_command_t _wifi_get_mac_address_command = {
  .description   = "Fetch MAC address",
  .argument_help = _wifi_get_mac_address_arg_help,
  .handler       = wifi_get_mac_address_command_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ARG_UINT), CONSOLE_ARG_END }
};

extern sl_status_t wifi_get_performance_profile_command_handler(console_args_t *arguments);
static const char *_wifi_get_performance_profile_arg_help[] = {};

static const console_descriptive_command_t _wifi_get_performance_profile_command = {
  .description   = "Get the current performance profile of the module",
  .argument_help = _wifi_get_performance_profile_arg_help,
  .handler       = wifi_get_performance_profile_command_handler,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t wifi_get_signal_strength_command_handler(console_args_t *arguments);
static const char *_wifi_get_signal_strength_arg_help[] = {
  0,
};

static const console_descriptive_command_t _wifi_get_signal_strength_command = {
  .description   = "Queries signal strength",
  .argument_help = _wifi_get_signal_strength_arg_help,
  .handler       = wifi_get_signal_strength_command_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(wifi_interface)), CONSOLE_ARG_END }
};

extern sl_status_t wifi_get_statistics_command_handler(console_args_t *arguments);
static const char *_wifi_get_statistics_arg_help[] = {};

static const console_descriptive_command_t _wifi_get_statistics_command = {
  .description   = "Get wifi statistics",
  .argument_help = _wifi_get_statistics_arg_help,
  .handler       = wifi_get_statistics_command_handler,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t wifi_get_transmit_rate_command_handler(console_args_t *arguments);
static const char *_wifi_get_transmit_rate_arg_help[] = {};

static const console_descriptive_command_t _wifi_get_transmit_rate_command = {
  .description   = "Fetch configured rate protocol and data rate",
  .argument_help = _wifi_get_transmit_rate_arg_help,
  .handler       = wifi_get_transmit_rate_command_handler,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t wifi_get_tx_power_command_handler(console_args_t *arguments);
static const char *_wifi_get_tx_power_arg_help[] = {};

static const console_descriptive_command_t _wifi_get_tx_power_command = { .description   = "get module TX power",
                                                                          .argument_help = _wifi_get_tx_power_arg_help,
                                                                          .handler = wifi_get_tx_power_command_handler,
                                                                          .argument_list = { CONSOLE_ARG_END } };

extern sl_status_t wifi_init_command_handler(console_args_t *arguments);
static const char *_wifi_init_arg_help[] = {
  0,
};

static const console_descriptive_command_t _wifi_init_command = {
  .description   = "Init Wi-Fi interface",
  .argument_help = _wifi_init_arg_help,
  .handler       = wifi_init_command_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(wifi_init_mode)), CONSOLE_ARG_END }
};

extern sl_status_t wifi_is_interface_up_command_handler(console_args_t *arguments);
static const char *_wifi_is_interface_up_arg_help[] = {
  0,
};

static const console_descriptive_command_t _wifi_is_interface_up_command = {
  .description   = "Check if the specified interface is up",
  .argument_help = _wifi_is_interface_up_arg_help,
  .handler       = wifi_is_interface_up_command_handler,
  .argument_list = { CONSOLE_ENUM_ARG(wifi_interface), CONSOLE_ARG_END }
};

extern sl_status_t wifi_load_certificate_handler(console_args_t *arguments);
static const char *_wifi_load_cert_arg_help[] = {
  0,
  0,
  0,
  0,
};

static const console_descriptive_command_t _wifi_load_cert_command = {
  .description   = "load cert",
  .argument_help = _wifi_load_cert_arg_help,
  .handler       = wifi_load_certificate_handler,
  .argument_list = { CONSOLE_ARG_HEX,
                     CONSOLE_OPTIONAL_ARG('c', CONSOLE_ENUM_ARG(cert_type)),
                     CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(credential_id)),
                     CONSOLE_OPTIONAL_ARG('n', CONSOLE_ARG_UINT),
                     CONSOLE_ARG_END }
};

extern sl_status_t wifi_load_certificate_handler(console_args_t *arguments);
static const char *_wifi_load_certificate_arg_help[] = {
  0,
  0,
  0,
};

static const console_descriptive_command_t _wifi_load_certificate_command = {
  .description   = "Load/Clear Certificate from Flash",
  .argument_help = _wifi_load_certificate_arg_help,
  .handler       = wifi_load_certificate_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('l', CONSOLE_ARG_INT),
                     CONSOLE_OPTIONAL_ARG('i', CONSOLE_ARG_INT),
                     CONSOLE_OPTIONAL_ARG('t', CONSOLE_ARG_INT),
                     CONSOLE_ARG_END }
};

extern sl_status_t wifi_scan_command_handler(console_args_t *arguments);
static const char *_wifi_scan_arg_help[] = {
  0, 0, 0, 0, 0, 0, 0,
};

static const console_descriptive_command_t _wifi_scan_command = {
  .description   = "Start scanning",
  .argument_help = _wifi_scan_arg_help,
  .handler       = wifi_scan_command_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('s', CONSOLE_ARG_STRING),
                     CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(wifi_interface)),
                     CONSOLE_OPTIONAL_ARG('m', CONSOLE_ENUM_ARG(enable_bg_scan)),
                     CONSOLE_OPTIONAL_ARG('c', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('a', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('t', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('l', CONSOLE_ARG_UINT),
                     CONSOLE_ARG_END }
};

extern sl_status_t sl_wifi_send_raw_data_command_handler(console_args_t *arguments);
static const char *_wifi_send_raw_data_arg_help[] = {
  0,
};

static const console_descriptive_command_t _wifi_send_raw_data_command = {
  .description   = "send raw data frame",
  .argument_help = _wifi_send_raw_data_arg_help,
  .handler       = sl_wifi_send_raw_data_command_handler,
  .argument_list = { CONSOLE_ENUM_ARG(wifi_interface), CONSOLE_ARG_END }
};

extern sl_status_t sl_wifi_set_11ax_config_command_handler(console_args_t *arguments);
static const char *_wifi_set_11ax_config_arg_help[] = {
  0,
};

static const console_descriptive_command_t _wifi_set_11ax_config_command = {
  .description   = "Set the 11ax configuration",
  .argument_help = _wifi_set_11ax_config_arg_help,
  .handler       = sl_wifi_set_11ax_config_command_handler,
  .argument_list = { CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t sl_wifi_set_advanced_scan_configuration_command_handler(console_args_t *arguments);
static const char *_wifi_set_advanced_scan_configuration_arg_help[] = {
  0, 0, 0, 0, 0, 0,
};

static const console_descriptive_command_t _wifi_set_advanced_scan_configuration_command = {
  .description   = "Set advanced scan configuration",
  .argument_help = _wifi_set_advanced_scan_configuration_arg_help,
  .handler       = sl_wifi_set_advanced_scan_configuration_command_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('j', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('k', CONSOLE_ARG_INT),
                     CONSOLE_OPTIONAL_ARG('l', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('m', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('n', CONSOLE_ARG_UINT),
                     CONSOLE_ARG_END }
};

extern sl_status_t wifi_set_antenna_command_handler(console_args_t *arguments);
static const char *_wifi_set_antenna_arg_help[] = {
  0,
  0,
};

static const console_descriptive_command_t _wifi_set_antenna_command = {
  .description   = "Set Wi-Fi antenna",
  .argument_help = _wifi_set_antenna_arg_help,
  .handler       = wifi_set_antenna_command_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(wifi_interface)),
                     CONSOLE_OPTIONAL_ARG('a', CONSOLE_ARG_UINT),
                     CONSOLE_ARG_END }
};

extern sl_status_t wifi_set_channel_command_handler(console_args_t *arguments);
static const char *_wifi_set_channel_arg_help[] = {
  0,
  0,
};

static const console_descriptive_command_t _wifi_set_channel_command = {
  .description   = "Set channel number in AP interface",
  .argument_help = _wifi_set_channel_arg_help,
  .handler       = wifi_set_channel_command_handler,
  .argument_list = { CONSOLE_ENUM_ARG(wifi_interface), CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t set_wifi_config_handler(console_args_t *arguments);
static const char *_wifi_set_config_arg_help[] = {
  "oper_mode",
  "coex_mode",
  "feature_bit_map",
  "tcp_ip_feature_bit_map",
  "custom_feature_bit_map",
  "ext_custom_feature_bit_map",
  "bt_feature_bit_map",
  "ext_tcp_ip_feature_bit_map",
  "ble_feature_bit_map",
  "ble_ext_feature_bit_map",
  "config_feature_bit_map",
};

static const console_descriptive_command_t _wifi_set_config_command = { .description   = "Config Wi-Fi init",
                                                                        .argument_help = _wifi_set_config_arg_help,
                                                                        .handler       = set_wifi_config_handler,
                                                                        .argument_list = { CONSOLE_ARG_UINT,
                                                                                           CONSOLE_ARG_UINT,
                                                                                           CONSOLE_ARG_UINT,
                                                                                           CONSOLE_ARG_UINT,
                                                                                           CONSOLE_ARG_UINT,
                                                                                           CONSOLE_ARG_UINT,
                                                                                           CONSOLE_ARG_UINT,
                                                                                           CONSOLE_ARG_UINT,
                                                                                           CONSOLE_ARG_UINT,
                                                                                           CONSOLE_ARG_UINT,
                                                                                           CONSOLE_ARG_UINT,
                                                                                           CONSOLE_ARG_END } };

extern sl_status_t set_default_connection_stats(console_args_t *arguments);
static const char *_wifi_set_default_con_dis_stats_arg_help[] = {};

static const console_descriptive_command_t _wifi_set_default_con_dis_stats_command = {
  .description   = "set connection and disconnection stats to zero",
  .argument_help = _wifi_set_default_con_dis_stats_arg_help,
  .handler       = set_default_connection_stats,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t wifi_set_default_interface_command_handler(console_args_t *arguments);
static const char *_wifi_set_default_interface_arg_help[] = {
  0,
};

static const console_descriptive_command_t _wifi_set_default_interface_command = {
  .description   = "Set default interface",
  .argument_help = _wifi_set_default_interface_arg_help,
  .handler       = wifi_set_default_interface_command_handler,
  .argument_list = { CONSOLE_ENUM_ARG(wifi_interface), CONSOLE_ARG_END }
};

extern sl_status_t wifi_set_default_ssl_cipher(console_args_t *arguments);
static const char *_wifi_set_default_ssl_cipher_arg_help[] = {};

static const console_descriptive_command_t _wifi_set_default_ssl_cipher_command = {
  .description   = "set default ssl cipher",
  .argument_help = _wifi_set_default_ssl_cipher_arg_help,
  .handler       = wifi_set_default_ssl_cipher,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t set_eap_credentials_command_handler(console_args_t *arguments);
static const char *_wifi_set_eap_credentials_arg_help[] = {
  0,
  0,
  0,
};

static const console_descriptive_command_t _wifi_set_eap_credentials_command = {
  .description   = "set eap credentials",
  .argument_help = _wifi_set_eap_credentials_arg_help,
  .handler       = set_eap_credentials_command_handler,
  .argument_list = { CONSOLE_ARG_STRING, CONSOLE_ARG_STRING, CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t wifi_set_extended_default_ssl_cipher(console_args_t *arguments);
static const char *_wifi_set_extended_default_ssl_cipher_arg_help[] = {};

static const console_descriptive_command_t _wifi_set_extended_default_ssl_cipher_command = {
  .description   = "set extended default ssl cipher",
  .argument_help = _wifi_set_extended_default_ssl_cipher_arg_help,
  .handler       = wifi_set_extended_default_ssl_cipher,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t wifi_ssl_extended_cipher_select(console_args_t *arguments);
static const char *_wifi_set_extended_ssl_cipher_arg_help[] = {
  0,
};

static const console_descriptive_command_t _wifi_set_extended_ssl_cipher_command = {
  .description   = "select extended ssl cipher",
  .argument_help = _wifi_set_extended_ssl_cipher_arg_help,
  .handler       = wifi_ssl_extended_cipher_select,
  .argument_list = { CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t set_join_configuration_command_handler(console_args_t *arguments);
static const char *_wifi_set_join_feature_bitmap_arg_help[] = {
  0,
  0,
};

static const console_descriptive_command_t _wifi_set_join_feature_bitmap_command = {
  .description   = "Set Join Feature Bitmap",
  .argument_help = _wifi_set_join_feature_bitmap_arg_help,
  .handler       = set_join_configuration_command_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(wifi_interface)),
                     CONSOLE_OPTIONAL_ARG('j', CONSOLE_ARG_UINT),
                     CONSOLE_ARG_END }
};

extern sl_status_t set_listen_interval_command_handler(console_args_t *arguments);
static const char *_wifi_set_listen_interval_arg_help[] = {
  0,
};

static const console_descriptive_command_t _wifi_set_listen_interval_command = {
  .description   = "Set Listen Interval",
  .argument_help = _wifi_set_listen_interval_arg_help,
  .handler       = set_listen_interval_command_handler,
  .argument_list = { CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t wifi_set_performance_profile_command_handler(console_args_t *arguments);
static const char *_wifi_set_performance_profile_arg_help[] = {
  0, 0, 0, 0, 0,
};

static const console_descriptive_command_t _wifi_set_performance_profile_command = {
  .description   = "Set the performance profile of the module",
  .argument_help = _wifi_set_performance_profile_arg_help,
  .handler       = wifi_set_performance_profile_command_handler,
  .argument_list = { CONSOLE_ENUM_ARG(performance_mode),
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_UINT,
                     CONSOLE_ARG_END }
};

extern sl_status_t wifi_set_roam_configuration_command_handler(console_args_t *arguments);
static const char *_wifi_set_roam_configuration_arg_help[] = {
  0,
  0,
  0,
};

static const console_descriptive_command_t _wifi_set_roam_configuration_command = {
  .description   = "Set roaming configuration",
  .argument_help = _wifi_set_roam_configuration_arg_help,
  .handler       = wifi_set_roam_configuration_command_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(wifi_interface)),
                     CONSOLE_ARG_INT,
                     CONSOLE_ARG_INT,
                     CONSOLE_ARG_END }
};

extern sl_status_t wifi_ssl_cipher_select(console_args_t *arguments);
static const char *_wifi_set_ssl_cipher_arg_help[] = {
  0,
};

static const console_descriptive_command_t _wifi_set_ssl_cipher_command = {
  .description   = "select ssl cipher",
  .argument_help = _wifi_set_ssl_cipher_arg_help,
  .handler       = wifi_ssl_cipher_select,
  .argument_list = { CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

extern sl_status_t wifi_set_timeout_config(console_args_t *arguments);
static const char *_wifi_set_timeout_config_arg_help[] = {
  0,
  0,
  0,
};

static const console_descriptive_command_t _wifi_set_timeout_config_command = {
  .description   = "Set timeout config",
  .argument_help = _wifi_set_timeout_config_arg_help,
  .handler       = wifi_set_timeout_config,
  .argument_list = { CONSOLE_OPTIONAL_ARG('k', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('a', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('c', CONSOLE_ARG_UINT),
                     CONSOLE_ARG_END }
};

extern sl_status_t wifi_set_transmit_rate_command_handler(console_args_t *arguments);
static const char *_wifi_set_transmit_rate_arg_help[] = {
  0,
  0,
  0,
};

static const console_descriptive_command_t _wifi_set_transmit_rate_command = {
  .description   = "Set rate protocol and data rate",
  .argument_help = _wifi_set_transmit_rate_arg_help,
  .handler       = wifi_set_transmit_rate_command_handler,
  .argument_list = { CONSOLE_ENUM_ARG(rate_protocol),
                     CONSOLE_ENUM_ARG(data_rate),
                     CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(wifi_interface)),
                     CONSOLE_ARG_END }
};

extern sl_status_t wifi_set_tx_power_command_handler(console_args_t *arguments);
static const char *_wifi_set_tx_power_arg_help[] = {
  0,
  0,
};

static const console_descriptive_command_t _wifi_set_tx_power_command = { .description   = "Set module TX power",
                                                                          .argument_help = _wifi_set_tx_power_arg_help,
                                                                          .handler = wifi_set_tx_power_command_handler,
                                                                          .argument_list = {
                                                                            CONSOLE_OPTIONAL_ARG('s', CONSOLE_ARG_UINT),
                                                                            CONSOLE_OPTIONAL_ARG('j', CONSOLE_ARG_UINT),
                                                                            CONSOLE_ARG_END } };

extern sl_status_t set_region_command_handler(console_args_t *arguments);
static const char *_wifi_set_wifi_region_arg_help[] = {
  0,
};

static const console_descriptive_command_t _wifi_set_wifi_region_command = {
  .description   = "set Wi-fi region before init",
  .argument_help = _wifi_set_wifi_region_arg_help,
  .handler       = set_region_command_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(region_code)), CONSOLE_ARG_END }
};

extern sl_status_t sl_si91x_transmit_test_start_command_handler(console_args_t *arguments);
static const char *_wifi_sl_si91x_transmit_test_start_arg_help[] = {
  0, 0, 0, 0, 0, 0, 0,
};

static const console_descriptive_command_t _wifi_sl_si91x_transmit_test_start_command = {
  .description   = "si91x transmit test start",
  .argument_help = _wifi_sl_si91x_transmit_test_start_arg_help,
  .handler       = sl_si91x_transmit_test_start_command_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('j', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('k', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('l', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('m', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('n', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('o', CONSOLE_ARG_UINT),
                     CONSOLE_ARG_END }
};

extern sl_status_t sl_si91x_transmit_test_stop_command_handler(console_args_t *arguments);
static const char *_wifi_sl_si91x_transmit_test_stop_arg_help[] = {};

static const console_descriptive_command_t _wifi_sl_si91x_transmit_test_stop_command = {
  .description   = "si91x transmit test start",
  .argument_help = _wifi_sl_si91x_transmit_test_stop_arg_help,
  .handler       = sl_si91x_transmit_test_stop_command_handler,
  .argument_list = { CONSOLE_ARG_END }
};

extern sl_status_t sl_wifi_set_antenna_command_handler(console_args_t *arguments);
static const char *_wifi_sl_wifi_set_antenna_arg_help[] = {
  0,
  0,
};

static const console_descriptive_command_t _wifi_sl_wifi_set_antenna_command = {
  .description   = "Set wifi antenna config",
  .argument_help = _wifi_sl_wifi_set_antenna_arg_help,
  .handler       = sl_wifi_set_antenna_command_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('c', CONSOLE_ARG_UINT),
                     CONSOLE_ARG_END }
};

extern sl_status_t wifi_start_statistic_report_command_handler(console_args_t *arguments);
static const char *_wifi_start_statistic_report_arg_help[] = {
  0,
  0,
};

static const console_descriptive_command_t _wifi_start_statistic_report_command = {
  .description   = "Get wifi statistic report",
  .argument_help = _wifi_start_statistic_report_arg_help,
  .handler       = wifi_start_statistic_report_command_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(wifi_interface)),
                     CONSOLE_OPTIONAL_ARG('c', CONSOLE_ARG_UINT),
                     CONSOLE_ARG_END }
};

extern sl_status_t wifi_stop_ap_command_handler(console_args_t *arguments);
static const char *_wifi_stop_ap_interface_arg_help[] = {
  0,
};

static const console_descriptive_command_t _wifi_stop_ap_interface_command = {
  .description   = "Stop Wi-Fi interface",
  .argument_help = _wifi_stop_ap_interface_arg_help,
  .handler       = wifi_stop_ap_command_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(wifi_interface)), CONSOLE_ARG_END }
};

extern sl_status_t wifi_stop_scan_command_handler(console_args_t *arguments);
static const char *_wifi_stop_scan_arg_help[] = {
  0,
};

static const console_descriptive_command_t _wifi_stop_scan_command = {
  .description   = "Stop scanning",
  .argument_help = _wifi_stop_scan_arg_help,
  .handler       = wifi_stop_scan_command_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(wifi_interface)), CONSOLE_ARG_END }
};

extern sl_status_t wifi_stop_statistic_report_command_handler(console_args_t *arguments);
static const char *_wifi_stop_statistic_report_arg_help[] = {
  0,
};

static const console_descriptive_command_t _wifi_stop_statistic_report_command = {
  .description   = "Stop wifi statistic report",
  .argument_help = _wifi_stop_statistic_report_arg_help,
  .handler       = wifi_stop_statistic_report_command_handler,
  .argument_list = { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(wifi_interface)), CONSOLE_ARG_END }
};

extern sl_status_t wifi_test_client_configuration_command_handler(console_args_t *arguments);
static const char *_wifi_test_client_configuration_arg_help[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

static const console_descriptive_command_t _wifi_test_client_configuration_command = {
  .description   = "Start connecting to specified AP",
  .argument_help = _wifi_test_client_configuration_arg_help,
  .handler       = wifi_test_client_configuration_command_handler,
  .argument_list = { CONSOLE_ARG_STRING,
                     CONSOLE_ARG_STRING,
                     CONSOLE_OPTIONAL_ARG('s', CONSOLE_ENUM_ARG(wifi_security)),
                     CONSOLE_OPTIONAL_ARG('u', CONSOLE_ARG_STRING),
                     CONSOLE_OPTIONAL_ARG('e', CONSOLE_ENUM_ARG(wifi_encryption)),
                     CONSOLE_OPTIONAL_ARG('t', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('l', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('c', CONSOLE_ARG_UINT),
                     CONSOLE_OPTIONAL_ARG('b', CONSOLE_ENUM_ARG(wifi_band)),
                     CONSOLE_OPTIONAL_ARG('w', CONSOLE_ENUM_ARG(wifi_bandwidth)),
                     CONSOLE_ARG_END }
};

extern sl_status_t sl_wifi_update_gain_table_command_handler(console_args_t *arguments);
static const char *_wifi_update_gain_table_arg_help[] = {
  0,
  0,
};

static const console_descriptive_command_t _wifi_update_gain_table_command = {
  .description   = "Assign the user configurable channel gain values in different regions to the module from user",
  .argument_help = _wifi_update_gain_table_arg_help,
  .handler       = sl_wifi_update_gain_table_command_handler,
  .argument_list = { CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_END }
};

const console_database_t _wifi_command_database = { CONSOLE_DATABASE_ENTRIES(
  { "ap", &_wifi_ap_command },
  { "clear_ca_store", &_wifi_clear_ca_store_command },
  { "clear_tls_certs", &_wifi_clear_tls_certs_command },
  { "client", &_wifi_client_command },
  { "deinit", &_wifi_deinit_command },
  { "disable_target_wake_time", &_wifi_disable_target_wake_time_command },
  { "enable_target_wake_time", &_wifi_enable_target_wake_time_command },
  { "filter_broadcast", &_wifi_filter_broadcast_command },
  { "generate_wps_pin", &_wifi_generate_wps_pin_command },
  { "get_advanced_scan_configuration", &_wifi_get_advanced_scan_configuration_command },
  { "get_channel", &_wifi_get_channel_command },
  { "get_default_interface", &_wifi_get_default_interface_command },
  { "get_fw_version", &_wifi_get_fw_version_command },
  { "get_mac_address", &_wifi_get_mac_address_command },
  { "get_performance_profile", &_wifi_get_performance_profile_command },
  { "get_signal_strength", &_wifi_get_signal_strength_command },
  { "get_statistics", &_wifi_get_statistics_command },
  { "get_transmit_rate", &_wifi_get_transmit_rate_command },
  { "get_tx_power", &_wifi_get_tx_power_command },
  { "init", &_wifi_init_command },
  { "is_interface_up", &_wifi_is_interface_up_command },
  { "load_cert", &_wifi_load_cert_command },
  { "load_certificate", &_wifi_load_certificate_command },
  { "scan", &_wifi_scan_command },
  { "send_raw_data", &_wifi_send_raw_data_command },
  { "set_11ax_config", &_wifi_set_11ax_config_command },
  { "set_advanced_scan_configuration", &_wifi_set_advanced_scan_configuration_command },
  { "set_antenna", &_wifi_set_antenna_command },
  { "set_channel", &_wifi_set_channel_command },
  { "set_config", &_wifi_set_config_command },
  { "set_default_con_dis_stats", &_wifi_set_default_con_dis_stats_command },
  { "set_default_interface", &_wifi_set_default_interface_command },
  { "set_default_ssl_cipher", &_wifi_set_default_ssl_cipher_command },
  { "set_eap_credentials", &_wifi_set_eap_credentials_command },
  { "set_extended_default_ssl_cipher", &_wifi_set_extended_default_ssl_cipher_command },
  { "set_extended_ssl_cipher", &_wifi_set_extended_ssl_cipher_command },
  { "set_join_feature_bitmap", &_wifi_set_join_feature_bitmap_command },
  { "set_listen_interval", &_wifi_set_listen_interval_command },
  { "set_performance_profile", &_wifi_set_performance_profile_command },
  { "set_roam_configuration", &_wifi_set_roam_configuration_command },
  { "set_ssl_cipher", &_wifi_set_ssl_cipher_command },
  { "set_timeout_config", &_wifi_set_timeout_config_command },
  { "set_transmit_rate", &_wifi_set_transmit_rate_command },
  { "set_tx_power", &_wifi_set_tx_power_command },
  { "set_wifi_region", &_wifi_set_wifi_region_command },
  { "sl_si91x_transmit_test_start", &_wifi_sl_si91x_transmit_test_start_command },
  { "sl_si91x_transmit_test_stop", &_wifi_sl_si91x_transmit_test_stop_command },
  { "sl_wifi_set_antenna", &_wifi_sl_wifi_set_antenna_command },
  { "start_statistic_report", &_wifi_start_statistic_report_command },
  { "stop_ap_interface", &_wifi_stop_ap_interface_command },
  { "stop_scan", &_wifi_stop_scan_command },
  { "stop_statistic_report", &_wifi_stop_statistic_report_command },
  { "test_client_configuration", &_wifi_test_client_configuration_command },
  { "update_gain_table", &_wifi_update_gain_table_command }, ) };

static const console_descriptive_command_t _wifi_command = { .description = "Wi-Fi commands",
                                                             0,
                                                             .sub_command_database = &_wifi_command_database,
                                                             { CONSOLE_ARG_SUB_COMMAND, CONSOLE_ARG_END } };
const console_database_t console_command_database        = { CONSOLE_DATABASE_ENTRIES(
  { "aws_iot_finalize_json_document", &_aws_iot_finalize_json_document_command },
  { "aws_iot_mqtt_autoreconnect_set_status", &_aws_iot_mqtt_autoreconnect_set_status_command },
  { "aws_iot_mqtt_connect", &_aws_iot_mqtt_connect_command },
  { "aws_iot_mqtt_disconnect", &_aws_iot_mqtt_disconnect_command },
  { "aws_iot_mqtt_init", &_aws_iot_mqtt_init_command },
  { "aws_iot_mqtt_publish", &_aws_iot_mqtt_publish_command },
  { "aws_iot_mqtt_publish_more", &_aws_iot_mqtt_publish_more_command },
  { "aws_iot_mqtt_subscribe", &_aws_iot_mqtt_subscribe_command },
  { "aws_iot_mqtt_unsubscribe", &_aws_iot_mqtt_unsubscribe_command },
  { "aws_iot_shadow_add_reported", &_aws_iot_shadow_add_reported_command },
  { "aws_iot_shadow_connect", &_aws_iot_shadow_connect_command },
  { "aws_iot_shadow_init", &_aws_iot_shadow_init_command },
  { "aws_iot_shadow_init_json_document", &_aws_iot_shadow_init_json_document_command },
  { "aws_iot_shadow_register_delta", &_aws_iot_shadow_register_delta_command },
  { "aws_iot_shadow_update", &_aws_iot_shadow_update_command },
  { "aws_iot_shadow_yield", &_aws_iot_shadow_yield_command },
  { "aws_select_socket", &_aws_select_socket_command },
  { "ble_add_char_serv_att", &_ble_add_char_serv_att_command },
  { "ble_add_char_val_att", &_ble_add_char_val_att_command },
  { "ble_add_service", &_ble_add_service_command },
  { "ble_add_white_list", &_ble_add_white_list_command },
  { "ble_add_white_list_adv_data", &_ble_add_white_list_adv_data_command },
  { "ble_ae_dev_to_periodic_list", &_ble_ae_dev_to_periodic_list_command },
  { "ble_ae_read_periodic_adv_list_size", &_ble_ae_read_periodic_adv_list_size_command },
  { "ble_ae_set_periodic_sync", &_ble_ae_set_periodic_sync_command },
  { "ble_ae_set_scan_enable", &_ble_ae_set_scan_enable_command },
  { "ble_ae_set_scan_params", &_ble_ae_set_scan_params_command },
  { "ble_clear_white_list", &_ble_clear_white_list_command },
  { "ble_connect", &_ble_connect_command },
  { "ble_connect_cancel", &_ble_connect_cancel_command },
  { "ble_deletefrom_white_list", &_ble_deletefrom_white_list_command },
  { "ble_disconnect", &_ble_disconnect_command },
  { "ble_execute_write_async", &_ble_execute_write_async_command },
  { "ble_extended_connect_with_params", &_ble_extended_connect_with_params_command },
  { "ble_get_att_desc", &_ble_get_att_desc_command },
  { "ble_get_att_value_async", &_ble_get_att_value_async_command },
  { "ble_get_char_services_async", &_ble_get_char_services_async_command },
  { "ble_get_char_value_by_uuid", &_ble_get_char_value_by_uuid_command },
  { "ble_get_device_state", &_ble_get_device_state_command },
  { "ble_get_max_adv_data_len", &_ble_get_max_adv_data_len_command },
  { "ble_get_max_no_of_supp_adv_sets", &_ble_get_max_no_of_supp_adv_sets_command },
  { "ble_get_profile_async", &_ble_get_profile_async_command },
  { "ble_get_profiles_async", &_ble_get_profiles_async_command },
  { "ble_notify_custom_value", &_ble_notify_custom_value_command },
  { "ble_notify_value", &_ble_notify_value_command },
  { "ble_per_rx", &_ble_per_rx_command },
  { "ble_per_tx", &_ble_per_tx_command },
  { "ble_prepare_write_async", &_ble_prepare_write_async_command },
  { "ble_read_phy", &_ble_read_phy_command },
  { "ble_remove_gatt_att", &_ble_remove_gatt_att_command },
  { "ble_remove_gatt_service", &_ble_remove_gatt_service_command },
  { "ble_remove_service", &_ble_remove_service_command },
  { "ble_set_adv_data", &_ble_set_adv_data_command },
  { "ble_set_ae_data", &_ble_set_ae_data_command },
  { "ble_set_ae_params", &_ble_set_ae_params_command },
  { "ble_set_att_cmd", &_ble_set_att_cmd_command },
  { "ble_set_att_value_async", &_ble_set_att_value_async_command },
  { "ble_set_phy", &_ble_set_phy_command },
  { "ble_set_scan_response_data", &_ble_set_scan_response_data_command },
  { "ble_start_advertising", &_ble_start_advertising_command },
  { "ble_start_ae_advertising", &_ble_start_ae_advertising_command },
  { "ble_start_notify_value", &_ble_start_notify_value_command },
  { "ble_start_scanning", &_ble_start_scanning_command },
  { "ble_stop_scanning", &_ble_stop_scanning_command },
  { "ble_update_conn_params", &_ble_update_conn_params_command },
  { "ble_uuid_32bit_to_128bit", &_ble_uuid_32bit_to_128bit_command },
  { "bsd_socket", &_bsd_socket_command },
  { "bt_deinit", &_bt_deinit_command },
  { "bt_get_local_address", &_bt_get_local_address_command },
  { "bt_get_local_name", &_bt_get_local_name_command },
  { "bt_power_save_profile", &_bt_power_save_profile_command },
  { "bt_set_local_name", &_bt_set_local_name_command },
  { "bt_stop_advertising", &_bt_stop_advertising_command },
  { "dns", &_dns_command },
  { "get", &_get_command },
  { "get_socket_rx_stats", &_get_socket_rx_stats_command },
  { "help", &_help_command },
  { "host_reset", &_host_reset_command },
  { "http_client", &_http_client_command },
  { "init_ble", &_init_ble_command },
  { "init_ble_gatt", &_init_ble_gatt_command },
  { "iot_socket", &_iot_socket_command },
  { "iperf", &_iperf_command },
  { "list", &_list_command },
  { "m4_powersave", &_m4_powersave_command },
  { "m4_ta_secure_handshake", &_m4_ta_secure_handshake_command },
  { "mallinfo", &_mallinfo_command },
  { "mem", &_mem_command },
  { "mqtt", &_mqtt_command },
  { "net", &_net_command },
  { "nvm", &_nvm_command },
  { "otaf", &_otaf_command },
  { "read_max_data_length", &_read_max_data_length_command },
  { "send_data_config_pkts", &_send_data_config_pkts_command },
  { "set", &_set_command },
  { "set_ae_set_random_address", &_set_ae_set_random_address_command },
  { "set_ble_duty_cycle", &_set_ble_duty_cycle_command },
  { "set_ble_params", &_set_ble_params_command },
  { "set_data_length", &_set_data_length_command },
  { "sl_net_join_multicast_address", &_sl_net_join_multicast_address_command },
  { "sl_net_leave_multicast_address", &_sl_net_leave_multicast_address_command },
  { "sl_si91x_wireless", &_sl_si91x_wireless_command },
  { "sntp", &_sntp_command },
  { "switch_m4_freq", &_switch_m4_freq_command },
  { "t", &_t_command },
  { "terminate", &_terminate_command },
  { "thread", &_thread_command },
  { "thread_info", &_thread_info_command },
  { "time", &_time_command },
  { "tx_data_on_multiple_sockets", &_tx_data_on_multiple_sockets_command },
  { "wifi", &_wifi_command }, ) };
