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
#ifndef SL_SI91X_SOCKET_SUPPORT_INCLUDED_H
#define SL_SI91X_SOCKET_SUPPORT_INCLUDED_H

#include <string.h>
#include <stdint.h>

#include "socket.h"
#include "sl_status.h"
#include "sl_si91x_protocol_types.h"

/**
 * @addtogroup SI91X_SOCKET_FUNCTIONS
 * @{ 
 */
/// SiWx91x Socket information query
typedef struct {
  uint8_t sock_id[2]; ///< Identifier for the socket.

  uint8_t sock_type[2]; ///< Type of the socket (TCP, UDP, etc.).

  uint8_t source_port[2]; ///< Port number used by the source.

  uint8_t dest_port[2]; ///< Port number used by the destination.

  union {
    uint8_t ipv4_address[4]; ///< IPv4 address of the remote host.

    uint8_t ipv6_address[16]; ///< IPv6 address of the remote host.

  } dest_ip_address; ///< IP address of the destination host.
} sl_si91x_sock_info_query_t;

/// SiWx91x socket information response
typedef struct {

  uint16_t number_of_opened_sockets; ///< Total number of currently opened sockets.

  sl_si91x_sock_info_query_t socket_info
    [SL_SI91X_SOCKET_INFO_RESPONSE_SOCKETS_COUNT]; ///< Array containing information about each opened socket.

} sl_si91x_socket_info_response_t;

/**
 * @brief     Set SiWx91X specific socket options.
 * @param[in]  socket_id      
 * Identifier for the socket.
 * @param[in]  option_level   
 * Level at which the option is defined. One of the values from @ref BSD_SOCKET_OPTION_LEVEL.
 * @param[in]  option_name    
 * Name of the option to be set. Currently, ONLY @ref SO_CERT_INDEX, @ref SO_HIGH_PERFORMANCE_SOCKET, @ref SO_TLS_SNI are supported.
 * @param[in]  option_value   
 * Pointer to the value for the option.
 * @param[in]  option_length  
 * Length of the option value.
 * @return     
 * Returns 0 on success or -1 on error (in which case, errno is set appropriately).
 *
 */
int sl_si91x_set_custom_sync_sockopt(int socket_id,
                                     int option_level,
                                     int option_name,
                                     const void *option_value,
                                     socklen_t option_length);

/**
 * @brief      Get SiWx91X specific socket options
 * @param[in]  socket_id      
 * Identifier for the socket.
 * @param[in]  option_level   
 * Level at which the option is defined. One of the values from @ref BSD_SOCKET_OPTION_LEVEL.
 * @param[in]  option_name    
 * Name of the option to be retrieved. Currently, ONLY @ref SO_CERT_INDEX is supported.
 * @param[in]  option_value   
 * Pointer to the value for the option.
 * @param[in]  option_length  
 * Pointer to the size of the option value.
 * @return     
 * Returns 0 on success, or -1 on error (in which case, errno is set appropriately).
 *
 */
int sl_si91x_get_custom_sync_sockopt(int socket_id,
                                     int option_level,
                                     int option_name,
                                     void *option_value,
                                     socklen_t *option_length);

/**
 * @brief
 * Retrieve information about currently opened sockets from the network stack.
 * @param[out] socket_info_response
 *    Pointer to a sl_si91x_socket_info_response_t structure that will hold the response from the firmware.
 * @return 
 * sl_status_t
 * @note The socket IDs in the response are specific to the firmware and should not be used as file descriptors in socket APIs.
*/
sl_status_t sl_si91x_get_socket_info(sl_si91x_socket_info_response_t *socket_info_response);
/** @} */

#endif //SL_SI91X_SOCKET_SUPPORT_INCLUDED_H
