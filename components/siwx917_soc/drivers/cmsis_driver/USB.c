/* -------------------------------------------------------------------------- 
 * Copyright (c) 2013-2018 ARM Limited. All rights reserved.
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
 * Project:      Common file for both USB(Device and Host) 
 * -------------------------------------------------------------------------- */

/* History:
 *  Version 1.0
 *    Initial release
 */

#include "rsi_ccp_user_config.h"
#if defined(CHIP_9118)

#include "rsi_chip.h"

#include "Driver_USB.h"
#include "USB.h"
#include "RTE_Device.h"
#include "RTE_Components.h"

volatile uint8_t USB_role  = ARM_USB_ROLE_NONE;
volatile uint8_t USB_state = 0U;

#ifdef RTE_Drivers_USBH
extern void USBH0_IRQ(void);
#endif
#ifdef RTE_Drivers_USBD
extern void USBD0_IRQ(void);
#endif

// Common IRQ Routine **********************************************************

/**
  \fn          void IRQ073_Handler (void)
  \brief       USB Interrupt Routine (IRQ).
*/
void IRQ073_Handler(void)
{
#if (defined(RTE_Drivers_USBH) && defined(RTE_Drivers_USBD))
  switch (USB_role) {
#ifdef RTE_Drivers_USBH
    case ARM_USB_ROLE_HOST:
      USBH_IRQ();
      break;
#endif
#ifdef RTE_Drivers_USBD
    case ARM_USB_ROLE_DEVICE:
      USBD_IRQ();
      break;
#endif
    default:
      break;
  }
#else
#ifdef RTE_Drivers_USBH
  USBH_IRQ();
#else
  USBD_IRQ();
#endif
#endif
}
#endif
