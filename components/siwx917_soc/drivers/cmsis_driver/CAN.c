/* -------------------------------------------------------------------------- 
 * Copyright (c) 2013-2016 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * $Date:        02. March 2016
 * $Revision:    V1.1
 *
 * Driver:       Driver_CAN1
 * Configured:   via RTE_Device.h configuration file
 * Project:      CAN Driver for Silicon Labs MCU
 * --------------------------------------------------------------------------
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 *
 *   Configuration Setting                 Value   CAN Interface
 *   ---------------------                 -----   -------------
 *   Connect to hardware via Driver_CAN# = 1       use CAN1
 * --------------------------------------------------------------------------
 * Defines used for driver configuration (at compile time):
 *
 *   CAN_CLOCK_TOLERANCE:  defines maximum allowed clock tolerance in 1/1024 steps
 *     - default value:    15 (approx. 1.5 %)
 *   CAN0_OBJ_NUM:         number of message objects used by CAN0 controller
 *                         this value can be reduced to save some RAM space
 *     - default value:    2 (also this is maximum value)
 *   CAN1_OBJ_NUM:         number of message objects used by CAN1 controller
 *                         this value can be reduced to save some RAM space
 *     - default value:    2 (also this is maximum value)
 * -------------------------------------------------------------------------- */

/* History:
 *  Version 1.0
 *    Initial release
 */
#include "rsi_ccp_user_config.h"
#if defined(CHIP_9118)
#include "CAN.h"
#if RTE_CAN1
// Externally overridable configuration definitions

// Maximum allowed clock tolerance in 1/1024 steps
#ifndef CAN_CLOCK_TOLERANCE
#define CAN_CLOCK_TOLERANCE (15U) ///< 15/1024 approx. 1.5 %
#endif

// Maximum number of Message Objects used for CAN1 controller
#ifndef CAN1_OBJ_NUM
#define CAN1_OBJ_NUM (2U) ///< Number of CAN objects
#endif
#if (CAN1_OBJ_NUM > 2U)
#error Too many Message Objects defined for CAN1, maximum number of Message Objects is 32 !!!
#endif

#define CAN_IRQ_NUM (CAN1_IRQn) ///< CAN IRQ number

// CAN Driver ******************************************************************

#define ARM_CAN_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1, 1) ///< CAN driver version

// Driver Version
static const ARM_DRIVER_VERSION can_driver_version = { ARM_CAN_API_VERSION, ARM_CAN_DRV_VERSION };

// Driver Capabilities
static const ARM_CAN_CAPABILITIES can_driver_capabilities = {
  CAN1_OBJ_NUM, ///< Number of CAN Objects available
  1U, // Supports reentrant calls to ARM_CAN_MessageSend, ARM_CAN_MessageRead, ARM_CAN_ObjectConfigure and abort message sending used by ARM_CAN_Control.
  0U, // Does not support CAN with Flexible Data-rate mode (CAN_FD)
  0U, // Does not support restricted operation mode
  1U, // Supports bus monitoring mode
  0U, // Supports internal loopback mode
  0U, // Supports external loopback mode
};

// Object Capabilities
static const ARM_CAN_OBJ_CAPABILITIES can_object_capabilities_rx = {
  0U, // Object does not support transmission
  1U, // Object supports reception
  0U, // Object does not support RTR reception and automatic Data transmission
  0U, // Object does not support RTR transmission and automatic Data reception
  0U, // Object allows assignment of multiple filters to it
  1U, // Object supports exact identifier filtering
  1U, // Object supports range identifier filtering
  1U, // Object does not support mask identifier filtering
  5U  // Object can buffer 5 messages
};
static const ARM_CAN_OBJ_CAPABILITIES can_object_capabilities_tx = {
  1U, // Object supports transmission
  0U, // Object does not support reception
  0U, // Object does not support RTR reception and automatic Data transmission
  0U, // Object does not support RTR transmission and automatic Data reception
  0U, // Object does not allow assignment of multiple filters to it
  0U, // Object does not support exact identifier filtering
  0U, // Object does not support range identifier filtering
  0U, // Object does not support mask identifier filtering
  1U  // Object can buffer 1 message
};

// Local variables and structures
static uint8_t can_driver_powered                        = 0U;
static uint8_t can_driver_initialized                    = 0U;
static ARM_CAN_SignalUnitEvent_t CAN_SignalUnitEvent     = NULL;
static ARM_CAN_SignalObjectEvent_t CAN_SignalObjectEvent = NULL;
static uint8_t can_obj_cfg_msk                           = 0U;
static uint8_t can_last_error_code                       = 0U;
/* CAN PIN configuration structure*/
static const CAN_PIN can_pins[] = {
  { RTE_CAN1_TX_PORT, RTE_CAN1_TX_PIN, RTE_CAN1_TX_MODE, RTE_CAN1_TX_PAD_SEL },
  { RTE_CAN1_RX_PORT, RTE_CAN1_RX_PIN, RTE_CAN1_RX_MODE, RTE_CAN1_TX_PAD_SEL },
};

// Helper Functions

/**
  @fn          void CAN_SetHwModeConfig(CAN_HW_MODE_CONFIG mode)
  @brief       Configures the CAN hardware mode
  @param[in]   mode  CAN operational mode
                 - \ref CAN_HW_RESET_MODE_CONFIG    : CAN configure in reset mode
                 - \ref CAN_HW_NORMAL_MODE_CONFIG   : CAN configure in normal mode
                 - \ref CAN_HW_LISTEN_ONLY_MODE_CONFIG : CAN configure in listen only mode
                 - \ref CAN_HW_DUAL_FILTER_MODE_CONFIG : CAN configure in dual filter mode
                 - \ref CAN_HW_SINGLE_FILTER_MODE_CONFIG : CAN configure in single filter mode
                 - \ref CAN_HW_ACCEPTANCE_SINGLE_FILTER_MODE_CONFIG_ENABLE : CAN enable configuration in hardware acceptance for single filter mode
                 - \ref CAN_HW_ACCEPTANCE_DUAL_FILTER_MODE_CONFIG_ENABLE   : CAN enable configuration in hardware acceptance for dual filter mode
   @return      none
*/
static void CAN_SetHwModeConfig(CAN_HW_MODE_CONFIG mode)
{
  switch (mode) {
    case CAN_HW_RESET_MODE_CONFIG: // Keep DCAN in RESET mode
      CAN1->CAN_MR = 0x04;
      break;
    case CAN_HW_NORMAL_MODE_CONFIG: // Keep DCAN in RESET mode
      CAN1->CAN_MR = 0x04;
      CAN1->CAN_MR = 0x00;
      break;
    case CAN_HW_LISTEN_ONLY_MODE_CONFIG: // Keep CAN in soft RESET operating mode to get the write access to AFM bit
      CAN1->CAN_MR = 0x04;
      CAN1->CAN_MR = 0x02; // Set standard single filter mode
      break;
    case CAN_HW_DUAL_FILTER_MODE_CONFIG: // Dual filter mode configuration set
      CAN1->CAN_MR = 0x04;
      break;
    case CAN_HW_SINGLE_FILTER_MODE_CONFIG: //Single filter mode configuration set
      CAN1->CAN_MR = 0x04;                 // Set CAN in RESET mode
      CAN1->CAN_MR = 0x05;                 // Set standard single filter mode
      break;
    case CAN_HW_ACCEPTANCE_SINGLE_FILTER_MODE_CONFIG_ENABLE: // Enable single filter mode configuration for filters
      CAN1->CAN_MR = 0x01;
      break;
    case CAN_HW_ACCEPTANCE_DUAL_FILTER_MODE_CONFIG_ENABLE: // Enable dual filter mode configuration for filters
      CAN1->CAN_MR = 0x00;
      break;
    default:
      break;
  }
}

/**
  @fn          int32_t CAN_AddHwFilter (CAN_FILTER_TYPE filter_type, uint32_t id, uint32_t mask) {
  @brief       Add filter for message reception.
  @param[in]   filter_type  Operation on filter
                 - \ref CAN_FILTER_TYPE_EXACT_ID    :    add    exact id filter
                 - \ref CAN_FILTER_TYPE_MASKABLE_ID :    add maskable id  filter
  @param[in]   id      ID or start of ID range (depending on filter type)
  @param[in]   mask    Mask or end of ID range (depending on filter type)
  @return      execution status
*/
static int32_t CAN_AddHwFilter(CAN_FILTER_TYPE filter_type, uint32_t id, uint32_t mask)
{
  if ((id & ARM_CAN_ID_IDE_Msk) == 0U) { // Standard Identifier (11 bit)
    switch (filter_type) {
      case CAN_FILTER_TYPE_EXACT_ID:
        CAN_SetHwModeConfig(CAN_HW_SINGLE_FILTER_MODE_CONFIG); //Keep CAN in filter configuration mode
        CAN1->CAN_ACR        = 0x00;                           //Reset defaults
        CAN1->CAN_AMR        = 0x00;
        CAN1->CAN_ACR_b.ACR0 = ((id >> 3) & 0xFF);
        CAN1->CAN_ACR_b.ACR1 = ((((id >> 0) & 0x07) << 5) | BIT(4));
        CAN1->CAN_ACR_b.ACR2 = 0x00;
        CAN1->CAN_ACR_b.ACR3 = 0x00;
        CAN1->CAN_AMR_b.AMR0 = 0x00;
        CAN1->CAN_AMR_b.AMR1 = BIT(4); // Mark RTR bit as don't care
        CAN1->CAN_AMR_b.AMR2 = 0xFF;
        CAN1->CAN_AMR_b.AMR3 = 0xFF;
        // Enable hardware filter scheme
        CAN_SetHwModeConfig(CAN_HW_ACCEPTANCE_SINGLE_FILTER_MODE_CONFIG_ENABLE);
        break;

      case CAN_FILTER_TYPE_MASKABLE_ID: // Add code to setup peripheral to receive messages with specified maskable ID
        CAN_SetHwModeConfig(CAN_HW_SINGLE_FILTER_MODE_CONFIG); //Keep CAN in filter configuration mode
        CAN1->CAN_ACR        = 0x00;                           //Reset defaults
        CAN1->CAN_AMR        = 0x00;
        CAN1->CAN_ACR_b.ACR0 = ((id >> 3) & 0xFF);
        CAN1->CAN_ACR_b.ACR1 = ((((id >> 0) & 0x07) << 5) | BIT(4));
        CAN1->CAN_ACR_b.ACR2 = 0x00;
        CAN1->CAN_ACR_b.ACR3 = 0x00;
        CAN1->CAN_AMR_b.AMR0 = ((mask >> 3) & 0xFF); //Configure the mask bits
        CAN1->CAN_AMR_b.AMR1 = ((((mask >> 0) & 0x07) << 5) | BIT(4));
        CAN1->CAN_AMR_b.AMR2 = 0xFF;
        CAN1->CAN_AMR_b.AMR3 = 0xFF;
        CAN_SetHwModeConfig(CAN_HW_ACCEPTANCE_SINGLE_FILTER_MODE_CONFIG_ENABLE); // Enable hardware filter scheme
        break;
      default:
        // Handle unknown operation code
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
  } else { // Extended Identifier (29 bit)
    switch (filter_type) {
      case CAN_FILTER_TYPE_EXACT_ID:
        // Add code to setup peripheral to receive messages with specified exact ID
        CAN_SetHwModeConfig(CAN_HW_SINGLE_FILTER_MODE_CONFIG);
        //Reset defaults
        CAN1->CAN_ACR        = 0x00;
        CAN1->CAN_AMR        = 0x00;
        CAN1->CAN_ACR_b.ACR0 = ((id >> 21) & 0xFF);
        CAN1->CAN_ACR_b.ACR1 = ((id >> 13) & 0xFF);
        CAN1->CAN_ACR_b.ACR2 = ((id >> 5) & 0xFF);
        CAN1->CAN_ACR_b.ACR3 = (((id >> 0) & 0xFF) << 3);
        CAN1->CAN_ACR_b.ACR3 |= BIT(2);
        CAN1->CAN_AMR_b.AMR3 = BIT(2);
        /*Apply filter logic here */
        CAN_SetHwModeConfig(CAN_HW_ACCEPTANCE_SINGLE_FILTER_MODE_CONFIG_ENABLE);
        break;

      case CAN_FILTER_TYPE_MASKABLE_ID:
        // Add code to setup peripheral to receive messages with specified maskable ID
        CAN_SetHwModeConfig(CAN_HW_SINGLE_FILTER_MODE_CONFIG);
        //Reset defaults
        CAN1->CAN_ACR        = 0x00;
        CAN1->CAN_AMR        = 0x00;
        CAN1->CAN_ACR_b.ACR0 = ((id >> 21) & 0xFF);
        CAN1->CAN_ACR_b.ACR1 = ((id >> 13) & 0xFF);
        CAN1->CAN_ACR_b.ACR2 = ((id >> 5) & 0xFF);
        CAN1->CAN_ACR_b.ACR3 = (((id >> 0) & 0xFF) << 3);
        CAN1->CAN_ACR_b.ACR3 |= BIT(2);
        //Configure the mask bits
        CAN1->CAN_AMR_b.AMR0 = ((mask >> 21) & 0xFF);
        CAN1->CAN_AMR_b.AMR1 = ((mask >> 13) & 0xFF);
        CAN1->CAN_AMR_b.AMR2 = ((mask >> 5) & 0xFF);
        CAN1->CAN_AMR_b.AMR3 = (((mask >> 0) & 0xFF) << 3);
        CAN1->CAN_AMR_b.AMR3 |= BIT(2);

        /*Apply filter logic here */
        CAN_SetHwModeConfig(CAN_HW_ACCEPTANCE_SINGLE_FILTER_MODE_CONFIG_ENABLE);
        break;
      default:
        // Handle unknown operation code
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
  }
  return ARM_DRIVER_OK;
}

/**
  @fn          int32_t CAN_RemoveFilter (CAN_FILTER_TYPE filter_type, uint32_t id, uint32_t mask) {
  @brief       Add filter for message reception.
  @param[in]   filter_type  Operation on filter
                 - \ref CAN_FILTER_TYPE_EXACT_ID :       remove exact id filter
                 - \ref CAN_FILTER_TYPE_MASKABLE_ID :    remove maskable id  filter
  @param[in]   id       ID or start of ID range (depending on filter type)
  @param[in]   mask     Mask or end of ID range (depending on filter type)
   @return     execution status
*/
static int32_t CAN_RemoveFilter(CAN_FILTER_TYPE filter_type, uint32_t id, uint32_t mask)
{
  switch (filter_type) {
    case CAN_FILTER_TYPE_EXACT_ID:
    case CAN_FILTER_TYPE_MASKABLE_ID:
      //Keep CAN in filter configuration mode
      CAN_SetHwModeConfig(CAN_HW_SINGLE_FILTER_MODE_CONFIG);
      CAN1->CAN_ACR = 0xFFFFFFFF;
      CAN1->CAN_AMR = 0xFFFFFFFF;
      // Enable hardware filter scheme
      CAN_SetHwModeConfig(CAN_HW_ACCEPTANCE_SINGLE_FILTER_MODE_CONFIG_ENABLE);
      break;
    default:
      // Handle unknown operation code
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  return ARM_DRIVER_OK;
}

// CAN Driver Functions

/**
  @fn          ARM_DRIVER_VERSION CAN_GetVersion (void)
  @brief       Get driver version.
  @return      \ref ARM_DRIVER_VERSION
*/
static ARM_DRIVER_VERSION CAN_GetVersion(void)
{
  // Return driver version
  return can_driver_version;
}

/**
  @fn          ARM_CAN_CAPABILITIES CAN_GetCapabilities (void)
  @brief       Get driver capabilities.
  @return      \ref ARM_CAN_CAPABILITIES
*/
static ARM_CAN_CAPABILITIES CAN_GetCapabilities(void)
{
  // Return driver capabilities
  return can_driver_capabilities;
}

/**
  @fn          int32_t CAN_Initialize (ARM_CAN_SignalUnitEvent_t   cb_unit_event,
                                        ARM_CAN_SignalObjectEvent_t cb_object_event)
  @brief       Initialize CAN interface and register signal (callback) functions.
  @param[in]   cb_unit_event   Pointer to \ref ARM_CAN_SignalUnitEvent_t callback function
  @param[in]   cb_object_event Pointer to \ref ARM_CAN_SignalObjectEvent_t callback function
  @return      execution status
*/
static int32_t CAN_Initialize(ARM_CAN_SignalUnitEvent_t cb_unit_event, ARM_CAN_SignalObjectEvent_t cb_object_event)
{

  const CAN_PIN *io;

  if (MCU_RET->CHIP_CONFIG_MCU_READ_b.DISABLE_CAN_INTERFACE == 1U) {
    return ARM_DRIVER_ERROR_UNSUPPORTED; // If CAN peripheral is not supported by this chip
  }

  if (can_driver_initialized != 0U) {
    return ARM_DRIVER_OK;
  }

  for (io = can_pins; io != &can_pins[sizeof(can_pins) / sizeof(CAN_PIN)]; io++) {
    if (io->pin > 63) {
      RSI_EGPIO_SetPinMux(EGPIO1, io->port, (io->pin - 64), 6);
      RSI_EGPIO_UlpPadReceiverEnable((io->pin) - 64);
    } else {
      RSI_EGPIO_PadReceiverEnable(io->pin);
    }
    RSI_EGPIO_SetPinMux(EGPIO, io->port, io->pin, io->mode);
    RSI_EGPIO_PadSelectionEnable(io->pad_sel);
  }

  CAN_SignalUnitEvent   = cb_unit_event;
  CAN_SignalObjectEvent = cb_object_event;

  can_driver_initialized = 1U;

  return ARM_DRIVER_OK;
}

/**
  @fn          int32_t CAN_Uninitialize (void)
  @brief       De-initialize CAN interface.
  @return      execution status
*/
static int32_t CAN_Uninitialize(void)
{
  const CAN_PIN *io;

  for (io = can_pins; io != &can_pins[sizeof(can_pins) / sizeof(CAN_PIN)]; io++) {
    RSI_EGPIO_SetPinMux(EGPIO, io->port, io->pin, EGPIO_PIN_MUX_MODE0);
    RSI_EGPIO_PadReceiverEnable(io->pin);
    if (io->pin < 63) {
      RSI_EGPIO_PadSelectionEnable(io->pad_sel);
    } else {
      RSI_EGPIO_SetPinMux(EGPIO1, (io->port), (io->pin - 64U), 6U);
    }
  }
  can_driver_initialized = 0U;
  return ARM_DRIVER_OK;
}

/**
  @fn          int32_t CAN_PowerControl (ARM_POWER_STATE state)
  @brief       Control CAN interface power.
  @param[in]   state  Power state
                 - \ref ARM_POWER_OFF  : power off     : no operation possible
                 - \ref ARM_POWER_LOW  : low power mode: retain state, detect and signal wake-up events
                 - \ref ARM_POWER_FULL : power on      : full operation at maximum performance
  @return      execution status
*/
static int32_t CAN_PowerControl(ARM_POWER_STATE state)
{
  switch (state) {
    case ARM_POWER_OFF:
      can_driver_powered = 0U;
      NVIC_DisableIRQ(CAN_IRQ_NUM); // Disable CAN NVIC
      CAN1->CAN_IMR = 0U;           // Disable all CAN controller interrupts
      // Clear bit rate
      CAN1->CAN_BTIM0_b.BRP   = 0U;
      CAN1->CAN_BTIM0_b.SJW   = 0U;
      CAN1->CAN_BTIM1_b.TSEG1 = 0U;
      CAN1->CAN_BTIM1_b.TSEG2 = 0U;
      CAN1->CAN_MR            = 4U; // Keep CAN in reset mode
    case ARM_POWER_FULL:
      if (can_driver_initialized == 0U) {
        return ARM_DRIVER_ERROR;
      }
      if (can_driver_powered != 0U) {
        return ARM_DRIVER_OK;
      }
      M4CLK->CLK_ENABLE_SET_REG3 =
        M4_SOC_CLK_FOR_OTHER_ENABLE;                // Enable common clock for peripheral on which CAN is also workin
      M4CLK->CLK_ENABLE_SET_REG2 = CAN1_CLK_ENABLE; // Enable CAN clock
      CAN1->CAN_IMR              = CAN_IMR_DOIM |   // Enable interrupts
                      CAN_IMR_BEIM | CAN_IMR_TIM | CAN_IMR_RIM | CAN_IMR_EPIM | CAN_IMR_EWIM | CAN_IMR_ALIM;
      can_driver_powered = 1U;
      NVIC_ClearPendingIRQ(CAN_IRQ_NUM);
      NVIC_EnableIRQ(CAN_IRQ_NUM);
      break;
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED; // Other states are not supported
  }
  return ARM_DRIVER_OK;
}

/**
  @fn          uint32_t CAN_GetClock (void)
  @brief       Retrieve CAN base clock frequency.
  @return      base clock frequency
*/
uint32_t CAN_GetClock(void)
{
  uint32_t can_div_factor = 0;
  if ((M4CLK->CLK_ENABLE_SET_REG3 & M4_SOC_CLK_FOR_OTHER_ENABLE) == 0U) { // If clock to peripheral is not enabled
    return 0U;
  }
  can_div_factor = M4CLK->CLK_CONFIG_REG3_b.CAN1_CLK_DIV_FAC; // Get CAN division factor value
  if (can_div_factor == 0) {                                  // If divider is 0 , divider is bypassed
    return SystemCoreClock;
  } else {
    return (SystemCoreClock / (2 * can_div_factor));
  }
}

/**
  @fn          CAN_SetBitrate (ARM_CAN_BITRATE_SELECT select, uint32_t bitrate, uint32_t bit_segments)
  @brief       Set bitrate for CAN interface.
  @param[in]   select       Bitrate selection
                 - \ref ARM_CAN_BITRATE_NOMINAL : nominal (flexible data-rate arbitration) bitrate
                 - \ref ARM_CAN_BITRATE_FD_DATA : flexible data-rate data bitrate
  @param[in]   bitrate      Bitrate
  @param[in]   bit_segments Bit segments settings
  @return      execution status
*/
static int32_t CAN_SetBitrate(ARM_CAN_BITRATE_SELECT select, uint32_t bitrate, uint32_t bit_segments)
{
  float sjw, prop_seg, phase_seg1, phase_seg2, pclk, brp, tq_num;

  if (select != ARM_CAN_BITRATE_NOMINAL) {
    return ARM_CAN_INVALID_BITRATE_SELECT;
  }
  if (can_driver_powered == 0U) {
    return ARM_DRIVER_ERROR;
  }

  prop_seg   = (bit_segments & ARM_CAN_BIT_PROP_SEG_Msk) >> ARM_CAN_BIT_PROP_SEG_Pos;
  phase_seg1 = (bit_segments & ARM_CAN_BIT_PHASE_SEG1_Msk) >> ARM_CAN_BIT_PHASE_SEG1_Pos;
  phase_seg2 = (bit_segments & ARM_CAN_BIT_PHASE_SEG2_Msk) >> ARM_CAN_BIT_PHASE_SEG2_Pos;
  sjw        = (bit_segments & ARM_CAN_BIT_SJW_Msk) >> ARM_CAN_BIT_SJW_Pos;

  if (((prop_seg + phase_seg1) < 1U) || ((prop_seg + phase_seg1) > 16U)) {
    return ARM_CAN_INVALID_BIT_PROP_SEG;
  }
  if ((phase_seg2 < 1U) || (phase_seg2 > 8U)) {
    return ARM_CAN_INVALID_BIT_PHASE_SEG2;
  }
  if ((sjw < 1U) || (sjw > 4U)) {
    return ARM_CAN_INVALID_BIT_SJW;
  }

  tq_num = 1U + prop_seg + phase_seg1 + phase_seg2;
  pclk   = CAN_GetClock();

  if (pclk == 0U) {
    return ARM_DRIVER_ERROR;
  }

  brp = (int)((float)(1.0f / bitrate) / (float)(2.0f * (1 / pclk) * tq_num));

  if (brp > 64U) {
    return ARM_CAN_INVALID_BITRATE;
  }

  CAN_SetHwModeConfig(CAN_HW_RESET_MODE_CONFIG); // Keep CAN in reset mode

  CAN1->CAN_BTIM0_b.BRP   = (brp - 1);
  CAN1->CAN_BTIM0_b.SJW   = (sjw - 1);
  CAN1->CAN_BTIM1_b.TSEG1 = ((prop_seg + phase_seg1) - 1);
  CAN1->CAN_BTIM1_b.TSEG2 = (phase_seg2 - 1);
  CAN_SetHwModeConfig(CAN_HW_NORMAL_MODE_CONFIG); // Keep CAN in reset mode

  return ARM_DRIVER_OK;
}

/**
  @fn          int32_t CAN_SetMode (ARM_CAN_MODE mode)
  @brief       Set operating mode for CAN interface.
  @param[in]   mode   Operating mode
                 - \ref ARM_CAN_MODE_INITIALIZATION :    initialization mode
                 - \ref ARM_CAN_MODE_NORMAL         :    normal operation mode
                 - \ref ARM_CAN_MODE_RESTRICTED     :    restricted operation mode
                 - \ref ARM_CAN_MODE_MONITOR        :    bus monitoring mode
                 - \ref ARM_CAN_MODE_LOOPBACK_INTERNAL : loopback internal mode
                 - \ref ARM_CAN_MODE_LOOPBACK_EXTERNAL : loopback external mode
  @return      execution status
*/
static int32_t CAN_SetMode(ARM_CAN_MODE mode)
{

  if (can_driver_powered == 0U) {
    return ARM_DRIVER_ERROR;
  }

  switch (mode) {
    case ARM_CAN_MODE_INITIALIZATION:
      CAN_RemoveFilter(CAN_FILTER_TYPE_EXACT_ID, 0, 0); // Remove filters
      break;
    case ARM_CAN_MODE_NORMAL:
      CAN_SetHwModeConfig(CAN_HW_NORMAL_MODE_CONFIG); // Configure CAN in normal mode
      break;
    case ARM_CAN_MODE_MONITOR:
      CAN_SetHwModeConfig(CAN_HW_LISTEN_ONLY_MODE_CONFIG); // Configure the CAN in listen only mode
      break;
    case ARM_CAN_MODE_LOOPBACK_INTERNAL: // Not supported
    case ARM_CAN_MODE_RESTRICTED:        // Not supported
    case ARM_CAN_MODE_LOOPBACK_EXTERNAL: // Not supported
    default:
      // Handle unknown mode code
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  return ARM_DRIVER_OK;
}

/**
  @fn          ARM_CAN_OBJ_CAPABILITIES CAN_ObjectGetCapabilities (uint32_t obj_idx)
  @brief       Retrieve capabilities of an object.
  @param[in]   obj_idx  Object index
  @return      \ref ARM_CAN_OBJ_CAPABILITIES
*/
ARM_CAN_OBJ_CAPABILITIES CAN_ObjectGetCapabilities(uint32_t obj_idx)
{
  ARM_CAN_OBJ_CAPABILITIES obj_cap_null;

  if (obj_idx > 1U) {
    memset(&obj_cap_null, 0U, sizeof(ARM_CAN_OBJ_CAPABILITIES));
    return obj_cap_null;
  }
  if (obj_idx == 0U) {
    return can_object_capabilities_rx;
  } else {
    return can_object_capabilities_tx;
  }
}

/**
  @fn          CAN_ObjectSetFilter (uint32_t obj_idx, ARM_CAN_FILTER_OPERATION operation, uint32_t id, uint32_t arg)
  @brief       Add or remove filter for message reception.
  @param[in]   obj_idx      Object index of object that filter should be or is assigned to
  @param[in]   operation    Operation on filter
                 - \ref ARM_CAN_FILTER_ID_EXACT_ADD    :    add    exact id filter
                 - \ref ARM_CAN_FILTER_ID_EXACT_REMOVE :    remove exact id filter
                 - \ref ARM_CAN_FILTER_ID_RANGE_ADD    :    add    range id filter
                 - \ref ARM_CAN_FILTER_ID_RANGE_REMOVE :    remove range id filter
                 - \ref ARM_CAN_FILTER_ID_MASKABLE_ADD :    add    maskable id filter
                 - \ref ARM_CAN_FILTER_ID_MASKABLE_REMOVE : remove maskable id filter
  @param[in]   id     ID or start of ID range (depending on filter type)
  @param[in]   arg    Mask or end of ID range (depending on filter type)
  @return      execution status
*/
static int32_t CAN_ObjectSetFilter(uint32_t obj_idx, ARM_CAN_FILTER_OPERATION operation, uint32_t id, uint32_t arg)
{
  int32_t status;

  if (obj_idx != 0U) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }
  if (can_driver_powered == 0U) {
    return ARM_DRIVER_ERROR;
  }

  switch (operation) {
    case ARM_CAN_FILTER_ID_EXACT_ADD:
      status = CAN_AddHwFilter(CAN_FILTER_TYPE_EXACT_ID, id, 0U);
      break;
    case ARM_CAN_FILTER_ID_MASKABLE_ADD:
      status = CAN_AddHwFilter(CAN_FILTER_TYPE_MASKABLE_ID, id, arg);
      break;
    case ARM_CAN_FILTER_ID_EXACT_REMOVE:
      status = CAN_RemoveFilter(CAN_FILTER_TYPE_EXACT_ID, id, 0U);
      break;
    case ARM_CAN_FILTER_ID_MASKABLE_REMOVE:
      status = CAN_RemoveFilter(CAN_FILTER_TYPE_MASKABLE_ID, id, 0);
      break;
    case ARM_CAN_FILTER_ID_RANGE_ADD:    // Not supported
    case ARM_CAN_FILTER_ID_RANGE_REMOVE: // Not supported
    default:
      status = ARM_DRIVER_ERROR_UNSUPPORTED;
      break;
  }

  return status;
}

/**
  @fn          int32_t CAN_ObjectConfigure (uint32_t obj_idx, ARM_CAN_OBJ_CONFIG obj_cfg)
  @brief       Configure object.
  @param[in]   obj_idx  Object index
  @param[in]   obj_cfg  Object configuration state
                 - \ref ARM_CAN_OBJ_INACTIVE : deactivate object
                 - \ref ARM_CAN_OBJ_RX       : configure object for reception
                 - \ref ARM_CAN_OBJ_TX       : configure object for transmission
                 - \ref ARM_CAN_OBJ_RX_RTR_TX_DATA : configure object that on RTR reception automatically transmits Data Frame
                 - \ref ARM_CAN_OBJ_TX_RTR_RX_DATA : configure object that transmits RTR and automatically receives Data Frame
  @return      execution status
*/
static int32_t CAN_ObjectConfigure(uint32_t obj_idx, ARM_CAN_OBJ_CONFIG obj_cfg)
{

  if (obj_idx > 1U) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }
  if (can_driver_powered == 0U) {
    return ARM_DRIVER_ERROR;
  }

  switch (obj_cfg) {
    case ARM_CAN_OBJ_INACTIVE:
      can_obj_cfg_msk &= ~(1U << obj_idx);
      break;
    case ARM_CAN_OBJ_RX_RTR_TX_DATA:
    case ARM_CAN_OBJ_TX_RTR_RX_DATA:
      can_obj_cfg_msk &= ~(1U << obj_idx);
      return ARM_DRIVER_ERROR_UNSUPPORTED;
    case ARM_CAN_OBJ_TX:
      if (obj_idx != 1U) {
        return ARM_DRIVER_ERROR_PARAMETER;
      }
      can_obj_cfg_msk = 2U;
      break;
    case ARM_CAN_OBJ_RX:
      if (obj_idx != 0U) {
        return ARM_DRIVER_ERROR_PARAMETER;
      }
      can_obj_cfg_msk = 1U;
      break;
    default:
      return ARM_DRIVER_ERROR;
  }

  return ARM_DRIVER_OK;
}

/**
  @fn          int32_t CAN_MessageSend (uint32_t obj_idx, ARM_CAN_MSG_INFO *msg_info, const uint8_t *data, uint8_t size)
  @brief       Send message on CAN bus.
  @param[in]   obj_idx  Object index
  @param[in]   msg_info Pointer to CAN message information
  @param[in]   data     Pointer to data buffer
  @param[in]   size     Number of data bytes to send
  @return      value >= 0  number of data bytes accepted to send
  @return      value < 0   execution status
*/
static int32_t CAN_MessageSend(uint32_t obj_idx, ARM_CAN_MSG_INFO *msg_info, const uint8_t *data, uint8_t size)
{
  uint32_t tx_buf_reg1 = 0U;
  uint32_t tx_buf_reg2 = 0U;
  uint32_t tx_buf_reg3 = 0U;

  if (can_driver_powered == 0U) {
    return ARM_DRIVER_ERROR;
  }
  if (obj_idx != 1U) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if (CAN1->CAN_SR_b.TBS != 1U) { // Check for the previous transmission status
    return ARM_DRIVER_ERROR_BUSY;
  }
  if (((msg_info->id) & ARM_CAN_ID_IDE_Msk) == 0U) { // Standard Identifier (11 bit)
    tx_buf_reg1 = (msg_info->dlc);
    tx_buf_reg1 |= ((msg_info->id >> 3) & 0xFF) << 8U;
    tx_buf_reg1 |= ((msg_info->id >> 0) & 0x07) << 21U;
    if (msg_info->rtr == 0U) { // If data frame
      tx_buf_reg1 |= (data[0] << 24);
      CAN1->CAN_TXBUF = tx_buf_reg1;
      tx_buf_reg1     = 0U;
      switch (size) {
        case 8:
          tx_buf_reg2 |= (data[7] << 16U);
        case 7:
          tx_buf_reg2 |= (data[6] << 8U);
        case 6:
          tx_buf_reg2 |= (data[5] << 0U);
        case 5:
          tx_buf_reg1 |= (data[4] << 24U);
        case 4:
          tx_buf_reg1 |= (data[3] << 16U);
        case 3:
          tx_buf_reg1 |= (data[2] << 8U);
        case 2:
          tx_buf_reg1 |= (data[1] << 0U);
          CAN1->CAN_TXBUF = tx_buf_reg1;
          CAN1->CAN_TXBUF = tx_buf_reg2;
          break;
        default:
          return ARM_DRIVER_ERROR_UNSUPPORTED;
      }
    } else { // Remote frame
      size = 0U;
      tx_buf_reg1 |= BIT(6);
      CAN1->CAN_TXBUF = tx_buf_reg1;
    }
  } else { // Extended Identifier (11 bit)
    tx_buf_reg1 = BIT(7);
    tx_buf_reg1 |= (((msg_info->id >> 21) & 0xFF) << 8);
    tx_buf_reg1 |= (((msg_info->id >> 13) & 0xFF) << 16);
    tx_buf_reg1 |= (((msg_info->id >> 5) & 0xFF) << 24);
    if (msg_info->rtr == 0U) { // If Data frame
      tx_buf_reg1 |= msg_info->dlc;
      CAN1->CAN_TXBUF = tx_buf_reg1; // Update the hardware;
      tx_buf_reg1     = 0;
      tx_buf_reg1     = ((msg_info->id << 3) & 0xFF);
      switch (size) {
        case 8:
          tx_buf_reg3 |= (data[7] << 0U);
        case 7:
          tx_buf_reg2 |= (data[6] << 24U);
        case 6:
          tx_buf_reg2 |= (data[5] << 16U);
        case 5:
          tx_buf_reg2 |= (data[4] << 8U);
        case 4:
          tx_buf_reg2 |= (data[3] << 0U);
        case 3:
          tx_buf_reg1 |= (data[2] << 24U);
        case 2:
          tx_buf_reg1 |= (data[1] << 16U);
        case 1:
          tx_buf_reg1 |= (data[0] << 8U);
          CAN1->CAN_TXBUF = tx_buf_reg1; // Update the hardware;
          CAN1->CAN_TXBUF = tx_buf_reg2; // Update the hardware;
          CAN1->CAN_TXBUF = tx_buf_reg3; // Update the hardware;
          break;
        default:
          return ARM_DRIVER_ERROR_UNSUPPORTED;
      }
    } else { // If remote frame
      size = 0U;
      tx_buf_reg1 |= BIT(6);
      CAN1->CAN_TXBUF = tx_buf_reg1; // Update the hardware;
      tx_buf_reg1     = 0;
      tx_buf_reg1     = ((msg_info->id << 3) & 0xFF);
      CAN1->CAN_TXBUF = tx_buf_reg1; // Update the hardware;
    }
  }

  CAN1->CAN_CMR_b.TR = 1U; // Trigger transfer

  return ((int32_t)size);
}

/**
  @fn          int32_t CAN_MessageRead (uint32_t obj_idx, ARM_CAN_MSG_INFO *msg_info, uint8_t *data, uint8_t size) {
  @brief       Read message received on CAN bus.
  @param[in]   obj_idx  Object index
  @param[out]  msg_info Pointer to read CAN message information
  @param[out]  data     Pointer to data buffer for read data
  @param[in]   size     Maximum number of data bytes to read
  @return      value >= 0  number of data bytes read
  @return      value < 0   execution status
*/
static int32_t CAN_MessageRead(uint32_t obj_idx, ARM_CAN_MSG_INFO *msg_info, uint8_t *data, uint8_t size)
{
  uint32_t rx_buf_reg0 = 0;
  uint32_t rx_buf_reg1 = 0;
  uint32_t rx_buf_reg2 = 0;
  uint32_t rx_buf_reg3 = 0;

  if (can_driver_powered == 0U) {
    return ARM_DRIVER_ERROR;
  }
  if (obj_idx != 0U) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  rx_buf_reg0 = CAN1->CAN_RXBUF; //Read FIFO

  if (rx_buf_reg0 & (1 << 6)) {
    msg_info->rtr = 1U;
  } else {
    msg_info->rtr = 0U;
  }
  msg_info->dlc = (rx_buf_reg0 & 0x0F);
  if (rx_buf_reg0 & (1 << 7)) {     // If extended ID is received
    rx_buf_reg1  = CAN1->CAN_RXBUF; //Read FIFO
    rx_buf_reg2  = CAN1->CAN_RXBUF; //Read FIFO
    rx_buf_reg3  = CAN1->CAN_RXBUF; //Read FIFO
    msg_info->id = (((rx_buf_reg0 >> 8) & 0xFF) << 21U);
    msg_info->id |= (((rx_buf_reg0 >> 16) & 0xFF) << 13U);
    msg_info->id |= (((rx_buf_reg0 >> 24) & 0xFF) << 5U);
    msg_info->id |= (((rx_buf_reg1 >> 3) & 0x1F) << 0);
    if (msg_info->rtr == 0U) { // Data frame is received
      switch (size) {
        case 8:
          data[7] = ((rx_buf_reg3 >> 0) & 0xFF);
        case 7:
          data[6] = ((rx_buf_reg2 >> 24) & 0xFF);
        case 6:
          data[5] = ((rx_buf_reg2 >> 16) & 0xFF);
        case 5:
          data[4] = ((rx_buf_reg2 >> 8) & 0xFF);
        case 4:
          data[3] = ((rx_buf_reg2 >> 0) & 0xFF);
        case 3:
          data[2] = ((rx_buf_reg1 >> 24) & 0xFF);
        case 2:
          data[1] = ((rx_buf_reg1 >> 16) & 0xFF);
        case 1:
          data[0] = ((rx_buf_reg1 >> 8) & 0xFF);
          break;
        case 0:
          break;
        default:
          return ARM_DRIVER_ERROR_UNSUPPORTED;
      }
    } else { // If remote frame is received
      // Nothing
      size = 0U;
    }
  } else {                          // If standard ID is received
    rx_buf_reg1  = CAN1->CAN_RXBUF; //Read FIFO
    rx_buf_reg2  = CAN1->CAN_RXBUF; //Read FIFO
    msg_info->id = ((rx_buf_reg0 & 0xFF00) >> 8);
    msg_info->id = (msg_info->id << 3);
    msg_info->id |= ((rx_buf_reg0 & 0xE00000) >> 21);
    if (msg_info->rtr == 0U) { // If data frame received
      switch (size) {
        case 8:
          data[7] = ((rx_buf_reg2 >> 16) & 0xFF);
        case 7:
          data[6] = ((rx_buf_reg2 >> 8) & 0xFF);
        case 6:
          data[5] = ((rx_buf_reg2 >> 0) & 0xFF);
        case 5:
          data[4] = ((rx_buf_reg1 >> 24) & 0xFF);
        case 4:
          data[3] = ((rx_buf_reg1 >> 16) & 0xFF);
        case 3:
          data[2] = ((rx_buf_reg1 >> 8) & 0xFF);
        case 2:
          data[1] = ((rx_buf_reg1 >> 0) & 0xFF);
        case 1:
          data[0] = ((rx_buf_reg0 >> 24) & 0xFF);
          break;
        case 0:
          break;
        default:
          return ARM_DRIVER_ERROR_UNSUPPORTED;
      }
    } else {
      // If remote frame is received
      // Nothing
      size = 0U;
    }
  }
  return ((int32_t)size);
}

/**
  @fn          int32_t CAN_Control (uint32_t control, uint32_t arg)
  @brief       Control CAN interface.
  @param[in]   control  Operation
                 - \ref ARM_CAN_SET_FD_MODE        : set FD operation mode
                 - \ref ARM_CAN_ABORT_MESSAGE_SEND : abort sending of CAN message
                 - \ref ARM_CAN_CONTROL_RETRANSMISSION : enable/disable automatic retransmission
                 - \ref ARM_CAN_SET_TRANSCEIVER_DELAY  : set transceiver delay
  @param[in]   arg   Argument of operation
  @return      execution status
*/
static int32_t CAN_Control(uint32_t control, uint32_t arg)
{

  if (can_driver_powered == 0U) {
    return ARM_DRIVER_ERROR;
  }

  switch (control & ARM_CAN_CONTROL_Msk) {
    case ARM_CAN_ABORT_MESSAGE_SEND:
      if (arg == 1U) {
        CAN1->CAN_CMR = CAN_CMR_AT;
      }
      break;
    case ARM_CAN_CONTROL_RETRANSMISSION:
      if (arg == 1U) {
        CAN1->CAN_CMR = CAN_CMR_TR;
      } else {
        CAN1->CAN_CMR = (CAN_CMR_AT | CAN_CMR_TR);
      }
      break;
    case ARM_CAN_SET_FD_MODE:
    case ARM_CAN_SET_TRANSCEIVER_DELAY:
    default:
      // Handle unknown control code
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  return ARM_DRIVER_OK;
}

/**
  @fn          ARM_CAN_STATUS CAN_GetStatus (void)
  @brief       Get CAN status.
  @return      CAN status \ref ARM_CAN_STATUS
*/
static ARM_CAN_STATUS CAN_GetStatus(void)
{
  ARM_CAN_STATUS status;

  memset(&status, 0U, sizeof(ARM_CAN_STATUS));

  if (can_driver_powered == 0U) {
    return status;
  }

  status.last_error_code = can_last_error_code;
  status.tx_error_count  = CAN1->CAN_TXERR; // Update TX error count
  status.rx_error_count  = CAN1->CAN_RXERR; // Update RX error count

  if ((((CAN1->CAN_TXERR) > 127U) || ((CAN1->CAN_RXERR) > 127U))) { // If Error Passive Interrupt is active
    status.unit_state = ARM_CAN_UNIT_STATE_PASSIVE;
  } else {
    status.unit_state = ARM_CAN_UNIT_STATE_ACTIVE;
  }
  return status;
}

/**
  @fn          void IRQ066_Handler (void)
  @brief       CAN1 Interrupt Routine (IRQ).
*/
void IRQ066_Handler(void)
{
  uint8_t isr, sr;

  if (can_driver_powered != 0U) {
    isr = CAN1->CAN_ISR_IACK; // Read interrupt status register
    sr  = CAN1->CAN_SR;
    if (isr & CAN_ISR_DO) { // Data Over run interrupt is active
      CAN_SignalObjectEvent(0U, ARM_CAN_EVENT_RECEIVE_OVERRUN);
      CAN1->CAN_ISR_IACK = CAN_ISR_DO;
    }
    if (isr & CAN_ISR_BEI) { // Bus error interrupt is active

      if (CAN1->CAN_ECC_b.BER) {
        can_last_error_code = ARM_CAN_LEC_BIT_ERROR;
      }
      if (CAN1->CAN_ECC_b.STFER) {
        can_last_error_code = ARM_CAN_LEC_STUFF_ERROR;
      }
      if (CAN1->CAN_ECC_b.CRCER) {
        can_last_error_code = ARM_CAN_LEC_CRC_ERROR;
      }
      if (CAN1->CAN_ECC_b.FRMER) {
        can_last_error_code = ARM_CAN_LEC_FORM_ERROR;
      }
      if (CAN1->CAN_ECC_b.ACKER) {
        can_last_error_code = ARM_CAN_LEC_ACK_ERROR;
      }
      CAN1->CAN_ISR_IACK = CAN_ISR_BEI;
    }
    if (isr & CAN_ISR_TI) { // Transmit interrupt interrupt is active
      CAN_SignalObjectEvent(1U, ARM_CAN_EVENT_SEND_COMPLETE);
      CAN1->CAN_ISR_IACK = CAN_ISR_TI;
    }
    if (isr & CAN_ISR_RI) { // Receive interrupt is triggered

      while (CAN1->CAN_RMC) // Note keep reading messages until FIFO empty
      {
        CAN_SignalObjectEvent(0U, ARM_CAN_EVENT_RECEIVE);
        CAN1->CAN_ISR_IACK = CAN_ISR_RI; //Clear the RX Data over run interrupt
      }
    }
    if (isr & CAN_ISR_EPI) {                                            // Error passive interrupt is active
      if ((((CAN1->CAN_TXERR) > 127U) || ((CAN1->CAN_RXERR) > 127U))) { // If Error Passive Interrupt is active
        CAN_SignalUnitEvent(ARM_CAN_EVENT_UNIT_PASSIVE);
      } else {
        CAN_SignalUnitEvent(ARM_CAN_EVENT_UNIT_ACTIVE);
      }
      CAN1->CAN_ISR_IACK = CAN_ISR_EPI;
    }
    if (isr & CAN_ISR_EWI) { // Error warning interrupt
      if (sr & CAN_SR_ES) {
        CAN_SignalUnitEvent(ARM_CAN_EVENT_UNIT_WARNING);
      }
      CAN1->CAN_ISR_IACK = CAN_ISR_EWI;
    }
    if (isr & CAN_ISR_ALI) { // Arbitration lost interrupt
      //FIXME: Handle this here //Added by me this handle
      CAN_SignalUnitEvent(ARM_CAN_ARBITRATION_LOST);
      CAN1->CAN_ISR_IACK = CAN_ISR_ALI;
    }
  }
}

// CAN driver functions structure
ARM_DRIVER_CAN Driver_CAN1 = { CAN_GetVersion,      CAN_GetCapabilities, CAN_Initialize,
                               CAN_Uninitialize,    CAN_PowerControl,    CAN_GetClock,
                               CAN_SetBitrate,      CAN_SetMode,         CAN_ObjectGetCapabilities,
                               CAN_ObjectSetFilter, CAN_ObjectConfigure, CAN_MessageSend,
                               CAN_MessageRead,     CAN_Control,         CAN_GetStatus };

#endif //RTE_CAN1
#endif
