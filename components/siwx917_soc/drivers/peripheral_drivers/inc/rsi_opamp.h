/*******************************************************************************
* @file  rsi_opamp.h
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

#ifndef RSI_OPAMP_H
#define RSI_OPAMP_H

#ifdef __cplusplus
extern "C" {
#endif

#define BG_SCDC_PROG_REG_1 0x127

#define AGPIO_PIN0  0
#define AGPIO_PIN1  1
#define AGPIO_PIN2  2
#define AGPIO_PIN3  3
#define AGPIO_PIN4  4
#define AGPIO_PIN5  5
#define AGPIO_PIN6  6
#define AGPIO_PIN7  7
#define AGPIO_PIN8  8
#define AGPIO_PIN9  9
#define AGPIO_PIN10 10
#define AGPIO_PIN11 11
#define AGPIO_PIN12 12
#define AGPIO_PIN13 13
#define AGPIO_PIN14 14
#define AGPIO_PIN15 15

#define TGPIO_PIN0 25
#define TGPIO_PIN1 26
#define TGPIO_PIN2 27
#define TGPIO_PIN3 28
#define TGPIO_PIN4 29
#define TGPIO_PIN5 30
#define TGPIO_PIN6 23
#define TGPIO_PIN7 24

#define AGPIO_MODE 7
#define TGPIO_MODE 14

#define OPAMP1_P_SEL_PIN AGPIO_PIN0
#define OPAMP1_N_SEL_PIN AGPIO_PIN7
#define OPAMP1_OUT       AGPIO_PIN4

#define OPAMP2_P_SEL_PIN AGPIO_PIN5
#define OPAMP2_N_SEL_PIN AGPIO_PIN11
#define OPAMP2_OUT       AGPIO_PIN9

#define OPAMP3_P_SEL_PIN AGPIO_PIN10
#define OPAMP3_N_SEL_PIN
#define OPAMP3_OUT TGPIO_PIN2

typedef struct {
  uint8_t opamp1_dyn_en;
  uint8_t opamp1_sel_p_mux;
  uint8_t opamp1_sel_n_mux;
  uint8_t opamp1_lp_mode;
  uint8_t opamp1_r1_sel;
  uint8_t opamp1_r2_sel;
  uint8_t opamp1_en_res_bank;
  uint8_t opamp1_res_mux_sel;
  uint8_t opamp1_res_to_out_vdd;
  uint8_t opamp1_out_mux_en;
  uint8_t opamp1_out_mux_sel;
  uint8_t opamp1_enable;
} OPAMP1_CONFIG_T;

typedef struct {
  uint8_t opamp2_dyn_en;
  uint8_t opamp2_sel_p_mux;
  uint8_t opamp2_sel_n_mux;
  uint8_t opamp2_lp_mode;
  uint8_t opamp2_r1_sel;
  uint8_t opamp2_r2_sel;
  uint8_t opamp2_en_res_bank;
  uint8_t opamp2_res_mux_sel;
  uint8_t opamp2_res_to_out_vdd;
  uint8_t opamp2_out_mux_en;
  uint8_t opamp2_enable;

} OPAMP2_CONFIG_T;

typedef struct {
  uint8_t opamp3_dyn_en;
  uint8_t opamp3_sel_p_mux;
  uint8_t opamp3_sel_n_mux;
  uint8_t opamp3_lp_mode;
  uint8_t opamp3_r1_sel;
  uint8_t opamp3_r2_sel;
  uint8_t opamp3_en_res_bank;
  uint8_t opamp3_res_mux_sel;
  uint8_t opamp3_res_to_out_vdd;
  uint8_t opamp3_out_mux_en;
  uint8_t opamp3_enable;
} OPAMP3_CONFIG_T;

typedef struct {
  OPAMP1_CONFIG_T opamp1;
  OPAMP2_CONFIG_T opamp2;
  OPAMP3_CONFIG_T opamp3;
} OPAMP_CONFIG_T;

void RSI_OPAMP1_UGB(uint8_t vin_p_sel,
                    uint8_t enable,
                    uint8_t lp_mode,
                    uint8_t out_mux_en,
                    uint8_t out_mux_sel,
                    uint8_t dyn_en,
                    uint8_t channel);
void RSI_OPAMP2_UGB(uint8_t vin_p_sel,
                    uint8_t enable,
                    uint8_t lp_mode,
                    uint8_t out_mux_en,
                    uint8_t dyn_en,
                    uint8_t channel);

void RSI_OPAMP3_UGB(uint8_t vin_p_sel,
                    uint8_t enable,
                    uint8_t lp_mode,
                    uint8_t out_mux_en,
                    uint8_t dyn_en,
                    uint8_t channel);

void RSI_OPAMP1_TIA(uint8_t vref_sel,
                    uint8_t iin_sel,
                    uint8_t enable,
                    uint8_t lp_mode,
                    uint8_t r2_sel,
                    uint8_t out_mux_en,
                    uint8_t out_mux_sel,
                    uint8_t dyn_en,
                    uint8_t channel);

void RSI_OPAMP2_TIA(uint8_t vref_sel,
                    uint8_t iin_sel,
                    uint8_t enable,
                    uint8_t lp_mode,
                    uint8_t r2_sel,
                    uint8_t out_mux_en,
                    uint8_t dyn_en,
                    uint8_t channel);

void RSI_OPAMP3_TIA(uint8_t vref_sel,
                    uint8_t iin_sel,
                    uint8_t enable,
                    uint8_t lp_mode,
                    uint8_t r2_sel,
                    uint8_t out_mux_en,
                    uint8_t dyn_en,
                    uint8_t channel);

void RSI_OPAMP1_InvPGA(uint8_t vref_sel,
                       uint8_t vin_sel,
                       uint8_t enable,
                       uint8_t lp_mode,
                       uint8_t r1_sel,
                       uint8_t r2_sel,
                       uint8_t out_mux_en,
                       uint8_t out_mux_sel,
                       uint8_t dyn_en,
                       uint8_t channel);

void RSI_OPAMP2_InvPGA(uint8_t vref_sel,
                       uint8_t vin_sel,
                       uint8_t enable,
                       uint8_t lp_mode,
                       uint8_t r1_sel,
                       uint8_t r2_sel,
                       uint8_t out_mux_en,
                       uint8_t dyn_en,
                       uint8_t channel);

void RSI_OPAMP3_InvPGA(uint8_t vref_sel,
                       uint8_t vin_sel,
                       uint8_t enable,
                       uint8_t lp_mode,
                       uint8_t r1_sel,
                       uint8_t r2_sel,
                       uint8_t out_mux_en,
                       uint8_t dyn_en,
                       uint8_t channel);

void RSI_OPAMP1_NonInvPGA(uint8_t vin_sel,
                          uint8_t vref_sel,
                          uint8_t enable,
                          uint8_t lp_mode,
                          uint8_t r1_sel,
                          uint8_t r2_sel,
                          uint8_t out_mux_en,
                          uint8_t out_mux_sel,
                          uint8_t dyn_en,
                          uint8_t channel);

void RSI_OPAMP2_NonInvPGA(uint8_t vin_sel,
                          uint8_t vref_sel,
                          uint8_t enable,
                          uint8_t lp_mode,
                          uint8_t r1_sel,
                          uint8_t r2_sel,
                          uint8_t out_mux_en,
                          uint8_t dyn_en,
                          uint8_t channel);

void RSI_OPAMP3_NonInvPGA(uint8_t vin_sel,
                          uint8_t vref_sel,
                          uint8_t enable,
                          uint8_t lp_mode,
                          uint8_t r1_sel,
                          uint8_t r2_sel,
                          uint8_t out_mux_en,
                          uint8_t dyn_en,
                          uint8_t channel);

void RSI_OPAMP1_InvCMP(uint8_t vin_sel,
                       uint8_t vref_sel,
                       uint8_t enable,
                       uint8_t lp_mode,
                       uint8_t r1_sel,
                       uint8_t r2_sel,
                       uint8_t out_mux_en,
                       uint8_t out_mux_sel,
                       uint8_t dyn_en,
                       uint8_t channel);

void RSI_OPAMP2_InvCMP(uint8_t vin_sel,
                       uint8_t vref_sel,
                       uint8_t enable,
                       uint8_t lp_mode,
                       uint8_t r1_sel,
                       uint8_t r2_sel,
                       uint8_t out_mux_en,
                       uint8_t dyn_en,
                       uint8_t channel);

void RSI_OPAMP3_InvCMP(uint8_t vin_sel,
                       uint8_t vref_sel,
                       uint8_t enable,
                       uint8_t lp_mode,
                       uint8_t r1_sel,
                       uint8_t r2_sel,
                       uint8_t out_mux_en,
                       uint8_t dyn_en,
                       uint8_t channel);

void RSI_OPAMP1_NonInvCMP(uint8_t vin_sel,
                          uint8_t vref_sel,
                          uint8_t enable,
                          uint8_t lp_mode,
                          uint8_t r1_sel,
                          uint8_t r2_sel,
                          uint8_t out_mux_en,
                          uint8_t out_mux_sel,
                          uint8_t dyn_en,
                          uint8_t channel);

void RSI_OPAMP2_NonInvCMP(uint8_t vin_sel,
                          uint8_t vref_sel,
                          uint8_t enable,
                          uint8_t lp_mode,
                          uint8_t r1_sel,
                          uint8_t r2_sel,
                          uint8_t out_mux_en,
                          uint8_t dyn_en,
                          uint8_t channel);

void RSI_OPAMP3_NonInvCMP(uint8_t vin_sel,
                          uint8_t vref_sel,
                          uint8_t enable,
                          uint8_t lp_mode,
                          uint8_t r1_sel,
                          uint8_t r2_sel,
                          uint8_t out_mux_en,
                          uint8_t dyn_en,
                          uint8_t channel);

void RSI_OPAMP_InstrAMP(uint8_t vin_p_sel,
                        uint8_t vin_n_sel,
                        uint8_t enable,
                        uint8_t lp_mode,
                        uint8_t r1_sel,
                        uint8_t r2_sel,
                        uint8_t out_mux_en,
                        uint8_t dyn_en,
                        uint8_t channel);

void RSI_OPAMP1_Config(OPAMP_Type *opamp, uint32_t channel, OPAMP_CONFIG_T *config);

void RSI_OPAMP2_Config(OPAMP_Type *pstcOpamp, uint32_t channel, OPAMP_CONFIG_T *config);

void RSI_OPAMP3_Config(OPAMP_Type *pstcOpamp, uint32_t channel, OPAMP_CONFIG_T *config);

#ifdef __cplusplus
}
#endif

#endif // RSI_OPAMP_H
