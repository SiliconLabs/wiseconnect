/***************************************************************************/ /**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_si91x_protocol_types.h"
#include "sl_si91x_constants.h"
#include "sl_net_constants.h"
#include "sl_wifi_constants.h"
#include "sl_status.h"
#include "sl_constants.h"
#include "sl_wifi_types.h"
#include "sl_net_rsi_utility.h"
#include <string.h>

sl_status_t convert_rsi_ipv4_address_to_sl_ip_address(sl_ip_address_t *ip_address_buffer,
                                                      const sl_si91x_rsp_ipv4_params_t *ip_params)
{
  // Verify input pointers
  SL_VERIFY_POINTER_OR_RETURN(ip_address_buffer, SL_STATUS_WIFI_NULL_PTR_ARG);
  SL_VERIFY_POINTER_OR_RETURN(ip_params, SL_STATUS_WIFI_NULL_PTR_ARG);

  uint8_t *ip_address;

  ip_address_buffer->type = SL_IPV4;
  ip_address              = ip_address_buffer->ip.v4.bytes;

  memcpy(ip_address, ip_params->ipaddr, sizeof(sl_ipv4_address_t));
  return SL_STATUS_OK;
}

sl_status_t convert_si91x_dns_response(sl_ip_address_t *ip_address, const sl_si91x_dns_response_t *si91x_dns_response)
{
  SL_VERIFY_POINTER_OR_RETURN(ip_address, SL_STATUS_WIFI_NULL_PTR_ARG);
  SL_VERIFY_POINTER_OR_RETURN(si91x_dns_response, SL_STATUS_WIFI_NULL_PTR_ARG);

  // Check if DNS response has IP addresses
  if ((si91x_dns_response->ip_count[0] | (si91x_dns_response->ip_count[1] << 8)) <= 0) {
    return SL_STATUS_OK;
  }

  // Determine IP address size (IPv4 or IPv6) and copy the address bytes
  uint8_t ip_address_size = (si91x_dns_response->ip_version[0] | si91x_dns_response->ip_version[1] << 8)
                                == SL_IPV4_ADDRESS_LENGTH
                              ? SL_IPV4_ADDRESS_LENGTH
                              : SL_IPV6_ADDRESS_LENGTH;
  uint8_t *sl_ip_address;
  const uint8_t *si91x_ip_address;

  ip_address->type = ip_address_size == SL_IPV4_ADDRESS_LENGTH ? SL_IPV4 : SL_IPV6;

  si91x_ip_address = ip_address_size == SL_IPV4_ADDRESS_LENGTH ? si91x_dns_response->ip_address[0].ipv4_address
                                                               : si91x_dns_response->ip_address[0].ipv6_address;
  sl_ip_address    = ip_address_size == SL_IPV4_ADDRESS_LENGTH ? ip_address->ip.v4.bytes : ip_address->ip.v6.bytes;

  memcpy(sl_ip_address, si91x_ip_address, ip_address_size);

  return SL_STATUS_OK;
}

sl_status_t convert_si91x_event_to_sl_net_event(const uint16_t *event, sl_net_event_t *sl_net_event)
{
  // Verify input pointers
  SL_WIFI_ARGS_CHECK_NULL_POINTER(event);
  SL_WIFI_ARGS_CHECK_NULL_POINTER(sl_net_event);

  // Map SI91X events to SimpleLink network events
  switch (*event) {
    case RSI_WLAN_RSP_DNS_QUERY: {
      *sl_net_event = SL_NET_DNS_RESOLVE_EVENT;
      return SL_STATUS_OK;
    }
    case RSI_WLAN_RSP_PING_PACKET: {
      *sl_net_event = SL_NET_PING_RESPONSE_EVENT;
      return SL_STATUS_OK;
    }
    case RSI_WLAN_RSP_OTA_FWUP: {
      *sl_net_event = SL_NET_OTA_FW_UPDATE_EVENT;
      return SL_STATUS_OK;
    }
    case RSI_WLAN_RSP_IPCONFV4: {
      *sl_net_event = SL_NET_DHCP_NOTIFICATION_EVENT;
      return SL_STATUS_OK;
    }
    case RSI_WLAN_RSP_IPV4_CHANGE:
    case RSI_WLAN_RSP_IPCONFV6: {
      *sl_net_event = SL_NET_IP_ADDRESS_CHANGE_EVENT;
      return SL_STATUS_OK;
    }
    default:
      break;
  }

  return SL_STATUS_FAIL;
}

#ifdef SLI_SI91X_INTERNAL_HTTP_CLIENT
// Convert integer to string
void convert_itoa(uint32_t val, uint8_t *str)
{
  int16_t ii = 0;
  int16_t jj = 0;
  uint8_t tmp[10];

  if (val == 0) {
    // if value is zero then handling
    str[jj] = '0';
    jj++;
    str[jj] = '\0';
    return;
  }

  // Convert the integer to a string
  while (val) {
    tmp[ii] = '0' + (val % 10);
    val /= 10;
    ii++;
  }

  // Reverse the string
  for (jj = 0, ii--; ii >= 0; ii--, jj++) {
    str[jj] = tmp[ii];
  }
  str[jj] = '\0';
}

sl_status_t convert_si91x_event_to_sl_http_client_event(const uint16_t *event,
                                                        sl_http_client_event_t *sl_http_client_event)
{
  // Verify input pointer
  SL_WIFI_ARGS_CHECK_NULL_POINTER(event);

  // Map SI91X HTTP client events to SimpleLink HTTP client events
  switch (*event) {
    case RSI_WLAN_RSP_HTTP_CLIENT_GET: {
      *sl_http_client_event = SL_HTTP_CLIENT_GET_RESPONSE_EVENT;
      return SL_STATUS_OK;
    }

    case RSI_WLAN_RSP_HTTP_CLIENT_POST:
    case RSI_WLAN_RSP_HTTP_CLIENT_POST_DATA: {
      *sl_http_client_event = SL_HTTP_CLIENT_POST_RESPONSE_EVENT;
      return SL_STATUS_OK;
    }

    case RSI_WLAN_RSP_HTTP_CLIENT_PUT: {
      *sl_http_client_event = SL_HTTP_CLIENT_PUT_RESPONSE_EVENT;
      return SL_STATUS_OK;
    }
    default:
      break;
  }

  return SL_STATUS_FAIL;
}
#endif
