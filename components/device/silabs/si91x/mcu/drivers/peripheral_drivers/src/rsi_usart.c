/******************************************************************************
* @file  rsi_usart.c
*******************************************************************************
* # License
* <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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

/**
 * Includes
 */
#include "rsi_ccp_user_config.h"
#if !defined(A11_ROM) || !defined(USART_ROMDRIVER_PRESENT)
#include "rsi_rom_clks.h"
#include "rsi_rom_ulpss_clk.h"
#include "rsi_rom_egpio.h"
#include "USART.h"
#include "rsi_usart.h"
#ifndef SL_SI91X_USART_DMA
#include "rsi_rom_udma_wrapper.h"
#endif
#ifdef SL_SI91X_USART_DMA
#include "sl_si91x_dma.h"
sl_dma_init_t dma_init;
#define UART_DMA_INSTANCE     0
#define ULP_UART_DMA_INSTANCE 1
#endif

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/** @addtogroup SOC14
* @{
*/
/**
 * @fn           void UartIrqHandler (USART_RESOURCES *usart)
 * @brief        USART Interrupt handler.
 * @param[in]    usart     Pointer to USART resources
 * @return       none
 */
void UartIrqHandler(USART_RESOURCES *usart)
{
  volatile uint32_t int_status;
  volatile uint32_t line_status;
  volatile uint32_t modem_status;
  uint32_t event;
  uint32_t val;
  uint8_t usart_instance = 0U;

  int_status   = 0U;
  line_status  = 0U;
  modem_status = 0U;
  event        = 0U;
  // Check which uart instance irq got triggered and update the usart instance
  if ((usart->pREGS == UART0) || (usart->pREGS == USART0)) {
    usart_instance = USART_0;
  } else if (usart->pREGS == UART1) {
    usart_instance = UART_1;
  } else if (usart->pREGS == ULP_UART) {
    usart_instance = ULPUART;
  }

  // Set the usart instance in event flag
  event |= (usart_instance << USART_INSTANCE_BIT);

  int_status = usart->pREGS->IIR;

  if ((int_status & USART_RX_LINE_STATUS) == USART_RX_LINE_STATUS) {
    line_status = usart->pREGS->LSR;
    // OverRun error
    if (((line_status & USART_OVERRUN_ERR) == USART_OVERRUN_ERR) && (usart->info->cb_event)) {
      usart->info->rx_status.rx_overflow = 1U;
      usart->info->cb_event(ARM_USART_EVENT_RX_OVERFLOW);
      // Sync Slave mode: If Transmitter enabled, signal TX underflow
      if ((usart->info->mode == ARM_USART_MODE_SYNCHRONOUS_SLAVE) && (usart->info->xfer.send_active != 0U)) {
        event |= ARM_USART_EVENT_TX_UNDERFLOW;
      }
    }
    // Parity error
    if (((line_status & USART_PARITY_ERR) == USART_PARITY_ERR) && (usart->info->cb_event)) {
      usart->info->rx_status.rx_parity_error = 1U;
      usart->info->cb_event(ARM_USART_EVENT_RX_PARITY_ERROR);
    }
    // Framing error
    if ((line_status & USART_FRAMING_ERR) == USART_FRAMING_ERR) {
      if (usart->info->cb_event) {
        usart->info->rx_status.rx_framing_error = 1U;
        usart->info->cb_event(ARM_USART_EVENT_RX_FRAMING_ERROR);
      }
    }
    // Break detected
    else if (((line_status & USART_BREAK_ERR) == USART_BREAK_ERR) && (usart->info->cb_event)) {
      usart->info->rx_status.rx_break = 1U;
      usart->info->cb_event(ARM_USART_EVENT_RX_BREAK);
    }
  }
  if ((int_status & USART_THR_EMPTY) == USART_THR_EMPTY) {
    val = 16U;
    while ((val--) && (usart->info->xfer.tx_num != usart->info->xfer.tx_cnt)) {
      usart->pREGS->THR = usart->info->xfer.tx_buf[usart->info->xfer.tx_cnt];
      usart->info->xfer.tx_cnt++;
    }
    // Check if all data is transmitted
    if (usart->info->xfer.tx_num == usart->info->xfer.tx_cnt) {
      // Disable THRE interrupt
      usart->pREGS->IER &= (uint32_t)(~USART_INTR_THRE);

      // Clear TX busy flag
      usart->info->xfer.send_active = 0U;
      if (usart->info->cb_event != NULL) {
        event |= ARM_USART_EVENT_SEND_COMPLETE;
        usart->info->cb_event(event);
      }
    }
  }
  if (((int_status & USART_RX_DATA_AVAILABLE) == USART_RX_DATA_AVAILABLE) && (usart->info->cb_event)) {
    //check if receiver contains atleast one char in RBR reg
    if (usart->pREGS->LSR_b.DR) {
      usart->info->xfer.rx_buf[usart->info->xfer.rx_cnt] = (uint8_t)usart->pREGS->RBR;
      usart->info->xfer.rx_cnt++;

      // Check if requested amount of data is received
      if (usart->info->xfer.rx_cnt == usart->info->xfer.rx_num) {
        // Disable RDA)rx_data_available) interrupt
        usart->pREGS->IER &= (uint32_t)(~USART_INTR_RX_DATA);
        // Clear RX busy flag and set receive transfer complete event
        usart->info->rx_status.rx_busy = 0U;
        if ((usart->info->mode == ARM_USART_MODE_SYNCHRONOUS_MASTER)
            || (usart->info->mode == ARM_USART_MODE_SYNCHRONOUS_SLAVE)) {
          val                         = usart->info->xfer.sync_mode;
          usart->info->xfer.sync_mode = 0U;
          switch (val) {
            case USART_SYNC_MODE_TX:
              event |= ARM_USART_EVENT_SEND_COMPLETE;
              break;
            case USART_SYNC_MODE_RX:
              event |= ARM_USART_EVENT_RECEIVE_COMPLETE;
              break;
            case USART_SYNC_MODE_TX_RX:
              event |= ARM_USART_EVENT_TRANSFER_COMPLETE;
              break;
            default:
              break;
          }
        } else {
          event |= ARM_USART_EVENT_RECEIVE_COMPLETE;
        }
        usart->info->cb_event(event);
      }
    }
    //Check if requested amount of data is not received
    if ((usart->info->cb_event != NULL) && (usart->info->xfer.rx_cnt != usart->info->xfer.rx_num)) {
      // Update the event with rx tiemout
      event |= ARM_USART_EVENT_RX_TIMEOUT;
      usart->info->cb_event(event);
    }
  }
  if ((int_status & 0xF) == USART_MODEM_STATUS_INTR) {
    modem_status = usart->pREGS->MSR;

    // CTS state changed
    if ((usart->capabilities.cts) && (modem_status & USART_MSR_DCTS)) {
      event |= ARM_USART_EVENT_CTS;
      usart->info->cb_event(event);
    }
    // DSR state changed
    if ((usart->capabilities.dsr) && (modem_status & USART_MSR_DDSR)) {
      event |= ARM_USART_EVENT_DSR;
      usart->info->cb_event(event);
    }
    // Ring indicator
    if ((usart->capabilities.ri) && (modem_status & USART_MSR_TERI)) {
      event |= ARM_USART_EVENT_RI;
      usart->info->cb_event(event);
    }
    // DCD state changed
    if ((usart->capabilities.dcd) && (modem_status & USART_MSR_DDCD)) {
      event |= ARM_USART_EVENT_DCD;
      usart->info->cb_event(event);
    }
  }
}

///////////////////////////////////////////////////////////////
//                         Functions                         //
///////////////////////////////////////////////////////////////
/**
 * @fn          void USART_UDMA_Tx_Event(uint32_t event, uint8_t dmaCh, USART_RESOURCES *usart)
 * @brief        USART UDMA transfer event Handler
 * @param[in]    event       : UDMA Event
 * @param[in]    dmaCh       : UDMA channel
 * @param[in]    usart       : Pointer to the USART resources
 * @return       none
 */
void USART_UDMA_Tx_Event(uint32_t event, uint8_t dmaCh, USART_RESOURCES *usart)
{
  UNUSED_PARAMETER(dmaCh);
  uint8_t usart_instance = 0;
  uint32_t tx_event      = 0;

  switch (event) {
    case UDMA_EVENT_XFER_DONE:
      // Update TX buffer info
      usart->info->xfer.tx_cnt = usart->info->xfer.tx_num;
      // Clear TX busy flag
      usart->info->xfer.send_active = 0U;

      // Set the usart tx complete in event flag
      tx_event |= ARM_USART_EVENT_SEND_COMPLETE;

      break;
    case UDMA_EVENT_ERROR:
      break;
    default:
      // Handle unexpected events
      break;
  }

  // Check if event flag is updated and callback is registered
  if ((tx_event != 0U) && (usart->info->cb_event != NULL)) {
    // Check which uart instance irq got triggered and update the usart instance
    if ((usart->pREGS == UART0) || (usart->pREGS == USART0)) {
      usart_instance = USART_0;
    } else if (usart->pREGS == UART1) {
      usart_instance = UART_1;
    } else if (usart->pREGS == ULP_UART) {
      usart_instance = ULPUART;
    }

    // Set the usart instance in tx_event flag
    tx_event |= (usart_instance << USART_INSTANCE_BIT);

    // call the registered callback function
    usart->info->cb_event(tx_event);
  }
}
/**
 * @fn          void USART_UDMA_Rx_Event(uint32_t event, uint8_t dmaCh, USART_RESOURCES *usart)
 * @brief        USART UDMA transfer event Handler
 * @param[in]    event       : UDMA Event
 * @param[in]    dmaCh       : UDMA channel
 * @param[in]    usart       : Pointer to the USART resources
 * @return       none
 */
void USART_UDMA_Rx_Event(uint32_t event, uint8_t dmaCh, USART_RESOURCES *usart)
{
  UNUSED_PARAMETER(dmaCh);
  uint32_t val;
  uint8_t usart_instance = 0;
  uint32_t rx_event      = 0;

  switch (event) {
    case UDMA_EVENT_XFER_DONE:
      usart->info->xfer.rx_cnt       = usart->info->xfer.rx_num;
      usart->info->rx_status.rx_busy = 0U;
      if ((usart->info->mode == ARM_USART_MODE_SYNCHRONOUS_MASTER)
          || (usart->info->mode == ARM_USART_MODE_SYNCHRONOUS_SLAVE)) {
        val                         = usart->info->xfer.sync_mode;
        usart->info->xfer.sync_mode = 0U;
        switch (val) {
          case USART_SYNC_MODE_TX:
            rx_event |= ARM_USART_EVENT_SEND_COMPLETE;
            break;
          case USART_SYNC_MODE_RX:
            rx_event |= ARM_USART_EVENT_RECEIVE_COMPLETE;
            break;
          case USART_SYNC_MODE_TX_RX:
            rx_event |= ARM_USART_EVENT_TRANSFER_COMPLETE;
            break;
          default:
            break;
        }
      } else {
        rx_event |= ARM_USART_EVENT_RECEIVE_COMPLETE;
      }

      break;
    case UDMA_EVENT_ERROR:
      break;
    default:
      // Handle unexpected events
      break;
  }

  // Check if event flag is updated and callback is registered
  if ((rx_event != 0U) && (usart->info->cb_event != NULL)) {
    // Check which uart instance irq got triggered and update the usart instance
    if ((usart->pREGS == UART0) || (usart->pREGS == USART0)) {
      usart_instance = USART_0;
    } else if (usart->pREGS == UART1) {
      usart_instance = UART_1;
    } else if (usart->pREGS == ULP_UART) {
      usart_instance = ULPUART;
    }

    // Set the usart instance in rx_event flag
    rx_event |= (usart_instance << USART_INSTANCE_BIT);

    // call the registered callback function
    usart->info->cb_event(rx_event);
  }
}

/**
 * @fn           USART_SetBaudrate (uint32_t baudrate,uint32_t baseClk,USART_RESOURCES  *usart)
 * @brief        This API programs the baudrate for USART
 * @param[in]    baudrate  baudrate
 * @param[in]    baseClk   base clock for usart
 * @param[in]    usart     Pointer to USART resources
 * @return       Returns the \ref RSI_OK on success else UART error codes
 */
int32_t USART_SetBaudrate(uint32_t baudrate, uint32_t baseClk, USART_RESOURCES *usart)
{
  uint16_t baud_divisor = 0;
  uint16_t dlf;
  uint32_t baud_divisor_frac = 0;

  if ((usart == NULL) || (baudrate > 7372800)) {
    return ERROR_UART_INVALID_ARG;
  } else {
    // Get the baud divisor
    baud_divisor_frac = (baseClk * 4) / baudrate;
    dlf               = (baud_divisor_frac & 0x3F);
    baud_divisor      = (uint16_t)(baud_divisor_frac >> 6);

    // Reset before accessing the DLL and DLH register
    usart->pREGS->LCR = USART_CNTL_DLAB_SET;

    //Reset the registers
    usart->pREGS->DLF_b.DLF = USART_RESET_REG;
    usart->pREGS->DLL       = USART_RESET_REG;
    usart->pREGS->DLH       = USART_RESET_REG;

    // Reset after accessing the DLL and DLH register
    usart->pREGS->LCR = USART_CNTL_DLAB_RESET;

    // Reset before accessing the DLL and DLH register
    usart->pREGS->LCR = USART_CNTL_DLAB_SET;

    // Fill the DLL and DLH registers with bauddivisor
    usart->pREGS->DLL = (baud_divisor & 0x00FF);
    usart->pREGS->DLH = (baud_divisor >> 8);
    usart->pREGS->DLF = dlf;

    // Reset after accessing the DLL and DLH register
    usart->pREGS->LCR = USART_CNTL_DLAB_RESET;
  }
  usart->info->baudrate = baudrate;
  return RSI_OK;
}

/**
 * @fn          int32_t int32_t USART_Initialize( ARM_USART_SignalEvent_t cb_event,
 *                                                 USART_RESOURCES *usart,
 *                                                 const UDMA_RESOURCES *udma,
 *                                                 RSI_UDMA_DESC_T *UDMA_Table,
 *                                                 RSI_UDMA_HANDLE_T *udmaHandle,
 *                                                 uint32_t *mem )
 * @brief       Initialize USART Interface.
 * note: By default USART initialize, transfer, send and receive APIs use rsi_udma_wrapper.c
 * drivers. SL_DMA driver can be enabled by defining SL_SI91X_USART_DMA to 1.
 * rsi_udma_wrapper drivers can be executed from ROM. SL_DMA drivers cannot be executed
 * from ROM.
 * @param[in]   cb_event   Pointer to \ref ARM_USART_SignalEvent
 * @param[in]   usart      Pointer to USART resources
 * @param[in]   udma       Pointer to UDMA resources
 * @param[in]   UDMA_Table Pointer to DMA descriptors
 * @param[in]   udmaHandle Pointer to udma handle
 * @param[in]   mem        Pointer to dma buffer
 * @return      \ref execution_status
 */
int32_t USART_Initialize(ARM_USART_SignalEvent_t cb_event,
                         USART_RESOURCES *usart,
                         const UDMA_RESOURCES *udma,
                         RSI_UDMA_DESC_T *UDMA_Table,
                         RSI_UDMA_HANDLE_T *udmaHandle,
                         uint32_t *mem)
{
#ifdef SL_SI91X_USART_DMA
  //Added to suppress unused variable warning
  (void)udma;
  (void)UDMA_Table;
  (void)udmaHandle;
  (void)mem;
#endif
  if (usart->info->flags & USART_FLAG_INITIALIZED) {
    // Driver is already initialized
    return ARM_DRIVER_OK;
  }
  // Initialize USART Run-time Resources
  usart->info->cb_event                   = cb_event;
  usart->info->rx_status.rx_busy          = 0U;
  usart->info->rx_status.rx_overflow      = 0U;
  usart->info->rx_status.rx_break         = 0U;
  usart->info->rx_status.rx_framing_error = 0U;
  usart->info->rx_status.rx_parity_error  = 0U;
  usart->info->xfer.send_active           = 0U;
  usart->info->xfer.tx_def_val            = 0U;

  if ((usart->pREGS == UART0) || (usart->pREGS == USART0)) {
#if defined(SLI_SI917) || defined(SLI_SI915)
    RSI_PS_M4ssPeriPowerUp(M4SS_PWRGATE_ULP_EFUSE_PERI);
#else
    RSI_PS_M4ssPeriPowerUp(M4SS_PWRGATE_ULP_PERI1);
#endif
    RSI_CLK_UsartClkConfig(M4CLK,
                           ENABLE_STATIC_CLK,
                           (boolean_t)usart->clock.frac_div_en,
                           USART1,
                           usart->clock.uart_clock_src,
                           usart->clock.divfact);
  }
  if (usart->pREGS == UART1) {
#if defined(SLI_SI917) || defined(SLI_SI915)
    RSI_PS_M4ssPeriPowerUp(M4SS_PWRGATE_ULP_EFUSE_PERI);
#else
    RSI_PS_M4ssPeriPowerUp(M4SS_PWRGATE_ULP_PERI1);
#endif
    RSI_CLK_UsartClkConfig(M4CLK,
                           ENABLE_STATIC_CLK,
                           (boolean_t)usart->clock.frac_div_en,
                           USART2,
                           usart->clock.uart_clock_src,
                           usart->clock.divfact);
  }
  if (usart->pREGS == ULP_UART) {
    RSI_PS_UlpssPeriPowerUp(ULPSS_PWRGATE_ULP_UART);
    RSI_ULPSS_UlpUartClkConfig(ULPCLK,
                               ENABLE_STATIC_CLK,
                               (boolean_t)usart->clock.frac_div_en,
                               usart->clock.ulp_uart_source,
                               (uint16_t)usart->clock.divfact);
  }
  // DMA Initialize
  if (usart->dma_tx || usart->dma_rx) {
    if ((usart->pREGS == UART0) || (usart->pREGS == USART0) || (usart->pREGS == UART1) || (usart->pREGS == ULP_UART)) {
#ifdef SL_SI91X_USART_DMA
      if (usart->pREGS == ULP_UART) {
        dma_init.dma_number = ULP_UART_DMA_INSTANCE;
      } else {
        dma_init.dma_number = UART_DMA_INSTANCE;
      }
      if (sl_si91x_dma_init(&dma_init)) {
        return ARM_DRIVER_ERROR;
      }
#else
      *udmaHandle = UDMAx_Initialize(udma, UDMA_Table, udmaHandle, mem);
#endif
    } else {
      return ARM_DRIVER_ERROR;
    }
  }
  //initialise
  usart->info->flags = USART_FLAG_INITIALIZED;

  return ARM_DRIVER_OK;
}

/**
 * @fn          int32_t USART_Uninitialize (const USART_RESOURCES *usart, const UDMA_RESOURCES *udma)
 * @brief       De-initialize USART Interface.
 * @param[in]   usart     Pointer to USART resources
 * @param[in]   udma      Pointer to UDMA resources
 * @return      \ref execution_status
 */
int32_t USART_Uninitialize(const USART_RESOURCES *usart, const UDMA_RESOURCES *udma)
{
#ifdef SL_SI91X_USART_DMA
  //Added to suppress unused variable warning
  (void)udma;
#endif
  if ((usart->pREGS == UART0) || (usart->pREGS == USART0)) {
    RSI_CLK_PeripheralClkDisable(M4CLK, USART1_CLK);
  }
  if (usart->pREGS == UART1) {
    RSI_CLK_PeripheralClkDisable(M4CLK, USART2_CLK);
  }
  if (usart->pREGS == ULP_UART) {
    RSI_ULPSS_PeripheralDisable(ULPCLK, ULP_UART_CLK);
  }
  // DMA Uninitialize
  if (usart->dma_tx || usart->dma_rx) {
    if ((usart->pREGS == UART0) || (usart->pREGS == USART0) || (usart->pREGS == UART1) || (usart->pREGS == ULP_UART)) {
      //unintialise dma
#ifdef SL_SI91X_USART_DMA
      if (sl_si91x_dma_unregister_callbacks(dma_init.dma_number,
                                            (usart->dma_tx->channel + 1),
                                            SL_DMA_TRANSFER_DONE_CB | SL_DMA_ERROR_CB)) {
        return ARM_DRIVER_ERROR;
      }
      if (sl_si91x_dma_unregister_callbacks(dma_init.dma_number,
                                            (usart->dma_rx->channel + 1),
                                            SL_DMA_TRANSFER_DONE_CB | SL_DMA_ERROR_CB)) {
        return ARM_DRIVER_ERROR;
      }
#else
      //unintialise dma
      UDMAx_Uninitialize(udma);
#endif
    } else {
      return ARM_DRIVER_ERROR;
    }
  }
  // Reset USART status flags
  usart->info->flags = 0U;
  return ARM_DRIVER_OK;
}

/**
 * @fn          int32_t USART_PowerControl ( ARM_POWER_STATE  state, USART_RESOURCES *usart, const UDMA_RESOURCES *udma, RSI_UDMA_HANDLE_T udmaHandle )
 * @brief       Control USART Interface Power.
 * @param[in]   state      Power state
 * @param[in]   usart      Pointer to USART resources
 * @param[in]   udma       Pointer to UDMA resources
 * @param[in]   udmaHandle UDMA handle
 * @return      \ref execution_status
 */
int32_t USART_PowerControl(ARM_POWER_STATE state,
                           USART_RESOURCES *usart,
                           const UDMA_RESOURCES *udma,
                           RSI_UDMA_HANDLE_T udmaHandle)
{
#ifdef SL_SI91X_USART_DMA
  //Added to suppress unused variable warning
  (void)udma;
  (void)udmaHandle;
#endif
  switch (state) {
    case ARM_POWER_OFF:
      // Disable USART IRQ
      NVIC_DisableIRQ(usart->irq_num);

      // If DMA mode - disable TX DMA channel
      if ((usart->dma_tx) && (usart->info->xfer.send_active)) {
        if ((usart->pREGS == UART0) || (usart->pREGS == USART0) || (usart->pREGS == UART1)
            || (usart->pREGS == ULP_UART)) {
#ifdef SL_SI91X_USART_DMA
          if (sl_si91x_dma_channel_disable(dma_init.dma_number, usart->dma_tx->channel + 1)) {
            return ARM_DRIVER_ERROR;
          }
#else
          UDMAx_ChannelDisable(usart->dma_tx->channel, udma, udmaHandle);
#endif
        } else {
          return ARM_DRIVER_ERROR;
        }
      }
      // If DMA mode - disable DMA channel
      if ((usart->dma_rx) && (usart->info->rx_status.rx_busy)) {
        if ((usart->pREGS == UART0) || (usart->pREGS == USART0) || (usart->pREGS == UART1)
            || (usart->pREGS == ULP_UART)) {
#ifdef SL_SI91X_USART_DMA
          if (sl_si91x_dma_channel_disable(dma_init.dma_number, usart->dma_rx->channel + 1)) {
            return ARM_DRIVER_ERROR;
          }
#else
          UDMAx_ChannelDisable(usart->dma_rx->channel, udma, udmaHandle);
#endif
        } else {
          return ARM_DRIVER_ERROR;
        }
      }
      if ((usart->pREGS == UART0) || (usart->pREGS == USART0) || (usart->pREGS == UART1)) {
#if defined(SLI_SI917) || defined(SLI_SI915)
        RSI_PS_M4ssPeriPowerDown(M4SS_PWRGATE_ULP_EFUSE_PERI);
#else
        RSI_PS_M4ssPeriPowerDown(M4SS_PWRGATE_ULP_PERI1);
#endif
      } else {
        RSI_PS_UlpssPeriPowerDown(ULPSS_PWRGATE_ULP_UART);
      }
      // Clear pending USART interrupts in NVIC
      NVIC_ClearPendingIRQ(usart->irq_num);

      // Clear driver variables
      usart->info->rx_status.rx_busy          = 0U;
      usart->info->rx_status.rx_overflow      = 0U;
      usart->info->rx_status.rx_break         = 0U;
      usart->info->rx_status.rx_framing_error = 0U;
      usart->info->rx_status.rx_parity_error  = 0U;
      usart->info->xfer.send_active           = 0U;

      usart->info->flags &= (uint8_t)(~USART_FLAG_POWERED);
      break;

    case ARM_POWER_LOW:
      return ARM_DRIVER_ERROR_UNSUPPORTED;

    case ARM_POWER_FULL:
      if ((usart->info->flags & USART_FLAG_INITIALIZED) == 0U) {
        return ARM_DRIVER_ERROR;
      }
      if ((usart->info->flags & USART_FLAG_POWERED) != 0U) {
        return ARM_DRIVER_OK;
      }

      // Disable interrupts
      usart->pREGS->IER = 0U;

      // Clear driver variables
      usart->info->rx_status.rx_busy          = 0U;
      usart->info->rx_status.rx_overflow      = 0U;
      usart->info->rx_status.rx_break         = 0U;
      usart->info->rx_status.rx_framing_error = 0U;
      usart->info->rx_status.rx_parity_error  = 0U;
      usart->info->mode                       = 0U;
      usart->info->flags                      = 0U;
      usart->info->xfer.send_active           = 0U;

      usart->info->flags = USART_FLAG_POWERED | USART_FLAG_INITIALIZED;

      // Clear and Enable USART IRQ
      NVIC_ClearPendingIRQ(usart->irq_num);

      NVIC_EnableIRQ(usart->irq_num);
      break;
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  return ARM_DRIVER_OK;
}

/**
 * @fn          int32_t USART_Send_Data(const void *data, uint32_t num,USART_RESOURCES *usart, const UDMA_RESOURCES *udma, UDMA_Channel_Info *chnl_info, RSI_UDMA_HANDLE_T udmaHandle)
 * @brief       Start sending data to USART transmitter.
 * @param[in]   data  Pointer to buffer with data to send to USART transmitter
 * @param[in]   num   Number of data items to send
 * @param[in]   usart      Pointer to USART resources
 * @param[in]   udma       Pointer to UDMA resources
 * @param[in]   chnl_info  Pointer to UDMA channel info
 * @param[in]   udmaHandle UDMA Handle
 * @return      \ref  execution_status
 */
int32_t USART_Send_Data(const void *data,
                        uint32_t num,
                        USART_RESOURCES *usart,
                        const UDMA_RESOURCES *udma,
                        UDMA_Channel_Info *chnl_info,
                        RSI_UDMA_HANDLE_T udmaHandle)
{
#ifdef SL_SI91X_USART_DMA
  //Added to suppress unused variable warning
  (void)udma;
  (void)chnl_info;
  (void)udmaHandle;
  sl_status_t status;
#else
  int32_t stat;
#endif
  uint32_t val = 0;

  RSI_UDMA_CHA_CFG_T chnl_cfg = { 0 };

  if (data == NULL || num < 1) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }
  if ((usart->info->flags & USART_FLAG_CONFIGURED) == 0U) {
    // USART is not configured (mode not selected)
    return ARM_DRIVER_ERROR;
  }

  if (usart->info->xfer.send_active != 0U) {
    // Send is not completed yet
    return ARM_DRIVER_ERROR_BUSY;
  }

  // Set Send active flag
  usart->info->xfer.send_active = 1U;
  // Save transmit buffer info
  usart->info->xfer.tx_buf = (uint8_t *)data;
  usart->info->xfer.tx_num = num;
  usart->info->xfer.tx_cnt = 0U;
  if (usart->dma_tx) {
    //DMA
    chnl_cfg.altStruct = 0;
    chnl_cfg.burstReq  = 1;
    if ((usart->pREGS == UART0) || (usart->pREGS == USART0)) {
      chnl_cfg.channelPrioHigh = UDMA0_CHNL_PRIO_LVL;
      chnl_cfg.dmaCh           = RTE_USART0_CHNL_UDMA_TX_CH;
      chnl_cfg.periAck         = USART0_ACK;
      chnl_cfg.periphReq       = 1;
      chnl_cfg.reqMask         = 0;
    }
    if (usart->pREGS == UART1) {
      chnl_cfg.channelPrioHigh = UDMA0_CHNL_PRIO_LVL;
      chnl_cfg.dmaCh           = RTE_UART1_CHNL_UDMA_TX_CH;
      chnl_cfg.periAck         = UART1_ACK;
      chnl_cfg.periphReq       = 1;
      chnl_cfg.reqMask         = 0;
    }
    if (usart->pREGS == ULP_UART) {
      chnl_cfg.channelPrioHigh = UDMA1_CHNL_PRIO_LVL;
      chnl_cfg.dmaCh           = RTE_ULPUART_CHNL_UDMA_TX_CH;
      chnl_cfg.periAck         = UART3_ACK;
      chnl_cfg.periphReq       = 1;
      chnl_cfg.reqMask         = 0;
    }

    // Configure DMA channel
    if ((usart->pREGS == UART0) || (usart->pREGS == USART0) || (usart->pREGS == UART1) || (usart->pREGS == ULP_UART)) {
#ifdef SL_SI91X_USART_DMA
      sl_dma_xfer_t dma_transfer_tx = { 0 };
      uint32_t channel              = usart->dma_tx->channel + 1;
      uint32_t channel_priority     = chnl_cfg.channelPrioHigh;
      sl_dma_callback_t usart_tx_callback;
      //Initialize sl_dma callback structure
      usart_tx_callback.transfer_complete_cb = usart_transfer_complete_callback;
      usart_tx_callback.error_cb             = usart_error_callback;
      //Initialize sl_dma transfer structure
      dma_transfer_tx.src_addr       = (uint32_t *)((uint32_t)(usart->info->xfer.tx_buf));
      dma_transfer_tx.dest_addr      = (uint32_t *)((uint32_t) & (usart->pREGS->THR));
      dma_transfer_tx.src_inc        = SRC_INC_8;
      dma_transfer_tx.dst_inc        = DST_INC_NONE;
      dma_transfer_tx.xfer_size      = SRC_SIZE_8;
      dma_transfer_tx.transfer_count = num;
      dma_transfer_tx.transfer_type  = SL_DMA_MEMORY_TO_PERIPHERAL;
      dma_transfer_tx.dma_mode       = UDMA_MODE_BASIC;
      dma_transfer_tx.signal         = (uint8_t)chnl_cfg.periAck;

      //Allocate DMA channel for Tx
      status = sl_si91x_dma_allocate_channel(dma_init.dma_number, &channel, channel_priority);
      if (status && (status != SL_STATUS_DMA_CHANNEL_ALLOCATED)) {
        return ARM_DRIVER_ERROR;
      }
      //Register transfer complete and error callback
      if (sl_si91x_dma_register_callbacks(dma_init.dma_number, channel, &usart_tx_callback)) {
        return ARM_DRIVER_ERROR;
      }
      //Configure the channel for DMA transfer
      if (sl_si91x_dma_transfer(dma_init.dma_number, channel, &dma_transfer_tx)) {
        return ARM_DRIVER_ERROR;
      }
#else
      stat = UDMAx_ChannelConfigure(udma,
                                    usart->dma_tx->channel,
                                    (uint32_t)(usart->info->xfer.tx_buf),
                                    (uint32_t) & (usart->pREGS->THR),
                                    num,
                                    usart->dma_tx->control,
                                    &chnl_cfg,
                                    usart->dma_tx->cb_event,
                                    chnl_info,
                                    udmaHandle);
      if (stat == -1) {
        return ARM_DRIVER_ERROR;
      }
#endif
#ifdef SL_SI91X_USART_DMA
      sl_si91x_dma_channel_enable(dma_init.dma_number, usart->dma_tx->channel + 1);
      sl_si91x_dma_enable(dma_init.dma_number);
#else
      UDMAx_ChannelEnable(usart->dma_tx->channel, udma, udmaHandle);
      UDMAx_DMAEnable(udma, udmaHandle);
#endif
    } else {
      return ARM_DRIVER_ERROR;
    }
  } else {
    // Fill TX FIFO
    if (usart->pREGS->LSR & USART_LSR_THRE) {
      val = 16U;
      while ((val--) && (usart->info->xfer.tx_cnt != usart->info->xfer.tx_num)) {
        usart->pREGS->THR = usart->info->xfer.tx_buf[usart->info->xfer.tx_cnt++];
      }
    }
    // Enable transmit holding register empty interrupt
    usart->pREGS->IER |= USART_INTR_THRE;
  }
  return ARM_DRIVER_OK;
}

/**
 * @fn           int32_t USART_Receive_Data( const void *data,uint32_t num,USART_RESOURCES *usart, const UDMA_RESOURCES *udma, UDMA_Channel_Info *chnl_info, RSI_UDMA_HANDLE_T udmaHandle)
 * @brief       Start receiving data from USART receiver.
 * @param[out]  data       Pointer to buffer for data to receive from USART receiver
 * @param[in]   num        Number of data items to receive
 * @param[in]   usart      Pointer to USART resources
 * @param[in]   udma       Pointer to UDMA resources
 * @param[in]   chnl_info  Pointer to UDMA channel info
 * @param[in]   udmaHandle UDMA Hanlde
 * @return      \ref execution_status
 */
int32_t USART_Receive_Data(const void *data,
                           uint32_t num,
                           USART_RESOURCES *usart,
                           const UDMA_RESOURCES *udma,
                           UDMA_Channel_Info *chnl_info,
                           RSI_UDMA_HANDLE_T udmaHandle)
{
#ifdef SL_SI91X_USART_DMA
  //Added to suppress unused variable warning
  (void)udma;
  (void)chnl_info;
  (void)udmaHandle;
  sl_status_t status;
#else
  int32_t stat;
#endif

  RSI_UDMA_CHA_CFG_T chnl_cfg = { 0 };

  if (data == NULL || num < 1) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((usart->info->flags & USART_FLAG_CONFIGURED) == 0U) {
    // USART is not configured (mode not selected)
    return ARM_DRIVER_ERROR;
  }
  // Check if receiver is busy
  if (usart->info->rx_status.rx_busy == 1U) {
    return ARM_DRIVER_ERROR_BUSY;
  }

  // Set RX busy flag
  usart->info->rx_status.rx_busy = 1U;

  // Save number of data to be received
  usart->info->xfer.rx_num = num;

  // Clear RX statuses
  usart->info->rx_status.rx_break         = 0U;
  usart->info->rx_status.rx_framing_error = 0U;
  usart->info->rx_status.rx_overflow      = 0U;
  usart->info->rx_status.rx_parity_error  = 0U;

  // Save receive buffer info
  usart->info->xfer.rx_buf = (uint8_t *)(const void *)data;
  usart->info->xfer.rx_cnt = 0U;

  // DMA mode
  if (usart->dma_rx) {
    chnl_cfg.altStruct = 0;
    chnl_cfg.burstReq  = 1;

    if ((usart->pREGS == UART0) || (usart->pREGS == USART0)) {
      chnl_cfg.channelPrioHigh = UDMA0_CHNL_PRIO_LVL;
      chnl_cfg.dmaCh           = RTE_USART0_CHNL_UDMA_RX_CH;
      chnl_cfg.periAck         = USART0_ACK;
      chnl_cfg.periphReq       = 1;
      chnl_cfg.reqMask         = 0;
    }
    if (usart->pREGS == UART1) {
      chnl_cfg.channelPrioHigh = UDMA0_CHNL_PRIO_LVL;
      chnl_cfg.dmaCh           = RTE_UART1_CHNL_UDMA_RX_CH;
      chnl_cfg.periAck         = UART1_ACK;
      chnl_cfg.periphReq       = 1;
      chnl_cfg.reqMask         = 0;
    }
    if (usart->pREGS == ULP_UART) {
      chnl_cfg.channelPrioHigh = UDMA1_CHNL_PRIO_LVL;
      chnl_cfg.dmaCh           = RTE_ULPUART_CHNL_UDMA_RX_CH;
      chnl_cfg.periAck         = UART3_ACK;
      chnl_cfg.periphReq       = 1;
      chnl_cfg.reqMask         = 0;
    }
    // Configure DMA channel
    if ((usart->pREGS == UART0) || (usart->pREGS == USART0) || (usart->pREGS == UART1) || (usart->pREGS == ULP_UART)) {
#ifdef SL_SI91X_USART_DMA
      sl_dma_xfer_t dma_transfer_rx = { 0 };
      uint32_t channel              = usart->dma_rx->channel + 1;
      uint32_t channel_priority     = chnl_cfg.channelPrioHigh;
      sl_dma_callback_t usart_rx_callback;
      //Initialize sl_dma callback structure
      usart_rx_callback.transfer_complete_cb = usart_transfer_complete_callback;
      usart_rx_callback.error_cb             = usart_error_callback;
      //Initialize sl_dma transfer structure
      dma_transfer_rx.src_addr       = (uint32_t *)((uint32_t) & (usart->pREGS->RBR));
      dma_transfer_rx.dest_addr      = (uint32_t *)((uint32_t)(usart->info->xfer.rx_buf));
      dma_transfer_rx.src_inc        = SRC_INC_NONE;
      dma_transfer_rx.dst_inc        = DST_INC_8;
      dma_transfer_rx.xfer_size      = SRC_SIZE_8;
      dma_transfer_rx.transfer_count = num;
      dma_transfer_rx.transfer_type  = SL_DMA_PERIPHERAL_TO_MEMORY;
      dma_transfer_rx.dma_mode       = UDMA_MODE_BASIC;
      dma_transfer_rx.signal         = (uint8_t)chnl_cfg.periAck;

      //Allocate DMA channel for Rx
      status = sl_si91x_dma_allocate_channel(dma_init.dma_number, &channel, channel_priority);
      if (status && (status != SL_STATUS_DMA_CHANNEL_ALLOCATED)) {
        return ARM_DRIVER_ERROR;
      }
      //Register transfer complete and error callback
      if (sl_si91x_dma_register_callbacks(dma_init.dma_number, channel, &usart_rx_callback)) {
        return ARM_DRIVER_ERROR;
      }
      //Configure the channel for DMA transfer
      if (sl_si91x_dma_transfer(dma_init.dma_number, channel, &dma_transfer_rx)) {
        return ARM_DRIVER_ERROR;
      }
#else
      stat = UDMAx_ChannelConfigure(udma,
                                    usart->dma_rx->channel,
                                    (uint32_t) & (usart->pREGS->RBR),
                                    (uint32_t)(usart->info->xfer.rx_buf),
                                    num,
                                    usart->dma_rx->control,
                                    &chnl_cfg,
                                    usart->dma_rx->cb_event,
                                    chnl_info,
                                    udmaHandle);
      if (stat == -1) {
        return ARM_DRIVER_ERROR;
      }
#endif
#ifdef SL_SI91X_USART_DMA
      sl_si91x_dma_channel_enable(dma_init.dma_number, usart->dma_rx->channel + 1);
      if (usart->sync_mode.en_usart_mode == 0) {
        sl_si91x_dma_enable(dma_init.dma_number);
      }
#else
      UDMAx_ChannelEnable(usart->dma_rx->channel, udma, udmaHandle);
      if (usart->sync_mode.en_usart_mode == 0) {
        UDMAx_DMAEnable(udma, udmaHandle);
      }
#endif
    } else {
      return ARM_DRIVER_ERROR;
    }
  } else {
    // Enable receive data available interrupt
    usart->pREGS->IER |= USART_INTR_RX_DATA;
  }
  return ARM_DRIVER_OK;
}

/**
 * @fn           int32_t USART_Transfer (const void *data_out, void *data_in, uint32_t num,USART_RESOURCES *usart,const UDMA_RESOURCES *udma,UDMA_Channel_Info *chnl_info,RSI_UDMA_HANDLE_T udmaHandle)
 * @brief        Start sending/receiving data to/from USART transmitter/receiver.
 * @param[in]    data_out  Pointer to buffer with data to send to USART transmitter
 * @param[out]   data_in   Pointer to buffer for data to receive from USART receiver
 * @param[in]    num       Number of data items to transfer
 * @param[in]    usart     Pointer to USART resources
 * @param[in]    udma       Pointer to UDMA resources
 * @param[in]    chnl_info  Pointer to UDMA channel info
 * @param[in]    udmaHandle UDMA Hanlde
 * @return      \ref execution_status
 */
int32_t USART_Transfer(const void *data_out,
                       const void *data_in,
                       uint32_t num,
                       USART_RESOURCES *usart,
                       const UDMA_RESOURCES *udma,
                       UDMA_Channel_Info *chnl_info,
                       RSI_UDMA_HANDLE_T udmaHandle)
{
  int32_t status;
  if ((data_out == NULL) || (data_in == NULL) || (num == 0U)) {
    // Invalid parameters
    return ARM_DRIVER_ERROR_PARAMETER;
  }
  if ((usart->info->flags & USART_FLAG_CONFIGURED) == 0U) {
    // USART is not configured
    return ARM_DRIVER_ERROR;
  }
  // for only usart mode
  if (usart->sync_mode.en_usart_mode) {
    if ((usart->capabilities.synchronous_master == 1) || (usart->capabilities.synchronous_slave == 1)) {
      // Set xfer mode
      usart->info->xfer.sync_mode = USART_SYNC_MODE_TX_RX;

      // Receive
      status = USART_Receive_Data(data_in, num, usart, udma, chnl_info, udmaHandle);
      if (status != ARM_DRIVER_OK) {
        return status;
      }
      // Send
      status = USART_Send_Data(data_out, num, usart, udma, chnl_info, udmaHandle);
      if (status != ARM_DRIVER_OK) {
        return status;
      }
    } else {
      // Only in synchronous mode
      return ARM_DRIVER_ERROR;
    }
  }
  return ARM_DRIVER_OK;
}

/**
 * @fn          uint32_t USART_GetTxCount (USART_RESOURCES *usart)
 * @brief       Get transmitted data count.
 * @param[in]   usart     Pointer to USART resources
 * @return      number of data items transmitted
 */
uint32_t USART_GetTxCount(const USART_RESOURCES *usart)
{
  uint32_t cnt;
  cnt = usart->info->xfer.tx_cnt;
  return cnt;
}

/**
 * @fn          uint32_t USART_GetRxCount (USART_RESOURCES *usart)
 * @brief       Get received data count.
 * @param[in]   usart     Pointer to USART resources
 * @return      number of data items received
 */
uint32_t USART_GetRxCount(const USART_RESOURCES *usart)
{
  uint32_t cnt;
  cnt = usart->info->xfer.rx_cnt;
  return cnt;
}

/**
 * @fn          int32_t USART_Control (uint32_t  control, uint32_t  arg,uint32_t baseClk, USART_RESOURCES  *usart, const UDMA_RESOURCES *udma, RSI_UDMA_HANDLE_T udmaHandle)
 * @brief       Control USART Interface.
 * @param[in]   control  Operation
 * @param[in]   arg      Argument of operation (optional)
 * @param[in]   baseClk  UART base clk
 * @param[in]   usart    Pointer to USART resources
 * @param[in]   udma       Pointer to UDMA resources
 * @param[in]   udmaHandle UDMA Hanlde
 * @return      common \ref execution_status and driver specific \ref usart_execution_status
 */
int32_t USART_Control(uint32_t control,
                      uint32_t arg,
                      uint32_t baseClk,
                      USART_RESOURCES *usart,
                      const UDMA_RESOURCES *udma,
                      RSI_UDMA_HANDLE_T udmaHandle)
{
#ifdef SL_SI91X_USART_DMA
  //Added to suppress unused variable warning
  (void)udma;
  (void)udmaHandle;
#endif
  uint32_t mode    = 0;
  uint16_t fcr     = 0;
  uint16_t lcr     = 0;
  uint16_t lcr_ext = 0;
  if ((usart->info->flags & USART_FLAG_POWERED) == 0U) {
    // USART not powered
    return ARM_DRIVER_ERROR;
  }
  switch (control & ARM_USART_CONTROL_Msk) {
    case ARM_USART_CONTROL_TX:
      if (arg) {
        if ((usart->pREGS == UART0) || (usart->pREGS == USART0) || (usart->pREGS == UART1)) {
          if (usart->io.tx->pin > 63) {
            RSI_EGPIO_UlpPadReceiverEnable((uint8_t)(usart->io.tx->pin - 64));
            RSI_EGPIO_SetPinMux(EGPIO1, 0, (uint8_t)(usart->io.tx->pin - 64), 6);
          }
          if (usart->io.tx->pad_sel != 0) {
            RSI_EGPIO_PadSelectionEnable(usart->io.tx->pad_sel);
          }
          if (usart->io.tx->pin >= 25 && usart->io.tx->pin <= 30) {
            RSI_EGPIO_HostPadsGpioModeEnable(usart->io.tx->pin);
          }
          RSI_EGPIO_PadReceiverEnable(usart->io.tx->pin);
          //configure TX pin
          RSI_EGPIO_SetPinMux(EGPIO, usart->io.tx->port, usart->io.tx->pin, usart->io.tx->mode);
        }
        if (usart->pREGS == ULP_UART) {
          //configure TX pin
          RSI_EGPIO_UlpPadReceiverEnable(usart->io.tx->pin);
          RSI_EGPIO_SetPinMux(EGPIO1, usart->io.tx->port, usart->io.tx->pin, usart->io.tx->mode);
        }
        usart->info->flags |= USART_FLAG_TX_ENABLED;
      } else {
        usart->info->flags &= (uint8_t)(~USART_FLAG_TX_ENABLED);
      }
      return ARM_DRIVER_OK;

    case ARM_USART_CONTROL_RX:

      if (arg) {
        if ((usart->pREGS == UART0) || (usart->pREGS == USART0) || (usart->pREGS == UART1)) {
          if (usart->io.rx->pin > 63) {
            RSI_EGPIO_UlpPadDriverDisableState((uint8_t)(usart->io.rx->pin - 64), ulp_Pullup);
            RSI_EGPIO_UlpPadReceiverEnable((uint8_t)(usart->io.rx->pin - 64));
            RSI_EGPIO_SetPinMux(EGPIO1, 0, (uint8_t)(usart->io.rx->pin - 64), 6);
          }
          if (usart->io.rx->pad_sel != 0) {
            RSI_EGPIO_PadSelectionEnable(usart->io.rx->pad_sel);
          }
          if (usart->io.rx->pin >= 25 && usart->io.rx->pin <= 30) {
            RSI_EGPIO_HostPadsGpioModeEnable(usart->io.rx->pin);
          }
          RSI_EGPIO_PadDriverDisableState(usart->io.rx->pin, Pullup);
          RSI_EGPIO_PadReceiverEnable(usart->io.rx->pin);
          RSI_EGPIO_SetPinMux(EGPIO, usart->io.rx->port, usart->io.rx->pin, usart->io.rx->mode);
        }
        if (usart->pREGS == ULP_UART) {
          RSI_EGPIO_UlpPadDriverDisableState(usart->io.rx->pin, ulp_Pullup);
          RSI_EGPIO_UlpPadReceiverEnable(usart->io.rx->pin);
          //configure RX pin
          RSI_EGPIO_SetPinMux(EGPIO1, usart->io.rx->port, usart->io.rx->pin, usart->io.rx->mode);
        }
        usart->info->flags |= USART_FLAG_RX_ENABLED;
      } else {
        usart->info->flags &= (uint8_t)(~USART_FLAG_RX_ENABLED);
      }
      return ARM_DRIVER_OK;

    case ARM_USART_CONTROL_BREAK:
      if (arg) {
        if (usart->info->xfer.send_active != 0U) {
          return ARM_DRIVER_ERROR_BUSY;
        }
        usart->pREGS->LCR |= USART_LCR_BC;

        // Set Send active flag
        usart->info->xfer.send_active = 1U;
      } else {
        usart->pREGS->LCR &= (uint32_t)(~USART_LCR_BC);
        // Clear Send active flag
        usart->info->xfer.send_active = 0U;
      }
      return ARM_DRIVER_OK;

    case ARM_USART_ABORT_SEND:
      // Disable transmit holding register empty interrupt
      usart->pREGS->IER &= (uint32_t)(~USART_INTR_THRE);

      if (usart->dma_rx || usart->dma_tx) {
        fcr |= USART_DMA_MODE_EN;
      }
      // Transmit FIFO reset
      fcr = USART_FIFO_ENABLE;
      fcr |= USART_FIFO_TX_RESET;

      usart->pREGS->FCR = fcr;

      // If DMA mode - disable DMA channel
      if (usart->dma_tx && usart->info->xfer.send_active != 0U
          && (usart->pREGS == UART0 || usart->pREGS == USART0 || usart->pREGS == UART1 || usart->pREGS == ULP_UART)) {
        //DISABLE DMa
#ifdef SL_SI91X_USART_DMA
        if (sl_si91x_dma_channel_disable(dma_init.dma_number, usart->dma_tx->channel + 1)) {
          return ARM_DRIVER_ERROR;
        }
#else
        UDMAx_ChannelDisable(usart->dma_tx->channel, udma, udmaHandle);
#endif
      }
      // Clear Send active flag
      usart->info->xfer.send_active = 0U;
      return ARM_DRIVER_OK;

      // Abort receive
    case ARM_USART_ABORT_RECEIVE:
      // Disable receive data available interrupt
      usart->pREGS->IER &= (uint32_t)(~USART_INTR_RX_DATA);

      if (usart->dma_rx || usart->dma_tx) {
        fcr |= USART_DMA_MODE_EN;
      }
      // Receive FIFO reset
      fcr = USART_FIFO_ENABLE;
      fcr |= USART_FIFO_RX_RESET;
      usart->pREGS->FCR = fcr;

      // If DMA mode - disable DMA channel
      if (usart->dma_rx && usart->info->rx_status.rx_busy
          && (usart->pREGS == UART0 || usart->pREGS == USART0 || usart->pREGS == UART1 || usart->pREGS == ULP_UART)) {
        //DISABLE DMa
#ifdef SL_SI91X_USART_DMA
        if (sl_si91x_dma_channel_disable(dma_init.dma_number, usart->dma_tx->channel + 1)) {
          return ARM_DRIVER_ERROR;
        }
#else
        UDMAx_ChannelDisable(usart->dma_tx->channel, udma, udmaHandle);
#endif
      }
      // Clear RX busy status
      usart->info->rx_status.rx_busy = 0U;

      return ARM_DRIVER_OK;

      // Abort transfer
    case ARM_USART_ABORT_TRANSFER:
      // Disable transmit holding register empty and
      // receive data available interrupts
      usart->pREGS->IER &= (uint32_t)(~(USART_INTR_THRE | USART_INTR_RX_DATA));

      // If DMA mode - disable DMA channel
      if (usart->dma_tx != NULL && usart->info->xfer.send_active
          && (usart->pREGS == UART0 || usart->pREGS == USART0 || usart->pREGS == UART1 || usart->pREGS == ULP_UART)) {
        //DISABLE DMa
#ifdef SL_SI91X_USART_DMA
        if (sl_si91x_dma_channel_disable(dma_init.dma_number, usart->dma_tx->channel + 1)) {
          return ARM_DRIVER_ERROR;
        }
#else
        UDMAx_ChannelDisable(usart->dma_tx->channel, udma, udmaHandle);
#endif
      }
      if (usart->dma_rx && usart->info->rx_status.rx_busy
          && (usart->pREGS == UART0 || usart->pREGS == USART0 || usart->pREGS == UART1 || usart->pREGS == ULP_UART)) {
        //DISABLE DMa
#ifdef SL_SI91X_USART_DMA
        if (sl_si91x_dma_channel_disable(dma_init.dma_number, usart->dma_rx->channel + 1)) {
          return ARM_DRIVER_ERROR;
        }
#else
        UDMAx_ChannelDisable(usart->dma_rx->channel, udma, udmaHandle);
#endif
      }
      // Set trigger level
      if (usart->dma_rx || usart->dma_tx) {
        fcr |= USART_DMA_MODE_EN;
      }
      // Transmit and receive FIFO reset
      fcr = USART_FIFO_ENABLE;
      fcr |= USART_FIFO_RX_RESET | USART_FIFO_TX_RESET;
      usart->pREGS->FCR = fcr;

      // Clear busy statuses
      usart->info->rx_status.rx_busy = 0U;
      usart->info->xfer.send_active  = 0U;
      return ARM_DRIVER_OK;
    default:
      break;
  }
  switch (control & ARM_USART_CONTROL_Msk) {
    case ARM_USART_MODE_ASYNCHRONOUS:
      mode = ARM_USART_MODE_ASYNCHRONOUS;
      break;
    case ARM_USART_MODE_SYNCHRONOUS_MASTER:
      if (usart->sync_mode.en_usart_mode) {
        if (usart->pREGS == USART0) {
          if (usart->capabilities.synchronous_master) {

            if (usart->io.clock->pin > 63) {
              RSI_EGPIO_UlpPadReceiverEnable((uint8_t)(usart->io.clock->pin - 64));
              RSI_EGPIO_SetPinMux(EGPIO1, 0, (uint8_t)(usart->io.clock->pin - 64), 6);
            }
            if (usart->io.clock->pad_sel != 0) {
              RSI_EGPIO_PadSelectionEnable(usart->io.clock->pad_sel);
            }
            if (usart->io.clock->pin >= 25 && usart->io.clock->pin <= 30) {
              RSI_EGPIO_HostPadsGpioModeEnable(usart->io.clock->pin);
            }
            RSI_EGPIO_PadReceiverEnable(usart->io.clock->pin);
            //configure clock pin
            RSI_EGPIO_SetPinMux(EGPIO, usart->io.clock->port, usart->io.clock->pin, usart->io.clock->mode);

            //enable Sync mode
            usart->pREGS->SMCR_b.SYNC_MODE = 1;

            //enable master mode
            usart->pREGS->SMCR_b.MST_MODE = 1;

            //SYNC CKLCOK ENABLE
            if (usart->sync_mode.continuous_clock) {
              usart->pREGS->SMCR_b.CONTI_CLK_MODE = 1U;
            } else {
              usart->pREGS->SMCR_b.CONTI_CLK_MODE = 0U;
            }
            usart->pREGS->SMCR_b.START_STOP_EN = 1;
          }
        } else {
          return ARM_USART_ERROR_MODE;
        }
      } else {
        return ARM_USART_ERROR_MODE;
      }
      mode = ARM_USART_MODE_SYNCHRONOUS_MASTER;

      break;
    case ARM_USART_MODE_SYNCHRONOUS_SLAVE:
      if (usart->pREGS == USART0 && usart->capabilities.synchronous_slave) {
        if (usart->io.clock->pin > 63) {
          RSI_EGPIO_UlpPadReceiverEnable((uint8_t)(usart->io.clock->pin - 64));
          RSI_EGPIO_SetPinMux(EGPIO1, 0, (uint8_t)(usart->io.clock->pin - 64), 6);
        }
        if (usart->io.clock->pad_sel != 0) {
          RSI_EGPIO_PadSelectionEnable(usart->io.clock->pad_sel);
        }
        if (usart->io.clock->pin >= 25 && usart->io.clock->pin <= 30) {
          RSI_EGPIO_HostPadsGpioModeEnable(usart->io.clock->pin);
        }
        RSI_EGPIO_PadReceiverEnable(usart->io.clock->pin);
        // Configure clock pin
        RSI_EGPIO_SetPinMux(EGPIO, usart->io.clock->port, usart->io.clock->pin, usart->io.clock->mode);

        // Enable sync mode
        usart->pREGS->SMCR_b.SYNC_MODE = 1;

        // Enable slave mode
        usart->pREGS->SMCR_b.MST_MODE = 0;
        // SYNC CLOCK ENABLE
        if (usart->sync_mode.continuous_clock) {
          usart->pREGS->SMCR_b.CONTI_CLK_MODE = 1U;
        } else {
          usart->pREGS->SMCR_b.CONTI_CLK_MODE = 0U;
        }
        usart->pREGS->SMCR_b.START_STOP_EN = 1;
      } else {
        return ARM_USART_ERROR_MODE;
      }
      mode = ARM_USART_MODE_SYNCHRONOUS_SLAVE;
      break;

    case ARM_USART_MODE_SINGLE_WIRE:
      if (usart->capabilities.single_wire == 1) {
        //!disable full duplex mode
        usart->pREGS->HDEN_b.FULL_DUPLEX_MODE = 1;
        if (usart->capabilities.synchronous_master) {
          usart->pREGS->HDEN_b.TX_MODE_RX_MODE = 0;
        } else {
          usart->pREGS->HDEN_b.TX_MODE_RX_MODE = 1;
        }
      }
      mode = ARM_USART_MODE_SINGLE_WIRE;
      break;
    case ARM_USART_MODE_IRDA:
      /*the number of data bits(8 DATA BITS) that can be sent is fixed. No parity information can be supplied,
       and only one stop bit is used in this mode. Trying to adjust the number of data bits sent or enable
       parity with the Line Control Register (LCR) has no effect*/
      if ((usart->pREGS == UART0) || (usart->pREGS == USART0)) {
        if (usart->capabilities.irda) {

          //IR TX PIN
          if (usart->io.ir_tx->pin > 63) {
            RSI_EGPIO_UlpPadReceiverEnable((uint8_t)(usart->io.ir_tx->pin - 64));
            RSI_EGPIO_SetPinMux(EGPIO1, 0, (uint8_t)(usart->io.ir_tx->pin - 64), 6);
          }
          if (usart->io.ir_tx->pad_sel != 0) {
            RSI_EGPIO_PadSelectionEnable(usart->io.ir_tx->pad_sel);
          }
          if (usart->io.ir_tx->pin >= 25 && usart->io.ir_tx->pin <= 30) {
            RSI_EGPIO_HostPadsGpioModeEnable(usart->io.ir_tx->pin);
          }
          RSI_EGPIO_PadReceiverEnable(usart->io.ir_tx->pin);
          RSI_EGPIO_SetPinMux(EGPIO, usart->io.ir_tx->port, usart->io.ir_tx->pin, usart->io.ir_tx->mode);

          //IR RX PIN
          if (usart->io.ir_rx->pin > 63) {
            RSI_EGPIO_UlpPadReceiverEnable((uint8_t)(usart->io.ir_rx->pin - 64));
            RSI_EGPIO_SetPinMux(EGPIO1, 0, (uint8_t)(usart->io.ir_rx->pin - 64), 6);
          }
          if (usart->io.ir_rx->pad_sel != 0) {
            RSI_EGPIO_PadSelectionEnable(usart->io.ir_rx->pad_sel);
          }
          if (usart->io.ir_rx->pin >= 25 && usart->io.ir_rx->pin <= 30) {
            RSI_EGPIO_HostPadsGpioModeEnable(usart->io.ir_rx->pin);
          }
          RSI_EGPIO_PadReceiverEnable(usart->io.ir_rx->pin);
          RSI_EGPIO_SetPinMux(EGPIO, usart->io.ir_rx->port, usart->io.ir_rx->pin, usart->io.ir_rx->mode);
        }
        if (usart->capabilities.irda == 1) {
          //Enable SIR mode
          usart->pREGS->MCR_b.SIRE = 1;
        } else {
          return ARM_USART_ERROR_MODE;
        }
      } else {
        return ARM_USART_ERROR_MODE;
      }
      mode = ARM_USART_MODE_IRDA;
      break;
    case ARM_USART_MODE_SMART_CARD:
      if (usart->capabilities.smart_card == 1) {
        return ARM_USART_ERROR_MODE;
      }
      break;
    case ARM_USART_SET_DEFAULT_TX_VALUE:
      usart->info->xfer.tx_def_val = (uint8_t)arg;
      return ARM_DRIVER_OK;

    case ARM_USART_SET_IRDA_PULSE:
      return ARM_USART_ERROR_MODE;

    case ARM_USART_SET_SMART_CARD_GUARD_TIME:
      if (usart->capabilities.smart_card) {
        return ARM_USART_ERROR_MODE;
      }
      break;
    case ARM_USART_SET_SMART_CARD_CLOCK:
      if (usart->capabilities.smart_card_clock) {
        return ARM_USART_ERROR_MODE;
      }
      break;
    case ARM_USART_CONTROL_SMART_CARD_NACK:
      if (usart->capabilities.smart_card) {
        return ARM_USART_ERROR_MODE;
      }
      break;
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  // USART Data bits
  switch (control & ARM_USART_DATA_BITS_Msk) {
    case ARM_USART_DATA_BITS_5:
      lcr |= USART_CNTL_DATALEN_5;
      break;
    case ARM_USART_DATA_BITS_6:
      lcr |= USART_CNTL_DATALEN_6;
      break;
    case ARM_USART_DATA_BITS_7:
      lcr |= USART_CNTL_DATALEN_7;
      break;
    case ARM_USART_DATA_BITS_8:
      lcr |= USART_CNTL_DATALEN_8;
      break;
    case ARM_USART_DATA_BITS_9:
      lcr_ext = USART_CNTL_DATALEN_9;
      lcr_ext |= (1 << 3);
      break;
    default:
      return ARM_USART_ERROR_DATA_BITS;
  }

  //USART Parity
  switch (control & ARM_USART_PARITY_Msk) {
    case ARM_USART_PARITY_NONE:
      lcr &= (uint16_t)(~(USART_PARITY_ENABLE));
      break;
    case ARM_USART_PARITY_EVEN:
      lcr |= USART_PARITY_ENABLE;
      lcr |= USART_CNTL_PARITY_EVEN;
      break;
    case ARM_USART_PARITY_ODD:
      lcr |= USART_PARITY_ENABLE;
      lcr |= USART_CNTL_PARITY_ODD;
      break;
    default:
      return (ARM_USART_ERROR_PARITY);
  }

  // USART Stop bits
  switch (control & ARM_USART_STOP_BITS_Msk) {
    case ARM_USART_STOP_BITS_1:
      lcr |= USART_CNTL_STOPBIT_1;
      break;
    case ARM_USART_STOP_BITS_1_5:
    case ARM_USART_STOP_BITS_2:
      lcr |= USART_CNTL_STOPBIT_2;
      break;
    default:
      return ARM_USART_ERROR_STOP_BITS;
  }
  //enable FIFO mode
  fcr = USART_FIFO_ENABLE;

  // Set trigger level
  if (usart->dma_rx || usart->dma_tx) {
    if (RTE_USART0_DMA_MODE1_EN || RTE_UART1_DMA_MODE1_EN || RTE_ULP_UART_DMA_MODE1_EN) {
      fcr |= USART_DMA_MODE1_EN;
    } else {
      fcr |= USART_DMA_MODE0_EN;
    }
  }
  if ((usart->pREGS == UART0) || (usart->pREGS == USART0)) {
    if (RTE_USART0_TX_FIFO_THRESHOLD == USART_TRIGGER_TX_EMPTY) {
      // Set the TX FIFO threshold level
      fcr |= USART_FIFO_TX_EMPTY;
    }
    if (RTE_USART0_RX_FIFO_THRESHOLD == USART_TRIGGER_RX_AEMPTY) {
      // Set the RX FIFO threshold level
      fcr |= USART_FIFO_RX_AEMPTY;
    }
  } else if (usart->pREGS == UART1) {
    if (RTE_UART1_TX_FIFO_THRESHOLD == USART_TRIGGER_TX_EMPTY) {
      // Set the TX FIFO threshold level
      fcr |= USART_FIFO_TX_EMPTY;
    }
    if (RTE_UART1_RX_FIFO_THRESHOLD == USART_TRIGGER_RX_AEMPTY) {
      // Set the RX FIFO threshold level
      fcr |= USART_FIFO_RX_AEMPTY;
    }
  } else if (usart->pREGS == ULP_UART) {
    if (RTE_ULP_UART_TX_FIFO_THRESHOLD == USART_TRIGGER_TX_EMPTY) {
      // Set the TX FIFO threshold level
      fcr |= USART_FIFO_TX_EMPTY;
    }
    if (RTE_ULP_UART_RX_FIFO_THRESHOLD == USART_TRIGGER_RX_AEMPTY) {
      // Set the RX FIFO threshold level
      fcr |= USART_FIFO_RX_AEMPTY;
    }
  }
  // USART Flow control (RTS and CTS lines are only available on USART0)
  switch (control & ARM_USART_FLOW_CONTROL_Msk) {
    case ARM_USART_FLOW_CONTROL_NONE:
      break;
    case ARM_USART_FLOW_CONTROL_RTS:
      if (usart->capabilities.rts) {
        if ((usart->pREGS == UART0) || (usart->pREGS == USART0) || (usart->pREGS == UART1)) {

          if (usart->io.rts->pin > 63) {
            RSI_EGPIO_UlpPadReceiverEnable((uint8_t)(usart->io.rts->pin - 64));
            RSI_EGPIO_SetPinMux(EGPIO1, 0, (uint8_t)(usart->io.rts->pin - 64), 6);
          }
          if (usart->io.rts->pad_sel != 0) {
            RSI_EGPIO_PadSelectionEnable(usart->io.rts->pad_sel);
          }
          if (usart->io.rts->pin >= 25 && usart->io.rts->pin <= 30) {
            RSI_EGPIO_HostPadsGpioModeEnable(usart->io.rts->pin);
          }
          RSI_EGPIO_PadReceiverEnable(usart->io.rts->pin);
          RSI_EGPIO_SetPinMux(EGPIO, usart->io.rts->port, usart->io.rts->pin, usart->io.rts->mode);
        }
        if (usart->pREGS == ULP_UART) {
          RSI_EGPIO_UlpPadReceiverEnable(usart->io.rts->pin);
          RSI_EGPIO_SetPinMux(EGPIO1, usart->io.rts->port, usart->io.rts->pin, usart->io.rts->mode);
        }
      }
      if (usart->capabilities.flow_control_rts) {
        usart->pREGS->MCR |= (USART_MODEM_AFCE_ENABLE | USART_MODEM_RTS_SET);
      } else {
        return ARM_USART_ERROR_FLOW_CONTROL;
      }
      break;
    case ARM_USART_FLOW_CONTROL_CTS:

      if (usart->capabilities.cts) {
        if ((usart->pREGS == UART0) || (usart->pREGS == USART0) || (usart->pREGS == UART1)) {
          if (usart->io.cts->pin > 63) {
            RSI_EGPIO_UlpPadReceiverEnable((uint8_t)(usart->io.cts->pin - 64));
            RSI_EGPIO_SetPinMux(EGPIO1, 0, (uint8_t)(usart->io.cts->pin - 64), 6);
          }
          if (usart->io.cts->pad_sel != 0) {
            RSI_EGPIO_PadSelectionEnable(usart->io.cts->pad_sel);
          }
          if (usart->io.cts->pin >= 25 && usart->io.cts->pin <= 30) {
            RSI_EGPIO_HostPadsGpioModeEnable(usart->io.cts->pin);
          }
          RSI_EGPIO_PadReceiverEnable(usart->io.cts->pin);
          RSI_EGPIO_SetPinMux(EGPIO, usart->io.cts->port, usart->io.cts->pin, usart->io.cts->mode);
        }
        if (usart->pREGS == ULP_UART) {
          RSI_EGPIO_UlpPadReceiverEnable(usart->io.cts->pin);
          RSI_EGPIO_SetPinMux(EGPIO, usart->io.cts->port, usart->io.cts->pin, usart->io.cts->mode);
        }
      }
      if (usart->capabilities.flow_control_cts) {
        usart->pREGS->MCR |= (USART_MODEM_AFCE_ENABLE);
      } else {
        return ARM_USART_ERROR_FLOW_CONTROL;
      }
      break;
    case ARM_USART_FLOW_CONTROL_RTS_CTS:

      if (usart->capabilities.cts && usart->capabilities.rts) {
        if ((usart->pREGS == UART0) || (usart->pREGS == USART0) || (usart->pREGS == UART1)) {
          //CTS
          if (usart->io.cts->pin > 63) {
            RSI_EGPIO_UlpPadReceiverEnable((uint8_t)(usart->io.cts->pin - 64));
            RSI_EGPIO_SetPinMux(EGPIO1, 0, (uint8_t)(usart->io.cts->pin - 64), 6);
          }
          if (usart->io.cts->pad_sel != 0) {
            RSI_EGPIO_PadSelectionEnable(usart->io.cts->pad_sel);
          }
          if (usart->io.cts->pin >= 25 && usart->io.cts->pin <= 30) {
            RSI_EGPIO_HostPadsGpioModeEnable(usart->io.cts->pin);
          }
          RSI_EGPIO_PadReceiverEnable(usart->io.cts->pin);
          RSI_EGPIO_SetPinMux(EGPIO, usart->io.cts->port, usart->io.cts->pin, usart->io.cts->mode);
          //RTS
          if (usart->io.rts->pin > 63) {
            RSI_EGPIO_UlpPadReceiverEnable((uint8_t)(usart->io.rts->pin - 64));
            RSI_EGPIO_SetPinMux(EGPIO1, 0, (uint8_t)(usart->io.rts->pin - 64), 6);
          }
          if (usart->io.rts->pad_sel != 0) {
            RSI_EGPIO_PadSelectionEnable(usart->io.rts->pad_sel);
          }
          if (usart->io.rts->pin >= 25 && usart->io.rts->pin <= 30) {
            RSI_EGPIO_HostPadsGpioModeEnable(usart->io.rts->pin);
          }
          RSI_EGPIO_PadReceiverEnable(usart->io.rts->pin);
          RSI_EGPIO_SetPinMux(EGPIO, usart->io.rts->port, usart->io.rts->pin, usart->io.rts->mode);
        }
        if (usart->pREGS == ULP_UART) {
          //RTS
          RSI_EGPIO_UlpPadReceiverEnable(usart->io.rts->pin);
          RSI_EGPIO_SetPinMux(EGPIO1, usart->io.rts->port, usart->io.rts->pin, usart->io.rts->mode);

          //CTS
          RSI_EGPIO_UlpPadReceiverEnable(usart->io.cts->pin);
          RSI_EGPIO_SetPinMux(EGPIO1, usart->io.cts->port, usart->io.cts->pin, usart->io.cts->mode);
        }
      }
      if (usart->capabilities.flow_control_rts && usart->capabilities.flow_control_cts) {
        usart->pREGS->MCR |= (USART_MODEM_AFCE_ENABLE | USART_MODEM_RTS_SET);
      } else {
        return ARM_USART_ERROR_FLOW_CONTROL;
      }
      break;
    default: {
      return ARM_USART_ERROR_FLOW_CONTROL;
    }
  }

  // Clock setting for synchronous mode
  if ((mode == ARM_USART_MODE_SYNCHRONOUS_MASTER) || (mode == ARM_USART_MODE_SYNCHRONOUS_SLAVE)) {
    // Only CPOL0 - CPHA1 combination available
    // USART clock polarity
    if ((control & ARM_USART_CPOL_Msk) != ARM_USART_CPOL0) {
      return ARM_USART_ERROR_CPOL;
    }
    // USART clock phase
    if ((control & ARM_USART_CPHA_Msk) != ARM_USART_CPHA0) {
      return ARM_USART_ERROR_CPHA;
    }
  }
  if (RTE_USART0_LOOPBACK || RTE_UART1_LOOPBACK || RTE_ULP_UART_LOOPBACK) {
    /* Set Loop back flag if loop back is enabled */
    usart->pREGS->MCR |= (USART_MODEM_LOOP_BACK_ENABLE);
  } else {
    usart->pREGS->MCR &= (uint32_t)(~(USART_MODEM_LOOP_BACK_ENABLE));
  }
  if (RTE_USART0_DTR_EANBLE) {
    //Enable DTR bit in MCR reg
    usart->pREGS->MCR_b.DTR = 0x1;
  }
  // baud rate calculation
  USART_SetBaudrate(arg, baseClk, usart);

  // Configuration is OK - Mode is valid
  usart->info->mode     = (uint8_t)mode;
  usart->pREGS->FCR     = fcr;
  usart->pREGS->LCR     = lcr;
  usart->pREGS->LCR_EXT = lcr_ext;

  usart->pREGS->TCR_b.XFER_MODE = 1;

  // Set configured flag
  usart->info->flags |= USART_FLAG_CONFIGURED;

  return ARM_DRIVER_OK;
}

/**
 * @fn          ARM_USART_STATUS USART_GetStatus (USART_RESOURCES *usart)
 * @brief       Get USART status.
 * @param[in]   usart     Pointer to USART resources
 * @return      USART status \ref ARM_USART_STATUS
 */
ARM_USART_STATUS USART_GetStatus(const USART_RESOURCES *usart)
{
  ARM_USART_STATUS stat;
  stat.tx_busy          = (usart->pREGS->LSR & USART_LCR_TEMT ? 0 : 1) & 0x01;
  stat.rx_busy          = (unsigned int)((usart->info->rx_status.rx_busy) & 0x01);
  stat.tx_underflow     = 0U;
  stat.rx_overflow      = (unsigned int)((usart->info->rx_status.rx_overflow) & 0x01);
  stat.rx_break         = (unsigned int)((usart->info->rx_status.rx_break) & 0x01);
  stat.rx_framing_error = (unsigned int)((usart->info->rx_status.rx_framing_error) & 0x01);
  stat.rx_parity_error  = (unsigned int)((usart->info->rx_status.rx_parity_error) & 0x01);

  return stat;
}

/**
 * @fn          int32_t USART_SetModemControl (ARM_USART_MODEM_CONTROL  control,USART_RESOURCES *usart)
 * @brief       Set USART Modem Control line state.
 * @param[in]   control   \ref ARM_USART_MODEM_CONTROL
 * @param[in]   usart     Pointer to USART resources
 * @return      \ref execution_status
 */
int32_t USART_SetModemControl(ARM_USART_MODEM_CONTROL control, USART_RESOURCES *usart)
{
  if ((usart->info->flags & USART_FLAG_CONFIGURED) == 0U) {
    // USART is not configured
    return ARM_DRIVER_ERROR;
  }
  // Only UART1 supports modem lines
  if (usart->pREGS == NULL) {
    return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  if (control == ARM_USART_RTS_CLEAR) {
    if (usart->capabilities.rts) {
      usart->pREGS->MCR &= (uint32_t)(~USART_MODEM_RTS_SET);
    } else {
      return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
  }
  if (control == ARM_USART_RTS_SET) {
    if (usart->capabilities.rts) {
      usart->pREGS->MCR |= USART_MODEM_RTS_SET;
    } else {
      return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
  }
  if (control == ARM_USART_DTR_CLEAR) {
    if ((usart->pREGS == UART0) || (usart->pREGS == USART0)) {
      if (usart->capabilities.dtr) {
        usart->pREGS->MCR &= (uint32_t)(~USART_MODEM_DTR_SET);
      } else {
        return ARM_DRIVER_ERROR_UNSUPPORTED;
      }
    } else {
      return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
  }
  if (control == ARM_USART_DTR_SET) {
    if ((usart->pREGS == UART0) || (usart->pREGS == USART0)) {
      if (usart->capabilities.dtr) {
        usart->pREGS->MCR |= USART_MODEM_DTR_SET;
      } else {
        return ARM_DRIVER_ERROR_UNSUPPORTED;
      }
    } else {
      return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
  }
  return ARM_DRIVER_OK;
}

/**
 * @fn          ARM_USART_MODEM_STATUS USART_GetModemStatus (USART_RESOURCES *usart)
 * @brief       Get USART Modem Status lines state.
 * @param[in]   usart     Pointer to USART resources
 * @return      modem status \ref ARM_USART_MODEM_STATUS
 */
ARM_USART_MODEM_STATUS USART_GetModemStatus(const USART_RESOURCES *usart)
{
  ARM_USART_MODEM_STATUS modem_status;
  uint32_t msr;

  if (usart->pREGS && (usart->info->flags & USART_FLAG_CONFIGURED)) {
    msr = usart->pREGS->MSR;

    if (usart->capabilities.cts) {
      if (msr & UART_MSR_CTS) {
        modem_status.cts = 1U;
      } else {
        modem_status.cts = 0U;
      }
    } else {
      modem_status.cts = 0U;
    }
    if (usart->capabilities.dsr) {
      if (msr & UART_MSR_DSR) {
        modem_status.dsr = 1U;
      } else {
        modem_status.dsr = 0U;
      }
    } else {
      modem_status.dsr = 0U;
    }
    if (usart->capabilities.ri) {
      if (msr & UART_MSR_RI) {
        modem_status.ri = 1U;
      } else {
        modem_status.ri = 0U;
      }
    } else {
      modem_status.ri = 0U;
    }
    if (usart->capabilities.dcd) {
      if (msr & UART_MSR_DCD) {
        modem_status.dcd = 1U;
      } else {
        modem_status.dcd = 0U;
      }
    } else {
      modem_status.dcd = 0U;
    }
  } else {
    modem_status.cts = 0U;
    modem_status.dsr = 0U;
    modem_status.ri  = 0U;
    modem_status.dcd = 0U;
  }
  return modem_status;
}
/** @} */

#else
typedef int dummy; // To remove empty translation unit warning.
#endif //A11_ROM || USART_ROMDRIVER_PRESENT
       /*End of file not truncated */
