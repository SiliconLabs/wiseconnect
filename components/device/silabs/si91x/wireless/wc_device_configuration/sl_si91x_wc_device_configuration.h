/***************************************************************************/ /**
 * @file sl_si91x_wc_device_configuration.h
 * @brief Wizard configuration support for device configuration.
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_SI91X_WC_DEVICE_CONFIGURATION_H
#define SL_SI91X_WC_DEVICE_CONFIGURATION_H

#include "sl_wifi_device.h"
#include "sl_si91x_types.h"

// <<< Use Configuration Wizard in Context Menu >>>
#ifdef __cplusplus
extern "C" {
#endif

#define SLI_DISABLE 0

// <h> General
// <o SL_SI91X_WC_BOOT_OPTION> Boot Option
// <LOAD_NWP_FW=> Load Default NWP Firmware (default)
// <LOAD_DEFAULT_NWP_FW_ACTIVE_LOW=> Load Default NWP Firmware Active Low (NCP)
// <BURN_NWP_FW=> Burn NWP Firmware
// <BURN_M4_FW=> Burn M4 Firmware
// <SL_SI91X_LOAD_NWP_FW_WITH_IMAGE_NUMBER=> Load Select NWP Firmware
// <i> Note: Select the desired image using the "NWP Firmware Image Number" option.
// <i> This option allows the user to configure the boot option for the device.
// <i> The boot option determines how the firmware is loaded or updated during initialization.
// <i> Users can choose from various options such as loading default firmware, burning firmware, or selecting specific firmware images.
// <i> Proper selection of the boot option is crucial for ensuring the device operates as intended.
// <i> Default: Load NWP Firmware
#define SL_SI91X_WC_BOOT_OPTION LOAD_NWP_FW

// <e SL_SI91X_WC_SET_MAC_ADDRESS> Set Mac Address
// <i> Enable or Disable.
// <i> Default: Disable
#define SL_SI91X_WC_SET_MAC_ADDRESS 0

// <a.6 SL_SI91X_WC_MAC_ADDRESS> Mac Address <0..255> <f.h>
// <i> Default: { 0x0 0x0 0x0 0x0 0x0 0x0 }
// clang-format off
 #define SL_SI91X_WC_MAC_ADDRESS { 0, 0, 0, 0, 0, 0 }
// clang-format on
// </e>

// <i> <o SL_SI91X_WC_WIFI_BAND> Wi-Fi Band
// <i> <SL_SI91X_WIFI_BAND_2_4GHZ=> 2.4 GHz Wi-Fi Band
// <i> <SL_SI91X_WIFI_BAND_5GHZ=> 5 GHz Wi-Fi Band (not currently supported)
// <i> <SL_SI91X_WIFI_DUAL_BAND=> Both 2.4 GHz and 5 GHZ WiFi Band(not currently supported)
// <i> Default: 2.4 GHz Wi-Fi band
#define SL_SI91X_WC_WIFI_BAND SL_SI91X_WIFI_BAND_2_4GHZ

// <o SL_SI91X_WC_REGION> Region
// <DEFAULT_REGION=> Factory Default Region
// <US=> United States
// <EU=> European Union
// <JP=> Japan
// <WORLD_DOMAIN=> Worldwide Domain
// <KR=> Korea
// <i> <SG=> Singapore (not currently supported)
// <CN=> China
// <IGNORE_REGION=> Ignore Region
// <i> "Ignore Region" - Skips updating the region code during initialization
// <i> Default: United States
#define SL_SI91X_WC_REGION DEFAULT_REGION

// <o SL_SI91X_WC_EFUSE_DATA_TYPE> Efuse Data Type
// <SL_SI91X_EFUSE_MFG_SW_VERSION=> Manufacturing Software Version
// <SL_SI91X_EFUSE_PTE_CRC=> PTE CRC
// <i> This option is used to specify the type of efuse data to be read from flash memory during initialization.
// <i> Default: Manufacturing software version
#define SL_SI91X_WC_EFUSE_DATA_TYPE SL_SI91X_EFUSE_MFG_SW_VERSION

// <o SL_SI91X_WC_NWP_FW_IMAGE_NUMBER> NWP Firmware Image Number
// <SL_SI91X_NWP_FW_IMAGE_NUMBER_0=> First NWP Firmware Image
// <SL_SI91X_NWP_FW_IMAGE_NUMBER_1=> Second NWP Firmware Image
// <i> Applicable when "Load select NWP Firmware" is chosen in the boot option.
// <i> Image number for the NWP firmware, used to specify which firmware image to load.
// <i> Default: SL_SI91X_NWP_FW_IMAGE_NUMBER_0
#define SL_SI91X_WC_NWP_FW_IMAGE_NUMBER SL_SI91X_NWP_FW_IMAGE_NUMBER_0
// </h>

// <h> Pool Ratio Configuration
// <o SL_SI91X_WC_TX_POOL_RATIO> Enable TX Pool Ratio <0-255>
// <i> Default: 0
#define SL_SI91X_WC_TX_POOL_RATIO 0

// <o SL_SI91X_WC_RX_POOL_RATIO> Enable RX Pool Ratio <0-255>
// <i> Default: 0
#define SL_SI91X_WC_RX_POOL_RATIO 0

// <o SL_SI91X_WC_GLOBAL_POOL_RATIO> Enable Global Pool Ratio <0-255>
// <i> Default: 0
#define SL_SI91X_WC_GLOBAL_POOL_RATIO 0
// </h>

// <h> Opermode Configuration
// <o SL_SI91X_WC_OPERMODE> Operating Mode
// <SL_SI91X_CLIENT_MODE=> Client Mode
// <SL_SI91X_ENTERPRISE_CLIENT_MODE=> Enterprise Client Mode
// <SL_SI91X_ACCESS_POINT_MODE=> Access Point Mode
// <SL_SI91X_TRANSCEIVER_MODE=> Transceiver Mode
// <SL_SI91X_TRANSMIT_TEST_MODE=> Transmit Test Mode
// <SL_SI91X_CONCURRENT_MODE=> Concurrent Mode
// <i> Default: Client Mode
#define SL_SI91X_WC_OPERMODE SL_SI91X_CLIENT_MODE
// </h>

// <h> Co-Existence Configuration
// <o SL_SI91X_WC_COEXMODE> Co-Existence Mode
// <SL_SI91X_WLAN_ONLY_MODE=> WLAN Only Mode
// <SL_SI91X_BLE_MODE=> BLE Only Mode (Supported in the "bt_stack_bypass" application only)
// <SL_SI91X_WLAN_BLE_MODE=> WLAN and BLE Mode
// <i> Note: "BLE_only mode" will be supported in the "bt_stack_bypass" application only.
// <i> Default: WLAN Only Mode
#define SL_SI91X_WC_COEXMODE SL_SI91X_WLAN_ONLY_MODE
// </h>

// <h> Wi-Fi
// <h> Wi-Fi Standards
// <o SL_SI91X_WC_WIFI_STANDARD> Wi-Fi Standard
// <0=> None
// <1=> Disable 11ax Support
// <2=> Enable 802.11w Support
// <i> <3=> Enable IEEE 802.11J Support (currently not supported and not visible in the UI)
// <i> Default: None
#define SL_SI91X_WC_WIFI_STANDARD 0

// Map the selected option to the corresponding feature
#if SLI_SI91X_WC_WIFI_STANDARD == 0
#define SL_SI91X_WC_FEAT_DISABLE_11AX_SUPPORT 0
#define SL_SI91X_WC_EXT_FEAT_IEEE_80211W      0
#define SL_SI91X_WC_EXT_FEAT_IEEE_80211J      0
#elif SLI_SI91X_WC_WIFI_STANDARD == 1
#define SL_SI91X_WC_FEAT_DISABLE_11AX_SUPPORT 1
#define SL_SI91X_WC_EXT_FEAT_IEEE_80211W      0
#define SL_SI91X_WC_EXT_FEAT_IEEE_80211J      0
#elif SLI_SI91X_WC_WIFI_STANDARD == 2
#define SL_SI91X_WC_FEAT_DISABLE_11AX_SUPPORT 0
#define SL_SI91X_WC_EXT_FEAT_IEEE_80211W      1
#define SL_SI91X_WC_EXT_FEAT_IEEE_80211J      0
#elif SLI_SI91X_WC_WIFI_STANDARD == 3
#define SL_SI91X_WC_FEAT_DISABLE_11AX_SUPPORT 0
#define SL_SI91X_WC_EXT_FEAT_IEEE_80211W      0
#define SL_SI91X_WC_EXT_FEAT_IEEE_80211J      1
#else
#error "Invalid Wi-Fi Standard selected."
#endif

#define SLI_SI91X_WC_FEAT_DISABLE_11AX_SUPPORT (SL_SI91X_WC_FEAT_DISABLE_11AX_SUPPORT << 15)
#define SLI_SI91X_WC_EXT_FEAT_IEEE_80211W      (SL_SI91X_WC_EXT_FEAT_IEEE_80211W << 13)
#define SLI_SI91X_WC_EXT_FEAT_IEEE_80211J      (SL_SI91X_WC_EXT_FEAT_IEEE_80211J << 12)
// </h>

// <i> <h> Scanning
// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_CUSTOM_FEAT_DFS_CHANNEL_SUPPORT> DFS Channel Scanning Support(not supported)
// <i> This bit enables the support for scanning Dynamic Frequency Selection (DFS) channels in the 5 GHz band.
// <i> It is applicable in Wi-Fi client mode only. Ensure to set the region configuration before scanning DFS channels.
// <i> @note 5 GHz is not supported in SI917.
// <i> Enable or disable support for scanning in DFS channels() in 5GHZ band feature.
// <i> Default: Disable
#define SL_SI91X_WC_CUSTOM_FEAT_DFS_CHANNEL_SUPPORT  0
#define SLI_SI91X_WC_CUSTOM_FEAT_DFS_CHANNEL_SUPPORT (SL_SI91X_WC_CUSTOM_FEAT_DFS_CHANNEL_SUPPORT << 8)
// <i> </h>

// <h> Roaming
// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_EXT_FEAT_ENABLE_11R_OTA> Enable 802.11R OTA Roaming(not supported)
// <i> To enable 802.11R Over The Air Roaming (currently not supported).
// <i> Enabling this bit activates support for 802.11R (Fast BSS Transition) Over The Air Roaming, which improves the handoff experience between access points.
// <i> @note Resource Request Support is not present.
// <i> @note If both 11R_OTA and 11R_ODS are not enabled, the device would default to Legacy Roaming.
// <i> Enable or Disable
// <i> Default: Disable
#define SL_SI91X_WC_EXT_FEAT_ENABLE_11R_OTA  0
#define SLI_SI91X_WC_EXT_FEAT_ENABLE_11R_OTA (SL_SI91X_WC_EXT_FEAT_ENABLE_11R_OTA << 11)

// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_EXT_FEAT_ENABLE_11R_ODS> Enable 802.11R ODS Roaming(not supported)
// <i> Enabling this bit activates support for 802.11R (Fast BSS Transition) Over the Distribution System Roaming, which enhances roaming performance across different access points in the distribution system.
// <i> @note 1. Resource Request Support is not present.
// <i> @note 2. If both 11R_OTA and 11R_ODS are not enabled, the device would default to Legacy Roaming.
// <i> Enable or Disable
// <i> Default: Disable
#define SL_SI91X_WC_EXT_FEAT_ENABLE_11R_ODS  0
#define SLI_SI91X_WC_EXT_FEAT_ENABLE_11R_ODS (SL_SI91X_WC_EXT_FEAT_ENABLE_11R_ODS << 16)

// <q SL_SI91X_WC_CUSTOM_FEAT_ROAM_WITH_DEAUTH_OR_NULL_DATA> Roaming Mode selection
// <i> Selects between de-authentication or null data (with power management bit set) for roaming.
// <i> If this bit is enabled then roam through DEAUTH, or roam through NULL.
// <i> Enable or disable.
// <i> Default: Disable
#define SL_SI91X_WC_CUSTOM_FEAT_ROAM_WITH_DEAUTH_OR_NULL_DATA 0
#define SLI_SI91X_WC_CUSTOM_FEAT_ROAM_WITH_DEAUTH_OR_NULL_DATA \
  (SL_SI91X_WC_CUSTOM_FEAT_ROAM_WITH_DEAUTH_OR_NULL_DATA << 17)

// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_CUSTOM_FEAT_DUAL_BAND_ROAM_VCSAFD> Dual-Band Roaming and VCSAFD(not supported)
// <i> Enables dual-band roaming and VCSAFD (Virtual Channel Scan and Frequency Avoidance Detection) feature (currently not supported).
// <i> This bit enables support for dual-band roaming and VCSAFD,
// <i> which enhances the module’s ability to switch between different frequency bands and avoid interference.
// <i> Enable or disable.
// <i> Default: Disable
#define SL_SI91X_WC_CUSTOM_FEAT_DUAL_BAND_ROAM_VCSAFD  0
#define SLI_SI91X_WC_CUSTOM_FEAT_DUAL_BAND_ROAM_VCSAFD (SL_SI91X_WC_CUSTOM_FEAT_DUAL_BAND_ROAM_VCSAFD << 27)
// </h>

// <h> Authentication Modes
// <q SL_SI91X_WC_FEAT_SECURITY_OPEN> Open Security (No Authentication)
// <i> This feature supports open security type in client mode.
// <i> @note It is recommended to enable this macro to configure the security type as open mode.
// <i> Default: Disable
#define SL_SI91X_WC_FEAT_SECURITY_OPEN  0
#define SLI_SI91X_WC_FEAT_SECURITY_OPEN (SL_SI91X_WC_FEAT_SECURITY_OPEN << 0)

// <q SL_SI91X_WC_FEAT_SECURITY_PSK> WPA/WPA2 Security (Password-Protected)
// <i> This feature supports WPA/WPA2 security type in client mode.
// <i> @note It is recommended to enable this macro to configure the security type as WPA, WPA2, or any other security modes.
// <i> Default: Disable
#define SL_SI91X_WC_FEAT_SECURITY_PSK  0
#define SLI_SI91X_WC_FEAT_SECURITY_PSK (SL_SI91X_WC_FEAT_SECURITY_PSK << 1)

// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_FEAT_EAP_LEAP_IN_COEX> Enterprise Security (EAP-LEAP Authentication) (not supported)
// <i> Enables Extensible Authentication Protocol - Lightweight Extensible Authentication Protocol (EAP-LEAP) in coexistence mode.
// <i> Default: Disable
#define SL_SI91X_WC_FEAT_EAP_LEAP_IN_COEX  0
#define SLI_SI91X_WC_FEAT_EAP_LEAP_IN_COEX (SL_SI91X_WC_FEAT_EAP_LEAP_IN_COEX << 8)
// </h>

// <h> AP Mode
// <o SL_SI91X_WC_CUSTOM_FEAT_MAX_NUM_OF_CLIENTS> Max Clients in AP Mode <1-15>
// <i> Sets the maximum number of clients supported in AP mode.
// <i> This bit field sets the maximum number of clients that can be supported in Access Point (AP) mode.
// <i> Default: 0
#define SL_SI91X_WC_CUSTOM_FEAT_MAX_NUM_OF_CLIENTS  0
#define SLI_SI91X_WC_CUSTOM_FEAT_MAX_NUM_OF_CLIENTS (SL_SI91X_WC_CUSTOM_FEAT_MAX_NUM_OF_CLIENTS << 13)

// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_EXT_FEAT_16th_STATION_IN_AP_MODE> Enable 16-client Support(not supported)
// <i> To enable 16 client support in Access Point (AP) mode.
// <i> Enabling this bit allows up to 16 stations to connect to the device when it is operating in AP mode.
// <i> @note If this bit is enabled, up to 16 stations can connect; otherwise, a maximum of 8 stations can connect.
// <i> Enable or Disable
// <i> Default: Disable
#define SL_SI91X_WC_EXT_FEAT_16th_STATION_IN_AP_MODE  0
#define SLI_SI91X_WC_EXT_FEAT_16th_STATION_IN_AP_MODE (SL_SI91X_WC_EXT_FEAT_16th_STATION_IN_AP_MODE << 15)

// <q SL_SI91X_WC_CUSTOM_FEAT_AP_IN_HIDDEN_MODE> Enable Hidden Mode for AP
// <i> If this bit is set, the AP is created in a hidden mode where its SSID is not broadcasted, making the AP less visible to clients.
// <i> This feature is valid only when the device is in AP mode.
// <i> Enable or disable creating AP in hidden mode feature.
// <i> Default: Disable
#define SL_SI91X_WC_CUSTOM_FEAT_AP_IN_HIDDEN_MODE  0
#define SLI_SI91X_WC_CUSTOM_FEAT_AP_IN_HIDDEN_MODE (SL_SI91X_WC_CUSTOM_FEAT_AP_IN_HIDDEN_MODE << 5)

// <q SL_SI91X_WC_CUSTOM_FEAT_LIMIT_PACKETS_PER_STA> Limit Packets Per STA in AP Mode
// <i>  In AP mode, if the bit is set, only two packets per Station (STA) would be buffered when the STA is in Power Save (PS) mode.
// <i>  This helps manage buffer usage and ensures efficient packet handling.
// <i> Enable or disable setting only two packets per STA to be buffered when STA is in PS feature.
// <i> Default: Disable
#define SL_SI91X_WC_CUSTOM_FEAT_LIMIT_PACKETS_PER_STA  0
#define SLI_SI91X_WC_CUSTOM_FEAT_LIMIT_PACKETS_PER_STA (SL_SI91X_WC_CUSTOM_FEAT_LIMIT_PACKETS_PER_STA << 22)
// </h>

// <h> Client Mode
// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_CUSTOM_FEAT_ENABLE_AP_BLACKLIST> Enable AP Blacklisting in STA Mode(not supported)
// <i> By default, the client maintains an AP blacklist to avoid specific access points.
// <i> Enabling this feature allows the client to bypass the AP blacklist during roaming or rejoin, if required.
// <i> Enable or disable enabling AP blacklisting in STA mode feature.
// <i> Default: Disable
#define SL_SI91X_WC_CUSTOM_FEAT_ENABLE_AP_BLACKLIST  0
#define SLI_SI91X_WC_CUSTOM_FEAT_ENABLE_AP_BLACKLIST (SL_SI91X_WC_CUSTOM_FEAT_ENABLE_AP_BLACKLIST << 12)

// <q SL_SI91X_WC_FEAT_WPS_DISABLE> Disable WPS In AP mode
// <i> Disables Wi-Fi Protected Setup (WPS) functionality in Client and Access Point (AP) mode.
// <i> Default: Disable
#define SL_SI91X_WC_FEAT_WPS_DISABLE  0
#define SLI_SI91X_WC_FEAT_WPS_DISABLE (SL_SI91X_WC_FEAT_WPS_DISABLE << 7)

// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_EXT_FEAT_AP_BROADCAST_PKT_SND_B4_DTIM> AP Broadcast Customization(not supported)
// <i> Extended custom bitmap for AP broadcast customization.
// <i> Enabling this bit configures the Access Point to send broadcast packets before the DTIM (Delivery Traffic Indication Message) interval.
// <i> If this bit is enabled, the clients connected in power save mode might miss the packets.
// <i> Enable or disable
// <i> Default: Disable
#define SL_SI91X_WC_EXT_FEAT_AP_BROADCAST_PKT_SND_B4_DTIM  0
#define SLI_SI91X_WC_EXT_FEAT_AP_BROADCAST_PKT_SND_B4_DTIM (SL_SI91X_WC_EXT_FEAT_AP_BROADCAST_PKT_SND_B4_DTIM << 4)

// <o SLI_SI91X_WC_FEAT_EAP_TLS_V1PX> TLS Version
// <SLI_DISABLE=> Disable
// <SL_SI91X_FEAT_EAP_TLS_V1P0=> Enable Enterprise Security TLS Version 1.0
// <SL_SI91X_FEAT_EAP_TLS_V1P2=> Enable Enterprise Security TLS Version 1.2
// <i> Default: Disable
#define SLI_SI91X_WC_FEAT_EAP_TLS_V1PX SLI_DISABLE
// </h>

// <h> Transceiver Mode
// <q SL_SI91X_WC_FEAT_TRANSCEIVER_MAC_PEER_DS_SUPPORT> MAC Peer Info Storage
// <i> @details Enables support to store peer information in the MAC layer in transceiver mode of operation.
// <i> Default: Disable
#define SL_SI91X_WC_FEAT_TRANSCEIVER_MAC_PEER_DS_SUPPORT  0
#define SLI_SI91X_WC_FEAT_TRANSCEIVER_MAC_PEER_DS_SUPPORT (SL_SI91X_WC_FEAT_TRANSCEIVER_MAC_PEER_DS_SUPPORT << 13)
// </h>

// <h> Advanced Wi-Fi Configurations
// <q SL_SI91X_WC_FEAT_HIDE_PSK_CREDENTIALS> Hide Sensitive Information (PSK, PMK, EAP credentials)
// <i> Hides sensitive information such as Pre-Shared Key (PSK), Pairwise Master Key (PMK), and EAP credentials.
// <i> Default: Disable
#define SL_SI91X_WC_FEAT_HIDE_PSK_CREDENTIALS  0
#define SLI_SI91X_WC_FEAT_HIDE_PSK_CREDENTIALS (SL_SI91X_WC_FEAT_HIDE_PSK_CREDENTIALS << 9)

// <q SL_SI91X_WC_FEAT_SOCKET_CMDS_ALLOW_BEFORE_WLAN_CONNECTION> Allow socket commands before establishing a WLAN connection.
// <i> This feature, when enabled, permits the execution of socket commands even if the Wi-Fi connection has not been established.
// <i> Default: Disable
#define SL_SI91X_WC_FEAT_SOCKET_CMDS_ALLOW_BEFORE_WLAN_CONNECTION 0
#define SLI_SI91X_WC_FEAT_SOCKET_CMDS_ALLOW_BEFORE_WLAN_CONNECTION \
  (SL_SI91X_WC_FEAT_SOCKET_CMDS_ALLOW_BEFORE_WLAN_CONNECTION << 18)

// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_FEAT_CONC_STA_AP_DYN_SWITCH_SEL> Dynamic STA/AP Mode Switching(not supported)
// <i> This bit enables or disables dynamic switching between Station (STA) and Access Point (AP) modes.
// <i> When enabled, the system can dynamically switch between STA and AP modes based on operational requirements or network conditions.
// <i> This feature is useful for applications requiring flexible mode changes to optimize performance or power consumption.
// <i> @note Enabling this feature allows the system to switch modes dynamically, which can improve adaptability in varying network scenarios.
// <i> Make sure to configure the system appropriately for the desired switching behavior.
// <i> Enable or disable
// <i> Default: Disable
#define SL_SI91X_WC_FEAT_CONC_STA_AP_DYN_SWITCH_SEL  0
#define SLI_SI91X_WC_FEAT_CONC_STA_AP_DYN_SWITCH_SEL (SL_SI91X_WC_FEAT_CONC_STA_AP_DYN_SWITCH_SEL << 17)

// <q SL_SI91X_WC_FEAT_AGGREGATION> Packet Aggregation (AMPDU Support)
// <i> Enables support for packet aggregation
// <i> @note Supports AMPDU for both TX and RX
// <i> Default: Disable
#define SL_SI91X_WC_FEAT_AGGREGATION  0
#define SLI_SI91X_WC_FEAT_AGGREGATION (SL_SI91X_WC_FEAT_AGGREGATION << 2)

// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_FEAT_DISABLE_MCS_5_6_7_DATARATES> Disable MCS-5,6,7 Data Rates (not supported)
// <i> This bit is used to disable the higher MCS data rates (5, 6, and 7) for reduced data rate requirements.
// <i> Enable or disable
// <i> Default: Disable
#define SL_SI91X_WC_FEAT_DISABLE_MCS_5_6_7_DATARATES  0
#define SLI_SI91X_WC_FEAT_DISABLE_MCS_5_6_7_DATARATES (SL_SI91X_WC_FEAT_DISABLE_MCS_5_6_7_DATARATES << 19)

// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_FEAT_DISABLE_SHORT_GI> Disable Short-GI (not supported)
// <i> This bit disables the use of Short-GI, which may affect the timing and performance of the system.
// <i> Enable or disable
// <i> Default: Disable
#define SL_SI91X_WC_FEAT_DISABLE_SHORT_GI  0
#define SLI_SI91X_WC_FEAT_DISABLE_SHORT_GI (SL_SI91X_WC_FEAT_DISABLE_SHORT_GI << 20)
// </h>
// </h>

// <h> Wireless Features
// <q SL_SI91X_WC_PTA_3WIRE_EN> To Enable PTA-3WIRE
// <i> It has three different configurations, which can be chosen in below option(To choose PTA-3WIRE) .
// <i> Enable or disable
// <i> Default: Disable
#define SL_SI91X_WC_PTA_3WIRE_EN  0
#define SLI_SI91X_WC_PTA_3WIRE_EN (SL_SI91X_WC_PTA_3WIRE_EN << 21)

// <o SL_SI91X_WC_PTA_3WIRE_CONFIG_SEL> To Choose PTA-3WIRE <0-3>
// <i> Default: 0
#define SL_SI91X_WC_PTA_3WIRE_CONFIG_SEL  0
#define SLI_SI91X_WC_PTA_3WIRE_CONFIG_SEL (SL_SI91X_WC_PTA_3WIRE_CONFIG_SEL << 22)

// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_EXT_FEAT_FCC_LOW_PWR> Extended Custom Bitmap to Support FCC(not supported)
// <i>  Extended custom bitmap to support FCC (currently not supported).
// <i>  Enabling this bit allows the device to operate in a mode that complies with FCC (Federal Communications Commission) regulations for low power operation.
// <i> Enable or disable
// <i> Default: Disable
#define SL_SI91X_WC_EXT_FEAT_FCC_LOW_PWR  0
#define SLI_SI91X_WC_EXT_FEAT_FCC_LOW_PWR (SL_SI91X_WC_EXT_FEAT_FCC_LOW_PWR << 5)
// </h>

// <h> Network Features
// <h> DHCP & IP Addressing
// <q SL_SI91X_WC_TCP_IP_FEAT_DHCPV4_CLIENT> Enable DHCPv4 Client Feature
// <i> This feature allows the device to obtain an IPv4 address,
// <i> subnet mask, default gateway, and DNS server from a DHCP server.
// <i> Enable or disable DHCPv4 client feature.
// <i> Default: Disable
#define SL_SI91X_WC_TCP_IP_FEAT_DHCPV4_CLIENT  0
#define SLI_SI91X_WC_TCP_IP_FEAT_DHCPV4_CLIENT (SL_SI91X_WC_TCP_IP_FEAT_DHCPV4_CLIENT << 2)

// <q SL_SI91X_WC_TCP_IP_FEAT_DHCPV6_CLIENT> Enable DHCPv6 Client Feature
// <i> This feature allows the device to obtain an IPv6 address
// <i> and other network configuration details from a DHCPv6 server.
// <i> Enable or disable DHCPv6 client feature.
// <i> Default: Disable
#define SL_SI91X_WC_TCP_IP_FEAT_DHCPV6_CLIENT  0
#define SLI_SI91X_WC_TCP_IP_FEAT_DHCPV6_CLIENT (SL_SI91X_WC_TCP_IP_FEAT_DHCPV6_CLIENT << 3)

// <q SL_SI91X_WC_TCP_IP_FEAT_DHCPV4_SERVER> Enable DHCPv4 Server Feature
// <i> This feature allows the device to act as a DHCPv4 server, which provides IPv4
// <i> addresses and network configuration to DHCPv4 clients on the network.
// <i> Enable or disable DHCPv4 server feature.
// <i> Default: Disable
#define SL_SI91X_WC_TCP_IP_FEAT_DHCPV4_SERVER  0
#define SLI_SI91X_WC_TCP_IP_FEAT_DHCPV4_SERVER (SL_SI91X_WC_TCP_IP_FEAT_DHCPV4_SERVER << 4)

// <q SL_SI91X_WC_TCP_IP_FEAT_DHCPV6_SERVER> Enable DHCPv6 Server Feature
// <i> This feature allows the device to act as a DHCPv6 server, which provides IPv6
// <i> addresses and network configuration to DHCPv6 clients on the network.
// <i> Enable or disable DHCPv6 server feature.
// <i> Default: Disable
#define SL_SI91X_WC_TCP_IP_FEAT_DHCPV6_SERVER  0
#define SLI_SI91X_WC_TCP_IP_FEAT_DHCPV6_SERVER (SL_SI91X_WC_TCP_IP_FEAT_DHCPV6_SERVER << 5)

// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_EXT_TCP_FEAT_DHCP_OPT77> Dynamic Host Configuration Protocol User Class(not supported)
// <i> Enable or disable DHCP USER CLASS feature.
// <i> This feature enables DHCP option 77, which allows the device to specify user class information in DHCP requests.
// <i> Default: 0
#define SL_SI91X_WC_EXT_TCP_FEAT_DHCP_OPT77  0
#define SLI_SI91X_WC_EXT_TCP_FEAT_DHCP_OPT77 (SL_SI91X_WC_EXT_TCP_FEAT_DHCP_OPT77 << 1)

// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_CUSTOM_FEAT_DISABLE_GATEWAY_IN_RSI_AP> Disable Gateway Configuration in RSI AP(not supported)
// <i> If this bit is set to 1, the DHCP server behavior changes when the device is in Access Point (AP) mode.
// <i> The DHCP server assigns IP addresses to client nodes without sending out a Gateway address, providing only the assigned IP and Subnet values.
// <i> It is highly recommended to keep this value at '0' for standard AP functionality,
// <i> as disabling the gateway address is typically needed only for very specialized use cases.
// <i> Enable or disable disabling gateway config sent to STA from RSI AP feature.
// <i> Default: Disable
#define SL_SI91X_WC_CUSTOM_FEAT_DISABLE_GATEWAY_IN_RSI_AP  0
#define SLI_SI91X_WC_CUSTOM_FEAT_DISABLE_GATEWAY_IN_RSI_AP (SL_SI91X_WC_CUSTOM_FEAT_DISABLE_GATEWAY_IN_RSI_AP << 2)

// <q SL_SI91X_WC_TCP_IP_FEAT_IPV6> Enable IPV6 Support
// <i> This feature allows the device to use IPv6 (Internet Protocol version 6),
// <i> that provides a larger address space and improved routing capabilities.
// <i> @note IPv6 would also be enabled if the DHCPv6 client or DHCPv6 server
// <i> feature is enabled, regardless of the tcp_ip_feature_bit_map[17] setting.
// <i> Enable or disable IPV6 support.
// <i> Default: Disable
#define SL_SI91X_WC_TCP_IP_FEAT_IPV6  0
#define SLI_SI91X_WC_TCP_IP_FEAT_IPV6 (SL_SI91X_WC_TCP_IP_FEAT_IPV6 << 17)
// </h>

// <h> Protocols
// <q SL_SI91X_WC_TCP_IP_FEAT_ICMP> Enable ICMP Feature (ping)
// <i> This feature allows the device to use ICMP (Internet Control Message
// <i>  Protocol) for network diagnostics, such as sending ping requests.
// <i> Enable or disable ICMP feature (ping).
// <i> Default: Disable
#define SL_SI91X_WC_TCP_IP_FEAT_ICMP  0
#define SLI_SI91X_WC_TCP_IP_FEAT_ICMP (SL_SI91X_WC_TCP_IP_FEAT_ICMP << 11)

// <q SL_SI91X_WC_TCP_IP_FEAT_MDNSD> Enable MDNSD
// <i> This feature allows the device to use Multicast DNS (mDNS) for local
// <i> network service discovery, enabling devices to find each other without
// <i> a central DNS server.
// <i> @note This feature is not supported in AP mode.
// <i> Enable or disable MDNSD (Multicast DNS).
// <i> Default: Disable
#define SL_SI91X_WC_TCP_IP_FEAT_MDNSD  0
#define SLI_SI91X_WC_TCP_IP_FEAT_MDNSD (SL_SI91X_WC_TCP_IP_FEAT_MDNSD << 19)

// <q SL_SI91X_WC_TCP_IP_FEAT_DNS_CLIENT> Enable DNS Client
// <i> This feature allows the device to resolve domain names to IP addresses
// <i> using the Domain Name System (DNS), enabling communication with servers
// <i> by hostname.
// <i> Enable or disable DNS client.
// <i> Default: Disable
#define SL_SI91X_WC_TCP_IP_FEAT_DNS_CLIENT  0
#define SLI_SI91X_WC_TCP_IP_FEAT_DNS_CLIENT (SL_SI91X_WC_TCP_IP_FEAT_DNS_CLIENT << 8)

// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_EXT_TCP_IP_DNS_CLIENT_BYPASS> Enable DNS Client Bypass(not supported)
// <i> This feature allows the device to bypass the internal DNS client,
// <i> using the host for DNS resolution instead.
// <i> Enable or disable DNS client bypass feature.
// <i> Default: Disable
#define SL_SI91X_WC_EXT_TCP_IP_DNS_CLIENT_BYPASS  0
#define SLI_SI91X_WC_EXT_TCP_IP_DNS_CLIENT_BYPASS (SL_SI91X_WC_EXT_TCP_IP_DNS_CLIENT_BYPASS << 7)

// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_TCP_IP_FEAT_SNMP_AGENT> Enable SNMP Client (not supported)
// <i> This feature allows the device to act as an SNMP (Simple Network Management
// <i>  Protocol) agent, which enables network management and monitoring.
// <i> Enable or disable SNMP client.
// <i> Default: Disable
#define SL_SI91X_WC_TCP_IP_FEAT_SNMP_AGENT  0
#define SLI_SI91X_WC_TCP_IP_FEAT_SNMP_AGENT (SL_SI91X_WC_TCP_IP_FEAT_SNMP_AGENT << 9)

// <q SL_SI91X_WC_TCP_IP_FEAT_HTTP_CLIENT> Enable HTTP Client
// <i> This feature allows the device to send HTTP requests and receive HTTP
// <i> responses from web servers, enabling web-based communication.
// <i> Enable or disable HTTP client.
// <i> Default: Disable
#define SL_SI91X_WC_TCP_IP_FEAT_HTTP_CLIENT  0
#define SLI_SI91X_WC_TCP_IP_FEAT_HTTP_CLIENT (SL_SI91X_WC_TCP_IP_FEAT_HTTP_CLIENT << 7)

// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_TCP_IP_FEAT_FTP_CLIENT> Enable FTP Client (not supported)
// <i> This feature allows the device to act as an FTP (File Transfer Protocol)
// <i> client, enabling file transfers to and from FTP servers.
// <i> Enable or disable FTP client.
// <i> Default: Disable
#define SL_SI91X_WC_TCP_IP_FEAT_FTP_CLIENT  0
#define SLI_SI91X_WC_TCP_IP_FEAT_FTP_CLIENT (SL_SI91X_WC_TCP_IP_FEAT_FTP_CLIENT << 15)

// <q SL_SI91X_WC_TCP_IP_FEAT_SNTP_CLIENT> Enable SNTP Client
// <i> This feature allows the device to synchronize its clock with an SNTP
// <i> (Simple Network Time Protocol) server, which ensures accurate timekeeping.
// <i> Enable or disable SNTP client.
// <i> Default: Disable
#define SL_SI91X_WC_TCP_IP_FEAT_SNTP_CLIENT  0
#define SLI_SI91X_WC_TCP_IP_FEAT_SNTP_CLIENT (SL_SI91X_WC_TCP_IP_FEAT_SNTP_CLIENT << 16)

// <q SL_SI91X_WC_EXT_EMB_MQTT_ENABLE> Enable Embedded MQTT
// <i> This feature enables support for embedded MQTT (Message Queuing
// <i> Telemetry Transport) functionality, allowing the device to use MQTT
// <i> without external libraries.
// <i> @note If user wants to use AT command for MQTT, enable this bit in the Opermode Command.
// <i> Enable or disable.
// <i> Default: Disable
#define SL_SI91X_WC_EXT_EMB_MQTT_ENABLE  0
#define SLI_SI91X_WC_EXT_EMB_MQTT_ENABLE (SL_SI91X_WC_EXT_EMB_MQTT_ENABLE << 17)

// <q SL_SI91X_WC_TCP_IP_FEAT_SSL> Enable SSL Feature
// <i> This feature allows the device to use SSL (Secure Sockets Layer) for secure
// <i> communication over the network, that provides encryption and authentication.
// <i> Enable or disable SSL feature.
// <i> Default: Disable
#define SL_SI91X_WC_TCP_IP_FEAT_SSL  0
#define SLI_SI91X_WC_TCP_IP_FEAT_SSL (SL_SI91X_WC_TCP_IP_FEAT_SSL << 10)
// </h>

// <h>  Advanced Protocol Features
// <q SL_SI91X_WC_FEAT_LONG_ICMP_PACKET> Support for Long-Sized ICMP Packets
// <i> Enables support for long-sized Internet Control Message Protocol (ICMP) packets.
// <i> Enable this bit to receive and process the ICMP requests of size more than 308 bytes sent from peers.
// <i> A maximum packet size of 1472 bytes can be processed for ICMP and 1452 bytes for ICMPv6.
// <i> The size limit for sending ICMP/ICMPv6 packets to peers using the sl_si91x_send_ping API remains restricted to 300 bytes.
// <i> Default: Disable
#define SL_SI91X_WC_FEAT_LONG_ICMP_PACKET  0
#define SLI_SI91X_WC_FEAT_LONG_ICMP_PACKET (SL_SI91X_WC_FEAT_LONG_ICMP_PACKET << 12)

// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_TCP_IP_FEAT_SEND_CONFIGS_TO_HOST> Send Web Page Configs to Host(not supported)
// <i> This feature allows the device to send web page configuration data to the
// <i> host system from the wireless configuration page.
// <i> Enable or disable.
// <i> Default: Disable
#define SL_SI91X_WC_TCP_IP_FEAT_SEND_CONFIGS_TO_HOST  0
#define SLI_SI91X_WC_TCP_IP_FEAT_SEND_CONFIGS_TO_HOST (SL_SI91X_WC_TCP_IP_FEAT_SEND_CONFIGS_TO_HOST << 14)

// <q SL_SI91X_WC_EXT_FEAT_HTTP_OTAF_SUPPORT> Enables the HTTP OTA Support
// <i> This feature enables support for HTTP-based Over-The-Air Firmware (OTAF)
// <i> updates, that allows the device to download and install firmware updates
// <i> via HTTP.
// <i> @note To do firmware upgrade with HTTP this bit should be enabled.
// <i> Enable or disable.
// <i> Default: Disable
#define SL_SI91X_WC_EXT_FEAT_HTTP_OTAF_SUPPORT  0
#define SLI_SI91X_WC_EXT_FEAT_HTTP_OTAF_SUPPORT (SL_SI91X_WC_EXT_FEAT_HTTP_OTAF_SUPPORT << 18)

// <q SL_SI91X_WC_FEAT_LONG_HTTP_URL> Support Long HTTP GET & OTA URLs
// <i> Enables support for long HTTP GET and OTAF URLs. The maximum URL length supported is 2048 bytes.
// <i> Default: Disable
#define SL_SI91X_WC_FEAT_LONG_HTTP_URL  0
#define SLI_SI91X_WC_FEAT_LONG_HTTP_URL (SL_SI91X_WC_FEAT_LONG_HTTP_URL << 14)

// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_CUSTOM_FEAT_DNS_SERVER_IN_DHCP_OFFER> Include DNS Server IP in DHCP Offer (AP mode)(not supported)
// <i> Includes DNS server IP address in DHCP offer response when in AP mode. This feature is currently not supported.
// <i> When this bit is set, the DHCP server runs in AP mode and includes the DNS server IP address in the DHCP offer response sent to clients.
// <i> Enable or disable DNS server IP address in DHCP offer response in AP mode feature.
// <i> Default: Disable
#define SL_SI91X_WC_CUSTOM_FEAT_DNS_SERVER_IN_DHCP_OFFER  0
#define SLI_SI91X_WC_CUSTOM_FEAT_DNS_SERVER_IN_DHCP_OFFER (SL_SI91X_WC_CUSTOM_FEAT_DNS_SERVER_IN_DHCP_OFFER << 6)

// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_EXT_HTTP_SKIP_DEFAULT_LEADING_CHARACTER> Skip Default Leading '\' In HTTP Header(not supported)
// <i> Enabling this bit configures the device to omit the default leading character '\' in HTTP headers, allows for custom header formatting.
// <i> Enable or disable
// <i> Default: Disable
#define SL_SI91X_WC_EXT_HTTP_SKIP_DEFAULT_LEADING_CHARACTER  0
#define SLI_SI91X_WC_EXT_HTTP_SKIP_DEFAULT_LEADING_CHARACTER (SL_SI91X_WC_EXT_HTTP_SKIP_DEFAULT_LEADING_CHARACTER << 9)

// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_EXT_FEAT_HOMEKIT_WAC_ENABLED> Lock MDNS Text Record Modification (not supported)
// <i> To inform firmware not to modify mDNS text records.
// <i> Enabling this bit indicates that the firmware should not alter mDNS (Multicast DNS) text records.
// <i> Enable or disable
// <i> Default: Disable
#define SL_SI91X_WC_EXT_FEAT_HOMEKIT_WAC_ENABLED  0
#define SLI_SI91X_WC_EXT_FEAT_HOMEKIT_WAC_ENABLED (SL_SI91X_WC_EXT_FEAT_HOMEKIT_WAC_ENABLED << 24)
// </h>

// <h> Sockets
// <o SL_SI91X_WC_TCP_IP_TOTAL_SOCKETS> Sockets are Allowed <0-10>
// <i> This macro allows the configuration of the total number of sockets
// <i> available. A maximum of 10 sockets are allowed. Bits 21-24 are used
// <i> to set the TOTAL_SOCKETS.
// <i> Default: 0
#define SL_SI91X_WC_TCP_IP_TOTAL_SOCKETS  0
#define SLI_SI91X_WC_TCP_IP_TOTAL_SOCKETS (SL_SI91X_WC_TCP_IP_TOTAL_SOCKETS << 21)

// <q SL_SI91X_WC_EXT_TCP_IP_WAIT_FOR_SOCKET_CLOSE> Enable Socket Wait Close
// <i> This feature ensures that a socket is not closed until close() is called
// <i> from the host, which is recommended for use with TCP sockets.
// <i> @note If it is set socket would not be closed until close() is called from host. It is recommended to enable this bit when using TCP sockets.
// <i> @note If this bit is enabled, closing the socket will flush any pending data to be sent to remote peer.
// <i> @note If this bit is disabled, closing the socket will first send any pending data to remote peer.
// <i> @note This is always set internally for Si91x chips.
// <i> Enable or disable socket wait close feature.
// <i> Default: Disable
#define SL_SI91X_WC_EXT_TCP_IP_WAIT_FOR_SOCKET_CLOSE  0
#define SLI_SI91X_WC_EXT_TCP_IP_WAIT_FOR_SOCKET_CLOSE (SL_SI91X_WC_EXT_TCP_IP_WAIT_FOR_SOCKET_CLOSE << 16)

// <o SL_SI91X_WC_EXT_TCP_IP_TOTAL_SELECTS> Configure The Number of Selects <0-10>
// <i> This feature configures the number of select operations the device can
// <i> handle, with a maximum value of 10.
// <i> Default: 0
#define SL_SI91X_WC_EXT_TCP_IP_TOTAL_SELECTS  0
#define SLI_SI91X_WC_EXT_TCP_IP_TOTAL_SELECTS (SL_SI91X_WC_EXT_TCP_IP_TOTAL_SELECTS << 12)

// <q SL_SI91X_WC_TCP_IP_FEAT_RAW_DATA> Enable Raw Data Support
// <i> This feature allows the device to handle raw data frames, which bypass the
// <i> TCP/IP stack. It is supported only in AP mode and requires the TCP_BYPASS
// <i> feature to be disabled. If any packet from the host with frame type 0x1
// <i> is received by the firmware, the packet would be sent on air without
// <i> TCP/IP stack processing. ARP and broadcast packets (other than DHCP
// <i> packets) received on air would be sent to the host.
// <i> Enable or disable Raw data support.
// <i> Default: Disable
#define SL_SI91X_WC_TCP_IP_FEAT_RAW_DATA  0
#define SLI_SI91X_WC_TCP_IP_FEAT_RAW_DATA (SL_SI91X_WC_TCP_IP_FEAT_RAW_DATA << 18)

// <q SL_SI91X_WC_TCP_IP_FEAT_DTLS_THREE_SOCKETS> Enables Three DTLS Sockets.
// <i> This feature allows the device to support up to three simultaneous
// <i> DTLS (Datagram Transport Layer Security) connections.
// <i> Default: Disable
#define SL_SI91X_WC_TCP_IP_FEAT_DTLS_THREE_SOCKETS  0
#define SLI_SI91X_WC_TCP_IP_FEAT_DTLS_THREE_SOCKETS (SL_SI91X_WC_TCP_IP_FEAT_DTLS_THREE_SOCKETS << 29)

// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_CUSTOM_FEAT_REJECT_CONNECT_REQ_IMMEDIATELY> Reject New LTCP Connections When Max Clients Reached(not supported)
// <i> When bit is set, any new connection request for an LTCP socket would be rejected immediately if the maximum number of clients are connected already.
// <i> By default, such requests are maintained in a pending list until an existing client disconnects.
// <i> @note When BIT = 0: New connection requests are held in a pending list. When BIT = 1: New connection requests are immediately rejected.
// <i> Enable or disable rejecting connection request immediately feature.
// <i> Default: Disable
#define SL_SI91X_WC_CUSTOM_FEAT_REJECT_CONNECT_REQ_IMMEDIATELY 0
#define SLI_SI91X_WC_CUSTOM_FEAT_REJECT_CONNECT_REQ_IMMEDIATELY \
  (SL_SI91X_WC_CUSTOM_FEAT_REJECT_CONNECT_REQ_IMMEDIATELY << 26)
// </h>

// <h> Advanced TCP/IP Features
// <q SL_SI91X_WC_TCP_IP_FEAT_BYPASS> TCP/IP Bypass Feature
// <i> When this feature is enabled, the TCP/IP stack processing is bypassed,
// <i> allowing raw Ethernet frames to be sent and received. This is useful
// <i> for applications that require direct control over Ethernet frames or
// <i> for implementing custom network protocols.
// <i> This is defined as a bit flag that can be set in the feature configuration
// <i> to enable the TCP/IP bypass.
// <i> Enable or disable TCP/IP bypass feature.
// <i> Default: Disable
#define SL_SI91X_WC_TCP_IP_FEAT_BYPASS  0
#define SLI_SI91X_WC_TCP_IP_FEAT_BYPASS (SL_SI91X_WC_TCP_IP_FEAT_BYPASS << 0)

// <q SL_SI91X_WC_EXT_TCP_IP_DUAL_MODE_ENABLE> Enables Both TCP/IP Bypass Mode & Embedded Modes
// <i> This feature enables support for dual network stack mode, which allows two different network
// <i> stacks to run on the NWP and the external host simultaneously, which provides flexibility in
// <i> network communication.
// <i> @note "TCP/IP bypass feature" should not be set to 1 when this bit is enabled.
// <i> Enable or disable.
// <i> Default: Disable
#define SL_SI91X_WC_EXT_TCP_IP_DUAL_MODE_ENABLE  0
#define SLI_SI91X_WC_EXT_TCP_IP_DUAL_MODE_ENABLE (SL_SI91X_WC_EXT_TCP_IP_DUAL_MODE_ENABLE << 10)

// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_TCP_IP_FEAT_JSON_OBJECTS> Enable JSON Objects (not supported)
// <i>  This feature allows the device to handle JSON (JavaScript Object Notation)
// <i>  objects, which can be used for data interchange in web applications.
// <i> Enable or disable JSON objects.
// <i> Default: Disable
#define SL_SI91X_WC_TCP_IP_FEAT_JSON_OBJECTS  0
#define SLI_SI91X_WC_TCP_IP_FEAT_JSON_OBJECTS (SL_SI91X_WC_TCP_IP_FEAT_JSON_OBJECTS << 6)

// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_TCP_IP_FEAT_SMTP_CLIENT> Enable SMTP Client (not supported)
// <i> This feature allows the device to act as an SMTP (Simple Mail Transfer
// <i> Protocol) client, that enables it to send emails.
// <i> Enable or disable SMTP client.
// <i> Default: Disable
#define SL_SI91X_WC_TCP_IP_FEAT_SMTP_CLIENT  0
#define SLI_SI91X_WC_TCP_IP_FEAT_SMTP_CLIENT (SL_SI91X_WC_TCP_IP_FEAT_SMTP_CLIENT << 20)

// <q SL_SI91X_WC_TCP_IP_FEAT_OTAF> Enable OTAF Client
// <i> This feature allows the device to perform Over-The-Air Firmware (OTAF)
// <i> updates, enables it to download and install firmware updates remotely.
// <i> Enable or disable OTAF (Over-The-Air Firmware update) client.
// <i> Default: Disable
#define SL_SI91X_WC_TCP_IP_FEAT_OTAF  0
#define SLI_SI91X_WC_TCP_IP_FEAT_OTAF (SL_SI91X_WC_TCP_IP_FEAT_OTAF << 30)

// Enable or disable TCP IP Extension.
// This feature allows the device to use extended TCP/IP features,
// If this bit is enabled only then, the features present in the ext_tcp ip feature bitmap can be used.
// Default: Disable
#define SL_SI91X_WC_TCP_IP_FEAT_EXTENSION_VALID  (SL_SI91X_WC_EXTENDED_TCPIP_FEATURE_BITMAP ? 1 : 0)
#define SLI_SI91X_WC_TCP_IP_FEAT_EXTENSION_VALID (SL_SI91X_WC_TCP_IP_FEAT_EXTENSION_VALID << 31)

// <q SL_SI91X_WC_FEAT_FW_UPDATE_NEW_CODE> Unique Firmware Update Status Codes
// <i> This bit in the feature bitmap is used to inform the NWP firmware whether
// <i> the host supports a new set of result codes to differentiate firmware update
// <i> results from other non-firmware-related results. If this bit is set,
// <i> the NWP firmware would send result codes from the new set after a firmware update.
// <i> If the bit is not set, the legacy result codes would be used.
// <i> Default: Disable
#define SL_SI91X_WC_FEAT_FW_UPDATE_NEW_CODE  0
#define SLI_SI91X_WC_FEAT_FW_UPDATE_NEW_CODE (SL_SI91X_WC_FEAT_FW_UPDATE_NEW_CODE << 16)

// <q SL_SI91X_WC_EXT_TCP_IP_WINDOW_SCALING> Enable TCP Window Scaling Feature
// <i> This feature enables TCP window scaling, which allows the device to use
// <i> receive window sizes larger than 64 KB.
// <i> @note If this feature is not enabled, then the maximum possible RX window size is 64 KB.
// <i> If you want to use more than 64 KB window size, tcp_rx_window_size_cap in socket configuration is used to increase the window size.
// <i> Enable or disable TCP window scaling feature.
// <i> Default: Disable
#define SL_SI91X_WC_EXT_TCP_IP_WINDOW_SCALING  0
#define SLI_SI91X_WC_EXT_TCP_IP_WINDOW_SCALING (SL_SI91X_WC_EXT_TCP_IP_WINDOW_SCALING << 8)

// <q SL_SI91X_WC_EXT_DYNAMIC_COEX_MEMORY> Enables the Dynamic Coex Memory
// <i> This feature dynamically adjusts the TCP receive window size based on
// <i> coexistence requirements, that improves network performance in coexistence scenarios.
// <i> @note To enable or disable the coexistence and update TCP RX window accordingly.
// <i> Enable or disable.
// <i> Default: Disable
#define SL_SI91X_WC_EXT_DYNAMIC_COEX_MEMORY  0
#define SLI_SI91X_WC_EXT_DYNAMIC_COEX_MEMORY (SL_SI91X_WC_EXT_DYNAMIC_COEX_MEMORY << 11)

// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_EXT_TCP_MAX_RECV_LENGTH> Update Max TCP Receive Length(not supported)
// <i> This feature allows the maximum receive length for TCP connections to be
// <i> updated, accommodating different application requirements.
// <i> Enable or disable.
// <i> Default: Disable
#define SL_SI91X_WC_EXT_TCP_MAX_RECV_LENGTH  0
#define SLI_SI91X_WC_EXT_TCP_MAX_RECV_LENGTH (SL_SI91X_WC_EXT_TCP_MAX_RECV_LENGTH << 20)

// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_EXT_TCP_DYNAMIC_WINDOW_UPDATE_FROM_HOST> Enable TCP Dynamic Window Update (not supported)
// <i> This feature allows the TCP window size to be dynamically updated from
// <i> the host, that provides more control over TCP flow management.
// <i> Enable or disable updating max receive length for TCP feature.
// <i> Default: Disable
#define SL_SI91X_WC_EXT_TCP_DYNAMIC_WINDOW_UPDATE_FROM_HOST  0
#define SLI_SI91X_WC_EXT_TCP_DYNAMIC_WINDOW_UPDATE_FROM_HOST (SL_SI91X_WC_EXT_TCP_DYNAMIC_WINDOW_UPDATE_FROM_HOST << 19)

// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_EXT_TCP_IP_BI_DIR_ACK_UPDATE> TCP Bi-Directional Acknowledgment Update.(not supported)
// <i> This feature enables bi-directional data transfer by updating TCP acknowledgement handling.
// <i> @note Need to enable this bit if you want to run the bi-directional data transfer.
// <i> Enable or disable.
// <i> Default: Disable
#define SL_SI91X_WC_EXT_TCP_IP_BI_DIR_ACK_UPDATE  0
#define SLI_SI91X_WC_EXT_TCP_IP_BI_DIR_ACK_UPDATE (SL_SI91X_WC_EXT_TCP_IP_BI_DIR_ACK_UPDATE << 3)

// <q SL_SI91X_WC_EXT_TCP_IP_WINDOW_DIV> TCP RX Window Division
// <i> This feature allows the division of the TCP receive window, enables
// <i> more granular control over the window size.
// <i> Enable or disable.
// <i> Default: Disable
#define SL_SI91X_WC_EXT_TCP_IP_WINDOW_DIV  0
#define SLI_SI91X_WC_EXT_TCP_IP_WINDOW_DIV (SL_SI91X_WC_EXT_TCP_IP_WINDOW_DIV << 4)
// </h>
// </h>

// <h> General Bluetooth Support
// <o SL_SI91X_WC_BT_RF_TYPE> Select Bluetooth RF Type(Incorrect selection will result in the BLE advertising not being visible on air)
// <0=> Rf Type External
// <1=> RF TYPE Internal
// <i> Macro to specify the BT RF type for the SI91X wireless device.
// <i> This macro is used to specify the BT RF type for the SI91X wireless device.
// <i> BLE advertising visible on air
// <i> Enable or disable.
// <i> Default: Disable
#define SL_SI91X_WC_BT_RF_TYPE  0
#define SLI_SI91X_WC_BT_RF_TYPE (SL_SI91X_WC_BT_RF_TYPE << 30)
// </h>

// <e SL_SI91X_WC_ENABLE_BLE_PROTOCOL> BLE Features
//BLE Features depends on SL_SI91X_ENABLE_BLE_PROTOCOL
//This macro is used to enable the BLE (Bluetooth Low Energy) protocol.
//The BLE protocol can be enabled by setting the bit 31 of the corresponding register.
#define SL_SI91X_WC_ENABLE_BLE_PROTOCOL  0
#define SLI_SI91X_WC_ENABLE_BLE_PROTOCOL (SL_SI91X_WC_ENABLE_BLE_PROTOCOL << 31)

// <h>Number of connections
// <o SL_SI91X_WC_BLE_MAX_NBR_PERIPHERALS> Max Number of Peripherals <1-2>
// <i> Note: Refer to the releasenotes for the maximum number of supported peripherals
// <i> Default: 1
#define SL_SI91X_WC_BLE_MAX_NBR_PERIPHERALS  1
#define SLI_SI91X_WC_BLE_MAX_NBR_PERIPHERALS (SL_SI91X_WC_BLE_MAX_NBR_PERIPHERALS << 12)

// <o SL_SI91X_WC_BLE_MAX_NBR_CENTRALS> Max Number of Centrals <1-2>
// <i> Note:Refer to the releasenotes for the maximum number of supported centrals
// <i> Default: 1
#define SL_SI91X_WC_BLE_MAX_NBR_CENTRALS  1
#define SLI_SI91X_WC_BLE_MAX_NBR_CENTRALS (SL_SI91X_WC_BLE_MAX_NBR_CENTRALS << 27)
// </h>

// <h> GATT Server Configurations
// <o SL_SI91X_WC_BLE_MAX_NBR_ATT_REC> Max Number of Attributes <0-80>
// <i> BLE maximum number of attribute records
// <i> The maximum value allowed is 80.
// <i> Default: 20
#define SL_SI91X_WC_BLE_MAX_NBR_ATT_REC  20
#define SLI_SI91X_WC_BLE_MAX_NBR_ATT_REC (SL_SI91X_WC_BLE_MAX_NBR_ATT_REC << 0)

// <o SL_SI91X_WC_BLE_MAX_NBR_ATT_SERV> Max Number of GATT Services <0-10>
// <i> Note: The maximum value is 10.
// <i> Default: 10
#define SL_SI91X_WC_BLE_MAX_NBR_ATT_SERV  10
#define SLI_SI91X_WC_BLE_MAX_NBR_ATT_SERV (SL_SI91X_WC_BLE_MAX_NBR_ATT_SERV << 8)
// </h>

// <o SL_SI91X_WC_BLE_PWR_INX> BLE TX Power Index Value <0-75>
// <i> Sets the BLE Tx power index value.
// <i> - Default value for BLE Tx Power Index is 31.
// <i> - Range for the BLE Tx Power Index is 1 to 127 (0, 32 index is invalid).
// <i>   -  1 - 31: BLE  - 0 dBm Mode
// <i>   - 33 - 63: BLE  - 10 dBm Mode
// <i>   - 64 - 127: BLE - HP Mode
// <i> Default: 30
#define SL_SI91X_WC_BLE_PWR_INX  30
#define SLI_SI91X_WC_BLE_PWR_INX (SL_SI91X_WC_BLE_PWR_INX << 16)

// This macro is not supported and is not displayed in the UI.
// <i> <o SL_SI91X_WC_BLE_PWR_SAVE_OPTIONS> BLE Powersave Options <0-7> (not supported)
// <i> Default: 0
#define SL_SI91X_WC_BLE_PWR_SAVE_OPTIONS  0
#define SLI_SI91X_WC_BLE_PWR_SAVE_OPTIONS (SL_SI91X_WC_BLE_PWR_SAVE_OPTIONS << 24)

// <q SL_SI91X_WC_BLE_GATT_ASYNC_ENABLE> GATT Asynchronous Enable
// <i> @note Default is disabled. When enabled, the response structure would be filled in the Event, which would come later, not in sync with the response for the query command.
// <i> Enable or disable.
// <i> Default: Disable
#define SL_SI91X_WC_BLE_GATT_ASYNC_ENABLE  0
#define SLI_SI91X_WC_BLE_GATT_ASYNC_ENABLE (SL_SI91X_WC_BLE_GATT_ASYNC_ENABLE << 29)

// <q SL_SI91X_WC_916_BLE_COMPATIBLE_FEAT_ENABLE> BLE Compatible Feature Enable
// <i> Enables new feature compatible.
// <i> @note This bit should be enable to get the set of events from controller for the new features.
// <i> Enable or disable.
// <i> Default: Disable
#define SL_SI91X_WC_916_BLE_COMPATIBLE_FEAT_ENABLE  0
#define SLI_SI91X_WC_916_BLE_COMPATIBLE_FEAT_ENABLE (SL_SI91X_WC_916_BLE_COMPATIBLE_FEAT_ENABLE << 30)

// </e>

// <e SL_SI91X_WC_FEAT_BLE_CUSTOM_FEAT_EXTENTION_VALID> Extended BLE Custom Features
// Extended BLE Custom Features depend on SL_SI91X_FEAT_BLE_CUSTOM_FEAT_EXTENSION_VALID
// This macro will be automatically enabled when any of the BLE Custom Features are set
#define SL_SI91X_WC_FEAT_BLE_CUSTOM_FEAT_EXTENTION_VALID  0
#define SLI_SI91X_WC_FEAT_BLE_CUSTOM_FEAT_EXTENTION_VALID (SL_SI91X_WC_FEAT_BLE_CUSTOM_FEAT_EXTENTION_VALID << 31)

// <o SL_SI91X_WC_BLE_NUM_CONN_EVENTS> Number of Connection Events <0-31>
// <i> Describes the number of buffers that need to be allocated for BLE on the opermode.
// <i> - By default, each role (central/peripheral) will be allocated with 1 buffer for the notify/write command.
// <i> - Increasing the buffer capacity for the notify/write commands helps achieve the best throughput.
// <i> - See rsi_ble_set_wo_resp_notify_buf_info() to set more buffers for the notify/write commands.
// <i> Note: value of SL_SI91X_BLE_NUM_CONN_EVENTS equal to:(SL_SI91X_BLE_NUM_CONN_EVENTS)RSI_BLE_NUM_CONN_EVENTS =
// <i> (RSI_BLE_MAX_NBR_PERIPHERALS + RSI_BLE_MAX_NBR_CENTRALS) + 2 *(TX_NOTIFICATIONS_TO_C1 + TX_NOTIFICATIONS_TO_C2 + TX_NOTIFICATIONS_TO_P1 + TX_NOTIFICATIONS_TO_P2 + TX_NOTIFICATIONS_TO_P3)
// <i> Default: 4
#define SL_SI91X_WC_BLE_NUM_CONN_EVENTS  4
#define SLI_SI91X_WC_BLE_NUM_CONN_EVENTS (SL_SI91X_WC_BLE_NUM_CONN_EVENTS << 0)

// <o SL_SI91X_WC_BLE_NUM_REC_BYTES> Number of Record Bytes <0-124>
// <i> Specifies the number of record bytes in multiples of 16.
// <i> - n*16 : (n=60, Default 1024 bytes (1K)).
// <i> eg:(64*16)=1024 bytes
// <i> Default: 64
#define SL_SI91X_WC_BLE_NUM_REC_BYTES  64
#define SLI_SI91X_WC_BLE_NUM_REC_BYTES (SL_SI91X_WC_BLE_NUM_REC_BYTES << 5)

// <h> Host-Driven Configuration
// <o SL_SI91X_WC_BLE_GATT_INIT> GATT Initialization Mode
// <0=> GATT Init in Firmware
// <1=> GATT Init in Host
// <i> Specifies whether the GATT is initialized in firmware or by the host.
// <i> - 0: GATT Init in Firmware. Both the GAP service and GATT service would be maintained by the firmware.
// <i> - 1: GATT Init in Host. The GAP service and GATT service should be created by the APP/Host/User, and the ATT transactions like read, write, notify, and indicates to be handled by the APP/Host/User.
// <i> - Default: GATT Init in Firmware.
#define SL_SI91X_WC_BLE_GATT_INIT  0
#define SLI_SI91X_WC_BLE_GATT_INIT (SL_SI91X_WC_BLE_GATT_INIT << 13)

// <q SL_SI91X_WC_BLE_INDICATE_CONFIRMATION_FROM_HOST> Indicate Confirmation from Host
// <i> As per the ATT protocol, every indication received from the server should be acknowledged (indication response) by the client.
// <i> - If this bit is disabled, the firmware sends the acknowledgment (indication response).
// <i> - If this bit is enabled, the APP/Host/User needs to send the acknowledgement (indication response).
// <i> Enable or Disable.
// <i> Default: Disable
#define SL_SI91X_WC_BLE_INDICATE_CONFIRMATION_FROM_HOST  0
#define SLI_SI91X_WC_BLE_INDICATE_CONFIRMATION_FROM_HOST (SL_SI91X_WC_BLE_INDICATE_CONFIRMATION_FROM_HOST << 14)

// <q SL_SI91X_WC_BLE_MTU_EXCHANGE_FROM_HOST> MTU Exchange from Host
// <i> - If this bit is disabled, the firmware initiates the MTU request to the remote device on a successful connection.
// <i> - If the peer initiates an MTU exchange request, the firmware will send an Exchange MTU Response in reply to the received Exchange MTU Request.
// <i> - If this bit is enabled, the APP/Host/User needs to initiate the MTU request using the rsi_ble_mtu_exchange_event API.
// <i> - If the peer initiates an MTU exchange request, the APP/Host/User shall send an Exchange MTU Response using the rsi_ble_mtu_exchange_resp API.
// <i> Enable or Disable.
// <i> Default: Disable
#define SL_SI91X_WC_BLE_MTU_EXCHANGE_FROM_HOST  0
#define SLI_SI91X_WC_BLE_MTU_EXCHANGE_FROM_HOST (SL_SI91X_WC_BLE_MTU_EXCHANGE_FROM_HOST << 15)

// <q SL_SI91X_WC_BLE_SET_SCAN_RESP_DATA_FROM_HOST> Set Scan Response Data from Host
// <i> The device would maintain some default scan response data to be used in the scan_response controller frame.
// <i> - Enabling this bit would make the default data Null (empty).
// <i> Enable or Disable.
// <i> Default: Disable
#define SL_SI91X_WC_BLE_SET_SCAN_RESP_DATA_FROM_HOST  0
#define SLI_SI91X_WC_BLE_SET_SCAN_RESP_DATA_FROM_HOST (SL_SI91X_WC_BLE_SET_SCAN_RESP_DATA_FROM_HOST << 16)

// <q SL_SI91X_WC_BLE_DISABLE_CODED_PHY_FROM_HOST> Disable Coded PHY from Host
// <i> The device supports the LE-coded PHY feature (that is, LR - 125 kbps, and 500 kbps) by default.
// <i> - If this bit is enabled, the device would not support the LE-coded PHY rates.
// <i> Enable or Disable.
// <i> Default: Disable
#define SL_SI91X_WC_BLE_DISABLE_CODED_PHY_FROM_HOST  0
#define SLI_SI91X_WC_BLE_DISABLE_CODED_PHY_FROM_HOST (SL_SI91X_WC_BLE_DISABLE_CODED_PHY_FROM_HOST << 17)
// </h>

// <e SL_SI91X_WC_BLE_ENABLE_ADV_EXTN> Advertising Extensions
// <i> Enable or Disable.
// <i> Default: Disable
#define SL_SI91X_WC_BLE_ENABLE_ADV_EXTN  0
#define SLI_SI91X_WC_BLE_ENABLE_ADV_EXTN (SL_SI91X_WC_BLE_ENABLE_ADV_EXTN << 19)

// <o SL_SI91X_WC_BLE_AE_MAX_ADV_SETS> Max Number of AE Advertising Sets
// <0-2> Number Of Max AE Adv Sets
// <i> Configures the maximum number of AE advertising sets.
// <i> - Maximum number of AE advertising sets is 2.
// <i> Default: 2
#define SL_SI91X_WC_BLE_AE_MAX_ADV_SETS  2
#define SLI_SI91X_WC_BLE_AE_MAX_ADV_SETS (SL_SI91X_WC_BLE_AE_MAX_ADV_SETS << 20)
// </e>
// </e>

// <h> Security Configurations
// <h> TLS/SSL
// <q SL_SI91X_WC_FEAT_SSL_HIGH_STREAMING_BIT> High SSL Streaming Throughput
// <i> Enables high throughput for Secure Sockets Layer (SSL) streaming
// <i> Default: Disable
#define SL_SI91X_WC_FEAT_SSL_HIGH_STREAMING_BIT  0
#define SLI_SI91X_WC_FEAT_SSL_HIGH_STREAMING_BIT (SL_SI91X_WC_FEAT_SSL_HIGH_STREAMING_BIT << 10)

// <q SL_SI91X_WC_EXT_TCP_IP_SSL_16K_RECORD> SSL 16K Record Size Support
// <i> This feature enables support for 16K SSL record sizes, which improves
// <i> performance for SSL connections that use larger record sizes.
// <i> Enable or disable.
// <i> Default: Disable
#define SL_SI91X_WC_EXT_TCP_IP_SSL_16K_RECORD  0
#define SLI_SI91X_WC_EXT_TCP_IP_SSL_16K_RECORD (SL_SI91X_WC_EXT_TCP_IP_SSL_16K_RECORD << 6)

// <q SL_SI91X_WC_EXT_FEAT_SSL_VERSIONS_SUPPORT> Enable Multi-Version SSL/TLS Support
// <i> Enabling this bit allows the device to support multiple versions of SSL/TLS over TCP, providing flexibility in handling different SSL/TLS versions.
// <i> Enable or disable
// <i> Default: Disable
#define SL_SI91X_WC_EXT_FEAT_SSL_VERSIONS_SUPPORT  0
#define SLI_SI91X_WC_EXT_FEAT_SSL_VERSIONS_SUPPORT (SL_SI91X_WC_EXT_FEAT_SSL_VERSIONS_SUPPORT << 14)

// <q SL_SI91X_WC_EXT_FEAT_SSL_CERT_WITH_4096_KEY_SUPPORT> Support 4096 Size KEY SSL Certificate
// <i> Enabling this bit allows the device to support SSL certificates with 4096-bit keys. Recommended only if 4096-bit keys are required.
// <i> Enable or disable
// <i> Default: Disable
#define SL_SI91X_WC_EXT_FEAT_SSL_CERT_WITH_4096_KEY_SUPPORT  0
#define SLI_SI91X_WC_EXT_FEAT_SSL_CERT_WITH_4096_KEY_SUPPORT (SL_SI91X_WC_EXT_FEAT_SSL_CERT_WITH_4096_KEY_SUPPORT << 3)

// <q SL_SI91X_WC_EXT_FEAT_RSA_KEY_WITH_4096_SUPPORT> Support 4096 Size RSA KEY Certificate
// <i> Enabling this bit allows the device to support 4096-bit RSA keys. Recommended only if 4096-bit keys are required.
// <i> Enable or disable
// <i> Default: Disable
#define SL_SI91X_WC_EXT_FEAT_RSA_KEY_WITH_4096_SUPPORT  0
#define SLI_SI91X_WC_EXT_FEAT_RSA_KEY_WITH_4096_SUPPORT (SL_SI91X_WC_EXT_FEAT_RSA_KEY_WITH_4096_SUPPORT << 1)

// <q SL_SI91X_WC_TCP_IP_FEAT_LOAD_PUBLIC_PRIVATE_CERTS> Load Public/Private Keys For TLS/SSL Handshake
// <i> This feature allows the device to load public and private keys for use in TLS/SSL handshakes.
// <i> @note If a secure handshake is to be done using only a CA-certificate, then loading of private and public keys can be
// <i> disabled, and these certificates can be erased from the flash using the load_cert API. If secure handshake verification of private and
// <i> public keys is needed, then loading of these keys must be enabled.
// <i> Enable or disable.
// <i> Default: Disable
#define SL_SI91X_WC_TCP_IP_FEAT_LOAD_PUBLIC_PRIVATE_CERTS  0
#define SLI_SI91X_WC_TCP_IP_FEAT_LOAD_PUBLIC_PRIVATE_CERTS (SL_SI91X_WC_TCP_IP_FEAT_LOAD_PUBLIC_PRIVATE_CERTS << 26)

// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_EXT_TCP_IP_CERT_BYPASS> SSL Server Certificate Bypass (not supported)
// <i> This feature allows the device to bypass SSL server certificate validation, with validation being performed by the host instead.
// <i> Enable or disable SSL server certificate bypass feature.
// <i> Default: Disable
#define SL_SI91X_WC_EXT_TCP_IP_CERT_BYPASS  0
#define SLI_SI91X_WC_EXT_TCP_IP_CERT_BYPASS (SL_SI91X_WC_EXT_TCP_IP_CERT_BYPASS << 5)

// <q SL_SI91X_WC_EXT_TCP_IP_FEAT_SSL_THREE_SOCKETS> Enable Three SSL/TLS Sockets
// <i> This feature allows the device to support up to three simultaneous SSL/TLS connections.
// <i> Enable or disable three SSL/TLS sockets feature.
// <i> Default: Disable
#define SL_SI91X_WC_EXT_TCP_IP_FEAT_SSL_THREE_SOCKETS  0
#define SLI_SI91X_WC_EXT_TCP_IP_FEAT_SSL_THREE_SOCKETS (SL_SI91X_WC_EXT_TCP_IP_FEAT_SSL_THREE_SOCKETS << 29)

// <q SL_SI91X_WC_EXT_TCP_IP_FEAT_SSL_MEMORY_CLOUD> Additional Memory for SSL/TLS Cloud Connections
// <i> This feature allocates additional memory for SSL/TLS connections,
// <i> typically required for connections to cloud servers, to avoid 0xD2 error.
// <i>  @note If user connects to a cloud server using two SSL/TLS connections then it is required to set this bit to avoid 0xD2 error.
// <i> Enable or disable.
// <i> Default: Disable
#define SL_SI91X_WC_EXT_TCP_IP_FEAT_SSL_MEMORY_CLOUD  0
#define SLI_SI91X_WC_EXT_TCP_IP_FEAT_SSL_MEMORY_CLOUD (SL_SI91X_WC_EXT_TCP_IP_FEAT_SSL_MEMORY_CLOUD << 30)

// <q SL_SI91X_WC_TCP_IP_FEAT_SINGLE_SSL_SOCKET> Enable Single SSL Socket
// <i> This feature allows the device to use a single SSL socket for secure
// <i> communication.
// <i> Enable or disable Single SSL socket.
// <i> Default: Disable
#define SL_SI91X_WC_TCP_IP_FEAT_SINGLE_SSL_SOCKET  0
#define SLI_SI91X_WC_TCP_IP_FEAT_SINGLE_SSL_SOCKET (SL_SI91X_WC_TCP_IP_FEAT_SINGLE_SSL_SOCKET << 25)

// <q SL_SI91X_WC_TCP_IP_FEAT_LOAD_CERTS_INTO_RAM> Load SSL Certificates into RAM
// <i> This feature allows the device to load SSL certificates into RAM for
// <i> faster access during secure communications.
// <i> Enable or disable
// <i> Default: Disable
#define SL_SI91X_WC_TCP_IP_FEAT_LOAD_CERTS_INTO_RAM  0
#define SLI_SI91X_WC_TCP_IP_FEAT_LOAD_CERTS_INTO_RAM (SL_SI91X_WC_TCP_IP_FEAT_LOAD_CERTS_INTO_RAM << 27)
// </h>

// <h> Other Security Features
// <q SL_SI91X_WC_FEAT_SECURE_ATTESTATION> Secure Attestation
// <i> Enables secure attestation functionality
// <i> Default: Disable
#define SL_SI91X_WC_FEAT_SECURE_ATTESTATION  0
#define SLI_SI91X_WC_FEAT_SECURE_ATTESTATION (SL_SI91X_WC_FEAT_SECURE_ATTESTATION << 30)

// <q SL_SI91X_WC_CUSTOM_FEAT_HTTP_HTTPS_AUTH> Bit to Set HTTP Authentication
// <i> This bit enables authentication for HTTP and HTTPS connections, adding an extra layer of security for web-based communications.
// <i> Enable or disable setting HTTP Authentication feature.
// <i> Default: Disable
#define SL_SI91X_WC_CUSTOM_FEAT_HTTP_HTTPS_AUTH  0
#define SLI_SI91X_WC_CUSTOM_FEAT_HTTP_HTTPS_AUTH (SL_SI91X_WC_CUSTOM_FEAT_HTTP_HTTPS_AUTH << 23)

// <q SL_SI91X_WC_EXT_FEAT_PUF> To Enable PUF
// <i> Enabling this bit activates the Physical Unclonable Function feature, which provides a unique identifier for each device based on its physical characteristics.
// <i> Enable or disable
// <i> Default: Disable
#define SL_SI91X_WC_EXT_FEAT_PUF  0
#define SLI_SI91X_WC_EXT_FEAT_PUF (SL_SI91X_WC_EXT_FEAT_PUF << 7)

// <q SL_SI91X_WC_EXT_FEAT_PUF_PRIVATE_KEY> To Enable PUF Private Key
// <i> Enabling this bit activates the use of a private key associated with the PUF feature for enhanced security.
// <i> Enable or disable
// <i> Default: Disable
#define SL_SI91X_WC_EXT_FEAT_PUF_PRIVATE_KEY  0
#define SLI_SI91X_WC_EXT_FEAT_PUF_PRIVATE_KEY (SL_SI91X_WC_EXT_FEAT_PUF_PRIVATE_KEY << 10)
// </h>
// </h>

// <h> Power Management
// <h> Sleep & Wake-Up Mechanisms
// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_FEAT_LP_GPIO_BASED_HANDSHAKE> Low Power GPIO Handshake(not supported)
// <i> @details Enables GPIO-based handshake for low power mode
// <i> @note Not applicable for SiWx91x.
// <i> Default: Disable
#define SL_SI91X_WC_FEAT_LP_GPIO_BASED_HANDSHAKE  0
#define SLI_SI91X_WC_FEAT_LP_GPIO_BASED_HANDSHAKE (SL_SI91X_WC_FEAT_LP_GPIO_BASED_HANDSHAKE << 3)

// <q SL_SI91X_WC_FEAT_ULP_GPIO_BASED_HANDSHAKE> Ultra Low Power GPIO Handshake
// <i> Enables GPIO-based handshake for ultra low power mode
// <i> @note Not applicable for SoC
// <i> Default: Disable
#define SL_SI91X_WC_FEAT_ULP_GPIO_BASED_HANDSHAKE  0
#define SLI_SI91X_WC_FEAT_ULP_GPIO_BASED_HANDSHAKE (SL_SI91X_WC_FEAT_ULP_GPIO_BASED_HANDSHAKE << 4)

// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_FEAT_DEV_TO_HOST_ULP_GPIO_1> ULP GPIO 1 Wake-Up Indication(not supported)
// <i> Configures ULP GPIO 1 for wake-up indication.
// <i> Default: Disable
#define SL_SI91X_WC_FEAT_DEV_TO_HOST_ULP_GPIO_1  0
#define SLI_SI91X_WC_FEAT_DEV_TO_HOST_ULP_GPIO_1 (SL_SI91X_WC_FEAT_DEV_TO_HOST_ULP_GPIO_1 << 5)

// <q SL_SI91X_WC_EXT_FEAT_WOWLAN_DISABLE> Disable WoWLAN
// <i> To disable the WoWLAN (Wake-on-Wireless-LAN) feature.
// <i> Enabling this bit disables the WoWLAN feature, which is used for waking the device from a low-power state through wireless network activity.
// <i> By default WOW LAN Is enabled to maintain backward compatibility. So given option to disable this feature.
// <i> @note This only valid in NCP mode.
// <i> To disable the WoWLAN (Wake-on-Wireless-LAN) feature
// <i> This only valid in NCP mode.
// <i> Default: Disable
#define SL_SI91X_WC_EXT_FEAT_WOWLAN_DISABLE  0
#define SLI_SI91X_WC_EXT_FEAT_WOWLAN_DISABLE (SL_SI91X_WC_EXT_FEAT_WOWLAN_DISABLE << 17)

// <q SL_SI91X_WC_FEAT_SLEEP_GPIO_SEL_BITMAP> Wakeup Indication to host
// <i> Selects the GPIO for wakeup indication to the host.
// <i> When this bit is disabled, UULP_GPIO_3 is used as the wakeup indication. When enabled, UULP_GPIO_0 is used instead.
// <i> Enable or disable
// <i> Default: Disable
#define SL_SI91X_WC_FEAT_SLEEP_GPIO_SEL_BITMAP  0
#define SLI_SI91X_WC_FEAT_SLEEP_GPIO_SEL_BITMAP (SL_SI91X_WC_FEAT_SLEEP_GPIO_SEL_BITMAP << 0)
// </h>

// <h> Low Power Features
// <q SL_SI91X_WC_ENABLE_ENHANCED_MAX_PSP> Enable Enhanced Max PSP
// <i> Set this bit to enable the Enhanced Max PSP feature for improved performance.
// <i> Enable or disable
// <i> Default: Disable
#define SL_SI91X_WC_ENABLE_ENHANCED_MAX_PSP  0
#define SLI_SI91X_WC_ENABLE_ENHANCED_MAX_PSP (SL_SI91X_WC_ENABLE_ENHANCED_MAX_PSP << 26)

// <q SL_SI91X_WC_EXT_FEAT_LOW_POWER_MODE> Enable Low Power Mode in Wlan
// <i> Enabling this bit activates low power mode for WLAN, Active current would also be reduced.
// <i> As most of the code which is needed to maintain connection is kept in RAM.
// <i> There would be minimal execution of code from Flash which in turn results in low average current.
// <i> Enable or disable
// <i> Default: Disable
#define SL_SI91X_WC_EXT_FEAT_LOW_POWER_MODE  0
#define SLI_SI91X_WC_EXT_FEAT_LOW_POWER_MODE (SL_SI91X_WC_EXT_FEAT_LOW_POWER_MODE << 19)
// </h>

#ifdef SI91X_32kHz_EXTERNAL_OSCILLATOR
#define SL_SI91X_WC_EXT_FEAT_XTAL_CLK_ENABLE(xtal_clk_enable) (xtal_clk_enable << 23)
#else
#define SL_SI91X_WC_EXT_FEAT_XTAL_CLK_ENABLE(xtal_clk_enable) (xtal_clk_enable << 22)
#endif

#if defined(SLI_SI917) || defined(SLI_SI915) && defined(SLI_SI91X_MCU_CONFIG_RADIO_BOARD_VER2)
#define SLI_SI91X_WC_EXT_FEAT_XTAL_CLK SL_SI91X_WC_EXT_FEAT_XTAL_CLK_ENABLE(1)
#else
#define SLI_SI91X_WC_EXT_FEAT_XTAL_CLK SL_SI91X_WC_EXT_FEAT_XTAL_CLK_ENABLE(2)
#endif
// </h>

// <h> Performance & Memory Configurations

// <o SL_SI91X_WC_MEMORY_CONFIG> Memory Configuration
// <0=> Default (Compatible with both SOC Mode and NCP Mode)
// <1=> Allocates 352K Memory to NWP and 320K to M4 (SOC Mode Only)
// <2=> Allocates 416K Memory to NWP and 256K to M4 (SOC Mode Only)
// <3=> Allocates 480K Memory to NWP and 192K to M4 (SOC Mode Only)
// <4=> Allocates 352K memory to NWP (NCP Mode Only)
// <5=> Allocates 672K memory to NWP (NCP Mode Only)
// <i> Select the appropriate memory configuration based on the mode of operation.
// <i> Choosing an incorrect configuration may lead to unexpected behavior.
// <i> Default: Default
#define SL_SI91X_WC_MEMORY_CONFIG 0

// If the SL_SI91X_WC_MEMORY_CONFIG macro is set to a value between 0 and 5, but the selected device mode is incompatible
// (e.g., values 0 to 3 are chosen for NCP mode, or vice versa) then SL_SI91X_WC_MEMORY_CONFIG will default to MEMORY_CONFIG.
// If the value is outside the range of 0-5, a preprocessor error (#error "Invalid") will be triggered during compilation.
#if SL_SI91X_WC_MEMORY_CONFIG == 0
#define SLI_SI91X_WC_MEMORY_CONFIG MEMORY_CONFIG
#elif SL_SI91X_WC_MEMORY_CONFIG == 1
#define SLI_SI91X_WC_MEMORY_CONFIG \
  (SLI_SI91X_MCU_INTERFACE         \
     ? 0                           \
     : MEMORY_CONFIG) //SLI_SI91X_WC_EXT_FEAT_352K_M4SS_320K Allocates 352K memory to NWP and 320K to M4
#elif SL_SI91X_WC_MEMORY_CONFIG == 2
#define SLI_SI91X_WC_MEMORY_CONFIG \
  (SLI_SI91X_MCU_INTERFACE         \
     ? BIT(21)                     \
     : MEMORY_CONFIG) //SLI_SI91X_WC_EXT_FEAT_416K_M4SS_256K Allocates 416K memory to NWP and 256K to M4.
#elif SL_SI91X_WC_MEMORY_CONFIG == 3
#define SLI_SI91X_WC_MEMORY_CONFIG \
  (SLI_SI91X_MCU_INTERFACE         \
     ? BIT(20)                     \
     : MEMORY_CONFIG) //SLI_SI91X_WC_EXT_FEAT_480K_M4SS_192K Allocates 480K memory to NWP and 192K to M4
#elif SL_SI91X_WC_MEMORY_CONFIG == 4
#define SLI_SI91X_WC_MEMORY_CONFIG \
  (SLI_SI91X_MCU_INTERFACE ? MEMORY_CONFIG : 0) //SLI_SI91X_WC_EXT_FEAT_352K Allocates 352K memory to NWP
#elif SL_SI91X_WC_MEMORY_CONFIG == 5
#define SLI_SI91X_WC_MEMORY_CONFIG         \
  (SLI_SI91X_MCU_INTERFACE ? MEMORY_CONFIG \
                           : (BIT(20) | BIT(21))) //SLI_SI91X_WC_EXT_FEAT_672K Allocates 672K memory to NWP
#else
#error "Invalid"
#endif

// <h> CPU & Clock Speed
// <o SL_SI91X_WC_CUSTOM_FEAT_SOC_CLK_CONFIG> NWP SOC Clock Configuration
// <0=> Disable
// <1=> Configure NWP SOC Clock to 160 MHz
// <2=> Configure NWP SOC Clock to 120 MHz
// <i> This configuration sets the System-on-Chip (SoC) clock to either 160 MHz or 120 MHz based on the selected option.
// <i> If higher performance, such as increased throughput, is required, select 160 MHz.
// <i> Ensure to set `pll_mode` to 1 in the feature frame command for this configuration to take effect.
// <i> Default: Disable
#define SL_SI91X_WC_CUSTOM_FEAT_SOC_CLK_CONFIG 0

#if SL_SI91X_WC_CUSTOM_FEAT_SOC_CLK_CONFIG == 1
#define SLI_SI91X_WC_CUSTOM_FEAT_SOC_CLK_CONFIG_160MHZ (1 << 4)
#define SLI_SI91X_WC_CUSTOM_FEAT_SOC_CLK_CONFIG_120MHZ 0
#elif SL_SI91X_WC_CUSTOM_FEAT_SOC_CLK_CONFIG == 2
#define SLI_SI91X_WC_CUSTOM_FEAT_SOC_CLK_CONFIG_160MHZ 0
#define SLI_SI91X_WC_CUSTOM_FEAT_SOC_CLK_CONFIG_120MHZ (1 << 24)
#else
#define SLI_SI91X_WC_CUSTOM_FEAT_SOC_CLK_CONFIG_160MHZ 0
#define SLI_SI91X_WC_CUSTOM_FEAT_SOC_CLK_CONFIG_120MHZ 0
#endif
// </h>

// <h> RF Front End Switch Configuration
// <o SL_SI91X_WC_FRONT_END_SWITCH_CONFIG> Front-End Switch Configuration
// <0=> Disable
// <1=> ULP_GPIO 4, ULP_GPIO 5, ULP_GPIO 0
// <2=> Internal Switch
// <i> Default: Disable
#define SL_SI91X_WC_FRONT_END_SWITCH_CONFIG 0

#if SL_SI91X_WC_FRONT_END_SWITCH_CONFIG == 0
#define SL_SI91X_WC_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0 0
#define SL_SI91X_WC_EXT_FEAT_FRONT_END_INTERNAL_SWITCH            0
#elif SL_SI91X_WC_FRONT_END_SWITCH_CONFIG == 1
#define SL_SI91X_WC_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0 1
#define SL_SI91X_WC_EXT_FEAT_FRONT_END_INTERNAL_SWITCH            0
#elif SL_SI91X_WC_FRONT_END_SWITCH_CONFIG == 2
#define SL_SI91X_WC_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0 0
#define SL_SI91X_WC_EXT_FEAT_FRONT_END_INTERNAL_SWITCH            1
#elif SL_SI91X_WC_FRONT_END_SWITCH_CONFIG == 3
#define SL_SI91X_WC_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0 1
#define SL_SI91X_WC_EXT_FEAT_FRONT_END_INTERNAL_SWITCH            1
#else
#error "Invalid"
#endif

#define SLI_SI91X_WC_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0 \
  (SL_SI91X_WC_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0 << 29)
#define SLI_SI91X_WC_EXT_FEAT_FRONT_END_INTERNAL_SWITCH (SL_SI91X_WC_EXT_FEAT_FRONT_END_INTERNAL_SWITCH << 30)
// </h>

// <h> Clock Configuration
// <q SL_SI91X_WC_EXT_FEAT_NWP_QSPI_80MHZ_CLK_ENABLE> NWP QSPI 80MHz Clock Enable
// <i> Enabling this bit switches the NWP QSPI clock from the default 40 MHz to 80 MHz. By increasing the QSPI clock speed, flash execution accelerates, resulting in improved throughput (in cases where flash execution is involved).
// <i> Default: Disable
#define SL_SI91X_WC_EXT_FEAT_NWP_QSPI_80MHZ_CLK_ENABLE  0
#define SLI_SI91X_WC_EXT_FEAT_NWP_QSPI_80MHZ_CLK_ENABLE (SL_SI91X_WC_EXT_FEAT_NWP_QSPI_80MHZ_CLK_ENABLE << 8)
// </h>

// <h> XTAL Clock Configurations
// This macro is not supported and is not displayed in the UI.
// <i> <o SL_SI91X_WC_XTAL_GOODTIME> XTAL Goodtime Configurations(not supported)
// <i> <SLI_DISABLE=> Disable
// <i> <SL_SI91X_XTAL_GOODTIME_1000us=> Enable 1000us XTAL Good Time
// <i> <SL_SI91X_XTAL_GOODTIME_2000us=> Enable 2000us XTAL Good Time
// <i> <SL_SI91X_XTAL_GOODTIME_3000us=> Enable 3000us XTAL Good Time
// <i> <SL_SI91X_XTAL_GOODTIME_600us=> Enable 600us XTAL Good Time
// <i> @note Not applicable for SiWx91x.
// <i> Default: Disable
#define SL_SI91X_WC_XTAL_GOODTIME SLI_DISABLE

// <q SL_SI91X_WC_EXT_FEAT_DISABLE_XTAL_CORRECTION> Disable Auto Correction of XTAL (40 MHz crystal)
// <i> To disable auto correction of XTAL (40 MHz crystal)
// <i> Enabling this bit would disable the automatic compensation for frequency offsets, ensuring error-free calibration.
// <i> @note This bit should be enabled in the following cases:
// <i> @note 1. Always enable it in the Calibration application.
// <i> @note 2. Enable it for all applications for the customer hardware with an XTAL part number other than 8Y40070013.
// <i> Default: Disable
#define SL_SI91X_WC_EXT_FEAT_DISABLE_XTAL_CORRECTION  0
#define SLI_SI91X_WC_EXT_FEAT_DISABLE_XTAL_CORRECTION (SL_SI91X_WC_EXT_FEAT_DISABLE_XTAL_CORRECTION << 18)
// </h>

// <h> RTC
// <q SL_SI91X_WC_CUSTOM_FEAT_RTC_FROM_HOST> Real Time Clock from Host
// <i> Enables Real-Time Clock (RTC) synchronization from the host.
// <i> When bit is set, the module uses the Real-Time Clock (RTC) provided by the host system for timekeeping.
// <i> @note Ensures the Real-Time Clock (RTC) timer is configured to enable certificate validation.
// <i> Enable or disable receiving real time clock from host feature.
// <i> Default: Disable
#define SL_SI91X_WC_CUSTOM_FEAT_RTC_FROM_HOST  0
#define SLI_SI91X_WC_CUSTOM_FEAT_RTC_FROM_HOST (SL_SI91X_WC_CUSTOM_FEAT_RTC_FROM_HOST << 28)
// </h>

// <i> <h> Dynamic Voltage Scaling (DVS) Configurations
// This macro is not supported and is not displayed in the UI.
// <i> <o SL_SI91X_WC_FEAT_DVS_SEL_CONFIG> DVS Dynamic Voltage Selection(not supported)
// <i> <SLI_DISABLE=> Disable
// <i> <SL_SI91X_FEAT_DVS_SEL_CONFIG_1=> Enable Dynamic Voltage selection configuration 1
// <i> <SL_SI91X_FEAT_DVS_SEL_CONFIG_2=> Enable Dynamic Voltage selection configuration 2
// <i> <SL_SI91X_FEAT_DVS_SEL_CONFIG_3=> Enable Dynamic Voltage selection configuration 3
// <i> <SL_SI91X_FEAT_DVS_SEL_CONFIG_4=> Enable Dynamic Voltage selection configuration 4
// <i> configures the dynamic voltage selection for the system
// <i> @note Not applicable for SiWx91x.
// <i> Default: Disable
#define SL_SI91X_WC_FEAT_DVS_SEL_CONFIG SLI_DISABLE
// <i> </h>

// <i> <h> External Power Management (PMU) Configurations
// This macro is not supported and is not displayed in the UI.
// <i> <o SL_SI91X_WC_EXTERNAL_PMU_GOOD_TIME> External PMU Selection(not supported)
// <i> <SLI_DISABLE=> Disable
// <i> <SL_SI91X_EXTERNAL_PMU_GOOD_TIME_100us=> Enable 100us External PMU Good Time
// <i> <SL_SI91X_EXTERNAL_PMU_GOOD_TIME_200us=> Enable 200us External PMU Good Time
// <i> <SL_SI91X_EXTERNAL_PMU_GOOD_TIME_300us=> Enable 300us External PMU Good Time
// <i> <SL_SI91X_EXTERNAL_PMU_GOOD_TIME_400us=> Enable 400us External PMU Good Time
// <i> <SL_SI91X_EXTERNAL_PMU_GOOD_TIME_500us=> Enable 500us External PMU Good Time
// <i> <SL_SI91X_EXTERNAL_PMU_GOOD_TIME_600us=> Enable 600us External PMU Good Time
// <i> <SL_SI91X_EXTERNAL_PMU_GOOD_TIME_700us=> Enable 700us External PMU Good Time
// <i> <SL_SI91X_EXTERNAL_PMU_GOOD_TIME_800us=> Enable 800us External PMU Good Time
// <i> <SL_SI91X_EXTERNAL_PMU_GOOD_TIME_900us=> Enable 900us External PMU Good Time
// <i> <SL_SI91X_EXTERNAL_PMU_GOOD_TIME_1000us=> Enable 1000us External PMU Good Time
// <i> <SL_SI91X_EXTERNAL_PMU_GOOD_TIME_1100us=> Enable 1100us External PMU Good Time
// <i> <SL_SI91X_EXTERNAL_PMU_GOOD_TIME_1200us=> Enable 1200us External PMU Good Time
// <i> <SL_SI91X_EXTERNAL_PMU_GOOD_TIME_1300us=> Enable 1300us External PMU Good Time
// <i> <SL_SI91X_EXTERNAL_PMU_GOOD_TIME_1400us=> Enable 1400us External PMU Good Time
// <i> <SL_SI91X_EXTERNAL_PMU_GOOD_TIME_1500us=> Enable 1500us External PMU Good Time
// <i> Default: Disable
#define SL_SI91X_WC_EXTERNAL_PMU_GOOD_TIME SLI_DISABLE
// <i> </h>

// This macro is not supported and is not displayed in the UI.
// <i> <e SL_SI91X_WC_FEAT_EXTERNAL_LDO_SEL> External LDO Voltage Selection(not supported)
// <i> When enabled, this bit allows the selection between using an external LDO or an internal PMU.
// <i> If both the external LDO selection and the LDO voltage are configured, the system uses the external LDO.
// <i> If this bit is cleared, the internal PMU is used.
// <i> @note These bits are used for External LDO selection External PMU:
// <i> 1. Incase of External PMU, User has to set EXTERNAL_PMU_GOOD_TIME_CONFIGURATION value to external PMU good time, If this is zero then it indicates using Internal PMU.
// <i> 2. Incase of External PMU 1.0 or 1.05 V, User has to set both the bits config_feature_bit_map[11] and config_feature_bit_map[10].
// <i> @note Not applicable for SiWx91x.
// <i> Enable or disable
// <i> Default: Disable
#define SL_SI91X_WC_FEAT_EXTERNAL_LDO_SEL  0
#define SLI_SI91X_WC_FEAT_EXTERNAL_LDO_SEL (SL_SI91X_WC_FEAT_EXTERNAL_LDO_SEL << 10)

// This macro is not supported and is not displayed in the UI.
// <i> <o SL_SI91X_WC_FEAT_EXTERNAL_LDO_VOL> LDO Voltage Selection(not supported)
// <i> <1=> 1.0V
// <i> <0=> 1.1V
// <i> If this bit is set, the LDO voltage is configured to 1.0 V; if cleared, it is set to 1.1 V.
// <i> @note Not applicable for SiWx91x.
// <i> Default: 1.1V
#define SL_SI91X_WC_FEAT_EXTERNAL_LDO_VOL  0
#define SLI_SI91X_WC_FEAT_EXTERNAL_LDO_VOL (SL_SI91X_WC_FEAT_EXTERNAL_LDO_VOL << 11)
// <i> </e>

// <h> Power Supply Configurations
// <q SL_SI91X_WC_EXT_FEAT_1P8V_SUPPORT> To Enable 1.8v Support for NWP
// <i> Enabling this bit activates support for 1.8 V operation.
// <i> Enable or disable
// <i> Default: Disable
#define SL_SI91X_WC_EXT_FEAT_1P8V_SUPPORT  0
#define SLI_SI91X_WC_EXT_FEAT_1P8V_SUPPORT (SL_SI91X_WC_EXT_FEAT_1P8V_SUPPORT << 25)

// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_FEAT_RF_SUPPLY_VOL_3_3_VOLT> 3.3 V Power Supply For RF(not supported)
// <i> Configures the device to use a 3.3 V power supply for RF
// <i> @note Not applicable for SiWx91x
// <i> Default: Disable
#define SL_SI91X_WC_FEAT_RF_SUPPLY_VOL_3_3_VOLT  0
#define SLI_SI91X_WC_FEAT_RF_SUPPLY_VOL_3_3_VOLT (SL_SI91X_WC_FEAT_RF_SUPPLY_VOL_3_3_VOLT << 6)
// </h>
// </h>

// <h> Debug & Developer Experience
// <h> UART Debugging (Mutually Exclusive)
// <o SL_SI91X_WC_EXT_FEAT_UART_SEL_FOR_DEBUG_PRINTS> UART Debug Prints Pin Selection
// <0=> Debug Prints Supported On UART2 (TX: GPIO_6 RX: GPIO_10)
// <1=> Debug Prints Supported On UART1 (TX: GPIO_9 RX: GPIO_8)
// <i> If BIT(27) is enabled, debug prints are supported on UART1. If BIT(27) is disabled, debug prints are supported on UART2.
// <i> @note By default, all debug prints from the device network processor would be sent to UART2 if this bit is not enabled. UART1 pins are mapped as follows:
// <i> - UART1-TX: GPIO_9
// <i> - UART1-RX: GPIO_8
// <i> - UART2-TX: GPIO_6
// <i> - UART2-RX: GPIO_10
// <i> Ensure these pins are not used in MCU applications in SoC mode to avoid pin usage conflicts. This bit is valid only if SL_SI91X_EXT_FEAT_DISABLE_DEBUG_PRINTS in the ext_custom_feature_bit_map is set to 0. There is no functionality on RX pins for debug prints.
// <i> Default: Debug prints supported on UART2
#define SL_SI91X_WC_EXT_FEAT_UART_SEL_FOR_DEBUG_PRINTS  0
#define SLI_SI91X_WC_EXT_FEAT_UART_SEL_FOR_DEBUG_PRINTS (SL_SI91X_WC_EXT_FEAT_UART_SEL_FOR_DEBUG_PRINTS << 27)

// <q SL_SI91X_WC_EXT_FEAT_DISABLE_DEBUG_PRINTS> Disable Debug Prints Support
// <i> By default the prints would be coming on UART2. If this bit is enabled, disable debug prints.
// <i> To enable prints on UART 1 @ref "UART debug prints pin selection"  toggle switch needs to set UART2.
// <i> Enable or disable
// <i> Default: Disable
#define SL_SI91X_WC_EXT_FEAT_DISABLE_DEBUG_PRINTS  0
#define SLI_SI91X_WC_EXT_FEAT_DISABLE_DEBUG_PRINTS (SL_SI91X_WC_EXT_FEAT_DISABLE_DEBUG_PRINTS << 28)

// <q SL_SI91X_WC_ULP_GPIO9_FOR_UART2_TX> UART2 Port for Debug Prints
// <i> If this bit is not set, the default UART2-TX pin GPIO_6 is used.
// <i> @note SiWx91x supports prints only on ULP_GPIO_9.
// <i> Enable or disable
// <i> Default: Disable
#define SL_SI91X_WC_ULP_GPIO9_FOR_UART2_TX  0
#define SLI_SI91X_WC_ULP_GPIO9_FOR_UART2_TX (SL_SI91X_WC_ULP_GPIO9_FOR_UART2_TX << 18)
// </h>

// <h> Real-Time System Monitoring
// <q SL_SI91X_WC_CUSTOM_FEAT_ASYNC_CONNECTION_STATUS> Enable Async WLAN Connection Status Indication
// <i> If this bit is enabled, the module will asynchronously notify the host of WLAN connection status changes.
// <i> This feature is valid in Wi-Fi client mode only.
// <i> Enable or disable indicating the host about the WLAN connection status asynchronously feature.
// <i> Default: Disable
#define SL_SI91X_WC_CUSTOM_FEAT_ASYNC_CONNECTION_STATUS  0
#define SLI_SI91X_WC_CUSTOM_FEAT_ASYNC_CONNECTION_STATUS (SL_SI91X_WC_CUSTOM_FEAT_ASYNC_CONNECTION_STATUS << 10)

// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_CUSTOM_FEAT_LED_FEATURE> Enables LED Blinking Feature(not supported)
// <i> When this bit is set, the LED (GPIO_16) will blink to indicate network activity.
// <i> The LED blinks when a TX packet is sent or when a unicast packet addressed to the device’s MAC is received.
// <i> @note Not applicable for SIWx91x.
// <i> Enable or disable enabling the LED blinking feature.
// <i> Default: Disable
#define SL_SI91X_WC_CUSTOM_FEAT_LED_FEATURE  0
#define SLI_SI91X_WC_CUSTOM_FEAT_LED_FEATURE (SL_SI91X_WC_CUSTOM_FEAT_LED_FEATURE << 9)
// </h>

// <i> <h> Additional Debug Features
// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_ENABLE_DEBUG_BBP_TEST_PINS> Enable BBP Test Pins(not supported)
// <i> Set this bit to enable the use of BBP test pins for debugging and testing purposes.
// <i> Enable or disable
// <i> Default: Disable
#define SL_SI91X_WC_ENABLE_DEBUG_BBP_TEST_PINS  0
#define SLI_SI91X_WC_ENABLE_DEBUG_BBP_TEST_PINS (SL_SI91X_WC_ENABLE_DEBUG_BBP_TEST_PINS << 27)
// <i> </h>
// </h>

// <h> Custom Features
// <i> <q SL_SI91X_WC_CUSTOM_FEAT_TRIGGER_AUTO_CONFIG> Trigger Auto Configuration(not supported)
// <i> This bit enables the auto-configuration feature, which allows the module to automatically configure itself based on predefined parameters.
// <i>  @note Not applicable for SIWx91x.
// <i> Enable or disable triggering Auto Configuration feature.
// <i> Default: Disable
#define SL_SI91X_WC_CUSTOM_FEAT_TRIGGER_AUTO_CONFIG  0
#define SLI_SI91X_WC_CUSTOM_FEAT_TRIGGER_AUTO_CONFIG (SL_SI91X_WC_CUSTOM_FEAT_TRIGGER_AUTO_CONFIG << 20)

// This macro is not supported and is not displayed in the UI.
// <i> <q SL_SI91X_WC_CUSTOM_FEAT_BT_IAP> Custom Feat BT IAP (not supported)
// <i>  Enables Bluetooth In-App Programming (IAP) feature.
// <i>  This bit enables the Bluetooth In-App Programming (IAP) feature, which allows the module to support Bluetooth-related IAP functionalities.
// <i> Enable or disable Custom feat BT IAP feature.
// <i> Default: Disable
#define SL_SI91X_WC_CUSTOM_FEAT_BT_IAP  0
#define SLI_SI91X_WC_CUSTOM_FEAT_BT_IAP (SL_SI91X_WC_CUSTOM_FEAT_BT_IAP << 29)

// <q SL_SI91X_WC_CUSTOM_FEAT_WAKE_ON_WIRELESS> Wake-On-Wireless in UART Mode
// <i> This bit enables the wake-on-wireless feature when operates in UART mode, which allows the module to wake up in response to wireless events.
// <i> @note applicable for NCP only.
// <i> Enable or disable Custom feat BT IAP feature.
// <i> Default: Disable
#define SL_SI91X_WC_CUSTOM_FEAT_WAKE_ON_WIRELESS  0
#define SLI_SI91X_WC_CUSTOM_FEAT_WAKE_ON_WIRELESS (SL_SI91X_WC_CUSTOM_FEAT_WAKE_ON_WIRELESS << 11)

// </h>

#define SL_SI91X_WC_FEATURE_BITMAP                                                                               \
  (SLI_SI91X_WC_FEAT_SECURITY_OPEN | SLI_SI91X_WC_FEAT_SECURITY_PSK | SLI_SI91X_WC_FEAT_AGGREGATION              \
   | SLI_SI91X_WC_FEAT_LP_GPIO_BASED_HANDSHAKE | SLI_SI91X_WC_FEAT_ULP_GPIO_BASED_HANDSHAKE                      \
   | SLI_SI91X_WC_FEAT_DEV_TO_HOST_ULP_GPIO_1 | SLI_SI91X_WC_FEAT_RF_SUPPLY_VOL_3_3_VOLT                         \
   | SLI_SI91X_WC_FEAT_WPS_DISABLE | SLI_SI91X_WC_FEAT_EAP_LEAP_IN_COEX | SLI_SI91X_WC_FEAT_HIDE_PSK_CREDENTIALS \
   | SLI_SI91X_WC_FEAT_SSL_HIGH_STREAMING_BIT | SLI_SI91X_WC_FEAT_LONG_ICMP_PACKET                               \
   | SLI_SI91X_WC_FEAT_TRANSCEIVER_MAC_PEER_DS_SUPPORT | SLI_SI91X_WC_FEAT_LONG_HTTP_URL                         \
   | SLI_SI91X_WC_FEAT_DISABLE_11AX_SUPPORT | SLI_SI91X_WC_FEAT_FW_UPDATE_NEW_CODE                               \
   | SLI_SI91X_WC_FEAT_SECURE_ATTESTATION | SLI_SI91X_WC_FEAT_SOCKET_CMDS_ALLOW_BEFORE_WLAN_CONNECTION)

#define SL_SI91X_WC_TCP_IP_FEATURE_BITMAP                                                                            \
  (SLI_SI91X_WC_TCP_IP_FEAT_BYPASS | SLI_SI91X_WC_TCP_IP_FEAT_DHCPV4_CLIENT | SLI_SI91X_WC_TCP_IP_FEAT_DHCPV6_CLIENT \
   | SLI_SI91X_WC_TCP_IP_FEAT_DHCPV4_SERVER | SLI_SI91X_WC_TCP_IP_FEAT_DHCPV6_SERVER                                 \
   | SLI_SI91X_WC_TCP_IP_FEAT_JSON_OBJECTS | SLI_SI91X_WC_TCP_IP_FEAT_HTTP_CLIENT                                    \
   | SLI_SI91X_WC_TCP_IP_FEAT_DNS_CLIENT | SLI_SI91X_WC_TCP_IP_FEAT_SNMP_AGENT | SLI_SI91X_WC_TCP_IP_FEAT_SSL        \
   | SLI_SI91X_WC_TCP_IP_FEAT_ICMP | SLI_SI91X_WC_TCP_IP_FEAT_SEND_CONFIGS_TO_HOST                                   \
   | SLI_SI91X_WC_TCP_IP_FEAT_FTP_CLIENT | SLI_SI91X_WC_TCP_IP_FEAT_SNTP_CLIENT | SLI_SI91X_WC_TCP_IP_FEAT_IPV6      \
   | SLI_SI91X_WC_TCP_IP_FEAT_RAW_DATA | SLI_SI91X_WC_TCP_IP_FEAT_MDNSD | SLI_SI91X_WC_TCP_IP_FEAT_SMTP_CLIENT       \
   | SLI_SI91X_WC_TCP_IP_TOTAL_SOCKETS | SLI_SI91X_WC_TCP_IP_FEAT_SINGLE_SSL_SOCKET                                  \
   | SLI_SI91X_WC_TCP_IP_FEAT_LOAD_PUBLIC_PRIVATE_CERTS | SLI_SI91X_WC_TCP_IP_FEAT_LOAD_CERTS_INTO_RAM               \
   | SLI_SI91X_WC_TCP_IP_FEAT_DTLS_THREE_SOCKETS | SLI_SI91X_WC_TCP_IP_FEAT_OTAF                                     \
   | SLI_SI91X_WC_TCP_IP_FEAT_EXTENSION_VALID)

// Validates the use of extended custom feature bitmap.
// The bit indicates the extended custom feature bitmap is valid.
// If this bit is enabled then only, the features present in the extended custom feature bitmap can be used.
#define SL_SI91X_WC_CUSTOM_FEAT_EXTENTION_VALID  (SL_SI91X_WC_EXTENDED_CUSTOM_FEATURE_BITMAP ? 1 : 0)
#define SLI_SI91X_WC_CUSTOM_FEAT_EXTENTION_VALID SL_SI91X_WC_CUSTOM_FEAT_EXTENTION_VALID << 31

#define SL_SI91X_WC_CUSTOM_FEATURE_BITMAP                                                                     \
  (SLI_SI91X_WC_CUSTOM_FEAT_DISABLE_GATEWAY_IN_RSI_AP | SLI_SI91X_WC_CUSTOM_FEAT_SOC_CLK_CONFIG_160MHZ        \
   | SLI_SI91X_WC_CUSTOM_FEAT_AP_IN_HIDDEN_MODE | SLI_SI91X_WC_CUSTOM_FEAT_DNS_SERVER_IN_DHCP_OFFER           \
   | SLI_SI91X_WC_CUSTOM_FEAT_DFS_CHANNEL_SUPPORT | SLI_SI91X_WC_CUSTOM_FEAT_LED_FEATURE                      \
   | SLI_SI91X_WC_CUSTOM_FEAT_ASYNC_CONNECTION_STATUS | SLI_SI91X_WC_CUSTOM_FEAT_WAKE_ON_WIRELESS             \
   | SLI_SI91X_WC_CUSTOM_FEAT_ENABLE_AP_BLACKLIST | SLI_SI91X_WC_CUSTOM_FEAT_MAX_NUM_OF_CLIENTS               \
   | SLI_SI91X_WC_CUSTOM_FEAT_ROAM_WITH_DEAUTH_OR_NULL_DATA | SLI_SI91X_WC_CUSTOM_FEAT_TRIGGER_AUTO_CONFIG    \
   | SLI_SI91X_WC_CUSTOM_FEAT_LIMIT_PACKETS_PER_STA | SLI_SI91X_WC_CUSTOM_FEAT_HTTP_HTTPS_AUTH                \
   | SLI_SI91X_WC_CUSTOM_FEAT_SOC_CLK_CONFIG_120MHZ | SLI_SI91X_WC_CUSTOM_FEAT_REJECT_CONNECT_REQ_IMMEDIATELY \
   | SLI_SI91X_WC_CUSTOM_FEAT_DUAL_BAND_ROAM_VCSAFD | SLI_SI91X_WC_CUSTOM_FEAT_RTC_FROM_HOST                  \
   | SLI_SI91X_WC_CUSTOM_FEAT_BT_IAP | SLI_SI91X_WC_CUSTOM_FEAT_EXTENTION_VALID)

#define SL_SI91X_WC_EXTENDED_CUSTOM_FEATURE_BITMAP                                                                   \
  (SLI_SI91X_WC_EXT_FEAT_RSA_KEY_WITH_4096_SUPPORT | SLI_SI91X_WC_EXT_FEAT_SSL_CERT_WITH_4096_KEY_SUPPORT            \
   | SLI_SI91X_WC_EXT_FEAT_AP_BROADCAST_PKT_SND_B4_DTIM | SLI_SI91X_WC_EXT_FEAT_FCC_LOW_PWR                          \
   | SLI_SI91X_WC_EXT_FEAT_PUF | SLI_SI91X_WC_EXT_FEAT_NWP_QSPI_80MHZ_CLK_ENABLE                                     \
   | SLI_SI91X_WC_EXT_HTTP_SKIP_DEFAULT_LEADING_CHARACTER | SLI_SI91X_WC_EXT_FEAT_PUF_PRIVATE_KEY                    \
   | SLI_SI91X_WC_EXT_FEAT_ENABLE_11R_OTA | SLI_SI91X_WC_EXT_FEAT_IEEE_80211J | SLI_SI91X_WC_EXT_FEAT_IEEE_80211W    \
   | SLI_SI91X_WC_EXT_FEAT_SSL_VERSIONS_SUPPORT | SLI_SI91X_WC_EXT_FEAT_16th_STATION_IN_AP_MODE                      \
   | SLI_SI91X_WC_EXT_FEAT_ENABLE_11R_ODS | SLI_SI91X_WC_EXT_FEAT_WOWLAN_DISABLE                                     \
   | SLI_SI91X_WC_EXT_FEAT_DISABLE_XTAL_CORRECTION | SLI_SI91X_WC_EXT_FEAT_LOW_POWER_MODE                            \
   | SLI_SI91X_WC_EXT_FEAT_XTAL_CLK | SLI_SI91X_WC_EXT_FEAT_HOMEKIT_WAC_ENABLED | SLI_SI91X_WC_EXT_FEAT_1P8V_SUPPORT \
   | SLI_SI91X_WC_EXT_FEAT_UART_SEL_FOR_DEBUG_PRINTS | SLI_SI91X_WC_EXT_FEAT_DISABLE_DEBUG_PRINTS                    \
   | SLI_SI91X_WC_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0 | SLI_SI91X_WC_EXT_FEAT_FRONT_END_INTERNAL_SWITCH    \
   | SLI_SI91X_WC_MEMORY_CONFIG)

#define SL_SI91X_WC_BT_FEATURE_BITMAP (SLI_SI91X_WC_BT_RF_TYPE | SLI_SI91X_WC_ENABLE_BLE_PROTOCOL)

#define SL_SI91X_WC_EXTENDED_TCPIP_FEATURE_BITMAP                                                                      \
  (SLI_SI91X_WC_EXT_TCP_FEAT_DHCP_OPT77 | SLI_SI91X_WC_EXT_TCP_IP_BI_DIR_ACK_UPDATE                                    \
   | SLI_SI91X_WC_EXT_TCP_IP_WINDOW_DIV | SLI_SI91X_WC_EXT_TCP_IP_CERT_BYPASS | SLI_SI91X_WC_EXT_TCP_IP_SSL_16K_RECORD \
   | SLI_SI91X_WC_EXT_TCP_IP_DNS_CLIENT_BYPASS | SLI_SI91X_WC_EXT_TCP_IP_WINDOW_SCALING                                \
   | SLI_SI91X_WC_EXT_TCP_IP_DUAL_MODE_ENABLE | SLI_SI91X_WC_EXT_DYNAMIC_COEX_MEMORY                                   \
   | SLI_SI91X_WC_EXT_TCP_IP_TOTAL_SELECTS | SLI_SI91X_WC_EXT_TCP_IP_WAIT_FOR_SOCKET_CLOSE                             \
   | SLI_SI91X_WC_EXT_EMB_MQTT_ENABLE | SLI_SI91X_WC_EXT_FEAT_HTTP_OTAF_SUPPORT                                        \
   | SLI_SI91X_WC_EXT_TCP_DYNAMIC_WINDOW_UPDATE_FROM_HOST | SLI_SI91X_WC_EXT_TCP_MAX_RECV_LENGTH                       \
   | SLI_SI91X_WC_EXT_TCP_IP_FEAT_SSL_THREE_SOCKETS | SLI_SI91X_WC_EXT_TCP_IP_FEAT_SSL_MEMORY_CLOUD                    \
   | SLI_SI91X_WC_CONFIG_FEAT_EXTENTION_VALID)

#define SL_SI91X_WC_BLE_FEATURE_BITMAP                                                                         \
  (SLI_SI91X_WC_BLE_MAX_NBR_ATT_REC | SLI_SI91X_WC_BLE_MAX_NBR_ATT_SERV | SLI_SI91X_WC_BLE_MAX_NBR_PERIPHERALS \
   | SLI_SI91X_WC_BLE_PWR_INX | SLI_SI91X_WC_BLE_PWR_SAVE_OPTIONS | SLI_SI91X_WC_BLE_MAX_NBR_CENTRALS          \
   | SLI_SI91X_WC_BLE_GATT_ASYNC_ENABLE | SLI_SI91X_WC_916_BLE_COMPATIBLE_FEAT_ENABLE                          \
   | SLI_SI91X_WC_FEAT_BLE_CUSTOM_FEAT_EXTENTION_VALID)

#define SL_SI91X_WC_EXTENDED_BLE_CUSTOM_FEATURE_BITMAP                                            \
  (SLI_SI91X_WC_BLE_NUM_CONN_EVENTS | SLI_SI91X_WC_BLE_NUM_REC_BYTES | SLI_SI91X_WC_BLE_GATT_INIT \
   | SLI_SI91X_WC_BLE_INDICATE_CONFIRMATION_FROM_HOST | SLI_SI91X_WC_BLE_MTU_EXCHANGE_FROM_HOST   \
   | SLI_SI91X_WC_BLE_SET_SCAN_RESP_DATA_FROM_HOST | SLI_SI91X_WC_BLE_DISABLE_CODED_PHY_FROM_HOST \
   | SLI_SI91X_WC_BLE_ENABLE_ADV_EXTN | SLI_SI91X_WC_BLE_AE_MAX_ADV_SETS)

#define SL_SI91X_WC_CONFIG_FEAT_EXTENTION_VALID  (SL_SI91X_WC_CONFIG_FEATURE_BITMAP ? 1 : 0)
#define SLI_SI91X_WC_CONFIG_FEAT_EXTENTION_VALID SL_SI91X_WC_CONFIG_FEAT_EXTENTION_VALID << 31

#define SL_SI91X_WC_CONFIG_FEATURE_BITMAP                                                                           \
  (SLI_SI91X_WC_FEAT_SLEEP_GPIO_SEL_BITMAP | SL_SI91X_WC_FEAT_DVS_SEL_CONFIG | SL_SI91X_WC_EXTERNAL_PMU_GOOD_TIME   \
   | SLI_SI91X_WC_FEAT_EXTERNAL_LDO_SEL | SLI_SI91X_WC_FEAT_EXTERNAL_LDO_VOL | SLI_SI91X_WC_FEAT_EAP_TLS_V1PX       \
   | SLI_SI91X_WC_FEAT_CONC_STA_AP_DYN_SWITCH_SEL | SLI_SI91X_WC_ULP_GPIO9_FOR_UART2_TX                             \
   | SLI_SI91X_WC_FEAT_DISABLE_MCS_5_6_7_DATARATES | SLI_SI91X_WC_FEAT_DISABLE_SHORT_GI | SLI_SI91X_WC_PTA_3WIRE_EN \
   | SLI_SI91X_WC_PTA_3WIRE_CONFIG_SEL | SL_SI91X_WC_XTAL_GOODTIME | SLI_SI91X_WC_ENABLE_ENHANCED_MAX_PSP           \
   | SLI_SI91X_WC_ENABLE_DEBUG_BBP_TEST_PINS)

#endif //SL_SI91X_WC_DEVICE_CONFIGURATION_H
