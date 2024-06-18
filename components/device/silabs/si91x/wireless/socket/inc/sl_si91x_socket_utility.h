/*******************************************************************************
* @file  sl_si91x_socket_utility.h
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

#include "sl_si91x_types.h"
#include "sl_si91x_socket_types.h"
#include "sl_si91x_protocol_types.h"
#include "sl_si91x_socket_constants.h"
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
#define SLI_SI91X_NULL_SAFE_FD_ZERO(fd_set) \
  {                                         \
    do {                                    \
      if (NULL != fd_set) {                 \
        FD_ZERO(fd_set);                    \
      }                                     \
    } while (0);                            \
  }

#define GET_SAFE_MEMCPY_LENGTH(destination_size, source_size) \
  source_size > destination_size ? destination_size : source_size

#define IS_POWER_OF_TWO(x) (x < 0) ? 0 : (x && (!(x & (x - 1))))

sl_status_t sl_si91x_socket_init(void);
sl_status_t sl_si91x_vap_shutdown(uint8_t vap_id);

/**
 * @addtogroup SOCKET_CONFIGURATION_FUNCTION
 * @{ 
 */
/// SiWx91x Socket Configuration
typedef struct {
  uint8_t total_sockets; ///< Total number of sockets (Which includes BSD, IoT, Si91x)

  uint8_t total_tcp_sockets; ///< Total number of TCP sockets

  uint8_t total_udp_sockets; ///< Total number of UDP sockets

  uint8_t tcp_tx_only_sockets; ///< Number of TCP sockets that are intended for TX

  uint8_t tcp_rx_only_sockets; ///< Number of TCP sockets that are intended for RX

  uint8_t udp_tx_only_sockets; ///< Number of UDP sockets that are intended for TX

  uint8_t udp_rx_only_sockets; ///< Number of UDP sockets that are intended for RX

  uint8_t tcp_rx_high_performance_sockets; ///< Total TCP RX high-performance sockets

  uint8_t
    tcp_rx_window_size_cap; ///< TCP RX window size - To scale the window size linearly according to the value (TCP MSS * TCP_RX_WINDOW_SIZE_CAP)

  uint8_t
    tcp_rx_window_div_factor; ///< TCP RX window division factor - To increase the ACK frequency for asynchronous sockets
} sl_si91x_socket_config_t;
/** @} */

/**
 * @addtogroup SOCKET_CONFIGURATION_FUNCTION
 * @{ 
 */

/***************************************************************************/ /**
 * @brief
 *   This function configures SiWx91x specific socket configuration.
 * @pre Pre-conditions:
 * - 
 *    This API to be called before calling @ref sl_si91x_socket_async.
 * @param[in] socket_config 
 *   Socket configuration of type @ref sl_si91x_socket_config_t.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details. 
 ******************************************************************************/
sl_status_t sl_si91x_config_socket(sl_si91x_socket_config_t socket_config);

/** @} */

/**
 * A internal function to reset the socket.
 * @param socket socket FD which needs to be reset.
 */
void reset_socket_state(int socket);

/**
 * A internal function to get sl_si91x_socket structure based on socket FD sent
 * @param socket 
 * Socket FD whose structure is required.
 * @param index 
 * Socket FD index number.
 * @return 
 * sl_si91x_socket or NULL in case of invalid FD.
 */
void get_free_socket(si91x_socket_t **socket, int *index);

/**
 * A internal function to get free socket.
 * @param socket_id 
 * Socket ID.
 */
si91x_socket_t *get_si91x_socket(int socket_id);

/**
 * A internal function to check whether a particular port is available or not.
 * @param port_number port_number which needs to verified for availability.
 * @return True if available else false.
 */
bool is_port_available(uint16_t port_number);

sl_status_t add_server_name_indication_extension(si91x_server_name_indication_extensions_t *socket_sni_extensions,
                                                 const si91x_socket_type_length_value_t *sni_extension);

sl_status_t create_and_send_socket_request(int socketIdIndex, int type, int *backlog);

int sli_si91x_shutdown(int socket, int how);

int sli_si91x_connect(int socket, const struct sockaddr *addr, socklen_t addr_len);

void handle_accept_response(int client_socket_id, sl_si91x_rsp_ltcp_est_t *accept_response);
int handle_select_response(sl_si91x_socket_select_rsp_t *response,
                           fd_set *readfds,
                           fd_set *writefds,
                           fd_set *exception_fd);

void set_select_callback(select_callback callback);

void sli_si91x_set_accept_callback(accept_callback callback, int32_t client_socket_id);

void sli_si91x_set_remote_socket_termination_callback(remote_socket_termination_callback callback);

/**
 * A utility function to send BSD management commands.
 * @param  command 				Command which needs to be sent to module.
 * @param  data 				Pointer to data segment of the request.
 * @param  data_length 			Length of data segment.
 * @param  queue 				Queue to which the commands needs to be enqueued.
 * @param  response_queue       Response queue.
 * @param  buffer				Buffer to hold address RX packet.
 * @param  response 			Double Pointer to response pointer, Response pointer can be NULL in case of response bit not being set in wait_period or if the sent command has failed.
 * @param  events_to_wait_for 	Pointer to events on which the API should wait on. This will be considered only in case of wait_period being null
 * @param  wait_period 			pointer to sl_si91x_wait_period_t to determine timeout and response bit. If this pointer is null, the function will block until it receives a response.
 * @param  sdk_context          SDK context.
 * @return sl_status 	 SL_STATUS_OK in case of success or appropriate sl_status error value.
 */
sl_status_t sl_si91x_socket_driver_send_command(rsi_wlan_cmd_request_t command,
                                                const void *data,
                                                uint32_t data_length,
                                                sl_si91x_queue_type_t queue,
                                                sl_si91x_queue_type_t response_queue,
                                                sl_wifi_buffer_t **buffer,
                                                void **response,
                                                uint32_t *events_to_wait_for,
                                                sl_si91x_wait_period_t *wait_period,
                                                void *sdk_context);
