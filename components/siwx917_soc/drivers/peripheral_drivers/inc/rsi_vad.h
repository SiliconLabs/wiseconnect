/*******************************************************************************
* @file  rsi_vad.h
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

//Include Files

#include "rsi_ccp_common.h"
#include "rsi_error.h"

#ifndef RSI_VAD_H
#define RSI_VAD_H

#ifdef __cplusplus
extern "C" {
#endif

// VAD algorithm select enum
typedef enum VAD_ALGORITHM_SELECT {
  ZCR,               // select the ZCR algorithm for VAD detection
  ACF,               // select the ACF algorithm for VAD detection
  AMDF,              // select the AMDF algorithm for VAD detection
  WACF,              // select the WACF algorithm for VAD detection
  ZCR_ACF_AMDF_WACF, // select the ZCR,ACF,AMDF and WACF algorithm for VAD detection
  ZCR_ACF,           // select the ZCR and ACF algorithm for VAD detection
  ZCR_AMDF,          // select the ZCR and ACF algorithm for VAD detection
  ZCR_WACF           // select the ZCR and WACF algorithm for VAD detection
} VAD_ALGORITHM_SELECT_T;

// AMDF threshold configuration structure (Future usage)
typedef struct {
  uint32_t null_threshold : 12;
  uint32_t null_threshold_count : 10;
  uint32_t peak_threshold : 10;
  uint32_t peak_threshold_count : 10;
} VAD_AMDF_THRESHOLD_T;

typedef VAD_Type RSI_VAD_T;

#define VAD_PING_IRQHandler          IRQ000_Handler // VAD ping interrupt
#define VAD_1SMPLS_PER_ADDR          0x2
#define VAD_2SMPLS_PER_ADDR          0x1
#define VAD_4SMPLS_PER_ADDR          0x0
#define VAD_INTREG_SOURCE            0x0 // Feed voice data to VAD through VAD input register
#define VAD_AUXADC_SOURCE            0x3 // Directly feed ADC data to VAD (This feature not available)
#define VAD_METHOD_ZCR               0x0
#define VAD_METHOD_ACF               0x1
#define VAD_METHOD_AMDF              0x2
#define VAD_METHOD_WACF              0x3
#define VAD_METHOD_ZCR_ACF_AMDF_WACF 0x4
#define VAD_METHOD_ZCR_ACF           0x5
#define VAD_METHOD_ZCR_AMDF          0x6
#define VAD_METHOD_ZCR_WACF          0x7

// Algorithm level threshold
#define VAD_ACF_START    0x05
#define VAD_ACF_END      0x50
#define VAD_ACF_THRSHOLD 1000
#define VAD_ZCR_THRSHOLD 0x190

// Energy detection threshold
#define VAD_ENERGY_THRSHOLD 0x32

#define MAXIMUM_VALUE_1024 1024
#define MAXIMUM_VALUE_4096 4096
#define MAXIMUM_VALUE_4    4
#define MAXIMUM_VALUE_8    8

#define VAD_DIGITAL_GAIN_FAC \
  4 /*Digital multiplication value,if this value is 5 then each ADC sample will multiply by 32. 
								   data =  data << VAD_DIGITAL_GAIN_FAC */
#define ULP_MEMORY_BASE 0x24060000
#define TEST_SAMPL_VAL  32
#define VAD_MASK_VALUE  0xFC00

#define DATA_FROM_INTER_ADC \
  1                                      /* For feeding voice data to VAD from RS14100 ADC then make this macro as 1, 
                                   This macro will enable the conversion of 12 bit ADC data to 16 bit data,
                                   VAD is required 16 bit 2s complement data so if input to VAD not using 
							       RS14100 ADC output then make this macro as 0 and give proper input data to VAD */
#define VAD_INTR                  0      // VAD interrupt event
#define VAD_ENERGY_DETECT         1      // energy detection event
#define NUMBER_OF_SAMPLE_IN_FRAME 1023   // Maximum number of samples can process in VAD block
#define VAD_SCRT_PAD              0x1800 // Configure the BANK3 for VAD data processing

typedef void (*VAD_SignalEvent_t)(uint32_t event); // Pointer to VAD Event.

typedef struct VAD_EVENT {
  uint8_t vad_intr;
  uint8_t energy_detect;
  uint8_t clk_config;
  VAD_SignalEvent_t callb_event;
} VAD_EVENT_T;

// User APIs
error_t VAD_Init(VAD_SignalEvent_t Event);

int32_t VAD_Process(int16_t *wr_buf, int32_t dc_est);

error_t VAD_Deinit(void);

// Internal APIs
void RSI_VAD_PingPongMemoryAddrConfig(RSI_VAD_T *pVAD,
                                      uint32_t ping_addr,
                                      uint32_t pong_addr,
                                      uint8_t ping_enable,
                                      uint8_t pong_enable);

error_t RSI_VAD_Config(RSI_VAD_T *ptrvad,
                       uint16_t samples_per_frame,
                       uint16_t samples_per_address,
                       bool fullwidth,
                       uint8_t datasourceselect);

void RSI_VAD_Enable(RSI_VAD_T *ptrvad);

void RSI_VAD_InterruptClr(RSI_VAD_T *ptrvad, uint16_t ping_interrupt);

error_t RSI_VAD_SetAlgorithmThreshold(RSI_VAD_T *ptrvad,
                                      uint16_t algorithm_type,
                                      uint32_t zcr_threshold,
                                      uint32_t acf_threshold,
                                      uint32_t wacf_threshold,
                                      VAD_AMDF_THRESHOLD_T *config);

error_t RSI_VAD_Set_Delay(RSI_VAD_T *ptrvad, uint16_t startdelayval, uint16_t enddelayval);

error_t RSI_VAD_Input(RSI_VAD_T *ptrVad, int16_t data);

error_t RSI_VAD_FrameEnergyConfig(RSI_VAD_T *ptrvad,
                                  uint32_t threshold_frame_energy,
                                  uint32_t threshold_smpl_collect,
                                  uint32_t prog_smpls_for_energy_check);

void RSI_VAD_Stop(RSI_VAD_T *pVAD);

uint8_t RSI_VAD_ProccessDone(RSI_VAD_T *pVAD);

void RSI_VAD_FastClkEnable(uint16_t fast_clk_sel, uint16_t clk_div_factor);

int32_t RSI_VAD_ProcessData(RSI_VAD_T *pVAD,
                            uint32_t vad_addr,
                            uint32_t adc_data_addr,
                            int32_t dc_est,
                            uint32_t dig_scale,
                            uint32_t sample_len);

#ifdef __cplusplus
}
#endif

#endif // RSI_VAD_H
