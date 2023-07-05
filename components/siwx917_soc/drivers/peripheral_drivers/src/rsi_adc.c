/*******************************************************************************
* @file  rsi_adc.c
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
#include "UDMA.h"
#include <math.h>

// ADC internal structure
adc_inter_config_t adcInterConfig;
adc_commn_config_t adc_commn_config;

// ADC calibration related variables
volatile uint32_t auxadcCalibValueLoad = 0, auxadcCalibValue = 0;
volatile uint32_t calib_done = 0;

// ADC ping or pong interrupt selection variable
volatile uint8_t pong_enable_sel;

#ifdef CHIP_917
extern dac_config_t dac_callback_fun;
#endif
#ifdef ADC_MULTICHANNEL_WITH_EXT_DMA
extern int16_t adc_output_buf[UDMA_PING_PONG_LENGTH * 2];
// Channel sorting related veriable
uint16_t ch_data[16];
extern adc_commn_config_t adc_commn_config;
extern RSI_UDMA_HANDLE_T udmaHandle1;
// DMA descriptors must be aligned to 16 bytes
#if defined(__CC_ARM)
#ifdef PS2_STATE_ADC_DATA_AQU_ENABLE
int16_t adc_output_buf[UDMA_PING_PONG_LENGTH * 2] __attribute__((at(ADC_DATA_AQU_BUF_PS2)));
#else
int16_t adc_output_buf[UDMA_PING_PONG_LENGTH * 2] __attribute__((at(ADC_DATA_AQU_BUF_PS4)));
#endif
extern RSI_UDMA_DESC_T UDMA1_Table[32];
#endif // defined (__CC_ARM)

#if defined(__GNUC__)
#ifdef PS2_STATE_ADC_DATA_AQU_ENABLE
int16_t __attribute__((section(".adc_multi_ch_buf_ps2_addr"))) adc_output_buf[UDMA_PING_PONG_LENGTH * 2];
#else
int16_t __attribute__((section(".adc_multi_ch_buf_ps4_addr"))) adc_output_buf[UDMA_PING_PONG_LENGTH * 2];
#endif
extern RSI_UDMA_DESC_T __attribute__((section(".udma_addr1"))) UDMA1_Table[32];
#endif // defined (__GNUC__)
#endif
/** @addtogroup SOC13
* @{
*/
/*==============================================*/
/**
 * @fn         error_t ADC_Init(adc_ch_config_t adcChConfig,adc_config_t adcConfig,adccallbacFunc event)
 * @brief      This API use for initialize ADC.
 * @param[in]  adcChConfig  : ADC channels configuration structure variable.
 * @param[in]  adcConfig    : ADC operation configuration structure variable. 
 * @param[in]  event        : Callback function pointer.
 * @return     error_t      : Execution status,This function return 'RSI_OK' on
 *                            successful execution.  
 */
error_t ADC_Init(adc_ch_config_t adcChConfig, adc_config_t adcConfig, adccallbacFunc event)
{
#ifndef SIMULATION
  uint32_t integer_val = 0;
  float frac           = 0;
#endif
  uint32_t clk_sel = 0;
  uint32_t ch_num  = 0;

  // Register callback event
  adc_commn_config.call_back_event = event;

  // Configure receive buffer
  adc_commn_config.num_of_channel_enable = adcConfig.num_of_channel_enable;

  // Verify the user given sampling rate is proper or not
  if ((adcChConfig.sampling_rate[0] > MAX_SINGCH_SAMPLING_RATE) || (adcChConfig.sampling_rate[0] == 0)) {
    return INVALID_SAMPLING_RATE;
  }

  // Verify the user given sampling rate is proper or not
  for (ch_num = 1; ch_num < adcConfig.num_of_channel_enable; ch_num++) {
    if (adcChConfig.sampling_rate[ch_num] > adcChConfig.sampling_rate[ch_num - 1]
        || (adcChConfig.sampling_rate[ch_num] == 0)) {
      return INVALID_SAMPLING_RATE;
    }
  }

  // Verify the user given sample length is proper or not
  if (adcConfig.num_of_channel_enable == 1) {
    if (adcConfig.operation_mode) {
      if (adcChConfig.num_of_samples[0] > 1) {
        return INVALID_SAMPLE_LENGTH;
      }
    }
    if (adcChConfig.num_of_samples[0] > 1023) {
      return INVALID_SAMPLE_LENGTH;
    }
  }

  if ((adcConfig.num_of_channel_enable < 1) || (adcConfig.num_of_channel_enable > 16)) {
    return INVALID_ADC_CHANNEL_ENABLE;
  }
  // Power up of ADC block
  RSI_ADC_PowerControl(ADC_POWER_ON);

  // Select 32MHZ RC clock for ADC
  RSI_ULPSS_AuxClkConfig(ULPCLK, ENABLE_STATIC_CLK, ULP_AUX_32MHZ_RC_CLK);

  // Clock division factor for calibration,Calibrate ADC on 4MHZ clock
  RSI_ADC_ClkDivfactor(AUX_ADC_DAC_COMP, 0, 4);

  // Set analog reference voltage
  RSI_AUX_RefVoltageConfig(2.8, 3.2);

  // ADC Calibration
  RSI_ADC_Calibration();

  // Configure clock source to ADC with respect to sampling rate
  if (adcConfig.num_of_channel_enable == 1) {
    clk_sel = (adcChConfig.sampling_rate[0] * (adcConfig.num_of_channel_enable * 4));
  } else {
    clk_sel = (adcChConfig.sampling_rate[0] * (adcConfig.num_of_channel_enable * 2));
  }

  // Configure 32Khz RC clock to ADC
  if (clk_sel < 32000) {
    // Select 32KHZ RC clock for ADC
    RSI_ULPSS_AuxClkConfig(ULPCLK, ENABLE_STATIC_CLK, ULP_AUX_32KHZ_RC_CLK);

    adc_commn_config.adc_clk_src = 32000;
  }
  // Configure 64Khz RC clock to ADC
  else if (clk_sel >= 32000 && clk_sel <= 64000) {
    RSI_IPMU_RC64khz_TrimEfuse();
    // Select 32KHZ RC clock for ADC
    RSI_ULPSS_AuxClkConfig(ULPCLK, ENABLE_STATIC_CLK, ULP_AUX_32KHZ_RC_CLK);

    adc_commn_config.adc_clk_src = 64000;
  } else {
    // Configure the 40Mhz XTAL clock to ADC
    if (!(M4_ULP_SLP_STATUS_REG & ULP_MODE_SWITCHED_NPSS)) {
#ifdef SIMULATION
      RSI_ULPSS_AuxClkConfig(ULPCLK, ENABLE_STATIC_CLK, ULP_AUX_32MHZ_RC_CLK);
      adc_commn_config.adc_clk_src = 32000000;
#else
      // Select 40MHZ XTAL ULP reference clock
      RSI_ULPSS_RefClkConfig(ULPSS_RF_REF_CLK);

      // Select 40MHZ XTAL clock for ADC
      RSI_ULPSS_AuxClkConfig(ULPCLK, ENABLE_STATIC_CLK, ULP_AUX_REF_CLK);

      adc_commn_config.adc_clk_src = 40000000;
#endif
    } else {
      adc_commn_config.adc_clk_src = 20000000;
    }
  }

#ifdef SIMULATION
  adc_commn_config.adc_sing_offset = 0x0;
  adc_commn_config.adc_diff_offset = 0x0;
  adc_commn_config.adc_diff_gain   = 0x0;
  adc_commn_config.adc_sing_gain   = 0x0;
#else
  // Trim 32Mhz rc clock to 20Mhz rc clock in PS2 state
  if (M4_ULP_SLP_STATUS_REG & ULP_MODE_SWITCHED_NPSS) {
    // Trim Mhz RC clock to 20Mhz
    RSI_IPMU_M20rcOsc_TrimEfuse();
  }

  // Offset value and gain value read from efuse
  adc_commn_config.adc_sing_offset = RSI_IPMU_Auxadcoff_SeEfuse();
  adc_commn_config.adc_diff_offset = RSI_IPMU_Auxadcoff_DiffEfuse();

  // Single ended gain
  integer_val = RSI_IPMU_Auxadcgain_SeEfuse();
  frac = ((integer_val) & (0x3FFF));
  frac /= 1000;
  adc_commn_config.adc_sing_gain = ((integer_val >> 14) + frac);

  // Differential ended gain
  integer_val = RSI_IPMU_Auxadcgain_DiffEfuse();
  frac = ((integer_val) & (0x3FFF));
  frac /= 1000;
  adc_commn_config.adc_diff_gain = ((integer_val >> 14) + frac);
#endif
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn         error_t ADC_ChannelConfig(adc_ch_config_t adcChConfig,adc_config_t adcConfig)
 * @brief      This API used for configure parameter to ADC channels.
 * @param[in]  adcChConfig  :  ADC channels configuration structure variable.
 * @param[in]  adcConfig    :  ADC operation configuration structure variable. 
 * @return     error_t      :  Execution status,This function return 'RSI_OK' on
 *                             successful execution.  
 */
error_t ADC_ChannelConfig(adc_ch_config_t adcChConfig, adc_config_t adcConfig)
{
  uint8_t ch_num                = 0;
  uint32_t fs_adc               = 0;
  uint32_t f_sample_rate_achive = 0;
  uint16_t total_clk            = 0;
  uint16_t on_clk               = 0;
  uint16_t i                    = 0;
  float inverse_sampl_val       = 0;
  float min_sampl_time          = 0;

  // Get minimum sampling time form given configuration
  min_sampl_time = get_min_sampling_time(adcConfig.num_of_channel_enable, adcChConfig);

  // Find out the maximum sampling rate of ADC can achieved from given configuration
  f_sample_rate_achive = max_sample_rate_achive(min_sampl_time); // Need to implement   Step2

  // Find out total number of ADC cycle
  total_clk = ceil(((1 / (float)f_sample_rate_achive) / (1 / (float)adc_commn_config.adc_clk_src)));

  // Find out number of ON cycles in total ADC cycle
  on_clk = ceil((min_sampl_time / (1 / (float)adc_commn_config.adc_clk_src)));

  if (total_clk == on_clk) {
    total_clk = total_clk + 1;
  }

  // modify the total cycle number for ADC static mode operation
  if (adcConfig.operation_mode) {
    if (total_clk < ceil(adc_commn_config.adc_clk_src / adcChConfig.sampling_rate[0])) {
      total_clk = ceil(adc_commn_config.adc_clk_src / adcChConfig.sampling_rate[0]);
    }
  }

  adc_commn_config.on_clk = on_clk;

  adc_commn_config.total_clk = total_clk;

  if (!(adcConfig.operation_mode)) {
    // find out initial sampling rate
    fs_adc = (adc_commn_config.adc_clk_src / total_clk);

    // Configure channel frequency value for channel1 to number of channel enable
    for (ch_num = 0; ch_num < adcConfig.num_of_channel_enable; ch_num++) {
      inverse_sampl_val = 0;

      if (ch_num == 0) {
        // Configure channel frequency value for channel0
        adcInterConfig.ch_sampling_factor[0] = (ceil(fs_adc / adcChConfig.sampling_rate[0]));

        if (adcInterConfig.ch_sampling_factor[0] < 2) {
          adcInterConfig.ch_sampling_factor[0] = 2;
        }
      } else {
        // Get channel frequency value
        // Configure channel frequency value
        adcInterConfig.ch_sampling_factor[ch_num] =
          (adcInterConfig.ch_sampling_factor[ch_num - 1]
           * (ceil(adcChConfig.sampling_rate[ch_num - 1] / adcChConfig.sampling_rate[ch_num])));
      }

      /* Check configured channel frequency value is power of 2 or not,
			   If configured channel frequency value is not power of 2 then round of value with nearest power of 2 value */
      if (!(adcConfig.num_of_channel_enable == 1)) {
        if (!(check_power_two(adcInterConfig.ch_sampling_factor[ch_num]))) {
          adcInterConfig.ch_sampling_factor[ch_num] = roundupto_pwr2(adcInterConfig.ch_sampling_factor[ch_num]);
        }
      }

      // Sum of enabled channels channel frequency inverse
      for (i = 0; i <= ch_num; i++) {
        inverse_sampl_val += (1 / (float)adcInterConfig.ch_sampling_factor[i]);
      }

      // Check sum of enabled channels channel frequency inverse is grater than 1
      if (inverse_sampl_val > 1) {
        for (i = 0; i <= ch_num; i++) {
          adcInterConfig.ch_sampling_factor[i] *= 2;
        }
      }
    }
    cal_adc_channel_offset();

#ifdef ADC_MULTICHANNEL_WITH_EXT_DMA
    if (adcConfig.num_of_channel_enable > 1) {
      UDMA_ADC_Init();
    }
#endif
  }

  // Configure ADC clock division factor
  RSI_ADC_ClkDivfactor(AUX_ADC_DAC_COMP, adc_commn_config.on_clk, adc_commn_config.total_clk); //Need to review

  // Enable noise average mode
  RSI_ADC_NoiseAvgMode(AUX_ADC_DAC_COMP, ENABLE);

  if (adcConfig.operation_mode) /* If ADC is working on static mode */
  {
#ifdef CHIP_9118
    // Configure the ADC in static mode
    RSI_ADC_Config(AUX_ADC_DAC_COMP, DYNAMIC_MODE_DI, adcConfig.operation_mode, 0, 0);
#endif
#ifdef CHIP_917
    RSI_ADC_Config(AUX_ADC_DAC_COMP, DYNAMIC_MODE_DI, adcConfig.operation_mode, 0, 0, 0);
#endif

    // Configure the input pin selection in static mode
    RSI_ADC_StaticMode(AUX_ADC_DAC_COMP,
                       adcChConfig.pos_inp_sel[0],
                       adcChConfig.neg_inp_sel[0],
                       adcChConfig.input_type[0]);

    // configures user given pins in analog mode
    ADC_PinMux(adcChConfig.pos_inp_sel[0], adcChConfig.neg_inp_sel[0], adcChConfig.input_type[0]);

#ifdef CHIP_917
    RSI_ADC_ChnlIntrUnMask(AUX_ADC_DAC_COMP, 0, adcConfig.operation_mode);
#endif
    adcInterConfig.achived_sampling_rate[0] = ((adc_commn_config.adc_clk_src / adc_commn_config.total_clk));
  } else {
#ifdef CHIP_9118
    // Configure ADC in FIFO mode operation
    if ((adcConfig.num_of_channel_enable) != 1) {
      RSI_ADC_Config(AUX_ADC_DAC_COMP, DYNAMIC_MODE_EN, ADC_FIFOMODE_ENABLE, ADC_FIFO_THR, EXTERNAL_DMA_EN);
    } else {
      RSI_ADC_Config(AUX_ADC_DAC_COMP, DYNAMIC_MODE_EN, ADC_FIFOMODE_ENABLE, ADC_FIFO_THR, INTERNAL_DMA_EN);
    }
#endif
#ifdef CHIP_917
#ifdef ADC_MULTICHANNEL_WITH_EXT_DMA
    RSI_ADC_Config(AUX_ADC_DAC_COMP,
                   DYNAMIC_MODE_EN,
                   ADC_FIFOMODE_ENABLE,
                   AEMPTY_THRSHOLD,
                   AFULL_THRSHOLD,
                   EXTERNAL_DMA_EN);
#else
    RSI_ADC_Config(AUX_ADC_DAC_COMP,
                   DYNAMIC_MODE_EN,
                   ADC_FIFOMODE_ENABLE,
                   AEMPTY_THRSHOLD,
                   AFULL_THRSHOLD,
                   INTERNAL_DMA_EN);
#endif
#endif

    for (ch_num = 0; ch_num < adcConfig.num_of_channel_enable; ch_num++) {
      RSI_ADC_ChannelConfig(AUX_ADC_DAC_COMP,
                            ch_num,
                            adcChConfig.pos_inp_sel[ch_num],
                            adcChConfig.neg_inp_sel[ch_num],
                            adcChConfig.input_type[ch_num]);

      // Configure the sampling rate of ADC
      RSI_ADC_ChannelSamplingRate(AUX_ADC_DAC_COMP,
                                  ch_num,
                                  adcInterConfig.ch_offset_val[ch_num],
                                  adcInterConfig.ch_sampling_factor[ch_num]);

      // ADC channel enable
      RSI_ADC_ChnlEnable(AUX_ADC_DAC_COMP, ch_num);

      ADC_PinMux(adcChConfig.pos_inp_sel[ch_num], adcChConfig.neg_inp_sel[ch_num], adcChConfig.input_type[ch_num]);

#ifndef ADC_MULTICHANNEL_WITH_EXT_DMA
      RSI_ADC_InternalPerChnlDmaEnable(AUX_ADC_DAC_COMP, ch_num);
      RSI_ADC_PingPongMemoryAdrConfig(AUX_ADC_DAC_COMP,
                                      ch_num,
                                      adcChConfig.chnl_ping_address[ch_num],
                                      (adcChConfig.chnl_pong_address[ch_num]),
                                      adcChConfig.num_of_samples[ch_num],
                                      adcChConfig.num_of_samples[ch_num],
                                      ADC_PING_EN,
                                      ADC_PONG_EN);
      RSI_ADC_PingpongEnable(AUX_ADC_DAC_COMP, ch_num);
#ifdef CHIP_9118
      RSI_ADC_ChnlIntrUnMask(AUX_ADC_DAC_COMP, ch_num);
#endif
#ifdef CHIP_917
      RSI_ADC_ChnlIntrUnMask(AUX_ADC_DAC_COMP, ch_num, adcConfig.operation_mode);
#endif
#endif
      adcInterConfig.achived_sampling_rate[ch_num] = ((adc_commn_config.adc_clk_src / adc_commn_config.total_clk)
                                                      / adcInterConfig.ch_sampling_factor[ch_num]); // Need to review

      adcInterConfig.num_of_samples[ch_num] = adcChConfig.num_of_samples[ch_num];

      adcInterConfig.rx_buf[ch_num] = adcChConfig.rx_buf[ch_num];

      adcInterConfig.input_type[ch_num] = adcChConfig.input_type[ch_num];
    }
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn        uint32_t ADC_GetSamplingRate(uint8_t ch_num)
 * @brief     This API get the channel sampling rate value which is configured to ADC.
 * @param[in] ch_num       : ADC Channel number (0 to 15).
 * @return    error_t       :Returns the channel sampling rate
 */
uint32_t ADC_GetSamplingRate(uint8_t ch_num)
{
  return (adcInterConfig.achived_sampling_rate[ch_num] =
            ((adc_commn_config.adc_clk_src / adc_commn_config.total_clk) / adcInterConfig.ch_sampling_factor[ch_num]));
}

/// @cond
#ifdef CHIP_9118

/*==============================================*/
/**
 * @fn        error_t ADC_Start(void)
 * @brief     This API used for start ADC operation.
 * @return    error_t      : Execution status,This function return 'RSI_OK' on
 *                        successful execution.  
 */
error_t ADC_Start(void)
#endif
/// @endcond
#ifdef CHIP_917
  error_t ADC_Start(adc_config_t adcConfig)
#endif
{
  if (adc_commn_config.num_of_channel_enable > 1) {
#ifdef ADC_MULTICHANNEL_WITH_EXT_DMA
    int16_t dummy_read;
#ifdef CHIP_9118
    RSI_ADC_Start(AUX_ADC_DAC_COMP);
#endif
#ifdef CHIP_917
    RSI_ADC_Start(AUX_ADC_DAC_COMP, adcConfig.operation_mode);
#endif
    // Read the dummy ADC output
    while (AUX_ADC_DAC_COMP->FIFO_STATUS_REG_b.ADC_FIFO_EMPTY) {
    };
    dummy_read = RSI_ADC_ReadDataStatic(AUX_ADC_DAC_COMP, 0, 0);
    UDMA_ADC_Start();
#else
    NVIC_EnableIRQ(ADC_IRQn);
#ifdef CHIP_9118
    RSI_ADC_Start(AUX_ADC_DAC_COMP);
#endif
#ifdef CHIP_917
    RSI_ADC_Start(AUX_ADC_DAC_COMP, adcConfig.operation_mode);
#endif
#endif
  } else {
    NVIC_EnableIRQ(ADC_IRQn);
#ifdef CHIP_9118
    RSI_ADC_Start(AUX_ADC_DAC_COMP);
#endif
#ifdef CHIP_917
    RSI_ADC_Start(AUX_ADC_DAC_COMP, adcConfig.operation_mode);
#endif
  }
  return RSI_OK;
}

/// @cond
#ifdef CHIP_9118

/*==============================================*/
/**
 * @fn     error_t ADC_Deinit(void)
 * @brief  This API used for De-initialize the ADC.
 * @return error_t      : Execution status,This function return 'RSI_OK' on
 *                        successful execution.  
 */
error_t ADC_Deinit(void)
#endif
/// @endcond
#ifdef CHIP_917

  /*==============================================*/
  /**
 * @fn         error_t ADC_Deinit(adc_config_t adcConfig)
 * @brief      This API used for De-initialize the ADC.
 * @param[in]  adcConfig    : ADC operation configuration structure variable.
 * @return     error_t      : Execution status,This function return 'RSI_OK' on
 *                            successful execution.  
 */
  error_t ADC_Deinit(adc_config_t adcConfig)
#endif
{
  // Power down the ADC block
  RSI_ADC_PowerControl(ADC_POWER_OFF);

#ifdef CHIP_9118
  // Stop ADC operation
  RSI_ADC_Stop(AUX_ADC_DAC_COMP);
#endif
#ifdef CHIP_917
  RSI_ADC_Stop(AUX_ADC_DAC_COMP, adcConfig.operation_mode);
#endif
  return RSI_OK;
}

/// @cond
#ifdef CHIP_9118

/*==============================================*/
/**
 * @fn           error_t ADC_Stop(void)
 * @brief        This API is used to stop the ADC
 * @return       execution status 
 */
error_t ADC_Stop(void)
{
  RSI_ADC_Stop(AUX_ADC_DAC_COMP);
  return RSI_OK;
}
#endif
/// @endcond
#ifdef CHIP_917

/*==============================================*/
/**
 * @fn           error_t ADC_Stop(adc_config_t adcConfig)
 * @brief        This API is used to stop the ADC
 * @param[in]    adcConfig    : ADC operation configuration structure variable.
 * @return       execution status 
 */
error_t ADC_Stop(adc_config_t adcConfig)
{

  RSI_ADC_Stop(AUX_ADC_DAC_COMP, adcConfig.operation_mode);
  return RSI_OK;
}
#endif

/*==============================================*/
/**
 * @fn     error_t ADC_PingPongReconfig(uint8_t event)
 * @brief  This API used for reconfigure the ADC ping or pong descriptor address.
 *         Use this APIs to reconfigure the ping or pong descriptor in ADC multi-channel case.   
 * @param[in]  event        : EXTERNAL_DMA_RECONFIG or INTERNAL_DMA          
 * @return error_t          : Execution status,This function return 'RSI_OK' on
 *                            successful execution.  
 */
error_t ADC_PingPongReconfig(uint8_t event, uint8_t channel_num)
{
  if (pong_enable_sel) {
    if (event == EXTERNAL_DMA_RECONFIG) {
      pong_enable_sel = 0;

#ifdef ADC_MULTICHANNEL_WITH_EXT_DMA
      /* Configure the UDMA pong descriptor */
      UDMA_ADC_Ping_Write(0);

      ADC_ChannelsDataSort(1);
#endif
    } else {
      pong_enable_sel = 0;

#ifdef CHIP_9118
#ifndef DONOT_READ_DATA_FROM_MEM
#ifdef GAIN_OFFSET_CAL_EN
      RSI_ADC_ReadData(adcInterConfig.rx_buf[0], pong_enable_sel, channel_num, 1, adcInterConfig.input_type[0]);
#else
      RSI_ADC_ReadData(adcInterConfig.rx_buf[channel_num],
                       pong_enable_sel,
                       channel_num,
                       0,
                       adcInterConfig.input_type[0]);
#endif
#endif
#endif
      // Reconfigure the pong address
      RSI_ADC_PingPongReInit(AUX_ADC_DAC_COMP, channel_num, 0, 1);
    }
  } else {
    if (event == EXTERNAL_DMA_RECONFIG) {
      pong_enable_sel = 1;

#ifdef ADC_MULTICHANNEL_WITH_EXT_DMA
      /* Configure the UDMA pong descriptor */
      UDMA_ADC_Pong_Write(0);

      ADC_ChannelsDataSort(0);
#endif
    } else {
      pong_enable_sel = 1;

#ifdef CHIP_9118
#ifndef DONOT_READ_DATA_FROM_MEM
#ifdef GAIN_OFFSET_CAL_EN
      RSI_ADC_ReadData(adcInterConfig.rx_buf[channel_num],
                       pong_enable_sel,
                       channel_num,
                       1,
                       adcInterConfig.input_type[0]);
#else
      RSI_ADC_ReadData(adcInterConfig.rx_buf[channel_num],
                       pong_enable_sel,
                       channel_num,
                       0,
                       adcInterConfig.input_type[0]);
#endif
#endif
#endif
      // Reconfigure the pong address
      RSI_ADC_PingPongReInit(AUX_ADC_DAC_COMP, channel_num, 1, 0);
    }
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn     void ADC_IRQ_Handler(void)
 * @brief  ADC IRQ handler to clear the ADC interrupt re configure the ping and pong address of ADC. 
 * @return None
 */
void ADC_IRQ_Handler(void)
{
#ifdef CHIP_9118
  // Clear channel0 ADC interrupt
  RSI_ADC_ChnlIntrClr(AUX_ADC_DAC_COMP, ADC_CHNL0_INTR);
  // Reconfigure the ADC ping or pong memory address
  ADC_PingPongReconfig(INTERNAL_DMA, 0);
  // Call ADC data acquisition complete event
  adc_commn_config.call_back_event(ADC_CHNL0_INTR, INTERNAL_DMA);
#endif
#ifdef CHIP_917
  RSI_ADC_InterruptHandler(AUX_ADC_DAC_COMP);
#endif
}

/*==============================================*/
/**
 * @fn         error_t ADC_ChannelsDataSort(uint8_t data_select)
 * @brief      This API used for sort channels data.  
 * @param[in]  data_select : This parameter define ADC data is available in ping location or pong location.
 *		       data_select : 0 , Data available in pong location.
 *		       data_select : 1 , Data available in ping location. 
 * @return     error_t     : Execution status,This function return 'RSI_OK' on
 *                         successful execution.  
 */
error_t ADC_ChannelsDataSort(uint8_t data_select)
{
#ifdef ADC_MULTICHANNEL_WITH_EXT_DMA
  uint16_t sample_incr = 0, ch_num = 0;
  int16_t channel_data;
  uint32_t addr_add;
  addr_add = (data_select * UDMA_PING_PONG_LENGTH);
  for (sample_incr = 0; sample_incr < UDMA_PING_PONG_LENGTH; sample_incr++) {
    // Read the Sample by sample from receive buffer
    channel_data = ((adc_output_buf + (addr_add))[sample_incr]);

#ifdef MAX_SAMPLING_RATE

#else
    // Find out channel number
    ch_num                                                                     = (((channel_data)) >> 12);
#endif

#ifdef GAIN_OFFSET_CAL_EN
    channel_data = (channel_data & 0x0fff);
    if (channel_data & BIT(11)) {
      channel_data = channel_data & (ADC_MASK_VALUE);
    } else {
      channel_data = channel_data | BIT(11);
    }

    if (adcInterConfig.input_type[ch_num]) {
      channel_data = ((channel_data - adc_commn_config.adc_diff_offset) * adc_commn_config.adc_diff_gain);
    } else {
      channel_data = ((channel_data - adc_commn_config.adc_sing_offset) * adc_commn_config.adc_sing_gain);
    }
    if (channel_data > 4095) {
      channel_data = 4095;
    }
    if (channel_data <= 0) {
      channel_data = 0;
    } else if (channel_data >= 2048) {
      channel_data = channel_data - 2048;
    } else {
      channel_data = channel_data + 2048;
    }
#ifdef CHIP_9118
    *(volatile int16_t *)(((adcInterConfig.rx_buf[ch_num]) + ch_data[ch_num])) = channel_data;
#endif
#ifdef CHIP_917
    if (ch_num != 0) {
      *(volatile int16_t *)(((adcInterConfig.rx_buf[ch_num - 1]) + ch_data[ch_num - 1])) = (channel_data & 0x0fff);
    }
#endif
#else
#ifdef CHIP_9118
    *(volatile int16_t *)(((adcInterConfig.rx_buf[ch_num]) + ch_data[ch_num])) = (channel_data & 0x0fff);
#endif
#ifdef CHIP_917
    if (ch_num != 0) {
      *(volatile int16_t *)(((adcInterConfig.rx_buf[ch_num - 1]) + ch_data[ch_num - 1])) = (channel_data & 0x0fff);
    }
#endif
#endif
#ifdef CHIP_9118
    ch_data[ch_num]++;

    if (ch_data[ch_num]
        >= adcInterConfig.num_of_samples[ch_num]) // Need to implement, comparision may be in configured value
    {
      adc_commn_config.call_back_event(ch_num, MULTI_CHANNEL_DATA_OCCOURE);
      ch_data[ch_num] = 0;
    }
#endif
#ifdef CHIP_917
    ch_data[ch_num - 1]++;

    if (ch_data[ch_num - 1]
        >= adcInterConfig.num_of_samples[ch_num - 1]) // Need to implement, comparision may be in configured value
    {
      adc_commn_config.call_back_event(ch_num - 1, MULTI_CHANNEL_DATA_OCCOURE);
      ch_data[ch_num - 1] = 0;
    }
#endif
  }
#endif
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn     error_t cal_adc_channel_offset(void)
 * @brief  This API used for calculating adc channels offset value.
 *         this value define on which ADC clock channels will be sampled. 
 * @return error_t     : Execution status,This function return 'RSI_OK' on
 *                        successful execution.  
 */
error_t cal_adc_channel_offset(void)
{
  uint8_t ch_num               = 0;
  uint16_t i                   = 0;
  uint16_t m                   = 0;
  uint16_t n                   = 0;
  uint16_t max_chan_swallo_fac = 0;
  uint16_t cnt                 = 0;

  // Find out max number of channel frequency value
  for (ch_num = 0; ch_num < adc_commn_config.num_of_channel_enable; ch_num++) {
    if (max_chan_swallo_fac < adcInterConfig.ch_sampling_factor[ch_num]) {
      max_chan_swallo_fac = adcInterConfig.ch_sampling_factor[ch_num];
    }
  }
  // Find out channels offset based on channels frequency value
  for (ch_num = 0; ch_num < adc_commn_config.num_of_channel_enable; ch_num++) {
    if (ch_num == 0) {
      adcInterConfig.ch_offset_val[ch_num] = ch_num;
    } else {
      for (i = 0; i < max_chan_swallo_fac; i++) {
        cnt = 0;
        for (n = 0; n < ch_num; n++) {
          for (m = adcInterConfig.ch_offset_val[n]; m < max_chan_swallo_fac;
               m += adcInterConfig.ch_sampling_factor[n]) {
            if (i == m) {
              cnt++;
              break;
            }
          }
        }
        if (!cnt) {
          adcInterConfig.ch_offset_val[ch_num] = i;
          break;
        }
      }
    }
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn     uint8_t check_power_two(uint16_t num)
 * @brief  This API used for find out given number is power of two. 
 * @param[in]  num      : Given number to find out number is power of two or not power of two. 
 * @return Return the status of given number is power of two or not power of two.
 *         Return 1 : Number is power of two.
 *         Return 0 : Number is not power of two. 
 */
uint8_t check_power_two(uint16_t num)
{
  while (num != 1) {
    if (num % 2) {
      return 0;
    }
    num /= 2;
  }
  return 1;
}

/*==============================================*/
/**
 * @fn     uint32_t roundupto_pwr2(uint16_t num)
 * @brief  This API used for round-off the number with nearest power of two number. 
 * @param[in]  num      : Given number to round-off with nearest power of two number. 
 * @return     Return nearest power of two number. 
 */
uint32_t roundupto_pwr2(uint16_t num)
{
  int power = 1, diff = 0;
  while (power < num) {
    power *= 2;
  }
  diff = num - (power / 2);
  if (diff < power / 4) {
    return power / 2;
  } else {
    return power;
  }
}

/*==============================================*/
/**
 * @fn     uint32_t max_sample_rate_achive(float min_sampling_time)
 * @brief  This API find-out maximum ADC sampling rate can achieve from required channels configuration. 
 * @param[in]  number_of_channel : Number of channel enable for ADC operation.
 * @return ADC effective sampling rate. 
 */
uint32_t max_sample_rate_achive(float min_sampling_time)
{
  uint32_t f_max1 = 0, f_max2 = 5000000, f_return;

  float t = 0;

  t = ((min_sampling_time) + (1 / (float)10000000));

  f_max1 = (1 / t);

  f_return = f_max1 < f_max2 ? f_max1 : f_max2;

  return f_return;
}

/*==============================================*/
/**
 * @fn     float get_min_sampling_time(uint8_t number_of_channel)
 * @brief  This API gives required minimum sampling time(ON time of ADC cycles) form given channels configuration.
 * @param[in]  number_of_channel : Number of channel enable for ADC operation.
 * @return Required minimum sampling time(ON time of ADC cycles)  
 */
float get_min_sampling_time(uint8_t number_of_channel, adc_ch_config_t adcChConfig)
{
  uint8_t chan_num_incr, opamp_cnt = 0, dac_cnt = 0;
  float gain_value = 0, delay_return = 0;
  for (chan_num_incr = 0; chan_num_incr < number_of_channel; chan_num_incr++) {
    if ((adcChConfig.pos_inp_sel[chan_num_incr] == POS20) || (adcChConfig.pos_inp_sel[chan_num_incr] == POS21)
        || (adcChConfig.pos_inp_sel[chan_num_incr] == POS22) || (adcChConfig.neg_inp_sel[chan_num_incr] == POS10)
        || (adcChConfig.neg_inp_sel[chan_num_incr] == POS11) || (adcChConfig.neg_inp_sel[chan_num_incr] == POS12)) {
      if (gain_value < adcChConfig.opamp_gain[chan_num_incr]) {
        gain_value = adcChConfig.opamp_gain[chan_num_incr];
        opamp_cnt++;
      }
    }
    if (((adcChConfig.pos_inp_sel[chan_num_incr]) == POS24) || ((adcChConfig.neg_inp_sel[chan_num_incr]) == POS13)) {
      dac_cnt++;
    }
  }
  if (opamp_cnt) {
    if ((gain_value >= (float)2) && (gain_value < (float)2.5)) {
      delay_return = GAIN2_DELAY_HP;
    } else if ((gain_value >= (float)2.5) && (gain_value < (float)3)) {
      delay_return = (float)GAIN2_5_DELAY_HP;
    } else if ((gain_value >= (float)3) && (gain_value < (float)4)) {
      delay_return = (float)GAIN3_DELAY_HP;
    } else if ((gain_value >= (float)4) && (gain_value < (float)5)) {
      delay_return = (float)GAIN4_DELAY_HP;
    } else if ((gain_value >= (float)5) && (gain_value < (float)6)) {
      delay_return = (float)GAIN5_DELAY_HP;
    } else if ((gain_value >= (float)7) && (gain_value < (float)8)) {
      delay_return = (float)GAIN7_DELAY_HP;
    } else if ((gain_value >= (float)8) && (gain_value < (float)9)) {
      delay_return = (float)GAIN8_DELAY_HP;
    } else if ((gain_value >= (float)9) && (gain_value < (float)10)) {
      delay_return = (float)GAIN9_DELAY_HP;
    } else if ((gain_value >= (float)12) && (gain_value < (float)14)) {
      delay_return = (float)GAIN12_5_DELAY_HP;
    } else if ((gain_value >= (float)17) && (gain_value < (float)18)) {
      delay_return = (float)GAIN17_DELAY_HP;
    } else if ((gain_value >= (float)25) && (gain_value <= (float)26)) {
      delay_return = (float)GAIN25_DELAY_HP;
    } else if ((gain_value >= (float)27) && (gain_value <= (float)51)) {
      delay_return = (float)GAIN51_DELAY_HP;
    }
  }
  if (dac_cnt) {
    if ((delay_return < (float)(3.5 / 1000000))) {
      delay_return = (float)(3.5 / 1000000);
    }
  }

  if ((delay_return < ((float)100 / (float)1000000000))) {
    delay_return = ((float)100 / (float)1000000000);
  }

  return delay_return;
}

/*==============================================*/
/**
 * @fn     error_t ADC_PinMux(uint8_t pos_input_pinsel,uint8_t neg_input_pinsel)
 * @brief  This API used for configure GPIO pins in analog mode for ADC operation.
 * @param[in]  pos_inp_sel : Positive input selection for ADC channel.
 * @param[in] neg_inp_sel  : Negative input selection for ADC channel.
 * @param[in]  input_type  : Select single ended or differential ended input to ADC, 
 *                           For Single ended : 0 and Differential ended : 1 
 * @return error_t         : Execution status,This function return 'RSI_OK' on
 *                           successful execution.  
 */
error_t ADC_PinMux(uint8_t pos_input_pinsel, uint8_t neg_input_pinsel, uint8_t input_type)
{
  if (pos_input_pinsel == 0) {
    RSI_EGPIO_UlpPadReceiverDisable(ADCGPIO0);
    RSI_EGPIO_SetPinMux(EGPIO1, 0, ADCGPIO0, ANALOG_MODE);
  }
  if (pos_input_pinsel == POS1) {
    RSI_EGPIO_UlpPadReceiverDisable(ADCGPIO2);
    RSI_EGPIO_SetPinMux(EGPIO1, 0, ADCGPIO2, ANALOG_MODE);
  }
  if (pos_input_pinsel == POS2) {
    RSI_EGPIO_UlpPadReceiverDisable(ADCGPIO4);
    RSI_EGPIO_SetPinMux(EGPIO1, 0, ADCGPIO4, ANALOG_MODE);
  }
  if (pos_input_pinsel == POS3) {
    RSI_EGPIO_UlpPadReceiverDisable(ADCGPIO6);
    RSI_EGPIO_SetPinMux(EGPIO1, 0, ADCGPIO6, ANALOG_MODE);
  }
  if (pos_input_pinsel == POS4) {
    RSI_EGPIO_UlpPadReceiverDisable(ADCGPIO8);
    RSI_EGPIO_SetPinMux(EGPIO1, 0, ADCGPIO8, ANALOG_MODE);
  }
  if (pos_input_pinsel == 5) {
    RSI_EGPIO_UlpPadReceiverDisable(ADCGPIO10);
    RSI_EGPIO_SetPinMux(EGPIO1, 0, ADCGPIO10, ANALOG_MODE);
  }
  if (pos_input_pinsel == POS6) {
    *(volatile uint32_t *)(SDIO_HOST_CONTROL_IN_M4) = BIT(5);
    RSI_EGPIO_PadReceiverDisable(ADC_TOPGPIO25);
    RSI_EGPIO_HostPadsGpioModeDisable(ADC_TOPGPIO25);
    RSI_EGPIO_SetPinMux(EGPIO, 0, ADC_TOPGPIO25, TGPIO_ANALOG_MODE);
  }
  if (pos_input_pinsel == POS7) {
    *(volatile uint32_t *)(SDIO_HOST_CONTROL_IN_M4) = BIT(5);
    RSI_EGPIO_PadReceiverDisable(ADC_TOPGPIO27);
    RSI_EGPIO_HostPadsGpioModeDisable(ADC_TOPGPIO27);
    RSI_EGPIO_SetPinMux(EGPIO, 0, ADC_TOPGPIO27, TGPIO_ANALOG_MODE);
  }
  if (pos_input_pinsel == POS8) {
    *(volatile uint32_t *)(SDIO_HOST_CONTROL_IN_M4) = BIT(5);
    RSI_EGPIO_PadReceiverDisable(ADC_TOPGPIO29);
    RSI_EGPIO_HostPadsGpioModeDisable(ADC_TOPGPIO29);
    RSI_EGPIO_SetPinMux(EGPIO, 0, ADC_TOPGPIO29, TGPIO_ANALOG_MODE);
  }
#ifdef CHIP_9118
  if (pos_input_pinsel == POS9) {
    RSI_EGPIO_PadReceiverDisable(ADC_TOPGPIO23);
    RSI_EGPIO_SetPinMux(EGPIO, 0, ADC_TOPGPIO23, TGPIO_ANALOG_MODE);
  }
#endif
  if ((pos_input_pinsel == POS10) || ((neg_input_pinsel == POS0) && (input_type == POS1))) {
    RSI_EGPIO_UlpPadReceiverDisable(ADCGPIO1);
    RSI_EGPIO_SetPinMux(EGPIO1, 0, ADCGPIO1, ANALOG_MODE);
  }
  if ((pos_input_pinsel == POS11) || ((neg_input_pinsel == POS1) && (input_type == POS1))) {
    RSI_EGPIO_UlpPadReceiverDisable(ADCGPIO3);
    RSI_EGPIO_SetPinMux(EGPIO1, 0, ADCGPIO3, ANALOG_MODE);
  }
  if ((pos_input_pinsel == POS12) || ((neg_input_pinsel == POS2) && (input_type == POS1))) {
    RSI_EGPIO_UlpPadReceiverDisable(ADCGPIO5);
    RSI_EGPIO_SetPinMux(EGPIO1, 0, ADCGPIO5, ANALOG_MODE);
  }
  if ((pos_input_pinsel == POS13) || ((neg_input_pinsel == POS3) && (input_type == POS1))) {
    RSI_EGPIO_UlpPadReceiverDisable(ADCGPIO11);
    RSI_EGPIO_SetPinMux(EGPIO1, 0, ADCGPIO11, ANALOG_MODE);
  }
  if ((pos_input_pinsel == POS14) || ((neg_input_pinsel == POS4) && (input_type == POS1))) {
    RSI_EGPIO_UlpPadReceiverDisable(ADCGPIO9);
    RSI_EGPIO_SetPinMux(EGPIO1, 0, ADCGPIO9, ANALOG_MODE);
  }
  if ((pos_input_pinsel == POS15) || ((neg_input_pinsel == POS5) && (input_type == POS1))) {
    RSI_EGPIO_UlpPadReceiverDisable(ADCGPIO7);
    RSI_EGPIO_SetPinMux(EGPIO1, 0, ADCGPIO7, ANALOG_MODE);
  }
  if ((pos_input_pinsel == POS16) || ((neg_input_pinsel == POS6) && (input_type == POS1))) {
    *(volatile uint32_t *)(SDIO_HOST_CONTROL_IN_M4) = BIT(5);
    RSI_EGPIO_PadReceiverDisable(ADC_TOPGPIO26);
    RSI_EGPIO_HostPadsGpioModeDisable(ADC_TOPGPIO26);
    RSI_EGPIO_SetPinMux(EGPIO, 0, ADC_TOPGPIO26, TGPIO_ANALOG_MODE);
  }
  if ((pos_input_pinsel == POS17) || ((neg_input_pinsel == POS7) && (input_type == POS1))) {
    *(volatile uint32_t *)(SDIO_HOST_CONTROL_IN_M4) = BIT(5);
    RSI_EGPIO_PadReceiverDisable(ADC_TOPGPIO28);
    RSI_EGPIO_HostPadsGpioModeDisable(ADC_TOPGPIO28);
    RSI_EGPIO_SetPinMux(EGPIO, 0, ADC_TOPGPIO28, TGPIO_ANALOG_MODE);
  }
  if ((pos_input_pinsel == POS18) || ((neg_input_pinsel == POS8) && (input_type == POS1))) {
    *(volatile uint32_t *)(SDIO_HOST_CONTROL_IN_M4) = BIT(5);
    RSI_EGPIO_PadReceiverDisable(ADC_TOPGPIO30);
    RSI_EGPIO_HostPadsGpioModeDisable(ADC_TOPGPIO30);
    RSI_EGPIO_SetPinMux(EGPIO, 0, ADC_TOPGPIO30, TGPIO_ANALOG_MODE);
  }
#ifdef CHIP_9118
  if ((pos_input_pinsel == POS19) || ((neg_input_pinsel == POS9) && (input_type == POS1))) {
    RSI_EGPIO_PadReceiverDisable(ADC_TOPGPIO24);
    RSI_EGPIO_SetPinMux(EGPIO, 0, ADC_TOPGPIO24, TGPIO_ANALOG_MODE);
  }
#endif
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_ADC_PingPongMemoryAdrConfig(AUX_ADC_DAC_COMP_Type *pstcADC, 
 *                                                       uint32_t channel ,uint32_t ping_addr,
 *											             uint32_t pong_addr,uint16_t ping_length,
 *														 uint16_t pong_length,uint8_t ping_enable,
 *														 uint8_t pong_enable )
 * @brief        This API is used to configure Ping and pong memory location along with 
 *               length of ping memory and pong memory.
 * @param[in]    pstcADC     : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    channel     : ADC channel to be configured as 0,1,2 ...15
 * @param[in]    ping_addr   : ping address configuration.
 * @param[in]    pong_addr   : pong address configuration.(e.g pong address = ping address +
 *                                                                             ping length ) 
 * @param[in]    ping_length : ping length 
 * @param[in]    pong_length : pong length 
 * @param[in]    ping_enable : ping enable 
 * @param[in]    pong_enable : pong enable  
 * @return       execution status 
 */
error_t RSI_ADC_PingPongMemoryAdrConfig(AUX_ADC_DAC_COMP_Type *pstcADC,
                                        uint32_t channel,
                                        uint32_t ping_addr,
                                        uint32_t pong_addr,
                                        uint16_t ping_length,
                                        uint16_t pong_length,
                                        uint8_t ping_enable,
                                        uint8_t pong_enable)
{
  if ((channel > MAX_CHNL_NO) || (ping_length > PING_LEN_MAX) || (pong_length > PONG_LEN_MAX)
      || (ping_addr > PING_ADDR_MAX) || (pong_addr > PONG_ADDR_MAX)) {
    return INVALID_PARAMETERS;
  }
  adcInterConfig.ping_addr[channel]   = ping_addr;
  adcInterConfig.pong_addr[channel]   = pong_addr;
  adcInterConfig.ping_length[channel] = ping_length;
  adcInterConfig.pong_length[channel] = pong_length;

  if (ping_addr == pong_addr) {
    return ERROR_PING_PONG_ADDR_MATCH;
  } else {
    if (ping_enable) {
      ping_addr                             = ping_addr + (ping_length * 2);
      pstcADC->ADC_INT_MEM_1_b.PROG_WR_DATA = ping_addr;
      adcInterConfig.ping_mem1[channel]     = ping_addr;

      pstcADC->ADC_INT_MEM_2            = (1 << POS15) | ((2 * channel) << POS10) | (ping_length << POS0);
      adcInterConfig.ping_mem2[channel] = pstcADC->ADC_INT_MEM_2;

      pstcADC->ADC_INT_MEM_2            = (0 << POS15) | ((2 * channel) << POS10) | (ping_length << POS0);
      adcInterConfig.ping_mem3[channel] = pstcADC->ADC_INT_MEM_2;
    }
    if (pong_enable) {
      pong_addr                             = pong_addr + ((ping_length + (pong_length * 2)));
      pstcADC->ADC_INT_MEM_1_b.PROG_WR_DATA = pong_addr;
      adcInterConfig.pong_mem1[channel]     = pong_addr;

      pstcADC->ADC_INT_MEM_2            = (1 << POS15) | ((2 * channel + 1) << POS10) | (pong_length << POS0);
      adcInterConfig.pong_mem2[channel] = pstcADC->ADC_INT_MEM_2;

      pstcADC->ADC_INT_MEM_2            = (0 << POS15) | ((2 * channel + 1) << POS10) | (pong_length << POS0);
      adcInterConfig.pong_mem3[channel] = pstcADC->ADC_INT_MEM_2;
    }
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           uint32_t RSI_ADC_PingPongReInit(AUX_ADC_DAC_COMP_Type *pstcADC,
 *                                                uint8_t ping_enable,uint8_t pong_enable )
 * @brief        This API is used to reconfigure Ping-pong memory location along with the length of the samples
 * @param[in]    pstcADC     : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    channel     : ADC channel to be configured as 0,1,2 ...15
 * @return       execution status 
 *
 */
error_t RSI_ADC_PingPongReInit(AUX_ADC_DAC_COMP_Type *pstcADC,
                               uint8_t channel,
                               uint8_t ping_enable,
                               uint8_t pong_enable)
{
  if (channel > MAX_CHNL_NO) {
    return INVALID_PARAMETERS;
  }
  if (ping_enable) {
    pstcADC->ADC_INT_MEM_1_b.PROG_WR_DATA = adcInterConfig.ping_mem1[channel];
    pstcADC->ADC_INT_MEM_2                = adcInterConfig.ping_mem2[channel];
    pstcADC->ADC_INT_MEM_2                = adcInterConfig.ping_mem3[channel];
  }
  if (pong_enable) {
    pstcADC->ADC_INT_MEM_1_b.PROG_WR_DATA = adcInterConfig.pong_mem1[channel];
    pstcADC->ADC_INT_MEM_2                = adcInterConfig.pong_mem2[channel];
    pstcADC->ADC_INT_MEM_2                = adcInterConfig.pong_mem3[channel];
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           void RSI_ADC_Calibration()
 * @brief        This API used to do ADC calibration.
 * @param[in]    None
 * @return       None 
 */
void RSI_ADC_Calibration(void)
{
  uint32_t impuDummyRead = 0;

  AUX_ADC_DAC_COMP->AUXADC_CTRL_1_b.ADC_ENABLE |= 1U;
  AUX_ADC_DAC_COMP->AUXADC_CONFIG_2 |= BIT(10);

  impuDummyRead = ULP_SPI_MEM_MAP(0);
  if (calib_done == 0) {
    ULP_SPI_MEM_MAP(AUXADCREG0) |= BIT(13) | BIT(12) | BIT(11) | BIT(8) | BIT(7) | BIT(6);
    while (GSPI_CTRL_REG1 & SPI_ACTIVE)
      ;
    /*Wait for 1*/
    while (!(ULP_SPI_MEM_MAP(SPAREREG2) & BIT(0)))
      ;
    /*wait for 0*/
    while ((ULP_SPI_MEM_MAP(SPAREREG2) & BIT(0)))
      ;
    /*150 clocks of 1 Mhz wait*/
    auxadcCalibValue = ULP_SPI_MEM_MAP(AUXADCREG2);
    auxadcCalibValueLoad |= BIT(0) | BIT(7);
    auxadcCalibValueLoad |= (auxadcCalibValue & 0x1F) << 2;
    auxadcCalibValueLoad |= (((auxadcCalibValue >> POS5) & 0x1F) << POS13);
    auxadcCalibValueLoad |= (((auxadcCalibValue >> POS10) & 0x1F) << POS8);
    calib_done = 1;
  } else {
  }

  ULP_SPI_MEM_MAP(AUXADCREG1) = (auxadcCalibValueLoad);
  while (GSPI_CTRL_REG1 & SPI_ACTIVE)
    ;
  AUX_ADC_DAC_COMP->AUXADC_CTRL_1_b.ADC_ENABLE = 0;
}

/*==============================================*/
/**
 * @fn           error_t RSI_ADC_PingpongEnable(AUX_ADC_DAC_COMP_Type *pstcADC, uint32_t channel)
 * @brief        This API is used to enable ping pong for corresponding ADC channels 
 * @param[in]    pstcADC     : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    channel  : ADC channel to be configured as 0,1,2 ...15
 * @return       execution status 
 *
 */
error_t RSI_ADC_PingpongEnable(AUX_ADC_DAC_COMP_Type *pstcADC, uint32_t channel)
{
  if (channel > MAX_CHNL_NO) {
    return INVALID_PARAMETERS;
  }
  pstcADC->ADC_SEQ_CTRL_b.ADC_SEQ_CTRL_PING_PONG |= 1 << channel;
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_ADC_PingpongDisable(AUX_ADC_DAC_COMP_Type *pstcADC, uint32_t channel)
 * @brief        This API is used to disable the ping pong for corresponding ADC channels 
 * @param[in]    pstcADC     : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    channel  :ADC channel to be configured as 0,1,2 ...15
 * @return       execution status 
 *
 */
error_t RSI_ADC_PingpongDisable(AUX_ADC_DAC_COMP_Type *pstcADC, uint32_t channel)
{
  if (channel > MAX_CHNL_NO) {
    return INVALID_PARAMETERS;
  }
  pstcADC->ADC_SEQ_CTRL_b.ADC_SEQ_CTRL_PING_PONG &= ~(1 << channel);
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_ADC_InternalPerChnlDmaEnable(AUX_ADC_DAC_COMP_Type *pstcADC, * uint32_t channel)
 * @brief        This API is used to enable internal DMA for corresponding ADC channels 
 * @param[in]    pstcADC     : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    channel  :ADC channel to be configured as 0,1,2 ...15
 * @return       execution status 
 */
error_t RSI_ADC_InternalPerChnlDmaEnable(AUX_ADC_DAC_COMP_Type *pstcADC, uint32_t channel)
{
  if (channel > MAX_CHNL_NO) {
    return INVALID_PARAMETERS;
  }
  pstcADC->ADC_SEQ_CTRL_b.ADC_SEQ_CTRL_DMA_MODE |= 1 << channel;
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_ADC_InternalPerChnlDmaDisable(AUX_ADC_DAC_COMP_Type *pstcADC,
 *                                                         uint32_t channel)
 * @brief        This API is used to disable the internal dma channel for corresponding ADC channels 
 * @param[in]    pstcADC     : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    channel  :ADC channel to be configured as 0,1,2 ...15
 * @return       execution status 
 */
error_t RSI_ADC_InternalPerChnlDmaDisable(AUX_ADC_DAC_COMP_Type *pstcADC, uint32_t channel)
{
  if (channel > MAX_CHNL_NO) {
    return INVALID_PARAMETERS;
  }
  pstcADC->ADC_SEQ_CTRL_b.ADC_SEQ_CTRL_DMA_MODE &= ~(1 << channel);
  return RSI_OK;
}

/// @cond
#ifdef CHIP_9118

/*==============================================*/
/**
 * @fn           void RSI_ADC_Config(AUX_ADC_DAC_COMP_Type *pstcADC,uint8_t multi_channel_en,
 *                                   uint8_t static_fifo_mode, uint8_t fifo_threshold )
 * @brief        This API is used to configure the ADC channels 
 * @param[in]    pstcADC     : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    multi_channel_en : Multi channel enable parameter.
 * @param[in]    static_fifo_mode : Static mode  enable parameter,
 *                                  static_fifo_mode =1 static mode
 *                                  static_fifo_mode =0 fifo mode
 * @param[in]    fifo_threshold   : fifo threshold value for ADC operation.
 * @param[in]    internal_dma_en  : Internal DMA enable parameter.
 * @return       execution status.
 */
error_t RSI_ADC_Config(AUX_ADC_DAC_COMP_Type *pstcADC,
                       uint8_t multi_channel_en,
                       uint8_t static_fifo_mode,
                       uint8_t fifo_threshold,
                       uint8_t internal_dma_en)
#endif
/// @endcond
#ifdef CHIP_917
  /*==============================================*/
  /**
 * @fn           void RSI_ADC_Config(AUX_ADC_DAC_COMP_Type *pstcADC,uint8_t multi_channel_en,
 *                                   uint8_t static_fifo_mode, uint8_t fifo_threshold )
 * @brief        This API is used to configure the ADC channels 
 * @param[in]    pstcADC     : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    multi_channel_en : Multi channel enable parameter.
 * @param[in]    static_fifo_mode : Static mode  enable parameter,
 *                                  static_fifo_mode =1 static mode
 *                                  static_fifo_mode =0 fifo mode
 * @param[in]    a_empty_threshold : aempty fifo threshold value.
 * @param[in]    a_full_threshold  : afull fifo threshold value. 
 * @param[in]    internal_dma_en   : Internal DMA enable parameter.
 * @return       execution status.
 */
  error_t RSI_ADC_Config(AUX_ADC_DAC_COMP_Type *pstcADC,
                         uint8_t multi_channel_en,
                         uint8_t static_fifo_mode,
                         uint8_t a_empty_threshold,
                         uint8_t a_full_threshold,
                         uint8_t internal_dma_en)
#endif
{
#ifdef CHIP_9118
  if (fifo_threshold > MAX_THRS_VAL) {
    return INVALID_PARAMETERS;
  }
#endif
  pstcADC->AUXADC_CTRL_1_b.ADC_NUM_PHASE            = 1;
  pstcADC->AUXADC_CTRL_1_b.ADC_MULTIPLE_CHAN_ACTIVE = multi_channel_en;
#ifdef CHIP_9118
  pstcADC->AUXADC_CTRL_1_b.ADC_STATIC_MODE = static_fifo_mode;
#endif
#ifdef CHIP_917
  if ((static_fifo_mode == ADC_STATICMODE_ENABLE) || (static_fifo_mode == ADC_STATICMODE_THRESHOLD_EN)) {
    pstcADC->AUXADC_CTRL_1_b.ADC_STATIC_MODE = 1;
  } else {
    pstcADC->AUXADC_CTRL_1_b.ADC_STATIC_MODE = static_fifo_mode;
  }
#endif

#ifdef CHIP_9118
  pstcADC->AUXADC_CTRL_1_b.ADC_FIFO_THRESHOLD = fifo_threshold;
#endif
#ifdef CHIP_917
  pstcADC->ADC_FIFO_THRESHOLD = (a_empty_threshold | a_full_threshold << 4);
#endif
  pstcADC->AUXADC_CTRL_1_b.ADC_FIFO_FLUSH = 1;
  if (internal_dma_en == INTERNAL_DMA_EN) {
    pstcADC->INTERNAL_DMA_CH_ENABLE_b.INTERNAL_DMA_ENABLE = internal_dma_en;
  } else {
    pstcADC->INTERNAL_DMA_CH_ENABLE_b.INTERNAL_DMA_ENABLE = 0;
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_ADC_ChannelConfig(AUX_ADC_DAC_COMP_Type *pstcADC, uint32_t channel , uint8_t an_perif_adc_ip_sel,uint8_t an_perif_adc_in_sel, uint8_t an_perif_adc_diffmode )
 * @brief        This API is used to configure the ADC channels 
 * @param[in]    pstcADC     : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    channel  :ADC channel to be configured as 0,1,2 ...15
 * @param[in]    an_perif_adc_ip_sel     : ADC positive input select in static mode.
 * @param[in]    an_perif_adc_in_sel     : ADC negative input select in static mode.
 * @param[in]    an_perif_adc_diffmode   : ADC differential mode selection in static mode.
 * @return       execution status 
 */
error_t RSI_ADC_ChannelConfig(AUX_ADC_DAC_COMP_Type *pstcADC,
                              uint32_t channel,
                              uint8_t an_perif_adc_ip_sel,
                              uint8_t an_perif_adc_in_sel,
                              uint8_t an_perif_adc_diffmode)
{
  if ((channel > MAX_CHNL_NO) || (an_perif_adc_ip_sel > MAX_POS_IN_SEL) || (an_perif_adc_in_sel > MAX_NEG_IN_SEL)
      || (an_perif_adc_diffmode > MAX_DIFF_MODE)) {
    return INVALID_PARAMETERS;
  }
  if (pstcADC->AUXADC_CTRL_1_b.ADC_MULTIPLE_CHAN_ACTIVE) {
    if (an_perif_adc_diffmode) {
      pstcADC->ADC_CH_BIT_MAP_CONFIG[channel].ADC_CH_BIT_MAP_CONFIG_0_b.CHANNEL_BITMAP |=
        ((an_perif_adc_diffmode << POS26) | (an_perif_adc_in_sel << POS22));
    }
    pstcADC->ADC_CH_BIT_MAP_CONFIG[channel].ADC_CH_BIT_MAP_CONFIG_0_b.CHANNEL_BITMAP |= an_perif_adc_ip_sel << POS17;
  } else {
    if (an_perif_adc_diffmode) {
#ifdef CHIP_9118
      pstcADC->AUXADC_CONFIG_1_b.AUXADC_DIFF_MODE = an_perif_adc_diffmode;
      pstcADC->AUXADC_CONFIG_1_b.AUXADC_INN_SEL   = an_perif_adc_in_sel;
#endif
#ifdef CHIP_917
      pstcADC->AUXADC_CONFIG_2_b.AUXADC_DIFF_MODE = an_perif_adc_diffmode;
      pstcADC->AUXADC_CONFIG_2_b.AUXADC_INN_SEL   = an_perif_adc_in_sel;
#endif
    }
#ifdef CHIP_9118
    pstcADC->AUXADC_CONFIG_1_b.AUXADC_INP_SEL = an_perif_adc_ip_sel;
#endif
#ifdef CHIP_917
    pstcADC->AUXADC_CONFIG_2_b.AUXADC_INP_SEL = an_perif_adc_ip_sel;
#endif
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_ADC_ChannelSamplingRate(AUX_ADC_DAC_COMP_Type *pstcADC, uint32_t channel )
 * @brief        This API is used to configure the ADC sampling rate for ADC channels. 
 * @param[in]    pstcADC     : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    channel  :ADC channel to be configured as 0,1,2 ...15
 * @param[in]    adc_ch_offset     : channel offset for each channel 
 * @param[in]    adc_ch_freq_val   : channel frequency for each channel to set sampling rate
 * @return       execution status 
 */
error_t RSI_ADC_ChannelSamplingRate(AUX_ADC_DAC_COMP_Type *pstcADC,
                                    uint32_t channel,
                                    uint16_t adc_ch_offset,
                                    uint16_t adc_ch_freq_val)
{
  if (adc_ch_freq_val == 1) {
    return ERROR_FREQ_VAL;
  } else {
    pstcADC->ADC_CH_OFFSET_b[channel].CH_OFFSET = adc_ch_offset;
    pstcADC->ADC_CH_FREQ[channel]               = adc_ch_freq_val;
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_ADC_StaticMode(AUX_ADC_DAC_COMP_Type *pstcADC, adcConfig *config)
 * @brief        This API is used to configure the ADC in Static Mode 
 * @param[in]    pstcADC     : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    an_perif_adc_ip_sel   : ADC positive input select in static mode.
 * @param[in]    an_perif_adc_in_sel   : ADC negative input select in static mode.
 * @param[in]    an_perif_adc_diffmode : ADC differential mode selection in static mode.
 * @return       execution status 
 */
error_t RSI_ADC_StaticMode(AUX_ADC_DAC_COMP_Type *pstcADC,
                           uint16_t an_perif_adc_ip_sel,
                           uint16_t an_perif_adc_in_sel,
                           uint8_t an_perif_adc_diffmode)
{

  if (an_perif_adc_diffmode) {
#ifdef CHIP_9118
    pstcADC->AUXADC_CONFIG_1_b.AUXADC_DIFF_MODE = an_perif_adc_diffmode;
    pstcADC->AUXADC_CONFIG_1_b.AUXADC_INN_SEL   = an_perif_adc_in_sel;
#endif
#ifdef CHIP_917
    pstcADC->AUXADC_CONFIG_2_b.AUXADC_DIFF_MODE = an_perif_adc_diffmode;
    pstcADC->AUXADC_CONFIG_2_b.AUXADC_INN_SEL   = an_perif_adc_in_sel;
#endif
  }
#ifdef CHIP_9118
  pstcADC->AUXADC_CONFIG_1_b.AUXADC_INP_SEL = an_perif_adc_ip_sel;
#endif
#ifdef CHIP_917
  pstcADC->AUXADC_CONFIG_2_b.AUXADC_INP_SEL = an_perif_adc_ip_sel;
#endif
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_ADC_FifoMode(AUX_ADC_DAC_COMP_Type *pstcADC,uint16_t an_perif_adc_ip_sel, uint16_t an_perif_adc_in_sel, uint8_t an_perif_adc_diffmode)
 * @brief        This API is used to configure the ADC in FIFO Mode 
 * @param[in]    pstcADC     : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    an_perif_adc_ip_sel   : ADC positive input select in Fifo mode.
 * @param[in]    an_perif_adc_in_sel   : ADC negative input select in Fifo mode.
 * @param[in]    an_perif_adc_diffmode : ADC differential mode selection in Fifo mode.
 * @return       execution status 
 */
error_t RSI_ADC_FifoMode(AUX_ADC_DAC_COMP_Type *pstcADC,
                         uint16_t channel_no,
                         uint16_t an_perif_adc_ip_sel,
                         uint16_t an_perif_adc_in_sel,
                         uint8_t an_perif_adc_diffmode)
{
  pstcADC->AUXADC_CTRL_1_b.ADC_NUM_PHASE = 1;

  pstcADC->INTERNAL_DMA_CH_ENABLE_b.PER_CHANNEL_ENABLE |= 1 << channel_no;

  if (an_perif_adc_diffmode) {
#ifdef CHIP_9118
    pstcADC->AUXADC_CONFIG_1_b.AUXADC_DIFF_MODE = an_perif_adc_diffmode;
    pstcADC->AUXADC_CONFIG_1_b.AUXADC_INN_SEL   = an_perif_adc_in_sel;
#endif
#ifdef CHIP_917
    pstcADC->AUXADC_CONFIG_2_b.AUXADC_DIFF_MODE = an_perif_adc_diffmode;
    pstcADC->AUXADC_CONFIG_2_b.AUXADC_INN_SEL   = an_perif_adc_in_sel;
#endif
  }
#ifdef CHIP_9118
  pstcADC->AUXADC_CONFIG_1_b.AUXADC_INP_SEL = an_perif_adc_ip_sel;
#endif
#ifdef CHIP_917
  pstcADC->AUXADC_CONFIG_2_b.AUXADC_INP_SEL = an_perif_adc_ip_sel;
#endif
  return RSI_OK;
}

/// @cond
#ifdef CHIP_9118
/*==============================================*/
/**
 * @fn           error_t RSI_ADC_Start( AUX_ADC_DAC_COMP_Type *pstcADC )
 * @brief        This API is used to start the ADC
 * @param[in]    pstcADC     : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    channel  : ADC channel to be configured as 0,1,2 ...15
 * @return       execution status 
 */
error_t RSI_ADC_Start(AUX_ADC_DAC_COMP_Type *pstcADC)
#endif
/// @endcond
#ifdef CHIP_917
  /*==============================================*/
  /**
 * @fn           error_t RSI_ADC_Start(AUX_ADC_DAC_COMP_Type *pstcADC,uint8_t oper_mode)
 * @brief        This API is used to start the ADC
 * @param[in]    pstcADC     : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    oper_mode   : oper_mode; Static or dynamic mode.
 * @return       execution status 
 */
  error_t RSI_ADC_Start(AUX_ADC_DAC_COMP_Type *pstcADC, uint8_t oper_mode)
#endif
{
#ifdef CHIP_9118
  pstcADC->AUXADC_CONFIG_2_b.AUXADC_CONFIG_ENABLE = 1U;
#endif
#ifdef CHIP_917
  if (oper_mode == ADC_FIFOMODE_ENABLE) {
    pstcADC->AUXADC_CONFIG_2_b.AUXADC_DYN_ENABLE = 1U;
  }
#endif
  pstcADC->AUXADC_CTRL_1_b.ADC_ENABLE = 1U;
  return RSI_OK;
}

/// @cond
#ifdef CHIP_9118
/*==============================================*/
/**
 * @fn           error_t RSI_ADC_Stop( AUX_ADC_DAC_COMP_Type *pstcADC )
 * @brief        This API is used to stop the ADC
 * @param[in]    pstcADC     : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    channel  :ADC channel to be configured as 0,1,2 ...15
 * @return       execution status 
 */
error_t RSI_ADC_Stop(AUX_ADC_DAC_COMP_Type *pstcADC)
#endif
/// @endcond
#ifdef CHIP_917
  error_t RSI_ADC_Stop(AUX_ADC_DAC_COMP_Type *pstcADC, uint8_t oper_mode)
#endif
{
#ifdef CHIP_9118
  pstcADC->AUXADC_CONFIG_2_b.AUXADC_CONFIG_ENABLE = 0;
#endif
#ifdef CHIP_917
  if (oper_mode == ADC_FIFOMODE_ENABLE) {
    pstcADC->AUXADC_CONFIG_2_b.AUXADC_DYN_ENABLE = 0;
  }
#endif
  pstcADC->AUXADC_CTRL_1_b.ADC_ENABLE = 0;
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_ADC_ChnlEnable(AUX_ADC_DAC_COMP_Type *pstcADC, uint32_t channel)
 * @brief        This API is used to Enable the ADC channel
 * @param[in]    pstcADC     : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    channel  :ADC channel to be configured as 0,1,2 ...15
 * @return       execution status 
 */
error_t RSI_ADC_ChnlEnable(AUX_ADC_DAC_COMP_Type *pstcADC, uint32_t channel)
{
  if (channel > MAX_CHNL_NO) {
    return INVALID_PARAMETERS;
  }
  if ((AUX_ADC_DAC_COMP->AUXADC_CTRL_1_b.ADC_MULTIPLE_CHAN_ACTIVE)
      || (AUX_ADC_DAC_COMP->INTERNAL_DMA_CH_ENABLE_b.INTERNAL_DMA_ENABLE)) {
    pstcADC->INTERNAL_DMA_CH_ENABLE_b.PER_CHANNEL_ENABLE |= 1 << channel;
  } else {
    return ERROR_NO_MULTI_CHNL_ENABLE;
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_ADC_ChnlDisable(AUX_ADC_DAC_COMP_Type *pstcADC, uint32_t channel)
 * @brief        This API is used to Disable the ADC
 * @param[in]    pstcADC     : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    channel  :ADC channel to be configured as 0,1,2 ...15
 * @return       execution status 
 */
error_t RSI_ADC_ChnlDisable(AUX_ADC_DAC_COMP_Type *pstcADC, uint32_t channel)
{
  if (channel > MAX_CHNL_NO) {
    return INVALID_PARAMETERS;
  }
  pstcADC->INTERNAL_DMA_CH_ENABLE_b.PER_CHANNEL_ENABLE &= ~(1 << channel);
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           int16_t RSI_ADC_ReadDataStatic(AUX_ADC_DAC_COMP_Type *pstcADC, uint8_t data_process_en, uint8_t diff_en)
 * @brief        This API is used to Read the ADC samples when static mode is enabled.
 * @param[in]    pstcADC             : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[out]   data_process_en     : This parameter define for the output ADC samples 
 *                                     gain and calculation enable or disable.
 *                                     data_process_en -  1 enable the gain 
 *                                                        and calculation on output samples
 *                                     data_process_en -  0 disable the gain 
 *                                                        and calculation on output samples
 * @param[in]    diff_en             : ADC mode of operation single ended or differential ended
 *                                     diff_en   - 1   differential ended mode enable.
 *                                     diff_en   - 0   single ended mode enable. 
 * @return       execution status 
 */
int16_t RSI_ADC_ReadDataStatic(AUX_ADC_DAC_COMP_Type *pstcADC, uint8_t data_process_en, uint8_t diff_en)
{
  int16_t adc_temp = 0;
  if (data_process_en) {
    adc_temp = pstcADC->AUXADC_DATA_b.AUXADC_DATA;
    adc_temp = RSI_ADC_GainOffsetCal(adc_temp, diff_en);
    return adc_temp;
  } else {
    return pstcADC->AUXADC_DATA_b.AUXADC_DATA;
  }
}
/*==============================================*/
/**
 * @fn           error_t RSI_ADC_ReadData(int16_t *data,uint8_t ping_pong,uint16_t channel,
 *                                        uint8_t data_process_en,uint8_t diff_en)
 * @brief        This API is used to read the ADC samples when ulp memories are used.
 * @param[out]   data            :Pointer to read buffer 
 * @param[in]    Channel         :ADC channel number 
 * @param[in]    ping_pong       : 1 - ping memory read
 *                                 0 - pong memory read
 * @param[in]    data_process_en :This parameter define gain and offset calculation 
 *                                enabled on ADC output samples. 
 *                                data_process_en :  1 , Enable the gain and offset calculation on ADC data.
 *                                data_process_en :  0 , Disable the gain and offset calculation on ADC data. 
 * @param[in]    diff_en         : ADC mode of operation single ended or differential ended
 *                                 diff_en   - 1   differential ended mode
 *                                 diff_en   - 0   single ended mode
 * @return       execution status 
 */
error_t RSI_ADC_ReadData(int16_t *data, uint8_t ping_pong, uint16_t channel, uint8_t data_process_en, uint8_t diff_en)
{
  uint32_t i, addr_read = 0, sample_len = 0;
  if (ping_pong) {
    addr_read = adcInterConfig.ping_addr[channel];

    sample_len = adcInterConfig.ping_length[channel];
  } else {
    addr_read = adcInterConfig.pong_addr[channel] + adcInterConfig.ping_length[channel];

    sample_len = adcInterConfig.pong_length[channel];
  }

  for (i = 0; i < sample_len; i++) {
    data[i] = *(volatile uint16_t *)(addr_read + i * 2);
    if (data_process_en) {
      if (data[i] & BIT(11)) {
        data[i] = (data[i] & (ADC_MASK_VALUE));
      } else {
        data[i] = data[i] | BIT(11);
      }
      if (diff_en) {
        data[i] = ((data[i] - adc_commn_config.adc_diff_offset) * adc_commn_config.adc_diff_gain);
      } else {
        data[i] = ((data[i] - adc_commn_config.adc_sing_offset) * adc_commn_config.adc_sing_gain);
      }
      if (data[i] > 4095) {
        data[i] = 4095;
      }
      if (data[i] <= 0) {
        data[i] = 0;
      }
      if (data[i] >= 2048) {
        data[i] = data[i] - 2048;
      } else {
        data[i] = data[i] + 2048;
      }
    }
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_ADC_ClkDivfactor(AUX_ADC_DAC_COMP_Type *pstcADC,
 *                                            uint16_t adc_on_time , uint16_t adc_total_duration)
 * @brief        This API is used to set clock with configurable on time 
 * @param[in]    pstcADC          : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    adcOnTime        : ON duration of the clock 
 * @param[in]    adcTotalDuration : Total ON and OFF duration of the clock
 * @return       execution status 
 */
error_t RSI_ADC_ClkDivfactor(AUX_ADC_DAC_COMP_Type *pstcADC, uint16_t adc_on_time, uint16_t adc_total_duration)
{
  if ((adc_on_time > MAX_ON_DUR_TIME) || (adc_total_duration > MAX_TOL_DUR_TIME)) {
    return INVALID_PARAMETERS;
  }
  pstcADC->AUXADC_CTRL_1_b.EN_ADC_CLK           = 0;
  pstcADC->AUXADC_CLK_DIV_FAC_b.ADC_CLK_ON_DUR  = adc_on_time;
  pstcADC->AUXADC_CLK_DIV_FAC_b.ADC_CLK_DIV_FAC = adc_total_duration;
  pstcADC->AUXADC_CTRL_1_b.EN_ADC_CLK           = 1;

  return RSI_OK;
}

/// @cond
#ifdef CHIP_9118

/*==============================================*/
/**
 * @fn           error_t RSI_ADC_ChnlIntrMask(AUX_ADC_DAC_COMP_Type *pstcADC, uint32_t channel)
 * @brief        This API is used to Unmask the ADC channel
 * @param[in]    pstcADC  : Pointer to the AUX_ADC_DAC_COMP_Type structure. 
 * @param[in]    channel  : ADC channel to be configured as 0,1,2 ...15
 * @return       execution status 
 */
error_t RSI_ADC_ChnlIntrUnMask(AUX_ADC_DAC_COMP_Type *pstcADC, uint32_t channel)
#endif
/// @endcond
#ifdef CHIP_917

  /*==============================================*/
  /**
 * @fn           error_t RSI_ADC_ChnlIntrUnMask(AUX_ADC_DAC_COMP_Type *pstcADC, uint32_t channel, uint8_t oper_mode)
 * @brief        This API is used to Unmask the ADC channel
 * @param[in]    pstcADC  : Pointer to the AUX_ADC_DAC_COMP_Type structure. 
 * @param[in]    channel  : ADC channel to be configured as 0,1,2 ...15
 * @param[in]    oper_mode: ADC operation mode select,
 *                          oper_mode : 1 Static Mode
 *                          oper_mode : 0 Dynamic Mode
 * @return       execution status 
 */
  error_t RSI_ADC_ChnlIntrUnMask(AUX_ADC_DAC_COMP_Type *pstcADC, uint32_t channel, uint8_t oper_mode)
#endif
{
  if (channel > MAX_CHNL_NO) {
    return INVALID_PARAMETERS;
  }
#ifdef CHIP_9118
  if ((AUX_ADC_DAC_COMP->INTERNAL_DMA_CH_ENABLE_b.INTERNAL_DMA_ENABLE)) {
    pstcADC->INTR_MASK_REG_b.FIRST_MEM_SWITCH_INTR_MASK &= ~(1 << channel);
  } else {
    pstcADC->INTR_MASK_REG_b.THRESHOLD_DETECTION_INTR_EN = 0;
    pstcADC->INTR_MASK_REG_b.ADC_FIFO_AFULL_INTR_MASK    = 0;
    pstcADC->INTR_MASK_REG_b.ADC_FIFO_FULL_INTR_MASK     = 0;
  }
#endif
#ifdef CHIP_917
  if (oper_mode == ADC_STATICMODE_ENABLE) {
    pstcADC->INTR_MASK_REG_b.ADC_STATIC_MODE_DATA_INTR_MASK = 0;
  } else if (oper_mode == ADC_STATICMODE_THRESHOLD_EN) {
    pstcADC->INTR_MASK_REG_b.THRESHOLD_DETECTION_INTR_EN = 0;
    pstcADC->INTR_MASK_REG_b.ADC_FIFO_AFULL_INTR_MASK    = 0;
    pstcADC->INTR_MASK_REG_b.ADC_FIFO_FULL_INTR_MASK     = 0;
  } else {
    if ((AUX_ADC_DAC_COMP->INTERNAL_DMA_CH_ENABLE_b.INTERNAL_DMA_ENABLE)) {
      pstcADC->INTR_MASK_REG_b.FIRST_MEM_SWITCH_INTR_MASK &= ~(1 << channel);
    }
  }
#endif
  return RSI_OK;
}

/// @cond
#ifdef CHIP_9118

/*==============================================*/
/**
 * @fn           error_t RSI_ADC_ChnlIntrMask(AUX_ADC_DAC_COMP_Type *pstcADC, uint32_t channel)
 * @brief        This API is used to Mask the ADC channel
 * @param[in]    pstcADC  : Pointer to the AUX_ADC_DAC_COMP_Type structure. 
 * @param[in]    channel  : ADC channel to be configured as 0,1,2 ...15
 * @return       execution status 
 */
error_t RSI_ADC_ChnlIntrMask(AUX_ADC_DAC_COMP_Type *pstcADC, uint32_t channel)
#endif
/// @endcond
#ifdef CHIP_917

  /*==============================================*/
  /**
 * @fn           error_t RSI_ADC_ChnlIntrMask(AUX_ADC_DAC_COMP_Type *pstcADC, uint32_t channel, uint8_t oper_mode)
 * @brief        This API is used to Mask the ADC channel
 * @param[in]    pstcADC  : Pointer to the AUX_ADC_DAC_COMP_Type structure. 
 * @param[in]    channel  : ADC channel to be configured as 0,1,2 ...15
 * @param[in]    oper_mode: ADC operation mode select,
 *                          oper_mode : 1 Static Mode
 *                          oper_mode : 0 Dynamic Mode
 * @return       execution status 
 */
  error_t RSI_ADC_ChnlIntrMask(AUX_ADC_DAC_COMP_Type *pstcADC, uint32_t channel, uint8_t oper_mode)
#endif
{
  if (channel > MAX_CHNL_NO) {
    return INVALID_PARAMETERS;
  }
#ifdef CHIP_9118
  if ((AUX_ADC_DAC_COMP->INTERNAL_DMA_CH_ENABLE_b.INTERNAL_DMA_ENABLE)) {
    pstcADC->INTR_MASK_REG_b.FIRST_MEM_SWITCH_INTR_MASK |= (1 << channel);
  } else {
    pstcADC->INTR_MASK_REG_b.THRESHOLD_DETECTION_INTR_EN = 1;
    pstcADC->INTR_MASK_REG_b.ADC_FIFO_AFULL_INTR_MASK    = 1;
    pstcADC->INTR_MASK_REG_b.ADC_FIFO_FULL_INTR_MASK     = 1;
  }
#endif
#ifdef CHIP_917
  if (oper_mode == ADC_STATICMODE_ENABLE) {
    pstcADC->INTR_MASK_REG_b.ADC_STATIC_MODE_DATA_INTR_MASK = 1;
  } else if (oper_mode == ADC_STATICMODE_THRESHOLD_EN) {
    pstcADC->INTR_MASK_REG_b.THRESHOLD_DETECTION_INTR_EN = 1;
    pstcADC->INTR_MASK_REG_b.ADC_FIFO_AFULL_INTR_MASK    = 1;
    pstcADC->INTR_MASK_REG_b.ADC_FIFO_FULL_INTR_MASK     = 1;
  } else {
    if ((AUX_ADC_DAC_COMP->INTERNAL_DMA_CH_ENABLE_b.INTERNAL_DMA_ENABLE)) {
      pstcADC->INTR_MASK_REG_b.FIRST_MEM_SWITCH_INTR_MASK |= (1 << channel);
    }
  }
#endif
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_ADC_ChnlIntrClr(AUX_ADC_DAC_COMP_Type *pstcADC, uint32_t channel)
 * @brief        This API is used to clear the ADC channel
 * @param[in]    pstcADC  : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    channel  : ADC channel to be configured as 2^n where n = channel number = 0,1,2,3...15
 * @return       execution status 
 */
error_t RSI_ADC_ChnlIntrClr(AUX_ADC_DAC_COMP_Type *pstcADC, uint32_t channel)
{
  if (channel > MAX_CHNL_NO) {
    return INVALID_PARAMETERS;
  }
  pstcADC->INTR_CLEAR_REG_b.INTR_CLEAR_REG |= (1 << (channel));
  pstcADC->INTR_CLEAR_REG_b.INTR_CLEAR_REG &= ~(1 << (channel));
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           uint32_t RSI_ADC_ChnlIntrStatus(AUX_ADC_DAC_COMP_Type *pstcADC, uint32_t channel)
 * @brief        This API is used to status the ADC channel
 * @param[in]    pstcADC  : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @return       execution status 
 */
uint32_t RSI_ADC_ChnlIntrStatus(AUX_ADC_DAC_COMP_Type *pstcADC)
{
#ifdef CHIP_9118
  return pstcADC->INTR_STATUS_REG_b.FIRST_MEM_SWITCH_INTR;
#endif
#ifdef CHIP_917
  return pstcADC->INTR_STATUS_REG;
#endif
}

/*==============================================*/
/**
 * @fn           void RSI_ADC_PowerControl(POWER_STATE state)
 * @brief        This API is used to Power On and off for ADC
 * @param[in]    state : ADC_POWER_ON - To powerup adc powergates
                         ADC_POWER_OFF - To powerdown adc powergates
 * @return       execution status 
 */
void RSI_ADC_PowerControl(POWER_STATE state)
{
  switch (state) {
    case ADC_POWER_ON:
      RSI_IPMU_PowerGateSet(AUXADC_PG_ENB);
      RSI_PS_UlpssPeriPowerUp(ULPSS_PWRGATE_ULP_AUX);
      break;
    case ADC_POWER_OFF:
      RSI_IPMU_PowerGateClr(AUXADC_PG_ENB);
      break;
  }
}

/*==============================================*/
/**
 * @fn           error_t RSI_ADC_NoiseAvgMode(AUX_ADC_DAC_COMP_Type *pstcADC, bool en_disable)
 * @brief        This API is used to Enable or Disable Noise averaging mode
 * @param[in]    pstcADC     : Pointer to the AUX_ADC_DAC_COMP_Type structure. 
 * @param[in]    en_disable  : 1 - To enable noise averaging mode
 *                             0 - To disable noise averaging mode           
 * @return       execution status 
 */
error_t RSI_ADC_NoiseAvgMode(AUX_ADC_DAC_COMP_Type *pstcADC, bool en_disable)
{
  pstcADC->AUXADC_CTRL_1_b.BYPASS_NOISE_AVG = en_disable;
  ULP_SPI_MEM_MAP(0x110) |= en_disable << POS17;
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_ADC_TempSensorEnable(AUX_ADC_DAC_COMP_Type *pstcADC)
 * @brief        This API is used to Enable temp-Sensor 
 * @param[in]    pstcADC             : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @return       execution status 
 */
error_t RSI_ADC_TempSensorEnable(AUX_ADC_DAC_COMP_Type *pstcADC)
{
  pstcADC->TS_PTAT_ENABLE_b.TS_PTAT_EN = 1U;
  return RSI_OK;
}

#ifdef CHIP_917

/*==============================================*/
/**
 * @fn           error_t RSI_ADC_ThresholdConfig(AUX_ADC_DAC_COMP_Type *pstcADC, int16_t threshold1, uint8_t *threshold1_cond,
 *                                int16_t threshold2, uint8_t *threshold2_cond, uint8_t range)
 * @brief        This API is used to compare threshold value with adc data 
 * @param[in]    pstcADC     : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    threshold1  : threshold value to be programmed
 * @param[in]    threshold1_cond : threshold1 condition configuration 
 *                                 THRESHOLD1_EQ   : 1
 *                                 THRESHOLD1_GRT  : 2
 *                                 THRESHOLD1_LESS : 3
 * @param[in]    threshold2  : threshold value to be programmed when range is 1 
 * @param[in]    threshold2_cond : threshold2 condition configuration 
 *                                 THRESHOLD2_EQ   : 1
 *                                 THRESHOLD2_GRT  : 2
 *                                 THRESHOLD2_LESS : 3
 * @param[in]    range       : When adc data compared lies in certain range of threshold values set this bit
 * @return       execution status 
 */
error_t RSI_ADC_ThresholdConfig(AUX_ADC_DAC_COMP_Type *pstcADC,
                                int16_t threshold1,
                                uint8_t *threshold1_cond,
                                int16_t threshold2,
                                uint8_t *threshold2_cond,
                                uint8_t range)
{
  int16_t temp_1                                                = 0;
  temp_1                                                        = pstcADC->AUXADC_DATA_b.AUXADC_DATA;
  pstcADC->ADC_DET_THR_CTRL_0_b.ADC_INPUT_DETECTION_THRESHOLD_0 = threshold1;
  pstcADC->ADC_DET_THR_CTRL_0_b.ADC_INPUT_DETECTION_THRESHOLD_1 = (threshold1 << 8);

  if ((pstcADC->ADC_DET_THR_CTRL_0_b.ADC_INPUT_DETECTION_THRESHOLD_0
       | (pstcADC->ADC_DET_THR_CTRL_0_b.ADC_INPUT_DETECTION_THRESHOLD_1 >> 8))
      <= temp_1) {
    if ((pstcADC->ADC_DET_THR_CTRL_0_b.ADC_INPUT_DETECTION_THRESHOLD_0
         | (pstcADC->ADC_DET_THR_CTRL_0_b.ADC_INPUT_DETECTION_THRESHOLD_1 >> 8))
        == temp_1) {
      pstcADC->ADC_DET_THR_CTRL_0_b.COMP_EQ_EN = 1;
      *(threshold1_cond)                       = THRESHOLD1_EQ;
    } else {
      pstcADC->ADC_DET_THR_CTRL_0_b.COMP_GRTR_THAN_EN = 1;
      *(threshold1_cond)                              = THRESHOLD1_GRT;
    }
  } else {
    pstcADC->ADC_DET_THR_CTRL_0_b.COMP_LESS_THAN_EN = 1;
    *(threshold1_cond)                              = THRESHOLD1_LESS;
  }

  if (range) {
    pstcADC->ADC_DET_THR_CTRL_0_b.RANGE_COMPARISON_ENABLE = 1U;

    pstcADC->ADC_DET_THR_CTRL_1_b.ADC_INPUT_DETECTION_THRESHOLD_2      = threshold2;
    pstcADC->ADC_DET_THR_CTRL_1_b.ADC_DETECTION_THRESHOLD_4_UPPER_BITS = (threshold2 << 8);

    if ((pstcADC->ADC_DET_THR_CTRL_1_b.ADC_INPUT_DETECTION_THRESHOLD_2
         | (pstcADC->ADC_DET_THR_CTRL_1_b.ADC_DETECTION_THRESHOLD_4_UPPER_BITS >> 8))
        <= temp_1) {
      if ((pstcADC->ADC_DET_THR_CTRL_1_b.ADC_INPUT_DETECTION_THRESHOLD_2
           | (pstcADC->ADC_DET_THR_CTRL_1_b.ADC_DETECTION_THRESHOLD_4_UPPER_BITS >> 8))
          == temp_1) {
        pstcADC->ADC_DET_THR_CTRL_1_b.COMP_EQ_EN = 1;
        *(threshold2_cond)                       = THRESHOLD2_EQ;
      } else {
        pstcADC->ADC_DET_THR_CTRL_1_b.COMP_GRTR_THAN_EN = 1;
        *(threshold2_cond)                              = THRESHOLD2_GRT;
      }
    } else {
      pstcADC->ADC_DET_THR_CTRL_1_b.COMP_LESS_THAN_EN = 1;
      *(threshold2_cond)                              = THRESHOLD2_LESS;
    }
  }
  return RSI_OK;
}
#endif

/*==============================================*/
/**
 * @fn           error_t RSI_ADC_Bbp(AUX_ADC_DAC_COMP_Type *pstcADC ,uint8_t adc_bbp_en, uint8_t bbp_en, uint8_t bbp_id)
 * @brief        This API is used for baseband
 * @param[in]    pstcADC    : Pointer to the AUX_ADC_DAC_COMP_Type structure. 
 * @param[in]    adc_bbp_en : To enable adc to bbp path
 * @param[in]    bbp_en     : Adc samples are given to bbp
 * @param[in]    bbp_id     : Channel id for bbp samples
 * @return       execution status 
 */
error_t RSI_ADC_Bbp(AUX_ADC_DAC_COMP_Type *pstcADC, uint8_t adc_bbp_en, uint8_t bbp_en, uint8_t bbp_id)
{
  pstcADC->VAD_BBP_ID_b.AUX_ADC_BPP_EN = adc_bbp_en;
  pstcADC->VAD_BBP_ID_b.BPP_EN         = bbp_en;
  pstcADC->VAD_BBP_ID_b.BPP_ID         = bbp_id;

  return RSI_OK;
}

#ifdef CHIP_917

/*==============================================*/
/**
 * @fn          error_t RSI_ADC_InterruptHandler( AUX_ADC_DAC_COMP_Type *pstcADC,RSI_ADC_CALLBACK_T *pADCCallBack)
 * @brief		    Handles all interrupt of ADC
 * @param[in]	  pstcADC   :  Pointer to the MCPWM instance register area
 * @param[in]	  pADCCallBack Pointer to the structure of type RSI_CALLBACK
 */
error_t RSI_ADC_InterruptHandler(AUX_ADC_DAC_COMP_Type *pstcADC)
{
  uint32_t intr_status = 0;

  intr_status = RSI_ADC_ChnlIntrStatus(AUX_ADC_DAC_COMP);

  if ((intr_status & ADC_STATIC_MODE_INTR) && (pstcADC->INTR_MASK_REG_b.ADC_STATIC_MODE_DATA_INTR_MASK == 0)) {
#ifdef CHIP_917
    RSI_ADC_ChnlIntrMask(AUX_ADC_DAC_COMP, 0, ADC_STATICMODE_ENABLE);
#endif
    adc_commn_config.call_back_event(ADC_CHNL0_INTR, ADC_STATIC_MODE_CALLBACK);
  } else if (intr_status & ADC_THRESHOLD_DET_INTR) {
    RSI_ADC_ThreshInterruptClr(AUX_ADC_DAC_COMP);
    adc_commn_config.call_back_event(ADC_CHNL0_INTR, ADC_THRSHOLD_CALLBACK);
  }
#ifdef CHIP_917
  else if (intr_status & DAC_STATIC_MODE_INTR) {
    RSI_DAC_InterruptMask(AUX_ADC_DAC_COMP, 1);
    dac_callback_fun.callback_event(DAC_STATIC_MODE_CALLBACK);
  }
#endif
  else {
    if ((AUX_ADC_DAC_COMP->AUXADC_CTRL_1_b.ADC_MULTIPLE_CHAN_ACTIVE)
        || (AUX_ADC_DAC_COMP->INTERNAL_DMA_CH_ENABLE_b.INTERNAL_DMA_ENABLE)) {
      if (intr_status != NULL) {
        if (intr_status & BIT(7)) {
          RSI_ADC_ChnlIntrClr(AUX_ADC_DAC_COMP, ADC_CHNL0_INTR);
          ADC_PingPongReconfig(INTERNAL_DMA, ADC_CHNL0_INTR);
          adc_commn_config.call_back_event(ADC_CHNL0_INTR, INTERNAL_DMA);
        } else if (intr_status & BIT(8)) {
          RSI_ADC_ChnlIntrClr(AUX_ADC_DAC_COMP, ADC_CHNL1_INTR);
          ADC_PingPongReconfig(INTERNAL_DMA, ADC_CHNL1_INTR);
          adc_commn_config.call_back_event(ADC_CHNL1_INTR, INTERNAL_DMA);
        } else if (intr_status & BIT(9)) {
          RSI_ADC_ChnlIntrClr(AUX_ADC_DAC_COMP, ADC_CHNL2_INTR);
          ADC_PingPongReconfig(INTERNAL_DMA, ADC_CHNL2_INTR);
          adc_commn_config.call_back_event(ADC_CHNL2_INTR, INTERNAL_DMA);
        } else if (intr_status & BIT(10)) {
          RSI_ADC_ChnlIntrClr(AUX_ADC_DAC_COMP, ADC_CHNL3_INTR);
          ADC_PingPongReconfig(INTERNAL_DMA, ADC_CHNL3_INTR);
          adc_commn_config.call_back_event(ADC_CHNL3_INTR, INTERNAL_DMA);
        } else if (intr_status & BIT(11)) {
          RSI_ADC_ChnlIntrClr(AUX_ADC_DAC_COMP, ADC_CHNL4_INTR);
          ADC_PingPongReconfig(INTERNAL_DMA, ADC_CHNL4_INTR);
          adc_commn_config.call_back_event(ADC_CHNL4_INTR, INTERNAL_DMA);
        } else if (intr_status & BIT(12)) {
          RSI_ADC_ChnlIntrClr(AUX_ADC_DAC_COMP, ADC_CHNL5_INTR);
          ADC_PingPongReconfig(INTERNAL_DMA, ADC_CHNL5_INTR);
          adc_commn_config.call_back_event(ADC_CHNL5_INTR, INTERNAL_DMA);
        } else if (intr_status & BIT(13)) {
          RSI_ADC_ChnlIntrClr(AUX_ADC_DAC_COMP, ADC_CHNL6_INTR);
          ADC_PingPongReconfig(INTERNAL_DMA, ADC_CHNL6_INTR);
          adc_commn_config.call_back_event(ADC_CHNL6_INTR, INTERNAL_DMA);
        } else if (intr_status & BIT(14)) {
          RSI_ADC_ChnlIntrClr(AUX_ADC_DAC_COMP, ADC_CHNL7_INTR);
          ADC_PingPongReconfig(INTERNAL_DMA, ADC_CHNL7_INTR);
          adc_commn_config.call_back_event(ADC_CHNL7_INTR, INTERNAL_DMA);
        } else if (intr_status & BIT(15)) {
          RSI_ADC_ChnlIntrClr(AUX_ADC_DAC_COMP, ADC_CHNL8_INTR);
          ADC_PingPongReconfig(INTERNAL_DMA, ADC_CHNL8_INTR);
          adc_commn_config.call_back_event(ADC_CHNL8_INTR, INTERNAL_DMA);
        } else if (intr_status & BIT(16)) {
          RSI_ADC_ChnlIntrClr(AUX_ADC_DAC_COMP, ADC_CHNL9_INTR);
          ADC_PingPongReconfig(INTERNAL_DMA, ADC_CHNL9_INTR);
          adc_commn_config.call_back_event(ADC_CHNL9_INTR, INTERNAL_DMA);
        } else if (intr_status & BIT(17)) {
          RSI_ADC_ChnlIntrClr(AUX_ADC_DAC_COMP, ADC_CHNL10_INTR);
          ADC_PingPongReconfig(INTERNAL_DMA, ADC_CHNL10_INTR);
          adc_commn_config.call_back_event(ADC_CHNL10_INTR, INTERNAL_DMA);
        } else if (intr_status & BIT(18)) {
          RSI_ADC_ChnlIntrClr(AUX_ADC_DAC_COMP, ADC_CHNL11_INTR);
          ADC_PingPongReconfig(INTERNAL_DMA, ADC_CHNL11_INTR);
          adc_commn_config.call_back_event(ADC_CHNL11_INTR, INTERNAL_DMA);
        } else if (intr_status & BIT(19)) {
          RSI_ADC_ChnlIntrClr(AUX_ADC_DAC_COMP, ADC_CHNL12_INTR);
          ADC_PingPongReconfig(INTERNAL_DMA, ADC_CHNL12_INTR);
          adc_commn_config.call_back_event(ADC_CHNL12_INTR, INTERNAL_DMA);
        } else if (intr_status & BIT(20)) {
          RSI_ADC_ChnlIntrClr(AUX_ADC_DAC_COMP, ADC_CHNL13_INTR);
          ADC_PingPongReconfig(INTERNAL_DMA, ADC_CHNL13_INTR);
          adc_commn_config.call_back_event(ADC_CHNL13_INTR, INTERNAL_DMA);
        } else if (intr_status & BIT(21)) {
          RSI_ADC_ChnlIntrClr(AUX_ADC_DAC_COMP, ADC_CHNL14_INTR);
          ADC_PingPongReconfig(INTERNAL_DMA, ADC_CHNL14_INTR);
          adc_commn_config.call_back_event(ADC_CHNL14_INTR, INTERNAL_DMA);
        } else if (intr_status & BIT(22)) {
          RSI_ADC_ChnlIntrClr(AUX_ADC_DAC_COMP, ADC_CHNL15_INTR);
          ADC_PingPongReconfig(INTERNAL_DMA, ADC_CHNL15_INTR);
          adc_commn_config.call_back_event(ADC_CHNL15_INTR, INTERNAL_DMA);
        }
      } else {
      }
    }
    if ((!(AUX_ADC_DAC_COMP->AUXADC_CTRL_1_b.ADC_STATIC_MODE))
        && (!(AUX_ADC_DAC_COMP->INTERNAL_DMA_CH_ENABLE_b.INTERNAL_DMA_ENABLE))) {
      AUX_ADC_DAC_COMP->AUXADC_CTRL_1_b.ADC_FIFO_FLUSH = 1;
      adc_commn_config.call_back_event(intr_status, FIFO_MODE_EVENT);
    }
  }
  return RSI_OK;
}
#endif

/*==============================================*/
/**
 * @fn           void RSI_ADC_ThreshInterruptClr(AUX_ADC_DAC_COMP_Type *pstcADC)
 * @brief        This API is used clear the threshold interrupt.
 * @param[in]    pstcADC     : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @return       None
 */
void RSI_ADC_ThreshInterruptClr(AUX_ADC_DAC_COMP_Type *pstcADC)
{
  pstcADC->INTR_CLEAR_REG_b.CLR_INTR = 1;
}

/*==============================================*/
/**
 * @fn           void RSI_ADC_AUXLdoConfig(AUX_ADC_DAC_COMP_Type *pstcADC,uint16_t bypass_en,uint16_t value)
 * @brief        This API is used enable or disable the LDO for application.
 * @param[in]    pstcADC   : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    bypass_en : This parameter define LDO bypass enable or disable.
 *                           bypass_en : 1 bypass the LDO    
 *                           bypass_en : 0 On LDO mode 
 * @param[in]    value     : Configuration value for LDO when bypass_en =1.
 * @return       None
 */
void RSI_ADC_AUXLdoConfig(AUX_ADC_DAC_COMP_Type *pstcADC, uint16_t bypass_en, uint16_t value)
{
  pstcADC->AUX_LDO_b.ENABLE_LDO       = 1;
  pstcADC->AUX_LDO_b.LDO_DEFAULT_MODE = 1;

  if (bypass_en) {
    pstcADC->AUX_LDO_b.BYPASS_LDO = 1;
    pstcADC->AUX_LDO_b.LDO_CTRL   = value;
  } else {
    pstcADC->AUX_LDO_b.LDO_CTRL = value;
  }
}

/*==============================================*/
/**
 * @fn           int16_t RSI_ADC_GainOffsetCal(int16_t data,uint8_t diff_en)
 * @brief        This API is used to calculate gain and offset value.
 * @param[in]    data : data sample. 
 * @param[in]    diff_en : This parameter define differential mode is enable or disable.
 * @return       None
 */
int16_t RSI_ADC_GainOffsetCal(int16_t data, uint8_t diff_en)
{
  if (data & BIT(11)) {
    data = data & (ADC_MASK_VALUE);
  } else {
    data = data | BIT(11);
  }

  if (diff_en) {
    data = ((data - adc_commn_config.adc_diff_offset) * adc_commn_config.adc_diff_gain);
  } else {
    data = ((data - adc_commn_config.adc_sing_offset) * adc_commn_config.adc_sing_gain);
  }
  if (data > 4095) {
    data = 4095;
  }
  if (data <= 0) {
    data = 0;
  }
  if (data >= 2048) {
    data = data - 2048;
  } else {
    data = data + 2048;
  }
  return data;
}

#ifdef ADC_MULTICHANNEL_WITH_EXT_DMA
/**
 * @brief  This function is callback function to reconfigure uDMA ping and pong descriptor.
 * @param[in]  udmaHandle  :  uDMA handler
 * @param[in]  pTranDesc   :  Event type 
 * @return None
 */
void adc_udmaTransferComplete(RSI_UDMA_HANDLE_T udmaHandle, RSI_UDMA_DESC_T *pTranDesc, uint32_t channel_no)
{
}

/*==============================================*/
/**
 * @fn     void UDMA_Start(void)
 * @brief  Enable the UDMA channel for operation.
 * @return None
 */
void UDMA_ADC_Start(void)
{
  RSI_UDMA_ChannelEnable(udmaHandle1, 11);

  RSI_UDMA_UDMAEnable(udmaHandle1);

  /* uDMA NVIC enable */
  NVIC_EnableIRQ(UDMA1_IRQn);
}

/*==============================================*/
/**
 * @fn     void UDMA_ADC_Pong_Write(uint8_t reconfig)
 * @brief  This function use for configure descriptor fo primary or alternate structure.
 * @param[in]  reconfig : This parameter use for skip uDMA initialization function in 
 *                     ping descriptor reconfiguration time.
 *         reconfig : 1 Dont skip configuration 
 *         reconfig : 0 skip configuration 
 * @return None
 */
void UDMA_ADC_Pong_Write(uint8_t reconfig)
{
  RSI_UDMA_CHA_CONFIG_DATA_T control = { 0 };
  RSI_UDMA_CHA_CFG_T config          = { 0 };

  config.altStruct       = 1;
  config.burstReq        = 1;
  config.channelPrioHigh = 0;
  config.periAck         = 0;
  config.periphReq       = 0;
  config.reqMask         = 0;
  config.dmaCh           = 11;

  // Set up source to destination copy for trigger for memory
  // Descriptor configurations
  control.transferType       = UDMA_MODE_PINGPONG;
  control.nextBurst          = 0;
  control.totalNumOfDMATrans = UDMA_PING_PONG_LENGTH - 1;
  control.rPower             = ARBSIZE_4;
  control.srcProtCtrl        = 0x000;
  control.dstProtCtrl        = 0x000;
  control.srcSize            = SRC_SIZE_16;
  control.srcInc             = SRC_INC_NONE;
  control.dstSize            = DST_SIZE_16;
  control.dstInc             = DST_INC_16;

  if (reconfig) {
    RSI_UDMA_SetupChannel(udmaHandle1, &config);
  }

  RSI_UDMA_SetupChannelTransfer(udmaHandle1,
                                &config,
                                control,
                                (uint32_t *)ADC_OUTPUT_REG,
                                (uint32_t *)(adc_output_buf));
}

/*==============================================*/
/**
 * @fn         void UDMA_ADC_Ping_Write(uint8_t ping_reconfig)
 * @brief      This function use for configure descriptor fo primary or alternate structure.
 * @param[in]  reconfig : This parameter use for skip uDMA initialization function in 
 *                        ping descriptor reconfiguration time.
 *             reconfig : 1 Dont skip configuration 
 *             reconfig : 0 skip configuration 
 * @return None
 */
void UDMA_ADC_Ping_Write(uint8_t ping_reconfig)
{
  RSI_UDMA_CHA_CONFIG_DATA_T control = { 0 };
  RSI_UDMA_CHA_CFG_T config          = { 0 };

  config.altStruct       = 0;
  config.burstReq        = 1;
  config.channelPrioHigh = 0;
  config.periAck         = 0;
  config.periphReq       = 0;
  config.reqMask         = 0;
  config.dmaCh           = 11;

  // Set up source to destination copy for trigger for memory
  // Descriptor configurations
  control.transferType       = UDMA_MODE_PINGPONG;
  control.nextBurst          = 0;
  control.totalNumOfDMATrans = UDMA_PING_PONG_LENGTH - 1;
  control.rPower             = ARBSIZE_4;
  control.srcProtCtrl        = 0x000;
  control.dstProtCtrl        = 0x000;
  control.srcSize            = SRC_SIZE_16;
  control.srcInc             = SRC_INC_NONE;
  control.dstSize            = DST_SIZE_16;
  control.dstInc             = DST_INC_16;

  if (ping_reconfig) {
    RSI_UDMA_SetupChannel(udmaHandle1, &config);
  }

  RSI_UDMA_SetupChannelTransfer(udmaHandle1,
                                &config,
                                control,
                                (uint32_t *)ADC_OUTPUT_REG,
                                (uint32_t *)(adc_output_buf + (UDMA_PING_PONG_LENGTH)));
}

/*==============================================*/
/**
 * @fn     void UDMA_ADC_Init(void)
 * @brief  Configure UDMA clock and parameters.
 * @retval None
 */
void UDMA_ADC_Init(void)
{
  RSI_UDMA_INIT_T udmaInit;
  uint32_t devMem[30];
  volatile bool done1 = NULL;

  RSI_ULPSS_PeripheralEnable(ULPCLK, ULP_UDMA_CLK, ENABLE_STATIC_CLK);

  RSI_PS_UlpssPeriPowerUp(ULPSS_PWRGATE_ULP_UDMA);

  udmaInit.pUserData = (void *)&done1;
  udmaInit.base      = (uint32_t)UDMA1;
  udmaInit.sramBase  = (uint32_t)&UDMA1_Table;
  udmaHandle1        = RSI_UDMA_Init(devMem, &udmaInit);

  // Configure the UDMA ping descriptor
  UDMA_ADC_Pong_Write(1);

  // Configure the UDMA pong descriptor
  UDMA_ADC_Ping_Write(1);

  // Soft-ware trigger for uDMA channel
  RSI_UDMA_ChannelSoftwareTrigger(udmaHandle1, 11);

  RSI_UDMA_RegisterCallback(udmaHandle1, (void *)adc_udmaTransferComplete);
}
#endif

#ifdef CHIP_917

/*==============================================*/
/**
 * @fn           error_t RSI_ADC_DectEdgeSel(uint8_t detection_num, uint8_t edge_sel)
 * @brief        This API is used to select detection edge.
 * @param[in]    detection_num      : Detection Number
 * @param[in]    edge_sel           : This parameter use for select edge for trigger operation.
 * @return       Execution Status
 */
error_t RSI_ADC_DectEdgeSel(uint8_t detection_num, uint8_t edge_sel)
{
  if (edge_sel == POSITIVE_EDGE) {
    if (detection_num == DETECTION1) {
      AUX_ADC_DAC_COMP->ADC_CTRL_REG_2_b.EXT_TRIG_DETECT_1 = 1;
    } else if (detection_num == DETECTION2) {
      AUX_ADC_DAC_COMP->ADC_CTRL_REG_2_b.EXT_TRIG_DETECT_2 = 1;
    } else if (detection_num == DETECTION3) {
      AUX_ADC_DAC_COMP->ADC_CTRL_REG_2_b.EXT_TRIG_DETECT_3 = 1;
    } else if (detection_num == DETECTION4) {
      AUX_ADC_DAC_COMP->ADC_CTRL_REG_2_b.EXT_TRIG_DETECT_4 = 1;
    } else {
      return ERROR_ADC_INVALID_ARG;
    }
  } else if (edge_sel == NEGATIVE_EDGE) {
    if (detection_num == DETECTION1) {
      AUX_ADC_DAC_COMP->ADC_CTRL_REG_2_b.EXT_TRIG_DETECT_1 = 2;
    } else if (detection_num == DETECTION2) {
      AUX_ADC_DAC_COMP->ADC_CTRL_REG_2_b.EXT_TRIG_DETECT_2 = 2;
    } else if (detection_num == DETECTION3) {
      AUX_ADC_DAC_COMP->ADC_CTRL_REG_2_b.EXT_TRIG_DETECT_3 = 2;
    } else if (detection_num == DETECTION4) {
      AUX_ADC_DAC_COMP->ADC_CTRL_REG_2_b.EXT_TRIG_DETECT_4 = 2;
    } else {
      return ERROR_ADC_INVALID_ARG;
    }
  } else if (edge_sel == POS_NEG_EDGE) {
    if (detection_num == DETECTION1) {
      AUX_ADC_DAC_COMP->ADC_CTRL_REG_2_b.EXT_TRIG_DETECT_1 = 3;
    } else if (detection_num == DETECTION2) {
      AUX_ADC_DAC_COMP->ADC_CTRL_REG_2_b.EXT_TRIG_DETECT_2 = 3;
    } else if (detection_num == DETECTION3) {
      AUX_ADC_DAC_COMP->ADC_CTRL_REG_2_b.EXT_TRIG_DETECT_3 = 3;
    } else if (detection_num == DETECTION4) {
      AUX_ADC_DAC_COMP->ADC_CTRL_REG_2_b.EXT_TRIG_DETECT_4 = 3;
    } else {
      return ERROR_ADC_INVALID_ARG;
    }
  } else {
    return ERROR_ADC_INVALID_ARG;
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_ADC_ChnlTriggSel(uint8_t ext_sel_num, uint8_t chnl_id)
 * @brief        This API is used to select channel trigger selection.
 * @param[in]    ext_sel_num       : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    chnl_id           : Channel Number
 * @return       Execution Status
 */
error_t RSI_ADC_ChnlTriggSel(uint8_t ext_sel_num, uint8_t chnl_id)
{
  if (ext_sel_num == EXT_TRIGGER_SEL1) {
    AUX_ADC_DAC_COMP->ADC_CTRL_REG_2_b.EXT_TRIGGER_SEL_1 = chnl_id;
  } else if (ext_sel_num == EXT_TRIGGER_SEL2) {
    AUX_ADC_DAC_COMP->ADC_CTRL_REG_2_b.EXT_TRIGGER_SEL_2 = chnl_id;
  } else if (ext_sel_num == EXT_TRIGGER_SEL3) {
    AUX_ADC_DAC_COMP->ADC_CTRL_REG_2_b.EXT_TRIGGER_SEL_3 = chnl_id;
  } else if (ext_sel_num == EXT_TRIGGER_SEL4) {
    AUX_ADC_DAC_COMP->ADC_CTRL_REG_2_b.EXT_TRIGGER_SEL_4 = chnl_id;
  } else {
    return ERROR_ADC_INVALID_ARG;
  }

  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_ADC_ExtTrigConfig(AUX_ADC_DAC_COMP_Type *pstcADC, adc_extr_config_t ExtrTrigConfig)
 * @brief        This API is used clear the threshold interrupt.
 * @param[in]    pstcADC           : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    ExtrTrigConfig    : ADC external trigger configuration structure variable. .
 * @return       Execution Status
 */
error_t RSI_ADC_ExtTrigConfig(AUX_ADC_DAC_COMP_Type *pstcADC, adc_extr_config_t ExtrTrigConfig)
{
  if (ExtrTrigConfig.trigger_type == ULP_TIMER_EXT_TRIGGER) {
    if (ExtrTrigConfig.trigger_num == DETECTION1) {
      // Timer 0 Select for trigger
      ULPCLK->ULP_MISC_SOFT_SET_REG_b.AUX_ULP_EXT_TRIG_1_SEL_b = 1;
    } else if (ExtrTrigConfig.trigger_num == DETECTION2) {
      // Timer 1 Select for trigger
      ULPCLK->ULP_MISC_SOFT_SET_REG_b.AUX_ULP_EXT_TRIG_2_SEL_b = 1;
    } else if (ExtrTrigConfig.trigger_num == DETECTION3) {
      // Timer 2 Select for trigger
      ULPCLK->ULP_MISC_SOFT_SET_REG_b.AUX_SOC_EXT_TRIG_1_SEL_b = 1;
    } else if (ExtrTrigConfig.trigger_num == DETECTION4) {
      ULPCLK->ULP_MISC_SOFT_SET_REG_b.AUX_SOC_EXT_TRIG_2_SEL_b = 1;
    } else {
      return ERROR_ADC_INVALID_ARG;
    }
  } else if (ExtrTrigConfig.trigger_type == ULP_GPIO_EXT_TRIGGER) {
    if (ExtrTrigConfig.trigger_num == DETECTION1) {
      ULPCLK->ULP_MISC_SOFT_SET_REG_b.AUX_ULP_EXT_TRIG_1_SEL_b = 0;
    } else if (ExtrTrigConfig.trigger_num == DETECTION2) {
      ULPCLK->ULP_MISC_SOFT_SET_REG_b.AUX_ULP_EXT_TRIG_2_SEL_b = 0;
    } else {
      return ERROR_ADC_INVALID_ARG;
    }
  } else if (ExtrTrigConfig.trigger_type == M4_CT_EXT_TRIGGER) {
    if (ExtrTrigConfig.trigger_num == DETECTION3) {
      ULPCLK->ULP_MISC_SOFT_SET_REG_b.AUX_SOC_EXT_TRIG_1_SEL_b = 0;
    } else if (ExtrTrigConfig.trigger_num == DETECTION4) {
      ULPCLK->ULP_MISC_SOFT_SET_REG_b.AUX_SOC_EXT_TRIG_2_SEL_b = 0;
    } else {
      return ERROR_ADC_INVALID_ARG;
    }
  } else {
    return ERROR_ADC_INVALID_ARG;
  }
  if (RSI_OK != RSI_ADC_DectEdgeSel(ExtrTrigConfig.trigger_num, ExtrTrigConfig.trigger_num)) {
    return ERROR_ADC_INVALID_ARG;
  }
  if (RSI_OK != RSI_ADC_ChnlTriggSel(ExtrTrigConfig.trigger_sel, ExtrTrigConfig.trigger_sel_val)) {
    return ERROR_ADC_INVALID_ARG;
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           uint8_t RSI_ADC_ExtTrigStatusRead(AUX_ADC_DAC_COMP_Type *pstcADC, adc_extr_config_t ExtrTrigConfig)
 * @brief        This API is used read external triggger status.
 * @param[in]    pstcADC           : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    ExtrTrigConfig    : ADC external trigger configuration structure variable. .
 * @return       If API return non value then interrupt match value occured
 */
uint8_t RSI_ADC_ExtTrigStatusRead(AUX_ADC_DAC_COMP_Type *pstcADC, adc_extr_config_t ExtrTrigConfig)
{
  if (ExtrTrigConfig.trigger_num == DETECTION1) {
    return (AUX_ADC_DAC_COMP->ADC_CTRL_REG_2_b.TRIG_1_MATCH);
  } else if (ExtrTrigConfig.trigger_num == DETECTION2) {
    return (AUX_ADC_DAC_COMP->ADC_CTRL_REG_2_b.TRIG_2_MATCH);
  } else if (ExtrTrigConfig.trigger_num == DETECTION3) {
    return (AUX_ADC_DAC_COMP->ADC_CTRL_REG_2_b.TRIG_3_MATCH);
  } else if (ExtrTrigConfig.trigger_num == DETECTION4) {
    return (AUX_ADC_DAC_COMP->ADC_CTRL_REG_2_b.TRIG_4_MATCH);
  }
}

/*==============================================*/
/**
 * @fn           error_t RSI_ADC_ExtTrigStatusClear(AUX_ADC_DAC_COMP_Type *pstcADC, adc_extr_config_t ExtrTrigConfig)
 * @brief        This API is used clear external trigger status.
 * @param[in]    pstcADC           : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    ExtrTrigConfig    : ADC external trigger configuration structure variable. .
 * @return       If API return non value then interrupt match value occured
 *
 */
error_t RSI_ADC_ExtTrigStatusClear(AUX_ADC_DAC_COMP_Type *pstcADC, adc_extr_config_t ExtrTrigConfig)
{
  if (ExtrTrigConfig.trigger_num == DETECTION1) {
    AUX_ADC_DAC_COMP->ADC_CTRL_REG_2_b.TRIG_1_MATCH = 1;
    AUX_ADC_DAC_COMP->ADC_CTRL_REG_2_b.TRIG_1_MATCH = 0;
  } else if (ExtrTrigConfig.trigger_num == DETECTION2) {
    AUX_ADC_DAC_COMP->ADC_CTRL_REG_2_b.TRIG_2_MATCH = 1;
    AUX_ADC_DAC_COMP->ADC_CTRL_REG_2_b.TRIG_2_MATCH = 0;
  } else if (ExtrTrigConfig.trigger_num == DETECTION3) {
    AUX_ADC_DAC_COMP->ADC_CTRL_REG_2_b.TRIG_3_MATCH = 1;
    AUX_ADC_DAC_COMP->ADC_CTRL_REG_2_b.TRIG_3_MATCH = 0;
  } else if (ExtrTrigConfig.trigger_num == DETECTION4) {
    AUX_ADC_DAC_COMP->ADC_CTRL_REG_2_b.TRIG_4_MATCH = 1;
    AUX_ADC_DAC_COMP->ADC_CTRL_REG_2_b.TRIG_4_MATCH = 0;
  }
}
/** @} */
#endif
