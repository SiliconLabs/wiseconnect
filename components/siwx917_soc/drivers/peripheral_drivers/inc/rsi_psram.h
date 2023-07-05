/*******************************************************************************
 * @file  rsi_psram.h
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
#ifndef __RSI_PSRAM_H_
#define __RSI_PSRAM_H_

#include <stdbool.h>
#include <stdint.h>
#include "rsi_qspi_proto.h"
#include "rsi_psram_config.h"

#define MHz_(a)  (a * 1000000)
#define Mbit_(a) (a * 1000000)

#define D_CACHE_ENABLE

/*PSRAM Read Write Page Size*/
#define PSRAM_READ_WRITE_PAGE_SIZE (16)

/*PSRAM Pin Configurations*/

#if defined(PSRAM_GPIO_PIN_SET_52_TO_57)
#define PSRAM_CHIP_SELECT  (CHIP_ZERO)
#define PSRAM_BASE_ADDRESS (0x0A000000)
#elif defined(PSRAM_GPIO_PIN_SET_0_TO_5)
#define PSRAM_CHIP_SELECT  (CHIP_ZERO)
#define PSRAM_BASE_ADDRESS (0x0A000000)
#elif defined(PSRAM_GPIO_PIN_SET_46_TO_51_CS_0)
#define PSRAM_CHIP_SELECT  (CHIP_ZERO)
#define PSRAM_BASE_ADDRESS (0x0A000000)
#elif defined(PSRAM_GPIO_PIN_SET_46_TO_51_CS_1)
#define PSRAM_CHIP_SELECT  (CHIP_ONE)
#define PSRAM_BASE_ADDRESS (0x0B000000)
#elif defined(PSRAM_GPIO_PIN_SET_46_TO_57_CS_0)
#define PSRAM_CHIP_SELECT  (CHIP_ZERO)
#define PSRAM_BASE_ADDRESS (0x0A000000)
#elif defined(PSRAM_GPIO_PIN_SET_46_TO_57_CS_1)
#define PSRAM_CHIP_SELECT  (CHIP_ONE)
#define PSRAM_BASE_ADDRESS (0x0B000000)
#endif

#if defined(PSRAM_GPIO_PIN_SET_0_TO_5)

#define M4SS_PSRAM_CLK_PORT (0)
#define M4SS_PSRAM_CLK_PIN  (0)
#define M4SS_PSRAM_CLK_MUX  (2)
#define M4SS_PSRAM_CLK_PAD  (0)

#define M4SS_PSRAM_CSN_PORT (0)
#define M4SS_PSRAM_CSN_PIN  (1)
#define M4SS_PSRAM_CSN_MUX  (2)
#define M4SS_PSRAM_CSN_PAD  (0)

#define M4SS_PSRAM_D0_PORT (0)
#define M4SS_PSRAM_D0_PIN  (2)
#define M4SS_PSRAM_D0_MUX  (2)
#define M4SS_PSRAM_D0_PAD  (0)

#define M4SS_PSRAM_D1_PORT (0)
#define M4SS_PSRAM_D1_PIN  (3)
#define M4SS_PSRAM_D1_MUX  (2)
#define M4SS_PSRAM_D1_PAD  (0)

#define M4SS_PSRAM_D2_PORT (0)
#define M4SS_PSRAM_D2_PIN  (4)
#define M4SS_PSRAM_D2_MUX  (2)
#define M4SS_PSRAM_D2_PAD  (0)

#define M4SS_PSRAM_D3_PORT (0)
#define M4SS_PSRAM_D3_PIN  (5)
#define M4SS_PSRAM_D3_MUX  (2)
#define M4SS_PSRAM_D3_PAD  (0)

#define NUM_OF_PSRAM_PINS (6)

#elif defined(PSRAM_GPIO_PIN_SET_46_TO_51_CS_0) || defined(PSRAM_GPIO_PIN_SET_46_TO_51_CS_1) \
  || defined(PSRAM_GPIO_PIN_SET_46_TO_57_CS_0) || defined(PSRAM_GPIO_PIN_SET_46_TO_57_CS_1)
#define M4SS_PSRAM_CLK_PORT (2)
#define M4SS_PSRAM_CLK_PIN  (14)
#define M4SS_PSRAM_CLK_MUX  (11)
#define M4SS_PSRAM_CLK_PAD  (10)

#if defined(PSRAM_GPIO_PIN_SET_46_TO_51_CS_0) || defined(PSRAM_GPIO_PIN_SET_46_TO_57_CS_0)
#define M4SS_PSRAM_CSN_PORT (3)
#define M4SS_PSRAM_CSN_PIN  (01)
#define M4SS_PSRAM_CSN_MUX  (11)
#define M4SS_PSRAM_CSN_PAD  (13)
#elif defined(PSRAM_GPIO_PIN_SET_46_TO_51_CS_1) || defined(PSRAM_GPIO_PIN_SET_46_TO_57_CS_1)
#define M4SS_PSRAM_CSN_PORT (3)
#define M4SS_PSRAM_CSN_PIN  (05)
#define M4SS_PSRAM_CSN_MUX  (11)
#define M4SS_PSRAM_CSN_PAD  (17)
#endif

#define M4SS_PSRAM_D0_PORT (2)
#define M4SS_PSRAM_D0_PIN  (15)
#define M4SS_PSRAM_D0_MUX  (11)
#define M4SS_PSRAM_D0_PAD  (11)

#define M4SS_PSRAM_D1_PORT (3)
#define M4SS_PSRAM_D1_PIN  (0)
#define M4SS_PSRAM_D1_MUX  (11)
#define M4SS_PSRAM_D1_PAD  (12)

#define M4SS_PSRAM_D2_PORT (3)
#define M4SS_PSRAM_D2_PIN  (2)
#define M4SS_PSRAM_D2_MUX  (11)
#define M4SS_PSRAM_D2_PAD  (14)

#define M4SS_PSRAM_D3_PORT (3)
#define M4SS_PSRAM_D3_PIN  (3)
#define M4SS_PSRAM_D3_MUX  (11)
#define M4SS_PSRAM_D3_PAD  (15)

#if defined(PSRAM_GPIO_PIN_SET_46_TO_57_CS_0) || defined(PSRAM_GPIO_PIN_SET_46_TO_57_CS_1)

#define M4SS_PSRAM_D4_PORT (3)
#define M4SS_PSRAM_D4_PIN  (6)
#define M4SS_PSRAM_D4_MUX  (11)
#define M4SS_PSRAM_D4_PAD  (11)

#define M4SS_PSRAM_D5_PORT (3)
#define M4SS_PSRAM_D5_PIN  (7)
#define M4SS_PSRAM_D5_MUX  (11)
#define M4SS_PSRAM_D5_PAD  (12)

#define M4SS_PSRAM_D6_PORT (3)
#define M4SS_PSRAM_D6_PIN  (8)
#define M4SS_PSRAM_D6_MUX  (11)
#define M4SS_PSRAM_D6_PAD  (14)

#define M4SS_PSRAM_D7_PORT (3)
#define M4SS_PSRAM_D7_PIN  (9)
#define M4SS_PSRAM_D7_MUX  (11)
#define M4SS_PSRAM_D7_PAD  (15)

#endif

#if defined(PSRAM_GPIO_PIN_SET_46_TO_51_CS_0) || defined(PSRAM_GPIO_PIN_SET_46_TO_51_CS_1)
#define NUM_OF_PSRAM_PINS (6)
#else
#define NUM_OF_PSRAM_PINS (10)
#endif

#elif defined(PSRAM_GPIO_PIN_SET_52_TO_57)

#define M4SS_PSRAM_CLK_PORT (3)
#define M4SS_PSRAM_CLK_PIN  (4)
#define M4SS_PSRAM_CLK_MUX  (12)
#define M4SS_PSRAM_CLK_PAD  (16)

#define M4SS_PSRAM_CSN_PORT (3)
#define M4SS_PSRAM_CSN_PIN  (7)
#define M4SS_PSRAM_CSN_MUX  (12)
#define M4SS_PSRAM_CSN_PAD  (19)

#define M4SS_PSRAM_D0_PORT (3)
#define M4SS_PSRAM_D0_PIN  (5)
#define M4SS_PSRAM_D0_MUX  (12)
#define M4SS_PSRAM_D0_PAD  (17)

#define M4SS_PSRAM_D1_PORT (3)
#define M4SS_PSRAM_D1_PIN  (6)
#define M4SS_PSRAM_D1_MUX  (12)
#define M4SS_PSRAM_D1_PAD  (18)

#define M4SS_PSRAM_D2_PORT (3)
#define M4SS_PSRAM_D2_PIN  (8)
#define M4SS_PSRAM_D2_MUX  (12)
#define M4SS_PSRAM_D2_PAD  (20)

#define M4SS_PSRAM_D3_PORT (3)
#define M4SS_PSRAM_D3_PIN  (9)
#define M4SS_PSRAM_D3_MUX  (12)
#define M4SS_PSRAM_D3_PAD  (21)

#define NUM_OF_PSRAM_PINS (6)

#endif

#ifdef D_CACHE_ENABLE
#define M4SS_DCACHE_BASE_ADDR   (0x44040000)
#define DCACHE_REG_CTRL         (*((uint32_t volatile *)(M4SS_DCACHE_BASE_ADDR + 0x010)))
#define DCACHE_REG_MAINT_STATUS (*((uint32_t volatile *)(M4SS_DCACHE_BASE_ADDR + 0x028)))
#endif

/* PSRAM commands*/
#define PSRAM_READ_ID        (0x9F)
#define PSRAM_ENTER_QPI      (0x35)
#define PSRAM_EXIT_QPI       (0xF5)
#define PSRAM_RESET_EN       (0x66)
#define PSRAM_RESET          (0x99)
#define PSRAM_BURST_LEN      (0xC0)
#define PSRAM_MODE_REG_READ  (0xB5)
#define PSRAM_MODE_REG_WRITE (0xB1)
#define PSRAM_HALF_SLEEP     (0xC0)

/*PSRAM Timings*/
#define tXPHS_US (12)
#define tXHS_US  (160)
#define tHS_US   (8)

typedef enum {
  PSRAM_SUCCESS,
  PSRAM_FAILURE,
  PSRAM_UNKNOWN,
  PSRAM_UNKNOWN_DEVICE,
  PSRAM_CLOCK_INIT_FAILURE,
  PSRAM_NOT_INITIALIZED,
  PSRAM_SUPPORTED_DEVICE,
  PSRAM_DEVICE_MISMATCH,
  PSRAM_INVALID_HSIZE,
  PSRAM_NULL_ADDRESS,
  PSRAM_INVALID_ADDRESS_LENGTH,
  PSRAM_AUTO_MODE,
  PSRAM_MANUAL_MODE,
  PSRAM_UNSUPPORTED_SECURITY,
  PSRAM_MAX_SEC_SEGMENT_REACH
} PSRAMReturnType;

typedef enum { APS1604M_SQR, APS6404L_SQRH, APS6404L_SQH, IS66WVS4M8ALL, MAX_SUPPORTED_PSRAM_DEVICES } PSRAMDevType;

typedef struct {
  uint8_t MFID;
  uint8_t KGD;
  uint8_t EID[6];
} PSRMIDType;

typedef enum { _WRAP16, _WRAP32, _WRAP64, _WRAP512 } PSRAMBurstSizeType;

typedef enum { DMA_NONE, DMA_DONE, DMA_FAIL } PSRAMDMAStatusType;

typedef struct {

  PSRAMDevType deviceName;
  PSRMIDType deviceID;
  uint32_t devDensity;
  uint32_t normalReadMAXFrequency;
  uint32_t fastReadMAXFrequency;
  spi_config_t spi_config;
  uint16_t defaultBurstWrapSize;
  uint16_t toggleBurstWrapSize;
} PSRAMInfoType;

typedef enum { initialised, uninitialised, unknown } PSRAMStateType;

typedef struct {
  PSRAMStateType state;
  PSRAMDevType ID;
  spi_config_t *spi_config;
  uint8_t spiMode;
  bool secureModeEnable;
  uint8_t secureSegmentNumber;
  uint16_t burstSize;
} PSRAMStatusType;

typedef struct {
  uint8_t port;
  uint8_t pin;
  uint8_t mux;
  uint8_t pad;
} PSRAMPinConfigType;

typedef enum { IDLE, TX_RUNNING, RX_RUNNING, FAILED } XferStatusType;

typedef struct {
  XferStatusType xferStatus;
  uint32_t xferNextAddress;
  void *xferNextSourceAddress;
  uint32_t xferRemLegth;
  uint8_t xferHsize;
  PSRAMDMAStatusType *done;
} xferContextType;

typedef struct {
  uint8_t segmentEnable;
  uint32_t lowerBoundary;
  uint32_t higherBoundary;
#define MAX_SEC_SEGMENTS 4
} PSRAMSecureSegmentType;

extern PSRAMInfoType PSRAM_Devices[MAX_SUPPORTED_PSRAM_DEVICES];

PSRAMReturnType PSRAM_Init(PSRAMDevType device);

PSRAMReturnType PSRAM_UnInit(void);

PSRAMReturnType PSRAM_Enable_AutoReadWrite(void);

PSRAMReturnType PSRAM_Disable_AutoReadWrite(void);

PSRAMReturnType PSRAM_ManualWriteInBlockingMode(uint32_t addr, void *SourceBuf, uint8_t hSize, uint32_t length);

PSRAMReturnType PSRAM_ManualReadInBlockingMode(uint32_t addr, void *DestBuf, uint8_t hSize, uint32_t length);

PSRAMReturnType PSRAM_ManualWriteInDMAMode(uint32_t addr,
                                           void *SourceBuf,
                                           uint8_t hSize,
                                           uint32_t length,
                                           PSRAMDMAStatusType *dmastatus);

PSRAMReturnType PSRAM_ManualReadInDMAMode(uint32_t addr,
                                          void *DestBuf,
                                          uint8_t hSize,
                                          uint32_t length,
                                          PSRAMDMAStatusType *dmaStatus);
PSRAMReturnType PRSAM_SetWrapSize(PSRAMBurstSizeType PSRAMBurstSize);

PSRAMReturnType PRSAM_Reset(void);

PSRAMReturnType PRSAM_Sleep(void);

PSRAMReturnType PRSAM_Wakeup(void);

PSRAMReturnType PRSAM_DetectDevice(const PSRAMDevType device);

PSRAMReturnType PRSAM_Enter_QPIMode(void);

PSRAMReturnType PRSAM_Exit_QPIMode(void);

PSRAMReturnType PSRAM_Enable_Encry_Decry(uint16_t keySize);

#endif //__RSI_PSRAM_H_
