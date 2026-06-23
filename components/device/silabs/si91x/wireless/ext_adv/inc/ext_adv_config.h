/*******************************************************************************
* @file  ext_adv_config.h
* @brief Extended Advertising Configuration Header
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

#ifndef EXT_ADV_CONFIG_H
#define EXT_ADV_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Extended Advertising Configuration

// <h> Firmware Feature Configuration
// <q RSI_BLE_ENABLE_ADV_EXTN> Enable Extended Advertising in firmware
// <i> 0 = Legacy only, 1 = Extended enabled. Default: 1
#ifndef RSI_BLE_ENABLE_ADV_EXTN
#define RSI_BLE_ENABLE_ADV_EXTN 1
#endif

// <o RSI_BLE_AE_MAX_ADV_SETS> Maximum advertising sets (1-15)
// <1-15:1>
// <i> Default: 2
#ifndef RSI_BLE_AE_MAX_ADV_SETS
#define RSI_BLE_AE_MAX_ADV_SETS 2
#endif

// </h>

// <h> Application Feature Flags
// <i> ADV_ENABLED_DEFAULT and SCAN_ENABLED_DEFAULT are configured in gap_config.h (GAP).

// <q BLE_AE_PERIODIC_ADV_EN> Enable periodic advertising (Set 1)
// <i> Default: 0
#ifndef BLE_AE_PERIODIC_ADV_EN
#define BLE_AE_PERIODIC_ADV_EN 0
#endif

// <q ADV_SET2> Enable second advertising set
// <i> Default: 1
#ifndef ADV_SET2
#define ADV_SET2 1
#endif

#ifndef WLAN_TRANSIENT_CASE
#define WLAN_TRANSIENT_CASE 0
#endif

// </h>

// <h> Address Configuration
// <i> Directed-advertising peer address and type are set per advertising set (Set 1 / Set 2 blocks).

// <s RSI_BLE_SET_RAND_ADDR> Local random address for AE set 1 (XX:XX:XX:XX:XX:XX)
// <i> Default: "CC:44:33:44:55:CC"
#ifndef RSI_BLE_SET_RAND_ADDR
#define RSI_BLE_SET_RAND_ADDR "CC:44:33:44:55:CC"
#endif

// </h>

/*=======================================================================*/
//  ! PHY DEFINITIONS (constants - not in wizard)
/*=======================================================================*/

#ifndef PHY_1M
#define PHY_1M 0x01
#endif
#ifndef PHY_2M
#define PHY_2M 0x02
#endif
#ifndef PHY_LE_CODED
#define PHY_LE_CODED 0x04
#endif

#ifndef PHY_1M_2M
#define PHY_1M_2M (PHY_1M | PHY_2M)
#endif
#ifndef PHY_1M_CODED
#define PHY_1M_CODED (PHY_1M | PHY_LE_CODED)
#endif
#ifndef PHY_2M_CODED
#define PHY_2M_CODED (PHY_2M | PHY_LE_CODED)
#endif
#ifndef PHY_ALL
#define PHY_ALL (PHY_1M | PHY_2M | PHY_LE_CODED)
#endif

// <h> PHY and connection initiation
// <o INITIATING_PHYS> PHYs for connection initiation
// <PHY_1M=> 1M only
// <PHY_2M=> 2M only
// <PHY_LE_CODED=> Coded only
// <PHY_1M_2M=> 1M + 2M
// <PHY_1M_CODED=> 1M + Coded
// <PHY_2M_CODED=> 2M + Coded
// <PHY_ALL=> 1M + 2M + Coded
// <i> Default: PHY_ALL
#ifndef INITIATING_PHYS
#define INITIATING_PHYS PHY_ALL
#endif
// </h>

/*=======================================================================*/
//  ! ADVERTISING HANDLES (fixed - not in wizard)
/*=======================================================================*/

#ifndef BLE_AE_ADV_HNDL_SET_1
#define BLE_AE_ADV_HNDL_SET_1 0x00
#endif
#ifndef BLE_AE_ADV_HNDL_SET_2
#define BLE_AE_ADV_HNDL_SET_2 0x01
#endif

// <h> Advertising parameters – Set 1
// <i> N * 0.625 ms for intervals. Channel map: 0x01=Ch37, 0x02=Ch38, 0x04=Ch39, 0x07=All

// <o BLE_AE_OWN_ADDR_TYPE_SET_1> Own address type
// <LE_PUBLIC_ADDRESS=> Public address
// <LE_RANDOM_ADDRESS=> Random address
// <i> Default: LE_RANDOM_ADDRESS
#ifndef BLE_AE_OWN_ADDR_TYPE_SET_1
#define BLE_AE_OWN_ADDR_TYPE_SET_1 LE_RANDOM_ADDRESS
#endif

// <s BLE_AE_PEER_DEV_ADDR_SET_1> Peer device address
// <i> Default: "00:00:00:00:00:00"
#ifndef BLE_AE_PEER_DEV_ADDR_SET_1
#define BLE_AE_PEER_DEV_ADDR_SET_1 "00:00:00:00:00:00"
#endif

// <o BLE_AE_PEER_ADDR_TYPE_SET_1> Peer address type
// <LE_PUBLIC_ADDRESS=> Public address
// <LE_RANDOM_ADDRESS=> Random address
// <i> Must match BLE_AE_PEER_DEV_ADDR_SET_1. Default: LE_RANDOM_ADDRESS
#ifndef BLE_AE_PEER_ADDR_TYPE_SET_1
#define BLE_AE_PEER_ADDR_TYPE_SET_1 LE_RANDOM_ADDRESS
#endif

// <o BLE_AE_ADV_INT_MIN_SET_1> Primary adv interval min
// <0x20-0x4000:1>
// <i> Default: 0x20 (20 ms)
#ifndef BLE_AE_ADV_INT_MIN_SET_1
#define BLE_AE_ADV_INT_MIN_SET_1 0x20
#endif

// <o BLE_AE_ADV_INT_MAX_SET_1> Primary adv interval max
// <0x20-0x4000:1>
// <i> Default: 0x20 (20 ms)
#ifndef BLE_AE_ADV_INT_MAX_SET_1
#define BLE_AE_ADV_INT_MAX_SET_1 0x20
#endif

// <o BLE_AE_ADV_CHANNEL_MAP_SET_1> Channel map
// <0x01=> Ch37
// <0x02=> Ch38
// <0x04=> Ch39
// <0x07=> All
// <i> Default: 0x07
#ifndef BLE_AE_ADV_CHANNEL_MAP_SET_1
#define BLE_AE_ADV_CHANNEL_MAP_SET_1 0x07
#endif

// <o BLE_AE_ADV_FILTER_POLICY_SET_1> Filter policy
// <0=> Allow all
// <1=> Whitelist scan
// <2=> Whitelist connect
// <3=> Whitelist both
// <i> Default: 0
#ifndef BLE_AE_ADV_FILTER_POLICY_SET_1
#define BLE_AE_ADV_FILTER_POLICY_SET_1 0x00
#endif

// <o BLE_AE_ADV_TX_PWR_SET_1> TX power (-127..20 dBm, 0x7F=no preference)
// <0x00-0x7F:1>
// <i> Default: 0x7F
#ifndef BLE_AE_ADV_TX_PWR_SET_1
#define BLE_AE_ADV_TX_PWR_SET_1 0x7f
#endif

// <o BLE_AE_PRIMARY_ADV_PHY_SET_1> Primary adv PHY
// <0x01=> 1M
// <0x03=> Coded
// <i> Default: 0x01
#ifndef BLE_AE_PRIMARY_ADV_PHY_SET_1
#define BLE_AE_PRIMARY_ADV_PHY_SET_1 0x01
#endif

// <o BLE_AE_SEC_ADV_MAX_SKIP_SET_1> Secondary adv max skip
// <0x00-0x0F:1>
// <i> Default: 0
#ifndef BLE_AE_SEC_ADV_MAX_SKIP_SET_1
#define BLE_AE_SEC_ADV_MAX_SKIP_SET_1 0x00
#endif

// <o BLE_AE_SECONDARY_ADV_PHY_SET_1> Secondary adv PHY
// <0x01=> 1M
// <0x02=> 2M
// <0x03=> Coded
// <i> Default: 0x01
#ifndef BLE_AE_SECONDARY_ADV_PHY_SET_1
#define BLE_AE_SECONDARY_ADV_PHY_SET_1 0x01
#endif

// <o BLE_AE_ADV_SID_SET_1> Advertising SID (Set ID)
// <0x00-0x0F:1>
// <i> Default: 0
#ifndef BLE_AE_ADV_SID_SET_1
#define BLE_AE_ADV_SID_SET_1 0x00
#endif

// <q BLE_AE_SCAN_REQ_NOTIF_EN_SET_1> Scan request notification enable
// <i> Default: 1
#ifndef BLE_AE_SCAN_REQ_NOTIF_EN_SET_1
#define BLE_AE_SCAN_REQ_NOTIF_EN_SET_1 0x01
#endif

// </h>

// <h> Advertising parameters – Set 2

// <o BLE_AE_OWN_ADDR_TYPE_SET_2> Own address type
// <LE_PUBLIC_ADDRESS=> Public address
// <LE_RANDOM_ADDRESS=> Random address
// <i> Default: LE_PUBLIC_ADDRESS
#ifndef BLE_AE_OWN_ADDR_TYPE_SET_2
#define BLE_AE_OWN_ADDR_TYPE_SET_2 LE_PUBLIC_ADDRESS
#endif

// <s BLE_AE_PEER_DEV_ADDR_SET_2> Peer device address
// <i> Default: "00:00:00:00:00:00"
#ifndef BLE_AE_PEER_DEV_ADDR_SET_2
#define BLE_AE_PEER_DEV_ADDR_SET_2 "00:00:00:00:00:00"
#endif

// <o BLE_AE_PEER_ADDR_TYPE_SET_2> Peer address type
// <LE_PUBLIC_ADDRESS=> Public address
// <LE_RANDOM_ADDRESS=> Random address
// <i> Must match BLE_AE_PEER_DEV_ADDR_SET_2. Default: LE_RANDOM_ADDRESS
#ifndef BLE_AE_PEER_ADDR_TYPE_SET_2
#define BLE_AE_PEER_ADDR_TYPE_SET_2 LE_RANDOM_ADDRESS
#endif

// <o BLE_AE_ADV_INT_MIN_SET_2> Primary adv interval min
// <0x20-0x4000:1>
// <i> Default: 0x30
#ifndef BLE_AE_ADV_INT_MIN_SET_2
#define BLE_AE_ADV_INT_MIN_SET_2 0x30
#endif

// <o BLE_AE_ADV_INT_MAX_SET_2> Primary adv interval max
// <0x20-0x4000:1>
// <i> Default: 0x30
#ifndef BLE_AE_ADV_INT_MAX_SET_2
#define BLE_AE_ADV_INT_MAX_SET_2 0x30
#endif

// <o BLE_AE_ADV_CHANNEL_MAP_SET_2> Channel map
// <0x01=> Ch37
// <0x02=> Ch38
// <0x04=> Ch39
// <0x07=> All
// <i> Default: 0x07
#ifndef BLE_AE_ADV_CHANNEL_MAP_SET_2
#define BLE_AE_ADV_CHANNEL_MAP_SET_2 0x07
#endif

// <o BLE_AE_ADV_FILTER_POLICY_SET_2> Filter policy
// <0=> Allow all
// <1=> Whitelist scan
// <2=> Whitelist connect
// <3=> Whitelist both
// <i> Default: 0
#ifndef BLE_AE_ADV_FILTER_POLICY_SET_2
#define BLE_AE_ADV_FILTER_POLICY_SET_2 0x00
#endif

// <o BLE_AE_ADV_TX_PWR_SET_2> TX power (-127..20 dBm, 0x7F=no preference)
// <0x00-0x7F:1>
// <i> Default: 0x7F
#ifndef BLE_AE_ADV_TX_PWR_SET_2
#define BLE_AE_ADV_TX_PWR_SET_2 0x7f
#endif

// <o BLE_AE_PRIMARY_ADV_PHY_SET_2> Primary adv PHY
// <0x01=> 1M
// <0x03=> Coded
// <i> Default: 0x01
#ifndef BLE_AE_PRIMARY_ADV_PHY_SET_2
#define BLE_AE_PRIMARY_ADV_PHY_SET_2 0x01
#endif

// <o BLE_AE_SEC_ADV_MAX_SKIP_SET_2> Secondary adv max skip
// <0x00-0x0F:1>
// <i> Default: 0
#ifndef BLE_AE_SEC_ADV_MAX_SKIP_SET_2
#define BLE_AE_SEC_ADV_MAX_SKIP_SET_2 0x00
#endif

// <o BLE_AE_SECONDARY_ADV_PHY_SET_2> Secondary adv PHY
// <0x01=> 1M
// <0x02=> 2M
// <0x03=> Coded
// <i> Default: 0x01
#ifndef BLE_AE_SECONDARY_ADV_PHY_SET_2
#define BLE_AE_SECONDARY_ADV_PHY_SET_2 0x01
#endif

// <o BLE_AE_ADV_SID_SET_2> Advertising SID (Set ID)
// <0x00-0x0F:1>
// <i> Default: 1
#ifndef BLE_AE_ADV_SID_SET_2
#define BLE_AE_ADV_SID_SET_2 0x01
#endif

// <q BLE_AE_SCAN_REQ_NOTIF_EN_SET_2> Scan request notification enable
// <i> Default: 1
#ifndef BLE_AE_SCAN_REQ_NOTIF_EN_SET_2
#define BLE_AE_SCAN_REQ_NOTIF_EN_SET_2 0x01
#endif

// </h>

/*=======================================================================*/
//  ! ADVERTISING PROPERTIES
/*=======================================================================*/

/**
 * @brief BLE Advertising Event Property Bit Flags
 * 
 * These flags define the properties of extended advertising events.
 * Multiple flags can be combined using bitwise OR.
 */
#ifndef BLE_CONNECTABLE_ADV
#define BLE_CONNECTABLE_ADV (1 << 0) //! Connectable advertising
#endif

#ifndef BLE_SCANNABLE_ADV
#define BLE_SCANNABLE_ADV (1 << 1) //! Scannable advertising
#endif

#ifndef BLE_LOW_DUTY_DIR_CONN_ADV
#define BLE_LOW_DUTY_DIR_CONN_ADV (1 << 2) //! Low duty cycle directed connectable
#endif

#ifndef BLE_HIGH_DUTY_DIR_CONN_ADV
#define BLE_HIGH_DUTY_DIR_CONN_ADV (1 << 3) //! High duty cycle directed connectable
#endif

#ifndef BLE_LEGACY_ADV
#define BLE_LEGACY_ADV (1 << 4) //! Legacy advertising PDUs
#endif

#ifndef BLE_ANONYMOUS_ADV
#define BLE_ANONYMOUS_ADV (1 << 5) //! Anonymous advertising
#endif

#ifndef BLE_TX_WR_ADV
#define BLE_TX_WR_ADV (1 << 6) //! Include TxPower in advertising PDU
#endif

#ifndef BLE_CONNECTABLE_SCANNABLE_ADV
#define BLE_CONNECTABLE_SCANNABLE_ADV (BLE_CONNECTABLE_ADV | BLE_SCANNABLE_ADV)
#endif
#ifndef BLE_CONNECTABLE_LEGACY_ADV
#define BLE_CONNECTABLE_LEGACY_ADV (BLE_CONNECTABLE_ADV | BLE_LEGACY_ADV)
#endif

// <h> Advertising event properties
// <i> Select event property set (symbol-style so default shows label like GAP)

// <o BLE_AE_ADV_EVNT_PROP_SET_1> Event properties – Set 1
// <BLE_CONNECTABLE_ADV=> Connectable only
// <BLE_SCANNABLE_ADV=> Scannable only
// <BLE_CONNECTABLE_SCANNABLE_ADV=> Connectable + Scannable
// <BLE_CONNECTABLE_LEGACY_ADV=> Connectable + Legacy
// <i> Default: BLE_CONNECTABLE_ADV
#ifndef BLE_AE_ADV_EVNT_PROP_SET_1
#define BLE_AE_ADV_EVNT_PROP_SET_1 BLE_CONNECTABLE_ADV
#endif

// <o BLE_AE_ADV_EVNT_PROP_SET_2> Event properties – Set 2
// <BLE_CONNECTABLE_ADV=> Connectable only
// <BLE_SCANNABLE_ADV=> Scannable only
// <BLE_CONNECTABLE_SCANNABLE_ADV=> Connectable + Scannable
// <BLE_CONNECTABLE_LEGACY_ADV=> Connectable + Legacy
// <i> Default: BLE_CONNECTABLE_ADV
#ifndef BLE_AE_ADV_EVNT_PROP_SET_2
#define BLE_AE_ADV_EVNT_PROP_SET_2 BLE_CONNECTABLE_ADV
#endif

// </h>

/*=======================================================================*/
//  ! ADVERTISING DATA (type constants - not in wizard)
/*=======================================================================*/

#ifndef BLE_AE_ADV_DATA_TYPE
#define BLE_AE_ADV_DATA_TYPE 0x01
#endif
#ifndef BLE_AE_PERIODIC_ADV_DATA_TYPE
#define BLE_AE_PERIODIC_ADV_DATA_TYPE 0x02
#endif
#ifndef BLE_AE_SCAN_RSP_DATA_TYPE
#define BLE_AE_SCAN_RSP_DATA_TYPE 0x03
#endif

// <h> Advertising data
// <o BLE_AE_ADV_DATA_LEN> Advertising data length (bytes)
// <1-251:1>
// <i> Default: 0x19 (25)
#ifndef BLE_AE_ADV_DATA_LEN
#define BLE_AE_ADV_DATA_LEN 0x19
#endif

// <s BLE_AE_ADV_DATA> Advertising data string
// <i> Default: AE_PERIPHERAL_DATA_1
#ifndef BLE_AE_ADV_DATA
#define BLE_AE_ADV_DATA "AE_PERIPHERAL_DATA_1"
#endif

// </h>

#ifndef BLE_AE_ADV_DATA_OPERATION_SET_1
#define BLE_AE_ADV_DATA_OPERATION_SET_1 0x03
#endif
#ifndef BLE_AE_ADV_DATA_FRAGMENT_PREF_SET_1
#define BLE_AE_ADV_DATA_FRAGMENT_PREF_SET_1 0x00
#endif
#ifndef BLE_AE_ADV_DATA_OPERATION_SET_2
#define BLE_AE_ADV_DATA_OPERATION_SET_2 0x03
#endif
#ifndef BLE_AE_ADV_DATA_FRAGMENT_PREF_SET_2
#define BLE_AE_ADV_DATA_FRAGMENT_PREF_SET_2 0x00
#endif

// <h> Advertising enable (duration and max events)
// <o BLE_AE_ADV_DUR_SET_1> Adv duration Set 1 (0=infinite, N= N*10ms)
// <0-65535:1>
// <i> Default: 0
#ifndef BLE_AE_ADV_DUR_SET_1
#define BLE_AE_ADV_DUR_SET_1 0x00
#endif

// <o BLE_AE_ADV_MAX_AE_EVENTS_SET_1> Max extended adv events Set 1 (0=no limit)
// <0-65535:1>
// <i> Default: 0
#ifndef BLE_AE_ADV_MAX_AE_EVENTS_SET_1
#define BLE_AE_ADV_MAX_AE_EVENTS_SET_1 0x00
#endif

// <o BLE_AE_ADV_DUR_SET_2> Adv duration Set 2 (0=infinite)
// <0-65535:1>
// <i> Default: 0
#ifndef BLE_AE_ADV_DUR_SET_2
#define BLE_AE_ADV_DUR_SET_2 0x00
#endif

// <o BLE_AE_ADV_MAX_AE_EVENTS_SET_2> Max extended adv events Set 2 (0=no limit)
// <0-65535:1>
// <i> Default: 0
#ifndef BLE_AE_ADV_MAX_AE_EVENTS_SET_2
#define BLE_AE_ADV_MAX_AE_EVENTS_SET_2 0x00
#endif

// </h>

// <h> Scan parameters
// <i> Intervals/windows in N * 0.625 ms

// <o BLE_AE_SCAN_OWN_ADDR_TYPE> Scanner own address type
// <LE_PUBLIC_ADDRESS=> Public address
// <LE_RANDOM_ADDRESS=> Random address
// <i> Default: LE_PUBLIC_ADDRESS (reference AE scan params)
#ifndef BLE_AE_SCAN_OWN_ADDR_TYPE
#define BLE_AE_SCAN_OWN_ADDR_TYPE LE_PUBLIC_ADDRESS
#endif

// <o BLE_AE_SCAN_FILTER_TYPE> Scan filter policy
// <0=> Accept all
// <1=> Only accept whitelist
// <i> Default: 0
#ifndef BLE_AE_SCAN_FILTER_TYPE
#define BLE_AE_SCAN_FILTER_TYPE 0x00
#endif

// <o PRI_PHY_BLE_AE_SCAN_TYPE> Primary PHY scan type
// <0=> Passive
// <1=> Active
// <i> Default: 1
#ifndef PRI_PHY_BLE_AE_SCAN_TYPE
#define PRI_PHY_BLE_AE_SCAN_TYPE 0x01
#endif

// <o PRI_PHY_LE_AE_SCAN_INTERVAL> Primary PHY scan interval (N * 0.625 ms)
// <0x0004-0x4000:1>
// <i> Default: 0x0100
#ifndef PRI_PHY_LE_AE_SCAN_INTERVAL
#define PRI_PHY_LE_AE_SCAN_INTERVAL 0x0100
#endif

// <o PRI_PHY_LE_AE_SCAN_WINDOW> Primary PHY scan window (N * 0.625 ms)
// <0x0004-0x4000:1>
// <i> Default: 0x0050
#ifndef PRI_PHY_LE_AE_SCAN_WINDOW
#define PRI_PHY_LE_AE_SCAN_WINDOW 0x0050
#endif

// <o SEC_PHY_BLE_AE_SCAN_TYPE> Secondary PHY scan type
// <0=> Passive
// <1=> Active
// <i> Default: 1
#ifndef SEC_PHY_BLE_AE_SCAN_TYPE
#define SEC_PHY_BLE_AE_SCAN_TYPE 0x01
#endif

// <o SEC_PHY_LE_AE_SCAN_INTERVAL> Secondary PHY scan interval (N * 0.625 ms)
// <0x0004-0x4000:1>
// <i> Default: 0x0100
#ifndef SEC_PHY_LE_AE_SCAN_INTERVAL
#define SEC_PHY_LE_AE_SCAN_INTERVAL 0x0100
#endif

// <o SEC_PHY_LE_AE_SCAN_WINDOW> Secondary PHY scan window (N * 0.625 ms)
// <0x0004-0x4000:1>
// <i> Default: 0x0050
#ifndef SEC_PHY_LE_AE_SCAN_WINDOW
#define SEC_PHY_LE_AE_SCAN_WINDOW 0x0050
#endif

// <q BLE_AE_SCAN_ENABLE_FILTER_DUP> Filter duplicate reports
// <i> Default: 0
#ifndef BLE_AE_SCAN_ENABLE_FILTER_DUP
#define BLE_AE_SCAN_ENABLE_FILTER_DUP 0x00
#endif

// <o BLE_AE_SCAN_DUR> Scan duration (0=infinite, N= N*10 ms)
// <0-65535:1>
// <i> Default: 0
#ifndef BLE_AE_SCAN_DUR
#define BLE_AE_SCAN_DUR 0x00
#endif

// <o BLE_AE_SCAN_PERIOD> Scan period (N * 1.28 s, 0=continuous)
// <0-65535:1>
// <i> Default: 0
#ifndef BLE_AE_SCAN_PERIOD
#define BLE_AE_SCAN_PERIOD 0x00
#endif

// </h>

// <h> Periodic advertising parameters
// <i> Intervals in N * 1.25 ms

// <o BLE_AE_PER_ADV_INT_MIN> Periodic adv interval min (N * 1.25 ms)
// <0x0060-0xFFFF:1>
// <i> Default: 0x90
#ifndef BLE_AE_PER_ADV_INT_MIN
#define BLE_AE_PER_ADV_INT_MIN 0x90
#endif

// <o BLE_AE_PER_ADV_INT_MAX> Periodic adv interval max (N * 1.25 ms)
// <0x0060-0xFFFF:1>
// <i> Default: 0x190
#ifndef BLE_AE_PER_ADV_INT_MAX
#define BLE_AE_PER_ADV_INT_MAX 0x190
#endif

// <o BLE_AE_PER_ADV_PROP> Periodic adv properties (bitmask)
// <0x0000-0xFFFF:1>
// <i> Default: 0x0040
#ifndef BLE_AE_PER_ADV_PROP
#define BLE_AE_PER_ADV_PROP 0x0040
#endif

#ifndef BLE_AE_PER_ADV_DATA_LEN
#define BLE_AE_PER_ADV_DATA_LEN 0x19
#endif
#ifndef BLE_AE_PER_ADV_DATA
#define BLE_AE_PER_ADV_DATA "SILABS_PER_ADV"
#endif
#ifndef BLE_AE_PER_ADV_DATA_OPERATION
#define BLE_AE_PER_ADV_DATA_OPERATION 0x03
#endif

#ifndef BLE_AE_PER_ADV_EN
#define BLE_AE_PER_ADV_EN 0x01
#endif

// </h>

// <h> Connection parameters (extended initiator)
// <i> Intervals in N * 1.25 ms, supervision timeout in N * 10 ms

// <o CONNECTION_INTERVAL_MIN> Connection interval min (N * 1.25 ms)
// <0x0006-0x0C80:1>
// <i> Default: 0x00A0
#ifndef CONNECTION_INTERVAL_MIN
#define CONNECTION_INTERVAL_MIN 0x00A0
#endif

// <o CONNECTION_INTERVAL_MAX> Connection interval max (N * 1.25 ms)
// <0x0006-0x0C80:1>
// <i> Default: 0x00A0
#ifndef CONNECTION_INTERVAL_MAX
#define CONNECTION_INTERVAL_MAX 0x00A0
#endif

// <o CONNECTION_LATENCY> Connection latency (slave latency)
// <0-499:1>
// <i> Default: 0
#ifndef CONNECTION_LATENCY
#define CONNECTION_LATENCY 0x0000
#endif

// <o SUPERVISION_TIMEOUT> Supervision timeout (N * 10 ms)
// <0x000A-0x0C80:1>
// <i> Default: 0x07D0
#ifndef SUPERVISION_TIMEOUT
#define SUPERVISION_TIMEOUT 0x07D0
#endif

// <o CONNECTION_EVENT_LEN_MIN> Connection event length min (N * 0.625 ms)
// <0x0000-0xFFFF:1>
// <i> Default: 0
#ifndef CONNECTION_EVENT_LEN_MIN
#define CONNECTION_EVENT_LEN_MIN 0x0000
#endif

// <o CONNECTION_EVENT_LEN_MAX> Connection event length max (N * 0.625 ms)
// <0x0000-0xFFFF:1>
// <i> Default: 0xFFFF
#ifndef CONNECTION_EVENT_LEN_MAX
#define CONNECTION_EVENT_LEN_MAX 0xFFFF
#endif

// </h>
// </h>
// <<< end of configuration section >>>

#endif // EXT_ADV_CONFIG_H
