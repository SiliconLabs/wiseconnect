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

#include "socket.h"

/**
 * @brief      Set SI91X specific socket options like:
 *             - SO_CERT_INDEX
 *             - SO_HIGH_PERFORMANCE_SOCKET
 *             - SO_TLS_SNI
 * @param[in]  socket_id      - Target Socket id.
 * @param[in]  option_level   - Option level.
 * @param[in]  option_name    - Option to be configured.
 * @param[in]  option_value   - Pointer to the buffer containing the value.
 * @param[in]  option_length  - Option value buffer length.
 * @return       int. 0 on success, -1 on failure with errno set to appropriate value.
 *
 */
int sl_si91x_set_custom_sync_sockopt(int socket_id,
                                     int option_level,
                                     int option_name,
                                     const void *option_value,
                                     socklen_t option_length);

/**
 * @brief      Get SI91X specific socket options like:
 *             - SO_CERT_INDEX
 * @param[in]  socket_id      - Target Socket id.
 * @param[in]  option_level   - Option level.
 * @param[in]  option_name    - Option to be configured.
 * @param[in]  option_value   - Pointer to the buffer to store the value.
 * @param[in]  option_length  - Option value buffer length.
 * @return       int. 0 on success, -1 on failure with errno set to appropriate value.
 *
 */
int sl_si91x_get_custom_sync_sockopt(int socket_id,
                                     int option_level,
                                     int option_name,
                                     void *option_value,
                                     socklen_t *option_length);