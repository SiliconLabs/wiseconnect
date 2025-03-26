/***************************************************************************/ /**
 * @file  sl_si91x_fw_fallback.c
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

#include "rsi_debug.h"
#include "sl_si91x_fw_fallback.h"
#include "base_types.h"
#include "si91x_device.h"
#include "core_cm4.h"
#ifdef SL_SI91X_FW_FALLBACK
#include "sl_si91x_driver.h"
#include "sli_siwx917_soc.h"
#include "sl_device.h"
#include "sli_siwx917_soc.h"
#include "sl_si91x_types.h"
#endif

#ifdef SL_SI91X_FW_FALLBACK
static uint32_t ota_image_address = 0; ///<        This variable holds the address of the OTA image.
static uint32_t ota_image_type    = 0; ///<        This variable holds the type of the OTA image.
static sl_status_t sli_si91x_erase_slot_info(uint32_t flash_offset);
static sl_status_t sli_si91x_calculate_firmware_slot_crc(sl_si91x_fw_ab_slot_management_t *slot_info);
static void sli_si91x_update_crc(sl_si91x_fw_ab_slot_management_t *slot_info);
#endif

#ifdef SL_SI91X_FW_FALLBACK
/***************************************************************************/ /**
 *  @fn          sl_status_t sl_si91x_ab_upgrade_get_rps_configs(const uint8_t *image_buffer, ota_image_info_t *ota_st)
 *  @pre         None
 *  @brief       Retrieves A/B fallback OTA image configuration from the provided image buffer.
 *               This function checks the magic number in the image buffer to validate the
 *               firmware header. If valid, it populates the ota_image_info_t structure with
 *               the image offset and size from the firmware header.
 *  @param[in]   image_buffer       Pointer to the buffer containing the firmware image.
 *  @param[out]  ota_st             Pointer to the ota_image_info_t structure to be populated.
 *  @return      sl_status_t        SL_STATUS_OK on success, or SL_STATUS_FAIL on failure.
 ******************************************************************************/
sl_status_t sl_si91x_ab_upgrade_get_rps_configs(const uint8_t *image_buffer, ota_image_info_t *ota_st)
{
  if ((image_buffer == NULL) || (ota_st == NULL)) {
    return SL_STATUS_NULL_POINTER;
  }

  const sl_si91x_firmware_header_t *rps_config_t = (const sl_si91x_firmware_header_t *)image_buffer;
  DEBUGOUT("\r\n\r\n magic_no:%lx\r\n", rps_config_t->magic_no);

  if (rps_config_t->magic_no == SLI_SI91X_RPS_MAGIC_NO) {

    // Extract image type from bit 0 of control_flags
    ota_st->ota_image_type   = (uint8_t)(rps_config_t->control_flags & BIT(0));
    ota_st->ota_image_offset = rps_config_t->flash_location;

    // Adjust the offset based on ota_image_type
    if ((ota_st->ota_image_type & BIT(0)) == SL_SI91X_M4_RPS_BIT) { // Ensuring bit-masking check
      ota_st->ota_image_offset += SLI_SI91X_M4_FLASH_BASE_ADDR;
    } else if ((ota_st->ota_image_type & BIT(0)) == SL_SI91X_NWP_RPS_BIT) {
      ota_st->ota_image_offset += SLI_SI91X_NWP_FLASH_BASE_ADDR;
    } else {
      DEBUGOUT("\r\nError: Unknown image type!\n");
      return SL_SI91X_AB_ERR_UNKNOWN_IMG;
    }

    ota_st->ota_image_size = rps_config_t->image_size + SLI_SI91X_RPS_HEADER_SIZE;

    ota_image_address = rps_config_t->flash_location;
    ota_image_type    = ota_st->ota_image_type;
    return SL_STATUS_OK;
  } else {
    DEBUGOUT("\r\nError: Magic number mismatch!\n");
    return SL_SI91X_AB_ERR_MAGIC_NUMBER;
  }
}

/***************************************************************************/ /**
 *  @fn          sl_status_t sl_si91x_flash_write(uint32_t address, const uint8_t *buffer, uint32_t length)
 *  @pre         None
 *  @brief       Write data to flash memory.
 *               This function writes data to the specified flash memory address.
 *               This function will work for A/B firmware only.
 *  @param[in]   address            Flash memory address to write to ( for M4: 0x8xxxxxx and for NWP: 0x4xxxxxx ).
 *  @param[in]   buffer             Pointer to the data buffer.
 *  @param[in]   length             Number of bytes to write.
 *  @return      sl_status_t        SL_STATUS_OK on success, error code otherwise.
 ******************************************************************************/
sl_status_t sl_si91x_flash_write(uint32_t address, const uint8_t *buffer, uint32_t length)
{
  sl_status_t status                        = SL_STATUS_FAIL;
  sl_si91x_fw_fallback_request_t fw_request = { 0 };
  size_t request_size                       = 0;

  // Validate length (ensure it fits within the buffer limit)
  if ((length > SL_SI91X_MAX_OTA_IMAGE_CHUNK_SIZE) || (buffer == NULL) || (length == 0)) {
    return SL_STATUS_INVALID_PARAMETER; // Error if length exceeds 1024 bytes
  }

  // Populate request structure
  fw_request.sub_command  = SL_SI91X_AB_FALLBACK_WRITE_CMD;
  fw_request.chunk_type   = SL_SI91X_DATA_PACKET;
  fw_request.data_length  = length;
  fw_request.flash_offset = address;
  memcpy(fw_request.data, buffer, length);

  request_size = offsetof(sl_si91x_fw_fallback_request_t, data) + length;

  // Send firmware update command
  status = sl_si91x_driver_send_command(SLI_SI91X_FW_FALLBACK_REQ_FROM_HOST,
                                        SI91X_COMMON_CMD,
                                        &fw_request,
                                        request_size,
                                        SL_SI91X_WAIT_FOR_RESPONSE(SL_SI91X_NWP_RESPONSE_TIMEOUT),
                                        NULL,
                                        NULL);

  return status;
}
/***************************************************************************/ /**
 *  @fn          sl_status_t sl_si91x_verify_image(uint32_t flash_address)
 *  @pre         None
 *  @brief       Checks the integrity of the firmware for fallback.
 *               This function sends a command to check the integrity of the firmware
 *               at the specified flash memory address. It populates the request structure
 *               with the necessary parameters and sends the command to the firmware.
 *               This function will work for A/B firmware only.
 *  @param[in]   address            Flash address to check integrity( for M4: 0x8xxxxxx and for NWP: 0x4xxxxxx ).
 *  @return      sl_status_t        SL_STATUS_OK on success, error code otherwise.
 ******************************************************************************/
sl_status_t sl_si91x_verify_image(uint32_t flash_address)
{
  sl_status_t status                        = SL_STATUS_FAIL;
  sl_si91x_fw_fallback_request_t fw_request = { 0 };

  // Populate request structure
  fw_request.chunk_type   = SL_SI91X_DATA_PACKET;
  fw_request.sub_command  = SL_SI91X_AB_FALLBACK_INTEGRITY_CHECK_CMD;
  fw_request.flash_offset = flash_address;

  size_t request_size = offsetof(sl_si91x_fw_fallback_request_t, data);

  // DEBUGOUT("\r\n integratiy+request_size:%u %u \r\n", sizeof(sl_si91x_fw_fallback_request_t), request_size);

  // Send firmware update command
  status = sl_si91x_driver_send_command(SLI_SI91X_FW_FALLBACK_REQ_FROM_HOST,
                                        SI91X_COMMON_CMD,
                                        &fw_request,
                                        request_size,
                                        SL_SI91X_WAIT_FOR_RESPONSE(SL_SI91X_NWP_RESPONSE_TIMEOUT),
                                        NULL,
                                        NULL);
  return status;
}

/***************************************************************************/ /**
 *  @fn          sl_status_t sl_si91x_flash_erase(uint32_t address, uint32_t length)
 *  @pre         None
 *  @brief       Erase data from flash memory.
 *               This function will Round up to the nearest 4KB boundary and erases data from the specified flash memory address.
 *               This function will work for A/B firmware only.
 *  @param[in]   address            Flash memory address to erase( for M4: 0x8xxxxxx and for NWP: 0x4xxxxxx ).
 *  @param[in]   length             Number of bytes to erase.
 *  @return      sl_status_t        SL_STATUS_OK on success, error code otherwise.
 ******************************************************************************/
sl_status_t sl_si91x_flash_erase(uint32_t address, uint32_t length)
{
  sl_status_t status                        = SL_STATUS_FAIL;
  sl_si91x_fw_fallback_request_t fw_request = { 0 };
  sl_si91x_fw_ab_slot_management_t slot_info;
  uint32_t erase_size = 0;

  // Get slot information
  status = sl_si91x_ab_get_slot_info(&slot_info);
  if (status != SL_STATUS_OK) {
    DEBUGOUT("\r\nError: Failed to retrieve slot information.\n");
    return status;
  }

  // Check if the address matches the active slot for M4 or NWP
  if ((address == slot_info.m4_slot_info.m4_slot_A.slot_image_offset
       && slot_info.m4_slot_info.current_active_M4_slot == SLOT_A)
      || (address == slot_info.m4_slot_info.m4_slot_B.slot_image_offset
          && slot_info.m4_slot_info.current_active_M4_slot == SLOT_B)
      || (address == slot_info.nwp_slot_info.nwp_slot_A.slot_image_offset
          && slot_info.nwp_slot_info.current_active_nwp_slot == SLOT_A)
      || (address == slot_info.nwp_slot_info.nwp_slot_B.slot_image_offset
          && slot_info.nwp_slot_info.current_active_nwp_slot == SLOT_B)) {
    DEBUGOUT("\r\nError: Attempted to erase active slot. Erase operation aborted.\n");
    return SL_SI91X_AB_ERR_ERASE_ACTIVE_SLOT;
  }

  erase_size = (length + 4095) & ~4095; // Round up to the nearest 4KB boundary

  // Populate request structure
  fw_request.sub_command  = SL_SI91X_AB_FALLBACK_ERASE_CMD;
  fw_request.data_length  = erase_size;
  fw_request.flash_offset = address;

  size_t request_size = offsetof(sl_si91x_fw_fallback_request_t, data);

  // Send firmware update command
  status = sl_si91x_driver_send_command(SLI_SI91X_FW_FALLBACK_REQ_FROM_HOST,
                                        SI91X_COMMON_CMD,
                                        &fw_request,
                                        request_size,
                                        SL_SI91X_WAIT_FOR_RESPONSE(SL_SI91X_NWP_RESPONSE_TIMEOUT),
                                        NULL,
                                        NULL);

  return status;
}

/***************************************************************************/ /**
 *  @fn          sl_status_t sl_si91x_ab_upgrade_set_slot_info(uint32_t new_image_offset, uint32_t new_image_size, sl_si91x_ab_image_type_t image_type)
 *  @pre         None
 *  @brief       Updates slot information and switches active slots on OTA updates.
 *               This function updates the slot information and switches the active slots
 *               based on the provided parameters.
 *               This function will work for A/B firmware only.
 *  @param[in]   new_image_offset   New image base address (used only during OTA update).
 *  @param[in]   new_image_size     New image size (used only during OTA update).
 *  @param[in]   image_type         Type of the image (M4 or NWP).
 *  @return      sl_status_t        SL_STATUS_OK if successful, error code otherwise.
 ******************************************************************************/
sl_status_t sl_si91x_ab_upgrade_set_slot_info(uint32_t new_image_offset,
                                              uint32_t new_image_size,
                                              sl_si91x_ab_image_type_t image_type)
{
  sl_status_t status                                   = SL_STATUS_FAIL;
  sl_si91x_fw_ab_slot_management_t fw_slot_info        = { 0 };
  sl_si91x_fw_ab_slot_management_t verify_fw_slot_info = { 0 };

  // Error check for new_image_offset(for M4: 0x8xxxxxx and for NWP: 0x4xxxxxx)
  if (((new_image_offset & 0xF0000000) != (SLI_SI91X_M4_FLASH_BASE_ADDR & 0xF0000000))
      && ((new_image_offset & 0xF0000000) != (SLI_SI91X_NWP_FLASH_BASE_ADDR & 0xF0000000))) {
    return SL_STATUS_INVALID_PARAMETER; // Invalid offset
  }

  // Read current slot info from flash
  status = sl_si91x_flash_read(SLI_SI91X_SLOT_INFO_FLASH_OFFSET,
                               (uint8_t *)&fw_slot_info,
                               sizeof(sl_si91x_fw_ab_slot_management_t));
  if (status != SL_STATUS_OK) {
    return SL_SI91X_AB_ERR_FLASH_READ;
  }

  bool need_flash_update =
    ((image_type == SL_SI91X_AB_OTA_IMAGE_TYPE_M4) || (image_type == SL_SI91X_AB_OTA_IMAGE_TYPE_NWP));

  if (need_flash_update) {
    // Handle OTA update for M4
    if (image_type == SL_SI91X_AB_OTA_IMAGE_TYPE_M4) {
      DEBUGOUT("\r\nProcessing Slot-Info update for M4\r\n");
      if (fw_slot_info.m4_slot_info.current_active_M4_slot == SLOT_A) {
        fw_slot_info.m4_slot_info.current_active_M4_slot      = SLOT_B;
        fw_slot_info.m4_slot_info.m4_slot_B.slot_image_offset = new_image_offset;
        fw_slot_info.m4_slot_info.m4_slot_B.image_size        = new_image_size;
      } else {
        fw_slot_info.m4_slot_info.current_active_M4_slot      = SLOT_A;
        fw_slot_info.m4_slot_info.m4_slot_A.slot_image_offset = new_image_offset;
        fw_slot_info.m4_slot_info.m4_slot_A.image_size        = new_image_size;
      }
    }

    // Handle OTA update for NWP
    if (image_type == SL_SI91X_AB_OTA_IMAGE_TYPE_NWP) {
      DEBUGOUT("\r\nProcessing SLot_Info update for NWP\r\n");
      if (fw_slot_info.nwp_slot_info.current_active_nwp_slot == SLOT_A) {
        fw_slot_info.nwp_slot_info.current_active_nwp_slot      = SLOT_B;
        fw_slot_info.nwp_slot_info.nwp_slot_B.slot_image_offset = new_image_offset;
        fw_slot_info.nwp_slot_info.nwp_slot_B.image_size        = new_image_size;
      } else {
        fw_slot_info.nwp_slot_info.current_active_nwp_slot      = SLOT_A;
        fw_slot_info.nwp_slot_info.nwp_slot_A.slot_image_offset = new_image_offset;
        fw_slot_info.nwp_slot_info.nwp_slot_A.image_size        = new_image_size;
      }
    }

    // Update magic word and CRC
    fw_slot_info.slot_magic_word = SLI_SI91X_AB_FW_SLOT_MAGIC_WORD;
    sli_si91x_update_crc(&fw_slot_info);

    // Erase and write backup slot info
    status = sli_si91x_erase_slot_info(SLI_SI91X_BACKUP_SLOT_INFO_FLASH_OFFSET);
    if (status != SL_STATUS_OK)
      return status;

    status = sl_si91x_flash_write(SLI_SI91X_BACKUP_SLOT_INFO_FLASH_OFFSET,
                                  (uint8_t *)&fw_slot_info,
                                  sizeof(sl_si91x_fw_ab_slot_management_t));
    if (status != SL_STATUS_OK)
      return status;

    // Verify backup slot info
    status = sl_si91x_flash_read(SLI_SI91X_BACKUP_SLOT_INFO_FLASH_OFFSET,
                                 (uint8_t *)&verify_fw_slot_info,
                                 sizeof(sl_si91x_fw_ab_slot_management_t));
    if (status != SL_STATUS_OK)
      return SL_SI91X_AB_ERR_FLASH_READ;

    if ((sli_si91x_calculate_firmware_slot_crc(&verify_fw_slot_info) != verify_fw_slot_info.slot_struct_crc)
        || (verify_fw_slot_info.slot_magic_word != SLI_SI91X_AB_FW_SLOT_MAGIC_WORD)) {
      return SL_SI91X_AB_ERR_BACKUP_VERIFY;
    }

    // Erase and write actual slot info
    status = sli_si91x_erase_slot_info(SLI_SI91X_SLOT_INFO_FLASH_OFFSET);
    if (status != SL_STATUS_OK)
      return status;

    status = sl_si91x_flash_write(SLI_SI91X_SLOT_INFO_FLASH_OFFSET,
                                  (uint8_t *)&fw_slot_info,
                                  sizeof(sl_si91x_fw_ab_slot_management_t));
    if (status != SL_STATUS_OK)
      return status;

    // Verify written slot info
    status = sl_si91x_flash_read(SLI_SI91X_SLOT_INFO_FLASH_OFFSET,
                                 (uint8_t *)&verify_fw_slot_info,
                                 sizeof(sl_si91x_fw_ab_slot_management_t));
    if (status != SL_STATUS_OK)
      return SL_SI91X_AB_ERR_FLASH_READ;

    if ((sli_si91x_calculate_firmware_slot_crc(&verify_fw_slot_info) != verify_fw_slot_info.slot_struct_crc)
        || (verify_fw_slot_info.slot_magic_word != SLI_SI91X_AB_FW_SLOT_MAGIC_WORD)) {
      return SL_SI91X_AB_ERR_FLASH_VERIFY;
    }

    if ((fw_slot_info.m4_slot_info.current_active_M4_slot != verify_fw_slot_info.m4_slot_info.current_active_M4_slot)
        || (fw_slot_info.nwp_slot_info.current_active_nwp_slot
            != verify_fw_slot_info.nwp_slot_info.current_active_nwp_slot)) {
      return SL_SI91X_AB_ERR_FLASH_VERIFY;
    }
  }
  DEBUGOUT("\r\nSlot update successful\n");
  return status;
}

/***************************************************************************/ /**
 *  @fn          sl_status_t sl_si91x_ab_get_slot_info(sl_si91x_fw_ab_slot_management_t *slot_info_t)
 *  @pre         None
 *  @brief       Retrieves the current slot information.
 *               This function reads the current slot information from flash memory.
 *               This function will work for A/B firmware only.
 *  @param[out]  slot_info_t        Pointer to the sl_si91x_fw_ab_slot_management_t structure to store the slot information.
 *  @return      sl_status_t        SL_STATUS_OK if successful, error code otherwise.
 ******************************************************************************/
sl_status_t sl_si91x_ab_get_slot_info(sl_si91x_fw_ab_slot_management_t *slot_info_t)
{
  if (slot_info_t == NULL) {
    DEBUGOUT("\r\nError: NULL pointer passed for slot_info!\n");
    return SL_STATUS_NULL_POINTER;
  }

  memset(slot_info_t, 0, sizeof(sl_si91x_fw_ab_slot_management_t));

  // Read slot information from primary flash location
  sl_status_t status = sl_si91x_flash_read(SLI_SI91X_SLOT_INFO_FLASH_OFFSET,
                                           (uint8_t *)slot_info_t,
                                           sizeof(sl_si91x_fw_ab_slot_management_t));

  if (status != SL_STATUS_OK) {
    return SL_SI91X_AB_ERR_FLASH_READ;
  }

  // Check if the magic number is 0xFF (invalid slot information)
  if (slot_info_t->slot_magic_word == 0xFFFFFFFF) {
    DEBUGOUT("\r\nError: Slot info magic number is 0xFFFFFFFF, indicating invalid slot info.\n");
    return SL_SI91X_AB_ERR_INVALID_SLOT_INFO;
  }

  // Verify CRC and Magic Word of the read-back structure
  if ((sli_si91x_calculate_firmware_slot_crc(slot_info_t) != slot_info_t->slot_struct_crc)
      || (slot_info_t->slot_magic_word != SLI_SI91X_AB_FW_SLOT_MAGIC_WORD)) {
    DEBUGOUT("\r\nError: CRC verification failed for primary slot info. Reading from backup...\n");

    // Clear the slot_info_t structure before reading from backup
    memset(slot_info_t, 0, sizeof(sl_si91x_fw_ab_slot_management_t));

    // Read slot information from backup flash location
    status = sl_si91x_flash_read(SLI_SI91X_BACKUP_SLOT_INFO_FLASH_OFFSET,
                                 (uint8_t *)slot_info_t,
                                 sizeof(sl_si91x_fw_ab_slot_management_t));

    if (status != SL_STATUS_OK) {
      return SL_SI91X_AB_ERR_FLASH_READ;
    }

    // Check again if magic number is 0xFFFFFFFF in backup slot
    if (slot_info_t->slot_magic_word == 0xFFFFFFFF) {
      DEBUGOUT("\r\nError: Backup slot info magic number is 0xFFFFFFFF, indicating invalid slot info.\n");
      return SL_SI91X_AB_ERR_INVALID_SLOT_INFO;
    }

    // Verify CRC and Magic Word of the read-back structure from backup
    if ((sli_si91x_calculate_firmware_slot_crc(slot_info_t) != slot_info_t->slot_struct_crc)
        || (slot_info_t->slot_magic_word != SLI_SI91X_AB_FW_SLOT_MAGIC_WORD)) {
      DEBUGOUT("\r\nError: CRC verification failed for backup slot info\n");
      return SL_SI91X_AB_ERR_FLASH_VERIFY;
    }
  }

  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 *  @fn          sl_status_t sl_si91x_toggle_slot_info(bool toggle_m4_image, bool toggle_nwp_image)
 *  @brief       Toggles the active slot for M4 and NWP firmware.
 *
 *  This function switches the active firmware slot for M4 and/or NWP between
 *  Slot A and Slot B, based on the current active slot.
 *
 *  @param[in]   toggle_m4_image  If true, toggles M4 firmware slot.
 *  @param[in]   toggle_nwp_image If true, toggles NWP firmware slot.
 *
 *  @return      sl_status_t Returns SL_STATUS_OK on success, error code otherwise.
 ******************************************************************************/
sl_status_t sl_si91x_toggle_slot_info(bool toggle_m4_image, bool toggle_nwp_image)
{
  sl_status_t status;
  sl_si91x_fw_ab_slot_management_t slot_info;

  // Get the current slot information
  status = sl_si91x_ab_get_slot_info(&slot_info);
  if (status != SL_STATUS_OK) {
    DEBUGOUT("\r\nFailed to get slot information: %lX\r\n", status);
    return status;
  }

  // Toggle M4 slot if requested
  if (toggle_m4_image) {
    uint32_t new_image_offset = (slot_info.m4_slot_info.current_active_M4_slot == SLOT_A)
                                  ? slot_info.m4_slot_info.m4_slot_B.slot_image_offset
                                  : slot_info.m4_slot_info.m4_slot_A.slot_image_offset;
    uint32_t new_image_size   = (slot_info.m4_slot_info.current_active_M4_slot == SLOT_A)
                                  ? slot_info.m4_slot_info.m4_slot_B.image_size
                                  : slot_info.m4_slot_info.m4_slot_A.image_size;

    DEBUGOUT("\r\nSwitching M4 slot to %s\r\n", (slot_info.m4_slot_info.current_active_M4_slot == SLOT_A) ? "B" : "A");

    status = sl_si91x_ab_upgrade_set_slot_info(new_image_offset, new_image_size, SL_SI91X_AB_OTA_IMAGE_TYPE_M4);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("\r\nFailed to set M4 slot information: %lX\r\n", status);
      return status;
    }
  }

  // Toggle NWP slot if requested
  if (toggle_nwp_image) {
    uint32_t new_image_offset = (slot_info.nwp_slot_info.current_active_nwp_slot == SLOT_A)
                                  ? slot_info.nwp_slot_info.nwp_slot_B.slot_image_offset
                                  : slot_info.nwp_slot_info.nwp_slot_A.slot_image_offset;
    uint32_t new_image_size   = (slot_info.nwp_slot_info.current_active_nwp_slot == SLOT_A)
                                  ? slot_info.nwp_slot_info.nwp_slot_B.image_size
                                  : slot_info.nwp_slot_info.nwp_slot_A.image_size;

    DEBUGOUT("\r\nSwitching NWP slot to %s\r\n",
             (slot_info.nwp_slot_info.current_active_nwp_slot == SLOT_A) ? "B" : "A");

    status = sl_si91x_ab_upgrade_set_slot_info(new_image_offset, new_image_size, SL_SI91X_AB_OTA_IMAGE_TYPE_NWP);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("\r\nFailed to set NWP slot information: %lX\r\n", status);
      return status;
    }
  }

  return SL_STATUS_OK;
}
/***************************************************************************/ /**
 *  @fn          sli_status_t sli_si91x_erase_slot_info(uint32_t flash_offset)
 *  @pre         None
 *  @brief       Erases the fallback slot information.
 *               This function erases the fallback slot information from flash memory.
 *               This function will work for A/B firmware only.
 *  @param[in]   flash_offset       Flash memory offset to erase.
 *  @return      sl_status_t        SL_STATUS_OK if successful, error code otherwise.
 ******************************************************************************/
static sl_status_t sli_si91x_erase_slot_info(uint32_t flash_offset)
{
  sl_status_t status  = SL_STATUS_FAIL;
  uint32_t erase_size = sizeof(sl_si91x_fw_ab_slot_management_t);
  status              = sl_si91x_flash_erase(flash_offset, erase_size);
  return status;
}
/***************************************************************************/ /**
 *  @fn          void sli_si91x_update_crc(sl_si91x_fw_ab_slot_management_t *slot_info)
 *  @pre         None
 *  @brief       Update CRC.
 *               This function updates the CRC of the given sl_si91x_fw_ab_slot_management_t structure.
 *               This function will work for A/B firmware only.
 *  @param[in]   slot_info          Pointer to the sl_si91x_fw_ab_slot_management_t structure.
 *  @return      None
 ******************************************************************************/
static void sli_si91x_update_crc(sl_si91x_fw_ab_slot_management_t *slot_info)
{
  slot_info->slot_struct_crc = sli_si91x_calculate_firmware_slot_crc(slot_info);
}
#endif
#ifdef SL_SI91X_FW_FALLBACK_UPDATER
/***************************************************************************/ /**
 *  @fn          uint32_t sli_si91x_calculate_crc32(const void *data, size_t length)
 *  @pre         None
 *  @brief       Calculate CRC32.
 *               This function calculates the CRC32 for the given data.
 *  @param[in]   data               Pointer to the data.
 *  @param[in]   length             Length of the data.
 *  @return      uint32_t           Calculated CRC32 value.
 ******************************************************************************/
static uint32_t sli_si91x_calculate_crc32(const void *data, size_t length)
{
  uint32_t crc         = 0xFFFFFFFF;
  const uint8_t *bytes = (const uint8_t *)data;

  for (size_t i = 0; i < length; i++) {
    crc ^= (uint32_t)bytes[i] << 24;

    for (int j = 0; j < 8; j++) {
      if (crc & 0x80000000)
        crc = (crc << 1) ^ CRC32_POLYNOMIAL;
      else
        crc <<= 1;
    }
  }
  return ~crc; // Final XOR for CRC32
}
/***************************************************************************/ /**
 *  @fn          sl_status_t sli_si91x_calculate_firmware_slot_crc(sl_si91x_fw_ab_slot_management_t *slot_info)
 *  @pre         None
 *  @brief       Calculate CRC of sl_si91x_fw_ab_slot_management_t.
 *               This function calculates the CRC of the given sl_si91x_fw_ab_slot_management_t structure.
 *               This function will work for A/B firmware only.
 *  @param[in]   slot_info          Pointer to the sl_si91x_fw_ab_slot_management_t structure.
 *  @return      sl_status_t        SL_STATUS_OK on success, error code otherwise.
 ******************************************************************************/
static sl_status_t sli_si91x_calculate_firmware_slot_crc(sl_si91x_fw_ab_slot_management_t *slot_info)
{
  return sli_si91x_calculate_crc32(slot_info, offsetof(sl_si91x_fw_ab_slot_management_t, slot_struct_crc));
}

/***************************************************************************/ /**
 *  @fn          sl_status_t sl_si91x_flash_read(uint32_t address, uint8_t *buffer, uint32_t length)
 *  @pre         None
 *  @brief       Read data from flash memory.
 *               This function reads data from the specified flash memory address.
 *               This function will work for A/B firmware only.
 *  @param[in]   address            Flash memory address to read from M4.
 *  @param[in]   buffer             Pointer to the buffer to store read data.
 *  @param[in]   length             Number of bytes to read.
 *  @return      sl_status_t        SL_STATUS_OK on success, error code otherwise.
 ******************************************************************************/
sl_status_t sl_si91x_flash_read(uint32_t address, uint8_t *buffer, uint32_t length)
{
  if (buffer == NULL || length == 0) {
    return SL_STATUS_INVALID_PARAMETER; // Prevent invalid buffer usage
  }
  // Directly copy from the fixed address (assuming it's always readable)
  memcpy(buffer, (const void *)address, length);
  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 *  @fn          sl_status_t sl_si91x_get_m4_app_addr(uint32_t *app_addr)
 *  @pre         None
 *  @brief       Retrieves the M4 application address from the active firmware slot.
 *               This function reads firmware slot information from flash memory,
 *               verifies its integrity using a magic word and CRC check, and determines
 *               the application address for the currently active M4 slot.
 *               This function will work for A/B firmware only.
 * 
 *               If a CRC failure is detected in the primary slot information,  
 *               the function attempts to retrieve the slot information from a backup slot.
 *  @param[out]  app_addr           Pointer to store the retrieved application address.
 *  @return      SL_STATUS_OK        If the operation is successful.
 *  @return      SL_SI91X_AB_ERR_FLASH_READ If reading slot information from flash fails.
 *  @return      SL_SI91X_AB_ERR_MAGIC_NUMBER If the magic word does not match the expected value.
 *  @return      SL_SI91X_AB_ERR_CRC_MISMATCH If the CRC verification fails.
 *  @return      SL_STATUS_INVALID_PARAMETER If the active slot is invalid.
 ******************************************************************************/
sl_status_t sl_si91x_get_m4_app_addr(uint32_t *app_addr)
{
  sl_status_t status                              = SL_STATUS_FAIL;
  sl_si91x_fw_ab_slot_management_t fw_slot_info_t = { 0 };

  // Read current slot info from flash
  status = sl_si91x_flash_read(SLI_SI91X_SLOT_INFO_FLASH_OFFSET,
                               (uint8_t *)&fw_slot_info_t,
                               sizeof(sl_si91x_fw_ab_slot_management_t));

  if (status != SL_STATUS_OK) {
    return SL_SI91X_AB_ERR_FLASH_READ; // Flash read error
  }

  // Verify CRC and Magic Word
  if ((fw_slot_info_t.slot_magic_word != SLI_SI91X_AB_FW_SLOT_MAGIC_WORD)
      || (sli_si91x_calculate_firmware_slot_crc(&fw_slot_info_t) != fw_slot_info_t.slot_struct_crc)) {

    DEBUGOUT("\r\nSlot info corrupted, reading from backup...\n");

    memset(&fw_slot_info_t, 0, sizeof(sl_si91x_fw_ab_slot_management_t)); // Clear memory again before backup read
    // Read from backup location
    status = sl_si91x_flash_read(SLI_SI91X_BACKUP_SLOT_INFO_FLASH_OFFSET,
                                 (uint8_t *)&fw_slot_info_t,
                                 sizeof(sl_si91x_fw_ab_slot_management_t));

    if (status != SL_STATUS_OK) {
      return SL_SI91X_AB_ERR_FLASH_READ; // Backup read error
    }

    // Validate backup CRC and Magic Word
    if ((fw_slot_info_t.slot_magic_word != SLI_SI91X_AB_FW_SLOT_MAGIC_WORD)) {
      return SL_SI91X_AB_ERR_MAGIC_NUMBER; // Magic number mismatch
    }
    if (sli_si91x_calculate_firmware_slot_crc(&fw_slot_info_t) != fw_slot_info_t.slot_struct_crc) {
      return SL_SI91X_AB_ERR_CRC_MISMATCH; // CRC verification failed
    }
  }
  // Determine active slot for M4 and fetch the application address
  if (fw_slot_info_t.m4_slot_info.current_active_M4_slot == SLOT_A) {
    *app_addr = fw_slot_info_t.m4_slot_info.m4_slot_A.slot_image_offset;
  } else if (fw_slot_info_t.m4_slot_info.current_active_M4_slot == SLOT_B) {
    *app_addr = fw_slot_info_t.m4_slot_info.m4_slot_B.slot_image_offset;
  } else {
    // Invalid slot; handle error
    return SL_STATUS_INVALID_PARAMETER; // Invalid active slot
  }

  // Convert offset to actual address
  (*app_addr) += SLI_SI91X_CHUNK_LENGTH;
  DEBUGOUT("\r\n app_addr:%lX M4_Slot:%X %lX\r\n",
           *app_addr,
           fw_slot_info_t.m4_slot_info.current_active_M4_slot,
           fw_slot_info_t.m4_slot_info.m4_slot_A.slot_image_offset);

  // This point is unreachable, but we add a return to satisfy the compiler
  return SL_STATUS_OK;
}
/***************************************************************************/ /**
 *  @fn          void sl_si91x_jump_to_m4_application(uint32_t app_addr)
 *  @pre         None
 *  @brief       Jumps to the M4 application.
 *               This function disables interrupts, fetches the stack pointer and reset handler
 *               from the application's vector table, sets the VTOR to point to the new
 *               application vector table, and branches to the reset handler.
 *               This function will work for A/B firmware only.
 *  @param[in]   app_addr           Address of the application to jump to.
 *  @return      None
 ******************************************************************************/
void sl_si91x_jump_to_m4_application(uint32_t app_addr)
{
  volatile uint32_t stack_pointer;
  volatile uint32_t reset_handler;

  // Disable interrupts and prepare to branch
  __asm volatile("cpsid i" ::: "memory"); // Disable interrupts
  __asm volatile("dsb");                  // Data Synchronization Barrier
  __asm volatile("isb");                  // Instruction Synchronization Barrier

  // Fetch stack pointer and reset handler from the application's vector table
  stack_pointer = *(uint32_t *)(app_addr + 0xB8); // Stack pointer
  reset_handler = *(uint32_t *)(app_addr + 0xC0); // Reset handler

  // Set VTOR to point to the new application vector table
  SCB->VTOR = (uint32_t)app_addr;

  //! Call RSI reset handler */
  __asm("ldr r1, %0" ::"m"(reset_handler) : "r1", "memory"); // Loads reset handler in r1 register

  //! load stack pointer
  __asm("ldr sp, %0" ::"m"(stack_pointer) : "memory"); // Loads stack pointer in sp

  __asm(" bx r1 "); // branch to r1
}

#endif
