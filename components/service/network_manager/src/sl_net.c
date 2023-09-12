#include "sl_net.h"
#include "sl_net_constants.h"
#include "sl_board_configuration.h"

#ifndef NETWORK_INTERFACE_VALID
#error Need to define NETWORK_INTERFACE_VALID in sl_board_configuration.h
#endif

#if NETWORK_INTERFACE_VALID(SL_NET_WIFI_CLIENT_INTERFACE) || NETWORK_INTERFACE_VALID(SL_NET_WIFI_AP_INTERFACE)
#include "sl_wifi_device.h"
#endif

sl_status_t sl_net_init(sl_net_interface_t interface,
                        const void *configuration,
                        void *context,
                        sl_net_event_handler_t event_handler)
{
  switch (SL_NET_INTERFACE_TYPE(interface)) {
#if NETWORK_INTERFACE_VALID(SL_NET_WIFI_CLIENT_INTERFACE)
    case SL_NET_WIFI_CLIENT_INTERFACE:
      if (configuration == NULL) {
        configuration = (const void *)&sl_wifi_default_client_configuration;
      }
      return sl_net_wifi_client_init(interface, configuration, context, event_handler);
      break;
#endif
#if NETWORK_INTERFACE_VALID(SL_NET_WIFI_AP_INTERFACE)
    case SL_NET_WIFI_AP_INTERFACE:
      if (configuration == NULL) {
        configuration = (const void *)&sl_wifi_default_ap_configuration;
      }
      return sl_net_wifi_ap_init(interface, configuration, context, event_handler);
      break;
#endif
#if NETWORK_INTERFACE_VALID(SL_NET_ETHERNET_INTERFACE)
    case SL_NET_ETHERNET_INTERFACE:
      return sl_net_ethernet_init(interface, configuration, context, event_handler);
      break;
#endif
#if NETWORK_INTERFACE_VALID(SL_NET_THREAD_INTERFACE)
    case SL_NET_THREAD_INTERFACE:
      return sl_net_thread_init(interface, configuration, context, event_handler);
      break;
#endif
    default:
      return SL_STATUS_NOT_SUPPORTED;
  }
}

sl_status_t sl_net_deinit(sl_net_interface_t interface, void *context)
{
  switch (SL_NET_INTERFACE_TYPE(interface)) {
#if NETWORK_INTERFACE_VALID(SL_NET_WIFI_CLIENT_INTERFACE)
    case SL_NET_WIFI_CLIENT_INTERFACE:
      return sl_net_wifi_client_deinit(interface, context);
      break;
#endif
#if NETWORK_INTERFACE_VALID(SL_NET_WIFI_AP_INTERFACE)
    case SL_NET_WIFI_AP_INTERFACE:
      return sl_net_wifi_ap_deinit(interface, context);
      break;
#endif
#if NETWORK_INTERFACE_VALID(SL_NET_ETHERNET_INTERFACE)
    case SL_NET_ETHERNET_INTERFACE:
      return sl_net_ethernet_deinit(interface, context);
      break;
#endif
#if NETWORK_INTERFACE_VALID(SL_NET_THREAD_INTERFACE)
    case SL_NET_THREAD_INTERFACE:
      return sl_net_thread_deinit(interface, context);
      break;
#endif
    default:
      return SL_STATUS_NOT_SUPPORTED;
  }
}

sl_status_t sl_net_up(sl_net_interface_t interface, sl_net_profile_id_t profile_id)
{
  switch (SL_NET_INTERFACE_TYPE(interface)) {
#if NETWORK_INTERFACE_VALID(SL_NET_WIFI_CLIENT_INTERFACE)
    case SL_NET_WIFI_CLIENT_INTERFACE:
      return sl_net_wifi_client_up(interface, profile_id);
      break;
#endif
#if NETWORK_INTERFACE_VALID(SL_NET_WIFI_AP_INTERFACE)
    case SL_NET_WIFI_AP_INTERFACE:
      return sl_net_wifi_ap_up(interface, profile_id);
      break;
#endif
#if NETWORK_INTERFACE_VALID(SL_NET_ETHERNET_INTERFACE)
    case SL_NET_ETHERNET_INTERFACE:
      return sl_net_ethernet_up(interface, profile_id);
      break;
#endif
#if NETWORK_INTERFACE_VALID(SL_NET_THREAD_INTERFACE)
    case SL_NET_THREAD_INTERFACE:
      return sl_net_thread_up(interface, profile_id);
      break;
#endif
    default:
      return SL_STATUS_NOT_SUPPORTED;
  }
}

sl_status_t sl_net_down(sl_net_interface_t interface)
{
  switch (SL_NET_INTERFACE_TYPE(interface)) {
#if NETWORK_INTERFACE_VALID(SL_NET_WIFI_CLIENT_INTERFACE)
    case SL_NET_WIFI_CLIENT_INTERFACE:
      return sl_net_wifi_client_down(interface);
      break;
#endif
#if NETWORK_INTERFACE_VALID(SL_NET_WIFI_AP_INTERFACE)
    case SL_NET_WIFI_AP_INTERFACE:
      return sl_net_wifi_ap_down(interface);
      break;
#endif
#if NETWORK_INTERFACE_VALID(SL_NET_ETHERNET_INTERFACE)
    case SL_NET_ETHERNET_INTERFACE:
      return sl_net_ethernet_down(interface);
      break;
#endif
#if NETWORK_INTERFACE_VALID(SL_NET_THREAD_INTERFACE)
    case SL_NET_THREAD_INTERFACE:
      return sl_net_thread_down(interface);
      break;
#endif
    default:
      return SL_STATUS_NOT_SUPPORTED;
  }
}
