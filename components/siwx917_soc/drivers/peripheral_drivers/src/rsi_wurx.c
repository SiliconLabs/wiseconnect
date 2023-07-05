/*******************************************************************************
* @file  rsi_wurx.c
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
/*==============================================*/
/**
 * @fn           void RSI_WURX_Init(uint16_t bypass_l1_enable, uint16_t l1_freq_div, uint16_t l2_freq_div)
 * @brief        This API is used to initialization of l1 and l2 frequency as well as enable wurx.
 * @param[in]    bypass_l1_enable   : Enable or disable the bypass functionality of level1 
 * @param[in]    l1_freq_div        : Set the level1 frequency by using division factor ,This parameter define frequency.
 *                                    Ranges : 32 khz incoming clock  then following option 
 *                                    0 : 0.125 khz, 1: 0.250 Khz, 2: 0.5 khz ,3: 1 khz, default : 2 khz  
 *                                    Ranges : 64 khz incoming clock  then following option 
 *                                    0 : 0.25 khz, 1: 0.5 Khz, 2: 1 khz ,3: 2 khz 
 * @param[in]    l2_freq_div        : Set the level2 frequency by using division factor ,This parameter define frequency.
 *                                    Ranges : 32 khz incoming clock  then following option 
 *                                    0 : 4 khz, 1: 8 Khz, 2: 16 khz ,3: 32 khz 
 *                                    Ranges : 64 khz incoming clock  then following option 
 *                                    0 : 8 khz, 1: 16 Khz, 2: 32 khz ,3: 64 khz
 * @return       none
 */
void RSI_WURX_Init(uint16_t bypass_l1_enable, uint16_t l1_freq_div, uint16_t l2_freq_div)
{
  // Wait for Out of reset
  while (!(WURX_MANUAL_CALIB_MODE_REG2 & 0x000001))
    ;
  while (!(WURX_CORR_DET_READ_REG & 0x000002))
    ;

  // Bypass L1 pattern
  WURX_BYPASS_LEVEL1_AND_FREQ |= (bypass_l1_enable << POS21);

  if (bypass_l1_enable) {
    // Do nothing here
  } else {
    // Clear the frequency for L1 level pattern
    WURX_BYPASS_LEVEL1_AND_FREQ &= ~(0x7 << POS18);
    // Freq L1 pattern (3 : 32Khz, 2 : 16Khz, 1 : 8Khz, 0 : 4Khz) for 32KHz Clock
    WURX_BYPASS_LEVEL1_AND_FREQ |= (l1_freq_div << POS18);
  }
  // Configure L2 pattern
  // Clear the frequency for L2 pattern
  WURX_BYPASS_LEVEL1_AND_FREQ &= ~(0x3 << POS16);
  // Freq L2 pattern (3 : 32Khz, 2 : 16Khz, 1 : 8Khz, 0 : 4Khz) for 32KHz Clock
  WURX_BYPASS_LEVEL1_AND_FREQ |= (l2_freq_div << POS16);

  WURX_ENABLE_AND_AAC_DET_REG |= (BIT(1) | BIT(0));
}

/*==============================================*/
/**
 * @fn           void RSI_IPMU_40MhzClkCalib(uint16_t clk_enable, uint32_t channel_selection_value)
 * @brief        This API is used to calculate the 40MHZ VCO calibration.
 * @param[in]    clk_enable              : Clock enable
 * @param[in]    channel_selection_value : This parameter define on which channel frequency use for transmission.
 *                                         E.g. If channel 11 use for transmission the channel_11 = 2475.
 *                                              channel_selection_value = (((channel_11)/(cal_clock*2)) * 2^6)    
 *                                              So [channel_selection_value =(((2475)/(40*2)) * 2^6) 
 *                                              Note : Refer  RSI_WURX_CalVCOCalFreq() API ,its return calculated value.   
 * @return       none
 */
void RSI_IPMU_40MhzClkCalib(uint16_t clk_enable, uint32_t channel_selection_value)
{
  volatile uint32_t cntr = 0, lco_coarse = 0, lco_fine = 0, chnl_freq = 0, coarse_word = 0, fine_word = 0;

  if (clk_enable) {
    chnl_freq = channel_selection_value;
    // vco manual
    WURX_MANUAL_CALIB_MODE_REG1 &= ~(BIT(21));
    // NPSS REF Clock Cleaner OFF = 1
    ULPCLKS_REFCLK_REG |= (BIT(17));
    // NPSS REF Clock Cleaner ON = 0  by default is one
    ULPCLKS_REFCLK_REG &= ~(BIT(18));
    // NPSS REF CLOCK Mux to SPI
    ULPCLKS_REFCLK_REG &= ~(BIT(16));
    // Ulp_clk 32Mhz clock for Calibration
    ULPCLKS_REFCLK_REG |= (BIT(21));
    // wait
    for (cntr = 0; cntr < (10 * 4); cntr++) {
      __ASM("nop");
    }
    // NPSS REF Clock Cleaner ON = 1
    ULPCLKS_REFCLK_REG |= (BIT(18));
    // NPSS REF Clock Cleaner OFF = 0
    ULPCLKS_REFCLK_REG &= ~(BIT(17));

    // Configure the frequency of reference clock as 0x400
    WURX_AAC_MODE_REG |= ((0x1 << POS20) | (REF_CLOCK_FREQ << POS7));

    // Configure the receive channel frequency
    WURX_LCO_FREQ_CALIB_REG |= ((BIT(13) | channel_selection_value));

    // Checking the calibration done
    while (!(WURX_TEST_MODE_REG & 0x000002))
      ;
    while ((WURX_TEST_MODE_REG & 0x000002))
      ;

    // Read the LCO coarse and fine word
    lco_coarse  = ((WURX_MANUAL_CALIB_MODE_REG2) & (0x3E00));
    lco_fine    = ((WURX_MANUAL_CALIB_MODE_REG2) & (0x3FC000));
    coarse_word = lco_coarse >> 9;
    fine_word   = lco_fine >> 14;

    // Calculating proper LCO fine word
    while (fine_word == 255) {
      chnl_freq = (chnl_freq - 4);

      WURX_MANUAL_CALIB_MODE_REG1 &= ~(BIT(21));
      // NPSS REF Clock Cleaner OFF = 1
      ULPCLKS_REFCLK_REG |= (BIT(17));
      // NPSS REF Clock Cleaner ON = 0  by default is one
      ULPCLKS_REFCLK_REG &= ~(BIT(18));
      // NPSS REF CLOCK Mux to SPI
      ULPCLKS_REFCLK_REG &= ~(BIT(16));
      // Ulp_clk 32Mhz clock for Calibration
      ULPCLKS_REFCLK_REG |= (BIT(21));
      // wait
      for (cntr = 0; cntr < (10 * 4); cntr++) {
        __ASM("nop");
      }
      // NPSS REF Clock Cleaner ON = 1
      ULPCLKS_REFCLK_REG |= (BIT(18));
      // NPSS REF Clock Cleaner OFF = 0
      ULPCLKS_REFCLK_REG &= ~(BIT(17));

      // Configure the frequency of reference clock as 0x400
      WURX_AAC_MODE_REG |= ((0x1 << POS20) | (REF_CLOCK_FREQ << POS7));
      WURX_LCO_FREQ_CALIB_REG &= ~(0x000fff);

      // Configure new receive channel frequency
      WURX_LCO_FREQ_CALIB_REG |= ((BIT(13) | (chnl_freq)));

      // Checking the calibration done
      while (!(WURX_TEST_MODE_REG & 0x000002))
        ;
      while ((WURX_TEST_MODE_REG & 0x000002))
        ;

      fine_word = ((WURX_MANUAL_CALIB_MODE_REG2) & (0x3FC000)) >> 14;

      if (fine_word < 255) {
        coarse_word = (coarse_word + 1);
        fine_word   = (fine_word - 7 * (channel_selection_value - chnl_freq));
      }
    }
    // Modified the LCO fine and coarse word
    lco_fine   = fine_word - 6;
    lco_coarse = coarse_word;

    // Write the LCO fine and coarse word manual calibration register
    ULP_SPI_MEM_MAP(0x87)       = 0x3F8000;
    WURX_MANUAL_CALIB_MODE_REG1 = BIT(21) | lco_fine << POS13 | lco_coarse << POS8;
    ULP_SPI_MEM_MAP(0x87)       = 0x000000;

    for (cntr = 0; cntr < 4000 * 4; cntr++) {
      __ASM("nop");
    }

    // Disable the calibration clock
    WURX_AAC_MODE_REG &= ~(BIT(19) | BIT(20));
    ULPCLKS_REFCLK_REG |= (BIT(17));
    ULPCLKS_REFCLK_REG &= ~(BIT(18));
  } else {
    WURX_AAC_MODE_REG &= ~(BIT(19) | BIT(20));
    /* NPSS Ref Clock CLeaner OFF */
    ULPCLKS_REFCLK_REG |= (BIT(17));
    ULPCLKS_REFCLK_REG &= ~(BIT(18));
  }
  return;
}

/*==============================================*/
/**
 * @fn           void RSI_IPMU_DCCalib()
 * @brief        This API is used to calculate the manual DC calibration as well as enable periodic detection enable 
 * @return       none
 */
void RSI_IPMU_DCCalib()
{
  uint16_t j, cal_val_ref = 0, det_ref = 0, cal_val = 0, i;
  uint32_t cntr = 0;

  // Set manual dc  calibration mode
  WURX_COMP_OFFSET_CALIB_REG |= ((VAL1 << POS10) | (DC_OFFSET_VALUE << POS3) | (VAL2 << POS1));

  // Enable the continuous calibration mode
  WURX_ENABLE_AND_AAC_DET_REG |= BIT(2);

  // Calculate the appropriate detection reference shift value
  for (i = 0; i <= 15; i++) {
    WURX_LNA_IF_REG = (DETECTION_REF_SHIFT - i);
    for (cntr = 0; cntr < 120000; cntr++) {
      __ASM("nop");
    }
    cal_val_ref = ((WURX_MANUAL_CALIB_MODE_REG1 & 0xFE) >> 1);

    if (cal_val_ref <= 100 && cal_val_ref >= 50) {
      det_ref = 0;
      for (j = 0; j <= 5; j++) {
        for (cntr = 0; cntr < 120000; cntr++) {
          __ASM("nop");
        }
        cal_val = ((WURX_MANUAL_CALIB_MODE_REG1 & 0xFE) >> 1);
        if (cal_val >= cal_val_ref - 3 && cal_val <= cal_val_ref + 3) {
          det_ref++;
        }
      }
      if (j == det_ref) {
        break;
      }
    }
  }
}

/*==============================================*/
/**
 * @fn           void RSI_WURX_CorrEnable(uint16_t wurx_enable)
 * @brief        This API is used enable wurx correlation.
 * @param[in]    wurx_enable : Enable wurx correlation
 * @return       none
 */
void RSI_WURX_CorrEnable(uint16_t wurx_enable)
{
  if (wurx_enable) {
    // Correlation Enable
    MCU_FSM->MCU_FSM_CRTL_PDM_AND_ENABLES |= (BIT(1));
  } else {
    // Disable wurx and clk_en
    MCU_FSM->MCU_FSM_CRTL_PDM_AND_ENABLES &= ~(BIT(1));
  }
}

/*==============================================*/
/**
 * @fn           void RSI_WURX_SetWakeUpThreshold(uint16_t threshold_1, uint16_t threshold_2)
 * @brief        This API is used set up threshold value for operation.
 * @param[in]    threshold_1   : Threshold value for pattern1.
 * @param[in]    threshold_2   : Threshold value for pattern2. 
 * @return       none
 */
void RSI_WURX_SetWakeUpThreshold(uint16_t threshold_1, uint16_t threshold_2)
{
  // clear bit9 to 14
  WURX_CORR_CALIB_REG &= ~(THRESH_CLR_MASK_VAL << POS9);
  // Set threshold value for pattern1
  WURX_CORR_CALIB_REG |= (threshold_1 << POS9);

  // clear bit3 to 8
  WURX_CORR_CALIB_REG &= ~(THRESH_CLR_MASK_VAL << POS3);
  // Set threshold value for pattern2
  WURX_CORR_CALIB_REG |= (threshold_2 << POS3);
}

/*==============================================*/
/**
 * @fn           void RSI_WURX_Pattern2DetectionEnable(uint16_t enable)
 * @brief        This API is used enable pattern2.
 * @param[in]    enable  : Enable the pattern2 detection bit  .  
 * @return       none
 */
void RSI_WURX_Pattern2DetectionEnable(uint16_t enable)
{
  if (enable) {
    // Pattern2 detection enable
    WURX_CORR_CALIB_REG |= BIT(18);
  } else {
    // Pattern2 detection disable
    WURX_CORR_CALIB_REG &= ~BIT(18);
  }
}

/*==============================================*/
/**
 * @fn           void RSI_WURX_TailDataDecodeEnable(uint16_t enable, uint16_t data_len)
 * @brief        This API is used to enable the Tail data decode.
 * @param[in]    enable    : Enable the tail data decode bit. 
 * @param[in]    data_len  : Set the detection bit length.
 *                          - 0 for 64 bit
 *                          - 1 for 128 bit
 *                          - 2 for 192 bit
 *                          - 3 for 256 bit
 * @return       none
 */
void RSI_WURX_TailDataDecodeEnable(uint16_t enable, uint16_t data_len)
{
  if (enable) {
    WURX_CORR_CALIB_REG &= ~VAL3;
    // WURX Decode Tail bits
    WURX_CORR_CALIB_REG |= BIT(2) | data_len;
  } else {
    WURX_CORR_CALIB_REG &= ~BIT(2);
  }
}

/*==============================================*/
/**
 * @fn           error_t RSI_WURX_GetTailData(uint32_t *tail_data, uint16_t tail_data_len)
 * @brief        This API is used get the tail data
 * @param[in]    tail_data      : Pointer to store the tail data.
 * @param[in]    tail_data_len  : This parameter define number of bit read in tail data.
 *                              - 0 for 64 bit
 *                              - 1 for 128 bit  
 *                              - 2 for 192 bit 
 *                              - 3 for 256 bit     
 * @return       Receive pattern tail data - If Success             
 */
error_t RSI_WURX_GetTailData(uint32_t *tail_data, uint16_t tail_data_len)
{
  if (tail_data_len == TAIL_DATA_DECODE_64BIT) {
    // get 64 bit tail data
    while (!(WURX_CORR_DET_READ_REG & BIT(16)))
      ;
    RSI_WURX_ReadPattern1Odd(tail_data);
  } else if (tail_data_len == TAIL_DATA_DECODE_128BIT) {
    // get 128 bit tail data
    while (!(WURX_CORR_DET_READ_REG & BIT(17)))
      ;
    RSI_WURX_ReadPattern1Odd(tail_data);
    while (!(WURX_CORR_DET_READ_REG & BIT(16)))
      ;
    RSI_WURX_ReadPattern1Even(tail_data);
  } else if (tail_data_len == TAIL_DATA_DECODE_192BIT) {
    // get 192 bit tail data
    while (!(WURX_CORR_DET_READ_REG & BIT(17)))
      ;
    RSI_WURX_ReadPattern1Odd(tail_data);
    while (!(WURX_CORR_DET_READ_REG & BIT(16)))
      ;
    RSI_WURX_ReadPattern1Even(tail_data);
    while (!(WURX_CORR_DET_READ_REG & BIT(15)))
      ;
    RSI_WURX_ReadPattern2Odd(tail_data);
  } else if (tail_data_len == TAIL_DATA_DECODE_256BIT) {
    // get 256 bit tail data
    while (!(WURX_CORR_DET_READ_REG & BIT(17)))
      ;
    RSI_WURX_ReadPattern1Odd(tail_data);
    while (!(WURX_CORR_DET_READ_REG & BIT(16)))
      ;
    RSI_WURX_ReadPattern1Even(tail_data);
    while (!(WURX_CORR_DET_READ_REG & BIT(15)))
      ;
    RSI_WURX_ReadPattern2Odd(tail_data);
    while (!(WURX_CORR_DET_READ_REG & BIT(14)))
      ;
    RSI_WURX_ReadPattern2Even(tail_data);
  } else {
    return INVALID_PARAMETERS;
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           uint32_t RSI_WURX_CalThershValue(uint32_t bit_length, uint32_t percentage)
 * @brief        This API is used to calculate the threshold value.
 * @param[in]    bit_length : Bit length 64 or 32 bit.
 * @param[in]    percentage : Percentage the calculate the threshold value. 
 * @return       If success - the threshold value.
 */
uint32_t RSI_WURX_CalThershValue(uint32_t bit_length, uint32_t percentage)
{
  return ((percentage * bit_length) / VAL100);
}

/*==============================================*/
/**
 * @fn           void RSI_WURX_Pattern1MatchValue(uint32_t *match_value)
 * @brief        This API is used set the match value for detection pattern1 purpose.
 * @param[in]    match_value  : Pointer to the Match value.
 * @return       none
 */
void RSI_WURX_Pattern1MatchValue(uint32_t *match_value)
{
  // Configure MSB match value for pattern1
  WURX_PATTERN1_REG_MSB = match_value[VAL0];

  // Configure MID match value for pattern1
  WURX_PATTERN1_REG_MID = match_value[VAL1];

  // Configure LSB match value for pattern1
  WURX_PATTERN1_REG_LSB = match_value[VAL2];
}

/*==============================================*/
/**
 * @fn           void RSI_WURX_Pattern2MatchValue(uint32_t *match_value)
 * @brief        This API is used set the match value for detection pattern2 purpose.
 * @param[in]    match_value : Pointer to the Match value.
 * @return       none
 */
void RSI_WURX_Pattern2MatchValue(uint32_t *match_value)
{
  // Configure MSB match value for pattern2
  WURX_PATTERN2_REG_MSB = match_value[VAL0];

  // Configure MID match value for pattern2
  WURX_PATTERN2_REG_MID = match_value[VAL1];

  // Configure LSB match value for pattern2
  WURX_PATTERN2_REG_LSB = match_value[VAL2];
}

/*==============================================*/
/**
 * @fn           void RSI_WURX_SetPatternLength(uint16_t enable, uint16_t l1_len, uint16_t l2_len)
 * @brief        This API is used set pattern length for wakeup
 * @param[in]    enable  : Enable the set pattern length API. 
 * @param[in]    l1_len  : value to decide the l1 pattern length.
 *                         Ranges 0: 2 bits  , 1: 4 bits , 2:8 bits 3: 16 bits 
 * @param[in]    l2_len  : value to decide the l2 pattern length .
 *                         Ranges  1 : 1 bits, 2: 2 bits, 3: 4 bits
 *                                 4 : 8 bits, 5: 16 bits, 6: 32 bits
 *                                 0,7: 64 bits 
 * @return       none
 */
void RSI_WURX_SetPatternLength(uint16_t enable, uint16_t l1_len, uint16_t l2_len)
{
  if (enable) {
    // clear pattern length
    WURX_CORR_CALIB_REG &= ~(PATTERN_LEN_CLR_MASK << POS15);
    // pattern length
    WURX_CORR_CALIB_REG |= l2_len << POS15;
    // set pattern length for l1
    WURX_LEVEL1_PATTERN_REG |= (l1_len << POS20); //2 bits	   pass 1 for 4 bit
  } else {
    // disable
    WURX_CORR_CALIB_REG &= ~(l2_len << POS15);
  }
}

/*==============================================*/
/**
 * @fn           uint16_t RSI_WURX_ReadPatternLength()
 * @brief        This API is used read pattern length
 * @return       If success - pattern length value
 */
uint16_t RSI_WURX_ReadPatternLength()
{
  uint16_t value;

  // pattern length
  value = (WURX_CORR_CALIB_REG & (PATTERN_LEN_MASK)) >> POS15;

  return value;
}

/*==============================================*/
/**
 * @fn           void RSI_WURX_AnalogOff()
 * @brief        This API is used to off the wurx analog block.
 * @return       none
 */
void RSI_WURX_AnalogOff()
{
  volatile uint32_t spareReg = 0;

  WURX_ENABLE_AND_AAC_DET_REG &= ~(BIT(0));
  spareReg = ULP_SPI_MEM_MAP(IPMU_SPARE_REG2);
  spareReg &= ~BIT(20);
  ULP_SPI_MEM_MAP(IPMU_SPARE_REG2) = spareReg;
  ULP_SPI_MEM_MAP(IPMU_SPARE_REG2) &= ~BIT(21);
  WURX_TEST_MODE_REG |= BIT(19);
}

/*==============================================*/
/**
 * @fn           void RSI_WURX_DigitalOff()
 * @brief        This API is used to off the digital block.
 * @return       none
 */
void RSI_WURX_DigitalOff()
{
  RSI_IPMU_PowerGateClr(WURX_CORR_PG_ENB | WURX_PG_ENB);
}

/*==============================================*/
/**
 * @fn           void RSI_WURX_ReadPattern1Odd(uint32_t *tail_data)
 * @brief        This API is used read odd 64 bit in pattern1. 
 * @param[in]    tail_data : Its data where we store the tail data.
 * @return       none
 */
void RSI_WURX_ReadPattern1Odd(uint32_t *tail_data)
{
  uint32_t read_tail_data = 0;

  // Read 22 LSB bits of odd pattern1
  tail_data[VAL0] = WURX_ODD_PATTERN1_REG_LSB;

  // Read 22 MID bits of odd pattern1
  read_tail_data = WURX_ODD_PATTERN1_REG_MID;
  read_tail_data = (read_tail_data << POS22);

  // Write first 10 bits of odd pattern1 MID register in output buffer
  tail_data[VAL0] = tail_data[VAL0] | read_tail_data;
  read_tail_data  = 0;

  // Read MID bits of odd pattern1
  read_tail_data = WURX_ODD_PATTERN1_REG_MID;
  read_tail_data = (read_tail_data >> POS10);

  // Write last 12 bits of odd pattern1 MID register in output buffer
  tail_data[VAL1] = read_tail_data;
  read_tail_data  = 0;

  // Read 20 MSB bits of odd pattern1
  read_tail_data  = WURX_ODD_PATTERN1_REG_MSB;
  read_tail_data  = read_tail_data << POS12;
  tail_data[VAL1] = tail_data[VAL1] | read_tail_data;
}

/*==============================================*/
/**
 * @fn           void RSI_WURX_ReadPattern1Even(uint32_t *tail_data)
 * @brief        This API is used read even 64 bit in pattern1. 
 * @param[in]    tail_data : Its data where we store the tail data.
 * @return       none
 */
void RSI_WURX_ReadPattern1Even(uint32_t *tail_data)
{
  uint32_t read_tail_data = 0;

  // Read 22 LSB bits of even pattern1
  tail_data[VAL2] = WURX_EVEN_PATTERN1_REG_LSB;

  // Read 22 MID bits of even pattern1
  read_tail_data = WURX_EVEN_PATTERN1_REG_MID;
  read_tail_data = (read_tail_data << POS22);

  // Write first 10 bits of even pattern1 MID register in output buffer
  tail_data[VAL2] = tail_data[VAL2] | read_tail_data;
  read_tail_data  = 0;

  // Read MID bits of even pattern1
  read_tail_data = WURX_EVEN_PATTERN1_REG_MID;
  read_tail_data = (read_tail_data >> POS10);

  // Write last 12 bits of even pattern1 MID register in output buffer
  tail_data[VAL3] = read_tail_data;
  read_tail_data  = 0;

  // Read 20 MSB bits of even pattern1
  read_tail_data  = WURX_EVEN_PATTERN1_REG_MSB;
  read_tail_data  = read_tail_data << POS12;
  tail_data[VAL3] = tail_data[VAL3] | read_tail_data;
}

/*==============================================*/
/**
 * @fn           void RSI_WURX_ReadPattern2Odd(uint32_t *tail_data)
 * @brief        This API is used read odd 64 bit in pattern2. 
 * @param[in]    tail_data : Its data where we store the tail data.
 * @return       none
 */
void RSI_WURX_ReadPattern2Odd(uint32_t *tail_data)
{
  uint32_t read_tail_data = 0;

  // Read 22 LSB bits of odd pattern2
  tail_data[VAL4] = WURX_ODD_PATTERN2_REG_LSB;

  // Read 22 MID bits of odd pattern2
  read_tail_data = WURX_ODD_PATTERN2_REG_MID;
  read_tail_data = (read_tail_data << POS22);

  // Write first 10 bits of odd pattern1 MID register in output buffer
  tail_data[VAL4] = tail_data[VAL4] | read_tail_data;
  read_tail_data  = 0;

  // Read MID bits of odd pattern2
  read_tail_data = WURX_ODD_PATTERN2_REG_MID;
  read_tail_data = (read_tail_data >> POS10);

  // Write last 12 bits of odd pattern2 MID register in output buffer
  tail_data[VAL5] = read_tail_data;
  read_tail_data  = 0;

  // Read 20 MSB bits of odd pattern2
  read_tail_data  = WURX_ODD_PATTERN2_REG_MSB;
  read_tail_data  = read_tail_data << POS12;
  tail_data[VAL5] = tail_data[VAL5] | read_tail_data;
}

/*==============================================*/
/**
 * @fn           void RSI_WURX_ReadPattern2Even(uint32_t *tail_data)
 * @brief        This API is used read even 64 bit in pattern2. 
 * @param[in]    tail_data : Its data where we store the tail data.
 * @return       none
 */
void RSI_WURX_ReadPattern2Even(uint32_t *tail_data)
{
  uint32_t read_tail_data = 0;

  // Read 22 LSB bits of even pattern2
  tail_data[VAL6] = WURX_ODD_PATTERN2_REG_LSB;

  // Read 22 MID bits of even pattern2
  read_tail_data = WURX_ODD_PATTERN2_REG_MID;
  read_tail_data = (read_tail_data << POS22);

  // Write first 10 bits of even pattern2 MID register in output buffer
  tail_data[VAL6] = tail_data[VAL6] | read_tail_data;
  read_tail_data  = 0;

  // Read MID bits of even pattern2
  read_tail_data = WURX_ODD_PATTERN2_REG_MID;

  // Write last 12 bits of even pattern2 MID register in output buffer
  read_tail_data  = (read_tail_data >> POS10);
  tail_data[VAL7] = read_tail_data;
  read_tail_data  = 0;

  // Read 20 MSB bits of even pattern2
  read_tail_data  = WURX_ODD_PATTERN2_REG_MSB;
  read_tail_data  = read_tail_data << POS12;
  tail_data[VAL7] = tail_data[VAL7] | read_tail_data;
}

/*==============================================*/
/**
 * @fn           uint16_t RSI_WURX_TaildataPresent() 
 * @brief        This API is used to verify the tail data detection is present or not. 
 * @return       the number which contain how much data we want to read in tail data e.g if return 0 then 64bit , if 1 then 128 bit and so on. 
 */
uint16_t RSI_WURX_TaildataPresent()
{
  uint16_t read;
  read = WURX_CORR_CALIB_REG;
  read = read & TAIL_DATA_VALUE_CHECK;
  return read;
}

/*==============================================*/
/**
 * @fn           void RSI_WURX_SoftwareRestart(void)
 * @brief        This API is used to do software restart. 
 * @return       none
 */
void RSI_WURX_SoftwareRestart(void)
{
  uint32_t i;
  // soft_reset_corr set
  WURX_CORR_CALIB_REG |= BIT(21);
  while (GSPI_CTRL_REG1 & SPI_ACTIVE)
    ;
  for (i = 0; i < 400; i++) {
    __ASM("nop");
  }
  // soft_reset_corr clear
  WURX_CORR_CALIB_REG &= ~BIT(21);
  while (GSPI_CTRL_REG1 & SPI_ACTIVE)
    ;
}

/*==============================================*/
/**
 * @fn           int32_t RSI_WURX_GetPatternType(void)
 * @brief        This API is use to get pattern type . e.g pattern1 or pattern2 or false wakeup. 
 * @return       If success - Pattern type
 *               If fails - -1
 */
int32_t RSI_WURX_GetPatternType(void)
{
  if ((WURX_CORR_DET_READ_REG & BIT(20)) | (WURX_CORR_DET_READ_REG & BIT(21))) {
    return 1;
  } else if ((WURX_CORR_DET_READ_REG & BIT(19)) | (WURX_CORR_DET_READ_REG & BIT(18))) {
    return 2;
  } else {
    return -1;
  }
}

/*==============================================*/
/**
 * @fn           uint32_t RSI_WURX_CalVCOCalFreq(uint32_t frequncy_value)
 * @brief        This API is use VCO calibration frequency value. 
 * @param[in]    frequncy_value  : Transmission channel frequency value.   
 * @return       If success - the VCO calibration frequency value. 
 */
uint32_t RSI_WURX_CalVCOCalFreq(uint32_t frequncy_value)
{
  frequncy_value = (((float)frequncy_value / (80)) * 64);
  return frequncy_value;
}

/*==============================================*/
/**
 * @fn           void RSI_WURX_BGSamplingEnable()
 * @brief        This API is use enable BG sampling mode.  
 * @return       none
 */
void RSI_WURX_BGSamplingEnable()
{
  *(volatile uint32_t *)BG_SAMPLING_ADDR = BG_SAMPLING_VALUE;

  // Moving to BG sampling mode
  *(volatile uint32_t *)0x24048140 = 0x3;
}
