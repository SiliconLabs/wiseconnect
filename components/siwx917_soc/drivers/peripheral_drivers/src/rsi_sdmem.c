/*******************************************************************************
* @file  rsi_sdmem.c
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
 * @fn           error_t RSI_SDMEM_GoIdleStateCmd0(SMIH_CARD_CONFIG_T *pSmihConfig)
 * @brief        This API is used to send the CMD0 to the memory card.
 * @param[in]    pSmihConfig        :   Pointer to the smih config structure.
 * @return       RSI_OK             :   If the command sent successfully
 *               ERROR_TIMEOUT      :   If the command error timeout occures.
 */
error_t RSI_SDMEM_GoIdleStateCmd0(SMIH_CARD_CONFIG_T *pSmihConfig)
{
  SMIH_TRANSFER_T transfer = { 0 };
  SMIH_CMD_FEILD_T cmd     = { 0 };
  SMIH_DATA_FEILD_T data   = { 0 };

  data.blockSize         = 0x0;
  data.blockCount        = 0x0;
  data.data              = NULL;
  data.direction         = SMIH_WRITE_DIRECTION;
  cmd.cmdIdx             = 0;
  cmd.cmdArg             = 0x0;
  cmd.responseTypeSelect = SMIH_NO_RESPONSE;

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
 * @fn           error_t RSI_SDMEM_SendCardInterfaceConditionCmd8(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Arg)
 * @brief        This API is used to send interface condition command(Cmd8).
 * @param[in]    pSmihConfig      :  Pointer to the smih config structure.
 * @param[in]    Arg              :  Command argument to send
 * @return       RSI_OK           :  If command sent successfully
 *               ERROR_TIMEOUT    :  If command error timeout occures.
 */
error_t RSI_SDMEM_SendCardInterfaceConditionCmd8(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Arg)
{
  SMIH_TRANSFER_T transfer = { 0 };
  SMIH_CMD_FEILD_T cmd     = { 0 };
  SMIH_DATA_FEILD_T data   = { 0 };

  data.blockSize         = 0x0;
  data.blockCount        = 0x0;
  data.data              = NULL;
  data.direction         = SMIH_WRITE_DIRECTION;
  cmd.cmdArg             = Arg;
  cmd.cmdIdx             = CMD_8;
  cmd.responseTypeSelect = SMIH_RESPONSE_R1R5R6R7;

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
 * @fn           error_t RSI_SDMEM_SendApplicationCommandCmd55(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Arg)
 * @brief        This API is used to send the application command(CMD55) to the memory card.
 * @param[in]    pSmihConfig      :  Pointer to the smih config structure.
 * @param[in]    Arg              :  Argument to the command
 * @return       RSI_OK           :  If the command sent successfully
 *               ERROR_TIMEOUT    :  If the command error timeout occures.
 */
error_t RSI_SDMEM_SendApplicationCommandCmd55(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Arg)
{
  SMIH_TRANSFER_T transfer = { 0 };
  SMIH_CMD_FEILD_T cmd     = { 0 };
  SMIH_DATA_FEILD_T data   = { 0 };

  data.data              = NULL;
  data.blockSize         = 0x0;
  data.blockCount        = 0x0;
  data.direction         = SMIH_WRITE_DIRECTION;
  cmd.cmdIdx             = CMD_55;
  cmd.cmdArg             = Arg << 16;
  cmd.responseTypeSelect = SMIH_RESPONSE_R1R5R6R7;

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
 * @fn           error_t RSI_SDMEM_SetCardOperationConditionAcmd41(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Arg)
 * @brief        This API is used to send operation condition command(Acmd41).
 * @param[in]    Arg            :  Command argument to send
 * @param[in]    pSmihConfig    :  Pointer to the Command info structure
 * @return       RSI_OK         :  If the command sent successfully
 *               ERROR_TIMEOUT  :  If the command error timeout occures.
 */
error_t RSI_SDMEM_SetCardOperationConditionAcmd41(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Arg)
{
  SMIH_TRANSFER_T transfer = { 0 };
  SMIH_CMD_FEILD_T cmd     = { 0 };
  SMIH_DATA_FEILD_T data   = { 0 };

  data.data              = NULL;
  data.blockSize         = 0x0;
  data.blockCount        = 0x0;
  data.direction         = SMIH_WRITE_DIRECTION;
  cmd.cmdIdx             = (CMD_40_hex + CMD_41);
  cmd.cmdArg             = Arg;
  cmd.responseTypeSelect = SMIH_RESPONSE_R3R4;

  transfer.command = &cmd;
  transfer.data    = &data;

  if (RSI_OK == RSI_SDMEM_SendApplicationCommandCmd55(pSmihConfig, pSmihConfig->rca)) {
    // transfer Command
    if (RSI_OK != pSmihConfig->cmd_transfer(&transfer)) {
      return ERROR_SMIH;
    }
  } else {
    return ERROR_SMIH;
  }
  memcpy((void *)&pSmihConfig->ocr, &transfer.command->response[0], sizeof(pSmihConfig->ocr));
  if (0 == (transfer.command->response[0] & ACMD41_BUSY_BIT)) {
    return ERROR_SMIH; //operation is in progress
  }
  pSmihConfig->cardType = SMIH_CARD_STANDARD;
  if (0 != (transfer.command->response[0] & ACMD41_OCR_BIT)) {
    pSmihConfig->cardType = SMIH_CARD_HIGH_CAPACITY;
  } else {
    pSmihConfig->cardType = SMIH_CARD_STANDARD;
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_SDMEM_SwitchVoltageCmd11(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t CmdArg)
 * @brief        This API is used to send voltage switch command(SDIO_CMD11).
 * @param[in]    pSmihConfig       :  Pointer to the Command info structure
 * @param[in]    Arg               :  Command argument to send
 * @return       RSI_OK            :  If the command sent successfully
 *               ERROR_TIMEOUT     :  If the command error timeout occures.
 */
error_t RSI_SDMEM_SwitchVoltageCmd11(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t CmdArg)
{
  SMIH_TRANSFER_T transfer = { 0 };
  SMIH_CMD_FEILD_T cmd     = { 0 };
  SMIH_DATA_FEILD_T data   = { 0 };

  data.data              = NULL;
  data.blockSize         = 0;
  data.blockCount        = 0;
  data.direction         = SMIH_WRITE_DIRECTION;
  cmd.cmdIdx             = CMD_11;
  cmd.cmdArg             = 0;
  cmd.responseTypeSelect = SMIH_RESPONSE_R1R5R6R7;

  transfer.command = &cmd;
  transfer.data    = &data;

  // transfer Command
  if (RSI_OK != pSmihConfig->cmd_transfer(&transfer)) {
    return ERROR_SMIH;
  } else {
    // set Relative Card Address
    pSmihConfig->rca = transfer.command->response[0] >> 16;
    return RSI_OK;
  }
}

/*==============================================*/
/**
 * @fn           error_t RSI_SDMEM_SendCidCmd2(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Arg)
 * @brief        This API is used to send CMD2(to get CID of the card).
 * @param[in]    pSmihConfig      :  Pointer to the Command info structure
 * @param[in]    Arg              :  Command argument to send
 * @return       RSI_OK           :  If the command sent successfully
 *               ERROR_TIMEOUT    :  If the command error timeout occures.
 */
error_t RSI_SDMEM_SendCidCmd2(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Arg)
{
  uint32_t i               = 0;
  SMIH_TRANSFER_T transfer = { 0 };
  SMIH_CMD_FEILD_T cmd     = { 0 };
  SMIH_DATA_FEILD_T data   = { 0 };

  data.data              = NULL;
  data.blockSize         = 0x0;
  data.blockCount        = 0x0;
  data.direction         = SMIH_WRITE_DIRECTION;
  cmd.cmdIdx             = CMD_2;
  cmd.cmdArg             = 0;
  cmd.responseTypeSelect = SMIH_RESPONSE_R2;

  transfer.command = &cmd;
  transfer.data    = &data;

  // transfer Command
  if (RSI_OK == pSmihConfig->cmd_transfer(&transfer)) {
    for (i = 0; i < 15; i++) {
      pSmihConfig->cid[i] = *((uint8_t *)(transfer.command->response) + (14 - i));
    }
    return RSI_OK;
  }
  return ERROR_SMIH;
}

/*==============================================*/
/**
 * @fn           error_t RSI_SDIOH_SendRelativeAddressCmd3(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t CmdArg)
 * @brief        This API is used to send CMD3(send card relative address).
 * @param[in]    pSmihConfig     :  Pointer to the Command info structure
 * @param[in]    CmdArg          :  Command argument to send
 * @return       RSI_OK          :  If command sent successfully
 *               ERROR_TIMEOUT   :  If command error timeout occures.
 */
error_t RSI_SDIOH_SendRelativeAddressCmd3(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t CmdArg)
{
  SMIH_TRANSFER_T transfer = { 0 };
  SMIH_CMD_FEILD_T cmd     = { 0 };
  SMIH_DATA_FEILD_T data   = { 0 };

  data.data              = NULL;
  data.blockSize         = 0;
  data.blockCount        = 0;
  data.direction         = SMIH_WRITE_DIRECTION;
  cmd.cmdIdx             = CMD_3;
  cmd.cmdArg             = 0;
  cmd.responseTypeSelect = SMIH_RESPONSE_R1R5R6R7;

  transfer.command = &cmd;
  transfer.data    = &data;

  // transfer Command
  if (RSI_OK == pSmihConfig->cmd_transfer(&transfer)) {
    // set Relative card address
    pSmihConfig->rca = (transfer.command->response[0] >> 16);
    return RSI_OK;
  } else {
    return ERROR_SMIH;
  }
}

/*==============================================*/
/**
 * @fn           error_t RSI_SDMEM_SendCsdCmd9(SMIH_CARD_CONFIG_T *pSmihConfig)
 * @brief        This API is used to send CMD9(CSD) to the sd card
 * @param[in]    pSmihConfig       :  Pointer to the Command info structure
 * @return       RSI_OK            :  If the command sent successfully
 *               ERROR_TIMEOUT     :  If the command error timeout occures.
 */
error_t RSI_SDMEM_SendCsdCmd9(SMIH_CARD_CONFIG_T *pSmihConfig)
{
  SMIH_TRANSFER_T transfer     = { 0 };
  SMIH_CMD_FEILD_T cmd         = { 0 };
  SMIH_DATA_FEILD_T data       = { 0 };
  CSD_REG_VERSION1_T *pSdCsd   = NULL;
  CSD_REG_VERSION2_T *pSdhcCsd = NULL;
  uint32_t u32NumSector        = 0;
  uint32_t u32Csize            = 0;
  uint32_t u32CsizeMulti       = 0;

  data.data              = NULL;
  data.blockSize         = 0x0;
  data.blockCount        = 0x0;
  data.direction         = SMIH_WRITE_DIRECTION;
  cmd.cmdIdx             = CMD_9;
  cmd.cmdArg             = pSmihConfig->rca << 16;
  cmd.responseTypeSelect = SMIH_RESPONSE_R2;

  transfer.command = &cmd;
  transfer.data    = &data;

  // transfer Command
  if (RSI_OK != pSmihConfig->cmd_transfer(&transfer)) {
    return ERROR_SMIH;
  }
  memcpy(pSmihConfig->csd, transfer.command->response, sizeof(transfer.command->response));

  pSdCsd   = (CSD_REG_VERSION1_T *)&pSmihConfig->csd[0];
  pSdhcCsd = (CSD_REG_VERSION2_T *)&pSmihConfig->csd[0];

  if (pSmihConfig->cardType == SMIH_CARD_STANDARD) {
    // Standard Capacity card
    u32Csize =
      ((unsigned int)pSdCsd->deviceSize3 << 10) + ((unsigned int)pSdCsd->deviceSize2 << 2) + pSdCsd->deviceSize1;
    u32CsizeMulti = (pSdCsd->deviceSizeMultiplier2 << 1) + pSdCsd->deviceSizeMultiplier1;
    u32NumSector  = (u32Csize + 1) << (u32CsizeMulti + 2);
    if (pSdCsd->readBlockLength == 0x0A) {
      u32NumSector *= 2;
    } else if (pSdCsd->readBlockLength == 0x0B) {
      u32NumSector *= 4;
    }
  } else {
    // high capacity card
    u32Csize =
      ((unsigned int)pSdhcCsd->deviceSize3 << 16) + ((unsigned int)pSdhcCsd->deviceSize2 << 8) + pSdhcCsd->deviceSize1;
    u32NumSector = (u32Csize + 1) * 1024;
  }
  pSmihConfig->maxSectorNum = u32NumSector;

  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_SDMEM_SelectCardCmd7(SMIH_CARD_CONFIG_T *pSmihConfig)
 * @brief        This API is used to send SDIO_CMD7(to select the card).
 * @param[in]    pSmihConfig      :  Pointer to the Command info structure
 * @return       RSI_OK           :  If command sent successfully
 *               ERROR_TIMEOUT    :  If command error timeout occures.
 */
error_t RSI_SDMEM_SelectCardCmd7(SMIH_CARD_CONFIG_T *pSmihConfig)
{
  SMIH_TRANSFER_T transfer = { 0 };
  SMIH_CMD_FEILD_T cmd     = { 0 };
  SMIH_DATA_FEILD_T data   = { 0 };

  data.data       = NULL;
  data.blockSize  = 0;
  data.blockCount = 0;
  data.direction  = SMIH_WRITE_DIRECTION;

  cmd.cmdIdx             = CMD_7;
  cmd.cmdArg             = pSmihConfig->rca << 16;
  cmd.responseTypeSelect = SMIH_RESPONSE_R1BR5B;

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
 * @fn           error_t RSI_SDMEM_SetBusWidthAcmd6(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Arg) 
 * @brief        This API is used to send ACMD6(set bus width command to the card).
 * @param[in]    pSmihConfig       :  Pointer to the Command info structure
 * @param[in]    Arg               :  Command argument to send
 * @return       RSI_OK            :  If command sent successfully
 *               ERROR_TIMEOUT     :  If command error timeout occures.
 */
error_t RSI_SDMEM_SetBusWidthAcmd6(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Arg)
{
  SMIH_TRANSFER_T transfer = { 0 };
  SMIH_CMD_FEILD_T cmd     = { 0 };
  SMIH_DATA_FEILD_T data   = { 0 };

  data.data       = NULL;
  data.blockSize  = 0;
  data.blockCount = 0;
  data.direction  = SMIH_WRITE_DIRECTION;

  cmd.cmdIdx             = (CMD_40_hex + CMD_6);
  cmd.cmdArg             = Arg;
  cmd.responseTypeSelect = SMIH_RESPONSE_R1R5R6R7;

  transfer.command = &cmd;
  transfer.data    = &data;

  // transfer Command
  if (RSI_OK != RSI_SDMEM_SendApplicationCommandCmd55(pSmihConfig, pSmihConfig->rca)) {
    return ERROR_SMIH;
  }
  // transfer Command
  if (RSI_OK != pSmihConfig->cmd_transfer(&transfer)) {
    return ERROR_SMIH;
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_SDMEM_OperationSwitchFunctionReadCmd6(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Argument)
 * @brief        This API is used to send the switch function read command(asks card about its capability).
 * @param[in]    pSmihConfig      :  Pointer to the Command info structure
 * @param[in]    Argument         :  Argument to the command
 * @return       RSI_OK           :  If the command sent successfully
 *               ERROR_TIMEOUT    :  If the command error timeout occures.
 */
error_t RSI_SDMEM_OperationSwitchFunctionReadCmd6(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Argument)
{
  SMIH_TRANSFER_T transfer = { 0 };
  SMIH_CMD_FEILD_T cmd     = { 0 };
  SMIH_DATA_FEILD_T data   = { 0 };

  data.data       = 0;
  data.blockSize  = 1;
  data.blockCount = 1;
  data.direction  = SMIH_READ_DIRECTION;

  cmd.cmdIdx             = CMD_6;
  cmd.cmdArg             = Argument;
  cmd.responseTypeSelect = SMIH_RESPONSE_R1R5R6R7;

  transfer.command = &cmd;
  transfer.data    = &data;

  // transfer command
  if (RSI_OK != pSmihConfig->cmd_transfer(&transfer)) {
    return ERROR_SMIH;
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_SDMEM_GetCardOperationConditionAcmd41(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Arg)
 * @brief        This API is used to send operation condition command(ACMD41).
 * @param[in]    pSmihConfig     :  Pointer to the smih command info structure
 * @param[in]    Arg             :  Command argument to send
 * @return       RSI_OK          :  If command sent successfully
 *               ERROR_TIMEOUT   :  If command error timeout occures.
 */
error_t RSI_SDMEM_GetCardOperationConditionAcmd41(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Arg)
{
  SMIH_TRANSFER_T transfer = { 0 };
  SMIH_CMD_FEILD_T cmd     = { 0 };
  SMIH_DATA_FEILD_T data   = { 0 };

  data.data       = NULL;
  data.blockSize  = 0x0;
  data.blockCount = 0x0;
  data.direction  = SMIH_WRITE_DIRECTION;

  cmd.cmdIdx             = (CMD_40_hex + CMD_41);
  cmd.cmdArg             = Arg;
  cmd.responseTypeSelect = SMIH_RESPONSE_R3R4;

  // transfer Command
  if (RSI_OK != RSI_SDMEM_SendApplicationCommandCmd55(pSmihConfig, pSmihConfig->rca)) {
    return ERROR_SMIH;
  }
  transfer.command = &cmd;
  transfer.data    = &data;

  // transfer Command
  if (RSI_OK != pSmihConfig->cmd_transfer(&transfer)) {
    return ERROR_SMIH;
  }

  memcpy((void *)&pSmihConfig->ocr, &transfer.command->response[0], sizeof(pSmihConfig->ocr));
  if (0 == Arg) {

  } else if ((transfer.command->response[0] & ACMD41_BUSY_BIT) == 0) {
    // set mode
    return ERROR_SMIH; //operation is in progress
  }
  pSmihConfig->cardType = SMIH_CARD_STANDARD;
  if ((transfer.command->response[0] & ACMD41_OCR_BIT) != 0) {
    pSmihConfig->cardType = SMIH_CARD_HIGH_CAPACITY;
  } else {
    pSmihConfig->cardType = SMIH_CARD_STANDARD;
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_SDMEM_CardBusWidthConfig(SMIH_CARD_CONFIG_T *pSmihConfig, uint8_t BusWidth)
 * @brief        This API is used to configure the bus width.
 * @param[in]    pSmihConfig          : Pointer to the Command info structure
 * @param[in]    BusWidth             : bus width mode
 *               possible values are    0-1bit
 *                                      1-4bit
 *                                      2-8bit
 * @return       RSI_OK               : If command sent successfully
 *               ERROR_TIMEOUT        : If command error timeout occures.
 */
error_t RSI_SDMEM_CardBusWidthConfig(SMIH_CARD_CONFIG_T *pSmihConfig, uint8_t BusWidth)
{
  uint32_t cmdArg = 0;
  boolean_t hostMode;
  if (BusWidth == SMIH_BUS_WIDTH1) {
    cmdArg   = 0;
    hostMode = FALSE;
  } else if (BusWidth == SMIH_BUS_WIDTH4) {
    cmdArg   = 0x2; //Need to talk with spandana
    hostMode = TRUE;
  } else if (BusWidth == SMIH_BUS_WIDTH8) {
    cmdArg   = 0x2;
    hostMode = TRUE;
  }
  // Ask card about its capability
  if (RSI_OK != RSI_SDMEM_SetBusWidthAcmd6(pSmihConfig, cmdArg)) {
    return ERROR_SMIH;
  } else {
    // set bus width mode in controller
    smih_bus_width_set(hostMode);
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_SDMEM_OperationSwitchFunctionWriteCmd6(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Argument)
 * @brief        This API is used to send the switch function command.
 * @param[in]    pSmihConfig      :  Pointer to the Command info structure
 * @param[in]    Argument         :  Argument to the command
 * @return       RSI_OK           :  If the command sent successfully
 *               ERROR_TIMEOUT    :  If the command error timeout occures.
 */
error_t RSI_SDMEM_OperationSwitchFunctionWriteCmd6(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Argument)
{
  SMIH_TRANSFER_T transfer = { 0 };
  SMIH_CMD_FEILD_T cmd     = { 0 };
  SMIH_DATA_FEILD_T data   = { 0 };

  data.data       = 0;
  data.blockSize  = 0;
  data.blockCount = 0;
  data.direction  = SMIH_WRITE_DIRECTION;

  cmd.cmdIdx             = CMD_6;
  cmd.cmdArg             = Argument;
  cmd.responseTypeSelect = SMIH_RESPONSE_R1R5R6R7;

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
 * @fn           error_t RSI_SDMEM_BlocksWrite(SMIH_CARD_CONFIG_T *pSmihConfig,
                              const uint8_t *DataIn,
                              uint32_t BlockIndex,
                              uint32_t BlockCount)
 * @brief        This API is used to write multiple blocks of data to the sd card.
 * @param[in]    pSmihConfig      :  Pointer to the sd card config structure
 * @param[in]    DataIn           :  Data buffer to send
 * @param[in]    BlockIndex       :  block index value
 * @param[in]    BlockCount       :  blocks count value
 * @return       RSI_OK           :  If the command sent successfully
 *               ERROR_TIMEOUT    :  If the command error timeout occures.
 */
error_t RSI_SDMEM_BlocksWrite(SMIH_CARD_CONFIG_T *pSmihConfig,
                              const uint8_t *DataIn,
                              uint32_t BlockIndex,
                              uint32_t BlockCount)
{
  SMIH_TRANSFER_T transfer = { 0 };
  SMIH_CMD_FEILD_T cmd     = { 0 };
  SMIH_DATA_FEILD_T data   = { 0 };
  if (pSmihConfig == NULL || DataIn == NULL) {
    return INVALID_PARAMETERS;
  }
  if (SMIH_CARD_STANDARD == pSmihConfig->cardType) {
    BlockIndex = BlockIndex << 9;
  }
  if (BlockCount == 1) {
    // single block write
    if (pSmihConfig->maxSectorNum < BlockIndex) {
      return INVALID_PARAMETERS;
    }
    data.data       = DataIn;
    data.blockSize  = BLOCK_SIZE_512;
    data.blockCount = 1;
    data.direction  = SMIH_WRITE_DIRECTION;

    cmd.cmdIdx             = CMD_24;
    cmd.cmdArg             = BlockIndex;
    cmd.responseTypeSelect = SMIH_RESPONSE_R1R5R6R7;
  } else {
    // multiple block write
    if (((pSmihConfig->maxSectorNum - BlockIndex) < BlockCount)
        || (pSmihConfig->maxSectorNum < (BlockIndex + BlockCount))) {
      return INVALID_PARAMETERS;
    }
    data.data       = DataIn;
    data.blockSize  = BLOCK_SIZE_512;
    data.blockCount = BlockCount;
    data.direction  = SMIH_WRITE_DIRECTION;

    cmd.cmdIdx             = CMD_25;
    cmd.cmdArg             = BlockIndex;
    cmd.responseTypeSelect = SMIH_RESPONSE_R1R5R6R7;
  }
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
 * @fn           error_t RSI_SDMEM_BlocksRead(SMIH_CARD_CONFIG_T *pSmihConfig,
                             uint8_t *DataOut,
                             uint32_t BlockIndex,
                             uint32_t BlockCount)
 * @brief        This API is used to read multiple blocks of data from the sd card.
 * @param[in]    pSmihConfig       :  Pointer to the sd card config structure
 * @param[in]    DataIn            :  Data buffer to send
 * @param[in]    BlockIndex        :  block index value
 * @param[in]    BlockCount        :  blocks count value
 * @return       RSI_OK            :  Command sent successfully
 *               ERROR_TIMEOUT     :  Command error timeout occured.
 */
error_t RSI_SDMEM_BlocksRead(SMIH_CARD_CONFIG_T *pSmihConfig,
                             uint8_t *DataOut,
                             uint32_t BlockIndex,
                             uint32_t BlockCount)
{
  SMIH_TRANSFER_T transfer = { 0 };
  SMIH_CMD_FEILD_T cmd     = { 0 };
  SMIH_DATA_FEILD_T data   = { 0 };

  if (NULL == pSmihConfig || NULL == DataOut) {
    return INVALID_PARAMETERS;
  }
  if (SMIH_CARD_STANDARD == pSmihConfig->cardType) {
    BlockIndex = BlockIndex << 9;
  }
  if (BlockCount == 1) {
    // single block read
    if (pSmihConfig->maxSectorNum < BlockIndex) {
      return INVALID_PARAMETERS;
    }
    cmd.cmdIdx             = CMD_17;
    cmd.cmdArg             = BlockIndex;
    data.data              = DataOut;
    cmd.responseTypeSelect = SMIH_RESPONSE_R1R5R6R7;

    data.blockSize  = BLOCK_SIZE_512;
    data.blockCount = 1;
    data.direction  = SMIH_READ_DIRECTION;
  } else {
    // multiple block read
    if (((pSmihConfig->maxSectorNum - BlockIndex) < BlockCount)
        || (pSmihConfig->maxSectorNum < (BlockIndex + BlockCount))) {
      return INVALID_PARAMETERS;
    }
    data.data              = DataOut;
    data.blockSize         = BLOCK_SIZE_512;
    data.blockCount        = BlockCount;
    data.direction         = SMIH_READ_DIRECTION;
    cmd.cmdIdx             = CMD_18;
    cmd.cmdArg             = BlockIndex;
    cmd.responseTypeSelect = SMIH_RESPONSE_R1R5R6R7;
  }
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
 * @fn          error_t RSI_SDMEM_EnableHighSpeed(SMIH_CARD_CONFIG_T *pSmihConfig, boolean_t HighSpeed, uint32_t Clock)
 * @brief        This API is used to enable high speed mode
 * @param[in]    pSmihConfig      :  Pointer to the smih card configuration structure
 * @param[in]    HighSpeed        :  High speed enable bit
 *                                   1- for high speed enable.
 *                                   0- for high speed disable.
 * @param[in]    Clock            :  Clock frequency
 * @return       RSI_OK           :  If the command sent successfully
 *               ERROR_TIMEOUT    :  If the command error timeout occures.
 */
error_t RSI_SDMEM_EnableHighSpeed(SMIH_CARD_CONFIG_T *pSmihConfig, boolean_t HighSpeed, uint32_t Clock)
{
  uint32_t actualClock    = 0;
  uint32_t highSpeedValue = 0;
  uint32_t arg            = 0;

  if (HighSpeed == 0) {
    actualClock = 400000;
  } else {
    actualClock    = Clock;
    highSpeedValue = 1;
  }
  arg = (BIT(31) | 0xFFF0);
  arg |= (highSpeedValue)&0xF;

  // Send switch command
  if (RSI_OK != RSI_SDMEM_OperationSwitchFunctionWriteCmd6(pSmihConfig, arg)) {
    return ERROR_SMIH;
  } else {
    smih_clock_config(pSmihConfig, actualClock);
    SMIH->SMIH_HOST_CONTROL_1_REGISTER_b.HIGH_SPEED_ENABLE = 0x1;
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           void RSI_SDMEM_PinMux(void)
 * @brief        This API is used to configure GPIOs for SDMEM operations.
 * @return       none
 */
void RSI_SDMEM_PinMux(void)
{
  // If GPIO_SET1 is equals to 0 then set2 gpios of sdmem will be used
#if (GPIO_SET1 == 0)
  RSI_EGPIO_HostPadsGpioModeEnable(SD_CLK_PIN);
  RSI_EGPIO_HostPadsGpioModeEnable(SD_CMD_PIN);
  RSI_EGPIO_HostPadsGpioModeEnable(SD_D0_PIN);
  RSI_EGPIO_HostPadsGpioModeEnable(SD_D1_PIN);
  RSI_EGPIO_HostPadsGpioModeEnable(SD_D2_PIN);
  RSI_EGPIO_HostPadsGpioModeEnable(SD_D3_PIN);
#endif
  RSI_EGPIO_PadSelectionEnable(SD_CLK_PAD);
  RSI_EGPIO_PadSelectionEnable(SD_CMD_PAD);
  RSI_EGPIO_PadSelectionEnable(SD_D0_PAD);
  RSI_EGPIO_PadSelectionEnable(SD_D1_PAD);
  RSI_EGPIO_PadSelectionEnable(SD_D2_PAD);
  RSI_EGPIO_PadSelectionEnable(SD_D3_PAD);

  RSI_EGPIO_PadReceiverEnable(SD_CLK_PIN);
  RSI_EGPIO_PadReceiverEnable(SD_CMD_PIN);
  RSI_EGPIO_PadReceiverEnable(SD_D0_PIN);
  RSI_EGPIO_PadReceiverEnable(SD_D1_PIN);
  RSI_EGPIO_PadReceiverEnable(SD_D2_PIN);
  RSI_EGPIO_PadReceiverEnable(SD_D3_PIN);

  RSI_EGPIO_PadDriverDisableState(SD_CMD_PIN, Pullup);
  RSI_EGPIO_PadDriverDisableState(SD_D0_PIN, Pullup);
  RSI_EGPIO_PadDriverDisableState(SD_D1_PIN, Pullup);
  RSI_EGPIO_PadDriverDisableState(SD_D2_PIN, Pullup);
  RSI_EGPIO_PadDriverDisableState(SD_D3_PIN, Pullup);

  RSI_EGPIO_SetPinMux(EGPIO, 0, SD_CLK_PIN, SD_CLK_MUX);
  RSI_EGPIO_SetPinMux(EGPIO, 0, SD_CMD_PIN, SD_CMD_MUX);
  RSI_EGPIO_SetPinMux(EGPIO, 0, SD_D0_PIN, SD_D0_MUX);
  RSI_EGPIO_SetPinMux(EGPIO, 0, SD_D1_PIN, SD_D1_MUX);
  RSI_EGPIO_SetPinMux(EGPIO, 0, SD_D2_PIN, SD_D2_MUX);
  RSI_EGPIO_SetPinMux(EGPIO, 0, SD_D3_PIN, SD_D3_MUX);

#if _8BIT_MODE
  RSI_EGPIO_PadSelectionEnable(SD_D4_PAD);
  RSI_EGPIO_PadSelectionEnable(SD_D5_PAD);
  RSI_EGPIO_PadSelectionEnable(SD_D6_PAD);
  RSI_EGPIO_PadSelectionEnable(SD_D7_PAD);

  RSI_EGPIO_PadReceiverEnable(SD_D4_PIN);
  RSI_EGPIO_PadReceiverEnable(SD_D5_PIN);
  RSI_EGPIO_PadReceiverEnable(SD_D6_PIN);
  RSI_EGPIO_PadReceiverEnable(SD_D7_PIN);

  RSI_EGPIO_PadDriverDisableState(SD_D4_PIN, Pullup);
  RSI_EGPIO_PadDriverDisableState(SD_D5_PIN, Pullup);
  RSI_EGPIO_PadDriverDisableState(SD_D6_PIN, Pullup);
  RSI_EGPIO_PadDriverDisableState(SD_D7_PIN, Pullup);

  RSI_EGPIO_SetPinMux(EGPIO, 0, SD_D4_PIN, SD_D4_MUX);
  RSI_EGPIO_SetPinMux(EGPIO, 0, SD_D5_PIN, SD_D5_MUX);
  RSI_EGPIO_SetPinMux(EGPIO, 0, SD_D6_PIN, SD_D6_MUX);
  RSI_EGPIO_SetPinMux(EGPIO, 0, SD_D7_PIN, SD_D7_MUX);
#endif
  RSI_EGPIO_PadSelectionEnable(SD_WP_PAD);
  RSI_EGPIO_PadSelectionEnable(SD_CD_PAD);

  RSI_EGPIO_PadReceiverEnable(SD_CD_PIN);
  RSI_EGPIO_PadReceiverEnable(SD_WP_PIN);

  RSI_EGPIO_PadDriverDisableState(SD_WP_PIN, Pulldown);
  RSI_EGPIO_PadDriverDisableState(SD_CD_PIN, Pulldown);

  RSI_EGPIO_SetPinMux(EGPIO, 0, SD_CD_PIN, SD_CD_MUX);
  RSI_EGPIO_SetPinMux(EGPIO, 0, SD_WP_PIN, SD_WP_MUX);
}

/*==============================================*/
/**
 * @fn           error_t RSI_SDMEM_Enumeration(SMIH_CARD_CONFIG_T *pSmihConfig, ARM_SMIH_SignalEvent_t Event)
 * @brief        This API is used for SD memory enumeration process
 * @param[in]    pSmihConfig      : Pointer to the smih card configuration structure
 * @param[in]    Event            : event handler to be register
 * @return       RSI_OK           : If commands sent successfully in enumeration process.
 *               ERROR_TIMEOUT    : If commands error timeout occures.
 */
error_t RSI_SDMEM_Enumeration(SMIH_CARD_CONFIG_T *pSmihConfig, ARM_SMIH_SignalEvent_t Event)
{
  uint8_t val  = 0;
  uint32_t arg = 0;
  uint32_t i;
  SMIH_CARD_CONFIG_T *SmihInfo = NULL;
  if (pSmihConfig == 0) {
    return INVALID_PARAMETERS;
  }
  // Configure gpios in smih mode
  RSI_SDMEM_PinMux();

  if (RSI_OK != RSI_SD_HostInit(pSmihConfig, Event, 1)) {
    return ERROR_SMIH;
  } else {
    // Commands for sdio enumeration
    rsi_delay_ms(5);

    // Reset card(send command 0)
    if (RSI_OK != RSI_SDMEM_GoIdleStateCmd0(pSmihConfig)) {
      return ERROR_SMIH;
    }

    // send interface condition
    if (RSI_OK != RSI_SDMEM_SendCardInterfaceConditionCmd8(pSmihConfig, 0x1AA)) {
      val = 0;
    } else {
      val = 1;
    }
    // send ACMD41
    if (RSI_OK != RSI_SDMEM_GetCardOperationConditionAcmd41(pSmihConfig, 0x0)) {
      return ERROR_SMIH;
    }
    if (pSmihConfig->uhsModeSelect == 0) {
      arg = ACMD41_VOLTAGE;
      if (1 == val) {
        arg |= ACMD41_HCS;
      }
    } else {
      arg = ACMD41_UHS_REQ;
      if (1 == val) {
        arg |= ACMD41_HCS;
      }
    }
    // send operation condition
    i = 5000;
    while (i != 0) {
      if (RSI_SDMEM_SetCardOperationConditionAcmd41(pSmihConfig, arg) == RSI_OK) {
        break;
      }
      i--;
    }
    // configure uhs mode if enabled
    if (pSmihConfig->uhsModeSelect != 0) {
      rsi_delay_ms(5);
      RSI_SDMEM_SwitchVoltageCmd11(SmihInfo, 0x0);
      M4CLK->SD_MEM_CLOCK_REG_b.SD_MEM_INTF_CLK_SEL = 0x7;
      // wait for some time
      rsi_delay_ms(5);
      switch (pSmihConfig->uhsModeSelect) {
        case UHS_NONE:
          break;
        case UHS_SDR12:
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
          break;
        default:
          return INVALID_PARAMETERS;
      }
      // enable 1.8v signalling bit
      SMIH->SMIH_HOST_CONTROL_2_REGISTER_b._1_8V_SIGNALING_ENABLE = 0x1;
      RSI_CLK_SdMemClkConfig(M4CLK, 1, SDMEM_SOCPLLCLK, 1);

      // wait for some
      rsi_delay_ms(5);
    }
    // send cmd2
    if (RSI_OK != RSI_SDMEM_SendCidCmd2(pSmihConfig, 0x0)) {
      return ERROR_SMIH;
    }
    // send cmd3
    if (RSI_OK != RSI_SDIOH_SendRelativeAddressCmd3(pSmihConfig, 0x0)) {
      return ERROR_SMIH;
    }
    // send cmd9
    if (RSI_OK != RSI_SDMEM_SendCsdCmd9(pSmihConfig)) {
      return ERROR_SMIH;
    }
    // select card :send rca as argument
    if (RSI_OK != RSI_SDMEM_SelectCardCmd7(pSmihConfig)) {
      return ERROR_SMIH;
    }
    // bus width configuration
    if (pSmihConfig->busWidthMode == 1) {
      if (RSI_OK != RSI_SDMEM_CardBusWidthConfig(pSmihConfig, SMIH_BUS_WIDTH4)) {
        return ERROR_SMIH;
      }
    }
    // enable high speed mode
    if (TRUE == pSmihConfig->highSpeedEnable) {
      rsi_delay_ms(1);
      if (RSI_OK != RSI_SDMEM_EnableHighSpeed(pSmihConfig, TRUE, 20000000)) {
        return ERROR_SMIH;
      }
      rsi_delay_ms(10);
    }
    if (pSmihConfig->uhsModeSelect != 0) {
      switch (pSmihConfig->uhsModeSelect) {
        case UHS_SDR12:
          break;
        case UHS_SDR25:
          // uhs support asking to card
          if (RSI_OK != RSI_SDMEM_OperationSwitchFunctionReadCmd6(pSmihConfig, 0x00000001)) {
            return ERROR_SMIH;
          }
          // switching mode
          if (RSI_OK != RSI_SDMEM_OperationSwitchFunctionWriteCmd6(pSmihConfig, 0x00000001)) {
            return ERROR_SMIH;
          }
          break;
        case UHS_SDR50:
          // uhs support asking to card
          if (RSI_OK != RSI_SDMEM_OperationSwitchFunctionReadCmd6(pSmihConfig, 0x00000002)) {
            return ERROR_SMIH;
          }
          // switching mode
          if (RSI_OK != RSI_SDMEM_OperationSwitchFunctionWriteCmd6(pSmihConfig, 0x00000002)) {
            return ERROR_SMIH;
          }
          break;
        case UHS_SDR104:
          // uhs support asking to card
          if (RSI_OK != RSI_SDMEM_OperationSwitchFunctionReadCmd6(pSmihConfig, 0x00000003)) {
            return ERROR_SMIH;
          }
          // switching mode
          if (RSI_OK != RSI_SDMEM_OperationSwitchFunctionWriteCmd6(pSmihConfig, 0x00000003)) {
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

/*==============================================*/
/**
 * @fn           error_t RSI_SDMMC_Enumeration(SMIH_CARD_CONFIG_T *pSmihConfig, ARM_SMIH_SignalEvent_t Event)
 * @brief        This API is used for SD MMC card enumeration process
 * @param[in]    pSmihConfig     :  Pointer to the smih card configuration structure
 * @param[in]    Event           :  event handler to be register
 * @return       RSI_OK          :  If MMC card enumeration successfully done
 *               ERROR_TIMEOUT   :  If the commands error timeout occures.
 */
error_t RSI_SDMMC_Enumeration(SMIH_CARD_CONFIG_T *pSmihConfig, ARM_SMIH_SignalEvent_t Event)
{
  if (pSmihConfig == 0) {
    return INVALID_PARAMETERS;
  }
  // Configure gpios in smih mode
  RSI_SDMEM_PinMux();

  if (RSI_OK != RSI_SD_HostInit(pSmihConfig, Event, 1)) {
    return ERROR_SMIH;
  } else {
    // Commands for sdio enumeration
    rsi_delay_ms(5);

    // Reset card(send command 0)
    if (RSI_OK != RSI_SDMEM_GoIdleStateCmd0(pSmihConfig)) {
      return ERROR_SMIH;
    }
    while (1) {
      if (RSI_OK == RSI_SDMMC_SendOperationConditionCmd1(pSmihConfig, 0x40FF8000)) {
        break;
      }
    }
    // send cmd2
    if (RSI_OK != RSI_SDMEM_SendCidCmd2(pSmihConfig, 0x0)) {
      return ERROR_SMIH;
    }
    // send cmd3
    if (RSI_OK != RSI_SDIOH_SendRelativeAddressCmd3(pSmihConfig, 0x0)) {
      return ERROR_SMIH;
    }
    // select card :send rca as argument
    if (RSI_OK != RSI_SDMEM_SelectCardCmd7(pSmihConfig)) {
      return ERROR_SMIH;
    }
    // send csd command to card
    if (RSI_OK != RSI_SDMMC_SendExtentCsdCmd(pSmihConfig)) {
      return ERROR_SMIH;
    }
    // bus width configuration
    if (pSmihConfig->busWidthMode == 1) {
      if (RSI_OK != RSI_SDMMC_ChangeBusWidthMode(pSmihConfig, SMIH_BUS_WIDTH4)) {
        return ERROR_SMIH;
      }
      rsi_delay_ms(5);
    } else if (pSmihConfig->busWidthMode == 2) {
      if (RSI_OK != RSI_SDMMC_ChangeBusWidthMode(pSmihConfig, SMIH_BUS_WIDTH8)) {
        return ERROR_SMIH;
      }
      rsi_delay_ms(5);
    }
    // enable high speed mode
    if (TRUE == pSmihConfig->highSpeedEnable) {
      rsi_delay_ms(1);
      if (RSI_OK != RSI_SDMMC_HighSpeedMode(pSmihConfig, TRUE, pSmihConfig->clock)) {
        return ERROR_SMIH;
      }
      rsi_delay_ms(1);
    }
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_SDMMC_SendOperationConditionCmd1(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Arg)
 * @brief        This API is used to send MMC operation condition command.
 * @param[in]    pSmihConfig      : Pointer to the smih card configuration structure
 * @param[in]    Arg              : Argument to the command
 * @return       RSI_OK           : If the command sent successfully
 *               ERROR_TIMEOUT    : If the command error timeout occures.
 */
error_t RSI_SDMMC_SendOperationConditionCmd1(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Arg)
{
  SMIH_TRANSFER_T transfer = { 0 };
  SMIH_CMD_FEILD_T cmd     = { 0 };
  SMIH_DATA_FEILD_T data   = { 0 };

  data.blockSize  = 0x0;
  data.blockCount = 0x0;
  data.data       = NULL;
  data.direction  = SMIH_WRITE_DIRECTION;

  cmd.cmdIdx             = CMD_1;
  cmd.cmdArg             = Arg;
  cmd.responseTypeSelect = SMIH_RESPONSE_R3R4;

  transfer.command = &cmd;
  transfer.data    = &data;

  // transfer Command
  if (RSI_OK != pSmihConfig->cmd_transfer(&transfer)) {
    return ERROR_SMIH;
  }
  if (transfer.command->response[0] & BIT(31)) {
    pSmihConfig->cardType = SMIH_CARD_STANDARD;
    if (transfer.command->response[0] & BIT(30)) {
      pSmihConfig->cardType = SMIH_CARD_HIGH_CAPACITY;
    }
  } else {
    return ERROR_SMIH;
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_SDMMC_SendExtentCsdCmd(SMIH_CARD_CONFIG_T *pSmihConfig)
 * @brief        This API is used to send csd command to the MMC card.
 * @param[in]    pSmihConfig       : Pointer to the smih card configuration structure
 * @return       RSI_OK            : If the command sent successfully
 *               ERROR_TIMEOUT     : If the command error timeout occures.
 */
error_t RSI_SDMMC_SendExtentCsdCmd(SMIH_CARD_CONFIG_T *pSmihConfig)
{
  uint8_t ext_csd[BLOCK_SIZE_512] = { 0 };
  SMIH_TRANSFER_T transfer        = { 0 };
  SMIH_CMD_FEILD_T cmd            = { 0 };
  SMIH_DATA_FEILD_T data          = { 0 };
  uint32_t *preadVal              = NULL;

  data.blockSize  = BLOCK_SIZE_512;
  data.blockCount = 1;
  data.data       = ext_csd;
  data.direction  = SMIH_READ_DIRECTION;

  cmd.cmdIdx             = (CMD_80_hex | CMD_8); //mmc cmd 8
  cmd.cmdArg             = pSmihConfig->rca << 16;
  cmd.responseTypeSelect = SMIH_RESPONSE_R1R5R6R7;

  transfer.command = &cmd;
  transfer.data    = &data;

  // transfer Command
  if (RSI_OK != pSmihConfig->cmd_transfer(&transfer)) {
    return ERROR_SMIH;
  }
  rsi_delay_ms(10);

  preadVal                  = (uint32_t *)&ext_csd[212];
  pSmihConfig->maxSectorNum = *preadVal;

  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_SDMMC_ChangeBusWidthMode(SMIH_CARD_CONFIG_T *pSmihConfig, uint8_t bus_wdith)
 * @brief        This API is used to change bus width  mode to MMC card.
 * @param[in]    pSmihConfig        :  Pointer to the smih card configuration structure
 * @param[in]    bus_wdith          :  bus width mode to be configured
 *               possible values are   SMIH_BUS_WIDTH1   : 1 bit width
 *                                     SMIH_BUS_WIDTH4   : 4 bit width
 *                                     SMIH_BUS_WIDTH8   : 8 bit width
 * @return       RSI_OK             :  If the command sent successfully
 *               ERROR_TIMEOUT      :  If the command error timeout occures.
 */
error_t RSI_SDMMC_ChangeBusWidthMode(SMIH_CARD_CONFIG_T *pSmihConfig, uint8_t bus_wdith)
{
  uint32_t argument = 0;
  switch (bus_wdith) {
    case SMIH_BUS_WIDTH1:
      argument = (0x3 << 24) | (0xB7 << 16) | (0x0 << 8) | (0x0 << 0);
      break;
    case SMIH_BUS_WIDTH4:
      argument = (0x3 << 24) | (0xB7 << 16) | (0x1 << 8) | (0x0 << 0);
      break;
    case SMIH_BUS_WIDTH8:
      argument = (0x3 << 24) | (0xB7 << 16) | (0x2 << 8) | (0x0 << 0);
      break;
    default:
      return INVALID_PARAMETERS;
  }
  // send switch command
  if (RSI_OK != RSI_SDMMC_SwitchFunctionCmd6(pSmihConfig, argument)) {
    return ERROR_SMIH;
  }
  // set bus width
  smih_bus_width_set(bus_wdith);

  return RSI_OK;
}

/*==============================================*/
/**
 * @fn           error_t RSI_SDMMC_SwitchFunctionCmd6(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Arg)
 * @brief        This API is used to send switch mode function command to MMC card.
 * @param[in]    pSmihConfig      :  Pointer to the smih card configuration structure
 * @param[in]    Arg              :  Command argument to send
 * @return       RSI_OK           :  If the command sent successfully
 *               ERROR_TIMEOUT    :  If the command error timeout occures.
 */
error_t RSI_SDMMC_SwitchFunctionCmd6(SMIH_CARD_CONFIG_T *pSmihConfig, uint32_t Arg)
{
  SMIH_TRANSFER_T transfer = { 0 };
  SMIH_CMD_FEILD_T cmd     = { 0 };
  SMIH_DATA_FEILD_T data   = { 0 };

  data.blockSize         = 0;
  data.blockCount        = 0;
  data.data              = NULL;
  data.direction         = SMIH_WRITE_DIRECTION;
  cmd.cmdIdx             = CMD_6;
  cmd.cmdArg             = Arg;
  cmd.responseTypeSelect = SMIH_RESPONSE_R1BR5B;

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
 * @fn           error_t RSI_SDMMC_HighSpeedMode(SMIH_CARD_CONFIG_T *pSmihConfig, boolean_t HighSpeed, uint32_t Clock)
 * @brief        This API is used to enable high speed mode to MMC card.
 * @param[in]    pSmihConfig      : Pointer to the smih card configuration structure
 * @param[in]    HighSpeed        : high speed value : 1 for enbale
 *                                                     0 for disable
 * @param[in]    Clock            : Clock frequency
 * @return       RSI_OK           : If the command sent successfully
 *               ERROR_TIMEOUT    : If the command error timeout occures.
 */
error_t RSI_SDMMC_HighSpeedMode(SMIH_CARD_CONFIG_T *pSmihConfig, boolean_t HighSpeed, uint32_t Clock)
{
  uint32_t clk       = 0;
  uint32_t arg       = 0;
  uint32_t highspeed = 0;
  if (HighSpeed == 1) {
    highspeed = 1;
    clk       = Clock;
    arg       = (0x3 << 24) | (0xB9 << 16) | (0x0 << 8) | (0x0 << 0);
  } else {
    highspeed = 0;
    clk       = 400 * 1000;
    arg       = (0x3 << 24) | (0xB9 << 16) | (0x0 << 8) | (0x0 << 0);
  }
  // Send switch mode command to card
  if (RSI_OK != RSI_SDMMC_SwitchFunctionCmd6(pSmihConfig, arg)) {
    return ERROR_SMIH;
  }
  // Configure clock
  smih_clock_config(pSmihConfig, clk);

  // Enable or disable high speed mode
  SMIH->SMIH_HOST_CONTROL_1_REGISTER_b.HIGH_SPEED_ENABLE = highspeed;

  return RSI_OK;
}
#endif
