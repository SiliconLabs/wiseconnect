#include "sl_si91x_socket_utility.h"
#include "sl_si91x_socket_types.h"
#include "sl_si91x_socket_callback_framework.h"
#include "sl_status.h"
#include "sl_constants.h"
#include "sl_si91x_driver.h"
#include "sl_si91x_protocol_types.h"
#include "sl_si91x_socket_constants.h"
#include "sl_si91x_host_interface.h"
#include "sl_rsi_utility.h"
#include <string.h>

/******************************************************
 *               Static Function Declarations
 ******************************************************/
static sl_status_t si91x_config_socket(void);

/******************************************************
 *               Variable Definitions
 ******************************************************/
static sl_si91x_socket_config_t socket_config = {
  TOTAL_SOCKETS,                   // Total sockets
  TOTAL_TCP_SOCKETS,               // Total TCP sockets
  TOTAL_UDP_SOCKETS,               // Total UDP sockets
  TCP_TX_ONLY_SOCKETS,             // TCP TX only sockets
  TCP_RX_ONLY_SOCKETS,             // TCP RX only sockets
  UDP_TX_ONLY_SOCKETS,             // UDP TX only sockets
  UDP_RX_ONLY_SOCKETS,             // UDP RX only sockets
  TCP_RX_HIGH_PERFORMANCE_SOCKETS, // TCP RX high performance sockets^M
  TCP_RX_WINDOW_SIZE,              // TCP RX window size^M
  TCP_RX_WINDOW_DIVISION_FACTOR    // TCP RX window division factor
};

static si91x_socket_t sockets[NUMBER_OF_SOCKETS];

static select_callback user_select_callback = NULL;

static bool is_configured = false;

/******************************************************
 *               Function Definitions
 ******************************************************/

void handle_accept_response(int client_socket_id, sl_si91x_rsp_ltcp_est_t *accept_response)
{
  si91x_socket_t *si91x_client_socket = get_si91x_socket(client_socket_id);

  si91x_client_socket->id                         = accept_response->socket_id;
  si91x_client_socket->local_address.sin6_port    = accept_response->src_port_num;
  si91x_client_socket->remote_address.sin6_port   = accept_response->dest_port;
  si91x_client_socket->mss                        = accept_response->mss;
  si91x_client_socket->state                      = CONNECTED;
  si91x_client_socket->remote_address.sin6_family = accept_response->ip_version == SL_IPV6_ADDRESS_LENGTH ? AF_INET6
                                                                                                          : AF_INET;

  if (si91x_client_socket->remote_address.sin6_family == SL_IPV6_ADDRESS_LENGTH) {
    memcpy(si91x_client_socket->remote_address.sin6_addr.__u6_addr.__u6_addr8,
           accept_response->dest_ip_addr.ipv6_address,
           SL_IPV6_ADDRESS_LENGTH);

  } else {
    memcpy(&((struct sockaddr_in *)&si91x_client_socket->remote_address)->sin_addr,
           accept_response->dest_ip_addr.ipv4_address,
           SL_IPV4_ADDRESS_LENGTH);
  }
}

int handle_select_response(sl_si91x_socket_select_rsp_t *response,
                           fd_set *readfds,
                           fd_set *writefds,
                           fd_set *exception_fd)
{
  int total_fd_set_count = 0;

  FD_ZERO(readfds);
  FD_ZERO(writefds);
  FD_ZERO(exception_fd);

  for (int host_socket_index = 0; host_socket_index < NUMBER_OF_SOCKETS; host_socket_index++) {
    int firmware_socket_id = get_si91x_socket(host_socket_index)->id;

    if (readfds != NULL) {
      if (response->read_fds.fd_array[0] & (1 << firmware_socket_id)) {
        FD_SET(host_socket_index, readfds);
        total_fd_set_count++;
      }
    }

    if (writefds != NULL) {
      if (response->write_fds.fd_array[0] & (1 << firmware_socket_id)) {
        FD_SET(host_socket_index, writefds);
        total_fd_set_count++;
      }
    }
  }

  return total_fd_set_count;
}

void set_select_callback(select_callback callback)
{
  user_select_callback = callback;
}

sl_status_t sl_si91x_socket_init(void)
{
  return SL_STATUS_OK;
}

sl_status_t sl_si91x_socket_deinit(void)
{
  if (!is_configured) {
    return SL_STATUS_OK;
  }

  for (uint8_t socket_index = 0; socket_index < NUMBER_OF_BSD_SOCKETS; socket_index++) {
    reset_socket_state(socket_index);
  }

  is_configured = false;

  return SL_STATUS_OK;
}

sl_status_t si91x_config_socket(void)
{
  sl_status_t status = SL_STATUS_OK;
  status             = sl_si91x_driver_send_command(RSI_WLAN_REQ_SOCKET_CONFIG,
                                        SI91X_SOCKET_CMD_QUEUE,
                                        &socket_config,
                                        sizeof(socket_config),
                                        SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                        NULL,
                                        NULL);

  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

void reset_socket_state(int socket)
{
  memset(&sockets[socket], 0, sizeof(si91x_socket_t));
}

si91x_socket_t *get_si91x_socket(int socket)
{
  return (((socket >= 0) && (socket < NUMBER_OF_SOCKETS)) && sockets[socket].state != RESET) ? &sockets[socket] : NULL;
}

void get_free_socket(si91x_socket_t **socket, int *socket_fd)
{
  *socket    = NULL;
  *socket_fd = -1;

  if (!is_configured) {
    sl_si91x_socket_init();

    if (si91x_config_socket() != SL_STATUS_OK) {
      return;
    }

    is_configured = true;
  }

  for (uint8_t socket_index = 0; socket_index < NUMBER_OF_SOCKETS; socket_index++) {

    if (sockets[socket_index].state != RESET) {
      continue;
    }

    *socket    = &sockets[socket_index];
    *socket_fd = socket_index;
    return;
  }
}

bool is_port_available(uint16_t port_number)
{
  // Check whether local port is already used or not
  for (uint8_t socket_index = 0; socket_index < NUMBER_OF_SOCKETS; socket_index++) {
    if (sockets[socket_index].local_address.sin6_port == port_number) {
      return false;
    }
  }

  return true;
}

static uint16_t get_socket_id_from_socket_command(sl_si91x_packet_t *packet)
{
  switch (packet->command) {
    case RSI_WLAN_REQ_SOCKET_READ_DATA: {
      sl_si91x_req_socket_read_t *read_request = (sl_si91x_req_socket_read_t *)(packet->data);
      return read_request->socket_id;
    }
    default:
      break;
  }

  return 0xFF;
}

static void si91x_socket_node_free_function(sl_wifi_buffer_t *buffer)
{
  sl_si91x_host_free_buffer(buffer, SL_WIFI_CONTROL_BUFFER);
}

static bool si91x_socket_identification_function(sl_wifi_buffer_t *buffer, void *user_data)
{
  sl_status_t status;
  sl_si91x_packet_t *packet                               = NULL;
  sl_si91x_queue_packet_t *node                           = NULL;
  sl_si91x_queue_packet_t *response_node                  = NULL;
  sl_wifi_buffer_t *response_buffer                       = NULL;
  uint16_t socket_id                                      = 0xFF;
  sl_si91x_socket_close_response_t *remote_socket_closure = (sl_si91x_socket_close_response_t *)user_data;

  node   = (sl_si91x_queue_packet_t *)sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  packet = sl_si91x_host_get_buffer_data(node->host_packet, 0, NULL);

  socket_id = get_socket_id_from_socket_command(packet);

  if (socket_id == remote_socket_closure->socket_id) {
    /* Send response if asked  */
    if ((node->flags & SI91X_PACKET_RESPONSE_STATUS) == SI91X_PACKET_RESPONSE_STATUS) {
      status =
        sl_si91x_host_allocate_buffer(&response_buffer, SL_WIFI_CONTROL_BUFFER, sizeof(sl_si91x_queue_packet_t), 1000);
      if (status == SL_STATUS_OK) {
        response_node = sl_si91x_host_get_buffer_data(response_buffer, 0, NULL);

        memcpy(response_node, node, sizeof(sl_si91x_queue_packet_t));
        response_node->frame_status = ENOTCONN;
        response_node->host_packet  = NULL;
        response_node->flags        = 0;

        sl_si91x_host_add_to_queue(SI91X_SOCKET_RESPONSE_QUEUE, response_buffer);
        sl_si91x_host_set_event(NCP_HOST_SOCKET_RESPONSE_EVENT);
      }
    }
    return true;
  }
  return false;
}

// Prepare socket request based on socket type and send the request down to the driver.
// socket type : [SL_SOCKET_TCP_SERVER, SL_SOCKET_TCP_CLIENT, SL_SOCKET_LUDP, SL_SOCKET_UDP_CLIENT]
sl_status_t create_and_send_socket_request(int socketIdIndex, int type, int *backlog)
{
  sl_status_t status                                        = SL_STATUS_OK;
  sl_si91x_socket_create_request_t socket_create_request    = { 0 };
  sl_si91x_socket_create_response_t *socket_create_response = NULL;
  si91x_socket_t *si91x_bsd_socket                          = get_si91x_socket(socketIdIndex);
  sl_si91x_wait_period_t wait_period                        = SL_SI91X_WAIT_FOR_COMMAND_RESPONSE;

  if (type == SI91X_SOCKET_TCP_CLIENT) {
    //In wlan_throughput example the tick_count_s configured as 10, for that reason the connect was timed out.
    //To avoid timeout, need to configure wait_period as 100000
    wait_period = SL_SI91X_WAIT_FOR_RESPONSE(100000);
  }

  sl_wifi_buffer_t *buffer  = NULL;
  sl_si91x_packet_t *packet = NULL;

  if (si91x_bsd_socket->local_address.sin6_family == AF_INET6) {
    socket_create_request.ip_version = SL_IPV6_VERSION;

    memcpy(socket_create_request.dest_ip_addr.ipv6_address,
           si91x_bsd_socket->remote_address.sin6_addr.__u6_addr.__u6_addr8,
           SL_IPV6_ADDRESS_LENGTH);
  } else {
    socket_create_request.ip_version = SL_IPV4_ADDRESS_LENGTH;
    memcpy(socket_create_request.dest_ip_addr.ipv4_address,
           &((struct sockaddr_in *)&si91x_bsd_socket->remote_address)->sin_addr,
           SL_IPV4_ADDRESS_LENGTH);
  }

  socket_create_request.local_port  = si91x_bsd_socket->local_address.sin6_port;
  socket_create_request.remote_port = si91x_bsd_socket->remote_address.sin6_port;

  // Fill socket type
  socket_create_request.socket_type = type;

  if (type == SI91X_SOCKET_TCP_SERVER) {
    socket_create_request.max_count = (backlog == NULL) ? 0 : *backlog;
    socket_create_request.socket_bitmap |= SI91X_SOCKET_FEAT_LTCP_ACCEPT;
  } else {
    socket_create_request.max_count = 0;
  }

  if (si91x_bsd_socket->recv_data_callback == NULL) {
    socket_create_request.socket_bitmap |= SI91X_SOCKET_FEAT_SYNCHRONOUS;
  }

  socket_create_request.socket_bitmap |= SI91X_SOCKET_FEAT_TCP_RX_WINDOW;

  //  socket_create_request.socket_bitmap|=SI91X_SOCKET_FEAT_TCP_ACK_INDICATION;

  // Set the RX window size
  socket_create_request.rx_window_size = socket_config.tcp_rx_window_size_cap;

  // Fill VAP_ID
  socket_create_request.vap_id                     = 0;
  socket_create_request.tos                        = 0;
  socket_create_request.max_tcp_retries_count      = MAX_TCP_RETRY_COUNT;
  socket_create_request.tcp_keepalive_initial_time = DEFAULT_TCP_KEEP_ALIVE_TIME;

  // Check for SSL feature and fill it in SSL bitmap
  if (si91x_bsd_socket->ssl_bitmap & SL_SI91X_ENABLE_TLS) {
    socket_create_request.ssl_bitmap         = si91x_bsd_socket->ssl_bitmap;
    socket_create_request.ssl_ciphers_bitmap = SSL_ALL_CIPHERS;
#ifdef CHIP_917
    socket_create_request.ssl_ext_ciphers_bitmap = SSL_EXT_CIPHERS;
#endif
    // Check if cert index is not default index
    if (si91x_bsd_socket->certificate_index > SI91X_CERT_INDEX_0) {
      socket_create_request.socket_bitmap |= SI91X_SOCKET_FEAT_CERT_INDEX;
    }

    socket_create_request.socket_cert_inx = si91x_bsd_socket->certificate_index;

    wait_period = SL_SI91X_WAIT_FOR_RESPONSE(15000);
  }

  // Check for HIGH_PERFORMANCE feature bit
  if (si91x_bsd_socket->ssl_bitmap & SI91X_HIGH_PERFORMANCE_SOCKET) {
    socket_create_request.ssl_bitmap |= SI91X_HIGH_PERFORMANCE_SOCKET;
  }

#ifdef CHIP_917
  socket_create_request.max_retransmission_timeout_value = si91x_bsd_socket->max_retransmission_timeout_value;
  socket_create_request.tos                              = si91x_bsd_socket->tos;
#endif

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_SOCKET_CREATE,
                                        SI91X_SOCKET_CMD_QUEUE,
                                        &socket_create_request,
                                        sizeof(socket_create_request),
                                        wait_period,
                                        NULL,
                                        &buffer);

  packet                 = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  socket_create_response = (sl_si91x_socket_create_response_t *)packet->data;
  VERIFY_STATUS_AND_RETURN(status);

  si91x_bsd_socket->id = (socket_create_response->socket_id[0]) | (socket_create_response->socket_id[1] << 8);
  si91x_bsd_socket->local_address.sin6_port = socket_create_response->module_port[0]
                                              | socket_create_response->module_port[1] << 8;

  if (type != SI91X_SOCKET_TCP_SERVER) {
    si91x_bsd_socket->remote_address.sin6_port = socket_create_response->dst_port[0]
                                                 | socket_create_response->dst_port[1] << 8;
  }

  si91x_bsd_socket->mss = (socket_create_response->mss[0]) | (socket_create_response->mss[1] << 8);

  // If socket is already bound to an local address and port, there is no need to copy it again.
  if (si91x_bsd_socket->state == BOUND) {
    sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
    return SL_STATUS_OK;
  }

  if (si91x_bsd_socket->local_address.sin6_family == AF_INET) {
    memcpy(&((struct sockaddr_in *)&si91x_bsd_socket->local_address)->sin_addr.s_addr,
           socket_create_response->module_ip_addr.ipv4_addr,
           SL_IPV4_ADDRESS_LENGTH);
  } else {
    memcpy(si91x_bsd_socket->local_address.sin6_addr.__u6_addr.__u6_addr8,
           socket_create_response->module_ip_addr.ipv6_addr,
           SL_IPV6_ADDRESS_LENGTH);
  }

  sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);

  return SL_STATUS_OK;
}

int sli_si91x_shutdown(int socket, int how)
{
  sl_status_t status                                      = SL_STATUS_OK;
  sl_si91x_socket_close_request_t socket_close_request    = { 0 };
  sl_si91x_socket_close_response_t *socket_close_response = NULL;
  sl_si91x_wait_period_t wait_period                      = SL_SI91X_WAIT_FOR_RESPONSE(SL_SI91X_WAIT_FOR_EVER);
  sl_wifi_buffer_t *buffer                                = NULL;

  si91x_socket_t *si91x_socket = get_si91x_socket(socket);
  int close_request_type       = (si91x_socket->state == LISTEN) ? SHUTDOWN_BY_PORT : how;

  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket == NULL, EBADF);
  if (si91x_socket->state == BOUND || si91x_socket->state == INITIALIZED
      || (si91x_socket->state == DISCONNECTED && is_tcp_auto_close_enabled())) {
    reset_socket_state(socket);

    return SI91X_NO_ERROR;
  }

  /*If socket is server socket, SHUTDOWN_BY_PORT is to be used irrespective of 'how' parameter.*/
  socket_close_request.socket_id   = (close_request_type == SHUTDOWN_BY_ID) ? si91x_socket->id : 0;
  socket_close_request.port_number = (close_request_type == SHUTDOWN_BY_ID) ? 0 : si91x_socket->local_address.sin6_port;

  status = sl_si91x_socket_driver_send_command(RSI_WLAN_REQ_SOCKET_CLOSE,
                                               &socket_close_request,
                                               sizeof(socket_close_request),
                                               SI91X_SOCKET_CMD_QUEUE,
                                               SI91X_SOCKET_RESPONSE_QUEUE,
                                               &buffer,
                                               (void *)&socket_close_response,
                                               NULL,
                                               &wait_period);

  SOCKET_VERIFY_STATUS_AND_RETURN(status, SL_STATUS_OK, SI91X_UNDEFINED_ERROR);

  for (uint8_t index = 0; index < NUMBER_OF_SOCKETS; index++) {
    si91x_socket_t *socket = get_si91x_socket(index);

    if (close_request_type == SHUTDOWN_BY_ID && socket->id == socket_close_response->socket_id) {
      reset_socket_state(index);
    } else if (close_request_type == SHUTDOWN_BY_PORT
               && socket->local_address.sin6_port == socket_close_response->port_number) {
      reset_socket_state(index);
    }
  }

  return SI91X_NO_ERROR;
}

sl_status_t si91x_socket_event_handler(sl_status_t status,
                                       sl_si91x_socket_context_t *sdk_context,
                                       sl_si91x_packet_t *rx_packet)
{
  UNUSED_PARAMETER(status);

  if (rx_packet->command == RSI_WLAN_RSP_CONN_ESTABLISH) {
    sl_si91x_rsp_ltcp_est_t *accept_response = (sl_si91x_rsp_ltcp_est_t *)rx_packet->data;
    int *client_socket_id                    = (int *)sdk_context->socket_context;

    handle_accept_response(*client_socket_id, accept_response);

    accept_callback callback = (accept_callback)sdk_context->user_context;

    si91x_socket_t *client_socket = get_si91x_socket(*client_socket_id);

    callback(*client_socket_id, (struct sockaddr *)&client_socket->remote_address, client_socket->type);

    free(sdk_context->socket_context);

  } else if (rx_packet->command == RSI_WLAN_RSP_REMOTE_TERMINATE) {

    sl_si91x_socket_close_response_t *remote_socket_closure = (sl_si91x_socket_close_response_t *)rx_packet->data;

    for (uint8_t socket_index = 0; socket_index < NUMBER_OF_SOCKETS; socket_index++) {
      if (sockets[socket_index].id == remote_socket_closure->socket_id) {
        // The break statement is not issued as there is a case where one firmware socket might be mapped with multiple host ID.
        sockets[socket_index].state = DISCONNECTED;

        /* Flush the pending tx request packets from the socket command queue */
        sl_si91x_host_flush_nodes_from_queue(SI91X_SOCKET_CMD_QUEUE,
                                             remote_socket_closure,
                                             si91x_socket_identification_function,
                                             si91x_socket_node_free_function);
      }
    }
  } else if (rx_packet->command == RSI_RECEIVE_RAW_DATA) {
    si91x_rsp_socket_recv_t *firmware_socket_response = (si91x_rsp_socket_recv_t *)rx_packet->data;

    uint8_t host_socket = 0;

    for (uint8_t host_socket_index = 0; host_socket_index < NUMBER_OF_SOCKETS; host_socket_index++) {
      if (firmware_socket_response->socket_id == sockets[host_socket_index].id) {
        host_socket = host_socket_index;
      }
    }

    si91x_socket_t *client_socket = get_si91x_socket(host_socket);
    client_socket->recv_data_callback(host_socket,
                                      (uint8_t *)(firmware_socket_response + firmware_socket_response->offset),
                                      firmware_socket_response->length);
  } else if (rx_packet->command == RSI_WLAN_RSP_SELECT_REQUEST) {
    fd_set read_fd, write_fd, exception_fd;

    handle_select_response((sl_si91x_socket_select_rsp_t *)rx_packet->data, &read_fd, &write_fd, &exception_fd);

    user_select_callback(&read_fd, &write_fd, &exception_fd, status);
  } else if (rx_packet->command == RSI_WLAN_RSP_TCP_ACK_INDICATION) {
    sl_si91x_rsp_tcp_ack_t *tcp_ack = (sl_si91x_rsp_tcp_ack_t *)rx_packet->data;

    uint8_t host_socket = 0;

    for (uint8_t host_socket_index = 0; host_socket_index < NUMBER_OF_SOCKETS; host_socket_index++) {
      if (tcp_ack->socket_id == sockets[host_socket_index].id) {
        host_socket = host_socket_index;
        break;
      }
    }
    si91x_socket_t *si91x_socket = get_si91x_socket(host_socket);
    if (si91x_socket != NULL && si91x_socket->data_transfer_callback != NULL) {
      si91x_socket->data_transfer_callback(host_socket, (uint8_t)(tcp_ack->length[0] | tcp_ack->length[1] << 8));
    }
  }

  SL_CLEANUP_MALLOC(sdk_context);
  return SL_STATUS_OK;
}

sl_status_t sl_si91x_socket_driver_send_command(rsi_wlan_cmd_request_t command,
                                                const void *data,
                                                uint32_t data_length,
                                                sl_si91x_queue_type_t queue,
                                                sl_si91x_queue_type_t response_queue,
                                                sl_wifi_buffer_t **buffer,
                                                void **response,
                                                uint32_t *events_to_wait_for,
                                                sl_si91x_wait_period_t *wait_period)
{
  UNUSED_PARAMETER(response_queue);
  UNUSED_PARAMETER(events_to_wait_for);
  sl_status_t status;
  sl_si91x_packet_t *packet;

  if (response != NULL) {
    *response = NULL;
  }

  if (wait_period != NULL) {
    status = sl_si91x_driver_send_command(command, queue, data, data_length, *wait_period, NULL, buffer);
  } else {
    status = sl_si91x_driver_send_command(command, queue, data, data_length, SL_SI91X_RETURN_IMMEDIATELY, NULL, buffer);

    if (status != SL_STATUS_IN_PROGRESS) {
      return SL_STATUS_FAIL;
    }
  }

  if ((wait_period != NULL) && (status != SL_STATUS_OK || ((*wait_period) & (SL_SI91X_WAIT_FOR_RESPONSE_BIT == 0)))) {
    return status;
  }

  packet    = sl_si91x_host_get_buffer_data(*buffer, 0, NULL);
  *response = packet->data;
  return SL_STATUS_OK;
}
