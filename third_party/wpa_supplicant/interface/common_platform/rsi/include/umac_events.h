/*******************************************************************************
* @file  umac_events.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef _EVENTS_TH1_H
#define _EVENTS_TH1_H
#include "event.h"
#ifdef THREADX_ENABLE
#include "threadx_porting.h"
#endif
// TH0 internal events
#ifdef CMX_ENABLE
#define TX_PKT_PENDING           EVENT2
#define RX_PKT_FROM_LMAC_PENDING EVENT4
#define SUPPLICANT_LOOP          EVENT5
#define TIMER_FLAG_SET           EVENT6
#define DHCP_RENEWAL             EVENT7
#define RESTART_HTTP_SEND        EVENT8
#define PS_PRESLEEP_UMAC         EVENT9
#else
#if ((defined CHIP_9117B0) && (defined ONEBOXE) && (defined CHIP_9117))
#define SSL_BUFFER_FREE_EVENT (EVENT2)
#endif
#ifdef MQTT_OS_TIMER
#define MQTT_KEEP_ALIVE_EVENT (EVENT2)
#endif
#define RX_PKT_FROM_LMAC_PENDING (EVENT4)
#define TX_PKT_PENDING           (EVENT5)
#define SUPPLICANT_LOOP          (EVENT6)
#ifndef FW_TIMEOUT
#define TIMER_FLAG_SET (EVENT7)
#else
#define FWUP_TIMEOUT_EVENT (EVENT7)
#endif
#define DHCP_RENEWAL            (EVENT8)
#define UNMASK_EVENT            (EVENT9)
#define DHCP_TIMEOUT_EVENT      (EVENT10)
#define SSL_BUFFER_AVAILABLE    (EVENT11)
#define DHCPV6_TIMEOUT_EVENT    (EVENT12)
#define PS_PRESLEEP_UMAC        (EVENT13)
#define TRANS_MODE_EXIT_EVENT   (EVENT14)
#define WEBSOCKET_TIMEOUT_EVENT (EVENT15)
#define SNTP_TIMEOUT_EVENT      (EVENT16)
#ifdef MDNSD_ENABLE
#define MDNSD_EVENT (EVENT17)
#endif
#define RSI_SSL_TIMEOUT_EVENT (EVENT18)
#ifdef BT_STACK_ON_THREADX
#define TH3_BT_RX_EVENT            (EVENT19)
#define TH3_BT_HOST_TO_STACK_EVENT (EVENT0)
#ifdef COEX_DYNAMIC_MEMORY
#define FREE_COEX_DYNAMIC_BUFFERS_EVENT (EVENT20)
#endif
#ifdef RTLS_ENABLE
#define TAG_APPLICATION_EVENT   (EVENT21)
#define TAG_FSM_EVENT           (EVENT22)
#define LOWER_PRI_PRESLEEP_UMAC (EVENT23)
#define TH3_PARK_EVENT          (EVENT24)
#else
#define LOWER_PRI_PRESLEEP_UMAC (EVENT21)
#define TH3_PARK_EVENT          (EVENT22)
#endif

#else
#define LOWER_PRI_PRESLEEP_UMAC (EVENT19)
#define TH3_PARK_EVENT          (EVENT20)
#endif

#endif
#define THREAD_UMAC 3
#if 0
#define SET_EVENT_TH1(n)             \
  {                                  \
    _ALL_INTR_CH_DISABLE_TH1;        \
    th1_int_event_map |= (1 << (n)); \
    _ALL_INTR_CH_ENABLE_TH1;         \
  }
#define CLEAR_EVENT_TH1(n)            \
  {                                   \
    _ALL_INTR_CH_DISABLE_TH1;         \
    th1_int_event_map &= ~(1 << (n)); \
    _ALL_INTR_CH_ENABLE_TH1;          \
  }
#define MASK_EVENT_TH1(n)             \
  {                                   \
    _ALL_INTR_CH_DISABLE_TH1;         \
    th1_int_mask_map1 &= ~(1 << (n)); \
    _ALL_INTR_CH_ENABLE_TH1;          \
  }
#define UNMASK_EVENT_TH1(n)          \
  {                                  \
    _ALL_INTR_CH_DISABLE_TH1;        \
    th1_int_mask_map1 |= (1 << (n)); \
    _ALL_INTR_CH_ENABLE_TH1;         \
  }
#endif

#ifdef THREADX_ENABLE

#define SET_EVENT_TH1(n)    SET_MAIN_THREAD_EVENT(n)
#define CLEAR_EVENT_TH1(n)  CLEAR_MAIN_THREAD_EVENT(n)
#define MASK_EVENT_TH1(n)   MASK_MAIN_THREAD_EVENT(n)
#define UNMASK_EVENT_TH1(n) UNMASK_MAIN_THREAD_EVENT(n)
#else
#define SET_EVENT_TH1(n)    set_event_atomic(n, THREAD_UMAC)
#define CLEAR_EVENT_TH1(n)  clear_event_atomic(n, THREAD_UMAC)
#define MASK_EVENT_TH1(n)   mask_1_event_atomic(n, THREAD_UMAC)
#define UNMASK_EVENT_TH1(n) unmask_1_event_atomic(n, THREAD_UMAC)
#endif

/*****************function prototypes************************/
void umac_event0(struct common_info_s *);
void umac_event1(struct common_info_s *);
void umac_event2(struct common_info_s *);
void umac_event3(struct common_info_s *);
void umac_event4(struct common_info_s *);
void umac_event5(struct common_info_s *);
void umac_event6(struct common_info_s *);
void umac_event7(struct common_info_s *);
void umac_event8(struct common_info_s *);
void umac_event9(struct common_info_s *);
void umac_event10(struct common_info_s *);
void umac_event11(struct common_info_s *);
void umac_event12(struct common_info_s *);
void umac_event13(struct common_info_s *);
void umac_event14(struct common_info_s *);
void umac_event15(struct common_info_s *);
void umac_event16(struct common_info_s *);
#ifdef MDNSD_ENABLE
void umac_event17(struct common_info_s *);
#endif
void umac_event18(struct common_info_s *);
#ifdef BT_STACK_ON_THREADX
void umac_event19(struct common_info_s *);
void umac_event20(struct common_info_s *);
#ifdef RTLS_ENABLE
void umac_event21(struct common_info_s *);
void umac_event22(struct common_info_s *);
void umac_event23(struct common_info_s *);
#else
void umac_event21(struct common_info_s *);
#endif
#else
void umac_event19(struct common_info_s *);
#endif
void lmac2umac_pkt_pending_isr();
#endif
