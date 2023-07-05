/*******************************************************************************
* @file  rsi_qei.c
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

// Include Files

#include "rsi_chip.h"
#include "rsi_ccp_common.h"

/*==============================================*/
/**
 * @fn          void RSI_QEI_Enable(volatile QEI_Type *pstcQei)      
 * @brief	    Enables the clock to the QEI module
 * @param[in]   pstcQei - Pointer to the QEI register instance
 * @return 	    none
 */
void RSI_QEI_Enable(volatile QEI_Type *pstcQei)
{
  // QEI Direction pin mux
  if (RTE_QEI_DIR_PIN >= 25 && RTE_QEI_DIR_PIN <= 30) {
    RSI_EGPIO_HostPadsGpioModeEnable(RTE_QEI_DIR_PIN);
  } else if (RTE_QEI_DIR_PIN > 63) {
    RSI_EGPIO_UlpPadReceiverEnable((uint8_t)(RTE_QEI_DIR_PIN - 64));
    RSI_EGPIO_SetPinMux(EGPIO1, 0, (uint8_t)(RTE_QEI_DIR_PIN - 64), 6);
  }
  RSI_EGPIO_PadSelectionEnable(RTE_QEI_DIR_PAD);

  RSI_EGPIO_PadReceiverEnable(RTE_QEI_DIR_PIN);

  RSI_EGPIO_SetPinMux(EGPIO, RTE_QEI_DIR_PORT, RTE_QEI_DIR_PIN, RTE_QEI_DIR_MUX);

  // QEI Index pin mux
  if (RTE_QEI_IDX_PIN >= 25 && RTE_QEI_IDX_PIN <= 30) {
    RSI_EGPIO_HostPadsGpioModeEnable(RTE_QEI_IDX_PIN);
  } else if (RTE_QEI_IDX_PIN > 63) {
    RSI_EGPIO_UlpPadReceiverEnable((uint8_t)(RTE_QEI_IDX_PIN - 64));
    RSI_EGPIO_SetPinMux(EGPIO1, 0, (uint8_t)(RTE_QEI_IDX_PIN - 64), 6);
  }
  RSI_EGPIO_PadSelectionEnable(RTE_QEI_IDX_PAD);

  RSI_EGPIO_PadReceiverEnable(RTE_QEI_IDX_PIN);

  RSI_EGPIO_SetPinMux(EGPIO, RTE_QEI_IDX_PORT, RTE_QEI_IDX_PIN, RTE_QEI_IDX_MUX);

  // QEI PhaseA pin mux
  if (RTE_QEI_PHA_PIN >= 25 && RTE_QEI_PHA_PIN <= 30) {
    RSI_EGPIO_HostPadsGpioModeEnable(RTE_QEI_PHA_PIN);
  } else if (RTE_QEI_PHA_PIN > 63) {
    RSI_EGPIO_UlpPadReceiverEnable((uint8_t)(RTE_QEI_PHA_PIN - 64));
    RSI_EGPIO_SetPinMux(EGPIO1, 0, (uint8_t)(RTE_QEI_PHA_PIN - 64), 6);
  }
  RSI_EGPIO_PadSelectionEnable(RTE_QEI_PHA_PAD);

  RSI_EGPIO_PadReceiverEnable(RTE_QEI_PHA_PIN);

  RSI_EGPIO_SetPinMux(EGPIO, RTE_QEI_PHA_PORT, RTE_QEI_PHA_PIN, RTE_QEI_PHA_MUX);

  // QEI PhaseB pin mux
  if (RTE_QEI_PHB_PIN >= 25 && RTE_QEI_PHB_PIN <= 30) {
    RSI_EGPIO_HostPadsGpioModeEnable(RTE_QEI_PHB_PIN);
  } else if (RTE_QEI_PHB_PIN > 63) {
    RSI_EGPIO_UlpPadReceiverEnable((uint8_t)(RTE_QEI_PHB_PIN - 64));
    RSI_EGPIO_SetPinMux(EGPIO1, 0, (uint8_t)(RTE_QEI_PHB_PIN - 64), 6);
  }

  RSI_EGPIO_PadSelectionEnable(RTE_QEI_PHB_PAD);

  RSI_EGPIO_SetPinMux(EGPIO, RTE_QEI_PHB_PORT, RTE_QEI_PHB_PIN, RTE_QEI_PHB_MUX);

  RSI_EGPIO_PadReceiverEnable(RTE_QEI_PHB_PIN);

  // QEI clock enable routine here
  RSI_CLK_PeripheralClkEnable2(M4CLK, QE_PCLK_ENABLE);

  return;
}

/*==============================================*/
/**
 * @fn          void RSI_QEI_Disable(volatile QEI_Type *pstcQei)
 * @brief	    Disables the clock to the QEI module
 * @param[in]   pstcQei - Pointer to the QEI register instance
 * @return 	    none
 */
void RSI_QEI_Disable(volatile QEI_Type *pstcQei)
{
  // clock disable to QEI module
  RSI_CLK_PeripheralClkDisable2(M4CLK, QE_PCLK_ENABLE);
  return;
}
