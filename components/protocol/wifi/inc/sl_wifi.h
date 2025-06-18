/***************************************************************************/ /**
 * @file
 * @brief SL Wi-Fi API
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#pragma once
#include "sl_wifi_device.h"
#include "sl_wifi_types.h"
#include "sl_constants.h"
#include <stdint.h>
#include <stdbool.h>

/// Default Wi-Fi scan configuration
extern const sl_wifi_scan_configuration_t default_wifi_scan_configuration;

/// Default Wi-Fi AP configuration
extern const sl_wifi_ap_configuration_t default_wifi_ap_configuration;

/** \addtogroup WIFI_COMMON_API Common
 * 
 *  @note To manage the Wi-Fi interface lifecycle correctly, it's important to use functions like sl_wifi_init(), sl_wifi_connect(), sl_wifi_disconnect(), and sl_wifi_deinit() as intended.
 *        Improper sequencing or combining sl_wifi_* and sl_net_* API functions without following the recommended order may lead to unpredictable or undefined behavior.
 *
 * \ingroup SL_WIFI_FUNCTIONS
 * @{ */

/***************************************************************************/ /**
 * @brief
 *   This function initializes the Wi-Fi module using the specified device configuration,
 *   device context, and event handler. It configures the Wi-Fi device and establishes
 *   the event handler for Wi-Fi events. This function must be called before using any
 *   other Wi-Fi functions.
 * @param[in] configuration
 *   [sl_wifi_device_configuration_t](../wiseconnect-api-reference-guide-si91x-driver/sl-wifi-device-configuration-t) object that contains Wi-Fi device configuration.
 * @param[in] device_context
 *  Reserved for future use.
 * @param[in] event_handler
 *   Wi-Fi event handler function of type @ref sl_wifi_event_handler_t.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note 
 *   This function should be called before calling any other sl_wifi functions.
 * @note
 *   In SoC mode, wireless initialization must be completed before using the NVM3 APIs in the common flash, as flash write and erase operations require communication between the NWP & M4.
 ******************************************************************************/
sl_status_t sl_wifi_init(const sl_wifi_device_configuration_t *configuration,
                         const sl_wifi_device_context_t *device_context,
                         sl_wifi_event_handler_t event_handler);

/***************************************************************************/ /**
 * @brief
 *   This function ensures proper shutdown of the Wi-Fi driver, resetting configurations and releasing resources.
 *   Call this API to deinitialize the Wi-Fi module to avoid resource leaks.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 *   All si91x, BSD, and IoT sockets must be closed before invoking this API.
 ******************************************************************************/
sl_status_t sl_wifi_deinit(void);

/***************************************************************************/ /**
 * @brief
 *   Check if Wi-Fi interface is up.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @return
 *   1. true: interface is up.
 *   2. false: interface is down.
 ******************************************************************************/
bool sl_wifi_is_interface_up(sl_wifi_interface_t interface);

/***************************************************************************/ /**
 * @brief
 *   Return the firmware version running on the Wi-Fi device.
 * @pre Pre-conditions:
 * -
 *   @ref sl_wifi_init should be called before this API.
 * @param[out] version
 *   @ref sl_wifi_firmware_version_t object that contains the version string.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 *   Moving forward, this API will be deprecated. Instead, use the [sl_si91x_get_firmware_version](../wiseconnect-api-reference-guide-si91x-driver/si91-x-driver-functions#sl-si91x-get-firmware-version) API.
 ******************************************************************************/
sl_status_t sl_wifi_get_firmware_version(sl_wifi_firmware_version_t *version);

/***************************************************************************/ /**
 * @brief
 *   Gets wlan info in AP mode / Client mode.
 * @pre Pre-conditions:
 * -
 *   @ref sl_wifi_init should be called before this API.
 * @param[out] info
 *   [sl_si91x_rsp_wireless_info_t](../wiseconnect-api-reference-guide-si91x-driver/sl-si91x-rsp-wireless-info-t) object that contains the wlan info.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 ******************************************************************************/
sl_status_t sl_wifi_get_wireless_info(sl_si91x_rsp_wireless_info_t *info);

/***************************************************************************/ /**
 * @brief
 *   Return the firmware image size from firmware image.
 * @param[in] buffer
 *   Buffer pointing to firmware image file.
 * @param[out] fw_image_size
 *   Size of the firmware image passed in the input buffer param. The value returned in this param is valid only if this API returns SL_STATUS_OK(0).
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 *   Moving forward, this API will be deprecated. Instead, use the [sl_si91x_get_firmware_size](../wiseconnect-api-reference-guide-si91x-driver/si91-x-driver-functions#sl-si91x-get-firmware-size) API.
 ******************************************************************************/
sl_status_t sl_wifi_get_firmware_size(const void *buffer, uint32_t *fw_image_size);

/***************************************************************************/ /**
 * @brief
 *   Set the default Wi-Fi interface as supported by @ref sl_wifi_interface_t.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 ******************************************************************************/
void sl_wifi_set_default_interface(sl_wifi_interface_t interface);

/***************************************************************************/ /**
 * @brief
 *   Get the default interface.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @return
 *   @ref sl_wifi_interface_t previously set by @ref sl_wifi_set_default_interface
 ******************************************************************************/
sl_wifi_interface_t sl_wifi_get_default_interface(void);

/***************************************************************************/ /**
 * @brief
 *   Retrieves the MAC addresses of the specified Wi-Fi interface, in concurrent mode retrieves two MAC addresses.
 * @details
 *   MAC address of the module. In concurrent mode, two MAC addresses are returned, MAC_Address1 is the station MAC 
 *   address and MAC_Address2 is the created AP MAC address. MAC address is returned in 6-bytes in hex format.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[out] mac
 *   [sl_mac_address_t](../wiseconnect-api-reference-guide-nwk-mgmt/sl-net-types#sl-mac-address-t) object contains the MAC address of the interface.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 ******************************************************************************/
sl_status_t sl_wifi_get_mac_address(sl_wifi_interface_t interface, sl_mac_address_t *mac);

/***************************************************************************/ /**
 * @brief
 *   Set the Wi-Fi interface MAC address.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] mac 
 *   [sl_mac_address_t](../wiseconnect-api-reference-guide-nwk-mgmt/sl-net-types#sl-mac-address-t) object to store the MAC address.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 *   This API is not supported by SiWx91x when called directly due to firmware constraints.
 *   Alternatively, @ref sl_wifi_init can be used to configure the MAC address. sl_wifi_init ensures the appropriate state of firmware and calls this API to set MAC address.
 ******************************************************************************/
sl_status_t sl_wifi_set_mac_address(sl_wifi_interface_t interface, const sl_mac_address_t *mac);

/** @} */

/** \addtogroup WIFI_RADIO_API Radio
  * \ingroup SL_WIFI_FUNCTIONS
  * @{ */

// Radio management functions

/***************************************************************************/ /**
 * @brief
 *   Get the maximum Wi-Fi transmit power.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[out] max_tx_power
 *   A variable that contains current maximum transmit power as identified by by @ref sl_wifi_max_tx_power_t.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 *   This function gets the transmit power for a particular radio interface: SL_WIFI_2_4GHZ_INTERFACE.
 ******************************************************************************/
sl_status_t sl_wifi_get_max_tx_power(sl_wifi_interface_t interface, sl_wifi_max_tx_power_t *max_tx_power);

/***************************************************************************/ /**
 * @brief
 *   Set the maximum Wi-Fi transmit power.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] max_tx_power
 *   Max transmission power as identified by @ref sl_wifi_max_tx_power_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 *   This function sets the transmit power for a particular radio interface: SL_WIFI_2_4GHZ_INTERFACE.
 *   Eg: Setting transmit power for client interface at 2.4 GHz will also set transmit power of the AP interface at 2.4 GHz.
 * @note
 *   The effective transmit power is subject to regional and device limitations. If the specified transmit power exceeds the
 *   maximum supported value for that region, the transmission occurs at the maximum supported transmit power.
 ******************************************************************************/
sl_status_t sl_wifi_set_max_tx_power(sl_wifi_interface_t interface, sl_wifi_max_tx_power_t max_tx_power);

/***************************************************************************/ /**
 * @brief
 *   Set the Wi-Fi antenna for an interface.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] antenna
 *   Antenna to select as identified by @ref sl_wifi_antenna_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 ******************************************************************************/
sl_status_t sl_wifi_set_antenna(sl_wifi_interface_t interface, sl_wifi_antenna_t antenna);

/***************************************************************************/ /**
 * @brief
 *   Get the Wi-Fi antenna for an interface.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[out] antenna
 *   @ref sl_wifi_antenna_t object that contains current antenna selection.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 ******************************************************************************/
sl_status_t sl_wifi_get_antenna(sl_wifi_interface_t interface, sl_wifi_antenna_t *antenna);

/***************************************************************************/ /**
 * @brief
 *   Get the current channel for the given Wi-Fi interface.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[out] channel
 *   @ref sl_wifi_channel_t object that contains current channel information.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 ******************************************************************************/
sl_status_t sl_wifi_get_channel(sl_wifi_interface_t interface, sl_wifi_channel_t *channel);

/***************************************************************************/ /**
 * @brief
 *   Set the channel for the given Wi-Fi Access Point interface.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] channel
 *   Channel as identified by @ref sl_wifi_channel_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 ******************************************************************************/
sl_status_t sl_wifi_set_channel(sl_wifi_interface_t interface, sl_wifi_channel_t channel);

/***************************************************************************/ /**
 * @brief
 *   Set the Wi-Fi transmit rate for the given 802.11 protocol on the specified Wi-Fi interface.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API. 
 * -
 *   In AP mode, this API should be called before sl_net_wifi_ap_up. This configured data rate is passed as part of the AP configuration while bringing up the AP interface.
 * -
 *   In Wi-Fi client mode, this API should be called after @ref sl_wifi_connect.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] rate_protocol
 *   802.11 protocol as identified by @ref sl_wifi_rate_protocol_t
 * @param[in] mask
 *   Data rate as identified by @ref sl_wifi_rate_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 *   Only 1 and 2 Mbps rates are allowed in channel 14.
 ******************************************************************************/
sl_status_t sl_wifi_set_transmit_rate(sl_wifi_interface_t interface,
                                      sl_wifi_rate_protocol_t rate_protocol,
                                      sl_wifi_rate_t mask);

/***************************************************************************/ /**
 * @brief
 *   Get the Wi-Fi transmit rate for the given 802.11 protocol on the specified Wi-Fi interface.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[out] rate_protocol
 *   802.11 protocol as identified by @ref sl_wifi_rate_protocol_t
 * @param[out] mask
 *   Data rate as identified by @ref sl_wifi_rate_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 ******************************************************************************/
sl_status_t sl_wifi_get_transmit_rate(sl_wifi_interface_t interface,
                                      sl_wifi_rate_protocol_t *rate_protocol,
                                      sl_wifi_rate_t *mask);

/***************************************************************************/ /**
 * @brief
 *   Set the Wi-Fi client interface listen interval.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] listen_interval
 *   @ref sl_wifi_listen_interval_t object
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 *   By default listen interval is set 1000 millisecs. User can call this API to overwrite the value.
 * @note
 *   Moving forward, this API will be deprecated. Instead, use the [sl_wifi_set_listen_interval_v2](../wiseconnect-api-reference-guide-wi-fi/wifi-radio-api#sl-wifi-set-listen-interval-v2) API. This is retained for backward compatibility.
 *   Si91X implementation allows this API ONLY to be called before calling @ref sl_wifi_connect(), @ref sl_wifi_start_ap(), @ref sl_wifi_start_wps()
 ******************************************************************************/
sl_status_t sl_wifi_set_listen_interval(sl_wifi_interface_t interface,
                                        sl_wifi_listen_interval_t listen_interval) SL_DEPRECATED_API_WISECONNECT_3_5;

/***************************************************************************/ /**
 * @brief
 *   Set the Wi-Fi client interface listen interval and listen interval multiplier.
 * @pre Pre-conditions:
 * -
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] listen_interval
 *   @ref sl_wifi_listen_interval_v2_t object
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
*   By default listen interval is set 1000 millisecs and listen interval multiplier is set to 1. User can call this API to overwrite the values for listen interval and listen interval multiplier.
*   Recommended max value for listen_interval_multiplier is 10. Higher value may cause interop issues. 
 *   Si91X implementation allows this API ONLY to be called before calling @ref sl_wifi_connect(), @ref sl_wifi_start_ap(), @ref sl_wifi_start_wps()
 ******************************************************************************/
sl_status_t sl_wifi_set_listen_interval_v2(sl_wifi_interface_t interface, sl_wifi_listen_interval_v2_t listen_interval);

/***************************************************************************/ /**
 * @brief
 *   Get the Wi-Fi client listen interval.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[out] listen_interval
 *   @ref sl_wifi_listen_interval_t object that contains the current listen interval.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
*   By default, the listen interval is set to 1000 millisecs.
 * @note
 *   Moving forward, this API will be deprecated. Instead, use the [sl_wifi_get_listen_interval_v2](../wiseconnect-api-reference-guide-wi-fi/wifi-radio-api#sl-wifi-get-listen-interval-v2) API. This is retained for backward compatibility.
 ******************************************************************************/
sl_status_t sl_wifi_get_listen_interval(sl_wifi_interface_t interface,
                                        sl_wifi_listen_interval_t *listen_interval) SL_DEPRECATED_API_WISECONNECT_3_5;

/***************************************************************************/ /**
 * @brief
 *   Get the Wi-Fi client listen interval and listen interval multiplier.
 * @pre Pre-conditions:
 * -
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[out] listen_interval
 *   @ref sl_wifi_listen_interval_v2_t object that contains the current listen interval and listen interval multiplier.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 *  By default, the listen interval is set to 1000 millisecs and listen interval multiplier is set to 1.
 ******************************************************************************/
sl_status_t sl_wifi_get_listen_interval_v2(sl_wifi_interface_t interface,
                                           sl_wifi_listen_interval_v2_t *listen_interval);

/***************************************************************************/ /**
 * @brief
 *   Assign the user configurable channel gain table entries in different regions to the module from the user.
 * 
 * @details
 *   This function overwrites the default gain tables present in the firmware. Customers can load gain tables for 2.4 GHz-20 MHz. This is a blocking API.
 * 
 * @param[in]  band
 *    1 - 2.4 GHz
 * 
 * @param[in]  bandwidth
 *    0 - 20 MHz
 * 
 * @param[in]  payload
 *    Pass channel gain table entries for different regions in a given array format. 
 *    The gain array format is as follows: \n
 *     No Of Regions, \n
 *     Region code, \n
 *     No Of Channels, \n
 *     | Rate | 11b Gain Value | 11g Gain Value | 11n Gain Value | 11ax Gain Value|
 *     |------|----------------|----------------|----------------|----------------|
 *     |  x   |        x       |        x       |        x       |       x        |
 *    Repeat the above format for all regions codes.
 * 
 * @param[in]  payload_length
 *    Max payload length (table size) in 2.4 GHz is 128 bytes.
 * 
 * @return
 *    sl_status_t. See [Status Codes](https://docs.silabs.com/gecko-platform/latest/platform-common/status) and [Additional Status Codes](../wiseconnect-api-reference-guide-err-codes/sl-additional-status-errors) for details..
 * 
 * @pre Pre-conditions:
 *   @ref sl_wifi_init should be called before this API.
 * 
 * @note 
 *   1. This API will be deprecated in further releases. Suggested to use updated API @ref sl_wifi_update_su_gain_table for better gain table entries in 11ax SU operation.
 *   1. This frame must be used only by customers who have done FCC/ETSI/TELEC/KCC certification with their own antenna. Silicon Labs is not liable for inappropriate usage of this frame that may result in violation of FCC/ETSI/TELEC/KCC or any certifications. 
 *   2. Internally, firmware maintains two tables: Worldwide table, and Region-based table. Worldwide table is populated by the firmware with maximum power values that the chip can transmit and meet target specs like EVM. Region-based table has a default gain value set.
 *   3. When certifying with a user antenna, the Region has to be set to Worldwide and sweep the power from 0 to 21 dBm. Arrive at a max power level that will pass certifications, especially band-edge.
 *   4. The FCC/ETSI/TELEC/KCC maximum power level should be loaded in an end-to-end mode via WLAN User Gain table. This has to be called done for every boot-up as this information is not saved inside the flash. Region-based user gain table sent by the application is copied onto the Region-based table. SoC uses this table in FCC/ETSI/TELEC/KCC to limit the power and to not violate the allowed limits.
 *   5. For the Worldwide region, the firmware uses the Worldwide table for Tx. For other regions (FCC/ETSI/TELEC/KCC), the firmware uses the min value out of the Worldwide & Region-based table for Tx.  Also, there will be part to part variation across the chips. Offsets that are estimated during the flow of manufacture will be applied as correction factor during normal mode of operation.
 *   6. In the 2.4 GHz band, 40 MHz is not supported.
 *   7. Executing this API overwrites calibration values in certified modules.
 *   8. In FCC-certified modules, this API triggers an error SL_STATUS_SI91X_FEATURE_NOT_AVAILABLE if used, except when in SL_SI91X_TRANSMIT_TEST_MODE mode.
 *   9. Below are the default gain tables:
 *      - Si917 IC OPNs:
 *      <br> Number of regions: 4
 *   | Region    | Number of channels | Channel | 11b | 11g | 11n | 11ax |
 *   |-----------|--------------------|---------|-----|-----|-----|------|
 *   | FCC       | 0xB                |         |     |     |     |      |
 *   |           |                    |  1      | 33  | 24  |  21 |  20  |
 *   |           |                    |  2      | 34  | 28  |  28 |  24  |
 *   |           |                    |  3      | 40  | 30  |  32 |  30  |
 *   |           |                    |  4      | 40  | 33  |  36 |  31  |
 *   |           |                    |  5      | 40  | 35  |  36 |  32  |
 *   |           |                    |  6      | 40  | 35  |  36 |  31  |
 *   |           |                    |  7      | 40  | 34  |  36 |  30  |
 *   |           |                    |  8      | 38  | 32  |  36 |  32  |
 *   |           |                    |  9      | 38  | 34  |  34 |  28  |
 *   |           |                    | 10      | 34  | 30  |  30 |  22  |
 *   |           |                    | 11      | 34  | 24  |  22 |  20  |
 *   | ETSI      | 0x11               |         |     |     |     |      |
 *   |           |                    | 255     | 36  | 36  |  36 |  24  |
 *   | TELEC     | 0x24               |         |     |     |     |      |
 *   |           |                    |   2     | 34  | 28  |  32 |  24  |
 *   |           |                    |  10     | 34  | 36  |  36 |  24  |
 *   |           |                    |  13     | 34  | 26  |  24 |  24  |
 *   |           |                    |  14     | 36  |  0  |   0 |   0  |
 *   | KCC       | 0x11               |         |     |     |     |      |
 *   |           |                    | 255     | 36  | 36  |  36 |  36  |
 *
 *      - Si917 ACx OPNs:
 *      <br> Number of regions: 6
 *   | Region        | Number of channels | Channel | 11b | 11g | 11n | 11ax |
 *   |---------------|--------------------|---------|-----|-----|-----|------|
 *   | FCC           |  0x29              |         |     |     |     |      |
 *   |               |                    |       1 |  30 |  20 |  20 |   18 |
 *   |               |                    |       2 |  36 |  26 |  26 |   22 |
 *   |               |                    |       3 |  40 |  30 |  30 |   26 |
 *   |               |                    |       4 |  40 |  36 |  36 |   32 |
 *   |               |                    |       7 |  40 |  40 |  40 |   36 |
 *   |               |                    |       8 |  40 |  36 |  34 |   36 |
 *   |               |                    |       9 |  40 |  34 |  32 |   28 |
 *   |               |                    |      10 |  36 |  30 |  28 |   20 |
 *   |               |                    |      11 |  30 |  20 |  18 |   16 |
 *   | ETSI          |  0x11              |         |     |     |     |      |
 *   |               |                    |     255 |  24 |  28 |  28 |   14 |
 *   | TELEC         |  0x24              |         |     |     |     |      |
 *   |               |                    |       1 |  28 |  28 |  26 |   16 |
 *   |               |                    |      12 |  28 |  36 |  36 |   16 |
 *   |               |                    |      13 |  28 |  26 |  26 |   16 |
 *   |               |                    |      14 |  28 |   0 |   0 |    0 |
 *   | KCC           |  0x11              |         |     |     |     |      |
 *   |               |                    |     255 |  36 |  36 |  36 |   36 |
 *   | WORLDSAFE     |  0x24              |         |     |     |     |      |
 *   |               |                    |       1 |  24 |  20 |  20 |   14 |
 *   |               |                    |       2 |  24 |  26 |  26 |   14 |
 *   |               |                    |      10 |  24 |  28 |  28 |   14 |
 *   |               |                    |      11 |  24 |  20 |  18 |   14 |
 *   | SRRC          |  0x24              |         |     |     |     |      |
 *   |               |                    |       1 |  26 |  20 |  20 |   14 |
 *   |               |                    |       2 |  26 |  26 |  26 |   14 |
 *   |               |                    |      10 |  26 |  30 |  30 |   14 |
 *   |               |                    |      13 |  26 |  20 |  20 |   14 |
 ******************************************************************************/
sl_status_t sl_wifi_update_gain_table(uint8_t band, uint8_t bandwidth, const uint8_t *payload, uint16_t payload_length)
  SL_DEPRECATED_API_WISECONNECT_3_5;

/***************************************************************************/ /**
 * @brief
 *   Assign the user configurable channel gain table entries in different regions to the module from the user including 11ax SU and 11ax TB.
 * 
 * @details
 *   This function overwrites the default gain tables present in the firmware. Customers can load gain tables for 2.4 GHz-20 MHz. This is a blocking API.
 * 
 * @param[in]  band
 *    1 - 2.4 GHz
 * 
 * @param[in]  bandwidth
 *    0 - 20 MHz
 * 
 * @param[in]  payload
 *    Pass channel gain table entries for different regions in a given array format. 
 *    The gain array format is as follows: \n
 *     No Of Regions, \n
 *     Region code, \n
 *     No Of Channels, \n
 *     | Rate | 11b Gain Value | 11g Gain Value | 11n Gain Value | 11ax(SU Gain) | 11ax(TB Gain) |
 *     |------|----------------|----------------|----------------|---------------|---------------|
 *     |  x   |        x       |        x       |        x       |       x       |       x       |
 *    Repeat the above format for all regions codes.
 *    This table contains the separate gain table entries for 11ax SU and TB
 * 
 * @param[in]  payload_length
 *    Max payload length (table size) in 2.4 GHz is 160 bytes.
 *  
 * @param[in]  x_offset
 *    Bump up offset for 11ax 56 tone RU
 * 
 * @param[in]  y_offset
 *    Bump up offset for 11ax 106 tone RU
 * 
 * @return
 *    sl_status_t. See [Status Codes](https://docs.silabs.com/gecko-platform/latest/platform-common/status) and [Additional Status Codes](../wiseconnect-api-reference-guide-err-codes/sl-additional-status-errors) for details.
 * 
 * @pre Pre-conditions:
 *   @ref sl_wifi_init should be called before this API.
 * 
 * @note
 *   1. This frame must be used only by customers who have done FCC/ETSI/TELEC/KCC certification with their own antenna. Silicon Labs is not liable for inappropriate usage of this frame that may result in violation of FCC/ETSI/TELEC/KCC or any certifications.
 *   2. Internally, firmware maintains two tables: Worldwide table, and Region-based table. Worldwide table is populated by the firmware with maximum power values that the chip can transmit and meet target specs like EVM. Region-based table has a default gain value set.
 *   3. When certifying with a user antenna, the Region has to be set to Worldwide and sweep the power from 0 to 21 dBm. Arrive at a max power level that will pass certifications, especially band-edge.
 *   4. The FCC/ETSI/TELEC/KCC maximum power level should be loaded in an end-to-end mode via WLAN User Gain table. This has to be called done for every boot-up as this information is not saved inside the flash. Region-based user gain table sent by the application is copied onto the Region-based table. SoC uses this table in FCC/ETSI/TELEC/KCC to limit the power and to not violate the allowed limits.
 *   5. For the Worldwide region, the firmware uses the Worldwide table for Tx. For other regions (FCC/ETSI/TELEC/KCC), the firmware uses the min value out of the Worldwide & Region-based table for Tx.  Also, there will be part to part variation across the chips. Offsets that are estimated during the flow of manufacture will be applied as correction factor during normal mode of operation.
 *   6. In the 2.4 GHz band, 40 MHz is not supported.
 *   7. Executing this API overwrites calibration values in certified modules.
 *   8. In FCC-certified modules, this API triggers an error SL_STATUS_SI91X_FEATURE_NOT_AVAILABLE if used, except when in SL_SI91X_TRANSMIT_TEST_MODE mode.
 *  ******************************************************************************/
sl_status_t sl_wifi_update_su_gain_table(uint8_t band,
                                         uint8_t bandwidth,
                                         const uint8_t *payload,
                                         uint16_t payload_length,
                                         uint8_t x_offset,
                                         uint8_t y_offset);

/***************************************************************************/ /**
 * @brief
 *   Configure the 11ax params. This is a blocking API.
 * @pre Pre-conditions:
 * -
 *   This API should be called before @ref sl_wifi_connect
 * @param[in] guard_interval
 *   Period of time delta between two packets in wireless transmission. Valid values : 0 - 3 (0 = 8 us, 1 = 16 us, 2 = 32 us, 3 = 64 us).
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 ******************************************************************************/
sl_status_t sl_wifi_set_11ax_config(uint8_t guard_interval);

/**
* @brief
 *   Start the transmit test.
 *
 * @details
 *   This function starts the transmit test using the provided configuration.
 *
 *   This is a blocking API.
 *
 *   This API is relevant in PER mode
 *
 * @pre-Pre-conditions:
 * - [sl_wifi_init](../wiseconnect-api-reference-guide-wi-fi/wifi-common-api#sl-wifi-init) should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] tx_test_info
 *   Pointer to @ref sl_wifi_transmitter_test_info_t structure containing the configuration for the transmit test.
 *
 * @return
 *   sl_status_t. See [Status Codes] (https://docs.silabs.com/gecko-platform/latest/platform-common/status) and [Additional Status Codes] (../wiseconnect-api-reference-guide-err-codes/sl-additional-status-errors) for details. 
 *
 * @note Before starting Continuous Wave mode, user must start Continuous mode with power and channel values that are intended to be used in Continuous Wave mode i.e. \n
 *       - Start Continuous mode with intended power value and channel values - Pass any valid values for rate and length.
 *       - Stop Continuous mode
 *       - Start Continuous Wave mode
 * @note If user wants to switch continuous wave mode, first need to stop the per mode and again need to give continuous wave mode which user wants to switch.
*************************************************************************************************************************************************************************************************************************/
sl_status_t sl_wifi_transmit_test_start(sl_wifi_interface_t interface,
                                        const sl_wifi_transmitter_test_info_t *tx_test_info);

/**
 * @brief
 *   Stop the transmit test.
 *
 * @details
 *   This function stops the ongoing transmit test on the Si91x device.
 *
 *   This is a blocking API.
 *
 *   This API is relevant in PER mode.
 *
 * @pre-Pre-conditions:
 * - [sl_wifi_init](../wiseconnect-api-reference-guide-wi-fi/wifi-common-api#sl-wifi-init) should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @return
 *   sl_status_t. See [Status Codes] (https://docs.silabs.com/gecko-platform/latest/platform-common/status) and [Additional Status Codes] (../wiseconnect-api-reference-guide-err-codes/sl-additional-status-errors) for details. 
 *
 * @note
 *  User should configure a minimum delay (approx. 10 milliseconds) before and after @ref sl_wifi_transmit_test_start API to observe a stable output at requested dBm level.
*****************************************************************************************************************************************************************************************************************************/
sl_status_t sl_wifi_transmit_test_stop(sl_wifi_interface_t interface);

/***************************************************************************/ /**
 * @brief
 *   Provide feedback of frequency error in KHz.
 *
 * @details
 *   This function provides feedback of the frequency error in KHz. The frequency error is specified using the `sl_wifi_freq_offset_t` structure.
 *
 *   This is a blocking API.
 * 
 * @pre-Pre-conditions:
 * - [sl_wifi_init](../wiseconnect-api-reference-guide-wi-fi/wifi-common-api#sl-wifi-init) should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] frequency_calibration
 *   Pointer to @ref sl_wifi_freq_offset_t structure containing the frequency error in KHz.
 *
 * @return
 *   sl_status_t. See [Status Codes] (https://docs.silabs.com/gecko-platform/latest/platform-common/status) and [Additional Status Codes] (../wiseconnect-api-reference-guide-err-codes/sl-additional-status-errors) for details.
******************************************************************************************************************************************************/
sl_status_t sl_wifi_frequency_offset(sl_wifi_interface_t interface, const sl_wifi_freq_offset_t *frequency_calibration);

/***************************************************************************/ /**
 * @brief
 *   Update Flash/EFuse DPD data.
 *
 * @details
 *   This function updates the Flash/EFuse DPD (Digital Pre-Distortion) data using the provided DPD calibration data.
 *
 *   This is a blocking API.
 *
 * @pre-Pre-conditions:
 * - [sl_wifi_init](../wiseconnect-api-reference-guide-wi-fi/wifi-common-api#sl-wifi-init) and @ref sl_wifi_transmit_test_start should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] dpd_calib_data
 *   Pointer to @ref sl_wifi_dpd_calib_data_t structure containing the DPD calibration data.
 * @return
 *   sl_status_t. See [Status Codes] (https://docs.silabs.com/gecko-platform/latest/platform-common/status) and [Additional Status Codes] (../wiseconnect-api-reference-guide-err-codes/sl-additional-status-errors) for details.
 * @note
 *   In FCC-certified modules, this API will trigger an error SL_STATUS_SI91X_FEATURE_NOT_AVAILABLE if used, except when in SL_WIFI_TRANSMIT_TEST_MODE mode.
 ******************************************************************************/
sl_status_t sl_wifi_dpd_calibration(sl_wifi_interface_t interface, const sl_wifi_dpd_calib_data_t *dpd_calib_data);

/** @} */

/** \addtogroup WIFI_SCANNING_API Scanning
  * \ingroup SL_WIFI_FUNCTIONS
  * @{ */

// Scanning functions
/***************************************************************************/ /**
 * @brief
 *   Initiates a Wi-Fi scan operation on the specified interface, supporting advanced and background scan types.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] optional_ssid
 *   Optional SSID of type @ref sl_wifi_ssid_t can be used to scan for a particular Wi-Fi network
 * @param[in] configuration
 *   @ref sl_wifi_scan_configuration_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 * 	For 911x, advanced scan results are not populated to user.
 *      Default Active Channel time is 100 milliseconds. If the user wants to modify the time, 
 *      sl_wifi_set_advanced_scan_configuration can be called. If the scan_type is not ADV_SCAN, then
 *      the time is for foreground scan. Otherwise, it is used for background scanning. For more details, refer to @ref sl_wifi_scan_type_t.
 *      If the user wants to enable Passive Scanning, user should set the scan_type to SL_WIFI_SCAN_TYPE_PASSIVE.
 *      If the user wants to enable Low Power (LP) mode in Passive Scan, user should enable lp_mode in sl_wifi_scan_configuration_t.
 *      The default channel time for passive scanning is set to 400 milliseconds. If user wants to modify the time, users can call the sl_si91x_set_timeout API to modify the time as per their requirements.
 *      Use the SL_WIFI_SCAN_TYPE_EXTENDED to obtain the scan results that exceed the SL_WIFI_MAX_SCANNED_AP. In this scan type, the number of scan results is not restricted; it is only limited by the amount of dynamic memory that the host can provide.
 *      Default Passive Scan Channel time is 400 milliseconds. If the user wants to modify the time, sl_si91x_set_timeout can be called.
 *      In case of SL_WIFI_SCAN_TYPE_EXTENDED scan type, use @ref sl_wifi_get_stored_scan_results() API to get the scan results; after the scan status callback is received. 
 *      This API is not applicable for ADV_SCAN scan_type in AP mode
 *      This API is supported in AP mode, to scan for - to trigger this, send a scan after sl_wifi_start_ap() API with the SL_WIFI_SCAN_TYPE_ACTIVE scan_type.
 ******************************************************************************/
sl_status_t sl_wifi_start_scan(sl_wifi_interface_t interface,
                               const sl_wifi_ssid_t *optional_ssid,
                               const sl_wifi_scan_configuration_t *configuration);

/***************************************************************************/ /**
 * @brief
 *   Returns the stored scan results of a detailed scan in the user provided scan results array.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in out] extended_scan_parameters
 *    A pointer to a structure of type @ref sl_wifi_extended_scan_result_parameters_t, where the scan results are stored.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 * 	This API will only hold scan results if sl_wifi_start_scan is called with scan type as SL_WIFI_SCAN_TYPE_EXTENDED.
 *  These results are stored until another call to sl_wifi_start_scan is made with scan type as SL_WIFI_SCAN_TYPE_EXTENDED.
 ******************************************************************************/
sl_status_t sl_wifi_get_stored_scan_results(sl_wifi_interface_t interface,
                                            sl_wifi_extended_scan_result_parameters_t *extended_scan_parameters);

/***************************************************************************/ /**
 * @brief
 *   Stops an ongoing advanced Wi-Fi scan operation on the specified interface.
 *   An advanced scan allows the user to perform a scan while the SiWx91x device is in a connected state.
 * @pre Pre-conditions:
 *   This API is applicable only for client interface.
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 * 	For 911x, sl_wifi_stop_scan is ONLY supported for advanced scan.
 ******************************************************************************/
sl_status_t sl_wifi_stop_scan(sl_wifi_interface_t interface);

/***************************************************************************/ /**
 * @brief
 *   Configures advanced scan settings for a Wi-Fi interface and enables instant scan capability.
 * @details
 *   @ref sl_wifi_advanced_scan_configuration_t object that contains the advanced scan configuration.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] configuration
 *   Set advanced scan configuration as identified by @ref sl_wifi_advanced_scan_configuration_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 *   Advance scan is not applicable in AP mode.
 ******************************************************************************/
sl_status_t sl_wifi_set_advanced_scan_configuration(const sl_wifi_advanced_scan_configuration_t *configuration);

/***************************************************************************/ /**
 * @brief
 *   Retrieves the current advanced scan configuration parameters from the Wi-Fi interface.
 * @details
 *   This function should be used after successful Wi-Fi connection.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @param[out] configuration
 *   @ref sl_wifi_advanced_scan_configuration_t object that contains the current advanced scan configuration.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 ******************************************************************************/
sl_status_t sl_wifi_get_advanced_scan_configuration(sl_wifi_advanced_scan_configuration_t *configuration);

/***************************************************************************/ /**
 * @brief
 *   Wait for current scan to complete and store the results in the provided array.
 * @pre Pre-conditions:
 * -
 *   This function also returns when the scan result array is full.
 * @pre Pre-conditions:
 * -
 *   Once the scan result array is full, any further scan results will be lost.
 * @param[in] scan_result_array
 *   Array of @ref sl_wifi_scan_result_t objects to store the scan results.
 * @param[in] max_scan_result_count
 *   The maximum number of scan result objects that can fit in the scan result array.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note 
 *   This API is not supported in the current release.
 ******************************************************************************/
sl_status_t sl_wifi_wait_for_scan_results(sl_wifi_scan_result_t **scan_result_array, uint32_t max_scan_result_count);

/** @} */

/** \addtogroup WIFI_CLIENT_API Client
 * 
 * @note To manage the Wi-Fi interface lifecycle correctly, it's important to use functions like sl_wifi_init(), sl_wifi_connect(), sl_wifi_disconnect(), and sl_wifi_deinit() as intended.
 *       Improper sequencing or combining sl_wifi_* and sl_net_* API functions without following the recommended order may lead to unpredictable or undefined behavior.
 *
 * \ingroup SL_WIFI_FUNCTIONS
 * @{ */
// Wi-Fi Client functions

/***************************************************************************/ /**
 * @brief
 *   Connect to the given Wi-Fi AP.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi client interface as identified by @ref sl_wifi_interface_t
 * @param[in] access_point
 *   @ref sl_wifi_client_configuration_t object that contains the Access Point details.
 * @param[in] timeout_ms
 *   Timeout value in milliseconds. The function aborts and returns when the timeout timer expires.
 *   A value of 0 indicates an asynchronous action.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 *   If channel, band, and BSSID are provided, this API attempts to connect without scanning.
 *   If security_type is SL_WIFI_WPA3/SL_WIFI_WPA3_ENTERPRISE then SL_SI91X_JOIN_FEAT_MFP_CAPABLE_REQUIRED join feature is enabled internally by SDK.
 *   If security_type is SL_WIFI_WPA3_TRANSITION/SL_WIFI_WPA3_TRANSITION_ENTERPRISE then SL_SI91X_JOIN_FEAT_MFP_CAPABLE_REQUIRED join feature is disabled and SL_SI91X_JOIN_FEAT_MFP_CAPABLE_ONLY join feature is enabled internally by SDK.
 *   Default Active Channel time is 100 milliseconds. If the user wants to modify the time, sl_wifi_set_advanced_scan_configuration can be called.
 *   Default Authentication timeout and Association timeout is 300 milliseconds. If the user wants to modify the time, sl_wifi_set_advanced_client_configuration can be called.
 *   Default Keep Alive timeout is 30 milliseconds. If the user wants to modify the time, sl_wifi_set_advanced_client_configuration can be called.
 * @note 
 *   In FCC certified module the behavior is as follows
 *      1. Region configuration is not supported and if triggered will return error SL_STATUS_SI91X_FEATURE_NOT_AVAILABLE.
 *      2. STA mode channels 1 to 11 are actively scanned and 12, 13, 14 are passively scanned.
 *      3. Concurrent mode supports only 1 to 11 channels.
 * @note
 *   It is recommended to set the timeout to 120000 milliseconds to cover the worst case timeout scenario.
 * @note
 *   If this API fails with error SL_STATUS_TIMEOUT, reset the NWP with [sl_net_deinit](../wiseconnect-api-reference-guide-nwk-mgmt/net-interface-functions#sl-net-deinit).
 ******************************************************************************/
sl_status_t sl_wifi_connect(sl_wifi_interface_t interface,
                            const sl_wifi_client_configuration_t *access_point,
                            uint32_t timeout_ms);

/***************************************************************************/ /**
 * @brief
 *   Disconnect the Wi-Fi client interface.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_connect should be called before this API.
 * @param[in] interface
 *   Wi-Fi client interface as identified by @ref sl_wifi_interface_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 *   All si91x, BSD, and IoT sockets associated with the station interface must be closed before invoking this API.
 ******************************************************************************/
sl_status_t sl_wifi_disconnect(sl_wifi_interface_t interface);

/***************************************************************************/ /**
 * @brief
 *   Retrieve the RSSI value of the Access Point to which the Wi-Fi client is connected.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_connect should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] rssi
 *   signal strength (RSSI) in dBm.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 *   The RSSI value is valid only when the Wi-Fi client is connected to an access point. If the Wi-Fi client is disconnected, the RSSI value will be 0xFF.
 ******************************************************************************/
sl_status_t sl_wifi_get_signal_strength(sl_wifi_interface_t interface, int32_t *rssi);

/***************************************************************************/ /**
 * @brief
 *   Get the station Timing Synchronization Function (TSF) time which is synchronised with connected AP beacon TSF. 
 * @pre
 *   Pre-condition: @ref sl_wifi_connect should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[out] tsf 
 *   64-bit TSF time in microseconds stored in @ref sl_wifi_tsf64_t structure.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 *   This API returns an error if the station is not connected to an Access Point or at least one beacon is not received.
 ******************************************************************************/
sl_status_t sl_wifi_get_sta_tsf(sl_wifi_interface_t interface, sl_wifi_tsf64_t *tsf);

/***************************************************************************/ /**
 * @brief
 *   Set the Wi-Fi roaming configuration.
 * @pre Pre-conditions:
 * -
 *   @ref sl_wifi_set_advanced_scan_configuration should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] roam_configuration
 *   @ref sl_wifi_roam_configuration_t object to store Wi-Fi roaming configuration.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 *	 For si91x chips, following ranges are valid:
 *	 trigger_level: [-10, -100] , 
 *   trigger_level_change: [0, 90]
 ******************************************************************************/
sl_status_t sl_wifi_set_roam_configuration(sl_wifi_interface_t interface,
                                           const sl_wifi_roam_configuration_t *roam_configuration);

/***************************************************************************/ /**
 * @brief
 *   Get the Wi-Fi roaming configuration.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[out] roam_configuration
 *   @ref sl_wifi_roam_configuration_t object that will contain the current roam configuration.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note 
 *   This API is not yet implemented.
 ******************************************************************************/
sl_status_t sl_wifi_get_roam_configuration(sl_wifi_interface_t interface,
                                           sl_wifi_roam_configuration_t *roam_configuration);

/***************************************************************************/ /**
 * @brief
 *   Verify the Wi-Fi client configuration is valid and available.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] ap
 *   @ref sl_wifi_client_configuration_t object that contains the details of Access Point.
 * @param[in] timeout_ms
 *   Timeout value in milliseconds. The function aborts and returns when the timeout timer expires.
 *   A timeout value of 0 means the function initiates the verification process and return immediately, without waiting for the process to complete. This indicates that the action would be handled asynchronously.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 ******************************************************************************/
sl_status_t sl_wifi_test_client_configuration(sl_wifi_interface_t interface,
                                              const sl_wifi_client_configuration_t *ap,
                                              uint32_t timeout_ms);

/***************************************************************************/ /**
 * @brief
 *   Load the certificate into the device.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] certificate_type
 *   Certificate type being set
 * @param[in] buffer
 *   Pointer to buffer containing the certificate.
 * @param[in] certificate_length
 *   Length of certificate buffer data.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 ******************************************************************************/
sl_status_t sl_wifi_set_certificate(uint8_t certificate_type, const uint8_t *buffer, uint32_t certificate_length);

/***************************************************************************/ /**
 * @brief
 *   Load the certificate into the device.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] certificate_type
 *   Certificate type being set.
 * @param[in] certificate_index
 *   Certificate to be loaded in specified index.
 * @param[in] buffer
 *   Pointer to buffer containing the certificate.
 * @param[in] certificate_length
 *   Length of certificate buffer data.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 ******************************************************************************/
sl_status_t sl_wifi_set_certificate_with_index(uint8_t certificate_type,
                                               uint8_t certificate_index,
                                               const uint8_t *buffer,
                                               uint32_t certificate_length);

/***************************************************************************/ /**
*   Set the advanced configuration options of a client interface.
 * @pre Pre-conditions:
 * -
 *   @ref sl_wifi_init should be called before this API.
* @param[in] interface
*   Wi-Fi interface as identified by @ref sl_wifi_interface_t
* @param[in] configuration
*   Wi-Fi client advanced configuration. See @ref sl_wifi_advanced_client_configuration_t
* @return
*   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
******************************************************************************/
sl_status_t sl_wifi_set_advanced_client_configuration(sl_wifi_interface_t interface,
                                                      const sl_wifi_advanced_client_configuration_t *configuration);

/***************************************************************************/ /**
 * @brief
 *   Send raw data frame.
 * @pre Pre-conditions:
 * -
 *   @ref sl_wifi_init should be called before this API.
 * -
 *   This API should be invoked only after the module has established a connection in STA or AP mode.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] data
 *   Data buffer.
 * @param[in] data_length
 *   length of the data.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 ******************************************************************************/
sl_status_t sl_wifi_send_raw_data_frame(sl_wifi_interface_t interface, const void *data, uint16_t data_length);

/***************************************************************************/ /**
 * @brief
 *   Configure TWT parameters. Enables a TWT session. This is blocking API.
 * @pre Pre-conditions:
 * -
 *   @ref sl_wifi_connect should be called before this API.
 * @param[in] twt_req
 *   Configurable TWT parameters specified in @ref sl_wifi_twt_request_t.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 ******************************************************************************/
sl_status_t sl_wifi_enable_target_wake_time(const sl_wifi_twt_request_t *twt_req);

/***************************************************************************/ /**
 * @brief
 *   Configure TWT parameters. Disables a TWT session. This is blocking API.
 * @pre Pre-conditions:
 * -
 *   @ref sl_wifi_enable_target_wake_time should be called before this API.
 * @param[in] twt_req
 *   Configurable TWT parameters specified in @ref sl_wifi_twt_request_t.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 ******************************************************************************/
sl_status_t sl_wifi_disable_target_wake_time(const sl_wifi_twt_request_t *twt_req);

/***************************************************************************/ /**
 * @brief
 *   Calculates and configures TWT parameters based on the given inputs. Enables or disables a TWT session. This is blocking API.
 * @pre Pre-conditions:
 * -
 *   @ref sl_wifi_connect should be called before this API.
 * @param[in] twt_selection_req
 *   @ref sl_wifi_twt_selection_t object containing configurable TWT selection parameters.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 ******************************************************************************/
sl_status_t sl_wifi_target_wake_time_auto_selection(sl_wifi_twt_selection_t *twt_selection_req);

/***************************************************************************/ /**
 * @brief
 *   Suspends the TWT agreement corresponding to given flow id and resumes it when suspend duration expires.
 *   This API performs the following actions on TWT agreement: SL_WIFI_SUSPEND_INDEFINITELY, SL_WIFI_RESUME_IMMEDIATELY, SL_WIFI_SUSPEND_FOR_DURATION.
 * @note
 *   The reschedule TWT actions are valid till the end of current TWT agreement. If the TWT agreement is terminated
 *   (TWT tear down or WLAN disconnection), these actions are not retained.
 *   To reapply these actions upon new TWT agreement, the user must re-issue the command.
 * @pre Pre-conditions:
 * -
 *   @ref sl_wifi_connect should be called before this API.
 * @param[in] flow_id
 *   Flow id of the twt agreement.
 * @param[in] twt_action
 *   @ref sl_wifi_reschedule_twt_action_t specifying different actions that can be taken in relation to rescheduling TWT.
 * @param[in] suspend_duration
 *   Time interval until which twt agreement is suspended, value taken in milliseconds.
 * ## The table below outlines the valid values for TWT actions and their corresponding suspend durations:
 * | twt_action           | Valid values for suspend duration |
 * | -------------------- | --------------------------------- |
 * | SL_WIFI_SUSPEND_INDEFINITELY | 0                                 |
 * | SL_WIFI_RESUME_IMMEDIATELY   | 0                                 |
 * | SL_WIFI_SUSPEND_FOR_DURATION | 1 to 86400000                     |
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 ******************************************************************************/
sl_status_t sl_wifi_reschedule_twt(uint8_t flow_id,
                                   sl_wifi_reschedule_twt_action_t twt_action,
                                   uint64_t suspend_duration);

/***************************************************************************/ /**
 * @brief
 *   Send Filter Broadcast Request frame.
 * @pre Pre-conditions:
 * -
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] beacon_drop_threshold
 *   The amount of time that FW waits to receive full beacon. Default value is 5000 ms.
 * @param[in] filter_bcast_in_tim
 *   If this bit is set, then from the next dtim any broadcast data pending bit in TIM indicated will be ignored valid values: 0 - 1.
 * @param[in] filter_bcast_tim_till_next_cmd
 *   0 - filter_bcast_in_tim is valid till disconnect of the STA.
 *   1 - filter_bcast_in_tim is valid till next update by giving the same command.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 ******************************************************************************/
sl_status_t sl_wifi_filter_broadcast(uint16_t beacon_drop_threshold,
                                     uint8_t filter_bcast_in_tim,
                                     uint8_t filter_bcast_tim_till_next_cmd);

/***************************************************************************/ /**
 * @brief
 *   Generate PMK if PSK and SSID are provided. This is a blocking API.
 * @pre Pre-conditions:
 * -
 *   This API should be called after @ref sl_wifi_init and called before @ref sl_wifi_connect.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] type
 *   Possible values of this field are 1, 2, and 3, but we only pass 3 for generation of PMK.
 * @param[in] ssid
 *  SSID of type @ref sl_wifi_ssid_t has the SSID of the access point
 * @param[in] pre_shared_key
 *  Expected parameters are pre-shared key(PSK) of the access point
 * @param[in] pairwise_master_key
 *   PMK array
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 ******************************************************************************/
sl_status_t sl_wifi_get_pairwise_master_key(sl_wifi_interface_t interface,
                                            const uint8_t type,
                                            const sl_wifi_ssid_t *ssid,
                                            const char *pre_shared_key,
                                            uint8_t *pairwise_master_key);

/***************************************************************************/ /**
 * @brief
 *   Configure multicast filter parameters. This is a blocking API.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] multicast_filter_info
 *   Configurable multicast filter parameters specified in @ref sl_wifi_multicast_filter_info_t.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 ******************************************************************************/
sl_status_t sl_wifi_configure_multicast_filter(sl_wifi_multicast_filter_info_t *multicast_filter_info);

/** @} */

/** \addtogroup WIFI_AP_API Access Point
  * \ingroup SL_WIFI_FUNCTIONS
  * @{ */
// Access point functions

/***************************************************************************/ /**
 * @brief
 *   Start a Wi-Fi access point (AP) interface.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] configuration
 *   Wi-Fi AP configuration. See @ref sl_wifi_ap_configuration_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 *   For AP mode with WPA3 security, only SAE-H2E method is supported. SAE hunting and pecking method is not supported.
 *   TKIP encryption mode is not supported. Encryption mode is automatically configured to CCMP.
 *   PMKSA is not supported in WPA3 AP mode.
 * @note   
 *   In FCC-certified modules, 
 *    1. Region configuration is not supported and if triggered returns error SL_STATUS_SI91X_FEATURE_NOT_AVAILABLE.
 *    2. AP supports only 1 to 11 channels. 
 *    3. AP does not advertise the Country IE.
 ******************************************************************************/
sl_status_t sl_wifi_start_ap(sl_wifi_interface_t interface, const sl_wifi_ap_configuration_t *configuration);

/***************************************************************************/ /**
 * @brief
 *   Reconfigure the dynamic parameters of a Wi-Fi Access point (AP) interface.
 * @pre
 *   @ref sl_wifi_start_ap should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] config
 *   Wi-Fi AP dynamic configuration. See @ref sl_wifi_ap_reconfiguration_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 *   The access point continues to transmit beacons when a client is connected, regardless of the beacon_stop configuration.
 ******************************************************************************/
sl_status_t sl_wifi_reconfigure_ap(sl_wifi_interface_t interface, sl_wifi_ap_reconfiguration_t config);

/***************************************************************************/ /**
 * @brief
 *   Set the configuration of a running Wi-Fi AP.
 *   If the new configuration modifies vital settings such as SSID or security, the AP will be stopped and restarted automatically.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] configuration
 *   Wi-Fi AP configuration. See @ref sl_wifi_ap_configuration_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 *   This API is not yet implemented.
 ******************************************************************************/
sl_status_t sl_wifi_set_ap_configuration(sl_wifi_interface_t interface,
                                         const sl_wifi_ap_configuration_t *configuration);

/***************************************************************************/ /**
 * @brief
 *   Get the configuration of a Wi-Fi AP interface.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[out] configuration
 *   @ref sl_wifi_ap_configuration_t object that contains the AP configuration.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 ******************************************************************************/
sl_status_t sl_wifi_get_ap_configuration(sl_wifi_interface_t interface, sl_wifi_ap_configuration_t *configuration);

/***************************************************************************/ /**
 * @brief
 *   Set the advanced configuration options of a running Wi-Fi access point (AP).
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] configuration
 *   Wi-Fi AP advanced configuration. See @ref sl_wifi_advanced_ap_configuration_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 *   This API is not yet implemented.
 ******************************************************************************/
sl_status_t sl_wifi_set_advanced_ap_configuration(sl_wifi_interface_t interface,
                                                  const sl_wifi_advanced_ap_configuration_t *configuration);

/***************************************************************************/ /**
 * @brief
 *   Get the advanced configuration options of a running Wi-Fi access point interface.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[out] configuration
 *   @ref sl_wifi_advanced_ap_configuration_t object that will contain the AP advanced configuration.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 *   This API is not yet implemented.
 ******************************************************************************/
sl_status_t sl_wifi_get_advanced_ap_configuration(sl_wifi_interface_t interface,
                                                  const sl_wifi_advanced_ap_configuration_t *configuration);

/***************************************************************************/ /**
 * @brief
 *   Stop Wi-Fi access point.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_start_ap should be called before this API.
 * @param[in] interface
 *   Wi-Fi Access Point interface as identified by @ref sl_wifi_interface_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 *   All si91x, BSD, and IoT sockets associated with the AP interface must be closed before invoking this API.
 ******************************************************************************/
sl_status_t sl_wifi_stop_ap(sl_wifi_interface_t interface);

/***************************************************************************/ /**
 * @brief
 *   Disconnects a client with the specified MAC address from Access Point (AP).
 * @details
 *   Use this function to disassociate (disconnect) a client from Access Point.
 *   This API is used when the device is in AP mode.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_start_ap should be called before this API.
 * @param[in] interface
 *   Wi-Fi Access Point interface as identified by @ref sl_wifi_interface_t
 * @param[in] mac
 *   Wi-Fi client's MAC address of type [sl_mac_address_t](../wiseconnect-api-reference-guide-nwk-mgmt/sl-net-types#sl-mac-address-t)
 * @param[in] reason
 *   Reason for de-authentication as specified in @ref sl_wifi_deauth_reason_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note 
 *   This API is supported only in AP mode.
 ******************************************************************************/
sl_status_t sl_wifi_disconnect_ap_client(sl_wifi_interface_t interface,
                                         const sl_mac_address_t *mac,
                                         sl_wifi_deauth_reason_t reason);

/***************************************************************************/ /**
 * @brief
 *   Return the Wi-Fi client information of all clients connected to the AP.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_start_ap should be called before this API.
 * @param[in] interface
 *   Wi-Fi Access Point interface as identified by @ref sl_wifi_interface_t
 * @param[out] client_info
 *   @ref sl_wifi_client_info_response_t object to store the client info.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note 
 *   This API is supported only in AP mode.
 ******************************************************************************/
sl_status_t sl_wifi_get_ap_client_info(sl_wifi_interface_t interface, sl_wifi_client_info_response_t *client_info);

/***************************************************************************/ /**
 * @brief
 *   Return a list of Wi-Fi clients connected to the Wi-Fi access point.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_start_ap should be called before this API.
 * @param[in] interface
 *   Wi-Fi Access Point interface as identified by @ref sl_wifi_interface_t
 * @param[in] client_list_count
 *   The number of [sl_mac_address_t](../wiseconnect-api-reference-guide-nwk-mgmt/sl-net-types#sl-mac-address-t) objects the client_list can store.
 * @param[out] client_list
 *   A pointer to an array of client_list_count number of [sl_mac_address_t](../wiseconnect-api-reference-guide-nwk-mgmt/sl-net-types#sl-mac-address-t) objects where the client list will be copied to.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note 
 *   This API is supported only in AP mode.
 ******************************************************************************/
sl_status_t sl_wifi_get_ap_client_list(sl_wifi_interface_t interface,
                                       uint16_t client_list_count,
                                       sl_mac_address_t *client_list);

/***************************************************************************/ /**
 * @brief
 *   Provide the number of Wi-Fi clients connected to the Wi-Fi access point
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_start_ap should be called before this API.
 * @param[in] interface
 *   Wi-Fi Access Point interface as identified by @ref sl_wifi_interface_t
 * @param[out] client_count
 *   A uint32_t pointer that will store the number of associated clients.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note 
 *   This API is supported only in AP mode.
 ******************************************************************************/
sl_status_t sl_wifi_get_ap_client_count(sl_wifi_interface_t interface, uint32_t *client_count);

/** @} */

/** \addtogroup WIFI_POWER_API Power and Performance
  * \ingroup SL_WIFI_FUNCTIONS
  * @{ */
// Power management functions

/***************************************************************************/ /**
 * @brief
 *   Set Wi-Fi performance profile.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] profile
 *   Wi-Fi performance profile as indicated by [sl_wifi_performance_profile_t](../wiseconnect-api-reference-guide-si91x-driver/sl-wifi-performance-profile-t)
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 *   For SI91x chips Enhanced MAX PSP is supported when profile is set to ASSOCIATED_POWER_SAVE_LOW_LATENCY and SL_SI91X_ENABLE_ENHANCED_MAX_PSP bit is enabled in config feature bitmap
 * @note
 *   Moving forward, this API will be deprecated. Instead, use the [sl_wifi_set_performance_profile_v2](../wiseconnect-api-reference-guide-wi-fi/wifi-power-api#sl-wifi-set-performance-profile-v2) API. This is retained for backward compatibility.
 * @note
 *   For further more details on connected and non-connected mode please refer https://www.silabs.com/documents/public/application-notes/an1430-siwx917-soc-low-power.pdf.
 ******************************************************************************/
sl_status_t sl_wifi_set_performance_profile(const sl_wifi_performance_profile_t *profile)
  SL_DEPRECATED_API_WISECONNECT_3_5;

/***************************************************************************/ /**
 * @brief
 *   Set Wi-Fi performance profile.
 * @pre Pre-conditions:
 * -
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] profile
 *   Wi-Fi performance profile as indicated by [sl_wifi_performance_profile_v2_t](../wiseconnect-api-reference-guide-si91x-driver/sl-wifi-performance-profile-v2-t) 
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 *   For SI91x chips Enhanced MAX PSP is supported when profile is set to ASSOCIATED_POWER_SAVE_LOW_LATENCY and SL_SI91X_ENABLE_ENHANCED_MAX_PSP bit is enabled in config feature bitmap
 * @note
 *   This v2 API is defined due to a new configuration member beacon_miss_ignore_limit added to the structure sl_wifi_performance_profile_v2_t.
 *   Default value for beacon_miss_ignore_limit is 1. Recommended max value is 10. Higher value may cause interop issues.
 * @note
 *   For further more details on connected and non-connected mode please refer https://www.silabs.com/documents/public/application-notes/an1430-siwx917-soc-low-power.pdf.
 ******************************************************************************/
sl_status_t sl_wifi_set_performance_profile_v2(const sl_wifi_performance_profile_v2_t *profile);

/***************************************************************************/ /**
 * @brief
 *   Get Wi-Fi performance profile.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @param[out] profile
 *   Wi-Fi performance profile as indicated by [sl_wifi_performance_profile_t](../wiseconnect-api-reference-guide-si91x-driver/sl-wifi-performance-profile-t)
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 *   Moving forward, this API will be deprecated. Instead, use the [sl_wifi_get_performance_profile_v2](../wiseconnect-api-reference-guide-wi-fi/wifi-power-api#sl-wifi-get-performance-profile-v2) API. This is retained for backward compatibility.
 ******************************************************************************/
sl_status_t sl_wifi_get_performance_profile(sl_wifi_performance_profile_t *profile) SL_DEPRECATED_API_WISECONNECT_3_5;

/***************************************************************************/ /**
 * @brief
 *   Get Wi-Fi performance profile.
 * @pre Pre-conditions:
 * -
 *   @ref sl_wifi_init should be called before this API.
 * @param[out] profile
 *   Wi-Fi performance profile as indicated by [sl_wifi_performance_profile_v2_t](../wiseconnect-api-reference-guide-si91x-driver/sl-wifi-performance-profile-v2-t)
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 ******************************************************************************/
sl_status_t sl_wifi_get_performance_profile_v2(sl_wifi_performance_profile_v2_t *profile);

/** @} */

// "Monitor Mode" functions

/***************************************************************************/ /**
 * @brief
 *   Enable monitor (promiscuous) mode on the Wi-Fi device.
 *   In this mode, all types of Wi-Fi frames will be forwarded to the host.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 *   This API is not yet implemented.
 ******************************************************************************/
sl_status_t sl_wifi_enable_monitor_mode(sl_wifi_interface_t interface);

/***************************************************************************/ /**
 * @brief
 *   Disable monitor mode on the Wi-Fi interface.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 *   This API is not yet implemented.
 ******************************************************************************/
sl_status_t sl_wifi_disable_monitor_mode(sl_wifi_interface_t interface);

// P2P functions

/***************************************************************************/ /**
 * @brief
 *   Start Wi-Fi direct discovery.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] configuration
 *   P2P configuration as identified by @ref sl_wifi_p2p_configuration_t
 * @param[in] credential_id
 *   Credential ID as identified by @ref sl_wifi_credential_id_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 *   This API is not yet implemented.
 ******************************************************************************/
sl_status_t sl_wifi_start_p2p_discovery(sl_wifi_interface_t interface,
                                        const sl_wifi_p2p_configuration_t *configuration,
                                        sl_wifi_credential_id_t credential_id);

/***************************************************************************/ /**
 * @brief
 *   Start Wi-Fi direct connection.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] configuration
 *   P2P configuration as identified by @ref sl_wifi_p2p_configuration_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 *   This API is not yet implemented.
 ******************************************************************************/
sl_status_t sl_wifi_p2p_connect(sl_wifi_interface_t interface, const sl_wifi_p2p_configuration_t *configuration);

/** \addtogroup WIFI_WPS_API Wi-Fi Protected Setup
  * \ingroup SL_WIFI_FUNCTIONS
  * @{ */
// WPS functions

/***************************************************************************/ /**
 * @brief
 *   Generate Wi-Fi Protected Setup (WPS) pin.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @param[out] response
 *   @ref sl_wifi_wps_pin_t object that will contain the WPS pin.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 ******************************************************************************/
sl_status_t sl_wifi_generate_wps_pin(sl_wifi_wps_pin_t *response);

/***************************************************************************/ /**
 * @brief
 *   Start Wi-Fi Protected Setup (WPS).
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_start_ap should be called before this API.
 * @param[in] interface
 *   Wi-Fi Access Point interface as identified by @ref sl_wifi_interface_t
 * @param[in] mode
 *   WPS mode as identified by @ref sl_wifi_wps_mode_t
 * @param[in] optional_wps_pin
 *   WPS pin object @ref sl_wifi_wps_pin_t when @ref SL_WIFI_WPS_PIN_MODE is used.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note 
 *   This API is supported only in AP mode.
 ******************************************************************************/
sl_status_t sl_wifi_start_wps(sl_wifi_interface_t interface,
                              sl_wifi_wps_mode_t mode,
                              const sl_wifi_wps_pin_t *optional_wps_pin);

/***************************************************************************/ /**
 * @brief
 *   Stop current running Wi-Fi Protected Setup (WPS).
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_start_wps should be called before this API.
 * @param[in] interface
 *   Wi-Fi Access Point interface as identified by @ref sl_wifi_interface_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note 
 *   This API is supported only in AP mode.
 ******************************************************************************/
sl_status_t sl_wifi_stop_wps(sl_wifi_interface_t interface);

/** @} */

/** \addtogroup WIFI_DEBUG_API Debugging
  * \ingroup SL_WIFI_FUNCTIONS
  * @{ */

// Debug functions
/***************************************************************************/ /**
 * @brief
 *   Return Wi-Fi operational statistics.
 * @pre Pre-conditions:
 * -
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[out] statistics
 *   @ref sl_wifi_statistics_t object that contains Wi-Fi statistics.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 ******************************************************************************/
sl_status_t sl_wifi_get_statistics(sl_wifi_interface_t interface, sl_wifi_statistics_t *statistics);

/***************************************************************************/ /**
 * @brief
 *   Return Wi-Fi operational statistics.
 * @pre Pre-conditions:
 * -
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[out] operational_statistics
 *   @ref sl_wifi_operational_statistics_t object that contains Wi-Fi statistics.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 ******************************************************************************/
sl_status_t sl_wifi_get_operational_statistics(sl_wifi_interface_t interface,
                                               sl_wifi_operational_statistics_t *operational_statistics);

/***************************************************************************/ /**
 * @brief
 *   Start collecting statistical data.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] channel
 *   Provides the statistics report on the channel specified by @ref sl_wifi_channel_t.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 ******************************************************************************/
sl_status_t sl_wifi_start_statistic_report(sl_wifi_interface_t interface, sl_wifi_channel_t channel);

/***************************************************************************/ /**
 * @brief
 *   Stop collecting statistical data.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_start_statistic_report should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 ******************************************************************************/
sl_status_t sl_wifi_stop_statistic_report(sl_wifi_interface_t interface);

/** @} */

/***************************************************************************/ /**
 * @brief
 *   Return the status of the Wi-Fi device.
 * @param[out] wifi_status
 *   @ref sl_wifi_status_t object that will contain the Wi-Fi status.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 *   This API is not yet implemented.
 ******************************************************************************/
sl_status_t sl_wifi_get_status(sl_wifi_status_t *wifi_status);

/** \addtogroup WIFI_TRANSCEIVER_API Wi-Fi Transceiver
  * \ingroup SL_WIFI_FUNCTIONS
  * @{ */
// Wi-Fi Transceiver functions
/***************************************************************************/ /**
 * @brief
 *   Start a Wi-Fi Transceiver interface.
 * @pre Pre-conditions:
 * - @ref sl_wifi_init shall be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] config
 *   Wi-Fi Transceiver configuration. See @ref sl_wifi_transceiver_configuration_t
 * @return
 *   sl_status_t. See [Status Codes](../../wiseconnect-api-reference-guide-err-codes/pages/sl-additional-status-errors). Possible Error Codes:
 *   - `0x11` - SL_STATUS_NOT_INITIALIZED
 *   - `0x21` - SL_STATUS_SI91X_COMMAND_GIVEN_IN_INVALID_STATE
 *   - `0x22` - SL_STATUS_NULL_POINTER
 *   - `0x0B65` - SL_STATUS_TRANSCEIVER_INVALID_CHANNEL
 *   - `0x0B67` - SL_STATUS_TRANSCEIVER_INVALID_CONFIG
 * @note This API is only supported in Wi-Fi Transceiver opermode (7).
 * @note `sl_wifi_transceiver_up` internally calls @ref sl_wifi_set_transceiver_parameters and @ref sl_wifi_transceiver_set_channel. Additionally, DUT MAC address is queried using @ref sl_wifi_get_mac_address and used as Addr2 for TX data packets.
 ******************************************************************************/
sl_status_t sl_wifi_transceiver_up(sl_wifi_interface_t interface, sl_wifi_transceiver_configuration_t *config);

/***************************************************************************/ /**
 * @brief Configure channel from the host.
 *
 * @pre Pre-conditions:
 * - @ref sl_wifi_init shall be called before this API.
 *
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] channel
 *   Application shall decide the channel at which device operates and transmits frames. See @ref sl_wifi_transceiver_set_channel_t.
 *             | Param                  | Description
 *             |:-----------------------|:-----------------------------------------------------------
 *             |channel                 | Primary channel number. Valid channels are 1-14.
 *             |band                    | Reserved
 *             |bandwidth               | Reserved
 *             |tx_power                | Max transmission power
 *
 * @return
 *   sl_status_t. See [Status Codes](../../wiseconnect-api-reference-guide-err-codes/pages/sl-additional-status-errors). Possible Error Codes:
 *   - `0x11` - SL_STATUS_NOT_INITIALIZED
 *   - `0x21` - SL_STATUS_SI91X_COMMAND_GIVEN_IN_INVALID_STATE
 *   - `0x0B65` - SL_STATUS_TRANSCEIVER_INVALID_CHANNEL
 *
 * @note This API is only supported in Wi-Fi Transceiver opermode (7).
 * @note This is a blocking API.
 * @note The effective transmit power is subject to regional and device limitations. If the specified transmit power exceeds the maximum supported value for that region, the transmission will occur at the maximum supported transmit power.
 *
 * Sample command usage:
 * @code
 * // Initialize channel
 * sl_wifi_transceiver_set_channel_t channel = {
 *   .chan_info.channel = 14,
 * };
 *
 * // Set channel
 * sl_wifi_transceiver_set_channel(SL_WIFI_TRANSCEIVER_INTERFACE, channel);
 * @endcode 
 *
 ******************************************************************************/
sl_status_t sl_wifi_transceiver_set_channel(sl_wifi_interface_t interface, sl_wifi_transceiver_set_channel_t channel);

/***************************************************************************/ /**
 * @brief This API shall be used to configure the CWmin, CWmax, and AIFSN per access category and retransmit count.
 *
 * @pre Pre-conditions:
 * - @ref sl_wifi_init shall be called before this API.
 *
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 *
 * @param[in] params
 *   Transceiver parameters as identified by @ref sl_wifi_transceiver_parameters_t. Shall be used to set/get the contention parameters per access category and the retransmit count in MAC layer.
 *
 * @return
 *   sl_status_t. See [Status Codes](../../wiseconnect-api-reference-guide-err-codes/pages/sl-additional-status-errors). Possible Error Codes:
 *   - `0x11` - SL_STATUS_NOT_INITIALIZED
 *   - `0x21` - SL_STATUS_SI91X_COMMAND_GIVEN_IN_INVALID_STATE
 *   - `0x22` - SL_STATUS_NULL_POINTER
 *   - `0x0B67` - SL_STATUS_TRANSCEIVER_INVALID_CONFIG
 *
 * @note This API is only supported in Wi-Fi Transceiver opermode (7).
 * @note Set is allowed only once before the first call to sl_wifi_transceiver_set_channel API.
 * @note This API is optional. Default configurations are used if API is not called.
 * @note This is a blocking API.
 *
 * Sample command usage:
 * @code
 * // Initialize parameters
 * sl_wifi_transceiver_parameters_t params = {
 *   .set = 1,
 *   .retransmit_count = 15,
 *   .cw_params[0].aifsn = 3,
 * };
 *
 * // Set parameters
 * sl_wifi_set_transceiver_parameters(SL_WIFI_TRANSCEIVER_INTERFACE, &params);
 * @endcode
 ******************************************************************************/
sl_status_t sl_wifi_set_transceiver_parameters(sl_wifi_interface_t interface, sl_wifi_transceiver_parameters_t *params);

/***************************************************************************/ /**
 * @brief When new peer is added or deleted from the network, application shall call this API to update peer information to the MAC layer.
 *
 * @pre Pre-conditions:
 * - @ref sl_wifi_transceiver_set_channel shall be called before this API.
 *
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] peer
 *   Peer to be added/deleted in MAC layer. See @ref sl_wifi_transceiver_peer_update_t.
 *
 * @return
 *   sl_status_t. See [Status Codes](../../wiseconnect-api-reference-guide-err-codes/pages/sl-additional-status-errors). Possible Error Codes:
 *   - `0x11` - SL_STATUS_NOT_INITIALIZED
 *   - `0x0B44` - SL_STATUS_WIFI_INTERFACE_NOT_UP
 *   - `0x0B63` - SL_STATUS_TRANSCEIVER_INVALID_MAC_ADDRESS
 *   - `0x0B66` - SL_STATUS_TRANSCEIVER_INVALID_DATA_RATE
 *   - `0x10096` - SL_STATUS_SI91X_TRANSCEIVER_PEER_DS_FEAT_DISABLED
 *   - `0x10097` - SL_STATUS_SI91X_TRANSCEIVER_PEER_ALREADY_EXISTS
 *   - `0x10098` - SL_STATUS_SI91X_TRANSCEIVER_MAX_PEER_LIMIT_REACHED
 *   - `0x10099` - SL_STATUS_SI91X_TRANSCEIVER_PEER_NOT_FOUND
 *
 * @note This API is only supported in Wi-Fi Transceiver opermode (7).
 * @note This is a blocking API.
 * @note MAC layer supports storing up to 100 peers.
 * @note To add peers in MAC layer, it is mandatory to enable SL_WIFI_FEAT_TRANSCEIVER_MAC_PEER_DS_SUPPORT/BIT(13) in [sl_wifi_device_configuration_t](../wiseconnect-api-reference-guide-si91x-driver/sl-wifi-device-configuration-t) feature_bit_map passed in @ref sl_wifi_init.
 *
 * Sample command usage:
 * @code
 * // Initialize peer
 * sl_wifi_transceiver_peer_update_t peer;
 * uint8_t peer_mac[6] = {0x00, 0x23, 0xa7, 0x20, 0x21, 0x24};
 * memcpy(peer.peer_mac_address, peer_mac, 6);
 * peer.peer_supported_rate_bitmap = PEER_DS_BITMAP_DATA_RATE_48 | PEER_DS_BITMAP_DATA_RATE_54;
 * peer.flags                     |= BIT(0)); // Set bit 0 to add peer
 *
 * // Add peer
 * sl_wifi_update_transceiver_peer_list(SL_WIFI_TRANSCEIVER_INTERFACE, peer);
 * @endcode
 ******************************************************************************/
sl_status_t sl_wifi_update_transceiver_peer_list(sl_wifi_interface_t interface, sl_wifi_transceiver_peer_update_t peer);

/***************************************************************************/ /**
 * @brief This API configures the multicast MAC address to filter Rx multicast packets.
 *
 * @pre Pre-conditions:
 * - @ref sl_wifi_transceiver_set_channel shall be called before this API.
 *
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] mcast
 *   Multicast MAC address to be added/deleted from MAC layer for filtering. See @ref sl_wifi_transceiver_mcast_filter_t.
 *
 * @return
 *   sl_status_t. See [Status Codes](../../wiseconnect-api-reference-guide-err-codes/pages/sl-additional-status-errors). Possible Error Codes:
 *   - `0x11` - SL_STATUS_NOT_INITIALIZED
 *   - `0x21` - SL_STATUS_INVALID_PARAMETER
 *   - `0x0B44` - SL_STATUS_WIFI_INTERFACE_NOT_UP
 *   - `0x0B63` - SL_STATUS_TRANSCEIVER_INVALID_MAC_ADDRESS
 *
 * @note This API is only supported in Wi-Fi Transceiver opermode (7).
 * @note This API can be called dynamically.
 * @note Maximum of two multicast MAC addresses can be configured for filtering.
 *
 * Sample command usage:
 * @code
 * // Initialize multicast filter address structure
 * sl_wifi_transceiver_mcast_filter_t mcast;
 * uint8_t filter_mac[6] = { 0x01, 0x00, 0x5e, 0x00, 0x01, 0x01 };
 * mcast.flags |= BIT(0);
 * mcast.num_of_mcast_addr = 1;
 * memcpy(mcast.mac[0], filter_mac, 6);
 *
 * // Add MAC address to be filtered
 * sl_wifi_set_transceiver_multicast_filter(SL_WIFI_TRANSCEIVER_INTERFACE, mcast);
 * @endcode
 ******************************************************************************/
sl_status_t sl_wifi_set_transceiver_multicast_filter(sl_wifi_interface_t interface,
                                                     sl_wifi_transceiver_mcast_filter_t mcast);

/***************************************************************************/ /**
 * @brief This API shall flush the entire software buffer pool.
 *
 * @pre Pre-conditions:
 * - @ref sl_wifi_transceiver_set_channel shall be called before this API.
 *
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 *
 * @return
 *   sl_status_t. See [Status Codes](../../wiseconnect-api-reference-guide-err-codes/pages/sl-additional-status-errors). Possible Error Codes:
 *   - `0x11` - SL_STATUS_NOT_INITIALIZED
 *   - `0x0B44` - SL_STATUS_WIFI_INTERFACE_NOT_UP
 *
 * @note This API is only supported in Wi-Fi Transceiver opermode (7).
 * @note All priority queues shall be flushed.
 *
 * Sample command usage:
 * @code
 * sl_wifi_flush_transceiver_data(SL_WIFI_TRANSCEIVER_INTERFACE);
 * @endcode
 ******************************************************************************/
sl_status_t sl_wifi_flush_transceiver_data(sl_wifi_interface_t interface);

/***************************************************************************/ /**
 * @brief Host shall call this API to encapsulate the data with 802.11 MAC header and send it to MAC layer.
 *
 * @pre Pre-conditions:
 * - @ref sl_wifi_transceiver_set_channel shall be called before this API.
 *
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 *
 * @param[in] control
 *   API uses metadata for preparing data packet along with MAC header for sending to MAC layer. See @ref sl_wifi_transceiver_tx_data_control_t. 
 * @param[in] payload
 *   Pointer to payload (encrypted by host) to be sent to LMAC.
 * @param[in] payload_len
 *   Length of the payload. Valid range is 1 - 2020 bytes.
 *
 * @return
 *   sl_status_t. See [Status Codes](../../wiseconnect-api-reference-guide-err-codes/pages/sl-additional-status-errors). Possible Error Codes:
 *   - `0x11` - SL_STATUS_NOT_INITIALIZED
 *   - `0x0B44` - SL_STATUS_WIFI_INTERFACE_NOT_UP
 *   - `0x0B63` - SL_STATUS_TRANSCEIVER_INVALID_MAC_ADDRESS
 *   - `0x0B64` - SL_STATUS_TRANSCEIVER_INVALID_QOS_PRIORITY
 *   - `0x0B66` - SL_STATUS_TRANSCEIVER_INVALID_DATA_RATE
 *   - `0x21` - SL_STATUS_INVALID_PARAMETER
 *   - `0x22` - SL_STATUS_NULL_POINTER
 * 
 * #### Format of encapsulated data sent to MAC ####
 * | Field name | Frame Control  | Duration | Addr1 | Addr2 | Adddr3 | Seq Ctrl | Addr4                  | QoS ctrl              | Payload (LLC + Data)  |
 * |:-----------|:---------------|:---------|:------|:------|:-------|:---------|:-----------------------|:----------------------|:----------------------|
 * | Size(bytes)| 2              | 2        | 6     | 6     | 6      | 2        | 6 (Optionally present) | 2 (Optionally present)| Variable              |
 *
 * @note This API is only supported in Wi-Fi Transceiver opermode (7).
 * @note Once sl_wifi_send_transceiver_data() returns, the calling API is responsible for freeing control and payload. The calling API refers to the function that invoked sl_wifi_send_transceiver_data().
 * @note On chip MAC level encryption is not supported in transceiver mode.
 * @note This is not a blocking API. Callback SL_WIFI_TRANSCEIVER_TX_DATA_STATUS_CB can be registered to get the status report from firmware.
 * @note Only 11b/g rates shall be supported.
 * @note It is recommended to use basic rate for multicast/broadcast packets.
 * @note Sample command usage:
 * @code
 * // Prepare payload
 * <Prepare data payload in "payload" buffer>
 * <Initialize data control block @ref sl_wifi_transceiver_tx_data_control_t >
 * control->ctrl_flags = BIT(0) | BIT(1) | BIT(2) | BIT(5) | BIT(6) | BIT(7);   // Configure control flags to enable 4-addr MAC header, QoS frame, fixed data rate, send status report, extended information, and immediate transfer.
 * control->priority   = 2;                                 // Voice priority queue
 * control->rate       = SL_WIFI_DATA_RATE_36;
 * control->token      = token;
 * control->ctrl_flags1= BIT(0);    // Set if it is last packet of that channel
 * control->channel    = channel_number;
 * control->tx_power   = transmission_power;
 * @note If BIT(6) of ctrl_flags is set, then fill channel, tx_power, and ctrl_flags1 BIT(0) information.
 * <Fill control addr1, addr2, addr3 and addr4(optionally) with 6 byte RA, NWP, DA and SA MAC addresses respectively>
 *
 * // Call API to encapsulate the data with 802.11 MAC header and send it to MAC layer.
 * sl_wifi_send_transceiver_data(SL_WIFI_TRANSCEIVER_INTERFACE, control, payload, payload_len);
 * @endcode
 ******************************************************************************/
sl_status_t sl_wifi_send_transceiver_data(sl_wifi_interface_t interface,
                                          sl_wifi_transceiver_tx_data_control_t *control,
                                          const uint8_t *payload,
                                          uint16_t payload_len);
/** @} */
