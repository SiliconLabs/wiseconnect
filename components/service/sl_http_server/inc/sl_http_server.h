/*******************************************************************************
 * # License
 * Copyright 2024 Silicon Laboratories Inc. www.silabs.com
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
#ifndef SL_HTTP_SERVER_H
#define SL_HTTP_SERVER_H

#include <stdbool.h>
#include <stdint.h>
#include "cmsis_os2.h"
#include "sl_status.h"
#include "sl_wifi_types.h"
#include "sl_ip_types.h"
#include "sl_net_constants.h"
#include "sl_si91x_socket_types.h"
#include "sl_http_server_types.h"

/******************************************************
 *               Function Declarations
 ******************************************************/

/**
 * @addtogroup SERVICE_HTTP_SERVER_FUNCTIONS
 * @{
 */

/***************************************************************************/ /**
 * @brief HTTP server initialization function.
 *
 * This function initializes an HTTP server by taking in the configuration structure and the pointer to the HTTP server handle.
 * All the APIs must be used only after this API using the HTTP server handle returned in the "handle" parameter.
 *
 * @param[out] handle
 *   @ref sl_http_server_t object which will be initialized with the server handle.
 * @param[in] config
 *   HTTP server configuration of type @ref sl_http_server_config_t.
 *
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 */
sl_status_t sl_http_server_init(sl_http_server_t *handle, const sl_http_server_config_t *config);

/***************************************************************************/ /**
 * @brief
 *   Deinitializes the HTTP server and frees all the resources related to the HTTP server handle.
 * @pre Pre-conditions:
 * - The HTTP server handle should be initialized using @ref sl_http_server_init before calling this API.
 * @param[in] handle
 *   HTTP server handle of type @ref sl_http_server_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 *   After calling this API, the parameter "handle" should not be used in any other API without calling sl_http_server_init() again.
 */
sl_status_t sl_http_server_deinit(sl_http_server_t *handle);

/***************************************************************************/ /**
 * @brief
 *   Spawns a new thread and starts accepting requests on the configured HTTP port.
 *   The HTTP request handlers are invoked from this thread.
 * @pre Pre-conditions:
 * - The HTTP server handle should be initialized using @ref sl_http_server_init before calling this API.
 * @param[in] handle
 *   HTTP server handle of type @ref sl_http_server_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 */
sl_status_t sl_http_server_start(sl_http_server_t *handle);

/***************************************************************************/ /**
 * @brief
 *   Stops the HTTP server thread.
 * @pre Pre-conditions:
 * - The HTTP server handle should be initialized using @ref sl_http_server_init before calling this API.
 * @param[in] handle
 *   HTTP server handle of type @ref sl_http_server_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 */
sl_status_t sl_http_server_stop(sl_http_server_t *handle);

/***************************************************************************/ /**
 * @brief
 *   Retrieves the headers from an HTTP request.
 *   This function extracts all headers from a given HTTP request and stores them in the provided headers array.
 * @param[in] handle
 *   HTTP server handle of type @ref sl_http_server_t.
 * @param[in] request
 *   HTTP request from which headers are to be retrieved of type @ref sl_http_server_request_t.
 * @param[out] headers
 *   Array where the retrieved headers will be stored of type @ref sl_http_header_t.
 * @param[in] header_count
 *   Number of headers that the array can hold.
 *
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 */
sl_status_t sl_http_server_get_request_headers(sl_http_server_t *handle,
                                               sl_http_server_request_t *request,
                                               sl_http_header_t *headers,
                                               uint16_t header_count);

/***************************************************************************/ /**
 * @brief
 *   Reads the HTTP request data being received after the headers.
 * @pre Pre-conditions:
 * - This API can only be called if req_data_length parameter in @ref sl_http_server_request_t is greater than 0.
 * - This API can only be called from with in the request handler of corresponding request.
 * @param[in] handle
 *   HTTP server handle of type @ref sl_http_server_t
 * @param[in] data
 *   Pointer to the receive parameters structure of type @ref sl_http_recv_req_data_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 */
sl_status_t sl_http_server_read_request_data(sl_http_server_t *handle, sl_http_recv_req_data_t *data);

/***************************************************************************/ /**
 * @brief
 *   Makes the HTTP server send the response to the current request.
 *   This API can only be called once per request.
 * @pre Pre-conditions:
 * - The HTTP server handle should be initialized using @ref sl_http_server_init before calling this API.
 * @param[in] handle
 *   HTTP server handle of type @ref sl_http_server_t
 * @param[in] response
 *   Pointer to the response of type @ref sl_http_server_response_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 */
sl_status_t sl_http_server_send_response(sl_http_server_t *handle, sl_http_server_response_t *response);

/***************************************************************************/ /**
 * @brief HTTP server data writing function.
 *
 * This function sends a chunk of data as part of the HTTP response. It should be used after a response has been started with 
 * sl_http_server_send_response, and can be called multiple times to send the response data in chunks.
 *
 * @param[in] handle
 *   HTTP server handle of type @ref sl_http_server_t.
 * @param[in] data
 *   Pointer to the data to be sent.
 * @param[in] data_length
 *   Length of the data to be sent.
 *
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 */
sl_status_t sl_http_server_write_data(sl_http_server_t *handle, uint8_t *data, uint32_t data_length);
/** @} */

#endif //SL_HTTP_SERVER_H
