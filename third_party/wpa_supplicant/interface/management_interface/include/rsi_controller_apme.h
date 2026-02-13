/*******************************************************************************
* @file  rsi_controller_apme.h
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

#ifndef RSI_CONTROLLER_APME_H_
#define RSI_CONTROLLER_APME_H_

#include "sl_pkt_desc_utils.h"

/* AP control block */
/* station info structure */
struct sta_info_s {
  uint8 *head;              /* queue head */
  uint8 *tail;              /* pending packets in station queue */
  uint8 mac[6];             /* Mac address */
  uint16 pending_pkt_count; /* pending packets in the queue*/
  uint16 state;             /* station state 0th bit - '1' station exist,
			                   1st bit - '1' station in power save
			                   2nd bit - '1' station queue reaches water mark level */
                            /* TODO: Water mark level need to be decided */
};

struct broadcast_queue_s {
  uint8 *head;              /* queue head */
  uint8 *tail;              /* pending packets in station queue */
  uint16 pending_pkt_count; /* pending packets in the queue*/
};

struct ap_cb_s {
  /* various bitmap variables */
  volatile uint8 sta_ps_bitmap;
  volatile uint8 sta_pending_pkt_bitmap;
  volatile uint8 sta_queue_mask_bitmap;
  volatile uint8 sta_virtual_bitmap;
  uint8 sta_keep_alive_virtual_bitmap;

  /* structure variables */
  struct sta_info_s sta_cb[16];     /* station queues */
  struct broadcast_queue_s bcast_q; /* broadcast queue */
  /* Keep alive */
  uint16 keep_alive_timeout;
};

#endif /* RSI_CONTROLLER_APME_H_ */
