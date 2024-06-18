/***************************************************************************/ /**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#include <stdint.h>
#pragma once

/** \addtogroup SI91X_LOAD_IMAGE_TYPES
  * @{ */
// Load Image types
/// Load NWP firmware
#define LOAD_NWP_FW '1'

/// Load default NWP firmware active low
#define LOAD_DEFAULT_NWP_FW_ACTIVE_LOW 0x71
/** @} */

/** \addtogroup SI91X_TLS_DEFINE
  * \ingroup SL_SI91X_CONSTANTS
  * @{ */
/*=========================================================================*/
// TLS feature parameters description !//
/*=========================================================================*/
/// Bit to enable SSL feature
#define SL_SI91X_ENABLE_TLS BIT(0)

/// Bitmap to enable TLS version 1.0
#define SL_SI91X_TLS_V_1_0 BIT(2)

/// Bitmap to enable TLS version 1.2
#define SL_SI91X_TLS_V_1_2 BIT(3)

/// Bitmap to enable TLS version 1.1
#define SL_SI91X_TLS_V_1_1 BIT(4)

#ifdef SLI_SI917
/// Bitmap to enable TLS version 1.3
#define SL_SI91X_TLS_V_1_3 BIT(8)
#endif
/** @} */

/** \addtogroup SI91X_HTTP_CLIENT_DEFINE
  * \ingroup SL_SI91X_CONSTANTS
  * @{ */
/*=========================================================================*/
// HTTP Client feature parameters description !//
/*=========================================================================*/
/// Bit to enable NULL delimiter for HTTP buffer instead of comma
#define SL_SI91X_ENABLE_NULL_DELIMETER BIT(1)

/// HTTP client post big data support feature bitmap
#define SL_SI91X_SUPPORT_HTTP_POST_DATA BIT(5)

/// HTTP version 1.1 support feature bitmap
#define SL_SI91X_HTTP_V_1_1 BIT(6)

/// Bit to enable user given content type in extended header
#define SL_SI91X_HTTP_USER_DEFINED_CONTENT_TYPE BIT(7)

/// To specify index of SSL cert to be used for HTTPS, for index 0 leave them unset
#define SL_SI91X_HTTPS_CERTIFICATE_INDEX_1 BIT(9)
/// To specify index of SSL cert to be used for HTTPS, for index 0 leave them unset
#define SL_SI91X_HTTPS_CERTIFICATE_INDEX_2 BIT(10)

/// To enable SNI
#define SL_SI91X_HTTPS_USE_SNI BIT(11)
/** @} */

// Upgrade images
#define BURN_NWP_FW 'B'

// Bootloader selects default NWP FW image number
#define SELECT_DEFAULT_NWP_FW_IMAGE_NUMBER '5'

#define RSI_PING_PONG_CHUNK_SIZE 4096
#define RSI_REG_READ             0xD1
#define RSI_REG_WRITE            0xD2
#define RSI_SEND_RPS_FILE        '2'
#define RSI_FWUP_SUCCESSFUL      'S'
#define RSI_EOF_REACHED          'E'
#define RSI_PONG_VALID           'O'
#define RSI_PING_VALID           'I'
#define RSI_PONG_WRITE           0xD4
#define RSI_PING_WRITE           0xD5
#define RSI_PONG_AVAIL           'O'
#define RSI_PING_AVAIL           'I'

#define RSI_PING_BUFFER_ADDR 0x418000
#define RSI_PONG_BUFFER_ADDR 0x419000

#define RSI_ACTIVE_LOW_INTR  0x2
#define RSI_ACTIVE_HIGH_INTR 0x0

#define RSI_OPERMODE_RESPONSE_WAIT_TIME      (1000) // Milliseconds
#define RSI_SEND_RAW_DATA_RESPONSE_WAIT_TIME (1000) // Milliseconds

#ifndef SL_WIFI_ALLOCATE_COMMAND_BUFFER_WAIT_TIME
#define SL_WIFI_ALLOCATE_COMMAND_BUFFER_WAIT_TIME 1000 // 1 second to wait for a command buffer
#endif

//STM 32 Init Sequence
#define SL_SI91X_INIT_CMD 0x005c4a12

// SPI transfer type (8 BIT/32 BIT)
#define RSI_MODE_8BIT  0
#define RSI_MODE_32BIT 1

// frame descriptor length
#define RSI_FRAME_DESC_LEN 16

// SPI interrupt register values
// Are these the same as other buses?
// buffer full indication register value from module
#ifndef RSI_BUFFER_FULL
#define RSI_BUFFER_FULL 0x01
#endif
// buffer empty indication register value from module
#define RSI_BUFFER_EMPTY 0x02
// RX packet pending register value from module
#define RSI_RX_PKT_PENDING 0x08
// Assertion Interrupt indication from module
#define RSI_ASSERT_INTR 0x80

//Bootloader defines
#define SL_SI91X_SAFE_UPGRADE_ADDR        0x55408
#define SL_SI91X_SAFE_UPGRADE             BIT(12)
#define SL_SI91X_FW_START_OF_FILE         BIT(0)
#define SL_SI91X_FW_END_OF_FILE           BIT(1)
#define SL_SI91X_MIN_CHUNK_SIZE           4096
#define SL_SI91X_LOOP_COUNT_UPGRADE_IMAGE 0xFFFF
#define SL_SI91X_RESET_LOOP_COUNTER(X)    X = 0;
#define SL_SI91X_WHILE_LOOP(X, Y)         while ((X++) < (int32_t)Y)
#define SL_SI91X_CHECK_LOOP_COUNTER(X, Y) \
  {                                       \
    if (X >= Y)                           \
      return -1;                          \
  }

//***************************** Macros for Crypto Start **********************************/

#define SL_SI91X_KEY_BUFFER_SIZE 32
#define SL_SI91X_TAG_SIZE        16
#define SL_SI91X_IV_SIZE         16

// ECDH defines
#define ECDH_BUFFER_SIZE 32

// TRNG defines
#define TRNG_INIT       1
#define TRNG_ENTROPY    2
#define TRNG_KEY        3
#define TRNG_GENERATION 4

#define TRNG_INIT_MSG_LENGTH 16
#define TRNG_KEY_SIZE        4
#define TRNG_TEST_DATA_SIZE  64

// CCM defines
#define SL_SI91X_CCM_IV_MIN_SIZE  7
#define SL_SI91X_CCM_IV_MAX_SIZE  13
#define SL_SI91X_CCM_IV_BUFF_LEN  16  // SL_SI91X_CCM_IV_MAX_SIZE + 3 Bytes for Padding.
#define SL_SI91X_CCM_AD_MAX_SIZE  128 // Limiting header length to 128.
#define SL_SI91X_CCM_MSG_MAX_SIZE 1200

// GCM defines
#define SL_SI91X_GCM_IV_SIZE      12
#define SL_SI91X_GCM_AD_MAX_SIZE  128
#define SL_SI91X_GCM_MSG_MAX_SIZE 1400

// Chachapoly defines
#define SL_SI91X_KEYR_SIZE               16
#define SL_SI91X_KEYS_SIZE               16
#define SL_SI91X_CHACHAPOLY_IV_SIZE      12
#define SL_SI91X_CHACHAPOLY_MSG_MAX_SIZE 1200

//ECDSA defines
#define SL_SI91X_PRIVATE_KEY_MAX_SIZE 64
#define SL_SI91X_PUBLIC_KEY_MAX_SIZE  136
#define SL_SI91X_SIGNATURE_MAX_SIZE   128
#define SL_SI91X_ECDSA_MSG_MAX_SIZE   1000

//***************************** Macros for Crypto End **********************************/

typedef struct {
  uint32_t buffer_full : 1;
  uint32_t ready_to_transmit : 1;
  uint32_t _reserved2 : 1;
  uint32_t rx_packet_pending : 1;
  uint32_t _reserved4 : 1;
  uint32_t _reserved5 : 1;
  uint32_t _reserved6 : 1;
  uint32_t _reserved7 : 1;
} sl_si91x_interrupt_status_t;

// Timeout used in get_channel API
#define SL_SI91X_GET_CHANNEL_TIMEOUT 30200

/** \addtogroup SL_SI91X_TYPES 
  * @{ */
/// Si91x specific keepalive types.
typedef enum {
  SL_SI91X_AP_KEEP_ALIVE_DISABLE = 0, ///< Disable keepalive
  SL_SI91X_AP_DEAUTH_BASED_KEEP_ALIVE =
    1, ///< AP performs keep alive functionality based on the RX packets received from its stations. If no packet is received from the station with in time out, AP discards it.
  SL_SI91X_AP_NULL_BASED_KEEP_ALIVE =
    3 ///< AP performs keep alive functionality by sending NULL DATA packet to the station. If no ACK is received from the station after specific no of retries, AP discards the station.
} sl_si91x_ap_keepalive_type_t;
/** @} */

typedef enum {
  SL_SI91X_RETURN_IMMEDIATELY              = 0,
  SL_SI91X_WAIT_FOR_EVER                   = 1,
  SL_SI91X_WAIT_FOR_RESPONSE_BIT           = (1 << 30),
  SL_SI91X_WAIT_FOR_SYNC_SCAN_RESULTS      = (SL_SI91X_WAIT_FOR_RESPONSE_BIT | 12000),
  SL_SI91X_WAIT_FOR_COMMAND_RESPONSE       = (SL_SI91X_WAIT_FOR_RESPONSE_BIT | 1000),
  SL_SI91X_WAIT_FOR_SOCKET_ACCEPT_RESPONSE = (SL_SI91X_WAIT_FOR_RESPONSE_BIT | 5000),
#ifdef SLI_SI91X_MCU_INTERFACE
  SL_SI91X_WAIT_FOR_COMMAND_SUCCESS = (3000),
#else
  SL_SI91X_WAIT_FOR_COMMAND_SUCCESS = (1000),
#endif
  SL_SI91X_WAIT_FOR_DNS_RESOLUTION = (20000),
} sl_si91x_wait_period_t;

#define SL_SI91X_WAIT_FOR(x)          (sl_si91x_wait_period_t)(x)
#define SL_SI91X_WAIT_FOR_RESPONSE(x) (sl_si91x_wait_period_t)(SL_SI91X_WAIT_FOR_RESPONSE_BIT | (x))

typedef enum {
  // (7+/-1)dBm in 2.4GHz band
  // (5+/-1)dBm in 5GHz band
  SL_SI91X_LOW_POWER_LEVEL,

  // (5+/-1)dBm in 2.4GHz band
  // (7+/-1)dBm in 5GHz band
  SL_SI91X_MEDIUM_POWER_LEVEL,

  SL_SI91X_HIGH_POWER_LEVEL
} sl_si91x_transmit_power_level;

/*====================================================*/
// Constant Defines
// SPI Status
#define RSI_SPI_SUCCESS 0x58
#define RSI_SPI_BUSY    0x54
#define RSI_SPI_FAIL    0x52
#define RSI_SUCCESS     0
//#define RSI_ERROR_SPI_BUSY  (-1)
#define RSI_ERROR_BUFFER_FULL -3 // module buffer full  error code
#define RSI_ERROR_IN_SLEEP    -4 // module in sleep error code

//SPI Internal Register Offset
#define RSI_SPI_INT_REG_ADDR    0x00 //@ register access method
#define RSI_SPI_MODE_REG_ADDR   0x08 //@ register access method
#define RSI_SPI_LENGTH_REG_ADDR 0x20

#define RSI_INT_MASK_REG_ADDR 0x41050000 // Interrupt mask register
#define RSI_INT_CLR_REG_ADDR  0x22000010 // Interrupt clear register

// Packet queue identifiers
#define RSI_COMMON_Q    0
#define RSI_ZB_Q        1
#define RSI_BT_Q        2
#define RSI_WLAN_MGMT_Q 4
#define RSI_WLAN_DATA_Q 5
#ifdef SAPIS_BT_STACK_ON_HOST
#define RSI_BT_INT_MGMT_Q 6
#define RSI_BT_HCI_Q      7
#endif

// Event IDs
#define RSI_RX_EVENT       0 // RX event number used in the driver
#define RSI_TX_EVENT       1 // TX event number used in the driver
#define RSI_SOCKET_EVENT   2 // Socket event number used in the driver
#define RSI_MAX_NUM_EVENTS 3 // Max number events used in the driver

#define RSI_HOST_INTF_REG_OUT    0x4105003C
#define RSI_HOST_INTF_REG_IN     0x41050034
#define RSI_HOST_INTF_STATUS_REG 0x41050004

// si91x boot results
#define RSI_EOF_REACHED                          'E'
#define RSI_BOOTUP_OPTIONS_LAST_CONFIG_NOT_SAVED 0xF1
#define RSI_BOOTUP_OPTIONS_CHECKSUM_FAIL         0xF2
#define RSI_INVALID_OPTION                       0xF3
#define RSI_CHECKSUM_SUCCESS                     0xAA
#define RSI_CHECKSUM_FAILURE                     0xCC
#define RSI_CHECKSUM_INVALID_ADDRESS             0x4C
#define VALID_FIRMWARE_NOT_PRESENT               0x23
#define RSI_BOOTLOADER_VERSION_1P0               0x10
#define RSI_BOOTLOADER_VERSION_1P1               0x11
#define RSI_ROM_VERSION_1P0                      1
#define RSI_ROM_VERSION_1P1                      2

#define SLI_WIFI_REGISTER_VALID (0xAB)

#define RSI_HOST_INTERACT_REG_VALID    (0xAB << 8)
#define RSI_HOST_INTERACT_REG_VALID_FW (0xA0 << 8)

#define CONFIG_RTSTHRESHOLD 1
#define RSI_RTS_THRESHOLD   2346

#define RSI_SEND_RAW_DATA    0x1
#define RSI_RECEIVE_RAW_DATA 0x0

// enumeration for command request used in common control block
typedef enum {
  // Common command requests
  RSI_COMMON_REQ_OPERMODE       = 0x10,
  RSI_COMMON_REQ_ANTENNA_SELECT = 0x1B,
  RSI_COMMON_REQ_FEATURE_FRAME  = 0xC8,
  RSI_COMMON_REQ_PWRMODE        = 0x15,
  // Reusing RSI_WLAN_REQ_FW_VERSION as RSI_COMMON_REQ_FW_VERSION
  RSI_COMMON_REQ_FW_VERSION     = 0x49,
  RSI_COMMON_REQ_GET_EFUSE_DATA = 0xA0,

  // Unimplemented common command requests
  RSI_COMMON_REQ_SOFT_RESET            = 0x1C,
  RSI_COMMON_REQ_ENCRYPT_CRYPTO        = 0x76,
  RSI_COMMON_REQ_UART_FLOW_CTRL_ENABLE = 0xA4,
  RSI_COMMON_REQ_TA_M4_COMMANDS        = 0xB0,
  RSI_COMMON_REQ_DEBUG_LOG             = 0x26
#ifdef RSI_WAC_MFI_ENABLE
  ,
  RSI_COMMON_REQ_IAP_GET_CERTIFICATE   = 0xB6,
  RSI_COMMON_REQ_IAP_INIT              = 0xB7,
  RSI_COMMON_REQ_IAP_GENERATE_SIGATURE = 0xB8
#endif

#ifdef RSI_PUF_ENABLE
  ,
  RSI_COMMON_REQ_PUF_ENROLL      = 0xD0,
  RSI_COMMON_REQ_PUF_DIS_ENROLL  = 0xD1,
  RSI_COMMON_REQ_PUF_START       = 0xD2,
  RSI_COMMON_REQ_PUF_SET_KEY     = 0xD3,
  RSI_COMMON_REQ_PUF_DIS_SET_KEY = 0xD4,
  RSI_COMMON_REQ_PUF_GET_KEY     = 0xD5,
  RSI_COMMON_REQ_PUF_DIS_GET_KEY = 0xD6,
  RSI_COMMON_REQ_PUF_LOAD_KEY    = 0xD7,
  RSI_COMMON_REQ_AES_ENCRYPT     = 0xD8,
  RSI_COMMON_REQ_AES_DECRYPT     = 0xD9,
  RSI_COMMON_REQ_AES_MAC         = 0xDA,
  RSI_COMMON_REQ_PUF_INTR_KEY    = 0xCE
#endif
  ,
  RSI_COMMON_REQ_SWITCH_PROTO  = 0x77,
  RSI_COMMON_REQ_GET_RAM_DUMP  = 0x92,
  RSI_COMMON_REQ_ASSERT        = 0xE1,
  RSI_COMMON_REQ_SET_RTC_TIMER = 0xE9,
  RSI_COMMON_REQ_GET_RTC_TIMER = 0xF2,
  RSI_COMMON_REQ_SET_CONFIG    = 0xBA
#ifdef CONFIGURE_GPIO_FROM_HOST
  ,
  RSI_COMMON_REQ_GPIO_CONFIG = 0x28
#endif
#ifdef FW_LOGGING_ENABLE
  ,
  RSI_COMMON_REQ_DEVICE_LOGGING_INIT = 0x82
#endif
} rsi_common_cmd_request_t;

typedef enum {
  // Common command responses
  RSI_COMMON_RSP_OPERMODE       = 0x10,
  RSI_COMMON_RSP_ANTENNA_SELECT = 0x1B,
  RSI_COMMON_RSP_FEATURE_FRAME  = 0xC8,
  RSI_COMMON_RSP_CARDREADY      = 0x89,
  RSI_COMMON_RSP_PWRMODE        = 0x15,

  // Unimplemented common command responses
  RSI_COMMON_RSP_CLEAR                 = 0x00,
  RSI_COMMON_RSP_SOFT_RESET            = 0x1C,
  RSI_COMMON_RSP_ULP_NO_RAM_RETENTION  = 0xCD,
  RSI_COMMON_RSP_ASYNCHRONOUS          = 0xFF,
  RSI_COMMON_RSP_ENCRYPT_CRYPTO        = 0x76,
  RSI_COMMON_RSP_UART_FLOW_CTRL_ENABLE = 0xA4,
  RSI_COMMON_RSP_TA_M4_COMMANDS        = 0xB0,
  RSI_COMMON_RSP_DEBUG_LOG             = 0x26
#ifdef RSI_PUF_ENABLE
  ,
  RSI_COMMON_RSP_PUF_ENROLL      = 0xD0,
  RSI_COMMON_RSP_PUF_DIS_ENROLL  = 0xD1,
  RSI_COMMON_RSP_PUF_START       = 0xD2,
  RSI_COMMON_RSP_PUF_SET_KEY     = 0xD3,
  RSI_COMMON_RSP_PUF_DIS_SET_KEY = 0xD4,
  RSI_COMMON_RSP_PUF_GET_KEY     = 0xD5,
  RSI_COMMON_RSP_PUF_DIS_GET_KEY = 0xD6,
  RSI_COMMON_RSP_PUF_LOAD_KEY    = 0xD7,
  RSI_COMMON_RSP_AES_ENCRYPT     = 0xD8,
  RSI_COMMON_RSP_AES_DECRYPT     = 0xD9,
  RSI_COMMON_RSP_AES_MAC         = 0xDA,
  RSI_COMMON_RSP_PUF_INTR_KEY    = 0xCE
#endif

#ifdef RSI_WAC_MFI_ENABLE
  ,
  RSI_COMMON_RSP_IAP_GET_CERTIFICATE   = 0xB6,
  RSI_COMMON_RSP_IAP_INIT              = 0xB7,
  RSI_COMMON_RSP_IAP_GENERATE_SIGATURE = 0xB8
#endif
  // Reusing RSI_WLAN_REQ_FW_VERSION as RSI_COMMON_REQ_FW_VERSION
  ,
  RSI_COMMON_RSP_GET_EFUSE_DATA = 0xA0,
  RSI_COMMON_RSP_FW_VERSION     = 0x49,
  RSI_COMMON_RSP_SWITCH_PROTO   = 0x77,
  RSI_COMMON_RSP_GET_RAM_DUMP   = 0x92,
  RSI_COMMON_RSP_ASSERT         = 0xE1,
  RSI_COMMON_RSP_SET_RTC_TIMER  = 0xE9,
  RSI_COMMON_RSP_GET_RTC_TIMER  = 0xF2
#ifdef CONFIGURE_GPIO_FROM_HOST
  ,
  RSI_COMMON_RSP_GPIO_CONFIG = 0x28
#endif
#ifdef FW_LOGGING_ENABLE
  ,
  RSI_COMMON_RSP_DEVICE_LOGGING_INIT = 0x82
#endif
} rsi_common_cmd_response_t;

// enumeration for WLAN command request codes
typedef enum {
  // Wi-Fi commands
  RSI_WLAN_REQ_CONFIG               = 0xBE,
  RSI_WLAN_REQ_BAND                 = 0x11,
  RSI_WLAN_REQ_INIT                 = 0x12,
  RSI_WLAN_REQ_SCAN                 = 0x13,
  RSI_WLAN_REQ_JOIN                 = 0x14,
  RSI_WLAN_REQ_SET_MAC_ADDRESS      = 0x17,
  RSI_WLAN_REQ_DISCONNECT           = 0x19,
  RSI_WLAN_REQ_AP_STOP              = 0xAE,
  RSI_WLAN_REQ_SET_REGION           = 0x1D,
  RSI_WLAN_REQ_QUERY_NETWORK_PARAMS = 0x18,
  RSI_WLAN_REQ_AP_CONFIGURATION     = 0x24,
  RSI_WLAN_REQ_EVM_OFFSET           = 0x36,
  RSI_WLAN_REQ_EVM_WRITE            = 0x37,
  RSI_WLAN_REQ_RSSI                 = 0x3A,
  RSI_WLAN_REQ_EAP_CONFIG           = 0x4C,
  RSI_WLAN_REQ_FW_VERSION           = 0x49,
  RSI_WLAN_REQ_MAC_ADDRESS          = 0x4A,
  RSI_WLAN_REQ_QUERY_GO_PARAMS      = 0x4E,
  RSI_WLAN_REQ_SET_CERTIFICATE      = 0x4D,
  RSI_WLAN_REQ_BG_SCAN              = 0x6A,
  RSI_WLAN_REQ_BEACON_STOP          = 0x63,
  RSI_WLAN_REQ_WPS_METHOD           = 0x72,
  RSI_WLAN_REQ_EFUSE_READ           = 0x73,
  RSI_WLAN_REQ_ROAM_PARAMS          = 0x7B,
  RSI_WLAN_REQ_RX_STATS             = 0xA2,
  RSI_WLAN_REQ_RADIO                = 0x81,
  RSI_WLAN_REQ_EXT_STATS            = 0x68,
  RSI_WLAN_REQ_TWT_AUTO_CONFIG      = 0x2E,
  RSI_WLAN_REQ_TWT_PARAMS           = 0x2F,
  SL_WIFI_REQ_RESCHEDULE_TWT        = 0x3F,
  RSI_WLAN_REQ_GAIN_TABLE           = 0x47,
  RSI_WLAN_REQ_TX_TEST_MODE         = 0x7C,
  RSI_WLAN_REQ_HOST_PSK             = 0xA5,
  RSI_WLAN_REQ_SET_REGION_AP        = 0xBD,
  RSI_WLAN_REQ_CALIB_WRITE          = 0xCA,
  RSI_WLAN_REQ_FILTER_BCAST_PACKETS = 0xC9,
  RSI_WLAN_REQ_CALIB_READ           = 0xCF,
  RSI_WLAN_REQ_FULL_FW_VERSION      = 0xE0,
  RSI_WLAN_REQ_HTTP_OTAF            = 0xF4,
  RSI_WLAN_REQ_11AX_PARAMS          = 0xFF,

  // Network commands
  RSI_WLAN_REQ_PING_PACKET           = 0x29,
  RSI_WLAN_REQ_IPCONFV4              = 0x41,
  RSI_WLAN_REQ_DNS_QUERY             = 0x44,
  RSI_WLAN_REQ_HTTP_CLIENT_GET       = 0x51,
  RSI_WLAN_REQ_HTTP_CLIENT_POST      = 0x52,
  RSI_WLAN_REQ_HTTP_CLIENT_PUT       = 0x53,
  RSI_WLAN_REQ_IPCONFV6              = 0x90,
  RSI_WLAN_REQ_MULTICAST             = 0xB1,
  RSI_WLAN_REQ_HTTP_ABORT            = 0xB3,
  RSI_WLAN_REQ_HTTP_CREDENTIALS      = 0xB4,
  RSI_WLAN_REQ_EMB_MQTT_CLIENT       = 0xCB,
  RSI_WLAN_REQ_SNTP_CLIENT           = 0xE4,
  RSI_WLAN_REQ_HTTP_CLIENT_POST_DATA = 0xEB,

  // Socket commands
  RSI_WLAN_REQ_SOCKET_CREATE    = 0x42,
  RSI_WLAN_REQ_SOCKET_CLOSE     = 0x43,
  RSI_WLAN_REQ_SOCKET_READ_DATA = 0x6B,
  RSI_WLAN_REQ_SOCKET_ACCEPT    = 0x6C,
  RSI_WLAN_REQ_SOCKET_CONFIG    = 0xA7,
  RSI_WLAN_REQ_SELECT_REQUEST   = 0x74,
  RSI_WLAN_REQ_SET_SNI_EMBEDDED = 0x6E,

  // Unimplemented commands
  RSI_WLAN_REQ_SET_SLEEP_TIMER      = 0x16,
  RSI_WLAN_REQ_CFG_SAVE             = 0x20,
  RSI_WLAN_REQ_AUTO_CONFIG_ENABLE   = 0x21,
  RSI_WLAN_REQ_GET_CFG              = 0x22,
  RSI_WLAN_REQ_USER_STORE_CONFIG    = 0x23,
  RSI_WLAN_REQ_SET_WEP_KEYS         = 0x25,
  RSI_WLAN_REQ_SET_PROFILE          = 0x31,
  RSI_WLAN_REQ_GET_PROFILE          = 0x32,
  RSI_WLAN_REQ_DELETE_PROFILE       = 0x33,
  RSI_WLAN_REQ_SET_MULTICAST_FILTER = 0x40,
  RSI_WLAN_REQ_CONNECTION_STATUS    = 0x48,
  RSI_WLAN_REQ_CONFIGURE_P2P        = 0x4B,
  RSI_WLAN_REQ_WIRELESS_FWUP        = 0x59,
  RSI_WLAN_REQ_HT_CAPABILITIES      = 0x6D,
  RSI_WLAN_REQ_REJOIN_PARAMS        = 0x6F,
  RSI_WLAN_REQ_WMM_PS               = 0x97,
  RSI_WLAN_REQ_FWUP                 = 0x99,
#ifdef RSI_WAC_MFI_ENABLE
  RSI_WLAN_REQ_ADD_MFI_IE = 0xB5,
#endif
#ifndef SLI_SI91X_MCU_INTERFACE
  RSI_WLAN_REQ_CERT_VALID = 0xBC,
#endif
  RSI_WLAN_REQ_FREQ_OFFSET                  = 0xF3,
  RSI_WLAN_REQ_DYNAMIC_POOL                 = 0xC7,
  RSI_WLAN_REQ_MDNSD                        = 0xDB,
  RSI_WLAN_REQ_GET_DPD_DATA                 = 0xDC,
  RSI_WLAN_REQ_FTP                          = 0xE2,
  RSI_WLAN_REQ_FTP_FILE_WRITE               = 0xE3,
  RSI_WLAN_REQ_SMTP_CLIENT                  = 0xE6,
  RSI_WLAN_REQ_OTA_FWUP                     = 0xEF,
  RSI_WLAN_REQ_WEBPAGE_LOAD                 = 0x50,
  RSI_WLAN_REQ_JSON_LOAD                    = 0x9c,
  RSI_WLAN_REQ_WEBPAGE_ERASE                = 0x9A,
  RSI_WLAN_REQ_JSON_OBJECT_ERASE            = 0x9B,
  RSI_WLAN_REQ_WEBPAGE_CLEAR_ALL            = 0x7F,
  RSI_WLAN_REQ_HOST_WEBPAGE_SEND            = 0x56,
  RSI_WLAN_REQ_GET_RANDOM                   = 0xF8,
  RSI_WLAN_REQ_POP3_CLIENT                  = 0xE7,
  RSI_WLAN_REQ_DHCP_USER_CLASS              = 0xEC,
  RSI_WLAN_REQ_TIMEOUT                      = 0xEA,
  RSI_WLAN_REQ_GET_STATS                    = 0xF1,
  RSI_WLAN_REQ_UPDATE_TCP_WINDOW            = 0xF5,
  RSI_WLAN_REQ_DNS_UPDATE                   = 0xED,
  RSI_WLAN_REQ_DNS_SERVER_ADD               = 0x55,
  RSI_WLAN_REQ_TSF                          = 0x65,
  RSI_WLAN_REQ_SET_TRANSCEIVER_CHANNEL      = 0x7A,
  RSI_WLAN_REQ_TRANSCEIVER_PEER_LIST_UPDATE = 0x8B,
  RSI_WLAN_REQ_TRANSCEIVER_CONFIG_PARAMS    = 0x8C,
  RSI_WLAN_REQ_SET_TRANSCEIVER_MCAST_FILTER = 0x8D,
  RSI_WLAN_REQ_TRANSCEIVER_FLUSH_DATA_Q     = 0x8E,
} rsi_wlan_cmd_request_t;

// enumeration for WLAN command response codes
typedef enum {
  // Wi-Fi command response
  RSI_WLAN_RSP_BAND                 = 0x11,
  RSI_WLAN_RSP_INIT                 = 0x12,
  RSI_WLAN_RSP_SCAN                 = 0x13,
  RSI_WLAN_RSP_JOIN                 = 0x14,
  RSI_WLAN_RSP_SET_MAC_ADDRESS      = 0x17,
  RSI_WLAN_RSP_QUERY_NETWORK_PARAMS = 0x18,
  RSI_WLAN_RSP_DISCONNECT           = 0x19,
  RSI_WLAN_RSP_AP_STOP              = 0xAE,
  RSI_WLAN_RSP_SET_REGION           = 0x1D,
  RSI_WLAN_RSP_AP_CONFIGURATION     = 0x24,
  RSI_WLAN_RSP_TWT_AUTO_CONFIG      = 0x2E,
  RSI_WLAN_RSP_TWT_PARAMS           = 0x2F,
  SL_WIFI_RSP_RESCHEDULE_TWT        = 0x3F,
  RSI_WLAN_RSP_EVM_OFFSET           = 0x36,
  RSI_WLAN_RSP_EVM_WRITE            = 0x37,
  RSI_WLAN_RSP_RSSI                 = 0x3A,
  RSI_WLAN_RSP_GAIN_TABLE           = 0x47,
  RSI_WLAN_RSP_FW_VERSION           = 0x49,
  RSI_WLAN_RSP_MAC_ADDRESS          = 0x4A,
  RSI_WLAN_RSP_EAP_CONFIG           = 0x4C,
  RSI_WLAN_RSP_SET_CERTIFICATE      = 0x4D,
  RSI_WLAN_RSP_QUERY_GO_PARAMS      = 0x4E,
  RSI_WLAN_RSP_BEACON_STOP          = 0x63,
  RSI_WLAN_RSP_BG_SCAN              = 0x6A,
  RSI_WLAN_RSP_EXT_STATS            = 0x68, // Neither part 22q2 nor alpha 2
  RSI_WLAN_RSP_EFUSE_READ           = 0x73,
  RSI_WLAN_RSP_TX_TEST_MODE         = 0x7C,
  RSI_WLAN_RSP_ROAM_PARAMS          = 0x7B,
  RSI_WLAN_RSP_RADIO                = 0x81,
  RSI_WLAN_RSP_RX_STATS             = 0xA2,
  RSI_WLAN_RSP_HOST_PSK             = 0xA5,
  RSI_WLAN_RSP_SCAN_RESULTS         = 0xAF,
  RSI_WLAN_RSP_CONFIG               = 0XBE,
  RSI_WLAN_RSP_SET_REGION_AP        = 0xBD,
  RSI_WLAN_RSP_FILTER_BCAST_PACKETS = 0xC9,
  RSI_WLAN_RSP_CALIB_READ           = 0xCF,
  RSI_WLAN_RSP_FULL_FW_VERSION      = 0xE0,
  RSI_WLAN_RSP_GET_STATS            = 0xF1,
  RSI_WLAN_RSP_HTTP_OTAF            = 0xF4,
  RSI_WLAN_RSP_11AX_PARAMS          = 0xFF,

  // Network command response
  RSI_WLAN_RSP_PING_PACKET           = 0x29,
  RSI_WLAN_RSP_IPCONFV4              = 0x41,
  RSI_WLAN_RSP_DNS_QUERY             = 0x44,
  RSI_WLAN_RSP_HTTP_CLIENT_GET       = 0x51,
  RSI_WLAN_RSP_HTTP_CLIENT_POST      = 0x52,
  RSI_WLAN_RSP_HTTP_CLIENT_PUT       = 0x53,
  RSI_WLAN_RSP_IPCONFV6              = 0xA1,
  RSI_WLAN_RSP_MULTICAST             = 0xB1,
  RSI_WLAN_RSP_HTTP_ABORT            = 0xB3,
  RSI_WLAN_RSP_HTTP_CREDENTIALS      = 0xB4,
  RSI_WLAN_RSP_EMB_MQTT_CLIENT       = 0xCB,
  RSI_WLAN_RSP_SNTP_CLIENT           = 0xE4,
  RSI_WLAN_RSP_HTTP_CLIENT_POST_DATA = 0xEB,

  // Socket command response
  RSI_WLAN_RSP_SOCKET_CREATE    = 0x42,
  RSI_WLAN_RSP_SOCKET_CLOSE     = 0x43,
  RSI_WLAN_RSP_SOCKET_READ_DATA = 0x6B,
  RSI_WLAN_RSP_SOCKET_ACCEPT    = 0x6C,
  RSI_WLAN_RSP_SOCKET_CONFIG    = 0xA7,
  RSI_WLAN_RSP_SELECT_REQUEST   = 0x74,
  RSI_WLAN_RSP_SET_SNI_EMBEDDED = 0x6E,

  //  Unimplemented command for 22q2
  RSI_WLAN_RSP_CLEAR                  = 0x00,
  RSI_WLAN_RSP_CFG_SAVE               = 0x20,
  RSI_WLAN_RSP_AUTO_CONFIG_ENABLE     = 0x21,
  RSI_WLAN_RSP_GET_CFG                = 0x22,
  RSI_WLAN_RSP_USER_STORE_CONFIG      = 0x23,
  RSI_WLAN_RSP_SET_WEP_KEYS           = 0x25,
  RSI_WLAN_RSP_P2P_CONNECTION_REQUEST = 0x30,
  RSI_WLAN_RSP_SET_PROFILE            = 0x31,
  RSI_WLAN_RSP_GET_PROFILE            = 0x32,
  RSI_WLAN_RSP_DELETE_PROFILE         = 0x33,
  RSI_WLAN_RSP_CONN_ESTABLISH         = 0x61,
  RSI_WLAN_RSP_REMOTE_TERMINATE       = 0x62,
  RSI_WLAN_RSP_IPV4_CHANGE            = 0xAA,
  RSI_WLAN_RSP_TCP_ACK_INDICATION     = 0xAB,
  RSI_WLAN_RSP_UART_DATA_ACK          = 0xAC,
  RSI_WLAN_RSP_SET_MULTICAST_FILTER   = 0x40,
  RSI_WLAN_RSP_DNS_UPDATE             = 0xED,
  RSI_WLAN_RSP_CONNECTION_STATUS      = 0x48,
  RSI_WLAN_RSP_CONFIGURE_P2P          = 0x4B,
  RSI_WLAN_RSP_WFD_DEVICE             = 0x54,
  RSI_WLAN_RSP_DNS_SERVER_ADD         = 0x55,
  RSI_WLAN_RSP_WIRELESS_FWUP_OK       = 0x59,
  RSI_WLAN_RSP_WIRELESS_FWUP_DONE     = 0x5A,
  RSI_WLAN_RSP_HT_CAPABILITIES        = 0x6D,
  RSI_WLAN_RSP_REJOIN_PARAMS          = 0x6F,
  RSI_WLAN_RSP_WPS_METHOD             = 0x72,
  RSI_WLAN_RSP_WMM_PS                 = 0x97,
  RSI_WLAN_RSP_FWUP                   = 0x99,
#ifdef RSI_WAC_MFI_ENABLE
  RSI_WLAN_RSP_ADD_MFI_IE = 0xB5,
#endif
#ifndef SLI_SI91X_MCU_INTERFACE
  RSI_WLAN_RSP_CERT_VALID = 0xBC,
#endif
  RSI_WLAN_RSP_CLIENT_CONNECTED             = 0xC2,
  RSI_WLAN_RSP_CLIENT_DISCONNECTED          = 0xC3,
  RSI_WLAN_RSP_FREQ_OFFSET                  = 0xF3,
  RSI_WLAN_RSP_CALIB_WRITE                  = 0xCA,
  RSI_WLAN_RSP_DYNAMIC_POOL                 = 0xC7,
  RSI_WLAN_RSP_EMB_MQTT_PUBLISH_PKT         = 0xCC,
  RSI_WLAN_RSP_MQTT_REMOTE_TERMINATE        = 0xF0,
  RSI_WLAN_RSP_MDNSD                        = 0xDB,
  RSI_WLAN_RSP_GET_DPD_DATA                 = 0xDC,
  RSI_WLAN_RSP_FTP                          = 0xE2,
  RSI_WLAN_RSP_FTP_FILE_WRITE               = 0xE3,
  RSI_WLAN_RSP_SNTP_SERVER                  = 0xE5,
  RSI_WLAN_RSP_SMTP_CLIENT                  = 0xE6,
  RSI_WLAN_RSP_OTA_FWUP                     = 0xEF,
  RSI_WLAN_RSP_WEBPAGE_LOAD                 = 0x50,
  RSI_WLAN_RSP_JSON_LOAD                    = 0x9c,
  RSI_WLAN_RSP_WEBPAGE_ERASE                = 0x9A,
  RSI_WLAN_RSP_JSON_OBJECT_ERASE            = 0x9B,
  RSI_WLAN_RSP_WEBPAGE_CLEAR_ALL            = 0x7F,
  RSI_WLAN_RSP_HOST_WEBPAGE_SEND            = 0x56,
  RSI_WLAN_RSP_JSON_UPDATE                  = 0x9D,
  RSI_WLAN_RSP_GET_RANDOM                   = 0xF8,
  RSI_WLAN_RSP_ASYNCHRONOUS                 = 0xFF,
  RSI_WLAN_RSP_JSON_EVENT                   = 0xEE,
  RSI_WLAN_RSP_POP3_CLIENT                  = 0xE7,
  RSI_WLAN_RSP_POP3_CLIENT_TERMINATE        = 0xE8,
  RSI_WLAN_RSP_DHCP_USER_CLASS              = 0xEC,
  RSI_WLAN_RSP_TIMEOUT                      = 0xEA,
  RSI_WLAN_RSP_URL_REQUEST                  = 0x64,
  RSI_WLAN_RSP_MODULE_STATE                 = 0x70,
  RSI_WLAN_RSP_TWT_ASYNC                    = 0x71,
  RSI_WLAN_RSP_UPDATE_TCP_WINDOW            = 0xF5,
  RSI_WLAN_RSP_TSF                          = 0x65,
  RSI_WLAN_RSP_TRANSCEIVER_SET_CHANNEL      = 0x7A,
  RSI_WLAN_RSP_TRANSCEIVER_PEER_LIST_UPDATE = 0x8B,
  RSI_WLAN_RSP_TRANSCEIVER_CONFIG_PARAMS    = 0x8C,
  RSI_WLAN_RSP_TRANSCEIVER_SET_MCAST_FILTER = 0x8D,
  RSI_WLAN_RSP_TRANSCEIVER_FLUSH_DATA_Q     = 0x8E,
  RSI_WLAN_RSP_TRANSCEIVER_TX_DATA_STATUS   = 0x3D,
  SL_SI91X_WIFI_RX_DOT11_DATA               = 0x03,
  // Unimplemented commands after 22Q2
  RSI_WLAN_RATE_RSP_STATS = 0x88
} rsi_wlan_cmd_response_t;

typedef enum { SET_REGION_CODE_FROM_BEACONS, SET_REGION_CODE_FROM_USER } si91x_set_region_code_command_t;

typedef enum { SL_SI91X_SOCKET_REMOTE_TERMINATED_EVENT, SL_SI91X_SOCKET_EVENT_COUNT } sl_si91x_socket_event_t;

typedef enum { SL_SI91X_NO_ENCRYPTION, SL_SI91X_TKIP_ENCRYPTION, SL_SI91X_CCMP_ENCRYPTION } sl_si91x_encryption_t;
