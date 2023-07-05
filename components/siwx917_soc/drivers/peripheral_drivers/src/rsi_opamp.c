/*******************************************************************************
* @file  rsi_opamp.c
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

// Include files

#include "rsi_chip.h"

/*==============================================*/
/**   
 * @fn          void RSI_OPAMP1_UGB(uint8_t vin_p_sel,uint8_t enable,uint8_t lp_mode,uint8_t out_mux_en,uint8_t out_mux_sel,
 *                                  uint8_t dyn_en,uint8_t channel)
 * @brief       This API is used to configure the opamp1 in Unity gain buffer
 * @param[in]   vin_p_sel   : input selection (0 - ULP_GPIO_12,1 - ULP_GPIO_7,2 - ULP_GPIO_0,3 -ULP_GPIO_2,4 -ULP_GPIO_6,5 -ULP_GPIO_8,6 - AUX_DAC_OUT)
 * @param[in]   enable      : opamp1 enable (1 - enable ,0 - disable)           
 * @param[in]   lp_mode     : opamp1 low power mode(1 - enable ,0 - disable)           
 * @param[in]   out_mux_en  : opamp1 output mux enable(1 - enable ,0 - disable)           
 * @param[in]   out_mux_sel : opamp1 output mux selection(0 - ULP_GPIO_4,1 - ULP_GPIO_15)           
 * @param[in]   dyn_en      : opamp1 dynamic mode enable(1 - enable ,0 - disable)                       
 * @param[in]   channel     : channel no to set in dynamic mode           
 * @return      none
 */
void RSI_OPAMP1_UGB(uint8_t vin_p_sel,
                    uint8_t enable,
                    uint8_t lp_mode,
                    uint8_t out_mux_en,
                    uint8_t out_mux_sel,
                    uint8_t dyn_en,
                    uint8_t channel)
{
  OPAMP_CONFIG_T Config;
  Config.opamp1.opamp1_dyn_en         = dyn_en;
  Config.opamp1.opamp1_sel_p_mux      = vin_p_sel;
  Config.opamp1.opamp1_sel_n_mux      = 4;
  Config.opamp1.opamp1_out_mux_en     = out_mux_en;
  Config.opamp1.opamp1_out_mux_sel    = out_mux_sel;
  Config.opamp1.opamp1_res_to_out_vdd = 0;
  Config.opamp1.opamp1_res_mux_sel    = 0;
  Config.opamp1.opamp1_en_res_bank    = 0;
  Config.opamp1.opamp1_r2_sel         = 0;
  Config.opamp1.opamp1_r1_sel         = 1;
  Config.opamp1.opamp1_lp_mode        = lp_mode;
  Config.opamp1.opamp1_enable         = enable;

  RSI_OPAMP1_Config(OPAMP, channel, &Config);
}

/*==============================================*/
/**   
 * @fn          void RSI_OPAMP2_UGB(uint8_t vin_p_sel,uint8_t enable,uint8_t lp_mode,uint8_t out_mux_en,
 *                                  uint8_t dyn_en,uint8_t channel)
 * @brief       This API is used to configure the opamp2 in Unity gain buffer 
 * @param[in]   vin_p_sel   : input selection (0 - ULP_GPIO_11,1 - ULP_GPIO_5,3 - AUX_DAC_OUT,6 - OPAMP1_OUT)
 * @param[in]   enable      : opamp2 enable (1 - enable ,0 - disable)           
 * @param[in]   lp_mode     : opamp2 low power mode(1 - enable ,0 - disable)           
 * @param[in]   out_mux_en  : opamp2 output mux enable(1 -opamp2 output on ULP_GPIO_9)           
 * @param[in]   dyn_en      : opamp2 dynamic mode enable(1 - enable ,0 - disable)                       
 * @param[in]   channel     : channel no to set in dynamic mode           
 * @return      none
 */
void RSI_OPAMP2_UGB(uint8_t vin_p_sel,
                    uint8_t enable,
                    uint8_t lp_mode,
                    uint8_t out_mux_en,
                    uint8_t dyn_en,
                    uint8_t channel)
{
  OPAMP_CONFIG_T Config;
  Config.opamp2.opamp2_dyn_en         = dyn_en;
  Config.opamp2.opamp2_sel_p_mux      = vin_p_sel;
  Config.opamp2.opamp2_sel_n_mux      = 3;
  Config.opamp2.opamp2_out_mux_en     = out_mux_en;
  Config.opamp2.opamp2_res_to_out_vdd = 0;
  Config.opamp2.opamp2_res_mux_sel    = 0;
  Config.opamp2.opamp2_en_res_bank    = 0;
  Config.opamp2.opamp2_r2_sel         = 0;
  Config.opamp2.opamp2_r1_sel         = 1;
  Config.opamp2.opamp2_lp_mode        = lp_mode;
  Config.opamp2.opamp2_enable         = enable;

  RSI_OPAMP2_Config(OPAMP, channel, &Config);
}

/*==============================================*/
/**   
 * @fn          void RSI_OPAMP3_UGB(uint8_t vin_p_sel,uint8_t enable,uint8_t lp_mode,uint8_t out_mux_en,
 *                                  uint8_t dyn_en,uint8_t channel)
 * @brief       This API is used to configure the opamp3 in Unity gain buffer 
 * @param[in]   vin_p_sel   : input selection (0 - ULP_GPIO_10,1 - ULP_GPIO_14,2 -AUX_DAC_OUT,5 -OPAMP2_OUT)
 * @param[in]   enable      : opamp3 enable (1 - enable ,0 - disable)           
 * @param[in]   lp_mode     : opamp3 low power mode(1 - enable ,0 - disable)           
 * @param[in]   out_mux_en  : opamp3 output mux enable(GPIO27)(1 - enable ,0 - disable)           
 * @param[in]   dyn_en      : opamp3 dynamic mode enable(1 - enable ,0 - disable)                       
 * @param[in]   channel     : channel no to set in dynamic mode           
 * @return      none
 */
void RSI_OPAMP3_UGB(uint8_t vin_p_sel,
                    uint8_t enable,
                    uint8_t lp_mode,
                    uint8_t out_mux_en,
                    uint8_t dyn_en,
                    uint8_t channel)
{
  OPAMP_CONFIG_T Config;
  Config.opamp3.opamp3_dyn_en         = dyn_en;
  Config.opamp3.opamp3_sel_p_mux      = vin_p_sel;
  Config.opamp3.opamp3_sel_n_mux      = 3;
  Config.opamp3.opamp3_out_mux_en     = out_mux_en;
  Config.opamp3.opamp3_res_to_out_vdd = 0;
  Config.opamp3.opamp3_res_mux_sel    = 0;
  Config.opamp3.opamp3_en_res_bank    = 0;
  Config.opamp3.opamp3_r2_sel         = 0;
  Config.opamp3.opamp3_r1_sel         = 1;
  Config.opamp3.opamp3_lp_mode        = lp_mode;
  Config.opamp3.opamp3_enable         = enable;

  RSI_OPAMP3_Config(OPAMP, channel, &Config);
}

/*==============================================*/
/**   
 * @fn          void RSI_OPAMP1_TIA(uint8_t vref_sel,uint8_t iin_sel,uint8_t enable,uint8_t lp_mode,
 *                                  uint8_t r2_sel,uint8_t out_mux_en,uint8_t out_mux_sel,uint8_t dyn_en,uint8_t channel)
 * @brief       This API is used to configure the opamp1 in Trans Impedance Amplifier 
 * @param[in]   vref_sel   : ref voltage gpio selection (0 - ULP_GPIO_12,1 - ULP_GPIO_7,2 - ULP_GPIO_0,3 -ULP_GPIO_2,4 -ULP_GPIO_6,5 -ULP_GPIO_8,6 - AUX_DAC_OUT)
 * @param[in]   iin_sel    : input current gpio selection (0 - ULP_GPIO_12,1 - ULP_GPIO_7,2 - ULP_GPIO_0,3 -ULP_GPIO_2,4 -ULP_GPIO_6,5 -ULP_GPIO_8)
 * @param[in]   r2_sel     : selection of resistor r2(0 - 20kohm,1 - 30kohm,2 - 40kohm,3- 60kohm,4 - 120kohm,5 - 250kohm,6 - 500kohm,7 - 1000kohm)
 * @param[in]   enable     : opamp1 enable (1 - enable ,0 - disable)           
 * @param[in]   lp_mode    : opamp1 low power mode(1 - enable ,0 - disable)           
 * @param[in]   out_mux_en : opamp1 output mux enable(1 - enable ,0 - disable)           
 * @param[in]   out_mux_sel: opamp1 output mux selection(0 - ULP_GPIO_4,1 - ULP_GPIO_15)           
 * @param[in]   dyn_en     : opamp1 dynamic mode enable(1 - enable ,0 - disable)                       
 * @param[in]   channel    : channel no to set in dynamic mode           
 * @return      none
 */
void RSI_OPAMP1_TIA(uint8_t vref_sel,
                    uint8_t iin_sel,
                    uint8_t enable,
                    uint8_t lp_mode,
                    uint8_t r2_sel,
                    uint8_t out_mux_en,
                    uint8_t out_mux_sel,
                    uint8_t dyn_en,
                    uint8_t channel)
{
  OPAMP_CONFIG_T Config;
  Config.opamp1.opamp1_dyn_en         = dyn_en;
  Config.opamp1.opamp1_sel_p_mux      = vref_sel;
  Config.opamp1.opamp1_sel_n_mux      = 3;
  Config.opamp1.opamp1_out_mux_en     = out_mux_en;
  Config.opamp1.opamp1_out_mux_sel    = out_mux_sel;
  Config.opamp1.opamp1_res_to_out_vdd = 0;
  Config.opamp1.opamp1_res_mux_sel    = iin_sel;
  Config.opamp1.opamp1_en_res_bank    = 1;
  Config.opamp1.opamp1_r2_sel         = r2_sel;
  Config.opamp1.opamp1_r1_sel         = 0;
  Config.opamp1.opamp1_lp_mode        = lp_mode;
  Config.opamp1.opamp1_enable         = enable;

  RSI_OPAMP1_Config(OPAMP, channel, &Config);
}

/*==============================================*/
/**   
 * @fn          void RSI_OPAMP2_TIA(uint8_t vref_sel,uint8_t iin_sel,uint8_t enable,uint8_t lp_mode,
 *                                  uint8_t r2_sel,uint8_t out_mux_en,uint8_t dyn_en,uint8_t channel)
 * @brief       This API is used to configure the opamp2 in Trans Impedance Amplifier 
 * @param[in]   vref_sel   : ref voltage  gpio selection (0 - ULP_GPIO_11,1 - ULP_GPIO_5,3 - AUX_DAC_OUT,6 - OPAMP1_OUT)
 * @param[in]   iin        : input current gpio selection (0 - ULP_GPIO_11,1 - ULP_GPIO_5)
 * @param[in]   enable     : opamp2 enable (1 - enable ,0 - disable)           
 * @param[in]   lp_mode    : opamp2 low power mode(1 - enable ,0 - disable)          
 * @param[in]   r2_sel     : selection of resistor r2(0 - 20kohm,1 - 30kohm,2 - 40kohm,3- 60kohm,4 - 120kohm,5 - 250kohm,6 - 500kohm,7 - 1000kohm) 
 * @param[in]   out_mux_en : opamp2 output mux enable(1 -opamp2 output on ULP_GPIO_9)           
 * @param[in]   dyn_en     : opamp2 dynamic mode enable(1 - enable ,0 - disable)                       
 * @param[in]   channel    : channel no to set in dynamic mode           
 * @return      none
 */
void RSI_OPAMP2_TIA(uint8_t vref_sel,
                    uint8_t iin_sel,
                    uint8_t enable,
                    uint8_t lp_mode,
                    uint8_t r2_sel,
                    uint8_t out_mux_en,
                    uint8_t dyn_en,
                    uint8_t channel)
{
  OPAMP_CONFIG_T Config;
  Config.opamp2.opamp2_dyn_en         = dyn_en;
  Config.opamp2.opamp2_sel_p_mux      = vref_sel;
  Config.opamp2.opamp2_sel_n_mux      = 2;
  Config.opamp2.opamp2_out_mux_en     = out_mux_en;
  Config.opamp2.opamp2_res_to_out_vdd = 0;
  Config.opamp2.opamp2_res_mux_sel    = iin_sel;
  Config.opamp2.opamp2_en_res_bank    = 1;
  Config.opamp2.opamp2_r2_sel         = r2_sel;
  Config.opamp2.opamp2_r1_sel         = 0;
  Config.opamp2.opamp2_lp_mode        = lp_mode;
  Config.opamp2.opamp2_enable         = enable;

  RSI_OPAMP2_Config(OPAMP, channel, &Config);
}

/*==============================================*/
/**   
 * @fn          void RSI_OPAMP3_TIA(uint8_t vref_sel,uint8_t iin_sel,uint8_t enable,uint8_t lp_mode,
 *                                  uint8_t r2_sel,uint8_t out_mux_en,uint8_t dyn_en,uint8_t channel)
 * @brief       This API is used to configure the opamp3 in Trans Impedance Amplifier 
 * @param[in]   vref_sel   : ref voltage gpio selection (0 - ULP_GPIO_10,1 - ULP_GPIO_14,2 - AUX_DAC_OUT,5 - OPAMP2_OUT)
 * @param[in]   iin        : input current gpio selection (0 - ULP_GPIO_10,1 - ULP_GPIO_14)
 * @param[in]   enable     : opamp3 enable (1 - enable ,0 - disable)           
 * @param[in]   lp_mode    : opamp3 low power mode(1 - enable ,0 - disable)           
 * @param[in]   r2_sel     : selection of resistor r2(0 - 20kohm,1 - 30kohm,2 - 40kohm,3- 60kohm,4 - 120kohm,5 - 250kohm,6 - 500kohm,7 - 1000kohm)
 * @param[in]   out_mux_en : opamp3 output mux enable(1 - enable ,0 - disable)           
 * @param[in]   out_mux_sel: opamp3 output mux selection(TGPIO2)           
 * @param[in]   dyn_en     : opamp3 dynamic mode enable(1 - enable ,0 - disable)                       
 * @param[in]   channel    : channel no to set in dynamic mode           
 * @return      none
 */
void RSI_OPAMP3_TIA(uint8_t vref_sel,
                    uint8_t iin_sel,
                    uint8_t enable,
                    uint8_t lp_mode,
                    uint8_t r2_sel,
                    uint8_t out_mux_en,
                    uint8_t dyn_en,
                    uint8_t channel)
{
  OPAMP_CONFIG_T Config;
  Config.opamp3.opamp3_dyn_en         = dyn_en;
  Config.opamp3.opamp3_sel_p_mux      = vref_sel;
  Config.opamp3.opamp3_sel_n_mux      = 2;
  Config.opamp3.opamp3_out_mux_en     = out_mux_en;
  Config.opamp3.opamp3_res_to_out_vdd = 0;
  Config.opamp3.opamp3_res_mux_sel    = iin_sel;
  Config.opamp3.opamp3_en_res_bank    = 1;
  Config.opamp3.opamp3_r2_sel         = r2_sel;
  Config.opamp3.opamp3_r1_sel         = 0;
  Config.opamp3.opamp3_lp_mode        = lp_mode;
  Config.opamp3.opamp3_enable         = enable;

  RSI_OPAMP3_Config(OPAMP, channel, &Config);
}

/*==============================================*/
/**   
 * @fn          void RSI_OPAMP1_InvPGA(uint8_t vref_sel,uint8_t vin_sel,uint8_t enable,uint8_t lp_mode,
 *                                   uint8_t r1_sel,uint8_t r2_sel,uint8_t out_mux_en,uint8_t out_mux_sel,uint8_t dyn_en,uint8_t channel)
 * @brief       This API is used to configure the opamp1 in Inverting PGA 
 * @param[in]   vref_sel    : ref voltage gpio selection (0 - ULP_GPIO_12,1 - ULP_GPIO_7,2 - ULP_GPIO_0,3 -ULP_GPIO_2,4 -ULP_GPIO_6,5 -ULP_GPIO_8,6 - AUX_DAC_OUT)
 * @param[in]   vin_sel     : input voltage gpio selection (0 - ULP_GPIO_12,1 - ULP_GPIO_7,2 - ULP_GPIO_0,3 -ULP_GPIO_2,4 -ULP_GPIO_6,5 -ULP_GPIO_8,6 - AUX_ADC_DAC,7- GND)
 * @param[in]   enable      : opamp1 enable (1 - enable ,0 - disable)           
 * @param[in]   r1_sel      : selection of resistor r1(0 - 0kohm,1 - 20kohm,2 - 60kohm,3- 140kohm)
 * @param[in]   r2_sel      : selection of resistor r2(0 - 20kohm,1 - 30kohm,2 - 40kohm,3- 60kohm,4 - 120kohm,5 - 250kohm,6 - 500kohm,7 - 1000kohm)
 * @param[in]   lp_mode     : opamp1 low power mode(1 - enable ,0 - disable)           
 * @param[in]   out_mux_en  : opamp1 output mux enable(1 - enable ,0 - disable)           
 * @param[in]   out_mux_sel : opamp1 output mux selection(0 - ULP_GPIO_4,1 - ULP_GPIO_15)           
 * @param[in]   dyn_en      : opamp1 dynamic mode enable(1 - enable ,0 - disable)                       
 * @param[in]   channel     : channel no to set in dynamic mode           
 * @return      none
 */
void RSI_OPAMP1_InvPGA(uint8_t vref_sel,
                       uint8_t vin_sel,
                       uint8_t enable,
                       uint8_t lp_mode,
                       uint8_t r1_sel,
                       uint8_t r2_sel,
                       uint8_t out_mux_en,
                       uint8_t out_mux_sel,
                       uint8_t dyn_en,
                       uint8_t channel)
{
  OPAMP_CONFIG_T Config;
  Config.opamp1.opamp1_dyn_en         = dyn_en;
  Config.opamp1.opamp1_sel_p_mux      = vref_sel;
  Config.opamp1.opamp1_sel_n_mux      = 3;
  Config.opamp1.opamp1_out_mux_en     = out_mux_en;
  Config.opamp1.opamp1_out_mux_sel    = out_mux_sel;
  Config.opamp1.opamp1_res_to_out_vdd = 0;
  Config.opamp1.opamp1_res_mux_sel    = vin_sel;
  Config.opamp1.opamp1_en_res_bank    = 1;
  Config.opamp1.opamp1_r2_sel         = r2_sel;
  Config.opamp1.opamp1_r1_sel         = r1_sel;
  Config.opamp1.opamp1_lp_mode        = lp_mode;
  Config.opamp1.opamp1_enable         = enable;

  RSI_OPAMP1_Config(OPAMP, channel, &Config);
}

/*==============================================*/
/**   
 * @fn          void RSI_OPAMP2_InvPGA(uint8_t vref_sel,uint8_t vin_sel,uint8_t enable,uint8_t lp_mode,
 *                                     uint8_t r1_sel,uint8_t r2_sel,uint8_t out_mux_en,uint8_t dyn_en,uint8_t channel)
 * @brief       This API is used to configure the opamp2 in Inverting PGA 
 * @param[in]   vref_sel  : ref voltage  gpio selection (0 - ULP_GPIO_11,1 - ULP_GPIO_5,3 - AUX_DAC_OUT,6 - OPAMP1_OUT)
 * @param[in]   vin_sel   : input voltage gpio selection (0 - ULP_GPIO_11,1 - ULP_GPIO_5,2 - AUX_DAC_OUT,4 - GND,5 - OPAMP1_OUT)
 * @param[in]   enable    : opamp2 enable (1 - enable ,0 - disable)           
 * @param[in]   lp_mode   : opamp2 low power mode(1 - enable ,0 - disable)           
 * @param[in]   r1_sel    : selection of resistor r1(0 - 0kohm,1 - 20kohm,2 - 60kohm,3- 140kohm)
 * @param[in]   r2_sel    : selection of resistor r2(0 - 20kohm,1 - 30kohm,2 - 40kohm,3- 60kohm,4 - 120kohm,5 - 250kohm,6 - 500kohm,7 - 1000kohm)
 * @param[in]   out_mux_en: opamp2 output mux enable(1 -opamp2 output on ULP_GPIO_9)           
 * @param[in]   dyn_en    : opamp2 dynamic mode enable(1 - enable ,0 - disable)                       
 * @param[in]   channel   : channel no to set in dynamic mode           
 * @return      none
 */
void RSI_OPAMP2_InvPGA(uint8_t vref_sel,
                       uint8_t vin_sel,
                       uint8_t enable,
                       uint8_t lp_mode,
                       uint8_t r1_sel,
                       uint8_t r2_sel,
                       uint8_t out_mux_en,
                       uint8_t dyn_en,
                       uint8_t channel)
{
  OPAMP_CONFIG_T Config;
  Config.opamp2.opamp2_dyn_en         = dyn_en;
  Config.opamp2.opamp2_sel_p_mux      = vref_sel;
  Config.opamp2.opamp2_sel_n_mux      = 2;
  Config.opamp2.opamp2_out_mux_en     = out_mux_en;
  Config.opamp2.opamp2_res_to_out_vdd = 0;
  Config.opamp2.opamp2_res_mux_sel    = vin_sel;
  Config.opamp2.opamp2_en_res_bank    = 1;
  Config.opamp2.opamp2_r2_sel         = r2_sel;
  Config.opamp2.opamp2_r1_sel         = r1_sel;
  Config.opamp2.opamp2_lp_mode        = lp_mode;
  Config.opamp2.opamp2_enable         = enable;

  RSI_OPAMP2_Config(OPAMP, channel, &Config);
}

/*==============================================*/
/**   
 * @fn          void RSI_OPAMP3_InvPGA(uint8_t vref_sel,uint8_t vin_sel,uint8_t enable,uint8_t lp_mode,
 *                                    uint8_t r1_sel,uint8_t r2_sel,uint8_t out_mux_en,uint8_t dyn_en,uint8_t channel)
 * @brief       This API is used to configuration of an opamp3 in Inverting PGA
 * @param[in]   vref_sel  : ref voltage gpio selection (0 - ULP_GPIO_10,1 - ULP_GPIO_14,2 -AUX_DAC_OUT,5 -OPAMP2_OUT)
 * @param[in]   vin_sel   : input voltage gpio selection (0 - ULP_GPIO_10,1 - ULP_GPIO_14,2 - AUX_DAC_OUT,3 - GND,4 - OPAMP2_OUT)
 * @param[in]   enable    : opamp3 enable (1 - enable ,0 - disable)           
 * @param[in]   lp_mode   : opamp3 low power mode(1 - enable ,0 - disable)           
 * @param[in]   r1_sel    : selection of resistor r1(0 - 0kohm,1 - 20kohm,2 - 60kohm,3- 140kohm)
 * @param[in]   r2_sel    : selection of resistor r2(0 - 20kohm,1 - 30kohm,2 - 40kohm,3- 60kohm,4 - 120kohm,5 - 250kohm,6 - 500kohm,7 - 1000kohm)
 * @param[in]   out_mux_en: opamp3 output mux enable(1 - enable ,0 - disable output on GPIO27)           
 * @param[in]   dyn_en    : opamp3 dynamic mode enable(1 - enable ,0 - disable)                       
 * @param[in]   channel   : channel no to set in dynamic mode           
 * @return      none
 */
void RSI_OPAMP3_InvPGA(uint8_t vref_sel,
                       uint8_t vin_sel,
                       uint8_t enable,
                       uint8_t lp_mode,
                       uint8_t r1_sel,
                       uint8_t r2_sel,
                       uint8_t out_mux_en,
                       uint8_t dyn_en,
                       uint8_t channel)
{
  OPAMP_CONFIG_T Config;
  Config.opamp3.opamp3_dyn_en         = dyn_en;
  Config.opamp3.opamp3_sel_p_mux      = vref_sel;
  Config.opamp3.opamp3_sel_n_mux      = 2;
  Config.opamp3.opamp3_out_mux_en     = out_mux_en;
  Config.opamp3.opamp3_res_to_out_vdd = 0;
  Config.opamp3.opamp3_res_mux_sel    = vin_sel;
  Config.opamp3.opamp3_en_res_bank    = 1;
  Config.opamp3.opamp3_r2_sel         = r2_sel;
  Config.opamp3.opamp3_r1_sel         = r1_sel;
  Config.opamp3.opamp3_lp_mode        = lp_mode;
  Config.opamp3.opamp3_enable         = enable;

  RSI_OPAMP3_Config(OPAMP, channel, &Config);
}

/*==============================================*/
/**   
 * @fn          void RSI_OPAMP1_NonInvPGA(uint8_t vin_sel,uint8_t vref_sel,uint8_t enable,uint8_t lp_mode,
 *                                        uint8_t r1_sel,uint8_t r2_sel,uint8_t out_mux_en,uint8_t out_mux_sel,uint8_t dyn_en,uint8_t channel)
 * @brief       This API is used to configure the opamp1 in Non inverting PGA
 * @param[in]   vin_sel    : input voltage gpio selection (0 - ULP_GPIO_12,1 - ULP_GPIO_7,2 - ULP_GPIO_0,3 -ULP_GPIO_2,4 -ULP_GPIO_6,5 -ULP_GPIO_8,6 - AUX_ADC_DAC)
 * @param[in]   vref_sel   : ref voltage gpio selection (0 - ULP_GPIO_12,1 - ULP_GPIO_7,2 - ULP_GPIO_0,3 -ULP_GPIO_2,4 -ULP_GPIO_6,5 -ULP_GPIO_8,6 - AUX_DAC_OUT,7 - GND)
 * @param[in]   enable     : opamp1 enable (1 - enable ,0 - disable)           
 * @param[in]   lp_mode    : opamp1 low power mode(1 - enable ,0 - disable)           
 * @param[in]   r1_sel     : selection of resistor r1(0 - 0kohm,1 - 20kohm,2 - 60kohm,3- 140kohm)
 * @param[in]   r2_sel     : selection of resistor r2(0 - 20kohm,1 - 30kohm,2 - 40kohm,3- 60kohm,4 - 120kohm,5 - 250kohm,6 - 500kohm,7 - 1000kohm)
 * @param[in]   out_mux_en : opamp1 output mux enable(1 - enable ,0 - disable)           
 * @param[in]   out_mux_sel: opamp1 output mux selection(0 - ULP_GPIO_4,1 - ULP_GPIO_15)           
 * @param[in]   dyn_en     : opamp1 dynamic mode enable(1 - enable ,0 - disable)                       
 * @param[in]   channel    : channel no to set in dynamic mode           
 * @return      none
 */
void RSI_OPAMP1_NonInvPGA(uint8_t vin_sel,
                          uint8_t vref_sel,
                          uint8_t enable,
                          uint8_t lp_mode,
                          uint8_t r1_sel,
                          uint8_t r2_sel,
                          uint8_t out_mux_en,
                          uint8_t out_mux_sel,
                          uint8_t dyn_en,
                          uint8_t channel)
{
  OPAMP_CONFIG_T Config;
  Config.opamp1.opamp1_dyn_en         = dyn_en;
  Config.opamp1.opamp1_sel_p_mux      = vin_sel;
  Config.opamp1.opamp1_sel_n_mux      = 3;
  Config.opamp1.opamp1_out_mux_en     = out_mux_en;
  Config.opamp1.opamp1_out_mux_sel    = out_mux_sel;
  Config.opamp1.opamp1_res_to_out_vdd = 0;
  Config.opamp1.opamp1_res_mux_sel    = vref_sel;
  Config.opamp1.opamp1_en_res_bank    = 1;
  Config.opamp1.opamp1_r2_sel         = r2_sel;
  Config.opamp1.opamp1_r1_sel         = r1_sel;
  Config.opamp1.opamp1_lp_mode        = lp_mode;
  Config.opamp1.opamp1_enable         = enable;

  RSI_OPAMP1_Config(OPAMP, channel, &Config);
}

/*==============================================*/
/**   
 * @fn          void RSI_OPAMP2_NonInvPGA(uint8_t vin_sel,uint8_t vref_sel,uint8_t enable,uint8_t lp_mode,
 *                                        uint8_t r1_sel,uint8_t r2_sel,uint8_t out_mux_en,uint8_t dyn_en,uint8_t channel)
 * @brief       This API is used to configure the opamp2 in Non inverting PGA 
 * @param[in]   vin_sel    : input voltage gpio selection (0 - ULP_GPIO_11,1 - ULP_GPIO_5,3 - AUX_DAC_OUT,6 - OPAMP1_OUT)
 * @param[in]   vref_sel   : ref voltage  gpio selection (0 - ULP_GPIO_11,1 - ULP_GPIO_5,3 - AUX_DAC_OUT,4 - GND,5 - OPAMP1_OUT)
 * @param[in]   enable     : opamp2 enable (1 - enable ,0 - disable)           
 * @param[in]   lp_mode    : opamp2 low power mode(1 - enable ,0 - disable)           
 * @param[in]   r1_sel     : selection of resistor r1(0 - 0kohm,1 - 20kohm,2 - 60kohm,3- 140kohm)
 * @param[in]   r2_sel     : selection of resistor r2(0 - 20kohm,1 - 30kohm,2 - 40kohm,3- 60kohm,4 - 120kohm,5 - 250kohm,6 - 500kohm,7 - 1000kohm)
 * @param[in]   out_mux_en : opamp2 output mux enable(1 -opamp2 output on ULP_GPIO_9)           
 * @param[in]   dyn_en     : opamp2 dynamic mode enable(1 - enable ,0 - disable)                       
 * @param[in]   channel    : channel no to set in dynamic mode           
 * @return      none
 */
void RSI_OPAMP2_NonInvPGA(uint8_t vin_sel,
                          uint8_t vref_sel,
                          uint8_t enable,
                          uint8_t lp_mode,
                          uint8_t r1_sel,
                          uint8_t r2_sel,
                          uint8_t out_mux_en,
                          uint8_t dyn_en,
                          uint8_t channel)
{
  OPAMP_CONFIG_T Config;
  Config.opamp2.opamp2_dyn_en         = dyn_en;
  Config.opamp2.opamp2_sel_p_mux      = vin_sel;
  Config.opamp2.opamp2_sel_n_mux      = 2;
  Config.opamp2.opamp2_out_mux_en     = out_mux_en;
  Config.opamp2.opamp2_res_to_out_vdd = 0;
  Config.opamp2.opamp2_res_mux_sel    = vref_sel;
  Config.opamp2.opamp2_en_res_bank    = 1;
  Config.opamp2.opamp2_r2_sel         = r2_sel;
  Config.opamp2.opamp2_r1_sel         = r1_sel;
  Config.opamp2.opamp2_lp_mode        = lp_mode;
  Config.opamp2.opamp2_enable         = enable;

  RSI_OPAMP2_Config(OPAMP, channel, &Config);
}

/*==============================================*/
/**   
 * @fn          void RSI_OPAMP3_NonInvPGA(uint8_t vin_sel,uint8_t vref_sel,uint8_t enable,uint8_t lp_mode,
 *                                        uint8_t r1_sel,uint8_t r2_sel,uint8_t out_mux_en,uint8_t dyn_en,uint8_t channel)
 * @brief       This API is used to configure the opamp3 in Non inverting PGA 
 * @param[in]   vin_sel   : input voltage gpio selection (0 - ULP_GPIO_10,1 - ULP_GPIO_14,2 - AUX_DAC_OUT,5 - OPAMP2_OUT)
 * @param[in]   vref_sel  : ref voltage gpio selection (0 - ULP_GPIO_10,1 - ULP_GPIO_14,2 -AUX_DAC_OUT,3 - GND,4 -OPAMP2_OUT)
 * @param[in]   enable    : opamp3 enable (1 - enable ,0 - disable)           
 * @param[in]   lp_mode   : opamp3 low power mode(1 - enable ,0 - disable)           
 * @param[in]   r1_sel    : selection of resistor r1(0 - 0kohm,1 - 20kohm,2 - 60kohm,3- 140kohm)
 * @param[in]   r2_sel    : selection of resistor r2(0 - 20kohm,1 - 30kohm,2 - 40kohm,3- 60kohm,4 - 120kohm,5 - 250kohm,6 - 500kohm,7 - 1000kohm)
 * @param[in]   out_mux_en: opamp3 output mux enable(1 - enable ,0 - disable output on GPIO27)           
 * @param[in]   dyn_en    : opamp3 dynamic mode enable(1 - enable ,0 - disable)                       
 * @param[in]   channel   : channel no to set in dynamic mode           
 * @return      none
 */
void RSI_OPAMP3_NonInvPGA(uint8_t vin_sel,
                          uint8_t vref_sel,
                          uint8_t enable,
                          uint8_t lp_mode,
                          uint8_t r1_sel,
                          uint8_t r2_sel,
                          uint8_t out_mux_en,
                          uint8_t dyn_en,
                          uint8_t channel)
{
  OPAMP_CONFIG_T Config;
  Config.opamp3.opamp3_dyn_en         = dyn_en;
  Config.opamp3.opamp3_sel_p_mux      = vin_sel;
  Config.opamp3.opamp3_sel_n_mux      = 2;
  Config.opamp3.opamp3_out_mux_en     = out_mux_en;
  Config.opamp3.opamp3_res_to_out_vdd = 0;
  Config.opamp3.opamp3_res_mux_sel    = vref_sel;
  Config.opamp3.opamp3_en_res_bank    = 1;
  Config.opamp3.opamp3_r2_sel         = r2_sel;
  Config.opamp3.opamp3_r1_sel         = r1_sel;
  Config.opamp3.opamp3_lp_mode        = lp_mode;
  Config.opamp3.opamp3_enable         = enable;

  RSI_OPAMP3_Config(OPAMP, channel, &Config);
}

/*==============================================*/
/**   
 * @fn          void RSI_OPAMP1_InvCMP(uint8_t vin_sel,uint8_t vref_sel,uint8_t enable,uint8_t lp_mode,
 *                                     uint8_t r1_sel,uint8_t r2_sel,uint8_t out_mux_en,uint8_t out_mux_sel,uint8_t dyn_en,uint8_t channel)
 * @brief       This API is used to configuration of an opamp1 for inverting comparator with programmable hysteresis 
 * @param[in]   vin_sel   : input voltage gpio selection (0 - ULP_GPIO_12,1 - ULP_GPIO_7,2 -AUX_DAC_OUT)
 * @param[in]   vref_sel  : ref voltage gpio selection (0 - ULP_GPIO_12,1 - ULP_GPIO_7,2 - ULP_GPIO_0,3 -ULP_GPIO_2,4 -ULP_GPIO_6,5 -ULP_GPIO_8,6 - AUX_DAC_OUT,7 - GND)
 * @param[in]   enable    : opamp1 enable (1 - enable ,0 - disable)           
 * @param[in]   lp_mode   : opamp1 low power mode(1 - enable ,0 - disable)           
 * @param[in]   r1_sel    : selection of resistor r1(0 - 0kohm,1 - 20kohm,2 - 60kohm,3- 140kohm)
 * @param[in]   r2_sel    : selection of resistor r2(0 - 20kohm,1 - 30kohm,2 - 40kohm,3- 60kohm,4 - 120kohm,5 - 250kohm,6 - 500kohm,7 - 1000kohm)
 * @param[in]   out_mux_en : opamp1 output mux enable(1 - enable ,0 - disable)           
 * @param[in]   out_mux_sel: opamp1 output mux selection(0 - ULP_GPIO_4,1 - ULP_GPIO_15)           
 * @param[in]   dyn_en     : opamp1 dynamic mode enable(1 - enable ,0 - disable)                       
 * @param[in]   channel    : channel no to set in dynamic mode           
 * @return      none
 */
void RSI_OPAMP1_InvCMP(uint8_t vin_sel,
                       uint8_t vref_sel,
                       uint8_t enable,
                       uint8_t lp_mode,
                       uint8_t r1_sel,
                       uint8_t r2_sel,
                       uint8_t out_mux_en,
                       uint8_t out_mux_sel,
                       uint8_t dyn_en,
                       uint8_t channel)
{
  OPAMP_CONFIG_T Config;
  Config.opamp1.opamp1_dyn_en         = dyn_en;
  Config.opamp1.opamp1_sel_p_mux      = 7;
  Config.opamp1.opamp1_sel_n_mux      = vin_sel;
  Config.opamp1.opamp1_out_mux_en     = out_mux_en;
  Config.opamp1.opamp1_out_mux_sel    = out_mux_sel;
  Config.opamp1.opamp1_res_to_out_vdd = 0;
  Config.opamp1.opamp1_res_mux_sel    = vref_sel;
  Config.opamp1.opamp1_en_res_bank    = 1;
  Config.opamp1.opamp1_r2_sel         = r2_sel;
  Config.opamp1.opamp1_r1_sel         = r1_sel;
  Config.opamp1.opamp1_lp_mode        = lp_mode;
  Config.opamp1.opamp1_enable         = enable;

  RSI_OPAMP1_Config(OPAMP, channel, &Config);
}

/*==============================================*/
/**   
 * @fn          void RSI_OPAMP2_InvCMP(uint8_t vin_sel,uint8_t vref_sel,uint8_t enable,uint8_t lp_mode,
 *                                     uint8_t r1_sel,uint8_t r2_sel,uint8_t out_mux_en,uint8_t dyn_en,uint8_t channel)
 * @brief       This API is used to configuration of an opamp2 for inverting comparator with programmable hysteresis 
 * @param[in]   vin_sel   : input voltage selection (0 - ULP_GPIO_11,1 - AUX_DAC_OUT)
 * @param[in]   vref_sel  : reference voltage selection (0 - ULP_GPIO_11,1 - ULP_GPIO_5,3 - AUX_DAC_OUT,4 - GND,5 - OPAMP1_OUT) 
 * @param[in]   enable    : opamp2 enable (1 - enable ,0 - disable)           
 * @param[in]   lp_mode   : opamp2 low power mode(1 - enable ,0 - disable)           
 * @param[in]   r1_sel    : selection of resistor r1(0 - 0kohm,1 - 20kohm,2 - 60kohm,3- 140kohm)
 * @param[in]   r2_sel    : selection of resistor r2(0 - 20kohm,1 - 30kohm,2 - 40kohm,3- 60kohm,4 - 120kohm,5 - 250kohm,6 - 500kohm,7 - 1000kohm)
 * @param[in]   out_mux_en: opamp2 output mux enable(1 -opamp2 output on ULP_GPIO_9)           
 * @param[in]   dyn_en    : opamp2 dynamic mode enable(1 - enable ,0 - disable)                       
 * @param[in]   channel   : channel no to set in dynamic mode           
 * @return      none
 */
void RSI_OPAMP2_InvCMP(uint8_t vin_sel,
                       uint8_t vref_sel,
                       uint8_t enable,
                       uint8_t lp_mode,
                       uint8_t r1_sel,
                       uint8_t r2_sel,
                       uint8_t out_mux_en,
                       uint8_t dyn_en,
                       uint8_t channel)
{
  OPAMP_CONFIG_T Config;
  Config.opamp2.opamp2_dyn_en         = dyn_en;
  Config.opamp2.opamp2_sel_p_mux      = 4;
  Config.opamp2.opamp2_sel_n_mux      = vin_sel;
  Config.opamp2.opamp2_out_mux_en     = out_mux_en;
  Config.opamp2.opamp2_res_to_out_vdd = 0;
  Config.opamp2.opamp2_res_mux_sel    = vref_sel;
  Config.opamp2.opamp2_en_res_bank    = 1;
  Config.opamp2.opamp2_r2_sel         = r2_sel;
  Config.opamp2.opamp2_r1_sel         = r1_sel;
  Config.opamp2.opamp2_lp_mode        = lp_mode;
  Config.opamp2.opamp2_enable         = enable;

  RSI_OPAMP2_Config(OPAMP, channel, &Config);
}

/*==============================================*/
/**   
 * @fn          void RSI_OPAMP3_InvCMP(uint8_t vin_sel,uint8_t vref_sel,uint8_t enable,uint8_t lp_mode,
 *                                     uint8_t r1_sel,uint8_t r2_sel,uint8_t out_mux_en,uint8_t dyn_en,uint8_t channel)
 * @brief       This API is used to configuration of an opamp3 for inverting comparator with programmable hysteresis 
 * @param[in]   vin_sel   : input voltage gpio selection (0 - ULP_GPIO_10,1 - AUX_DAC_OUT)
 * @param[in]   vref_sel  : ref voltage gpio selection (0 - ULP_GPIO_10,1 - ULP_GPIO_14,2 -AUX_DAC_OUT,3 - GND,4 -OPAMP2_OUT) 
 * @param[in]   enable    : opamp3 enable (1 - enable ,0 - disable)           
 * @param[in]   lp_mode   : opamp3 low power mode(1 - enable ,0 - disable)           
 * @param[in]   r1_sel    : selection of resistor r1(0 - 0kohm,1 - 20kohm,2 - 60kohm,3- 140kohm)
 * @param[in]   r2_sel    : selection of resistor r2(0 - 20kohm,1 - 30kohm,2 - 40kohm,3- 60kohm,4 - 120kohm,5 - 250kohm,6 - 500kohm,7 - 1000kohm)
 * @param[in]   out_mux_en: opamp3 output mux enable(1 - enable ,0 - disable output on GPIO27)            
 * @param[in]   dyn_en    : opamp3 dynamic mode enable(1 - enable ,0 - disable)                       
 * @param[in]   channel   : channel no to set in dynamic mode           
 * @return      none
 */
void RSI_OPAMP3_InvCMP(uint8_t vin_sel,
                       uint8_t vref_sel,
                       uint8_t enable,
                       uint8_t lp_mode,
                       uint8_t r1_sel,
                       uint8_t r2_sel,
                       uint8_t out_mux_en,
                       uint8_t dyn_en,
                       uint8_t channel)
{
  OPAMP_CONFIG_T Config;
  Config.opamp3.opamp3_dyn_en         = dyn_en;
  Config.opamp3.opamp3_sel_p_mux      = 3;
  Config.opamp3.opamp3_sel_n_mux      = vin_sel;
  Config.opamp3.opamp3_out_mux_en     = out_mux_en;
  Config.opamp3.opamp3_res_to_out_vdd = 0;
  Config.opamp3.opamp3_res_mux_sel    = vref_sel;
  Config.opamp3.opamp3_en_res_bank    = 1;
  Config.opamp3.opamp3_r2_sel         = r2_sel;
  Config.opamp3.opamp3_r1_sel         = r1_sel;
  Config.opamp3.opamp3_lp_mode        = lp_mode;
  Config.opamp3.opamp3_enable         = enable;

  RSI_OPAMP3_Config(OPAMP, channel, &Config);
}

/*==============================================*/
/**   
 * @fn          void RSI_OPAMP1_NonInvCMP(uint8_t vin_sel,uint8_t vref_sel,uint8_t enable,uint8_t lp_mode,
 *                                        uint8_t r1_sel,uint8_t r2_sel,uint8_t out_mux_en,uint8_t out_mux_sel,uint8_t dyn_en,uint8_t channel)
 * @brief       This API is used to configuration of an opamp1 for inverting comparator with programmable hysteresis 
 * @param[in]   vin_sel    : input voltage gpio selection (0 - ULP_GPIO_12,1 - ULP_GPIO_7,2 - ULP_GPIO_0,3 - ULP_GPIO_2,4 -ULP_GPIO_6,5 -ULP_GPIO_8,6 - AUX_DAC_OUT)
 * @param[in]   vref_sel   : ref voltage gpio selection (0 - ULP_GPIO_12,1 - ULP_GPIO_7,2 - AUX_DAC_OUT)
 * @param[in]   enable     : opamp1 enable (1 - enable ,0 - disable)           
 * @param[in]   lp_mode    : opamp1 low power mode(1 - enable ,0 - disable)           
 * @param[in]   r1_sel     : selection of resistor r1(0 - 0kohm,1 - 20kohm,2 - 60kohm,3- 140kohm)
 * @param[in]   r2_sel     : selection of resistor r2(0 - 20kohm,1 - 30kohm,2 - 40kohm,3- 60kohm,4 - 120kohm,5 - 250kohm,6 - 500kohm,7 - 1000kohm)
 * @param[in]   out_mux_en : opamp1 output mux enable(1 - enable ,0 - disable)           
 * @param[in]   out_mux_sel: opamp1 output mux selection(0 - ULP_GPIO_4,1 - ULP_GPIO_15)           
 * @param[in]   dyn_en     : opamp1 dynamic mode enable(1 - enable ,0 - disable)                       
 * @param[in]   channel    : channel no to set in dynamic mode           
 * @return      none
 */
void RSI_OPAMP1_NonInvCMP(uint8_t vin_sel,
                          uint8_t vref_sel,
                          uint8_t enable,
                          uint8_t lp_mode,
                          uint8_t r1_sel,
                          uint8_t r2_sel,
                          uint8_t out_mux_en,
                          uint8_t out_mux_sel,
                          uint8_t dyn_en,
                          uint8_t channel)
{
  OPAMP_CONFIG_T Config;
  Config.opamp1.opamp1_dyn_en         = dyn_en;
  Config.opamp1.opamp1_sel_p_mux      = 7;
  Config.opamp1.opamp1_sel_n_mux      = vref_sel;
  Config.opamp1.opamp1_out_mux_en     = out_mux_en;
  Config.opamp1.opamp1_out_mux_sel    = out_mux_sel;
  Config.opamp1.opamp1_res_to_out_vdd = 0;
  Config.opamp1.opamp1_res_mux_sel    = vin_sel;
  Config.opamp1.opamp1_en_res_bank    = 1;
  Config.opamp1.opamp1_r2_sel         = r2_sel;
  Config.opamp1.opamp1_r1_sel         = r1_sel;
  Config.opamp1.opamp1_lp_mode        = lp_mode;
  Config.opamp1.opamp1_enable         = enable;

  RSI_OPAMP1_Config(OPAMP, channel, &Config);
}

/*==============================================*/
/**   
 * @fn          void RSI_OPAMP2_NonInvCMP(uint8_t vin_sel,uint8_t vref_sel,uint8_t enable,uint8_t lp_mode,
 *                                uint8_t r1_sel,uint8_t r2_sel,uint8_t out_mux_en,uint8_t dyn_en,uint8_t channel)
 * @brief       This API is used to configuration of an opamp2 for inverting comparator with programmable hysteresis 
 * @param[in]   vin_sel   : input voltae gpio selection (0 - ULP_GPIO_11,1 - ULP_GPIO_5,3 - AUX_DAC_OUT, 5 - OPAMP1_OUT)
 * @param[in]   vref_sel  : ref voltage gpio selection (0 - ULP_GPIO_11,1 - AUX_DAC_OUT) 
 * @param[in]   enable    : opamp2 enable (1 - enable ,0 - disable)           
 * @param[in]   lp_mode   : opamp2 low power mode(1 - enable ,0 - disable)           
 * @param[in]   r1_sel    : selection of resistor r1(0 - 0kohm,1 - 20kohm,2 - 60kohm,3- 140kohm)
 * @param[in]   r2_sel    : selection of resistor r2(0 - 20kohm,1 - 30kohm,2 - 40kohm,3- 60kohm,4 - 120kohm,5 - 250kohm,6 - 500kohm,7 - 1000kohm)
 * @param[in]   out_mux_en: opamp2 output mux enable(1 -opamp2 output on ULP_GPIO_9)           
 * @param[in]   dyn_en    : opamp2 dynamic mode enable(1 - enable ,0 - disable)                       
 * @param[in]   channel   : channel no to set in dynamic mode           
 * @return      none
 */
void RSI_OPAMP2_NonInvCMP(uint8_t vin_sel,
                          uint8_t vref_sel,
                          uint8_t enable,
                          uint8_t lp_mode,
                          uint8_t r1_sel,
                          uint8_t r2_sel,
                          uint8_t out_mux_en,
                          uint8_t dyn_en,
                          uint8_t channel)
{
  OPAMP_CONFIG_T Config;
  Config.opamp2.opamp2_dyn_en         = dyn_en;
  Config.opamp2.opamp2_sel_p_mux      = 4;
  Config.opamp2.opamp2_sel_n_mux      = vref_sel;
  Config.opamp2.opamp2_out_mux_en     = out_mux_en;
  Config.opamp2.opamp2_res_to_out_vdd = 0;
  Config.opamp2.opamp2_res_mux_sel    = vin_sel;
  Config.opamp2.opamp2_en_res_bank    = 1;
  Config.opamp2.opamp2_r2_sel         = r2_sel;
  Config.opamp2.opamp2_r1_sel         = r1_sel;
  Config.opamp2.opamp2_lp_mode        = lp_mode;
  Config.opamp2.opamp2_enable         = enable;

  RSI_OPAMP2_Config(OPAMP, channel, &Config);
}

/*==============================================*/
/**   
 * @fn          void RSI_OPAMP3_NonInvCMP(uint8_t vin_sel,uint8_t vref_sel,uint8_t enable,uint8_t lp_mode,
 *                                        uint8_t r1_sel,uint8_t r2_sel,uint8_t out_mux_en,uint8_t dyn_en,uint8_t channel)
 * @brief       This API is used to configuration of an opamp3 for inverting comparator with programmable hysteresis 
 * @param[in]   vin_sel    : input voltage gpio selection (0 - ULP_GPIO_10,1 - ULP_GPIO_14,2 - AUX_DAC_OUT,4 - OPAMP2_OUT)
 * @param[in]   vref_sel   : ref voltage gpio selection (0 - ULP_GPIO_10,1 - AUX_DAC_OUT)
 * @param[in]   enable     : opamp3 enable (1 - enable ,0 - disable)           
 * @param[in]   lp_mode    : opamp3 low power mode(1 - enable ,0 - disable)           
 * @param[in]   r1_sel     : selection of resistor r1(0 - 0kohm,1 - 20kohm,2 - 60kohm,3- 140kohm)
 * @param[in]   r2_sel     : selection of resistor r2(0 - 20kohm,1 - 30kohm,2 - 40kohm,3- 60kohm,4 - 120kohm,5 - 250kohm,6 - 500kohm,7 - 1000kohm)
 * @param[in]   out_mux_en : opamp3 output mux enable(1 - enable ,0 - disable)           
 * @param[in]   out_mux_sel: opamp3 output mux selection(TGPIO2)           
 * @param[in]   dyn_en     : opamp3 dynamic mode enable(1 - enable ,0 - disable)                       
 * @param[in]   channel    : channel no to set in dynamic mode           
 * @return      none
 */
void RSI_OPAMP3_NonInvCMP(uint8_t vin_sel,
                          uint8_t vref_sel,
                          uint8_t enable,
                          uint8_t lp_mode,
                          uint8_t r1_sel,
                          uint8_t r2_sel,
                          uint8_t out_mux_en,
                          uint8_t dyn_en,
                          uint8_t channel)
{
  OPAMP_CONFIG_T Config;
  Config.opamp3.opamp3_dyn_en         = dyn_en;
  Config.opamp3.opamp3_sel_p_mux      = 3;
  Config.opamp3.opamp3_sel_n_mux      = vref_sel;
  Config.opamp3.opamp3_out_mux_en     = out_mux_en;
  Config.opamp3.opamp3_res_to_out_vdd = 0;
  Config.opamp3.opamp3_res_mux_sel    = vin_sel;
  Config.opamp3.opamp3_en_res_bank    = 1;
  Config.opamp3.opamp3_r2_sel         = r2_sel;
  Config.opamp3.opamp3_r1_sel         = r1_sel;
  Config.opamp3.opamp3_lp_mode        = lp_mode;
  Config.opamp3.opamp3_enable         = enable;

  RSI_OPAMP3_Config(OPAMP, channel, &Config);
}

/*==============================================*/
/**   
 * @fn          void RSI_OPAMP_InstrAMP(uint8_t vin_p_sel,uint8_t vin_n_sel,uint8_t enable,uint8_t lp_mode,
 *                                      uint8_t r1_sel,uint8_t r2_sel,uint8_t out_mux_en,uint8_t dyn_en,uint8_t channel)
 * @brief       This API is used to configuration of opamp as instrumentation amplifier
 * @param[in]   vin_p_sel   : input selection (0 - ULP_GPIO_12,1 - ULP_GPIO_7,2 - ULP_GPIO_0,3 -ULP_GPIO_2,4 -ULP_GPIO_6,5 -ULP_GPIO_8)
 * @param[in]   vin_n_sel   : input selection (0 - ULP_GPIO_11,1 - ULP_GPIO_5)
 * @param[in]   enable      : opamp1 enable (1 - enable ,0 - disable)           
 * @param[in]   lp_mode     : opamp1 low power mode(1 - enable ,0 - disable)           
 * @param[in]   r1_sel     : selection of resistor r1(0 - 0kohm,1 - 20kohm,2 - 60kohm,3- 140kohm)
 * @param[in]   r2_sel     : selection of resistor r2(0 - 20kohm,1 - 30kohm,2 - 40kohm,3- 60kohm,4 - 120kohm,5 - 250kohm,6 - 500kohm,7 - 1000kohm) 
 * @param[in]   out_mux_en  : opamp output mux enable(1 - enable ,0 - disable output on GPIO27)           
 * @param[in]   dyn_en      : opamp dynamic mode enable(1 - enable ,0 - disable)                       
 * @param[in]   channel     : channel no to set in dynamic mode           
 * @return      none
 */
void RSI_OPAMP_InstrAMP(uint8_t vin_p_sel,
                        uint8_t vin_n_sel,
                        uint8_t enable,
                        uint8_t lp_mode,
                        uint8_t r1_sel,
                        uint8_t r2_sel,
                        uint8_t out_mux_en,
                        uint8_t dyn_en,
                        uint8_t channel)
{
  OPAMP_CONFIG_T Config;
  Config.opamp1.opamp1_dyn_en         = dyn_en;
  Config.opamp1.opamp1_sel_p_mux      = vin_p_sel;
  Config.opamp1.opamp1_sel_n_mux      = 4;
  Config.opamp1.opamp1_out_mux_en     = 0;
  Config.opamp1.opamp1_out_mux_sel    = 0;
  Config.opamp1.opamp1_res_to_out_vdd = 0;
  Config.opamp1.opamp1_res_mux_sel    = 0;
  Config.opamp1.opamp1_en_res_bank    = 0;
  Config.opamp1.opamp1_r2_sel         = 0;
  Config.opamp1.opamp1_r1_sel         = 1;
  Config.opamp1.opamp1_lp_mode        = lp_mode;
  Config.opamp1.opamp1_enable         = enable;

  Config.opamp2.opamp2_dyn_en         = dyn_en;
  Config.opamp2.opamp2_sel_p_mux      = vin_n_sel;
  Config.opamp2.opamp2_sel_n_mux      = 3;
  Config.opamp2.opamp2_out_mux_en     = 0;
  Config.opamp2.opamp2_res_to_out_vdd = 2;
  Config.opamp2.opamp2_res_mux_sel    = 5;
  Config.opamp2.opamp2_en_res_bank    = 1;
  Config.opamp2.opamp2_r2_sel         = r2_sel;
  Config.opamp2.opamp2_r1_sel         = r1_sel;
  Config.opamp2.opamp2_lp_mode        = lp_mode;
  Config.opamp2.opamp2_enable         = enable;

  Config.opamp3.opamp3_dyn_en         = dyn_en;
  Config.opamp3.opamp3_sel_p_mux      = 6;
  Config.opamp3.opamp3_sel_n_mux      = 2;
  Config.opamp3.opamp3_out_mux_en     = out_mux_en;
  Config.opamp3.opamp3_res_to_out_vdd = 0;
  Config.opamp3.opamp3_res_mux_sel    = 4;
  Config.opamp3.opamp3_en_res_bank    = 1;
  Config.opamp3.opamp3_r2_sel         = r2_sel;
  Config.opamp3.opamp3_r1_sel         = r1_sel;
  Config.opamp3.opamp3_lp_mode        = lp_mode;
  Config.opamp3.opamp3_enable         = enable;

  RSI_OPAMP1_Config(OPAMP, channel, &Config);
  RSI_OPAMP2_Config(OPAMP, channel, &Config);
  RSI_OPAMP3_Config(OPAMP, channel, &Config);
}

/*==============================================*/
/**   
 * @fn          void RSI_OPAMP1_Config(AUX_ADC_DAC_COMP_Type *Opamp,uint32_t channel,opamp_config *config)
 * @brief       This API is used to configuration of an opamp1 
 * @param[in]   Opamp   : pointer to opamp
 * @param[in]   channel : channel no to set in dynamic mode           
 * @param[in]   config  : pointer to opamp configuration                       
 * @return      none
 */
void RSI_OPAMP1_Config(OPAMP_Type *Opamp, uint32_t channel, OPAMP_CONFIG_T *config)
{

  if (config->opamp1.opamp1_dyn_en) {
    Opamp->OPAMP_1_b.OPAMP1_DYN_EN = 1;
    AUX_ADC_DAC_COMP->ADC_CH_BIT_MAP_CONFIG[channel].ADC_CH_BIT_MAP_CONFIG_1_b.CHANNEL_BITMAP =
      (config->opamp1.opamp1_out_mux_sel << 29 | config->opamp1.opamp1_sel_p_mux << 25
       | config->opamp1.opamp1_sel_n_mux << 22 | config->opamp1.opamp1_out_mux_en << 21
       | config->opamp1.opamp1_res_to_out_vdd << 20 | config->opamp1.opamp1_res_mux_sel << 17
       | config->opamp1.opamp1_en_res_bank << 16 | config->opamp1.opamp1_r2_sel << 13
       | config->opamp1.opamp1_r1_sel << 11 | config->opamp1.opamp1_lp_mode << 10 | config->opamp1.opamp1_enable << 9);
  } else {
    Opamp->OPAMP_1_b.OPAMP1_EN_RES_BANK    = config->opamp1.opamp1_en_res_bank;
    Opamp->OPAMP_1_b.OPAMP1_INN_SEL        = config->opamp1.opamp1_sel_n_mux;
    Opamp->OPAMP_1_b.OPAMP1_INP_SEL        = config->opamp1.opamp1_sel_p_mux;
    Opamp->OPAMP_1_b.OPAMP1_LP_MODE        = config->opamp1.opamp1_lp_mode;
    Opamp->OPAMP_1_b.OPAMP1_R1_SEL         = config->opamp1.opamp1_r1_sel;
    Opamp->OPAMP_1_b.OPAMP1_R2_SEL         = config->opamp1.opamp1_r2_sel;
    Opamp->OPAMP_1_b.OPAMP1_RES_MUX_SEL    = config->opamp1.opamp1_res_mux_sel;
    Opamp->OPAMP_1_b.OPAMP1_RES_TO_OUT_VDD = config->opamp1.opamp1_res_to_out_vdd;
    Opamp->OPAMP_1_b.OPAMP1_OUT_MUX_EN     = config->opamp1.opamp1_out_mux_en;
    Opamp->OPAMP_1_b.OPAMP1_OUT_MUX_SEL    = config->opamp1.opamp1_out_mux_sel;
    Opamp->OPAMP_1_b.OPAMP1_ENABLE         = config->opamp1.opamp1_enable;
  }
}

/*==============================================*/
/**   
 * @fn          void RSI_OPAMP2_Config(AUX_ADC_DAC_COMP_Type *Opamp,uint32_t channel,opamp_config *config)
 * @brief       This API is used to configuration of an opamp2 
 * @param[in]   Opamp   : pointer to opamp
 * @param[in]   channel : channel no  to set in dynamic mode           
 * @param[in]   config  : pointer to opamp configuration                        
 * @return      none
 */
void RSI_OPAMP2_Config(OPAMP_Type *Opamp, uint32_t channel, OPAMP_CONFIG_T *config)
{

  if (config->opamp2.opamp2_dyn_en) {
    Opamp->OPAMP_2_b.OPAMP2_DYN_EN = 1;
    AUX_ADC_DAC_COMP->ADC_CH_BIT_MAP_CONFIG[channel].ADC_CH_BIT_MAP_CONFIG_1_b.CHANNEL_BITMAP =
      (config->opamp2.opamp2_lp_mode << 31 | config->opamp2.opamp2_enable << 30);
    AUX_ADC_DAC_COMP->ADC_CH_BIT_MAP_CONFIG[channel].ADC_CH_BIT_MAP_CONFIG_2_b.CHANNEL_BITMAP =
      (config->opamp2.opamp2_sel_p_mux << 14 | config->opamp2.opamp2_sel_n_mux << 12
       | config->opamp2.opamp2_out_mux_en << 11 | config->opamp2.opamp2_res_to_out_vdd << 9
       | config->opamp2.opamp2_res_mux_sel << 6 | config->opamp2.opamp2_en_res_bank << 5
       | config->opamp2.opamp2_r2_sel << 2 | config->opamp2.opamp2_r1_sel << 0);
  } else {
    Opamp->OPAMP_2_b.OPAMP2_EN_RES_BANK    = config->opamp2.opamp2_en_res_bank;
    Opamp->OPAMP_2_b.OPAMP2_INN_SEL        = config->opamp2.opamp2_sel_n_mux;
    Opamp->OPAMP_2_b.OPAMP2_INP_SEL        = config->opamp2.opamp2_sel_p_mux;
    Opamp->OPAMP_2_b.OPAMP2_LP_MODE        = config->opamp2.opamp2_lp_mode;
    Opamp->OPAMP_2_b.OPAMP2_R1_SEL         = config->opamp2.opamp2_r1_sel;
    Opamp->OPAMP_2_b.OPAMP2_R2_SEL         = config->opamp2.opamp2_r2_sel;
    Opamp->OPAMP_2_b.OPAMP2_RES_MUX_SEL    = config->opamp2.opamp2_res_mux_sel;
    Opamp->OPAMP_2_b.OPAMP2_RES_TO_OUT_VDD = config->opamp2.opamp2_res_to_out_vdd;
    Opamp->OPAMP_2_b.OPAMP2_OUT_MUX_EN     = config->opamp2.opamp2_out_mux_en;
    Opamp->OPAMP_2_b.OPAMP2_ENABLE         = config->opamp2.opamp2_enable;
  }
}

/*==============================================*/
/**   
 * @fn          void RSI_OPAMP3_Config(AUX_ADC_DAC_COMP_Type *Opamp,uint32_t channel,opamp_config *config)
 * @brief       This API is used to configuration of an opamp3 
 * @param[in]   Opamp   : pointer to opamp
 * @param[in]   channel : channel no to set in dynamic mode           
 * @param[in]   config  : pointer to opamp configuration                        
 * @return      none
 */
void RSI_OPAMP3_Config(OPAMP_Type *Opamp, uint32_t channel, OPAMP_CONFIG_T *config)
{
  if (config->opamp3.opamp3_dyn_en) {
    Opamp->OPAMP_3_b.OPAMP3_DYN_EN = 1;
    AUX_ADC_DAC_COMP->ADC_CH_BIT_MAP_CONFIG[channel].ADC_CH_BIT_MAP_CONFIG_3_b.CHANNEL_BITMAP =
      config->opamp3.opamp3_sel_p_mux;
    AUX_ADC_DAC_COMP->ADC_CH_BIT_MAP_CONFIG[channel].ADC_CH_BIT_MAP_CONFIG_2_b.CHANNEL_BITMAP =
      (config->opamp3.opamp3_sel_n_mux << 30 | config->opamp3.opamp3_out_mux_en << 29
       | config->opamp3.opamp3_res_to_out_vdd << 28 | config->opamp3.opamp3_res_mux_sel << 25
       | config->opamp3.opamp3_en_res_bank << 24 | config->opamp3.opamp3_r2_sel << 21
       | config->opamp3.opamp3_r1_sel << 19 | config->opamp3.opamp3_lp_mode << 18 | config->opamp3.opamp3_enable << 17);

  } else {
    Opamp->OPAMP_3_b.OPAMP3_EN_RES_BANK    = config->opamp3.opamp3_en_res_bank;
    Opamp->OPAMP_3_b.OPAMP3_INN_SEL        = config->opamp3.opamp3_sel_n_mux;
    Opamp->OPAMP_3_b.OPAMP3_INP_SEL        = config->opamp3.opamp3_sel_p_mux;
    Opamp->OPAMP_3_b.OPAMP3_LP_MODE        = config->opamp3.opamp3_lp_mode;
    Opamp->OPAMP_3_b.OPAMP3_R1_SEL         = config->opamp3.opamp3_r1_sel;
    Opamp->OPAMP_3_b.OPAMP3_R2_SEL         = config->opamp3.opamp3_r2_sel;
    Opamp->OPAMP_3_b.OPAMP3_RES_MUX_SEL    = config->opamp3.opamp3_res_mux_sel;
    Opamp->OPAMP_3_b.OPAMP3_RES_TO_OUT_VDD = config->opamp3.opamp3_res_to_out_vdd;
    Opamp->OPAMP_3_b.OPAMP3_OUT_MUX_EN     = config->opamp3.opamp3_out_mux_en;
    Opamp->OPAMP_3_b.OPAMP3_ENABLE         = config->opamp3.opamp3_enable;
  }
}
