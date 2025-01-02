#ifndef SL_WEBSOCKET_CLIENT_TYPES_H
#define SL_WEBSOCKET_CLIENT_TYPES_H

#include "sl_status.h"
#include "sl_wifi_types.h"
#include "sl_ip_types.h"
#include "sl_net_constants.h"
#include "cmsis_os2.h"
#include "sl_si91x_socket_types.h"
#include <stdbool.h>
#include <stdint.h>

/******************************************************
 *                    Constants
 ******************************************************/
/**
 *  @addtogroup SERVICE_WEBSOCKET_CLIENT_CONSTANTS 
 *  @{
 */

#define SL_WEBSOCKET_FIN_BIT 0x80 /**< Final frame bit. Not to be configured by the user. */

#define SL_SI91X_WEBSOCKET_MAX_HOST_LENGTH 51 /**< Websocket max host length. Not to be configured by the user. */

#define SL_SI91X_WEBSOCKET_MAX_RESOURCE_LENGTH \
  51 /**< Websocket max resource length. Not to be configured by the user. */

/******************************************************
 *                   Enumerations
 ******************************************************/

/**
 * @brief WebSocket opcodes for different frame types.
 * 
 * @details This enumeration defines the opcodes used in WebSocket frames to indicate the type of frame being sent or received.
 */
typedef enum {
  SL_WEBSOCKET_OPCODE_CONTINUE = 0x0, /**< Continuation frame */
  SL_WEBSOCKET_OPCODE_TEXT     = 0x1, /**< Text frame */
  SL_WEBSOCKET_OPCODE_BINARY   = 0x2, /**< Binary frame */
  SL_WEBSOCKET_OPCODE_CLOSE    = 0x8, /**< Connection close frame */
  SL_WEBSOCKET_OPCODE_PING     = 0x9, /**< Ping frame */
  SL_WEBSOCKET_OPCODE_PONG     = 0xA, /**< Pong frame */
} sl_websocket_opcode_t;

/**
 * @brief Error codes for WebSocket operations.
 * 
 * @details This enumeration defines the error codes that can be returned by WebSocket operations to indicate the result of the operation.
 */
typedef enum {
  SL_WEBSOCKET_SUCCESS               = 0,  /**< Operation successful */
  SL_WEBSOCKET_ERR_SOCKET_CREATION   = -1, /**< Error creating socket */
  SL_WEBSOCKET_ERR_SOCKET_BIND       = -2, /**< Error binding socket */
  SL_WEBSOCKET_ERR_SOCKET_CONNECT    = -3, /**< Error connecting socket */
  SL_WEBSOCKET_ERR_SEND_FRAME        = -4, /**< Error sending frame */
  SL_WEBSOCKET_ERR_RECEIVE_FRAME     = -5, /**< Error receiving frame */
  SL_WEBSOCKET_ERR_CLOSE_FRAME       = -6, /**< Error closing frame */
  SL_WEBSOCKET_ERR_SSL_SETSOCKOPT    = -7, /**< Error setting socket options for SSL */
  SL_WEBSOCKET_ERR_INVALID_PARAMETER = -8  /**< Invalid input parameter */
} sl_websocket_error_t;

/**
 * @brief WebSocket connection states.
 * 
 * @details This enumeration defines the possible states of a WebSocket connection.
 */
typedef enum {
  SL_WEBSOCKET_STATE_DISCONNECTED, /**< The WebSocket client is not connected to the server. This is the initial state before any connection attempt is made or after a connection has been closed or failed. */
  SL_WEBSOCKET_STATE_CONNECTING, /**< The WebSocket client is in the process of establishing a connection to the server. This state is set when the connection process begins. */
  SL_WEBSOCKET_STATE_CONNECTED, /**< The WebSocket client is successfully connected to the server. This state indicates that the client can now send and receive WebSocket frames. */
  SL_WEBSOCKET_STATE_CLOSING, /**< The WebSocket client is in the process of closing the connection. This state is set when the client initiates a close operation. */
  SL_WEBSOCKET_STATE_CLOSED /**< The WebSocket connection has been closed. This state indicates that the client is no longer connected to the server. */
} sl_websocket_state_t;

/** @} */
/******************************************************
 *                   Type Definitions
 ******************************************************/
// Forward declaration of the type
typedef struct sl_websocket_client_s sl_websocket_client_t;

/******************************************************
 *                    Structures
 ******************************************************/

/**
 * @addtogroup SERVICE_WEBSOCKET_CLIENT_TYPES
 * @{
 */

/**
 * @brief WebSocket client configuration structure.
 * 
 * @details This structure holds the configuration parameters for initializing a WebSocket client.
 */
typedef struct {
  char *host;                                      /**< WebSocket server host (for example, "example.com"). */
  char *resource;                                  /**< WebSocket resource path (for example, "/chat"). */
  uint16_t server_port;                            /**< WebSocket server port number. */
  uint16_t client_port;                            /**< Local client port number. */
  char *ip_address;                                /**< WebSocket server IP address. */
  sl_si91x_socket_receive_data_callback_t data_cb; /**< Data receive callback function. */
  sl_si91x_socket_remote_termination_callback_t
    remote_terminate_cb; /**< Callback function for remote termination event. */
  bool enable_ssl;       /**< Enable SSL for WebSocket connection. */
} sl_websocket_config_t;

/**
 * @brief WebSocket client structure.
 * 
 * @details This structure holds the state and configuration of a WebSocket client, including socket descriptors, connection state, and callback functions. This is not a user configurable structure.
 */
typedef struct sl_websocket_client_s {
  int socket_fd;                                         /**< BSD socket file descriptor. */
  char host[SL_SI91X_WEBSOCKET_MAX_HOST_LENGTH];         /**< WebSocket server host (for example, "example.com"). */
  char resource[SL_SI91X_WEBSOCKET_MAX_RESOURCE_LENGTH]; /**< WebSocket resource path (for example, "/chat"). */
  uint16_t server_port;                                  /**< WebSocket server port number. */
  uint16_t client_port;                                  /**< Local client port number. */
  sl_ip_address_t ip_address;                            /**< WebSocket server IP address. */
  uint8_t mask_key[4];        /**< Masking key for client-to-server frames (used in hosted mode). */
  sl_websocket_state_t state; /**< WebSocket connection state. */
  sl_si91x_socket_receive_data_callback_t data_cb; /**< Data receive callback function. */
  sl_si91x_socket_remote_termination_callback_t
    remote_terminate_cb; /**< Callback function for remote termination event. */
  bool enable_ssl;       /**< Enable SSL for WebSocket connection. */
  void *user_context;    /**< User-defined context (for future reference). */
} sl_websocket_client_t;

/**
 * @brief WebSocket send request structure.
 * 
 * @details This structure holds the parameters for sending a WebSocket frame, including the opcode, payload buffer, and payload length.
 */
typedef struct {
  sl_websocket_opcode_t opcode; /**< Opcode (TEXT, BINARY, and so on.). */
  const uint8_t *buffer;        /**< Pointer to the payload. */
  size_t length;                /**< Length of the payload. */
} sl_websocket_send_request_t;

/** @} */

#endif //SL_WEBSOCKET_CLIENT_TYPES_H
