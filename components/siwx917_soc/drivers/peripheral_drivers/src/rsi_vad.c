/*******************************************************************************
* @file  rsi_vad.c
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
#include "rsi_ccp_user_config.h"

volatile static VAD_EVENT_T event;
#if 1

/*==============================================*/
/**
 * @fn     VAD_PING_IRQHandler()
 * @brief  VAD IRQ handler , clear the VAD interrupt.   
 * @return none
 */
void VAD_PING_IRQHandler()
{
  RSI_VAD_InterruptClr(VAD, 1);
  event.callb_event(VAD_INTR);
}
#endif

/*==============================================*/
/**
 * @fn         error_t VAD_Init(VAD_SignalEvent_t Event)
 * @brief      This API is used to configure the VAD related parameters.
 * @param[in]  Event      : Register callback event.
 * @return     Execution status - If success
 */
error_t VAD_Init(VAD_SignalEvent_t Event)
{
  // Register callback event
  event.callb_event = Event;
#if defined(CHIP_9118)
  RSI_PS_UlpssPeriPowerUp(ULPSS_PWRGATE_ULP_VAD);
#endif

  if (!(event.clk_config)) {
    // Configure the fast and slow clock for VAD
    RSI_ULPSS_VadClkConfig(ULPCLK, ULP_VAD_32KHZ_RC_CLK, ULP_VAD_32MHZ_RC_CLK, 4);
    event.clk_config = 1U;
  }

  // Select the algorithm and algorithm threshold for VAD
  RSI_VAD_SetAlgorithmThreshold(VAD, VAD_METHOD_ZCR_ACF, VAD_ZCR_THRSHOLD, VAD_ACF_THRSHOLD, 0, 0);

  // Configure the sample per frame and sample per address for VAD
  RSI_VAD_Config(VAD, NUMBER_OF_SAMPLE_IN_FRAME, VAD_2SMPLS_PER_ADDR, 1, VAD_INTREG_SOURCE);

  // Set the start delay and end delay for ACF algorithm
  RSI_VAD_Set_Delay(VAD, VAD_ACF_START, VAD_ACF_END);

  // Set energy threshold value */
  RSI_VAD_FrameEnergyConfig(VAD, VAD_ENERGY_THRSHOLD, 1, 1);

  RSI_VAD_PingPongMemoryAddrConfig(VAD, VAD_SCRT_PAD, 0, 1, 0);

  // Enable Nvic
  NVIC_EnableIRQ(VAD_INTR_PING_IRQn);

  return RSI_OK;
}

/*==============================================*/
/**
 * @fn         int32_t VAD_Process(int16_t *wr_buf, int32_t dc_est)
 * @brief      This API is used to process VAD data.
 * @param[in]  wr_buf      : Input data buffer 
 * @param[in]  dc_est      : dc estimation value of previous 1023 samples.
 * @return     The dc estimation value of current 1023 samples - If success
 */
int32_t VAD_Process(int16_t *wr_buf, int32_t dc_est)
{
  uint8_t energy_status = 0;
  uint16_t index;
  int32_t dc_est1   = 0;
  int16_t temp_data = 0, data_in1 = 0;
  static uint8_t flag = 1;
  RSI_VAD_Stop(VAD);
  dc_est = dc_est >> 10;

  for (index = 0; index < (NUMBER_OF_SAMPLE_IN_FRAME); index++) {
    data_in1 = wr_buf[index];
#if DATA_FROM_INTER_ADC
    if (data_in1 & BIT(11)) {
      data_in1 = data_in1 | (VAD_MASK_VALUE);
    } else {
      data_in1 = data_in1;
    }
#else
    data_in1 = data_in1;
#endif
    dc_est1   = dc_est1 + data_in1;
    temp_data = data_in1 - dc_est;
    temp_data = temp_data << VAD_DIGITAL_GAIN_FAC;
    // fill data in VAD ping memory
    ((*(volatile uint16_t *)(ULP_MEMORY_BASE + VAD_SCRT_PAD + index * 2))) = temp_data;
  }

  if ((dc_est == 0) && (flag == 1)) {
    RSI_VAD_Stop(VAD);
    flag = 0;
  } else {
    for (index = 0; index < (32); index++) {
      // fill data in VAD register
      VAD->VAD_CONF_REG8 = ((*(volatile uint16_t *)(ULP_MEMORY_BASE + VAD_SCRT_PAD + index * 2))) | BIT(10);
    }
    for (index = 0; index < (NUMBER_OF_SAMPLE_IN_FRAME); index++) {
      // fill data in VAD register
      VAD->VAD_CONF_REG8 = ((*(volatile uint16_t *)(ULP_MEMORY_BASE + VAD_SCRT_PAD + index * 2))) | BIT(10);
    }
  }

  energy_status = RSI_VAD_ProccessDone(VAD);

  if (energy_status) {
    // Enable Fast clock for VAD for fast post processing
    RSI_VAD_FastClkEnable(ULP_VAD_32MHZ_RC_CLK, 0);
    event.callb_event(VAD_ENERGY_DETECT);
  }
  return dc_est1;
}

/*==============================================*/
/**
 * @fn         error_t VAD_Deinit(void)
 * @brief      This API is used to deinitialize the VAD related parameters.
 * @return     Execution status - If success
 */
error_t VAD_Deinit(void)
{
  RSI_ULPSS_PeripheralDisable(ULPCLK, ULP_VAD_CLK);
#if defined(CHIP_9118)
  RSI_PS_UlpssPeriPowerDown(ULPSS_PWRGATE_ULP_VAD);
#endif
  event.clk_config = 0U;
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn         void RSI_VAD_PingPongMemoryAddrConfig(RSI_VAD_T *pVAD,
 *                                     uint32_t ping_addr,
 *                                     uint32_t pong_addr,
 *                                     uint8_t ping_enable,
 *                                     uint8_t pong_enable)
 * @brief      This API is used to write the  ulp mem address for pong buffer and ping buffer  
 * @param[in]  pVAD      : Pointer to the VAD_Type structure.
 * @param[in]  ping_addr : 13 bit ulp mem address offset for ping buffer
 * @param[in]  pong_addr : 13 bit ulp mem address offset for pong buffer
 * @param[in]  ping_enable : This parameter enable the ping address configuration.
 * @param[in]  pong_enable : This parameter enable the pong address configuration.
 * @return     none
 */
void RSI_VAD_PingPongMemoryAddrConfig(RSI_VAD_T *pVAD,
                                      uint32_t ping_addr,
                                      uint32_t pong_addr,
                                      uint8_t ping_enable,
                                      uint8_t pong_enable)
{
  if (ping_enable) {
    pVAD->VAD_CONF_REG9_b.PING_ADDR = (ping_addr >> 2);
  }
  if (pong_enable) {
    pVAD->VAD_CONF_REG9_b.PONG_ADDR = (pong_addr >> 2);
  }
}

/*==============================================*/
/**
 * @fn         RSI_VAD_Config()
 * @brief      This API is used to configure the VAD parameter    
 * @param[in]  pVAD                : Pointer to the VAD_Type structure.
 * @param[in]  samples_per_frame   : Number of samples in one processing frame, maximum value is 1023 and
 *                                   default is 512
 * @param[in]  samples_per_address : Number of samples for address \n
 *                                   0 - 4 samples per address
 *                                   1 - 2 Samples per address 
 *                                   2 - 1 Sample per address   
 * @param[in]  fullwidth           : 0 - 12/24 when VAD_REG1_ADDR[21:20]
 *                                   1 - 2 Samples per address
 * @param[in]  datasourceselect    : Source of Data for VAD processing
 *                                   00/10: Internal Register
 *                                   11: ADC as source
 *                                   01: Reserved
 * @return      RSI_OK - If success
 */
error_t RSI_VAD_Config(RSI_VAD_T *pVAD,
                       uint16_t samples_per_frame,
                       uint16_t samples_per_address,
                       bool fullwidth,
                       uint8_t datasourceselect)
{
  if ((samples_per_frame > MAXIMUM_VALUE_1024) || (samples_per_address > MAXIMUM_VALUE_4)
      || (datasourceselect > MAXIMUM_VALUE_4)) {
    return INVALID_PARAMETERS;
  }
  // Set the samples_per_frame and samples per address for VAD
  pVAD->VAD_CONF_REG1_b.SAMPLS_PER_FRAME = samples_per_frame;
  pVAD->VAD_CONF_REG1_b.SMPLS_PER_ADDR   = samples_per_address;
  pVAD->VAD_CONF_REG1_b.FULL_WIDTH       = fullwidth; //need to be review this parameter
                                                      // selecting the Source of Data for VAD processing
  pVAD->VAD_CONF_REG7_b.DATA_SOURCE_SELECT = datasourceselect;
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn         void RSI_VAD_Enable(RSI_VAD_T *pVAD)
 * @brief      This API is used to Enable Processing of VAD
 * @param[in]  pVAD    : Pointer to the VAD_Type structure.
 * @return     none
 */
void RSI_VAD_Enable(RSI_VAD_T *pVAD)
{
  pVAD->VAD_CONF_REG8_b.EN_VAD_PROCESS = 1U;
}

/*==============================================*/
/**
 * @fn         RSI_VAD_InterruptClr()
 * @brief      This API is used to clear the interrupt of VAD
 * @param[in]  pVAD                 : Pointer to the VAD_Type structure.
 * @param[in]  ping_interrupt       : This parameter define which interrupt want to be clear.
 *                                    ping_interrupt = 1 , To clear the VAD ping interrupt.
 *                                    ping_interrupt = 0 , To clear the VAD pong interrupt.
 * @return     none
 */
void RSI_VAD_InterruptClr(RSI_VAD_T *pVAD, uint16_t ping_interrupt)
{
  if (ping_interrupt) {
    // clear the ping interrupt in VAD_CONF_REG9 register
    pVAD->VAD_CONF_REG9_b.PING_INT_CLEAR = 1U;
  } else {
    // clear the pong interrupt in VAD_CONF_REG9 register
    pVAD->VAD_CONF_REG9_b.PONG_INT_CLEAR = 1U;
  }
}

/*==============================================*/
/**
 * @fn         error_t RSI_VAD_SetAlgorithmThreshold(RSI_VAD_T *pVAD,
 *                                     uint16_t algorithm_type,
 *                                     uint32_t zcr_threshold,
 *                                     uint32_t acf_threshold,
 *                                     uint32_t wacf_threshold,
 *                                     VAD_AMDF_THRESHOLD_T *config)
 * @brief      This API is used to set algorithm and threshold value for that algorithm.
 * @param[in]  pVAD           : Pointer to the VAD_Type structure.
 * @param[in]  algorithm_type : Select the algorithm type here refer this #VAD_ALGORITHM_SELECT_T enum.
 *                              pass the specific value for selection of algorithm   
 * @param[in]  zcr_threshold  : This parameter define threshold value for zcr algorithm maximum value is 1023
 *                              and default value is 50.
 * @param[in]  acf_threshold  : This parameter define threshold value for acf algorithm maximum value is 4095
 *                              and default value is 1024.
 * @param[in]  admf_threshold : This parameter define threshold value for acf algorithm maximum value is 4095
 *                              and default value is 1024.
 * @param[in]  wacf_threshold : This parameter define threshold value for wacf_threshold algorithm maximum value is 4095
 *                              and default value is 51.
 * @param[in]  config         : VAD_AMDF_THRESHOLD_T structure veriable, configure this structure
 *                              for AMDF algorithm delay.  
 * @return     RSI_OK - If success
 */
error_t RSI_VAD_SetAlgorithmThreshold(RSI_VAD_T *pVAD,
                                      uint16_t algorithm_type,
                                      uint32_t zcr_threshold,
                                      uint32_t acf_threshold,
                                      uint32_t wacf_threshold,
                                      VAD_AMDF_THRESHOLD_T *config)
{
  if ((algorithm_type > MAXIMUM_VALUE_8) || (zcr_threshold > MAXIMUM_VALUE_1024) || (acf_threshold > MAXIMUM_VALUE_4096)
      || (wacf_threshold > MAXIMUM_VALUE_4096)) {
    return INVALID_PARAMETERS;
  }
  // set the required the algorithm for detection purpose
  pVAD->VAD_CONF_REG7_b.CHOOSE_VAD_METHOD = algorithm_type;

  switch (algorithm_type) {
    case ZCR:
      // write the threshold value to smpls_zero_cross bits
      pVAD->VAD_CONF_REG2_b.SMPLS_ZERO_CROSS = zcr_threshold;
      break;
    case ACF:
      // To clear the threshold_acf bits
      pVAD->VAD_CONF_REG4_b.THRESHOLD_ACF = acf_threshold;
      break;
    case AMDF:
      pVAD->VAD_CONF_REG5_b.THRESHOLD_NULL       = config->null_threshold;
      pVAD->VAD_CONF_REG5_b.THRESHOLD_NULL_COUNT = config->null_threshold_count;
      pVAD->VAD_CONF_REG6_b.THRESHOLD_PEAK       = config->peak_threshold;
      pVAD->VAD_CONF_REG6_b.THRESHOLD_PEAK       = config->peak_threshold_count;
      break;
    case WACF:
      // write the threshold value to smpls_zero_cross bits
      pVAD->VAD_CONF_REG4_b.THRESHOLD_WACF = wacf_threshold;
      break;
    case ZCR_ACF_AMDF_WACF:
      pVAD->VAD_CONF_REG2_b.SMPLS_ZERO_CROSS     = zcr_threshold;
      pVAD->VAD_CONF_REG4_b.THRESHOLD_ACF        = acf_threshold;
      pVAD->VAD_CONF_REG5_b.THRESHOLD_NULL       = config->null_threshold;
      pVAD->VAD_CONF_REG5_b.THRESHOLD_NULL_COUNT = config->null_threshold_count;
      pVAD->VAD_CONF_REG6_b.THRESHOLD_PEAK       = config->peak_threshold;
      pVAD->VAD_CONF_REG6_b.THRESHOLD_PEAK       = config->peak_threshold_count;
      pVAD->VAD_CONF_REG4_b.THRESHOLD_WACF       = wacf_threshold;
      break;
    case ZCR_ACF:
      pVAD->VAD_CONF_REG2_b.SMPLS_ZERO_CROSS = zcr_threshold;
      pVAD->VAD_CONF_REG4_b.THRESHOLD_ACF    = acf_threshold;
      break;
    case ZCR_AMDF:
      pVAD->VAD_CONF_REG2_b.SMPLS_ZERO_CROSS     = zcr_threshold;
      pVAD->VAD_CONF_REG5_b.THRESHOLD_NULL       = config->null_threshold;
      pVAD->VAD_CONF_REG5_b.THRESHOLD_NULL_COUNT = config->null_threshold_count;
      pVAD->VAD_CONF_REG6_b.THRESHOLD_PEAK       = config->peak_threshold;
      pVAD->VAD_CONF_REG6_b.THRESHOLD_PEAK       = config->peak_threshold_count;
      break;
    case ZCR_WACF:
      pVAD->VAD_CONF_REG2_b.SMPLS_ZERO_CROSS = zcr_threshold;
      pVAD->VAD_CONF_REG4_b.THRESHOLD_WACF   = wacf_threshold;
      break;
    default:
      return INVALID_PARAMETERS;
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn         error_t RSI_VAD_Set_Delay(RSI_VAD_T *pVAD, uint16_t startdelayval, uint16_t enddelayval)
 * @brief      This API is used to set start the end delay value for ACF,WACF,AMDF algorithm .
 * @param[in]  pVAD          : Pointer to the VAD_Type structure.
 * @param[in]  startdelayval : This parameter define the start delay value for ACF,WACF,AMDF algorithm.
 *                             maximum value is 1023 and default value is 2
 * @param[in]  enddelayval   : This parameter define the end delay value for ACF,WACF,AMDF algorithm.
 *                             maximum value is 1023 and default value is 16
 * @return     RSI_OK - If success 
 */
error_t RSI_VAD_Set_Delay(RSI_VAD_T *pVAD, uint16_t startdelayval, uint16_t enddelayval)
{
  if ((startdelayval > MAXIMUM_VALUE_1024) || (enddelayval > MAXIMUM_VALUE_1024)) {
    return INVALID_PARAMETERS;
  }
  // start delay
  pVAD->VAD_CONF_REG7_b.START_DELAY_VAL = startdelayval;
  // End delay
  pVAD->VAD_CONF_REG7_b.END_DELAY_VAL = enddelayval;
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn         error_t RSI_VAD_Input(RSI_VAD_T *pVAD, int16_t data)
 * @brief      This API is used to give the input for VAD.
 * @param[in]  pVAD   : Pointer to the VAD_Type structure.
 * @param[in]  data   : This parameter input for VAD block input is 1023 and default value is 16
 * @return     RSI_OK - If success
 */
error_t RSI_VAD_Input(RSI_VAD_T *pVAD, int16_t data)
{
  if (data > MAXIMUM_VALUE_1024) {
    return INVALID_PARAMETERS;
  }
  pVAD->VAD_CONF_REG8_b.EN_VAD_PROCESS = 1U;
  /* Writing the  data used as source for VAD */
  pVAD->VAD_CONF_REG8_b.INP_DATA = data;
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn         error_t RSI_VAD_FrameEnergyConfig(RSI_VAD_T *pVAD,
 *                                 uint32_t threshold_frame_energy,
 *                                 uint32_t threshold_smpl_collect,
 *                                 uint32_t prog_smpls_for_energy_check)
 * @brief      This API is used to configure the frame energy.
 * @param[in]  pVAD                        : Pointer to the VAD_Type structure.
 * @param[in]  threshold_frame_energy      : This parameter give the threshold frame energy,
 *                                           maximum value is 1023 and default value is 0.
 * @param[in]  threshold_smpl_collect      : This parameter give the number of threshold sample collect,
 *                                           maximum value is 1023 and default value is 1.
 * @param[in]  prog_smpls_for_energy_check : This parameter define the sample for energy check,
 *                                           maximum value is 3 and default value is 1.
 * @return     RSI_OK - If success 
 */
error_t RSI_VAD_FrameEnergyConfig(RSI_VAD_T *pVAD,
                                  uint32_t threshold_frame_energy,
                                  uint32_t threshold_smpl_collect,
                                  uint32_t prog_smpls_for_energy_check)
{
  if ((threshold_frame_energy > MAXIMUM_VALUE_1024) || (threshold_smpl_collect > MAXIMUM_VALUE_1024)
      || (prog_smpls_for_energy_check > MAXIMUM_VALUE_4)) {
    return INVALID_PARAMETERS;
  }
  pVAD->VAD_CONF_REG3_b.THRESHOLD_FRAME_ENERGY      = threshold_frame_energy;
  pVAD->VAD_CONF_REG3_b.THRESHOLD_SMPL_COLLECT      = threshold_smpl_collect;
  pVAD->VAD_CONF_REG3_b.PROG_SMPLS_FOR_ENERGY_CHECK = prog_smpls_for_energy_check;
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn         void RSI_VAD_Stop(RSI_VAD_T *pVAD)
 * @brief      This API is used to disable VAD functionality.
 * @param[in]  pVAD   : Pointer to the VAD_Type structure.
 * @return     none
 */
void RSI_VAD_Stop(RSI_VAD_T *pVAD)
{
  pVAD->VAD_CONF_REG8_b.EN_VAD_PROCESS = 0;
}

/*==============================================*/
/**
 * @fn         uint8_t RSI_VAD_ProccessDone(RSI_VAD_T *pVAD)
 * @brief      This API is used show the VAD energy detect.
 * @param[in]  pVAD   : Pointer to the VAD_Type structure.
 * @return     The VAD energy detection status.
 *             If 1 - Energy detect.    
 *             If 0 - No energy detect. 
 */
uint8_t RSI_VAD_ProccessDone(RSI_VAD_T *pVAD)
{
  return pVAD->VAD_CONF_REG8_b.VAD_PROC_DONE;
}

/*==============================================*/
/**
 * @fn         void RSI_VAD_FastClkEnable(uint16_t fast_clk_sel, uint16_t clk_div_factor)
 * @brief      This API is used enable fast clock for VAD peripheral.
 * @param[in]  fast_clk_sel   : fast clock select for VAD peripheral
 * @param[in]  clk_div_factor : Select the clock division factor for VAD peripheral
 * @return     none
 */
void RSI_VAD_FastClkEnable(uint16_t fast_clk_sel, uint16_t clk_div_factor)
{
  ULPCLK->ULP_VAD_CLK_GEN_REG_b.ULP_VAD_FCLK_EN       = 1U;
  ULPCLK->ULP_VAD_CLK_GEN_REG_b.ULP_VAD_FCLK_SEL      = fast_clk_sel;
  ULPCLK->ULP_VAD_CLK_GEN_REG_b.ULP_VAD_CLKDIV_FACTOR = clk_div_factor;
}

/*==============================================*/
/**
 * @fn         int32_t RSI_VAD_ProcessData(RSI_VAD_T *pVAD,
 *                          uint32_t vad_addr,
 *                           uint32_t adc_data_addr,
 *                           int32_t dc_est,
 *                           uint32_t dig_scale,
 *                           uint32_t sample_len)
 * @brief      This API is used process data for input to VAD.
 * @param[in]  pVAD          : Pointer to the VAD_Type structure.
 * @param[in]  vad_addr      : VAD ULPSS memory address
 * @param[in]  adc_data_addr : ADC output data address    
 * @param[in]  dc_est        : dc estimation value.
 * @param[in]  dig_scale     : Scaling the ADC output.
 * @param[in]  sample_len    : Number of samples to process in VAD engine.
 * @return     dc estimation value - If success  
 */
int32_t RSI_VAD_ProcessData(RSI_VAD_T *pVAD,
                            uint32_t vad_addr,
                            uint32_t adc_data_addr,
                            int32_t dc_est,
                            uint32_t dig_scale,
                            uint32_t sample_len)
{
  uint32_t index;
  int32_t dc_est1   = 0;
  int16_t temp_data = 0, data_in1 = 0;
  static uint8_t flag = 1;
  RSI_VAD_Stop(VAD);

  // processing the adc sample
  for (index = 0; index < (sample_len); index++) {
    data_in1 = ((*(volatile uint16_t *)(adc_data_addr + index * 2)));

    if (data_in1 & BIT(11)) {
      data_in1 = data_in1 | (VAD_MASK_VALUE);
    } else {
      data_in1 = data_in1;
    }
    dc_est1   = dc_est1 + data_in1;
    temp_data = data_in1 - dc_est;
    temp_data = temp_data << dig_scale;
    // fill data in VAD ping memory
    ((*(volatile uint16_t *)(ULP_MEMORY_BASE + vad_addr + index * 2))) = temp_data;
  }

  if ((dc_est == 0) && (flag == 1)) {
    RSI_VAD_Stop(VAD);
    flag = 0;
  } else {
    for (index = 0; index < (32); index++) {
      /* fill data in VAD register */
      pVAD->VAD_CONF_REG8 = ((*(volatile uint16_t *)(ULP_MEMORY_BASE + vad_addr + index * 2))) | BIT(10);
    }
    for (index = 0; index < (sample_len); index++) {
      // fill data in VAD register
      pVAD->VAD_CONF_REG8 = ((*(volatile uint16_t *)(ULP_MEMORY_BASE + vad_addr + index * 2))) | BIT(10);
    }
  }
  return dc_est1;
}
