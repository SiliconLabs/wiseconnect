/*******************************************************************************
* @file   mgmt_if_rx.h
* @brief  This file contains functions to receive commands from UMAC
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

#ifndef MGMT_IF_RX_H
#define MGMT_IF_RX_H

#include "sl_wlan_data_types.h"

int16 mgmtif_rx_process_mgmt_frame(uint8 *rxPkt);
int16 mgmtif_rx_process_data_frame(uint8 *rxPkt, uint32 vap_id);

#endif /* MGMT_IF_RX_H */
