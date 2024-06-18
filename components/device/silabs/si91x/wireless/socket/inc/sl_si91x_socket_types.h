/*******************************************************************************
* @file  sl_si91x_socket_types.h
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

#include <stdint.h>

#include "socket.h"
#include "select.h"

/**
 * @addtogroup SI91X_SOCKET_FUNCTIONS
 * @{ 
 */
/**
 * @typedef receive_data_callback
 * @brief This callback is used to read data asynchronously from a socket. It is registered when the sl_si91x_socket_async API is called.
 * @param socket
 * Socket ID.
 * @param buffer
 * Pointer to the buffer that will store the received data.
 * @param length
 * Size of the buffer.
 * @return
 * N/A
 */
typedef void (*receive_data_callback)(uint32_t socket, uint8_t *buffer, uint32_t length);

/**
 * @typedef accept_callback
 * @brief This callback is used to provide the parameters of a newly accepted connection. It is registered when the sl_si91x_accept_async API is called.
 * @param socket
 * Socket ID of the accepted connection.
 * @param addr
 * Address of the remote peer.
 * @param ip_version
 * IP version of the connection.
 * @return
 * N/A
 */
typedef void (*accept_callback)(int32_t socket, struct sockaddr *addr, uint8_t ip_version);

/**
 * @typedef data_transfer_complete_handler
 * @brief This callback is used to indicate the status of a data transfer. It is registered when either the sl_si91x_send_async or sl_si91x_sendto_async API is called.
 * @param socket
 * Socket ID.
 * @param length
 * Number of bytes transferred.
 * @return
 * N/A
 */
typedef void (*data_transfer_complete_handler)(int32_t socket, uint16_t length);

/**
 * @typedef select_callback
 * @brief This callback is used to indicate when an asynchronous response reaches the select request. It is registered when the sl_si91x_select API is called.
 * @param fd_read
 * Read file descriptor set.
 * @param fd_write
 * Write file descriptor set.
 * @param fd_except
 * Exception file descriptor set.
 * @param status
 * Status of the select request.
 * @return
 * N/A
 */
typedef void (*select_callback)(fd_set *fd_read, fd_set *fd_write, fd_set *fd_except, int32_t status);

/**
 * @typedef remote_socket_termination_callback
 * @brief This callback is used to indicate when a remote socket is terminated. It is registered when the sl_si91x_set_remote_termination_callback API is called.
 * @param socket
 * Socket ID.
 * @param port
 * Port number of the remote socket.
 * @param bytes_sent
 * Number of bytes sent before termination.
 * @return
 * N/A
 */
typedef void (*remote_socket_termination_callback)(int socket, uint16_t port, uint32_t bytes_sent);
/** @} */

/// Internal  si91x BSD socket status
typedef enum {
  RESET = 0,   // State of unallocated socket.
  INITIALIZED, // Socket attains this state when socket() has been executed successfully.
  BOUND,       // Socket attains this state when bind() has been executed successfully.
  LISTEN,      // (TCP ONLY STATE) Socket attains this state when listen() has been executed successfully.
  UDP_UNCONNECTED_READY, // (UDP ONLY STATE) Socket attains this state when sendto() or recvfrom() has been executed successfully prior connect.
  CONNECTED,   // Socket attains this state when connect() has been executed successfully.
  DISCONNECTED // Socket attains this state when underlying connection is lost
} si91x_bsd_socket_state_t;

#define SI91X_MAX_SIZE_OF_EXTENSION_DATA 256

#pragma pack()
/// Internal  si91x server name indication extensions
typedef struct {
  uint8_t buffer[SI91X_MAX_SIZE_OF_EXTENSION_DATA]; ///< Buffer
  uint16_t total_extensions;                        ///< Total extensions
  uint16_t current_size_of_extensions;              ///< Current size of extensions
} si91x_server_name_indication_extensions_t;

#pragma pack()

/// Internal si91x socket type length
typedef struct {
  uint16_t type;   ///< Socket type
  uint16_t length; ///< Data length
  uint8_t value[]; ///< Data
} si91x_socket_type_length_value_t;

/// Internal si91x socket handle
typedef struct {
  int32_t id;                                               ///< Socket ID
  int32_t type;                                             ///< Socket type
  int32_t protocol;                                         ///< Protocol
  uint16_t tcp_keepalive_initial_time;                      ///< TCP keepalive intial time
  uint8_t max_tcp_retries;                                  ///< MAX TCOP retries
  uint16_t read_timeout;                                    ///< Read timeout
  uint8_t certificate_index;                                ///< Certificate Index
  uint8_t vap_id;                                           ///< Virtual AP ID
  uint16_t mss;                                             ///< Maximum segment size (MSS) value
  struct sockaddr_in6 local_address;                        ///< Using sockaddr_in6 to hold either IPV4 or IPV6.
  struct sockaddr_in6 remote_address;                       ///< Using sockaddr_in6 to hold either IPV4 or IPV6.
  si91x_bsd_socket_state_t state;                           ///< BSD socket state (used for internal tracking)
  si91x_server_name_indication_extensions_t sni_extensions; ///< SNI Extension
#ifdef SLI_SI917
  uint32_t ssl_bitmap;                       ///< SSL bitmap
  uint32_t max_retransmission_timeout_value; ///< Max retransmission timeout value
  uint32_t tos;                              ///< TOS
#else
  uint8_t ssl_bitmap; ///< SSL Bitmap
#endif

  receive_data_callback recv_data_callback;              ///< Receive data callback
  data_transfer_complete_handler data_transfer_callback; ///< Data transfer callback
} si91x_socket_t;

/// SiWx91x select context
typedef struct {
  int nfds; ///< no of FDs

  fd_set *read_fd;      ///< Read FD set
  fd_set *write_fd;     ///< Write FD set
  fd_set *exception_fd; ///< Exception FD set

  select_callback callback; ///< Select callback
} sl_si91x_select_context;
