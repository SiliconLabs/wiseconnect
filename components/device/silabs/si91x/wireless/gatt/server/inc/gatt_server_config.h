/*******************************************************************************
* @file  gatt_server_config.h
* @brief GATT Server Component - Configuration Macros
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

#ifndef GATT_SERVER_CONFIG_H
#define GATT_SERVER_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> GATT Server Configuration

// <h> MTU and Attribute Table
// <o LOCAL_MTU_SIZE> Local MTU size (MTU exchange response)
// <23-512:1>
// <i> Default: 232
#ifndef LOCAL_MTU_SIZE
#define LOCAL_MTU_SIZE 232
#endif

// <o RSI_BLE_MAX_NBR_ATT_SERV> Maximum number of attribute services
// <1-32:1>
// <i> Default: 10
#ifndef RSI_BLE_MAX_NBR_ATT_SERV
#define RSI_BLE_MAX_NBR_ATT_SERV 10
#endif

// <o RSI_BLE_MAX_NBR_ATT_REC> Maximum number of attribute records
// <1-255:1>
// <i> Default: 80
#ifndef RSI_BLE_MAX_NBR_ATT_REC
#define RSI_BLE_MAX_NBR_ATT_REC 80
#endif

// <o RSI_BLE_NUM_REC_BYTES> GATT record size (n*16 bytes)
// <1-256:1>
// <i> e.g. 40 => 640 bytes. Default: 40
#ifndef RSI_BLE_NUM_REC_BYTES
#define RSI_BLE_NUM_REC_BYTES 40
#endif

// </h>

// <h> TX Notifications/Indications (per connection)
// <i> P1-P8 = peripherals, C1-C2 = centrals

// <h> Peripheral 1
// <q TX_NOTIFICATIONS_TO_P1> TX notifications
#ifndef TX_NOTIFICATIONS_TO_P1
#define TX_NOTIFICATIONS_TO_P1 1
#endif
// <q TX_INDICATIONS_TO_P1> TX indications
#ifndef TX_INDICATIONS_TO_P1
#define TX_INDICATIONS_TO_P1 0
#endif
// </h>

// <h> Peripheral 2
// <q TX_NOTIFICATIONS_TO_P2> TX notifications
#ifndef TX_NOTIFICATIONS_TO_P2
#define TX_NOTIFICATIONS_TO_P2 1
#endif
// <q TX_INDICATIONS_TO_P2> TX indications
#ifndef TX_INDICATIONS_TO_P2
#define TX_INDICATIONS_TO_P2 0
#endif
// </h>

// <h> Peripheral 3
// <q TX_NOTIFICATIONS_TO_P3> TX notifications
#ifndef TX_NOTIFICATIONS_TO_P3
#define TX_NOTIFICATIONS_TO_P3 1
#endif
// <q TX_INDICATIONS_TO_P3> TX indications
#ifndef TX_INDICATIONS_TO_P3
#define TX_INDICATIONS_TO_P3 0
#endif
// </h>

// <h> Peripheral 4
// <q TX_NOTIFICATIONS_TO_P4> TX notifications
#ifndef TX_NOTIFICATIONS_TO_P4
#define TX_NOTIFICATIONS_TO_P4 1
#endif
// <q TX_INDICATIONS_TO_P4> TX indications
#ifndef TX_INDICATIONS_TO_P4
#define TX_INDICATIONS_TO_P4 0
#endif
// </h>

// <h> Peripheral 5
// <q TX_NOTIFICATIONS_TO_P5> TX notifications
#ifndef TX_NOTIFICATIONS_TO_P5
#define TX_NOTIFICATIONS_TO_P5 1
#endif
// <q TX_INDICATIONS_TO_P5> TX indications
#ifndef TX_INDICATIONS_TO_P5
#define TX_INDICATIONS_TO_P5 0
#endif
// </h>

// <h> Peripheral 6
// <q TX_NOTIFICATIONS_TO_P6> TX notifications
#ifndef TX_NOTIFICATIONS_TO_P6
#define TX_NOTIFICATIONS_TO_P6 1
#endif
// <q TX_INDICATIONS_TO_P6> TX indications
#ifndef TX_INDICATIONS_TO_P6
#define TX_INDICATIONS_TO_P6 0
#endif
// </h>

// <h> Peripheral 7
// <q TX_NOTIFICATIONS_TO_P7> TX notifications
#ifndef TX_NOTIFICATIONS_TO_P7
#define TX_NOTIFICATIONS_TO_P7 1
#endif
// <q TX_INDICATIONS_TO_P7> TX indications
#ifndef TX_INDICATIONS_TO_P7
#define TX_INDICATIONS_TO_P7 0
#endif
// </h>

// <h> Peripheral 8
// <q TX_NOTIFICATIONS_TO_P8> TX notifications
#ifndef TX_NOTIFICATIONS_TO_P8
#define TX_NOTIFICATIONS_TO_P8 1
#endif
// <q TX_INDICATIONS_TO_P8> TX indications
#ifndef TX_INDICATIONS_TO_P8
#define TX_INDICATIONS_TO_P8 0
#endif
// </h>

// <h> Central 1
// <q TX_NOTIFICATIONS_TO_C1> TX notifications
#ifndef TX_NOTIFICATIONS_TO_C1
#define TX_NOTIFICATIONS_TO_C1 1
#endif
// <q TX_INDICATIONS_TO_C1> TX indications
#ifndef TX_INDICATIONS_TO_C1
#define TX_INDICATIONS_TO_C1 0
#endif
// </h>

// <h> Central 2
// <q TX_NOTIFICATIONS_TO_C2> TX notifications
#ifndef TX_NOTIFICATIONS_TO_C2
#define TX_NOTIFICATIONS_TO_C2 1
#endif
// <q TX_INDICATIONS_TO_C2> TX indications
#ifndef TX_INDICATIONS_TO_C2
#define TX_INDICATIONS_TO_C2 0
#endif
// </h>

// </h>
// </h>
// <<< end of configuration section >>>

#endif // GATT_SERVER_CONFIG_H
