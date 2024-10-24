/*******************************************************************************
* @file  sl_si91x_socket.c
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

#include "sl_si91x_socket_utility.h"
#include "sl_status.h"
#include "sl_constants.h"
#include "sl_si91x_socket.h"
#include "sl_si91x_socket_callback_framework.h"
#include "sl_si91x_socket_types.h"
#include "sl_si91x_socket_constants.h"
#include "sl_si91x_protocol_types.h"
#include "sl_rsi_utility.h"
#include "sl_si91x_driver.h"
#include <stdint.h>
#include <string.h>

/******************************************************
 *                      Macros
 ******************************************************/
#define TCP_HEADER_LENGTH          56
#define UDP_HEADER_LENGTH          44
#define TCP_V6_HEADER_LENGTH       76
#define UDP_V6_HEADER_LENGTH       64
#define SI91X_SSL_HEADER_SIZE_IPV4 90
#define SI91X_SSL_HEADER_SIZE_IPV6 110

static int sli_si91x_accept_async(int socket,
                                  const struct sockaddr *addr,
                                  socklen_t addr_len,
                                  accept_callback callback);
static int sli_si91x_socket(int family, int type, int protocol, receive_data_callback callback);

void sl_si91x_set_remote_termination_callback(remote_socket_termination_callback callback)
{
  sli_si91x_set_remote_socket_termination_callback(callback);
}

// Create a new socket
int sl_si91x_socket(int family, int type, int protocol)
{
  return sli_si91x_socket(family, type, protocol, NULL);
}

int sl_si91x_socket_async(int family, int type, int protocol, receive_data_callback callback)
{
  SET_ERRNO_AND_RETURN_IF_TRUE(NULL == callback, 0);

  return sli_si91x_socket(family, type, protocol, callback);
}

static int sli_si91x_socket(int family, int type, int protocol, receive_data_callback callback)
{
  // Validate the socket parameters
  SET_ERRNO_AND_RETURN_IF_TRUE(family != AF_INET && family != AF_INET6, EAFNOSUPPORT);
  SET_ERRNO_AND_RETURN_IF_TRUE(type != SOCK_STREAM && type != SOCK_DGRAM, EINVAL);
  SET_ERRNO_AND_RETURN_IF_TRUE(protocol != IPPROTO_TCP && protocol != IPPROTO_UDP && protocol != 0, EINVAL);
  SET_ERRNO_AND_RETURN_IF_TRUE((type == SOCK_STREAM && (protocol != IPPROTO_TCP && protocol != 0)), EPROTOTYPE);
  SET_ERRNO_AND_RETURN_IF_TRUE((type == SOCK_DGRAM && (protocol != IPPROTO_UDP && protocol != 0)), EPROTOTYPE);

  // Initialize a new socket structure
  si91x_socket_t *si91x_socket;
  int socket_index = -1;

  get_free_socket(&si91x_socket, &socket_index);

  // Check if there is enough memory to create the socket
  if (socket_index < 0) {
    SET_ERROR_AND_RETURN(ENOMEM);
  }

  // Populate the socket structure with provided parameters and callbacks
  si91x_socket->type                      = type;
  si91x_socket->local_address.sin6_family = (uint8_t)family;
  si91x_socket->protocol                  = protocol;
  si91x_socket->state                     = INITIALIZED;
  si91x_socket->recv_data_callback        = callback;

  // Return the socket index
  return socket_index;
}

int sl_si91x_bind(int socket, const struct sockaddr *addr, socklen_t addr_len)
{
  return sli_si91x_bind(socket, addr, addr_len);
}

int sl_si91x_connect(int socket, const struct sockaddr *addr, socklen_t addr_len)
{
  return sli_si91x_connect(socket, addr, addr_len);
}

int sl_si91x_listen(int socket, int max_number_of_clients)
{
  int32_t status;
  si91x_socket_t *si91x_socket = get_si91x_socket(socket);

  // Check if the socket is valid
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket == NULL, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket->state != INITIALIZED && si91x_socket->state != BOUND, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket->type != SOCK_STREAM, EOPNOTSUPP);

  // Create and send a socket request to make it a TCP server with the specified maximum number of clients
  status = create_and_send_socket_request(socket, SI91X_SOCKET_TCP_SERVER, &max_number_of_clients);
  SOCKET_VERIFY_STATUS_AND_RETURN(status, SI91X_NO_ERROR, SI91X_UNDEFINED_ERROR);

  si91x_socket->state = LISTEN;

  return SI91X_NO_ERROR;
}

int sl_si91x_accept(int socket, const struct sockaddr *addr, socklen_t addr_len)
{
  return sli_si91x_accept_async(socket, addr, addr_len, NULL);
}

int sl_si91x_shutdown(int socket, int how)
{
  return sli_si91x_shutdown(socket, how);
}

int sl_si91x_accept_async(int socket, accept_callback callback)
{
  return sli_si91x_accept_async(socket, NULL, 0, callback);
}

static int sli_si91x_accept_async(int socket, const struct sockaddr *addr, socklen_t addr_len, accept_callback callback)
{
  sl_status_t status = SL_STATUS_OK;
  si91x_socket_t *si91x_server_socket =
    get_si91x_socket(socket); // Get the server socket associated with the given socket ID
  si91x_socket_t *si91x_client_socket             = NULL;
  sl_si91x_socket_accept_request_t accept_request = { 0 };
  int32_t client_socket_id                        = -1;

  // Check if the server socket is valid
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_server_socket == NULL, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_server_socket->type != SOCK_STREAM, EOPNOTSUPP);
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_server_socket->state != LISTEN, EINVAL);

  // Create a new instance for socket
  client_socket_id = sli_si91x_socket(si91x_server_socket->local_address.sin6_family,
                                      si91x_server_socket->type,
                                      si91x_server_socket->protocol,
                                      si91x_server_socket->recv_data_callback);

  si91x_client_socket = get_si91x_socket(client_socket_id);
  //Verifying socket existence
  if (si91x_client_socket == NULL)
    return -1;
  memcpy(&si91x_client_socket->local_address, &si91x_server_socket->local_address, sizeof(struct sockaddr_in6));

  // Create accept request
  accept_request.socket_id   = (uint8_t)si91x_server_socket->id;
  accept_request.source_port = si91x_server_socket->local_address.sin6_port;

  // Set the callback and client socket ID.
  sli_si91x_set_accept_callback(si91x_server_socket, callback, client_socket_id);
  if (callback != NULL) {
    status = sl_si91x_driver_send_async_command(RSI_WLAN_REQ_SOCKET_ACCEPT,
                                                SI91X_SOCKET_CMD_QUEUE,
                                                &accept_request,
                                                sizeof(accept_request));
    SOCKET_VERIFY_STATUS_AND_RETURN(status, SL_STATUS_OK, SI91X_UNDEFINED_ERROR);
    return SL_STATUS_OK;
  } else {
    status = sli_si91x_sync_accept_command(si91x_server_socket, (void *)&accept_request, sizeof(accept_request));
  }

  // If the accept request fails, clean up allocated memory and return an error
  if (status != SL_STATUS_OK) {
    close(client_socket_id);
    SET_ERROR_AND_RETURN(SI91X_UNDEFINED_ERROR);
  }

  if ((addr != NULL) && (addr_len > 0)) {
    memcpy((struct sockaddr *)&addr,
           &si91x_client_socket->remote_address,
           (addr_len > sizeof(struct sockaddr_in6)) ? sizeof(struct sockaddr_in6) : addr_len);
  }

  return client_socket_id;
}

int sl_si91x_setsockopt_async(int32_t sockID,
                              int level,
                              int option_name,
                              const void *option_value,
                              socklen_t option_len)
{
  UNUSED_PARAMETER(level);

  // Retrieve the socket using the socket index
  si91x_socket_t *si91x_socket = get_si91x_socket(sockID);
  sl_si91x_time_value *timeout = NULL;
  uint16_t timeout_val;

  // Check if the socket is valid
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket == NULL, EBADF);

  // Check if the option value is not NULL
  SET_ERRNO_AND_RETURN_IF_TRUE(option_value == NULL, EFAULT);

  switch (option_name) {
    case SL_SI91X_SO_RCVTIME: {
      // Configure receive timeout
      timeout = (sl_si91x_time_value *)option_value;

      // Ensure that the timeout value is at least 1 millisecond
      if ((timeout->tv_sec == 0) && (timeout->tv_usec != 0) && (timeout->tv_usec < 1000)) {
        timeout->tv_usec = 1000;
      }
      // Calculate the timeout value in milliseconds
      timeout_val = (uint16_t)((timeout->tv_usec / 1000) + (timeout->tv_sec * 1000));

      // Need to add check here if Synchronous bit map is set (after async socket_id implementation)
      memcpy(&si91x_socket->read_timeout,
             &timeout_val,
             GET_SAFE_MEMCPY_LENGTH(sizeof(si91x_socket->read_timeout), option_len));
      break;
    }

    case SL_SI91X_SO_MAXRETRY: {
      // Set the maximum number of TCP retries
      memcpy(&si91x_socket->max_tcp_retries,
             (const uint16_t *)option_value,
             GET_SAFE_MEMCPY_LENGTH(sizeof(si91x_socket->max_tcp_retries), option_len));
      break;
    }

    case SL_SI91X_SO_MSS: {
      memcpy(&si91x_socket->mss,
             (const uint16_t *)option_value,
             GET_SAFE_MEMCPY_LENGTH(sizeof(si91x_socket->mss), option_len));
      break;
    }

    case SL_SI91X_SO_TCP_KEEPALIVE: {
      // Set the TCP keep-alive initial time
      memcpy(&si91x_socket->tcp_keepalive_initial_time,
             (const uint16_t *)option_value,
             GET_SAFE_MEMCPY_LENGTH(sizeof(si91x_socket->tcp_keepalive_initial_time), option_len));
      break;
    }
    case SL_SI91X_SO_HIGH_PERFORMANCE_SOCKET: {
      // Enable high-performance socket mode
      SET_ERRNO_AND_RETURN_IF_TRUE(*(uint8_t *)option_value != SI91X_HIGH_PERFORMANCE_SOCKET, EINVAL);
      si91x_socket->ssl_bitmap |= SI91X_HIGH_PERFORMANCE_SOCKET;
      break;
    }

    case SL_SI91X_SO_SSL_ENABLE: {
      // Enable SSL for the socket
      SET_ERRNO_AND_RETURN_IF_TRUE((*(uint8_t *)option_value) != SI91X_SOCKET_FEAT_SSL, EINVAL);
      si91x_socket->ssl_bitmap |= SI91X_SOCKET_FEAT_SSL;
      break;
    }
    case SL_SI91X_SO_SSL_V_1_0_ENABLE: {
      // Enable SSL version 1.0 for the socket
      SET_ERRNO_AND_RETURN_IF_TRUE(((*(uint8_t *)option_value) != (SI91X_SOCKET_FEAT_SSL | SL_SI91X_TLS_V_1_0)),
                                   EINVAL);
      si91x_socket->ssl_bitmap |= SI91X_SOCKET_FEAT_SSL | SL_SI91X_TLS_V_1_0;
      break;
    }
    case SL_SI91X_SO_SSL_V_1_1_ENABLE: {
      // Enable SSL version 1.1 for the socket
      SET_ERRNO_AND_RETURN_IF_TRUE(((*(uint8_t *)option_value) != (SI91X_SOCKET_FEAT_SSL | SL_SI91X_TLS_V_1_1)),
                                   EINVAL);
      si91x_socket->ssl_bitmap |= SI91X_SOCKET_FEAT_SSL | SL_SI91X_TLS_V_1_1;
      break;
    }
    case SL_SI91X_SO_SSL_V_1_2_ENABLE: {
      // Enable SSL version 1.2 for the socket
      SET_ERRNO_AND_RETURN_IF_TRUE(((*(uint8_t *)option_value) != (SI91X_SOCKET_FEAT_SSL | SL_SI91X_TLS_V_1_2)),
                                   EINVAL);
      si91x_socket->ssl_bitmap |= SI91X_SOCKET_FEAT_SSL | SL_SI91X_TLS_V_1_2;
      break;
    }

    case SL_SI91X_SO_SOCK_VAP_ID: {
      // Set the VAP ID for the socket
      si91x_socket->vap_id = *((const uint8_t *)option_value);
      break;
    }

    case SL_SI91x_SO_TCP_ACK_INDICATION: {
      // Enable TCP_ACK_INDICATION
      SET_ERRNO_AND_RETURN_IF_TRUE((*(uint8_t *)option_value) != SI91X_SOCKET_FEAT_TCP_ACK_INDICATION, EINVAL);
      si91x_socket->socket_bitmap |= SI91X_SOCKET_FEAT_TCP_ACK_INDICATION;
      break;
    }
#ifdef SLI_SI917
    case SL_SI91X_SO_SSL_V_1_3_ENABLE: {
      // Enable SSL version 1.3 for the socket.
      SET_ERRNO_AND_RETURN_IF_TRUE(((*(uint8_t *)option_value) != (SI91X_SOCKET_FEAT_SSL | SL_SI91X_TLS_V_1_3)),
                                   EINVAL);
      si91x_socket->ssl_bitmap |= SI91X_SOCKET_FEAT_SSL | SL_SI91X_TLS_V_1_3;
      break;
    }
#endif

    case SL_SI91X_SO_CERT_INDEX: {
      SET_ERRNO_AND_RETURN_IF_TRUE(
        ((*(uint8_t *)option_value < SI91X_CERT_INDEX_0) || (*(uint8_t *)option_value > SI91X_CERT_INDEX_2)),
        EINVAL);

      si91x_socket->certificate_index = *(const uint8_t *)option_value;
      break;
    }

    case SL_SI91X_SO_TLS_SNI:
    case SL_SI91X_SO_TLS_ALPN: {
      sl_status_t status = sli_si91x_add_tls_extension(&si91x_socket->tls_extensions,
                                                       (const sl_si91x_socket_type_length_value_t *)option_value);

      if (status != SL_STATUS_OK) {
        SET_ERROR_AND_RETURN(ENOMEM);
      }
      break;
    }

#ifdef SLI_SI917
    case SL_SI91X_SO_MAX_RETRANSMISSION_TIMEOUT_VALUE: {
      if (IS_POWER_OF_TWO((*(uint8_t *)option_value))
          && ((*(const uint8_t *)option_value) < MAX_RETRANSMISSION_TIME_VALUE)) {
        memcpy(&si91x_socket->max_retransmission_timeout_value,
               (const uint8_t *)option_value,
               GET_SAFE_MEMCPY_LENGTH(sizeof(si91x_socket->max_retransmission_timeout_value), option_len));
      } else {
        SL_DEBUG_LOG("\n Max retransmission timeout value in between 1 - 32 and "
                     "should be power of two. ex:1,2,4,8,16,32 \n");
        SET_ERROR_AND_RETURN(EINVAL);
      }
      break;
    }
#endif
    default: {
      // Invalid socket option
      SET_ERROR_AND_RETURN(ENOPROTOOPT);
    }
  }
  return SI91X_NO_ERROR;
}

int sl_si91x_send(int socket, const uint8_t *buffer, size_t buffer_length, int32_t flags)
{
  return sl_si91x_send_async(socket, buffer, buffer_length, flags, NULL);
}

int sl_si91x_send_async(int socket,
                        const uint8_t *buffer,
                        size_t buffer_length,
                        int32_t flags,
                        data_transfer_complete_handler callback)
{
  return sl_si91x_sendto_async(socket, buffer, buffer_length, flags, NULL, 0, callback);
}

int sl_si91x_sendto(int socket,
                    const uint8_t *buffer,
                    size_t buffer_length,
                    int32_t flags,
                    const struct sockaddr *addr,
                    socklen_t addr_len)
{
  return sl_si91x_sendto_async(socket, buffer, buffer_length, flags, addr, addr_len, NULL);
}

int sl_si91x_send_large_data(int socket, const uint8_t *buffer, size_t buffer_length, int32_t flags)
{
  const si91x_socket_t *si91x_socket = get_si91x_socket(socket);
  int bsd_ret_code                   = 0;
  size_t offset                      = 0;
  size_t chunk_size                  = 0;
  size_t max_len                     = 0;

  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket == NULL, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket->state == RESET || si91x_socket->state == INITIALIZED, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(buffer == NULL, EFAULT);

  // Find maximum limit based on the protocol
  if (si91x_socket->type == SOCK_STREAM && si91x_socket->ssl_bitmap & SL_SI91X_ENABLE_TLS) {
    max_len = (si91x_socket->local_address.sin6_family == AF_INET) ? si91x_socket->mss - SI91X_SSL_HEADER_SIZE_IPV4
                                                                   : si91x_socket->mss - SI91X_SSL_HEADER_SIZE_IPV6;
  } else if (si91x_socket->type == SOCK_DGRAM) {
    max_len = (si91x_socket->local_address.sin6_family == AF_INET) ? DEFAULT_DATAGRAM_MSS_SIZE_IPV4
                                                                   : DEFAULT_DATAGRAM_MSS_SIZE_IPV6;
  } else {
    max_len = (si91x_socket->local_address.sin6_family == AF_INET) ? DEFAULT_STREAM_MSS_SIZE_IPV4
                                                                   : DEFAULT_STREAM_MSS_SIZE_IPV6;
  }

  while (offset < buffer_length) {
    chunk_size = (max_len < (buffer_length - offset)) ? max_len : (buffer_length - offset);
    // Send chunk of data and return the total data sent in successful case
    bsd_ret_code = sl_si91x_send_async(socket, buffer + offset, chunk_size, flags, NULL);
    if (bsd_ret_code < 0) {
      SL_DEBUG_LOG("\n Send failed with error code 0x%X \n", errno);
      break;
    } else {
      offset += bsd_ret_code;
    }
  }
  return offset;
}

int sl_si91x_sendto_async(int socket,
                          const uint8_t *buffer,
                          size_t buffer_length,
                          int32_t flags,
                          const struct sockaddr *to_addr,
                          socklen_t to_addr_len,
                          data_transfer_complete_handler callback)
{

  UNUSED_PARAMETER(flags);
  sl_status_t status                     = SL_STATUS_OK;
  si91x_socket_t *si91x_socket           = get_si91x_socket(socket);
  sl_si91x_socket_send_request_t request = { 0 };

  // Check if the socket is valid
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket == NULL, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket->type == SOCK_STREAM && si91x_socket->state != CONNECTED, ENOTCONN);
  SET_ERRNO_AND_RETURN_IF_TRUE(buffer == NULL, EFAULT);
  if (si91x_socket->socket_bitmap & SI91X_SOCKET_FEAT_TCP_ACK_INDICATION) {
    SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket->is_waiting_on_ack == true, EWOULDBLOCK);
  }
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket->state != CONNECTED && to_addr == NULL, EFAULT);

  // Set the data transfer callback for this socket
  si91x_socket->data_transfer_callback = callback;

  // Check message size depending on socket type
  if (si91x_socket->type == SOCK_STREAM) {
    if (si91x_socket->ssl_bitmap & SL_SI91X_ENABLE_TLS) {
      // For SOCK_STREAM (TCP), consider SSL overhead if TLS is enabled
      size_t max_size = (si91x_socket->local_address.sin6_family == AF_INET)
                          ? si91x_socket->mss - SI91X_SSL_HEADER_SIZE_IPV4
                          : si91x_socket->mss - SI91X_SSL_HEADER_SIZE_IPV6;
      SET_ERRNO_AND_RETURN_IF_TRUE(buffer_length > max_size, EMSGSIZE);
    } else {
      size_t max_size = (si91x_socket->local_address.sin6_family == AF_INET) ? DEFAULT_STREAM_MSS_SIZE_IPV4
                                                                             : DEFAULT_STREAM_MSS_SIZE_IPV6;
      SET_ERRNO_AND_RETURN_IF_TRUE(buffer_length > max_size, EMSGSIZE);
    }
    if (si91x_socket->socket_bitmap & SI91X_SOCKET_FEAT_TCP_ACK_INDICATION) {
      // When using SOCK_STREAM (TCP), socket will wait for an ack if the SI91X_SOCKET_FEAT_TCP_ACK_INDICATION bit is set.
      si91x_socket->is_waiting_on_ack = true;
    }
  } else if (si91x_socket->type == SOCK_DGRAM) {
    // For SOCK_DGRAM (UDP), check the message size against the default maximum size
    size_t max_size = (si91x_socket->local_address.sin6_family == AF_INET) ? DEFAULT_DATAGRAM_MSS_SIZE_IPV4
                                                                           : DEFAULT_DATAGRAM_MSS_SIZE_IPV6;
    SET_ERRNO_AND_RETURN_IF_TRUE(buffer_length > max_size, EMSGSIZE);
  }

  if (si91x_socket->type == SOCK_DGRAM && (si91x_socket->state == BOUND || si91x_socket->state == INITIALIZED)) {
    status = create_and_send_socket_request(socket, SI91X_SOCKET_LUDP, NULL);

    SET_ERRNO_AND_RETURN_IF_TRUE(status != SL_STATUS_OK, SI91X_UNDEFINED_ERROR);
    si91x_socket->state = UDP_UNCONNECTED_READY;
  }

  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket->type == SOCK_DGRAM && si91x_socket->state != CONNECTED
                                 && si91x_socket->state != UDP_UNCONNECTED_READY,
                               EBADF);

  SET_ERRNO_AND_RETURN_IF_TRUE(
    si91x_socket->state == UDP_UNCONNECTED_READY
      && ((si91x_socket->local_address.sin6_family == AF_INET && to_addr_len < sizeof(struct sockaddr_in))
          || (si91x_socket->local_address.sin6_family == AF_INET6 && to_addr_len < sizeof(struct sockaddr_in6))),
    EINVAL);

  // create a socket send request
  if (si91x_socket->local_address.sin6_family == AF_INET6) {
    // If the socket uses IPv6, set the IP version and destination IPv6 address
    const struct sockaddr_in6 *socket_address = (const struct sockaddr_in6 *)to_addr;
    request.ip_version                        = SL_IPV6_ADDRESS_LENGTH;
    request.data_offset = (si91x_socket->type == SOCK_STREAM) ? TCP_V6_HEADER_LENGTH : UDP_V6_HEADER_LENGTH;
    const uint8_t *destination_ip =
      (si91x_socket->state == UDP_UNCONNECTED_READY || to_addr_len >= sizeof(struct sockaddr_in6))
        ? socket_address->sin6_addr.__u6_addr.__u6_addr8
        : si91x_socket->remote_address.sin6_addr.__u6_addr.__u6_addr8;

    memcpy(&request.dest_ip_addr.ipv6_address[0], destination_ip, SL_IPV6_ADDRESS_LENGTH);
  } else {
    // If the socket uses IPv4, set the IP version and destination IPv4 address
    struct sockaddr_in *socket_address = (struct sockaddr_in *)to_addr;
    request.ip_version                 = SL_IPV4_ADDRESS_LENGTH;
    request.data_offset                = (si91x_socket->type == SOCK_STREAM) ? TCP_HEADER_LENGTH : UDP_HEADER_LENGTH;
    uint32_t destination_ip =
      (si91x_socket->state == UDP_UNCONNECTED_READY || to_addr_len >= sizeof(struct sockaddr_in))
        ? socket_address->sin_addr.s_addr
        : ((struct sockaddr_in *)&si91x_socket->remote_address)->sin_addr.s_addr;

    memcpy(&request.dest_ip_addr.ipv4_address[0], &destination_ip, SL_IPV4_ADDRESS_LENGTH);
  }
  // Set other parameters in the send request
  request.socket_id = (uint16_t)si91x_socket->id;
  request.dest_port = (si91x_socket->state == UDP_UNCONNECTED_READY || to_addr_len > 0)
                        ? ((const struct sockaddr_in *)to_addr)->sin_port
                        : si91x_socket->remote_address.sin6_port;
  request.length    = buffer_length;

  // Send the socket data
  status = sl_si91x_driver_send_socket_data(&request, buffer, 0);
  if (status != SL_STATUS_OK && (si91x_socket->socket_bitmap & SI91X_SOCKET_FEAT_TCP_ACK_INDICATION)) {
    si91x_socket->is_waiting_on_ack = false;
  }
  SOCKET_VERIFY_STATUS_AND_RETURN(status, SL_STATUS_OK, ENOBUFS);

  return buffer_length;
}

int sl_si91x_recv(int socket, uint8_t *buf, size_t buf_len, int32_t flags)
{
  return sl_si91x_recvfrom(socket, buf, buf_len, flags, NULL, NULL);
}

int sl_si91x_recvfrom(int socket,
                      uint8_t *buf,
                      size_t buf_len,
                      int32_t flags,
                      struct sockaddr *addr,
                      socklen_t *addr_len)
{
  UNUSED_PARAMETER(flags);

  // Initialize variables for socket communication
  sl_si91x_wait_period_t wait_time     = 0;
  sl_si91x_req_socket_read_t request   = { 0 };
  uint32_t event                       = NCP_HOST_SOCKET_RESPONSE_EVENT;
  ssize_t bytes_read                   = 0;
  sl_si91x_socket_metadata_t *response = NULL;
  si91x_socket_t *si91x_socket         = get_si91x_socket(socket);
  sl_wifi_buffer_t *buffer             = NULL;
  void *sdk_context                    = NULL;

  // Check if the socket is valid
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket == NULL, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket->type == SOCK_STREAM && si91x_socket->state != CONNECTED, ENOTCONN);

  // Check if the buffer pointer is valid
  SET_ERRNO_AND_RETURN_IF_TRUE(buf == NULL, EFAULT);

  // Check if the specified buffer length is valid
  SET_ERRNO_AND_RETURN_IF_TRUE(buf_len <= 0, EINVAL);

  // create and send a socket request to configure it as UDP.
  if (si91x_socket->type == SOCK_DGRAM && (si91x_socket->state == BOUND || si91x_socket->state == INITIALIZED)) {
    int bsd_status = create_and_send_socket_request(socket, SI91X_SOCKET_LUDP, NULL);
    SOCKET_VERIFY_STATUS_AND_RETURN(bsd_status, SI91X_NO_ERROR, SI91X_UNDEFINED_ERROR);

    si91x_socket->state = UDP_UNCONNECTED_READY;
  }

  // Possible states are only reset and disconnected.
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket->state != CONNECTED && si91x_socket->state != UDP_UNCONNECTED_READY, EBADF);

  // Limit the buffer length based on the socket type
  if (si91x_socket->type == SOCK_STREAM) {
    if (buf_len > DEFAULT_STREAM_MSS_SIZE_IPV4 || buf_len > DEFAULT_STREAM_MSS_SIZE_IPV6)
      buf_len = (si91x_socket->local_address.sin6_family == AF_INET) ? DEFAULT_DATAGRAM_MSS_SIZE_IPV4
                                                                     : DEFAULT_DATAGRAM_MSS_SIZE_IPV6;
  } else if (si91x_socket->type == SOCK_DGRAM) {
    if (buf_len > DEFAULT_STREAM_MSS_SIZE_IPV4 || buf_len > DEFAULT_STREAM_MSS_SIZE_IPV6)
      buf_len = (si91x_socket->local_address.sin6_family == AF_INET) ? DEFAULT_DATAGRAM_MSS_SIZE_IPV4
                                                                     : DEFAULT_DATAGRAM_MSS_SIZE_IPV6;
  }

  // Initialize the socket read request with the socket ID and requested buffer length
  request.socket_id = (uint8_t)si91x_socket->id;
  sdk_context       = &(request.socket_id);
  memcpy(request.requested_bytes, &buf_len, sizeof(buf_len));
  wait_time = (SL_SI91X_WAIT_FOR_EVER | SL_SI91X_WAIT_FOR_RESPONSE_BIT);

  sl_status_t status = sl_si91x_socket_driver_send_command(RSI_WLAN_REQ_SOCKET_READ_DATA,
                                                           &request,
                                                           sizeof(request),
                                                           SI91X_SOCKET_CMD_QUEUE,
                                                           SI91X_SOCKET_RESPONSE_QUEUE,
                                                           &buffer,
                                                           (void *)&response,
                                                           &event,
                                                           &wait_time,
                                                           sdk_context);

  // If the command failed and a buffer was allocated, free the buffer
  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sl_si91x_host_free_buffer(buffer);
  }

  SOCKET_VERIFY_STATUS_AND_RETURN(status, SL_STATUS_OK, SI91X_UNDEFINED_ERROR);

  // Determine the number of bytes read, considering the buffer length and response length
  bytes_read = (response->length <= buf_len) ? response->length : buf_len;
  memcpy(buf, ((uint8_t *)response + response->offset), bytes_read);

  // If address information is provided, populate it based on the IP version
  if (addr != NULL) {
    if (response->ip_version == SL_IPV4_ADDRESS_LENGTH && *addr_len >= sizeof(struct sockaddr_in)) {
      struct sockaddr_in *socket_address = (struct sockaddr_in *)addr;

      socket_address->sin_port   = response->dest_port;
      socket_address->sin_family = AF_INET;
      memcpy(&socket_address->sin_addr.s_addr, response->dest_ip_addr.ipv4_address, SL_IPV4_ADDRESS_LENGTH);

      *addr_len = sizeof(struct sockaddr_in);
    } else if (response->ip_version == SL_IPV6_ADDRESS_LENGTH && *addr_len >= sizeof(struct sockaddr_in6)) {
      struct sockaddr_in6 *ipv6_socket_address = ((struct sockaddr_in6 *)addr);

      ipv6_socket_address->sin6_port   = response->dest_port;
      ipv6_socket_address->sin6_family = AF_INET;
      memcpy(&ipv6_socket_address->sin6_addr.__u6_addr.__u6_addr8,
             response->dest_ip_addr.ipv6_address,
             SL_IPV6_ADDRESS_LENGTH);

      *addr_len = sizeof(struct sockaddr_in6);
    } else {
      *addr_len = 0;
    }
  }

  sl_si91x_host_free_buffer(buffer);

  return bytes_read;
}

int sl_si91x_select(int nfds,
                    fd_set *readfds,
                    fd_set *writefds,
                    fd_set *exceptfds,
                    const struct timeval *timeout,
                    select_callback callback)
{
  UNUSED_PARAMETER(exceptfds);

  sl_status_t status = SL_STATUS_OK;

  if (callback == NULL) {
    int total_fd_set_count = select(nfds, readfds, writefds, exceptfds, timeout);
    if (total_fd_set_count == 0 || total_fd_set_count == -1) {
      return SI91X_UNDEFINED_ERROR;
    }
    return SI91X_NO_ERROR;
  }

  // Define a structure to hold the select request parameters
  sl_si91x_socket_select_req_t request = { 0 };

  // Check if all file descriptor sets are NULL
  // exceptfds are not being checked as firmware doesn't support it.
  if ((readfds == NULL) && (writefds == NULL)) {
    SET_ERROR_AND_RETURN(EINVAL); // Invalid argument, no sets specified
  }
  // Check if the number of file descriptors (nfds) is within a valid range
  if (nfds < 0 || nfds > NUMBER_OF_BSD_SOCKETS) {
    SET_ERROR_AND_RETURN(EINVAL); // Invalid argument, nfds out of range
  }

  // Check if the provided timeout is valid
  if ((timeout != NULL) && ((timeout->tv_sec < 0) || (timeout->tv_usec < 0))) {
    SET_ERROR_AND_RETURN(EINVAL); // Invalid argument, negative timeout
  }

  // Set the select callback function to be called upon completion
  set_select_callback(callback);

  // Loop through file descriptor sets and populate the select request structure
  for (uint8_t host_socket_index = 0; host_socket_index < nfds; host_socket_index++) {

    // Retrieve the si91x_socket associated with the host socket index
    const si91x_socket_t *socket = get_si91x_socket(host_socket_index);

    // Throw error if the socket file descriptor set by developer is not valid
    if (socket == NULL
        && ((readfds != NULL && FD_ISSET(host_socket_index, readfds))
            || (writefds != NULL && FD_ISSET(host_socket_index, writefds)))) {
      SET_ERROR_AND_RETURN(EBADF);
    }

    // If the socket is NULL, continue to iterate through other sockets.
    // The code will reach this if clause in the case of a socket being NULL and the socket being neither set in readfds nor writefds.
    if (socket == NULL) {
      continue;
    }

    // Check if the socket is set for read operations in the readfds set
    // Set the corresponding bit in the read file descriptor set
    if ((readfds != NULL) && (FD_ISSET(host_socket_index, readfds))) {
      request.read_fds.fd_array[0] |= (1U << socket->id);
    }

    // Check if the socket is set for write operations in the writefds set
    // Set the corresponding bit in the write file descriptor set
    if ((writefds != NULL) && (FD_ISSET(host_socket_index, writefds))) {
      request.write_fds.fd_array[0] |= (1U << socket->id);
    }

    // Update the maximum file descriptor number encountered
    if (request.num_fd <= socket->id) {
      request.num_fd = (uint8_t)(socket->id + 1);
    }
  }

  // Check if a timeout value is provided
  if (timeout != NULL) {
    request.select_timeout.tv_sec  = timeout->tv_sec;
    request.select_timeout.tv_usec = timeout->tv_usec;
  } else {
    // Indicate that there is no timeout specified
    request.no_timeout = 1;
  }

  status =
    sl_si91x_driver_send_async_command(RSI_WLAN_REQ_SELECT_REQUEST, SI91X_SOCKET_CMD_QUEUE, &request, sizeof(request));

  SOCKET_VERIFY_STATUS_AND_RETURN(status, SL_STATUS_OK, SI91X_UNDEFINED_ERROR);

  return SI91X_NO_ERROR;
}
