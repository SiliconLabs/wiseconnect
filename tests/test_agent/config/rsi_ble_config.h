// <<< Use Configuration Wizard in Context Menu >>>

/******************************************************
 * *                      Macros
 * ******************************************************/

#define RSI_BLE_SET_RAND_ADDR "00:23:A7:12:34:56"

#define RSI_DEV_ADDR_LEN 6

//globals for ble advertising, referenced in #defines below
//and set via rsi_ble_start_advertising_command
extern uint8_t g_rsi_ble_adv_type;
extern uint8_t g_rsi_ble_adv_filter_type;
extern uint8_t g_rsi_ble_addr_type;
extern uint8_t g_rsi_ble_dir_addr[RSI_DEV_ADDR_LEN];
extern uint16_t g_rsi_ble_adv_int_min;
extern uint16_t g_rsi_ble_adv_int_max;
extern uint8_t g_rsi_own_addr_type;
extern uint8_t g_rsi_ble_adv_channel_map;

//globals for ble scan, referenced in #defines below
//and set via rsi_ble_start_scanning_command
extern uint8_t g_ble_scan_type;
extern uint8_t g_ble_scan_filter_type;
extern uint16_t g_le_scan_interval;
extern uint16_t g_le_scan_window;
extern uint8_t g_le_public_address;
extern uint8_t g_rsi_ble_pwr_inx;

extern uint16_t g_connection_interval_min;
extern uint16_t g_connection_interval_max;
extern uint16_t g_connection_latency;
extern uint16_t g_supervision_timeout;
extern uint8_t g_ble_power_save_options;

//set ble advertising parameters
#define RSI_BLE_ADV_TYPE          (g_rsi_ble_adv_type)
#define RSI_BLE_ADV_FILTER_TYPE   (g_rsi_ble_adv_filter_type)
#define RSI_BLE_ADV_DIR_ADDR_TYPE (g_rsi_ble_addr_type)
#define RSI_BLE_ADV_DIR_ADDR      (g_rsi_ble_dir_addr)

#define RSI_BLE_ADV_INT_MIN (g_rsi_ble_adv_int_min)
#define RSI_BLE_ADV_INT_MAX (g_rsi_ble_adv_int_max)
// TODO: this does not work since it's redefined below
//#define  LE_PUBLIC_ADDRESS						  (g_rsi_own_addr_type)
#define RSI_BLE_ADV_CHANNEL_MAP (g_rsi_ble_adv_channel_map)

#define CLEAR_WHITELIST              0x00
#define ADD_DEVICE_TO_WHITELIST      0x01
#define DELETE_DEVICE_FROM_WHITELIST 0x02

#define ALL_PHYS 0x00

#define RSI_BLE_DEV_ADDR_RESOLUTION_ENABLE 0

#define RSI_OPERMODE_WLAN_BLE 13

#define RSI_BLE_MAX_NBR_ATT_REC  80
#define RSI_BLE_MAX_NBR_ATT_SERV 10

#define RSI_BLE_MAX_NBR_SLAVES    3
#define RSI_BLE_MAX_NBR_MASTERS   2
#define RSI_BLE_GATT_ASYNC_ENABLE 0
#define RSI_BLE_GATT_INIT         0

/* Number of BLE notifications */
#define RSI_BLE_NUM_CONN_EVENTS 0x5

/* Number of BLE GATT RECORD SIZE IN (n*16 BYTES), eg:(0x40*16)=1024 bytes */
#define RSI_BLE_NUM_REC_BYTES 0x40

/*=======================================================================*/
//! Advertising command parameters
/*=======================================================================*/

#if 0
#define RSI_BLE_ADV_TYPE          UNDIR_CONN
#define RSI_BLE_ADV_FILTER_TYPE   ALLOW_SCAN_REQ_ANY_CONN_REQ_ANY
#define RSI_BLE_ADV_DIR_ADDR_TYPE LE_PUBLIC_ADDRESS
#define RSI_BLE_ADV_DIR_ADDR      "00:15:83:6A:64:17"

#define RSI_BLE_ADV_INT_MIN     0x100
#define RSI_BLE_ADV_INT_MAX     0x200
#define RSI_BLE_ADV_CHANNEL_MAP 0x07
#endif
//!Advertise status
//! Start the advertising process
#define RSI_BLE_START_ADV 0x01
//! Stop the advertising process
#define RSI_BLE_STOP_ADV 0x00

//! BLE Tx Power Index On Air
//#define RSI_BLE_PWR_INX                               30

//! BLE Active H/w Pwr Features
#define BLE_DISABLE_DUTY_CYCLING 0
#define BLE_DUTY_CYCLING         1
#define BLR_DUTY_CYCLING         2
#define BLE_4X_PWR_SAVE_MODE     4
//#define RSI_BLE_PWR_SAVE_OPTIONS                      BLE_DISABLE_DUTY_CYCLING

#undef RSI_BLE_PWR_SAVE_OPTIONS
#define RSI_BLE_PWR_SAVE_OPTIONS g_ble_power_save_options

//!Advertise types

/* Advertising will be visible(discoverable) to all the devices.
 * Scanning/Connection is also accepted from all devices
 * */
#define UNDIR_CONN 0x80

/* Advertising will be visible(discoverable) to the particular device 
 * mentioned in RSI_BLE_ADV_DIR_ADDR only. 
 * Scanning and Connection will be accepted from that device only.
 * */
#define DIR_CONN 0x81

/* Advertising will be visible(discoverable) to all the devices.
 * Scanning will be accepted from all the devices.
 * Connection will be not be accepted from any device.
 * */
#define UNDIR_SCAN 0x82

/* Advertising will be visible(discoverable) to all the devices.
 * Scanning and Connection will not be accepted from any device
 * */
#define UNDIR_NON_CONN 0x83

/* Advertising will be visible(discoverable) to the particular device 
 * mentioned in RSI_BLE_ADV_DIR_ADDR only. 
 * Scanning and Connection will be accepted from that device only.
 * */
#define DIR_CONN_LOW_DUTY_CYCLE 0x84

//!Advertising flags
#define LE_LIMITED_DISCOVERABLE 0x01
#define LE_GENERAL_DISCOVERABLE 0x02
#define LE_BR_EDR_NOT_SUPPORTED 0x04

//!Advertise filters
#define ALLOW_SCAN_REQ_ANY_CONN_REQ_ANY               0x00
#define ALLOW_SCAN_REQ_WHITE_LIST_CONN_REQ_ANY        0x01
#define ALLOW_SCAN_REQ_ANY_CONN_REQ_WHITE_LIST        0x02
#define ALLOW_SCAN_REQ_WHITE_LIST_CONN_REQ_WHITE_LIST 0x03

//! Address types
/* re-defined below
#define LE_PUBLIC_ADDRESS                             0x00
*/
#define LE_RANDOM_ADDRESS            0x01
#define LE_RESOLVABLE_PUBLIC_ADDRESS 0x02
#define LE_RESOLVABLE_RANDOM_ADDRESS 0x03

/*=======================================================================*/

/*=======================================================================*/
//! Connection parameters
/*=======================================================================*/
/*

// Redefined below 

#define LE_SCAN_INTERVAL                             0x0100
#define LE_SCAN_WINDOW                               0x0050

#define CONNECTION_INTERVAL_MIN                      0x00A0
#define CONNECTION_INTERVAL_MAX                      0x00A0

#define CONNECTION_LATENCY                           0x0000
#define SUPERVISION_TIMEOUT                          0x07D0 //2000
*/
/*=======================================================================*/

/*=======================================================================*/
//! Scan command parameters
/*=======================================================================*/

#define RSI_BLE_SCAN_TYPE        (g_ble_scan_type)
#define RSI_BLE_SCAN_FILTER_TYPE (g_ble_scan_filter_type)
#define LE_SCAN_INTERVAL         (g_le_scan_interval)
#define LE_SCAN_WINDOW           (g_le_scan_window)
#define LE_PUBLIC_ADDRESS        (g_le_public_address)

#define CONNECTION_INTERVAL_MIN (g_connection_interval_min)
#define CONNECTION_INTERVAL_MAX (g_connection_interval_max)
#define CONNECTION_LATENCY      (g_connection_latency)
#define SUPERVISION_TIMEOUT     (g_supervision_timeout)

//!Scan status
#define RSI_BLE_START_SCAN 0x01
#define RSI_BLE_STOP_SCAN  0x00

//!Scan types
#define SCAN_TYPE_ACTIVE  0x01
#define SCAN_TYPE_PASSIVE 0x00

//!Scan filters
#define SCAN_FILTER_TYPE_ALL             0x00
#define SCAN_FILTER_TYPE_ONLY_WHITE_LIST 0x01

#define RSI_SEL_INTERNAL_ANTENNA 0x00
#define RSI_SEL_EXTERNAL_ANTENNA 0x01

/*=======================================================================*/
//! Parameters for whitelisting of BLE advertise reports based on the ADV payload
/*=======================================================================*/
#define RSI_BLE_ENABLE_WHITELIST_BASEDON_ADV_PAYLOAD     0x00
#define RSI_BLE_ADV_PAYLOAD_INDEX_FOR_COMPARE            0x00
#define RSI_BLE_ADV_PAYLOAD_LENGTH_FROM_INDEX_TO_COMPARE 0x00

// Power Save Profile Mode
#define PSP_TYPE RSI_MAX_PSP

// Power Save Profile Mode
#define PSP_MODE RSI_SLEEP_MODE_2

#undef RSI_BLE_PWR_INX
//! BLE Tx Power Index On Air
#define RSI_BLE_PWR_INX g_rsi_ble_pwr_inx
