/***************************************************************************/ /**
* @file ulp_dma_example.c
* @brief Ulp DMA Example
*******************************************************************************
* # License
* <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* The licenser of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/
/*==================================================================================
 * @brief : This file contains example application for memory to memory transfer of DMA
 * using sl_si91x DMA driver.
 * This example uses both simple transfer and generic transfer API for transferring.
 * User can transfer different sizes of data by changing DMA transfer size in bytes
 * in UC
 * This example transfers data from src0 to dst0 buffer
 ===================================================================================*/
#include "sl_common.h"
#include "sl_si91x_dma.h"
#include "rsi_board.h"
#include "sl_si91x_dma_inst_config.h"
#include "ulp_dma_example.h"
/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/

/*******************************************************************************
 **********************  Local variables   ***************************
 ******************************************************************************/
volatile uint32_t transfer_done = 0;         // Transfer done flag
uint32_t src0[SL_DMA_TRANSFER_SIZE];         // source buffer
uint32_t dst0[SL_DMA_TRANSFER_SIZE] = { 0 }; // destination buffer

#define RESERVED_IRQ_COUNT   16                                   // Reserved IRQ count
#define EXT_IRQ_COUNT        98                                   // External IRQ count
#define VECTOR_TABLE_ENTRIES (RESERVED_IRQ_COUNT + EXT_IRQ_COUNT) // Vector table entries

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/

/*******************************************************************************
 * Transfer callback function.
 ******************************************************************************/
void transfer_complete_callback_dmadrv(uint32_t channel, void *data);

/*******************************************************************************
 **********************  Local Function definitions   ***************************
 ******************************************************************************/

/*******************************************************************************
 * Transfer callback function.
 ******************************************************************************/
void transfer_complete_callback_dmadrv(uint32_t channel, void *data)
{

  (void)channel;
  (void)data;
  transfer_done = 1;
}
uint32_t ramVector[VECTOR_TABLE_ENTRIES] __attribute__((aligned(256)));
extern void hardware_setup(void);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/*******************************************************************************
 * DMA example initialization function
 * Configures DMA for basic memory to memory transfer
 ******************************************************************************/
void dma_example_init(void)
{
  /* At this stage the micro-controller clock setting is already configured,
   * this is done through SystemInit() function which is called from startup
   * file (startup_rs1xxxx.s) before to branch to application main.
   * To reconfigure the default setting of SystemInit() function, refer to
   * startup_rs1xxxx.c file
   */
  // copying the vector table from flash to ram
  memcpy(ramVector, (uint32_t *)SCB->VTOR, sizeof(uint32_t) * VECTOR_TABLE_ENTRIES);
  // Assigning the ram vector address to VTOR register
  SCB->VTOR = (uint32_t)ramVector;
  // Switching MCU from PS4 to PS2 state(low power state)
  // In this mode, whatever be the timer clock source value, it will run with
  // 20MHZ only, as it trims higher clock frequencies to 20MHZ. To use timer in
  // high power mode, don't call hardware_setup()
  hardware_setup();
  DEBUGINIT();

  sl_status_t status = SL_STATUS_OK;
  uint32_t channel   = SL_DMA_CHANNEL;
  sl_dma_callback_t callbacks;
  sl_dma_init_t dma_init = { SL_DMA_INSTANCE };

  do {
    //Initialize UDMA peripheral
    status = sl_si91x_dma_init(&dma_init);
    if (status) {
      //UDMA initialization fail
      DEBUGOUT("\r\nFailed to Initialize UDMA\r\n");
      break;
    } else {
      //UDMA initialization success
      DEBUGOUT("\r\nUDMA Initialization Success\r\n");
    }
    //Allocate channel for transfer
    status = sl_si91x_dma_allocate_channel(SL_DMA_INSTANCE, &channel, 0);
    if (status) {
      //Channel allocation failed
      DEBUGOUT("\r\nChannel not allocated\r\n");
      break;
    } else {
      //Channel successfully allocated
      DEBUGOUT("\r\nChannel Allocated successfully\r\n");
    }
    callbacks.transfer_complete_cb = transfer_complete_callback_dmadrv;

    //Register transfer complete callback
    status = sl_si91x_dma_register_callbacks(SL_DMA_INSTANCE, channel, &callbacks);
    if (status) {
      //Callback registration success
      DEBUGOUT("\r\nCallbacks not registered\r\n");
      break;
    } else {
      //Callback not registered
      DEBUGOUT("\r\nCallbacks registered\r\n");
    }

    // Filled data in source buffer
    for (int i = 0; i < SL_DMA_TRANSFER_SIZE; i++) {
      src0[i] = (uint32_t)(i + 1);
    }
#if SL_DMA_SIMPLE_TRANSFER
    //Perform DMA transfer using simple dma transfer API
    status = sl_si91x_dma_simple_transfer(SL_DMA_INSTANCE, channel, src0, dst0, SL_DMA_TRANSFER_SIZE);
#else
    sl_dma_xfer_t dma_transfer_t = { 0 };
    //Update source and destination address
    dma_transfer_t.src_addr  = src0;
    dma_transfer_t.dest_addr = dst0;
    //Update Source and destination data increment
    dma_transfer_t.src_inc = SL_TRANSFER_SRC_INC_32;
    dma_transfer_t.dst_inc = SL_TRANSFER_DST_INC_32;
    //Update the single data transfer size
    dma_transfer_t.xfer_size = SL_TRANSFER_SIZE_32;
    //Update total DMA transfer size
    dma_transfer_t.transfer_count = SL_DMA_TRANSFER_SIZE;
    //Update DMA transfer type and mode
    dma_transfer_t.dma_mode      = SL_DMA_BASIC_MODE;
    dma_transfer_t.transfer_type = SL_DMA_MEMORY_TO_MEMORY;
    //Perform DMA transfer using generic dma transfer API
    status = sl_si91x_dma_transfer(SL_DMA_INSTANCE, channel, &dma_transfer_t);
#endif
    if (status) {
      //Transfer start failed
      DEBUGOUT("\r\nTransfer start fail\r\n");
    } else {
      //Transfer started successfully
      DEBUGOUT("\r\n Xfer start\r\n");
    }
  } while (0);
}

/*******************************************************************************
 * Function will run continuously and will wait for trigger
 ******************************************************************************/
void dma_example_process_action(void)
{
  if (transfer_done) {
    //DMA transfer done
    DEBUGOUT("\r\nTransfer completed successfully\r\n");
    //De-initialize DMA peripheral
    if (sl_si91x_dma_deinit(SL_DMA_INSTANCE)) {
      DEBUGOUT("\r\nFailed to Uninitialize UDMA\r\n");
    } else {
      DEBUGOUT("\r\nUDMA Uninitialization Success\r\n");
    }
    transfer_done = 0;
  }
}
