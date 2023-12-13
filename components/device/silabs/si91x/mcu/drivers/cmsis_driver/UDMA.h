/*******************************************************************************
* @file  UDMA.h
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

#ifndef __UDMA_H__
#define __UDMA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "rsi_ccp_common.h"
#include "Driver_Common.h" 
#include "rsi_udma.h"

#define UDMA_EVENT_XFER_DONE              (1)
#define UDMA_EVENT_ERROR                  (2)

void IRQ033_Handler (void);
void IRQ010_Handler (void);
#if defined(DAC_FIFO_MODE_EN) || defined(ADC_MULTICHANNEL_WITH_EXT_DMA) 
#define DAC_UDMA_CHANNEL            10 
#define ADC_UDMA_CHANNEL            11
#endif

/* UDMA Resource Configuration */
typedef struct 
{
  UDMA0_Type             *reg;               // UDMA register interface
  IRQn_Type               udma_irq_num;       // UDMA Event IRQ Numbe
  RSI_UDMA_DESC_T        *desc;              // Run-Time control information
} UDMA_RESOURCES;

/* Number of UDMA channels */
#define UDMA_NUMBER_OF_CHANNELS           ((uint8_t) 32)
#define ULP_UDMA_NUMBER_OF_CHANNELS       ((uint8_t) 12)


#define CONTROL_STRUCT0   (UDMA_NUMBER_OF_CHANNELS * 2)
#define CONTROL_STRUCT1   (ULP_UDMA_NUMBER_OF_CHANNELS * 2)

/**
 * @fn          void UDMA_SignalEvent_t (uint32_t event)
 * @brief       Signal UDMA Events.
 * @param[in]   event  UDMA Event mask
 * @param[in]   ch     Channel no
 * @return      none
*/
typedef void  (*UDMA_SignalEvent_t) (uint32_t event, uint32_t ch);


/**
\brief Access structure of the UDMA Driver.
*/
typedef struct _RSI_DRIVER_UDMA {
                                                             
  int32_t  (*Initialize)      ( void );                                      
  int32_t  (*Uninitialize)    ( void );                                                                
  int32_t  (*ChannelConfigure)( uint8_t                    ch,
                                uint32_t                   src_addr, 
                                uint32_t                   dest_addr,
                                uint32_t                   size,
                                RSI_UDMA_CHA_CONFIG_DATA_T control,
                                RSI_UDMA_CHA_CFG_T         *config,
                                UDMA_SignalEvent_t         cb_event );                                               
  int32_t  (*ChannelEnable)   ( uint8_t ch ); 
  int32_t  (*ChannelDisable)  ( uint8_t ch );                   
  uint32_t (*ChannelGetCount) ( uint8_t ch, 
                                RSI_UDMA_CHA_CONFIG_DATA_T control,
                                RSI_UDMA_CHA_CFG_T         config );
  int32_t  (*DMAEnable)       ( void );
} const RSI_DRIVER_UDMA;

#ifdef __cplusplus
}
#endif
#endif // __UDMA_H__


