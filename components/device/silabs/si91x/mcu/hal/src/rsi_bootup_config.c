/*******************************************************************************
* @file  rsi_bootup_config.c
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

//Includes
#include "rsi_api.h"
#include "rsi_driver.h"
#include "sl_si91x_constants.h"

/** @addtogroup SOC4
* @{
*/

/*-------To ignore -Wcast-align warnings--------*/
#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wcast-align"
#endif
/*===========================================================================*/
/** 
 * @fn          int16 rsi_mem_wr(uint32 addr, uint16 len, uint8 *dBuf)
 * @brief       Performs a memory write to the Wi-Fi module
 * @param[in]   uint32 addr, address to write to
 * @param[in]   uint16, len, number of bytes to write
 * @param[in]   uint8 *dBuf, pointer to the buffer of data to write
 * @param[out]  none
 * @return      errCode
 *              -1 = SPI busy / Timeout
 *              -2 = SPI Failure
 *              0  = SUCCESS
 *
 * ABH Master Write (Internal Legacy Name)
 */
int16_t rsi_mem_wr(volatile uint32_t addr, uint16_t len, uint8_t *dBuf)
{
  (void)len;
  *(uint32_t *)addr = *(uint32_t *)dBuf;

  return 0;
}

/*===========================================================================*/
/**
 * @fn          int16 rsi_mem_rd(uint32 addr, uint16 len, uint8 *dBuf)
 * @brief       Performs a memory read from the Wi-Fi module
 * @param[in]   uint32, address to read from
 * @param[in]   uint16, len, number of bytes to read
 * @param[in]   uint8 *dBuf, pointer to the buffer to receive the data into
 * @param[out]  none
 * @return      errCode
 *              -1 = SPI busy / Timeout
 *              -2 = SPI Failure
 *              0  = SUCCESS
 *
 * ABH Master Read (Internal Legacy Name)
 */
int16_t rsi_mem_rd(volatile uint32_t addr, uint16_t len, uint8_t *dBuf)
{
  (void)len;
  *(uint32_t *)dBuf = *(uint32_t *)addr;
  return 0;
}

/*==============================================*/
/**
 * @fn          int16 rsi_boot_insn(uint8 type, uint16 *data)
 * @brief       Sends boot instructions to WiFi module
 * @param[in]   uint8 type, type of the insruction to perform
 * @param[in]   uint32 *data, pointer to data which is to be read/write
 * @param[out]  none
 * @return      errCode
 *              < 0  = Command issued failure/Invalid command 
 *                0  = SUCCESS
 *              > 0  = Read value
 * @section description 
 * This API is used to send boot instructions to WiFi module.
 */

int16_t rsi_boot_insn(uint8_t type, uint16_t *data)
{
  int16_t retval                = 0;
  uint16_t local                = 0;
  uint32_t j                    = 0;
  uint32_t cmd                  = 0;
  uint16_t read_data            = 0;
  volatile int32_t loop_counter = 0;
#ifdef RSI_DEBUG_PRINT
  RSI_DPRINT(RSI_PL3, "\nBootInsn\n");
#endif

  switch (type) {
    case REG_READ:
      *data = SI91X_INTERFACE_OUT_REGISTER;
      break;

    case REG_WRITE:
      retval = rsi_mem_wr(HOST_INTF_REG_IN, 2, (uint8_t *)data);
      break;

    case PING_WRITE:

      for (j = 0; j < 2048; j++) {
        retval = rsi_mem_wr(0x19000 + (j * 2), 2, (uint8_t *)((uint32_t)data + (j * 2)));
        if (retval < 0) {
          return retval;
        }
      }

      local  = 0xab49;
      retval = rsi_mem_wr(HOST_INTF_REG_IN, 2, (uint8_t *)&local);
      break;

    case PONG_WRITE:

      for (j = 0; j < 2048; j++) {
        retval = rsi_mem_wr(0x1a000 + (j * 2), 2, (uint8_t *)((uint32_t)data + (j * 2)));
        if (retval < 0) {
          return retval;
        }
      }
      // Perform the write operation
      local  = 0xab4f;
      retval = rsi_mem_wr(HOST_INTF_REG_IN, 2, (uint8_t *)&local);
      break;

    case BURN_NWP_FW:
      cmd    = BURN_NWP_FW | HOST_INTERACT_REG_VALID;
      retval = rsi_mem_wr(HOST_INTF_REG_IN, 2, (uint8_t *)&cmd);
      if (retval < 0) {
        return retval;
      }

      RSI_RESET_LOOP_COUNTER(loop_counter);
      RSI_WHILE_LOOP(loop_counter, RSI_LOOP_COUNT_UPGRADE_IMAGE)
      {
        read_data = SI91X_INTERFACE_OUT_REGISTER;
        if (read_data == (uint16_t)(RSI_SEND_RPS_FILE | HOST_INTERACT_REG_VALID)) {
          break;
        }
      }
      RSI_CHECK_LOOP_COUNTER(loop_counter, RSI_LOOP_COUNT_UPGRADE_IMAGE);
      break;

    case LOAD_NWP_FW:
      cmd    = LOAD_NWP_FW | HOST_INTERACT_REG_VALID;
      retval = rsi_mem_wr(HOST_INTF_REG_IN, 2, (uint8_t *)&cmd);
      break;
    case LOAD_DEFAULT_NWP_FW_ACTIVE_LOW:
      cmd    = LOAD_DEFAULT_NWP_FW_ACTIVE_LOW | HOST_INTERACT_REG_VALID;
      retval = rsi_mem_wr(HOST_INTF_REG_IN, 2, (uint8_t *)&cmd);
      break;
    case RSI_UPGRADE_BL:
      cmd    = RSI_UPGRADE_BL | HOST_INTERACT_REG_VALID;
      retval = rsi_mem_wr(HOST_INTF_REG_IN, 2, (uint8_t *)&cmd);
      if (retval < 0) {
        return retval;
      }
      RSI_RESET_LOOP_COUNTER(loop_counter);
      RSI_WHILE_LOOP(loop_counter, RSI_LOOP_COUNT_UPGRADE_IMAGE)
      {
        read_data = SI91X_INTERFACE_OUT_REGISTER;
        if (read_data == (uint16_t)(RSI_SEND_RPS_FILE | HOST_INTERACT_REG_VALID)) {
          break;
        }
      }
      RSI_CHECK_LOOP_COUNTER(loop_counter, RSI_LOOP_COUNT_UPGRADE_IMAGE);
      break;
    default:
      retval = -2;
      break;
  }
  return retval;
}

/*==============================================*/
/**
 * @fn          int16 rsi_waitfor_boardready(void)
 * @brief       Waits to receive board ready from WiFi module
 * @param[in]   none
 * @param[out]  none
 * @return      errCode
 *              0  = SUCCESS
 *              < 0 = Failure 
 *              -3 = Board ready not received
 *              -4 = Bootup options last configuration not saved
 *              -5 = Bootup options checksum failed
 *              -6 = Bootloader version mismatch 
 * @section description 
 * This API is used to check board ready from WiFi module.
 */
int16_t rsi_waitfor_boardready(void)
{
  int16_t retval      = 0;
  uint16_t read_value = 0;

  retval = rsi_boot_insn(REG_READ, &read_value);

  if (retval < 0) {
    return retval;
  }
  if (read_value == 0) {
    return -7;
  }
  if ((read_value & 0xFF00) == (HOST_INTERACT_REG_VALID_READ & 0xFF00)) {
    if ((read_value & 0xFF) == RSI_BOOTUP_OPTIONS_LAST_CONFIG_NOT_SAVED) {
#ifdef RSI_DEBUG_PRINT
      RSI_DPRINT(RSI_PL3, "BOOTUP OPTIOINS LAST CONFIGURATION NOT SAVED\n");
#endif
      return -4;
    } else if ((read_value & 0xFF) == RSI_BOOTUP_OPTIONS_CHECKSUM_FAIL) {
#ifdef RSI_DEBUG_PRINT
      RSI_DPRINT(RSI_PL3, "BOOTUP OPTIONS CHECKSUM FAIL\n");
#endif
      return -5;
    }
#if defined(BOOTLOADER_VERSION_CHECK) && (BOOTLOADER_VERSION_CHECK == 1)
    else if ((read_value & 0xFF) == BOOTLOADER_VERSION) {
#ifdef RSI_DEBUG_PRINT
      RSI_DPRINT(RSI_PL3, "BOOTLOADER VERSION CORRECT\n");
#endif
    } else {
#ifdef RSI_DEBUG_PRINT
      RSI_DPRINT(RSI_PL3, "BOOTLOADER VERSION NOT MATCHING\n");
#endif

      return -6;
    }
#endif

#ifdef RSI_DEBUG_PRINT
    RSI_DPRINT(RSI_PL3, "RECIEVED BOARD READY\n");
#endif
    return 0;
  }

#ifdef RSI_DEBUG_PRINT
  RSI_DPRINT(RSI_PL3, "WAITING FOR BOARD READY\n");
#endif
  return -3;
}

/*==============================================*/
/**
 * @fn          int16 rsi_select_option(uint8 cmd)
 * @brief       Sends cmd to select option to load or update configuration 
 * @param[in]   uint8 cmd, type of configuration to be saved
 * @param[out]  none
 * @return      errCode
                < 0 = Command issue failed
 *              0  = SUCCESS
 * @section description 
 * This API is used to send firmware load request to WiFi module or update default configurations.
 */
int16_t rsi_select_option(uint8_t cmd)
{
  uint16_t boot_cmd             = 0;
  int16_t retval                = 0;
  uint16_t read_value           = 0;
  uint8_t image_number          = 0;
  volatile int32_t loop_counter = 0;

  boot_cmd = (uint16_t)(HOST_INTERACT_REG_VALID | cmd);
  if (cmd == CHECK_NWP_INTEGRITY) {
    boot_cmd &= 0xF0FF;
    boot_cmd |= (uint16_t)(image_number << 8);
  }
  retval = rsi_boot_insn(REG_WRITE, &boot_cmd);
  if (retval < 0) {
    return retval;
  }

  if ((cmd != LOAD_NWP_FW) && (cmd != LOAD_DEFAULT_NWP_FW_ACTIVE_LOW) && (cmd != RSI_JUMP_TO_PC)) {
    RSI_RESET_LOOP_COUNTER(loop_counter);
    RSI_WHILE_LOOP(loop_counter, RSI_LOOP_COUNT_SELECT_OPTION)
    {
      retval = rsi_boot_insn(REG_READ, &read_value);
      if (retval < 0) {
        return retval;
      }
      if (cmd == CHECK_NWP_INTEGRITY) {
        if ((read_value & 0xFF) == RSI_CHECKSUM_SUCCESS) {
#ifdef RSI_DEBUG_PRINT
          RSI_DPRINT(RSI_PL3, "CHECKSUM SUCCESS\n");
#endif
        } else if (read_value == RSI_CHECKSUM_FAILURE) {
#ifdef RSI_DEBUG_PRINT
          RSI_DPRINT(RSI_PL3, "CHECKSUM FAIL\n");
#endif
        } else if (read_value == RSI_CHECKSUM_INVALID_ADDRESS) {
#ifdef RSI_DEBUG_PRINT
          RSI_DPRINT(RSI_PL3, "Invalid Address \n");
#endif
        }
      }
      if (read_value == (uint16_t)(HOST_INTERACT_REG_VALID | cmd)) {
        break;
      }
    }
    RSI_CHECK_LOOP_COUNTER(loop_counter, RSI_LOOP_COUNT_SELECT_OPTION);
  } else if ((cmd == LOAD_NWP_FW) || (cmd == LOAD_DEFAULT_NWP_FW_ACTIVE_LOW) || (cmd == RSI_JUMP_TO_PC)) {
    retval = rsi_boot_insn(REG_READ, &read_value);
    if (retval < 0) {
      return retval;
    }
    if ((read_value & 0xFF) == VALID_FIRMWARE_NOT_PRESENT) {
#ifdef RSI_DEBUG_PRINT
      RSI_DPRINT(RSI_PL3, "VALID_FIRMWARE_NOT_PRESENT\n");
#endif
      return -1;
    }
    if ((read_value & 0xFF) == RSI_INVALID_OPTION) {
#ifdef RSI_DEBUG_PRINT
      RSI_DPRINT(RSI_PL3, "INVALID CMD\n");
#endif
      return -1;
    }
  }
  return retval;
}
/*==============================================*/
/**
 * @fn          int16_t sl_si91x_upgrade_firmware(uint8_t *firmware_image , uint32_t fw_image_size, uint8_t flags)
 * @brief       Upgrades firmware to WiFi module
 * @param[in]   uint8_t image_type, type of firmware image to upgrade
 * @param[in]   uint8_t *firmware_image, pointer to firmware
 * @param[in]   uint32_t fw_image_size, size of firmware image
 * @param[out]  none
 * @return      errCode
 *              <0 = Command issue failed
 *              0  = SUCCESS
 * @section description 
 * This API is used to upgrade firmware to WiFi module.
 */
int16_t sl_si91x_bl_upgrade_firmware(uint8_t *firmware_image, uint32_t fw_image_size, uint8_t flags)
{
  static uint16_t boot_cmd = 0;
  uint16_t read_value      = 0;
  uint32_t offset          = 0;
  int16_t retval           = 0;
  uint32_t boot_insn = 0, poll_resp = 0;
  uint32_t timer_instance;

  //! If it is a start of file set the boot cmd to pong valid
  if (flags & RSI_FW_START_OF_FILE) {
    boot_cmd = RSI_HOST_INTERACT_REG_VALID | RSI_PONG_VALID;
  }

  //! check for invalid packet
  if ((fw_image_size % (RSI_MIN_CHUNK_SIZE) != 0) && (!(flags & RSI_FW_END_OF_FILE))) {
    return -1;
  }

  //! loop to execute multiple of 4K chunks
  while (offset < fw_image_size) {
    switch (boot_cmd) {
      case (RSI_HOST_INTERACT_REG_VALID | RSI_PING_VALID):
        boot_insn = RSI_PONG_WRITE;
        poll_resp = RSI_PING_AVAIL;
        boot_cmd  = RSI_HOST_INTERACT_REG_VALID | RSI_PONG_VALID;
        break;

      case (RSI_HOST_INTERACT_REG_VALID | RSI_PONG_VALID):
        boot_insn = RSI_PING_WRITE;
        poll_resp = RSI_PONG_AVAIL;
        boot_cmd  = RSI_HOST_INTERACT_REG_VALID | RSI_PING_VALID;
        break;
    }

    retval = rsi_boot_insn(boot_insn, (uint16_t *)((uint8_t *)firmware_image + offset));
    if (retval < 0) {
      return retval;
    }

    while (1) {
      retval = rsi_boot_insn(RSI_REG_READ, &read_value);
      if (retval < 0) {
        return retval;
      }

      if (read_value == (RSI_HOST_INTERACT_REG_VALID | poll_resp)) {
        break;
      }
    }
    offset += RSI_MIN_CHUNK_SIZE;
  }

  //! For last chunk set boot cmd as End of file reached
  if (flags & RSI_FW_END_OF_FILE) {
    boot_cmd = RSI_HOST_INTERACT_REG_VALID | RSI_EOF_REACHED;

    retval = rsi_boot_insn(RSI_REG_WRITE, &boot_cmd);
    if (retval < 0) {
      return retval;
    }

    //! check for successful firmware upgrade
    do {
      retval = rsi_boot_insn(RSI_REG_READ, &read_value);
      if (retval < 0) {
        return retval;
      }

    } while (read_value != (RSI_HOST_INTERACT_REG_VALID | RSI_FWUP_SUCCESSFUL));
  }
  return retval;
}

/*==============================================*/
/**
 * @fn          int32_t sl_si91x_set_fast_fw_up(void)
 * @brief       Set fast firmware upgrade.
 * @param[in]   void 
 * @return      0              - Success \n 
 * @return      Non-Zero Value - Failure
 */
int32_t sl_si91x_set_fast_fw_up(void)
{
  uint32_t read_data = 0;
  int32_t retval     = 0;
  retval             = rsi_mem_rd(RSI_SAFE_UPGRADE_ADDR, 4, (uint8_t *)&read_data);
  if (retval < 0) {
#ifdef RSI_DEBUG_PRINT
    RSI_DPRINT(RSI_PL3, "retval: %d", retval);
#endif
    return retval;
  }
  //disabling safe upgradation bit
  if ((read_data & RSI_SAFE_UPGRADE)) {
    read_data &= ~(RSI_SAFE_UPGRADE);
    retval = rsi_mem_wr(RSI_SAFE_UPGRADE_ADDR, 4, (uint8_t *)&read_data);
    if (retval < 0) {
#ifdef RSI_DEBUG_PRINT
      RSI_DPRINT(RSI_PL3, "retval: %d", retval);
#endif
      return retval;
    }
  }
#ifdef RSI_DEBUG_PRINT
  RSI_DPRINT(RSI_PL3, "retval: %d", retval);
#endif
  return retval;
}

/** @} */
