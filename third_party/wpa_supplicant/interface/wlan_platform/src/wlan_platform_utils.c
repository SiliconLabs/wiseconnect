/*******************************************************************************
* @file  wlan_platform_utils.c
* @brief
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
#include "wlan_platform_utils.h"
#include "mgmt_platform_if.h"
#include "mgmt_if_core.h"
#include "mgmt_if_utils.h"
#include "mgmt_if_header.h"
#include "sl_status.h"
#include "rsi_spi_api.h"

extern void rsi_wfd_join_timed_out(void *eloop_ctx, void *timeout_ctx);

typedef void (*eloop_timeout_handler)(void *eloop_ctx, void *user_ctx);
extern int eloop_cancel_timeout(eloop_timeout_handler handler, void *eloop_data, void *user_data);

/**
 * @brief Prepares an RX packet from a linear buffer.
 *
 * This function processes the provided linear buffer and constructs an RX packet 
 * of type `sl_wlan_mgmt_if_rx_pkt_t` using the specified parameters. It populates 
 * the RX packet with the given data, type, length, and status, and can optionally 
 * prepend additional data to the packet.
 *
 * @param[in] type Type of the RX packet.
 * @param[in] len Length of the data to be included in the RX packet.
 * @param[in] status Status associated with the RX packet.
 * @param[in] data Pointer to the buffer containing the data to be included in the RX packet.
 * @param[in] prepend_data Pointer to the data to be prepended to the RX packet (optional).
 * @param[in] prepend_len Length of the data to be prepended (optional).
 *
 * @return Pointer to the prepared RX packet of type `sl_wlan_mgmt_if_rx_pkt_t`.
 * @retval NULL Failure - If the RX packet preparation failed.
 * @retval Non-NULL SUCCESS - If the RX packet was successfully prepared.
 */

sl_wlan_mgmt_if_rx_pkt_t *sl_prepare_rx_pkt_from_linear_buffer(uint16 type,
                                                               uint16 len,
                                                               uint16 status,
                                                               uint8 *data,
                                                               uint8 *prepend_data,
                                                               uint8 prepend_len)
{
  sl_wlan_mgmt_if_rx_pkt_t *rx_pkt = NULL;
  host_desc_t *rx_host_desc        = NULL;
  uint16 buf_index                 = 0;
  uint32 scatter_count             = 0;
  uint32 cmd_len                   = 0;

  len += prepend_len;
  rx_pkt = (sl_wlan_mgmt_if_rx_pkt_t *)sl_alloc_rx_cmd_desc(type, 0, status, len);
  if (rx_pkt == NULL) {
    SL_DEBUG_LOG("[ERROR] SL_MGMT_ASSERT_LINEAR_CONVERSION_FAILED: file %s, line %d\n", __FILE__, __LINE__);
    return NULL;
  }

  rx_host_desc = SL_PKT_GET_RXPKT_HDESC_ADDR(rx_pkt);

  if (len <= SL_WLAN_MGMT_IF_PKT_BLOCK_SIZE - (SL_WLAN_MGMT_IF_RX_PKT_STRUCT_SIZE + HOST_DESC_LENGTH)) {
    rx_pkt->scatter_info[0].len = len;
    len                         = 0;
  } else {
    rx_pkt->scatter_info[0].len =
      SL_WLAN_MGMT_IF_PKT_BLOCK_SIZE - (SL_WLAN_MGMT_IF_RX_PKT_STRUCT_SIZE + HOST_DESC_LENGTH);
    len -= rx_pkt->scatter_info[0].len;
  }
  buf_index     = 1;
  scatter_count = 1;
  while (len) {
    if (buf_index >= SLI_MGMT_IF_RX_SCATTER_NUM) {
      SL_DEBUG_LOG("[ERROR] SL_MGMT_ASSERT_LINEAR_CONVERSION_FAILED: file %s, line %d\n", __FILE__, __LINE__);
      sl_free_rx_pkt(rx_pkt);
      return NULL;
    }
    rx_pkt->scatter_info[buf_index].addr = (uint32)cpf_sl_pkb_alloc(SL_WLAN_MGMT_IF_PKT_TYPE_RX);
    cmd_len                              = len < SL_WLAN_MGMT_IF_PKT_BLOCK_SIZE ? len : SL_WLAN_MGMT_IF_PKT_BLOCK_SIZE;
    rx_pkt->scatter_info[0].len          = cmd_len;
    len                                  = len - cmd_len;
    scatter_count++;
    buf_index++;
  }
  rx_pkt->scatter_count        = scatter_count;
  rx_pkt->scatter_info[0].addr = ((uint32)rx_pkt + rx_pkt->hostss_metadata.host_desc_offset + HOST_DESC_LENGTH);

  if (data != NULL) {
    if (prepend_len <= rx_pkt->scatter_info[0].len) {
      if (prepend_data != NULL) {
        //! Copy prepend buffer
        sl_memcpy((UINT8 *)rx_pkt->scatter_info[0].addr, prepend_data, prepend_len);
      }
      sl_memcpy((UINT8 *)rx_pkt->scatter_info[0].addr + prepend_len, data, rx_pkt->scatter_info[0].len - prepend_len);
      data += (rx_pkt->scatter_info[0].len - prepend_len);
      buf_index = 1;
    } else {
      buf_index = 0;
    }
    for (; buf_index < scatter_count; buf_index++) {
      sl_memcpy((UINT8 *)rx_pkt->scatter_info[buf_index].addr, data, rx_pkt->scatter_info[buf_index].len);
      data += rx_pkt->scatter_info[buf_index].len;
    }
  }
  rx_pkt->next = 0;
  return rx_pkt;
}

/**************************************************************************/ /**
 * @brief Indicates a management packet to the host.
 *
 * This function processes the provided data to prepare a management RX packet
 * and indicates it to the host for further handling. If the status is non-zero,
 * the function avoids including the payload in the prepared packet.
 *
 * @param[in] type   Packet type identifier. Must be non-zero.
 * @param[in] len    Length of the data in the buffer.
 * @param[in] status Status to be associated with the packet.
 *                   - 0: Indicates success and includes the payload.
 *                   - Non-zero: Indicates failure; the payload is excluded.
 * @param[in] data   Pointer to the data buffer to be sent in the packet.
 *
 * This function first validates the packet type and status, then prepares an RX 
 * packet using the `sl_prepare_rx_pkt_from_linear_buffer` function. If packet 
 * preparation fails, the function logs an error and triggers an assertion. 
 * Upon successful preparation, the packet is indicated to the host using the 
 * `sli_send_host_rx_pkt` function.
 *****************************************************************************/

void sl_mgmt_indicate_to_host(uint16 type, uint16 len, uint16 status, uint8 *data)
{
  if (!type) {
    return;
  }

  if (status != 0) {
    // If status is non-zero, don't send payload
    len  = 0;
    data = NULL;
  }

  // Prepare the packet with or without prepend data
  sl_wlan_mgmt_if_rx_pkt_t *rx_pkt = sl_prepare_rx_pkt_from_linear_buffer(type, len, status, data, NULL, 0);

  if (rx_pkt == NULL) {
    SL_DEBUG_LOG("[ERROR] SL_MGMT_ASSERT_PREPEND_BUFFER_FAILED: file %s, line %d\n", __FILE__, __LINE__);
    SL_MGMT_ASSERT(0);
  }
  sli_send_host_rx_pkt(rx_pkt);
  return;
}

/**************************************************************************/ /**
 * @brief Process the rx_pkt packet to the host.
 *
 * This fucntion processes the rx_pkt and forwared it to the host via `sli_send_rx_pkt_to_command_parser`.
 *
 * @param[in] rx_pkt Pointer to the RX packet to host.
 *
 * @return SL_STATUS_OK if the packet sent to the host or the response from the `sli_send_rx_pkt_to_command_parser`.
 *****************************************************************************/

sl_status_t sli_send_host_rx_pkt(uint8_t *rx_pkt)
{
  // Indicate the packet to the host
  host_desc_t *rx_host_desc = (host_desc_t *)SL_PKT_GET_RXPKT_HDESC_ADDR(rx_pkt);

  if ((mgmt_if_adapter.rejoin_going_on == SL_TRUE) && (!is_join_cmd_from_host())
      && (rx_host_desc->dword3.token != WISE_ERROR_WRONG_STATE)
      && (rx_host_desc->dword3.token != WISE_ERROR_REJOIN_HAPPENING)
      && ((rx_host_desc->dword0.frame_type != RSI_STATION_CONN_IND)
          && (rx_host_desc->dword0.frame_type != RSI_STATION_DISCON_IND)
          && (rx_host_desc->dword0.frame_type != RSI_RSP_MODULE_STATE)
          && (rx_host_desc->dword0.frame_type != SLI_WLAN_RSP_RF_POWER_LEVEL))) {
    sl_free_rx_pkt(rx_pkt);
    return SL_STATUS_OK;
  }

  if (rx_host_desc->dword0.frame_type == SLI_WLAN_RSP_JOIN) {
    set_join_cmd_from_host_flag(0);
    sl_set_wps_client(SL_FALSE);

    if ((rx_host_desc->dword3.token != 0) && (rx_host_desc->dword3.token != WISE_ERROR_REJOIN_HAPPENING)
        && (rx_host_desc->dword3.token != SLI_WLAN_WPS_NOT_SUPPORTED)
        && (rx_host_desc->dword3.token != WISE_ERROR_WRONG_STATE)) {
      if (rx_host_desc->dword0.frame_type == SLI_WLAN_RSP_JOIN) {
        if ((get_operating_mode() == WISE_CONCURRENT_MODE)
            && ((rx_host_desc->dword3.token == CONCURRENT_AP_CHANNEL_MISMATCH)
                || (rx_host_desc->dword3.token == WISE_ERROR_UNABLE_TO_FORM_AUTOGO)
                || (rx_host_desc->dword3.token == NOT_AP_INTERFACE))) {
          wise_reset_to_band(1);
        } else {
          wise_reset_to_band(0);
        }
      }
    }

    if (get_operating_mode() == WISE_MODE_P2P) {
      /*Cancel timer for  fail response of the WFD connection*/
      eloop_cancel_timeout(rsi_wfd_join_timed_out, NULL, NULL);
    }
  }

  return sli_send_rx_pkt_to_command_parser((uint8_t *)rx_pkt);
}

sl_status_t free_tx_host_q(void)
{
  sl_status_t status              = SL_STATUS_OK;
  uint32_t pkt_count              = 0;
  sl_mgmt_if_tx_queue_t *tx_queue = (sl_mgmt_if_tx_queue_t *)get_tx_queue_addr();

  // Get packet count in the TX queue
  SLI_MGMT_IF_GET_LIST_PKT_COUNT(tx_queue, pkt_count);
  SL_DEBUG_LOG("\r\nStarting to free TX queue with %u packets\r\n", pkt_count);

  while (pkt_count > 0) {
    sl_cmd_psr_tx_pkt_t *tx_pkt = NULL;
    SLI_MGMT_IF_DEQUEUE_FROM_HEAD(tx_queue, tx_pkt);
    if (tx_pkt == NULL) {
      SL_DEBUG_LOG("\r\nFailed to remove TX packet from queue. Queue may be empty.\r\n");
      return SL_STATUS_FAIL;
    }

    pkt_count--;
    // Free TX packet and check for errors
    sl_free_tx_pkt(tx_pkt);
  }

  return status;
}
