/*******************************************************************************
* @file  rsi_dac.c
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
#include "clock_update.h"
#include "math.h"
#include "UDMA.h"

// UDMA config
// DMA descriptors must be aligned to 16 bytes
#if defined(__CC_ARM)
extern RSI_UDMA_DESC_T UDMA1_Table[32];
#endif // defined (__CC_ARM)

#if defined(__GNUC__)
extern RSI_UDMA_DESC_T __attribute__((section(".udma_addr1"))) UDMA1_Table[32];
#endif // defined (__GNUC__)

extern RSI_UDMA_HANDLE_T udmaHandle1;

dac_config_t dac_callback_fun;
uint8_t dac_pong_enable_sel = 0;
uint32_t devMem[30];

/*==============================================*/
/**
 * @fn           uint32_t DAC_Init(uint8_t operation_mode,uint32_t sampling_rate,daccallbacFunc event)
 * @brief        This API used for initialize DAC operation in static mode or in FIFO mode. 
 * @param[in]    operation_mode     : This parameter define DAC in FIFO mode or in static mode
 *                                    Configure operation_mode = 0  , for Fifo mode
 *                                    Configure operation_mode = 1  , for Static mode 
 * @param[in]    sampling_rate      : Sampling rate for DAC operation.
 * @param[in]    event              : Call back event pointer. 
 * @return       Return the sampling value which configured in DAC module.
 */
uint32_t DAC_Init(uint8_t operation_mode, uint32_t sampling_rate, daccallbacFunc event)
{
  uint32_t achived_sample_freq = 0;

  dac_callback_fun.callback_event = event;

  if (sampling_rate > MAX_DAC_SAMP_RATE) {
    return INVALID_PARAMETERS;
  }

  // DAC power gate enable
  RSI_DAC_PowerControl(DAC_POWER_ON);

  // Configure sampling rate
  achived_sample_freq = dac_set_clock(sampling_rate);

  if (operation_mode) {
    // Configure the DAC control parameter
    RSI_DAC_Config(AUX_ADC_DAC_COMP, operation_mode, ENABLE, DISABLE, DISABLE);
  } else {
    //Set the DAC threshold value
    RSI_DAC_SetFifoThreshold(AUX_ADC_DAC_COMP, DAC_FIFO_THR);
    // Configure the DAC control parameter
    RSI_DAC_Config(AUX_ADC_DAC_COMP, operation_mode, ENABLE, DISABLE, ENABLE);
#ifdef DAC_FIFO_MODE_EN
    // Configure the UDMA for DAC peripheral
    dac_udma_init();
#endif
  }

  //Configure DAC output on AGPIO4
  DAC_PinMux(0);

  return achived_sample_freq;
}

/*==============================================*/
/**
 * @fn           error_t DAC_WriteData(uint8_t operation_mode,int16_t *wr_buf,uint16_t length)
 * @brief        This API use for writing digital sample value in DAC input register. 
 * @param[in]    operation_mode     : This parameter define DAC in FIFO mode or in static mode
 *                                    Configure operation_mode = 0  , for Fifo mode
 *                                    Configure operation_mode = 1  , for Static mode
 * @param[in]    wr_buf             : Input samples buffer pointer.
 * @param[in]    length             : Number of sample, This value will be one when DAC 
 *                                    using static mode for operation.
 * @return       Return zero on successful API execution.         
 */
error_t DAC_WriteData(uint8_t operation_mode, int16_t *wr_buf, uint16_t length)
{
  if (operation_mode) {
    // writing digital sample value in DAC input register in DAC in static mode
    RSI_DAC_WriteData(AUX_ADC_DAC_COMP, wr_buf, STATIC_MODE_EN, length);
  } else {
#ifdef DAC_FIFO_MODE_EN
    // Enable UDMA for DAC channel
    // Configure ping descriptor address
    dac_udma_write(1, length, wr_buf, ENABLE);
    // Configure pong descriptor address
    dac_udma_write(0, length, wr_buf, ENABLE);
#endif
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t DAC_Start(uint8_t operation_mode)
 * @brief        Start DAC operation.  
 * @param[in]    operation_mode     : This parameter define DAC in FIFO mode or in static mode
 *                                    Configure operation_mode = 0  , for Fifo mode
 *                                    Configure operation_mode = 1  , for Static mode
 * @return       Return zero on successful API execution.
 */
error_t DAC_Start(uint8_t operation_mode)
{
  // FIFO mode enable
  if (operation_mode) {
#ifdef CHIP_917
    RSI_DAC_InterruptUnMask(AUX_ADC_DAC_COMP, operation_mode);
    NVIC_EnableIRQ(ADC_IRQn);
#endif
    // Enable DAC
    RSI_DAC_Start(AUX_ADC_DAC_COMP, ENABLE);
  } else {
#ifdef DAC_FIFO_MODE_EN
    dac_udma_start();
#endif
    // Enable DAC
    RSI_DAC_Start(AUX_ADC_DAC_COMP, ENABLE);
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t DAC_PingPongReconfig(int16_t *wr_buf,uint16_t length)
 * @brief        This API used for reconfigure the udma ping or pong descriptor for 
 *               playing continuous digital word in DAC input buffer. 
 *               This API used in DAC in FIFO mode.
 * @param[in]    wr_buf  : Input samples buffer pointer.
 * @param[in]    length  : Number of samples to play in DAC.
 * @return       Return zero on successful API execution.
 */
error_t DAC_PingPongReconfig(int16_t *wr_buf, uint16_t length)
{
#ifdef DAC_FIFO_MODE_EN
  if (dac_pong_enable_sel) {
    dac_udma_write(dac_pong_enable_sel, length, wr_buf, DISABLE);
    dac_pong_enable_sel = 0;
  } else {
    dac_udma_write(dac_pong_enable_sel, length, wr_buf, DISABLE);
    dac_pong_enable_sel = 1;
  }
#endif
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t DAC_Stop(void)
 * @brief        Stop DAC operation.  
 * @return       Return zero on successful API execution.
 */
error_t DAC_Stop(void)
{
  RSI_DAC_Stop(AUX_ADC_DAC_COMP);
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t DAC_Deinit(void)
 * @brief        This API used for De-initialize DAC.  
 * @return       Return zero on successful API execution.
 */
error_t DAC_Deinit(void)
{
  // DAC power gate enable
  RSI_DAC_PowerControl(DAC_POWER_OFF);
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           uint32_t dac_set_clock(uint32_t sampl_rate)
 * @brief        Set DAC sampling rate.
 * @param[in]    sampling_rate      : Sampling rate for DAC operation.
 * @return       Return the sampling value which configured in DAC module.
 */
uint32_t dac_set_clock(uint32_t sampl_rate)
{
  uint32_t clk_src_val = 0;
  uint16_t clk_div_fac = 0;

  // Check the AUX-Clock is eable or not
  if (ULPCLK->ULP_AUXADC_CLK_GEN_REG_b.ULP_AUX_CLK_EN_b) {
    clk_src_val = RSI_CLK_GetBaseClock(ULPSS_AUX);

    if ((clk_src_val * 2) >= sampl_rate) {
      clk_div_fac = ceil(clk_src_val / (2 * sampl_rate));
      // Configure the DAC division factor for required sampling rate
      RSI_DAC_ClkDivFactor(AUX_ADC_DAC_COMP, clk_div_fac);
    }
    return clk_src_val;
  } else {
    if (sampl_rate < 20000) {
      // Configured ADC clock as 20Khz RC
      RSI_ULPSS_AuxClkConfig(ULPCLK, ENABLE_STATIC_CLK, ULP_AUX_32KHZ_RC_CLK);
      clk_div_fac = ceil((float)32000 / (2 * sampl_rate));
      // Configure the DAC division factor for required sampling rate
      RSI_DAC_ClkDivFactor(AUX_ADC_DAC_COMP, clk_div_fac);
      return (32000 / (2 * clk_div_fac));
    } else {
      if (M4_ULP_SLP_STATUS_REG & ULP_MODE_SWITCHED_NPSS) {
        // Program in PS2 state  Need to integrate
        // Trim Mhz RC clock to 20Mhz
        RSI_IPMU_M20rcOsc_TrimEfuse();
        // Configured DAC clock as 32Mhz RC
        RSI_ULPSS_AuxClkConfig(ULPCLK, ENABLE_STATIC_CLK, ULP_AUX_32MHZ_RC_CLK);
        clk_div_fac = ceil((float)20000000 / (2 * sampl_rate));
        // Configure the DAC division factor for required sampling rate
        RSI_DAC_ClkDivFactor(AUX_ADC_DAC_COMP, clk_div_fac);
        return (20000000 / (2 * clk_div_fac));
      } else {
        RSI_ULPSS_RefClkConfig(ULPSS_RF_REF_CLK);
        // Configured DAC clock as 40Mhz XTAL
        RSI_ULPSS_AuxClkConfig(ULPCLK, ENABLE_STATIC_CLK, ULP_AUX_REF_CLK);
        clk_div_fac = ceil((float)40000000 / (2 * sampl_rate));
        // Configure the DAC division factor for required sampling rate
        RSI_DAC_ClkDivFactor(AUX_ADC_DAC_COMP, clk_div_fac);
        return (40000000 / (2 * clk_div_fac));
      }
    }
  }
}

/*==============================================*/
/**
 * @fn           error_t DAC_PinMux(uint8_t pin_sel)
 * @brief        This API use for configure the DAC output pin in analog mode.
 * @param[in]    pin_sel      : pin_sel = 0 , Configure the ULP_GPIO15 in Analog mode.
 *                              pin_sel = 1 , Configure the ULP_GPIO4 in Analog mode.
 */
error_t DAC_PinMux(uint8_t pin_sel)
{
  if (pin_sel) {
    RSI_EGPIO_UlpPadReceiverDisable(DAC_OUT_AGPIO15); //REN disable
    // Configure ULP_GPIO15 in analog mode
    RSI_EGPIO_SetPinMux(EGPIO1, 0, DAC_OUT_AGPIO15, EGPIO_PIN_MUX_MODE7);
  } else {
    RSI_EGPIO_UlpPadReceiverDisable(DAC_OUT_AGPIO4);
    // Configure ULP_GPIO15 in analog mode
    RSI_EGPIO_SetPinMux(EGPIO1, 0, DAC_OUT_AGPIO4, EGPIO_PIN_MUX_MODE7);
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_DAC_Config(AUX_ADC_DAC_COMP_Type *pstcDAC,uint8_t static_fifo_mode,
 *                                      uint16_t aux_dac_out_mux_en,uint16_t aux_dac_out_mux_sel,  uint8_t prbs_sel))
 * @brief        This API is used to configure DAC in static or FIFO mode.
 * @param[in]    pstcADC              : Pointer to the AUX_ADC_DAC_COMP_Type structure.  
 * @param[in]    static_fifo_mode     : This parameter define DAC in FIFO mode or static mode
 *                                      static_fifo_mode = 0  , for Fifo mode
 *                                      static_fifo_mode = 1  , for Static mode 
 * @param[in]    aux_dac_out_mux_en   : This parameter define DAC output play on AGPIO or not. 
 *                                      aux_dac_out_mux_en = 0  , DAC output not play on any AGPIO.
 *                                      aux_dac_out_mux_en = 1  , DAC output on GPIO. 
 * @param[in]    aux_dac_out_mux_sel  : This parameter define DAC output play on which AGPIO 
 *                                      aux_dac_out_mux_sel = 0 , DAC output play on ULP_GPIO4
 *                                      aux_dac_out_mux_sel = 1 , DAC output play on ULP_GPIO15
 * @param[in]    prbs_sel             : This parameter used for switch on or off PRBS RRBS for DAC.
 *                                      prbs_sel = 0 ,Switch off PRBS RRBS for DAC
 *                                      prbs_sel = 1 ,Switch on PRBS RRBS for DAC
 * @return       execution status 
 */
error_t RSI_DAC_Config(AUX_ADC_DAC_COMP_Type *pstcDAC,
                       uint8_t static_fifo_mode,
                       uint16_t aux_dac_out_mux_en,
                       uint16_t aux_dac_out_mux_sel,
                       uint8_t prbs_sel)
{
  if (prbs_sel) {
    ULP_SPI_MEM_MAP(AUXDACREG0) |= (BIT(5));
  } else {
    ULP_SPI_MEM_MAP(AUXDACREG0) &= ~(BIT(5));
  }
  pstcDAC->AUXDAC_CONIG_1_b.AUXDAC_OUT_MUX_EN  = aux_dac_out_mux_en;
  pstcDAC->AUXDAC_CONIG_1_b.AUXDAC_OUT_MUX_SEL = aux_dac_out_mux_sel;
  pstcDAC->AUXDAC_CTRL_1_b.DAC_STATIC_MODE     = static_fifo_mode;
  if (static_fifo_mode == 0) {
    pstcDAC->AUXDAC_CONIG_1_b.AUXDAC_DYN_EN = 1U;
  }
#ifdef CHIP_917
  if (static_fifo_mode == 1) {
    pstcDAC->AUXDAC_CTRL_1_b.DAC_FIFO_AEMPTY_THRESHOLD = 0;
    pstcDAC->AUXDAC_CONIG_1_b.AUXDAC_DYN_EN            = 1U;
  }
#endif
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_DAC_DynamicModeConfig(AUX_ADC_DAC_COMP_Type *pstcDAC,uint16_t channel_no,
 *                                                uint16_t aux_dac_out_mux_en,uint16_t aux_dac_out_mux_sel)
 * @brief        This API is used to configure DAC in dynamic mode.
 * @param[in]    pstcADC              : Pointer to the AUX_ADC_DAC_COMP_Type structure. 
 * @param[in]    channel_no           : DAC channel to be configured as 0,1,2 ...15 when ADC multichannel enable is present
 * @param[in]    aux_dac_out_mux_en   : This parameter define DAC output play on AGPIO or not. 
 *                                      aux_dac_out_mux_en = 0  , DAC output not play on any AGPIO.
 *                                      aux_dac_out_mux_en = 1  , DAC output on GPIO. 
 * @param[in]    aux_dac_out_mux_sel  : This parameter define DAC output play on which AGPIO 
 *                                      aux_dac_out_mux_sel = 0 , DAC output play on ULP_GPIO4
 *                                      aux_dac_out_mux_sel = 1 , DAC output play on ULP_GPIO15
 * @return       execution status 
 */
error_t RSI_DAC_DynamicModeConfig(AUX_ADC_DAC_COMP_Type *pstcDAC,
                                  uint16_t channel_no,
                                  uint16_t aux_dac_out_mux_en,
                                  uint16_t aux_dac_out_mux_sel)
{
  pstcDAC->INTERNAL_DMA_CH_ENABLE_b.PER_CHANNEL_ENABLE |= 1 << channel_no;
  pstcDAC->AUXDAC_CONIG_1_b.AUXDAC_DYN_EN = 1U;
#ifdef CHIP_9118
  pstcDAC->AUXDAC_CTRL_1_b.DAC_TO_CTRL_ADC = 1U;
#endif
  pstcDAC->ADC_CH_BIT_MAP_CONFIG[channel_no].ADC_CH_BIT_MAP_CONFIG_0_b.CHANNEL_BITMAP |=
    (aux_dac_out_mux_en << 29 | aux_dac_out_mux_sel << 30);
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_DAC_DynamicModeWriteData(AUX_ADC_DAC_COMP_Type *pstcDAC,uint16_t channel_no,
 *                                                    uint16_t *data,uint32_t len)
 * @brief        This API is used to write input data DAC in dynamic mode  
 * @param[in]    pstcADC     : Pointer to the AUX_ADC_DAC_COMP_Type structure. 
 * @param[in]    channel_no  : DAC channel to be configured as 0,1,2 ...15 when ADC multichannel enable is present
 * @param[in]    data        : Input buffer pointer
 * @param[in]    len         : length of digital samples which play in DAC .
 * @return       execution status 
 */
error_t RSI_DAC_DynamicModeWriteData(AUX_ADC_DAC_COMP_Type *pstcDAC, uint16_t channel_no, uint16_t *data, uint32_t len)
{
  uint32_t i;
  for (i = 0; i < len; i++) {
    // clear the DAC input register in dynamic mode
    pstcDAC->ADC_CH_BIT_MAP_CONFIG[channel_no].ADC_CH_BIT_MAP_CONFIG_0_b.CHANNEL_BITMAP &= (0x7FFFFFFF);
    // Feed the input sample in dynamic mode
    pstcDAC->ADC_CH_BIT_MAP_CONFIG[channel_no].ADC_CH_BIT_MAP_CONFIG_0_b.CHANNEL_BITMAP |= (data[i]) << 31;
    // clear the DAC input register in dynamic mode
    pstcDAC->ADC_CH_BIT_MAP_CONFIG[channel_no].ADC_CH_BIT_MAP_CONFIG_1_b.CHANNEL_BITMAP &= (0xFFFFFE00);
    // Feed the input sample in dynamic mode
    pstcDAC->ADC_CH_BIT_MAP_CONFIG[channel_no].ADC_CH_BIT_MAP_CONFIG_1_b.CHANNEL_BITMAP |= (data[i]) >> 1;
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_DAC_WriteData(AUX_ADC_DAC_COMP_Type *pstcDAC,int16_t *data,
 *                               uint8_t static_fifo_mode,uint16_t len)
 * @brief        This API is used to write input data to DAC in static or in FIFO mode. 
 * @param[in]    pstcADC             : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    data                : This parameter define input data to DAC.
 * @param[in]    static_fifo_mode    : This parameter define write data to DAC in FIFO mode 
 *                                      or in static mode
 *                                      static_fifo_mode = 1  , Write data to DAC in static mode
 *                                      static_fifo_mode = 0  , Write data to DAC in fifo mode  
 * @return       execution status 
 */
error_t RSI_DAC_WriteData(AUX_ADC_DAC_COMP_Type *pstcDAC, int16_t *data, uint8_t static_fifo_mode, uint16_t len)
{
  uint32_t i;
  for (i = 0; i < len; i++) {
#ifdef CHIP_9118
    if (static_fifo_mode == 0) {
      pstcDAC->AUXDAC_DATA_REG_b.AUXDAC_DATA = (*data);
      data++;
    } else {
      pstcDAC->AUXDAC_CONIG_1_b.AUXDAC_DATA_S = (*data);
      data++;
    }
#endif
#ifdef CHIP_917
    pstcDAC->AUXDAC_DATA_REG_b.AUXDAC_DATA = (*data);
    data++;
#endif
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           uint16_t RSI_DAC_ReadData(AUX_ADC_DAC_COMP_Type *pstcDAC,uint16_t data)
 * @brief        This API is used to Read output data of DAC in FIFO or static mode. 
 * @param[in]    pstcADC             : Pointer to the AUX_ADC_DAC_COMP_Type structure. 
 * @param[out]   data                : Output data 
 * @return       Output data of DAC. 
 */
uint16_t RSI_DAC_ReadData(AUX_ADC_DAC_COMP_Type *pstcDAC)
{
  uint16_t data;
  data = pstcDAC->AUXDAC_CONIG_1_b.AUXDAC_DATA_S;
  return data;
}

/*==============================================*/
/**
 * @fn           uint16_t RSI_DAC_DynamicModeReadData(AUX_ADC_DAC_COMP_Type *pstcDAC, uint32_t channel,uint16_t data)
 * @brief        This API is used to Read output data 
 * @param[in]    pstcADC  : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    channel  : DAC channel to be configured as 0,1,2 ...15 when ADC multichannel enable is present 
 * @param[out]   data     : Output data in dynamic mode  
 * @return       DAC output data , where DAC in dynamic mode. 
 */
uint16_t RSI_DAC_DynamicModeReadData(AUX_ADC_DAC_COMP_Type *pstcDAC, uint32_t channel, uint16_t data)
{
  data = pstcDAC->ADC_CH_BIT_MAP_CONFIG[channel].ADC_CH_BIT_MAP_CONFIG_1_b.CHANNEL_BITMAP;
  data = data << 1;
  data = (data | (pstcDAC->ADC_CH_BIT_MAP_CONFIG[channel].ADC_CH_BIT_MAP_CONFIG_0_b.CHANNEL_BITMAP >> 31));
  return data;
}

/*==============================================*/
/**
 * @fn           uint32_t RSI_DAC_Start( AUX_ADC_DAC_COMP_Type *pstcDAC ,uint32_t channel, dac_config *config )
 * @brief        This API is used to start the DAC operation in Static or FIFO mode.
 * @param[in]    pstcADC     : Pointer to the AUX_ADC_DAC_COMP_Type structure. 
 * @param[in]    aux_dac_en  : This parameter define DAC module start or not start.
 *                             aux_dac_en  : 1  -> Enable DAC in static or in dynamic mode operation.
 *                             aux_dac_en  : 0  -> Disable DAC in static or in dynamic mode operation. 
 * @return       execution status 
 */
error_t RSI_DAC_Start(AUX_ADC_DAC_COMP_Type *pstcDAC, uint16_t aux_dac_en)
{
  pstcDAC->AUXDAC_CONIG_1_b.AUXDAC_EN_S      = 1U;
  pstcDAC->AUXDAC_CTRL_1_b.ENDAC_FIFO_CONFIG = 1U;
  pstcDAC->AUXDAC_CTRL_1_b.DAC_ENABLE_F      = aux_dac_en;
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           uint32_t RSI_DAC_Stop( AUX_ADC_DAC_COMP_Type *pstcDAC )
 * @brief        This API is used to stop the DAC operation in FIFO or Static mode.
 * @param[in]    pstcADC             : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @return       execution status 
 */
error_t RSI_DAC_Stop(AUX_ADC_DAC_COMP_Type *pstcDAC)
{
  pstcDAC->AUXDAC_CONIG_1_b.AUXDAC_DYN_EN    = 0U;
  pstcDAC->AUXDAC_CONIG_1_b.AUXDAC_EN_S      = 0U;
  pstcDAC->AUXDAC_CTRL_1_b.DAC_ENABLE_F      = 0U;
  pstcDAC->AUXDAC_CTRL_1_b.ENDAC_FIFO_CONFIG = 0U;
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_DAC_DynamicModeStart(AUX_ADC_DAC_COMP_Type *pstcDAC, uint32_t channel, uint16_t aux_dac_en)
 * @brief        This API is used to start the DAC in dynamic mode.
 * @param[in]    pstcADC      : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    channel      : DAC channel to be configured as 0,1,2 ...15 when ADC multichannel enable is present
 * @param[in]    aux_dac_en   : This parameter define DAC enable or disable.
 *                              aux_dac_en = 0 , Disable DAC for dynamic mode operation. 
 *                              aux_dac_en = 1 , Enable  DAC for dynamic mode operation. 
 * @return       execution status 
 */
error_t RSI_DAC_DynamicModeStart(AUX_ADC_DAC_COMP_Type *pstcDAC, uint32_t channel, uint16_t aux_dac_en)
{
  pstcDAC->AUXDAC_CONIG_1_b.AUXDAC_EN_S = 1U;
  pstcDAC->ADC_CH_BIT_MAP_CONFIG[channel].ADC_CH_BIT_MAP_CONFIG_0_b.CHANNEL_BITMAP |= aux_dac_en << 28;
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_DAC_DynamicModeStop( AUX_ADC_DAC_COMP_Type *pstcDAC,uint32_t channel)
 * @brief        This API is used to stop the DAC in dynamic mode.
 * @param[in]    pstcADC  : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    channel  : DAC channel to be configured as 0,1,2 ...15 when ADC multichannel enable is present
 * @return       execution status 
 */
error_t RSI_DAC_DynamicModeStop(AUX_ADC_DAC_COMP_Type *pstcDAC, uint32_t channel)
{
  pstcDAC->AUXDAC_CONIG_1_b.AUXDAC_DYN_EN                                          = 0;
  pstcDAC->AUXDAC_CONIG_1_b.AUXDAC_EN_S                                            = 0;
  pstcDAC->ADC_CH_BIT_MAP_CONFIG[channel].ADC_CH_BIT_MAP_CONFIG_0_b.CHANNEL_BITMAP = 0 << 28;
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           uint32_t RSI_DAC_ClkDivFactor(AUX_ADC_DAC_COMP_Type *pstcDAC, uint16_t div_factor)
 * @brief        This API is used to set clock with configurable on time 
 * @param[in]    pstcADC      : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    div_factor   : clock division factor to be programmed
 * @return       execution status 
 *
 */
error_t RSI_DAC_ClkDivFactor(AUX_ADC_DAC_COMP_Type *pstcDAC, uint16_t div_factor)
{
  if (div_factor > MAX_DIV_FAC_DAC) {
    return INVALID_PARAMETERS;
  }
  pstcDAC->AUXDAC_CLK_DIV_FAC_b.DAC_CLK_DIV_FAC = div_factor;
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           uint32_t RSI_DAC_PowerControl(POWER_STATE state)
 * @brief        This API is used to Power On and off for DAC
 * @param[in]    state : DAC_POWER_ON - To powerup dac powergates
 *                       DAC_POWER_OFF - To powerdown dac powergates
 * @return       execution status 
 *
 */
void RSI_DAC_PowerControl(POWER_STATE_DAC state)
{
  switch (state) {
    case DAC_POWER_ON:
      RSI_IPMU_PowerGateSet(AUXDAC_PG_ENB);
      RSI_PS_UlpssPeriPowerUp(ULPSS_PWRGATE_ULP_AUX);
      break;
    case DAC_POWER_OFF:
      RSI_IPMU_PowerGateClr(AUXDAC_PG_ENB);
      break;
  }
}

/*==============================================*/
/**
 * @fn           error_t RSI_DAC_SetFifoThreshold(AUX_ADC_DAC_COMP_Type *pstcDAC,uint32_t fifo_threshold)
 * @brief        This API is used to set fifo threshold for DAC.
 * @param[in]    pstcADC        : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    fifo_threshold : FIFO threshold value.
 * @return       execution status 
 *
 */
error_t RSI_DAC_SetFifoThreshold(AUX_ADC_DAC_COMP_Type *pstcDAC, uint32_t fifo_threshold)
{
  if (fifo_threshold > MAX_DAC_FIFO_THR) {
    return INVALID_PARAMETERS;
  }
  pstcDAC->AUXDAC_CTRL_1_b.DAC_FIFO_THRESHOLD = fifo_threshold;
  pstcDAC->AUXDAC_CTRL_1_b.DAC_FIFO_FLUSH     = 1U;
  return RSI_OK;
}

#ifdef CHIP_9118

/*==============================================*/
/**
 * @fn           error_t RSI_DAC_InterruptUnMask(AUX_ADC_DAC_COMP_Type *pstcDAC)
 * @brief        This API is used to unmask the DAC interrupt in FIFO mode.
 * @param[in]    pstcADC  : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @return       zero on success. 
 *
 */
error_t RSI_DAC_InterruptUnMask(AUX_ADC_DAC_COMP_Type *pstcDAC)
#endif
#ifdef CHIP_917

  /*==============================================*/
  /**
 * @fn           error_t RSI_DAC_InterruptUnMask(AUX_ADC_DAC_COMP_Type *pstcDAC, uint8_t oper_mode)
 * @brief        This API is used to unmask the DAC interrupt in FIFO mode.
 * @param[in]    pstcADC   : Pointer to the AUX_ADC_DAC_COMP_Type structure.
 * @param[in]    oper_mode : Operation mode select. 0 for Static mode and 1 for FIFO mode. 
 * @return       zero on success. 
 *
 */
  error_t RSI_DAC_InterruptUnMask(AUX_ADC_DAC_COMP_Type *pstcDAC, uint8_t oper_mode)
#endif
{
#ifdef CHIP_9118
  pstcDAC->INTR_MASK_REG_b.DAC_FIFO_EMPTY_INTR_MASK  = 0;
  pstcDAC->INTR_MASK_REG_b.DAC_FIFO_AEMPTY_INTR_MASK = 0;
#endif
#ifdef CHIP_917
  if (oper_mode == STATIC_MODE_EN) {
    pstcDAC->INTR_MASK_REG_b.DAC_STATIC_MODE_DATA_INTR_MASK = 0;
  } else {
    pstcDAC->INTR_MASK_REG_b.DAC_FIFO_EMPTY_INTR_MASK  = 0;
    pstcDAC->INTR_MASK_REG_b.DAC_FIFO_AEMPTY_INTR_MASK = 0;
  }
#endif
  return RSI_OK;
}

#ifdef CHIP_9118

/*==============================================*/
/**
 * @fn           error_t RSI_DAC_InterruptMask(AUX_ADC_DAC_COMP_Type *pstcDAC)
 * @brief        This API is used to mask the DAC interrupt in FIFO mode.
 * @param[in]    pstcADC      : Pointer to the AUX_ADC_DAC_COMP_Type structure. 
 * @return       zero on success. 
 *
 */
error_t RSI_DAC_InterruptMask(AUX_ADC_DAC_COMP_Type *pstcDAC)
#endif
#ifdef CHIP_917

  /*==============================================*/
  /**
 * @fn           error_t RSI_DAC_InterruptMask(AUX_ADC_DAC_COMP_Type *pstcDAC, uint8_t oper_mode)
 * @brief        This API is used to mask the DAC interrupt in FIFO mode.
 * @param[in]    pstcADC      : Pointer to the AUX_ADC_DAC_COMP_Type structure. 
 * @param[in]    oper_mode    : Operation mode select. 0 for Static mode and 1 for FIFO mode. 
 * @return       zero on success. 
 *
 */
  error_t RSI_DAC_InterruptMask(AUX_ADC_DAC_COMP_Type *pstcDAC, uint8_t oper_mode)
#endif
{
#ifdef CHIP_9118
  pstcDAC->INTR_MASK_REG_b.DAC_FIFO_EMPTY_INTR_MASK  = 1;
  pstcDAC->INTR_MASK_REG_b.DAC_FIFO_AEMPTY_INTR_MASK = 1;
#endif
#ifdef CHIP_917
  if (oper_mode == STATIC_MODE_EN) {
    pstcDAC->INTR_MASK_REG_b.DAC_STATIC_MODE_DATA_INTR_MASK = 1;
  } else {
    pstcDAC->INTR_MASK_REG_b.DAC_FIFO_EMPTY_INTR_MASK  = 1;
    pstcDAC->INTR_MASK_REG_b.DAC_FIFO_AEMPTY_INTR_MASK = 1;
  }
#endif
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_DAC_InterruptClr(AUX_ADC_DAC_COMP_Type *pstcDAC)
 * @brief        This API is used to clear the DAC interrupt in FIFO mode.
 * @param[in]    pstcADC      : Pointer to the AUX_ADC_DAC_COMP_Type structure. 
 * @return       zero on success. 
 *
 */
error_t RSI_DAC_InterruptClr(AUX_ADC_DAC_COMP_Type *pstcDAC)
{
#ifdef CHIP_9118
  pstcDAC->AUXDAC_CTRL_1_b.DAC_FIFO_FLUSH = 1U;
#endif
#ifdef CHIP_917
  int32_t dac_data_read = 0;
  dac_data_read         = pstcDAC->AUXDAC_DATA_REG_b.AUXDAC_DATA;
#endif
  return RSI_OK;
}

#ifdef DAC_FIFO_MODE_EN

/*==============================================*/
/**
 * @fn void dac_udmaTransferComplete(RSI_UDMA_HANDLE_T udmaHandle, RSI_UDMA_DESC_T *pTranDesc, uint32_t channel_no)
 * @brief  This function is callback function to reconfigure uDMA ping and pong descriptor.
 * @param[in]  - udmaHandle  :  uDMA handler
 * @param[in]  - pTranDesc   :  Event type 
 * @return     None
 */
void dac_udmaTransferComplete(RSI_UDMA_HANDLE_T udmaHandle, RSI_UDMA_DESC_T *pTranDesc, uint32_t channel_no)
{
}

/*==============================================*/
/**
 * @fn void UDMA_DAC_Ping_Write(uint16_t num_of_samples, int16_t *input_buff, uint8_t pingreconfig)
 * @brief  This function use for configure descriptor for  primary structure.
 * @param[in]  ping_reconfig  : This parameter use for skip uDMA initialization function in 
 *                          ping descriptor reconfiguration time.
 * @param[in]  num_of_samples : Number of samples to play in DAC.
 * @param[in]  input_buff     : Input samples buffer pointer.
 * @return     None
 */
void UDMA_DAC_Ping_Write(uint16_t num_of_samples, int16_t *input_buff, uint8_t pingreconfig)
{
  RSI_UDMA_CHA_CONFIG_DATA_T control = { 0 };
  RSI_UDMA_CHA_CFG_T config          = { 0 };

  config.altStruct       = 0;
  config.burstReq        = 1;
  config.channelPrioHigh = 0;
  config.periAck         = 0;
  config.periphReq       = 0;
  config.reqMask         = 0;
  config.dmaCh           = 10;

  // Setup source to destination copy for trigger for memory
  // Descriptor configurations
  control.transferType       = UDMA_MODE_PINGPONG;
  control.nextBurst          = 0;
  control.totalNumOfDMATrans = num_of_samples - 1;
  control.rPower             = ARBSIZE_4;
  control.srcProtCtrl        = 0x000;
  control.dstProtCtrl        = 0x000;
  control.srcSize            = SRC_SIZE_16;
  control.srcInc             = SRC_INC_16;
  control.dstSize            = DST_SIZE_16;
  control.dstInc             = DST_INC_NONE;

  if (pingreconfig) {
    RSI_UDMA_SetupChannel(udmaHandle1, &config);
  }

  RSI_UDMA_SetupChannelTransfer(udmaHandle1,
                                &config,
                                control,
                                (uint32_t *)input_buff,
                                (uint32_t *)(DAC_INPUT_REG_ADDR));
}

/*==============================================*/
/**
 * @fn    void UDMA_DAC_Pong_Write(uint16_t num_of_samples, int16_t *input_buff, uint8_t pongreconfig)
 * @brief  This function use for configure descriptor for alternate uDMA structure.
 * @param[in]  pong_reconfig  : This parameter use for skip uDMA initialization function in 
 *                          ping descriptor reconfiguration time.
 * @param[in]  num_of_samples : Number of samples to play in DAC.
 * @param[in]  input_buff     : Input samples buffer pointer.
 * @return None
 */
void UDMA_DAC_Pong_Write(uint16_t num_of_samples, int16_t *input_buff, uint8_t pongreconfig)
{
  RSI_UDMA_CHA_CONFIG_DATA_T control = { 0 };
  RSI_UDMA_CHA_CFG_T config          = { 0 };

  config.altStruct       = 1;
  config.burstReq        = 1;
  config.channelPrioHigh = 0;
  config.periAck         = 0;
  config.periphReq       = 0;
  config.reqMask         = 0;
  config.dmaCh           = 10;

  // Setup source to destination copy for trigger for memory
  // Descriptor configurations
  control.transferType       = UDMA_MODE_PINGPONG;
  control.nextBurst          = 0;
  control.totalNumOfDMATrans = num_of_samples - 1;
  control.rPower             = ARBSIZE_4;
  control.srcProtCtrl        = 0x000;
  control.dstProtCtrl        = 0x000;
  control.srcSize            = SRC_SIZE_16;
  control.srcInc             = SRC_INC_16;
  control.dstSize            = DST_SIZE_16;
  control.dstInc             = DST_INC_NONE;

  if (pongreconfig) {
    RSI_UDMA_SetupChannel(udmaHandle1, &config);
  }
  RSI_UDMA_SetupChannelTransfer(udmaHandle1,
                                &config,
                                control,
                                (uint32_t *)input_buff,
                                (uint32_t *)(DAC_INPUT_REG_ADDR));
}

/*==============================================*/
/**
 * @fn    void dac_udma_init(void)
 * @brief  This API use for Init ULP-uDMA.
 * @return None
 */
void dac_udma_init(void)
{
  RSI_UDMA_INIT_T udmaInit;
  volatile bool done1 = NULL;

  RSI_ULPSS_PeripheralEnable(ULPCLK, ULP_UDMA_CLK, ENABLE_STATIC_CLK);

  RSI_PS_UlpssPeriPowerUp(ULPSS_PWRGATE_ULP_UDMA);

  udmaInit.pUserData = (void *)&done1;
  udmaInit.base      = (uint32_t)UDMA1;
  udmaInit.sramBase  = (uint32_t)&UDMA1_Table;
  udmaHandle1        = RSI_UDMA_Init(devMem, &udmaInit);

  RSI_UDMA_RegisterCallback(udmaHandle1, (void *)dac_udmaTransferComplete);
}

/*==============================================*/
/**
 * @fn void dac_udma_start(void)
 * @brief  This API use for trigger uDMA channel10 for DAC operation.
 * @return None
 */
void dac_udma_start(void)
{
  RSI_UDMA_ChannelEnable(udmaHandle1, 10);

  RSI_UDMA_UDMAEnable(udmaHandle1);

  // uDMA NVIC enable
  NVIC_EnableIRQ(UDMA1_IRQn);
}

/*==============================================*/
/**
 * @fn  void dac_udma_write(uint8_t ping_pong_write, uint16_t num_of_samples, int16_t *input_buff, uint8_t skip_flag)
 * @brief  Configure ping or pong descriptor address.
 * @param[in]  ping_pong_write  - Initialize Ping descriptor
 * @param[in]  num_of_samples   - Number of samples to play in DAC
 * @param[in]  input_buff       - Input samples buffer pointer
 * @param[in]  skip_flag        - Skips the flag
 * @return     None
 */
void dac_udma_write(uint8_t ping_pong_write, uint16_t num_of_samples, int16_t *input_buff, uint8_t skip_flag)
{
  if (ping_pong_write) {
    // Initialize Ping descriptor
    UDMA_DAC_Ping_Write(num_of_samples, input_buff, skip_flag);
  } else {
    // Initialize Pong descriptor
    UDMA_DAC_Pong_Write(num_of_samples, input_buff, skip_flag);
  }
}
#endif
