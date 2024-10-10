/***************************************************************************/ /**
 * @file sl_si91x_led_init_ledb_config.h
 * @brief Led Driver Configuration
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************/

#ifndef SL_SI91X_LED_INIT_LEDB_CONFIG_H
#define SL_SI91X_LED_INIT_LEDB_CONFIG_H

// <<< sl:start pin_tool >>>
// <gpio> SL_SI91X_LED_B
// $[GPIO_SL_SI91X_LED_B]
#ifndef SL_SI91X_LED_B_PORT
#define SL_SI91X_LED_B_PORT HP
#endif
#ifndef SL_SI91X_LED_B_PIN
#define SL_SI91X_LED_B_PIN 15
#endif
// [GPIO_SL_SI91X_LED_B]$
// <<< sl:end pin_tool >>>

#define SL_LED_LEDB_PIN    SL_SI91X_LED_B_PIN
#define SL_LED_LEDB_PORT   RTE_LEDB_PORT
#define SL_LED_LEDB_NUMBER RTE_LEDB_NUMBER

#endif // SL_SI91X_LED_INIT_LEDB_CONFIG_H
