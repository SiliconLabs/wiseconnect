#pragma once

#include <stdint.h>

#include "socket.h"
#include "select.h"

typedef void (*receive_data_callback)(uint32_t sock_no, uint8_t *buffer, uint32_t length);
typedef void (*accept_callback)(int32_t sock_id, struct sockaddr *addr, uint8_t ip_version);
typedef void (*data_transfer_complete_handler)(int32_t sockID, uint16_t length);
typedef void (*sl_si91x_sock_data_tx_done_cb)(int32_t sockID, int16_t status, uint16_t total_data_sent);
typedef void (*select_callback)(fd_set *fd_read, fd_set *fd_write, fd_set *fd_except, int32_t status);

typedef enum {
  RESET = 0,   // State of unallocated socket.
  INITIALIZED, // Socket attains this state when socket() has been executed successfully.
  BOUND,       // Socket attains this state when bind() has been executed successfully.
  LISTEN,      // (TCP ONLY STATE) Socket attains this state when listen() has been executed successfully.
  UDP_UNCONNECTED_READY, // (UDP ONLY STATE) Socket attains this state when sendto() or recvfrom() has been executed successfully prior connect.
  CONNECTED,   // Socket attains this state when connect() has been executed successfully.
  DISCONNECTED // Socket attains this state when underlying connection is lost
} si91x_bsd_socket_state_t;

typedef struct {
  int32_t id;
  int32_t type;
  int32_t protocol;
  uint16_t tcp_keepalive_initial_time;
  uint8_t max_tcp_retries;
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

  receive_data_callback recv_data_callback;
  data_transfer_complete_handler data_transfer_callback;
} si91x_socket_t;

typedef struct {
  int nfds;

  fd_set *read_fd;
  fd_set *write_fd;
  fd_set *exception_fd;

  select_callback callback;
} sl_si91x_select_context;
