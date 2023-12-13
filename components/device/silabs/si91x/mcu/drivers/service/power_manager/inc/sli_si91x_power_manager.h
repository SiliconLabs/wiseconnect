/***************************************************************************/ /**
 * @file sli_si91x_power_manager.h
 * @brief Power Manager Internal API implementation
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SLI_SI91X_POWER_MANAGER_H
#define SLI_SI91X_POWER_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_si91x_power_manager.h"
#include "system_si91x.h"
#include "rsi_power_save.h"

/***************************************************************************/ /**
 * @addtogroup POWER-MANAGER
 * @ingroup SI91X_SERVICE_APIS
 * @{
 ******************************************************************************/
// -----------------------------------------------------------------------------
// Data Types

/// @brief Structure to store configuration parameters for sleep
typedef struct {
  uint32_t stack_address;           // Stack address
  uint32_t vector_offset;           // Vector offset
  uint32_t wakeup_callback_address; // Wakeup callback address
  uint32_t mode;                    // Mode for wakeup \ref sli_power_sleep_mode_t
  uint8_t low_freq_clock;           // Low freq clock \ref sli_power_low_freq_clock_t
} sli_power_sleep_config_t;

/// @brief Enumeration for the sleep modes
typedef enum {
#if (SLI_SI917B0)
  SLI_SI91X_POWER_MANAGER_WAKEUP_FROM_FLASH_MODE = 0x11, // Wakeup from flash mode
#else
  SLI_SI91X_POWER_MANAGER_WAKEUP_FROM_FLASH_MODE = 1, // Wakeup from flash mode
#endif
  SLI_SI91X_POWER_MANAGER_WAKEUP_WITHOUT_RETENTION = 2,               // Wakeup without retention
  SLI_SI91X_POWER_MANAGER_WAKEUP_WITH_RETENTION,                      // Wakeup with retention
  SLI_SI91X_POWER_MANAGER_WAKEUP_WITH_RETENTION_WITHOUT_ULPSS_RAM,    // Wakeup with retention without ULPSS RAM
  SLI_SI91X_POWER_MANAGER_WAKEUP_WITHOUT_RETENTION_WITHOUT_ULPSS_RAM, // Wakeup without retention without ULPSS RAM
  LAST_ENUM_POWER_SLEEP_MODE,                                         // Last Enum for validation
} sli_power_sleep_mode_t;

/// @brief Enumeration for low frequency clocks
typedef enum {
  SLI_SI91X_POWER_MANAGER_DISABLE_LF_MODE   = DISABLE_LF_MODE,    // Disable low frequency clock
  SLI_SI91X_POWER_MANAGER_LF_32_KHZ_RC      = (uint32_t)1 << (0), // Low Frequency 32 KHz RC Clock
  SLI_SI91X_POWER_MANAGER_LF_32_KHZ_XTAL    = (uint32_t)1 << (1), // Low Frequency 32 KHz XTAL Clock
  SLI_SI91X_POWER_MANAGER_EXTERNAL_CAP_MODE = (uint32_t)1 << (2), // External cap mode
  LAST_ENUM_POWER_LOW_FREQ_CLOCK,                                 // Last Enum for validation
} sli_power_low_freq_clock_t;

// -----------------------------------------------------------------------------
// Prototypes
/***************************************************************************/ /**
 * Updates the Power State as per the from and to parameters.
 * 
 * @note FOR INTERNAL USE ONLY.
 * 
 * @param[in] from ( \ref sl_power_state_t type enum) Power State from which the transition takes place.
 * @param[in] to ( \ref sl_power_state_t type enum) Power State to which the transition takes place.
 * 
 * @return status SL_STATUS_OK on success, else error code as follow: 
*-
 *         - SL_STATUS_OK (0x0000) Success. 
*- 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) Invalid Parameter is passed. 
*
 ******************************************************************************/
sl_status_t sli_si91x_power_manager_change_power_state(sl_power_state_t from, sl_power_state_t to);

/***************************************************************************/ /**
 * Configures the parameters for sleep and transit to sleep mode.
 * 
 * @note FOR INTERNAL USE ONLY.
 * 
 * @param[in] state ( \ref sl_power_state_t type enum) Current Power state
 * 
 * @return status SL_STATUS_OK on success, else error code. 
*-
 *         - SL_STATUS_OK (0x0000) Success. 
*- 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) Invalid Parameter is passed. 
*
******************************************************************************/
sl_status_t sli_si91x_power_manager_set_sleep_configuration(sl_power_state_t state);

/***************************************************************************/ /**
 * Updates the peripheral power state, i.e., enables and disables the peripheral
 * as per requirements. 
 * If flag is set, the peripherals which are passed in the \ref sl_power_peripheral_t
 * structures is be powered on.
 * If flag is cleared, the peripherals which are passed in the \ref sl_power_peripheral_t
 * structures is be powered off.
 * 
 * @note FOR INTERNAL USE ONLY.
 * 
 * @param[in] peripheral ( \ref sl_power_peripheral_t type structure)
 * @param[in] add (boolean) true -> add peripheral, false -> remove peripheral.
 * 
 * @return status SL_STATUS_OK on success, else error code. 
*-
 *         - SL_STATUS_OK (0x0000) Success. 
*- 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) Invalid Parameter is passed. 
*-
 *         - SL_STATUS_NULL_POINTER (0x0022) Null Pointer is passed. 
*
 ******************************************************************************/
sl_status_t sli_power_manager_update_peripheral(sl_power_peripheral_t *peripheral, boolean_t add);

/***************************************************************************/ /**
 * Validates the power state transitions. If invalid, returns false.
 * @note FOR INTERNAL USE ONLY.
 * 
 * @param[in] from ( \ref sl_power_state_t type enum) Power State from which the transition takes place.
 * @param[in] to ( \ref sl_power_state_t type enum) Power State to which the transition takes place.
 * 
 * @return boolean value true or false.
 ******************************************************************************/
boolean_t sli_si91x_power_manager_is_valid_transition(sl_power_state_t from, sl_power_state_t to);

/***************************************************************************/ /**
 * Configures the wakeup sources.
 * If add is set, the wakeup source which are passed in the source variable is set.
 * If add is clear, the wakeup source which are passed in the source variable is clear.
 * 
 * @note FOR INTERNAL USE ONLY.
 * 
 * @param[in] source (uint32_t) Ored value of wakeup sources.
 * @param[in] flag (boolean) true -> add peripheral, false -> remove peripheral.
 * 
 * @return status SL_STATUS_OK on success, else error code. 
*-
 *         - SL_STATUS_OK (0x0000) Success. 
*- 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) Invalid Parameter is passed. 
*
 ******************************************************************************/
sl_status_t sli_si91x_power_configure_wakeup_resource(uint32_t source, boolean_t add);

/***************************************************************************/ /**
 * Sets the RAM retention as well as configures the RAM banks which needs to be retained.
 * 
 * @note FOR INTERNAL USE ONLY.
 * 
 * @param[in] sram_bank ( \ref sl_power_ram_retention_config_t type struct) 
 * 
 * @return status SL_STATUS_OK on success, else error code. 
*-
 *         - SL_STATUS_OK (0x0000) Success. 
*- 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) Invalid Parameter is passed. 
*
 *         - SL_STATUS_NULL_POINTER (0x0022) Null Pointer is passed. 
 ******************************************************************************/
sl_status_t sli_si91x_power_manager_set_ram_retention_configuration(sl_power_ram_retention_config_t *sram_bank);

/***************************************************************************/ /**
 * Configures the clock as per the input, i.e. powersave or performance.
 * 
 * @note FOR INTERNAL USE ONLY.
 * 
 * @param[in] state ( \ref sl_power_state_t type enum) current power state.
 * @param[in] mode (boolean) true -> performance mode, false -> powersave mode.
 *  
 * @return status SL_STATUS_OK on success, else error code. 
*-
 *         - SL_STATUS_OK (0x0000) Success. 
*- 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) Invalid Parameter is passed. 
*
 ******************************************************************************/
sl_status_t sli_si91x_power_manager_configure_clock(sl_power_state_t state, boolean_t mode);

/***************************************************************************/ /**
 * Sets the initial hardware configuration.
 * 
 * @note FOR INTERNAL USE ONLY.
 * 
 * @param[in] none
 * 
 * @return none
 ******************************************************************************/
void sli_si91x_power_manager_init_hardware(void);

/** @} (end addtogroup POWER-MANAGER) */

#ifdef __cplusplus
}
#endif

#endif /* SLI_SI91X_POWER_MANAGER_H */