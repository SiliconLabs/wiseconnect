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
#include "sl_wifi_host_interface.h"
#include <stdint.h>
#include <stdbool.h>

/// Default Wi-Fi scan configuration
extern const sl_wifi_scan_configuration_t default_wifi_scan_configuration;

/// Default Wi-Fi AP configuration
extern const sl_wifi_ap_configuration_t default_wifi_ap_configuration;

/** \addtogroup WIFI_COMMON_API Common
  * \ingroup SL_WIFI_FUNCTIONS
  * @{ */

/***************************************************************************/ /**
 * @brief
 *   Initialize the Wi-Fi device.
 * @param[in] configuration
 *   @ref sl_wifi_device_configuration_t object that contains Wi-Fi device configuration.
 * @param[in] event_handler
 *   Wi-Fi event handler function.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note 
 *   This function should be called before calling any other sl_wifi functions.
 ******************************************************************************/
sl_status_t sl_wifi_init(const sl_wifi_device_configuration_t *configuration, sl_wifi_event_handler_t event_handler);

/***************************************************************************/ /**
 * @brief
 *   De-initialize the Wi-Fi device.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_wifi_deinit(void);

/***************************************************************************/ /**
 * @brief
 *   Check if Wi-Fi interface is up.
 * @pre 
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
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[out] version
 *   @ref sl_wifi_version_string_t object that contains the version string.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_wifi_get_firmware_version(sl_wifi_version_string_t *version);

/***************************************************************************/ /**
 * @brief
 *   Return Wi-Fi operational statistics.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[out] statistics
 *   @ref sl_wifi_statistics_t object that contains Wi-Fi statistics.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_wifi_get_statistics(sl_wifi_interface_t interface, sl_wifi_statistics_t *statistics);

/***************************************************************************/ /**
 * @brief
 *   Set the default interface.
 *   Used by API when @ref SL_WIFI_DEFAULT_INTERFACE is provided.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 ******************************************************************************/
void sl_wifi_set_default_interface(sl_wifi_interface_t interface);

/***************************************************************************/ /**
 * @brief
 *   Get the default interface.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @return
 *   @ref sl_wifi_interface_t previously set by @ref sl_wifi_set_default_interface
 ******************************************************************************/
sl_wifi_interface_t sl_wifi_get_default_interface(void);

/***************************************************************************/ /**
 * @brief
 *   Get the Wi-Fi interface MAC address.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[out] mac
 *   @ref sl_mac_address_t object that contains the MAC address of the interface.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_wifi_get_mac_address(sl_wifi_interface_t interface, sl_mac_address_t *mac);

/***************************************************************************/ /**
 * @brief
 *   Set the Wi-Fi interface MAC address.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] mac 
 *   @ref sl_mac_address_t object to store the MAC address.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 *   This API is not supported by RS9116, Si917 when called directly due to firmware constraints.
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
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[out] max_tx_power
 *   A variable that contains current maximum transmit power in dBm.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 *   This function gets the transmit power for a particular radio interface: SL_WIFI_2_4GHZ_INTERFACE or SL_WIFI_5GHZ_INTERFACE.
 ******************************************************************************/
sl_status_t sl_wifi_get_max_tx_power(sl_wifi_interface_t interface, uint8_t *max_tx_power);

/***************************************************************************/ /**
 * @brief
 *   Set the maximum Wi-Fi transmit power.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] max_tx_power
 *   Maximum transmit power to set in dBm.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 *   This function sets the transmit power for a particular radio interface: SL_WIFI_2_4GHZ_INTERFACE or SL_WIFI_5GHZ_INTERFACE.
 *   Eg: Setting transmit power for client interface at 2.4GHz will also set transmit power of the AP interface at 2.4GHz.
 ******************************************************************************/
sl_status_t sl_wifi_set_max_tx_power(sl_wifi_interface_t interface, uint8_t max_tx_power);

/***************************************************************************/ /**
 * @brief
 *   Set the Wi-Fi antenna for an interface.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] antenna
 *   Antenna to select as identified by @ref sl_wifi_antenna_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_wifi_set_antenna(sl_wifi_interface_t interface, sl_wifi_antenna_t antenna);

/***************************************************************************/ /**
 * @brief
 *   Get the Wi-Fi antenna for an interface.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[out] antenna
 *   @ref sl_wifi_antenna_t object that contains current antenna selection.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_wifi_get_antenna(sl_wifi_interface_t interface, sl_wifi_antenna_t *antenna);

/***************************************************************************/ /**
 * @brief
 *   Get the current channel for the given Wi-Fi interface.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[out] channel
 *   @ref sl_wifi_channel_t object that contains current channel information.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_wifi_get_channel(sl_wifi_interface_t interface, sl_wifi_channel_t *channel);

/***************************************************************************/ /**
 * @brief
 *   Set the channel for the given Wi-Fi Access Point interface.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] channel
 *   Channel as identified by @ref sl_wifi_channel_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_wifi_set_channel(sl_wifi_interface_t interface, sl_wifi_channel_t channel);

/***************************************************************************/ /**
 * @brief
 *   Set the Wi-Fi transmit rate for the given 802.11 protocol on the specified Wi-Fi interface.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] rate_protocol
 *   802.11 protocol as identified by @ref sl_wifi_rate_protocol_t
 * @param[in] mask
 *   Data rate as identified by @ref sl_wifi_rate_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_wifi_set_transmit_rate(sl_wifi_interface_t interface,
                                      sl_wifi_rate_protocol_t rate_protocol,
                                      sl_wifi_rate_t mask);

/***************************************************************************/ /**
 * @brief
 *   Get the Wi-Fi transmit rate for the given 802.11 protocol on the specified Wi-Fi interface.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[out] rate_protocol
 *   802.11 protocol as identified by @ref sl_wifi_rate_protocol_t
 * @param[out] mask
 *   Data rate as identified by @ref sl_wifi_rate_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_wifi_get_transmit_rate(sl_wifi_interface_t interface,
                                      sl_wifi_rate_protocol_t *rate_protocol,
                                      sl_wifi_rate_t *mask);

/***************************************************************************/ /**
 * @brief
 *   Set the Wi-Fi client interface listen interval.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] listen_interval
 *   @ref sl_wifi_listen_interval_t object
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 *   This API is not yet implemented.
 ******************************************************************************/
sl_status_t sl_wifi_set_listen_interval(sl_wifi_interface_t interface, sl_wifi_listen_interval_t listen_interval);

/***************************************************************************/ /**
 * @brief
 *   Get the Wi-Fi client listen interval.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[out] listen_interval
 *   @ref sl_wifi_listen_interval_t object that will contain the current listen interval.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 *   This API is not yet implemented.
 ******************************************************************************/
sl_status_t sl_wifi_get_listen_interval(sl_wifi_interface_t interface, sl_wifi_listen_interval_t *listen_interval);

/***************************************************************************/ /**
 * @brief
 *   Assign the user configurable channel gain values in different regions to the module from user. 
 * @pre 
 *   This method is used for overwriting default gain tables that are present in firmware.
 * @pre 
 *   Customer can load all the three gain tables (i.e., 2.4GHz-20Mhz, 5GHz-20Mhz, 5GHz-40Mhz) one the after other by changing band and bandwidth values.
 * @pre
 * This is a blocking API.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in]  band 
 *    1 - 2.4GHz,
 *		2 - 5GHz
 * @param[in]  bandwidth   
 *    0 - 20 MHz,
 *    1 - 40 MHz
 * @param[in]  payload
 *    Pass channel gain values for different regions in a given array format.
 * @param[in]  payload_len
 *    Max payload length (table size) in 2.4GHz is 128 bytes and in 5GHz is 64 bytes.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note 
 *   1. This frame has to be used only by customers who has done FCC/ETSI/TELEC/KCC certification with their own antenna. Silicon Labs is not liable for inappropriate usage of this frame that may result in violation of FCC/ETSI/TELEC/KCC or any certifications. 
 *   2. Internally, firmware maintains two tables : Worldwide table & Region based table. Worldwide table is populated by firmware with max power values that chip can transmit and meet target specs like EVM. Region based table has default gain value set.
 *   3. When certifying with user antenna, Region has to be set to Worldwide and sweep the power from 0 to 21dBm. Arrive at max power level that is passing certification especially band-edge.
 *   4. These FCC/ETSI/TELEC/KCC max power level should be loaded in end-to-end mode via WLAN User Gain table. This has to be called done for every boot-up as this information is not saved inside the flash. Region based user gain table sent by application is copied onto Region based table. SoC uses this table in FCC/ETSI/TELEC/KCC to limit the power and to not violate the allowed limits.
 *   5. For Worldwide region, the firmware uses Worldwide table for Tx. For other regions(FCC/ETSI/TELEC/KCC), the firmware uses min value out of Worldwide & Region based table for Tx.  Also, there will be part to part variation across the chips. Offsets that are estimated during the flow of manufacture will be applied as correction factor during normal mode of operation.
 *   6. In 2.4Ghz band, 40Mhz is not supported.
 ******************************************************************************/
sl_status_t sl_wifi_update_gain_table(uint8_t band, uint8_t bandwidth, uint8_t *payload, uint16_t payload_len);

/** @} */

/** \addtogroup WIFI_SCANNING_API Scanning
  * \ingroup SL_WIFI_FUNCTIONS
  * @{ */

// Scanning functions
/***************************************************************************/ /**
 * @brief
 *   Start scanning for Wi-Fi networks.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] optional_ssid
 *   Optional SSID of type @ref sl_wifi_ssid_t can be used to scan for a particular Wi-Fi network
 * @param[in] configuration
 *   @ref sl_wifi_scan_configuration_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 * 	For 911x, Advanced scan results are not populated to user.
 ******************************************************************************/
sl_status_t sl_wifi_start_scan(sl_wifi_interface_t interface,
                               const sl_wifi_ssid_t *optional_ssid,
                               const sl_wifi_scan_configuration_t *configuration);

/***************************************************************************/ /**
 * @brief
 *   Stop Wi-Fi scan.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 * 	For 911x, sl_wifi_stop_scan is ONLY supported for advanced scan
 ******************************************************************************/
sl_status_t sl_wifi_stop_scan(sl_wifi_interface_t interface);

/***************************************************************************/ /**
 * @brief
 *   Set advanced scan configuration.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] configuration
 *   Set advanced scan configuration as identified by @ref sl_wifi_advanced_scan_configuration_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_wifi_set_advanced_scan_configuration(const sl_wifi_advanced_scan_configuration_t *configuration);

/***************************************************************************/ /**
 * @brief
 *   Get advanced scan configuration.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[out] configuration
 *   @ref sl_wifi_advanced_scan_configuration_t object that will contain the current advanced scan configuration.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_wifi_get_advanced_scan_configuration(sl_wifi_advanced_scan_configuration_t *configuration);

/***************************************************************************/ /**
 * @brief
 *   Wait for current scan to complete and stores the results in the provided array.
 * @pre
 *   This function also returns when the scan result array is full.
 * @pre
 *   Once the scan result array is full, any further scan results will be lost.
 * @param[in] scan_result_array
 *   Array of @ref sl_wifi_scan_result_t objects to store the scan results.
 * @param[in] max_scan_result_count
 *   The maximum number of scan result objects that can fit in the scan result array.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note 
 *   This API is not supported in the current release.
 ******************************************************************************/
sl_status_t sl_wifi_wait_for_scan_results(sl_wifi_scan_result_t **scan_result_array, uint32_t max_scan_result_count);

/** @} */

/** \addtogroup WIFI_CLIENT_API Client
  * \ingroup SL_WIFI_FUNCTIONS
  * @{ */
// Wi-Fi Client functions

/***************************************************************************/ /**
 * @brief
 *   Connect to the given Wi-Fi AP.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi client interface as identified by @ref sl_wifi_interface_t
 * @param[in] access_point
 *   @ref sl_wifi_client_configuration_t object that contains the details of Access Point.
 * @param[in] timeout_ms
 *   Timeout value in milliseconds. The function will abort and return when the timeout timer expires.
 *   A value of 0 indicates an asynchronous action. TBD: @ref asynchronous action page.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 *   If channel, band, and BSSID are provided, this API will attempt to connect without scanning.
 ******************************************************************************/
sl_status_t sl_wifi_connect(sl_wifi_interface_t interface,
                            const sl_wifi_client_configuration_t *access_point,
                            uint32_t timeout_ms);

/***************************************************************************/ /**
 * @brief
 *   Disconnect the Wi-Fi client interface.
 * @pre 
 *   @ref sl_wifi_connect should be called before this API.
 * @param[in] interface
 *   Wi-Fi client interface as identified by @ref sl_wifi_interface_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_wifi_disconnect(sl_wifi_interface_t interface);

/***************************************************************************/ /**
 * @brief
 *   Get current Wi-Fi client's signal strength (RSSI).
 * @pre 
 *   @ref sl_wifi_connect should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] rssi
 *   signal strength (RSSI) in dBm.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_wifi_get_signal_strength(sl_wifi_interface_t interface, int32_t *rssi);

/***************************************************************************/ /**
 * @brief
 *   Set the Wi-Fi roaming configuration.
 * @pre
 *   @ref sl_wifi_set_advanced_scan_configuration should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] roam_configuration
 *   @ref sl_wifi_roam_configuration_t object to store Wi-Fi roaming configuration.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 *	 For si91x chips, following ranges are valid:
 *	 trigger_level: [-10, -100] , 
 *   trigger_level_change: [0, 90]
 ******************************************************************************/
sl_status_t sl_wifi_set_roam_configuration(sl_wifi_interface_t interface,
                                           sl_wifi_roam_configuration_t *roam_configuration);

/***************************************************************************/ /**
 * @brief
 *   Get the Wi-Fi roaming configuration.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[out] roam_configuration
 *   @sl_wifi_roam_configuration_t object that will contain the current roam configuration.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note 
 *   This API is not yet implemented.
 ******************************************************************************/
sl_status_t sl_wifi_get_roam_configuration(sl_wifi_interface_t interface,
                                           sl_wifi_roam_configuration_t *roam_configuration);

/***************************************************************************/ /**
 * @brief
 *   Verify the Wi-Fi client configuration is valid and available.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] ap
 *   @ref sl_wifi_client_configuration_t object that contains the details of Access Point.
 * @param[in] timeout_ms
 *   Timeout value in milliseconds. The function will abort and return when the timeout timer expires.
 *   A value of 0 indicates an asynchronous action.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_wifi_test_client_configuration(sl_wifi_interface_t interface,
                                              const sl_wifi_client_configuration_t *ap,
                                              uint32_t timeout_ms);

/***************************************************************************/ /**
 * @brief
 *   Load the certificate into the device.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] certificate_type
 *   Certificate type being set
 * @param[in] buffer
 *   Pointer to buffer containing the certificate.
 * @param[in] certificate_length
 *   Length of certificate buffer data.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_wifi_set_certificate(uint8_t certificate_type, const uint8_t *buffer, uint32_t certificate_length);

/***************************************************************************/ /**
 * @brief
 *   Load the certificate into the device.
 * @pre 
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
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_wifi_set_certificate_with_index(uint8_t certificate_type,
                                               uint8_t certificate_index,
                                               uint8_t *buffer,
                                               uint32_t certificate_length);

/***************************************************************************/ /**
*   Set the advanced configuration options of a client interface.
* @param[in] interface
*   Wi-Fi interface as identified by @ref sl_wifi_interface_t
* @param[in] configuration
*   Wi-Fi client advanced configuration. See @ref sl_wifi_advanced_client_configuration_t
* @return
*   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
******************************************************************************/
sl_status_t sl_wifi_set_advanced_client_configuration(sl_wifi_interface_t interface,
                                                      const sl_wifi_advanced_client_configuration_t *configuration);

/** @} */

/** \addtogroup WIFI_AP_API Access Point
  * \ingroup SL_WIFI_FUNCTIONS
  * @{ */
// Access point functions

/***************************************************************************/ /**
 * @brief
 *   Start a Wi-Fi access point (AP) interface.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] configuration
 *   Wi-Fi AP configuration. See @ref sl_wifi_ap_configuration_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_wifi_start_ap(sl_wifi_interface_t interface, const sl_wifi_ap_configuration_t *configuration);

/***************************************************************************/ /**
 * @brief
 *   Set the configuration of a running Wi-Fi access point (AP).
 *   If the new configuration modifies vital settings such as SSID or security, the AP will be stopped and restarted automatically.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] configuration
 *   Wi-Fi AP configuration. See @ref sl_wifi_ap_configuration_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 *   This API is not yet implemented.
 ******************************************************************************/
sl_status_t sl_wifi_set_ap_configuration(sl_wifi_interface_t interface,
                                         const sl_wifi_ap_configuration_t *configuration);

/***************************************************************************/ /**
 * @brief
 *   Get the configuration of a Wi-Fi AP interface.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[out] configuration
 *   @ref sl_wifi_ap_configuration_t object that contains the AP configuration.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
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
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
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
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 *   This API is not yet implemented.
 ******************************************************************************/
sl_status_t sl_wifi_get_advanced_ap_configuration(sl_wifi_interface_t interface,
                                                  const sl_wifi_advanced_ap_configuration_t *configuration);

/***************************************************************************/ /**
 * @brief
 *   Stop Wi-Fi access point.
 * @pre 
 *   @ref sl_wifi_start_ap should be called before this API.
 * @param[in] interface
 *   Wi-Fi Access Point interface as identified by @ref sl_wifi_interface_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 *   This API is not yet implemented.
 ******************************************************************************/
sl_status_t sl_wifi_stop_ap(sl_wifi_interface_t interface);

/***************************************************************************/ /**
 * @brief
 *   De-authenticate Wi-Fi client with the given MAC address.
 * @pre 
 *   @ref sl_wifi_start_ap should be called before this API.
 * @param[in] interface
 *   Wi-Fi Access Point interface as identified by @ref sl_wifi_interface_t
 * @param[in] mac
 *   Wi-Fi client's MAC address of type @ref sl_mac_address_t
 * @param[in] reason
 *   Reason for de-authentication as specified in @ref sl_wifi_deauth_reason_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note 
 *   Client interfaces are not supported.
 ******************************************************************************/
sl_status_t sl_wifi_disconnect_ap_client(sl_wifi_interface_t interface,
                                         const sl_mac_address_t *mac,
                                         sl_wifi_deauth_reason_t reason);

/***************************************************************************/ /**
 * @brief
 *   Return the Wi-Fi client information of all clients connected to the AP.
 * @pre 
 *   @ref sl_wifi_start_ap should be called before this API.
 * @param[in] interface
 *   Wi-Fi Access Point interface as identified by @ref sl_wifi_interface_t
 * @param[out] client_info
 *   @ref sl_wifi_client_info_response object to store the client info.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note 
 *   Client interfaces are not supported.
 ******************************************************************************/
sl_status_t sl_wifi_get_ap_client_info(sl_wifi_interface_t interface, sl_wifi_client_info_response *client_info);

/***************************************************************************/ /**
 * @brief
 *   Return a list of Wi-Fi clients connected to the Wi-Fi access point.
 * @pre 
 *   @ref sl_wifi_start_ap should be called before this API.
 * @param[in] interface
 *   Wi-Fi Access Point interface as identified by @ref sl_wifi_interface_t
 * @param[in] client_list_count
 *   The number of sl_mac_address_t objects the client_list can store.
 * @param[out] client_list
 *   A pointer to an array of client_list_count number of @ref sl_mac_address_t objects where the client list will be copied to.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note 
 *   Client interfaces are not supported.
 ******************************************************************************/
sl_status_t sl_wifi_get_ap_client_list(sl_wifi_interface_t interface,
                                       uint16_t client_list_count,
                                       sl_mac_address_t *client_list);

/***************************************************************************/ /**
 * @brief
 *   Provide the number of Wi-Fi clients connected to the Wi-Fi access point
 * @pre 
 *   @ref sl_wifi_start_ap should be called before this API.
 * @param[in] interface
 *   Wi-Fi Access Point interface as identified by @ref sl_wifi_interface_t
 * @param[out] client_count
 *   A uint32_t pointer that will store the number of associated clients.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note 
 *   Client interfaces are not supported.
 ******************************************************************************/
sl_status_t sl_wifi_get_ap_client_count(sl_wifi_interface_t interface, uint32_t *client_count);

/** @} */

/** \addtogroup WIFI_POWER_API Performance Management
  * \ingroup SL_WIFI_FUNCTIONS
  * @{ */
// Power management functions

/***************************************************************************/ /**
 * @brief
 *   Set Wi-Fi performance profile.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] profile
 *   Wi-Fi performance profile as indicated by @ref sl_wifi_performance_profile_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 *   For SI91x chips Enhanced MAX PSP is supported when profile is set to ASSOCIATED_POWER_SAVE_LOW_LATENCY and SL_SI91X_ENABLE_ENHANCED_MAX_PSP bit is enabled in config feature bitmap
 ******************************************************************************/
sl_status_t sl_wifi_set_performance_profile(const sl_wifi_performance_profile_t *profile);

/***************************************************************************/ /**
 * @brief
 *   Get Wi-Fi performance profile.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[out] profile
 *   Wi-Fi performance profile as indicated by @ref sl_wifi_performance_profile_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_wifi_get_performance_profile(sl_wifi_performance_profile_t *profile);

/** @} */

/** \addtogroup WIFI_MONITOR_API Monitor Mode
  * \ingroup SL_WIFI_FUNCTIONS
  * @{ */
// "Monitor Mode" functions

/***************************************************************************/ /**
 * @brief
 *   Enable monitor (promiscuous) mode on the Wi-Fi device.
 *   In this mode, all types of Wi-Fi frames will be forwarded to the host.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 *   This API is not yet implemented.
 ******************************************************************************/
sl_status_t sl_wifi_enable_monitor_mode(sl_wifi_interface_t interface);

/***************************************************************************/ /**
 * @brief
 *   Disable monitor mode on the Wi-Fi interface
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 *   This API is not yet implemented.
 ******************************************************************************/
sl_status_t sl_wifi_disable_monitor_mode(sl_wifi_interface_t interface);

/** @} */

/** \addtogroup WIFI_P2P_API Wi-Fi Direct
  * \ingroup SL_WIFI_FUNCTIONS
  * @{ */
// P2P functions

/***************************************************************************/ /**
 * @brief
 *   Start Wi-Fi direct discovery.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] configuration
 *   P2P configuration as identified by @ref sl_wifi_p2p_configuration_t
 * @param[in] credential_id
 *   Credential ID
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
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
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 *   This API is not yet implemented.
 ******************************************************************************/
sl_status_t sl_wifi_p2p_connect(sl_wifi_interface_t interface, const sl_wifi_p2p_configuration_t *configuration);

/** @} */

/** \addtogroup WIFI_WPS_API Wi-Fi Protected Setup
  * \ingroup SL_WIFI_FUNCTIONS
  * @{ */
// WPS functions

/***************************************************************************/ /**
 * @brief
 *   Generate Wi-Fi Protected Setup (WPS) pin.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[out] response
 *   @ref sl_wifi_wps_pin_t object that will contain the WPS pin.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_wifi_generate_wps_pin(sl_wifi_wps_pin_t *response);

/***************************************************************************/ /**
 * @brief
 *   Start Wi-Fi Protected Setup (WPS).
 * @pre 
 *   @ref sl_wifi_start_ap should be called before this API.
 * @param[in] interface
 *   Wi-Fi Access Point interface as identified by @ref sl_wifi_interface_t
 * @param[in] mode
 *   WPS mode as identified by @ref sl_wifi_wps_mode_t
 * @param[in] optional_wps_pin
 *   WPS pin object @ref sl_wifi_wps_pin_t when @ref SL_WIFI_WPS_PIN_MODE is used.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note 
 *   Client interfaces are not supported.
 ******************************************************************************/
sl_status_t sl_wifi_start_wps(sl_wifi_interface_t interface,
                              sl_wifi_wps_mode_t mode,
                              const sl_wifi_wps_pin_t *optional_wps_pin);

/***************************************************************************/ /**
 * @brief
 *   Stop current running Wi-Fi Protected Setup (WPS).
 * @pre 
 *   @ref sl_wifi_start_wps should be called before this API.
 * @param[in] interface
 *   Wi-Fi Access Point interface as identified by @ref sl_wifi_interface_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note 
 *   Client interfaces are not supported.
 ******************************************************************************/
sl_status_t sl_wifi_stop_wps(sl_wifi_interface_t interface);

/** @} */

/** \addtogroup WIFI_DEBUG_API Debugging
  * \ingroup SL_WIFI_FUNCTIONS
  * @{ */

// Debug functions

/***************************************************************************/ /**
 * @brief
 *   Start collecting statistical data.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] channel
 *   Provides the statistics report on the specified channel.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_wifi_start_statistic_report(sl_wifi_interface_t interface, sl_wifi_channel_t channel);

/***************************************************************************/ /**
 * @brief
 *   Stop collecting statistical data.
 * @pre 
 *   @ref sl_wifi_start_statistic_report should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_wifi_stop_statistic_report(sl_wifi_interface_t interface);

/***************************************************************************/ /**
 * @brief
 *   Send raw data frame.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi interface as identified by @ref sl_wifi_interface_t
 * @param[in] data
 *   Data buffer.
 * @param[in] data_length
 *   length of the data.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_wifi_send_raw_data_frame(sl_wifi_interface_t interface, const void *data, uint16_t data_length);

/***************************************************************************/ /**
 * @brief
 *   Configure TWT parameters. Enables or disables a TWT session. This is blocking API.
 * @pre 
 *   @ref sl_wifi_connect should be called before this API.
 * @param[in] twt_req
 *   Configurable TWT parameters.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_wifi_enable_target_wake_time(sl_wifi_twt_request_t *twt_req);

/***************************************************************************/ /**
 * @brief
 *   Configure TWT parameters. Enables or disables a TWT session. This is blocking API.
 * @pre 
 *   @ref sl_wifi_enable_target_wake_time should be called before this API.
 * @param[in] twt_req
 *   Configurable TWT parameters.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_wifi_disable_target_wake_time(sl_wifi_twt_request_t *twt_req);

/***************************************************************************/ /**
 * @brief
 *   Send Filter Broadcast Request frame.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] beacon_drop_threshold
 *   The amount of time that FW waits to receive full beacon. Default value is 5000ms.
 * @param[in] filter_bcast_in_tim
 *   If this bit is set, then from the next dtim any broadcast data pending bit in TIM indicated will be ignored valid values: 0 - 1.
 * @param[in] filter_bcast_tim_till_next_cmd
 *   0 - filter_bcast_in_tim is valid till disconnect of the STA. 
 *   1 - filter_bcast_in_tim is valid till next update by giving the same command.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_wifi_filter_broadcast(uint16_t beacon_drop_threshold,
                                     uint8_t filter_bcast_in_tim,
                                     uint8_t filter_bcast_tim_till_next_cmd);

/***************************************************************************/ /**
 * @brief
 *   Configure the 11ax params.This is a blocking API.
 * @pre
 *   This API should be called before @ref sl_wifi_connect
 * @param[in] guard_interval
 *   Period of time delta between two packets in wireless transmission. Valid values : 0 - 3 (0 = 8us, 1 = 16us, 2 = 32us, 3 = 64us).
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_wifi_set_11ax_config(uint8_t guard_interval);

/***************************************************************************/ /**
 * @brief
 *   Generate PMK if PSK and SSID are provided. This is a blocking API.
 * @pre
 *   @ref This API should be called after @ref sl_wifi_init and called before @ref sl_wifi_connect.
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
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_wifi_get_pairwise_master_key(sl_wifi_interface_t interface,
                                            const uint8_t type,
                                            const sl_wifi_ssid_t *ssid,
                                            const char *pre_shared_key,
                                            uint8_t *pairwise_master_key);

/** @} */

/***************************************************************************/ /**
 * @brief
 *   Return the status of the Wi-Fi device
 * @param[out] wifi_status
 *   @ref sl_wifi_status_t object that will contain the Wi-Fi status.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 *   This API is not yet implemented.
 ******************************************************************************/
sl_status_t sl_wifi_get_status(sl_wifi_status_t *wifi_status);
