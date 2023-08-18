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
#define TCP_HEADER_LENGTH     56
#define UDP_HEADER_LENGTH     44
#define SI91X_SSL_HEADER_SIZE 90

static int sli_si91x_accept_async(int socket,
                                  const struct sockaddr *addr,
                                  socklen_t addr_len,
                                  accept_callback callback);
static int sli_si91x_socket(int family, int type, int protocol, receive_data_callback callback);

int sl_si91x_socket(int family, int type, int protocol)
{
  return sli_si91x_socket(family, type, protocol, NULL);
}

int sl_si91x_socket_async(int family, int type, int protocol, receive_data_callback callback)
{
  SET_ERRNO_AND_RETURN_IF_TRUE(NULL == callback, 0);
  return sli_si91x_socket(family, type, protocol, callback);
}

int sli_si91x_socket(int family, int type, int protocol, receive_data_callback callback)
{
  SET_ERRNO_AND_RETURN_IF_TRUE(family != AF_INET && family != AF_INET6, EAFNOSUPPORT);
  SET_ERRNO_AND_RETURN_IF_TRUE(type != SOCK_STREAM && type != SOCK_DGRAM, EINVAL);
  SET_ERRNO_AND_RETURN_IF_TRUE(protocol != IPPROTO_TCP && protocol != IPPROTO_UDP && protocol != 0, EINVAL);
  SET_ERRNO_AND_RETURN_IF_TRUE((type == SOCK_STREAM && (protocol != IPPROTO_TCP && protocol != 0)), EPROTOTYPE);
  SET_ERRNO_AND_RETURN_IF_TRUE((type == SOCK_DGRAM && (protocol != IPPROTO_UDP && protocol != 0)), EPROTOTYPE);

  si91x_socket_t *si91x_socket;
  int socket_index = -1;

  get_free_socket(&si91x_socket, &socket_index);

  if (socket_index < 0) {
    SET_ERROR_AND_RETURN(ENOMEM);
  }

  si91x_socket->type                      = type;
  si91x_socket->local_address.sin6_family = family;
  si91x_socket->protocol                  = protocol;
  si91x_socket->state                     = INITIALIZED;
  si91x_socket->recv_data_callback        = callback;

  return socket_index;
}

int sl_si91x_bind(int socket, const struct sockaddr *addr, socklen_t addr_len)
{

  si91x_socket_t *si91x_socket       = get_si91x_socket(socket);
  struct sockaddr_in *socket_address = (struct sockaddr_in *)addr;

  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket == NULL || si91x_socket->state != INITIALIZED, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(
    (si91x_socket->local_address.sin6_family == AF_INET && addr_len < sizeof(struct sockaddr_in))
      || (si91x_socket->local_address.sin6_family == AF_INET6 && addr_len < sizeof(struct sockaddr_in6)),
    EINVAL);

  SET_ERRNO_AND_RETURN_IF_TRUE(addr == NULL, EFAULT);

  if (!is_port_available(socket_address->sin_port)) {
    SET_ERROR_AND_RETURN(EADDRINUSE);
  }

  memcpy(&si91x_socket->local_address,
         addr,
         (addr_len > sizeof(struct sockaddr_in6)) ? sizeof(struct sockaddr_in6) : addr_len);

  si91x_socket->state = BOUND;

  if (si91x_socket->recv_data_callback != NULL && si91x_socket->type == SOCK_DGRAM) {
    int si91x_status = create_and_send_socket_request(socket, SI91X_SOCKET_LUDP, NULL);
    SOCKET_VERIFY_STATUS_AND_RETURN(si91x_status, SI91X_NO_ERROR, SI91X_UNDEFINED_ERROR);

    si91x_socket->state = UDP_UNCONNECTED_READY;
  }

  return SI91X_NO_ERROR;
}

int sl_si91x_connect(int socket, const struct sockaddr *addr, socklen_t addr_len)
{

  sl_status_t status           = SL_STATUS_FAIL;
  si91x_socket_t *si91x_socket = get_si91x_socket(socket);

  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket == NULL, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket->type == SOCK_STREAM && si91x_socket->state == CONNECTED, EISCONN);

  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket->type == SOCK_STREAM && si91x_socket->state > BOUND, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket->type == SOCK_DGRAM && si91x_socket->state != INITIALIZED
                                 && si91x_socket->state != BOUND && si91x_socket->state != UDP_UNCONNECTED_READY,
                               EBADF);

  SET_ERRNO_AND_RETURN_IF_TRUE(
    (si91x_socket->local_address.sin6_family == AF_INET && addr_len < sizeof(struct sockaddr_in))
      || (si91x_socket->local_address.sin6_family == AF_INET6 && addr_len < sizeof(struct sockaddr_in6)),
    EINVAL);

  SET_ERRNO_AND_RETURN_IF_TRUE(addr == NULL, EFAULT);

  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket->local_address.sin6_family != addr->sa_family, EAFNOSUPPORT)

  memcpy(&si91x_socket->remote_address,
         addr,
         (addr_len > sizeof(struct sockaddr_in6)) ? sizeof(struct sockaddr_in6) : addr_len);

  // Since socket is already created, there is no need to send create request again.
  if (si91x_socket->type == SOCK_DGRAM && si91x_socket->state == UDP_UNCONNECTED_READY) {
    si91x_socket->state = CONNECTED;

    return SI91X_NO_ERROR;
  }

  if (si91x_socket->type == SOCK_STREAM) {
    status = create_and_send_socket_request(socket, SI91X_SOCKET_TCP_CLIENT, NULL);
  } else if (si91x_socket->type == SOCK_DGRAM) {
    status = create_and_send_socket_request(socket, SI91X_SOCKET_LUDP, NULL);
  }

  SOCKET_VERIFY_STATUS_AND_RETURN(status, SL_STATUS_OK, SI91X_UNDEFINED_ERROR);

  si91x_socket->state = CONNECTED;
  return SI91X_NO_ERROR;
}

int sl_si91x_listen(int socket, int max_number_of_clients)
{
  int32_t status;
  si91x_socket_t *si91x_socket = get_si91x_socket(socket);

  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket == NULL, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket->state != INITIALIZED && si91x_socket->state != BOUND, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket->type != SOCK_STREAM, EOPNOTSUPP);

  status = create_and_send_socket_request(socket, SI91X_SOCKET_TCP_SERVER, (int *)&max_number_of_clients);
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

  si91x_socket_t *si91x_server_socket = get_si91x_socket(socket);
  si91x_socket_t *si91x_client_socket = NULL;

  sl_si91x_socket_context_t *context = NULL;
  int *client_socket                 = NULL;

  sl_si91x_socket_accept_request_t accept_request = { 0 };
  sl_si91x_rsp_ltcp_est_t *ltcp                   = NULL;

  sl_wifi_buffer_t *buffer         = NULL;
  sl_si91x_packet_t *packet        = NULL;
  sl_si91x_wait_period_t wait_time = 0;

  sl_status_t status       = SL_STATUS_OK;
  int32_t client_socket_id = -1;

  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_server_socket == NULL, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_server_socket->type != SOCK_STREAM, EOPNOTSUPP);
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_server_socket->state != LISTEN, EINVAL);

  // Create a new instance for socket
  client_socket_id = sli_si91x_socket(si91x_server_socket->local_address.sin6_family,
                                      si91x_server_socket->type,
                                      si91x_server_socket->protocol,
                                      si91x_server_socket->recv_data_callback);

  SET_ERRNO_AND_RETURN_IF_TRUE(client_socket_id < 0, SI91X_UNDEFINED_ERROR);

  si91x_client_socket = get_si91x_socket(client_socket_id);
  memcpy(&si91x_client_socket->local_address, &si91x_server_socket->local_address, sizeof(struct sockaddr_in6));

  // Create accept request
  accept_request.socket_id   = si91x_server_socket->id;
  accept_request.source_port = si91x_server_socket->local_address.sin6_port;
  wait_time =
    (callback == NULL ? (SL_SI91X_WAIT_FOR_EVER | SL_SI91X_WAIT_FOR_RESPONSE_BIT) : SL_SI91X_RETURN_IMMEDIATELY);

  if (callback != NULL) {
    context       = calloc(sizeof(sl_si91x_socket_context_t), 1);
    client_socket = malloc(sizeof(int));

    if (context == NULL || client_socket == NULL) {
      SL_CLEANUP_MALLOC(context);
      SL_CLEANUP_MALLOC(client_socket);

      return -1;
    }

    *client_socket          = client_socket_id;
    context->user_context   = callback;
    context->socket_context = client_socket;
  }

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_SOCKET_ACCEPT,
                                        SI91X_SOCKET_CMD_QUEUE,
                                        &accept_request,
                                        sizeof(accept_request),
                                        wait_time,
                                        context,
                                        &buffer);

  if (callback != NULL && status == SL_STATUS_IN_PROGRESS) {
    return SL_STATUS_IN_PROGRESS;
  }

  if (status != SL_STATUS_OK) {
    SL_CLEANUP_MALLOC(client_socket);
    SL_CLEANUP_MALLOC(context);

    close(client_socket_id);
    SET_ERROR_AND_RETURN(SI91X_UNDEFINED_ERROR);
  }

  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  ltcp   = (sl_si91x_rsp_ltcp_est_t *)packet->data;

  handle_accept_response(client_socket_id, ltcp);
  sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);

  if (addr_len <= 0) {
    return client_socket_id;
  }

  memcpy((struct sockaddr *)&addr,
         &si91x_client_socket->remote_address,
         (addr_len > sizeof(struct sockaddr_in6)) ? sizeof(struct sockaddr_in6) : addr_len);

  addr_len = si91x_client_socket->local_address.sin6_family == AF_INET ? sizeof(struct sockaddr_in)
                                                                       : sizeof(struct sockaddr_in6);

  return client_socket_id;
}

int sl_si91x_setsockopt(int32_t sockID, int level, int option_name, const void *option_value, socklen_t option_len)
{
  UNUSED_PARAMETER(level);
  si91x_socket_t *si91x_socket = get_si91x_socket(sockID);
  sl_si91x_time_value *timeout = NULL;
  uint16_t timeout_val;

  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket == NULL, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(option_value == NULL, EFAULT)

  switch (option_name) {
    case sl_si91x_SO_RCVTIME: {
      // Configure receive timeout
      timeout = (sl_si91x_time_value *)option_value;
      if ((timeout->tv_sec == 0) && (timeout->tv_usec != 0) && (timeout->tv_usec < 1000)) {
        timeout->tv_usec = 1000;
      }
      timeout_val = (timeout->tv_usec / 1000) + (timeout->tv_sec * 1000);

      // Need to add check here if Synchronous bit map is set (after async socket_id implementation)
      memcpy(&si91x_socket->read_timeout,
             &timeout_val,
             GET_SAFE_MEMCPY_LENGTH(sizeof(si91x_socket->read_timeout), option_len));
      break;
    }

    case sl_si91x_SO_MAXRETRY: {
      memcpy(&si91x_socket->max_tcp_retries,
             (uint16_t *)option_value,
             GET_SAFE_MEMCPY_LENGTH(sizeof(si91x_socket->max_tcp_retries), option_len));
      break;
    }

    case sl_si91x_SO_TCP_KEEPALIVE: {
      memcpy(&si91x_socket->tcp_keepalive_initial_time,
             (uint16_t *)option_value,
             GET_SAFE_MEMCPY_LENGTH(sizeof(si91x_socket->tcp_keepalive_initial_time), option_len));
      break;
    }
    case sl_si91x_SO_HIGH_PERFORMANCE_SOCKET: {
      SET_ERRNO_AND_RETURN_IF_TRUE(*(uint8_t *)option_value != SI91X_HIGH_PERFORMANCE_SOCKET, EINVAL);
      si91x_socket->ssl_bitmap |= SI91X_HIGH_PERFORMANCE_SOCKET;
      break;
    }

    case sl_si91x_SO_SSL_ENABLE: {
      SET_ERRNO_AND_RETURN_IF_TRUE((*(uint8_t *)option_value) != SI91X_SOCKET_FEAT_SSL, EINVAL);
      si91x_socket->ssl_bitmap |= SI91X_SOCKET_FEAT_SSL;
      break;
    }
    case sl_si91x_SO_SSL_V_1_0_ENABLE: {
      SET_ERRNO_AND_RETURN_IF_TRUE(((*(uint8_t *)option_value) != (SI91X_SOCKET_FEAT_SSL | SL_SI91X_TLS_V_1_0)),
                                   EINVAL);
      si91x_socket->ssl_bitmap |= SI91X_SOCKET_FEAT_SSL | SL_SI91X_TLS_V_1_0;
      break;
    }
    case sl_si91x_SO_SSL_V_1_1_ENABLE: {
      SET_ERRNO_AND_RETURN_IF_TRUE(((*(uint8_t *)option_value) != (SI91X_SOCKET_FEAT_SSL | SL_SI91X_TLS_V_1_1)),
                                   EINVAL);
      si91x_socket->ssl_bitmap |= SI91X_SOCKET_FEAT_SSL | SL_SI91X_TLS_V_1_1;
      break;
    }
    case sl_si91x_SO_SSL_V_1_2_ENABLE: {
      SET_ERRNO_AND_RETURN_IF_TRUE(((*(uint8_t *)option_value) != (SI91X_SOCKET_FEAT_SSL | SL_SI91X_TLS_V_1_2)),
                                   EINVAL);
      si91x_socket->ssl_bitmap |= SI91X_SOCKET_FEAT_SSL | SL_SI91X_TLS_V_1_2;
      break;
    }

#ifdef CHIP_917
    case sl_si91x_SO_SSL_V_1_3_ENABLE: {
      SET_ERRNO_AND_RETURN_IF_TRUE(((*(uint8_t *)option_value) != (SI91X_SOCKET_FEAT_SSL | SL_SI91X_TLS_V_1_3)),
                                   EINVAL);
      si91x_socket->ssl_bitmap |= SI91X_SOCKET_FEAT_SSL | SL_SI91X_TLS_V_1_3;
      break;
    }
#endif

    //  case si91x_SO_TCP_ACK_INDICATION:{
    //     SET_ERRNO_AND_RETURN_IF_TRUE(*(uint8_t *)option_value !=SI91X_SOCKET_FEAT_TCP_ACK_INDICATION,EINVAL );
    //     si91x_socket->ssl_bitmap |= SI91X_SOCKET_FEAT_TCP_ACK_INDICATION;
    //     break;
    //  }
    case sl_si91x_SO_CERT_INDEX: {
      SET_ERRNO_AND_RETURN_IF_TRUE(
        ((*(uint8_t *)option_value < SI91X_CERT_INDEX_0) || (*(uint8_t *)option_value > SI91X_CERT_INDEX_2)),
        EINVAL);

      si91x_socket->certificate_index = *(uint8_t *)option_value;
      break;
    }

#ifdef CHIP_917
    case sl_si91x_SO_MAX_RETRANSMISSION_TIMEOUT_VALUE: {
      if (IS_POWER_OF_TWO((*(uint8_t *)option_value)) && ((*(uint8_t *)option_value) < MAX_RETRANSMISSION_TIME_VALUE)) {
        memcpy(&si91x_socket->max_retransmission_timeout_value,
               (uint8_t *)option_value,
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
  return sl_si91x_sendto_async(socket, (uint8_t *)buffer, buffer_length, flags, NULL, 0, callback);
}

int sl_si91x_sendto(int socket,
                    uint8_t *buffer,
                    size_t buffer_length,
                    int32_t flags,
                    const struct sockaddr *addr,
                    socklen_t addr_len)
{
  return sl_si91x_sendto_async(socket, buffer, buffer_length, flags, addr, addr_len, NULL);
}

int sl_si91x_sendto_async(int socket,
                          uint8_t *buffer,
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

  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket == NULL, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket->type == SOCK_STREAM && si91x_socket->state != CONNECTED, ENOTCONN);
  SET_ERRNO_AND_RETURN_IF_TRUE(buffer == NULL, EFAULT);

  si91x_socket->data_transfer_callback = callback;

  if (si91x_socket->type == SOCK_STREAM) {
    if (si91x_socket->ssl_bitmap & SL_SI91X_ENABLE_TLS) {
      SET_ERRNO_AND_RETURN_IF_TRUE((uint16_t)buffer_length > (si91x_socket->mss - SI91X_SSL_HEADER_SIZE), EMSGSIZE);
    } else {
      SET_ERRNO_AND_RETURN_IF_TRUE(buffer_length > si91x_socket->mss, EMSGSIZE);
    }
  } else if (si91x_socket->type == SOCK_DGRAM) {
    SET_ERRNO_AND_RETURN_IF_TRUE(buffer_length > DEFAULT_DATAGRAM_MSS_SIZE, EMSGSIZE);
  }

  if (si91x_socket->type == SOCK_DGRAM && (si91x_socket->state == BOUND || si91x_socket->state == INITIALIZED)) {
    sl_status_t status = create_and_send_socket_request(socket, SI91X_SOCKET_LUDP, NULL);

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
    EINVAL)

  // create a socket send request
  if (si91x_socket->local_address.sin6_family == AF_INET6) {
    struct sockaddr_in6 *socket_address = (struct sockaddr_in6 *)to_addr;
    request.ip_version                  = SL_IPV6_ADDRESS_LENGTH;
    uint8_t *destination_ip =
      (si91x_socket->state == UDP_UNCONNECTED_READY || to_addr_len >= sizeof(struct sockaddr_in6))
        ? socket_address->sin6_addr.__u6_addr.__u6_addr8
        : si91x_socket->remote_address.sin6_addr.__u6_addr.__u6_addr8;

    memcpy(request.dest_ip_addr.ipv6_address, destination_ip, SL_IPV6_ADDRESS_LENGTH);
  } else {
    struct sockaddr_in *socket_address = (struct sockaddr_in *)to_addr;
    request.ip_version                 = SL_IPV4_ADDRESS_LENGTH;
    uint32_t *destination_ip =
      (si91x_socket->state == UDP_UNCONNECTED_READY || to_addr_len >= sizeof(struct sockaddr_in))
        ? &socket_address->sin_addr.s_addr
        : &((struct sockaddr_in *)&si91x_socket->remote_address)->sin_addr.s_addr;

    memcpy(request.dest_ip_addr.ipv4_address, destination_ip, SL_IPV4_ADDRESS_LENGTH);
  }

  request.socket_id   = si91x_socket->id;
  request.dest_port   = ((si91x_socket->state == UDP_UNCONNECTED_READY || to_addr_len > 0))
                          ? ((struct sockaddr_in *)to_addr)->sin_port
                          : si91x_socket->remote_address.sin6_port;
  request.data_offset = (si91x_socket->type == SOCK_STREAM) ? TCP_HEADER_LENGTH : UDP_HEADER_LENGTH;
  request.length      = buffer_length;

  status = sl_si91x_driver_send_socket_data(&request, buffer, 0);
  SOCKET_VERIFY_STATUS_AND_RETURN(status, SL_STATUS_OK, ENOBUFS);

  return buffer_length;
}

int sl_si91x_recv(int socket, uint8_t *buf, size_t buf_len, int32_t flags)
{
  return recvfrom(socket, buf, buf_len, flags, NULL, NULL);
}

int sl_si91x_recvfrom(int socket,
                      uint8_t *buf,
                      size_t buf_len,
                      int32_t flags,
                      struct sockaddr *addr,
                      socklen_t *addr_len)
{
  UNUSED_PARAMETER(flags);
  sl_si91x_wait_period_t wait_time   = 0;
  sl_si91x_req_socket_read_t request = { 0 };
  uint32_t event                     = NCP_HOST_SOCKET_RESPONSE_EVENT;
  ssize_t bytes_read                 = 0;
  si91x_rsp_socket_recv_t *response  = NULL;
  si91x_socket_t *si91x_socket       = get_si91x_socket(socket);
  sl_wifi_buffer_t *buffer;

  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket == NULL, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket->type == SOCK_STREAM && si91x_socket->state != CONNECTED, ENOTCONN);
  SET_ERRNO_AND_RETURN_IF_TRUE(buf == NULL, EFAULT);
  SET_ERRNO_AND_RETURN_IF_TRUE(buf_len <= 0, EINVAL)

  if (si91x_socket->type == SOCK_DGRAM && (si91x_socket->state == BOUND || si91x_socket->state == INITIALIZED)) {
    int bsd_status = create_and_send_socket_request(socket, SI91X_SOCKET_LUDP, NULL);
    SOCKET_VERIFY_STATUS_AND_RETURN(bsd_status, SI91X_NO_ERROR, SI91X_UNDEFINED_ERROR);

    si91x_socket->state = UDP_UNCONNECTED_READY;
  }

  // Possible states are only reset and disconnected.
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket->state != CONNECTED && si91x_socket->state != UDP_UNCONNECTED_READY, EBADF)

  if (si91x_socket->type == SOCK_STREAM && buf_len > DEFAULT_STREAM_MSS_SIZE) {
    buf_len = DEFAULT_STREAM_MSS_SIZE;
  } else if (si91x_socket->type == SOCK_DGRAM && buf_len > DEFAULT_DATAGRAM_MSS_SIZE) {
    buf_len = DEFAULT_DATAGRAM_MSS_SIZE;
  }

  request.socket_id = si91x_socket->id;
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
                                                           &wait_time);

  SOCKET_VERIFY_STATUS_AND_RETURN(status, SL_STATUS_OK, SI91X_UNDEFINED_ERROR)

  bytes_read = (response->length <= buf_len) ? response->length : buf_len;
  memcpy(buf, ((uint8_t *)response + response->offset), bytes_read);

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

  sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);

  return bytes_read;
}

int sl_si91x_select(int nfds,
                    fd_set *readfds,
                    fd_set *writefds,
                    fd_set *exceptfds,
                    struct timeval *timeout,
                    select_callback callback)
{
  sl_status_t status = SL_STATUS_OK;

  sl_si91x_socket_select_req_t request = { 0 };
  if ((readfds == NULL) && (writefds == NULL) && (exceptfds == NULL)) {
    SET_ERROR_AND_RETURN(EINVAL);
  }
  if (nfds < 0 || nfds > NUMBER_OF_BSD_SOCKETS) {
    SET_ERROR_AND_RETURN(EINVAL);
  }
  if ((timeout != NULL) && ((timeout->tv_sec < 0) || (timeout->tv_usec < 0))) {
    SET_ERROR_AND_RETURN(EINVAL);
  }

  set_select_callback(callback);

  for (uint8_t host_socket_index = 0; host_socket_index < nfds; host_socket_index++) {
    if (readfds != NULL) {

      if (FD_ISSET(host_socket_index, readfds)) {
        si91x_socket_t *socket = get_si91x_socket(host_socket_index);
        request.read_fds.fd_array[0] |= (1U << socket->id);
      }
    }

    if (writefds != NULL) {

      if (FD_ISSET(host_socket_index, writefds)) {
        si91x_socket_t *socket = get_si91x_socket(host_socket_index);
        request.write_fds.fd_array[0] |= (1U << socket->id);
      }
    }

    if (request.num_fd <= get_si91x_socket(host_socket_index)->id) {
      request.num_fd = get_si91x_socket(host_socket_index)->id + 1;
    }
  }

  if (timeout != NULL && ((timeout->tv_sec != 0) || (timeout->tv_usec != 0))) {
    request.select_timeout.tv_sec  = timeout->tv_sec;
    request.select_timeout.tv_usec = timeout->tv_usec;
  } else {
    request.no_timeout = 1;
  }

  status =
    sl_si91x_driver_send_asycn_command(RSI_WLAN_REQ_SELECT_REQUEST, SI91X_SOCKET_CMD_QUEUE, &request, sizeof(request));

  SOCKET_VERIFY_STATUS_AND_RETURN(status, SL_STATUS_OK, SI91X_UNDEFINED_ERROR);
  return SI91X_NO_ERROR;
}
