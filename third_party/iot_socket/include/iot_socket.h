/*
 * Copyright (c) 2018-2022 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * $Date:        27. January 2020
 * $Revision:    V1.2.0
 *
 * Project:      IoT Socket API definitions
 *
 * Version 1.2.0
 *   Extended iotSocketRecv/RecvFrom/Send/SendTo (support for polling)
 * Version 1.1.0
 *   Added function iotSocketRecvFrom
 *   Added function iotSocketSendTo
 *   Added function iotSocketGetSockName
 *   Added function iotSocketGetPeerName
 *   Added function iotSocketGetOpt
 *   Removed function iotSocketGetStatus
 *   Changed IP address pointer type to uint8_t *
 *   Added socket option SO_KEEPALIVE and SO_TYPE
 *   Removed socket option SO_REUSEADDR
 * Version 1.0.0
 *   Initial Release
 */

#ifndef IOT_SOCKET_H
#define IOT_SOCKET_H

#ifdef  __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/**
 * @addtogroup IOT_SOCKET_ADDRESS_FAMILY Address Family
 * @ingroup IOT_SOCKET_FUNCTIONS
 * @{ 
 */
/**** Address Family definitions ****/
#define IOT_SOCKET_AF_INET              1       ///< Internet Protocol version 4 (IPv4) address family.
#define IOT_SOCKET_AF_INET6             2       ///< Internet Protocol version 6 (IPv6) address family.
/** @} */

/**
 * @addtogroup IOT_SOCKET_TYPE Socket Type
 * @ingroup IOT_SOCKET_FUNCTIONS
 * @{ 
 */
/**** Socket Type definitions ****/
#define IOT_SOCKET_SOCK_STREAM          1       ///< Stream socket type. Provides sequenced, reliable, two-way, connection-based byte streams.
#define IOT_SOCKET_SOCK_DGRAM           2       ///< Datagram socket type. Supports datagrams, which are connectionless, unreliable messages of a fixed maximum length.
/** @} */

/**
 * @addtogroup IOT_SOCKET_PROTOCOL Socket Protocol
 * @ingroup IOT_SOCKET_FUNCTIONS
 * @{ 
 */
/**** Socket Protocol definitions ****/
#define IOT_SOCKET_IPPROTO_TCP          1       ///< Transmission Control Protocol (TCP). This is a reliable, stream-oriented protocol.
#define IOT_SOCKET_IPPROTO_UDP          2       ///< User Datagram Protocol (UDP). This is a connectionless, unreliable, datagram protocol.
/** @} */

/**
 * @addtogroup IOT_SOCKET_OPTION_ID Socket Option Id
 * @ingroup IOT_SOCKET_FUNCTIONS
 * @{ 
 */
/**** Socket Option definitions ****/
#define IOT_SOCKET_IO_FIONBIO           1       ///< Non-blocking I/O (Set only, default = 0); opt_val = &nbio, opt_len = sizeof(nbio), nbio (integer): 0=blocking, non-blocking otherwise
#define IOT_SOCKET_SO_RCVTIMEO          2       ///< Receive timeout in ms (default = 0); opt_val = &timeout, opt_len = sizeof(timeout)
#define IOT_SOCKET_SO_SNDTIMEO          3       ///< Send timeout in ms (default = 0); opt_val = &timeout, opt_len = sizeof(timeout)
#define IOT_SOCKET_SO_KEEPALIVE         4       ///< Keep-alive messages (default = 0); opt_val = &keepalive, opt_len = sizeof(keepalive), keepalive (integer): 0=disabled, enabled otherwise
#define IOT_SOCKET_SO_TYPE              5       ///< Socket Type (Get only); opt_val = &socket_type, opt_len = sizeof(socket_type), socket_type (integer): IOT_SOCKET_SOCK_xxx

/**
 * @name Silicon Labs vendor extensions
 *
 * @warning These option ids are NOT part of the OSF / CMSIS IoT Socket
 *          specification. They are defined only for the SiWx91x BSD-backed
 *          IoT Socket wrapper and are non-portable. Code that needs to run
 *          against any other IoT Socket implementation must guard these
 *          option ids with @c \#ifdef @c IOT_SOCKET_VENDOR_SILABS.
 *
 * The standard CMSIS IoT Socket option ids occupy values 1..5; the
 * 0x1100+ range is reserved for Silicon Labs vendor extensions.
 * @{
 */
#define IOT_SOCKET_VENDOR_SILABS        1       ///< Marker that the SiLabs vendor extensions below are available.

/**
 * @brief Enable TLS on a stream socket (Silicon Labs vendor extension).
 *
 * @details
 *   Internally translated by the SiWx91x BSD-backed wrapper to
 *   @c setsockopt(socket, @c SOL_TCP, @c TCP_ULP, @c opt_val, @c opt_len).
 *   After the option is set, @ref iotSocketConnect / @ref iotSocketSend /
 *   @ref iotSocketRecv operate over TLS transparently.
 *
 * @par Call ordering
 *   This option must be set on a freshly created stream socket before calling 
 *   @ref iotSocketConnect(). Setting it after connection has no
 *   effect because the TLS handshake is performed during connection.
 *
 * @par Argument layout
 *   - @c opt_val : pointer to a NUL-terminated TLS-version string.
 *                  Accepted values are defined in @c socket.h:
 *                  @c "tls", @c "tls_1_0", @c "tls_1_2", @c "tls_1_3".
 *   - @c opt_len : @c strlen(opt_val) + 1 (must include the trailing NUL).
 *
 * @par Other TLS attributes (certificate index, SNI, ALPN, high-performance
 *      flag, ...) are NOT exposed through this opt_id. They must be configured
 *      using the underlying @c setsockopt() call directly on the same fd.
 *
 * @par Example
 * @code{.c}
 * #ifdef IOT_SOCKET_VENDOR_SILABS
 *   const char tls_version[] = "tls_1_2";
 *   int32_t rc = iotSocketSetOpt(sock, IOT_SOCKET_SO_SSL_ENABLE,
 *                                tls_version, sizeof(tls_version));
 *   if (rc < 0) { ... }
 * #endif
 *   rc = iotSocketConnect(sock, server_ip, sizeof(server_ip), 443);
 * @endcode
 */
#ifndef IOT_SOCKET_SO_SSL_ENABLE
#define IOT_SOCKET_SO_SSL_ENABLE        0x1100  ///< Enable TLS on a stream socket. opt_val = TLS version string ("tls", "tls_1_0", "tls_1_2", "tls_1_3"); opt_len = strlen(opt_val) + 1. Must be set before iotSocketConnect().
#endif
/** @} */ /* end of Silicon Labs vendor extensions */

/** @} */

/**
 * @addtogroup IOT_SOCKET_RETURN_CODES Socket Return Codes
 * @ingroup IOT_SOCKET_FUNCTIONS
 * @{ 
 */
/**** Socket Return Codes ****/
#define IOT_SOCKET_ERROR                (-1)    ///< General error code for unspecified errors.
#define IOT_SOCKET_ESOCK                (-2)    ///< Error code indicating an invalid socket.
#define IOT_SOCKET_EINVAL               (-3)    ///< Error code indicating an invalid argument.
#define IOT_SOCKET_ENOTSUP              (-4)    ///< Error code indicating that the requested operation is not supported.
#define IOT_SOCKET_ENOMEM               (-5)    ///< Error code indicating that there is not enough memory to perform the requested operation.
#define IOT_SOCKET_EAGAIN               (-6)    ///< Error code indicating that the operation would block or has timed out.
#define IOT_SOCKET_EINPROGRESS          (-7)    ///< Error code indicating that the operation is in progress.
#define IOT_SOCKET_ETIMEDOUT            (-8)    ///< Error code indicating that the operation has timed out.
#define IOT_SOCKET_EISCONN              (-9)    ///< Error code indicating that the socket is already connected.
#define IOT_SOCKET_ENOTCONN             (-10)   ///< Error code indicating that the socket is not connected.
#define IOT_SOCKET_ECONNREFUSED         (-11)   ///< Error code indicating that the connection was refused by the peer.
#define IOT_SOCKET_ECONNRESET           (-12)   ///< Error code indicating that the connection was reset by the peer.
#define IOT_SOCKET_ECONNABORTED         (-13)   ///< Error code indicating that the connection was aborted locally.
#define IOT_SOCKET_EALREADY             (-14)   ///< Error code indicating that a connection is already in progress.
#define IOT_SOCKET_EADDRINUSE           (-15)   ///< Error code indicating that the address is already in use.
#define IOT_SOCKET_EHOSTNOTFOUND        (-16)   ///< Error code indicating that the host was not found.
/** @} */

/**
 * @addtogroup IOT_SOCKET_FUNCTIONS
 * @{ 
 */

/***************************************************************************/ 
/**
 * @brief
 *   Create a communication socket.
 * 
 * @details
 *   The @ref iotSocketCreate() function creates a new socket that is used for communication. 
 *   The socket is created with the specified address family, socket type, and protocol.
 *
 * @pre Pre-conditions:
 * - The Wi-Fi/Net stack must be initialized and a network profile must be up.
 * - If socket resources are configured using @ref sl_si91x_config_socket(), that call must be made before @ref iotSocketCreate().
 *
 * @post Post-conditions:
 * - On success a new IoT socket descriptor is allocated and can be passed to @ref iotSocketBind(),
 *   @ref iotSocketConnect(), @ref iotSocketListen(), etc.
 * - On failure no descriptor is allocated and a negative @c IOT_SOCKET_E* error code is returned.
 *
 * @param[in] af
 *   Address family. One of the values from @ref IOT_SOCKET_ADDRESS_FAMILY. Only @c IOT_SOCKET_AF_INET and @c IOT_SOCKET_AF_INET6 are supported.
 * 
 * @param[in] type
 *   Socket type. One of the values from @ref IOT_SOCKET_TYPE. Only @c IOT_SOCKET_SOCK_STREAM and @c IOT_SOCKET_SOCK_DGRAM are supported.
 * 
 * @param[in] protocol
 *   Socket protocol. One of the values from @ref IOT_SOCKET_PROTOCOL. Use @c IOT_SOCKET_IPPROTO_TCP with
 *   @c IOT_SOCKET_SOCK_STREAM and @c IOT_SOCKET_IPPROTO_UDP with @c IOT_SOCKET_SOCK_DGRAM.
 * 
 * @return
 *   Returns the socket identification number (>=0) on success. On failure, returns one of the following error codes:
 *   - @ref IOT_SOCKET_EINVAL : Invalid argument.
 *   - @ref IOT_SOCKET_ENOTSUP : Operation not supported.
 *   - @ref IOT_SOCKET_ENOMEM : Not enough memory.
 *   - @ref IOT_SOCKET_ERROR : Unspecified error.
 *
 * @retval >=0                        Valid IoT socket descriptor.
 * @retval IOT_SOCKET_EINVAL          Invalid combination of @p af, @p type or @p protocol.
 * @retval IOT_SOCKET_ENOTSUP         Requested family/type/protocol is not supported.
 * @retval IOT_SOCKET_ENOMEM          Not enough memory to allocate the socket.
 *
 * @note Thread safety:
 * - Thread-safe: multiple threads may call @ref iotSocketCreate() concurrently to obtain distinct descriptors.
 *
 * @note Side effects:
 * - Allocates an entry in the internal socket pool.
 *
 * @see iotSocketBind(), iotSocketConnect(), iotSocketListen(), iotSocketClose(), sl_si91x_config_socket()
 *
 * @code{.c}
 * // Create an IPv4 TCP socket.
 * int32_t sock = iotSocketCreate(IOT_SOCKET_AF_INET,
 *                                IOT_SOCKET_SOCK_STREAM,
 *                                IOT_SOCKET_IPPROTO_TCP);
 * if (sock < 0) {
 *   printf("iotSocketCreate() failed, rc = %ld\r\n", (long)sock);
 *   return sock;
 * }
 * @endcode
 ******************************************************************************/
extern int32_t iotSocketCreate(int32_t af, int32_t type, int32_t protocol);

/***************************************************************************/ 
/**
 * @brief
 *   Assign a local address to a socket.
 * 
 * @details
 *   The @ref iotSocketBind() function assigns a local IP address and port number to a socket identified by `socket`. 
 *   This function is typically used on the server side to bind a socket to a specific IP address and port number, 
 *   so that the socket can listen for incoming connections or datagrams.
 *
 * @pre Pre-conditions:
 * - @p socket must be a valid descriptor returned by @ref iotSocketCreate().
 * - Must be called before @ref iotSocketListen() on a TCP server socket and before send/receive on a UDP socket that needs a fixed local port.
 *
 * @post Post-conditions:
 * - On success the socket is associated with the supplied local address/port.
 * - On failure the socket remains unbound.
 * 
 * @param[in] socket
 *   Socket identification number.
 * 
 * @param[in] ip
 *   Pointer to the local IP address. The address should be in network byte order. Must be non-NULL.
 * 
 * @param[in] ip_len
 *   Length of the `ip` address in bytes. Must be 4 for IPv4 or 16 for IPv6.
 * 
 * @param[in] port
 *   Local port number. The port number should be in host byte order.
 * 
 * @return
 *   Returns 0 on success. On failure, returns one of the following error codes:
 *   - @ref IOT_SOCKET_ESOCK : Invalid socket.
 *   - @ref IOT_SOCKET_EINVAL : Invalid argument (address or socket already bound).
 *   - @ref IOT_SOCKET_EADDRINUSE : Address already in use.
 *   - @ref IOT_SOCKET_ERROR : Unspecified error.
 *
 * @retval 0                          Success.
 * @retval IOT_SOCKET_ESOCK           @p socket is not a valid descriptor.
 * @retval IOT_SOCKET_EINVAL          @p ip is NULL, @p ip_len invalid, or the socket is already bound.
 * @retval IOT_SOCKET_EADDRINUSE      The requested address/port is already in use.
 * @retval IOT_SOCKET_ERROR           Unspecified stack error.
 *
 * @note Thread safety:
 * - Not thread-safe on the same descriptor; serialize concurrent access.
 *
 * @note Side effects:
 * - Reserves the local address/port in the stack.
 *
 * @see iotSocketCreate(), iotSocketListen(), iotSocketConnect(), iotSocketGetSockName()
 *
 * @code{.c}
 * // Bind to any local IPv4 address (0.0.0.0) on TCP port 5000.
 * const uint8_t any_addr[4] = { 0, 0, 0, 0 };
 * int32_t       rc          = iotSocketBind(sock, any_addr, sizeof(any_addr), 5000);
 * if (rc < 0) {
 *   printf("iotSocketBind() failed, rc = %ld\r\n", (long)rc);
 *   iotSocketClose(sock);
 *   return rc;
 * }
 * @endcode
 ******************************************************************************/
extern int32_t iotSocketBind(int32_t socket, const uint8_t *ip, uint32_t ip_len, uint16_t port);

/***************************************************************************/ 
/**
 * @brief
 *   Listen for socket connections.
 * 
 * @details
 *   The @ref iotSocketListen() function marks the socket referred to by `socket` as a passive socket, 
 *   that is, as a socket that will be used to accept incoming connection requests using @ref iotSocketAccept(). 
 *   The `backlog` parameter defines the maximum length to which the queue of pending connections for `socket` may grow.
 *
 * @pre Pre-conditions:
 * - @p socket must be a TCP (@c IOT_SOCKET_SOCK_STREAM) socket created using @ref iotSocketCreate() and bound using @ref iotSocketBind().
 *
 * @post Post-conditions:
 * - On success the socket transitions into the listening state and can be passed to @ref iotSocketAccept().
 * 
 * @param[in] socket
 *   Socket identification number.
 * 
 * @param[in] backlog
 *   Maximum number of pending connections that can be queued. Must be > 0.
 * 
 * @return
 *   Returns 0 on success. On failure, returns one of the following error codes:
 *   - @ref IOT_SOCKET_ESOCK : Invalid socket.
 *   - @ref IOT_SOCKET_EINVAL : Invalid argument (socket not bound).
 *   - @ref IOT_SOCKET_ENOTSUP : Operation not supported.
 *   - @ref IOT_SOCKET_EISCONN : Socket is already connected.
 *   - @ref IOT_SOCKET_ERROR : Unspecified error.
 *
 * @retval 0                          Success.
 * @retval IOT_SOCKET_ESOCK           @p socket is not a valid descriptor.
 * @retval IOT_SOCKET_EINVAL          Socket is not bound or @p backlog is invalid.
 * @retval IOT_SOCKET_ENOTSUP         Socket type does not support listening (e.g. UDP).
 * @retval IOT_SOCKET_EISCONN         Socket is already connected.
 * @retval IOT_SOCKET_ERROR           Unspecified stack error.
 *
 * @note Thread safety:
 * - Not thread-safe on the same descriptor; serialize concurrent access.
 *
 * @note Side effects:
 * - Allocates a pending-connection queue of size @p backlog in the stack.
 *
 * @see iotSocketCreate(), iotSocketBind(), iotSocketAccept()
 *
 * @code{.c}
 * // Put the bound TCP server socket into listening mode with 1 pending connection.
 * int32_t rc = iotSocketListen(sock, 1);
 * if (rc < 0) {
 *   printf("iotSocketListen() failed, rc = %ld\r\n", (long)rc);
 *   iotSocketClose(sock);
 *   return rc;
 * }
 * @endcode
 ******************************************************************************/
extern int32_t iotSocketListen(int32_t socket, int32_t backlog);

/***************************************************************************/ 
/**
 * @brief
 *   Accept a new connection on a socket.
 * 
 * @details
 *   The @ref iotSocketAccept() function is used to accept a new incoming connection on a socket that is in listening mode. 
 *   It extracts the first connection request on the queue of pending connections for the listening socket, 
 *   `socket`, creates a new connected socket, and returns a new file descriptor referring to that socket. 
 *   The newly created socket is not in the listening state. The original socket remains open and can continue 
 *   to accept new connections.
 *
 * @pre Pre-conditions:
 * - @p socket must be a TCP socket placed in the listening state using @ref iotSocketListen().
 *
 * @post Post-conditions:
 * - On success a new connected socket descriptor is returned. If @p ip / @p port are non-NULL they are populated with the peer's address.
 * - The original listening descriptor remains open.
 * 
 * @param[in] socket
 *   Socket identification number.
 * 
 * @param[out] ip
 *   Pointer to a buffer where the address of the connecting socket will be returned. If NULL, no address is returned.
 * 
 * @param[in, out] ip_len
 *   Pointer to the length of the `ip` buffer. On input, it should contain the size of the buffer. On output, 
 *   it will be modified to indicate the actual length of the address returned. If `ip` is NULL, `ip_len` should also be NULL.
 *   Must be non-NULL when @p ip is non-NULL.
 * 
 * @param[out] port
 *   Pointer to a buffer where the port of the connecting socket will be returned. If NULL, no port is returned.
 * 
 * @return
 *   Returns the socket identification number of the accepted socket (>=0) on success. On failure, returns one of the following error codes:
 *   - @ref IOT_SOCKET_ESOCK : Invalid socket.
 *   - @ref IOT_SOCKET_EINVAL : Invalid argument (socket not in listen mode).
 *   - @ref IOT_SOCKET_ENOTSUP : Operation not supported (socket type does not support accepting connections).
 *   - @ref IOT_SOCKET_ECONNRESET : Connection reset by the peer.
 *   - @ref IOT_SOCKET_ECONNABORTED : Connection aborted locally.
 *   - @ref IOT_SOCKET_EAGAIN : Operation would block or timed out (may be called again).
 *   - @ref IOT_SOCKET_ERROR : Unspecified error.
 *
 * @retval >=0                        Descriptor for the accepted connection.
 * @retval IOT_SOCKET_ESOCK           @p socket is not a valid descriptor.
 * @retval IOT_SOCKET_EINVAL          Socket is not in listen mode or arguments are invalid.
 * @retval IOT_SOCKET_ENOTSUP         Socket type does not support accepting.
 * @retval IOT_SOCKET_ECONNRESET      Peer reset the incoming connection.
 * @retval IOT_SOCKET_ECONNABORTED    The connection has been aborted locally.
 * @retval IOT_SOCKET_EAGAIN          Non-blocking socket with no pending connections.
 * @retval IOT_SOCKET_ERROR           Unspecified stack error.
 *
 * @note Thread safety:
 * - Not thread-safe on the same listening descriptor; serialize concurrent @ref iotSocketAccept() calls.
 *
 * @note Side effects:
 * - Allocates a new socket slot for the accepted connection.
 *
 * @see iotSocketCreate(), iotSocketBind(), iotSocketListen(), iotSocketClose()
 *
 * @code{.c}
 * // Accept a new TCP client and log the peer's IPv4 address and port.
 * uint8_t  peer_ip[4]  = { 0 };
 * uint32_t peer_ip_len = sizeof(peer_ip);
 * uint16_t peer_port   = 0;
 *
 * int32_t client_sock = iotSocketAccept(sock, peer_ip, &peer_ip_len, &peer_port);
 * if (client_sock < 0) {
 *   printf("iotSocketAccept() failed, rc = %ld\r\n", (long)client_sock);
 * } else {
 *   printf("Client %u.%u.%u.%u:%u connected\r\n",
 *          peer_ip[0], peer_ip[1], peer_ip[2], peer_ip[3], peer_port);
 * }
 * @endcode
 ******************************************************************************/
extern int32_t iotSocketAccept(int32_t socket, uint8_t *ip, uint32_t *ip_len, uint16_t *port);

/***************************************************************************/ 
/**
 * @brief
 *   Connect a socket to a remote host.
 * 
 * @details
 *   The @ref iotSocketConnect() function establishes a connection to a remote host specified by the IP address and port number. 
 *   This function is used for connection-oriented protocols such as TCP. The socket identified by `socket` must be created 
 *   and optionally bound to a local address before calling this function.
 *
 * @pre Pre-conditions:
 * - @p socket must be a descriptor created using @ref iotSocketCreate().
 * - Network must be up and the destination must be reachable.
 *
 * @post Post-conditions:
 * - On success a TCP socket enters the connected state and can be used with @ref iotSocketSend() / @ref iotSocketRecv().
 * - For UDP, the peer is cached so subsequent @ref iotSocketSend() / @ref iotSocketRecv() can be used without specifying the peer.
 * 
 * @param[in] socket
 *   Socket identification number.
 * 
 * @param[in] ip
 *   Pointer to the remote IP address. Must be non-NULL.
 * 
 * @param[in] ip_len
 *   Length of the `ip` address in bytes. Must be 4 for IPv4 or 16 for IPv6.
 * 
 * @param[in] port
 *   Remote port number.
 * 
 * @return
 *   Returns 0 on success. On failure, returns one of the following error codes:
 *   - @ref IOT_SOCKET_ESOCK : Invalid socket.
 *   - @ref IOT_SOCKET_EINVAL : Invalid argument.
 *   - @ref IOT_SOCKET_EALREADY : Connection already in progress.
 *   - @ref IOT_SOCKET_EINPROGRESS : Operation in progress.
 *   - @ref IOT_SOCKET_EISCONN : Socket is already connected.
 *   - @ref IOT_SOCKET_ECONNREFUSED : Connection rejected by the peer.
 *   - @ref IOT_SOCKET_ECONNABORTED : Connection aborted locally.
 *   - @ref IOT_SOCKET_EADDRINUSE : Address already in use.
 *   - @ref IOT_SOCKET_ETIMEDOUT : Operation timed out.
 *   - @ref IOT_SOCKET_ERROR : Unspecified error.
 *
 * @retval 0                          Success.
 * @retval IOT_SOCKET_ESOCK           @p socket is not a valid descriptor.
 * @retval IOT_SOCKET_EINVAL          Invalid @p ip / @p ip_len.
 * @retval IOT_SOCKET_EALREADY        A previous connect attempt is still in progress.
 * @retval IOT_SOCKET_EINPROGRESS     Non-blocking connect in progress.
 * @retval IOT_SOCKET_EISCONN         Socket is already connected.
 * @retval IOT_SOCKET_ECONNREFUSED    Remote peer refused the connection.
 * @retval IOT_SOCKET_ECONNABORTED    Connection aborted locally.
 * @retval IOT_SOCKET_EADDRINUSE      Local address is already in use.
 * @retval IOT_SOCKET_ETIMEDOUT       Connection attempt timed out.
 * @retval IOT_SOCKET_ERROR           Unspecified stack error.
 *
 * @note Thread safety:
 * - Not thread-safe on the same descriptor; serialize concurrent access.
 *
 * @note Side effects:
 * - Allocates internal TCP state. For TLS-enabled sockets the handshake runs synchronously.
 *
 * @see iotSocketCreate(), iotSocketBind(), iotSocketSend(), iotSocketRecv(), iotSocketClose()
 *
 * @code{.c}
 * // Connect a TCP client socket to 192.168.0.100 on port 5001.
 * const uint8_t server_ip[4] = { 192, 168, 0, 100 };
 * int32_t       rc           = iotSocketConnect(sock, server_ip, sizeof(server_ip), 5001);
 * if (rc < 0) {
 *   printf("iotSocketConnect() failed, rc = %ld\r\n", (long)rc);
 *   iotSocketClose(sock);
 *   return rc;
 * }
 * @endcode
 ******************************************************************************/
extern int32_t iotSocketConnect(int32_t socket, const uint8_t *ip, uint32_t ip_len, uint16_t port);

/***************************************************************************/ 
/**
 * @brief
 *   Receive data from a socket.
 * 
 * @details
 *   The @ref iotSocketRecv() function receives data from a connected socket identified by `socket`. 
 *   The received data is stored in the buffer pointed to by `buf`, which has a length of `len` bytes.
 *
 * @pre Pre-conditions:
 * - @p socket must be connected using @ref iotSocketConnect() (TCP) or have a cached peer (UDP).
 *
 * @post Post-conditions:
 * - On success @p buf contains the received bytes and the return value is the number of bytes read.
 * 
 * @param[in] socket
 *   Socket identification number.
 * 
 * @param[out] buf
 *   Pointer to the buffer where the received data should be stored. Must be non-NULL unless @p len is 0.
 * 
 * @param[in] len
 *   Length of the buffer in bytes.
 * 
 * @return
 *   Returns the number of bytes received (>=0) on success. On failure, returns one of the following error codes:
 *   - 0: Data is available (when `len` is 0).
 *   - @ref IOT_SOCKET_ESOCK : Invalid socket.
 *   - @ref IOT_SOCKET_EINVAL : Invalid argument (pointer to buffer or length).
 *   - @ref IOT_SOCKET_ENOTCONN : Socket is not connected.
 *   - @ref IOT_SOCKET_ECONNRESET : Connection reset by the peer.
 *   - @ref IOT_SOCKET_ECONNABORTED : Connection aborted locally.
 *   - @ref IOT_SOCKET_EAGAIN : Operation would block or timed out (may be called again).
 *   - @ref IOT_SOCKET_ERROR : Unspecified error.
 *
 * @retval >0                         Number of bytes read into @p buf.
 * @retval 0                          Data is available (when @p len is 0) or peer performed orderly shutdown (TCP).
 * @retval IOT_SOCKET_ESOCK           @p socket is not a valid descriptor.
 * @retval IOT_SOCKET_EINVAL          @p buf is NULL.
 * @retval IOT_SOCKET_ENOTCONN        Socket is not connected.
 * @retval IOT_SOCKET_ECONNRESET      Peer reset the connection.
 * @retval IOT_SOCKET_ECONNABORTED    Connection aborted locally.
 * @retval IOT_SOCKET_EAGAIN          Non-blocking socket has no data available, or timeout elapsed.
 * @retval IOT_SOCKET_ERROR           Unspecified stack error.
 *
 * @note Thread safety:
 * - Not thread-safe on the same descriptor; serialize concurrent access.
 *
 * @note Side effects:
 * - Blocks the calling thread unless @ref IOT_SOCKET_SO_RCVTIMEO or @ref IOT_SOCKET_IO_FIONBIO is set.
 *
 * @see iotSocketRecvFrom(), iotSocketSend(), iotSocketConnect(), iotSocketSetOpt()
 *
 * @code{.c}
 * // Read up to 1460 bytes from a connected socket, looping until peer closes.
 * uint8_t buffer[1460];
 * int32_t n;
 *
 * while ((n = iotSocketRecv(sock, buffer, sizeof(buffer))) > 0) {
 *   // Process buffer[0 .. n - 1].
 * }
 * if (n == IOT_SOCKET_EAGAIN) {
 *   // Receive timed out - retry later.
 * } else if (n < 0) {
 *   printf("iotSocketRecv() failed, rc = %ld\r\n", (long)n);
 * }
 * @endcode
 ******************************************************************************/
extern int32_t iotSocketRecv(int32_t socket, void *buf, uint32_t len);

/***************************************************************************/ 
/**
 * @brief
 *   Receive data from a socket.
 * 
 * @details
 *   The @ref iotSocketRecvFrom() function receives data from a socket identified by `socket`. 
 *   The received data is stored in the buffer pointed to by `buf`, which has a length of `len` bytes. 
 *   If the `ip` and `port` parameters are provided, the function also returns the source address and port 
 *   of the received data.
 *
 * @pre Pre-conditions:
 * - @p socket must be bound using @ref iotSocketBind() (for UDP) or connected using @ref iotSocketConnect() (for TCP).
 *
 * @post Post-conditions:
 * - On success @p buf contains the received bytes and @p ip / @p port (when non-NULL) are populated with the source address.
 * 
 * @param[in] socket
 *   Socket identification number.
 * 
 * @param[out] buf
 *   Pointer to the buffer where the received data should be stored. Must be non-NULL unless @p len is 0.
 * 
 * @param[in] len
 *   Length of the buffer in bytes.
 * 
 * @param[out] ip
 *   Pointer to a buffer where the remote source address will be returned. If NULL, no address is returned.
 * 
 * @param[in, out] ip_len
 *   Pointer to the length of the `ip` buffer. On input, it should contain the size of the buffer. On output, 
 *   it will be modified to indicate the actual length of the address returned. If `ip` is NULL, `ip_len` should also be NULL.
 *   Must be non-NULL when @p ip is non-NULL.
 * 
 * @param[out] port
 *   Pointer to a buffer where the remote source port will be returned. If NULL, no port is returned.
 * 
 * @return
 *   Returns the number of bytes received (>=0) on success. On failure, returns one of the following error codes:
 *   - 0: Data is available (when `len` is 0).
 *   - @ref IOT_SOCKET_ESOCK : Invalid socket.
 *   - @ref IOT_SOCKET_EINVAL : Invalid argument (pointer to buffer or length).
 *   - @ref IOT_SOCKET_ENOTCONN : Socket is not connected.
 *   - @ref IOT_SOCKET_ECONNRESET : Connection reset by the peer.
 *   - @ref IOT_SOCKET_ECONNABORTED : Connection aborted locally.
 *   - @ref IOT_SOCKET_EAGAIN : Operation would block or timed out (may be called again).
 *   - @ref IOT_SOCKET_ERROR : Unspecified error.
 *
 * @retval >=0                        Number of bytes received.
 * @retval IOT_SOCKET_ESOCK           @p socket is not a valid descriptor.
 * @retval IOT_SOCKET_EINVAL          Invalid @p buf / @p len / @p ip_len.
 * @retval IOT_SOCKET_ENOTCONN        Socket is not connected (TCP only).
 * @retval IOT_SOCKET_ECONNRESET      Peer reset the connection.
 * @retval IOT_SOCKET_ECONNABORTED    Connection aborted locally.
 * @retval IOT_SOCKET_EAGAIN          Non-blocking socket has no data available, or timeout elapsed.
 * @retval IOT_SOCKET_ERROR           Unspecified stack error.
 *
 * @note Thread safety:
 * - Not thread-safe on the same descriptor; serialize concurrent access.
 *
 * @note Side effects:
 * - Blocks the calling thread unless @ref IOT_SOCKET_SO_RCVTIMEO or @ref IOT_SOCKET_IO_FIONBIO is set.
 * - Truncated UDP datagrams are silently discarded beyond @p len bytes.
 *
 * @see iotSocketRecv(), iotSocketSendTo(), iotSocketBind(), iotSocketSetOpt()
 *
 * @code{.c}
 * // Receive a UDP datagram and capture the sender's IPv4 address/port.
 * uint8_t  buffer[1472];
 * uint8_t  src_ip[4]    = { 0 };
 * uint32_t src_ip_len   = sizeof(src_ip);
 * uint16_t src_port     = 0;
 *
 * int32_t n = iotSocketRecvFrom(sock, buffer, sizeof(buffer),
 *                               src_ip, &src_ip_len, &src_port);
 * if (n > 0) {
 *   printf("Received %ld bytes from %u.%u.%u.%u:%u\r\n",
 *          (long)n, src_ip[0], src_ip[1], src_ip[2], src_ip[3], src_port);
 * } else if (n < 0) {
 *   printf("iotSocketRecvFrom() failed, rc = %ld\r\n", (long)n);
 * }
 * @endcode
 ******************************************************************************/
extern int32_t iotSocketRecvFrom(int32_t socket, void *buf, uint32_t len, uint8_t *ip, uint32_t *ip_len, uint16_t *port);

/***************************************************************************/ 
/**
 * @brief
 *   Send data or check if data can be sent on a connected socket.
 * 
 * @details
 *   The @ref iotSocketSend() function sends data from the buffer pointed to by `buf` through the socket identified by `socket`. 
 *   If `len` is 0, the function checks if data can be sent on the socket without blocking.
 *
 * @pre Pre-conditions:
 * - @p socket must be connected (TCP) or have a cached peer (UDP using @ref iotSocketConnect()).
 *
 * @post Post-conditions:
 * - On success the bytes have been handed to the stack for transmission.
 * 
 * @param[in] socket
 *   Socket identification number.
 * 
 * @param[in] buf
 *   Pointer to the buffer containing the data to send. Must be non-NULL unless @p len is 0.
 * 
 * @param[in] len
 *   Length of the data in bytes. Must not exceed 1460 bytes for plain TCP/UDP or 1370 bytes for TLS.
 * 
 * @return
 *   Returns the number of bytes sent (>=0) on success if `len` is not 0. If `len` is 0, returns 0 if data can be sent. 
 *   On failure, returns one of the following error codes:
 *   - @ref IOT_SOCKET_ESOCK : Invalid socket.
 *   - @ref IOT_SOCKET_EINVAL : Invalid argument (pointer to buffer or length).
 *   - @ref IOT_SOCKET_ENOTCONN : Socket is not connected.
 *   - @ref IOT_SOCKET_ECONNRESET : Connection reset by the peer.
 *   - @ref IOT_SOCKET_ECONNABORTED : Connection aborted locally.
 *   - @ref IOT_SOCKET_EAGAIN : Operation would block or timed out (may be called again).
 *   - @ref IOT_SOCKET_ERROR : Unspecified error.
 *
 * @retval >=0                        Number of bytes accepted by the stack for transmission.
 * @retval IOT_SOCKET_ESOCK           @p socket is not a valid descriptor.
 * @retval IOT_SOCKET_EINVAL          @p buf is NULL or @p len exceeds the limits described above.
 * @retval IOT_SOCKET_ENOTCONN        Socket is not connected.
 * @retval IOT_SOCKET_ECONNRESET      Peer reset the connection.
 * @retval IOT_SOCKET_ECONNABORTED    Connection aborted locally.
 * @retval IOT_SOCKET_EAGAIN          Non-blocking socket cannot accept the data now, or timeout elapsed.
 * @retval IOT_SOCKET_ERROR           Unspecified stack error.
 *
 * @note Thread safety:
 * - Not thread-safe on the same descriptor; serialize concurrent access.
 *
 * @note Side effects:
 * - Enqueues the payload in the firmware send queue; transmission happens asynchronously.
 *
 * @see iotSocketSendTo(), iotSocketRecv(), iotSocketConnect(), iotSocketSetOpt()
 * 
 * @note
 *   - The function does not guarantee that the packets are transmitted to the remote node; they are enqueued in the queue.
 *   - The function can only send a maximum of 1460 bytes in the case of plain TCP and UDP. For TLS, the maximum buffer length is 1370 bytes.
 *
 * @code{.c}
 * // Send a payload on a connected TCP socket, handling partial sends.
 * const uint8_t payload[] = "GET / HTTP/1.0\r\n\r\n";
 * uint32_t      remaining = (uint32_t)(sizeof(payload) - 1);
 * uint32_t      offset    = 0;
 *
 * while (remaining > 0) {
 *   int32_t sent = iotSocketSend(sock, payload + offset, remaining);
 *   if (sent < 0) {
 *     printf("iotSocketSend() failed, rc = %ld\r\n", (long)sent);
 *     break;
 *   }
 *   offset    += (uint32_t)sent;
 *   remaining -= (uint32_t)sent;
 * }
 * @endcode
 ******************************************************************************/
extern int32_t iotSocketSend(int32_t socket, const void *buf, uint32_t len);

/***************************************************************************/ 
/**
 * @brief
 *   Send data or check if data can be sent on a socket.
 * 
 * @details
 *   The @ref iotSocketSendTo() function sends data from the buffer pointed to by `buf` through the socket identified by `socket` 
 *   to the specified remote destination IP address and port. If `len` is 0, the function checks if data can be sent on the socket 
 *   without blocking.
 *
 * @pre Pre-conditions:
 * - @p socket must have been created using @ref iotSocketCreate().
 * - For a connected socket, @p ip / @p port are ignored.
 *
 * @post Post-conditions:
 * - On success the datagram has been handed to the stack for transmission.
 * 
 * @param[in] socket
 *   Socket identification number.
 * 
 * @param[in] buf
 *   Pointer to the buffer containing the data to send. Must be non-NULL unless @p len is 0.
 * 
 * @param[in] len
 *   Length of the data in bytes. Must not exceed 1460 bytes for plain TCP/UDP or 1370 bytes for TLS.
 * 
 * @param[in] ip
 *   Pointer to the remote destination IP address. Must be non-NULL for unconnected sockets.
 * 
 * @param[in] ip_len
 *   Length of the `ip` address in bytes. Must be 4 for IPv4 or 16 for IPv6.
 * 
 * @param[in] port
 *   Remote destination port number.
 * 
 * @return
 *   Returns the number of bytes sent (>=0) on success if `len` is not 0. If `len` is 0, returns 0 if data can be sent. 
 *   On failure, returns one of the following error codes:
 *   - @ref IOT_SOCKET_ESOCK : Invalid socket.
 *   - @ref IOT_SOCKET_EINVAL : Invalid argument (pointer to buffer or length).
 *   - @ref IOT_SOCKET_ENOTCONN : Socket is not connected.
 *   - @ref IOT_SOCKET_ECONNRESET : Connection reset by the peer.
 *   - @ref IOT_SOCKET_ECONNABORTED : Connection aborted locally.
 *   - @ref IOT_SOCKET_EAGAIN : Operation would block or timed out (may be called again).
 *   - @ref IOT_SOCKET_ERROR : Unspecified error.
 *
 * @retval >=0                        Number of bytes accepted by the stack for transmission.
 * @retval IOT_SOCKET_ESOCK           @p socket is not a valid descriptor.
 * @retval IOT_SOCKET_EINVAL          @p buf / @p ip / @p ip_len invalid, or @p len exceeds the limits.
 * @retval IOT_SOCKET_ECONNRESET      Peer reset the connection.
 * @retval IOT_SOCKET_ECONNABORTED    Connection aborted locally.
 * @retval IOT_SOCKET_EAGAIN          Non-blocking socket cannot accept the data now, or timeout elapsed.
 * @retval IOT_SOCKET_ERROR           Unspecified stack error (e.g. @p len exceeds the MSS).
 *
 * @note Thread safety:
 * - Not thread-safe on the same descriptor; serialize concurrent access.
 *
 * @note Side effects:
 * - Enqueues the datagram in the firmware send queue; transmission happens asynchronously.
 *
 * @see iotSocketSend(), iotSocketRecvFrom(), iotSocketBind(), iotSocketConnect()
 * 
 * @note
 *   - If the number of bytes to be sent exceeds the Maximum Segment Size (MSS) specified by the remote node, the API will return @ref IOT_SOCKET_ERROR.
 *     To know the MSS size for the socket, use the `sl_si91x_get_socket_mss()` utility. In the case of TCP, is should be called after @ref iotSocketConnect().
 *   - The function does not guarantee that the packets are transmitted to the remote node; they are enqueued in the queue.
 *   - The function can only send a maximum of 1460 bytes in the case of plain TCP or UDP. For TLS, the maximum buffer length is 1370 bytes.
 *
 * @code{.c}
 * // Send a UDP datagram to a specific remote IPv4 host.
 * const uint8_t dst_ip[4]    = { 192, 168, 0, 100 };
 * const uint8_t payload[]    = "ping";
 *
 * int32_t sent = iotSocketSendTo(sock,
 *                                payload,
 *                                (uint32_t)(sizeof(payload) - 1),
 *                                dst_ip,
 *                                sizeof(dst_ip),
 *                                5002);
 * if (sent < 0) {
 *   printf("iotSocketSendTo() failed, rc = %ld\r\n", (long)sent);
 * }
 * @endcode
 ******************************************************************************/
extern int32_t iotSocketSendTo(int32_t socket, const void *buf, uint32_t len, const uint8_t *ip, uint32_t ip_len, uint16_t port);

/***************************************************************************/ 
/**
 * @brief
 *   Retrieve local IP address and port of a socket.
 * 
 * @details
 *   The @ref iotSocketGetSockName() function retrieves the local IP address and port number associated with the socket 
 *   identified by `socket`. This function is typically used to obtain the local address and port assigned to a socket 
 *   after it has been bound or connected.
 *
 * @pre Pre-conditions:
 * - @p socket must be a descriptor returned by @ref iotSocketCreate() and should be bound or connected.
 *
 * @post Post-conditions:
 * - On success @p ip / @p port (when non-NULL) are populated with the local endpoint.
 * 
 * @param[in] socket
 *   Socket identification number.
 * 
 * @param[out] ip
 *   Pointer to a buffer where the local IP address will be returned. If NULL, no address is returned.
 * 
 * @param[in, out] ip_len
 *   Pointer to the length of the `ip` buffer. On input, it should contain the size of the buffer. On output, 
 *   it will be modified to indicate the actual length of the address returned. If `ip` is NULL, `ip_len` should also be NULL.
 *   Must be non-NULL when @p ip is non-NULL.
 * 
 * @param[out] port
 *   Pointer to a buffer where the local port number would be returned. If NULL, no port is returned.
 * 
 * @return
 *   Returns 0 on success. On failure, returns one of the following error codes:
 *   - @ref IOT_SOCKET_ESOCK : Invalid socket.
 *   - @ref IOT_SOCKET_EINVAL : Invalid argument (pointer to buffer or length).
 *   - @ref IOT_SOCKET_ERROR : Unspecified error.
 *
 * @retval 0                          Success.
 * @retval IOT_SOCKET_ESOCK           @p socket is not a valid descriptor.
 * @retval IOT_SOCKET_EINVAL          @p ip_len too small or invalid.
 * @retval IOT_SOCKET_ERROR           Unspecified stack error.
 *
 * @note Thread safety:
 * - Safe to call concurrently with I/O on the same descriptor; read-only access to socket state.
 *
 * @note Side effects:
 * - None. Does not modify socket behavior.
 *
 * @see iotSocketGetPeerName(), iotSocketBind(), iotSocketCreate()
 *
 * @code{.c}
 * // Retrieve the local address/port bound to the socket.
 * uint8_t  local_ip[4]  = { 0 };
 * uint32_t local_ip_len = sizeof(local_ip);
 * uint16_t local_port   = 0;
 *
 * int32_t rc = iotSocketGetSockName(sock, local_ip, &local_ip_len, &local_port);
 * if (rc == 0) {
 *   printf("Local endpoint %u.%u.%u.%u:%u\r\n",
 *          local_ip[0], local_ip[1], local_ip[2], local_ip[3], local_port);
 * }
 * @endcode
 ******************************************************************************/
extern int32_t iotSocketGetSockName(int32_t socket, uint8_t *ip, uint32_t *ip_len, uint16_t *port);

/***************************************************************************/ 
/**
 * @brief
 *   Retrieve remote IP address and port of a socket.
 * 
 * @details
 *   The @ref iotSocketGetPeerName() function retrieves the remote IP address and port number associated with the socket 
 *   identified by `socket`. This function is typically used to obtain the address and port of the peer to which the socket 
 *   is connected.
 *
 * @pre Pre-conditions:
 * - @p socket must be a connected descriptor returned by @ref iotSocketConnect() or @ref iotSocketAccept().
 *
 * @post Post-conditions:
 * - On success @p ip / @p port (when non-NULL) are populated with the peer's endpoint.
 * 
 * @param[in] socket
 *   Socket identification number.
 * 
 * @param[out] ip
 *   Pointer to a buffer where the remote IP address would be returned. If NULL, no address is returned.
 * 
 * @param[in, out] ip_len
 *   Pointer to the length of the `ip` buffer. On input, it should contain the size of the buffer. On output, 
 *   it will be modified to indicate the actual length of the address returned. If `ip` is NULL, `ip_len` should also be NULL.
 *   Must be non-NULL when @p ip is non-NULL.
 * 
 * @param[out] port
 *   Pointer to a buffer where the remote port number would be returned. If NULL, no port is returned.
 * 
 * @return
 *   Returns 0 on success. On failure, returns one of the following error codes:
 *   - @ref IOT_SOCKET_ESOCK : Invalid socket.
 *   - @ref IOT_SOCKET_EINVAL : Invalid argument (pointer to buffer or length).
 *   - @ref IOT_SOCKET_ENOTCONN : Socket is not connected.
 *   - @ref IOT_SOCKET_ERROR : Unspecified error.
 *
 * @retval 0                          Success.
 * @retval IOT_SOCKET_ESOCK           @p socket is not a valid descriptor.
 * @retval IOT_SOCKET_EINVAL          @p ip_len too small or invalid.
 * @retval IOT_SOCKET_ENOTCONN        Socket is not connected.
 * @retval IOT_SOCKET_ERROR           Unspecified stack error.
 *
 * @note Thread safety:
 * - Safe to call concurrently with I/O on the same descriptor; read-only access to socket state.
 *
 * @note Side effects:
 * - None. Does not modify socket behavior.
 *
 * @see iotSocketGetSockName(), iotSocketConnect(), iotSocketAccept()
 *
 * @code{.c}
 * // Fetch the connected peer's IPv4 address and port.
 * uint8_t  peer_ip[4]  = { 0 };
 * uint32_t peer_ip_len = sizeof(peer_ip);
 * uint16_t peer_port   = 0;
 *
 * if (iotSocketGetPeerName(sock, peer_ip, &peer_ip_len, &peer_port) == 0) {
 *   printf("Connected to %u.%u.%u.%u:%u\r\n",
 *          peer_ip[0], peer_ip[1], peer_ip[2], peer_ip[3], peer_port);
 * }
 * @endcode
 ******************************************************************************/
extern int32_t iotSocketGetPeerName(int32_t socket, uint8_t *ip, uint32_t *ip_len, uint16_t *port);

/***************************************************************************/ 
/**
 * @brief
 *   Retrieve socket option.
 * 
 * @details
 *   The @ref iotSocketGetOpt() function retrieves the value of a socket option specified by `opt_id` for the socket 
 *   identified by `socket`. The option value is stored in the buffer pointed to by `opt_val`, and the length of the 
 *   option value is specified by `opt_len`.
 *
 * @pre Pre-conditions:
 * - @p socket must be a descriptor returned by @ref iotSocketCreate().
 *
 * @post Post-conditions:
 * - On success @p opt_val is populated and @p opt_len is updated with the number of bytes written.
 * 
 * @param[in] socket
 *   Socket identification number.
 * 
 * @param[in] opt_id
 *   Option identifier. One of the values from @ref IOT_SOCKET_OPTION_ID.
 * 
 * @param[out] opt_val
 *   Pointer to the buffer that would receive the option value. Must be non-NULL.
 * 
 * @param[in,out] opt_len
 *   Pointer to the length of the option value:
 *   - On input, it specifies the length of the buffer.
 *   - On output, it specifies the length of the data returned.
 *   Must be non-NULL and initially >= the size required by @p opt_id.
 * 
 * @return
 *   Returns 0 on success. On failure, returns one of the following error codes:
 *   - @ref IOT_SOCKET_ESOCK : Invalid socket.
 *   - @ref IOT_SOCKET_EINVAL : Invalid argument.
 *   - @ref IOT_SOCKET_ENOTSUP : Operation not supported.
 *   - @ref IOT_SOCKET_ERROR : Unspecified error.
 *
 * @retval 0                          Success.
 * @retval IOT_SOCKET_ESOCK           @p socket is not a valid descriptor.
 * @retval IOT_SOCKET_EINVAL          @p opt_val / @p opt_len invalid.
 * @retval IOT_SOCKET_ENOTSUP         @p opt_id is not supported.
 * @retval IOT_SOCKET_ERROR           Unspecified stack error.
 *
 * @note Thread safety:
 * - Not thread-safe on the same descriptor; serialize concurrent access.
 *
 * @note Side effects:
 * - None. Does not modify socket behavior.
 *
 * @see iotSocketSetOpt()
 * 
 * @note
 *   The following options are currently supported:
 *   - @ref IOT_SOCKET_IO_FIONBIO
 *   - @ref IOT_SOCKET_SO_RCVTIMEO
 *   - @ref IOT_SOCKET_SO_SNDTIMEO
 *   - @ref IOT_SOCKET_SO_KEEPALIVE
 *   - @ref IOT_SOCKET_SO_TYPE
 *   - @ref IOT_SOCKET_SO_SSL_ENABLE (Silicon Labs vendor extension; non-portable)
 *
 * @code{.c}
 * // Query the current receive timeout of a socket.
 * uint32_t timeout_ms = 0;
 * uint32_t opt_len    = sizeof(timeout_ms);
 *
 * if (iotSocketGetOpt(sock, IOT_SOCKET_SO_RCVTIMEO, &timeout_ms, &opt_len) == 0) {
 *   printf("Current RX timeout: %lu ms\r\n", (unsigned long)timeout_ms);
 * }
 * @endcode
 ******************************************************************************/
extern int32_t iotSocketGetOpt(int32_t socket, int32_t opt_id, void *opt_val, uint32_t *opt_len);

/***************************************************************************/ 
/**
 * @brief
 *   Set socket option.
 * 
 * @details
 *   The @ref iotSocketSetOpt() function sets the value of a socket option specified by `opt_id` for the socket 
 *   identified by `socket`. The option value is provided in the buffer pointed to by `opt_val`, and the length of the 
 *   option value is specified by `opt_len`.
 *
 * @pre Pre-conditions:
 * - @p socket must be a descriptor returned by @ref iotSocketCreate().
 *
 * @post Post-conditions:
 * - On success the option has been applied. Subsequent calls on the socket observe the new behavior.
 * 
 * @param[in] socket
 *   Socket identification number.
 * 
 * @param[in] opt_id
 *   Option identifier. One of the values from @ref IOT_SOCKET_OPTION_ID.
 * 
 * @param[in] opt_val
 *   Pointer to the buffer containing the option value. Must be non-NULL.
 * 
 * @param[in] opt_len
 *   Length of the option value in bytes. Must match the type expected by @p opt_id.
 * 
 * @return
 *   Returns 0 on success. On failure, returns one of the following error codes:
 *   - @ref IOT_SOCKET_ESOCK : Invalid socket.
 *   - @ref IOT_SOCKET_EINVAL : Invalid argument.
 *   - @ref IOT_SOCKET_ENOTSUP : Operation not supported.
 *   - @ref IOT_SOCKET_ERROR : Unspecified error.
 *
 * @retval 0                          Success.
 * @retval IOT_SOCKET_ESOCK           @p socket is not a valid descriptor.
 * @retval IOT_SOCKET_EINVAL          @p opt_val is NULL or @p opt_len is wrong.
 * @retval IOT_SOCKET_ENOTSUP         @p opt_id is not supported.
 * @retval IOT_SOCKET_ERROR           Unspecified stack error.
 *
 * @note Thread safety:
 * - Not thread-safe on the same descriptor; serialize concurrent access.
 *
 * @note Side effects:
 * - Changes internal state of the socket (non-blocking mode, timeouts, keepalive).
 *
 * @see iotSocketGetOpt()
 * 
 * @note
 *   The following options are currently supported:
 *   - @ref IOT_SOCKET_IO_FIONBIO
 *   - @ref IOT_SOCKET_SO_RCVTIMEO
 *   - @ref IOT_SOCKET_SO_SNDTIMEO
 *   - @ref IOT_SOCKET_SO_KEEPALIVE
 *   - @ref IOT_SOCKET_SO_TYPE
 *   - @ref IOT_SOCKET_SO_SSL_ENABLE (Silicon Labs vendor extension; non-portable)
 *
 * @code{.c}
 * // 1. Enable non-blocking mode on the socket.
 * uint32_t nbio = 1;
 * (void)iotSocketSetOpt(sock, IOT_SOCKET_IO_FIONBIO, &nbio, sizeof(nbio));
 *
 * // 2. Set a 2-second receive timeout (value is in milliseconds).
 * uint32_t rx_timeout_ms = 2000;
 * if (iotSocketSetOpt(sock, IOT_SOCKET_SO_RCVTIMEO,
 *                     &rx_timeout_ms, sizeof(rx_timeout_ms)) < 0) {
 *   printf("iotSocketSetOpt(RCVTIMEO) failed\r\n");
 * }
 *
 * // 3. Turn on TCP keepalive.
 * uint32_t keepalive = 1;
 * (void)iotSocketSetOpt(sock, IOT_SOCKET_SO_KEEPALIVE,
 *                       &keepalive, sizeof(keepalive));
 *
 * // 4. (Silicon Labs vendor extension) Enable TLS 1.2 on a stream socket
 * //    BEFORE iotSocketConnect(). See IOT_SOCKET_SO_SSL_ENABLE for details.
 * #ifdef IOT_SOCKET_VENDOR_SILABS
 * const char tls_version[] = "tls_1_2";
 * (void)iotSocketSetOpt(sock, IOT_SOCKET_SO_SSL_ENABLE,
 *                       tls_version, sizeof(tls_version));
 * #endif
 * @endcode
 ******************************************************************************/
extern int32_t iotSocketSetOpt(int32_t socket, int32_t opt_id, const void *opt_val, uint32_t opt_len);

/***************************************************************************/ 
/**
 * @brief
 *   Close and release a socket.
 * 
 * @details
 *   The @ref iotSocketClose() function closes the socket identified by `socket` and releases any resources associated with it. 
 *   If the socket is a server socket or the first client socket, closing it may also close other related sockets.
 *
 * @pre Pre-conditions:
 * - @p socket must be a descriptor returned by @ref iotSocketCreate() or @ref iotSocketAccept().
 *
 * @post Post-conditions:
 * - The descriptor is released back to the pool and must not be used for further operations.
 * - Any pending I/O on the descriptor is aborted; queued unsent data may be discarded.
 * 
 * @param[in] socket
 *   Socket identification number.
 * 
 * @return
 *   Returns 0 on success. On failure, returns one of the following error codes:
 *   - @ref IOT_SOCKET_ESOCK : Invalid socket.
 *   - @ref IOT_SOCKET_EAGAIN : Operation would block (may be called again).
 *   - @ref IOT_SOCKET_ERROR : Unspecified error.
 *
 * @retval 0                          Success.
 * @retval IOT_SOCKET_ESOCK           @p socket is not a valid descriptor.
 * @retval IOT_SOCKET_EAGAIN          Close would block; retry later.
 * @retval IOT_SOCKET_ERROR           Unspecified stack error.
 *
 * @note Thread safety:
 * - Not thread-safe on the same descriptor. Caller must ensure no other thread is operating on @p socket when @ref iotSocketClose() is invoked.
 *
 * @note Side effects:
 * - Releases stack resources (TCP state, internal buffers, pending timers).
 * - Closing a server or first client socket may close related sockets (see note).
 *
 * @see iotSocketCreate(), iotSocketAccept()
 * 
 * @note
 *   Calling close on a server or the first client socket may result in closing other related sockets as well.
 *
 * @code{.c}
 * // Always close a socket in every exit path (including errors) to avoid leaks.
 * int32_t rc = iotSocketClose(sock);
 * if (rc < 0) {
 *   printf("iotSocketClose() failed, rc = %ld\r\n", (long)rc);
 * }
 * sock = -1;
 * @endcode
 ******************************************************************************/
extern int32_t iotSocketClose(int32_t socket);

/***************************************************************************/ 
/**
 * @brief
 *   Retrieve host IP address from host name.
 * 
 * @details
 *   The @ref iotSocketGetHostByName() function resolves a host name to an IP address. The resolved IP address is stored 
 *   in the buffer pointed to by `ip`, and the length of the IP address is specified by `ip_len`. The address family is 
 *   specified by `af`.
 *
 * @pre Pre-conditions:
 * - The network stack must be initialized and a network profile must be up.
 * - A reachable DNS server must be configured.
 *
 * @post Post-conditions:
 * - On success @p ip is populated with the resolved address and @p ip_len is updated to reflect the returned length.
 * 
 * @param[in] name
 *   Host name to be resolved. Must be a non-NULL, null-terminated string.
 * 
 * @param[in] af
 *   Address family. One of the values from @ref IOT_SOCKET_ADDRESS_FAMILY.
 * 
 * @param[out] ip
 *   Pointer to the buffer where the resolved IP address will be returned. Must be non-NULL.
 * 
 * @param[in,out] ip_len
 *   Pointer to the length of the `ip` buffer:
 *   - On input, it specifies the length of the supplied `ip` buffer.
 *   - On output, it specifies the length of the stored `ip` address.
 *   Must be non-NULL and initially >= 4 for IPv4 or 16 for IPv6.
 * 
 * @return
 *   Returns 0 on success. On failure, returns one of the following error codes:
 *   - @ref IOT_SOCKET_EINVAL : Invalid argument.
 *   - @ref IOT_SOCKET_ENOTSUP : Operation not supported.
 *   - @ref IOT_SOCKET_ETIMEDOUT : Operation timed out.
 *   - @ref IOT_SOCKET_EHOSTNOTFOUND : Host not found.
 *   - @ref IOT_SOCKET_ERROR : Unspecified error.
 *
 * @retval 0                          Success.
 * @retval IOT_SOCKET_EINVAL          @p name, @p ip or @p ip_len is invalid.
 * @retval IOT_SOCKET_ENOTSUP         @p af is not supported.
 * @retval IOT_SOCKET_ETIMEDOUT       DNS lookup timed out.
 * @retval IOT_SOCKET_EHOSTNOTFOUND   No record exists for @p name.
 * @retval IOT_SOCKET_ERROR           Unspecified stack error.
 *
 * @note Thread safety:
 * - Thread-safe; may be called concurrently from multiple threads.
 *
 * @note Side effects:
 * - Blocks the caller for the duration of the DNS transaction (subject to the stack's DNS timeout).
 *
 * @see iotSocketConnect(), iotSocketSendTo()
 *
 * @code{.c}
 * // Resolve an IPv4 host name (requires an active network).
 * uint8_t  resolved_ip[4] = { 0 };
 * uint32_t ip_len         = sizeof(resolved_ip);
 *
 * int32_t rc = iotSocketGetHostByName("example.com",
 *                                     IOT_SOCKET_AF_INET,
 *                                     resolved_ip,
 *                                     &ip_len);
 * if (rc == 0) {
 *   printf("example.com -> %u.%u.%u.%u\r\n",
 *          resolved_ip[0], resolved_ip[1], resolved_ip[2], resolved_ip[3]);
 * } else {
 *   printf("DNS resolve failed, rc = %ld\r\n", (long)rc);
 * }
 * @endcode
 ******************************************************************************/
extern int32_t iotSocketGetHostByName(const char *name, int32_t af, uint8_t *ip, uint32_t *ip_len);

/** @} */

#ifdef  __cplusplus
}
#endif

#endif /* IOT_SOCKET_H */
