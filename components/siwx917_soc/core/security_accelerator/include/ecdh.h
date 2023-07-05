/*******************************************************************************
* @file  ecdh.h
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
/*************************************************************************
 *
 */

#ifndef __ECDH__
#define __ECDH__

#include "stdint.h"

#define RSI_SUCCESS 0

#define ECDH_BASE_ADDRESS 0x412C0000

#define TASS_CLK_PWR_CTRL_BASE_ADDR 0x41400000

#define TASS_CLK_ENABLE_SET_REG    *(volatile uint32_t *)(TASS_CLK_PWR_CTRL_BASE_ADDR + 0x0)
#define TASS_CLK_ENABLE_CLEAR_REG  *(volatile uint32_t *)(TASS_CLK_PWR_CTRL_BASE_ADDR + 0x4)
#define TASS_PLL_ENABLE_SET_REG1   *(volatile uint32_t *)(TASS_CLK_PWR_CTRL_BASE_ADDR + 0x2C)
#define TASS_PLL_ENABLE_CLEAR_REG1 *(volatile uint32_t *)(TASS_CLK_PWR_CTRL_BASE_ADDR + 0x30)
#define TASS_PLL_STATUS_REG        *(volatile uint32_t *)(TASS_CLK_PWR_CTRL_BASE_ADDR + 0x34)
#define TASS_CLK_ENABLE_SET_REG2   *(volatile uint32_t *)(TASS_CLK_PWR_CTRL_BASE_ADDR + 0x38)
#define TASS_CLK_ENABLE_CLEAR_REG2 *(volatile uint32_t *)(TASS_CLK_PWR_CTRL_BASE_ADDR + 0x3C)

#define ECDH_CTRL_REG   *(volatile uint16_t *)(ECDH_BASE_ADDRESS + 0x20)
#define ECDH_STS_REG    *(volatile uint16_t *)(ECDH_BASE_ADDRESS + 0x22)
#define ECDH_MEM_REG    *(volatile uint16_t *)(ECDH_BASE_ADDRESS + 0x24)
#define ECDH_CTRL_K_REG *(volatile uint16_t *)(ECDH_BASE_ADDRESS + 0x26)

//! ECDH modes
#define ECDH_163 0 //! Valid only for K/B curves
#define ECDH_192 1 //! Valid for P curve
#define ECDH_224 2 //! Valid for P curve
#define ECDH_233 3 //! Valid only for K/B curves
#define ECDH_256 4 //! Valid for P curve

//! ECDH_CTRL_REG BIT defines
#define ECDH_PM_TRIG         BIT(0)
#define ECDH_PD_TRIG         BIT(1)
#define ECDH_PA_TRIG         BIT(2)
#define ECDH_AFN_TRIG        BIT(3)
#define ECDH_AFN_ON_INPUT    BIT(4)
#define ECDH_AFN_ON_OUTPUT   BIT(5)
#define ECDH_EXT_BIT_MODE    BIT(6)
#define ECDH_SOFT_RESET      BIT(7)
#define ECDH_PM_INTR_UNMASK  BIT(8)
#define ECDH_PD_INTR_UNMASK  BIT(9)
#define ECDH_PA_INTR_UNMASK  BIT(10)
#define ECDH_AFN_INTR_UNMASK BIT(11)
#define ECDH_PM_ACK          BIT(12)
#define ECDH_PD_ACK          BIT(13)
#define ECDH_PA_ACK          BIT(14)
#define ECDH_AFN_ACK         BIT(15)

//! ECDH_STS_REG BIT defines
#define ECDH_PM_DONE     BIT(0)
#define ECDH_PD_DONE     BIT(1)
#define ECDH_PA_DONE     BIT(2)
#define ECDH_AFN_DONE    BIT(3)
#define ECDH_PM_IN_PROG  BIT(4)
#define ECDH_PD_IN_PROG  BIT(5)
#define ECDH_PA_IN_PROG  BIT(6)
#define ECDH_AFN_IN_PROG BIT(7)
#define ECDH_AFN_ERR     BIT(8)
#define ECDH_ECDH_BUSY   BIT(9)

//! ECDH_MEM_REG BIT defines
#define ECDH_D_MEM_ROW        0
#define ECDH_D_THRICE_MEM_ROW BIT(0)
#define ECDH_SX_MEM_ROW       BIT(1)
#define ECDH_SY_MEM_ROW       BIT(0) | BIT(1)
#define ECDH_SZ_MEM_ROW       BIT(2)
#define ECDH_RX_MEM_ROW       BIT(2) | BIT(0)
#define ECDH_RY_MEM_ROW       BIT(2) | BIT(1)
#define ECDH_RZ_MEM_ROW       BIT(2) | BIT(1) | BIT(0)
#define ECDH_MEM_READ_EN      BIT(4)
#define ECDH_NAF_EN           BIT(5)
#define ECDH_STOP_NOWHERE     0
#define ECDH_STOP_AT_LOOP_1   BIT(6)
#define ECDH_STOP_AT_LOOP_2   BIT(7)
#define ECDH_STOP_AT_LOOP_3   BIT(6) | BIT(7)
#define ECDH_FULL_SUB_REG     BIT(8)
#define ECDH_FULL_TIME_CLK_EN BIT(9)
#define ECDH_MODE_224         BIT(10)

//! ECDH operation
#define ECDH_P_MUL 0
#define ECDH_P_ADD 1
#define ECDH_P_DOB 2
#define ECDH_P_AFN 3

#define ECDH_ROW_PM_D  0
#define ECDH_ROW_PM_3D 1
#define ECDH_ROW_PM_SX 2
#define ECDH_ROW_PM_SY 3
#define ECDH_ROW_PM_SZ 4
#define ECDH_ROW_PM_RX 5
#define ECDH_ROW_PM_RY 6
#define ECDH_ROW_PM_RZ 7
#define ECDH_ROW_PM_B  14

#define ECDH_ROW_PD_SX 5
#define ECDH_ROW_PD_SY 6
#define ECDH_ROW_PD_SZ 7
#define ECDH_ROW_PD_RX 5
#define ECDH_ROW_PD_RY 6
#define ECDH_ROW_PD_RZ 7

#define ECDH_ROW_PA_SX 5
#define ECDH_ROW_PA_SY 6
#define ECDH_ROW_PA_SZ 7
#define ECDH_ROW_PA_TX 2
#define ECDH_ROW_PA_TY 3
#define ECDH_ROW_PA_TZ 4
#define ECDH_ROW_PA_RX 5
#define ECDH_ROW_PA_RY 6
#define ECDH_ROW_PA_RZ 7

#define ECDH_ROW_AFN_SX 2
#define ECDH_ROW_AFN_SY 3
#define ECDH_ROW_AFN_SZ 4
#define ECDH_ROW_AFN_RX 2
#define ECDH_ROW_AFN_RY 3
#define ECDH_ROW_AFN_RZ 4

#define ECDH_ROW_AFN_OUT_RX 5
#define ECDH_ROW_AFN_OUT_RY 6
#define ECDH_ROW_AFN_OUT_RZ 7

//! ECDH curve types
#define ECDH_CURVE_P 0
#define ECDH_CURVE_K 1
#define ECDH_CURVE_B 2

//! ECDH Control K register bits
#define ECDH_K_ENB     BIT(0)
#define ECDH_A_K_CURVE BIT(2)
#define ECDH_MODE_163  BIT(3)
#define ECDH_MODE_233  BIT(4)

//! Proto types
int ecdh_done_hw(uint8_t ecdh_operation, uint8_t ecdh_mode, uint8_t *rx, uint8_t *ry, uint8_t *rz);
void ecdh_point_multiplication_hw(uint8_t ecdh_mode,
                                  uint8_t ecdh_curve_type,
                                  uint8_t a_curve,
                                  uint8_t *d,
                                  uint8_t *sx,
                                  uint8_t *sy,
                                  uint8_t *sz,
                                  uint8_t affinity_on_input,
                                  uint8_t affinity_on_output,
                                  uint8_t interrupt,
                                  uint8_t *rx,
                                  uint8_t *ry,
                                  uint8_t *rz);
void ecdh_point_addition_or_subtraction_hw(uint8_t ecdh_mode,
                                           uint8_t operation,
                                           uint8_t *sx,
                                           uint8_t *sy,
                                           uint8_t *sz,
                                           uint8_t *tx,
                                           uint8_t *ty,
                                           uint8_t *tz,
                                           uint8_t affinity_on_input,
                                           uint8_t affinity_on_output,
                                           uint8_t interrupt,
                                           uint8_t *rx,
                                           uint8_t *ry,
                                           uint8_t *rz);
void ecdh_point_doubling_hw(uint8_t ecdh_mode,
                            uint8_t *sx,
                            uint8_t *sy,
                            uint8_t *sz,
                            uint8_t interrupt,
                            uint8_t *rx,
                            uint8_t *ry,
                            uint8_t *rz);
void affinify(uint8_t ecdh_mode, uint8_t *rx, uint8_t *ry, uint8_t *rz);
#endif
