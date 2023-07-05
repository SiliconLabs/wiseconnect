/* --------------------------------------------------------------------------
 * Copyright (c) 2013-2016 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * $Date:        02. March 2016
 * $Revision:    V1.1
 *
 * Driver:       Driver_MCI0
 * Configured:   via RTE_Device.h configuration file
 * Project:      MCI Driver for RS1xxxx
 */

/* History:
 * Version 1.0
 * Initial release
 */
#include "rsi_ccp_user_config.h"
#if defined(CHIP_9118)
#include "MCI.h"

#define SDHC_IRQHandler     IRQ068_Handler
#define ARM_MCI_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1, 2) /* driver version */
static MCI_CTRL MCI;

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = { ARM_MCI_API_VERSION, ARM_MCI_DRV_VERSION };

/* Driver Capabilities */
static const ARM_MCI_CAPABILITIES DriverCapabilities = {
  RTE_MCI_CD_PIN,      /* cd_state          */
  0,                   /* cd_event          */
  RTE_MCI_WP_PIN,      /* wp_state          */
  0,                   /* vdd               */
  0,                   /* vdd_1v8           */
  0,                   /* vccq              */
  0,                   /* vccq_1v8          */
  0,                   /* vccq_1v2          */
  RTE_MCI_BUS_WIDTH_4, /* data_width_4      */
  RTE_MCI_BUS_WIDTH_8, /* data_width_8      */
  0,                   /* data_width_4_ddr  */
  0,                   /* data_width_8_ddr  */
  1,                   /* high_speed        */
  0,                   /* uhs_signaling     */
  0,                   /* uhs_tuning        */
  0,                   /* uhs_sdr50         */
  0,                   /* uhs_sdr104        */
  0,                   /* uhs_ddr50         */
  0,                   /* uhs_driver_type_a */
  0,                   /* uhs_driver_type_c */
  0,                   /* uhs_driver_type_d */
  0,                   /* sdio_interrupt    */
  0,                   /* read_wait         */
  0,                   /* suspend_resume    */
  0,                   /* mmc_interrupt     */
  0,                   /* mmc_boot          */
  0,                   /* rst_n             */
  0,                   /* ccs               */
  0                    /* ccs_timeout       */
};

/*********functions************/
static MCI_ADMA_DESC_TABLE_T Adma2DescriptorTable[2] = { 0 };

/**
 * @fn           ARM_DRIVER_VERSION  MCI_GetVersion(void)
 * @brief        Get Driver Version.
 * @param[in]    none
 * @return       ARM DRIVER VERSION
 */
static ARM_DRIVER_VERSION MCI_GetVersion(void)
{
  return DriverVersion;
}

/**
 * @fn           ARM_MCI_CAPABILITIES  MCI_GetCapabilities(void)
 * @brief        Get Driver capabilities.
 * @param[in]    none
 * @return 	 ARM_MCI_CAPABILITIES  
 */
static ARM_MCI_CAPABILITIES MCI_GetCapabilities(void)
{
  return DriverCapabilities;
}

/**
 * @fn           int32_t MCI_Initialize(ARM_MCI_SignalEvent_t cb_event)
 * @brief        Initialize the Memory Card Interface.
 * @param[in]    cb_event  Pointer to the ARM_MCI_SignalEvent
 * @return 	 excecution status  
 */
static int32_t MCI_Initialize(ARM_MCI_SignalEvent_t cb_event)
{
  if (MCI.flags & MCI_INIT) {
    return ARM_DRIVER_OK;
  }

  MCI.cb_event = cb_event;

  /* Clear status */
  MCI.status.command_active   = 0U;
  MCI.status.command_timeout  = 0U;
  MCI.status.command_error    = 0U;
  MCI.status.transfer_active  = 0U;
  MCI.status.transfer_timeout = 0U;
  MCI.status.transfer_error   = 0U;
  MCI.status.sdio_interrupt   = 0U;
  MCI.status.ccs              = 0U;

  /*Configure clock gpio pin*/
  if (RTE_MCI_CLK_PIN == 25) {
    RSI_EGPIO_HostPadsGpioModeEnable(25);
  }
  RSI_EGPIO_PadSelectionEnable(RTE_MCI_CLOCK_PAD);
  RSI_EGPIO_PadReceiverEnable(RTE_MCI_CLK_PIN);
  RSI_EGPIO_SetPinMux(EGPIO, RTE_MCI_CLOCK_PORT, RTE_MCI_CLK_PIN, EGPIO_PIN_MUX_MODE8);

  if (RTE_MCI_CD_PIN) {
    /*Configure cd gpio pin*/
    RSI_EGPIO_PadSelectionEnable(RTE_MCI_CDD_PAD);
    RSI_EGPIO_PadReceiverEnable(RTE_MCI_CDD_PIN);
    RSI_EGPIO_PadDriverDisableState(RTE_MCI_CDD_PIN, Pulldown);
    RSI_EGPIO_SetPinMux(EGPIO, RTE_MCI_CD_PORT, RTE_MCI_CDD_PIN, EGPIO_PIN_MUX_MODE8);
  }
  if (RTE_MCI_WP_PIN) {
    /*Configure write protect gpio pin*/
    RSI_EGPIO_PadSelectionEnable(RTE_MCI_WPP_PAD);
    RSI_EGPIO_PadReceiverEnable(RTE_MCI_WPP_PIN);
    RSI_EGPIO_PadDriverDisableState(RTE_MCI_WPP_PIN, Pulldown);
    RSI_EGPIO_SetPinMux(EGPIO, RTE_MCI_WP_PORT, RTE_MCI_WPP_PIN, EGPIO_PIN_MUX_MODE8);
  }

  /*Configure command gpio pin*/
  if (RTE_MCI_CMD_PIN == 26) {
    RSI_EGPIO_HostPadsGpioModeEnable(26);
  }
  RSI_EGPIO_PadSelectionEnable(RTE_MCI_CMD_PAD);
  RSI_EGPIO_PadReceiverEnable(RTE_MCI_CMD_PIN);
  RSI_EGPIO_PadDriverDisableState(RTE_MCI_CMD_PIN, Pullup);
  RSI_EGPIO_SetPinMux(EGPIO, RTE_MCI_CMD_PORT, RTE_MCI_CMD_PIN, EGPIO_PIN_MUX_MODE8);

  /*Configure data0 gpio pin*/
  if (RTE_MCI_DATA0_PIN == 27) {
    RSI_EGPIO_HostPadsGpioModeEnable(27);
  }
  RSI_EGPIO_PadSelectionEnable(RTE_MCI_DATA0_PAD);
  RSI_EGPIO_PadReceiverEnable(RTE_MCI_DATA0_PIN);
  RSI_EGPIO_PadDriverDisableState(RTE_MCI_DATA0_PIN, Pullup);
  RSI_EGPIO_SetPinMux(EGPIO, RTE_MCI_DATA0_PORT, RTE_MCI_DATA0_PIN, EGPIO_PIN_MUX_MODE8);

#if (RTE_SDMMC_BUS_WIDTH_4) /* SD_DAT[3..1] */
                            /*Configure data1 gpio pin*/
  if (RTE_MCI_DATA1_PIN == 28) {
    RSI_EGPIO_HostPadsGpioModeEnable(28);
  }
  RSI_EGPIO_PadSelectionEnable(RTE_MCI_DATA1_PAD);
  RSI_EGPIO_PadReceiverEnable(RTE_MCI_DATA1_PIN);
  RSI_EGPIO_PadDriverDisableState(RTE_MCI_DATA1_PIN, Pullup);
  RSI_EGPIO_SetPinMux(EGPIO, RTE_MCI_DATA1_PORT, RTE_MCI_DATA1_PIN, EGPIO_PIN_MUX_MODE8);

  /*Configure data2 gpio pin*/
  if (RTE_MCI_DATA2_PIN == 29) {
    RSI_EGPIO_HostPadsGpioModeEnable(29);
  }
  RSI_EGPIO_PadSelectionEnable(RTE_MCI_DATA2_PAD);
  RSI_EGPIO_PadReceiverEnable(RTE_MCI_DATA2_PIN);
  RSI_EGPIO_PadDriverDisableState(RTE_MCI_DATA2_PIN, Pullup);
  RSI_EGPIO_SetPinMux(EGPIO, RTE_MCI_DATA2_PORT, RTE_MCI_DATA2_PIN, EGPIO_PIN_MUX_MODE8);

  /*Configure data3 gpio pin*/
  if (RTE_MCI_DATA3_PIN == 30) {
    RSI_EGPIO_HostPadsGpioModeEnable(30);
  }
  RSI_EGPIO_PadSelectionEnable(RTE_MCI_DATA3_PAD);
  RSI_EGPIO_PadReceiverEnable(RTE_MCI_DATA3_PIN);
  RSI_EGPIO_PadDriverDisableState(RTE_MCI_DATA3_PIN, Pullup);
  RSI_EGPIO_SetPinMux(EGPIO, RTE_MCI_DATA3_PORT, RTE_MCI_DATA3_PIN, EGPIO_PIN_MUX_MODE8);

#if (RTE_SDMMC_BUS_WIDTH_8) /* RTE_SDMMC_BUS_WIDTH_8 */

  /*Configure data4 gpio pin*/
  RSI_EGPIO_PadSelectionEnable(RTE_MCI_DATA4_PAD);
  RSI_EGPIO_PadReceiverEnable(RTE_MCI_DATA4_PIN);
  RSI_EGPIO_PadDriverDisableState(RTE_MCI_DATA4_PIN, Pullup);
  RSI_EGPIO_SetPinMux(EGPIO, RTE_MCI_DATA4_PORT, RTE_MCI_DATA4_PIN, RTE_MCI_DATA6_MODE);

  /*Configure data5 gpio pin*/
  RSI_EGPIO_PadSelectionEnable(RTE_MCI_DATA5_PAD);
  RSI_EGPIO_PadReceiverEnable(RTE_MCI_DATA5_PIN);
  RSI_EGPIO_PadDriverDisableState(RTE_MCI_DATA5_PIN, Pullup);
  RSI_EGPIO_SetPinMux(EGPIO, RTE_MCI_DATA5_PORT, RTE_MCI_DATA5_PIN, RTE_MCI_DATA5_MODE);

  /*Configure data6 gpio pin*/
  RSI_EGPIO_PadSelectionEnable(RTE_MCI_DATA6_PAD);
  RSI_EGPIO_PadReceiverEnable(RTE_MCI_DATA6_PIN);
  RSI_EGPIO_PadDriverDisableState(RTE_MCI_DATA6_PIN, Pullup);
  RSI_EGPIO_SetPinMux(EGPIO, RTE_MCI_DATA6_PORT, RTE_MCI_DATA6_PIN, RTE_MCI_DATA6_MODE);

#endif /* RTE_SDMMC_BUS_WIDTH_8 */
#endif /* RTE_SDMMC_BUS_WIDTH_4 */

  MCI.flags = MCI_INIT;

  return ARM_DRIVER_OK;
}

/**
 * @fn           int32_t MCI_UnInitialize(ARM_MCI_SignalEvent_t cb_event)
 * @brief        DeInitialize the Memory Card Interface.
 * @param[in]    none
 * @return 	 excecution status  
 */
int32_t MCI_Uninitialize(void)
{
  MCI.flags = 0;

  /*Disable sdmem clock */
  SMIH->SMIH_CLOCK_CONTROL_REGISTER_b.SD_CLOCK_ENABLE = 0u;

  return ARM_DRIVER_OK;
}
boolean_t sdioh_wait_for_card_insert()
{
  /* Wait until card is stable */
  while (SMIH->SMIH_PRESENT_STATE_REGISTER_b.CARD_STATE_STABLE != 0x1)
    ;
  return (boolean_t)SMIH->SMIH_PRESENT_STATE_REGISTER_b.CARD_INSERTED;
}

/**
 * @fn           int32_t  MCI_PowerControl(ARM_POWER_STATE state)
 * @brief        Control Memory Card Interface Power.
 * @param[in]    state  Power state
 * @return 	 excecution status  
 */
int32_t MCI_PowerControl(ARM_POWER_STATE state)
{
  switch (state) {
    case ARM_POWER_OFF:
      /* Disable SDHC interrupt in NVIC */
      NVIC_DisableIRQ(SDMEM_IRQn);

      /* Clear flags */
      MCI.flags = MCI_POWER;

      /* Clear status */
      MCI.status.command_active   = 0U;
      MCI.status.command_timeout  = 0U;
      MCI.status.command_error    = 0U;
      MCI.status.transfer_active  = 0U;
      MCI.status.transfer_timeout = 0U;
      MCI.status.transfer_error   = 0U;
      MCI.status.sdio_interrupt   = 0U;
      MCI.status.ccs              = 0U;
      break;

    case ARM_POWER_FULL:
      if ((MCI.flags & MCI_POWER) == 0) {
        /* Clear response variable */
        MCI.response = NULL;

        /* sleepclock prog */
        *(volatile uint32_t *)(0x46000024) = (0x0 << 21);

        /* wait for clock switch */
        while ((M4CLK->PLL_STAT_REG_b.SLEEP_CLK_SWITCHED) != 1)
          ;

        /* Wait for card insert */
        while (sdioh_wait_for_card_insert() == 0)
          ;

        /* enable adma*/
        SMIH->SMIH_HOST_CONTROL_1_REGISTER_b.DMA_SELECT = 0x2;

        /* Enable normal interrupts*/
        SMIH->SMIH_NORMAL_INTERRUPT_STATUS_ENABLE_REGISTER =
          (COMMAND_COMPLETE_STATUS_ENABLE | TRANSFER_COMPLETE_STATUS_ENABLE | BLOCK_GAP_EVENT_STATUS_ENABLE
           | DMA_INTERRUPT_STATUS_ENABLE | BUFFER_WRITE_READY_STATUS_ENABLE | BUFFER_READ_READY_STATUS_ENABLE
           | CARD_INSERTION_STATUS_ENABLE | CARD_REMOVAL_STATUS_ENABLE | CARD_INTERRUPT_STATUS_ENABLE
           | INT_A_STATUS_ENABLE | INT_B_STATUS_ENABLE | INT_C_STATUS_ENABLE | RE_TUNING_EVENT_STATUS_ENABLE);

        /* Enable error interrupts*/
        SMIH->SMIH_ERROR_INTERRUPT_STATUS_ENABLE_REGISTER =
          (COMMAND_TIMEOUT_ERROR_STATUS_ENABLE | COMMAND_CRC_ERROR_STATUS_ENABLE | COMMAND_END_BIT_ERROR_STATUS_ENABLE
           | COMMAND_INDEX_ERROR_STATUS_ENABLE | DATA_TIMEOUT_ERROR_STATUS_ENABLE | DATA_CRC_ERROR_STATUS_ENABLE
           | DATA_END_BIT_ERROR_STATUS_ENABLE | CURRENT_LIMIT_ERROR_STATUS_ENABLE | AUTO_CMD_ERROR_STATUS_ENABLE
           | ADMA_ERROR_STATUS_ENABLE | TUNING_ERROR_STATUS_ENABLE);

        /* Enable normal interrupts signals*/
        SMIH->SMIH_NORMAL_INTERRUPT_SIGNAL_ENABLE_REGISTER =
          (COMMAND_COMPLETE_SIGNAL_ENABLE | TRANSFER_COMPLETE_SIGNAL_ENABLE | BUFFER_WRITE_READY_SIGNAL_ENABLE
           | BUFFER_READ_READY_SIGNAL_ENABLE | CARD_REMOVAL_SIGNAL_ENABLE);

        SMIH->SMIH_ERROR_INTERRUPT_SIGNAL_ENABLE_REGISTER = 0x1;

        /* select voltage to 3.3v*/
        SMIH->SMIH_POWER_CONTROL_REGISTER_b.SD_BUS_VOLTAGE_SELECT = 7U;

        /*power on bus*/
        SMIH->SMIH_POWER_CONTROL_REGISTER_b.SD_BUS_POWER = 0x1;

        /*configure data timeout counter value*/
        SMIH->SMIH_TIMEOUT_CONTROL_REGISTER_b.DATA_TIMEOUT_COUNTER_VALUE = 0xE;

        /* NVIC Enable*/
        NVIC_ClearPendingIRQ(SDMEM_IRQn);
        NVIC_EnableIRQ(SDMEM_IRQn);

        MCI.flags |= MCI_POWER;
      }
      break;
    case ARM_POWER_LOW:
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  return ARM_DRIVER_OK;
}

/**
 * @fn           int32_t  MCI_CardPower(uint32_t voltage)
 * @brief        Set Memory Card Power supply voltage.
 * @param[in]    voltage  : Memory Card Power supply voltage
 * @return 	 excecution status  
 */
int32_t MCI_CardPower(uint32_t voltage)
{
  if ((MCI.flags & MCI_POWER) == 0U) {
    return ARM_DRIVER_ERROR;
  }
  return ARM_DRIVER_ERROR_UNSUPPORTED;
}

/**
 * @fn           int32_t  MCI_ReadCD(void)
 * @brief        Read Card Detect (CD) state.
 * @param[in]    none
 * @return 	 excecution status  
 */
int32_t MCI_ReadCD(void)
{
  if (RTE_MCI_CD_PIN != 0) {
    if (MCI.flags & MCI_POWER) {
      if ((RSI_EGPIO_GetPin(EGPIO, RTE_MCI_CD_PORT, RTE_MCI_CDD_PIN) == 0)) {
        return (1);
      }
    }
  }
  return (0);
}

/**
 * @fn           int32_t  MCI_ReadWP(void)
 * @brief        Read Write Protect (WP) state.
 * @param[in]    none
 * @return 	 excecution status  
 */
int32_t MCI_ReadWP(void)
{
  if (RTE_MCI_WP_PIN != 0) {
    if (MCI.flags & MCI_POWER) {
      if ((RSI_EGPIO_GetPin(EGPIO, RTE_MCI_WP_PORT, RTE_MCI_WPP_PIN) == 0)) {
        return (1);
      }
    }
  }
  return (0);
}

/**
 * @fn           int32_t  MCI_SendCommand(uint32_t cmd, uint32_t arg, uint32_t flags, uint32_t *response)
 * @brief        Send Command to card and get the response.
 * @param[in]    cmd  : Memory Card command
 * @param[in]    arg  :	Command argument
 * @param[in]    flags : Command flags
 * @param[in]    response : Pointer to buffer for response
 * @return 	 excecution status  
 */
int32_t MCI_SendCommand(uint32_t cmd, uint32_t arg, uint32_t flags, uint32_t *response)
{
  MCI_COMMAND_FRAME_CONFIG_T CommandCfg = { 0 };
  MCI_DATA_CONFIG_T DataCfg             = { 0 };
  uint32_t i                            = 0;
  if (((flags & MCI_RESPONSE_EXPECTED) != 0U) && (response == NULL)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }
  if ((MCI.flags & MCI_SETUP) == 0U) {
    return ARM_DRIVER_ERROR;
  }
  if (MCI.status.command_active) {
    return ARM_DRIVER_ERROR_BUSY;
  }
  /* wait for data line free */
  if (flags & ARM_MCI_TRANSFER_DATA) {

    while (SMIH->SMIH_PRESENT_STATE_REGISTER_b.COMMAND_INHIBIT_DAT == 1)
      ;
  }
  while (SMIH->SMIH_PRESENT_STATE_REGISTER_b.COMMAND_INHIBIT_CMD == 1)
    ;

  MCI.status.command_active   = 1U;
  MCI.status.command_timeout  = 0U;
  MCI.status.command_error    = 0U;
  MCI.status.transfer_timeout = 0U;
  MCI.status.transfer_error   = 0U;
  MCI.status.ccs              = 0U;

  if (flags & ARM_MCI_CARD_INITIALIZE) {
    /* Configure sdmem internal clock */
    RSI_MCI_ClockCnfig(400000);

    /*wait for some time*/
    for (i = 0; i < 20000; i++)
      ;
  }
  CommandCfg.argument = arg;
  CommandCfg.cmdIndex = cmd & 0xFF;
  DataCfg.blockSize   = MCI.bl_sz;
  DataCfg.blockCount  = MCI.bl_cnt;

  if (CommandCfg.cmdIndex == 0x0C) {
    //abort command
    CommandCfg.cmdType = 3;
  }
  MCI.response = response;
  MCI.flags &= ~MCI_RESP_LONG;

  switch (flags & ARM_MCI_RESPONSE_Msk) {
    case ARM_MCI_RESPONSE_NONE:
      /* No response expected */
      break;
    case ARM_MCI_RESPONSE_SHORT:
      /* Short response expected */
      CommandCfg.responseType = MCI_RESPONSE_48BIT;
      break;
    case ARM_MCI_RESPONSE_SHORT_BUSY:
      /* Short response with busy expected */
      CommandCfg.responseType = MCI_RESPONSE_48BIT_BUSY_CHECK;
      break;
    case ARM_MCI_RESPONSE_LONG:
      MCI.flags |= MCI_RESP_LONG;
      /* Long response expected */
      CommandCfg.responseType = MCI_RESPONSE_136BIT;
      break;
    default:
      return ARM_DRIVER_ERROR;
  }
  if (flags & ARM_MCI_RESPONSE_INDEX) {
    /* Check for command index error */
    CommandCfg.checkCmdIndex = 1;
  }
  if (flags & ARM_MCI_RESPONSE_CRC) {
    /* Check for CRC error */
    CommandCfg.checkCmdCrc = 1;
  }

  DataCfg.admaDespTableAddress = (uint32_t)&Adma2DescriptorTable[0];
  if (flags & ARM_MCI_TRANSFER_DATA) {
    /* Enable data transfer */
    CommandCfg.dataPresent = 1;
    if (MCI.flags & MCI_DATA_READ) {
      /* Read transfer */
      DataCfg.dataTransferDirection = MCI_READ_DIRECTION;
    }
    if (MCI.flags & MCI_DATA_MULB) {
      /* Multiple block transfer */
      DataCfg.blockCountEnable = 1;
      DataCfg.multiBlock       = 1;
    }
    MCI.status.transfer_active = 1U;
  }
  /* initialize data transfer */
  RSI_MCI_DataTransferInitialization(&DataCfg);

  /* send command */
  RSI_MCI_SendCommand(&CommandCfg);

  return ARM_DRIVER_OK;
}

/**
 * @fn           int32_t  MCI_SetupTransfer(uint8_t  *data, uint32_t block_count, uint32_t block_size, uint32_t mode)
 * @brief        Setup read or write transfer operation.
 * @param[in]    data        : Pointer to data block(s) to be written or read
 * @param[in]    block_count : Number of blocks
 * @param[in]    block_size  : 	Size of a block in bytes
 * @param[in]    mode :  Transfer mode
 * @return 	 excecution status  
 */
int32_t MCI_SetupTransfer(uint8_t *data, uint32_t block_count, uint32_t block_size, uint32_t mode)
{
  if ((data == NULL) || (block_count == 0U) || (block_size == 0U)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }
  if ((MCI.flags & MCI_SETUP) == 0U) {
    return ARM_DRIVER_ERROR;
  }
  if (MCI.status.transfer_active) {
    return ARM_DRIVER_ERROR_BUSY;
  }
  if (mode & ARM_MCI_TRANSFER_STREAM) {
    /* Stream or SDIO multibyte data transfer not supported by peripheral */
    return ARM_DRIVER_ERROR;
  }

  SMIH->TRANSFER_MODE_REGISTER_b.DMA_ENABLE = 0x1;

  memset(Adma2DescriptorTable, 0x0, sizeof(Adma2DescriptorTable));

  /*Fill adma descriptor table*/
  Adma2DescriptorTable[0].attributeValid = 1;
  Adma2DescriptorTable[0].attributeEnd   = 1;
  Adma2DescriptorTable[0].attributeInt   = 0;
  Adma2DescriptorTable[0].attributeAct   = 2;
  Adma2DescriptorTable[0].length         = (block_size * block_count);
  Adma2DescriptorTable[0]._32BIT_Adress  = (uint32_t)data;

  /* Set transfer block count and size */
  MCI.bl_cnt = block_count;
  MCI.bl_sz  = block_size;

  if (block_count == 1) {
    /* Single block transfer */
    MCI.flags &= ~MCI_DATA_MULB;
  } else {
    /* Multiple block transfer */
    MCI.flags |= MCI_DATA_MULB;
  }

  if (mode & ARM_MCI_TRANSFER_WRITE) {
    /* Direction: From controller to card */
    MCI.flags &= ~MCI_DATA_READ;
  } else {
    /* Direction: From card to controller */
    MCI.flags |= MCI_DATA_READ;
  }
  while (SMIH->SMIH_PRESENT_STATE_REGISTER_b.COMMAND_INHIBIT_DAT == 0x1)
    ;
  return (ARM_DRIVER_OK);
}

/**
 * @fn           int32_t  MCI_AbortTransfer(void)
 * @brief        Abort current read/write data transfer.
 * @param[in]    none
 * @return 	 excecution status  
 */
int32_t MCI_AbortTransfer(void)
{
  int32_t status;
  if ((MCI.flags & MCI_SETUP) == 0U) {
    return ARM_DRIVER_ERROR;
  }
  status = ARM_DRIVER_OK;

  /* Disable normal interrupts*/
  SMIH->SMIH_NORMAL_INTERRUPT_STATUS_ENABLE_REGISTER &=
    ~(COMMAND_COMPLETE_STATUS_ENABLE | TRANSFER_COMPLETE_STATUS_ENABLE | BLOCK_GAP_EVENT_STATUS_ENABLE
      | DMA_INTERRUPT_STATUS_ENABLE | BUFFER_WRITE_READY_STATUS_ENABLE | BUFFER_READ_READY_STATUS_ENABLE
      | CARD_INSERTION_STATUS_ENABLE | CARD_REMOVAL_STATUS_ENABLE | CARD_INTERRUPT_STATUS_ENABLE | INT_A_STATUS_ENABLE
      | INT_B_STATUS_ENABLE | INT_C_STATUS_ENABLE | RE_TUNING_EVENT_STATUS_ENABLE);

  /* Disable error interrupts*/
  SMIH->SMIH_ERROR_INTERRUPT_STATUS_ENABLE_REGISTER &=
    ~(COMMAND_TIMEOUT_ERROR_STATUS_ENABLE | COMMAND_CRC_ERROR_STATUS_ENABLE | COMMAND_END_BIT_ERROR_STATUS_ENABLE
      | COMMAND_INDEX_ERROR_STATUS_ENABLE | DATA_TIMEOUT_ERROR_STATUS_ENABLE | DATA_CRC_ERROR_STATUS_ENABLE
      | DATA_END_BIT_ERROR_STATUS_ENABLE | CURRENT_LIMIT_ERROR_STATUS_ENABLE | AUTO_CMD_ERROR_STATUS_ENABLE
      | ADMA_ERROR_STATUS_ENABLE | TUNING_ERROR_STATUS_ENABLE);

  /* Disable normal interrupts signals*/
  SMIH->SMIH_NORMAL_INTERRUPT_SIGNAL_ENABLE_REGISTER &=
    ~(COMMAND_COMPLETE_SIGNAL_ENABLE | TRANSFER_COMPLETE_SIGNAL_ENABLE | BUFFER_WRITE_READY_SIGNAL_ENABLE
      | BUFFER_READ_READY_SIGNAL_ENABLE | CARD_REMOVAL_SIGNAL_ENABLE);
  SMIH->SMIH_ERROR_INTERRUPT_SIGNAL_ENABLE_REGISTER = 0x0;

  MCI.status.command_active  = 0U;
  MCI.status.transfer_active = 0U;
  MCI.status.sdio_interrupt  = 0U;
  MCI.status.ccs             = 0U;

  return status;
}

/**
 * @fn           int32_t  MCI_Control(uint32_t control, uint32_t arg)
 * @brief        Control MCI Interface.
 * @param[in]    control : Operation
 * @param[in]    arg  :  Argument of operation (optional)
 * @return 	 excecution status  
 */
int32_t MCI_Control(uint32_t control, uint32_t arg)
{
  if ((MCI.flags & MCI_POWER) == 0U) {
    return ARM_DRIVER_ERROR;
  }
  switch (control) {
    case ARM_MCI_BUS_SPEED:
      /* Bus speed configured */
      MCI.flags |= MCI_SETUP;
      break;
    case ARM_MCI_BUS_SPEED_MODE:
      switch (arg) {
        case ARM_MCI_BUS_DEFAULT_SPEED:
          /* Speed mode up to 25MHz */
          RSI_MCI_ClockCnfig(25000000);
          break;
        case ARM_MCI_BUS_HIGH_SPEED:
          /* Speed mode up to 50MHz */
          break;
        default:
          return ARM_DRIVER_ERROR_UNSUPPORTED;
      }
      break;

    case ARM_MCI_BUS_CMD_MODE:
      switch (arg) {

        case ARM_MCI_BUS_CMD_OPEN_DRAIN:
          /* Configure command line in open-drain mode */
          break;

        case ARM_MCI_BUS_CMD_PUSH_PULL:
          /* Configure command line in push-pull mode */
          break;

        default:
          return ARM_DRIVER_ERROR_UNSUPPORTED;
      }
      break;

    case ARM_MCI_BUS_DATA_WIDTH:
      switch (arg) {
        case ARM_MCI_BUS_DATA_WIDTH_1:
          SMIH->SMIH_HOST_CONTROL_1_REGISTER_b.DATA_TRANSFER_WIDTH = 0x0;
          break;
        case ARM_MCI_BUS_DATA_WIDTH_4:
          SMIH->SMIH_HOST_CONTROL_1_REGISTER_b.DATA_TRANSFER_WIDTH = 0x1;
          break;
        case ARM_MCI_BUS_DATA_WIDTH_8:
          break;
        default:
          return ARM_DRIVER_ERROR_UNSUPPORTED;
      }
      break;
    case ARM_MCI_CONTROL_CLOCK_IDLE:
      if (arg) {

      } else {
      }
      break;

    case ARM_MCI_DATA_TIMEOUT:
      SMIH->SMIH_TIMEOUT_CONTROL_REGISTER_b.DATA_TIMEOUT_COUNTER_VALUE = 0xE;
      break;

    case ARM_MCI_MONITOR_SDIO_INTERRUPT:
      MCI.status.sdio_interrupt = 0U;

      /* Enable card interrupt*/
      SMIH->SMIH_NORMAL_INTERRUPT_STATUS_ENABLE_REGISTER |= CARD_INTERRUPT_STATUS_ENABLE;

      break;

    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  return ARM_DRIVER_OK;
}

/**
 * @fn           ARM_MCI_STATUS  MCI_GetStatus(void)
 * @brief        Get MCI status.
 * @param[in]    none
 * @return 	 ARM_MCI_STATUS  
 */
ARM_MCI_STATUS MCI_GetStatus(void)
{
  return MCI.status;
}

/*SDHC IRQ Handler*/
void SDHC_IRQHandler(void)
{
  uint32_t msk, event;
  uint16_t normal_intr_status;
  uint16_t error_intr_status;
  uint32_t int_status;

  event = 0U;
  /*read normal interrupt status reg*/
  normal_intr_status = SMIH->SMIH_NORMAL_INTERRUPT_STATUS_REGISTER;

  /*read error interrupt status reg*/
  error_intr_status = SMIH->SMIH_ERROR_INTERRUPT_STATUS_REGISTER;

  int_status = error_intr_status << 16;
  int_status |= normal_intr_status;

  if (int_status & COMMANDS_INTERRUPTS) {
    int_status &= COMMANDS_INTERRUPTS;

    /* Command interrupts status */
    MCI.status.command_active = 0U;

    if ((int_status & ((COMMAND_TIMEOUT_ERROR_STATUS_ENABLE | COMMAND_CRC_ERROR_STATUS_ENABLE) << 16))
        == (COMMAND_TIMEOUT_ERROR_STATUS_ENABLE << 16)) {
      /* command timeout error */
      MCI.status.command_timeout = 1U;
      event                      = ARM_MCI_EVENT_COMMAND_TIMEOUT;
    } else if ((int_status & ((COMMAND_TIMEOUT_ERROR_STATUS_ENABLE | COMMAND_CRC_ERROR_STATUS_ENABLE) << 16))
               == (COMMAND_CRC_ERROR_STATUS_ENABLE << 16)) {
      /*  cmd crc error */
      MCI.status.command_error = 1U;
      event                    = ARM_MCI_EVENT_COMMAND_ERROR;
    } else if (int_status & (COMMAND_INDEX_ERROR_STATUS_ENABLE << 16)) {
      /* Command index error */
      MCI.status.command_error = 1U;
      event                    = ARM_MCI_EVENT_COMMAND_ERROR;
    } else if (int_status & (COMMAND_END_BIT_ERROR_STATUS_ENABLE << 16)) {
      /* Command end bit error */
      event = ARM_MCI_EVENT_COMMAND_ERROR;
    } else {
      msk = COMMAND_COMPLETE_STATUS_ENABLE
            | ((COMMAND_TIMEOUT_ERROR_STATUS_ENABLE | COMMAND_CRC_ERROR_STATUS_ENABLE) << 16);
      if ((int_status & msk) == COMMAND_COMPLETE_STATUS_ENABLE) {
        /* Command complete */
        if (MCI.response) {
          if (MCI.flags & MCI_RESP_LONG) {
            /* read response registers */
            MCI.response[3] = ((SMIH->SMIH_RESPONSE_REGISTER7 | SMIH->SMIH_RESPONSE_REGISTER6) << 8)
                              | ((SMIH->SMIH_RESPONSE_REGISTER5 | SMIH->SMIH_RESPONSE_REGISTER4) >> 24);
            MCI.response[2] = ((SMIH->SMIH_RESPONSE_REGISTER5 | SMIH->SMIH_RESPONSE_REGISTER4) << 8)
                              | ((SMIH->SMIH_RESPONSE_REGISTER3 | SMIH->SMIH_RESPONSE_REGISTER2) >> 24);
            MCI.response[1] = ((SMIH->SMIH_RESPONSE_REGISTER3 | SMIH->SMIH_RESPONSE_REGISTER2) << 8)
                              | ((SMIH->SMIH_RESPONSE_REGISTER1 | SMIH->SMIH_RESPONSE_REGISTER0) >> 24);
            MCI.response[0] = ((SMIH->SMIH_RESPONSE_REGISTER1 | SMIH->SMIH_RESPONSE_REGISTER0) << 8);
          } else {
            MCI.response[0] = SDMEM_RESPONSE_REG;
          }
        }
        event = ARM_MCI_EVENT_COMMAND_COMPLETE;
      }
    }
  }
  if (int_status & DATA_INTERRUPTS) {
    int_status &= DATA_INTERRUPTS;
    /* Data interrupts status */
    MCI.status.transfer_active = 0U;

    msk = ((DATA_CRC_ERROR_STATUS_ENABLE | DATA_END_BIT_ERROR_STATUS_ENABLE | ADMA_ERROR_STATUS_ENABLE) << 16);
    if (int_status & msk) {
      /*  data crc or data end bit or DMA errors */
      MCI.status.transfer_error = 1U;
      event                     = ARM_MCI_EVENT_TRANSFER_ERROR;
    } else if (int_status & (DATA_TIMEOUT_ERROR_STATUS_ENABLE << 16)) {
      /* Data transfer timeout error */
      MCI.status.transfer_timeout = 1U;
      event                       = ARM_MCI_EVENT_TRANSFER_TIMEOUT;
    } else {
      if (int_status & TRANSFER_COMPLETE_STATUS_ENABLE) {
        /* transfer complete interrupt */
        event = ARM_MCI_EVENT_TRANSFER_COMPLETE;
      }
    }
  } else {
    if (int_status & CARD_INTERRUPT_STATUS_ENABLE) {
      /* SDIO interrupt */
      MCI.status.sdio_interrupt = 1U;
      event                     = ARM_MCI_EVENT_SDIO_INTERRUPT;

      /* Disable SDIO Interrupt */
      SMIH->SMIH_NORMAL_INTERRUPT_STATUS_ENABLE_REGISTER = CARD_INTERRUPT_STATUS_ENABLE;
    }
  }
  /*clear interrupts*/
  SDMEM_INTR_STATUS_REG = int_status;

  /*send events*/
  if (event && (MCI.cb_event != NULL)) {
    MCI.cb_event(event);
  }
}

/**
 * @fn           void  RSI_MCI_ClockCnfig(uint32_t freq)
 * @brief        This API is used to configure the MCI clock
 */
void RSI_MCI_ClockCnfig(uint32_t freq)
{
  uint16_t u16Div = 0;

  uint32_t u32ClockInput = RTE_INPUT_CLOCK;

  u16Div = u32ClockInput / 2 / (freq);

  /*Disable sdmem clock */
  SMIH->SMIH_CLOCK_CONTROL_REGISTER_b.SD_CLOCK_ENABLE = 0u;

  /*set division value to the card*/
  SMIH->SMIH_CLOCK_CONTROL_REGISTER_b.SDCLK_FREQUENCY_SELECT               = (u16Div & 0xFFu);
  SMIH->SMIH_CLOCK_CONTROL_REGISTER_b.UPPER_BITS_OF_SDCLK_FREQUENCY_SELECT = ((u16Div >> 8u) & 0x03u);

  /*Enable Smih internal clock*/
  SMIH->SMIH_CLOCK_CONTROL_REGISTER_b.INTERNAL_CLOCK_ENABLE = 0x1;
  while (0x1 != SMIH->SMIH_CLOCK_CONTROL_REGISTER_b.INTERNAL_CLOCK_STABLE)
    ;

  /*enables SDMEM clock*/
  SMIH->SMIH_CLOCK_CONTROL_REGISTER_b.SD_CLOCK_ENABLE = 0x1;
}

/**
 * @fn           error_t RSI_MCI_SendCommand( MCI_COMMAND_FRAME_CONFIG_T* pConfig )
 * @brief        This API is used to send the command.
 * @param[in]    pConfig                 pointer to the command structure
 * @return       RSI_OK                  If command sent properly.
 *               INVALID_PARAMETERS      If pConfig==NULL
 */
error_t RSI_MCI_SendCommand(MCI_COMMAND_FRAME_CONFIG_T *pConfig)
{
  MCI_COMMAND_REG_T CmdData;

  memset(&CmdData, 0, sizeof(CmdData));

  if (pConfig == NULL) {
    return INVALID_PARAMETERS;
  }
  /* Set command CRC check */
  if (pConfig->checkCmdCrc) {
    CmdData.cmdCrcCheckEnable = 0x1;
  } else {
    CmdData.cmdCrcCheckEnable = 0x0;
  }
  /* Set command index check */
  if (pConfig->checkCmdIndex) {
    CmdData.cmdIndexCheckEnable = 0x1;
  } else {
    CmdData.cmdIndexCheckEnable = 0x0;
  }

  /* Set data present or not when sending the command */
  if (pConfig->dataPresent) {
    CmdData.dataPresentSelect = 0x1;
  } else {
    CmdData.dataPresentSelect = 0x0;
  }
  /* Configure command type */
  switch (pConfig->cmdType) {
    case MCI_NORMAL_CMD:
      CmdData.cmdType = 0u;
      break;
    case MCI_SUSPEND_CMD:
      CmdData.cmdType = 1u;
      break;
    case MCI_RESUME_CMD:
      CmdData.cmdType = 2u;
      break;
    case MCI_ABORT_CMD:
      CmdData.cmdType = 3u;
      break;
    default:
      return INVALID_PARAMETERS;
  }
  /* Set command response type */
  switch (pConfig->responseType) {
    case MCI_RESPONSE_NONE:
      CmdData.respType = 0u;
      break;
    case MCI_RESPONSE_136BIT:
      CmdData.respType = 1u;
      break;
    case MCI_RESPONSE_48BIT:
      CmdData.respType = 2u;
      break;
    case MCI_RESPONSE_48BIT_BUSY_CHECK:
      CmdData.respType = 3u;
      break;
    default:
      return INVALID_PARAMETERS;
  }

  /* Set command index */
  CmdData.cmdIndex = pConfig->cmdIndex;

  /* Auto command setting */
  switch (pConfig->autoCmdType) {
    case MCI_DISABLE_AUTO_CMD:
      SMIH->TRANSFER_MODE_REGISTER_b.AUTO_CMD_ENABLE = 0u;
      break;
    case MCI_ENABLE_AUTO_CMD12:
      SMIH->TRANSFER_MODE_REGISTER_b.AUTO_CMD_ENABLE = 1u;
      break;
    case MCI_ENABLE_AUTO_CMD23:
      SMIH->TRANSFER_MODE_REGISTER_b.AUTO_CMD_ENABLE = 2u;
      break;
    default:
      return INVALID_PARAMETERS;
  }
  /* Configure argument register */
  SMIH->SMIH_ARGUMENT1_REGISTER = pConfig->argument;

  if (pConfig->cmdIndex == 5) {
    if ((pConfig->argument & (1 << 24))) {
      SMIH->SMIH_HOST_CONTROL_2_REGISTER = (1 << 3);
    }
  }
  /* assign fiiled data to the command register */
  SMIH->SMIH_COMMAND_REGISTER = *((uint16_t *)&CmdData);
  return RSI_OK;
}
/**
 * @fn           error_t  RSI_MCI_DataTransferInitialization(MCI_DATA_CONFIG_T* pDataConfig)
 * @brief        This API is used to initialize the data transfer(this must be called before data transfer).
 * @param[in]    pDataConfig            pointer to the data transfer configuration
 * @return       RSI_OK                 data initialized properly.
 *               INVALID_PARAMETERS     If pDataConfig==NULL
 */
error_t RSI_MCI_DataTransferInitialization(MCI_DATA_CONFIG_T *pDataConfig)
{
  if (pDataConfig == 0) {
    return INVALID_PARAMETERS;
  }
  /* Configure multiple block or single block transfer */
  if (pDataConfig->multiBlock) {
    SMIH->TRANSFER_MODE_REGISTER_b.MULTI_OR_SINGLE_BLOCK_SELECT = 0x1;
  } else {
    SMIH->TRANSFER_MODE_REGISTER_b.MULTI_OR_SINGLE_BLOCK_SELECT = 0x0;
  }
  /* Confgure data transfer direction */
  if (pDataConfig->dataTransferDirection) {
    SMIH->TRANSFER_MODE_REGISTER_b.DATA_TRANSFER_DIRECTION_SELECT = 0x1;
  } else {
    SMIH->TRANSFER_MODE_REGISTER_b.DATA_TRANSFER_DIRECTION_SELECT = 0x0;
  }
  /* Configure block size */
  SMIH->SMIH_BLOCK_SIZE_REGISTER_b.TRANSFER_BLOCK_SIZE = pDataConfig->blockSize;

  /* Configure block count */
  if (pDataConfig->blockCount == 0) {
    SMIH->SMIH_BLOCK_COUNT_REGISTER = 1;
  } else {
    SMIH->SMIH_BLOCK_COUNT_REGISTER = pDataConfig->blockCount;
  }
  /*enable block count*/
  if (pDataConfig->blockCountEnable) {
    SMIH->TRANSFER_MODE_REGISTER_b.BLOCK_COUNT_ENABLE = 0x1;
  } else {
    SMIH->TRANSFER_MODE_REGISTER_b.BLOCK_COUNT_ENABLE = 0x0;
  }
  /* Enable DMA mode */
  if (pDataConfig->dmaEnable) {
    SMIH->TRANSFER_MODE_REGISTER_b.DMA_ENABLE = 0x1;
  } else {
    SMIH->TRANSFER_MODE_REGISTER_b.DMA_ENABLE = 0x1;
  }

  /* Configure descriptor table for ADMA */
  SMIH->SMIH_ADMA_SYSTEM_ADDRESS0_REGISTER = (uint16_t)pDataConfig->admaDespTableAddress;
  SMIH->SMIH_ADMA_SYSTEM_ADDRESS1_REGISTER = (uint16_t)(pDataConfig->admaDespTableAddress >> 16u);

  return RSI_OK;
}
// End MCI Interface
ARM_DRIVER_MCI Driver_MCI0 = { MCI_GetVersion,   MCI_GetCapabilities, MCI_Initialize,    MCI_Uninitialize,
                               MCI_PowerControl, MCI_CardPower,       MCI_ReadCD,        MCI_ReadWP,
                               MCI_SendCommand,  MCI_SetupTransfer,   MCI_AbortTransfer, MCI_Control,
                               MCI_GetStatus };
#endif
