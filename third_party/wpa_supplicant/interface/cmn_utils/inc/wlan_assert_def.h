/*******************************************************************************
* @file  wlan_assert_def.h
* @brief This file acts as interface between management interface and
*        external modules
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
#ifndef WLAN_ASSERT_DEF_H
#define WLAN_ASSERT_DEF_H

/*Assert Type in K60*/
#define RECV_CNT_RANGE_CHECK_FAIL                                         119
#define INVALID_SOCKET_NO_1                                               120
#define INVALID_SOCKET_NO_2                                               121
#define INVALID_TOTAL_LENGTH                                              122
#define FRAGMENTATION_NOT_ALLOWED                                         123
#define NULL_TX_PKT_1                                                     125
#define UART_HANDLER_FAIL                                                 126
#define SOCKET_NO_EQ_ARP_SOCKET_NO                                        127
#define INVALID_SOCKET_PTR                                                128
#define TX_PKT_IPSEC_FLAG_EQ_14                                           129
#define TAP_PTR_EQ_NULL                                                   130
#define SOCK_PTR_EQ_NULL                                                  131
#define RECV_LEN_GT_1536                                                  132
#define NO_PS_RESPONSE_EXIST                                              134
#define NO_RESPONSE_EXIST                                                 135
#define NO_SCAN_RESULT                                                    136
#define INITIALIZATION_FAILED                                             137
#define RISING_EDGE_GET                                                   138
#define ADD_TO_QUEUE_FAILED                                               139
#define SEND_TO_ETH_FAIL                                                  140
#define DEBUG_DEFINITION                                                  141
#define SPI_SEND_RECV_LEN_GT_1516                                         142
#define NODE_NULL_1                                                       143
#define DMA_STATUS_ERROR                                                  144
#define DMA_BUFFER_ERROR                                                  145
#define SLAVE_RX_PTR_NULL                                                 145
#define INVALID_SLAVE_STATE                                               146
#define BOOT_LOADER_FAILED                                                147
#define INVALID_PAYLOAD_1                                                 148
#define NULL_HOST_CMD                                                     149
#define INVALID_DATA_RECV_STATE                                           150
#define MORE_DATA_IS_COMING                                               151
#define TX_BUFFER_NULL                                                    152
#define NULL_TX_PKT_2                                                     153
#define NULL_TX_PKT_3                                                     154
#define INVALID_USB_DATA_RECV_STATE                                       156
#define INVALID_AP_SSID                                                   157
#define IE_LEN_GT_128                                                     158
#define RSI_JOIN_ERROR                                                    159
#define RSI_GET_SSID_ERROR                                                160
#define RSI_SET_KEY_ERROR                                                 161
#define SET_GENERIC_ELEMENT_FAILED                                        162
#define SET_AP_WPS_IE_FAILED                                              163
#define RSI_GET_BSSID_FAILED                                              164
#define SEND_EAPOL_FAILED                                                 165
#define NULL_TX_PKT_4                                                     166
#define WSC_ASSERT_ALIGN_READ_LEN_INSUFFICIENT                            167
#define WSC_ASSERT_READ_PAYLOAD_LEN_INSUFFICIENT                          168
#define WSC_ASSERT_DHCPD_XMIT_BUF_NULL                                    169
#define WSC_ASSERT_FREE_RCV_PKT_WRONG_LENGTH                              170
#define WSC_ASSERT_MEM_ALLOC_FAILED                                       171
#define WSC_ASSERT_BGET_ASSERT                                            172
#define WSC_ASSERT_CHANNEL_NUM_ZERO                                       173
#define WSC_ASSERT_ALIGN_READ_SECOND_BUF                                  174
#define WSC_ASSERT_READ_RSP_PKT_GT_1500                                   175
#define WSC_ASSERT_RX_BUFFERS_OVER                                        176
#define WSC_ASSERT_ETH_SEND_BUF_LEN_ZERO                                  177
#define WSC_ASSERT_PS_NOTIFY_STA_ID_NOT_VALID                             178
#define WSC_ASSERT_FREE_PKT_WITH_WRONG_FLAG                               180
#define WSC_ASSERT_APME_NULL_PKT_WHILE_PENDING_COUNT_NON_ZERO             181
#define WSC_ASSERT_APME_HEAD_NULL_WHILE_PENDING_COUNT_NON_ZERO            182
#define WSC_ASSERT_APME_FLUSH_STAQ_HEAD_NULL_WHILE_PENDING_COUNT_NON_ZERO 183
#define WSC_ASSERT_APME_ADD_NODE_TOQ_LEN_ZERO                             184
#define WSC_ASSERT_APME_SEND_PKT_TOSTA_PKT_NULL                           185
#define WSC_ASSERT_HTTP_RESP_MEM_ALLOC_FAILED_1                           186
#define WSC_ASSERT_HTTP_RESP_MEM_ALLOC_FAILED_2                           187
#define WSC_ASSERT_HTTP_RESP_MEM_ALLOC_FAILED_3                           188
#define WSC_ASSERT_PREAUTH_DISABLED_1                                     189
#define WSC_ASSERT_PREAUTH_DISABLED_2                                     190

//! 0xABCD
//! C,D - Assertion number
//! B(type) - 0000 - LMAC core, 0001-SME, 0010 - UMAC, 0100 - NETX
//! A(Level)- 0000 - Assertion required, 0010 - Recoverable , 0100 - Information
#define ASSERTION_NOT_REQUIRED 0

#define ASSERTION_REQUIRED                                             1
#define UMAC_ASSERT_DETECT_MEM_LEAK_IN_ALLOC                           0x00FF
#define UMAC_ASSERT_DETECT_MEM_LEAK_IN_FREE                            0x00FE
#define UMAC_ASSERT_DETECT_MEM_LEAK_TYPE_MISMATCH                      0x20FD
#define UMAC_ASSERT_SPI_TXPKT_NOT_ALLOCATED                            0x0200
#define UMAC_ASSERT_SPI_RECEIVED_WRONG_UDP_PKT_LENGTH                  0x4201
#define UMAC_ASSERT_SPI_RECEIVED_WRONG_TCP_PKT_LENGTH                  0x4202
#define UMAC_ASSERT_INVALID_LOCATION_FOR_FLASH_WRITE                   0x0203
#define UMAC_ASSERT_ERROR_IN_BIG_NUM_SET_P                             0x2204
#define UMAC_ASSERT_ERROR_IN_BIG_NUM_EXPTMOD_P                         0x2205
#define UMAC_ASSERT_ERROR_IN_BIG_NUM_SET_Q                             0x2206
#define UMAC_ASSERT_ERROR_IN_BIG_NUM_EXPTMOD_Q                         0x2207
#define UMAC_ASSERT_ERROR_IN_BIG_NUM_SET_N                             0x2208
#define UMAC_ASSERT_ERROR_IN_BIG_NUM_EXPTMOD_N                         0x2209
#define UMAC_ASSERT_BUF_NOT_ALLOCATED_IN_OS_READ_FILE                  0x220A
#define UMAC_ASSERT_BUF_NOT_ALLOCATED_IN_OS_READ_FILE_1                0x220B
#define UMAC_ASSERT_OS_REALLOC_FAILED                                  0x220C
#define UMAC_ASSERT_OS_ASSERT                                          0x020D
#define UMAC_ASSERT_OS_MALLOC_FAILED_IN_UPDATE_SCAN_RES                0x220E
#define UMAC_ASSERT_OS_MALLOC_FAILED_DURING_ASSOCIATION                0x22AA
#define UMAC_ASSERT_UNABLE_TO_LOAD_KEYS                                0x220F
#define UMAC_ASSERT_DETECT_LEAK_TX_FREED_IN_LAMC_IF                    0x0210
#define UMAC_ASSERT_CERT_EXPANSION_EXCEEDING_FLASH_ALLOWED_LIMIT       0x0211
#define UMAC_ASSERT_LMAC_TX_PKT_NULL_IN_LMAC_IF                        0x0212
#define UMAC_ASSERT_TX_PKT_ALLOC_FAIL_IN_EXECUTE_CMD                   0x2213
#define UMAC_ASSERT_TX_PKT_ALLOC_FAIL_IN_LOAD_IE                       0x2214
#define UMAC_ASSERT_TX_PKT_ALLOC_FAIL_IN_LOAD_ROAM_IND                 0x2215
#define UMAC_ASSERT_TX_PKT_ALLOC_FAIL_IN_SEND_DEAUTH_FROM_HHOST        0x2216
#define UMAC_ASSERT_TX_PKT_ALLOC_FAIL_IN_HOST_MAC_IF_TX                0x2217
#define UMAC_ASSERT_LMAC_WAKEUP_COUNT_IS_GREATER_THAN_SLEEP_COUNT      0x221D
#define UMAC_ASSERT_MEMORY_FREE_DETECTED_1_IN_PROCESS_RX_PKT_FROM_LMAC 0x0218
#define UMAC_ASSERT_MEMORY_FREE_DETECTED_2_IN_PROCESS_RX_PKT_FROM_LMAC 0x0219
#define UMAC_ASSERT_RX_PKT_NULL_IN_EVENT_RX_PKT_FROM_LMAC_PENDING      0x021A
#define UMAC_ASSERT_WEBS_HS_ALLOC_FAILED                               0x421C
#define UMAC_ASSERT_WEBS_HS_SEND_FAILED                                0x421D
#define UMAC_ASSERT_TX_PKT_ALLOC_FAIL_IN_SEND_ASSOC_REASSOC_RESP       0x421E
#define UMAC_ASSERT_HRNG_INIT_FAILED                                   0x0223
#define UMAC_ASSERT_MIC_FAILURE                                        0x421F
#define UMAC_ASSERT_CORTEX_M4_INVALID_INTERRUPT                        0x0224
#define UMAC_ASSERT_MQTT_KA_TIMER_ERROR                                0x4220
#define UMAC_ASSERT_MQTT_ACK_TIMER_ERROR                               0x4221
#define UMAC_ASSERT_TRNG_INIT_FAILED                                   0x0225

#define UMAC_ASSERT_GOT_PS_SLEEP_REQ_TO_PM                 0x021C
#define UMAC_ASSERT_EVENT6_NOT_DEFINED_FOR_THREADX         0x021E
#define UMAC_ASSERT_EVENT7_NOT_DEFINED_FOR_THREADX         0x021F
#define UMAC_ASSERT_INVALID_FLASH_WRITE_LOCATION           0x0220
#define UMAC_ASSERT_PENDING_RX_PKT_TO_HOST_HEAD_NULL_ERROR 0x0221
#define UMAC_ASSERT_DUP_CERT_MALLOC_FAILED                 0x0222
#define UMAC_ASSERT_TRY_TO_FREE_FLASH_LOCATION             0x22A8
#define UMAC_ASSERT_MEMORY_REQUIREMENT_EXCEEDED            0x02A9
#define UMAC_ASSERT_DH_MALLOC_FAILED                       0x2221
#define UMAC_ASSERT_BGET_ALLOC_FAILED                      0x2222
#define UMAC_ASSERT_NXPKT_FREE_FAILED_1                    0x2223
#define UMAC_ASSERT_NXPKT_FREE_FAILED_2                    0x2224
#define UMAC_ASSERT_NXPKT_FREE_FAILED_3                    0x2225
#define UMAC_ASSERT_NXPKT_FREE_FAILED_4                    0x2226
#define UMAC_ASSERT_ALLOC_FAILED_IN_SSL_TCP_SEND           0x2227
#define UMAC_ASSERT_NXPKT_FREE_FAILED_5                    0x2228
#define UMAC_ASSERT_WEBS_MALLOC_FAILED                     0x2229
#define UMAC_ASSERT_WEBS_SIZE_NOT_SUPPORTED                0x222a
#define UMAC_ASSERT_MALLOC_FAIL_IN_SSL_RX                  0x222b
#define UMAC_ASSERT_NXPKT_FREE_FAILED_6                    0x222c
#define UMAC_ASSERT_NXPKT_FREE_FAILED_7                    0x222d
#define UMAC_ASSERT_BINAY_FIRST_WORD_ZERO                  0x222e
#define UMAC_ASSERT_INVALID_WSC_ASSERT                     0x222f
#define UMAC_ASSERT_NO_BUFFER_FOR_ASYNC_MSG_IN_STATE_I     0x8230
#define UMAC_ASSERT_NO_BUFFER_FOR_ASYNC_MSG_IN_STATE_II    0x8231
#define UMAC_ASSERT_NO_BUFFER_FOR_ASYNC_MSG_IN_STATE_III   0x8232
#define UMAC_ASSERT_DES_IS_NOT_ENABLED                     0x0233
#define UMAC_ASSERT_MEMORY_REQUIREMENT_EXCEEDED_BY_COEX    0x0234

/*NETX ASSERTION*/

#define UMAC_ASSERT_NETX_POOL_1_CREATION_FAILED                         0x2421
#define UMAC_ASSERT_NETX_POOL_2_CREATION_FAILED                         0x2422
#define UMAC_ASSERT_NETX_IP_CREATE_FAIL                                 0x2423
#define UMAC_ASSERT_NETX_ARP_CREATE_FAIL                                0x2424
#define UMAC_ASSERT_NETX_UDP_ENABLE_FAIL                                0x2425
#define UMAC_ASSERT_NETX_TCP_ENABLE_FAIL                                0x2426
#define UMAC_ASSERT_NETX_ICMP_ENABLE_FAIL                               0x2427
#define UMAC_ASSERT_NETX_IGMP_ENABLE_FAIL                               0x2428
#define UMAC_ASSERT_NETX_DNS_CREATE_AND_INIT_FAIL                       0x2429
#define UMAC_ASSERT_NETX_DNS_POOL_SET_FAIL                              0x243A
#define UMAC_ASSERT_NETX_DHCPV6_SERVER_ERROR_HANDLER                    0x443B
#define UMAC_ASSERT_NETX_DHCPV4_SERVER_CREATE_FAIL                      0x243C
#define UMAC_ASSERT_NETX_DHCPV4_SERVER_CREATE_LIST_FAIL                 0x243D
#define UMAC_ASSERT_NETX_DHCPV4_SERVER_NETWRK_PARAM_ADD_FAIL            0x243E
#define UMAC_ASSERT_NETX_DHCPV4_SERVER_START_FAIL                       0x243F
#define UMAC_ASSERT_NETX_IPV6_LINK_LOCAL_ADDRESS_SET_FAIL               0x2440
#define UMAC_ASSERT_NETX_IPV6_GLOBAL_ADDRESS_SET_FAIL                   0x2441
#define UMAC_ASSERT_NETX_IPV6_ROUTER_ADD_FAIL                           0x2442
#define UMAC_ASSERT_NETX_DHCPV6_SERVER_CREATE_FAIL                      0x2443
#define UMAC_ASSERT_NETX_DHCPV6_SERVER_SET_DUID_CREATE_FAIL             0x2444
#define UMAC_ASSERT_NETX_DHCPV6_SERVER_IP_ADDRESS_RANGE_ADD_FAIL        0x2445
#define UMAC_ASSERT_NETX_DHCPV6_SERVER_START_FAIL                       0x2446
#define UMAC_ASSERT_NETX_DHCPV4_CLIENT_CREATE_FAIL                      0x2447
#define UMAC_ASSERT_NETX_DHCPV4_CLIENT_POOL_SET_FAIL                    0x2448
#define UMAC_ASSERT_NETX_DHCPV4_CLIENT_REQUEST_IP_FAIL                  0x4449
#define UMAC_ASSERT_NETX_DHCPV4_CLIENT_STATE_CHANGE_NOTIFY_FAIL         0x244A
#define UMAC_ASSERT_NETX_DHCPV4_CLIENT_STATE_FAIL                       0x244B
#define UMAC_ASSERT_NETX_DHCPV6_CLIENT_CREATE_FAIL                      0x244C
#define UMAC_ASSERT_NETX_DHCPV6_CLIENT_DUID_CREATE_FAIL                 0x244D
#define UMAC_ASSERT_NETX_DHCPV6_CLIENT_IANA_CREATE_FAIL                 0x244E
#define UMAC_ASSERT_NETX_DHCPV6_CLIENT_START_FAIL                       0x244F
#define UMAC_ASSERT_NETX_DHCPV6_CLIENT_REQ_DNS_SERVER_FAIL              0x4450
#define UMAC_ASSERT_NETX_DHCPV6_CLIENT_REQUSET_SOLICIT_FAIL             0x4451
#define UMAC_ASSERT_NETX_DNS_SERVER_ADD_FAIL                            0x4452
#define UMAC_ASSERT_NETX_DNS_THREAD_CREATE_FAIL                         0x2453
#define UMAC_ASSERT_NETX_RECEIVED_NULL_IN_CONVERTING_RX_TO_NX           0x2454
#define UMAC_ASSERT_NETX_RECEIVED_NOT_IPV4_OR_IPV6_PKT                  0x2455
#define UMAC_ASSERT_NETX_RECEIVED_NULL_WHILE_SENDING_PKT_TO_LMAC        0x2456
#define UMAC_ASSERT_NETX_MEM_LEAK_IN_TX_PKT_TO_LMAC                     0x0457
#define UMAC_ASSERT_NETX_NOT_TCP_OR_UDP_PKT                             0x2258
#define UMAC_ASSERT_NETX_NOT_TCP_OR_UDP_WHILE_SOCKET_ALLOC              0x2459
#define UMAC_ASSERT_NETX_UDP_SOCK_UNBIND_FAIL                           0x445A
#define UMAC_ASSERT_NETX_UDP_SOCK_DELETE_FAIL                           0x445B
#define UMAC_ASSERT_NETX_TCP_SOCK_DISCONNECT_FAIL                       0x445C
#define UMAC_ASSERT_NETX_TCP_SOCK_UNBIND_FAIL                           0x445D
#define UMAC_ASSERT_NETX_TCP_SOCK_UNLISTEN_FAIL                         0x445E
#define UMAC_ASSERT_NETX_TCP_SOCK_DELETE_FAIL_1                         0x445F
#define UMAC_ASSERT_NETX_NON_TCP_CLOSE                                  0x0460
#define UMAC_ASSERT_NETX_TCP_SOCK_UNBIND_FAIL_1                         0x4461
#define UMAC_ASSERT_NETX_TCP_SOCK_UNBIND_FAIL_2                         0x4462
#define UMAC_ASSERT_NETX_TCP_SOCK_CREATE_FAIL                           0x4464
#define UMAC_ASSERT_NETX_TCP_SOCK_RELISTEN_FAIL                         0x4465
#define UMAC_ASSERT_NETX_TCP_SOCK_ACCEPT_FAIL                           0x4466
#define UMAC_ASSERT_NETX_TCP_SOCK_ESTABLISH_NOTIFY_FAIL                 0x4467
#define UMAC_ASSERT_NETX_TCP_SOCK_RECEIVE_NOTIFY_FAIL                   0x4468
#define UMAC_ASSERT_NETX_TCP_SOCK_DELETE_FAIL_3                         0x4469
#define UMAC_ASSERT_NETX_TCP_SOCK_CLIENT_UNBLIND_FAIL                   0x446A
#define UMAC_ASSERT_NETX_TCP_SOCK_SERVER_RELISTEN_FAIL                  0x446B
#define UMAC_ASSERT_NETX_TCP_SOCK_SERVER_ACCEPT_FAIL                    0x446C
#define UMAC_ASSERT_NETX_TCP_SOCK_CLIENT_UNBLIND_FAIL_1                 0x446D
#define UMAC_ASSERT_NETX_TCP_SOCK_CLIENT_DELETE_FAIL                    0x446E
#define UMAC_ASSERT_NETX_SOCK_CLOSE_INIT_FROM_REMOTE_FAIL               0x446F
#define UMAC_ASSERT_NETX_HTTP_CLIENT_CREATION_FAILED                    0x2470
#define UMAC_ASSERT_NETX_HTTP_CLIENT_DELETE_FAIL                        0x2471
#define UMAC_ASSERT_NETX_FTP_CLIENT_DELETE_FAIL                         0x2472
#define UMAC_ASSERT_NETX_FTP_CLIENT_DISCONNECT_FAIL                     0x2473
#define UMAC_ASSERT_NETX_IPV6_ADDRESS_DELETE_FAIL_0                     0x4472
#define UMAC_ASSERT_NETX_IPV6_ADDRESS_DELETE_FAIL_1                     0x4473
#define UMAC_ASSERT_NETX_DHCPV4_CLIENT_DELETE_FAIL                      0x2474
#define UMAC_ASSERT_NETX_DHCPV6_CLIENT_DELETION_FAILED                  0x2475
#define UMAC_ASSERT_NETX_WRAP_SOCKET_CLOSE_FROM_HOST_FAIL               0x2476
#define UMAC_ASSERT_NETX_SNTP_CLIENT_DELETE_FAIL                        0x2477
#define UMAC_ASSERT_NETX_WRAP_DNS_SERVER_DELETE_FAIL                    0x4477
#define UMAC_ASSERT_NETX_WRAP_DNS_SERVER_REMOVE_FAIL                    0x4478
#define UMAC_ASSERT_NETX_WRAP_DHCPV4_START_FAIL                         0x2479
#define UMAC_ASSERT_NETX_WRAP_DHCPV4_CLIENT_INIT_FAIL                   0x447A
#define UMAC_ASSERT_NETX_WRAP_DHCPV6_START_FAIL                         0x247B
#define UMAC_ASSERT_NETX_WRAP_DHCPV6_CLIENT_INIT_FAIL                   0x447C
#define UMAC_ASSERT_NETX_WRAP_STATIC_IPV4_INIT_FAIL                     0x447D
#define UMAC_ASSERT_NETX_WRAP_STATIC_IPV4_INTF_ADDRESS_SET_FAIL         0x447E
#define UMAC_ASSERT_NETX_WRAP_STATIC_IPV4_GATEWAY_ADDRESS_SET_FAIL      0x447F
#define UMAC_ASSERT_NETX_WRAP_STATIC_IPV6_INIT_FAIL                     0x4480
#define UMAC_ASSERT_NETX_WRAP_STATIC_IPV6_ADDRESS_CHANGE_NOTIFY_FAIL    0x4481
#define UMAC_ASSERT_NETX_WRAP_STATIC_IPV6_LOCAL_ADDRESS_SET_FAIL        0x4482
#define UMAC_ASSERT_NETX_WRAP_STATIC_IPV6_ADDRESS_SET_FAIL              0x4483
#define UMAC_ASSERT_NETX_WRAP_STATIC_IPV6_ROUTER_ADD_FAIL               0x4484
#define UMAC_ASSERT_NETX_WRAP_AP_MODE_STATIC_IPV4_INIT_FAIL             0x4485
#define UMAC_ASSERT_NETX_WRAP_AP_MODE_STATIC_IPV4_DHCP_SERVER_INIT_FAIL 0x4486
#define UMAC_ASSERT_NETX_WRAP_AP_MODE_STATIC_IPV6_INIT_FAIL             0x4487
#define UMAC_ASSERT_NETX_WRAP_AP_MODE_STATIC_IPV6_DHCP_SERVER_INIT_FAIL 0x4488
#define UMAC_ASSERT_NETX_WRAP_TCP_SOCKET_CREATE_FAIL                    0x4489
#define UMAC_ASSERT_NETX_WRAP_TCP_SERVER_SOCK_ACCPET_FAIL               0x448A
#define UMAC_ASSERT_NETX_WRAP_UDP_SOCK_DELETE_FAIL                      0x248B
#define UMAC_ASSERT_NETX_WRAP_SOCKET_HANDLER_IS_NOT_TCP                 0x248C
#define UMAC_ASSERT_NETX_WRAP_UDP_DATA_SEND_FAIL                        0x448D
#define UMAC_ASSERT_NETX_WRAP_TCP_DATA_SEND_FAIL                        0x448E
#define UMAC_ASSERT_NETX_WRAP_NOT_UDP_OR_TCP                            0x248F
#define UMAC_ASSERT_NETX_WRAP_GET_PROTOCOL_NOT_UDP_OR_TCP               0x2491
#define UMAC_ASSERT_NETX_WRAP_IGMP_MULTICAST_JOIN_FAIL                  0x4492
#define UMAC_ASSERT_NETX_WRAP_IGMP_MULTICAST_LEAVE_FAIL                 0x4493
#define UMAC_ASSERT_NETX_SNMP_ERROR_WRONG_TYPE                          0x4494
#define UMAC_ASSERT_NETX_SNMP_CALL_BACK_RETURN_FAIL                     0x4495
#define UMAC_ASSERT_NETX_SNMP_ERROR_WRONG_OBJECT_TYPE                   0x4496
#define UMAC_ASSERT_NETX_SNMP_OS_MALLOC_FAILED                          0x2497
#define UMAC_ASSERT_NETX_SNMP_FLAG_CREATE_FAIL                          0x2498
#define UMAC_ASSERT_NETX_SNMP_AGENT_CREATE_FAIL                         0x4499
#define UMAC_ASSERT_NETX_SNMP_AGENT_ENGINE_SET_FAIL                     0x249A
#define UMAC_ASSERT_NETX_SNMP_AGENT_NAME_SET_FAIL                       0x249B
#define UMAC_ASSERT_NETX_STUB_NULL_VF_PTR                               0x449C
#define UMAC_ASSERT_NETX_STUB_FX_FILE_CREATE_FAIL                       0x449D
#define UMAC_ASSERT_NETX_STUB_FX_FILE_DELETE_FAIL                       0x449E
#define UMAC_ASSERT_NETX_STUB_POST_FUNCTION_NULL_PTR                    0x449F
#define UMAC_ASSERT_NETX_STUB_FX_FILE_OPEN_NULL_PTR                     0x44A0
#define UMAC_ASSERT_NETX_STUB_FXFILE_FXPTR_NOT_SAME                     0x44A1
#define UMAC_ASSERT_NETX_STUB_FX_FILE_CLOSE_FAIL                        0x44A2
#define UMAC_ASSERT_NETX_STUB_FX_FILE_READ_FAIL                         0x44A3
#define UMAC_ASSERT_NETX_STUB_FX_FILE_WRITE_FAIL                        0x44A4
#define UMAC_ASSERT_NETX_STUB_NOT_IMPLEMENTED                           0x24A5
#define UMAC_ASSERT_NETX_TCP_SOCK_UNBIND_FAIL_3                         0x44A6
#define UMAC_ASSERT_NETX_TCP_SOCK_DELETE_FAIL_2                         0x44A7
#define UMAC_ASSERT_NETX_POST_PKT_RELEASE_FAILED                        0x24A8
#define UMAC_ASSERT_NETX_PUT_PKT_RELEASE_FAILED                         0x24A9
#define UMAC_ASSERT_NETX_OTAF_REQ_PKT_RELEASE_FAILED                    0x24B0
#define UMAC_ASSERT_NETX_TCP_PERIODIC_PROCESS_FAILED                    0x24B1
#ifdef CHIP_9117
#define UMAC_ASSERT_NETX_POOL_3_CREATION_FAILED 0x24B2
#endif

/*LMAC APME AND SME ASSERTION*/
#define UMAC_ASSERT_APME_PREPARING_BEACON_OR_PROBE_RSP_PKT_LEN_EXCEEDED_512 0x2100
#define UMAC_ASSERT_APME_NOT_ASSOC_NOR_REASSOC_REQ_TYPE                     0x2101
#define UMAC_ASSERT_APME_ASSOC_REASSOC_INVALID_FRAME_LENGTH                 0x2102
#define UMAC_ASSERT_APME_ERROR_TX_RX_CONVERSTION                            0x2103
#define UMAC_ASSERT_SME_TX_PKT_NEXT_CORRUPTION                              0x0104
#define UMAC_ASSERT_SME_PROBE_REQUEST_PKT_LENGTH_ERROR_1                    0x0105 //need to verify
#define UMAC_ASSERT_SME_PROBE_REQUEST_PKT_LENGTH_ERROR_2                    0x0106 //need to verify
#define UMAC_ASSERT_SME_WPA_WRONG_IE                                        0x4107
#define UMAC_ASSERT_SME_WFA_WRONG_IE                                        0x4108
#define UMAC_ASSERT_SME_WRONG_IE_1                                          0x4109
#define UMAC_ASSERT_SME_WRONG_IE_2                                          0x410A
#define UMAC_ASSERT_SME_FSM_INVALID_FRAME_TYPE                              0x210B
#define UMAC_ASSERT_SME_TA_MGMT_TYP_SET_KEYS_RESP_FAIL                      0x210c
#define UMAC_ASSERT_SME_RX_PKT_ALLOC_FAILED_NW_PARAMS_REQ                   0x210D
#define UMAC_ASSERT_SME_RX_PKT_ALLOC_FAILED_RSSI_REQ                        0x210E
#define UMAC_ASSERT_SME_RX_PKT_ALLOC_FAILED_SNR_REQ                         0x210F
#define UMAC_ASSERT_SME_RX_PKT_ALLOC_FAILED_AP_CONF_REQ                     0x2110
#define UMAC_ASSERT_SME_EXCEEDED_MAX_STATION_SUPPORT                        0x2111
#define UMAC_ASSERT_SME_HDR_MAC_STA_MAC_NOT_MACTHING_1                      0x4113
#define UMAC_ASSERT_SME_TX_PKT_ALLOC_FAIL                                   0x2114
#define UMAC_ASSERT_SME_HDR_MAC_STA_MAC_NOT_MACTHING_2                      0x2115
#define UMAC_ASSERT_SME_AP_MAC_HW_MAC_NOT_MACTHING_1                        0x2116
#define UMAC_ASSERT_SME_RX_PKT_ALLOC_FAILED_ASSCO_REQ                       0x2117
#define UMAC_ASSERT_SME_PS_NOTIFY_FRAME_FLAG_ERROR                          0x4118
#define UMAC_ASSERT_LMAC_TO_UMAC_INTERFACE_QUEUE_ERROR                      0x2119
#define UMAC_ASSERT_SME_INVALID_STATION_DISCONNECT                          0x211a
#define UMAC_ASSERT_SME_BLOCK_UNBLOCK_FAILED                                0x211b
#define UMAC_ASSERT_SME_CONNECT_FAILED                                      0x211c
#define UMAC_ASSERT_SME_STATION_RMV_FAILED                                  0x211d
#define UMAC_ASSERT_SME_NO_BUFF_FOR_ASYC_MSG                                0x211e
#define UMAC_ASSERT_LINEAR_CONVERSION_FAILED                                0x211f
#define UMAC_ASSERT_PREPEND_BUFFER_FAILED                                   0x2120
#define UMAC_ASSERT_SME_NO_BUFFER_FOR_SCAN_RSP                              0x2121
#define UMAC_ASSERT_SME_ALLOCATION_LEVEL_FAILED_0                           0x2122
#define UMAC_ASSERT_SME_ALLOCATION_LEVEL_FAILED_1                           0x2123
#define UMAC_ASSERT_SME_ALLOCATION_LEVEL_FAILED_2                           0x2124
#define UMAC_ASSERT_SME_ALLOCATION_LEVEL_FAILED_3                           0x2125
#define UMAC_ASSERT_UNSUPPORTED_BAR_PACKET_RECEIVED                         0x2126
#define UMAC_ASSER_SME_RX_PKT_ALLOC_FAILED_READ_CONNECTED_TSF               0x2127

#endif /* WLAN_ASSERT_DEF_H */
