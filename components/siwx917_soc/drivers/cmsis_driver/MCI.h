/* -----------------------------------------------------------------------------
 * Copyright (c) 2013-2014 ARM Ltd.
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software. Permission is granted to anyone to use this
 * software for any purpose, including commercial applications, and to alter
 * it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *
 * $Date:        24. Nov 2014
 * $Revision:    V2.02
 *
 * Project:      MCI (Memory Card Interface)
 *               Driver definitions
 * -------------------------------------------------------------------------- */
/*
 *  Version 1.00
 *    Initial release
 */

#include "Driver_MCI.h"
#include "rsi_chip.h"
#include "RTE_Device.h"
#include "RTE_Components.h"

typedef void (*ARM_MCI_SignalEvent_t)(uint32_t event); ///< Pointer to \ref ARM_MCI_SignalEvent : Signal MCI Event.

/* MCI Driver State Definition */
typedef struct MCI_Ctrl {
  ARM_MCI_SignalEvent_t cb_event; /* Driver event callback function     */
  ARM_MCI_STATUS status;          /* Driver status                      */
  uint32_t *response;             /* Pointer to response buffer         */
  uint32_t bl_sz;
  uint32_t bl_cnt;
  uint8_t volatile flags; /* Driver state flags                 */
} MCI_CTRL;

/* Driver flag definitions */
#define MCI_INIT      ((uint8_t)0x01) /* MCI initialized         */
#define MCI_POWER     ((uint8_t)0x02) /* MCI powered on          */
#define MCI_SETUP     ((uint8_t)0x04) /* MCI configured          */
#define MCI_RESP_LONG ((uint8_t)0x08) /* Long response expected  */

#define MCI_DATA_READ ((uint8_t)0x10) /* Read transfer             */
#define MCI_DATA_MULB ((uint8_t)0x20) /* Multiple block transfer   */

/* Normal interrupt status enable reg */
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

/* Error interrupt status enables */
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

/* Normal interrupt status enable reg */
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

typedef struct MCI_COMMAND_FRAME_CONFIG {
  uint8_t cmdIndex;
  uint32_t argument;
  uint8_t cmdType;
  boolean_t dataPresent;
  boolean_t checkCmdIndex;
  boolean_t checkCmdCrc;
  uint8_t responseType;
  uint8_t autoCmdType;
} MCI_COMMAND_FRAME_CONFIG_T;
typedef struct MCI_ADMA_DESC_TABLE {
  uint16_t attributeValid : 1;
  uint16_t attributeEnd : 1;
  uint16_t attributeInt : 1;
  uint16_t reserved1 : 1;
  uint16_t attributeAct : 2;
  uint16_t reserved2 : 10;
  uint16_t length;
  uint32_t _32BIT_Adress;
} MCI_ADMA_DESC_TABLE_T;

/*Command type defines */
#define MCI_ABORT_CMD   3
#define MCI_RESUME_CMD  2
#define MCI_SUSPEND_CMD 1
#define MCI_NORMAL_CMD  0

#define MCI_DISABLE_AUTO_CMD  0
#define MCI_ENABLE_AUTO_CMD12 1
#define MCI_ENABLE_AUTO_CMD23 2

/*response type defines*/
#define MCI_RESPONSE_NONE             0
#define MCI_RESPONSE_136BIT           1
#define MCI_RESPONSE_48BIT            2
#define MCI_RESPONSE_48BIT_BUSY_CHECK 3

/*data direction defines*/
#define MCI_WRITE_DIRECTION 0x0
#define MCI_READ_DIRECTION  0x1

#define MCI_RESPONSE_EXPECTED (ARM_MCI_RESPONSE_SHORT | ARM_MCI_RESPONSE_SHORT_BUSY | ARM_MCI_RESPONSE_LONG)

/* MCI data configuration structure*/
typedef struct MCI_DATA_CONFIG {
  boolean_t multiBlock;
  boolean_t dataTransferDirection;
  uint16_t blockSize;
  uint16_t blockCount;
  boolean_t blockCountEnable;
  boolean_t dmaEnable;
  uint32_t admaDespTableAddress;
  uint8_t dataTimeout;
} MCI_DATA_CONFIG_T;

/* MCI command reg structure*/
typedef struct MCI_COMMAND_REG {
  uint16_t respType : 2;
  uint16_t resrvd : 1;
  uint16_t cmdCrcCheckEnable : 1;
  uint16_t cmdIndexCheckEnable : 1;
  uint16_t dataPresentSelect : 1;
  uint16_t cmdType : 2;
  uint16_t cmdIndex : 6;
} MCI_COMMAND_REG_T;

#define COMMANDS_INTERRUPTS                                                                                      \
  (((COMMAND_TIMEOUT_ERROR_STATUS_ENABLE | COMMAND_CRC_ERROR_STATUS_ENABLE | COMMAND_END_BIT_ERROR_STATUS_ENABLE \
     | COMMAND_INDEX_ERROR_STATUS_ENABLE | AUTO_CMD_ERROR_STATUS_ENABLE)                                         \
    << 16)                                                                                                       \
   | COMMAND_COMPLETE_STATUS_ENABLE)

#define DATA_INTERRUPTS                                                                                         \
  (((DATA_TIMEOUT_ERROR_STATUS_ENABLE | DATA_CRC_ERROR_STATUS_ENABLE | DATA_END_BIT_ERROR_STATUS_ENABLE) << 16) \
   | TRANSFER_COMPLETE_STATUS_ENABLE | BUFFER_READ_READY_STATUS_ENABLE | BUFFER_WRITE_READY_STATUS_ENABLE       \
   | DMA_INTERRUPT_STATUS_ENABLE)

#define SDMEM_INTR_STATUS_REG (*(uint32_t *)(0x20220000 + 0x30))
#define SDMEM_RESPONSE_REG    ((*(uint32_t *)(0x20220000 + 0x10)))

/* Function Prototypes */
void RSI_MCI_ClockCnfig(uint32_t freq);
error_t RSI_MCI_SendCommand(MCI_COMMAND_FRAME_CONFIG_T *pConfig);
error_t RSI_MCI_DataTransferInitialization(MCI_DATA_CONFIG_T *pDataConfig);
