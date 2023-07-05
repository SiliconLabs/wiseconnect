/*******************************************************************************
* @file  rsi_smih.h
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

#ifndef RSI_SMIH_H
#define RSI_SMIH_H

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

#define ACMD41_VOLTAGE (0x00FF8000)
#define ACMD41_UHS_REQ (0x41FF8000)
#define ACMD41_HCS     (1 << 30)

// SMIH Controller related defines and structures
typedef void (*ARM_SMIH_SignalEvent_t)(uint32_t event); // Pointer to \ref ARM_USART_SignalEvent : Signal USART Event.

#define LOOP_COUNT_TIME (500)

// Normal interrupt status enable reg
#define COMMAND_COMPLETE_STATUS_ENABLE   BIT(0)
#define TRANSFER_COMPLETE_STATUS_ENABLE  BIT(1)
#define BLOCK_GAP_EVENT_STATUS_ENABLE    BIT(2)
#define DMA_INTERRUPT_STATUS_ENABLE      BIT(3)
#define BUFFER_WRITE_READY_STATUS_ENABLE BIT(4)
#define BUFFER_READ_READY_STATUS_ENABLE  BIT(5)
#define CARD_INSERTION_STATUS_ENABLE     BIT(6)
#define CARD_REMOVAL_STATUS_ENABLE       BIT(7)
#define CARD_INTERRUPT_STATUS_ENABLE     BIT(8)
#define INT_A_STATUS_ENABLE              BIT(9)
#define INT_B_STATUS_ENABLE              BIT(10)
#define INT_C_STATUS_ENABLE              BIT(11)
#define RE_TUNING_EVENT_STATUS_ENABLE    BIT(12)

// Error interrupt status enables
#define COMMAND_TIMEOUT_ERROR_STATUS_ENABLE BIT(0)
#define COMMAND_CRC_ERROR_STATUS_ENABLE     BIT(1)
#define COMMAND_END_BIT_ERROR_STATUS_ENABLE BIT(2)
#define COMMAND_INDEX_ERROR_STATUS_ENABLE   BIT(3)
#define DATA_TIMEOUT_ERROR_STATUS_ENABLE    BIT(4)
#define DATA_CRC_ERROR_STATUS_ENABLE        BIT(5)
#define DATA_END_BIT_ERROR_STATUS_ENABLE    BIT(6)
#define CURRENT_LIMIT_ERROR_STATUS_ENABLE   BIT(7)
#define AUTO_CMD_ERROR_STATUS_ENABLE        BIT(8)
#define ADMA_ERROR_STATUS_ENABLE            BIT(9)
#define TUNING_ERROR_STATUS_ENABLE          BIT(10)

// Normal interrupt status enable reg
#define COMMAND_COMPLETE_SIGNAL_ENABLE   BIT(0)
#define TRANSFER_COMPLETE_SIGNAL_ENABLE  BIT(1)
#define BLOCK_GAP_EVENT_SIGNAL_ENABLE    BIT(2)
#define DMA_INTERRUPT_SIGNAL_ENABLE      BIT(3)
#define BUFFER_WRITE_READY_SIGNAL_ENABLE BIT(4)
#define BUFFER_READ_READY_SIGNAL_ENABLE  BIT(5)
#define CARD_INSERTION_SIGNAL_ENABLE     BIT(6)
#define CARD_REMOVAL_SIGNAL_ENABLE       BIT(7)
#define CARD_INTERRUPT_SIGNAL_ENABLE     BIT(8)
#define INT_A_SIGNAL_ENABLE              BIT(9)
#define INT_B_SIGNAL_ENABLE              BIT(10)
#define INT_C_SIGNALS_ENABLE             BIT(11)
#define RE_TUNING_EVENT_SIGNAL_ENABLE    BIT(12)

#define DISABLE_AUTO_CMD  0
#define ENABLE_AUTO_CMD12 1
#define ENABLE_AUTO_CMD23 2

// Command type defines
#define ABORT_CMD   0
#define RESUME_CMD  1
#define SUSPEND_CMD 2
#define NORMAL_CMD  3

// response type defines
#define SMIH_NO_RESPONSE                      0
#define SMIH_RESPONSE_LENGTH_136              1
#define SMIH_RESPONSE_LENGTH_48               2
#define SMIH_RESPONSE_LENGTH_48BIT_BUSY_CHECK 3

// ultra high speed mode defines
#define UHS_NONE   0x0
#define UHS_SDR12  0x1
#define UHS_SDR25  0x2
#define UHS_SDR50  0x3
#define UHS_SDR104 0x4
#define UHS_DDR50  0x5

// response types to the commands
#define SMIH_RESPONSE_NONE     0x0
#define SMIH_RESPONSE_R2       0x1
#define SMIH_RESPONSE_R3R4     0x2
#define SMIH_RESPONSE_R1R5R6R7 0x3
#define SMIH_RESPONSE_R1BR5B   0x5

// smih volatge level defines
#define VOLTAGE_18V 0x0
#define VOLTAGE_30V 0x1
#define VOLTAGE_33V 0x2

// reset defines
#define SMIH_DATA_LINE_RESET    0x0
#define SMIH_COMMAND_LINE_RESET 0x1
#define SMIH_ALL_RESET          0x2

#define SMIH_CARD_STANDARD      0x0
#define SMIH_CARD_HIGH_CAPACITY 0x1

// data direction defines
#define SMIH_WRITE_DIRECTION 0x0
#define SMIH_READ_DIRECTION  0x1

// command events defines
#define SMIH_CMD_COMPLETE_EVENT       0x1
#define SMIH_TRANSFER_COMPLETE_EVENT  0x2
#define SMIH_BUFFER_READ_READY_EVENT  0x3
#define SMIH_BUFFER_WRITE_READY_EVENT 0x4

// bus width mode defines
#define SMIH_BUS_WIDTH1 0x0
#define SMIH_BUS_WIDTH4 0x1
#define SMIH_BUS_WIDTH8 0x2
typedef struct SMIH_COMMAND_REG {
  uint16_t respType : 2;
  uint16_t resrvd : 1;
  uint16_t cmdCrcCheckEnable : 1;
  uint16_t cmdIndexCheckEnable : 1;
  uint16_t dataPresentSelect : 1;
  uint16_t cmdType : 2;
  uint16_t cmdIndex : 6;
} SMIH_COMMAND_REG_T;

typedef struct SMIH_COMMAND_FRAME_CONFIG {
  uint8_t cmdIndex;
  uint32_t cmdArgument;
  uint8_t cmdType;
  boolean_t dataPresentSelect;
  boolean_t cmdIndexCheckEn;
  boolean_t cmdCrcCheckEn;
  uint8_t responseTypeSelect;
  uint8_t autoCmdType;
} SMIH_COMMAND_FRAME_CONFIG_T;

typedef struct SMIH_EVENT {
  uint8_t commandComplete;
  uint8_t transferComplete;
  uint8_t dmaInterrupt;
  uint8_t bufferWriteReady;
  uint8_t bufferReadReady;
  uint8_t cardInsertion;
  uint8_t cardRemoval;
  uint8_t cardInterrupt;
  uint8_t commandTimeoutError;
  uint8_t commandCrcError;
  uint8_t commandEndBitError;
  uint8_t commandIndexError;
  uint8_t dataTimeoutError;
  uint8_t dataEndbitError;
  uint8_t dataCrcError;
  uint8_t currentLimitError;
  uint8_t autoCommandError;
  uint8_t admaError;
  uint8_t tuningError;
  ARM_SMIH_SignalEvent_t callb_event;
} SMIH_EVENT_T;

#define COMMAND_COMPLETE    0
#define TRANSFER_COMPLETE   1
#define DMA_INTR            2
#define BUFFER_WRITE_READY  3
#define BUFFER_READ_READY   4
#define CARD_INSERTION      5
#define CARD_REMOVAL        6
#define CARD_INTERRUPT      7
#define CMD_TIMEOUT_ERROR   8
#define CMD_CRC_ERROR       9
#define CMD_END_BIT_ERROR   10
#define CMD_INDEX_ERROR     11
#define DATA_TIMEOUT_ERROR  12
#define DATA_END_BIT_ERROR  13
#define DATA_CRC_ERROR      14
#define CURRENT_LIMIT_ERROR 15
#define AUTO_CMD_ERROR      16
#define ADMA_ERROR          17
#define TUNING_ERROR        18

typedef struct SMIH_CMD_FEILD {
  uint32_t cmdIdx;
  uint32_t cmdArg;
  uint8_t responseTypeSelect;
  uint32_t response[4];
  SMIH_EVENT_T event;
} SMIH_CMD_FEILD_T;

typedef struct SMIH_DATA_FEILD {
  const uint8_t *data;
  uint32_t blockSize;
  uint32_t blockCount;
  uint8_t direction;
} SMIH_DATA_FEILD_T;

typedef struct SMIH_TRANSFER {
  SMIH_CMD_FEILD_T *command;
  SMIH_DATA_FEILD_T *data;

} SMIH_TRANSFER_T;

typedef struct SMIH_DATA_CONFIG {
  boolean_t multiBlock;
  boolean_t dataTransferDirection;
  uint16_t blockSize;
  uint16_t blockCount;
  boolean_t blockCountEnable;
  boolean_t dmaEnable;
  uint32_t admaDespTableAddress;
  uint8_t dataTimeout;
} SMIH_DATA_CONFIG_T;

typedef struct SMIH_CONFIG_MODES {
  uint8_t busWidthMode;
  uint32_t clock;
  boolean_t highSpeedEnable;
  boolean_t admaMode;
} SMIH_CONFIG_MODES_T;

typedef struct SMIH_ADMA_DESC_TABLE {
  uint16_t attributeValid : 1;
  uint16_t attributeEnd : 1;
  uint16_t attributeInt : 1;
  uint16_t reserved1 : 1;
  uint16_t attributeAct : 2;
  uint16_t reserved2 : 10;
  uint16_t length;
  uint32_t _32BIT_Adress;
} SMIH_ADMA_DESC_TABLE_T;

typedef struct SMIH_CMD_OCR {
  uint32_t reserved1 : 8;
  uint32_t v20_21 : 1;
  uint32_t v21_22 : 1;
  uint32_t v22_23 : 1;
  uint32_t v23_24 : 1;
  uint32_t v24_25 : 1;
  uint32_t v25_26 : 1;
  uint32_t v26_27 : 1;
  uint32_t v27_28 : 1;
  uint32_t v28_29 : 1;
  uint32_t v29_30 : 1;
  uint32_t v30_31 : 1;
  uint32_t v31_32 : 1;
  uint32_t v32_33 : 1;
  uint32_t v33_34 : 1;
  uint32_t v34_35 : 1;
  uint32_t v35_36 : 1;
  uint32_t s18A : 1;
  uint32_t reserved2 : 2;
  uint32_t memPresent : 1;
  uint32_t ioNum : 3;
  uint32_t cardReady : 1;
} SMIH_CMD_OCR_T;
// command transfer function.
typedef error_t (*cmd_transfer_function_t)(SMIH_TRANSFER_T *Transfer);

typedef struct SMIH_CARD_CONFIG {
  uint8_t busWidthMode;
  uint32_t clock;
  boolean_t highSpeedEnable;
  uint8_t uhsModeSelect;
  uint32_t voltage;
  boolean_t admaMode;
  uint16_t byteBlockSize;
  uint16_t numberOfBlocks;
  uint8_t funcNum : 3;
  uint8_t blockModeEnable : 1;
  uint8_t opCode : 1;
  SMIH_CMD_OCR_T ocr;
  uint8_t cardType;
  uint32_t maxSectorNum;
  uint8_t cid[16];
  uint8_t csd[16];
  uint16_t rca;
  uint8_t response[4];
  cmd_transfer_function_t cmd_transfer;

} SMIH_CARD_CONFIG_T;

typedef struct SMIH_CCCR_REG_INFO {
  __IO uint8_t ccidSdioRevReg;
  __IO uint8_t sdSpecRevReg;
  __IO uint8_t ioEnableReg;
  __IO uint8_t ioReady;
  __IO uint8_t intEnable;
  __IO uint8_t intrPending;
  __IO uint8_t ioAbort;
  __IO uint8_t busControl;
  __IO uint8_t cardCapacity;
  __IO uint8_t commonCisPointer;
  __IO uint8_t commonCisPointer1;
  __IO uint8_t commonCisPointer2;
  __IO uint8_t busSped;
  __IO uint8_t funcSelect;
  __IO uint8_t execFlags;
  __IO uint8_t redayFlags;
  __IO uint16_t funcoBlockSize;
  __IO uint8_t powerControl;
  __IO uint8_t busSpeddSelect;
  __IO uint8_t uhs1Support;
  __IO uint8_t driverStrength;
  __IO uint8_t intrExtension;
} SMIH_CCCR_REG_INFO_T;

typedef struct SD_CARD_STATUS_REG {
  uint32_t reserved1 : 3;
  uint32_t sequenceEerror : 1;
  uint32_t reserved2 : 1;
  uint32_t applicationCmd : 1;
  uint32_t reserved3 : 2;
  uint32_t readyForData : 1;
  uint32_t currentStatus : 4;
  uint32_t eraseReset : 1;
  uint32_t cardEccDisable : 1;
  uint32_t wpEraseSkip : 1;
  uint32_t csdOverwrite : 1;
  uint32_t reserved4 : 2;
  uint32_t error : 1;
  uint32_t ccError : 1;
  uint32_t cardEccFail : 1;
  uint32_t illegalCmd : 1;
  uint32_t commandCrcError : 1;
  uint32_t lockUnlockFail : 1;
  uint32_t cradIsLOcked : 1;
  uint32_t wpViolation : 1;
  uint32_t erasePram : 1;
  uint32_t eraseSeqError : 1;
  uint32_t blockLenghtError : 1;
  uint32_t addressError : 1;
  uint32_t outOfRange : 1;
} SD_CARD_STATUS_REG_T;

typedef struct CSD_REG_VERSION1 {
  uint8_t reserved1 : 2;
  uint8_t fileFormat : 2;
  uint8_t temporaryWriteProtect : 1;
  uint8_t permanantWriteProtect : 1;
  uint8_t copy : 1;
  uint8_t fileFormatGroup : 1;
  uint8_t reserved2 : 5;
  uint8_t writeBlockLengthPartial : 1;
  uint8_t writeBlockLength : 4;
  uint8_t R2WFactor : 3;
  uint8_t reserved3 : 2;
  uint8_t wpGrpEnable : 1;
  uint8_t wpGrpSize : 7;
  uint8_t sectorSize : 7;
  uint8_t eraseBlockEn : 1;
  uint8_t deviceSizeMultiplier1 : 1;
  uint8_t deviceSizeMultiplier2 : 2;
  uint8_t vddWCrrMax : 3;
  uint8_t vddWcurrMin : 3;
  uint8_t vddRcurrMax : 3;
  uint8_t vddRcurrmin : 3;
  uint8_t deviceSize1 : 2;
  uint8_t deviceSize2 : 8;
  uint8_t deviceSize3 : 2;
  uint8_t reserved4 : 2;
  uint8_t dsrIMP : 1;
  uint8_t readBlockMisallign : 1;
  uint8_t writeBlockMisallign : 1;
  uint8_t readBlockPartial : 1;
  uint8_t readBlockLength : 4;
  uint8_t ccc1 : 4;
  uint8_t ccc2 : 8;
  uint8_t transferSpeed : 8;
  uint8_t nsac : 8;
  uint8_t taac : 8;
  uint8_t RESERVE5 : 6;
  uint8_t csdStructure : 2;
} CSD_REG_VERSION1_T;

typedef struct CSD_REG_VERSION2 {
  uint8_t RESERVED1 : 2;
  uint8_t fileFormat : 2;
  uint8_t temporaryWriteProtect : 1;
  uint8_t permanantWriteProtect : 1;
  uint8_t copy : 1;
  uint8_t fileFormatGroup : 1;
  uint8_t reserved2 : 5;
  uint8_t writeBlockLengthPartial : 1;
  uint8_t writeBlockLength : 4;
  uint8_t R2WFactor : 3;
  uint8_t reserved3 : 2;
  uint8_t wpGrpEnable : 1;
  uint8_t wpGrpSize : 7;
  uint8_t sectorSize : 7;
  uint8_t eraseBlockEnable : 1;
  uint8_t reserved4 : 1;
  uint8_t deviceSize1 : 8;
  uint8_t deviceSize2 : 8;
  uint8_t deviceSize3 : 6;
  uint8_t reserved5 : 2;
  uint8_t reserved6 : 4;
  uint8_t dsrIMP : 1;
  uint8_t readBlockMisallign : 1;
  uint8_t writeBlockMisallign : 1;
  uint8_t readBlockPartial : 1;
  uint8_t readBlockLength : 4;
  uint8_t ccc1 : 4;
  uint8_t ccc2 : 8;
  uint8_t transferSpeed : 8;
  uint8_t nsac : 8;
  uint8_t taac : 8;
  uint8_t reserved7 : 6;
  uint8_t csdStructure : 2;
} CSD_REG_VERSION2_T;

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

error_t RSI_SD_HostInit(SMIH_CARD_CONFIG_T *pSmihConfig, ARM_SMIH_SignalEvent_t Event, uint8_t MemType);
error_t Smih_DeInitialization(void);
error_t smih_bus_width_set(uint8_t BusWidthMode);
error_t smih_bus_voltage_select(uint8_t enVoltage);
error_t smih_send_command(SMIH_COMMAND_FRAME_CONFIG_T *pConfig);
error_t smih_get_response(uint16_t *pResponseData, uint8_t ResponseRegCount);
error_t smih_stop_at_block_gap(void);
error_t smih_transfer_restart(void);
void smih_18v_signal_enable(void);
error_t smih_uhs_mode_select(uint8_t UhsMode);
void smih_irq_handler(void);
error_t smih_modes_configuration(SMIH_CONFIG_MODES_T *pSmihConfig);
error_t smih_clock_config(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t freq);
error_t smih_check_for_error_interrupt(void);
error_t smih_send_data(SMIH_TRANSFER_T *pTransfer);
error_t smih_receive_data(SMIH_TRANSFER_T *pTransfer);
error_t smih_memory_command_transfer(SMIH_TRANSFER_T *pTransfer);
error_t smih_io_command_transfer(SMIH_TRANSFER_T *pTransfer);
void RegisterCallBack(ARM_SMIH_SignalEvent_t Event);

#ifdef __cplusplus
}
#endif

#endif // RSI_SMIH_H
