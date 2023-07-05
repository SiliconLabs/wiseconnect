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
#include "netdb.h"
#include "sl_si91x_driver.h"
#include "sl_rsi_utility.h"
#include "sl_si91x_status.h"
#include "sl_si91x_protocol_types.h"
#include "sl_si91x_host_interface.h"
#include "sl_si91x_bsd_socket_types.h"
#include "sl_ip_types.h"
#include "sl_net_dns.h"
#include "netinet_in.h"
#include "netinet6_in6.h"
#include "errno.h"
#include "socket.h"
#include "select.h"
#include "sl_bsd_utility.h"

/******************************************************
 *                      Macros
 ******************************************************/
#define SET_ERROR_AND_RETURN(error)    \
  {                                    \
    do {                               \
      if (PRINT_ERROR_LOGS) {          \
        PRINT_ERROR_STATUS(ERROR_TAG, error) \
      }                                \
      errno = error;                   \
      return -1;                       \
    } while (0);                       \
  }

#define SET_ERRNO_AND_RETURN_IF_TRUE(condition, errno_value) \
  {                                                          \
    do {                                                     \
      if (condition) {                                       \
        if (PRINT_ERROR_LOGS) {                              \
          PRINT_ERROR_STATUS(ERROR_TAG, errno_value)               \
        }                                                    \
        errno = errno_value;                                 \
        return -1;                                           \
      }                                                      \
    } while (0);                                             \
  }

#define BSD_VERIFY_STATUS_AND_RETURN(status, expected_status, errno_value) \
  {                                                                        \
    do {                                                                   \
      if (status != expected_status) {                                     \
        if (PRINT_ERROR_LOGS) {                                            \
          PRINT_ERROR_STATUS(ERROR_TAG, errno_value)                             \
        }                                                                  \
        errno = errno_value;                                               \
        return -1;                                                         \
      }                                                                    \
    } while (0);                                                           \
  }

#define GET_SAFE_MEMCPY_LENGTH(destination_size, source_size) \
  source_size > destination_size ? destination_size : source_size

#define IS_POWER_OF_TWO(x) (x < 0) ? 0 : (x && (!(x & (x - 1))))
/******************************************************
 *                    Constants
 ******************************************************/
#define NUMBER_OF_SOCKETS 10

#define SI91X_BSD_NO_ERROR 0 // This is returned by BSD functions after successful execution
#define SI91X_BSD_UNDEFINED_ERROR \
  0 // This value is set to errno global variable when a BSD API failed due to unknown error.

#define MAX_TCP_RETRY_COUNT 10

#define SI91X_SOCKET_TCP_CLIENT 0x0000
#define SI91X_SOCKET_UDP_CLIENT 0x0001
#define SI91X_SOCKET_TCP_SERVER 0x0002
#define SI91X_SOCKET_LUDP       0x0004

#define SI91X_SOCKET_FEAT_SYNCHRONOUS        BIT(0)
#define SI91X_SOCKET_FEAT_LTCP_ACCEPT        BIT(1)
#define SI91X_SOCKET_FEAT_TCP_ACK_INDICATION BIT(2)
#define SI91X_SOCKET_FEAT_TCP_RX_WINDOW      BIT(4)
#define SI91X_SOCKET_FEAT_CERT_INDEX         BIT(5)

#define SI91X_HOST_WAIT_FOR_SELECT_RSP 1000

#define SI91X_BSD_SOCKET_LOCAL_ADDRESS 0
#define SI91X_BSD_SOCKET_PEER_ADDRESS  1

#define TCP_HEADER_LENGTH                    56
#define UDP_HEADER_LENGTH                    44
#define SOCKET_LISTEN_RESPONSE_PACKET_LENGTH 42

#define DEFAULT_STREAM_MSS_SIZE   1460
#define DEFAULT_DATAGRAM_MSS_SIZE 1472

#define SI91X_SSL_HEADER_SIZE 90

#define DEFAULT_TCP_KEEP_ALIVE_TIME 1200

#define TCP_TX_ONLY_SOCKETS             3
#define TCP_RX_ONLY_SOCKETS             3
#define TCP_RX_HIGH_PERFORMANCE_SOCKETS 2
#define TOTAL_TCP_SOCKETS               ((TCP_TX_ONLY_SOCKETS) + (TCP_RX_ONLY_SOCKETS))

#define UDP_TX_ONLY_SOCKETS 1
#define UDP_RX_ONLY_SOCKETS 1
#define TOTAL_UDP_SOCKETS   ((UDP_TX_ONLY_SOCKETS) + (UDP_RX_ONLY_SOCKETS))

#define TOTAL_SOCKETS TOTAL_TCP_SOCKETS + TOTAL_UDP_SOCKETS

#define TCP_RX_WINDOW_SIZE            10
#define TCP_RX_WINDOW_DIVISION_FACTOR 10

#define SI91X_HIGH_PERFORMANCE_SOCKET  BIT(7)

#define SI91X_CERT_INDEX_0 0
#define SI91X_CERT_INDEX_1 1
#define SI91X_CERT_INDEX_2 2

#define MAX_RETRANSMISSION_TIME_VALUE 32

/******************************************************
 *                   Enumerations
 ******************************************************/
typedef enum {
  RESET = 0,   // State of unallocated socket.
  INITIALIZED, // Socket attains this state when socket() has been executed successfully.
  BOUND,       // Socket attains this state when bind() has been executed successfully.
  LISTEN,      // (TCP ONLY STATE) Socket attains this state when listen() has been executed successfully.
  UDP_UNCONNECTED_READY, // (UDP ONLY STATE) Socket attains this state when sendto() or recvfrom() has been executed successfully prior connect.
  CONNECTED,   // Socket attains this state when connect() has been executed successfully.
  DISCONNECTED // Socket attains this state when underlying connection is lost
} si91x_bsd_socket_state_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/
typedef struct {
  int32_t id;
  int32_t type;
  int32_t protocol;
  uint16_t tcp_keepalive_initial_time;
  uint16_t read_timeout;
  uint8_t certificate_index;
  uint16_t mss;
  struct sockaddr_in6 local_address; // Using sockaddr_in6 to hold either IPV4 or IPV6.
  struct sockaddr_in6 remote_address;
  si91x_bsd_socket_state_t state;
#ifdef CHIP_917
  uint32_t ssl_bitmap;
  uint32_t max_retransmission_timeout_value;
  uint32_t tos;
#else
  uint8_t ssl_bitmap;
#endif
} si91x_bsd_socket_t;

/******************************************************
 *               Static Function Declarations
 ******************************************************/
static int32_t create_and_send_socket_request(int32_t socketIdIndex, int32_t type, int32_t *backlog);
static sl_status_t si91x_config_socket(void);

/******************************************************
 *               Variable Definitions
 ******************************************************/
static si91x_bsd_socket_t sockets[NUMBER_OF_BSD_SOCKETS];
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

// Defining errno extern variable of errno.h
int errno = 0;

static int herrno;
static struct hostent host_ent;
static sl_ip_address_t host_ip_address;
static bool is_configured = false;

/******************************************************
 *               Function Definitions
 ******************************************************/
int *__h_errno_location(void)
{
  return &herrno;
}

static inline si91x_bsd_socket_t *get_socket(int socket_id)
{
  return (((socket_id >= 0) && (socket_id < NUMBER_OF_SOCKETS)) && sockets[socket_id].state != RESET)
           ? &sockets[socket_id]
           : NULL;
}

static void reset_socket_state(int socket_id)
{
  si91x_bsd_socket_t *socket_to_be_reset = get_socket(socket_id);

  if (NULL != socket_to_be_reset) {
    memset(socket_to_be_reset, 0, sizeof(si91x_bsd_socket_t));
  }
}

int16_t si91x_get_socket_mss(int32_t socketIndex)
{
  si91x_bsd_socket_t *si91x_socket = get_socket(socketIndex);

  if (si91x_socket == NULL)
    return -1;

  if ((si91x_socket->type == SOCK_STREAM) && si91x_socket->state == CONNECTED) {
    if (si91x_socket->ssl_bitmap & SL_SI91X_ENABLE_TLS) {
      return (si91x_socket->mss - SI91X_SSL_HEADER_SIZE);
    } else {
      return si91x_socket->mss;
    }
  } else if (si91x_socket->type == SOCK_DGRAM) {
    return DEFAULT_DATAGRAM_MSS_SIZE;
  }

  return -1;
}

static sl_status_t socket_disconnect_event_handler(uint32_t event, sl_status_t status, void *data, uint32_t data_length)
{
  UNUSED_PARAMETER(event);
  UNUSED_PARAMETER(status);
  UNUSED_PARAMETER(data_length);
  sl_si91x_socket_close_response_t *remote_socket_closure = (sl_si91x_socket_close_response_t *)data;

  for (uint8_t socket_index = 0; socket_index < NUMBER_OF_SOCKETS; socket_index++) {
    if (sockets[socket_index].id == remote_socket_closure->socket_id) {
      sockets[socket_index].state = DISCONNECTED;
      break;
    }
  }
  return SL_STATUS_OK;
}

/**
 * A static utility function to send BSD management commands.
 * @param  command 				Command which needs to be sent to module.
 * @param  data 				Pointer to data segment of the request.
 * @param  data_length 			length of data segment.
 * @param  queue 				Queue to which the commands needs to be enqueued.
 * @param  buffer				Buffer to hold address RX packet.
 * @param  response 			Double Pointer to response pointer, Response pointer can be NULL in case of response bit not being set in wait_period or if the sent command has failed.
 * @param  events_to_wait_for 	Pointer to events on which the API should wait on. This will be considered only in case of wait_period being null
 * @param  wait_period 			pointer to sl_si91x_wait_period_t to determine timeout and response bit. If this pointer is null, the function will block until it receives a response.
 * @return sl_status 			SL_STATUS_OK in case of success or appropriate sl_status error value.
 */
static sl_status_t si91x_bsd_send_driver_command(rsi_wlan_cmd_request_t command,
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

// Prepare socket request based on socket type and send the request down to the driver.
// socket type : [SL_SOCKET_TCP_SERVER, SL_SOCKET_TCP_CLIENT, SL_SOCKET_LUDP, SL_SOCKET_UDP_CLIENT]
static int32_t create_and_send_socket_request(int32_t socketIdIndex, int32_t type, int32_t *backlog)
{
  errno = 0;

  sl_status_t status                                     = SL_STATUS_OK;
  sl_si91x_socket_create_request_t socket_create_request    = { 0 };
  sl_si91x_socket_create_response_t *socket_create_response = NULL;
  si91x_bsd_socket_t *si91x_bsd_socket                   = get_socket(socketIdIndex);
  sl_si91x_wait_period_t wait_period  = SL_SI91X_WAIT_FOR_COMMAND_RESPONSE;

  if (type == SI91X_SOCKET_TCP_CLIENT ){
  //In wlan_throughput example the tick_count_s configured as 10, for that reason the connect was timed out.
  //To avoid timeout, need to configure wait_period as 100000
   wait_period                     = SL_SI91X_WAIT_FOR_RESPONSE(100000);
  } 

  sl_wifi_buffer_t *buffer;

  if (si91x_bsd_socket->local_address.sin6_family == AF_INET6) {
    socket_create_request.ip_version = SL_IPV6_VERSION;

    memcpy(socket_create_request.dest_ip_addr.ipv6_address,
           si91x_bsd_socket->remote_address.sin6_addr.__u6_addr.__u6_addr8,
           SL_IPV6_ADDRESS_LENGTH);
  } else {
    socket_create_request.ip_version = SL_IPV4_VERSION;
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

  socket_create_request.socket_bitmap |= SI91X_SOCKET_FEAT_SYNCHRONOUS;
  socket_create_request.socket_bitmap |= SI91X_SOCKET_FEAT_TCP_RX_WINDOW;

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

  status = si91x_bsd_send_driver_command(RSI_WLAN_REQ_SOCKET_CREATE,
                                         &socket_create_request,
                                         sizeof(socket_create_request),
                                         SI91X_SOCKET_CMD_QUEUE,
                                         SI91X_SOCKET_RESPONSE_QUEUE,
                                         &buffer,
                                         (void *)&socket_create_response,
                                         NULL,
                                         &wait_period);

  BSD_VERIFY_STATUS_AND_RETURN(status, SL_STATUS_OK, SI91X_BSD_UNDEFINED_ERROR);

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
    return SI91X_BSD_NO_ERROR;
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

  return SI91X_BSD_NO_ERROR;
}

static sl_status_t si91x_config_socket(void)
{
  sl_status_t status                 = SL_STATUS_OK;
  sl_si91x_wait_period_t wait_period = SL_SI91X_WAIT_FOR_COMMAND_SUCCESS;
  errno                              = 0;

  status = si91x_bsd_send_driver_command(RSI_WLAN_REQ_SOCKET_CONFIG,
                                         &socket_config,
                                         sizeof(socket_config),
                                         SI91X_SOCKET_CMD_QUEUE,
                                         0,
                                         NULL,
                                         NULL,
                                         NULL,
                                         &wait_period);

  BSD_VERIFY_STATUS_AND_RETURN(status, SL_STATUS_OK, SI91X_BSD_UNDEFINED_ERROR);
  return status;
}

/**
 * A static utility function to get either peer address or socket address given the socket ID.
 * @param socket_id 			ID of socket.
 * @param name 					structure to hold the IP address, port, family type of IP address
 * @param name_len 				size of of buffer sent by caller.
 * @param required_address_type Whether peer data or local data to be copied to buffer.
 * @return						0 when successes, otherwise -1.
 */
static int get_sock_address(int socket_id, struct sockaddr *name, socklen_t *name_len, uint8_t required_address_type)
{
  si91x_bsd_socket_t *si91x_socket = get_socket(socket_id);
  uint8_t bytes_to_be_copied = (*name_len > sizeof(struct sockaddr_in6)) ? sizeof(struct sockaddr_in6) : *name_len;

  // Validate socket
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket == NULL, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(name == NULL || name_len == NULL, EFAULT);
  SET_ERRNO_AND_RETURN_IF_TRUE(*name_len <= 0, EINVAL);

  SET_ERRNO_AND_RETURN_IF_TRUE(
    required_address_type == SI91X_BSD_SOCKET_PEER_ADDRESS && si91x_socket->state != CONNECTED,
    ENOTCONN)
  SET_ERRNO_AND_RETURN_IF_TRUE(required_address_type == SI91X_BSD_SOCKET_LOCAL_ADDRESS
                                 && (si91x_socket->state < BOUND || si91x_socket->state == DISCONNECTED),
                               EBADF);

  if (required_address_type == SI91X_BSD_SOCKET_LOCAL_ADDRESS) {
    memcpy(name, &si91x_socket->local_address, bytes_to_be_copied);
  } else {
    memcpy(name, &si91x_socket->remote_address, bytes_to_be_copied);
  }

  *name_len = si91x_socket->local_address.sin6_family == AF_INET ? sizeof(struct sockaddr_in)
                                                                 : sizeof(struct sockaddr_in6);
  return SI91X_BSD_NO_ERROR;
}

sl_status_t sl_si91x_socket_init(void)
{
  memset(sockets, 0, sizeof(sockets));
  return SL_STATUS_OK;
}

int socket(int family, int type, int protocol)
{
  SET_ERRNO_AND_RETURN_IF_TRUE(family != AF_INET && family != AF_INET6, EAFNOSUPPORT);

  SET_ERRNO_AND_RETURN_IF_TRUE(type != SOCK_STREAM && type != SOCK_DGRAM, EINVAL);
  SET_ERRNO_AND_RETURN_IF_TRUE(protocol != IPPROTO_TCP && protocol != IPPROTO_UDP && protocol != 0, EINVAL);

  SET_ERRNO_AND_RETURN_IF_TRUE((type == SOCK_STREAM && (protocol != IPPROTO_TCP && protocol != 0)), EPROTOTYPE);
  SET_ERRNO_AND_RETURN_IF_TRUE((type == SOCK_DGRAM && (protocol != IPPROTO_UDP && protocol != 0)), EPROTOTYPE);

  if (!is_configured) {
    sl_si91x_socket_init();
    if (si91x_config_socket() != SL_STATUS_OK) {
      SET_ERROR_AND_RETURN(SI91X_BSD_UNDEFINED_ERROR);
    }

    sl_si91x_register_socket_callback(RSI_WLAN_RSP_REMOTE_TERMINATE, (sl_net_event_handler_t)socket_disconnect_event_handler);
    is_configured = true;
  }

  for (uint8_t socket_index = 0; socket_index < NUMBER_OF_BSD_SOCKETS; socket_index++) {
    si91x_bsd_socket_t *si91x_socket = &sockets[socket_index];

    if (si91x_socket->state != RESET) {
      continue;
    }

    si91x_socket->type                      = type;
    si91x_socket->local_address.sin6_family = family;
    si91x_socket->protocol                  = protocol;
    si91x_socket->state                     = INITIALIZED;

    return socket_index;
  }

  SET_ERROR_AND_RETURN(ENOMEM);
}

int bind(int socket_id, const struct sockaddr *addr, socklen_t addr_len)
{
  errno = 0;

  si91x_bsd_socket_t *si91x_socket   = get_socket(socket_id);
  struct sockaddr_in *socket_address = (struct sockaddr_in *)addr;

  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket == NULL || si91x_socket->state != INITIALIZED, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(
    (si91x_socket->local_address.sin6_family == AF_INET && addr_len < sizeof(struct sockaddr_in))
      || (si91x_socket->local_address.sin6_family == AF_INET6 && addr_len < sizeof(struct sockaddr_in6)),
    EINVAL);

  SET_ERRNO_AND_RETURN_IF_TRUE(addr == NULL, EFAULT);

  // Check whether local port is already used or not
  for (uint16_t socket_index = 0; socket_index < NUMBER_OF_SOCKETS; socket_index++) {
    if (sockets[socket_index].local_address.sin6_port == socket_address->sin_port) {

      SET_ERROR_AND_RETURN(EADDRINUSE)
    }
  }

  memcpy(&si91x_socket->local_address,
         addr,
         (addr_len > sizeof(struct sockaddr_in6)) ? sizeof(struct sockaddr_in6) : addr_len);

  si91x_socket->state = BOUND;
  return SI91X_BSD_NO_ERROR;
}

int listen(int socket_id, int backlog)
{
  errno = 0;

  int32_t status;
  si91x_bsd_socket_t *si91x_socket = get_socket(socket_id);

  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket == NULL, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket->state != INITIALIZED && si91x_socket->state != BOUND, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket->type != SOCK_STREAM, EOPNOTSUPP);

  status = create_and_send_socket_request(socket_id, SI91X_SOCKET_TCP_SERVER, (int32_t *)&backlog);

  BSD_VERIFY_STATUS_AND_RETURN(status, SI91X_BSD_NO_ERROR, SI91X_BSD_UNDEFINED_ERROR);

  si91x_socket->state = LISTEN;

  return SI91X_BSD_NO_ERROR;
}

int accept(int socket_id, struct sockaddr *addr, socklen_t *addr_len)
{
  sl_si91x_wait_period_t wait_time = 0;
  errno                            = 0;

  si91x_bsd_socket_t *si91x_server_socket = get_socket(socket_id);
  si91x_bsd_socket_t *si91x_client_socket = NULL;

  sl_si91x_socket_accept_request_t accept_request = { 0 };
  sl_wifi_buffer_t *buffer                     = NULL;
  sl_si91x_rsp_ltcp_est_t *ltcp                     = NULL;

  sl_status_t status       = SL_STATUS_OK;
  int32_t client_socket_id = -1;
  uint32_t events          = NCP_HOST_SOCKET_RESPONSE_EVENT;

  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_server_socket == NULL, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_server_socket->type != SOCK_STREAM, EOPNOTSUPP);
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_server_socket->state != LISTEN, EINVAL);

  // Create a new instance for socket
  client_socket_id =
    socket(si91x_server_socket->local_address.sin6_family, si91x_server_socket->type, si91x_server_socket->protocol);

  SET_ERRNO_AND_RETURN_IF_TRUE(client_socket_id < 0, SI91X_BSD_UNDEFINED_ERROR);

  si91x_client_socket = get_socket(client_socket_id);

  memcpy(&si91x_client_socket->local_address, &si91x_server_socket->local_address, sizeof(struct sockaddr_in6));

  // Create accept request
  accept_request.socket_id   = si91x_server_socket->id;
  accept_request.source_port = si91x_server_socket->local_address.sin6_port;
  wait_time                  = (SL_SI91X_WAIT_FOR_EVER | SL_SI91X_WAIT_FOR_RESPONSE_BIT);

  status = si91x_bsd_send_driver_command(RSI_WLAN_REQ_SOCKET_ACCEPT,
                                         &accept_request,
                                         sizeof(accept_request),
                                         SI91X_SOCKET_CMD_QUEUE,
                                         SI91X_SOCKET_RESPONSE_QUEUE,
                                         &buffer,
                                         (void *)&ltcp,
                                         &events,
                                         &wait_time);

  if (status != SL_STATUS_OK) {
    close(client_socket_id);
    SET_ERROR_AND_RETURN(SI91X_BSD_UNDEFINED_ERROR);
  }

  si91x_client_socket->id                         = ltcp->socket_id;
  si91x_client_socket->local_address.sin6_port    = ltcp->src_port_num;
  si91x_client_socket->remote_address.sin6_port   = ltcp->dest_port;
  si91x_client_socket->mss                        = ltcp->mss;
  si91x_client_socket->state                      = CONNECTED;
  si91x_client_socket->remote_address.sin6_family = ltcp->ip_version == SL_IPV6_VERSION ? AF_INET6 : AF_INET;

  if (si91x_client_socket->remote_address.sin6_family == AF_INET6) {
    memcpy(si91x_client_socket->remote_address.sin6_addr.__u6_addr.__u6_addr8,
           ltcp->dest_ip_addr.ipv6_address,
           SL_IPV6_ADDRESS_LENGTH);

  } else {
    memcpy(&((struct sockaddr_in *)&si91x_client_socket->remote_address)->sin_addr,
           ltcp->dest_ip_addr.ipv4_address,
           SL_IPV4_ADDRESS_LENGTH);
  }

  if (addr_len == NULL || *addr_len <= 0) {
    return client_socket_id;
  }

  memcpy(addr,
         &si91x_client_socket->remote_address,
         (*addr_len > sizeof(struct sockaddr_in6)) ? sizeof(struct sockaddr_in6) : *addr_len);

  *addr_len = si91x_client_socket->local_address.sin6_family == AF_INET ? sizeof(struct sockaddr_in)
                                                                        : sizeof(struct sockaddr_in6);

  return client_socket_id;
}

int connect(int socket_id, const struct sockaddr *addr, socklen_t addr_len)
{
  errno = 0;

  int status                         = SI91X_BSD_UNDEFINED_ERROR;
  si91x_bsd_socket_t *si91x_socket   = get_socket(socket_id);

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

    return SI91X_BSD_NO_ERROR;
  }

  if (si91x_socket->type == SOCK_STREAM) {
    status = create_and_send_socket_request(socket_id, SI91X_SOCKET_TCP_CLIENT, NULL);
  } else if (si91x_socket->type == SOCK_DGRAM) {
    status = create_and_send_socket_request(socket_id, SI91X_SOCKET_LUDP, NULL);
  }

  BSD_VERIFY_STATUS_AND_RETURN(status, SI91X_BSD_NO_ERROR, SI91X_BSD_UNDEFINED_ERROR);

  si91x_socket->state = CONNECTED;
  return SI91X_BSD_NO_ERROR;
}

ssize_t send(int socket_id, const void *data, size_t data_len, int flags)
{
  return sendto(socket_id, data, data_len, flags, NULL, 0);
}

ssize_t recv(int socket_id, void *buf, size_t buf_len, int flags)
{
  return recvfrom(socket_id, buf, buf_len, flags, NULL, NULL);
}

ssize_t sendto(int socket_id,
               const void *data,
               size_t data_len,
               int flags,
               const struct sockaddr *to_addr,
               socklen_t to_addr_len)
{
  UNUSED_PARAMETER(flags);
  errno = 0;

  sl_status_t status                  = SL_STATUS_OK;
  si91x_bsd_socket_t *si91x_socket    = get_socket(socket_id);
  sl_si91x_socket_send_request_t request = { 0 };

  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket == NULL, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket->type == SOCK_STREAM && si91x_socket->state != CONNECTED, ENOTCONN);
  SET_ERRNO_AND_RETURN_IF_TRUE(data == NULL, EFAULT);

  //! Note: The below line of code needs to be removed once the below bug in the Firmware is fixed
  //! https://jira.silabs.com/browse/WIFISDK-406
  if (si91x_socket->type == SOCK_STREAM) {
    if (si91x_socket->ssl_bitmap & SL_SI91X_ENABLE_TLS) {
      SET_ERRNO_AND_RETURN_IF_TRUE((uint16_t)data_len > (si91x_socket->mss - SI91X_SSL_HEADER_SIZE), EMSGSIZE);
    } else {
      SET_ERRNO_AND_RETURN_IF_TRUE(data_len > si91x_socket->mss, EMSGSIZE);
    }
  } else if (si91x_socket->type == SOCK_DGRAM) {
    SET_ERRNO_AND_RETURN_IF_TRUE(data_len > DEFAULT_DATAGRAM_MSS_SIZE, EMSGSIZE);
  }

  if (si91x_socket->type == SOCK_DGRAM && (si91x_socket->state == BOUND || si91x_socket->state == INITIALIZED)) {
    int status = create_and_send_socket_request(socket_id, SI91X_SOCKET_LUDP, NULL);

    SET_ERRNO_AND_RETURN_IF_TRUE(status < 0, SI91X_BSD_UNDEFINED_ERROR);
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
    uint8_t *destination_ip =
      (si91x_socket->state == UDP_UNCONNECTED_READY || to_addr_len >= sizeof(struct sockaddr_in6))
        ? socket_address->sin6_addr.__u6_addr.__u6_addr8
        : si91x_socket->remote_address.sin6_addr.__u6_addr.__u6_addr8;

    memcpy(request.dest_ip_addr.ipv6_address, destination_ip, SL_IPV6_ADDRESS_LENGTH);
  } else {
    struct sockaddr_in *socket_address = (struct sockaddr_in *)to_addr;
    request.ip_version                 = SL_IPV4_VERSION;
    uint32_t *destination_ip =
      (si91x_socket->state == UDP_UNCONNECTED_READY || to_addr_len >= sizeof(struct sockaddr_in))
        ? &socket_address->sin_addr.s_addr
        : &((struct sockaddr_in *)&si91x_socket->remote_address)->sin_addr.s_addr;

    memcpy(request.dest_ip_addr.ipv4_address, destination_ip, SL_IPV4_ADDRESS_LENGTH);
  }

  request.socket_id = si91x_socket->id;
  request.dest_port = ((si91x_socket->state == UDP_UNCONNECTED_READY || to_addr_len > 0))
                        ? ((struct sockaddr_in *)to_addr)->sin_port
                        : si91x_socket->remote_address.sin6_port;
  request.data_offset = (si91x_socket->type == SOCK_STREAM) ? TCP_HEADER_LENGTH : UDP_HEADER_LENGTH;
  request.length      = data_len;

  status = sl_si91x_driver_send_socket_data(&request, data, 0);
  BSD_VERIFY_STATUS_AND_RETURN(status, SL_STATUS_OK, ENOBUFS);

  return data_len;
}

ssize_t recvfrom(int socket_id, void *buf, size_t buf_len, int flags, struct sockaddr *addr, socklen_t *addr_len)
{
  UNUSED_PARAMETER(flags);
  sl_si91x_wait_period_t wait_time = 0;
  errno                            = 0;

  sl_si91x_req_socket_read_t request   = { 0 };
  sl_status_t status                = SL_STATUS_OK;
  uint32_t event                    = NCP_HOST_SOCKET_RESPONSE_EVENT;
  ssize_t bytes_read                = 0;
  si91x_rsp_socket_recv_t *response = NULL;
  si91x_bsd_socket_t *si91x_socket  = get_socket(socket_id);

  sl_wifi_buffer_t *buffer;

  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket == NULL, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket->type == SOCK_STREAM && si91x_socket->state != CONNECTED, ENOTCONN);
  SET_ERRNO_AND_RETURN_IF_TRUE(buf == NULL, EFAULT);
  SET_ERRNO_AND_RETURN_IF_TRUE(buf_len <= 0, EINVAL)

  if (si91x_socket->type == SOCK_DGRAM && (si91x_socket->state == BOUND || si91x_socket->state == INITIALIZED)) {
    int bsd_status = create_and_send_socket_request(socket_id, SI91X_SOCKET_LUDP, NULL);
    BSD_VERIFY_STATUS_AND_RETURN(bsd_status, SI91X_BSD_NO_ERROR, SI91X_BSD_UNDEFINED_ERROR)

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
  memcpy(request.read_timeout, &si91x_socket->read_timeout, sizeof(request.read_timeout));

  wait_time = (SL_SI91X_WAIT_FOR_EVER | SL_SI91X_WAIT_FOR_RESPONSE_BIT);

  status = si91x_bsd_send_driver_command(RSI_WLAN_REQ_SOCKET_READ_DATA,
                                         &request,
                                         sizeof(request),
                                         SI91X_SOCKET_CMD_QUEUE,
                                         SI91X_SOCKET_RESPONSE_QUEUE,
                                         &buffer,
                                         (void *)&response,
                                         &event,
                                         &wait_time);

  BSD_VERIFY_STATUS_AND_RETURN(status, SL_STATUS_OK, SI91X_BSD_UNDEFINED_ERROR);

  bytes_read = (response->length <= buf_len) ? response->length : buf_len;
  memcpy(buf, ((uint8_t *)response + response->offset), bytes_read);

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

  sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
  return bytes_read;
}

int getsockname(int socket_id, struct sockaddr *name, socklen_t *name_len)
{
  return get_sock_address(socket_id, name, name_len, SI91X_BSD_SOCKET_LOCAL_ADDRESS);
}

int getpeername(int socket_id, struct sockaddr *name, socklen_t *name_len)
{
  return get_sock_address(socket_id, name, name_len, SI91X_BSD_SOCKET_PEER_ADDRESS);
}

int setsockopt(int socket_id, int option_level, int option_name, const void *option_value, socklen_t option_length)
{
  si91x_bsd_socket_t *si91x_socket = get_socket(socket_id);
  sl_si91x_time_value *timeout         = NULL;
  uint16_t timeout_val;

  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket == NULL, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(option_value == NULL, EFAULT)
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
      memcpy(&si91x_socket->tcp_keepalive_initial_time,
             (uint16_t *)option_value,
             GET_SAFE_MEMCPY_LENGTH(sizeof(si91x_socket->tcp_keepalive_initial_time), option_length));
      break;
    }

    case TCP_ULP: {
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
#ifdef CHIP_917
      else if (strncmp(option_value, TLS_1_3, option_length) == 0) {
        si91x_socket->ssl_bitmap = SL_SI91X_ENABLE_TLS | SL_SI91X_TLS_V_1_3;
      }
#endif
      break;
    }

    case SO_CERT_INDEX: {
      SET_ERRNO_AND_RETURN_IF_TRUE(
        ((*(uint8_t *)option_value < SI91X_CERT_INDEX_0) || (*(uint8_t *)option_value > SI91X_CERT_INDEX_2)),
        EINVAL);

      si91x_socket->certificate_index = *(uint8_t *)option_value;
      break;
    }
    
    case SO_HIGH_PERFORMANCE_SOCKET: {
      SET_ERRNO_AND_RETURN_IF_TRUE(*(uint8_t *)option_value != SI91X_HIGH_PERFORMANCE_SOCKET, EINVAL);
      si91x_socket->ssl_bitmap |= SI91X_HIGH_PERFORMANCE_SOCKET;
      break;
    }

#ifdef CHIP_917
    case SO_MAX_RETRANSMISSION_TIMEOUT_VALUE: {
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
      memcpy(&si91x_socket->tos,
             (uint8_t *)option_value,
             GET_SAFE_MEMCPY_LENGTH(sizeof(si91x_socket->tos), option_length));
      break;
    }
#endif
    default: {
      SET_ERROR_AND_RETURN(ENOPROTOOPT);
    }
  }

  return SI91X_BSD_NO_ERROR;
}

int getsockopt(int socket_id, int option_level, int option_name, void *option_value, socklen_t *option_length)
{
  si91x_bsd_socket_t *si91x_socket = get_socket(socket_id);

  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket == NULL, EBADF);
  SET_ERRNO_AND_RETURN_IF_TRUE(option_value == NULL, EFAULT);
  SET_ERRNO_AND_RETURN_IF_TRUE(option_level != SOL_SOCKET || option_length == NULL, EINVAL)

  switch (option_name) {
    case SO_RCVTIMEO: {
      *option_length = GET_SAFE_MEMCPY_LENGTH(*option_length, sizeof(si91x_socket->read_timeout));
      memcpy(option_value, &si91x_socket->read_timeout, *option_length);
      break;
    }

    case SO_KEEPALIVE: {
      *option_length = GET_SAFE_MEMCPY_LENGTH(*option_length, sizeof(si91x_socket->tcp_keepalive_initial_time));
      memcpy(option_value, &si91x_socket->tcp_keepalive_initial_time, *option_length);
      break;
    }

    case SO_TYPE: {
      *option_length = GET_SAFE_MEMCPY_LENGTH(*option_length, sizeof(si91x_socket->type));
      memcpy(option_value, &si91x_socket->type, *option_length);
      break;
    }

    case SO_DOMAIN: {
      *option_length = GET_SAFE_MEMCPY_LENGTH(*option_length, sizeof(si91x_socket->local_address.sin6_family));
      memcpy(option_value, &si91x_socket->local_address.sin6_family, *option_length);
      break;
    }

    case SO_PROTOCOL: {
      *option_length = GET_SAFE_MEMCPY_LENGTH(*option_length, sizeof(si91x_socket->protocol));
      memcpy(option_value, &si91x_socket->protocol, *option_length);
      break;
    }

    case SO_ERROR: {
      *option_length = GET_SAFE_MEMCPY_LENGTH(*option_length, sizeof(errno));
      memcpy(option_value, &errno, *option_length);
      errno = 0;
      break;
    }

    case SO_CERT_INDEX: {
      *option_length = GET_SAFE_MEMCPY_LENGTH(*option_length, sizeof(si91x_socket->certificate_index));
      memcpy(option_value, &si91x_socket->certificate_index, *option_length);
      break;
    }

    default: {
      SET_ERROR_AND_RETURN(ENOPROTOOPT);
    }
  }

  return SI91X_BSD_NO_ERROR;
}

int close(int socket_id)
{
  errno = 0;

  sl_status_t status                                   = SL_STATUS_OK;
  sl_si91x_socket_close_request_t socket_close_request    = { 0 };
  sl_si91x_socket_close_response_t *socket_close_response = NULL;
  sl_si91x_wait_period_t wait_period                   = SL_SI91X_WAIT_FOR_RESPONSE(35000);
  sl_wifi_buffer_t *buffer;

  si91x_bsd_socket_t *si91x_socket = get_socket(socket_id);

  SET_ERRNO_AND_RETURN_IF_TRUE(si91x_socket == NULL, EBADF);
  if (si91x_socket->state == BOUND || si91x_socket->state == INITIALIZED
      || (si91x_socket->state == DISCONNECTED && is_tcp_auto_close_enabled())) {
    reset_socket_state(socket_id);

    return SI91X_BSD_NO_ERROR;
  }

  // Socket descriptor based close
  socket_close_request.socket_id = si91x_socket->id;
  // Setting request.port with zero to indicate firmware to close socket based on socket ID not using port.
  socket_close_request.port_number = 0;

  status = si91x_bsd_send_driver_command(RSI_WLAN_REQ_SOCKET_CLOSE,
                                         &socket_close_request,
                                         sizeof(socket_close_request),
                                         SI91X_SOCKET_CMD_QUEUE,
                                         SI91X_SOCKET_RESPONSE_QUEUE,
                                         &buffer,
                                         (void *)&socket_close_response,
                                         NULL,
                                         &wait_period);

  BSD_VERIFY_STATUS_AND_RETURN(status, SL_STATUS_OK, SI91X_BSD_UNDEFINED_ERROR);

  if (socket_close_request.socket_id != socket_close_response->socket_id) {
    sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
    SET_ERROR_AND_RETURN(SI91X_BSD_UNDEFINED_ERROR);
  }

  sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
  reset_socket_state(socket_id);

  return SI91X_BSD_NO_ERROR;
}

struct hostent *gethostbyname(const char *name)
{
  sl_status_t status = SL_STATUS_OK;

  if ((name == NULL) || (strlen(name) > SI91X_DNS_REQUEST_MAX_URL_LEN)) {
    herrno = TRY_AGAIN;
    return NULL;
  }

#ifdef ipv6_FEATURE_REQUIRED
  status = sl_dns_host_get_by_name(name, SL_SI91X_WAIT_FOR_DNS_RESOLUTION, SL_NET_DNS_TYPE_IPV6, &host_ip_address);
#else
  status = sl_dns_host_get_by_name(name, SL_SI91X_WAIT_FOR_DNS_RESOLUTION, SL_NET_DNS_TYPE_IPV4, &host_ip_address);
#endif

  if (status == SL_STATUS_TIMEOUT) {
    herrno = TRY_AGAIN;
    return NULL;
  } else if (status != SL_STATUS_OK) {
    herrno = HOST_NOT_FOUND;
    return NULL;
  }

#ifdef ipv6_FEATURE_REQUIRED
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

  if ((name == NULL) || (strlen(name) > SI91X_DNS_REQUEST_MAX_URL_LEN)) {
    herrno = TRY_AGAIN;
    return NULL;
  }

  if (af == AF_INET6) {
    status = sl_dns_host_get_by_name(name, SL_SI91X_WAIT_FOR_DNS_RESOLUTION, SL_NET_DNS_TYPE_IPV6, &host_ip_address);
  } else {
    status = sl_dns_host_get_by_name(name, SL_SI91X_WAIT_FOR_DNS_RESOLUTION, SL_NET_DNS_TYPE_IPV4, &host_ip_address);
  }

  if (status == SL_STATUS_TIMEOUT) {
    herrno = TRY_AGAIN;
    return NULL;
  } else if (status != SL_STATUS_OK) {
    herrno = HOST_NOT_FOUND;
    return NULL;
  }

  if (af == AF_INET6) {
    host_ent.h_length   = SL_IPV6_ADDRESS_LENGTH;
    host_ent.h_addrtype = AF_INET6;
  } else {
    host_ent.h_length   = SL_IPV4_ADDRESS_LENGTH;
    host_ent.h_addrtype = AF_INET;
  }

  host_ent.h_name = (char *)name;
  host_ent.h_addr = (char *)&host_ip_address;

  herrno = NETDB_SUCCESS;
  return &host_ent;
}

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
  sl_status_t status                = SL_STATUS_OK;

  int32_t total_fd_set_count		= 0; // Specifies total number of FD's set across all categories.
  int32_t select_response_wait_time = 0;

  sl_si91x_socket_select_req_t request = { 0 };
  sl_si91x_socket_select_rsp_t *response = NULL;
  sl_wifi_buffer_t *buffer = NULL;
  sl_si91x_packet_t *packet = NULL;
  sl_si91x_wait_period_t wait_time = 0;

  if ((readfds == NULL) && (writefds == NULL) && (exceptfds == NULL)) {
    SET_ERROR_AND_RETURN(EINVAL);
  }
  if (nfds < 0 || nfds > NUMBER_OF_BSD_SOCKETS) {
    SET_ERROR_AND_RETURN(EINVAL);
  }
  if ((timeout != NULL) && ((timeout->tv_sec < 0) || (timeout->tv_usec < 0))) {
    SET_ERROR_AND_RETURN(EINVAL);
  }

  for (uint8_t host_socket_index = 0; host_socket_index < nfds; host_socket_index++) {
    if (readfds != NULL) {

      if (FD_ISSET(host_socket_index, readfds)) {
    	  request.read_fds.fd_array[0] |= (1U << sockets[host_socket_index].id);
      }
    }

    if (writefds != NULL) {

      if (FD_ISSET(host_socket_index, writefds)) {
    	  request.write_fds.fd_array[0] |= (1U << sockets[host_socket_index].id);
      }
    }

    if (request.num_fd <= sockets[host_socket_index].id) {request.num_fd = sockets[host_socket_index].id + 1;}
  }

  if (timeout != NULL && ((timeout->tv_sec != 0) || (timeout->tv_usec != 0))) {
    request.select_timeout.tv_sec  = timeout->tv_sec;
    request.select_timeout.tv_usec = timeout->tv_usec;
    select_response_wait_time          = ((request.select_timeout.tv_sec * 1000)
                                 + (request.select_timeout.tv_usec / 1000) + SI91X_HOST_WAIT_FOR_SELECT_RSP);
    wait_time                          = SL_SI91X_WAIT_FOR_RESPONSE(select_response_wait_time);
  } else {
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

  BSD_VERIFY_STATUS_AND_RETURN(status, SL_STATUS_OK, SI91X_BSD_UNDEFINED_ERROR);

  packet   = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  response = (sl_si91x_socket_select_rsp_t *)packet->data;

  memset(exceptfds, 0, sizeof(fd_set));
  FD_ZERO(readfds);
  FD_ZERO(writefds);

  for (int host_socket_index = 0; host_socket_index < nfds; host_socket_index++) {
	  int firmware_socket_id = sockets[host_socket_index].id;

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

  sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
  return total_fd_set_count;
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
