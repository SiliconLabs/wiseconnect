/*******************************************************************************
* @file  rsi_sdioh.c
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
#include "clock_update.h"

/*==============================================*/
/**
 * @fn           error_t RSI_SDIOH_RegisterInfo(SMIH_CARD_CONFIG_T *pSmihConfig, SMIH_CCCR_REG_INFO_T *pRegInfo)
 * @brief        This API is used to know the register info of the card.
 * @param[in]    pSmihConfig : Pointer to the card command information structure
 * @param[in]    pRegInfo    : Pointer to the Command info structure 
 * @return       RSI_OK      : command sent succesfully
 *               ERROR_SMIH  : If Parameter is invalid. 
 */
error_t RSI_SDIOH_RegisterInfo(SMIH_CARD_CONFIG_T *pSmihConfig, SMIH_CCCR_REG_INFO_T *pRegInfo)
{
  SMIH_TRANSFER_T transfer = { 0 };
  SMIH_CMD_FEILD_T cmd     = { 0 };
  SMIH_DATA_FEILD_T data   = { 0 };
  uint8_t cccr[22]         = { 0 };

  cmd.cmdIdx             = CMD_53;
  cmd.cmdArg             = CCCR_BYTE_READ;
  cmd.responseTypeSelect = SMIH_RESPONSE_R1R5R6R7;
  data.data              = cccr;
  data.blockSize         = 0x16;
  data.blockCount        = 1;
  cmd.responseTypeSelect = SMIH_RESPONSE_R1R5R6R7;
  data.direction         = SMIH_READ_DIRECTION;

  transfer.command = &cmd;
  transfer.data    = &data;

  // transfer command to the slave
  if (RSI_OK != pSmihConfig->cmd_transfer(&transfer)) {
    return ERROR_SMIH;
  }
  memset((void *)pRegInfo, 0x0, 22);
  memcpy((void *)pRegInfo, &transfer.data->data, 22);

  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_SDIOH_WriteCommandCmd52(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Argument)
 * @brief        This API is used to send the SDIO_CMD52 to the card.
 * @param[in]    pSmihConfig    : Pointer to the card command information structure
 * @param[in]    Argument       : Argument to the command
 * @return       RSI_OK         : command sent succesfully
 *               ERROR_SMIH     : If Parameter is invalid.
 *               ERROR_TIMEOUT  : If the command error timeout occures.
 */
error_t RSI_SDIOH_WriteCommandCmd52(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Argument)
{
  SMIH_TRANSFER_T transfer = { 0 };
  SMIH_CMD_FEILD_T cmd     = { 0 };
  SMIH_DATA_FEILD_T data   = { 0 };

  cmd.cmdIdx             = CMD_52;
  cmd.cmdArg             = Argument;
  cmd.responseTypeSelect = SMIH_RESPONSE_R1R5R6R7;
  data.data              = 0;
  data.blockSize         = 0;
  data.blockCount        = 0;
  data.direction         = SMIH_WRITE_DIRECTION;

  transfer.command = &cmd;
  transfer.data    = &data;

  memset((void *)&pSmihConfig->response[0], 0x0, sizeof(pSmihConfig->response));

  // transfer command
  if (RSI_OK != pSmihConfig->cmd_transfer(&transfer)) {
    return ERROR_SMIH;
  } else {
    memcpy((void *)&pSmihConfig->response[0], &transfer.command->response[0], sizeof(pSmihConfig->response));
    return RSI_OK;
  }
}

/*==============================================*/
/**
 * @fn           error_t RSI_SDIOH_ReadCommandCmd52(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Argument)     
 * @brief        This API is used to send the SDIO_CMD52 to the card.
 * @param[in]    pcmdInfo      : Pointer to the card information strut
 * @param[in]    Argument      : Argument to the command
 * @return       RSI_OK        : In case of command sent succesfully
 *               ERROR_SMIH    : If Parameter is invalid.
 *               ERROR_TIMEOUT : If the command error timeout occures.
 */
error_t RSI_SDIOH_ReadCommandCmd52(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Argument)
{
  SMIH_TRANSFER_T transfer = { 0 };
  SMIH_CMD_FEILD_T cmd     = { 0 };
  SMIH_DATA_FEILD_T data   = { 0 };

  cmd.cmdIdx             = CMD_52;
  cmd.cmdArg             = Argument;
  cmd.responseTypeSelect = SMIH_RESPONSE_R1R5R6R7;
  data.data              = 0;
  data.blockSize         = 0;
  data.blockCount        = 0;
  data.direction         = SMIH_READ_DIRECTION;

  transfer.command = &cmd;
  transfer.data    = &data;

  // transfer command
  if (RSI_OK != pSmihConfig->cmd_transfer(&transfer)) {
    return ERROR_SMIH;
  } else {
    memset((void *)&pSmihConfig->response[0], 0x0, sizeof(pSmihConfig->response));
    memcpy((void *)&pSmihConfig->response[0], &transfer.command->response[0], sizeof(pSmihConfig->response));
    return RSI_OK;
  }
}

/*==============================================*/
/**
 * @fn           error_t RSI_SDIOH_SetBusWidthCmd52(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t CmdArg)
 * @brief        This API is used to send the SDIO_CMD52 to set bus width to the IO card.
 * @param[in]    pSmihConfig      :  Pointer to the card info struct
 * @param[in]    CmdArg           :  Command Argument
 * @return       RSI_OK           :  If the command sent successfully
 *               ERROR_TIMEOUT    :  If the command error timeout occures.
 */
error_t RSI_SDIOH_SetBusWidthCmd52(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t CmdArg)
{
  SMIH_TRANSFER_T transfer = { 0 };
  SMIH_CMD_FEILD_T cmd     = { 0 };
  SMIH_DATA_FEILD_T data   = { 0 };

  cmd.cmdIdx             = CMD_52;
  cmd.cmdArg             = CmdArg;
  data.data              = NULL;
  data.blockSize         = 0;
  data.blockCount        = 0;
  cmd.responseTypeSelect = SMIH_RESPONSE_R1R5R6R7;
  data.direction         = SMIH_WRITE_DIRECTION;

  transfer.command = &cmd;
  transfer.data    = &data;

  // transfer Command
  if (RSI_OK != pSmihConfig->cmd_transfer(&transfer)) {
    return ERROR_SMIH;
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_SDIOH_BusWidthConfig(SMIH_CARD_CONFIG_T *pSmihConfig, uint8_t BusWidth)
 * @brief        This API is used to change the bus width.
 * @param[in]    pcmdInfo       :  Pointer to the card information strut
 * @param[in]    BusWidth       :  Width of the bus
 *               \n                possible values are
 *               \n                SMIH_BUS_WIDTH1     = 0u,
 *               \n                SMIH_BUS_WIDTH4     = 1u,
 *               \n                SMIH_BUS_WIDTH8     = 2u,
 * @return       RSI_OK         :  If  bus width set properly
 *               ERROR_SMIH     :  If Parameter is invalid.
 */
error_t RSI_SDIOH_BusWidthConfig(SMIH_CARD_CONFIG_T *pSmihConfig, uint8_t BusWidth)
{
  uint32_t cmdArg        = 0;
  boolean_t host4BitMode = FALSE;

  if (BusWidth == SMIH_BUS_WIDTH4) {
    cmdArg       = BIT4_BUS_WIDTH_ARG;
    host4BitMode = TRUE;
  } else {
    cmdArg       = 0;
    host4BitMode = FALSE;
  }
  if (RSI_OK != RSI_SDIOH_SetBusWidthCmd52(pSmihConfig, cmdArg)) {
    return ERROR_SMIH;
  } else {
    // host side setting
    smih_bus_width_set(host4BitMode);
    return RSI_OK;
  }
}

/*==============================================*/
/**
 * @fn           error_t RSI_SDIOH_GoIdleStateCmd0(SMIH_CARD_CONFIG_T *pSmihConfig)
 * @brief        This API is used to send the cmd0 to the io card.
 * @param[in]    pSmihConfig      :  Pointer to the Command info structure
 * @return       RSI_OK           :  If  command sent successfully
 *               ERROR_TIMEOUT    :  If  command error timeout occures.
 */
error_t RSI_SDIOH_GoIdleStateCmd0(SMIH_CARD_CONFIG_T *pSmihConfig)
{
  SMIH_TRANSFER_T transfer = { 0 };
  SMIH_CMD_FEILD_T cmd     = { 0 };
  SMIH_DATA_FEILD_T data   = { 0 };

  cmd.cmdIdx             = 0;
  cmd.cmdArg             = 0x0;
  cmd.responseTypeSelect = SMIH_NO_RESPONSE;
  data.data              = NULL;
  data.blockSize         = 0x0;
  data.blockCount        = 0x0;
  data.direction         = SMIH_WRITE_DIRECTION;

  transfer.command = &cmd;
  transfer.data    = &data;

  // transfer Command
  if (RSI_OK != pSmihConfig->cmd_transfer(&transfer)) {
    return ERROR_SMIH;
  }
  return RSI_OK;
}
/*==============================================*/
/**
 * @fn           error_t RSI_SDIOH_SendRelativeCardAddressCmd3(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t CmdArg)
 * @brief        This API is used to send the SDIO_CMD3(Get relative card address) to the IO card.
 * @param[in]    pSmihConfig     :  Pointer to the card info struct
 * @param[in]    CmdArg          :  Command argument to send
 * @return       RSI_OK          :  If command sent successfully
 *               ERROR_TIMEOUT   :  If command error timeout occures.
 */
error_t RSI_SDIOH_SendRelativeCardAddressCmd3(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t CmdArg)
{
  SMIH_TRANSFER_T transfer = { 0 };
  SMIH_CMD_FEILD_T cmd     = { 0 };
  SMIH_DATA_FEILD_T data   = { 0 };

  cmd.cmdIdx             = CMD_3;
  cmd.cmdArg             = CmdArg;
  cmd.responseTypeSelect = SMIH_RESPONSE_R1R5R6R7;
  data.data              = NULL;
  data.blockSize         = 0;
  data.blockCount        = 0;
  data.direction         = SMIH_WRITE_DIRECTION;

  transfer.command = &cmd;
  transfer.data    = &data;

  // transfer Command
  if (RSI_OK != pSmihConfig->cmd_transfer(&transfer)) {
    return ERROR_SMIH;
  }
  if (CmdArg == 0x0) {
    pSmihConfig->rca = transfer.command->response[0] >> 16;
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_SDIOH_SwitchVoltageCmd11(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t CmdArg)
 * @brief        This API is used to send the SDIO_CMD11(Voltage switch command in case of uhs modes) to the io card.
 * @param[in]    pSmihConfig      :  Pointer to the card info struct
 * @param[in]    CmdArg           :  Command argument to send
 * @return       RSI_OK           :  If  command sent successfully
 *               ERROR_TIMEOUT    :  If the command error timeout occures.
 */
error_t RSI_SDIOH_SwitchVoltageCmd11(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t CmdArg)
{
  SMIH_TRANSFER_T transfer = { 0 };
  SMIH_CMD_FEILD_T cmd     = { 0 };
  SMIH_DATA_FEILD_T data   = { 0 };

  cmd.cmdIdx             = CMD_11;
  cmd.cmdArg             = 0;
  cmd.responseTypeSelect = SMIH_RESPONSE_R1R5R6R7;
  data.data              = NULL;
  data.blockSize         = 0;
  data.blockCount        = 0;
  data.direction         = SMIH_WRITE_DIRECTION;

  transfer.command = &cmd;
  transfer.data    = &data;

  // transfer Command
  if (RSI_OK != pSmihConfig->cmd_transfer(&transfer)) {
    return ERROR_SMIH;
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_SDIOH_SelectCardCmd7(SMIH_CARD_CONFIG_T *pSmihConfig)
 * @brief        This API is used to send the cmd 7(select the cards) to the IO card.
 * @param[in]    pSmihConfig     : Pointer to the Command info structure
 * @return       RSI_OK          : If the command sent successfully
 *               ERROR_TIMEOUT   : If the command error timeout occures.
 */
error_t RSI_SDIOH_SelectCardCmd7(SMIH_CARD_CONFIG_T *pSmihConfig)
{
  SMIH_TRANSFER_T transfer = { 0 };
  SMIH_CMD_FEILD_T cmd     = { 0 };
  SMIH_DATA_FEILD_T data   = { 0 };

  cmd.cmdIdx             = CMD_7;
  cmd.cmdArg             = pSmihConfig->rca << 16;
  cmd.responseTypeSelect = SMIH_RESPONSE_R1BR5B;
  data.data              = NULL;
  data.blockSize         = 0;
  data.blockCount        = 0;
  data.direction         = SMIH_WRITE_DIRECTION;

  transfer.command = &cmd;
  transfer.data    = &data;

  // transfer Command
  if (RSI_OK != pSmihConfig->cmd_transfer(&transfer)) {
    return ERROR_SMIH;
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_SDIOH_ByteBlockWriteCmd53(SMIH_CARD_CONFIG_T *pSmihConfig, uint8_t *pData, uint32_t Addr)
 * @brief        This API is used to send the SDIO_CMD53 to write data to the IO card.
 * @param[in]    pSmihConfig        : Pointer to the smih config struct
 * @param[in]    pData              : Pointer to the buffer data to write
 * @param[in]    Addr               : Address to write the data
 * @return       RSI_OK             : If the command sent successfully
 *               ERROR_TIMEOUT      : If the command error timeout occured.
 */
error_t RSI_SDIOH_ByteBlockWriteCmd53(SMIH_CARD_CONFIG_T *pSmihConfig, uint8_t *pData, uint32_t Addr)
{
  SMIH_TRANSFER_T transfer = { 0 };
  SMIH_CMD_FEILD_T cmd     = { 0 };
  SMIH_DATA_FEILD_T data   = { 0 };

  if ((NULL == pData)) {
    return INVALID_PARAMETERS;
  }
  cmd.cmdIdx = CMD_53;
  if (pSmihConfig->blockModeEnable) {
    cmd.cmdArg = (pSmihConfig->numberOfBlocks);
  } else {
    cmd.cmdArg = (pSmihConfig->byteBlockSize);
  }
  cmd.cmdArg |= (((Addr)&0x0001FFFF) << 9);
  cmd.cmdArg |= (pSmihConfig->opCode << 26);
  cmd.cmdArg |= (pSmihConfig->blockModeEnable << 27);
  cmd.cmdArg |= (pSmihConfig->funcNum << 28);
  cmd.cmdArg |= BIT(31);
  cmd.responseTypeSelect = SMIH_RESPONSE_R1R5R6R7;
  data.data              = pData;
  data.blockSize         = pSmihConfig->byteBlockSize;
  data.blockCount        = pSmihConfig->numberOfBlocks;
  data.direction         = SMIH_WRITE_DIRECTION;

  transfer.command = &cmd;
  transfer.data    = &data;

  // transfer Command
  if (RSI_OK != pSmihConfig->cmd_transfer(&transfer)) {
    return ERROR_SMIH;
  }
  return RSI_OK;
}
/*==============================================*/
/**
 * @fn           error_t RSI_SDIOH_ByteBlockReadCmd53(SMIH_CARD_CONFIG_T *pSmihConfig, uint8_t *pData, uint32_t Addr)
 * @brief        This API is used to send the SDIO_CMD53 to read data from the IO card.
 * @param[in]    pSmihConfig        :  Pointer to the smih config struct
 * @param[in]    pData              :  Pointer to the buffer data to read
 * @param[in]    Addr               :  Address to read the data
 * @return       RSI_OK             :  If the command sent successfully
 *               ERROR_TIMEOUT      :  If the command error timeout occures.
 */
error_t RSI_SDIOH_ByteBlockReadCmd53(SMIH_CARD_CONFIG_T *pSmihConfig, uint8_t *pData, uint32_t Addr)
{
  SMIH_TRANSFER_T transfer = { 0 };
  SMIH_CMD_FEILD_T cmd     = { 0 };
  SMIH_DATA_FEILD_T data   = { 0 };

  cmd.cmdIdx = CMD_53;
  if (pSmihConfig->blockModeEnable == 1) {
    cmd.cmdArg = (pSmihConfig->numberOfBlocks);
  } else {
    cmd.cmdArg = (pSmihConfig->byteBlockSize);
  }
  cmd.cmdArg |= (((Addr)&0x0001FFFF) << 9);
  cmd.cmdArg |= (pSmihConfig->opCode << 26);
  cmd.cmdArg |= (pSmihConfig->blockModeEnable << 27);
  cmd.cmdArg |= (pSmihConfig->funcNum << 28);
  cmd.cmdArg |= (((0 << 0)) << 31);
  cmd.responseTypeSelect = SMIH_RESPONSE_R1R5R6R7;
  data.data              = pData;
  data.blockSize         = pSmihConfig->byteBlockSize;
  data.blockCount        = pSmihConfig->numberOfBlocks;
  data.direction         = SMIH_READ_DIRECTION;

  transfer.command = &cmd;
  transfer.data    = &data;

  // transfer Command
  if (RSI_OK != pSmihConfig->cmd_transfer(&transfer)) {
    return ERROR_SMIH;
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_SDIOH_ReInitializationCmd5(SMIH_CARD_CONFIG_T *pSmihConfig)
 * @brief        This API is used to send the SDIO_CMD5 to reinitialize the SDIO card.
 * @param[in]    pSmihConfig      :  Pointer to the smih config struct
 * @return       RSI_OK           :  If the command sent successfully
 *               ERROR_TIMEOUT    :  If the command error timeout occures.
 */
error_t RSI_SDIOH_ReInitializationCmd5(SMIH_CARD_CONFIG_T *pSmihConfig)
{
  SMIH_TRANSFER_T transfer = { 0 };
  SMIH_CMD_FEILD_T cmd     = { 0 };
  SMIH_DATA_FEILD_T data   = { 0 };

  cmd.cmdIdx = CMD_5;
  cmd.cmdArg = *(uint32_t *)(&pSmihConfig->ocr);
  if (pSmihConfig->voltage == 1) {
    cmd.cmdArg |= 0x01100000;
  } else {
    cmd.cmdArg |= 0x00100000;
  }
  cmd.responseTypeSelect = SMIH_RESPONSE_R3R4;
  data.data              = NULL;
  data.blockSize         = 0;
  data.blockCount        = 0;
  data.direction         = SMIH_WRITE_DIRECTION;

  transfer.command = &cmd;
  transfer.data    = &data;

  // gtransfer Command
  if (RSI_OK == pSmihConfig->cmd_transfer(&transfer)) {
    memcpy((void *)&pSmihConfig->ocr, &transfer.command->response[0], sizeof(pSmihConfig->ocr));
    return RSI_OK;
  }
  return ERROR_SMIH;
}

/*==============================================*/
/**
 * @fn           error_t RSI_SDIOH_InitializationCmd5(SMIH_CARD_CONFIG_T *pSmihConfig)
 * @brief        This API is used to send the SDIO_CMD5 to initialize the SDIO card.
 * @param[in]    pSmihConfig       :  Pointer to the smih card information struct
 * @return       RSI_OK            :  If the command sent successfully
 *               ERROR_TIMEOUT     :  If the command error timeout occures.
 */
error_t RSI_SDIOH_InitializationCmd5(SMIH_CARD_CONFIG_T *pSmihConfig)
{
  SMIH_TRANSFER_T transfer = { 0 };
  SMIH_CMD_FEILD_T cmd     = { 0 };
  SMIH_DATA_FEILD_T data   = { 0 };

  cmd.cmdIdx             = CMD_5;
  cmd.cmdArg             = *(uint32_t *)&pSmihConfig->ocr;
  cmd.cmdArg             = (cmd.cmdArg & 0x00000000);
  cmd.responseTypeSelect = SMIH_RESPONSE_R3R4;
  data.data              = NULL;
  data.blockSize         = 0;
  data.blockCount        = 0;
  data.direction         = SMIH_WRITE_DIRECTION;

  transfer.command = &cmd;
  transfer.data    = &data;

  // transfer Command
  if (RSI_OK == pSmihConfig->cmd_transfer(&transfer)) {
    memcpy((void *)&pSmihConfig->ocr, &transfer.command->response[0], sizeof(pSmihConfig->ocr));
    return RSI_OK;
  }
  return ERROR_SMIH;
}

/*==============================================*/
/**
 * @fn           void RSI_SDIOH_PinMux(void)
 * @brief        This API is used to configure GPIOs for SDIOH operations.
 * @return       none
 */
void RSI_SDIOH_PinMux(void)
{
  // enable pads for pins
#if SDIO_SET1
  RSI_EGPIO_PadSelectionEnable(SDIO_CLK_PAD);
  RSI_EGPIO_PadSelectionEnable(SDIO_CMD_PAD);
  RSI_EGPIO_PadSelectionEnable(SDIO_D0_PAD);
  RSI_EGPIO_PadSelectionEnable(SDIO_D1_PAD);
  RSI_EGPIO_PadSelectionEnable(SDIO_D2_PAD);
  RSI_EGPIO_PadSelectionEnable(SDIO_D3_PAD);
#else
  // Enable Host Pad Gpio modes
  // SDIO Connected to Tass
  (*(volatile uint32_t *)(0x41300004)) = (0x1 << 5);
  RSI_EGPIO_HostPadsGpioModeEnable(SDIO_CLK_PIN);
  RSI_EGPIO_HostPadsGpioModeEnable(SDIO_CMD_PIN);
  RSI_EGPIO_HostPadsGpioModeEnable(SDIO_D0_PIN);
  RSI_EGPIO_HostPadsGpioModeEnable(SDIO_D1_PIN);
  RSI_EGPIO_HostPadsGpioModeEnable(SDIO_D2_PIN);
  RSI_EGPIO_HostPadsGpioModeEnable(SDIO_D3_PIN);
#endif

  RSI_EGPIO_PadSelectionEnable(SDIO_WP_PAD);
  RSI_EGPIO_PadSelectionEnable(SDIO_CD_PAD);

  // Ren enables for Gpios
  RSI_EGPIO_PadReceiverEnable(SDIO_CLK_PIN);
  RSI_EGPIO_PadReceiverEnable(SDIO_CMD_PIN);
  RSI_EGPIO_PadReceiverEnable(SDIO_D0_PIN);
  RSI_EGPIO_PadReceiverEnable(SDIO_D1_PIN);
  RSI_EGPIO_PadReceiverEnable(SDIO_D2_PIN);
  RSI_EGPIO_PadReceiverEnable(SDIO_D3_PIN);
  RSI_EGPIO_PadReceiverEnable(SDIO_CD_PIN);
  RSI_EGPIO_PadReceiverEnable(SDIO_WP_PIN);

  // Configure software pull ups for cmd ,d0,d1,d2,d3
  RSI_EGPIO_PadDriverDisableState(SDIO_CMD_PIN, Pullup);
  RSI_EGPIO_PadDriverDisableState(SDIO_D0_PIN, Pullup);
  RSI_EGPIO_PadDriverDisableState(SDIO_D1_PIN, Pullup);
  RSI_EGPIO_PadDriverDisableState(SDIO_D2_PIN, Pullup);
  RSI_EGPIO_PadDriverDisableState(SDIO_D3_PIN, Pullup);

  RSI_EGPIO_PadDriverDisableState(SDIO_WP_PIN, Pulldown);
  RSI_EGPIO_PadDriverDisableState(SDIO_CD_PIN, Pulldown);

  // Configure Mux
  RSI_EGPIO_SetPinMux(EGPIO, 0, SDIO_CLK_PIN, SDIO_CLK_MUX);
  RSI_EGPIO_SetPinMux(EGPIO, 0, SDIO_CMD_PIN, SDIO_CMD_MUX);
  RSI_EGPIO_SetPinMux(EGPIO, 0, SDIO_D0_PIN, SDIO_D0_MUX);
  RSI_EGPIO_SetPinMux(EGPIO, 0, SDIO_D1_PIN, SDIO_D1_MUX);
  RSI_EGPIO_SetPinMux(EGPIO, 0, SDIO_D2_PIN, SDIO_D2_MUX);
  RSI_EGPIO_SetPinMux(EGPIO, 0, SDIO_D3_PIN, SDIO_D3_MUX);
  RSI_EGPIO_SetPinMux(EGPIO, 0, SDIO_CD_PIN, SDIO_CD_MUX);
  RSI_EGPIO_SetPinMux(EGPIO, 0, SDIO_WP_PIN, SDIO_WP_MUX);
}

/*==============================================*/
/**
 * @fn           error_t RSI_SDIOH_Enumeration(SMIH_CARD_CONFIG_T *pSmihConfig, ARM_SMIH_SignalEvent_t Event)
 * @brief        This API is used to initialize the IO card
 * @param[in]    pSmihConfig           : Pointer to the smih card configuration
 *               Event                 : event handler to be register
 * @return       RSI_OK                : IO card initialized successfully
 *               INVALID_PARAMETERS    : if  pSmihConfig == NULL
 */
error_t RSI_SDIOH_Enumeration(SMIH_CARD_CONFIG_T *pSmihConfig, ARM_SMIH_SignalEvent_t Event)
{
  if (pSmihConfig == 0) {
    return INVALID_PARAMETERS;
  }

  // Configure gpios in smih mode
  RSI_SDIOH_PinMux();

  // initialize the host
  if (RSI_OK != RSI_SD_HostInit(pSmihConfig, Event, 0)) {
    return ERROR_SMIH;
  } else {
    // Commands for sdio enumeration

    rsi_delay_ms(5);

init:
    // Reset Card
    if (RSI_OK != RSI_SDIOH_GoIdleStateCmd0(pSmihConfig)) {
      goto init;
    }
    // Get Operational voltage
    if (RSI_OK != RSI_SDIOH_InitializationCmd5(pSmihConfig)) {
      goto init;
    }
    // Set Operational voltage and get ocr
    if (RSI_OK != RSI_SDIOH_ReInitializationCmd5(pSmihConfig)) {
      return ERROR_SMIH;
    }
    if (!(pSmihConfig->ocr.cardReady)) {
      goto init;
    }
    if ((pSmihConfig->ocr.memPresent)) {
      return CARD_TYPE_MEMCARD;
    }
    if (!(pSmihConfig->ocr.ioNum)) {
      return ERROR_INAVLID_MODE;
    }
    if (pSmihConfig->highSpeedEnable == 1) {
      // disable clock to the sd
      M4CLK->SD_MEM_CLOCK_REG_b.SD_MEM_INTF_CLK_SEL = 0x7;

      // wait for some time
      rsi_delay_ms(5);

      SMIH->SMIH_HOST_CONTROL_1_REGISTER_b.HIGH_SPEED_ENABLE = 0x1;

      // enable clock
      RSI_CLK_SdMemClkConfig(M4CLK, 1, SDMEM_SOCPLLCLK, 1);

      // wait for some time
      rsi_delay_ms(2);
    }
    if (pSmihConfig->uhsModeSelect != 0) {
      // works only with 1.8 volatge level
      if (!(pSmihConfig->ocr.s18A)) {
        return ERROR_INAVLID_MODE;
      }
      // send volatge switching command in case of uhs modes
      if (RSI_OK != RSI_SDIOH_SwitchVoltageCmd11(pSmihConfig, 0x0)) {
        return ERROR_SMIH;
      }
      //disable clock to the sd
      M4CLK->SD_MEM_CLOCK_REG_b.SD_MEM_INTF_CLK_SEL = 0x7;

      // wait for some time
      rsi_delay_ms(5);

      // enable 1.8 volt bit in controller
      smih_18v_signal_enable();

      // select uhs mode
      smih_uhs_mode_select(pSmihConfig->uhsModeSelect);

      // enable clock
      RSI_CLK_SdMemClkConfig(M4CLK, 1, SDMEM_SOCPLLCLK, 1);

      // wait for some time
      rsi_delay_ms(2);
    }

    // Get relative card address
    if (RSI_OK != RSI_SDIOH_SendRelativeCardAddressCmd3(pSmihConfig, 0x0)) {
      return ERROR_SMIH;
    }

    // Select Card (send cmd7) :send rca as argument
    if (RSI_OK != RSI_SDIOH_SelectCardCmd7(pSmihConfig)) {
      return ERROR_SMIH;
    }
    if (RSI_OK != RSI_SDIOH_WriteCommandCmd52(pSmihConfig, CD_DISABLE_ARG)) {
      return ERROR_SMIH;
    }
bus_send_again:

    if (RSI_OK != RSI_SDIOH_WriteCommandCmd52(pSmihConfig, LOW_SPEED_CHECK_ARG)) {
      return ERROR_SMIH;
    }
    if (pSmihConfig->response[0] & LOW_SPEED_CHECK) {
      if (pSmihConfig->response[0] & BIT4_MODE_CHECK) {
        if (SMIH_BUS_WIDTH4 == pSmihConfig->busWidthMode) {
          // configure 4bit mode
          RSI_SDIOH_BusWidthConfig(pSmihConfig, SMIH_BUS_WIDTH4);
        }
      } else {
      }
    } else {
      if (SMIH_BUS_WIDTH4 == pSmihConfig->busWidthMode) {
        if (RSI_OK != RSI_SDIOH_BusWidthConfig(pSmihConfig, SMIH_BUS_WIDTH4)) {
          return ERROR_SMIH;
        }
      }
    }
    while (1) {
      // Enable function1
      if (RSI_OK == RSI_SDIOH_WriteCommandCmd52(pSmihConfig, FUCNTION1_CHECK_ARG)) {
        if (!((pSmihConfig->response[0]) & FUNCTION1_ENABLE)) {
          if (RSI_OK != RSI_SDIOH_WriteCommandCmd52(pSmihConfig, FUCNTION1_ENB_ARG)) {
            return ERROR_SMIH;
          } else {
            break;
          }
        } else
          break;
      }
    }
    while (1) {
      // Check for function ready
      if (RSI_OK == RSI_SDIOH_WriteCommandCmd52(pSmihConfig, FUCNTION1_READY_ARG)) {
        if (pSmihConfig->response[0] & FUNCTION1_READY) {
          break;
        }
      }
    }
    // Interrupt Enable
    if (RSI_OK != RSI_SDIOH_WriteCommandCmd52(pSmihConfig, FUNCTION1_INTR_ENB_ARG)) {
      return ERROR_SMIH;
      ;
    }
    // Check interrupts are enabled or not
    if (RSI_OK != RSI_SDIOH_WriteCommandCmd52(pSmihConfig, FUNCTION1_INTR_CHECK_ARG)) {
      return ERROR_SMIH;
    }
    // Check for 1bit or 4bit mode of I/O
    if (RSI_OK != RSI_SDIOH_WriteCommandCmd52(pSmihConfig, BIT4_BUS_WIDTH_SET_ARG)) {
      return ERROR_SMIH;
    }
    if (SMIH_BUS_WIDTH4 == pSmihConfig->busWidthMode) {
      if (!(pSmihConfig->response[0] & BUS_BIT)) {
        goto bus_send_again;
      }
    }
    // Check for CD Disable
    if (!(pSmihConfig->response[0] & (1 << 7))) {
      return RSI_OK;
    }
    if (RSI_OK == RSI_SDIOH_WriteCommandCmd52(pSmihConfig, CSA_SUPPORT_ARG)) {
      if (pSmihConfig->response[0] & (1 << 6)) {
        if (RSI_OK != RSI_SDIOH_WriteCommandCmd52(pSmihConfig, CSA_ENABLE_ARG)) {
          return ERROR_SMIH;
        }
      }
      if (RSI_OK != RSI_SDIOH_WriteCommandCmd52(pSmihConfig, IO_BLOCKSIZE_ARG)) {
        return ERROR_SMIH;
      }
      if (RSI_OK != RSI_SDIOH_WriteCommandCmd52(pSmihConfig, IO_BLOCKSIZE_ARG_1)) {
        return ERROR_SMIH;
      }
    }
    // High speed mode configuration
    if (pSmihConfig->highSpeedEnable) {
      while (1) {
        if (RSI_OK != RSI_SDIOH_WriteCommandCmd52(pSmihConfig, CHECK_HIGH_SPEED_SUPPORT)) {
          return ERROR_SMIH;
        }

        if ((pSmihConfig->response[0] & HIGH_SPEED_BIT)) {
          break;
        }
      }
      // high speed mode switching command
      if (RSI_OK != RSI_SDIOH_WriteCommandCmd52(pSmihConfig, ENABLE_HIGH_SPEED_MODE_ARG)) {
        return ERROR_SMIH;
      }
    }
    // uhs mode configuration
    if (pSmihConfig->uhsModeSelect != 0) {
      while (1) {
        // ask card about uhs support modes capability
        if (RSI_OK != RSI_SDIOH_WriteCommandCmd52(pSmihConfig, CHECK_UHS_SUPPORT_MODES)) {
          return ERROR_SMIH;
        }
        if ((pSmihConfig->response[0] & UHS_SUPPORT_BITS)) {
          // card supports uhs modes
          break;
        }
      }
      // Send UHS mode to the slave
      switch (pSmihConfig->uhsModeSelect) {
        // configure uhs modes
        case UHS_SDR12:
          break;
        case UHS_SDR25:
          if (RSI_OK != RSI_SDIOH_WriteCommandCmd52(pSmihConfig, UHS_1_SDR25_MODE_ARG)) {
            return ERROR_SMIH;
          }
          break;
        case UHS_SDR50:
          if (RSI_OK != RSI_SDIOH_WriteCommandCmd52(pSmihConfig, UHS_1_SDR50_MODE_ARG)) {
            return ERROR_SMIH;
          }
          break;
        case UHS_SDR104:
          if (RSI_OK != RSI_SDIOH_WriteCommandCmd52(pSmihConfig, UHS_1_SDR104_MODE_ARG)) {
            return ERROR_SMIH;
          }
          break;
        case UHS_DDR50:
          if (RSI_OK != RSI_SDIOH_WriteCommandCmd52(pSmihConfig, UHS_1_DDR50_MODE_ARG)) {
            return ERROR_SMIH;
          }
          break;
        default:
          return INVALID_PARAMETERS;
      }
    }
  }
  return RSI_OK;
}
#endif
