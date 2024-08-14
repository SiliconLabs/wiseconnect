/************************************************************************************
 * @file sl_si91x_wdt_manager.h
 * @brief Watchdog Manager Service API implementation
 ************************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
 ************************************************************************************
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
 ************************************************************************************/

#ifndef SL_SI91X_WDT_MANAGER_
#define SL_SI91X_WDT_MANAGER_

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_status.h"
#include "rsi_wwdt.h"
#include "sl_si91x_watchdog_timer.h"

/************************************************************************************
 * @addtogroup WDT-MANAGER Clock Manager
 * @ingroup SI91X_SERVICE_APIS
 * @{
 ************************************************************************************/
// -----------------------------------------------------------------------------------
// GLOBAL DEFINES / MACROS
// -----------------------------------------------------------------------------------

#define SL_SI91X_WAKEUP_INDICATION       BIT(0)
#define SL_SI91X_TIMEOUT_WAKEUP          BIT(1)
#define SL_SI91X_HOST_BASED_WAKEUP_S     BIT(2)
#define SL_SI91X_MCU_PROCESSOR_WAKE_STAT BIT(4)
#define SL_SI91X_MCU_WWD_RESET           BIT(5)
#define SL_SI91X_MCU_WWD_WINDOW_RESET    BIT(6)
#define SL_SI91X_NWP_WWD_RESET           BIT(8)
#define SL_SI91X_NWP_WWD_WINDOW_RESET    BIT(9)
#define SL_SI91X_HOST_RESET_REQUEST      BIT(10)

#define SL_SI91X_WDT_RESET                                                                                         \
  (SL_SI91X_MCU_WWD_WINDOW_RESET | SL_SI91X_MCU_WWD_RESET | SL_SI91X_NWP_WWD_WINDOW_RESET | SL_SI91X_NWP_WWD_RESET \
   | SL_SI91X_HOST_RESET_REQUEST)

// -----------------------------------------------------------------------------------
// DATA TYPES
// -----------------------------------------------------------------------------------
/// @brief Reason for the system being reset.
typedef enum {
  SL_SI91X_WATCHDOG_MANAGER_RESET_POR,      ///< A power-on reset was performed.
  SL_SI91X_WATCHDOG_MANAGER_RESET_WATCHDOG, ///< System was reset by the watchdog manager service.
  SL_SI91X_WATCHDOG_MANAGER_RESET_SOFTWARE, ///< A reset was invoked from the system firmware.
  SL_SI91X_WATCHDOG_MANAGER_RESET_MAX
} sl_watchdog_manager_reset_reason_t;

/// @brief Enumeration to represent possible time delays values for WDT interrupt time and system reset time with 32 KHZ clock freq.
typedef enum {
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_0,   ///< for time delay of 0.03125 milliseconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_1,   ///< for time delay of 0.0625 milliseconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_2,   ///< for time delay of 0.125 milliseconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_3,   ///< for time delay of 0.25 milliseconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_4,   ///< for time delay of 0.5 milliseconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_5,   ///< for time delay of 1 milliseconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_6,   ///< for time delay of 2 milliseconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_7,   ///< for time delay of 4 milliseconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_8,   ///< for time delay of 8 milliseconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_9,   ///< for time delay of 16 milliseconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_10,  ///< for time delay of 32 milliseconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_11,  ///< for time delay of 64 milliseconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_12,  ///< for time delay of 128 milliseconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_13,  ///< for time delay of 256 milliseconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_14,  ///< for time delay of 512 milliseconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_15,  ///< for time delay of 1.024 seconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_16,  ///< for time delay of 2.048 seconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_17,  ///< for time delay of 4.096 seconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_18,  ///< for time delay of 8.192 seconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_19,  ///< for time delay of 16.384 seconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_20,  ///< for time delay of 32.768 seconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_21,  ///< for time delay of 65.536 seconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_22,  ///< for time delay of 131.072 seconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_23,  ///< for time delay of 262.144 seconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_24,  ///< for time delay of 524.288 seconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_25,  ///< for time delay of 1048.576 seconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_26,  ///< for time delay of 2097.152 seconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_27,  ///< for time delay of 4194.304 seconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_28,  ///< for time delay of 8388.60 seconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_29,  ///< for time delay of 16777.216 seconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_30,  ///< for time delay of 33554.432 seconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_31,  ///< for time delay of 67108.864 seconds
  SL_SI91X_WATCHDOG_MANAGER_TIMEOUT_INDEX_MAX, ///< for time delay  value validation
} sl_watchdog_manager_wdog_timeout_t;

// -----------------------------------------------------------------------------------
// GLOBAL FUNCTION PROTOTYPES
// -----------------------------------------------------------------------------------

/***************************************************************************/ /**
   * Configures the watchdog manager service.
   * Configures the hardware watchdog timer.
   * - The 91x watchdog peripheral's interrupt interval is set to 2 seconds less than the configured value of @ref SL_WDT_MANAGER_INTERRUPT_TIME.
   * - The 91x watchdog peripheral's system reset interval is set to the configured value of @ref SL_WDT_SYSTEM_RESET_TIME.
   *   The hardware watchdog triggers an interrupt when this timeout expires.
   * - The 91x watchdog peripheral's window interval is set almost equal to the difference between the system reset interval and interrupt interval.
   *   This is the period during which the software is permitted to feed the watchdog after the interrupt is triggered.
   *
   * @pre Pre-conditions:
   * - none
   *
   * @param[in] none
   *
   * @return The following values are returned:
   * - SL_STATUS_OK - Success
   * - SL_STATUS_XXX - <error condition description>
   ******************************************************************************/
sl_status_t sl_watchdog_manager_init(void);

/***************************************************************************/ /**
   * Start the watchdog manager service.
   * WDT starts counting the counter
   * The hardware watchdog triggers an interrupt when WDT Timeout Interval is expired.
   *
   * @pre Pre-conditions:
   * - \ref sl_watchdog_manager_init 
   *
   * @param[in] none
   *
   * @return The following values are returned:
   * - SL_STATUS_OK - Success
   * - SL_STATUS_XXX - <error condition description>
   ******************************************************************************/
sl_status_t sl_watchdog_manager_start(void);

/***************************************************************************/ /**
   * Retrieve the system reset reason.
   * Returns the reason for the system being reset. This may be invoked during a start up flow
   * to determine if the system has recovered from an error condition such as a crash.
   *
   * @pre Pre-conditions:
   * - None
   *
   * @param[out] reset_reason This is populated with the reason for the system being reset. This is of type @ref sl_watchdog_manager_reset_reason_t.
   *
   * @return The following values are returned:
   *          - \ref SL_SI91X_WATCHDOG_MANAGER_RESET_WATCHDOG  -  WDT System Reset 
   *          - \ref SL_SI91X_WATCHDOG_MANAGER_RESET_SOFTWARE  -  Software System Reset
   *          - \ref SL_SI91X_WATCHDOG_MANAGER_RESET_POR       -  Power-on Reset
   ******************************************************************************/
sl_watchdog_manager_reset_reason_t sl_wdt_manager_get_system_reset_status(void);

/***************************************************************************/ /**
   * Disables the hardware watchdog in sleep mode
   * The WDT counter stops running in sleep mode
   *
   * @pre Pre-conditions:
   * - \ref sl_watchdog_manager_init 
   *
   * @param[in] none
   *
   * @return none
   ******************************************************************************/
void sl_watchdog_disable_wdt_in_sleep(void);

/***************************************************************************/ /**
   * Enables the hardware watchdog in sleep mode
   * The WDT counter starts running in sleep mode
   * It enables the WDT as a wakeup source to the system to serve the ISR and 
   * kick the watchdog 
   * @pre Pre-conditions:
   * - \ref sl_watchdog_manager_init 
   *
   * @param[in] none
   *
   * @return none
   ******************************************************************************/
void sl_watchdog_enable_wdt_in_sleep(void);

/// @} end group WDT-MANAGER ******************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SL_SI91X_WDT_MANAGER */
