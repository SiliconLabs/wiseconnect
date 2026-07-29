/*******************************************************************************
* @file  smp.h
* @brief SMP (Security Manager Protocol) Public API
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
 * @brief Include Order for SMP Component
 * 
 * IMPORTANT: Include smp_config.h BEFORE smp.h in source files:
 * 
 * @code
 * #include <smp_config.h>  // Configuration (angle brackets: resolved via -I path)
 * #include "smp.h"         // Types and API
 * @endcode
 * 
 * Use angle brackets for config headers so the user-editable copy under
 * config/ble_config/ is always found before the default in the component's inc/.
 */

#ifndef SMP_H
#define SMP_H

#include <stdint.h>
#include "rsi_ble.h"
#include "gap.h" // For GAP types (rsi_ble_conn_info_t, etc.) and config (TOTAL_CONNECTIONS, etc.)

/*=======================================================================*/
//! SMP Macros
/*=======================================================================*/

//! BD Address Length
#ifndef BD_ADDR_LEN
#define BD_ADDR_LEN 6
#endif

// Note: RSI_SUCCESS/RSI_FAILURE are provided by:
//       - rsi_bt_common_apis.h (include this in source files)
//       - rsi_common_apis.h (alternative)

/*=======================================================================*/
//! SMP Constants (Bluetooth Specification-Defined)
/*=======================================================================*/

//! OOB Data Flags (Bluetooth Spec)
#define LOCAL_OOB_DATA_FLAG_NOT_PRESENT (0x00)

//! Authentication Requirement Bits (Bluetooth Spec Section 3.5.1)
#define AUTH_REQ_BONDING_BITS ((1 << 0)) // Bit 0: Bonding
#define AUTH_REQ_MITM_BIT     (1 << 2)   // Bit 2: MITM Protection
#define AUTH_REQ_SC_BIT       (1 << 3)   // Bit 3: Secure Connections
#define AUTH_REQ_CT2_BIT      (1 << 5)   // Bit 5: CT2

//! Encryption Key Size (Bluetooth Spec)
#define MAXIMUM_ENC_KEY_SIZE_16 (16)

//! Key Distribution Bits (Bluetooth Spec Section 3.6.1)
#define ENC_KEY_DIST  (1 << 0) // Bit 0: LTK (Encryption Key)
#define ID_KEY_DIST   (1 << 1) // Bit 1: IRK (Identity Key)
#define SIGN_KEY_DIST (1 << 2) // Bit 2: CSRK (Signature Key)
#define LINK_KEY_DIST (0 << 3) // Bit 3: Link Key (BR/EDR)

/*=======================================================================*/
//! SMP Structures
/*=======================================================================*/

//! LTK (Long Term Key) device list entry
//! Used to store encryption keys and device information for bonded devices
typedef struct rsi_ble_dev_ltk_list_s {
  uint8_t used;                 // Entry in use flag
  uint8_t enc_enable;           // Encryption enabled flag
  uint8_t sc_enable;            // Secure Connections enabled flag
  uint8_t remote_dev_addr_type; // Remote device address type
  uint8_t remote_dev_addr[6];   // Remote device BD address
  uint8_t peer_irk[16];         // Peer Identity Resolving Key
  uint8_t local_irk[16];        // Local Identity Resolving Key
  uint16_t remote_ediv;         // Remote Encrypted Diversifier
  uint16_t local_ediv;          // Local Encrypted Diversifier
  uint8_t remote_rand[8];       // Remote Random number
  uint8_t localrand[8];         // Local Random number
  uint8_t remote_ltk[16];       // Remote Long Term Key
  uint8_t localltk[16];         // Local Long Term Key
  uint8_t Identity_addr_type;   // Identity address type
  uint8_t Identity_addr[6];     // Identity address
  uint8_t derived_linkkey[16];  // Derived link key for BR/EDR
} rsi_ble_dev_ltk_list_t;

/*=======================================================================*/
//! Multi-protocol Variables (GAP owns - use extern)
/*=======================================================================*/

//! Connection information array (defined in GAP)
extern rsi_ble_conn_info_t rsi_ble_conn_info[];

//! Connection configuration (defined in BLE Unified)
extern rsi_ble_conn_config_t ble_conn_spec_conf;

/*=======================================================================*/
//! SMP-Owned Variables (Defined in smp_user_event_hdlrs.c)
/*=======================================================================*/

//! LTK key storage for SMP
extern rsi_ble_dev_ltk_list_t ble_dev_ltk_list[TOTAL_CONNECTIONS];

//! SMP in progress flag
extern uint8_t smp_in_progress;

/*=======================================================================*/
//! SMP Public API
/*=======================================================================*/

/**
 * @brief Initialize SMP component
 * @return int32_t - SL_STATUS_OK on success, error code otherwise
 * 
 * @details Initializes SMP pairing capabilities, registers callbacks,
 *          and sets up SMP-specific connection configuration.
 */
int32_t ble_smp_init(void);

/**
 * @brief Initialize SMP-specific fields in rsi_ble_conn_info[]
 * @details Sets SMP pairing state flags for all connections
 * 
 * EXTRACTED FROM: gap_utilities.c:ble_private_default_init()
 */
void rsi_ble_smp_default_init(void);

/**
 * @brief Initialize SMP-specific connection buffer configuration
 * @param[in] ble_conn_spec_conf - Connection configuration buffer
 * @return int8_t - RSI_SUCCESS or RSI_FAILURE
 * 
 * @details Sets SMP enable flags for all connections
 * 
 * EXTRACTED FROM: gap_utilities.c:rsi_ble_initialize_conn_buffer()
 */
int8_t rsi_ble_smp_initialize_conn_config(rsi_ble_conn_config_t *ble_conn_spec_conf);

/**
 * @brief Add security keys to device LTK list
 * @param[in,out] ble_dev_ltk_list - LTK device list pointer
 * @param[in] le_sec_keys - Security keys event data
 * @return int8_t - 0 on success, -1 if device not found
 * 
 * @details Updates IRKs and LTK information for an existing device in the list
 */
int8_t add_security_keys_to_device_list(rsi_ble_dev_ltk_list_t *ble_dev_ltk_list,
                                        rsi_bt_event_le_security_keys_t le_sec_keys);

/**
 * @brief Add device to LTK key list
 * @param[in,out] ble_dev_ltk_list - LTK device list pointer
 * @param[in] enc_enabled - Encryption enabled event data
 * @return int8_t - 0 on success, -1 on failure
 * 
 * @details Adds a new device or updates existing device in LTK list with encryption info
 */
int8_t add_device_to_ltk_key_list(rsi_ble_dev_ltk_list_t *ble_dev_ltk_list,
                                  rsi_bt_event_encryption_enabled_t *enc_enabled);

/**
 * @brief Add derived link key to LTK list
 * @param[in,out] ble_dev_ltk_list - LTK device list pointer
 * @param[in] ble_ctkd - Cross-Transport Key Derivation event data
 * @return int32_t - 0 on success, -1 if device not found
 * 
 * @details Updates the derived link key for BR/EDR for an existing device
 */
int32_t add_derived_key_to_ltk_list(rsi_ble_dev_ltk_list_t *ble_dev_ltk_list, rsi_ble_event_ctkd_t *ble_ctkd);

#endif // SMP_H
