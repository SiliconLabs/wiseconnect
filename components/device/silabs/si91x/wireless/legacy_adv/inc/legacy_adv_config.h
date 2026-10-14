/*******************************************************************************
* @file  legacy_adv_config.h
* @brief Legacy Advertising Configuration Header
*******************************************************************************
* # License
* <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef LEGACY_ADV_CONFIG_H
#define LEGACY_ADV_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Legacy Advertising Configuration
// <i> Advertising interval and data size. ADV_ENABLED_DEFAULT and SCAN_ENABLED_DEFAULT are in gap_config.h

// <h> Advertising Interval
// <i> Units: 0.625ms (e.g., 0x152 = 338*0.625ms = 210ms). Range: 0x20-0x4000 (20ms to 10.24s)

// <o RSI_BLE_ADV_INT_MIN> Minimum advertising interval
// <0x20-0x4000:1>
// <i> Default: 0x152 (210ms)
#ifndef RSI_BLE_ADV_INT_MIN
#define RSI_BLE_ADV_INT_MIN 0x152
#endif

// <o RSI_BLE_ADV_INT_MAX> Maximum advertising interval
// <0x20-0x4000:1>
// <i> Default: 0x152 (210ms)
#ifndef RSI_BLE_ADV_INT_MAX
#define RSI_BLE_ADV_INT_MAX 0x152
#endif

// </h>

// <h> Advertising Data Size
// <o LEGACY_ADV_DATA_MAX_LEN> Maximum legacy advertising data length (bytes)
// <1-31:1>
// <i> Default: 31 (Bluetooth spec maximum)
#ifndef LEGACY_ADV_DATA_MAX_LEN
#define LEGACY_ADV_DATA_MAX_LEN 31
#endif

// </h>
// </h>
// <<< end of configuration section >>>

#ifdef __cplusplus
}
#endif

#endif // LEGACY_ADV_CONFIG_H
