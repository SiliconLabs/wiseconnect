/*******************************************************************************
* @file  app.c
* @brief BLE HCI raw (stack bypass) — SoC (Si91x) or NCP host (EFR32 + Si91x module)
*******************************************************************************
* # License
* <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_board_configuration.h"
#include "sl_constants.h"
#include "sl_wifi.h"
#include "sl_wifi_callback_framework.h"
#include "cmsis_os2.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "ble_config.h"
#include "rsi_ble.h"
#include "rsi_ble_apis.h"
#include "rsi_ble_common_config.h"
#include "rsi_bt_common.h"
#include "rsi_bt_common_apis.h"
#include "rsi_common_apis.h"
#include "sl_si91x_driver.h"

#ifdef SLI_SI91X_MCU_INTERFACE
#include "sl_si91x_usart.h"
#include "sl_si91x_hal_soc_soft_reset.h"
#include "USART.h"
#include "rsi_debug.h"
#else
#include "sl_iostream.h"
#include "sl_iostream_handles.h"
#include "em_device.h"
#endif

#define UART_READING_HCI_PKT_TYPE    0
#define UART_READING_HCI_OPCODE      1
#define UART_READING_HCI_LEN         2
#define UART_RECEIVING_ACTUAL_PACKET 3
#define USART0_INT_PRI               5
// !Event from UART and Firmware
#define RSI_APP_EVENT_UART 0
#define RSI_APP_EVENT_RCP  1

#define RSI_BLE_RCP_HCI_HDR_BEFORE_DATA 12U

#define BUFFER_SIZE 1024

#define BAUD_VALUE 115200

#define INTF_PLL_CLK ((uint32_t)(80000000)) // 80MHz default Interface PLL Clock as source to UART
#define SOC_PLL_CLK  ((uint32_t)(80000000)) // 80MHz default SoC PLL Clock as source to Processor

//static uint8_t uart_data_in[BUFFER_SIZE];

/*=======================================================================*/
//!    Powersave configurations
/*=======================================================================*/
#define ENABLE_NWP_POWER_SAVE 0 //! Set to 1 for powersave mode

#if ENABLE_NWP_POWER_SAVE
//! Power Save Profile Mode
#define PSP_MODE RSI_SLEEP_MODE_2
//! Power Save Profile type
#define PSP_TYPE RSI_MAX_PSP
sl_wifi_performance_profile_v2_t wifi_profile = { .profile = ASSOCIATED_POWER_SAVE_LOW_LATENCY };
#endif

#define MAX_UART_RX_QUEUE_SIZE 30
#define ADD_TO_LIST(list, new_desc) \
  {                                 \
    list->pkt_cnt++;                \
    if (list->tail == NULL) {       \
      list->head = new_desc;        \
    } else {                        \
      list->tail->next = new_desc;  \
    }                               \
    list->tail       = new_desc;    \
    list->tail->next = NULL;        \
  }

#define DEL_FROM_LIST(list)        \
  {                                \
    if (list->pkt_cnt == 0)        \
      while (1)                    \
        ;                          \
    list->pkt_cnt--;               \
    list->head = list->head->next; \
    if (list->head == NULL) {      \
      list->tail = NULL;           \
    }                              \
  }

/* uart */
typedef struct rx_uart_pkt_s {
  struct rx_uart_pkt_s *next;
  uint8_t tx_buf[400];
  uint8_t pkt_in_use;
  uint16_t cmd_len;
} rx_uart_pkt_t;

typedef struct rx_uart_queue_s {
  rx_uart_pkt_t *head;
  rx_uart_pkt_t *tail;
  uint32_t pkt_cnt;
} rx_uart_queue_t;

rx_uart_queue_t g_uart_rx_queue;
rx_uart_pkt_t g_uart_rx_pkt[MAX_UART_RX_QUEUE_SIZE];

/* uart */
#ifdef SLI_SI91X_MCU_INTERFACE
sl_usart_handle_t uart_handle;

extern ARM_DRIVER_USART Driver_USART0;
static ARM_DRIVER_USART *USARTdrv = &Driver_USART0;
ARM_USART_CAPABILITIES drv_capabilities;
#endif

sl_status_t status;

static uint32_t ble_app_event_map;
static uint32_t ble_app_event_map1;
#ifdef SLI_SI91X_MCU_INTERFACE
static uint8_t uart_rx_in_progress;
#endif
osSemaphoreId_t ble_main_task_sem;

#ifndef SLI_SI91X_MCU_INTERFACE
static osSemaphoreId_t hci_uart_rx_sem;
#endif

volatile uint32_t read_tx_cnt = 0;
volatile uint32_t read_rx_cnt = 0;

uint8_t rx_buffer[BUFFER_SIZE];

uint8_t uart_rx_state = UART_READING_HCI_PKT_TYPE;
uint8_t uart_tx_done  = 0;
uint8_t pkt_len       = 0;
uint16_t cmd_length   = 0;
uint8_t dummy_tx      = 0;
rsi_data_packet_t rsi_data_packet;

#ifdef SLI_SI91X_MCU_INTERFACE
void uart_callback_event(uint32_t event);
#endif

#if (defined(SLI_SI91X_MCU_INTERFACE) || defined(SLI_SI91X_NCP_INTERFACE))
/*
 * FreeRTOS idle hook: drains the logger ring buffer via sl_log_flush().
 * Active only for backends that emit the proprietary stream (IOStream
 * Compact over UART/VCOM and the proprietary UART backend); a no-op for
 * IOStream Compact over RTT, IOStream Formatted, SystemView, and Log None.
 */
void vApplicationIdleHook(void)
{
  sl_log_flush();
}
#endif

static const sl_wifi_device_configuration_t config = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .region_code = US,
  .boot_config = { .oper_mode = SL_SI91X_CLIENT_MODE,
                   .coex_mode = SL_SI91X_WLAN_BLE_MODE,
#ifdef SLI_SI91X_MCU_INTERFACE
                   .feature_bit_map = (SL_WIFI_FEAT_WPS_DISABLE | RSI_FEATURE_BIT_MAP),
#else
                   .feature_bit_map            = RSI_FEATURE_BIT_MAP,
#endif
#if RSI_TCP_IP_BYPASS
                   .tcp_ip_feature_bit_map = RSI_TCP_IP_FEATURE_BIT_MAP,
#else
                   .tcp_ip_feature_bit_map     = (RSI_TCP_IP_FEATURE_BIT_MAP | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID),
#endif
                   .custom_feature_bit_map = (SL_WIFI_SYSTEM_CUSTOM_FEAT_EXTENSION_VALID | RSI_CUSTOM_FEATURE_BIT_MAP),
                   .ext_custom_feature_bit_map =
                     (SL_WIFI_SYSTEM_EXT_FEAT_LOW_POWER_MODE | MEMORY_CONFIG | SL_SI91X_EXT_FEAT_XTAL_CLK
                      | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
                      | SL_SI91X_EXT_FEAT_BT_CUSTOM_FEAT_ENABLE),
                   .bt_feature_bit_map = (RSI_BT_FEATURE_BITMAP),
#ifdef RSI_PROCESS_MAX_RX_DATA
                   .ext_tcp_ip_feature_bit_map = (RSI_EXT_TCPIP_FEATURE_BITMAP | SL_SI91X_CONFIG_FEAT_EXTENTION_VALID
                                                  | SL_SI91X_EXT_TCP_MAX_RECV_LENGTH),
#else
                   .ext_tcp_ip_feature_bit_map = (RSI_EXT_TCPIP_FEATURE_BITMAP | SL_SI91X_CONFIG_FEAT_EXTENTION_VALID),
#endif
                   //!ENABLE_BLE_PROTOCOL in bt_feature_bit_map
                   .ble_feature_bit_map =
                     ((SL_SI91X_BLE_MAX_NBR_PERIPHERALS(RSI_BLE_MAX_NBR_PERIPHERALS)
                       | SL_SI91X_BLE_MAX_NBR_CENTRALS(RSI_BLE_MAX_NBR_CENTRALS)
                       | SL_SI91X_BLE_MAX_NBR_ATT_SERV(RSI_BLE_MAX_NBR_ATT_SERV)
                       | SL_SI91X_BLE_MAX_NBR_ATT_REC(RSI_BLE_MAX_NBR_ATT_REC))
                      | SL_SI91X_FEAT_BLE_CUSTOM_FEAT_EXTENTION_VALID | SL_SI91X_BLE_PWR_INX(RSI_BLE_PWR_INX)
                      | SL_SI91X_BLE_PWR_SAVE_OPTIONS(RSI_BLE_PWR_SAVE_OPTIONS)
                      | SL_SI91X_916_BLE_COMPATIBLE_FEAT_ENABLE
#if RSI_BLE_GATT_ASYNC_ENABLE
                      | SL_SI91X_BLE_GATT_ASYNC_ENABLE
#endif
                      ),

                   .ble_ext_feature_bit_map =
                     ((SL_SI91X_BLE_NUM_CONN_EVENTS(RSI_BLE_NUM_CONN_EVENTS)
                       | SL_SI91X_BLE_NUM_REC_BYTES(RSI_BLE_NUM_REC_BYTES))
#if RSI_BLE_INDICATE_CONFIRMATION_FROM_HOST
                      | SL_SI91X_BLE_INDICATE_CONFIRMATION_FROM_HOST //indication response from app
#endif
#if RSI_BLE_MTU_EXCHANGE_FROM_HOST
                      | SL_SI91X_BLE_MTU_EXCHANGE_FROM_HOST //MTU Exchange request initiation from app
#endif
#if RSI_BLE_SET_SCAN_RESP_DATA_FROM_HOST
                      | (SL_SI91X_BLE_SET_SCAN_RESP_DATA_FROM_HOST) //Set SCAN Resp Data from app
#endif
#if RSI_BLE_DISABLE_CODED_PHY_FROM_HOST
                      | (SL_SI91X_BLE_DISABLE_CODED_PHY_FROM_HOST) //Disable Coded PHY from app
#endif
#if BLE_SIMPLE_GATT
                      | SL_SI91X_BLE_GATT_INIT
#endif
#if RSI_BLE_ENABLE_ADV_EXTN
                      | SL_SI91X_BLE_ENABLE_ADV_EXTN
#endif
#if RSI_BLE_AE_MAX_ADV_SETS
                      | SL_SI91X_BLE_AE_MAX_ADV_SETS(RSI_BLE_AE_MAX_ADV_SETS)
#endif
                      | SL_SI91X_BT_BLE_STACK_BYPASS_ENABLE),
                   .config_feature_bit_map = (SL_SI91X_FEAT_SLEEP_GPIO_SEL_BITMAP | SL_WIFI_ENABLE_ENHANCED_MAX_PSP
                                              | RSI_CONFIG_FEATURE_BITMAP) },
  .ta_pool         = { .tx_ratio_in_buffer_pool = 0, .rx_ratio_in_buffer_pool = 0, .global_ratio_in_buffer_pool = 0 },
  .efuse_data_type = SL_SI91X_EFUSE_MFG_SW_VERSION,
  .nwp_fw_image_number = SL_SI91X_NWP_FW_IMAGE_NUMBER_0
};

const osThreadAttr_t thread_attributes = {
  .name       = "application_thread",
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = 3072,
  .priority   = 0,
  .tz_module  = 0,
};

#ifndef SLI_SI91X_MCU_INTERFACE
static const osThreadAttr_t hci_iostream_rx_thread_attributes = {
  .name       = "hci_iostream_rx",
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = 3072,
  .priority   = 0,
  .tz_module  = 0,
};

static void hci_iostream_rx_thread(void *argument);
#endif

#ifndef SLI_SI91X_MCU_INTERFACE
static sl_status_t iostream_rx(uint8_t *buf, size_t len)
{
  // Read HCI bytes ONLY from the vcom UART, not from the "default" / "recommended
  // console" stream. This decouples the HCI pipe from the log pipe so we can
  // route SL_DEBUG_LOG_V2 output to RTT without corrupting HCI on vcom.
  size_t total = 0;
  while (total < len) {
    size_t got         = 0;
    sl_status_t retval = sl_iostream_read(sl_iostream_vcom_handle, &buf[total], len - total, &got);
    if (retval == SL_STATUS_OK) {
      total += got;
    } else if (retval != SL_STATUS_EMPTY) {
      return retval;
    }
    // SL_STATUS_EMPTY or got == 0: spin until the EUSART has more bytes
  }
  return SL_STATUS_OK;
}

static sl_status_t iostream_tx(const uint8_t *buf, size_t len)
{
  // HCI replies must go back to the host over the vcom UART, NEVER to the
  // log/console stream (which app_init() redirects to RTT).
  return sl_iostream_write(sl_iostream_vcom_handle, buf, len);
}

static void iostream_usart_init(void)
{
#if !defined(__CROSSWORKS_ARM) && defined(__GNUC__)
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stdin, NULL, _IONBF, 0);
#endif
}
#endif

void rsi_ble_app_init_events()
{
  ble_app_event_map  = 0;
  ble_app_event_map1 = 0;
  return;
}

void rsi_ble_app_set_event(uint32_t event_num)
{
  if (event_num < 32) {
    ble_app_event_map |= BIT(event_num);
  } else {
    ble_app_event_map1 |= BIT((event_num - 32));
  }

  if (ble_main_task_sem) {
    osSemaphoreRelease(ble_main_task_sem);
  }
  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_clear_event
 * @brief      clears the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to clear the specific event.
 */
static void rsi_ble_app_clear_event(uint32_t event_num)
{
  if (event_num < 32) {
    ble_app_event_map &= ~BIT(event_num);
  }

  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_get_event
 * @brief      returns the first set event based on priority
 * @param[in]  none.
 * @return     int32_t
 *             > 0  = event number
 *             -1   = not received any event
 * @section description
 * This function returns the highest priority event among all the set events
 */
static int32_t rsi_ble_app_get_event(void)
{
  uint32_t ix;

  for (ix = 0; ix < 64; ix++) {
    if (ix < 32) {
      if (ble_app_event_map & (1 << ix)) {
        return ix;
      }
    } else {
      if (ble_app_event_map1 & (1 << (ix - 32))) {
        return ix;
      }
    }
  }

  return (-1);
}

void rsi_ble_on_rcp_resp_rcvd(uint16_t status, rsi_ble_event_rcp_rcvd_info_t *resp_buf)
{
  UNUSED_PARAMETER(status);
  uint8_t ii;
  rx_uart_queue_t *rx_queue = &g_uart_rx_queue;
  rx_uart_pkt_t *rx_pkt     = NULL;
  const uint8_t *rcp_hci    = (const uint8_t *)resp_buf - RSI_BLE_RCP_HCI_HDR_BEFORE_DATA;

  for (ii = 0; ii < MAX_UART_RX_QUEUE_SIZE; ii++) {
    rx_pkt = &g_uart_rx_pkt[ii];

    if (rx_pkt != NULL) {
      if (rx_pkt->pkt_in_use == 0) {
        rx_pkt->pkt_in_use = 1;
        rx_pkt->tx_buf[0]  = rcp_hci[0];
        switch (rx_pkt->tx_buf[0]) {
          case HCI_EVENT_PKT:
            rx_pkt->cmd_len = (uint16_t)resp_buf->data[1];
            rx_pkt->cmd_len += 2;
            break;
          case HCI_ACLDATA_PKT:
          case HCI_SCODATA_PKT:
            rx_pkt->cmd_len = *(uint16_t *)&resp_buf->data[2];
            rx_pkt->cmd_len += 4;
            break;
        }
        memcpy(&rx_pkt->tx_buf[1], resp_buf->data, rx_pkt->cmd_len);
        __disable_irq();

        ADD_TO_LIST(rx_queue, rx_pkt);
        __enable_irq();

        rsi_ble_app_set_event(RSI_APP_EVENT_RCP);
        return;
      }
    }
  }

  //Pkt dropped;
  return;
}

void get_pkt_length(void)
{
  if (rx_buffer[0] == HCI_COMMAND_PKT) {
    rx_buffer[3] = (uint8_t)cmd_length;
    pkt_len++;
  } else {
    *(uint16_t *)&rx_buffer[3] = cmd_length;
    pkt_len += 2;
  }
}

#ifndef SLI_SI91X_MCU_INTERFACE

static void hci_uart_rx_sem_give(void)
{
  if (hci_uart_rx_sem != NULL) {
    (void)osSemaphoreRelease(hci_uart_rx_sem);
  }
}

/* NCP: synchronous UART � advance all HCI framing states in one call (SoC uses ISR per step). */
static void read_user_packet_iostream_rx(void)
{
  for (;;) {
    switch (uart_rx_state) {
      case UART_READING_HCI_PKT_TYPE: {
        pkt_len++;
        status = iostream_rx(&rx_buffer[1], 2);
        if (status != SL_STATUS_OK) {
          SL_DEBUG_LOG_V2(ERROR, "iostream_rx: Error Code : %lu ", (unsigned long)status);
          hci_uart_rx_sem_give();
          return;
        }
        uart_rx_state = UART_READING_HCI_OPCODE;
      } break;
      case UART_READING_HCI_OPCODE: {
        pkt_len += 2;
        if (rx_buffer[0] == HCI_COMMAND_PKT) {
          status = iostream_rx((uint8_t *)&cmd_length, 1);
          if (status != SL_STATUS_OK) {
            SL_DEBUG_LOG_V2(ERROR, "iostream_rx: Error Code : %lu ", (unsigned long)status);
            hci_uart_rx_sem_give();
            return;
          }
          uart_rx_state = UART_READING_HCI_LEN;
        } else {
          status = iostream_rx((uint8_t *)&cmd_length, 2);
          if (status != SL_STATUS_OK) {
            SL_DEBUG_LOG_V2(ERROR, "iostream_rx: Error Code : %lu ", (unsigned long)status);
            hci_uart_rx_sem_give();
            return;
          }
          uart_rx_state = UART_READING_HCI_LEN;
        }
      } break;
      case UART_READING_HCI_LEN: {
        get_pkt_length();
        if (cmd_length == 0) {
          uart_rx_state = UART_READING_HCI_PKT_TYPE;
          uart_tx_done  = 1;
#if RSI_BT_RESET
          if (*(uint32_t *)rx_buffer == HCI_RESET_COMMAND) {
            (void)sl_wifi_deinit();
            NVIC_SystemReset();
          } else
#endif
          {
            rsi_ble_app_set_event(RSI_APP_EVENT_UART);
          }
          return;
        }
        status = iostream_rx(&rx_buffer[pkt_len], cmd_length);
        if (status != SL_STATUS_OK) {
          SL_DEBUG_LOG_V2(ERROR, "iostream_rx: Error Code : %lu ", (unsigned long)status);
          hci_uart_rx_sem_give();
          return;
        }
        uart_rx_state = UART_RECEIVING_ACTUAL_PACKET;
      } break;
      case UART_RECEIVING_ACTUAL_PACKET: {
        uart_rx_state = UART_READING_HCI_PKT_TYPE;
        uart_tx_done  = 1;
        rsi_ble_app_set_event(RSI_APP_EVENT_UART);
        return;
      }
      default:
        hci_uart_rx_sem_give();
        return;
    }
  }
}

/** NCP VCOM HCI RX thread: iostream_rx + read_user_packet_iostream_rx.
 *  Semaphore released by rsi_ble_hci_raw_task after each completed packet (uart_tx_done) so rx_buffer
 *  is not overwritten before rsi_data_tx_send. On framing error, thread releases the sem to retry. */
static void hci_iostream_rx_thread(void *argument)
{
  UNUSED_PARAMETER(argument);

  if (sl_iostream_vcom_handle != NULL) {
    (void)sl_iostream_set_default(sl_iostream_vcom_handle);
  }

  for (;;) {
    if (hci_uart_rx_sem != NULL) {
      (void)osSemaphoreAcquire(hci_uart_rx_sem, osWaitForever);
    }
    uart_rx_state = UART_READING_HCI_PKT_TYPE;
    pkt_len       = 0;
    status        = iostream_rx(&rx_buffer[0], 1);
    if (status != SL_STATUS_OK) {
      SL_DEBUG_LOG_V2(ERROR, "iostream_rx first byte: Error Code : %lu ", (unsigned long)status);
      hci_uart_rx_sem_give();
      continue;
    }
    read_user_packet_iostream_rx();
  }
}
#endif

#ifdef SLI_SI91X_MCU_INTERFACE
void read_user_packet(void)
{
  uint8_t *rx_ptr = rx_buffer;
  UNUSED_PARAMETER(rx_ptr);

  switch (uart_rx_state) {
    case UART_READING_HCI_PKT_TYPE: {
      pkt_len++;
      status = sl_si91x_usart_receive_data(uart_handle, &rx_buffer[1], 2);
      if (status != SL_STATUS_OK) {
        // If it fails to execute the API, it will not execute rest of the things
        SL_DEBUG_LOG_V2(DEBUG, "sl_si91x_uart_receive_data: Error Code : %lu ", status);
      }
      uart_rx_state = UART_READING_HCI_OPCODE;
    } break;
    case UART_READING_HCI_OPCODE: {
      pkt_len += 2;
      if (rx_buffer[0] == HCI_COMMAND_PKT) {
        status = sl_si91x_usart_receive_data(uart_handle, &cmd_length, 1);
        if (status != SL_STATUS_OK) {
          // If it fails to execute the API, it will not execute rest of the things
          SL_DEBUG_LOG_V2(DEBUG, "sl_si91x_uart_receive_data: Error Code : %lu ", status);
        }
        uart_rx_state = UART_READING_HCI_LEN;
      } else {
        status = sl_si91x_usart_receive_data(uart_handle, &cmd_length, 2);
        if (status != SL_STATUS_OK) {
          // If it fails to execute the API, it will not execute rest of the things
          SL_DEBUG_LOG_V2(DEBUG, "sl_si91x_uart_receive_data: Error Code : %lu ", status);
        }
        uart_rx_state = UART_READING_HCI_LEN;
      }
    } break;
    case UART_READING_HCI_LEN: {
      get_pkt_length();
      if (cmd_length == 0) {
        uart_rx_state = UART_READING_HCI_PKT_TYPE;
        uart_tx_done  = 1;
#if RSI_BT_RESET
        /* checking the HCI-RESET command */
        if (*(uint32_t *)rx_buffer == HCI_RESET_COMMAND) {
          sl_si91x_soc_nvic_reset();
        } else
#endif
        {
          rsi_ble_app_set_event(RSI_APP_EVENT_UART);
        }
      } else {
        status = sl_si91x_usart_receive_data(uart_handle, &rx_buffer[pkt_len], cmd_length);
        if (status != SL_STATUS_OK) {
          // If it fails to execute the API, it will not execute rest of the things
          SL_DEBUG_LOG_V2(DEBUG, "sl_si91x_uart_receive_data: Error Code : %lu ", status);
        }
        uart_rx_state = UART_RECEIVING_ACTUAL_PACKET;
      }
    } break;
    case UART_RECEIVING_ACTUAL_PACKET: {
      uart_rx_state = UART_READING_HCI_PKT_TYPE;
      uart_tx_done  = 1;
      rsi_ble_app_set_event(RSI_APP_EVENT_UART);
    } break;
  }
}

void uart_callback_event(uint32_t event)
{
  // Mask to get only the usart events,
  // bits 30-31 gives the instance from which this event occurred
  event &= (USART_EVENT_MASK);

  switch (event) {
    case SL_USART_EVENT_SEND_COMPLETE:
      read_tx_cnt         = USARTdrv->GetTxCount();
      uart_rx_in_progress = 0;
      if (!dummy_tx) {
        rx_uart_queue_t *rx_queue = &g_uart_rx_queue;
        rx_uart_pkt_t *rx_pkt     = rx_queue->head;
        if (rx_pkt != NULL) {
          rx_pkt->pkt_in_use = 0;
          DEL_FROM_LIST(rx_queue);
        }
        if (g_uart_rx_queue.pkt_cnt) {
          rsi_ble_app_set_event(RSI_APP_EVENT_RCP);
        }
      } else {
        dummy_tx = 0;
      }
      break;
    case SL_USART_EVENT_RECEIVE_COMPLETE:
      read_rx_cnt = USARTdrv->GetRxCount();
      read_user_packet();
      break;
    case SL_USART_EVENT_TRANSFER_COMPLETE:
      break;
    case SL_USART_EVENT_TX_COMPLETE:
      break;
    case SL_USART_EVENT_TX_UNDERFLOW:
      break;
    case SL_USART_EVENT_RX_OVERFLOW:
      break;
    case ARM_USART_EVENT_RX_TIMEOUT:
      break;
    case ARM_USART_EVENT_RX_BREAK:
      break;
    case ARM_USART_EVENT_RX_FRAMING_ERROR:
      break;
    case ARM_USART_EVENT_RX_PARITY_ERROR:
      break;
    case ARM_USART_EVENT_CTS:
      break;
    case ARM_USART_EVENT_DSR:
      break;
    case ARM_USART_EVENT_DCD:
      break;
    case ARM_USART_EVENT_RI:
      break;
  }
}
#endif

static void rsi_data_tx_send(void)
{
  memcpy(&rsi_data_packet.data[0], rx_buffer, (cmd_length + pkt_len));
  pkt_len = 0;
  rsi_bt_driver_send_cmd(RSI_BLE_REQ_HCI_RAW, &rsi_data_packet, NULL);
}

#ifdef SLI_SI91X_MCU_INTERFACE
/*******************************************************************************
 * USART Example Initialization function
 ******************************************************************************/
int32_t rsi_ble_app_init_uart(void)
{
  int32_t status = 0;
  sl_si91x_usart_control_config_t uart_config;
  sl_si91x_usart_control_config_t get_config;
  do {
    // Initialize the UART
    status = sl_si91x_usart_init(USART_0, &uart_handle);
    if (status != SL_STATUS_OK) {
      SL_DEBUG_LOG_V2(DEBUG, "sl_si91x_usart_initialize: Error Code : %lu \r\n", status);
      break;
    }
    SL_DEBUG_LOG_V2(DEBUG, "UART initialization is successful \r\n");
    // Configure the UART configurations
    status = sl_si91x_usart_set_configuration(uart_handle, &uart_config);
    if (status != SL_STATUS_OK) {
      SL_DEBUG_LOG_V2(DEBUG, "sl_si91x_usart_set_configuration: Error Code : %lu \r\n", status);
      break;
    }
    SL_DEBUG_LOG_V2(DEBUG, "UART configuration is successful \r\n");
    // Register user callback function
    status = sl_si91x_usart_multiple_instance_register_event_callback(USART_0, uart_callback_event);
    if (status != SL_STATUS_OK) {
      SL_DEBUG_LOG_V2(DEBUG, "sl_si91x_usart_register_event_callback: Error Code : %lu \r\n", status);
      break;
    }
    SL_DEBUG_LOG_V2(DEBUG, "UART user event callback registered successfully \r\n");
    sl_si91x_usart_get_configurations(USART_0, &get_config);
    SL_DEBUG_LOG_V2(DEBUG, "Baud Rate = %ld \r\n", get_config.baudrate);
  } while (false);
  return status;
}
#endif

uint8_t tx_buf_dummy[7] = { 0x04, 0x0E, 0x04, 0x01, 0x03, 0x0C, 0x00 };

void rsi_ble_hci_raw_task(void *argument)
{
  UNUSED_PARAMETER(argument);
  sl_status_t status     = 0;
  int32_t temp_event_map = 0;

#ifdef SLI_SI91X_MCU_INTERFACE
#if RSI_BT_RESET
  uint32_t reg_read = 0;
  /* checking the bit(5) in MCU_STORAGE_REG2 to know the WWD reset source */
  reg_read = *(volatile uint32_t *)(0x24048138);
  SL_DEBUG_LOG_V2(DEBUG, "Reset reason : 0x%ld ", reg_read);
  if (reg_read & BIT(5)) {
    //! If we are sending response immediately for hci reset command remote device not able to understand it. So adding the delay.
    status = sl_si91x_usart_send_data(uart_handle, tx_buf_dummy, (tx_buf_dummy[2] + 3));
    if (status != SL_STATUS_OK) {
      // If it fails to execute the API, it will not execute rest of the things
      SL_DEBUG_LOG_V2(DEBUG, "sl_si91x_uart_send_data: Error Code : %lu ", status);
    }
    status = sl_si91x_usart_receive_data(uart_handle, rx_buffer, sizeof(rx_buffer));
    if (status != SL_STATUS_OK) {
      // If it fails to execute the API, it will not execute rest of the things
      SL_DEBUG_LOG_V2(DEBUG, "sl_si91x_uart_receive_data: Error Code : %lu ", status);
    }
    /* clearing bit(5) in MCU_STORAGE_REG2 */
    *(volatile uint32_t *)(0x24048138) &= ~BIT(5);
    uart_rx_in_progress = 1;
    dummy_tx            = 1;
  }
#endif
#else
  iostream_usart_init();
#endif

  status = sl_wifi_init(&config, NULL, sl_wifi_default_event_handler);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Wi-Fi Initialization Failed, Error Code : 0x%lX\r\n", status);
    return;
  } else {
    SL_DEBUG_LOG_V2(INFO, "Wi-Fi Initialization Successful\r\n");
  }

#if RSI_SET_REGION_SUPPORT && !SL_SI91X_ACX_MODULE
  status = sl_si91x_set_device_region(0, 0, 4);
  if (status != RSI_SUCCESS) {
    SL_DEBUG_LOG_V2(ERROR, "Set Region Failed, Error Code : %ld\r\n", status);
    return;
  } else {
    SL_DEBUG_LOG_V2(INFO, "Set Region Success\r\n");
  }
#endif

  rsi_ble_enhanced_gap_extended_register_callbacks(RSI_BLE_ON_RCP_EVENT, (void *)rsi_ble_on_rcp_resp_rcvd);

  //! create ble main task if ble protocol is selected
  ble_main_task_sem = osSemaphoreNew(1, 0, NULL);

  //! initialize the event map
  rsi_ble_app_init_events();

#ifdef SLI_SI91X_MCU_INTERFACE
  // Receives data
  status = sl_si91x_usart_receive_data(uart_handle, &rx_buffer[0], 1);
  if (status != SL_STATUS_OK) {
    // If it fails to execute the API, it will not execute rest of the things
    return;
  }
#else
  hci_uart_rx_sem = osSemaphoreNew(1, 1, NULL);
  if (hci_uart_rx_sem == NULL) {
    SL_DEBUG_LOG_V2(ERROR, "hci_uart_rx_sem create failed");
    return;
  }
  (void)osThreadNew((osThreadFunc_t)hci_iostream_rx_thread, NULL, &hci_iostream_rx_thread_attributes);
#endif

#if ENABLE_NWP_POWER_SAVE
  SL_DEBUG_LOG_V2(INFO, "keep module in to power save ");

  //! initiating power save in BLE mode
  status = rsi_bt_power_save_profile(PSP_MODE, PSP_TYPE);
  if (status != RSI_SUCCESS) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to initiate BLE power save ");
    return;
  }

  //! initiating power save in BLE only mode, for coex mode, wifi power save is called in wifiapp.c
  status = sl_wifi_set_performance_profile_v2(&wifi_profile);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to initiate Wi-Fi power save  :%lx", status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Module is in power save ");
#endif

  while (1) {
    //! checking for received events
    temp_event_map = rsi_ble_app_get_event();
    if (temp_event_map == RSI_FAILURE) {
      //! if events are not received, loop will be continued
      if (ble_main_task_sem) {
        osSemaphoreAcquire(ble_main_task_sem, osWaitForever);
      }
      continue;
    }
    switch (temp_event_map) {
      case RSI_APP_EVENT_UART: {
        rsi_ble_app_clear_event(RSI_APP_EVENT_UART);
        rsi_data_tx_send();
      } break;
      case RSI_APP_EVENT_RCP: {
#ifdef SLI_SI91X_MCU_INTERFACE
        __disable_irq();
        rx_uart_queue_t *rx_queue = &g_uart_rx_queue;
        if ((rx_queue->pkt_cnt) && (uart_rx_in_progress == 0)) {
          rx_uart_pkt_t *rx_pkt = rx_queue->head;
          if (rx_pkt != NULL) {
            status = sl_si91x_usart_send_data(uart_handle, rx_pkt->tx_buf, (rx_pkt->cmd_len + 1));
            if (status != SL_STATUS_OK) {
              // If it fails to execute the API, it will not execute rest of the things
              SL_DEBUG_LOG_V2(DEBUG, "sl_si91x_uart_send_data: Error Code : %lu ", status);
            } else {
              SL_DEBUG_LOG_V2(DEBUG, "sl_si91x_uart_send_data success");
            }
            uart_rx_in_progress = 1;
          }
        }
        rsi_ble_app_clear_event(RSI_APP_EVENT_RCP);
        __enable_irq();
#else
        {

          rx_uart_queue_t *rx_queue = &g_uart_rx_queue;

          while ((rx_queue->pkt_cnt > 0)) {
            rx_uart_pkt_t *rx_pkt = rx_queue->head;
            if (rx_pkt == NULL) {
              break;
            }
            status = iostream_tx(rx_pkt->tx_buf, (size_t)(rx_pkt->cmd_len + 1));
            if (status != SL_STATUS_OK) {
              SL_DEBUG_LOG_V2(DEBUG, "iostream_tx: Error Code : %lu ", status);
              break;
            }
            rx_pkt->pkt_in_use = 0;
            DEL_FROM_LIST(rx_queue);
          }
          if (g_uart_rx_queue.pkt_cnt > 0) {
            rsi_ble_app_set_event(RSI_APP_EVENT_RCP);
          }

          rsi_ble_app_clear_event(RSI_APP_EVENT_RCP);
        }
#endif
      } break;
    }
    if (uart_tx_done == 1) {
      uart_tx_done = 0;
#ifdef SLI_SI91X_MCU_INTERFACE
      // Receives data
      status = sl_si91x_usart_receive_data(uart_handle, &rx_buffer[0], 1);
      if (status != SL_STATUS_OK) {
        return;
      }
#else
      hci_uart_rx_sem_give();
#endif
    }
  }
}

/***************************************************************************/ /**
 * Initialize application.
 ******************************************************************************/
void app_init(void)
{
#ifndef SLI_SI91X_MCU_INTERFACE
  // Force the log backend (log_backend_iostream_formatted) to write to RTT
  // instead of vcom. sl_iostream_set_console_instance() picks UART over RTT by
  // priority; without this override, SL_DEBUG_LOG_V2 output would land on vcom
  // and corrupt the HCI byte stream.
  extern sl_iostream_t *sl_iostream_recommended_console_stream;
  extern sl_iostream_t *sl_iostream_rtt_handle;
  sl_iostream_recommended_console_stream = sl_iostream_rtt_handle;
#endif
#ifdef SLI_SI91X_MCU_INTERFACE
  rsi_ble_app_init_uart();
#endif
  osThreadNew((osThreadFunc_t)rsi_ble_hci_raw_task, NULL, &thread_attributes);
}

/***************************************************************************/ /**
 * App ticking function.
 ******************************************************************************/
void app_process_action(void)
{
}
