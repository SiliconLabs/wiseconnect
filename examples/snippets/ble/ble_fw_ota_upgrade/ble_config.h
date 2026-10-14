/*******************************************************************************
* @file  ble_config.h
* @brief BLE configuration parameters for OTA firmware upgrade application.
*        Defines stack sizes, advertising/connection parameters, GATT settings,
*        application events, and OTA-related macros used by the BLE FW OTA
*        upgrade example.
*******************************************************************************
* # License
* <b>Copyright 2026 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef BLE_CONFIG_H
#define BLE_CONFIG_H

/******************************************************
 *                      Macros
 ******************************************************/

//! BLE random address used when address resolution is enabled
#define RSI_BLE_SET_RAND_ADDR "00:23:A7:12:34:56"

//! Accept list control values
#define CLEAR_ACCEPTLIST              0x00
#define ADD_DEVICE_TO_ACCEPTLIST      0x01
#define DELETE_DEVICE_FROM_ACCEPTLIST 0x02

//! All PHYs (1M, 2M, Coded) for PHY update
#define ALL_PHYS 0x00

//! Enable (1) or disable (0) BLE address resolution
#define RSI_BLE_DEV_ADDR_RESOLUTION_ENABLE 0

/*=======================================================================*/
//! BLE stack capacity (SoC vs NCP: reduced for MCU interface)
/*=======================================================================*/
#ifdef SLI_SI91X_MCU_INTERFACE
#define RSI_BLE_MAX_NBR_ATT_REC     20 //! Max GATT attribute records
#define RSI_BLE_MAX_NBR_PERIPHERALS 1  //! Max peripheral connections
#define RSI_BLE_MAX_NBR_CENTRALS    1  //! Max central connections
#define RSI_BLE_NUM_CONN_EVENTS     4  //! Connection event slots
#else
#define RSI_BLE_MAX_NBR_ATT_REC     80
#define RSI_BLE_MAX_NBR_PERIPHERALS 3
#define RSI_BLE_MAX_NBR_CENTRALS    1
#define RSI_BLE_NUM_CONN_EVENTS     20
#endif

#define RSI_BLE_MAX_NBR_ATT_SERV 10 //! Max GATT services

#define RSI_BLE_GATT_ASYNC_ENABLE 0 //! GATT async operations (0 = sync)
#define RSI_BLE_GATT_INIT         0 //! GATT init option

/* Number of BLE GATT RECORD SIZE IN (n*16 BYTES), eg:(0x40*16)=1024 bytes */
#define RSI_BLE_NUM_REC_BYTES 0x40

#define RSI_BLE_INDICATE_CONFIRMATION_FROM_HOST 0 //! Host sends indicate confirm

/*=======================================================================*/
//! OTA firmware upgrade application macros
/*=======================================================================*/
//! Local device name advertised for OTA FW upgrade profile
#define RSI_BLE_OTA_FWUP_PROFILE "BLE_OTA_FWUP"

//! OTA target: TA (transceiver) or M4 (MCU) or combined (TA+M4) firmware
#define TA_FW_UP       0
#define M4_FW_UP       1
#define COMBINED_FW_UP 2

//! Select which firmware to upgrade (TA_FW_UP or M4_FW_UP or COMBINED_FW_UP)
#define FW_UPGRADE_TYPE TA_FW_UP

/*=======================================================================*/
//! Advertising command parameters
/*=======================================================================*/

#define RSI_BLE_ADV_TYPE          UNDIR_CONN
#define RSI_BLE_ADV_FILTER_TYPE   ALLOW_SCAN_REQ_ANY_CONN_REQ_ANY
#define RSI_BLE_ADV_DIR_ADDR_TYPE LE_PUBLIC_ADDRESS
#define RSI_BLE_ADV_DIR_ADDR      "00:15:83:6A:64:17"

#define RSI_BLE_ADV_INT_MIN     0x100 //! Min advertising interval
#define RSI_BLE_ADV_INT_MAX     0x200 //! Max advertising interval
#define RSI_BLE_ADV_CHANNEL_MAP 0x07  //! Advertising channel map (37, 38, 39)

//! Advertise status
#define RSI_BLE_START_ADV 0x01 //! Start the advertising process
#define RSI_BLE_STOP_ADV  0x00 //! Stop the advertising process

//! BLE Tx Power Index On Air
#define RSI_BLE_PWR_INX 30

//! BLE active hardware power save options
#define BLE_DISABLE_DUTY_CYCLING 0
#define BLE_DUTY_CYCLING         1
#define BLR_DUTY_CYCLING         2
#define BLE_4X_PWR_SAVE_MODE     4
#define RSI_BLE_PWR_SAVE_OPTIONS BLE_DISABLE_DUTY_CYCLING

//! Advertise types

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

//! Advertising flags (discoverability mode)
#define LE_LIMITED_DISCOVERABLE 0x01
#define LE_GENERAL_DISCOVERABLE 0x02
#define LE_BR_EDR_NOT_SUPPORTED 0x04

//! Advertise filters (scan/connection from any or accept list only)
#define ALLOW_SCAN_REQ_ANY_CONN_REQ_ANY               0x00
#define ALLOW_SCAN_REQ_WHITE_LIST_CONN_REQ_ANY        0x01
#define ALLOW_SCAN_REQ_ANY_CONN_REQ_WHITE_LIST        0x02
#define ALLOW_SCAN_REQ_WHITE_LIST_CONN_REQ_WHITE_LIST 0x03

//! BLE address types
#define LE_PUBLIC_ADDRESS            0x00
#define LE_RANDOM_ADDRESS            0x01
#define LE_RESOLVABLE_PUBLIC_ADDRESS 0x02
#define LE_RESOLVABLE_RANDOM_ADDRESS 0x03

/*=======================================================================*/

/*=======================================================================*/
//! Connection parameters
/*=======================================================================*/
#define LE_SCAN_INTERVAL 0x0100 //! Scan interval (units of 0.625 ms)
#define LE_SCAN_WINDOW   0x0050 //! Scan window (units of 0.625 ms)

#define CONNECTION_INTERVAL_MIN 6   //! Min connection interval (7.5 ms units)
#define CONNECTION_INTERVAL_MAX 6   //! Max connection interval (7.5 ms units)
#define CONNECTION_LATENCY      1   //! Slave latency (number of events)
#define SUPERVISION_TIMEOUT     800 //! Supervision timeout (10 ms units)

/*=======================================================================*/
//! Data length extension (DLE) and PHY parameters
/*=======================================================================*/
#define TX_LEN  0xFB   //! Requested TX payload length
#define TX_TIME 0x0148 //! Requested TX packet duration (us)

//! PHY rate: 0x01 = 1Mbps, 0x02 = 2Mbps, 0x04 = Coded PHY
#define TX_PHY_RATE     0x02
#define RX_PHY_RATE     0x02
#define CODDED_PHY_RATE 0x00 //! Coded PHY rate option

#define DLE_BUFFER_MODE      1   //! DLE buffer mode
#define DLE_BUFFER_COUNT     2   //! DLE buffer count
#define RSI_BLE_MAX_DATA_LEN 230 //! Max TX/RX data length (bytes)

/*=======================================================================*/
//! GATT attribute properties
/*=======================================================================*/
#define RSI_BLE_ATT_PROPERTY_READ  0x02
#define RSI_BLE_ATT_PROPERTY_WRITE 0x08

/*=======================================================================*/
//! Application event IDs (used in app event handler)
/*=======================================================================*/
#define RSI_APP_EVENT_ADV_REPORT         0x00 //! Advertisement report (central)
#define RSI_BLE_CONN_EVENT               0x01 //! Connection established
#define RSI_BLE_MTU_EX_EVENT             0x02 //! MTU exchange complete
#define RSI_BLE_PHY_UPDATE_EVENT         0x03 //! PHY update complete
#define RSI_BLE_CONN_UPDATE_EVENT        0x04 //! Connection parameter update
#define RSI_BLE_DISCONN_EVENT            0x05 //! Disconnection
#define RSI_BLE_GATT_WRITE_EVENT         0x06 //! GATT write (OTA data, etc.)
#define RSI_BLE_RECEIVE_REMOTE_FEATURES  0x07 //! Remote features received
#define RSI_APP_EVENT_DATA_LENGTH_CHANGE 0x08 //! Data length changed
#define RSI_BLE_MORE_DATA_REQ_EVENT      0x09 //! More data request (flow ctrl)

//! OTA firmware upgrade success event
#define FW_UP_SUCCESS 0x10003

//! BLE GATT characteristic declaration UUID (0x2803)
#define RSI_BLE_CHAR_SERV_UUID 0x2803

#define UUID_SIZE 16 //! Standard 128-bit UUID size in bytes

#define FIRMWARE_VERSION_SIZE 18 //! Length of firmware version string for OTA
#define BD_ADDR_STRING_SIZE   18 //! BD address string length "XX:XX:XX:XX:XX:XX"
/*=======================================================================*/

/*=======================================================================*/
//! Scan command parameters (used in central role for discovering OTA peripheral)
/*=======================================================================*/
#define RSI_BLE_SCAN_TYPE        SCAN_TYPE_ACTIVE
#define RSI_BLE_SCAN_FILTER_TYPE SCAN_FILTER_TYPE_ALL

//! Scan control
#define RSI_BLE_START_SCAN 0x01
#define RSI_BLE_STOP_SCAN  0x00

//! Scan type: active (sends scan requests) or passive
#define SCAN_TYPE_ACTIVE  0x01
#define SCAN_TYPE_PASSIVE 0x00

//! Filter: all advertisers or accept list only
#define SCAN_FILTER_TYPE_ALL             0x00
#define SCAN_FILTER_TYPE_ONLY_WHITE_LIST 0x01

//! Antenna selection for RF
#define RSI_SEL_INTERNAL_ANTENNA 0x00
#define RSI_SEL_EXTERNAL_ANTENNA 0x01

/**************************************************************************/

#endif
