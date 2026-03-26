/*******************************************************************************
* @file   host_mac_if_tx.h
* @brief  This file contains function declarations that handle packet
          preparation and send it to UMAC
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

#ifndef HOST_MAC_IF_TX_H
#define HOST_MAC_IF_TX_H

#include "sl_pkt_desc_utils.h"

/* Definition of flag */
#define DISCONNECT_FROM_SUPP_IN_WPS BIT(0)
#define DISCONNECT_FROM_HOST        BIT(1)

int32 send_msg_to_nwp(uint8 *data, uint16 data_len, uint32 data_type);
int32 send_msg_to_nwp_2(uint8 *buf,
                        uint16 buf_len,
                        uint8 *offset_buf,
                        uint16 offset_len,
                        uint32 frame_type,
                        uint16 type,
                        uint8 flag);

#endif /* HOST_MAC_IF_TX_H */