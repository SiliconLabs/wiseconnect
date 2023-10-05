/***************************************************************************/ /**
 * @file sl_si91x_button.c
 * @brief  Sample API functions for operating buttons (external interrupts).
 *  Please see the board header file for the definition of the BUTTON*
 *  instances used below.  The BUTTON* defines used here are designed to make
 *  this library generic such that only the board header needs to be modified
 *  to change which GPIO the buttons are on.
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

#include "rsi_chip.h"
#include "rsi_board.h"
#include "sl_common.h"
#include "sl_si91x_button.h"
#include "sl_si91x_button_config.h"
#include "si91x_device.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/
#define PININT2_NVIC_NAME         NPSS_TO_MCU_GPIO_INTR_IRQn /* External interrupt number */
#define NPSS_GPIO_IRQHandler      IRQ021_Handler             /* UULP GPIO interrupt IRQ function name   */
#define PININT_IRQ_HANDLER        IRQ059_Handler             /* GPIO interrupt IRQ function name            */
#define PININT_NVIC_NAME          EGPIO_PIN_7_IRQn           /* GPIO interrupt NVIC interrupt name          */
#define PIN_INT                   7                          /* Pin  interrupt number(0 to 7)               */
#define SL_SI91x_MAX_BUTTON_COUNT SL_SI91x_BUTTON_COUNT

// This stores the button state so that IRQ ISRs know when to notify buttonIsrs.
#if (SL_SI91x_BUTTON_COUNT > 0)
static int8_t buttonState[SL_SI91x_BUTTON_COUNT];
void sl_si91x_button_internal_isr(uint8_t pin);
#endif //(SL_SI91x_BUTTON_COUNT > 0)

/**
 * @brief Interrupt handler for UULP GPIO button
 * @return  Nothing
 */
void NPSS_GPIO_IRQHandler(void)
{
  volatile uint32_t intrStatus = 0;

  intrStatus = RSI_NPSSGPIO_GetIntrStatus();

  if (intrStatus & NPSS_GPIO_2_INTR) {
    RSI_NPSSGPIO_ClrIntr(NPSS_GPIO_2_INTR);
#ifdef SI917_RADIO_BOARD_V2
    sl_si91x_button_internal_isr(SL_BUTTON_BTN0_PIN);
#else // V1 Boards
    sl_si91x_button_internal_isr(SL_BUTTON_BTN1_PIN);
#endif
  }

  if (intrStatus & NPSS_GPIO_0_INTR) {
    RSI_NPSSGPIO_ClrIntr(NPSS_GPIO_0_INTR);
    sl_si91x_button_internal_isr(SL_BUTTON_BTN0_PIN);
  }
}

/**
 * @brief Interrupt handler for M4 GPIO mapped button
 * @return  Nothing
 */
void PININT_IRQ_HANDLER(void)
{
  uint32_t intrStatus;

  /*get interrupt status*/
  intrStatus = RSI_EGPIO_GetIntStat(EGPIO, PIN_INT);

  if ((intrStatus & EGPIO_PIN_INT_CLR_RISING) || (intrStatus & EGPIO_PIN_INT_CLR_FALLING)) {
    /*clear interrupt*/
    RSI_EGPIO_IntClr(EGPIO, PIN_INT, INTERRUPT_STATUS_CLR);
  } else {
    RSI_EGPIO_IntMask(EGPIO, PIN_INT);
  }
  sl_si91x_button_internal_isr(SL_BUTTON_BTN1_PIN);
}

void sl_si91x_button_init(const sl_button_t *handle)
{
#if ((defined(SI917_RADIO_BOARD) && (SI917_RADIO_BOARD == 1)) \
     && (defined(SI917_RADIO_BOARD_V2) && (SI917_RADIO_BOARD_V2 == 1)))
  if (handle->button_number == 0U) {
    RSI_NPSSGPIO_InputBufferEn(handle->pin, 1);

    /* Set the NPSS GPIO pin MUX */
    RSI_NPSSGPIO_SetPinMux(handle->pin, 0);

    /* Set the direction of the NPSS GPIO */
    RSI_NPSSGPIO_SetDir(handle->pin, NPSS_GPIO_DIR_INPUT);

    /* Enables rise edge interrupt detection for UULP_VBAT_GPIO_2 */
    RSI_NPSSGPIO_SetIntRiseEdgeEnable(NPSS_GPIO_2_INTR);

    /* Enables fall edge interrupt detection for UULP_VBAT_GPIO_2 */
    RSI_NPSSGPIO_SetIntFallEdgeEnable(NPSS_GPIO_2_INTR);

    /*Clear interrupt to eliminate false interrupts after unmasking*/
    RSI_NPSSGPIO_ClrIntr(NPSS_GPIO_2_INTR);

    /* un mask the NPSS GPIO interrupt */
    RSI_NPSSGPIO_IntrUnMask(NPSS_GPIO_2_INTR);

    /*NVIC enable */
    NVIC_EnableIRQ(PININT2_NVIC_NAME);
  }

  if (handle->button_number == 1U) {
    /*Enable clock for EGPIO module*/
    RSI_CLK_PeripheralClkEnable(M4CLK, EGPIO_CLK, ENABLE_STATIC_CLK);

    /*PAD selection*/
    RSI_EGPIO_PadSelectionEnable(SL_BUTTON_BTN1_PAD);

    RSI_EGPIO_SetDir(EGPIO, handle->port, handle->pin, 1);

    /*REN enable */
    RSI_EGPIO_PadReceiverEnable(handle->pin);

    /*Configure default GPIO mode(0) */
    RSI_EGPIO_SetPinMux(EGPIO, handle->port, handle->pin, EGPIO_PIN_MUX_MODE0);

    /*Selects the pin interrupt for the GPIO*/
    RSI_EGPIO_PinIntSel(EGPIO, PIN_INT, handle->port, handle->pin);

    /* Set Rise Edge interrupt*/
    RSI_EGPIO_SetIntRiseEdgeEnable(EGPIO, PIN_INT);

    /* Set Fall Edge interrupt*/
    RSI_EGPIO_SetIntFallEdgeEnable(EGPIO, PIN_INT);

    /*Clear interrupt to eliminate false interrupts after unmasking*/
    RSI_EGPIO_IntClr(EGPIO, PIN_INT, INTERRUPT_STATUS_CLR);

    /*Unmask the  interrupt*/
    RSI_EGPIO_IntUnMask(EGPIO, PIN_INT);

    /*NVIC enable */
    NVIC_EnableIRQ(PININT_NVIC_NAME);
  }
#elif SI917_RADIO_BOARD
  RSI_NPSSGPIO_InputBufferEn(handle->pin, 1);

  /* Set the NPSS GPIO pin MUX */
  RSI_NPSSGPIO_SetPinMux(handle->pin, 0);

  /* Set the direction of the NPSS GPIO */
  RSI_NPSSGPIO_SetDir(handle->pin, NPSS_GPIO_DIR_INPUT);

  if (handle->button_number == 0U) {
    /* Enables rise edge interrupt detection for UULP_VBAT_GPIO_2 */
    RSI_NPSSGPIO_SetIntRiseEdgeEnable(NPSS_GPIO_0_INTR);

    /* Enables fall edge interrupt detection for UULP_VBAT_GPIO_2 */
    RSI_NPSSGPIO_SetIntFallEdgeEnable(NPSS_GPIO_0_INTR);

    /*Clear interrupt to eliminate false interrupts after unmasking*/
    RSI_NPSSGPIO_ClrIntr(NPSS_GPIO_0_INTR);

    /* un mask the NPSS GPIO interrupt */
    RSI_NPSSGPIO_IntrUnMask(NPSS_GPIO_0_INTR);
  } else {
    /* Enables rise edge interrupt detection for UULP_VBAT_GPIO_2 */
    RSI_NPSSGPIO_SetIntRiseEdgeEnable(NPSS_GPIO_2_INTR);

    /* Enables fall edge interrupt detection for UULP_VBAT_GPIO_2 */
    RSI_NPSSGPIO_SetIntFallEdgeEnable(NPSS_GPIO_2_INTR);

    /*Clear interrupt to eliminate false interrupts after unmasking*/
    RSI_NPSSGPIO_ClrIntr(NPSS_GPIO_2_INTR);

    /* un mask the NPSS GPIO interrupt */
    RSI_NPSSGPIO_IntrUnMask(NPSS_GPIO_2_INTR);
  }

  /*NVIC enable */
  NVIC_EnableIRQ(PININT2_NVIC_NAME);
#endif
}

int8_t sl_si91x_button_state_get(uint8_t pin)
{
  // Note: this returns the "soft" state rather than reading the port
  //  so it gives with the interrupts and their callbacks
  if (pin == SL_BUTTON_BTN0_PIN) {
    return buttonState[SL_BUTTON_BTN0_NUMBER];
  } else if (pin == SL_BUTTON_BTN1_PIN) {
    return buttonState[SL_BUTTON_BTN1_NUMBER];
  }
  //if no matching button was found
  return BUTTON_STATE_INVALID;
}

void sl_si91x_button_state_set(uint8_t pin, int8_t state)
{
  // Note: this sets the "soft" state
  //  so it gives with the interrupts and their callbacks
  uint8_t index = 0;

  if (pin == SL_BUTTON_BTN0_PIN) {
    index = SL_BUTTON_BTN0_NUMBER;
  } else if (pin == SL_BUTTON_BTN1_PIN) {
    index = SL_BUTTON_BTN1_NUMBER;
  }

  if (index < SL_SI91x_MAX_BUTTON_COUNT) {
    buttonState[index] = state;
  }
}

void sl_si91x_button_state_toggle(uint8_t pin)
{
  // Note: this toggles the "soft" state
  //  so it gives with the interrupts and their callbacks
  uint8_t index = 0;

  if (pin == SL_BUTTON_BTN0_PIN) {
    index = SL_BUTTON_BTN0_NUMBER;
  } else if (pin == SL_BUTTON_BTN1_PIN) {
    index = SL_BUTTON_BTN1_NUMBER;
  }

  if (index < SL_SI91x_MAX_BUTTON_COUNT) {
    buttonState[index] = !buttonState[index];
  }
}

int8_t sl_si91x_button_pin_state(uint8_t pin)
{
  if (pin == SL_BUTTON_BTN1_PIN)
    return RSI_EGPIO_GetPin(EGPIO, SL_BUTTON_BTN1_PORT, pin) ? BUTTON_RELEASED : BUTTON_PRESSED;
  else
    return RSI_NPSSGPIO_GetPin(pin) ? BUTTON_RELEASED : BUTTON_PRESSED;
}

/***************************************************************************/ /**
*
* DEBOUNCE operation is based upon the theory that when multiple reads in a row
* return the same value, we have passed any debounce created by the mechanical
* action of a button.  The define "DEBOUNCE" says how many reads in a row
* should return the same value.  The value '5', below, is the recommended value
* since this should require the signal to have stabalized for approximately
* 100us which should be much longer than any debounce action.
* Uncomment the following line to enable software debounce operation:
* #define DEBOUNCE 5
*
 ******************************************************************************/

//Typically, software debounce is disabled by defaulting to a value of '0'
//which will cause the preprocessor to strip out the debounce code and save
//flash space.
#ifndef DEBOUNCE
#define DEBOUNCE 0
#endif //DEBOUNCE

#if (SL_SI91x_BUTTON_COUNT > 0)
void sl_si91x_button_internal_isr(uint8_t pin)
{
  int8_t buttonStateNow;

#if (DEBOUNCE > 0)
  uint8_t buttonStatePrev;
  uint32_t debounce;
#endif //(DEBOUNCE > 0)

  buttonStateNow = sl_si91x_button_pin_state(pin);

#if (DEBOUNCE > 0)
  //read button until get "DEBOUNCE" number of consistent readings
  for (debounce = 0; debounce < DEBOUNCE; debounce = (buttonStateNow == buttonStatePrev) ? debounce + 1 : 0) {
    buttonStatePrev = buttonStateNow;
    buttonStateNow  = sl_si91x_button_pin_state(pin);
  }
#endif //(DEBOUNCE > 0)

  if (sl_si91x_button_state_get(pin) != buttonStateNow) {
    //state changed, notify app
    sl_si91x_button_state_set(pin, buttonStateNow);
    sl_si91x_button_isr(pin, sl_si91x_button_state_get(pin));
  } else {
    // state unchanged, then notify app for a double-transition.
    // Invert temporarily for calling ISR.
    sl_si91x_button_state_toggle(pin);
    sl_si91x_button_isr(pin, sl_si91x_button_state_get(pin));
  }
}

SL_WEAK void sl_si91x_button_isr(uint8_t pin, int8_t state)
{
  (void)pin;
  (void)state;
}
#endif // (SL_SI91x_BUTTON_COUNT > 0)
