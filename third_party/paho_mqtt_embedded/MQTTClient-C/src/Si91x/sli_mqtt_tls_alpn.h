/***************************************************************************/ /**
 * @file sli_mqtt_tls_alpn.h
 * @brief Shared MQTT TLS ALPN configuration and helpers for Si91x Paho ports.
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
#ifndef SLI_MQTT_TLS_ALPN_H
#define SLI_MQTT_TLS_ALPN_H

// MQTT TLS ALPN Configuration (for non-AWS MQTT brokers on port 443)
#ifndef MQTT_TLS_ALPN_ENABLED
#define MQTT_TLS_ALPN_ENABLED 0 // Set to 1 to enable ALPN during TLS handshake
#endif

#ifndef MQTT_TLS_ALPN_PROTOCOL
#define MQTT_TLS_ALPN_PROTOCOL "mqtt" // ALPN protocol name (e.g. "mqtt" for Mosquitto on port 443)
#endif

#define SLI_MQTT_TLS_ALPN_PROTOCOL_MAX_LEN 64
#define SLI_MQTT_TLS_ALPN_PROTOCOL_LEN     (sizeof(MQTT_TLS_ALPN_PROTOCOL) - 1U)

#if MQTT_TLS_ALPN_ENABLED && defined(SLI_MQTT_TLS_ALPN_USE_BSD)
int sli_mqtt_tls_alpn_set_bsd(int socket_fd);
#endif

#if MQTT_TLS_ALPN_ENABLED && defined(SLI_MQTT_TLS_ALPN_USE_ASYNC)
int sli_mqtt_tls_alpn_set_async(int socket_fd);
#endif

#endif // SLI_MQTT_TLS_ALPN_H
