/*******************************************************************************
* @file  mgmt_if_init.c
* @brief This contains Management i/f functions related to initilizations.
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
#include "mgmt_platform_if.h"
#include "mgmt_supplicant_if.h"
#include "mgmt_if_init.h"
#include "mgmt_if_core.h"
#include "mgmt_if_rsi_wsc.h"

void wsc_set_default_params(void)
{
#ifdef RSI_GLOBAL_VARIABLES
  set_client_mode(1);
#endif /* RSI_GLOBAL_VARIABLES */
}

void wsc_init()
{
  wsc_set_default_params();
  mgmt_if_adapter.client                      = SL_TRUE;
  mgmt_if_adapter.roam_params.roam_threshold  = 5;
  mgmt_if_adapter.roam_params.roam_hysteresis = 2;
#ifdef ENABLE_DRAEGER_CUSTOMIZATION
  mgmt_if_adapter.max_rejoin_retry = MAX_REJOIN_RETRIES;
#endif
}

void set_security_protocol(void *ssid, uint8 auth_mode)
{
  switch (auth_mode) {
    case SEC_MODE_WPA:
    case SEC_MODE_WPA_ENTERPRISE:
      wpa_config_set_wrapper(ssid, "proto", "WPA", 0); /*WPA network*/
      break;
    case SEC_MODE_WPA2:
    case SEC_MODE_WPA3:
    case SEC_MODE_WPA2_ENTERPRISE:
    case SEC_MODE_WPA3_TRANSITION:
      wpa_config_set_wrapper(ssid, "proto", "RSN", 0); /*WPA2 Network*/
      break;
    case SEC_MODE_MIXED_MODE:
      wpa_config_set_wrapper(ssid, "proto", "WPA WPA2", 0); /*Mixed mode*/
      break;
  }
}

#ifndef EXCLUDE_WPS
int16 start_wps_sta(void *ctx)
{
  struct wpa_supplicant *wpa_s = ctx;
  if (supp_mgmt_if_ctx.wps_method) {
    wpas_wps_start_pin_wrapper(wpa_s, NULL, (const char *)mgmt_if_adapter.wps_pin, 0, 0);
  } else {
    wpas_wps_start_pbc_wrapper(wpa_s, NULL, 0, 0);
  }
  return 0;
}
#endif
