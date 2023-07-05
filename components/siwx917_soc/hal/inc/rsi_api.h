/*******************************************************************************
* @file  rsi_api.h
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

#ifndef _SPIAPI_H_
#define _SPIAPI_H_
#include "rsi_driver.h"
/**
 * Include Files
 */
/**
 * Global Defines
 */

//! Host interfaces
#define RSI_SPI  0
#define RSI_UART 1
#define RSI_USB  2

#ifdef RS9116
//! Upgrade Image types
#define BURN_NWP_FW 'B'
#else
#define RSI_UPGRADE_IMAGE_I_FW '2'
#endif
#define RSI_UPGRADE_BL '#'

//! Firmware Upgradation form host params
#define FW_UP_PL          0
#define FW_UP_REQ         1
#define RSI_RPS_HEADER    32
#define RSI_FW_UP_SUCCESS 0x0003

//!Load Image types
#ifdef RS9116
#define LOAD_NWP_FW 'A'
#else
#define LOAD_NWP_FW '1'
#endif
#define LOAD_DEFAULT_NWP_FW_ACTIVE_LOW 0x71

//!Select Default image
#define SELECT_DEFAULT_NWP_FW '5'

// bypass bootup
#define ENABLE_GPIO_BASED_BYPASS  '7'
#define DISABLE_GPIO_BASED_BYPASS '8'

//!load default
#define RSI_LOAD_DEFAULT '9'

//! Check CRC
#define CHECK_NWP_INTEGRITY 'K'

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
#if 0
#define PONG_AVAIL                           'O'
#define PING_AVAIL                           'I'
#define PONG_VALID                           'O'
#define PING_VALID                           'I'
#define LOADING_INITIATED                    '1'
#define SEND_RPS_FILE                        '2'
#define FWUP_SUCCESSFUL                      'S'
#define EOF_REACHED                          'E'
#define BOOTUP_OPTIONS_LAST_CONFIG_NOT_SAVED 0xF1
#define BOOTUP_OPTIONS_CHECKSUM_FAIL         0xF2
#define INVALID_OPTION                       0xF3
#define CHECKSUM_SUCCESS                     0xAA
#define CHECKSUM_FAILURE                     0xCC
#define CHECKSUM_INVALID_ADDRESS             0x4C
#define VALID_FIRMWARE_NOT_PRESENT           0x23
#endif
#define RSI_SUCCESS 0
#define RSI_BUSY    -1
#define RSI_FAIL    -2
#ifndef RSI_BUFFER_FULL
#define RSI_BUFFER_FULL -3
#endif
#define RSI_IN_SLEEP -4

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
int16_t rsi_waitfor_boardready(void);
int16_t rsi_mem_wr(uint32_t addr, uint16_t len, uint8_t *dBuf);
int16_t rsi_mem_rd(uint32_t addr, uint16_t len, uint8_t *dBuf);
int16_t rsi_boot_insn(uint8_t type, uint16_t *data);
int16_t rsi_select_option(uint8_t cmd);
#endif
