/*******************************************************************************
 * @file  sl_si91x_sdio_slave.h
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
#ifndef __SL_SI91X_SDIO_SLAVE_H_
#define __SL_SI91X_SDIO_SLAVE_H_

#include "rsi_chip.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#define SDIO_MODE_SELECT    0x0705
#define MASK_HOST_INTERRUPT 0xF0

#define M4_MISC_CONFIG_BASE 0x46008000
#define SDIO_BASE           0x20200000

#define M4_HOST_INTR_MASK_REG   (*(volatile uint32_t *)(M4_MISC_CONFIG_BASE + 0x00))
#define M4_HOST_INTR_STATUS_REG (*(volatile uint32_t *)(M4_MISC_CONFIG_BASE + 0x04))
#define M4_HOST_INTR_CLEAR      (*(volatile uint32_t *)(M4_MISC_CONFIG_BASE + 0x08))
#define MISC_CFG_HOST_CTRL      (*(volatile uint32_t *)(M4_MISC_CONFIG_BASE + 0x0C))

#define SDIO_FN1_INT_CLEAR      (*(volatile uint32_t *)(SDIO_BASE + 0x00))
#define SDIO_FN1_INT_ENABLE     (*(volatile uint32_t *)(SDIO_BASE + 0x04))
#define SDIO_FN1_INT_MASK_REG   (*(volatile uint16_t *)(SDIO_BASE + 0x08))
#define SDIO_FN1_INT_UNMASK_REG (*(volatile uint32_t *)(SDIO_BASE + 0x0C))
#define SDIO_BLK_LEN_REG        (*(volatile uint32_t *)(SDIO_BASE + 0x10))
#define SDIO_BLK_CNT            (*(volatile uint16_t *)(SDIO_BASE + 0x14))
#define SDIO_ADDRESS_REG        (*(volatile uint16_t *)(SDIO_BASE + 0x18))
#define RX_NUM_CHUNKS           (*(volatile uint32_t *)(SDIO_BASE + 0x242))

#define NUMGPDMADESC 10

/// SDIO Write FIFO Data Register
#define RX_SOURCE_ADDR 0x20200080

/// SDIO Read FIFO Data Register
#define TX_SOURCE_ADDR 0x20200040

/// Events for HIF irq handler
#define HOST_INTR_RECEIVE_EVENT BIT(0)

/// Bits masks for registers
#define INT_STAT_TX_TMOUT  BIT(15)
#define INT_STAT_ABRT_SPI  BIT(14)
#define INT_STAT_ABRT_SDIO BIT(6)
#define INT_STAT_CRC_SDIO  BIT(11)
#define INT_STAT_CMD53_RD  BIT(1)
#define INT_STAT_CMD53_WR  BIT(0)
#define INT_STAT_WR_RD_REQ BIT(8)
#define INT_EN_CMD53_WR    BIT(0)
#define INT_STAT_CMD53_WR  BIT(0)

/*******************************************************************************
 *****************************   DATA TYPES   *********************************
 ******************************************************************************/

typedef enum {
  HOST_INTR_NOT_RECEIVED = 0,
  HOST_INTR_RECEIVED     = 1,
} sl_sdio_slave_rx_intr_status_t;

typedef void (*sl_sdio_slave_callback_t)(uint8_t events);

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/***************************************************************************/ /**
 * @brief       
 *   Initialize sdio slave peripheral for transfers 
 *
 * @param[in] event_callback
 *   Address of application callback function for host interrupt events 
 ******************************************************************************/
void sl_si91x_sdio_slave_init(sl_sdio_slave_callback_t event_callback);

/***************************************************************************/ /**
 * @brief       
 *   Trigger sequence to send data from sdio slave to host/master device 
 *
 * @param[in] num_of_blocks
 *   Number of blocks to be sent
 * 
 * @param[in] data_buf
 *   Reference of the Source buffer
 ******************************************************************************/
void sl_si91x_sdio_slave_send(uint8_t num_of_blocks, uint8_t *data_buf);

/***************************************************************************/ /**
 * @brief       
 *   Trigger sequence to receive data on sdio slave from host/master device  
 *
 * @param[in] data_buf
 *   Reference of the Destination buffer
 ******************************************************************************/
void sl_si91x_sdio_slave_receive(uint8_t *data_buf);

#endif //__SL_SI91X_SDIO_SLAVE_H_