/*******************************************************************************
* @file crc_example.c
* @brief CRC example
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

/* Includes ------------------------------------------------------------------*/
#include "sw_crc.h"
#include "rsi_debug.h"
#include "sl_si91x_crc.h"
#include "sl_si91x_dma.h"
#include "crc_example.h"

/*******************************************************************************
***************************  Defines / Macros  ********************************
******************************************************************************/
/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/

/*******************************************************************************
 **************************   GLOBAL VARIABLES   *******************************
 ******************************************************************************/
/**
 * @brief Input data buffer for CRC calculation.
 */
uint32_t gcrc_tx_Buf[BUFFSIZE] = { 0x6e95c16f, 0x26032526, 0x453ea633, 0x33301c04, 0xa531e3de, 0xb040d130, 0xabac09c0,
                                   0x539d6754, 0x3036bedf, 0xa2c998df, 0xa7d3bced, 0xb93e262e, 0xd096b2a7, 0xf2b32b1c,
                                   0x5192ff8d, 0x6dc046e6, 0x2d3256fe, 0x73ddaa7c, 0x17e73ab6, 0xc3ca122a, 0xb03f5ba4,
                                   0xb34e87a3, 0x9854db27, 0x3c5c176a, 0x67a367ac };

/**
 * @brief Temporary buffer used for CRC calculation.
 */
uint32_t gtx_temp_buff[BUFFSIZE];

/**
 * @brief CRC parameters structure.
 */
sl_crc_params_t crc_params;

/**
 * @brief Hardware CRC value.
 */
uint32_t ghw_crc;

/**
 * @brief DMA transfer structure.
 */
sl_dma_xfer_t dma_transfer_t = { 0 };

/**
 * @brief DMA channel number.
 */
uint32_t gchannel = SL_DMA_CHANNEL;

/**
 * @brief Flag indicating DMA transfer completion.
 */
volatile uint8_t dma_transfer_done = 0;

/**
 * @brief software CRC value.
 */
unsigned int gsw_crc;

/* Private function prototypes -----------------------------------------------*/
static uint32_t crc_convert_tx_buffer(uint32_t num);
/*******************************************************************************
**************************   GLOBAL FUNCTIONS   *******************************
******************************************************************************/
/*******************************************************************************
 * Transfer callback function.
 ******************************************************************************/
void transfer_complete_callback_dmadrv(uint32_t channel, void *data)
{
  (void)channel;
  (void)data;
  dma_transfer_done = 1;
}

/**
 * @brief Converts a 32-bit number to network byte order (big-endian).
 *
 * This function converts a 32-bit number to network byte order (big-endian).
 *
 * @param num Number to be converted.
 *
 * @return Converted number in network byte order.
 */
uint32_t crc_convert_tx_buffer(uint32_t num)
{
  int byte0, byte1, byte2, byte3;

  byte0 = (num & 0x000000FF) >> 0;
  byte1 = (num & 0x0000FF00) >> 8;
  byte2 = (num & 0x00FF0000) >> 16;
  byte3 = (num & 0xFF000000) >> 24;
  return ((byte0 << 24) | (byte1 << 16) | (byte2 << 8) | (byte3 << 0));
}

/**
 * @brief Initializes the CRC calculation example.
 *
 * This function initializes the CRC calculation example.
 */
void crc_example_init(void)
{
  sl_status_t status     = SL_STATUS_OK;
  sl_dma_init_t dma_init = { SL_DMA_INSTANCE };
  sl_dma_callback_t callbacks;

  DEBUGOUT("\n******* CRC EXAMPLE INIT *********\n");
  do {
    status = sl_si91x_dma_init(&dma_init);
    if (status == SL_STATUS_OK) {
      //UDMA initialization success
      DEBUGOUT("\r\nUDMA Initialization Success\r\n");

    } else {
      //UDMA initialization fail
      DEBUGOUT("\r\nFailed to Initialize UDMA\r\n");
      break;
    }
    //Allocate channel for transfer
    status = sl_si91x_dma_allocate_channel(SL_DMA_INSTANCE, &gchannel, 0);
    if (status == SL_STATUS_OK) {
      //Channel successfully allocated
      DEBUGOUT("\r\nChannel Allocated successfully\r\n");

    } else {
      //Channel allocation failed
      DEBUGOUT("\r\nChannel not allocated\r\n");
      break;
    }
    callbacks.transfer_complete_cb = transfer_complete_callback_dmadrv;

    //Register transfer complete callback
    status = sl_si91x_dma_register_callbacks(SL_DMA_INSTANCE, gchannel, &callbacks);
    if (status == SL_STATUS_OK) {
      //Callback registration success
      DEBUGOUT("\r\nCallbacks registered\r\n");

    } else {
      //Callback not registered
      DEBUGOUT("\r\nCallbacks not registered\r\n");
      break;
    }

    //Update source and destination address
    dma_transfer_t.src_addr  = (uint32_t *)gtx_temp_buff;
    dma_transfer_t.dest_addr = (uint32_t *)&CRC->CRC_DIN_FIFO;
    //Update Source and destination data increment
    dma_transfer_t.src_inc = SL_TRANSFER_SRC_INC_32;
    dma_transfer_t.dst_inc = SL_TRANSFER_DST_INC_NONE;
    //Update the single data transfer size
    dma_transfer_t.xfer_size = SL_TRANSFER_SIZE_32;
    //Update total DMA transfer size
    dma_transfer_t.transfer_count = BUFFSIZE;
    //Update DMA transfer type and mode
    dma_transfer_t.dma_mode      = SL_DMA_BASIC_MODE;
    dma_transfer_t.transfer_type = SL_DMA_MEMORY_TO_MEMORY;
    //Perform DMA transfer using generic dma transfer API

  } while (0);
}

/**
 * @brief Performs CRC calculation example process.
 *
 * This function performs the CRC calculation example process.
 */
void crc_example_process_action(void)
{
  uint32_t index, reg_status, status;
  static uint32_t counter_for_delay = 0;

  // counter for adding delay to slowdown repetitive operation
  counter_for_delay++;
  if (counter_for_delay % 100000) {
    return;
  }
  DEBUGOUT("\n******* CRC EXAMPLE START *********\n");

  /* Software CRC calculation */
  gsw_crc = sw_crc_compute(BUFF_LEN_8BIT_TYPE);
  DEBUGOUT("\r\n Input Data Buffer:\n {\n\t");
  for (index = 0; index < BUFFSIZE; index++) {
    gtx_temp_buff[index] = crc_convert_tx_buffer(gcrc_tx_Buf[index]);
    DEBUGOUT("0x%08lX ", gtx_temp_buff[index]);

    if (!((index + 1) % 5))
      DEBUGOUT("\n\t");
  }
  DEBUGOUT("\n }\n");

  /* CRC_32 polynomial */
  crc_params.polynomial = SL_CRC_POLYNOMIAL;
  crc_params.polyWidth  = SL_CRC_POLY_WIDTH;
  crc_params.lfsrVal    = SL_CRC_LFSR_VAL;
  crc_params.widthType  = SL_CRC_WIDTH_TYPE;
  crc_params.dinWidth   = SL_CRC_DIN_WIDTH;
  crc_params.numBytes   = SL_CRC_NUM_BYTES;
  crc_params.aempty     = SL_CRC_AEMPTY;
  crc_params.afull      = SL_CRC_AFULL;
  crc_params.swapDin    = SL_CRC_SWAP_DIN;
  crc_params.useUdma    = SL_CRC_USE_UDMA;
  crc_params.swapLfsr   = SL_CRC_SWAP_LFSR;

  /* Clears the FIFO and settles all the state machines to their idle. */

  do {
    status = sl_si91x_crc_set_gen_control();
    if (status == SL_STATUS_OK) {
      DEBUGOUT("\r\n Clearing the FIFO completed\r\n");
    } else {
      DEBUGOUT("\r\n Clearing the FIFO is Failed\r\n");
      break;
    }

    /* Enable the peripheral clocks for SET1 register */
    status = sl_si91x_crc_enable();
    if (!status) {
      DEBUGOUT("\r\n CRC enabled successfully\r\n");
    } else {
      DEBUGOUT("\r\n CRC enabled failed\r\n");
    }

    /* Load the polynomial value into the polynomial reg */
    sl_si91x_crc_polynomial(&crc_params);
    if (status == SL_STATUS_OK) {
      DEBUGOUT("\r\n Polynomial Update successful\r\n");
    } else {
      DEBUGOUT("\r\n Polynomial Update failed\r\n");
      break;
    }

    DEBUGOUT("\n CRC polynomial is : 0x%08lX \n", (unsigned long)crc_params.polynomial);

    /*  Load the width of the polynomial*/
    status = sl_si91x_crc_polynomial_width(&crc_params, &reg_status);
    if (status == SL_STATUS_OK) {
      DEBUGOUT("\r\n Polynomial width Update successful\r\n");
    } else {
      DEBUGOUT("\r\n Polynomial width Update failed\r\n");
      break;
    }
    /* Initialize the  LFSR Value */
    status = sl_si91x_crc_lfsr_init(&crc_params);
    if (status == SL_STATUS_OK) {
      DEBUGOUT("\r\n LFSR Value Update successful\r\n");
    } else {
      DEBUGOUT("\r\n LFSR Value Update failed\r\n");
      break;
    }
    /* swapped init value. If this is set bit swapped*/
    status = sl_si91x_crc_use_swapped_init(&crc_params, &reg_status);
    if (status == SL_STATUS_OK) {
      DEBUGOUT("\r\n Swapped init value Update successful\r\n");
    } else {
      DEBUGOUT("\r\n Swapped init value Update failed\r\n");
      break;
    }

    /* Set and control the data width types */
    status = sl_si91x_crc_set_data_width_type(&crc_params, &reg_status);
    if (status == SL_STATUS_OK) {
      DEBUGOUT("\r\n Data width type Update successful\r\n");
    } else {
      DEBUGOUT("\r\n Data width type Update failed\r\n");
    }

    /*  Set the FIFO Threshold Levels  */
    status = sl_si91x_crc_set_fifo_thresholds(&crc_params, &reg_status);
    if (!status) {
      DEBUGOUT("\r\n FIFO Threshold Level Update successful\r\n");
    } else {
      DEBUGOUT("\r\n FIFO Threshold Level Update failed\r\n");
    }

    dma_transfer_t.src_addr = (uint32_t *)gtx_temp_buff;
    status                  = sl_si91x_dma_transfer(SL_DMA_INSTANCE, gchannel, &dma_transfer_t);

    if (status == SL_STATUS_OK) {
      //Transfer started successfully
      DEBUGOUT("\r\n DMA to CRC Buffer data transfer started\r\n");

    } else {
      //Transfer start failed
      DEBUGOUT("\r\n Transfer start fail\r\n");
      break;
    }
    while (!dma_transfer_done)
      ;
    DEBUGOUT("\n Data transfer Completed\n");

    /* Monitor the CRC Calculation status and the returns the CRC Value */
    status = sl_si91x_crc_monitor_crc_calc(&crc_params, &ghw_crc);
    if (status == SL_STATUS_OK) {
      DEBUGOUT("\n CRC Calculation completed successfully\n");
    } else {
      DEBUGOUT("\n CRC Calculation Un-successful\n");
      break;
    }

    DEBUGOUT(" hw_crc is 0x%08lX and \n sw_crc is 0x%08lX", (unsigned long)ghw_crc, (unsigned long)gsw_crc);
    if (ghw_crc == gsw_crc) {
      /*  Prints on hyper-terminal  */
      DEBUGOUT("\n Both Matched and CRC TEST PASSED \n");
      /*  Prints on hyper-terminal  */

    } else {
      /*  Prints on hyper-terminal as */
      DEBUGOUT("\n CRC TEST FAILED \n");
    }
    status = sl_si91x_crc_disable();
    if (status == SL_STATUS_OK) {
      DEBUGOUT("\n CRC Disabled successfully\n");
    } else {
      DEBUGOUT("\n CRC Disable failed\n");
      break;
    }
    DEBUGOUT("\n******* CRC EXAMPLE END *********\n");
  } while (false);
}
