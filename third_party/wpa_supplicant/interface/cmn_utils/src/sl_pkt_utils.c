/*******************************************************************************
* @file   sl_pkt_utils.c
* @brief  This file contains definitions of packet util wrapper functions
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
#include "sl_pkt_utils.h"
#include "mgmt_platform_if.h"
#include "sl_pkt_desc_utils.h"
#include "mgmt_if_core.h"
#include "sl_wlan_data_types.h"

/*******************************************************
 * @ Description: This function allocates the TX packet and memset the values
 * @ Input: uint16 pkt_type : type of the TX packet (SL_WLAN_MGMT_Q - 0x4 / SL_WLAN_DATA_Q - 0x5)
 *          uint8 alloc_type : priority level (NHCP_HIGH_PRIORITY / NHCP_LOW_PRIORITY)
 *
 * @ Called from : Host MAC Interface
 * @ Output : pointer to txPkt
 */

sli_nhcp_tx_pkt_t *sl_txpkt_alloc_and_initiate(uint16 pkt_type, uint8 alloc_type)
{
  sli_nhcp_tx_pkt_t *txPkt = NULL;

  //!allocating chunk of 2k bytes
  txPkt = (sli_nhcp_tx_pkt_t *)sli_nhcp_allocate_pkt(NHCP_PKT_TYPE_TX, alloc_type);

  if (txPkt == NULL)
    return NULL;

  //!memset upto descriptor
  sl_memzero((uint8 *)txPkt, (SL_TX_PKT_STRUCT_SIZE + HOST_DESC_LENGTH));

  //!scatter_0 buffer address copies host desc address
  SL_PKT_SET_TXPKT_SCATTER_BUFF_ADDR(txPkt, 0, &(txPkt->host_desc));

  if (pkt_type == SL_WLAN_MGMT_Q) {
    SL_PKT_TX_HDESC_SET_DW0_QNUM(txPkt, SL_WLAN_MGMT_Q);

    if (mgmt_if_adapter.band == 1) {
      SL_PKT_TX_HDESC_SET_DW1_B2(txPkt, (1 << 0)); //setting rate info
      SL_PKT_TX_HDESC_SET_DW2_B0(txPkt, 0x8b);
    } else {
      SL_PKT_TX_HDESC_SET_DW1_B2(txPkt, (1 << 0)); //setting rate info
      SL_PKT_TX_HDESC_SET_DW2_B0(txPkt, 0x0);
    }
    /* seting the 5Ghz rate for dual band*/
    if (mgmt_if_adapter.dual_band && mgmt_if_adapter.current_freq_band == SL_BAND_5GHZ) {
      SL_PKT_TX_HDESC_SET_DW1_B2(txPkt, (1 << 0)); //setting rate info
      SL_PKT_TX_HDESC_SET_DW2_B0(txPkt, 0x8b);
    }
  } else {
    SL_PKT_TX_HDESC_SET_DW0_QNUM(txPkt, SL_WLAN_DATA_Q);
  }

  return txPkt;
}

/*******************************************************
 * @ Description: This function prepares the TX packet
 * @ Input: UINT8 *payload            Buffer used for txPkt initialization
 *          uint16_t length             Length of the Buffer
 *          uint8 *offset_buf         Buffer containing data to be copied to scatter buffer in txPkt
 *          uint16_t offset_length      Determines offset to be left after host_desc in txPkt, for data to be filled in
 *          uint16_t type               Determines whether txPkt is created from Management or Data buffer pool
 *          UINT8 allocation_level    Required for buffer allocation in txPkt initialization
 *
 * @ Called from : Host MAC Interface
 * @ Output : pointer to txPkt
 */
sli_nhcp_tx_pkt_t *sl_mgmt_prepare_tx_pkt(uint8 *payload,
                                          uint16 length,
                                          uint8 *offset_buf,
                                          uint16 offset_length,
                                          uint16 type,
                                          uint8 allocation_level)
{
  uint16 total_length = length + offset_length;
  uint16 buf_index    = 0;

  /* length of payload which is already copied into the scatter buffers */
  uint16 copied_length = 0;

  /* length of payload which is left to copy into the scatter buffers */
  uint16 length_to_copy = 0;
  uint8 *buf_ptr        = NULL;

  sli_nhcp_tx_pkt_t *tx_pkt = sl_txpkt_alloc_and_initiate(type, allocation_level);

  if (tx_pkt == NULL) {
    return NULL;
  }

  SL_PKT_TX_HDESC_SET_DW0_LENGTH(tx_pkt, total_length);
  SL_PKT_TX_HDESC_SET_DW0_FRAME_TYPE(tx_pkt, type);

  /* Copy Payload into scatter buffers */

  // Initial state handling for the first buffer
  buf_ptr        = ((uint8 *)SL_PKT_GET_TXPKT_HDESC_ADDR(tx_pkt) + HOST_DESC_LENGTH + offset_length);
  length_to_copy = (total_length <= FIRST_SCATTER_AVALIABLE_LENGTH_FOR_TX_PKT)
                     ? length
                     : FIRST_SCATTER_AVALIABLE_LENGTH_FOR_TX_PKT - offset_length;

  sl_memcpy(buf_ptr, payload, length_to_copy);
  SL_PKT_SET_TXPKT_SCATTER_BUFF_LEN(tx_pkt, buf_index, length_to_copy + offset_length + HOST_DESC_LENGTH);
  total_length -= (length_to_copy + offset_length);
  copied_length += length_to_copy;
  buf_index += 1;

  // Handle remaining buffers in the payload
  while (total_length > 0) {
    if (buf_index >= SLI_MGMT_IF_TX_SCATTER_NUM) {
      return NULL;
    }

    length_to_copy = (total_length <= SLI_NHCP_PKT_BLOCK_SIZE) ? total_length : SLI_NHCP_PKT_BLOCK_SIZE;
    buf_ptr        = (uint8 *)SL_PKT_GET_TXPKT_SCATTER_BUFF_ADDR(tx_pkt, buf_index);

    // Copy the data to the buffer
    sl_memcpy(buf_ptr, payload + copied_length, length_to_copy);
    SL_PKT_SET_TXPKT_SCATTER_BUFF_LEN(tx_pkt, buf_index, length_to_copy);
    total_length -= length_to_copy;
    copied_length += length_to_copy;
    buf_index += 1;
  }

  /* Copy the offset buffer data to host descriptor */
  host_desc_t *host_desc = (host_desc_t *)SL_PKT_GET_TXPKT_HDESC_ADDR(tx_pkt);

  if (offset_buf != NULL && offset_length > 0) {
    sl_memcpy((uint8 *)(host_desc + 1), offset_buf, offset_length);
  }

  return tx_pkt;
}

/*******************************************************
 * @ Description: This function prepares the RX packet
 * @ Input: type     - frame type of the packet
 *          sub_type - Station ID in host descriptor
 *          token    - holds command status
 *          len      - payload length
 *
 * @ Called from : Module Interface
 * @ Output : pointer to rxPkt
 */

uint8 *sl_alloc_rx_cmd_desc(uint32 type, uint32 sub_type, uint32 token, uint32 len)
{
  sli_nhcp_rx_pkt_t *rx_pkt = (sli_nhcp_rx_pkt_t *)sli_nhcp_allocate_pkt(NHCP_PKT_TYPE_RX, NHCP_HIGH_PRIORITY);

  if (rx_pkt == NULL) {
    return NULL;
  }

  //! Preparing payload links
  SL_PKT_SET_RXPKT_SCATTER_BUFF_ADDR(rx_pkt, 0, (uint8_t *)rx_pkt + SL_RX_PKT_STRUCT_SIZE);
  SL_PKT_SET_RXPKT_HOST_DESC_OFFSET(rx_pkt,
                                    (uint8_t *)SL_PKT_GET_RXPKT_SCATTER_BUFF_ADDR(rx_pkt, 0) - (uint8_t *)rx_pkt);
  SL_PKT_SET_RXPKT_SCATTER_BUFF_LEN(rx_pkt, 0, SL_PKT_GET_RXPKT_HOST_DESC_OFFSET(rx_pkt) + HOST_DESC_LENGTH + len);
  SL_PKT_SET_RXPKT_HOST_XFER_LEN(rx_pkt, SL_PKT_GET_RXPKT_HOST_DESC_OFFSET(rx_pkt) + HOST_DESC_LENGTH + len);
  SL_PKT_SET_RXPKT_SCATTER_COUNT(rx_pkt, 1);

  /* Preparing payload */
  SL_PKT_RX_HDESC_SET_DW0_LENGTH(rx_pkt, len);
  SL_PKT_RX_HDESC_SET_DW0_QNUM(rx_pkt, SL_WLAN_MGMT_Q);
  SL_PKT_RX_HDESC_SET_DW0_FRAME_TYPE(rx_pkt, type);

  SL_PKT_RX_HDESC_SET_DW3_TOKEN(rx_pkt, token);
  SL_PKT_RX_HDESC_SET_DW3_STA_ID(rx_pkt, sub_type);

  return (uint8 *)rx_pkt;
}

/*******************************************************
 * @ Description: This function reconstructs a packet that was 
 *                split into multiple scattered memory buffers into one contiguous buffer.
 * @ Input:  uint8 *rxPkt
 *
 * @ Output :  uint8 *ret_buf     contigous buffer having scatter data
 */

uint8 *sli_form_linear_buffer(uint8 *rxPkt)
{
  uint16 ii            = 0;
  uint8 *buf           = NULL;
  uint8 *ret_buf       = NULL;
  uint32 scatter_count = 0;

  buf = sl_malloc(SL_PKT_RX_HDESC_GET_DW0_LENGTH(rxPkt));
  if (!buf)
    return NULL;

  ret_buf       = buf;
  scatter_count = SL_PKT_GET_RXPKT_SCATTER_COUNT(rxPkt);
  for (ii = 0; ((ii < scatter_count) && (ii < SLI_MGMT_IF_RX_SCATTER_NUM)); ii++) {
    if (ii == 0) {
      sl_memcpy((void *)buf,
                (void *)(((uint8 *)SL_PKT_GET_RXPKT_HDESC_ADDR(rxPkt))
                         + (HOST_DESC_LENGTH + SL_PKT_RX_HDESC_GET_DW1_EXT_DESC_SIZE(rxPkt))),
                (uint32)(SL_PKT_RX_HDESC_GET_DW0_LENGTH(rxPkt)));
      buf += (SL_PKT_RX_HDESC_GET_DW0_LENGTH(rxPkt));
    } else {
      sl_memcpy((void *)buf,
                (void *)SL_PKT_GET_RXPKT_SCATTER_BUFF_ADDR(rxPkt, ii),
                (uint32)SL_PKT_GET_RXPKT_SCATTER_BUFF_LEN(rxPkt, ii));
      buf += SL_PKT_GET_RXPKT_SCATTER_BUFF_LEN(rxPkt, ii);
    }
  }
  return ret_buf;
}

/****************** Packet Utils ******************/
void sl_free_tx_pkt(uint8 *txPkt)
{
  sli_nhcp_free_pkt((void *)txPkt, NHCP_PKT_TYPE_TX);
}

void sl_free_rx_pkt(uint8 *rxPkt)
{
  sli_nhcp_free_pkt((void *)rxPkt, NHCP_PKT_TYPE_RX);
}
