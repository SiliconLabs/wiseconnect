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
