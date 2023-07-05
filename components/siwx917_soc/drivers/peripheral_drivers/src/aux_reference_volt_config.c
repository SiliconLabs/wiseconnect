/*******************************************************************************
* @file  aux_reference_volt_config.c
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

#include "RS1xxxx.h"
#include "si91x_device.h"
#include "aux_reference_volt_config.h"

/*==============================================*/
/**
 * @fn        error_t RSI_AUX_RefVoltageConfig(float verf_val, float chip_vltg)
 * @brief     This API used for configure reference voltage for ADC operation.
 * @param[in] verf_val     - Reference voltage for ADC.
 * @param[in] chip_vltg    - Chip voltage 3.2V or 1.8V,If chip voltage to chip is 1.8V then. 
 *							\n chip voltage(bypass LDO) act as reference voltage to analog peripheral.
 * @return    Success      - Returns 'RSI_OK' on successful execution.
 */
error_t RSI_AUX_RefVoltageConfig(float verf_val, float chip_vltg)
{
  uint8_t temp                                 = 0;
  float cntrl_val                              = 0;
  AUX_ADC_DAC_COMP->AUX_LDO_b.ENABLE_LDO       = 1;
  AUX_ADC_DAC_COMP->AUX_LDO_b.LDO_DEFAULT_MODE = 1;
  if ((verf_val > (float)2.8) || (chip_vltg < (float)3.0)) {
    AUX_ADC_DAC_COMP->AUX_LDO_b.BYPASS_LDO = 1;
  } else {
    if (verf_val < (float)1.6) {
      AUX_ADC_DAC_COMP->AUX_LDO_b.LDO_CTRL         = 0;
      AUX_ADC_DAC_COMP->AUX_LDO_b.ENABLE_LDO       = 0;
      AUX_ADC_DAC_COMP->AUX_LDO_b.LDO_DEFAULT_MODE = 0;
    } else {
      cntrl_val = ((verf_val - (float)1.6) / (float)(0.08));
      temp      = cntrl_val + 1;
      cntrl_val = (cntrl_val + (float)0.50);
      if ((float)temp > cntrl_val) {
        temp = cntrl_val;
      }
      AUX_ADC_DAC_COMP->AUX_LDO_b.LDO_CTRL = temp;
    }
  }
  return RSI_OK;
}
