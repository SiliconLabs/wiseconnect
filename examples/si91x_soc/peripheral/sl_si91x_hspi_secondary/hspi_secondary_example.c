/*******************************************************************************
 * @file  hspi_secondary_example.c
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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
 * @brief : This file contains application code for hspi slave device
 * @section Description :
 * This example demonstrates data transfer through hspi. The device acts as a
 * slave which interfaces with an external spi host/master.
============================================================================**/
#include "hspi_secondary_example.h"
/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

/*******************************************************************************
 ******************************  Data Types  ***********************************
 ******************************************************************************/

/*******************************************************************************
 ***************************   LOCAL VARIABLES   *******************************
 ******************************************************************************/
uint8_t sl_hspi_rx_buffer[SL_HSPI_RX_BUFFER_SIZE];
uint8_t sl_hspi_tx_buffer[SL_HSPI_TX_BUFFER_SIZE];
volatile boolean_t sl_hspi_send_event    = false;
volatile boolean_t sl_hspi_receive_event = false;

#define TEST_MEMORY_READ_WRITES        0
#define SPI_SECONDARY_ADDRESS_LOCATION 0x30000
#define SPI_SECONDARY_CHECK_LOCATION   0x30500
#define SPI_SECONDARY_CHECK_BYTE       0xAA
volatile uint8_t *ptr   = (uint8_t *)SPI_SECONDARY_CHECK_LOCATION;
volatile uint8_t *check = (uint8_t *)SPI_SECONDARY_ADDRESS_LOCATION;

bool flag = false;
/*******************************************************************************
 ******************************   FUNCTIONS   **********************************
 ******************************************************************************/
// Function prototypes for application callbacks
void sl_hspi_secondary_callback(uint8_t event);
/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
/*******************************************************************************
 * HSPI Secondary initialization function
 ******************************************************************************/
void hspi_secondary_example_init(void)
{
  sl_status_t sl_hspi_status = 0;
  (void)sl_hspi_status;

  for (int i = 0; i < SL_HSPI_TX_BUFFER_SIZE; i++) {
    sl_hspi_tx_buffer[i] = (uint8_t)(i + 1);
  }

  sl_hspi_status = sl_si91x_hspi_secondary_init();
  if (sl_hspi_status != SL_STATUS_OK) {
    /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */
    SL_PRINT_STRING_ERROR("\rHSPI Secondary initialization failed ;Error Code : %lu \n", sl_hspi_status);
  }
  SL_PRINT_STRING_ERROR("\rHSPI Secondary initialization success\r\n");
  // Register user GPDMA callback function
  sl_hspi_status = sl_si91x_hspi_secondary_register_event_callback(sl_hspi_secondary_callback);
  if (sl_hspi_status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("\rHSPI Secondary callback function registration "
                          "failed ;Error Code : %lu \n",
                          sl_hspi_status);
  }
  SL_PRINT_STRING_ERROR("\rHSPI Secondary callback function registration success\r\n");

#if (SL_HSPI_DMA == ENABLE && TEST_MEMORY_READ_WRITES == 0)

  // HSPI Receive configuration when DMA is enabled
  sl_hspi_status = sl_si91x_hspi_secondary_receive_non_blocking(sl_hspi_rx_buffer);
  if (sl_hspi_status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("\rHSPI Secondary receive configuration "
                          "failed ;Error Code : %lu \n",
                          sl_hspi_status);
  }
  SL_PRINT_STRING_ERROR("\rHSPI Secondary receive configuration success\r\n");

  // HSPI Send configuration  when DMA is enabled
  sl_hspi_status = sl_si91x_hspi_secondary_send_non_blocking(sl_hspi_tx_buffer);
  if (sl_hspi_status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("\rHSPI Secondary send  configuration failed ;Error Code : %lu \n", sl_hspi_status);
  }
  SL_PRINT_STRING_ERROR("\rHSPI Secondary send  configuration success\r\n");
#endif
}

/*******************************************************************************
 * Function will run continuously in while loop
 ******************************************************************************/
void hspi_secondary_example_process_action(void)
{
#if TEST_MEMORY_READ_WRITES
  if ((*ptr == SPI_SECONDARY_CHECK_BYTE) && (flag == false)) {
    flag                    = true;
    uint32_t mismatch_count = 0;
    for (int i = 0; i < SL_HSPI_RX_BUFFER_SIZE; i++) {
      if (check[i] != (uint8_t)(i + 1U)) {
        mismatch_count++;
      }
    }
    if (mismatch_count == 0U) {
      SL_PRINT_STRING_ERROR("\rHSPI receive data completed \r\n");
    } else {
      SL_PRINT_STRING_ERROR("\rHSPI receive data failed \r\n");
    }
    sl_si91x_hspi_secondary_deinit();
  }
#else
  // Check if receive DMA is done
  if (sl_hspi_receive_event) {
    sl_hspi_receive_event = false;
#if (SL_HSPI_DMA == DISABLE)
    // read the data in blocking mode, loop's until read done
    sl_si91x_hspi_secondary_receive_blocking(sl_hspi_rx_buffer);
#endif
#if (SL_HSPI_DMA == ENABLE)
    // Reconfigure the receive DMA for next receive
    sl_si91x_hspi_secondary_receive_non_blocking(sl_hspi_rx_buffer);
#endif
    SL_PRINT_STRING_ERROR("\rHSPI receive data completed\r\n");
  }
  // Check if send DMA is done
  if (sl_hspi_send_event) {
    sl_hspi_send_event = false;
#if (SL_HSPI_DMA == DISABLE)
    // send the data in blocking mode, loop's until send  done
    sl_si91x_hspi_secondary_send_blocking(sl_hspi_tx_buffer);
#endif
#if (SL_HSPI_DMA == ENABLE)
    // Reconfigure the send DMA for next send
    sl_si91x_hspi_secondary_send_non_blocking(sl_hspi_tx_buffer);
#endif
    SL_PRINT_STRING_ERROR("\rHSPI send data completed\r\n");
  }
#endif
}

/***************************************************************************/ /**
 * @brief
 * GPDMA callback function
 * It is responsible for handling  transfer completion events.
 ******************************************************************************/
void sl_hspi_secondary_callback(uint8_t event)
{
  if (event == SL_HSPI_RECEIVE_EVENT) {
    sl_hspi_receive_event = true;
  }
  if (event == SL_HSPI_SEND_EVENT) {
    sl_hspi_send_event = true;
  }
}
