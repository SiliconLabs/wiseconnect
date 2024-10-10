/***************************************************************************/ /**
* @file sl_si91x_i2c_init_i2c2_config.h
* @brief I2c driver instance configuration file.
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

#ifndef SL_SI91X_I2C_INIT_I2C2_CONFIG_H
#define SL_SI91X_I2C_INIT_I2C2_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>
#ifdef __cplusplus
extern "C" {
#endif

#warning \
  "For an OPN or SoC project, I2C instances not defined. Installing the [ENABLE USER CONFIGURATION] component or defining USER_CONFIGURATION_ENABLE MACRO to 1 is the first step towards configuring the board macros. Then, define the macros in the header file in accordance with the board connections.."

#if USER_CONFIGURATION_ENABLE
#include "sl_si91x_i2c.h"
/******************************************************************************/
/******************************* I2C Configuration **************************/
// <h> I2C2 Configuration
// <o SL_I2C_I2C2_MODE> Mode
//   <SL_I2C_LEADER_MODE=> Leader mode
//   <SL_I2C_FOLLOWER_MODE=> Follower mode
// <i> Selection of the I2C Mode.
#define SL_I2C_I2C2_MODE SL_I2C_LEADER_MODE

// <o SL_I2C_I2C2_OPERATING_MODE> Operating Mode
//   <SL_I2C_STANDARD_MODE=> Standard mode
//   <SL_I2C_FAST_MODE=> Fast mode
//   <SL_I2C_FAST_PLUS_MODE=> Fast plus mode
//   <SL_I2C_HIGH_SPEED_MODE=> High speed mode
// <i> Selection of the I2C Mode.
#define SL_I2C_I2C2_OPERATING_MODE SL_I2C_STANDARD_MODE

// <o SL_I2C_I2C2_TRANSFER_TYPE> Transfer Type
//   <SL_I2C_USING_INTERRUPT=> Using Interrupt
//   <SL_I2C_USING_DMA=> Using DMA
// <i> Selection of the I2C Mode.
#define SL_I2C_I2C2_TRANSFER_TYPE SL_I2C_USING_INTERRUPT

// </h> End I2C2 Configuration
/******************************************************************************/
// <<< end of configuration section >>>
// <<< sl:start pin_tool >>>
// <i2c signal=SCL,SDA> SL_SI70XX
// $[I2C_SL_SI70XX]
#ifndef SL_SI70XX_PERIPHERAL
#define SL_SI70XX_PERIPHERAL ULP_I2C
#endif

// ULP_I2C SCL on ULP_GPIO_7/GPIO_71
#ifndef SL_SI70XX_SCL_PORT
#define SL_SI70XX_SCL_PORT ULP
#endif
#ifndef SL_SI70XX_SCL_PIN
#define SL_SI70XX_SCL_PIN 7
#endif
#ifndef SL_SI70XX_SCL_LOC
#define SL_SI70XX_SCL_LOC 2
#endif

// ULP_I2C SDA on ULP_GPIO_6/GPIO_70
#ifndef SL_SI70XX_SDA_PORT
#define SL_SI70XX_SDA_PORT ULP
#endif
#ifndef SL_SI70XX_SDA_PIN
#define SL_SI70XX_SDA_PIN 6
#endif
#ifndef SL_SI70XX_SDA_LOC
#define SL_SI70XX_SDA_LOC 6
#endif
// [I2C_SL_SI70XX]$
// <<< sl:end pin_tool >>>

#define SL_I2C_I2C2_SCL_PORT 0
#define SL_I2C_I2C2_SCL_PIN  SL_SI70XX_SCL_PIN
#define SL_I2C_I2C2_SCL_MUX  SL_SI91X_I2C2_SCL_MUX
#define SL_I2C_I2C2_SCL_PAD  SL_SI91X_I2C2_SCL_PAD
#define SL_I2C_I2C2_SCL_REN  SL_SI91X_I2C2_SCL_REN

#define SL_I2C_I2C2_SDA_PORT 0
#define SL_I2C_I2C2_SDA_PIN  SL_SI70XX_SDA_PIN
#define SL_I2C_I2C2_SDA_MUX  SL_SI91X_I2C2_SDA_MUX
#define SL_I2C_I2C2_SDA_PAD  SL_SI91X_I2C2_SDA_PAD
#define SL_I2C_I2C2_SDA_REN  SL_SI91X_I2C2_SDA_REN

#ifdef __cplusplus
}
#endif

#endif // USER_CONFIGURATION_ENABLE
#endif // SL_SI91X_I2C_INIT_I2C2_CONFIG_H