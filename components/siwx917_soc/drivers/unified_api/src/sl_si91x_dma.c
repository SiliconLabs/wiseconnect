/***************************************************************************/ /**
 * @file sl_si91x_dma.c
 * @brief DMA API  implemAPIentation
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_si91x_dma.h"
#include "sl_status.h"
#include "rsi_rom_udma_wrapper.h"
#include "rsi_udma.h"
#include "RS1xxxx_9117.h"
#include "rsi_board.h"

/*******************************************************************************
 ***************************  DEFINES / MACROS   ********************************
 ******************************************************************************/

#define DMA0_IRQ_HANDLER           IRQ033_Handler // IRQ handler for UDMA0
#define DMA1_IRQ_HANDLER           IRQ010_Handler // IRQ handler for UDMA1
#define DMA_MAX_TRANSFER_COUNT     1024           // Maximum transfer count of DMA in one cycle
#define DMA_CHANNEL_32             32             // Channel no 32
#define ERROR_CALLBACK             2              // Error callback bit position
#define TRANSFER_COMPLETE_CALLBACK 1              // Transfer complete callback bit position
#define DMA_INSTANCE0              0              // DMA0 Instance
#define DMA_INSTANCE1              1              // DMA1 Instance
#define TOTAL_DMA_INSTANCES        2              // Total number of DMA instances
#define DMA_DRIVER_ROM_BUFF_SIZE   30             // Size of dma_driver_rom_buff0
#define EXTRACT_LSB                1

/*******************************************************************************
 ***************************  EXTERN VARIABLES  ********************************
 ******************************************************************************/
extern RSI_UDMA_DESC_T __attribute__((section(".udma_addr0")))
UDMA0_Table[CONTROL_STRUCT0]; // UDMA0 DMA descriptors SRAM base address
extern RSI_UDMA_DESC_T __attribute__((section(".udma_addr1")))
UDMA1_Table[CONTROL_STRUCT1];                             // UDMA1 DMA descriptors SRAM base address
extern UDMA_RESOURCES UDMA0_Resources;                    // UDMA0 resources(address, irq no and DMA descriptor)
extern UDMA_RESOURCES UDMA1_Resources;                    // UDMA1 resources(address, irq no and DMA descriptor)
extern UDMA_Channel_Info udma0_chnl_info[DMA_CHANNEL_32]; // UDMA0 channels information
extern UDMA_Channel_Info udma1_chnl_info[DMA_CHANNEL_32]; // UDMA1 channels information
/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
RSI_UDMA_HANDLE_T
udmaHandle[TOTAL_DMA_INSTANCES]; // Array stores driver and peripheral context fo UDMA peripheral
uint32_t dma_driver_rom_buff0[DMA_DRIVER_ROM_BUFF_SIZE]; // Pointer to memory area used to UDMA0 driver context
uint32_t dma_driver_rom_buff1[DMA_DRIVER_ROM_BUFF_SIZE]; // Pointer to memory area used to UDMA1 driver context
uint32_t *dma_rom_buff[TOTAL_DMA_INSTANCES] = {
  dma_driver_rom_buff0,
  dma_driver_rom_buff1
}; // Array stores addresses of UDMA0 & UDMA1 dma_driver_rom_buff
sl_channel_data_t sl_channel_allocation_data_t[TOTAL_DMA_INSTANCES][SL_CHANNEL_COUNT] = { 0 }; // DMA channel allocator
UDMA_RESOURCES *UDMA_driver_resources[TOTAL_DMA_INSTANCES]                            = {
                             &UDMA0_Resources,
                             &UDMA1_Resources
}; // Array stores addresses of UDMA0 & UDMA1 driver resources
UDMA_Channel_Info *udma_driver_channel_info[TOTAL_DMA_INSTANCES] = {
  udma0_chnl_info,
  udma1_chnl_info
}; // Array stores addresses of UDMA0 & UDMA1 driver channel resources
RSI_UDMA_DESC_T *udma_driver_table[TOTAL_DMA_INSTANCES] = {
  UDMA0_Table,
  UDMA1_Table
}; // Array stores addresses of UDMA0 & UDMA1 descriptors

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/
/***************************************************************************/ /**
 * Return DMA channel status
 *
 * @param[in] dma_number - dma instance
 * @param[in] channel -    dma channel number
 *
 * @return true - DMA channel busy
 *         false - DMA channel idle
 ******************************************************************************/
static boolean_t channel_status(uint32_t dma_number, uint32_t channel);
/***************************************************************************/ /**
 * Process dma interrupt handler.
 *
 * @param[in] dma_number - dma instance
 * @param[in] channel -    dma channel number
 * @param[in] irq_type -    irq type,
 *                          SL_DMA_TRANSFER_DONE_CB - transfer complete interrupt
 *                          SL_DMA_ERROR_CB - channel error callback
 *
 * @return none
 ******************************************************************************/
static void process_dma_irq(uint32_t dma_number, uint32_t channel, uint32_t irq_type);

/*******************************************************************************
 **********************  Local Function Definition****************************
 ******************************************************************************/

/*******************************************************************************
 * This function return DMA channel status
 * true - DMA channel busy
 * false - DMA channel idle
 ******************************************************************************/
static boolean_t channel_status(uint32_t dma_number, uint32_t channel)
{
  return ((UDMA_driver_resources[dma_number]->reg->CHANNEL_STATUS_REG >> channel) & EXTRACT_LSB);
}

/*******************************************************************************
 * This function process DMA IRQ.
 * If DMA transfer size is greater than DMA transfer count (1024) then multiple DMA requests are
 * triggered by updating DMA descriptors.
 * This function calls transfer and error callback functions if registered.
 ******************************************************************************/
static void process_dma_irq(uint32_t dma_number, uint32_t channel, uint32_t irq_type)
{

  uint32_t remaining_data_size;
  uint32_t dma_count;
  uint32_t src_inc = 0;
  uint32_t dst_inc = 0;
  // Obtain the DMA channel info
  volatile UDMA_Channel_Info *channel_info =
    (UDMA_Channel_Info *)((UDMA_Channel_Info *)(udma_driver_channel_info[dma_number]) + channel);
  // Obtain DMA descriptor info
  RSI_UDMA_DESC_T *udma_table = (RSI_UDMA_DESC_T *)((RSI_UDMA_DESC_T *)(udma_driver_table[dma_number]) + channel);
  uint32_t size;
  uint32_t count;

  if (irq_type == SL_DMA_TRANSFER_DONE_CB) {
    // Interrupt is triggered by transfer complete
    if (channel_info->Cnt < channel_info->Size) {
      // Transfer size is greater than max DMA transfer count

      // Obtain remaining transfer data size
      remaining_data_size = channel_info->Size - channel_info->Cnt;
      // Update next transfer count
      if (remaining_data_size >= DMA_MAX_TRANSFER_COUNT) {
        remaining_data_size = DMA_MAX_TRANSFER_COUNT;
        dma_count           = DMA_MAX_TRANSFER_COUNT - 1;
      }
      if ((remaining_data_size > 0) && (remaining_data_size < DMA_MAX_TRANSFER_COUNT)) {
        dma_count = remaining_data_size - 1;
      }
      channel_info->Cnt += remaining_data_size;
      // Update source and dest increment for next transfer
      src_inc = udma_table->vsUDMAChaConfigData1.srcInc;
      dst_inc = udma_table->vsUDMAChaConfigData1.dstInc;
      // Update src and dest end address for next transfer
      udma_table->pSrcEndAddr = (void *)((uint32_t)udma_table->pSrcEndAddr + (remaining_data_size << src_inc));
      udma_table->pDstEndAddr = (void *)((uint32_t)udma_table->pDstEndAddr + (remaining_data_size << dst_inc));

      // Update transfer type and transfer count
      udma_table->vsUDMAChaConfigData1.transferType = sl_channel_allocation_data_t[dma_number][channel].transfer_mode;
      udma_table->vsUDMAChaConfigData1.totalNumOfDMATrans = dma_count;

      // Enable channel for next transfer
      UDMA_driver_resources[dma_number]->reg->CHNL_ENABLE_SET |= SET_BIT(channel);
      if (sl_channel_allocation_data_t[dma_number][channel].transfer_type == SL_DMA_MEMORY_TO_MEMORY) {
        // for memory to memory request, trigger SW request
        UDMA_driver_resources[dma_number]->reg->CHNL_SW_REQUEST |= SET_BIT(channel);
      }
    } else {
      if (sl_channel_allocation_data_t[dma_number][channel].dma_callback_t->transfer_complete_cb != NULL) {
        // Transfer complete callback
        sl_channel_allocation_data_t[dma_number][channel].dma_callback_t->transfer_complete_cb(channel, NULL);
      }
    }
  }
  if (irq_type == SL_DMA_ERROR_CB) {
    if (sl_channel_allocation_data_t[dma_number][channel].dma_callback_t->error_cb != NULL) {
      // error complete callback
      sl_channel_allocation_data_t[dma_number][channel].dma_callback_t->error_cb(channel, NULL);
    }
  }
}

/*******************************************************************************
***********************  Global function Definitions *************************
 ******************************************************************************/

/*******************************************************************************
* This function should be called only once for initializing UDMA0/UDMA1 peripheral
* before starting any channel transfer
 *******************************************************************************/
sl_status_t sl_si91x_dma_init(sl_dma_init_t *dma_init)
{
  sl_status_t status;

  // Initialize dma peripheral
  udmaHandle[dma_init->dma_number] = UDMAx_Initialize(UDMA_driver_resources[dma_init->dma_number],
                                                      udma_driver_table[dma_init->dma_number],
                                                      (RSI_UDMA_HANDLE_T)udmaHandle[dma_init->dma_number],
                                                      dma_rom_buff[dma_init->dma_number]);

  if (udmaHandle[dma_init->dma_number] == NULL) {
    // DMA init fail
    status = SL_STATUS_NOT_INITIALIZED;
  } else {
    //DMA init pass
    status = SL_STATUS_OK;
  }
  return status;
}

/*******************************************************************************
 * This function de-initializes DMA peripheral
  *******************************************************************************/
sl_status_t sl_si91x_dma_deinit(uint32_t dma_number)
{

  sl_status_t status = SL_STATUS_OK;

  if (udmaHandle[dma_number] != NULL) {
    for (int ch = 0; ch < DMA_CHANNEL_32; ch++) {
      if (channel_status(dma_number, ch)) {
        // DMA channel is busy
        status = SL_STATUS_BUSY;
        break;
      }
    }
    if (status == SL_STATUS_OK) {
      // Uninitialize DMA
      status = (sl_status_t)UDMAx_Uninitialize(UDMA_driver_resources[dma_number]);
    }
  } else {
    // DMA not initialized
    status = SL_STATUS_NOT_INITIALIZED;
  }
  return status;
}

/*******************************************************************************
* This Allocates channel for DMA transfer
* If channel no is 0, available channel is allocated
* If channel no is between 1 - 32, channel availability is chacked and allocated
* accordingly
 *******************************************************************************/
sl_status_t sl_si91x_dma_allocate_channel(uint32_t dma_number, uint32_t *channel_no, uint32_t priority)
{

  sl_status_t status = SL_STATUS_OK;

  do {
    if (udmaHandle[dma_number] == NULL) {
      // DMA not initialized
      status = SL_STATUS_NOT_INITIALIZED;
      break;
    }

    if ((*channel_no > DMA_CHANNEL_32) || (channel_no == NULL)) {
      // Invalid channel number
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }

    if (*channel_no == 0) {
      // Scan for available channels
      for (uint32_t ch = 0; ch < DMA_CHANNEL_32; ch++) {
        if (!sl_channel_allocation_data_t[dma_number][ch].allocated) {
          // Channel available
          *channel_no = ch + 1;
          // Allocate the channel using channel allocator
          sl_channel_allocation_data_t[dma_number][ch].allocated                            = true;
          sl_channel_allocation_data_t[dma_number][ch].priority                             = priority;
          sl_channel_allocation_data_t[dma_number][ch].dma_callback_t->transfer_complete_cb = NULL;
          sl_channel_allocation_data_t[dma_number][ch].dma_callback_t->error_cb             = NULL;
          break;
        }
      }
      if (*channel_no > DMA_CHANNEL_32) {
        // No DMA channel is available
        status = SL_STATUS_DMA_NO_CHANNEL_AVAILABLE;
      }
    } else {
      if (!sl_channel_allocation_data_t[dma_number][*channel_no - 1].allocated) {
        // Allocate desired DMA channel, if it is available
        sl_channel_allocation_data_t[dma_number][*channel_no - 1].priority                             = priority;
        sl_channel_allocation_data_t[dma_number][*channel_no - 1].allocated                            = true;
        sl_channel_allocation_data_t[dma_number][*channel_no - 1].dma_callback_t->transfer_complete_cb = NULL;
        sl_channel_allocation_data_t[dma_number][*channel_no - 1].dma_callback_t->error_cb             = NULL;
      } else {
        // Desired DMA channel is already allocated
        status = SL_STATUS_DMA_CHANNEL_ALLOCATED;
      }
    }
  } while (0);

  return status;
}

/*******************************************************************************
 * This function De-allocates channel for DMA transfer
 * Clears the channel allocator data and makes available for
 * other transfers
 * *****************************************************************************/
sl_status_t sl_si91x_dma_deallocate_channel(uint32_t dma_number, uint32_t channel_no)
{

  sl_status_t status = SL_STATUS_OK;

  do {
    if (udmaHandle[dma_number] == NULL) {
      // DMA not initialized
      status = SL_STATUS_NOT_INITIALIZED;
      break;
    }
    if ((channel_no > DMA_CHANNEL_32) && (channel_no == 0)) {
      // Invalid channel number
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    if (channel_status(dma_number, channel_no)) {
      // Transfer is in progress
      status = SL_STATUS_BUSY;
      break;
    }
    if (sl_channel_allocation_data_t[dma_number][channel_no - 1].allocated) {
      // Clear channel allocator data
      sl_channel_allocation_data_t[dma_number][channel_no - 1].allocated                            = false;
      sl_channel_allocation_data_t[dma_number][channel_no - 1].dma_callback_t->transfer_complete_cb = NULL;
      sl_channel_allocation_data_t[dma_number][channel_no - 1].dma_callback_t->error_cb             = NULL;
    } else {
      // DMA channel is already unallocated
      status = SL_STATUS_DMA_CHANNEL_ALREADY_UNALLOCATED;
    }
  } while (0);

  return status;
}

/*******************************************************************************
 * This function registers DMA callbacks
 * sl_dma_transfer_complete called when transfer is completed
 * sl_dma_error is called when there is bus error while channel transfer
 * *****************************************************************************/
sl_status_t sl_si91x_dma_register_callbacks(uint32_t dma_number, uint32_t channel_no, sl_dma_callback_t *callback_t)
{

  sl_status_t status = SL_STATUS_OK;

  do {
    if (udmaHandle[dma_number] == NULL) {
      // DMA not initialized
      status = SL_STATUS_NOT_INITIALIZED;
      break;
    }
    if ((channel_no > DMA_CHANNEL_32) && (channel_no == 0)) {
      // Invalid channel number
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // Register callbacks
    sl_channel_allocation_data_t[dma_number][channel_no - 1].dma_callback_t = callback_t;
  } while (0);

  return status;
}

/*******************************************************************************
 * This function unregisters DMA callbacks
 * *****************************************************************************/
sl_status_t sl_si91x_dma_unregister_callbacks(uint32_t dma_number, uint32_t channel_no, uint8_t callback_type)
{

  sl_status_t status = SL_STATUS_OK;

  do {
    if (udmaHandle[dma_number] == NULL) {
      // DMA not initialized
      status = SL_STATUS_NOT_INITIALIZED;
      break;
    }
    if ((channel_no > DMA_CHANNEL_32) && (channel_no == 0)) {
      // Invalid channel number
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    if (callback_type & TRANSFER_COMPLETE_CALLBACK) {
      // unregister transfer complete callback
      sl_channel_allocation_data_t[dma_number][channel_no - 1].dma_callback_t->transfer_complete_cb = NULL;
    } else if (callback_type & ERROR_CALLBACK) {
      // unregister error callback
      sl_channel_allocation_data_t[dma_number][channel_no - 1].dma_callback_t->error_cb = NULL;
    } else {
      // Invalid callback type
      status = SL_STATUS_INVALID_PARAMETER;
    }
  } while (0);

  return status;
}

/*******************************************************************************
 * This function perform simple memory to memory DMA transfer.
 * User needs to allocate channel before initializing transfer using this function
 * User no need to update any DMA descriptors if using this function
 * *****************************************************************************/
sl_status_t sl_si91x_dma_simple_transfer(uint32_t dma_number,
                                         uint32_t channel_no,
                                         void *src_addr,
                                         void *dst_addr,
                                         uint32_t data_size)
{

  sl_status_t status = SL_STATUS_OK;
  uint32_t channel   = channel_no - 1;

  do {
    if (udmaHandle[dma_number] == NULL) {
      // DMA not initialized
      status = SL_STATUS_NOT_INITIALIZED;
      break;
    }
    if ((channel_no > DMA_CHANNEL_32) && (channel_no == 0)) {
      // Invalid channel number
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    if (sl_channel_allocation_data_t[dma_number][channel].allocated) {
      // Channel allocated
      RSI_UDMA_CHA_CONFIG_DATA_T control;
      RSI_UDMA_CHA_CFG_T config;

      // Update channel configuration structure with basic parameters
      config.altStruct       = ALTERNATE_DESCRIPTOR_DISABLE;
      config.burstReq        = BURST_REQUEST_ENABLE;
      config.channelPrioHigh = CHANNEL_PRIO_DISABLE;
      config.dmaCh           = (channel);
      config.periAck         = PERIPHERAL_ACK_DISABLE;
      config.periphReq       = PERIPHERAL_REQUEST_DISABLE;
      config.reqMask         = REQUEST_MASK_DISABLE;
      control.transferType   = SL_DMA_BASIC_MODE;
      control.nextBurst      = NEXT_BURST_DISABLE;
      if (data_size > (DMA_MAX_TRANSFER_COUNT - 1)) {
        // Transfer size is more than maximum transfer size in one DMA cycle
        control.totalNumOfDMATrans = (DMA_MAX_TRANSFER_COUNT - 1);
      } else {
        control.totalNumOfDMATrans = (data_size - 1);
      }
      // Update DMA descriptor
      control.rPower      = ARBSIZE_1024;
      control.srcProtCtrl = SOURCE_PROTECT_CONTROL_DISABLE;
      control.dstProtCtrl = DESTINATION_PROTECT_CONTROL_DISABLE;
      control.srcSize     = SRC_SIZE_32;
      control.srcInc      = SRC_INC_32;
      control.dstSize     = DST_SIZE_32;
      control.dstInc      = DST_INC_32;

      // Update DMA transfer type in channel allocator
      sl_channel_allocation_data_t[dma_number][channel].transfer_type = SL_DMA_MEMORY_TO_MEMORY;
      // Update DMA transfer mode in channel allocator
      sl_channel_allocation_data_t[dma_number][channel].transfer_mode = SL_DMA_BASIC_MODE;
      // Configure dma channel for transfer
      status = UDMAx_ChannelConfigure(UDMA_driver_resources[dma_number],
                                      (channel),
                                      (uint32_t)src_addr,
                                      (uint32_t)dst_addr,
                                      data_size,
                                      control,
                                      &config,
                                      NULL,
                                      udma_driver_channel_info[dma_number],
                                      udmaHandle[dma_number]);
      // Enable DMA channel
      status = UDMAx_ChannelEnable(channel, UDMA_driver_resources[dma_number], udmaHandle[dma_number]);
      // Enable DMA peripheral
      status = UDMAx_DMAEnable(UDMA_driver_resources[dma_number], udmaHandle[dma_number]);
      // Start transfer using software trigger
      status = RSI_UDMA_ChannelSoftwareTrigger(udmaHandle[dma_number], channel);
    } else {
      // Channel is unallocated
      status = SL_STATUS_DMA_CHANNEL_UNALLOCATED;
    }
  } while (0);

  return status;
}

/*******************************************************************************
 * This function DMA transfer.
 * User needs to allocate channel before initializing transfer using this function
 * User need to update sl_dma_xfer_t and pass it to function.
 * This function perform all types of available DMA transfers
 * *****************************************************************************/
sl_status_t sl_si91x_dma_transfer(uint32_t dma_number, uint32_t channel_no, sl_dma_xfer_t *dma_transfer_t)
{

  sl_status_t status = SL_STATUS_OK;
  uint32_t channel   = channel_no - 1;

  do {
    if (udmaHandle[dma_number] == NULL) {
      // DMA peripheral not initialized
      status = SL_STATUS_NOT_INITIALIZED;
      break;
    }
    if ((channel_no > DMA_CHANNEL_32) && (channel_no == 0)) {
      // Invalid channel number
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    if (sl_channel_allocation_data_t[dma_number][channel].allocated) {
      // Channel allocated
      RSI_UDMA_CHA_CONFIG_DATA_T control;
      RSI_UDMA_CHA_CFG_T config;

      // Upadte DMA mode
      if (dma_transfer_t->dma_mode == SL_DMA_BASIC_MODE) {
        config.altStruct = ALTERNATE_DESCRIPTOR_DISABLE;
      } else {
        config.altStruct = ALTERNATE_DESCRIPTOR_ENABLE;
      }
      // Upadte DMA priority
      if (sl_channel_allocation_data_t[dma_number][channel].priority) {
        config.channelPrioHigh = CHANNEL_PRIO_ENABLE;
      } else {
        config.channelPrioHigh = CHANNEL_PRIO_DISABLE;
      }
      if ((dma_transfer_t->transfer_type == SL_DMA_MEMORY_TO_PERIPHERAL)
          || (dma_transfer_t->transfer_type == SL_PERIPHERAL_TO_MEMORY)) {
        // Memory-Peripheral/Peripheral-Memory transfer
        config.periphReq = PERIPHERAL_REQUEST_ENABLE;
        control.rPower   = ARBSIZE_1;
      } else {
        // Memory-memory transfer
        config.periphReq = PERIPHERAL_REQUEST_DISABLE;
        control.rPower   = ARBSIZE_1024;
      }
      if ((dma_transfer_t->signal > 0) && (dma_transfer_t->signal < 8)) {
        // Update the peripheral ACK
        config.periAck = dma_transfer_t->signal;
      } else {
        config.periAck = PERIPHERAL_ACK_DISABLE;
      }
      config.dmaCh    = (channel);
      config.burstReq = BURST_REQUEST_ENABLE;
      config.reqMask  = REQUEST_MASK_DISABLE;
      // Memory-meory transfer
      if (dma_transfer_t->dma_mode == SL_DMA_BASIC_MODE) {
        control.transferType = SL_DMA_BASIC_MODE;
      } else {
        control.transferType = SL_DMA_PINGPONG_MODE;
      }

      control.nextBurst = NEXT_BURST_DISABLE;
      if (dma_transfer_t->transfer_count > (DMA_MAX_TRANSFER_COUNT - 1)) {
        // Transfer size is more than maximum transfer size in one DMA cycle
        control.totalNumOfDMATrans = (DMA_MAX_TRANSFER_COUNT - 1);
      } else {
        control.totalNumOfDMATrans = (dma_transfer_t->transfer_count - 1);
      }
      // Update DMA descriptor
      control.srcProtCtrl = SOURCE_PROTECT_CONTROL_DISABLE;
      control.dstProtCtrl = DESTINATION_PROTECT_CONTROL_DISABLE;
      control.srcSize     = dma_transfer_t->xfer_size;
      control.srcInc      = dma_transfer_t->src_inc;
      control.dstSize     = dma_transfer_t->xfer_size;
      control.dstInc      = dma_transfer_t->dst_inc;

      // Update DMA transfer type in channel allocator
      sl_channel_allocation_data_t[dma_number][channel].transfer_type = dma_transfer_t->transfer_type;
      // Update DMA transfer mode in channel allocator
      sl_channel_allocation_data_t[dma_number][channel].transfer_mode = control.transferType;
      // Configure dma channel for transfer
      status = UDMAx_ChannelConfigure(UDMA_driver_resources[dma_number],
                                      (channel),
                                      (uint32_t)dma_transfer_t->src_addr,
                                      (uint32_t)dma_transfer_t->dest_addr,
                                      dma_transfer_t->transfer_count,
                                      control,
                                      &config,
                                      NULL,
                                      udma_driver_channel_info[dma_number],
                                      udmaHandle[dma_number]);
      // Enable DMA channel
      status = UDMAx_ChannelEnable(channel, UDMA_driver_resources[dma_number], udmaHandle[dma_number]);
      // Enable DMA peripheral
      status = UDMAx_DMAEnable(UDMA_driver_resources[dma_number], udmaHandle[dma_number]);
      if (dma_transfer_t->transfer_type == SL_DMA_MEMORY_TO_MEMORY) {
        // Start transfer using software trigger
        status = RSI_UDMA_ChannelSoftwareTrigger(udmaHandle[dma_number], channel);
      }
    } else {
      // Channel is unallocated
      status = SL_STATUS_DMA_CHANNEL_UNALLOCATED;
    }
  } while (0);

  return status;
}

/*******************************************************************************
 * Stop on-going DMA transfer
 * *****************************************************************************/
sl_status_t sl_si91x_dma_stop_transfer(uint32_t dma_number, uint32_t channel_no)
{

  sl_status_t status = SL_STATUS_OK;

  do {
    if (udmaHandle[dma_number] == NULL) {
      // DMA not initialized
      status = SL_STATUS_NOT_INITIALIZED;
      break;
    }
    // Disable channel for stopping transfer
    if (RSI_UDMA_ChannelDisable(udmaHandle[dma_number], (channel_no - 1))) {
      // Invalid channel number
      status = SL_STATUS_INVALID_PARAMETER;
    }
  } while (0);

  return status;
}

/*******************************************************************************
 * Get the status of channel
 * SL_STATUS_BUSY - Transfer is in progress
 * SL_STATUS_DMA_CHANNEL_ALLOCATED - Channel is allocated but idle
 * *****************************************************************************/
sl_status_t sl_si91x_dma_channel_status_get(uint32_t dma_number, uint32_t channel_no)
{

  sl_status_t status = SL_STATUS_IDLE;

  do {
    if (udmaHandle[dma_number] == NULL) {
      // DMA not initialized
      status = SL_STATUS_NOT_INITIALIZED;
      break;
    }
    if ((channel_no > DMA_CHANNEL_32) && (channel_no == 0)) {
      // Invalid channel number
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    if (channel_status(dma_number, channel_no - 1)) {
      // DMA channel is busy (i.e) transfer in progress
      status = SL_STATUS_BUSY;
      break;
    }
    if (sl_channel_allocation_data_t[dma_number][channel_no - 1].allocated) {
      // Channel is allocated by idle
      status = SL_STATUS_DMA_CHANNEL_ALLOCATED;
    }
  } while (0);

  return status;
}
#ifdef SL_SI91X_DMA
/*******************************************************************************
 * Interrupt handler for UDMA0 peripheral.
 * This function clears the interrupts
 * If transfer size is greater than DMA max transfer size, process_dma_irq
 * will initiate the transfer again until all the bytes are transferred
 * *****************************************************************************/
void DMA0_IRQ_HANDLER(void)
{

  uint32_t channel;
  volatile uint32_t interrupt_status;
  volatile uint32_t error_status;
  uint32_t irq_type = 0;

  // Scan the channel which triggered interrupt
  for (channel = 0; channel < DMA_CHANNEL_32; channel++) {
    irq_type = 0;
    // Check for transfer complete interrupt
    interrupt_status = UDMA_driver_resources[0]->reg->UDMA_DONE_STATUS_REG;
    if (interrupt_status & (1 << channel)) {
      // Interrupt is due to transfer complete
      UDMA_driver_resources[0]->reg->UDMA_DONE_STATUS_REG |= SET_BIT(channel);
      irq_type = TRANSFER_COMPLETE_CALLBACK;
    } else {
      // Check for bus error interrupt
      error_status = UDMA_driver_resources[0]->reg->ERR_CLR;
      if (error_status & (1 << channel)) {
        // Interrupt is due to bus error
        UDMA_driver_resources[0]->reg->ERR_CLR &= CLR_BIT(channel);
        UDMA_driver_resources[0]->reg->UDMA_DONE_STATUS_REG |= SET_BIT(channel);
        irq_type = ERROR_CALLBACK;
      }
    }
    if (irq_type) {
      // Process the interrupt.
      process_dma_irq(DMA_INSTANCE0, channel, irq_type);
    }
  }
}

/*******************************************************************************
 * Interrupt handler for UDMA1 peripheral.
 * This function clears the interrupts
 * If transfer size is greater than DMA max transfer size, process_dma_irq
 * will initiate the transfer again until all the bytes are transferred
 * *****************************************************************************/
void DMA1_IRQ_HANDLER(void)
{

  uint32_t channel;
  volatile uint32_t interrupt_status;
  volatile uint32_t error_status;
  uint32_t irq_type = 0;

  // Scan the channel which triggered interrupt
  for (channel = 0; channel < DMA_CHANNEL_32; channel++) {
    irq_type = 0;
    // Check for transfer complete interrupt
    interrupt_status = UDMA_driver_resources[1]->reg->UDMA_DONE_STATUS_REG;
    if (interrupt_status & (1 << channel)) {
      // Interrupt is due to transfer complete
      UDMA_driver_resources[1]->reg->UDMA_DONE_STATUS_REG |= SET_BIT(channel);
      irq_type = TRANSFER_COMPLETE_CALLBACK;
    } else {
      // Check for bus error interrupt
      error_status = UDMA_driver_resources[1]->reg->ERR_CLR;
      if (error_status & (1 << channel)) {
        // Interrupt is due to bus error
        UDMA_driver_resources[1]->reg->ERR_CLR &= CLR_BIT(channel);
        UDMA_driver_resources[1]->reg->UDMA_DONE_STATUS_REG |= SET_BIT(channel);
        irq_type = ERROR_CALLBACK;
      }
    }
    if (irq_type) {
      // Process the interrupt.
      process_dma_irq(DMA_INSTANCE1, channel, irq_type);
    }
  }
}
#endif /* SL_SI91X_DMA */
