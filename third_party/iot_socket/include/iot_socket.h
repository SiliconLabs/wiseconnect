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

/***************************************************************************/ /**
 * @brief
 *   Create a communication socket.
 * @param[in] af       Address family. One of the values from @ref IOT_SOCKET_ADDRESS_FAMILY.
 * @param[in] type     Socket type. One of the values from @ref IOT_SOCKET_TYPE.
 * @param[in] protocol Socket protocol. One of the values from @ref IOT_SOCKET_PROTOCOL.
 * @return    int32_t. Status information:
 *            - Socket identification number (>=0).
              - @ref IOT_SOCKET_EINVAL        = Invalid argument.
              - @ref IOT_SOCKET_ENOTSUP       = Operation not supported.
              - @ref IOT_SOCKET_ENOMEM        = Not enough memory.
              - @ref IOT_SOCKET_ERROR         = Unspecified error.
 ******************************************************************************/
extern int32_t iotSocketCreate (int32_t af, int32_t type, int32_t protocol);

/***************************************************************************/ /**
 * @brief
 *   Assign a local address to a socket.
 * @param[in] socket   Socket identification number.
 * @param[in] ip       Pointer to local IP address.
 * @param[in] ip_len   Length of 'ip' address in bytes.
 * @param[in] port     Local port number.
 * @return    int32_t. Status information:
 *            - 0                             = Operation successful.
              - @ref IOT_SOCKET_ESOCK         = Invalid socket.
              - @ref IOT_SOCKET_EINVAL        = Invalid argument (address or socket already bound).
              - @ref IOT_SOCKET_EADDRINUSE    = Address already in use.
              - @ref IOT_SOCKET_ERROR         = Unspecified error.
 ******************************************************************************/
extern int32_t iotSocketBind (int32_t socket, const uint8_t *ip, uint32_t ip_len, uint16_t port);

/***************************************************************************/ /**
 * @brief
 *   Listen for socket connections.
 * @param[in] socket   Socket identification number.
 * @param[in] backlog  Maximum number of the clients supported. 
 * @return    int32_t. Status information:
 *            - 0                             = Operation successful.
              - @ref IOT_SOCKET_ESOCK         = Invalid socket.
              - @ref IOT_SOCKET_EINVAL        = Invalid argument (socket not bound).
              - @ref IOT_SOCKET_ENOTSUP       = Operation not supported.
              - @ref IOT_SOCKET_EISCONN       = Socket is already connected.
              - @ref IOT_SOCKET_ERROR         = Unspecified error.
 ******************************************************************************/
extern int32_t iotSocketListen (int32_t socket, int32_t backlog);

/***************************************************************************/ /**
 * @brief
 *   Accept a new connection on a socket.
 * @param[in]     socket   Socket identification number.
 * @param[out]    ip       Pointer to buffer where address of connecting socket shall be returned (NULL for none).
 * @param[in,out] ip_len   pointer to length of 'ip' (or NULL if 'ip' is NULL):
                  - Length of supplied 'ip' on input.
                  - Length of stored 'ip' on output.
 * @param[out]    port     Pointer to buffer where port of connecting socket shall be returned (NULL for none).
 * @return       int32_t. Status information:
                 - socket identification number of accepted socket (>=0).
                 - @ref IOT_SOCKET_ESOCK         = Invalid socket.
                 - @ref IOT_SOCKET_EINVAL        = Invalid argument (socket not in listen mode).
                 - @ref IOT_SOCKET_ENOTSUP       = Operation not supported (socket type does not support accepting connections).
                 - @ref IOT_SOCKET_ECONNRESET    = Connection reset by the peer.
                 - @ref IOT_SOCKET_ECONNABORTED  = Connection aborted locally.
                 - @ref IOT_SOCKET_EAGAIN        = Operation would block or timed out (may be called again).
                 - @ref IOT_SOCKET_ERROR         = Unspecified error.
 ******************************************************************************/
extern int32_t iotSocketAccept (int32_t socket, uint8_t *ip, uint32_t *ip_len, uint16_t *port);

/***************************************************************************/ /**
 * @brief
 *   Connect a socket to a remote host.
 * @param[in]     socket   Socket identification number.
 * @param[in]     ip       Pointer to remote IP address.
 * @param[in]     ip_len   Length of 'ip' address in bytes.
 * @param[in]     port     Remote port number. 
 * @return        int32_t. Status information:          
                  - 0                             = Operation successful.
                  - @ref IOT_SOCKET_ESOCK         = Invalid socket.
                  - @ref IOT_SOCKET_EINVAL        = Invalid argument.
                  - @ref IOT_SOCKET_EALREADY      = Connection already in progress.
                  - @ref IOT_SOCKET_EINPROGRESS   = Operation in progress.
                  - @ref IOT_SOCKET_EISCONN       = Socket is connected.
                  - @ref IOT_SOCKET_ECONNREFUSED  = Connection rejected by the peer.
                  - @ref IOT_SOCKET_ECONNABORTED  = Connection aborted locally.
                  - @ref IOT_SOCKET_EADDRINUSE    = Address already in use.
                  - @ref IOT_SOCKET_ETIMEDOUT     = Operation timed out.
                  - @ref IOT_SOCKET_ERROR         = Unspecified error.
 ******************************************************************************/
extern int32_t iotSocketConnect (int32_t socket, const uint8_t *ip, uint32_t ip_len, uint16_t port);

/***************************************************************************/ /**
 * @brief
 *   Receives data from the socket.
 * @param[in]     socket   Socket identification number.
 * @param[out]    buf      Pointer to buffer where data should be stored.
 * @param[in]     len      Length of buffer (in bytes).
 * @return        int32_t. Status information:          
                  - Number of bytes received (>=0), if len != 0.
                  - 0                             = Data is available (len = 0).
                  - \ref IOT_SOCKET_ESOCK         = Invalid socket.
                  - \ref IOT_SOCKET_EINVAL        = Invalid argument (pointer to buffer or length).
                  - \ref IOT_SOCKET_ENOTCONN      = Socket is not connected.
                  - \ref IOT_SOCKET_ECONNRESET    = Connection reset by the peer.
                  - \ref IOT_SOCKET_ECONNABORTED  = Connection aborted locally.
                  - \ref IOT_SOCKET_EAGAIN        = Operation would block or timed out (may be called again).
                  - \ref IOT_SOCKET_ERROR         = Unspecified error.
 ******************************************************************************/
extern int32_t iotSocketRecv (int32_t socket, void *buf, uint32_t len);

/***************************************************************************/ /**
 * @brief
 *   Receives data from the socket.
 * @param[in]     socket   Socket identification number.
 * @param[out]    buf      Pointer to buffer where data should be stored.
 * @param[in]     len      Length of buffer (in bytes).
 * @param[out]    ip       Pointer to buffer where remote source address shall be returned (NULL for none).
 * @param[in,out] ip_len   Pointer to length of 'ip' (or NULL if 'ip' is NULL):
 *                 - Length of supplied 'ip' on input.
 *                 - Length of stored 'ip' on output.
 * @param[out]    port     Pointer to buffer where remote source port shall be returned (NULL for none).
 * @return        int32_t. Status information:          
                  - Number of bytes received (>=0), if len != 0.
                  - 0                             = Data is available (len = 0).
                  - @ref IOT_SOCKET_ESOCK         = Invalid socket.
                  - @ref IOT_SOCKET_EINVAL        = Invalid argument (pointer to buffer or length).
                  - @ref IOT_SOCKET_ENOTCONN      = Socket is not connected.
                  - @ref IOT_SOCKET_ECONNRESET    = Connection reset by the peer.
                  - @ref IOT_SOCKET_ECONNABORTED  = Connection aborted locally.
                  - @ref IOT_SOCKET_EAGAIN        = Operation would block or timed out (may be called again).
                  - @ref IOT_SOCKET_ERROR         = Unspecified error.
 ******************************************************************************/
extern int32_t iotSocketRecvFrom (int32_t socket, void *buf, uint32_t len, uint8_t *ip, uint32_t *ip_len, uint16_t *port);

/***************************************************************************/ /**
 * @brief
 *   Send data or check if data can be sent on a connected socket.
 * @param[in]     socket   Socket identification number.
 * @param[in]     buf      Pointer to buffer containing data to send.
 * @param[in]     len      Length of data (in bytes).
 * @return        int32_t. Status information:
                  - Number of bytes sent (>=0), if len != 0.
                  - 0                             = Data can be sent (len = 0).
                  - @ref IOT_SOCKET_ESOCK         = Invalid socket.
                  - @ref IOT_SOCKET_EINVAL        = Invalid argument (pointer to buffer or length).
                  - @ref IOT_SOCKET_ENOTCONN      = Socket is not connected.
                  - @ref IOT_SOCKET_ECONNRESET    = Connection reset by the peer.
                  - @ref IOT_SOCKET_ECONNABORTED  = Connection aborted locally.
                  - @ref IOT_SOCKET_EAGAIN        = Operation would block or timed out (may be called again).
                  - @ref IOT_SOCKET_ERROR         = Unspecified error.
 * @note The function doesn't guarantees the packets are transmitted to remote note, which are enqueued in the queue. 
 * @note The function can only send max of 1460 bytes in case of plain TCP and UDP. For TLS, the max buffer length is 1370.
 ******************************************************************************/
extern int32_t iotSocketSend (int32_t socket, const void *buf, uint32_t len);

/***************************************************************************/ /**
 * @brief
 *   Send data or check if data can be sent on a socket.
 * @param[in]     socket   Socket identification number.
 * @param[in]     buf      Pointer to buffer containing data to send.
 * @param[in]     len      Length of data (in bytes).
 * @param[in]     ip       Pointer to remote destination IP address.
 * @param[in]     ip_len   Length of 'ip' address in bytes.
 * @param[in]     port     Remote destination port number.
 * @return        int32_t. Status information:
                  - Number of bytes sent (>=0), if len != 0.
                  - 0                             = Data can be sent (len = 0).
                  - @ref IOT_SOCKET_ESOCK         = Invalid socket.
                  - @ref IOT_SOCKET_EINVAL        = Invalid argument (pointer to buffer or length).
                  - @ref IOT_SOCKET_ENOTCONN      = Socket is not connected.
                  - @ref IOT_SOCKET_ECONNRESET    = Connection reset by the peer.
                  - @ref IOT_SOCKET_ECONNABORTED  = Connection aborted locally.
                  - @ref IOT_SOCKET_EAGAIN        = Operation would block or timed out (may be called again).
                  - @ref IOT_SOCKET_ERROR         = Unspecified error.
 * @note          If number of bytes to be send exceeds the MSS size specified by remote node, the API will return IOT_SOCKET_ERROR.
 *                To know the MSS size for the socket, use sl_si91x_get_socket_mss() utility, In the case of TCP, this should be called after iotSocketConnect().                 
 * @note          The function doesn't guarantees the packets are transmitted to remote note, which are enqueued in the queue. 
 * @note          The function can only send max of 1460 bytes in case of plain TCP or UDP. For TLS, the max buffer length is 1370.
 ******************************************************************************/
extern int32_t iotSocketSendTo (int32_t socket, const void *buf, uint32_t len, const uint8_t *ip, uint32_t ip_len, uint16_t port);

/***************************************************************************/ /**
 * @brief
 *   Retrieve local IP address and port of a socket.
 * @param[in]     socket   Socket identification number.
 * @param[out]    ip       Pointer to buffer where local address shall be returned (NULL for none).
 * @param[in,out] ip_len   Pointer to length of 'ip' (or NULL if 'ip' is NULL):
                  - Length of supplied 'ip' on input.
                  - Length of stored 'ip' on output.
 * @param[out]    port     Pointer to buffer where local port shall be returned (NULL for none).
 * @return        int32_t. Status information:
                  - 0                             = Operation successful.
                  - @ref IOT_SOCKET_ESOCK         = Invalid socket.
                  - @ref IOT_SOCKET_EINVAL        = Invalid argument (pointer to buffer or length).
                  - @ref IOT_SOCKET_ERROR         = Unspecified error.
 ******************************************************************************/
extern int32_t iotSocketGetSockName (int32_t socket, uint8_t *ip, uint32_t *ip_len, uint16_t *port);

/***************************************************************************/ /**
 * @brief
 *   Retrieve remote IP address and port of a socket.
 * @param[in]     socket   Socket identification number.
 * @param[out]    ip       Pointer to buffer where remote address shall be returned (NULL for none).
 * @param[in,out] ip_len   Pointer to length of 'ip' (or NULL if 'ip' is NULL):
                  - Length of supplied 'ip' on input.
                  - Length of stored 'ip' on output.
 * @param[out]    port     Pointer to buffer where remote port shall be returned (NULL for none).
 * @return        int32_t. Status information:
                  - 0                             = Operation successful.
                  - @ref IOT_SOCKET_ESOCK         = Invalid socket.
                  - @ref IOT_SOCKET_EINVAL        = Invalid argument (pointer to buffer or length).
                  - @ref IOT_SOCKET_ENOTCONN      = Socket is not connected.
                  - @ref IOT_SOCKET_ERROR         = Unspecified error.
 ******************************************************************************/
extern int32_t iotSocketGetPeerName (int32_t socket, uint8_t *ip, uint32_t *ip_len, uint16_t *port);

/***************************************************************************/ /**
 * @brief
 *   Get socket option.
 * @param[in]     socket   Socket identification number.
 * @param[in]     opt_id   Option identifier. One of the values from @ref IOT_SOCKET_OPTION_ID
 * @param[out]    opt_val  Pointer to the buffer that will receive the option value.
 * @param[in,out] opt_len  Pointer to length of the option value:
                  - Length of buffer on input.
                  - Length of data on output.
 * @return        int32_t. Satus information:
                  - 0                             = Operation successful.
                  - @ref IOT_SOCKET_ESOCK         = Invalid socket.
                  - @ref IOT_SOCKET_EINVAL        = Invalid argument.
                  - @ref IOT_SOCKET_ENOTSUP       = Operation not supported.
                  - @ref IOT_SOCKET_ERROR         = Unspecified error.
 * @note
 *    The following are the options, which are supported currently.
 *                - @ref IOT_SOCKET_IO_FIONBIO
 *                - @ref IOT_SOCKET_SO_RCVTIMEO
 *                - @ref IOT_SOCKET_SO_SNDTIMEO
 *                - @ref IOT_SOCKET_SO_KEEPALIVE
 *                - @ref IOT_SOCKET_SO_TYPE.
 ******************************************************************************/
extern int32_t iotSocketGetOpt (int32_t socket, int32_t opt_id, void *opt_val, uint32_t *opt_len);

/***************************************************************************/ /**
 * @brief
 *   Set socket option.
 * @param[in]     socket   Socket identification number.
 * @param[in]     opt_id   Option identifier. One of the values from @ref IOT_SOCKET_OPTION_ID.
 * @param[in]     opt_val  Pointer to the option value.
 * @param[in]     opt_len  Length of the option value in bytes.
 * @return        int32_t. Status information:
                  - 0                             = Operation successful.
                  - @ref IOT_SOCKET_ESOCK         = Invalid socket.
                  - @ref IOT_SOCKET_EINVAL        = Invalid argument.
                  - @ref IOT_SOCKET_ENOTSUP       = Operation not supported.
                  - @ref IOT_SOCKET_ERROR         = Unspecified error.
 * @note
 *   The following are the options, which are supported currently.
 *                - @ref IOT_SOCKET_IO_FIONBIO
 *                - @ref IOT_SOCKET_SO_RCVTIMEO
 *                - @ref IOT_SOCKET_SO_SNDTIMEO
 *                - @ref IOT_SOCKET_SO_KEEPALIVE
 *                - @ref IOT_SOCKET_SO_TYPE.
 ******************************************************************************/
extern int32_t iotSocketSetOpt (int32_t socket, int32_t opt_id, const void *opt_val, uint32_t opt_len);

/***************************************************************************/ /**
 * @brief
 *   Close and release a socket.
 * @param[in]     socket   Socket identification number.
 * @return        int32_t. Status information:
                  - 0                             = Operation successful.
                  - @ref IOT_SOCKET_ESOCK         = Invalid socket.
                  - @ref IOT_SOCKET_EAGAIN        = Operation would block (may be called again).
                  - @ref IOT_SOCKET_ERROR         = Unspecified error.
 * @note
 *   Calling close on server or first client socket would end up closing other socket as well.
 ******************************************************************************/
extern int32_t iotSocketClose (int32_t socket);

/***************************************************************************/ /**
 * @brief
 *   Retrieve host IP address from host name.
 * @param[in]     name     Host name.
 * @param[in]     af       Address family. One of value from IOT_SOCKET_ADDRESS_FAMILY
 * @param[out]    ip       Pointer to buffer where resolved IP address shall be returned.
 * @param[in,out] ip_len   Pointer to length of 'ip':
                  - length of supplied 'ip' on input.
                  - length of stored 'ip' on output.
 * @return        int32_t. Status information:
                  - 0                             = Operation successful.
                  - @ref IOT_SOCKET_EINVAL        = Invalid argument.
                  - @ref IOT_SOCKET_ENOTSUP       = Operation not supported.
                  - @ref IOT_SOCKET_ETIMEDOUT     = Operation timed out.
                  - @ref IOT_SOCKET_EHOSTNOTFOUND = Host not found.
                  - @ref IOT_SOCKET_ERROR         = Unspecified error.
 ******************************************************************************/
extern int32_t iotSocketGetHostByName (const char *name, int32_t af, uint8_t *ip, uint32_t *ip_len);

/** @} */

#ifdef  __cplusplus
}
#endif

#endif /* IOT_SOCKET_H */
