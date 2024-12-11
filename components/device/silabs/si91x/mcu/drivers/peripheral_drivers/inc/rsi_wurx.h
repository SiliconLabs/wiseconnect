/******************************************************************************
* @file  rsi_wurx.h
*******************************************************************************
* # License
* <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* SPDX-License-Identifier: Zlib
*
* The licensor of this software is Silicon Laboratories Inc.
*
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software. If you use this software
*    in a product, an acknowledgment in the product documentation would be
*    appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*
******************************************************************************/

//Include Files

#ifndef RSI_WURX_H
#define RSI_WURX_H

#ifdef __cplusplus
extern "C" {
#endif

#define PMU_SPI_BASE_ADDR 0x24050000

#define WURX_COMP_OFFSET_CALIB_REG  *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x080) * 4)
#define WURX_LCO_FREQ_CALIB_REG     *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x081) * 4)
#define WURX_AAC_MODE_REG           *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x082) * 4)
#define WURX_CLK_GEN_REG            *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x083) * 4)
#define WURX_LNA_IF_REG             *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x084) * 4)
#define WURX_ENABLE_AND_AAC_DET_REG *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x085) * 4)
#define WURX_TEST_MUX_REG           *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x086) * 4)
#define WURX_TEST_MODE_REG          *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x087) * 4)

#define WURX_PATTERN1_REG_MSB       *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x089) * 4)
#define WURX_PATTERN1_REG_MID       *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x08A) * 4)
#define WURX_PATTERN1_REG_LSB       *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x08B) * 4)
#define WURX_PATTERN2_REG_MSB       *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x08C) * 4)
#define WURX_PATTERN2_REG_MID       *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x08D) * 4)
#define WURX_PATTERN2_REG_LSB       *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x08E) * 4)
#define WURX_CORR_DET_READ_REG      *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x08F) * 4)
#define WURX_ODD_PATTERN1_REG_LSB   *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x090) * 4)
#define WURX_ODD_PATTERN1_REG_MID   *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x091) * 4)
#define WURX_ODD_PATTERN1_REG_MSB   *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x092) * 4)
#define WURX_EVEN_PATTERN1_REG_LSB  *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x093) * 4)
#define WURX_EVEN_PATTERN1_REG_MID  *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x094) * 4)
#define WURX_EVEN_PATTERN1_REG_MSB  *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x095) * 4)
#define WURX_ODD_PATTERN2_REG_LSB   *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x096) * 4)
#define WURX_ODD_PATTERN2_REG_MID   *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x097) * 4)
#define WURX_ODD_PATTERN2_REG_MSB   *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x098) * 4)
#define WURX_EVEN_PATTERN2_REG_LSB  *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x099) * 4)
#define WURX_EVEN_PATTERN2_REG_MID  *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x09A) * 4)
#define WURX_EVEN_PATTERN2_REG_MSB  *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x09B) * 4)
#define WURX_LEVEL1_PATTERN_REG     *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x09C) * 4)
#define WURX_BYPASS_LEVEL1_AND_FREQ *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x09D) * 4)
#define WURX_MANUAL_CALIB_MODE_REG1 *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x09E) * 4)
#define WURX_MANUAL_CALIB_MODE_REG2 *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x09F) * 4)
#define WURX_CORR_CALIB_REG         *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x088) * 4)
#define ULPCLKS_REFCLK_REG          *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xa000 + (0x106) * 4)

#define BG_SAMPLING_ADDR       0x41300140
#define BG_SAMPLING_VALUE      0x3
#define TAIL_DATA_VALUE_CHECK  0x3
#define SYNC_32KHZ_RESET_VALUE 0x000001
#define RESET_CALB_CLOCK       0x000002
#define LCO_CALB_STATUS        0x000002

#define VAL0   0
#define VAL1   1
#define VAL2   2
#define VAL3   3
#define VAL4   4
#define VAL5   5
#define VAL6   6
#define VAL7   7
#define VAL10  10
#define VAL100 100

#define POS1  1
#define POS3  3
#define POS4  4
#define POS7  7
#define POS9  9
#define POS10 10
#define POS9  9
#define POS11 11
#define POS12 12
#define POS13 13
#define POS16 16
#define POS15 15
#define POS17 17
#define POS18 18
#define POS19 19
#define POS20 20
#define POS21 21
#define POS22 22

#define IPMU_SPARE_REG2         0x141
#define DC_OFFSET_VALUE         0x40
#define REF_CLOCK_FREQ          0x400
#define DETECTION_REF_SHIFT     0x1FC1FF
#define TAIL_DATA_DECODE_64BIT  0x0
#define TAIL_DATA_DECODE_128BIT 0x1
#define TAIL_DATA_DECODE_192BIT 0x2
#define TAIL_DATA_DECODE_256BIT 0x3
#define LDO_ON_MODE             0xA8000020
#define PATTERN_LEN_MASK        0x38000
#define PATTERN_LEN_CLR_MASK    0x7
#define THRESH_CLR_MASK_VAL     0x3F

uint8_t RSI_WURX_GetPatternDetectedType(void);
void RSI_WURX_ConfigL2Patttern(uint8_t freq_div);
void RSI_IPMU_DCCalib(void);
uint16_t RSI_WURX_ReadPatternLength(void);
void RSI_WURX_AnalogOff(void);
void RSI_WURX_DigitalOff(void);
void RSI_WURX_Init(uint16_t bypass_l1_enable, uint16_t l1_freq_div, uint16_t l2_freq_div);
void RSI_WURX_ReadPattern2Even(uint32_t *tail_data);
void RSI_WURX_ReadPattern1Even(uint32_t *tail_data);
void RSI_WURX_ReadPattern1Odd(uint32_t *tail_data);
void RSI_WURX_ReadPattern2Odd(uint32_t *tail_data);
void RSI_WURX_Pattern1MatchValue(uint32_t *match_value);
void RSI_WURX_Pattern2MatchValue(uint32_t *match_value);
uint16_t RSI_WURX_TaildataPresent(void);
void RSI_WURX_CorrEnable(uint16_t wurx_enable);
void RSI_WURX_SetPatternLength(uint16_t enable, uint16_t l1_len, uint16_t l2_len);
void RSI_WURX_SetWakeUpThreshold(uint16_t threshold_1, uint16_t threshold_2);
void RSI_WURX_TailDataDecodeEnable(uint16_t enable, uint16_t data_len);
rsi_error_t RSI_WURX_GetTailData(uint32_t *tail_data, uint16_t tail_data_len);
void RSI_IPMU_40MhzClkCalib(uint16_t clk_enable, uint32_t channel_selection_value);
uint32_t RSI_WURX_CalThershValue(uint32_t bit_length, uint32_t percentage);
void RSI_WURX_SoftwareRestart(void);
int32_t RSI_WURX_GetPatternType(void);
void RSI_WURX_BGSamplingEnable(void);

#ifdef __cplusplus
}
#endif

#endif // RSI_WURX_H
