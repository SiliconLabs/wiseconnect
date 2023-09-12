/*******************************************************************************
* @file  rsi_pmu.h
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

#ifndef __RSI_PMU_H__
#define __RSI_PMU_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "RS1xxxx.h"

/**PMU REGISTER MAP*/
#define PMU_IP3_CTRL_REG    0x1D0
#define PMU_PFM_REG         0x1D1
#define PMU_ADC_REG         0x1D2
#define PMU_PID_REG1        0x1D3
#define PMU_PID_REG2        0x1D4
#define PMU_PTAT_REG        0x1D5
#define PMU_LDO_REG         0x1D6
#define PMU_PID_ADC_OUT_REG 0x1D7
#define PMU_POWERTRAIN_REG  0x1D8
#define PMU_TESTMUX_REG1    0x1D9
#define PMU_TEST_MODES      0x1DA
#define PMU_TESTMUX_REG2    0x1DB

#define SET_VREF1P3_MASK          0x001E0000
#define SET_VREF_ISENSE1P3_MASK   0x00060000
#define SET_VREF_ADC_MASK         0x00060000
#define P_1P3_MASK                0x00000FFF
#define D_1P3_MASK                0x00003FFF
#define N_1P3_MASK                0x00018000
#define I_SOFT_START1P3_MASK      0x00000FFF
#define I_STEADY_STATE1P3_MASK    0x00000FFF
#define UI_RESET_VAL1P3_MASK      0X0003FF00
#define DPWM_FREQ_TRIM1P3_MASK    0x0001E000
#define DEAD_TIME_CONTROLLP3_MASK 0x0001E000
#define DRIVER_P_SEL_MASK         0x00003000
#define DRIVER_N_SEL_MASK         0x00000c00
#define PFMMODE_NMOS_TON_INC_MASK 0x00001000
#define PFM_PON_TIME_SEL_MASK     0x003C0000
#define PFM_NON_TIME_SEL_MASK     0x00018000
#define SET_PFM_FREQ1P3_MASK      0x0000e000
#define PT_GATE_CTRL_MASK         0x0000e000
#define CTRL_LDOSOC_MASK          0x0000000F
#define CL_ON_LDOSOC              BIT(12)
#define PTAT_LOAD_CTRL_MASK       0x00070000
#define LDOSOC_DEFAULT_MODE_EN    BIT(5)
#define DISABLE_LDOSOC_SFST       BIT(16)
#define CTRL_LDORF_MASK           0x000003c0
#define CL_ON_LDORF               BIT(13)
#define LDORF_DEFAULT_MODE_EN     BIT(11)

#define TEST_ANA_MUX1_SEL_MASK 0x00380000
#define TEST_ANA_MUX2_SEL_MASK 0x00070000

#define TEST_DIG_MUX1_SEL_MASK 0x0000F000
#define TEST_DIG_MUX2_SEL_MASK 0x00000F00

rsi_error_t RSI_PMU_SetChipVoltage(uint8_t enVoltage);

/*End of file not truncated*/
#ifdef __cplusplus
}
#endif

#endif /*__RSI_PMU_H__*/
