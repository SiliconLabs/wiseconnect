/*******************************************************************************
* @file  gap.h
* @brief GAP Component API Header
*******************************************************************************
* # License
* <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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

/**
 * @brief GAP Component API Header
 * 
 * EXTRACTED FROM: ble_private.h (GAP-specific structures only)
 * 
 * This header provides GAP-specific types, structures, and APIs including:
 * - Connection state management structures
 * - Advertising and scanning state definitions
 * - Connection configuration structures
 * - Multiprotocol connection info structure (GAP owns/defines this)
 * - GAP utility function declarations
 */

#ifndef GAP_H
#define GAP_H

// Standard C headers
#include <stdint.h>  // uint8_t, uint16_t, int8_t, int16_t, int32_t
#include <stdbool.h> // bool type

// GAP component configuration.
// SLC copies gap_config.h to <project>/config/ble_config/ and adds that directory
// to the compiler include (-I) path, so user customizations made after project
// generation take precedence over the default copy in the component's inc/ folder.
//
// IMPORTANT: this MUST use the angle-bracket form. Quoted ("...") includes search
// the directory of the including file first, and gap.h itself lives next to the
// default gap_config.h in inc/ -- so a quoted include would always pick the
// default and silently ignore any user override under config/ble_config/.
#include <gap_config.h> // resolved via -I: config/ble_config/ first, then component inc/

// BLE SDK headers (for type definitions used in structures)
#include "rsi_ble.h"      // rsi_ble_event_* types, uuid_t
#include "rsi_ble_apis.h" // profile_descriptors_t, rsi_bt_event_* types

/*=======================================================================*/
//! GAP Connection Configuration Structure
//! EXTRACTED FROM: ble_private.h lines 14-47
/*=======================================================================*/
typedef struct rsi_ble_conn_config_s {
  uint8_t conn_id;
  bool smp_enable;
  bool add_to_acceptlist;
  bool profile_discovery;
  bool data_transfer;
  bool rx_notifications;
  bool rx_indications;
  bool tx_notifications;
  bool tx_write;
  bool tx_write_no_response;
  bool tx_indications;
  uint16_t tx_write_clientservice_uuid;
  uint16_t tx_write_client_char_uuid;
  uint16_t tx_wnr_client_service_uuid;
  uint16_t tx_wnr_client_char_uuid;
  uint16_t rx_indi_client_service_uuid;
  uint16_t rx_indi_client_char_uuid;
  uint16_t rx_notif_client_service_uuid;
  uint16_t rx_notif_client_char_uuid;
  struct buff_mode_sel_s {
    bool buffer_mode;
    uint8_t buffer_cnt;
    uint8_t max_data_length;
  } buff_mode_sel;
  struct conn_param_update_s {
    uint16_t conn_int;
    uint8_t conn_latncy;
    uint16_t supervision_to;
  } conn_param_update;
} rsi_ble_conn_config_t;

/*=======================================================================*/
//! GAP BLE Configuration Info Structure
//! EXTRACTED FROM: ble_private.h lines 49-51
/*=======================================================================*/
typedef struct ble_confg_info_s {
  rsi_ble_conn_config_t ble_conn_configuration[TOTAL_CONNECTIONS];
} ble_confg_info_t;

/*=======================================================================*/
//! GAP Connection State Enumerations
//! EXTRACTED FROM: ble_private.h lines 59-90
/*=======================================================================*/
typedef enum ble_connection_state_s {
  disconnected_state,      //! ACL link does not exist
  initiated_connect_state, //! Application triggered connect request, waiting on_connect
  on_connect_state,        //! received on_connect, security exchanges ongoing
  connected_state,         //! auth complete
} ble_connection_state_t;

typedef enum adv_state_s {
  adv_disabled, //! adv is off
  adv_enabled,  //! adv enabled
  adv_enabled_non_connectable,
} adv_state_t;

typedef enum scan_state_s {
  scan_off,             //! scan_off
  connectable_scan,     //! scan on and connection is possible
  non_connectable_scan, //! scan on and connection is not possible
} scan_state_t;

typedef enum mtu_state_s {
  mtu_exchange_info_state,
  mtu_exchange_done_state,
} mtu_state_t;

typedef enum smp_state_s {
  smp_enable,
  smp_triggered,
  smp_pending,
  smp_process_done,
} smp_state_t;

/*=======================================================================*/
//! More Data State Structure
//! EXTRACTED FROM: ble_private.h lines 91-94
/*=======================================================================*/
typedef struct more_data_state_beta_s {
  uint8_t data_transmit;
} more_data_state_beta_t;

/*=======================================================================*/
//! GAP Profile List Structure
//! EXTRACTED FROM: ble_private.h lines 53-57
/*=======================================================================*/
typedef struct rsi_ble_profile_list_by_conn_s {
  profile_descriptors_t *profile_desc;
  rsi_ble_event_profile_by_uuid_t *profile_info_uuid;
  rsi_ble_event_read_by_type1_t *profile_char_info;
} rsi_ble_profile_list_by_conn_t;

/*=======================================================================*/
//! Multiprotocol Connection Info Structure (GAP DEFINES/OWNS)
//! EXTRACTED FROM: ble_private.h lines 101-216
//! NOTE: This structure is used by GAP, SMP, and GATT
//!       GAP component DEFINES it, others extern it
/*=======================================================================*/
typedef struct rsi_ble_conn_info_s {
  uint8_t conn_id;
  uint8_t conn_status;

  ble_connection_state_t conn_state;
  adv_state_t adv_state;
  scan_state_t scan_state;
  bool buff_config_done;
  bool conn_param_req_given;
  bool char_resp_recvd;
  bool char_desc_resp_recvd;
  bool write_handle_found;
  rsi_ble_conn_config_t *ble_conn_conf;
  bool smp_pairing_initated;
  bool smp_pairing_request_received;
  uint16_t write_cnt;
  bool write_wwr_handle_found;
  int16_t write_handle;
  uint16_t wwr_count;
  bool notify_handle_found;
  uint16_t notify_handle;
  bool indication_handle_found;
  uint16_t indication_handle;
  int16_t write_wwr_handle;
  bool transmit;
  uint8_t l_num_of_services;
  uint8_t l_char_property;
  uint8_t char_for_serv_cnt;
  uint8_t char_desc_cnt;
  uint8_t profile_index_for_char_query;
  uint8_t temp1;
  uint8_t temp2;
  uint16_t temp_prepare_write_value_len;

  uint8_t temp_prepare_write_value[250];
  uint8_t mtu_exchange_done;

  uint8_t prep_write_err;
  uint8_t first_connect;
  uint8_t smp_done;
  uint8_t neg_rply;
  uint16_t offset;
  uint16_t handle;
  uint8_t type;
  uint8_t read_data1[230];
  uint8_t max_data_length;
  bool rsi_rx_from_rem_dev;
  bool rsi_tx_to_rem_dev;
  uint16_t indication_cnt;
  uint16_t notfy_cnt;
  uint8_t prof_cnt;
  uint8_t char_cnt;

  bool notification_received;
  uint8_t str_remote_address[RSI_REM_DEV_ADDR_LEN];
  bool done_profiles_query;
  bool prof_resp_recvd;

  uint8_t no_of_profiles;
  uint8_t total_remote_profiles;

  uint8_t profs_evt_cnt, prof_evt_cnt;

  uint16_t profiles_endhandle;
  uint8_t rsi_connected_dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t skip_end_profile;
  uint8_t switch_case_count;
  rsi_ble_profile_list_by_conn_t rsi_ble_profile_list_by_conn;
  bool profile_mem_init;
  bool service_char_mem_init;

  uint8_t remote_device_role;  //! 1 - remote device is peripheral, 2 - remote device is central
  uint8_t remote_dev_addr[18]; //! 18 = RSI_REM_DEV_ADDR_LEN
  rsi_ble_event_remote_features_t remote_dev_feature;
  rsi_ble_event_adv_report_t rsi_app_adv_reports_to_app;
  rsi_ble_ae_adv_report_t rsi_app_ae_adv_reports_to_app;
  rsi_ble_event_conn_update_t conn_update_resp;
  rsi_ble_event_remote_conn_param_req_t rsi_app_remote_device_conn_params;
  rsi_ble_event_conn_status_t conn_event_to_app;
  rsi_ble_event_enhance_conn_status_t rsi_enhc_conn_status;
  bool is_enhanced_conn;
  rsi_ble_event_write_t app_ble_write_event;
  rsi_ble_event_prepare_write_t app_ble_prepared_write_event;
  rsi_ble_execute_write_t app_ble_execute_write_event;
  rsi_ble_set_att_resp_t rsi_ble_write_resp_event;
  rsi_ble_read_req_t app_ble_read_event;
  rsi_ble_event_mtu_t app_ble_mtu_event;
  rsi_ble_event_mtu_exchange_information_t mtu_exchange_info;
  rsi_ble_event_disconnect_t rsi_ble_disconn_resp;
  rsi_ble_event_profiles_list_t get_allprofiles;
  rsi_ble_event_profile_by_uuid_t get_profile;
  rsi_ble_event_read_by_type1_t get_char_services;
  rsi_ble_event_att_value_t rsi_char_descriptors;
  rsi_ble_event_error_resp_t rsi_ble_gatt_err_resp;
  rsi_bt_event_smp_resp_t rsi_ble_event_smp_resp;
  rsi_bt_event_smp_req_t rsi_ble_event_smp_req;
  rsi_bt_event_smp_passkey_t rsi_ble_event_smp_passkey;
  rsi_bt_event_smp_passkey_display_t rsi_ble_smp_passkey_display;
  rsi_bt_event_sc_passkey_t rsi_event_sc_passkey;
  rsi_bt_event_smp_failed_t rsi_ble_smp_failed;
  rsi_bt_event_encryption_enabled_t rsi_encryption_enabled;
  rsi_bt_event_le_ltk_request_t rsi_le_ltk_resp;
  rsi_bt_event_le_security_keys_t rsi_le_security_keys;
  rsi_bt_event_encryption_enabled_t l_rsi_encryption_enabled;
#if (CONNECT_OPTION == CONN_BY_NAME)
  uint8_t *rsi_remote_name;
#endif
  uuid_t search_serv_conn[10];

  mtu_state_t mtu_state;
  smp_state_t smp_state;
  bool notify_enabled;
  bool indication_enabled;
} rsi_ble_conn_info_t;

/*=======================================================================*/
//! Parsed Configuration Structure
//! EXTRACTED FROM: ble_private.h lines 218-238
/*=======================================================================*/
typedef struct rsi_parsed_conf_s {
  struct rsi_protocol_sel_t {
    bool is_ble_enabled;
    bool is_bt_enabled;
    bool is_ant_enabled;
    bool is_wifi_enabled;
  } rsi_protocol_sel;
  struct rsi_ble_config_t {
    uint8_t no_of_peripherals;
    uint8_t no_of_centrals;
    uint8_t conn_by_name;
    uint8_t peripheral1_name[RSI_REM_DEV_NAME_LEN];
    uint8_t peripheral2_name[RSI_REM_DEV_NAME_LEN];
    uint8_t peripheral3_name[RSI_REM_DEV_NAME_LEN];
    uint8_t peripheral1_addr[RSI_REM_DEV_ADDR_LEN];
    uint8_t peripheral2_addr[RSI_REM_DEV_ADDR_LEN];
    uint8_t peripheral3_addr[RSI_REM_DEV_ADDR_LEN];
    rsi_ble_conn_config_t rsi_ble_conn_config[TOTAL_CONNECTIONS];
  } rsi_ble_config;
} rsi_parsed_conf_t;

/*=======================================================================*/
//! GAP Initialization API
/*=======================================================================*/

/**
 * @fn         ble_gap_init
 * @brief      Initialize GAP component
 * @return     int32_t - SL_STATUS_OK on success, error code otherwise
 */
int32_t ble_gap_init(void);

/*=======================================================================*/
//! Multiprotocol Connection Management APIs (GAP provides for SMP, GATT)
/*=======================================================================*/

/**
 * @fn         rsi_get_remote_device_role
 * @brief      Get the role of the remote device (central or peripheral)
 * @param[in]  remote_dev_addr - Remote device address
 * @return     uint8_t - CENTRAL_ROLE or PERIPHERAL_RL
 */
uint8_t rsi_get_remote_device_role(uint8_t *remote_dev_addr);

/**
 * @fn         rsi_check_dev_list_driver
 * @brief      Check if a device is already connected or should be connected
 * @param[in]  remote_dev_name - Remote device name (if CONNECT_OPTION == CONN_BY_NAME)
 * @param[in]  adv_dev_addr - Remote device address
 * @return     uint8_t - PERIPHERAL_FOUND, PERIPHERAL_CONNECTED, or NO_PERIPHERAL_FOUND
 */
uint8_t rsi_check_dev_list_driver(uint8_t *remote_dev_name, uint8_t *adv_dev_addr);

/**
 * @fn         rsi_get_ble_conn_id
 * @brief      Get connection ID for a remote device, add if not found
 * @param[in]  remote_dev_addr - Remote device address
 * @param[in]  remote_name - Remote device name (if CONNECT_OPTION == CONN_BY_NAME)
 * @param[in]  size - Size of remote name
 * @return     uint8_t - Connection ID
 */
#if (CONNECT_OPTION == CONN_BY_NAME)
uint8_t rsi_get_ble_conn_id(uint8_t *remote_dev_addr, uint8_t *remote_name, uint8_t size);
#else
uint8_t rsi_get_ble_conn_id(uint8_t *remote_dev_addr);
#endif

/**
 * @fn         rsi_add_ble_conn_id
 * @brief      Add a new connection ID for a remote device
 * @param[in]  remote_dev_addr - Remote device address
 * @param[in]  remote_name - Remote device name (if CONNECT_OPTION == CONN_BY_NAME)
 * @param[in]  size - Size of remote name
 * @return     uint8_t - Connection ID
 */
#if (CONNECT_OPTION == CONN_BY_NAME)
uint8_t rsi_add_ble_conn_id(uint8_t *remote_dev_addr, uint8_t *remote_name, uint8_t size);
#else
uint8_t rsi_add_ble_conn_id(uint8_t *remote_dev_addr);
#endif

/**
 * @fn         rsi_ble_initialize_conn_buffer
 * @brief      Initialize configurations for each connection
 * @param[in]  ble_conn_spec_conf - Connection configuration buffer
 * @return     int8_t - RSI_SUCCESS or RSI_FAILURE
 */
int8_t rsi_ble_initialize_conn_buffer(rsi_ble_conn_config_t *ble_conn_spec_conf);

/**
 * @fn         rsi_fill_ble_user_config
 * @brief      Fill BLE user configuration
 * @return     int8_t - RSI_SUCCESS or RSI_FAILURE
 */
int8_t rsi_fill_ble_user_config(void);

/**
 * @fn         ble_private_default_init
 * @brief      Initialize connection info structures to default values
 * @return     void
 */
void ble_private_default_init(void);

/**
 * @fn         rsi_change_ble_adv_and_scan_params
 * @brief      Setup default advertising and scan parameters
 * @return     void
 */
void rsi_change_ble_adv_and_scan_params(void);

// TODO: GATT Client init code - will be moved to GATT client component
// /**
//  * @fn         rsi_assign_remote_data_serv_and_char
//  * @brief      Assign remote data transfer service and characteristic UUIDs
//  * @return     void
//  */
// void rsi_assign_remote_data_serv_and_char(void);

#endif // GAP_H
