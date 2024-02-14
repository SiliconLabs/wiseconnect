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
 * @brief 
 *  This @ref sl_si91x_socket_async() function creates an asynchronous socket and registers the provided callback.
 * @param[in] family 
 *  This specifies a communication domain within which communication will take place; this selects the protocol	family which
    should be used.
 * @param[in] type 
 *  The socket has the indicate type, which specifies the semantics of communication. Currently defined types are SOCK_STREAM,
    SOCK_DGRAM.
 * @param[in] protocol 
    The protocol  argument specifies a particular protocol to be used with the socket.
 * @param[in] callback 
    A function pointer that will be called when the socket receive the data.
 * @return int 
 */
int sl_si91x_socket_async(int family, int type, int protocol, receive_data_callback callback);

/**
 * @brief 
 *  The @ref sl_si91x_socket() function creates a new socket.
 * @param[in] family 
 *  This specifies a communication domain within which communication will take place; this selects the protocol	family which
    should be used.
 * @param[in] type 
    The socket has the indicate type, which specifies the semantics of communication. Currently defined types are SOCK_STREAM,
    SOCK_DGRAM.
 * @param[in] protocol 
    The protocol argument specifies a particular protocol to be used with the socket.
 * @return int 
 */
int sl_si91x_socket(int family, int type, int protocol);

/**
 * @brief 
 * The @ref sl_si91x_setsockopt_async() function's purpose would be to set the specified socket option on the identified socket asynchronously. 
 * @param[in] sockID 
 *  Socket identification number.
 * @param[in] level 
 *  Level for which the option is set.
 * @param[in] option_name 
 *  This parameter is used to specify the particular socket option that may be set.
 * @param[in] option_value 
 *  Pointer to option data.
 * @param[in] option_len 
 *  The option_length is a value-result argument, initially containing the size of the buffer pointed to by option_value, and 
 *  modified on return to indicate	the actual size	of the value returned. If no option value is to be supplied or returned, optval 
 *  may be NULL.
 * @return int 
 */
int sl_si91x_setsockopt_async(int32_t sockID,
                              int level,
                              int option_name,
                              const void *option_value,
                              socklen_t option_len);

/**
 * @brief 
 *  The @ref sl_si91x_bind() api assigns the local protocol address to a socket.
 * @param[in] socket
 *  The socket_id argument is a socket.
 * @param[in] addr 
 *  The addr argument is the address is that to which datagrams are to be sent.
 * @param[in] addr_len
 *  The addr_len argument indicates the amount of space pointed to by addr, in bytes.
 * @return int 
 */
int sl_si91x_bind(int socket, const struct sockaddr *addr, socklen_t addr_len);

/**
 * @brief 
 *  The @ref sl_si91x_listen() function listen for socket connections.
 * @param[in] socket 
 *  The socket_id argument is a socket.
 * @param[in] max_number_of_clients
 *  The maximum number of clients that the socket can accept.
 * @return int 
 */
int sl_si91x_listen(int socket, int max_number_of_clients);

/**
 * @brief 
 *  The purpose of the @ref sl_si91x_accept function is to block until a client attempts to connect to the server socket, 
 *  once a connection request is received.
 * @param[in] socket 
 *  The socket_id argument is a socket.
 * @param[in] addr 
 *  The addr argument is used to store the accepted client socket address.
 * @param[in] addr_len 
 *  The addr_len argument indicates the amount of space pointed to by addr, in bytes.
 * @return int 
 */
int sl_si91x_accept(int socket, const struct sockaddr *addr, socklen_t addr_len);

/**
 * @brief 
 *  The @ref sl_si91x_accept_async() api primary purpose is to set up the server socket to listen for incoming connections
 *  and immediately return without blocking the main program's execution. 
 * @param[in] socket 
 *  The socket_id argument is a socket.
 * @param[in] callback 
 *  A function pointer that will be called when the new client is connected to the server.
 * @return int 
 */
int sl_si91x_accept_async(int socket, accept_callback callback);

/**
 * @brief 
 * The purpose of this @ref sl_si91x_connect() function is to initiate a connection to a remote socket specified by the addr parameter.
 * @param[in] socket 
 *  The socket_id argument is a socket.
 * @param[in] addr 
 *  The addr argument is the address is that to which datagrams are to be sent.
 * @param[in] addr_len 
 *  The addr_len argument indicates the amount of space pointed to by addr, in bytes.
 * @return int 
 */
int sl_si91x_connect(int socket, const struct sockaddr *addr, socklen_t addr_len);

/**
 * @brief 
 *  The  @ref sl_si91x_send() function is used to transmit one or more messages to a socket. The sl_si91x_send() function may be used 
 *  only when the socket is in a connected state.
 * @param[in] socket 
 *  The socket_id argument is a socket.
 * @param[in] buffer 
 *  Pointer to the buffer that receives the data.
 * @param[in] buffer_length 
 *  Length of the buffer pointed to by the buffer parameter.
 * @param[in] flags 
 *  Controls the transmission of the data.
 * @return int 
 * @note The argument flags are not supported currently.
 */
int sl_si91x_send(int socket, const uint8_t *buffer, size_t buffer_length, int32_t flags);

/**
 * @brief 
 * The  @ref sl_si91x_send_async() function is used to transmit one or more messages to a socket asynchronously. The sl_si91x_send_async() function may be used 
 *  only when the socket is in a connected state.
 * @param[in] socket 
 *  The socket_id argument is a socket.
 * @param[in] buffer 
 *  Pointer to the buffer that receives the data.
 * @param[in] buffer_length 
 *  Length of the buffer pointed to by the buffer parameter.
 * @param[in] flags 
 *  Controls the transmission of the data.
 * @param[in] callback 
 *  A function pointer that will be called when the socket receive the data.
 * @return int 
 * @note The argument flags are not supported currently.
 */
int sl_si91x_send_async(int socket,
                        const uint8_t *buffer,
                        size_t buffer_length,
                        int32_t flags,
                        data_transfer_complete_handler callback);

/**
 * @brief 
 * The @ref sl_si91x_sendto() function is used to transmit one or more messages to another socket. The function can also be called 
 * from an unconnected socket typically like a UDP socket.
 * @param[in] socket 
 *  The socket_id argument is a socket.
 * @param[in] buffer 
 *  Pointer to the buffer that receives the data.
 * @param[in] buffer_length 
 *  Length of the buffer pointed to by the buffer parameter.
 * @param[in] flags 
 *  Controls the transmission of the data.
 * @param[in] addr 
 *  The addr argument is the address is that to which datagrams are to be sent.
 * @param[in] addr_len 
 *  The addr_len argument indicates the amount of space pointed to by addr, in bytes.
 * @return int 
 * @note The argument flags are not supported currently.
 */
int sl_si91x_sendto(int socket,
                    uint8_t *buffer,
                    size_t buffer_length,
                    int32_t flags,
                    const struct sockaddr *addr,
                    socklen_t addr_len);

/**
 * @brief 
 * The @ref sl_si91x_sendto_async() function is used to transmit one or more messages to another socket asynchronously.
 * The function can also be called from an unconnected socket typically like a UDP socket.
 * @param[in] socket 
 *  The socket_id argument is a socket.
 * @param[in] buffer 
 *  Pointer to the buffer that receives the data.
 * @param[in] buffer_length 
 *  Length of the buffer pointed to by the buffer parameter.
 * @param[in] flags 
 *  Controls the transmission of the data.
 * @param[in] to_addr 
 *  Address of the target.
 * @param[in] to_addr_len 
 *  Size of the address pointed by to_addr.
 * @param[in] callback 
 *  A function pointer that will be called when the socket receive the data.
 * @return int 
 * @note The argument flags are not supported currently.
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
 *  Basically @ref sl_si91x_send_large_data() function is used to send the data that is greater than MSS size.
 * @param[in] socket 
 *  The socket_id argument is a socket
 * @param[in] buffer 
 *  Pointer to the buffer that receives the data.
 * @param[in] buffer_length 
 *  Length of the buffer pointed to by the buffer parameter.
 * @param[in] flags 
 *  Controls the transmission of the data.
 * @return int 
 * @note The argument flags are not supported currently.
 */
int sl_si91x_send_large_data(int socket, const uint8_t *buffer, size_t buffer_length, int32_t flags);

/**
 * @brief 
 *  The purpose of the @ref sl_si91x_recv function is to receive data from a connected socket. 
 * @param[in] socket 
 *  The socket_id argument is a socket
 * @param[in] buffer 
 *  Pointer to the buffer that receives the data.
 * @param[in] bufferLength 
 *  Length of the buffer pointed to by the buffer parameter.
 * @param[in] flags 
 *  Controls the reception of the data.
 * @return int 
 * @note The argument flags are not supported currently.
 */
int sl_si91x_recv(int socket, uint8_t *buffer, size_t bufferLength, int32_t flags);

/**
 * @brief 
 *  The purpose of the @ref sl_si91x_recvfrom function is to receive data from an unconnected socket typically like a UDP socket.
 * @param[in] socket 
 *  The socket_id argument is a socket.
 * @param[in] buffer 
 *  Pointer to the buffer that receives the data.
 * @param[in] buffersize 
 *  Size of the buffer pointed to by the buffer parameter.
 * @param[in] flags 
 *  Controls the reception of the data.
 * @param[in] fromAddr 
 *  Pointer to a socket address structure from which data is received.
 * @param[in] fromAddrLen 
 *  The fromAddrLen argument is a value-result argument, initialized to the size of the buffer associated with from_addr, and 
 *  modified on return to indicate the actual size of the address stored there.
 * @return int 
 * @note The argument flags are not supported currently.
 */
int sl_si91x_recvfrom(int socket,
                      uint8_t *buffer,
                      size_t buffersize,
                      int32_t flags,
                      struct sockaddr *fromAddr,
                      socklen_t *fromAddrLen);

/**
 * @brief 
 *   The purpose of the @ref sl_si91x_shutdown is to disable sends or receives on a socket.
 * @param[in] socket
 *    Socket FD that is to be closed.
 * @param[in] how
 *    How determines whether the socket is closed based on given socket ID or port number.
 * @note 
 *    In case of socket being a server socket, the "how" parameter would be ignored and socket shall always be closed based on port number.
 */
int sl_si91x_shutdown(int socket, int how);

/**
 * @brief 
 *  The purpose of the @ref sl_si91x_select function is to monitor multiple file descriptors, including sockets, for various I/O events, such as readiness for reading or writing, and exceptional conditions.
 *  It allows you to efficiently manage multiple sockets and determine when they are available for specific I/O operations.
 * @param[in] nfds 
 *  The first nfds descriptors are checked in each set; i.e., the descriptors from 0 through nfds-1.
 * @param[in,out] readfds 
 *  A pointer to a fd_set object that specifies the descriptors to check for files that are ready for reading.
 * @param[in,out] writefds 
 *  A pointer to a fd_set object that specifies the descriptors to check for files that are ready for writing.
 * @param[in,out] exceptfds 
 *  A pointer to a fd_set object that will be watched for exceptions
 * @param[in] timeout 
 *  If timeout is not a null pointer, it specifies the maximum interval to wait for the selection to complete.
 * @param[in] callback 
 *  A function pointer that will be called when the socket receive the data.
 * @return int 
 * @note The readfds, writefds are modified incase of callback being NULL.
 *       exceptfds are not currently being supported.
 */
int sl_si91x_select(int nfds,
                    fd_set *readfds,
                    fd_set *writefds,
                    fd_set *exceptfds,
                    struct timeval *timeout,
                    select_callback callback);

/**
 * @brief
 * A function to register callback for remote socket termination event.
 * @param[in] callback
 * A valid function pointer that will be called when remote socket is terminated.
 */
void sl_si91x_set_remote_termination_callback(remote_socket_termination_callback callback);
/** @} */
