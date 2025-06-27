/*
 * example_crc.h
 *
 *  Created on: May 3, 2024
 *      Author: vebommak
 */

#ifndef EXAMPLE_CRC_H_
#define EXAMPLE_CRC_H_

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
/**
 * @brief Enable or disable uDMA.
 *
 * Set USE_UDMA to 1 to enable uDMA, set to 0 to disable.
 */
#define USE_UDMA 0

/**
 * @brief Size of the buffer.
 */
#define BUFFSIZE 25

/**
 * @brief Number of DMA descriptors.
 */
#define NUMDMADESC 1

/**
 * @brief DMA channel number.
 */
#define SL_DMA_CHANNEL 5

/**
 * @brief DMA instance number.
 */
#define SL_DMA_INSTANCE 0

// -----------------------------------------------------------------------------
// Prototypes
/***************************************************************************/ /**
 * It is an initialization function, it initializes the clock for CRC.
 *
 * @param none
 * @return none
 ******************************************************************************/
void crc_example_init(void);

/***************************************************************************/ /**
 * The state machine code for send and receive is implemented here.
 * This function is called in while loop.
 *
 * @param none
 * @return none
 ******************************************************************************/
void crc_example_process_action(void);

#endif /* EXAMPLE_CRC_H_ */
