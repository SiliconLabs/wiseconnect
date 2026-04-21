/***************************************************************************/ /**
 * @file  sl_net.c
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include "sl_net.h"
#include "sl_net_constants.h"
#include "sl_wifi_device.h"
#include "sl_net_default_values.h"
#include "sli_net_common_utility.h"
#ifdef SL_SI91X_NVM3_CONFIG_MANAGER
#include "nvm3_default.h"
#endif

#ifndef NETWORK_INTERFACE_VALID
#error Need to define NETWORK_INTERFACE_VALID in sl_board_configuration.h
#endif

#if NETWORK_INTERFACE_VALID(SL_NET_WIFI_CLIENT_1_INTERFACE) || NETWORK_INTERFACE_VALID(SL_NET_WIFI_AP_1_INTERFACE) \
  || NETWORK_INTERFACE_VALID(SL_NET_WIFI_CLIENT_2_INTERFACE) || NETWORK_INTERFACE_VALID(SL_NET_WIFI_AP_2_INTERFACE)
#include "sl_wifi_device.h"
#endif

sl_status_t sl_net_init(sl_net_interface_t interface,
                        const void *configuration,
                        void *network_context,
                        sl_net_event_handler_t event_handler)
{
  sl_status_t status = SL_STATUS_OK;
  switch (SL_NET_INTERFACE_TYPE(interface)) {
#if NETWORK_INTERFACE_VALID(SL_NET_WIFI_CLIENT_1_INTERFACE)
    case SL_NET_WIFI_CLIENT_1_INTERFACE:
      if (configuration == NULL) {
        configuration = (const void *)&sl_wifi_default_client_configuration;
      }
      status = sl_net_wifi_client_init(interface, configuration, network_context, event_handler);
      VERIFY_STATUS_AND_RETURN(status);

#ifdef SL_SI91X_NVM3_CONFIG_MANAGER
      status = nvm3_initDefault();
      VERIFY_STATUS_AND_RETURN(status);
#endif
      if (default_wifi_client_credential.data_length > 0) {
        status = sl_net_set_credential(SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID,
                                       default_wifi_client_credential.type,
                                       (const void *)default_wifi_client_credential.data,
                                       default_wifi_client_credential.data_length);
        VERIFY_STATUS_AND_RETURN(status);
      }
      status = sl_net_set_profile(SL_NET_WIFI_CLIENT_1_INTERFACE,
                                  SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID,
                                  &DEFAULT_WIFI_CLIENT_PROFILE);
      return status;
#endif
#if NETWORK_INTERFACE_VALID(SL_NET_WIFI_CLIENT_2_INTERFACE)
    case SL_NET_WIFI_CLIENT_2_INTERFACE:
      if (configuration == NULL) {
        configuration = (const void *)&sl_wifi_default_client_configuration;
      }
      status = sl_net_wifi_client_init(interface, configuration, network_context, event_handler);
      VERIFY_STATUS_AND_RETURN(status);
#ifdef SL_SI91X_NVM3_CONFIG_MANAGER
      status = nvm3_initDefault();
      VERIFY_STATUS_AND_RETURN(status);
#endif
      if (default_wifi_client_credential.data_length > 0) {
        status = sl_net_set_credential(SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID,
                                       default_wifi_client_credential.type,
                                       (const void *)default_wifi_client_credential.data,
                                       default_wifi_client_credential.data_length);
        VERIFY_STATUS_AND_RETURN(status);
      }
      status = sl_net_set_profile(SL_NET_WIFI_CLIENT_2_INTERFACE,
                                  SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID,
                                  &DEFAULT_WIFI_CLIENT_PROFILE);
      return status;
#endif
#if NETWORK_INTERFACE_VALID(SL_NET_WIFI_AP_1_INTERFACE)
    case SL_NET_WIFI_AP_1_INTERFACE:
      if (configuration == NULL) {
        configuration = (const void *)&sl_wifi_default_ap_configuration;
      }
      status = sl_net_wifi_ap_init(interface, configuration, network_context, event_handler);
      VERIFY_STATUS_AND_RETURN(status);
#ifdef SL_SI91X_NVM3_CONFIG_MANAGER
      status = nvm3_initDefault();
      VERIFY_STATUS_AND_RETURN(status);
#endif
      if (default_wifi_ap_credential.data_length > 0) {
        status = sl_net_set_credential(SL_NET_DEFAULT_WIFI_AP_CREDENTIAL_ID,
                                       default_wifi_ap_credential.type,
                                       (const void *)default_wifi_ap_credential.data,
                                       default_wifi_ap_credential.data_length);
        VERIFY_STATUS_AND_RETURN(status);
      }
      status = sl_net_set_profile(SL_NET_WIFI_AP_1_INTERFACE,
                                  SL_NET_DEFAULT_WIFI_AP_PROFILE_ID,
                                  &DEFAULT_WIFI_ACCESS_POINT_PROFILE);
      return status;
#endif
#if NETWORK_INTERFACE_VALID(SL_NET_WIFI_AP_2_INTERFACE)
    case SL_NET_WIFI_AP_2_INTERFACE:
      if (configuration == NULL) {
        configuration = (const void *)&sl_wifi_default_ap_configuration;
      }
      status = sl_net_wifi_ap_init(interface, configuration, network_context, event_handler);
      VERIFY_STATUS_AND_RETURN(status);
#ifdef SL_SI91X_NVM3_CONFIG_MANAGER
      status = nvm3_initDefault();
      VERIFY_STATUS_AND_RETURN(status);
#endif
      if (default_wifi_ap_credential.data_length > 0) {
        status = sl_net_set_credential(SL_NET_DEFAULT_WIFI_AP_CREDENTIAL_ID,
                                       default_wifi_ap_credential.type,
                                       (const void *)default_wifi_ap_credential.data,
                                       default_wifi_ap_credential.data_length);
        VERIFY_STATUS_AND_RETURN(status);
      }
      status = sl_net_set_profile(SL_NET_WIFI_AP_2_INTERFACE,
                                  SL_NET_DEFAULT_WIFI_AP_PROFILE_ID,
                                  &DEFAULT_WIFI_ACCESS_POINT_PROFILE);
      return status;
#endif
    default:
      return SL_STATUS_NOT_SUPPORTED;
  }
}

sl_status_t sl_net_deinit(sl_net_interface_t interface)
{
  switch (SL_NET_INTERFACE_TYPE(interface)) {
#if NETWORK_INTERFACE_VALID(SL_NET_WIFI_CLIENT_1_INTERFACE)
    case SL_NET_WIFI_CLIENT_1_INTERFACE:
      return sl_net_wifi_client_deinit(interface);
#endif
#if NETWORK_INTERFACE_VALID(SL_NET_WIFI_CLIENT_2_INTERFACE)
    case SL_NET_WIFI_CLIENT_2_INTERFACE:
      return sl_net_wifi_client_deinit(interface);
#endif
#if NETWORK_INTERFACE_VALID(SL_NET_WIFI_AP_1_INTERFACE)
    case SL_NET_WIFI_AP_1_INTERFACE:
      return sl_net_wifi_ap_deinit(interface);
#endif
#if NETWORK_INTERFACE_VALID(SL_NET_WIFI_AP_2_INTERFACE)
    case SL_NET_WIFI_AP_2_INTERFACE:
      return sl_net_wifi_ap_deinit(interface);
#endif
    default:
      return SL_STATUS_NOT_SUPPORTED;
  }
}

sl_status_t sl_net_up(sl_net_interface_t interface, sl_net_profile_id_t profile_id)
{
  switch (SL_NET_INTERFACE_TYPE(interface)) {
#if NETWORK_INTERFACE_VALID(SL_NET_WIFI_CLIENT_1_INTERFACE)
    case SL_NET_WIFI_CLIENT_1_INTERFACE:
      return sl_net_wifi_client_up(interface, profile_id);
#endif
#if NETWORK_INTERFACE_VALID(SL_NET_WIFI_CLIENT_2_INTERFACE)
    case SL_NET_WIFI_CLIENT_2_INTERFACE:
      return sl_net_wifi_client_up(interface, profile_id);
#endif
#if NETWORK_INTERFACE_VALID(SL_NET_WIFI_AP_1_INTERFACE)
    case SL_NET_WIFI_AP_1_INTERFACE:
      return sl_net_wifi_ap_up(interface, profile_id);
#endif
#if NETWORK_INTERFACE_VALID(SL_NET_WIFI_AP_2_INTERFACE)
    case SL_NET_WIFI_AP_2_INTERFACE:
      return sl_net_wifi_ap_up(interface, profile_id);
#endif
    default:
      return SL_STATUS_NOT_SUPPORTED;
  }
}

sl_status_t sl_net_down(sl_net_interface_t interface)
{
  switch (SL_NET_INTERFACE_TYPE(interface)) {
#if NETWORK_INTERFACE_VALID(SL_NET_WIFI_CLIENT_1_INTERFACE)
    case SL_NET_WIFI_CLIENT_1_INTERFACE:
      return sl_net_wifi_client_down(interface);
#endif
#if NETWORK_INTERFACE_VALID(SL_NET_WIFI_CLIENT_2_INTERFACE)
    case SL_NET_WIFI_CLIENT_2_INTERFACE:
      return sl_net_wifi_client_down(interface);
#endif
#if NETWORK_INTERFACE_VALID(SL_NET_WIFI_AP_1_INTERFACE)
    case SL_NET_WIFI_AP_1_INTERFACE:
      return sl_net_wifi_ap_down(interface);
#endif
#if NETWORK_INTERFACE_VALID(SL_NET_WIFI_AP_2_INTERFACE)
    case SL_NET_WIFI_AP_2_INTERFACE:
      return sl_net_wifi_ap_down(interface);
#endif
    default:
      return SL_STATUS_NOT_SUPPORTED;
  }
}

sl_status_t sl_net_inet_addr(const char *addr, uint32_t *value)
{
  uint8_t ip_bytes[5] = { 0 };
  int i;
  int digits;
  int j;

  if ((NULL == addr) || (NULL == value)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  digits = 0;

  // Iterate through the characters in the IP address string
  for (i = 0, j = 0; 0 != addr[i]; i++) {
    if ('.' == addr[i]) {
      ++j;
      digits = 0;
      continue;
    }

    // Check if the character is a digit (0-9)
    if ((addr[i] < '0') || (addr[i] > '9')) {
      return SL_STATUS_INVALID_PARAMETER;
    }

    // Convert character to numeric value and update IP bytes
    ip_bytes[j] = (ip_bytes[j] * 10) + (uint8_t)(addr[i] - '0');

    digits++;
    if (digits > 3) {
      return SL_STATUS_INVALID_PARAMETER;
    }
  }

  // Ensure that there are exactly three '.' separators in the IP address
  if (j != 3) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Calculate the 32-bit integer value of the IP address
  *value = (uint32_t)ip_bytes[0];
  *value |= (uint32_t)(ip_bytes[1] << 8);
  *value |= (uint32_t)(ip_bytes[2] << 16);
  *value |= (uint32_t)(ip_bytes[3] << 24);

  return SL_STATUS_OK;
}

sl_status_t sl_net_nat_enable(const sl_net_nat_config_t *nat_config)
{
  if (nat_config == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_net_nat_config_t sli_nat_config = {
    .enable                  = 1,
    .tcp_session_timeout     = nat_config->tcp_session_timeout,
    .non_tcp_session_timeout = nat_config->non_tcp_session_timeout,
    .interface               = nat_config->interface,
  };

  return sli_net_nat_configure(&sli_nat_config);
}

sl_status_t sl_net_nat_disable(const sl_net_interface_t interface)
{

  sli_net_nat_config_t sli_nat_config = {
    .enable    = 0,
    .interface = interface,
  };

  return sli_net_nat_configure(&sli_nat_config);
}