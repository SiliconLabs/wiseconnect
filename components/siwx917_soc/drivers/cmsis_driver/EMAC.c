/* -----------------------------------------------------------------------------
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
 * $Date:        25. Dec 2018
 * $Revision:    V1.0
 *
 * Driver:       Driver_ETH_MAC0
 * Configured:   via RTE_Device.h configuration file
 * Project:      Ethernet Media Access (MAC) Driver for Silicon Labs MCU
 * --------------------------------------------------------------------------
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 *
 *   Configuration Setting                     Value
 *   ---------------------                     -----
 *   Connect to hardware via Driver_ETH_MAC# = 0
 * -------------------------------------------------------------------------- */

/* History:
 *  Version 1.0
 *    Initial release
 */
#include "rsi_ccp_user_config.h"
#if defined(CHIP_9118)
/* Receive/transmit Checksum offload enable */
#ifndef EMAC_CHECKSUM_OFFLOAD
#define EMAC_CHECKSUM_OFFLOAD 1
#endif

/* IEEE 1588 time stamping enable (PTP) */
#ifndef EMAC_TIME_STAMP
#define EMAC_TIME_STAMP 0
#endif

#include "EMAC.h"
#include "rsi_chip.h"
#include "clock_update.h"
#include "RTE_Device.h"
#include "PHY_LAN8742A.h"
#define ARM_ETH_MAC_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0) /* driver version */

/* Timeouts */
#define PHY_TIMEOUT 2U /* PHY Register access timeout in ms  */

/* ETH Memory Buffer configuration */
#define NUM_RX_BUF   4U                                 /* 0x1800 for Rx (4*1536=6K)          */
#define NUM_TX_BUF   2U                                 /* 0x0C00 for Tx (2*1536=3K)          */
#define ETH_BUF_SIZE 1536U                              /* ETH Receive/Transmit buffer size   */
#define CLK_BIT      (*(volatile uint32_t *)0x46000000) /* ETH clock enable register*/
#define ETH_SPEED    (*(volatile uint32_t *)0x46008040) /* ETH SPEED SELECTION*/

/* Interrupt Handler Prototype */
void ETH_IRQHandler(void);

/* DMA RX Descriptor */
typedef struct rx_desc {
  uint32_t volatile Stat;
  uint32_t Ctrl;
  uint8_t const *Addr;
  struct rx_desc *Next;
#if ((EMAC_CHECKSUM_OFFLOAD != 0) || (EMAC_TIME_STAMP != 0))
  uint32_t ExtStat;
  uint32_t Reserved[1];
  uint32_t TimeLo;
  uint32_t TimeHi;
#endif
} RX_Desc;

/* DMA TX Descriptor */
typedef struct tx_desc {
  uint32_t volatile CtrlStat;
  uint32_t Size;
  uint8_t const *Addr;
  struct tx_desc *Next;
#if ((EMAC_CHECKSUM_OFFLOAD != 0) || (EMAC_TIME_STAMP != 0))
  uint32_t Reserved[2];
  uint32_t TimeLo;
  uint32_t TimeHi;
#endif
} TX_Desc;

/*Ethernet pin definations for RMII interface*/
static const ETH_PIN_Config eth_pins[] = {
  { RTE_ETH_RMII_TXD0_PORT, RTE_ETH_RMII_TXD0_PIN, RTE_ETH_RMII_TXD0_MODE, RTE_ETH_RMII_TXD0_PAD_SEL },
  { RTE_ETH_RMII_TXD1_PORT, RTE_ETH_RMII_TXD1_PIN, RTE_ETH_RMII_TXD1_MODE, RTE_ETH_RMII_TXD1_PAD_SEL },
  { RTE_ETH_RMII_TX_EN_PORT, RTE_ETH_RMII_TX_EN_PIN, RTE_ETH_RMII_TX_EN_MODE, RTE_ETH_RMII_TX_EN_PAD_SEL },
  { RTE_ETH_RMII_RXD0_PORT, RTE_ETH_RMII_RXD0_PIN, RTE_ETH_RMII_RXD0_MODE, RTE_ETH_RMII_RXD0_PAD_SEL },
  { RTE_ETH_RMII_RXD1_PORT, RTE_ETH_RMII_RXD1_PIN, RTE_ETH_RMII_RXD1_MODE, RTE_ETH_RMII_RXD1_PAD_SEL },
  { RTE_ETH_RMII_REF_CLK_PORT, RTE_ETH_RMII_REF_CLK_PIN, RTE_ETH_RMII_REF_CLK_MODE, RTE_ETH_RMII_REF_CLK_PAD_SEL },
  { RTE_ETH_RMII_CRS_DV_PORT, RTE_ETH_RMII_CRS_DV_PIN, RTE_ETH_RMII_CRS_DV_MODE, RTE_ETH_RMII_CRS_DV_PAD_SEL },
  { RTE_ETH_MDI_MDC_PORT, RTE_ETH_MDI_MDC_PIN, RTE_ETH_MDI_MDC_MODE, RTE_ETH_MDI_MDC_PAD_SEL },
  { RTE_ETH_MDI_MDIO_PORT, RTE_ETH_MDI_MDIO_PIN, RTE_ETH_MDI_MDIO_MODE, RTE_ETH_MDI_MDIO_PAD_SEL },
};

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = { ARM_ETH_MAC_API_VERSION, ARM_ETH_MAC_DRV_VERSION };

/* Driver Capabilities */
static const ARM_ETH_MAC_CAPABILITIES DriverCapabilities = {
  (EMAC_CHECKSUM_OFFLOAD != 0) ? 1U : 0U,                          /* checksum_offload_rx_ip4  */
  (EMAC_CHECKSUM_OFFLOAD != 0) ? 1U : 0U,                          /* checksum_offload_rx_ip6  */
  (EMAC_CHECKSUM_OFFLOAD != 0) ? 1U : 0U,                          /* checksum_offload_rx_udp  */
  (EMAC_CHECKSUM_OFFLOAD != 0) ? 1U : 0U,                          /* checksum_offload_rx_tcp  */
  (EMAC_CHECKSUM_OFFLOAD != 0) ? 1U : 0U,                          /* checksum_offload_rx_icmp */
  (EMAC_CHECKSUM_OFFLOAD != 0) ? 1U : 0U,                          /* checksum_offload_tx_ip4  */
  (EMAC_CHECKSUM_OFFLOAD != 0) ? 1U : 0U,                          /* checksum_offload_tx_ip6  */
  (EMAC_CHECKSUM_OFFLOAD != 0) ? 1U : 0U,                          /* checksum_offload_tx_udp  */
  (EMAC_CHECKSUM_OFFLOAD != 0) ? 1U : 0U,                          /* checksum_offload_tx_tcp  */
  (EMAC_CHECKSUM_OFFLOAD != 0) ? 1U : 0U,                          /* checksum_offload_tx_icmp */
  (ETH_MII != 0) ? ARM_ETH_INTERFACE_MII : ARM_ETH_INTERFACE_RMII, /* media_interface          */
  0U,                                                              /* mac_address              */
  1U,                                                              /* event_rx_frame           */
  1U,                                                              /* event_tx_frame           */
  1U,                                                              /* event_wakeup             */
  (EMAC_TIME_STAMP != 0) ? 1U : 0U                                 /* precision_timer          */
};

/* Local variables */
static EMAC_CTRL Emac;

static RX_Desc rx_desc[NUM_RX_BUF];
static TX_Desc tx_desc[NUM_TX_BUF];
static uint32_t rx_buf[NUM_RX_BUF][ETH_BUF_SIZE >> 2];
static uint32_t tx_buf[NUM_TX_BUF][ETH_BUF_SIZE >> 2];

/**
 * @fn         void init_rx_desc (void)
 * @brief      Initialize Rx DMA descriptors.
 * @return     none
 */
static void init_rx_desc(void)
{
  uint32_t i = 0, next;

  for (i = 0U; i < NUM_RX_BUF; i++) {
    rx_desc[i].Stat = DMA_RX_OWN;
    rx_desc[i].Ctrl = DMA_RX_RCH | ETH_BUF_SIZE;
    rx_desc[i].Addr = (uint8_t *)&rx_buf[i];
    next            = i + 1U;
    if (next == NUM_RX_BUF) {
      next = 0U;
    }
    rx_desc[i].Next = &rx_desc[next];
  }
  ETH->DMA_RX_DESC_LIST_ADDR_REG = (uint32_t)&rx_desc[0];
  Emac.rx_index                  = 0U;
}

/**
 * @fn         void init_tx_desc (void)
 * @brief      Initialize Tx DMA descriptors.
 * @return     none
 */
static void init_tx_desc(void)
{
  uint32_t i, next;

  for (i = 0; i < NUM_TX_BUF; i++) {
    tx_desc[i].Size = DMA_TX_TCH | DMA_TX_LS | DMA_TX_FS;
    tx_desc[i].Addr = (uint8_t *)&tx_buf[i];
    next            = i + 1U;
    if (next == NUM_TX_BUF) {
      next = 0U;
    }
    tx_desc[i].Next = &tx_desc[next];
  }
  ETH->DMA_TX_DESC_LIST_ADDR_REG = (uint32_t)&tx_desc[0];
  Emac.tx_index                  = 0U;
}

/**
 * @fn         void Init(void)
 * @brief      MDIO iniitialization.
 * @param[in]  dir   : 1=input,0=output.
 * @return     None
 */
void MDIO_Init(void)
{
  /*Enable the clock*/
  RSI_CLK_PeripheralClkEnable(M4CLK, EGPIO_CLK, ENABLE_STATIC_CLK);
  /*MDC*/
  RSI_EGPIO_PadSelectionEnable(RTE_ETH_MDI_MDC_PAD_SEL);
  RSI_EGPIO_PadReceiverEnable(RTE_ETH_MDI_MDC_PIN);
  RSI_EGPIO_SetPinMux(EGPIO, RTE_ETH_MDI_MDC_PORT, RTE_ETH_MDI_MDC_PIN, EGPIO_PIN_MUX_MODE0);
  RSI_EGPIO_SetDir(EGPIO, RTE_ETH_MDI_MDC_PORT, RTE_ETH_MDI_MDC_PIN, EGPIO_PIN_MUX_MODE0);

  /*MDIO*/
  RSI_EGPIO_PadSelectionEnable(RTE_ETH_MDI_MDIO_PAD_SEL);
  RSI_EGPIO_PadReceiverEnable(RTE_ETH_MDI_MDIO_PIN);
  RSI_EGPIO_SetPinMux(EGPIO, RTE_ETH_MDI_MDC_PORT, RTE_ETH_MDI_MDIO_PIN, EGPIO_PIN_MUX_MODE0);
}

/**
 * @fn         void MDIO_Dir(uint8_t dir)
 * @brief      Set MDIO dir.
 * @param[in]  dir   : 1=input,0=output.
 * @return     None
 */
void MDIO_Dir(uint8_t dir)
{
  if (dir) {
    RSI_EGPIO_SetDir(EGPIO, RTE_ETH_MDI_MDC_PORT, RTE_ETH_MDI_MDIO_PIN, EGPIO_CONFIG_DIR_INPUT);
  } else {
    RSI_EGPIO_SetDir(EGPIO, RTE_ETH_MDI_MDC_PORT, RTE_ETH_MDI_MDIO_PIN, EGPIO_CONFIG_DIR_OUTPUT);
  }
}

/**
 * @fn         void MDIO_Write(uint32_t data,uint32_t length)
 * @brief      Write's the data to phy register.
 * @param[in]  data   : Data to be written.
 * @param[in]  length : Length of the data.
 * @return     Returns data
 */
void MDIO_Write(uint32_t data, uint32_t length)
{
  volatile int x = 0;
  uint32_t i;

  data = data << (32 - length);
  for (i = 0; i < length; i++) {
    if (data & 0x80000000)
      RSI_EGPIO_SetPin(EGPIO, 0, 12, 1);
    else
      RSI_EGPIO_SetPin(EGPIO, 0, 12, 0);

    RSI_EGPIO_SetPin(EGPIO, 0, 11, 1);
    for (x = 0; x < 5; x++) {
      __ASM("NOP");
    }
    RSI_EGPIO_SetPin(EGPIO, 0, 11, 0);

    data = data << 1;
  }
}

/**
 * @fn         uint16_t MDIO_Read(void)
 * @brief      Read the data from phy register.
 * @param[in]  None
 * @return     Returns data
 */
uint16_t MDIO_Read(void)
{
  volatile int x = 0;
  uint32_t i;
  uint16_t data = 0;

  for (i = 0; i < 16; i++) {
    data <<= 1;
    if ((RSI_EGPIO_GetPin(EGPIO, 0, 12)) == 1)
      data |= 1;

    RSI_EGPIO_SetPin(EGPIO, 0, 11, 1);
    for (x = 0; x < 5; x++) {
      __ASM("NOP");
    }
    RSI_EGPIO_SetPin(EGPIO, 0, 11, 0);
  }
  return data;
}

/**
 * @fn         uint32_t crc32_8bit_rev (uint32_t crc32, uint8_t val)
 * @brief      Calculate 32-bit CRC (Polynomial: 0x04C11DB7, data bit-reversed).
 * @param[in]  crc32 :  CRC initial value
 * @param[in]  val  :  Input value
 * @return     Calculated CRC value
 */
static uint32_t crc32_8bit_rev(uint32_t crc32, uint8_t val)
{
  uint32_t n;

  crc32 ^= __RBIT(val);
  for (n = 8; n; n--) {
    if (crc32 & 0x80000000U) {
      crc32 <<= 1;
      crc32 ^= 0x04C11DB7U;
    } else {
      crc32 <<= 1;
    }
  }
  return (crc32);
}

/**
 * @fn         uint32_t crc32_data (const uint8_t *data, uint32_t len)
 * @brief      Calculate standard 32-bit Ethernet CRC.
 * @param[in]  data :  Pointer to buffer containing the data
 * @param[in]  len :  Data length in bytes
 * @return     Calculated CRC value
 */
static uint32_t crc32_data(const uint8_t *data, uint32_t len)
{
  uint32_t cnt, crc;

  crc = 0xFFFFFFFFU;
  for (cnt = len; cnt; cnt--) {
    crc = crc32_8bit_rev(crc, *data++);
  }
  return (crc ^ 0xFFFFFFFFU);
}

/* Ethernet Driver functions */

/**
 * @fn         ARM_DRIVER_VERSION GetVersion (void)
 * @brief      Get driver version.
 * @return     \ref ARM_DRIVER_VERSION
 */
static ARM_DRIVER_VERSION GetVersion(void)
{
  return DriverVersion;
}

/**
 * @fn         ARM_ETH_MAC_CAPABILITIES GetCapabilities (void)
 * @brief      ARM_ETH_MAC_CAPABILITIES GetCapabilities (void)
 * @return     \ref ARM_ETH_MAC_CAPABILITIES
 */
static ARM_ETH_MAC_CAPABILITIES GetCapabilities(void)
{
  return DriverCapabilities;
}

/**
 * @fn          int32_t Initialize (ARM_ETH_MAC_SignalEvent_t cb_event)
 * @brief       Initialize Ethernet MAC Device.
 * @param[in]   cb_event : Pointer to \ref ARM_ETH_MAC_SignalEvent
 * @return      \ref execution_status
*/
static int32_t Initialize(ARM_ETH_MAC_SignalEvent_t cb_event)
{
  const ETH_PIN_Config *i;
  /*Ethernet pins configuraation*/
  for (i = eth_pins; i != &eth_pins[sizeof(eth_pins) / sizeof(ETH_PIN_Config)]; i++) {
    RSI_EGPIO_SetPinMux(EGPIO, i->port, i->pin, i->mode);
    RSI_EGPIO_PadReceiverEnable(i->pin);
    RSI_EGPIO_PadSelectionEnable(i->pad_sel);
  }
  memset(&Emac, 0, sizeof(EMAC_CTRL));

  Emac.cb_event = cb_event;
  Emac.flags    = EMAC_FLAG_INIT;
  return ARM_DRIVER_OK;
}

/**
 * @fn          int32_t Uninitialize (void)
 * @brief       De-initialize Ethernet MAC Device.
 * @return      \ref execution_status
*/
static int32_t Uninitialize(void)
{
  const ETH_PIN_Config *i;
  /* Ethernet ping Unconfigure */
  for (i = eth_pins; i != &eth_pins[sizeof(eth_pins) / sizeof(ETH_PIN_Config)]; i++) {
    RSI_EGPIO_SetPinMux(EGPIO, i->port, i->pin, EGPIO_PIN_MUX_MODE0);
    RSI_EGPIO_PadReceiverDisable(i->pin);
    RSI_EGPIO_PadSelectionDisable(i->pad_sel);
  }

  Emac.flags &= ~EMAC_FLAG_INIT;

  return ARM_DRIVER_OK;
}

/**
 * @fn          int32_t PowerControl (ARM_POWER_STATE state)
 * @brief       Control Ethernet MAC Device Power.
 * @param[in]   state : Power state
 * @return      \ref execution_status
*/
static int32_t PowerControl(ARM_POWER_STATE state)
{
  volatile uint32_t systemclk, clkdiv;

  switch (state) {
    case ARM_POWER_OFF:

      /* Disable ethernet interrupts */
      NVIC_DisableIRQ(ETHERNET_IRQn);
      ETH->DMA_INTR_EN_REG = 0U;

      Emac.flags &= ~EMAC_FLAG_POWER;
      break;

    case ARM_POWER_LOW:
      return ARM_DRIVER_ERROR_UNSUPPORTED;

    case ARM_POWER_FULL:
      /* Clock to Ethernet peripheral */
      RSI_CLK_PeripheralClkEnable1(M4CLK, ETH_HCLK_ENABLE);

      if ((Emac.flags & EMAC_FLAG_INIT) == 0U) {
        /* Driver not initialized */
        return ARM_DRIVER_ERROR;
      }
      if ((Emac.flags & EMAC_FLAG_POWER) != 0U) {
        /* Driver already powered */
        break;
      }

      /* set Software reset   */
      ETH->DMA_BUS_MODE_REG |= ETH_DMABMR_SR;
      while (ETH->DMA_BUS_MODE_REG & ETH_DMABMR_SR) {
        ; /* Wait until software reset completed */
      }

      /* MDC clock range selection */

      systemclk = RSI_CLK_GetBaseClock(M4_ETHERNET);

      if (systemclk >= 150000000U) {
        clkdiv = ETH_MACMIIAR_CR_Div102;
      } else if (systemclk >= 100000000U) {
        clkdiv = ETH_MACMIIAR_CR_Div62;
      } else if (systemclk >= 60000000U) {
        clkdiv = ETH_MACMIIAR_CR_Div42;
      } else if (systemclk >= 35000000U) {
        clkdiv = ETH_MACMIIAR_CR_Div26;
      } else if (systemclk >= 25000000U) {
        clkdiv = ETH_MACMIIAR_CR_Div16;
      } else {
        /* systemclock is too slow for Ethernet */
        return (ARM_DRIVER_ERROR);
      }
      ETH->MAC_GMII_ADDR_REG = clkdiv;

      /* Initialize MAC configuration */
      ETH->MAC_CONFIG_REG = ETH_MACCR_DO | ETH_MACCR_PS;

      /* Initialize Filter registers */
      ETH->MAC_FRAME_FILTER_REG = ETH_MACFFR_DBF;
      ETH->MAC_FLOW_CTRL_REG    = ETH_MACFCR_DZPQ;

      /* Initialize Address registers */
      ETH->MAC_ADDR0_HIGH_REG = 0U;
      ETH->MAC_ADDR0_LOW_REG  = 0U;
      ETH->MAC_ADDR1_HIGH_REG = 0U;
      ETH->MAC_ADDR1_LOW_REG  = 0U;
      ETH->MAC_ADDR2_HIGH_REG = 0U;
      ETH->MAC_ADDR2_LOW_REG  = 0U;
      ETH->MAC_ADDR3_HIGH_REG = 0U;
      ETH->MAC_ADDR3_LOW_REG  = 0U;

      /* Mask pmt interrupt */
      ETH->MAC_INTR_MASK_REG |= ETH_MACIMR_PMTIM;

      /* Initialize DMA Descriptors */
      init_rx_desc();
      init_tx_desc();

      /* Enable Rx interrupts */
      ETH->DMA_STATUS_REG  = 0xFFFFFFFFU;
      ETH->DMA_INTR_EN_REG = ETH_DMAIER_NIE | ETH_DMAIER_RIE | ETH_DMAIER_TIE;

      /* Disable MMC interrupts */
      ETH->MMC_INTR_MASK_TX_REG = ETH_MMCTIMR_TGFM | ETH_MMCTIMR_TGFSCM;
      ETH->MMC_INTR_MASK_RX_REG = ETH_MMCRIMR_RFCEM;

      /*Enable ETHERNET interrupt */
      NVIC_ClearPendingIRQ(ETHERNET_IRQn);
      NVIC_EnableIRQ(ETHERNET_IRQn);

      Emac.frame_end = NULL;
      Emac.flags |= EMAC_FLAG_POWER;
      break;

    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  return ARM_DRIVER_OK;
}

/**
 * @fn          int32_t GetMacAddress (ARM_ETH_MAC_ADDR *ptr_addr)
 * @brief       Get Ethernet MAC Address.
 * @param[in]   ptr_addr :  Pointer to address
 * @return      \ref execution_status
*/
static int32_t GetMacAddress(ARM_ETH_MAC_ADDR *ptr_addr)
{
  uint32_t val;

  if (ptr_addr == NULL) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((Emac.flags & EMAC_FLAG_POWER) == 0U) {
    return ARM_DRIVER_ERROR;
  }

  val            = ETH->MAC_ADDR0_HIGH_REG;
  ptr_addr->b[5] = (uint8_t)(val >> 8);
  ptr_addr->b[4] = (uint8_t)(val);
  val            = ETH->MAC_ADDR0_LOW_REG;
  ptr_addr->b[3] = (uint8_t)(val >> 24);
  ptr_addr->b[2] = (uint8_t)(val >> 16);
  ptr_addr->b[1] = (uint8_t)(val >> 8);
  ptr_addr->b[0] = (uint8_t)(val);

  return ARM_DRIVER_OK;
}

/**
 * @fn          int32_t SetMacAddress (const ARM_ETH_MAC_ADDR *ptr_addr)
 * @brief       Set Ethernet MAC Address.
 * @param[in]   ptr_addr : Pointer to address
 * @return      \ref execution_status
*/
static int32_t SetMacAddress(const ARM_ETH_MAC_ADDR *ptr_addr)
{

  if (ptr_addr == NULL) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((Emac.flags & EMAC_FLAG_POWER) == 0U) {
    return ARM_DRIVER_ERROR;
  }

  /* Set Ethernet MAC Address registers */
  ETH->MAC_ADDR0_HIGH_REG = ((uint32_t)ptr_addr->b[5] << 8) | (uint32_t)ptr_addr->b[4];
  ETH->MAC_ADDR0_LOW_REG  = ((uint32_t)ptr_addr->b[3] << 24) | ((uint32_t)ptr_addr->b[2] << 16)
                           | ((uint32_t)ptr_addr->b[1] << 8) | (uint32_t)ptr_addr->b[0];

  return ARM_DRIVER_OK;
}

/**
 * @fn          int32_t SetAddressFilter (const ARM_ETH_MAC_ADDR *ptr_addr,
                                               uint32_t          num_addr)
 * @brief       Configure Address Filter.
 * @param[in]   ptr_addr : Pointer to addresses
 * @param[in]   num_addr :  Number of addresses to configure
 * @return      \ref execution_status
*/
static int32_t SetAddressFilter(const ARM_ETH_MAC_ADDR *ptr_addr, uint32_t num_addr)
{
  uint32_t crc;

  if ((ptr_addr == NULL) && (num_addr != 0)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((Emac.flags & EMAC_FLAG_POWER) == 0U) {
    return ARM_DRIVER_ERROR;
  }

  /* Use unicast address filtering for first 3 MAC addresses */
  ETH->MAC_FRAME_FILTER_REG &= ~(ETH_MACFFR_HPF | ETH_MACFFR_HMC);
  ETH->MAC_HASH_TABLE_HIGH_REG = 0U;
  ETH->MAC_HASH_TABLE_LOW_REG  = 0U;

  if (num_addr == 0U) {
    ETH->MAC_ADDR1_HIGH_REG = 0U;
    ETH->MAC_ADDR1_LOW_REG  = 0U;
    ETH->MAC_ADDR2_HIGH_REG = 0U;
    ETH->MAC_ADDR2_LOW_REG  = 0U;
    ETH->MAC_ADDR3_HIGH_REG = 0U;
    ETH->MAC_ADDR3_LOW_REG  = 0U;
    return ARM_DRIVER_OK;
  }

  ETH->MAC_ADDR1_HIGH_REG = ((uint32_t)ptr_addr->b[5] << 8) | (uint32_t)ptr_addr->b[4] | ETH_MACA1HR_AE;
  ETH->MAC_ADDR1_LOW_REG  = ((uint32_t)ptr_addr->b[3] << 24) | ((uint32_t)ptr_addr->b[2] << 16)
                           | ((uint32_t)ptr_addr->b[1] << 8) | (uint32_t)ptr_addr->b[0];
  num_addr--;
  if (num_addr == 0U) {
    ETH->MAC_ADDR2_HIGH_REG = 0U;
    ETH->MAC_ADDR2_LOW_REG  = 0U;
    ETH->MAC_ADDR3_HIGH_REG = 0U;
    ETH->MAC_ADDR3_LOW_REG  = 0U;
    return ARM_DRIVER_OK;
  }
  ptr_addr++;

  ETH->MAC_ADDR2_HIGH_REG = ((uint32_t)ptr_addr->b[5] << 8) | (uint32_t)ptr_addr->b[4] | ETH_MACA2HR_AE;
  ETH->MAC_ADDR2_LOW_REG  = ((uint32_t)ptr_addr->b[3] << 24) | ((uint32_t)ptr_addr->b[2] << 16)
                           | ((uint32_t)ptr_addr->b[1] << 8) | (uint32_t)ptr_addr->b[0];
  num_addr--;
  if (num_addr == 0U) {
    ETH->MAC_ADDR3_HIGH_REG = 0U;
    ETH->MAC_ADDR3_LOW_REG  = 0U;
    return ARM_DRIVER_OK;
  }
  ptr_addr++;

  ETH->MAC_ADDR3_HIGH_REG = ((uint32_t)ptr_addr->b[5] << 8) | (uint32_t)ptr_addr->b[4] | ETH_MACA3HR_AE;
  ETH->MAC_ADDR3_LOW_REG  = ((uint32_t)ptr_addr->b[3] << 24) | ((uint32_t)ptr_addr->b[2] << 16)
                           | ((uint32_t)ptr_addr->b[1] << 8) | (uint32_t)ptr_addr->b[0];
  num_addr--;
  if (num_addr == 0U) {
    return ARM_DRIVER_OK;
  }
  ptr_addr++;

  /* Calculate 64-bit Hash table for remaining MAC addresses */
  for (; num_addr; ptr_addr++, num_addr--) {
    crc = crc32_data(&ptr_addr->b[0], 6U) >> 26;
    if (crc & 0x20U) {
      ETH->MAC_HASH_TABLE_HIGH_REG |= (1U << (crc & 0x1FU));
    } else {
      ETH->MAC_HASH_TABLE_LOW_REG |= (1U << crc);
    }
  }
  /* Enable both, unicast and hash address filtering */
  ETH->MAC_FRAME_FILTER_REG |= ETH_MACFFR_HPF | ETH_MACFFR_HMC;

  return ARM_DRIVER_OK;
}

/**
 * @fn          int32_t SendFrame (const uint8_t *frame, uint32_t len, uint32_t flags)
 * @brief       Send Ethernet frame.
 * @param[in]   frame :  Pointer to frame buffer with data to send
 * @param[in]   len  :  Frame buffer length in bytes
 * @param[in]   flags :  Frame transmit flags (see ARM_ETH_MAC_TX_FRAME_...)
 * @return      \ref execution_status
*/
static int32_t SendFrame(const uint8_t *frame, uint32_t len, uint32_t flags)
{
  uint8_t *dst = Emac.frame_end;
  uint32_t ctrl;
  uint32_t dummy = 0;

  if ((frame == NULL) || (len == 0U)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((Emac.flags & EMAC_FLAG_POWER) == 0U) {
    return ARM_DRIVER_ERROR;
  }

  if (dst == NULL) {
    /* New TX frame  start */
    if (tx_desc[Emac.tx_index].CtrlStat & DMA_TX_OWN) {
      /* wait upto transmitter is busy */
      return ARM_DRIVER_ERROR_BUSY;
    }
    dst                         = (uint8_t *)tx_desc[Emac.tx_index].Addr;
    dummy                       = tx_desc[Emac.tx_index].Size;
    dummy                       = (dummy & 0xFF800000);
    tx_desc[Emac.tx_index].Size = (len | dummy);
  } else {
    /* Sending data fragments in progress */
    tx_desc[Emac.tx_index].Size += len;
  }
  /* Fast-copy data fragments to ETH-DMA buffer */
  for (; len > 7U; dst += 8, frame += 8, len -= 8U) {
    ((__packed uint32_t *)dst)[0] = ((__packed uint32_t *)frame)[0];
    ((__packed uint32_t *)dst)[1] = ((__packed uint32_t *)frame)[1];
  }
  /* Copy remaining 7 bytes */
  for (; len > 1U; dst += 2, frame += 2, len -= 2U) {
    ((__packed uint16_t *)dst)[0] = ((__packed uint16_t *)frame)[0];
  }
  if (len > 0U) {
    dst++[0] = frame++[0];
  }

  if (flags & ARM_ETH_MAC_TX_FRAME_FRAGMENT) {
    /* If more data is there copy current write position */
    Emac.frame_end = dst;
    return ARM_DRIVER_OK;
  }

  /* Send the frame to DMA */
  ctrl = tx_desc[Emac.tx_index].Size & ~DMA_TX_CIC;
#if (EMAC_CHECKSUM_OFFLOAD != 0)
  if (Emac.tx_cks_offload) {

    uint16_t prot = (*((const __packed uint16_t *)(&tx_desc[Emac.tx_index].Addr[12])));
    uint16_t frag = (*((const __packed uint16_t *)(&tx_desc[Emac.tx_index].Addr[20])));
    ;
    if ((prot == 0x0008) && (frag & 0xFF3F)) {
      /* Insert only IP header checksum in fragmented frame */
      ctrl |= DMA_TX_CIC_IP;
    } else {
      /* Insert IP header and payload checksums (TCP,UDP,ICMP) */
      ctrl |= DMA_TX_CIC;
    }
  }
#endif
  ctrl &= ~(DMA_TX_IC);
  if (flags & ARM_ETH_MAC_TX_FRAME_EVENT) {
    ctrl |= DMA_TX_IC;
  }

  tx_desc[Emac.tx_index].Size     = ctrl;
  tx_desc[Emac.tx_index].CtrlStat = DMA_TX_OWN;

  Emac.tx_index++;
  if (Emac.tx_index == NUM_TX_BUF) {
    Emac.tx_index = 0U;
  }
  Emac.frame_end = NULL;

  /* Start frame transmission */
  ETH->DMA_STATUS_REG = ETH_DMASR_TPSS;

  ETH->DMA_TX_POLL_DEMAND_REG = 0U;

  return ARM_DRIVER_OK;
}

/**
 * @fn          int32_t ReadFrame (uint8_t *frame, uint32_t len)
 * @brief       Read data of received Ethernet frame.
 * @param[in]   frame :  Pointer to frame buffer for data to read into
 * @param[in]   len :   Frame buffer length in bytes
 * @return      number of data bytes read or execution status
                 - value >= 0: number of data bytes read
                 - value < 0: error occurred, value is execution status as defined with \ref execution_status 
*/
static int32_t ReadFrame(uint8_t *frame, uint32_t len)
{
  uint8_t const *src = rx_desc[Emac.rx_index].Addr;
  int32_t cnt        = (int32_t)len;

  if ((frame == NULL) && (len != 0U)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((Emac.flags & EMAC_FLAG_POWER) == 0U) {
    return ARM_DRIVER_ERROR;
  }

  /* Fast-copy data to frame buffer */
  for (; len > 7U; frame += 8, src += 8, len -= 8U) {
    ((__packed uint32_t *)frame)[0] = ((uint32_t *)src)[0];
    ((__packed uint32_t *)frame)[1] = ((uint32_t *)src)[1];
  }
  /* Copy remaining 7 bytes */
  for (; len > 1U; frame += 2, src += 2, len -= 2U) {
    ((__packed uint16_t *)frame)[0] = ((uint16_t *)src)[0];
  }
  if (len > 0U) {
    frame[0] = src[0];
  }

  /* Return this block back to ETH-DMA */
  rx_desc[Emac.rx_index].Stat = DMA_RX_OWN;

  Emac.rx_index++;
  if (Emac.rx_index == NUM_RX_BUF) {
    Emac.rx_index = 0;
  }

  if (ETH->DMA_STATUS_REG & ETH_DMASR_RBU) {
    /* Receive buffer unavailable, resume DMA */
    ETH->DMA_STATUS_REG         = ETH_DMASR_RBU;
    ETH->DMA_RX_POLL_DEMAND_REG = 0;
  }

  return (cnt);
}

/**
 * @fn          uint32_t GetRxFrameSize (void)
 * @brief       Get size of received Ethernet frame.
 * @return      number of bytes in received frame
*/
static uint32_t GetRxFrameSize(void)
{

  uint32_t stat = rx_desc[Emac.rx_index].Stat;

  if ((Emac.flags & EMAC_FLAG_POWER) == 0U) {
    return (0U);
  }

  if (stat & DMA_RX_OWN) {
    /* Owned by DMA */
    return (0U);
  }
  if (((stat & DMA_RX_ES) != 0) || ((stat & DMA_RX_FS) == 0) || ((stat & DMA_RX_LS) == 0)) {
    /* Error, this block is invalid */
    return (0xFFFFFFFFU);
  }

  return (((stat & DMA_RX_FL) >> 16) - 4U);
}

/**
 * @fn          int32_t GetRxFrameTime (ARM_ETH_MAC_TIME *time)
 * @brief       Get time of received Ethernet frame.
 * @param[in]   time :  Pointer to time structure for data to read into
 * @return      \ref execution_status
*/
static int32_t GetRxFrameTime(ARM_ETH_MAC_TIME *time)
{
  return ARM_DRIVER_ERROR_UNSUPPORTED;
}

/**
 * @fn          int32_t GetTxFrameTime (ARM_ETH_MAC_TIME *time)
 * @brief       Get time of transmitted Ethernet frame.
 * @param[in]   time :  Pointer to time structure for data to read into
 * @return      \ref execution_status
*/
static int32_t GetTxFrameTime(ARM_ETH_MAC_TIME *time)
{
  return ARM_DRIVER_ERROR_UNSUPPORTED;
}

/**
 * @fn          int32_t ControlTimer (uint32_t control, ARM_ETH_MAC_TIME *time)
 * @brief       Control Precision Timer.
 * @param[in]   control :  operation
 * @param[in]   time :    Pointer to time structure
 * @return      \ref execution_status
*/
static int32_t ControlTimer(uint32_t control, ARM_ETH_MAC_TIME *time)
{
  return ARM_DRIVER_ERROR_UNSUPPORTED;
}

/**
 * @fn          int32_t Control (uint32_t control, uint32_t arg)
 * @brief       Control Ethernet Interface.
 * @param[in]   control : operation
 * @param[in]   arg    :  argument of operation (optional)
 * @return      \ref execution_status
*/
static int32_t Control(uint32_t control, uint32_t arg)
{
  uint32_t maccr;
  uint32_t dmaomr;
  uint32_t macffr;

  if ((Emac.flags & EMAC_FLAG_POWER) == 0U) {
    return ARM_DRIVER_ERROR;
  }

  switch (control) {
    case ARM_ETH_MAC_CONFIGURE:
      maccr = ETH->MAC_CONFIG_REG & ~(ETH_MACCR_DM | ETH_MACCR_LM | ETH_MACCR_IPC);

      /* Configure the speed of operation(10/100 mbps) */
      switch (arg & ARM_ETH_MAC_SPEED_Msk) {
        case ARM_ETH_MAC_SPEED_10M:
          ETH_SPEED &= ~ETH_MACCR_FES;
          break;
        case ARM_ETH_SPEED_100M:
          ETH_SPEED |= ETH_MACCR_FES;
          break;
        default:
          return ARM_DRIVER_ERROR_UNSUPPORTED;
      }

      /* Confige the mode (Half/Full duplex)  */
      switch (arg & ARM_ETH_MAC_DUPLEX_Msk) {
        case ARM_ETH_MAC_DUPLEX_FULL:
          maccr |= ETH_MACCR_DM;
          break;
        case ARM_ETH_MAC_DUPLEX_HALF:
          maccr &= ~ETH_MACCR_DM;
          break;
        default:
          return ARM_DRIVER_ERROR;
      }

      /* Configurration of mac level loopback opearation */
      if (arg & ARM_ETH_MAC_LOOPBACK) {
        maccr |= ETH_MACCR_LM;
      }

      dmaomr = ETH->DMA_OPER_MODE_REG & ~(ETH_DMAOMR_RSF | ETH_DMAOMR_TSF);
#if (EMAC_CHECKSUM_OFFLOAD != 0)
      /* Enable rx checksum verification */
      if (arg & ARM_ETH_MAC_CHECKSUM_OFFLOAD_RX) {
        maccr |= ETH_MACCR_IPC;
      }

      /* Enable tx checksum generation */
      if (arg & ARM_ETH_MAC_CHECKSUM_OFFLOAD_TX) {
        dmaomr |= ETH_DMAOMR_TSF;
        Emac.tx_cks_offload = true;
      } else {
        Emac.tx_cks_offload = false;
      }
#else
      if ((arg & ARM_ETH_MAC_CHECKSUM_OFFLOAD_RX) || (arg & ARM_ETH_MAC_CHECKSUM_OFFLOAD_TX)) {
        /* Checksum offload is disabled in the driver */
        return ARM_DRIVER_ERROR;
      }
#endif
      ETH->DMA_OPER_MODE_REG = dmaomr;
      ETH->MAC_CONFIG_REG    = maccr;

      macffr = ETH->MAC_FRAME_FILTER_REG & ~(ETH_MACFFR_PR | ETH_MACFFR_PM | ETH_MACFFR_DBF);
      /* Enable broadcast frame receive */
      if ((arg & ARM_ETH_MAC_ADDRESS_BROADCAST) == 0) {
        macffr |= ETH_MACFFR_DBF;
      }

      /* Enable all multicast frame receive */
      if (arg & ARM_ETH_MAC_ADDRESS_MULTICAST) {
        macffr |= ETH_MACFFR_PM;
      }

      /* Enable promiscuous mode (no filtering) */
      if (arg & ARM_ETH_MAC_ADDRESS_ALL) {
        macffr |= ETH_MACFFR_PR;
      }
      ETH->MAC_FRAME_FILTER_REG = macffr;
      break;

    case ARM_ETH_MAC_CONTROL_TX:
      /* Enable/disable MAC transmitter */
      maccr  = ETH->MAC_CONFIG_REG & ~ETH_MACCR_TE;
      dmaomr = ETH->DMA_OPER_MODE_REG & ~ETH_DMAOMR_ST;
      if (arg != 0) {
        maccr |= ETH_MACCR_TE;
        dmaomr |= ETH_DMAOMR_ST;
      }
      ETH->MAC_CONFIG_REG    = maccr;
      ETH->DMA_OPER_MODE_REG = dmaomr;
      break;

    case ARM_ETH_MAC_CONTROL_RX:
      /* Enable/disable MAC receiver */
      maccr  = ETH->MAC_CONFIG_REG & ~ETH_MACCR_RE;
      dmaomr = ETH->DMA_OPER_MODE_REG & ~ETH_DMAOMR_SR;
      if (arg != 0) {
        maccr |= ETH_MACCR_RE;
        dmaomr |= ETH_DMAOMR_SR;
      }
      ETH->MAC_CONFIG_REG    = maccr;
      ETH->DMA_OPER_MODE_REG = dmaomr;
      break;

    case ARM_ETH_MAC_FLUSH:
      /* Flush tx and rx buffers */
      if (arg & ARM_ETH_MAC_FLUSH_RX) {
      }
      if (arg & ARM_ETH_MAC_FLUSH_TX) {
        ETH->DMA_OPER_MODE_REG |= ETH_DMAOMR_FTF;
      }
      break;

    case ARM_ETH_MAC_VLAN_FILTER:
      /* Configure VLAN filter */
      ETH->MAC_VLAN_TAG_REG = arg;
      break;

    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  return ARM_DRIVER_OK;
}

/**
 * @fn          int32_t PHY_Read (uint8_t phy_addr, uint8_t reg_addr, uint16_t *data)
 * @brief       Read Ethernet PHY Register through Management Interface.
 * @param[in]   phy_addr : 5-bit device address
 * @param[in]   reg_addr : 5-bit register address
 * @param[out]  data   :   Pointer where the result is written to
 * @return      \ref execution_status
*/
static int32_t PHY_Read(uint8_t phy_addr, uint8_t reg_addr, uint16_t *data)
{

  /*Initialize the mdio*/
  MDIO_Init();
  /*Set the MDIO Direction*/
  MDIO_Dir(EGPIO_CONFIG_DIR_OUTPUT);
  /*32 continuous 1's*/
  MDIO_Write(0xFFFFFFFF, 32);
  /*Start of frame(01) and read command(10) */
  MDIO_Write(0x6, 4);
  /*PHY address*/
  MDIO_Write(phy_addr, 5);
  /*reg address*/
  MDIO_Write(reg_addr, 5);

  MDIO_Dir(EGPIO_CONFIG_DIR_INPUT);
  /*Turn around time*/
  MDIO_Write(0x0, 2);

  *data = MDIO_Read();

  MDIO_Dir(EGPIO_CONFIG_DIR_OUTPUT);

  return ARM_DRIVER_OK;
}

/**
 * @fn          int32_t PHY_Write (uint8_t phy_addr, uint8_t reg_addr, uint16_t data)
 * @brief       Write Ethernet PHY Register through Management Interface.
 * @param[in]   phy_addr : 5-bit device address
 * @param[in]   reg_addr :  5-bit register address
 * @param[in]   data  :    16-bit data to write
 * @return      \ref execution_status
*/
static int32_t PHY_Write(uint8_t phy_addr, uint8_t reg_addr, uint16_t data)
{

  /*Initialize the mdio*/
  MDIO_Init();
  /*Set the MDIO Direction*/
  MDIO_Dir(EGPIO_CONFIG_DIR_OUTPUT);
  /*32 continuous 1's*/
  MDIO_Write(0xFFFFFFFF, 32);
  /*start of frame(01) and write command(01) */
  MDIO_Write(0x5, 4);
  /*phy address*/
  MDIO_Write(phy_addr, 5);
  /*reg address*/
  MDIO_Write(reg_addr, 5);
  /*Turn around time*/
  MDIO_Write(0x2, 2);
  /*write the data*/
  MDIO_Write(data, 16);

  return ARM_DRIVER_OK;
}

/* Ethernet IRQ Handler */
void IRQ062_Handler(void)
{
  uint32_t dmasr, macsr, event = 0;

  dmasr               = ETH->DMA_STATUS_REG;
  ETH->DMA_STATUS_REG = dmasr & (ETH_DMASR_NIS | ETH_DMASR_RI | ETH_DMASR_TI);

  if (dmasr & ETH_DMASR_TI) {
    /* Frame sent */
    event |= ARM_ETH_MAC_EVENT_TX_FRAME;
  }
  if (dmasr & ETH_DMASR_RI) {
    /* Frame received */
    event |= ARM_ETH_MAC_EVENT_RX_FRAME;
  }
  macsr = ETH->MAC_STATUS_REG;

  if (macsr & ETH_MACSR_PMTS) {
    ETH->MAC_PMT_CTRL_STATUS_REG;
    event |= ARM_ETH_MAC_EVENT_WAKEUP;
  }

  /* Callback event notification */
  if (event && Emac.cb_event) {
    Emac.cb_event(event);
  }
}

/* MAC Driver Control Block */
ARM_DRIVER_ETH_MAC Driver_ETH_MAC0 = { GetVersion,     GetCapabilities, Initialize,       Uninitialize, PowerControl,
                                       GetMacAddress,  SetMacAddress,   SetAddressFilter, SendFrame,    ReadFrame,
                                       GetRxFrameSize, GetRxFrameTime,  GetTxFrameTime,   ControlTimer, Control,
                                       PHY_Read,       PHY_Write };
#endif
