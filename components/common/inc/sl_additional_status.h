/*******************************************************************************
* @file  sl_additional_status.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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
#pragma once

#define SL_STATUS_SI91X_SUBSPACE ((sl_status_t)0x00010000)

/**
 * @addtogroup SL_ADDITIONAL_STATUS_ERRORS
 * @{ 
 */

// Additional generic errors
#define SL_STATUS_OS_OPERATION_FAILURE            ((sl_status_t)0x0051) ///< OS operation failed.
#define SL_STATUS_BOOTUP_OPTIONS_NOT_SAVED        ((sl_status_t)0x0052) ///< Bootup options not saved.
#define SL_STATUS_BOOTUP_OPTIONS_CHECKSUM_FAILURE ((sl_status_t)0x0053) ///< Bootup options checksum failed.
#define SL_STATUS_BOOTLOADER_VERSION_MISMATCH     ((sl_status_t)0x0054) ///< Bootloader version mismatch.
#define SL_STATUS_WAITING_FOR_BOARD_READY         ((sl_status_t)0x0055) ///< Waiting for board ready.
#define SL_STATUS_VALID_FIRMWARE_NOT_PRESENT      ((sl_status_t)0x0056) ///< Invalid firmware.
#define SL_STATUS_INVALID_OPTION                  ((sl_status_t)0x0057) ///< Invalid option.
#define SL_STATUS_SPI_BUSY                        ((sl_status_t)0x0058) ///< SPI busy.
#define SL_STATUS_CARD_READY_TIMEOUT              ((sl_status_t)0x0059) ///< Card ready not received.
#define SL_STATUS_FW_LOAD_OR_UPGRADE_TIMEOUT      ((sl_status_t)0x005A) ///< Firmware upgrade timed out.

// Additional Wi-Fi errors
#define SL_STATUS_WIFI_DOES_NOT_EXIST               ((sl_status_t)0x0B21) ///< Does not exist.
#define SL_STATUS_WIFI_NOT_AUTHENTICATED            ((sl_status_t)0x0B22) ///< Not authenticated.
#define SL_STATUS_WIFI_NOT_KEYED                    ((sl_status_t)0x0B23) ///< Not keyed.
#define SL_STATUS_WIFI_IOCTL_FAIL                   ((sl_status_t)0x0B24) ///< IOCTL fail.
#define SL_STATUS_WIFI_BUFFER_UNAVAILABLE_TEMPORARY ((sl_status_t)0x0B25) ///< Buffer unavailable temporarily.
#define SL_STATUS_WIFI_BUFFER_UNAVAILABLE_PERMANENT ((sl_status_t)0x0B26) ///< Buffer unavailable permanently.
#define SL_STATUS_WIFI_WPS_PBC_OVERLAP              ((sl_status_t)0x0B27) ///< WPS PBC overlap.
#define SL_STATUS_WIFI_CONNECTION_LOST              ((sl_status_t)0x0B28) ///< Connection lost.
#define SL_STATUS_WIFI_OUT_OF_EVENT_HANDLER_SPACE   ((sl_status_t)0x0B29) ///< Cannot add extra event handler.
#define SL_STATUS_WIFI_SEMAPHORE_ERROR              ((sl_status_t)0x0B2A) ///< Error manipulating a semaphore.
#define SL_STATUS_WIFI_FLOW_CONTROLLED              ((sl_status_t)0x0B2B) ///< Packet retrieval cancelled due to flow control.
#define SL_STATUS_WIFI_NO_CREDITS                   ((sl_status_t)0x0B2C) ///< Packet retrieval cancelled due to lack of bus credits.
#define SL_STATUS_WIFI_NO_PACKET_TO_SEND \
  ((sl_status_t)0x0B2D) ///< Packet retrieval cancelled due to no pending packets.
#define SL_STATUS_WIFI_CORE_CLOCK_NOT_ENABLED   ((sl_status_t)0x0B2E) ///< Core disabled due to no clock.
#define SL_STATUS_WIFI_CORE_IN_RESET            ((sl_status_t)0x0B2F) ///< Core disabled - in reset.
#define SL_STATUS_WIFI_UNSUPPORTED              ((sl_status_t)0x0B30) ///< Unsupported function.
#define SL_STATUS_WIFI_BUS_WRITE_REGISTER_ERROR ((sl_status_t)0x0B31) ///< Error writing to WLAN register.
#define SL_STATUS_WIFI_SDIO_BUS_UP_FAIL         ((sl_status_t)0x0B32) ///< SDIO bus failed to come up.
#define SL_STATUS_WIFI_JOIN_IN_PROGRESS         ((sl_status_t)0x0B33) ///< Join not finished yet.
#define SL_STATUS_WIFI_NETWORK_NOT_FOUND        ((sl_status_t)0x0B34) ///< Specified network was not found.
#define SL_STATUS_WIFI_INVALID_JOIN_STATUS      ((sl_status_t)0x0B35) ///< Join status error.
#define SL_STATUS_WIFI_UNKNOWN_INTERFACE        ((sl_status_t)0x0B36) ///< Unknown interface specified.
#define SL_STATUS_WIFI_SDIO_RX_FAIL             ((sl_status_t)0x0B37) ///< Error during SDIO receive.
#define SL_STATUS_WIFI_HWTAG_MISMATCH           ((sl_status_t)0x0B38) ///< Hardware tag header corrupt.
#define SL_STATUS_WIFI_RX_BUFFER_ALLOC_FAIL     ((sl_status_t)0x0B39) ///< Failed to allocate a buffer to receive into.
#define SL_STATUS_WIFI_BUS_READ_REGISTER_ERROR  ((sl_status_t)0x0B3A) ///< Error reading a bus hardware register.
#define SL_STATUS_WIFI_THREAD_CREATE_FAILED     ((sl_status_t)0x0B3B) ///< Failed to create a new thread.
#define SL_STATUS_WIFI_QUEUE_ERROR              ((sl_status_t)0x0B3C) ///< Error manipulating a queue.
#define SL_STATUS_WIFI_BUFFER_POINTER_MOVE_ERROR \
  ((sl_status_t)0x0B3D)                                            ///< Error moving the current pointer of a buffer.
#define SL_STATUS_WIFI_BUFFER_SIZE_SET_ERROR ((sl_status_t)0x0B3E) ///< Error setting size of packet buffer.
#define SL_STATUS_WIFI_THREAD_STACK_NULL \
  ((sl_status_t)0x0B3F) ///< Null stack pointer passed when non null was required.
#define SL_STATUS_WIFI_THREAD_DELETE_FAIL    ((sl_status_t)0x0B40) ///< Error deleting a thread.
#define SL_STATUS_WIFI_SLEEP_ERROR           ((sl_status_t)0x0B41) ///< Failed to put a thread to sleep.
#define SL_STATUS_WIFI_BUFFER_ALLOC_FAIL     ((sl_status_t)0x0B42) ///< Failed to allocate a packet buffer.
#define SL_STATUS_WIFI_INTERFACE_NOT_UP      ((sl_status_t)0x0B44) ///< Requested interface is not active.
#define SL_STATUS_WIFI_DELAY_TOO_LONG        ((sl_status_t)0x0B45) ///< Requested delay is too long.
#define SL_STATUS_WIFI_INVALID_DUTY_CYCLE    ((sl_status_t)0x0B46) ///< Duty cycle is outside limit 0 to 0.
#define SL_STATUS_WIFI_PMK_WRONG_LENGTH      ((sl_status_t)0x0B47) ///< Returned pmk was the wrong length.
#define SL_STATUS_WIFI_UNKNOWN_SECURITY_TYPE ((sl_status_t)0x0B48) ///< AP security type was unknown.
#define SL_STATUS_WIFI_WEP_NOT_ALLOWED       ((sl_status_t)0x0B49) ///< AP not allowed to use WEP - use Open instead.
#define SL_STATUS_WIFI_WPA_KEYLEN_BAD        ((sl_status_t)0x0B4A) ///< WPA / WPA2 key length must be between 8 & 64 bytes.
#define SL_STATUS_WIFI_FILTER_NOT_FOUND      ((sl_status_t)0x0B4B) ///< Specified filter id not found.
#define SL_STATUS_WIFI_SPI_ID_READ_FAIL      ((sl_status_t)0x0B4C) ///< Failed to read 0xfeedbead SPI id from chip.
#define SL_STATUS_WIFI_SPI_SIZE_MISMATCH     ((sl_status_t)0x0B4D) ///< Mismatch in sizes between SPI and SDPCM header.
#define SL_STATUS_WIFI_ADDRESS_ALREADY_REGISTERED \
  ((sl_status_t)0x0B4E) ///< Attempt to register a multicast address twice.
#define SL_STATUS_WIFI_SDIO_RETRIES_EXCEEDED     ((sl_status_t)0x0B4F) ///< SDIO transfer failed too many times.
#define SL_STATUS_WIFI_NULL_PTR_ARG              ((sl_status_t)0x0B50) ///< Null Pointer argument passed to function.
#define SL_STATUS_WIFI_THREAD_FINISH_FAIL        ((sl_status_t)0x0B51) ///< Error deleting a thread.
#define SL_STATUS_WIFI_WAIT_ABORTED              ((sl_status_t)0x0B52) ///< Semaphore/Mutex wait has been aborted.
#define SL_STATUS_WIFI_QUEUE_MESSAGE_UNALIGNED   ((sl_status_t)0x0B53) ///< Unaligned message in the queue.
#define SL_STATUS_WIFI_MUTEX_ERROR               ((sl_status_t)0x0B54) ///< Error while Mutex operation.
#define SL_STATUS_WIFI_SECURE_LINK_DECRYPT_ERROR ((sl_status_t)0x0B57) ///< Error while decryption over secure link.
#define SL_STATUS_WIFI_SECURE_LINK_KEY_RENEGOTIATION_ERROR \
  ((sl_status_t)0x0B59) ///< Error while renegotiation of key over secure link.
#define SL_STATUS_WIFI_INVALID_OPERMODE          ((sl_status_t)0x0B60) ///< Invalid opermode provided.
#define SL_STATUS_WIFI_INVALID_ENCRYPTION_METHOD ((sl_status_t)0x0B61) ///< Invalid security encryption method provided.

// Si91X Crypto errors
#define SL_STATUS_TRNG_DUPLICATE_ENTROPY ((sl_status_t)0x0B62) ///< TRNG duplicate elements error.

// Si91X Wi-Fi transceiver error codes
#define SL_STATUS_TRANSCEIVER_INVALID_MAC_ADDRESS  ((sl_status_t)0x0B63) ///< Invalid MAC address provided
#define SL_STATUS_TRANSCEIVER_INVALID_QOS_PRIORITY ((sl_status_t)0x0B64) ///< Invalid QOS priority provided
#define SL_STATUS_TRANSCEIVER_INVALID_CHANNEL      ((sl_status_t)0x0B65) ///< Invalid channel provided
#define SL_STATUS_TRANSCEIVER_INVALID_DATA_RATE    ((sl_status_t)0x0B66) ///< Invalid data rate provided
#define SL_STATUS_TRANSCEIVER_INVALID_CONFIG \
  ((sl_status_t)0x0B67) ///< Invalid transceiver configuration parameters provided

// Si91X Crypto Firmware Errors
#define SL_STATUS_CRYPTO_INVALID_PARAMETER \
  ((sl_status_t)0x1CCFE) ///< Return when parameter passed to Crypto SAPI is invalid.
#define SL_STATUS_CRYPTO_INVALID_SIGNATURE \
  ((sl_status_t)0x1CC9A) ///< Return in AEAD (CCM, GCM, Chachapoly) decryption function, when MAC generated during decryption does not match the MAC passed.

// Si91X Wi-Fi Firmware errors
#define SL_STATUS_SI91X_SCAN_ISSUED_IN_ASSOCIATED_STATE \
  ((sl_status_t)0x10002) ///< Scan command issued while device is already associated with an access point.
#define SL_STATUS_SI91X_NO_AP_FOUND ((sl_status_t)0x10003) ///< No AP found.
#define SL_STATUS_SI91X_INVALID_PSK_IN_WEP_SECURITY \
  ((sl_status_t)0x10004) ///< Wrong PSK is issued while the device client tries to join an access point with WEP security enabled.
#define SL_STATUS_SI91X_INVALID_BAND ((sl_status_t)0x10005) ///< Invalid band.
#define SL_STATUS_SI91X_UNASSOCIATED ((sl_status_t)0x10006) ///< Association not done or in unassociated state.
#define SL_STATUS_SI91X_DEAUTHENTICATION_RECEIVED_FROM_AP \
  ((sl_status_t)0x10008) ///< De-authentication received from AP.
#define SL_STATUS_SI91X_ASSOCIATION_FAILED \
  ((sl_status_t)0x10009)                                       ///< Failed to associate to access point during "Join".
#define SL_STATUS_SI91X_INVALID_CHANNEL ((sl_status_t)0x1000A) ///< Invalid channel.
#define SL_STATUS_SI91X_JOIN_AUTHENTICATION_FAILED \
  ((sl_status_t)0x1000E) ///< Authentication failure during "Join". Unable to find AP during join which was found during scan.
#define SL_STATUS_SI91X_BEACON_MISSED_FROM_AP_DURING_JOIN ((sl_status_t)0x1000F) ///< Missed beacon from AP during join.
#define SL_STATUS_SI91X_INVALID_MAC_SUPPLIED \
  ((sl_status_t)0x10013) ///< Non-existent MAC address supplied in "Disassociate" command.
#define SL_STATUS_SI91X_EAP_CONFIG_NOT_DONE ((sl_status_t)0x10014) ///< EAP configuration is not done.
#define SL_STATUS_SI91X_MEMORY_FAILED_FROM_MODULE \
  ((sl_status_t)0x10015) ///< Memory allocation failed or Store configuration check sum failed.
#define SL_STATUS_SI91X_INSUFFICIENT_INFO \
  ((sl_status_t)0x10016) ///< Information is wrong or insufficient in join command.
#define SL_STATUS_SI91X_NOT_AP_INTERFACE ((sl_status_t)0x10017) ///< Not an AP interface.
#define SL_STATUS_SI91X_INVALID_PUSH_BUTTON_SEQUENCE \
  ((sl_status_t)0x10018) ///< Push button command given before the expiry of previous push button command.
#define SL_STATUS_SI91X_REJOIN_FAILURE          ((sl_status_t)0x10019) ///< Access point not found. Rejoin failure.
#define SL_STATUS_SI91X_FREQUENCY_NOT_SUPPORTED ((sl_status_t)0x1001A) ///< Frequency not supported.
#define SL_STATUS_SI91X_INVALID_OPERMODE        ((sl_status_t)0x1001B) ///< Invalid opermode.
#define SL_STATUS_SI91X_EAP_CONFIG_FAILED       ((sl_status_t)0x1001C) ///< EAP configuration failed.
#define SL_STATUS_SI91X_P2P_CONFIG_FAILED       ((sl_status_t)0x1001D) ///< P2P configuration failed.
#define SL_STATUS_SI91X_GROUP_OWNER_NEGOTIATION_FAILED \
  ((sl_status_t)0x1001E) ///< Unable to start Group Owner negotiation.
#define SL_STATUS_SI91X_JOIN_TIMEOUT                   ((sl_status_t)0x10020) ///< Join timeout.
#define SL_STATUS_SI91X_COMMAND_GIVEN_IN_INVALID_STATE ((sl_status_t)0x10021) ///< Command given in incorrect state.
#define SL_STATUS_SI91X_INVALID_QUERY_GO_PARAMS \
  ((sl_status_t)0x10022) ///< Query GO parameters issued in incorrect operating mode.
#define SL_STATUS_SI91X_ACCESS_POINT_FAILED ((sl_status_t)0x10023) ///< Unable to form access point.
#define SL_STATUS_SI91X_INVALID_SCAN_INFO \
  ((sl_status_t)0x10024) ///< Wrong scan input parameters supplied to "Scan" command.
#define SL_STATUS_SI91X_COMMAND_ISSUED_IN_REJOIN_STATE \
  ((sl_status_t)0x10025) ///< Command issued during re-join in progress.
#define SL_STATUS_SI91X_WRONG_PARAMETERS                  ((sl_status_t)0x10026) ///< Wrong parameters the command request.
#define SL_STATUS_SI91X_PROVISION_DISCOVERY_FAILED_IN_P2P ((sl_status_t)0x10027) ///< Provision discovery failed in P2P.
#define SL_STATUS_SI91X_INVALID_PSK_LENGTH \
  ((sl_status_t)0x10028) ///< PSK length less than 8 bytes or more than 63 bytes.
#define SL_STATUS_SI91X_FAILED_TO_CLEAR_OR_SET_EAP_CERTIFICATE \
  ((sl_status_t)0x10029) ///< Failed to clear or to set the Enterprise Certificate (Set Certificate).
#define SL_STATUS_SI91X_P2P_GO_NEGOTIATED_FAILED ((sl_status_t)0x1002A) ///< P2P Go negotiation failed.
#define SL_STATUS_SI91X_ASSOCIATION_TIMEOUT_IN_P2P_WPS_MODE \
  ((sl_status_t)0x1002B) ///< Association between nodes failed in P2P WPS mode due to timeout.
#define SL_STATUS_SI91X_COMMAND_ISSUED_WHILE_INTERNAL_OPERATION \
  ((sl_status_t)0x1002C) ///< If a command is issued by the Host when the device is internally executing auto-join or auto-create.
#define SL_STATUS_SI91X_INVALID_WEP_KEY_LEN        ((sl_status_t)0x1002D) ///< WEP key is of wrong length.
#define SL_STATUS_SI91X_ICMP_REQUEST_TIMEOUT_ERROR ((sl_status_t)0x1002E) ///< ICMP request timeout error.
#define SL_STATUS_SI91X_ICMP_DATA_SIZE_EXCEED_MAX_LIMIT \
  ((sl_status_t)0x1002F) ///< ICMP data size exceeds maximum limit.
#define SL_STATUS_SI91X_SEND_DATA_PACKET_EXCEED_LIMIT \
  ((sl_status_t)0x10030) ///< Send data packet exceeded the limit or length that is mentioned (or) MQTT publish data and publish data length mismatched (or) MQTT Send data packet exceeded the limit.
#define SL_STATUS_SI91X_ARP_CACHE_NOT_FOUND  ((sl_status_t)0x10031) ///< ARP cache entry not found.
#define SL_STATUS_SI91X_UART_COMMAND_TIMEOUT ((sl_status_t)0x10032) ///< UART command timeout happened.
#define SL_STATUS_SI91X_FIXED_DATA_RATE_NOT_SUPPORTED_BY_AP \
  ((sl_status_t)0x10033) ///< Fixed data rate is not supported by connecting AP.
#define SL_STATUS_SI91X_USERNAME_PASSWORD_CLIENTID_TOPIC_MAX_LEN \
  ((sl_status_t)0x10036) ///< Maximum length exceeded of username/password/client_id/topic in MQTT.
#define SL_STATUS_SI91X_INVALID_WPS_PIN     ((sl_status_t)0x10037) ///< Wrong WPS PIN.
#define SL_STATUS_SI91X_INVALID_WPS_PIN_LEN ((sl_status_t)0x10038) ///< Wrong WPS PIN length.
#define SL_STATUS_SI91X_INVALID_PMK_LEN     ((sl_status_t)0x10039) ///< Wrong PMK length.
#define SL_STATUS_SI91X_SSID_NOT_PRESENT_FOR_PMK_GENERATION \
  ((sl_status_t)0x1003A) ///< SSID not present for PMK generation.
#define SL_STATUS_SI91X_SSID_INCORRECT_PMK_GENERATION \
  ((sl_status_t)0x1003B) ///< SSID incorrect for PMK generation(more than 32 bytes).
#define SL_STATUS_SI91X_BAND_NOT_SUPPORTED ((sl_status_t)0x1003C) ///< Band not supported.
#define SL_STATUS_SI91X_INVALID_USR_STORE_CONFIGURATION_LEN \
  ((sl_status_t)0x1003D) ///< User store configuration invalid length.
#define SL_STATUS_SI91X_INVALID_COMMAND_LEN \
  ((sl_status_t)0x1003E) ///< Error in length of the command (Exceeds number of characters is mentioned in the PRM).
#define SL_STATUS_SI91X_DATA_PACKET_DROPPED ((sl_status_t)0x1003F) ///< Data packet dropped.
#define SL_STATUS_SI91X_WEP_KEY_NOT_GIVEN   ((sl_status_t)0x10040) ///< WEP key not given.
#define SL_STATUS_SI91X_INVALID_STORE_CONFIG_PROFILE \
  ((sl_status_t)0x10041)                                          ///< Error in length of store config profile.
#define SL_STATUS_SI91X_MISSING_PSK_OR_PMK ((sl_status_t)0x10042) ///< PSK or PMK not given.
#define SL_STATUS_SI91X_INVALID_SECURITY_MODE_IN_JOIN_COMMAND \
  ((sl_status_t)0x10043) ///< Security mode given in join command is invalid.
#define SL_STATUS_SI91X_MAX_BEACON_MISCOUNT \
  ((sl_status_t)0x10044) ///< Beacon miscount reaches max beacon miscount (De-authentication due to beacon miss).
#define SL_STATUS_SI91X_DEAUTH_REQUEST_FROM_SUPPLICANT \
  ((sl_status_t)0x10045) ///< De-authentication received from supplicant.
#define SL_STATUS_SI91X_DEAUTH_REQUEST_FROM_FROM_AP \
  ((sl_status_t)0x10046) ///< De-authentication received from AP after channel switching.
#define SL_STATUS_SI91X_MISSED_SYNCHRONIZATION ((sl_status_t)0x10047) ///< Synchronization missed.
#define SL_STATUS_SI91X_AUTHENTICATION_TIMEOUT ((sl_status_t)0x10048) ///< Authentication timeout occurred.
#define SL_STATUS_SI91X_ASSOCIATION_TIMEOUT    ((sl_status_t)0x10049) ///< Association timeout.
#define SL_STATUS_SI91X_BG_SCAN_NOT_ALLOWED    ((sl_status_t)0x1004A) ///< BG scan in given channels is not allowed.
#define SL_STATUS_SI91X_SSID_MISMATCH          ((sl_status_t)0x1004B) ///< Scanned SSID and SSID given in join are not matching.
#define SL_STATUS_SI91X_CLIENT_MAX_SUPPORTED_EXCEEDED \
  ((sl_status_t)0x1004C) ///< Given number of clients exceeded max number of stations supported.
#define SL_STATUS_SI91X_HT_CAPABILITIES_NOT_SUPPORTED \
  ((sl_status_t)0x1004D)                                               ///< Given HT capabilities are not supported.
#define SL_STATUS_SI91X_UART_FLOW_NOT_SUPPORTED ((sl_status_t)0x1004E) ///< UART Flow control not supported.
#define SL_STATUS_SI91X_ZB_BT_BLE_PKT_RECEIVED \
  ((sl_status_t)0x1004F) ///< ZB/BT/BLE packet received and protocol is not enabled.
#define SL_STATUS_SI91X_MGMT_PKT_DROPPED        ((sl_status_t)0x10050) ///< MGMT pkt dropped.
#define SL_STATUS_SI91X_INVALID_RF_CURRENT_MODE ((sl_status_t)0x10051) ///< Invalid RF current mode.
#define SL_STATUS_SI91X_POWER_SAVE_NOT_SUPPORTED \
  ((sl_status_t)0x10052) ///< Power save support is not present for a given interface.
#define SL_STATUS_SI91X_CONCURRENT_AP_IN_CONNECTED_STATE ((sl_status_t)0x10053) ///< Concurrent AP in connected state.
#define SL_STATUS_SI91X_CONNECTED_AP_OR_STATION_CHANNEL_MISMATCH \
  ((sl_status_t)0x10054)                                             ///< Connected AP or Station channel mismatch.
#define SL_STATUS_SI91X_IAP_COPROCESSOR_ERROR ((sl_status_t)0x10055) ///< IAP co processor error.
#define SL_STATUS_SI91X_WPS_NOT_SUPPORTED     ((sl_status_t)0x10056) ///< WPS not supported in current operating mode.
#define SL_STATUS_SI91X_CONCURRENT_AP_CHANNEL_MISMATCH \
  ((sl_status_t)0x10057) ///< Concurrent AP doesn't have same channel as connected station channel.
#define SL_STATUS_SI91X_PBC_SESSION_OVERLAP       ((sl_status_t)0x10058) ///< PBC session overlap error.
#define SL_STATUS_SI91X_BT_FEATURE_BITMAP_INVALID ((sl_status_t)0x10059) ///< BT feature bit map invalid.
#define SL_STATUS_SI91X_FOUR_WAY_HANDSHAKE_FAILED \
  ((sl_status_t)0x1005A) ///< 4/4 confirmation of 4 way handshake failed.
#define SL_STATUS_SI91X_MAC_ADDRESS_NOT_PRESENT_IN_MAC_JOIN \
  ((sl_status_t)0x1005B) ///< MAC address not present in MAC based join.
#define SL_STATUS_SI91X_CONCURRENT_MODE_DOWN \
  ((sl_status_t)0x1005C) ///< Concurrent mode, both AP and client should be up, to enable configuration.
#define SL_STATUS_SI91X_CERTIFICATE_LOAD_NOT_ALLOWED_IN_FLASH \
  ((sl_status_t)0x1005D) ///< Certificate load not allowed in flash.
#define SL_STATUS_SI91X_CERTIFICATE_LOAD_NOT_ALLOWED_IN_RAM \
  ((sl_status_t)0x1005E) ///< Certificate load not allowed in RAM.
#define SL_STATUS_SI91X_WRONG_CERTIFICATE_LOAD_INDEX \
  ((sl_status_t)0x1005F)                                              ///< Certificate load failed due to wrong index.
#define SL_STATUS_SI91X_AP_HT_CAPS_NOT_ENABLED ((sl_status_t)0x10060) ///< AP HT caps not enabled.
#define SL_STATUS_SI91X_ADDRESS_FAMILY_NOT_SUPPORTED \
  ((sl_status_t)0x10061) ///< Address family not supported by protocol.
#define SL_STATUS_SI91X_INVALID_BEACON_INTERVAL_OR_DTM_PERIOD \
  ((sl_status_t)0x10062) ///< Invalid beacon interval or DTIM period provided.
#define SL_STATUS_SI91X_INVALID_CONFIG_RANGE_PROVIDED \
  ((sl_status_t)0x10063)                                               ///< Invalid range of the configuration provided.
#define SL_STATUS_SI91X_INVALID_CONFIG_TYPE     ((sl_status_t)0x10064) ///< RTS THRESHOLD Config type is invalid.
#define SL_STATUS_SI91X_ERROR_WITH_MQTT_COMMAND ((sl_status_t)0x10065) ///< Error with MQTT command.
#define SL_STATUS_SI91X_HIGHER_LISTEN_INTERVAL \
  ((sl_status_t)0x10066) ///< listen interval in power save is greater than that of join.
#define SL_STATUS_SI91X_WLAN_RADIO_DEREGISTERED ((sl_status_t)0x10067) ///< WLAN radio deregistered.
#define SL_STATUS_SI91X_SAE_FAILURE_DUE_TO_MULTIPLE_CONFIRM_FRAMES_FROM_AP \
  ((sl_status_t)0x10069) ///< SAE failure due to multiple confirm frames from AP.
#define SL_STATUS_SI91X_EC_GROUP_STATION_UNSUPPORTED_BY_AP \
  ((sl_status_t)0x1006A) ///< AP does not support the EC-group set by station.
#define SL_STATUS_SI91X_NO_11AX_SUPPORT_IN_AP ((sl_status_t)0x1006C) ///< 11AX is not supported in AP mode.
#define SL_STATUS_SI91X_NON_PREF_CHAN_CONFIG_FAILED \
  ((sl_status_t)0x1006D) ///< Non-preferred channel configuration failed.
#define SL_STATUS_TWT_SUPPORT_NOT_ENABLED_ERR \
  ((sl_status_t)0x10070) ///< Occurs when HE_PARAMS_SUPPORT and SLI_SI91X_ENABLE_TWT_FEATURE macros are not enabled.
#define SL_STATUS_TWT_SETUP_ERR_SESSION_ACTIVE \
  ((sl_status_t)0x10071) ///< Occurs when user tries to give TWT config command when there is an already active TWT session.
#define SL_STATUS_TWT_TEARDOWN_ERR_FLOWID_NOT_MATCHED \
  ((sl_status_t)0x10072) ///< Occurs when TWT teardown command is given with a flow ID that does not match existing session flow ID.
#define SL_STATUS_TWT_TEARDOWN_ERR_NOACTIVE_SESS \
  ((sl_status_t)0x10073) ///< Occurs when teardown command is given while there is no active session.
#define SL_STATUS_TWT_SESSION_NOT_FEASIBLE \
  ((sl_status_t)0x10074) ///< This error code indicates that TWT session is not feasible. It is thrown only when TWT Auto Selection API is used.
#define SL_STATUS_SI91X_RESCHEDULE_TWT_NOT_SUPPORTED \
  ((sl_status_t)0x10075) ///< AP does not support TWT information frame reception.
#define SL_STATUS_SI91X_RESCHEDULE_TWT_ERR_NOACTIVE_SESS \
  ((sl_status_t)0x10076) ///< No active TWT agreement corresponding to given flow id.
#define SL_STATUS_SI91X_TWT_RESCHEDULING_IN_PROGRESS \
  ((sl_status_t)0x10077) ///< Suspend or resume TWT action is in progress.
#define SL_STATUS_SI91X_RESCHEDULE_TWT_PACKET_CREATION_FAILED \
  ((sl_status_t)0x10078) ///< TWT information frame packet creation failed in firmware.
#define SL_STATUS_SI91X_INVALID_STATION_TSF \
  ((sl_status_t)0x10079) ///< Invalid station TSF error returned when station is not connected or atleast one beacon is not received
#define SL_STATUS_SI91X_MQTT_ERROR_UNACCEPTABLE_PROTOCOL \
  ((sl_status_t)0x10081) ///< The Server does not support the level of the MQTT protocol requested by the Client.
#define SL_STATUS_SI91X_MQTT_ERROR_IDENTIFIER_REJECTED \
  ((sl_status_t)0x10082) ///< The Client identifier is correct UTF-8 but not allowed by the Server.
#define SL_STATUS_SI91X_MQTT_ERROR_SERVER_UNAVAILABLE \
  ((sl_status_t)0x10083) ///< The Network Connection has been made but the MQTT service is unavailable.
#define SL_STATUS_SI91X_MQTT_ERROR_BAD_USERNAME_PASSWORD \
  ((sl_status_t)0x10084) ///< The data in the user name or password is malformed.
#define SL_STATUS_SI91X_MQTT_ERROR_NOT_AUTHORIZED ((sl_status_t)0x10085) ///< The Client is not authorized to connect.
#define SL_STATUS_SI91X_SA_QUERY_TIMEOUT          ((sl_status_t)0x10086) ///< Disconnection due to SA Query Timeout.
#define SL_STATUS_SI91X_TRANSCEIVER_PEER_DS_FEAT_DISABLED \
  ((sl_status_t)0x10096) ///< Feature to add peers in MAC layer is disabled
#define SL_STATUS_SI91X_TRANSCEIVER_PEER_ALREADY_EXISTS ((sl_status_t)0x10097) ///< Peer already exists in MAC layer
#define SL_STATUS_SI91X_TRANSCEIVER_MAX_PEER_LIMIT_REACHED \
  ((sl_status_t)0x10098)                                                  ///< Max peer limit reached in MAC layer
#define SL_STATUS_SI91X_TRANSCEIVER_PEER_NOT_FOUND ((sl_status_t)0x10099) ///< Peer not found in MAC layer
#define SL_STATUS_SI91X_DUPLICATE_ENTRY_EXISTS_IN_DNS_SERVER_TABLE \
  ((sl_status_t)0x100AF) ///< Duplicate entry exists in DNS server table.
#define SL_STATUS_SI91X_NO_MEM_AVAILABLE                  ((sl_status_t)0x100B1) ///< Memory error: No memory available.
#define SL_STATUS_SI91X_INVALID_CHARACTERS_IN_JSON_OBJECT ((sl_status_t)0x100B2) ///< Invalid characters in JSON object.
#define SL_STATUS_SI91X_NO_KEY_FOUND                      ((sl_status_t)0x100B3) ///< Update commands: No such key found.
#define SL_STATUS_SI91X_NO_FILE_FOUND                     ((sl_status_t)0x100B4) ///< No such file found: Re-check filename.
#define SL_STATUS_SI91X_NO_WEB_PAGE_EXISTS_WITH_SAME_FILENAME \
  ((sl_status_t)0x100B5) ///< No corresponding web page exists with same filename.
#define SL_STATUS_SI91X_SPACE_UNAVAILABLE_FOR_NEW_FILE ((sl_status_t)0x100B6) ///< Space unavailable for new file.
#define SL_STATUS_SI91X_INVALID_INPUT_DATA \
  ((sl_status_t)0x100C1) ///< Invalid input data, Re-check filename, lengths, etc.
#define SL_STATUS_SI91X_NO_SPACE_AVAILABLE_FOR_NEW_FILE ((sl_status_t)0x100C2) ///< Space unavailable for new file.
#define SL_STATUS_SI91X_EXISTING_FILE_OVERWRITE \
  ((sl_status_t)0x100C3) ///< Existing file overwrite: Exceeds size of previous file. Use erase and try again.
#define SL_STATUS_SI91X_NO_SUCH_FILE_FOUND ((sl_status_t)0x100C4) ///< No such file found. Re-check filename.
#define SL_STATUS_SI91X_MEMORY_ERROR       ((sl_status_t)0x100C5) ///< Memory Error: No memory available.
#define SL_STATUS_SI91X_RECEIVED_MORE_WEB_PAGE_DATA \
  ((sl_status_t)0x100C6) ///< Received more web page data than the total length initially specified.
#define SL_STATUS_SI91X_SET_REGION_ERROR ((sl_status_t)0x100C7) ///< Error in set region command.
#define SL_STATUS_SI91X_INVALID_WEBPAGE_CURRENT_CHUNK_LEN \
  ((sl_status_t)0x100C8) ///< Web page current chunk length is incorrect.
#define SL_STATUS_SI91X_AP_SET_REGION_COMMAND_ERROR ((sl_status_t)0x100CA) ///< Error in AP set region command.
#define SL_STATUS_SI91X_AP_SET_REGION_COMMAND_PARAMETERS_ERROR \
  ((sl_status_t)0x100CB)                                                 ///< Error in AP set region command parameters.
#define SL_STATUS_SI91X_REGION_CODE_NOT_SUPPORTED ((sl_status_t)0x100CC) ///< Region code not supported.
#define SL_STATUS_SI91X_EXTRACTING_COUNTRY_REGION_FROM_BEACON_FAILED \
  ((sl_status_t)0x100CD) ///< Error in extracting country region from beacon.
#define SL_STATUS_SI91X_SELECTED_REGION_NOT_SUPPORTED \
  ((sl_status_t)0x100CE) ///< Device does not have selected region support.
#define SL_STATUS_SI91X_SSL_TLS_CONTEXT_CREATION_FAILED ((sl_status_t)0x100D1) ///< SSL/TLS context create failed.
#define SL_STATUS_SI91X_SSL_TLS_HANDSHAKE_FAIL \
  ((sl_status_t)0x100D2) ///< SSL/TLS handshake failed. Socket will be closed.
#define SL_STATUS_SI91X_SSL_TLS_MAX_SOCKETS_REACHED ((sl_status_t)0x100D3) ///< SSL/TLS max sockets reached.
#define SL_STATUS_SI91X_FTP_CLIENT_NOT_CONNECTED    ((sl_status_t)0x100D3) ///< FTP client is not connected.
#define SL_STATUS_SI91X_CIPHER_SET_FAILED           ((sl_status_t)0x100D4) ///< Cipher set failure.
#define SL_STATUS_SI91X_HTTP_CREDENTIALS_MAX_LEN_EXCEEDED \
  ((sl_status_t)0x100F1)                                             ///< HTTP credentials maximum length exceeded.
#define SL_STATUS_SI91X_FEATURE_NOT_SUPPORTED ((sl_status_t)0x100F7) ///< Feature not supported.
#define SL_STATUS_SI91X_FLASH_WRITE_OR_FLASH_DATA_VERIFICATION_FAILED \
  ((sl_status_t)0x100F8) ///< Unable to write to flash OR flash data verification failed.
#define SL_STATUS_SI91X_CALIBRATION_DATA_VERIFICATION_FAILED \
  ((sl_status_t)0x100F9)                                                ///< Calibration data verification failed.
#define SL_STATUS_SI91X_SNMP_INTERNAL_ERROR      ((sl_status_t)0x10100) ///< SNMP internal error.
#define SL_STATUS_SI91X_SNMP_INVALID_IP_PROTOCOL ((sl_status_t)0x10104) ///< SNMP invalid IP protocol error.
#define SL_STATUS_SI91X_NO_DATA_RECEIVED_OR_RECEIVE_TIMEOUT \
  ((sl_status_t)0x1BB01) ///< No data received or receive timeout.
#define SL_STATUS_SI91X_INSUFFICIENT_DATA_FOR_TIME_CONVERSION \
  ((sl_status_t)0x1BB08) ///< Insufficient data for converting NTP time to mm-dd-yy time format.
#define SL_STATUS_SI91X_INVALID_SNTP_SERVER_ADDRESS ((sl_status_t)0x1BB0A) ///< Invalid SNTP server address.
#define SL_STATUS_SI91X_SNTP_CLIENT_NOT_STARTED     ((sl_status_t)0x1BB0B) ///< SNTP client not started.
#define SL_STATUS_SI91X_SNTP_SERVER_UNAVAILABLE \
  ((sl_status_t)0x1BB10) ///< SNTP server not available. Client will not get any time update service from current server.
#define SL_STATUS_SI91X_SNTP_SERVER_AUTHENTICATION_FAILED ((sl_status_t)0x1BB15) ///< SNTP server authentication failed.
#define SL_STATUS_SI91X_INTERNAL_ERROR                    ((sl_status_t)0x1BB0E) ///< Internal error.
#define SL_STATUS_SI91X_MULTICAST_IP_ADDRESS_ENTRY_NOT_FOUND \
  ((sl_status_t)0x1BB16)                                                  ///< Entry not found for multicast IP address.
#define SL_STATUS_SI91X_MULTICAST_NO_ENTRIES_FOUND ((sl_status_t)0x1BB17) ///< No more entries found for multicast.
#define SL_STATUS_SI91X_IP_ADDRESS_ERROR           ((sl_status_t)0x1BB21) ///< IP address error.
#define SL_STATUS_SI91X_SOCKET_ALREADY_BOUND       ((sl_status_t)0x1BB22) ///< Socket already bound.
#define SL_STATUS_SI91X_PORT_UNAVAILABLE           ((sl_status_t)0x1BB23) ///< Port not available.
#define SL_STATUS_SI91X_SOCKET_NOT_CREATED         ((sl_status_t)0x1BB27) ///< Socket is not created.
#define SL_STATUS_SI91X_ICMP_REQUEST_FAILED        ((sl_status_t)0x1BB29) ///< ICMP request failed.
#define SL_STATUS_SI91X_MAX_LISTEN_SOCKETS_REACHED ((sl_status_t)0x1BB33) ///< Maximum listen sockets reached.
#define SL_STATUS_SI91X_DHCP_DUPLICATE_LISTEN      ((sl_status_t)0x1BB34) ///< DHCP duplicate listen.
#define SL_STATUS_SI91X_PORT_NOT_IN_CLOSE_STATE    ((sl_status_t)0x1BB35) ///< Port not in closed state.
#define SL_STATUS_SI91X_SOCKET_CLOSED              ((sl_status_t)0x1BB36) ///< Socket is closed or in process of closing.
#define SL_STATUS_SI91X_PROCESS_IN_PROGRESS        ((sl_status_t)0x1BB37) ///< Process in progress.
#define SL_STATUS_SI91X_CONNECT_TO_NON_EXISTING_TCP_SERVER_SOCKET \
  ((sl_status_t)0x1BB38) ///< Trying to connect non-existing TCP server socket.
#define SL_STATUS_SI91X_ERROR_IN_LEN_OF_THE_COMMAND \
  ((sl_status_t)0x1BB3E) ///< Error in length of the command ('Exceeds number of characters' is mentioned in the PRM).
#define SL_STATUS_SI91X_WRONG_PACKET_INFO  ((sl_status_t)0x1BB40) ///< Wrong packet info.
#define SL_STATUS_SI91X_SOCKET_STILL_BOUND ((sl_status_t)0x1BB42) ///< Socket is still bound.
#define SL_STATUS_SI91X_NO_FREE_PORT       ((sl_status_t)0x1BB45) ///< No free port.
#define SL_STATUS_SI91X_INVALID_PORT       ((sl_status_t)0x1BB46) ///< Invalid port.
#define SL_STATUS_SI91X_CORRUPTED_RPS_HEADER \
  ((sl_status_t)0x1BB49) ///< Corrupted RPS header encountered during firmware update.
#define SL_STATUS_SI91X_FEATURE_UNSUPPORTED ((sl_status_t)0x1BB4B) ///< Feature not supported.
#define SL_STATUS_SI91X_SOCKET_IN_UNCONNECTED_STATE \
  ((sl_status_t)0x1BB50) ///< Socket is not in connected state. Disconnected from server. In case of FTP, user need to give destroy command after receiving this error.
#define SL_STATUS_SI91X_POP3_SESSION_CREATION_FAILED \
  ((sl_status_t)0x1BB87) ///< POP3 session creation failed / POP3 session got terminated.
#define SL_STATUS_SI91X_DHCPV6_HANDSHAKE_FAIL     ((sl_status_t)0x1BB9C) ///< DHCPv6 handshake failure.
#define SL_STATUS_SI91X_DHCP_INVALID_IP_RESPONSE  ((sl_status_t)0x1BB9D) ///< DHCP invalid IP response.
#define SL_STATUS_SI91X_SMTP_AUTHENTICATION_ERROR ((sl_status_t)0x1BBA0) ///< SMTP authentication error.
#define SL_STATUS_SI91X_SMTP_OVER_SIZE_MAIL_DATA \
  ((sl_status_t)0x1BBA1) ///< No DNS server was specified, SMTP over size mail data.
#define SL_STATUS_SI91X_SMTP_INVALID_SERVER_REPLY ((sl_status_t)0x1BBA2) ///< SMTP invalid server reply.
#define SL_STATUS_SI91X_SMTP_DNS_QUERY_FAILED     ((sl_status_t)0x1BBA3) ///< DNS query failed, SMTP internal error.
#define SL_STATUS_SI91X_SMTP_BAD_DNS_ADDRESS \
  ((sl_status_t)0x1BBA4) ///< Bad DNS address, SMTP server error code received.
#define SL_STATUS_SI91X_SMTP_INVALID_PARAMETERS       ((sl_status_t)0x1BBA5) ///< SMTP invalid parameters.
#define SL_STATUS_SI91X_SMTP_PACKET_ALLOCATION_FAILED ((sl_status_t)0x1BBA6) ///< SMTP packet allocation failed.
#define SL_STATUS_SI91X_SMTP_GREET_REPLY_FAILED       ((sl_status_t)0x1BBA7) ///< SMTP Greet reply failed.
#define SL_STATUS_SI91X_SMTP_PARAMETER_ERROR          ((sl_status_t)0x1BBA8) ///< Parameter error, SMTP hello reply error.
#define SL_STATUS_SI91X_SMTP_MAIL_REPLY_ERROR         ((sl_status_t)0x1BBA9) ///< SMTP mail reply error.
#define SL_STATUS_SI91X_SMTP_RCPT_REPLY_ERROR         ((sl_status_t)0x1BBAA) ///< SMTP RCPT reply error.
#define SL_STATUS_SI91X_SMTP_MESSAGE_REPLY_ERROR      ((sl_status_t)0x1BBAB) ///< SMTP message reply error.
#define SL_STATUS_SI91X_SMTP_DATA_REPLY_ERROR         ((sl_status_t)0x1BBAC) ///< SMTP data reply error.
#define SL_STATUS_SI91X_SMTP_AUTH_REPLY_ERROR         ((sl_status_t)0x1BBAD) ///< SMTP authentication reply error.
#define SL_STATUS_SI91X_SMTP_SERVER_REPLY_ERROR       ((sl_status_t)0x1BBAE) ///< SMTP server error reply.
#define SL_STATUS_SI91X_DNS_DUPLICATE_ENTRY           ((sl_status_t)0x1BBAF) ///< DNS duplicate entry.
#define SL_STATUS_SI91X_SMTP_OVERSIZE_SERVER_REPLY    ((sl_status_t)0x1BBB1) ///< SMTP oversize server reply.
#define SL_STATUS_SI91X_SMTP_CLIENT_NOT_INITIALIZED   ((sl_status_t)0x1BBB2) ///< SMTP client not initialized.
#define SL_STATUS_SI91X_DNS_IPV6_NOT_SUPPORTED        ((sl_status_t)0x1BBB3) ///< DNS IPv6 not supported.
#define SL_STATUS_SI91X_INVALID_MAIL_INDEX_FOR_POP3_MAIL_RETRIEVE_COMMAND \
  ((sl_status_t)0x1BBC5) ///< Invalid mail index for POP3 mail retrieve command.
#define SL_STATUS_SI91X_SSL_TLS_HANDSHAKE_FAILED ((sl_status_t)0x1BBD2) ///< SSL/TLS handshake failed.
#define SL_STATUS_SI91X_FTP_CLIENT_DISCONNECTED \
  ((sl_status_t)0x1BBD3) ///< FTP client is not connected or disconnected with the FTP server.
#define SL_STATUS_SI91X_FTP_CLIENT_NOT_DISCONNECTED ((sl_status_t)0x1BBD4) ///< FTP client is not disconnected.
#define SL_STATUS_SI91X_FTP_FILE_NOT_OPENED         ((sl_status_t)0x1BBD5) ///< FTP file is not opened.
#define SL_STATUS_SI91X_SSL_TLS_HANDSHAKE_TIMEOUT_OR_FTP_FILE_NOT_CLOSED \
  ((sl_status_t)0x1BBD6) ///< SSL/TLS handshake timeout or FTP file is not closed.
#define SL_STATUS_SI91X_FTP_EXPECTED_1XX_RESPONSE_NOT_RECEIVED \
  ((sl_status_t)0x1BBD9) ///< Expected [1XX response from FTP server but not received].
#define SL_STATUS_SI91X_FTP_EXPECTED_2XX_RESPONSE_NOT_RECEIVED \
  ((sl_status_t)0x1BBDA) ///< Expected [2XX response from FTP server but not received].
#define SL_STATUS_SI91X_FTP_EXPECTED_22X_RESPONSE_NOT_RECEIVED \
  ((sl_status_t)0x1BBDB) ///< Expected [22X response from FTP server but not received].
#define SL_STATUS_SI91X_FTP_EXPECTED_23X_RESPONSE_NOT_RECEIVED \
  ((sl_status_t)0x1BBDC) ///< Expected [23X response from FTP server but not received].
#define SL_STATUS_SI91X_FTP_EXPECTED_3XX_RESPONSE_NOT_RECEIVED \
  ((sl_status_t)0x1BBDD) ///< Expected [3XX response from FTP server but not received].
#define SL_STATUS_SI91X_FTP_EXPECTED_33X_RESPONSE_NOT_RECEIVED \
  ((sl_status_t)0x1BBDE) ///< Expected [33X response from FTP server but not received].
#define SL_STATUS_SI91X_HTTP_TIMEOUT            ((sl_status_t)0x1BBE1) ///< HTTP timeout.
#define SL_STATUS_SI91X_HTTP_FAILED             ((sl_status_t)0x1BBE2) ///< HTTP failed.
#define SL_STATUS_SI91X_HTTP_PUT_CLIENT_TIMEOUT ((sl_status_t)0x1BBE7) ///< HTTP timeout for HTTP PUT client.
#define SL_STATUS_SI91X_AUTHENTICATION_ERROR    ((sl_status_t)0x1BBEB) ///< Authentication error.
#define SL_STATUS_SI91X_INVALID_PACKET_LENGTH \
  ((sl_status_t)0x1BBED) ///< Invalid packet length. Content length and received data length is mismatching.
#define SL_STATUS_SI91X_SERVER_RESPONDS_BEFORE_REQUEST_COMPLETE \
  ((sl_status_t)0x1BBEF) ///< Server responds before HTTP client request is complete.
#define SL_STATUS_SI91X_HTTP_PASSWORD_TOO_LONG ((sl_status_t)0x1BBF0) ///< HTTP/HTTPS password is too long.
#define SL_STATUS_SI91X_MQTT_PING_TIMEOUT      ((sl_status_t)0x1BBF1) ///< MQTT ping time out error.
#define SL_STATUS_SI91X_MQTT_COMMAND_SENT_IN_INCORRECT_STATE \
  ((sl_status_t)0x1BBF2)                                               ///< MQTT command sent in incorrect state.
#define SL_STATUS_SI91X_MQTT_ACK_TIMEOUT        ((sl_status_t)0x1BBF3) ///< MQTT ACK time out error.
#define SL_STATUS_SI91X_POP3_INVALID_MAIL_INDEX ((sl_status_t)0x1BBFF) ///< POP3 error for invalid mail index.
#define SL_STATUS_SI91X_SOCKET_NOT_CONNECTED \
  ((sl_status_t)0x1FFFF) ///< Listening TCP socket in device is not connected to the remote peer, or the LTCP socket is not yet opened in the device.
#define SL_STATUS_SI91X_SOCKET_LIMIT_EXCEEDED \
  ((sl_status_t)0x1FFFE) ///< Sockets not available. The error comes if the host tries to open more than 10 sockets.
#define SL_STATUS_SI91X_HTTP_OTAF_INVALID_PACKET ((sl_status_t)0x1FFFD) ///< HTTP OTAF invalid packet.
#define SL_STATUS_SI91X_TCP_IP_INIT_FAILED       ((sl_status_t)0x1FFFC) ///< TCP_IP initialization failed.
#define SL_STATUS_SI91X_CONCURRENT_IP_CREATION_ERROR \
  ((sl_status_t)0x1FFFB) ///< Cannot create IP in same interface in concurrent mode.
#define SL_STATUS_SI91X_HTTP_OTAF_INCOMPLETE_PACKET ((sl_status_t)0x1FFF4) ///< HTTP OTAF incomplete packet.
#define SL_STATUS_SI91X_INVALID_STORE_CONFIGURATION_PROFILE \
  ((sl_status_t)0x1FFF5) ///< Store configuration profile type mismatch or invalid profile type.
#define SL_STATUS_SI91X_MQTT_REMOTE_TERMINATE_ERROR ((sl_status_t)0x1FFF6) ///< MQTT remote terminate error.
#define SL_STATUS_SI91X_MQTT_KEEP_ALIVE_TERMINATE_ERROR \
  ((sl_status_t)0x1BBF1) ///< MQTT remote terminate error due to keep alive response timeout.
#define SL_STATUS_SI91X_BYTE_STUFFING_ERROR_IN_AT_MODE ((sl_status_t)0x1FFF7) ///< Byte stuffing error in AT mode.
#define SL_STATUS_SI91X_INVALID_COMMAND_OR_OPERATION \
  ((sl_status_t)0x1FFF8) ///< Invalid command (e.g. parameters insufficient or invalid in the command). Invalid operation (e.g. power save command with the same mode given twice, accessing wrong socket, creating more than allowed sockets ).
#define SL_STATUS_SI91X_HTTP_OTAF_NO_PACKET      ((sl_status_t)0x1FFF9) ///< HTTP OTAF no packet.
#define SL_STATUS_SI91X_TCP_SOCKET_NOT_CONNECTED ((sl_status_t)0x1FFFA) ///< TCP socket is not connected.
#define SL_STATUS_SI91X_MAX_STATION_COUNT_EXCEEDED \
  ((sl_status_t)0x1FFC5) ///< Station count exceeded max station supported.
#define SL_STATUS_SI91X_UNABLE_TO_SEND_TCP_DATA ((sl_status_t)0x1FFC4) ///< Unable to send TCP data.
#define SL_STATUS_SI91X_SOCKET_BUFFER_TOO_SMALL ((sl_status_t)0x1FFBC) ///< Socket buffer too small.
#define SL_STATUS_SI91X_INVALID_CONTENT_IN_DNS_RESPONSE \
  ((sl_status_t)0x1FFBB) ///< Invalid content in the DNS response to the DNS resolution query.
#define SL_STATUS_SI91X_DNS_CLASS_ERROR_IN_DNS_RESPONSE \
  ((sl_status_t)0x1FFBA) ///< DNS class error in response to the DNS resolution query.
#define SL_STATUS_SI91X_DNS_COUNT_ERROR_IN_DNS_RESPONSE \
  ((sl_status_t)0x1FFB8) ///< DNS count error in response to the DNS resolution query.
#define SL_STATUS_SI91X_DNS_RETURN_CODE_ERROR_IN_DNS_RESPONSE \
  ((sl_status_t)0x1FFB7) ///< DNS return code error in the response to the DNS resolution query.
#define SL_STATUS_SI91X_DNS_OPCODE_ERROR_IN_DNS_RESPONSE \
  ((sl_status_t)0x1FFB6) ///< DNS Opcode error in the response to the DNS resolution query.
#define SL_STATUS_SI91X_DNS_ID_MISMATCH \
  ((sl_status_t)0x1FFB5) ///< DNS id mismatch between DNS resolution request and response.
#define SL_STATUS_SI91X_INVALID_INPUT_IN_DNS_QUERY \
  ((sl_status_t)0x1FFAB) ///< An invalid input to the DNS resolution query.
#define SL_STATUS_SI91X_DNS_RESPONSE_TIMEOUT        ((sl_status_t)0x1FF42) ///< DNS response was timed out.
#define SL_STATUS_SI91X_ARP_REQUEST_FAILURE         ((sl_status_t)0x1FFA1) ///< ARP request failure.
#define SL_STATUS_SI91X_UNABLE_TO_UPDATE_TCP_WINDOW ((sl_status_t)0x1FF91) ///< Unable to update TCP window.
#define SL_STATUS_SI91X_DHCP_LEASE_EXPIRED          ((sl_status_t)0x1FF9D) ///< DHCP lease time expired.
#define SL_STATUS_SI91X_DHCP_HANDSHAKE_FAILURE      ((sl_status_t)0x1FF9C) ///< DHCP handshake failure.
#define SL_STATUS_SI91X_WEBSOCKET_CREATION_FAILED \
  ((sl_status_t)0x1FF88) ///< This error is issued when WebSocket creation failed.
#define SL_STATUS_SI91X_TRYING_TO_CONNECT_NON_EXISTENT_TCP_SERVER_SOCKET \
  ((sl_status_t)0x1FF87) ///< This error is issued when device tried to connect to a non-existent TCP server socket on the remote side.
#define SL_STATUS_SI91X_TRYING_TO_CLOSE_NON_EXISTENT_SOCKET \
  ((sl_status_t)0x1FF86) ///< This error is issued when tried to close non-existent socket. or invalid socket descriptor.
#define SL_STATUS_SI91X_INVALID_SOCKET_PARAMETERS ((sl_status_t)0x1FF85) ///< Invalid socket parameters.
#define SL_STATUS_SI91X_FEATURE_NOT_AVAILABLE     ((sl_status_t)0x1FF82) ///< Feature not supported.
#define SL_STATUS_SI91X_SOCKET_ALREADY_OPEN       ((sl_status_t)0x1FF81) ///< Socket already open.
#define SL_STATUS_SI91X_MAX_SOCKETS_EXCEEDED \
  ((sl_status_t)0x1FF80) ///< Attempt to open more than the maximum allowed number of sockets.
#define SL_STATUS_SI91X_DATA_LENGTH_EXCEEDS_MSS ((sl_status_t)0x1FF7E) ///< Data length exceeds mss.
#define SL_STATUS_SI91X_IP_CONFLICT_ERROR \
  ((sl_status_t)0x1FF75) ///< DUT unable to configure IP address due to IP conflict.
#define SL_STATUS_SI91X_FEATURE_NOT_ENABLED       ((sl_status_t)0x1FF74) ///< Feature not enabled.
#define SL_STATUS_SI91X_DHCP_SERVER_NOT_SET       ((sl_status_t)0x1FF73) ///< DHCP server not set in AP mode.
#define SL_STATUS_SI91X_AP_SET_REGION_PARAM_ERROR ((sl_status_t)0x1FF71) ///< Error in AP set region command parameters.
#define SL_STATUS_SI91X_SSL_TLS_NOT_SUPPORTED     ((sl_status_t)0x1FF70) ///< SSL/TLS not supported.
#define SL_STATUS_SI91X_JSON_NOT_SUPPORTED        ((sl_status_t)0x1FF6F) ///< JSON not supported.
#define SL_STATUS_SI91X_INVALID_OPERATING_MODE    ((sl_status_t)0x1FF6E) ///< Invalid operating mode.
#define SL_STATUS_SI91X_INVALID_SOCKET_CONFIG_PARAMS \
  ((sl_status_t)0x1FF6D)                                                  ///< Invalid socket configuration parameters.
#define SL_STATUS_SI91X_WEBSOCKET_CREATION_TIMEOUT ((sl_status_t)0x1FF6C) ///< Web socket creation timeout.
#define SL_STATUS_SI91X_PARAM_MAX_VALUE_EXCEEDED \
  ((sl_status_t)0x1FF6B) ///< Parameter maximum allowed value is exceeded.
#define SL_STATUS_SI91X_SOCKET_READ_TIMEOUT         ((sl_status_t)0x1FF6A) ///< Socket read timeout.
#define SL_STATUS_SI91X_INVALID_COMMAND_SEQUENCE    ((sl_status_t)0x1FF69) ///< Invalid command in sequence.
#define SL_STATUS_SI91X_DNS_RESPONSE_TIMEOUT_ERROR  ((sl_status_t)0x1FF42) ///< DNS response timed out.
#define SL_STATUS_SI91X_HTTP_SOCKET_CREATION_FAILED ((sl_status_t)0x1FF41) ///< HTTP socket creation failed.
#define SL_STATUS_SI91X_HTTP_GET_CMD_IN_PROGRESS    ((sl_status_t)0x10005) ///< HTTP GET command is in progress
#define SL_STATUS_SI91X_TCP_CLOSE_BEFORE_RESPONSE_ERROR \
  ((sl_status_t)0x1FF40) ///< TCP socket close command is issued before getting the response of the previous close command.
#define SL_STATUS_SI91X_WAIT_ON_HOST_FEATURE_NOT_ENABLED ((sl_status_t)0x1FF36) ///< 'Wait On Host' feature not enabled.
#define SL_STATUS_SI91X_STORE_CONFIG_CHECKSUM_INVALID \
  ((sl_status_t)0x1FF35) ///< Store configuration checksum validation failed.
#define SL_STATUS_SI91X_TCP_KEEP_ALIVE_TIMEOUT     ((sl_status_t)0x1FF33) ///< TCP keep alive timed out.
#define SL_STATUS_SI91X_TCP_ACK_FAILED_FOR_SYN_ACK ((sl_status_t)0x1FF2D) ///< TCP ACK failed for TCP SYN-ACK.
#define SL_STATUS_SI91X_MEMORY_LIMIT_EXCEEDED \
  ((sl_status_t)0x1FF2C) ///< Memory limit exceeded in a given operating mode.
#define SL_STATUS_SI91X_MEMORY_LIMIT_EXCEEDED_DURING_AUTO_JOIN \
  ((sl_status_t)0x1FF2A) ///< Memory limit exceeded in an operating mode during auto join/create.
#define SL_STATUS_SI91X_MDNS_COMMAND_NOT_SUPPORTED   ((sl_status_t)0x1FF2B) ///< MDNS command type is invalid.
#define SL_STATUS_SI91X_PUF_OPERATION_BLOCKED        ((sl_status_t)0x1CC2F) ///< PUF operation is blocked.
#define SL_STATUS_SI91X_PUF_ACTIVATION_CODE_INVALID  ((sl_status_t)0x1CC31) ///< PUF activation code invalid.
#define SL_STATUS_SI91X_PUF_INPUT_PARAMETERS_INVALID ((sl_status_t)0x1CC32) ///< PUF input parameters invalid.
#define SL_STATUS_SI91X_PUF_IN_ERROR_STATE           ((sl_status_t)0x1CC33) ///< PUF in error state.
#define SL_STATUS_SI91X_PUF_OPERATION_NOT_ALLOWED    ((sl_status_t)0x1CC34) ///< PUF operation not allowed.
#define SL_STATUS_SI91X_PUF_OPERATION_FAILED         ((sl_status_t)0x1CC35) ///< PUF operation failed.
#define SL_STATUS_SI91X_AUTO_JOIN_IN_PROGRESS \
  ((sl_status_t)0x15A5A) ///< Auto join or user store configuration going on.
#define SL_STATUS_SI91X_RSNIE_FROM_AP_INVALID     ((sl_status_t)0x1FFE1) ///< Improper RSNIE from AP to station.
#define SL_STATUS_SI91X_SNTP_MAX_ATTEMPTS_REACHED ((sl_status_t)0x1FF5F) ///< Reached maximum SNTP invalid attempts.
#define SL_STATUS_SI91X_FREQUENCY_OFFSET_ZER0     ((sl_status_t)0x100FC) ///< Frequency offset sent is zero.
#define SL_STATUS_SI91X_FREQUENCY_OFFSET_OUT_OF_LIMITS \
  ((sl_status_t)0x100FB) ///< Frequency offset specified goes beyond upper or lower limits and indicates that frequency offset cannot be changed further.

//Bluetooth Generic Error Codes
#define SL_STATUS_SI91X_UNKNOWN_HCI_COMMAND           (0x4E01) ///< Unknown HCI command .
#define SL_STATUS_SI91X_UNKNOWN_CONNECTION_IDENTIFIER (0x4E02) ///< Unknown Connection Identifier .
#define SL_STATUS_SI91X_HARDWARE_FAILURE              (0x4E03) ///< Hardware failure .
#define SL_STATUS_SI91X_PAGE_TIMEOUT                  (0x4E04) ///< Page timeout .
#define SL_STATUS_SI91X_AUTHENTICATION_FAILURE        (0x4E05) ///< Authentication failure .
#define SL_STATUS_SI91X_PIN_MISSING                   (0x4E06) ///< Pin missing .
#define SL_STATUS_SI91X_MEMORY_CAPACITY_EXCEED        (0x4E07) ///< Memory capacity exceeded .
#define SL_STATUS_SI91X_CONNECTION_TIMEOUT            (0x4E08) ///< Connection timeout .
#define SL_STATUS_SI91X_CONNECTION_LIMIT_EXCEED       (0x4E09) ///< Connection limit exceeded .
#define SL_STATUS_SI91X_SCO_LIMIT_EXCEED              (0x4E0A) ///< SCO limit exceeded .
#define SL_STATUS_SI91X_ACL_CONNECTION_ALREADY_EXIST  (0x4E0B) ///< ACL Connection already exists .
#define SL_STATUS_SI91X_COMMAND_DISALLOWED            (0x4E0C) ///< Command disallowed .
#define SL_STATUS_SI91X_CONNECTION_REJECTED_LIMITED_RESOURCES \
  (0x4E0D) ///< Connection rejected due to limited resources .
#define SL_STATUS_SI91X_CONNECTION_REJECTED_SECURITY_REASONS (0x4E0E) ///< Connection rejected due to security reasons .
#define SL_STATUS_SI91X_CONNECTION_REJECTED_FOR_BD_ADDR      (0x4E0F) ///< Connection rejected for BD address .
#define SL_STATUS_SI91X_CONNECTION_ACCEPT_TIMEOUT            (0x4E10) ///< Connection accept timeout .
#define SL_STATUS_SI91X_UNSUPPORTED_FEATURE_OR_PARAMETER     (0x4E11) ///< Unsupported feature or parameter .
#define SL_STATUS_SI91X_INVALID_HCI_COMMAND_PARAMETER        (0x4E12) ///< Invalid HCI command parameter .
#define SL_STATUS_SI91X_REMOTE_USER_TERMINATED_CONNECTION    (0x4E13) ///< Remote user terminated connection .
#define SL_STATUS_SI91X_REMOTE_DEVICE_TERMINATED_CONNECTION_LOW_RESOURCES \
  (0x4E14) ///< Remote device terminated connection due to low resources .
#define SL_STATUS_SI91X_REMOTE_DEVICE_TERMINATED_CONNECTION_POWER_OFF \
  (0x4E15) ///< Remote device terminated connection due to power off .
#define SL_STATUS_SI91X_LOCAL_DEVICE_TERMINATED_CONNECTION           (0x4E16) ///< Local device terminated connection .
#define SL_STATUS_SI91X_REPEATED_ATTEMPTS                            (0x4E17) ///< Repeated attempts .
#define SL_STATUS_SI91X_PAIRING_NOT_ALLOWED                          (0x4E18) ///< Pairing not allowed .
#define SL_STATUS_SI91X_UNKNOWN_LMP_PDU                              (0x4E19) ///< Unknown LMP PDU .
#define SL_STATUS_SI91X_UNSUPPORTED_REMOTE_FEATURE                   (0x4E1A) ///< Unsupported remote feature .
#define SL_STATUS_SI91X_SCO_OFFSET_REJECTED                          (0x4E1B) ///< SCO offset rejected .
#define SL_STATUS_SI91X_SCO_INTERVAL_REJECTED                        (0x4E1C) ///< SCO interval rejected .
#define SL_STATUS_SI91X_SCO_AIR_MODE_REJECTED                        (0x4E1D) ///< SCO Air mode rejected .
#define SL_STATUS_SI91X_INVALID_LMP_PARAMETERS                       (0x4E1E) ///< Invalid LMP parameters .
#define SL_STATUS_SI91X_UNSPECIFIED                                  (0x4E1F) ///< Unspecified .
#define SL_STATUS_SI91X_UNSUPPORTED_LMP_PARAMETER                    (0x4E20) ///< Unsupported LMP Parameter .
#define SL_STATUS_SI91X_ROLE_CHANGE_NOT_ALLOWED                      (0x4E21) ///< Role change not allowed .
#define SL_STATUS_SI91X_LMP_RESPONSE_TIMEOUT                         (0x4E22) ///< LMP response timeout .
#define SL_STATUS_SI91X_LMP_TRANSACTION_COLLISION                    (0x4E23) ///< LMP transaction collision .
#define SL_STATUS_SI91X_LMP_PDU_NOT_ALLOWED                          (0x4E24) ///< LMP PDU not allowed .
#define SL_STATUS_SI91X_ENCRYPTION_MODE_NOT_ACCEPTABLE               (0x4E25) ///< Encryption mode not acceptable .
#define SL_STATUS_SI91X_LINK_KEY_CANNOT_CHANGE                       (0x4E26) ///< Link key cannot change .
#define SL_STATUS_SI91X_REQUESTED_QOS_NOT_SUPPORTED                  (0x4E27) ///< Requested QOS not supported .
#define SL_STATUS_SI91X_INSTANT_PASSED                               (0x4E28) ///< Instant passed .
#define SL_STATUS_SI91X_PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED          (0x4E29) ///< Pairing with unit key not supported .
#define SL_STATUS_SI91X_DIFFERENT_TRANSACTION_COLLISION              (0x4E2A) ///< Different transaction collision .
#define SL_STATUS_SI91X_RESERVED_1                                   (0x4E2B) ///< Reserved 1 .
#define SL_STATUS_SI91X_QOS_PARAMETER_NOT_ACCEPTABLE                 (0x4E2C) ///< QOS parameter not acceptable .
#define SL_STATUS_SI91X_QOS_REJECTED                                 (0x4E2D) ///< QOS rejected .
#define SL_STATUS_SI91X_CHANNEL_CLASSIFICATION_NOT_SUPPORTED         (0x4E2E) ///< Channel classification not supported .
#define SL_STATUS_SI91X_INSUFFICIENT_SECURITY                        (0x4E2F) ///< Insufficient security .
#define SL_STATUS_SI91X_PARAMETER_OUT_OF_MANDATORY_RANGE             (0x4E30) ///< Parameter out of mandatory range .
#define SL_STATUS_SI91X_RESERVED_2                                   (0x4E31) ///< Reserved 2 .
#define SL_STATUS_SI91X_ROLE_SWITCH_PENDING                          (0x4E32) ///< Role switch pending .
#define SL_STATUS_SI91X_RESERVED_3                                   (0x4E33) ///< Reserved 3 .
#define SL_STATUS_SI91X_RESERVED_SLOT_VIOLATION                      (0x4E34) ///< Reserved slot violation .
#define SL_STATUS_SI91X_ROLE_SWITCH_FAILED                           (0x4E35) ///< Role switch failed .
#define SL_STATUS_SI91X_EXTENDED_INQUIRY_RESPONSE_TOO_LARGE          (0x4E36) ///< Extended Inquiry Response too large .
#define SL_STATUS_SI91X_EXTENDED_SECURE_SIMPLE_PAIRING_NOT_SUPPORTED (0x4E37) ///< Extended SSP not supported .
#define SL_STATUS_SI91X_HOST_BUSY_PAIRING                            (0X4E38) ///< Host busy pairing .
#define SL_STATUS_SI91X_PINCODE_REPLY_FOR_WRONG_BD_ADDRESS           (0x4E39) ///< Wrong BD Address .
#define SL_STATUS_SI91X_CONNECTION_FAILED_ESTABLISHED                (0x4E3E) ///< Connection Failed to be Established .
#define SL_STATUS_SI91X_INVALID_GAIN_TABLE_PAYLOAD_LENGTH            (0x4F01) ///< Invalid Gain table payload length .
#define SL_STATUS_SI91X_INVALID_REGION                               (0x4F02) ///< Invalid Region .
#define SL_STATUS_SI91X_INVALID_GAIN_TABLE_OFFSET_REQUEST_TYPE       (0x4F03) ///< Invalid Gain Table offset request type .
#define SL_STATUS_SI91X_INVALID_NODE_ID                              (0x4F04) ///< Invalid Node ID .
#define SL_STATUS_SI91X_BT_INVALID_COMMAND                           (0x4FF8) ///< BT Invalid Command .
#define SL_STATUS_SI91X_INVALID_ARGS                                 (0x0101) ///< Invalid Args .
#define SL_STATUS_SI91X_UNKNOWN                                      (0x0102) ///< Unknown .
#define SL_STATUS_SI91X_FIRMWARE_TIMEOUT                             (0x0103) ///< Firmware Timeout .
#define SL_STATUS_SI91X_MEMORY_ALLOC_FAIL                            (0x0104) ///< Memory alloc fail .
#define SL_STATUS_SI91X_IO_FAIL                                      (0x0106) ///< I/O fail .
#define SL_STATUS_SI91X_UNSUPPORTED                                  (0x0108) ///< Unsupported .
#define SL_STATUS_SI91X_SHORT_BUF                                    (0x0109) ///< Short buf .
#define SL_STATUS_SI91X_BUF_OVERFLOW                                 (0x010A) ///< Buf overflow .
#define SL_STATUS_SI91X_TOO_LARGE_BUF                                (0x010B) ///< Too large buf .
#define SL_STATUS_SI91X_I_O_ABORT                                    (0x010C) ///< I/O abort .
#define SL_STATUS_SI91X_OPEN_FAIL                                    (0x010D) ///< File open fail .
#define SL_STATUS_SI91X_OS_TASK_INVALID_PRIORITY                     (0x1010) ///< OS task invalid priority .
#define SL_STATUS_SI91X_OS_TASK_PRIO_EXISTS                          (0x1011) ///< OS task prio exists .
#define SL_STATUS_SI91X_OS_TASK_NOT_STOPPED                          (0x1012) ///< OS task not stopped .
#define SL_STATUS_SI91X_OS_SEM_MAX_VALUE                             (0x1020) ///< OS sem max value .
#define SL_STATUS_SI91X_OS_SEM_NOT_AVAILABLE                         (0x1021) ///< OS sem not available .
#define SL_STATUS_SI91X_OS_SEM_RESET                                 (0x1022) ///< OS sem reset .
#define SL_STATUS_SI91X_OS_MUTEX_NOT_OWNER                           (0x1030) ///< OS mutex not owner .
#define SL_STATUS_SI91X_OS_MUTEX_NOT_LOCKED                          (0x1031) ///< OS mutex not locked .
#define SL_STATUS_SI91X_OS_MUTEX_LOCK_FAILED                         (0x1032) ///< OS mutex lock failed .
#define SL_STATUS_SI91X_OS_MUTEX_TRY_LOCK_FAILED                     (0x1033) ///< OS mutex try lock failed .
#define SL_STATUS_SI91X_OS_MSG_QUEUE_FULL                            (0x1040) ///< OS msg queue full .
#define SL_STATUS_SI91X_OS_MESSAGE_QUEUE_EMPTY                       (0x1041) ///< OS message queue empty .
#define SL_STATUS_SI91X_PIPE_EMPTY                                   (0x1050) ///< Pipe empty .
#define SL_STATUS_SI91X_PIPE_FULL                                    (0x1051) ///< Pipe full .
#define SL_STATUS_SI91X_INVALID_LEN                                  (0x1052) ///< Invalid len .
#define SL_STATUS_SI91X_PIPE_READ_IN_USE                             (0x1053) ///< Pipe read in use .
#define SL_STATUS_SI91X_PIPE_WRITE_IN_USE                            (0x1054) ///< Pipe write in use .
#define SL_STATUS_SI91X_OS_TIMER_EXPIRED                             (0x1060) ///< OS timer expired .
#define SL_STATUS_SI91X_OS_TIMER_STATE_RUNNING                       (0x1061) ///< OS timer state running .
#define SL_STATUS_SI91X_OS_CANNOT_WAIT                               (0x1070) ///< OS cannot wait .
#define SL_STATUS_SI91X_OS_MEM_POOL_EMPTY                            (0x1080) ///< OS mem pool empty .
#define SL_STATUS_SI91X_OS_MEM_POOL_SIZE_SHORT                       (0x1081) ///< OS mem pool size short .

//BLE Generic Error Codes
#define SL_STATUS_SI91X_DIRECTED_ADVERTISING_TIMEOUT (0x4E3C) ///<  Directed Advertising Timeout .
#define SL_STATUS_SI91X_CONNECTION_TERMINATED_DUE_TO_MIC_FAILURE \
  (0x4E3D)                                                 ///<  Connection terminated due to MIC failure .
#define SL_STATUS_SI91X_INVALID_HANDLE_RANGE      (0x4E60) ///<  Invalid Handle Range .
#define SL_STATUS_SI91X_MEMORY_IS_NOT_SUFFICIENT  (0x4E61) ///<  Memory is not sufficient .
#define SL_STATUS_SI91X_INVALID_PARAMETERS        (0x4E62) ///<  Invalid Parameters .
#define SL_STATUS_SI91X_BLE_BUFFER_COUNT_EXCEEDED (0x4E63) ///<  BLE Buffer Count Exceeded .
#define SL_STATUS_SI91X_BLE_BUFFER_ALREADY_IN_USE (0x4E64) ///<  BLE Buffer already in use .
#define SL_STATUS_SI91X_INVALID_ATTRIBUTE_LENGTH \
  (0x4E65) ///<  Invalid Attribute Length When Small Buffer Mode is Configured .
#define SL_STATUS_SI91X_INVALID_NAME_LENGTH (0x4E66) ///<  Invalid Name length when set to more than 16 bytes .

//BLE Mode Error Codes
#define SL_STATUS_SI91X_INVALID_HANDLE                       (0x4A01) ///< Invalid Handle .
#define SL_STATUS_SI91X_READ_NOT_PERMITTED                   (0x4A02) ///< Read not permitted .
#define SL_STATUS_SI91X_WRITE_NOT_PERMITTED                  (0x4A03) ///< Write not permitted .
#define SL_STATUS_SI91X_INVALID_PDU                          (0x4A04) ///< Invalid PDU .
#define SL_STATUS_SI91X_INSUFFICIENT_AUTHENTICATION          (0x4A05) ///< Insufficient authentication .
#define SL_STATUS_SI91X_REQUEST_NOT_SUPPORTED                (0x4A06) ///< Request not supported .
#define SL_STATUS_SI91X_INVALID_OFFSET                       (0x4A07) ///< Invalid offset .
#define SL_STATUS_SI91X_INSUFFICIENTA_UTHORIZATION           (0x4A08) ///< Insufficient authorization .
#define SL_STATUS_SI91X_PREPARE_QUEUE_FULL                   (0x4A09) ///< Prepare queue full .
#define SL_STATUS_SI91X_ATTRIBUTE_NOT_FOUND                  (0x4A0A) ///< Attribute not found .
#define SL_STATUS_SI91X_ATTRIBUTE_NOT_LONG                   (0x4A0B) ///< Attribute not Long .
#define SL_STATUS_SI91X_INSUFFICIENT_ENCRYPTION_KEY_SIZE     (0x4A0C) ///< Insufficient encryption key size .
#define SL_STATUS_SI91X_INVALID_ATTRIBUTE_VALUE_LENGTH       (0x4A0D) ///< Invalid attribute value length .
#define SL_STATUS_SI91X_UNLIKELY_ERROR                       (0x4A0E) ///< Unlikely error .
#define SL_STATUS_SI91X_INSUFFICIENT_ENCRYPTION              (0x4A0F) ///< Insufficient encryption .
#define SL_STATUS_SI91X_UNSUPPORTED_GROUP_TYPE               (0x4A10) ///< Unsupported group type .
#define SL_STATUS_SI91X_INSUFFICIENT_RESOURCES               (0x4A11) ///< Insufficient resources .
#define SL_STATUS_SI91X_GATT_TIMEOUT                         (0x4A80) ///< GATT Timeout .
#define SL_STATUS_SI91X_DATA_MAINTAINED_IN_HOST              (0x4AFF) ///< Data Maintained in Host .
#define SL_STATUS_SI91X_SMP_PASSKEY_ENTRY_FAILED             (0x4B01) ///< SMP Passkey entry failed .
#define SL_STATUS_SI91X_SMP_OOB_NOT_AVAILABLE                (0x4B02) ///< SMP OOB not available .
#define SL_STATUS_SI91X_SMP_AUTHENTICATION_REQUIREMENTS      (0x4B03) ///< SMP Authentication Requirements .
#define SL_STATUS_SI91X_SMP_CONFIRM_VALUE_FAILED             (0x4B04) ///< SMP confirm value failed .
#define SL_STATUS_SI91X_SMP_PAIRING_NOT_SUPPORTED            (0x4B05) ///< SMP Pairing not supported .
#define SL_STATUS_SI91X_SMP_ENCRYPTION_KEY_SIZE_INSUFFICIENT (0x4B06) ///< SMP Encryption key size insufficient .
#define SL_STATUS_SI91X_SMP_COMMAND_NOT_SUPPORTED            (0x4B07) ///< SMP command not supported .
#define SL_STATUS_SI91X_SMP_PAIRING_FAILED                   (0x4B08) ///< SMP pairing failed .
#define SL_STATUS_SI91X_SMP_REPEATED_ATTEMPTS                (0x4B09) ///< SMP repeated attempts .
#define SL_STATUS_SI91X_SMP_INVALID_PARAMETERS               (0x4B0A) ///< SMP Invalid parameters .
#define SL_STATUS_SI91X_SMP_DH_KEY_CHECK_FAILED              (0x4B0B) ///< SMP DH Key check failed .
#define SL_STATUS_SI91X_SMP_NUMERIC_COMPARISON_FAILED        (0x4B0C) ///< SMP Numeric Comparison Failed .
#define SL_STATUS_SI91X_SMP_INVALID_KEYS_GENERATED           (0x4BFD) ///< SMP Invalid Keys Generated .
#define SL_STATUS_SI91X_SMP_IDENTICAL_PUBLIC_KEY             (0x4BFE) ///< SMP Identical Public Key .
#define SL_STATUS_SI91X_PSM_CONN_FAILED                      (0x4C02) ///< PSM Conn Failed .
#define SL_STATUS_SI91X_BLE_REMOTE_DEVICE_FOUND              (0x4D00) ///< BLE Remote device found .
#define SL_STATUS_SI91X_BLE_REMOTE_DEVICE_NOT_FOUND          (0x4D01) ///< BLE Remote device not found .
#define SL_STATUS_SI91X_BLE_REMOTE_DEVICE_STRUCTURE_FULL     (0x4D02) ///< BLE Remote device structure full .
#define SL_STATUS_SI91X_UNABLE_TO_CHANGE_STATE               (0x4D03) ///< Unable to change state .
#define SL_STATUS_SI91X_BLE_NOT_CONNECTED                    (0x4D04) ///< BLE not Connected .
#define SL_STATUS_SI91X_BLE_SOCKET_NOT_AVAILABLE             (0x4D05) ///< BLE socket not available. .
#define SL_STATUS_SI91X_ATTRIBUTE_RECORD_NOT_FOUND           (0x4D06) ///< Attribute record not found .
#define SL_STATUS_SI91X_ATTRIBUTE_ENTRY_NOT_FOUND            (0x4D07) ///< Attribute entry not found .
#define SL_STATUS_SI91X_PROFILE_RECORD_FULL                  (0x4D08) ///< Profile record full .
#define SL_STATUS_SI91X_ATTRIBUTE_RECORD_FULL                (0x4D09) ///< Attribute record full .
#define SL_STATUS_SI91X_BLE_PROFILE_HANDLER_INVALID          (0x4D0A) ///< BLE profile not found(profile handler invalid) .
#define SL_STATUS_SI91X_BLE_ATTRIBUTE_BUFFER_FULL            (0x4D0B) ///< BLE Attribute Buffer Full .
#define SL_STATUS_SI91X_BLE_CONNECTION_SOCK_NOT_AVAILABLE    (0x4D10) ///< BLE Connection Sock not Available .
#define SL_STATUS_SI91X_BLE_REMOTE_CREDITS_NOT_AVAILABLE     (0x4D11) ///< BLE Remote Credits not Available .
#define SL_STATUS_SI91X_PARAMETER_OUTOFF_MANADATORY_RANGE    (0x4D14) ///< Parameter is outoff the manadatory range .

/** @} */
