/*******************************************************************************
* @file  rsi_bod.c
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
#include "rsi_bod.h"

volatile uint32_t batt_status_check;
/**
 * \ingroup   RSI_SPECIFIC_DRIVERS
 * \defgroup  RSI_BOD RSI:RS1xxxx BOD 
 *  @{
 *
 */

/*==============================================*/
/**
 * @fn        float RSI_BOD_SoftTriggerGetBatteryStatus(void) 
 * @brief     This API is used to get the battery level status
 * @return    returns the Voltage value
 */

float RSI_BOD_SoftTriggerGetBatteryStatus(void)
{
  volatile static uint32_t batt_status, batt_status_supply = 0;
  volatile static float vbatt = 0;

  /*disable the BOD interrupt  */
  RSI_BOD_IntrEnable(DISABLE);
  /* Enable's the power gating for calibration block  and enable's the check vbatt status  bit  */
  ULP_SPI_MEM_MAP(BOD_VBATT_STATUS_REG) |= BOD_PWRGATE_EN_N_ULP_BUTTON_CALIB
                                           | (CHECK_VBATT_STATUS); //set bit 15 and bit 7 */
  /*Wait for vbatt status valid bit to be 1. */
  while (!(ULP_SPI_MEM_MAP(BOD_VBATT_STATUS_REG) & BIT(6)))
    ;
  /*   battery status value */
  batt_status = ((ULP_SPI_MEM_MAP(BOD_VBATT_STATUS_REG) & BOD_VBATT_STATUS_REG_BITS));
  if (batt_status > BATTERY_STATUS_MAX) {
    /*If the battery status is > than 38 make it to 38 */
    batt_status = BATTERY_STATUS_MAX;
  }
  if (vbatt_status_trim_efuse[2]) {
    /*read the battery status value at the 3.3 supply from config file */
    batt_status_supply = vbatt_status_trim_efuse[2] & (BOD_CLEAR_BATTERY_STATUS_BITS_EFUSE);
    /* converts the battery status to vbatt voltage value*/
    vbatt = BOD_VBATT_COMPUT_FIRST_ELEMENT * (BOD_VBATT_COMPUT_SECOND_ELEMENT + batt_status)
            / (BOD_VBATT_COMPUT_SECOND_ELEMENT + batt_status_supply);
  } else {
    /* converts the battery status to vbatt voltage value*/
    vbatt = BOD_VBATT_COMPUT_FIRST_ELEMENT * (BOD_VBATT_COMPUT_SECOND_ELEMENT + batt_status)
            / (BOD_VBATT_COMPUT_SECOND_ELEMENT + BOD_VBATT_COMPUT_THIRD_ELEMENT);
  }

  /*clr  the BOD interrupt  */
  RSI_BOD_IntrClr();
  /* enable the BOD interrupt */
  RSI_BOD_IntrEnable(ENABLE);
  return vbatt;
}

/*==============================================*/
/**
 * @fn          void RSI_BOD_Enable(uint8_t enable, float vbatt_threshold)
 * @brief       This API is used to enable or disable the BOD .
 * @param[in]   enable: 1: enables  the BOD.
 *                      0: disables  the BOD.  
 * @param[in]   vbatt_threshold : Set's the threshold value 
 * @return      none 
 */

void RSI_BOD_Enable(uint8_t enable, float vbatt_threshold)
{
  volatile static uint32_t threshold_i = 0;
  volatile static float threshold_f    = 0;
  volatile uint32_t delay_button = 0, button_loop = 0;
  /* Disable signal for bod detection */
  if (BOD_ULP_PROC_CLK_SEL == 0) {
    delay_button = (16 / BOD_CLOCK_DIVISON_FACTOR) + 1;
  } else {
    delay_button = 16;
  }

  if (enable == 1) {
    /* Enable signal for bod detection */
    ULP_SPI_MEM_MAP(BOD_COMP_SEL_REG) |= BIT(16);
    /* Clear the MANUAL_CMP_MUX_SEL_VALUE bits for manual bod */
    ULP_SPI_MEM_MAP(BOD_COMP_MODE_REG) &= ~(MANUAL_CMP_MUX_SEL_BOD_CLR);
    /* Writes the mode value 4*/
    ULP_SPI_MEM_MAP(BOD_COMP_MODE_REG) |= (MANUAL_CMP_MUX_SEL_VALUE);

    /* Converts threshold value to float voltage value */
    threshold_f = ((vbatt_threshold / BOD_VBATT_COMPUT_FIRST_ELEMENT)
                   * (BOD_VBATT_COMPUT_SECOND_ELEMENT + BOD_VBATT_COMPUT_THIRD_ELEMENT))
                  - BOD_VBATT_COMPUT_SECOND_ELEMENT;
    /* Converts the thershold value to integer value */
    threshold_i = (float)((vbatt_threshold / BOD_VBATT_COMPUT_FIRST_ELEMENT)
                          * (BOD_VBATT_COMPUT_SECOND_ELEMENT + BOD_VBATT_COMPUT_THIRD_ELEMENT))
                  - BOD_VBATT_COMPUT_SECOND_ELEMENT;
    /* Round off to near of integer value */
    if (threshold_f - (float)threshold_i > (float)THRESHOLD_OFFSET_VALUE)
      /* if the decimal value is greater than 0.5 then roundoff to next integer value  */
      threshold_i += THRESHOLD_ROUNDUP_VALUE;
    /* Clear the batt scale factor bits */
    ULP_SPI_MEM_MAP(BOD_COMP_SEL_REG) &= ~(BATT_SCALE_FACTOR_BITS);
    /* ORed with threshold value	 */
    ULP_SPI_MEM_MAP(BOD_COMP_SEL_REG) |= (threshold_i << 1);
    /* To give the BOD interrupt( to NPSS) even if the voltage is less than brown out interrupt threshold */
    ULP_SPI_MEM_MAP(BOD_VBATT_STATUS_REG) &= ~(0x3E00);
  }
  /* To disable BOD */
  /*Checks to disable manual mode or automatic mode */
  else if ((ULP_SPI_MEM_MAP(BOD_COMP_MODE_REG) & (MANUAL_CMP_MUX_SEL_BOD_CLR)) == (MANUAL_CMP_MUX_SEL_VALUE)) {
    /* Disable manual mode */
    ULP_SPI_MEM_MAP(BOD_COMP_MODE_REG) &= ~(BIT(20));
    for (button_loop = 0; button_loop <= delay_button; button_loop++) {
      /* Enables the  the button wake up */
      ULP_SPI_MEM_MAP(BOD_COMP_SEL_REG) &= ~(BIT(16));
    }
  } else
    for (button_loop = 0; button_loop <= delay_button; button_loop++) {
      /* Disable signal for bod detection */
      ULP_SPI_MEM_MAP(BOD_COMP_SEL_REG) &= ~(BIT(16));
    }
}

/*==============================================*/
/** 
 * @fn        error_t RSI_BOD_SetMode(uint8_t mode)
 * @brief     This API is used to  set the mode of BOD.
 * @param[in] mode : 1: enable automatic mode .
 *                   0: enable manual mode . 
 * @return    RSI_OK on success 
 *            error code on failure .            
 */

error_t RSI_BOD_SetMode(uint8_t mode)
{
  if (mode >= MAX_MODE_VALUE) {
    return INVALID_PARAMETERS;
  }
  /* read back synced reset with 32KHz fsm clock  */
  while ((!(ULP_SPI_MEM_MAP(BOD_BUTTON_REG) & BIT(21)))) {
  };
  if (mode == AUTOMATIC_BOD) {
    /*Disables the manual mode  */
    ULP_SPI_MEM_MAP(BOD_COMP_MODE_REG) &= ~(BIT(20));
    /* Enables the automatic mode */
    ULP_SPI_MEM_MAP(BOD_COMP_MODE_REG) |= BIT(21); //1E0
  } else {
    /*Disable the automatic mode */
    ULP_SPI_MEM_MAP(BOD_COMP_MODE_REG) &= ~(BIT(21));
    /*Enables the manual mode*/
    ULP_SPI_MEM_MAP(BOD_COMP_MODE_REG) |= BIT(20);
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn       uint32_t RSI_BOD_GetThreshold(void)
 * @brief    This API is used to get the threshold value 
 * @return   Returns the threshold value
 */

uint32_t RSI_BOD_GetThreshold(void)
{
  /* return the threshold value */
  return ((ULP_SPI_MEM_MAP(BOD_COMP_SEL_REG) & BATT_SCALE_FACTOR_BITS) >> 1);
}

/*==============================================*/
/**
 * @fn        error_t RSI_BOD_ConfigSlotValue(uint16_t slot_value) 
 * @brief     This API is used to configure the slot value for automatic BOD .
 * @param     slot_value : Slot value after which compactor  outputs are sampled in auto mode.
 *                          The lowest possible values for this parameter 1.
 *                           The highest possible values for this parameter 32767.
 *            Example:
 *                if slot_value = '1' that means it will count 5 32kHz clock pulses to sample the next battery level.
 *                slot_value = '2' that means it will count (2*5)  32kHz clock pulses to sample the next battery level.
 * @return    RSI_OK on success 
 *            error code on failure . 
 */

error_t RSI_BOD_ConfigSlotValue(uint16_t slot_value)
{
  if (slot_value == SLOT_MIN_VALUE | slot_value > SLOT_MAX_VALUE) {
    return INVALID_PARAMETERS;
  }
  /* To clear the slot value bits */
  ULP_SPI_MEM_MAP(BOD_COMP_MODE_REG) &= ~(SLOT_VALUE_BITS); // 1E0
  /* ORed with slotvalue */
  (ULP_SPI_MEM_MAP(BOD_COMP_MODE_REG) |= (slot_value << 1)); //1E0
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           void RSI_BOD_ButtonWakeUpEnable(uint8_t enable)
 * @brief        This API is used to enable Button wake up 
 * @param[in]    enable: 1: enables  the BOD.
 *                       0: disables  the BOD.
 * @return       none
 */

void RSI_BOD_ButtonWakeUpEnable(uint8_t enable)
{

  volatile uint32_t delay_button = 0, button_loop = 0, division_factor = 0;
  if (BOD_ULP_PROC_CLK_SEL == 0) {
    delay_button = (16 / BOD_CLOCK_DIVISON_FACTOR) + 1;
  } else {
    delay_button = 16;
  }

  if (enable == 1) {
    /*To set the UULPSS GPIO[2] in analog mode (TO GIVE VOLTAGE SUPPLY THROUGH PIN) */
    (NPSS_GPIO_2_ANALOG_MODE) |= NPSS_GPIO_2_ANALOG_MODE_VALUE;
    /* clear the button range values */
    ULP_SPI_MEM_MAP(BOD_BUTTON_REG) &= (BUTTON_RANGE_VALUE_CLR);
    /*button-1 2(1.95v) to 8,button-2 12(1.53V) to 18 and button-3 22(1.26v) to 28  */
    ULP_SPI_MEM_MAP(BOD_BUTTON_REG) |= (BUTTON_RANGE_VALUE);
    for (button_loop = 0; button_loop <= delay_button; button_loop++) {
      /* Enables the  the button wake up */
      ULP_SPI_MEM_MAP(BOD_COMP_SEL_REG) |= BIT(21); // 1E1
    }
    /* Selecting and fixing the inputs of comparator */
    (ULP_SPI_MEM_MAP(BOD_COMP_MODE_REG)) &= ~(MANUAL_CMP_MUX_SEL_BUTTON_CLR);

    /* write the comp mux value 5 for button mode  */
    (ULP_SPI_MEM_MAP(BOD_COMP_MODE_REG) |= MANUAL_CMP_MUX_SEL_BUTTON_VALUE);

  }
  /* check for button enable */
  else if ((ULP_SPI_MEM_MAP(BOD_COMP_MODE_REG) & (MANUAL_CMP_MUX_SEL_BUTTON_CLR))
           == (MANUAL_CMP_MUX_SEL_BUTTON_VALUE)) {
    /*disables the manual bod */
    ULP_SPI_MEM_MAP(BOD_COMP_MODE_REG) &= ~(BIT(20));
    /* disables button mode */
    for (button_loop = 0; button_loop <= delay_button; button_loop++) {
      /* Enables the  the button wake up */
      ULP_SPI_MEM_MAP(BOD_COMP_SEL_REG) &= ~(BIT(21));
    }
  }

  else
    /* disables button mode */
    for (button_loop = 0; button_loop <= delay_button; button_loop++) {
      /* Enables the  the button wake up */
      ULP_SPI_MEM_MAP(BOD_COMP_SEL_REG) &= ~(BIT(21)); // 1E1
    }
}

/*==============================================*/
/**
 * @fn      uint32_t RSI_BOD_Buttonvalue(void)
 * @brief   This API is used to get the  Button value
 * @return  return the button value
 */

uint32_t RSI_BOD_Buttonvalue(void)
{
  volatile static uint32_t buton_value = 0;
  /* To read which button has set to one */
  buton_value = (((ULP_SPI_MEM_MAP(BOD_VBATT_STATUS_REG)) & (READ_BUTTON_VALUE_BITS)) >> 16);
  if (buton_value == BUTTON_THREE_VALUE)
    return BUTTON_THREE;
  else if (buton_value == BUTTON_TWO_VALUE)
    return BUTTON_TWO;
  else if (buton_value == BUTTON_ONE_VALUE)
    return BUTTON_ONE;
  else
    return 0;
}

/*==============================================*/
/**
 * @fn        error_t RSI_BOD_CMP_Hysteresis(uint8_t value)
 * @brief     This API used to set the comparator hysteresis
 * @param[in] value : hysteresis value bits 
 * @return    RSI_OK on success
 *            error code on failure 
 */

error_t RSI_BOD_CMP_Hysteresis(uint8_t value)
{
  if (value > 3) {
    return INVALID_PARAMETERS;
  }
  /*Clears the cmp hysteresis value bits*/
  ULP_SPI_MEM_MAP(BOD_COMP_SEL_REG) &= ~(BOD_COMP_SEL_REG_CLR);
  /* writes the hysteresis value  */
  ULP_SPI_MEM_MAP(BOD_COMP_SEL_REG) |= value;
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn        error_t RSI_BOD_BodTestSel(uint8_t enable, uint8_t bod_test_sel_value)
 * @brief     This API is used to test mux the bod compartor output
 * @param[in] enable : enable to connect GPIO
 * @param[in] bod_test_sel_value : bod test select value bits
 * @return    RSI_OK on success
 *            error code on failure
 */

error_t RSI_BOD_BodTestSel(uint8_t enable, uint8_t bod_test_sel_value)
{
  if (enable == 1) {
    /* To enable test mux to connect to GPIO pad*/
    ULP_SPI_MEM_MAP(BOD_VBATT_STATUS_REG) |= BIT(21);

    if (bod_test_sel_value > 3) {
      return INVALID_PARAMETERS;
    }
    /* clear the bod_test_sel_value bits  */
    ULP_SPI_MEM_MAP(BOD_VBATT_STATUS_REG) &= ~(BOD_TEST_SEL_BITS_CLR);
    /* write the value to bod_test_sel_value (3 TO SET AS OUTPUT )*/
    ULP_SPI_MEM_MAP(BOD_VBATT_STATUS_REG) |= (bod_test_sel_value << 19);

    /* clear the as_bod_test_mux_out_vbatt TO SEE THE OUTPUT THROUGH ULP GPIO[8]*/
    *(volatile uint32_t *)(0x2405A50C) &= ~(0x3E0000);
    /*write the value into as_bod_test_mux_out_vbatt TO SEE THE OUTPUT THROUGH ULP GPIO[8] */
    *(volatile uint32_t *)(0x2405A50C) |= BIT(21);

  } else
    /*Disable BOD test mux */
    ULP_SPI_MEM_MAP(BOD_VBATT_STATUS_REG) &= ~(BIT(21));
  return RSI_OK;
}

/*==============================================*/
/** 
 * @fn         void RSI_BOD_BlackOutReset(uint16_t enable)
 * @brief      This API is used to enable/disable the black out reset of BOD.
 * @param[in]  enable: 1: enables  the blackout reset.
 *                     0: disables  the black out reset.
 * @return    none
 */

void RSI_BOD_BlackOutReset(uint16_t enable)
{
  if (enable == ENABLE)
    /* Enables black out in active state and when brown out is detected.*/
    ULP_SPI_MEM_MAP(BOD_VBATT_STATUS_REG) |= BIT(14);
  else
    /* Disables black out in active state and when brown out is detected.*/
    ULP_SPI_MEM_MAP(BOD_VBATT_STATUS_REG) &= ~(BIT(14));
}

/*==============================================*/
/**
 * @fn         void RSI_BOD_BGSampleEnable(void)
 * @brief      This API is used to enable the blackout reset in sleep mode 
 * @return     none
 */

void RSI_BOD_BGSampleEnable(void)
{
  /* enable the blackout in sleep mode */
  BGSAMPLE = ENABLE_IN_SLEEP_MODE;
}

/*==============================================*/
/**
 * @fn         void RSI_BOD_BGSampleDisable(void)
 * @brief      This API is used to enable the blackout reset in sleep mode 
 * @return     none
 */

void RSI_BOD_BGSampleDisable(void)
{
  /* disable the blackout in sleep mode */
  BGSAMPLE = DISABLE_IN_SLEEP_MODE;
}

/*==============================================*/
/**
 * @fn        void RSI_BOD_IntrEnable(uint16_t enable) 
 * @brief     This API is used to enable/disable the BOD interrupt
 * @param     enable:    1: enables  the interrupt .
 *                       0: disables  the  interrupt .
 * @return    none   
 */

void RSI_BOD_IntrEnable(uint16_t enable)
{
  if (enable == ENABLE)
    /* enables the bod interrupt */
    NPSS_INTR_MASK_CLR_REG = NPSS_TO_MCU_BOD_INTR;
  else
    /*disables the bod interrupt */
    NPSS_INTR_MASK_SET_REG = NPSS_TO_MCU_BOD_INTR;
}

/*==============================================*/
/**
 * @fn         void RSI_BOD_ButtonIntrEnable(uint16_t enable)
 * @brief      This API is used to enable/disable the Button interrupt
 * @param      enable:    1: enables  the interrupt .
 *                        0: disables  the  interrupt .
 * @return     none
 */

void RSI_BOD_ButtonIntrEnable(uint16_t enable)
{
  if (enable == ENABLE)
    /*enable the button wake up interrupt */
    NPSS_INTR_MASK_CLR_REG = NPSS_TO_MCU_BUTTON_INTR;
  else
    /*disables the bod interrupt */
    NPSS_INTR_MASK_SET_REG = NPSS_TO_MCU_BUTTON_INTR;
}

/*==============================================*/
/**
 * @fn         void RSI_BOD_IntrClr(void) 
 * @brief      This API is used to clear the BOD interrupt
 * @return     none
 */

void RSI_BOD_IntrClr(void)
{
  /* clear the bod interrupt */
  MCU_FSM->MCU_FSM_WAKEUP_STATUS_CLEAR = COMP5_BASED_WAKEUP_STATUS_CLEAR;
  NPSS_INTR_CLEAR_REG                  = NPSS_TO_MCU_BOD_INTR;
}

/*==============================================*/
/**
 * @fn        void RSI_BOD_ButtonIntrClr(void) 
 * @brief     This API is used to clear the Button interrupt
 * @return    none
 */

void RSI_BOD_ButtonIntrClr(void)
{
  /* clear the button  interrupt */
  MCU_FSM->MCU_FSM_WAKEUP_STATUS_CLEAR = COMP6_BASED_WAKEUP_STATUS_CLEAR;
  NPSS_INTR_CLEAR_REG                  = NPSS_TO_MCU_BUTTON_INTR;
}

/*==============================================*/
/**
 * @fn         uint32_t RSI_BOD_GetIntrStatus(void)
 * @brief      This API is used to get status of the BOD interrupt
 * @return     return NPSS_INTR_STATUS_REG on success
 */

uint32_t RSI_BOD_GetIntrStatus(void)
{
  /*returns the interrupt status */
  return NPSS_INTR_STATUS_REG;
}
