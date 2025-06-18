/*******************************************************************************
* @file  mgmt_if_thread.c
* @brief This contains Management i/f thread and events initialization.
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
#include "cmsis_os2.h"
#include "sl_mgmt_if_thread.h"
#include "mgmt_if_header.h"
#include "mgmt_if_core.h"

static void mgmt_if_thread(void *argument);
static sl_status_t handle_mgmt_if_tx_event(sl_mgmt_if_thread_ctx_t *mgmt_if_ctx);
static sl_status_t handle_mgmt_if_rx_event(sl_mgmt_if_thread_ctx_t *mgmt_if_ctx);

sl_mgmt_if_thread_ctx_t *mgmt_if_thread_ctx = NULL;

static osThreadId_t mgmt_if_thread_id;

sl_mgmt_if_tx_queue_t *get_tx_queue_addr(void)
{
  return (mgmt_if_thread_ctx->mgmt_if_tx_q);
}

/***************************************************************************/ /**
 * @brief Initializes the Management interface and its resources.
 *
 * This function allocates memory for the Management interface context, initializes TX
 * and RX queues, creates event flags, and starts the thread. If any
 * part of the initialization fails, it performs cleanup and returns an
 * appropriate error status.
 *
 * @return SL_STATUS_OK if the initialization is successful, otherwise an
 *         error status.
 ******************************************************************************/
sl_status_t sl_mgmt_if_thread_init()
{
  sl_mgmt_if_thread_ctx_t *ctx = (sl_mgmt_if_thread_ctx_t *)sl_malloc(sizeof(sl_mgmt_if_thread_ctx_t));
  if (ctx == NULL) {
    SL_DEBUG_LOG("\r\nFailed to allocate memory for management interface context\r\n");
    return SL_STATUS_ALLOCATION_FAILED;
  }

  sl_status_t status = SL_STATUS_OK;

  // Initialize TX queues
  status = SLI_MGMT_IF_INIT_TX_QUEUE(&ctx->mgmt_if_tx_q, "Mgmt IF TX Queue");
  if (status != SL_STATUS_OK)
    goto cleanup;

  // Initialize RX queues
  status = SLI_MGMT_IF_INIT_RX_QUEUE(&ctx->mgmt_if_rx_q, "Mgmt IF RX Queue");
  if (status != SL_STATUS_OK)
    goto cleanup;

  // Initialize event flags
  ctx->event_flags_id = osEventFlagsNew(NULL);
  if (ctx->event_flags_id == NULL) {
    SL_DEBUG_LOG("\r\nFailed to create event flags\r\n");
    status = SL_STATUS_FAIL;
    goto cleanup;
  } else {
    SL_DEBUG_LOG("\r\nEvent flags created successfully\r\n");
  }

  // Define and create the mgmt if thread
  const osThreadAttr_t mgmt_if_thread_attributes = {
    .name       = "mgmt_if_thread",
    .attr_bits  = 0,
    .cb_mem     = NULL,
    .cb_size    = 0,
    .stack_mem  = NULL,
    .stack_size = 5120,
    .priority   = osPriorityNormal,
    .tz_module  = 0,
    .reserved   = 0,
  };

  mgmt_if_thread_id = osThreadNew((osThreadFunc_t)mgmt_if_thread, ctx, &mgmt_if_thread_attributes);
  if (mgmt_if_thread_id == NULL) {
    SL_DEBUG_LOG("\r\nFailed to create mgmt i/f thread\r\n");
    osEventFlagsDelete(ctx->event_flags_id);
    status = SL_STATUS_FAIL;
    goto cleanup;
  } else {
    SL_DEBUG_LOG("\r\nMgmt i/f thread created successfully\r\n");
  }

  mgmt_if_thread_ctx = ctx; // Set the global context only if initialization succeeds

  SL_DEBUG_LOG("\r\nCP driver initialized successfully\r\n");
  return SL_STATUS_OK;

cleanup:
  // Cleanup in case of failure
  if (ctx->event_flags_id)
    osEventFlagsDelete(ctx->event_flags_id);

  if (ctx->mgmt_if_rx_q) {
    sl_free(ctx->mgmt_if_rx_q);
  }

  if (ctx->mgmt_if_tx_q) {
    sl_free(ctx->mgmt_if_tx_q);
  }

  sl_free(ctx);

  SL_DEBUG_LOG("\r\nMgmt I/f driver initialization failed with status: 0x%lX\r\n", status);
  return status;
}

/***************************************************************************/ /**
 * @brief Thread function to handle mgmt interface events.
 *
 * This function runs in an infinite loop, waiting for and processing mgmt intf
 * events based on flags set by the driver. The events include various
 * operations like transmission and reception of packets. 
 * It processes each event type accordingly and handles errors
 * that occur during event processing.
 *
 * @param argument Pointer to the mgmt interface context. This is cast to the
 *                 appropriate type within the function.
 ******************************************************************************/
static void mgmt_if_thread(void *argument)
{
  sl_mgmt_if_thread_ctx_t *mgmt_if_ctx = (sl_mgmt_if_thread_ctx_t *)argument;
  if (mgmt_if_ctx == NULL) {
    return;
  }

  while (1) {
    // Wait for any of the events to occur
    uint32_t pending_events =
      osEventFlagsWait(mgmt_if_ctx->event_flags_id, SL_MGMT_IF_EVENTS, osFlagsWaitAny, osWaitForever);

    // Process events
    if (pending_events & SL_MGMT_IF_EVENT_HOST_TX_PENDING) {
      sl_status_t status = handle_mgmt_if_tx_event(mgmt_if_ctx);
      if (status != SL_STATUS_OK) {
        SL_DEBUG_LOG("\r\nFailed to handle HOST TX pending event : 0x%lX\r\n", status);
      }
    }

    if (pending_events & SL_MGMT_IF_EVENT_HOST_RX_PENDING) {
      sl_status_t status = handle_mgmt_if_rx_event(mgmt_if_ctx);
      if (status != SL_STATUS_OK) {
        SL_DEBUG_LOG("\r\nFailed to handle HOST Rx pending event : 0x%lX\r\n", status);
      }
    }
  }
}

/***************************************************************************/ /**
 * @brief  Handles the HostSS TX done event by processing and sending TX packets
 *         to the network processor. The function also clears the relevant event
 *         flag once all packets are processed.
 *
 * This function processes packets in the HostSS TX queue, sends them to the network
 * processor, and clears the TX done event flag.
 *
 * @param mgmt_if_ctx Pointer to the driver context structure.
 * @return sl_status_t Status of the operation. Returns SL_STATUS_OK on success,
 *                     or an appropriate error code on failure.
 ******************************************************************************/
static sl_status_t handle_mgmt_if_rx_event(sl_mgmt_if_thread_ctx_t *mgmt_if_ctx)
{
  sl_wlan_mgmt_if_rx_pkt_t *rx_pkt    = NULL;
  sl_mgmt_if_rx_queue_t *mgmt_if_rx_q = mgmt_if_ctx->mgmt_if_rx_q;
  sl_status_t status                  = SL_STATUS_OK;
  uint32_t pkt_count                  = 0;
  uint32_t os_status                  = osOK;

  // Process each packet in the TX queue
  SLI_MGMT_IF_GET_LIST_PKT_COUNT(mgmt_if_rx_q, pkt_count);
  while (pkt_count > 0) {
    SLI_MGMT_IF_DEQUEUE_FROM_HEAD(mgmt_if_rx_q, rx_pkt);
    if (rx_pkt == NULL) {
      SL_DEBUG_LOG("\r\nFailed to remove Rx packet from queue\r\n");
      status = SL_STATUS_FAIL;
      break;
    }

    pkt_count--;
    // Process rx_pkt
    process_rx_pkt_from_nwp(rx_pkt);
  }
  // Clear the event flag for management interface RX pending
  os_status = osEventFlagsClear(mgmt_if_ctx->event_flags_id, SL_MGMT_IF_EVENT_HOST_RX_PENDING);
  if (os_status & osFlagsError) { // Check for errors
    SL_DEBUG_LOG("\r\nFailed to clear management interface RX  pending event flag: 0x%lX\r\n", os_status);
    return SL_STATUS_FAIL;
  }

  return status;
}

/***************************************************************************/ /**
 * @brief  Handles the HostSS TX done event by processing and sending TX packets
 *         to the network processor. The function also clears the relevant event
 *         flag once all packets are processed.
 *
 * This function processes packets in the HostSS TX queue, sends them to the network
 * processor, and clears the TX done event flag.
 *
 * @param mgmt_if_ctx Pointer to the driver context structure.
 * @return sl_status_t Status of the operation. Returns SL_STATUS_OK on success,
 *                     or an appropriate error code on failure.
 ******************************************************************************/
static sl_status_t handle_mgmt_if_tx_event(sl_mgmt_if_thread_ctx_t *mgmt_if_ctx)
{
  sl_wlan_mgmt_if_tx_pkt_t *tx_pkt    = NULL;
  sl_mgmt_if_tx_queue_t *mgmt_if_tx_q = mgmt_if_ctx->mgmt_if_tx_q;
  sl_status_t status                  = SL_STATUS_OK;
  uint32_t pkt_count                  = 0;
  uint32_t os_status                  = osOK;
  mgmt_command_status_t cmd_status    = { 0 };

  // Process each packet in the TX queue
  SLI_MGMT_IF_GET_LIST_PKT_COUNT(mgmt_if_tx_q, pkt_count);
  while (pkt_count > 0) {
    SLI_MGMT_IF_DEQUEUE_FROM_HEAD(mgmt_if_tx_q, tx_pkt);
    if (tx_pkt == NULL) {
      SL_DEBUG_LOG("\r\nFailed to remove TX packet from queue\r\n");
      status = SL_STATUS_FAIL;
      break;
    }

    pkt_count--;
    host_desc_t *host_desc = (host_desc_t *)&tx_pkt->host_desc;
    // tx_pkt->hostss_metadata.pkt_len = sizeof(host_desc_t) + ((host_desc->dword0.length) & 0xFFF);
    if (host_desc->dword0.queue_num & BIT(3)) {
      host_desc->dword0.queue_num &= ~BIT(3);
    }
    if (host_desc->dword0.queue_num == 4) {
      wlan_mgmt_if_cmd_handler(tx_pkt);
    } else {
      SL_MGMT_ASSERT(0);
    }

    if (status != SL_STATUS_OK) {
      // TODO: handle failure case
      SL_DEBUG_LOG("\r\nFailed to send TX packet: 0x%lX\r\n", status);
      SL_MGMT_ASSERT(0);
    }
  }

  // Clear the event flag for management interface TX pending
  os_status = osEventFlagsClear(mgmt_if_ctx->event_flags_id, SL_MGMT_IF_EVENT_HOST_TX_PENDING);
  if (os_status & osFlagsError) { // Check for errors
    SL_DEBUG_LOG("\r\nFailed to clear management interface TX pending event flag: 0x%lX\r\n", os_status);
    return SL_STATUS_FAIL;
  }

  return status;
}

/***************************************************************************/ /**
 * @brief Deinitialize the Command parser and free all allocated resources.
 *
 * @return sl_status_t Returns SL_STATUS_OK on success or appropriate error code.
 ******************************************************************************/
sl_status_t sl_wlan_mgmt_if_deinit()
{
  if (mgmt_if_adapter.state > WISE_STATE_BAND_DONE) {
    SL_DEBUG_LOG("[ERROR] Module state is greater than BAND_DONE is NULL, cannot deinitialize.\r\n");
    return SL_STATUS_FAIL;
  }

  // Check if the driver context is NULL
  if (mgmt_if_thread_ctx == NULL) {
    SL_DEBUG_LOG("[ERROR] mgmt_if_thread_ctx is NULL, cannot deinitialize.\r\n");
    return SL_STATUS_NULL_POINTER;
  }

  sl_status_t status = SL_STATUS_OK;

  // Terminate the Supplicant event loop  thread
  SL_DEBUG_LOG("[INFO] Terminating MGMT Interface thread...\r\n");
  osThreadTerminate(mgmt_if_thread_id);

  // Check if the thread was terminated successfully
  osThreadState_t state = osThreadGetState(mgmt_if_thread_id);
  if (state == osThreadTerminated) {
    SL_DEBUG_LOG("[INFO] MGMT Interface  thread terminated successfully.\r\n");
  } else {
    SL_DEBUG_LOG("[ERROR]MGMT Interface thread termination failed. Current state: %d\r\n", state);
    return SL_STATUS_FAIL;
  }

  // Delete the event flags
  SL_DEBUG_LOG("[INFO]MGMT Interface event flags...\r\n");
  osEventFlagsDelete(mgmt_if_thread_ctx->event_flags_id);

  // Free cmd_parser_rx_q
  SL_DEBUG_LOG("[INFO] Freeing mgmt_if_rx_q...\r\n");
  status = SLI_MGMT_IF_PURGE_QUEUE(NHCP_PKT_TYPE_RX, mgmt_if_thread_ctx->mgmt_if_rx_q);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG("[ERROR] Failed to free mgmt_if_rx_q: 0x%lX\r\n", status);
    return status;
  }

  // Free cmd_parser_tx_q
  SL_DEBUG_LOG("[INFO] Freeing mgmt_if_tx_q...\r\n");
  status = SLI_MGMT_IF_PURGE_QUEUE(NHCP_PKT_TYPE_TX, mgmt_if_thread_ctx->mgmt_if_tx_q);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG("[ERROR] Failed to free mgmt_if_tx_q: 0x%lX\r\n", status);
    return status;
  }

  // Free the command parser context memory
  SL_DEBUG_LOG("[INFO] Freeing MGMT Interface context memory...\r\n");
  sl_free(mgmt_if_thread_ctx);
  mgmt_if_thread_ctx = NULL;

  SL_DEBUG_LOG("[INFO]MGMT interface thread deinitialized successfully.\r\n");
  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 * @brief Sends a TX packet to management interface.
 *
 * Queues a prepared packet for transmission and sets the TX pending
 * event flag. Caller should prepare the packet.
 *
 * @param[in] buffer Pointer to the Tx packet.
 *
 * @return SL_STATUS_OK on success.
 * @return SL_STATUS_NULL_POINTER if buffer is NULL.
 * @return SL_STATUS_FAIL if setting the event flag fails.
 ******************************************************************************/
sl_status_t sl_send_tx_pkt_to_mgmt_intf(uint8_t *pkt)
{
  // Check for NULL buffer
  if (pkt == NULL) {
    SL_DEBUG_LOG("\r\nFailed to handle TX: NULL buffer\r\n");
    return SL_STATUS_NULL_POINTER;
  }

  sl_wlan_mgmt_if_tx_pkt_t *tx_pkt = (sl_wlan_mgmt_if_tx_pkt_t *)pkt;

  sl_mgmt_if_tx_queue_t *mgmt_if_tx_q = mgmt_if_thread_ctx->mgmt_if_tx_q;
  uint32_t os_status                  = osOK;

  // Fill TX Head room and Metadata
  // tx_pkt->hostss_metadata.pkt_len = tx_pkt->host_desc.dword0.length + sizeof(host_desc_t);
  tx_pkt->scatter_count = 1;

  // Add to TX queue
  SLI_ADD_TO_LIST(mgmt_if_tx_q, tx_pkt);
  // Set event flag
  os_status = osEventFlagsSet(mgmt_if_thread_ctx->event_flags_id, SL_MGMT_IF_EVENT_HOST_TX_PENDING);
  if (os_status & osFlagsError) {
    SL_DEBUG_LOG("\r\nFailed to set event flag: 0x%lX\r\n", os_status);
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 * @brief Sends a RX packet to mgmt interface.
 *
 * Queues a prepared packet for transmission and sets the RX pending
 * event flag. Caller should prepare the packet.
 *
 * @param[in] rx_pkt Pointer to the Rx packet.
 *
 * @return SL_STATUS_OK on success.
 * @return SL_STATUS_NULL_POINTER if buffer is NULL.
 * @return SL_STATUS_FAIL if setting the event flag fails.
 ******************************************************************************/
sl_status_t sl_send_rx_pkt_to_mgmt_intf(uint8_t *rx_pkt)
{
  // Check for NULL buffer
  if (rx_pkt == NULL) {
    SL_DEBUG_LOG("\r\nFailed to handle RX: NULL buffer\r\n");
    return SL_STATUS_NULL_POINTER;
  }

  sl_mgmt_if_rx_queue_t *mgmt_if_rx_q = mgmt_if_thread_ctx->mgmt_if_rx_q;
  uint32_t os_status                  = osOK;

  // Add to RX queue
  SLI_ADD_TO_LIST(mgmt_if_rx_q, rx_pkt);
  // Set event flag
  os_status = osEventFlagsSet(mgmt_if_thread_ctx->event_flags_id, SL_MGMT_IF_EVENT_HOST_RX_PENDING);
  if (os_status & osFlagsError) {
    SL_DEBUG_LOG("\r\nFailed to set event flag: 0x%lX\r\n", os_status);
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}
