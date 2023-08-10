#include "sl_net.h"
#include "sl_net_ip_types.h"
#include "sl_net_wifi_types.h"
#include "nvm3.h"
#include <string.h>

#define SL_NET_WIFI_NVM3_BASE_KEY   0x1000
#define SL_NET_THREAD_NVM3_BASE_KEY 0x2000

#define SL_NET_MAXIMUM_NVM_PROFILES_PER_INTERFACE 32

#ifndef SL_NET_WIFI_CLIENT_NVM3_KEY
#define SL_NET_WIFI_CLIENT_NVM3_KEY (SL_NET_WIFI_NVM3_BASE_KEY)
#endif

#ifndef SL_NET_WIFI_AP_NVM3_KEY
#define SL_NET_WIFI_AP_NVM3_KEY (SL_NET_WIFI_NVM3_BASE_KEY + SL_NET_MAXIMUM_NVM_PROFILES_PER_INTERFACE)
#endif

sl_status_t sl_net_set_profile(sl_net_interface_t interface,
                               sl_net_profile_id_t profile_id,
                               const sl_net_profile_t *profile)
{
  sl_status_t status;
  if (profile_id >= SL_NET_MAXIMUM_NVM_PROFILES_PER_INTERFACE) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  switch (interface) {
#ifdef wifi_FEATURE_REQUIRED
    case SL_NET_WIFI_CLIENT_INTERFACE:
      status = nvm3_writeData(nvm3_defaultHandle,
                              SL_NET_WIFI_CLIENT_NVM3_KEY + profile_id,
                              (const void *)profile,
                              sizeof(sl_net_wifi_client_profile_t));
      break;

    case SL_NET_WIFI_AP_INTERFACE:
      status = nvm3_writeData(nvm3_defaultHandle,
                              SL_NET_WIFI_AP_NVM3_KEY + profile_id,
                              (const void *)profile,
                              sizeof(sl_net_wifi_ap_profile_t));
      break;
#endif
    default:
      return SL_STATUS_NOT_SUPPORTED;
  }
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_net_get_profile(sl_net_interface_t interface, sl_net_profile_id_t profile_id, sl_net_profile_t *profile)
{
  sl_status_t status;
  ARGS_CHECK_NULL_POINTER(profile);
  if (profile_id >= SL_NET_MAXIMUM_NVM_PROFILES_PER_INTERFACE) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  switch (interface) {
#ifdef wifi_FEATURE_REQUIRED
    case SL_NET_WIFI_CLIENT_INTERFACE:
      status = nvm3_readData(nvm3_defaultHandle,
                             SL_NET_WIFI_CLIENT_NVM3_KEY + profile_id,
                             (void *)profile,
                             sizeof(sl_net_wifi_client_profile_t));
      break;

    case SL_NET_WIFI_AP_INTERFACE:
      status = nvm3_readData(nvm3_defaultHandle,
                             SL_NET_WIFI_AP_NVM3_KEY + profile_id,
                             (void *)profile,
                             sizeof(sl_net_wifi_ap_profile_t));
      break;
#endif

    default:
      return SL_STATUS_NOT_SUPPORTED;
  }

  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_net_delete_profile(sl_net_interface_t interface, sl_net_profile_id_t profile_id)
{
  if (profile_id >= SL_NET_MAXIMUM_NVM_PROFILES_PER_INTERFACE) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  switch (interface) {
#ifdef wifi_FEATURE_REQUIRED
    case SL_NET_WIFI_CLIENT_INTERFACE:
      nvm3_deleteObject(nvm3_defaultHandle, SL_NET_WIFI_CLIENT_NVM3_KEY + profile_id);
      break;

    case SL_NET_WIFI_AP_INTERFACE:
      nvm3_deleteObject(nvm3_defaultHandle, SL_NET_WIFI_AP_NVM3_KEY + profile_id);
      break;
#endif

    default:
      return SL_STATUS_NOT_SUPPORTED;
  }
  return SL_STATUS_OK;
}
