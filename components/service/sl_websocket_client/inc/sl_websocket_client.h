#ifndef SL_WEBSOCKET_CLIENT_H
#define SL_WEBSOCKET_CLIENT_H

#include <stdbool.h>
#include <stdint.h>
#include "cmsis_os2.h"
#include "sl_status.h"
#include "sl_wifi_types.h"
#include "sl_ip_types.h"
#include "sl_net_constants.h"
#include "sl_si91x_socket_types.h"
#include "sl_websocket_client_types.h"

/******************************************************
 *               Function Declarations
 ******************************************************/

/**
 * @addtogroup SERVICE_WEBSOCKET_CLIENT_FUNCTIONS
 * @{
 */

/***************************************************************************/ /**
 * @brief Initialize the WebSocket client.
 *
 * @details
 *   This function initializes the WebSocket client with the provided configuration.
 *   It allocates necessary resources and sets up the client for connection.
 *
 * @param[in] handle
 *   Pointer to the WebSocket client structure. Must not be NULL.
 *
 * @param[in] config
 *   Pointer to the WebSocket client configuration structure. Must not be NULL.
 *
 * @return
 *   sl_websocket_error_t - Error code indicating the result of the operation.
 */
sl_websocket_error_t sl_websocket_init(sl_websocket_client_t *handle, const sl_websocket_config_t *config);

/***************************************************************************/ /**
 * @brief Connect to a WebSocket server.
 *
 * @details
 *   This function creates a socket, binds it, and connects to the specified WebSocket server.
 *
 * @param[in] handle
 *   Pointer to the WebSocket client structure. Must not be NULL.
 *
 * @return
 *   sl_websocket_error_t - Error code indicating the result of the operation.
 */
sl_websocket_error_t sl_websocket_connect(sl_websocket_client_t *handle);

/***************************************************************************/ /**
 * @brief Send a WebSocket frame.
 *
 * @details
 *   This function sends a WebSocket frame to the server. Masking is taken care of by the firmware.
 *
 * @param[in] handle
 *   Pointer to the WebSocket client structure. Must not be NULL.
 *
 * @param[in] send_request
 *   Pointer to the send request structure containing the frame details. Must not be NULL.
 *
 * @return
 *   sl_websocket_error_t - Error code indicating the result of the operation.
 */
sl_websocket_error_t sl_websocket_send_frame(sl_websocket_client_t *handle,
                                             const sl_websocket_send_request_t *send_request);

/***************************************************************************/ /**
 * @brief Close the WebSocket connection.
 *
 * @details
 *   This function closes the WebSocket connection and cleans up resources.
 *
 * @param[in] handle
 *   Pointer to the WebSocket client structure. Must not be NULL.
 *
 * @return
 *   sl_websocket_error_t - Error code indicating the result of the operation.
 */
sl_websocket_error_t sl_websocket_close(sl_websocket_client_t *handle);

/***************************************************************************/ /**
 * @brief Deinitialize the WebSocket client.
 *
 * @details
 *   This function deinitializes the WebSocket client by freeing allocated resources and resetting the state.
 *   It should be called only after the WebSocket connection has been closed.
 *
 * @param[in] handle
 *   Pointer to the WebSocket client structure. Must not be NULL.
 *
 * @return
 *   sl_websocket_error_t - Error code indicating the result of the operation.
 */
sl_websocket_error_t sl_websocket_deinit(sl_websocket_client_t *handle);

/***************************************************************************/ /**
 * @brief Extracts the WebSocket opcode from a given socket ID.
 *
 * @details
 *   This function determines the opcode of the WebSocket from the socket ID.
 *
 * @param[in] socket_id
 *   The 16-bit socket ID from which to extract the opcode.
 *
 * @return
 *   sl_websocket_opcode_t - The extracted WebSocket opcode.
 */
sl_websocket_opcode_t sl_si91x_get_opcode_from_socket_id(uint16_t socket_id);
/** @} */

#endif //SL_WEBSOCKET_CLIENT_H