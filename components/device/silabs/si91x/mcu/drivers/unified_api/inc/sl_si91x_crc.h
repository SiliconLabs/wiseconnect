/***************************************************************************/
/**
 * @file sl_si91x_crc.h
 * @brief CRC API implementation
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

#ifndef SL_SI91X_CRC_H_
#define SL_SI91X_CRC_H_
#include <stdint.h>
#include "rsi_crc.h"
#include "sl_status.h"
#include "data_types.h"

/***************************************************************************/ /**
 * @addtogroup CRC Cyclic Redundancy Check
 * @ingroup SI91X_PERIPHERAL_APIS
 * @{
 *
 ******************************************************************************/

/**
 * @brief Renamed CRC param structure
 */
typedef RSI_CRC_PARAMS_T sl_crc_params_t;

/**
 * @brief Polynomial used for CRC calculation.
 */
#define SL_CRC_POLYNOMIAL 0x04C11DB7

/**
 * @brief Width of the polynomial.
 */
#define SL_CRC_POLY_WIDTH 31

/**
 * @brief Initial LFSR value.
 */
#define SL_CRC_LFSR_VAL 0x0000

/**
 * @brief Width type of the CRC.
 */
#define SL_CRC_WIDTH_TYPE 0

/**
 * @brief Width of the input data.
 */
#define SL_CRC_DIN_WIDTH 31

/**
 * @brief Number of bytes to be written to the CRC.
 */
#define SL_CRC_NUM_BYTES 3

/**
 * @brief Almost empty threshold level.
 */
#define SL_CRC_AEMPTY 6

/**
 * @brief Almost full threshold level.
 */
#define SL_CRC_AFULL 2

/**
 * @brief Whether to swap the input data.
 */
#define SL_CRC_SWAP_DIN 0

/**
 * @brief Whether to use uDMA for CRC calculation.
 */
#define SL_CRC_USE_UDMA 1

/**
 * @brief Whether to swap the LFSR value.
 */
#define SL_CRC_SWAP_LFSR 1

/*******************************************************************************
 * @fn           sl_status_t sl_si91x_crc_set_gen_control(void)
 * @brief        This API is used to clears the FIFO and settles all the
 * state machines to their IDLE.
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid.
 *******************************************************************************/
sl_status_t sl_si91x_crc_set_gen_control(void);

/*******************************************************************************
 * @fn           sl_status_t sl_si91x_crc_get_general_status(  uint32_t* reg_status)
 * @brief        This API is used to get the general status of the crc signals.
 * @param [out] reg_status: Will provide the status of the GEN_STS Register.
 * @return
 * @return API has below possible returns
 *       - SL_STATUS_OK (0x0000) - Success
 *       - SL_STATUS_NULL_POINTER (0x0021) - Invalid null pointer
 *         received as argument.
 *******************************************************************************/
sl_status_t sl_si91x_crc_get_general_status(uint32_t *reg_status);

/*******************************************************************************
 * @fn           sl_status_t sl_si91x_crc_polynomial(sl_crc_params_t *pCRCParams) {
 * @brief        This API is used to load the polynomial value into the
 *polynomial reg.
 * @param[in]    pCRCParams : Pointer to the CRC Parameters structure.
 *                            The possible variable is polynomial.
 * @return API has below possible returns
 *       - SL_STATUS_OK (0x0000) - Success
 *       - SL_STATUS_NULL_POINTER (0x0021) - Invalid null pointer
 *         received as argument.
 *******************************************************************************/
sl_status_t sl_si91x_crc_polynomial(sl_crc_params_t *pCRCParams);

/*******************************************************************************
 * @fn           sl_status_t sl_si91x_crc_polynomial_width(
 *                                         sl_crc_params_t *pCRCParams,
 *                                          uint32_t* reg_status)
 * @brief        This API is used to load the width of the polynomail .Number of
 *bits/width of the polynomial has to be written here for the computation of
 *final CRC. If a new width has to be configured, clear the existing length
 *first by writing 0x1f in polynomial_ctrl_reset register.
 * @param[in]    pCRCParams : Pointer to the CRC Parameters structure.
 *                            The possible variable is Poly_width.
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid.
 *******************************************************************************/
sl_status_t sl_si91x_crc_polynomial_width(sl_crc_params_t *pCRCParams, uint32_t *reg_status);

/*******************************************************************************
 * @fn           sl_status_t sl_si91x_crc_lfsr_init( sl_crc_params_t *pCRCParams)
 * @brief        This API is used to initialize the  LFSR Value. When ever the
 * LFSR need to be Initilized this has to be updated with the Init value.
 * @param[in]    pCRCParams : Pointer to the CRC Parameters structure.
 *                            The possible variable is lfsr_val.
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid.
 ********************************************************************************/
sl_status_t sl_si91x_crc_lfsr_init(sl_crc_params_t *pCRCParams);

/*******************************************************************************
 * @fn           sl_status_t sl_si91x_crc_use_swapped_init(
 *                                          sl_crc_params_t *pCRCParams,
 *                                          uint32_t* reg_status)
 * @brief        This API is used for swapped init value. If this is set bit
 *swapped version of LFSR init value will be loaded / initialized to LFSR state.
 * @param[in]    pCRCParams : Pointer to the CRC Parameters structure.
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid.
 *******************************************************************************/
sl_status_t sl_si91x_crc_use_swapped_init(sl_crc_params_t *pCRCParams, uint32_t *reg_status);

/*******************************************************************************
 * @fn           sl_status_t sl_si91x_crc_set_data_width_type(
 *                                            sl_crc_params_t *pCRCParams,
 *                                            uint32_t oper_status)
 * @brief        This API is used to set and control the data width types.
 * @param[in]    pCRCParams : Pointer to the CRC Parameters structure.
 *                The possible parameter is num_bytes. When only the data width
 * type is DIN_WIDTH_FROM_CNT then only uses this variable.
 * @param[out]    oper_status : Will give the status of the API process.
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid.
 *******************************************************************************/
sl_status_t sl_si91x_crc_set_data_width_type(sl_crc_params_t *pCRCParams, uint32_t *oper_status);

/*******************************************************************************
 * @fn           sl_status_t sl_si91x_crc_set_fifo_thresholds(
 *                                            sl_crc_params_t *pCRCParams,
 *                                            uint32_t oper_status)
 * @brief        This API is used to set the FIFO Threshold Levels.
 * @param[in]    pCRCParams : Pointer to the CRC Parameters structure.
 *               The possible parameters are aempty and afull.
 *               Max value is 15 for both the threshold levels.
 * @param[out]    oper_status : Will give the status of the API process.
 * @return       return \ref RSI_OK on success and failure return error code.*
 *******************************************************************************/
sl_status_t sl_si91x_crc_set_fifo_thresholds(sl_crc_params_t *pCRCParams, uint32_t *oper_status);

/*******************************************************************************
 * @fn           sl_status_t sl_si91x_crc_write_data(
 *                                   sl_crc_params_t *pCRCParams,
 *                                   uint32_t data,
 *                                   uint32_t oper_status)
 * @brief        This API is used to set the Input data Information for CRC
 *Calculation.
 * @param[in]    pCRCParams : Pointer to the CRC Parameters structure.
 *               The possible parameters are aempty and afull.
 * @param[out]    oper_status : Will give the status of the API process.
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid.
*******************************************************************************/
sl_status_t sl_si91x_crc_write_data(sl_crc_params_t *pCRCParams, uint32_t data, uint32_t *oper_status);

/*******************************************************************************
 * @fn           sl_status_t sl_si91x_crc_monitor_crc_calc(
 *                                         sl_crc_params_t *pCRCParams,
 *                                         uint32_t crc)
 * @brief        This API is used to monitor the CRC Caluclation status and the
 *returns the CRC Value.
 * @param[in]    pCRCParams : Pointer to the CRC Parameters structure.
 *                          The possible parameter is width_type. This Params
 *always 2 when we want Max value is 15 for both the threshold levels.
 * @return       CRC Value to be returned.
 *******************************************************************************/
sl_status_t sl_si91x_crc_monitor_crc_calc(sl_crc_params_t *pCRCParams, uint32_t *crc);

/*******************************************************************************
 * @fn           void sl_si91x_crc_lfsr_dynamic_write( sl_crc_params_t *pCRCParams )
 * @brief        This API can be used for writing the LFSR state directly.
 * @param[in]    pCRCParams - Pointer to the CRC Parameters structure.
                 The possible variable is lfsrState.
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid.
 *******************************************************************************/
sl_status_t sl_si91x_crc_lfsr_dynamic_write(sl_crc_params_t *pCRCParams);

/*******************************************************************************
 * @fn           sl_status_t sl_si91x_crc_reset_fifo(void)
 * @brief        This API is used to Reset fifo pointer. This clears the
 *FIFO.When this is set
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid.
 *******************************************************************************/
sl_status_t sl_si91x_crc_reset_fifo(void);

/*******************************************************************************
 * @fn           sl_status_t sl_si91x_crc_get_fifo_status(void)
 * @brief        This API is used to get the fifo status of the crc occupancy.
 * @param reg_status: Will give the status of the FIFO status register.
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid.
 *******************************************************************************/
sl_status_t sl_si91x_crc_get_fifo_status(uint32_t *reg_status);
/** ****************************************************************************
 *
 * @brief Enables the CRC peripheral.
 *
 * This function enables the CRC peripheral by enabling its clock.
 *
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid.
 * *******************************************************************************/
sl_status_t sl_si91x_crc_enable(void);

/** ****************************************************************************
 *
 * @brief Disable the CRC peripheral.
 *
 * This function Disables the CRC peripheral by disabling its clock.
 *
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid.
*******************************************************************************/

sl_status_t sl_si91x_crc_disable(void);

#endif /* SL_SI91X_CRC_H_ */

// ******** THE REST OF THE FILE IS DOCUMENTATION ONLY !***********************
/// @addtogroup CRC
/// @{
///   @details
///
///   @n @section CRC_Intro Introduction
///   The Cyclic Redundancy Check (CRC) in embedded systems identifies inadvertent data changes.
///   It generates a checksum, which is applied to the data before transmission or storage.
///   When data is received or retrieved, the CRC is recalculated and compared to the original checksum to confirm its integrity.
///
///   @n @section CRC_Config Configuration
///  Configuring CRC typically involves several parameters like below:
/// @li Modify `SL_CRC_POLYNOMIAL` to set the desired polynomial value.
/// @li Modify `SL_CRC_POLY_WIDTH` to set the desired polynomial width.
///
///   @n @section CRC_Usage Usage
///
/// The common CRC functions can be used after the CRC Structures are specified, passing an instance of
///   @ref sl_crc_params_t. The following functions will initiate and configure the CRC below, which is the flow for implementation.
///
///   1. @ref sl_si91x_crc_set_gen_control
///   2. @ref sl_si91x_crc_enable
///   3. @ref sl_si91x_crc_polynomial
///   4. @ref sl_si91x_crc_polynomial_width
///   5. @ref sl_si91x_crc_lfsr_init
///   6. @ref sl_si91x_crc_use_swapped_init
///   7. @ref sl_si91x_crc_set_data_width_type
///   8. @ref sl_si91x_crc_set_fifo_thresholds
///   9. @ref sl_si91x_crc_monitor_crc_calc
///   10. @ref sl_si91x_crc_disable
///
/** @} (end addtogroup CRC) */
