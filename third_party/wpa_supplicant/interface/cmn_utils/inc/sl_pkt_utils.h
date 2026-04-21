/*******************************************************************************
* @file  sl_pkt_utils.h
* @brief This file contains function declarations of packet util wrapper functions
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

#ifndef SL_PKT_UTILS_H
#define SL_PKT_UTILS_H

#include "sli_nhcp_pkt_struct.h"
#include "sl_pkt_desc_utils.h"

sli_nhcp_tx_pkt_t *sl_txpkt_alloc_and_initiate(uint16 pkt_type, uint8 alloc_type);

sli_nhcp_tx_pkt_t *sl_mgmt_prepare_tx_pkt(uint8 *payload,
                                          uint16 length,
                                          uint8 *offset_buf,
                                          uint16 offset_length,
                                          uint16 type,
                                          uint8 allocation_level);

uint8 *sl_alloc_rx_cmd_desc(uint32 type, uint32 sub_type, uint32 token, uint32 len);

uint8 *sli_form_linear_buffer(uint8_t *rxPkt);

void sl_free_tx_pkt(uint8 *txPkt);
void sl_free_rx_pkt(uint8 *rxPkt);

#endif /* SL_PKT_UTILS_H */
