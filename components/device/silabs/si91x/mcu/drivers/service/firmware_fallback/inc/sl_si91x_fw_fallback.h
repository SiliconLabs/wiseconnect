/*******************************************************************************
 * @file  sli_si91x_fw_fallback.h
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

#ifndef _SLI_SI91X_FW_FALLBACK__
#define _SLI_SI91X_FW_FALLBACK__
#include "sl_status.h"
#include <stdbool.h>
#include <stdint.h>

#if defined(SL_SI91X_FW_FALLBACK) || defined(SL_SI91X_FW_FALLBACK_UPDATER)
/******************************** Macros ********************************/

/** @brief Magic number located in RPS Header. */
#define SLI_SI91X_RPS_MAGIC_NO 0x900D900D

/** @brief Firmware header size in bytes. */
#define SLI_SI91X_RPS_HEADER_SIZE sizeof(sl_si91x_firmware_header_t)

/** @brief Flash memory chunk size for erase operations. */
#define SLI_SI91X_CHUNK_LENGTH 4096 ///< Flash erase chunk length (4KB)

/** @brief Base address of M4 and NWP flash memory. */
#define SLI_SI91X_M4_FLASH_BASE_ADDR  0x8000000 ///< M4 flash base address
#define SLI_SI91X_NWP_FLASH_BASE_ADDR 0x4000000 ///< NWP flash base address

/** @brief Slot information flash offsets. */
#define SLI_SI91X_SLOT_INFO_FLASH_OFFSET 0x81F1000 ///< Primary slot information offset
#define SLI_SI91X_BACKUP_SLOT_INFO_FLASH_OFFSET \
  (SLI_SI91X_SLOT_INFO_FLASH_OFFSET + SLI_SI91X_CHUNK_LENGTH) ///< Backup slot offset

/** @brief Image type identifiers in RPS. */
#define SL_SI91X_M4_RPS_BIT  1 ///< M4 RPS image type
#define SL_SI91X_NWP_RPS_BIT 0 ///< NWP RPS image type

/** @brief OTA image chunk size. */
#define SL_SI91X_MAX_OTA_IMAGE_CHUNK_SIZE 1024 ///< OTA firmware chunk size

/** @brief Timeout for NWP responses. */
#define SL_SI91X_NWP_RESPONSE_TIMEOUT 30000

/** @brief Magic word to verify slot information integrity. */
#define SLI_SI91X_AB_FW_SLOT_MAGIC_WORD 0xA5A5B5B5

#define CRC32_POLYNOMIAL 0x04C11DB7 ///< This macro defines the polynomial used for CRC32 calculation.

// @brief Error codes for SI91x A/B slot and fallback operations.
#define SL_SI91X_AB_ERR_FLASH_READ        ((sl_status_t)0xAB001) ///< A/B fallback: Flash read operation failed
#define SL_SI91X_AB_ERR_FLASH_VERIFY      ((sl_status_t)0xAB002) ///< A/B fallback: Flash verification failure
#define SL_SI91X_AB_ERR_BACKUP_READ       ((sl_status_t)0xAB003) ///< A/B fallback: Backup slot read failure
#define SL_SI91X_AB_ERR_BACKUP_VERIFY     ((sl_status_t)0xAB004) ///< A/B fallback: CRC verification failed in backup slot
#define SL_SI91X_AB_ERR_CRC_MISMATCH      ((sl_status_t)0xAB005) ///< A/B fallback: CRC mismatch detected
#define SL_SI91X_AB_ERR_MAGIC_NUMBER      ((sl_status_t)0xAB006) ///< A/B fallback: Magic number mismatch error
#define SL_SI91X_AB_ERR_UNKNOWN_IMG       ((sl_status_t)0xAB007) ///< A/B fallback: Unknown image type
#define SL_SI91X_AB_ERR_INVALID_SLOT_INFO ((sl_status_t)0xAB008) ///< A/B fallback: Invalid slot info
#define SL_SI91X_AB_ERR_ERASE_ACTIVE_SLOT ((sl_status_t)0xAB009) ///< A/B fallback: Invalid slot info

// This enumeration is used to specify the type of OTA image being processed
// during an A/B firmware upgrade and for slot management.
typedef enum {
  SL_SI91X_AB_OTA_IMAGE_TYPE_M4  = 1, ///< Image for M4 processor
  SL_SI91X_AB_OTA_IMAGE_TYPE_NWP = 2, ///< Image for NWP processor
  SL_SI91X_AB_OTA_IMAGE_TYPE_MAX      ///< Maximum image type value
} sl_si91x_ab_image_type_t;

/** @brief Enum for AB slot identifiers */
typedef enum {
  SLOT_A = 1, ///< Primary slot (typically the active firmware slot)
  SLOT_B = 2, ///< Secondary slot (used for updates or fallback)
  SLOT_MAX    ///< Maximum number of slots (used for validation)
} sl_si91x_ab_slot_t;

typedef enum {
  SL_SI91X_DATA_PACKET   = 0, ///< Chunk type for Data packet
  SL_SI91X_HEADER_PACKET = 1  ///< Chunk type for Header packet
} sl_si91x_chunk_type_t;

/** @brief SI91x-specific fallback command types. */
typedef enum {
  SL_SI91X_AB_FALLBACK_WRITE_CMD           = 1, ///< Write firmware data for fallback
  SL_SI91X_AB_FALLBACK_READ_CMD            = 2, ///< Read firmware data for verification
  SL_SI91X_AB_FALLBACK_ERASE_CMD           = 3, ///< Erase fallback firmware slot
  SL_SI91X_AB_FALLBACK_INTEGRITY_CHECK_CMD = 4, ///< Perform integrity check before fallback
  SL_SI91X_AB_FALLBACK_MAX_CMD             = 5  ///< Maximum fallback command value (for validation)
} sl_si91x_ab_fallback_cmd_t;

/************************** Structures ***************************/

/** @brief Structure for OTA image information. */
typedef struct {
  uint8_t ota_image_type;    ///< OTA image type
  uint32_t ota_image_offset; ///< OTA image offset
  uint32_t ota_image_size;   ///< OTA image size
} ota_image_info_t;

/** @brief Firmware update request structure. */
typedef struct {
  uint16_t chunk_type;   ///< Header (1) or Data (0)
  uint16_t sub_command;  ///< Operation type: WRITE (1), READ (2), ERASE (3), INTEGRITY_CHECK (4)
  uint32_t data_length;  ///< Length of data (for erase, must be a multiple of 4K)
  uint32_t flash_offset; ///< Flash memory address: M4 -> 0x8000000, NWP -> 0x4000000
  uint32_t reserved1;    ///< Reserved for future use
  uint32_t reserved2;    ///< Reserved for future use
  uint8_t data
    [SL_SI91X_MAX_OTA_IMAGE_CHUNK_SIZE]; ///< Data buffer (used for WRITE operation, NULL for READ, ERASE, and INTEGRITY_CHECK)
} sl_si91x_fw_fallback_request_t;

/** @brief Structure for slot information. */
typedef struct {
  uint8_t slot_id;            ///< Slot Identifier (e.g., SLOT_A, SLOT_B)
  uint32_t slot_image_offset; ///< Base address of the slot in flash memory
  uint32_t image_size;        ///< Size of the firmware image in bytes
} SlotInfo_t;

/** @brief M4 slot management structure. */
typedef struct {
  SlotInfo_t m4_slot_A;           ///< Information for Slot A
  SlotInfo_t m4_slot_B;           ///< Information for Slot B
  uint8_t current_active_M4_slot; ///< Currently active slot (A or B)
} M4_SlotManagement_t;

/** @brief NWP slot management structure. */
typedef struct {
  SlotInfo_t nwp_slot_A;           ///< Information for NWP Slot A
  SlotInfo_t nwp_slot_B;           ///< Information for NWP Slot B
  uint8_t current_active_nwp_slot; ///< Currently active NWP slot (A or B)
} NWP_SlotManagement_t;

/** @brief Firmware slot management structure. */
typedef struct {
  uint32_t slot_magic_word;           ///< Magic word for slot integrity verification
  M4_SlotManagement_t m4_slot_info;   ///< Slot information for M4
  NWP_SlotManagement_t nwp_slot_info; ///< Slot information for NWP
  uint32_t slot_struct_crc;           ///< CRC for the slot information structure
} sl_si91x_fw_ab_slot_management_t;

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
sl_status_t sl_si91x_ab_upgrade_get_rps_configs(const uint8_t *image_buffer, ota_image_info_t *ota_st);

/***************************************************************************/ /**
  *  @fn          sl_status_t sl_si91x_flash_write(uint32_t address, uint8_t *buffer, uint32_t length)
  *  @pre         None
  *  @brief       Write data to flash memory.
  *               This function writes data to the specified flash memory address.
  *               This function will work for A/B firmware only.
  *  @param[in]   address            Flash memory address to write to.
  *  @param[in]   buffer             Pointer to the data buffer.
  *  @param[in]   length             Number of bytes to write.
  *  @return      sl_status_t        SL_STATUS_OK on success, error code otherwise.
  ******************************************************************************/
sl_status_t sl_si91x_flash_write(uint32_t address, const uint8_t *buffer, uint32_t length);

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
sl_status_t sl_si91x_flash_read(uint32_t address, uint8_t *buffer, uint32_t length);

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
                                              sl_si91x_ab_image_type_t image_type);

/***************************************************************************/ /**
 *  @fn          sl_status_t sl_si91x_flash_erase(uint32_t address, uint32_t length)
 *  @pre         None
 *  @brief       Erases data from flash memory sector-wise.
 *
 *  This function erases data from the specified flash memory address in
 *  sector-sized blocks. The erase operation will only work for A/B firmware
 *  slots and ensures that no partial sector erases occur.
 *
 *  @note        The erase operation is performed in full sectors. If the
 *               provided address or length does not align with sector boundaries,
 *               the function will round to the nearest sector boundary.
 *
 *  @param[in]   address  Starting flash memory address to erase (must be sector-aligned).
 *  @param[in]   length   Number of bytes to erase (will be rounded up to sector size).
  *  @return      sl_status_t        SL_STATUS_OK on success, error code otherwise.
  ******************************************************************************/
sl_status_t sl_si91x_flash_erase(uint32_t address, uint32_t length);

/***************************************************************************/ /**
  *  @fn          sl_status_t sl_si91x_ab_get_slot_info(sl_si91x_fw_ab_slot_management_t *slot_info_t)
  *  @pre         None
  *  @brief       Retrieves the current slot information.
  *               This function reads the current slot information from flash memory.
  *               This function will work for A/B firmware only.
  *  @param[out]  slot_info_t        Pointer to the sl_si91x_fw_ab_slot_management_t structure to store the slot information.
  *  @return      sl_status_t        SL_STATUS_OK if successful, error code otherwise.
  ******************************************************************************/
sl_status_t sl_si91x_ab_get_slot_info(sl_si91x_fw_ab_slot_management_t *slot_info_t);

/***************************************************************************/ /**
 *  @fn          sl_status_t sl_si91x_toggle_slot_info(bool toggle_m4_image, bool toggle_nwp_image)
 *  @pre         None
 *  @brief       Toggles the active slot information for M4 or NWP.
 *               This function retrieves the current slot information, checks which slot is active,
 *               and switches to the other slot by calling the sl_si91x_set_slot_info API.
 *  @param[in]   toggle_m4_image    Set to 1 to toggle M4 slot, 0 otherwise.
 *  @param[in]   toggle_nwp_image   Set to 1 to toggle NWP slot, 0 otherwise.
 *  @return      sl_status_t        SL_STATUS_OK if successful, error code otherwise.
 ******************************************************************************/
sl_status_t sl_si91x_toggle_slot_info(bool toggle_m4_image, bool toggle_nwp_image);

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
sl_status_t sl_si91x_get_m4_app_addr(uint32_t *app_addr);

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
void sl_si91x_jump_to_m4_application(uint32_t app_addr);

/***************************************************************************/ /**
  *  @fn          sl_status_t sl_si91x_verify_image(uint32_t address)
  *  @pre         None
  *  @brief       Checks the integrity of the firmware for Fallback.
  *               This function sends a command to check the integrity of the firmware
  *               at the specified flash memory address. It populates the request structure
  *               with the necessary parameters and sends the command to the firmware.
  *               This function will work for A/B firmware only.
  *  @param[in]   address            Flash memory address to check integrity.
  *  @return      sl_status_t        SL_STATUS_OK on success, error code otherwise.
  ******************************************************************************/
sl_status_t sl_si91x_verify_image(uint32_t flash_address);

#endif
#endif /* _SLI_SI91X_FW_FALLBACK__ */
