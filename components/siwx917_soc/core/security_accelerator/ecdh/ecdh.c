/*******************************************************************************
* @file  ecdh.c
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

#include "rsi_chip.h"
#include "ecdh.h"

/*==============================================*/
/**
 * @fn          void ecdh_clock_enable()
 * @brief       This API is used to enable ECDH clock
 * @return      none
 */
void ecdh_clock_enable()
{
  *(volatile uint32_t *)TASS_CLK_PWR_CTRL_BASE_ADDR |= BIT(16);
  TASS_CLK_ENABLE_SET_REG2 = BIT(25);
}
/*==============================================*/
/**
 * @fn          void ecdh_clock_disable()
 * @brief       This API is used to disable ECDH clock
 * @return      none
 */
void ecdh_clock_disable()
{
  *(volatile uint32_t *)TASS_CLK_PWR_CTRL_BASE_ADDR &= ~BIT(16);
  TASS_CLK_ENABLE_CLEAR_REG2 = BIT(25);
}

/*==============================================*/
/**
 * @fn          int ecdh_done_hw(uint8_t ecdh_operation, uint8_t ecdh_mode, uint8_t *rx, uint8_t *ry, uint8_t *rz)
 * @brief       This API is used to get ECDH result after performing point operations
 * @param[in]   ecdh_operation, ECDH operation triggered 
 *              - ECDH_P_MUL : point multiplication
 *              - ECDH_P_ADD : point addition
 *              - ECDH_P_DOB : point doubling
 *@param[in]    rx,ry,rz: x,y,z cordinates of the result vector
 *@return       Zero - If success
 *              Non zero - If fails         
 */
int ecdh_done_hw(uint8_t ecdh_operation, uint8_t ecdh_mode, uint8_t *rx, uint8_t *ry, uint8_t *rz)
{
  uint8_t no_of_bytes, i, rx_row, ry_row, rz_row;

  if (ecdh_mode == ECDH_192) {
    no_of_bytes = 24;
  } else if (ecdh_mode == ECDH_224) {
    no_of_bytes = 28;
  } else {
    no_of_bytes = 32;
  }

  //! Setting the PM_ACK bit in control register to acknowledge receive of pm_done interrupt
  ECDH_CTRL_REG = ECDH_PM_ACK;

  //! If AFN_ERR bit of status register is set, there is an error in getting affine coordinates
  if (ECDH_STS_REG & ECDH_AFN_ERR) {
    return -1;
  }

  if (ecdh_operation == ECDH_P_MUL) {
    rx_row = ECDH_ROW_PM_RX;
    ry_row = ECDH_ROW_PM_RY;
    rz_row = ECDH_ROW_PM_RZ;
  } else if (ecdh_operation == ECDH_P_ADD) {
    rx_row = ECDH_ROW_PA_RX;
    ry_row = ECDH_ROW_PA_RY;
    rz_row = ECDH_ROW_PA_RZ;
  } else if (ecdh_operation == ECDH_P_DOB) {
    rx_row = ECDH_ROW_PD_RX;
    ry_row = ECDH_ROW_PD_RY;
    rz_row = ECDH_ROW_PD_RZ;
  } else if (ecdh_operation == ECDH_P_AFN) {
    rx_row = ECDH_ROW_AFN_OUT_RX;
    ry_row = ECDH_ROW_AFN_OUT_RY;
    rz_row = ECDH_ROW_AFN_OUT_RZ;
  } else {
  }

  //! Read RX
  ECDH_MEM_REG = (ECDH_MEM_READ_EN | rx_row);

  for (i = 0; i < no_of_bytes; i += 2) {
    *(uint16_t *)&rx[i] = *(volatile uint16_t *)(ECDH_BASE_ADDRESS + i);
  }

  //! Read RY
  ECDH_MEM_REG = (ECDH_MEM_READ_EN | ry_row);

  for (i = 0; i < no_of_bytes; i += 2) {
    *(uint16_t *)&ry[i] = *(volatile uint16_t *)(ECDH_BASE_ADDRESS + i);
  }

  //! Read RZ
  ECDH_MEM_REG = (ECDH_MEM_READ_EN | rz_row);

  for (i = 0; i < no_of_bytes; i += 2) {
    *(uint16_t *)&rz[i] = *(volatile uint16_t *)(ECDH_BASE_ADDRESS + i);
  }

  return 0;
}

/*==============================================*/
/**
 * @fn          void affinify(uint8_t ecdh_mode, uint8_t *rx, uint8_t *ry, uint8_t *rz)
 * @brief       This API is used to apply affinify in output results
 * @param[in]   ecdh_mode: ECDH mode
 *                - ECDH_163
 *                - ECDH_192
 *                - ECDH_224
 *                - ECDH_233
 *                - ECDH_256
 * @param[in]   rx,ry,rz: x,y,z cordinates of the result vector
 * @return      none
 */
void affinify(uint8_t ecdh_mode, uint8_t *rx, uint8_t *ry, uint8_t *rz)
{
  uint8_t no_of_bytes, i;
  uint32_t mode_224 = 0;
  ecdh_clock_enable();

  //! Giving reset to the ECDH engine
  ECDH_CTRL_REG = ECDH_SOFT_RESET;

  if (ecdh_mode == ECDH_192) {
    no_of_bytes = 24;
  } else if (ecdh_mode == ECDH_224) {
    no_of_bytes = 28;
    //! Set 224 mode
    mode_224 = ECDH_MODE_224;
  } else {
    no_of_bytes = 32;
    //! Set ECDH 256 mode
    ECDH_CTRL_REG = ECDH_EXT_BIT_MODE;
  }

  //! Loading Rx to HW
  ECDH_MEM_REG = (ECDH_ROW_AFN_OUT_RX | mode_224);

  for (i = 0; i < no_of_bytes; i += 2) {
    *(volatile uint16_t *)(ECDH_BASE_ADDRESS + i) = *(uint16_t *)&rx[i];
  }

  //! Loading Ry to HW
  ECDH_MEM_REG = (ECDH_ROW_AFN_OUT_RY | mode_224);

  for (i = 0; i < no_of_bytes; i += 2) {
    *(volatile uint16_t *)(ECDH_BASE_ADDRESS + i) = *(uint16_t *)&ry[i];
  }

  //! Loading Rz to HW
  ECDH_MEM_REG = (ECDH_ROW_AFN_OUT_RZ | mode_224);

  for (i = 0; i < no_of_bytes; i += 2) {
    *(volatile uint16_t *)(ECDH_BASE_ADDRESS + i) = *(uint16_t *)&rz[i];
  }

  //! Trigger AFFN
  ECDH_CTRL_REG |= ECDH_AFN_TRIG;

  while (!(ECDH_STS_REG & ECDH_AFN_DONE))
    ;

  ecdh_done_hw(ECDH_P_AFN, ecdh_mode, rx, ry, rz);
#ifdef _CHIP_9118
  ecdh_clock_disable();
#endif
}

/*==============================================*/
/**
 * @fn          void ecdh_point_multiplication_hw(uint8_t ecdh_mode,
                                  uint8_t ecdh_curve_type,
                                  uint8_t a_k_curve,
                                  uint8_t *d,
                                  uint8_t *sx,
                                  uint8_t *sy,
                                  uint8_t *sz,
                                  uint8_t affinity_on_input,
                                  uint8_t affinity_on_output,
                                  uint8_t interrupt,
                                  uint8_t *rx,
                                  uint8_t *ry,
                                  uint8_t *rz)
 * @brief       This API is used to perform point multiplication for P,K,B curves
 * @param[in]   ecdh_mode: ECDH mode
 *                - ECDH_163
 *                - ECDH_192
 *                - ECDH_224
 *                - ECDH_233
 *                - ECDH_256
 * @param[in]   ecdh_curve_type: type of the curve
 *                - ECDH_CURVE_P
 *                - ECDH_CURVE_K
 *                - ECDH_CURVE_B
 * @param[in]   d: scalar multiplicand
 * @param[in]   sx,sy,sz: x,y,z cordinates of the vector to mulply
 * @param[in]   affinity_on_input: Enable or Disable affinity on input
 *                - 0 : disable
 *                - 1 : enable
 * @param[in]   affinity_on_output: Enable or Disable affinity on output
 *                - 0 : disable
 *                - 1 : enable
 * @param[in]   interrupt: To enable interrupt or not
 *                - 0 : Disable(polling mode)
 *                - 1 : Enable
 * @param[in]   rx,ry,rz: x,y,z cordinates of the result vector
 * @return      none
 */
const uint8_t b163[] = { 0xFD, 0x05, 0x32, 0x4A, 0x74, 0x78, 0x2F, 0x51, 0x10, 0xEB, 0x81,
                         0x14, 0xCA, 0x53, 0xC9, 0xB8, 0x07, 0x19, 0x60, 0x0A, 0x2 };
const uint8_t b233[] = {
  0xAD, 0x90, 0x8F, 0x7D, 0x5F, 0x11, 0xFE, 0x81, 0x42, 0xCE, 0xE9, 0x20, 0x3B, 0x33, 0x3B, 0x21,
  0x58, 0xBB, 0x23, 0x09, 0x8C, 0x7F, 0x2C, 0x33, 0x6C, 0xDE, 0x7E, 0x64, 0x66, 0x00, 0x00, 0x00
};

void ecdh_point_multiplication_hw(uint8_t ecdh_mode,
                                  uint8_t ecdh_curve_type,
                                  uint8_t a_k_curve,
                                  uint8_t *d,
                                  uint8_t *sx,
                                  uint8_t *sy,
                                  uint8_t *sz,
                                  uint8_t affinity_on_input,
                                  uint8_t affinity_on_output,
                                  uint8_t interrupt,
                                  uint8_t *rx,
                                  uint8_t *ry,
                                  uint8_t *rz)
{

  uint8_t no_of_bytes = 0, i;
  uint32_t mode_224   = 0;

  ecdh_clock_enable();

  //! Giving reset to the ECDH engine
  ECDH_CTRL_REG = ECDH_SOFT_RESET;

  if (ecdh_mode == ECDH_192) {
    no_of_bytes = 24;
  } else if (ecdh_mode == ECDH_224) {
    no_of_bytes = 28;
    //! Set 224 mode
    mode_224 = ECDH_MODE_224;
  } else if (ecdh_mode == ECDH_256) {
    no_of_bytes = 32;
    //! Set ECDH 256 mode
    ECDH_CTRL_REG = ECDH_EXT_BIT_MODE;
  }

  if (ecdh_curve_type != ECDH_CURVE_P) {
    //! Enable K/B curve mode
    ECDH_CTRL_K_REG = ECDH_K_ENB;

    if (a_k_curve) {
      ECDH_CTRL_K_REG |= ECDH_A_K_CURVE;
    }

    if (ecdh_mode == ECDH_163) {
      ECDH_CTRL_K_REG |= ECDH_MODE_163;
    } else {
      ECDH_CTRL_K_REG |= ECDH_MODE_233;
    }
  }

  //! Load B curve
  if (ecdh_curve_type == ECDH_CURVE_B) {
    //! Loading B to HW
    ECDH_MEM_REG = ECDH_ROW_PM_B;

    if (ecdh_mode == ECDH_163) {
      for (i = 0; i < (163 / 2 + 1); i += 2) {
        *(volatile uint16_t *)(ECDH_BASE_ADDRESS + i) = *(uint16_t *)&b163[i];
      }
    } else {
      for (i = 0; i < (233 / 2 + 1); i += 2) {
        *(volatile uint16_t *)(ECDH_BASE_ADDRESS + i) = *(uint16_t *)&b233[i];
      }
    }
  } else if (ecdh_curve_type == ECDH_CURVE_K) {
    //! Load B curve
    //! Loading B to HW
    ECDH_MEM_REG = ECDH_ROW_PM_B;

    *(volatile uint16_t *)(ECDH_BASE_ADDRESS + 0) = 1;

    if (ecdh_mode == ECDH_163) {
      for (i = 2; i < (163 / 2 + 1); i += 2) {
        *(volatile uint16_t *)(ECDH_BASE_ADDRESS + i) = 0;
      }
    } else {
      for (i = 2; i < (233 / 2 + 1); i += 2) {
        *(volatile uint16_t *)(ECDH_BASE_ADDRESS + i) = 0;
      }
    }
  }

  //! Loading d to HW
  ECDH_MEM_REG = (ECDH_ROW_PM_D | mode_224);

  for (i = 0; i < no_of_bytes; i += 2) {
    *(volatile uint16_t *)(ECDH_BASE_ADDRESS + i) = *(uint16_t *)&d[i];
  }

  //! Loading Sx to HW
  ECDH_MEM_REG = (ECDH_ROW_PM_SX | mode_224);

  for (i = 0; i < no_of_bytes; i += 2) {
    *(volatile uint16_t *)(ECDH_BASE_ADDRESS + i) = *(uint16_t *)&sx[i];
  }

  //! Loading Sy to HW
  ECDH_MEM_REG = (ECDH_ROW_PM_SY | mode_224);

  for (i = 0; i < no_of_bytes; i += 2) {
    *(volatile uint16_t *)(ECDH_BASE_ADDRESS + i) = *(uint16_t *)&sy[i];
  }

  //! Loading Sz to HW
  ECDH_MEM_REG = (ECDH_ROW_PM_SZ | mode_224);

  if (affinity_on_input) {
    *(volatile uint16_t *)(ECDH_BASE_ADDRESS + 0) = 1;

    for (i = 2; i < no_of_bytes; i += 2) {
      *(volatile uint16_t *)(ECDH_BASE_ADDRESS + i) = 0;
    }
  } else {
    for (i = 0; i < no_of_bytes; i += 2) {
      *(volatile uint16_t *)(ECDH_BASE_ADDRESS + i) = *(uint16_t *)&sz[i];
    }
  }

  if (affinity_on_input) {
    //! Set affinity on input mode
    ECDH_CTRL_REG |= ECDH_AFN_ON_INPUT;
  }

  if (interrupt) {
    //! Set interrupt enable
    ECDH_CTRL_REG |= ECDH_PM_INTR_UNMASK;
  }

  //! Trigger PM mode
  ECDH_CTRL_REG |= ECDH_PM_TRIG;

  //! If interrupt is not enabled poll for the PM_DONE
  if (!interrupt) {
    while (!(ECDH_STS_REG & ECDH_PM_DONE))
      ;
  } else {
    return;
  }

  ecdh_done_hw(ECDH_P_MUL, ecdh_mode, rx, ry, rz);

  if (affinity_on_output) {
    affinify(ecdh_mode, rx, ry, rz);
  }
  ecdh_clock_disable();
}

/*==============================================*/
/**
 * @fn          void ecdh_point_addition_or_subtraction_hw(uint8_t ecdh_mode,
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
                                           uint8_t *rz)
 * @brief       This API is used to perform point addition or point subtraction
 * @param[in]   ecdh_mode: ECDH mode
 *                - ECDH_163
 *                - ECDH_192
 *                - ECDH_224
 *                - ECDH_233
 *                - ECDH_256
 * @param[in]   operation: Select operation either addition or subtraction
 *                - 0 : addition
 *                - 1 : subtraction
 * @param[in]   sx,sy,sz: x,y,z cordinates of the vector to add or subtract
 * @param[in]   tx,ty,tz: x,y,z cordinates of the vector to add or subtract
 * @param[in]   interrupt: To enable interrupt or not
 *                - 0 : Disable(polling mode)
 *                - 1 : Enable
 * @param[in]   rx,ry,rz: x,y,z cordinates of the result vector
 * @return      none
 */
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
                                           uint8_t *rz)
{
  uint8_t no_of_bytes, i;
  uint32_t mode_224 = 0;

  ecdh_clock_enable();
  //! Giving reset to the ECDH engine
  ECDH_CTRL_REG = ECDH_SOFT_RESET;

  if (ecdh_mode == ECDH_192) {
    no_of_bytes = 24;
  } else if (ecdh_mode == ECDH_224) {
    no_of_bytes = 28;
    //! Set 224 mode
    mode_224 = ECDH_MODE_224;
  } else {
    no_of_bytes = 32;
    //! Set ECDH 256 mode
    ECDH_CTRL_REG = ECDH_EXT_BIT_MODE;
  }

  //! Loading Sx to HW
  ECDH_MEM_REG = (ECDH_ROW_PA_SX | mode_224);

  for (i = 0; i < no_of_bytes; i += 2) {
    *(volatile uint16_t *)(ECDH_BASE_ADDRESS + i) = *(uint16_t *)&sx[i];
  }

  //! Loading Sy to HW
  ECDH_MEM_REG = (ECDH_ROW_PA_SY | mode_224);

  for (i = 0; i < no_of_bytes; i += 2) {
    *(volatile uint16_t *)(ECDH_BASE_ADDRESS + i) = *(uint16_t *)&sy[i];
  }

  //! Loading Sz to HW
  ECDH_MEM_REG = (ECDH_ROW_PA_SZ | mode_224);

  if (affinity_on_input) {
    *(volatile uint16_t *)(ECDH_BASE_ADDRESS + 0) = 1;

    for (i = 2; i < no_of_bytes; i += 2) {
      *(volatile uint16_t *)(ECDH_BASE_ADDRESS + i) = 0;
    }
  } else {
    for (i = 0; i < no_of_bytes; i += 2) {
      *(volatile uint16_t *)(ECDH_BASE_ADDRESS + i) = *(uint16_t *)&sz[i];
    }
  }

  //! Loading TX to HW
  ECDH_MEM_REG = (ECDH_ROW_PA_TX | mode_224);

  for (i = 0; i < no_of_bytes; i += 2) {
    *(volatile uint16_t *)(ECDH_BASE_ADDRESS + i) = *(uint16_t *)&tx[i];
  }

  //! Loading TY to HW
  ECDH_MEM_REG = (ECDH_ROW_PA_TY | mode_224);

  for (i = 0; i < no_of_bytes; i += 2) {
    *(volatile uint16_t *)(ECDH_BASE_ADDRESS + i) = *(uint16_t *)&ty[i];
  }

  //! Loading TZ to HW
  ECDH_MEM_REG = (ECDH_ROW_PA_TZ | mode_224);
  if (affinity_on_input) {
    *(volatile uint16_t *)(ECDH_BASE_ADDRESS + 0) = 1;

    for (i = 2; i < no_of_bytes; i += 2) {
      *(volatile uint16_t *)(ECDH_BASE_ADDRESS + i) = 0;
    }
  } else {
    for (i = 0; i < no_of_bytes; i += 2) {
      *(volatile uint16_t *)(ECDH_BASE_ADDRESS + i) = *(uint16_t *)&tz[i];
    }
  }

  if (affinity_on_input) {
    //! Set affinity on input mode
    ECDH_CTRL_REG |= ECDH_AFN_ON_INPUT;
  }

  if (interrupt) {
    //! Set interrupt enable
    ECDH_CTRL_REG |= ECDH_PA_INTR_UNMASK;
  }

  if (operation) {
    //! Set subtraction bit
    ECDH_MEM_REG |= ECDH_FULL_SUB_REG;
  }

  //! Trigger PM mode
  ECDH_CTRL_REG |= ECDH_PA_TRIG;

  //! If interrupt is not enabled poll for the PM_DONE
  if (!interrupt) {
    while (!(ECDH_STS_REG & ECDH_PA_DONE))
      ;
  } else {
    return;
  }

  ecdh_done_hw(ECDH_P_ADD, ecdh_mode, rx, ry, rz);

  if (affinity_on_output) {
    affinify(ecdh_mode, rx, ry, rz);
  }
  ecdh_clock_disable();
}

/*==============================================*/
/**
 * @fn          void ecdh_point_doubling_hw(uint8_t ecdh_mode,
                            uint8_t *sx,
                            uint8_t *sy,
                            uint8_t *sz,
                            uint8_t interrupt,
                            uint8_t *rx,
                            uint8_t *ry,
                            uint8_t *rz)
 * @brief       This API is used to perform point doubling
 * @param[in]   ecdh_mode: ECDH mode
 *                - ECDH_163
 *                - ECDH_192
 *                - ECDH_224
 *                - ECDH_233
 *                - ECDH_256
 * @param[in]   sx,sy,sz: x,y,z cordinates of the vector to mulply
 * @param[in]   interrupt: To enable interrupt or not
 *                - 0 : Disable(polling mode)
 *                - 1 : Enable
 * @param[in]   rx,ry,rz: x,y,z cordinates of the result vector
 * @return      none
 */
void ecdh_point_doubling_hw(uint8_t ecdh_mode,
                            uint8_t *sx,
                            uint8_t *sy,
                            uint8_t *sz,
                            uint8_t interrupt,
                            uint8_t *rx,
                            uint8_t *ry,
                            uint8_t *rz)
{
  uint8_t no_of_bytes, i;
  uint32_t mode_224 = 0;
  ecdh_clock_enable();

  //! Giving reset to the ECDH engine
  ECDH_CTRL_REG = ECDH_SOFT_RESET;

  if (ecdh_mode == ECDH_192) {
    no_of_bytes = 24;
  } else if (ecdh_mode == ECDH_224) {
    no_of_bytes = 28;
    //! Set 224 mode
    mode_224 = ECDH_MODE_224;
  } else {
    no_of_bytes = 32;
    //! Set ECDH 256 mode
    ECDH_CTRL_REG = ECDH_EXT_BIT_MODE;
  }

  //! Loading Sx to HW
  ECDH_MEM_REG = (ECDH_ROW_PD_SX | mode_224);

  for (i = 0; i < no_of_bytes; i += 2) {
    *(volatile uint16_t *)(ECDH_BASE_ADDRESS + i) = *(uint16_t *)&sx[i];
  }

  //! Loading Sy to HW
  ECDH_MEM_REG = (ECDH_ROW_PD_SY | mode_224);

  for (i = 0; i < no_of_bytes; i += 2) {
    *(volatile uint16_t *)(ECDH_BASE_ADDRESS + i) = *(uint16_t *)&sy[i];
  }

  //! Loading Sz to HW
  ECDH_MEM_REG = (ECDH_ROW_PD_SZ | mode_224);

  for (i = 0; i < no_of_bytes; i += 2) {
    *(volatile uint16_t *)(ECDH_BASE_ADDRESS + i) = *(uint16_t *)&sz[i];
  }

  if (interrupt) {
    //! Set interrupt enable
    ECDH_CTRL_REG |= ECDH_PD_INTR_UNMASK;
  }

  //! Trigger PM mode
  ECDH_CTRL_REG |= ECDH_PD_TRIG;

  //! If interrupt is not enabled poll for the PM_DONE
  if (!interrupt) {
    while (!(ECDH_STS_REG & ECDH_PD_DONE))
      ;
  } else {
    return;
  }

  ecdh_done_hw(ECDH_P_DOB, ecdh_mode, rx, ry, rz);
  ecdh_clock_disable();
}
