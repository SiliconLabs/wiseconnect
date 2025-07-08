/*******************************************************************************
* @file host_mac_if_tx.c
* @brief This file contains function definitions that handle packet preparation
*        and send it to UMAC
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
#include "host_mac_if_tx.h"
#include "mgmt_platform_if.h"
#include "internal_mgmt_command_if.h"

#include "sli_nhcp_pkt_struct.h"
#include "sli_nhcp_driver_thread.h"
#include "sl_pkt_desc_utils.h"

/*******************************************************
 * @ Description: This function shall prepare the packet and send it to UMAC
 * @ Input: uint8 *data           Data to be sent via txPkt
 *          uint16 data_len       Length of the data that is going to be sent via txPkt
 *          uint32 data_type      Type of frame that the data belongs to
 * @ Called from : Host MAC Interface
 * @ Output : Command status : -1/0    0: The packet has been sent to UMAC
 *                                     Non-Zero: Failure
 */
int32 send_msg_to_nwp(uint8 *data, uint16 data_len, uint32 data_type)
{
  sl_status_t status = SL_STATUS_OK;
  uint8 *txPkt       = NULL;
  txPkt              = (uint8 *)sl_txpkt_alloc_and_initiate(SL_WLAN_MGMT_Q, NHCP_HIGH_PRIORITY);

  if (txPkt == NULL) {
    SLI_WLAN_MGMT_ASSERT(UMAC_ASSERT_TX_PKT_ALLOC_FAIL_IN_HOST_MAC_IF_TX);
    return -1;
  }

  if ((data != NULL) && (data_len > 0))
    sl_memcpy(((uint8 *)(SL_PKT_GET_TXPKT_HDESC_ADDR(txPkt) + HOST_DESC_LENGTH)), data, data_len);

  SL_PKT_TX_HDESC_SET_DW0_LENGTH(txPkt, data_len);
  SL_PKT_TX_HDESC_SET_DW0_QNUM(txPkt, SL_WLAN_MGMT_Q);
  SL_PKT_SET_TXPKT_SCATTER_BUFF_LEN(txPkt, 0, data_len + HOST_DESC_LENGTH);
  SL_PKT_SET_TXPKT_NEXT_PTR(txPkt, 0);
  SL_PKT_TX_HDESC_SET_DW0_FRAME_TYPE(txPkt, data_type);

  status = sli_hal_nhcp_send_pkt_without_copy((uint8_t *)(&((sli_nhcp_tx_pkt_t *)txPkt)->host_desc));
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG("\r\nFailed to send TX packet to NWP: 0x%lX\r\n", status);
    SL_MGMT_ASSERT(0);
  }

  return 0;
}

/*******************************************************
 * @ Description: This function shall prepare the packet and send it to UMAC
 * @ Input: uint8 *buf                Buffer used for txPkt initialization 
 *          uint16 buf_len            Length of the Buffer
 *          uint8 *offset_buf         Buffer containing data to be copied to scatter buffer in txPkt
 *          uint16 offset_len         Determines offset to be left after host_desc in txPkt and contains 
 *                                    the length of offset_buf
 *          uint32 frame_type         Type of frame that the data belongs to
 *          uint16 type               Determines whether txPkt is created from Management or Data buffer pool
 *          uint8 allocation_level    Required for buffer allocation in txPkt initialization
 *          uint8 flag                Used to set the value of dword2.byte1 in host descriptor
 *         
 * @ Called from : Host MAC Interface 
 * @ Output : Command status : -1/0    0: The packet has been sent to UMAC
 *                                     Non-Zero: Failure
 */
int32 send_msg_to_nwp_2(uint8 *buf,
                        uint16 buf_len,
                        uint8 *offset_buf,
                        uint16 offset_len,
                        uint32 frame_type,
                        uint16 type,
                        uint8 flag)
{
  sl_status_t status = SL_STATUS_OK;
  uint8 *txPkt       = NULL;
  txPkt              = (uint8 *)sl_mgmt_prepare_tx_pkt(buf, buf_len, offset_buf, offset_len, type, NHCP_HIGH_PRIORITY);

  if (txPkt == NULL) {
    // TODO : move it to MUST_ALLOC case
    SLI_WLAN_MGMT_ASSERT(UMAC_ASSERT_TX_PKT_ALLOC_FAIL_IN_HOST_MAC_IF_TX);
    return -1;
  }

  /* TODO : Try passing type as 0 in sl_mgmt_prepare_tx_pkt and accordingly remove below lines */
  SL_PKT_TX_HDESC_SET_DW0_LENGTH(txPkt, (buf_len + offset_len));
  SL_PKT_TX_HDESC_SET_DW0_QNUM(txPkt, SL_WLAN_MGMT_Q);
  SL_PKT_TX_HDESC_SET_DW0_FRAME_TYPE(txPkt, frame_type);

  /* TODO : Look to remove this during refactoring */
  if (frame_type == MGMT_P2P_ACTION_FRAME || frame_type == TX_DOT11_MGMT_FRAME) {
    SL_PKT_TX_HDESC_SET_DW1_B1(txPkt, MAC_HEADER_LENGTH);
  }

  if (flag & DISCONNECT_FROM_HOST)
    SL_PKT_TX_HDESC_SET_DW2_B0(txPkt, 1);

  if (flag & DISCONNECT_FROM_SUPP_IN_WPS) {
    SL_PKT_TX_HDESC_SET_DW2_B1(txPkt, 1);
  } else {
    SL_PKT_TX_HDESC_SET_DW2_B1(txPkt, 0);
  }

  status = sli_hal_nhcp_send_pkt_without_copy((uint8_t *)(&((sli_nhcp_tx_pkt_t *)txPkt)->host_desc));
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG("\r\nFailed to send TX packet to NWP: 0x%lX\r\n", status);
    SL_MGMT_ASSERT(0);
  }

  return 0;
}
