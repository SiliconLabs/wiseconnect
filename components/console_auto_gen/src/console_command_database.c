#include "console.h"
#include "console_argument_types.h"



extern sl_status_t console_variable_get( console_args_t* arguments );
static const char* _get_arg_help[] = {
  0,

};
static const console_descriptive_command_t _get_command = { "Get",_get_arg_help, console_variable_get, { CONSOLE_ARG_STRING, CONSOLE_ARG_END } };

extern sl_status_t help_command_handler( console_args_t* arguments );
static const char* _help_arg_help[] = {

};
static const console_descriptive_command_t _help_command = { "Print help",_help_arg_help, help_command_handler, { CONSOLE_ARG_END } };

extern sl_status_t console_variable_list( console_args_t* arguments );
static const char* _list_arg_help[] = {

};
static const console_descriptive_command_t _list_command = { "List variables",_list_arg_help, console_variable_list, { CONSOLE_ARG_END } };

extern sl_status_t net_deinit_command_handler( console_args_t* arguments );
static const char* _net_deinit_arg_help[] = {
  0,

};
static const console_descriptive_command_t _net_deinit_command = { "Deinit the network interface",_net_deinit_arg_help, net_deinit_command_handler, { CONSOLE_ENUM_ARG(net_interface), CONSOLE_ARG_END } };

extern sl_status_t net_down_command_handler( console_args_t* arguments );
static const char* _net_down_arg_help[] = {
  0,

};
static const console_descriptive_command_t _net_down_command = { "Down the network interface",_net_down_arg_help, net_down_command_handler, { CONSOLE_ENUM_ARG(net_interface), CONSOLE_ARG_END } };

extern sl_status_t net_init_command_handler( console_args_t* arguments );
static const char* _net_init_arg_help[] = {
  0,

};
static const console_descriptive_command_t _net_init_command = { "Init the network interface",_net_init_arg_help, net_init_command_handler, { CONSOLE_ENUM_ARG(net_interface), CONSOLE_ARG_END } };

extern sl_status_t net_join_multicast_address_command_handler( console_args_t* arguments );
static const char* _net_join_multicast_address_arg_help[] = {
  0,
  0,

};
static const console_descriptive_command_t _net_join_multicast_address_command = { "Join multicast address",_net_join_multicast_address_arg_help, net_join_multicast_address_command_handler, { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(net_interface) ), CONSOLE_OPTIONAL_ARG('p', CONSOLE_ARG_STRING ), CONSOLE_ARG_END } };

extern sl_status_t net_leave_multicast_address_command_handler( console_args_t* arguments );
static const char* _net_leave_multicast_address_arg_help[] = {
  0,
  0,

};
static const console_descriptive_command_t _net_leave_multicast_address_command = { "Leave multicast address",_net_leave_multicast_address_arg_help, net_leave_multicast_address_command_handler, { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(net_interface) ), CONSOLE_OPTIONAL_ARG('p', CONSOLE_ARG_STRING ), CONSOLE_ARG_END } };

extern sl_status_t net_up_command_handler( console_args_t* arguments );
static const char* _net_up_arg_help[] = {
  0,
  0,

};
static const console_descriptive_command_t _net_up_command = { "Up the network interface",_net_up_arg_help, net_up_command_handler, { CONSOLE_ENUM_ARG(net_interface), CONSOLE_OPTIONAL_ARG('i', CONSOLE_ARG_UINT ), CONSOLE_ARG_END } };

extern sl_status_t rtt_command_handler( console_args_t* arguments );
static const char* _rtt_arg_help[] = {
  0,

};
static const console_descriptive_command_t _rtt_command = { "Print to RTT",_rtt_arg_help, rtt_command_handler, { CONSOLE_ARG_STRING, CONSOLE_ARG_END } };

extern sl_status_t console_variable_set( console_args_t* arguments );
static const char* _set_arg_help[] = {
  0,
  0,

};
static const console_descriptive_command_t _set_command = { "Set",_set_arg_help, console_variable_set, { CONSOLE_ARG_STRING, CONSOLE_ARG_STRING, CONSOLE_ARG_END } };

extern sl_status_t set_nvm_profile_command_handler( console_args_t* arguments );
static const char* _set_nvm_profile_arg_help[] = {
  0,
  0,

};
static const console_descriptive_command_t _set_nvm_profile_command = { "set profile and credential in nvm",_set_nvm_profile_arg_help, set_nvm_profile_command_handler, { CONSOLE_ENUM_ARG(net_interface), CONSOLE_OPTIONAL_ARG('i', CONSOLE_ARG_UINT ), CONSOLE_ARG_END } };

extern sl_status_t sl_dns_hostgetbyname_command_handler( console_args_t* arguments );
static const char* _sl_dns_host_get_by_name_arg_help[] = {
  0,
  0,
  0,

};
static const console_descriptive_command_t _sl_dns_host_get_by_name_command = { "DNS resolution",_sl_dns_host_get_by_name_arg_help, sl_dns_hostgetbyname_command_handler, { CONSOLE_ARG_STRING, CONSOLE_OPTIONAL_ARG('t', CONSOLE_ARG_UINT ), CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(sl_net_dns_resolution_ip_type_t) ), CONSOLE_ARG_END } };

extern sl_status_t sl_net_ping_command_handler( console_args_t* arguments );
static const char* _sl_net_ping_arg_help[] = {
  0,
  0,
  0,

};
static const console_descriptive_command_t _sl_net_ping_command = { "Send ping request",_sl_net_ping_arg_help, sl_net_ping_command_handler, { CONSOLE_ARG_STRING, CONSOLE_ENUM_ARG(sl_ip_address_type_t), CONSOLE_ARG_UINT, CONSOLE_ARG_END } };

extern sl_status_t sl_si91x_get_ram_log_command_handler( console_args_t* arguments );
static const char* _sl_si91x_get_ram_log_arg_help[] = {
  0,
  0,

};
static const console_descriptive_command_t _sl_si91x_get_ram_log_command = { "read the mentioned length of data from firmware RAM",_sl_si91x_get_ram_log_arg_help, sl_si91x_get_ram_log_command_handler, { CONSOLE_OPTIONAL_ARG('a', CONSOLE_ARG_UINT ), CONSOLE_OPTIONAL_ARG('l', CONSOLE_ARG_UINT ), CONSOLE_ARG_END } };

extern sl_status_t start_dhcp_command_handler( console_args_t* arguments );
static const char* _start_dhcp_arg_help[] = {
  0,
  0,

};
static const console_descriptive_command_t _start_dhcp_command = { "Start dhcp and get ip address",_start_dhcp_arg_help, start_dhcp_command_handler, { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(sl_ip_address_type_t) ), CONSOLE_OPTIONAL_ARG('m', CONSOLE_ENUM_ARG(sl_ip_management_t) ), CONSOLE_ARG_END } };

extern sl_status_t thread_command_handler( console_args_t* arguments );
static const char* _thread_arg_help[] = {
  0,
  0,

};
static const console_descriptive_command_t _thread_command = { "Spawn thread",_thread_arg_help, thread_command_handler, { CONSOLE_ARG_UINT, CONSOLE_ARG_REMAINING_COMMAND_LINE, CONSOLE_ARG_END } };

extern sl_status_t sl_wifi_set_11ax_config_command_handler( console_args_t* arguments );
static const char* _wifi_11ax_config_arg_help[] = {
  0,

};
static const console_descriptive_command_t _wifi_11ax_config_command = { "Configure the 11ax for WiFi 6",_wifi_11ax_config_arg_help, sl_wifi_set_11ax_config_command_handler, { CONSOLE_ARG_UINT, CONSOLE_ARG_END } };

extern sl_status_t sl_wifi_assert_command_handler( console_args_t* arguments );
static const char* _wifi_assert_arg_help[] = {

};
static const console_descriptive_command_t _wifi_assert_command = { "Trigger an assert",_wifi_assert_arg_help, sl_wifi_assert_command_handler, { CONSOLE_ARG_END } };

extern sl_status_t wifi_bsd_get_host_by_name_handler( console_args_t* arguments );
static const char* _wifi_bsd_get_host_by_name_arg_help[] = {
  0,

};
static const console_descriptive_command_t _wifi_bsd_get_host_by_name_command = { "BSD socket listen, \n parameters is \n 1 - hostName \n",_wifi_bsd_get_host_by_name_arg_help, wifi_bsd_get_host_by_name_handler, { CONSOLE_ARG_STRING, CONSOLE_ARG_END } };

extern sl_status_t wifi_bsd_get_peer_name( console_args_t* arguments );
static const char* _wifi_bsd_get_peer_name_arg_help[] = {
  0,

};
static const console_descriptive_command_t _wifi_bsd_get_peer_name_command = { "BSD get remote node address \n parameter \n1 - socket_fd",_wifi_bsd_get_peer_name_arg_help, wifi_bsd_get_peer_name, { CONSOLE_ARG_INT, CONSOLE_ARG_END } };

extern sl_status_t wifi_bsd_get_sock_name( console_args_t* arguments );
static const char* _wifi_bsd_get_sock_name_arg_help[] = {
  0,

};
static const console_descriptive_command_t _wifi_bsd_get_sock_name_command = { "BSD get local address of socket \n parameter \n1 - socket_fd",_wifi_bsd_get_sock_name_arg_help, wifi_bsd_get_sock_name, { CONSOLE_ARG_INT, CONSOLE_ARG_END } };

extern sl_status_t wifi_bsd_socket_get_opt_handler( console_args_t* arguments );
static const char* _wifi_bsd_get_sock_opt_arg_help[] = {
  0,
  0,

};
static const console_descriptive_command_t _wifi_bsd_get_sock_opt_command = { "BSD get the socket options \n parameters\n 1- socket_fd 2- option level\n",_wifi_bsd_get_sock_opt_arg_help, wifi_bsd_socket_get_opt_handler, { CONSOLE_ARG_INT, CONSOLE_ENUM_ARG(option_name), CONSOLE_ARG_END } };

extern sl_status_t wifi_bsd_socket_set_opt_handler( console_args_t* arguments );
static const char* _wifi_bsd_set_sock_opt_arg_help[] = {
  0,
  0,
  0,
  0,

};
static const console_descriptive_command_t _wifi_bsd_set_sock_opt_command = { "BSD get the socket options \n parameters\n 1- socket_fd 2- option name  3- option value  4- option length\n",_wifi_bsd_set_sock_opt_arg_help, wifi_bsd_socket_set_opt_handler, { CONSOLE_ARG_INT, CONSOLE_ENUM_ARG(option_name), CONSOLE_ARG_INT, CONSOLE_ARG_INT, CONSOLE_ARG_END } };

extern sl_status_t wifi_bsd_socket_accept_handler( console_args_t* arguments );
static const char* _wifi_bsd_socket_accept_arg_help[] = {
  0,

};
static const console_descriptive_command_t _wifi_bsd_socket_accept_command = { "BSD socket listen, \n parameters is \n 1 - sock_fd \n",_wifi_bsd_socket_accept_arg_help, wifi_bsd_socket_accept_handler, { CONSOLE_ARG_INT, CONSOLE_ARG_END } };

extern sl_status_t wifi_bsd_socket_bind_handler( console_args_t* arguments );
static const char* _wifi_bsd_socket_bind_arg_help[] = {
  0,
  0,
  0,
  0,

};
static const console_descriptive_command_t _wifi_bsd_socket_bind_command = { "BSD socket bind, \n parameters is \n 1 - sock_fd, \n 2 - ip_address, \n 3 - ip_address_len, \n 4 - port \n",_wifi_bsd_socket_bind_arg_help, wifi_bsd_socket_bind_handler, { CONSOLE_ARG_INT, CONSOLE_ARG_STRING, CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_END } };

extern sl_status_t wifi_bsd_socket_close_handler( console_args_t* arguments );
static const char* _wifi_bsd_socket_close_arg_help[] = {
  0,

};
static const console_descriptive_command_t _wifi_bsd_socket_close_command = { "BSD socket close, \n parameters is \n 1 - sock_fd \n",_wifi_bsd_socket_close_arg_help, wifi_bsd_socket_close_handler, { CONSOLE_ARG_INT, CONSOLE_ARG_END } };

extern sl_status_t wifi_bsd_socket_connect_handler( console_args_t* arguments );
static const char* _wifi_bsd_socket_connect_arg_help[] = {
  0,
  0,
  0,
  0,

};
static const console_descriptive_command_t _wifi_bsd_socket_connect_command = { "BSD socket connect, \n parameters are \n 1 - sock_fd, \n 2 - ip_address, \n 3 - ip_address_len, \n 4 - port \n",_wifi_bsd_socket_connect_arg_help, wifi_bsd_socket_connect_handler, { CONSOLE_ARG_INT, CONSOLE_ARG_STRING, CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_END } };

extern sl_status_t wifi_bsd_socket_create_handler( console_args_t* arguments );
static const char* _wifi_bsd_socket_create_arg_help[] = {
  0,
  0,
  0,

};
static const console_descriptive_command_t _wifi_bsd_socket_create_command = { "BSD socket creation, \n parameters are \n 1 - Domain, possible value: 1(AF_INET), 2(AF_INET6) .., \n 2 - Type, possible value: 1(SOCK_STREAM), 2(SOCK_DGRAM),  \n3 - protocol, possible value:1(IPPROTO_TCP), 2(IPPROTO_UDP).. \n",_wifi_bsd_socket_create_arg_help, wifi_bsd_socket_create_handler, { CONSOLE_ENUM_ARG(bsd_socket_family), CONSOLE_ENUM_ARG(bsd_socket_type), CONSOLE_ENUM_ARG(bsd_socket_protocol), CONSOLE_ARG_END } };

extern sl_status_t wifi_bsd_socket_listen_handler( console_args_t* arguments );
static const char* _wifi_bsd_socket_listen_arg_help[] = {
  0,
  0,

};
static const console_descriptive_command_t _wifi_bsd_socket_listen_command = { "BSD socket listen, \n parameters is \n 1 - sock_fd, \n 2 - backlog \n",_wifi_bsd_socket_listen_arg_help, wifi_bsd_socket_listen_handler, { CONSOLE_ARG_INT, CONSOLE_ARG_INT, CONSOLE_ARG_END } };

extern sl_status_t wifi_bsd_socket_receive_handler( console_args_t* arguments );
static const char* _wifi_bsd_socket_receive_arg_help[] = {
  0,

};
static const console_descriptive_command_t _wifi_bsd_socket_receive_command = { "BSD socket receive from, \n parameters is \n 1 - sock_fd \n",_wifi_bsd_socket_receive_arg_help, wifi_bsd_socket_receive_handler, { CONSOLE_ARG_INT, CONSOLE_ARG_END } };

extern sl_status_t wifi_bsd_socket_receive_from_handler( console_args_t* arguments );
static const char* _wifi_bsd_socket_receive_from_arg_help[] = {
  0,

};
static const console_descriptive_command_t _wifi_bsd_socket_receive_from_command = { "BSD socket receive from, \n parameters is \n 1 - sock_fd\n",_wifi_bsd_socket_receive_from_arg_help, wifi_bsd_socket_receive_from_handler, { CONSOLE_ARG_INT, CONSOLE_ARG_END } };

extern sl_status_t wifi_bsd_select_handler( console_args_t* arguments );
static const char* _wifi_bsd_socket_select_arg_help[] = {
  0,
  0,

};
static const console_descriptive_command_t _wifi_bsd_socket_select_command = { " nfd - number of file descriptors\n time - time in seconds",_wifi_bsd_socket_select_arg_help, wifi_bsd_select_handler, { CONSOLE_ARG_INT, CONSOLE_ARG_INT, CONSOLE_ARG_END } };

extern sl_status_t wifi_bsd_socket_send_handler( console_args_t* arguments );
static const char* _wifi_bsd_socket_send_arg_help[] = {
  0,
  0,

};
static const console_descriptive_command_t _wifi_bsd_socket_send_command = { "BSD socket send, \n parameters is \n 1 - sock_fd, \n 2 - data to be sent \n",_wifi_bsd_socket_send_arg_help, wifi_bsd_socket_send_handler, { CONSOLE_ARG_INT, CONSOLE_ARG_STRING, CONSOLE_ARG_END } };

extern sl_status_t wifi_bsd_socket_send_to_handler( console_args_t* arguments );
static const char* _wifi_bsd_socket_send_to_arg_help[] = {
  0,
  0,
  0,
  0,
  0,

};
static const console_descriptive_command_t _wifi_bsd_socket_send_to_command = { "BSD socket send, \n parameters is \n 1 - sock_fd, \n 2 - data to be sent, \n 3 - ip_address, \n 4 - ip_address_len (4 or 16 bytes), \n 5 - port \n",_wifi_bsd_socket_send_to_arg_help, wifi_bsd_socket_send_to_handler, { CONSOLE_ARG_INT, CONSOLE_ARG_STRING, CONSOLE_ARG_STRING, CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_END } };

extern sl_status_t wifi_connect_command_handler( console_args_t* arguments );
static const char* _wifi_connect_arg_help[] = {
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,

};
static const console_descriptive_command_t _wifi_connect_command = { "Start connecting to specified AP",_wifi_connect_arg_help, wifi_connect_command_handler, { CONSOLE_ARG_STRING, CONSOLE_ARG_STRING, CONSOLE_OPTIONAL_ARG('s', CONSOLE_ENUM_ARG(wifi_security) ), CONSOLE_OPTIONAL_ARG('u', CONSOLE_ARG_STRING ), CONSOLE_OPTIONAL_ARG('e', CONSOLE_ENUM_ARG(wifi_encryption) ), CONSOLE_OPTIONAL_ARG('t', CONSOLE_ARG_UINT ), CONSOLE_OPTIONAL_ARG('l', CONSOLE_ARG_UINT ), CONSOLE_OPTIONAL_ARG('c', CONSOLE_ARG_UINT ), CONSOLE_OPTIONAL_ARG('b', CONSOLE_ENUM_ARG(wifi_band) ), CONSOLE_OPTIONAL_ARG('w', CONSOLE_ENUM_ARG(wifi_bandwidth) ), CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(wifi_interface) ), CONSOLE_ARG_END } };

extern sl_status_t wifi_deinit_command_handler( console_args_t* arguments );
static const char* _wifi_deinit_arg_help[] = {

};
static const console_descriptive_command_t _wifi_deinit_command = { "Deinit Wi-Fi interface",_wifi_deinit_arg_help, wifi_deinit_command_handler, { CONSOLE_ARG_END } };

extern sl_status_t sl_wifi_disable_twt( console_args_t* arguments );
static const char* _wifi_disable_target_wake_time_arg_help[] = {

};
static const console_descriptive_command_t _wifi_disable_target_wake_time_command = { "Configures TWT parameters. Disables a TWT session. This is blocking API",_wifi_disable_target_wake_time_arg_help, sl_wifi_disable_twt, { CONSOLE_ARG_END } };

extern sl_status_t wifi_disconnect_command_handler( console_args_t* arguments );
static const char* _wifi_disconnect_arg_help[] = {
  0,

};
static const console_descriptive_command_t _wifi_disconnect_command = { "Disconnect",_wifi_disconnect_arg_help, wifi_disconnect_command_handler, { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(wifi_interface) ), CONSOLE_ARG_END } };

extern sl_status_t wifi_disconnect_ap_client_command_handler( console_args_t* arguments );
static const char* _wifi_disconnect_ap_client_arg_help[] = {
  0,
  0,

};
static const console_descriptive_command_t _wifi_disconnect_ap_client_command = { "Disconnect client connected to AP",_wifi_disconnect_ap_client_arg_help, wifi_disconnect_ap_client_command_handler, { CONSOLE_ARG_MAC_ADDRESS, CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(ap_client_deauth) ), CONSOLE_ARG_END } };

extern sl_status_t sl_wifi_enable_twt( console_args_t* arguments );
static const char* _wifi_enable_target_wake_time_arg_help[] = {

};
static const console_descriptive_command_t _wifi_enable_target_wake_time_command = { "Configures TWT parameters. Enables a TWT session. This is blocking API",_wifi_enable_target_wake_time_arg_help, sl_wifi_enable_twt, { CONSOLE_ARG_END } };

extern sl_status_t sl_wifi_filter_broadcast_command_handler( console_args_t* arguments );
static const char* _wifi_filter_broadcast_arg_help[] = {

};
static const console_descriptive_command_t _wifi_filter_broadcast_command = { "control the amount of broadcast and multicast data that is transmitted",_wifi_filter_broadcast_arg_help, sl_wifi_filter_broadcast_command_handler, { CONSOLE_ARG_END } };

extern sl_status_t wifi_generate_wps_pin_command_handler( console_args_t* arguments );
static const char* _wifi_generate_wps_pin_arg_help[] = {

};
static const console_descriptive_command_t _wifi_generate_wps_pin_command = { "Generate 8 digit wps pin",_wifi_generate_wps_pin_arg_help, wifi_generate_wps_pin_command_handler, { CONSOLE_ARG_END } };

extern sl_status_t sl_wifi_get_advanced_scan_configuration_command_handler( console_args_t* arguments );
static const char* _wifi_get_advanced_scan_configuration_arg_help[] = {

};
static const console_descriptive_command_t _wifi_get_advanced_scan_configuration_command = { "Get advanced scan configuration",_wifi_get_advanced_scan_configuration_arg_help, sl_wifi_get_advanced_scan_configuration_command_handler, { CONSOLE_ARG_END } };

extern sl_status_t wifi_get_ap_client_count_command_handler( console_args_t* arguments );
static const char* _wifi_get_ap_client_count_arg_help[] = {

};
static const console_descriptive_command_t _wifi_get_ap_client_count_command = { "Client count for get ap",_wifi_get_ap_client_count_arg_help, wifi_get_ap_client_count_command_handler, { CONSOLE_ARG_END } };

extern sl_status_t wifi_get_ap_client_list_command_handler( console_args_t* arguments );
static const char* _wifi_get_ap_client_list_arg_help[] = {

};
static const console_descriptive_command_t _wifi_get_ap_client_list_command = { "Client list for ap",_wifi_get_ap_client_list_arg_help, wifi_get_ap_client_list_command_handler, { CONSOLE_ARG_END } };

extern sl_status_t wifi_get_ap_configuration_command_handler( console_args_t* arguments );
static const char* _wifi_get_ap_configuration_arg_help[] = {
  0,

};
static const console_descriptive_command_t _wifi_get_ap_configuration_command = { "Get ap configuration",_wifi_get_ap_configuration_arg_help, wifi_get_ap_configuration_command_handler, { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(wifi_interface) ), CONSOLE_ARG_END } };

extern sl_status_t wifi_get_channel_command_handler( console_args_t* arguments );
static const char* _wifi_get_channel_arg_help[] = {
  0,

};
static const console_descriptive_command_t _wifi_get_channel_command = { "Fetch channel info",_wifi_get_channel_arg_help, wifi_get_channel_command_handler, { CONSOLE_ENUM_ARG(wifi_interface), CONSOLE_ARG_END } };

extern sl_status_t wifi_get_ap_client_info_command_handler( console_args_t* arguments );
static const char* _wifi_get_client_info_arg_help[] = {

};
static const console_descriptive_command_t _wifi_get_client_info_command = { "Fetch the client info that are currently connected to AP",_wifi_get_client_info_arg_help, wifi_get_ap_client_info_command_handler, { CONSOLE_ARG_END } };

extern sl_status_t wifi_get_default_interface_command_handler( console_args_t* arguments );
static const char* _wifi_get_default_interface_arg_help[] = {

};
static const console_descriptive_command_t _wifi_get_default_interface_command = { "Fetch default interface",_wifi_get_default_interface_arg_help, wifi_get_default_interface_command_handler, { CONSOLE_ARG_END } };

extern sl_status_t wifi_get_firmware_version_command_handler( console_args_t* arguments );
static const char* _wifi_get_fw_version_arg_help[] = {

};
static const console_descriptive_command_t _wifi_get_fw_version_command = { "Fetch firmware version",_wifi_get_fw_version_arg_help, wifi_get_firmware_version_command_handler, { CONSOLE_ARG_END } };

extern sl_status_t wifi_get_mac_address_command_handler( console_args_t* arguments );
static const char* _wifi_get_mac_address_arg_help[] = {
  0,

};
static const console_descriptive_command_t _wifi_get_mac_address_command = { "Fetch MAC address",_wifi_get_mac_address_arg_help, wifi_get_mac_address_command_handler, { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ARG_UINT ), CONSOLE_ARG_END } };

extern sl_status_t wifi_get_pairwise_master_key_command_handler( console_args_t* arguments );
static const char* _wifi_get_pairwise_master_key_arg_help[] = {
  0,
  0,
  0,
  0,

};
static const console_descriptive_command_t _wifi_get_pairwise_master_key_command = { "Generate PMK if PSK and SSID are provided This is a blocking API",_wifi_get_pairwise_master_key_arg_help, wifi_get_pairwise_master_key_command_handler, { CONSOLE_ARG_UINT, CONSOLE_ARG_STRING, CONSOLE_ARG_STRING, CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(wifi_interface) ), CONSOLE_ARG_END } };

extern sl_status_t wifi_get_performance_profile_command_handler( console_args_t* arguments );
static const char* _wifi_get_performance_profile_arg_help[] = {

};
static const console_descriptive_command_t _wifi_get_performance_profile_command = { "Get the current performance profile of the module",_wifi_get_performance_profile_arg_help, wifi_get_performance_profile_command_handler, { CONSOLE_ARG_END } };

extern sl_status_t wifi_get_signal_strength_command_handler( console_args_t* arguments );
static const char* _wifi_get_signal_strength_arg_help[] = {
  0,

};
static const console_descriptive_command_t _wifi_get_signal_strength_command = { "Queries signal strength",_wifi_get_signal_strength_arg_help, wifi_get_signal_strength_command_handler, { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(wifi_interface) ), CONSOLE_ARG_END } };

extern sl_status_t wifi_get_statistics_command_handler( console_args_t* arguments );
static const char* _wifi_get_statistics_arg_help[] = {

};
static const console_descriptive_command_t _wifi_get_statistics_command = { "Get wifi statistics",_wifi_get_statistics_arg_help, wifi_get_statistics_command_handler, { CONSOLE_ARG_END } };

extern sl_status_t wifi_get_transmit_rate_command_handler( console_args_t* arguments );
static const char* _wifi_get_transmit_rate_arg_help[] = {

};
static const console_descriptive_command_t _wifi_get_transmit_rate_command = { "Fetch configured rate protocol and data rate",_wifi_get_transmit_rate_arg_help, wifi_get_transmit_rate_command_handler, { CONSOLE_ARG_END } };

extern sl_status_t wifi_get_tx_power_command_handler( console_args_t* arguments );
static const char* _wifi_get_tx_power_arg_help[] = {

};
static const console_descriptive_command_t _wifi_get_tx_power_command = { "get module TX power",_wifi_get_tx_power_arg_help, wifi_get_tx_power_command_handler, { CONSOLE_ARG_END } };

extern sl_status_t wifi_init_command_handler( console_args_t* arguments );
static const char* _wifi_init_arg_help[] = {
  0,

};
static const console_descriptive_command_t _wifi_init_command = { "Init Wi-Fi interface",_wifi_init_arg_help, wifi_init_command_handler, { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(wifi_init_mode) ), CONSOLE_ARG_END } };

extern sl_status_t wifi_iot_socket_accept_handler( console_args_t* arguments );
static const char* _wifi_iot_socket_accept_arg_help[] = {
  0,

};
static const console_descriptive_command_t _wifi_iot_socket_accept_command = { "IOT socket listen, \n parameters is \n 1 - sock_fd \n",_wifi_iot_socket_accept_arg_help, wifi_iot_socket_accept_handler, { CONSOLE_ARG_INT, CONSOLE_ARG_END } };

extern sl_status_t wifi_iot_socket_bind_handler( console_args_t* arguments );
static const char* _wifi_iot_socket_bind_arg_help[] = {
  0,
  0,
  0,
  0,

};
static const console_descriptive_command_t _wifi_iot_socket_bind_command = { "IOT socket bind, \n parameters is \n 1 - sock_fd, \n 2 - ip_address, \n 3 - port \n 4 - ip_address_len \n",_wifi_iot_socket_bind_arg_help, wifi_iot_socket_bind_handler, { CONSOLE_ARG_INT, CONSOLE_ARG_IP_ADDRESS, CONSOLE_ARG_UINT, CONSOLE_OPTIONAL_ARG('l', CONSOLE_ARG_UINT ), CONSOLE_ARG_END } };

extern sl_status_t wifi_iot_socket_close_handler( console_args_t* arguments );
static const char* _wifi_iot_socket_close_arg_help[] = {
  0,

};
static const console_descriptive_command_t _wifi_iot_socket_close_command = { "IOT closing socket, \n parameters is \n 1 - sock_fd \n",_wifi_iot_socket_close_arg_help, wifi_iot_socket_close_handler, { CONSOLE_ARG_INT, CONSOLE_ARG_END } };

extern sl_status_t wifi_iot_socket_connect_handler( console_args_t* arguments );
static const char* _wifi_iot_socket_connect_arg_help[] = {
  0,
  0,
  0,
  0,

};
static const console_descriptive_command_t _wifi_iot_socket_connect_command = { "IOT socket connect, \n parameters is \n 1 - sock_fd, , \n 2 - ip_address, \n 3 - port, \n 4 - ip_address_len \n",_wifi_iot_socket_connect_arg_help, wifi_iot_socket_connect_handler, { CONSOLE_ARG_INT, CONSOLE_ARG_IP_ADDRESS, CONSOLE_ARG_UINT, CONSOLE_OPTIONAL_ARG('l', CONSOLE_ARG_UINT ), CONSOLE_ARG_END } };

extern sl_status_t wifi_iot_socket_create_handler( console_args_t* arguments );
static const char* _wifi_iot_socket_create_arg_help[] = {
  0,
  0,
  0,

};
static const console_descriptive_command_t _wifi_iot_socket_create_command = { "IOT socket creation, \n parameters are \n 1 - Type, possible value: 1(stream), 2(dgram), 3(raw),  \n2 - protocol, possible value:1(tcp), 2(udp).. \n  - Domain, possible value: 1(ipv4), 2(ipv6) \n",_wifi_iot_socket_create_arg_help, wifi_iot_socket_create_handler, { CONSOLE_ENUM_ARG(socket_type), CONSOLE_ENUM_ARG(ip_protocol), CONSOLE_OPTIONAL_ARG('j', CONSOLE_ENUM_ARG(ipv4_or_ipv6) ), CONSOLE_ARG_END } };

extern sl_status_t wifi_iot_get_host_by_name_handler( console_args_t* arguments );
static const char* _wifi_iot_socket_get_host_by_name_arg_help[] = {
  0,
  0,
  0,

};
static const console_descriptive_command_t _wifi_iot_socket_get_host_by_name_command = { "IOT socket get host by name, \n parameters is \nstring - url, \nipv4_or_ipv6 - AF interface\nuint32 - length of ip in bytes",_wifi_iot_socket_get_host_by_name_arg_help, wifi_iot_get_host_by_name_handler, { CONSOLE_ARG_STRING, CONSOLE_ENUM_ARG(ipv4_or_ipv6), CONSOLE_OPTIONAL_ARG('l', CONSOLE_ARG_UINT ), CONSOLE_ARG_END } };

extern sl_status_t wifi_iot_socket_get_opt_handler( console_args_t* arguments );
static const char* _wifi_iot_socket_get_opt_arg_help[] = {
  0,
  0,

};
static const console_descriptive_command_t _wifi_iot_socket_get_opt_command = { "IOT socket get opt  parameters are \n 1 - sock_fd \n 2 - option_id \n",_wifi_iot_socket_get_opt_arg_help, wifi_iot_socket_get_opt_handler, { CONSOLE_ARG_INT, CONSOLE_ENUM_ARG(get_option_id), CONSOLE_ARG_END } };

extern sl_status_t wifi_iot_socket_get_peer_name_handler( console_args_t* arguments );
static const char* _wifi_iot_socket_get_peer_name_arg_help[] = {
  0,

};
static const console_descriptive_command_t _wifi_iot_socket_get_peer_name_command = { "IOT socket get peer name, \n parameters is \n 1 - sock_fd \n",_wifi_iot_socket_get_peer_name_arg_help, wifi_iot_socket_get_peer_name_handler, { CONSOLE_ARG_INT, CONSOLE_ARG_END } };

extern sl_status_t wifi_iot_socket_get_socket_name_handler( console_args_t* arguments );
static const char* _wifi_iot_socket_get_sock_name_arg_help[] = {
  0,

};
static const console_descriptive_command_t _wifi_iot_socket_get_sock_name_command = { "IOT socket get socket name, \n parameters is \n 1 - sock_fd \n",_wifi_iot_socket_get_sock_name_arg_help, wifi_iot_socket_get_socket_name_handler, { CONSOLE_ARG_INT, CONSOLE_ARG_END } };

extern sl_status_t wifi_iot_socket_listen_handler( console_args_t* arguments );
static const char* _wifi_iot_socket_listen_arg_help[] = {
  0,
  0,

};
static const console_descriptive_command_t _wifi_iot_socket_listen_command = { "IOT socket listen, \n parameters is \n 1 - sock_fd, \n 2 - backlog \n",_wifi_iot_socket_listen_arg_help, wifi_iot_socket_listen_handler, { CONSOLE_ARG_INT, CONSOLE_ARG_INT, CONSOLE_ARG_END } };

extern sl_status_t wifi_iot_socket_receive_handler( console_args_t* arguments );
static const char* _wifi_iot_socket_receive_arg_help[] = {
  0,

};
static const console_descriptive_command_t _wifi_iot_socket_receive_command = { "IOT socket receive, \n parameters is \n 1 - sock_fd \n",_wifi_iot_socket_receive_arg_help, wifi_iot_socket_receive_handler, { CONSOLE_ARG_INT, CONSOLE_ARG_END } };

extern sl_status_t wifi_iot_socket_receive_from_handler( console_args_t* arguments );
static const char* _wifi_iot_socket_receive_from_arg_help[] = {
  0,

};
static const console_descriptive_command_t _wifi_iot_socket_receive_from_command = { "IOT socket receive from, \n parameters is \n 1 - sock_fd \n",_wifi_iot_socket_receive_from_arg_help, wifi_iot_socket_receive_from_handler, { CONSOLE_ARG_INT, CONSOLE_ARG_END } };

extern sl_status_t wifi_iot_socket_send_handler( console_args_t* arguments );
static const char* _wifi_iot_socket_send_arg_help[] = {
  0,
  0,

};
static const console_descriptive_command_t _wifi_iot_socket_send_command = { "IOT socket receieve, \n parameters is \n 1 - sock_fd, \n 2 - data to be sent \n",_wifi_iot_socket_send_arg_help, wifi_iot_socket_send_handler, { CONSOLE_ARG_INT, CONSOLE_ARG_STRING, CONSOLE_ARG_END } };

extern sl_status_t wifi_iot_socket_send_to_handler( console_args_t* arguments );
static const char* _wifi_iot_socket_send_to_arg_help[] = {
  0,
  0,
  0,
  0,
  0,

};
static const console_descriptive_command_t _wifi_iot_socket_send_to_command = { "IOT socket send to, \n parameters are \n 1 - sock_fd, \n 2 - data to be sent \n 3 - ip_address, \n 4 - port, \n 5 - ip_address_len (4 or 16 bytes) \n",_wifi_iot_socket_send_to_arg_help, wifi_iot_socket_send_to_handler, { CONSOLE_ARG_INT, CONSOLE_ARG_STRING, CONSOLE_ARG_IP_ADDRESS, CONSOLE_ARG_UINT, CONSOLE_OPTIONAL_ARG('l', CONSOLE_ARG_UINT ), CONSOLE_ARG_END } };

extern sl_status_t wifi_iot_socket_set_opt_handler( console_args_t* arguments );
static const char* _wifi_iot_socket_set_opt_arg_help[] = {
  0,
  0,
  0,

};
static const console_descriptive_command_t _wifi_iot_socket_set_opt_command = { "IOT socket set opt parameters are \n 1 - sock_fd \n 2 - option_id \n 3 - value \n",_wifi_iot_socket_set_opt_arg_help, wifi_iot_socket_set_opt_handler, { CONSOLE_ARG_INT, CONSOLE_ENUM_ARG(set_option_id), CONSOLE_ARG_INT, CONSOLE_ARG_END } };

extern sl_status_t wifi_is_interface_up_command_handler( console_args_t* arguments );
static const char* _wifi_is_interface_up_arg_help[] = {
  0,

};
static const console_descriptive_command_t _wifi_is_interface_up_command = { "Check if the specified interface is up",_wifi_is_interface_up_arg_help, wifi_is_interface_up_command_handler, { CONSOLE_ENUM_ARG(wifi_interface), CONSOLE_ARG_END } };

extern sl_status_t wifi_load_certificate_handler( console_args_t* arguments );
static const char* _wifi_load_certificate_arg_help[] = {
  0,
  0,
  0,

};
static const console_descriptive_command_t _wifi_load_certificate_command = { "Load/Clear Certificate from Flash, \n parameters are \n 1 - load/clear, possible value: 0(Clear), 1(Load) .., \n 2 - Certificate Index , possible value: 0, 1, 2 ..,  \n 3 - Type, possible value: 0(OPENSSL), 1(AWS) .. \n",_wifi_load_certificate_arg_help, wifi_load_certificate_handler, { CONSOLE_OPTIONAL_ARG('l', CONSOLE_ARG_INT ), CONSOLE_OPTIONAL_ARG('i', CONSOLE_ARG_INT ), CONSOLE_OPTIONAL_ARG('t', CONSOLE_ARG_INT ), CONSOLE_ARG_END } };

extern sl_status_t wifi_scan_command_handler( console_args_t* arguments );
static const char* _wifi_scan_arg_help[] = {
  0,
  0,
  0,
  0,
  0,
  0,

};
static const console_descriptive_command_t _wifi_scan_command = { "Start scanning",_wifi_scan_arg_help, wifi_scan_command_handler, { CONSOLE_OPTIONAL_ARG('s', CONSOLE_ARG_STRING ), CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(wifi_interface) ), CONSOLE_OPTIONAL_ARG('m', CONSOLE_ENUM_ARG(enable_bg_scan) ), CONSOLE_OPTIONAL_ARG('c', CONSOLE_ARG_UINT ), CONSOLE_OPTIONAL_ARG('a', CONSOLE_ARG_UINT ), CONSOLE_OPTIONAL_ARG('t', CONSOLE_ARG_UINT ), CONSOLE_ARG_END } };

extern sl_status_t sl_wifi_send_raw_data_command_handler( console_args_t* arguments );
static const char* _wifi_send_raw_data_arg_help[] = {
  0,

};
static const console_descriptive_command_t _wifi_send_raw_data_command = { "send raw data frame",_wifi_send_raw_data_arg_help, sl_wifi_send_raw_data_command_handler, { CONSOLE_ENUM_ARG(wifi_interface), CONSOLE_ARG_END } };

extern sl_status_t wifi_set_advanced_client_config_command_handler( console_args_t* arguments );
static const char* _wifi_set_advanced_client_config_arg_help[] = {
  0,
  0,
  0,
  0,
  0,

};
static const console_descriptive_command_t _wifi_set_advanced_client_config_command = { "Set advanced client config",_wifi_set_advanced_client_config_arg_help, wifi_set_advanced_client_config_command_handler, { CONSOLE_OPTIONAL_ARG('o', CONSOLE_ARG_UINT ), CONSOLE_OPTIONAL_ARG('r', CONSOLE_ARG_UINT ), CONSOLE_OPTIONAL_ARG('i', CONSOLE_ARG_UINT ), CONSOLE_OPTIONAL_ARG('b', CONSOLE_ARG_UINT ), CONSOLE_OPTIONAL_ARG('f', CONSOLE_ARG_UINT ), CONSOLE_ARG_END } };

extern sl_status_t sl_wifi_set_advanced_scan_configuration_command_handler( console_args_t* arguments );
static const char* _wifi_set_advanced_scan_configuration_arg_help[] = {
  0,
  0,
  0,
  0,
  0,

};
static const console_descriptive_command_t _wifi_set_advanced_scan_configuration_command = { "Set advanced scan configuration",_wifi_set_advanced_scan_configuration_arg_help, sl_wifi_set_advanced_scan_configuration_command_handler, { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ARG_UINT ), CONSOLE_OPTIONAL_ARG('j', CONSOLE_ARG_UINT ), CONSOLE_OPTIONAL_ARG('k', CONSOLE_ARG_INT ), CONSOLE_OPTIONAL_ARG('l', CONSOLE_ARG_UINT ), CONSOLE_OPTIONAL_ARG('m', CONSOLE_ARG_UINT ), CONSOLE_ARG_END } };

extern sl_status_t wifi_set_antenna_command_handler( console_args_t* arguments );
static const char* _wifi_set_antenna_arg_help[] = {
  0,
  0,

};
static const console_descriptive_command_t _wifi_set_antenna_command = { "Set Wi-Fi antenna",_wifi_set_antenna_arg_help, wifi_set_antenna_command_handler, { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(wifi_interface) ), CONSOLE_OPTIONAL_ARG('a', CONSOLE_ARG_UINT ), CONSOLE_ARG_END } };

extern sl_status_t wifi_set_ap_configuration_command_handler( console_args_t* arguments );
static const char* _wifi_set_ap_configuration_arg_help[] = {
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,

};
static const console_descriptive_command_t _wifi_set_ap_configuration_command = { "set ap configuration -s ssid -c channel -o option -i credential_id -t client_timeout -d becon_count -m max_clients",_wifi_set_ap_configuration_arg_help, wifi_set_ap_configuration_command_handler, { CONSOLE_OPTIONAL_ARG('s', CONSOLE_ARG_STRING ), CONSOLE_OPTIONAL_ARG('p', CONSOLE_ENUM_ARG(wifi_security) ), CONSOLE_OPTIONAL_ARG('e', CONSOLE_ENUM_ARG(wifi_encryption) ), CONSOLE_OPTIONAL_ARG('c', CONSOLE_ARG_UINT ), CONSOLE_OPTIONAL_ARG('r', CONSOLE_ENUM_ARG(rate_protocol) ), CONSOLE_OPTIONAL_ARG('o', CONSOLE_ARG_UINT ), CONSOLE_OPTIONAL_ARG('i', CONSOLE_ARG_UINT ), CONSOLE_OPTIONAL_ARG('t', CONSOLE_ARG_UINT ), CONSOLE_OPTIONAL_ARG('d', CONSOLE_ARG_UINT ), CONSOLE_OPTIONAL_ARG('m', CONSOLE_ARG_UINT ), CONSOLE_ARG_END } };

extern sl_status_t wifi_set_channel_command_handler( console_args_t* arguments );
static const char* _wifi_set_channel_arg_help[] = {
  0,
  0,

};
static const console_descriptive_command_t _wifi_set_channel_command = { "Set channel number in AP interface",_wifi_set_channel_arg_help, wifi_set_channel_command_handler, { CONSOLE_ENUM_ARG(wifi_interface), CONSOLE_ARG_UINT, CONSOLE_ARG_END } };

extern sl_status_t wifi_set_default_interface_command_handler( console_args_t* arguments );
static const char* _wifi_set_default_interface_arg_help[] = {
  0,

};
static const console_descriptive_command_t _wifi_set_default_interface_command = { "Set default interface",_wifi_set_default_interface_arg_help, wifi_set_default_interface_command_handler, { CONSOLE_ENUM_ARG(wifi_interface), CONSOLE_ARG_END } };

extern sl_status_t wifi_set_performance_profile_command_handler( console_args_t* arguments );
static const char* _wifi_set_performance_profile_arg_help[] = {
  0,

};
static const console_descriptive_command_t _wifi_set_performance_profile_command = { "Set the performance profile of the module",_wifi_set_performance_profile_arg_help, wifi_set_performance_profile_command_handler, { CONSOLE_ENUM_ARG(performance_mode), CONSOLE_ARG_END } };

extern sl_status_t wifi_set_roam_configuration_command_handler( console_args_t* arguments );
static const char* _wifi_set_roam_configuration_arg_help[] = {
  0,
  0,
  0,

};
static const console_descriptive_command_t _wifi_set_roam_configuration_command = { "Set roaming configuration",_wifi_set_roam_configuration_arg_help, wifi_set_roam_configuration_command_handler, { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(wifi_interface) ), CONSOLE_ARG_INT, CONSOLE_ARG_INT, CONSOLE_ARG_END } };

extern sl_status_t wifi_set_transmit_rate_command_handler( console_args_t* arguments );
static const char* _wifi_set_transmit_rate_arg_help[] = {
  0,
  0,
  0,

};
static const console_descriptive_command_t _wifi_set_transmit_rate_command = { "Set rate protocol and data rate",_wifi_set_transmit_rate_arg_help, wifi_set_transmit_rate_command_handler, { CONSOLE_ENUM_ARG(rate_protocol), CONSOLE_ENUM_ARG(data_rate), CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(wifi_interface) ), CONSOLE_ARG_END } };

extern sl_status_t wifi_set_tx_power_command_handler( console_args_t* arguments );
static const char* _wifi_set_tx_power_arg_help[] = {
  0,

};
static const console_descriptive_command_t _wifi_set_tx_power_command = { "Set module TX power",_wifi_set_tx_power_arg_help, wifi_set_tx_power_command_handler, { CONSOLE_ARG_UINT, CONSOLE_ARG_END } };

extern sl_status_t wifi_start_ap_command_handler( console_args_t* arguments );
static const char* _wifi_start_ap_arg_help[] = {
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,

};
static const console_descriptive_command_t _wifi_start_ap_command = { "Start in AP mode. Check sl_wifi_ap_configuration_t for parameters description",_wifi_start_ap_arg_help, wifi_start_ap_command_handler, { CONSOLE_OPTIONAL_ARG('a', CONSOLE_ARG_STRING ), CONSOLE_OPTIONAL_ARG('b', CONSOLE_ENUM_ARG(wifi_security) ), CONSOLE_OPTIONAL_ARG('c', CONSOLE_ENUM_ARG(wifi_encryption) ), CONSOLE_OPTIONAL_ARG('d', CONSOLE_ARG_UINT ), CONSOLE_OPTIONAL_ARG('e', CONSOLE_ENUM_ARG(wifi_band) ), CONSOLE_OPTIONAL_ARG('f', CONSOLE_ENUM_ARG(wifi_bandwidth) ), CONSOLE_OPTIONAL_ARG('g', CONSOLE_ENUM_ARG(rate_protocol) ), CONSOLE_OPTIONAL_ARG('h', CONSOLE_ENUM_ARG(wifi_ap_flag) ), CONSOLE_OPTIONAL_ARG('i', CONSOLE_ARG_UINT ), CONSOLE_OPTIONAL_ARG('j', CONSOLE_ARG_UINT ), CONSOLE_OPTIONAL_ARG('k', CONSOLE_ARG_UINT ), CONSOLE_OPTIONAL_ARG('l', CONSOLE_ARG_UINT ), CONSOLE_OPTIONAL_ARG('m', CONSOLE_ARG_UINT ), CONSOLE_OPTIONAL_ARG('n', CONSOLE_ARG_UINT ), CONSOLE_ARG_END } };

extern sl_status_t wifi_start_statistic_report_command_handler( console_args_t* arguments );
static const char* _wifi_start_statistic_report_arg_help[] = {
  0,
  0,

};
static const console_descriptive_command_t _wifi_start_statistic_report_command = { "Get wifi statistic report",_wifi_start_statistic_report_arg_help, wifi_start_statistic_report_command_handler, { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(wifi_interface) ), CONSOLE_OPTIONAL_ARG('c', CONSOLE_ARG_UINT ), CONSOLE_ARG_END } };

extern sl_status_t wifi_start_wps( console_args_t* arguments );
static const char* _wifi_start_wps_arg_help[] = {
  0,
  0,
  0,

};
static const console_descriptive_command_t _wifi_start_wps_command = { "Enable WPS on access point",_wifi_start_wps_arg_help, wifi_start_wps, { CONSOLE_ENUM_ARG(wifi_interface), CONSOLE_ENUM_ARG(wps_mode), CONSOLE_OPTIONAL_ARG('p', CONSOLE_ARG_STRING ), CONSOLE_ARG_END } };

extern sl_status_t wifi_stop_scan_command_handler( console_args_t* arguments );
static const char* _wifi_stop_scan_arg_help[] = {
  0,

};
static const console_descriptive_command_t _wifi_stop_scan_command = { "Stop scanning",_wifi_stop_scan_arg_help, wifi_stop_scan_command_handler, { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(wifi_interface) ), CONSOLE_ARG_END } };

extern sl_status_t wifi_stop_statistic_report_command_handler( console_args_t* arguments );
static const char* _wifi_stop_statistic_report_arg_help[] = {
  0,

};
static const console_descriptive_command_t _wifi_stop_statistic_report_command = { "Stop wifi statistic report",_wifi_stop_statistic_report_arg_help, wifi_stop_statistic_report_command_handler, { CONSOLE_OPTIONAL_ARG('i', CONSOLE_ENUM_ARG(wifi_interface) ), CONSOLE_ARG_END } };

extern sl_status_t wifi_test_client_configuration_command_handler( console_args_t* arguments );
static const char* _wifi_test_client_configuration_arg_help[] = {
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,

};
static const console_descriptive_command_t _wifi_test_client_configuration_command = { "Start connecting to specified AP",_wifi_test_client_configuration_arg_help, wifi_test_client_configuration_command_handler, { CONSOLE_ARG_STRING, CONSOLE_ARG_STRING, CONSOLE_OPTIONAL_ARG('s', CONSOLE_ENUM_ARG(wifi_security) ), CONSOLE_OPTIONAL_ARG('u', CONSOLE_ARG_STRING ), CONSOLE_OPTIONAL_ARG('e', CONSOLE_ENUM_ARG(wifi_encryption) ), CONSOLE_OPTIONAL_ARG('t', CONSOLE_ARG_UINT ), CONSOLE_OPTIONAL_ARG('l', CONSOLE_ARG_UINT ), CONSOLE_OPTIONAL_ARG('c', CONSOLE_ARG_UINT ), CONSOLE_OPTIONAL_ARG('b', CONSOLE_ENUM_ARG(wifi_band) ), CONSOLE_OPTIONAL_ARG('w', CONSOLE_ENUM_ARG(wifi_bandwidth) ), CONSOLE_ARG_END } };

extern sl_status_t sl_wifi_transmit_test_start_command_handler( console_args_t* arguments );
static const char* _wifi_transmit_test_start_arg_help[] = {
  0,
  0,
  0,
  0,
  0,

};
static const console_descriptive_command_t _wifi_transmit_test_start_command = { "Start the transmit test. This is a blocking API",_wifi_transmit_test_start_arg_help, sl_wifi_transmit_test_start_command_handler, { CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_END } };

extern sl_status_t sl_wifi_transmit_test_stop_command_handler( console_args_t* arguments );
static const char* _wifi_transmit_test_stop_arg_help[] = {

};
static const console_descriptive_command_t _wifi_transmit_test_stop_command = { "Stop the transmit test. This is a blocking API",_wifi_transmit_test_stop_arg_help, sl_wifi_transmit_test_stop_command_handler, { CONSOLE_ARG_END } };

extern sl_status_t sl_wifi_update_gain_table_command_handler( console_args_t* arguments );
static const char* _wifi_update_gain_table_arg_help[] = {
  0,
  0,

};
static const console_descriptive_command_t _wifi_update_gain_table_command = { "Assign the user configurable channel gain values in different regions to the module from user",_wifi_update_gain_table_arg_help, sl_wifi_update_gain_table_command_handler, { CONSOLE_ARG_UINT, CONSOLE_ARG_UINT, CONSOLE_ARG_END } };


const console_database_t console_command_database =
{
    CONSOLE_DATABASE_ENTRIES(
    	{ "get",    &_get_command },
    	{ "help",    &_help_command },
    	{ "list",    &_list_command },
    	{ "net_deinit",    &_net_deinit_command },
    	{ "net_down",    &_net_down_command },
    	{ "net_init",    &_net_init_command },
    	{ "net_join_multicast_address",    &_net_join_multicast_address_command },
    	{ "net_leave_multicast_address",    &_net_leave_multicast_address_command },
    	{ "net_up",    &_net_up_command },
    	{ "rtt",    &_rtt_command },
    	{ "set",    &_set_command },
    	{ "set_nvm_profile",    &_set_nvm_profile_command },
    	{ "sl_dns_host_get_by_name",    &_sl_dns_host_get_by_name_command },
    	{ "sl_net_ping",    &_sl_net_ping_command },
    	{ "sl_si91x_get_ram_log",    &_sl_si91x_get_ram_log_command },
    	{ "start_dhcp",    &_start_dhcp_command },
    	{ "thread",    &_thread_command },
    	{ "wifi_11ax_config",    &_wifi_11ax_config_command },
    	{ "wifi_assert",    &_wifi_assert_command },
    	{ "wifi_bsd_get_host_by_name",    &_wifi_bsd_get_host_by_name_command },
    	{ "wifi_bsd_get_peer_name",    &_wifi_bsd_get_peer_name_command },
    	{ "wifi_bsd_get_sock_name",    &_wifi_bsd_get_sock_name_command },
    	{ "wifi_bsd_get_sock_opt",    &_wifi_bsd_get_sock_opt_command },
    	{ "wifi_bsd_set_sock_opt",    &_wifi_bsd_set_sock_opt_command },
    	{ "wifi_bsd_socket_accept",    &_wifi_bsd_socket_accept_command },
    	{ "wifi_bsd_socket_bind",    &_wifi_bsd_socket_bind_command },
    	{ "wifi_bsd_socket_close",    &_wifi_bsd_socket_close_command },
    	{ "wifi_bsd_socket_connect",    &_wifi_bsd_socket_connect_command },
    	{ "wifi_bsd_socket_create",    &_wifi_bsd_socket_create_command },
    	{ "wifi_bsd_socket_listen",    &_wifi_bsd_socket_listen_command },
    	{ "wifi_bsd_socket_receive",    &_wifi_bsd_socket_receive_command },
    	{ "wifi_bsd_socket_receive_from",    &_wifi_bsd_socket_receive_from_command },
    	{ "wifi_bsd_socket_select",    &_wifi_bsd_socket_select_command },
    	{ "wifi_bsd_socket_send",    &_wifi_bsd_socket_send_command },
    	{ "wifi_bsd_socket_send_to",    &_wifi_bsd_socket_send_to_command },
    	{ "wifi_connect",    &_wifi_connect_command },
    	{ "wifi_deinit",    &_wifi_deinit_command },
    	{ "wifi_disable_target_wake_time",    &_wifi_disable_target_wake_time_command },
    	{ "wifi_disconnect",    &_wifi_disconnect_command },
    	{ "wifi_disconnect_ap_client",    &_wifi_disconnect_ap_client_command },
    	{ "wifi_enable_target_wake_time",    &_wifi_enable_target_wake_time_command },
    	{ "wifi_filter_broadcast",    &_wifi_filter_broadcast_command },
    	{ "wifi_generate_wps_pin",    &_wifi_generate_wps_pin_command },
    	{ "wifi_get_advanced_scan_configuration",    &_wifi_get_advanced_scan_configuration_command },
    	{ "wifi_get_ap_client_count",    &_wifi_get_ap_client_count_command },
    	{ "wifi_get_ap_client_list",    &_wifi_get_ap_client_list_command },
    	{ "wifi_get_ap_configuration",    &_wifi_get_ap_configuration_command },
    	{ "wifi_get_channel",    &_wifi_get_channel_command },
    	{ "wifi_get_client_info",    &_wifi_get_client_info_command },
    	{ "wifi_get_default_interface",    &_wifi_get_default_interface_command },
    	{ "wifi_get_fw_version",    &_wifi_get_fw_version_command },
    	{ "wifi_get_mac_address",    &_wifi_get_mac_address_command },
    	{ "wifi_get_pairwise_master_key",    &_wifi_get_pairwise_master_key_command },
    	{ "wifi_get_performance_profile",    &_wifi_get_performance_profile_command },
    	{ "wifi_get_signal_strength",    &_wifi_get_signal_strength_command },
    	{ "wifi_get_statistics",    &_wifi_get_statistics_command },
    	{ "wifi_get_transmit_rate",    &_wifi_get_transmit_rate_command },
    	{ "wifi_get_tx_power",    &_wifi_get_tx_power_command },
    	{ "wifi_init",    &_wifi_init_command },
    	{ "wifi_iot_socket_accept",    &_wifi_iot_socket_accept_command },
    	{ "wifi_iot_socket_bind",    &_wifi_iot_socket_bind_command },
    	{ "wifi_iot_socket_close",    &_wifi_iot_socket_close_command },
    	{ "wifi_iot_socket_connect",    &_wifi_iot_socket_connect_command },
    	{ "wifi_iot_socket_create",    &_wifi_iot_socket_create_command },
    	{ "wifi_iot_socket_get_host_by_name",    &_wifi_iot_socket_get_host_by_name_command },
    	{ "wifi_iot_socket_get_opt",    &_wifi_iot_socket_get_opt_command },
    	{ "wifi_iot_socket_get_peer_name",    &_wifi_iot_socket_get_peer_name_command },
    	{ "wifi_iot_socket_get_sock_name",    &_wifi_iot_socket_get_sock_name_command },
    	{ "wifi_iot_socket_listen",    &_wifi_iot_socket_listen_command },
    	{ "wifi_iot_socket_receive",    &_wifi_iot_socket_receive_command },
    	{ "wifi_iot_socket_receive_from",    &_wifi_iot_socket_receive_from_command },
    	{ "wifi_iot_socket_send",    &_wifi_iot_socket_send_command },
    	{ "wifi_iot_socket_send_to",    &_wifi_iot_socket_send_to_command },
    	{ "wifi_iot_socket_set_opt",    &_wifi_iot_socket_set_opt_command },
    	{ "wifi_is_interface_up",    &_wifi_is_interface_up_command },
    	{ "wifi_load_certificate",    &_wifi_load_certificate_command },
    	{ "wifi_scan",    &_wifi_scan_command },
    	{ "wifi_send_raw_data",    &_wifi_send_raw_data_command },
    	{ "wifi_set_advanced_client_config",    &_wifi_set_advanced_client_config_command },
    	{ "wifi_set_advanced_scan_configuration",    &_wifi_set_advanced_scan_configuration_command },
    	{ "wifi_set_antenna",    &_wifi_set_antenna_command },
    	{ "wifi_set_ap_configuration",    &_wifi_set_ap_configuration_command },
    	{ "wifi_set_channel",    &_wifi_set_channel_command },
    	{ "wifi_set_default_interface",    &_wifi_set_default_interface_command },
    	{ "wifi_set_performance_profile",    &_wifi_set_performance_profile_command },
    	{ "wifi_set_roam_configuration",    &_wifi_set_roam_configuration_command },
    	{ "wifi_set_transmit_rate",    &_wifi_set_transmit_rate_command },
    	{ "wifi_set_tx_power",    &_wifi_set_tx_power_command },
    	{ "wifi_start_ap",    &_wifi_start_ap_command },
    	{ "wifi_start_statistic_report",    &_wifi_start_statistic_report_command },
    	{ "wifi_start_wps",    &_wifi_start_wps_command },
    	{ "wifi_stop_scan",    &_wifi_stop_scan_command },
    	{ "wifi_stop_statistic_report",    &_wifi_stop_statistic_report_command },
    	{ "wifi_test_client_configuration",    &_wifi_test_client_configuration_command },
    	{ "wifi_transmit_test_start",    &_wifi_transmit_test_start_command },
    	{ "wifi_transmit_test_stop",    &_wifi_transmit_test_stop_command },
    	{ "wifi_update_gain_table",    &_wifi_update_gain_table_command },
    )
};
