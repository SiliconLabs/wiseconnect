/*******************************************************************************
* @file  smp_config.h
* @brief SMP (Security Manager Protocol) Configuration
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

#ifndef SMP_CONFIG_H
#define SMP_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> SMP Pairing Configuration
// <o RSI_BLE_SMP_IO_CAPABILITY> IO Capability
//   <0x00=> Display Only
//   <0x01=> Display Yes/No
//   <0x02=> Keyboard Only
//   <0x03=> No Input No Output
//   <0x04=> Keyboard Display
// <i> Default: 0x00 (Display Only)
#ifndef RSI_BLE_SMP_IO_CAPABILITY
#define RSI_BLE_SMP_IO_CAPABILITY 0x00
#endif

// <o RSI_BLE_APP_SMP_PASSKEY> Passkey (for pairing)
// <0-999999:1>
// <i> Default: 0
#ifndef RSI_BLE_APP_SMP_PASSKEY
#define RSI_BLE_APP_SMP_PASSKEY 0
#endif

// <q MITM_ENABLE> MITM (Man-In-The-Middle) protection
// <i> 0 = Just Works, 1 = Passkey/Numeric Comparison. Default: 1
#ifndef MITM_ENABLE
#define MITM_ENABLE 1
#endif

// <o SMP_PAIRING_TIMEOUT_MS> Pairing timeout (milliseconds)
// <1000-60000:1000>
// <i> Default: 30000
#define SMP_PAIRING_TIMEOUT_MS 30000

// </h>

// <h> SMP Enable (per connection)
// <i> Enable SMP for each connection P1-P8 (peripherals), C1-C2 (centrals)

// <q SMP_ENABLE_P1> Enable SMP for Peripheral 1
#define SMP_ENABLE_P1 1
// <q SMP_ENABLE_P2> Enable SMP for Peripheral 2
#define SMP_ENABLE_P2 1
// <q SMP_ENABLE_P3> Enable SMP for Peripheral 3
#define SMP_ENABLE_P3 1
// <q SMP_ENABLE_P4> Enable SMP for Peripheral 4
#define SMP_ENABLE_P4 0
// <q SMP_ENABLE_P5> Enable SMP for Peripheral 5
#define SMP_ENABLE_P5 0
// <q SMP_ENABLE_P6> Enable SMP for Peripheral 6
#define SMP_ENABLE_P6 0
// <q SMP_ENABLE_P7> Enable SMP for Peripheral 7
#define SMP_ENABLE_P7 0
// <q SMP_ENABLE_P8> Enable SMP for Peripheral 8
#define SMP_ENABLE_P8 0
// <q SMP_ENABLE_C1> Enable SMP for Central 1
#define SMP_ENABLE_C1 1
// <q SMP_ENABLE_C2> Enable SMP for Central 2
#define SMP_ENABLE_C2 0

/*=======================================================================*/
//! SMP Key Distribution Configuration (User-Configurable)
/*=======================================================================*/

//! Address Resolution Enable
#ifndef RESOLVE_ENABLE
#define RESOLVE_ENABLE 1
#endif

//! Responder Key Distribution
#if RESOLVE_ENABLE
#define RESPONDER_KEYS_TO_DIST (ENC_KEY_DIST | SIGN_KEY_DIST | ID_KEY_DIST)
#else
#define RESPONDER_KEYS_TO_DIST (ENC_KEY_DIST | SIGN_KEY_DIST | ID_KEY_DIST | LINK_KEY_DIST)
#endif

//! Initiator Key Distribution
#if RESOLVE_ENABLE
#define INITIATOR_KEYS_TO_DIST (ID_KEY_DIST | ENC_KEY_DIST | SIGN_KEY_DIST)
#else
#define INITIATOR_KEYS_TO_DIST (ENC_KEY_DIST | SIGN_KEY_DIST | ID_KEY_DIST | LINK_KEY_DIST)
#endif

// </h>
// <<< end of configuration section >>>

#define AUTH_REQ_BITS (AUTH_REQ_BONDING_BITS | AUTH_REQ_MITM_BIT | AUTH_REQ_SC_BIT | AUTH_REQ_CT2_BIT)

#endif // SMP_CONFIG_H
