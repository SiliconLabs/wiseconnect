#ifndef SL_MGMT_IF_THREAD_H
#define SL_MGMT_IF_THREAD_H

#include "cmsis_os2.h"
#include "sl_status.h"
#include "sli_nhcp_driver_thread.h"
#include "sli_nhcp_pkt_struct.h"
#include "sli_nhcp_memory_mgmt.h"
#include "sli_nhcp_queue_mgmt.h" // TODO: Move unnecessary includes to the C file
#include "sl_lib_utils_rtos.h"

#define SL_MGMT_IF_EVENT_HOST_TX_PENDING BIT(1)
#define SL_MGMT_IF_EVENT_HOST_RX_PENDING BIT(2)

#define SL_MGMT_IF_EVENTS (SL_MGMT_IF_EVENT_HOST_TX_PENDING | SL_MGMT_IF_EVENT_HOST_RX_PENDING)

#define SLI_MGMT_IF_TX_SCATTER_NUM SLI_NHCP_TX_SCATTER_NUM
#define SLI_MGMT_IF_RX_SCATTER_NUM SLI_NHCP_RX_SCATTER_NUM

#define SL_WLAN_MGMT_IF_PKT_BLOCK_SIZE 2048

#define SL_WLAN_MGMT_IF_TX_PKT_STRUCT_SIZE sizeof(sl_wlan_mgmt_if_tx_pkt_t)
#define SL_WLAN_MGMT_IF_RX_PKT_STRUCT_SIZE sizeof(sl_wlan_mgmt_if_rx_pkt_t)

#define sl_wlan_mgmt_if_pkt_priority_t sli_nhcp_pkt_priority_t
#define WLAN_MGMT_IF_HIGH_PRIORITY     NHCP_HIGH_PRIORITY
#define WLAN_MGMT_IF_LOW_PRIORITY      NHCP_LOW_PRIORITY

#define sl_wlan_mgmt_if_pkt_type_t  sli_nhcp_pkt_type_t
#define SL_WLAN_MGMT_IF_PKT_TYPE_TX NHCP_PKT_TYPE_TX
#define SL_WLAN_MGMT_IF_PKT_TYPE_RX NHCP_PKT_TYPE_RX

#define sl_wlan_mgmt_if_tx_pkt_t sli_nhcp_tx_pkt_t
#define sl_wlan_mgmt_if_rx_pkt_t sli_nhcp_rx_pkt_t

#define sl_mgmt_if_tx_queue_t sli_nhcp_tx_queue_t
#define sl_mgmt_if_rx_queue_t sli_nhcp_rx_queue_t

typedef struct {
  osEventFlagsId_t event_flags_id;
  /* EXT HOST/SDIO to CP Queues  */
  sl_mgmt_if_tx_queue_t *mgmt_if_tx_q;
  sl_mgmt_if_rx_queue_t *mgmt_if_rx_q;
} sl_mgmt_if_thread_ctx_t;

sl_status_t sl_mgmt_if_thread_init();

sl_status_t sl_wlan_mgmt_if_deinit();

sl_status_t sl_send_tx_pkt_to_mgmt_intf(uint8_t *tx_pkt);

sl_status_t sl_send_rx_pkt_to_mgmt_intf(uint8_t *rx_pkt);

sl_mgmt_if_tx_queue_t *get_tx_queue_addr(void);

extern sl_status_t sli_send_rx_pkt_to_command_parser(uint8_t *rx_pkt);
#endif /*SL_MGMT_IF_THREAD_H*/
