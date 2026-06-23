/***************************************************************************/ /**
 * @file
 * @brief  Twt Tcp Client Application
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include "errno.h"
#include "sl_wifi_callback_framework.h"
#include "sl_status.h"
#include "sl_board_configuration.h"
#include "cmsis_os2.h"
#include "sl_wifi.h"
#include "socket.h"
#include "sl_utility.h"
#include "sl_net.h"
#include "sl_ip_types.h"
#include "sl_string.h"
#include "sl_net_si91x.h"

#ifdef SLI_SI91X_MCU_INTERFACE
#include "sl_si91x_power_manager.h"
#include "sl_si91x_m4_ps.h"
#endif
/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define SERVER_IP         "192.168.50.32"
#define SERVER_PORT       5001
#define NUMBER_OF_PACKETS 1000
#define DATA              "hello from tcp client"
#define TWT_SCAN_TIMEOUT  10000
#define SEND_TCP_DATA     0
#define TWT_AUTO_CONFIG   1

/* Defaults migrated from sl_wifi_filter_broadcast(5000, 1, 1). */
#define BCAST_FILTER_ENABLE      (1U)
#define MCAST_FILTER_ENABLE      (1U)
#define FILTER_MODE              (0U)
#define BEACON_DROP_THRESHOLD_MS (5000U)

// Use case based TWT selection params
#define TWT_RX_LATENCY      60000 // in milli seconds
#define TCP_KEEP_ALIVE_TIME 240

static const sl_wifi_device_configuration_t twt_client_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .boot_config = { .oper_mode = SL_SI91X_CLIENT_MODE,
                   .coex_mode = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map =
                     (SL_WIFI_FEAT_SECURITY_OPEN | SL_WIFI_FEAT_AGGREGATION | SL_SI91X_FEAT_ULP_GPIO_BASED_HANDSHAKE
#ifdef SLI_SI91X_MCU_INTERFACE
                      | SL_WIFI_FEAT_WPS_DISABLE
#endif
                      ),
                   .tcp_ip_feature_bit_map =
                     (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID),
                   .custom_feature_bit_map     = (SL_WIFI_SYSTEM_CUSTOM_FEAT_EXTENSION_VALID),
                   .ext_custom_feature_bit_map = (SL_WIFI_SYSTEM_EXT_FEAT_LOW_POWER_MODE | SL_SI91X_EXT_FEAT_XTAL_CLK
                                                  | SL_SI91X_EXT_FEAT_DISABLE_DEBUG_PRINTS | MEMORY_CONFIG
#ifdef SLI_SI917
                                                  | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
                                                  ),
                   .bt_feature_bit_map         = 0,
                   .ext_tcp_ip_feature_bit_map = SL_SI91X_CONFIG_FEAT_EXTENSION_VALID,
                   .ble_feature_bit_map        = 0,
                   .ble_ext_feature_bit_map    = 0,
                   .config_feature_bit_map = (SL_SI91X_FEAT_SLEEP_GPIO_SEL_BITMAP | SL_WIFI_ENABLE_ENHANCED_MAX_PSP) }
};

/******************************************************
 *               Variable Definitions
 ******************************************************/
const osThreadAttr_t thread_attributes = {
  .name       = "application_thread",
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = 3072,
  .priority   = 0,
  .tz_module  = 0,
};

sl_wifi_twt_request_t default_twt_setup_configuration = {
  .twt_enable              = 1,
  .twt_flow_id             = 1,
  .wake_duration           = 0x60,
  .wake_duration_unit      = 0,
  .wake_duration_tol       = 0x60,
  .wake_int_exp            = 13,
  .wake_int_exp_tol        = 13,
  .wake_int_mantissa       = 0x1D4C,
  .wake_int_mantissa_tol   = 0x1D4C,
  .implicit_twt            = 1,
  .un_announced_twt        = 1,
  .triggered_twt           = 0,
  .twt_channel             = 0,
  .twt_protection          = 0,
  .restrict_tx_outside_tsp = 1,
  .twt_retry_limit         = 6,
  .twt_retry_interval      = 10,
  .req_type                = 1,
  .negotiation_type        = 0,
};

sl_wifi_twt_selection_v2_t default_twt_config = {
  .twt_enable            = 1,
  .average_tx_throughput = 0,
  .tx_latency            = 0,
  .rx_latency            = TWT_RX_LATENCY,
};

volatile sl_status_t callback_status = SL_STATUS_OK;
int client_socket                    = -1;
/******************************************************
  *               Function Declarations
  ******************************************************/
void application_start();
sl_status_t set_twt(void);
sl_status_t send_data(void);
static sl_status_t twt_callback_handler(sl_wifi_event_t event,
                                        sl_status_t status_code,
                                        sl_wifi_twt_response_t *result,
                                        uint32_t result_length,
                                        void *arg);

/******************************************************
  *               Function Definitions
  ******************************************************/

void app_init(void)
{
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

void application_start()
{
  sl_status_t status;
  int return_value                  = 0;
  sl_ipv4_address_t ip              = { 0 };
  struct sockaddr_in server_address = { 0 };
  uint16_t tcp_keepalive_time       = TCP_KEEP_ALIVE_TIME;

  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &twt_client_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to start Wi-Fi client interface: 0x%" PRIx32 "", (uint32_t)status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Wi-Fi Init Done\r\n");

  status = sl_net_up(SL_NET_WIFI_CLIENT_INTERFACE, 0);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to bring Wi-Fi client interface up: 0x%" PRIx32 "", (uint32_t)status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Wi-Fi Client Connected\r\n");

  sl_net_inet_addr(SERVER_IP, (uint32_t *)&ip);

  server_address.sin_family      = AF_INET;
  server_address.sin_port        = SERVER_PORT;
  server_address.sin_addr.s_addr = ip.value;

  //! Create socket
  client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  SL_DEBUG_LOG_V2(INFO, " Client Socket: %d\r\n", client_socket);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket Create failed with bsd error: %d\r\n", errno);
    return;
  }

  //! Configure TCP keep alive timeout
  return_value = setsockopt(client_socket, SOL_SOCKET, SO_KEEPALIVE, &tcp_keepalive_time, sizeof(tcp_keepalive_time));
  if (return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "setsockopt tcp_keepalive_time failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }

  //! Socket connect
  return_value = connect(client_socket, (struct sockaddr *)&server_address, sizeof(struct sockaddr_in));
  if (return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket Connect failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, " Socket Connected\r\n");

  status = set_twt();
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Error while configuring TWT parameters: 0x%" PRIx32 " ", (uint32_t)status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "TWT Config Done\r\n");

#if SEND_TCP_DATA
  status = send_data();
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Error while sending data: 0x%" PRIx32 " ", (uint32_t)status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Data Sent\r\n");
#endif

#ifdef SLI_SI91X_MCU_INTERFACE
  osSemaphoreId_t wait_semaphore;
  wait_semaphore = osSemaphoreNew(1, 0, NULL);
  if (wait_semaphore == NULL) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to create semaphore\r\n");
    return;
  }
  // Waiting forever using semaphore to put M4 to sleep in tick less mode
  osSemaphoreAcquire(wait_semaphore, osWaitForever);
#endif
}

sl_status_t set_twt(void)
{
  sl_wifi_performance_profile_v2_t performance_profile = { 0 };
  sl_status_t status                                   = SL_STATUS_OK;

  //! Set TWT Config
  sl_wifi_set_twt_config_callback_v2(twt_callback_handler, NULL);
  if (TWT_AUTO_CONFIG == 1) {
    status = sl_wifi_target_wake_time_auto_selection_v2(&default_twt_config);
  } else {
    performance_profile.twt_request = default_twt_setup_configuration;
    status                          = sl_wifi_enable_target_wake_time(&performance_profile.twt_request);
  }
  VERIFY_STATUS_AND_RETURN(status);
  // A small delay is added so that the asynchronous response from TWT is printed in correct format.
  osDelay(100);

  //! Enable groupcast filter and beacon drop threshold
  sl_wifi_groupcast_filter_config_t groupcast_filter_config = { 0 };
  groupcast_filter_config.enable_bcast_filter               = (uint8_t)BCAST_FILTER_ENABLE;
  groupcast_filter_config.enable_mcast_filter               = (uint8_t)MCAST_FILTER_ENABLE;
  groupcast_filter_config.filter_mode                       = (uint8_t)FILTER_MODE;

  status = sl_wifi_set_groupcast_filter_config(&groupcast_filter_config);
  VERIFY_STATUS_AND_RETURN(status);
  status = sl_wifi_set_beacon_drop_threshold(SL_WIFI_CLIENT_INTERFACE, (uint16_t)BEACON_DROP_THRESHOLD_MS);
  VERIFY_STATUS_AND_RETURN(status);
  SL_DEBUG_LOG_V2(INFO, "Enabled Broadcast Data Filter\r\n");

  //! Apply power save profile
  performance_profile.profile = ASSOCIATED_POWER_SAVE_LOW_LATENCY;
  status                      = sl_wifi_set_performance_profile_v2(&performance_profile);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Powersave Configuration Failed, Error Code : 0x%" PRIx32 "", (uint32_t)status);
    return status;
  }
  SL_DEBUG_LOG_V2(INFO, "Associated Power Save Enabled\r\n");
  return SL_STATUS_OK;
}

#if SEND_TCP_DATA
sl_status_t send_data(void)
{
  //! send data
  int sent_bytes   = 0;
  int packet_count = 0;
  while (packet_count < NUMBER_OF_PACKETS) {
    sent_bytes = send(client_socket, DATA, strlen(DATA), 0);
    if (sent_bytes < 0) {
      if (errno == ENOBUFS)
        continue;
      PRINT_STATUS(ERROR_TAG, SL_STATUS_FAIL);
      close(client_socket);
      return SL_STATUS_FAIL;
    }
    packet_count++;
  }
  SL_DEBUG_LOG_V2(INFO, " Data sent successfully\r\n");

  return SL_STATUS_OK;
}
#endif

static sl_status_t twt_callback_handler(sl_wifi_event_t event,
                                        sl_status_t status_code,
                                        sl_wifi_twt_response_t *result,
                                        uint32_t result_length,
                                        void *arg)
{
  UNUSED_PARAMETER(result_length);
  UNUSED_PARAMETER(arg);

  if (SL_WIFI_CHECK_IF_EVENT_FAILED(event)) {
    return status_code;
  }

  switch (event) {
    case SL_WIFI_TWT_RESPONSE_EVENT:
      SL_DEBUG_LOG_V2(INFO, "TWT Setup success");
      break;
    case SL_WIFI_TWT_UNSOLICITED_SESSION_SUCCESS_EVENT:
      SL_DEBUG_LOG_V2(INFO, "Unsolicited TWT Setup success");
      break;
    case SL_WIFI_TWT_AP_REJECTED_EVENT:
      SL_DEBUG_LOG_V2(ERROR, "TWT Setup Failed. TWT Setup rejected by AP");
      break;
    case SL_WIFI_TWT_OUT_OF_TOLERANCE_EVENT:
      SL_DEBUG_LOG_V2(ERROR, "TWT Setup Failed. TWT response out of tolerance limits");
      break;
    case SL_WIFI_TWT_RESPONSE_NOT_MATCHED_EVENT:
      SL_DEBUG_LOG_V2(ERROR, "TWT Setup Failed. TWT Response not matched with the request parameters");
      break;
    case SL_WIFI_TWT_UNSUPPORTED_RESPONSE_EVENT:
      SL_DEBUG_LOG_V2(ERROR, "TWT Setup Failed. TWT Response Unsupported");
      break;
    case SL_WIFI_TWT_FAIL_MAX_RETRIES_REACHED_EVENT:
      SL_DEBUG_LOG_V2(ERROR, "TWT Setup Failed. Max retries reached");
      break;
    case SL_WIFI_TWT_INACTIVE_DUE_TO_ROAMING_EVENT:
      SL_DEBUG_LOG_V2(WARN, "TWT session inactive due to roaming");
      break;
    case SL_WIFI_TWT_INACTIVE_DUE_TO_DISCONNECT_EVENT:
      SL_DEBUG_LOG_V2(WARN, "TWT session inactive due to wlan disconnection");
      break;
    case SL_WIFI_TWT_TEARDOWN_SUCCESS_EVENT:
      SL_DEBUG_LOG_V2(INFO, "TWT session teardown success");
      break;
    case SL_WIFI_TWT_AP_TEARDOWN_SUCCESS_EVENT:
      SL_DEBUG_LOG_V2(INFO, "TWT session teardown from AP");
      break;
    case SL_WIFI_TWT_INACTIVE_NO_AP_SUPPORT_EVENT:
      SL_DEBUG_LOG_V2(WARN, "Connected AP Does not support TWT");
      break;
    case SL_WIFI_RESCHEDULE_TWT_SUCCESS_EVENT:
      SL_DEBUG_LOG_V2(INFO, "TWT rescheduled");
      break;
    case SL_WIFI_TWT_INFO_FRAME_EXCHANGE_FAILED_EVENT:
      SL_DEBUG_LOG_V2(ERROR, "TWT rescheduling failed due to a failure in the exchange of TWT information frames.");
      break;
    default:
      SL_DEBUG_LOG_V2(ERROR, "TWT Setup Failed.");
  }
  if (event < SL_WIFI_TWT_TEARDOWN_SUCCESS_EVENT) {
    SL_DEBUG_LOG_V2(DEBUG, " wake duration : 0x%X", result->wake_duration);
    SL_DEBUG_LOG_V2(DEBUG, " wake_duration_unit: 0x%X", result->wake_duration_unit);
    SL_DEBUG_LOG_V2(DEBUG, " wake_int_exp : 0x%X", result->wake_int_exp);
    SL_DEBUG_LOG_V2(DEBUG, " negotiation_type : 0x%X\r\n", result->negotiation_type);
    SL_DEBUG_LOG_V2(DEBUG, " wake_int_mantissa : 0x%X", result->wake_int_mantissa);
    SL_DEBUG_LOG_V2(DEBUG, " implicit_twt : 0x%X", result->implicit_twt);
    SL_DEBUG_LOG_V2(DEBUG, " un_announced_twt : 0x%X", result->un_announced_twt);
    SL_DEBUG_LOG_V2(DEBUG, " triggered_twt : 0x%X", result->triggered_twt);
    SL_DEBUG_LOG_V2(DEBUG, " twt_channel : 0x%X", result->twt_channel);
    SL_DEBUG_LOG_V2(DEBUG, " twt_protection : 0x%X", result->twt_protection);
    SL_DEBUG_LOG_V2(DEBUG, " twt_flow_id : 0x%X\r\n", result->twt_flow_id);
  } else if (event < SL_WIFI_TWT_EVENTS_END) {
    SL_DEBUG_LOG_V2(DEBUG, " twt_flow_id : 0x%X\r\n", result->twt_flow_id);
    SL_DEBUG_LOG_V2(DEBUG, " negotiation_type : 0x%X\r\n", result->negotiation_type);
  }
  return SL_STATUS_OK;
}
