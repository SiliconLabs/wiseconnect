/***************************************************************************/ /**
 * @file sl_si91x_power_manager.h
 * @brief Power Manager Service API implementation
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_POWER_MANAGER_H
#define SL_POWER_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

// Current module name for debugging features
#ifndef CURRENT_MODULE_NAME
#define CURRENT_MODULE_NAME "Anonymous" ///< current module name
#endif

#include "sl_status.h"
#include "sl_slist.h"
#include "system_si91x.h"
#include "base_types.h"
#include "rsi_power_save.h"

#ifdef SL_SI91X_POWER_MANAGER_DEBUG_COMPONENT
#include "sl_si91x_power_manager_debug_config.h"
#endif

/***************************************************************************/ /**
 * @addtogroup POWER-MANAGER Power Manager
 * @ingroup SI91X_SERVICE_APIS
 * @{
 ******************************************************************************/

// -----------------------------------------------------------------------------
// Defines

// Flags are used to subscribe to the power manager transition events, i.e., provides callback after state transitions
// This can be used individually or can be ored and pass to the \ref sl_si91x_power_manager_subscribe_ps_transition_event

#define SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS4    (1 << 0) ///< Event transition for entering PS4 state
#define SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS4     (1 << 1) ///< Event transition for leaving PS4 state
#define SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS3    (1 << 2) ///< Event transition for entering PS3 state
#define SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS3     (1 << 3) ///< Event transition for leaving PS3 state
#define SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS2    (1 << 4) ///< Event transition for entering PS2 state
#define SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS2     (1 << 5) ///< Event transition for leaving PS2 state
#define SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS1     (1 << 6) ///< Event transition for leaving PS1 state
#define SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_SLEEP   (1 << 7) ///< Event transition for leaving sleep state
#define SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_STANDBY (1 << 8) ///< Event transition for leaving standby state

//  Wakeup sources
#define SL_SI91X_POWER_MANAGER_DST_WAKEUP        DST_BASED_WAKEUP      ///< Deep Sleep Timer based wakeup source
#define SL_SI91X_POWER_MANAGER_WIRELESS_WAKEUP   WIRELESS_BASED_WAKEUP ///< Wireless based wakeup source
#define SL_SI91X_POWER_MANAGER_GPIO_WAKEUP       GPIO_BASED_WAKEUP     ///< GPIO based wakeup source
#define SL_SI91X_POWER_MANAGER_COMPARATOR_WAKEUP COMPR_BASED_WAKEUP    ///< Comparator based wakeup source
#define SL_SI91X_POWER_MANAGER_SYSRTC_WAKEUP     SYSRTC_BASED_WAKEUP   ///< Sysrtc based wakeup source
#define SL_SI91X_POWER_MANAGER_ULPSS_WAKEUP      ULPSS_BASED_WAKEUP    ///< ULP peripheral based wakeup source
#define SL_SI91X_POWER_MANAGER_SDCSS_WAKEUP      SDCSS_BASED_WAKEUP ///< SDC (Sensor data collector) based wakeup source
#define SL_SI91X_POWER_MANAGER_ALARM_WAKEUP      ALARM_BASED_WAKEUP ///< Alarm based wakeup source
#define SL_SI91X_POWER_MANAGER_SEC_WAKEUP        SEC_BASED_WAKEUP   ///< Second based wakeup source
#define SL_SI91X_POWER_MANAGER_MSEC_WAKEUP       MSEC_BASED_WAKEUP  ///< Milli second based wakeup source
#define SL_SI91X_POWER_MANAGER_WDT_WAKEUP        WDT_INTR_BASED_WAKEUP ///< Watchdog interrupt based wakeup source

// M4SS peripherals
#define SL_SI91X_POWER_MANAGER_M4SS_PG_EFUSE        M4SS_PWRGATE_ULP_EFUSE_PERI   ///< M4SS EFUSE Power Gate
#define SL_SI91X_POWER_MANAGER_M4SS_PG_RPDMA        M4SS_PWRGATE_ULP_RPDMA        ///< M4SS RPDMA Power Gate
#define SL_SI91X_POWER_MANAGER_M4SS_PG_SDIO_SPI     M4SS_PWRGATE_ULP_SDIO_SPI     ///< M4SS SDIO SPI Power Gate
#define SL_SI91X_POWER_MANAGER_M4SS_PG_QSPI         M4SS_PWRGATE_ULP_QSPI_ICACHE  ///< M4SS QSPI and ICACHE Power Gate
#define SL_SI91X_POWER_MANAGER_M4SS_PG_IID          M4SS_PWRGATE_ULP_IID          ///< M4SS IID Power Gate
#define SL_SI91X_POWER_MANAGER_M4SS_PG_M4_DEBUG     M4SS_PWRGATE_ULP_M4_DEBUG_FPU ///< M4SS M4 Debug Power Gate
#define SL_SI91X_POWER_MANAGER_M4SS_PG_M4_CORE      M4SS_PWRGATE_ULP_M4_CORE      ///< M4SS M4 Core Power Gate
#define SL_SI91X_POWER_MANAGER_M4SS_PG_EXTERNAL_ROM M4SS_PWRGATE_ULP_EXT_ROM      ///< M4SS External ROM Power Gate

// ULPSS peripherals
#define SL_SI91X_POWER_MANAGER_ULPSS_PG_MISC ULPSS_PWRGATE_ULP_MISC ///< ULP Miscellaneous Power Gate
#define SL_SI91X_POWER_MANAGER_ULPSS_PG_CAP  ULPSS_PWRGATE_ULP_CAP  ///< ULP Capacitive Touch Sensor Power Gate
#define SL_SI91X_POWER_MANAGER_ULPSS_PG_UART ULPSS_PWRGATE_ULP_UART ///< ULP UART Power Gate
#define SL_SI91X_POWER_MANAGER_ULPSS_PG_SSI  ULPSS_PWRGATE_ULP_SSI  ///< ULP SSI Power Gate
#define SL_SI91X_POWER_MANAGER_ULPSS_PG_I2S  ULPSS_PWRGATE_ULP_I2S  ///< ULP I2S Power Gate
#define SL_SI91X_POWER_MANAGER_ULPSS_PG_I2C  ULPSS_PWRGATE_ULP_I2C  ///< ULP I2C Power Gate
#define SL_SI91X_POWER_MANAGER_ULPSS_PG_AUX  ULPSS_PWRGATE_ULP_AUX  ///< ULP AUX Power Gate
#define SL_SI91X_POWER_MANAGER_ULPSS_PG_IR   ULPSS_PWRGATE_ULP_IR   ///< ULP IR Power Gate
#define SL_SI91X_POWER_MANAGER_ULPSS_PG_UDMA ULPSS_PWRGATE_ULP_UDMA ///< ULP UDMA Power Gate
#define SL_SI91X_POWER_MANAGER_ULPSS_PG_FIM  ULPSS_PWRGATE_ULP_FIM  ///< ULP FIM Power Gate

// NPSS peripherals
#define SL_SI91X_POWER_MANAGER_NPSS_PG_MCUBFFS    SLPSS_PWRGATE_ULP_MCUBFFS ///< NPSS MCU BFFS (Battery FF's) Power Gate
#define SL_SI91X_POWER_MANAGER_NPSS_PG_MCUFSM     SLPSS_PWRGATE_ULP_MCUFSM  ///< NPSS MCU FSM Power Gate
#define SL_SI91X_POWER_MANAGER_NPSS_PG_MCURTC     SLPSS_PWRGATE_ULP_MCURTC ///< NPSS MCU RTC (Real Time Clock) Power Gate
#define SL_SI91X_POWER_MANAGER_NPSS_PG_MCUWDT     SLPSS_PWRGATE_ULP_MCUWDT ///< NPSS MCU WDT (Watchdog Timer) Power Gate
#define SL_SI91X_POWER_MANAGER_NPSS_PG_MCUPS      SLPSS_PWRGATE_ULP_MCUPS  ///< NPSS MCU Process Sensor Power Gate
#define SL_SI91X_POWER_MANAGER_NPSS_PG_MCUTS      SLPSS_PWRGATE_ULP_MCUTS  ///< NPSS MCU Temperature Sensor Power Gate
#define SL_SI91X_POWER_MANAGER_NPSS_PG_MCUSTORE1  SLPSS_PWRGATE_ULP_MCUSTORE1  ///< NPSS MCU Storage 1 Power Gate
#define SL_SI91X_POWER_MANAGER_NPSS_PG_MCUSTORE2  SLPSS_PWRGATE_ULP_MCUSTORE2  ///< NPSS MCU Storage 2 Power Gate
#define SL_SI91X_POWER_MANAGER_NPSS_PG_MCUSTORE3  SLPSS_PWRGATE_ULP_MCUSTORE3  ///< NPSS MCU Storage 3 Power Gate
#define SL_SI91X_POWER_MANAGER_NPSS_PG_TIMEPERIOD SLPSS_PWRGATE_ULP_TIMEPERIOD ///< NPSS Time Period Power Gate
#define SL_SI91X_POWER_MANAGER_NPSS_PG_NWPAPB_MCU_CTRL \
  SLPSS_PWRGATE_ULP_NWPAPB_MCU_CTRL ///< NPSS MCU APB Control Power Gate

// RAM Bank numbers
#define SL_SI91X_POWER_MANAGER_M4SS_RAM_BANK_1  RAM_BANK_0      ///< 4 KB (Bank 1 of first 192k chunk)
#define SL_SI91X_POWER_MANAGER_M4SS_RAM_BANK_2  RAM_BANK_1      ///< 4 KB (Bank 2 of first 192k chunk)
#define SL_SI91X_POWER_MANAGER_M4SS_RAM_BANK_3  RAM_BANK_2      ///< 4 KB (Bank 3 of first 192k chunk)
#define SL_SI91X_POWER_MANAGER_M4SS_RAM_BANK_4  RAM_BANK_3      ///< 4 KB (Bank 4 of first 192k chunk)
#define SL_SI91X_POWER_MANAGER_M4SS_RAM_BANK_5  RAM_BANK_4      ///< 4 KB (Bank 5 of first 192k chunk)
#define SL_SI91X_POWER_MANAGER_M4SS_RAM_BANK_6  RAM_BANK_5      ///< 32 KB (Bank 6-7 of first 192k chunk)
#define SL_SI91X_POWER_MANAGER_M4SS_RAM_BANK_7  RAM_BANK_6      ///< 64 KB (Bank 9-11 of first 192k chunk)
#define SL_SI91X_POWER_MANAGER_M4SS_RAM_BANK_8  RAM_BANK_7      ///< 64 KB (Bank 12-15 of first 192k chunk)
#define SL_SI91X_POWER_MANAGER_M4SS_RAM_BANK_9  RAM_BANK_8      ///< 64 KB (Bank 1-4 of second 192k chunk)
#define SL_SI91X_POWER_MANAGER_M4SS_RAM_BANK_10 RAM_BANK_9      ///< 64 KB (Bank 1-4 of third 192k chunk)
#define SL_SI91X_POWER_MANAGER_ULPSS_RAM_BANK_1 ULPSS_2K_BANK_0 ///< 2 KB
#define SL_SI91X_POWER_MANAGER_ULPSS_RAM_BANK_2 ULPSS_2K_BANK_1 ///< 2 KB
#define SL_SI91X_POWER_MANAGER_ULPSS_RAM_BANK_3 ULPSS_2K_BANK_2 ///< 2 KB
#define SL_SI91X_POWER_MANAGER_ULPSS_RAM_BANK_4 ULPSS_2K_BANK_3 ///< 2 KB

// -----------------------------------------------------------------------------
// Data Types

/// @brief Struture to store configuration parameters for RAM retention
typedef struct {
  uint16_t m4ss_ram_size_kb;  ///< M4SS RAM size that needs to be restored
  uint16_t ulpss_ram_size_kb; ///< ULPSS RAM size that needs to be restored
  boolean_t
    configure_ram_banks;    ///< Enable will set the RAM banks using size, disable will set RAM banks using bank number
  uint32_t m4ss_ram_banks;  ///< M4SS RAM bank number that needs to be restored
  uint32_t ulpss_ram_banks; ///< ULPSS RAM bank number that needs to be restored
} sl_power_ram_retention_config_t;

/// @brief Structure to store masked values of peripherals to enable/disable it
typedef struct {
  uint32_t m4ss_peripheral;  ///< Masked value of M4SS Peripherals \ref sl_power_m4ss_peripheral_t
  uint32_t ulpss_peripheral; ///< Masked value of ULPSS Peripherals \ref sl_power_ulpss_peripheral_t
  uint32_t npss_peripheral;  ///< Masked value of NPSS Peripherals \ref sl_power_npss_peripheral_t
} sl_power_peripheral_t;

/// @brief Enumeration for the power states
typedef enum {
  SL_SI91X_POWER_MANAGER_PS0 = 0, ///< PS0 Power State
  SL_SI91X_POWER_MANAGER_PS1,     ///< PS1 Power State
  SL_SI91X_POWER_MANAGER_PS2,     ///< PS2 Power State
  SL_SI91X_POWER_MANAGER_PS3,     ///< PS3 Power State
  SL_SI91X_POWER_MANAGER_PS4,     ///< PS4 Power State
  SL_SI91X_POWER_MANAGER_SLEEP,   ///< Sleep
  SL_SI91X_POWER_MANAGER_STANDBY, ///< Standby
  LAST_ENUM_POWER_STATE,          ///< Last enum for validation
} sl_power_state_t;

/// @brief Enumeration for clock scaling parameters
typedef enum {
  SL_SI91X_POWER_MANAGER_POWERSAVE,   ///< Minimum supported frequency in a power state
  SL_SI91X_POWER_MANAGER_PERFORMANCE, ///< Maximum supported frequency in a power state
  LAST_ENUM_CLOCK_SCALING,            ///< Last enum for validation
} sl_clock_scaling_t;

/// On ISR Exit Hook answer
typedef enum {
  SL_SI91X_POWER_MANAGER_ISR_IGNORE =
    (1UL << 0UL), ///< The module did not trigger an ISR and it doesn't want to contribute to the decision
  SL_SI91X_POWER_MANAGER_ISR_SLEEP =
    (1UL << 1UL), ///< The module was the one that caused the system wakeup and the system SHOULD go back to sleep
  SL_SI91X_POWER_MANAGER_ISR_WAKEUP =
    (1UL << 2UL), ///< The module was the one that caused the system wakeup and the system MUST NOT go back to sleep
} sl_si91x_power_manager_on_isr_exit_t;

/// @brief Mask of all the event(s) to listen to.
typedef uint32_t sl_power_manager_ps_transition_event_t;

/***************************************************************************/ /**
 * Typedef for the user supplied callback function which is called when
 * an power state transition occurs.
 *
 * @param from Power state we are leaving.
 * @param to   Power state we are entering.
 ******************************************************************************/
typedef void (*sl_power_manager_ps_transition_on_event_t)(sl_power_state_t from, sl_power_state_t to);

/// @brief Structure representing power state transition event information
typedef struct {
  const sl_power_manager_ps_transition_event_t
    event_mask; ///< Mask of the transitions on which the callback should be called.
  const sl_power_manager_ps_transition_on_event_t on_event; ///< Function that must be called when the event occurs.
} sl_power_manager_ps_transition_event_info_t;

/// @brief Structure representing power state transition event handle
typedef struct {
  sl_slist_node_t node;                              ///< List node.
  sl_power_manager_ps_transition_event_info_t *info; ///< Handle event info.
} sl_power_manager_ps_transition_event_handle_t;

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
// -----------------------------------------------------------------------------
// Internal API Prototypes
/*******************************************************************************
 * Updates the power state requirement, requirement table and current state variable.
 * 
 * 
 * @note FOR INTERNAL USE ONLY.
 * 
 * @pre Pre-conditions:
 * - none
 * 
 * @param[in] state Power state requirement that needs to be updated
 * @param[in] add  Flag indicating if requirement is added (true) or removed
 *                 (false).
 * 
 * @return The following values are returned:
 * - status SL_STATUS_OK on success, else error code. 
 * -
 *         SL_STATUS_OK (0x0000) Success. 
 * -
 *         SL_STATUS_ALREADY_INITIALIZED (0x0012) Power Manager is already initialized. 
 * 
 ******************************************************************************/
sl_status_t sli_si91x_power_manager_update_ps_requirement(sl_power_state_t state, boolean_t add);
/** @endcond */

// To make sure that we are able to optimize out the string argument when the
// debug feature is disable, we use a pre-processor macro resulting in a no-op.
// We also make sure to always have a definition for the function regardless if
// the debug feature is enable or not for binary compatibility.
#if (SL_SI91X_POWER_MANAGER_DEBUG == 1)
void sli_si91x_power_manager_debug_log_ps_requirement(sl_power_state_t ps, bool add, const char *name);
#else
#define sli_si91x_power_manager_debug_log_ps_requirement(em, add, name) /* no-op */
#endif

// -----------------------------------------------------------------------------
// Prototypes
/***************************************************************************/ /**
 * Initialize the power manager service.
 * Configures PS4 state with 100 MHz system clock.
 * 
 * @pre Pre-conditions:
 * - none
 * @param[in] none
 * @return The following values are returned:
 * - status SL_STATUS_OK on success, else error code. 
 * -
 *         SL_STATUS_OK (0x0000) Success. 
 * -
 *         SL_STATUS_ALREADY_INITIALIZED (0x0012) Power Manager is already initialized. 
 * 
 ******************************************************************************/
sl_status_t sl_si91x_power_manager_init(void);

/***************************************************************************/ /**
 * Adds requirement on power states.
 * Default state for power manager is PS4.
 * If any requirements are added then power manager switches to the state if it is a valid transition.
 * Before transition from one state to another, make sure to remove requirements of previous states if any added.
 * If any invalid state requirement is added then it returns SL_STATUS_INVALID_PARAMETER.
 * If power manager service is not initialized then it returns SL_STATUS_NOT_INITIALIZED, to initialize call \ref sl_si91x_power_manager_init.
 * To get the requirements on all power states, call \ref sl_si91x_power_manager_get_requirement_table.
 * To know the current power state, use \ref sl_si91x_power_manager_get_current_state.
 * 
 * @pre Pre-conditions:
 * - \ref sl_si91x_power_manager_init 
 * 
 * 
 * @param[in] state Power state to add requirement: \ref sl_power_state_t 
 * -
 *           SL_POWER_MANAGER_PS4 
 * -
 *           SL_POWER_MANAGER_PS3 
 * -
 *           SL_POWER_MANAGER_PS2 
 * -
 *           SL_POWER_MANAGER_PS1 
 *
 * @return The following values are returned:
 * - status SL_STATUS_OK on success, else error code. 
 *
 *         - SL_STATUS_OK (0x0000) Success. 
 *
 *         - SL_STATUS_NOT_INITIALIZED (0x0011) Power Manager is not initialized. 
 *
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) Invalid Parameter is passed.
 ******************************************************************************/
__STATIC_INLINE sl_status_t sl_si91x_power_manager_add_ps_requirement(sl_power_state_t state)
{
  sl_status_t status = SL_STATUS_OK;
  // updates the current power state.
  status = sli_si91x_power_manager_update_ps_requirement(state, true);
  if (status != SL_STATUS_OK) {
    return status;
  }
  if ((state != SL_SI91X_POWER_MANAGER_PS1) && (state != SL_SI91X_POWER_MANAGER_PS0)) {
    sli_si91x_power_manager_debug_log_ps_requirement(state, true, (const char *)CURRENT_MODULE_NAME);
  }
  return status;
}

/***************************************************************************/ /**
 * Removes requirement on power states.
 * Default state for power manager is PS4.
 * Removing requirement will not impact on power state transitions.
 * If the current state is PS4 and no other requirements are added, and PS4 requirement is removed then it returns SL_STATUS_INVALID_PARAMETER.
 * If power manager service is not initialized then it returns SL_STATUS_NOT_INITIALIZED, to initialize call \ref sl_si91x_power_manager_init.
 * To get the requirements on all power states, call \ref sl_si91x_power_manager_get_requirement_table.
 * To know the current power state, use \ref sl_si91x_power_manager_get_current_state.
 * 
 * @pre Pre-conditions:
 * - \ref sl_si91x_power_manager_init 
 *
 * 
 * @param[in] state Power state to remove requirement: \ref sl_power_state_t 
 * -
 *           SL_POWER_MANAGER_PS4 
 * -
 *           SL_POWER_MANAGER_PS3 
 * -
 *           SL_POWER_MANAGER_PS2 
 * -
 *           SL_POWER_MANAGER_PS1 
 *
 * @return The following values are returned:
 * - status SL_STATUS_OK on success, else error code.
 *         - SL_STATUS_OK (0x0000) Success. 
 * 
 *         - SL_STATUS_NOT_INITIALIZED (0x0011) Power Manager is not initialized. 
 *
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) Invalid Parameter is passed.
 ******************************************************************************/
__STATIC_INLINE sl_status_t sl_si91x_power_manager_remove_ps_requirement(sl_power_state_t state)
{
  sl_status_t status = SL_STATUS_OK;
  // updated the current power state.
  status = sli_si91x_power_manager_update_ps_requirement(state, false);
  if (status != SL_STATUS_OK) {
    return status;
  }
  if ((state != SL_SI91X_POWER_MANAGER_PS1) && (state != SL_SI91X_POWER_MANAGER_PS0)) {
    sli_si91x_power_manager_debug_log_ps_requirement(state, false, (const char *)CURRENT_MODULE_NAME);
  }
  return status;
}

/***************************************************************************/ /**
 * Configures the clock scaling.
 * PS4 and PS3 states are supported only.
 * Possible values for clock scaling are:
 * - POWERSAVE and PERFORMANCE 
 * -
 * PS4 Performance: 180 MHz clock 
 * -
 * PS4 Power-save:   100 MHz clock 
 * -
 * PS3 Performance: 80 MHz clock 
 * -
 * PS3 Power-save:   32 MHz clock 
 * -
 * For PS2 state, 20 MHz clock is default. 
 * -
 * If power manager service is not initialized then it returns SL_STATUS_NOT_INITIALIZED, to initialize call \ref sl_si91x_power_manager_init.
 * 
 * @pre Pre-conditions:
 * - \ref sl_si91x_power_manager_init 
 *
 * 
 * @param[in] mode (sl_clock_scaling_t type enum) the clock scaling mode \ref sl_clock_scaling_t
 * @return The following values are returned:
 * - status SL_STATUS_OK on success, else error code. 
 * 
 *         - SL_STATUS_OK (0x0000) Success. 
 *  
 *         - SL_STATUS_NOT_INITIALIZED (0x0011) Power Manager is not initialized. 
 * 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) Invalid Parameter is passed. 
 * 
 *         - SL_STATUS_INVALID_CONFIGURATION (0x0023) Invalid configuration of mode. 
 * 
 ******************************************************************************/
sl_status_t sl_si91x_power_manager_set_clock_scaling(sl_clock_scaling_t mode);

/***************************************************************************/ /**
 * Adds the peripheral requirement.
 * Power on the peripherals the valid peripherals passed in the structure. 
 * Structure member possible values: \ref sl_power_peripheral_t
 * - m4ss_peripheral  -> Accepts masked value of m4ss peripherals.
 * - ulpss_peripheral -> Accepts masked value of ulpss peripherals.
 * - npss_peripheral  -> Accepts masked value of npss peripherals. 
 * The values of enums can be combined by using 'OR' operator and then passed to the variable.
 * 
 * @pre Pre-conditions:
 * - \ref sl_si91x_power_manager_init 
 * 
 * 
 * @param[in] peripheral structure for different peripherals \ref sl_power_peripheral_t 
 * 
 * @return The following values are returned:
 * - status SL_STATUS_OK on success, else error code. 
 * 
 *         - SL_STATUS_OK (0x0000) Success. 
 *  
 *         - SL_STATUS_NOT_INITIALIZED (0x0011) Power Manager is not initialized. 
 * 
 *         - SL_STATUS_INVALID_STATE (0x0002) Not a valid transition. 
 * 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) Invalid Parameter is passed.
 ******************************************************************************/
sl_status_t sl_si91x_power_manager_add_peripheral_requirement(sl_power_peripheral_t *peripheral);

/***************************************************************************/ /**
 * Removes the peripheral requirement.
 * Power off the peripherals the valid peripherals passed in the structure.
 * Structure member possible values: \ref sl_power_peripheral_t
 * - m4ss_peripheral  -> Accepts masked value of m4ss peripherals.  
 * - ulpss_peripheral -> Accepts masked value of ulpss peripherals. 
 * - npss_peripheral  -> Accepts masked value of npss peripherals.
 * The values of enums can be combined by using 'OR' operator and then passed to the variable.
 * 
 * @pre Pre-conditions:
 * - \ref sl_si91x_power_manager_init 
 * 
 * 
 * @param[in] peripheral Structure for different peripherals \ref sl_power_peripheral_t 
 * 
 * @return The following values are returned:
 * - status SL_STATUS_OK on success, else error code. 
 * 
 *         - SL_STATUS_OK (0x0000) Success. 
 *  
 *         - SL_STATUS_NOT_INITIALIZED (0x0011) Power Manager is not initialized. 
 * 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) Invalid Parameter is passed.
 ******************************************************************************/
sl_status_t sl_si91x_power_manager_remove_peripheral_requirement(sl_power_peripheral_t *peripheral);

/***************************************************************************/ /**
 * Registers a callback to be called on given Power state transition(s).
 * If power manager service is not initialized then it returns SL_STATUS_NOT_INITIALIZED, 
 * to initialize call \ref sl_si91x_power_manager_init.
 * 
 * @pre Pre-conditions:
 * - \ref sl_si91x_power_manager_init 
 * -
 *
 * @param[in] event_handle  Event handle (no initialization needed).
 * @param[in] event_info    Event info structure that contains the event mask and the callback that must be called.
 * 
 * @return The following values are returned:
 * - status SL_STATUS_OK on success, else error code.
 *         - SL_STATUS_OK (0x0000) Success. 
* 
 *         - SL_STATUS_NOT_INITIALIZED (0x0011) Power Manager is not initialized. 
*
 *         - SL_STATUS_NULL_POINTER (0x0022) Null Pointer is passed. 
*
 *
 * @note Adding and removing power state transition requirement(s) from a callback on a transition event
 *       is not supported.
 * @note The parameters passed must be persistent, meaning that they need to survive
 *       until the callback fires.
 * @note  An ASSERT is thrown if the handle is not found.
 *
 * Usage example:
 *  ```c
 * #define PS_EVENT_MASK          (  SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS4 \
 *                                 | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS4  \
 *                                 | SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS3 \
 *                                 | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS3  \
 *                                 | SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS2 \
 *                                 | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS2  \
 *                                 | SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS0 \
 *                                 | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_SLEEP)
 *
 * sl_power_manager_ps_transition_event_handle_t handle;
 * sl_power_manager_ps_transition_event_info_t info = { .event_mask = PS_EVENT_MASK,
 *                                                      .on_event   = transition_callback };
 *
 * void transition_callback(sl_power_state_t from, sl_power_state_t to)
 * {
 *   [...]
 * }
 *
 * void main(void)
 * {
 *   sl_status_t status;
 * 
 *   status = sl_si91x_power_manager_init();
 *   // Validate the status
 * 
 *   status = sl_si91x_power_manager_subscribe_ps_transition_event(&handle, &info);
 *   // Validate the status
 * }
 * ```
 ******************************************************************************/
sl_status_t sl_si91x_power_manager_subscribe_ps_transition_event(
  sl_power_manager_ps_transition_event_handle_t *event_handle,
  const sl_power_manager_ps_transition_event_info_t *event_info);

/***************************************************************************/ /**
 * Unregisters an event callback handle on Power State transition.
 * If power manager service is not initialized then it returns SL_STATUS_NOT_INITIALIZED, 
 * to initialize call \ref sl_si91x_power_manager_init.
 * 
 * @pre Pre-conditions:
 * -  \ref sl_si91x_power_manager_init 
 * -
 *       \ref sl_si91x_power_manager_subscribe_ps_transition 
*
 * 
 * @param[in] event_handle  Event handle which must be unregistered (must have been
 *                          registered previously).
 * @param[in] event_info    Event info structure that contains the event mask and the callback that must be called.
 * 
 * @return The following values are returned:
 * - status SL_STATUS_OK on success, else error code.
 *         - SL_STATUS_OK (0x0000) Success. 
* 
 *         - SL_STATUS_NOT_INITIALIZED (0x0011) Power Manager is not initialized. 
*
 *         - SL_STATUS_NULL_POINTER (0x0022) Null Pointer is passed. 
*
 * 
 * @note  An ASSERT is thrown if the handle is not found.
 ******************************************************************************/
sl_status_t sl_si91x_power_manager_unsubscribe_ps_transition_event(
  sl_power_manager_ps_transition_event_handle_t *event_handle,
  const sl_power_manager_ps_transition_event_info_t *event_info);

/***************************************************************************/ /**
 * Transit to sleep mode and waits for the peripheral set as wakeup source
 * to trigger and wakeup the m4 soc.
 * It supports PS4, PS3 and PS2 only, it cannot enter sleep mode from any other 
 * active states.
 * If any error is there, it returns the error code and does not transit to sleep mode.
 * 
 * @note This function expects and call a callback with the following
 *       signature: `boolean_t sl_si91x_power_manager_is_ok_to_sleep(void)`.
 *       This function can be used to cancel a sleep action and handle the
 *       possible race condition where an ISR that would cause a wakeup is
 *       triggered right after the decision to call sl_si91x_power_manager_sleep()
 *       has been made.
 * 
 * This function also expects and call a callback with the following
 * signature: `boolean_t sl_si91x_power_manager_isr_wakeup(void)` after 
 * wakeup from sleep. The possible return values are 
 *        - SL_SI91X_POWER_MANAGER_ISR_IGNORE
 *        - SL_SI91X_POWER_MANAGER_ISR_SLEEP
 *        - SL_SI91X_POWER_MANAGER_ISR_WAKEUP
 * @note It can end up in infinite sleep-wakeup loop if continuously SL_SI91X_POWER_MANAGER_ISR_SLEEP 
 *       return value is passed.
 * 
 * @pre Pre-conditions:
 * - \ref sl_si91x_power_manager_init 
 * -
 *      \ref sl_si91x_power_manager_subscribe_ps_transition 
 * -
 *      \ref sl_si91x_power_manager_configure_ram_retention 
 * -
 *      \ref sl_si91x_power_manager_set_wakeup_sources 
*
 * 
 * @param none
 * 
 * @return The following values are returned:
 * - status SL_STATUS_OK on success, else error code.
 *         - SL_STATUS_OK (0x0000) Success. 
* 
 *         - SL_STATUS_NOT_INITIALIZED (0x0011) Power Manager is not initialized. 
*
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) Invalid Parameter is passed. 
*
 *         - SL_STATUS_INVALID_STATE (0x0002) Not a valid transition.
 ******************************************************************************/
sl_status_t sl_si91x_power_manager_sleep(void);

/***************************************************************************/ /**
 * Transit to standby state and waits for the interrupt.
 * Standby transition is possible from PS4, PS3, PS2 state only.
 * Transition from sleep, PS1 or PS0 is not supported.
 * 
 * @pre Pre-conditions:
 * - \ref sl_si91x_power_manager_init 
 * -
 *      \ref sl_si91x_power_manager_subscribe_ps_transition 
*
 * 
 * @param none
 * 
 * @return The following values are returned:
 * - none
 ******************************************************************************/
void sl_si91x_power_manager_standby(void);

/***************************************************************************/ /**
 * Configures the wakeup sources.
 * One or many wakeup sources can be configured by using 'OR' operation.
 * The initialization of the peripheral configured as wakeup source needs to be 
 * performed by user. Power Manager only sets it as a wakeup source.
 * 
 * @pre Pre-conditions:
 * - \ref sl_si91x_power_manager_init 
 * -
 *      \ref sl_si91x_power_manager_configure_ram_retention 
*
 * 
 * @post \ref sl_si91x_power_manager_sleep  OR 
 * -
 *       \ref sl_si91x_power_manager_standby 
*
 * 
 * @param[in] source (sl_power_wakeup_sources_t type enum) Wakeup sources \ref sl_power_wakeup_sources_t
 * @param[in] add (boolean_t) true enables and false disables wakeup source
 * @return The following values are returned:
 * - status SL_STATUS_OK on success, else error code.
 *         - SL_STATUS_OK (0x0000) Success. 
* 
 *         - SL_STATUS_NOT_INITIALIZED (0x0011) Power Manager is not initialized. 
*
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) Invalid Parameter is passed. 
*
 ******************************************************************************/
sl_status_t sl_si91x_power_manager_set_wakeup_sources(uint32_t source, boolean_t add);

/***************************************************************************/ /**
 * Retains the RAM in low power state either by using size or RAM bank as input parameter.
 * Structure member possible values: \ref sl_power_ram_retention_config_t
 * - configure_ram_banks -> Boolean to switch between RAM Bank retentions. Either by size
 *                          or by RAM bank number. 
 * - 
 *                          (Enable -> Use RAM Bank Number). 
 * -
 *                          (Disable -> Use Size). 
 * -
 * - m4ss_ram_size_kb -> Retains m4ss RAM banks according to the size. 
 * - 
 *                       less than 320 KB (Enter 100 for 100 KB).
 * - ulpss_ram_size_kb -> Retains ulpss RAM banks according to the size. 
 * - 
 *                        less than 8 KB (Enter 5 for 5 KB).
 * - ram_bank_number -> Retains the m4ss and ulpss RAM Bank using bank number
 * 
 * @pre Pre-conditions:
 * - \ref sl_si91x_power_manager_init 
 * -
 *      \ref sl_si91x_power_manager_subscribe_ps_transition 
*
 *  
 * @post \ref sl_si91x_power_manager_sleep
 * 
 * @param[in] config Structure for the parameters of RAM retention \ref sl_power_ram_retention_config_t.
 * @return The following values are returned:
 * - status SL_STATUS_OK on success, else error code.
 *         - SL_STATUS_OK (0x0000) Success. 
* 
 *         - SL_STATUS_NOT_INITIALIZED (0x0011) Power Manager is not initialized. 
*
 *         - SL_STATUS_NULL_POINTER (0x0022) Null Pointer is passed. 
*
 ******************************************************************************/
sl_status_t sl_si91x_power_manager_configure_ram_retention(sl_power_ram_retention_config_t *config);

/***************************************************************************/ /**
 * Returns the current power state.
 * Possible return values: 
*
 *  - 2 : SL_POWER_MANAGER_PS2,     ///< PS2 Power State 
*
 *  - 3 : SL_POWER_MANAGER_PS3,     ///< PS3 Power State 
*
 *  - 4 : SL_POWER_MANAGER_PS4,     ///< PS4 Power State 
*
 * 
 * @param[in] none
 * @return The following values are returned:
 * - sl_power_state_t enum value indicating current power state
 ******************************************************************************/
sl_power_state_t sl_si91x_power_manager_get_current_state(void);

/***************************************************************************/ /**
 * Get the current requirements on all the power states.
 * It returns 5 values starting from PS0 to PS4.
 * 
 * @pre Pre-conditions:
 * - \ref sl_si91x_power_manager_init 
*
 * 
 * @param[in] none
 * @return The following values are returned:
 * - Pointer to the uint8_t type array which contains 5 elements
 * 
 * Usage example:
 *  ```c
 * void main()
 * {
 *   uint8_t *requirement_table;
 *   sl_si91x_power_manager_init();
 *   requirement_table = sl_si91x_power_manager_get_requirement_table();
 *   DEBUGOUT("PS4: %d, PS3: %d, PS2: %d, PS1: %d, PS0: %d", 
 * -
 *            requirement_table[4], 
 * -
 *            requirement_table[3], 
 * -
 *            requirement_table[2], 
 * -
 *            requirement_table[1], 
 * -
 *            requirement_table[0]); 
 * -
 * }
 * ```
 ******************************************************************************/
uint8_t *sl_si91x_power_manager_get_requirement_table(void);

/***************************************************************************/ /**
 * De-initialize the power manager service.
 * It clears all the power requirements and callback event subscriptions.
 * If power manager service is not initialized then it returns SL_STATUS_NOT_INITIALIZED, to initialize call \ref sl_si91x_power_manager_init.
 * 
 * @pre Pre-conditions:
 * - \ref sl_si91x_power_manager_init 
*
 * 
 * @param[in] none
 * @return The following values are returned:
 * - none
 ******************************************************************************/
void sl_si91x_power_manager_deinit(void);

/** @} (end addtogroup POWER-MANAGER) */

// ******** THE REST OF THE FILE IS DOCUMENTATION ONLY !***********************
/// @addtogroup POWER-MANAGER Power Manager
/// @{
///
///   @details
///
///
///   @n @section POWER-MANAGER_Intro Introduction
///   The power manager is a platform-level software module that manages the system's power states. The power state requirements are set by the different software modules (drivers, stacks, application code, etc...).
///   Power manager also offers a notification mechanism through which any piece of software module can be notified of power state transitions through callbacks.
///
///   ***Initialization***
///
///   Power manager must be initialized before any call to power manager API.
///   If sl_system is used, only sl_system_init() must be called, otherwise
///   @ref sl_si91x_power_manager_init() must be called manually.
///
///   ***Add and remove requirements***
///
///   The driver/application can add and remove power state requirements, at runtime. Add requirement will change the power state, remove requirement function call will not have any effect for state transition.
///   @ref sl_si91x_power_manager_add_peripheral_requirement()
///
///   @ref sl_si91x_power_manager_remove_peripheral_requirement()
///
///   ***Subscribe to events***
///
///   It is possible to get notified when the system transitions from a power state to
///   another power state. This can allow to do some operations depending on which level
///   the system goes, such as saving/restoring context.
///   @ref sl_si91x_power_manager_subscribe_ps_transition_event()
///
///   @ref sl_si91x_power_manager_unsubscribe_ps_transition_event()
///
///   ***Sleep***
///
///   When the software has no more operation and only needs to wait for an event, the
///   software must call @ref sl_si91x_power_manager_sleep().
///
///   Query callback functions
///
///   Is OK to sleep
///
///   Between the time @ref sl_si91x_power_manager_sleep() is called and the MCU goes to sleep, an ISR may occur and require the system to resume at that time instead of sleeping. So a callback is called in a critical section to validate that the MCU can go to sleep.
///
///   The function sl_si91x_power_manager_is_ok_to_sleep() will be generated automatically by Simplicity Studio's wizard.
///   The function will look at multiple software modules from the SDK to make a decision.
///   The application can contribute to the decision by defining the function `app_is_ok_to_sleep().
///   If any of the software modules (including the application via app_is_ok_to_sleep()) return false,
///   the process of entering in sleep will be aborted.
///
///   ***Sleep on ISR exit***
///
///   When the system enters sleep, the only way to wake it up is via an interrupt or
///   exception. By default, power manager will assume that when an interrupt
///   occurs and the corresponding ISR has been executed, the system must not go back
///   to sleep. However, in the case where all the processing related to this interrupt
///   is performed in the ISR, it is possible to go back to sleep by using this hook.
///
///   The function sl_si91x_power_manager_sleep_on_isr_exit() will be generated
///   automatically by Simplicity Studio's wizard. The function will look at multiple software modules from the SDK
///   to make a decision. The application can contribute to the decision by defining the
///   function app_sleep_on_isr_exit().
///   The generated function will make a decision based on the value returned by the different software modules
///   (including the application via app_sleep_on_isr_exit()):
///
///   SL_SI91X_POWER_MANAGER_ISR_IGNORE: if the software module did not cause the system wakeup and/or doesn't want to contribute to the decision.
///
///   SL_SI91X_POWER_MANAGER_ISR_SLEEP: if the software module did cause the system wakeup, but the system should go back to sleep.
///
///   SL_SI91X_POWER_MANAGER_ISR_WAKEUP: if the software module did cause the system wakeup, and the system should not go back to sleep.
///
///   If any software module returned SL_SI91X_POWER_MANAGER_ISR_SLEEP and none returned SL_SI91X_POWER_MANAGER_ISR_WAKEUP,
///   the system will go back to sleep. Any other combination will cause the system not to go back to sleep.
///
///   ***Debugging feature***
///
///   By installing power manager debug component and setting the configuration define SL_SI91X_POWER_MANAGER_DEBUG to 1, it is possible
///   to record the requirements currently set and their owner. It is possible to print
///   at any time a table that lists all the added requirements and their owner. This
///   table can be printed by calling the function:
///   @ref sl_power_manager_debug_print_em_requirements().
///
///   Make sure to add the following define
///
///  ```C
///   #define CURRENT_MODULE_NAME // Module printable name here
///  ```
///   to any application code source file that adds and removes requirements.
///
///
///   @n @section POWER-MANAGER_Config Configuration
///
///   Power manager allows configuration of RAM retention, peripheral states, wakeup sources, and clock scaling.
///
///   - Clock Scaling:
///    - Clock can be configured as power-save or performance based on the user requirement. By default after the state change, the clock is configured as powersave. The below API is used to set the clock scaling.
///      @ref sl_si91x_power_manager_set_clock_scaling(); where mode can be either power-save or performance.
///   - Peripheral States:
///    - High power, ULPSS, and NPSS peripherals can be configured using the below APIs. It can be powered on or off based on user requirements.
///      @ref sl_si91x_power_manager_add_peripheral_requirement()
///      @ref sl_si91x_power_manager_remove_peripheral_requirement();
///   - RAM retention:
///    - Retains the RAM in low power state either by using size or RAM bank as input parameter.
///      @ref sl_si91x_power_manager_configure_ram_retention();
///   - Wakeup source:
///    - The wakeup sources can be configured using the below API.
///       @ref sl_si91x_power_manager_set_wakeup_sources();
///
///   @n @section POWER-MANAGER_Usage Usage
///
///   - Initialization:
///    - @ref sl_si91x_power_manager_init(): Initializes the power manager service and sets the initial power state.
///   - Peripheral requirements:
///    - @ref sl_si91x_power_manager_add_peripheral_requirement(): It turns on/off the peripheral power
///    - @ref sl_si91x_power_manager_remove_peripheral_requirement(): Removes the peripheral requirement.
///   - Power State Transitions:
///    - @ref sl_si91x_power_manager_subscribe_ps_transition_event(): Registers a callback for a specific power state transition.
///    - @ref sl_si91x_power_manager_sleep(): Puts the device into sleep mode.
///    - @ref sl_si91x_power_manager_unsubscribe_ps_transition_event(): Unregisters the callback for the power state transition.
///   - De-initialization:
///    - @ref sl_si91x_power_manager_deinit(): De-initializes the power manager service.
///
///   ***Usage Example***
///
///   ```C
///   #define EM_EVENT_MASK_ALL  (SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS4
///                               | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS4
///                               | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS3
///                               | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS3
///                               | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS2
///                               | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS2
///                               | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_SLEEP)
///
///   static void power_manager_app(void)
///   {
///   sl_status_t status;
///   sl_power_manager_ps_transition_event_handle_t handle;
///   sl_power_manager_ps_transition_event_info_t info = { .event_mask = PS_EVENT_MASK, .on_event = transition_callback };
///
///
///   // Subscribe the state transition callback events, the ored value of flag and function pointer is passed in this API.
///   status = sl_si91x_power_manager_subscribe_ps_transition_event(&handle, &info);
///   if (status != SL_STATUS_OK) {
///     // If status is not OK, return with the error code.
///     return;
///    }
///   // Configuring the RAM retention used for sleep-wakeup.
///   sl_power_ram_retention_config_t config;
///    config.configure_ram_banks = true;
///    config.m4ss_ram_banks      = SL_SI91X_POWER_MANAGER_M4SS_RAM_BANK_8 | SL_SI91X_POWER_MANAGER_M4SS_RAM_BANK_9 | SL_SI91X_POWER_MANAGER_M4SS_RAM_BANK_10;
///    config.ulpss_ram_banks     = SL_SI91X_POWER_MANAGER_ULPSS_RAM_BANK_2 | SL_SI91X_POWER_MANAGER_ULPSS_RAM_BANK_3;
///    // RAM retention modes are configured and passed into this API.
///    status = sl_si91x_power_manager_configure_ram_retention(&config);
///    if (status != SL_STATUS_OK) {
///    // If status is not OK, return with the error code.
///     return;
///    }
///
///   // Change state to PS2
///   sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS2);
///
///   // CODE BLOCK //
///
///   // Change state to PS4
///   // Removed PS2 requirement as it is no longer required.
///   sl_si91x_power_manager_remove_ps_requirement(SL_SI91X_POWER_MANAGER_PS2);
///   sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS2);
///
///   // SLEEP_WAKEUP
///
///   // Initialize wakeup source
///   // Replace the wakeup source peripheral macro defined in sl_si91x_power_manager.h file
///   // It sets the below peripheral as wakeup source
///   sl_si91x_power_manager_set_wakeup_source(WAKEUP_SOURCE, true);
///   sl_si91x_power_manager_sleep();
///   }
///
///   ```
///
/// @} end group POWER-MANAGER ********************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SL_POWER_MANAGER_H */
