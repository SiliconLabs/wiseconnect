/************************************************************************************
 * @file sl_si91x_clock_manager.c
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

#include "sl_si91x_clock_manager.h"
#include "rsi_rom_clks.h"

/************************************************************************************
 *************************  DEFINES / MACROS  ***************************************
 ************************************************************************************/
#define MANUAL_LOCK            1    // Manual lock enable
#define BYPASS_MANUAL_LOCK     1    // Bypass manual lock enable
#define SOC_PLL_MM_COUNT_LIMIT 0xA4 // Soc pll count limit
#define DIVISION_FACTOR        0    // Division factor
#define QSPI_ODD_DIV_ENABLE    0    // Odd division enable for QSPI clock
#define QSPI_SWALLO_ENABLE     0    // Swallo enable for QSPI clock
#define QSPI_DIVISION_FACTOR   0    // Division factor for QSPI clock
/************************************************************************************
 *************************  LOCAL VARIABLES  ****************************************
 ************************************************************************************/

/************************************************************************************
 *************************  LOCAL TYPE DEFINITIONS  *********************************
 ************************************************************************************/

/************************************************************************************
 *************************  LOCAL FUNCTION PROTOTYPES  ******************************
 ************************************************************************************/
static sl_status_t convert_rsi_to_sl_error_code(rsi_error_t error);
/************************************************************************************
 *************************  GLOBAL FUNCTION DEFINITIONS  ****************************
 ************************************************************************************/

/**
 * @fn	        sl_status_t sl_si91x_clock_manager_m4_set_core_clk(M4_SOC_CLK_SRC_SEL_T clk_source,
                                                          uint32_t pll_freq)
 * @brief		    This API is used to configure the m4_soc clocks
 * @param[in]   clk_source: Enum values of different Core source clocks. See #M4_SOC_CLK_SRC_SEL_T and NOTE for more info
 * @param[in]   pll_freq: M4 Core Frequency value (in MHz) to set
 * @return 		  returns zero \ref RSI_OK on success, corresponding error code on failure
 * @note        For using UlpRefClk clksource need to configure M4ssRefClk frequency. For that need to call \ref RSI_CLK_M4ssRefClkConfig Api first
 *              -  For using SocPllCLK clksource need to configure SocPll frequency. For that need to call \ref RSI_CLK_SetSocPllFreq Api first
 *							-  For using IntfPllCLK clksource need to configure IntfPll frequency. For that need to call \ref RSI_CLK_SetIntfPllFreq Api first
 *							-  For using Sleep clksource need to configure Sleep Clock. For that need to call \ref RSI_CLK_SlpClkConfig Api first
 */
sl_status_t sl_si91x_clock_manager_m4_set_core_clk(M4_SOC_CLK_SRC_SEL_T clk_source, uint32_t pll_freq)
{
  M4CLK_Type *pCLK         = M4CLK;
  uint32_t div_factor      = DIVISION_FACTOR;
  rsi_error_t error_status = RSI_OK;
  sl_status_t status;

  // PLL reference clock set to XTAL_CLK by default
  uint32_t pll_ref_clk = PLL_REF_CLK_VAL_XTAL;

  // Validating for correct Clock Source input
  if (clk_source > M4_SLEEPCLK) {
    status = SL_STATUS_INVALID_PARAMETER;
    return status;
  }

  // Changing M4SOC clock to M4_ULPREFCLK
  error_status = RSI_CLK_M4SocClkConfig(pCLK, M4_ULPREFCLK, 0);
  status       = convert_rsi_to_sl_error_code(error_status);
  if (status != SL_STATUS_OK) {
    return status;
  }

  if (clk_source == M4_INTFPLLCLK) {
    // RSI API to set INTF PLL clock frequency
    error_status = sl_si91x_clock_manager_set_pll_freq(INFT_PLL, pll_freq, pll_ref_clk);
  } else if (clk_source == M4_SOCPLLCLK) {
    // RSI API to set SOC PLL clock frequency
    error_status = sl_si91x_clock_manager_set_pll_freq(SOC_PLL, pll_freq, pll_ref_clk);
  }

  // The error status is converted to the SL error code
  status = convert_rsi_to_sl_error_code(error_status);
  if (status != SL_STATUS_OK) {
    return status;
  }

  // RSI API to set M4 SOC clock is called and the status is converted to the SL error code.
  error_status = RSI_CLK_M4SocClkConfig(pCLK, clk_source, div_factor);
  status       = convert_rsi_to_sl_error_code(error_status);
  if (status != SL_STATUS_OK) {
    return status;
  }

  return status;
}

/**
 * @fn	        sl_status_t sl_si91x_clock_manager_set_pll_freq(PLL_TYPE_T pll_type, uint32_t pll_freq, uint32_t pll_ref_clk)
 * @brief		    This API is used to set the selected PLL clock to particular frequency
 * @param[in]	  pll_type: PLL type to configure
 * @return 		  returns zero \ref RSI_OK on success, corresponding error code on failure
 */
sl_status_t sl_si91x_clock_manager_set_pll_freq(PLL_TYPE_T pll_type, uint32_t pll_freq, uint32_t pll_ref_clk)
{
  M4CLK_Type *pCLK         = M4CLK;
  rsi_error_t error_status = RSI_OK;
  sl_status_t status;

  switch (pll_type) {
    case SOC_PLL:
      // Configure SOC-PLL lock settings before configuring SOC PLL clock
      RSI_CLK_SocPllLockConfig(MANUAL_LOCK, BYPASS_MANUAL_LOCK, SOC_PLL_MM_COUNT_LIMIT);

      // RSI API to set SOC PLL clock frequency
      error_status = RSI_CLK_SetSocPllFreq(pCLK, pll_freq, pll_ref_clk);
      break;

    case INFT_PLL:
      // RSI API to set INTF PLL clock frequency
      error_status = RSI_CLK_SetIntfPllFreq(pCLK, pll_freq, pll_ref_clk);
      break;

    case I2S_PLL:
      // RSI API to set I2S PLL clock frequency
      error_status = RSI_CLK_SetI2sPllFreq(pCLK, pll_freq, pll_ref_clk);
      break;

    default:
      break;
  }

  status = convert_rsi_to_sl_error_code(error_status);
  return status;
}

/**
 * @fn	        sl_si91x_m4_soc_clk_src_sel_t sl_si91x_clock_manager_m4_get_core_clk_src_freq(uint32_t *core_clock)
 * @brief		    This API is used to read the currently active m4_soc clock source and frequency
 * @param[in]	  core_clock: Pointer to fill core clock frequency
 * @return 		  returns the currently active core clock source of type sl_si91x_m4_soc_clk_src_sel_t
 */
sl_si91x_m4_soc_clk_src_sel_t sl_si91x_clock_manager_m4_get_core_clk_src_freq(uint32_t *core_clock)
{
  M4CLK_Type *pCLK = M4CLK;
  sl_si91x_m4_soc_clk_src_sel_t clk_src;

  // return currently active core clock frequency via the pointer by reference
  *core_clock = system_clocks.soc_clock;

  // read currently active core clock source
  clk_src = pCLK->CLK_CONFIG_REG5_b.M4_SOC_CLK_SEL;

  return clk_src;
}
/*******************************************************************************
 * To validate the RSI error code
 * While calling the RSI APIs, it returns the RSI Error codes.
 * This function converts the RSI error codes into SL error codes.
 * It takes argument as RSI error type and returns the SL error type.
 * It has a single switch statement which is mapped with the SL error code and
 * after successful conversion it breaks the switch statement.
 * If the error code is not listed, by default is SL_STATUS_FAIL.
 ******************************************************************************/
static sl_status_t convert_rsi_to_sl_error_code(rsi_error_t error)
{
  sl_status_t status;
  switch (error) {
    case RSI_OK:
      status = SL_STATUS_OK;
      break;
    case INVALID_PARAMETERS:
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    case ERROR_INVALID_INPUT_FREQUENCY:
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    case ERROR_CLOCK_NOT_ENABLED:
      status = SL_STATUS_NOT_INITIALIZED;
      break;
    case INVALID_SAMPLING_RATE:
      status = SL_STATUS_INVALID_RANGE;
      break;
    case INVALID_SAMPLE_LENGTH:
      status = SL_STATUS_INVALID_COUNT;
      break;
    case ERROR_ADC_INVALID_ARG:
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    case ERROR_PS_INVALID_STATE:
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    default:
      status = SL_STATUS_FAIL;
      break;
  }
  return status;
}
