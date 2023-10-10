/*******************************************************************************
* @file  sli_siwx917_soc.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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

#pragma once

#include <stdint.h>

// Upgrade images
#define RSI_JUMP_TO_PC 'J'

//! Check CRC
#define CHECK_NWP_INTEGRITY 'K'
#define RSI_UPGRADE_BL      '#'

#define HOST_INTF_REG_OUT 0x4105003C
#define HOST_INTF_REG_IN  0x41050034
#define BOARD_READY       0xABCD
#define REG_READ          0xD1
#define REG_WRITE         0xD2
#define PONG_WRITE        0xD4
#define PING_WRITE        0xD5
#define GPIO_RESET        0xD7
#define LOAD_BOOTLOADER   0xD8
#ifdef RS9116
#define HOST_INTERACT_REG_VALID      (0xA0 << 8)
#define HOST_INTERACT_REG_VALID_READ (0xAB << 8)
#else
#define HOST_INTERACT_REG_VALID      (0xAB << 8)
#define HOST_INTERACT_REG_VALID_READ (0xAB << 8)
#endif

#define RSI_RESET_LOOP_COUNTER(X)     X = 0;
#define RSI_WHILE_LOOP(X, Y)          while ((X++) < (uint32_t)Y)
#define RSI_LOOP_COUNT_UPGRADE_IMAGE  0xFFFF
#define RSI_LOOP_COUNT_WAKEUP_REQ     0xFFFFFFFF
#define RSI_LOOP_COUNT_WAKEUP_WAIT    0xFFFFFFFF
#define RSI_LOOP_COUNT_UPGRADE_REQ    0xFFFF
#define RSI_LOOP_COUNT_UPGRADE_CHUNK  0xFFFF
#define RSI_LOOP_COUNT_UPGRADE_STATUS 0xFFFF
#define RSI_LOOP_COUNT_SELECT_OPTION  0xFFFF
#define RSI_CHECK_LOOP_COUNTER(X, Y) \
  {                                  \
    if (X >= Y)                      \
      return -1;                     \
  }

void sli_siwx917_update_system_core_clock(void);
void RSI_Set_Cntrls_To_M4(void);
void RSI_Set_Cntrls_To_TA(void);

void sli_si91x_platform_init(void);
int16_t sli_si91x_send_boot_instruction(uint8_t type, uint16_t *data);
int16_t rsi_waitfor_boardready(void);
int16_t rsi_select_option(uint8_t cmd);
int16_t rsi_bl_select_option(uint8_t cmd);
int16_t rsi_boot_insn(uint8_t type, uint16_t *data);
int16_t rsi_mem_rd(uint32_t addr, uint16_t len, uint8_t *dBuf);
void sl_si91x_ulp_wakeup_init(void);