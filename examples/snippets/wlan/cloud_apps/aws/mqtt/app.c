/***************************************************************************/ /**
 * @file
 * @brief AWS MQTT Application
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
#include "cmsis_os2.h"
#include "sl_status.h"
#include "sl_net.h"
#include "sl_wifi.h"
#include "sl_wifi_device.h"
#include "sl_net_wifi_types.h"
#include "sl_utility.h"
#include "sl_tls.h"
#include "sl_si91x_driver.h"

#include "sl_board_configuration.h"
#include "errno.h"
#include "socket.h"
#include "sl_net_si91x.h"
#include "sl_wifi_callback_framework.h"
#include "sl_si91x_socket.h"

#ifdef RSI_M4_INTERFACE
#include "rsi_rtc.h"
#include "rsi_chip.h"
#include "rsi_wisemcu_hardware_setup.h"
#endif

//! Certificates to be loaded
#include "aws_client_certificate.pem.crt.h"
#include "aws_client_private_key.pem.key.h"
#include "aws_starfield_ca.pem.h"

//! AWS files
#include "aws_iot_error.h"
#include "aws_iot_config.h"
#include "aws_iot_shadow_interface.h"

/******************************************************
*                    Constants
******************************************************/

#define SUBSCRIBE_TO_TOPIC   "aws_status_555"     //! Subscribe Topic to receive the message from cloud
#define PUBLISH_ON_TOPIC     "siwx91x_status_555" //! Publish Topic to send the status from application to cloud
#define MQTT_PUBLISH_PAYLOAD "Hi from SiWx91x"    //! Publish message
#define SUBSCRIBE_QOS        QOS1                 //! Quality of Service for subscribed topic "SUBSCRIBE_TO_TOPIC"
#define PUBLISH_QOS          QOS1                 //! Quality of Service for publish topic "PUBLISH_ON_TOPIC"
#define PUBLISH_PERIODICITY  30000                //! Publish periodicity in milliseconds
#define MQTT_USERNAME        "username"
#define MQTT_PASSWORD        "password"

#define ENABLE_POWER_SAVE 1
#ifdef RSI_M4_INTERFACE
#define ALARM_TIMER_BASED_WAKEUP 0 //!Enable this macro for M4 processor to wake up based on alarm time period
#define BUTTON_BASED_WAKEUP      1 //!Enable this macro for M4 processor to wake up based on button (BTN0) press
#endif

#if ENABLE_POWER_SAVE
volatile uint8_t powersave_given = 0;
#endif

#if (defined(RSI_M4_INTERFACE) && ENABLE_POWER_SAVE)
#define WIRELESS_WAKEUP_IRQHandler NPSS_TO_MCU_WIRELESS_INTR_IRQn

#define ALARM_PERIODIC_TIME 30 /*<! periodic alarm configuration in SEC */

#define RTC_ALARM_INTR         BIT(16)
#define NPSS_GPIO_2            2
#define NPSSGPIO_PIN_MUX_MODE2 2
#define NPSS_GPIO_INTR_LOW     0
#define NPSS_GPIO_DIR_INPUT    1
#define NPSS_GPIO_2_INTR       BIT(2)

/*Update time configurations for next boundary alarm*/
#define RC_TRIGGER_TIME           5
#define RO_TRIGGER_TIME           0
#define NO_OF_HOURS_IN_A_DAY      24
#define NO_OF_MINUTES_IN_AN_HOUR  60
#define NO_OF_SECONDS_IN_A_MINUTE 60
#define NO_OF_MONTHS_IN_A_YEAR    12
#define BASE_YEAR                 2000
#define NO_OF_DAYS_IN_A_MONTH_1   28
#define NO_OF_DAYS_IN_A_MONTH_2   29
#define NO_OF_DAYS_IN_A_MONTH_3   30
#define NO_OF_DAYS_IN_A_MONTH_4   31

/* Constants required to manipulate the NVIC. */
#define portNVIC_SHPR3_REG   (*((volatile uint32_t *)0xe000ed20))
#define portNVIC_PENDSV_PRI  (((uint32_t)(0x3f << 4)) << 16UL)
#define portNVIC_SYSTICK_PRI (((uint32_t)(0x3f << 4)) << 24UL)

#define RTC_ALARM_IRQHandler IRQ028_Handler
#define NVIC_RTC_ALARM       MCU_CAL_ALARM_IRQn
#endif
/******************************************************
*               Function Declarations
******************************************************/
static void application_start(void *argument);
sl_status_t load_certificates_in_flash(void);
sl_status_t start_aws_mqtt(void);
void subscribe_handler(struct _Client *pClient,
                       char *pTopicName,
                       short unsigned int topicNameLen,
                       IoT_Publish_Message_Params *pParams,
                       void *pClientData);
void disconnect_notify_handler(AWS_IoT_Client *pClient, void *data);

#if (defined(RSI_M4_INTERFACE) && ENABLE_POWER_SAVE)
void initialize_m4_alarm(void);
void set_alarm_interrupt_timer(uint16_t interval);
void m4_sleep_wakeup(void);
void wakeup_source_config(void);
#endif

//! Enumeration for states in application
typedef enum app_state {
  WLAN_INITIAL_STATE = 0,
  WLAN_SCAN_STATE,
  WLAN_UNCONNECTED_STATE,
  WLAN_CONNECTED_STATE,
  WLAN_DISCONNECT,
  AWS_MQTT_INIT_STATE,
  AWS_MQTT_CONNECT_STATE,
  AWS_MQTT_SUBSCRIBE_STATE,
  AWS_MQTT_PUBLISH_STATE,
  AWS_MQTT_RECEIVE_STATE,
  AWS_MQTT_DISCONNECT,
  AWS_SELECT_CONNECT_STATE,
  SLEEP_STATE,
  IDLE_STATE
} app_state_t;

volatile app_state_t application_state;

/******************************************************
*               Variable Definitions
******************************************************/

IoT_Publish_Message_Params publish_iot_msg = { 0 };
#define WIRELESS_WAKEUP_IRQHandler          NPSS_TO_MCU_WIRELESS_INTR_IRQn
#define WIRELESS_WAKEUP_IRQHandler_Priority 8

fd_set read_fds;
AWS_IoT_Client mqtt_client = { 0 };
#define RSI_FD_ISSET(x, y) rsi_fd_isset(x, y)
volatile uint8_t check_for_recv_data;
extern osSemaphoreId_t select_sem;
extern volatile uint8_t pub_state, qos1_publish_handle, select_given;
int32_t status = SL_STATUS_OK;

#if (defined(RSI_M4_INTERFACE) && ENABLE_POWER_SAVE)
static RTC_TIME_CONFIG_T rtc_configuration, alarm_configuration, rtc_get_time;
#endif

const osThreadAttr_t thread_attributes = {
  .name       = "app",
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = 3072,
  .priority   = osPriorityNormal,
  .tz_module  = 0,
  .reserved   = 0,
};

//! No of ltcp socktes
#define RSI_NUMBER_OF_LTCP_SOCKETS 0

//! Default number of sockets supported,max 10 sockets are supported
#define RSI_NUMBER_OF_SOCKETS (6 + RSI_NUMBER_OF_LTCP_SOCKETS)

//! Default number of sockets supported,max 10 selects are supported
#define RSI_NUMBER_OF_SELECTS (RSI_NUMBER_OF_SOCKETS)

static const sl_wifi_device_configuration_t client_init_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .region_code = US,
  .boot_config = { .oper_mode = SL_SI91X_CLIENT_MODE,
                   .coex_mode = SL_SI91X_WLAN_MODE,
                   .feature_bit_map =
#ifdef RSI_M4_INTERFACE
                     (SL_SI91X_FEAT_SECURITY_OPEN | SL_SI91X_FEAT_WPS_DISABLE | SL_SI91X_FEAT_ULP_GPIO_BASED_HANDSHAKE),
#else
                     (SL_SI91X_FEAT_SECURITY_OPEN | SL_SI91X_FEAT_AGGREGATION
#if ENABLE_POWER_SAVE
                      | SL_SI91X_FEAT_ULP_GPIO_BASED_HANDSHAKE
#endif
                      ),
#endif

                   .tcp_ip_feature_bit_map =
                     (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT | SL_SI91X_TCP_IP_FEAT_DNS_CLIENT | SL_SI91X_TCP_IP_FEAT_SSL
#ifdef ipv6_FEATURE_REQUIRED
                      | SL_SI91X_TCP_IP_FEAT_DHCPV6_CLIENT | SL_SI91X_TCP_IP_FEAT_IPV6
#endif
                      | SL_SI91X_TCP_IP_FEAT_ICMP | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID),
                   .custom_feature_bit_map = SL_SI91X_FEAT_CUSTOM_FEAT_EXTENTION_VALID,
                   .ext_custom_feature_bit_map =
                     (SL_SI91X_EXT_FEAT_XTAL_CLK_ENABLE(1) | SL_SI91X_EXT_FEAT_UART_SEL_FOR_DEBUG_PRINTS
                      | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0 | SL_SI91X_EXT_FEAT_LOW_POWER_MODE
#ifdef RSI_M4_INTERFACE
                      | RAM_LEVEL_NWP_BASIC_MCU_ADV
#else
                      | RAM_LEVEL_NWP_ALL_MCU_ZERO
#endif
#ifdef CHIP_917
                      | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
                      ),
                   .bt_feature_bit_map = 0,
                   .ext_tcp_ip_feature_bit_map =
                     (SL_SI91X_EXT_TCP_IP_WINDOW_SCALING | SL_SI91X_EXT_TCP_IP_TOTAL_SELECTS(1)
                      | SL_SI91X_CONFIG_FEAT_EXTENTION_VALID),
                   .ble_feature_bit_map     = 0,
                   .ble_ext_feature_bit_map = 0,
#ifdef RSI_M4_INTERFACE
                   .config_feature_bit_map = 0
#else
#if ENABLE_POWER_SAVE
                   .config_feature_bit_map = SL_SI91X_FEAT_SLEEP_GPIO_SEL_BITMAP
#else
                   .config_feature_bit_map = 0
#endif
#endif
  }
};

/******************************************************
*               Function Definitions
******************************************************/

void async_socket_select(fd_set *fd_read, fd_set *fd_write, fd_set *fd_except, int32_t status)
{
  UNUSED_PARAMETER(fd_except);
  UNUSED_PARAMETER(fd_write);
  UNUSED_PARAMETER(status);

  //!Check the data pending on this particular socket descriptor
  if (FD_ISSET(mqtt_client.networkStack.socket_id, fd_read)) {
    if (pub_state != 1) { //This check is for handling PUBACK in QOS1
      check_for_recv_data = 1;
      application_state   = AWS_SELECT_CONNECT_STATE;
    } else if (pub_state == 1) { //This check is for handling PUBACK in QOS1
      osSemaphoreRelease(select_sem);
    }
  }
  application_state = AWS_SELECT_CONNECT_STATE;
}

void disconnect_notify_handler(AWS_IoT_Client *pClient, void *data)
{
  UNUSED_PARAMETER(pClient);
  UNUSED_PARAMETER(data);
  printf("\r\nMQTT disconnected abruptly and pClient state is: %d\r\n", pClient->clientStatus.clientState);
}

void subscribe_handler(struct _Client *pClient,
                       char *pTopicName,
                       short unsigned int topicNameLen,
                       IoT_Publish_Message_Params *pParams,
                       void *data)
{
  UNUSED_PARAMETER(pClient);
  UNUSED_PARAMETER(pTopicName);
  UNUSED_PARAMETER(topicNameLen);
  UNUSED_PARAMETER(data);
  printf("\rData received on the Subscribed Topic: %.*s \r\n", pParams->payloadLen, (char *)pParams->payload);
}

void app_init(void)
{
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

static void application_start(void *argument)
{
  UNUSED_PARAMETER(argument);
  sl_net_wifi_client_profile_t profile = { 0 };
  sl_ip_address_t ip_address           = { 0 };

  select_sem = osSemaphoreNew(1, 0, NULL);

  sl_status_t status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &client_init_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    printf("\r\nUnexpected error while initializing Wi-Fi: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nWi-Fi is Initialized\r\n");

#ifdef RSI_M4_INTERFACE
  uint8_t xtal_enable = 1;
  status              = sl_si91x_m4_ta_secure_handshake(SI91X_ENABLE_XTAL, 1, &xtal_enable, 0, NULL);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to bring m4_ta_secure_handshake: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nM4-NWP secure handshake is successful\r\n");
#endif

  status = sl_net_up(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID);
  if (status != SL_STATUS_OK) {
    printf("\r\nError while connecting to Access point: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nConnected to Access point\r\n");

  status = sl_net_get_profile(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID, &profile);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to get client profile: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nGetting client profile is successful\r\n");

  ip_address.type = SL_IPV4;
  memcpy(&ip_address.ip.v4.bytes, &profile.ip.ip.v4.ip_address.bytes, sizeof(sl_ipv4_address_t));
  printf("\r\nIP address is ");
  print_sl_ip_address(&ip_address);
  printf("\r\n");

  status = load_certificates_in_flash();
  if (status != SL_STATUS_OK) {
    printf("\r\Error while loading certificates: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nLoaded certificates\r\n");

  status = start_aws_mqtt();
  if (status != SL_STATUS_OK) {
    printf("\r\nUnexpected error occurred in AWS connection: 0x%lx\r\n", status);
    return;
  }
}

sl_status_t load_certificates_in_flash(void)
{
  sl_status_t status                             = SL_STATUS_FAIL;
  sl_tls_store_configuration_t tls_configuration = { 0 };

  //! Clearing SSL CA certificate loaded in to FLASH if any
  status = sl_tls_clear_global_ca_store();
  if (status != SL_STATUS_OK) {
    return status;
  }

  tls_configuration.cacert             = (uint8_t *)aws_starfield_ca;
  tls_configuration.cacert_length      = (sizeof(aws_starfield_ca));
  tls_configuration.cacert_type        = SL_TLS_SSL_CA_CERTIFICATE;
  tls_configuration.clientcert         = (uint8_t *)aws_client_certificate;
  tls_configuration.clientcert_length  = (sizeof(aws_client_certificate));
  tls_configuration.clientcert_type    = SL_TLS_SSL_CLIENT;
  tls_configuration.clientkey          = (uint8_t *)aws_client_private_key;
  tls_configuration.clientkey_length   = (sizeof(aws_client_private_key));
  tls_configuration.clientkey_type     = SL_TLS_SSL_CLIENT_PRIVATE_KEY;
  tls_configuration.use_secure_element = false;

  //! Loading SSL Client certificate in FLASH
  status = sl_tls_set_global_ca_store(tls_configuration);
  if (status != SL_STATUS_OK) {
    printf("\r\nLoading TLS certificate in to FLASH failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  return SL_STATUS_OK;
}

sl_status_t start_aws_mqtt(void)
{
#if !(defined(RSI_M4_INTERFACE) && ENABLE_POWER_SAVE)
  uint32_t start_time         = 0;
  uint8_t publish_timer_start = 0;
  uint8_t publish_msg         = 0;
#endif
  IoT_Error_t rc = FAILURE;

  IoT_Client_Init_Params mqtt_init_params       = iotClientInitParamsDefault;
  IoT_Client_Connect_Params mqtt_connect_params = iotClientConnectParamsDefault;

  sl_mac_address_t mac_addr = { 0 };
  char mac_id[12];
  sl_wifi_get_mac_address(SL_WIFI_CLIENT_INTERFACE, &mac_addr);
  sprintf(mac_id,
          "%x:%x:%x:%x:%x:%x",
          mac_addr.octet[0],
          mac_addr.octet[1],
          mac_addr.octet[2],
          mac_addr.octet[3],
          mac_addr.octet[4],
          mac_addr.octet[5]);
  printf("\r\n client_id:%s \r\n", mac_id);

  sl_wifi_version_string_t fw_version = { 0 };
  status                              = sl_wifi_get_firmware_version(&fw_version);

  if (status == SL_STATUS_OK) {
    printf("\r\nFirmware version response: %s\r\n", fw_version.version);
  } else {
    printf("\r\nFailed to get Firmware version \r\n");
  }

  mqtt_init_params.enableAutoReconnect       = true;
  mqtt_init_params.pHostURL                  = AWS_IOT_MQTT_HOST;
  mqtt_init_params.port                      = AWS_IOT_MQTT_PORT;
  mqtt_init_params.pRootCALocation           = (char *)aws_starfield_ca;
  mqtt_init_params.pDeviceCertLocation       = (char *)aws_client_certificate;
  mqtt_init_params.pDevicePrivateKeyLocation = (char *)aws_client_private_key;
  mqtt_init_params.mqttCommandTimeout_ms     = 20000;
  mqtt_init_params.tlsHandshakeTimeout_ms    = 5000;
  mqtt_init_params.isSSLHostnameVerify       = true;
  mqtt_init_params.disconnectHandler         = disconnect_notify_handler;
  mqtt_init_params.disconnectHandlerData     = NULL;

  mqtt_connect_params.keepAliveIntervalInSec = 1200;
  mqtt_connect_params.isCleanSession         = true;
  mqtt_connect_params.MQTTVersion            = MQTT_3_1_1;
  mqtt_connect_params.pClientID              = AWS_IOT_MQTT_CLIENT_ID;
  mqtt_connect_params.clientIDLen            = (uint16_t)strlen(AWS_IOT_MQTT_CLIENT_ID);
  mqtt_connect_params.isWillMsgPresent       = false;
  mqtt_connect_params.pUsername              = MQTT_USERNAME;
  mqtt_connect_params.usernameLen            = strlen(MQTT_USERNAME);
  mqtt_connect_params.pPassword              = MQTT_PASSWORD;
  mqtt_connect_params.passwordLen            = strlen(MQTT_PASSWORD);

  application_state = AWS_MQTT_INIT_STATE;
#if (defined(RSI_M4_INTERFACE) && ENABLE_POWER_SAVE)
#if ALARM_TIMER_BASED_WAKEUP
  initialize_m4_alarm();
#endif
#endif

  while (1) {
    switch (application_state) {

      case AWS_MQTT_INIT_STATE: {
        rc = aws_iot_mqtt_init(&mqtt_client, &mqtt_init_params);
        if (SUCCESS != rc) {
          application_state = AWS_MQTT_INIT_STATE;
          printf("\r\nMQTT Initialization failed with error: 0x%x\r\n", rc);
        } else {
          application_state = AWS_MQTT_CONNECT_STATE;
        }

      } break;
      case AWS_MQTT_CONNECT_STATE: {
        rc = aws_iot_mqtt_connect(&mqtt_client, &mqtt_connect_params);
        if (SUCCESS != rc) {
          if (rc == NETWORK_ALREADY_CONNECTED_ERROR) {
            printf("\r\nNetwork is already connected\r\n");

          } else {
            printf("\r\nMQTT connect failed with error: 0x%x\r\n", rc);
            application_state = AWS_MQTT_DISCONNECT;
          }
        } else {
          printf("\r\nConnected to AWS IoT Cloud\n");
          application_state = AWS_MQTT_SUBSCRIBE_STATE;
        }

      } break;
      case AWS_MQTT_SUBSCRIBE_STATE: {
        rc = aws_iot_mqtt_subscribe(&mqtt_client,
                                    SUBSCRIBE_TO_TOPIC,
                                    strlen(SUBSCRIBE_TO_TOPIC),
                                    SUBSCRIBE_QOS,
                                    subscribe_handler,
                                    NULL);

        if (SUCCESS != rc) {
          if (NETWORK_DISCONNECTED_ERROR == rc) {
            printf("\r\nSubscription failed with error: 0x%x\r\n", rc);
            application_state = AWS_MQTT_CONNECT_STATE;
          } else if (NETWORK_ATTEMPTING_RECONNECT == rc) {
            // If the client is attempting to reconnect skip the rest of the loop
            continue;
          }
          application_state = AWS_MQTT_SUBSCRIBE_STATE;
        } else {
          printf("\rSubscribed to the specified topic with QoS%d\n", SUBSCRIBE_QOS);
          application_state = AWS_SELECT_CONNECT_STATE;
        }

      } break;

      case AWS_SELECT_CONNECT_STATE: {
        {
          if (!select_given) {
            select_given = 1;
            memset(&read_fds, 0, sizeof(fd_set));

            FD_SET(mqtt_client.networkStack.socket_id, &read_fds);
            printf("\rSocket ID: %d\n", mqtt_client.networkStack.socket_id);
            status =
              sl_si91x_select(mqtt_client.networkStack.socket_id + 1, &read_fds, NULL, NULL, NULL, async_socket_select);

            printf("\rSelect status: 0x%lX\r\n", status);
          }

          if (check_for_recv_data) {
            check_for_recv_data = 0;
            select_given        = 0;
            application_state   = AWS_MQTT_RECEIVE_STATE;
          } else if (SUBSCRIBE_QOS == QOS1 || PUBLISH_QOS == QOS1) {
            if (qos1_publish_handle == 0) {
              application_state = AWS_MQTT_PUBLISH_STATE;
            } else {
#if ENABLE_POWER_SAVE
              qos1_publish_handle = 0;
              application_state   = SLEEP_STATE;
#else
              application_state = AWS_MQTT_PUBLISH_STATE;
#endif
            }
          } else if (SUBSCRIBE_QOS == QOS0 || PUBLISH_QOS == QOS0) {
            application_state = AWS_MQTT_PUBLISH_STATE;
          }
        }

      } break;

      case AWS_MQTT_RECEIVE_STATE: {
        rc = aws_iot_shadow_yield(&mqtt_client, 1);
        if (SUCCESS == rc) {
          printf("\rYield is successful\r\n", rc);
#if !(ENABLE_POWER_SAVE)
          publish_msg = 1;
#endif
        }
        application_state = AWS_SELECT_CONNECT_STATE;
      } break;

      case AWS_MQTT_PUBLISH_STATE: {

#if !(ENABLE_POWER_SAVE)

        if ((!publish_timer_start) || publish_msg) {
#endif

          printf("\rData to be published: %s\n", MQTT_PUBLISH_PAYLOAD);

          publish_iot_msg.qos        = PUBLISH_QOS;
          publish_iot_msg.payload    = MQTT_PUBLISH_PAYLOAD;
          publish_iot_msg.isRetained = 0;
          publish_iot_msg.payloadLen = strlen(MQTT_PUBLISH_PAYLOAD);

          if (SUBSCRIBE_QOS == QOS1 || PUBLISH_QOS == QOS1) {
            pub_state = 1;
          }
          rc = aws_iot_mqtt_publish(&mqtt_client, PUBLISH_ON_TOPIC, strlen(PUBLISH_ON_TOPIC), &publish_iot_msg);

          if (rc != SUCCESS) {
            printf("\r\nMQTT Publish with QoS%d failed with error: 0x%x\n", PUBLISH_QOS, rc);
          }

          else if (rc == MQTT_REQUEST_TIMEOUT_ERROR) {
            printf("\r\nACK not received for QoS%d publish\r\n", PUBLISH_QOS);
          } else {
            printf("\rQoS%d publish is successful\r\n", PUBLISH_QOS);
          }

#if (!ENABLE_POWER_SAVE)
          publish_msg = 0;
          if (!publish_timer_start) {
            publish_timer_start = 1;
            start_time          = sl_si91x_host_get_timestamp();
          }

        } else {

          if (sl_si91x_host_elapsed_time(start_time) >= PUBLISH_PERIODICITY) {
            start_time          = 0;
            publish_timer_start = 0;
            publish_msg         = 1;
            continue;
          }
        }
#endif

#if ENABLE_POWER_SAVE
        sl_wifi_performance_profile_t performance_profile = { .profile         = ASSOCIATED_POWER_SAVE,
                                                              .listen_interval = 1000 };
        if (!powersave_given) {
          rc = sl_wifi_set_performance_profile(&performance_profile);
          if (rc != SL_STATUS_OK) {
            printf("\r\nPower save configuration Failed, Error Code : 0x%X\r\n", rc);
          }
          printf("\r\nAssociated Power Save is enabled\r\n");
          powersave_given = 1;
        }

        if (SUBSCRIBE_QOS == QOS1 || PUBLISH_QOS == QOS1) {
          application_state = AWS_SELECT_CONNECT_STATE;
        } else if (SUBSCRIBE_QOS == QOS0 || PUBLISH_QOS == QOS0) {
          application_state = SLEEP_STATE;
        }

#else
        application_state = IDLE_STATE;
#endif
      } break;

#if ENABLE_POWER_SAVE
      case SLEEP_STATE: {
        sl_si91x_host_delay_ms(200);
        printf("\rM4 going to power save state..\r\n");
        printf("\select_given before sleep: %d\r\n", select_given);

        if (select_given == 1 && (check_for_recv_data != 1)) {

#ifdef RSI_M4_INTERFACE
          m4_sleep_wakeup();
#endif
        }
        application_state = AWS_SELECT_CONNECT_STATE;

      } break;
#endif
      case AWS_MQTT_DISCONNECT: {
        rc = aws_iot_mqtt_disconnect(&mqtt_client);
        if (SUCCESS != rc) {
          printf("\r\nMQTT disconnection error\r\n");
          application_state = AWS_MQTT_INIT_STATE;
        }
        application_state = AWS_MQTT_INIT_STATE;

      } break;
      case IDLE_STATE: {
        application_state = AWS_SELECT_CONNECT_STATE;

      } break;

#if 0
        case RSI_WLAN_DISCONNECT: {
          rc = sl_net_down();
          if (SUCCESS != rc) {
            printf("\r\nWLAN disconnection error\r\n");
            application_state = WLAN_DISCONNECT;
          }
          sl_si91x_host_delay_ms(100);
          application_state = WLAN_INITIAL_STATE;
        }
          break;
#endif
      default:
        break;
    }
  }

  return rc;
}

#ifdef RSI_M4_INTERFACE
#if ENABLE_POWER_SAVE
void m4_sleep_wakeup(void)
{
#ifndef FLASH_BASED_EXECUTION_ENABLE
  /* LDOSOC Default Mode needs to be disabled */
  sl_si91x_disable_default_ldo_mode();

  /* bypass_ldorf_ctrl needs to be enabled */
  sl_si91x_enable_bypass_ldo_rf();

  sl_si91x_disable_flash_ldo();

  /* Configure RAM Usage and Retention Size */
  sl_si91x_configure_ram_retention(WISEMCU_48KB_RAM_IN_USE, WISEMCU_RETAIN_DEFAULT_RAM_DURING_SLEEP);

  /* Trigger M4 Sleep */
  sl_si91x_trigger_sleep(SLEEP_WITH_RETENTION,
                         DISABLE_LF_MODE,
                         0,
                         (uint32_t)RSI_PS_RestoreCpuContext,
                         0,
                         RSI_WAKEUP_WITH_RETENTION_WO_ULPSS_RAM);

#else

#if ALARM_TIMER_BASED_WAKEUP
  /* Update the alarm time interval, when to get next interrupt  */
  set_alarm_interrupt_timer(ALARM_PERIODIC_TIME);

#endif

  /* Configure Wakeup-Source */
  RSI_PS_SetWkpSources(WIRELESS_BASED_WAKEUP);
  NVIC_SetPriority(WIRELESS_WAKEUP_IRQHandler, WIRELESS_WAKEUP_IRQHandler_Priority);

  /* Enable NVIC */
  NVIC_EnableIRQ(WIRELESS_WAKEUP_IRQHandler);

#if BUTTON_BASED_WAKEUP
  /*Configure the UULP GPIO 2 as wakeup source */
  wakeup_source_config();
#endif

  /* Configure RAM Usage and Retention Size */
  sl_si91x_configure_ram_retention(WISEMCU_192KB_RAM_IN_USE, WISEMCU_RETAIN_DEFAULT_RAM_DURING_SLEEP);

  printf("\rM4 Sleep\r\n");

  /* Trigger M4 Sleep*/
  sl_si91x_trigger_sleep(SLEEP_WITH_RETENTION,
                         DISABLE_LF_MODE,
                         WKP_RAM_USAGE_LOCATION,
                         (uint32_t)RSI_PS_RestoreCpuContext,
                         IVT_OFFSET_ADDR,
                         RSI_WAKEUP_FROM_FLASH_MODE);

  /* Enable M4_TA interrupt */
  sli_m4_ta_interrupt_init();

  //  /*Start of M4 init after wake up  */
  printf("\r\nM4 Wake Up\r\n");
#endif
}
#if ALARM_TIMER_BASED_WAKEUP
void set_alarm_interrupt_timer(uint16_t interval)
{
  /* Get the RTC time,which is used to update alarm time as per RTC time  */
  RSI_RTC_GetDateTime(RTC, &rtc_get_time);

  /*RTC alarm configuration */
  alarm_configuration.DayOfWeek    = rtc_get_time.DayOfWeek;
  alarm_configuration.Month        = rtc_get_time.Month;
  alarm_configuration.Century      = rtc_get_time.Century;
  alarm_configuration.MilliSeconds = rtc_get_time.MilliSeconds;
  alarm_configuration.Day          = rtc_get_time.Day;
  alarm_configuration.Year         = rtc_get_time.Year;
  alarm_configuration.Minute       = rtc_get_time.Minute;
  alarm_configuration.Hour         = rtc_get_time.Hour;
  alarm_configuration.Second       = rtc_get_time.Second;

  /*Update seconds for next boundary alarm */
  alarm_configuration.Second = alarm_configuration.Second + (interval % 60);
  if (alarm_configuration.Second >= (NO_OF_SECONDS_IN_A_MINUTE)) {
    alarm_configuration.Second -= NO_OF_SECONDS_IN_A_MINUTE;
    alarm_configuration.Minute += 1;
  }

  /*Update minutes for next boundary alarm */
  alarm_configuration.Minute = alarm_configuration.Minute + ((interval / 60) % 60);
  if (alarm_configuration.Minute >= (NO_OF_MINUTES_IN_AN_HOUR)) {
    alarm_configuration.Minute -= NO_OF_MINUTES_IN_AN_HOUR;
    alarm_configuration.Hour += 1;
  }

  /*Update hour for next boundary alarm */
  alarm_configuration.Hour = alarm_configuration.Hour + (interval / 3600) % 24;
  if (alarm_configuration.Hour >= (NO_OF_HOURS_IN_A_DAY)) {
    alarm_configuration.Hour -= NO_OF_HOURS_IN_A_DAY;
    alarm_configuration.Day += 1;
  }

  /*Update month for next boundary alarm */
  if (alarm_configuration.Day > NO_OF_DAYS_IN_A_MONTH_1) {
    if (alarm_configuration.Month == February) {
      if (alarm_configuration.Year % 4) {
        alarm_configuration.Day = 1;
        alarm_configuration.Month += 1;
      } else if (alarm_configuration.Day > NO_OF_DAYS_IN_A_MONTH_2) {
        alarm_configuration.Day = 1;
        alarm_configuration.Month += 1;
      }
    }
    if (alarm_configuration.Month <= July) {
      if (alarm_configuration.Month % 2 == 0) {
        if (alarm_configuration.Day > NO_OF_DAYS_IN_A_MONTH_3) {
          alarm_configuration.Day = 1;
          alarm_configuration.Month += 1;
        }
      } else if (alarm_configuration.Day > NO_OF_DAYS_IN_A_MONTH_4) {
        alarm_configuration.Day = 1;
        alarm_configuration.Month += 1;
      }

    } else if (alarm_configuration.Month % 2 == 0) {
      if (alarm_configuration.Day > NO_OF_DAYS_IN_A_MONTH_4) {
        alarm_configuration.Day = 1;
        alarm_configuration.Month += 1;
      }
    } else if (alarm_configuration.Day > NO_OF_DAYS_IN_A_MONTH_3) {
      alarm_configuration.Day = 1;
      alarm_configuration.Month += 1;
    }
  }

  /*Update year  for next boundary alarm */
  if (alarm_configuration.Month > (NO_OF_MONTHS_IN_A_YEAR)) {
    alarm_configuration.Month = 1;
    alarm_configuration.Year += 1;
  }

  /*Set Alarm configuration */
  RSI_RTC_SetAlarmDateTime(RTC, &alarm_configuration);
}

void initialize_m4_alarm(void)
{
  /*Init RTC*/
  RSI_RTC_Init(RTC);

  /*RTC configuration with some default time */
  rtc_configuration.DayOfWeek    = Saturday;
  rtc_configuration.Month        = March;
  rtc_configuration.Day          = 19;
  rtc_configuration.Century      = 0;
  rtc_configuration.Year         = 19;
  rtc_configuration.Hour         = 23;
  rtc_configuration.Minute       = 59;
  rtc_configuration.Second       = 50;
  rtc_configuration.MilliSeconds = 0;

  /*Set the RTC configuration*/
  RSI_RTC_SetDateTime(RTC, &rtc_configuration);
  /*Enable Alarm feature*/
  RSI_RTC_AlamEnable(RTC, ENABLE);
  /*Enable RTC ALARM interrupts*/
  RSI_RTC_IntrUnMask(RTC_ALARM_INTR);
  /*Initialization of RTC CALIBRATION*/
  RSI_RTC_CalibInitilization();
  /*To calibrate rc and ro */
  RSI_RTC_ROCLK_Calib(TIME_PERIOD, ENABLE, ENABLE, RC_TRIGGER_TIME, ENABLE, ENABLE, RO_TRIGGER_TIME);
  /*Set Alarm as a  wake up source to wake up from deep sleep */
  RSI_PS_SetWkpSources(ALARM_BASED_WAKEUP);
  /*Enable the RTC alarm interrupts */
  RSI_RTC_IntrUnMask(RTC_ALARM_INTR);
  /*Enable NVIC for RTC */
  NVIC_EnableIRQ(NVIC_RTC_ALARM);
}
/*RTC Alarm interrupt*/
void RTC_ALARM_IRQHandler(void)
{
  volatile uint32_t statusRead = 0;
  /*Get the interrupt status */
  statusRead = RSI_RTC_GetIntrStatus();

  if (statusRead & NPSS_TO_MCU_ALARM_INTR) {

    /* TRIGGER SLEEP STATE */
    /*Clear wake up interrupt */
    RSI_RTC_IntrClear(RTC_ALARM_INTR);
  }
  return;
}
#endif

void IRQ026_Handler()
{
  RSI_PS_GetWkpUpStatus();
  /*Clear interrupt */
  ps_clr_wkp_up_status(NPSS_TO_MCU_WIRELESS_INTR);
  return;
}

#if BUTTON_BASED_WAKEUP
/**
 * @brief  Configure the UULP GPIO 2 as wakeup source
 * @param  none
 * @return none
 */
void wakeup_source_config(void)
{
  /*Configure the NPSS GPIO mode to wake up  */
  RSI_NPSSGPIO_SetPinMux(NPSS_GPIO_2, NPSSGPIO_PIN_MUX_MODE2);

  /*Configure the NPSS GPIO direction to input */
  RSI_NPSSGPIO_SetDir(NPSS_GPIO_2, NPSS_GPIO_DIR_INPUT);

  /*Configure the NPSS GPIO interrupt polarity */
  RSI_NPSSGPIO_SetPolarity(NPSS_GPIO_2, NPSS_GPIO_INTR_LOW);

  /*Enable the REN*/
  RSI_NPSSGPIO_InputBufferEn(NPSS_GPIO_2, 1);

  /* Set the GPIO to wake from deep sleep */
  RSI_NPSSGPIO_SetWkpGpio(NPSS_GPIO_2_INTR);

  /* Un mask the NPSS GPIO interrupt*/
  RSI_NPSSGPIO_IntrUnMask(NPSS_GPIO_2_INTR);

  /*Select wake up sources */
  RSI_PS_SetWkpSources(GPIO_BASED_WAKEUP);

  /*Enable the NPSS GPIO interrupt slot*/
  NVIC_EnableIRQ(NPSS_TO_MCU_GPIO_INTR_IRQn);
}

/**
 * @brief  GPIO based wake up IRQ
 * @param  none
 * @return none
 */
void IRQ021_Handler(void)
{

  /* clear NPSS GPIO interrupt*/
  RSI_NPSSGPIO_ClrIntr(NPSS_GPIO_2_INTR);
}
#endif
#endif
#endif
