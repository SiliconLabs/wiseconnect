#include "sl_net.h"
#include "sl_net_ip_types.h"
#include "sl_net_wifi_types.h"
#include "sl_net_default_values.h"
#include <string.h>

#define MAX_WIFI_CLIENT_PROFILES 2
#define MAX_WIFI_AP_PROFILES     2

static sl_net_wifi_client_profile_t wifi_client_profiles[MAX_WIFI_CLIENT_PROFILES] = {
  [0] = default_wifi_client_profile,
};
static sl_net_wifi_ap_profile_t wifi_ap_profiles[MAX_WIFI_AP_PROFILES] = {
  [0] = default_wifi_ap_profile,
};

sl_status_t sl_net_set_profile(sl_net_interface_t interface,
                               sl_net_profile_id_t profile_id,
                               const sl_net_profile_t *profile)
{
  sl_status_t status;

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
      break;

    case SL_NET_WIFI_AP_INTERFACE:
      if (profile_id >= MAX_WIFI_AP_PROFILES) {
        return SL_STATUS_INVALID_INDEX;
      }
      memcpy(&wifi_ap_profiles[profile_id], (sl_net_wifi_ap_profile_t *)profile, sizeof(sl_net_wifi_ap_profile_t));
      return SL_STATUS_OK;
      break;
#endif
    default:
      return SL_STATUS_NOT_SUPPORTED;
  }
  return status;
}

sl_status_t sl_net_get_profile(sl_net_interface_t interface, sl_net_profile_id_t profile_id, sl_net_profile_t *profile)
{
  sl_status_t status;
  ARGS_CHECK_NULL_POINTER(profile);

  switch (interface) {
#ifdef SL_WIFI_COMPONENT_INCLUDED
    case SL_NET_WIFI_CLIENT_INTERFACE:
      if (profile_id >= MAX_WIFI_CLIENT_PROFILES) {
        return SL_STATUS_INVALID_INDEX;
      }
      memcpy(profile, &wifi_client_profiles[profile_id], sizeof(sl_net_wifi_client_profile_t));
      return SL_STATUS_OK;
      break;

    case SL_NET_WIFI_AP_INTERFACE:
      if (profile_id >= MAX_WIFI_AP_PROFILES) {
        return SL_STATUS_INVALID_INDEX;
      }
      memcpy(profile, &wifi_ap_profiles[profile_id], sizeof(sl_net_wifi_ap_profile_t));
      return SL_STATUS_OK;
      break;
#endif

    default:
      return SL_STATUS_NOT_SUPPORTED;
  }

  return status;
}

sl_status_t sl_net_delete_profile(sl_net_interface_t interface, sl_net_profile_id_t profile_id)
{
  sl_status_t status;

  switch (interface) {
#ifdef SL_WIFI_COMPONENT_INCLUDED
    case SL_NET_WIFI_CLIENT_INTERFACE:
      if (profile_id >= MAX_WIFI_CLIENT_PROFILES) {
        return SL_STATUS_INVALID_INDEX;
      }
      memset(&wifi_client_profiles[profile_id], 0, sizeof(sl_net_wifi_client_profile_t));
      return SL_STATUS_OK;
      break;

    case SL_NET_WIFI_AP_INTERFACE:
      if (profile_id >= MAX_WIFI_AP_PROFILES) {
        return SL_STATUS_INVALID_INDEX;
      }
      memset(&wifi_ap_profiles[profile_id], 0, sizeof(sl_net_wifi_ap_profile_t));
      return SL_STATUS_OK;
      break;
#endif

    default:
      return SL_STATUS_NOT_SUPPORTED;
  }
  return status;
}
