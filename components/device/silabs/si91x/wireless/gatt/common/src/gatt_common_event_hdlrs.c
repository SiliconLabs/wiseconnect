/*******************************************************************************
* @file  gatt_common_event_hdlrs.c
* @brief Common GATT Event Handlers - Shared by Server and Client
* 
* This module contains GATT event handlers that are common to both GATT Server
* and GATT Client roles. These handlers manage shared functionality like buffer
* management that applies to both roles.
* 
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

/*=======================================================================*/
//   ! INCLUDES
/*=======================================================================*/

// Component Headers (2 headers, 23 entities)
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
#include "gap.h" // GAP structures and APIs (23 entities)
// Types (4): rsi_ble_conn_info_t, ble_confg_info_t, more_data_state_beta_t, rsi_ble_event_le_dev_buf_ind_t
// Constants (2): TOTAL_CONNECTIONS, RSI_REM_DEV_ADDR_LEN
// Config macros (3): CONNECT_OPTION, CONN_BY_NAME, RSI_DEBUG_EN
// Debug macro (1): LOG_PRINT_D
// Functions (2): rsi_6byte_dev_address_to_ascii(), rsi_get_ble_conn_id()
// Extern vars (4): rsi_ble_conn_info[], remote_dev_addr_conn, ble_confgs, more_data_state_beta[]
// Struct fields (7): tx_notifications, tx_indications, tx_write, tx_write_no_response,
//                    smp_enable, data_transmit, remote_dev_bd_addr

#include "gatt_common.h" // GATT Common - Module's own header (must be included)
                         // Not directly used but part of module's public interface

// BLE Event Handler Infrastructure (1 header)
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
#include "ble_event_hdlr_auto_gen.h" // Event handler registration (not directly visible but needed for event system)

// BLE SDK Headers (2 headers)
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
#include "rsi_ble_apis.h"    // BLE API functions (rsi_ble_conn_params_update)
#include "sl_si91x_status.h" // Status codes (RSI_SUCCESS, RSI_ERROR_BLE_ATT_CMD_IN_PROGRESS)

// Utility Headers (2 headers)
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
#include "rsi_utils.h" // RSI utilities (UNUSED_PARAMETER, LOG_PRINT_D)
#include <stdio.h>     // Standard I/O (printf)
#include <inttypes.h>

// GAP-owned variables (defined in gap_user_event_hdlrs.c)
extern rsi_ble_conn_info_t rsi_ble_conn_info[TOTAL_CONNECTIONS];
extern uint8_t remote_dev_addr_conn[RSI_REM_DEV_ADDR_LEN];
extern ble_confg_info_t ble_confgs;
extern more_data_state_beta_t more_data_state_beta[TOTAL_CONNECTIONS];

/*==============================================*/
/**
 * @fn          void rsi_conn_update_req_event(uint8_t conn_id)
 * @brief       Common handler for connection update request - shared by server and client
 * @param[in]   uint8_t conn_id - Connection ID
 * @return      None
 *
 * @section description
 * This handler performs connection parameter updates and manages data receive/transmit
 * events for both GATT Server and GATT Client roles.
 */
void rsi_conn_update_req_event(uint8_t conn_id)
{
  int32_t status;
  uint8_t ble_conn_id = conn_id;
  if (!rsi_ble_conn_info[ble_conn_id].conn_param_req_given) {
#if RSI_DEBUG_EN
    LOG_PRINT_D("\r\n in connection update req event -conn%d \r\n", ble_conn_id);
#endif
    //! update connection interval to 45ms, latency 0, supervision timeout 4s
    status =
      rsi_ble_conn_params_update(rsi_ble_conn_info[ble_conn_id].rsi_connected_dev_addr,
                                 ble_confgs.ble_conn_configuration[ble_conn_id].conn_param_update.conn_int,
                                 ble_confgs.ble_conn_configuration[ble_conn_id].conn_param_update.conn_int,
                                 ble_confgs.ble_conn_configuration[ble_conn_id].conn_param_update.conn_latncy,
                                 ble_confgs.ble_conn_configuration[ble_conn_id].conn_param_update.supervision_to);
    if (status != RSI_SUCCESS) {
      //! check for procedure already in progress error
      if (status == RSI_ERROR_BLE_ATT_CMD_IN_PROGRESS) {
        printf("\r\n rsi_ble_conn_params_update procedure is already in progress -conn%d \r\n", ble_conn_id);
      } else {
        printf("\r\n failed to update connection paramaters error:0x%" PRIX32 " -conn%d \r\n", status, ble_conn_id);
      }
    } else {
      printf("\r\n connection params request was successfull -conn%d \n", ble_conn_id);
      rsi_ble_conn_info[ble_conn_id].conn_param_req_given = true;
    }
  }

  //! set RSI_DATA_RECEIVE_EVENT to receive data from remote device (Client operation)
#ifdef SL_SI91X_BLE_GATT_CLIENT_COMPONENT
  if (rsi_ble_conn_info[ble_conn_id].rsi_rx_from_rem_dev) {
    rsi_ble_conn_info[ble_conn_id].rsi_rx_from_rem_dev = false;
    printf("\r\n start receiving data from - conn%d\r\n", ble_conn_id);

    rsi_ble_event_on_data_receive_driver_callback(ble_conn_id);
  }
#endif

  //! set RSI_DATA_TRANSMIT_EVENT to transmit data to remote device (Server operation)
#ifdef SL_SI91X_BLE_GATT_SERVER_COMPONENT
  if (rsi_ble_conn_info[ble_conn_id].rsi_tx_to_rem_dev) {
    rsi_ble_conn_info[ble_conn_id].rsi_tx_to_rem_dev = false;
    printf("\r\n start transmitting data to - conn%d\r\n", ble_conn_id);

    rsi_ble_event_data_transmit_server_driver_callback(ble_conn_id);
  }
#endif

  return;
}

/*==============================================*/
/**
 * @fn          void rsi_ble_gatt_server_data_transmit(uint8_t ble_conn_id)
 * @brief       Weak default implementation for GATT server data transmit
 * @param[in]   uint8_t ble_conn_id - Connection ID
 * @return      None
 * @note        This is overridden by the actual implementation in gatt_server module
 */
__attribute__((weak)) void rsi_ble_gatt_server_data_transmit(uint8_t ble_conn_id)
{
  UNUSED_PARAMETER(ble_conn_id);
  // Default empty implementation - overridden by server module
}

/*==============================================*/
/**
 * @fn          void rsi_ble_gatt_client_data_transmit(uint8_t ble_conn_id)
 * @brief       Weak default implementation for GATT client data transmit
 * @param[in]   uint8_t ble_conn_id - Connection ID
 * @return      None
 * @note        This is overridden by the actual implementation in gatt_client module
 */
__attribute__((weak)) void rsi_ble_gatt_client_data_transmit(uint8_t ble_conn_id)
{
  UNUSED_PARAMETER(ble_conn_id);
  // Default empty implementation - overridden by client module
}

/*==============================================*/
/**
 * @fn          void rsi_ble_event_more_data_req(uint16_t status, void *event_data)
 * @brief       Common handler for buffer availability event - shared by server and client
 * @param[in]   uint16_t status - Event status
 * @param[in]   void *event_data - Event data (rsi_ble_event_le_dev_buf_ind_t)
 * @return      None
 *
 * @section description
 * This handler is called when the firmware indicates that TX buffer space is available.
 * It retries previously failed data transmission operations for both:
 * - GATT Server: Notifications and Indications
 * - GATT Client: Write with Response and Write without Response
 *
 * This event is triggered by RSI_BLE_EVENT_LE_DEV_BUF_IND from the firmware.
 */
void rsi_ble_event_more_data_req(uint16_t __attribute__((unused)) status, void *event_data)
{
  uint8_t ble_conn_id;
  rsi_ble_event_le_dev_buf_ind_t *rsi_ble_more_data_evt = (rsi_ble_event_le_dev_buf_ind_t *)event_data;

  //! Convert device address to ASCII
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, rsi_ble_more_data_evt->remote_dev_bd_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! Get connection ID
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif

#if RSI_DEBUG_EN
  LOG_PRINT_D("\r\n Received more data event (buffer available) -conn%d\r\n", ble_conn_id);
#endif

  //! Check if there's pending data transmission that needs retry
  if (more_data_state_beta[ble_conn_id].data_transmit) {
    more_data_state_beta[ble_conn_id].data_transmit = 0;

    //! Retry transmission based on role:
    //! - Server: Retry notifications/indications (tx_notifications, tx_indications)
    //! - Client: Retry write operations (tx_write, tx_write_no_response)
    if (ble_confgs.ble_conn_configuration[ble_conn_id].tx_notifications
        || ble_confgs.ble_conn_configuration[ble_conn_id].tx_indications) {
      rsi_ble_gatt_server_data_transmit(ble_conn_id);
    }

    if (ble_confgs.ble_conn_configuration[ble_conn_id].tx_write
        || ble_confgs.ble_conn_configuration[ble_conn_id].tx_write_no_response) {
      rsi_ble_gatt_client_data_transmit(ble_conn_id);
    }
  }
}

/*==============================================*/
/**
 * @fn          void rsi_ble_server_handle_gatt_write(uint16_t status, void *event_data)
 * @brief       Weak default implementation for GATT Server write handler
 * @param[in]   uint16_t status - Event status
 * @param[in]   void *event_data - Event data (rsi_ble_event_write_t)
 * @return      None
 * @note        This is overridden by the actual implementation in gatt_server module
 *              Handles server-side write operations:
 *              - Write requests to local attributes
 *              - CCCD changes for TX notifications/indications
 */
__attribute__((weak)) void rsi_ble_server_handle_gatt_write(uint16_t status, void *event_data)
{
  UNUSED_PARAMETER(status);
  UNUSED_PARAMETER(event_data);
  // Default empty implementation - overridden by server module
}

/*==============================================*/
/**
 * @fn          void rsi_ble_client_handle_gatt_write(uint16_t status, void *event_data)
 * @brief       Weak default implementation for GATT Client write handler
 * @param[in]   uint16_t status - Event status
 * @param[in]   void *event_data - Event data (rsi_ble_event_write_t)
 * @return      None
 * @note        This is overridden by the actual implementation in gatt_client module
 *              Handles client-side write operations:
 *              - RX indications from remote server (sends confirmation)
 *              - RX notifications from remote server
 */
__attribute__((weak)) void rsi_ble_client_handle_gatt_write(uint16_t status, void *event_data)
{
  UNUSED_PARAMETER(status);
  UNUSED_PARAMETER(event_data);
  // Default empty implementation - overridden by client module
}

/*==============================================*/
/**
 * @fn          void rsi_ble_event_gatt_write(uint16_t status, void *event_data)
 * @brief       Common dispatcher for GATT write events - shared by server and client
 * @param[in]   uint16_t status - Event status
 * @param[in]   void *event_data - Event data (rsi_ble_event_write_t)
 * @return      None
 *
 * @section description
 * This dispatcher is called when a GATT write event is received from the firmware.
 * It dispatches to both server and client handlers:
 * - Server handler: Processes writes to local attributes, CCCD changes for TX
 * - Client handler: Processes RX indications and notifications from remote server
 *
 * This event is triggered by RSI_BLE_EVENT_GATT_WRITE from the firmware.
 */
void rsi_ble_event_gatt_write(uint16_t status, void *event_data)
{
  // Dispatch to server handler (if server component present)
  // Handles: write requests, write without response, CCCD changes for TX notifications/indications
  rsi_ble_server_handle_gatt_write(status, event_data);

  // Dispatch to client handler (if client component present)
  // Handles: RX indications and notifications from remote GATT server
  rsi_ble_client_handle_gatt_write(status, event_data);
}
