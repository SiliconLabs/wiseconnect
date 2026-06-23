/*****************************************************************************
 * @file sli_wifi_radio_priority_config.h
 * @brief Wi-Fi Radio priorities
 *******************************************************************************
 * # License
 * <b>Copyright 2026 Silicon Laboratories Inc. www.silabs.com</b>
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

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Wi-Fi Radio priorities

// <o SL_WIFI_RADIO_PRIO_STATION_BEACON_RECEPTION_VALUE_MIN> Beacon Reception - Min
// <0-255:1>
// <i> Default: 127
// <i> Wi-Fi Station Beacon Reception Minimum priority.
#define SL_WIFI_STA_BEACON_RECEPTION_MIN 0

// <o SL_WIFI_RADIO_PRIO_STATION_BEACON_RECEPTION_VALUE_MAX> Beacon Reception - Max
// <0-255:1>
// <i> Default: 127
// <i> Wi-Fi Station Beacon Reception Maximum priority.
#define SL_WIFI_STA_BEACON_RECEPTION_MAX 255

// <o SL_WIFI_RADIO_PRIO_STATION_BEACON_RECEPTION_VALUE_MAX> Beacon Reception - Step
// <0-255:1>
// <i> Default: 127
// <i> Wi-Fi Station Beacon Reception priority increase.
#define SL_WIFI_STA_BEACON_RECEPTION_STEP 1

// <o SL_WIFI_RADIO_PRIO_KEEP_ALIVE_VALUE_MIN> KEEP_ALIVE - Min
// <0-255:1>
// <i> Default: 127
// <i> Wi-Fi Station Keep alive Minimum priority.
#define SL_WIFI_STA_KEEP_ALIVE_MIN 0

// <o SL_WIFI_RADIO_PRIO_KEEP_ALIVE_VALUE_MAX> KEEP_ALIVE - Max
// <0-255:1>
// <i> Default: 127
// <i> Wi-Fi Station Keep alive Maximum priority.
#define SL_WIFI_STA_KEEP_ALIVE_MAX 255

// <o SL_WIFI_RADIO_PRIO_KEEP_ALIVE_VALUE_MAX> KEEP_ALIVE - Step
// <0-255:1>
// <i> Default: 127
// <i> Wi-Fi Station Keep alive priority increase.
#define SL_WIFI_STA_KEEP_ALIVE_STEP 1

// <o SL_WIFI_RADIO_PRIO_DATA_TRANSFER_VALUE_MIN> Data Transfer - Min
// <0-255:1>
// <i> Default: 127
// <i> Wi-Fi Station Data Transfer Minimum priority.
#define SL_WIFI_DATA_TRANSFER_PRIORITY_MIN 0

// <o SL_WIFI_RADIO_PRIO_DATA_TRANSFER_VALUE_MAX> Data Transfer - Max
// <0-255:1>
// <i> Default: 127
// <i> Wi-Fi Station Data Transfer Maximum priority.
#define SL_WIFI_DATA_TRANSFER_PRIORITY_MAX 255

// <o SL_WIFI_RADIO_PRIO_DATA_TRANSFER_VALUE_MAX> Data Transfer - Step
// <0-255:1>
// <i> Default: 127
// <i> Wi-Fi Station Data Transfer priority increase.
#define SL_WIFI_DATA_TRANSFER_PRIORITY_STEP 1

// </h>
// <<< end of configuration section >>>