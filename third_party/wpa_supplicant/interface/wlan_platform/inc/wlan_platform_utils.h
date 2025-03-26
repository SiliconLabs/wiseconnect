/*******************************************************************************
* @file  wlan_platform_utils.h
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

#ifndef WLAN_PLATFORM_H
#define WLAN_PLATFORM_H

#include "sl_wlan_data_types.h"
#include "sl_mgmt_if_thread.h"

sl_wlan_mgmt_if_rx_pkt_t *sl_prepare_rx_pkt_from_linear_buffer(uint16 type,
                                                               uint16 len,
                                                               uint16 status,
                                                               uint8 *data,
                                                               uint8 *prepend_data,
                                                               uint8 prepend_len);
void sl_mgmt_indicate_to_host(uint16 type, uint16 len, uint16 status, uint8 *data);
sl_status_t sli_send_host_rx_pkt(uint8_t *rxPkt);
#endif /* WLAN_PLATFORM_H */