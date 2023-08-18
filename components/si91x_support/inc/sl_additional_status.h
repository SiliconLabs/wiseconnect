#pragma once

#define SL_STATUS_SI91X_SUBSPACE          ((sl_status_t)0x00010000)

/**
 * @addtogroup SL_STATUS_ERRORS
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
#define SL_STATUS_WIFI_DOES_NOT_EXIST                      ((sl_status_t)0x0B21) ///< Does not exist.
#define SL_STATUS_WIFI_NOT_AUTHENTICATED                   ((sl_status_t)0x0B22) ///< Not authenticated.
#define SL_STATUS_WIFI_NOT_KEYED                           ((sl_status_t)0x0B23) ///< Not keyed.
#define SL_STATUS_WIFI_IOCTL_FAIL                          ((sl_status_t)0x0B24) ///< IOCTL fail.
#define SL_STATUS_WIFI_BUFFER_UNAVAILABLE_TEMPORARY        ((sl_status_t)0x0B25) ///< Buffer unavailable temporarily.
#define SL_STATUS_WIFI_BUFFER_UNAVAILABLE_PERMANENT        ((sl_status_t)0x0B26) ///< Buffer unavailable permanently.
#define SL_STATUS_WIFI_WPS_PBC_OVERLAP                     ((sl_status_t)0x0B27) ///< WPS PBC overlap.
#define SL_STATUS_WIFI_CONNECTION_LOST                     ((sl_status_t)0x0B28) ///< Connection lost.
#define SL_STATUS_WIFI_OUT_OF_EVENT_HANDLER_SPACE          ((sl_status_t)0x0B29) ///< Cannot add extra event handler.
#define SL_STATUS_WIFI_SEMAPHORE_ERROR                     ((sl_status_t)0x0B2A) ///< Error manipulating a semaphore.
#define SL_STATUS_WIFI_FLOW_CONTROLLED                     ((sl_status_t)0x0B2B) ///< Packet retrieval cancelled due to flow control.
#define SL_STATUS_WIFI_NO_CREDITS                          ((sl_status_t)0x0B2C) ///< Packet retrieval cancelled due to lack of bus credits.
#define SL_STATUS_WIFI_NO_PACKET_TO_SEND                   ((sl_status_t)0x0B2D) ///< Packet retrieval cancelled due to no pending packets.
#define SL_STATUS_WIFI_CORE_CLOCK_NOT_ENABLED              ((sl_status_t)0x0B2E) ///< Core disabled due to no clock.
#define SL_STATUS_WIFI_CORE_IN_RESET                       ((sl_status_t)0x0B2F) ///< Core disabled - in reset.
#define SL_STATUS_WIFI_UNSUPPORTED                         ((sl_status_t)0x0B30) ///< Unsupported function.
#define SL_STATUS_WIFI_BUS_WRITE_REGISTER_ERROR            ((sl_status_t)0x0B31) ///< Error writing to WLAN register.
#define SL_STATUS_WIFI_SDIO_BUS_UP_FAIL                    ((sl_status_t)0x0B32) ///< SDIO bus failed to come up.
#define SL_STATUS_WIFI_JOIN_IN_PROGRESS                    ((sl_status_t)0x0B33) ///< Join not finished yet.
#define SL_STATUS_WIFI_NETWORK_NOT_FOUND                   ((sl_status_t)0x0B34) ///< Specified network was not found.
#define SL_STATUS_WIFI_INVALID_JOIN_STATUS                 ((sl_status_t)0x0B35) ///< Join status error.
#define SL_STATUS_WIFI_UNKNOWN_INTERFACE                   ((sl_status_t)0x0B36) ///< Unknown interface specified.
#define SL_STATUS_WIFI_SDIO_RX_FAIL                        ((sl_status_t)0x0B37) ///< Error during SDIO receive.
#define SL_STATUS_WIFI_HWTAG_MISMATCH                      ((sl_status_t)0x0B38) ///< Hardware tag header corrupt.
#define SL_STATUS_WIFI_RX_BUFFER_ALLOC_FAIL                ((sl_status_t)0x0B39) ///< Failed to allocate a buffer to receive into.
#define SL_STATUS_WIFI_BUS_READ_REGISTER_ERROR             ((sl_status_t)0x0B3A) ///< Error reading a bus hardware register.
#define SL_STATUS_WIFI_THREAD_CREATE_FAILED                ((sl_status_t)0x0B3B) ///< Failed to create a new thread.
#define SL_STATUS_WIFI_QUEUE_ERROR                         ((sl_status_t)0x0B3C) ///< Error manipulating a queue.
#define SL_STATUS_WIFI_BUFFER_POINTER_MOVE_ERROR           ((sl_status_t)0x0B3D) ///< Error moving the current pointer of a buffer.
#define SL_STATUS_WIFI_BUFFER_SIZE_SET_ERROR               ((sl_status_t)0x0B3E) ///< Error setting size of packet buffer.
#define SL_STATUS_WIFI_THREAD_STACK_NULL                   ((sl_status_t)0x0B3F) ///< Null stack pointer passed when non null was required.
#define SL_STATUS_WIFI_THREAD_DELETE_FAIL                  ((sl_status_t)0x0B40) ///< Error deleting a thread.
#define SL_STATUS_WIFI_SLEEP_ERROR                         ((sl_status_t)0x0B41) ///< Failed to put a thread to sleep.
#define SL_STATUS_WIFI_BUFFER_ALLOC_FAIL                   ((sl_status_t)0x0B42) ///< Failed to allocate a packet buffer.
#define SL_STATUS_WIFI_INTERFACE_NOT_UP                    ((sl_status_t)0x0B44) ///< Requested interface is not active.
#define SL_STATUS_WIFI_DELAY_TOO_LONG                      ((sl_status_t)0x0B45) ///< Requested delay is too long.
#define SL_STATUS_WIFI_INVALID_DUTY_CYCLE                  ((sl_status_t)0x0B46) ///< Duty cycle is outside limit 0 to 0.
#define SL_STATUS_WIFI_PMK_WRONG_LENGTH                    ((sl_status_t)0x0B47) ///< Returned pmk was the wrong length.
#define SL_STATUS_WIFI_UNKNOWN_SECURITY_TYPE               ((sl_status_t)0x0B48) ///< AP security type was unknown.
#define SL_STATUS_WIFI_WEP_NOT_ALLOWED                     ((sl_status_t)0x0B49) ///< AP not allowed to use WEP - use Open instead.
#define SL_STATUS_WIFI_WPA_KEYLEN_BAD                      ((sl_status_t)0x0B4A) ///< WPA / WPA2 key length must be between 8 & 64 bytes.
#define SL_STATUS_WIFI_FILTER_NOT_FOUND                    ((sl_status_t)0x0B4B) ///< Specified filter id not found.
#define SL_STATUS_WIFI_SPI_ID_READ_FAIL                    ((sl_status_t)0x0B4C) ///< Failed to read 0xfeedbead SPI id from chip.
#define SL_STATUS_WIFI_SPI_SIZE_MISMATCH                   ((sl_status_t)0x0B4D) ///< Mismatch in sizes between SPI and SDPCM header.
#define SL_STATUS_WIFI_ADDRESS_ALREADY_REGISTERED          ((sl_status_t)0x0B4E) ///< Attempt to register a multicast address twice.
#define SL_STATUS_WIFI_SDIO_RETRIES_EXCEEDED               ((sl_status_t)0x0B4F) ///< SDIO transfer failed too many times.
#define SL_STATUS_WIFI_NULL_PTR_ARG                        ((sl_status_t)0x0B50) ///< Null Pointer argument passed to function.
#define SL_STATUS_WIFI_THREAD_FINISH_FAIL                  ((sl_status_t)0x0B51) ///< Error deleting a thread.
#define SL_STATUS_WIFI_WAIT_ABORTED                        ((sl_status_t)0x0B52) ///< Semaphore/Mutex wait has been aborted.
#define SL_STATUS_WIFI_QUEUE_MESSAGE_UNALIGNED             ((sl_status_t)0x0B53) ///< Unaligned message in the queue.
#define SL_STATUS_WIFI_MUTEX_ERROR                         ((sl_status_t)0x0B54) ///< Error while Mutex operation.
#define SL_STATUS_WIFI_SECURE_LINK_DECRYPT_ERROR           ((sl_status_t)0x0B57) ///< Error while decryption over secure link.
#define SL_STATUS_WIFI_SECURE_LINK_KEY_RENEGOTIATION_ERROR ((sl_status_t)0x0B59) ///< Error while renegotiation of key over secure link.
#define SL_STATUS_WIFI_INVALID_OPERMODE                    ((sl_status_t)0x0B60) ///< Invalid opermode provided.
#define SL_STATUS_WIFI_INVALID_ENCRYPTION_METHOD           ((sl_status_t)0x0B61) ///< Invalid security encryption method provided.

// Si91X Wi-Fi Firmware errors
#define SL_STATUS_SI91X_SCAN_ISSUED_IN_ASSOCIATED_STATE                         ((sl_status_t)0x10002) ///< Scan command issued while device is already associated with an access point.
#define SL_STATUS_SI91X_NO_AP_FOUND                                             ((sl_status_t)0x10003) ///< No AP found.
#define SL_STATUS_SI91X_INVALID_PSK_IN_WEP_SECURITY                             ((sl_status_t)0x10004) ///< Wrong PSK is issued while the device client tries to join an access point with WEP security enabled.
#define SL_STATUS_SI91X_INVALID_BAND                                            ((sl_status_t)0x10005) ///< Invalid band.
#define SL_STATUS_SI91X_UNASSOCIATED                                            ((sl_status_t)0x10006) ///< Association not done or in unassociated state.
#define SL_STATUS_SI91X_DEAUTHENTICATION_RECEIVED_FROM_AP                       ((sl_status_t)0x10008) ///< De-authentication received from AP.
#define SL_STATUS_SI91X_ASSOCIATION_FAILED                                      ((sl_status_t)0x10009) ///< Failed to associate to access point during "Join".
#define SL_STATUS_SI91X_INVALID_CHANNEL                                         ((sl_status_t)0x1000A) ///< Invalid channel.
#define SL_STATUS_SI91X_JOIN_AUTHENTICATION_FAILED                              ((sl_status_t)0x1000E) ///< Authentication failure during "Join". Unable to find AP during join which was found during scan.
#define SL_STATUS_SI91X_BEACON_MISSED_FROM_AP_DURING_JOIN                       ((sl_status_t)0x1000F) ///< Missed beacon from AP during join.
#define SL_STATUS_SI91X_INVALID_MAC_SUPPLIED                                    ((sl_status_t)0x10013) ///< Non-existent MAC address supplied in "Disassociate" command.
#define SL_STATUS_SI91X_EAP_CONFIG_NOT_DONE                                     ((sl_status_t)0x10014) ///< EAP configuration is not done.
#define SL_STATUS_SI91X_MEMORY_FAILED_FROM_MODULE                               ((sl_status_t)0x10015) ///< Memory allocation failed or Store configuration check sum failed.
#define SL_STATUS_SI91X_INSUFFICIENT_INFO                                       ((sl_status_t)0x10016) ///< Information is wrong or insufficient in join command.
#define SL_STATUS_SI91X_NOT_AP_INTERFACE                                        ((sl_status_t)0x10017) ///< Not an AP interface.
#define SL_STATUS_SI91X_INVALID_PUSH_BUTTON_SEQUENCE                            ((sl_status_t)0x10018) ///< Push button command given before the expiry of previous push button command.
#define SL_STATUS_SI91X_REJOIN_FAILURE                                          ((sl_status_t)0x10019) ///< Access point not found. Rejoin failure.
#define SL_STATUS_SI91X_FREQUENCY_NOT_SUPPORTED                                 ((sl_status_t)0x1001A) ///< Frequency not supported.
#define SL_STATUS_SI91X_INVALID_OPERMODE                                        ((sl_status_t)0x1001B) ///< Invalid opermode.
#define SL_STATUS_SI91X_EAP_CONFIG_FAILED                                       ((sl_status_t)0x1001C) ///< EAP configuration failed.
#define SL_STATUS_SI91X_P2P_CONFIG_FAILED                                       ((sl_status_t)0x1001D) ///< P2P configuration failed.
#define SL_STATUS_SI91X_GROUP_OWNER_NEGOTIATION_FAILED                          ((sl_status_t)0x1001E) ///< Unable to start Group Owner negotiation.
#define SL_STATUS_SI91X_JOIN_TIMEOUT                                            ((sl_status_t)0x10020) ///< Join timeout.
#define SL_STATUS_SI91X_COMMAND_GIVEN_IN_INVALID_STATE                          ((sl_status_t)0x10021) ///< Command given in incorrect state.
#define SL_STATUS_SI91X_INVALID_QUERY_GO_PARAMS                                 ((sl_status_t)0x10022) ///< Query GO parameters issued in incorrect operating mode.
#define SL_STATUS_SI91X_ACCESS_POINT_FAILED                                     ((sl_status_t)0x10023) ///< Unable to form access point.
#define SL_STATUS_SI91X_INVALID_SCAN_INFO                                       ((sl_status_t)0x10024) ///< Wrong scan input parameters supplied to "Scan" command.
#define SL_STATUS_SI91X_COMMAND_ISSUED_IN_REJOIN_STATE                          ((sl_status_t)0x10025) ///< Command issued during re-join in progress.
#define SL_STATUS_SI91X_WRONG_PARAMETERS                                        ((sl_status_t)0x10026) ///< Wrong parameters the command request.
#define SL_STATUS_SI91X_PROVISION_DISCOVERY_FAILED_IN_P2P                       ((sl_status_t)0x10027) ///< Provision discovery failed in P2P.
#define SL_STATUS_SI91X_INVALID_PSK_LENGTH                                      ((sl_status_t)0x10028) ///< PSK length less than 8 bytes or more than 63 bytes.
#define SL_STATUS_SI91X_FAILED_TO_CLEAR_OR_SET_EAP_CERTIFICATE                  ((sl_status_t)0x10029) ///< Failed to clear or to set the Enterprise Certificate (Set Certificate).
#define SL_STATUS_SI91X_P2P_GO_NEGOTIATED_FAILED                                ((sl_status_t)0x1002A) ///< P2P Go negotiation failed.
#define SL_STATUS_SI91X_ASSOCIATION_TIMEOUT_IN_P2P_WPS_MODE                     ((sl_status_t)0x1002B) ///< Association between nodes failed in P2P WPS mode due to timeout.
#define SL_STATUS_SI91X_COMMAND_ISSUED_WHILE_INTERNAL_OPERATION                 ((sl_status_t)0x1002C) ///< If a command is issued by the Host when the device is internally executing auto-join or auto-create.
#define SL_STATUS_SI91X_INVALID_WEP_KEY_LEN                                     ((sl_status_t)0x1002D) ///< WEP key is of wrong length.
#define SL_STATUS_SI91X_ICMP_REQUEST_TIMEOUT_ERROR                              ((sl_status_t)0x1002E) ///< ICMP request timeout error.
#define SL_STATUS_SI91X_ICMP_DATA_SIZE_EXCEED_MAX_LIMIT                         ((sl_status_t)0x1002F) ///< ICMP data size exceeds maximum limit.
#define SL_STATUS_SI91X_SEND_DATA_PACKET_EXCEED_LIMIT                           ((sl_status_t)0x10030) ///< Send data packet exceeded the limit or length that is mentioned (or) MQTT publish data and publish data length mismatched (or) MQTT Send data packet exceeded the limit.
#define SL_STATUS_SI91X_ARP_CACHE_NOT_FOUND                                     ((sl_status_t)0x10031) ///< ARP cache entry not found.
#define SL_STATUS_SI91X_UART_COMMAND_TIMEOUT                                    ((sl_status_t)0x10032) ///< UART command timeout happened.
#define SL_STATUS_SI91X_FIXED_DATA_RATE_NOT_SUPPORTED_BY_AP                     ((sl_status_t)0x10033) ///< Fixed data rate is not supported by connecting AP.
#define SL_STATUS_SI91X_USERNAME_PASSWORD_CLIENTID_TOPIC_MAX_LEN                ((sl_status_t)0x10036) ///< Maximum length exceeded of username/password/client_id/topic in MQTT.
#define SL_STATUS_SI91X_INVALID_WPS_PIN                                         ((sl_status_t)0x10037) ///< Wrong WPS PIN.
#define SL_STATUS_SI91X_INVALID_WPS_PIN_LEN                                     ((sl_status_t)0x10038) ///< Wrong WPS PIN length.
#define SL_STATUS_SI91X_INVALID_PMK_LEN                                         ((sl_status_t)0x10039) ///< Wrong PMK length.
#define SL_STATUS_SI91X_SSID_NOT_PRESENT_FOR_PMK_GENERATION                     ((sl_status_t)0x1003A) ///< SSID not present for PMK generation.
#define SL_STATUS_SI91X_SSID_INCORRECT_PMK_GENERATION                           ((sl_status_t)0x1003B) ///< SSID incorrect for PMK generation(more than 32 bytes).
#define SL_STATUS_SI91X_BAND_NOT_SUPPORTED                                      ((sl_status_t)0x1003C) ///< Band not supported.
#define SL_STATUS_SI91X_INVALID_USR_STORE_CONFIGURATION_LEN                     ((sl_status_t)0x1003D) ///< User store configuration invalid length.
#define SL_STATUS_SI91X_INVALID_COMMAND_LEN                                     ((sl_status_t)0x1003E) ///< Error in length of the command (Exceeds number of characters is mentioned in the PRM).
#define SL_STATUS_SI91X_DATA_PACKET_DROPPED                                     ((sl_status_t)0x1003F) ///< Data packet dropped.
#define SL_STATUS_SI91X_WEP_KEY_NOT_GIVEN                                       ((sl_status_t)0x10040) ///< WEP key not given.
#define SL_STATUS_SI91X_INVALID_STORE_CONFIG_PROFILE                            ((sl_status_t)0x10041) ///< Error in length of store config profile.
#define SL_STATUS_SI91X_MISSING_PSK_OR_PMK                                      ((sl_status_t)0x10042) ///< PSK or PMK not given.
#define SL_STATUS_SI91X_INVALID_SECURITY_MODE_IN_JOIN_COMMAND                   ((sl_status_t)0x10043) ///< Security mode given in join command is invalid.
#define SL_STATUS_SI91X_MAX_BEACON_MISCOUNT                                     ((sl_status_t)0x10044) ///< Beacon miscount reaches max beacon miscount (De-authentication due to beacon miss).
#define SL_STATUS_SI91X_DEAUTH_REQUEST_FROM_SUPPLICANT                          ((sl_status_t)0x10045) ///< De-authentication received from supplicant.
#define SL_STATUS_SI91X_DEAUTH_REQUEST_FROM_FROM_AP                             ((sl_status_t)0x10046) ///< De-authentication received from AP after channel switching.
#define SL_STATUS_SI91X_MISSED_SYNCHRONIZATION                                  ((sl_status_t)0x10047) ///< Synchronization missed.
#define SL_STATUS_SI91X_AUTHENTICATION_TIMEOUT                                  ((sl_status_t)0x10048) ///< Authentication timeout occurred.
#define SL_STATUS_SI91X_ASSOCIATION_TIMEOUT                                     ((sl_status_t)0x10049) ///< Association timeout.
#define SL_STATUS_SI91X_BG_SCAN_NOT_ALLOWED                                     ((sl_status_t)0x1004A) ///< BG scan in given channels is not allowed.
#define SL_STATUS_SI91X_SSID_MISMATCH                                           ((sl_status_t)0x1004B) ///< Scanned SSID and SSID given in join are not matching.
#define SL_STATUS_SI91X_CLIENT_MAX_SUPPORTED_EXCEEDED                           ((sl_status_t)0x1004C) ///< Given number of clients exceeded max number of stations supported.
#define SL_STATUS_SI91X_HT_CAPABILITIES_NOT_SUPPORTED                           ((sl_status_t)0x1004D) ///< Given HT capabilities are not supported.
#define SL_STATUS_SI91X_UART_FLOW_NOT_SUPPORTED                                 ((sl_status_t)0x1004E) ///< UART Flow control not supported.
#define SL_STATUS_SI91X_ZB_BT_BLE_PKT_RECEIVED                                  ((sl_status_t)0x1004F) ///< ZB/BT/BLE packet received and protocol is not enabled.
#define SL_STATUS_SI91X_MGMT_PKT_DROPPED                                        ((sl_status_t)0x10050) ///< MGMT pkt dropped.
#define SL_STATUS_SI91X_INVALID_RF_CURRENT_MODE                                 ((sl_status_t)0x10051) ///< Invalid RF current mode.
#define SL_STATUS_SI91X_POWER_SAVE_NOT_SUPPORTED                                ((sl_status_t)0x10052) ///< Power save support is not present for a given interface.
#define SL_STATUS_SI91X_CONCURRENT_AP_IN_CONNECTED_STATE                        ((sl_status_t)0x10053) ///< Concurrent AP in connected state.
#define SL_STATUS_SI91X_CONNECTED_AP_OR_STATION_CHANNEL_MISMATCH                ((sl_status_t)0x10054) ///< Connected AP or Station channel mismatch.
#define SL_STATUS_SI91X_IAP_COPROCESSOR_ERROR                                   ((sl_status_t)0x10055) ///< IAP co processor error.
#define SL_STATUS_SI91X_WPS_NOT_SUPPORTED                                       ((sl_status_t)0x10056) ///< WPS not supported in current operating mode.
#define SL_STATUS_SI91X_CONCURRENT_AP_CHANNEL_MISMATCH                          ((sl_status_t)0x10057) ///< Concurrent AP doesn't have same channel as connected station channel.
#define SL_STATUS_SI91X_PBC_SESSION_OVERLAP                                     ((sl_status_t)0x10058) ///< PBC session overlap error.
#define SL_STATUS_SI91X_BT_FEATURE_BITMAP_INVALID                               ((sl_status_t)0x10059) ///< BT feature bit map invalid.
#define SL_STATUS_SI91X_FOUR_WAY_HANDSHAKE_FAILED                               ((sl_status_t)0x1005A) ///< 4/4 confirmation of 4 way handshake failed.
#define SL_STATUS_SI91X_MAC_ADDRESS_NOT_PRESENT_IN_MAC_JOIN                     ((sl_status_t)0x1005B) ///< MAC address not present in MAC based join.
#define SL_STATUS_SI91X_CONCURRENT_MODE_DOWN                                    ((sl_status_t)0x1005C) ///< Concurrent mode, both AP and client should be up, to enable configuration.
#define SL_STATUS_SI91X_CERTIFICATE_LOAD_NOT_ALLOWED_IN_FLASH                   ((sl_status_t)0x1005D) ///< Certificate load not allowed in flash.
#define SL_STATUS_SI91X_CERTIFICATE_LOAD_NOT_ALLOWED_IN_RAM                     ((sl_status_t)0x1005E) ///< Certificate load not allowed in RAM.
#define SL_STATUS_SI91X_WRONG_CERTIFICATE_LOAD_INDEX                            ((sl_status_t)0x1005F) ///< Certificate load failed due to wrong inx.
#define SL_STATUS_SI91X_AP_HT_CAPS_NOT_ENABLED                                  ((sl_status_t)0x10060) ///< AP HT caps not enabled.
#define SL_STATUS_SI91X_ADDRESS_FAMILY_NOT_SUPPORTED                            ((sl_status_t)0x10061) ///< Address family not supported by protocol.
#define SL_STATUS_SI91X_INVALID_BEACON_INTERVAL_OR_DTM_PERIOD                   ((sl_status_t)0x10062) ///< Invalid beacon interval or DTIM period provided.
#define SL_STATUS_SI91X_INVALID_CONFIG_RANGE_PROVIDED                           ((sl_status_t)0x10063) ///< Invalid range of the configuration provided.
#define SL_STATUS_SI91X_INVALID_CONFIG_TYPE                                     ((sl_status_t)0x10064) ///< RTS THRESHOLD Config type is invalid.
#define SL_STATUS_SI91X_ERROR_WITH_MQTT_COMMAND                                 ((sl_status_t)0x10065) ///< Error with MQTT command.
#define SL_STATUS_SI91X_HIGHER_LISTEN_INTERVAL                                  ((sl_status_t)0x10066) ///< listen interval in power save is greater than that of join.
#define SL_STATUS_SI91X_WLAN_RADIO_DEREGISTERED                                 ((sl_status_t)0x10067) ///< WLAN radio deregistered.
#define SL_STATUS_SI91X_SAE_FAILURE_DUE_TO_MULTIPLE_CONFIRM_FRAMES_FROM_AP      ((sl_status_t)0x10069) ///< SAE failure due to multiple confirm frames from AP.
#define SL_STATUS_SI91X_EC_GROUP_STATION_UNSUPPORTED_BY_AP                      ((sl_status_t)0x1006A) ///< AP does not support the EC-group set by station.
#define SL_STATUS_SI91X_SECURITY_ASSOCIATION_QUERY_TIMEOUT                      ((sl_status_t)0x10072) ///< Disconnected due to Security Association Query Timeout. 
#define SL_STATUS_SI91X_EAPOL_TIMEOUT                                           ((sl_status_t)0x10073) ///< EAPOL Timeout.
#define SL_STATUS_SI91X_DUPLICATE_ENTRY_EXISTS_IN_DNS_SERVER_TABLE              ((sl_status_t)0x100AF) ///< Duplicate entry exists in DNS server table.
#define SL_STATUS_SI91X_NO_MEM_AVAILABLE                                        ((sl_status_t)0x100B1) ///< Memory error: No memory available.
#define SL_STATUS_SI91X_INVALID_CHARACTERS_IN_JSON_OBJECT                       ((sl_status_t)0x100B2) ///< Invalid characters in JSON object.
#define SL_STATUS_SI91X_NO_KEY_FOUND                                            ((sl_status_t)0x100B3) ///< Update commands: No such key found.
#define SL_STATUS_SI91X_NO_FILE_FOUND                                           ((sl_status_t)0x100B4) ///< No such file found: Re-check filename.
#define SL_STATUS_SI91X_NO_WEB_PAGE_EXISTS_WITH_SAME_FILENAME                   ((sl_status_t)0x100B5) ///< No corresponding web page exists with same filename.
#define SL_STATUS_SI91X_SPACE_UNAVAILABLE_FOR_NEW_FILE                          ((sl_status_t)0x100B6) ///< Space unavailable for new file.
#define SL_STATUS_SI91X_INVALID_INPUT_DATA                                      ((sl_status_t)0x100C1) ///< Invalid input data, Re-check filename, lengths, etc.
#define SL_STATUS_SI91X_NO_SPACE_AVAILABLE_FOR_NEW_FILE                         ((sl_status_t)0x100C2) ///< Space unavailable for new file.
#define SL_STATUS_SI91X_EXISTING_FILE_OVERWRITE                                 ((sl_status_t)0x100C3) ///< Existing file overwrite: Exceeds size of previous file. Use erase and try again.
#define SL_STATUS_SI91X_NO_SUCH_FILE_FOUND                                      ((sl_status_t)0x100C4) ///< No such file found. Re-check filename.
#define SL_STATUS_SI91X_MEMORY_ERROR                                            ((sl_status_t)0x100C5) ///< Memory Error: No memory available.
#define SL_STATUS_SI91X_RECEIVED_MORE_WEB_PAGE_DATA                             ((sl_status_t)0x100C6) ///< Received more web page data than the total length initially specified.
#define SL_STATUS_SI91X_SET_REGION_ERROR                                        ((sl_status_t)0x100C7) ///< Error in set region command.
#define SL_STATUS_SI91X_INVALID_WEBPAGE_CURRENT_CHUNK_LEN                       ((sl_status_t)0x100C8) ///< Web page current chunk length is incorrect.
#define SL_STATUS_SI91X_AP_SET_REGION_COMMAND_ERROR                             ((sl_status_t)0x100CA) ///< Error in AP set region command.
#define SL_STATUS_SI91X_AP_SET_REGION_COMMAND_PARAMETERS_ERROR                  ((sl_status_t)0x100CB) ///< Error in AP set region command parameters.
#define SL_STATUS_SI91X_REGION_CODE_NOT_SUPPORTED                               ((sl_status_t)0x100CC) ///< Region code not supported.
#define SL_STATUS_SI91X_EXTRACTING_COUNTRY_REGION_FROM_BEACON_FAILED            ((sl_status_t)0x100CD) ///< Error in extracting country region from beacon.
#define SL_STATUS_SI91X_SELECTED_REGION_NOT_SUPPORTED                           ((sl_status_t)0x100CE) ///< Device does not have selected region support.
#define SL_STATUS_SI91X_SSL_TLS_CONTEXT_CREATION_FAILED                         ((sl_status_t)0x100D1) ///< SSL/TLS context create failed.
#define SL_STATUS_SI91X_SSL_TLS_HANDSHAKE_FAIL                                  ((sl_status_t)0x100D2) ///< SSL/TLS handshake failed. Socket will be closed.
#define SL_STATUS_SI91X_SSL_TLS_MAX_SOCKETS_REACHED_OR_FTP_CLIENT_NOT_CONNECTED ((sl_status_t)0x100D3) ///< SSL/TLS max sockets reached. Or FTP client is not connected.
#define SL_STATUS_SI91X_CIPHER_SET_FAILED                                       ((sl_status_t)0x100D4) ///< Cipher set failure.
#define SL_STATUS_SI91X_HTTP_CREDENTIALS_MAX_LEN_EXCEEDED                       ((sl_status_t)0x100F1) ///< HTTP credentials maximum length exceeded.
#define SL_STATUS_SI91X_FEATURE_NOT_SUPPORTED                                   ((sl_status_t)0x100F7) ///< Feature not supported.
#define SL_STATUS_SI91X_FLASH_WRITE_OR_FLASH_DATA_VERIFICATION_FAILED           ((sl_status_t)0x100F8) ///< Unable to write to flash OR flash data verification failed.
#define SL_STATUS_SI91X_CALIBRATION_DATA_VERIFICATION_FAILED                    ((sl_status_t)0x100F9) ///< Calibration data verification failed.
#define SL_STATUS_SI91X_SNMP_INTERNAL_ERROR                                     ((sl_status_t)0x10100) ///< SNMP internal error.
#define SL_STATUS_SI91X_SNMP_INVALID_IP_PROTOCOL                                ((sl_status_t)0x10104) ///< SNMP invalid IP protocol error.
#define SL_STATUS_SI91X_NO_DATA_RECEIVED_OR_RECEIVE_TIMEOUT                     ((sl_status_t)0x1BB01) ///< No data received or receive timeout.
#define SL_STATUS_SI91X_INSUFFICIENT_DATA_FOR_TIME_CONVERSION                   ((sl_status_t)0x1BB08) ///< Insufficient data for converting NTP time to mm-dd-yy time format.
#define SL_STATUS_SI91X_INVALID_SNTP_SERVER_ADDRESS                             ((sl_status_t)0x1BB0A) ///< Invalid SNTP server address.
#define SL_STATUS_SI91X_SNTP_CLIENT_NOT_STARTED                                 ((sl_status_t)0x1BB0B) ///< SNTP client not started.
#define SL_STATUS_SI91X_SNTP_SERVER_UNAVAILABLE                                 ((sl_status_t)0x1BB10) ///< SNTP server not available. Client will not get any time update service from current server.
#define SL_STATUS_SI91X_SNTP_SERVER_AUTHENTICATION_FAILED                       ((sl_status_t)0x1BB15) ///< SNTP server authentication failed.
#define SL_STATUS_SI91X_INTERNAL_ERROR                                          ((sl_status_t)0x1BB0E) ///< Internal error.
#define SL_STATUS_SI91X_MULTICAST_IP_ADDRESS_ENTRY_NOT_FOUND                    ((sl_status_t)0x1BB16) ///< Entry not found for multicast IP address.
#define SL_STATUS_SI91X_MULTICAST_NO_ENTRIES_FOUND                              ((sl_status_t)0x1BB17) ///< No more entries found for multicast.
#define SL_STATUS_SI91X_IP_ADDRESS_ERROR                                        ((sl_status_t)0x1BB21) ///< IP address error.
#define SL_STATUS_SI91X_SOCKET_ALREADY_BOUND                                    ((sl_status_t)0x1BB22) ///< Socket already bound.
#define SL_STATUS_SI91X_PORT_UNAVAILABLE                                        ((sl_status_t)0x1BB23) ///< Port not available.
#define SL_STATUS_SI91X_SOCKET_NOT_CREATED                                      ((sl_status_t)0x1BB27) ///< Socket is not created.
#define SL_STATUS_SI91X_ICMP_REQUEST_FAILED                                     ((sl_status_t)0x1BB29) ///< ICMP request failed.
#define SL_STATUS_SI91X_MAX_LISTEN_SOCKETS_REACHED                              ((sl_status_t)0x1BB33) ///< Maximum listen sockets reached.
#define SL_STATUS_SI91X_DHCP_DUPLICATE_LISTEN                                   ((sl_status_t)0x1BB34) ///< DHCP duplicate listen.
#define SL_STATUS_SI91X_PORT_NOT_IN_CLOSE_STATE                                 ((sl_status_t)0x1BB35) ///< Port not in closed state.
#define SL_STATUS_SI91X_SOCKET_CLOSED                                           ((sl_status_t)0x1BB36) ///< Socket is closed or in process of closing.
#define SL_STATUS_SI91X_PROCESS_IN_PROGRESS                                     ((sl_status_t)0x1BB37) ///< Process in progress.
#define SL_STATUS_SI91X_CONNECT_TO_NON_EXISTING_TCP_SERVER_SOCKET               ((sl_status_t)0x1BB38) ///< Trying to connect non-existing TCP server socket.
#define SL_STATUS_SI91X_ERROR_IN_LEN_OF_THE_COMMAND                             ((sl_status_t)0x1BB3E) ///< Error in length of the command ('Exceeds number of characters' is mentioned in the PRM).
#define SL_STATUS_SI91X_WRONG_PACKET_INFO                                       ((sl_status_t)0x1BB40) ///< Wrong packet info.
#define SL_STATUS_SI91X_SOCKET_STILL_BOUND                                      ((sl_status_t)0x1BB42) ///< Socket is still bound.
#define SL_STATUS_SI91X_NO_FREE_PORT                                            ((sl_status_t)0x1BB45) ///< No free port.
#define SL_STATUS_SI91X_INVALID_PORT                                            ((sl_status_t)0x1BB46) ///< Invalid port.
#define SL_STATUS_SI91X_FEATURE_UNSUPPORTED                                     ((sl_status_t)0x1BB4B) ///< Feature not supported.
#define SL_STATUS_SI91X_SOCKET_IN_UNCONNECTED_STATE                             ((sl_status_t)0x1BB50) ///< Socket is not in connected state. Disconnected from server. In case of FTP, user need to give destroy command after receiving this error.
#define SL_STATUS_SI91X_POP3_SESSION_CREATION_FAILED                            ((sl_status_t)0x1BB87) ///< POP3 session creation failed / POP3 session got terminated.
#define SL_STATUS_SI91X_DHCPV6_HANDSHAKE_FAIL                                   ((sl_status_t)0x1BB9C) ///< DHCPv6 handshake failure.
#define SL_STATUS_SI91X_DHCP_INVALID_IP_RESPONSE                                ((sl_status_t)0x1BB9D) ///< DHCP invalid IP response.
#define SL_STATUS_SI91X_SMTP_AUTHENTICATION_ERROR                               ((sl_status_t)0x1BBA0) ///< SMTP authentication error.
#define SL_STATUS_SI91X_SMTP_OVER_SIZE_MAIL_DATA                                ((sl_status_t)0x1BBA1) ///< No DNS server was specified, SMTP over size mail data.
#define SL_STATUS_SI91X_SMTP_INVALID_SERVER_REPLY                               ((sl_status_t)0x1BBA2) ///< SMTP invalid server reply.
#define SL_STATUS_SI91X_SMTP_DNS_QUERY_FAILED                                   ((sl_status_t)0x1BBA3) ///< DNS query failed, SMTP internal error.
#define SL_STATUS_SI91X_SMTP_BAD_DNS_ADDRESS                                    ((sl_status_t)0x1BBA4) ///< Bad DNS address, SMTP server error code received.
#define SL_STATUS_SI91X_SMTP_INVALID_PARAMETERS                                 ((sl_status_t)0x1BBA5) ///< SMTP invalid parameters.
#define SL_STATUS_SI91X_SMTP_PACKET_ALLOCATION_FAILED                           ((sl_status_t)0x1BBA6) ///< SMTP packet allocation failed.
#define SL_STATUS_SI91X_SMTP_GREET_REPLY_FAILED                                 ((sl_status_t)0x1BBA7) ///< SMTP Greet reply failed.
#define SL_STATUS_SI91X_SMTP_PARAMETER_ERROR                                    ((sl_status_t)0x1BBA8) ///< Parameter error, SMTP hello reply error.
#define SL_STATUS_SI91X_SMTP_MAIL_REPLY_ERROR                                   ((sl_status_t)0x1BBA9) ///< SMTP mail reply error.
#define SL_STATUS_SI91X_SMTP_RCPT_REPLY_ERROR                                   ((sl_status_t)0x1BBAA) ///< SMTP RCPT reply error.
#define SL_STATUS_SI91X_SMTP_MESSAGE_REPLY_ERROR                                ((sl_status_t)0x1BBAB) ///< SMTP message reply error.
#define SL_STATUS_SI91X_SMTP_DATA_REPLY_ERROR                                   ((sl_status_t)0x1BBAC) ///< SMTP data reply error.
#define SL_STATUS_SI91X_SMTP_AUTH_REPLY_ERROR                                   ((sl_status_t)0x1BBAD) ///< SMTP authentication reply error.
#define SL_STATUS_SI91X_SMTP_SERVER_REPLY_ERROR                                 ((sl_status_t)0x1BBAE) ///< SMTP server error reply.
#define SL_STATUS_SI91X_DNS_DUPLICATE_ENTRY                                     ((sl_status_t)0x1BBAF) ///< DNS duplicate entry.
#define SL_STATUS_SI91X_SMTP_OVERSIZE_SERVER_REPLY                              ((sl_status_t)0x1BBB1) ///< SMTP oversize server reply.
#define SL_STATUS_SI91X_SMTP_CLIENT_NOT_INITIALIZED                             ((sl_status_t)0x1BBB2) ///< SMTP client not initialized.
#define SL_STATUS_SI91X_DNS_IPV6_NOT_SUPPORTED                                  ((sl_status_t)0x1BBB3) ///< DNS IPv6 not supported.
#define SL_STATUS_SI91X_INVALID_MAIL_INDEX_FOR_POP3_MAIL_RETRIEVE_COMMAND       ((sl_status_t)0x1BBC5) ///< Invalid mail index for POP3 mail retrieve command.
#define SL_STATUS_SI91X_SSL_TLS_HANDSHAKE_FAILED                                ((sl_status_t)0x1BBD2) ///< SSL/TLS handshake failed.
#define SL_STATUS_SI91X_FTP_CLIENT_DISCONNECTED                                 ((sl_status_t)0x1BBD3) ///< FTP client is not connected or disconnected with the FTP server.
#define SL_STATUS_SI91X_FTP_CLIENT_NOT_DISCONNECTED                             ((sl_status_t)0x1BBD4) ///< FTP client is not disconnected.
#define SL_STATUS_SI91X_FTP_FILE_NOT_OPENED                                     ((sl_status_t)0x1BBD5) ///< FTP file is not opened.
#define SL_STATUS_SI91X_SSL_TLS_HANDSHAKE_TIMEOUT_OR_FTP_FILE_NOT_CLOSED        ((sl_status_t)0x1BBD6) ///< SSL/TLS handshake timeout or FTP file is not closed.
#define SL_STATUS_SI91X_FTP_EXPECTED_1XX_RESPONSE_NOT_RECEIVED                  ((sl_status_t)0x1BBD9) ///< Expected [1XX response from FTP server but not received].
#define SL_STATUS_SI91X_FTP_EXPECTED_2XX_RESPONSE_NOT_RECEIVED                  ((sl_status_t)0x1BBDA) ///< Expected [2XX response from FTP server but not received].
#define SL_STATUS_SI91X_FTP_EXPECTED_22X_RESPONSE_NOT_RECEIVED                  ((sl_status_t)0x1BBDB) ///< Expected [22X response from FTP server but not received].
#define SL_STATUS_SI91X_FTP_EXPECTED_23X_RESPONSE_NOT_RECEIVED                  ((sl_status_t)0x1BBDC) ///< Expected [23X response from FTP server but not received].
#define SL_STATUS_SI91X_FTP_EXPECTED_3XX_RESPONSE_NOT_RECEIVED                  ((sl_status_t)0x1BBDD) ///< Expected [3XX response from FTP server but not received].
#define SL_STATUS_SI91X_FTP_EXPECTED_33X_RESPONSE_NOT_RECEIVED                  ((sl_status_t)0x1BBDE) ///< Expected [33X response from FTP server but not received].
#define SL_STATUS_SI91X_HTTP_TIMEOUT                                            ((sl_status_t)0x1BBE1) ///< HTTP timeout.
#define SL_STATUS_SI91X_HTTP_FAILED                                             ((sl_status_t)0x1BBE2) ///< HTTP failed.
#define SL_STATUS_SI91X_HTTP_PUT_CLIENT_TIMEOUT                                 ((sl_status_t)0x1BBE7) ///< HTTP timeout for HTTP PUT client.
#define SL_STATUS_SI91X_AUTHENTICATION_ERROR                                    ((sl_status_t)0x1BBEB) ///< Authentication error.
#define SL_STATUS_SI91X_INVALID_PACKET_LENGTH                                   ((sl_status_t)0x1BBED) ///< Invalid packet length. Content length and received data length is mismatching.
#define SL_STATUS_SI91X_SERVER_RESPONDS_BEFORE_REQUEST_COMPLETE                 ((sl_status_t)0x1BBEF) ///< Server responds before HTTP client request is complete.
#define SL_STATUS_SI91X_HTTP_PASSWORD_TOO_LONG                                  ((sl_status_t)0x1BBF0) ///< HTTP/HTTPS password is too long.
#define SL_STATUS_SI91X_MQTT_PING_TIMEOUT                                       ((sl_status_t)0x1BBF1) ///< MQTT ping time out error.
#define SL_STATUS_SI91X_MQTT_COMMAND_SENT_IN_INCORRECT_STATE                    ((sl_status_t)0x1BBF2) ///< MQTT command sent in incorrect state.
#define SL_STATUS_SI91X_MQTT_ACK_TIMEOUT                                        ((sl_status_t)0x1BBF3) ///< MQTT ACK time out error.
#define SL_STATUS_SI91X_POP3_INVALID_MAIL_INDEX                                 ((sl_status_t)0x1BBFF) ///< POP3 error for invalid mail index.
#define SL_STATUS_SI91X_SOCKET_NOT_CONNECTED                                    ((sl_status_t)0x1FFFF) ///< Listening TCP socket in device is not connected to the remote peer, or the LTCP socket is not yet opened in the device.
#define SL_STATUS_SI91X_SOCKET_LIMIT_EXCEEDED                                   ((sl_status_t)0x1FFFE) ///< Sockets not available. The error comes if the host tries to open more than 10 sockets.
#define SL_STATUS_SI91X_HTTP_OTAF_INVALID_PACKET                                ((sl_status_t)0x1FFFD) ///< HTTP OTAF invalid packet.
#define SL_STATUS_SI91X_TCP_IP_INIT_FAILED                                      ((sl_status_t)0x1FFFC) ///< TCP_IP initialization failed.
#define SL_STATUS_SI91X_CONCURRENT_IP_CREATION_ERROR                            ((sl_status_t)0x1FFFB) ///< Cannot create IP in same interface in concurrent mode.
#define SL_STATUS_SI91X_HTTP_OTAF_INCOMPLETE_PACKET                             ((sl_status_t)0x1FFF4) ///< HTTP OTAF incomplete packet.
#define SL_STATUS_SI91X_INVALID_STORE_CONFIGURATION_PROFILE                     ((sl_status_t)0x1FFF5) ///< Store configuration profile type mismatch or invalid profile type.
#define SL_STATUS_SI91X_MQTT_REMOTE_TERMINATE_ERROR                             ((sl_status_t)0x1FFF6) ///< MQTT remote terminate error.
#define SL_STATUS_SI91X_BYTE_STUFFING_ERROR_IN_AT_MODE                          ((sl_status_t)0x1FFF7) ///< Byte stuffing error in AT mode.
#define SL_STATUS_SI91X_INVALID_COMMAND_OR_OPERATION                            ((sl_status_t)0x1FFF8) ///< Invalid command (e.g. parameters insufficient or invalid in the command). Invalid operation (e.g. power save command with the same mode given twice, accessing wrong socket, creating more than allowed sockets ).
#define SL_STATUS_SI91X_HTTP_OTAF_NO_PACKET                                     ((sl_status_t)0x1FFF9) ///< HTTP OTAF no packet.
#define SL_STATUS_SI91X_TCP_SOCKET_NOT_CONNECTED                                ((sl_status_t)0x1FFFA) ///< TCP socket is not connected.
#define SL_STATUS_SI91X_MAX_STATION_COUNT_EXCEEDED                              ((sl_status_t)0x1FFC5) ///< Station count exceeded max station supported.
#define SL_STATUS_SI91X_UNABLE_TO_SEND_TCP_DATA                                 ((sl_status_t)0x1FFC4) ///< Unable to send TCP data.
#define SL_STATUS_SI91X_SOCKET_BUFFER_TOO_SMALL                                 ((sl_status_t)0x1FFBC) ///< Socket buffer too small.
#define SL_STATUS_SI91X_INVALID_CONTENT_IN_DNS_RESPONSE                         ((sl_status_t)0x1FFBB) ///< Invalid content in the DNS response to the DNS resolution query.
#define SL_STATUS_SI91X_DNS_CLASS_ERROR_IN_DNS_RESPONSE                         ((sl_status_t)0x1FFBA) ///< DNS class error in response to the DNS resolution query.
#define SL_STATUS_SI91X_DNS_COUNT_ERROR_IN_DNS_RESPONSE                         ((sl_status_t)0x1FFB8) ///< DNS count error in response to the DNS resolution query.
#define SL_STATUS_SI91X_DNS_RETURN_CODE_ERROR_IN_DNS_RESPONSE                   ((sl_status_t)0x1FFB7) ///< DNS return code error in the response to the DNS resolution query.
#define SL_STATUS_SI91X_DNS_OPCODE_ERROR_IN_DNS_RESPONSE                        ((sl_status_t)0x1FFB6) ///< DNS Opcode error in the response to the DNS resolution query.
#define SL_STATUS_SI91X_DNS_ID_MISMATCH                                         ((sl_status_t)0x1FFB5) ///< DNS id mismatch between DNS resolution request and response.
#define SL_STATUS_SI91X_INVALID_INPUT_IN_DNS_QUERY                              ((sl_status_t)0x1FFAB) ///< An invalid input to the DNS resolution query.
#define SL_STATUS_SI91X_DNS_RESPONSE_TIMEOUT                                    ((sl_status_t)0x1FF42) ///< DNS response was timed out.
#define SL_STATUS_SI91X_ARP_REQUEST_FAILURE                                     ((sl_status_t)0x1FFA1) ///< ARP request failure.
#define SL_STATUS_SI91X_UNABLE_TO_UPDATE_TCP_WINDOW                             ((sl_status_t)0x1FF91) ///< Unable to update TCP window.
#define SL_STATUS_SI91X_DHCP_LEASE_EXPIRED                                      ((sl_status_t)0x1FF9D) ///< DHCP lease time expired.
#define SL_STATUS_SI91X_DHCP_HANDSHAKE_FAILURE                                  ((sl_status_t)0x1FF9C) ///< DHCP handshake failure.
#define SL_STATUS_SI91X_WEBSOCKET_CREATION_FAILED                               ((sl_status_t)0x1FF88) ///< This error is issued when WebSocket creation failed.
#define SL_STATUS_SI91X_TRYING_TO_CONNECT_NON_EXISTENT_TCP_SERVER_SOCKET        ((sl_status_t)0x1FF87) ///< This error is issued when device tried to connect to a non-existent TCP server socket on the remote side.
#define SL_STATUS_SI91X_TRYING_TO_CLOSE_NON_EXISTENT_SOCKET                     ((sl_status_t)0x1FF86) ///< This error is issued when tried to close non-existent socket. or invalid socket descriptor.
#define SL_STATUS_SI91X_INVALID_SOCKET_PARAMETERS                               ((sl_status_t)0x1FF85) ///< Invalid socket parameters.
#define SL_STATUS_SI91X_FEATURE_NOT_AVAILABLE                                   ((sl_status_t)0x1FF82) ///< Feature not supported.
#define SL_STATUS_SI91X_SOCKET_ALREADY_OPEN                                     ((sl_status_t)0x1FF81) ///< Socket already open.
#define SL_STATUS_SI91X_MAX_SOCKETS_EXCEEDED                                    ((sl_status_t)0x1FF80) ///< Attempt to open more than the maximum allowed number of sockets.
#define SL_STATUS_SI91X_DATA_LENGTH_EXCEEDS_MSS                                 ((sl_status_t)0x1FF7E) ///< Data length exceeds mss.
#define SL_STATUS_SI91X_IP_CONFLICT_ERROR                                       ((sl_status_t)0x1FF75) ///< DUT unable to configure IP address due to IP conflict.
#define SL_STATUS_SI91X_FEATURE_NOT_ENABLED                                     ((sl_status_t)0x1FF74) ///< Feature not enabled.
#define SL_STATUS_SI91X_DHCP_SERVER_NOT_SET                                     ((sl_status_t)0x1FF73) ///< DHCP server not set in AP mode.
#define SL_STATUS_SI91X_AP_SET_REGION_PARAM_ERROR                               ((sl_status_t)0x1FF71) ///< Error in AP set region command parameters.
#define SL_STATUS_SI91X_SSL_TLS_NOT_SUPPORTED                                   ((sl_status_t)0x1FF70) ///< SSL/TLS not supported.
#define SL_STATUS_SI91X_JSON_NOT_SUPPORTED                                      ((sl_status_t)0x1FF6F) ///< JSON not supported.
#define SL_STATUS_SI91X_INVALID_OPERATING_MODE                                  ((sl_status_t)0x1FF6E) ///< Invalid operating mode.
#define SL_STATUS_SI91X_INVALID_SOCKET_CONFIG_PARAMS                            ((sl_status_t)0x1FF6D) ///< Invalid socket configuration parameters.
#define SL_STATUS_SI91X_WEBSOCKET_CREATION_TIMEOUT                              ((sl_status_t)0x1FF6C) ///< Web socket creation timeout.
#define SL_STATUS_SI91X_PARAM_MAX_VALUE_EXCEEDED                                ((sl_status_t)0x1FF6B) ///< Parameter maximum allowed value is exceeded.
#define SL_STATUS_SI91X_SOCKET_READ_TIMEOUT                                     ((sl_status_t)0x1FF6A) ///< Socket read timeout.
#define SL_STATUS_SI91X_INVALID_COMMAND_SEQUENCE                                ((sl_status_t)0x1FF69) ///< Invalid command in sequence.
#define SL_STATUS_SI91X_DNS_RESPONSE_TIMEOUT_ERROR                              ((sl_status_t)0x1FF42) ///< DNS response timed out.
#define SL_STATUS_SI91X_HTTP_SOCKET_CREATION_FAILED                             ((sl_status_t)0x1FF41) ///< HTTP socket creation failed.
#define SL_STATUS_SI91X_TCP_CLOSE_BEFORE_RESPONSE_ERROR                         ((sl_status_t)0x1FF40) ///< TCP socket close command is issued before getting the response of the previous close command.
#define SL_STATUS_SI91X_WAIT_ON_HOST_FEATURE_NOT_ENABLED                        ((sl_status_t)0x1FF36) ///< 'Wait On Host' feature not enabled.
#define SL_STATUS_SI91X_STORE_CONFIG_CHECKSUM_INVALID                           ((sl_status_t)0x1FF35) ///< Store configuration checksum validation failed.
#define SL_STATUS_SI91X_TCP_KEEP_ALIVE_TIMEOUT                                  ((sl_status_t)0x1FF33) ///< TCP keep alive timed out.
#define SL_STATUS_SI91X_TCP_ACK_FAILED_FOR_SYN_ACK                              ((sl_status_t)0x1FF2D) ///< TCP ACK failed for TCP SYN-ACK.
#define SL_STATUS_SI91X_MEMORY_LIMIT_EXCEEDED                                   ((sl_status_t)0x1FF2C) ///< Memory limit exceeded in a given operating mode.
#define SL_STATUS_SI91X_MEMORY_LIMIT_EXCEEDED_DURING_AUTO_JOIN                  ((sl_status_t)0x1FF2A) ///< Memory limit exceeded in an operating mode during auto join/create.
#define SL_STATUS_SI91X_PUF_OPERATION_BLOCKED                                   ((sl_status_t)0x1CC2F) ///< PUF operation is blocked.
#define SL_STATUS_SI91X_PUF_ACTIVATION_CODE_INVALID                             ((sl_status_t)0x1CC31) ///< PUF activation code invalid.
#define SL_STATUS_SI91X_PUF_INPUT_PARAMETERS_INVALID                            ((sl_status_t)0x1CC32) ///< PUF input parameters invalid.
#define SL_STATUS_SI91X_PUF_IN_ERROR_STATE                                      ((sl_status_t)0x1CC33) ///< PUF in error state.
#define SL_STATUS_SI91X_PUF_OPERATION_NOT_ALLOWED                               ((sl_status_t)0x1CC34) ///< PUF operation not allowed.
#define SL_STATUS_SI91X_PUF_OPERATION_FAILED                                    ((sl_status_t)0x1CC35) ///< PUF operation failed.
#define SL_STATUS_SI91X_AUTO_JOIN_IN_PROGRESS                                   ((sl_status_t)0x15A5A) ///< Auto join or user store configuration going on.
#define SL_STATUS_SI91X_RSNIE_FROM_AP_INVALID                                   ((sl_status_t)0x1FFE1) ///< Improper RSNIE from AP to station.
#define SL_STATUS_SI91X_SNTP_MAX_ATTEMPTS_REACHED                               ((sl_status_t)0x1FF5F) ///< Reached maximum SNTP invalid attempts.
#define SL_STATUS_SI91X_FREQUENCY_OFFSET_ZER0                                   ((sl_status_t)0x100FC) ///< Frequency offset sent is zero.
#define SL_STATUS_SI91X_FREQUENCY_OFFSET_OUT_OF_LIMITS                          ((sl_status_t)0x100FB) ///< Frequency offset specified goes beyond upper or lower limits and indicates that frequency offset cannot be changed further.

/** @} */
