/*******************************************************************************
* @file  sl_si91x_socket.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* The licensor of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/

#pragma once
#include "sl_si91x_socket_types.h"

/**
 * @addtogroup SI91X_SOCKET_FUNCTIONS
 * @{ 
 */
/**
 * @brief Creates a new socket.
 *
 * @param[in] family Specifies the communication domain for the socket. This selects the protocol family to be used. 
 *                   Accepts values from @ref BSD_SOCKET_FAMILIY. Currently, only @ref AF_INET and @ref AF_INET6 are supported.
 * @param[in] type Specifies the type of the socket, which determines the semantics of communication. 
 *                 Accepts values from @ref BSD_SOCKET_TYPES. Currently, only @ref SOCK_STREAM and @ref SOCK_DGRAM are supported.
 * @param[in] protocol Specifies a particular protocol to be used with the socket. 
 *                     Accepts values from @ref BSD_SOCKET_PROTOCOL. Currently, only @ref IPPROTO_TCP, @ref IPPROTO_UDP, and @ref IPPROTO_IP are supported.
 * @return int 
 */
int sl_si91x_socket(int family, int type, int protocol);

/**
 * @brief Creates an asynchronous socket and registers the provided callback.
 *
 * @param[in] family Specifies the communication domain for the socket. This selects the protocol family to be used. 
 *                   Accepts values from @ref BSD_SOCKET_FAMILIY. Currently, only @ref AF_INET and @ref AF_INET6 are supported.
 * @param[in] type Specifies the type of the socket, which determines the semantics of communication. 
 *                 Accepts values from @ref BSD_SOCKET_TYPES. Currently, only @ref SOCK_STREAM and @ref SOCK_DGRAM are supported.
 * @param[in] protocol Specifies a particular protocol to be used with the socket. 
 *                     Accepts values from @ref BSD_SOCKET_PROTOCOL. Currently, only @ref IPPROTO_TCP, @ref IPPROTO_UDP, and @ref IPPROTO_IP are supported.
 * @param[in] callback A function pointer of type @ref receive_data_callback. This function is called when the socket receives data.
 * @return int 
 */
int sl_si91x_socket_async(int family, int type, int protocol, receive_data_callback callback);

/**
 * @brief Sets a specified socket option on the identified socket asynchronously. 
 *
 * @param[in] socket The socket ID.
 * @param[in] level The option level. Accepts values from @ref BSD_SOCKET_OPTION_LEVEL.
 * @param[in] option_name The option to be configured. Accepts values from @ref SI91X_SOCKET_OPTION_NAME. 
 *                        Currently, only the following options are supported:
 *                        - @ref SL_SI91X_SO_RCVTIME
 *                        - @ref SL_SI91X_SO_MAXRETRY
 *                        - @ref SL_SI91X_SO_MSS
 *                        - @ref SL_SI91X_SO_TCP_KEEPALIVE
 *                        - @ref SL_SI91X_SO_HIGH_PERFORMANCE_SOCKET
 *                        - @ref SL_SI91X_SO_SSL_ENABLE
 *                        - @ref SL_SI91X_SO_SSL_V_1_0_ENABLE 
 *                        - @ref SL_SI91X_SO_SSL_V_1_1_ENABLE 
 *                        - @ref SL_SI91X_SO_SSL_V_1_2_ENABLE
 *                        - @ref SL_SI91X_SO_SOCK_VAP_ID
 *                        - @ref SL_SI91X_SO_SSL_V_1_3_ENABLE
 *                        - @ref SL_SI91X_SO_CERT_INDEX
 *                        - @ref SL_SI91X_SO_TLS_SNI
 *                        - @ref SL_SI91X_SO_MAX_RETRANSMISSION_TIMEOUT_VALUE
 * @param[in] option_value The value of the parameter.
 * @param[in] option_len The length of the parameter of type @ref socklen_t.
 * @return int 
 */
int sl_si91x_setsockopt_async(int32_t socket,
                              int level,
                              int option_name,
                              const void *option_value,
                              socklen_t option_len);

/**
 * @brief Assigns a local protocol address to a socket.
 *
 * @param[in] socket The socket ID.
 * @param[in] addr The address of type @ref sockaddr to which datagrams are to be sent.
 * @param[in] addr_len The length of the socket address of type @ref socklen_t in bytes.
 * @return int 
 */
int sl_si91x_bind(int socket, const struct sockaddr *addr, socklen_t addr_len);

/**
 * @brief Enables a socket to listen for remote connection requests in passive mode.
 *
 * @param[in] socket The socket ID.
 * @param[in] max_number_of_clients The maximum number of clients that the socket can accept.
 * @return int 
 */
int sl_si91x_listen(int socket, int max_number_of_clients);

/**
 * @brief Accepts a connection request from a remote peer. 
 *
 * This function blocks until a client attempts to connect to the server socket. After receiving a connection request, it proceeds.
 *
 * @param[in] socket The socket ID.
 * @param[in] addr The address of type @ref sockaddr to which datagrams are to be sent.
 * @param[in] addr_len The length of the socket address of type @ref socklen_t in bytes.
 * @return int 
 */
int sl_si91x_accept(int socket, const struct sockaddr *addr, socklen_t addr_len);

/**
 * @brief 
 *  Accepts a connection request from the remote peer and registers a callback.
 * 
 *  This function sets up the server socket to listen for incoming connections
 *  and immediately returns without blocking the main program's execution. 
 * 
 * @param[in] socket 
 *  Socket ID.
 * @param[in] callback 
 *  A function pointer of type @ref accept_callback that will be called when a new client is connected to the server.
 * @return int 
 */
int sl_si91x_accept_async(int socket, accept_callback callback);

/**
 * @brief 
 * Initiates a connection to a remote socket specified by the addr parameter.
 * @param[in] socket 
 * Socket ID.
 * @param[in] addr 
 *  Address of type @ref sockaddr to which datagrams are to be sent.
 * @param[in] addr_len
 *  Length of the socket address of type @ref socklen_t in bytes.
 * @return int 
 */
int sl_si91x_connect(int socket, const struct sockaddr *addr, socklen_t addr_len);

/**
 * @brief 
 * Sends the data to the remote peer on the given socket. 
 * 
 * This should be used only when the socket is in a connected state.
 * 
 * @param[in] socket 
 * Socket ID.
 * @param[in] buffer 
 * Pointer to the buffer containing data to send to the remote peer.
 * @param[in] buffer_length 
 *  Length of the buffer pointed to by the buffer parameter.
 * @param[in] flags 
 *  Controls the transmission of the data.
 * @return int 
 * @note The flags parameter is not currently supported.
 */
int sl_si91x_send(int socket, const uint8_t *buffer, size_t buffer_length, int32_t flags);

/**
 * @brief 
 * Transmits one or more messages to a socket asynchronously. 
 * 
 * This should be used only when the socket is in a connected state.
 * 
 * @param[in] socket 
 * Socket ID.
 * @param[in] buffer 
 * Pointer to the buffer containing data to send to the remote peer
 * @param[in] buffer_length 
 *  Length of the buffer pointed to by the buffer parameter.
 * @param[in] flags 
 *  Controls the transmission of the data.
 * @param[in] callback 
 *  A function pointer of type @ref data_transfer_complete_handler that will be called after complete data transfer.
 * @return int 
 * @note The flags parameter is not currently supported.
 */
int sl_si91x_send_async(int socket,
                        const uint8_t *buffer,
                        size_t buffer_length,
                        int32_t flags,
                        data_transfer_complete_handler callback);

/**
 * @brief 
 * Transmits one or more messages to another socket. 
 * 
 * The function can also be called from an unconnected socket, typically like a UDP socket.
 * 
 * @param[in] socket 
 * Socket ID.
 * @param[in] buffer 
 *  Pointer to data buffer containing data to send to remote peer.
 * @param[in] buffer_length 
 *  Length of the buffer pointed to by the buffer parameter.
 * @param[in] flags 
 *  Controls the transmission of the data.
 * @param[in] addr 
 *  Address of type @ref sockaddr to which datagrams are to be sent.
 * @param[in] addr_len
 *  Length of the socket address of type @ref socklen_t in bytes.
 * @return int 
 * @note The flags parameter is not currently supported.
 */
int sl_si91x_sendto(int socket,
                    uint8_t *buffer,
                    size_t buffer_length,
                    int32_t flags,
                    const struct sockaddr *addr,
                    socklen_t addr_len);

/**
 * @brief 
 * Transmits one or more messages to another socket asynchronously and receives acknowledgement through the registered callback.
 * 
 * The function can also be called from an unconnected socket, typically like a UDP socket.
 * 
 * @param[in] socket 
 * Socket ID.
 * @param[in] buffer 
 *  Pointer to data buffer containing data to send to remote peer.
 * @param[in] buffer_length 
 *  Length of the buffer pointed to by the buffer parameter.
 * @param[in] flags 
 *  Controls the transmission of the data.
 * @param[in] to_addr 
 *  Address of type @ref sockaddr to which datagrams are to be sent.
 * @param[in] to_addr_len
 *  Length of the socket address of type @ref socklen_t in bytes.
 * @param[in] callback 
 *  A function pointer of type @ref data_transfer_complete_handler that will be called after complete data transfer.
 * @return int 
 * @note The flags parameter is not currently supported.
 */
int sl_si91x_sendto_async(int socket,
                          uint8_t *buffer,
                          size_t buffer_length,
                          int32_t flags,
                          const struct sockaddr *to_addr,
                          socklen_t to_addr_len,
                          data_transfer_complete_handler callback);

/**
 * @brief 
 *  Sends the data that is greater than MSS size.
 * @param[in] socket 
 * Socket ID.
 * @param[in] buffer 
 *  Pointer to data buffer containing data to send to remote peer.
 * @param[in] buffer_length 
 *  Length of the buffer pointed to by the buffer parameter.
 * @param[in] flags 
 *  Controls the transmission of the data.
 * @return int 
 * @note The flags parameter is not currently supported.
 */
int sl_si91x_send_large_data(int socket, const uint8_t *buffer, size_t buffer_length, int32_t flags);

/**
 * @brief 
 *  Receives data from a connected socket. 
 * @param[in] socket 
 *  Socket ID.
 * @param[in] buffer 
 *  Pointer to the buffer to hold the data received from the remote peer.
 * @param[in] bufferLength 
 *  Length of the buffer pointed to by the buffer parameter.
 * @param[in] flags 
 *  Controls the reception of the data.
 * @return int 
 * @note The flags parameter is not currently supported.
 */
int sl_si91x_recv(int socket, uint8_t *buffer, size_t bufferLength, int32_t flags);

/**
 * @brief 
 *  Receives data from an unconnected socket typically like a UDP socket.
 * @param[in] socket 
 *  Socket Id.
 * @param[in] buffer 
 *  Pointer to the buffer to hold the data received from the remote peer.
 * @param[in] buffersize 
 *  Size of the buffer pointed to by the buffer parameter.
 * @param[in] flags 
 *  Controls the reception of the data.
 * @param[in] fromAddr 
 *  Address of remote peer of type @ref sockaddr, from where current packet was received.
 * @param[in] fromAddrLen 
 *  Pointer that contains remote peer address (fromAddr) length of type @ref socklen_t
 * @return int 
 * @note The flags parameter is not currently supported.
 */
int sl_si91x_recvfrom(int socket,
                      uint8_t *buffer,
                      size_t buffersize,
                      int32_t flags,
                      struct sockaddr *fromAddr,
                      socklen_t *fromAddrLen);

/**
 * @brief 
 *   Disables send or receive on a socket.
 * @param[in] socket
 *    Socket ID that is to be closed.
 * @param[in] how
 *    Determines whether the socket is closed based on given socket ID or port number.
 *    - 0: Close the specified socket.
 *    - 1: Close all the sockets open on a specified socket's source port number.
 * @return int 
 * @note 
 *    In case of socket being a server socket, the "how" parameter would be ignored and socket shall always be closed based on port number.
 */
int sl_si91x_shutdown(int socket, int how);

/**
 * @brief 
 *  Monitors multiple file descriptors, including sockets, for various I/O events, such as readiness for reading or writing, and exceptional conditions.
 * 
 * select() allows a program to monitor multiple file descriptors, 
 * waiting until one or more of the file descriptors become "ready" 
 * for some class of I/O operation (e.g., input possible).  A file 
 * descriptor is considered ready if it is possible to perform a 
 * corresponding I/O operation without blocking.
 * 
 * @param[in] nfds 
 *  The first nfds descriptors are checked in each set; i.e., the descriptors from 0 through nfds-1.
 * @param[in,out] readfds 
 *  A pointer to a fd_set object that specifies the descriptors to check for files that are ready for reading.
 * @param[in,out] writefds 
 *  A pointer to a fd_set object that specifies the descriptors to check for files that are ready for writing.
 * @param[in,out] exceptfds 
 *  A pointer to a fd_set object that will be watched for exceptions.
 * @param[in] timeout 
 *  If timeout is not a null pointer, it specifies the maximum interval to wait for the selection to complete.
 * @param[in] callback 
 *  A function pointer of type @ref select_callback that will be called when asynchronous response reach the select request.
 * @return int 
 * @note The readfds and writefds parameters are modified in the case of callback being NULL.
 *       The exceptfds parameter is not currently supported.
 */
int sl_si91x_select(int nfds,
                    fd_set *readfds,
                    fd_set *writefds,
                    fd_set *exceptfds,
                    struct timeval *timeout,
                    select_callback callback);

/**
 * @brief
 * Register callback for remote socket termination event.
 * @param[in] callback
 * A valid function pointer of type @ref remote_socket_termination_callback that will be called when remote socket is terminated.
 */
void sl_si91x_set_remote_termination_callback(remote_socket_termination_callback callback);
/** @} */
