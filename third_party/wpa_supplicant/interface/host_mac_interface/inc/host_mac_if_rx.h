/*******************************************************************************
* @file  host_mac_if_rx.h
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

#ifndef HOST_MAC_IF_RX_H
#define HOST_MAC_IF_RX_H

#include "sl_pkt_desc_utils.h"

void process_rx_pkt_from_nwp(uint8 *rxPkt);

#endif /* HOST_MAC_IF_RX_H */