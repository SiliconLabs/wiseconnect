/*******************************************************************************
* @file  rsi_dac.h
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

// Includes Files

#include "rsi_chip.h"

#ifndef RSI_AUX_DAC_H
#define RSI_AUX_DAC_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_DIV_FAC_DAC      1023
#define MAX_DAC_FIFO_THR     7
#define MAX_DAC_SAMP_RATE    5000000
#define AUXDACREG0           0x11A
#define DAC_FIFO_THR         3 // Set FIFO threshold as 3
#define STATIC_MODE_EN       1
#define FIFO_MODE_EN         0
#define DAC_OUT_AGPIO15      15
#define DAC_OUT_AGPIO4       4
#define DAC_STATIC_MODE_INTR BIT(24)

// brief DAC channel Configuration structure
typedef enum { DAC_POWER_ON = 0, DAC_POWER_OFF } POWER_STATE_DAC;

// Call back handler
typedef void (*daccallbacFunc)(uint8_t event); // Call back function pointer

typedef struct {
  daccallbacFunc callback_event;
} dac_config_t;

// Call back event
#define DAC_UDMA_PING_PONG_CONFIG 1
#define DAC_STATIC_MODE_CALLBACK  2
#define DAC_INPUT_REG_ADDR        0x24043810

uint32_t DAC_Init(uint8_t operation_mode, uint32_t sampling_rate, daccallbacFunc event);

error_t DAC_WriteData_StaticMode(int16_t input_data);

error_t DAC_WriteData(uint8_t operation_mode, int16_t *wr_buf, uint16_t length);

error_t DAC_Start(uint8_t operation_mode);

error_t DAC_Stop(void);

error_t DAC_Deinit(void);

error_t DAC_PingPongReconfig(int16_t *wr_buf, uint16_t length);

// New internal API
error_t DAC_PinMux(uint8_t pin_sel);

uint32_t dac_set_clock(uint32_t sampl_rate);

error_t RSI_DAC_ClkDivFactor(AUX_ADC_DAC_COMP_Type *pstcDAC, uint16_t div_factor);

void RSI_DAC_PowerControl(POWER_STATE_DAC state);

error_t RSI_DAC_Config(AUX_ADC_DAC_COMP_Type *pstcDAC,
                       uint8_t static_fifo_mode,
                       uint16_t aux_dac_out_mux_en,
                       uint16_t aux_dac_out_mux_sel,
                       uint8_t prbs_sel);

error_t RSI_DAC_WriteData(AUX_ADC_DAC_COMP_Type *pstcDAC, int16_t *data, uint8_t static_fifo_mode, uint16_t len);

error_t RSI_DAC_DynamicModeWriteData(AUX_ADC_DAC_COMP_Type *pstcDAC, uint16_t channel, uint16_t *data, uint32_t len);

uint16_t RSI_DAC_DyanamicModeReadData(AUX_ADC_DAC_COMP_Type *pstcDAC, uint32_t channel, uint16_t data);

error_t RSI_DAC_Stop(AUX_ADC_DAC_COMP_Type *pstcDAC);

error_t RSI_DAC_Start(AUX_ADC_DAC_COMP_Type *pstcDAC, uint16_t aux_dac_en);

error_t RSI_DAC_DynamicModeConfig(AUX_ADC_DAC_COMP_Type *pstcDAC,
                                  uint16_t channel_no,
                                  uint16_t aux_dac_out_mux_en,
                                  uint16_t aux_dac_out_mux_sel);

error_t RSI_DAC_DynamicModeStart(AUX_ADC_DAC_COMP_Type *pstcDAC, uint32_t channel, uint16_t aux_dac_en);

error_t RSI_DAC_SetFifoThreshold(AUX_ADC_DAC_COMP_Type *pstcDAC, uint32_t fifo_threshold);

uint16_t RSI_DAC_ReadData(AUX_ADC_DAC_COMP_Type *pstcDAC);

#ifdef CHIP_9118
error_t RSI_DAC_InterruptUnMask(AUX_ADC_DAC_COMP_Type *pstcDAC);
error_t RSI_DAC_InterruptMask(AUX_ADC_DAC_COMP_Type *pstcDAC);
#endif
#ifdef CHIP_917
error_t RSI_DAC_InterruptUnMask(AUX_ADC_DAC_COMP_Type *pstcDAC, uint8_t oper_mode);
error_t RSI_DAC_InterruptMask(AUX_ADC_DAC_COMP_Type *pstcDAC, uint8_t oper_mode);
#endif

error_t RSI_DAC_InterruptClr(AUX_ADC_DAC_COMP_Type *pstcDAC);

void dac_udma_init(void);

void dac_udma_write(uint8_t ping_pong_write, uint16_t num_of_samples, int16_t *input_buff, uint8_t skip_flag);

void dac_udma_start(void);

#ifdef __cplusplus
}
#endif

#endif // RSI_AUX_DAC_H
