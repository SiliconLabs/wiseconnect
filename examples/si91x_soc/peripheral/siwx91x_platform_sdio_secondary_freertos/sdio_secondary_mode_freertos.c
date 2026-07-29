/*******************************************************************************
 * @file  sdio_secondary_mode_freertos.c
 * @brief SDIO secondary FreeRTOS example
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
/**===========================================================================
 * @brief : This file contains application code for SDIO secondary device
 * @section Description :
 * This example demonstrates data transfer through SDIO. The device acts as a
 * secondary which interfaces with an external sdio host/master, running as a
 * dedicated FreeRTOS task using CMSIS-RTOS2 APIs.
============================================================================**/
#include "sdio_secondary_mode_freertos.h"
#include "UDMA.h"
#include "sl_si91x_sdio_secondary_drv_config.h"
#include "sl_si91x_peripheral_sdio_secondary.h"
#include "rsi_debug.h"
#include "rsi_rom_clks.h"
#include "cmsis_os2.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/
#define BLOCK_LEN        256
#define NO_OF_BLOCKS     4
#define XFER_BUFFER_SIZE (BLOCK_LEN * NO_OF_BLOCKS) // Buffer size is 256B*4 = 1KB

#define SW_CORE_CLK 1

#if SW_CORE_CLK
#define ICACHE2_ADDR_TRANSLATE_1_REG *(volatile uint32_t *)(0x20280000 + 0x24)
#ifndef MISC_CFG_SRAM_REDUNDANCY_CTRL
#define MISC_CFG_SRAM_REDUNDANCY_CTRL *(volatile uint32_t *)(M4_MISC_CONFIG_BASE + 0x18)
#endif // MISC_CFG_SRAM_REDUNDANCY_CTRL
#ifndef MISC_CONFIG_MISC_CTRL1
#define MISC_CONFIG_MISC_CTRL1 *(volatile uint32_t *)(M4_MISC_CONFIG_BASE + 0x44)
#endif // MISC_CONFIG_MISC_CTRL1
#define MISC_QUASI_SYNC_MODE  *(volatile uint32_t *)(M4_MISC_CONFIG_BASE + 0x84)
#define SOC_PLL_REF_FREQUENCY 40000000  // PLL input REFERENCE clock 40MHZ
#define PS4_SOC_FREQ          150000000 // PLL out clock 150MHz
#endif                                  // SW_CORE_CLK

/*******************************************************************************
 ******************************  Data Types  ***********************************
 ******************************************************************************/

typedef enum {
  SEND_DATA,
  RECEIVE_DATA,
  TRANSMISSION_COMPLETED,
} sdio_mode_enum_t;

// Change to SEND_DATA to send data to the host instead of receiving
static sdio_mode_enum_t current_mode = RECEIVE_DATA;

/*******************************************************************************
 ***************************   LOCAL VARIABLES   *******************************
 ******************************************************************************/

static uint8_t xfer_buffer[XFER_BUFFER_SIZE];

static osSemaphoreId_t host_intr_sem = NULL;
static osSemaphoreId_t dma_done_sem  = NULL;

static boolean_t send_data_flag    = true;
static boolean_t receive_data_flag = true;

static uint32_t tt_start     = 0;
static uint32_t packet_count = 0;

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void sdio_secondary_mode_task(void *argument);
static sl_status_t sdio_secondary_init_function(void);
static void application_callback(uint8_t events);
static void gpdma_callback(uint8_t dma_ch);

static const osThreadAttr_t sdio_secondary_mode_thread_attributes = {
  .name       = "sdio_task",
  .stack_size = 2048,
  .priority   = osPriorityLow1,
};

/*******************************************************************************
 ******************************   CALLBACKS   **********************************
 ******************************************************************************/

static void application_callback(uint8_t events)
{
  if (events & HOST_INTR_RECEIVE_EVENT) {
    if (host_intr_sem != NULL) {
      (void)osSemaphoreRelease(host_intr_sem);
    }
  }

  if (events & HOST_INTR_SEND_EVENT) {
    if (host_intr_sem != NULL) {
      (void)osSemaphoreRelease(host_intr_sem);
    }
  }

  if (events & HOST_INTR_CMD52_EVENT) {
    sl_si91x_sdio_secondary_set_interrupts(SL_SDIO_WR_INT_UNMSK | SL_SDIO_RD_INT_UNMSK | SL_SDIO_CMD52_INT_UNMSK);
  }
}

static void gpdma_callback(uint8_t dma_ch)
{
  UNUSED_PARAMETER(dma_ch);
  if (dma_done_sem != NULL) {
    (void)osSemaphoreRelease(dma_done_sem);
  }
}

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/*******************************************************************************
 * @brief  Entry point: creates the SDIO secondary FreeRTOS task.
 * @return None
 ******************************************************************************/
void sdio_secondary_mode_example_init(void)
{
  osThreadId_t thread_id =
    osThreadNew((osThreadFunc_t)sdio_secondary_mode_task, NULL, &sdio_secondary_mode_thread_attributes);
  if (thread_id == NULL) {
    /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */
    SL_PRINT_STRING_ERROR("Failed to create SDIO secondary thread\r\n");
    return;
  }
}

/*******************************************************************************
 * @brief  One-time SDIO hardware init: semaphores, SysTick, callback
 *         registration.
 * @return SL_STATUS_OK on success, error code on failure
 ******************************************************************************/
static sl_status_t sdio_secondary_init_function(void)
{
  sl_status_t status;

  host_intr_sem = osSemaphoreNew(1U, 0U, NULL);
  dma_done_sem  = osSemaphoreNew(1U, 0U, NULL);
  if (host_intr_sem == NULL || dma_done_sem == NULL) {
    SL_PRINT_STRING_ERROR("Failed to create SDIO semaphores\r\n");
    return SL_STATUS_ALLOCATION_FAILED;
  }

  status =
    sl_si91x_sdio_secondary_register_event_callback(application_callback,
                                                    SL_SDIO_WR_INT_EN | SL_SDIO_RD_INT_EN | SL_SDIO_CMD52_INT_EN);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("\r\nSDIO Secondary callback function registration failed\r\n");
    return status;
  }
  SL_PRINT_STRING_ERROR("\r\nSDIO Secondary callback function registration success\r\n");

  sl_si91x_sdio_secondary_gpdma_register_event_callback(gpdma_callback);

  return SL_STATUS_OK;
}

/*******************************************************************************
 * @brief  SDIO Secondary FreeRTOS task. Initialises SDIO hardware, then loops
 *         forever running the send/receive state machine, blocking on
 *         semaphores for ISR events.
 * @param  argument  Unused (NULL)
 * @return None
 ******************************************************************************/
static void sdio_secondary_mode_task(void *argument)
{
  (void)argument;

  sl_status_t status = sdio_secondary_init_function();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("SDIO Secondary init failed, exiting task\r\n");
    osThreadExit();
  }

  tt_start = osKernelGetTickCount();

  while (1) {
    uint32_t tt_end;
    uint32_t throughput;

    switch (current_mode) {
      case RECEIVE_DATA:
        if (receive_data_flag) {
          sl_si91x_sdio_secondary_receive(xfer_buffer);
          packet_count++;
          receive_data_flag = false;
        }

        if (osSemaphoreAcquire(host_intr_sem, osWaitForever) == osOK
            && osSemaphoreAcquire(dma_done_sem, osWaitForever) == osOK) {
          receive_data_flag = true;

          if ((osKernelGetTickCount() - tt_start) >= 2000) {
            tt_end = osKernelGetTickCount();
            SL_PRINT_STRING_ERROR("Data is received from host->Secondary successfully \n");
            SL_PRINT_STRING_ERROR("\r\nPackets received: %lu\r\n", (unsigned long)packet_count);
            SL_PRINT_STRING_ERROR("Total bits received: %lu \r\n",
                                  (unsigned long)(packet_count * XFER_BUFFER_SIZE * 8));
            SL_PRINT_STRING_ERROR("Time diff: %lu ms\r\n", (unsigned long)(tt_end - tt_start));

            throughput = (packet_count * XFER_BUFFER_SIZE * 8) / ((tt_end - tt_start) / 1000);
            SL_PRINT_STRING_ERROR("Throughput host->secondary = %ld bps \r\n", throughput);

            packet_count = 0;
            tt_start     = osKernelGetTickCount();
          }
          memset(xfer_buffer, 0, XFER_BUFFER_SIZE);
        }
        break;

      case SEND_DATA:
        if (send_data_flag) {
          for (int i = 0; i < XFER_BUFFER_SIZE; i++) {
            xfer_buffer[i] = (uint8_t)(i / 256) + 1;
          }
          sl_si91x_sdio_secondary_send(NO_OF_BLOCKS, xfer_buffer);
          packet_count++;
          send_data_flag = false;
        }

        if (osSemaphoreAcquire(dma_done_sem, osWaitForever) == osOK) {
          SL_PRINT_STRING_ERROR("Data is transferred from secondary to host successfully \n");
          send_data_flag = true;

          if ((osKernelGetTickCount() - tt_start) >= 2000) {
            tt_end = osKernelGetTickCount();

            SL_PRINT_STRING_ERROR("\r\nPackets sent: %lu\r\n", (unsigned long)packet_count);
            SL_PRINT_STRING_ERROR("Total bits sent: %lu \r\n", (unsigned long)(packet_count * XFER_BUFFER_SIZE * 8));
            SL_PRINT_STRING_ERROR("Time diff: %lu ms \r\n", (unsigned long)(tt_end - tt_start));

            throughput = (packet_count * XFER_BUFFER_SIZE * 8) / ((tt_end - tt_start) / 1000);
            SL_PRINT_STRING_ERROR("Throughput for secondary->host = %lu bps \r\n", (unsigned long)throughput);

            packet_count = 0;
            tt_start     = osKernelGetTickCount();
          }
        }
        break;

      case TRANSMISSION_COMPLETED:
        SL_PRINT_STRING_ERROR("SDIO Secondary transmission completed, exiting task\r\n");
        osThreadExit();
        break;

      default:
        break;
    }
  }
}
