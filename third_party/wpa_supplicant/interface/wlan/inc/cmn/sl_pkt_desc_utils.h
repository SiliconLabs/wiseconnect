/*******************************************************************************
* @file  sl_pkt_desc_utils.h
* @brief
*******************************************************************************
* # License
* <b>Copyright 2020-2024 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_PKT_DESC_UTILS_H
#define SL_PKT_DESC_UTILS_H

#include "sl_wlan_data_types.h"
#include "common_platform_utils.h"
#include "sli_nhcp_pkt_struct.h"
#include "sli_nhcp_memory_mgmt.h"

/* ENCRYPTION_ENABLE */
#define PKT_ENCRYPTION_ENABLE 15

#define HOST_DESC_LENGTH     sizeof(host_desc_t)
#define EXTENDED_DESC_LENGTH sizeof(extended_desc_t)

#define SL_TX_PKT_STRUCT_SIZE                     sizeof(sli_nhcp_tx_pkt_t)
#define SL_RX_PKT_STRUCT_SIZE                     sizeof(sli_nhcp_rx_pkt_t)
#define FIRST_SCATTER_AVALIABLE_LENGTH_FOR_TX_PKT (SLI_NHCP_PKT_BLOCK_SIZE - SL_TX_PKT_STRUCT_SIZE)

#define SL_CONT_TRANSFER BIT(11)

/* XMIT_BUF_IND */
#define XMIT_BUF_IND 0

/*PKT_CHUNK_SIZE*/
#define SL_PKT_CHUNK_SIZE 2048

#define IS_FREE_RX_PKT(p) (((host_desc_t *)p)->dword3.queue_id == 1)

/* dword0 ... frame type */
#define PKT_TYPE_DATA BIT(1)

/* dword0 ... misc_flags */

#define SL_CONFIRM_REQUIRED_TO_HOST BIT(2)
#define SL_REQUIRE_TSF_SYNC_CONFIRM BIT(6)

/* dword1 ... Byte 1 */
#define ANT_SELECTION  1
#define ANT_TYPE_FRAME 2

/* dword1 ... Byte 2-3 */
#define MAC_BBP_INFO BIT(0)
#define NO_ACK_IND   BIT(9)
#define BEACON_FRAME BIT(11)
#define DTIM_BEACON  BIT(10) | BIT(11)
#define INSERT_TSF   BIT(15)

#if ((defined ENABLE_KRACK_PATCH) || (defined KRACK_PN_ENABLE))
#define IS_PN_VALID(desc) (((host_desc_t *)desc)->dword1.byte3 & BIT(1))
#endif

/* dword1 ... Byte 2 */
enum sl_timeout_type {
  SL_HOST_DESC_DEFAULT_TIMEOUT,
  SL_HOST_DESC_SCAN_TIMEOUT,
  SL_HOST_DESC_RADAR_SCAN_TIMEOUT,
  SL_HOST_DESC_JOIN_TIMEOUT,
  SL_HOST_DESC_AUTH_TIMEOUT,
  SL_HOST_DESC_ASSOC_TIMEOUT,
  SL_HOST_DESC_DL_DATA_TIMEOUT,
  SL_HOST_DESC_DEEP_SLEEP_TIMEOUT,
  SL_HOST_DESC_DOT11_SLEEP_TIMEOUT,
  SL_HOST_DESC_BGSCAN_TIMEOUT,
  SL_HOST_DESC_HOME_CHANNEL_TIMEOUT,
  SL_HOST_DESC_NUM_TYPES,
  SL_HOST_DESC_STATS_RESP_TIMEOUT,
  SL_HOST_DESC_UAPSD_BG_TIMEOUT,
  SL_HOST_DESC_PKT_SEND_TIMEOUT,
  SL_HOST_DESC_ACS_TIMEOUT
};

/* dword2 ... Byte 2 */
#define WITHOUT_ONBOARD_ANT 0
#define WITH_ONBOARD_ANT    1

/* dword2 ... Byte 3 */
#define BAND_WIDTH_20 0

/* dword3 */
#define CARD_READY_BIT_AUTO_JOIN_FAILED BIT(1)
#define CARD_READY_BIT_AUTO_JOINING     BIT(2)
#define CARD_READY_BIT_AUTO_JOIN_DONE   BIT(3)

typedef struct extended_frame_desc_s {
  uint8 confirm_frame_type;
  uint8 retry_cnt;
  uint16 reserved;
} extended_desc_t;

typedef struct {
  uint32 time_stamp_ls;
  uint32 time_stamp_ms;
} time_stamp_t;

typedef struct {
  uint16 rssi;
  uint16 rate;
  time_stamp_t time_stamp;
  uint32 status_word;
  uint8 ch_num;
  uint8 tx_pkt_power;
  uint16 tx_rate_inx;
} rx_onair_extended_desc_t;

/* TX PACKET MACROS */

// TX Packet Descriptor Address
#define SL_PKT_GET_TXPKT_HDESC_ADDR(tx_pkt) (((sli_nhcp_tx_pkt_t *)tx_pkt)->scatter_info[0].addr)

// TX Packet scatter buffer address
#define SL_PKT_GET_TXPKT_SCATTER_BUFF_ADDR(tx_pkt, x) (((sli_nhcp_tx_pkt_t *)tx_pkt)->scatter_info[x].addr)
#define SL_PKT_SET_TXPKT_SCATTER_BUFF_ADDR(tx_pkt, x, value) \
  (((sli_nhcp_tx_pkt_t *)tx_pkt)->scatter_info[x].addr = (uint32_t)value)

// TX Packet scatter buffer length
#define SL_PKT_GET_TXPKT_SCATTER_BUFF_LEN(tx_pkt, x)        (((sli_nhcp_tx_pkt_t *)tx_pkt)->scatter_info[x].len)
#define SL_PKT_SET_TXPKT_SCATTER_BUFF_LEN(tx_pkt, x, value) (((sli_nhcp_tx_pkt_t *)tx_pkt)->scatter_info[x].len = value)

// TX Packet next ptr
#define SL_PKT_GET_TXPKT_NEXT_PTR(tx_pkt)        (((sli_nhcp_tx_pkt_t *)tx_pkt)->next)
#define SL_PKT_SET_TXPKT_NEXT_PTR(tx_pkt, value) (((sli_nhcp_tx_pkt_t *)tx_pkt)->next = (sli_nhcp_tx_pkt_t *)value)

// TX Packet Payload Address
#define SL_PKT_GET_TXPKT_PAYLOAD_ADDR(tx_pkt) ((uint8 *)SL_PKT_GET_TXPKT_HDESC_ADDR(tx_pkt) + HOST_DESC_LENGTH)

// TX Packet Payload Address With Extended Descriptor
#define SL_PKT_GET_TXPKT_PAYLOAD_ADDR_WITH_EXTDESC(tx_pkt) \
  (SL_PKT_GET_TXPKT_PAYLOAD_ADDR(tx_pkt) + SL_PKT_TX_HDESC_GET_DW1_EXT_DESC_SIZE(tx_pkt))

// Get TX Host Descriptor members
#define SL_PKT_TX_HDESC_GET_DW0_LENGTH(addr)     (((host_desc_t *)SL_PKT_GET_TXPKT_HDESC_ADDR(addr))->dword0.length)
#define SL_PKT_TX_HDESC_GET_DW0_QNUM(addr)       (((host_desc_t *)SL_PKT_GET_TXPKT_HDESC_ADDR(addr))->dword0.queue_num)
#define SL_PKT_TX_HDESC_GET_DW0_FRAME_TYPE(addr) (((host_desc_t *)SL_PKT_GET_TXPKT_HDESC_ADDR(addr))->dword0.frame_type)
#define SL_PKT_TX_HDESC_GET_DW0_MISC_FLAGS(addr) (((host_desc_t *)SL_PKT_GET_TXPKT_HDESC_ADDR(addr))->dword0.misc_flags)
#define SL_PKT_TX_HDESC_GET_DW1_EXT_DESC_SIZE(addr) \
  (((host_desc_t *)SL_PKT_GET_TXPKT_HDESC_ADDR(addr))->dword1.extended_desc_size)
#define SL_PKT_TX_HDESC_GET_DW1_B2(addr)       (((host_desc_t *)SL_PKT_GET_TXPKT_HDESC_ADDR(addr))->dword1.byte2)
#define SL_PKT_TX_HDESC_GET_DW1_B3(addr)       (((host_desc_t *)SL_PKT_GET_TXPKT_HDESC_ADDR(addr))->dword1.byte3)
#define SL_PKT_TX_HDESC_GET_DW2(addr)          (((host_desc_t *)SL_PKT_GET_TXPKT_HDESC_ADDR(addr))->dword2)
#define SL_PKT_TX_HDESC_GET_DW2_B0(addr)       (((host_desc_t *)SL_PKT_GET_TXPKT_HDESC_ADDR(addr))->dword2.byte0)
#define SL_PKT_TX_HDESC_GET_DW2_B1(addr)       (((host_desc_t *)SL_PKT_GET_TXPKT_HDESC_ADDR(addr))->dword2.byte1)
#define SL_PKT_TX_HDESC_GET_DW2_B2(addr)       (((host_desc_t *)SL_PKT_GET_TXPKT_HDESC_ADDR(addr))->dword2.byte2)
#define SL_PKT_TX_HDESC_GET_DW3_TOKEN(addr)    (((host_desc_t *)SL_PKT_GET_TXPKT_HDESC_ADDR(addr))->dword3.token)
#define SL_PKT_TX_HDESC_GET_DW3_QUEUE_ID(addr) (((host_desc_t *)SL_PKT_GET_TXPKT_HDESC_ADDR(addr))->dword3.queue_id)

// Set TX Host Descriptor members
#define SL_PKT_TX_HDESC_SET_DW0_LENGTH(addr, value) \
  (((host_desc_t *)SL_PKT_GET_TXPKT_HDESC_ADDR(addr))->dword0.length = value)
#define SL_PKT_TX_HDESC_SET_DW0_QNUM(addr, value) \
  (((host_desc_t *)SL_PKT_GET_TXPKT_HDESC_ADDR(addr))->dword0.queue_num = value)
#define SL_PKT_TX_HDESC_SET_DW0_FRAME_TYPE(addr, value) \
  (((host_desc_t *)SL_PKT_GET_TXPKT_HDESC_ADDR(addr))->dword0.frame_type = value)
#define SL_PKT_TX_HDESC_SET_DW0_MISC_FLAGS(addr, value) \
  (((host_desc_t *)SL_PKT_GET_TXPKT_HDESC_ADDR(addr))->dword0.misc_flags = value)
#define SL_PKT_TX_HDESC_SET_DW1_EXT_DESC_SIZE(addr, value) \
  (((host_desc_t *)SL_PKT_GET_TXPKT_HDESC_ADDR(addr))->dword1.extended_desc_size = value)
#define SL_PKT_TX_HDESC_SET_DW1_B1(addr, value) \
  (((host_desc_t *)SL_PKT_GET_TXPKT_HDESC_ADDR(addr))->dword1.byte1 = value)
#define SL_PKT_TX_HDESC_SET_DW1_B2(addr, value) \
  (((host_desc_t *)SL_PKT_GET_TXPKT_HDESC_ADDR(addr))->dword1.byte2 = value)
#define SL_PKT_TX_HDESC_SET_DW1_B3(addr, value) \
  (((host_desc_t *)SL_PKT_GET_TXPKT_HDESC_ADDR(addr))->dword1.byte3 = value)
#define SL_PKT_TX_HDESC_SET_DW2_B0(addr, value) \
  (((host_desc_t *)SL_PKT_GET_TXPKT_HDESC_ADDR(addr))->dword2.byte0 = value)
#define SL_PKT_TX_HDESC_SET_DW2_B1(addr, value) \
  (((host_desc_t *)SL_PKT_GET_TXPKT_HDESC_ADDR(addr))->dword2.byte1 = value)
#define SL_PKT_TX_HDESC_SET_DW2_B2(addr, value) \
  (((host_desc_t *)SL_PKT_GET_TXPKT_HDESC_ADDR(addr))->dword2.byte2 = value)
#define SL_PKT_TX_HDESC_SET_DW2_B3(addr, value) \
  (((host_desc_t *)SL_PKT_GET_TXPKT_HDESC_ADDR(addr))->dword2.byte3 = value)
#define SL_PKT_TX_HDESC_SET_DW3_TOKEN(addr, value) \
  (((host_desc_t *)SL_PKT_GET_TXPKT_HDESC_ADDR(addr))->dword3.token = value)
#define SL_PKT_TX_HDESC_SET_DW3_QUEUE_ID(addr, value) \
  (((host_desc_t *)SL_PKT_GET_TXPKT_HDESC_ADDR(addr))->dword3.queue_id = value)
#define SL_PKT_TX_HDESC_SET_DW3_TID(addr, value) \
  (((host_desc_t *)SL_PKT_GET_TXPKT_HDESC_ADDR(addr))->dword3.tid = value)
#define SL_PKT_TX_HDESC_SET_DW3_STA_ID(addr, value) \
  (((host_desc_t *)SL_PKT_GET_TXPKT_HDESC_ADDR(addr))->dword3.sta_id = value)

/* RX PKT MACROS */

// RX Packet Descriptor Address
#define SL_PKT_GET_RXPKT_HDESC_ADDR(rx_pkt) \
  ((uint8 *)(rx_pkt) + ((sli_nhcp_rx_pkt_t *)rx_pkt)->hostss_metadata.host_desc_offset)

// RX Packet scatter buffer address
#define SL_PKT_GET_RXPKT_SCATTER_BUFF_ADDR(rx_pkt, x) (((sli_nhcp_rx_pkt_t *)rx_pkt)->scatter_info[x].addr)
#define SL_PKT_SET_RXPKT_SCATTER_BUFF_ADDR(rx_pkt, x, value) \
  (((sli_nhcp_rx_pkt_t *)rx_pkt)->scatter_info[x].addr = (uint32_t)value)

// RX Packet host_xfer_length
#define SL_PKT_GET_RXPKT_HOST_XFER_LEN(rx_pkt) (((sli_nhcp_rx_pkt_t *)rx_pkt)->hostss_metadata.host_transfer_length)
#define SL_PKT_SET_RXPKT_HOST_XFER_LEN(rx_pkt, value) \
  (((sli_nhcp_rx_pkt_t *)rx_pkt)->hostss_metadata.host_transfer_length = value)

// RX Packet host_xfer_length
#define SL_PKT_GET_RXPKT_HOST_DESC_OFFSET(rx_pkt) (((sli_nhcp_rx_pkt_t *)rx_pkt)->hostss_metadata.host_desc_offset)
#define SL_PKT_SET_RXPKT_HOST_DESC_OFFSET(rx_pkt, value) \
  (((sli_nhcp_rx_pkt_t *)rx_pkt)->hostss_metadata.host_desc_offset = value)

// RX Packet scatter buffer length macros
#define SL_PKT_GET_RXPKT_SCATTER_BUFF_LEN(rx_pkt, x)        (((sli_nhcp_rx_pkt_t *)rx_pkt)->scatter_info[x].len)
#define SL_PKT_SET_RXPKT_SCATTER_BUFF_LEN(rx_pkt, x, value) (((sli_nhcp_rx_pkt_t *)rx_pkt)->scatter_info[x].len = value)

// RX Packet scatter count
#define SL_PKT_GET_RXPKT_SCATTER_COUNT(rx_pkt)        (((sli_nhcp_rx_pkt_t *)rx_pkt)->scatter_count)
#define SL_PKT_SET_RXPKT_SCATTER_COUNT(rx_pkt, value) (((sli_nhcp_rx_pkt_t *)rx_pkt)->scatter_count = value)

// RX Packet Payload Address
#define SL_PKT_GET_RXPKT_PAYLOAD_ADDR(rx_pkt) (SL_PKT_GET_RXPKT_HDESC_ADDR(rx_pkt) + HOST_DESC_LENGTH)
// RX Packet Payload Address With Extended Descriptor
#define SL_PKT_GET_RXPKT_PAYLOAD_ADDR_WITH_EXTDESC(rx_pkt) \
  (SL_PKT_GET_RXPKT_PAYLOAD_ADDR(rx_pkt) + SL_PKT_RX_HDESC_GET_DW1_EXT_DESC_SIZE(rx_pkt))

// RX Packet next ptr
#define SL_PKT_SET_RXPKT_NEXT_PTR(rx_pkt, value) (((sli_nhcp_rx_pkt_t *)rx_pkt)->next = (sli_nhcp_rx_pkt_t *)value)

// RX Packet scatter buffer macros
#define SL_PKT_GET_RXPKT_SCATTER_BUFF(rx_pkt, x) (((sli_nhcp_rx_pkt_t *)rx_pkt)->scatter_info[x])

// Get RX Host Descriptor members
#define SL_PKT_RX_HDESC_GET_DW0_LENGTH(addr)     (((host_desc_t *)SL_PKT_GET_RXPKT_HDESC_ADDR(addr))->dword0.length)
#define SL_PKT_RX_HDESC_GET_DW0_QNUM(addr)       (((host_desc_t *)SL_PKT_GET_RXPKT_HDESC_ADDR(addr))->dword0.queue_num)
#define SL_PKT_RX_HDESC_GET_DW0_FRAME_TYPE(addr) (((host_desc_t *)SL_PKT_GET_RXPKT_HDESC_ADDR(addr))->dword0.frame_type)
#define SL_PKT_RX_HDESC_GET_DW1_EXT_DESC_SIZE(addr) \
  (((host_desc_t *)SL_PKT_GET_RXPKT_HDESC_ADDR(addr))->dword1.extended_desc_size)
#define SL_PKT_RX_HDESC_GET_DW1_B2(addr)     (((host_desc_t *)SL_PKT_GET_RXPKT_HDESC_ADDR(addr))->dword1.byte2)
#define SL_PKT_RX_HDESC_GET_DW1_B3(addr)     (((host_desc_t *)SL_PKT_GET_RXPKT_HDESC_ADDR(addr))->dword1.byte3)
#define SL_PKT_RX_HDESC_GET_DW2(addr)        (((host_desc_t *)SL_PKT_GET_RXPKT_HDESC_ADDR(addr))->dword2)
#define SL_PKT_RX_HDESC_GET_DW3_TOKEN(addr)  (((host_desc_t *)SL_PKT_GET_RXPKT_HDESC_ADDR(addr))->dword3.token)
#define SL_PKT_RX_HDESC_GET_DW3_STA_ID(addr) (((host_desc_t *)SL_PKT_GET_RXPKT_HDESC_ADDR(addr))->dword3.sta_id)

// Set RX Host Descriptor members
#define SL_PKT_RX_HDESC_SET_DW0_LENGTH(addr, value) \
  (((host_desc_t *)SL_PKT_GET_RXPKT_HDESC_ADDR(addr))->dword0.length = value)
#define SL_PKT_RX_HDESC_SET_DW0_QNUM(addr, value) \
  (((host_desc_t *)SL_PKT_GET_RXPKT_HDESC_ADDR(addr))->dword0.queue_num = value)
#define SL_PKT_RX_HDESC_SET_DW0_FRAME_TYPE(addr, value) \
  (((host_desc_t *)SL_PKT_GET_RXPKT_HDESC_ADDR(addr))->dword0.frame_type = value)
#define SL_PKT_RX_HDESC_SET_DW1_EXT_DESC_SIZE(addr, value) \
  (((host_desc_t *)SL_PKT_GET_RXPKT_HDESC_ADDR(addr))->dword1.extended_desc_size = value)
#define SL_PKT_RX_HDESC_SET_DW1_B3(addr, value) \
  (((host_desc_t *)SL_PKT_GET_RXPKT_HDESC_ADDR(addr))->dword1.byte3 = value)
#define SL_PKT_RX_HDESC_SET_DW2_B0(addr, value) \
  (((host_desc_t *)SL_PKT_GET_RXPKT_HDESC_ADDR(addr))->dword2.byte0 = value)
#define SL_PKT_RX_HDESC_SET_DW2_B1(addr, value) \
  (((host_desc_t *)SL_PKT_GET_RXPKT_HDESC_ADDR(addr))->dword2.byte1 = value)
#define SL_PKT_RX_HDESC_SET_DW2_B2(addr, value) \
  (((host_desc_t *)SL_PKT_GET_RXPKT_HDESC_ADDR(addr))->dword2.byte2 = value)
#define SL_PKT_RX_HDESC_SET_DW2_B3(addr, value) \
  (((host_desc_t *)SL_PKT_GET_RXPKT_HDESC_ADDR(addr))->dword2.byte3 = value)
#define SL_PKT_RX_HDESC_SET_DW3_TOKEN(addr, value) \
  (((host_desc_t *)SL_PKT_GET_RXPKT_HDESC_ADDR(addr))->dword3.token = value)
#define SL_PKT_RX_HDESC_SET_DW3_QUEUE_ID(addr, value) \
  (((host_desc_t *)SL_PKT_GET_RXPKT_HDESC_ADDR(addr))->dword3.queue_id = value)
#define SL_PKT_RX_HDESC_SET_DW3_STA_ID(addr, value) \
  (((host_desc_t *)SL_PKT_GET_RXPKT_HDESC_ADDR(addr))->dword3.sta_id = value)

#endif /* SL_PKT_DESC_UTILS_H */
