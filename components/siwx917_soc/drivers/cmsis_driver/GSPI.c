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
 * $Date:        25 Sep 2019
 * $Revision:    V1.1
 *
 * Driver:       Driver_USART
 * Configured:   via RTE_Device.h configuration file
 * Project:      USART Driver for RS1xxxx
 */

/* History:
 * Version 1.0
 * Initial release
 */
#include "rsi_ccp_user_config.h"

#include "clock_update.h"
#include "GSPI.h"
#include "rsi_gspi.h"
#include "rsi_power_save.h"
#ifdef GSPI_CONFIG
#include "sl_si91x_gspi_common_config.h"
#endif

#if defined(A11_ROM)
#include "rsi_rom_table_si91x.h"
extern const ROM_GSPI_API_T gspi_api;
#endif

extern RSI_UDMA_HANDLE_T udmaHandle0; //check
extern uint32_t dma_rom_buff0[30];    //we can keep wrapeers

#define CONTROL_STRUCT0 (UDMA_NUMBER_OF_CHANNELS * 2)

/* IAR support */
#if defined(__ICCARM__)
#pragma location = UDMA0_SRAM_BASE
extern RSI_UDMA_DESC_T UDMA0_Table[CONTROL_STRUCT0];
#pragma location = UDMA1_SRAM_BASE
extern RSI_UDMA_DESC_T UDMA1_Table[CONTROL_STRUCT1];
#endif

/* DMA descriptors must be aligned to 16 bytes */
#if defined(__CC_ARM)
extern RSI_UDMA_DESC_T UDMA0_Table[CONTROL_STRUCT0];
#endif /* defined (__CC_ARM) */

#if defined(__GNUC__)
extern RSI_UDMA_DESC_T __attribute__((section(".udma_addr0"))) UDMA0_Table[CONTROL_STRUCT0];
extern RSI_UDMA_DESC_T __attribute__((section(".udma_addr1"))) UDMA1_Table[CONTROL_STRUCT1];
#endif /* defined (__GNUC__) */

extern UDMA_Channel_Info udma0_chnl_info[32];

// GSPI DMA configuration
#if SL_GSPI_DMA_CONFIG_ENABLE
#define RTE_GSPI_MASTER_CHNL_UDMA_TX_EN 1
#define RTE_GSPI_MASTER_CHNL_UDMA_RX_EN 1

#if defined(SL_GSPI_AFULL_THRESHOLD)
#define RTE_FIFO_AFULL_THRLD SL_GSPI_AFULL_THRESHOLD
#endif

#if defined(SL_GSPI_AEMPTY_THRESHOLD)
#define RTE_FIFO_AEMPTY_THRLD SL_GSPI_AEMPTY_THRESHOLD
#endif

#define TX_DMA_ARB_SIZE ARBSIZE_4
#define RX_DMA_ARB_SIZE ARBSIZE_4
#endif

/* UDMA0 Resources */
extern UDMA_RESOURCES UDMA0_Resources;

#define ARM_SPI_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2, 0) /* driver version */
extern RSI_UDMA_HANDLE_T udmaHandle0, udmaHandle1;
/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = { ARM_SPI_API_VERSION, ARM_SPI_DRV_VERSION };

/* Driver Capabilities */
static const ARM_SPI_CAPABILITIES DriverCapabilities = {
  1, /* Simplex Mode (Master and Slave) */
  0, /* TI Synchronous Serial Interface */
  0, /* Microwire Interface */
  1  /* Signal Mode Fault event: \ref SPI_EVENT_MODE_FAULT */
};
#if (RTE_GSPI_MASTER)

static GSPI_PIN gspi_clock = { RTE_GSPI_MASTER_CLK_PORT,
                               RTE_GSPI_MASTER_CLK_PIN,
                               RTE_GSPI_MASTER_CLK_MUX,
                               RTE_GSPI_MASTER_CLK_PAD };
static GSPI_PIN gspi_cs0   = { RTE_GSPI_MASTER_CS0_PORT,
                               RTE_GSPI_MASTER_CS0_PIN,
                               RTE_GSPI_MASTER_CS0_MUX,
                               RTE_GSPI_MASTER_CS0_PAD };
#ifdef GSPI_MULTI_SLAVE
static GSPI_PIN gspi_cs1 = { RTE_GSPI_MASTER_CS1_PORT,
                             RTE_GSPI_MASTER_CS1_PIN,
                             RTE_GSPI_MASTER_CS1_MUX,
                             RTE_GSPI_MASTER_CS1_PAD };
static GSPI_PIN gspi_cs2 = { RTE_GSPI_MASTER_CS2_PORT,
                             RTE_GSPI_MASTER_CS2_PIN,
                             RTE_GSPI_MASTER_CS2_MUX,
                             RTE_GSPI_MASTER_CS2_PAD };
#endif
static GSPI_PIN gspi_mosi = { RTE_GSPI_MASTER_MOSI_PORT,
                              RTE_GSPI_MASTER_MOSI_PIN,
                              RTE_GSPI_MASTER_MOSI_MUX,
                              RTE_GSPI_MASTER_MOSI_PAD };
static GSPI_PIN gspi_miso = { RTE_GSPI_MASTER_MISO_PORT,
                              RTE_GSPI_MASTER_MISO_PIN,
                              RTE_GSPI_MASTER_MISO_MUX,
                              RTE_GSPI_MASTER_MISO_PAD };

static GSPI_FIFO_THRESHOLDS gspi_threshold = { TX_DMA_ARB_SIZE,
                                               RX_DMA_ARB_SIZE,
                                               RTE_FIFO_AFULL_THRLD,
                                               RTE_FIFO_AEMPTY_THRLD };

#define GSPI_MASTER_IRQHandler IRQ046_Handler
// GSPI_MASTER Run-Time Information
static GSPI_INFO GSPI_MASTER_Info                  = { 0U };
static GSPI_TRANSFER_INFO GSPI_MASTER_TransferInfo = { 0U };

#if (RTE_GSPI_MASTER_CHNL_UDMA_TX_EN == 1)
void GSPI_MASTER_UDMA_Tx_Event(uint32_t event, uint8_t dmaCh);
static GSPI_DMA GSPI_MASTER_UDMA_TX_CHNL = { {
                                               0,                              //channelPrioHigh
                                               0,                              //altStruct
                                               0,                              //burstReq
                                               0,                              //reqMask
                                               1,                              //periphReq
                                               0,                              //DMA PERI ACK
                                               RTE_GSPI_MASTER_CHNL_UDMA_TX_CH //dmaCh
                                             },
                                             RTE_GSPI_MASTER_CHNL_UDMA_TX_CH,
                                             GSPI_MASTER_UDMA_Tx_Event };
#endif
#if (RTE_GSPI_MASTER_CHNL_UDMA_RX_EN == 1)
void GSPI_MASTER_UDMA_Rx_Event(uint32_t event, uint8_t dmaCh);
static GSPI_DMA GSPI_MASTER_UDMA_RX_CHNL = { { 0,
                                               0,
                                               0,
                                               0,
                                               1,
                                               0, //DMA ACK
                                               RTE_GSPI_MASTER_CHNL_UDMA_RX_CH },
                                             RTE_GSPI_MASTER_CHNL_UDMA_RX_CH,
                                             GSPI_MASTER_UDMA_Rx_Event };
#endif

// GSPI_MASTER Resources
static const GSPI_RESOURCES GSPI_MASTER_Resources = {
  GSPI0,
  GSPI0_IRQn,

#if (RTE_GSPI_MASTER_CHNL_UDMA_RX_EN)
  &GSPI_MASTER_UDMA_RX_CHNL,
#else
  NULL,
#endif

#if (RTE_GSPI_MASTER_CHNL_UDMA_TX_EN)
  &GSPI_MASTER_UDMA_TX_CHNL,
#else
  NULL,
#endif
  &GSPI_MASTER_Info,
  &GSPI_MASTER_TransferInfo,
  //pins
  &gspi_clock,
  &gspi_cs0,
#ifdef GSPI_MULTI_SLAVE
  &gspi_cs1,
  &gspi_cs2,
#endif
  &gspi_mosi,
  &gspi_miso,            //pins end
  RTE_GSPI_CLOCK_SOURCE, //clock
  //thresholds
  &gspi_threshold,
};
#endif /* GSPI_Master */

ARM_DRIVER_VERSION GSPI_MASTER_GetVersion(void)
{
  return DriverVersion;
}

ARM_SPI_CAPABILITIES GSPI_MASTER_GetCapabilities(void)
{
  return DriverCapabilities;
}
ARM_SPI_STATUS GSPI_GetStatus(const GSPI_RESOURCES *gspi)
{
  ARM_SPI_STATUS status;

  return status;
}

uint8_t gspi_slavenumber = 0xA5;

void RSI_GSPI_SetSlaveSelectNumber(uint8_t slavenumber)
{
  gspi_slavenumber = slavenumber;
}
uint8_t RSI_GSPI_GetSlaveSelectNumber(void)
{
  return gspi_slavenumber;
}

#if (RTE_GSPI_MASTER_CHNL_UDMA_TX_EN == 1)
void GSPI_MASTER_UDMA_Tx_Event(uint32_t event, uint8_t dmaCh)
{
#if defined(A11_ROM) && defined(ROMDRIVER_PRESENT)
  ROMAPI_GSPI_API->GSPI_UDMA_Tx_Event(event, dmaCh, &GSPI_MASTER_Resources);
#else
  GSPI_UDMA_Tx_Event(event, dmaCh, &GSPI_MASTER_Resources);
#endif
}
#endif

#if (RTE_GSPI_MASTER_CHNL_UDMA_RX_EN == 1)
void GSPI_MASTER_UDMA_Rx_Event(uint32_t event, uint8_t dmaCh)
{
#if defined(A11_ROM) && defined(ROMDRIVER_PRESENT)
  ROMAPI_GSPI_API->GSPI_UDMA_Rx_Event(event, dmaCh, &GSPI_MASTER_Resources);
#else
  GSPI_UDMA_Rx_Event(event, dmaCh, &GSPI_MASTER_Resources);
#endif
}
#endif

#if RTE_GSPI_MASTER
static int32_t GSPI_MASTER_Initialize(ARM_SPI_SignalEvent_t pSignalEvent)
{
#if defined(A11_ROM) && defined(ROMDRIVER_PRESENT)
  return ROMAPI_GSPI_API
    ->GSPI_Initialize(pSignalEvent, &GSPI_MASTER_Resources, &UDMA0_Resources, UDMA0_Table, &udmaHandle0, dma_rom_buff0);
#else
  return GSPI_Initialize(pSignalEvent,
                         &GSPI_MASTER_Resources,
                         &UDMA0_Resources,
                         UDMA0_Table,
                         &udmaHandle0,
                         dma_rom_buff0);
#endif
}

static int32_t GSPI_MASTER_Uninitialize(void)
{
#if defined(CHIP_9118)
  RSI_PS_M4ssPeriPowerDown(M4SS_PWRGATE_ULP_PERI2);
#else
  RSI_PS_M4ssPeriPowerDown(M4SS_PWRGATE_ULP_EFUSE_PERI);
#endif
#if defined(A11_ROM) && defined(ROMDRIVER_PRESENT)
  return ROMAPI_GSPI_API->GSPI_Uninitialize(&GSPI_MASTER_Resources, &UDMA0_Resources);
#else
  return GSPI_Uninitialize(&GSPI_MASTER_Resources, &UDMA0_Resources);
#endif
}

static int32_t GSPI_MASTER_PowerControl(ARM_POWER_STATE state)
{
#if defined(A11_ROM) && defined(ROMDRIVER_PRESENT)
  return ROMAPI_GSPI_API->GSPI_PowerControl(state, &GSPI_MASTER_Resources);
#else
  return GSPI_PowerControl(state, &GSPI_MASTER_Resources);
#endif
}

static int32_t GSPI_MASTER_Send(const void *data, uint32_t num)
{
#if defined(A11_ROM) && defined(ROMDRIVER_PRESENT)
  return ROMAPI_GSPI_API->GSPI_Send(data, num, &GSPI_MASTER_Resources, &UDMA0_Resources, udma0_chnl_info, udmaHandle0);
#else
  return GSPI_Send(data, num, &GSPI_MASTER_Resources, &UDMA0_Resources, udma0_chnl_info, udmaHandle0);
#endif
}

static int32_t GSPI_MASTER_Receive(void *data, uint32_t num)
{
#if defined(A11_ROM) && defined(ROMDRIVER_PRESENT)
  return ROMAPI_GSPI_API
    ->GSPI_Receive(data, num, &GSPI_MASTER_Resources, &UDMA0_Resources, udma0_chnl_info, udmaHandle0);
#else
  return GSPI_Receive(data, num, &GSPI_MASTER_Resources, &UDMA0_Resources, udma0_chnl_info, udmaHandle0);
#endif
}

static int32_t GSPI_MASTER_Transfer(const void *data_out, void *data_in, uint32_t num)
{
#if defined(A11_ROM) && defined(ROMDRIVER_PRESENT)
  return ROMAPI_GSPI_API
    ->GSPI_Transfer(data_out, data_in, num, &GSPI_MASTER_Resources, &UDMA0_Resources, udma0_chnl_info, udmaHandle0);
#else
  return GSPI_Transfer(data_out, data_in, num, &GSPI_MASTER_Resources, &UDMA0_Resources, udma0_chnl_info, udmaHandle0);
#endif
}

static uint32_t GSPI_MASTER_GetDataCount(void)
{
#if defined(A11_ROM) && defined(ROMDRIVER_PRESENT)
  return ROMAPI_GSPI_API->GSPI_GetDataCount(&GSPI_MASTER_Resources);
#else
  return GSPI_GetDataCount(&GSPI_MASTER_Resources);
#endif
}

static int32_t GSPI_MASTER_Control(uint32_t control, uint32_t arg)
{
  uint32_t gspi_get_clock = 0;
  gspi_get_clock          = RSI_CLK_GetBaseClock(M4_GSPI);
#ifdef GSPI_MULTI_SLAVE
  if ((control & ARM_SPI_CONTROL_Msk) == ARM_SPI_CONTROL_SS) {
    gspi_slavenumber = RSI_GSPI_GetSlaveSelectNumber();
    /* check slave number is valied or not */
    if (gspi_slavenumber == 0xA5) {
      /* Update the slave number by using RSI_GSPI_SetSlaveSelectNumber API */
      return RSI_MUTLI_SLAVE_SELECT_ERROR;
    }
  }
#endif
#if (defined(ROMDRIVER_PRESENT))
#if ((defined(A11_ROM)) && (defined(CHIP_917)) && (defined(CHIP_917B0)))
  return ROMAPI_GSPI_API->GSPI_Control(control, arg, &GSPI_MASTER_Resources, gspi_get_clock, gspi_slavenumber);
#else
  return ROMAPI_GSPI_API->GSPI_Control(control, arg, &GSPI_MASTER_Resources, gspi_get_clock);
#endif
#else
  GSPI_Control(control, arg, &GSPI_MASTER_Resources, gspi_get_clock, gspi_slavenumber);
#endif
}

static ARM_SPI_STATUS GSPI_MASTER_GetStatus(void)
{
  return GSPI_GetStatus(&GSPI_MASTER_Resources);
}

void GSPI_MASTER_IRQHandler(void)
{
#if defined(A11_ROM) && defined(ROMDRIVER_PRESENT)
  ROMAPI_GSPI_API->GSPI_IRQHandler(&GSPI_MASTER_Resources);
#else
  GSPI_IRQHandler(&GSPI_MASTER_Resources);
#endif
}

ARM_DRIVER_SPI Driver_GSPI_MASTER = { GSPI_MASTER_GetVersion,   GSPI_MASTER_GetCapabilities, GSPI_MASTER_Initialize,
                                      GSPI_MASTER_Uninitialize, GSPI_MASTER_PowerControl,    GSPI_MASTER_Send,
                                      GSPI_MASTER_Receive,      GSPI_MASTER_Transfer,        GSPI_MASTER_GetDataCount,
                                      GSPI_MASTER_Control,      GSPI_MASTER_GetStatus };

// To get the Frame length
uint32_t GSPI_GetFrameLength(void)
{
  return GSPI_MASTER_Resources.reg->GSPI_WRITE_DATA2_b.GSPI_MANUAL_WRITE_DATA2;
}

// To enable/disable the swapping of byte for read and write operation
int32_t GSPI_SwapReadWriteByte(boolean_t read, boolean_t write)
{
  if (!(GSPI_MASTER_Resources.info->state & SPI_POWERED)) {
    return ARM_DRIVER_ERROR;
  }
  if (GSPI_MASTER_Resources.info->status.busy) {
    return ARM_DRIVER_ERROR_BUSY;
  }
  if (read) {
    GSPI_MASTER_Resources.reg->GSPI_CONFIG2 &= ~BIT(4);
  } else {
    GSPI_MASTER_Resources.reg->GSPI_CONFIG2 |= BIT(4);
  }
  if (write) {
    GSPI_MASTER_Resources.reg->GSPI_CONFIG2 &= ~BIT(0);
  } else {
    GSPI_MASTER_Resources.reg->GSPI_CONFIG2 |= BIT(0);
  }
  return ARM_DRIVER_OK;
}

//To get the transfer count
uint32_t GSPI_GetTxCount(void)
{
  if (!(GSPI_MASTER_Resources.info->state & SPI_CONFIGURED)) {
    return 0U;
  }
  return GSPI_MASTER_Resources.xfer->tx_cnt;
}

//To get the GSPI state, initialized or not
uint8_t GSPI_GetInitState(void)
{
  return (GSPI_MASTER_Resources.info->state & SPI_INITIALIZED);
}
#endif
