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
 * $Date:        01. Oct 2018
 * $Revision:    V1.0
 *
 * Driver:       Driver_USBH0_HCI
 * Configured:   via RTE_Device.h configuration file
 * Project:      USB Host 0 HCI Controller (EHCI) Driver for Silicon Labs MCU
 * --------------------------------------------------------------------------
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 *
 *   Configuration Setting                  Value
 *   ---------------------                  -----
 *   Connect to hardware via Driver_USBH# = 0
 *   USB Host controller interface        = EHCI
 * -------------------------------------------------------------------------- */

/* History:
 *  Version 1.0
 *    - Initial CMSIS Driver API V5.4.0 release
 */
#include "rsi_ccp_user_config.h"
#if defined(CHIP_9118)
#include "Driver_USBH.h"

#include "rsi_chip.h"
#include "USB.h"
#include "RTE_Device.h"
#include "RTE_Components.h"

extern uint8_t USB_role;
extern uint8_t USB_state;

#define M4SS_CLK_PWR_CTRL_BASE_ADDR 0x46000000
#define USB_SYSCLK_CLKCLNR_ON       (1 << 23)
#define M4SS_CLOCK_CONFIG_REG4      *(volatile uint32_t *)(M4SS_CLK_PWR_CTRL_BASE_ADDR + 0x24)
#define M4SS_CLOCK_CONFIG_REG5      *(volatile uint32_t *)(M4SS_CLK_PWR_CTRL_BASE_ADDR + 0x28)

#define M4SS_MISC_REG_BASE 0x46008000

#define MISC_USB_CONFIG_REG       *((volatile uint32_t *)(M4SS_MISC_REG_BASE + 0x1C))
#define NWPAON_ACCESS_CTRL_CLEAR  *((volatile uint32_t *)(NWPAON_MEM_HOST_ACCESS_CTRL_CLEAR))
#define MISC_CFG_RST_LATCH_STATUS *((volatile uint32_t *)(M4SS_MISC_REG_BASE + 0x10))
#define MISC_CFG_HOST_CTRL        *((volatile uint32_t *)(M4SS_MISC_REG_BASE + 0x0C))

#define MISC_USB_SET_REG1   *((volatile uint32_t *)(M4SS_MISC_REG_BASE + 0xF0))
#define MISC_USB_CLEAR_REG1 *((volatile uint32_t *)(M4SS_MISC_REG_BASE + 0xF4))

// USBH EHCI Driver ************************************************************

#define ARM_USBH_EHCI_DRIVER_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)

// Driver Version
static const ARM_DRIVER_VERSION usbh_ehci_driver_version = { ARM_USBH_API_VERSION, ARM_USBH_EHCI_DRIVER_VERSION };

// Driver Capabilities
static const ARM_USBH_HCI_CAPABILITIES usbh_ehci_driver_capabilities = {
  0x0001U // Root HUB available Ports Mask
};

static ARM_USBH_HCI_Interrupt_t EHCI_IRQ;

/**
 * @fn           void RSI_TimerDelayUs(uint32_t delay_ms)
 * @brief        This API is used create delay in  micro seconds.              
 * @param[in]    delay_ms  : timer delay in micro seconds
 * @return       None              
 */
void RSI_DelayUs(uint32_t delay_us)
{
  /**/
  RSI_ULPSS_TimerClkConfig(ULPCLK, ENABLE_STATIC_CLK, 0, ULP_TIMER_32MHZ_RC_CLK, 0);

  /* Sets periodic mode */
  RSI_TIMERS_SetTimerMode(TIMERS, ONESHOT_TIMER, TIMER_0);

  /* Sets timer in 1 Micro second mode */
  RSI_TIMERS_SetTimerType(TIMERS, MICRO_SEC_MODE, TIMER_0);

  /* 1 Micro second timer configuration */
  RSI_TIMERS_MicroSecTimerConfig(TIMERS, TIMER_0, 32, 0, MICRO_SEC_MODE);

  RSI_TIMERS_SetMatch(TIMERS, TIMER_0, delay_us);

  RSI_TIMERS_TimerStart(TIMERS, TIMER_0);

  while (!RSI_TIMERS_InterruptStatus(TIMERS, TIMER_0))
    ;
}

// USBH EHCI Driver functions

/**
 * @fn          ARM_DRIVER_VERSION USBH_HCI_GetVersion (void)
 * @brief       Get USB Host HCI (OHCI/EHCI) driver version.
 * @return      \ref ARM_DRIVER_VERSION
*/
static ARM_DRIVER_VERSION USBH_HCI_GetVersion(void)
{
  return usbh_ehci_driver_version;
}

/**
 * @fn          ARM_USBH_HCI_CAPABILITIES USBH_HCI_GetCapabilities (void)
 * @brief       Get driver capabilities.
 * @return      \ref ARM_USBH_HCI_CAPABILITIES
*/
static ARM_USBH_HCI_CAPABILITIES USBH_HCI_GetCapabilities(void)
{
  return usbh_ehci_driver_capabilities;
}

/**
 * @fn          int32_t USBH_HCI_Initialize (ARM_USBH_HCI_Interrupt_t cb_interrupt)
 * @brief       Initialize USB Host HCI (OHCI/EHCI) Interface.
 * @param[in]   cb_interrupt : Pointer to Interrupt Handler Routine
 * @return      \ref execution_status
*/
static int32_t USBH_HCI_Initialize(ARM_USBH_HCI_Interrupt_t cb_interrupt)
{

  if ((USB_state & USBH_DRIVER_INITIALIZED) != 0U) {
    return ARM_DRIVER_OK;
  }

  EHCI_IRQ = cb_interrupt;

  USB_role = ARM_USB_ROLE_HOST;

  /*USB_PinsConfigure*/
  RSI_EGPIO_SetPinMux(EGPIO, RTE_USB_OTG_PORT, RTE_USB_OTG_PIN, EGPIO_PIN_MUX_MODE0);

  RSI_EGPIO_SetDir(EGPIO, RTE_USB_OTG_PORT, RTE_USB_OTG_PIN, EGPIO_CONFIG_DIR_OUTPUT);

  RSI_EGPIO_SetPin(EGPIO, RTE_USB_OTG_PORT, RTE_USB_OTG_PIN, RTE_USB_OTG_STATUS_HIGH);

  USB_state = USBH_DRIVER_INITIALIZED;

  return ARM_DRIVER_OK;
}

/**
 * @fn          int32_t USBH_HCI_Uninitialize (void)
 * @brief       De-initialize USB Host HCI (OHCI/EHCI) Interface.
 * @return      \ref execution_status
*/
static int32_t USBH_HCI_Uninitialize(void)
{

  USB_role = ARM_USB_ROLE_NONE;
  USB_state &= ~USBH_DRIVER_INITIALIZED;

  /*Uninitialize the USB pin configuration*/
  RSI_EGPIO_SetPin(EGPIO, RTE_USB_OTG_MODE, RTE_USB_OTG_PIN, RTE_USB_OTG_STATUS_LOW);

  return ARM_DRIVER_OK;
}

/**
 * @fn          int32_t USBH_HCI_PowerControl (ARM_POWER_STATE state)
 * @brief       Control USB Host HCI (OHCI/EHCI) Interface Power.
 * @param[in]   state : Power state
 * @return      \ref execution_status
*/
static int32_t USBH_HCI_PowerControl(ARM_POWER_STATE state)
{

  switch (state) {
    case ARM_POWER_OFF:
      NVIC_DisableIRQ(USB_IRQn);      // Disable interrupt
      NVIC_ClearPendingIRQ(USB_IRQn); // Clear pending interrupt
                                      /*USB Host detection disable through software */
      MISC_CFG_HOST_CTRL |= (BIT(14) & (~BIT(12)));
      USB_state &= ~USBH_DRIVER_POWERED; // Clear powered flag
      break;

    case ARM_POWER_FULL:
      if ((USB_state & USBH_DRIVER_INITIALIZED) == 0U) {
        return ARM_DRIVER_ERROR;
      }
      if ((USB_state & USBH_DRIVER_POWERED) != 0U) {
        return ARM_DRIVER_OK;
      }

      /*USB configuration*/
      MISC_USB_CONFIG_REG = 0x11;
      /*USB Host detection through software */
      MISC_CFG_HOST_CTRL |= (BIT(14) | BIT(12));
      /*USB in m4*/
      NWPAON_ACCESS_CTRL_CLEAR = BIT(4);
      /*USB is in soft reset*/
      MISC_USB_SET_REG1 = BIT(0);
      /*100ms delay*/
      RSI_DelayUs(100000);
      /*USB soft reset release after 100ms*/
      MISC_USB_CLEAR_REG1 = BIT(0);

      USB_state |= USBH_DRIVER_POWERED; // Set powered flag
      NVIC_EnableIRQ(USB_IRQn);

      break;

    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  return ARM_DRIVER_OK;
}

/**
 * @fn          int32_t USBH_HCI_PortVbusOnOff (uint8_t port, bool vbus)
 * @brief       USB Host HCI (OHCI/EHCI) Root HUB Port VBUS on/off.
 * @param[in]   port : Root HUB Port Number
 * @param[in]   vbus :
                - \b false VBUS off
                - \b true  VBUS on
  \return      \ref execution_status
*/
static int32_t USBH_HCI_PortVbusOnOff(uint8_t port, bool power)
{

  /*VBUS signal is controlled by ehci only*/
  if (((1U << port) & usbh_ehci_driver_capabilities.port_mask) == 0U) {
    return ARM_DRIVER_ERROR;
  }
  return ARM_DRIVER_OK;
}

/**
 * @fn          void USBH0_IRQ (void)
 * @brief       USB0 Host Interrupt Routine (IRQ).
*/
void USBH_IRQ(void)
{
  EHCI_IRQ();
}

ARM_DRIVER_USBH_HCI Driver_USBH0_HCI = { USBH_HCI_GetVersion,   USBH_HCI_GetCapabilities, USBH_HCI_Initialize,
                                         USBH_HCI_Uninitialize, USBH_HCI_PowerControl,    USBH_HCI_PortVbusOnOff };
#endif
