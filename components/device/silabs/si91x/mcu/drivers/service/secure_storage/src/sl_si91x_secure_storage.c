/******************************************************************************
* @file sl_si91x_secure_storage.c
* @brief Si91x MCU Secure Storage API implementation
*******************************************************************************
* # License
* <b>Copyright 2026 Silicon Laboratories Inc. www.silabs.com</b>
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
* 2. Altered source versions must be plainly marked as such, and must not
*    be misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*
*******************************************************************************
* # Overview
*
* Implements write/read for Si91x MCU secure storage registers (REG0–REG7).
* At the hardware level, only REG0–REG3 are gated by MCU_STORAGE_WRITE_KEY; REG4–REG7
* accept writes without needing unlock. The driver still runs the same unlock/lock sequence
* around every write (see sl_si91x_secure_storage_write_register) so all indices share one code path.
*
* ## Why REG4–REG7 are not key-gated in hardware
* Hardware gates only REG0–REG3 with MCU_STORAGE_WRITE_KEY. REG4–REG7 are in
* the same peripheral block but are not protected by the key; writes to them
* are always accepted. This is a silicon design choice: the first four
* registers are intended for more sensitive data (key material, etc.); REG4–REG7
* are general-purpose storage without key protection. Unlock/lock in software is
* redundant for REG4–REG7 but harmless and keeps the driver consistent with the API docs.
*
******************************************************************************/
#include "sl_si91x_secure_storage.h"
#include "base_types.h"
#include "si91x_device.h"
#include "sl_si91x_secure_storage_config.h"
#include "sl_si91x_driver.h"
#include "sl_si91x_protocol_types.h"

/*******************************************************************************
 ***************************  LOCAL MACROS   ***********************************
 ******************************************************************************/
/** Hardware-defined unlock key. Written to MCU_STORAGE_WRITE_KEY to allow
 *  writes to REG0–REG3. Value is fixed by silicon; see device reference manual.
 *  Do not change. */
#define MCU_STORAGE_UNLOCK_KEY 0x91437B2B

/** Hardware-defined lock key. Written to MCU_STORAGE_WRITE_KEY to write-protect
 *  REG0–REG3 after a batch of writes. Value is fixed by silicon; see device
 *  reference manual. Do not change. */
#define MCU_STORAGE_LOCK_KEY 0xCCCCCCCC

/**
 * @brief Local functions declarations
 * @note Program MCU_STORAGE_WRITE_KEY (hardware gates REG0–REG3 only). write_register
 *       calls them for every index so unlock/lock is uniform across REG0–REG7.
 */
static void sli_si91x_secure_storage_write_enable(void); // Enable write to REG0–REG3
static void sli_si91x_secure_storage_write_disable(void);

/*******************************************************************************
 * sli_si91x_secure_storage_write_enable: (static) Write unlock key to
 * MCU_STORAGE_WRITE_KEY so hardware allows writes to REG0–REG3. REG4–REG7 do not
 * require unlock in silicon; this is still invoked for every write for a single
 * driver path. No return value.
 ******************************************************************************/
static void sli_si91x_secure_storage_write_enable(void)
{
  MCU_SECURE_STORAGE->MCU_STORAGE_WRITE_KEY = MCU_STORAGE_UNLOCK_KEY;
}

/*******************************************************************************
 * sl_si91x_secure_storage_write_register: Write a 32-bit value to one
 * register (MCU_STORAGE_REG0–MCU_STORAGE_REG7). Validates register index (0–7).
 * Always write_enable, store, then write_disable so MCU_STORAGE_WRITE_KEY is
 * unlocked then re-locked around the access. Hardware only requires that for
 * REG0–REG3; REG4–REG7 are not key-gated but use the same sequence (see file overview).
 ******************************************************************************/
sl_status_t sl_si91x_secure_storage_write_register(mcu_secure_storage_register_t register_index,
                                                   uint32_t secure_storage_write_data)
{
  if (register_index >= MCU_STORAGE_REGISTER_LAST) {

    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_si91x_secure_storage_write_enable();
  __NOP();
  __NOP();

  ((volatile uint32_t *)&MCU_SECURE_STORAGE->MCU_STORAGE_REG0)[register_index] = secure_storage_write_data;
  __NOP();
  __NOP();

  sli_si91x_secure_storage_write_disable();
  __NOP();
  __NOP();

  return SL_STATUS_OK;
}

/*******************************************************************************
 * sl_si91x_secure_storage_read_register: Read a 32-bit value from one
 * register (REG0–REG7). Validates register_index and non-NULL data. Reads the
 * corresponding MCU_STORAGE_REGx into *data. Returns SL_STATUS_INVALID_PARAMETER,
 * SL_STATUS_NULL_POINTER, or SL_STATUS_OK.
 ******************************************************************************/
sl_status_t sl_si91x_secure_storage_read_register(mcu_secure_storage_register_t register_index,
                                                  uint32_t *secure_storage_read_data)
{
  sl_status_t status = SL_STATUS_OK;

  /* Check for valid parameters - allow reading any valid register (0-7) */
  if (register_index >= MCU_STORAGE_REGISTER_LAST) {
    SL_PRINT_STRING_ERROR("sl_si91x_secure_storage_read_register():[secure_storage_read_register] Error: Invalid "
                          "register index. line no: %d",
                          __LINE__);
    status = SL_STATUS_INVALID_PARAMETER; // Invalid register index
  } else if (secure_storage_read_data == NULL) {
    SL_PRINT_STRING_ERROR("sl_si91x_secure_storage_read_register():[secure_storage_read_register] Error: Null pointer "
                          "to secure storage read data. line no: %d",
                          __LINE__);
    status = SL_STATUS_NULL_POINTER; // Null pointer to secure storage read data
  } else {
    /* REG0–REG7 are consecutive 32-bit words; access as REG0 + offset. */
    *secure_storage_read_data = ((volatile uint32_t *)&MCU_SECURE_STORAGE->MCU_STORAGE_REG0)[register_index];
  }

  return status;
}

/*******************************************************************************
 * sli_si91x_secure_storage_write_disable: (static) Write lock key to
 * MCU_STORAGE_WRITE_KEY so REG0–REG3 are write-protected again. Called after
 * every write_register, including REG4–REG7 (hardware effect is on REG0–REG3 only).
 * No return value.
 ******************************************************************************/
static void sli_si91x_secure_storage_write_disable(void)
{
  /* Write lock key (MCU_SECURE_STORAGE from si91x_device.h) */
  MCU_SECURE_STORAGE->MCU_STORAGE_WRITE_KEY = MCU_STORAGE_LOCK_KEY;
}

/*******************************************************************************
 * sl_si91x_secure_storage_enable_protection_and_lock: When ENABLE_SECURE_PROTECTION
 * (UC configuration) is 1: enable MCU secure storage write protection through NWP
 * handshake. When ENABLE_SECURE_PROTECTION is 0:
 * return SL_STATUS_NOT_AVAILABLE (UC configuration not enabled). Returns
 * SL_STATUS_OK on success, SL_STATUS_NOT_AVAILABLE if config disabled, or
 * handshake status on failure.
 ******************************************************************************/
sl_status_t sl_si91x_secure_storage_enable_protection_and_lock(void)
{

#if (ENABLE_SECURE_PROTECTION == 1)
  sl_status_t status                       = SL_STATUS_OK;
  uint8_t enable_secure_storage_protection = 1;
  const uint8_t input_data_size            = 1;

  status = sl_si91x_m4_ta_secure_handshake(SL_SI91X_MCU_SECURE_STORAGE_ENABLE_PROTECTION,
                                           input_data_size,
                                           &enable_secure_storage_protection,
                                           0,
                                           NULL);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_secure_storage_enable_protection_and_lock():[secure_storage_enable_protection_and_"
                          "lock] Error: Failed to enable secure storage protection. status: %d, line no: %d",
                          status,
                          __LINE__);
    return status;
  }

  return status;
#else
  return SL_STATUS_NOT_AVAILABLE; /* UC configuration (ENABLE_SECURE_PROTECTION) not enabled */
#endif
}
