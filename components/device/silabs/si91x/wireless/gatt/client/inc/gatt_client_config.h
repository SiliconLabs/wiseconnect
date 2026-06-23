/*******************************************************************************
* @file  gatt_client_config.h
* @brief GATT Client Component - Configuration Macros
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

#ifndef GATT_CLIENT_CONFIG_H
#define GATT_CLIENT_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> GATT Client Configuration

// <h> Remote Service/Characteristic UUIDs
// <i> 16-bit UUIDs for GATT Client data transfer (write, notify, indicate)

// <o RSI_BLE_CLIENT_WRITE_SERVICE_UUID> Write service UUID
// <0x0-0xFFFF>
// <i> Default: 0x180D (Heart Rate)
#ifndef RSI_BLE_CLIENT_WRITE_SERVICE_UUID
#define RSI_BLE_CLIENT_WRITE_SERVICE_UUID 0x180D
#endif

// <o RSI_BLE_CLIENT_WRITE_CHAR_UUID> Write characteristic UUID
// <0x0-0xFFFF>
// <i> Default: 0x2A39 (Heart Rate Control Point)
#ifndef RSI_BLE_CLIENT_WRITE_CHAR_UUID
#define RSI_BLE_CLIENT_WRITE_CHAR_UUID 0x2A39
#endif

// <o RSI_BLE_CLIENT_WRITE_NO_RESP_SERVICE_UUID> Write no-response service UUID
// <0x0-0xFFFF>
// <i> Default: 0x1802 (Immediate Alert)
#ifndef RSI_BLE_CLIENT_WRITE_NO_RESP_SERVICE_UUID
#define RSI_BLE_CLIENT_WRITE_NO_RESP_SERVICE_UUID 0x1802
#endif

// <o RSI_BLE_CLIENT_WRITE_NO_RESP_CHAR_UUID> Write no-response characteristic UUID
// <0x0-0xFFFF>
// <i> Default: 0x2A06 (Alert Level)
#ifndef RSI_BLE_CLIENT_WRITE_NO_RESP_CHAR_UUID
#define RSI_BLE_CLIENT_WRITE_NO_RESP_CHAR_UUID 0x2A06
#endif

// <o RSI_BLE_CLIENT_INIDCATIONS_SERVICE_UUID> Indications service UUID
// <0x0-0xFFFF>
// <i> Default: 0x1809 (Glucose)
#ifndef RSI_BLE_CLIENT_INIDCATIONS_SERVICE_UUID
#define RSI_BLE_CLIENT_INIDCATIONS_SERVICE_UUID 0x1809
#endif

// <o RSI_BLE_CLIENT_INIDCATIONS_CHAR_UUID> Indications characteristic UUID
// <0x0-0xFFFF>
// <i> Default: 0x2A1C (Glucose Measurement)
#ifndef RSI_BLE_CLIENT_INIDCATIONS_CHAR_UUID
#define RSI_BLE_CLIENT_INIDCATIONS_CHAR_UUID 0x2A1C
#endif

// <o RSI_BLE_CLIENT_NOTIFICATIONS_SERVICE_UUID> Notifications service UUID
// <0x0-0xFFFF>
// <i> Default: 0x180D (Heart Rate)
#ifndef RSI_BLE_CLIENT_NOTIFICATIONS_SERVICE_UUID
#define RSI_BLE_CLIENT_NOTIFICATIONS_SERVICE_UUID 0x180D
#endif

// <o RSI_BLE_CLIENT_NOTIFICATIONS_CHAR_UUID> Notifications characteristic UUID
// <0x0-0xFFFF>
// <i> Default: 0x2A37 (Heart Rate Measurement)
#ifndef RSI_BLE_CLIENT_NOTIFICATIONS_CHAR_UUID
#define RSI_BLE_CLIENT_NOTIFICATIONS_CHAR_UUID 0x2A37
#endif

// </h>

// <o RSI_MAX_PROFILE_CNT> Maximum remote device profiles to support
// <1-16:1>
// <i> Default: 10
#ifndef RSI_MAX_PROFILE_CNT
#define RSI_MAX_PROFILE_CNT 10
#endif

// <q RSI_BLE_INDICATE_CONFIRMATION_FROM_HOST> Host sends indication confirmation
// <i> 1 = Application calls rsi_ble_indicate_confirm(), 0 = Firmware auto. Default: 1
#ifndef RSI_BLE_INDICATE_CONFIRMATION_FROM_HOST
#define RSI_BLE_INDICATE_CONFIRMATION_FROM_HOST 1
#endif

// <h> Profile Discovery and Data Transfer (per connection)
// <i> P1-P8 = peripherals, C1-C2 = centrals

// <h> Peripheral 1
// <q PROFILE_QUERY_P1> Profile query
#ifndef PROFILE_QUERY_P1
#define PROFILE_QUERY_P1 1
#endif
// <q DATA_TRANSFER_P1> Data transfer
#ifndef DATA_TRANSFER_P1
#define DATA_TRANSFER_P1 1
#endif
// <q RX_NOTIFICATIONS_FROM_P1> RX notifications
#ifndef RX_NOTIFICATIONS_FROM_P1
#define RX_NOTIFICATIONS_FROM_P1 1
#endif
// <q RX_INDICATIONS_FROM_P1> RX indications
#ifndef RX_INDICATIONS_FROM_P1
#define RX_INDICATIONS_FROM_P1 0
#endif
// <q TX_WRITES_TO_P1> TX writes
#ifndef TX_WRITES_TO_P1
#define TX_WRITES_TO_P1 0
#endif
// <q TX_WRITES_NO_RESP_TO_P1> TX writes (no response)
#ifndef TX_WRITES_NO_RESP_TO_P1
#define TX_WRITES_NO_RESP_TO_P1 0
#endif
// </h>

// <h> Peripheral 2
// <q PROFILE_QUERY_P2> Profile query
#ifndef PROFILE_QUERY_P2
#define PROFILE_QUERY_P2 1
#endif
// <q DATA_TRANSFER_P2> Data transfer
#ifndef DATA_TRANSFER_P2
#define DATA_TRANSFER_P2 1
#endif
// <q RX_NOTIFICATIONS_FROM_P2> RX notifications
#ifndef RX_NOTIFICATIONS_FROM_P2
#define RX_NOTIFICATIONS_FROM_P2 1
#endif
// <q RX_INDICATIONS_FROM_P2> RX indications
#ifndef RX_INDICATIONS_FROM_P2
#define RX_INDICATIONS_FROM_P2 0
#endif
// <q TX_WRITES_TO_P2> TX writes
#ifndef TX_WRITES_TO_P2
#define TX_WRITES_TO_P2 0
#endif
// <q TX_WRITES_NO_RESP_TO_P2> TX writes (no response)
#ifndef TX_WRITES_NO_RESP_TO_P2
#define TX_WRITES_NO_RESP_TO_P2 0
#endif
// </h>

// <h> Peripheral 3
// <q PROFILE_QUERY_P3> Profile query
#ifndef PROFILE_QUERY_P3
#define PROFILE_QUERY_P3 1
#endif
// <q DATA_TRANSFER_P3> Data transfer
#ifndef DATA_TRANSFER_P3
#define DATA_TRANSFER_P3 1
#endif
// <q RX_NOTIFICATIONS_FROM_P3> RX notifications
#ifndef RX_NOTIFICATIONS_FROM_P3
#define RX_NOTIFICATIONS_FROM_P3 1
#endif
// <q RX_INDICATIONS_FROM_P3> RX indications
#ifndef RX_INDICATIONS_FROM_P3
#define RX_INDICATIONS_FROM_P3 0
#endif
// <q TX_WRITES_TO_P3> TX writes
#ifndef TX_WRITES_TO_P3
#define TX_WRITES_TO_P3 0
#endif
// <q TX_WRITES_NO_RESP_TO_P3> TX writes (no response)
#ifndef TX_WRITES_NO_RESP_TO_P3
#define TX_WRITES_NO_RESP_TO_P3 0
#endif
// </h>

// <h> Peripheral 4
// <q PROFILE_QUERY_P4> Profile query
#ifndef PROFILE_QUERY_P4
#define PROFILE_QUERY_P4 1
#endif
// <q DATA_TRANSFER_P4> Data transfer
#ifndef DATA_TRANSFER_P4
#define DATA_TRANSFER_P4 1
#endif
// <q RX_NOTIFICATIONS_FROM_P4> RX notifications
#ifndef RX_NOTIFICATIONS_FROM_P4
#define RX_NOTIFICATIONS_FROM_P4 1
#endif
// <q RX_INDICATIONS_FROM_P4> RX indications
#ifndef RX_INDICATIONS_FROM_P4
#define RX_INDICATIONS_FROM_P4 0
#endif
// <q TX_WRITES_TO_P4> TX writes
#ifndef TX_WRITES_TO_P4
#define TX_WRITES_TO_P4 0
#endif
// <q TX_WRITES_NO_RESP_TO_P4> TX writes (no response)
#ifndef TX_WRITES_NO_RESP_TO_P4
#define TX_WRITES_NO_RESP_TO_P4 0
#endif
// </h>

// <h> Peripheral 5
// <q PROFILE_QUERY_P5> Profile query
#ifndef PROFILE_QUERY_P5
#define PROFILE_QUERY_P5 1
#endif
// <q DATA_TRANSFER_P5> Data transfer
#ifndef DATA_TRANSFER_P5
#define DATA_TRANSFER_P5 1
#endif
// <q RX_NOTIFICATIONS_FROM_P5> RX notifications
#ifndef RX_NOTIFICATIONS_FROM_P5
#define RX_NOTIFICATIONS_FROM_P5 1
#endif
// <q RX_INDICATIONS_FROM_P5> RX indications
#ifndef RX_INDICATIONS_FROM_P5
#define RX_INDICATIONS_FROM_P5 0
#endif
// <q TX_WRITES_TO_P5> TX writes
#ifndef TX_WRITES_TO_P5
#define TX_WRITES_TO_P5 0
#endif
// <q TX_WRITES_NO_RESP_TO_P5> TX writes (no response)
#ifndef TX_WRITES_NO_RESP_TO_P5
#define TX_WRITES_NO_RESP_TO_P5 0
#endif
// </h>

// <h> Peripheral 6
// <q PROFILE_QUERY_P6> Profile query
#ifndef PROFILE_QUERY_P6
#define PROFILE_QUERY_P6 1
#endif
// <q DATA_TRANSFER_P6> Data transfer
#ifndef DATA_TRANSFER_P6
#define DATA_TRANSFER_P6 1
#endif
// <q RX_NOTIFICATIONS_FROM_P6> RX notifications
#ifndef RX_NOTIFICATIONS_FROM_P6
#define RX_NOTIFICATIONS_FROM_P6 1
#endif
// <q RX_INDICATIONS_FROM_P6> RX indications
#ifndef RX_INDICATIONS_FROM_P6
#define RX_INDICATIONS_FROM_P6 0
#endif
// <q TX_WRITES_TO_P6> TX writes
#ifndef TX_WRITES_TO_P6
#define TX_WRITES_TO_P6 0
#endif
// <q TX_WRITES_NO_RESP_TO_P6> TX writes (no response)
#ifndef TX_WRITES_NO_RESP_TO_P6
#define TX_WRITES_NO_RESP_TO_P6 0
#endif
// </h>

// <h> Peripheral 7
// <q PROFILE_QUERY_P7> Profile query
#ifndef PROFILE_QUERY_P7
#define PROFILE_QUERY_P7 1
#endif
// <q DATA_TRANSFER_P7> Data transfer
#ifndef DATA_TRANSFER_P7
#define DATA_TRANSFER_P7 1
#endif
// <q RX_NOTIFICATIONS_FROM_P7> RX notifications
#ifndef RX_NOTIFICATIONS_FROM_P7
#define RX_NOTIFICATIONS_FROM_P7 1
#endif
// <q RX_INDICATIONS_FROM_P7> RX indications
#ifndef RX_INDICATIONS_FROM_P7
#define RX_INDICATIONS_FROM_P7 0
#endif
// <q TX_WRITES_TO_P7> TX writes
#ifndef TX_WRITES_TO_P7
#define TX_WRITES_TO_P7 0
#endif
// <q TX_WRITES_NO_RESP_TO_P7> TX writes (no response)
#ifndef TX_WRITES_NO_RESP_TO_P7
#define TX_WRITES_NO_RESP_TO_P7 0
#endif
// </h>

// <h> Peripheral 8
// <q PROFILE_QUERY_P8> Profile query
#ifndef PROFILE_QUERY_P8
#define PROFILE_QUERY_P8 1
#endif
// <q DATA_TRANSFER_P8> Data transfer
#ifndef DATA_TRANSFER_P8
#define DATA_TRANSFER_P8 1
#endif
// <q RX_NOTIFICATIONS_FROM_P8> RX notifications
#ifndef RX_NOTIFICATIONS_FROM_P8
#define RX_NOTIFICATIONS_FROM_P8 1
#endif
// <q RX_INDICATIONS_FROM_P8> RX indications
#ifndef RX_INDICATIONS_FROM_P8
#define RX_INDICATIONS_FROM_P8 0
#endif
// <q TX_WRITES_TO_P8> TX writes
#ifndef TX_WRITES_TO_P8
#define TX_WRITES_TO_P8 0
#endif
// <q TX_WRITES_NO_RESP_TO_P8> TX writes (no response)
#ifndef TX_WRITES_NO_RESP_TO_P8
#define TX_WRITES_NO_RESP_TO_P8 0
#endif
// </h>

// <h> Central 1
// <q PROFILE_QUERY_C1> Profile query
#ifndef PROFILE_QUERY_C1
#define PROFILE_QUERY_C1 1
#endif
// <q DATA_TRANSFER_C1> Data transfer
#ifndef DATA_TRANSFER_C1
#define DATA_TRANSFER_C1 1
#endif
// <q RX_NOTIFICATIONS_FROM_C1> RX notifications
#ifndef RX_NOTIFICATIONS_FROM_C1
#define RX_NOTIFICATIONS_FROM_C1 1
#endif
// <q RX_INDICATIONS_FROM_C1> RX indications
#ifndef RX_INDICATIONS_FROM_C1
#define RX_INDICATIONS_FROM_C1 0
#endif
// <q TX_WRITES_TO_C1> TX writes
#ifndef TX_WRITES_TO_C1
#define TX_WRITES_TO_C1 0
#endif
// <q TX_WRITES_NO_RESP_TO_C1> TX writes (no response)
#ifndef TX_WRITES_NO_RESP_TO_C1
#define TX_WRITES_NO_RESP_TO_C1 0
#endif
// </h>

// <h> Central 2
// <q PROFILE_QUERY_C2> Profile query
#ifndef PROFILE_QUERY_C2
#define PROFILE_QUERY_C2 1
#endif
// <q DATA_TRANSFER_C2> Data transfer
#ifndef DATA_TRANSFER_C2
#define DATA_TRANSFER_C2 1
#endif
// <q RX_NOTIFICATIONS_FROM_C2> RX notifications
#ifndef RX_NOTIFICATIONS_FROM_C2
#define RX_NOTIFICATIONS_FROM_C2 0
#endif
// <q RX_INDICATIONS_FROM_C2> RX indications
#ifndef RX_INDICATIONS_FROM_C2
#define RX_INDICATIONS_FROM_C2 0
#endif
// <q TX_WRITES_TO_C2> TX writes
#ifndef TX_WRITES_TO_C2
#define TX_WRITES_TO_C2 0
#endif
// <q TX_WRITES_NO_RESP_TO_C2> TX writes (no response)
#ifndef TX_WRITES_NO_RESP_TO_C2
#define TX_WRITES_NO_RESP_TO_C2 0
#endif
// </h>

// </h>
// </h>
// <<< end of configuration section >>>

#endif // GATT_CLIENT_CONFIG_H
