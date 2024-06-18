/*******************************************************************************
* @file  sl_si91x_socket_utility.c
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
 *               Variable Definitions
 ******************************************************/
si91x_socket_t *sli_si91x_sockets[NUMBER_OF_SOCKETS];

static select_callback user_select_callback = NULL;

static accept_callback user_accept_callback = NULL;

static int32_t client_socket_accept_id = -1;

static remote_socket_termination_callback user_remote_socket_termination_callback = NULL;

static bool is_configured = false;

/******************************************************
 *               Function Definitions
 ******************************************************/

void handle_accept_response(int client_socket_id, sl_si91x_rsp_ltcp_est_t *accept_response)
{
  si91x_socket_t *si91x_client_socket = get_si91x_socket(client_socket_id);
  //Verifying socket existence
  if (si91x_client_socket == NULL)
    return;
  // Update socket parameters based on the accept response
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
  // To track of the total number of file descriptors set
  int total_fd_set_count = 0;

  // Clear file descriptor sets
  SLI_SI91X_NULL_SAFE_FD_ZERO(readfds);
  SLI_SI91X_NULL_SAFE_FD_ZERO(writefds);
  SLI_SI91X_NULL_SAFE_FD_ZERO(exception_fd);

  // Iterate through all host sockets
  for (int host_socket_index = 0; host_socket_index < NUMBER_OF_SOCKETS; host_socket_index++) {
    si91x_socket_t *socket = get_si91x_socket(host_socket_index);
    //Verifying socket existence
    if (socket == NULL) {
      continue;
    }

    // Check if the read file descriptor set is provided and if the corresponding bit is set in the response
    if (readfds != NULL && (response->read_fds.fd_array[0] & (1 << socket->id))) {
      FD_SET(host_socket_index, readfds);
      total_fd_set_count++;
    }

    // Check if the write file descriptor set is provided and if the corresponding bit is set in the response.
    if (writefds != NULL && (response->write_fds.fd_array[0] & (1 << socket->id))) {
      FD_SET(host_socket_index, writefds);
      total_fd_set_count++;
    }
  }

  return total_fd_set_count;
}

void set_select_callback(select_callback callback)
{
  user_select_callback = callback;
}

void sli_si91x_set_accept_callback(accept_callback callback, int32_t client_socket_id)
{
  // Set the user-defined accept callback function and the client socket ID
  user_accept_callback    = callback;
  client_socket_accept_id = client_socket_id;
}

void sli_si91x_set_remote_socket_termination_callback(remote_socket_termination_callback callback)
{
  user_remote_socket_termination_callback = callback;
}

sl_status_t sl_si91x_socket_init(void)
{
  return SL_STATUS_OK;
}

sl_status_t sl_si91x_vap_shutdown(uint8_t vap_id)
{
  if (!is_configured) {
    return SL_STATUS_OK;
  }

  // Iterate through all BSD sockets and reset those associated with the given VAP ID
  for (uint8_t socket_index = 0; socket_index < NUMBER_OF_BSD_SOCKETS; socket_index++) {
    if ((sli_si91x_sockets[socket_index] != NULL) && (sli_si91x_sockets[socket_index]->vap_id == vap_id)) {
      reset_socket_state(socket_index);
    }
  }

  is_configured = false;

  return SL_STATUS_OK;
}

sl_status_t sl_si91x_config_socket(sl_si91x_socket_config_t socket_config)
{
  sl_status_t status = SL_STATUS_OK;

  // Send the socket configuration command to the SI91X driver
  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_SOCKET_CONFIG,
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
  if (sli_si91x_sockets[socket] != NULL) {
    free(sli_si91x_sockets[socket]);
    sli_si91x_sockets[socket] = NULL;
  }
}

// Get the SI91X socket with the specified index, if it is valid and not in RESET state
si91x_socket_t *get_si91x_socket(int socket)
{
  return sli_si91x_sockets[socket];
}

// Find and return an available socket and its index
void get_free_socket(si91x_socket_t **socket, int *socket_fd)
{
  *socket    = NULL;
  *socket_fd = -1;

  // Check if the SI91X sockets have been configured
  if (!is_configured) {
    // If not configured, initialize the SI91X sockets
    sl_si91x_socket_init();
    // Set the configured flag to true to avoid reconfiguration
    is_configured = true;
  }

  // Iterate through all available sockets to find a free one
  for (uint8_t socket_index = 0; socket_index < NUMBER_OF_SOCKETS; socket_index++) {

    // If the socket is in use skip it
    if (sli_si91x_sockets[socket_index] != NULL) {
      continue;
    }

    // Allocate new socket
    sli_si91x_sockets[socket_index] = malloc(sizeof(si91x_socket_t));
    if (sli_si91x_sockets[socket_index] == NULL) {
      return;
    }
    memset(sli_si91x_sockets[socket_index], 0, sizeof(si91x_socket_t));
    sli_si91x_sockets[socket_index]->id = -1;

    // If a free socket is found, set the socket pointer to point to it
    *socket = sli_si91x_sockets[socket_index];
    // Set the socket_fd to the index of the free socket, which can be used as a file descriptor
    *socket_fd = socket_index;
    // Exit the loop because a free socket has been found.
    return;
  }
}

bool is_port_available(uint16_t port_number)
{
  // Check whether local port is already used or not
  for (uint8_t socket_index = 0; socket_index < NUMBER_OF_SOCKETS; socket_index++) {
    if (sli_si91x_sockets[socket_index] != NULL
        && sli_si91x_sockets[socket_index]->local_address.sin6_port == port_number) {
      return false;
    }
  }

  return true;
}

/**
 * @brief This function is responsible to copy the SNI information provided by application into socket structure.
 * 
 * @param socket_sni_extensions pointer to SNI extension in socket structure
 * @param sni_extension pointer to the SNI information provided by application
 * @return sl_status_t possible return values are SL_STATUS_OK and SL_STATUS_SI91X_MEMORY_ERROR
 */
sl_status_t add_server_name_indication_extension(si91x_server_name_indication_extensions_t *socket_sni_extensions,
                                                 const si91x_socket_type_length_value_t *sni_extension)
{
  // To check if memory available for new extension in SNI buffer of socket, max 256 Bytes only
  if (SI91X_MAX_SIZE_OF_EXTENSION_DATA - socket_sni_extensions->current_size_of_extensions
      < (int)(sizeof(si91x_socket_type_length_value_t) + sni_extension->length)) {
    return SL_STATUS_SI91X_MEMORY_ERROR;
  }

  uint8_t sni_size = (sizeof(si91x_socket_type_length_value_t) + sni_extension->length);

  // copies SNI provided by app into SDK socket struct
  memcpy(&socket_sni_extensions->buffer[socket_sni_extensions->current_size_of_extensions], sni_extension, sni_size);
  socket_sni_extensions->current_size_of_extensions += sni_size;
  socket_sni_extensions->total_extensions++;

  return SL_STATUS_OK;
}

static uint16_t get_socket_id_from_socket_command(sl_si91x_packet_t *packet)
{
  switch (packet->command) {
    case RSI_WLAN_REQ_SOCKET_READ_DATA: {
      sl_si91x_req_socket_read_t *read_request = (sl_si91x_req_socket_read_t *)(packet->data);
      return read_request->socket_id;
    }
    case RSI_WLAN_REQ_SOCKET_CLOSE: {
      sl_si91x_socket_close_request_t *socket_close_request = (sl_si91x_socket_close_request_t *)(packet->data);
      return (uint16_t)socket_close_request->socket_id;
    }
    case RSI_WLAN_REQ_SOCKET_ACCEPT: {
      sl_si91x_socket_accept_request_t *accept_request = (sl_si91x_socket_accept_request_t *)(packet->data);
      return (uint16_t)accept_request->socket_id;
    }
    default:
      break;
  }

  return 0xFF;
}

static void si91x_socket_node_free_function(sl_wifi_buffer_t *buffer)
{
  sl_si91x_host_free_buffer(buffer);
}

static uint8_t si91x_socket_identification_function(sl_wifi_buffer_t *buffer, void *user_data)
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
      } else {
        SL_DEBUG_LOG("\r\n HEAP EXHAUSTED DURING ALLOCATION \r\n");
        BREAKPOINT();
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
  sl_si91x_wait_period_t wait_period                        = SL_SI91X_WAIT_FOR_RESPONSE(5000);
  //Verifying socket existence
  if (si91x_bsd_socket == NULL) {
    return -1;
  }
  if (type == SI91X_SOCKET_TCP_CLIENT) {
    wait_period = SL_SI91X_WAIT_FOR_RESPONSE(100000); // timeout is 10 sec
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
  socket_create_request.rx_window_size = TCP_RX_WINDOW_SIZE;

  // Fill VAP_ID
  socket_create_request.vap_id = si91x_bsd_socket->vap_id;
  socket_create_request.tos    = 0;
  if (si91x_bsd_socket->max_tcp_retries) {
    socket_create_request.max_tcp_retries_count = si91x_bsd_socket->max_tcp_retries;
  } else {
    socket_create_request.max_tcp_retries_count = MAX_TCP_RETRY_COUNT;
  }
  if (si91x_bsd_socket->tcp_keepalive_initial_time) {
    socket_create_request.tcp_keepalive_initial_time = si91x_bsd_socket->tcp_keepalive_initial_time;
  } else {
    socket_create_request.tcp_keepalive_initial_time = DEFAULT_TCP_KEEP_ALIVE_TIME;
  }

  socket_create_request.tcp_mss = si91x_bsd_socket->mss;

  // Check for SSL feature and fill it in SSL bitmap
  if (si91x_bsd_socket->ssl_bitmap & SL_SI91X_ENABLE_TLS) {
    socket_create_request.ssl_bitmap         = si91x_bsd_socket->ssl_bitmap;
    socket_create_request.ssl_ciphers_bitmap = SSL_ALL_CIPHERS;
#ifdef SLI_SI917
    socket_create_request.ssl_ext_ciphers_bitmap = SSL_EXT_CIPHERS;
#endif
    // Check if cert index is not default index
    if (si91x_bsd_socket->certificate_index > SI91X_CERT_INDEX_0) {
      socket_create_request.socket_bitmap |= SI91X_SOCKET_FEAT_CERT_INDEX;
    }

    socket_create_request.socket_cert_inx = si91x_bsd_socket->certificate_index;

    // Check if extension is provided my application and memcopy until the provided size of extensions
    if (si91x_bsd_socket->sni_extensions.total_extensions > 0) {
      memcpy(socket_create_request.tls_extension_data,
             si91x_bsd_socket->sni_extensions.buffer,
             si91x_bsd_socket->sni_extensions.current_size_of_extensions);

      socket_create_request.total_extension_length = si91x_bsd_socket->sni_extensions.current_size_of_extensions;
      socket_create_request.no_of_tls_extensions   = si91x_bsd_socket->sni_extensions.total_extensions;
    }
    wait_period = SL_SI91X_WAIT_FOR_RESPONSE(150000); // timeout is 15 sec
  }

  // Check for HIGH_PERFORMANCE feature bit
  if (si91x_bsd_socket->ssl_bitmap & SI91X_HIGH_PERFORMANCE_SOCKET) {
    socket_create_request.ssl_bitmap |= SI91X_HIGH_PERFORMANCE_SOCKET;
  }

#ifdef SLI_SI917
  // Set socket's max retransmission timeout value and TOS (Type of Service) if applicable
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

  // If the status is not OK and there's a buffer, free the buffer
  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sl_si91x_host_free_buffer(buffer);
  }
  VERIFY_STATUS_AND_RETURN(status);
  // Extract socket creation response information
  packet                 = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  socket_create_response = (sl_si91x_socket_create_response_t *)packet->data;

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
    sl_si91x_host_free_buffer(buffer);
    return SL_STATUS_OK;
  }

  // Copy the local address (IPv4 or IPv6) based on family type
  if (si91x_bsd_socket->local_address.sin6_family == AF_INET) {
    memcpy(&((struct sockaddr_in *)&si91x_bsd_socket->local_address)->sin_addr.s_addr,
           socket_create_response->module_ip_addr.ipv4_addr,
           SL_IPV4_ADDRESS_LENGTH);
  } else {
    memcpy(si91x_bsd_socket->local_address.sin6_addr.__u6_addr.__u6_addr8,
           socket_create_response->module_ip_addr.ipv6_addr,
           SL_IPV6_ADDRESS_LENGTH);
  }

  // Free the buffer
  sl_si91x_host_free_buffer(buffer);

  return SL_STATUS_OK;
}

// Shutdown a socket
int sli_si91x_shutdown(int socket, int how)
{
  sl_status_t status                                      = SL_STATUS_OK;
  sl_si91x_socket_close_request_t socket_close_request    = { 0 };
  sl_si91x_socket_close_response_t *socket_close_response = NULL;
  sl_si91x_wait_period_t wait_period                      = SL_SI91X_WAIT_FOR_RESPONSE(SL_SI91X_WAIT_FOR_EVER);
  sl_wifi_buffer_t *buffer                                = NULL;
  void *sdk_context                                       = NULL;

  si91x_socket_t *si91x_socket = get_si91x_socket(socket);

  // Verify the socket's existence
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket == NULL, EBADF);

  // The firmware maps server socket and first client socket connected to the server would be assigned same firmware socket ID.
  // Therefore, if Dev attempts to close either first client or server, close request type needs to be set to SHUTDOWN_BY_PORT.
  int close_request_type = (si91x_socket->state == LISTEN) ? SHUTDOWN_BY_PORT : how;

  // If the socket is in an initial state or marked for auto-close, reset it and return
  if (si91x_socket->state == BOUND || si91x_socket->state == INITIALIZED
      || (si91x_socket->state == DISCONNECTED && is_tcp_auto_close_enabled())) {
    reset_socket_state(socket);

    return SI91X_NO_ERROR;
  }

  sdk_context = &(si91x_socket->id);
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
                                               &wait_period,
                                               sdk_context);

  // If the status is not OK and there's a buffer, free the buffer
  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sl_si91x_host_free_buffer(buffer);
  }
  SOCKET_VERIFY_STATUS_AND_RETURN(status, SL_STATUS_OK, SI91X_UNDEFINED_ERROR);
  if (close_request_type == SHUTDOWN_BY_ID && si91x_socket->id == socket_close_response->socket_id) {
    reset_socket_state(socket);
    sl_si91x_host_free_buffer(buffer);
    return SI91X_NO_ERROR;
  }
  // Reset sockets that match the close request
  for (uint8_t index = 0; index < NUMBER_OF_SOCKETS; index++) {
    si91x_socket_t *socket = get_si91x_socket(index);
    //Verifying socket existence
    if (socket == NULL)
      continue;
    if (close_request_type == SHUTDOWN_BY_ID && socket->id == socket_close_response->socket_id) {
      reset_socket_state(index);
    } else if (close_request_type == SHUTDOWN_BY_PORT
               && socket->local_address.sin6_port == socket_close_response->port_number) {
      reset_socket_state(index);
    }
  }

  sl_si91x_host_free_buffer(buffer);

  return SI91X_NO_ERROR;
}

sl_status_t si91x_socket_event_handler(sl_status_t status,
                                       sl_si91x_socket_context_t *sdk_context,
                                       sl_si91x_packet_t *rx_packet)
{
  UNUSED_PARAMETER(status);

  // Handle connection establishment response
  if (rx_packet->command == RSI_WLAN_RSP_CONN_ESTABLISH) {
    sl_si91x_rsp_ltcp_est_t *accept_response = (sl_si91x_rsp_ltcp_est_t *)rx_packet->data;
    int32_t *client_socket_id                = &client_socket_accept_id;

    handle_accept_response(*client_socket_id, accept_response);

    si91x_socket_t *client_socket = get_si91x_socket(*client_socket_id);
    //Verifying socket existence
    if (client_socket == NULL) {
      return -1;
    }
    // Call the accept callback function with relevant socket information
    user_accept_callback(*client_socket_id, (struct sockaddr *)&client_socket->remote_address, client_socket->type);
  }
  // Handle remote socket termination response
  else if (rx_packet->command == RSI_WLAN_RSP_REMOTE_TERMINATE) {

    sl_si91x_socket_close_response_t *remote_socket_closure = (sl_si91x_socket_close_response_t *)rx_packet->data;
    // Reset sockets that match the close request
    for (uint8_t index = 0; index < NUMBER_OF_SOCKETS; index++) {
      si91x_socket_t *socket = get_si91x_socket(index);
      //Verifying socket existence
      if (socket == NULL || remote_socket_closure->socket_id != socket->id || socket->state == LISTEN)
        continue;

      socket->state = DISCONNECTED;
      /* Flush the pending tx request packets from the socket command queue */
      sl_si91x_host_flush_nodes_from_queue(SI91X_SOCKET_CMD_QUEUE,
                                           remote_socket_closure,
                                           si91x_socket_identification_function,
                                           si91x_socket_node_free_function);

      if (user_remote_socket_termination_callback != NULL) {
        user_remote_socket_termination_callback(socket->id,
                                                socket->local_address.sin6_port,
                                                remote_socket_closure->sent_bytes_count);
      }
      break;
    }
  } else if (rx_packet->command == RSI_RECEIVE_RAW_DATA) {
    // Handle the case when raw data is received
    si91x_rsp_socket_recv_t *firmware_socket_response = (si91x_rsp_socket_recv_t *)rx_packet->data;
    uint8_t *data                                     = (uint8_t *)(rx_packet->data + firmware_socket_response->offset);

    int8_t host_socket = -1;

    // Find the host socket corresponding to the received data
    for (uint8_t host_socket_index = 0; host_socket_index < NUMBER_OF_SOCKETS; host_socket_index++) {
      if ((sli_si91x_sockets[host_socket_index] != NULL)
          && (firmware_socket_response->socket_id == sli_si91x_sockets[host_socket_index]->id)) {
        host_socket = host_socket_index;
      }
    }

    // Retrieve the client socket
    si91x_socket_t *client_socket = get_si91x_socket(host_socket);
    //Verifying socket existence
    if (client_socket == NULL) {
      SL_CLEANUP_MALLOC(sdk_context);
      return -1;
    }
    // Call the user-defined receive data callback
    client_socket->recv_data_callback(host_socket, data, firmware_socket_response->length);
  } else if (rx_packet->command == RSI_WLAN_RSP_SELECT_REQUEST) {
    fd_set read_fd, write_fd, exception_fd;

    // This function handles responses received from the SI91X socket driver
    handle_select_response((sl_si91x_socket_select_rsp_t *)rx_packet->data, &read_fd, &write_fd, &exception_fd);

    // Call the user-defined select callback function with the updated file descriptor sets and status
    user_select_callback(&read_fd, &write_fd, &exception_fd, status);
  }
  // This block of code is executed when a TCP acknowledgment indication is received.
  else if (rx_packet->command == RSI_WLAN_RSP_TCP_ACK_INDICATION) {
    sl_si91x_rsp_tcp_ack_t *tcp_ack = (sl_si91x_rsp_tcp_ack_t *)rx_packet->data;

    // Initialize a variable to store the host socket ID
    int8_t host_socket = -1;

    // Iterate through all host sockets to find a matching socket ID
    for (uint8_t host_socket_index = 0; host_socket_index < NUMBER_OF_SOCKETS; host_socket_index++) {
      if ((sli_si91x_sockets[host_socket_index] != NULL)
          && (tcp_ack->socket_id == sli_si91x_sockets[host_socket_index]->id)) {
        host_socket = host_socket_index;
        break;
      }
    }
    // Retrieve the SI91X socket associated with the host socket
    si91x_socket_t *si91x_socket = get_si91x_socket(host_socket);
    //Verifying socket existence
    if (si91x_socket == NULL) {
      SL_CLEANUP_MALLOC(sdk_context);
      return -1;
    }
    // Check if the SI91X socket and its data transfer callback function exist
    if (si91x_socket != NULL && si91x_socket->data_transfer_callback != NULL) {
      si91x_socket->data_transfer_callback(host_socket, (uint8_t)(tcp_ack->length[0] | tcp_ack->length[1] << 8));
    }
  }

  // Cleanup any dynamically allocated memory in the SDK context
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
                                                sl_si91x_wait_period_t *wait_period,
                                                void *sdk_context)
{
  // Unused parameters (to suppress compiler warnings)
  UNUSED_PARAMETER(response_queue);
  UNUSED_PARAMETER(events_to_wait_for);
  sl_status_t status;
  sl_si91x_packet_t *packet;

  if (response != NULL) {
    *response = NULL;
  }

  if (wait_period != NULL) {
    // Send the command and data with the specified wait period
    status = sl_si91x_driver_send_command(command, queue, data, data_length, *wait_period, sdk_context, buffer);
  } else {
    // Send the command and data with immediate return if no wait period is specified
    status =
      sl_si91x_driver_send_command(command, queue, data, data_length, SL_SI91X_RETURN_IMMEDIATELY, sdk_context, buffer);

    if (status != SL_STATUS_IN_PROGRESS) {
      return SL_STATUS_FAIL;
    }
  }

  // Check if a wait period is specified and if the wait period bit for response is set.
  if ((wait_period != NULL) && (status != SL_STATUS_OK || ((*wait_period) & (SL_SI91X_WAIT_FOR_RESPONSE_BIT == 0)))) {
    return status;
  }

  packet = sl_si91x_host_get_buffer_data(*buffer, 0, NULL);
  if (packet != NULL) {
    *response = packet->data;
  } else {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

int sli_si91x_connect(int socket, const struct sockaddr *addr, socklen_t addr_len)
{
  errno = 0;

  sl_status_t status = SL_STATUS_FAIL;
  si91x_socket_t *si91x_socket;

  // Retrieve the socket using the socket index
  si91x_socket = get_si91x_socket(socket);

  // Check if the socket is valid.
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket == NULL, EBADF);

  // Check if the socket is already connected
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket->type == SOCK_STREAM && si91x_socket->state == CONNECTED, EISCONN);

  // Check the socket state based on its type
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket->type == SOCK_STREAM && si91x_socket->state > BOUND, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket->type == SOCK_DGRAM && si91x_socket->state != INITIALIZED
                                 && si91x_socket->state != BOUND && si91x_socket->state != UDP_UNCONNECTED_READY,
                               EBADF);

  // Check if the provided sockaddr length is sufficient
  SET_ERRNO_AND_RETURN_IF_TRUE(
    (si91x_socket->local_address.sin6_family == AF_INET && addr_len < sizeof(struct sockaddr_in))
      || (si91x_socket->local_address.sin6_family == AF_INET6 && addr_len < sizeof(struct sockaddr_in6)),
    EINVAL);

  // Check if the provided sockaddr pointer is valid
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

  // Prepare socket request based on socket type and send the request to the bus driver
  if (si91x_socket->type == SOCK_STREAM) {
    status = create_and_send_socket_request(socket, SI91X_SOCKET_TCP_CLIENT, NULL);
  } else if (si91x_socket->type == SOCK_DGRAM) {
    status = create_and_send_socket_request(socket, SI91X_SOCKET_LUDP, NULL);
  }

  // Verify the status of the socket operation and return errors if necessary
  SOCKET_VERIFY_STATUS_AND_RETURN(status, SL_STATUS_OK, SI91X_UNDEFINED_ERROR);

  // Update the socket state to "CONNECTED" and return success
  si91x_socket->state = CONNECTED;
  return SI91X_NO_ERROR;
}