/******************************************************************************
 * @file sl_si91x_secure_storage.h
 * @brief Si91x MCU Secure Storage API
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
 ******************************************************************************/

#ifndef SL_SI91X_SECURE_STORAGE_H
#define SL_SI91X_SECURE_STORAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_status.h"
#include <stdint.h>
#include "sl_log_helper.h"

/***************************************************************************/
/**
 * @addtogroup MCU-SECURE-STORAGE MCU Secure Storage
 * @ingroup SI91X_SERVICE_APIS
 * @{
 *
 ******************************************************************************/

/** @brief Shared hardware note for consumers of this service.
 *  Secure storage and the MCU temperature sensor (MCU_TEMP) share the same
 *  hardware block; do not use both concurrently without coordination. */

/** @brief Enumeration of the eight MCU secure storage data registers (REG0–REG7). */
typedef enum {
  MCU_STORAGE_REG0,         ///< MCU Storage register 0
  MCU_STORAGE_REG1,         ///< MCU Storage register 1
  MCU_STORAGE_REG2,         ///< MCU Storage register 2
  MCU_STORAGE_REG3,         ///< MCU Storage register 3
  MCU_STORAGE_REG4,         ///< MCU Storage register 4
  MCU_STORAGE_REG5,         ///< MCU Storage register 5
  MCU_STORAGE_REG6,         ///< MCU Storage register 6
  MCU_STORAGE_REG7,         ///< MCU Storage register 7
  MCU_STORAGE_REGISTER_LAST ///< Sentinel: invalid index (not a storage register)
} mcu_secure_storage_register_t;

/*******************************************************************************
    **************************   FUNCTION DECLARATIONS   **************************
 ******************************************************************************/

/***************************************************************************/ /**
 *  @fn          sl_status_t sl_si91x_secure_storage_write_register(mcu_secure_storage_register_t register_index, uint32_t value)
 *  @pre         None.
 *  @brief       Write a 32-bit value to one secure storage register (MCU_STORAGE_REG0–MCU_STORAGE_REG7).
 *               The driver unlocks then re-locks @c MCU_STORAGE_WRITE_KEY around the store so that
 *               writes to REG0–REG3 (hardware key-gated) succeed. REG4–REG7 are not key-gated in
 *               hardware; the driver still runs the same unlock/lock sequence for every index (harmless
 *               for REG4–REG7), giving one code path and matching this implementation.
 *  @param[in]   register_index   Target register; must be less than @ref MCU_STORAGE_REGISTER_LAST.
 *  @param[in]   value            32-bit value to write.
 *  @return      sl_status_t      @c SL_STATUS_OK on success, @c SL_STATUS_INVALID_PARAMETER if @a register_index is out of range.
 * For more information on status codes, refer to [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 ******************************************************************************/
sl_status_t sl_si91x_secure_storage_write_register(mcu_secure_storage_register_t register_index, uint32_t value);

/***************************************************************************/ /**
 *  @fn          sl_status_t sl_si91x_secure_storage_read_register(mcu_secure_storage_register_t register_index, uint32_t *value)
 *  @pre         None.
 *  @brief       Read a 32-bit value from one secure storage register (MCU_STORAGE_REG0–MCU_STORAGE_REG7).
 *  @param[in]   register_index   Source register; must be less than @ref MCU_STORAGE_REGISTER_LAST.
 *  @param[out]  value            Pointer to location that receives the read value; must not be NULL.
 *  @return      sl_status_t      @c SL_STATUS_OK on success, @c SL_STATUS_NULL_POINTER if @a value is NULL,
 *                                or @c SL_STATUS_INVALID_PARAMETER if @a register_index is out of range.
 * For more information on status codes, refer to [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 ******************************************************************************/
sl_status_t sl_si91x_secure_storage_read_register(mcu_secure_storage_register_t register_index, uint32_t *value);

/***************************************************************************/ /**
 *  @fn          sl_status_t sl_si91x_secure_storage_enable_protection_and_lock(void)
 *  @pre         Secure protection must be enabled in configuration: @c ENABLE_SECURE_PROTECTION set to 1
 *               in @c sl_si91x_secure_storage_config.h. Call after required register values have been written.
 *  @brief       Request MCU secure storage write protection through the M4–NWP secure handshake
 *               (@c SL_SI91X_MCU_SECURE_STORAGE_ENABLE_PROTECTION). Use when NWP-backed protection is required
 *               (for example before sleep or a secure handoff). When protection is disabled in configuration
 *               (@c ENABLE_SECURE_PROTECTION is 0), this API returns @c SL_STATUS_NOT_AVAILABLE and performs no handshake.
 *  @return      sl_status_t      @c SL_STATUS_OK if the handshake succeeds;
 *                                @c SL_STATUS_NOT_AVAILABLE if protection is not enabled in configuration;
 *                                otherwise the error status returned from the handshake.
 * For more information on status codes, refer to [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 ******************************************************************************/
sl_status_t sl_si91x_secure_storage_enable_protection_and_lock(void);

/// @} (end addtogroup MCU-SECURE-STORAGE)

// ******** THE REST OF THE FILE IS DOCUMENTATION ONLY! ***********************
/***************************************************************************/
/***************************************************************************/
/**
 * @addtogroup MCU-SECURE-STORAGE MCU Secure Storage
 * @{
 *
 * @details
 *
 * @section Mcu_Secure_Storage_Intro Introduction
 *
 * MCU Secure Storage exposes eight 32-bit retention registers (REG0–REG7) for sensitive or
 * persistent data on Si91x. REG0–REG3 are protected by a hardware write key; REG4–REG7 are
 * general-purpose storage in the same block without key gating. Values typically survive reset;
 *
 * @section Mcu_Secure_Storage_Features Features
 *
 * - **Per-register access:** @ref sl_si91x_secure_storage_write_register and @ref sl_si91x_secure_storage_read_register operate on one register at a time.
 * - **Key handling in software:** Each @ref sl_si91x_secure_storage_write_register call unlocks then re-locks @c MCU_STORAGE_WRITE_KEY;
 *   hardware only requires that for REG0–REG3; REG4–REG7 use the same sequence for one implementation path.
 * - **NWP protection:** @ref sl_si91x_secure_storage_enable_protection_and_lock requests write protection through the NWP when enabled in configuration.
 * - **Hardware sharing:** The same peripheral block is shared with MCU temperature (MCU_TEMP); coordinate use at the application level.
 *
 * @section Mcu_Secure_Storage_Usage Usage
 *
 * 1. **Write required data:** For each register that must change, call @ref sl_si91x_secure_storage_write_register with
 *    @ref mcu_secure_storage_register_t and the 32-bit value. Repeat until all intended REG0–REG7 updates are complete.
 *
 * 2. ** NWP protection:** If @c ENABLE_SECURE_PROTECTION is 1, call @ref sl_si91x_secure_storage_enable_protection_and_lock once
 *    after writes are finished. If it returns @c SL_STATUS_NOT_AVAILABLE, protection was not compiled in; handle per product policy.
 *
 * 3. **Read back:** Use @ref sl_si91x_secure_storage_read_register to read any register (for verification, boot checks, or runtime use).
 *
 * @section Mcu_Secure_Storage_Notes Notes
 *
 * - Register contents may already match desired values on a second boot; applications may read first and skip redundant writes.
 * - For more context and an example flow, see the WiSeConnect MCU Secure Storage example application.
 *
 * @} (end addtogroup MCU-SECURE-STORAGE)
 */
/***************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SL_SI91X_SECURE_STORAGE_H */
