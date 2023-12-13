/*******************************************************************************
* @file  sl_platform.c
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
#include "sli_siwx917_soc.h"
#include "sl_device.h"
#include "sl_si91x_constants.h"
#include "sl_si91x_status.h"
#include "sl_si91x_core_utilities.h"
#include "sl_status.h"
#include "sl_constants.h"
#include <stdbool.h>
#include "sl_component_catalog.h"
#include "sl_board_configuration.h"

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "cmsis_os2.h"
#endif
sl_status_t sli_si91x_submit_rx_pkt(void);

void sli_si91x_platform_init(void)
{
  //  SystemCoreClockUpdate();

#ifdef SLI_SI91X_MCU_COMMON_FLASH_MODE
  /* Before TA going to power save mode ,set m4ss_ref_clk_mux_ctrl ,tass_ref_clk_mux_ctrl,
  AON domain power supply controls form TA to M4 */
  RSI_Set_Cntrls_To_M4();
#endif

  // Enable DWT and cycle counting
  CoreDebug->DEMCR |= 0x01000000;
  DWT->CTRL |= 0x1;

  SysTick_Config(SystemCoreClock / 1000);

  // Set P2P Intr priority
  NVIC_SetPriority(SysTick_IRQn, SYSTICK_INTR_PRI);
}

void sl_board_enable_vcom(void)
{
}

sl_status_t si91x_bootup_firmware(const uint8_t select_option)
{
  uint8_t skip_bootload_sequence = 0;
  si91x_status_t retval          = RSI_ERROR_NONE;

  if (!(P2P_STATUS_REG & TA_is_active)) {
    P2P_STATUS_REG |= M4_wakeup_TA;
    skip_bootload_sequence = 1;
  }
  while (!(P2P_STATUS_REG & TA_is_active)) {
  }

  if (!skip_bootload_sequence) {
    do {
      retval = rsi_waitfor_boardready();
      if (retval == RSI_ERROR_IN_OS_OPERATION) {
      }
      if ((retval < 0) && (retval != RSI_ERROR_WAITING_FOR_BOARD_READY) && (retval != RSI_ERROR_IN_OS_OPERATION)) {
        return convert_si91x_status_to_sl_status(retval);
      }
    } while ((retval == RSI_ERROR_WAITING_FOR_BOARD_READY) && (retval == RSI_ERROR_IN_OS_OPERATION));
    retval = rsi_select_option(select_option);
    VERIFY_STATUS_AND_RETURN(convert_si91x_status_to_sl_status(retval));
  }

  // Update TX & RX DMA descriptor address
  rsi_update_tx_dma_desc(skip_bootload_sequence);
  rsi_update_rx_dma_desc();

#if RSI_FAST_FW_UP
  status = rsi_set_fast_fw_up();
  if (status != RSI_SUCCESS) {
    SL_PRINTF(SL_DEVICE_INIT_SET_FAST_FIRMWARE_UP_ERROR, COMMON, LOG_ERROR, "status: %4x", status);
    return status;
  }
#endif

  sli_m4_ta_interrupt_init();
  if (!(M4SS_P2P_INTR_SET_REG & RX_BUFFER_VALID)) {
    sli_si91x_submit_rx_pkt();
  }

#if defined(SL_CATALOG_KERNEL_PRESENT)
  osKernelInitialize();
#endif

  return SL_STATUS_OK;
}
