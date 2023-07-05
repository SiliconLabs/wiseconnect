/*******************************************************************************
* @file  rsi_cci.c
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

#if defined(CHIP_9118)
#include "rsi_chip.h"
/*==============================================*/
/** 
 * @fn      void RSI_CCI_AmsEnable() 
 * @brief   This API enables the CCI in the master mode 
 * @return  None
 */
void RSI_CCI_AmsEnable()
{
  AMS_EN |= (1 << 15);
}

/*==============================================*/
/**   
 * @fn          error_t RSI_CCI_AMS_Initialise(RSI_CCI_Init_t *p_cci_config)
 * @brief       This API configures the CCI peripheral 
 * @param[in]   p_cci_config CCI configuration structure pointer
 *              \n 
 *              \ref    RSI_CCI_Init_t
 *              \ref    CCI_CONTROL_REG_b
 *              \ref    CCI_LSB_A_S1_REG 
 * @return      \ref RSI_OK if success full or 
 *              else error code
 */
error_t RSI_CCI_AMS_Initialise(RSI_CCI_Init_t *p_cci_config)
{
  CCI_Type *pcci = CCI;

  //Choose the SDR or DDR mode, Address translation enable, address width config and number of slaves by programming the CCI_CONTROL register.
  pcci->CCI_CONTROL_b.MODE              = (p_cci_config->mode << 2);
  pcci->CCI_CONTROL_b.TRANSLATE_ENABLE  = p_cci_config->translation_enable;
  pcci->CCI_CONTROL_b.ADDR_WIDTH_CONFIG = p_cci_config->address_width_config;
  pcci->CCI_CONTROL_b.MODE              = (p_cci_config->interface_width << 0);
  pcci->CCI_CONTROL_b.ENABLED_SLAVES    = p_cci_config->slave_enable;
  pcci->CCI_CONTROL_b.SLAVE_PRIORITY    = p_cci_config->slave_priority;
  pcci->CCI_CONTROL_b.TIME_OUT_PRG      = p_cci_config->slave_timeout;
  pcci->CCI_CONTROL_b.CCI_CTRL_ENABLE   = p_cci_config->cci_cntrl_en;

  if (p_cci_config->mode == 1) // if mode is ddr then load 2x clock enable
  {
    // 2x clock enable for ddr mode
    pcci->CCI_PREFETCH_CTRL_b.CCI_2X_CLK_ENABLE_FOR_DDR_MODE = 1;
  }

  // Allocate the lower and higher address range for the each slave by programming in the following registers
  switch (p_cci_config->slave_enable) {
    case 0: {
      if (p_cci_config->slave_lsb_address[0] < p_cci_config->slave_msb_address[0]) {
        // Load LSB address supportes and MSB address supported
        pcci->CCI_LSB_A_S1 = p_cci_config->slave_lsb_address[0];
        pcci->CCI_MSB_A_S1 = p_cci_config->slave_msb_address[0];
      } else {
        return ERROR_CCI_ADDRESS_ERR;
      }
      break;
    }
    case 1: {
      if (p_cci_config->slave_lsb_address[0] < p_cci_config->slave_msb_address[0]) {
        // Load LSB address supportes and MSB address supported
        pcci->CCI_LSB_A_S1 = p_cci_config->slave_lsb_address[0];
        pcci->CCI_MSB_A_S1 = p_cci_config->slave_msb_address[0];
      } else {
        return ERROR_CCI_ADDRESS_ERR;
      }
      break;
    }
    case 2: {
      if (p_cci_config->slave_lsb_address[1] < p_cci_config->slave_msb_address[1]) {
        // Load LSB address supportes and MSB address supported
        pcci->CCI_LSB_A_S2 = p_cci_config->slave_lsb_address[1];
        pcci->CCI_MSB_A_S2 = p_cci_config->slave_msb_address[1];
      } else {
        return ERROR_CCI_ADDRESS_ERR;
      }
      break;
    }

    case 3: {
      if (p_cci_config->slave_lsb_address[0] < p_cci_config->slave_msb_address[0]) {
        // Load LSB address supportes and MSB address supported
        pcci->CCI_LSB_A_S1 = p_cci_config->slave_lsb_address[0];
        pcci->CCI_MSB_A_S1 = p_cci_config->slave_msb_address[0];
      } else {
        return ERROR_CCI_ADDRESS_ERR;
      }

      if (p_cci_config->slave_lsb_address[1] < p_cci_config->slave_msb_address[1]) {
        // Load LSB address supportes and MSB address supported
        pcci->CCI_LSB_A_S2 = p_cci_config->slave_lsb_address[1];
        pcci->CCI_MSB_A_S2 = p_cci_config->slave_msb_address[1];
      } else {
        return ERROR_CCI_ADDRESS_ERR;
      }
      break;
    }
  }

  // Load the translation address if address translation feature is enabled by programming the
  // cci_translation_address.
  if ((p_cci_config->translation_enable) == 1) {
    pcci->CCI_TRANS_ADDRESS = p_cci_config->translation_address;
  }

  return RSI_OK;
}

/*==============================================*/
/**
 * @fn          uint32_t RSI_CCI_SetFifoThreshlod(volatile CCI_Type *pstcCCI,uint8_t val) 
 * @brief       This API sets the CCI threshhold fifo value
 * @param[in]   pstcCCI CCI configuration structure pointer \ref CCI_Type
 *                                                          \ref CCI_FIFO_THRESHOLD_REG 
 * @param[in]   val        Threshold value 
 * @return      \ref RSI_OK if success full or 
 *              \n else error code
 */
uint32_t RSI_CCI_SetFifoThreshlod(volatile CCI_Type *pstcCCI, uint8_t val)
{
  pstcCCI->CCI_FIFO_THRESHOLD_REG |= val;
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn          uint32_t RSI_CCI_PrefetchEnable(volatile CCI_Type *pstcCCI)
 * @brief       This API is used for prefetct enable
 * @param[in]   pstcCCI CCI configuration structure pointer
 *              \ref  CCI_Type
 * @return      \ref  RSI_OK if success full or 
 *              \n else error code
 */

uint32_t RSI_CCI_PrefetchEnable(volatile CCI_Type *pstcCCI)
{
  pstcCCI->CCI_PREFETCH_CTRL_b.CCI_PREFETCH_EN = 0x1;
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn          uint32_t RSI_CCI_IntClear(volatile CCI_Type *pstcCCI,uint8_t interrupt)
 * @brief       This API is used to clear the interrupts
 * @param[in]   pstcCCI CCI configuration structure pointer 
 *              \ref  CCI_Type
 * @param[in]   interrupt     0-2 for interrupt from peer chips
 *              \n            3 for message interrupt from slave
 * @return      \ref  RSI_OK if success full or 
 *              \n else error code
 */
uint32_t RSI_CCI_IntClear(volatile CCI_Type *pstcCCI, uint8_t interrupt)
{
  return pstcCCI->CCI_MODE_INTR_STATUS_b.INTR_CLEAR = (0x1 << interrupt);
}
#endif
