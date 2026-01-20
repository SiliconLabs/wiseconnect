/*******************************************************************************
* @file host_mac_if_rx.c
* @brief This file contains function definitions that handle packet reception
*        and from UMAC
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

#include "mgmt_if_rx.h"
#include "sl_pkt_utils.h"
#include "wlan_header.h"

void process_rx_pkt_from_nwp(uint8 *rx_pkt)
{
  uint16 host_q;

  host_q = SL_PKT_RX_HDESC_GET_DW0_QNUM(rx_pkt);
  SL_PKT_SET_RXPKT_SCATTER_BUFF_ADDR(rx_pkt,
                                     0,
                                     (uint32)((void *)rx_pkt + SL_PKT_GET_RXPKT_HOST_DESC_OFFSET(rx_pkt)
                                              + HOST_DESC_LENGTH + SL_PKT_RX_HDESC_GET_DW1_EXT_DESC_SIZE(rx_pkt)));
  SL_PKT_SET_RXPKT_SCATTER_BUFF_LEN(
    rx_pkt,
    0,
    SL_PKT_GET_RXPKT_SCATTER_BUFF_LEN(rx_pkt, 0)
      - (SL_PKT_GET_RXPKT_HOST_DESC_OFFSET(rx_pkt) + HOST_DESC_LENGTH + SL_PKT_RX_HDESC_GET_DW1_EXT_DESC_SIZE(rx_pkt)));

  if ((host_q == SL_WLAN_MGMT_Q) || (host_q == TH0_COMMON_Q)) {
    mgmtif_rx_process_mgmt_frame(rx_pkt);
  } else {
    mgmtif_rx_process_data_frame(rx_pkt, SL_PKT_RX_HDESC_GET_DW3_STA_ID(rx_pkt));
  }
}
