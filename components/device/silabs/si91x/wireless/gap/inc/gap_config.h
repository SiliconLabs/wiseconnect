/*******************************************************************************
* @file  gap_config.h
* @brief GAP Component Configuration Header
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
 * @file  gap_config.h
 * @brief GAP Component Configuration Header
 * 
 * This header contains all GAP (Generic Access Profile) component configuration
 * parameters used by the GAP event handlers, initialization, and utility functions.
 * 
 * Configuration Categories:
 * 1. Device Role and State Definitions
 * 2. Connection Limits and Capacity
 * 3. Connection Parameters (intervals, latency, timeouts)
 * 4. Address and Name Formats
 * 5. Advertising and Scanning Control
 * 6. Security and MTU Settings
 * 7. Debug Configuration
 */

#ifndef GAP_CONFIG_H
#define GAP_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> GAP Configuration

/*=======================================================================*/
//   BOOLEAN DEFINITIONS
/*=======================================================================*/

/**
 * @brief Boolean value definitions
 * 
 * Generic true/false definitions used throughout GAP component
 */
#ifndef BT_TRUE
#define BT_TRUE 1
#endif

#ifndef BT_FLASE
#define BT_FLASE 0
#endif

/*=======================================================================*/
//   UTILITY MACROS
/*=======================================================================*/

// Local MIN helper used below (e.g. C2P12_SUPERVISION_TIMEOUT).
//
// Defined here -- instead of relying on a transitively-included SDK header --
// so this config works identically on Si91x SoC and EFR32 NCP host builds.
// Matches the convention already used by other BLE examples
// (e.g. examples/snippets/ble/ble_unified_ae_coex_app/ble_config.h).
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

/*=======================================================================*/
//   DEVICE ROLE AND STATE DEFINITIONS
/*=======================================================================*/

/**
 * @brief Device role identifiers
 * 
 * Used by:
 * - gap_user_event_hdlrs.c: rsi_ble_on_connect_event(), rsi_ble_on_enhance_conn_status_event()
 * - gap_utilities.c: rsi_get_remote_device_role()
 * 
 * These define whether a connected device is acting as a peripheral or central.
 */
#define PERIPHERAL_RL 1 //! Remote device is a BLE peripheral (advertising, connectable)
#define CENTRAL_ROLE  2 //! Remote device is a BLE central (scanning, initiating connections)

/**
 * @brief Peripheral device connection state values
 * 
 * Used by:
 * - gap_utilities.c: rsi_check_dev_list_driver() - return values
 * - gap_user_event_hdlrs.c: rsi_ble_ae_adv_report_event() - device state checking
 * 
 * These values indicate the current connection status of a peripheral device.
 */
#define NO_PERIPHERAL_FOUND      0 //! Peripheral not in configured device list
#define PERIPHERAL_FOUND         1 //! Peripheral found in device list, not yet connected
#define PERIPHERAL_CONNECTED     2 //! Peripheral already connected
#define PERIPHERAL_NOT_CONNECTED 3 //! Peripheral in list but not connected

/*=======================================================================*/
//   CONNECTION LIMITS AND CAPACITY
/*=======================================================================*/

#define EXTENDED_ADV_SCAN 1

// <h> Connection Limits
// <i> Max peripherals (as central) and max centrals (as peripheral). Use 1 and 1 for extended adv; 8 and 2 for legacy.

// <o RSI_BLE_MAX_NBR_PERIPHERALS> Max peripherals (as central)
// <1-8:1>
// <i> Default: 8 (legacy). Set to 1 for extended advertising.
#ifndef RSI_BLE_MAX_NBR_PERIPHERALS
#define RSI_BLE_MAX_NBR_PERIPHERALS 8
#endif

// <o RSI_BLE_MAX_NBR_CENTRALS> Max centrals (as peripheral)
// <1-2:1>
// <i> Default: 2 (legacy). Set to 1 for extended advertising.
#ifndef RSI_BLE_MAX_NBR_CENTRALS
#define RSI_BLE_MAX_NBR_CENTRALS 2
#endif

// </h>

#define TOTAL_CONNECTIONS (RSI_BLE_MAX_NBR_PERIPHERALS + RSI_BLE_MAX_NBR_CENTRALS)

/**
 * @brief Connection index definitions for peripheral and central roles
 * 
 * Used by:
 * - gap_utilities.c: Array indexing for connection buffers (rsi_ble_conn_info[])
 * - gap_user_event_hdlrs.c: Connection management and event handling
 * 
 * These define the array indices for each peripheral/central connection slot.
 * Peripheral indices: 0-7 (when RSI_BLE_MAX_NBR_PERIPHERALS = 8)
 * Central indices: Start after peripherals (8-9 when RSI_BLE_MAX_NBR_CENTRALS = 2)
 */
#define PERIPHERAL1 0
#define PERIPHERAL2 1
#define PERIPHERAL3 2
#define PERIPHERAL4 3
#define PERIPHERAL5 4
#define PERIPHERAL6 5
#define PERIPHERAL7 6
#define PERIPHERAL8 7
#define CENTRAL1    RSI_BLE_MAX_NBR_PERIPHERALS
#define CENTRAL2    RSI_BLE_MAX_NBR_PERIPHERALS + 1

/*=======================================================================*/
//   REMOTE DEVICE IDENTIFICATION
/*=======================================================================*/

/**
 * @brief Connection method selection
 * 
 * Used by:
 * - gap_user_event_hdlrs.c: Device filtering and connection logic
 * 
 * Defines how to identify remote devices for connection:
 * - CONN_BY_NAME: Connect to devices based on their advertised name
 * - CONN_BY_ADDR: Connect to devices based on their Bluetooth address
 */
#ifndef CONN_BY_ADDR
#define CONN_BY_ADDR 1
#endif

#ifndef CONN_BY_NAME
#define CONN_BY_NAME 2
#endif

// <h> Connection Method
// <o CONNECT_OPTION> Connect to remote by
// <CONN_BY_ADDR=> Address
// <CONN_BY_NAME=> Name
// <i> Default: CONN_BY_NAME
#ifndef CONNECT_OPTION
#define CONNECT_OPTION CONN_BY_NAME
#endif

// <h> Remote Device Names (connect by name)
// <i> Used when CONNECT_OPTION is CONN_BY_NAME. Names to look for when scanning.

// <s RSI_REMOTE_DEVICE_NAME1> Remote device name 1
#ifndef RSI_REMOTE_DEVICE_NAME1
#define RSI_REMOTE_DEVICE_NAME1 "dev1"
#endif

// <s RSI_REMOTE_DEVICE_NAME2> Remote device name 2
#ifndef RSI_REMOTE_DEVICE_NAME2
#define RSI_REMOTE_DEVICE_NAME2 "dev2"
#endif

// <s RSI_REMOTE_DEVICE_NAME3> Remote device name 3
#ifndef RSI_REMOTE_DEVICE_NAME3
#define RSI_REMOTE_DEVICE_NAME3 "dev3"
#endif

// <s RSI_REMOTE_DEVICE_NAME4> Remote device name 4
#ifndef RSI_REMOTE_DEVICE_NAME4
#define RSI_REMOTE_DEVICE_NAME4 "dev4"
#endif

// <s RSI_REMOTE_DEVICE_NAME5> Remote device name 5
#ifndef RSI_REMOTE_DEVICE_NAME5
#define RSI_REMOTE_DEVICE_NAME5 "dev5"
#endif

// <s RSI_REMOTE_DEVICE_NAME6> Remote device name 6
#ifndef RSI_REMOTE_DEVICE_NAME6
#define RSI_REMOTE_DEVICE_NAME6 "dev6"
#endif

// <s RSI_REMOTE_DEVICE_NAME7> Remote device name 7
#ifndef RSI_REMOTE_DEVICE_NAME7
#define RSI_REMOTE_DEVICE_NAME7 "dev7"
#endif

// <s RSI_REMOTE_DEVICE_NAME8> Remote device name 8
#ifndef RSI_REMOTE_DEVICE_NAME8
#define RSI_REMOTE_DEVICE_NAME8 "dev8"
#endif

// </h>

// <h> Remote Device Addresses (connect by address)
// <i> Used when CONNECT_OPTION is CONN_BY_ADDR. Format XX:XX:XX:XX:XX:XX

// <o RSI_BLE_DEV_ADDR_TYPE> Remote address type
//   <0=> LE_PUBLIC_ADDRESS
//   <1=> LE_RANDOM_ADDRESS
// <i> Default: LE_PUBLIC_ADDRESS
#ifndef RSI_BLE_DEV_ADDR_TYPE
#define RSI_BLE_DEV_ADDR_TYPE LE_PUBLIC_ADDRESS
#endif

// <s RSI_BLE_DEV_1_ADDR> Remote device address 1
#ifndef RSI_BLE_DEV_1_ADDR
#define RSI_BLE_DEV_1_ADDR "D0:DC:AD:8F:4A:B3"
#endif

// <s RSI_BLE_DEV_2_ADDR> Remote device address 2
#ifndef RSI_BLE_DEV_2_ADDR
#define RSI_BLE_DEV_2_ADDR "FC:5D:44:9D:78:D9"
#endif

// <s RSI_BLE_DEV_3_ADDR> Remote device address 3
#ifndef RSI_BLE_DEV_3_ADDR
#define RSI_BLE_DEV_3_ADDR "D2:24:F1:E6:F1:2B"
#endif

// <s RSI_BLE_DEV_4_ADDR> Remote device address 4
#ifndef RSI_BLE_DEV_4_ADDR
#define RSI_BLE_DEV_4_ADDR "DB:DE:BA:13:35:ED"
#endif

// <s RSI_BLE_DEV_5_ADDR> Remote device address 5
#ifndef RSI_BLE_DEV_5_ADDR
#define RSI_BLE_DEV_5_ADDR "D2:65:66:D4:8D:88"
#endif

// <s RSI_BLE_DEV_6_ADDR> Remote device address 6
#ifndef RSI_BLE_DEV_6_ADDR
#define RSI_BLE_DEV_6_ADDR "E6:D2:90:DA:A4:38"
#endif

// <s RSI_BLE_DEV_7_ADDR> Remote device address 7
#ifndef RSI_BLE_DEV_7_ADDR
#define RSI_BLE_DEV_7_ADDR "CC:F0:A6:28:C1:1B"
#endif

// <s RSI_BLE_DEV_8_ADDR> Remote device address 8
#ifndef RSI_BLE_DEV_8_ADDR
#define RSI_BLE_DEV_8_ADDR "D4:64:29:7B:20:B2"
#endif

// </h>

// </h>

/*=======================================================================*/
//   DEVICE NAME CONFIGURATION
/*=======================================================================*/

// <h> Device Name
// <s RSI_BT_LOCAL_NAME> Local BLE device name (advertising)
// <i> Default: BLE_8S2M_COEX_DEMO. Use BLE_AE_APP for extended adv if desired.
#ifndef RSI_BT_LOCAL_NAME
#define RSI_BT_LOCAL_NAME "BLE_8S2M_COEX_DEMO"
#endif

// </h>

/*=======================================================================*/
//   CONNECTION PARAMETERS
/*=======================================================================*/

// <h> Central-to-Peripheral Connection Parameters
// <i> Used when initiating connection in central role (units: interval 1.25ms, timeout 10ms)

// <o C2P12_CONNECTION_INTERVAL_MIN> Connection interval min (N*1.25ms)
// <0x0006-0x0C80:1>
// <i> Default: 0x00C8 (250ms)
#define C2P12_CONNECTION_INTERVAL_MIN 0x00C8

// <o C2P12_CONNECTION_INTERVAL_MAX> Connection interval max (N*1.25ms)
// <0x0006-0x0C80:1>
// <i> Default: 0x00C8 (250ms)
#define C2P12_CONNECTION_INTERVAL_MAX 0x00C8

// <o C2P12_CONNECTION_LATENCY> Slave latency (connection events)
// <0-499:1>
// <i> Default: 0
#define C2P12_CONNECTION_LATENCY 0x0000

// <o BLE_MAX_SUPPORETED_SUPERVISION_TIMEOUT> Max supervision timeout (N*10ms)
// <0x000A-0x0C80:1>
// <i> Default: 0x0C80 (32s)
#define BLE_MAX_SUPPORETED_SUPERVISION_TIMEOUT 0x0C80
#define C2P12_SUPERVISION_TIMEOUT              MIN(BLE_MAX_SUPPORETED_SUPERVISION_TIMEOUT, (4 * C2P12_CONNECTION_INTERVAL_MAX))

// <o LE_SCAN_INTERVAL_CONN> Scan interval for connection (N*0.625ms)
// <0x0004-0x4000:1>
// <i> Default: 0x0050 (50ms)
#define LE_SCAN_INTERVAL_CONN 0x0050

// <o LE_SCAN_WINDOW_CONN> Scan window for connection (N*0.625ms)
// <0x0004-0x4000:1>
// <i> Default: 0x0050 (50ms)
#define LE_SCAN_WINDOW_CONN 0x0050

// </h>

// Address and name lengths (fixed; not in Configuration Wizard)
#define RSI_REM_DEV_ADDR_LEN 18 // Remote device address string length (e.g. "XX:XX:XX:XX:XX:XX")
#define RSI_REM_DEV_NAME_LEN 31 // Maximum remote device name length

/*=======================================================================*/
//   ADVERTISING AND SCANNING CONTROL
/*=======================================================================*/

// <h> Advertising and Scanning Defaults
// <q ADV_ENABLED_DEFAULT> Enable advertising at startup (peripheral role)
// <i> Default: 1
#define ADV_ENABLED_DEFAULT 1
// <q SCAN_ENABLED_DEFAULT> Enable scanning at startup (central role)
// <i> Default: 1
#define SCAN_ENABLED_DEFAULT 1

// </h>

/*=======================================================================*/
//   CONNECTION EVENTS AND PHY CONFIGURATION
/*=======================================================================*/

// <h> Connection Events and PHY
// <o RSI_BLE_NUM_CONN_EVENTS> Number of connection events (queued/processed)
// <1-64:1>
// <i> Default: 20
#ifndef RSI_BLE_NUM_CONN_EVENTS
#define RSI_BLE_NUM_CONN_EVENTS 20
#endif

// <q RSI_BLE_SET_SCAN_RESP_DATA_FROM_HOST> Host sets scan response data
// <i> 1 = Host sets custom scan response, 0 = Default. Default: 1
#ifndef RSI_BLE_SET_SCAN_RESP_DATA_FROM_HOST
#define RSI_BLE_SET_SCAN_RESP_DATA_FROM_HOST 1
#endif

// <q RSI_BLE_DISABLE_CODED_PHY_FROM_HOST> Disable coded PHY (long range)
// <i> 1 = Disable, 0 = Enable. Default: 0
#ifndef RSI_BLE_DISABLE_CODED_PHY_FROM_HOST
#define RSI_BLE_DISABLE_CODED_PHY_FROM_HOST 0
#endif

// </h>

/*=======================================================================*/
//   SECURITY AND MTU SETTINGS
/*=======================================================================*/

// <h> MTU and Debug
// <o MAX_MTU_SIZE> Maximum MTU size (bytes)
// <23-512:1>
// <i> Default: 240 (BLE 4.2+)
#define MAX_MTU_SIZE 240

// </h>

/**
 * @brief Security pairing configuration
 * 
 * Used by:
 * - gap_user_event_hdlrs.c: rsi_ble_event_remote_features() - rsi_ble_smp_pair_request()
 * 
 * MITM (Man-In-The-Middle) protection level for BLE security pairing.
 * 
 * MOVED TO: components/device/silabs/si91x/wireless/smp/inc/smp_config.h
 * This is an SMP-specific security configuration owned by SMP component.
 */

/*=======================================================================*/
//   PER-CONNECTION CONFIGURATION MACROS (DEFAULTS)
/*=======================================================================*/

// <h> Per-Connection Configuration (8 Peripherals + 2 Centrals)
// <i> GAP-owned: CONN_*, DLE_*, ADD_TO_ACCEPTLIST. SMP/GATT options in their config headers.

// <h> Peripheral 1
// <q ADD_TO_ACCEPTLIST_P1> Add to accept list
#ifndef ADD_TO_ACCEPTLIST_P1
#define ADD_TO_ACCEPTLIST_P1 0
#endif

// <o CONN_INTERVAL_P1> Connection interval (N*1.25ms)
#ifndef CONN_INTERVAL_P1
#define CONN_INTERVAL_P1 240
#endif

// <o CONN_LATENCY_P1> Connection latency
#ifndef CONN_LATENCY_P1
#define CONN_LATENCY_P1 0
#endif

// <o CONN_SUPERVISION_TIMEOUT_P1> Supervision timeout (N*10ms)
#ifndef CONN_SUPERVISION_TIMEOUT_P1
#define CONN_SUPERVISION_TIMEOUT_P1 400
#endif

// <o DLE_BUFFER_MODE_P1> DLE buffer mode (0=Small, 1=Big)
#ifndef DLE_BUFFER_MODE_P1
#define DLE_BUFFER_MODE_P1 0
#endif

// <o DLE_BUFFER_COUNT_P1> DLE buffer count
#ifndef DLE_BUFFER_COUNT_P1
#define DLE_BUFFER_COUNT_P1 2
#endif

// <o RSI_BLE_MAX_DATA_LEN_P1> Max data length (bytes)
#ifndef RSI_BLE_MAX_DATA_LEN_P1
#define RSI_BLE_MAX_DATA_LEN_P1 20
#endif

// </h>

// <h> Peripheral 2
// <q ADD_TO_ACCEPTLIST_P2> Add to accept list
#ifndef ADD_TO_ACCEPTLIST_P2
#define ADD_TO_ACCEPTLIST_P2 0
#endif

// <o CONN_INTERVAL_P2> Connection interval (N*1.25ms)
#ifndef CONN_INTERVAL_P2
#define CONN_INTERVAL_P2 240
#endif

// <o CONN_LATENCY_P2> Connection latency
#ifndef CONN_LATENCY_P2
#define CONN_LATENCY_P2 0
#endif

// <o CONN_SUPERVISION_TIMEOUT_P2> Supervision timeout (N*10ms)
#ifndef CONN_SUPERVISION_TIMEOUT_P2
#define CONN_SUPERVISION_TIMEOUT_P2 400
#endif

// <o DLE_BUFFER_MODE_P2> DLE buffer mode (0=Small, 1=Big)
#ifndef DLE_BUFFER_MODE_P2
#define DLE_BUFFER_MODE_P2 0
#endif

// <o DLE_BUFFER_COUNT_P2> DLE buffer count
#ifndef DLE_BUFFER_COUNT_P2
#define DLE_BUFFER_COUNT_P2 2
#endif

// <o RSI_BLE_MAX_DATA_LEN_P2> Max data length (bytes)
#ifndef RSI_BLE_MAX_DATA_LEN_P2
#define RSI_BLE_MAX_DATA_LEN_P2 20
#endif

// </h>

// <h> Peripheral 3
// <q ADD_TO_ACCEPTLIST_P3> Add to accept list
#ifndef ADD_TO_ACCEPTLIST_P3
#define ADD_TO_ACCEPTLIST_P3 0
#endif

// <o CONN_INTERVAL_P3> Connection interval (N*1.25ms)
#ifndef CONN_INTERVAL_P3
#define CONN_INTERVAL_P3 240
#endif

// <o CONN_LATENCY_P3> Connection latency
#ifndef CONN_LATENCY_P3
#define CONN_LATENCY_P3 0
#endif

// <o CONN_SUPERVISION_TIMEOUT_P3> Supervision timeout (N*10ms)
#ifndef CONN_SUPERVISION_TIMEOUT_P3
#define CONN_SUPERVISION_TIMEOUT_P3 400
#endif

// <o DLE_BUFFER_MODE_P3> DLE buffer mode (0=Small, 1=Big)
#ifndef DLE_BUFFER_MODE_P3
#define DLE_BUFFER_MODE_P3 0
#endif

// <o DLE_BUFFER_COUNT_P3> DLE buffer count
#ifndef DLE_BUFFER_COUNT_P3
#define DLE_BUFFER_COUNT_P3 2
#endif

// <o RSI_BLE_MAX_DATA_LEN_P3> Max data length (bytes)
#ifndef RSI_BLE_MAX_DATA_LEN_P3
#define RSI_BLE_MAX_DATA_LEN_P3 20
#endif

// </h>

// <h> Peripheral 4
// <q ADD_TO_WHITELIST_P4> Add to accept list
#ifndef ADD_TO_WHITELIST_P4
#define ADD_TO_WHITELIST_P4 0
#endif

// <o CONN_INTERVAL_P4> Connection interval (N*1.25ms)
#ifndef CONN_INTERVAL_P4
#define CONN_INTERVAL_P4 240
#endif

// <o CONN_LATENCY_P4> Connection latency
#ifndef CONN_LATENCY_P4
#define CONN_LATENCY_P4 0
#endif

// <o CONN_SUPERVISION_TIMEOUT_P4> Supervision timeout (N*10ms)
#ifndef CONN_SUPERVISION_TIMEOUT_P4
#define CONN_SUPERVISION_TIMEOUT_P4 400
#endif

// <o DLE_BUFFER_MODE_P4> DLE buffer mode (0=Small, 1=Big)
#ifndef DLE_BUFFER_MODE_P4
#define DLE_BUFFER_MODE_P4 0
#endif

// <o DLE_BUFFER_COUNT_P4> DLE buffer count
#ifndef DLE_BUFFER_COUNT_P4
#define DLE_BUFFER_COUNT_P4 2
#endif

// <o RSI_BLE_MAX_DATA_LEN_P4> Max data length (bytes)
#ifndef RSI_BLE_MAX_DATA_LEN_P4
#define RSI_BLE_MAX_DATA_LEN_P4 20
#endif

// </h>

// <h> Peripheral 5
// <q ADD_TO_WHITELIST_P5> Add to accept list
#ifndef ADD_TO_WHITELIST_P5
#define ADD_TO_WHITELIST_P5 0
#endif

// <o CONN_INTERVAL_P5> Connection interval (N*1.25ms)
#ifndef CONN_INTERVAL_P5
#define CONN_INTERVAL_P5 240
#endif

// <o CONN_LATENCY_P5> Connection latency
#ifndef CONN_LATENCY_P5
#define CONN_LATENCY_P5 0
#endif

// <o CONN_SUPERVISION_TIMEOUT_P5> Supervision timeout (N*10ms)
#ifndef CONN_SUPERVISION_TIMEOUT_P5
#define CONN_SUPERVISION_TIMEOUT_P5 400
#endif

// <o DLE_BUFFER_MODE_P5> DLE buffer mode (0=Small, 1=Big)
#ifndef DLE_BUFFER_MODE_P5
#define DLE_BUFFER_MODE_P5 0
#endif

// <o DLE_BUFFER_COUNT_P5> DLE buffer count
#ifndef DLE_BUFFER_COUNT_P5
#define DLE_BUFFER_COUNT_P5 2
#endif

// <o RSI_BLE_MAX_DATA_LEN_P5> Max data length (bytes)
#ifndef RSI_BLE_MAX_DATA_LEN_P5
#define RSI_BLE_MAX_DATA_LEN_P5 20
#endif

// </h>

// <h> Peripheral 6
// <q ADD_TO_WHITELIST_P6> Add to accept list
#ifndef ADD_TO_WHITELIST_P6
#define ADD_TO_WHITELIST_P6 0
#endif

// <o CONN_INTERVAL_P6> Connection interval (N*1.25ms)
#ifndef CONN_INTERVAL_P6
#define CONN_INTERVAL_P6 240
#endif

// <o CONN_LATENCY_P6> Connection latency
#ifndef CONN_LATENCY_P6
#define CONN_LATENCY_P6 0
#endif

// <o CONN_SUPERVISION_TIMEOUT_P6> Supervision timeout (N*10ms)
#ifndef CONN_SUPERVISION_TIMEOUT_P6
#define CONN_SUPERVISION_TIMEOUT_P6 400
#endif

// <o DLE_BUFFER_MODE_P6> DLE buffer mode (0=Small, 1=Big)
#ifndef DLE_BUFFER_MODE_P6
#define DLE_BUFFER_MODE_P6 0
#endif

// <o DLE_BUFFER_COUNT_P6> DLE buffer count
#ifndef DLE_BUFFER_COUNT_P6
#define DLE_BUFFER_COUNT_P6 2
#endif

// <o RSI_BLE_MAX_DATA_LEN_P6> Max data length (bytes)
#ifndef RSI_BLE_MAX_DATA_LEN_P6
#define RSI_BLE_MAX_DATA_LEN_P6 20
#endif

// </h>

// <h> Peripheral 7
// <q ADD_TO_WHITELIST_P7> Add to accept list
#ifndef ADD_TO_WHITELIST_P7
#define ADD_TO_WHITELIST_P7 0
#endif

// <o CONN_INTERVAL_P7> Connection interval (N*1.25ms)
#ifndef CONN_INTERVAL_P7
#define CONN_INTERVAL_P7 240
#endif

// <o CONN_LATENCY_P7> Connection latency
#ifndef CONN_LATENCY_P7
#define CONN_LATENCY_P7 0
#endif

// <o CONN_SUPERVISION_TIMEOUT_P7> Supervision timeout (N*10ms)
#ifndef CONN_SUPERVISION_TIMEOUT_P7
#define CONN_SUPERVISION_TIMEOUT_P7 400
#endif

// <o DLE_BUFFER_MODE_P7> DLE buffer mode (0=Small, 1=Big)
#ifndef DLE_BUFFER_MODE_P7
#define DLE_BUFFER_MODE_P7 0
#endif

// <o DLE_BUFFER_COUNT_P7> DLE buffer count
#ifndef DLE_BUFFER_COUNT_P7
#define DLE_BUFFER_COUNT_P7 2
#endif

// <o RSI_BLE_MAX_DATA_LEN_P7> Max data length (bytes)
#ifndef RSI_BLE_MAX_DATA_LEN_P7
#define RSI_BLE_MAX_DATA_LEN_P7 20
#endif

// </h>

// <h> Peripheral 8
// <q ADD_TO_WHITELIST_P8> Add to accept list
#ifndef ADD_TO_WHITELIST_P8
#define ADD_TO_WHITELIST_P8 0
#endif

// <o CONN_INTERVAL_P8> Connection interval (N*1.25ms)
#ifndef CONN_INTERVAL_P8
#define CONN_INTERVAL_P8 240
#endif

// <o CONN_LATENCY_P8> Connection latency
#ifndef CONN_LATENCY_P8
#define CONN_LATENCY_P8 0
#endif

// <o CONN_SUPERVISION_TIMEOUT_P8> Supervision timeout (N*10ms)
#ifndef CONN_SUPERVISION_TIMEOUT_P8
#define CONN_SUPERVISION_TIMEOUT_P8 400
#endif

// <o DLE_BUFFER_MODE_P8> DLE buffer mode (0=Small, 1=Big)
#ifndef DLE_BUFFER_MODE_P8
#define DLE_BUFFER_MODE_P8 0
#endif

// <o DLE_BUFFER_COUNT_P8> DLE buffer count
#ifndef DLE_BUFFER_COUNT_P8
#define DLE_BUFFER_COUNT_P8 2
#endif

// <o RSI_BLE_MAX_DATA_LEN_P8> Max data length (bytes)
#ifndef RSI_BLE_MAX_DATA_LEN_P8
#define RSI_BLE_MAX_DATA_LEN_P8 20
#endif

// </h>

// <h> Central 1
// <q ADD_TO_ACCEPTLIST_C1> Add to accept list
#ifndef ADD_TO_ACCEPTLIST_C1
#define ADD_TO_ACCEPTLIST_C1 0
#endif

// <o CONN_INTERVAL_C1> Connection interval (N*1.25ms)
#ifndef CONN_INTERVAL_C1
#define CONN_INTERVAL_C1 240
#endif

// <o CONN_LATENCY_C1> Connection latency
#ifndef CONN_LATENCY_C1
#define CONN_LATENCY_C1 0
#endif

// <o CONN_SUPERVISION_TIMEOUT_C1> Supervision timeout (N*10ms)
#ifndef CONN_SUPERVISION_TIMEOUT_C1
#define CONN_SUPERVISION_TIMEOUT_C1 400
#endif

// <o DLE_BUFFER_MODE_C1> DLE buffer mode (0=Small, 1=Big)
#ifndef DLE_BUFFER_MODE_C1
#define DLE_BUFFER_MODE_C1 0
#endif

// <o DLE_BUFFER_COUNT_C1> DLE buffer count
#ifndef DLE_BUFFER_COUNT_C1
#define DLE_BUFFER_COUNT_C1 2
#endif

// <o RSI_BLE_MAX_DATA_LEN_C1> Max data length (bytes)
#ifndef RSI_BLE_MAX_DATA_LEN_C1
#define RSI_BLE_MAX_DATA_LEN_C1 20
#endif

// </h>

// <h> Central 2
// <q ADD_TO_ACCEPTLIST_C2> Add to accept list
#ifndef ADD_TO_ACCEPTLIST_C2
#define ADD_TO_ACCEPTLIST_C2 0
#endif

// <o CONN_INTERVAL_C2> Connection interval (N*1.25ms)
#ifndef CONN_INTERVAL_C2
#define CONN_INTERVAL_C2 240
#endif

// <o CONN_LATENCY_C2> Connection latency
#ifndef CONN_LATENCY_C2
#define CONN_LATENCY_C2 0
#endif

// <o CONN_SUPERVISION_TIMEOUT_C2> Supervision timeout (N*10ms)
#ifndef CONN_SUPERVISION_TIMEOUT_C2
#define CONN_SUPERVISION_TIMEOUT_C2 400
#endif

// <o DLE_BUFFER_MODE_C2> DLE buffer mode (0=Small, 1=Big)
#ifndef DLE_BUFFER_MODE_C2
#define DLE_BUFFER_MODE_C2 0
#endif

// <o DLE_BUFFER_COUNT_C2> DLE buffer count
#ifndef DLE_BUFFER_COUNT_C2
#define DLE_BUFFER_COUNT_C2 2
#endif

// <o RSI_BLE_MAX_DATA_LEN_C2> Max data length (bytes)
#ifndef RSI_BLE_MAX_DATA_LEN_C2
#define RSI_BLE_MAX_DATA_LEN_C2 20
#endif

// </h>
// </h>
// </h>
// <<< end of configuration section >>>

#endif // GAP_CONFIG_H
