/***************************************************************************/ /**
 * @file
 * @brief SL Network API
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_status.h"
#include "sl_net_types.h"
#include "sl_net_wifi_types.h"

/**
 * @brief Register an event handler for network events.
 *
 * @param[in] function Pointer to the event handler function to register.
 * @return Status of the operation.
 *         - SL_STATUS_OK: Event handler registered successfully.
 *         - SL_STATUS_FAIL: Failed to register the event handler.
 */
sl_status_t sli_net_register_event_handler(sl_net_event_handler_t function);

/**
 * @brief Set a network credential.
 *
 * @param[in] id Credential ID to identify the credential.
 * @param[in] type Type of the credential (for example, password or certificate).
 * @param[in] credential Pointer to the credential data.
 * @param[in] credential_length Length of the credential data in bytes.
 * @return Status of the operation.
 *         - SL_STATUS_OK: Credential set successfully.
 *         - SL_STATUS_FAIL: Failed to set the credential.
 */
sl_status_t sli_net_set_credential(sl_net_credential_id_t id,
                                   sl_net_credential_type_t type,
                                   const void *credential,
                                   uint32_t credential_length);

/**
 * @brief Retrieve a network credential.
 *
 * @param[in] id Credential ID to identify the credential.
 * @param[out] type Pointer to store the type of the credential (for example, password or certificate).
 * @param[out] credential Pointer to store the retrieved credential data.
 * @param[out] credential_length Pointer to store the length of the retrieved credential data in bytes.
 * @return Status of the operation.
 *         - SL_STATUS_OK: Credential retrieved successfully.
 *         - SL_STATUS_FAIL: Failed to retrieve the credential.
 *         - SL_STATUS_INVALID_PARAMETER: Invalid input parameters.
 */
sl_status_t sli_net_get_credential(sl_net_credential_id_t id,
                                   const sl_net_credential_type_t *type,
                                   const void *credential,
                                   const uint32_t *credential_length);

/**
 * @brief Delete a network credential.
 *
 * @param[in] id Credential ID to identify the credential to delete.
 * @param[in] type Type of the credential to delete (for example, password or certificate).
 * @return Status of the operation.
 *         - SL_STATUS_OK: Credential deleted successfully.
 *         - SL_STATUS_FAIL: Failed to delete the credential.
 *         - SL_STATUS_INVALID_PARAMETER: Invalid input parameters.
 */
sl_status_t sli_net_delete_credential(sl_net_credential_id_t id, sl_net_credential_type_t type);

/**
 * @brief Handles the auto-join process for a specified network interface.
 *
 * @param[in] interface The network interface to use for the auto-join process.
 * @param[out] profile Pointer to the Wi-Fi client profile to be used during the auto-join process.
 * @return Status of the operation.
 *         - SL_STATUS_OK: Auto-join process completed successfully.
 *         - SL_STATUS_FAIL: Auto-join process failed.
 */
sl_status_t sli_handle_auto_join(sl_net_interface_t interface, sl_net_wifi_client_profile_t *profile);

/**
 * @brief Cleans up resources allocated for auto-join functionality.
 *
 * @details
 * This function frees all resources allocated for the auto-join process, including:
 * - Terminating the network manager thread
 * - Deleting the network manager message queue
 * - Deleting the auto-join event flags
 */
void sli_cleanup_auto_join(void);

/**
 * @brief Event handler for network manager events.
 *
 * @param[in] arg Pointer to the argument passed to the event handler (can be NULL).
 * 
 * @details
 * This function processes network manager events, such as connection, disconnection, 
 * and failure events. It handles auto-join processes and notifies the user of the 
 * network status through registered event handlers.
 */
void sli_network_manager_event_handler(const void *arg);
