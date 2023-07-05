/*******************************************************************************
* @file  rsi_sdmem.h
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

#include "RS1xxxx.h"
#include "base_types.h"
#include "rsi_smih.h"
#include "RTE_Device.h"

#ifndef RSI_SDMEM_H
#define RSI_SDMEM_H

#ifdef __cplusplus
extern "C" {
#endif

#define MMC_CARDS   0 // Enable this if MMC card is using
#define _8BIT_MODE  0
#define _1BIT_MODE  0
#define GPIO_SET1   1
#define CD_WPP_SET1 0

#define HIGH_SPEED_EN    0
#define ADMA_ENABLE      0
#define SD_CLOCK         25000000
#define __1P8_VOLTAGE_EN 0

#if GPIO_SET1
#define SD_CLK_PIN 46
#define SD_CLK_PAD 11
#define SD_CLK_MUX EGPIO_PIN_MUX_MODE8

#define SD_CMD_PIN 47
#define SD_CMD_PAD 11
#define SD_CMD_MUX EGPIO_PIN_MUX_MODE8

#define SD_D0_PIN 48
#define SD_D0_PAD 11
#define SD_D0_MUX EGPIO_PIN_MUX_MODE8

#define SD_D1_PIN 49
#define SD_D1_PAD 12
#define SD_D1_MUX EGPIO_PIN_MUX_MODE8

#define SD_D2_PIN 50
#define SD_D2_PAD 12
#define SD_D2_MUX EGPIO_PIN_MUX_MODE8

#define SD_D3_PIN 51
#define SD_D3_PAD 12
#define SD_D3_MUX EGPIO_PIN_MUX_MODE8

#if CD_WPP_SET1
#define SD_CD_PIN 6
#define SD_CD_PAD 1
#define SD_CD_MUX EGPIO_PIN_MUX_MODE8

#define SD_WP_PIN 7
#define SD_WP_PAD 1
#define SD_WP_MUX EGPIO_PIN_MUX_MODE8
#else
#define SD_CD_PIN 53
#define SD_CD_PAD 13
#define SD_CD_MUX EGPIO_PIN_MUX_MODE8

#define SD_WP_PIN 52
#define SD_WP_PAD 13
#define SD_WP_MUX EGPIO_PIN_MUX_MODE8
#endif

#else
#define SD_CLK_PIN 25
#define SD_CLK_PAD 0 // no pad
#define SD_CLK_MUX EGPIO_PIN_MUX_MODE8

#define SD_CMD_PIN 26
#define SD_CMD_PAD 0 // no pad
#define SD_CMD_MUX EGPIO_PIN_MUX_MODE8

#define SD_D0_PIN 27
#define SD_D0_PAD 0 // no pad
#define SD_D0_MUX EGPIO_PIN_MUX_MODE8

#define SD_D1_PIN 28
#define SD_D1_PAD 0 // no pad
#define SD_D1_MUX EGPIO_PIN_MUX_MODE8

#define SD_D2_PIN 29
#define SD_D2_PAD 0 // no pad
#define SD_D2_MUX EGPIO_PIN_MUX_MODE8

#define SD_D3_PIN 30
#define SD_D3_PAD 0 // no pad
#define SD_D3_MUX EGPIO_PIN_MUX_MODE8
#if CD_WPP_SET1
#define SD_CD_PIN 6
#define SD_CD_PAD 1
#define SD_CD_MUX EGPIO_PIN_MUX_MODE8

#define SD_WP_PIN 7
#define SD_WP_PAD 1
#define SD_WP_MUX EGPIO_PIN_MUX_MODE8
#else
#define SD_CD_PIN 53
#define SD_CD_PAD 13
#define SD_CD_MUX EGPIO_PIN_MUX_MODE8

#define SD_WP_PIN 52
#define SD_WP_PAD 13
#define SD_WP_MUX EGPIO_PIN_MUX_MODE8
#endif

#endif

#if _8BIT_MODE
#if (PACKAGE_TYPE == CC0) || (PACKAGE_TYPE == SB0N_B00) || (PACKAGE_TYPE == SB00_B00)
#define SD_D4_PIN 54
#define SD_D4_PAD 13
#define SD_D4_MUX EGPIO_PIN_MUX_MODE8

#define SD_D5_PIN 55
#define SD_D5_PAD 13
#define SD_D5_MUX EGPIO_PIN_MUX_MODE8

#define SD_D6_PIN 56
#define SD_D6_PAD 14
#define SD_D6_MUX EGPIO_PIN_MUX_MODE8

#define SD_D7_PIN 57
#define SD_D7_PAD 14
#define SD_D7_MUX EGPIO_PIN_MUX_MODE8
#else
#error "8BIT mode not supported in this package"
#endif
#endif

#define ACMD41_VOLTAGE (0x00FF8000)
#define ACMD41_UHS_REQ (0x41FF8000)
#define ACMD41_HCS     (1 << 30)

#define SD_ACMD_OFFSET  (0x40)
#define MMC_CMD_TAG     (0x80)
#define ACMD41_BUSY_BIT BIT(31)
#define ACMD41_OCR_BIT  BIT(30)

// COMMMANDS VALUE
#define CMD_8      8
#define CMD_55     55
#define CMD_11     11
#define CMD_1      1
#define CMD_2      2
#define CMD_3      3
#define CMD_9      9
#define CMD_7      7
#define CMD_6      6
#define CMD_24     24
#define CMD_25     25
#define CMD_18     18
#define CMD_17     17
#define CMD_80_hex 0x80
#define CMD_40_hex 0x40
#define CMD_8      8
#define CMD_41     41

// POSITION VALUE
#define BLOCK_SIZE_512 512

// SDMEM related function prototypes
void RSI_SDMEM_PinMux(void);
error_t RSI_SDMEM_GoIdleStateCmd0(SMIH_CARD_CONFIG_T *pSmihConfig);
error_t RSI_SDMEM_SendCardInterfaceConditionCmd8(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Arg);
error_t RSI_SDMEM_SendApplicationCommandCmd55(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Arg);
error_t RSI_SDMEM_SetCardOperationConditionAcmd41(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Arg);
error_t RSI_SDMEM_SwitchVoltageCmd11(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t CmdArg);
error_t RSI_SDMEM_SendCidCmd2(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Arg);
error_t RSI_SDIOH_SendRelativeAddressCmd3(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t CmdArg);
error_t RSI_SDMEM_SendCsdCmd9(SMIH_CARD_CONFIG_T *pSmihConfig);
error_t RSI_SDMEM_SelectCardCmd7(SMIH_CARD_CONFIG_T *pSmihConfig);
error_t RSI_SDMEM_SetBusWidthAcmd6(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Arg);
error_t RSI_SDMEM_OperationSwitchFunctionReadCmd6(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Argument);
error_t RSI_SDMEM_GetCardOperationConditionAcmd41(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Arg);
error_t RSI_SDMEM_CardBusWidthConfig(SMIH_CARD_CONFIG_T *pSmihConfig, uint8_t BusWidth);
error_t RSI_SDMEM_OperationSwitchFunctionWriteCmd6(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Argument);
error_t RSI_SDMEM_BlocksWrite(SMIH_CARD_CONFIG_T *pSmihConfig,
                              const uint8_t *DataIn,
                              uint32_t BlockIndex,
                              uint32_t BlockCount);
error_t RSI_SDMEM_BlocksRead(SMIH_CARD_CONFIG_T *pSmihConfig,
                             uint8_t *DataOut,
                             uint32_t BlockIndex,
                             uint32_t BlockCount);
error_t RSI_SDMEM_EnableHighSpeed(SMIH_CARD_CONFIG_T *pSmihConfig, boolean_t HighSpeed, uint32_t Clock);
error_t RSI_SDMEM_Enumeration(SMIH_CARD_CONFIG_T *pSmihConfig, ARM_SMIH_SignalEvent_t Event);
error_t RSI_SDMMC_Enumeration(SMIH_CARD_CONFIG_T *pSmihConfig, ARM_SMIH_SignalEvent_t Event);
error_t RSI_SDMMC_SendOperationConditionCmd1(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Arg);
error_t RSI_SDMMC_SendExtentCsdCmd(SMIH_CARD_CONFIG_T *pSmihConfig);
error_t RSI_SDMMC_ChangeBusWidthMode(SMIH_CARD_CONFIG_T *pSmihConfig, uint8_t bus_wdith);
error_t RSI_SDMMC_SwitchFunctionCmd6(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Arg);
error_t RSI_SDMMC_HighSpeedMode(SMIH_CARD_CONFIG_T *pSmihConfig, boolean_t HighSpeed, uint32_t Clock);

#ifdef __cplusplus
}
#endif

#endif // RSI_SDMEM_H
