/*
 * Copyright (c) 2013-2018 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 * $Date:        12. Dec 2018
 * $Revision:    V1.0
 *
 * Driver:       Driver_USBD0
 * Configured:   via RTE_Device.h configuration file
 * Project:      USB High-Speed Device Driver for Silicon Labs RS1xxxx
 * --------------------------------------------------------------------------
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 *
 *   Configuration Setting                  Value
 *   ---------------------                  -----
 *   Connect to hardware via Driver_USBD# = 0
 * --------------------------------------------------------------------------
 * Defines used for driver configuration (at compile time):
 *
 *   USBD_MAX_ENDPOINT_NUM:  defines maximum number of IN/OUT Endpoint pairs 
 *                           that driver will support with Control Endpoint 0
 *                           not included, this value impacts driver memory
 *                           requirements
 *     - default value: 5
 *     - maximum value: 5
 * -------------------------------------------------------------------------- */

/* History:
 *  Version 1.0
 *    Initial release
 */

#include "rsi_ccp_user_config.h"
#if defined(CHIP_9118)

#include <stdint.h>
#include <string.h>

#include "Driver_USBD.h"

#include "rsi_chip.h"
#include "USB.h"
#include "RTE_Device.h"
#include "RTE_Components.h"

#ifndef USBD_MAX_ENDPOINT_NUM
#define USBD_MAX_ENDPOINT_NUM 5U
#endif
#if (USBD_MAX_ENDPOINT_NUM > 6)
#error Too many Endpoints, maximum IN/OUT Endpoint pairs that this driver supports is 5 !!!
#endif

extern uint8_t USB_role;
extern uint8_t USB_state;

// USBD Driver *****************************************************************

#define ARM_USBD_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)

// Driver Version
static const ARM_DRIVER_VERSION usbd_driver_version = { ARM_USBD_API_VERSION, ARM_USBD_DRV_VERSION };

// Driver Capabilities
static const ARM_USBD_CAPABILITIES usbd_driver_capabilities = {
  1U, // VBUS Detection
  1U, // Event VBUS On
  1U  // Event VBUS Off
};

#define ENDPTCTRL(ep_num) (*(volatile uint32_t *)((uint32_t)(&USB->USB_ENDPTCTRL0) + 4U * ep_num))

#define EP_NUM(ep_addr)   (ep_addr & ARM_USB_ENDPOINT_NUMBER_MASK)
#define EP_DIR(ep_addr)   ((ep_addr >> 7) & 1U)
#define EP_SLL(ep_addr)   (EP_DIR(ep_addr) * 16U)
#define EP_QHNUM(ep_addr) ((EP_NUM(ep_addr) * 2U) + EP_DIR(ep_addr))
#define EP_MSK(ep_addr)   (1UL << (EP_NUM(ep_addr) + EP_SLL(ep_addr)))

/*USB Device endpoint Queue Head*/
typedef struct {
  uint32_t cap;
  uint32_t curr_dTD;
  uint32_t next_dTD;
  uint32_t dTD_token;
  uint32_t buf[5];
  uint32_t reserved;
  uint32_t setup[2];
  uint8_t *data;
  uint32_t num;
  uint32_t num_transferred_total;
  uint16_t num_transferring;
  uint8_t ep_type;
  uint8_t ep_active;
} dQH_t;

/*USB Device Endpoint transfer descriptor*/
typedef struct {
  uint32_t next_dTD;
  uint32_t dTD_token;
  uint32_t buf[5];
  uint32_t reserved;
} dTD_t;

static dQH_t __align(2048) dQH[(USBD_MAX_ENDPOINT_NUM + 1U) * 2U]; /* Queue Heads aligned to 2k boundary */
static dTD_t __align(32) dTD[(USBD_MAX_ENDPOINT_NUM + 1U) * 2U];   /* Transfer Descriptors */

static ARM_USBD_SignalDeviceEvent_t SignalDeviceEvent;
static ARM_USBD_SignalEndpointEvent_t SignalEndpointEvent;

static ARM_USBD_STATE usbd_state;
/* Setup packet data buffer */
static uint32_t setup_packet[2];
/* Setup packet received */
static volatile uint8_t setup_packet_recv;

// Function prototypes
static int32_t USBD_EndpointConfigure(uint8_t ep_addr, uint8_t ep_type, uint16_t ep_max_packet_size);

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

// Auxiliary functions

/**
 * @fn          void USBD_HW_EndpointFlush (uint8_t ep_addr)
 * @brief       Flush Endpoint.
 * @param[in]     ep_addr : Endpoint Address
 *                 - ep_addr.0..3: Address
 *                 - ep_addr.7:    Direction
 *
*/
static void USBD_HW_EndpointFlush(uint8_t ep_addr)
{
  uint32_t ep_msk = 0;

  ep_msk = EP_MSK(ep_addr);
  /*USB endpoint flush*/
  USB->USB_ENDPTFLUSH = ep_msk;
  while (USB->USB_ENDPTFLUSH & ep_msk)
    ;
}

/**
 * @fn          void USBD_Reset (void)
 * @brief       Reset USB Endpoint settings and variables.
*/
static void USBD_Reset(void)
{
  uint8_t i;

  setup_packet[0]   = 0U;
  setup_packet[1]   = 0U;
  setup_packet_recv = 0U;
  memset((void *)dQH, 0, sizeof(dQH));
  memset((void *)dTD, 0, sizeof(dTD));

  /*Disable all the supported endpoint for tx and rx*/
  for (i = 1U; i <= USBD_MAX_ENDPOINT_NUM; i++) {
    ENDPTCTRL(i) &= ~(USB_ENDPTCTRL_RXE | USB_ENDPTCTRL_TXE);
  }

  /* Clear interrupts*/
  USB->USB_ENDPTNAK   = 0xFFFFFFFFUL;
  USB->USB_ENDPTNAKEN = 0U;
  USB->USBSTS_D       = 0xFFFFFFFFUL;
  /*Clearing all setup tokens*/
  USB->USB_ENDPTSETUPSTAT = USB->USB_ENDPTSETUPSTAT;
  /*Clearing all endpoint complete status bits*/
  USB->USB_ENDPTCOMPLETE = USB->USB_ENDPTCOMPLETE;

  /*Clear all prime status*/
  while (USB->USB_ENDPTPRIME)
    ;
  /*Clear all primed buffers*/
  USB->USB_ENDPTFLUSH = 0xFFFFFFFFUL;
  while (USB->USB_ENDPTFLUSH)
    ;

  /*Interupt threshold control to isusue an interrupt*/
  USB->USBCMD_D &= ~(USB_USBCMD_D_ITC(0xFFUL));

  /* Initialization of an control Endpoint0*/
  if (usbd_state.speed == ARM_USB_SPEED_HIGH) {
    USBD_EndpointConfigure(0x00U, ARM_USB_ENDPOINT_CONTROL, 64U);
    USBD_EndpointConfigure(0x80U, ARM_USB_ENDPOINT_CONTROL, 64U);
  } else {
    /*for full/low speed*/
    USBD_EndpointConfigure(0x00U, ARM_USB_ENDPOINT_CONTROL, 8U);
    USBD_EndpointConfigure(0x80U, ARM_USB_ENDPOINT_CONTROL, 8U);
  }

  /*Assign the start of endpoint list address register*/
  USB->USB_ENDPOINTLISTADDR = (uint32_t)dQH;

  /*setup lockout mode off*/
  USB->USBMODE_D |= USB_USBMODE_D_SLOM;
}

/**
 * @fn          void USBD_HW_ReadSetupPacket (void)
 * @brief       Read Setup Packet to buffer.
*/
static void USBD_HW_ReadSetupPacket(void)
{

  do {
    /*Setup tripwire*/
    USB->USBCMD_D |= USB_USBCMD_D_SUTW;
    /*Copy the setup packet data received to buffer */
    setup_packet[0] = dQH[0].setup[0];
    setup_packet[1] = dQH[0].setup[1];
  } while (!(USB->USBCMD_D & USB->USBCMD_D));
  /*Clear the setup tripwire*/
  USB->USBCMD_D &= ~USB_USBCMD_D_SUTW;
  /*clear the setup endpoint status bit*/
  USB->USB_ENDPTSETUPSTAT = 1U;
}

/**
 * @fn            void USBD_HW_EndpointTransfer (uint8_t ep_addr)
 * @brief         Start transfer on Endpoint.
 * @param[in]     ep_addr : Endpoint Address
 *                 - ep_addr.0..3: Address
 *                 - ep_addr.7:    Direction
*/
static void USBD_HW_EndpointTransfer(uint8_t ep_addr)
{
  dQH_t *ptr_dqh;
  dTD_t *ptr_dtd;
  uint8_t *data;
  uint32_t ep_msk = 0, num = 0;
  uint8_t ep_qhnum = 0;

  ep_qhnum = EP_QHNUM(ep_addr);
  ep_msk   = EP_MSK(ep_addr);
  ptr_dqh  = &dQH[ep_qhnum];
  ptr_dtd  = &dTD[ep_qhnum];

  data = ptr_dqh->data + ptr_dqh->num_transferred_total;
  num  = ptr_dqh->num - ptr_dqh->num_transferred_total;
  /* max transfer length is 16k*/
  if (num > 0x4000U) {
    num = 0x4000U;
  }

  while (USB->USB_ENDPTSTAT & ep_msk)
    ;

  memset(ptr_dtd, 0, sizeof(dTD_t));

  /* Driver does not support linked endpoint descriptors next address is invalid*/
  ptr_dtd->next_dTD = 1U;

  /* Configure Transfer Descriptor */
  ptr_dtd->dTD_token |= USB_dTD_TOKEN_TOTAL_BYTES(num) | USB_dTD_TOKEN_IOC | USB_dTD_TOKEN_STATUS_ACTIVE;

  /* Set Buffer Addresses */
  ptr_dtd->buf[0] = (uint32_t)(data);
  ptr_dtd->buf[1] = (uint32_t)(data + 0x1000U);
  ptr_dtd->buf[2] = (uint32_t)(data + 0x2000U);
  ptr_dtd->buf[3] = (uint32_t)(data + 0x3000U);
  ptr_dtd->buf[4] = (uint32_t)(data + 0x4000U);
  /*clear status*/
  ptr_dqh->dTD_token &= ~USB_dTD_TOKEN_STATUS_MSK;
  /* Save Transfer Descriptor address to overlay area of queue heads*/
  ptr_dqh->next_dTD = (uint32_t)(ptr_dtd);

  ptr_dqh->num_transferring = num;
  /*start the endpoint transfer*/
  USB->USB_ENDPTPRIME |= ep_msk;
}

// USBD Driver functions

/**
 * @fn          ARM_DRIVER_VERSION USBD_GetVersion (void)
 * @brief       Get driver version.
 * @return      \ref ARM_DRIVER_VERSION
*/
static ARM_DRIVER_VERSION USBD_GetVersion(void)
{
  return usbd_driver_version;
}

/**
 * @fn          ARM_USBD_CAPABILITIES USBD_GetCapabilities (void)
 * @brief       Get driver capabilities.
 * @return      \ref ARM_USBD_CAPABILITIES
*/
static ARM_USBD_CAPABILITIES USBD_GetCapabilities(void)
{
  return usbd_driver_capabilities;
}

/**
 * @fn          int32_t USBD_Initialize (ARM_USBD_SignalDeviceEvent_t   cb_device_event,
                                        ARM_USBD_SignalEndpointEvent_t cb_endpoint_event)
 * @brief       Initialize USB Device Interface.
 * @param[in]   cb_device_event :   Pointer to \ref ARM_USBD_SignalDeviceEvent
 * @param[in]   cb_endpoint_event :  Pointer to \ref ARM_USBD_SignalEndpointEvent
 * @return      \ref execution_status
*/
static int32_t USBD_Initialize(ARM_USBD_SignalDeviceEvent_t cb_device_event,
                               ARM_USBD_SignalEndpointEvent_t cb_endpoint_event)
{

  if (MCU_RET->CHIP_CONFIG_MCU_READ_b.DISABLE_USB == 1U) {
    /* If USB peripheral is not supported by this chip*/
    return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  if ((USB_state & USBD_DRIVER_INITIALIZED) != 0U) {
    return ARM_DRIVER_OK;
  }

  SignalDeviceEvent   = cb_device_event;
  SignalEndpointEvent = cb_endpoint_event;

  USB_role = ARM_USB_ROLE_DEVICE;

  USB_state = USBD_DRIVER_INITIALIZED;

  return ARM_DRIVER_OK;
}

/**
 * @fn          int32_t USBD_Uninitialize (void)
 * @brief       De-initialize USB Device Interface.
 * @return      \ref execution_status
*/
static int32_t USBD_Uninitialize(void)
{

  USB_role = ARM_USB_ROLE_NONE;
  USB_state &= ~USBD_DRIVER_INITIALIZED;

  return ARM_DRIVER_OK;
}

/**
 * @fn          int32_t USBD_PowerControl (ARM_POWER_STATE state)
 * @brief       Control USB Device Interface Power.
 * @param[in]   state : Power state
 * @return      \ref execution_status
*/
static int32_t USBD_PowerControl(ARM_POWER_STATE state)
{

  switch (state) {
    case ARM_POWER_OFF:
      /*Disable interrupt*/
      NVIC_DisableIRQ(USB_IRQn);
      NVIC_ClearPendingIRQ(USB_IRQn);
      USB_state &= ~USBD_DRIVER_POWERED;

      setup_packet_recv = 0U;
      memset((void *)&usbd_state, 0, sizeof(usbd_state));
      memset((void *)dQH, 0, sizeof(dQH));
      memset((void *)dTD, 0, sizeof(dTD));
      break;

    case ARM_POWER_FULL:
      if ((USB_state & USBD_DRIVER_INITIALIZED) == 0U) {
        return ARM_DRIVER_ERROR;
      }
      if ((USB_state & USBD_DRIVER_POWERED) != 0U) {
        return ARM_DRIVER_OK;
      }

      /*USB Config*/
      MISC_USB_CONFIG_REG = 0x11;
      /*Enable M4 USB*/
      NWPAON_ACCESS_CTRL_CLEAR = BIT(4);

      // Reset USB Controller
      USB->USBCMD_D = USB_USBCMD_D_RST;

      while ((USB->USBCMD_D & (USB_USBCMD_D_RS | USB_USBCMD_D_RST)) != 0U)
        ;

      /*Set the usb in device mode and setup lockout mode*/
      USB->USBMODE_D = USB_USBMODE_D_CM1_0(2U) | USB_USBMODE_D_SLOM;
      /* USB device reset*/
      USBD_Reset();

#if (RTE_USB_USB0_HS_EN)
      USB->USB_PORTSC1_D &= ~USB_PORTSC1_D_PFSC;
#else
      USB->USB_PORTSC1_D |= USB_PORTSC1_D_PFSC;
#endif

      /* Set all the usb interrupts*/
      USB->USBINTR_D = (USB_USBINTR_D_UE | USB_USBINTR_D_PCE | USB_USBINTR_D_SLE | USB_USBINTR_D_URE);

      /*Set power flag*/
      USB_state |= USBD_DRIVER_POWERED;
      /*Enable the usb interrupt */
      NVIC_EnableIRQ(USB_IRQn);
      break;

    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  return ARM_DRIVER_OK;
}

/**
 * @fn          int32_t USBD_DeviceConnect (void)
 * @brief       Connect USB Device.
 * @return      \ref execution_status
*/
static int32_t USBD_DeviceConnect(void)
{

  if ((USB_state & USBD_DRIVER_POWERED) == 0U) {
    return ARM_DRIVER_ERROR;
  }

  USB->USBCMD_D |= USB_USBCMD_D_RS;

  return ARM_DRIVER_OK;
}

/**
 * @fn          int32_t USBD_DeviceDisconnect (void)
 * @brief       Disconnect USB Device.
 * @return      \ref execution_status
*/
static int32_t USBD_DeviceDisconnect(void)
{

  if ((USB_state & USBD_DRIVER_POWERED) == 0U) {
    return ARM_DRIVER_ERROR;
  }

  USB->USBCMD_D &= ~USB_USBCMD_D_RS;

#if (RTE_USB0_IND0_PIN_EN)
  USB->PORTSC1_D &= ~USB_PORTSC1_D_PIC1_0(1); // Clear indicator LED0 :FIXME :led indication dedicated pin not there
#endif

  return ARM_DRIVER_OK;
}

/**
 * @fn          ARM_USBD_STATE USBD_DeviceGetState (void)
 * @brief       Get current USB Device State.
 * @return      Device State \ref ARM_USBD_STATE
*/
static ARM_USBD_STATE USBD_DeviceGetState(void)
{
  ARM_USBD_STATE dev_state = { 0U, 0U, 0U };
  uint32_t portsc1_d       = 0;

  if ((USB_state & USBD_DRIVER_POWERED) == 0U) {
    return dev_state;
  }

  portsc1_d = USB->USB_PORTSC1_D;
  dev_state = usbd_state;
  /*Current status of usb device connection and usb device bus status*/
  dev_state.active = ((portsc1_d & USB_PORTSC1_D_CCS) != 0U) && ((portsc1_d & USB_USBSTS_D_SLI) == 0U);

  return dev_state;
}

/**
 * @fn          int32_t USBD_DeviceRemoteWakeup (void)
 * @brief       Trigger USB Remote Wakeup.
 * @return      \ref execution_status
*/
static int32_t USBD_DeviceRemoteWakeup(void)
{

  if ((USB_state & USBD_DRIVER_POWERED) == 0U) {
    return ARM_DRIVER_ERROR;
  }
  /*Enable the phy clock*/
  USB->USB_PORTSC1_D &= ~USB_PORTSC1_D_PHCD;
  /* Force Port Resume*/
  USB->USB_PORTSC1_D |= USB_PORTSC1_D_FPR;

  return ARM_DRIVER_OK;
}

/**
 * @fn          int32_t USBD_DeviceSetAddress (uint8_t dev_addr)
 * @brief       Set USB Device Address.
 * @param[in]   dev_addr :  Device Address
 * @return      \ref execution_status
*/
static int32_t USBD_DeviceSetAddress(uint8_t dev_addr)
{

  if ((USB_state & USBD_DRIVER_POWERED) == 0U) {
    return ARM_DRIVER_ERROR;
  }

  USB->USB_DEVICEADDR = (dev_addr << USB_DEVICEADDR_USBADR_POS) & USB_DEVICEADDR_USBADR_MSK;
  USB->USB_DEVICEADDR |= USB_DEVICEADDR_USBADRA;

  return ARM_DRIVER_OK;
}

/**
 * @fn          int32_t USBD_ReadSetupPacket (uint8_t *setup)
 * @brief       Read setup packet received over Control Endpoint.
 * @param[out]  setup : Pointer to buffer for setup packet
 * @return      \ref execution_status
*/
static int32_t USBD_ReadSetupPacket(uint8_t *setup)
{

  if ((USB_state & USBD_DRIVER_POWERED) == 0U) {
    return ARM_DRIVER_ERROR;
  }
  if (setup_packet_recv == 0U) {
    return ARM_DRIVER_ERROR;
  }

  setup_packet_recv = 0U;
  memcpy(setup, setup_packet, 8);
  /* If new setup packet was received while this was being read*/
  if (setup_packet_recv != 0U) {
    return ARM_DRIVER_ERROR;
  }

  return ARM_DRIVER_OK;
}

/**
 * @fn          int32_t USBD_EndpointConfigure (uint8_t  ep_addr,
                                               uint8_t  ep_type,
                                               uint16_t ep_max_packet_size)
 * @brief       Configure USB Endpoint.
 * @param[in]   ep_addr : Endpoint Address
                - ep_addr.0..3: Address
                - ep_addr.7:    Direction
 * @param[in]   ep_type :  Endpoint Type (ARM_USB_ENDPOINT_xxx)
 * @param[in]   ep_max_packet_size : Endpoint Maximum Packet Size
 * @return      \ref execution_status
*/
static int32_t USBD_EndpointConfigure(uint8_t ep_addr, uint8_t ep_type, uint16_t ep_max_packet_size)
{
  dQH_t *ptr_dqh;
  uint32_t ep_mult        = 0;
  uint32_t ep_packet_size = 0;
  uint8_t ep_num = 0, ep_sll = 0;

  ep_num = EP_NUM(ep_addr);
  if (ep_num > USBD_MAX_ENDPOINT_NUM) {
    return ARM_DRIVER_ERROR;
  }
  if ((USB_state & USBD_DRIVER_POWERED) == 0U) {
    return ARM_DRIVER_ERROR;
  }

  ptr_dqh = &dQH[EP_QHNUM(ep_addr)];
  if (ptr_dqh->ep_active != 0U) {
    return ARM_DRIVER_ERROR_BUSY;
  }

  ep_num         = EP_NUM(ep_addr);
  ep_sll         = EP_SLL(ep_addr);
  ep_mult        = (ep_max_packet_size & ARM_USB_ENDPOINT_MICROFRAME_TRANSACTIONS_MASK) >> 11;
  ep_packet_size = ep_max_packet_size & ARM_USB_ENDPOINT_MAX_PACKET_SIZE_MASK;

  memset((void *)ptr_dqh, 0, sizeof(dQH_t));

  ptr_dqh->ep_type = ep_type;
  if (ep_type == ARM_USB_ENDPOINT_ISOCHRONOUS) {
    /* For isochronous endpoints number of transactions per microframe in high-speed (or frame in full-speed)*/
    /* has to be 1 more than additional transactions per microframe for high-speed (or 1 for full-speed)*/
    ep_mult++;
  }

  if ((ep_mult > 1U) && (usbd_state.speed == ARM_USB_SPEED_FULL)) {
    ep_mult = 1U;
  }

  ptr_dqh->cap = ((ep_mult << USB_EPQH_CAP_MULT_POS) & USB_EPQH_CAP_MULT_MSK)
                 | (USB_EPQH_CAP_MAX_PACKET_LEN(ep_packet_size)) | (USB_EPQH_CAP_ZLT)
                 | ((ep_addr == 0U) * USB_EPQH_CAP_IOS);
  ptr_dqh->next_dTD  = 1U;
  ptr_dqh->dTD_token = 0U;

  USBD_HW_EndpointFlush(ep_addr);

  /*clear all the enpoint settings*/
  ENDPTCTRL(ep_num) &= ~(
    (USB_ENDPTCTRL_RXS | USB_ENDPTCTRL_RXT_MSK | USB_ENDPTCTRL_RXI | USB_ENDPTCTRL_RXR | USB_ENDPTCTRL_RXE) << ep_sll);

  /*set the enpoint setting*/
  ENDPTCTRL(ep_num) |= (USB_ENDPTCTRL_RXT(ep_type) | USB_ENDPTCTRL_RXR | USB_ENDPTCTRL_RXE) << ep_sll;

  return ARM_DRIVER_OK;
}

/**
 * @fn          int32_t USBD_EndpointUnconfigure (uint8_t ep_addr)
 * @brief       Unconfigure USB Endpoint.
 * @param[in]   ep_addr : Endpoint Address
 *                  - ep_addr.0..3: Address
 *                  - ep_addr.7:    Direction
 * @return      \ref execution_status
*/
static int32_t USBD_EndpointUnconfigure(uint8_t ep_addr)
{
  dQH_t *ptr_dqh;
  dTD_t *ptr_dtd;
  uint8_t ep_qhnum = 0, ep_num = 0, ep_sll = 0;

  ep_num = EP_NUM(ep_addr);
  if (ep_num > USBD_MAX_ENDPOINT_NUM) {
    return ARM_DRIVER_ERROR;
  }
  if ((USB_state & USBD_DRIVER_POWERED) == 0U) {
    return ARM_DRIVER_ERROR;
  }

  ep_qhnum = EP_QHNUM(ep_addr);
  ptr_dqh  = &dQH[ep_qhnum];
  if (ptr_dqh->ep_active != 0U) {
    return ARM_DRIVER_ERROR_BUSY;
  }

  ptr_dtd = &dTD[ep_qhnum];
  ep_sll  = EP_SLL(ep_addr);

  /*clear all the enpoint settings*/
  ENDPTCTRL(ep_num) &= ~(
    (USB_ENDPTCTRL_RXS | USB_ENDPTCTRL_RXT_MSK | USB_ENDPTCTRL_RXI | USB_ENDPTCTRL_RXR | USB_ENDPTCTRL_RXE) << ep_sll);

  ENDPTCTRL(ep_num) |= (USB_ENDPTCTRL_RXR << ep_sll);

  memset((void *)ptr_dqh, 0, sizeof(dQH_t));
  memset((void *)ptr_dtd, 0, sizeof(dTD_t));

  return ARM_DRIVER_OK;
}

/**
 * @fn          int32_t USBD_EndpointStall (uint8_t ep_addr, bool stall)
 * @brief       Set/Clear Stall for USB Endpoint.
 * @param[in]   ep_addr :  Endpoint Address
 *                - ep_addr.0..3: Address
 *                - ep_addr.7:    Direction
 * @param[in]   stall : Operation
 *                - \b false Clear
 *                - \b true Set
 * @return      \ref execution_status
*/
static int32_t USBD_EndpointStall(uint8_t ep_addr, bool stall)
{
  dQH_t *ptr_dqh;
  uint8_t ep_num = 0, ep_sll = 0;

  ep_num = EP_NUM(ep_addr);
  if (ep_num > USBD_MAX_ENDPOINT_NUM) {
    return ARM_DRIVER_ERROR;
  }
  if ((USB_state & USBD_DRIVER_POWERED) == 0U) {
    return ARM_DRIVER_ERROR;
  }

  ptr_dqh = &dQH[EP_QHNUM(ep_addr)];
  if (ptr_dqh->ep_active != 0U) {
    return ARM_DRIVER_ERROR_BUSY;
  }
  /*check the endppoint is IN packet or OUT packet*/
  ep_sll = EP_SLL(ep_addr);

  if (stall != 0U) {
    /*Set endpoint stall for IN or OUT packet*/
    ENDPTCTRL(ep_num) |= (USB_ENDPTCTRL_RXS << ep_sll);
  } else {
    /*Clear the endpoint stalling*/
    ENDPTCTRL(ep_num) &= ~(USB_ENDPTCTRL_RXS << ep_sll);

    ptr_dqh->dTD_token = 0U;

    USBD_HW_EndpointFlush(ep_addr);
    /*Set data toggle reset */
    ENDPTCTRL(ep_num) |= (USB_ENDPTCTRL_RXR << ep_sll);
  }

  return ARM_DRIVER_OK;
}

/**
 * @fn          int32_t USBD_EndpointTransfer (uint8_t ep_addr, uint8_t *data, uint32_t num)
 * @brief       Read data from or Write data to USB Endpoint.
 * @param[in]   ep_addr : Endpoint Address
 *               - ep_addr.0..3: Address
 *               - ep_addr.7:    Direction
 * @param[out]  data : Pointer to buffer for data to read or with data to write
 * @param[in]   num : Number of data bytes to transfer
 * @return      \ref execution_status
*/
static int32_t USBD_EndpointTransfer(uint8_t ep_addr, uint8_t *data, uint32_t num)
{
  dQH_t *ptr_dqh;

  if (EP_NUM(ep_addr) > USBD_MAX_ENDPOINT_NUM) {
    return ARM_DRIVER_ERROR;
  }
  if ((USB_state & USBD_DRIVER_POWERED) == 0U) {
    return ARM_DRIVER_ERROR;
  }

  ptr_dqh = &dQH[EP_QHNUM(ep_addr)];
  if (ptr_dqh->ep_active != 0U) {
    return ARM_DRIVER_ERROR_BUSY;
  }

  ptr_dqh->ep_active = 1U;

  ptr_dqh->data                  = data;
  ptr_dqh->num                   = num;
  ptr_dqh->num_transferred_total = 0U;
  ptr_dqh->num_transferring      = 0U;
  /* Start transfer*/
  USBD_HW_EndpointTransfer(ep_addr);

  return ARM_DRIVER_OK;
}

/**
 * @fn          uint32_t USBD_EndpointTransferGetResult (uint8_t ep_addr)
 * @brief       Get result of USB Endpoint transfer.
 * @param[in]   ep_addr  :  Endpoint Address
 *               - ep_addr.0..3: Address
 *               - ep_addr.7:    Direction
 * @return      number of successfully transferred data bytes
*/
static uint32_t USBD_EndpointTransferGetResult(uint8_t ep_addr)
{

  if (EP_NUM(ep_addr) > USBD_MAX_ENDPOINT_NUM) {
    return 0U;
  }

  return (dQH[EP_QHNUM(ep_addr)].num_transferred_total);
}

/**
 * @fn          int32_t USBD_EndpointTransferAbort (uint8_t ep_addr)
 * @brief       Abort current USB Endpoint transfer.
 * @param[in]   ep_addr : Endpoint Address
 *               - ep_addr.0..3: Address
 *               - ep_addr.7:    Direction
 * @return      \ref execution_status
*/
static int32_t USBD_EndpointTransferAbort(uint8_t ep_addr)
{
  dQH_t *ptr_dqh;
  uint32_t ep_msk = 0;
  uint8_t ep_num = 0, ep_sll = 0;

  ep_num = EP_NUM(ep_addr);
  if (ep_num > USBD_MAX_ENDPOINT_NUM) {
    return ARM_DRIVER_ERROR;
  }
  if ((USB_state & USBD_DRIVER_POWERED) == 0U) {
    return ARM_DRIVER_ERROR;
  }

  ptr_dqh = &dQH[EP_QHNUM(ep_addr)];
  ep_msk  = EP_MSK(ep_addr);
  ep_sll  = EP_SLL(ep_addr);

  USBD_HW_EndpointFlush(ep_addr);
  /*Clear completed Flag*/
  USB->USB_ENDPTCOMPLETE = ep_msk;
  ENDPTCTRL(ep_num) |= (USB_ENDPTCTRL_RXR << ep_sll);

  ptr_dqh->dTD_token &= ~0xFFU;

  ptr_dqh->ep_active = 0U;

  return ARM_DRIVER_OK;
}

/**
 * @fn          uint16_t USBD_GetFrameNumber (void)
 * @brief       Get current USB Frame Number.
 * @return      Frame Number
*/
static uint16_t USBD_GetFrameNumber(void)
{

  if ((USB_state & USBD_DRIVER_POWERED) == 0U) {
    return 0U;
  }

  return ((USB->USB_FRINDEX_D & USB_FRINDEX_D_FRINDEX13_3_MSK) >> USB_FRINDEX_D_FRINDEX13_3_POS);
}

/**
 * @fn          void USBD0_IRQ (void)
 * @brief       USB0 Device Interrupt Routine (IRQ).
*/
void USBD_IRQ(void)
{
  dQH_t *ptr_dqh;
  uint32_t status = 0, complete = 0;
  uint16_t ep_packet_size = 0, received_data = 0;
  uint8_t ep_num = 0, ep_addr = 0;

  status = USB->USBSTS_D & USB->USBINTR_D;
  /*Read the enpoint complete status*/
  complete = USB->USB_ENDPTCOMPLETE;

  /*Clear all active interrupts*/
  USB->USBSTS_D = status;
  /*Endpoint complete status clear*/
  USB->USB_ENDPTCOMPLETE = complete;

  /*Reset interrupt*/
  if ((status & USB_USBSTS_D_URI) != 0U) {
    USBD_Reset();
    usbd_state.speed = ARM_USB_SPEED_FULL;
    SignalDeviceEvent(ARM_USBD_EVENT_RESET);
  }

  /* Suspend interrupt */
  if ((status & USB_USBSTS_D_SLI) != 0U) {
    SignalDeviceEvent(ARM_USBD_EVENT_SUSPEND);

#if (RTE_USB0_IND0_PIN_EN)
    USB->USB_PORTSC1_D &= ~USB_PORTSC1_D_PIC15_14(1); // Clear indicator LED0
#endif
  }
  /* Port change detect interrupt*/
  if ((status & USB_USBSTS_D_PCI) != 0U) {
    if (((USB->USB_PORTSC1_D & USB_PORTSC1_D_PSPD_MSK) >> USB_PORTSC1_D_PSPD_POS) == 2U) {
      usbd_state.speed = ARM_USB_SPEED_HIGH;
      SignalDeviceEvent(ARM_USBD_EVENT_HIGH_SPEED);
    } else {
      usbd_state.speed = ARM_USB_SPEED_FULL;
    }

#if (RTE_USB0_IND0_PIN_EN)
    USB->USB_PORTSC1_D |= USB_PORTSC1_D_PIC15_14(1); // Set indicator LED0
#endif
    SignalDeviceEvent(ARM_USBD_EVENT_RESUME);
  }

  if ((status & USB_USBSTS_D_UI) != 0U) {  /* USB interrupt on short packet rx*/
    if ((USB->USB_ENDPTSETUPSTAT) != 0U) { /* Setup Packet Received*/
      USBD_HW_ReadSetupPacket();
      setup_packet_recv = 1U;
      SignalEndpointEvent(0, ARM_USBD_EVENT_SETUP);
    }

    if ((complete & USB_ENDPTCOMPLETE_ETCE_MSK) != 0U) {
      /*IN packet data sent*/
      for (ep_num = 0U; ep_num <= USBD_MAX_ENDPOINT_NUM; ep_num++) {
        if ((complete & USB_ENDPTCOMPLETE_ETCE_MSK) & (1U << (ep_num + USB_ENDPTCOMPLETE_ETCE_POS))) {
          ep_addr = ep_num | ARM_USB_ENDPOINT_DIRECTION_MASK;
          ptr_dqh = &dQH[EP_QHNUM(ep_addr)];

          ptr_dqh->num_transferred_total += ptr_dqh->num_transferring;

          /*Max packet data sent ot not*/
          if (ptr_dqh->num == ptr_dqh->num_transferred_total) {
            ptr_dqh->ep_active = 0U;
            SignalEndpointEvent(ep_addr, ARM_USBD_EVENT_IN);
          } else if (ptr_dqh->ep_active != 0U) {
            /*Transfer remaining data if max packet data is not transmitted*/
            USBD_HW_EndpointTransfer(ep_addr);
          }
        }
      }
    }
    /*OUT packet data received*/
    if ((complete & USB_ENDPTCOMPLETE_ERCE_MSK) != 0U) {
      for (ep_num = 0U; ep_num <= USBD_MAX_ENDPOINT_NUM; ep_num++) {
        if ((complete & USB_ENDPTCOMPLETE_ERCE_MSK) & (1 << ep_num)) {
          ep_addr        = ep_num;
          ptr_dqh        = &dQH[EP_QHNUM(ep_addr)];
          ep_packet_size = (ptr_dqh->cap & USB_EPQH_CAP_MAX_PACKET_LEN_MSK) >> USB_EPQH_CAP_MAX_PACKET_LEN_POS;

          received_data = ptr_dqh->num_transferring
                          - ((ptr_dqh->dTD_token & USB_dTD_TOKEN_TOTAL_BYTES_MSK) >> USB_dTD_TOKEN_TOTAL_BYTES_POS);
          ptr_dqh->num_transferred_total += received_data;

          /* check for the All data received and data terminated with zero length packet*/
          if (((received_data % ep_packet_size) != 0U) || (ptr_dqh->num == ptr_dqh->num_transferred_total)) {
            ptr_dqh->ep_active = 0U;
            SignalEndpointEvent(ep_addr, ARM_USBD_EVENT_OUT);
          } else if (ptr_dqh->ep_active != 0U) {
            /*if all data is not received receive next*/
            USBD_HW_EndpointTransfer(ep_addr);
          }
        }
      }
    }
  }
}

ARM_DRIVER_USBD Driver_USBD0 = { USBD_GetVersion,
                                 USBD_GetCapabilities,
                                 USBD_Initialize,
                                 USBD_Uninitialize,
                                 USBD_PowerControl,
                                 USBD_DeviceConnect,
                                 USBD_DeviceDisconnect,
                                 USBD_DeviceGetState,
                                 USBD_DeviceRemoteWakeup,
                                 USBD_DeviceSetAddress,
                                 USBD_ReadSetupPacket,
                                 USBD_EndpointConfigure,
                                 USBD_EndpointUnconfigure,
                                 USBD_EndpointStall,
                                 USBD_EndpointTransfer,
                                 USBD_EndpointTransferGetResult,
                                 USBD_EndpointTransferAbort,
                                 USBD_GetFrameNumber };
#endif
