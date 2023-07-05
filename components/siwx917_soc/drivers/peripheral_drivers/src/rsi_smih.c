/*******************************************************************************
* @file  rsi_smih.c
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

#include "rsi_ccp_user_config.h"
#if defined(CHIP_9118)
#include "rsi_chip.h"

static SMIH_TRANSFER_T *CommandInProgress = 0;
static SMIH_CONFIG_MODES_T *modesConfig   = 0;
static SMIH_CONFIG_MODES_T modeConfig;
static SMIH_ADMA_DESC_TABLE_T Adma2DescriptorTable[2] = { 0 };
volatile static SMIH_EVENT_T event;

#define SD_IRQHandler IRQ068_Handler

// SMIH CONTROLLER RELATED FUNCTIONS
/*==============================================*/
/**
 * @fn           error_t RSI_SD_HostInit(SMIH_CARD_CONFIG_T *pSmihConfig, ARM_SMIH_SignalEvent_t Event, uint8_t MemType)
 * @brief        This API is used to initialize the smih host configuration
 * @param[in]    pSmihConfig     : Pointer to the smih card configuration structure
 * @param[in]    Event           : event handler to be register
 * @param[in]    MemType         : type of operation to be pragram
 *                                 1 for memory operations ,
 *                                 0 for IO operations
 * @return       RSI_OK          : If host initialized successfully
 */
error_t RSI_SD_HostInit(SMIH_CARD_CONFIG_T *pSmihConfig, ARM_SMIH_SignalEvent_t Event, uint8_t MemType)
{
  SMIH_CONFIG_MODES_T commandCfg;
  if (pSmihConfig == 0) {
    return INVALID_PARAMETERS;
  }
  // sleepclock prog
  *(volatile uint32_t *)(0x46000024) = (0x0 << 21);

  // wait for clock switch
  while ((M4CLK->PLL_STAT_REG_b.SLEEP_CLK_SWITCHED) != 1)
    ;

  //  Wait until card inserts
  while (SMIH->SMIH_PRESENT_STATE_REGISTER_b.CARD_STATE_STABLE != 0x1)
    ;
  if (SMIH->SMIH_PRESENT_STATE_REGISTER_b.CARD_INSERTED == 0) {
    return ERROR_SMIH;
  }

  //  Register callbacks
  event.callb_event = Event;

  if (MemType) {
    pSmihConfig->cmd_transfer = smih_memory_command_transfer;
  } else {
    pSmihConfig->cmd_transfer = smih_io_command_transfer;
  }

  // Set bus violtage
  if (pSmihConfig->voltage) {
    smih_bus_voltage_select(VOLTAGE_18V);
  } else {
    smih_bus_voltage_select(VOLTAGE_33V);
  }

  SMIH->SMIH_POWER_CONTROL_REGISTER_b.SD_BUS_POWER = 0x1;

  if (pSmihConfig->busWidthMode) {
    // SMIH->SMIH_HOST_CONTROL_1_REGISTER_b.DATA_TRANSFER_WIDTH = 0x1; //fix for MMC
  } else {
    SMIH->SMIH_HOST_CONTROL_1_REGISTER_b.DATA_TRANSFER_WIDTH = 0x0;
  }

  // Configure ADMA or IO mode
  if (pSmihConfig->admaMode) {
    SMIH->SMIH_HOST_CONTROL_1_REGISTER_b.DMA_SELECT = 0x2;
  } else {
    SMIH->SMIH_HOST_CONTROL_1_REGISTER_b.DMA_SELECT = 0x0;
  }

  // Configure high speed mode
  if (pSmihConfig->highSpeedEnable) {
    SMIH->SMIH_HOST_CONTROL_1_REGISTER_b.HIGH_SPEED_ENABLE = 0x1;
  } else {
    SMIH->SMIH_HOST_CONTROL_1_REGISTER_b.HIGH_SPEED_ENABLE = 0x0;
  }

  smih_clock_config(pSmihConfig, 400000);

  // Enable normal interrupts
  SMIH->SMIH_NORMAL_INTERRUPT_STATUS_ENABLE_REGISTER =
    (COMMAND_COMPLETE_STATUS_ENABLE | TRANSFER_COMPLETE_STATUS_ENABLE | BLOCK_GAP_EVENT_STATUS_ENABLE
     | DMA_INTERRUPT_STATUS_ENABLE | BUFFER_WRITE_READY_STATUS_ENABLE | BUFFER_READ_READY_STATUS_ENABLE
     | CARD_INSERTION_STATUS_ENABLE | CARD_REMOVAL_STATUS_ENABLE | CARD_INTERRUPT_STATUS_ENABLE | INT_A_STATUS_ENABLE
     | INT_B_STATUS_ENABLE | INT_C_STATUS_ENABLE | RE_TUNING_EVENT_STATUS_ENABLE);

  // Enable error interrupts
  SMIH->SMIH_ERROR_INTERRUPT_STATUS_ENABLE_REGISTER =
    (COMMAND_TIMEOUT_ERROR_STATUS_ENABLE | COMMAND_CRC_ERROR_STATUS_ENABLE | COMMAND_END_BIT_ERROR_STATUS_ENABLE
     | COMMAND_INDEX_ERROR_STATUS_ENABLE | DATA_TIMEOUT_ERROR_STATUS_ENABLE | DATA_CRC_ERROR_STATUS_ENABLE
     | DATA_END_BIT_ERROR_STATUS_ENABLE | CURRENT_LIMIT_ERROR_STATUS_ENABLE | AUTO_CMD_ERROR_STATUS_ENABLE
     | ADMA_ERROR_STATUS_ENABLE | TUNING_ERROR_STATUS_ENABLE);

  // Enable normal interrupts signals
  SMIH->SMIH_NORMAL_INTERRUPT_SIGNAL_ENABLE_REGISTER =
    (COMMAND_COMPLETE_SIGNAL_ENABLE | TRANSFER_COMPLETE_SIGNAL_ENABLE | BUFFER_WRITE_READY_SIGNAL_ENABLE
     | BUFFER_READ_READY_SIGNAL_ENABLE | CARD_REMOVAL_SIGNAL_ENABLE | CARD_INTERRUPT_SIGNAL_ENABLE);
  SMIH->SMIH_ERROR_INTERRUPT_SIGNAL_ENABLE_REGISTER = 0x1;

  // Enable Irq
  NVIC_EnableIRQ(SDMEM_IRQn);

  memset(&commandCfg, 0, sizeof(commandCfg));
  commandCfg.highSpeedEnable = pSmihConfig->highSpeedEnable;
  commandCfg.admaMode        = pSmihConfig->admaMode;
  commandCfg.busWidthMode    = pSmihConfig->busWidthMode;
  commandCfg.clock           = pSmihConfig->clock;

  if (RSI_OK != smih_modes_configuration(&commandCfg)) {
    return ERROR_SMIH;
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t Smih_DeInitialization(void)
 * @brief        This API is used to Deinitialize the host controller.               *
 * @return       RSI_OK  : If host deinitialized successfully.
 */
error_t Smih_DeInitialization(void)
{
  // Clear clock control and power control registers
  SMIH->SMIH_CLOCK_CONTROL_REGISTER = 0x0000;
  SMIH->SMIH_POWER_CONTROL_REGISTER = 0x00;

  SMIH->SMIH_NORMAL_INTERRUPT_SIGNAL_ENABLE_REGISTER = 0x0000;
  SMIH->SMIH_ERROR_INTERRUPT_SIGNAL_ENABLE_REGISTER  = 0x0000;

  // disable nvic
  NVIC_DisableIRQ(SDMEM_IRQn);
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t smih_bus_width_set(uint8_t BusWidthMode)
 * @brief        This API is used to set the smih bus width.
 * @param[in]    BusWidthMode    :   bus width mode
 *               possible values are  0 for 1bit mode
 *                                    1 for 4bit mode
 *                                    2 for 8bit mode
 * @return       RSI_OK : If bus width set successfully.
 */
error_t smih_bus_width_set(uint8_t BusWidthMode)
{
  // Configure bus width
  if (BusWidthMode == 0) {
    SMIH->SMIH_HOST_CONTROL_1_REGISTER_b.DATA_TRANSFER_WIDTH = 0x0;
  } else if (BusWidthMode == 1) {
    SMIH->SMIH_HOST_CONTROL_1_REGISTER_b.DATA_TRANSFER_WIDTH = 0x1;
  } else if (BusWidthMode == 2) {
    SMIH->SMIH_HOST_CONTROL_1_REGISTER_b.EXTENDED_DATA_TRANSFER_WIDTH = 0x1;
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t smih_bus_voltage_select(uint8_t enVoltage)
 * @brief        This API is used to select the smih voltage.
 * @param[in]    enVoltage  : voltage selection
 *                            possbile selections are
 *                            VOLTAGE_18V   1.8v voltage selection for sdio interface
 *                            VOLTAGE_30V   3.0v voltage selection for sdio interface
 *                            VOLTAGE_33V   3.3v voltage selection for sdio interface
 * @return       RSI_OK     : If voltage configured Successfully.
 */
error_t smih_bus_voltage_select(uint8_t enVoltage)
{
  if (enVoltage == VOLTAGE_18V) {
    SMIH->SMIH_POWER_CONTROL_REGISTER_b.SD_BUS_VOLTAGE_SELECT = 0x5;
  } else if (enVoltage == VOLTAGE_30V) {
    SMIH->SMIH_POWER_CONTROL_REGISTER_b.SD_BUS_VOLTAGE_SELECT = 0x6;
  } else if (enVoltage == VOLTAGE_33V) {
    SMIH->SMIH_POWER_CONTROL_REGISTER_b.SD_BUS_VOLTAGE_SELECT = 0x7;
  } else {
    return INVALID_PARAMETERS;
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t smih_command_xfer(SMIH_COMMAND_FRAME_CONFIG_T *pConfig)
 * @brief        This API is used to send the command.
 * @param[in]    pConfig             :  pointer to the command structure
 * @return       RSI_OK              :  If command sent properly.
 *               INVALID_PARAMETERS  :  If pConfig == NULL
 */
error_t smih_command_xfer(SMIH_COMMAND_FRAME_CONFIG_T *pConfig)
{
  SMIH_COMMAND_REG_T cmdData;

  memset(&cmdData, 0, sizeof(cmdData));

  if (pConfig == NULL) {
    return INVALID_PARAMETERS;
  }

  // Set command response type
  switch (pConfig->responseTypeSelect) {
    case SMIH_NO_RESPONSE:
      cmdData.respType = 0x0;
      break;
    case SMIH_RESPONSE_LENGTH_136:
      cmdData.respType = 0x1;
      break;
    case SMIH_RESPONSE_LENGTH_48:
      cmdData.respType = 0x2;
      break;
    case SMIH_RESPONSE_LENGTH_48BIT_BUSY_CHECK:
      cmdData.respType = 0x3;
      break;
    default:
      return INVALID_PARAMETERS;
  }

  // Set command CRC check
  if (pConfig->cmdCrcCheckEn) {
    cmdData.cmdCrcCheckEnable = 0x1;
  } else {
    cmdData.cmdCrcCheckEnable = 0x0;
  }

  // Set command index check
  if (pConfig->cmdIndexCheckEn) {
    cmdData.cmdIndexCheckEnable = 0x1;
  } else {
    cmdData.cmdIndexCheckEnable = 0x0;
  }

  // Set data present or not when sending the command
  if (pConfig->dataPresentSelect) {
    cmdData.dataPresentSelect = 0x1;
  } else {
    cmdData.dataPresentSelect = 0x0;
  }

  // Configure command type
  switch (pConfig->cmdType) {
    case NORMAL_CMD:
      cmdData.cmdType = 0x0;
      break;
    case SUSPEND_CMD:
      cmdData.cmdType = 0x1;
      break;
    case RESUME_CMD:
      cmdData.cmdType = 0x2;
      break;
    case ABORT_CMD:
      cmdData.cmdType = 0x3;
      break;
    default:
      return INVALID_PARAMETERS;
  }

  // Set command index
  cmdData.cmdIndex = pConfig->cmdIndex;

  // Auto command setting
  switch (pConfig->autoCmdType) {
    case DISABLE_AUTO_CMD:
      SMIH->TRANSFER_MODE_REGISTER_b.AUTO_CMD_ENABLE = 0x0;
      break;
    case ENABLE_AUTO_CMD12:
      SMIH->TRANSFER_MODE_REGISTER_b.AUTO_CMD_ENABLE = 0x1;
      break;
    case ENABLE_AUTO_CMD23:
      SMIH->TRANSFER_MODE_REGISTER_b.AUTO_CMD_ENABLE = 0x2;
      break;
    default:
      return INVALID_PARAMETERS;
  }
  // Configure argument register
  SMIH->SMIH_ARGUMENT1_REGISTER = pConfig->cmdArgument;

  if (pConfig->cmdIndex == 5) {
    if ((pConfig->cmdArgument & (1 << 24))) {
      SMIH->SMIH_HOST_CONTROL_2_REGISTER = (1 << 3);
    }
  }
  // assign fiiled data to the command register
  SMIH->SMIH_COMMAND_REGISTER = *((uint16_t *)&cmdData);
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t smih_get_response(uint16_t *pResponseData, uint8_t ResponseRegCount)
 * @brief        This API is used to receive response on cmd line.
 * @param[in]    pResponseData       :   pointer to the response data
 * @return       RSI_OK              :   If command sent properly.
 *               INVALID_PARAMETERS  :   If pConfig==NULL or ResponseRegCount >8
 */
error_t smih_get_response(uint16_t *pResponseData, uint8_t ResponseRegCount)
{
  uint16_t *pResponseBaseAddr;
  uint8_t i;

  if ((pResponseData == 0) || (ResponseRegCount > 8u)) {
    return INVALID_PARAMETERS;
  }
  pResponseBaseAddr = (uint16_t *)&SMIH->SMIH_RESPONSE_REGISTER0;

  for (i = 0; i < ResponseRegCount; i++) {
    *pResponseData++ = *pResponseBaseAddr++;
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t smih_stop_at_block_gap(void)
 * @brief        This API is used to stop multiple block transfer.
 * @return       RSI_OK    : Stops data transfer.
 */
error_t smih_stop_at_block_gap(void)
{
  SMIH->SMIH_BLOCK_GAP_CONTROL_REGISTER_b.STOP_AT_BLOCK_GAP_REQUEST = 0x1;
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t smih_transfer_restart(void)
 * @brief        This API is used to restart the transfer when the transfer is pending.
 * @return       RSI_OK   :   If data restarts transfer successfully.
 */
error_t smih_transfer_restart(void)
{
  SMIH->SMIH_BLOCK_GAP_CONTROL_REGISTER_b.CONTINUE_REQUEST = 0x1;
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           void smih_18v_signal_enable(void)
 * @brief        This API is used to enable 1.8v signal enable bit.
 * @return       void
 */
void smih_18v_signal_enable(void)
{
  SMIH->SMIH_HOST_CONTROL_2_REGISTER_b._1_8V_SIGNALING_ENABLE = 0x1;
}

/*==============================================*/
/**
 * @fn           error_t smih_uhs_mode_select(uint8_t UhsMode)
 * @brief        This API is used to select the smih UHS(ULTRA HIGH SPEED) mode.
 * @param[in]    UhsMode   : Uhs mode selection
 *                           possbile selections are
 *                           UHS_NONE    no uhs mode
 *                           UHS_SDR12   in case of SDR12  mode requirement
 *                           UHS_SDR25   in case of SDR25  mode requirement
 *                           UHS_SDR50   in case of SDR50  mode requirement
 *                           UHS_SDR104  in case of SDR104 mode requirement
 *                           UHS_DDR50   in case of DDR50  mode requirement
 * @return       RSI_OK    : If uhs mode configured  properly.
 */
error_t smih_uhs_mode_select(uint8_t UhsMode)
{
  switch (UhsMode) {
    case UHS_SDR12:
      SMIH->SMIH_HOST_CONTROL_2_REGISTER_b.UHS_MODE_SELECT = 0x0;
      break;
    case UHS_SDR25:
      SMIH->SMIH_HOST_CONTROL_2_REGISTER_b.UHS_MODE_SELECT = 0x1;
      break;
    case UHS_SDR50:
      SMIH->SMIH_HOST_CONTROL_2_REGISTER_b.UHS_MODE_SELECT = 0x2;
      break;
    case UHS_SDR104:
      SMIH->SMIH_HOST_CONTROL_2_REGISTER_b.UHS_MODE_SELECT = 0x3;
      break;
    case UHS_DDR50:
      SMIH->SMIH_HOST_CONTROL_2_REGISTER_b.UHS_MODE_SELECT = 0x4;
      break;
    default:
      return INVALID_PARAMETERS;
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           void smih_irq_handler(void)
 * @brief        This is smih Irq Handler
 * @return       none 
*/
void smih_irq_handler(void)
{
  uint32_t normal_intr_status;
  uint32_t error_intr_status;

  // read normal interrupt status reg
  normal_intr_status = SMIH->SMIH_NORMAL_INTERRUPT_STATUS_REGISTER;

  // read error interrupt status reg
  error_intr_status = SMIH->SMIH_ERROR_INTERRUPT_STATUS_REGISTER;

  // Command complete
  if (normal_intr_status & BIT(0)) {
    event.commandComplete = 1;
    if (event.callb_event != NULL) {
      event.callb_event(COMMAND_COMPLETE);
    }
    SMIH->SMIH_NORMAL_INTERRUPT_STATUS_REGISTER = BIT(0);
  }
  // Transfer complete
  if (normal_intr_status & BIT(1)) {
    event.transferComplete = 1;
    if (event.callb_event != NULL) {
      event.callb_event(TRANSFER_COMPLETE);
    }
    SMIH->SMIH_NORMAL_INTERRUPT_STATUS_REGISTER = BIT(1);
  }
  // block gap event
  if (normal_intr_status & BIT(2)) {
    SMIH->SMIH_NORMAL_INTERRUPT_STATUS_REGISTER = BIT(2);
  }
  // DMA  event
  if (normal_intr_status & BIT(3)) {
    if (event.callb_event != NULL) {
      event.callb_event(DMA_INTR);
    }
    SMIH->SMIH_NORMAL_INTERRUPT_STATUS_REGISTER = BIT(3);
  }
  // Buffer write ready
  if (normal_intr_status & BIT(4)) {
    event.bufferWriteReady = 1;
    if (event.callb_event != NULL) {
      event.callb_event(BUFFER_WRITE_READY);
    }
    SMIH->SMIH_NORMAL_INTERRUPT_STATUS_REGISTER = BIT(4);
  }
  // Buffer read ready
  if (normal_intr_status & BIT(5)) {
    event.bufferReadReady = 1;
    if (event.callb_event != NULL) {
      event.callb_event(BUFFER_READ_READY);
    }
    SMIH->SMIH_NORMAL_INTERRUPT_STATUS_REGISTER = BIT(5);
  }
  // Card insetion
  if (normal_intr_status & BIT(6)) {
    if (event.callb_event != NULL) {
      event.callb_event(CARD_INSERTION);
    }
    SMIH->SMIH_NORMAL_INTERRUPT_STATUS_REGISTER = BIT(6);
  }
  // Card removal
  if (normal_intr_status & BIT(7)) {
    event.cardRemoval = 1;
    if (event.callb_event != NULL) {
      event.callb_event(CARD_REMOVAL);
    }
    SMIH->SMIH_NORMAL_INTERRUPT_STATUS_REGISTER = BIT(7);
  }
  // Card interrupt
  if (normal_intr_status & BIT(8)) {
    event.cardInterrupt = 1;
    if (event.callb_event != NULL) {
      event.callb_event(CARD_INTERRUPT);
    }
    SMIH->SMIH_NORMAL_INTERRUPT_STATUS_REGISTER = BIT(8);
  }
  // Command timeout error
  if (error_intr_status & BIT(0)) {
    event.commandTimeoutError = 1;
    if (event.callb_event != NULL) {
      event.callb_event(CMD_TIMEOUT_ERROR);
    }
    SMIH->SMIH_ERROR_INTERRUPT_STATUS_REGISTER = BIT(0);
  }
  // Command CRC error
  if (error_intr_status & BIT(1)) {
    event.commandCrcError = 1;
    if (event.callb_event != NULL) {
      event.callb_event(CMD_CRC_ERROR);
    }
    SMIH->SMIH_ERROR_INTERRUPT_STATUS_REGISTER = BIT(1);
  }
  // Command end bit error
  if (error_intr_status & BIT(2)) {
    event.commandEndBitError = 1;
    if (event.callb_event != NULL) {
      event.callb_event(CMD_END_BIT_ERROR);
    }
    SMIH->SMIH_ERROR_INTERRUPT_STATUS_REGISTER = BIT(2);
  }
  // Command index error
  if (error_intr_status & BIT(3)) {
    event.commandIndexError = 1;
    if (event.callb_event != NULL) {
      event.callb_event(CMD_INDEX_ERROR);
    }
    SMIH->SMIH_ERROR_INTERRUPT_STATUS_REGISTER = BIT(3);
  }
  // data timeout error
  if (error_intr_status & BIT(4)) {
    event.dataTimeoutError = 1;
    if (event.callb_event != NULL) {
      event.callb_event(DATA_TIMEOUT_ERROR);
    }
    SMIH->SMIH_ERROR_INTERRUPT_STATUS_REGISTER = BIT(4);
  }
  // data end bit error
  if (error_intr_status & BIT(5)) {
    event.dataEndbitError = 1;
    if (event.callb_event != NULL) {
      event.callb_event(DATA_END_BIT_ERROR);
    }
    SMIH->SMIH_ERROR_INTERRUPT_STATUS_REGISTER = BIT(5);
  }
  // data CRC error
  if (error_intr_status & BIT(6)) {
    event.dataCrcError = 1;
    if (event.callb_event != NULL) {
      event.callb_event(DATA_CRC_ERROR);
    }
    SMIH->SMIH_ERROR_INTERRUPT_STATUS_REGISTER = BIT(6);
  }
  // Current limitation error
  if (error_intr_status & BIT(7)) {
    event.currentLimitError = 1;
    if (event.callb_event != NULL) {
      event.callb_event(CURRENT_LIMIT_ERROR);
    }
    SMIH->SMIH_ERROR_INTERRUPT_STATUS_REGISTER = BIT(7);
  }
  // Auto CMD12 error
  if (error_intr_status & BIT(8)) {
    event.autoCommandError = 1;
    if (event.callb_event != NULL) {
      event.callb_event(AUTO_CMD_ERROR);
    }
    SMIH->SMIH_ERROR_INTERRUPT_STATUS_REGISTER = BIT(8);
  }
  // ADMA error
  if (error_intr_status & BIT(9)) {
    event.admaError = 1;
    if (event.callb_event != NULL) {
      event.callb_event(ADMA_ERROR);
    }
    SMIH->SMIH_ERROR_INTERRUPT_STATUS_REGISTER = BIT(9);
  }
  // Tuning error
  if (error_intr_status & BIT(10)) {
    event.tuningError = 1;
    if (event.callb_event != NULL) {
      event.callb_event(TUNING_ERROR);
    }
    SMIH->SMIH_ERROR_INTERRUPT_STATUS_REGISTER = BIT(10);
  }
  return;
}

/*==============================================*/
/**
 * @fn           void SD_IRQHandler(void)
 * @brief        SMIH Interrupt Handler
 * @return       none
*/
void SD_IRQHandler(void)
{
  smih_irq_handler();
}

/*==============================================*/
/**
 * @fn           error_t smih_modes_configuration(SMIH_CONFIG_MODES_T *pSmihConfig)
 * @brief        This API is used to configure modes to SMIH
 * @param[in]    pSmihConfig          : Pointer to the IO card configuration
 * @return       RSI_OK               : IO command configuration structure initialized successfully
 *               INVALID_PARAMETERS   : In case of Invalid parameter
 */
error_t smih_modes_configuration(SMIH_CONFIG_MODES_T *pSmihConfig)
{
  if (modesConfig == NULL) {
    modesConfig = &modeConfig;
  }
  if (pSmihConfig != NULL) {
    memcpy(modesConfig, pSmihConfig, sizeof(SMIH_CONFIG_MODES_T));
    return RSI_OK;
  }
  return ERROR_SMIH;
}

/*==============================================*/
/**
 * @fn           error_t smih_clock_config(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t freq)
 * @brief        This API is used to configure the host controller clock
 * @param[in]    freq    : clock frequency to the host
 * @return       RSI_OK  :  If new frequency is set
 */
error_t smih_clock_config(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t freq)
{
  uint16_t Div        = 0;
  uint32_t clockInput = pSmihConfig->clock;
  Div                 = clockInput / 2 / (freq);

  // disables smih clock
  SMIH->SMIH_CLOCK_CONTROL_REGISTER_b.SD_CLOCK_ENABLE = 0u;

  // set division value to the clock
  SMIH->SMIH_CLOCK_CONTROL_REGISTER_b.SDCLK_FREQUENCY_SELECT               = (Div & 0xFF);
  SMIH->SMIH_CLOCK_CONTROL_REGISTER_b.UPPER_BITS_OF_SDCLK_FREQUENCY_SELECT = ((Div >> 8) & 0x03);

  // Enable Smih internal clock
  SMIH->SMIH_CLOCK_CONTROL_REGISTER_b.INTERNAL_CLOCK_ENABLE = 0x1;

  //  wait for inter clock to be stable
  while (0x1 != SMIH->SMIH_CLOCK_CONTROL_REGISTER_b.INTERNAL_CLOCK_STABLE)
    ;

  // enables smih clock
  SMIH->SMIH_CLOCK_CONTROL_REGISTER_b.SD_CLOCK_ENABLE = 0x1;

  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t smih_check_for_error_interrupt(void)
 * @brief        This API is used to check perticular error event happend or not.
 * @return       RSI_OK         : If error event not occured.
 *               ERROR_TIMEOUT  : If event occured
 */
error_t smih_check_for_error_interrupt(void)
{
  if (event.cardRemoval) {
    event.cardRemoval = 0;
    return ERROR_ACCESS_RIGHTS;
  }
  if (event.commandTimeoutError || event.commandCrcError || event.commandEndBitError || event.commandIndexError
      || event.dataTimeoutError || event.dataEndbitError || event.dataCrcError) {
    return ERROR_TIMEOUT;
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t smih_send_data(SMIH_TRANSFER_T *pTransfer)
 * @brief        This API is used to write the data to the SMIH FIFO
 * @param[in]    pTransfer       : Pointer to the command and data structure
 * @return       RSI_OK          : If the data sent properly.
 *               ERROR_TIMEOUT   : If the write data time out occurs.
 */
error_t smih_send_data(SMIH_TRANSFER_T *pTransfer)
{
  uint32_t blocksize = 0;
  uint32_t blockcnt  = 0;
  uint32_t i         = 0;
  uint32_t *pBuffer  = NULL;

  if (modesConfig->admaMode != 1) // IO mode
  {
    while (1) {
      if (event.bufferWriteReady) {
        event.bufferWriteReady = 0;
        break;
      }
      //  check for error interrupt
      if (RSI_OK != smih_check_for_error_interrupt()) {
        return ERROR_TIMEOUT;
      }
      // wait until card inserts
      while (SMIH->SMIH_PRESENT_STATE_REGISTER_b.CARD_STATE_STABLE != 0x1)
        ;

      if (SMIH->SMIH_PRESENT_STATE_REGISTER_b.CARD_INSERTED == 0) {
        return ERROR_SMIH;
      }
    }
    blockcnt  = pTransfer->data->blockCount;
    blocksize = pTransfer->data->blockSize;
    pBuffer   = (uint32_t *)pTransfer->data->data;

    while (blockcnt > 0) {
      while (0x0 == SMIH->SMIH_PRESENT_STATE_REGISTER_b.BUFFER_WRITE_ENABLE)
        ;
      if (0x0 == SMIH->SMIH_PRESENT_STATE_REGISTER_b.BUFFER_WRITE_ENABLE) {
        return ERROR_TIMEOUT;
      }
      // write data to the fifo
      for (i = 0; i < (blocksize >> 2); i++) {
        SMIH->SMIH_BUFFER_DATA_PORT_REGISTER = (*pBuffer++);
      }
      blockcnt--;
    }
    // wait for transfer completion event to be occur
    while (1) {
      if (event.transferComplete) {
        event.transferComplete = 0;
        break;
      }
      // check for error interrupt
      if (RSI_OK != smih_check_for_error_interrupt()) {
        return ERROR_TIMEOUT;
      }
      // wait until card inserts
      while (SMIH->SMIH_PRESENT_STATE_REGISTER_b.CARD_STATE_STABLE != 0x1)
        ;

      if (SMIH->SMIH_PRESENT_STATE_REGISTER_b.CARD_INSERTED == 0) {
        return ERROR_SMIH;
      }
    }
  } else //adm2 mode
  {
    // wait for transfer done
    while (1) {
      if (event.transferComplete) {
        event.transferComplete = 0;
        break;
      }
      // check for error interrupt
      if (RSI_OK != smih_check_for_error_interrupt()) {
        return ERROR_TIMEOUT;
      }
      // wait until card inserts
      while (SMIH->SMIH_PRESENT_STATE_REGISTER_b.CARD_STATE_STABLE != 0x1)
        ;

      if (SMIH->SMIH_PRESENT_STATE_REGISTER_b.CARD_INSERTED == 0) {
        return ERROR_SMIH;
      }
    }
  }
  // Wait for data line to be free
  while ((SMIH->SMIH_PRESENT_STATE_REGISTER_b.DAT_LINE_ACTIVE) != 0)
    ;

  if ((SMIH->SMIH_PRESENT_STATE_REGISTER_b.DAT_LINE_ACTIVE) == 0x1) {
    return ERROR_SMIH;
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t smih_receive_data(SMIH_TRANSFER_T *pTransfer)
 * @brief        This API is used to read the data from SMIH FIFO
 * @param[in]    pTransfer       : Pointer to the command and data structure
 * @return       RSI_OK          : If the data read properly.
 *               ERROR_TIMEOUT   : If data timeout occurs.
 */
error_t smih_receive_data(SMIH_TRANSFER_T *pTransfer)
{
  uint32_t blocksize       = 0;
  static uint32_t blockcnt = 0;
  uint32_t i               = 0;
  uint32_t *pBuffer        = NULL;

  if (modesConfig->admaMode != 1) // IO mode
  {
    // wait for read ready event
    while (1) {
      if (event.bufferReadReady) {
        event.bufferReadReady = 0;
        break;
      }
      // check for error interrupt
      if (RSI_OK != smih_check_for_error_interrupt()) {
        return ERROR_TIMEOUT;
      }

      // wait until card inserts
      while (SMIH->SMIH_PRESENT_STATE_REGISTER_b.CARD_STATE_STABLE != 0x1)
        ;

      if (SMIH->SMIH_PRESENT_STATE_REGISTER_b.CARD_INSERTED == 0) {
        return ERROR_SMIH;
      }
    }
    pBuffer   = (uint32_t *)pTransfer->data->data;
    blockcnt  = pTransfer->data->blockCount;
    blocksize = pTransfer->data->blockSize;

    // read data from fifo
    while (blockcnt > 0) {
      while (SMIH->SMIH_PRESENT_STATE_REGISTER_b.BUFFER_READ_ENABLE == 0)
        ;
      if (SMIH->SMIH_PRESENT_STATE_REGISTER_b.BUFFER_READ_ENABLE == 0) {
        break;
      }
      for (i = 0; i < (blocksize >> 2); i++) {
        *pBuffer++ = SMIH->SMIH_BUFFER_DATA_PORT_REGISTER;
      }
      blockcnt--;
    }

    // wait until data transfer done
    while (1) {
      if (event.transferComplete) {
        event.transferComplete = 0;
        break;
      }
      // check for error interrupt
      if (RSI_OK != smih_check_for_error_interrupt()) {
        return ERROR_TIMEOUT;
      }
      // wait until card inserts
      while (SMIH->SMIH_PRESENT_STATE_REGISTER_b.CARD_STATE_STABLE != 0x1)
        ;
      if (SMIH->SMIH_PRESENT_STATE_REGISTER_b.CARD_INSERTED == 0) {
        return ERROR_SMIH;
      }
    }
  } else // adma2 mode
  {
    while (1) {
      if (event.transferComplete) {
        event.transferComplete = 0;
        break;
      }
      // check for error interrupt
      if (RSI_OK != smih_check_for_error_interrupt()) {
        return ERROR_TIMEOUT;
      }
      // wait until card inserts
      while (SMIH->SMIH_PRESENT_STATE_REGISTER_b.CARD_STATE_STABLE != 0x1)
        ;
      if (SMIH->SMIH_PRESENT_STATE_REGISTER_b.CARD_INSERTED == 0) {
        return ERROR_SMIH;
      }
    }
  }
  // Wait for data line to be free
  while ((SMIH->SMIH_PRESENT_STATE_REGISTER_b.DAT_LINE_ACTIVE) != 0)
    ;

  if ((SMIH->SMIH_PRESENT_STATE_REGISTER_b.DAT_LINE_ACTIVE) == 0x1) {
    return ERROR_SMIH;
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t smih_memory_command_transfer(SMIH_TRANSFER_T *pTransfer)
 * @brief        This API is used to send memory command.
 * @param[in]    pTransfer       :  Pointer to the command and data structure
 * @return       ERROR_SMIH      :  If Parameter is invalid.
 *               ERROR_TIMEOUT   :  If the command error timeout occures.
 *               RSI_OK          :  If command sent succesfully.
 */
error_t smih_memory_command_transfer(SMIH_TRANSFER_T *pTransfer)
{
  uint32_t admaDespTableAddress;
  SMIH_COMMAND_FRAME_CONFIG_T commandCfg = { 0 };

  if (NULL == modesConfig) {
    return INVALID_PARAMETERS;
  }
  // wait for command line to be stable
  while (SMIH->SMIH_PRESENT_STATE_REGISTER_b.COMMAND_INHIBIT_CMD != 0)
    ;

  // wait for data line to be stable
  while (SMIH->SMIH_PRESENT_STATE_REGISTER_b.COMMAND_INHIBIT_DAT != 0)
    ;

  // cnfigure adma2 descriptor table
  if ((modesConfig->admaMode) && (pTransfer->data->data != NULL)) {
    memset(Adma2DescriptorTable, 0x0, sizeof(Adma2DescriptorTable));

    Adma2DescriptorTable[0].attributeValid   = 1;
    Adma2DescriptorTable[0].attributeEnd     = 1;
    Adma2DescriptorTable[0].attributeInt     = 0;
    Adma2DescriptorTable[0].attributeAct     = 2;
    Adma2DescriptorTable[0].length           = (pTransfer->data->blockSize * pTransfer->data->blockCount);
    Adma2DescriptorTable[0]._32BIT_Adress    = (uint32_t)pTransfer->data->data;
    admaDespTableAddress                     = (uint32_t)&Adma2DescriptorTable[0];
    SMIH->SMIH_ADMA_SYSTEM_ADDRESS0_REGISTER = (uint16_t)admaDespTableAddress;
    SMIH->SMIH_ADMA_SYSTEM_ADDRESS1_REGISTER = (uint16_t)(admaDespTableAddress >> 16u);
  }
  SMIH->SMIH_BLOCK_SIZE_REGISTER_b.TRANSFER_BLOCK_SIZE          = pTransfer->data->blockSize;
  SMIH->TRANSFER_MODE_REGISTER_b.DATA_TRANSFER_DIRECTION_SELECT = pTransfer->data->direction;
  commandCfg.cmdArgument                                        = pTransfer->command->cmdArg;

  // Configure the CMD register to send the command.
  if (pTransfer->data->data == NULL) {
    commandCfg.dataPresentSelect = 0;
  } else {
    commandCfg.dataPresentSelect = 1;
  }
  if ((modesConfig->admaMode) && (pTransfer->data->data != NULL)) {
    SMIH->TRANSFER_MODE_REGISTER_b.DMA_ENABLE = 1;
  } else {
    SMIH->TRANSFER_MODE_REGISTER_b.DMA_ENABLE = 0;
  }
  commandCfg.cmdIndex = pTransfer->command->cmdIdx & 0x3F;
  if ((commandCfg.cmdIndex == 18) || (commandCfg.cmdIndex == 25)) {
    //  multiple block mode
    commandCfg.autoCmdType                                      = ENABLE_AUTO_CMD12;
    SMIH->TRANSFER_MODE_REGISTER_b.BLOCK_COUNT_ENABLE           = 0x1;
    SMIH->TRANSFER_MODE_REGISTER_b.MULTI_OR_SINGLE_BLOCK_SELECT = 0x1;
    SMIH->SMIH_BLOCK_COUNT_REGISTER                             = pTransfer->data->blockCount;
  } else {
    //  single block mode
    commandCfg.autoCmdType                                      = DISABLE_AUTO_CMD;
    SMIH->TRANSFER_MODE_REGISTER_b.BLOCK_COUNT_ENABLE           = 0x0;
    SMIH->TRANSFER_MODE_REGISTER_b.MULTI_OR_SINGLE_BLOCK_SELECT = 0x0;
    SMIH->SMIH_BLOCK_COUNT_REGISTER                             = pTransfer->data->blockCount;
  }
  switch (pTransfer->command->responseTypeSelect) {
    case SMIH_NO_RESPONSE:
      commandCfg.responseTypeSelect = SMIH_NO_RESPONSE;
      commandCfg.cmdIndexCheckEn    = 0;
      commandCfg.cmdCrcCheckEn      = 0;
      break;
    case SMIH_RESPONSE_R2:
      commandCfg.responseTypeSelect = SMIH_RESPONSE_LENGTH_136;
      commandCfg.cmdIndexCheckEn    = 0;
      commandCfg.cmdCrcCheckEn      = 1;
      break;
    case SMIH_RESPONSE_R3R4:
      commandCfg.responseTypeSelect = SMIH_RESPONSE_LENGTH_48;
      commandCfg.cmdIndexCheckEn    = 0;
      commandCfg.cmdCrcCheckEn      = 0;
      break;
    case SMIH_RESPONSE_R1R5R6R7:
      commandCfg.responseTypeSelect = SMIH_RESPONSE_LENGTH_48;
      commandCfg.cmdIndexCheckEn    = 1;
      commandCfg.cmdCrcCheckEn      = 1;
      break;
    case SMIH_RESPONSE_R1BR5B:
      commandCfg.responseTypeSelect = SMIH_RESPONSE_LENGTH_48BIT_BUSY_CHECK;
      commandCfg.cmdIndexCheckEn    = 1;
      commandCfg.cmdCrcCheckEn      = 1;
      break;
    default:
      return INVALID_PARAMETERS;
  }

  if (CommandInProgress != NULL) {
    return ERROR_OPERATION_INPROGRESS;
  }

  SMIH->SMIH_TIMEOUT_CONTROL_REGISTER_b.DATA_TIMEOUT_COUNTER_VALUE = 0xC;

  commandCfg.cmdType = NORMAL_CMD;

  // set current command to global value for callback
  CommandInProgress = pTransfer;

  // send command
  smih_command_xfer(&commandCfg);

  // wait command line to be stable
  while (1) {
    if (event.commandComplete) {
      event.commandComplete = 0;
      break;
    }
    // check for error interrupt
    if (RSI_OK != smih_check_for_error_interrupt()) {
      return ERROR_SMIH;
    }
    // wait until card inserts
    while (SMIH->SMIH_PRESENT_STATE_REGISTER_b.CARD_STATE_STABLE != 0x1)
      ;

    if (SMIH->SMIH_PRESENT_STATE_REGISTER_b.CARD_INSERTED == 0) {
      return ERROR_TIMEOUT;
    }
  }

  // get response data
  if (RSI_OK != smih_get_response((uint16_t *)pTransfer->command->response, sizeof(pTransfer->command->response) / 2)) {
    return ERROR_SMIH;
  }
  if (pTransfer->data->data != 0) {
    if ((pTransfer->command->cmdIdx == 24) || (pTransfer->command->cmdIdx == 25)) {
      // send data to card
      if (RSI_OK != smih_send_data(pTransfer)) {
        return ERROR_SMIH;
      }
    }
    if ((pTransfer->command->cmdIdx == 17) || (pTransfer->command->cmdIdx == 18)
        || (pTransfer->command->cmdIdx == (0x80 | 8) /*mmc command 8*/)
        || (pTransfer->command->cmdIdx == (0x40 + 51) /*sd ACMD51*/)) {
      /* read data from card */
      if (RSI_OK != smih_receive_data(pTransfer)) {
        return ERROR_SMIH;
      }
    }
  }
  CommandInProgress = NULL;
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t smih_io_command_transfer(SMIH_TRANSFER_T *pTransfer)
 * @brief        This API is used to transfer SMIH command.
 * @param[in]    pTransfer       : Pointer to the command and data structure
 * @return       ERROR_SMIH      : If Parameter is invalid.
 *               ERROR_TIMEOUT   : If the command error timeout occures.
 */
error_t smih_io_command_transfer(SMIH_TRANSFER_T *pTransfer)
{
  SMIH_COMMAND_FRAME_CONFIG_T commandCfg = { 0 };
  uint32_t admaDespTableAddress;
  if (NULL == modesConfig) {
    return INVALID_PARAMETERS;
  }
  // wait for command line to be stable
  while (SMIH->SMIH_PRESENT_STATE_REGISTER_b.COMMAND_INHIBIT_CMD != 0)
    ;

  // wait for data line to be stable
  while (SMIH->SMIH_PRESENT_STATE_REGISTER_b.COMMAND_INHIBIT_DAT != 0)
    ;

  // cnfigure adma2 descriptor table
  if ((modesConfig->admaMode) && (pTransfer->data->data != NULL)) {
    memset(Adma2DescriptorTable, 0x0, sizeof(Adma2DescriptorTable));
    Adma2DescriptorTable[0].attributeValid = 1;
    Adma2DescriptorTable[0].attributeEnd   = 1;
    Adma2DescriptorTable[0].attributeInt   = 0;
    Adma2DescriptorTable[0].attributeAct   = 2;
    Adma2DescriptorTable[0].length         = pTransfer->data->blockSize * pTransfer->data->blockCount;
    Adma2DescriptorTable[0]._32BIT_Adress  = (uint32_t)pTransfer->data->data;
    admaDespTableAddress                   = (uint32_t)&Adma2DescriptorTable[0];
    // Update descriptor address
    SMIH->SMIH_ADMA_SYSTEM_ADDRESS0_REGISTER = (uint16_t)admaDespTableAddress;
    SMIH->SMIH_ADMA_SYSTEM_ADDRESS1_REGISTER = (uint16_t)(admaDespTableAddress >> 16u);
  }
  // Configure block size
  SMIH->SMIH_BLOCK_SIZE_REGISTER_b.TRANSFER_BLOCK_SIZE          = pTransfer->data->blockSize;
  SMIH->TRANSFER_MODE_REGISTER_b.DATA_TRANSFER_DIRECTION_SELECT = pTransfer->data->direction;

  if ((modesConfig->admaMode) && (pTransfer->data->data != NULL)) {
    SMIH->TRANSFER_MODE_REGISTER_b.DMA_ENABLE = 1;
  } else {
    SMIH->TRANSFER_MODE_REGISTER_b.DMA_ENABLE = 0;
  }

  SMIH->SMIH_TIMEOUT_CONTROL_REGISTER_b.DATA_TIMEOUT_COUNTER_VALUE = 0xC;

  // Configure the CMD register to send the command.
  if (pTransfer->data->data == NULL) {
    commandCfg.dataPresentSelect = 0;
  } else {
    commandCfg.dataPresentSelect = 1;
  }
  commandCfg.cmdArgument = pTransfer->command->cmdArg;
  commandCfg.cmdIndex    = pTransfer->command->cmdIdx & 0x3F;

  if ((commandCfg.cmdIndex == 53) && (pTransfer->command->cmdArg & BIT(27))) {
    // multiple block mode
    SMIH->TRANSFER_MODE_REGISTER_b.BLOCK_COUNT_ENABLE           = 0x1;
    SMIH->TRANSFER_MODE_REGISTER_b.MULTI_OR_SINGLE_BLOCK_SELECT = 0x1;
    SMIH->SMIH_BLOCK_COUNT_REGISTER                             = pTransfer->data->blockCount;
  } else {
    // single block mode
    SMIH->TRANSFER_MODE_REGISTER_b.BLOCK_COUNT_ENABLE           = 0x0;
    SMIH->TRANSFER_MODE_REGISTER_b.MULTI_OR_SINGLE_BLOCK_SELECT = 0x0;
    SMIH->SMIH_BLOCK_COUNT_REGISTER                             = pTransfer->data->blockCount;
  }

  switch (pTransfer->command->responseTypeSelect) {
    case SMIH_NO_RESPONSE:
      commandCfg.responseTypeSelect = SMIH_NO_RESPONSE;
      commandCfg.cmdIndexCheckEn    = 0;
      commandCfg.cmdCrcCheckEn      = 0;
      break;
    case SMIH_RESPONSE_R2:
      commandCfg.responseTypeSelect = SMIH_RESPONSE_LENGTH_136;
      commandCfg.cmdIndexCheckEn    = 0;
      commandCfg.cmdCrcCheckEn      = 1;
      break;
    case SMIH_RESPONSE_R3R4:
      commandCfg.responseTypeSelect = SMIH_RESPONSE_LENGTH_48;
      commandCfg.cmdIndexCheckEn    = 0;
      commandCfg.cmdCrcCheckEn      = 0;
      break;
    case SMIH_RESPONSE_R1R5R6R7:
      commandCfg.responseTypeSelect = SMIH_RESPONSE_LENGTH_48;
      commandCfg.cmdIndexCheckEn    = 1;
      commandCfg.cmdCrcCheckEn      = 1;
      break;
    case SMIH_RESPONSE_R1BR5B:
      commandCfg.responseTypeSelect = SMIH_RESPONSE_LENGTH_48BIT_BUSY_CHECK;
      commandCfg.cmdIndexCheckEn    = 1;
      commandCfg.cmdCrcCheckEn      = 1;
      break;
    default:
      return INVALID_PARAMETERS;
  }

  commandCfg.autoCmdType = DISABLE_AUTO_CMD;

  if (CommandInProgress != NULL) {
    return ERROR_OPERATION_INPROGRESS;
  }
  commandCfg.cmdType = NORMAL_CMD;

  // set current command to global value for callback
  CommandInProgress = pTransfer;

  // send command
  smih_command_xfer(&commandCfg);

  // wait command line to be stable
  while (1) {
    if (event.commandComplete) {
      event.commandComplete = 0;
      break;
    }
    // check for error interrupt
    if (RSI_OK != smih_check_for_error_interrupt()) {
      return ERROR_SMIH;
    }

    // wait until card inserts
    while (SMIH->SMIH_PRESENT_STATE_REGISTER_b.CARD_STATE_STABLE != 0x1)
      ;
    if (SMIH->SMIH_PRESENT_STATE_REGISTER_b.CARD_INSERTED == 0) {
      return ERROR_TIMEOUT;
    }
  }
  // get response data
  if (RSI_OK != smih_get_response((uint16_t *)pTransfer->command->response, sizeof(pTransfer->command->response) / 2)) {
    return ERROR_SMIH;
  }
  if (pTransfer->data->data != 0) {
    if (pTransfer->command->cmdIdx == 53) {
      if ((pTransfer->command->cmdArg) & BIT(31)) {
        // send data to card
        if (RSI_OK != smih_send_data(pTransfer)) {
          return ERROR_SMIH;
        }
      } else {
        // read data from card
        if (RSI_OK != smih_receive_data(pTransfer)) {
          return ERROR_SMIH;
        }
      }
    }
  }
  CommandInProgress = NULL;
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           void RegisterCallBack(ARM_SMIH_SignalEvent_t Event)
 * @brief        This API is used to register the call back handler
 * @param[in]    Event     : Call back handler to register
 * @return       none
 */
void RegisterCallBack(ARM_SMIH_SignalEvent_t Event)
{
  event.callb_event = Event;
}
#endif
