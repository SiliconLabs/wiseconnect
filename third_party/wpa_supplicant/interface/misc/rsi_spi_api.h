/*******************************************************************************
* @file  rsi_spi_api.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
 * @file
 * @version		2.0.0.0
 * @date 		2011-May-30
 *
 *
 *
 * @brief HEADER, SPI, specific API defines
 *
 * @section Description
 * This file contains the function prototypes of the api's defined in spi library. 
 *
 */

#ifndef _SPIAPI_H_
#define _SPIAPI_H_

/**
 * Include Files
 */
#include "rsi_global.h"

/**
 * Global Defines
 */

/*=========================================================*/
// C1 Register Bit Field Defines
#define RSI_C1_INIT_CMD 0x15
// sent to spi interface after reset/powerup to init the spi interface

#define RSI_C1_INIT_RESP      0x55
#define RSI_REQ_GET_DEVICE_ID 0xBB
// response from spi interface to successful init

#define RSI_C176_INIT      0b00111111 // and
#define RSI_C176_RW        0b01000000 // or
#define RSI_C15_RD         0b11011111 // and
#define RSI_C15_WR         0b00100000 // or
#define RSI_C14_SPISLV     0b11101111 // and
#define RSI_C14_AHBBUS     0b00010000 // or
#define RSI_C13_AHBMEM     0b11110111 // and
#define RSI_C13_AHBFRM     0b00001000 // or
#define RSI_C12_02BITXFR   0b11111011 // and
#define RSI_C12_16BITXFR   0b00000100 // or
#define RSI_C110_4BYTESLEN 0b11111100
// and	number of C bytes transferred, usually 4
#define RSI_C110_1BYTESLEN 0b00000001
// or/~and number of C bytes transferred
#define RSI_C110_2BYTESLEN 0b00000010
// or/~and number of C bytes transferred
#define RSI_C110_3BYTESLEN 0b00000011
// or number of C bytes transferred

/*=========================================================*/
// C2 Register Defines
#define RSI_C276_08BIT      0b00111111 // and
#define RSI_C276_32BIT      0b01000000 // or
#define RSI_C250_SPIADDR    0b00111111 // and
#define RSI_C250_DATAPACKET 0b00000010 // or
#define RSI_C250_MGMTPACKET 0b00000100 // or

//#define RSI_C3_XFERLENLSB				0x00				// set value
//#define RSI_C4_XFERLENMSB				0x00				// set value

/*==========================================================*/
// C1 Register Defines
// Internal Read
#define RSI_C1INTREAD2BYTES 0x42
// (((((0x00 & C176_INT) & C15_RD) & C14_SPISLV) & C13_AHBMEM) & C12_02BITXFR) | C110_2BYTESLEN	// 01000010
#define RSI_C1INTREAD1BYTES 0x41

#define RSI_C1INTWRITE2BYTES 0x62
#define RSI_C1INTWRITE1BYTES 0x61

// Memory ReadWrite (AHB Master Read/Write, Internal Legacy Name)
// Memory read/write is normally done using 16-bit transfer length with 4 C bytes transferred
#define RSI_C1MEMWR16BIT1BYTE 0x75
// (((0x00 | C176_RW | C15_WR | C14_AHBBUS) & C13_AHBMEM) & C12_16BITXFR) | C110_1BYTESLEN // 01110101
#define RSI_C1MEMRD16BIT1BYTE 0x55
// (((0x00 | C176_RW & C15_RD | C14_AHBBUS) & C13_AHBMEM) & C12_16BITXFR) | C110_1BYTESLEN // 01010101

#define RSI_C1MEMWR16BIT4BYTE 0x74
// (((0x00 | C176_RW | C15_WR | C14_AHBBUS) & C13_AHBMEM) & C12_16BITXFR) & C110_4BYTESLEN // 01110100
#define RSI_C1MEMRD16BIT4BYTE 0x54
// ((((0x00 | C176_RW & C15_RD) | C14_AHBBUS) & C13_AHBMEM) & C12_16BITXFR) & C110_4BYTESLEN // 01010100

// Normally, 2-bit transfer length is not used for memory read/write
#define RSI_C1MEMWR02BIT1BYTE 0x71
// (((0x00 | C176_RW | C15_WR | C14_AHBBUS) & C13_AHBMEM) & C12_02BITXFR) | C110_1BYTESLEN // 01110001
#define RSI_C1MEMRD02BIT1BYTE 0x51
// (((0x00 | C176_RW & C15_RD | C14_AHBBUS) & C13_AHBMEM) & C12_02BITXFR) | C110_1BYTESLEN // 01010001
#define RSI_C1MEMWR02BIT4BYTE 0x70
// (((0x00 | C176_RW | C15_WR | C14_AHBBUS) & C13_AHBMEM) & C12_02BITXFR) & C110_4BYTESLEN // 01110000
#define RSI_C1MEMRD02BIT4BYTE 0x50
// ((((0x00 | C176_RW & C15_RD) | C14_AHBBUS) & C13_AHBMEM) & C12_02BITXFR) & C110_4BYTESLEN // 01010000

// Frame ReadWrite
// Frame read/writes normally  use 16-bit transfer length
#define RSI_C1FRMWR16BIT1BYTE 0x7d
// ((C176_RW | C15_WR | C14_AHBBUS | C13_AHBFRM) & C12_16BITXFR) | C110_1BYTESLEN // 01111101
#define RSI_C1FRMRD16BIT1BYTE 0x5d
// ((C176_RW & C15_RD | C14_AHBBUS | C13_AHBFRM) & C12_16BITXFR) | C110_1BYTESLEN // 01011101

#define RSI_C1FRMWR16BIT4BYTE 0x7c
// ((C176_RW | C15_WR | C14_AHBBUS | C13_AHBFRM) & C12_16BITXFR) | C110_4BYTESLEN // 01111100
#define RSI_C1FRMRD16BIT4BYTE 0x5c
// ((C176_RW & C15_RD | C14_AHBBUS | C13_AHBFRM) & C12_16BITXFR) | C110_4BYTESLEN // 01011100

// Frame read/writes normally do not use 2-bit transfer length
#define RSI_C1FRMWR02BIT1BYTE 0x79
// ((C176_RW | C15_WR | C14_AHBBUS | C13_AHBFRM) & C12_02BITXFR) | C110_1BYTESLEN // 01111001
#define RSI_C1FRMRD02BIT1BYTE 0x59
// ((C176_RW & C15_RD | C14_AHBBUS | C13_AHBFRM) & C12_02BITXFR) | C110_1BYTESLEN // 01011001

#define RSI_C1FRMWR02BIT4BYTE 0x78
// ((C176_RW | C15_WR | C14_AHBBUS | C13_AHBFRM) & C12_02BITXFR) | C110_4BYTESLEN // 01111000
#define RSI_C1FRMRD02BIT4BYTE 0x58
// ((C176_RW & C15_RD | C14_AHBBUS | C13_AHBFRM) & C12_02BITXFR) | C110_4BYTESLEN // 01011000

// SPI Register ReadWrite
#define RSI_C1SPIREGWR16BIT4BYTE 0x64
// ((((C176_RW | C15_WR) & C14_SPISLV) & C13_AHBMEM) & C12_16BITXFR) | C110_4BYTESLEN // 01100100
#define RSI_C1SPIREGRD16BIT4BYTE 0x44
// ((((C176_RW & C15_RD) & C14_SPISLV) & C13_AHBMEM) & C12_16BITXFR) | C110_4BYTESLEN // 01000100

#define RSI_C1SPIREGWR02BIT4BYTE 0x60
// ((((C176_RW | C15_WR) & C14_SPISLV) & C13_AHBMEM) & C12_02BITXFR) | C110_4BYTESLEN // 01100000
#define RSI_C1SPIREGRD02BIT4BYTE 0x40
// ((((C176_RW & C15_RD) & C14_SPISLV) & C13_AHBMEM) & C12_02BITXFR) | C110_4BYTESLEN // 01000000

#define RSI_C1SPIREGWR02BIT1BYTE 0x61
// ((((C176_RW | C15_WR) & C14_SPISLV) & C13_AHBMEM) & C12_02BITXFR) | C110_1BYTESLEN // 01100001
#define RSI_C1SPIREGRD02BIT1BYTE 0x41
// ((((C176_RW & C15_RD) & C14_SPISLV) & C13_AHBMEM) & C12_02BITXFR) | C110_1BYTESLEN // 01000001

// C2 Register Defines
#define RSI_C2RDWR4BYTE 0x40 // 0x00 | C276_32BIT | C250_DATAPACKET															// 01000010
#define RSI_C2RDWR4BYTE 0x40 // 0x00 | C276_32BIT | C250_MGMTPACKET															// 01000100

#define RSI_C2RDWR1BYTE 0x00 // (0x00 & C276_08BIT) | C250_DATAPACKET														// 00000010
#define RSI_C2RDWR1BYTE 0x00 // (0x00 & C276_08BIT) | C250_MGMTPACKET
// 00000100
#define RSI_C2MGMT 0x00
#define RSI_C2DATA 0x02

#define RSI_C2SPIADDR1BYTE  0x00 // (0x00 & C276_08BIT) | C250_SPIADDR															// 00xxxxxx
#define RSI_C2MEMRDWRNOCARE 0x00 // 0x00 or ANYTHING																	// 00000000
#define RSI_C2SPIADDR4BYTE  0x40 // (0x00 | C276_32BIT) | C250_SPIADDR															// 01xxxxxx

/*====================================================*/
// Constant Defines
// Sizes

// SPI Status
#define RSI_SPI_SUCCESS 0x58
#define RSI_SPI_BUSY    0x54
#define RSI_SPI_FAIL    0x52

#define RSI_SUCCESS     0
#define RSI_BUSY        -1
#define RSI_FAIL        -2
#define RSI_BUFFER_FULL -3
#define RSI_IN_SLEEP    -4

// SPI Internal Register Offset
#define RSI_SPI_INT_REG_ADDR  0x00 // register access method
#define RSI_SPI_MODE_REG_ADDR 0x08 // register access method
//#define RSI_SPI_INT_REG_ADDR		 	        0x08000000	// memory access method
//#define RSI_SPI_MODE_REG_ADDR			        0x08000008	// memory access method

// SPI Mode Register
#define RSI_SPI_MODE_LOW  0x00
#define RSI_SPI_MODE_HIGH 0x01

// Power Mode Constants
#define RSI_POWER_MODE_0 0x0000
#define RSI_POWER_MODE_1 0x0001
#define RSI_POWER_MODE_2 0x0002

//Tx Power level
#define RSI_POWER_LEVEL_LOW    0x0000
#define RSI_POWER_LEVEL_MEDIUM 0x0001
#define RSI_POWER_LEVEL_HIGH   0x0002

// SPI Constants
#define RSI_SPI_START_TOKEN 0x55 // SPI Start Token

// Interrupt Defines
#define RSI_INT_REG_ADDR        0x08000000
#define RSI_INT_MSKCLR_REG_ADDR 0x22000000
// base address of the interrupt mask/clear register
#define RSI_INT_MASK_REG_ADDR 0x22000008 // Interrupt mask register
#define RSI_INT_CLR_REG_ADDR  0x22000010 // Interrupt clear register
#define RSI_INT_MASK_OFFSET   0x08       // offset from base address of interrupt mask/clear register
#define RSI_INT_CLR_OFFSET    0x10       // offset from base address of interrupt mask/clear register
#define RSI_IMASK_PWR_ENABLE  0xf3       // mask power interrupt
#define RSI_IMASK_PWR_DISABLE 0xd3       // clear power interrupt mask
#define RSI_ICLEAR_PWR        0x20
// bit5=1 to clear the interrupt, must use read-modify-write to preserver the other bits

// Software Bootloader Addresses
#define RSI_SBINST1_ADDR 0x00000000
#define RSI_SBINST2_ADDR 0x02014010
#define RSI_SBDATA1_ADDR 0x20003100
#define RSI_SBDATA2_ADDR 0x02010000

// Image Upgrader Addresses
#define RSI_IUINST1_ADDR 0x00000000
#define RSI_IUINST2_ADDR 0x02000000
#define RSI_IUDATA_ADDR  0x20003100

#define RSI_FFTAIM1_ADDR 0x02008000
#define RSI_FFTAIM2_ADDR 0x02008000
#define RSI_FFTADM_ADDR  0x02008000

// Soft Reset Defines
#define RSI_RST_SOFT_ADDR 0x22000004
#define RSI_RST_SOFT_SET  0x00000001
#define RSI_RST_SOFT_CLR  0x00000000

#define RSI_HOST_INT_CLR 0x22000010

#define RSI_SOCKET_CREATION_FAILED -121
/*======================================================*/
// Frame Defines
// Management Frames (AHB Slave Read/Write)
// Management Request Frame Codes (Type)
#ifdef EVK_DEMO
#define RSI_REQ_SCAN_LIST 0x9F
#endif
#define RSI_REQ_SLEEP_TIMER 0x16
#define RSI_REQ_SOFT_RESET  0x1C
#define RSI_REQ_DBG_LVL     0x27
#define RSI_REQ_GPIO_CONF   0x28

#ifdef FW_UP_FROM_HOST
#define RSI_REQ_FWUP_FRM_HOST 0x99
#endif

#define RSI_REQ_READ_USER_CONFIGURABLE_EFUSE_DATA 0x73
#define RSI_REQ_CERT_VALID                        0xBC

#define RSI_REQ_UART_FLOW_CONTROL_ENABLE 0xA4
#define RSI_REQ_SWITCH_PROTO             0x77
#define E_EVT_MODULE_TYPE_GET            0xBF
#define RSI_REQ_FW_LOG_PKT               0x82

#ifdef PUF_ENABLE
#define RSI_REQ_PUF_ENROLL          0xD0
#define RSI_REQ_PUF_DISABLE_ENROLL  0xD1
#define RSI_REQ_PUF_START           0xD2
#define RSI_REQ_PUF_SET_KEY         0xD3
#define RSI_REQ_PUF_DISABLE_SET_KEY 0xD4
#define RSI_REQ_PUF_GET_KEY         0xD5
#define RSI_REQ_PUF_DISABLE_GET_KEY 0xD6
#define RSI_REQ_PUF_LOAD_KEY        0xD7
#define RSI_REQ_AES_ENCRYPT         0xD8
#define RSI_REQ_AES_DECRYPT         0xD9
#define RSI_REQ_AES_MAC             0xDA
#endif
#ifdef PUF_INTRINSIC
#define RSI_REQ_PUF_INTR_KEY 0xCE
#endif

#define RSI_REQ_GET_RTC_DATE_AND_TIME 0xF2
#define RSI_REQ_GET_CSI_DATA          0xC4

#ifdef OBE_LOW_POWER_DYNAMIC_API
#define RSI_REQ_DYNAMIC_POOL 0xC7
#endif

#define RSI_REQ_TA_M4_COMMANDS 0xB0

//! Loading of Spectral mask configuration
#ifdef SPECTRAL_MASK
//! Vinay correct this
#define RSI_REQ_LOAD_SPECTRAL_MASK_CONFIG 0xB9
//#define RSI_REQ_LOAD_SPECTRAL_MASK_CONFIG    0xF7
#endif

#ifdef FIPS_CODE_ENABLE
#define RSI_REQ_FIPS_GET_FWUPGRADE_KEY 0xF6
#define RSI_REQ_FIPS_AUTO_JOIN_KEY     0xF7
#define RSI_REQ_FIPS_GENERATE_PMK      0xF8
#define RSI_REQ_GET_CFG_CHKSM          0xFC
#define RSI_REQ_FIPS_KEY_ZEROIZATION   0xFD
#define RSI_REQ_FIPS_MODE              0xFE
#endif
#ifdef FIPS_CRYPTO_CERTIFICATION
#define RSI_REQ_CRYPTO_TEST 0xFF
#endif
#define RSI_REQ_CRYPTO_HARDWARE 0x76
#ifdef CHIP_9117
#define RSI_REQ_ECDSA_256_VERIFY_HASH 0x3E
#endif
#ifdef RSI_ASSERT_API
#define RSI_REQ_ASSERT 0xE1
#endif
#define RSI_REQ_CALIB_WRITE 0xCA
#define RSI_REQ_EVM_WRITE   0x37
#define RSI_REQ_CALIB_READ  0xCF

/* Configuration types */
#define CONFIG_RTSTHRESHOLD 1

// Management Response Frame Codes
#define RSI_RSP_CARD_READY 0x89
#ifdef EVK_DEMO
#define RSI_RSP_SCAN_LIST 0x9F
#endif
#define RSI_RSP_PWRMODE              0x15
#define RSI_RSP_SLEEP_TIMER          0x16
#define RSI_RSP_SOFT_RESET           0x1C
#define RSI_RSP_ULP_NO_RAM_RETENTION 0xCD
#define RSI_RSP_CFG_ENABLE           0x21
#define RSI_RSP_CFG_GET              0x22
#define LOAD_FT_ACTION_IE            0x7F

#define RSI_RSP_DEBUG_DUMP  0x26
#define RSI_RSP_DBG_LVL     0x27
#define RSI_RSP_GPIO_CONF   0x28
#define RSI_RSP_P2P_CONNREQ 0x30
#define RSI_RSP_FW_LOG_PKT  0x82

#ifdef ENABLE_DRAEGER_CUSTOMIZATION
#define RSI_RSP_MODULE_STATE 0x70
#endif

#define RSI_RSP_CERT_VALID    0xBC
#define RSI_FWUP_REQ          0x59
#define RSI_FWUP_OK           0x5a
#define RSI_WAKEUP            0xDD
#define RSI_REQ_SLEEP_ACK     0xDE
#define RSI_RSP_STORE_IP_PORT 0xA9

#define RSI_RSP_READ_USER_CONFIGURABLE_EFUSE_DATA 0x73
#define RSI_RSP_START_AUTO_CONFIG                 0x91
#define RSI_RSP_GET_RAM_DUMP                      0x92

#define RSI_RSP_UART_FLOW_CONTROL_ENABLE 0xA4
#define RSI_RSP_SWITCH_PROTO             0x77
#define RSI_REQ_MODULE_TYPE_GET          0xBF
#define RSI_RSP_ZB_BT_PACKET_RECEIVED    0xA5

#define RSI_RSP_GET_CSI_DATA 0xC4

#ifdef PUF_ENABLE
#define RSI_RSP_PUF_ENROLL          0xD0
#define RSI_RSP_PUF_DISABLE_ENROLL  0xD1
#define RSI_RSP_PUF_START           0xD2
#define RSI_RSP_PUF_SET_KEY         0xD3
#define RSI_RSP_PUF_DISABLE_SET_KEY 0xD4
#define RSI_RSP_PUF_GET_KEY         0xD5
#define RSI_RSP_PUF_DISABLE_GET_KEY 0xD6
#define RSI_RSP_PUF_LOAD_KEY        0xD7
#define RSI_RSP_AES_ENCRYPT         0xD8
#define RSI_RSP_AES_DECRYPT         0xD9
#define RSI_RSP_AES_MAC             0xDA
#endif
#ifdef OBE_LOW_POWER_DYNAMIC_API
#define RSI_RSP_DYNAMIC_POOL 0xC7
#endif
#ifdef PUF_INTRINSIC
#define RSI_RSP_PUF_INTR_KEY 0xCE
#endif

//! Loading of Spectral mask configuration
#ifdef SPECTRAL_MASK
//! Vinay please correct this
// #define RSI_RSP_LOAD_SPECTRAL_MASK_CONFIG    0xF7
#define RSI_RSP_LOAD_SPECTRAL_MASK_CONFIG 0xB9
#endif

#ifdef TSF_SYNC
#define RSI_RSP_HOST_TSF_SYNC_PKT_RECVD 0xC1
#define RSI_STATION_CONN_IND            0xC2
#define RSI_STATION_DISCON_IND          0xC3
#define RSI_RSP_SET_AUTO_RATE           0xC5
#endif

#define RSI_RSP_FREQ_OFFSET           0xF3
#define RSI_RSP_CALIB_WRITE           0xCA
#define RSI_RSP_EVM_WRITE             0x37
#define RSI_RSP_CALIB_READ            0xCF
#define RSI_RSP_GET_RTC_DATE_AND_TIME 0xF2
#ifdef FIPS_CODE_ENABLE
#define RSI_RSP_FIPS_GET_FWUPGRADE_KEY 0xF6
#define RSI_RSP_FIPS_AUTO_JOIN_KEY     0xF7
#define RSI_RSP_FIPS_GENERATE_KEY      0xF8
#define RSI_RSP_LOAD_SECRET_KEY        0xF9
#define RSI_RSP_FIPS_FAILURE           0xFA
#define RSI_RSP_RE_KEY                 0xFB
#define RSI_RSP_GET_CFG_CHKSM          0xFC
#define RSI_RSP_FIPS_KEY_ZEROIZATION   0xFD
#define RSI_RSP_FIPS_MODE              0xFE
#endif
#ifdef FIPS_CRYPTO_CERTIFICATION
#define RSI_RSP_CRYPTO_TEST 0xFF
#endif
#define RSI_RSP_CRYPTO_HARDWARE 0x76

#ifdef CHIP_9117
#define RSI_RSP_ECDSA_256_VERIFY_HASH 0x3E
#endif
#ifdef FW_UP_FROM_HOST
#define RSI_RSP_FWUP_FRM_HOST 0x99
#endif

#ifndef ROM_IMAGE_20
//sleep and wake up confirm definitions
#define RSI_WKP_CFM   0x8A
#define RSI_SLEEP_CFM 0x85
#endif

// Control Request Frame Codes

#define RSI_RSP_WFD_DEV 0x20

// Management Frame Responses
#define RSI_RSP_SUCCESS                     0x00
#define RSI_RSP_ALLREADY_ASSOCIATED         0x02
#define RSI_RSP_NO_AP                       0x03
#define RSI_RSP_PSK_NOT_CONFIG              0x04
#define RSI_RSP_ROAM_LIST_EXCEEDED          0x05
#define RSI_RSP_ROAM_LIST_EMPTY             0x06
#define RSI_RSP_ROAM_SSID_NOT_FOUND         0x07
#define RSI_RSP_SECURITY_JOIN_FAILED        0x08
#define RSI_RSP_ROAM_CONNECT_ERROR          0x09
#define RSI_RSP_INVALID_CHANNEL_NUMBER      0x0A
#define RSI_RSP_INVALID_COMMAND_BEFORE_SCAN 0x0B
#define RSI_RSP_AUTHENTICATION_FAIL         0x0E
#define RSI_RSP_UNKNOWN_ERROR               0x64
#ifdef RSI_ASSERT_API
#define RSI_RSP_ASSERT 0XE1
#endif
#define RSI_RSP_RTC_DATE_AND_TIME 0xE9
#define RSI_RSP_SET_CONFIG        0xBA

/*==============================================*/
// Mode Value Defines
// Band Defines
#define RSI_BAND_2P4GHZ  0x00 // uint8
#define RSI_BAND_5GHZ    0x01 // uint8
#define RSI_DUAL_BAND    0x02 // uint8
#define RSI_BAND_4_9_GHZ 0x01
// P2P mode defines
#define RSI_P2P_DISABLE 0x00 // uint8
#define RSI_P2P_ENABLE  0x01 // uint8

// TCPIP Defines
#define RSI_DHCP_MODE_ENABLE  0x01
#define RSI_DHCP_MODE_DISABLE 0x00
#define RSI_AUTO_IP_CFG       0x02

// SOCKET Defines
#define RSI_SOCKET_TCP_CLIENT 0x0000 // uint16
#define RSI_SOCKET_UDP_CLIENT 0x0001 // uint16
#define RSI_SOCKET_TCP_SERVER 0x0002 // uint16
#define RSI_SOCKET_MULTICAST  0x0003 // uint16
#define RSI_SOCKET_LUDP       0x0004 // uint16

// SEND Defines
#define RSI_UDP_FRAME_HEADER_LEN 42
// UDP Frame header is 42 bytes, padding bytes are extra
#define RSI_TCP_FRAME_HEADER_LEN 54
// TCP Frame header is 54 bytes, padding bytes are extra
#define RSI_UDP_SEND_OFFSET 32
// Offset of sent UDP payload data
#define RSI_TCP_SEND_OFFSET 44
// Offset of sent TCP payload data
#define RSI_UDP_RECV_OFFSET 26
// Offset of received UDP payload data
#define RSI_TCP_RECV_OFFSET 38
// Offset of received TCP payload data

// SECURITY Type Defines
#define RSI_SECURITY_NONE 0
// Security type NONE and OPEN are alises for each other
#define RSI_SECURITY_OPEN 0
#define RSI_SECURITY_WPA1 1
#define RSI_SECURITY_WPA2 2
#define RSI_SECURITY_WEP  3

// NETWORK Type
#define RSI_IBSS_OPEN_MODE      0
#define RSI_INFRASTRUCTURE_MODE 1
#define RSI_IBSS_SEC_MODE       2
#define RSI_IBSS_JOINER         0
#define RSI_IBSS_CREATOR        1

// DATA Rates
#define RSI_DATA_RATE_AUTO 0
#define RSI_DATA_RATE_1    1
#define RSI_DATA_RATE_2    2
#define RSI_DATA_RATE_5P5  3
#define RSI_DATA_RATE_11   4
#define RSI_DATA_RATE_6    5
#define RSI_DATA_RATE_9    6
#define RSI_DATA_RATE_12   7

#define RSI_MACADDRLEN   6
#define RSI_DFRAMECMDLEN 4

#define RSI_SPISSWIFI  0 // 0 is for the WiFi
#define RSI_SPISSFLASH 1 // 1 for the SPI Flash

#define RSI_MODE_8BIT  1
#define RSI_MODE_32BIT 0

/**
 * Enumerations
 */
enum RSI_INTTYPE {
  RSI_IRQ_NONE             = 0x00,
  RSI_IRQ_BUFFERFULL       = 0x01,
  RSI_IRQ_BUFFEREMPTY      = 0x02,
  RSI_IRQ_MGMTPACKET       = 0x04,
  RSI_IRQ_DATAPACKET       = 0x08,
  RSI_IRQ_10               = 0x10,
  RSI_IRQ_DEEP_SLP_WKP     = 0x20,
  RSI_IRQ_PRETBTT          = 0x40,
  RSI_IRQ_DEEP_SLP_ENTERED = 0x80,
  RSI_IRQ_ANY              = 0xff
};

/*=====================================================================================*/
/**
 * 		This is platform dependent operation.Needs to be implemented 
 * 		specific to the platform.This timer is mentioned in the following functions
 * 			Application/TCPDemo/Source/main.c
 * 			WLAN/SPI/Source/spi_functs.c
 * 			WLAN/SPI/Source/spi_iface_init.c
 * 	
 */

extern uint32 rsi_spiTimer1;
extern uint32 rsi_spiTimer2;
extern uint32 rsi_spiTimer3;
extern volatile rsi_powerstate rsi_pwstate;
/*
 * Function Prototype Definitions
 */
int16 rsi_band(uint8 band);
int16 rsi_init(void);
int16 rsi_bootloader(void);
int16 rsi_disconnect(void);
int16 rsi_ipparam_set(rsi_uIpparam *uIpparamFrame);
int16 rsi_query_rssi(void);
int16 rsi_set_mac_addr(uint8 *macAddress);
int16 rsi_query_dns(rsi_uDns *uDnsFrame);
int16 rsi_power_mode(uint8 powermode);
int16 rsi_socket(rsi_uSocket *uSocketFrame);
int16 rsi_socket_close(uint16 socketDescriptor);
int16 rsi_query_conn_status(void);
int16 rsi_query_dhcp_parms(void);
int16 rsi_query_fwversion(void);
int16 rsi_query_net_parms(void);
int16 rsi_query_bssid_nwtype(void);
int16 rsi_set_listen_interval(uint8 *listeninterval);
int16 rsi_send_data(uint16 socketDescriptor, uint8 *payload, uint32 payloadLen, uint8 protocol);
int16 rsi_spi_iface_init(void);
int16 rsi_module_power_cycle(void);
int16 rsi_SpiFrameDscRd(rsi_uFrameDsc *uFrmDscFrame);
int16 rsi_SpiFrameDscWr(rsi_uFrameDsc *uFrmDscFrame, uint8 type);
int16 rsi_SpiFrameDataRd(uint16 bufLen, uint8 *dBuf);
int16 rsi_SpiFrameDataWr(uint16 bufLen, uint8 *dBuf, uint16 tbufLen, uint8 *tBuf, uint8 type);
int16 rsi_memWr(uint32 addr, uint16 len, uint8 *dBuf);
int16 rsi_memRd(uint32 addr, uint16 len, uint8 *dBuf);
int16 rsi_sendC1C2(uint8 c1, uint8 c2);
int16 rsi_sendC3C4(uint8 c3, uint8 c4);
int16 rsi_spiWaitStartToken(uint32 timeout, uint8 mode);
int16 rsi_irqstatus(void);
void rsi_buildFrameDescriptor(rsi_uFrameDsc *uFrameDscFrame, uint16 *cmd);
void rsi_setMFrameFwUpgradeLen(rsi_uFrameDsc *uFrameDscFrame, uint16 len);
int16 rsi_set_intr_mask(uint8 interruptMask);
int16 rsi_clear_interrupt(uint8 interruptClear);
int16 rsi_regRd(uint8 addr, uint8 *dBuf);
int16 rsi_regWr(uint8 regAddr, uint8 *dBuf);
int16 rsi_sys_init(void);
int16 rsi_read_packet(rsi_uCmdRsp *uCmdRspFrame);

int16 rsi_execute_control_cmd(uint16 *descparam, uint8 *payloadparam, uint16 size_param);
int16 rsi_spi_bind_stop(void);
int16 rsi_spi_bind_start(void);

int16 rsi_hal_mcu_sysinit(void);
int16 rsi_hal_mcu_shutdown(void);

uint8 rsi_checkPktIrq(void);
void rsi_clearPktIrq(void);
uint8 rsi_checkBufferFullIrq(void);
uint8 rsi_checkIrqStatus(void);
uint8 rsi_checkPowerModeIrq(void);
int16 rsi_pwrsave_continue(void);
int16 rsi_pwrsave_hold(void);
#endif

/**
*$Log: rsi_spi_api.h,v $
*Revision 1.1.1.1  2012/02/28 07:14:12  kiran.gunda
*WiSe Connet initial import
*
**/
