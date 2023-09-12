/*******************************************************************************
 * @file  sl_si91x_peripheral_sdio_secondary.h
 * @brief sdio secondary peripheral API's
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
#ifndef __SL_SI91X_PERIPHERAL_SDIO_SECONDARY_H_
#define __SL_SI91X_PERIPHERAL_SDIO_SECONDARY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "rsi_chip.h"

/***************************************************************************/ /**
 * @addtogroup SDIO Secondary
 * @ingroup SI91X_PERIPHERAL_APIS
 * @{
 * 
 ******************************************************************************/

/*******************************************************************************
 ***************************  DEFINES / MACROS   ********************************
 ******************************************************************************/
#define SDIO SDIO0 // SDIO Base Address

#define SDIO_Handler  HIF1_IRQHandler
#define GPDMA_Handler IRQ031_Handler

/// SDIO Write FIFO Data Register
#define RX_SOURCE_ADDR 0x20200080

/// SDIO Read FIFO Data Register
#define TX_SOURCE_ADDR 0x20200040

#define SDIO_MODE_SELECT    0x0705
#define MASK_HOST_INTERRUPT 0xF0

#define M4_MISC_CONFIG_BASE 0x46008000
#define SDIO_BASE           0x20200000

#define RX_NUM_CHUNKS           (*(volatile uint32_t *)(SDIO_BASE + 0x242))
#define M4_HOST_INTR_MASK_REG   (*(volatile uint32_t *)(M4_MISC_CONFIG_BASE + 0x00))
#define M4_HOST_INTR_STATUS_REG (*(volatile uint32_t *)(M4_MISC_CONFIG_BASE + 0x04))
#define M4_HOST_INTR_CLEAR      (*(volatile uint32_t *)(M4_MISC_CONFIG_BASE + 0x08))
#define MISC_CFG_HOST_CTRL      (*(volatile uint32_t *)(M4_MISC_CONFIG_BASE + 0x0C))

// SDIO Function1 interrupt enable bits
#define SL_SDIO_WR_INT_EN      BIT(0) // This bit is used to enable CMD53 write interrupt
#define SL_SDIO_RD_INT_EN      BIT(1) // This bit is used to enable CMD53 read interrupt
#define SL_SDIO_CSA_INT_EN     BIT(2) // This bit is used to enable CMD53 CSA interrupt
#define SL_SDIO_CMD52_INT_EN   BIT(3) // This bit is used to enable CMD52 interrupt
#define SL_SDIO_PWR_LEV_INT_EN BIT(4) // This bit is used to enable power level change interrupt
#define SL_SDIO_CRC_ERR_INT_EN BIT(5) // This bit is used to enable CRC error interrupt
#define SL_SDIO_ABORT_INT_EN   BIT(6) // This bit is used to enable abort interrupt
#define SL_SDIO_TOUT_INT_EN    BIT(7) // This bit is used to enable "read FIFO wait time over" interrupt

// SDIO Function1 interrupt mask bits
#define SL_SDIO_WR_INT_MSK      BIT(0) // This bit is used to mask CMD53 write interrupt
#define SL_SDIO_RD_INT_MSK      BIT(1) // This bit is used to mask CMD53 read interrupt
#define SL_SDIO_CSA_INT_MSK     BIT(2) // This bit is used to mask CMD53 CSA interrupt
#define SL_SDIO_CMD52_INT_MSK   BIT(3) // This bit is used to mask CMD52 interrupt
#define SL_SDIO_PWR_LEV_INT_MSK BIT(4) // This bit is used to mask power level change interrupt
#define SL_SDIO_CRC_ERR_INT_MSK BIT(5) // This bit is used to mask CRC error interrupt
#define SL_SDIO_ABORT_INT_MSK   BIT(6) // This bit is used to mask abort interrupt
#define SL_SDIO_TOUT_INT_MSK    BIT(7) // This bit is used to mask "read FIFO wait time over" interrupt

// SDIO Function1 interrupt unmask bits
#define SL_SDIO_WR_INT_UNMSK      BIT(0) // This bit is used to unmask CMD53 write interrupt
#define SL_SDIO_RD_INT_UNMSK      BIT(1) // This bit is used to unmask CMD53 read interrupt
#define SL_SDIO_CSA_INT_UNMSK     BIT(2) // This bit is used to unmask CMD53 CSA interrupt
#define SL_SDIO_CMD52_INT_UNMSK   BIT(3) // This bit is used to unmask CMD52 interrupt
#define SL_SDIO_PWR_LEV_INT_UNMSK BIT(4) // This bit is used to unmask power level change interrupt
#define SL_SDIO_CRC_ERR_INT_UNMSK BIT(5) // This bit is used to unmask CRC error interrupt
#define SL_SDIO_ABORT_INT_UNMSK   BIT(6) // This bit is used to unmask abort interrupt
#define SL_SDIO_TOUT_INT_UNMSK    BIT(7) // This bit is used to unmask "read FIFO wait time over" interrupt

/***************************************************************************/ /**
 * This API initializes the SDIO secondary, by default SDIO pin access with TA
 * this API gives the pin access to M4, selects the SDIO mode and enable the sdio
 * primary interrupts
 *
 * @param[in] none
 *
 * @return none
 ******************************************************************************/
void sl_si91x_sdio_secondary_peripheral_init(void);

/***************************************************************************/ /**
 * Enable the SDIO interrupts.
 *
 * @param[in] flags Interrupt Flag which need to be enabled
 *          Different interrupts flag which can be enabled are,
 *          \ref SL_SDIO_WR_INT_EN      \n
 *          \ref SL_SDIO_RD_INT_EN      \n
 *          \ref SL_SDIO_CSA_INT_EN     \n
 *          \ref SL_SDIO_CMD52_INT_EN   \n
 *          \ref SL_SDIO_PWR_LEV_INT_EN \n
 *          \ref SL_SDIO_CRC_ERR_INT_EN \n
 *          \ref SL_SDIO_ABORT_INT_EN   \n
 *          \ref SL_SDIO_TOUT_INT_EN    \n
 *
 * @return none
 ******************************************************************************/
__STATIC_INLINE void sl_si91x_sdio_secondary_enable_interrupts(uint32_t flags)
{
  SDIO->SDIO_INTR_FN1_ENABLE_REG |= flags;
  NVIC_EnableIRQ(HIF0_IRQn);
}

/***************************************************************************/ /**
 * Disable the SDIO interrupts.
 *
 * @param[in] flags Interrupt Flag which need to be disabled
 *          Different interrupts flags which can be disabled are,
 *          \ref SL_SDIO_WR_INT_EN      \n
 *          \ref SL_SDIO_RD_INT_EN      \n
 *          \ref SL_SDIO_CSA_INT_EN     \n
 *          \ref SL_SDIO_CMD52_INT_EN   \n
 *          \ref SL_SDIO_PWR_LEV_INT_EN \n
 *          \ref SL_SDIO_CRC_ERR_INT_EN \n
 *          \ref SL_SDIO_ABORT_INT_EN   \n
 *          \ref SL_SDIO_TOUT_INT_EN    \n
 *
 * @return none
 ******************************************************************************/
__STATIC_INLINE void sl_si91x_sdio_secondary_disable_interrupts(uint32_t flags)
{
  SDIO->SDIO_INTR_FN1_ENABLE_REG &= (~flags);
  NVIC_DisableIRQ(HIF0_IRQn);
}

/***************************************************************************/ /**
 * Set the interrupts i.e., unmask according to the flags passed in the parameter.
 * @pre \ref sl_si91x_sdio_secondary_enable_interrupts();
 *
 * @param[in] flags Interrupt flags which needs to be set.
 *          Different interrupts flags which need to be set,
 *          \ref SL_SDIO_WR_INT_UNMSK      \n
 *          \ref SL_SDIO_RD_INT_UNMSK      \n
 *          \ref SL_SDIO_CSA_INT_UNMSK     \n
 *          \ref SL_SDIO_CMD52_INT_UNMSK   \n
 *          \ref SL_SDIO_PWR_LEV_INT_UNMSK \n
 *          \ref SL_SDIO_CRC_ERR_INT_UNMSK \n
 *          \ref SL_SDIO_ABORT_INT_UNMSK   \n
 *          \ref SL_SDIO_TOUT_INT_UNMSK    \n
 *
 * @return none
 ******************************************************************************/
__INLINE void sl_si91x_sdio_secondary_set_interrupts(uint32_t flags)
{
  SDIO->SDIO_INTR_FN1_UNMASK_REG |= flags;
}

/***************************************************************************/ /**
 * Clear the interrupts i.e., mask according to the flags passed in the parameter.
 * @pre \ref sl_si91x_sdio_secondary_disable_interrupts();
 *
 * @param[in] flags Interrupt flags which needs to be masked.
 *          \ref SL_SDIO_WR_INT_MSK      \n
 *          \ref SL_SDIO_RD_INT_MSK      \n
 *          \ref SL_SDIO_CSA_INT_MSK     \n
 *          \ref SL_SDIO_CMD52_INT_MSK   \n
 *          \ref SL_SDIO_PWR_LEV_INT_MSK \n
 *          \ref SL_SDIO_CRC_ERR_INT_MSK \n
 *          \ref SL_SDIO_ABORT_INT_MSK   \n
 *          \ref SL_SDIO_TOUT_INT_MSK    \n

 * @return none
 ******************************************************************************/
__INLINE void sl_si91x_sdio_secondary_clear_interrupts(uint32_t flags)
{
  SDIO->SDIO_INTR_FN1_MASK_REG |= flags;
  SDIO->SDIO_INTR_FN1_STATUS_CLEAR_REG |= flags;
}

/***************************************************************************/ /**
 * Get the pending function interrupt
 *
 * @param[in] none
 *
 * @return Return the pending interrupt status
 ******************************************************************************/
__INLINE uint32_t sl_si91x_sdio_secondary_get_pending_interrupts(void)
{
  return SDIO->SDIO_INTR_STATUS_REG;
}

/***************************************************************************/ /**
 * Get all the enabled interrupts in function1
 *
 * @param[in] none
 *
 * @return No of interrupts enabled
 ******************************************************************************/
__INLINE uint32_t sl_si91x_sdio_secondary_get_enabled_interrupts(void)
{
  return SDIO->SDIO_INTR_FN1_UNMASK_REG;
}

/***************************************************************************/ /**
 * Get all the enabled interrupts in function1 and pending interrupts
 *
 * @param[in] none
 *
 * @return No of interrupts enabled
 ******************************************************************************/
__INLINE uint32_t sl_si91x_sdio_secondary_get_enabled_pending_interrupts(void)
{
  return (SDIO->SDIO_INTR_STATUS_REG & SDIO->SDIO_INTR_FN1_UNMASK_REG);
}

/**************************************************************************/ /**
 * To get block count for the last received CMD53
 *
 * @param[in] none
 *
 * @return No of block counts
 ******************************************************************************/
__INLINE uint16_t sl_si91x_sdio_secondary_get_block_cnt(void)
{
  // return the block count for last received command 53
  return SDIO->SDIO_BLK_CNT_REG;
}

/**************************************************************************/ /**
 * To get the length of each for the last received CMD53
 *
 * @param[in] none
 *
 * @return Length of each block
 ******************************************************************************/
__INLINE uint16_t sl_si91x_sdio_secondary_get_block_len(void)
{
  // return the length of each block for last received command 53
  return SDIO->SDIO_BLK_LEN_REG;
}

/**************************************************************************/ /**
 * To set no of blocks to be transferred
 * This API is used when transferring the data from secondary to primary in
 * block mode
 *
 * @param[in] no_of_blocks  no of blocks to be transfered
 *
 * @return none
 ******************************************************************************/
__INLINE void sl_si91x_sdio_secondary_set_tx_blocks(uint8_t no_of_blocks)
{
  RX_NUM_CHUNKS = no_of_blocks;
}

/** @} (end addtogroup SDIO Secondary) */

#ifdef __cplusplus
}
#endif

#endif //__SL_SI91X_PERIPHERAL_SDIO_SECONDARY_H_
