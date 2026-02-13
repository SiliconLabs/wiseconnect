/*******************************************************************************
* @file  mgmt_data_flow.h
* @brief This file contains data related functionalities
*******************************************************************************
* # License
* <b>Copyright 2020-2024 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef MGMT_DATA_FLOW_H
#define MGMT_DATA_FLOW_H

#define SOCK_O_NONBLOCK 0x4000

enum client_ps_state_e {
  CLIENT_PS_STATE_WAKEUP = 0,
  CLIENT_PS_STATE_SLP_REQ_FROM_HOST,
  CLIENT_PS_STATE_WAIT_FOR_SLP_CFM,
  CLIENT_PS_STATE_SLP_CFM_RCVD,
  CLIENT_PS_STATE_CONFIG_LCM, /* Config low clock mode by changing the clock configurations */
  CLIENT_PS_STATE_SLEEP
};

enum cert_type_e {
  SERVER_CERT = 0,
  SERVER_INT_ROOT_CA,
  SERVER_ROOT_CA,
  SERVER_KEY,
  CLIENT_CERT,
  CLIENT_INT_ROOT_CA,
  CLIENT_ROOT_CA,
  CLIENT_KEY
};

/* Receive data respose */
#ifdef ENABLE_WC_PLUS
RSI_PACK typedef struct rsi_rsp_data_recv_non_rom_s
#else
typedef struct __PACKED__ rsi_rsp_data_recv_non_rom_s
#endif
{
  //! IP version
  uint16 ip_version;

  //! socket descriptor
  uint16 socket_descriptor;

  //! Buffer length
  uint16 buffer_length[2];

  //! data offset
  uint16 data_offset;

  //! Destination port
  uint16 dest_port;
  union {
    //! Destination IPv4 address
    uint8 ipv4_address[4];

    //! Destination IPv6 address
    uint32 ipv6_address[4];
  } dest_ip_address;

  //! total data processed
  uint8 end_of_the_data;

  //! more data pending
  uint8 more_data_pending;
} rsi_rsp_data_recv_non_rom_t;

typedef struct rsi_switch_protocol {
  uint8 mode_value;
} rsi_switch_protocol_t;

#ifdef ENABLE_WC_PLUS
RSI_PACK typedef struct rsi_rsp_socket_close_non_rom_s
#else
typedef struct __PACKED__ rsi_rsp_socket_close_non_rom_s
#endif
{
  //! Socket desriptor close
  uint16 socket_descriptor;

  //! Number of bytes sent on that socket
  uint32 total_bytes_sent;

  //! Socket port number
  uint16 port_number;

} rsi_rsp_socket_close_non_rom_t;

#ifdef ENABLE_WC_PLUS
RSI_PACK typedef struct rsi_req_socket_config_cap_s
#else
typedef struct __PACKED__ rsi_req_socket_config_cap_s
#endif
{
  //! TCP TX + TCP RX + UDP TX + UDP RX
  uint8 total_sockets;

  //! TCP TX + TCP RX
  uint8 total_tcp_sockets;

  //! UDP TX + UDP RX
  uint8 total_udp_sockets;

  //! TCP TX
  uint8 tcp_tx_only_sockets;

  //! TCP RX
  uint8 tcp_rx_only_sockets;

  //! UDP TX
  uint8 udp_tx_only_sockets;

  //! UDP RX
  uint8 udp_rx_only_sockets;

  //! TCP RX High Performance
  uint8 tcp_rx_high_performance_sockets;

  //! TCP RX Window caps
  uint8 tcp_rx_window_size_cap;

  //! TCP Window division factor
  uint8 tcp_ack_div_factor;

} rsi_req_socket_config_cap_t;

#ifdef ENABLE_WC_PLUS
RSI_PACK typedef struct rsi_rsp_socket_create_non_rom_s
#else
typedef struct __PACKED__ rsi_rsp_socket_create_non_rom_s
#endif
{
  uint16 ip_version;
  uint16 socket_type;
  uint16 socket_descriptor;
  uint16 src_port;
  uint16 dest_port;
  union {
    uint8 ipv4_address[4];
    uint32 ipv6_address[4];
  } ip_address;
  union {
    uint8 ipv4_address[4];
    uint32 ipv6_address[4];
  } dest_ip_address;
  uint16 mss;
  uint32 window_size;
} rsi_rsp_socket_create_non_rom_t;

void set_tls_version_1();
int16 process_network_params_rsp(uint8 *rx_pkt);
int16 is_sl_ip_pkt(uint8 *rxPkt, uint8 vap_id);
void sl_handle_tcp_packet(uint8 *pkt);
void get_go_ip_address_info(void *go_params, void *sta);
void dhcp_release_wrapper(uint8 *buf);
void sl_free_netx_malloc_mem(uint8 vap_id);
#endif /* MGMT_DATA_FLOW_H */
