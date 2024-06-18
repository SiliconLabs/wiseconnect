/***************************************************************************/ /**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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
#include <string.h>
#include <stdint.h>

#include "sl_si91x_socket_utility.h"
#include "netdb.h"
#include "sl_si91x_driver.h"
#include "sl_rsi_utility.h"
#include "sl_si91x_status.h"
#include "sl_si91x_protocol_types.h"
#include "sl_si91x_host_interface.h"
#include "sl_ip_types.h"
#include "sl_net_dns.h"
#include "netinet_in.h"
#include "netinet6_in6.h"
#include "errno.h"
#include "socket.h"
#include "select.h"
#include "sl_bsd_utility.h"
#include "sl_si91x_socket_constants.h"
#include "sl_si91x_socket_support.h"
#include <sl_string.h>

/******************************************************

 *                    Constants
 ******************************************************/

#define SI91X_SOCKET_FEAT_TCP_ACK_INDICATION BIT(2)

#define SI91X_BSD_SOCKET_LOCAL_ADDRESS 0
#define SI91X_BSD_SOCKET_PEER_ADDRESS  1

#define TCP_HEADER_LENGTH                    56
#define UDP_HEADER_LENGTH                    44
#define TCP_V6_HEADER_LENGTH                 76
#define UDP_V6_HEADER_LENGTH                 64
#define SOCKET_LISTEN_RESPONSE_PACKET_LENGTH 42

#define SI91X_SSL_HEADER_SIZE_IPV4 90
#define SI91X_SSL_HEADER_SIZE_IPV6 110

/******************************************************
 *               Variable Definitions
 ******************************************************/
// Declaration of the global error number variable for hostname resolution errors
static int herrno;
// Structure to hold host entry information for hostname resolution
static struct hostent host_ent;
// IP address structure for host IP address resolution
static sl_ip_address_t host_ip_address;

/******************************************************
 *               Function Declarations
 ******************************************************/

int sl_si91x_set_custom_sync_sockopt(int socket_id,
                                     int option_level,
                                     int option_name,
                                     const void *option_value,
                                     socklen_t option_length);
int sl_si91x_get_custom_sync_sockopt(int socket_id,
                                     int option_level,
                                     int option_name,
                                     void *option_value,
                                     socklen_t *option_length);

/******************************************************
 *               Function Definitions
 ******************************************************/

int *__h_errno_location(void)
{
  return &herrno;
}

int16_t sl_si91x_get_socket_mss(int32_t socketIndex)
{
  // Retrieve the SI91X socket associated with the given socket index
  si91x_socket_t *si91x_socket = get_si91x_socket(socketIndex);
  //Verifying socket existence
  if (si91x_socket == NULL)
    return -1;

  // Calculate and return the MSS based on the socket type and state
  if ((si91x_socket->type == SOCK_STREAM) && si91x_socket->state == CONNECTED) {
    if (si91x_socket->ssl_bitmap & SL_SI91X_ENABLE_TLS) {
      // Subtract TLS header size from MSS for TLS connections
      size_t max_size = (si91x_socket->local_address.sin6_family == AF_INET)
                          ? si91x_socket->mss - SI91X_SSL_HEADER_SIZE_IPV4
                          : si91x_socket->mss - SI91X_SSL_HEADER_SIZE_IPV6;
      return max_size;
    } else {
      size_t max_size = (si91x_socket->local_address.sin6_family == AF_INET) ? DEFAULT_STREAM_MSS_SIZE_IPV4
                                                                             : DEFAULT_STREAM_MSS_SIZE_IPV6;
      return max_size;
    }
  } else if (si91x_socket->type == SOCK_DGRAM) {
    size_t max_size = (si91x_socket->local_address.sin6_family == AF_INET) ? DEFAULT_DATAGRAM_MSS_SIZE_IPV4
                                                                           : DEFAULT_DATAGRAM_MSS_SIZE_IPV6;
    return max_size;
  }
  return -1;
}

sl_status_t sl_si91x_get_socket_info(sl_si91x_socket_info_response_t *socket_info_response)
{
  SL_WIFI_ARGS_CHECK_NULL_POINTER(socket_info_response);

  sl_status_t status                           = SL_STATUS_FAIL;
  sl_wifi_buffer_t *buffer                     = NULL;
  sl_si91x_network_params_response_t *response = NULL;

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_QUERY_NETWORK_PARAMS,
                                        SI91X_WLAN_CMD_QUEUE,
                                        NULL,
                                        0,
                                        SL_SI91X_WAIT_FOR_RESPONSE(SL_SI91X_GET_CHANNEL_TIMEOUT),
                                        NULL,
                                        &buffer);

  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sl_si91x_host_free_buffer(buffer);
  }

  VERIFY_STATUS_AND_RETURN(status);

  sl_si91x_packet_t *packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  response                  = (sl_si91x_network_params_response_t *)packet->data;

  memcpy(&socket_info_response->number_of_opened_sockets,
         response->num_open_socks,
         sizeof(socket_info_response->number_of_opened_sockets));
  memcpy(socket_info_response->socket_info,
         response->socket_info,
         (sizeof(sl_si91x_sock_info_query_t) * socket_info_response->number_of_opened_sockets));

  sl_si91x_host_free_buffer(buffer);
  return SL_STATUS_OK;
}

/**
 * A static utility function to get either peer address or socket address given the socket ID.
 * @param socket_id 			ID of socket.
 * @param name 					structure to hold the IP address, port, family type of IP address
 * @param name_len 				size of of buffer sent by caller.
 * @param required_address_type Whether peer data or local data to be copied to buffer.
 * @return						0 when successes, otherwise -1.
 */
static int sli_si91x_get_sock_address(int socket_id,
                                      struct sockaddr *name,
                                      socklen_t *name_len,
                                      uint8_t required_address_type)
{
  // Retrieve the SI91X socket associated with the given socket ID
  si91x_socket_t *si91x_socket = get_si91x_socket(socket_id);
  // Determine the number of bytes to copy
  uint8_t bytes_to_be_copied = (*name_len > sizeof(struct sockaddr_in6)) ? sizeof(struct sockaddr_in6) : *name_len;

  // Validate socket
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket == NULL, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(name == NULL || name_len == NULL, EFAULT);
  SET_ERRNO_AND_RETURN_IF_TRUE(*name_len <= 0, EINVAL);

  // Ensure the required conditions for copying the address are met
  SET_ERRNO_AND_RETURN_IF_TRUE(
    required_address_type == SI91X_BSD_SOCKET_PEER_ADDRESS && si91x_socket->state != CONNECTED,
    ENOTCONN)
  SET_ERRNO_AND_RETURN_IF_TRUE(required_address_type == SI91X_BSD_SOCKET_LOCAL_ADDRESS
                                 && (si91x_socket->state < BOUND || si91x_socket->state == DISCONNECTED),
                               EBADF);

  // Copy the address based on the required type (local or peer).
  if (required_address_type == SI91X_BSD_SOCKET_LOCAL_ADDRESS) {
    memcpy(name, &si91x_socket->local_address, bytes_to_be_copied);
  } else {
    memcpy(name, &si91x_socket->remote_address, bytes_to_be_copied);
  }

  // Set the actual length of the copied address structure
  *name_len = si91x_socket->local_address.sin6_family == AF_INET ? sizeof(struct sockaddr_in)
                                                                 : sizeof(struct sockaddr_in6);
  return SI91X_NO_ERROR;
}

int socket(int family, int type, int protocol)
{
  // Check for valid values of family, type, and protocol
  SET_ERRNO_AND_RETURN_IF_TRUE(family != AF_INET && family != AF_INET6, EAFNOSUPPORT);

  SET_ERRNO_AND_RETURN_IF_TRUE(type != SOCK_STREAM && type != SOCK_DGRAM, EINVAL);
  SET_ERRNO_AND_RETURN_IF_TRUE(protocol != IPPROTO_TCP && protocol != IPPROTO_UDP && protocol != 0, EINVAL);

  SET_ERRNO_AND_RETURN_IF_TRUE((type == SOCK_STREAM && (protocol != IPPROTO_TCP && protocol != 0)), EPROTOTYPE);
  SET_ERRNO_AND_RETURN_IF_TRUE((type == SOCK_DGRAM && (protocol != IPPROTO_UDP && protocol != 0)), EPROTOTYPE);

  int socket_index             = -1;
  si91x_socket_t *si91x_socket = NULL;

  // Allocate a socket and initialize its properties.
  get_free_socket(&si91x_socket, &socket_index);

  if (socket_index < 0) {
    SET_ERROR_AND_RETURN(ENOMEM);
  }

  si91x_socket->type                      = type;
  si91x_socket->local_address.sin6_family = family;
  si91x_socket->protocol                  = protocol;
  si91x_socket->state                     = INITIALIZED;

  return socket_index;
}

int bind(int socket_id, const struct sockaddr *addr, socklen_t addr_len)
{
  errno = 0;

  // Retrieve the SI91X socket associated with the given socket ID.
  si91x_socket_t *si91x_socket       = get_si91x_socket(socket_id);
  struct sockaddr_in *socket_address = (struct sockaddr_in *)addr;

  // Validate socket, address, and address length
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket == NULL || si91x_socket->state != INITIALIZED, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(
    (si91x_socket->local_address.sin6_family == AF_INET && addr_len < sizeof(struct sockaddr_in))
      || (si91x_socket->local_address.sin6_family == AF_INET6 && addr_len < sizeof(struct sockaddr_in6)),
    EINVAL);

  SET_ERRNO_AND_RETURN_IF_TRUE(addr == NULL, EFAULT);

  // Check whether local port is already used or not
  if (!is_port_available(socket_address->sin_port)) {
    SET_ERROR_AND_RETURN(EADDRINUSE)
  }

  // Copy the provided address to the local address structure
  memcpy(&si91x_socket->local_address,
         addr,
         (addr_len > sizeof(struct sockaddr_in6)) ? sizeof(struct sockaddr_in6) : addr_len);

  si91x_socket->state = BOUND;

  // For UDP sockets, create and send a socket request.
  if (si91x_socket->type == SOCK_DGRAM) {
    sl_status_t socket_create_request_status = create_and_send_socket_request(socket_id, SI91X_SOCKET_LUDP, NULL);
    SOCKET_VERIFY_STATUS_AND_RETURN(socket_create_request_status, SI91X_NO_ERROR, SI91X_UNDEFINED_ERROR);

    si91x_socket->state = UDP_UNCONNECTED_READY;
  }

  return SI91X_NO_ERROR;
}

int listen(int socket_id, int backlog)
{
  errno = 0;

  sl_status_t status;
  // Retrieve the SI91X socket associated with the given socket ID.
  si91x_socket_t *si91x_socket = get_si91x_socket(socket_id);

  // Validate socket
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket == NULL, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket->state != INITIALIZED && si91x_socket->state != BOUND, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket->type != SOCK_STREAM, EOPNOTSUPP);

  status = create_and_send_socket_request(socket_id, SI91X_SOCKET_TCP_SERVER, (int *)&backlog);

  SOCKET_VERIFY_STATUS_AND_RETURN(status, SL_STATUS_OK, SI91X_UNDEFINED_ERROR);

  si91x_socket->state = LISTEN;

  return SI91X_NO_ERROR;
}

int accept(int socket_id, struct sockaddr *addr, socklen_t *addr_len)
{
  sl_si91x_wait_period_t wait_time = 0;
  errno                            = 0;

  // Get the server socket
  si91x_socket_t *si91x_server_socket = get_si91x_socket(socket_id);
  si91x_socket_t *si91x_client_socket = NULL;

  sl_si91x_socket_accept_request_t accept_request = { 0 };
  sl_wifi_buffer_t *buffer                        = NULL;
  sl_si91x_rsp_ltcp_est_t *ltcp                   = NULL;
  void *sdk_context                               = NULL;

  sl_status_t status       = SL_STATUS_OK;
  int32_t client_socket_id = -1;
  uint32_t events          = NCP_HOST_SOCKET_RESPONSE_EVENT;

  // Check if server socket is valid and in the correct state
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_server_socket == NULL, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_server_socket->type != SOCK_STREAM, EOPNOTSUPP);
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_server_socket->state != LISTEN, EINVAL);

  // Create a new instance for socket
  client_socket_id =
    socket(si91x_server_socket->local_address.sin6_family, si91x_server_socket->type, si91x_server_socket->protocol);
  si91x_client_socket = get_si91x_socket(client_socket_id);
  //Verifying socket existence
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_client_socket == NULL, SI91X_UNDEFINED_ERROR);
  // Copy the local address information to the client socket
  memcpy(&si91x_client_socket->local_address, &si91x_server_socket->local_address, sizeof(struct sockaddr_in6));

  // populating socket info to accept request structure
  accept_request.socket_id   = si91x_server_socket->id;
  sdk_context                = &(accept_request.socket_id);
  accept_request.source_port = si91x_server_socket->local_address.sin6_port;
  wait_time                  = (SL_SI91X_WAIT_FOR_EVER | SL_SI91X_WAIT_FOR_RESPONSE_BIT);

  status = sl_si91x_socket_driver_send_command(RSI_WLAN_REQ_SOCKET_ACCEPT,
                                               &accept_request,
                                               sizeof(accept_request),
                                               SI91X_SOCKET_CMD_QUEUE,
                                               SI91X_SOCKET_RESPONSE_QUEUE,
                                               &buffer,
                                               (void *)&ltcp,
                                               &events,
                                               &wait_time,
                                               sdk_context);

  if (status != SL_STATUS_OK) {
    // Close the client socket and free resources if the connection fails
    close(client_socket_id);
    if (buffer != NULL) {
      sl_si91x_host_free_buffer(buffer);
    }
    SET_ERROR_AND_RETURN(SI91X_UNDEFINED_ERROR);
  }

  // Populate the client socket info based on the accepted connection
  si91x_client_socket->id                         = ltcp->socket_id;
  si91x_client_socket->local_address.sin6_port    = ltcp->src_port_num;
  si91x_client_socket->remote_address.sin6_port   = ltcp->dest_port;
  si91x_client_socket->mss                        = ltcp->mss;
  si91x_client_socket->state                      = CONNECTED;
  si91x_client_socket->remote_address.sin6_family = ltcp->ip_version == SL_IPV6_VERSION ? AF_INET6 : AF_INET;

  // Copy the destination IP address to the client socket
  if (si91x_client_socket->remote_address.sin6_family == AF_INET6) {
    memcpy(si91x_client_socket->remote_address.sin6_addr.__u6_addr.__u6_addr8,
           ltcp->dest_ip_addr.ipv6_address,
           SL_IPV6_ADDRESS_LENGTH);

  } else {
    memcpy(&((struct sockaddr_in *)&si91x_client_socket->remote_address)->sin_addr,
           ltcp->dest_ip_addr.ipv4_address,
           SL_IPV4_ADDRESS_LENGTH);
  }

  // If addr_len is NULL or invalid value, just return the client socket ID
  if (addr_len == NULL || *addr_len <= 0) {
    if (buffer != NULL) {
      sl_si91x_host_free_buffer(buffer);
    }
    return client_socket_id;
  }

  // Copy the remote address to the provided sockaddr structure
  memcpy(addr,
         &si91x_client_socket->remote_address,
         (*addr_len > sizeof(struct sockaddr_in6)) ? sizeof(struct sockaddr_in6) : *addr_len);

  // Update addr_len based on the family of the local address
  *addr_len = si91x_client_socket->local_address.sin6_family == AF_INET ? sizeof(struct sockaddr_in)
                                                                        : sizeof(struct sockaddr_in6);

  // Free resources and return the client socket ID
  if (buffer != NULL) {
    sl_si91x_host_free_buffer(buffer);
  }
  return client_socket_id;
}

int connect(int socket_id, const struct sockaddr *addr, socklen_t addr_len)
{
  return sli_si91x_connect(socket_id, addr, addr_len);
}

ssize_t send(int socket_id, const void *data, size_t data_len, int flags)
{
  // Sending data to a socket without specifying a destination address
  return sendto(socket_id, data, data_len, flags, NULL, 0);
}

ssize_t recv(int socket_id, void *buf, size_t buf_len, int flags)
{
  // Receiving data from a socket without specifying the source address
  return recvfrom(socket_id, buf, buf_len, flags, NULL, NULL);
}

ssize_t sendto(int socket_id,
               const void *data,
               size_t data_len,
               int flags,
               const struct sockaddr *to_addr,
               socklen_t to_addr_len)
{
  // Initialize variables and error handling
  UNUSED_PARAMETER(flags);
  errno = 0;

  sl_status_t status                     = SL_STATUS_OK;
  si91x_socket_t *si91x_socket           = get_si91x_socket(socket_id);
  sl_si91x_socket_send_request_t request = { 0 };

  // Check for various error conditions
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket == NULL, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket->type == SOCK_STREAM && si91x_socket->state != CONNECTED, ENOTCONN);
  SET_ERRNO_AND_RETURN_IF_TRUE(data == NULL, EFAULT);

  //! TBD
  if (si91x_socket->type == SOCK_STREAM) {
    if (si91x_socket->ssl_bitmap & SL_SI91X_ENABLE_TLS) {
      // Check if TLS is enabled for IPV4 or IPV6 and data length exceeds allowed size
      size_t max_size = (si91x_socket->local_address.sin6_family == AF_INET)
                          ? si91x_socket->mss - SI91X_SSL_HEADER_SIZE_IPV4
                          : si91x_socket->mss - SI91X_SSL_HEADER_SIZE_IPV6;
      SET_ERRNO_AND_RETURN_IF_TRUE(data_len > max_size, EMSGSIZE);
    } else {
      size_t max_size = (si91x_socket->local_address.sin6_family == AF_INET) ? DEFAULT_STREAM_MSS_SIZE_IPV4
                                                                             : DEFAULT_STREAM_MSS_SIZE_IPV6;
      SET_ERRNO_AND_RETURN_IF_TRUE(data_len > max_size, EMSGSIZE);
    }
  } else if (si91x_socket->type == SOCK_DGRAM) {
    // For UDP sockets, check if data length exceeds allowed size
    size_t max_size = (si91x_socket->local_address.sin6_family == AF_INET) ? DEFAULT_DATAGRAM_MSS_SIZE_IPV4
                                                                           : DEFAULT_DATAGRAM_MSS_SIZE_IPV6;
    SET_ERRNO_AND_RETURN_IF_TRUE(data_len > max_size, EMSGSIZE);
  }

  // If it's a UDP socket in an unconnected state, establish a connection
  if (si91x_socket->type == SOCK_DGRAM && (si91x_socket->state == BOUND || si91x_socket->state == INITIALIZED)) {
    sl_status_t status = create_and_send_socket_request(socket_id, SI91X_SOCKET_LUDP, NULL);

    SET_ERRNO_AND_RETURN_IF_TRUE(status != SL_STATUS_OK, SI91X_UNDEFINED_ERROR);
    si91x_socket->state = UDP_UNCONNECTED_READY;
  }

  // Possible state of socket can be reset or disconnect only.
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket->type == SOCK_DGRAM && si91x_socket->state != CONNECTED
                                 && si91x_socket->state != UDP_UNCONNECTED_READY,
                               EBADF);

  SET_ERRNO_AND_RETURN_IF_TRUE(
    si91x_socket->state == UDP_UNCONNECTED_READY
      && ((si91x_socket->local_address.sin6_family == AF_INET && to_addr_len < sizeof(struct sockaddr_in))
          || (si91x_socket->local_address.sin6_family == AF_INET6 && to_addr_len < sizeof(struct sockaddr_in6))),
    EINVAL)

  // create a socket send request
  if (si91x_socket->local_address.sin6_family == AF_INET6) {
    struct sockaddr_in6 *socket_address = (struct sockaddr_in6 *)to_addr;
    request.ip_version                  = SL_IPV6_VERSION;
    request.data_offset = (si91x_socket->type == SOCK_STREAM) ? TCP_V6_HEADER_LENGTH : UDP_V6_HEADER_LENGTH;
    uint8_t *destination_ip =
      (si91x_socket->state == UDP_UNCONNECTED_READY || to_addr_len >= sizeof(struct sockaddr_in6))
        ? socket_address->sin6_addr.__u6_addr.__u6_addr8
        : si91x_socket->remote_address.sin6_addr.__u6_addr.__u6_addr8;

    memcpy(request.dest_ip_addr.ipv6_address, destination_ip, SL_IPV6_ADDRESS_LENGTH);
  } else {
    struct sockaddr_in *socket_address = (struct sockaddr_in *)to_addr;
    request.ip_version                 = SL_IPV4_VERSION;
    request.data_offset                = (si91x_socket->type == SOCK_STREAM) ? TCP_HEADER_LENGTH : UDP_HEADER_LENGTH;
    uint32_t *destination_ip =
      (si91x_socket->state == UDP_UNCONNECTED_READY || to_addr_len >= sizeof(struct sockaddr_in))
        ? &socket_address->sin_addr.s_addr
        : &((struct sockaddr_in *)&si91x_socket->remote_address)->sin_addr.s_addr;

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif // __GNUC__
    memcpy(request.dest_ip_addr.ipv4_address, destination_ip, SL_IPV4_ADDRESS_LENGTH);
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif // __GNUC__
  }

  // Set other parameters in the request structure
  request.socket_id = si91x_socket->id;
  request.dest_port = ((si91x_socket->state == UDP_UNCONNECTED_READY || to_addr_len > 0))
                        ? ((struct sockaddr_in *)to_addr)->sin_port
                        : si91x_socket->remote_address.sin6_port;
  request.length    = data_len;

  // Send the socket data request
  status = sl_si91x_driver_send_socket_data(&request, data, 0);
  SOCKET_VERIFY_STATUS_AND_RETURN(status, SL_STATUS_OK, ENOBUFS);

  return data_len;
}

ssize_t recvfrom(int socket_id, void *buf, size_t buf_len, int flags, struct sockaddr *addr, socklen_t *addr_len)
{
  UNUSED_PARAMETER(flags); // Ignoring the 'flags' parameter
  sl_si91x_wait_period_t wait_time = 0;
  errno                            = 0;

  sl_si91x_req_socket_read_t request = { 0 }; // Initialize a request structure
  sl_status_t status                 = SL_STATUS_OK;
  uint32_t event                     = NCP_HOST_SOCKET_RESPONSE_EVENT; // Event identifier
  ssize_t bytes_read                 = 0;                              // Number of bytes read
  si91x_rsp_socket_recv_t *response  = NULL;                           // Response structure
  si91x_socket_t *si91x_socket       = get_si91x_socket(socket_id);    // Get socket information
  void *sdk_context                  = NULL;

  sl_wifi_buffer_t *buffer = NULL;

  // Check for error conditions and return appropriate error codes
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket == NULL, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket->type == SOCK_STREAM && si91x_socket->state != CONNECTED, ENOTCONN);
  SET_ERRNO_AND_RETURN_IF_TRUE(buf == NULL, EFAULT);
  SET_ERRNO_AND_RETURN_IF_TRUE(buf_len <= 0, EINVAL)

  // If it's a UDP socket and not yet initialized, initialize it
  if (si91x_socket->type == SOCK_DGRAM && (si91x_socket->state == BOUND || si91x_socket->state == INITIALIZED)) {
    sl_status_t bsd_status = create_and_send_socket_request(socket_id, SI91X_SOCKET_LUDP, NULL);
    SOCKET_VERIFY_STATUS_AND_RETURN(bsd_status, SL_STATUS_OK, SI91X_UNDEFINED_ERROR)

    si91x_socket->state = UDP_UNCONNECTED_READY;
  }

  // Possible states are only reset and disconnected.
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket->state != CONNECTED && si91x_socket->state != UDP_UNCONNECTED_READY, EBADF)

  // Limit buffer length based on socket type
  if (si91x_socket->type == SOCK_STREAM) {
    if (buf_len > DEFAULT_STREAM_MSS_SIZE_IPV4 || buf_len > DEFAULT_STREAM_MSS_SIZE_IPV6)
      buf_len = (si91x_socket->local_address.sin6_family == AF_INET) ? DEFAULT_DATAGRAM_MSS_SIZE_IPV4
                                                                     : DEFAULT_DATAGRAM_MSS_SIZE_IPV6;
  } else if (si91x_socket->type == SOCK_DGRAM) {
    if (buf_len > DEFAULT_STREAM_MSS_SIZE_IPV4 || buf_len > DEFAULT_STREAM_MSS_SIZE_IPV6)
      buf_len = (si91x_socket->local_address.sin6_family == AF_INET) ? DEFAULT_DATAGRAM_MSS_SIZE_IPV4
                                                                     : DEFAULT_DATAGRAM_MSS_SIZE_IPV6;
  }

  // Prepare the request structure with socket and buffer information
  request.socket_id = (uint8_t)si91x_socket->id;
  sdk_context       = &(request.socket_id);
  memcpy(request.requested_bytes, &buf_len, sizeof(buf_len));
  memcpy(request.read_timeout, &si91x_socket->read_timeout, sizeof(request.read_timeout));

  // Configure the wait time for the socket response
  wait_time = (SL_SI91X_WAIT_FOR_EVER | SL_SI91X_WAIT_FOR_RESPONSE_BIT);

  // Send the command to read data from the socket
  status = sl_si91x_socket_driver_send_command(RSI_WLAN_REQ_SOCKET_READ_DATA,
                                               &request,
                                               sizeof(request),
                                               SI91X_SOCKET_CMD_QUEUE,
                                               SI91X_SOCKET_RESPONSE_QUEUE,
                                               &buffer,
                                               (void *)&response,
                                               &event,
                                               &wait_time,
                                               sdk_context);

  // Free the buffer if there was an error
  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sl_si91x_host_free_buffer(buffer);
  }
  SOCKET_VERIFY_STATUS_AND_RETURN(status, SL_STATUS_OK, SI91X_UNDEFINED_ERROR);

  // Determine the number of bytes to copy
  bytes_read = (ssize_t)((response->length <= buf_len) ? response->length : buf_len);

  memcpy(buf, ((uint8_t *)response + response->offset), (size_t)bytes_read);

  // If an address structure is provided, fill it with destination address information
  if (addr != NULL) {
    if (response->ip_version == SL_IPV4_VERSION && *addr_len >= sizeof(struct sockaddr_in)) {
      struct sockaddr_in *socket_address = (struct sockaddr_in *)addr;

      socket_address->sin_port   = response->dest_port;
      socket_address->sin_family = AF_INET;
      memcpy(&socket_address->sin_addr.s_addr, response->dest_ip_addr.ipv4_address, SL_IPV4_ADDRESS_LENGTH);

      *addr_len = sizeof(struct sockaddr_in);
    } else if (response->ip_version == SL_IPV6_VERSION && *addr_len >= sizeof(struct sockaddr_in6)) {
      struct sockaddr_in6 *ipv6_socket_address = ((struct sockaddr_in6 *)addr);

      ipv6_socket_address->sin6_port   = response->dest_port;
      ipv6_socket_address->sin6_family = AF_INET6;
      memcpy(&ipv6_socket_address->sin6_addr.__u6_addr.__u6_addr8,
             response->dest_ip_addr.ipv6_address,
             SL_IPV6_ADDRESS_LENGTH);

      *addr_len = sizeof(struct sockaddr_in6);
    } else {
      // Not BSD compliant.
      *addr_len = 0;
    }
  }

  // Free the buffer
  sl_si91x_host_free_buffer(buffer);
  return bytes_read;
}

int getsockname(int socket_id, struct sockaddr *name, socklen_t *name_len)
{
  return sli_si91x_get_sock_address(socket_id, name, name_len, SI91X_BSD_SOCKET_LOCAL_ADDRESS);
}

int getpeername(int socket_id, struct sockaddr *name, socklen_t *name_len)
{
  return sli_si91x_get_sock_address(socket_id, name, name_len, SI91X_BSD_SOCKET_PEER_ADDRESS);
}

int setsockopt(int socket_id, int option_level, int option_name, const void *option_value, socklen_t option_length)
{
  si91x_socket_t *si91x_socket = get_si91x_socket(socket_id);
  sl_si91x_time_value *timeout = NULL;
  uint16_t timeout_val;

  // Check if the socket is valid
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket == NULL, EBADF);
  // Check if the option value is valid
  SET_ERRNO_AND_RETURN_IF_TRUE(option_value == NULL, EFAULT)
  // Check if the option_level is valid
  SET_ERRNO_AND_RETURN_IF_TRUE(
    ((option_level != SOL_SOCKET) && (option_level != SOL_TCP) && (option_level != IPPROTO_IP)),
    EINVAL);

  switch (option_name) {
    case SO_RCVTIMEO: {
      // Configure receive timeout
      timeout = (sl_si91x_time_value *)option_value;
      if ((timeout->tv_sec == 0) && (timeout->tv_usec != 0) && (timeout->tv_usec < 1000)) {
        timeout->tv_usec = 1000;
      }
      timeout_val = (timeout->tv_usec / 1000) + (timeout->tv_sec * 1000);

      // Need to add check here if Synchronous bit map is set (after async socket_id implementation)
      memcpy(&si91x_socket->read_timeout,
             &timeout_val,
             GET_SAFE_MEMCPY_LENGTH(sizeof(si91x_socket->read_timeout), option_length));
      break;
    }

    case SO_KEEPALIVE: {
      // Set TCP keep-alive initial time
      memcpy(&si91x_socket->tcp_keepalive_initial_time,
             (uint16_t *)option_value,
             GET_SAFE_MEMCPY_LENGTH(sizeof(si91x_socket->tcp_keepalive_initial_time), option_length));
      break;
    }

    case TCP_ULP: {
      // Set TLS version based on the provided option_value
      if ((sizeof(TLS_1_2) != option_length) && (sizeof(TLS) != option_length)) {
        SET_ERROR_AND_RETURN(EINVAL);
      }
      if (strncmp(option_value, TLS, option_length) == 0) {
        si91x_socket->ssl_bitmap = SL_SI91X_ENABLE_TLS;
      } else if (strncmp(option_value, TLS_1_2, option_length) == 0) {
        si91x_socket->ssl_bitmap = SL_SI91X_ENABLE_TLS | SL_SI91X_TLS_V_1_2;
      } else if (strncmp(option_value, TLS_1_1, option_length) == 0) {
        si91x_socket->ssl_bitmap = SL_SI91X_ENABLE_TLS | SL_SI91X_TLS_V_1_1;
      } else if (strncmp(option_value, TLS_1_0, option_length) == 0) {
        si91x_socket->ssl_bitmap = SL_SI91X_ENABLE_TLS | SL_SI91X_TLS_V_1_0;
      }
#ifdef SLI_SI917
      else if (strncmp(option_value, TLS_1_3, option_length) == 0) {
        si91x_socket->ssl_bitmap = SL_SI91X_ENABLE_TLS | SL_SI91X_TLS_V_1_3;
      }
#endif
      break;
    }

#ifdef SLI_SI917
    case SO_MAX_RETRANSMISSION_TIMEOUT_VALUE: {
      // Set max retransmission timeout value with bounds check
      if (IS_POWER_OF_TWO(*(uint8_t *)option_value) && ((*(uint8_t *)option_value) < MAX_RETRANSMISSION_TIME_VALUE)) {
        memcpy(&si91x_socket->max_retransmission_timeout_value,
               (uint8_t *)option_value,
               GET_SAFE_MEMCPY_LENGTH(sizeof(si91x_socket->max_retransmission_timeout_value), option_length));
      } else {
        SL_DEBUG_LOG("\n Max retransmission timeout value in between 1 - 32 and "
                     "should be power of two. ex:1,2,4,8,16,32 \n");
        SET_ERROR_AND_RETURN(EINVAL);
      }
      break;
    }

    case IP_TOS: {
      // Set IP TOS (Type of Service)
      memcpy(&si91x_socket->tos,
             (uint8_t *)option_value,
             GET_SAFE_MEMCPY_LENGTH(sizeof(si91x_socket->tos), option_length));
      break;
    }
#endif
    default: {
      // Unsupported option
      SET_ERROR_AND_RETURN(ENOPROTOOPT);
    }
  }

  return SI91X_NO_ERROR;
}

int sl_si91x_set_custom_sync_sockopt(int socket_id,
                                     int option_level,
                                     int option_name,
                                     const void *option_value,
                                     socklen_t option_length)
{
  UNUSED_PARAMETER(option_length);
  // Get a reference to the socket associated with the given socket id
  si91x_socket_t *si91x_socket = get_si91x_socket(socket_id);

  // Check if socket is valid
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket == NULL, EBADF);

  // Check if option_value is valid
  SET_ERRNO_AND_RETURN_IF_TRUE(option_value == NULL, EFAULT)

  // Check if option_level is SOL_SOCKET
  SET_ERRNO_AND_RETURN_IF_TRUE(
    ((option_level != SOL_SOCKET) && (option_level != SOL_TCP) && (option_level != IPPROTO_IP)),
    EINVAL);

  // Handle different socket options based on option_name
  switch (option_name) {
    case SO_CERT_INDEX: {
      // Check if the provided certificate index is within a valid range
      SET_ERRNO_AND_RETURN_IF_TRUE(
        ((*(uint8_t *)option_value < SI91X_CERT_INDEX_0) || (*(uint8_t *)option_value > SI91X_CERT_INDEX_2)),
        EINVAL);

      // Set the certificate index
      si91x_socket->certificate_index = *(uint8_t *)option_value;
      break;
    }

    case SO_HIGH_PERFORMANCE_SOCKET: {
      // Check if the provided value is SI91X_HIGH_PERFORMANCE_SOCKET
      SET_ERRNO_AND_RETURN_IF_TRUE(*(uint8_t *)option_value != SI91X_HIGH_PERFORMANCE_SOCKET, EINVAL);

      // Set the SI91X_HIGH_PERFORMANCE_SOCKET flag in the ssl_bitmap of si91x_socket
      si91x_socket->ssl_bitmap |= SI91X_HIGH_PERFORMANCE_SOCKET;
      break;
    }

    case SO_TLS_SNI: {
      // Call a function to add a Server Name Indication (SNI) extension to si91x_socket
      sl_status_t status = add_server_name_indication_extension(&si91x_socket->sni_extensions,
                                                                (si91x_socket_type_length_value_t *)option_value);
      // Check if the operation was successful
      if (status != SL_STATUS_OK) {
        SET_ERROR_AND_RETURN(ENOMEM);
      }
      break;
    }

    default: {
      // If option_name is not recognized, return an error code
      SET_ERROR_AND_RETURN(ENOPROTOOPT);
    }
  }

  return SI91X_NO_ERROR;
}

int getsockopt(int socket_id, int option_level, int option_name, void *option_value, socklen_t *option_length)
{
  int16_t mss                  = 0;
  si91x_socket_t *si91x_socket = get_si91x_socket(socket_id);

  // Check if the socket is valid
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket == NULL, EBADF);
  // Check if option_value is valid
  SET_ERRNO_AND_RETURN_IF_TRUE(option_value == NULL, EFAULT);
  // Check if option_level is SOL_SOCKET
  SET_ERRNO_AND_RETURN_IF_TRUE((option_level != SOL_SOCKET && option_level != SOL_TCP) || option_length == NULL, EINVAL)

  // Determine the requested socket option and handle it accordingly
  switch (option_name) {
    case SO_SNDBUF: {
      mss            = sl_si91x_get_socket_mss(socket_id);
      *option_length = GET_SAFE_MEMCPY_LENGTH(*option_length, sizeof(mss));
      memcpy(option_value, &mss, *option_length);
      break;
    }

    case SO_RCVTIMEO: {
      // Get receive timeout
      *option_length = GET_SAFE_MEMCPY_LENGTH(*option_length, sizeof(si91x_socket->read_timeout));
      memcpy(option_value, &si91x_socket->read_timeout, *option_length);
      break;
    }

    case SO_KEEPALIVE: {
      // Get TCP keep-alive initial time
      *option_length = GET_SAFE_MEMCPY_LENGTH(*option_length, sizeof(si91x_socket->tcp_keepalive_initial_time));
      memcpy(option_value, &si91x_socket->tcp_keepalive_initial_time, *option_length);
      break;
    }

    case SO_TYPE: {
      // Retrieve and copy the socket type
      *option_length = GET_SAFE_MEMCPY_LENGTH(*option_length, sizeof(si91x_socket->type));
      memcpy(option_value, &si91x_socket->type, *option_length);
      break;
    }

    case SO_DOMAIN: {
      // Retrieve and copy the socket domain
      *option_length = GET_SAFE_MEMCPY_LENGTH(*option_length, sizeof(si91x_socket->local_address.sin6_family));
      memcpy(option_value, &si91x_socket->local_address.sin6_family, *option_length);
      break;
    }

    case SO_PROTOCOL: {
      // Retrieve and copy the socket protocol
      *option_length = GET_SAFE_MEMCPY_LENGTH(*option_length, sizeof(si91x_socket->protocol));
      memcpy(option_value, &si91x_socket->protocol, *option_length);
      break;
    }

    case SO_ERROR: {
      // Retrieve and copy the socket error (errno) and reset errno to 0
      *option_length = GET_SAFE_MEMCPY_LENGTH(*option_length, sizeof(errno));
      memcpy(option_value, &errno, *option_length);
      errno = 0;
      break;
    }

    case TCP_ULP: {
      // Retrieve and copy the socket ssl_bitmap
      *option_length = GET_SAFE_MEMCPY_LENGTH(*option_length, sizeof(si91x_socket->ssl_bitmap));
      memcpy(option_value, &si91x_socket->ssl_bitmap, *option_length);
      break;
    }

    default: {
      SET_ERROR_AND_RETURN(ENOPROTOOPT);
    }
  }

  return SI91X_NO_ERROR;
}

int sl_si91x_get_custom_sync_sockopt(int socket_id,
                                     int option_level,
                                     int option_name,
                                     void *option_value,
                                     socklen_t *option_length)
{
  si91x_socket_t *si91x_socket = get_si91x_socket(socket_id);

  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket == NULL, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(option_value == NULL, EFAULT);
  SET_ERRNO_AND_RETURN_IF_TRUE(option_level != SOL_SOCKET || option_length == NULL, EINVAL)

  switch (option_name) {
    case SO_CERT_INDEX: {
      // Retrieve and copy the socket certificate index
      *option_length = GET_SAFE_MEMCPY_LENGTH(*option_length, sizeof(si91x_socket->certificate_index));
      memcpy(option_value, &si91x_socket->certificate_index, *option_length);
      break;
    }

    default: {
      // Unsupported option
      SET_ERROR_AND_RETURN(ENOPROTOOPT);
    }
  }

  return SI91X_NO_ERROR;
}

int close(int socket_id)
{
  errno = 0; // Clear any existing error
  return sli_si91x_shutdown(socket_id, SHUTDOWN_BY_ID);
}

struct hostent *gethostbyname(const char *name)
{
  sl_status_t status = SL_STATUS_OK;

  if ((name == NULL)
      || (sl_strnlen((char *)(name), SI91X_DNS_REQUEST_MAX_URL_LEN + 1) > SI91X_DNS_REQUEST_MAX_URL_LEN)) {
    herrno = TRY_AGAIN;
    return NULL;
  }
// Retrieve host information based on address type
#ifdef SLI_SI91X_ENABLE_IPV6
  status = sl_net_host_get_by_name(name, SL_SI91X_WAIT_FOR_DNS_RESOLUTION, SL_NET_DNS_TYPE_IPV6, &host_ip_address);
#else
  status = sl_net_host_get_by_name(name, SL_SI91X_WAIT_FOR_DNS_RESOLUTION, SL_NET_DNS_TYPE_IPV4, &host_ip_address);
#endif

  // Handle the DNS resolution result
  if (status == SL_STATUS_TIMEOUT) {
    herrno = TRY_AGAIN;
    return NULL;
  } else if (status != SL_STATUS_OK) {
    herrno = HOST_NOT_FOUND;
    return NULL;
  }

// Set host information
#ifdef SLI_SI91X_ENABLE_IPV6
  host_ent.h_length   = (host_ip_address.type == SL_IPV4 ? SL_IPV4_ADDRESS_LENGTH : SL_IPV6_ADDRESS_LENGTH);
  host_ent.h_addrtype = (host_ip_address.type == SL_IPV4 ? AF_INET : AF_INET6);
#else
  host_ent.h_length   = SL_IPV4_ADDRESS_LENGTH;
  host_ent.h_addrtype = AF_INET;
#endif

  host_ent.h_name = (char *)name;
  host_ent.h_addr = (char *)&host_ip_address;

  herrno = NETDB_SUCCESS;
  return &host_ent;
}

struct hostent *gethostbyname2(const char *name, int af)
{
  sl_status_t status = SL_STATUS_OK;

  // Validate the 'name' argument
  if ((name == NULL)
      || (sl_strnlen((char *)(name), SI91X_DNS_REQUEST_MAX_URL_LEN + 1) > SI91X_DNS_REQUEST_MAX_URL_LEN)) {
    herrno = TRY_AGAIN;
    return NULL;
  }

  // Retrieve host information based on address type
  if (af == AF_INET6) {
    status = sl_net_host_get_by_name(name, SL_SI91X_WAIT_FOR_DNS_RESOLUTION, SL_NET_DNS_TYPE_IPV6, &host_ip_address);
  } else {
    status = sl_net_host_get_by_name(name, SL_SI91X_WAIT_FOR_DNS_RESOLUTION, SL_NET_DNS_TYPE_IPV4, &host_ip_address);
  }

  // Handle the DNS resolution result
  if (status == SL_STATUS_TIMEOUT) {
    herrno = TRY_AGAIN;
    return NULL;
  } else if (status != SL_STATUS_OK) {
    herrno = HOST_NOT_FOUND;
    return NULL;
  }

  // Set host information based on the address family
  if (af == AF_INET6) {
    host_ent.h_length   = SL_IPV6_ADDRESS_LENGTH;
    host_ent.h_addrtype = AF_INET6;
  } else {
    host_ent.h_length   = SL_IPV4_ADDRESS_LENGTH;
    host_ent.h_addrtype = AF_INET;
  }

  host_ent.h_name = (char *)name;
  host_ent.h_addr = (char *)&host_ip_address;

  herrno = NETDB_SUCCESS; // Set success status
  return &host_ent;
}

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
  sl_status_t status = SL_STATUS_OK;

  int32_t total_fd_set_count        = 0; // Specifies total number of file descriptors.
  int32_t select_response_wait_time = 0;

  sl_si91x_socket_select_req_t request   = { 0 };
  sl_si91x_socket_select_rsp_t *response = NULL;
  sl_wifi_buffer_t *buffer               = NULL;
  sl_si91x_packet_t *packet              = NULL;
  sl_si91x_wait_period_t wait_time       = 0;

  // Validate input parameters
  // exceptfds are not being checked as firmware doesn't support it.
  if ((readfds == NULL) && (writefds == NULL)) {
    // No file descriptor sets provided
    SET_ERROR_AND_RETURN(EINVAL);
  }
  if (nfds < 0 || nfds > NUMBER_OF_BSD_SOCKETS) {
    // Invalid number of file descriptors
    SET_ERROR_AND_RETURN(EINVAL);
  }
  if ((timeout != NULL) && ((timeout->tv_sec < 0) || (timeout->tv_usec < 0))) {
    // Invalid timeout values
    SET_ERROR_AND_RETURN(EINVAL);
  }

  // Iterate through the file descriptor sets and populate the request
  for (uint8_t host_socket_index = 0; host_socket_index < nfds; host_socket_index++) {
    si91x_socket_t *socket = get_si91x_socket(host_socket_index);

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

    if (readfds != NULL && FD_ISSET(host_socket_index, readfds)) {
      // Sets the corresponding bit for read in the request
      request.read_fds.fd_array[0] |= (1U << socket->id);
    }

    if (writefds != NULL && FD_ISSET(host_socket_index, writefds)) {
      // Sets the corresponding bit for write in the request
      request.write_fds.fd_array[0] |= (1U << socket->id);
    }

    // Update 'num_fd' to the maximum file descriptor ID encountered
    if (request.num_fd <= socket->id) {
      request.num_fd = socket->id + 1;
    }
  }

  // Calculate the select timeout based on the provided timeout values
  if (timeout != NULL && ((timeout->tv_sec != 0) || (timeout->tv_usec != 0))) {
    request.select_timeout.tv_sec  = timeout->tv_sec;
    request.select_timeout.tv_usec = timeout->tv_usec;
    select_response_wait_time      = ((request.select_timeout.tv_sec * 1000) + (request.select_timeout.tv_usec / 1000)
                                 + SI91X_HOST_WAIT_FOR_SELECT_RSP);
    wait_time                      = SL_SI91X_WAIT_FOR_RESPONSE(select_response_wait_time);
  } else {
    // No timeout specified
    request.no_timeout        = 1;
    select_response_wait_time = 0;
    wait_time                 = (SL_SI91X_WAIT_FOR_EVER | SL_SI91X_WAIT_FOR_RESPONSE_BIT);
  }

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_SELECT_REQUEST,
                                        SI91X_SOCKET_CMD_QUEUE,
                                        &request,
                                        sizeof(sl_si91x_socket_select_req_t),
                                        wait_time,
                                        NULL,
                                        &buffer);
  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sl_si91x_host_free_buffer(buffer);
  }
  SOCKET_VERIFY_STATUS_AND_RETURN(status, SL_STATUS_OK, SI91X_UNDEFINED_ERROR);

  // Retrieve the response packet and data
  packet   = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  response = (sl_si91x_socket_select_rsp_t *)packet->data;

  // Update the file descriptor sets
  total_fd_set_count = handle_select_response(response, readfds, writefds, exceptfds);

  sl_si91x_host_free_buffer(buffer);

  return total_fd_set_count;
}
