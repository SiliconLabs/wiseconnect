/***************************************************************************/ /**
 * @file
 * @brief Rf_Test Example Application
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

#include <string.h>
#include "sl_status.h"
#include "cmsis_os2.h"
#include "sl_wifi.h"
#include "sl_net.h"
#include "socket.h"
#include "sl_ip_types.h"
#include "sl_utility.h"
#include "sl_net_si91x.h"
#include "errno.h"
#include "sl_net_wifi_types.h"
#include "sl_wifi_callback_framework.h"
#include "sl_si91x_driver.h"
#ifdef RSI_M4_INTERFACE
#include "rsi_rom_clks.h"
#endif

#define SL_TX_TEST_POWER   127
#define SL_TX_TEST_RATE    0x0
#define SL_TX_TEST_LENGTH  100
#define SL_TX_TEST_MODE    0
#define SL_TX_TEST_CHANNEL 1

#define RECEIVE_STATS           0
#define MAX_RECEIVE_STATS_COUNT 5
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

#if RECEIVE_STATS
static uint8_t stats_count = 0;
#endif
volatile sl_status_t callback_status = SL_STATUS_OK;

/******************************************************
 *               Function Declarations
 ******************************************************/

static void application_start(void *argument);
#if RECEIVE_STATS
static sl_status_t wifi_stats_receive_handler(sl_wifi_event_t event, void *result, uint32_t result_length, void *arg);
#endif

/******************************************************
 *               Function Definitions
 ******************************************************/

void app_init(const void *unused)
{
  UNUSED_PARAMETER(unused);
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

static void application_start(void *argument)
{
  UNUSED_PARAMETER(argument);
  sl_status_t status;

  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &sl_wifi_transmit_test_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    printf("Failed to start Wi-Fi client interface: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nWi-Fi Init Done \r\n");

#if RECEIVE_STATS
  // Register WLAN receive stats call back handler
  sl_wifi_set_stats_callback(wifi_stats_receive_handler, NULL);
#endif

  status = sl_wifi_set_antenna(SL_WIFI_CLIENT_2_4GHZ_INTERFACE, SL_WIFI_ANTENNA_EXTERNAL);
  if (status != SL_STATUS_OK) {
    printf("Failed to start set Antenna: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nAntenna Command Frame Success \r\n");

  status = sl_si91x_transmit_test_start(SL_TX_TEST_POWER,
                                        SL_TX_TEST_RATE,
                                        SL_TX_TEST_LENGTH,
                                        SL_TX_TEST_MODE,
                                        SL_TX_TEST_CHANNEL);
  if (status != SL_STATUS_OK) {
    printf("\r\nTransmit test start Failed, Error Code : 0x%lX\r\n", status);
    return;
  }
  printf("\r\nTransmit test start Success\r\n");

  // Add delay here to see the TX packets on AIR
  osDelay(1000);

  status = sl_si91x_transmit_test_stop();
  if (status != SL_STATUS_OK) {
    printf("\r\nTransmit test stop Failed, Error Code : 0x%lX\r\n", status);
    return;
  }
  printf("\r\nTransmit test stop Success\r\n");

#if RECEIVE_STATS
  ////////////////////////////////////////
  // Transmit data/TX from the peer//////
  ////////////////////////////////////////

  // Start/Receive publishing RX stats
  sl_wifi_channel_t channel = { 0 };
  channel.channel           = 1;
  status                    = sl_wifi_start_statistic_report(SL_WIFI_CLIENT_INTERFACE, channel);
  if (SL_STATUS_IN_PROGRESS == status) {
    callback_status = SL_STATUS_IN_PROGRESS;
    printf("\r\nReceive Statistics...\r\n");
    while (stats_count <= MAX_RECEIVE_STATS_COUNT) {
      osThreadYield();
      if (stats_count == MAX_RECEIVE_STATS_COUNT && callback_status != SL_STATUS_IN_PROGRESS) {
        printf("\r\n%s: Stop Statistics Report\n", __func__);
        sl_wifi_stop_statistic_report(SL_WIFI_CLIENT_INTERFACE);
        printf("\r\nStart Statistic Report Success\r\n");
        return;
      }
    }
    status = callback_status;
  }
  if (status != SL_STATUS_OK) {
    printf("\r\nStart Statistic Report Failed, Error Code : 0x%lX\r\n", status);
    return;
  }
#endif
}

#if RECEIVE_STATS
sl_status_t wifi_stats_receive_handler(sl_wifi_event_t event, void *reponse, uint32_t result_length, void *arg)
{
  UNUSED_PARAMETER(result_length);
  UNUSED_PARAMETER(arg);
  if (CHECK_IF_EVENT_FAILED(event)) {
    callback_status = *(sl_status_t *)reponse;
    return SL_STATUS_FAIL;
  }

  if (event == SL_WIFI_STATS_AYSNC_EVENT) {
    sl_si91x_async_stats_response_t *result = (sl_si91x_async_stats_response_t *)reponse;

    printf("\r\n%s: WIFI STATS Recieved packet# %d\n", __func__, stats_count);
    printf("stats : crc_pass %x, crc_fail %x, cal_rssi :%x\n", result->crc_pass, result->crc_fail, result->cal_rssi);
    stats_count++;
    callback_status = SL_STATUS_OK;
  }
  return SL_STATUS_OK;
}
#endif
