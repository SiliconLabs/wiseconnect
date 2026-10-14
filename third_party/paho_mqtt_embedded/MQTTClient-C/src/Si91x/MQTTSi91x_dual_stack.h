/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Allan Stockdill-Mander - initial API and implementation and/or initial documentation
 *******************************************************************************/

#ifndef MQTTSI91X_DUAL_STACK_H
#define MQTTSI91X_DUAL_STACK_H

#include <stdint.h>
#include "sl_utility.h"
#include "cmsis_os2.h"

#define MQTT_WITH_WEBSOCKET_HEADER_LEN         3
#define WAIT_TIMEOUT                           5000
#define SLI_917_WAIT_FOREVER                   0
#define WIFI_BLOCKED_TIMEOUT_SF                1
#define DEFAULT_TIMEOUT                        SLI_TX_EVENT_WAIT_TIME
#define SLI_TX_EVENT_WAIT_TIME                 (30000 + TX_WAIT_TIME)
#define TX_WAIT_TIME                           0
#define SLI_SOCKET_RECVFROM_RESPONSE_WAIT_TIME ((SLI_917_WAIT_FOREVER * WIFI_BLOCKED_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define SINGLE_PKT_TCP_STREAM_TIMEOUT          WAIT_TIMEOUT + SLI_SOCKET_RECVFROM_RESPONSE_WAIT_TIME
#define NETWORK_ERROR_NULL_STRUCTURE           -1 // Error: NULL network structure
#define NETWORK_ERROR_NULL_ADDRESS             -2 // Error: NULL address
#define NETWORK_ERROR_INVALID_TYPE             -3 // Error: Invalid transport type
#define NETWORK_ERROR_CONNECT_FAILED           -4 // Error: Socket connect failed
#define NETWORK_ERROR_INVALID_FLAGS            -5 // Error: sl_paho_network_connect flags invalid

// Address family for sl_paho_network_connect; set exactly one (distinct bits).
#define SL_PAHO_NETWORK_FLAG_IPV4 (1U << 0)
#define SL_PAHO_NETWORK_FLAG_IPV6 (1U << 1)

// MQTT TLS Configuration
#ifndef MQTT_TLS_AVAILABLE
#define MQTT_TLS_AVAILABLE 1 // Set to 1 to enable TLS support, 0 to disable
#endif

#include "sli_mqtt_tls_alpn.h"

typedef struct Timer Timer;
struct Timer {
  uint32_t systick_period;
  uint32_t end_time;
};

typedef enum { MQTT_TRANSPORT_TCP, MQTT_TRANSPORT_WEBSOCKET } mqtt_transport_t;

/**
 * @brief Represents the network abstraction layer for MQTT communication with dual stack.
 *
 * @details
 * The `Network` structure provides an abstraction for network operations required by the MQTT client.
 * This implementation uses sl_si91x_socket APIs for the offload network stack in dual stack mode.
 * It supports TCP transport over the Si91x offload stack.
 */
typedef struct Network Network;
struct Network {
  int socket;
  int (*mqttread)(Network *, unsigned char *, int, int);
  int (*mqttwrite)(Network *, unsigned char *, int, int);
  void (*disconnect)(Network *);
  mqtt_transport_t transport_type;
};

uint32_t osKernelGetTickCount(void);
void InitTimer(Timer *);
char expired(Timer *);
void countdown_ms_mqtt(Timer *, unsigned int);
void countdown(Timer *, unsigned int);
int left_ms_mqtt(Timer *);

void NetworkInit(Network *n);

/**
 * @brief Connects the network to an MQTT broker over TCP.
 *
 * @param[in] n         Pointer to the initialized Network structure.
 * @param[in] flags     Reserved; unused.
 * @param[in] addr      Broker IP address string.
 * @param[in] dst_port  Broker port number.
 * @param[in] src_port  Local client port number.
 * @param[in] ssl       Set to true to enable TLS on the TCP connection.
 *
 * @return 0 on success, or a negative error code on failure.
 *
 * @details
 * - When `MQTT_TLS_ALPN_ENABLED` is set to `1`, ALPN is configured automatically during
 *   TCP connection setup (after socket creation and TLS enable, before connect).
 *   Set `MQTT_TLS_ALPN_PROTOCOL` to the broker's ALPN name (e.g. `"mqtt"` for Mosquitto on port 443).
 * - ALPN applies to @ref MQTT_TRANSPORT_TCP with TLS only. The WebSocket transport
 *   (`MQTT_TRANSPORT_WEBSOCKET`) does not configure TLS ALPN through this SDK path.
 *   Secure WebSocket (WSS) on port 443 typically negotiates `http/1.1` at the TLS layer
 *   rather than `mqtt`; that scenario is not covered by this ALPN integration.
 */
int NetworkConnect(Network *n, uint8_t flags, char *addr, int dst_port, int src_port, bool ssl);

/**
 * @brief Connects the network to an MQTT broker over TCP with explicit IPv4/IPv6 selection.
 *
 * @param[in] n         Pointer to the initialized Network structure.
 * @param[in] flags     Address family: set exactly one of `SL_PAHO_NETWORK_FLAG_IPV4` or
 *                      `SL_PAHO_NETWORK_FLAG_IPV6`.
 * @param[in] addr      Pointer to raw broker IP octets (4 bytes for IPv4, 16 bytes for IPv6).
 * @param[in] dst_port  Broker port number.
 * @param[in] src_port  Local client port number.
 * @param[in] ssl       Set to true to enable TLS on the TCP connection.
 *
 * @return
 * - `0` on success.
 * - `NETWORK_ERROR_NULL_STRUCTURE (-1)` if the `Network` structure is `NULL`.
 * - `NETWORK_ERROR_NULL_ADDRESS (-2)` if the `addr` is `NULL`.
 * - `NETWORK_ERROR_INVALID_TYPE (-3)` if the `transport_type` is not TCP.
 * - `NETWORK_ERROR_CONNECT_FAILED (-4)` if the socket connect fails.
 * - `NETWORK_ERROR_INVALID_FLAGS (-5)` if neither or both of IPV4/IPV6 are set, or if IPV6
 *   is requested on an IPv4-only build.
 * - Other negative error codes for bind/TLS setup failures.
 *
 * @details
 * - Prefer this API over NetworkConnect when the application must choose the MQTT TCP socket
 *   family on dual-IP builds.
 * - When `MQTT_TLS_ALPN_ENABLED` is set to `1`, ALPN is configured automatically during
 *   TCP connection setup (after socket creation and TLS enable, before connect).
 *   Set `MQTT_TLS_ALPN_PROTOCOL` to the broker's ALPN name (e.g. `"mqtt"` for Mosquitto on port 443).
 *
 * @note
 * - Ensure that the `Network` structure is initialized using `NetworkInit` before calling this function.
 * - WebSocket transport is not supported by this API.
 */
int sl_paho_network_connect(Network *n, uint8_t flags, char *addr, int dst_port, int src_port, bool ssl);

void NetworkDisconnect(Network *n);

#endif /* MQTTSI91X_DUAL_STACK_H */
