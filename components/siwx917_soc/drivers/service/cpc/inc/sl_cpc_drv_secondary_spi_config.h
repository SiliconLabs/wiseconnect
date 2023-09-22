/*******************************************************************************
* @file  sl_cpc_drv_secondary_spi_config.h
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

#ifndef SL_CPC_DRV_SPI_SECONDARY_CONFIG_H
#define SL_CPC_DRV_SPI_SECONDARY_CONFIG_H

#define SPI_BAUD      10000000 // speed at which data transmitted through SPI; Max is 40000000
#define SPI_BIT_WIDTH 8        // SPI bit width can be 16/8 for 16/8 bit data transfer

// <o SL_CPC_DRV_SPI_EXP_RX_QUEUE_SIZE> Number of frame that can be queued in the driver receive queue
// <i> Default: 10
#define SL_CPC_DRV_SPI_EXP_RX_QUEUE_SIZE 10

// <o SL_CPC_DRV_SPI_EXP_TX_QUEUE_SIZE> Number of frame that can be queued in the driver transmit queue
// <i> Default: 10
#define SL_CPC_DRV_SPI_EXP_TX_QUEUE_SIZE 10

#define SL_CPC_DRV_SPI_RX_QUEUE_SIZE SL_CPC_DRV_SPI_EXP_RX_QUEUE_SIZE
#define SL_CPC_DRV_SPI_TX_QUEUE_SIZE SL_CPC_DRV_SPI_EXP_TX_QUEUE_SIZE

void sl_cpc_process_tx_rx(void);
#endif
