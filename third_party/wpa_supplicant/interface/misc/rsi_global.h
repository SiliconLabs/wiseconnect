/*******************************************************************************
* @file  rsi_global.h
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
 * @brief HEADER, GLOBAL, Global Header file, the things that must be almost everywhere 
 *
 * @section Description
 * This is the top level global.h file for things that need to be in every module
 *
 *
 */

#ifndef _RSIGLOBAL_H_
#define _RSIGLOBAL_H_

#include "packed.h"
//#include "rsi_debug_prints.h"

#ifdef NETX_ENABLE
#include "nx_api.h"
#endif
/**
 * Global defines
 */

#define IP_VERSION 4

//@ Some things are boolean values
#define RSI_TRUE  1
#define RSI_FALSE 0

//@ Interrupt Mode Selection
//#define RSI_INTERRUPTS

//@ Polled Mode selection
#define RSI_POLLED
//#define RSI_RX62N

#define RSI_FIRMWARE_UPGRADE 0

/*
 * @Time out for various operations
 */

#ifndef RSI_HWTIMER /*@ need to define this macro if h/w timer is available and it should increment spiTimer2, spiTimer1 */
#define RSI_TICKS_PER_SECOND 5000 /*@ approximate ticks for timeout implementation */
#define RSI_INC_TIMER_2      rsi_spiTimer2++
#define RSI_INC_TIMER_1      rsi_spiTimer1++
#define RSI_INC_TIMER_3      rsi_spiTimer3++
#define RSI_RESET_TIMER1     rsi_spiTimer1 = 0
#define RSI_RESET_TIMER2     rsi_spiTimer2 = 0
#define RSI_RESET_TIMER3     rsi_spiTimer3 = 0
#else
#define RSI_TICKS_PER_SECOND 10
#define RSI_INC_TIMER_3      rsi_spiTimer3
#define RSI_INC_TIMER_2      rsi_spiTimer2
#define RSI_INC_TIMER_1      rsi_spiTimer1
#define RSI_RESET_TIMER1     rsi_spiTimer1 = 0
#define RSI_RESET_TIMER2     rsi_spiTimer2 = 0
#define RSI_RESET_TIMER3     rsi_spiTimer3 = 0
#endif

/*@ firmware upgradation timeout */
#define RSI_FWUPTIMEOUT 100 * RSI_TICKS_PER_SECOND
/*@ bootloading timeout */
#define RSI_BLTIMEOUT 1 * RSI_TICKS_PER_SECOND
/*@ band timeout */
#define RSI_BANDTIMEOUT 1 * RSI_TICKS_PER_SECOND
/*@ Init timeout */
#define RSI_INITTIMEOUT 1 * RSI_TICKS_PER_SECOND
/*@ Query firmware version timeout */
#define RSI_QFWVTIMEOUT 1 * RSI_TICKS_PER_SECOND
/*@ Set Mac address timeout */
#define RSI_SMATIMEOUT 1 * RSI_TICKS_PER_SECOND
/*@ Scan timeout */
#define RSI_SCANTIMEOUT 10 * RSI_TICKS_PER_SECOND
/*@ Join timeout */
#define RSI_JOINTIMEOUT 8 * RSI_TICKS_PER_SECOND
/*@ Disconnect timeout */
#define RSI_DISCONTIMEOUT 1 * RSI_TICKS_PER_SECOND
/*@ Query connection status timeout */
#define RSI_QCSTIMEOUT 3 * RSI_TICKS_PER_SECOND
/*@ Query dhcp params timeout */
#define RSI_QDPTIMEOUT 3 * RSI_TICKS_PER_SECOND
/*@ Query network params timeout */
#define RSI_QNPTIMEOUT 3 * RSI_TICKS_PER_SECOND
/*@ Ip configuration timeout */
#define RSI_IPPTIMEOUT 6 * RSI_TICKS_PER_SECOND
/*@ Query RSSI Value timeout */
#define RSI_RSSITIMEOUT 1 * RSI_TICKS_PER_SECOND
/*@ recv timeout */
#define RSI_RECVTIMEOUT 1 * RSI_TICKS_PER_SECOND
/*@ Socket open timeout */
#define RSI_SOPTIMEOUT 3 * RSI_TICKS_PER_SECOND
/*@ Regread timeout */
#define RSI_REGREADTIMEOUT 1 * RSI_TICKS_PER_SECOND
/*@ Query DNS timeout */
#define RSI_QDNSTIMEOUT 6 * RSI_TICKS_PER_SECOND
/*@ Start token timeout */
#define RSI_START_TOKEN_TIMEOUT 10 * RSI_TICKS_PER_SECOND
/*@ Set Listen interval timeout */
#define RSI_SLITIMEOUT 1 * RSI_TICKS_PER_SECOND

/*@ Command response timeout */
#define RSI_RESPONSE_TIMEOUT(A)           \
  RSI_RESET_TIMER3;                       \
  while (rsi_checkPktIrq() != RSI_TRUE) { \
    if (RSI_INC_TIMER_3 > A) {            \
      retval = -1;                        \
      break;                              \
    }                                     \
  }
//#define Cpu_EnableInt()  CONDITIONAL_INTR_ENABLE(ipl_reg);
//#define Cpu_DisableInt() uint32 ipl_reg = CONDITIONAL_INTR_DISABLE();

/*=======================================================================================*/
/**
 * Device Parameters
 */
#define RSI_MAXSOCKETS 8 // maximum number of open sockets

/**
 * Debugging Parameters
 */
#define RSI_DEBUG_DEVICE           "UART_1"
#define RSI_MAX_PAYLOAD_SIZE       1400 // maximum data payload size
#define RSI_AP_SCANNED_MAX         15   // maximum number of scanned acces points
#define RSI_MAX_AP_SCANNED_TO_HOST 11   // maximum number of scanned acces points response to host

/**
 * Things that are needed in this .h file
 */
#define RSI_FRAME_DESC_LEN      16 // Length of the frame descriptor, for both read and write
#define RSI_FRAME_CMD_RSP_LEN   56 // Length of the command response buffer/frame
#define RSI_TXDATA_OFFSET_TCP   44 // required Tx data offset value for TCP, 44
#define RSI_TXDATA_OFFSET_UDP   32 // required Tx data offset value for UDP, 32
#define RSI_RXDATA_OFFSET_TCP   38 // required Rx data offset value for TCP, 38
#define RSI_RXDATA_OFFSET_UDP   26 // required Rx data offset value for UDP, 26
#define RSI_PSK_LEN             64 // maximum length of PSK
#define RSI_PMK_LEN             32 // maximum length of PMK
#define RSI_SSID_LEN            34 // maximum length of SSID
#define RSI_WPS_PIN_LEN         8  // maximum length of WPS PIN
#define RSI_IP_ADD_LEN          4
#define RSI_MAC_ADD_LEN         6
#define RSI_MGMT_PKT_TYPE       0x00
#define RSI_DATA_PKT_TYPE       0x02
#define RSI_MAX_DOMAIN_NAME_LEN 42
#define RSI_MAX_DNS_REPLY       10

#define RSI_LITTLE_ENDIAN 1
//#define RSI_BIT_32_SUPPORT 0

/*===============================================*/
/**
 * PSK_OR_PMK Structure
 */

#ifdef SPECTRAL_MASK

#define SPECTRAL_MASK_CONFIG_SIZE 42

typedef struct Spectral_Mask_Config_Req {
  UINT8 length[2];
} __PACKED__ Spectral_Mask_Config_Req;

#endif

/*===============================================*/
/**
 * Scan Structures
 */

typedef struct rsi_set_mac_s {
  uint8 mac[6];
} rsi_setmac;

/*********************************/
/* Added by venkatesh */
typedef union {
  struct {
    uint8 total_len[2];
    uint8 current_len[2];
    uint8 more_chunks;
  } webpageFrameSnd;
  uint8 uWebpageBuf[2 + 2 + 1 + 3 /* padding bytes */];
} rsi_uWebpage;
#define MAX_NO_OF_FIELDS 10
typedef struct field_st {
  uint8 field_index;
  uint8 field_val[64];
} field_st;
typedef union {
  struct {
    uint8 field_cnt;
    struct field_st field_st[MAX_NO_OF_FIELDS];
  } webFieldsFrameSnd;
  uint8 uWebFieldBuf[680];
} rsi_uWebFields;
/*********************************/

typedef struct rsi_scanInfo {
  uint8 rfChannel;    //@ rf channel to us, 0=scan for all
  uint8 securityMode; //@ security mode, 0=open, 1=wpa1, 2=wpa2, 3=wep
  uint8 rssiVal;      //@ absolute value of RSSI
  uint8 uNetworkType;
  uint8 ssid[34]; //@ 32-byte ssid of scanned access point
  uint8 bssid[6];
#ifndef RSI_FEATSEL_ENABLE
  uint8 reserved[2];
#else
  uint8 snr;
  uint8 reserved;
  uint8 ap_name[16];
#endif
} __attribute__((packed)) rsi_scanInfo_t;

typedef struct rsi_uScanList {
  uint8 scanCount[4]; //@ uint32, number of access points found
  uint8 padding[4];
  rsi_scanInfo_t strScanInfo[RSI_AP_SCANNED_MAX];
} __attribute__((packed)) rsi_uScanList_t;

typedef struct rsi_uwps_s {
  uint16 wps_method;
  uint16 generate_pin;
  uint8 wps_pin[RSI_WPS_PIN_LEN + 1];
} rsi_uwps_t;

typedef struct module_rtc_time_s {
  uint32 tm_sec;  /* seconds after the minute [0-60] */
  uint32 tm_min;  /* minutes after the hour [0-59] */
  uint32 tm_hour; /* hours since midnight [0-23] */
  uint32 tm_mday; /* day of the month [1-31] */
  uint32 tm_mon;  /* months since January [0-11] */
  uint32 tm_year; /* years since 1900 */
} module_rtc_time_t;

#ifdef ENABLE_DRAEGER_CUSTOMIZATION
/*Possible Reason Code*/
#define RSI_MODULE_STATE_START_UP_OR_NOT_AVAVILABLE 0x00
#define RSI_MODULE_STATE_BEACON_LOSS                0x10
#define RSI_MODULE_STATE_DEAUTH_FROM_AP             0x20
#define RSI_MODULE_STATE_AUTH_FAILED                0x01
#define RSI_MODULE_STATE_ASSOC_FAILED               0x02
#define RSI_MODULE_STATE_DEAUTH_FRM_HOST            0x06
#define RSI_MODULE_STATE_PSK_NOT_CONF               0x07
#define RSI_MODULE_STATE_SUPP_DEAUTH                0x08
#define RSI_MODULE_STATE_ROAMING_NOT_ENABLED        0x09

#define RSI_MODULE_STATE_CURRENT_AP_IS_BEST        0x50
#define RSI_MODULE_STATE_BETTER_AP_FOUND           0x60
#define RSI_MODULE_STATE_NO_AP_FOUND               0x70
#define RSI_MODULE_STATE_ASSOCIATED                0x80
#define RSI_MODULE_STATE_UNASSOCIATED              0x90
#define RSI_MODULE_STATE_CONFIGURED_AP_NOT_PRESENT 0x03
#define RSI_MODULE_STATE_EAP_DENAIL                0x04
#define RSI_MODULE_STATE_FOUR_WAY_HANDSHAKE_FAIL   0x05
#define RSI_MODULE_STATE_RSSI_NOT_AVAILABLE        0x64

/*Possible module state*/
#define RSI_MODULE_STATE_SCANNING        1
#define RSI_MODULE_STATE_SCAN_RESULT     2
#define RSI_MODULE_STATE_FINAL_CONNECTON 3

/*Structure for module state async. notification*/
typedef struct rsi_con_state_notificaton_s {
  uint32 TimeStamp;
  uint8 stateCode;
  uint8 reason_code;
  uint8 rsi_channel;
  uint8 rsi_rssi;
  uint8 rsi_bssid[6];
} __PACKED__ rsi_con_state_notificaton_t;
uint8 rsi_find_reason_code(uint8 failure_casue);
void wise_module_state(uint8 currentState);
#endif

// Control Frames
/**
 * TCP/IP Configure
 */
// Command

typedef union {
  struct {
    uint8 ipparamCmd[2]; // int16, TCPIP/NETWORK command code, 0x0001
    uint8 dhcpMode;      // 0=Manual, 1=Use DHCP
    uint8 ipaddr[4];     // IP address of this module if in manual mode
    uint8 netmask[4];    // Netmask used if in manual mode
    uint8 gateway[4];    // IP address of default gateway if in manual mode
    uint8 dnsip[4];
    uint8 padding;
  } ipparamFrameSnd;
  uint8 uIpparamBuf[20]; // 16 bytes, byte format to send to spi
} rsi_uIpparam;

/*===============================================*/
/**
 * Dns get request
 */
// Command

typedef union {
  struct {
    uint8 DnsGetCmd[2];                        // uint16 dns get command 0x0011
    uint8 DomainName[RSI_MAX_DOMAIN_NAME_LEN]; // 42 bytes, Domain name like www.google.com
  } dnsFrameSnd;
  uint8 uDnsBuf[44]; // 16 bytes, byte format to send to spi
} rsi_uDns;

/*===================================================*/
/**
 * Socket Configure
 */
// Commad
typedef union {
  struct {
    uint8 socketCmd[2];    // uint16, socket command code, 0x0002
    uint8 socketType[2];   // 0=TCP Client, 1=UDP Client, 2=TCP Server (Listening TCP)
    uint8 moduleSocket[2]; // Our local module port number
    uint8 destSocket[2];   // Port number of what we are connecting to
    uint8 destIpaddr[4];   // IP address of socket we are connecting to
  } socketFrameSnd;
  uint8 uSocketBuf[12]; // 12 bytes, byte format to send to spi
} rsi_uSocket;

/*===================================================*/
/**
 * Sockets Structure
 * Structure linking socket number to protocol
 */
typedef struct {
  uint8 socketDescriptor[2]; // socket descriptor
  uint8 protocol;            // PROTOCOL_UDP, PROTOCOL_TCP or PROTOCOL_UNDEFINED
  uint8 src_port[2];         // socket local port number
  uint8 dest_ip[4];          // Destination ipaddress
  uint8 dest_port[2];        // Destination port number
} rsi_socketsStr;

/*===================================================*/
/**
 * Sockets Structure Array
 * Array of Structures linking socket number to protocol
 */
typedef struct {
  rsi_socketsStr socketsArray[RSI_MAXSOCKETS + 1]; // Socket numbers are from 1 to 8
} rsi_sockets;

/*===================================================*/
/**
 * Socket Close
 */

typedef union {
  struct {
    uint8 socketCloseCmd[2];   // uint16, command code
    uint8 socketDescriptor[2]; // uint16, socket descriptor to close
  } socketCloseFrameSnd;
  uint8 uSocketCloseBuf[4]; // byte format to send to the spi interface, 8 bytes
} rsi_uSocketClose;

/*===================================================*/
/**
 * Set Mac Address
 */

typedef union {
  struct {
    uint8 setMacAddrCmd[2]; // uint16, command code
    uint8 macAddr[6];       // byte array, mac address
  } setMacAddrFrameSnd;
  uint8 setMacAddrBuf[8]; // byte format to send to the spi interface, 8 bytes

} rsi_uSetMacAddr;

/*===================================================*/
/**
 * Set Listen interval
 */
typedef union {
  struct {
    uint8 setListenIntervalCmd[2]; // uint16, command code
    uint8 interval[2];             // uint16, listen inteval
  } setListenIntervalFrameSnd;
  uint8 setListenIntervalBuf[4]; // byte format to send to the spi interface, 4 bytes
} rsi_uSetListenInterval;

/*===================================================*/
/**
 * Band
 */

typedef union {
  struct {
    uint8 bandVal;    // uint8, band value to set
    uint8 padding[3]; // needs to be 4 bytes long
  } bandFrameSnd;
  uint8 uBandBuf[4]; // byte format to send to the spi interface, 4 bytes
} rsi_uBand;

/*===================================================*/
/**
 * Power
 */

typedef union {
  struct {
    uint8 powerVal; // uint16, power value to set
    uint8 ulp_mode_enable;
    uint8 listen_interval_dtim;
#ifdef ENABLE_FAST_PSP
    uint8 fast_psp_enable;
    uint16 monitor_interval;
#endif
  } powerFrameSnd;
  uint8 uPowerBuf[2]; // byte format to send to the spi interface, 4 bytes
} rsi_uPower;

/*===================================================*/
/**
 * SEND
 */
typedef union {
  struct {
    uint8 socketDescriptor[2];   // socket descriptor of the already opened socket connection
    uint8 sendBufLen[4];         // length of the data to be sent
    uint8 sendDataOffsetSize[2]; // Data Offset, TCP=44, UDP=32
    uint8 padding[RSI_TXDATA_OFFSET_TCP + RSI_MAX_PAYLOAD_SIZE]; // large enough for TCP or UDP frames
  } sendFrameSnd;
  struct {
    uint8 socketDescriptor[2];                      // socket descriptor of the already opened socket connection
    uint8 sendBufLen[4];                            // length of the data to be sent
    uint8 sendDataOffsetSize[2];                    // Data Offset, TCP=44, UDP=32
    uint8 sendDataOffsetBuf[RSI_TXDATA_OFFSET_UDP]; // Empty offset buffer, UDP=32
    uint8 sendDataBuf[RSI_MAX_PAYLOAD_SIZE];        // Data payload buffer, 1400 bytes max
  } sendFrameUdpSnd;
  struct {
    uint8 socketDescriptor[2];                      // socket descriptor of the already opened socket connection
    uint8 sendBufLen[4];                            // length of the data to be sent
    uint8 sendDataOffsetSize[2];                    // Data Offset, 44 for TCP, 32 for UDP
    uint8 sendDataOffsetBuf[RSI_TXDATA_OFFSET_TCP]; // Empty offset buffer, 44 for TCP
    uint8 sendDataBuf[RSI_MAX_PAYLOAD_SIZE];        // Data payload buffer, 1400 bytes max
  } sendFrameTcpSnd;
  uint8 uSendBuf[RSI_MAX_PAYLOAD_SIZE + RSI_TXDATA_OFFSET_TCP
                 + 8]; // byte format to send to spi, TCP is the larger of the two, 1456 bytes
} rsi_uSend;

/*===================================================*/
/**
 * Frame Descriptor
 */
typedef union {
  struct {
    uint8 mgmtFrmBodyLenShort; // If < 0xff, length of the management frame body
    uint8 mgmtPacketType;      // Management packet type, Band, Init, Scan, Join, Power, fftaim1, fftaim2, fftadm
    uint8 padding1[6];         // Unused, set to 0x00
    uint8 mgmtFrmBodyLenLong
      [2];              // If managementFrameBodyLenShort=0xff, these two bytes are the management frame body length
    uint8 padding2[4];  // Unused, set to 0x00
    uint8 frameType[2]; // 0x0004=Management Frame, 0x0002=Data Frame
  } frameDscMgmtSnd;
  struct {
    uint8 dataFrmBodyLen[2]; // Data frame body length. Bits 15:12=0, Bits 11:0 are the length
    uint8 padding[12];       // Unused, set to 0x00
    uint8 frameType[2];      // 0x0004=Management Frame, 0x0002=Data Frame
  } frameDscDataSnd;
  struct {
    uint8 mgmtFrmBodyLenQueue[2]; // If < 0xff, length of the management frame body
    uint8 mgmtFrmDscRspStatus[2]; // Management frame descriptor response status, 0x00=success, else error
    uint8 padding1[10];           // Unknown
    uint8 mgmtPacketType[2];      // Management packet type, Band, Init, Scan, Join, Power, fftaim1, fftaim2, fftadm
  } frameDscMgmtRsp;
  uint8 uFrmDscBuf[RSI_FRAME_DESC_LEN]; // byte format for spi interface, 16 bytes
} rsi_uFrameDsc;

/*===================================================*/
/**
 * Command Response Frame Union
 */
typedef struct {
  uint8 rfChannel;    // rf channel to us, 0=scan for all
  uint8 securityMode; // security mode, 0=open, 1=wpa1, 2=wpa2, 3=wep
  uint8 rssiVal;      // absolute value of RSSI
  uint8 network_type;
  uint8 ssid[RSI_SSID_LEN]; // 32-byte ssid of scanned access point
  uint8 bssid[6];           // BSSID of the scanned AP
  uint8 reserved[2];
} rsi_scanInfo;

typedef struct {
  uint8 rfChannel;          // rf channel to us, 0=scan for all
  uint8 securityMode;       // security mode, 0=open, 1=wpa1, 2=wpa2, 3=wep
  uint8 rssiVal;            // absolute value of RSSI
  uint8 ssid[RSI_SSID_LEN]; // 32-byte ssid of scanned access point
  uint8 uNetworkType;
  uint8 BSSID[6];
} rsi_bssid_nwtypeInfo;

typedef struct {
  uint8 rspCode[4];
  uint8 scanCount[4];                           // uint32, number of access points found
  rsi_scanInfo strScanInfo[RSI_AP_SCANNED_MAX]; // 32 maximum responses from scan command
  uint8 status;
} rsi_scanResponse;

typedef struct {
  uint8 rspCode[2];
  uint8 status;
} rsi_mgmtResponse;

typedef struct {
  uint8 rssiVal[2]; // uint16, RSSI value for the device the module is currently connected to
} rsi_rssiFrameRcv;

typedef struct {
  uint8 socketType[2];       // uint16, type of socket created
  uint8 socketDescriptor[2]; // uinr16, socket descriptor, like a file handle, usually 0x00
  uint8 moduleSocket[2];     // uint16, Port number of our local socket
  uint8 moduleIpaddr[4];     // uint32, Our (module) IP Address
} rsi_socketFrameRcv;

typedef struct {
  uint8 socketDsc[2]; // uint16, socket that was closed
} rsi_socketCloseFrameRcv;

typedef struct {
  uint8 macAddr[6]; // MAC address of this module
  uint8 ipaddr[4];  // Configured IP address
  uint8 netmask[4]; // Configured netmask
  uint8 gateway[4]; // Configured default gateway
} rsi_ipparamFrameRcv;

typedef struct {
  uint8 state[2]; // uint16, connection state, 0=Not Connected, 1=Connected
} rsi_conStatusFrameRcv;

typedef struct {
  uint8 leaseTime[4];     // uint32, total lease time for dhcp connection
  uint8 leaseTimeLeft[4]; // uint32, lease time left for dhcp connection
  uint8 renewTime[4];     // uint32, time left for renewal of IP address
  uint8 rebindTime[4];    // uint32, time left for rebind of IP address
  uint8 serverIpAddr[4];  // uint8[4], IP address of DHCP server
} rsi_qryDhcpInfoFrameRcv;

typedef struct {
  uint8 wlanState[2];       // uint16, 0=NOT Connected, 1=Connected
  uint8 ssid[RSI_SSID_LEN]; // uint8[32], SSID of connected access point
  uint8 ipaddr[4];          // uint8[4], Module IP Address
  uint8 subnetMask[4];      // uint8[4], Module Subnet Mask
  uint8 gateway[4];         // uint8[4], Gateway address for the Module
  uint8 dhcpMode[2];        // uint16, 0=Manual IP Configuration,1=DHCP
  uint8 connType[2];        // uint16, 0=AdHoc, 1=Infrastructure
} rsi_qryNetParmsFrameRcv;

typedef struct {
  uint8 rspCode[2];
  uint8 fwversion[20]; // uint8[20], firmware version text string, x.y.z as 1.3.0
} rsi_qryFwversionFrameRcv;

typedef struct {
  uint8 rspCode[2];
  uint8 macaddress[6]; // uint8[20], firmware version text string, x.y.z as 1.3.0
} rsi_qryMacFrameRcv;

typedef struct {
  uint8 recvSocket[2];         // uint16, the socket number associated with this read event
  uint8 recvBufLen[4];         // uint32, length of data received
  uint8 recvDataOffsetSize[2]; // uint16, offset of data from start of buffer
  uint8 fromPortNum[2];        // uint16, port number of the device sending the data to us
  uint8 fromIpaddr[4];         // uint32, IP Address of the device sending the data to us
  uint8 recvDataOffsetBuf
    [RSI_RXDATA_OFFSET_UDP];               // uint8, empty offset buffer, 26 for UDP, 42 bytes from beginning of buffer
  uint8 recvDataBuf[RSI_MAX_PAYLOAD_SIZE]; // uint8, buffer with received data
  uint8 padding[2];                        // data length received should always be in multiples of 4 bytes
} rsi_recvFrameUdp;

typedef struct {
  uint8 recvSocket[2];                            // uint16, the socket number associated with this read event
  uint8 recvBufLen[4];                            // uint32, length of payload data received
  uint8 recvDataOffsetSize[2];                    // uint16, offset of data from start of buffer
  uint8 fromPortNum[2];                           // uint16, port number of the device sending the data to us
  uint8 fromIpaddr[4];                            // uint32, IP Address of the device sending the data to us
  uint8 recvDataOffsetBuf[RSI_RXDATA_OFFSET_TCP]; // uint8, empty offset buffer, 38 for TCP
  uint8 recvDataBuf[RSI_MAX_PAYLOAD_SIZE];        // uint8, buffer with received data
  uint8 padding[2];                               // data length received should always be in multiples of 4 bytes
} rsi_recvFrameTcp;

typedef struct {
  uint8 socket[2]; // uint16, socket handle for the terminated connection
} rsi_recvRemTerm;

typedef struct {
  uint8 socket[2];      // uint16, socket handle
  uint8 fromPortNum[2]; // uint16, remote port number
  uint8 fromIpaddr[4];  // uint32, remote IP Address
} rsi_recvLtcpEst;

typedef struct {
  uint8 uipcount[2];
  uint8 ipaddr[RSI_MAX_DNS_REPLY][4];
} rsi_dnsFrameRcv;

typedef struct {
  uint8 rspCode[4];
  uint8 scanCount[4];                                           // uint32, number of access points found
  rsi_bssid_nwtypeInfo strBssid_NwtypeInfo[RSI_AP_SCANNED_MAX]; // 32 maximum responses from scan command
  uint8 status;
} rsi_bssid_nwtypeFrameRecv;

typedef union {
  uint8 rspCode[2]; // command code response
  rsi_scanResponse scanResponse;
  rsi_mgmtResponse mgmtResponse;
  rsi_rssiFrameRcv rssiFrameRcv;
  rsi_socketFrameRcv socketFrameRcv;
  rsi_socketCloseFrameRcv socketCloseFrameRcv;
  rsi_ipparamFrameRcv ipparamFrameRcv;
  rsi_conStatusFrameRcv conStatusFrameRcv;
  rsi_qryDhcpInfoFrameRcv qryDhcpInfoFrameRcv;
  rsi_qryNetParmsFrameRcv qryNetParmsFrameRcv;
  rsi_qryFwversionFrameRcv qryFwversionFrameRcv;
  rsi_recvFrameUdp recvFrameUdp;
  rsi_recvFrameTcp recvFrameTcp;
  rsi_recvRemTerm recvRemTerm;
  rsi_recvLtcpEst recvLtcpEst;
  rsi_dnsFrameRcv dnsFrameRcv;
  rsi_bssid_nwtypeFrameRecv bssid_nwtypeFrameRecv;
  rsi_qryMacFrameRcv qryMacFrameRcv;
  uint8 uCmdRspBuf[RSI_FRAME_CMD_RSP_LEN + RSI_MAX_PAYLOAD_SIZE];
} rsi_uCmdRsp;

/*===================================================*/
/**
 * Interrupt Handeling Structure
 */
typedef struct {
  uint8 mgmtPacketPending; // TRUE for management packet pending in module
  uint8 dataPacketPending; // TRUE for data packet pending in module
  uint8 powerIrqPending;   // TRUE for power interrupt pending in the module
  uint8 bufferFull;        // TRUE=Cannot send data, FALSE=Ok to send data
  uint8 bufferEmpty;       // TRUE, Tx buffer empty, seems broken on module
  int16 isrRegLiteFi;
} rsi_intStatus;

/*==============================================*/
/**
 * Main struct which defines all the paramaters of the API library
 * This structure is defined as a variable, the  pointer to, which is passed
 *  toall the functions in the API
 * The struct is initialized by a set of #defines for default values
 *  or for simple use cases
 */

typedef struct {
  uint16 socketDescriptor;    // uint16, socket descriptor of the last opened socket
  uint8 spiSsActive;          // Which SPI SS is currently active, set this to talk to a different peripheral
  uint8 band;                 // uint8, frequency band to use
  uint8 powerLevel;           // uint8, xmit power level
  uint8 scratch;              // uint8, scratch variable for unspecific use
  uint8 powerMode[2];         // uint16, power mode, 0=power mode0, 1=power mode 1, 2=power mode 2
  uint8 macAddress[6];        // 6 bytes, mac address
  rsi_uScan uScanFrame;       // frame sent as the scan command
  rsi_uJoin uJoinFrame;       // frame sent as the join command
  rsi_uIpparam uIpparamFrame; // frame sent to set the IP parameters
  rsi_uSocket uSocketFrame;   // frame sent to set the socket connection
  rsi_uDns uDnsFrame;
  uint8 listeninterval[2];
  // to control the number of times user function is called 0 -> funtion called once , >0 -> multiple times
  int8 iterations;
} rsi_api;
/*==================================================*/
/**
 * This structure maintain power save state.
 *
 */
typedef struct {
  uint8 current_mode;
  uint8 ack_pwsave;
} rsi_powerstate;

typedef struct debug {
  uint16 debug_location;
} debug_loc_ds;

/* GPIO CHANGE */
typedef struct gpio_conf_s {
  uint8 pin;
  uint8 direction; //0: for Input and 1: for output and 2: for disabling the pin
  uint8 value;
  uint8 reserved;
} gpio_conf_t;

/*@breif Need to define MINIMAL_CODE to get minimal code size */
#define RSI_MINIMAL_CODE
#ifdef RSI_MINIMAL_CODE
#define RSI_DEBUG_LVL 0x0000
#else
#define RSI_DEBUG_LVL 0x00ff
#endif

#ifdef PUF_INTRINSIC
typedef struct puf_intr_key_s {
  UINT8 key_index;
  UINT8 key_size;
} puf_intr_key_t;
#endif
#ifdef PUF_ENABLE

typedef struct puf_set_key_s {
  UINT8 key_index;
  UINT8 key_size;
  UINT8 key[32];
} puf_set_key_t;

typedef struct puf_get_key_s {
  UINT8 key_code[44];
} puf_get_key_t;

typedef struct puf_load_key_s {
  UINT8 key_code[44];
} puf_load_key_t;

typedef struct aes_encrypt_s {
  UINT8 mode;
  UINT8 key[32];
  UINT8 iv[32];
  UINT8 size_of_plain_data[2];
  UINT8 plain_data[500];
} aes_encrypt_t;

typedef struct aes_decrypt_s {
  UINT8 mode;
  UINT8 key[32];
  UINT8 iv[32];
  UINT8 size_of_encry_data[2];
  UINT8 encry_data[500];
} aes_decrypt_t;

typedef struct aes_mac_s {
  UINT8 mode;
  UINT8 key[32];
  UINT8 iv[32];
  UINT8 size_of_plain_data[2];
  UINT8 plain_data[500];
} aes_mac_t;

#endif

//@ Debug Print Levels
//@ These bit values may be ored to all different combinations of debug printing
#define RSI_PL0  0xffff
#define RSI_PL1  0x0001
#define RSI_PL2  0x0002
#define RSI_PL3  0x0004
#define RSI_PL4  0x0008
#define RSI_PL5  0x0010
#define RSI_PL6  0x0020
#define RSI_PL7  0x0040
#define RSI_PL8  0x0080
#define RSI_PL9  0x0100
#define RSI_PL10 0x0200
#define RSI_PL11 0x0400
#define RSI_PL12 0x0800
#define RSI_PL13 0x1000
#define RSI_PL14 0x2000
#define RSI_PL15 0x4000
#define RSI_PL16 0x8000

/**
 * Enumerations
 */
enum RSI_INTSOURCE { RSI_FROM_MODULE, RSI_FROM_CODE, RSI_FROM_UNDEFINED };

enum RSI_INTERRUPT_TYPE {
  RSI_TXBUFFER_FULL  = 0x01,
  RSI_TXBUFFER_EMPTY = 0x02,
  RSI_MGMT_PENDING   = 0x04,
  RSI_DATA_PENDING   = 0x08,
  RSI_PWR_MODE       = 0x10
};

extern rsi_api rsi_strApi;
// global array of 8 possible open sockets with their associated protocol type
#include "rsi_spi_api.h"
void rsi_delayMs(uint16 delay);

#define SUCCESS 0
#define ERROR   1

/* Error codes */
#define INVALID_RATE                     51
#define DISCONNECT_IN_WRONG_STATE        31
#define WISE_P2P_PROV_DISC_FAILED        39 /* provision discovery failed in P2P*/
#define WISE_ERROR_FAILED_TO_CLEAR       41 /* failed to clear certificate or Webpage */
#define WISE_ERROR_STORE_CONFIG_GOING_ON 44 /* Store config connecting process is going on */
#define ICMP_REQUEST_TIMEOUT_ERROR       46
//#define LARGE_PING_DATA_BYTES					47
#define TX_DATA_PACKET_SIZE_EXCEEDED            48
#define ARP_CACHE_ENTRY_NOT_FOUND               49
#define UART_CMD_TIMEOUT                        50
#define GO_ENDING_SESSION                       53
#define MQTT_PARAM_LENGTH_SIZE_EXCEEDED         54
#define FWUP_RESP_OUT_OF_ORDER                  52
#define WISE_ERROR_WRONG_PMK_LEN                57
#define WISE_ERROR_SSID_LENGTH_EXCEEDED         59
#define ERROR_IN_LENGTH_OF_USER_STORECFG_PARAMS 61
#define WISE_ERROR_PACKET_DROPPED               63
#define WISE_ERROR_MGMT_PACKET_DROPPED          80
#define ERROR_IN_LENGTH_OF_STORECFG_PROFILE     65

#define UART_FLOW_CONTROL_NOT_SUPPORTED                78
#define BT_ZB_PROTOCOL_NOT_ENABLED_BUT_RECEIVED_PACKET 79
#define POWER_SAVE_MODE_NOT_SUPPORTED                  82

#ifdef RSI_MFI_WAC_ENABLE
#define RSI_MFI_COPROCESSOR_ERROR 85
#endif

#define BT_FEATURE_BITMAP_INVALID               89
#define SAVE_CONFIG_CONCURRENT_BOTH_MODE_NOT_UP 92

#define RSI_MQTT_ERROR                      101
#define INVALID_LISTEN_INTERVAL             102
#define WLAN_RADIO_DEREGISTERED             103
#define SELECTED_BLE_FEATURES_NOT_SUPPORTED 104
#define CERT_LOAD_NOT_ALLOWED_IN_FLASH      93
#define CERT_LOAD_NOT_ALLOWED_IN_RAM        94
#define CERT_LOAD_FAILED_DUE_TO_WRONG_INX   95
#define INVALID_TOTAL_NUMBER_OF_BLE_BUFFERS 99

#endif

/**
*$Log: rsi_global.h,v $
*Revision 1.1.1.1  2012/02/28 07:14:12  kiran.gunda
*WiSe Connet initial import
*
**/