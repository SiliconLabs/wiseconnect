/*******************************************************************************
* @file  rsi_temp_sensor.c
* @brief 
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

/**
 * Includes
 */
#include "rsi_chip.h"

/*==============================================*/
/** 
 * @fn          void RSI_TS_SetCntFreez(MCU_TEMP_Type *pstcTempSens, uint32_t u32CountFreez)
 * @brief       This API is used to set the count of reference clock on which ptat clock counts
 * @param[in]   pstcTempSens  : pointer to the temperature sensor register instance
 * @param[in]   u32CountFreez : count of reference clock on which ptat clock counts
 * @return      none
 */

void RSI_TS_SetCntFreez(MCU_TEMP_Type *pstcTempSens, uint32_t u32CountFreez)
{
  pstcTempSens->TS_ENABLE_AND_TEMPERATURE_DONE_b.CONT_COUNT_FREEZE = (unsigned int)(u32CountFreez & 0x03FF);
  return;
}

/*==============================================*/
/** 
 * @fn         void RSI_TS_RefClkSel(MCU_TEMP_Type *pstcTempSens, boolean_t bRefClk)
 * @brief      This API is used to select the reference clock to the temperature sensor
 * @param[in]  pstcTempSens : pointer to the temperature sensor register instance
 * @param[in]  bRefClk      : reference clock selection
 *                - 0: reference RO clock from analog
 *                - 1: MCU FSM clock
 * @return      none
 */

void RSI_TS_RefClkSel(MCU_TEMP_Type *pstcTempSens, boolean_t bRefClk)
{

  pstcTempSens->TS_ENABLE_AND_TEMPERATURE_DONE_b.REF_CLK_SEL = (unsigned int)(bRefClk & 0x01);
  return;
}

/*==============================================*/
/** 
 * @fn          void RSI_TS_Enable(MCU_TEMP_Type *pstcTempSens, boolean_t bEn)
 * @brief       This API is used to enable / disable the temperature sensor
 * @param[in]    pstcTempSens  : pointer to the temperature sensor register instance
 * @param[in]    bEn           : enable / disable parameter
 *                - 0: Disable
 *                - 1: Enable
 * @return      none
 */

void RSI_TS_Enable(MCU_TEMP_Type *pstcTempSens, boolean_t bEn)
{
  pstcTempSens->TS_ENABLE_AND_TEMPERATURE_DONE_b.TEMP_SENS_EN = (unsigned int)(bEn & 0x01);
  return;
}

/*==============================================*/
/**
 * @fn           void RSI_TS_Config(MCU_TEMP_Type *pstcTempSens, uint32_t u32Nomial)   
 * @brief        This API is used to set the slope of the temperature sensor
 * @param[in]    pstcTempSens  : pointer to the temperature sensor register instance
 * @param[in]    u32Slope      : slope value to be programmed
 * @param[in]    u32Nomial     : calibrated temperature value
 * @param[in]    u32F2Nominal  : ptat clock count during calibration. This will vary with chip to chip
 * @return       none
 */

void RSI_TS_Config(MCU_TEMP_Type *pstcTempSens, uint32_t u32Nomial)
{
  volatile uint32_t reg_write_data                                  = 0;
  reg_write_data                                                    = RSI_IPMU_RO_TsConfig();
  pstcTempSens->TS_SLOPE_SET_b.SLOPE                                = (unsigned int)(reg_write_data & 0x03FF);
  pstcTempSens->TS_FE_COUNTS_NOMINAL_SETTINGS_b.NOMINAL_TEMPERATURE = (unsigned int)(u32Nomial & 0x7F);
  reg_write_data                                                    = RSI_IPMU_RO_TsEfuse();
  pstcTempSens->TS_FE_COUNTS_NOMINAL_SETTINGS_b.F2_NOMINAL          = (unsigned int)(reg_write_data & 0x03FF);
  return;
}

/*==============================================*/
/**
 * @fn          uint32_t RSI_TS_ReadTemp(MCU_TEMP_Type *pstcTempSens)    
 * @brief       This API is used to read the temperature value
 * @param[in]   pstcTempSens   : pointer to the temperature sensor register instance
 * @return      returns the temperature value
 */

uint32_t RSI_TS_ReadTemp(MCU_TEMP_Type *pstcTempSens)
{
  /*Wait for done */
  while (pstcTempSens->TS_ENABLE_AND_TEMPERATURE_DONE_b.TEMP_MEASUREMENT_DONE != 1)
    ;
  /*Return the temperature value*/
  return pstcTempSens->TEMPERATURE_READ_b.TEMPERATURE_RD;
}

/*==============================================*/
/** 
 * @fn          void RSI_TS_RoBjtEnable(MCU_TEMP_Type *pstcTempSens, boolean_t enable)
 * @brief       This API is used to read the temperature value
 * @param[in]   pstcTempSens   : pointer to the temperature sensor register instance
 * @param[in]   enable         : enable or disable bjt based temp sensor
 *                - 0: Disable
 *                - 1: Enable
 * @return      returns the temperature value
 */

void RSI_TS_RoBjtEnable(MCU_TEMP_Type *pstcTempSens, boolean_t enable)
{
  uint32_t i;

  if (pstcTempSens->TS_SLOPE_SET_b.BJT_BASED_TEMP == 1U) {
    if (enable == 1U) {
      return;
    } else {
      pstcTempSens->TS_SLOPE_SET_b.BJT_BASED_TEMP = (unsigned int)(enable & 0x01); //0
    }
  } else {
    if (pstcTempSens->TS_SLOPE_SET_b.BJT_BASED_TEMP == 0U) {
      if (enable == 1U) {
        pstcTempSens->TS_SLOPE_SET_b.BJT_BASED_TEMP = 1;
        for (i = 100; i; i--)
          ; // wait for 100 us
      } else {
        pstcTempSens->TS_SLOPE_SET_b.BJT_BASED_TEMP = (unsigned int)(enable & 0x01); //0
      }
    }
  }
}

/*==============================================*/
/**
 * @fn          void RSI_TS_LoadBjt(MCU_TEMP_Type *pstcTempSens, uint8_t temp)
 * @brief       This API is used to read the temperature value
 * @param[in]   pstcTempSens   : pointer to the temperature sensor register instance
 * @param[in]   temp           : known temprature
 * @return      returns the temperature value
 */

void RSI_TS_LoadBjt(MCU_TEMP_Type *pstcTempSens, uint8_t temp)
{
  pstcTempSens->TS_SLOPE_SET_b.TEMPERATURE_SPI = temp; // update the temp value 	temperature_spi = temp;
  pstcTempSens->TS_SLOPE_SET_b.TEMP_UPDATED    = 1;    // temp_updated =1;
}

/*==============================================*/
/**
 * @fn         uint32_t RSI_TS_GetRefClkCnt(MCU_TEMP_Type *pstcTempSens)     
 * @brief      This API is used to read the reference clock count
 * @param[in]  pstcTempSens   : pointer to the temperature sensor register instance
 * @return     returns the reference clock count
 */

uint32_t RSI_TS_GetRefClkCnt(MCU_TEMP_Type *pstcTempSens)
{
  /*Return the count value*/
  return pstcTempSens->TS_COUNTS_READ_b.COUNT_F1;
}

/*==============================================*/
/**
 * @fn         uint32_t RSI_TS_GetPtatClkCnt(MCU_TEMP_Type *pstcTempSens)
 * @brief      This API is used to read the ptat clock count
 * @param[in]  pstcTempSens   : pointer to the temperature sensor register instance
 * @return     returns the ptat clock count
 */

uint32_t RSI_TS_GetPtatClkCnt(MCU_TEMP_Type *pstcTempSens)
{
  /*Return the count value*/
  return pstcTempSens->TS_COUNTS_READ_b.COUNT_F2;
}

/*==============================================*/
/**
 * @fn         void RSI_Periodic_TempUpdate(TIME_PERIOD_Type *temp, uint8_t enable, uint8_t trigger_time)
 * @brief      This API is used to update the temp periodically after some time
 * @parm[in]   temp         : pointer to the timeperiod register instance
 * @parm[in]   enable       : enable periodic checking of temp
 * @parm[in]   trigger_time : periodic check time in sec
 *             0 -1 sec
 *             1 -2 sec
 *             2 -4 sec
 *             3 -5 sec
 * @return     none
 */

void RSI_Periodic_TempUpdate(TIME_PERIOD_Type *temp, uint8_t enable, uint8_t trigger_time)
{
  temp->MCU_CAL_TEMP_PROG_REG_b.PERIODIC_TEMP_CALIB_EN = (unsigned int)(enable & 0x01);
  temp->MCU_CAL_TEMP_PROG_REG_b.TEMP_TRIGGER_TIME_SEL  = (unsigned int)(trigger_time & 0x03);
  return;
}

/*End of file not truncated */
