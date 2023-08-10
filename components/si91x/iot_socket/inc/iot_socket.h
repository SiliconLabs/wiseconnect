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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**** Address Family definitions ****/
#define IOT_SOCKET_AF_INET  1 ///< IPv4
#define IOT_SOCKET_AF_INET6 2 ///< IPv6

/**** Socket Type definitions ****/
#define IOT_SOCKET_SOCK_STREAM 1 ///< Stream socket
#define IOT_SOCKET_SOCK_DGRAM  2 ///< Datagram socket

/**** Socket Protocol definitions ****/
#define IOT_SOCKET_IPPROTO_TCP 1 ///< TCP
#define IOT_SOCKET_IPPROTO_UDP 2 ///< UDP

/**** Socket Option definitions ****/
#define IOT_SOCKET_IO_FIONBIO \
  1 ///< Non-blocking I/O (Set only, default = 0); opt_val = &nbio, opt_len = sizeof(nbio), nbio (integer): 0=blocking, non-blocking otherwise
#define IOT_SOCKET_SO_RCVTIMEO 2 ///< Receive timeout in ms (default = 0); opt_val = &timeout, opt_len = sizeof(timeout)
#define IOT_SOCKET_SO_SNDTIMEO 3 ///< Send timeout in ms (default = 0); opt_val = &timeout, opt_len = sizeof(timeout)
#define IOT_SOCKET_SO_KEEPALIVE \
  4 ///< Keep-alive messages (default = 0); opt_val = &keepalive, opt_len = sizeof(keepalive), keepalive (integer): 0=disabled, enabled otherwise
#define IOT_SOCKET_SO_TYPE \
  5 ///< Socket Type (Get only); opt_val = &socket_type, opt_len = sizeof(socket_type), socket_type (integer): IOT_SOCKET_SOCK_xxx

/**** Socket Return Codes ****/
#define IOT_SOCKET_ERROR         (-1)  ///< Unspecified error
#define IOT_SOCKET_ESOCK         (-2)  ///< Invalid socket
#define IOT_SOCKET_EINVAL        (-3)  ///< Invalid argument
#define IOT_SOCKET_ENOTSUP       (-4)  ///< Operation not supported
#define IOT_SOCKET_ENOMEM        (-5)  ///< Not enough memory
#define IOT_SOCKET_EAGAIN        (-6)  ///< Operation would block or timed out
#define IOT_SOCKET_EINPROGRESS   (-7)  ///< Operation in progress
#define IOT_SOCKET_ETIMEDOUT     (-8)  ///< Operation timed out
#define IOT_SOCKET_EISCONN       (-9)  ///< Socket is connected
#define IOT_SOCKET_ENOTCONN      (-10) ///< Socket is not connected
#define IOT_SOCKET_ECONNREFUSED  (-11) ///< Connection rejected by the peer
#define IOT_SOCKET_ECONNRESET    (-12) ///< Connection reset by the peer
#define IOT_SOCKET_ECONNABORTED  (-13) ///< Connection aborted locally
#define IOT_SOCKET_EALREADY      (-14) ///< Connection already in progress
#define IOT_SOCKET_EADDRINUSE    (-15) ///< Address in use
#define IOT_SOCKET_EHOSTNOTFOUND (-16) ///< Host not found

/**
  \brief         Create a communication socket.
  \param[in]     af       address family.
  \param[in]     type     socket type.
  \param[in]     protocol socket protocol.
  \return        status information:
                 - Socket identification number (>=0).
                 - \ref IOT_SOCKET_EINVAL        = Invalid argument.
                 - \ref IOT_SOCKET_ENOTSUP       = Operation not supported.
                 - \ref IOT_SOCKET_ENOMEM        = Not enough memory.
                 - \ref IOT_SOCKET_ERROR         = Unspecified error.
 */
extern int32_t iotSocketCreate(int32_t af, int32_t type, int32_t protocol);

/**
  \brief         Assign a local address to a socket.
  \param[in]     socket   socket identification number.
  \param[in]     ip       pointer to local IP address.
  \param[in]     ip_len   length of 'ip' address in bytes.
  \param[in]     port     local port number.
  \return        status information:
                 - 0                             = Operation successful.
                 - \ref IOT_SOCKET_ESOCK         = Invalid socket.
                 - \ref IOT_SOCKET_EINVAL        = Invalid argument (address or socket already bound).
                 - \ref IOT_SOCKET_EADDRINUSE    = Address already in use.
                 - \ref IOT_SOCKET_ERROR         = Unspecified error.
 */
extern int32_t iotSocketBind(int32_t socket, const uint8_t *ip, uint32_t ip_len, uint16_t port);

/**
  \brief         Listen for socket connections.
  \param[in]     socket   socket identification number.
  \param[in]     backlog  number of connection requests that can be queued.
  \return        status information:
                 - 0                             = Operation successful.
                 - \ref IOT_SOCKET_ESOCK         = Invalid socket.
                 - \ref IOT_SOCKET_EINVAL        = Invalid argument (socket not bound).
                 - \ref IOT_SOCKET_ENOTSUP       = Operation not supported.
                 - \ref IOT_SOCKET_EISCONN       = Socket is already connected.
                 - \ref IOT_SOCKET_ERROR         = Unspecified error.
 */
extern int32_t iotSocketListen(int32_t socket, int32_t backlog);

/**
  \brief         Accept a new connection on a socket.
  \param[in]     socket   socket identification number.
  \param[out]    ip       pointer to buffer where address of connecting socket shall be returned (NULL for none).
  \param[in,out] ip_len   pointer to length of 'ip' (or NULL if 'ip' is NULL):
                 - length of supplied 'ip' on input.
                 - length of stored 'ip' on output.
  \param[out]    port     pointer to buffer where port of connecting socket shall be returned (NULL for none).
  \return        status information:
                 - socket identification number of accepted socket (>=0).
                 - \ref IOT_SOCKET_ESOCK         = Invalid socket.
                 - \ref IOT_SOCKET_EINVAL        = Invalid argument (socket not in listen mode).
                 - \ref IOT_SOCKET_ENOTSUP       = Operation not supported (socket type does not support accepting connections).
                 - \ref IOT_SOCKET_ECONNRESET    = Connection reset by the peer.
                 - \ref IOT_SOCKET_ECONNABORTED  = Connection aborted locally.
                 - \ref IOT_SOCKET_EAGAIN        = Operation would block or timed out (may be called again).
                 - \ref IOT_SOCKET_ERROR         = Unspecified error.
 */
extern int32_t iotSocketAccept(int32_t socket, uint8_t *ip, uint32_t *ip_len, uint16_t *port);

/**
  \brief         Connect a socket to a remote host.
  \param[in]     socket   socket identification number.
  \param[in]     ip       pointer to remote IP address.
  \param[in]     ip_len   length of 'ip' address in bytes.
  \param[in]     port     remote port number.
  \return        status information:
                 - 0                             = Operation successful.
                 - \ref IOT_SOCKET_ESOCK         = Invalid socket.
                 - \ref IOT_SOCKET_EINVAL        = Invalid argument.
                 - \ref IOT_SOCKET_EALREADY      = Connection already in progress.
                 - \ref IOT_SOCKET_EINPROGRESS   = Operation in progress.
                 - \ref IOT_SOCKET_EISCONN       = Socket is connected.
                 - \ref IOT_SOCKET_ECONNREFUSED  = Connection rejected by the peer.
                 - \ref IOT_SOCKET_ECONNABORTED  = Connection aborted locally.
                 - \ref IOT_SOCKET_EADDRINUSE    = Address already in use.
                 - \ref IOT_SOCKET_ETIMEDOUT     = Operation timed out.
                 - \ref IOT_SOCKET_ERROR         = Unspecified error.
 */
extern int32_t iotSocketConnect(int32_t socket, const uint8_t *ip, uint32_t ip_len, uint16_t port);

/**
  \brief         Receive data or check if data is available on a connected socket.
  \param[in]     socket   socket identification number.
  \param[out]    buf      pointer to buffer where data should be stored.
  \param[in]     len      length of buffer (in bytes), set len = 0 to check if data is available.
  \return        status information:
                 - number of bytes received (>=0), if len != 0.
                 - 0                             = Data is available (len = 0).
                 - \ref IOT_SOCKET_ESOCK         = Invalid socket.
                 - \ref IOT_SOCKET_EINVAL        = Invalid argument (pointer to buffer or length).
                 - \ref IOT_SOCKET_ENOTCONN      = Socket is not connected.
                 - \ref IOT_SOCKET_ECONNRESET    = Connection reset by the peer.
                 - \ref IOT_SOCKET_ECONNABORTED  = Connection aborted locally.
                 - \ref IOT_SOCKET_EAGAIN        = Operation would block or timed out (may be called again).
                 - \ref IOT_SOCKET_ERROR         = Unspecified error.
 */
extern int32_t iotSocketRecv(int32_t socket, void *buf, uint32_t len);

/**
  \brief         Receive data or check if data is available on a socket.
  \param[in]     socket   socket identification number.
  \param[out]    buf      pointer to buffer where data should be stored.
  \param[in]     len      length of buffer (in bytes), set len = 0 to check if data is available.
  \param[out]    ip       pointer to buffer where remote source address shall be returned (NULL for none).
  \param[in,out] ip_len   pointer to length of 'ip' (or NULL if 'ip' is NULL):
                 - length of supplied 'ip' on input.
                 - length of stored 'ip' on output.
  \param[out]    port     pointer to buffer where remote source port shall be returned (NULL for none).
  \return        status information:
                 - number of bytes received (>=0), if len != 0.
                 - 0                             = Data is available (len = 0).
                 - \ref IOT_SOCKET_ESOCK         = Invalid socket.
                 - \ref IOT_SOCKET_EINVAL        = Invalid argument (pointer to buffer or length).
                 - \ref IOT_SOCKET_ENOTCONN      = Socket is not connected.
                 - \ref IOT_SOCKET_ECONNRESET    = Connection reset by the peer.
                 - \ref IOT_SOCKET_ECONNABORTED  = Connection aborted locally.
                 - \ref IOT_SOCKET_EAGAIN        = Operation would block or timed out (may be called again).
                 - \ref IOT_SOCKET_ERROR         = Unspecified error.
 */
extern int32_t iotSocketRecvFrom(int32_t socket,
                                 void *buf,
                                 uint32_t len,
                                 uint8_t *ip,
                                 uint32_t *ip_len,
                                 uint16_t *port);

/**
  \brief         Send data or check if data can be sent on a connected socket.
  \param[in]     socket   socket identification number.
  \param[in]     buf      pointer to buffer containing data to send.
  \param[in]     len      length of data (in bytes), set len = 0 to check if data can be sent.
  \return        status information:
                 - number of bytes sent (>=0), if len != 0.
                 - 0                             = Data can be sent (len = 0).
                 - \ref IOT_SOCKET_ESOCK         = Invalid socket.
                 - \ref IOT_SOCKET_EINVAL        = Invalid argument (pointer to buffer or length).
                 - \ref IOT_SOCKET_ENOTCONN      = Socket is not connected.
                 - \ref IOT_SOCKET_ECONNRESET    = Connection reset by the peer.
                 - \ref IOT_SOCKET_ECONNABORTED  = Connection aborted locally.
                 - \ref IOT_SOCKET_EAGAIN        = Operation would block or timed out (may be called again).
                 - \ref IOT_SOCKET_ERROR         = Unspecified error.
 */
extern int32_t iotSocketSend(int32_t socket, const void *buf, uint32_t len);

/**
  \brief         Send data or check if data can be sent on a socket.
  \param[in]     socket   socket identification number.
  \param[in]     buf      pointer to buffer containing data to send.
  \param[in]     len      length of data (in bytes), set len = 0 to check if data can be sent.
  \param[in]     ip       pointer to remote destination IP address.
  \param[in]     ip_len   length of 'ip' address in bytes.
  \param[in]     port     remote destination port number.
  \return        status information:
                 - number of bytes sent (>=0), if len != 0.
                 - 0                             = Data can be sent (len = 0).
                 - \ref IOT_SOCKET_ESOCK         = Invalid socket.
                 - \ref IOT_SOCKET_EINVAL        = Invalid argument (pointer to buffer or length).
                 - \ref IOT_SOCKET_ENOTCONN      = Socket is not connected.
                 - \ref IOT_SOCKET_ECONNRESET    = Connection reset by the peer.
                 - \ref IOT_SOCKET_ECONNABORTED  = Connection aborted locally.
                 - \ref IOT_SOCKET_EAGAIN        = Operation would block or timed out (may be called again).
                 - \ref IOT_SOCKET_ERROR         = Unspecified error.
 */
extern int32_t iotSocketSendTo(int32_t socket,
                               const void *buf,
                               uint32_t len,
                               const uint8_t *ip,
                               uint32_t ip_len,
                               uint16_t port);

/**
  \brief         Retrieve local IP address and port of a socket.
  \param[in]     socket   socket identification number.
  \param[out]    ip       pointer to buffer where local address shall be returned (NULL for none).
  \param[in,out] ip_len   pointer to length of 'ip' (or NULL if 'ip' is NULL):
                 - length of supplied 'ip' on input.
                 - length of stored 'ip' on output.
  \param[out]    port     pointer to buffer where local port shall be returned (NULL for none).
  \return        status information:
                 - 0                             = Operation successful.
                 - \ref IOT_SOCKET_ESOCK         = Invalid socket.
                 - \ref IOT_SOCKET_EINVAL        = Invalid argument (pointer to buffer or length).
                 - \ref IOT_SOCKET_ERROR         = Unspecified error.
 */
extern int32_t iotSocketGetSockName(int32_t socket, uint8_t *ip, uint32_t *ip_len, uint16_t *port);

/**
  \brief         Retrieve remote IP address and port of a socket.
  \param[in]     socket   socket identification number.
  \param[out]    ip       pointer to buffer where remote address shall be returned (NULL for none).
  \param[in,out] ip_len   pointer to length of 'ip' (or NULL if 'ip' is NULL):
                 - length of supplied 'ip' on input.
                 - length of stored 'ip' on output.
  \param[out]    port     pointer to buffer where remote port shall be returned (NULL for none).
  \return        status information:
                 - 0                             = Operation successful.
                 - \ref IOT_SOCKET_ESOCK         = Invalid socket.
                 - \ref IOT_SOCKET_EINVAL        = Invalid argument (pointer to buffer or length).
                 - \ref IOT_SOCKET_ENOTCONN      = Socket is not connected.
                 - \ref IOT_SOCKET_ERROR         = Unspecified error.
 */
extern int32_t iotSocketGetPeerName(int32_t socket, uint8_t *ip, uint32_t *ip_len, uint16_t *port);

/**
  \brief         Get socket option.
  \param[in]     socket   socket identification number.
  \param[in]     opt_id   option identifier.
  \param[out]    opt_val  pointer to the buffer that will receive the option value.
  \param[in,out] opt_len  pointer to length of the option value:
                 - length of buffer on input.
                 - length of data on output.
  \return        status information:
                 - 0                             = Operation successful.
                 - \ref IOT_SOCKET_ESOCK         = Invalid socket.
                 - \ref IOT_SOCKET_EINVAL        = Invalid argument.
                 - \ref IOT_SOCKET_ENOTSUP       = Operation not supported.
                 - \ref IOT_SOCKET_ERROR         = Unspecified error.
 */
extern int32_t iotSocketGetOpt(int32_t socket, int32_t opt_id, void *opt_val, uint32_t *opt_len);

/**
  \brief         Set socket option.
  \param[in]     socket   socket identification number.
  \param[in]     opt_id   option identifier.
  \param[in]     opt_val  pointer to the option value.
  \param[in]     opt_len  length of the option value in bytes.
  \return        status information:
                 - 0                             = Operation successful.
                 - \ref IOT_SOCKET_ESOCK         = Invalid socket.
                 - \ref IOT_SOCKET_EINVAL        = Invalid argument.
                 - \ref IOT_SOCKET_ENOTSUP       = Operation not supported.
                 - \ref IOT_SOCKET_ERROR         = Unspecified error.
 */
extern int32_t iotSocketSetOpt(int32_t socket, int32_t opt_id, const void *opt_val, uint32_t opt_len);

/**
  \brief         Close and release a socket.
  \param[in]     socket   socket identification number.
  \return        status information:
                 - 0                             = Operation successful.
                 - \ref IOT_SOCKET_ESOCK         = Invalid socket.
                 - \ref IOT_SOCKET_EAGAIN        = Operation would block (may be called again).
                 - \ref IOT_SOCKET_ERROR         = Unspecified error.
 */
extern int32_t iotSocketClose(int32_t socket);

/**
  \brief         Retrieve host IP address from host name.
  \param[in]     name     host name.
  \param[in]     af       address family.
  \param[out]    ip       pointer to buffer where resolved IP address shall be returned.
  \param[in,out] ip_len   pointer to length of 'ip':
                 - length of supplied 'ip' on input.
                 - length of stored 'ip' on output.
  \return        status information:
                 - 0                             = Operation successful.
                 - \ref IOT_SOCKET_EINVAL        = Invalid argument.
                 - \ref IOT_SOCKET_ENOTSUP       = Operation not supported.
                 - \ref IOT_SOCKET_ETIMEDOUT     = Operation timed out.
                 - \ref IOT_SOCKET_EHOSTNOTFOUND = Host not found.
                 - \ref IOT_SOCKET_ERROR         = Unspecified error.
 */
extern int32_t iotSocketGetHostByName(const char *name, int32_t af, uint8_t *ip, uint32_t *ip_len);

#ifdef __cplusplus
}
#endif

#endif /* IOT_SOCKET_H */
