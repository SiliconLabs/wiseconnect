/***************************************************************************/ /**
 * @file sl_si91x_rgb_led_config.h
 * @brief Led Driver Configuration
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************/

#ifndef SL_SI91X_RGB_LED_CONFIG_H
#define SL_SI91X_RGB_LED_CONFIG_H

#include "RTE_Device_917.h"

#define SL_SI91x_LED_COUNT 2

#define SL_LED_RED_PIN    RTE_LEDR_PIN
#define SL_LED_RED_PORT   RTE_LEDR_PORT
#define SL_LED_RED_NUMBER RTE_LEDR_NUMBER
#define SL_LED_RED_PAD    RTE_LEDR_PAD

#define SL_LED_GREEN_PIN    RTE_LEDG_PIN
#define SL_LED_GREEN_PORT   RTE_LEDG_PORT
#define SL_LED_GREEN_NUMBER RTE_LEDG_NUMBER
#define SL_LED_GREEN_PAD    RTE_LEDG_PAD

#define SL_LED_BLUE_PIN    RTE_LEDB_PIN
#define SL_LED_BLUE_PORT   RTE_LEDB_PORT
#define SL_LED_BLUE_NUMBER RTE_LEDB_NUMBER
#define SL_LED_BLUE_PAD    RTE_LEDB_PAD

#endif // SL_SI91X_LED_CONFIG_H
