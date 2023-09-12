/*******************************************************************************
 * @file  sl_si91x_sdio_secondary.h
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
#ifndef __SL_SI91X_SDIO_SECONDARY_H_
#define __SL_SI91X_SDIO_SECONDARY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "rsi_chip.h"
#include "sl_status.h"

/***************************************************************************/ /**
 * @addtogroup SDIO Secondary
 * @ingroup SI91X_PERIPHERAL_APIS
 * @{
 *
 *******************************************************************************/

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/
#define NUMGPDMADESC 10

/// Events for HIF irq handler
#define HOST_INTR_RECEIVE_EVENT BIT(0)

/*******************************************************************************
 *****************************   DATA TYPES   *********************************
 ******************************************************************************/
typedef enum {
  HOST_INTR_NOT_RECEIVED = 0,
  HOST_INTR_RECEIVED     = 1,
} sl_sdio_slave_rx_intr_status_t;

/// @brief Structure to hold the versions number of  API
typedef struct {
  uint8_t release; ///< Release version number
  uint8_t major;   ///< Major version number
  uint8_t minor;   ///< Minor version number
} sl_sdio_secondary_version_t;

typedef void (*sl_sdio_secondary_callback_t)(uint8_t events);       // SDIO callback function
typedef void (*sl_sdio_secondary_gpdma_callback_t)(uint8_t dma_ch); // SDIO dma callback function

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/***************************************************************************/ /**
 * @brief       
 * Initializes SDIO secondary, by default SDIO pin access with TA
 * this API gives the pin access to M4, selects the SDIO mode and enable the sdio
 * primary interrupts 
 *
 * @param[in] none
 *
 * @return none
 ******************************************************************************/
sl_status_t sl_si91x_sdio_secondary_init(void);

/***************************************************************************/ /**
 * @brief       
 *   Trigger sequence to send data from sdio secondary in non blocking mode to 
 *   host/primary device.
 *   This API accepts the num of blocks i.e one block will have  block length
 *   (1 to 1024) byptes and pointer to the data buffer to be transffered.  
 *
 * @param[in] num_of_blocks
 *   Number of blocks to be sent
 * 
 * @param[in] data_buf
 *   Reference of the Source buffer
 *
 * @return none
 ******************************************************************************/
void sl_si91x_sdio_secondary_send(uint8_t num_of_blocks, uint8_t *data_buf);

/***************************************************************************/ /**
 * @brief       
 *  Receive data on sdio secondary in non blocking mode from host/primary device 
 *  using DMA.
 *
 * @param[in] data_buf
 *   Reference of the Destination buffer in whuch data will receive
 *
 * @return none
 ******************************************************************************/
void sl_si91x_sdio_secondary_receive(uint8_t *data_buf);

/***************************************************************************/ /**
* @brief
* Register the SDIO secondary user callback function.
*
* @pre sl_si91x_sdio_secondary_init();
*
* @param[in] callback_event Pointer to the function which needs to be called at the time of interrupt
* @param[in] flag Interrupt flag to be registered
* @return status 0 if successful, else error code
*         \ref SL_STATUS_OK (0x0000) - Success \n
*         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
*         \ref SL_STATUS_BUSY (0x0004) - Driver is busy \n
******************************************************************************/
sl_status_t sl_si91x_sdio_secondary_register_event_callback(sl_sdio_secondary_callback_t callback_event, uint32_t flag);

/***************************************************************************/ /**
* @brief
* Un-register the SDIO secondary user callback function.
*
* @param[in] flag Interrupt flag to be unregistered
* @return none
******************************************************************************/
void sl_si91x_sdio_secondary_unregister_event_callback(uint32_t flag);

/***************************************************************************/ /**
* @brief
* Register the SDIO secondary dma user callback function.
*
* @pre sl_si91x_sdio_secondary_init();
*
* @param[in] callback_event Pointer to the function which needs to be called at the time of interrupt
* @return status 0 if successful, else error code
*         \ref SL_STATUS_OK (0x0000) - Success \n
*         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
*         \ref SL_STATUS_BUSY (0x0004) - Driver is busy \n
******************************************************************************/
sl_status_t sl_si91x_sdio_secondary_gpdma_register_event_callback(sl_sdio_secondary_gpdma_callback_t callback_event);

/***************************************************************************/ /**
* @brief
* Un-register the SDIO secondary dma user callback function.
*
* @param[in] none
* @return none
******************************************************************************/
void sl_si91x_sdio_secondary_gpdma_unregister_event_callback(void);

/***************************************************************************/ /**
* @brief
* Get the SDIO Secondary Driver version.
*
* @details
* This function is used to know the SDIO Secondary Driver version
*
* @param[in] none
* @return \ref sl_sdio_secondary_version_t type version
******************************************************************************/
sl_sdio_secondary_version_t sl_si91x_sdio_secondary_get_version(void);
/** @} (end addtogroup SDIO Secondary) */

#ifdef __cplusplus
}
#endif

#endif //__SL_SI91X_SDIO_SECONDARY_H_
