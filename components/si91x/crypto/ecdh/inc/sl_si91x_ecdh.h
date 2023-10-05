/***************************************************************************/ /**
 * @file
 * @brief SL SI91X ECDH Header file
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

#pragma once
#include "sl_status.h"

/******************************************************
 *                    Constants
 ******************************************************/
/**
 * @addtogroup CRYPTO_ECDH_CONSTANTS
 * @{ 
 */

#define SL_SI91X_ECDH_MAX_VECTOR_SIZE 32 ///< Maximum size of an ECDH vector

typedef enum {
  SL_SI91X_ECDH_192 = 1, ///< ECDH 192 mode
  SL_SI91X_ECDH_224 = 2, ///< ECDH 224 mode
  SL_SI91X_ECDH_256 = 4  ///< ECDH 256 mode
} sl_si91x_ecdh_mode_t;

typedef enum {
  SL_SI91X_ECDH_MUL      = 1, ///< ECDH multiplication mode
  SL_SI91X_ECDH_ADD      = 2, ///< ECDH addition mode
  SL_SI91X_ECDH_SUB      = 3, ///< ECDH subtraction mode
  SL_SI91X_ECDH_DOUBLE   = 4, ///< ECDH double mode
  SL_SI91X_ECDH_AFFINITY = 5  ///< ECDH affinity mode
} sl_si91x_ecdh_sub_mode_t;

typedef enum {
  SL_SI91X_ECDH_VECTOR_SIZE_192 = 24, ///< 192 bits or 24 bytes vector size
  SL_SI91X_ECDH_VECTOR_SIZE_224 = 28, ///< 224 bits or 28 bytes vector size
  SL_SI91X_ECDH_VECTOR_SIZE_256 = 32  ///< 256 bits or 32 bytes vector size
} sl_si91x_ecdh_vector_size_t;

typedef enum {
  SL_SI91X_ECDH_CURVE_P = 0, ///< Prime Field Curves
  SL_SI91X_ECDH_CURVE_K = 1, ///< Binary Field Curves
  SL_SI91X_ECDH_CURVE_B = 2, ///< Edwards Curves
} sl_si91x_ecdh_curve_type_t;

/** @} */

/**
 * @addtogroup CRYPTO_ECDH_FUNCTIONS 
 * @{ 
 */

/***************************************************************************/ /**
 * @brief      Compute the ECDH point addition vector. This is a blocking API.
 * @param[in]  ecdh_mode 
 *   ECDH mode of type @ref sl_si91x_ecdh_mode_t 
 * @param[in]  sx 
 *   Pointer to x coordinate of the point1 that needs to be added
 * @param[in]  sy 
 *   Pointer to y coordinate of the point1 that needs to be added
 * @param[in]  sz 
 *   Pointer to z coordinate of the point1 that needs to be added
 * @param[in]  tx 
 *   Pointer to x coordinate of the point2 that needs to be added
 * @param[in]  ty 
 *   Pointer to y coordinate of the point2 that needs to be added
 * @param[in]  tz 
 *   Pointer to z coordinate of the point2 that needs to be added
 * @param[out] rx 
 *   Pointer to x coordinate of the result point
 * @param[out] ry 
 *   Pointer to y coordinate of the result point
 * @param[out] rz 
 *   Pointer to z coordinate of the result point
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
******************************************************************************/
sl_status_t sl_si91x_ecdh_point_addition(sl_si91x_ecdh_mode_t ecdh_mode,
                                         uint8_t *sx,
                                         uint8_t *sy,
                                         uint8_t *sz,
                                         uint8_t *tx,
                                         uint8_t *ty,
                                         uint8_t *tz,
                                         uint8_t *rx,
                                         uint8_t *ry,
                                         uint8_t *rz);

/***************************************************************************/ /**
 * @brief      Compute the ECDH point subtraction vector. This is a blocking API.
 * @param[in]  ecdh_mode 
 *   ECDH mode of type @ref sl_si91x_ecdh_mode_t 
 * @param[in]  sx 
 *   Pointer to x coordinate of the point1 that needs to be subtracted
 * @param[in]  sy 
 *   Pointer to y coordinate of the point1 that needs to be subtracted
 * @param[in]  sz 
 *   Pointer to z coordinate of the point1 that needs to be subtracted
 * @param[in]  tx 
 *   Pointer to x coordinate of the point2 that needs to be subtracted
 * @param[in]  ty 
 *   Pointer to y coordinate of the point2 that needs to be subtracted
 * @param[in]  tz 
 *   Pointer to z coordinate of the point2 that needs to be subtracted
 * @param[out] rx 
 *   Pointer to x coordinate of the result point
 * @param[out] ry 
 *   Pointer to y coordinate of the result point
 * @param[out] rz 
 *   Pointer to z coordinate of the result point
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
******************************************************************************/
sl_status_t sl_si91x_ecdh_point_subtraction(sl_si91x_ecdh_mode_t ecdh_mode,
                                            uint8_t *sx,
                                            uint8_t *sy,
                                            uint8_t *sz,
                                            uint8_t *tx,
                                            uint8_t *ty,
                                            uint8_t *tz,
                                            uint8_t *rx,
                                            uint8_t *ry,
                                            uint8_t *rz);

/***************************************************************************/ /**
 * @brief      
 *   Compute the ECDH point multiplication vector. This is a blocking API.
 * @param[in]  ecdh_mode
 *   ECDH mode of type @ref sl_si91x_ecdh_mode_t 
 * @param[in]  d         
 *   Pointer to scalar value that needs to be multiplied
 * @param[in]  sx        
 *   Pointer to x coordinate of the point to be multiplied with scalar ‘d’
 * @param[in]  sy        
 *   Pointer to y coordinate of the point to be multiplied with scalar ‘d’
 * @param[in]  sz        
 *   Pointer to z coordinate of the point to be multiplied with scalar ‘d’
 * @param[in]  affinity  
 *   0 : no affinity \n 1 : affinity on input \n 2 : affinity on output \n 3 : affinity on both input and output \n
 * @param[out] rx        
 *   Pointer to x coordinate of the result point
 * @param[out] ry        
 *   Pointer to  y coordinate of the result point
 * @param[out] rz        
 *   Pointer to z coordinate of the result point
 * @param[in]  reverse   
 *   Setting this will perform \ref reverse_8 on the inputs and outputs.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
******************************************************************************/
sl_status_t sl_si91x_ecdh_point_multiplication(sl_si91x_ecdh_mode_t ecdh_mode,
                                               uint8_t *d,
                                               uint8_t *sx,
                                               uint8_t *sy,
                                               uint8_t *sz,
                                               uint32_t affinity,
                                               uint8_t *rx,
                                               uint8_t *ry,
                                               uint8_t *rz,
                                               uint8_t reverse);

/***************************************************************************/ /**
 * @brief
 *   Compute the ECDH point double vector. This is a blocking API.
 * @param[in]  ecdh_mode 
 *   ECDH mode of type @ref sl_si91x_ecdh_mode_t 
 * @param[in]  sx 
 *   Pointer to x coordinate of the point1 that needs to be doubled
 * @param[in]  sy 
 *   Pointer to y coordinate of the point1 that needs to be doubled
 * @param[in]  sz 
 *   Pointer to z coordinate of the point1 that needs to be doubled
 * @param[out] rx 
 *   Pointer to x coordinate of the result point
 * @param[out] ry 
 *   Pointer to y coordinate of the result point
 * @param[out] rz 
 *   Pointer to z coordinate of the result point
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
******************************************************************************/
sl_status_t sl_si91x_ecdh_point_double(sl_si91x_ecdh_mode_t ecdh_mode,
                                       uint8_t *sx,
                                       uint8_t *sy,
                                       uint8_t *sz,
                                       uint8_t *rx,
                                       uint8_t *ry,
                                       uint8_t *rz);

/***************************************************************************/ /**
 * @brief      Compute the ECDH point affinity vector. This is a blocking API.
 * @param[in]  ecdh_mode 
 *   ECDH mode of type @ref sl_si91x_ecdh_mode_t 
 * @param[in]  sx 
 *   Pointer to x coordinate of the point1
 * @param[in]  sy 
 *   Pointer to y coordinate of the point1
 * @param[in]  sz 
 *   Pointer to z coordinate of the point1
 * @param[out] rx 
 *   Pointer to x coordinate of the result point
 * @param[out] ry 
 *   Pointer to y coordinate of the result point
 * @param[out] rz 
 *   Pointer to z coordinate of the result point
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
******************************************************************************/
sl_status_t sl_si91x_ecdh_point_affine(sl_si91x_ecdh_mode_t ecdh_mode,
                                       uint8_t *sx,
                                       uint8_t *sy,
                                       uint8_t *sz,
                                       uint8_t *rx,
                                       uint8_t *ry,
                                       uint8_t *rz);

/** @} */
