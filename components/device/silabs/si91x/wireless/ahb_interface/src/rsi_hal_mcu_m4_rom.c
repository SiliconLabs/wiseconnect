/*******************************************************************************
* @file  rsi_hal_mcu_m4_rom.c
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_si91x_types.h"
#include "sl_constants.h"
#include "sl_status.h"
#include "sl_device.h"
#include "sl_rsi_utility.h"
#include "rsi_m4.h"

#ifdef SL_WIFI_COMPONENT_INCLUDED
#include "sl_si91x_host_interface.h"
#endif

#include "cmsis_os2.h"
osEventFlagsId_t ta_events = NULL;
#define TA_PKT_TX_DONE (1 << 1)
#ifdef SL_SI91X_SIDE_BAND_CRYPTO
#define SIDE_BAND_DONE (1 << 2)
#endif

/** @addtogroup SOC4
* @{
*/
/**
 * @fn           void sli_si91x_raise_pkt_pending_interrupt_to_ta(void)
 * @brief        Raise the packet pending interrupt to TA
 * @param[in]    void  
 * @return       void
 */
void sli_si91x_raise_pkt_pending_interrupt_to_ta(void)
{
  // Write the packet pending interrupt to TA register
  M4SS_P2P_INTR_SET_REG = TX_PKT_PENDING_INTERRUPT;
  osEventFlagsWait(ta_events, TA_PKT_TX_DONE, (osFlagsWaitAny), osWaitForever);
}

#ifdef SL_SI91X_SIDE_BAND_CRYPTO
/**
 * @fn           void sli_si91x_raise_side_band_interrupt_to_ta(void)
 * @brief        Raise the side band interrupt to TA
 * @param[in]    void  
 * @return       void
 */
void sli_si91x_raise_side_band_interrupt_to_ta(void)
{
  // Write the packet pending interrupt to TA register
  M4SS_P2P_INTR_SET_REG = SIDE_BAND_CRYPTO_INTR;
}
#endif

/**
 * @fn          void raise_m4_to_ta_interrupt(uint32_t interrupt_no)
 * @brief       Set interrupt.
 * @param[in]   interrupt_no - Process of a interrupt number 
 * @return      void 
 */

void raise_m4_to_ta_interrupt(uint32_t interrupt_no)
{
  M4SS_P2P_INTR_SET_REG = interrupt_no;
}

/**
 * @fn          void mask_ta_interrupt(uint32_t interrupt_no)
 * @brief       Process a interrupt mask.
 * @param[in]   void  
 * @return      void
 */
void mask_ta_interrupt(uint32_t interrupt_no)
{
  TASS_P2P_INTR_MASK_SET = interrupt_no;
}

/**
 * @fn          void clear_ta_to_m4_interrupt(uint32_t interrupt_no)
 * @brief       Clear interrupt raised by TA.
 * @param[in]   interrupt_no - Process of a interrupt number 
 * @return      void 
 */
void clear_ta_to_m4_interrupt(uint32_t interrupt_no)
{
  TASS_P2P_INTR_CLEAR   = interrupt_no;
  TASS_P2P_INTR_CLR_REG = interrupt_no;
}

/**
 * @fn           void sli_m4_interrupt_isr(void)
 * @brief        Raise the packet pending interrupt to TA
 * @param[in]    void
 * @return       void
 */

sl_status_t sli_m4_interrupt_isr(void)
{
  if (TASS_P2P_INTR_CLEAR & TX_PKT_TRANSFER_DONE_INTERRUPT) {

    osEventFlagsSet(ta_events, TA_PKT_TX_DONE);
    // Clear the interrupt
    clear_ta_to_m4_interrupt(TX_PKT_TRANSFER_DONE_INTERRUPT);

  } else if (TASS_P2P_INTR_CLEAR & RX_PKT_TRANSFER_DONE_INTERRUPT) {

    // Call done interrupt isr
    sl_status_t status = sli_receive_from_ta_done_isr();
    VERIFY_STATUS_AND_RETURN(status);

    // Clear the interrupt
    clear_ta_to_m4_interrupt(RX_PKT_TRANSFER_DONE_INTERRUPT);

  }
#ifdef SLI_SI917
  else if (TASS_P2P_INTR_CLEAR & TA_WRITING_ON_COMM_FLASH) {
    //! moves m4 app to RAM and polls for TA done
    sl_mv_m4_app_from_flash_to_ram(TA_WRITES_ON_COMM_FLASH);
    // Clear the interrupt
    clear_ta_to_m4_interrupt(TA_WRITING_ON_COMM_FLASH);
  } else if (TASS_P2P_INTR_CLEAR & NWP_DEINIT_IN_COMM_FLASH) {
    //! moves m4 app to RAM and polls for TA done
    sl_mv_m4_app_from_flash_to_ram(M4_WAIT_FOR_NWP_DEINIT);
    // Clear the interrupt
    clear_ta_to_m4_interrupt(NWP_DEINIT_IN_COMM_FLASH);
  }
  //! Below changes are requried for M4 Image upgration in dual flash config
  else if (TASS_P2P_INTR_CLEAR & M4_IMAGE_UPGRADATION_PENDING_INTERRUPT) {
    //! moves m4 app to RAM and polls for TA done
    sl_mv_m4_app_from_flash_to_ram(UPGRADE_M4_IMAGE_OTA);
    // Clear the interrupt
    clear_ta_to_m4_interrupt(M4_IMAGE_UPGRADATION_PENDING_INTERRUPT);
  }
#endif
#ifdef SL_SI91X_SIDE_BAND_CRYPTO
  //! Below changes are requried for SIDE BAND CRYPTO
  else if (TASS_P2P_INTR_CLEAR & SIDE_BAND_CRYPTO_DONE) {
    osEventFlagsSet(ta_events, SIDE_BAND_DONE);
    // Clear the interrupt
    clear_ta_to_m4_interrupt(SIDE_BAND_CRYPTO_DONE);
  }
#endif
  else {
    SL_DEBUG_LOG("\r\n INVALID INTERRUPT \r\n", 0);
    BREAKPOINT();
  }
  return SL_STATUS_OK;
}

/**
 * @fn           sl_status_t sli_receive_from_ta_done_isr(void)
 * @brief        Called when DMA done for RX packet is received
 * @param[in]    global_cb_p - pointer to the global control block
 * @return       void
 */
sl_status_t sli_receive_from_ta_done_isr(void)
{
#ifdef SL_WIFI_COMPONENT_INCLUDED
  extern sl_wifi_buffer_t *rx_pkt_buffer;
  // Add to rx packet to CCP queue
  sl_status_t status = sl_si91x_host_add_to_queue(CCP_M4_TA_RX_QUEUE, rx_pkt_buffer);
  VERIFY_STATUS_AND_RETURN(status);

  //! Set event RX pending event to host
  sl_si91x_host_set_bus_event(SL_SI91X_NCP_HOST_BUS_RX_EVENT);
#endif

  return SL_STATUS_OK;
}

/*==================================================*/
/**
 * @fn          sl_status_t sl_si91x_bus_read_interrupt_status(uint8_t *int_status)
 * @brief       Returns the value of the Interrupt register
 * @param[in]   status
 * @param[out]  buffer full status reg value
 * @return      errorcode
 *               0 = Success
 *              -2 = Reg read failure
 */
sl_status_t sl_si91x_bus_read_interrupt_status(uint16_t *int_status)
{
  *int_status = (uint8_t)HOST_INTR_STATUS_REG;

  return RSI_SUCCESS;
}

sl_status_t si91x_req_wakeup(void)
{
  P2P_STATUS_REG |= M4_wakeup_TA;
  if (!(P2P_STATUS_REG & TA_is_active)) {
    //!TBD Need add timeout
    while (!(P2P_STATUS_REG & TA_is_active))
      ;
  }
  return SL_STATUS_OK;
}

void sl_si91x_host_clear_sleep_indicator(void)
{
  P2P_STATUS_REG &= ~M4_wakeup_TA;
}

void IRQ074_Handler(void)
{
  sli_m4_interrupt_isr();
}

void sl_si91x_ta_events_init(void)
{
  if (ta_events == NULL) {
    ta_events = osEventFlagsNew(NULL);
  }
}
/** @} */