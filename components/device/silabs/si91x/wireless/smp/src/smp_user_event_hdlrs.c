/*******************************************************************************
* @file  smp_user_event_hdlrs.c
* @brief 
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
/**
 * @file         smp_user_event_hdlrs.c
 * @version      0.1
 * @date         01 FEB 2021*
 *
 *
 *  @brief : This file contains user event handlers for SMP protocol
 *
 *  @section Description  This file contains user event handlers for SMP protocol
 */

/*=======================================================================*/
//   INCLUDES
/*=======================================================================*/

// Standard C library
#include <stdio.h>  // printf() - debug and error messages (64 uses)
#include <string.h> // memcpy() - memory operations (used throughout)
#include <inttypes.h>

// SMP Component headers
#include <smp_config.h> // SMP configuration - resolved via -I: config/ble_config/ (user override) first, then component inc/
#include "smp.h"        // SMP types and API (includes gap.h for types and config)

// BLE Event and Callback Infrastructure
#include "ble_event_hdlr_auto_gen.h" // Auto-generated event handler registration

// BLE Unified Framework utilities
#include "rsi_common_utils.h" // rsi_6byte_dev_address_to_ascii(), rsi_get_ble_conn_id() (35 uses)

// BLE SDK APIs
#include "rsi_ble.h" // rsi_ble_smp_pair_request(), rsi_ble_smp_pair_response(),
                     // rsi_ble_smp_passkey(), rsi_ble_disconnect(), rsi_ble_start_encryption(),
                     // rsi_ble_ltk_req_reply(), rsi_ble_get_profiles_async() (16 BLE API calls)

// BLE SDK Configuration (fallback if not defined in smp_config.h)
#include "rsi_ble_common_config.h" // RSI_DEBUG_EN, UNUSED_PARAMETER macro fallbacks

// Status and Constants
#include "sl_si91x_status.h" // RSI_SUCCESS, RSI_ERROR_BLE_* status codes (12 uses)
#include "sl_constants.h"    // General constants

// Common APIs
#include "rsi_common_apis.h" // RSI_FAILURE, RSI_SUCCESS, common utility APIs

// Utils
#include "rsi_utils.h" // Utility macros and functions

extern rsi_ble_conn_info_t rsi_ble_conn_info[TOTAL_CONNECTIONS];

// External variable declarations for shared variables (defined in GAP or other files)
extern uint8_t remote_dev_addr_conn[RSI_REM_DEV_ADDR_LEN];
extern ble_confg_info_t ble_confgs;

// SMP-owned variable definitions (moved from monolithic file)
uint8_t smp_in_progress                                    = 0;     // SMP-owned: Overrides weak definition in GAP
rsi_ble_dev_ltk_list_t ble_dev_ltk_list[TOTAL_CONNECTIONS] = { 0 }; // LTK key storage for SMP

// SMP-specific variable declarations (defined in this file)
static rsi_bt_event_le_security_keys_t temp_le_sec_keys;
static rsi_bt_event_encryption_enabled_t glbl_enc_enabled;
static rsi_ble_event_ctkd_t ble_ctkd;

// Helper functions moved to smp_utilities.c:
// - add_security_keys_to_device_list()
// - add_device_to_ltk_key_list()
// - add_derived_key_to_ltk_list()

/*==============================================*/
/**
 * @fn          void rsi_ble_event_smp_req(uint16_t status, void *event_data)
 * @brief       handler for event_smp_req to be executed in ble task context
 * @param[in]   uint16_t , event_status
 * @param[in]   void, event_data
 * @return      None
 *
 * @section description
 * handler for event_smp_req to be executed in ble task context
 *
 */
void rsi_ble_event_smp_req(uint16_t status, void *event_data)
{
  uint8_t ble_conn_id;
  rsi_bt_event_smp_req_t *remote_smp = (rsi_bt_event_smp_req_t *)event_data;
  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, remote_smp->dev_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif

  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].rsi_ble_event_smp_req, remote_smp, sizeof(rsi_bt_event_smp_req_t));
  printf("\r\n in smp request \r\n -conn%d \r\n", ble_conn_id);
  rsi_ble_conn_info[ble_conn_id].conn_state = on_connect_state;
#ifdef SL_SI91X_BLE_GATT_CLIENT_COMPONENT
  // If GATT Client is present, wait for MTU exchange event before responding to SMP
  // (event_mtu handler in Client component sets mtu_exchange_done flag)
  if (rsi_ble_conn_info[ble_conn_id].mtu_exchange_done)
#endif
  {
    if (ble_confgs.ble_conn_configuration[ble_conn_id].smp_enable) {
      rsi_ble_conn_info[ble_conn_id].smp_pairing_request_received = true;

      if (rsi_ble_conn_info[ble_conn_id].first_connect == 0) {
        if (!rsi_ble_conn_info[ble_conn_id].smp_pairing_initated) {
          //! initiating the SMP pairing process
          status = rsi_ble_smp_pair_request(rsi_ble_conn_info[ble_conn_id].rsi_ble_event_smp_req.dev_addr,
                                            RSI_BLE_SMP_IO_CAPABILITY,
                                            MITM_ENABLE);
          if (status != RSI_SUCCESS) {
            printf("\r\n RSI_BLE_SMP_REQ_EVENT: failed to initiate the SMP pairing process: 0x%x \r\n -conn%d",
                   status,
                   ble_conn_id);
          } else {
            rsi_ble_conn_info[ble_conn_id].smp_pairing_initated = true;
            // rsi_6byte_dev_address_to_ascii(rsi_ble_conn_info[ble_conn_id].str_remote_address, rsi_ble_conn_info[ble_conn_id].rsi_connected_dev_addr);
            printf("\r\n smp pairing request initiated to %s - conn%d \r\n",
                   rsi_ble_conn_info[ble_conn_id].str_remote_address,
                   ble_conn_id);
          }
        }
      } else {
        status = rsi_ble_start_encryption(rsi_ble_conn_info[ble_conn_id].rsi_ble_event_smp_req.dev_addr,
                                          rsi_ble_conn_info[ble_conn_id].rsi_encryption_enabled.localediv,
                                          rsi_ble_conn_info[ble_conn_id].rsi_encryption_enabled.localrand,
                                          rsi_ble_conn_info[ble_conn_id].rsi_encryption_enabled.localltk);
      }
    }
  }
}

/*==============================================*/
/**
 * @fn          void rsi_ble_event_smp_resp(uint16_t status, void *event_data)
 * @brief       handler for event_smp_resp to be executed in ble task context
 * @param[in]   uint16_t , event_status
 * @param[in]   void, event_data
 * @return      None
 *
 * @section description
 * handler for event_smp_resp to be executed in ble task context
 *
 */
void rsi_ble_event_smp_resp(uint16_t __attribute__((unused)) status, void *event_data)
{
  uint8_t ble_conn_id;
  rsi_bt_event_smp_resp_t *remote_smp = (rsi_bt_event_smp_resp_t *)event_data;

  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, remote_smp->dev_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif
  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].rsi_ble_event_smp_resp, remote_smp, sizeof(rsi_bt_event_smp_resp_t));
  rsi_ble_conn_info[ble_conn_id].conn_state = on_connect_state;

  printf("\r\n in smp response -conn%d \r\n", ble_conn_id);
  if (ble_confgs.ble_conn_configuration[ble_conn_id].smp_enable) {
    //! initiating the SMP pairing process
#if RSI_DEBUG_EN
    status = rsi_ble_smp_pair_response(rsi_ble_conn_info[ble_conn_id].rsi_ble_event_smp_resp.dev_addr,
                                       RSI_BLE_SMP_IO_CAPABILITY,
                                       MITM_ENABLE);
#else
    rsi_ble_smp_pair_response(rsi_ble_conn_info[ble_conn_id].rsi_ble_event_smp_resp.dev_addr,
                              RSI_BLE_SMP_IO_CAPABILITY,
                              MITM_ENABLE);
#endif
  }
#if RSI_DEBUG_EN
  LOG_PRINT_D("Status = %x", status);
#endif
}

/*==============================================*/
/**
 * @fn          void rsi_ble_event_smp_passkey(uint16_t status, void *event_data)
 * @brief       handler for event_smp_passkey to be executed in ble task context
 * @param[in]   uint16_t , event_status
 * @param[in]   void, event_data
 * @return      None
 *
 * @section description
 * handler for event_smp_passkey to be executed in ble task context
 *
 */
void rsi_ble_event_smp_passkey(uint16_t __attribute__((unused)) status, void *event_data)
{
  uint8_t ble_conn_id;
  rsi_bt_event_smp_passkey_t *smp_pass_key = (rsi_bt_event_smp_passkey_t *)event_data;
  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, smp_pass_key->dev_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif

  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].rsi_ble_event_smp_passkey, smp_pass_key, sizeof(rsi_bt_event_smp_passkey_t));

  printf("\r\n in smp_passkey - rsi_ble_conn_info[ble_conn_id].str_remote_address : %s\r\n",
         rsi_ble_conn_info[ble_conn_id].remote_dev_addr);
  rsi_ble_conn_info[ble_conn_id].conn_state = on_connect_state;

  //! initiating the SMP pairing process
#if RSI_DEBUG_EN
  status =
    rsi_ble_smp_passkey(rsi_ble_conn_info[ble_conn_id].rsi_ble_event_smp_passkey.dev_addr, RSI_BLE_APP_SMP_PASSKEY);
#else
  rsi_ble_smp_passkey(rsi_ble_conn_info[ble_conn_id].rsi_ble_event_smp_passkey.dev_addr, RSI_BLE_APP_SMP_PASSKEY);
#endif
#if RSI_DEBUG_EN
  LOG_PRINT_D("Status = %x", status);
#endif
}

/*==============================================*/
/**
 * @fn          void rsi_ble_event_smp_failed(uint16_t status, void *event_data)
 * @brief       handler for event_smp_failed to be executed in ble task context
 * @param[in]   uint16_t , event_status
 * @param[in]   void, event_data
 * @return      None
 *
 * @section description
 * handler for event_smp_failed to be executed in ble task context
 *
 */
void rsi_ble_event_smp_failed(uint16_t status, void *event_data)
{
  uint8_t ble_conn_id;
  rsi_bt_event_smp_failed_t *remote_dev_address = (rsi_bt_event_smp_failed_t *)event_data;
  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, remote_dev_address->dev_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif
  smp_in_progress = 0;

  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].rsi_ble_smp_failed, remote_dev_address, sizeof(rsi_bt_event_smp_failed_t));
  rsi_ble_conn_info[ble_conn_id].conn_state = on_connect_state;

  printf("\r\n SMP failed for remote address : %s with status : %x", remote_dev_addr_conn, status);
  printf("\r\n in smp failed remote address: %s -conn%d \r\n",
         rsi_ble_conn_info[ble_conn_id].remote_dev_addr,
         ble_conn_id);

  printf("\r\n Initiating a disconnect as the security failed \r\n ");
  status = rsi_ble_disconnect((int8_t *)rsi_ble_conn_info[ble_conn_id].rsi_ble_smp_failed.dev_addr);
  if (status != RSI_SUCCESS) {
    printf("\ndisconnect command failed with reason %x\n", status);
  }
}

/*==============================================*/
/**
 * @fn          void rsi_ble_event_encryption_enabled(uint16_t status, void *event_data)
 * @brief       handler for event_encryption_enabled to be executed in ble task context
 * @param[in]   uint16_t , event_status
 * @param[in]   void, event_data
 * @return      None
 *
 * @section description
 * handler for event_encryption_enabled to be executed in ble task context
 *
 */
void rsi_ble_event_encryption_enabled(uint16_t status, void *event_data)
{

  uint8_t ble_conn_id;
  uint8_t remote_device_role                     = 0;
  rsi_bt_event_encryption_enabled_t *enc_enabled = (rsi_bt_event_encryption_enabled_t *)event_data;
  // This statement is added only to resolve compilation warning  : [-Wunused-parameter] , value is unchanged
  UNUSED_PARAMETER(status);
  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, enc_enabled->dev_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif

  smp_in_progress = 0;

  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].rsi_encryption_enabled,
         enc_enabled,
         sizeof(rsi_bt_event_encryption_enabled_t));
  memcpy(&rsi_ble_conn_info[ble_conn_id].l_rsi_encryption_enabled,
         &rsi_ble_conn_info[ble_conn_id].rsi_encryption_enabled,
         sizeof(rsi_bt_event_encryption_enabled_t));
  rsi_ble_conn_info[ble_conn_id].smp_pairing_initated = false;
  rsi_ble_conn_info[ble_conn_id].smp_state            = smp_process_done;
  uint8_t ix;

  memcpy(&glbl_enc_enabled, enc_enabled, sizeof(rsi_bt_event_encryption_enabled_t));
  printf("\n ************************** \n");
  printf("\n LE LTK \n");

  for (ix = 0; ix < 16; ix++) {
    printf("%x ", enc_enabled->localltk[ix]);
  }

  printf("\n ************************** \n");
  status = add_device_to_ltk_key_list(ble_dev_ltk_list, &glbl_enc_enabled);
  if (status != RSI_SUCCESS) {
    printf("\n Failed to add LTK to the device");
  }
  rsi_ble_conn_info[ble_conn_id].conn_state = on_connect_state;
  printf("\r\n in smp encrypt event -conn%d \r\n", ble_conn_id);

  remote_device_role = rsi_get_remote_device_role(remote_dev_addr_conn);
  if (remote_device_role == PERIPHERAL_RL) {
    if (ble_confgs.ble_conn_configuration[ble_conn_id].smp_enable) {
      rsi_ble_event_scan_restart_driver_callback();
    }
  }

  LOG_PRINT_D("\n Triggering SMP pending callback\n");

  rsi_ble_event_smp_pending_driver_callback(&ble_conn_id);

  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, rsi_ble_conn_info[ble_conn_id].rsi_connected_dev_addr);

  LOG_PRINT_D("\n address sent from encryption to rpfoile disc RSI CONNECTEED ADDRESS : %s \n", remote_dev_addr_conn);
  if (rsi_ble_conn_info[ble_conn_id].smp_state == smp_process_done) {
    status = rsi_ble_get_profiles_async(rsi_ble_conn_info[ble_conn_id].rsi_connected_dev_addr, 1, 0xffff, NULL);
    if (status != RSI_SUCCESS) {
      //! check for procedure already in progress error
      if (status == (uint16_t)RSI_ERROR_BLE_ATT_CMD_IN_PROGRESS) {

        printf("\r\n rsi_ble_get_profiles_async procedure is already in progress -conn%d \r\n", ble_conn_id);

      }
      //! check for buffer full error, which is not expected for this procedure
      else if (status == (uint16_t)RSI_ERROR_BLE_DEV_BUF_FULL) {
        printf("\r\n rsi_ble_get_profiles_async failed with buffer full error -conn%d \r\n", ble_conn_id);

      } else {
        printf("\r\n get profile async call failed with error code :%x -conn%d \r\n", status, ble_conn_id);
      }
    }
  }
}

/*==============================================*/
/**
 * @fn          void rsi_ble_event_smp_passkey_display(uint16_t status, void *event_data)
 * @brief       handler for event_smp_passkey_display to be executed in ble task context
 * @param[in]   uint16_t , event_status
 * @param[in]   void, event_data
 * @return      None
 *
 * @section description
 * handler for event_smp_passkey_display to be executed in ble task context
 *
 */
void rsi_ble_event_smp_passkey_display(uint16_t __attribute__((unused)) status, void *event_data)
{

  uint8_t ble_conn_id;
  rsi_bt_event_smp_passkey_display_t *smp_passkey_display = (rsi_bt_event_smp_passkey_display_t *)event_data;
  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, smp_passkey_display->dev_addr);

  //! get conn_id
#if (CONNECT_OPTION != CONN_BY_NAME)
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif
  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].rsi_ble_smp_passkey_display,
         smp_passkey_display,
         sizeof(rsi_bt_event_smp_passkey_display_t));
  printf("\r\nremote addr: %s, passkey: %s \r\n",
         rsi_ble_conn_info[ble_conn_id].remote_dev_addr,
         rsi_ble_conn_info[ble_conn_id].rsi_ble_smp_passkey_display.passkey);
  rsi_ble_conn_info[ble_conn_id].conn_state = on_connect_state;
}

/*==============================================*/
/**
 * @fn          void rsi_ble_event_sc_passkey(uint16_t status, void *event_data)
 * @brief       handler for event_sc_passkey to be executed in ble task context
 * @param[in]   uint16_t , event_status
 * @param[in]   void, event_data
 * @return      None
 *
 * @section description
 * handler for event_sc_passkey to be executed in ble task context
 *
 */
void rsi_ble_event_sc_passkey(uint16_t __attribute__((unused)) status, void *event_data)
{

  uint8_t ble_conn_id;
  rsi_bt_event_sc_passkey_t *sc_passkey = (rsi_bt_event_sc_passkey_t *)event_data;
  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, sc_passkey->dev_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif
  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].rsi_event_sc_passkey, sc_passkey, sizeof(rsi_bt_event_sc_passkey_t));
  printf("\r\n in smp sc passkey event -conn%d \r\n", ble_conn_id);

  printf("\r\n In passkey event, remote addr: %s, passkey: %" PRIu32 " -conn%u \r\n",
         rsi_ble_conn_info[ble_conn_id].remote_dev_addr,
         rsi_ble_conn_info[ble_conn_id].rsi_event_sc_passkey.passkey,
         ble_conn_id);

  rsi_ble_smp_passkey(rsi_ble_conn_info[ble_conn_id].rsi_event_sc_passkey.dev_addr,
                      rsi_ble_conn_info[ble_conn_id].rsi_event_sc_passkey.passkey);
  rsi_ble_conn_info[ble_conn_id].conn_state = on_connect_state;
}

/*==============================================*/
/**
 * @fn          void rsi_ble_event_le_ltk_request(uint16_t status, void *event_data)
 * @brief       handler for event_le_ltk_request to be executed in ble task context
 * @param[in]   uint16_t , event_status
 * @param[in]   void, event_data
 * @return      None
 *
 * @section description
 * handler for event_le_ltk_request to be executed in ble task context
 *
 */
void rsi_ble_event_le_ltk_request(uint16_t status, void *event_data)
{
  uint8_t ble_conn_id;

  rsi_bt_event_le_ltk_request_t *le_ltk_req = (rsi_bt_event_le_ltk_request_t *)event_data;
  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, le_ltk_req->dev_addr);

#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif

  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].rsi_le_ltk_resp, le_ltk_req, sizeof(rsi_bt_event_le_ltk_request_t));
  rsi_ble_conn_info[ble_conn_id].conn_state = on_connect_state;
  printf("\r\n in LTK  request -conn%d \r\n", ble_conn_id);

  if (0) {
    printf("\r\n positive reply\n");
    //! give le ltk req reply cmd with positive reply
    status = rsi_ble_ltk_req_reply(rsi_ble_conn_info[ble_conn_id].rsi_le_ltk_resp.dev_addr,
                                   1,
                                   rsi_ble_conn_info[ble_conn_id].l_rsi_encryption_enabled.localltk);
    if (status != RSI_SUCCESS) {
      printf("\r\n failed to restart smp pairing with status: 0x%x -conn%d\r\n", status, ble_conn_id);
    }
  } else {
    printf("\r\n negative reply\n");
    //! give le ltk req reply cmd with negative reply
    status = rsi_ble_ltk_req_reply(rsi_ble_conn_info[ble_conn_id].rsi_le_ltk_resp.dev_addr, 0, NULL);
    if (status != RSI_SUCCESS) {
      printf("\r\n failed to restart smp pairing with status: 0x%x \r\n", status);
    }
    rsi_ble_conn_info[ble_conn_id].neg_rply = 1;
  }
}

/*==============================================*/
/**
 * @fn          void rsi_ble_event_le_security_keys(uint16_t status, void *event_data)
 * @brief       handler for event_le_security_keys to be executed in ble task context
 * @param[in]   uint16_t , event_status
 * @param[in]   void, event_data
 * @return      None
 *
 * @section description
 * handler for event_le_security_keys to be executed in ble task context
 *
 */
void rsi_ble_event_le_security_keys(uint16_t status, void *event_data)
{

  uint8_t ble_conn_id;
  rsi_bt_event_le_security_keys_t *le_sec_keys = (rsi_bt_event_le_security_keys_t *)event_data;
  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, le_sec_keys->dev_addr);
#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif
  rsi_ble_conn_info[ble_conn_id].conn_state = on_connect_state;

  //! copy to conn specific buffer
  memcpy(&rsi_ble_conn_info[ble_conn_id].rsi_le_security_keys, le_sec_keys, sizeof(rsi_bt_event_le_security_keys_t));
  printf("\r\n in smp security keys event  -conn%d \r\n", ble_conn_id);
  rsi_ble_conn_info[ble_conn_id].conn_state = connected_state;
  memcpy(&temp_le_sec_keys, le_sec_keys, sizeof(rsi_bt_event_le_security_keys_t));
  rsi_ble_conn_info[ble_conn_id].smp_done = 1;

  status = add_security_keys_to_device_list(ble_dev_ltk_list, temp_le_sec_keys);
  if (status != RSI_SUCCESS) {
    printf("\n Failed to add Security keys to list \n");
  }
}

/*==============================================*/
/**
 * @fn          void rsi_ble_on_ctkd(uint16_t status, void *event_data)
 * @brief       handler for event_ctkd to be executed in ble task context
 * @param[in]   uint16_t , event_status 
 * @param[in]   void, event_data
 * @return      None
 *
 * @section description
 * handler for event_ctkd to be executed in ble task context
 *
 */
void rsi_ble_on_ctkd(uint16_t status, void *event_data)
{
  //! Add handling here
  uint8_t ble_conn_id;
  rsi_ble_event_ctkd_t *ctkd = (rsi_ble_event_ctkd_t *)event_data;
  uint8_t ix;
  rsi_6byte_dev_address_to_ascii(remote_dev_addr_conn, ctkd->dev_addr);
#if (CONNECT_OPTION != CONN_BY_NAME)
  //! get conn_id
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn);
#else
  ble_conn_id = rsi_get_ble_conn_id(remote_dev_addr_conn, NULL, 0);
#endif
  printf(" \n Received ctkd event and its status =  %x \n", status);
  memcpy(&ble_ctkd, ctkd, sizeof(rsi_ble_event_ctkd_t));
#if 1
  //! convert to ascii
  rsi_6byte_dev_address_to_ascii(rsi_ble_conn_info[ble_conn_id].str_remote_address, ctkd->dev_addr);
  printf("\n CTKD remote dev addr = %s \n", rsi_ble_conn_info[ble_conn_id].str_remote_address);
#endif
  printf("\n Derived LinkKey from CTKD :\n");
  for (ix = 0; ix < 16; ix++) {
    printf("%x ", ctkd->key[ix]);
  }

  status = add_derived_key_to_ltk_list(ble_dev_ltk_list, &ble_ctkd);
  if (status != RSI_SUCCESS) {
    printf("\n Failed to add_derived_key_to_ltk_list");
  }
}

/*==============================================*/
/**
 * @fn          void rsi_ble_event_smp_pending(uint16_t status, void *event_data)
 * @brief       handler for event_smp_pending to be executed in ble task context
 * @param[in]   uint16_t , event_status
 * @param[in]   void, event_data
 * @return      None
 *
 * @section description
 * handler for event_smp_pending to be executed in ble task context
 *
 */
void rsi_ble_event_smp_pending(uint16_t status, void *event_data)
{
  uint8_t *conn_id             = (uint8_t *)event_data;
  uint8_t ble_conn_id          = *conn_id;
  uint8_t BD_ADDR[BD_ADDR_LEN] = { 0 };
  uint32_t i; // Local loop variable
  for (i = 0; i < TOTAL_CONNECTIONS; i++) {
#ifdef SL_SI91X_BLE_GATT_CLIENT_COMPONENT
    // If GATT Client is present, wait for MTU exchange event before initiating SMP
    // (event_mtu handler in Client component sets mtu_exchange_done flag)
    if (rsi_ble_conn_info[ble_conn_id].mtu_exchange_done && ble_confgs.ble_conn_configuration[ble_conn_id].smp_enable
        && (!smp_in_progress))
#else
    // If GATT Server only or no GATT, proceed without waiting for MTU exchange
    if (ble_confgs.ble_conn_configuration[ble_conn_id].smp_enable && (!smp_in_progress))
#endif
    {
      if (!rsi_ble_conn_info[ble_conn_id].smp_pairing_initated) {
        if (rsi_ble_conn_info[i].smp_state == smp_pending) {
          memcpy(BD_ADDR, rsi_ble_conn_info[i].rsi_enhc_conn_status.dev_addr, BD_ADDR_LEN);
          status = rsi_ble_smp_pair_request(BD_ADDR, RSI_BLE_SMP_IO_CAPABILITY, MITM_ENABLE);
          if (status != RSI_SUCCESS) {
            printf("\r\n RSI_BLE_SMP_REQ_EVENT: failed to initiate the SMP pairing process: 0x%x \r\n -conn%d",
                   status,
                   ble_conn_id);
          } else {
            smp_in_progress                                     = 1;
            rsi_ble_conn_info[ble_conn_id].smp_pairing_initated = true;
            rsi_ble_conn_info[i].smp_state                      = smp_triggered;
            rsi_6byte_dev_address_to_ascii(rsi_ble_conn_info[ble_conn_id].str_remote_address, BD_ADDR);
            printf("\r\n smp pairing request initiated to %s - conn%d \r\n",
                   rsi_ble_conn_info[ble_conn_id].str_remote_address,
                   ble_conn_id);
            break;
          }
        }
      }
    }
  }
}
