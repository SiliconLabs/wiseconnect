#pragma once

#include "sl_si91x_types.h"
#include "sl_si91x_socket_types.h"
#include "sl_si91x_protocol_types.h"
#include "errno.h"
#include <stdbool.h>

#define SET_ERROR_AND_RETURN(error)          \
  {                                          \
    do {                                     \
      if (PRINT_ERROR_LOGS) {                \
        PRINT_ERROR_STATUS(ERROR_TAG, error) \
      }                                      \
      errno = error;                         \
      return -1;                             \
    } while (0);                             \
  }

#define SET_ERRNO_AND_RETURN_IF_TRUE(condition, errno_value) \
  {                                                          \
    do {                                                     \
      if (condition) {                                       \
        if (PRINT_ERROR_LOGS) {                              \
          PRINT_ERROR_STATUS(ERROR_TAG, errno_value)         \
        }                                                    \
        errno = errno_value;                                 \
        return -1;                                           \
      }                                                      \
    } while (0);                                             \
  }

#define SOCKET_VERIFY_STATUS_AND_RETURN(status, expected_status, errno_value) \
  {                                                                           \
    do {                                                                      \
      if (status != expected_status) {                                        \
        if (PRINT_ERROR_LOGS) {                                               \
          PRINT_ERROR_STATUS(ERROR_TAG, errno_value)                          \
        }                                                                     \
        errno = errno_value;                                                  \
        return -1;                                                            \
      }                                                                       \
    } while (0);                                                              \
  }

#define GET_SAFE_MEMCPY_LENGTH(destination_size, source_size) \
  source_size > destination_size ? destination_size : source_size

#define IS_POWER_OF_TWO(x) (x < 0) ? 0 : (x && (!(x & (x - 1))))

sl_status_t sl_si91x_socket_init(void);
sl_status_t sl_si91x_socket_deinit(void);

/**
 * A internal function to reset the socket.
 * @param socket socket FD which needs to be reset.
 */
void reset_socket_state(int socket);

/**
 * A internal function to get sl_si91x_socket structure based on socket FD sent
 * @param socket socket FD whose structure is required.
 * @return sl_si91x_socket or NULL in case of invalid FD.
 */
void get_free_socket(si91x_socket_t **socket, int *index);

/**
 * A internal function to get free socket.
 * @param socket  	A double pointer which will be assigned to free sl_si91x_socket structure.
 * @param socket_fd FD of the free socket.
 */
si91x_socket_t *get_si91x_socket(int socket_id);
/**
 * A internal function to check whether a particular port is available or not.
 * @param port_number port_number which needs to verified for availability.
 * @return True if available else false.
 */
bool is_port_available(uint16_t port_number);

sl_status_t create_and_send_socket_request(int socketIdIndex, int type, int *backlog);

int sli_si91x_shutdown(int socket, int how);

void handle_accept_response(int client_socket_id, sl_si91x_rsp_ltcp_est_t *accept_response);
int handle_select_response(sl_si91x_socket_select_rsp_t *response,
                           fd_set *readfds,
                           fd_set *writefds,
                           fd_set *exception_fd);

void set_select_callback(select_callback callback);

/**
 * A utility function to send BSD management commands.
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
sl_status_t sl_si91x_socket_driver_send_command(rsi_wlan_cmd_request_t command,
                                                const void *data,
                                                uint32_t data_length,
                                                sl_si91x_queue_type_t queue,
                                                sl_si91x_queue_type_t response_queue,
                                                sl_wifi_buffer_t **buffer,
                                                void **response,
                                                uint32_t *events_to_wait_for,
                                                sl_si91x_wait_period_t *wait_period);
