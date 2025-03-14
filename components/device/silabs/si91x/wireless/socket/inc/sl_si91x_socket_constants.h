/********************************************************************************
 * @file  sl_si91x_socket_constants.h
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

#pragma once

#include <stdint.h>
#include "cmsis_os2.h"

#ifndef BIT
#define BIT(a) ((uint32_t)1U << a)
#endif

/******************************************************
 *               Socket Configurations
 ******************************************************/
#define SI91X_NO_ERROR 0 // This is returned by socket functions after successful execution
#define SI91X_UNDEFINED_ERROR \
  0 // This value is set to errno global variable when a socket API failed due to unknown error.

#define NUMBER_OF_SOCKETS 20

#define SI91X_HOST_WAIT_FOR_SELECT_RSP 1000

#define TCP_RX_WINDOW_SIZE 10

#define DEFAULT_STREAM_MSS_SIZE_IPV4   1460
#define DEFAULT_DATAGRAM_MSS_SIZE_IPV4 1472

#define DEFAULT_STREAM_MSS_SIZE_IPV6   1440
#define DEFAULT_DATAGRAM_MSS_SIZE_IPV6 1452

#define MAX_TCP_RETRY_COUNT         10
#define DEFAULT_TCP_KEEP_ALIVE_TIME 1200

#define SI91X_CERT_INDEX_0 0
#define SI91X_CERT_INDEX_1 1
#define SI91X_CERT_INDEX_2 2

#define SL_SI91X_TLS_EXTENSION_SNI_TYPE  1 ///< TLS extension for SNI
#define SL_SI91X_TLS_EXTENSION_ALPN_TYPE 2 ///< TLS extension for ALPN

#define SI91X_SOCKET_TCP_CLIENT 0x0000
#define SI91X_SOCKET_UDP_CLIENT 0x0001
#define SI91X_SOCKET_TCP_SERVER 0x0002
#define SI91X_SOCKET_LUDP       0x0004

#define SI91X_SOCKET_FEAT_SSL                BIT(0) // SAPI maps both SSL and synchronous to BIT(0)
#define SI91X_SOCKET_FEAT_SYNCHRONOUS        BIT(0)
#define SI91X_SOCKET_FEAT_LTCP_ACCEPT        BIT(1)
#define SI91X_WEBSOCKET_FEAT                 BIT(1)
#define SI91X_SOCKET_FEAT_TCP_ACK_INDICATION BIT(2)
#define SI91X_SOCKET_FEAT_TCP_RX_WINDOW      BIT(4)
#define SI91X_SOCKET_FEAT_CERT_INDEX         BIT(5)
#define SI91X_HIGH_PERFORMANCE_SOCKET        BIT(7)

#define MAX_RETRANSMISSION_TIME_VALUE 32

/**
 * @addtogroup SI91X_SOCKET_OPTION_NAME SiWx91x Socket Option Name
 * @ingroup SI91X_SOCKET_FUNCTIONS
 * @{ 
 */
#define SL_SI91X_SO_RCVTIME                          20 ///< Enable receive timeout
#define SL_SI91X_SO_TCP_KEEPALIVE                    26 ///< To configure the TCP keep alive
#define SL_SI91X_SO_HIGH_PERFORMANCE_SOCKET          38 ///< To configure the high performance socket
#define SL_SI91X_SO_CERT_INDEX                       46 ///< To enable set certificate index
#define SL_SI91X_SO_SSL_ENABLE                       37 ///< To enable SSL
#define SL_SI91X_SO_SSL_V_1_0_ENABLE                 42 ///< To enable SSL 1.0
#define SL_SI91X_SO_SSL_V_1_1_ENABLE                 43 ///< To enable SSL 1.1
#define SL_SI91X_SO_SSL_V_1_2_ENABLE                 44 ///< To enable SSL 1.2
#define SL_SI91x_SO_TCP_ACK_INDICATION               45 ///< To enable TCP ACK indication feature
#define SL_SI91X_SO_MAX_RETRANSMISSION_TIMEOUT_VALUE 48 ///< to configure max retransmission timeout value
#define SL_SI91X_IP_TOS                              48 ///< To configure TOS
#define SL_SI91X_SO_SSL_V_1_3_ENABLE                 49 ///< To enable SSL 1.3
#define SL_SI91X_SO_MAXRETRY                         24 ///< To enable max TCP retry count
#define SL_SI91X_SO_MSS                              40 ///< To configure the TCP MSS
#define SL_SI91X_SO_SOCK_VAP_ID                      25 ///< To configure the socket VAP ID
#define SL_SI91X_SO_TLS_SNI                          47 ///< To configure the TLS SNI extension
#define SL_SI91X_SO_TLS_ALPN                         50 ///< To configure the TLS ALPN extension
/** @} */

#define SHUTDOWN_BY_ID   0
#define SHUTDOWN_BY_PORT 1
/******************************************************
 *                 SSL features
 ******************************************************/
//Release 2.0 default ciphers
#define SSL_DEFAULT_CIPHERS                                                                    \
  (BIT_TLS_DHE_RSA_WITH_AES_256_CBC_SHA256 | BIT_TLS_DHE_RSA_WITH_AES_128_CBC_SHA256           \
   | BIT_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384 | BIT_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256     \
   | BIT_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384 | BIT_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256 \
   | BIT_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA | BIT_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA           \
   | BIT_TLS_DHE_RSA_WITH_AES_256_CBC_SHA | BIT_TLS_DHE_RSA_WITH_AES_128_CBC_SHA               \
   | BIT_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA | BIT_TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA          \
   | BIT_TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA | BIT_TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA)

//Release 2.0 all configurable ciphers
#define SSL_RELEASE_2_0_ALL_CIPHERS                                                                                   \
  (SSL_DEFAULT_CIPHERS | BIT_TLS_RSA_WITH_AES_256_CBC_SHA256 | BIT_TLS_RSA_WITH_AES_128_CBC_SHA256                    \
   | BIT_TLS_RSA_WITH_AES_256_CBC_SHA | BIT_TLS_RSA_WITH_AES_128_CBC_SHA | BIT_TLS_RSA_WITH_AES_128_CCM_8             \
   | BIT_TLS_RSA_WITH_AES_256_CCM_8 | BIT_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8 | BIT_TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8 \
   | BIT_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256 | BIT_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384                        \
   | BIT_TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256 | BIT_TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256              \
   | BIT_TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256)

#if defined(SLI_SI917) || defined(SLI_SI915)
#define SSL_EXT_CIPHERS SSL_TLSV1_3_ALL_CIPHERS
#endif

#define SSL_ALL_CIPHERS SSL_RELEASE_2_0_ALL_CIPHERS

//TLSv1.3 configurable ciphers
#if defined(SLI_SI917) || defined(SLI_SI915)
#define SSL_TLSV1_3_ALL_CIPHERS                                                                     \
  (BIT_TLS13_AES_128_GCM_SHA256 | BIT_TLS13_AES_256_GCM_SHA384 | BIT_TLS13_CHACHA20_POLY1305_SHA256 \
   | BIT_TLS13_AES_128_CCM_SHA256 | BIT_TLS13_AES_128_CCM_8_SHA256)
#endif

#define BIT_TLS_RSA_WITH_AES_256_CBC_SHA256               BIT(0)
#define BIT_TLS_RSA_WITH_AES_128_CBC_SHA256               BIT(1)
#define BIT_TLS_RSA_WITH_AES_256_CBC_SHA                  BIT(2)
#define BIT_TLS_RSA_WITH_AES_128_CBC_SHA                  BIT(3)
#define BIT_TLS_RSA_WITH_AES_128_CCM_8                    BIT(4)
#define BIT_TLS_RSA_WITH_AES_256_CCM_8                    BIT(5)
#define BIT_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8            BIT(6)
#define BIT_TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8            BIT(7)
#define BIT_TLS_DHE_RSA_WITH_AES_128_GCM_SHA256           BIT(8)
#define BIT_TLS_DHE_RSA_WITH_AES_256_GCM_SHA384           BIT(9)
#define BIT_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256         BIT(10)
#define BIT_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384         BIT(11)
#define BIT_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256       BIT(12)
#define BIT_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384       BIT(13)
#define BIT_TLS_DHE_RSA_WITH_AES_256_CBC_SHA256           BIT(14)
#define BIT_TLS_DHE_RSA_WITH_AES_128_CBC_SHA256           BIT(15)
#define BIT_TLS_DHE_RSA_WITH_AES_256_CBC_SHA              BIT(16)
#define BIT_TLS_DHE_RSA_WITH_AES_128_CBC_SHA              BIT(17)
#define BIT_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384         BIT(18)
#define BIT_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256         BIT(19)
#define BIT_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA            BIT(20)
#define BIT_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA            BIT(21)
#define BIT_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384       BIT(22)
#define BIT_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256       BIT(23)
#define BIT_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA          BIT(24)
#define BIT_TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA             BIT(25)
#define BIT_TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA           BIT(26)
#define BIT_TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA         BIT(27)
#define BIT_TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256   BIT(28)
#define BIT_TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256 BIT(29)
#define BIT_TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256     BIT(30)
#define SSL_NEW_CIPHERS                                   BIT(31)

// TLSv1.3 supported ciphers
#if defined(SLI_SI917) || defined(SLI_SI915)
#define BIT_TLS13_AES_128_GCM_SHA256       BIT(0)
#define BIT_TLS13_AES_256_GCM_SHA384       BIT(1)
#define BIT_TLS13_CHACHA20_POLY1305_SHA256 BIT(2)
#define BIT_TLS13_AES_128_CCM_SHA256       BIT(3)
#define BIT_TLS13_AES_128_CCM_8_SHA256     BIT(4)
#endif
