/************************************************************************************
 * @file sl_si91x_clock_manager.h
 * @brief Clock Manager Service API implementation
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

#ifndef SL_CLOCK_MANAGER_H
#define SL_CLOCK_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_status.h"
#include "rsi_pll.h"

/************************************************************************************
 * @addtogroup CLOCK-MANAGER Clock Manager
 * @ingroup SI91X_SERVICE_APIS
 * @{
 ************************************************************************************/
// -----------------------------------------------------------------------------------
// GLOBAL DEFINES / MACROS
// -----------------------------------------------------------------------------------
#define PLL_REF_CLK_VAL_RC_32MHZ ((uint32_t)(32000000)) // PLL reference clock frequency value of RC_32MHZ_CLK
#define PLL_REF_CLK_VAL_XTAL     ((uint32_t)(40000000)) // PLL reference clock frequency value of XTAL_CLK

// -----------------------------------------------------------------------------------
// DATA TYPES
// -----------------------------------------------------------------------------------
typedef M4_SOC_CLK_SRC_SEL_T sl_si91x_m4_soc_clk_src_sel_t;
// -----------------------------------------------------------------------------------
// GLOBAL FUNCTION PROTOTYPES
// -----------------------------------------------------------------------------------
/**
 * @fn	        sl_status_t sl_si91x_clock_manager_m4_set_core_clk(M4_SOC_CLK_SRC_SEL_T clk_source,
                                                          uint32_t pll_freq)
 * @brief		    This API is used to configure the m4_soc clock
 * @param[in]	  pCLK: Pointer to the pll register instance
 * @param[in]   clk_source: Enum values of different Core source clocks. See #M4_SOC_CLK_SRC_SEL_T and NOTE for more info
 * @param[in]   pll_freq: M4 Core Frequency value (in MHz) to set 
 * @param[in]   div_factor: Division value for M4 Core clock
 * @return 		  returns zero \ref RSI_OK on success, corresponding error code on failure
 * @note        For using UlpRefClk clksource need to configure M4ssRefClk frequency. For that need to call \ref RSI_CLK_M4ssRefClkConfig Api first
 *              -  For using SocPllCLK clksource need to configure SocPll frequency. For that need to call \ref RSI_CLK_SetSocPllFreq Api first
 *							-  For using IntfPllCLK clksource need to configure IntfPll frequency. For that need to call \ref RSI_CLK_SetIntfPllFreq Api first
 *							-  For using Sleep clksource need to configure Sleep Clock. For that need to call \ref RSI_CLK_SlpClkConfig Api first
 */
sl_status_t sl_si91x_clock_manager_m4_set_core_clk(M4_SOC_CLK_SRC_SEL_T clk_source, uint32_t pll_freq);

/**
 * @fn	        sl_status_t sl_si91x_clock_manager_set_pll_freq(PLL_TYPE_T pll_type, uint32_t pll_freq, uint32_t pll_ref_clk)
 * @brief		    This API is used to set the selected PLL clock to desired frequency
 * @param[in]	  pll_type: PLL type to configure
 * @return 		  returns zero \ref RSI_OK on success, corresponding error code on failure
 */
sl_status_t sl_si91x_clock_manager_set_pll_freq(PLL_TYPE_T pll_type, uint32_t pll_freq, uint32_t pll_ref_clk);

/**
 * @fn	        sl_si91x_m4_soc_clk_src_sel_t sl_si91x_clock_manager_m4_get_core_clk_src_freq(uint32_t *core_clock)
 * @brief		    This API is used to read the currently active m4_soc clock source and frequency
 * @param[in]	  core_clock: Pointer to fill core clock frequency
 * @return 		  returns the currently active core clock source of type sl_si91x_m4_soc_clk_src_sel_t
 */
sl_si91x_m4_soc_clk_src_sel_t sl_si91x_clock_manager_m4_get_core_clk_src_freq(uint32_t *core_clock);

/// @} end group CLOCK-MANAGER ******************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SL_CLOCK_MANAGER_H */
