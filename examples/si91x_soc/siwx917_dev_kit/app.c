/***************************************************************************/ /**
 * @file
 * @brief Embedded Sensor Demo
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

#include "sl_net.h"
#include "app.h"
#include "errno.h"
#include "sl_utility.h"
#include "sl_wifi.h"
#include "sl_net_wifi_types.h"
#include "sl_si91x_socket_support.h"
#include "sl_si91x_socket_constants.h"
#include "sl_wifi_callback_framework.h"
#include "sl_si91x_socket.h"
#include "sl_net_si91x.h"
#include "login.h"
#include "provisioning.h"
#include "jsmn.h"
#include "sl_http_server.h"
#include "light_sensor.h"
#include "motion_sensor.h"
#include "humidity_sensor.h"
#include "rgb_led.h"
#include "stdbool.h"
#include "wifi_provisioning.h"
#include "sensor_webserver.h"
#include "sl_sleeptimer.h"
/******************************************************
 *                      Macros
 ******************************************************/

#define METHOD_BAD_REQUEST           "400 Bad Request"
#define METHOD_NOT_ALLOWED           "405 Not Allowed"
#define METHOD_INTERNAL_SERVER_ERROR "500 Internal Server Error"

#define DEFAULT_HTTP_RESPONSE_METHOD_NOT_ALLOWED                                                                  \
  {                                                                                                               \
    .response_code = SL_HTTP_RESPONSE_METHOD_NOT_ALLOWED, .content_type = SL_HTTP_CONTENT_TYPE_TEXT_HTML,         \
    .headers = NULL, .header_count = 0, .data = (uint8_t *)METHOD_NOT_ALLOWED,                                    \
    .current_data_length = sizeof(METHOD_NOT_ALLOWED) - 1, .expected_data_length = sizeof(METHOD_NOT_ALLOWED) - 1 \
  }

#define HTTP_SERVER_PORT 80

// Server port number
#define SERVER_PORT 5000

#define SCAN_RESULT_BUFFER_SIZE (2000)

#define ON            "on"
#define OFF           "off"
#define OPEN          "Open"
#define WPA           "WPA"
#define WPA2          "WPA2"
#define WPA3          "WPA3"
#define MIXED_MODE    "Mixed Mode"
#define UNKNOWN       "Unknown"
#define SSID          "ssid"
#define SECURITY_TYPE "security_type"
#define PASSPHRASE    "passphrase"

#define LED_JSON_RESPONSE           "{\"red\": \"%s\", \"green\": \"%s\", \"blue\": \"%s\"}"
#define TEMPERATURE_JSON_RESPONSE   "{\"temperature_celcius\": \"%0.2f\"}"
#define LIGHT_JSON_RESPONSE         "{\"ambient_light_lux\": \"%0.2f\", \"white_light_lux\": \"%0.2f\"}"
#define MOTION_SENSOR_JSON_RESPONSE "{\"x\": \"%0.2f\", \"y\": \"%0.2f\", \"z\": \"%0.2f\"}"
#define HUMIDITY_JSON_RESPONSE      "{\"humidity_percentage\": \"%lu\"}"
#define MICROPHONE_JSON_RESPONSE    "{\"microphone_decibel\": \"%lu\"}"
#define STATUS_LED_JSON_RESPONSE    "{\"status_led\": \"%s\"}"

#define PROVISIONING_LED_INTERVAL_MS (300)
#define CONNECTING_LED_INTERVAL_MS   (2000)
#define CONNECTED_LED_INTERVAL_MS    (2000)
#define ERROR_LED_INTERVAL_MS        (150)
#define LED_ERROR_BLINK_COUNT        (3)

//! Enumeration for states in application
typedef enum {
  PROVISIONING_INIT_STATE,
  PROVISIONING_STATE,
  CONNECTING_STATE,
  CONNECTED_STATE,
  DISCONNECTING_STATE,
} app_state_t;

typedef enum {
  STATUS_LED_OFF,
  STATUS_LED_PROVISIONING,
  STATUS_LED_CONNECTING,
  STATUS_LED_CONNECTED,
} status_led_state_t;

/******************************************************
 *               Function Declarations
 ******************************************************/

static void application_start(void *argument);
static sl_wifi_security_t string_to_security_type(const char *security_type);
static char *security_type_to_string(sl_wifi_security_t security_type);
static sl_status_t index_request_handler(sl_http_server_t *handle, sl_http_server_request_t *req);
static sl_status_t default_handler(sl_http_server_t *handle, sl_http_server_request_t *req);
static sl_status_t provisioning_request_handler(sl_http_server_t *handle, sl_http_server_request_t *req);
static sl_status_t connect_page_request_handler(sl_http_server_t *handle, sl_http_server_request_t *req);
static sl_status_t connect_data_handler(sl_http_server_t *handle, sl_http_server_request_t *req);
static sl_status_t wifi_scan_request_handler(sl_http_server_t *handle, sl_http_server_request_t *req);
static sl_status_t light_request_handler(sl_http_server_t *handle, sl_http_server_request_t *req);
static sl_status_t led_request_handler(sl_http_server_t *handle, sl_http_server_request_t *req);
static sl_status_t temperature_request_handler(sl_http_server_t *handle, sl_http_server_request_t *req);
static sl_status_t accelerometer_request_handler(sl_http_server_t *handle, sl_http_server_request_t *req);
static sl_status_t gyroscope_request_handler(sl_http_server_t *handle, sl_http_server_request_t *req);
static sl_status_t humidity_request_handler(sl_http_server_t *handle, sl_http_server_request_t *req);
static sl_status_t microphone_request_handler(sl_http_server_t *handle, sl_http_server_request_t *req);
static sl_status_t sensor_webserver_handler(sl_http_server_t *handle, sl_http_server_request_t *req);
static sl_status_t all_sensors_request_handler(sl_http_server_t *handle, sl_http_server_request_t *req);
static sl_status_t status_led_request_handler(sl_http_server_t *handle, sl_http_server_request_t *req);
static sl_status_t ap_connected_event_handler(sl_wifi_event_t event, void *data, uint32_t data_length, void *arg);
static sl_status_t ap_disconnected_event_handler(sl_wifi_event_t event, void *data, uint32_t data_length, void *arg);

/******************************************************
 *               Variable Definitions
 ******************************************************/

static const char *rgb_colours[] = { [0] = "red", [1] = "green", [2] = "blue" };

static bool scan_complete             = false;
static uint8_t retry                  = 0;
static sl_http_server_t server_handle = { 0 };
static sl_status_t callback_status    = SL_STATUS_OK;
static app_state_t app_state          = PROVISIONING_INIT_STATE;

static const osThreadAttr_t thread_attributes = {
  .name       = "app",
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = 3072,
  .priority   = osPriorityLow,
  .tz_module  = 0,
  .reserved   = 0,
};

static const sl_net_wifi_client_profile_t wifi_client_profile_4 = {
    .config = {
        .channel.channel = SL_WIFI_AUTO_CHANNEL,
        .channel.band = SL_WIFI_AUTO_BAND,
        .channel.bandwidth = SL_WIFI_AUTO_BANDWIDTH,
        .bssid = {{0}},
        .bss_type = SL_WIFI_BSS_TYPE_INFRASTRUCTURE,
        .client_options = 0,
        .credential_id = SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID,
    },
    .ip = {
        .mode = SL_IP_MANAGEMENT_DHCP,
        .type = SL_IPV4,
        .host_name = NULL,
        .ip = {{{0}}},
    }
};

static const sl_http_server_handler_t provisioning_server_request_handlers[] = {
  { .uri = "/", .handler = index_request_handler },
  { .uri = "/index.html", .handler = index_request_handler },
  { .uri = "/connect.html", .handler = connect_page_request_handler },
  { .uri = "/connect", .handler = connect_data_handler },
  { .uri = "/scan", .handler = wifi_scan_request_handler },
  { .uri = "/light", .handler = light_request_handler },
  { .uri = "/led", .handler = led_request_handler },
  { .uri = "/temperature", .handler = temperature_request_handler },
  { .uri = "/accelerometer", .handler = accelerometer_request_handler },
  { .uri = "/gyroscope", .handler = gyroscope_request_handler },
  { .uri = "/humidity", .handler = humidity_request_handler },
  { .uri = "/microphone", .handler = microphone_request_handler },
  { .uri = "/all_sensors", .handler = all_sensors_request_handler },
  { .uri = "/status_led", .handler = status_led_request_handler }
};

static const sl_http_server_handler_t sensor_server_request_handlers[] = {
  { .uri = "/", .handler = sensor_webserver_handler },
  { .uri = "/index.html", .handler = sensor_webserver_handler },
  { .uri = "/provisioning", .handler = provisioning_request_handler },
  { .uri = "/light", .handler = light_request_handler },
  { .uri = "/led", .handler = led_request_handler },
  { .uri = "/temperature", .handler = temperature_request_handler },
  { .uri = "/accelerometer", .handler = accelerometer_request_handler },
  { .uri = "/gyroscope", .handler = gyroscope_request_handler },
  { .uri = "/humidity", .handler = humidity_request_handler },
  { .uri = "/microphone", .handler = microphone_request_handler },
  { .uri = "/all_sensors", .handler = all_sensors_request_handler },
  { .uri = "/status_led", .handler = status_led_request_handler }
};

static sl_wifi_client_configuration_t provisioned_access_point = { 0 };

static sl_sleeptimer_timer_handle_t led_timer;

static bool status_led_on = true;

/******************************************************
 *               Function Definitions
 ******************************************************/

void app_init(void)
{
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

static sl_status_t join_callback_handler(sl_wifi_event_t event, char *result, uint32_t result_length, void *arg)
{
  UNUSED_PARAMETER(result);
  UNUSED_PARAMETER(arg);
  printf("in join CB\r\n");
  if (SL_WIFI_CHECK_IF_EVENT_FAILED(event)) {
    printf("F: Join Event received with %lu bytes payload\n", result_length);
    return SL_STATUS_FAIL;
  }
  return SL_STATUS_OK;
}

static void provisioning_and_connecting_led_callback(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  UNUSED_PARAMETER(handle);
  UNUSED_PARAMETER(data);
  blue_led_toggle();
}

static void connected_led_callback(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  UNUSED_PARAMETER(handle);
  UNUSED_PARAMETER(data);
  white_led_toggle();
}

static void play_error_led_sequence(void)
{
  if (status_led_on == false) {
    return;
  }
  rgb_led_set_state(false, false, false);
  sl_sleeptimer_stop_timer(&led_timer);
  for (uint8_t a = 0; a < 2 * LED_ERROR_BLINK_COUNT; a++) {
    red_led_toggle();
    osDelay(ERROR_LED_INTERVAL_MS);
  }
}

static void status_led_set_state(bool state)
{
  status_led_on = state;
  rgb_led_set_state(false, false, false);
  sl_sleeptimer_stop_timer(&led_timer);
}

static void status_led_update_state(void)
{
  if (status_led_on == false) {
    return;
  }
  switch (app_state) {
    case PROVISIONING_INIT_STATE:
    case PROVISIONING_STATE:
      rgb_led_set_state(false, false, false);
      sl_sleeptimer_stop_timer(&led_timer);
      sl_sleeptimer_start_periodic_timer_ms(&led_timer,
                                            PROVISIONING_LED_INTERVAL_MS,
                                            provisioning_and_connecting_led_callback,
                                            NULL,
                                            0,
                                            SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
      break;
    case CONNECTING_STATE:
      sl_sleeptimer_stop_timer(&led_timer);
      sl_sleeptimer_start_periodic_timer_ms(&led_timer,
                                            CONNECTING_LED_INTERVAL_MS,
                                            provisioning_and_connecting_led_callback,
                                            NULL,
                                            0,
                                            SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
      break;
    case CONNECTED_STATE:
      rgb_led_set_state(false, false, false);
      sl_sleeptimer_stop_timer(&led_timer);
      sl_sleeptimer_start_periodic_timer_ms(&led_timer,
                                            CONNECTED_LED_INTERVAL_MS,
                                            connected_led_callback,
                                            NULL,
                                            0,
                                            SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
      break;
    default:
      break;
  }
}

static void application_start(void *argument)
{
  UNUSED_PARAMETER(argument);
  sl_status_t status                    = SL_STATUS_OK;
  sl_net_wifi_client_profile_t profile  = { 0 };
  sl_ip_address_t ip_address            = { 0 };
  sl_http_server_config_t server_config = { 0 };

  printf("\r\nEmbedded sensor demo started\r\n");

  // Init sensor when application starts
  light_sensor_init();
  humidity_sensor_init();
  motion_sensor_init();
  memset(&led_timer, 0, sizeof(led_timer));

  while (1) {
    switch (app_state) {
      case PROVISIONING_INIT_STATE: {
        sl_net_wifi_ap_profile_t ap_profile;

        status_led_update_state();

        status = sl_net_init(SL_NET_WIFI_AP_INTERFACE, (const void *)&sl_wifi_default_ap_configuration, NULL, NULL);
        if (status != SL_STATUS_OK) {
          printf("Failed to start Wi-Fi AP interface: 0x%lx\r\n", status);
          return;
        }

        sl_wifi_set_callback(SL_WIFI_CLIENT_CONNECTED_EVENTS, ap_connected_event_handler, NULL);
        sl_wifi_set_callback(SL_WIFI_CLIENT_DISCONNECTED_EVENTS, ap_disconnected_event_handler, NULL);
        printf("Wi-Fi AP initialized\r\n");

        status = sl_net_up(SL_NET_WIFI_AP_INTERFACE, SL_NET_DEFAULT_WIFI_AP_PROFILE_ID);
        if (status != SL_STATUS_OK) {
          printf("Failed to bring Wi-Fi AP interface up: 0x%lx\r\n", status);
          return;
        }
        printf("Wi-Fi AP started\r\n");

        server_config.port            = HTTP_SERVER_PORT;
        server_config.default_handler = default_handler;
        server_config.handlers_list   = (sl_http_server_handler_t *)provisioning_server_request_handlers;
        server_config.handlers_count  = sizeof(provisioning_server_request_handlers) / sizeof(sl_http_server_handler_t);

        status = sl_http_server_init(&server_handle, &server_config);
        if (status != SL_STATUS_OK) {
          printf("HTTP server init failed:%lx\r\n", status);
          return;
        }
        status = sl_http_server_start(&server_handle);
        if (status != SL_STATUS_OK) {
          printf("Server start fail:%lx\r\n", status);
          return;
        }
        printf("Provisioning HTTP server started\r\n");

        sl_net_get_profile(SL_NET_WIFI_AP_INTERFACE,
                           SL_NET_DEFAULT_WIFI_AP_PROFILE_ID,
                           (sl_net_profile_t *)&ap_profile);
        printf("\r\nConnect to access point \"%s\" from your device\r\n", ap_profile.config.ssid.value);
        printf("Go to http://%u.%u.%u.%u/ on your browser to provision the sensor demo\r\n",
               ap_profile.ip.ip.v4.ip_address.bytes[0],
               ap_profile.ip.ip.v4.ip_address.bytes[1],
               ap_profile.ip.ip.v4.ip_address.bytes[2],
               ap_profile.ip.ip.v4.ip_address.bytes[3]);
        app_state = PROVISIONING_STATE;
        break;
      }
      case CONNECTING_STATE: {
        status_led_update_state();

        status = sl_http_server_stop(&server_handle);
        if (status != SL_STATUS_OK) {
          printf("Server stop fail:%lx\r\n", status);
          return;
        }
        status = sl_http_server_deinit(&server_handle);
        if (status != SL_STATUS_OK) {
          printf("Server deinit fail:%lx\r\n", status);
          return;
        }
        printf("HTTP Server deinitialized\r\n");

        status = sl_net_deinit(SL_NET_WIFI_AP_INTERFACE);
        if (status != SL_STATUS_OK) {
          printf("Ap deinit : 0x%lx\r\n", status);
          return;
        }
        printf("Wi-Fi AP deinitialized\r\n");

        status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &sl_wifi_default_client_configuration, NULL, NULL);
        if (status != SL_STATUS_OK) {
          printf("Failed to start Wi-Fi client interface: 0x%lx\r\n", status);
          return;
        }
        printf("Wi-Fi client interface initialized\r\n");

        sl_wifi_set_callback(SL_WIFI_CLIENT_CONNECTED_EVENTS, ap_connected_event_handler, NULL);
        sl_wifi_set_callback(SL_WIFI_CLIENT_DISCONNECTED_EVENTS, ap_disconnected_event_handler, NULL);

        //  Keeping the station ipv4 record in profile_id_0
        status = sl_net_set_profile(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_PROFILE_ID_0, &wifi_client_profile_4);
        if (status != SL_STATUS_OK) {
          printf("Failed to set client profile: 0x%lx\r\n", status);
          return;
        }
        printf("Wi-Fi set client profile v4 success\r\n");
        printf("SSID %s\r\n", provisioned_access_point.ssid.value);

        sl_wifi_set_join_callback(join_callback_handler, NULL);
        status = sl_wifi_connect(SL_WIFI_CLIENT_2_4GHZ_INTERFACE, &provisioned_access_point, 15000);
        if (status != SL_STATUS_OK) {
          printf("Failed to bring Wi-Fi client interface up: 0x%lx\r\n", status);
          //! If scan failed retry again after 5 seconds
          if (status == 0x10003) {
            if (retry) {
              retry--;
              osDelay(5000);
              break;
            } else {
              retry = 5;
            }
          }
          status = sl_net_deinit(SL_NET_WIFI_CLIENT_INTERFACE);
          if (status != SL_STATUS_OK) {
            printf("STA deinit : 0x%lx\r\n", status);
          }
          play_error_led_sequence();
          app_state = PROVISIONING_INIT_STATE;
          break;
        }

        status = sl_net_get_profile(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_PROFILE_ID_0, &profile);
        if (status != SL_STATUS_OK) {
          printf("Failed to bring Wi-Fi client interface up: 0x%lx\r\n", status);
          return;
        }

        status = sl_si91x_configure_ip_address(&profile.ip, SL_SI91X_WIFI_CLIENT_VAP_ID);
        if (status != SL_STATUS_OK) {
          printf("IPv4 address configuration is failed : 0x%lx\r\n", status);
          return;
        }

        printf("IPv4 address configuration complete\r\n");
        memcpy(&ip_address.ip.v4.bytes, &profile.ip.ip.v4.ip_address.bytes, sizeof(sl_ipv4_address_t));
        printf("Client IPv4: ");
        print_sl_ipv4_address(&ip_address.ip.v4);
        printf("\r\n");

        server_config.port            = HTTP_SERVER_PORT;
        server_config.default_handler = default_handler;
        server_config.handlers_list   = (sl_http_server_handler_t *)sensor_server_request_handlers;
        server_config.handlers_count  = sizeof(sensor_server_request_handlers) / sizeof(sl_http_server_handler_t);

        status = sl_http_server_init(&server_handle, &server_config);
        if (status != SL_STATUS_OK) {
          printf("HTTP server init failed:%lx\r\n", status);
          return;
        }
        status = sl_http_server_start(&server_handle);
        if (status != SL_STATUS_OK) {
          printf("Server start fail:%lx\r\n", status);
          return;
        }
        printf("Sensor HTTP server started\r\n");

        printf("\r\nThe sensor demo is now connected to your Wi-Fi network\r\n");
        printf("Go to http://%u.%u.%u.%u/ on your browser to view the sensor data\r\n",
               profile.ip.ip.v4.ip_address.bytes[0],
               profile.ip.ip.v4.ip_address.bytes[1],
               profile.ip.ip.v4.ip_address.bytes[2],
               profile.ip.ip.v4.ip_address.bytes[3]);
        app_state = CONNECTED_STATE;
        status_led_update_state();
        break;
      }
      case DISCONNECTING_STATE: {
        status = sl_http_server_stop(&server_handle);
        if (status != SL_STATUS_OK) {
          printf("Server stop fail:%lx\r\n", status);
          return;
        }
        status = sl_http_server_deinit(&server_handle);
        if (status != SL_STATUS_OK) {
          printf("Server deinit fail:%lx\r\n", status);
          return;
        }
        printf("Sensor HTTP Server deinitialized\r\n");

        status = sl_net_deinit(SL_NET_WIFI_CLIENT_INTERFACE);
        if (status != SL_STATUS_OK) {
          printf("Wi-Fi client deinit failed: 0x%lx\r\n", status);
          return;
        }
        printf("Wi-Fi client deinitialized\r\n");
        app_state = PROVISIONING_INIT_STATE;
        break;
      }
      case PROVISIONING_STATE:
      case CONNECTED_STATE: {
        osDelay(1000);
        break;
      }
    }
  }
  while (1) {
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
    // Let the CPU go to sleep if the system allows it.
    sl_power_manager_sleep();
#else
    osDelay(osWaitForever);
#endif
  }
}

static sl_status_t ap_connected_event_handler(sl_wifi_event_t event, void *data, uint32_t data_length, void *arg)
{
  UNUSED_PARAMETER(data_length);
  UNUSED_PARAMETER(arg);
  UNUSED_PARAMETER(event);

  printf("Remote Client connected: ");
  print_mac_address((sl_mac_address_t *)data);
  printf("\r\n");
  return SL_STATUS_OK;
}

static sl_status_t ap_disconnected_event_handler(sl_wifi_event_t event, void *data, uint32_t data_length, void *arg)
{
  UNUSED_PARAMETER(data_length);
  UNUSED_PARAMETER(arg);
  UNUSED_PARAMETER(event);

  printf("Remote Client disconnected: ");
  print_mac_address((sl_mac_address_t *)data);
  printf("\r\n");

  return SL_STATUS_OK;
}

static sl_status_t index_request_handler(sl_http_server_t *handle, sl_http_server_request_t *req)
{
  sl_http_server_response_t http_response = DEFAULT_HTTP_RESPONSE_METHOD_NOT_ALLOWED;

  printf("Got request %s with data length : %lu\r\n", req->uri.path, req->request_data_length);

  if (req->type == SL_HTTP_REQUEST_GET) {
    http_response.response_code        = SL_HTTP_RESPONSE_OK;
    http_response.content_type         = SL_HTTP_CONTENT_TYPE_TEXT_HTML;
    http_response.data                 = (uint8_t *)wifi_provisioning_content;
    http_response.current_data_length  = strlen((const char *)wifi_provisioning_content);
    http_response.expected_data_length = http_response.current_data_length;
  }
  sl_http_server_send_response(handle, &http_response);
  return SL_STATUS_OK;
}

static sl_status_t connect_page_request_handler(sl_http_server_t *handle, sl_http_server_request_t *req)
{
  sl_http_server_response_t http_response = DEFAULT_HTTP_RESPONSE_METHOD_NOT_ALLOWED;

  printf("Got request %s with data length : %lu\r\n", req->uri.path, req->request_data_length);

  if (req->type == SL_HTTP_REQUEST_GET) {
    http_response.response_code        = SL_HTTP_RESPONSE_OK;
    http_response.content_type         = SL_HTTP_CONTENT_TYPE_TEXT_HTML;
    http_response.data                 = (uint8_t *)login_content;
    http_response.current_data_length  = strlen((const char *)login_content);
    http_response.expected_data_length = http_response.current_data_length;
  }
  sl_http_server_send_response(handle, &http_response);
  return SL_STATUS_OK;
}

static sl_status_t light_request_handler(sl_http_server_t *handle, sl_http_server_request_t *req)
{
  sl_http_server_response_t http_response = DEFAULT_HTTP_RESPONSE_METHOD_NOT_ALLOWED;

  printf("Got request %s with data length : %lu\r\n", req->uri.path, req->request_data_length);

  if (req->type == SL_HTTP_REQUEST_GET) {
    char response_data[100] = { 0 };
    float ambient_light_lux = 0.0f;
    float white_light_lux   = 0.0f;
    ambient_light_read(&ambient_light_lux);
    white_light_read(&white_light_lux);

    snprintf(response_data, sizeof(response_data) - 1, LIGHT_JSON_RESPONSE, ambient_light_lux, white_light_lux);
    http_response.response_code        = SL_HTTP_RESPONSE_OK;
    http_response.content_type         = SL_HTTP_CONTENT_TYPE_APPLICATION_JSON;
    http_response.data                 = (uint8_t *)response_data;
    http_response.current_data_length  = strlen(response_data);
    http_response.expected_data_length = http_response.current_data_length;
  }
  sl_http_server_send_response(handle, &http_response);
  return SL_STATUS_OK;
}

static sl_status_t led_request_handler(sl_http_server_t *handle, sl_http_server_request_t *req)
{
  sl_http_server_response_t http_response = DEFAULT_HTTP_RESPONSE_METHOD_NOT_ALLOWED;
  char response_data[100]                 = { 0 };
  bool rgb_states[RGB_LED_MAX]            = { false };

  printf("Got request %s with data length : %lu\r\n", req->uri.path, req->request_data_length);

  switch (req->type) {
    case SL_HTTP_REQUEST_POST: {
      char received_data_buffer[200]        = { 0 };
      sl_http_recv_req_data_t received_data = { 0 };
      jsmn_parser parser;
      jsmntok_t tokens[10];
      bool rgb_found[RGB_LED_MAX] = { false };

      if (status_led_on == true) {
        sl_http_server_response_t http_response = DEFAULT_HTTP_RESPONSE_METHOD_NOT_ALLOWED;
        sl_http_server_send_response(handle, &http_response);
        return SL_STATUS_OK;
      }

      received_data.request       = req;
      received_data.buffer        = (uint8_t *)received_data_buffer;
      received_data.buffer_length = sizeof(received_data_buffer) - 1;
      sl_http_server_read_request_data(handle, &received_data);

      jsmn_init(&parser);

      memset(&tokens, 0, sizeof(tokens));
      jsmn_parse(&parser, received_data_buffer, req->request_data_length, tokens, sizeof(tokens) / sizeof(jsmntok_t));
      for (uint8_t a = 1; a < RGB_LED_MAX * 2 + 1; a += 2) {
        if (tokens[a].type == JSMN_STRING) {
          for (uint8_t b = 0; b < 3; b++) {
            if (memcmp(&received_data_buffer[tokens[a].start], rgb_colours[b], tokens[a].end - tokens[a].start) == 0) {
              if (tokens[a + 1].type == JSMN_STRING) {
                if (memcmp(&received_data_buffer[tokens[a + 1].start], ON, tokens[a + 1].end - tokens[a + 1].start)
                    == 0) {
                  rgb_found[b]  = true;
                  rgb_states[b] = true;
                } else if (memcmp(&received_data_buffer[tokens[a + 1].start],
                                  OFF,
                                  tokens[a + 1].end - tokens[a + 1].start)
                           == 0) {
                  rgb_found[b]  = true;
                  rgb_states[b] = false;
                }
              }
            }
          }
        }
      }
      if (rgb_found[RGB_LED_RED] == false || rgb_found[RGB_LED_GREEN] == false || rgb_found[RGB_LED_BLUE] == false) {
        http_response.response_code        = SL_HTTP_RESPONSE_BAD_REQUEST;
        http_response.data                 = (uint8_t *)METHOD_BAD_REQUEST;
        http_response.current_data_length  = sizeof(METHOD_BAD_REQUEST) - 1;
        http_response.expected_data_length = http_response.current_data_length;
        break;
      } else {
        rgb_led_set_state(rgb_states[RGB_LED_RED], rgb_states[RGB_LED_GREEN], rgb_states[RGB_LED_BLUE]);
      }
    } // fall-through
    // @suppress("No break at end of case")
    case SL_HTTP_REQUEST_GET: {
      rgb_led_get_state(&rgb_states[RGB_LED_RED], &rgb_states[RGB_LED_GREEN], &rgb_states[RGB_LED_BLUE]);
      snprintf(response_data,
               sizeof(response_data) - 1,
               LED_JSON_RESPONSE,
               (rgb_states[RGB_LED_RED] == true) ? ON : OFF,
               (rgb_states[RGB_LED_GREEN] == true) ? ON : OFF,
               (rgb_states[RGB_LED_BLUE] == true) ? ON : OFF);
      http_response.response_code        = SL_HTTP_RESPONSE_OK;
      http_response.content_type         = SL_HTTP_CONTENT_TYPE_APPLICATION_JSON;
      http_response.data                 = (uint8_t *)response_data;
      http_response.current_data_length  = strlen(response_data);
      http_response.expected_data_length = http_response.current_data_length;
      break;
    }
    default:
      break;
  }
  sl_http_server_send_response(handle, &http_response);
  return SL_STATUS_OK;
}

static sl_status_t temperature_request_handler(sl_http_server_t *handle, sl_http_server_request_t *req)
{
  sl_http_server_response_t http_response = DEFAULT_HTTP_RESPONSE_METHOD_NOT_ALLOWED;

  printf("Got request %s with data length : %lu\r\n", req->uri.path, req->request_data_length);

  if (req->type == SL_HTTP_REQUEST_GET) {
    char response_data[100] = { 0 };
    float temperature       = 0.0f;

    temperature_read(&temperature);

    snprintf(response_data, sizeof(response_data) - 1, TEMPERATURE_JSON_RESPONSE, temperature);
    http_response.response_code        = SL_HTTP_RESPONSE_OK;
    http_response.content_type         = SL_HTTP_CONTENT_TYPE_APPLICATION_JSON;
    http_response.data                 = (uint8_t *)response_data;
    http_response.current_data_length  = strlen(response_data);
    http_response.expected_data_length = http_response.current_data_length;
  }
  sl_http_server_send_response(handle, &http_response);
  return SL_STATUS_OK;
}

static sl_status_t accelerometer_request_handler(sl_http_server_t *handle, sl_http_server_request_t *req)
{
  sl_http_server_response_t http_response = DEFAULT_HTTP_RESPONSE_METHOD_NOT_ALLOWED;

  if (req->type == SL_HTTP_REQUEST_GET) {
    char response_data[100] = { 0 };
    float x                 = 0.0f;
    float y                 = 0.0f;
    float z                 = 0.0f;

    printf("Got request %s with data length : %lu\r\n", req->uri.path, req->request_data_length);

    accelerometer_read(&x, &y, &z);

    snprintf(response_data, sizeof(response_data) - 1, MOTION_SENSOR_JSON_RESPONSE, x, y, z);
    http_response.response_code        = SL_HTTP_RESPONSE_OK;
    http_response.content_type         = SL_HTTP_CONTENT_TYPE_APPLICATION_JSON;
    http_response.data                 = (uint8_t *)response_data;
    http_response.current_data_length  = strlen(response_data);
    http_response.expected_data_length = http_response.current_data_length;
  }
  sl_http_server_send_response(handle, &http_response);
  return SL_STATUS_OK;
}

static sl_status_t gyroscope_request_handler(sl_http_server_t *handle, sl_http_server_request_t *req)
{
  sl_http_server_response_t http_response = DEFAULT_HTTP_RESPONSE_METHOD_NOT_ALLOWED;

  printf("Got request %s with data length : %lu\r\n", req->uri.path, req->request_data_length);

  if (req->type == SL_HTTP_REQUEST_GET) {
    char response_data[100] = { 0 };
    float x                 = 0.0f;
    float y                 = 0.0f;
    float z                 = 0.0f;

    gyro_read(&x, &y, &z);

    snprintf(response_data, sizeof(response_data) - 1, MOTION_SENSOR_JSON_RESPONSE, x, y, z);
    http_response.response_code        = SL_HTTP_RESPONSE_OK;
    http_response.content_type         = SL_HTTP_CONTENT_TYPE_APPLICATION_JSON;
    http_response.data                 = (uint8_t *)response_data;
    http_response.current_data_length  = strlen(response_data);
    http_response.expected_data_length = http_response.current_data_length;
  }
  sl_http_server_send_response(handle, &http_response);
  return SL_STATUS_OK;
}

static sl_status_t humidity_request_handler(sl_http_server_t *handle, sl_http_server_request_t *req)
{
  sl_http_server_response_t http_response = DEFAULT_HTTP_RESPONSE_METHOD_NOT_ALLOWED;

  printf("Got request %s with data length : %lu\r\n", req->uri.path, req->request_data_length);

  if (req->type == SL_HTTP_REQUEST_GET) {
    char response_data[100] = { 0 };
    uint32_t humidity       = 0;

    humidity_read(&humidity);

    snprintf(response_data, sizeof(response_data) - 1, HUMIDITY_JSON_RESPONSE, humidity);
    http_response.response_code        = SL_HTTP_RESPONSE_OK;
    http_response.content_type         = SL_HTTP_CONTENT_TYPE_APPLICATION_JSON;
    http_response.data                 = (uint8_t *)response_data;
    http_response.current_data_length  = strlen(response_data);
    http_response.expected_data_length = http_response.current_data_length;
  }
  sl_http_server_send_response(handle, &http_response);
  return SL_STATUS_OK;
}

static sl_status_t microphone_request_handler(sl_http_server_t *handle, sl_http_server_request_t *req)
{
  sl_http_server_response_t http_response = DEFAULT_HTTP_RESPONSE_METHOD_NOT_ALLOWED;

  printf("Got request %s with data length : %lu\r\n", req->uri.path, req->request_data_length);

  if (req->type == SL_HTTP_REQUEST_GET) {
    char response_data[100]     = { 0 };
    uint32_t microphone_decibel = 0;

    // TODO: add call to microphone read

    snprintf(response_data, sizeof(response_data) - 1, MICROPHONE_JSON_RESPONSE, microphone_decibel);
    http_response.response_code        = SL_HTTP_RESPONSE_OK;
    http_response.content_type         = SL_HTTP_CONTENT_TYPE_APPLICATION_JSON;
    http_response.data                 = (uint8_t *)response_data;
    http_response.current_data_length  = strlen(response_data);
    http_response.expected_data_length = http_response.current_data_length;
  }
  sl_http_server_send_response(handle, &http_response);
  return SL_STATUS_OK;
}

static sl_status_t provisioning_request_handler(sl_http_server_t *handle, sl_http_server_request_t *req)
{
  sl_http_server_response_t http_response = DEFAULT_HTTP_RESPONSE_METHOD_NOT_ALLOWED;

  printf("Got request %s with data length : %lu\r\n", req->uri.path, req->request_data_length);

  if (req->type == SL_HTTP_REQUEST_POST) {
    char response_data[100] = { 0 };

    snprintf(response_data, sizeof(response_data) - 1, "{\"status\": \"ok\"}");
    http_response.response_code        = SL_HTTP_RESPONSE_OK;
    http_response.content_type         = SL_HTTP_CONTENT_TYPE_APPLICATION_JSON;
    http_response.data                 = (uint8_t *)response_data;
    http_response.current_data_length  = strlen(response_data);
    http_response.expected_data_length = http_response.current_data_length;
    sl_http_server_send_response(handle, &http_response);
    if (app_state == CONNECTED_STATE) {
      app_state = DISCONNECTING_STATE;
    }
  } else {
    sl_http_server_send_response(handle, &http_response);
  }
  return SL_STATUS_OK;
}

static sl_status_t connect_data_handler(sl_http_server_t *handle, sl_http_server_request_t *req)
{
  sl_http_server_response_t http_response = DEFAULT_HTTP_RESPONSE_METHOD_NOT_ALLOWED;

  printf("Got request %s with data length : %lu\r\n", req->uri.path, req->request_data_length);

  if (req->type == SL_HTTP_REQUEST_POST) {
    char received_data_buffer[200]        = { 0 };
    sl_http_recv_req_data_t received_data = { 0 };
    char response_data[100]               = { 0 };
    jsmn_parser parser;
    jsmntok_t tokens[10];
    bool ssid_found          = false;
    bool security_type_found = false;
    bool passphrase_found    = false;

    received_data.request       = req;
    received_data.buffer        = (uint8_t *)received_data_buffer;
    received_data.buffer_length = sizeof(received_data_buffer) - 1;
    sl_http_server_read_request_data(handle, &received_data);

    jsmn_init(&parser);

    memset(&provisioned_access_point, 0, sizeof(provisioned_access_point));
    provisioned_access_point.encryption    = SL_WIFI_CCMP_ENCRYPTION;
    provisioned_access_point.credential_id = SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID;

    memset(&tokens, 0, sizeof(tokens));
    jsmn_parse(&parser, received_data_buffer, req->request_data_length, tokens, sizeof(tokens) / sizeof(jsmntok_t));

    for (uint8_t a = 1; a < 7; a += 2) {
      if (tokens[a].type == JSMN_STRING) {
        if (memcmp(&received_data_buffer[tokens[a].start], SSID, tokens[a].end - tokens[a].start) == 0) {
          if (tokens[a + 1].type == JSMN_STRING) {
            provisioned_access_point.ssid.length = tokens[a + 1].end - tokens[a + 1].start;
            if (provisioned_access_point.ssid.length <= sizeof(provisioned_access_point.ssid.value)) {
              ssid_found = true;
              memcpy(provisioned_access_point.ssid.value,
                     &received_data_buffer[tokens[a + 1].start],
                     provisioned_access_point.ssid.length);
            } else {
              printf("SSID is too long. Max length is 32.\r\n");
            }
          }
        } else if (memcmp(&received_data_buffer[tokens[a].start], PASSPHRASE, tokens[a].end - tokens[a].start) == 0) {
          if (tokens[a].type == JSMN_STRING) {
            sl_status_t status = sl_net_set_credential(SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID,
                                                       SL_NET_WIFI_PSK,
                                                       &received_data_buffer[tokens[a + 1].start],
                                                       tokens[a + 1].end - tokens[a + 1].start);
            if (status == SL_STATUS_OK) {
              passphrase_found = true;
            }
          }
        } else if (memcmp(&received_data_buffer[tokens[a].start], SECURITY_TYPE, tokens[a].end - tokens[a].start)
                   == 0) {
          char security_type[20] = { 0 };

          strncpy(security_type, &received_data_buffer[tokens[a + 1].start], tokens[a + 1].end - tokens[a + 1].start);
          provisioned_access_point.security = string_to_security_type(security_type);
          if (provisioned_access_point.security != SL_WIFI_SECURITY_UNKNOWN) {
            security_type_found = true;
          }
        }
      }
    }

    if (ssid_found == false || passphrase_found == false || security_type_found == false) {
      http_response.response_code        = SL_HTTP_RESPONSE_BAD_REQUEST;
      http_response.data                 = (uint8_t *)METHOD_BAD_REQUEST;
      http_response.current_data_length  = sizeof(METHOD_BAD_REQUEST) - 1;
      http_response.expected_data_length = http_response.current_data_length;
      sl_http_server_send_response(handle, &http_response);
    } else {
      snprintf(response_data, sizeof(response_data) - 1, "{\"status\": \"ok\"}");
      http_response.response_code        = SL_HTTP_RESPONSE_OK;
      http_response.content_type         = SL_HTTP_CONTENT_TYPE_APPLICATION_JSON;
      http_response.data                 = (uint8_t *)response_data;
      http_response.current_data_length  = strlen(response_data);
      http_response.expected_data_length = http_response.current_data_length;
      sl_http_server_send_response(handle, &http_response);
      app_state = CONNECTING_STATE;
    }
  } else {
    sl_http_server_send_response(handle, &http_response);
  }
  return SL_STATUS_OK;
}

static sl_status_t wlan_app_scan_callback_handler(sl_wifi_event_t event,
                                                  sl_wifi_scan_result_t *scan_result,
                                                  uint32_t result_length,
                                                  void *arg)
{
  char *scan_result_buffer = (char *)arg;
  uint8_t *bssid           = NULL;
  UNUSED_PARAMETER(arg);
  UNUSED_PARAMETER(result_length);

  if (SL_WIFI_CHECK_IF_EVENT_FAILED(event)) {
    callback_status = *(sl_status_t *)scan_result;
    return SL_STATUS_FAIL;
  }

  if (scan_result->scan_count) {
    uint32_t buffer_length = SCAN_RESULT_BUFFER_SIZE - 1;
    int32_t index =
      snprintf(scan_result_buffer, buffer_length, "{\"count\": \"%lu\", \"scan_results\": [", scan_result->scan_count);
    scan_result_buffer += index;
    buffer_length -= index;

    for (uint32_t a = 0; a < scan_result->scan_count; a++) {
      bssid = (uint8_t *)&scan_result->scan_info[a].bssid;
      index = snprintf(scan_result_buffer,
                       buffer_length,
                       "{\"ssid\": \"%s\", \"security_type\": \"%s\", \"network_type\": \"%u\",",
                       scan_result->scan_info[a].ssid,
                       security_type_to_string(scan_result->scan_info[a].security_mode),
                       scan_result->scan_info[a].network_type);
      scan_result_buffer += index;
      buffer_length -= index;
      index = snprintf(scan_result_buffer,
                       buffer_length,
                       " \"bssid\": \"%02x:%02x:%02x:%02x:%02x:%02x\", \"channel\": \"%u\", \"rssi\": \"-%u\"}",
                       bssid[0],
                       bssid[1],
                       bssid[2],
                       bssid[3],
                       bssid[4],
                       bssid[5],
                       scan_result->scan_info[a].rf_channel,
                       scan_result->scan_info[a].rssi_val);
      scan_result_buffer += index;
      buffer_length -= index;
      if (a < scan_result->scan_count - 1) {
        index = snprintf(scan_result_buffer, buffer_length, ",");
        scan_result_buffer += index;
        buffer_length -= index;
      }
    }
    index = snprintf(scan_result_buffer, buffer_length, "]}");
    scan_result_buffer += index;
    buffer_length -= index;
  }

  scan_complete = true;
  return SL_STATUS_OK;
}

static sl_status_t wifi_scan_request_handler(sl_http_server_t *handle, sl_http_server_request_t *req)
{
  sl_http_server_response_t http_response = DEFAULT_HTTP_RESPONSE_METHOD_NOT_ALLOWED;

  if (req->type == SL_HTTP_REQUEST_GET) {
    sl_status_t status;
    char *scan_result_buffer                             = (char *)malloc(SCAN_RESULT_BUFFER_SIZE);
    sl_wifi_scan_configuration_t wifi_scan_configuration = default_wifi_scan_configuration;

    printf("Got request %s with data length : %lu\r\n", req->uri.path, req->request_data_length);
    memset(scan_result_buffer, 0, SCAN_RESULT_BUFFER_SIZE);

    printf("WLAN scan started \r\n");
    scan_complete = false;
    sl_wifi_set_scan_callback(wlan_app_scan_callback_handler, (void *)scan_result_buffer);
    status = sl_wifi_start_scan(SL_WIFI_AP_INTERFACE, NULL, &wifi_scan_configuration);
    if (SL_STATUS_IN_PROGRESS == status) {
      printf("Scanning...\r\n");
      const uint32_t start = osKernelGetTickCount();

      while (!scan_complete && (osKernelGetTickCount() - start) <= 10000) {
        osThreadYield();
      }
      status = scan_complete ? callback_status : SL_STATUS_TIMEOUT;
    }
    if (status != RSI_SUCCESS) {
      printf("WLAN Scan failed %lx. Please make sure the latest connectivity firmware is used.\r\n", status);
      http_response.response_code        = SL_HTTP_RESPONSE_INTERNAL_SERVER_ERROR;
      http_response.data                 = (uint8_t *)METHOD_INTERNAL_SERVER_ERROR;
      http_response.current_data_length  = sizeof(METHOD_INTERNAL_SERVER_ERROR) - 1;
      http_response.expected_data_length = http_response.current_data_length;

    } else {
      printf("Scan done state \r\n");
      http_response.response_code        = SL_HTTP_RESPONSE_OK;
      http_response.content_type         = SL_HTTP_CONTENT_TYPE_APPLICATION_JSON;
      http_response.data                 = (uint8_t *)scan_result_buffer;
      http_response.current_data_length  = strlen(scan_result_buffer);
      http_response.expected_data_length = http_response.current_data_length;
    }
    sl_http_server_send_response(handle, &http_response);
    free(scan_result_buffer);
  } else {
    sl_http_server_send_response(handle, &http_response);
  }
  return SL_STATUS_OK;
}

static sl_status_t default_handler(sl_http_server_t *handle, sl_http_server_request_t *req)
{
  sl_http_server_response_t http_response = DEFAULT_HTTP_RESPONSE_METHOD_NOT_ALLOWED;
  sl_http_header_t header                 = { .key = "Server", .value = "SI917-HTTPServer" };

  UNUSED_PARAMETER(req);

  http_response.response_code = SL_HTTP_RESPONSE_NOT_FOUND;
  http_response.content_type  = SL_HTTP_CONTENT_TYPE_TEXT_PLAIN;
  http_response.headers       = &header;
  http_response.header_count  = 1;

  char *response_data                = "404 Not Found";
  http_response.data                 = (uint8_t *)response_data;
  http_response.current_data_length  = strlen(response_data);
  http_response.expected_data_length = http_response.current_data_length;
  sl_http_server_send_response(handle, &http_response);

  return SL_STATUS_OK;
}

static sl_status_t sensor_webserver_handler(sl_http_server_t *handle, sl_http_server_request_t *req)
{
  sl_http_server_response_t http_response = DEFAULT_HTTP_RESPONSE_METHOD_NOT_ALLOWED;

  printf("Got request %s with data length : %lu\r\n", req->uri.path, req->request_data_length);

  if (req->type == SL_HTTP_REQUEST_GET) {
    http_response.response_code        = SL_HTTP_RESPONSE_OK;
    http_response.content_type         = SL_HTTP_CONTENT_TYPE_TEXT_HTML;
    http_response.data                 = (uint8_t *)sensor_webserver_content;
    http_response.current_data_length  = strlen((const char *)sensor_webserver_content);
    http_response.expected_data_length = http_response.current_data_length;
  }
  sl_http_server_send_response(handle, &http_response);
  return SL_STATUS_OK;
}

static sl_status_t all_sensors_request_handler(sl_http_server_t *handle, sl_http_server_request_t *req)
{
  sl_http_server_response_t http_response = DEFAULT_HTTP_RESPONSE_METHOD_NOT_ALLOWED;

  if (req->type == SL_HTTP_REQUEST_GET) {
    char response_data[1000]         = { 0 };
    uint32_t remaining_buffer_length = sizeof(response_data) - 1;
    char *current_response_data      = response_data;
    uint32_t index                   = 0;
    bool red                         = false;
    bool green                       = false;
    bool blue                        = false;
    float temperature                = 0.0f;
    float ambient_light_lux          = 0.0f;
    float white_light_lux            = 0.0f;
    float x                          = 0.0f;
    float y                          = 0.0f;
    float z                          = 0.0f;
    uint32_t humidity                = 0;
    uint32_t microphone_decibel      = 0;

    printf("Got request %s with data length : %lu\r\n", req->uri.path, req->request_data_length);

    rgb_led_get_state(&red, &green, &blue);
    index = snprintf(current_response_data,
                     remaining_buffer_length,
                     "{\"led\": " LED_JSON_RESPONSE ",",
                     (red == true) ? ON : OFF,
                     (green == true) ? ON : OFF,
                     (blue == true) ? ON : OFF);
    current_response_data += index;
    remaining_buffer_length -= index;

    ambient_light_read(&ambient_light_lux);
    white_light_read(&white_light_lux);
    index = snprintf(current_response_data,
                     remaining_buffer_length,
                     "\"light\": " LIGHT_JSON_RESPONSE ",",
                     ambient_light_lux,
                     white_light_lux);
    current_response_data += index;
    remaining_buffer_length -= index;

    temperature_read(&temperature);
    index = snprintf(current_response_data,
                     remaining_buffer_length,
                     "\"temperature\": " TEMPERATURE_JSON_RESPONSE ",",
                     temperature);
    current_response_data += index;
    remaining_buffer_length -= index;

    accelerometer_read(&x, &y, &z);
    index = snprintf(current_response_data,
                     remaining_buffer_length,
                     "\"accelerometer\": " MOTION_SENSOR_JSON_RESPONSE ",",
                     x,
                     y,
                     z);
    current_response_data += index;
    remaining_buffer_length -= index;

    gyro_read(&x, &y, &z);
    index = snprintf(current_response_data,
                     remaining_buffer_length,
                     "\"gyroscope\": " MOTION_SENSOR_JSON_RESPONSE ",",
                     x,
                     y,
                     z);
    current_response_data += index;
    remaining_buffer_length -= index;

    humidity_read(&humidity);
    index =
      snprintf(current_response_data, remaining_buffer_length, "\"humidity\": " HUMIDITY_JSON_RESPONSE ",", humidity);
    current_response_data += index;
    remaining_buffer_length -= index;

    // TODO: add call to microphone read
    index = snprintf(current_response_data,
                     remaining_buffer_length,
                     "\"microphone\": " MICROPHONE_JSON_RESPONSE "}",
                     microphone_decibel);
    current_response_data += index;
    remaining_buffer_length -= index;

    http_response.response_code        = SL_HTTP_RESPONSE_OK;
    http_response.content_type         = SL_HTTP_CONTENT_TYPE_APPLICATION_JSON;
    http_response.data                 = (uint8_t *)response_data;
    http_response.current_data_length  = strlen(response_data);
    http_response.expected_data_length = http_response.current_data_length;
  }
  sl_http_server_send_response(handle, &http_response);
  return SL_STATUS_OK;
}

static sl_status_t status_led_request_handler(sl_http_server_t *handle, sl_http_server_request_t *req)
{
  sl_http_server_response_t http_response = DEFAULT_HTTP_RESPONSE_METHOD_NOT_ALLOWED;
  char response_data[100]                 = { 0 };

  printf("Got request %s with data length : %lu\r\n", req->uri.path, req->request_data_length);

  switch (req->type) {
    case SL_HTTP_REQUEST_POST: {
      char received_data_buffer[200]        = { 0 };
      sl_http_recv_req_data_t received_data = { 0 };
      jsmn_parser parser;
      jsmntok_t tokens[10];
      bool state_found = false;
      bool state       = false;

      received_data.request       = req;
      received_data.buffer        = (uint8_t *)received_data_buffer;
      received_data.buffer_length = sizeof(received_data_buffer) - 1;
      sl_http_server_read_request_data(handle, &received_data);

      jsmn_init(&parser);

      memset(&tokens, 0, sizeof(tokens));
      jsmn_parse(&parser, received_data_buffer, req->request_data_length, tokens, sizeof(tokens) / sizeof(jsmntok_t));
      if (tokens[1].type == JSMN_STRING) {
        if (memcmp(&received_data_buffer[tokens[1].start], "status_led", tokens[1].end - tokens[1].start) == 0) {
          if (tokens[2].type == JSMN_STRING) {
            if (memcmp(&received_data_buffer[tokens[2].start], ON, tokens[2].end - tokens[2].start) == 0) {
              state_found = true;
              state       = true;
            } else if (memcmp(&received_data_buffer[tokens[2].start], OFF, tokens[2].end - tokens[2].start) == 0) {
              state_found = true;
              state       = false;
            }
          }
        }
      }
      if (state_found == false) {
        http_response.response_code        = SL_HTTP_RESPONSE_BAD_REQUEST;
        http_response.data                 = (uint8_t *)METHOD_BAD_REQUEST;
        http_response.current_data_length  = sizeof(METHOD_BAD_REQUEST) - 1;
        http_response.expected_data_length = http_response.current_data_length;
        break;
      } else {
        status_led_set_state(state);
        status_led_update_state();
      }
    } // fall-through
    // @suppress("No break at end of case")
    case SL_HTTP_REQUEST_GET: {
      snprintf(response_data, sizeof(response_data) - 1, STATUS_LED_JSON_RESPONSE, (status_led_on == true) ? ON : OFF);
      http_response.response_code        = SL_HTTP_RESPONSE_OK;
      http_response.content_type         = SL_HTTP_CONTENT_TYPE_APPLICATION_JSON;
      http_response.data                 = (uint8_t *)response_data;
      http_response.current_data_length  = strlen(response_data);
      http_response.expected_data_length = http_response.current_data_length;
      break;
    }
    default:
      break;
  }
  sl_http_server_send_response(handle, &http_response);
  return SL_STATUS_OK;
}

static sl_wifi_security_t string_to_security_type(const char *security_type)
{
  if (strcmp(security_type, OPEN) == 0) {
    return SL_WIFI_OPEN;
  } else if (strcmp(security_type, WPA) == 0) {
    return SL_WIFI_WPA;
  } else if (strcmp(security_type, WPA2) == 0) {
    return SL_WIFI_WPA2;
  } else if (strcmp(security_type, WPA3) == 0) {
    return SL_WIFI_WPA3;
  } else if (strcmp(security_type, MIXED_MODE) == 0) {
    return SL_WIFI_WPA_WPA2_MIXED;
  } else {
    return SL_WIFI_SECURITY_UNKNOWN;
  }
}

static char *security_type_to_string(sl_wifi_security_t security_type)
{
  switch (security_type) {
    case SL_WIFI_OPEN:
      return OPEN;
    case SL_WIFI_WPA:
      return WPA;
    case SL_WIFI_WPA2:
      return WPA2;
    case SL_WIFI_WPA3:
      return WPA3;
    case SL_WIFI_WPA_WPA2_MIXED:
      return MIXED_MODE;
    default:
      return UNKNOWN;
  }
}
