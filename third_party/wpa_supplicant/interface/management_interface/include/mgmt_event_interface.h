/*******************************************************************************
* @file  mgmt_event_if.h
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

#ifndef MGMT_EVENT_IF_H
#define MGMT_EVENT_IF_H

#include "sl_wlan_data_types.h"

int16 mgmtif_ap_stop_rsp(uint32 token);
uint32 wlan_mgmt_if_event_handler(uint8 *rx_pkt);

#endif /* MGMT_EVENT_IF_H */