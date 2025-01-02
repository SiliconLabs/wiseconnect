/********************************************************************************
 * @file  sl_si91x_socket_utility.h
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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

#pragma once

#include "sl_si91x_types.h"
#include "sl_si91x_socket_types.h"
#include "sl_si91x_protocol_types.h"
#include "sl_si91x_socket_constants.h"
#include "errno.h"
#include <stdbool.h>

#define SET_ERROR_AND_RETURN(error)         \
  do {                                      \
    if (PRINT_ERROR_LOGS) {                 \
      PRINT_ERROR_STATUS(ERROR_TAG, error); \
    }                                       \
    errno = error;                          \
    return -1;                              \
  } while (0)

#define SET_ERRNO_AND_RETURN_IF_TRUE(condition, errno_value) \
  do {                                                       \
    if (condition) {                                         \
      if (PRINT_ERROR_LOGS) {                                \
        PRINT_ERROR_STATUS(ERROR_TAG, errno_value);          \
      }                                                      \
      errno = errno_value;                                   \
      return -1;                                             \
    }                                                        \
  } while (0)

#define SOCKET_VERIFY_STATUS_AND_RETURN(status, expected_status, errno_value) \
  do {                                                                        \
    if (status != expected_status) {                                          \
      if (PRINT_ERROR_LOGS) {                                                 \
        PRINT_ERROR_STATUS(ERROR_TAG, errno_value);                           \
      }                                                                       \
      errno = errno_value;                                                    \
      return -1;                                                              \
    }                                                                         \
  } while (0)

#define SLI_SI91X_NULL_SAFE_FD_ZERO(fd_set) \
  do {                                      \
    if (NULL != fd_set) {                   \
      FD_ZERO(fd_set);                      \
    }                                       \
  } while (0)

#define GET_SAFE_MEMCPY_LENGTH(destination_size, source_size) \
  source_size > destination_size ? destination_size : source_size

#define IS_POWER_OF_TWO(x) (x < 0) ? 0 : (x && (!(x & (x - 1))))

extern sli_si91x_socket_t *sli_si91x_sockets[NUMBER_OF_SOCKETS];

sl_status_t sli_si91x_socket_init(uint8_t max_select_count);

sl_status_t sli_si91x_socket_deinit(void);

sl_status_t sli_si91x_vap_shutdown(uint8_t vap_id, sli_si91x_bsd_disconnect_reason_t disconnect_reason);

bool sli_si91x_is_ip_address_zero(const sl_ip_address_t *ip_addr);

/**
 * @addtogroup SOCKET_CONFIGURATION_FUNCTION
 * @{ 
 */
/**
 * @brief SiWx91x Socket Configuration
 *
 * The structure defines the configuration parameters for SiWx91x sockets.
 * It includes settings for the total number of sockets, TCP and UDP socket
 * configurations, and performance-related parameters.
 */
typedef struct {
  uint8_t total_sockets; ///< Total number of sockets (that includes BSD, IoT, Si91x)

  uint8_t total_tcp_sockets; ///< Total number of TCP sockets

  uint8_t total_udp_sockets; ///< Total number of UDP sockets

  uint8_t tcp_tx_only_sockets; ///< Number of TCP sockets intended for transmission (TX)

  uint8_t tcp_rx_only_sockets; ///< Number of TCP sockets intended for reception (RX)

  uint8_t udp_tx_only_sockets; ///< Number of UDP sockets intended for transmission (TX)

  uint8_t udp_rx_only_sockets; ///< Number of UDP sockets intended for reception (RX)

  uint8_t tcp_rx_high_performance_sockets; ///< Total number of high-performance TCP RX sockets

  uint8_t
    tcp_rx_window_size_cap; ///< TCP RX window size cap, scales window size linearly (TCP MSS * TCP_RX_WINDOW_SIZE_CAP)

  uint8_t tcp_rx_window_div_factor; ///< TCP RX window division factor, increases ACK frequency for asynchronous sockets
} sl_si91x_socket_config_t;

/// SiWx91x specific socket type length value
typedef struct {
  uint16_t type;   ///< Socket type
  uint16_t length; ///< Data length
  uint8_t value[]; ///< Data
} sl_si91x_socket_type_length_value_t;

/** @} */

/**
 * @addtogroup SOCKET_CONFIGURATION_FUNCTION
 * @{ 
 */

/**
 * @brief
 *   Configures SiWx91x specific socket settings.
 *
 * @details
 *   This function sets up the socket configuration specific to the SiWx91x.
 *   It must be called before invoking @ref sl_si91x_socket_async.
 *   The configuration includes setting parameters such as socket type,
 *   protocol, and other options specific to the SiWx91x series.
 *
 * @pre Pre-conditions:
 * - Ensure that the necessary initialization for the SiWx91x module has been completed.
 * - This API is called before calling @ref sl_si91x_socket_async.
 *
 * @param[in] socket_config 
 *   Socket configuration of type @ref sl_si91x_socket_config_t.
 *
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details. 
 *
 ******************************************************************************/
sl_status_t sl_si91x_config_socket(sl_si91x_socket_config_t socket_config);

/** @} */

/**
 * A internal function to free the socket.
 * @param socket socket FD which needs to be reset.
 */
void sli_si91x_free_socket(int socket);

/**
 * A internal function to get sl_si91x_socket structure based on socket FD sent
 * @param socket 
 * Socket FD whose structure is required.
 * @param index 
 * Socket FD index number.
 * @return 
 * sl_si91x_socket or NULL in case of invalid FD.
 */
void get_free_socket(sli_si91x_socket_t **socket, int *index);

/**
 * A internal function to get free socket.
 * @param socket_id 
 * Socket ID.
 */
sli_si91x_socket_t *get_si91x_socket(int32_t socket_id);

sl_status_t sli_si91x_add_tls_extension(sli_si91x_tls_extensions_t *socket_tls_extensions,
                                        const sl_si91x_socket_type_length_value_t *tls_extension);

sl_status_t create_and_send_socket_request(int socketIdIndex, int type, const int *backlog);

int sli_si91x_socket(int family, int type, int protocol, sl_si91x_socket_receive_data_callback_t callback);

int sli_si91x_shutdown(int socket, int how);

int sli_si91x_connect(int socket, const struct sockaddr *addr, socklen_t addr_len);

int sli_si91x_bind(int socket, const struct sockaddr *addr, socklen_t addr_len);

int sli_si91x_accept(int socket,
                     struct sockaddr *addr,
                     socklen_t *addr_len,
                     sl_si91x_socket_accept_callback_t callback);

int sli_si91x_select(int nfds,
                     fd_set *readfds,
                     fd_set *writefds,
                     fd_set *exceptfds,
                     const struct timeval *timeout,
                     sl_si91x_socket_select_callback_t callback);

void handle_accept_response(sli_si91x_socket_t *si91x_client_socket, const sl_si91x_rsp_ltcp_est_t *accept_response);

int handle_select_response(const sl_si91x_socket_select_rsp_t *response,
                           fd_set *readfds,
                           fd_set *writefds,
                           fd_set *exception_fd);

uint8_t sli_si91x_socket_identification_function_based_on_socketid(sl_wifi_buffer_t *buffer, void *user_data);

void set_select_callback(sl_si91x_socket_select_callback_t callback);

void sli_si91x_set_accept_callback(sli_si91x_socket_t *server_socket,
                                   sl_si91x_socket_accept_callback_t callback,
                                   int32_t client_socket_id);

void sli_si91x_set_remote_socket_termination_callback(sl_si91x_socket_remote_termination_callback_t callback);

sl_status_t sli_si91x_send_socket_command(sli_si91x_socket_t *socket,
                                          uint32_t command,
                                          const void *data,
                                          uint32_t data_length,
                                          uint32_t wait_period,
                                          sl_wifi_buffer_t **response_buffer);

int sli_si91x_get_socket_id(sl_si91x_packet_t *packet);

/**
 * A internal function to find a socket with the matching ID and not in the exlcuded_state
 * @param socket_id Socket ID
 * @param excluded_state The socket state that the socket must not be
 * @param role Socket role
 */
sli_si91x_socket_t *sli_si91x_get_socket_from_id(int socket_id,
                                                 sli_si91x_bsd_socket_state_t excluded_state,
                                                 int16_t role);

sl_status_t sli_si91x_send_socket_data(sli_si91x_socket_t *si91x_socket,
                                       const sli_si91x_socket_send_request_t *request,
                                       const void *data);
int32_t sli_get_socket_command_from_host_packet(sl_wifi_buffer_t *buffer);

void sli_si91x_set_socket_event(uint32_t event_mask);

sl_status_t sli_si91x_flush_select_request_table(uint16_t error_code);
