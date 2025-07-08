/******************************************************************************
* @file sl_si91x_clock_manager.h
* @brief Clock Manager Service API implementation
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

#ifndef SL_SI91X_CLOCK_MANAGER_H
#define SL_SI91X_CLOCK_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_status.h"
#include "rsi_pll.h"

/***************************************************************************/
/**
 * @addtogroup  CLOCK-MANAGER Clock Manager
 * @ingroup SI91X_SERVICE_APIS
 * @{ 
 *
 ******************************************************************************/
// -----------------------------------------------------------------------------------
// GLOBAL DEFINES / MACROS
// Macros for defining supported PLL Ref Clock frequencies
#define PLL_REF_CLK_VAL_XTAL (40000000UL)  ///< PLL reference clock frequency value of XTAL CLK
#define MAX_PLL_FREQUENCY    (180000000UL) ///< Max PLL frequency is 180MHz
// -----------------------------------------------------------------------------------

/***************************************************************************/
/**
 * @typedef sl_si91x_m4_soc_clk_src_sel_t
 * @brief Typedef to select the clock source for the M4 core in the Si91x SOC.
 *
 * @details This typedef maps to the M4_SOC_CLK_SRC_SEL_T type, which defines the possible clock source selections for the M4 core. 
 *          The clock source selection impacts the operating frequency and performance characteristics of the M4 processor within the Si91x SOC.
 *
 *          Users can configure the clock source to optimize power consumption or performance based on application requirements.
 *
 * @note Ensure that the selected clock source is properly configured and stable before switching to avoid system instability.
 ******************************************************************************/
typedef M4_SOC_CLK_SRC_SEL_T sl_si91x_m4_soc_clk_src_sel_t;
// -----------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------
// GLOBAL FUNCTION PROTOTYPES

/***************************************************************************/
/**
 * @brief Initializes the M4_SOC and other required clocks.
 * 
 * @return sl_status_t Status code indicating the result:
 *         - SL_STATUS_OK  - Success.
 *         - Corresponding error code on failure.
 * 
 * For more information on status codes, refer to [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 ******************************************************************************/
sl_status_t sl_si91x_clock_manager_init(void);

/***************************************************************************/
/**
 * @brief To configure the M4 core clock source and configure the PLL frequency if selected as source.
 * 
 * @param[in] clk_source Enum value representing different core clock sources.
 * @param[in] pll_freq Desired M4 core frequency in MHz.
 * 
 * @return sl_status_t Status code indicating the result:
 *         - SL_STATUS_OK  - Success.
 *         - Corresponding error code on failure.
 * 
 * For more information on status codes, see [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 ******************************************************************************/
sl_status_t sl_si91x_clock_manager_m4_set_core_clk(M4_SOC_CLK_SRC_SEL_T clk_source, uint32_t pll_freq);

/***************************************************************************/
/**
 * @brief To set the selected PLL (Phase-Locked Loop) clock to the desired frequency.
 * 
 * @param[in] pll_type Enum specifying the type of PLL to configure.
 * @param[in] pll_freq Desired frequency for the PLL clock (in MHz).
 * @param[in] pll_ref_clk Reference clock frequency for the PLL configuration.
 * 
 * @return sl_status_t Status code indicating the result:
 *         - SL_STATUS_OK  - Success.
 *         - Corresponding error code on failure.
 * 
 * For more information on status codes, see [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 ******************************************************************************/
sl_status_t sl_si91x_clock_manager_set_pll_freq(PLL_TYPE_T pll_type, uint32_t pll_freq, uint32_t pll_ref_clk);

/***************************************************************************/
/**
 * @brief To read the currently active M4 core clock source and its frequency.
 * 
 * @param[out] m4_core_clk_freq Pointer to a variable where the current core clock frequency will be stored (in MHz).
 * 
 * @return sl_si91x_m4_soc_clk_src_sel_t The currently active core clock source:
 *         - 0: M4_ULPREFCLK
 *         - 2: M4_SOCPLLCLK
 *         - 3: M4_MODEMPLLCLK1
 *         - 4: M4_INTFPLLCLK
 *         - 5: M4_SLEEPCLK
 * 
 ******************************************************************************/
sl_si91x_m4_soc_clk_src_sel_t sl_si91x_clock_manager_m4_get_core_clk_src_freq(uint32_t *m4_core_clk_freq);

/***************************************************************************/
/**
 * @brief Gets the selected PLL (Phase-Locked Loop) clock to the desired frequency.
 * 
 * @param[in] pll_type Enum specifying the type of PLL to configure.
 * 
 * @return uint32_t PLL frequency value in MHz.
 ******************************************************************************/
uint32_t sl_si91x_clock_manager_get_pll_freq(PLL_TYPE_T pll_type);

/***************************************************************************/
/**
 * @brief Controls the selected PLL (Phase-Locked Loop) clock.
 * 
 * @param[in] pll_type Enum specifying the type of PLL to control.
 * @param[in] enable Boolean value to enable (true) or disable (false) the PLL.
 * 
 * @return sl_status_t Status code indicating the result:
 *         - SL_STATUS_OK  - Success.
 *         - Corresponding error code on failure.
 * 
 * For more information on status codes, see [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 ******************************************************************************/
sl_status_t sl_si91x_clock_manager_control_pll(PLL_TYPE_T pll_type, bool enable);

/***************************************************************************/
/**
 * @brief Delays execution for the specified number of milliseconds.
 *
 * @param[in] milli_seconds Delay time in milliseconds.
 *
 * @note This function provides a blocking delay.The delay is calibrated based on the SystemCoreClock frequency.
 *  If SystemCoreClock < CLOCK_THRESHOLD, the delay is calibrated with a division factor of LOW_FREQ_CLK_DIV_FAC.
 *  If SystemCoreClock >= CLOCK_THRESHOLD, the delay is calibrated with a division factor of HIGH_FREQ_CLK_DIV_FAC.
 *  This function uses `__NOP()` instructions for the delay loop.
 *
 * For more information on status codes, see [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 ***************************************************************************/
void sl_si91x_delay_ms(uint32_t milli_seconds);

// -----------------------------------------------------------------------------------

/// @} end addtogroup CLOCK-MANAGER ******************************************************/

// ******** THE REST OF THE FILE IS DOCUMENTATION ONLY! ***********************
/***************************************************************************/
/***************************************************************************/
/**
 * @addtogroup CLOCK-MANAGER Clock Manager
 * @{
 *
 * @details
 *
 * @section Clock_Manager_Intro Introduction
 *
 * The Clock Manager module is responsible for configuring the clock tree in the Si91x system.
 * It enhances the clock management features provided by the GSDK's emlib and device_init services,
 * offering additional functionality for selecting clock sources and setting dividers.
 * This module ensures optimized performance and power efficiency across various system components
 * by managing clock configurations according to application requirements.
 *
 * @section Clock_Manager_Config Configuration
 *
 * The Clock Manager supports a variety of configurations:
 * - **Clock Source Selection**: Allows specific clock sources to be chosen for different branches.
 * - **Divider Configuration**: Modifies clock frequencies by setting divider values.
 *
 * Configuration is typically performed during device initialization using configuration files and source code.
 *
 * @section Clock_Manager_Usage Usage
 *
 * The following steps outline typical usage of the Clock Manager:
 * 1. Initialize the Clock Manager with desired configuration settings.
 * 2. Set up oscillator sources and configure clock dividers.
 * 3. Use runtime APIs to manage dynamic clock changes.
 * 4. Maintain stable clock configurations to ensure system reliability.
 *
 * @section Clock_Manager_Benefits Benefits
 *
 * - Manages complex clock trees efficiently.
 * - Enhances system stability and performance.
 * - Supports a wide range of Si91x devices.
 * - Optimizes power consumption through efficient clock management.
 *
 * @} (end addtogroup CLOCK-MANAGER)
 */
/***************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SL_SI91X_CLOCK_MANAGER_H */
