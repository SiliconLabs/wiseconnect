/*******************************************************************************
* @file  sl_net_basic_profiles.c
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_net.h"
#include "sl_net_ip_types.h"
#include "sl_net_wifi_types.h"
#include "sl_net_default_values.h"
#include <string.h>

#define MAX_WIFI_CLIENT_PROFILES 2
#define MAX_WIFI_AP_PROFILES     2

static sl_net_wifi_client_profile_t wifi_client_profiles[MAX_WIFI_CLIENT_PROFILES] = { 0 };
static sl_net_wifi_ap_profile_t wifi_ap_profiles[MAX_WIFI_AP_PROFILES]             = { 0 };

sl_status_t sl_net_set_profile(sl_net_interface_t interface,
                               sl_net_profile_id_t profile_id,
                               const sl_net_profile_t *profile)
{

  switch (interface) {
#ifdef SL_WIFI_COMPONENT_INCLUDED
    case SL_NET_WIFI_CLIENT_INTERFACE:
      if (profile_id >= MAX_WIFI_CLIENT_PROFILES) {
        return SL_STATUS_INVALID_INDEX;
      }
      memcpy(&wifi_client_profiles[profile_id],
             (sl_net_wifi_client_profile_t *)profile,
             sizeof(sl_net_wifi_client_profile_t));
      return SL_STATUS_OK;

    case SL_NET_WIFI_AP_INTERFACE:
      if (profile_id >= MAX_WIFI_AP_PROFILES) {
        return SL_STATUS_INVALID_INDEX;
      }
      memcpy(&wifi_ap_profiles[profile_id], (sl_net_wifi_ap_profile_t *)profile, sizeof(sl_net_wifi_ap_profile_t));
      return SL_STATUS_OK;
#endif
    default:
      return SL_STATUS_NOT_SUPPORTED;
  }
}

sl_status_t sl_net_get_profile(sl_net_interface_t interface, sl_net_profile_id_t profile_id, sl_net_profile_t *profile)
{

  SL_WIFI_ARGS_CHECK_NULL_POINTER(profile);

  switch (interface) {
#ifdef SL_WIFI_COMPONENT_INCLUDED
    case SL_NET_WIFI_CLIENT_INTERFACE:
      if (profile_id >= MAX_WIFI_CLIENT_PROFILES) {
        return SL_STATUS_INVALID_INDEX;
      }
      memcpy(profile, &wifi_client_profiles[profile_id], sizeof(sl_net_wifi_client_profile_t));
      return SL_STATUS_OK;

    case SL_NET_WIFI_AP_INTERFACE:
      if (profile_id >= MAX_WIFI_AP_PROFILES) {
        return SL_STATUS_INVALID_INDEX;
      }
      memcpy(profile, &wifi_ap_profiles[profile_id], sizeof(sl_net_wifi_ap_profile_t));
      return SL_STATUS_OK;
#endif

    default:
      return SL_STATUS_NOT_SUPPORTED;
  }
}

sl_status_t sl_net_delete_profile(sl_net_interface_t interface, sl_net_profile_id_t profile_id)
{

  switch (interface) {
#ifdef SL_WIFI_COMPONENT_INCLUDED
    case SL_NET_WIFI_CLIENT_INTERFACE:
      if (profile_id >= MAX_WIFI_CLIENT_PROFILES) {
        return SL_STATUS_INVALID_INDEX;
      }
      memset(&wifi_client_profiles[profile_id], 0, sizeof(sl_net_wifi_client_profile_t));
      return SL_STATUS_OK;

    case SL_NET_WIFI_AP_INTERFACE:
      if (profile_id >= MAX_WIFI_AP_PROFILES) {
        return SL_STATUS_INVALID_INDEX;
      }
      memset(&wifi_ap_profiles[profile_id], 0, sizeof(sl_net_wifi_ap_profile_t));
      return SL_STATUS_OK;
#endif

    default:
      return SL_STATUS_NOT_SUPPORTED;
  }
}
