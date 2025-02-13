/*******************************************************************************
* @file  mgmt_event_if.c
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

#include "mgmt_event_interface.h"
#include "mgmt_platform_if.h"
#include "mgmt_if_core.h"

int16 mgmtif_ap_stop_rsp(uint32 token)
{
  if (token == 0) {
    wise_reset_ap((mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE));
  } else {
    sl_mgmt_indicate_to_host(SLI_WLAN_RSP_AP_STOP, 0, token, 0);
  }

  return WLAN_STATUS_SUCCESS;
}

uint32 wlan_mgmt_if_event_handler(uint8 *rx_pkt)
{
  if (rx_pkt == NULL) {
    return WLAN_STATUS_FAILURE;
  }

  switch (SL_PKT_RX_HDESC_GET_DW0_FRAME_TYPE(rx_pkt)) {
    case SLI_WLAN_RSP_AP_STOP: {
      mgmtif_ap_stop_rsp(SL_PKT_RX_HDESC_GET_DW3_TOKEN(rx_pkt));
    } break;

    case SLI_WLAN_RSP_BEACON_STOP: {
      sl_mgmt_indicate_to_host(SLI_WLAN_RSP_BEACON_STOP, 0, SL_PKT_RX_HDESC_GET_DW3_TOKEN(rx_pkt), 0);
    } break;
  }

  return WLAN_STATUS_SUCCESS;
}
