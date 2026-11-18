/***************************************************************************/ /**
 * @file sli_mqtt_tls_alpn.c
 * @brief Shared MQTT TLS ALPN helpers for Si91x Paho ports.
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sli_mqtt_tls_alpn.h"

#if MQTT_TLS_ALPN_ENABLED

#include <errno.h>
#include <string.h>
#include "sl_constants.h"
#include "sl_si91x_socket_constants.h"
#include "sl_si91x_socket_utility.h"

#if defined(SLI_MQTT_TLS_ALPN_USE_BSD)
#include "socket.h"
#endif

#if defined(SLI_MQTT_TLS_ALPN_USE_ASYNC)
#include "sl_si91x_socket.h"
#endif

static socklen_t sli_mqtt_build_alpn_tlv(sl_si91x_socket_type_length_value_t *tlv, size_t buffer_capacity)
{
  const size_t header_size   = sizeof(sl_si91x_socket_type_length_value_t);
  const uint16_t alpn_length = (uint16_t)SLI_MQTT_TLS_ALPN_PROTOCOL_LEN;
  const size_t total_size    = header_size + alpn_length;

  if ((alpn_length == 0U) || (total_size > buffer_capacity)) {
    return 0;
  }

  tlv->type   = SL_SI91X_TLS_EXTENSION_ALPN_TYPE;
  tlv->length = alpn_length;
  memcpy(tlv->value, MQTT_TLS_ALPN_PROTOCOL, alpn_length);
  return (socklen_t)total_size;
}

#if defined(SLI_MQTT_TLS_ALPN_USE_BSD)
int sli_mqtt_tls_alpn_set_bsd(int socket_fd)
{
  uint8_t alpn_storage[sizeof(sl_si91x_socket_type_length_value_t) + SLI_MQTT_TLS_ALPN_PROTOCOL_MAX_LEN];
  sl_si91x_socket_type_length_value_t *alpn_value = (sl_si91x_socket_type_length_value_t *)alpn_storage;
  const socklen_t option_len                      = sli_mqtt_build_alpn_tlv(alpn_value, sizeof(alpn_storage));

  if (option_len == 0) {
    errno = EINVAL;
    SL_DEBUG_LOG_V2(ERROR,
                    "ALPN protocol length invalid or exceeds max length (%d)\r\n",
                    SLI_MQTT_TLS_ALPN_PROTOCOL_MAX_LEN);
    return -1;
  }

  const int socket_return_value = setsockopt(socket_fd, SOL_SOCKET, SL_SO_TLS_ALPN, alpn_value, option_len);
  if (socket_return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Set Socket option ALPN extension failed with bsd error: %d\r\n", errno);
    return -1;
  }

  return 0;
}
#endif

#if defined(SLI_MQTT_TLS_ALPN_USE_ASYNC)
int sli_mqtt_tls_alpn_set_async(int socket_fd)
{
  uint8_t alpn_storage[sizeof(sl_si91x_socket_type_length_value_t) + SLI_MQTT_TLS_ALPN_PROTOCOL_MAX_LEN];
  sl_si91x_socket_type_length_value_t *alpn_value = (sl_si91x_socket_type_length_value_t *)alpn_storage;
  const socklen_t option_len                      = sli_mqtt_build_alpn_tlv(alpn_value, sizeof(alpn_storage));

  if (option_len == 0) {
    errno = EINVAL;
    SL_DEBUG_LOG_V2(ERROR,
                    "ALPN protocol length invalid or exceeds max length (%d)\r\n",
                    SLI_MQTT_TLS_ALPN_PROTOCOL_MAX_LEN);
    return -1;
  }

  const int status = sl_si91x_setsockopt(socket_fd, SL_SI91X_SOL_SOCKET, SL_SI91X_SO_TLS_ALPN, alpn_value, option_len);
  if (status < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Set Socket option ALPN extension failed with error: %d\r\n", errno);
    return -1;
  }

  return 0;
}
#endif

#endif // MQTT_TLS_ALPN_ENABLED
