/***************************************************************************/ /**
 * @file app.c
 * @brief Application demonstrates Transceiver Mode of operation.
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

#include "cmsis_os2.h"
#include "sl_board_configuration.h"
#include "sl_utility.h"
#include "sl_si91x_driver.h"
#include "sl_wifi.h"
#include "sl_wifi_callback_framework.h"
#include <string.h>

/******************************************************
 *                    Constants
 ******************************************************/

#define TX_FRAME_TYPE_4ADDR  BIT(0)
#define TX_FRAME_TYPE_QOS    BIT(1)
#define TX_FRAME_FIXED_RATE  BIT(2)
#define TX_FRAME_TODS        BIT(3)
#define TX_FRAME_FROMDS      BIT(4)
#define TX_FRAME_CFM_TO_HOST BIT(5)

#define LLC_HEAD_LEN         8
#define MAC80211_HDR_MIN_LEN 24

#define TX_RX_FRAME_DUMP_BYTE_COUNT 20

#define DEL_PEER     0
#define ADD_PEER     1
#define MCAST_CNT    2
#define MAC_ADDR_LEN 6

#define PEER_DS_BITMAP_DATA_RATE_1   BIT(0)
#define PEER_DS_BITMAP_DATA_RATE_2   BIT(1)
#define PEER_DS_BITMAP_DATA_RATE_5P5 BIT(2)
#define PEER_DS_BITMAP_DATA_RATE_11  BIT(3)
#define PEER_DS_BITMAP_DATA_RATE_6   BIT(4)
#define PEER_DS_BITMAP_DATA_RATE_9   BIT(5)
#define PEER_DS_BITMAP_DATA_RATE_12  BIT(6)
#define PEER_DS_BITMAP_DATA_RATE_18  BIT(7)
#define PEER_DS_BITMAP_DATA_RATE_24  BIT(8)
#define PEER_DS_BITMAP_DATA_RATE_36  BIT(9)
#define PEER_DS_BITMAP_DATA_RATE_48  BIT(10)
#define PEER_DS_BITMAP_DATA_RATE_54  BIT(11)

/* Wi-Fi transceiver mode configurable options */

/*
 * Below options can be enabled in application pre-processor settings:
 * TRANSCEIVER_TX_TEST_ENABLE - enable TX test mode
 * TRANSCEIVER_RX_DBG_FILTER_BCAST_MCAST - filter out all RX bcast/mcast frames in application RX debug messages.
 * TRANSCEIVER_MAC_PEER_DS_SUPPORT - add peers in MAC layer. This is mandatory to support auto-rate and per RA per TID sequence numbers for TX unicast packets.
 * TRANSCEIVER_RX_MCAST_MAC_FILTER_EN - configure RX mcast MAC address filtering in firmware
 */

#ifdef TRANSCEIVER_TX_TEST_ENABLE
//! Configure payload length for TX packet. Max sample packet length supported is 2020 (excluding MAC header).
#define TX_SAMPLE_PACKET_LEN 2020
//! Configure number of data packets to send
#define TX_SAMPLE_PACKET_COUNT 10
//! Configure priority queue for data: AC_BE - 0, AC_BK - 1, AC_VI - 2, AC_V0 - 3
#define TX_SAMPLE_PACKET_PRIORITY_Q 2
//! Configure data rate to be used for TX packet
#define TX_SAMPLE_PACKET_DATA_RATE SL_WIFI_DATA_RATE_36
/* Control flags for data frame.
 * TX_FRAME_TYPE_4ADDR - Set to encapsulate TX data packet with 4-addr MAC header. If not set, 3-addr type MAC header shall be used
 * TX_FRAME_TYPE_QOS - If set, data packet shall be sent as QoS frame. Unset to send as non-Qos frame
 * TX_FRAME_FIXED_RATE - Send data packet with fixed rate. Use rate field in sl_wifi_transceiver_tx_data_control_t to provide the required data rate
 * TX_FRAME_TODS - Shall be set to enable ToDS bit in FC. Valid only for 3-addr frame format.
 * TX_FRAME_FROMDS - Shall be set to enable FromDS bit in FC. Valid only for 3-addr frame format.
 * TX_FRAME_CFM_TO_HOST - Shall be set if host requires confirm status for data packet
 */
#define TX_SAMPLE_PACKET_CTRL_FLAGS \
  (TX_FRAME_TYPE_QOS | TX_FRAME_TYPE_4ADDR | TX_FRAME_CFM_TO_HOST | TX_FRAME_FIXED_RATE)
#endif

//! Peer 1 destination MAC address (Addr1)
#define TX_SAMPLE_PACKET_PEER1_ADDR1   \
  {                                    \
    0x00, 0x23, 0xa7, 0x20, 0x21, 0x23 \
  }
//! Peer 2 destination MAC address (Addr1)
#define TX_SAMPLE_PACKET_PEER2_ADDR1   \
  {                                    \
    0x00, 0x23, 0xa7, 0x20, 0x21, 0x24 \
  }
//! Hard coded dummy DA MAC addresses.
#define TX_SAMPLE_PACKET_DUMMY_ADDR3_DA \
  {                                     \
    0x00, 0xDA, 0x11, 0x11, 0x11, 0x11  \
  }
//! Hard coded dummy SA MAC addresses.
#define TX_SAMPLE_PACKET_DUMMY_ADDR4_SA \
  {                                     \
    0x00, 0x5A, 0x22, 0x22, 0x22, 0x22  \
  }
//! Broadcast destination MAC address (Addr1)
#define TX_SAMPLE_PACKET_BCAST_ADDR1   \
  {                                    \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff \
  }
//! Multicast destination MAC address (Addr1)
#define TX_SAMPLE_PACKET_MCAST_ADDR1   \
  {                                    \
    0x01, 0x00, 0x5E, 0x11, 0x11, 0x11 \
  }

#ifdef TRANSCEIVER_MAC_PEER_DS_SUPPORT
#define PEER_DS_RATE_BITMAP                                                                  \
  (PEER_DS_BITMAP_DATA_RATE_1 | PEER_DS_BITMAP_DATA_RATE_2 | PEER_DS_BITMAP_DATA_RATE_5P5    \
   | PEER_DS_BITMAP_DATA_RATE_11 | PEER_DS_BITMAP_DATA_RATE_6 | PEER_DS_BITMAP_DATA_RATE_9   \
   | PEER_DS_BITMAP_DATA_RATE_12 | PEER_DS_BITMAP_DATA_RATE_18 | PEER_DS_BITMAP_DATA_RATE_24 \
   | PEER_DS_BITMAP_DATA_RATE_36 | PEER_DS_BITMAP_DATA_RATE_48 | PEER_DS_BITMAP_DATA_RATE_54)
uint8_t peer_ds_mac_1[MAC_ADDR_LEN] = TX_SAMPLE_PACKET_PEER1_ADDR1;
uint8_t peer_ds_mac_2[MAC_ADDR_LEN] = TX_SAMPLE_PACKET_PEER2_ADDR1;
#endif

#ifdef TRANSCEIVER_RX_MCAST_MAC_FILTER_EN
//! RX multicast MAC address 1 to filter in firmware
#define MCAST_FILTER_ADDR_IPV4         \
  {                                    \
    0x01, 0x00, 0x5e, 0x00, 0x01, 0x01 \
  }
//! RX multicast MAC address 2 to filter in firmware
#define MCAST_FILTER_ADDR_IPV6         \
  {                                    \
    0x33, 0x33, 0xff, 0xff, 0xff, 0xff \
  }
#endif

/******************************************************
 *               Variable Definitions
 ******************************************************/

const osThreadAttr_t thread_attributes = {
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

static const sl_wifi_device_configuration_t device_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .region_code = JP,
  .boot_config = { .oper_mode = SL_SI91X_TRANSCEIVER_MODE,
                   .coex_mode = SL_SI91X_WLAN_ONLY_MODE,
#ifdef TRANSCEIVER_MAC_PEER_DS_SUPPORT
                   .feature_bit_map = (SL_SI91X_FEAT_TRANSCEIVER_MAC_PEER_DS_SUPPORT | SL_SI91X_FEAT_SECURITY_OPEN),
#else
                   .feature_bit_map = SL_SI91X_FEAT_SECURITY_OPEN,
#endif
                   .tcp_ip_feature_bit_map = SL_SI91X_TCP_IP_FEAT_BYPASS,
                   .custom_feature_bit_map = SL_SI91X_CUSTOM_FEAT_EXTENTION_VALID,
                   .ext_custom_feature_bit_map =
                     (SL_SI91X_EXT_FEAT_XTAL_CLK | SL_SI91X_EXT_FEAT_UART_SEL_FOR_DEBUG_PRINTS
#ifdef SLI_SI917
                      | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
                      ),
                   .bt_feature_bit_map         = 0,
                   .ext_tcp_ip_feature_bit_map = (SL_SI91X_CONFIG_FEAT_EXTENTION_VALID),
                   .ble_feature_bit_map        = 0,
                   .ble_ext_feature_bit_map    = 0,
                   .config_feature_bit_map     = 0 }
};

static sl_wifi_transceiver_configuration_t transceiver_config = {
  .channel.chan_info = { .channel = SL_CHANNEL_NO, .band = SL_WIFI_BAND_2_4GHZ, .bandwidth = SL_WIFI_BANDWIDTH_20MHz },
  .channel.tx_power  = SL_TX_POWER,
  .parameters        = { .set              = 1,
                         .retransmit_count = DEFAULT_RETRANSMIT_COUNT,
                         .cw_params[0]     = { .cwmin = DEFAULT_QOS_BE_CWMIN,
                                               .cwmax = DEFAULT_QOS_BE_CWMAX,
                                               .aifsn = DEFAULT_QOS_BE_AIFSN },
                         .cw_params[1]     = { .cwmin = DEFAULT_QOS_BK_CWMIN,
                                               .cwmax = DEFAULT_QOS_BK_CWMAX,
                                               .aifsn = DEFAULT_QOS_BK_AIFSN },
                         .cw_params[2]     = { .cwmin = DEFAULT_QOS_VI_CWMIN,
                                               .cwmax = DEFAULT_QOS_VI_CWMAX,
                                               .aifsn = DEFAULT_QOS_VI_AIFSN },
                         .cw_params[3]     = { .cwmin = DEFAULT_QOS_VO_CWMIN,
                                               .cwmax = DEFAULT_QOS_VO_CWMAX,
                                               .aifsn = DEFAULT_QOS_VO_AIFSN } },
};

/******************************************************
 *               Function Declarations
 ******************************************************/

static void process_rx_data(int32_t status, uint8_t *buffer, uint32_t length, int8_t rssi, uint32_t rate);
static void process_tx_confirmation(uint16_t status, uint32_t token, uint8_t priority, uint32_t rate);
static sl_status_t transceiver_event_handler(sl_wifi_event_t event, void *data, uint32_t data_length, void *arg);
static void print_transceiver_config(sl_wifi_transceiver_configuration_t *transceiver_config);

#ifdef TRANSCEIVER_TX_TEST_ENABLE
static sl_status_t get_sample_packet(sl_wifi_transceiver_tx_data_control_t *control,
                                     uint8_t *payload,
                                     uint16_t payload_length);
static void perform_transceiver_tx_test(void);
#endif

#ifdef TRANSCEIVER_MAC_PEER_DS_SUPPORT
static void peer_update(uint8_t mac[6], uint32_t rate_bitmap, uint8_t add_flag);
#endif

#ifdef TRANSCEIVER_RX_MCAST_MAC_FILTER_EN
static int32_t get_transceiver_multicast_filter_data(sl_wifi_transceiver_mcast_filter_t *mcast);
#endif

static void application_start(void *argument);

/******************************************************
 *               Function Definitions
 ******************************************************/

osThreadId_t app_thread_id;

void app_init(const void *unused)
{
  UNUSED_PARAMETER(unused);
  app_thread_id = osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

static void application_start(void *argument)
{
  UNUSED_PARAMETER(argument);

  sl_status_t status;

  status = sl_wifi_init(&device_configuration, NULL, sl_wifi_default_event_handler);
  if (status != SL_STATUS_OK) {
    printf("\r\nTransceiver initialization failed: 0x%lX\r\n", status);
    return;
  }
  printf("\r\nTransceiver initialization successful\r\n");

  //! Register callback for SL_WIFI_TRANSCEIVER_EVENTS group.
  sl_wifi_set_transceiver_callback(transceiver_event_handler, NULL);

  status = sl_wifi_transceiver_up(SL_WIFI_TRANSCEIVER_INTERFACE, &transceiver_config);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to bring up Transceiver interface: 0x%lX\r\n", status);
    return;
  }
  printf("\r\nTransceiver interface up\r\n");

  print_transceiver_config(&transceiver_config);

#ifdef TRANSCEIVER_RX_MCAST_MAC_FILTER_EN
  sl_wifi_transceiver_mcast_filter_t mcast = { 0 };
  if (get_transceiver_multicast_filter_data(&mcast) == SL_STATUS_OK) {
    status = sl_wifi_set_transceiver_multicast_filter(SL_WIFI_TRANSCEIVER_INTERFACE, mcast);
    if (status != SL_STATUS_OK) {
      printf("\r\nSet multicast filter data failed, error code : 0x%lX\r\n", status);
    } else {
      printf("\r\nSet multicast filter data successful\r\n");
    }
  }
#endif

#ifdef TRANSCEIVER_MAC_PEER_DS_SUPPORT
  // Add peer list to MAC layer
  peer_update(peer_ds_mac_1, PEER_DS_RATE_BITMAP, ADD_PEER);
  peer_update(peer_ds_mac_2, PEER_DS_RATE_BITMAP, ADD_PEER);
#endif

#ifdef TRANSCEIVER_TX_TEST_ENABLE
  perform_transceiver_tx_test();
#endif

  //! Flush the entire TX buffer pool.
  status = sl_wifi_flush_transceiver_data(SL_WIFI_TRANSCEIVER_INTERFACE);
  if (status != SL_STATUS_OK) {
    printf("Flush queue failed, error Code : 0x%lX\r\n", status);
  } else {
    printf("Flush queue successful\r\n");
  }

#ifdef TRANSCEIVER_MAC_PEER_DS_SUPPORT
  //! Delete peer list from MAC layer
  peer_update(peer_ds_mac_1, 0, DEL_PEER);
  peer_update(peer_ds_mac_2, 0, DEL_PEER);
#endif

  osThreadSuspend(app_thread_id);
}

static void print_transceiver_config(sl_wifi_transceiver_configuration_t *transceiver_config)
{
  printf("\r\nChannel: %d", transceiver_config->channel.chan_info.channel);
  printf("\r\nRetransmit count: %d", transceiver_config->parameters.retransmit_count);
  printf("\r\nContention parameters:");
  for (uint8_t i = 0; i < 4; i++) {
    printf("\r\n  AC index[%d] - cwmin: %d, cwmax: %d, aifsn: %d",
           i,
           transceiver_config->parameters.cw_params[i].cwmin,
           transceiver_config->parameters.cw_params[i].cwmax,
           transceiver_config->parameters.cw_params[i].aifsn);
  }
  printf("\r\nDevice MAC address: %x:%x:%x:%x:%x:%x\n",
         transceiver_config->dut_mac.octet[0],
         transceiver_config->dut_mac.octet[1],
         transceiver_config->dut_mac.octet[2],
         transceiver_config->dut_mac.octet[3],
         transceiver_config->dut_mac.octet[4],
         transceiver_config->dut_mac.octet[5]);
}

#ifdef TRANSCEIVER_RX_MCAST_MAC_FILTER_EN
/*
 * @brief - Fill in sample multicast MAC address to filter RX multicast packets in firmware
 */
static int32_t get_transceiver_multicast_filter_data(sl_wifi_transceiver_mcast_filter_t *mcast)
{
  uint8_t mac[MCAST_CNT][6] = { MCAST_FILTER_ADDR_IPV4, MCAST_FILTER_ADDR_IPV6 };

  mcast->flags |= BIT(0);
  mcast->num_of_mcast_addr = 2;
  for (int i = 0; i < MCAST_CNT; i++) {
    memcpy(mcast->mac[i], mac[i], 6);
  }

  return SL_STATUS_OK;
}
#endif

#ifdef TRANSCEIVER_MAC_PEER_DS_SUPPORT
/*
 * @brief - Add/delete peer in MAC layer. 
 *
 * @param[in] mac - MAC address of peer to be added or deleted.
 * @param[in] rate_bitmap - Rate bitmap of peer.
 * @param[in] add_flag - Peer will be added if set to 1, else deleted.
 */
static void peer_update(uint8_t mac[MAC_ADDR_LEN], uint32_t rate_bitmap, uint8_t add_flag)
{
  sl_status_t status                     = SL_STATUS_OK;
  sl_wifi_transceiver_peer_update_t peer = { 0 };

  //! Fill peer data
  memcpy(peer.peer_mac_address, mac, MAC_ADDR_LEN);
  peer.peer_supported_rate_bitmap = rate_bitmap & 0x0fff;
  peer.flags                      = add_flag ? (peer.flags | BIT(0)) : (peer.flags & ~BIT(0));

  //! Call API to add peer in MAC layer
  status = sl_wifi_update_transceiver_peer_list(SL_WIFI_TRANSCEIVER_INTERFACE, peer);
  if (status != SL_STATUS_OK) {
    printf("\r\nPeer list update failed for %02x:%02x:%02x:%02x:%02x:%02x with error code: 0x%lX\r\n",
           mac[0],
           mac[1],
           mac[2],
           mac[3],
           mac[4],
           mac[5],
           status);
  } else {
    printf("\r\nPeer %s successful: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
           add_flag ? "add" : "delete",
           mac[0],
           mac[1],
           mac[2],
           mac[3],
           mac[4],
           mac[5]);
  }
}
#endif

/**
 * @brief Callback handler for asynchronous data received from the firmware in Wi-Fi transceiver mode.
 */
static void process_rx_data(int32_t status, uint8_t *buffer, uint32_t length, int8_t rssi, uint32_t rate)
{
#if TRANSCEIVER_RX_DBG_FILTER_BCAST_MCAST
  if (IS_BCAST_MCAST_MAC(buffer[4])) {
    return;
  }
#endif
  printf("Status[0x%lx]Len[%ld]", status, length);
  printf("RSSI[%d]Rate[0x%lx] ", rssi, rate);

  print_80211_packet(buffer, length, TX_RX_FRAME_DUMP_BYTE_COUNT);
}

/**
 * @brief Callback handler for TX data packet status report in Wi-Fi transceiver mode.
 */
static void process_tx_confirmation(uint16_t status, uint32_t token, uint8_t priority, uint32_t rate)
{
  printf("\nStatus[0x%x] Token[0x%lx]", status, token);
  printf(" Priority[0x%x] Rate[0x%lx]\n", priority, rate);
}

//! Callback for SL_WIFI_TRANSCEIVER_EVENTS group
static sl_status_t transceiver_event_handler(sl_wifi_event_t event, void *data, uint32_t data_length, void *arg)
{
  UNUSED_PARAMETER(data_length);
  UNUSED_PARAMETER(arg);

  if (SL_WIFI_CHECK_IF_EVENT_FAILED(event)) {
    return SL_STATUS_FAIL;
  }

  switch (event) {
    case SL_WIFI_TRANSCEIVER_RX_DATA_RECEIVE_CB: {
      sl_wifi_transceiver_rx_data_t *rx_data = (sl_wifi_transceiver_rx_data_t *)data;
      process_rx_data(rx_data->status, rx_data->buffer, rx_data->length, rx_data->rssi, rx_data->rate);
      break;
    }
    case SL_WIFI_TRANSCEIVER_TX_DATA_STATUS_CB: {
      sl_wifi_transceiver_tx_data_confirmation_t *confirmation = (sl_wifi_transceiver_tx_data_confirmation_t *)data;
      process_tx_confirmation(confirmation->status, confirmation->token, confirmation->priority, confirmation->rate);
      break;
    }
    default:
      return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

#ifdef TRANSCEIVER_TX_TEST_ENABLE
/**
 * @brief Create a sample payload based on user configured params.
 *
 * @param[out] control - Meta data for payload
 * @param[out] payload - Sample payload
 * @param[in] payload_len - Length of payload
 */
static sl_status_t get_sample_packet(sl_wifi_transceiver_tx_data_control_t *control,
                                     uint8_t *payload,
                                     uint16_t payload_length)
{
  uint32_t sample_data             = 0;
  static uint16_t token            = 0;
  uint32_t pos                     = 0;
  uint8_t llc_header[LLC_HEAD_LEN] = { 0xaa, 0xaa, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00 };
  uint8_t addr1_ra[MAC_ADDR_LEN]   = TX_SAMPLE_PACKET_PEER1_ADDR1;    //RA
  uint8_t addr3_da[MAC_ADDR_LEN]   = TX_SAMPLE_PACKET_DUMMY_ADDR3_DA; //DA
  uint8_t addr4_sa[MAC_ADDR_LEN]   = TX_SAMPLE_PACKET_DUMMY_ADDR4_SA; //SA

  memset(control, 0, sizeof(sl_wifi_transceiver_tx_data_control_t));

  control->ctrl_flags = TX_SAMPLE_PACKET_CTRL_FLAGS;

  if (control->ctrl_flags & TX_FRAME_TYPE_4ADDR) {
    memcpy(control->addr4, addr4_sa, MAC_ADDR_LEN);
  }

  if (control->ctrl_flags & TX_FRAME_TYPE_QOS) {
    control->priority = TX_SAMPLE_PACKET_PRIORITY_Q;
  }

  if (control->ctrl_flags & TX_FRAME_FIXED_RATE) {
    control->rate = TX_SAMPLE_PACKET_DATA_RATE;
  }

  //! Token is used for synchronization between data packets sent and reports received.
  //! MAC layer shall send the same token/identifier in status report along with the status of the transmitted packet.
  control->token = token++;
  if (token > 0xfff) {
    token = 0;
  }

  memcpy(control->addr1, addr1_ra, MAC_ADDR_LEN);
  memcpy(control->addr2, &transceiver_config.dut_mac.octet, MAC_ADDR_LEN);
  memcpy(control->addr3, addr3_da, MAC_ADDR_LEN);

  memset(payload, 0, payload_length);

  if (payload_length < 1) {
    return SL_STATUS_FAIL;
  }

  /* Based on available payload length, fill dummy LLC header */
  if (payload_length > LLC_HEAD_LEN) {
    memcpy(payload, llc_header, LLC_HEAD_LEN);
    pos += LLC_HEAD_LEN;
  }

  /* Fill payload */
  while (pos < payload_length) {
    payload[pos++] = ++sample_data;
  }

  return SL_STATUS_OK;
}

static void perform_transceiver_tx_test(void)
{
  sl_status_t status;
  sl_wifi_transceiver_tx_data_control_t control;
  uint8_t *payload = malloc(TX_SAMPLE_PACKET_LEN);

  if (payload == NULL) {
    printf("Failed to allocate payload\r\n");
    return;
  }

  for (int packet_count = TX_SAMPLE_PACKET_COUNT; packet_count > 0; packet_count--) {
    status = get_sample_packet(&control, payload, TX_SAMPLE_PACKET_LEN);
    if (status != SL_STATUS_OK) {
      printf("Packet format error\r\n");
    }

    status = sl_wifi_send_transceiver_data(SL_WIFI_TRANSCEIVER_INTERFACE, &control, payload, TX_SAMPLE_PACKET_LEN);
    if (status != SL_STATUS_OK) {
      printf("Sending data packet failed with error: 0x%lX\r\n", status);
      break;
    }
  }

  free(payload);
}
#endif
