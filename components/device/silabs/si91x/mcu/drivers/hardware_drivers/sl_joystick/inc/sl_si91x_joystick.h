/***************************************************************************/ /**
 * @file sl_si91x_joystick.h
 * @brief JOYSTICK API implementation
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

#ifndef SL_SI91X_JOYSTICK_H_
#define SL_SI91X_JOYSTICK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_status.h"
#include "sl_si91x_adc.h"
#include "sl_si91x_joystick_config.h"

/***************************************************************************/ /**
 * @addtogroup JOYSTICK Joystick
 * @ingroup SI91X_HARDWARE_DRIVER_APIS
 * @{
 ******************************************************************************/
/*******************************************************************************
 ******************************   DEFINES   ************************************
 ******************************************************************************/
/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN
#define JOYSTICK_MV_ERROR JOYSTICK_MV_ERR_CARDINAL_ONLY // tolerance in mV for cardinal
/// @endcond

/*******************************************************************************
 *****************************   DATA TYPES   *********************************
 ******************************************************************************/
/// @brief Enumeration for finding the position of the Joystick.
typedef enum {
  SL_JOYSTICK_NONE, ///< Not pressed
  SL_JOYSTICK_C,    ///< Center
  SL_JOYSTICK_N,    ///< North
  SL_JOYSTICK_E,    ///< East
  SL_JOYSTICK_S,    ///< South
  SL_JOYSTICK_W,    ///< West
} sl_joystick_position_t;

/// @brief Enumeration for the Joystick state (ENABLE/DISABLE) enum.
typedef enum {
  SL_JOYSTICK_DISABLED, ///< Joystick data acquisition is disabled
  SL_JOYSTICK_ENABLED,  ///< Joystick data acquisition is enabled
} sl_joystick_state_t;

// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************/ /**
* Initialize the Joystick. ADC will configure here to read the different 
* voltage based on different Joystick position.
*
* @param  None.
* @return If successful
* - \ref SL_STATUS_OK (0x0000) - Success 
*
******************************************************************************/
sl_status_t sl_si91x_joystick_init(void);

/***************************************************************************/ /**
* Getting the direction/position of Joystick.
*
* @pre Pre-conditions:
* - \ref sl_si91x_joystick_init 
* - \ref sl_si91x_joystick_start 
* @param[in]  state  : Joystick enable/disable
*                ( \ref sl_joystick_state_t )
* @param[out]  pos    : position of joystick.
*                ( \ref sl_joystick_position_t)
*
* @return Error code as follow:
* - \ref SL_STATUS_OK (0x0000) - Success 
* - \ref SL_STATUS_NOT_READY (0x0003) - Module is not ready for requested operation.
******************************************************************************/
sl_status_t sl_si91x_joystick_get_position(sl_joystick_state_t state, sl_joystick_position_t *pos);

/***************************************************************************/ /**
* Start/Enable the Joystick.
*
* @pre Pre-conditions:
* - \ref sl_si91x_joystick_init 
* @param[in]  state    : Joystick enable/disable.
*                ( \ref sl_joystick_state_t )
*
* @return Error code as follow:
* - \ref SL_STATUS_OK (0x0000) - Success \n
* - \ref SL_STATUS_ABORT (0x0006) - Operation aborted.
******************************************************************************/
sl_status_t sl_si91x_joystick_start(sl_joystick_state_t state);

/***************************************************************************/ /**
* Stop/Disable the Joystick.
*
* @pre Pre-conditions:
* - \ref sl_si91x_joystick_init 
* - \ref sl_si91x_joystick_start 
* @param[in]  state    : Joystick enable/disable.
*                ( \ref sl_joystick_state_t )
*
* @return Error code as follow:
* - \ref SL_STATUS_OK (0x0000) - Success 
* - \ref SL_STATUS_BUSY (0x0004) - Module is busy.
******************************************************************************/
sl_status_t sl_si91x_joystick_stop(sl_joystick_state_t state);

// ******** THE REST OF THE FILE IS DOCUMENTATION ONLY !***********************
/// @addtogroup JOYSTICK Joystick
/// @{
///
///   @details
///
///   @n @section joystick_intro Introduction
///
///   The SI91x joystick driver is a platform-level software module responsible for managing the control of joystick.
///   All joystick functions are called through the generic driver.
///
///   @n @section joystick_config Configuration
///
///   The @ref sl_joystick_state_t enumeration defines the states of the joystick data acquisition, either disabled or enabled. This allows for controlling when
///   joystick input is actively monitored. The @ref sl_joystick_position_t This enumeration defines the possible positions
///   of the joystick, including not pressed, center, north, east, south, and west. These positions correspond to the physical orientation
///   of the joystick and can be used to determine the direction or action indicated by the joystick's current position.
///
///   @li For more information on configuring available parameters refer to the respective peripheral example readme document and
///   refer here https://docs.silabs.com/wiseconnect/latest/wiseconnect-api-reference-guide-si91x-peripherals/disable-uc-config.
///
///
///   @n @section led_usage Usage
///
///   The common functions include the following:
///
///   @li @ref sl_si91x_joystick_init
///   @li @ref sl_si91x_joystick_start
///   @li @ref sl_si91x_joystick_get_position
///   @li @ref sl_si91x_joystick_stop
///
///   @ref sl_si91x_joystick_init initialises the joystick. Sets the operation mode and initialises the ADC channel.
///
///   @ref sl_si91x_joystick_get_position can be implemented by the application if required. This function returns the position
///   corresponding to the physical orientation of the joystick. @ref sl_si91x_joystick_start and @ref sl_si91x_joystick_stop both can be implemented by the application.
///   These functions are used to start and stop the data acquisition from the joystick respectively.
///
/// @} end group JOYSTICK ********************************************************/

/** @} (end addtogroup JOYSTICK) */

#ifdef __cplusplus
}
#endif

#endif /* SL_SI91X_JOYSTICK_H_ */
