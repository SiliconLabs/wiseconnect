/*******************************************************************************
 * @file  main.c
 * @brief
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
 * @brief : This file contains application code for demonstrating PSRAM Driver
 * usage. 
 * @section Description :
 * The example writes and reads data to and from PSRAM using auto mode, 
 * manual blocking mode and DMA mode.
============================================================================**/

//  ! INCLUDES

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "clock_update.h"
#include "rsi_debug.h"
#include "rsi_rom_clks.h"
#include "sl_log.h"
#include "sl_si91x_psram_handle.h"
#if defined(SL_SI91X_BOARD_INIT)
#include "rsi_board.h"
#endif // SL_SI91X_BOARD_INIT

/* Private macro -------------------------------------------------------------*/
#define READ_WRITE_LENGTH       256
#define BIT_8_READ_WRITE_LENGTH 32

/* Private variables ---------------------------------------------------------*/
uint8_t *psramBufWrtPtr = (uint8_t *)PSRAM_BASE_ADDRESS; /// Buffer at PSRAM base address

uint8_t verifyBuf[READ_WRITE_LENGTH]; /// Verification Buffer

/* Test Data -----------------------------------------------------------------*/
const uint8_t testBuf[READ_WRITE_LENGTH] = {
  0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x5a, 0x5b, 0x5c,
  0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
  0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62,
  0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65,
  0x66, 0x67, 0x68, 0x69, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
  0x69, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x5a, 0x5b,
  0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e,
  0x5f, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61,
  0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 0x64,
  0x65, 0x66, 0x67, 0x68, 0x69, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
  0x68, 0x69, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x5a,
  0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x5a, 0x5b, 0x5c, 0x5d,
  0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60,
  0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69
};

int main()
{
  sl_psram_return_type_t status = 0;

  uint8_t status_flag = 0;

  SystemCoreClockUpdate();

  /* Enable the DEBUG UART port for debug prints and Set up and initialize all required
    blocks and functions related to the board hardware. */
#if defined(SL_SI91X_BOARD_INIT)
  RSI_Board_Init();
#endif // SL_SI91X_BOARD_INIT

  sl_log_init_stage1();
  (void)sl_log_init_stage2();
  /* Initialize debug UART */
  SL_PRINT_STRING_ERROR("\r\nPSRAM driver example started\r\n");

  /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */

  status = sl_si91x_psram_uninit();
  status = sl_si91x_psram_init();
  UNUSED_PARAMETER(status);
  /* Write and read in Auto mode ---------------------------------------------*/

  /// Auto Write to PSRAM base address
  for (uint32_t index = 0; index < BIT_8_READ_WRITE_LENGTH; index++) {
    psramBufWrtPtr[index] = testBuf[index];
  }

  memset(verifyBuf, 0, READ_WRITE_LENGTH); /// Clearing the buffer

  /// Auto Read from PSRAM base address
  for (uint32_t index = 0; index < BIT_8_READ_WRITE_LENGTH; index++) {
    verifyBuf[index] = psramBufWrtPtr[index];
  }

  for (size_t i = 0; i < BIT_8_READ_WRITE_LENGTH; i++) {
    if (testBuf[i] != verifyBuf[i]) {
      status_flag = 1;
    }
  }

  if (status_flag) {
    SL_PRINT_STRING_ERROR("\r\nAuto Read Write Failed\r\n");
    status_flag = 0;
  } else {
    SL_PRINT_STRING_ERROR("\r\nAuto Write and Read successful from PSRAM\r\n");
  }

  /* Write and read in manual blocking mode ----------------------------------*/

  /// Manual write to PSRAM base address
  sl_si91x_psram_manual_write_in_blocking_mode(PSRAM_BASE_ADDRESS,
                                               (void *)testBuf,
                                               sizeof(uint8_t),
                                               BIT_8_READ_WRITE_LENGTH);

  memset(verifyBuf, 0, READ_WRITE_LENGTH); /// Clearing the buffer

  /// Manual read from PSRAM base address
  sl_si91x_psram_manual_read_in_blocking_mode(PSRAM_BASE_ADDRESS, verifyBuf, sizeof(uint8_t), BIT_8_READ_WRITE_LENGTH);

  for (size_t i = 0; i < BIT_8_READ_WRITE_LENGTH; i++) {
    if (testBuf[i] != verifyBuf[i]) {
      status_flag = 1;
    }
  }

  if (status_flag) {
    SL_PRINT_STRING_ERROR("\r\nManual Read Write Failed\r\n");
    status_flag = 0;
  } else {
    SL_PRINT_STRING_ERROR("\r\nManual Write and Read successful from PSRAM\r\n");
  }

  /* Write and read in manual dma mode ---------------------------------------*/

  volatile sl_psram_dma_status_type_t xferStatus = 0;

  /// Manual write in dma mode
  sl_si91x_psram_manual_write_in_dma_mode(PSRAM_BASE_ADDRESS,
                                          (void *)testBuf,
                                          sizeof(uint8_t),
                                          BIT_8_READ_WRITE_LENGTH,
                                          (sl_psram_dma_status_type_t *)&xferStatus);

  while (!xferStatus) {
    /// Wait till dma done
  }

  memset(verifyBuf, 0, READ_WRITE_LENGTH); /// Clearing the buffer

  /// Manual read in dma mode
  sl_si91x_psram_manual_read_in_dma_mode(PSRAM_BASE_ADDRESS,
                                         verifyBuf,
                                         sizeof(uint8_t),
                                         BIT_8_READ_WRITE_LENGTH,
                                         (sl_psram_dma_status_type_t *)&xferStatus);

  while (!xferStatus) {
    /// Wait till dma done
  }

  for (size_t i = 0; i < BIT_8_READ_WRITE_LENGTH; i++) {
    if (testBuf[i] != verifyBuf[i]) {
      status_flag = 1;
    }
  }

  if (status_flag) {
    SL_PRINT_STRING_ERROR("\r\nManual Read Write with DMA Failed\r\n");
    status_flag = 0;
  } else {
    SL_PRINT_STRING_ERROR("\r\nManual Write and Read with DMA successful from PSRAM\r\n");
  }

  SL_PRINT_STRING_ERROR("\r\n***************************************************"
                        "**********\r\n\n\n");

  while (1)
    ;
}
