/*******************************************************************************
* @file  puf_api.c
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

#ifdef _CHIP_9118
#include "puf_api.h"
#include "rsi_board.h"

#define AES_BLOCK_SIZE 16
uint8_t puf_state;

/*==============================================*/
/**
 * @fn          int32_t puf_block(uint8_t block_type)
 * @brief       Blocks the operation according to the given block type
 * @param[in]   block type
 * @return      Zero - if success
 * 		        Non zero - if fails
 */
int32_t puf_block(uint8_t block_type)
{
  uint8_t puf_block_status = 0;
  if ((block_type < 0) || (block_type > 3))
    return QK_RETURN_INVALID_PARAM; //parameter  error

#ifdef _CHIP_9118
  puf_block_status = CHIP_CONFIG_WIRELESS;
#else
  puf_block_status = CHIP_MODE_CNFG_REG;
#endif

  /*check if iid is disabled*/
#ifdef _CHIP_9118
  if ((CHIP_CONFIG_WIRELESS & DISABLE_PUF) != 0x00)
#else
  if ((CHIP_MODE_CNFG_REG & DISABLE_IID) != 0x00)
#endif
  {
    return QK_RETURN_OP_NOT_ALLOWED;
  }

  if (block_type == BLOCK_IID) {
/*disable IID*/
#ifdef _CHIP_9118
    CHIP_CONFIG_WIRELESS |= DISABLE_PUF;
#else
    CHIP_MOD_CNFG_REG |= DISABLE_IID;
#endif

    /*Check if disabled or not*/
#ifdef _CHIP_9118
    if ((CHIP_CONFIG_WIRELESS & DISABLE_PUF) == DISABLE_PUF)
#else
    if ((CHIP_MODE_CNFG_REG & DISABLE_IID) == DISABLE_IID)
#endif
    {
      return QK_RETURN_SUCCESS;
    } else {
      return QK_RETURN_ERROR;
    }
  } else if (block_type == BLOCK_ENROLL) {
    /*Enroll blocked*/
#ifdef _CHIP_9118
    CHIP_CONFIG_WIRELESS |= BLOCK_PUF_IID_ENROLL;
#else
    CHIP_MODE_CNFG_REG |= BLCK_ENROLL;
#endif

    /*Check for Allow register if disabled or not*/
#ifdef _CHIP_9118
    if ((CHIP_CONFIG_WIRELESS & BLOCK_PUF_IID_ENROLL) == BLOCK_PUF_IID_ENROLL)
#else
    if ((CHIP_MODE_CNFG_REG & BLCK_ENROLL) == BLCK_ENROLL)
#endif
    {
      return QK_RETURN_SUCCESS;
    } else {
      return QK_RETURN_ERROR;
    }
  }

  else if (block_type == BLOCK_SET_KEY) {
    /*block set key*/
#ifdef _CHIP_9118
    CHIP_CONFIG_WIRELESS |= BLOCK_PUF_IID_SET_KEY;
#else
    CHIP_MODE_CNFG_REG |= BLCK_SET_KEY;
#endif
#ifdef _CHIP_9118
    if ((CHIP_CONFIG_WIRELESS & BLOCK_PUF_IID_SET_KEY) == BLOCK_PUF_IID_SET_KEY)
#else
    if ((CHIP_MODE_CNFG_REG & BLCK_SET_KEY) == BLCK_SET_KEY)
#endif
    {
      return QK_RETURN_SUCCESS;
    } else {
      return QK_RETURN_ERROR;
    }
  } else if (block_type == BLOCK_GET_KEY_REG) {
    /*block key via reg*/
#ifdef _CHIP_9118
    CHIP_CONFIG_WIRELESS |= BLOCK_PUF_IID_GET_KEY_REG;
#else
    CHIP_MODE_CNFG_REG |= BLCK_KEY_VIA_REG;
#endif

#ifdef _CHIP_9118
    if ((CHIP_CONFIG_WIRELESS & BLOCK_PUF_IID_GET_KEY_REG) == BLOCK_PUF_IID_GET_KEY_REG)
#else
    if ((CHIP_MODE_CNFG_REG & BLCK_KEY_VIA_REG) == BLCK_KEY_VIA_REG)
#endif
    {
      return QK_RETURN_SUCCESS;
    } else {
      return QK_RETURN_ERROR;
    }
  }

  return QK_RETURN_ERROR;
}
/*==============================================*/
/**
 * @fn           int32_t puf_wait_for_init()
 * @brief        Wait for initialization of PUF after power up QK_SR
 *               status register bit Zero, qk_busy will be 1 till the initialization
 *               and it goes to 0 after initialization. Depending on PUF state
 *               OK bit or error bit will set to 1
 * @return:      Zero - if success
 * 		         Non zero - if fails
 */
int32_t puf_wait_for_init()
{
  /*Check if IID is blocked */
#ifdef _CHIP_9118
  if (CHIP_CONFIG_WIRELESS & DISABLE_PUF)
#else
  if (CHIP_MODE_CNFG_REG & DISABLE_IID)
#endif
  {
    return QK_RETURN_ERROR;
  }

  /*Waiting until initialization is finished without any error*/
  while ((QK_STATUS_REG & (QK_ERROR_BUSY)) != 0)
    ;

  /*Checking the status register for OK*/
  if ((QK_STATUS_REG & QK_BUSY) == 0) {
    return QK_RETURN_ERROR;
  }
  return QK_RETURN_SUCCESS;
}
/*==============================================*/
/**
 * @fn         int32_t puf_enroll(uint32_t *actv_code)
 * @brief      This API generates the activation code and can be read through PUF o/p registers
 *             Activation code shall be different on each enroll.
 * @param[out] actv_code, Pointer to activation code
 * @return     Zero - if success
 * 		       Non zero - if fails
 */
int32_t puf_enroll(uint32_t *actv_code)
{
  uint16_t enroll_counter = 0;

  puf_wait_for_init(); //fixme remove in actual apis

  /*Check if IID enroll is blocked */
#ifdef _CHIP_9118
  if (CHIP_CONFIG_WIRELESS & BLOCK_PUF_IID_ENROLL)
#else
  if (CHIP_MODE_CNFG_REG & BLCK_ENROLL)
#endif
  {
    return QK_RETURN_OP_BLOCKED;
  }

  /*Check for enroll_allow bit high*/
  if ((QK_ALLOW_REG & QK_ALLOW_ENROLL) != QK_ALLOW_ENROLL) {
    return QK_RETURN_OP_NOT_ALLOWED;
  }

  /*Begin enroll*/
  QK_CONTROL_REG = QK_ENROLL;

  /*Check if command is accepted */
  if ((!(QK_STATUS_REG & QK_BUSY)) || (QK_STATUS_REG & QK_ERROR)) {
    return QK_RETURN_QK_ERROR;
  }

  /*copying the activation code generated from register to buffer*/
  while (enroll_counter < ACTIVATION_CODE_SIZE) {
    while ((QK_STATUS_REG & QK_CO_AV) != QK_CO_AV)
      ;
    actv_code[enroll_counter] = QK_CODE_OUTPUT_REG;
    enroll_counter            = enroll_counter + 0x1;
  }

  while ((QK_STATUS_REG & QK_OK) != QK_OK)
    ;
  /*check result or status*/
  if ((QK_STATUS_REG & QK_OK) == 0) {
    return QK_RETURN_ERROR;
  }

  return QK_RETURN_SUCCESS;
}
/*==============================================*/
/**
 * @fn         int32_t puf_start(uint32_t *actv_code)
 * @brief      In the start operation,the given activation code is loaded to the PUF
 * @param[in]  actv_code, pointer of activation code
 * @return     Zero - if success
 * 		       Non zero - if fails
 */
int32_t puf_start(uint32_t *actv_code)
{
  uint32_t start_counter = 0x00;

  puf_wait_for_init();

  /*Check if start is allowed*/
  if ((QK_ALLOW_REG & QK_ALLOW_START) != QK_ALLOW_START) {
    return QK_RETURN_OP_NOT_ALLOWED;
  }

  /*Begin start operation*/
  QK_CONTROL_REG = QK_START;

  /*Check if command is accepted */
  if ((!(QK_STATUS_REG & QK_BUSY)) || (QK_STATUS_REG & QK_ERROR)) {
    return QK_RETURN_QK_ERROR;
  }

  /*loading the activation code to PUF registers*/
  while (start_counter < ACTIVATION_CODE_SIZE) {
    while ((QK_STATUS_REG & QK_CI_REQ) != QK_CI_REQ)
      ;
    QK_CODE_INPUT_REG = actv_code[start_counter];
    start_counter     = start_counter + 0x1;
  }

  /*Waiting for Error or OK bit to be high*/
  while (!((QK_STATUS_REG & QK_OK) || (QK_STATUS_REG & QK_ERROR)))
    ;

  /*check for Qk ok */
  if ((QK_STATUS_REG & QK_ERROR_OK) && (QK_STATUS_REG & QK_ERROR)) {
    return QK_RETURN_INVALID_QK_INPUT;
  }
  puf_state = PUF_STARTED;
  return QK_RETURN_SUCCESS;
}
/*==============================================*/
/**
 * @fn           int32_t puf_set_uk(uint32_t *ukdata, uint8_t keyindex, uint16_t keysize, uint32_t *kcdata)
 * @brief        For the given User key,key code of 11*4 bytes is generated which can be stored in NVM
 * @param[in]	 keyindex, index of the key
 * @param[in]	 ukdata, user key
 * @param[in]	 keysize, key size(128/256)
 * @param[out]	 kcdata, Key code for the corresponding User key
 * @return       Zero - if success
 * 		         Non zero - if fails
 */
int32_t puf_set_uk(uint32_t *ukdata, uint8_t keyindex, uint16_t keysize, uint32_t *kcdata)
{
  uint32_t uk_counter;

#ifdef _CHIP_9118
  if (CHIP_CONFIG_WIRELESS & BLOCK_PUF_IID_SET_KEY)
#else
  if (CHIP_MODE_CNFG_REG & BLCK_SET_KEY)
#endif
  {
    return QK_RETURN_OP_BLOCKED;
  }

  /*Only 128/256 size is supported*/
  if ((keysize != BITS_256) && (keysize != BITS_128))
    return QK_RETURN_INVALID_PARAM;

  /* Wait till Set Key is allowed*/
  if ((QK_ALLOW_REG & QK_ALLOW_SET_KEY) != QK_ALLOW_SET_KEY) {
    return QK_RETURN_OP_NOT_ALLOWED;
  }
  /* program the key size and index*/

  /*setting output target as register*/
  QK_TARGET_REG = QK_TARGET_AS_REG;

  /*load key index*/
  QK_INDEX_REG = keyindex;

  /*feeding key size*/
  QK_SIZE_REG = keysize >> 6;

  /*begin Set Key*/
  QK_CONTROL_REG = QK_SET_UK;

  /* Check if command is accepted*/
  if ((!(QK_STATUS_REG & QK_BUSY)) || (QK_STATUS_REG & QK_ERROR)) {
    return QK_RETURN_QK_ERROR;
  }
  uk_counter = 0x0;

  /*getting the user key size from the input*/
  /*only 256 and 128 bits key size are supported*/

  /*Loading user key*/
  while (uk_counter < (keysize / 32)) {
    while ((QK_STATUS_REG & QK_KI_REQ) != QK_KI_REQ)
      ;
    QK_INPUT_REG = ukdata[uk_counter]; // Loading User Key
    uk_counter   = uk_counter + 0x1;
  }
  uk_counter = 0x0;

  /*Reading the key code generated for the corresponding user key*/
  while (uk_counter < KEY_CODE_SIZE) {
    while ((QK_STATUS_REG & QK_CO_AV) != QK_CO_AV)
      ;
    kcdata[uk_counter] = QK_CODE_OUTPUT_REG;
    uk_counter         = uk_counter + 0x1;
  }
  /*waiting for QK busy to go low ,operation was success*/
  while ((QK_STATUS_REG & (QK_OK | QK_BUSY)) != QK_OK)
    ;
  return QK_RETURN_SUCCESS;
}

int32_t puf_intf_key_frm_kh(uint32_t key_len, uint32_t intf_mode)
{
  uint32_t key_addr = 0;

  if (intf_mode & BIT(7)) {
    key_addr = IID_KH_ADDR;
  }
  /*WAITING for GET operation completion*/
  if (key_len == 32) {
    IID_CTRL_REG           = IID_KH_KEY_256 | IID_KH_OUT_RESET | IID_KH_SURCE_AES | IID_MAX_KEY_256 | key_addr;
    QK_AES_KEY_CONTROL_REG = AES_KEY_CHANGE_REQ | AES_KEY_SIZE_256;
  } else {
    IID_CTRL_REG           = IID_KH_OUT_RESET | IID_KH_SURCE_AES | key_addr;
    QK_AES_KEY_CONTROL_REG = AES_KEY_CHANGE_REQ;
  }

  /* Waiting For Key Present*/
  while ((QK_AES_STATUS_REG & QK_AES_KEY_PRESENT) != QK_AES_KEY_PRESENT)
    ;

  return QK_RETURN_SUCCESS;
}
/*==============================================*/
/**
 * @fn          int32_t puf_get_key(uint32_t *kcdata, uint32_t *mast_key, uint8_t intf_mode, uint32_t *key_size)
 * @brief    	For the given key code, key is generated
 * 				if interface mode is register => master key can be read through register
 * 				if interface mode is interface =>master key is loaded in to AES
 * 				if interface mode is Key holder=>master key is loaded from keyholder to AES
 * @param[in]	kcdata, key code data
 * @param[in]   intf_mode, interface mode(0x00=>interface;0x01=>register;0x02 => keyholder)
 * @param[in]   key_size, key size in byte
 * @param[out]  key output if interface mode is Register
 * @return:     Zero - if success
 * 		        Non zero - if fails
 */
int32_t puf_get_key(uint32_t *kcdata, uint32_t *mast_key, uint8_t intf_mode, uint32_t *key_size)
{
  uint32_t gk_counter, user_key_sz;
  uint32_t key_addr = 0;

  /* check  For get key Allow bit */
  if ((QK_ALLOW_REG & QK_ALLOW_GET_KEY) != QK_ALLOW_GET_KEY) {
    return QK_RETURN_OP_NOT_ALLOWED;
  }

  if (intf_mode & BIT(7)) {
    key_addr = IID_KH_ADDR;
    intf_mode &= ~BIT(7);
  }

  if (intf_mode & BIT(6)) {
    IID_CTRL_REG &= ~IID_KH_OUT_RESET;
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    IID_CTRL_REG |= IID_KH_OUT_RESET;
    intf_mode &= ~BIT(6);
  }

  /*extracting user key size from key code header*/
  user_key_sz = (kcdata[0] >> 24);

  if (user_key_sz == IID_256_BIT)
    user_key_sz = USER_KEY_SIZE_8;
  else if (user_key_sz == IID_128_BIT)
    user_key_sz = USER_KEY_SIZE_4;
  else {
    *key_size = 0;
    return QK_RETURN_INVALID_PARAM;
  }

  *key_size = (user_key_sz * 4); //key size in bytes

  if (intf_mode == QK_TARGET_AS_REG) {
#ifdef _CHIP_9118
    if (CHIP_CONFIG_WIRELESS & BLOCK_PUF_IID_GET_KEY_REG)
#else
    if (CHIP_MODE_CNFG_REG & BLCK_KEY_VIA_REG)
#endif
    {
      return QK_RETURN_OP_BLOCKED;
    }

    /*setting key output target as register*/
    QK_TARGET_REG = QK_TARGET_AS_REG;
  } else if (intf_mode == QK_TARGET_AS_INTF) {
    if (user_key_sz == USER_KEY_SIZE_8)
      IID_CTRL_REG = IID_MAX_KEY_256 | IID_KH_OUT_RESET;

    /*setting key output target as interface*/
    QK_TARGET_REG = QK_TARGET_AS_INTF;

    /*AES key change request*/
    QK_AES_KEY_CONTROL_REG = AES_KEY_CHANGE_REQ;

  } else if (intf_mode == QK_TARGET_AS_KH) {
    /*setting key output target as interface*/
    QK_TARGET_REG = QK_TARGET_AS_KH_INTF;
    if (user_key_sz == USER_KEY_SIZE_8) {
      IID_CTRL_REG = IID_KH_KEY_256 | IID_KH_OUT_RESET | IID_KH_LATCH_KEY_QK | IID_MAX_KEY_256 | key_addr;
    } else {
      IID_CTRL_REG = IID_KH_OUT_RESET | IID_KH_LATCH_KEY_QK | key_addr;
    }
  } else {
    return QK_RETURN_INVALID_MODE;
  }

  /*Begin GET KEY Operation*/
  QK_CONTROL_REG = QK_GET_KEY;

  gk_counter = 0x0;

  /*Check if command is accepted*/
  if ((!(QK_STATUS_REG & QK_BUSY)) || (QK_STATUS_REG & QK_ERROR)) {
    return QK_RETURN_QK_ERROR;
  }

  /*Feeding key code*/

  while (gk_counter < KEY_CODE_SIZE) {
    while ((QK_STATUS_REG & QK_AES_DATA_RQST) != QK_AES_DATA_RQST)
      ;
    QK_CODE_INPUT_REG = kcdata[gk_counter];
    gk_counter        = gk_counter + 0x1;
  }

  if (intf_mode == QK_TARGET_AS_REG) {
    gk_counter = 0x0;
    /*Reading Master code of the given key code*/

    while (gk_counter < user_key_sz) {
      while ((QK_STATUS_REG & QK_KO_AV) != QK_KO_AV) {
        if ((QK_STATUS_REG & QK_ERROR) && !(QK_STATUS_REG & QK_BUSY)) {
          return QK_INVALID_KEYCODE;
        }
      }
      mast_key[gk_counter] = QK_OUTPUT_REG;
      gk_counter           = gk_counter + 0x1;
    }
    /*waiting for QK busy to go low , operation was success*/

    while ((QK_STATUS_REG & QK_OK) != QK_OK) {

      if ((QK_STATUS_REG & QK_ERROR) && !(QK_STATUS_REG & QK_BUSY)) {
        return QK_INVALID_KEYCODE;
      }
    }

    return QK_RETURN_SUCCESS;
  } else if (intf_mode == QK_TARGET_AS_INTF) {
    /*Programming AES control registers according to the key size*/
    if ((kcdata[0] >> 24) == IID_256_BIT)
      QK_AES_KEY_CONTROL_REG = QK_AES_KEY_CHANGE_REQ_256B_INTF;
    else if ((kcdata[0] >> 24) == IID_128_BIT)
      QK_AES_KEY_CONTROL_REG = QK_AES_KEY_CHANGE_REQ_128B_INTF;

    /*waiting for QK busy to go low , operation was success*/
    while ((QK_STATUS_REG & QK_OK) != QK_OK) {

      if ((QK_STATUS_REG & QK_ERROR) && !(QK_STATUS_REG & QK_BUSY)) {
        return QK_INVALID_KEYCODE;
      }
    }

    /* Indicates That Key is Present*/

    while ((QK_AES_STATUS_REG & QK_AES_KEY_PRESENT) != QK_AES_KEY_PRESENT)
      ;
    return QK_RETURN_SUCCESS;
  }

  else if (intf_mode == QK_TARGET_AS_KH) {
    /*WAITING for GET operation completion*/
    while ((QK_STATUS_REG & QK_OK) != QK_OK) {

      if ((QK_STATUS_REG & QK_ERROR) && !(QK_STATUS_REG & QK_BUSY)) {
        return QK_INVALID_KEYCODE;
      }
    }
    /* Waiting for Latch_kh done*/
    while ((IID_CTRL_REG & IID_KH_LATCH_KEY_QK) == IID_KH_LATCH_KEY_QK)
      ;
    if ((kcdata[0] >> 24) == IID_256_BIT) {
      IID_CTRL_REG           = IID_KH_KEY_256 | IID_KH_OUT_RESET | IID_KH_SURCE_AES | IID_MAX_KEY_256 | key_addr;
      QK_AES_KEY_CONTROL_REG = AES_KEY_CHANGE_REQ | AES_KEY_SIZE_256;
    } else {
      IID_CTRL_REG           = IID_KH_OUT_RESET | IID_KH_SURCE_AES | key_addr;
      QK_AES_KEY_CONTROL_REG = AES_KEY_CHANGE_REQ;
    }

    /* Waiting For Key Present*/
    while ((QK_AES_STATUS_REG & QK_AES_KEY_PRESENT) != QK_AES_KEY_PRESENT)
      ;
    return QK_RETURN_SUCCESS;
  }
  return QK_RETURN_ERROR; //fixme check its usage
}
/*==============================================*/
/**
 * @fn            int32_t data_aes_mac(uint8_t mode, uint32_t *plain_data, uint32_t *iv, uint32_t *key, uint32_t data_size, uint32_t *mac)
 * @brief         This function is used to calculate MAC for the given bytes
 * @param[in]     Mode (Reserved for now)
 * 				  B1 : 1- Key Size 256, 0- Key size 128;
 * 				  B3 : 1-IV Size 256, 0- IV size 128;
 * @param[in]	  plain_data, Pointer to plain data
 * @param[in]	  iv, Pointer to IV
 * @param[in] 	  key, Pointer to AES key
 * @param[in]	  data_size, Plain data size(in Bytes)
 * @param[out]    mac, Pointer to Calculated MAC
 * @return :      Zer0 - if success
 * 		          Non-Zero - if fails
 */
int32_t data_aes_mac(uint8_t mode, uint32_t *plain_data, uint32_t *iv, uint32_t *key, uint32_t data_size, uint32_t *mac)
{

  uint32_t en_count, key_size;
  uint32_t i, j, k, no_of_iter, block_write_size;

  /*Check if given key code is valid or not*/
  if (data_size % AES_BLOCK_SIZE) {
    return QK_RETURN_INVALID_PARAM;
  }

  if (mode & MODE_KEY_SIZE_256)
    key_size = 32;
  else
    key_size = 16;

  block_write_size = (AES_BLOCK_SIZE / 4);
  no_of_iter       = data_size / AES_BLOCK_SIZE;

  if (mode & MODE_KEY_FROM_AES) {
    /*Programming registers according to the key size*/
    if ((key_size * 8) == BITS_256) {
      IID_CTRL_REG           = IID_MAX_KEY_256 | IID_KH_OUT_RESET;
      QK_AES_KEY_CONTROL_REG = AES_KEY_SRC_REG | AES_KEY_CHANGE_REQ | AES_KEY_SIZE_256;
    } else {
      IID_CTRL_REG           = IID_KH_OUT_RESET;
      QK_AES_KEY_CONTROL_REG = AES_KEY_SRC_REG | AES_KEY_CHANGE_REQ;
    }

    en_count = 0x0;
    while (en_count < (key_size / 4)) {
      /*Feeding the AES key*/
      while ((QK_AES_STATUS_REG & (QK_AES_KEY_RQST | QK_AES_KEY_PRESENT)) != QK_AES_KEY_RQST)
        ;
      QK_AES_KEY_REG = key[en_count];
      en_count       = en_count + 0x1;
    }
  }

  //!Added as a fix for AES - ECB and AES - CBC usage itteratively
  /*AES in idle state*/
  QK_AES_ACTION_REG = QK_AES_ACTION_IDLE;

  /*Configure AES in CBC mode*/
  QK_AES_MODE_REG = QK_AES_MODE_CBC;

  /*AES should be busy now*/
  while ((QK_AES_STATUS_REG & QK_AES_BUSY) != 0x0)
    ;

  /*Programming action to encode given data*/
  QK_AES_ACTION_REG = QK_AES_ACTION_ENCODE;

  /*waiting for the AES data request bit to be high*/
  while ((QK_AES_STATUS_REG & QK_AES_DATA_RQST) != QK_AES_DATA_RQST)
    ;
  /*Loading the  128 bits of random number*/

  //for IV Loading 16Bytes IV
  for (i = 0; i < 4; i++)
    QK_AES_DIN_REG = iv[i];
  //Fixme: Issue if IV size 256 bit
  if (mode & MODE_IV_SIZE_256) {
    /*waiting for the AES data request bit to be high*/
    while ((QK_AES_STATUS_REG & QK_AES_DATA_RQST) != QK_AES_DATA_RQST)
      ;

    for (i = 4; i < 8; i++)
      QK_AES_DIN_REG = iv[i];
  }

  for (i = 0; i < no_of_iter; i++) {
    /* After loading 4 words wait on register status */
    if (!(i % 4))
      while ((QK_AES_STATUS_REG & QK_AES_DATA_RQST) != QK_AES_DATA_RQST)
        ;

    for (j = 0; j < block_write_size; j++) {
      if (!(j % 4))
        while ((QK_AES_STATUS_REG & QK_AES_DATA_RQST) != QK_AES_DATA_RQST)
          ;

      QK_AES_DIN_REG = plain_data[j + (i * block_write_size)];
    }

    for (j = 0, k = 0; j < block_write_size; j++, k++) {
      if (!(j % 4)) {
        while ((QK_AES_STATUS_REG & QK_AES_DATA_AVAIL) != QK_AES_DATA_AVAIL)
          ;
        k = 0;
      }
      mac[k] = QK_AES_DOUT_REG; //overwrites 4 words
    }
  }
  return QK_RETURN_SUCCESS;
}
/*==============================================*/
/**
 * @fn            int32_t data_aes_encry_decry(uint8_t mode,
                             uint32_t *in_data,
                             uint32_t offset_in_flash,
                             uint32_t *iv,
                             uint32_t *key,
                             uint32_t data_size,
                             uint32_t *out_data,
                             uint32_t *nonce,
                             uint32_t type)
 * @brief         This function is used to Encrypt/Decrypt using AES engine either
 *                in CBC/ECB mode
 * @param[in]     Mode   B0 : 1 - CBC, 0 - ECB;
 * 						 B1 : 1 - Key Size 256, 0- Key size 128;
 * 						 B2 : 1 - Key from AES, 0-Key from PUF;
 * 						 B3 : 1 - IV Size 256, 0- IV size 128;
 * 						 B4 : 1 - Encrypt, 0- Decrypt;
 * @param[in]	  in_data, Pointer to Input data
 * @param[in] 	  iv, Pointer to IV
 * @param[in]	  key, Pointer to AES key
 * @param[in]	  data_size, Plain data size(in Bytes)
 * @para[out]     out_data, Pointer to Output Data
 * @return        Zero - if success
 * 		          Non-Zero - if fails
 */
int32_t data_aes_encry_decry(uint8_t mode,
                             uint32_t *in_data,
                             uint32_t offset_in_flash,
                             uint32_t *iv,
                             uint32_t *key,
                             uint32_t data_size,
                             uint32_t *out_data,
                             uint32_t *nonce,
                             uint32_t type)
{
  uint32_t en_count, key_size;
  uint32_t i, j, no_of_iter, block_write_size;
  //! REVIEW is it not dependent on key size
  uint32_t input_data[4];

  if (data_size % AES_BLOCK_SIZE) {
    return QK_RETURN_INVALID_PARAM;
  }

  //  if(type)
  //    lib_memcpy((uint8_t *)&input_data[1], (uint8_t *)nonce, 12);

  if (mode & MODE_KEY_SIZE_256)
    key_size = 32;
  else
    key_size = 16;

  block_write_size = (AES_BLOCK_SIZE / 4);
  no_of_iter       = data_size / AES_BLOCK_SIZE;

  if (mode & MODE_KEY_FROM_AES) {
    /*Programming registers according to the key size*/
    if ((key_size * 8) == BITS_256) {
      IID_CTRL_REG           = IID_MAX_KEY_256 | IID_KH_OUT_RESET;
      QK_AES_KEY_CONTROL_REG = AES_KEY_SRC_REG | AES_KEY_CHANGE_REQ | AES_KEY_SIZE_256;
    } else {
      IID_CTRL_REG           = IID_KH_OUT_RESET;
      QK_AES_KEY_CONTROL_REG = AES_KEY_SRC_REG | AES_KEY_CHANGE_REQ;
    }

    en_count = 0x0;
    while (en_count < (key_size / 4)) {
      /*Feeding the AES key*/
      while ((QK_AES_STATUS_REG & (QK_AES_KEY_RQST | QK_AES_KEY_PRESENT)) != QK_AES_KEY_RQST)
        ;
      QK_AES_KEY_REG = key[en_count];
      en_count       = en_count + 0x1;
    }
  }

  //!Added as a fix for AES - ECB and AES - CBC usage itteratively
  /*AES in idle state*/
  QK_AES_ACTION_REG = QK_AES_ACTION_IDLE;

  if (mode & MODE_CBC) {
    /*Configure AES in CBC mode*/
    QK_AES_MODE_REG = QK_AES_MODE_CBC;
  } else //ECB
  {
    QK_AES_MODE_REG = QK_AES_MODE_ECB;
  }

  /*AES should be busy now*/
  while ((QK_AES_STATUS_REG & QK_AES_BUSY) != 0x0)
    ;

  if (mode & MODE_ENCRYPT) { /*Programming action to encode given data*/
    QK_AES_ACTION_REG = QK_AES_ACTION_ENCODE;
  } else {
    /*Programming action to encode given data*/
    QK_AES_ACTION_REG = QK_AES_ACTION_DECODE;
  }

  if (mode & MODE_CBC) {
    /*waiting for the AES data request bit to be high*/
    while ((QK_AES_STATUS_REG & QK_AES_DATA_RQST) != QK_AES_DATA_RQST)
      ;

    //for Loading 16Bytes IV
    for (i = 0; i < 4; i++)
      QK_AES_DIN_REG = iv[i];

    //for Loading next 16Bytes IV if 32bytes IV
    if (mode & MODE_IV_SIZE_256) {
      /*waiting for the AES data request bit to be high*/
      while ((QK_AES_STATUS_REG & QK_AES_DATA_RQST) != QK_AES_DATA_RQST)
        ;

      for (i = 4; i < 8; i++)
        QK_AES_DIN_REG = iv[i];
    }
  }
  //!REVIEW, why assumed  16 bytes
  for (i = 0; i < no_of_iter; i++) {
    //! Icrement flash offset to next address(16 bytes aligned) offset
    if (type)
      input_data[0] = (offset_in_flash + (block_write_size * 4 * i));

    for (j = 0; j < block_write_size; j++) {
      if (!(j % 4))
        while ((QK_AES_STATUS_REG & QK_AES_DATA_RQST) != QK_AES_DATA_RQST)
          ;
      if (type)
        QK_AES_DIN_REG = input_data[j];
      else
        QK_AES_DIN_REG = in_data[j + (i * block_write_size)];
    }

    for (j = 0; j < block_write_size; j++) {
      /* waiting for the AES data available bit to be high*/
      if (!(j % 4))
        while ((QK_AES_STATUS_REG & QK_AES_DATA_AVAIL) != QK_AES_DATA_AVAIL)
          ;

      if (type)
        out_data[j + (i * block_write_size)] = (QK_AES_DOUT_REG) ^ in_data[j + (i * block_write_size)];
      else
        out_data[j + (i * block_write_size)] = QK_AES_DOUT_REG;
    }
  }

  return QK_RETURN_SUCCESS;
}
/*==============================================*/
/**
 * @fn           int32_t puf_set_intrensic_key(uint8_t keyindex, uint16_t keysize, uint32_t *keycode)
 * @brief        This function is used to generate key code for the intrinsic key
 * @param[in]    keyindex, key index
 * 				 keysize, key size (128/256)
 * @param[out]   keycode, key code
 * @return       Zero - if success
 * 		         Non zero - if fails
 */
int32_t puf_set_intrensic_key(uint8_t keyindex, uint16_t keysize, uint32_t *keycode)
{
  uint8_t intr_counter = 0x00;

  if (CHIP_MODE_CNFG_REG & BLCK_SET_KEY) {

    return QK_RETURN_OP_BLOCKED;
  }
  /* check  For set key Allow bit */
  if ((QK_ALLOW_REG & QK_ALLOW_SET_KEY) != QK_ALLOW_SET_KEY) {
    return QK_RETURN_OP_NOT_ALLOWED;
  }

  if ((keysize != BITS_256) && (keysize != BITS_128))
    return QK_RETURN_INVALID_PARAM; //size is not 128/256 bit

  /*Reset the IID*/
  IID_CTRL_REG = IID_RESET_DEFAULT;

  /*Key is read Through register*/
  QK_TARGET_REG = QK_TARGET_AS_REG;

  /*loading Index of key*/
  QK_INDEX_REG = keyindex;

  /*Extracting Key Size */
  QK_SIZE_REG = keysize >> 6;

  /* Begin SET intrensic KEY Operation*/
  QK_CONTROL_REG = QK_SET_IK;

  /*Check if command is accepted*/
  if ((!(QK_STATUS_REG & QK_BUSY)) || (QK_STATUS_REG & QK_ERROR)) {
    return QK_RETURN_QK_ERROR;
  }

  /* Reading Key code*/
  while (intr_counter < KEY_CODE_SIZE) {
    while ((QK_STATUS_REG & QK_CO_AV) != QK_CO_AV)
      ;
    keycode[intr_counter] = QK_CODE_OUTPUT_REG;
    intr_counter          = intr_counter + 0x1;
  }
  /*waiting for QK busy to go low , operation was success*/
  while ((QK_STATUS_REG & QK_BUSY_OK) != QK_OK)
    ;
  return QK_RETURN_SUCCESS;
}
/*==============================================*/
/**
 * @fn               int32_t data_aes_encry_decry_qspi(uint8_t mode, uint32_t* in_data, uint32_t offset_in_flash, uint32_t* iv, uint32_t * key, uint32_t data_size, uint32_t *out_data, uint32_t *nonce)
 * @brief            This function is used to Encrypt/Decrypt using AES engine either
 *                   in CBC/ECB mode. Supports only 128-bit(16 bytes) key.
 * @param[in]        Mode  B0 : 1 - CBC, 0 - ECB;
 * 						   B1 : 1 - Key Size 256, 0- Key size 128
 * 						   B2 : 1 - Key from AES, 0-Key from PUF
 * 						   B3 : 1 - IV Size 256, 0- IV size 128
 * 						   B4 : 1 - Encrypt, 0- Decrypt
 * @param[in]	 	 in_data, Pointer to Input data
 * @param[in]	 	 offset_in_flash, Input data offset in flash
 * @param[in] 	 	 iv, Pointer to IV
 * @param[in] 	 	 key, Pointer to AES key
 * @param[in] 	 	 sz, Plain data size(in Bytes)
 * @param[out]       out_data, Pointer to Output Data
 * @return :         Zero - if success
 * 		             Non-Zero - if fails
 */
#if 0

int32_t data_aes_encry_decry_qspi(uint8_t mode, uint32_t* in_data, uint32_t offset_in_flash, uint32_t* iv, uint32_t * key, uint32_t data_size, uint32_t *out_data, uint32_t *nonce)
{
	uint32_t en_count,key_size;
	uint32_t keysz,i,j,no_of_iter;
  //! REVIEW is it not dependent on key size
   uint32_t input_data[4]; 
  //uint32_t output_data[4];

  lib_memcpy((uint8_t *)&input_data[1], (uint8_t *)nonce, 12);

	if(mode & MODE_KEY_SIZE_256)
		key_size = 32;
	else
		key_size = 16;

  /* To load the key 4 bytes at a time, so keysz represent in number of dwords */
	keysz = (key_size / 4);
	key_size *= 8 ; //Bytes into bits
	no_of_iter = data_size/(keysz * 4);


	if(data_size % (key_size/8))
	{
		return QK_RETURN_INVALID_PARAM;
	}

	if(mode & MODE_KEY_FROM_AES)
	{
		/*Programming registers according to the key size*/
		if(key_size==BITS_256)
		{
			IID_CTRL_REG = IID_MAX_KEY_256;
			QK_AES_KEY_CONTROL_REG  = AES_KEY_SRC_REG|AES_KEY_CHANGE_REQ|AES_KEY_SIZE_256;
		}
		else
		{
			IID_CTRL_REG = 0;
			QK_AES_KEY_CONTROL_REG  = AES_KEY_SRC_REG|AES_KEY_CHANGE_REQ;
		}

		en_count = 0x0;
		while(en_count < keysz)
		{
			/*Feeding the AES key*/
			while((QK_AES_STATUS_REG & (QK_AES_KEY_RQST|QK_AES_KEY_PRESENT)) != QK_AES_KEY_RQST);
			QK_AES_KEY_REG = key[en_count];
			en_count = en_count + 0x1;
		}
	}
	
	//!Added as a fix for AES - ECB and AES - CBC usage itteratively
	/*AES in idle state*/
	QK_AES_ACTION_REG   = QK_AES_ACTION_IDLE;	

	if(mode & MODE_CBC)
	{
		/*Configure AES in CBC mode*/
		QK_AES_MODE_REG     = QK_AES_MODE_CBC;
	}
	else //ECB
	{
		QK_AES_MODE_REG     = QK_AES_MODE_ECB;
	}

	/*AES should be busy now*/
	while((QK_AES_STATUS_REG & QK_AES_BUSY) != 0x0);

	if(mode & MODE_ENCRYPT)
	{	/*Programming action to encode given data*/
		QK_AES_ACTION_REG   = QK_AES_ACTION_ENCODE;
	}
	else
	{
		/*Programming action to encode given data*/
		QK_AES_ACTION_REG   = QK_AES_ACTION_DECODE;
	}

	if(mode & MODE_CBC)
	{
		/*waiting for the AES data request bit to be high*/
		while((QK_AES_STATUS_REG & QK_AES_DATA_RQST) != QK_AES_DATA_RQST);

		//for Loading 16Bytes IV
		for(i=0; i < 4; i++)
			QK_AES_DIN_REG  = iv[i];

		//for Loading next 16Bytes IV if 32bytes IV
		if(mode & MODE_IV_SIZE_256)
		{
			/*waiting for the AES data request bit to be high*/
			while((QK_AES_STATUS_REG & QK_AES_DATA_RQST) != QK_AES_DATA_RQST);

			for(i=4; i < 8; i++)
				QK_AES_DIN_REG  = iv[i];
		}
	}
  //!REVIEW, why assumed  16 bytes
	for(i=0; i < no_of_iter; i++)
	{
    //! Icrement flash offset to next address(16 bytes aligned) offset
    input_data[0] = (offset_in_flash + (keysz * 4 *i));

		for(j=0; j < keysz; j++)
		{
			if(!(j%4))
				while((QK_AES_STATUS_REG & QK_AES_DATA_RQST) != QK_AES_DATA_RQST);

			QK_AES_DIN_REG  = input_data[j];
		}

		for(j=0; j < keysz; j++)
		{
			/* waiting for the AES data available bit to be high*/
			if(!(j%4))
				while((QK_AES_STATUS_REG & QK_AES_DATA_AVAIL) != QK_AES_DATA_AVAIL);

     // output_data[j] = QK_AES_DOUT_REG;
			out_data[j+(i*keysz)] = (QK_AES_DOUT_REG) ^ in_data[j+(i*keysz)];
		}
	}

	return QK_RETURN_SUCCESS;
}
#endif
#endif
