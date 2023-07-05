/*******************************************************************************
* @file  rsi_ct.c
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

#include "rsi_ccp_user_config.h"

#include "rsi_rom_ct.h"
#include "rsi_rom_clks.h"
#include "rsi_rom_egpio.h"
#include "rsi_ccp_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ROMDRIVER_PRESENT

/*==============================================*/
/**
 * @fn           void ct_ocu_high_Low_toggle_select(RSI_CT_T *pCT, boolean_t lowHigh, boolean_t counterNum, uint8_t outputSel)
 * @brief		 This API is used to toggle output state of OCU.
 * @param[in]	 pCT         : Pointer to the CT instance register area
 * @param[in]	 lowHigh     : Output low/high state
 * @param[in]    counterNum  : Counter 0/1
 * @param[in]    outputSel   : OCU possibilities(0 to 7)
 * @return 		 none
 */
void ct_ocu_high_Low_toggle_select(RSI_CT_T *pCT, boolean_t lowHigh, boolean_t counterNum, uint8_t outputSel)
{
  if (counterNum == 0) {
    if (lowHigh == LOW) {
      // Sets Make output low signal
      pCT->CT_OCU_CTRL_REG_b.MAKE_OUTPUT_0_LOW_SEL = outputSel;
    } else {
      // Sets Make output high signal
      pCT->CT_OCU_CTRL_REG_b.MAKE_OUTPUT_0_HIGH_SEL = outputSel;
    }
  } else {
    if (lowHigh == LOW) {
      // Sets Make output low signal
      pCT->CT_OCU_CTRL_REG_b.MAKE_OUTPUT_1_LOW_SEL = outputSel;
    } else {
      // Sets Make output high signal
      pCT->CT_OCU_CTRL_REG_b.MAKE_OUTPUT_1_HIGH_SEL = outputSel;
    }
  }
}

/*==============================================*/
/**
 * @fn            error_t ct_wfg_control_config(RSI_CT_T *pCT, WFG_PARAMS_T ctrlReg)
 * @brief		  This API is used to toggle output state of WFG
 * @param[in]	  pCT         : Pointer to the CT instance register area
 * @param[in]	  ctrlReg     : Variable to a structure of type \ref WFG_PARAMS_T
 * @return 		  \ref ERROR_CT_INVALID_ARG : If WFG_PARAMS_T structure parameters are invalid
 *                \n \ref RSI_OK            : If process is done successfully
 */
error_t ct_wfg_control_config(RSI_CT_T *pCT, WFG_PARAMS_T ctrlReg)
{
  uint32_t value = 0;

  if (((ctrlReg.output0_tgl0_sel <= OUTPUT_0_TGL_0_MAX) && (ctrlReg.output0_tgl1_sel <= OUTPUT_0_TGL_1_MAX)
       && (ctrlReg.tgl_cnt0_peak <= TGL_CNT_0_PEAK))
      || ((ctrlReg.output1_tgl0_sel <= OUTPUT_1_TGL_0_MAX) && (ctrlReg.output1_tgl1_sel <= OUTPUT_1_TGL_1_MAX)
          && (ctrlReg.tgl_cnt1_peak <= TGL_CNT_1_PEAK))) {
    value = ((ctrlReg.output0_tgl0_sel << 0) | (ctrlReg.output0_tgl1_sel << 3) | (ctrlReg.tgl_cnt0_peak << 8)
             | (ctrlReg.output1_tgl0_sel << 16) | (ctrlReg.output1_tgl1_sel << 19) | (ctrlReg.tgl_cnt1_peak << 24));

    pCT->CT_WFG_CTRL_REG = value;
  } else {
    return ERROR_CT_INVALID_ARG;
  }

  return RSI_OK;
}

/*==============================================*/
/**
 * @fn          error_t ct_ocu_control(RSI_CT_T *pCT, boolean_t counterNum,boolean_t dmaEn,
 *                                         OCU_PARAMS_T *pOCUparams,RSI_CT_CALLBACK_T *pCB)
 * @brief		This API controls OCU operation
 * @param[in]   pCT        	 : Pointer to the CT instance register area
 * @param[in]   counterNum   : Counter 0/1
 * @param[in]   dmaEn		     : DMA enable flag
 *                               - \ref 0 : not enable DMA.
 *                               - \ref 1 : DMA enable.
 * @param[in]   pOCUparams   : Pointer to the OCU parameters structure and its members are as below \ref OCU_PARAMS_T
 *                              - \ref  CompareVal1_0       : Threshold one of Counter 0,possible values  (0x0 to 0xFFFF)
 *                              - \ref  CompareVal2_0 	    : Threshold two of Counter 0,possible values (0x0 to 0xFFFF)
 *                              - \ref  CompareVal1_1 	    : Threshold two of Counter 1,possible values (0x0 to 0xFFFF)
 *                              - \ref  CompareVal2_1 	    : Threshold two of Counter 1,possible values (0x0 to 0xFFFF)
 *                              - \ref  CompareVal1_0 	    : Threshold two of Counter 0,possible values (0x0 to 0xFFFF)
 *                              - \ref  CompareNextVal2_0 	: next compare value of counter 0,possible values (0x0 to 0xFFFF)
 *                              - \ref  CompareNextVal1_1 	: next compare value of counter 1,possible values (0x0 to 0xFFFF)
 *                              - \ref  CompareNextVal2_1 	: next compare value of counter 1,possible values (0x0 to 0xFFFF)
 *                              - \ref  SyncValue         	: Holds the starting point of counters for synchronization purpose
 *                                                          This is logical OR of sync values for counter 0& 1,
 *                                                          possible values for two counters (0x0 to 0xFFFF).
 * @param[in]   pCB          : Pointer to CT callback structure \ref RSI_CT_CALLBACK_T
 * @return 	    \ref ERROR_CT_INVALID_ARG : If passed parameters are invalid
 *               \n \ref RSI_OK            : If process is done successfully
 */
error_t ct_ocu_control(RSI_CT_T *pCT,
                       boolean_t counterNum,
                       boolean_t dmaEn,
                       OCU_PARAMS_T *pOCUparams,
                       RSI_CT_CALLBACK_T *pCB)
{
  if ((pOCUparams) == NULL) {
    return ERROR_CT_INVALID_ARG;
  }

  if (counterNum) {
    // set 2 threshold vlaues for couter 1
    pCT->CT_OCU_COMPARE2_REG_b.OCU_COMPARE2_0_REG = pOCUparams->CompareVal1_1;
    pCT->CT_OCU_COMPARE2_REG_b.OCU_COMPARE2_1_REG = pOCUparams->CompareVal2_1;
    // set Sync value
    pCT->CT_OCU_SYNC_REG_b.OCU_SYNC_CHANNEL1_REG = (pOCUparams->SyncWith1);
  } else {
    // set 2 threshold values for counter 0
    pCT->CT_OCU_COMPARE_REG_b.OCU_COMPARE_0_REG = pOCUparams->CompareVal1_0;
    pCT->CT_OCU_COMPARE_REG_b.OCU_COMPARE_1_REG = pOCUparams->CompareVal2_0;
    // set Sync value
    pCT->CT_OCU_SYNC_REG_b.OCU_SYNC_CHANNEL0_REG = pOCUparams->SyncWith0;
  }
  // DMA call back handle
  if (dmaEn) {
    if (counterNum == 0) {
      if (pCT->CT_OCU_CTRL_REG_b.OCU_0_DMA_MODE) {
        if ((pCB->cbFunc) != NULL) {
          // DMA call back event
          pCB->cbFunc(CT_OCU_DMA_EN_COUNTER0, counterNum);
        } else {
          return ERROR_CT_INVALID_ARG;
        }
      }
    } else {
      if (pCT->CT_OCU_CTRL_REG_b.OCU_1_DMA_MODE) {
        if ((pCB->cbFunc) != NULL) {
          // DMA call back event
          pCB->cbFunc(CT_OCU_DMA_EN_COUNTER1, counterNum);
        } else {
          return ERROR_CT_INVALID_ARG;
        }
      }
    }
  }

  else {
    if (counterNum == 0) {
      // set compare next value for coutner 0
      pCT->CT_OCU_COMPARE_NXT_REG_b.OCU_COMPARE_NXT_COUNTER0   = pOCUparams->CompareNextVal1_0;
      pCT->CT_OCU_COMPARE2_NXT_REG_b.OCU_COMPARE2_NXT_COUNTER0 = pOCUparams->CompareNextVal2_0;
    } else {
      // set compare next value for counter 1
      pCT->CT_OCU_COMPARE_NXT_REG_b.OCU_COMPARE_NXT_COUNTER1   = pOCUparams->CompareNextVal1_1;
      pCT->CT_OCU_COMPARE2_NXT_REG_b.OCU_COMPARE2_NXT_COUNTER1 = pOCUparams->CompareNextVal2_1;
    }
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t ct_wfg_comapre_value_set( RSI_CT_T *pCT,
 *                                                 boolean_t counterNum,																                  OCU_PARAMS_T *pOCUparams)
 * @brief		 This function controls WFG operation
 * @param[in]	 pCT          : Pointer to the CT instance register area
 * @param[in]    counterNum   : Counter 0/1
 * @param[in]    pOCUparams   : Pointer to the OCU parameters structure and its members are as below \ref OCU_PARAMS_T
 *								- \ref  CompareVal1_0   : Threshold one of Counter 0,possible values  (0x0 to 0xFFFF)
 *								- \ref  CompareVal2_0   : Threshold two of Counter 0,possible values  (0x0 to 0xFFFF)
 *							    - \ref  CompareVal1_1   : Threshold one of Counter 1,possible values  (0x0 to 0xFFFF)
 *								- \ref  CompareVal2_1   : Threshold one of Counter 1,possible values  (0x0 to 0xFFFF)
 *                              - \ref  SyncValue       : Holds the starting point of counters for synchronization purpose
 *			                    This is logical OR of sync values for counter 0& 1 possible values for two counters (0x0 to 0xFFFF)
 * @return 		  \ref ERROR_CT_INVALID_ARG : If passed parameters are invalid
 *                 \n \ref RSI_OK            : If process is done successfully
 */
error_t ct_wfg_comapre_value_set(RSI_CT_T *pCT, boolean_t counterNum, OCU_PARAMS_T *pOCUparams)
{
  if (pOCUparams != NULL) {
    if (counterNum == 0) {
      /* set 2 threshold vlaues for couter 0 */
      pCT->CT_OCU_COMPARE_REG_b.OCU_COMPARE_0_REG = pOCUparams->CompareVal1_0;
      pCT->CT_OCU_COMPARE_REG_b.OCU_COMPARE_1_REG = pOCUparams->CompareVal2_0;
    } else {
      /* set 2 threshold values for counter 1 */
      pCT->CT_OCU_COMPARE2_REG_b.OCU_COMPARE2_0_REG = pOCUparams->CompareVal1_1;
      pCT->CT_OCU_COMPARE2_REG_b.OCU_COMPARE2_1_REG = pOCUparams->CompareVal2_1;
    }
  } else {
    return ERROR_CT_INVALID_ARG;
  }
  return RSI_OK;
}
#endif //ROMDRIVER_PRESENT

/*==============================================*/
/**
 * @fn          void RSI_CT_Init( void )
 * @brief		 This API is initialize the ct pins and clocks
 * @return 		 none
 */
void RSI_CT_Init(void)
{
  // CT OUT0
  if (RTE_SCT_OUT_0_PIN >= 25 && RTE_SCT_OUT_0_PIN <= 30) {
    RSI_EGPIO_HostPadsGpioModeEnable(RTE_SCT_OUT_0_PIN);
  }
  if (RTE_SCT_OUT_0_PIN > 63) {
    RSI_EGPIO_UlpPadReceiverEnable((uint8_t)(RTE_SCT_OUT_0_PIN - 64));
    RSI_EGPIO_SetPinMux(EGPIO1, 0, (uint8_t)(RTE_SCT_OUT_0_PIN - 64), 6);
  } else {
    RSI_EGPIO_PadReceiverEnable(RTE_SCT_OUT_0_PIN);
  }
  if (RTE_SCT_OUT_0_PAD != 0) {
    RSI_EGPIO_PadSelectionEnable(RTE_SCT_OUT_0_PAD);
  }
  RSI_EGPIO_SetPinMux(EGPIO, 0, RTE_SCT_OUT_0_PIN, RTE_SCT_OUT_0_MUX);

  // CT OUT1
  if (RTE_SCT_OUT_1_PIN >= 25 && RTE_SCT_OUT_1_PIN <= 30) {
    RSI_EGPIO_HostPadsGpioModeEnable(RTE_SCT_OUT_1_PIN);
  }
  if (RTE_SCT_OUT_1_PIN > 63) {
    RSI_EGPIO_UlpPadReceiverEnable((uint8_t)(RTE_SCT_OUT_1_PIN - 64));
    RSI_EGPIO_SetPinMux(EGPIO1, 0, (uint8_t)(RTE_SCT_OUT_1_PIN - 64), 6);
  } else {
    RSI_EGPIO_PadReceiverEnable(RTE_SCT_OUT_1_PIN);
  }
  if (RTE_SCT_OUT_1_PAD != 0) {
    RSI_EGPIO_PadSelectionEnable(RTE_SCT_OUT_1_PAD);
  }

  RSI_EGPIO_SetPinMux(EGPIO, 0, RTE_SCT_OUT_1_PIN, RTE_SCT_OUT_1_MUX);

  // Initialization and Configure the CT clocks
  RSI_CLK_CtClkConfig(M4CLK, SCT_CLOCK_SOURCE, SCT_CLOCK_DIV_FACT, ENABLE_STATIC_CLK);
}

/* ROM API Structure

const ROM_CT_API_T ct_api = {

		&ct_ocu_high_Low_toggle_select,
		&ct_wfg_control_config,
		&ct_ocu_control,
		&ct_wfg_comapre_value_set
};

*/
#ifdef __cplusplus
}
#endif
