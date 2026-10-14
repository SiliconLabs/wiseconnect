/***************************************************************************/ /**
 * @file
 * @brief Azure IOT Application
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

#include <string.h>
#include <stdlib.h>
#include "cmsis_os2.h"
#include "sl_status.h"
#include "sl_net.h"
#include "sl_wifi.h"
#include "sl_wifi_device.h"
#include "sl_net_wifi_types.h"
#include "sl_utility.h"
#include "sl_si91x_driver.h"

#include "sl_board_configuration.h"
#include "errno.h"
#include "socket.h"
#include "sl_net_si91x.h"
#include "sl_net_dns.h"
#include "sl_wifi_callback_framework.h"
#include "sl_si91x_socket.h"
#include "sl_si91x_hmac.h"
#include "sl_si91x_core_utilities.h"
#include "sl_sntp.h"

/* Demo Specific configs. */
#include "demo_config.h"

/* Azure Provisioning/IoT Hub library includes */
#include "azure_iot_hub_client.h"
#include "task.h"

/* Transport interface implementation include header for TLS. */
#include "transport_tls_socket.h"

//! Certificates to be loaded
#ifndef democonfigDEVICE_SYMMETRIC_KEY
#include "mydevkitcertificate.pem.h"
#include "mydevkitkey.pem.h"
#endif
#include "silabs_dgcert_ca.pem.h"

/*-----------------------------------------------------------*/

/* Compile time error for undefined configs. */
#if !defined(democonfigHOSTNAME)
#error "Define the config democonfigHOSTNAME by following the instructions in file demo_config.h."
#endif

#if defined(democonfigDEVICE_SYMMETRIC_KEY) && defined(democonfigCLIENT_CERTIFICATE_PEM)
#error \
  "Please define only one auth democonfigDEVICE_SYMMETRIC_KEY or democonfigCLIENT_CERTIFICATE_PEM in demo_config.h."
#endif

#if !defined(democonfigDEVICE_SYMMETRIC_KEY) && !defined(democonfigCLIENT_CERTIFICATE_PEM)
#error "Please define one auth democonfigDEVICE_SYMMETRIC_KEY or democonfigCLIENT_CERTIFICATE_PEM in demo_config.h."
#endif

/*-----------------------------------------------------------*/
/******************************************************
*                    Constants
******************************************************/

#define CERTIFICATE_INDEX 0
#define LOAD_CERTIFICATE  1

#define DNS_REQ_COUNT 5
#define DNS_TIMEOUT   10
#define RETRY_COUNT   1
// Server port number
#define SERVER_PORT 8883

/**
 * @brief Timeout for receiving CONNACK packet in milliseconds.
 */
#define sampleazureiotCONNACK_RECV_TIMEOUT_MS (10 * 1000U)

/**
 * @brief The Telemetry message published in this example.
 */
#define sampleazureiotMESSAGE "Hello World : %d !"

/**
 * @brief  The content type of the Telemetry message published in this example.
 * @remark Message properties must be url-encoded.
 *         This message property is not required to send telemetry.
 */
#define sampleazureiotMESSAGE_CONTENT_TYPE "text%2Fplain"

/**
 * @brief  The content encoding of the Telemetry message published in this example.
 * @remark Message properties must be url-encoded.
 *         This message property is not required to send telemetry.
 */
#define sampleazureiotMESSAGE_CONTENT_ENCODING "us-ascii"

/**
 * @brief The reported property payload to send to IoT Hub
 */
#define sampleazureiotPROPERTY "{ \"PropertyIterationForCurrentConnection\": \"%d\" }"

/**
 * @brief Time in ticks to wait between each cycle of the demo implemented
 * by prvMQTTDemoTask().
 */
#define sampleazureiotDELAY_BETWEEN_DEMO_ITERATIONS_TICKS (pdMS_TO_TICKS(5000U))

/**
 * @brief Timeout for MQTT_ProcessLoop in milliseconds.
 */
#define sampleazureiotPROCESS_LOOP_TIMEOUT_MS (1000U)

/**
 * @brief Delay (in ticks) between consecutive cycles of MQTT publish operations in a
 * demo iteration.
 *
 * Note that the process loop also has a timeout, so the total time between
 * publishes is the sum of the two delays.
 */
#define sampleazureiotDELAY_BETWEEN_PUBLISHES_TICKS (pdMS_TO_TICKS(5000U))

/**
 * @brief Transport timeout in milliseconds for transport send and receive.
 */
#define sampleazureiotTRANSPORT_SEND_RECV_TIMEOUT_MS (5000U)

/**
 * @brief Transport timeout in milliseconds for transport send and receive.
 */
#define sampleazureiotProvisioning_Registration_TIMEOUT_MS (3 * 1000U)

/**
 * @brief Wait timeout for subscribe to finish.
 */
#define sampleazureiotSUBSCRIBE_TIMEOUT (10 * 1000U)

#define ENABLE_NWP_POWER_SAVE 1

#define NTP_SERVER_NAME       "0.pool.ntp.org"
#define SNTP_RETRY_COUNT      5
#define SNTP_BLOCK_TIMEOUT_MS 30000
#define SNTP_CMD_TIMEOUT_MS   1000

/* Offset in seconds between the NTP epoch (1900-01-01 UTC) and the Unix
 * epoch (1970-01-01 UTC). Used to convert SNTP get_time NTP timestamps
 * to Unix time. */
#define NTP_TO_UNIX_OFFSET 2208988800ULL

/******************************************************
*               Function Declarations
******************************************************/
static void application_start(void *argument);
#if LOAD_CERTIFICATE
sl_status_t load_certificates_in_flash(void);
#endif
sl_status_t create_tls_client(void);
static void azure_iot_mqtt_demo();
int32_t TLS_Socket_Recv(NetworkContext_t *pNetworkContext, void *pBuffer, size_t bytesToRecv);
int32_t TLS_Socket_Send(NetworkContext_t *pNetworkContext, const void *pBuffer, size_t bytesToSend);
uint32_t Crypto_HMAC(const uint8_t *pucKey,
                     uint32_t ulKeyLength,
                     const uint8_t *pucData,
                     uint32_t ulDataLength,
                     uint8_t *pucOutput,
                     uint32_t ulOutputLength,
                     uint32_t *pulBytesCopied);
uint64_t ullGetUnixTime(void);
static sl_status_t sync_unix_time_from_sntp(void);
bool xAzureSample_IsConnectedToInternet();

/******************************************************
*               Variable Definitions
******************************************************/

//! Enumeration for states in application
typedef enum app_state {
  AZURE_MQTT_INIT_STATE,
  AZURE_MQTT_CONNECT_STATE,
  AZURE_MQTT_DISCONNECT,
  AZURE_MQTT_CLEANUP_STATE,
} app_state_t;

volatile app_state_t application_state;

static uint8_t ucPropertyBuffer[80];
static uint8_t ucScratchBuffer[128];

/* The NetworkContext struct. */
struct NetworkContext {
  void *pParams;
};

sl_si91x_hmac_config_t config = { 0 };

static AzureIoTHubClient_t xAzureIoTHubClient;
static uint64_t unix_time_at_sync;
static uint32_t tick_at_sync;

/**
 * @brief Static buffer used to hold MQTT messages being sent and received.
 */
static uint8_t ucMQTTMessageBuffer[democonfigNETWORK_BUFFER_SIZE];

/**
 * @brief Cloud to Device received message count.
 */
int C2DMessageCount;

int client_socket = -1;

const osThreadAttr_t thread_attributes = {
  .name       = "app",
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = 3072,
  .priority   = osPriorityNormal,
  .tz_module  = 0,
};

static const sl_wifi_device_configuration_t client_init_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .region_code = US,
  .boot_config = { .oper_mode = SL_SI91X_CLIENT_MODE,
                   .coex_mode = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map =
                     (SL_WIFI_FEAT_SECURITY_OPEN | SL_WIFI_FEAT_WPS_DISABLE | SL_SI91X_FEAT_ULP_GPIO_BASED_HANDSHAKE),
                   .tcp_ip_feature_bit_map = (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT | SL_SI91X_TCP_IP_FEAT_DNS_CLIENT
                                              | SL_SI91X_TCP_IP_FEAT_SSL | SL_SI91X_TCP_IP_FEAT_SNTP_CLIENT
#ifdef SLI_SI91X_ENABLE_IPV6
                                              | SL_SI91X_TCP_IP_FEAT_DHCPV6_CLIENT | SL_SI91X_TCP_IP_FEAT_IPV6
#endif
                                              | SL_SI91X_TCP_IP_FEAT_ICMP | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID),
                   .custom_feature_bit_map = SL_WIFI_SYSTEM_CUSTOM_FEAT_EXTENSION_VALID,
                   .ext_custom_feature_bit_map =
                     (SL_SI91X_EXT_FEAT_XTAL_CLK | SL_SI91X_EXT_FEAT_UART_SEL_FOR_DEBUG_PRINTS
                      | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0 | SL_WIFI_SYSTEM_EXT_FEAT_LOW_POWER_MODE
                      | MEMORY_CONFIG),
                   .bt_feature_bit_map = 0,
                   .ext_tcp_ip_feature_bit_map =
                     (SL_SI91X_EXT_TCP_IP_WINDOW_SCALING | SL_SI91X_EXT_TCP_IP_TOTAL_SELECTS(1)
                      | SL_SI91X_CONFIG_FEAT_EXTENSION_VALID),
                   .ble_feature_bit_map     = 0,
                   .ble_ext_feature_bit_map = 0,
                   .config_feature_bit_map  = (SL_SI91X_FEAT_SLEEP_GPIO_SEL_BITMAP | SL_WIFI_ENABLE_ENHANCED_MAX_PSP) },
  .ta_pool         = { .tx_ratio_in_buffer_pool = 0, .rx_ratio_in_buffer_pool = 0, .global_ratio_in_buffer_pool = 0 },
  .efuse_data_type = SL_SI91X_EFUSE_MFG_SW_VERSION,
  .nwp_fw_image_number = SL_SI91X_NWP_FW_IMAGE_NUMBER_0
};
/******************************************************
*               Function Definitions
******************************************************/
void app_init(void)
{
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

static void application_start(void *argument)
{
  UNUSED_PARAMETER(argument);
  sl_net_wifi_client_profile_t profile = { 0 };
  sl_ip_address_t ip_address           = { 0 };

  sl_status_t status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &client_init_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Unexpected error while initializing Wi-Fi: 0x%lx\r\n", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Wi-Fi is Initialized\r\n");

#ifdef SLI_SI91X_MCU_INTERFACE
  uint8_t xtal_enable = 1;
  status              = sl_si91x_m4_ta_secure_handshake(SL_SI91X_ENABLE_XTAL, 1, &xtal_enable, 0, NULL);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to bring m4_ta_secure_handshake: 0x%lx\r\n", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "M4-NWP secure handshake is successful\r\n");
#endif

  status = sl_net_up(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Error while connecting to Access point: 0x%lx\r\n", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Connected to Access point\r\n");

  status = sl_net_get_profile(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID, &profile);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to get client profile: 0x%lx\r\n", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Getting client profile is successful\r\n");

  ip_address.type = SL_IPV4;
  memcpy(&ip_address.ip.v4.bytes, &profile.ip.ip.v4.ip_address.bytes, sizeof(sl_ipv4_address_t));
  SL_DEBUG_LOG_V2(INFO, "IP address is ");
  print_sl_ip_address(&ip_address);
  SL_DEBUG_LOG_V2(INFO, "\r\n");

#if LOAD_CERTIFICATE
  status = load_certificates_in_flash();
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, " Error while loading certificates: 0x%lx\r\n", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Loaded certificates\r\n");
#endif

  status = create_tls_client();
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, " Error while creating TLS client: 0x%lx\r\n", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "TLS client created \r\n");

  azure_iot_mqtt_demo();

  if (client_socket >= 0)
    close(client_socket);

  SL_DEBUG_LOG_V2(INFO, "Demo is completed\r\n");
}

#if LOAD_CERTIFICATE
sl_status_t load_certificates_in_flash(void)
{
  sl_status_t status;

  // Load TLS CA Certificate
  status = sl_net_set_credential(SL_NET_TLS_SERVER_CREDENTIAL_ID(CERTIFICATE_INDEX),
                                 SL_NET_SIGNING_CERTIFICATE,
                                 silabs_dgcert_ca,
                                 sizeof(silabs_dgcert_ca) - 1);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Loading TLS CA certificate in to FLASH Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  SL_DEBUG_LOG_V2(INFO, "Loading TLS CA certificate at index %d Successful\r\n", CERTIFICATE_INDEX);

#ifdef democonfigDEVICE_SYMMETRIC_KEY
  // Clear TLS Client certificate
  status = sl_net_delete_credential(SL_NET_TLS_CLIENT_CREDENTIAL_ID(CERTIFICATE_INDEX), SL_NET_CERTIFICATE);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "TLS client certificate location not cleared, Error Code : 0x%lX\r\n", status);
    return status;
  }
  SL_DEBUG_LOG_V2(INFO, "TLS Client certificate at index %d cleared successfully\r\n", CERTIFICATE_INDEX);

  // Clear TLS Client private key
  status = sl_net_delete_credential(SL_NET_TLS_CLIENT_CREDENTIAL_ID(CERTIFICATE_INDEX), SL_NET_PRIVATE_KEY);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "TLS Client private key location not cleared, Error Code : 0x%lX\r\n", status);
    return status;
  }
  SL_DEBUG_LOG_V2(INFO, "TLS Client private key at index %d cleared successfully\r\n", CERTIFICATE_INDEX);
#else
  // Load SSL Client certificate
  status = sl_net_set_credential(SL_NET_TLS_CLIENT_CREDENTIAL_ID(CERTIFICATE_INDEX),
                                 SL_NET_CERTIFICATE,
                                 mydevkitcertificate,
                                 sizeof(mydevkitcertificate) - 1);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Loading TLS certificate in to FLASH failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  SL_DEBUG_LOG_V2(INFO, "Loading TLS Client certificate at index %d Successful\r\n", CERTIFICATE_INDEX);

  // Load SSL Client private key
  status = sl_net_set_credential(SL_NET_TLS_CLIENT_CREDENTIAL_ID(CERTIFICATE_INDEX),
                                 SL_NET_PRIVATE_KEY,
                                 mydevkitkey,
                                 sizeof(mydevkitkey) - 1);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Loading TLS Client private key in to FLASH Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  SL_DEBUG_LOG_V2(INFO, "Loading TLS Client private key at index %d Successful\r\n", CERTIFICATE_INDEX);
#endif
  return SL_STATUS_OK;
}
#endif

sl_status_t create_tls_client(void)
{

  sl_status_t status;

  uint8_t count                     = DNS_REQ_COUNT;
  int socket_return_value           = 0;
  socklen_t socket_length           = sizeof(struct sockaddr_in);
  struct sockaddr_in server_address = { 0 };
  sl_ip_address_t dns_query_rsp     = { 0 };

  // Perform DNS to resolve the ntp server domain name
  do {

    status = sl_net_dns_resolve_hostname_v2((const char *)democonfigHOSTNAME,
                                            DNS_TIMEOUT,
                                            RETRY_COUNT,
                                            SL_NET_DNS_TYPE_IPV4,
                                            &dns_query_rsp);
    if (status == SL_STATUS_OK) {
      break;
    }
    count--;
  } while (count != 0);

  if (status != SL_STATUS_OK) {
    // Return if DNS resolution fails
    return status;
  }

  SL_DEBUG_LOG_V2(INFO, "Azure server port : %d, ip : ", SERVER_PORT);
  print_sl_ip_address(&dns_query_rsp);

  if (client_socket != -1) {
    SL_DEBUG_LOG_V2(INFO, "Close previous client_socket : %d\r\n", client_socket);
    close(client_socket);
  }

  client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (client_socket < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket creation failed with bsd error: %d\r\n", errno);
    return SL_STATUS_FAIL;
  }
  SL_DEBUG_LOG_V2(INFO, "Socket ID : %d\r\n", client_socket);

  socket_return_value = setsockopt(client_socket, SOL_TCP, TCP_ULP, TLS, sizeof(TLS));
  if (socket_return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Set Socket option failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return SL_STATUS_FAIL;
  }

  sl_si91x_time_value timeout = { 0 };
  timeout.tv_usec             = sampleazureiotTRANSPORT_SEND_RECV_TIMEOUT_MS * 1000;
  socket_return_value         = setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
  if (socket_return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Set Socket option failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return SL_STATUS_FAIL;
  }

  server_address.sin_family      = AF_INET;
  server_address.sin_port        = SERVER_PORT;
  server_address.sin_addr.s_addr = dns_query_rsp.ip.v4.value;

  socket_return_value = connect(client_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    SL_DEBUG_LOG_V2(ERROR, "Socket Connect failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return SL_STATUS_FAIL;
  } else {
    SL_DEBUG_LOG_V2(INFO, "Socket : %d connected to TLS server \r\n", client_socket);
#if ENABLE_NWP_POWER_SAVE
    sl_wifi_performance_profile_v2_t performance_profile = { .profile         = ASSOCIATED_POWER_SAVE_LOW_LATENCY,
                                                             .listen_interval = 1000 };

    sl_status_t status = SL_STATUS_OK;
    status             = sl_wifi_set_performance_profile_v2(&performance_profile);
    if (status != SL_STATUS_OK) {
      SL_DEBUG_LOG_V2(ERROR, "Power save configuration Failed, Error Code : 0x%ld\r\n", status);
    }
    SL_DEBUG_LOG_V2(INFO, "Associated Power Save is enabled\r\n");
#endif
  }
  return SL_STATUS_OK;
}

uint32_t Crypto_HMAC(const uint8_t *pucKey,
                     uint32_t ulKeyLength,
                     const uint8_t *pucData,
                     uint32_t ulDataLength,
                     uint8_t *pucOutput,
                     uint32_t ulOutputLength,
                     uint32_t *pulBytesCopied)
{
  config.hmac_mode              = SL_SI91X_HMAC_SHA_256;
  config.msg_length             = ulDataLength;
  config.msg                    = pucData;
  config.key_config.B0.key_size = ulKeyLength;
  config.key_config.B0.key_type = SL_SI91X_TRANSPARENT_KEY;
  config.key_config.B0.key      = (uint8_t *)pucKey;
  sl_status_t status            = sl_si91x_hmac(&config, pucOutput);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "HMAC failed, Error Code : 0x%lX\r\n", status);
    return 1;
  }
  *pulBytesCopied = 32;
  SL_DEBUG_LOG_V2(DEBUG, "ulOutputLength : %ld, pulBytesCopied : %ld\r\n", ulOutputLength, *pulBytesCopied);
  SL_DEBUG_LOG_V2(INFO, "HMAC success\r\n");
  return 0;
}
/*-----------------------------------------------------------*/

/**
 * @brief Unix time.
 *
 * @return Seconds since the Unix epoch (1970-01-01 UTC).
 */
uint64_t ullGetUnixTime(void)
{
  uint32_t elapsed_ticks = osKernelGetTickCount() - tick_at_sync;
  return unix_time_at_sync + (elapsed_ticks / configTICK_RATE_HZ);
}

static sl_status_t sync_unix_time_from_sntp(void)
{
  sl_status_t status               = SL_STATUS_FAIL;
  sl_ip_address_t ntp_ip           = { 0 };
  sl_sntp_client_config_t sntp_cfg = { 0 };
  uint8_t time_buf[64]             = { 0 };
  uint8_t retry_count              = DNS_REQ_COUNT;
  uint64_t epoch                   = 0;

  do {
    status = sl_net_dns_resolve_hostname_v2(NTP_SERVER_NAME, DNS_TIMEOUT, RETRY_COUNT, SL_NET_DNS_TYPE_IPV4, &ntp_ip);
    if (status == SL_STATUS_OK) {
      break;
    }
    retry_count--;
  } while (retry_count != 0);

  if (status != SL_STATUS_OK) {
    return status;
  }

  sntp_cfg.server_host_name = ntp_ip.ip.v4.bytes;
  sntp_cfg.sntp_method      = SL_SNTP_UNICAST_MODE;
  sntp_cfg.sntp_timeout     = SNTP_CMD_TIMEOUT_MS;
  sntp_cfg.flags            = 0;
  sntp_cfg.event_handler    = NULL;

  status = sl_sntp_client_start(&sntp_cfg, SNTP_BLOCK_TIMEOUT_MS);
  if (status != SL_STATUS_OK) {
    return status;
  }

  retry_count = SNTP_RETRY_COUNT;
  do {
    status = sl_sntp_client_get_time(time_buf, sizeof(time_buf) - 1, SNTP_BLOCK_TIMEOUT_MS);
    if (status == SL_STATUS_OK) {
      break;
    }
    retry_count--;
  } while (retry_count != 0);

  if (status != SL_STATUS_OK) {
    sl_sntp_client_stop(SNTP_BLOCK_TIMEOUT_MS);
    return status;
  }

  status = sl_sntp_client_stop(SNTP_BLOCK_TIMEOUT_MS);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "SNTP client stop failed: 0x%lx\r\n", status);
  }

  SL_DEBUG_LOG_V2(DEBUG, "SNTP raw get_time: '%s'\r\n", (uintptr_t)time_buf);

  /*
   * sl_sntp_client_get_time() returns the NTP timestamp as a string
   * (for example, "Time: 3996200822. sec."), not a binary integer.
   * Advance to the first digit before converting with strtoull().
   */
  {
    const char *p = (const char *)time_buf;
    while ((*p != '\0') && ((*p < '0') || (*p > '9'))) {
      p++;
    }
    epoch = strtoull(p, NULL, 10);
  }

  /* Ensure the parsed NTP timestamp is large enough for a valid Unix conversion. */
  if (epoch < NTP_TO_UNIX_OFFSET) {
    SL_DEBUG_LOG_V2(ERROR, "Invalid NTP timestamp: %lu\r\n", (unsigned long)epoch);
    return SL_STATUS_FAIL;
  }

  /* Convert NTP timestamp to Unix time. */
  epoch -= NTP_TO_UNIX_OFFSET;

  unix_time_at_sync = epoch;
  tick_at_sync      = osKernelGetTickCount();

  return SL_STATUS_OK;
}

/*-----------------------------------------------------------*/
/**
 * @brief Cloud message callback handler
 */
static void prvHandleCloudMessage(AzureIoTHubClientCloudToDeviceMessageRequest_t *pxMessage, void *pvContext)
{
  (void)pvContext;
  (void)pxMessage;

  C2DMessageCount++;
  printf("Cloud message payload : %.*s , C2DMessageCount : %d\r\n",
         (int)pxMessage->ulPayloadLength,
         (const char *)pxMessage->pvMessagePayload,
         C2DMessageCount);
}
/*-----------------------------------------------------------*/

/**
 * @brief Command message callback handler
 */
static void prvHandleCommand(AzureIoTHubClientCommandRequest_t *pxMessage, void *pvContext)
{
  printf("Command payload : %.*s \r\n", (int)pxMessage->ulPayloadLength, (const char *)pxMessage->pvMessagePayload);

  AzureIoTHubClient_t *xHandle = (AzureIoTHubClient_t *)pvContext;

  if (AzureIoTHubClient_SendCommandResponse(xHandle, pxMessage, 200, NULL, 0) != eAzureIoTSuccess) {
    SL_DEBUG_LOG_V2(ERROR, "Error sending command response");
  }
}
/*-----------------------------------------------------------*/

/**
 * @brief Property message callback handler
 */
static void prvHandlePropertiesMessage(AzureIoTHubClientPropertiesResponse_t *pxMessage, void *pvContext)
{
  (void)pvContext;

  switch (pxMessage->xMessageType) {
    case eAzureIoTHubPropertiesRequestedMessage:
      SL_DEBUG_LOG_V2(INFO, "Device property document GET received");
      break;

    case eAzureIoTHubPropertiesReportedResponseMessage:
      SL_DEBUG_LOG_V2(INFO, "Device property reported property response received");
      break;

    case eAzureIoTHubPropertiesWritablePropertyMessage:
      SL_DEBUG_LOG_V2(INFO, "Device property desired property received");
      break;

    default:
      SL_DEBUG_LOG_V2(WARN, "Unknown property message");
  }

  printf("Property document payload : %.*s \r\n",
         (int)pxMessage->ulPayloadLength,
         (const char *)pxMessage->pvMessagePayload);
}
/*-----------------------------------------------------------*/

bool xAzureSample_IsConnectedToInternet()
{
  return true;
}

int32_t TLS_Socket_Recv(NetworkContext_t *pNetworkContext, void *pBuffer, size_t bytesToRecv)
{
  (void)pNetworkContext;
  int32_t recv_bytes = recv(client_socket, pBuffer, bytesToRecv, 0);
  if (recv_bytes < 0) {
    if (SL_STATUS_SI91X_SOCKET_READ_TIMEOUT == sl_wifi_get_saved_firmware_status()) {
      //printf("Receive timed out\r\n");
      return 0;
    }
  }
  SL_DEBUG_LOG_V2(DEBUG, " %s: bytes recv : %ld\r\n", (uintptr_t) __func__, recv_bytes);
  return recv_bytes;
}

int32_t TLS_Socket_Send(NetworkContext_t *pNetworkContext, const void *pBuffer, size_t bytesToSend)
{
  (void)pNetworkContext;
  int32_t sent_bytes = send(client_socket, pBuffer, bytesToSend, 0);
  SL_DEBUG_LOG_V2(DEBUG, " %s: bytes sent : %ld\r\n", (uintptr_t) __func__, sent_bytes);
  return sent_bytes;
}
/**
 * @brief Azure IoT demo task that gets started in the platform specific project.
 *  In this demo task, middleware API's are used to connect to Azure IoT Hub.
 */
static void azure_iot_mqtt_demo()
{
  int lPublishCount              = 0;
  uint32_t ulScratchBufferLength = 0U;
  AzureIoTTransportInterface_t xTransport;
  NetworkContext_t xNetworkContext         = { 0 };
  TlsTransportParams_t xTlsTransportParams = { 0 };
  AzureIoTResult_t xResult;
  AzureIoTHubClientOptions_t xHubOptions = { 0 };
  AzureIoTMessageProperties_t xPropertyBag;
  bool xSessionPresent;
  uint8_t *pucIotHubHostname       = (uint8_t *)democonfigHOSTNAME;
  uint8_t *pucIotHubDeviceId       = (uint8_t *)democonfigDEVICE_ID;
  uint32_t pulIothubHostnameLength = sizeof(democonfigHOSTNAME) - 1;
  uint32_t pulIothubDeviceIdLength = sizeof(democonfigDEVICE_ID) - 1;

  xNetworkContext.pParams = &xTlsTransportParams;

  /* Attempt to establish TLS session with IoT Hub. */

  /* Fill in Transport Interface send and receive function pointers. */
  xTransport.pxNetworkContext = &xNetworkContext;
  xTransport.xSend            = TLS_Socket_Send;
  xTransport.xRecv            = TLS_Socket_Recv;

  /* Init IoT Hub option */
  xResult = AzureIoTHubClient_OptionsInit(&xHubOptions);
  SL_DEBUG_LOG_V2(DEBUG, "AzureIoTHubClient_OptionsInit: %x\r\n", xResult);
  assert(xResult == eAzureIoTSuccess);

  xHubOptions.pucModuleID      = (const uint8_t *)democonfigMODULE_ID;
  xHubOptions.ulModuleIDLength = sizeof(democonfigMODULE_ID) - 1;

  application_state = AZURE_MQTT_INIT_STATE;

  while (true) {
    switch (application_state) {
      case AZURE_MQTT_INIT_STATE: {

        xResult = AzureIoTHubClient_Init(&xAzureIoTHubClient,
                                         pucIotHubHostname,
                                         pulIothubHostnameLength,
                                         pucIotHubDeviceId,
                                         pulIothubDeviceIdLength,
                                         &xHubOptions,
                                         ucMQTTMessageBuffer,
                                         sizeof(ucMQTTMessageBuffer),
                                         ullGetUnixTime,
                                         &xTransport);
        SL_DEBUG_LOG_V2(DEBUG, "AzureIoTHubClient_Init: %x\r\n", xResult);
        assert(xResult == eAzureIoTSuccess);

#ifdef democonfigDEVICE_SYMMETRIC_KEY
        xResult = AzureIoTHubClient_SetSymmetricKey(&xAzureIoTHubClient,
                                                    (const uint8_t *)democonfigDEVICE_SYMMETRIC_KEY,
                                                    sizeof(democonfigDEVICE_SYMMETRIC_KEY) - 1,
                                                    Crypto_HMAC);
        SL_DEBUG_LOG_V2(DEBUG, "AzureIoTHubClient_SetSymmetricKey: %x\r\n", xResult);
        assert(xResult == eAzureIoTSuccess);
#endif /* democonfigDEVICE_SYMMETRIC_KEY */

        /* Refresh wall clock immediately before SAS generation (Connect). */
        {
          sl_status_t sntp_status = sync_unix_time_from_sntp();
          if (sntp_status != SL_STATUS_OK) {
            SL_DEBUG_LOG_V2(ERROR, "SNTP refresh before MQTT connect failed: 0x%lx\r\n", sntp_status);
            application_state = AZURE_MQTT_DISCONNECT;
            break;
          }
        }

        /* Sends an MQTT Connect packet over the already established TLS connection,
             * and waits for connection acknowledgment (CONNACK) packet. */
        SL_DEBUG_LOG_V2(INFO, "Creating an MQTT connection to %s.\r\n", (uintptr_t)pucIotHubHostname);

        xResult = AzureIoTHubClient_Connect(&xAzureIoTHubClient,
                                            false,
                                            &xSessionPresent,
                                            sampleazureiotCONNACK_RECV_TIMEOUT_MS);
        if (xResult != eAzureIoTSuccess) {
          application_state = AZURE_MQTT_DISCONNECT;
          break;
        }
        application_state = AZURE_MQTT_CONNECT_STATE;
      } break;

      case AZURE_MQTT_CONNECT_STATE: {
        xResult = AzureIoTHubClient_SubscribeCloudToDeviceMessage(&xAzureIoTHubClient,
                                                                  prvHandleCloudMessage,
                                                                  &xAzureIoTHubClient,
                                                                  sampleazureiotSUBSCRIBE_TIMEOUT);
        assert(xResult == eAzureIoTSuccess);

        xResult = AzureIoTHubClient_SubscribeCommand(&xAzureIoTHubClient,
                                                     prvHandleCommand,
                                                     &xAzureIoTHubClient,
                                                     sampleazureiotSUBSCRIBE_TIMEOUT);
        assert(xResult == eAzureIoTSuccess);

        xResult = AzureIoTHubClient_SubscribeProperties(&xAzureIoTHubClient,
                                                        prvHandlePropertiesMessage,
                                                        &xAzureIoTHubClient,
                                                        sampleazureiotSUBSCRIBE_TIMEOUT);
        assert(xResult == eAzureIoTSuccess);

        /* Get property document after initial connection */
        xResult = AzureIoTHubClient_RequestPropertiesAsync(&xAzureIoTHubClient);
        assert(xResult == eAzureIoTSuccess);

        /* Create a bag of properties for the telemetry */
        xResult = AzureIoTMessage_PropertiesInit(&xPropertyBag, ucPropertyBuffer, 0, sizeof(ucPropertyBuffer));
        assert(xResult == eAzureIoTSuccess);

        /* Sending a default property (Content-Type). */
        xResult = AzureIoTMessage_PropertiesAppend(&xPropertyBag,
                                                   (uint8_t *)AZ_IOT_MESSAGE_PROPERTIES_CONTENT_TYPE,
                                                   sizeof(AZ_IOT_MESSAGE_PROPERTIES_CONTENT_TYPE) - 1,
                                                   (uint8_t *)sampleazureiotMESSAGE_CONTENT_TYPE,
                                                   sizeof(sampleazureiotMESSAGE_CONTENT_TYPE) - 1);
        assert(xResult == eAzureIoTSuccess);

        /* Sending a default property (Content-Encoding). */
        xResult = AzureIoTMessage_PropertiesAppend(&xPropertyBag,
                                                   (uint8_t *)AZ_IOT_MESSAGE_PROPERTIES_CONTENT_ENCODING,
                                                   sizeof(AZ_IOT_MESSAGE_PROPERTIES_CONTENT_ENCODING) - 1,
                                                   (uint8_t *)sampleazureiotMESSAGE_CONTENT_ENCODING,
                                                   sizeof(sampleazureiotMESSAGE_CONTENT_ENCODING) - 1);
        assert(xResult == eAzureIoTSuccess);

        /* How to send an user-defined custom property. */
        xResult = AzureIoTMessage_PropertiesAppend(&xPropertyBag,
                                                   (uint8_t *)"name",
                                                   sizeof("name") - 1,
                                                   (uint8_t *)"value",
                                                   sizeof("value") - 1);
        assert(xResult == eAzureIoTSuccess);

        /* Publish messages with QoS1, send and process Keep alive messages. */
        while (true) {
          lPublishCount++;
          SL_DEBUG_LOG_V2(INFO, "Attempt to receive publish message from Cloud to IoT Hub.\r\n");
          xResult = AzureIoTHubClient_ProcessLoop(&xAzureIoTHubClient, sampleazureiotPROCESS_LOOP_TIMEOUT_MS);
          if (xResult != eAzureIoTSuccess) {
            // check if socket is closed and Retry socket connection.
            application_state = AZURE_MQTT_DISCONNECT;
            break;
          }

          ulScratchBufferLength =
            snprintf((char *)ucScratchBuffer, sizeof(ucScratchBuffer), sampleazureiotMESSAGE, lPublishCount);
          xResult = AzureIoTHubClient_SendTelemetry(&xAzureIoTHubClient,
                                                    ucScratchBuffer,
                                                    ulScratchBufferLength,
                                                    &xPropertyBag,
                                                    eAzureIoTHubMessageQoS1,
                                                    NULL);
          if (xResult != eAzureIoTSuccess) {
            // check if socket is closed and Retry socket connection.
            application_state = AZURE_MQTT_DISCONNECT;
            break;
          }

          SL_DEBUG_LOG_V2(INFO, "Attempt to send publish message : %s, from IoT Hub.\r\n", (uintptr_t)ucScratchBuffer);

          if (lPublishCount % 2 == 0) {
            /* Send reported property every other cycle */
            ulScratchBufferLength =
              snprintf((char *)ucScratchBuffer, sizeof(ucScratchBuffer), sampleazureiotPROPERTY, lPublishCount / 2 + 1);
            xResult = AzureIoTHubClient_SendPropertiesReported(&xAzureIoTHubClient,
                                                               ucScratchBuffer,
                                                               ulScratchBufferLength,
                                                               NULL);
            assert(xResult == eAzureIoTSuccess);
          }

          /* Leave Connection Idle for some time. */
          SL_DEBUG_LOG_V2(INFO, "Keeping Connection Idle..., lPublishCount : %d\r\n", lPublishCount);

          osDelay(sampleazureiotDELAY_BETWEEN_PUBLISHES_TICKS);
        }
      } break;

      case AZURE_MQTT_DISCONNECT: {
        // if TLS or MQTT connection is closed, retry connection.
        sl_status_t status = create_tls_client();
        if (status != SL_STATUS_OK) {
          SL_DEBUG_LOG_V2(ERROR, " Error while creating TLS client: 0x%lx\r\n", status);
        }
        application_state = AZURE_MQTT_INIT_STATE;
      } break;

      case AZURE_MQTT_CLEANUP_STATE: {
        if (xAzureSample_IsConnectedToInternet()) {
          xResult = AzureIoTHubClient_UnsubscribeProperties(&xAzureIoTHubClient);
          assert(xResult == eAzureIoTSuccess);

          xResult = AzureIoTHubClient_UnsubscribeCommand(&xAzureIoTHubClient);
          assert(xResult == eAzureIoTSuccess);

          xResult = AzureIoTHubClient_UnsubscribeCloudToDeviceMessage(&xAzureIoTHubClient);
          assert(xResult == eAzureIoTSuccess);

          /* Send an MQTT Disconnect packet over the already connected TLS over
                 * TCP connection. There is no corresponding response for the disconnect
                 * packet. After sending disconnect, client must close the network
                 * connection. */
          xResult = AzureIoTHubClient_Disconnect(&xAzureIoTHubClient);

          assert(xResult == eAzureIoTSuccess);
        }

        /* Wait for some time between two iterations to ensure that we do not
             * bombard the IoT Hub. */
        SL_DEBUG_LOG_V2(INFO, "Demo completed successfully.\r\n");
      } break;
      default: {
        SL_DEBUG_LOG_V2(ERROR, "Invalid application state. %d\r\n", application_state);
      } break;
    }
  }
}
