#include <gtest/gtest.h>
extern "C" {
#include "sl_net.h"
#include "sl_net_constants.h"
#include "sl_net_si91x.h"
#include "sl_net_rsi_utility.h"
#include "sl_wifi.h"
#include "sl_wifi_device.h"
#include "sl_rsi_utility.h"
#include "sl_si91x_driver.h"
#include "sl_wifi_callback_framework.h"
#include "fff.h"
extern bool device_initialized;
}

DEFINE_FFF_GLOBALS;
FAKE_VALUE_FUNC2(sl_status_t, sl_wifi_default_event_handler, sl_wifi_event_t, sl_wifi_buffer_t *);
FAKE_VALUE_FUNC3(sl_status_t, sl_net_set_profile, sl_net_interface_t, sl_net_profile_id_t, const sl_net_profile_t *);
FAKE_VALUE_FUNC3(sl_status_t, sl_net_get_profile, sl_net_interface_t, sl_net_profile_id_t, sl_net_profile_t *);
FAKE_VALUE_FUNC3(sl_status_t,
                 sl_wifi_init,
                 const sl_wifi_device_configuration_t *,
                 sl_wifi_device_context_t *,
                 sl_wifi_event_handler_t);
FAKE_VALUE_FUNC(sl_status_t, sl_wifi_deinit);
FAKE_VALUE_FUNC1(bool, sl_wifi_is_interface_up, sl_wifi_interface_t);
FAKE_VALUE_FUNC3(sl_status_t, sl_wifi_connect, sl_wifi_interface_t, const sl_wifi_client_configuration_t *, uint32_t);
FAKE_VALUE_FUNC1(sl_status_t, sl_wifi_disconnect, sl_wifi_interface_t);
FAKE_VALUE_FUNC2(sl_status_t, sl_wifi_start_ap, sl_wifi_interface_t, const sl_wifi_ap_configuration_t *);
FAKE_VALUE_FUNC2(sl_status_t, sl_wifi_btr_up, sl_wifi_interface_t, sl_wifi_btr_configuration_t *);
FAKE_VALUE_FUNC1(sl_status_t, sl_wifi_stop_ap, sl_wifi_interface_t);
FAKE_VALUE_FUNC2(sl_status_t, sl_si91x_configure_ip_address, sl_net_ip_configuration_t *, uint8_t);
FAKE_VALUE_FUNC3(void *, sl_si91x_host_get_buffer_data, sl_wifi_buffer_t *, uint16_t, uint16_t *);
FAKE_VOID_FUNC2(sl_si91x_host_free_buffer, sl_wifi_buffer_t *, sl_wifi_buffer_type_t);
FAKE_VALUE_FUNC7(sl_status_t,
                 sl_si91x_driver_send_command,
                 uint32_t,
                 sl_si91x_queue_type_t,
                 const void *,
                 uint32_t,
                 sl_si91x_wait_period_t,
                 void *,
                 sl_wifi_buffer_t **);
FAKE_VALUE_FUNC1(sl_status_t, sl_si91x_register_event_handler, sl_net_event_handler_t);
FAKE_VALUE_FUNC2(sl_status_t, convert_si91x_dns_response, sl_ip_address_t *, sl_si91x_dns_response_t *);

TEST(network_manager, sl_net_init_unsupported_interfaces)
{
  sl_status_t status;

  status = sl_net_init((sl_net_interface_t)SL_NET_ETHERNET_INTERFACE, NULL, NULL, NULL);
  EXPECT_TRUE(status == SL_STATUS_NOT_SUPPORTED);
  status = sl_net_init((sl_net_interface_t)SL_NET_THREAD_INTERFACE, NULL, NULL, NULL);
  EXPECT_TRUE(status == SL_STATUS_NOT_SUPPORTED);
  status = sl_net_init((sl_net_interface_t)SL_NET_BLUETOOTH_INTERFACE, NULL, NULL, NULL);
  EXPECT_TRUE(status == SL_STATUS_NOT_SUPPORTED);
  status = sl_net_init((sl_net_interface_t)SL_NET_ZWAVE_INTERFACE, NULL, NULL, NULL);
  EXPECT_TRUE(status == SL_STATUS_NOT_SUPPORTED);
}

TEST(network_manager, sl_net_init_already_initialised)
{
  sl_status_t status;

  device_initialized           = true;
  sl_wifi_init_fake.return_val = SL_STATUS_ALREADY_INITIALIZED;

  status = sl_net_init((sl_net_interface_t)SL_NET_WIFI_CLIENT_INTERFACE, NULL, NULL, NULL);
  EXPECT_TRUE(status == SL_STATUS_ALREADY_INITIALIZED);
  status = sl_net_init((sl_net_interface_t)SL_NET_WIFI_AP_INTERFACE, NULL, NULL, NULL);
  EXPECT_TRUE(status == SL_STATUS_ALREADY_INITIALIZED);
}

TEST(network_manager, sl_net_deinit_unsupported_interfaces)
{
  sl_status_t status;

  status = sl_net_deinit((sl_net_interface_t)SL_NET_ETHERNET_INTERFACE);
  EXPECT_TRUE(status == SL_STATUS_NOT_SUPPORTED);
  status = sl_net_deinit((sl_net_interface_t)SL_NET_THREAD_INTERFACE);
  EXPECT_TRUE(status == SL_STATUS_NOT_SUPPORTED);
  status = sl_net_deinit((sl_net_interface_t)SL_NET_BLUETOOTH_INTERFACE);
  EXPECT_TRUE(status == SL_STATUS_NOT_SUPPORTED);
  status = sl_net_deinit((sl_net_interface_t)SL_NET_ZWAVE_INTERFACE);
  EXPECT_TRUE(status == SL_STATUS_NOT_SUPPORTED);
}

TEST(network_manager, sl_net_deinit_interface_not_up)
{
  sl_status_t status;

  sl_wifi_is_interface_up_fake.return_val = false;

  status = sl_net_deinit((sl_net_interface_t)SL_NET_WIFI_CLIENT_INTERFACE);
  EXPECT_TRUE(status == SL_STATUS_WIFI_INTERFACE_NOT_UP);
  status = sl_net_deinit((sl_net_interface_t)SL_NET_WIFI_AP_INTERFACE);
  EXPECT_TRUE(status == SL_STATUS_WIFI_INTERFACE_NOT_UP);
}

TEST(network_manager, sl_net_deinit_not_initialised)
{
  sl_status_t status;

  device_initialized                      = false;
  sl_wifi_is_interface_up_fake.return_val = true;
  sl_wifi_deinit_fake.return_val          = SL_STATUS_NOT_INITIALIZED;

  status = sl_net_deinit((sl_net_interface_t)SL_NET_WIFI_CLIENT_INTERFACE);
  EXPECT_TRUE(status == SL_STATUS_NOT_INITIALIZED);
  status = sl_net_deinit((sl_net_interface_t)SL_NET_WIFI_AP_INTERFACE);
  EXPECT_TRUE(status == SL_STATUS_NOT_INITIALIZED);
}

TEST(network_manager, sl_net_up_unsupported_interfaces)
{
  sl_status_t status;

  status = sl_net_up((sl_net_interface_t)SL_NET_ETHERNET_INTERFACE, SL_NET_DEFAULT_ETHERNET_PROFILE_ID);
  EXPECT_TRUE(status == SL_STATUS_NOT_SUPPORTED);
  status = sl_net_up((sl_net_interface_t)SL_NET_THREAD_INTERFACE, SL_NET_DEFAULT_THREAD_PROFILE_ID);
  EXPECT_TRUE(status == SL_STATUS_NOT_SUPPORTED);
  status = sl_net_up((sl_net_interface_t)SL_NET_BLUETOOTH_INTERFACE, SL_NET_PROFILE_ID_0);
  EXPECT_TRUE(status == SL_STATUS_NOT_SUPPORTED);
  status = sl_net_up((sl_net_interface_t)SL_NET_ZWAVE_INTERFACE, SL_NET_DEFAULT_ZWAVE_PROFILE_ID);
  EXPECT_TRUE(status == SL_STATUS_NOT_SUPPORTED);
}

TEST(network_manager, sl_net_up_wifi_client_interface_invalid_profile)
{
  sl_status_t status;

  sl_net_get_profile_reset();
  sl_net_get_profile_fake.return_val = SL_STATUS_INVALID_INDEX;

  status = sl_net_up((sl_net_interface_t)SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID);
  EXPECT_TRUE(sl_net_get_profile_fake.call_count == 1);
  EXPECT_TRUE(status == SL_STATUS_INVALID_INDEX);
  sl_net_get_profile_reset();
}

TEST(network_manager, sl_net_up_wifi_client_interface_failed_to_connect)
{
  sl_status_t status;

  sl_wifi_connect_reset();
  sl_wifi_connect_fake.return_val = SL_STATUS_NOT_INITIALIZED;

  status = sl_net_up((sl_net_interface_t)SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID);
  EXPECT_TRUE(sl_wifi_connect_fake.call_count == 1);
  EXPECT_TRUE(status == SL_STATUS_NOT_INITIALIZED);
  sl_wifi_connect_reset();
}

TEST(network_manager, sl_net_up_wifi_client_interface_failed_to_configure_ip_address)
{
  sl_status_t status;

  sl_si91x_configure_ip_address_reset();
  sl_si91x_configure_ip_address_fake.return_val = SL_STATUS_NOT_INITIALIZED;

  status = sl_net_up((sl_net_interface_t)SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID);
  EXPECT_TRUE(sl_si91x_configure_ip_address_fake.call_count == 1);
  EXPECT_TRUE(status == SL_STATUS_NOT_INITIALIZED);
  sl_si91x_configure_ip_address_reset();
}

TEST(network_manager, sl_net_up_wifi_client_interface_failed_to_set_profile)
{
  sl_status_t status;

  sl_net_set_profile_reset();
  sl_net_set_profile_fake.return_val = SL_STATUS_INVALID_INDEX;

  status = sl_net_up((sl_net_interface_t)SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID);
  EXPECT_TRUE(sl_net_set_profile_fake.call_count == 1);
  EXPECT_TRUE(status == SL_STATUS_INVALID_INDEX);
  sl_net_set_profile_reset();
}

TEST(network_manager, sl_net_up_wifi_client_interface_success)
{
  sl_status_t status;

  status = sl_net_up((sl_net_interface_t)SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID);
  EXPECT_TRUE(status == SL_STATUS_OK);
}

TEST(network_manager, sl_net_down_wifi_client_interface)
{
  sl_status_t status;

  status = sl_net_down((sl_net_interface_t)SL_NET_WIFI_CLIENT_INTERFACE);
  EXPECT_TRUE(sl_wifi_disconnect_fake.call_count == 1);
}

TEST(network_manager, sl_net_up_wifi_ap_interface_invalid_profile)
{
  sl_status_t status;

  sl_net_get_profile_reset();
  sl_net_get_profile_fake.return_val = SL_STATUS_INVALID_INDEX;

  status = sl_net_up((sl_net_interface_t)SL_NET_WIFI_AP_INTERFACE, SL_NET_DEFAULT_WIFI_AP_PROFILE_ID);
  EXPECT_TRUE(sl_net_get_profile_fake.call_count == 1);
  EXPECT_TRUE(status == SL_STATUS_INVALID_INDEX);
  sl_net_get_profile_reset();
}


TEST(network_manager, sl_net_down_wifi_ap_interface)
{
  sl_status_t status;

  status = sl_net_down((sl_net_interface_t)SL_NET_WIFI_AP_INTERFACE);
  EXPECT_TRUE(sl_wifi_stop_ap_fake.call_count == 1);
}

TEST(network_manager, sl_net_down_unsupported_interfaces)
{
  sl_status_t status;

  status = sl_net_down((sl_net_interface_t)SL_NET_ETHERNET_INTERFACE);
  EXPECT_TRUE(status == SL_STATUS_NOT_SUPPORTED);
  status = sl_net_down((sl_net_interface_t)SL_NET_THREAD_INTERFACE);
  EXPECT_TRUE(status == SL_STATUS_NOT_SUPPORTED);
  status = sl_net_down((sl_net_interface_t)SL_NET_BLUETOOTH_INTERFACE);
  EXPECT_TRUE(status == SL_STATUS_NOT_SUPPORTED);
  status = sl_net_down((sl_net_interface_t)SL_NET_ZWAVE_INTERFACE);
  EXPECT_TRUE(status == SL_STATUS_NOT_SUPPORTED);
}

TEST(network_manager, sl_net_inet_addr_check_null_args)
{
  sl_status_t status;
  const char *ip = "192.168.1.1";
  uint32_t value = 0;

  status = sl_net_inet_addr(NULL, &value);
  EXPECT_TRUE(status == SL_STATUS_INVALID_PARAMETER);
  status = sl_net_inet_addr(ip, NULL);
  EXPECT_TRUE(status == SL_STATUS_INVALID_PARAMETER);
}

TEST(network_manager, sl_net_inet_addr_check_ip_string_format)
{
  sl_status_t status;
  const char non_numeric[]    = "192.168.1.a";
  const char wrong_notation[] = "192.168.1:1";
  const char too_short[]      = "192.168.1";
  const char too_long[]       = "192.168.1.1.1";
  uint32_t value              = 0;

  status = sl_net_inet_addr(non_numeric, &value);
  EXPECT_TRUE(status == SL_STATUS_INVALID_PARAMETER);
  status = sl_net_inet_addr(wrong_notation, &value);
  EXPECT_TRUE(status == SL_STATUS_INVALID_PARAMETER);
  status = sl_net_inet_addr(too_short, &value);
  EXPECT_TRUE(status == SL_STATUS_INVALID_PARAMETER);
  status = sl_net_inet_addr(too_long, &value);
  EXPECT_TRUE(status == SL_STATUS_INVALID_PARAMETER);
}

TEST(network_manager, sl_net_inet_addr_check_value)
{
  sl_status_t status;
  const char ip1[]      = "192.168.1.1";
  const char ip2[]      = "10.10.10.1";
  uint32_t expected_ip1 = ((uint32_t)192 | (uint32_t)(168 << 8) | (uint32_t)(1 << 16) | (uint32_t)(1 << 24));
  uint32_t expected_ip2 = ((uint32_t)10 | (uint32_t)(10 << 8) | (uint32_t)(10 << 16) | (uint32_t)(1 << 24));
  uint32_t value        = 0;

  status = sl_net_inet_addr(ip1, &value);
  EXPECT_TRUE(status == SL_STATUS_OK);
  EXPECT_TRUE(value == expected_ip1);
  status = sl_net_inet_addr(ip2, &value);
  EXPECT_TRUE(status == SL_STATUS_OK);
  EXPECT_TRUE(value == expected_ip2);
}

TEST(network_manager, sl_net_join_multicast_address_uninitialised)
{
  sl_status_t status;
  sl_ip_address_t ip;
                                               
  ip.ip.v4.bytes[0] = 239;
  ip.ip.v4.bytes[1] = 1;
  ip.ip.v4.bytes[2] = 1;
  ip.ip.v4.bytes[3] = 1;
  ip.type = SL_IPV4;

  device_initialized = false;

  status = sl_net_join_multicast_address((sl_net_interface_t)SL_NET_WIFI_CLIENT_INTERFACE, (const sl_ip_address_t*)&ip);
  EXPECT_TRUE(status == SL_STATUS_NOT_INITIALIZED);
}

TEST(network_manager, sl_net_join_multicast_address_ipv4_success)
{
  sl_status_t status;
  sl_ip_address_t ip;
                                               
  ip.ip.v4.bytes[0] = 239;
  ip.ip.v4.bytes[1] = 1;
  ip.ip.v4.bytes[2] = 1;
  ip.ip.v4.bytes[3] = 1;
  ip.type = SL_IPV4;

  device_initialized = true;
  sl_si91x_driver_send_command_reset();

  status = sl_net_join_multicast_address((sl_net_interface_t)SL_NET_WIFI_CLIENT_INTERFACE, (const sl_ip_address_t*)&ip);
  EXPECT_TRUE(sl_si91x_driver_send_command_fake.call_count == 1);
  EXPECT_TRUE(status == SL_STATUS_OK);
}

TEST(network_manager, sl_net_join_multicast_address_ipv6_success)
{
  sl_status_t status;
  sl_ip_address_t ip;
                                               
  ip.ip.v6.bytes[0] = 0xff;
  ip.ip.v6.bytes[1] = 0x02;
  ip.ip.v6.bytes[2] = 0;
  ip.ip.v6.bytes[3] = 0;
  ip.ip.v6.bytes[4] = 0;
  ip.ip.v6.bytes[5] = 0;
  ip.ip.v6.bytes[6] = 0;
  ip.ip.v6.bytes[7] = 0;
  ip.ip.v6.bytes[8] = 0;
  ip.ip.v6.bytes[9] = 0;
  ip.ip.v6.bytes[10] = 0;
  ip.ip.v6.bytes[11] = 0;
  ip.ip.v6.bytes[12] = 0;
  ip.ip.v6.bytes[13] = 0;
  ip.ip.v6.bytes[14] = 0;
  ip.ip.v6.bytes[14] = 1;
  ip.type = SL_IPV6;

  device_initialized = true;
  sl_si91x_driver_send_command_reset();

  status = sl_net_join_multicast_address((sl_net_interface_t)SL_NET_WIFI_CLIENT_INTERFACE, (const sl_ip_address_t*)&ip);
  EXPECT_TRUE(sl_si91x_driver_send_command_fake.call_count == 1);
  EXPECT_TRUE(status == SL_STATUS_OK);
}

TEST(network_manager, sl_net_leave_multicast_address_uninitialised)
{
  sl_status_t status;
  sl_ip_address_t ip;
                                               
  ip.ip.v4.bytes[0] = 239;
  ip.ip.v4.bytes[1] = 1;
  ip.ip.v4.bytes[2] = 1;
  ip.ip.v4.bytes[3] = 1;
  ip.type = SL_IPV4;

  device_initialized = false;

  status = sl_net_leave_multicast_address((sl_net_interface_t)SL_NET_WIFI_CLIENT_INTERFACE, (const sl_ip_address_t*)&ip);
  EXPECT_TRUE(status == SL_STATUS_NOT_INITIALIZED);
}

// TEST(network_manager, sl_net_host_get_by_name_invalid_args)
// {
//   sl_status_t status;
//   sl_ip_address_t ip;
                                               
//   device_initialized = false;

//   status = sl_net_host_get_by_name(NULL, 10000, SL_NET_DNS_TYPE_IPV4, &ip)''
//   EXPECT_TRUE(status == SL_STATUS_NULL_POINTER);
// }
