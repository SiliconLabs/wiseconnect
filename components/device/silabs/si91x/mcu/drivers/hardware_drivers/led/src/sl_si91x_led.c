/***************************************************************************/ /**
 * @file sl_si91x_led.c
 * @brief LED manipulation routines; stack and example APIs
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
#include "sl_si91x_led.h"
#include "si91x_device.h"
#include "rsi_rom_egpio.h"

void sl_si91x_led_init(const sl_led_t *handle)
{
#if ((defined(SLI_SI91X_MCU_CONFIG_RADIO_BOARD_BASE_VER)) && (defined(SLI_SI91X_MCU_CONFIG_RADIO_BOARD_VER2)))
  if (handle->led_number == 0U) {
    /*Set the GPIO pin MUX */
    RSI_EGPIO_SetPinMux(EGPIO1, handle->port, handle->pin, 0);
    /*Set GPIO direction*/
    RSI_EGPIO_SetDir(EGPIO1, handle->port, handle->pin, 0);
  } else {
    RSI_EGPIO_PadSelectionEnable(5);
    /*Set the GPIO pin MUX */
    RSI_EGPIO_SetPinMux(EGPIO, handle->port, handle->pin, 0);
    /*Set GPIO direction*/
    RSI_EGPIO_SetDir(EGPIO, handle->port, handle->pin, 0);
  }
#elif SLI_SI91X_MCU_CONFIG_RADIO_BOARD_BASE_VER
  if (handle->led_number == 0U) {
    RSI_EGPIO_PadSelectionEnable(5);
    /*Set the GPIO pin MUX */
    RSI_EGPIO_SetPinMux(EGPIO, handle->port, handle->pin, 0);
    /*Set GPIO direction*/
    RSI_EGPIO_SetDir(EGPIO, handle->port, handle->pin, 0);
  } else {
    /*Set the GPIO pin MUX */
    RSI_EGPIO_SetPinMux(EGPIO1, handle->port, handle->pin, 0);
    /*Set GPIO direction*/
    RSI_EGPIO_SetDir(EGPIO1, handle->port, handle->pin, 0);
  }
#endif
}

void sl_si91x_led_set(uint8_t pin)
{
#if ((SLI_SI91X_MCU_CONFIG_RADIO_BOARD_BASE_VER) && (SLI_SI91X_MCU_CONFIG_RADIO_BOARD_VER2))
  if (pin == SL_LED_LED0_PIN)
    RSI_EGPIO_SetPin(EGPIO1, SL_LED_LED0_PORT, pin, 1);
  else
    RSI_EGPIO_SetPin(EGPIO, SL_LED_LED1_PORT, pin, 1);
#elif SLI_SI91X_MCU_CONFIG_RADIO_BOARD_BASE_VER
  if (pin == SL_LED_LED0_PIN)
    RSI_EGPIO_SetPin(EGPIO, SL_LED_LED0_PORT, pin, 1);
  else
    RSI_EGPIO_SetPin(EGPIO1, SL_LED_LED1_PORT, pin, 1);
#endif
}

void sl_si91x_led_clear(uint8_t pin)
{
#if ((SLI_SI91X_MCU_CONFIG_RADIO_BOARD_BASE_VER) && (SLI_SI91X_MCU_CONFIG_RADIO_BOARD_VER2))
  if (pin == SL_LED_LED0_PIN)
    RSI_EGPIO_SetPin(EGPIO1, SL_LED_LED0_PORT, pin, 0);
  else
    RSI_EGPIO_SetPin(EGPIO, SL_LED_LED1_PORT, pin, 0);
#elif SLI_SI91X_MCU_CONFIG_RADIO_BOARD_BASE_VER
  if (pin == SL_LED_LED0_PIN)
    RSI_EGPIO_SetPin(EGPIO, SL_LED_LED0_PORT, pin, 0);
  else
    RSI_EGPIO_SetPin(EGPIO1, SL_LED_LED1_PORT, pin, 0);
#endif
}

void sl_si91x_led_toggle(uint8_t pin)
{
#if ((SLI_SI91X_MCU_CONFIG_RADIO_BOARD_BASE_VER) && (SLI_SI91X_MCU_CONFIG_RADIO_BOARD_VER2))
  if (pin == SL_LED_LED0_PIN)
    RSI_EGPIO_TogglePort(EGPIO1, SL_LED_LED0_PORT, (1 << pin));
  else
    RSI_EGPIO_TogglePort(EGPIO, SL_LED_LED1_PORT, (1 << pin));
#elif SLI_SI91X_MCU_CONFIG_RADIO_BOARD_BASE_VER
  if (pin == SL_LED_LED0_PIN)
    RSI_EGPIO_TogglePort(EGPIO, SL_LED_LED0_PORT, (1 << pin));
  else
    RSI_EGPIO_TogglePort(EGPIO1, SL_LED_LED1_PORT, (1 << pin));
#endif
}
