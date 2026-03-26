/*******************************************************************************
* @file  mgmt_platform_if.h
* @brief This file acts as interface between management interface and
*        platform interface
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

#ifndef MGMT_PLATFORM_IF_H
#define MGMT_PLATFORM_IF_H

#include "wlan_assert_def.h"
#include "sl_lib_utils_rtos.h"
#include "sl_pkt_utils.h"
#include "wlan_platform_utils.h"
#include "common_platform_utils.h"
#include "rsi_eap_reason_code.h"
#include "FreeRTOS.h"
#ifndef DISABLE_UMAC_SL_PRINTF
#include "sl_logging.h"
#else
#define SL_PRINTF(...)
#endif /* DISABLE_UMAC_SL_PRINTF */

/* Mgmt_pf_if_headers */
#include "rsi_error_macro_v29.h"
#if K60_PORTING
#include "k60_port.h"
#endif /* K60_PORTING */
#include "k60_port_in.h"

/****************************** Queue-Related Macros ******************************/

// Macro to get the packet count in a queue
#define SLI_MGMT_IF_GET_LIST_PKT_COUNT(queue, count) SLI_GET_LIST_PKT_COUNT(queue, count)

// Macro to dequeue a packet from the head of a queue
#define SLI_MGMT_IF_DEQUEUE_FROM_HEAD(queue, packet) SLI_DEQUEUE_FROM_HEAD(queue, packet)

// Macro to initialize the TX queue
#define SLI_MGMT_IF_INIT_TX_QUEUE(queue, queue_name) sli_init_tx_queue(queue, queue_name)

// Macro to initialize the RX queue
#define SLI_MGMT_IF_INIT_RX_QUEUE(queue, queue_name) sli_init_rx_queue(queue, queue_name)

// Macro to purge a queue based on packet type
#define SLI_MGMT_IF_PURGE_QUEUE(pkt_type, queue) sli_nhcp_purge_queue(pkt_type, queue)

#define SL_MGMT_ASSERT(condition)                                                                          \
  do {                                                                                                     \
    if (!(condition)) {                                                                                    \
      SL_DEBUG_LOG("Supplicant Assertion failed: %s, file %s, line %d\n", #condition, __FILE__, __LINE__); \
      while (1)                                                                                            \
        ;                                                                                                  \
    }                                                                                                      \
  } while (0)

#define SLI_WLAN_MGMT_ASSERT(x) SL_MGMT_ASSERT(0)

sl_status_t sli_set_event_supplicant_eloop(void);
sl_status_t sli_clear_event_supplicant_eloop(void);
sl_status_t sli_notify_eloop_thread_ready_to_terminate(void);
sl_status_t sli_clear_eloop_thread_terminate_event(void);
#endif /* MGMT_PLATFORM_IF_H */
