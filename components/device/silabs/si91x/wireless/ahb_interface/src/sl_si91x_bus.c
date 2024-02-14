/*******************************************************************************
* @file
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
#include "sl_status.h"
#include "sl_si91x_types.h"
#include "system_si91x.h"
#include "rsi_m4.h"
#include "sl_constants.h"
#include "cmsis_os2.h"
#include "rsi_power_save.h"
#include "sl_si91x_host_interface.h"
#include <stddef.h>
#include <stdlib.h>
#include "sl_rsi_utility.h"

rsi_m4ta_desc_t tx_desc[2];
rsi_m4ta_desc_t rx_desc[2];

/******************************************************
 * *               Function Declarations
 * ******************************************************/
sl_status_t sli_si91x_submit_rx_pkt(void);
void sli_submit_rx_buffer(void);
void sli_si91x_raise_pkt_pending_interrupt_to_ta(void);

/**
 * @fn          sl_status_t sli_si91x_submit_rx_pkt(void)
 * @brief       Submit receiver packets
 * @param[in]   None
 * @return      0 - Success \n
 *          Non-Zero - Failure
 */
sl_wifi_buffer_t *rx_pkt_buffer;
sl_status_t sli_si91x_submit_rx_pkt(void)
{
  sl_status_t status;
  uint16_t data_length = 0;
  sl_si91x_packet_t *packet;
  int8_t *pkt_buffer = NULL;

  if (M4SS_P2P_INTR_SET_REG & RX_BUFFER_VALID) {
    return -2;
  }

  // Allocate packet to receive packet from module
  status = sl_si91x_host_allocate_buffer(&rx_pkt_buffer, SL_WIFI_RX_FRAME_BUFFER, 1616, 1000);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG("\r\n HEAP EXHAUSTED DURING ALLOCATION \r\n");
    BREAKPOINT();
  }

  packet     = sl_si91x_host_get_buffer_data(rx_pkt_buffer, 0, &data_length);
  pkt_buffer = (int8_t *)&packet->desc[0];

  // Fill source address in the TX descriptors
  rx_desc[0].addr = (M4_MEMORY_OFFSET_ADDRESS + (uint32_t)pkt_buffer);

  // Fill source address in the TX descriptors
  rx_desc[0].length = (16);

  // Fill source address in the TX descriptors
  rx_desc[1].addr = (M4_MEMORY_OFFSET_ADDRESS + (uint32_t)(pkt_buffer + 16));

  // Fill source address in the TX descriptors
  rx_desc[1].length = (1600);

  raise_m4_to_ta_interrupt(RX_BUFFER_VALID);

  return SL_STATUS_OK;
}

sl_status_t sl_si91x_bus_read_frame(sl_wifi_buffer_t **buffer)
{
  sl_status_t status = sl_si91x_host_remove_from_queue(CCP_M4_TA_RX_QUEUE, buffer);
  VERIFY_STATUS_AND_RETURN(status);

  return SL_STATUS_OK;
}

/**
 * @fn          sl_status_t sl_si91x_bus_write_frame(sl_si91x_packet_t *packet,
 *                  uint8_t *payloadparam, uint16_t size_param)
 * @brief       writing a command to the module.
 * @param[in]   payloadparam - pointer to the command payload parameter structure
 * @param[in]   size_param - size of the payload for the command
 * @return      0              - Success \n
 *              Negative Value - Failure
 */

sl_status_t sl_si91x_bus_write_frame(sl_si91x_packet_t *packet, const uint8_t *payloadparam, uint16_t size_param)
{

  // Fill source address in the TX descriptors
  tx_desc[0].addr = (M4_MEMORY_OFFSET_ADDRESS + (uint32_t)&packet->desc[0]);

  // Fill source address in the TX descriptors
  tx_desc[0].length = (16);

  // Fill source address in the TX descriptors
  tx_desc[1].addr = (M4_MEMORY_OFFSET_ADDRESS + (uint32_t)payloadparam);

  // Fill source address in the TX descriptors
  tx_desc[1].length = (size_param);

  sli_si91x_raise_pkt_pending_interrupt_to_ta();

  return SL_STATUS_OK;
}

void sli_submit_rx_buffer(void)
{
  mask_ta_interrupt(RX_PKT_TRANSFER_DONE_INTERRUPT);

  //! submit to TA submit packet
  sli_si91x_submit_rx_pkt();

  unmask_ta_interrupt(RX_PKT_TRANSFER_DONE_INTERRUPT);
}

/**
 * @fn           void rsi_update_tx_dma_desc(uint8 skip_dma_valid)
 * @brief        This function updates the TX DMA descriptor address
 * @param[in]    skip_dma_valid
 * @param[out]   none
 * @return       none
 * @section description
 * This function updates the TX DMA descriptor address
 *
 *
 */

void rsi_update_tx_dma_desc(uint8_t skip_dma_valid)
{
  if (!skip_dma_valid) {
#ifdef SLI_SI91X_MCU_COMMON_FLASH_MODE
    if (!(M4_ULP_SLP_STATUS_REG & MCU_ULP_WAKEUP))
#endif
    {
      while (M4_TX_DMA_DESC_REG & DMA_DESC_REG_VALID)
        ;
    }
  }
  M4_TX_DMA_DESC_REG = (uint32_t)&tx_desc;
}

/*==============================================*/
/**
 * @fn           void rsi_update_rx_dma_desc()
 * @brief        This function updates the RX DMA descriptor address
 * @param[in]    none
 * @param[out]   none
 * @return       none
 * @section description
 * This function updates the RX DMA descriptor address
 *
 *
 */
void rsi_update_rx_dma_desc(void)
{
  M4_RX_DMA_DESC_REG = (uint32_t)&rx_desc;
}

void sli_si91x_config_m4_dma_desc_on_reset(void)
{

  //! Wait for TA to wakeup and should be in bootloader
  while (!(P2P_STATUS_REG & TA_is_active))
    ;
  SL_DEBUG_LOG("\r\nTA is in active state\r\n");
  //! TBD Need to address why soft reset expecting delay
  osDelay(100);
  //! Update M4 Tx and Rx DMA descriptors
  M4_TX_DMA_DESC_REG = (uint32_t)&tx_desc;
  M4_RX_DMA_DESC_REG = (uint32_t)&rx_desc;
}
