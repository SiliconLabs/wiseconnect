/*******************************************************************************
* @file  rsi_sdioh.h
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

#ifndef RSI_SDIOH_H
#define RSI_SDIOH_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__CC_ARM)
#pragma push
#pragma anon_unions
#elif defined(__ICCARM__)
#pragma language = extended
#elif defined(__GNUC__)
// anonymous unions are enabled by default
#elif defined(__TMS470__)
// anonymous unions are enabled by default
#elif defined(__TASKING__)
#pragma warning 586
#else
#warning Not supported compiler type
#endif

#define MAX_SD_RETRY_TIME      (500)
#define BIT4_BUS_WIDTH_ARG     (0x80000E82)
#define BIT4_BUS_WIDTH_SET_ARG (0x00000E00)
#define LOW_SPEED_CHECK_ARG    (0x00001000)
#define IO_BLOCKSIZE_ARG       (0x80022000)
#define IO_BLOCKSIZE_ARG_1     (0x80022201)
#define HIGH_SPEED_SPRT_ARG    (0x00002600)
#define SELECT_FUNC_ARG        (0x00001A00)
#define RESET_ARG              (0x80000C08)
#define CCCR_BYTE_READ         (0x04000016)
#define CD_DISABLE_ARG         (0x80000E80)
// Function1 argument
#define FUCNTION1_CHECK_ARG      (0x00000400)
#define FUCNTION1_ENB_ARG        (0x80000402)
#define FUCNTION1_READY_ARG      (0x00000600)
#define FUNCTION1_INTR_ENB_ARG   (0x80000803)
#define FUNCTION1_INTR_CHECK_ARG (0x00000800)
#define FUNCTION1_READY          (1 << 1)
#define FUNCTION1_ENABLE         (1 << 1)
#define CSA_SUPPORT_ARG          (0x00020000)
#define CSA_ENABLE_ARG           (0x80020080)

// Function2 argument
#define FUCNTION2_CHECK_ARG    (0x00000400)
#define FUCNTION2_ENB_ARG      (0x80000404)
#define FUCNTION2_READY_ARG    (0x00000600)
#define FUNCTION2_INTR_ENB_ARG (0x80000805)
#define FUNCTION2_READY        (1 << 2)
#define FUNCTION2_ENABLE       (1 << 2)
#define LOW_SPEED_CHECK        (1 << 6)
#define BIT4_MODE_CHECK        (1 << 7)
#define BUS_BIT                (1 << 1)

#define SD_ACMD_OFFSET    (0x40)
#define MMC_CMD_TAG       (0x80)
#define ARG_ACMD41_BUSY   (0x80000000)
#define OCR_CAPACITY_MASK (0x40000000)

#define CHECK_HIGH_SPEED_SUPPORT (0x00002600)
#define ENABLE_HIGH_SPEED_MODE_ARG \
  (0x80002602) // SDIO_CMD52 ARG(CCR REG OFFSET IS 0X13 IE:13 LEFT SHIF BY 9,HERE 9 MEANS 8 BITS DATA AND 1 BIT STUFF IN CMD 52(SETTING BIT 1 IN 13TH PFFSET))

#define CHECK_UHS_SUPPORT_MODES \
  (0x00002800) // SDIO_CMD52 ARG(CCR REG OFFSET IS 0X14 IE:14 LEFT SHIF BY 9,HERE 9 MEANS 8 BITS DATA AND 1 BIT STUFF IN CMD 52) (CHECK ARGMENT)
#define UHS_1_SDR25_MODE_ARG \
  (0x80002602) // SDIO_CMD52 ARG(CCR REG OFFSET IS 0X13 IE:13 LEFT SHIF BY 9,HERE 9 MEANS 8 BITS DATA AND 1 BIT STUFF IN CMD 52(SETTING BIT 1 IN 13TH PFFSET))
#define UHS_1_SDR50_MODE_ARG  (0x80002604) // BIT 2 SETTING IN 13TH OFFSET
#define UHS_1_SDR104_MODE_ARG (0x80002606) // SETTING  BIT 1 AND 2 IN 13TH OFFSET (CCCR REG)
#define UHS_1_DDR50_MODE_ARG  (0x80002608) // SETTING BIT 3 IN 13TH OFFSET

#define HIGH_SPEED_BIT   BIT(0)
#define UHS_SUPPORT_BITS 0x7 // bit 0,1,2

#define SDIO_SET1 1

#if SDIO_SET1
#define SDIO_CLK_PIN 46
#define SDIO_CLK_PAD 11
#define SDIO_CLK_MUX EGPIO_PIN_MUX_MODE8

#define SDIO_CMD_PIN 47
#define SDIO_CMD_PAD 11
#define SDIO_CMD_MUX EGPIO_PIN_MUX_MODE8

#define SDIO_D0_PIN 48
#define SDIO_D0_PAD 11
#define SDIO_D0_MUX EGPIO_PIN_MUX_MODE8

#define SDIO_D1_PIN 49
#define SDIO_D1_PAD 12
#define SDIO_D1_MUX EGPIO_PIN_MUX_MODE8

#define SDIO_D2_PIN 50
#define SDIO_D2_PAD 12
#define SDIO_D2_MUX EGPIO_PIN_MUX_MODE8

#define SDIO_D3_PIN 51
#define SDIO_D3_PAD 12
#define SDIO_D3_MUX EGPIO_PIN_MUX_MODE8

#define SDIO_CD_PIN 53
#define SDIO_CD_PAD 13
#define SDIO_CD_MUX EGPIO_PIN_MUX_MODE8

#define SDIO_WP_PIN 52
#define SDIO_WP_PAD 13
#define SDIO_WP_MUX EGPIO_PIN_MUX_MODE8
#else
#define SDIO_CLK_PIN 25
#define SDIO_CLK_PAD 0 // no pad
#define SDIO_CLK_MUX EGPIO_PIN_MUX_MODE8

#define SDIO_CMD_PIN 26
#define SDIO_CMD_PAD 0 // no pad
#define SDIO_CMD_MUX EGPIO_PIN_MUX_MODE8

#define SDIO_D0_PIN 27
#define SDIO_D0_PAD 0 // no pad
#define SDIO_D0_MUX EGPIO_PIN_MUX_MODE8

#define SDIO_D1_PIN 28
#define SDIO_D1_PAD 0 // no pad
#define SDIO_D1_MUX EGPIO_PIN_MUX_MODE8

#define SDIO_D2_PIN 29
#define SDIO_D2_PAD 0 // no pad
#define SDIO_D2_MUX EGPIO_PIN_MUX_MODE8

#define SDIO_D3_PIN 30
#define SDIO_D3_PAD 0 // no pad
#define SDIO_D3_MUX EGPIO_PIN_MUX_MODE8

#define SDIO_CD_PIN 53
#define SDIO_CD_PAD 13
#define SDIO_CD_MUX EGPIO_PIN_MUX_MODE8

#define SDIO_WP_PIN 52
#define SDIO_WP_PAD 13
#define SDIO_WP_MUX EGPIO_PIN_MUX_MODE8
#endif

// COMMMANDS VALUE
#define CMD_53 53
#define CMD_52 52
#define CMD_5  5

#if defined(__CC_ARM)
#pragma pop
#elif defined(__ICCARM__)
// leave anonymous unions enabled
#elif defined(__GNUC__)
// anonymous unions are enabled by default
#elif defined(__TMS470__)
// anonymous unions are enabled by default
#elif defined(__TASKING__)
#pragma warning restore
#else
#warning Not supported compiler type
#endif
void RSI_SDIOH_PinMux(void);
error_t RSI_SDIOH_Enumeration(SMIH_CARD_CONFIG_T *pSmihConfig, ARM_SMIH_SignalEvent_t Event);
error_t RSI_SDIOH_RegisterInfo(SMIH_CARD_CONFIG_T *pSmihConfig, SMIH_CCCR_REG_INFO_T *pRegInfo);
error_t RSI_SDIOH_WriteCommandCmd52(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Argument);
error_t RSI_SDIOH_ReadCommandCmd52(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Argument);
error_t RSI_SDIOH_SetBusWidthCmd52(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t CmdArg);
error_t RSI_SDIOH_BusWidthConfig(SMIH_CARD_CONFIG_T *pSmihConfig, uint8_t BusWidth);
error_t RSI_SDIOH_GoIdleStateCmd0(SMIH_CARD_CONFIG_T *pSmihConfig);
error_t RSI_SDIOH_SendRelativeCardAddressCmd3(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t CmdArg);
error_t RSI_SDIOH_SwitchVoltageCmd11(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t CmdArg);
error_t RSI_SDIOH_SelectCardCmd7(SMIH_CARD_CONFIG_T *pSmihConfig);
error_t RSI_SDIOH_ByteBlockWriteCmd53(SMIH_CARD_CONFIG_T *pSmihConfig, uint8_t *pData, uint32_t Addr);
error_t RSI_SDIOH_ByteBlockReadCmd53(SMIH_CARD_CONFIG_T *pSmihConfig, uint8_t *pData, uint32_t Addr);
error_t RSI_SDIOH_ReInitializationCmd5(SMIH_CARD_CONFIG_T *pSmihConfig);
error_t RSI_SDIOH_InitializationCmd5(SMIH_CARD_CONFIG_T *pSmihConfig);

#ifdef __cplusplus
}
#endif

#endif // RSI_SDIOH_H
