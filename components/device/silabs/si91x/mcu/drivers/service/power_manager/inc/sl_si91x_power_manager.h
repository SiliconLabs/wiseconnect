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

#include "sl_status.h"
#include "sl_slist.h"
#include "system_si91x.h"
#include "base_types.h"
#include "rsi_power_save.h"

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
#define SL_SI91X_POWER_MANAGER_DST_WAKEUP          DST_BASED_WAKEUP      ///< Deep Sleep Timer based wakeup source
#define SL_SI91X_POWER_MANAGER_HOST_WAKEUP         HOST_BASED_WAKEUP     ///< Host based wakeup source
#define SL_SI91X_POWER_MANAGER_WIRELESS_WAKEUP     WIRELESS_BASED_WAKEUP ///< Wireless based wakeup source
#define SL_SI91X_POWER_MANAGER_M4_PROCESSOR_WAKEUP M4_PROCS_BASED_WAKEUP ///< M4 Processor based wakeup source
#define SL_SI91X_POWER_MANAGER_GPIO_WAKEUP         GPIO_BASED_WAKEUP     ///< GPIO based wakeup source
#define SL_SI91X_POWER_MANAGER_COMPARATOR_WAKEUP   COMPR_BASED_WAKEUP    ///< Comparator based wakeup source
#define SL_SI91X_POWER_MANAGER_SYSRTC_WAKEUP       SYSRTC_BASED_WAKEUP   ///< Sysrtc based wakeup source
#define SL_SI91X_POWER_MANAGER_ULPSS_WAKEUP        ULPSS_BASED_WAKEUP    ///< ULP peripheral based wakeup source
#define SL_SI91X_POWER_MANAGER_SDCSS_WAKEUP        SDCSS_BASED_WAKEUP ///< SDC (Sensor data collector) based wakeup source
#define SL_SI91X_POWER_MANAGER_ALARM_WAKEUP        ALARM_BASED_WAKEUP    ///< Alarm based wakeup source
#define SL_SI91X_POWER_MANAGER_SEC_WAKEUP          SEC_BASED_WAKEUP      ///< Second based wakeup source
#define SL_SI91X_POWER_MANAGER_MSEC_WAKEUP         MSEC_BASED_WAKEUP     ///< Milli second based wakeup source
#define SL_SI91X_POWER_MANAGER_WDT_WAKEUP          WDT_INTR_BASED_WAKEUP ///< Watchdog interrupt based wakeup source

//  Ram retention modes
#define SL_SI91X_POWER_MANAGER_HPSRAM_RETENTION_ULP_ENABLE \
  HPSRAM_RET_ULP_MODE_EN ///< High Power ULP SRAM retention enable
#define SL_SI91X_POWER_MANAGER_M4SS_RAM_RETENTION_ENABLE  M4SS_RAM_RETENTION_MODE_EN  ///< M4SS RAM retention enable
#define SL_SI91X_POWER_MANAGER_M4ULP_RAM_RETENTION_ENABLE M4ULP_RAM_RETENTION_MODE_EN ///< M4 ULP RAM retention enable
#define SL_SI91X_POWER_MANAGER_TA_RAM_RETENTION_ENABLE    TA_RAM_RETENTION_MODE_EN    ///< TA RAM retention enable
#define SL_SI91X_POWER_MANAGER_ULPSS_RAM_RETENTION_ENABLE ULPSS_RAM_RETENTION_MODE_EN ///< ULPSS RAM retention enable
#define SL_SI91X_POWER_MANAGER_M4ULP_RAM16K_RETENTION_ENABLE \
  M4ULP_RAM16K_RETENTION_MODE_EN ///< M4 ULP 16K RAM retention enable

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
  uint16_t m4ss_ram_size_kb;         ///< M4SS RAM size that needs to be restored
  uint16_t ulpss_ram_size_kb;        ///< ULPSS RAM size that needs to be restored
  boolean_t configure_ram_retention; ///< Set or Clear RAM retention
  boolean_t
    configure_ram_banks;    ///< Enable will set the RAM banks using size, disable will set RAM banks using bank number
  uint32_t m4ss_ram_banks;  ///< M4SS RAM bank number that needs to be restored
  uint32_t ulpss_ram_banks; ///< ULPSS RAM bank number that needs to be restored
  uint32_t ram_retention_mode; ///< RAM Retention modes based on the flags \ref sl_power_ram_retention_mode_t
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
sl_status_t sl_si91x_power_manager_add_ps_requirement(sl_power_state_t state);

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
sl_status_t sl_si91x_power_manager_remove_ps_requirement(sl_power_state_t state);

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
 * @param[in] none
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
 * @param[in] none
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
 * - configure_ram_retention -> Boolean to enable RAM retention. (Enable/Disable).
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
 * - ram_retention_mode -> Configures the RAM retention mode 
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

#ifdef __cplusplus
}
#endif

#endif /* SL_POWER_MANAGER_H */
