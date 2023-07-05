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
#include "sl_status.h"
#include "cmsis_os2.h"
#include <string.h>
#include <stdbool.h>

#include "sl_wifi_device.h"
#include "sl_wifi.h"
#include "sl_net.h"
#include "sl_tls.h"
#include "sl_utility.h"

//! Certificates to be loaded
#include "aws_client_certificate.pem.crt.h"
#include "aws_client_private_key.pem.key.h"
#include "aws_starfield_ca.pem.h"

//! AWS files
#include "aws_iot_log.h"
#include "aws_iot_error.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"
#include "aws_iot_config.h"

/******************************************************
*                    Constants
******************************************************/
#define MQTT_TOPIC                "$aws/things/silicon_labs_thing/shadow/update"
#define MQTT_PUBLISH_QOS1_PAYLOAD "{\"message\": \"toggle\"}"
#define MQTT_USERNAME             "username"
#define MQTT_PASSWORD             "password"

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
  .reserved   = 0,
};

static const sl_wifi_device_configuration_t client_init_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .boot_config = { .oper_mode       = SL_SI91X_CLIENT_MODE,
                   .coex_mode       = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map = (SL_SI91X_FEAT_SECURITY_PSK | SL_SI91X_FEAT_AGGREGATION),
                   .tcp_ip_feature_bit_map =
                     (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT | SL_SI91X_TCP_IP_FEAT_DNS_CLIENT | SL_SI91X_TCP_IP_FEAT_SSL),
                   .custom_feature_bit_map     = (SL_SI91X_FEAT_CUSTOM_FEAT_EXTENTION_VALID),
                   .ext_custom_feature_bit_map = (
#ifdef CHIP_917B0
#ifndef RSI_M4_INTERFACE
                     SL_SI91X_EXT_FEAT_672K_M4SS_0K
#else
                     SL_SI91X_EXT_FEAT_480K_M4SS_192K
#endif // RSI_M4_INTERFACE
#elif CHIP_917
#ifndef RSI_M4_INTERFACE
                     SL_SI91X_EXT_FEAT_704K_M4SS_0K
#else
                     SL_SI91X_EXT_FEAT_512K_M4SS_192K
#endif // RSI_M4_INTERFACE
                     | SL_SI91X_EXT_FEAT_XTAL_CLK_ENABLE(2)

#else
                     SL_SI91X_EXT_FEAT_384K_MODE | SL_SI91X_EXT_FEAT_XTAL_CLK_ENABLE(2)
#endif // CHIP_917
                     ),
                   .bt_feature_bit_map         = 0,
                   .ext_tcp_ip_feature_bit_map = 0,
                   .ble_feature_bit_map        = 0,
                   .ble_ext_feature_bit_map    = 0,
                   .config_feature_bit_map     = 0 }
};

/******************************************************
*               Function Declarations
******************************************************/
extern void sl_platform_init(void);
void application_start(void *argument);
sl_status_t connect_to_access_point();
sl_status_t load_certificates_in_flash();
sl_status_t start_aws_mqtt();
void disconnect_notify_handler(AWS_IoT_Client *pClient, void *data);
void subscribe_handler(struct _Client *pClient,
                       char *pTopicName,
                       short unsigned int topicNameLen,
                       IoT_Publish_Message_Params *pParams,
                       void *pClientData);

/******************************************************
*               Function Definitions
******************************************************/
void disconnect_notify_handler(AWS_IoT_Client *pClient, void *data)
{
  UNUSED_PARAMETER(pClient);
  UNUSED_PARAMETER(data);
  printf("\r\nMqtt disconnected abruptly and pClient state is: %d\r\n", pClient->clientStatus.clientState);
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
  printf("\r\nData received on the Subscribed Topic is %.*s \r\n", pParams->payloadLen, (char *)pParams->payload);
}

void app_init(void)
{
  osThreadNew((osThreadFunc_t)(application_start), NULL, &thread_attributes);
}

void application_start(void *argument)
{
  UNUSED_PARAMETER(argument);
  sl_status_t status = sl_net_init(SL_NET_DEFAULT_WIFI_CLIENT_INTERFACE, &client_init_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    printf("\r\nUnexpected error while doing init: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nWiFi Init Success\r\n");

  status = sl_net_up(SL_NET_DEFAULT_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE);
  if (status != SL_STATUS_OK) {
    printf("\r\nError while connecting to Access point: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nConnected to Access point \r\n");

  status = load_certificates_in_flash();
  if (status != SL_STATUS_OK) {
    printf("\r\nUnexpected error while loading certificates: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nCertificate loading success\r\n");

  status = start_aws_mqtt();
  if (status != SL_STATUS_OK) {
    printf("\r\nUnexpected error occurred in AWS MQTT: 0x%lx\r\n", status);
    return;
  }

  return;
}

sl_status_t load_certificates_in_flash()
{
  sl_status_t status;
  sl_tls_store_configuration_t tls_configuration = { 0 };

  tls_configuration.cacert             = (uint8_t *)aws_starfield_ca;
  tls_configuration.cacert_length      = (sizeof(aws_starfield_ca) - 1);
  tls_configuration.cacert_type        = SL_TLS_SSL_CA_CERTIFICATE;
  tls_configuration.clientcert         = (uint8_t *)aws_client_certificate;
  tls_configuration.clientcert_length  = (sizeof(aws_client_certificate) - 1);
  tls_configuration.clientcert_type    = SL_TLS_SSL_CLIENT;
  tls_configuration.clientkey          = (uint8_t *)aws_client_private_key;
  tls_configuration.clientkey_length   = (sizeof(aws_client_private_key) - 1);
  tls_configuration.clientkey_type     = SL_TLS_SSL_CLIENT_PRIVATE_KEY;
  tls_configuration.use_secure_element = false;

  //! Loading SSL certificate in FLASH
  status = sl_tls_set_global_ca_store(tls_configuration);
  if (status != SL_STATUS_OK) {
    printf("\r\nLoading SSL certificate in FLASH Failed, Error Code : 0x%lx\r\n", status);
    return status;
  }

  printf("\r\nLoading SSL certificate in FLASH Success \r\n");
  return SL_STATUS_OK;
}

sl_status_t start_aws_mqtt()
{
  IoT_Error_t rc                                = FAILURE;
  AWS_IoT_Client client                         = { 0 };
  IoT_Client_Init_Params mqtt_init_params       = iotClientInitParamsDefault;
  IoT_Client_Connect_Params mqtt_connect_params = iotClientConnectParamsDefault;

  mqtt_init_params.enableAutoReconnect       = false;
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

  mqtt_connect_params.keepAliveIntervalInSec = 600;
  mqtt_connect_params.isCleanSession         = true;
  mqtt_connect_params.MQTTVersion            = MQTT_3_1_1;
  mqtt_connect_params.pClientID              = AWS_IOT_MQTT_CLIENT_ID;
  mqtt_connect_params.clientIDLen            = (uint16_t)strlen(AWS_IOT_MQTT_CLIENT_ID);
  mqtt_connect_params.isWillMsgPresent       = false;
  mqtt_connect_params.pUsername              = MQTT_USERNAME;
  mqtt_connect_params.usernameLen            = strlen(MQTT_USERNAME);
  mqtt_connect_params.pPassword              = MQTT_PASSWORD;
  mqtt_connect_params.passwordLen            = strlen(MQTT_PASSWORD);

  IoT_Publish_Message_Params publish_QOS1 = { 0 };

  // mqtt init
  rc = aws_iot_mqtt_init(&client, &mqtt_init_params);
  if (rc != SUCCESS) {
    printf("\r\nMqtt Init failed with error: 0x%x\r\n", rc);
    return rc;
  }
  printf("\r\nMqtt Init success\r\n");

  // mqtt connect
  rc = aws_iot_mqtt_connect(&client, &mqtt_connect_params);
  if (rc != SUCCESS) {
    printf("\r\nMqtt Connect failed with error: 0x%x\r\n", rc);
    return rc;
  }
  printf("\r\nMqtt Connect success\r\n");

  // mqtt subscribe
  rc = aws_iot_mqtt_subscribe(&client, MQTT_TOPIC, strlen(MQTT_TOPIC), QOS1, subscribe_handler, NULL);
  if (rc != SUCCESS) {
    printf("\r\nMqtt Subscribe failed with error: 0x%x\r\n", rc);
    return rc;
  }
  printf("\r\nMqtt Subscribe success\r\n");

  publish_QOS1.qos        = QOS1;
  publish_QOS1.payload    = MQTT_PUBLISH_QOS1_PAYLOAD;
  publish_QOS1.payloadLen = strlen(MQTT_PUBLISH_QOS1_PAYLOAD);

  // mqtt publish with QOS1
  rc = aws_iot_mqtt_publish(&client, MQTT_TOPIC, strlen(MQTT_TOPIC), &publish_QOS1);
  if (rc != SUCCESS) {
    printf("\r\nMqtt Publish for QOS1 failed with error: 0x%x\r\n", rc);
    return rc;
  }
  printf("\r\nMqtt Publish with QOS1 success\r\n");

  aws_iot_mqtt_yield(&client, 5000);

  // mqtt disconnect
  rc = aws_iot_mqtt_disconnect(&client);
  if (rc != SUCCESS) {
    printf("\r\nMqtt Disconnect failed with error: 0x%x\r\n", rc);
    return rc;
  }
  printf("\r\nMqtt Disconnect success\r\n");

  return SL_STATUS_OK;
}
