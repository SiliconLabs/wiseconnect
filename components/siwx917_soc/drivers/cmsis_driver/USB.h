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
 * $Date:        12. Dec 2018
 * $Revision:    V1.0
 *
 * Project:      USB Driver Definitions for Silicon Labs MCU
 * -------------------------------------------------------------------------- */

/* History:
 *  Version 1.0
 *    - Initial release
 */

#ifndef __USB_H
#define __USB_H

#include <stdint.h>

#ifndef USB_ENDPT_MSK
#define USB_ENDPT_MSK (0x3FU)
#endif

// USB Device Command Register
#define USB_USBCMD_D_RS      (1U)
#define USB_USBCMD_D_RST     (1U << 1U)
#define USB_USBCMD_D_SUTW    (1U << 13U)
#define USB_USBCMD_D_ATDTW   (1U << 14U)
#define USB_USBCMD_D_ITC_POS (16U)
#define USB_USBCMD_D_ITC_MSK (0xFFU << USB_USBCMD_D_ITC_POS)
#define USB_USBCMD_D_ITC(n)  (((n) << USB_USBCMD_D_ITC_POS) & USB_USBCMD_D_ITC_MSK)

// USB Device Status Register
#define USB_USBSTS_D_UI   (1U)
#define USB_USBSTS_D_UEI  (1U << 1U)
#define USB_USBSTS_D_PCI  (1U << 2U)
#define USB_USBSTS_D_URI  (1U << 6U)
#define USB_USBSTS_D_SRI  (1U << 7U)
#define USB_USBSTS_D_SLI  (1U << 8U)
#define USB_USBSTS_D_NAKI (1U << 16U)

// USB Device Interrupt Register
#define USB_USBINTR_D_UE   (1U)
#define USB_USBINTR_D_UEE  (1U << 1U)
#define USB_USBINTR_D_PCE  (1U << 2U)
#define USB_USBINTR_D_URE  (1U << 6U)
#define USB_USBINTR_D_SRE  (1U << 7U)
#define USB_USBINTR_D_SLE  (1U << 8U)
#define USB_USBINTR_D_NAKE (1U << 16U)

// USB Device Frame Index Register
#define USB_FRINDEX_D_FRINDEX2_0_POS  (0U)
#define USB_FRINDEX_D_FRINDEX2_0_MSK  (7U)
#define USB_FRINDEX_D_FRINDEX13_3_POS (3U)
#define USB_FRINDEX_D_FRINDEX13_3_MSK (0x7FFU << USB_FRINDEX_D_FRINDEX13_3_POS)

// USB Device Address Register
#define USB_DEVICEADDR_USBADRA    (1U << 24U)
#define USB_DEVICEADDR_USBADR_POS (25U)
#define USB_DEVICEADDR_USBADR_MSK (0x7FUL << USB_DEVICEADDR_USBADR_POS)

// USB Endpoint List Address Register
#define USB_ENDPOINTLISTADDR_EPBASE31_11_POS (11U)
#define USB_ENDPOINTLISTADDR_EPBASE_MSK      (0x1FFFFFUL << USB_ENDPOINTLISTADDR_EPBASE31_11_POS)

// USB Burst Size Register
#define USB_BURSTSIZE_RXPBURST_POS (0U)
#define USB_BURSTSIZE_RXPBURST_MSK (0xFFU)
#define USB_BURSTSIZE_TXPBURST_POS (8U)
#define USB_BURSTSIZE_TXPBURST_MSK (0xFFU << USB_BURSTSIZE_TXPBURST_POS)

// USB BInterval Register
#define USB_BINTERVAL_BINT_POS (0U)
#define USB_BINTERVAL_BINT_MSK (0x0FU << USB_BINTERVAL_BINT_POS)

// USB Endpoint NAK Register
#define USB_ENDPTNAK_EPRN_POS (0U)
#define USB_ENDPTNAK_EPRN_MSK (USB_ENDPT_MSK)
#define USB_ENDPTNAK_EPTN_POS (16U)
#define USB_ENDPTNAK_EPTN_MSK (USB_ENDPT_MSK << USB_ENDPTNAK_EPTN_POS)

// USB Endpoint NAK Enable Register
#define USB_ENDPTNAKEN_EPRNE_POS (0U)
#define USB_ENDPTNAKEN_EPRNE_MSK (USB_ENDPT_MSK)
#define USB_ENDPTNAKEN_EPTNE_POS (16U)
#define USB_ENDPTNAKEN_EPTNE_MSK (USB_ENDPT_MSK << USB_ENDPTNAKEN_EPTNE_POS)

// USB Device Port Status and Control Register
#define USB_PORTSC1_D_CCS          (1U)
#define USB_PORTSC1_D_PE           (1U << 2U)
#define USB_PORTSC1_D_PEC          (1U << 3U)
#define USB_PORTSC1_D_FPR          (1U << 6U)
#define USB_PORTSC1_D_SUSP         (1U << 7U)
#define USB_PORTSC1_D_PR           (1U << 8U)
#define USB_PORTSC1_D_HSP          (1U << 9U)
#define USB_PORTSC1_D_PIC15_14_POS (14U)
#define USB_PORTSC1_D_PIC15_14_MSK (3U << USB_PORTSC1_D_PIC15_14_POS)
#define USB_PORTSC1_D_PIC15_14(n)  (((n) << USB_PORTSC1_D_PIC15_14_POS) & USB_PORTSC1_D_PIC15_14_MSK)
#define USB_PORTSC1_D_PTC19_16_POS (16U)
#define USB_PORTSC1_D_PTC19_16_MSK (0x0FU << USB_PORTSC1_D_PTC19_16_POS)
#define USB_PORTSC1_D_PHCD         (1U << 23U)
#define USB_PORTSC1_D_PFSC         (1U << 24U)
#define USB_PORTSC1_D_PSPD_POS     (26U)
#define USB_PORTSC1_D_PSPD_MSK     (3U << USB_PORTSC1_D_PSPD_POS)
#define USB_PORTSC1_D_PTS_POS      (30U)
#define USB_PORTSC1_D_PTS_MSK      (3UL << USB_PORTSC1_D_PTS_POS)
#define USB_PORTSC1_D_PTS(n)       (((n) << USB_PORTSC1_D_PTS_POS) & USB_PORTSC1_D_PTS_MSK)

// USB Device Mode Register
#define USB_USBMODE_D_CM1_0_POS (0U)
#define USB_USBMODE_D_CM1_0_MSK (3U)
#define USB_USBMODE_D_CM1_0(n)  ((n)&USB_USBMODE_D_CM1_0_MSK)
#define USB_USBMODE_D_ES        (1U << 2U)
#define USB_USBMODE_D_SLOM      (1U << 3U)
#define USB_USBMODE_D_SDIS      (1U << 4U)

// USB Endpoint Setup Status Register
#define USB_ENDPTSETUPSTAT_POS (0U)
#define USB_ENDPTSETUPSTAT_MSK (USB_ENDPT_MSK << USB_ENDPTSETUPSTAT_POS)

// USB Endpoint Prime Register
#define USB_ENDPTRPRIME_PERB_POS (0U)
#define USB_ENDPTRPRIME_PERB_MSK (USB_ENDPT_MSK)
#define USB_ENDPTRPRIME_PETB_POS (16U)
#define USB_ENDPTRPRIME_PETB_MSK (USB_ENDPT_MSK << USB_ENDPTRPRIME_PETB_POS)

// USB Endpoint Flush Register
#define USB_ENDPTFLUSH_FERB_POS (0U)
#define USB_ENDPTFLUSH_FERB_MSK (USB_ENDPT_MSK)
#define USB_ENDPTFLUSH_FETB_POS (16U)
#define USB_ENDPTFLUSH_FETB_MSK (USB_ENDPT_MSK << USB_ENDPTFLUSH_FETB_POS)

// USB Endpoint Status Register
#define USB_ENDPTSTAT_ERBR_POS (0U)
#define USB_ENDPTSTAT_ERBR_MSK (USB_ENDPT_MSK)
#define USB_ENDPTSTAT_ETBR_POS (16U)
#define USB_ENDPTSTAT_ETBR_MSK (USB_ENDPT_MSK << USB_ENDPTSTAT_ETBR_POS)

// USB Endpoint Complete Register
#define USB_ENDPTCOMPLETE_ERCE_POS (0U)
#define USB_ENDPTCOMPLETE_ERCE_MSK (USB_ENDPT_MSK)
#define USB_ENDPTCOMPLETE_ETCE_POS (16U)
#define USB_ENDPTCOMPLETE_ETCE_MSK (USB_ENDPT_MSK << USB_ENDPTCOMPLETE_ETCE_POS)

// USB Endpoint Control Register
#define USB_ENDPTCTRL_RXS     (1U)
#define USB_ENDPTCTRL_RXT_POS (2U)
#define USB_ENDPTCTRL_RXT_MSK (3U << USB_ENDPTCTRL_RXT_POS)
#define USB_ENDPTCTRL_RXT(n)  (((n) << USB_ENDPTCTRL_RXT_POS) & USB_ENDPTCTRL_RXT_MSK)
#define USB_ENDPTCTRL_RXI     (1U << 5U)
#define USB_ENDPTCTRL_RXR     (1U << 6U)
#define USB_ENDPTCTRL_RXE     (1U << 7U)
#define USB_ENDPTCTRL_TXS     (1U << 16U)
#define USB_ENDPTCTRL_TXT_POS (18U)
#define USB_ENDPTCTRL_TXT_MSK (3U << USB_ENDPTCTRL_TXT_POS)
#define USB_ENDPTCTRL_TXT(n)  (((n) << USB_ENDPTCTRL_TXT_POS) & USB_ENDPTCTRL_TXT_MSK)
#define USB_ENDPTCTRL_TXI     (1U << 21U)
#define USB_ENDPTCTRL_TXR     (1U << 22U)
#define USB_ENDPTCTRL_TXE     (1U << 23U)

// Endpoint Queue Head Capabilities and Characteristics
#define USB_EPQH_CAP_IOS                (1U << 15U)
#define USB_EPQH_CAP_MAX_PACKET_LEN_POS (16U)
#define USB_EPQH_CAP_MAX_PACKET_LEN_MSK (0x7FFU << USB_EPQH_CAP_MAX_PACKET_LEN_POS)
#define USB_EPQH_CAP_MAX_PACKET_LEN(n)  (((n) << USB_EPQH_CAP_MAX_PACKET_LEN_POS) & USB_EPQH_CAP_MAX_PACKET_LEN_MSK)
#define USB_EPQH_CAP_ZLT                (1U << 29U)
#define USB_EPQH_CAP_MULT_POS           (30U)
#define USB_EPQH_CAP_MULT_MSK           (3UL << USB_EPQH_CAP_MULT_POS)

// Transfer Descriptor Token
#define USB_dTD_TOKEN_STATUS_POS          (0U)
#define USB_dTD_TOKEN_STATUS_MSK          (0xFFU)
#define USB_dTD_TOKEN_STATUS(n)           (n & USB_dTD_TOKEN_STATUS_MSK)
#define USB_dTD_TOKEN_STATUS_TRAN_ERROR   (0x08U & USB_dTD_TOKEN_STATUS_MSK)
#define USB_dTD_TOKEN_STATUS_BUFFER_ERROR (0x20U & USB_dTD_TOKEN_STATUS_MSK)
#define USB_dTD_TOKEN_STATUS_HALTED       (0x40U & USB_dTD_TOKEN_STATUS_MSK)
#define USB_dTD_TOKEN_STATUS_ACTIVE       (0x80U & USB_dTD_TOKEN_STATUS_MSK)
#define USB_dTD_TOKEN_MULTO_POS           (10U)
#define USB_dTD_TOKEN_MULTO_MSK           (3U << USB_dTD_TOKEN_MULTO_POS)
#define USB_dTD_TOKEN_MULTO(n)            (((n) << USB_dTD_TOKEN_MULTO_POS) & USB_dTD_TOKEN_MULTO_MSK)
#define USB_dTD_TOKEN_IOC                 (1U << 15U)
#define USB_dTD_TOKEN_TOTAL_BYTES_POS     (16U)
#define USB_dTD_TOKEN_TOTAL_BYTES_MSK     (0x7FFFU << USB_dTD_TOKEN_TOTAL_BYTES_POS)
#define USB_dTD_TOKEN_TOTAL_BYTES(n)      (((n) << USB_dTD_TOKEN_TOTAL_BYTES_POS) & USB_dTD_TOKEN_TOTAL_BYTES_MSK)

// USB Host and Device Driver status flags
#define USBD_DRIVER_INITIALIZED (1U)
#define USBD_DRIVER_POWERED     (1U << 1U)

#define USBH_DRIVER_INITIALIZED (1U << 4U)
#define USBH_DRIVER_POWERED     (1U << 5U)

//USB Host and Device function declaration
void USBH_IRQ(void);
void USBD_IRQ(void);
#endif /* __USB_H */
