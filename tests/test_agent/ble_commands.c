#include "console.h"
#include "rsi_ble_apis.h"
#include "rsi_bt_common.h"
#include "rsi_bt_common_apis.h"
#include "rsi_utils.h"
#include "rsi_common_apis.h"
#include "sl_constants.h"
#include "sl_si91x_status.h"
#include "sl_ieee802_types.h"
#include "sl_utility.h"
#include <stdint.h>
#include <string.h>
#include <printf.h>
#include <stdbool.h>
#include "command_identifiers.h"
#include <rsi_ble.h>
#include "ble_config.h"

osSemaphoreId_t ble_wait;
uint8_t suspend_ble;
int8_t rsi_app_resp_tx_power = 0;
extern uint8_t g_smp;

//REFERENCED FROM
//Host_MCU\examples\_internal\Wireless_Examples\ble\simple_smp\rsi_ble_smp.c
//Host_MCU\examples\_internal\Wireless_Examples\ble\peripheral_secureconnection\rsi_ble_sc.c

//! application event list
#define RSI_APP_EVENT_ADV_REPORT                 0
#define RSI_BLE_CONN_EVENT                       1
#define RSI_BLE_DISCONN_EVENT                    2
#define RSI_BLE_SMP_REQ_EVENT                    3
#define RSI_BLE_SMP_RESP_EVENT                   4
#define RSI_BLE_SMP_PASSKEY_EVENT                5
#define RSI_BLE_SMP_FAILED_EVENT                 6
#define RSI_BLE_ENCRYPT_STARTED_EVENT            7
#define RSI_BLE_SMP_PASSKEY_DISPLAY_EVENT        8
#define RSI_BLE_SC_PASSKEY_EVENT                 9
#define RSI_BLE_LTK_REQ_EVENT                    10
#define RSI_BLE_MTU_EVENT                        11
#define RSI_BLE_GATT_WRITE_EVENT                 12
#define RSI_BLE_DATA_LENGTH_CHANGE_EVENT         13
#define RSI_BLE_PHY_UPDATE_COMPLETE_EVENT        14
#define RSI_BLE_CONN_PARAM_UPDATE_COMPLETE_EVENT 15
#define RSI_BLE_MTU_EXCHANGE_INFORMATION         16
#define RSI_BLE_ENHC_CONN_EVENT                  17
#define RSI_APP_EVENT_AE_ADV_REPORT              18
#define RSI_APP_EVENT_AE_PER_SYNC_ESTBL          19
#define RSI_APP_EVENT_AE_PER_ADV_REPORT          20
#define RSI_APP_EVENT_AE_PER_ADV_SYNC_LOST       21
#define RSI_APP_EVENT_AE_SCAN_TIMEOUT            22
#define RSI_APP_EVENT_AE_ADV_SET_TERMINATED      23
#define RSI_APP_EVENT_AE_SCAN_REQ_RECVD          24
#define RSI_APP_EVENT_CONNECTED                  25
#define RSI_APP_EVENT_PHY_UPDATE_COMPLETE        26
#define MAX_MTU_SIZE                             240
#define LOCAL_MTU_SIZE                           232
#define RSI_PRINT_LOGS                           1
#define RSI_BLE_MTU_EXCHANGE_FROM_HOST           1

#ifndef LOG_PRINT
#if RSI_PRINT_LOGS
#define LOG_PRINT(...) printf(__VA_ARGS__)
#else
#define LOG_PRINT(...)
#endif
#endif
#define AE_SCAN_SET1        0
#define AE_SCAN_SET2        1
#define SET2_ENABLE         1
#define BLE_AE_ADV_DATA_LEN 0x19
#define BLE_AE_ADV_DATA     "AE_PERIPHERAL_DATA_1"
#define RSI_PRINT_EVENTS    1

#if RSI_PRINT_EVENTS
#define LOG_EVENT(...) printf(__VA_ARGS__)
#else
#define LOG_EVENT(...)
#endif

#define STACK_BTLE_MODE 0x02
#define MITM_REQ        0x01

#define RSI_DEV_ADDR_LEN 6

//! local device IO capability
#define RSI_BLE_SMP_IO_CAPABILITY 0x03 //3 - NoInputNoOutput
#define RSI_BLE_SMP_PASSKEY       0
//!  Gatt Thermometer macros
#define RSI_BLE_HEALTH_THERMOMETER_UUID       0x1809
#define RSI_BLE_TEMPERATURE_MEASUREMENT_UUID  0x2A1C
#define RSI_BLE_TEMPERATURE_TYPE_UUID         0x2A1D
#define RSI_BLE_INTERMEDIATE_TEMPERATURE_UUID 0x2A1E
#define RSI_BLE_ATT_PROPERTY_READ             0x02
#define RSI_BLE_ATT_PROPERTY_WRITE            0x08
#define RSI_BLE_ATT_PROPERTY_NOTIFY           0x10
#define RSI_BLE_ATT_PROPERTY_INDICATE         0x20
typedef struct temp_measurement_s {
  uint8_t flags;
  float float_val;
} temp_measurement_t;
//TODO make the above definitions to dynamic variable

// global for ble capabilities
uint8_t g_ble_capability = 3;

//global for bond info
uint8_t g_bond_info = 0;

// global for set smp
uint8_t g_smp = 0;

//ble connection flag
uint8_t already_paired = 0;

//! max data length
#define RSI_BLE_MAX_DATA_LEN 230

#define RSI_BLE_TASK_STACK_SIZE 2048
#define RSI_BLE_TASK_PRIORITY   osPriorityNormal

//! BLE attribute service types uuid values
#define RSI_BLE_CHAR_SERV_UUID   0x2803
#define RSI_BLE_CLIENT_CHAR_UUID 0x2902

//! attribute properties
#define RSI_BLE_ATT_PROPERTY_READ   0x02
#define RSI_BLE_ATT_PROPERTY_WRITE  0x08
#define RSI_BLE_ATT_PROPERTY_NOTIFY 0x10

//! Notify status
#define NOTIFY_DISABLE 0x00
#define NOTIFY_ENABLE  0x01

// Callbacks
static void rsi_ble_app_init_events();
static int32_t rsi_ble_app_get_event(void);
static void rsi_ble_app_clear_event(uint32_t event_num);
static void rsi_ble_app_set_event(uint32_t event_num);

void rsi_ble_simple_central_on_adv_report_event(rsi_ble_event_adv_report_t *adv_report);
void rsi_ble_simple_central_on_ae_adv_report_event(uint16_t status, rsi_ble_ae_adv_report_t *ae_adv_report);
void rsi_ble_simple_central_on_ae_periodic_adv_sync_estbl_event(uint16_t status,
                                                                rsi_ble_per_adv_sync_estbl_t *ae_periodic_adv_sync);
void rsi_ble_simple_central_on_ae_periodic_adv_report_event(uint16_t status,
                                                            rsi_ble_per_adv_report_t *ae_per_adv_report);
void rsi_ble_simple_central_on_ae_periodic_adv_sync_lost_event(uint16_t status,
                                                               rsi_ble_per_adv_sync_lost_t *ae_per_sync_lost);
void rsi_ble_simple_central_on_ae_scan_timeout_event(uint16_t status, rsi_ble_scan_timeout_t *ae_scan_timeout);
void rsi_ble_simple_central_on_ae_adv_set_terminated_event(uint16_t status, rsi_ble_adv_set_terminated_t *ae_adv_set);
void rsi_ble_simple_central_on_ae_scan_req_recvd_event(uint16_t status, rsi_ble_scan_req_recvd_t *ae_scan_req);
void rsi_ble_on_enhance_conn_status_event(rsi_ble_event_enhance_conn_status_t *resp_enh_conn);
void rsi_ble_data_length_change_event(rsi_ble_event_data_length_update_t *rsi_ble_data_length_update);
void rsi_ble_phy_set_update_complete_event(rsi_ble_event_phy_update_t *rsi_ble_event_phy_update_complete);
void rsi_ble_update_conn_param_complete_event(rsi_ble_event_conn_update_t *rsi_ble_event_conn_update_complete,
                                              uint16_t resp_status);
static void rsi_ble_on_connect_event(rsi_ble_event_conn_status_t *resp_conn);
static void rsi_ble_on_disconnect_event(rsi_ble_event_disconnect_t *resp_disconnect, uint16_t reason);
void rsi_ble_on_smp_request(rsi_bt_event_smp_req_t *remote_dev_address);
void rsi_ble_on_smp_response(rsi_bt_event_smp_resp_t *remote_dev_address);
void rsi_ble_on_smp_passkey(rsi_bt_event_smp_passkey_t *remote_dev_address);
void rsi_ble_on_smp_passkey_display(rsi_bt_event_smp_passkey_display_t *smp_passkey_display);
void rsi_ble_on_sc_passkey(rsi_bt_event_sc_passkey_t *sc_passkey);
void rsi_ble_on_smp_failed(uint16_t status, rsi_bt_event_smp_failed_t *remote_dev_address);
void rsi_ble_on_encrypt_started(uint16_t status, rsi_bt_event_encryption_enabled_t *enc_enabled);
static void rsi_ble_on_le_ltk_req_event(rsi_bt_event_le_ltk_request_t *le_ltk_req);
void rsi_ble_on_le_security_keys(rsi_bt_event_le_security_keys_t *rsi_ble_event_le_security_keys);
static void rsi_ble_on_mtu_event(rsi_ble_event_mtu_t *rsi_ble_mtu);
static void rsi_ble_on_gatt_write_event(uint16_t event_id, rsi_ble_event_write_t *rsi_ble_write);
static void rsi_ble_on_event_mtu_exchange_info(
  rsi_ble_event_mtu_exchange_information_t *rsi_ble_event_mtu_exchange_info);
void rsi_ble_add_char_serv_att(void *serv_handler,
                               uint16_t handle,
                               uint8_t val_prop,
                               uint16_t att_val_handle,
                               uuid_t att_val_uuid);
void rsi_ble_add_char_val_att(void *serv_handler,
                              uint16_t handle,
                              uuid_t att_type_uuid,
                              uint8_t val_prop,
                              uint8_t *data,
                              uint8_t data_len);
static uint8_t remote_addr_type = 0;
void rsi_ble_fill_128bit_uuid(uint32_t uuid_32bit, uuid_t *serv_uuid);

static volatile uint32_t rsi_app_async_event_map = 0;

static uint8_t device_found = 0;
uint8_t mtu_exchange_done   = 0;
rsi_ble_event_mtu_exchange_information_t mtu_exchange_info;
// unused at this time
//static uint8_t remote_name[31];

static uint8_t ble_str_conn_bd_addr[18]              = { 0 };
static uint8_t ble_remote_dev_addr[RSI_DEV_ADDR_LEN] = { 0 };

// unused at this time
//static uint8_t target_ble_remote_dev_addr_type = LE_PUBLIC_ADDRESS;
//static uint8_t target_remote_name[31];

uint8_t target_ble_remote_dev_addr[18] = { 0 };

rsi_ble_resp_add_serv_t new_serv_resp = { 0 };

static uint32_t numeric_value;
static uint8_t rsi_app_resp_device_state = 0;
rsi_bt_event_encryption_enabled_t glbl_enc_enabled;
rsi_bt_event_le_ltk_request_t temp_le_ltk_req;
static rsi_bt_event_le_security_keys_t app_ble_sec_keys;
rsi_ble_event_mtu_t app_ble_mtu_event;
static uint16_t rsi_ble_att_val_hndl;
static rsi_ble_event_write_t app_ble_write_event;
uint16_t notification_tx_handle = 0;

osSemaphoreId_t ble_thread_sem;
//globals for ble advertising parameters, referenced by #defines in rsi_ble_config.h
uint8_t g_rsi_ble_adv_type;
uint8_t g_rsi_ble_adv_filter_type;
uint8_t g_rsi_ble_addr_type;
uint8_t g_rsi_ble_dir_addr[RSI_DEV_ADDR_LEN];

uint16_t g_rsi_ble_adv_int_min;
uint16_t g_rsi_ble_adv_int_max;
uint8_t g_rsi_own_addr_type;
uint8_t g_rsi_ble_adv_channel_map;
uint8_t g_rsi_ble_pwr_inx = 30;
//globals for ble scan parameters, referenced by #defines in rsi_ble_config.h
uint8_t g_ble_scan_type        = 1;
uint8_t g_ble_scan_filter_type = 0;
uint16_t g_le_scan_interval    = 0x100;
uint16_t g_le_scan_window      = 0x0050;
uint8_t g_le_public_address    = 0;

uint16_t g_connection_interval_min;
uint16_t g_connection_interval_max;
uint16_t g_connection_latency;
uint16_t g_supervision_timeout;
uint8_t g_ble_power_save_options = 0;

osThreadId_t ble_task_handle;

typedef enum { BLE_MASTER_ROLE = 0, BLE_SLAVE_ROLE = 1, BLE_DUAL_ROLE = 2 } rsi_ble_role_t;
static rsi_ble_role_t current_ble_role;

int8_t ble_gatt_data[RSI_BLE_MAX_DATA_LEN] = { 0 };

static void ble_task(void *arg)
{
  UNUSED_PARAMETER(arg);
  int32_t temp_event_map = 0;
  rsi_ble_resp_local_att_value_t local_att_val;
  uint32_t status;

  while (1) {

    if (suspend_ble) {
      suspend_ble = 0;
      osSemaphoreAcquire(ble_wait, osWaitForever);
    }
    // checking for received events
    temp_event_map = rsi_ble_app_get_event();
    if (temp_event_map == RSI_FAILURE) {
      // if events are not received loop will be continued.
      osSemaphoreAcquire(ble_thread_sem, osWaitForever);
      continue;
    }
    switch (temp_event_map) {
      case RSI_APP_EVENT_ADV_REPORT:
        //! advertise report event.
        LOG_EVENT("\nRSI_APP_EVENT_ADV_REPORT \n");
        rsi_ble_app_clear_event(RSI_APP_EVENT_ADV_REPORT);
        //TODO: Do we keep this here are move it outside and call from UTF
        // LOG_PRINT("\nSending rsi_ble_connect\n");
        // print_status(rsi_ble_connect(remote_addr_type, (int8_t *)ble_remote_dev_addr));
        break;

      case RSI_BLE_CONN_EVENT:
        //! event invokes when connection was completed
        PRINT_EVENT_START(BLE_CONNECT, BLE)
        LOG_EVENT("\nRSI_BLE_CONN_EVENT \n");
        rsi_ble_app_clear_event(RSI_BLE_CONN_EVENT);
        if (RSI_BLE_MTU_EXCHANGE_FROM_HOST) {
          uint8_t status = rsi_ble_mtu_exchange_event(ble_remote_dev_addr, MAX_MTU_SIZE);
          if (status != 0) {
            LOG_EVENT("\n MTU Exchange request failed \n");
          } else {
            mtu_exchange_done = 1;
          }
          PRINT_EVENT_END();
        }
        break;

      case RSI_BLE_ENHC_CONN_EVENT:
        PRINT_EVENT_START(BLE_CONNECT, BLE)
        LOG_EVENT("\n====>RSI_BLE_ENHC_CONN_EVENT\n");
        rsi_ble_app_clear_event(RSI_BLE_ENHC_CONN_EVENT);
        if (RSI_BLE_MTU_EXCHANGE_FROM_HOST) {
          uint8_t status = rsi_ble_mtu_exchange_event(ble_remote_dev_addr, MAX_MTU_SIZE);
          if (status != 0) {
            LOG_EVENT("\r\n MTU Exchange request failed ");
          } else {
            mtu_exchange_done = 1;
          }
          PRINT_EVENT_END();
        }
        break;

      case RSI_APP_EVENT_AE_ADV_REPORT:
        LOG_EVENT("\nRSI_APP_EVENT_AE_ADV_REPORT \n");
        rsi_ble_app_clear_event(RSI_APP_EVENT_AE_ADV_REPORT);
        rsi_ble_ae_extended_create_connect_t ble_extended_create_conn;
        status = rsi_ble_extended_connect_with_params(&ble_extended_create_conn);
        break;

      case RSI_BLE_MTU_EXCHANGE_INFORMATION:
        PRINT_EVENT_START(BLE_CONNECT, BLE)
        LOG_EVENT("\n===>RSI_BLE_MTU_EXCHANGE_INFORMATION \n");
        rsi_ble_app_clear_event(RSI_BLE_MTU_EXCHANGE_INFORMATION); //! perform MTU exchange with remote device
        PRINT_EVENT_END();
        break;

      case RSI_BLE_DISCONN_EVENT:
        //! event invokes when disconnection was completed
        LOG_EVENT("\n===>RSI_BLE_DISCONN_EVENT \n");
        rsi_ble_app_clear_event(RSI_BLE_DISCONN_EVENT);
        device_found = 0;
        if (g_bond_info == 1) {
          already_paired = 1;
        } else {
          already_paired = 0;
          // if(current_ble_role == BLE_SLAVE_ROLE){
          //     //! start the advertising
          //     LOG_PRINT("\nSending rsi_ble_start_advertising\n");
          //     print_status(rsi_ble_start_advertising());
          // }
          // if (current_ble_role == BLE_MASTER_ROLE){
          //     //! start scanning
          //     device_found = 0;
          //     LOG_PRINT("\nSending rsi_ble_start_scanning\n");
          //     print_status(rsi_ble_start_scanning());
          // }
        }
        break;

      case RSI_BLE_DATA_LENGTH_CHANGE_EVENT: {
        //! Data change event
        LOG_EVENT("\n===>RSI_BLE_DATA_LENGTH_CHANGE_EVENT \n");
        //! clear the event.
        rsi_ble_app_clear_event(RSI_BLE_DATA_LENGTH_CHANGE_EVENT);
      } break;

      case RSI_BLE_PHY_UPDATE_COMPLETE_EVENT: {
        //! phy update complete event
        LOG_EVENT("\nRSI_BLE_PHY_UPDATE_COMPLETE_EVENT \n");
        //! clear the event.
        rsi_ble_app_clear_event(RSI_BLE_PHY_UPDATE_COMPLETE_EVENT);
      } break;

      case RSI_BLE_CONN_PARAM_UPDATE_COMPLETE_EVENT: {
        //! conn_params_update complete event
        LOG_EVENT("\n RSI_BLE_CONN_PARAM_UPDATE_COMPLETE_EVENT \n");
        //! clear the event
        rsi_ble_app_clear_event(RSI_BLE_CONN_PARAM_UPDATE_COMPLETE_EVENT);
      } break;

      case RSI_BLE_MTU_EVENT:
        //! event invokes when write/notification events received
        PRINT_EVENT_START(BLE_CONNECT, BLE)
        LOG_EVENT("\n===>RSI_BLE_MTU_EVENT \n");
        rsi_ble_app_clear_event(RSI_BLE_MTU_EVENT);
        if (RSI_BLE_MTU_EXCHANGE_FROM_HOST) {
          uint8_t status = rsi_ble_mtu_exchange_resp(ble_remote_dev_addr, LOCAL_MTU_SIZE);
          if (status != 0) {
            LOG_EVENT("\n MTU Exchange responce failed \n");
          }
        }
        if (current_ble_role == BLE_MASTER_ROLE) {
          if (g_smp == 1) {
            LOG_EVENT("\n===>RSI_SMP_ENABLED \n");
            if (!already_paired) {
              //! If not paired even once
              //! initiating the SMP pairing process
              LOG_EVENT("\n==>Sending rsi_ble_smp_pair_request\n");
              status = rsi_ble_smp_pair_request(app_ble_mtu_event.dev_addr, g_ble_capability, MITM_REQ);
            } else {
              //handle reconnection
              LOG_EVENT("\nEnable encryption from DUT\n");
              status = rsi_ble_start_encryption(app_ble_mtu_event.dev_addr,
                                                glbl_enc_enabled.localediv,
                                                glbl_enc_enabled.localrand,
                                                glbl_enc_enabled.localltk);
            }
          } else {
            LOG_EVENT("\n===>RSI_BLE_ENCRYPT_STARTED_EVENT_NON_SMP_CONNECTION\n");
          }
          PRINT_EVENT_END();
        }
        break;

      case RSI_BLE_LTK_REQ_EVENT:
        //! event invokes when disconnection was completed
        LOG_EVENT("\nRSI_BLE_LTK_REQ_EVENT \n");
        rsi_ble_app_clear_event(RSI_BLE_LTK_REQ_EVENT);
        if (current_ble_role == BLE_SLAVE_ROLE) {
          if ((temp_le_ltk_req.localediv == glbl_enc_enabled.localediv)
              && !((memcmp(temp_le_ltk_req.localrand, glbl_enc_enabled.localrand, 8)))) {
            LOG_EVENT("\n===>RSI_BLE_LTK_REQ_EVENT positive reply\n");
            //! give le ltk req reply cmd with positive reply
            LOG_EVENT("\n==>Sending rsi_ble_ltk_req_reply\n");
            status = rsi_ble_ltk_req_reply(temp_le_ltk_req.dev_addr, 1, glbl_enc_enabled.localltk);
          } else {
            LOG_EVENT("\n===>RSI_BLE_LTK_REQ_EVENT negative reply\n");
            //! give le ltk req reply cmd with negative reply
            LOG_EVENT("\n==>Sending rsi_ble_ltk_req_reply\n");
            status = rsi_ble_ltk_req_reply(temp_le_ltk_req.dev_addr, 0, NULL);
          }
        }
        break;

      case RSI_BLE_SMP_REQ_EVENT:
        //! initiate SMP protocol as a Master
        LOG_EVENT("\nRSI_BLE_SMP_REQ_EVENT \n");
        rsi_ble_app_clear_event(RSI_BLE_SMP_REQ_EVENT);
        if ((current_ble_role == BLE_MASTER_ROLE)) {
          if (!already_paired) {
            //! If once paired
            //! initiating the SMP pairing process
            LOG_EVENT("\n==>Sending rsi_ble_smp_pair_request\n");
            status = rsi_ble_smp_pair_request(ble_remote_dev_addr, g_ble_capability, MITM_REQ);
          }
        } else {
          //handle reconnection
          LOG_EVENT("\n==>Enable encryption from DUT\n");
          status = rsi_ble_start_encryption(app_ble_mtu_event.dev_addr,
                                            glbl_enc_enabled.localediv,
                                            glbl_enc_enabled.localrand,
                                            glbl_enc_enabled.localltk);
        }
        break;

      case RSI_BLE_SMP_RESP_EVENT:
        //! initiate SMP protocol as a Master
        LOG_EVENT("\n===>RSI_BLE_SMP_RESP_EVENT \n");
        rsi_ble_app_clear_event(RSI_BLE_SMP_RESP_EVENT);
        status = rsi_ble_smp_pair_response(ble_remote_dev_addr, g_ble_capability, MITM_REQ);
        break;

      case RSI_BLE_SMP_PASSKEY_EVENT:
        LOG_EVENT("\n===>RSI_BLE_SMP_PASSKEY_EVENT \n");
        rsi_ble_app_clear_event(RSI_BLE_SMP_PASSKEY_EVENT);
        //! initiating the SMP pairing process
        status = rsi_ble_smp_passkey(ble_remote_dev_addr, RSI_BLE_SMP_PASSKEY);
        break;

      case RSI_BLE_SMP_PASSKEY_DISPLAY_EVENT:
        LOG_EVENT("\n===>RSI_BLE_SMP_PASSKEY_DISPLAY_EVENT \n");
        rsi_ble_app_clear_event(RSI_BLE_SMP_PASSKEY_DISPLAY_EVENT);
        break;

      case RSI_BLE_SC_PASSKEY_EVENT:
        LOG_EVENT("\n===>RSI_BLE_SC_PASSKEY_EVENT\n");
        rsi_ble_app_clear_event(RSI_BLE_SC_PASSKEY_EVENT);
        LOG_PRINT("\n==>rsi_ble_smp_passkey\n");
        status = rsi_ble_smp_passkey(ble_remote_dev_addr, numeric_value);
        break;

      case RSI_BLE_SMP_FAILED_EVENT:
        LOG_EVENT("\n===>RSI_BLE_SMP_FAILED_EVENT \n");
        rsi_ble_app_clear_event(RSI_BLE_SMP_FAILED_EVENT);
        break;

      case RSI_BLE_ENCRYPT_STARTED_EVENT:
        //! start the encrypt event
        LOG_EVENT("\n===>RSI_BLE_ENCRYPT_STARTED_EVENT \n");
        rsi_ble_app_clear_event(RSI_BLE_ENCRYPT_STARTED_EVENT);
        already_paired = 0;
        break;

      case RSI_APP_EVENT_CONNECTED:
        LOG_EVENT("\nRSI_APP_EVENT_CONNECTED \n");
        rsi_ble_app_clear_event(RSI_APP_EVENT_CONNECTED);
        break;

      case RSI_APP_EVENT_PHY_UPDATE_COMPLETE:
        LOG_EVENT("\nRSI_APP_EVENT_PHY_UPDATE_COMPLETE \n");
        rsi_ble_app_clear_event(RSI_APP_EVENT_PHY_UPDATE_COMPLETE);
        break;

      case RSI_BLE_GATT_WRITE_EVENT:
        //! event invokes when write/notification events received
        //! clear the served event
        LOG_EVENT("\nRSI_BLE_GATT_WRITE_EVENT \n");
        rsi_ble_app_clear_event(RSI_BLE_GATT_WRITE_EVENT);

        LOG_EVENT("\n rsi_ble_att_val_hndl = %d,  *((uint16_t *)app_ble_write_event.handle) = %d\n",
                  rsi_ble_att_val_hndl,
                  *((uint16_t *)app_ble_write_event.handle));

        if (rsi_ble_att_val_hndl == *((uint16_t *)app_ble_write_event.handle)) {
          LOG_EVENT("\n Preparing and setting the data as local attribute value\n");
          //! prepare the data to set as local attribute value.
          memset(ble_gatt_data, 0, sizeof(ble_gatt_data));
          memcpy(ble_gatt_data, app_ble_write_event.att_value, app_ble_write_event.length);

          rsi_ble_get_local_att_value(rsi_ble_att_val_hndl, &local_att_val);
          //! set the local attribute value.
          rsi_ble_set_local_att_value(rsi_ble_att_val_hndl, RSI_BLE_MAX_DATA_LEN, (uint8_t *)ble_gatt_data);
          rsi_ble_get_local_att_value(rsi_ble_att_val_hndl, &local_att_val);
        }
        if (rsi_ble_att_val_hndl == *((uint16_t *)app_ble_write_event.handle) - 1) {
          //This block is executed when Notification are turned on/off by remote device
          //check for valid notifications
          if (app_ble_write_event.att_value[0] == NOTIFY_ENABLE) {
            notification_tx_handle = *((uint16_t *)app_ble_write_event.handle) - 1;
            LOG_EVENT("\n Remote device enabled the notification\n");
          } else if (app_ble_write_event.att_value[0] == NOTIFY_DISABLE) {
            notification_tx_handle = 0;
            LOG_EVENT("\n Remote device disabled the notification\n");
          }
        }
        break;

      default: {
      }
    }
  }
}

sl_status_t init_ble_command(console_args_t *arguments)
{
  if (ble_task_handle != NULL) {
    // already initialized
    return (RSI_ERROR_NONE);
  }

  g_ble_capability = sl_cli_get_argument_uint8(arguments, 0);
  g_bond_info      = sl_cli_get_argument_uint8(arguments, 1);
  g_smp            = sl_cli_get_argument_uint8(arguments, 2);

  ble_thread_sem = osSemaphoreNew(1, 0, 0);
  if (ble_thread_sem == NULL) {
    return SL_STATUS_FAIL;
  }

  //set default role to master
  current_ble_role = BLE_MASTER_ROLE;

  const osThreadAttr_t attr = {
    .name       = "ble_task",
    .priority   = RSI_BLE_TASK_PRIORITY,
    .stack_mem  = 0,
    .stack_size = RSI_BLE_TASK_STACK_SIZE,
    .cb_mem     = 0,
    .cb_size    = 0,
    .attr_bits  = 0u,
    .tz_module  = 0u,
  };

  ble_task_handle = osThreadNew(ble_task, NULL, &attr);
  if (ble_task_handle == NULL) {
    return SL_STATUS_FAIL;
  }
  //! registering the GAP callback functions
  rsi_ble_gap_register_callbacks(rsi_ble_simple_central_on_adv_report_event,
                                 rsi_ble_on_connect_event,
                                 rsi_ble_on_disconnect_event,
                                 NULL,
                                 rsi_ble_phy_set_update_complete_event,
                                 rsi_ble_data_length_change_event,
                                 rsi_ble_on_enhance_conn_status_event,
                                 NULL,
                                 rsi_ble_update_conn_param_complete_event,
                                 NULL);

  //! registering the SMP callback functions
  rsi_ble_smp_register_callbacks(rsi_ble_on_smp_request,
                                 rsi_ble_on_smp_response,
                                 rsi_ble_on_smp_passkey,
                                 rsi_ble_on_smp_failed,
                                 rsi_ble_on_encrypt_started,
                                 rsi_ble_on_smp_passkey_display,
                                 rsi_ble_on_sc_passkey,
                                 rsi_ble_on_le_ltk_req_event,
                                 rsi_ble_on_le_security_keys,
                                 NULL,
                                 NULL);

  //! registering the GATT callback functions
  rsi_ble_gatt_register_callbacks(NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  rsi_ble_on_gatt_write_event,
                                  NULL,
                                  NULL,
                                  NULL,
                                  rsi_ble_on_mtu_event,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL);
  rsi_ble_gatt_extended_register_callbacks(rsi_ble_on_event_mtu_exchange_info);
  rsi_ble_adv_ext_events_register_callbacks(RSI_BLE_ON_ADV_EXT_ADVERTISE_REPORT_EVENT,
                                            (void *)rsi_ble_simple_central_on_ae_adv_report_event);
  rsi_ble_adv_ext_events_register_callbacks(RSI_BLE_ON_ADV_EXT_PERIODIC_ADV_SYNC_ESTBL_EVENT,
                                            (void *)rsi_ble_simple_central_on_ae_periodic_adv_sync_estbl_event);
  rsi_ble_adv_ext_events_register_callbacks(RSI_BLE_ON_ADV_EXT_PERIODIC_ADVERTISE_REPORT_EVENT,
                                            (void *)rsi_ble_simple_central_on_ae_periodic_adv_report_event);
  rsi_ble_adv_ext_events_register_callbacks(RSI_BLE_ON_ADV_EXT_PERIODIC_ADV_SYNC_LOST_EVENT,
                                            (void *)rsi_ble_simple_central_on_ae_periodic_adv_sync_lost_event);
  rsi_ble_adv_ext_events_register_callbacks(RSI_BLE_ON_ADV_EXT_SCAN_TIMEOUT_EVENT,
                                            (void *)rsi_ble_simple_central_on_ae_scan_timeout_event);
  rsi_ble_adv_ext_events_register_callbacks(RSI_BLE_ON_ADV_EXT_ADVERTISE_SET_TERMINATED_EVENT,
                                            (void *)rsi_ble_simple_central_on_ae_adv_set_terminated_event);
  rsi_ble_adv_ext_events_register_callbacks(RSI_BLE_ON_ADV_EXT_SCAN_REQUEST_RECEIVED_EVENT,
                                            (void *)rsi_ble_simple_central_on_ae_scan_req_recvd_event);
  //!  initializing the application events map
  rsi_ble_app_init_events();

  return SL_STATUS_OK;
}

sl_status_t rsi_ble_ae_set_scan_params_command(console_args_t *arguments)
{
  rsi_ble_ae_set_scan_params_t ae_set_scan_params          = { 0 };
  ae_set_scan_params.own_addr_type                         = sl_cli_get_argument_uint8(arguments, 0);
  ae_set_scan_params.scanning_filter_policy                = sl_cli_get_argument_uint8(arguments, 1);
  ae_set_scan_params.scanning_phys                         = sl_cli_get_argument_uint8(arguments, 2);
  ae_set_scan_params.ScanParams[AE_SCAN_SET1].ScanInterval = sl_cli_get_argument_uint16(arguments, 3);
  ae_set_scan_params.ScanParams[AE_SCAN_SET1].ScanWindow   = sl_cli_get_argument_uint16(arguments, 4);
  if (SET2_ENABLE) {
    ae_set_scan_params.ScanParams[AE_SCAN_SET2].ScanType     = sl_cli_get_argument_uint16(arguments, 5);
    ae_set_scan_params.ScanParams[AE_SCAN_SET2].ScanInterval = sl_cli_get_argument_uint16(arguments, 3);
    ae_set_scan_params.ScanParams[AE_SCAN_SET2].ScanWindow   = sl_cli_get_argument_uint16(arguments, 4);
  }
  int32_t status = rsi_ble_ae_set_scan_params(&ae_set_scan_params);
  return status;
}

sl_status_t rsi_ble_ae_set_scan_enable_command(console_args_t *arguments)
{
  rsi_ble_ae_set_scan_enable_t ae_set_scan_enable = { 0 };
  ae_set_scan_enable.enable                       = sl_cli_get_argument_uint8(arguments, 0);
  ae_set_scan_enable.filter_duplicates            = sl_cli_get_argument_uint8(arguments, 1);
  ae_set_scan_enable.duration                     = sl_cli_get_argument_uint8(arguments, 2);
  ae_set_scan_enable.period                       = sl_cli_get_argument_uint8(arguments, 3);
  int32_t status                                  = rsi_ble_ae_set_scan_enable(&ae_set_scan_enable);
  return status;
}

sl_status_t rsi_ble_set_ae_params_command(console_args_t *arguments)
{
  rsi_ble_ae_adv_params_t ble_ae_params    = { 0 };
  uint8_t device_address[RSI_DEV_ADDR_LEN] = { 0 };
  ble_ae_params.adv_handle                 = sl_cli_get_argument_uint8(arguments, 0);
  ble_ae_params.adv_event_prop             = sl_cli_get_argument_uint8(arguments, 1);
  ble_ae_params.primary_adv_intterval_min  = sl_cli_get_argument_uint8(arguments, 2);
  ble_ae_params.primary_adv_intterval_max  = sl_cli_get_argument_uint8(arguments, 3);
  ble_ae_params.primary_adv_chnl_map       = sl_cli_get_argument_uint8(arguments, 4);
  ble_ae_params.own_addr_type              = sl_cli_get_argument_uint8(arguments, 5);
  ble_ae_params.peer_addr_type             = sl_cli_get_argument_uint8(arguments, 6);
  char *remote_dev_addr                    = sl_cli_get_argument_string(arguments, 7);
  rsi_ascii_dev_address_to_6bytes_rev(device_address, (int8_t *)remote_dev_addr);
  memcpy(ble_ae_params.peer_dev_addr, device_address, RSI_DEV_ADDR_LEN);
  ble_ae_params.adv_filter_policy      = sl_cli_get_argument_uint8(arguments, 8);
  ble_ae_params.adv_tx_power           = sl_cli_get_argument_uint8(arguments, 9);
  ble_ae_params.primary_adv_phy        = sl_cli_get_argument_uint8(arguments, 10);
  ble_ae_params.sec_adv_max_skip       = sl_cli_get_argument_uint8(arguments, 11);
  ble_ae_params.sec_adv_phy            = sl_cli_get_argument_uint8(arguments, 12);
  ble_ae_params.adv_sid                = sl_cli_get_argument_uint8(arguments, 13);
  ble_ae_params.scan_req_notify_enable = sl_cli_get_argument_uint8(arguments, 14);
  int32_t status                       = rsi_ble_set_ae_params(&ble_ae_params, &rsi_app_resp_tx_power);
  printf("\nstatus = %ld", status);
  return status;
}

sl_status_t rsi_ble_start_ae_advertising_command(console_args_t *arguments)
{
  rsi_ble_ae_adv_enable_t ble_ae_adv = { 0 };
  ble_ae_adv.enable                  = sl_cli_get_argument_uint8(arguments, 0);
  ble_ae_adv.no_of_sets              = 1;
  ble_ae_adv.adv_handle              = sl_cli_get_argument_uint8(arguments, 1);
  ble_ae_adv.duration                = 0;
  ble_ae_adv.max_ae_events           = sl_cli_get_argument_uint8(arguments, 2);
  int32_t status                     = rsi_ble_start_ae_advertising(&ble_ae_adv);
  printf("\nstatus = %ld", status);
  return status;
}

sl_status_t rsi_ble_set_ae_set_random_address_command(console_args_t *arguments)
{
  uint8_t adv_handle                  = sl_cli_get_argument_uint8(arguments, 0);
  char *remote_dev_addr               = sl_cli_get_argument_string(arguments, 1);
  uint8_t rand_addr[RSI_DEV_ADDR_LEN] = { 0 };
  rsi_ascii_dev_address_to_6bytes_rev(rand_addr, (int8_t *)remote_dev_addr);
  int32_t status = rsi_ble_set_ae_set_random_address(adv_handle, rand_addr);
  return status;
}

sl_status_t rsi_ble_set_ae_data_command(console_args_t *arguments)
{
  rsi_ble_ae_data_t ble_ae_data     = { 0 };
  uint8_t data[BLE_AE_ADV_DATA_LEN] = { 0 };
  ble_ae_data.type                  = sl_cli_get_argument_uint8(arguments, 0);
  ble_ae_data.adv_handle            = sl_cli_get_argument_uint8(arguments, 1);
  ble_ae_data.operation             = 0;
  ble_ae_data.frag_pref             = 0;
  ble_ae_data.data_len              = sl_cli_get_argument_uint8(arguments, 2);
  strncpy((char *)data, BLE_AE_ADV_DATA, BLE_AE_ADV_DATA_LEN);
  memcpy(ble_ae_data.data, data, ble_ae_data.data_len);
  int32_t status = rsi_ble_set_ae_data(&ble_ae_data);
  printf("\nstatus = %ld", status);
  return status;
}

sl_status_t rsi_ble_start_scanning_command(console_args_t *arguments)
{
  rsi_ble_req_scan_t ble_scan = { 0 };
  printf("\r\n rsi_ble_start_scanning_command invoked \r\n");
  char *remote_dev_addr  = sl_cli_get_argument_string(arguments, 0);
  ble_scan.scan_type     = sl_cli_get_argument_uint8(arguments, 1);
  ble_scan.filter_type   = sl_cli_get_argument_uint8(arguments, 2);
  ble_scan.scan_int      = sl_cli_get_argument_uint16(arguments, 3);
  ble_scan.scan_win      = sl_cli_get_argument_uint16(arguments, 4);
  ble_scan.own_addr_type = sl_cli_get_argument_uint8(arguments, 5);
  memcpy(target_ble_remote_dev_addr, remote_dev_addr, sizeof(target_ble_remote_dev_addr));
  device_found   = 0;
  int32_t status = rsi_ble_start_scanning_with_values(&ble_scan);
  //This assignment may change when we implement dual role
  if (status == RSI_SUCCESS) {
    current_ble_role = BLE_MASTER_ROLE;
  }
  return status;
}

sl_status_t rsi_ble_white_list_using_adv_data_comamnd(console_args_t *arguments)
{
  uint8_t adv_payload_for_compare[31];
  memset(adv_payload_for_compare, 0, 31);
  uint8_t enable              = sl_cli_get_argument_uint8(arguments, 0);
  uint8_t data_compare_index  = sl_cli_get_argument_uint8(arguments, 1);
  uint8_t compare_data_length = sl_cli_get_argument_uint8(arguments, 2);
  char *payload               = sl_cli_get_argument_string(arguments, 3);
  uint8_t counter             = 0;
  for (uint8_t index = data_compare_index; index < (data_compare_index + compare_data_length); index++) {
    sscanf(payload + counter * 3, "%02hhd", &adv_payload_for_compare[index]);
    counter++;
  }
  return (rsi_ble_accept_list_using_adv_data(enable, data_compare_index, compare_data_length, adv_payload_for_compare));
}

sl_status_t rsi_ble_add_white_list_comamnd(console_args_t *arguments)
{
  char *remote_dev_addr    = sl_cli_get_argument_string(arguments, 0);
  uint8_t remote_addr_type = sl_cli_get_argument_uint8(arguments, 1);
  uint8_t device_address[6];
  rsi_ascii_dev_address_to_6bytes_rev(device_address, (int8_t *)remote_dev_addr);
  return (rsi_ble_addto_acceptlist((int8_t *)device_address, remote_addr_type));
}

sl_status_t rsi_ble_stop_scanning_command(console_args_t *arguments)
{
  (void)arguments;
  return (rsi_ble_stop_scanning());
}

sl_status_t rsi_ble_connect_comamnd(console_args_t *arguments)
{
  char *remote_dev_addr     = sl_cli_get_argument_string(arguments, 0);
  g_le_scan_interval        = sl_cli_get_argument_uint16(arguments, 1);
  g_le_scan_window          = sl_cli_get_argument_uint16(arguments, 2);
  g_connection_interval_min = sl_cli_get_argument_uint16(arguments, 3);
  g_connection_interval_max = sl_cli_get_argument_uint16(arguments, 4);
  g_connection_latency      = sl_cli_get_argument_uint16(arguments, 5);
  g_supervision_timeout     = sl_cli_get_argument_uint16(arguments, 6);

  uint8_t device_address[6];
  rsi_ascii_dev_address_to_6bytes_rev(device_address, (int8_t *)remote_dev_addr);
  return (rsi_ble_connect_with_params(remote_addr_type,
                                      (int8_t *)device_address,
                                      g_le_scan_interval,
                                      g_le_scan_window,
                                      g_connection_interval_max,
                                      g_connection_interval_min,
                                      g_connection_latency,
                                      g_supervision_timeout));
}

sl_status_t rsi_ble_disconnect_comamnd(console_args_t *arguments)
{
  char *remote_dev_addr = sl_cli_get_argument_string(arguments, 0);

  uint8_t ble_device_address[6] = { 0 };
  // str_to_mac_address(remote_dev_addr, device_address);
  //    for (int i = 0; i < 6; i++) {
  //        int val;
  //        sscanf(remote_dev_addr + 3 * i, "%02X", &val);
  //        device_address[5-i] = (uint8_t)val;
  //    }
  return (
    rsi_ble_disconnect((int8_t *)rsi_ascii_dev_address_to_6bytes_rev(ble_device_address, (int8_t *)remote_dev_addr)));
}

sl_status_t rsi_ble_get_device_state_command(console_args_t *arguments)
{
  (void)arguments;
  int32_t status = rsi_ble_get_device_state(&rsi_app_resp_device_state);
  if (status == RSI_SUCCESS) {
    printf("\n%d\n", rsi_app_resp_device_state);
  }
  return (status);
}

sl_status_t set_ble_params_command(console_args_t *arguments)
{
  g_rsi_ble_pwr_inx = sl_cli_get_argument_uint8(arguments, 0);
  return (RSI_ERROR_NONE);
}

sl_status_t rsi_ble_update_conn_params_comamnd(console_args_t *arguments)
{
  char *remote_dev_addr        = sl_cli_get_argument_string(arguments, 0);
  uint16_t min_int             = sl_cli_get_argument_uint16(arguments, 1);
  uint16_t max_int             = sl_cli_get_argument_uint16(arguments, 2);
  uint16_t latency             = sl_cli_get_argument_uint16(arguments, 3);
  uint16_t supervision_timeout = sl_cli_get_argument_uint16(arguments, 4);

  uint8_t device_address[6];
  rsi_ascii_dev_address_to_6bytes_rev(device_address, (int8_t *)remote_dev_addr);
  return (rsi_ble_conn_params_update(device_address, min_int, max_int, latency, supervision_timeout));
}

sl_status_t rsi_set_ble_duty_cycle_command(console_args_t *arguments)
{
  g_ble_power_save_options = sl_cli_get_argument_uint8(arguments, 0);
  return (RSI_ERROR_NONE);
}

sl_status_t rsi_ble_set_address(console_args_t *arguments)
{
  uint8_t device_address[6];
  char *device_addr_str = sl_cli_get_argument_string(arguments, 0);
  //for (int i = 0; i < 6; i++) {
  //    int val;
  //    sscanf(device_addr_str + 3 * i, "%02X", &val);
  //    device_address[5-i] = (uint8_t)val;
  //}

  convert_string_to_mac_address(device_addr_str, (sl_mac_address_t *)&device_address);
  int32_t status = rsi_ble_set_random_address_with_value(device_address);
  if (status == RSI_SUCCESS) {
    status = rsi_bt_set_bd_addr(device_address);
  }
  return status;
}

sl_status_t rsi_ble_start_advertising_command(console_args_t *arguments)
{
  char *data = sl_cli_get_argument_string(arguments, 0);

  rsi_ble_req_adv_t ble_adv = { 0 };
  ble_adv.status            = 0x01;
  ble_adv.adv_type          = sl_cli_get_argument_uint8(arguments, 1);
  ble_adv.filter_type       = sl_cli_get_argument_uint8(arguments, 2);
  ble_adv.direct_addr_type  = sl_cli_get_argument_uint8(arguments, 3);

  rsi_ascii_dev_address_to_6bytes_rev(ble_adv.direct_addr, (int8_t *)sl_cli_get_argument_string(arguments, 4));

  ble_adv.adv_int_min     = sl_cli_get_argument_uint16(arguments, 5);
  ble_adv.adv_int_max     = sl_cli_get_argument_uint16(arguments, 6);
  ble_adv.own_addr_type   = sl_cli_get_argument_uint8(arguments, 7);
  ble_adv.adv_channel_map = sl_cli_get_argument_uint8(arguments, 8);
  uint8_t ibeacon_info    = sl_cli_get_argument_uint8(arguments, 9);

  if (!(strcmp(data, "NULL"))) {
    memset(data, 0, strlen(data));
  }

  const uint8_t data_length = strlen(data);
  uint8_t adv[64]           = { 2, 1, 6 };

  // prepare advertise data //local/device name
  adv[3] = data_length + 1;
  adv[4] = 9;
  strcpy((char *)&adv[5], data);

  // set advertise data
  if (ibeacon_info) {
    //prefix(9bytes)
    adv[0]           = 0x02;
    adv[1]           = 0x01;
    adv[2]           = 0x02;
    adv[3]           = 0x1A;
    adv[4]           = 0xFF;
    adv[5]           = 0x4C;
    adv[6]           = 0x00;
    adv[7]           = 0x02;
    adv[8]           = 0x15;
    uint8_t uuid[16] = {
      0xFB, 0x0B, 0x57, 0xA2, 0x82, 0x28, 0x44, 0xCD, 0x91, 0x3A, 0x94, 0xA1, 0x22, 0xBA, 0x12, 0x06
    };
    uint8_t major_num[2] = { 0x11, 0x22 };
    uint8_t minor_num[2] = { 0x33, 0x44 };
    uint8_t tx_power     = 0x33;
    //! memcpy the uuid value
    memcpy(&adv[9], uuid, 16);
    //! memcpy the major_number value
    memcpy(&adv[9 + 16], major_num, 2);
    //! memcpy the minor_number value
    memcpy(&adv[9 + 16 + 2], minor_num, 2);
    //! memcpy the minor_number value
    adv[9 + 16 + 2 + 2] = tx_power;
  }
  int32_t status = 0;
  if (ibeacon_info)
    status = rsi_ble_set_advertise_data(adv, 30);
  else
    status = rsi_ble_set_advertise_data(adv, data_length + 5);

  // set device in advertising mode.
  if (status == RSI_SUCCESS) {
    status = rsi_ble_start_advertising_with_values(&ble_adv);
    //This assignment may have to be changed when we implement dual role
    if (status == RSI_SUCCESS) {
      current_ble_role = BLE_SLAVE_ROLE;
    }
  }

  return status;
}

sl_status_t rsi_ble_stop_advertising_command(console_args_t *arguments)
{
  (void)arguments;
  int32_t status = rsi_ble_stop_advertising();
  return status;
}

sl_status_t rsi_ble_add_service_command(console_args_t *arguments)
{
  uint16_t uuid = sl_cli_get_argument_uint16(arguments, 0);

  //! adding new service
  uuid_t new_uuid    = { 0 };
  new_uuid.size      = 2;
  new_uuid.val.val16 = uuid;
  printf("\r\n rsi_ble_add_service_command invoked \r\n");
  return (rsi_ble_add_service(new_uuid, &new_serv_resp));
}

int32_t rsi_ble_notify_value_internal()
{
  if (notification_tx_handle == 0) {
    LOG_EVENT("\nNotifications are turned off on remote device.\n");
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  int32_t status =
    rsi_ble_notify_value(ble_remote_dev_addr, notification_tx_handle, sizeof(ble_gatt_data), (uint8_t *)ble_gatt_data);
  return status;
}

int32_t rsi_ble_notify_value_internal_custom(uint8_t custom_feature, uint8_t length)
{
  uint8_t data_length = 0;
  if (notification_tx_handle == 0) {
    LOG_EVENT("\nNotifications are turned off on remote device.\n");
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  if (custom_feature) {
    data_length = length;
  } else {
    data_length = sizeof(ble_gatt_data);
  }
  int32_t status =
    rsi_ble_notify_value(ble_remote_dev_addr, notification_tx_handle, data_length, (uint8_t *)ble_gatt_data);
  return status;
}

sl_status_t rsi_ble_set_adv_data_command(console_args_t *arguments)
{
  char *data           = sl_cli_get_argument_string(arguments, 0);
  uint16_t data_length = sl_cli_get_argument_uint16(arguments, 1);
  if (!(strcmp(data, "NULL"))) {
    memset(data, 0, strlen(data));
  }

  data_length     = strlen(data);
  uint8_t adv[64] = { 2, 1, 6 };
  // prepare advertise data //local/device name
  adv[3] = data_length + 1;
  adv[4] = 9;
  strcpy((char *)&adv[5], data);

  //prefix(9bytes)
  adv[0]           = 0x02;
  adv[1]           = 0x01;
  adv[2]           = 0x02;
  adv[3]           = 0x1A;
  adv[4]           = 0xFF;
  adv[5]           = 0x4C;
  adv[6]           = 0x00;
  adv[7]           = 0x02;
  adv[8]           = 0x15;
  uint8_t uuid[16] = { 0xFB, 0x0B, 0x57, 0xA2, 0x82, 0x28, 0x44, 0xCD, 0x91, 0x3A, 0x94, 0xA1, 0x22, 0xBA, 0x12, 0x06 };
  uint8_t major_num[2] = { 0x11, 0x22 };
  uint8_t minor_num[2] = { 0x33, 0x44 };
  uint8_t tx_power     = 0x33;
  //! memcpy the uuid value
  memcpy(&adv[9], uuid, 16);
  //! memcpy the major_number value
  memcpy(&adv[9 + 16], major_num, 2);
  //! memcpy the minor_number value
  memcpy(&adv[9 + 16 + 2], minor_num, 2);
  //! memcpy the minor_number value
  adv[9 + 16 + 2 + 2] = tx_power;

  int32_t status = 0;
  status         = rsi_ble_set_advertise_data(adv, data_length + 5);
  return status;
}

sl_status_t rsi_ble_notify_value_command(console_args_t *arguments)
{
  (void)arguments;
  return (rsi_ble_notify_value_internal());
}

sl_status_t rsi_ble_custom_notify_value_command(console_args_t *arguments)
{
  char *send_data        = sl_cli_get_argument_string(arguments, 0);
  uint8_t custom_feature = sl_cli_get_argument_uint8(arguments, 1);
  uint8_t length         = sl_cli_get_argument_uint8(arguments, 2);
  if (send_data) {
    memcpy(ble_gatt_data, send_data, sizeof(ble_gatt_data));
  }
  return rsi_ble_notify_value_internal_custom(custom_feature, length);
}

sl_status_t rsi_ble_start_notify_value_command(console_args_t *arguments)
{
  uint16_t time_out_ms = sl_cli_get_argument_uint16(arguments, 0);
  uint16_t count       = sl_cli_get_argument_uint16(arguments, 1);
  if (notification_tx_handle == 0) {
    LOG_PRINT("\nNotifications are turned off on remote device.\n");
    return (RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE);
  }
  int32_t status = 0;
  while (count > 0) {
    status = rsi_ble_notify_value_internal();
    if (status != RSI_ERROR_NONE) {
      break;
    }
    osDelay(time_out_ms);
    count--;
  }
  return status;
}

sl_status_t rsi_ble_add_char_serv_att_command(console_args_t *arguments)
{
  uint16_t uuid            = sl_cli_get_argument_uint16(arguments, 0);
  uint8_t val_prop         = sl_cli_get_argument_uint8(arguments, 1);
  uint16_t serv_handle_num = sl_cli_get_argument_uint8(arguments, 2);
  uint16_t val_handle_num  = sl_cli_get_argument_uint8(arguments, 3);

  //! adding characteristic service attribute to the service
  uuid_t new_uuid    = { 0 };
  new_uuid.size      = 2;
  new_uuid.val.val16 = uuid;
  rsi_ble_add_char_serv_att(new_serv_resp.serv_handler,
                            new_serv_resp.start_handle + serv_handle_num,
                            val_prop,
                            new_serv_resp.start_handle + val_handle_num,
                            new_uuid);
  return SL_STATUS_OK;
}

sl_status_t rsi_ble_add_char_val_att_command(console_args_t *arguments)
{
  uint16_t uuid                 = sl_cli_get_argument_uint16(arguments, 0);
  uint8_t val_prop              = sl_cli_get_argument_uint8(arguments, 1);
  uint16_t val_handle_num       = sl_cli_get_argument_uint8(arguments, 2);
  char *data                    = sl_cli_get_argument_string(arguments, 3);
  uint8_t enable_custom_feature = sl_cli_get_argument_uint8(arguments, 4);
  uint8_t custom_feature        = sl_cli_get_argument_uint8(arguments, 5);

  memset(ble_gatt_data, 0, sizeof(ble_gatt_data));
  memcpy(ble_gatt_data, data, strlen(data));
  temp_measurement_t measurement = { 0, 27.6 };
  //! adding characteristic value attribute to the service
  uuid_t new_uuid      = { 0 };
  rsi_ble_att_val_hndl = new_serv_resp.start_handle + val_handle_num;
  new_uuid.size        = 2;
  new_uuid.val.val16   = uuid;
  if (!enable_custom_feature) {
    rsi_ble_add_char_val_att(new_serv_resp.serv_handler,
                             new_serv_resp.start_handle + val_handle_num,
                             new_uuid,
                             val_prop,
                             (uint8_t *)ble_gatt_data,
                             sizeof(ble_gatt_data));

  } else {
    rsi_ble_add_char_val_att(new_serv_resp.serv_handler,
                             new_serv_resp.start_handle + val_handle_num,
                             new_uuid,
                             val_prop,
                             (uint8_t *)&custom_feature,
                             sizeof(custom_feature));
  }
  if (uuid == 10780) {
    rsi_ble_add_char_val_att(new_serv_resp.serv_handler,
                             new_serv_resp.start_handle + val_handle_num,
                             new_uuid,
                             val_prop,
                             (uint8_t *)&measurement,
                             sizeof(measurement));
  }
  return SL_STATUS_OK;
}

//get profiles async command written for multi connection gatt example
sl_status_t rsi_ble_get_profiles_async_command(console_args_t *arguments)
{
  char *remote_dev_bd_addr = sl_cli_get_argument_string(arguments, 0);

  uint8_t rsi_connected_dev_addr[6] = { 0 };
  rsi_ascii_dev_address_to_6bytes_rev(rsi_connected_dev_addr, (int8_t *)remote_dev_bd_addr);
  return (rsi_ble_get_profiles_async(rsi_connected_dev_addr, 1, 0xffff, NULL));
}

sl_status_t rsi_ble_get_profile_async_command(console_args_t *arguments)
{

  char *remote_dev_bd_addr = sl_cli_get_argument_string(arguments, 0);

  uuid_t search_serv    = { 0 };
  search_serv.size      = 2;
  search_serv.val.val16 = sl_cli_get_argument_uint16(arguments, 1);

  uint8_t rsi_connected_dev_addr[6] = { 0 };
  rsi_ascii_dev_address_to_6bytes_rev(rsi_connected_dev_addr, (int8_t *)remote_dev_bd_addr);
  return (rsi_ble_get_profile_async(rsi_connected_dev_addr, search_serv, NULL));
}

sl_status_t rsi_ble_get_char_services_async_command(console_args_t *arguments)
{
  char *remote_dev_bd_addr = sl_cli_get_argument_string(arguments, 0);
  uint16_t start_handle    = sl_cli_get_argument_uint16(arguments, 1);
  uint16_t end_handle      = sl_cli_get_argument_uint16(arguments, 2);

  uint8_t rsi_connected_dev_addr[6] = { 0 };
  rsi_ascii_dev_address_to_6bytes_rev(rsi_connected_dev_addr, (int8_t *)remote_dev_bd_addr);

  //! Get characteristic services of searched profile
  uint8_t status = rsi_ble_get_char_services_async(rsi_connected_dev_addr, start_handle, end_handle, NULL);
  return status;
}

sl_status_t rsi_ble_get_att_value_async_command(console_args_t *arguments)
{
  char *remote_dev_bd_addr = sl_cli_get_argument_string(arguments, 0);
  uint16_t handle          = sl_cli_get_argument_uint16(arguments, 1);

  uint8_t rsi_connected_dev_addr[6] = { 0 };
  rsi_ascii_dev_address_to_6bytes_rev(rsi_connected_dev_addr, (int8_t *)remote_dev_bd_addr);

  //! Get characteristic services of searched profile
  return (rsi_ble_get_att_value_async(rsi_connected_dev_addr, handle, NULL));
}

sl_status_t rsi_ble_set_att_value_async_command(console_args_t *arguments)
{
  char *remote_dev_bd_addr = sl_cli_get_argument_string(arguments, 0);
  uint16_t handle          = sl_cli_get_argument_uint16(arguments, 1);
  uint8_t data             = sl_cli_get_argument_uint8(arguments, 2);

  uint8_t data_to_send[1] = { data };

  uint8_t rsi_connected_dev_addr[6] = { 0 };
  rsi_ascii_dev_address_to_6bytes_rev(rsi_connected_dev_addr, (int8_t *)remote_dev_bd_addr);
  return (rsi_ble_set_att_value_async(rsi_connected_dev_addr, handle, sizeof(data_to_send), (uint8_t *)data_to_send));
}

/*==============================================*/
/**
 * @fn         rsi_ble_add_char_serv_att
 * @brief      this function is used to add characteristic service attribute..
 * @param[in]  serv_handler, service handler.
 * @param[in]  handle, characteristic service attribute handle.
 * @param[in]  val_prop, characteristic value property.
 * @param[in]  att_val_handle, characteristic value handle
 * @param[in]  att_val_uuid, characteristic value uuid
 * @return     none.
 * @section description
 * This function is used at application to add characteristic attribute
 */
void rsi_ble_add_char_serv_att(void *serv_handler,
                               uint16_t handle,
                               uint8_t val_prop,
                               uint16_t att_val_handle,
                               uuid_t att_val_uuid)
{
  rsi_ble_req_add_att_t new_att = { 0 };

  //! preparing the attribute service structure
  new_att.serv_handler       = serv_handler;
  new_att.handle             = handle;
  new_att.att_uuid.size      = 2;
  new_att.att_uuid.val.val16 = RSI_BLE_CHAR_SERV_UUID;
  new_att.property           = RSI_BLE_ATT_PROPERTY_READ;

  //! preparing the characteristic attribute value
  new_att.data_len = 6;
  new_att.data[0]  = val_prop;
  rsi_uint16_to_2bytes(&new_att.data[2], att_val_handle);
  rsi_uint16_to_2bytes(&new_att.data[4], att_val_uuid.val.val16);

  //! Add attribute to the service
  rsi_ble_add_attribute(&new_att);

  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_add_char_val_att
 * @brief      this function is used to add characteristic value attribute.
 * @param[in]  serv_handler, new service handler.
 * @param[in]  handle, characteristic value attribute handle.
 * @param[in]  att_type_uuid, attribute uuid value.
 * @param[in]  val_prop, characteristic value property.
 * @param[in]  data, characteristic value data pointer.
 * @param[in]  data_len, characteristic value length.
 * @return     none.
 * @section description
 * This function is used at application to create new service.
 */

void rsi_ble_add_char_val_att(void *serv_handler,
                              uint16_t handle,
                              uuid_t att_type_uuid,
                              uint8_t val_prop,
                              uint8_t *data,
                              uint8_t data_len)
{
  rsi_ble_req_add_att_t new_att = { 0 };

  //! preparing the attributes
  new_att.serv_handler = serv_handler;
  new_att.handle       = handle;
  memcpy(&new_att.att_uuid, &att_type_uuid, sizeof(uuid_t));
  new_att.property = val_prop;

  //! preparing the attribute value
  new_att.data_len = RSI_MIN(sizeof(new_att.data), data_len);
  memcpy(new_att.data, data, new_att.data_len);

  //! add attribute to the service
  rsi_ble_add_attribute(&new_att);

  //! check the attribute property with notification
  if (val_prop & RSI_BLE_ATT_PROPERTY_NOTIFY) {
    //! if notification property supports then we need to add client characteristic service.

    //! preparing the client characteristic attribute & values
    memset(&new_att, 0, sizeof(rsi_ble_req_add_att_t));
    new_att.serv_handler       = serv_handler;
    new_att.handle             = handle + 1;
    new_att.att_uuid.size      = 2;
    new_att.att_uuid.val.val16 = RSI_BLE_CLIENT_CHAR_UUID;
    new_att.property           = RSI_BLE_ATT_PROPERTY_READ | RSI_BLE_ATT_PROPERTY_WRITE;
    new_att.data_len           = 2;

    //! add attribute to the service
    rsi_ble_add_attribute(&new_att);
  }

  return;
}

/**
 * @fn         rsi_ble_app_init_events
 * @brief      initializes the event parameter.
 * @param[in]  none.
 * @return     none.
 * @section description
 * This function is used during BLE initialization.
 */
static void rsi_ble_app_init_events()
{
  rsi_app_async_event_map = 0;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_set_event
 * @brief      set the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to set/raise the specific event.
 */
static void rsi_ble_app_set_event(uint32_t event_num)
{
  rsi_app_async_event_map |= BIT(event_num);
  osSemaphoreRelease(ble_thread_sem);
  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_clear_event
 * @brief      clear the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to clear the specific event.
 */
static void rsi_ble_app_clear_event(uint32_t event_num)
{
  rsi_app_async_event_map &= ~BIT(event_num);
  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_get_event
 * @brief      returns the first set event based on priority
 * @param[in]  none.
 * @return     int32_t
 *             > 0  = event number
 *             -1   = not received any event
 * @section description
 * This function returns the highest priority event among all the set events
 */
static int32_t rsi_ble_app_get_event(void)
{
  uint32_t ix;

  for (ix = 0; ix < 32; ix++) {
    if (rsi_app_async_event_map & (1 << ix)) {
      return ix;
    }
  }

  return (-1);
}

/*==============================================*/
/**
 * @fn         rsi_ble_simple_central_on_adv_report_event
 * @brief      invoked when advertise report event is received
 * @param[in]  adv_report, pointer to the received advertising report
 * @return     none.
 * @section description
 * This callback function updates the scanned remote devices list
 */
void rsi_ble_simple_central_on_adv_report_event(rsi_ble_event_adv_report_t *adv_report)
{
  PRINT_EVENT_START(ADVERTISE, BLE)
  uint8_t count = 0;
  //    if (device_found == 1) {
  //        return;
  //    }

  // memset(remote_name, 0, 31);
  // BT_LE_ADPacketExtract(remote_name, adv_report->adv_data, adv_report->adv_data_len);

  // remote_addr_type = adv_report->dev_addr_type;
  memcpy(ble_remote_dev_addr, (uint8_t *)adv_report->dev_addr, RSI_DEV_ADDR_LEN);

  rsi_6byte_dev_address_to_ascii(ble_str_conn_bd_addr, adv_report->dev_addr);

  LOG_EVENT("\n Found : %s\n", ble_str_conn_bd_addr);
  if (strcmp((const char *)ble_str_conn_bd_addr, (const char *)target_ble_remote_dev_addr) == 0
      || strcmp((const char *)"None", (const char *)target_ble_remote_dev_addr) == 0) {
    LOG_EVENT("\n dev_addr: %s, report_type:%d,", ble_str_conn_bd_addr, adv_report->report_type);
  }
  if ((current_ble_role == BLE_MASTER_ROLE)
      && (strcmp((const char *)"None", (const char *)target_ble_remote_dev_addr) != 0)) {
    bool device_was_not_found_yet = device_found == 0;
    // bool found_desired_remote_name = strcmp(remote_name, target_remote_name) == 0;
    // bool found_desired_remote_type = remote_addr_type == target_ble_remote_dev_addr_type;
    bool found_desired_remote_addr =
      strcmp((const char *)ble_str_conn_bd_addr, (const char *)target_ble_remote_dev_addr) == 0;
    //if (device_was_not_found_yet && found_desired_remote_name || (found_desired_remote_type && found_desired_remote_addr)) {
    if (device_was_not_found_yet && found_desired_remote_addr) {
      device_found = 1;
      // LOG_EVENT("\nrsi_ble_simple_central_on_adv_report_event- dev_addr: %s, dev_addr_type: %s, remote_name: %s\n",
      //     ble_str_conn_bd_addr,
      //     remote_addr_type, remote_name);
      //            LOG_EVENT("\nrsi_ble_simple_central_on_adv_report_event- dev_addr: %s\n",
      //                ble_str_conn_bd_addr);
      // rsi_ble_app_set_event(RSI_APP_EVENT_ADV_REPORT);
      LOG_EVENT(
        "\n =====rsi_ble_simple_central_on_adv_report_event- dev_addr: %s, dev_addr_type: %d, adv_data_len: %d, "
        "rssi:%d, report_type:%d,",
        ble_str_conn_bd_addr,
        adv_report->dev_addr_type,
        adv_report->adv_data_len,
        adv_report->rssi,
        adv_report->report_type);

      LOG_EVENT("adv_data:");
      for (count = 0; count < RSI_MAX_ADV_REPORT_SIZE; count++) {
        LOG_EVENT("%d-", adv_report->adv_data[count]);
      }
      LOG_EVENT("\n");
      rsi_ble_app_set_event(RSI_APP_EVENT_ADV_REPORT);
      osDelay(10);
    }
  } else {
    LOG_EVENT("\n ===rsi_ble_simple_central_on_adv_report_event- dev_addr: %s, dev_addr_type: %d, adv_data_len: %d, "
              "rssi:%d, report_type:%d,",
              ble_str_conn_bd_addr,
              adv_report->dev_addr_type,
              adv_report->adv_data_len,
              adv_report->rssi,
              adv_report->report_type);
    LOG_EVENT("adv_data:");
    for (count = 0; count < RSI_MAX_ADV_REPORT_SIZE; count++) {
      LOG_EVENT("%d-", adv_report->adv_data[count]);
    }
    LOG_EVENT("\n");
    rsi_ble_app_set_event(RSI_APP_EVENT_ADV_REPORT);
    osDelay(10);
  }
  PRINT_EVENT_END();
}

/*==============================================*/
/**
 * @fn         rsi_ble_data_length_change_event
 * @brief      invoked when data length is set
  * @section description
 * This Callback function indicates data length is set
 */
void rsi_ble_data_length_change_event(rsi_ble_event_data_length_update_t *rsi_ble_data_length_update)
{
  LOG_EVENT("\nrsi_ble_data_length_change_event- dev_addr: %s\n",
            rsi_6byte_dev_address_to_ascii(ble_str_conn_bd_addr, rsi_ble_data_length_update->dev_addr));
  rsi_ble_app_set_event(RSI_BLE_DATA_LENGTH_CHANGE_EVENT);
}

void rsi_ble_phy_set_update_complete_event(rsi_ble_event_phy_update_t *rsi_ble_event_phy_update_complete)
{
  LOG_EVENT("\nrsi_ble_phy_set_update_complete_event- dev_addr: %s, TxPhy:%d, RxPhy:%d\n",
            rsi_6byte_dev_address_to_ascii(ble_str_conn_bd_addr, rsi_ble_event_phy_update_complete->dev_addr),
            rsi_ble_event_phy_update_complete->TxPhy,
            rsi_ble_event_phy_update_complete->RxPhy);
  rsi_ble_app_set_event(RSI_BLE_PHY_UPDATE_COMPLETE_EVENT);
}

void rsi_ble_update_conn_param_complete_event(rsi_ble_event_conn_update_t *rsi_ble_event_conn_update_complete,
                                              uint16_t resp_status)
{
  LOG_EVENT("\nrsi_ble_update_conn_param_complete_event- dev_addr: %s\n",
            rsi_6byte_dev_address_to_ascii(ble_str_conn_bd_addr, rsi_ble_event_conn_update_complete->dev_addr));
  rsi_ble_app_set_event(RSI_BLE_CONN_PARAM_UPDATE_COMPLETE_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_enhance_conn_status_event
 * @brief      invoked when enhanced connection complete event is received
 * @param[out] resp_conn, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */
void rsi_ble_on_enhance_conn_status_event(rsi_ble_event_enhance_conn_status_t *resp_enh_conn)
{
  memcpy((int8_t *)ble_remote_dev_addr, resp_enh_conn->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_EVENT("\nrsi_ble_on_enhance_conn_status_event- dev_addr: %s\n",
            rsi_6byte_dev_address_to_ascii(ble_str_conn_bd_addr, resp_enh_conn->dev_addr));
  rsi_ble_app_set_event(RSI_BLE_CONN_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_connect_event
 * @brief      invoked when connection complete event is received
 * @param[out] resp_conn, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */
static void rsi_ble_on_connect_event(rsi_ble_event_conn_status_t *resp_conn)
{
  memcpy((int8_t *)ble_remote_dev_addr, resp_conn->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_EVENT("\nrsi_ble_on_connect_event- dev_addr: %s\n",
            rsi_6byte_dev_address_to_ascii(ble_str_conn_bd_addr, resp_conn->dev_addr));

  rsi_ble_app_set_event(RSI_BLE_CONN_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_disconnect_event
 * @brief      invoked when disconnection event is received
 * @param[in]  resp_disconnect, disconnected remote device information
 * @param[in]  reason, reason for disconnection.
 * @return     none.
 * @section description
 * This callback function indicates disconnected device information and status
 */
static void rsi_ble_on_disconnect_event(rsi_ble_event_disconnect_t *resp_disconnect, uint16_t reason)
{
  memcpy((int8_t *)ble_remote_dev_addr, resp_disconnect->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_EVENT("\nrsi_ble_on_disconnect_event- dev_addr: %s, reason: 0x%x\n",
            rsi_6byte_dev_address_to_ascii(ble_str_conn_bd_addr, resp_disconnect->dev_addr),
            reason);
  device_found = 0;
  rsi_ble_app_set_event(RSI_BLE_DISCONN_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_smp_request
 * @brief      its invoked when smp request event is received.
 * @param[in]  remote_dev_address, it indicates remote bd address.
 * @return     none.
 * @section description
 * This callback function is invoked when SMP request events is received(we are in Master mode)
 * Note: slave requested to start SMP request, we have to send SMP request command
 */
void rsi_ble_on_smp_request(rsi_bt_event_smp_req_t *remote_dev_address)
{
  memcpy((int8_t *)ble_remote_dev_addr, remote_dev_address->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_EVENT("\nrsi_ble_on_smp_request- dev_addr: %s\n",
            rsi_6byte_dev_address_to_ascii(ble_str_conn_bd_addr, remote_dev_address->dev_addr));
  rsi_ble_app_set_event(RSI_BLE_SMP_REQ_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_smp_response
 * @brief      its invoked when smp response event is received.
 * @param[in]  remote_dev_address, it indicates remote bd address.
 * @return     none.
 * @section description
 * This callback function is invoked when SMP response events is received(we are in slave mode)
 * Note: Master initiated SMP protocol, we have to send SMP response command
 */
void rsi_ble_on_smp_response(rsi_bt_event_smp_resp_t *remote_dev_address)
{
  memcpy((int8_t *)ble_remote_dev_addr, remote_dev_address->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_EVENT("\nrsi_ble_on_smp_response- dev_addr: %s\n",
            rsi_6byte_dev_address_to_ascii(ble_str_conn_bd_addr, remote_dev_address->dev_addr));
  rsi_ble_app_set_event(RSI_BLE_SMP_RESP_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_smp_passkey
 * @brief      its invoked when smp passkey event is received.
 * @param[in]  remote_dev_address, it indicates remote bd address.
 * @return     none.
 * @section description
 * This callback function is invoked when SMP passkey events is received
 * Note: We have to send SMP passkey command
 */
void rsi_ble_on_smp_passkey(rsi_bt_event_smp_passkey_t *remote_dev_address)
{
  memcpy((int8_t *)ble_remote_dev_addr, remote_dev_address->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_EVENT("\nrsi_ble_on_smp_passkey- dev_addr: %s\n",
            rsi_6byte_dev_address_to_ascii(ble_str_conn_bd_addr, remote_dev_address->dev_addr));
  rsi_ble_app_set_event(RSI_BLE_SMP_PASSKEY_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_smp_passkey_display
 * @brief      its invoked when smp passkey event is received.
 * @param[in]  remote_dev_address, it indicates remote bd address.
 * @return     none.
 * @section description
 * This callback function is invoked when SMP passkey events is received
 * Note: We have to send SMP passkey command
 */
void rsi_ble_on_smp_passkey_display(rsi_bt_event_smp_passkey_display_t *smp_passkey_display)
{
  LOG_EVENT("\nrsi_ble_on_smp_passkey_display- passkey: %s\n", smp_passkey_display->passkey);
  rsi_ble_app_set_event(RSI_BLE_SMP_PASSKEY_DISPLAY_EVENT);
}

void rsi_ble_on_sc_passkey(rsi_bt_event_sc_passkey_t *sc_passkey)
{
  memcpy(ble_remote_dev_addr, sc_passkey->dev_addr, 6);
  LOG_EVENT("\nrsi_ble_on_sc_passkey- dev_addr: %s, passkey: %06ld \r\n",
            rsi_6byte_dev_address_to_ascii(ble_str_conn_bd_addr, sc_passkey->dev_addr),
            sc_passkey->passkey);
  numeric_value = sc_passkey->passkey;
  rsi_ble_app_set_event(RSI_BLE_SC_PASSKEY_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_smp_failed
 * @brief      its invoked when smp failed event is received.
 * @param[in]  remote_dev_address, it indicates remote bd address.
 * @return     none.
 * @section description
 * This callback function is invoked when SMP failed events is received
 */
void rsi_ble_on_smp_failed(uint16_t resp_status, rsi_bt_event_smp_failed_t *remote_dev_address)
{
  memcpy((int8_t *)ble_remote_dev_addr, remote_dev_address->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_EVENT("\nrsi_ble_on_smp_failed- resp_status: 0x%x, dev_addr: %s\n",
            resp_status,
            rsi_6byte_dev_address_to_ascii(ble_str_conn_bd_addr, remote_dev_address->dev_addr));
  rsi_ble_app_set_event(RSI_BLE_SMP_FAILED_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_encrypt_started
 * @brief      its invoked when encryption started event is received.
 * @param[in]  remote_dev_address, it indicates remote bd address.
 * @return     none.
 * @section description
 * This callback function is invoked when encryption started events is received
 */
void rsi_ble_on_encrypt_started(uint16_t status, rsi_bt_event_encryption_enabled_t *enc_enabled)
{
  (void)status;
  LOG_EVENT("\nrsi_ble_on_encrypt_started- resp_status: 0x%x\n", status);
  if (!already_paired)
    memcpy(&glbl_enc_enabled, enc_enabled, sizeof(rsi_bt_event_encryption_enabled_t));
  rsi_ble_app_set_event(RSI_BLE_ENCRYPT_STARTED_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_le_ltk_req_event
 * @brief      invoked when disconnection event is received
 * @param[in]  resp_disconnect, disconnected remote device information
 * @param[in]  reason, reason for disconnection.
 * @return     none.
 * @section description
 * This callback function indicates disconnected device information and status
 */
static void rsi_ble_on_le_ltk_req_event(rsi_bt_event_le_ltk_request_t *le_ltk_req)
{
  LOG_EVENT("\nrsi_ble_on_le_ltk_req_event\n");
  memcpy(&temp_le_ltk_req, le_ltk_req, sizeof(rsi_bt_event_le_ltk_request_t));
  rsi_ble_app_set_event(RSI_BLE_LTK_REQ_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_le_security_keys
 * @brief      its invoked when le secure keys events is received.
 * @param[in]  rsi_ble_event_le_security_keys, secure event parameters.
 * @return     none.
 * @section description
 * This callback function is invoked when security keys event is received after encription started.
 */
void rsi_ble_on_le_security_keys(rsi_bt_event_le_security_keys_t *rsi_ble_event_le_security_keys)
{
  memcpy(&app_ble_sec_keys, rsi_ble_event_le_security_keys, sizeof(rsi_bt_event_le_security_keys_t));
  LOG_EVENT("\nrsi_ble_on_encrypt_started- remote_ediv: 0x%x\n", app_ble_sec_keys.remote_ediv);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_mtu_event
 * @brief      its invoked when write/notify/indication events are received.
 * @param[in]  event_id, it indicates write/notification event id.
 * @param[in]  rsi_ble_write, write event parameters.
 * @return     none.
 * @section description
 * This callback function is invoked when write/notify/indication events are received
 */
static void rsi_ble_on_mtu_event(rsi_ble_event_mtu_t *rsi_ble_mtu)
{
  LOG_EVENT("\nrsi_ble_on_mtu_event\n");
  memcpy(&app_ble_mtu_event, rsi_ble_mtu, sizeof(rsi_ble_event_mtu_t));
  rsi_ble_app_set_event(RSI_BLE_MTU_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_gatt_write_event
 * @brief      its invoked when write/notify/indication events are received.
 * @param[in]  event_id, it indicates write/notification event id.
 * @param[in]  rsi_ble_write, write event parameters.
 * @return     none.
 * @section description
 * This callback function is invoked when write/notify/indication events are received
 */
static void rsi_ble_on_gatt_write_event(uint16_t event_id, rsi_ble_event_write_t *rsi_ble_write)
{
  //  uint16_t i = 0;
  LOG_EVENT("\nrsi_ble_on_gatt_write_event\n");
  (void)event_id; //This statement is added only to resolve compilation warning, value is unchanged
  memcpy(&app_ble_write_event, rsi_ble_write, sizeof(rsi_ble_event_write_t));
  rsi_ble_app_set_event(RSI_BLE_GATT_WRITE_EVENT);
}

sl_status_t rsi_ble_set_data_len_command(console_args_t *arguments)
{
  char *remote_dev_addr = sl_cli_get_argument_string(arguments, 0);
  uint16_t tx_len       = sl_cli_get_argument_uint16(arguments, 1);
  uint16_t tx_time      = sl_cli_get_argument_uint16(arguments, 2);

  uint8_t device_address[6];
  rsi_ascii_dev_address_to_6bytes_rev(device_address, (int8_t *)remote_dev_addr);
  return (rsi_ble_set_data_len(device_address, tx_len, tx_time));
}

sl_status_t rsi_ble_read_max_data_len_command(console_args_t *arguments)
{
  uint32_t status = 0;
  rsi_ble_read_max_data_length_t blereadmaxdatalen;

  status = rsi_ble_read_max_data_len(&blereadmaxdatalen);
  if (status == 0) {
    printf("maxtx_octets:%d, maxtx_time:%d, maxrx_octets:%d, maxrx_time:%d\n",
           blereadmaxdatalen.maxtxoctets,
           blereadmaxdatalen.maxtxtime,
           blereadmaxdatalen.maxrxoctets,
           blereadmaxdatalen.maxrxtime);
  }
  return status;
}

sl_status_t rsi_ble_set_phy_command(console_args_t *arguments)
{
  char *remote_dev_addr = sl_cli_get_argument_string(arguments, 0);
  uint8_t tx_phy        = sl_cli_get_argument_uint8(arguments, 1);
  uint8_t rx_phy        = sl_cli_get_argument_uint8(arguments, 2);
  uint16_t coded_phy    = sl_cli_get_argument_uint16(arguments, 3);

  uint8_t device_address[6];
  rsi_ascii_dev_address_to_6bytes_rev(device_address, (int8_t *)remote_dev_addr);
  return (rsi_ble_setphy((int8_t *)device_address, tx_phy, rx_phy, coded_phy));
}

sl_status_t rsi_ble_read_phy_command(console_args_t *arguments)
{
  uint32_t status = 0;
  rsi_ble_resp_read_phy_t read_phy_resp;

  char *remote_dev_addr = sl_cli_get_argument_string(arguments, 0);
  uint8_t dev_addr[6];
  rsi_ascii_dev_address_to_6bytes_rev(dev_addr, (int8_t *)remote_dev_addr);
  status = rsi_ble_readphy((int8_t *)dev_addr, &read_phy_resp);
  if (status == 0) {
    printf("dev_addr:%02x:%02x:%02x:%02x:%02x:%02x, tx_phy:%d, rx_phy:%d\n",
           read_phy_resp.dev_addr[0],
           read_phy_resp.dev_addr[1],
           read_phy_resp.dev_addr[2],
           read_phy_resp.dev_addr[3],
           read_phy_resp.dev_addr[4],
           read_phy_resp.dev_addr[5],
           read_phy_resp.tx_phy,
           read_phy_resp.rx_phy);
  }
  return (status);
}

sl_status_t rsi_ble_per_tx_command(console_args_t *arguments)
{

  rsi_ble_per_transmit_t ble_per_tx;
  memset(&ble_per_tx, '\0', sizeof(ble_per_tx));

  ble_per_tx.cmd_ix          = sl_cli_get_argument_uint8(arguments, 0);
  ble_per_tx.transmit_enable = sl_cli_get_argument_uint8(arguments, 1);
  rsi_uint32_to_4bytes(ble_per_tx.access_addr, sl_cli_get_argument_uint32(arguments, 2));
  ble_per_tx.phy_rate = sl_cli_get_argument_uint8(arguments, 3);

  char *tmp = sl_cli_get_argument_string(arguments, 4);

  ble_per_tx.rx_chnl_num    = (uint8_t)strtol(tmp, &tmp, 10);
  ble_per_tx.tx_chnl_num    = (uint8_t)strtol(++tmp, &tmp, 10);
  ble_per_tx.scrambler_seed = (uint8_t)strtol(++tmp, &tmp, 10);
  ble_per_tx.le_chnl_type   = (uint8_t)strtol(++tmp, &tmp, 10);
  ble_per_tx.freq_hop_en    = (uint8_t)strtol(++tmp, &tmp, 10);

  char *tmp1 = sl_cli_get_argument_string(arguments, 5);

  ble_per_tx.ant_sel  = (uint8_t)strtol(tmp1, &tmp1, 10);
  ble_per_tx.pll_mode = (uint8_t)strtol(++tmp1, &tmp1, 10);
  ble_per_tx.rf_type  = (uint8_t)strtol(++tmp1, &tmp1, 10);
  ble_per_tx.rf_chain = (uint8_t)strtol(++tmp1, &tmp1, 10);

  rsi_uint16_to_2bytes(ble_per_tx.pkt_len, sl_cli_get_argument_uint16(arguments, 6));
  ble_per_tx.payload_type  = sl_cli_get_argument_uint8(arguments, 7);
  ble_per_tx.tx_power      = sl_cli_get_argument_uint8(arguments, 8);
  ble_per_tx.transmit_mode = sl_cli_get_argument_uint8(arguments, 9);
  ble_per_tx.inter_pkt_gap = sl_cli_get_argument_uint8(arguments, 10);
  rsi_uint32_to_4bytes(ble_per_tx.num_pkts, sl_cli_get_argument_uint32(arguments, 11));

  int32_t status = rsi_ble_per_transmit(&ble_per_tx);
  return (status);
}

sl_status_t rsi_ble_per_rx_command(console_args_t *arguments)
{

  rsi_ble_per_receive_t rsi_ble_per_rx;
  memset(&rsi_ble_per_rx, '\0', sizeof(rsi_ble_per_rx));

  rsi_ble_per_rx.cmd_ix         = sl_cli_get_argument_uint8(arguments, 0);
  rsi_ble_per_rx.receive_enable = sl_cli_get_argument_uint8(arguments, 1);
  rsi_uint32_to_4bytes(rsi_ble_per_rx.access_addr, sl_cli_get_argument_uint32(arguments, 2));
  rsi_ble_per_rx.phy_rate = sl_cli_get_argument_uint8(arguments, 3);

  char *tmp = sl_cli_get_argument_string(arguments, 4);

  rsi_ble_per_rx.rx_chnl_num    = (uint8_t)strtol(tmp, &tmp, 10);
  rsi_ble_per_rx.tx_chnl_num    = (uint8_t)strtol(++tmp, &tmp, 10);
  rsi_ble_per_rx.scrambler_seed = (uint8_t)strtol(++tmp, &tmp, 10);
  rsi_ble_per_rx.le_chnl_type   = (uint8_t)strtol(++tmp, &tmp, 10);
  rsi_ble_per_rx.freq_hop_en    = (uint8_t)strtol(++tmp, &tmp, 10);

  rsi_ble_per_rx.ant_sel                 = sl_cli_get_argument_uint8(arguments, 5);
  rsi_ble_per_rx.pll_mode                = sl_cli_get_argument_uint8(arguments, 6);
  rsi_ble_per_rx.rf_type                 = sl_cli_get_argument_uint8(arguments, 7);
  rsi_ble_per_rx.rf_chain                = sl_cli_get_argument_uint8(arguments, 8);
  rsi_ble_per_rx.ext_data_len_indication = sl_cli_get_argument_uint8(arguments, 9);
  rsi_ble_per_rx.loop_back_mode          = sl_cli_get_argument_uint8(arguments, 10);
  rsi_ble_per_rx.duty_cycling_en         = sl_cli_get_argument_uint8(arguments, 11);

  int32_t status = rsi_ble_per_receive(&rsi_ble_per_rx);
  return (status);
}
sl_status_t rsi_ble_connect_cancel_command(console_args_t *arguments)
{
  char *remote_dev_addr = sl_cli_get_argument_string(arguments, 0);
  uint8_t device_address[6];
  rsi_ascii_dev_address_to_6bytes_rev(device_address, (int8_t *)remote_dev_addr);
  return (rsi_ble_connect_cancel((int8_t *)device_address));
}
sl_status_t rsi_ble_add_accept_list_command(console_args_t *arguments)
{
  char *remote_dev_addr    = sl_cli_get_argument_string(arguments, 0);
  uint8_t remote_addr_type = sl_cli_get_argument_uint8(arguments, 1);
  uint8_t device_address[6];
  rsi_ascii_dev_address_to_6bytes_rev(device_address, (int8_t *)remote_dev_addr);
  return (rsi_ble_addto_acceptlist((int8_t *)device_address, remote_addr_type));
}
sl_status_t rsi_ble_get_att_descriptors_async_command(console_args_t *arguments)
{
  char *remote_dev_addr     = sl_cli_get_argument_string(arguments, 0);
  uint8_t device_address[6] = { 0 };
  rsi_ascii_dev_address_to_6bytes_rev(device_address, (int8_t *)remote_dev_addr);
  uint16_t start_handle = sl_cli_get_argument_uint16(arguments, 1);
  uint16_t end_handle   = sl_cli_get_argument_uint16(arguments, 2);
  return (rsi_ble_get_att_descriptors_async(device_address, start_handle, end_handle, NULL));
}

sl_status_t rsi_ble_remove_service_command(console_args_t *arguments)
{
  //! remove service
  printf("\r\n rsi_ble_remove_service_command invoked \r\n");
  return (rsi_ble_remove_gatt_service((uint32_t)&new_serv_resp));
}

#if 0
sl_status_t rsi_ble_remove_service_attribute_command(console_args_t* arguments)
{
    //! remove service attribute
    printf("\r\n rsi_ble_remove_service_command invoked \r\n");
    return (rsi_ble_remove_gatt_attribute(&new_serv_resp, &new_att));
}
#endif

sl_status_t rsi_ble_accept_list_using_adv_data_comamnd(console_args_t *arguments)
{

  uint8_t enable              = sl_cli_get_argument_uint8(arguments, 0);
  uint8_t data_compare_index  = sl_cli_get_argument_uint8(arguments, 1);
  uint8_t compare_data_length = sl_cli_get_argument_uint8(arguments, 2);
  uint8_t *payload            = (uint8_t *)sl_cli_get_argument_string(arguments, 3);
  return (rsi_ble_accept_list_using_adv_data(enable, data_compare_index, compare_data_length, payload));
}

sl_status_t rsi_ble_uuid_32bit_to_128bit_command(console_args_t *arguments)
{
  (void)arguments;
  printf("\r\n Generating a 128 bit UUID \r\n");
  uuid_t new_uuid                       = { 0 };
  rsi_ble_resp_add_serv_t new_serv_resp = { 0 };
  uint8_t data[20]                      = { "redpine_sampletest_1" };
  int32_t status                        = 0;

  //! adding new service
  new_uuid.size      = 2;
  new_uuid.val.val16 = 0xAABB;
  rsi_ble_fill_128bit_uuid(0xAABB, &new_uuid);
  status = rsi_ble_add_service(new_uuid, &new_serv_resp);
  if (status != RSI_SUCCESS) {
    return status;
  }

  //! adding characteristic service attribute to the service
  new_uuid.size      = 2;
  new_uuid.val.val16 = 0x1AA1;
  rsi_ble_fill_128bit_uuid(0x1AA1, &new_uuid);
  rsi_ble_add_char_serv_att(new_serv_resp.serv_handler,
                            new_serv_resp.start_handle + 1,
                            0x08,
                            new_serv_resp.start_handle + 2,
                            new_uuid);

  //! adding characteristic value attribute to the service
  new_uuid.size      = 2;
  new_uuid.val.val16 = 0x1AA1;
  rsi_ble_fill_128bit_uuid(0x1AA1, &new_uuid);
  rsi_ble_add_char_val_att(new_serv_resp.serv_handler,
                           new_serv_resp.start_handle + 2,
                           new_uuid,
                           0x08,
                           data,
                           sizeof(data));

  //! adding characteristic service attribute to the service
  new_uuid.size      = 2;
  new_uuid.val.val16 = 0x1BB1;
  rsi_ble_fill_128bit_uuid(0x1BB1, &new_uuid);
  rsi_ble_add_char_serv_att(new_serv_resp.serv_handler,
                            new_serv_resp.start_handle + 3,
                            0x02 | 0x10,
                            new_serv_resp.start_handle + 4,
                            new_uuid);

  //! adding characteristic value attribute to the service
  new_uuid.size      = 2;
  new_uuid.val.val16 = 0x1BB1;
  rsi_ble_fill_128bit_uuid(0x1BB1, &new_uuid);
  rsi_ble_add_char_val_att(new_serv_resp.serv_handler,
                           new_serv_resp.start_handle + 4,
                           new_uuid,
                           0x02 | 0x10,
                           data,
                           sizeof(data));
  return (status);
}

void rsi_ble_fill_128bit_uuid(uint32_t uuid_32bit, uuid_t *serv_uuid)
{
  uint8_t ix;
  serv_uuid->size = 16;
  rsi_uint32_to_4bytes((uint8_t *)&serv_uuid->val.val128.data1, uuid_32bit);
  rsi_uint16_to_2bytes((uint8_t *)&serv_uuid->val.val128.data2, 0x00);
  rsi_uint16_to_2bytes((uint8_t *)&serv_uuid->val.val128.data3, 0x1000);
  rsi_uint16_to_2bytes(&serv_uuid->val.val128.data4[0], 0x8000);
  for (ix = 0; ix < 6; ix++) {
    serv_uuid->val.val128.data4[2] = 0x26;
    serv_uuid->val.val128.data4[3] = 0x00;
    serv_uuid->val.val128.data4[4] = 0x91;
    serv_uuid->val.val128.data4[5] = 0x52;
    serv_uuid->val.val128.data4[6] = 0x76;
    serv_uuid->val.val128.data4[7] = 0xBB;
  }

  return;
}

sl_status_t rsi_ble_get_char_value_by_uuid_async_command(console_args_t *arguments)
{
  char *remote_dev_bd_addr          = sl_cli_get_argument_string(arguments, 0);
  uint8_t rsi_connected_dev_addr[6] = { 0 };
  rsi_ascii_dev_address_to_6bytes_rev(rsi_connected_dev_addr, (int8_t *)remote_dev_bd_addr);

  uint16_t start_handle = sl_cli_get_argument_uint16(arguments, 1);
  uint16_t end_handle   = sl_cli_get_argument_uint16(arguments, 2);
  uint16_t uuid         = sl_cli_get_argument_uint16(arguments, 3);

  //! adding new service
  uuid_t new_uuid    = { 0 };
  new_uuid.size      = 2;
  new_uuid.val.val16 = uuid;

  return (rsi_ble_get_char_value_by_uuid_async(rsi_connected_dev_addr, start_handle, end_handle, new_uuid, NULL));
}

sl_status_t rsi_ble_clear_acceptlist_command(console_args_t *arguments)
{
  (void)arguments;
  printf("\r\n clear acceptlist invoked \r\n");
  return (rsi_ble_clear_acceptlist());
}

sl_status_t rsi_ble_deletefrom_acceptlist_command(console_args_t *arguments)
{
  printf("\r\n deletefrom acceptlist invoked \r\n");
  char *remote_dev_bd_addr          = sl_cli_get_argument_string(arguments, 0);
  uint8_t dev_addr_type             = sl_cli_get_argument_uint8(arguments, 1);
  uint8_t rsi_connected_dev_addr[6] = { 0 };
  rsi_ascii_dev_address_to_6bytes_rev(rsi_connected_dev_addr, (int8_t *)remote_dev_bd_addr);
  return (rsi_ble_deletefrom_acceptlist((int8_t *)rsi_connected_dev_addr, dev_addr_type));
}

sl_status_t rsi_ble_set_scan_response_data_command(console_args_t *arguments)
{
  char *data = sl_cli_get_argument_string(arguments, 0);

  if (!(strcmp(data, "NULL"))) {
    memset(data, 0, strlen(data));
  }

  const uint8_t data_length = strlen(data);
  uint8_t adv[31]           = { 2, 1, 6 };

  // prepare advertise data //local/device name
  adv[3] = data_length + 1;
  adv[4] = 9;
  strcpy((char *)&adv[5], data);

  return (rsi_ble_set_scan_response_data(adv, data_length + 5));
}

sl_status_t rsi_ble_prepare_write_async_command(console_args_t *arguments)
{
  char *remote_dev_addr     = sl_cli_get_argument_string(arguments, 0);
  uint8_t device_address[6] = { 0 };
  rsi_ascii_dev_address_to_6bytes_rev(device_address, (int8_t *)remote_dev_addr);
  uint16_t handle  = sl_cli_get_argument_uint16(arguments, 1);
  uint16_t offset  = sl_cli_get_argument_uint16(arguments, 2);
  uint8_t data_len = sl_cli_get_argument_uint8(arguments, 3);
  return (rsi_ble_prepare_write_async(device_address, handle, offset, data_len, NULL));
}

sl_status_t rsi_ble_execute_write_async_command(console_args_t *arguments)
{
  char *remote_dev_addr     = sl_cli_get_argument_string(arguments, 0);
  uint8_t device_address[6] = { 0 };
  rsi_ascii_dev_address_to_6bytes_rev(device_address, (int8_t *)remote_dev_addr);
  uint8_t exe_flag = sl_cli_get_argument_uint8(arguments, 1);
  return (rsi_ble_execute_write_async(device_address, exe_flag));
}

sl_status_t rsi_ble_set_att_command(console_args_t *arguments)
{
  char *remote_dev_bd_addr = sl_cli_get_argument_string(arguments, 0);
  uint16_t handle          = sl_cli_get_argument_uint16(arguments, 1);
  uint8_t data             = sl_cli_get_argument_uint8(arguments, 2);

  uint8_t data_to_send[1]           = { data };
  uint8_t rsi_connected_dev_addr[6] = { 0 };
  rsi_ascii_dev_address_to_6bytes_rev(rsi_connected_dev_addr, (int8_t *)remote_dev_bd_addr);
  return (rsi_ble_set_att_cmd(rsi_connected_dev_addr, handle, sizeof(data_to_send), (uint8_t *)data_to_send));
}

sl_status_t rsi_ble_remove_gatt_service_command(console_args_t *arguments)
{
  uint32_t service_handler = sl_cli_get_argument_uint32(arguments, 0);
  return (rsi_ble_remove_gatt_service(service_handler));
}

sl_status_t rsi_ble_remove_gatt_attribute_command(console_args_t *arguments)
{
  uint32_t service_handler = sl_cli_get_argument_uint32(arguments, 0);
  uint16_t att_handle      = sl_cli_get_argument_uint16(arguments, 1);
  return (rsi_ble_remove_gatt_attibute(service_handler, att_handle));
}

sl_status_t rsi_bt_power_save_profile_command(console_args_t *arguments)
{
  uint8_t mode     = sl_cli_get_argument_uint8(arguments, 0);
  uint8_t psp_type = sl_cli_get_argument_uint8(arguments, 1);
  return (rsi_bt_power_save_profile(mode, psp_type));
}

sl_status_t rsi_bt_get_local_device_address_command(console_args_t *arguments)
{
  (void)arguments;
  uint8_t device_address[6] = { 0 };
  printf("\r\n rsi_bt_get_local_address_command invoked \r\n");
  int32_t status = rsi_bt_get_local_device_address(device_address);
  if (status == RSI_SUCCESS) {
    printf("\n%02X:%02X:%02X:%02X:%02X:%02X\n",
           device_address[5],
           device_address[4],
           device_address[3],
           device_address[2],
           device_address[1],
           device_address[0]);
  }
  return (status);
}

sl_status_t rsi_bt_set_local_name_command(console_args_t *arguments)
{
  printf("\r\n rsi_ble_set_local_name_command invoked \r\n");
  char *name = sl_cli_get_argument_string(arguments, 0);
  if (!(strcmp(name, "NULL"))) {
    memset(name, 0, strlen(name));
    return (rsi_bt_set_local_name((uint8_t *)name));
  } else {
    return (rsi_bt_set_local_name((uint8_t *)name));
  }
}

sl_status_t rsi_bt_get_local_name_command(console_args_t *arguments)
{
  (void)arguments;
  rsi_bt_resp_get_local_name_t name = { 0 };
  int32_t status                    = rsi_bt_get_local_name(&name);
  if (status == RSI_SUCCESS) {
    printf("\n%s\n", name.name);
  }
  return (status);
}

static void rsi_ble_on_event_mtu_exchange_info(
  rsi_ble_event_mtu_exchange_information_t *rsi_ble_event_mtu_exchange_info)
{
  rsi_6byte_dev_address_to_ascii(ble_str_conn_bd_addr, rsi_ble_event_mtu_exchange_info->dev_addr);
  //! copy to conn specific buffer
  memcpy(&mtu_exchange_info, rsi_ble_event_mtu_exchange_info, sizeof(rsi_ble_event_mtu_exchange_information_t));
  rsi_ble_app_set_event(RSI_BLE_MTU_EXCHANGE_INFORMATION);
}
sl_status_t rsi_bt_deinit_command_handler(console_args_t *arguments)
{
  (void)arguments;
  int32_t status = rsi_bt_deinit();
  return status;
}

void rsi_ble_simple_central_on_ae_adv_report_event(uint16_t status, rsi_ble_ae_adv_report_t *ae_adv_report)
{
  UNUSED_PARAMETER(status);
  UNUSED_PARAMETER(&ae_adv_report);
  rsi_ble_app_set_event(RSI_APP_EVENT_AE_ADV_REPORT);
}

void rsi_ble_simple_central_on_ae_periodic_adv_sync_estbl_event(uint16_t status,
                                                                rsi_ble_per_adv_sync_estbl_t *ae_periodic_adv_sync)
{
  UNUSED_PARAMETER(status);
  UNUSED_PARAMETER(&ae_periodic_adv_sync);
  rsi_ble_app_set_event(RSI_APP_EVENT_AE_PER_SYNC_ESTBL);
}

void rsi_ble_simple_central_on_ae_periodic_adv_report_event(uint16_t status,
                                                            rsi_ble_per_adv_report_t *ae_per_adv_report)
{
  UNUSED_PARAMETER(status);
  UNUSED_PARAMETER(&ae_per_adv_report);
  rsi_ble_app_set_event(RSI_APP_EVENT_AE_PER_ADV_REPORT);
}

void rsi_ble_simple_central_on_ae_periodic_adv_sync_lost_event(uint16_t status,
                                                               rsi_ble_per_adv_sync_lost_t *ae_per_sync_lost)
{
  UNUSED_PARAMETER(status);
  UNUSED_PARAMETER(&ae_per_sync_lost);
  rsi_ble_app_set_event(RSI_APP_EVENT_AE_PER_ADV_SYNC_LOST);
}

void rsi_ble_simple_central_on_ae_scan_timeout_event(uint16_t status, rsi_ble_scan_timeout_t *ae_scan_timeout)
{
  UNUSED_PARAMETER(status);
  UNUSED_PARAMETER(&ae_scan_timeout);
  rsi_ble_app_set_event(RSI_APP_EVENT_AE_SCAN_TIMEOUT);
}

void rsi_ble_simple_central_on_ae_adv_set_terminated_event(uint16_t status, rsi_ble_adv_set_terminated_t *ae_adv_set)
{
  UNUSED_PARAMETER(status);
  UNUSED_PARAMETER(&ae_adv_set);
  rsi_ble_app_set_event(RSI_APP_EVENT_AE_ADV_SET_TERMINATED);
}

void rsi_ble_simple_central_on_ae_scan_req_recvd_event(uint16_t status, rsi_ble_scan_req_recvd_t *ae_scan_req)
{
  UNUSED_PARAMETER(status);
  UNUSED_PARAMETER(&ae_scan_req);
  rsi_ble_app_set_event(RSI_APP_EVENT_AE_SCAN_REQ_RECVD);
}
sl_status_t rsi_ble_ae_set_periodic_sync_command(console_args_t *arguments)
{
  rsi_ble_ae_set_periodic_adv_create_sync_t ae_per_sync_create = { 0 };
  char *remote_dev_addr                                        = sl_cli_get_argument_string(arguments, 0);
  rsi_ascii_dev_address_to_6bytes_rev((uint8_t *)ae_per_sync_create.adv_addr, (int8_t *)remote_dev_addr);
  ae_per_sync_create.adv_addr_type = sl_cli_get_argument_uint8(arguments, 1);
  ae_per_sync_create.adv_sid       = sl_cli_get_argument_uint8(arguments, 2);
  ae_per_sync_create.fil_policy    = sl_cli_get_argument_uint8(arguments, 3);
  ae_per_sync_create.skip          = sl_cli_get_argument_uint8(arguments, 4);
  ae_per_sync_create.sync_timeout  = sl_cli_get_argument_uint8(arguments, 5);
  uint8_t BLE_AE_PER_SYNC          = sl_cli_get_argument_uint8(arguments, 6);
  int32_t status                   = rsi_ble_ae_set_periodic_sync(BLE_AE_PER_SYNC, &ae_per_sync_create);
  return status;
}

sl_status_t rsi_ble_ae_dev_to_periodic_list_command(console_args_t *arguments)
{
  rsi_ble_ae_dev_to_periodic_list_t ae_add_dev = { 0 };
  char *remote_dev_addr                        = sl_cli_get_argument_string(arguments, 0);
  rsi_ascii_dev_address_to_6bytes_rev((uint8_t *)ae_add_dev.adv_addr, (int8_t *)remote_dev_addr);
  ae_add_dev.adv_addr_type = sl_cli_get_argument_uint8(arguments, 1);
  ae_add_dev.adv_sid = 1, ae_add_dev.type = sl_cli_get_argument_uint8(arguments, 2);
  int32_t status = rsi_ble_ae_dev_to_periodic_list(&ae_add_dev);
  return status;
}

sl_status_t rsi_ble_ae_read_periodic_adv_list_size_command(console_args_t *arguments)
{
  uint8_t size   = sl_cli_get_argument_uint8(arguments, 0);
  int32_t status = rsi_ble_ae_read_periodic_adv_list_size(&size);
  return status;
}

sl_status_t rsi_ble_extended_connect_with_params_command(console_args_t *arguments)
{
  rsi_ble_ae_extended_create_connect_t ble_extended_create_conn = { 0 };
  char *remote_dev_addr                                         = sl_cli_get_argument_string(arguments, 0);
  rsi_ascii_dev_address_to_6bytes_rev(ble_extended_create_conn.remote_addr, (int8_t *)remote_dev_addr);
  ble_extended_create_conn.remote_addr_type        = sl_cli_get_argument_uint8(arguments, 1);
  ble_extended_create_conn.initiator_filter_policy = sl_cli_get_argument_uint8(arguments, 2);
  ble_extended_create_conn.init_phys               = sl_cli_get_argument_uint8(arguments, 3);
  for (int8_t ix = 0; ix < 3; ix++) {
    ble_extended_create_conn.init_params[ix].ScanInterval    = sl_cli_get_argument_uint8(arguments, 4);
    ble_extended_create_conn.init_params[ix].ScanWindow      = sl_cli_get_argument_uint8(arguments, 5);
    ble_extended_create_conn.init_params[ix].ConnIntervalMin = sl_cli_get_argument_uint8(arguments, 6);
    ble_extended_create_conn.init_params[ix].ConnIntervalMax = sl_cli_get_argument_uint8(arguments, 7);
    ble_extended_create_conn.init_params[ix].ConnLatency     = sl_cli_get_argument_uint8(arguments, 8);
    ble_extended_create_conn.init_params[ix].ConnSTO         = sl_cli_get_argument_uint16(arguments, 9);
    ble_extended_create_conn.init_params[ix].MinCELen        = sl_cli_get_argument_uint16(arguments, 10);
    ble_extended_create_conn.init_params[ix].MaxCELen        = sl_cli_get_argument_uint16(arguments, 11);
    ble_extended_create_conn.own_addr_type                   = sl_cli_get_argument_uint8(arguments, 12);
  }
  int32_t status = rsi_ble_extended_connect_with_params(&ble_extended_create_conn);
  return status;
}

sl_status_t rsi_ble_get_max_no_of_supp_adv_sets_command(console_args_t *arguments)
{
  uint8_t max_no_of_supp_adv_sets = sl_cli_get_argument_uint8(arguments, 0);
  int32_t status                  = rsi_ble_get_max_no_of_supp_adv_sets(&max_no_of_supp_adv_sets);
  return status;
}

sl_status_t rsi_ble_get_max_adv_data_len_command(console_args_t *arguments)
{
  uint8_t max_data_len = sl_cli_get_argument_uint8(arguments, 0);
  int32_t status       = rsi_ble_get_max_adv_data_len((uint8_t *)&max_data_len);
  return status;
}

sl_status_t rsi_ble_set_att_cmd_command(console_args_t *arguments)
{
  char *remote_dev_bd_addr = sl_cli_get_argument_string(arguments, 0);
  uint16_t handle          = sl_cli_get_argument_uint16(arguments, 1);
  uint8_t data             = sl_cli_get_argument_uint8(arguments, 2);

  uint8_t data_to_send[1] = { data };

  uint8_t rsi_connected_dev_addr[6] = { 0 };
  rsi_ascii_dev_address_to_6bytes_rev(rsi_connected_dev_addr, (int8_t *)remote_dev_bd_addr);
  return (rsi_ble_set_att_cmd(rsi_connected_dev_addr, handle, sizeof(data_to_send), (uint8_t *)data_to_send));
}
