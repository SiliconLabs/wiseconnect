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
/// @brief Enumeration for finding the position of Joystick.
typedef enum {
  SL_JOYSTICK_NONE, ///< Not pressed
  SL_JOYSTICK_C,    ///< Center
  SL_JOYSTICK_N,    ///< North
  SL_JOYSTICK_E,    ///< East
  SL_JOYSTICK_S,    ///< South
  SL_JOYSTICK_W,    ///< West
} sl_joystick_position_t;

/// @brief Enumeration for Joystick state (ENABLE / DISABLE) enum.
typedef enum {
  SL_JOYSTICK_DISABLED, ///< Joystick data acquisition is disabled
  SL_JOYSTICK_ENABLED,  ///< Joystick data acquisition is enabled
} sl_joystick_state_t;

// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************/ /**
 * sl_status_t sl_si91x_joystick_init(void)
 * Initialize Joystick.
 *
 * @param[in]  None.
 * @return status 0 if successful,
 *         \ref SL_STATUS_OK (0x0000) - Success 
*
 ******************************************************************************/
sl_status_t sl_si91x_joystick_init(void);

/***************************************************************************/ /**
 * sl_status_t sl_si91x_joystick_get_position(sl_joystick_state_t state, sl_joystick_position_t *pos)
 * Getting the direction of Joystick.
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_joystick_init 
*-
 *      \ref sl_si91x_joystick_start 
*
 *
 * @param[in]  state  : Joystick enable/disable
 * @param[in]  pos    : position of joystick.
 * @return status 0 if successful, else error code as follow:
 * -        \ref SL_STATUS_OK (0x0000) - Success 
*-
 *         \ref SL_STATUS_NOT_READY (0x0003) - Module is not ready for requested operation.
 ******************************************************************************/
sl_status_t sl_si91x_joystick_get_position(sl_joystick_state_t state, sl_joystick_position_t *pos);

/***************************************************************************/ /**
 * sl_status_t sl_si91x_joystick_start(sl_joystick_state_t state)
 * Start/Enable the Joystick.
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_joystick_init 
*
 *
 * @param[in]  state    : Joystick enable/disable.
 *
 * @return status 0 if successful, else error code as follow:
 *-         \ref SL_STATUS_OK (0x0000) - Success 
*-
 *         \ref SL_STATUS_ABORT (0x0006) - Operation aborted.
 ******************************************************************************/
sl_status_t sl_si91x_joystick_start(sl_joystick_state_t state);

/***************************************************************************/ /**
 * sl_status_t sl_si91x_joystick_stop(sl_joystick_state_t state)
 * Stop/Disable the Joystick.
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_joystick_init 
*-
 *      \ref sl_si91x_joystick_start 
*
 *
 * @param[in]  state    : Joystick enable/disable.
 *
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_OK (0x0000) - Success 
*-
 *         \ref SL_STATUS_BUSY (0x0004) - Module is busy.
 ******************************************************************************/
sl_status_t sl_si91x_joystick_stop(sl_joystick_state_t state);

/** @} (end addtogroup JOYSTICK) */

#ifdef __cplusplus
}
#endif

#endif /* SL_SI91X_JOYSTICK_H_ */
