/* -----------------------------------------------------------------------------
 * Copyright (c) 2013-2015 ARM Ltd.
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software. Permission is granted to anyone to use this
 * software for any purpose, including commercial applications, and to alter
 * it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *
 * $Date:        9. September 2015
 * $Revision:    V1.00
 *
 * Project:      CAN (Controller Area Network) Driver definitions
 * -------------------------------------------------------------------------- */

/* History:
 *  Version 1.0
 *    - Initial CMSIS Driver API V4.5.0 release
 *
 */

/** @defgroup CAN  CAN Peripheral
 * @{
 *
*/

#ifndef __CAN_H
#define __CAN_H

#include "Driver_CAN.h"
#include "rsi_chip.h"
#include "RTE_Device.h"
#include "RTE_Components.h"

#define ARM_CAN_ARBITRATION_LOST (5U) ///< Unit entered arbitration lost error FIXME: added by me

/** @defgroup CANIMRREG  Can interrupt mask register bit fields
 * @{
 */
/****** CAN Interrupt mask bits *****/
#define CAN_IMR_DOIM (1UL << 0) ///< mask for DOI interrupt
#define CAN_IMR_BEIM (1UL << 1) ///< mask for BEI interrupt
#define CAN_IMR_TIM  (1UL << 2) ///< mask for TI interrupt
#define CAN_IMR_RIM  (1UL << 3) ///< mask for RI interrupt
#define CAN_IMR_EPIM (1UL << 4) ///< mask for EPI interrupt
#define CAN_IMR_EWIM (1UL << 5) ///< mask for EWI interrupt
#define CAN_IMR_ALIM (1UL << 6) ///< mask for ALI interrupt
/*
 * @} end of CAN_IMR_REG
 * */

/** @defgroup CAN_ISR_REG:  Can interrupt Status register bit fields
 * \addtogroup CAN_ISR_REG
 * @{
 */
/****** CAN Interrupt status bits *****/
#define CAN_ISR_DO  (1UL << 0) ///< DOI interrupt
#define CAN_ISR_BEI (1UL << 1) ///< BEI interrupt
#define CAN_ISR_TI  (1UL << 2) ///< TI interrupt
#define CAN_ISR_RI  (1UL << 3) ///< RI interrupt
#define CAN_ISR_EPI (1UL << 4) ///< EPI interrupt
#define CAN_ISR_EWI (1UL << 5) ///< EWI interrupt
#define CAN_ISR_ALI (1UL << 6) ///< ALI interrupt
/*
 * @} end of CAN_ISR_REG
 * */

/** @defgroup CAN_SR_REG:  Can  Status register bit fields
 * \addtogroup CAN_SR_REG
 * @{
 */
/****** CAN Interrupt status bits *****/
#define CAN_SR_BS  (1UL << 0) ///< Bus off Status
#define CAN_SR_ES  (1UL << 1) ///< Error Status
#define CAN_SR_TS  (1UL << 2) ///< Transmit Status
#define CAN_SR_RS  (1UL << 3) ///< Receive Status
#define CAN_SR_TBS (1UL << 5) ///< Transmit Buffer Status
#define CAN_SR_DSO (1UL << 6) ///< Data Overrun Status
#define CAN_SR_RBS (1UL << 7) ///< Receive Buffer Status
/*
 * @} end of CAN_SR_REG
 * */

/** @defgroup CAN_MR_REG:  Can mode register bit fields
 * \addtogroup CAN_MR_REG
 * @{
 */
/****** CAN mode control bits *****/
#define CAN_CMR_AT (1UL << 1) ///< Transmit Request
#define CAN_CMR_TR (1UL << 2) ///< Abort Transmission
/*
 * @} end of CAN_MR_REG
 * */

/****** CAN mode configuration codes *****/
typedef enum {
  CAN_HW_RESET_MODE_CONFIG,                           ///< CAN configure in reset mode
  CAN_HW_NORMAL_MODE_CONFIG,                          ///< CAN configure in normal mode
  CAN_HW_LISTEN_ONLY_MODE_CONFIG,                     ///< CAN configure in listen only mode
  CAN_HW_DUAL_FILTER_MODE_CONFIG,                     ///< CAN configure in dual filter mode
  CAN_HW_SINGLE_FILTER_MODE_CONFIG,                   ///< CAN configure in single filter mode
  CAN_HW_ACCEPTANCE_SINGLE_FILTER_MODE_CONFIG_ENABLE, ///< CAN enable configuration in hardware acceptance for single filter mode
  CAN_HW_ACCEPTANCE_DUAL_FILTER_MODE_CONFIG_ENABLE, ///< CAN enable configuration in hardware acceptance for dual filter mode
} CAN_HW_MODE_CONFIG;

/****** CAN filter type configuration codes *****/
typedef enum {
  CAN_FILTER_TYPE_EXACT_ID    = 0U, ///< Add exact id filter
  CAN_FILTER_TYPE_MASKABLE_ID = 1U  ///< Add maskable id filter
} CAN_FILTER_TYPE;

/**
\brief CAN Device Driver pin configurations
*/
typedef struct {
  uint8_t port;    ///< CAN GPIO port
  uint8_t pin;     ///< CAN GPIO pin
  uint8_t mode;    ///< CAN GPIO mode
  uint8_t pad_sel; ///< CAN GPIO pad selection
} CAN_PIN;

#endif /* __CAN_H */
