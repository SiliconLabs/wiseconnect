/*******************************************************************************
 * @file  sl_si91x_psram_handle.h
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef __SL_SI91X_PSRAM_HANDLE_H_
#define __SL_SI91X_PSRAM_HANDLE_H_

#include "sl_si91x_psram.h"
#include "sl_si91x_psram_config.h"
#include "sl_si91x_psram_pin_config.h"

// Handle for PSRAM Operations
extern sl_psram_info_type_t PSRAM_Device;

#define PSRAM_GPIO_PIN_SET_52_TO_57      1
#define PSRAM_GPIO_PIN_SET_0_TO_5        2
#define PSRAM_GPIO_PIN_SET_46_TO_51_CS_0 3
#define PSRAM_GPIO_PIN_SET_46_TO_51_CS_1 4
#define PSRAM_GPIO_PIN_SET_46_TO_57_CS_0 5
#define PSRAM_GPIO_PIN_SET_46_TO_57_CS_1 6

// Resolving Chip Select & PSRAM Base Address based on Pin Set
#if (PSRAM_GPIO_PIN_SET_SEL == PSRAM_GPIO_PIN_SET_52_TO_57)
#define PSRAM_CHIP_SELECT  (CHIP_ZERO)
#define PSRAM_BASE_ADDRESS (0x0A000000)
#elif (PSRAM_GPIO_PIN_SET_SEL == PSRAM_GPIO_PIN_SET_0_TO_5)
#define PSRAM_CHIP_SELECT  (CHIP_ZERO)
#define PSRAM_BASE_ADDRESS (0x0A000000)
#elif (PSRAM_GPIO_PIN_SET_SEL == PSRAM_GPIO_PIN_SET_46_TO_51_CS_0)
#define PSRAM_CHIP_SELECT  (CHIP_ZERO)
#define PSRAM_BASE_ADDRESS (0x0A000000)
#elif (PSRAM_GPIO_PIN_SET_SEL == PSRAM_GPIO_PIN_SET_46_TO_51_CS_1)
#define PSRAM_CHIP_SELECT  (CHIP_ONE)
#define PSRAM_BASE_ADDRESS (0x0B000000)
#elif (PSRAM_GPIO_PIN_SET_SEL == PSRAM_GPIO_PIN_SET_46_TO_57_CS_0)
#define PSRAM_CHIP_SELECT  (CHIP_ZERO)
#define PSRAM_BASE_ADDRESS (0x0A000000)
#elif (PSRAM_GPIO_PIN_SET_SEL == PSRAM_GPIO_PIN_SET_46_TO_57_CS_1)
#define PSRAM_CHIP_SELECT  (CHIP_ONE)
#define PSRAM_BASE_ADDRESS (0x0B000000)
#endif

// Resolving Ports, Pins, Muxes & Pads based on Selected Pin Set
#if (PSRAM_GPIO_PIN_SET_SEL == PSRAM_GPIO_PIN_SET_0_TO_5)

#define M4SS_PSRAM_CLK_PORT (0)
#define M4SS_PSRAM_CLK_PIN  (0)
#define M4SS_PSRAM_CLK_MUX  (2)
#define M4SS_PSRAM_CLK_PAD  (0)

#define M4SS_PSRAM_CSN_PORT (0)
#define M4SS_PSRAM_CSN_PIN  (1)
#define M4SS_PSRAM_CSN_MUX  (2)
#define M4SS_PSRAM_CSN_PAD  (0)

#define M4SS_PSRAM_D0_PORT (0)
#define M4SS_PSRAM_D0_PIN  (2)
#define M4SS_PSRAM_D0_MUX  (2)
#define M4SS_PSRAM_D0_PAD  (0)

#define M4SS_PSRAM_D1_PORT (0)
#define M4SS_PSRAM_D1_PIN  (3)
#define M4SS_PSRAM_D1_MUX  (2)
#define M4SS_PSRAM_D1_PAD  (0)

#define M4SS_PSRAM_D2_PORT (0)
#define M4SS_PSRAM_D2_PIN  (4)
#define M4SS_PSRAM_D2_MUX  (2)
#define M4SS_PSRAM_D2_PAD  (0)

#define M4SS_PSRAM_D3_PORT (0)
#define M4SS_PSRAM_D3_PIN  (5)
#define M4SS_PSRAM_D3_MUX  (2)
#define M4SS_PSRAM_D3_PAD  (0)

#define NUM_OF_PSRAM_PINS (6)

#elif (PSRAM_GPIO_PIN_SET_SEL == PSRAM_GPIO_PIN_SET_46_TO_51_CS_0) \
  || (PSRAM_GPIO_PIN_SET_SEL == PSRAM_GPIO_PIN_SET_46_TO_51_CS_1)  \
  || (PSRAM_GPIO_PIN_SET_SEL == PSRAM_GPIO_PIN_SET_46_TO_57_CS_0)  \
  || (PSRAM_GPIO_PIN_SET_SEL == PSRAM_GPIO_PIN_SET_46_TO_57_CS_1)
#define M4SS_PSRAM_CLK_PORT (2)
#define M4SS_PSRAM_CLK_PIN  (14)
#define M4SS_PSRAM_CLK_MUX  (11)
#define M4SS_PSRAM_CLK_PAD  (10)

#if (PSRAM_GPIO_PIN_SET_SEL == PSRAM_GPIO_PIN_SET_46_TO_51_CS_0) \
  || (PSRAM_GPIO_PIN_SET_SEL == PSRAM_GPIO_PIN_SET_46_TO_57_CS_0)
#define M4SS_PSRAM_CSN_PORT (3)
#define M4SS_PSRAM_CSN_PIN  (01)
#define M4SS_PSRAM_CSN_MUX  (11)
#define M4SS_PSRAM_CSN_PAD  (13)
#elif (PSRAM_GPIO_PIN_SET_SEL == PSRAM_GPIO_PIN_SET_46_TO_51_CS_1) \
  || (PSRAM_GPIO_PIN_SET_SEL == PSRAM_GPIO_PIN_SET_46_TO_57_CS_1)
#define M4SS_PSRAM_CSN_PORT (3)
#define M4SS_PSRAM_CSN_PIN  (05)
#define M4SS_PSRAM_CSN_MUX  (11)
#define M4SS_PSRAM_CSN_PAD  (17)
#endif

#define M4SS_PSRAM_D0_PORT (2)
#define M4SS_PSRAM_D0_PIN  (15)
#define M4SS_PSRAM_D0_MUX  (11)
#define M4SS_PSRAM_D0_PAD  (11)

#define M4SS_PSRAM_D1_PORT (3)
#define M4SS_PSRAM_D1_PIN  (0)
#define M4SS_PSRAM_D1_MUX  (11)
#define M4SS_PSRAM_D1_PAD  (12)

#define M4SS_PSRAM_D2_PORT (3)
#define M4SS_PSRAM_D2_PIN  (2)
#define M4SS_PSRAM_D2_MUX  (11)
#define M4SS_PSRAM_D2_PAD  (14)

#define M4SS_PSRAM_D3_PORT (3)
#define M4SS_PSRAM_D3_PIN  (3)
#define M4SS_PSRAM_D3_MUX  (11)
#define M4SS_PSRAM_D3_PAD  (15)

#if (PSRAM_GPIO_PIN_SET_SEL == PSRAM_GPIO_PIN_SET_46_TO_57_CS_0) \
  || (PSRAM_GPIO_PIN_SET_SEL == PSRAM_GPIO_PIN_SET_46_TO_57_CS_1)

#define M4SS_PSRAM_D4_PORT (3)
#define M4SS_PSRAM_D4_PIN  (6)
#define M4SS_PSRAM_D4_MUX  (11)
#define M4SS_PSRAM_D4_PAD  (11)

#define M4SS_PSRAM_D5_PORT (3)
#define M4SS_PSRAM_D5_PIN  (7)
#define M4SS_PSRAM_D5_MUX  (11)
#define M4SS_PSRAM_D5_PAD  (12)

#define M4SS_PSRAM_D6_PORT (3)
#define M4SS_PSRAM_D6_PIN  (8)
#define M4SS_PSRAM_D6_MUX  (11)
#define M4SS_PSRAM_D6_PAD  (14)

#define M4SS_PSRAM_D7_PORT (3)
#define M4SS_PSRAM_D7_PIN  (9)
#define M4SS_PSRAM_D7_MUX  (11)
#define M4SS_PSRAM_D7_PAD  (15)

#endif

#if (PSRAM_GPIO_PIN_SET_SEL == PSRAM_GPIO_PIN_SET_46_TO_51_CS_0) \
  || (PSRAM_GPIO_PIN_SET_SEL == PSRAM_GPIO_PIN_SET_46_TO_51_CS_1)
#define NUM_OF_PSRAM_PINS (6)
#else
#define NUM_OF_PSRAM_PINS (10)
#endif

#elif (PSRAM_GPIO_PIN_SET_SEL == PSRAM_GPIO_PIN_SET_52_TO_57)

#define M4SS_PSRAM_CLK_PORT (3)
#define M4SS_PSRAM_CLK_PIN  (4)
#define M4SS_PSRAM_CLK_MUX  (12)
#define M4SS_PSRAM_CLK_PAD  (16)

#define M4SS_PSRAM_CSN_PORT (3)
#define M4SS_PSRAM_CSN_PIN  (7)
#define M4SS_PSRAM_CSN_MUX  (12)
#define M4SS_PSRAM_CSN_PAD  (19)

#define M4SS_PSRAM_D0_PORT (3)
#define M4SS_PSRAM_D0_PIN  (5)
#define M4SS_PSRAM_D0_MUX  (12)
#define M4SS_PSRAM_D0_PAD  (17)

#define M4SS_PSRAM_D1_PORT (3)
#define M4SS_PSRAM_D1_PIN  (6)
#define M4SS_PSRAM_D1_MUX  (12)
#define M4SS_PSRAM_D1_PAD  (18)

#define M4SS_PSRAM_D2_PORT (3)
#define M4SS_PSRAM_D2_PIN  (8)
#define M4SS_PSRAM_D2_MUX  (12)
#define M4SS_PSRAM_D2_PAD  (20)

#define M4SS_PSRAM_D3_PORT (3)
#define M4SS_PSRAM_D3_PIN  (9)
#define M4SS_PSRAM_D3_MUX  (12)
#define M4SS_PSRAM_D3_PAD  (21)

#define NUM_OF_PSRAM_PINS (6)

#endif

#endif //__SL_SI91X_PSRAM_HANDLE_H_