/*******************************************************************************
* @file  sli_p2p_fsm.c
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
#ifdef CONFIG_P2P
#include "mgmt_if_core.h"

void rsi_p2p_start_full_scan();
void rsi_p2p_start_find();

void sli_p2p_fsm(uint16 event)
{
  switch (mgmt_if_adapter.p2p_curr_state) {
    case P2P_SM_INIT: {
      if (event == P2P_EVENT_START_FULL_SCAN) {
        rsi_p2p_start_full_scan();
        mgmt_if_adapter.p2p_curr_state = P2P_SM_SCANNING;
      }
    } break;
    case P2P_SM_SCANNING: {
      if (event == P2P_EVENT_SCAN_DONE) {
        rsi_p2p_start_find();
        mgmt_if_adapter.p2p_curr_state = P2P_SM_FIND;
      }
    } break;
    default:
      break;
  }
  return;
}
#endif /*CONFIG_P2P*/