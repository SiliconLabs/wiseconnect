/***************************************************************************/ /**
 * @file sl_si91x_dma.h
 * @brief DMA API implementation
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

#ifndef SL_SI91X_DMA_
#define SL_SI91X_DMA_

#ifdef __cplusplus
extern "C" {
#endif

//// Includes

#include "sl_status.h"
#include "UDMA.h"
/***************************************************************************/ /**
* @addtogroup DMA Direct Memory Access
* @ingroup SI91X_PERIPHERAL_APIS
* @{
* 
******************************************************************************/
// -----------------------------------------------------------------------------
// Macros
#define SL_STATUS_DMA_CHANNEL_ALLOCATED \
  (sl_status_t)0x45 //// Status code indicates that DMA channel already allocated for another transfer
#define SL_STATUS_DMA_NO_CHANNEL_AVAILABLE \
  (sl_status_t)0x46 //// Status code indicates that no DMA channel is available for allocation
#define SL_STATUS_DMA_CHANNEL_ALREADY_UNALLOCATED \
  (sl_status_t)0X47 //// Status code indicates that DMA channel is already unallocated
#define SL_STATUS_DMA_CHANNEL_UNALLOCATED \
  (sl_status_t)0X48         //// Status code indicates that channel is not allocated for DMA transfer
#define SL_CHANNEL_COUNT 32 //// DMA channel count

#define ALTERNATE_DESCRIPTOR_DISABLE 0 // DMA doesn't use alternate descriptor
#define ALTERNATE_DESCRIPTOR_ENABLE  1 // DMA use alternate descriptor
#define BURST_REQUEST_ENABLE         1 // DMA burst request enable
#define BURST_REQUEST_DISABLE        0 // DMA burst request disable
#define CHANNEL_PRIO_DISABLE         0 // Channel has low priority
#define CHANNEL_PRIO_ENABLE          1 // Channel has high priority
#define PERIPHERAL_ACK_DISABLE       0 // Peripheral ACK is disabled
#define PERIPHERAL_REQUEST_DISABLE   0 // Peripheral Request is disabled
#define PERIPHERAL_REQUEST_ENABLE    1 // Peripheral Request is enabled
#define REQUEST_MASK_DISABLE         0 // DMA request mask disabled
#define NEXT_BURST_ENABLE \
  1 // Force the channel to only respond to burst requests at \
    // the tail end of a scatter-gather transfer
#define NEXT_BURST_DISABLE                  0
#define SOURCE_PROTECT_CONTROL_DISABLE      0x000 // Disable source protect control
#define DESTINATION_PROTECT_CONTROL_DISABLE 0x000 // Enable source protect control

// -----------------------------------------------------------------------------
// Data Types
/***************************************************************************/ /**
   *
   *
   */

/***************************************************************************/ /**
     * Typedef for user supplied callback function which is called when a DMA transfer completes
     *
     * @param[in]   channel_no      DMA channel number
     * @param[in]   *data           An extra parameter for user application

     ******************************************************************************/
typedef void (*sl_dma_transfer_complete)(uint32_t channel, void *data);

/***************************************************************************/ /**
     * Typedef for user supplied callback function which is called when a DMA error occurs
     *
     * @param[in]   channel_no      DMA channel number
     * @param[in]   *data           An extra parameter for user application

     ******************************************************************************/
typedef void (*sl_dma_error)(uint32_t channel, void *data);

/// @brief Enumeration holds transfer types of DMA
typedef enum {
  SL_DMA_MEMORY_TO_MEMORY,     ///< Memory to memory transfer
  SL_DMA_MEMORY_TO_PERIPHERAL, ///< Memory to peripheral transfer
  SL_PERIPHERAL_TO_MEMORY      ///< Peripheral to memory transfer
} sl_dma_transfer_type_t;

/// @brief Enumeration holds transfer modes of DMA
typedef enum {
  SL_DMA_BASIC_MODE    = UDMA_MODE_BASIC,   ///< Basic DMA mode
  SL_DMA_PINGPONG_MODE = UDMA_MODE_PINGPONG ///< Ping pong mode
} sl_dma_transfer_mode_t;

/// @brief Enumeration holds peripheral ACK signals to DMA
typedef enum {
  SL_USART0_ACK     = 0x01, ///< ACK code for USART0
  SL_UART1_ACK      = 0x02, ///< ACK code for UART1
  SL_UART3_ACK      = 0x03, ///< ACK code for UART3
  SL_SSI_SLAVE_ACK  = 0x04, ///< ACK code for SSI slave
  SL_SSI_MASTER_ACK = 0x05, ///< ACK code for SSI master
  SL_SSI1_SLAVE_ACK = 0x06, ///< ACK code for SSI1
  SL_I2C_ACK        = 0x07  ///< ACK code for I2C
} sl_dma_peripheral_ack_t;

/// @brief Enumeration holds DMA transfer sizes
typedef enum {
  SL_TRANSFER_SIZE_32 = SRC_SIZE_32, ///< 4 bytes transfer size
  SL_TRANSFER_SIZE_16 = SRC_SIZE_16, ///< 2 bytes transfer size
  SL_TRANSFER_SIZE_8  = SRC_SIZE_8   ///< 1 bytes transfer size
} sl_dma_transfer_size_t;

/// @brief Enumeration holds DMA transfer address increment for source and destination
typedef enum {
  SL_TRANSFER_SRC_INC_32   = SRC_INC_32,   ///< 4 bytes source address increment
  SL_TRANSFER_SRC_INC_16   = SRC_INC_16,   ///< 2 bytes source address increment
  SL_TRANSFER_SRC_INC_8    = SRC_INC_8,    ///< 1 byte source address increment
  SL_TRANSFER_SRC_INC_NONE = SRC_INC_NONE, ///< No source address increment
  SL_TRANSFER_DST_INC_32   = DST_INC_32,   ///< 4 bytes destination address increment
  SL_TRANSFER_DST_INC_16   = DST_INC_16,   ///< 2 bytes destination address increment
  SL_TRANSFER_DST_INC_8    = DST_INC_8,    ///< 1 byte destination address increment
  SL_TRANSFER_DST_INC_NONE = DST_INC_NONE  ///< No destination address increment
} sl_dma_transfer_inc_t;

/// @brief Enumeration holds 8-bit codes which are used by callback_type in
/// sl_si91x_dma_unregister_callbacks function
typedef enum {
  SL_DMA_TRANSFER_DONE_CB = 1, ///< 8-bit code for transfer complete callback
  SL_DMA_ERROR_CB         = 2  ///< 8-bit code for error callback
} sl_dma_callback_code_t;

/// @brief Structure holds Channel Callbacks
typedef struct {
  sl_dma_transfer_complete transfer_complete_cb; ///< data transfer complete callback
  sl_dma_error error_cb;                         ///< Error callback
} sl_dma_callback_t;

/// @brief Channel Initialization structure
typedef struct {
  uint32_t dma_number; ///< 0 - UDMA0, 1 - UDMA1
} sl_dma_init_t;

/// @brief channel allocation data
typedef struct {
  uint32_t priority;                 ///< Channel priority
  bool allocated;                    ///< Channel allocation status
  sl_dma_callback_t *dma_callback_t; ///< Channel callback structure
  uint32_t transfer_type;            ///< DMA Transfer type
  uint32_t transfer_mode;            ///< DMA transfer mode
} sl_channel_data_t;

/// @brief  DMA transfer structure
typedef struct {
  uint32_t *src_addr;      ///< Source transfer address
  uint32_t *dest_addr;     ///< Destination transfer address
  uint32_t src_inc;        ///< Source address increment size
  uint32_t dst_inc;        ///< Destination address increment size
  uint32_t xfer_size;      ///< Transfer data size
  uint32_t transfer_count; ///< Total transfer length
  uint8_t transfer_type;   ///< DMA transfer type
  uint8_t dma_mode;        ///< DMA transfer mode
  uint8_t signal;          ///< Peripheral signal which triggers DMA transfer (if 0, consider as software trigger)
} sl_dma_xfer_t;

extern sl_channel_data_t sl_channel_allocation_data_t[2][SL_CHANNEL_COUNT]; ///< DMA driver channel allocator

// -----------------------------------------------------------------------------
// Prototypes
/***************************************************************************/ /**
   *
   *
   */

/***************************************************************************/ /**
   * Initialize DMA peripheral by enabling DMA clock and clearing DMA interrupts.
   *
   * @param[in]  *dma_init   dma initialization structure,
   *              dma_init->dma_number - 0->UDMA0, 1->UDMA1
   *              dma_init->dma_channel_count - Number of channels used by DMA driver (0-32)
   * @return      Initialization status
                  SL_STATUS_OK - Initialization success
                  SL_STATUS_NOT_INITIALIZED - Initialization fail

   ******************************************************************************/

sl_status_t sl_si91x_dma_init(sl_dma_init_t *dma_init);

/***************************************************************************/ /**
   * Deinitialize the DMA module by disabling peripheral clock and interrupts, if there is no ongoing DMA transfer.
   *
   * @param[in]   dma_number  0->UDMA0, 1->UDMA1
   * @return      deinitialization status
                  SL_STATUS_OK - Deinit success
                  SL_STATUS_BUSY - Cannot deinit the peripheral due to an ongoing transfer
                  SL_STATUS_NOT_INITIALIZED - DMA peripheral not initialized

  ******************************************************************************/

sl_status_t sl_si91x_dma_deinit(uint32_t dma_number);

/***************************************************************************/ /**
   * Check the available DMA channel and allocate the channel. This function also sets the priority of allocated channel
   * and assign the channel number to *channel_no variable. If no channel is available, it will return SL_DMA_NO_CHANNEL_AVAILABLE.
   * note: Users can also initialize the desired channel number and this API checks whether the desired channel is available and allocates the
   * channel if available. To allocate the available channel, channel_no should be initialized to 0.
   *
   *@pre This function should be called after initializing DMA using sl_si91x_dma_init
   * @param[in] dma_number  dma_number  0->UDMA0, 1->UDMA1
   * @param[in] *channel_no   Address of channel number.
   * @param[in] priority      channel priority.
   * @return channel allocation status
             SL_STATUS_OK - Channel allocated
             SL_STATUS_DMA_NO_CHANNEL_AVAILABLE - All DMA channels are allocated
             SL_STATUS_DMA_CHANNEL_ALLOCATED - Desired channel is already allocated
           SL_STATUS_NOT_INITIALIZED - DMA peripheral not initialized
             SL_STATUS_INVALID_PARAMETER - Channel no is invalid
   ******************************************************************************/

sl_status_t sl_si91x_dma_allocate_channel(uint32_t dma_number, uint32_t *channel_no, uint32_t priority);

/***************************************************************************/ /**
   * Deallocate the DMA channel if there is no ongoing transfer on channel.
   *
   * @param[in] dma_number  dma_number  0->UDMA0, 1->UDMA1
   * @param[in] channel_no    channel number.

   * @return channel deallocation status
             SL_STATUS_OK - Channel deallocated
             SL_STATUS_BUSY - Cannot deallocate channel due to an ongoing transfer
           SL_STATUS_NOT_INITIALIZED - DMA peripheral not initialized
             SL_STATUS_INVALID_PARAMETER - Channel no is invalid
   ******************************************************************************/

sl_status_t sl_si91x_dma_deallocate_channel(uint32_t dma_number, uint32_t channel_no);

/***************************************************************************/ /**
   * Register DMA callbacks (transfer complete & error).
   * Users must update the sl_dma_callback_t structure and pass its address to this function.
   * Users can have separate callbacks for transfer complete and error for each channel.
   *
   *@pre This function should be called after sl_si91x_dma_init and sl_si91x_dma_allocate_channel
   * @param[in] dma_number  dma_number  0->UDMA0, 1->UDMA1
   * @param[in] channel_no    channel number.
   * @param[in] callback_t    structure containing callback functions.
   *
   * @return channel deallocation status
             SL_STATUS_OK - Callback registered successfully
             SL_STATUS_INVALID_PARAMETER - Invalid channel number
             SL_STATUS_NOT_INITIALIZED - DMA peripheral not initialized

   ******************************************************************************/

sl_status_t sl_si91x_dma_register_callbacks(uint32_t dma_number, uint32_t channel_no, sl_dma_callback_t *callback_t);

/***************************************************************************/ /**
   * Unregister DMA callbacks (transfer complete & error).
   * Update the 8-bit variable callback_type and pass to the function.
   * Each bit is mapped to a specific callback.
   *
   * @param[in] dma_number        dma_number  0->UDMA0, 1->UDMA1
   * @param[in] channel_no        channel number.
   * @param[in] callback_type     Unregister the DMA callbacks based on the callback type (bit mapped to callbacks),
                                  (SL_DMA_TRANSFER_DONE_CB)  - unregister transfer complete callback
                                  (SL_DMA_ERROR_CB) - unregister error callback
                                  (SL_DMA_TRANSFER_DONE_CB | SL_DMA_ERROR_CB)   - unregister both transfer complete and error callback
   * @return channel deallocation status
             SL_STATUS_OK - Callback unregistered successfully
             SL_STATUS_INVALID_PARAMETER - Invalid channel number
             SL_STATUS_NOT_INITIALIZED - DMA peripheral not initialized

  ******************************************************************************/

sl_status_t sl_si91x_dma_unregister_callbacks(uint32_t dma_number, uint32_t channel_no, uint8_t callback_type);

/***************************************************************************/ /**
   * Configure the DMA channel descriptor and initiate DMA transfer.
   * DMA primary descriptor is updated in this function and based on transfer mode
   * the alternate descriptor is also updated (only for ping pong mode). Additionally, other DMA parameters
   * such as peripheral ACK signal (for peripheral memory transfers), DMA priority etc. are
   * also updated.
   *
   *@pre This function should be called after sl_si91x_dma_init and sl_si91x_dma_allocate_channel
   * @param[in] dma_number  dma_number  0->UDMA0, 1->UDMA1
   * @param[in] channel_no        channel number.
   * @param[in] *dma_transfer_t      channel transfer data structure containing channel descriptor and other basic DMA parameters.

   * @return DMA transfer status
             SL_STATUS_OK  - Transfer started successfully
             SL_STATUS_SUSPENDED - Transfer initialization fail
             SL_STATUS_NOT_INITIALIZED - DMA peripheral not initialized
             SL_STATUS_INVALID_PARAMETER - Channel no is invalid
   ******************************************************************************/

sl_status_t sl_si91x_dma_transfer(uint32_t dma_number, uint32_t channel_no, sl_dma_xfer_t *dma_transfer_t);

/***************************************************************************/ /**
   * Configure the DMA channel descriptor and initiate simple memory to memory
   * DMA transfer. Pass the source address and destination address of transfer
   * along with transfer length in bytes.
   *
   *@pre This function should be called after sl_si91x_dma_init and sl_si91x_dma_allocate_channel
   *
   * @param[in] dma_number  dma_number  0->UDMA0, 1->UDMA1
   * @param[in] sl_channel_t  channel no
   * @param[in] *src_addr     source address.
   * @param[in] *dst_addr    destination address.
   * @param[in] data_size     transfer size in bytes

   * @return DMA transfer status
             SL_STATUS_OK  - Transfer success
             SL_STATUS_SUSPENDED - Transfer initialization fail
             SL_STATUS_NOT_INITIALIZED - DMA peripheral not initialized
             SL_STATUS_INVALID_PARAMETER - Channel no is invalid
   ******************************************************************************/

sl_status_t sl_si91x_dma_simple_transfer(uint32_t dma_number,
                                         uint32_t channel_no,
                                         void *src_addr,
                                         void *dst_addr,
                                         uint32_t data_size);

/***************************************************************************/ /**
   * Stop any active transfer on a channel. If there is no active transfer on the channel,
   * this function returns SL_DMA_CHANNEL_IDLE.
   *
   *@pre This function should be called after sl_si91x_dma_init and sl_si91x_dma_allocate_channel
   * @param[in] dma_number  dma_number  0->UDMA0, 1->UDMA1
   * @param[in] channel_no        channel number.

   * @return DMA transfer status
             SL_STATUS_OK         - Transfer stopped successfully
             SL_STATUS_IDLE       - There is no active transfer on channel
             SL_STATUS_INVALID_PARAMETER  - Invalid channel number
             SL_STATUS_NOT_INITIALIZED - DMA peripheral not initialized
   ******************************************************************************/

sl_status_t sl_si91x_dma_stop_transfer(uint32_t dma_number, uint32_t channel_no);

/***************************************************************************/ /**
   * Return the channel status i.e., whether the channel is allocated/busy/idle.
   *
   *@pre This function should be called after sl_si91x_dma_init
   * @param[in] dma_number  dma_number  0->UDMA0, 1->UDMA1
   * @param[in] channel_no    channel number.

   * @return  channel status
              SL_STATUS_IDLE - Channel is not allocated
              SL_STATUS_DMA_CHANNEL_ALREADY_ALLOCATED - Channel is already allocated and idle
              SL_STATUS_BUSY - Channel is allocated and busy
              SL_STATUS_NOT_INITIALIZED - DMA peripheral not initialized
              SL_STATUS_INVALID_PARAMETER - Channel no is invalid
   ******************************************************************************/

sl_status_t sl_si91x_dma_channel_status_get(uint32_t dma_number, uint32_t channel_no);

/** @} (end addtogroup DMA) */

#ifdef __cplusplus
}
#endif
/** @} (end addtogroup DMA) */
#endif /*SL_SI917_DMA_*/
