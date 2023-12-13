/*******************************************************************************
* @file  rsi_gspi.c
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

// Include Files

#include "rsi_ccp_user_config.h"

#ifndef ROMDRIVER_PRESENT
#include "rsi_rom_clks.h"
#include "rsi_rom_egpio.h"
#include "GSPI.h"
#ifndef SL_SI91X_GSPI_DMA
#include "rsi_rom_udma_wrapper.h"
#endif
#ifdef SL_SI91X_GSPI_DMA
#include "sl_si91x_dma.h"
#include "rsi_gspi.h"
#define DMA_INSTANCE 0
#endif
#include "rsi_gspi.h"

/*==============================================*/
/**
 * @fn          int32_t GSPI_Initialize(ARM_SPI_SignalEvent_t cb_event,
 *                       const GSPI_RESOURCES *gspi,
 *                       UDMA_RESOURCES *udma,
 *                       RSI_UDMA_DESC_T *UDMA_Table,
 *                       RSI_UDMA_HANDLE_T *udmaHandle,
 *                       uint32_t *mem)
 * @brief		     This API is used to Initialize the GSPI interface.
 * note: By default GSPI initialize, transfer, send and receive APIs use rsi_udma_wrapper.c
 * drivers. SL_DMA driver can be enabled by defining SL_SI91X_GSPI_DMA to 1.
 * rsi_udma_wrapper drivers can be executed from ROM. SL_DMA drivers cannot be executed
 * from ROM.
 * @param[in]	   cb_event    : Pointer to the ARM_SPI_SignalEvent_t
 * @param[in]	   gspi        : Pointer to the GSPI resources
 * @param[in]    udma        : Pointer to the UDMA resources
 * @param[in]    UDMA_Table  : Pointer to the RSI_UDMA_DESC_T
 * @param[in]    mem         : Pointer to the memory Buffer
 * @return 		   excecution status
 */
int32_t GSPI_Initialize(ARM_SPI_SignalEvent_t cb_event,
                        const GSPI_RESOURCES *gspi,
                        UDMA_RESOURCES *udma,
                        RSI_UDMA_DESC_T *UDMA_Table,
                        RSI_UDMA_HANDLE_T *udmaHandle,
                        uint32_t *mem)
{
#ifdef SL_SI91X_GSPI_DMA
  //Added to suppress unused variable warning
  (void)udma;
  (void)udmaHandle;
  (void)UDMA_Table;
  (void)mem;
#endif
#if defined(SLI_SI917)
  RSI_PS_M4ssPeriPowerUp(M4SS_PWRGATE_ULP_EFUSE_PERI);
#else
  RSI_PS_M4ssPeriPowerUp(M4SS_PWRGATE_ULP_PERI2);
#endif

  if (gspi->info->state & SPI_INITIALIZED) {
    return ARM_DRIVER_OK;
  }

  // Initialize SPI Run-Time Resources
  gspi->info->cb_event          = cb_event;
  gspi->info->status.busy       = 0U;
  gspi->info->status.data_lost  = 0U;
  gspi->info->status.mode_fault = 0U;

  // Clear transfer information
  memset(gspi->xfer, 0, sizeof(GSPI_TRANSFER_INFO));

  gspi->info->state = SPI_INITIALIZED;

  // Pin Mux
  if ((gspi->reg == GSPI0)) {
    //Configure clock pin
    if (gspi->io.clock->pin > 63) {
      RSI_EGPIO_UlpPadReceiverEnable((uint8_t)(gspi->io.clock->pin - 64));
      RSI_EGPIO_SetPinMux(EGPIO1, 0, (uint8_t)(gspi->io.clock->pin - 64), 6);
    }
    if (gspi->io.clock->pad_sel != 0) {
      RSI_EGPIO_PadSelectionEnable(gspi->io.clock->pad_sel);
    }
    if (gspi->io.clock->pin >= 25 && gspi->io.clock->pin <= 30) {
      RSI_EGPIO_HostPadsGpioModeEnable(gspi->io.clock->pin);
    }
    RSI_EGPIO_PadReceiverEnable(gspi->io.clock->pin);
    RSI_EGPIO_SetPinMux(EGPIO, gspi->io.clock->port, gspi->io.clock->pin, gspi->io.clock->mode);

    if (gspi->io.cs0 != NULL) {
      //configure cs0 pin
      if (gspi->io.cs0->pin > 63) {
        RSI_EGPIO_UlpPadReceiverEnable((uint8_t)(gspi->io.cs0->pin - 64));
        RSI_EGPIO_SetPinMux(EGPIO1, 0, (uint8_t)(gspi->io.cs0->pin - 64), 6);
      }
      if (gspi->io.cs0->pad_sel != 0) {
        RSI_EGPIO_PadSelectionEnable(gspi->io.cs0->pad_sel);
      }
      if (gspi->io.cs0->pin >= 25 && gspi->io.cs0->pin <= 30) {
        RSI_EGPIO_HostPadsGpioModeEnable(gspi->io.cs0->pin);
      }
      RSI_EGPIO_PadReceiverEnable(gspi->io.cs0->pin);
      RSI_EGPIO_SetPinMux(EGPIO, gspi->io.cs0->port, gspi->io.cs0->pin, gspi->io.cs0->mode);
    }
    if (gspi->io.cs1 != NULL) {
      //configure cs1 pin
      if (gspi->io.cs1->pin > 63) {
        RSI_EGPIO_UlpPadReceiverEnable((uint8_t)(gspi->io.cs1->pin - 64));
        RSI_EGPIO_SetPinMux(EGPIO1, 0, (uint8_t)(gspi->io.cs1->pin - 64), 6);
      }
      if (gspi->io.cs1->pad_sel != 0) {
        RSI_EGPIO_PadSelectionEnable(gspi->io.cs1->pad_sel);
      }
      if (gspi->io.cs1->pin >= 25 && gspi->io.cs1->pin <= 30) {
        RSI_EGPIO_HostPadsGpioModeEnable(gspi->io.cs1->pin);
      }
      RSI_EGPIO_PadReceiverEnable(gspi->io.cs1->pin);
      RSI_EGPIO_SetPinMux(EGPIO, gspi->io.cs1->port, gspi->io.cs1->pin, gspi->io.cs1->mode);
    }

    if (gspi->io.cs2 != NULL) {
      //configure cs2 pin
      if (gspi->io.cs2->pin > 63) {
        RSI_EGPIO_UlpPadReceiverEnable((uint8_t)(gspi->io.cs2->pin - 64));
        RSI_EGPIO_SetPinMux(EGPIO1, 0, (uint8_t)(gspi->io.cs2->pin - 64), 6);
      }
      if (gspi->io.cs2->pad_sel != 0) {
        RSI_EGPIO_PadSelectionEnable(gspi->io.cs2->pad_sel);
      }
      if (gspi->io.cs2->pin >= 25 && gspi->io.cs2->pin <= 30) {
        RSI_EGPIO_HostPadsGpioModeEnable(gspi->io.cs2->pin);
      }
      RSI_EGPIO_PadReceiverEnable(gspi->io.cs2->pin);
      RSI_EGPIO_SetPinMux(EGPIO, gspi->io.cs2->port, gspi->io.cs2->pin, gspi->io.cs2->mode);
    }

    //configure MOSI pin
    if (gspi->io.mosi->pin > 63) {
      RSI_EGPIO_UlpPadReceiverEnable((uint8_t)(gspi->io.mosi->pin - 64));
      RSI_EGPIO_SetPinMux(EGPIO1, 0, (uint8_t)(gspi->io.mosi->pin - 64), 6);
    }
    if (gspi->io.mosi->pad_sel != 0) {
      RSI_EGPIO_PadSelectionEnable(gspi->io.mosi->pad_sel);
    }
    if (gspi->io.mosi->pin >= 25 && gspi->io.mosi->pin <= 30) {
      RSI_EGPIO_HostPadsGpioModeEnable(gspi->io.mosi->pin);
    }
    RSI_EGPIO_PadReceiverEnable(gspi->io.mosi->pin);
    RSI_EGPIO_SetPinMux(EGPIO, gspi->io.mosi->port, gspi->io.mosi->pin, gspi->io.mosi->mode);

    //configure MISO pin
    if (gspi->io.miso->pin > 63) {
      RSI_EGPIO_UlpPadReceiverEnable((uint8_t)(gspi->io.miso->pin - 64));
      RSI_EGPIO_SetPinMux(EGPIO1, 0, (uint8_t)(gspi->io.miso->pin - 64), 6);
    }
    if (gspi->io.miso->pad_sel != 0) {
      RSI_EGPIO_PadSelectionEnable(gspi->io.miso->pad_sel);
    }
    if (gspi->io.miso->pin >= 25 && gspi->io.miso->pin <= 30) {
      RSI_EGPIO_HostPadsGpioModeEnable(gspi->io.miso->pin);
    }
    RSI_EGPIO_PadReceiverEnable(gspi->io.miso->pin);
    RSI_EGPIO_SetPinMux(EGPIO, gspi->io.miso->port, gspi->io.miso->pin, gspi->io.miso->mode);
  }
  // DMA Initialize
  if (gspi->tx_dma || gspi->rx_dma) {
    if ((gspi->rx_dma != NULL) || (gspi->tx_dma != NULL)) {
      // Enable DMA instance
      if ((gspi->reg == GSPI0)) {
#ifdef SL_SI91X_GSPI_DMA
        sl_dma_init_t dma_init;
        dma_init.dma_number = DMA_INSTANCE;
        if (sl_si91x_dma_init(&dma_init)) {
          return ARM_DRIVER_ERROR;
        }
#else
        //if using uart debug init in application this UDMA0_Uninitialize is power gating the uart peri power,then disabled this  ,
        // if other issue occures enable UDMA0_Uninitialize and try
        // DMA0
        *udmaHandle = UDMAx_Initialize(udma, UDMA_Table, udmaHandle, mem);
#endif
      }
    }
  }
  gspi->info->state = SPI_INITIALIZED;

  return ARM_DRIVER_OK;
}

/*==============================================*/
/**
 * @fn          int32_t GSPI_Uninitialize(const GSPI_RESOURCES *gspi, UDMA_RESOURCES *udma)
 * @brief		     This API is used to UnInitialize the GSPI interface.
 * @param[in]	   gspi        : Pointer to the GSPI resources
 * @param[in]    udma        : Pointer to the UDMA resources
 * @return 		   excecution status
 */
int32_t GSPI_Uninitialize(const GSPI_RESOURCES *gspi, UDMA_RESOURCES *udma)
{
#ifdef SL_SI91X_GSPI_DMA
  //Added to suppress unused variable warning
  (void)udma;
#endif
  // Clear SPI state
  gspi->info->state = 0U;

  if ((gspi->rx_dma != NULL) || (gspi->tx_dma != NULL)) {
    // Diasable DMA instance
    if ((gspi->reg == GSPI0)) {
#ifdef SL_SI91X_GSPI_DMA
      if (sl_si91x_dma_deinit(DMA_INSTANCE)) {
        return ARM_DRIVER_ERROR;
      }
#else
      UDMAx_Uninitialize(udma);
#endif
    }
  }
  return ARM_DRIVER_OK;
}

/*==============================================*/
/**
 * @fn          int32_t GSPI_PowerControl(ARM_POWER_STATE state, const GSPI_RESOURCES *gspi)
 * @brief		     This API is used control the GSPI interface power.
 * @param[in]    state       : Power State
 * @param[in]	   gspi        : Pointer to the GSPI resources
 * @return 		   excecution status
 */
int32_t GSPI_PowerControl(ARM_POWER_STATE state, const GSPI_RESOURCES *gspi)
{
  switch (state) {
    case ARM_POWER_OFF:

      NVIC_DisableIRQ(gspi->irq_num); // Disable SPI IRQ in NVIC

      // Reset SPI Run-Time Resources
      gspi->info->status.busy       = 0U;
      gspi->info->status.data_lost  = 0U;
      gspi->info->status.mode_fault = 0U;

      // Clear pending SPI interrupts in NVIC
      NVIC_ClearPendingIRQ(gspi->irq_num);

      // Clear transfer information
      memset(gspi->xfer, 0, sizeof(GSPI_TRANSFER_INFO));

      gspi->info->state &= (uint8_t)~SPI_POWERED; // SPI is not powered

// power down
#if defined(SLI_SI917)
      RSI_PS_M4ssPeriPowerDown(M4SS_PWRGATE_ULP_EFUSE_PERI);
#else
      RSI_PS_M4ssPeriPowerDown(M4SS_PWRGATE_ULP_PERI2);
#endif

      break;

    case ARM_POWER_FULL:

      if ((gspi->info->state & SPI_INITIALIZED) == 0U) {
        return ARM_DRIVER_ERROR;
      }
      if ((gspi->info->state & SPI_POWERED) != 0U) {
        return ARM_DRIVER_OK;
      }

      // Configure the M4 Clock
      RSI_CLK_PeripheralClkEnable3(M4CLK, M4_SOC_CLK_FOR_OTHER_ENABLE);

      // Configure the GSPI Clocks
      RSI_CLK_GspiClkConfig(M4CLK, gspi->clock_source);

      gspi->reg->GSPI_BUS_MODE_b.SPI_HIGH_PERFORMANCE_EN = 0x1;

      // Reset SPI Run-Time Resources
      gspi->info->status.busy       = 0U;
      gspi->info->status.data_lost  = 0U;
      gspi->info->status.mode_fault = 0U;
      gspi->info->state |= SPI_POWERED; // SPI is powered

      NVIC_ClearPendingIRQ(gspi->irq_num);
      NVIC_EnableIRQ(gspi->irq_num); // Enable SPI IRQ in NVIC
      break;

    case ARM_POWER_LOW:
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  return ARM_DRIVER_OK;
}

/*==============================================*/
/**
 * @fn          int32_t GSPI_Control(uint32_t control, uint32_t arg, const GSPI_RESOURCES *gspi, uint32_t base_clock)
 * @brief		     This API is used control the GSPI interface.
 * @param[in]    control     : Operation
 * @param[in]    arg         : Argument of operation (optional)
 * @param[in]	   gspi        : Pointer to the GSPI resources
 * @param[in]    base_clock  : GSPI clock
 * @return 		   excecution status
 */
int32_t GSPI_Control(uint32_t control,
                     uint32_t arg,
                     const GSPI_RESOURCES *gspi,
                     uint32_t base_clock,
                     uint8_t slavenumber)
{
  uint32_t clk_div_factor = 0;
  uint32_t val            = 0;
  uint16_t data_bits      = 0;
  uint8_t cs_pin          = 0;

  if (!(gspi->info->state & SPI_POWERED)) {
    return ARM_DRIVER_ERROR;
  }
  if ((gspi->reg == GSPI0) && ((control & ARM_SPI_CONTROL_Msk) == ARM_SPI_MODE_SLAVE)) {
    return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  if ((control & ARM_SPI_CONTROL_Msk) == ARM_SPI_ABORT_TRANSFER) {
    memset(gspi->xfer, 0, sizeof(GSPI_TRANSFER_INFO));
    gspi->info->status.busy = 0U;
    return ARM_DRIVER_OK;
  }
  if (gspi->info->status.busy) {
    return ARM_DRIVER_ERROR_BUSY;
  }
  switch (control & ARM_SPI_CONTROL_Msk) {
    case ARM_SPI_MODE_INACTIVE:
      gspi->info->mode &= ~ARM_SPI_CONTROL_Msk;
      gspi->info->mode |= ARM_SPI_MODE_INACTIVE;
      gspi->info->state &= (uint8_t)~SPI_CONFIGURED;
      return ARM_DRIVER_OK;

    case ARM_SPI_MODE_MASTER:
      gspi->info->mode &= ~ARM_SPI_CONTROL_Msk;
      gspi->info->mode |= ARM_SPI_MODE_MASTER;
      gspi->info->state |= SPI_CONFIGURED;
      gspi->info->mode |= ARM_SPI_MODE_MASTER;
      goto set_speed;

    case ARM_SPI_MODE_SLAVE:
      return ARM_SPI_ERROR_MODE;

    case ARM_SPI_MODE_MASTER_SIMPLEX:
      return ARM_SPI_ERROR_MODE;

    case ARM_SPI_MODE_SLAVE_SIMPLEX:
      return ARM_SPI_ERROR_MODE;

    case ARM_SPI_SET_BUS_SPEED:
set_speed:
      clk_div_factor = (base_clock / arg);

      // NOTE : division factor is multiples of 2
      clk_div_factor = (clk_div_factor / 2);

      if (clk_div_factor < 1) {
        gspi->reg->GSPI_CLK_CONFIG_b.GSPI_CLK_EN = 0x1;

        gspi->reg->GSPI_CLK_CONFIG_b.GSPI_CLK_SYNC = 0x1;
      }
      // Update the clock rate to hardware
      gspi->reg->GSPI_CLK_DIV_b.GSPI_CLK_DIV_FACTOR = (uint8_t)clk_div_factor;
      break;

    case ARM_SPI_GET_BUS_SPEED: // Get Bus Speed in bps
      return gspi->reg->GSPI_CLK_DIV_b.GSPI_CLK_DIV_FACTOR;

    case ARM_SPI_SET_DEFAULT_TX_VALUE: // Set default Transmit value; arg = value
      gspi->xfer->def_val = (uint16_t)(arg & 0xFFFF);
      return ARM_DRIVER_OK;

    case ARM_SPI_CONTROL_SS:

      if (((gspi->info->mode & ARM_SPI_CONTROL_Msk) != ARM_SPI_MODE_MASTER)) {
        return ARM_DRIVER_ERROR;
      }
      val = (gspi->info->mode & ARM_SPI_SS_MASTER_MODE_Msk);

      if (arg == ARM_SPI_SS_INACTIVE) {
        if (val == ARM_SPI_SS_MASTER_HW_OUTPUT) {
          // Assert selected CS line
          gspi->reg->GSPI_CONFIG1_b.GSPI_MANUAL_CSN_SELECT = (unsigned int)(slavenumber & 0x03);
          gspi->reg->GSPI_CONFIG1_b.GSPI_MANUAL_CSN        = 0x1;
          while (!(gspi->reg->GSPI_STATUS & GSPI_MAN_CSN))
            ;
        } else {

          switch (slavenumber) {
            case GSPI_CS0:
              cs_pin = gspi->io.cs0->pin;
              break;
            case GSPI_CS1:
              cs_pin = gspi->io.cs1->pin;
              break;
            case GSPI_CS2:
              cs_pin = gspi->io.cs2->pin;
              break;
          }
          RSI_EGPIO_SetPin(EGPIO, 0, cs_pin, 1);

          gspi->reg->GSPI_CONFIG1_b.GSPI_MANUAL_CSN = 0;
          while ((gspi->reg->GSPI_STATUS & GSPI_MAN_CSN))
            ;
        }
      } else {
        if (val == ARM_SPI_SS_MASTER_HW_OUTPUT) {
          // Assert selected CS line
          gspi->reg->GSPI_CONFIG1_b.GSPI_MANUAL_CSN_SELECT = (unsigned int)(slavenumber & 0x03);
          gspi->reg->GSPI_CONFIG1_b.GSPI_MANUAL_CSN        = 0;
          while ((gspi->reg->GSPI_STATUS & GSPI_MAN_CSN))
            ;
        } else {
          switch (slavenumber) {
            case GSPI_CS0:
              cs_pin = gspi->io.cs0->pin;
              break;
            case GSPI_CS1:
              cs_pin = gspi->io.cs1->pin;
              break;
            case GSPI_CS2:
              cs_pin = gspi->io.cs2->pin;
              break;
          }
          RSI_EGPIO_SetPin(EGPIO, 0, cs_pin, 0);
        }
      }
      return ARM_DRIVER_OK;
  }

  if ((gspi->info->mode & ARM_SPI_CONTROL_Msk) == ARM_SPI_MODE_MASTER) {
    switch (control & ARM_SPI_SS_MASTER_MODE_Msk) {
      case ARM_SPI_SS_MASTER_UNUSED: // SPI Slave Select when Master: Not used (default)
                                     //unconfigure the slave selct pin
        RSI_EGPIO_SetPinMux(EGPIO, 0, cs_pin, 0);
        gspi->info->mode &= ~ARM_SPI_SS_MASTER_MODE_Msk;
        gspi->info->mode |= ARM_SPI_SS_MASTER_UNUSED;
        break;

      case ARM_SPI_SS_MASTER_HW_INPUT: // SPI Slave Select when Master: Hardware monitored Input
        gspi->info->mode &= ~ARM_SPI_SS_MASTER_MODE_Msk;
        return ARM_SPI_ERROR_SS_MODE;

      case ARM_SPI_SS_MASTER_SW: // SPI Slave Select when Master: Software controlled
        //configure Gpio pin output mode
        //configure cs pin
        //!pad selection,host pad gpio mode enables for host pad gpios are already pogrammed in initialization
        switch (slavenumber) {
          case GSPI_CS0:
            cs_pin = gspi->io.cs0->pin;
            if (gspi->io.cs0->pad_sel != 0) {
              RSI_EGPIO_PadSelectionEnable(gspi->io.cs0->pad_sel);
            }
            break;
          case GSPI_CS1:
            cs_pin = gspi->io.cs1->pin;
            if (gspi->io.cs1->pad_sel != 0) {
              RSI_EGPIO_PadSelectionEnable(gspi->io.cs1->pad_sel);
            }
            break;
          case GSPI_CS2:
            cs_pin = gspi->io.cs2->pin;
            if (gspi->io.cs2->pad_sel != 0) {
              RSI_EGPIO_PadSelectionEnable(gspi->io.cs2->pad_sel);
            }
            break;
        }
        if (cs_pin > 63) {
          RSI_EGPIO_SetPinMux(EGPIO1, 0, (uint8_t)(cs_pin - 64), 6);
        }
        RSI_EGPIO_SetPinMux(EGPIO, 0, cs_pin, 0); //gpio mode
        RSI_EGPIO_SetDir(EGPIO, 0, cs_pin, 0);    //set output direction
        RSI_EGPIO_SetPin(EGPIO, 0, cs_pin, 1);    //set pin value to 1
        gspi->info->mode |= ARM_SPI_SS_MASTER_SW;
        break;
      case ARM_SPI_SS_MASTER_HW_OUTPUT: // SPI Slave Select when Master: Hardware controlled Output
        gspi->info->mode &= ~ARM_SPI_SS_MASTER_MODE_Msk;
        if ((gspi->io.cs0->pin != (int)NULL) || (gspi->io.cs1->pin != (int)NULL) || (gspi->io.cs2->pin != (int)NULL)) {
          gspi->info->mode |= ARM_SPI_SS_MASTER_HW_OUTPUT;
        } else {
          return ARM_SPI_ERROR_SS_MODE;
        }
        break;
      default:
        break;
    }
  }
  // Frame format:
  switch (control & ARM_SPI_FRAME_FORMAT_Msk) {
    case ARM_SPI_CPOL0_CPHA0:
      gspi->reg->GSPI_BUS_MODE_b.GSPI_CLK_MODE_CSN0 = 0; //mode 0
      break;

    case ARM_SPI_CPOL0_CPHA1:
      return ARM_SPI_ERROR_FRAME_FORMAT;

    case ARM_SPI_CPOL1_CPHA0:
      return ARM_SPI_ERROR_FRAME_FORMAT;

    case ARM_SPI_CPOL1_CPHA1:
      gspi->reg->GSPI_BUS_MODE_b.GSPI_CLK_MODE_CSN0 = 0x1; //mode 3
      break;

    case ARM_SPI_TI_SSI:
      return ARM_SPI_ERROR_FRAME_FORMAT;

    case ARM_SPI_MICROWIRE:
      return ARM_SPI_ERROR_FRAME_FORMAT;

    default:
      return ARM_SPI_ERROR_FRAME_FORMAT;
  }
  // Swap the read data inside the GSPI controller it-self.
  gspi->reg->GSPI_CONFIG2 &= ~BIT(4);

  // Configure Number of Data Bits
  data_bits = ((control & ARM_SPI_DATA_BITS_Msk) >> ARM_SPI_DATA_BITS_Pos);
  if ((data_bits == 0) || (data_bits > 16)) {
    return ARM_SPI_ERROR_DATA_BITS;
  } else {
    // Update the number of Data Bits
    gspi->reg->GSPI_WRITE_DATA2_b.GSPI_MANUAL_WRITE_DATA2 = (unsigned int)(data_bits & 0x0F);
  }

  return ARM_DRIVER_OK;
}

/*==============================================*/
/**
 * @fn          int32_t GSPI_Send(const void *data,
 *                               uint32_t num,
 *								const GSPI_RESOURCES *gspi,
 *								UDMA_RESOURCES *udma,
 *								DMA_Channel_Info *chnl_info,
 *								RSI_UDMA_HANDLE_T udmaHandle)
 * @brief		     This API Starts sending data to GSPI transmitter.
 * @param[in]    data        : Pointer to buffer with data to send to SPI transmitter
 * @param[in]    num         : Number of data items to send
 * @param[in]	   gspi        : Pointer to the GSPI resources
 * @param[in]	   udma        : Pointer to the UDMA resources
 * @param[in]	   CHNL_INFO   : Pointer to the UDMA channel info
 * @param[in]    udmaHandle  : Pointer to UDMA Handle
 * @return 		   excecution status
 */
int32_t GSPI_Send(const void *data,
                  uint32_t num,
                  const GSPI_RESOURCES *gspi,
                  UDMA_RESOURCES *udma,
                  UDMA_Channel_Info *chnl_info,
                  RSI_UDMA_HANDLE_T udmaHandle)
{
#ifdef SL_SI91X_GSPI_DMA
  //Added to suppress unused variable warning
  (void)udma;
  (void)udmaHandle;
  (void)chnl_info;
  sl_status_t status;
#else
  volatile int32_t stat = 0;
#endif
  RSI_UDMA_CHA_CONFIG_DATA_T control = { 0 };

  uint16_t data_bits      = 0, data_16bit;
  gspi->info->status.busy = 0U;
  if ((data == NULL) || (num == 0U)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }
  if (!(gspi->info->state & SPI_CONFIGURED)) {
    return ARM_DRIVER_ERROR;
  }
  if (gspi->info->status.busy) {
    return ARM_DRIVER_ERROR_BUSY;
  }
  gspi->info->status.busy       = 1U;
  gspi->info->status.data_lost  = 0U;
  gspi->info->status.mode_fault = 0U;
  gspi->xfer->rx_buf            = NULL;
  gspi->xfer->num               = num;
  gspi->xfer->rx_cnt            = 0U;
  gspi->xfer->tx_cnt            = 0U;
  gspi->xfer->tx_buf            = (uint8_t *)data;

  data_bits = GSPI0->GSPI_WRITE_DATA2_b.GSPI_MANUAL_WRITE_DATA2;
  if (gspi->tx_dma) {
    if (num > 10) {
      // Configure the FIFO thresholds
      gspi->reg->GSPI_FIFO_THRLD_b.FIFO_AEMPTY_THRLD = (unsigned int)(gspi->threshold->gspi_aempty_threshold & 0x0F);
      gspi->reg->GSPI_FIFO_THRLD_b.FIFO_AFULL_THRLD  = (unsigned int)(gspi->threshold->gspi_afull_threshold & 0x0F);

      control.transferType = UDMA_MODE_BASIC;
      control.nextBurst    = 0;
      if (num < 1024) {
        control.totalNumOfDMATrans = (unsigned int)((num - 1) & 0x03FF);
      } else {
        control.totalNumOfDMATrans = 0x3FF;
      }
      control.rPower      = (unsigned int)(gspi->threshold->txdma_arb_size & 0x0F);
      control.srcProtCtrl = 0x0;
      control.dstProtCtrl = 0x0;
      if ((data_bits <= 8) && (data_bits != 0)) {
        //8-bit data frame
        control.srcSize = SRC_SIZE_8;
        control.srcInc  = SRC_INC_8;
        control.dstSize = DST_SIZE_8;
        control.dstInc  = DST_INC_NONE;
      } else {
        //16-bit data frame
        control.srcSize = SRC_SIZE_16;
        control.srcInc  = SRC_INC_16;
        control.dstSize = DST_SIZE_16;
        control.dstInc  = DST_INC_NONE;
      }
#if SL_SI91X_GSPI_DMA
      sl_dma_xfer_t dma_transfer_tx = { 0 };
      uint32_t channel              = gspi->tx_dma->channel + 1;
      uint32_t channel_priority     = gspi->tx_dma->chnl_cfg.channelPrioHigh;
      sl_dma_callback_t gspi_tx_callback;
      //Initialize sl_dma callback structure
      gspi_tx_callback.transfer_complete_cb = gspi_transfer_complete_callback;
      gspi_tx_callback.error_cb             = gspi_error_callback;
      //Initialize sl_dma transfer structure
      dma_transfer_tx.src_addr       = (uint32_t *)((uint32_t)(gspi->xfer->tx_buf));
      dma_transfer_tx.dest_addr      = (uint32_t *)((uint32_t) & (gspi->reg->GSPI_WRITE_FIFO));
      dma_transfer_tx.src_inc        = control.srcInc;
      dma_transfer_tx.dst_inc        = control.dstInc;
      dma_transfer_tx.xfer_size      = control.dstSize;
      dma_transfer_tx.transfer_count = num;
      dma_transfer_tx.transfer_type  = SL_DMA_MEMORY_TO_PERIPHERAL;
      dma_transfer_tx.dma_mode       = control.transferType;
      dma_transfer_tx.signal         = gspi->tx_dma->chnl_cfg.periAck;

      //Allocate DMA channel for Tx
      status = sl_si91x_dma_allocate_channel(DMA_INSTANCE, &channel, channel_priority);
      if (status && (status != SL_STATUS_DMA_CHANNEL_ALLOCATED)) {
        return ARM_DRIVER_ERROR;
      }
      //Register transfer complete and error callback
      if (sl_si91x_dma_register_callbacks(DMA_INSTANCE, channel, &gspi_tx_callback)) {
        return ARM_DRIVER_ERROR;
      }
      //Configure the channel for DMA transfer
      if (sl_si91x_dma_transfer(DMA_INSTANCE, channel, &dma_transfer_tx)) {
        return ARM_DRIVER_ERROR;
      }
#else
      stat = UDMAx_ChannelConfigure(udma,
                                    gspi->tx_dma->channel,
                                    (uint32_t)(gspi->xfer->tx_buf),
                                    (uint32_t) & (gspi->reg->GSPI_WRITE_FIFO),
                                    num,
                                    control,
                                    &gspi->tx_dma->chnl_cfg,
                                    gspi->tx_dma->cb_event,
                                    chnl_info,
                                    udmaHandle);
      if (stat == -1) {
        return ARM_DRIVER_ERROR;
      }
#endif
      GSPI0->GSPI_WRITE_DATA2_b.GSPI_MANUAL_WRITE_DATA2 = (unsigned int)(data_bits & 0x0F);
      gspi->reg->GSPI_CONFIG1_b.GSPI_MANUAL_WR          = 0x1; //write enable
      GSPI0->GSPI_WRITE_DATA2_b.USE_PREV_LENGTH         = 0x1;
#ifdef SL_SI91X_GSPI_DMA
      sl_si91x_dma_channel_enable(DMA_INSTANCE, gspi->tx_dma->channel + 1);
      sl_si91x_dma_enable(DMA_INSTANCE);
#else
      UDMAx_ChannelEnable(gspi->tx_dma->channel, udma, udmaHandle);
      UDMAx_DMAEnable(udma, udmaHandle);
#endif
    } else { // configure IO mode
      // Configure the FIFO thresholds in I/0 mode
      gspi->reg->GSPI_FIFO_THRLD_b.FIFO_AEMPTY_THRLD        = 0x1;
      gspi->reg->GSPI_FIFO_THRLD_b.FIFO_AFULL_THRLD         = 0xC;
      gspi->reg->GSPI_WRITE_DATA2_b.GSPI_MANUAL_WRITE_DATA2 = (unsigned int)(data_bits & 0x0F);
      gspi->reg->GSPI_CONFIG1_b.GSPI_MANUAL_WR              = 0x1; //write enable
      if ((data_bits <= 8U) && (data_bits != 0)) {
        (*(volatile uint8_t *)(0x45030000 + 0x80)) = *(gspi->xfer->tx_buf++); //rite first index data
      } else {
        data_16bit = *(gspi->xfer->tx_buf++);
        data_16bit |= (uint16_t)(*(gspi->xfer->tx_buf++) << 8U);
        (*(volatile uint16_t *)(0x45030000 + 0x80)) = data_16bit; //write first index data
      }
      while (gspi->reg->GSPI_STATUS & GSPI_BUSY_F)
        ;
      gspi->reg->GSPI_INTR_UNMASK |= GSPI_INTR_UNMASK_BIT;
    }
  } else {
    // Configure the FIFO thresholds in I/0 mode
    gspi->reg->GSPI_FIFO_THRLD_b.FIFO_AEMPTY_THRLD = 0x1;
    gspi->reg->GSPI_FIFO_THRLD_b.FIFO_AFULL_THRLD  = 0xC;
    gspi->reg->GSPI_INTR_UNMASK |= GSPI_INTR_UNMASK_BIT;
    gspi->reg->GSPI_WRITE_DATA2_b.GSPI_MANUAL_WRITE_DATA2 = (unsigned int)(data_bits & 0x0F);
    gspi->reg->GSPI_CONFIG1_b.GSPI_MANUAL_WR              = 0x1; //write enable
    if ((data_bits <= 8U) && (data_bits != 0)) {
      (*(volatile uint8_t *)(0x45030000 + 0x80)) = *(gspi->xfer->tx_buf++); //rite first index data
    } else {
      data_16bit = *(gspi->xfer->tx_buf++);
      data_16bit |= (uint16_t)(*(gspi->xfer->tx_buf++) << 8U);
      (*(volatile uint16_t *)(0x45030000 + 0x80)) = data_16bit; //write first index data
    }
    while (gspi->reg->GSPI_STATUS & GSPI_BUSY_F)
      ;
  }
  return ARM_DRIVER_OK;
}

/*==============================================*/
/**
 * @fn          int32_t GSPI_Receive( void *data,
 *                               uint32_t num,
 *								const GSPI_RESOURCES *gspi,
 *								UDMA_RESOURCES *udma,
 *								UDMA_Channel_Info *chnl_info,
 *								RSI_UDMA_HANDLE_T udmaHandle)
 * @brief		     Start receiving data from GSPI receiver.
 * @param[in]    data        : Pointer to buffer for data to receive from GSPI receiver
 * @param[in]    num         : Number of data items to receive
 * @param[in]	   gspi        : Pointer to the GSPI resources
 * @param[in]	   udma        : Pointer to the UDMA resources
 * @param[in]	   CHNL_INFO   : Pointer to the UDMA channel info
 * @param[in]    udmaHandle  : Pointer to UDMA Handle
 * @return 		   excecution status
 */
int32_t GSPI_Receive(void *data,
                     uint32_t num,
                     const GSPI_RESOURCES *gspi,
                     UDMA_RESOURCES *udma,
                     UDMA_Channel_Info *chnl_info,
                     RSI_UDMA_HANDLE_T udmaHandle)
{
#ifdef SL_SI91X_GSPI_DMA
  //Added to suppress unused variable warning
  (void)udma;
  (void)udmaHandle;
  (void)chnl_info;
  sl_status_t status;
#else
  volatile int32_t stat = 0;
#endif
  RSI_UDMA_CHA_CONFIG_DATA_T control = { 0 };
  uint8_t dummy_data                 = 0;
  uint16_t data_bits                 = 0;
  gspi->info->status.busy            = 0U;

  if ((data == NULL) || (num == 0U)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }
  if (!(gspi->info->state & SPI_CONFIGURED)) {
    return ARM_DRIVER_ERROR;
  }

  if (gspi->info->status.busy) {
    return ARM_DRIVER_ERROR_BUSY;
  }
  gspi->info->status.busy       = 1U;
  gspi->info->status.data_lost  = 0U;
  gspi->info->status.mode_fault = 0U;

  gspi->xfer->rx_buf = NULL;
  gspi->xfer->num    = num;
  gspi->xfer->rx_cnt = 0U;
  gspi->xfer->tx_cnt = 0U;

  gspi->info->receive_function = 1;

  gspi->xfer->rx_buf = (uint8_t *)data;

  data_bits = GSPI0->GSPI_WRITE_DATA2_b.GSPI_MANUAL_WRITE_DATA2;

  if ((gspi->rx_dma) || (gspi->tx_dma)) {
    if (num > 10) {
      // Configure the FIFO thresholds
      gspi->reg->GSPI_FIFO_THRLD_b.FIFO_AEMPTY_THRLD = (unsigned int)(gspi->threshold->gspi_aempty_threshold & 0x0F);
      gspi->reg->GSPI_FIFO_THRLD_b.FIFO_AFULL_THRLD  = (unsigned int)(gspi->threshold->gspi_afull_threshold & 0x0F);

      // Enable Full duplex mode
      gspi->reg->GSPI_CONFIG1_b.SPI_FULL_DUPLEX_EN = 0x1;
      if ((gspi->tx_dma)) {
        control.transferType = UDMA_MODE_BASIC;
        control.nextBurst    = 0;
        if (num < 1024) {
          control.totalNumOfDMATrans = (unsigned int)((num - 1) & 0x03FF);
        } else {
          control.totalNumOfDMATrans = 0x3FF;
        }
        control.rPower      = (unsigned int)(gspi->threshold->rxdma_arb_size & 0x0F);
        control.srcProtCtrl = 0x0;
        control.dstProtCtrl = 0x0;
        if ((data_bits <= 8) && (data_bits != 0)) {
          //8-bit data frame
          control.srcSize = SRC_SIZE_8;
          control.srcInc  = SRC_INC_8;
          control.dstSize = DST_SIZE_8;
          control.dstInc  = DST_INC_NONE;
        } else {
          //16-bit data frame
          control.srcSize = SRC_SIZE_16;
          control.srcInc  = SRC_INC_16;
          control.dstSize = DST_SIZE_16;
          control.dstInc  = DST_INC_NONE;
        }
#if SL_SI91X_GSPI_DMA
        sl_dma_xfer_t dma_transfer_tx = { 0 };
        uint32_t channel              = gspi->tx_dma->channel + 1;
        uint32_t channel_priority     = gspi->tx_dma->chnl_cfg.channelPrioHigh;
        sl_dma_callback_t gspi_tx_callback;
        //Initialize sl_dma callback structure
        gspi_tx_callback.transfer_complete_cb = gspi_transfer_complete_callback;
        gspi_tx_callback.error_cb             = gspi_error_callback;
        //Initialize sl_dma transfer structure
        dma_transfer_tx.src_addr       = (uint32_t *)((uint32_t)dummy_data);
        dma_transfer_tx.dest_addr      = (uint32_t *)((uint32_t) & (gspi->reg->GSPI_WRITE_FIFO));
        dma_transfer_tx.src_inc        = control.srcInc;
        dma_transfer_tx.dst_inc        = control.dstInc;
        dma_transfer_tx.xfer_size      = control.dstSize;
        dma_transfer_tx.transfer_count = num;
        dma_transfer_tx.transfer_type  = SL_DMA_MEMORY_TO_PERIPHERAL;
        dma_transfer_tx.dma_mode       = control.transferType;
        dma_transfer_tx.signal         = gspi->tx_dma->chnl_cfg.periAck;

        //Allocate DMA channel for Tx
        status = sl_si91x_dma_allocate_channel(DMA_INSTANCE, &channel, channel_priority);
        if (status && (status != SL_STATUS_DMA_CHANNEL_ALLOCATED)) {
          return ARM_DRIVER_ERROR;
        }
        //Register transfer complete and error callback
        if (sl_si91x_dma_register_callbacks(DMA_INSTANCE, channel, &gspi_tx_callback)) {
          return ARM_DRIVER_ERROR;
        }
        //Configure the channel for DMA transfer
        if (sl_si91x_dma_transfer(DMA_INSTANCE, channel, &dma_transfer_tx)) {
          return ARM_DRIVER_ERROR;
        }
#else
        stat = UDMAx_ChannelConfigure(udma,
                                      gspi->tx_dma->channel,
                                      (dummy_data),
                                      (uint32_t) & (gspi->reg->GSPI_WRITE_FIFO),
                                      num,
                                      control,
                                      &gspi->tx_dma->chnl_cfg,
                                      gspi->tx_dma->cb_event,
                                      chnl_info,
                                      udmaHandle);
        if (stat == -1) {
          return ARM_DRIVER_ERROR;
        }
#endif
        GSPI0->GSPI_WRITE_DATA2_b.GSPI_MANUAL_WRITE_DATA2 = (unsigned int)(data_bits & 0x0F);
        gspi->reg->GSPI_CONFIG1_b.GSPI_MANUAL_WR          = 0x1; //write enable
        GSPI0->GSPI_WRITE_DATA2_b.USE_PREV_LENGTH         = 0x1;
      } else {
        return ARM_DRIVER_ERROR;
      }
      if ((gspi->rx_dma)) {
        control.transferType = UDMA_MODE_BASIC;
        control.nextBurst    = 0;
        if (num < 1024) {
          control.totalNumOfDMATrans = (unsigned int)((num - 1) & 0x03FF);
        } else {
          control.totalNumOfDMATrans = 0x3FF;
        }
        control.rPower      = ARBSIZE_1;
        control.srcProtCtrl = 0x0;
        control.dstProtCtrl = 0x0;
        if ((data_bits <= 8) && (data_bits != 0)) {
          //8-bit data frame
          control.srcSize = SRC_SIZE_8;
          control.srcInc  = SRC_INC_NONE;
          control.dstSize = DST_SIZE_8;
          control.dstInc  = DST_INC_8;
        } else {
          // 16-bit data frame
          control.srcSize = SRC_SIZE_16;
          control.srcInc  = SRC_INC_NONE;
          control.dstSize = DST_SIZE_16;
          control.dstInc  = DST_INC_16;
        }
#ifdef SL_SI91X_GSPI_DMA
        sl_dma_xfer_t dma_transfer_rx = { 0 };
        uint32_t channel              = gspi->rx_dma->channel + 1;
        uint32_t channel_priority     = gspi->rx_dma->chnl_cfg.channelPrioHigh;
        sl_dma_callback_t gspi_rx_callback;
        //Initialize sl_dma callback structure
        gspi_rx_callback.transfer_complete_cb = gspi_transfer_complete_callback;
        gspi_rx_callback.error_cb             = gspi_error_callback;
        //Initialize sl_dma transfer structure
        dma_transfer_rx.src_addr       = (uint32_t *)((uint32_t) & (gspi->reg->GSPI_READ_FIFO));
        dma_transfer_rx.dest_addr      = (uint32_t *)((uint32_t)(gspi->xfer->rx_buf));
        dma_transfer_rx.src_inc        = control.srcInc;
        dma_transfer_rx.dst_inc        = control.dstInc;
        dma_transfer_rx.xfer_size      = control.dstSize;
        dma_transfer_rx.transfer_count = num;
        dma_transfer_rx.transfer_type  = SL_DMA_PERIPHERAL_TO_MEMORY;
        dma_transfer_rx.dma_mode       = control.transferType;
        dma_transfer_rx.signal         = gspi->rx_dma->chnl_cfg.periAck;

        //Allocate DMA channel for Rx
        status = sl_si91x_dma_allocate_channel(DMA_INSTANCE, &channel, channel_priority);
        if (status && (status != SL_STATUS_DMA_CHANNEL_ALLOCATED)) {
          return ARM_DRIVER_ERROR;
        }
        //Register transfer complete and error callback
        if (sl_si91x_dma_register_callbacks(DMA_INSTANCE, channel, &gspi_rx_callback)) {
          return ARM_DRIVER_ERROR;
        }
        //Configure the channel for DMA transfer
        if (sl_si91x_dma_transfer(DMA_INSTANCE, channel, &dma_transfer_rx)) {
          return ARM_DRIVER_ERROR;
        }
#else
        stat = UDMAx_ChannelConfigure(udma,
                                      gspi->rx_dma->channel,
                                      (uint32_t) & (gspi->reg->GSPI_READ_FIFO),
                                      (uint32_t)(gspi->xfer->rx_buf),
                                      num,
                                      control,
                                      &gspi->rx_dma->chnl_cfg,
                                      gspi->rx_dma->cb_event,
                                      chnl_info,
                                      udmaHandle);
        if (stat == -1) {
          return ARM_DRIVER_ERROR;
        }
#endif
        gspi->reg->GSPI_CONFIG1_b.GSPI_MANUAL_RD = 0x1;
      } else {
        return ARM_DRIVER_ERROR;
      }
#ifdef SL_SI91X_GSPI_DMA
      sl_si91x_dma_channel_enable(DMA_INSTANCE, gspi->tx_dma->channel + 1);
      sl_si91x_dma_channel_enable(DMA_INSTANCE, gspi->rx_dma->channel + 1);
      sl_si91x_dma_enable(DMA_INSTANCE);
#else
      UDMAx_ChannelEnable(gspi->tx_dma->channel, udma, udmaHandle);
      UDMAx_ChannelEnable(gspi->rx_dma->channel, udma, udmaHandle);
      UDMAx_DMAEnable(udma, udmaHandle);
#endif
    } else {
      // receive data in IO mode
      // Configure the FIFO thresholds in I/0 mode
      gspi->reg->GSPI_FIFO_THRLD_b.FIFO_AEMPTY_THRLD        = 0x1;
      gspi->reg->GSPI_FIFO_THRLD_b.FIFO_AFULL_THRLD         = 0xC;
      gspi->reg->GSPI_WRITE_DATA2_b.GSPI_MANUAL_WRITE_DATA2 = (unsigned int)(data_bits & 0x0F);
      gspi->reg->GSPI_CONFIG1_b.GSPI_MANUAL_RD              = 0x1; //read enable
      gspi->reg->GSPI_CONFIG3_b.SPI_MANUAL_RD_LNTH_TO_BC    = 1;
      while (gspi->reg->GSPI_STATUS & GSPI_BUSY_F)
        ;
      gspi->reg->GSPI_INTR_UNMASK |= GSPI_INTR_UNMASK_BIT;
    }
  } else {
    gspi->reg->GSPI_FIFO_THRLD_b.FIFO_AEMPTY_THRLD = 0x1;
    gspi->reg->GSPI_FIFO_THRLD_b.FIFO_AFULL_THRLD  = 0xC;
    gspi->reg->GSPI_INTR_UNMASK |= GSPI_INTR_UNMASK_BIT;
    gspi->reg->GSPI_WRITE_DATA2_b.GSPI_MANUAL_WRITE_DATA2 = (unsigned int)(data_bits & 0x0F);
    gspi->reg->GSPI_CONFIG1_b.GSPI_MANUAL_RD              = 0x1; //read enable
    gspi->reg->GSPI_CONFIG3_b.SPI_MANUAL_RD_LNTH_TO_BC    = 1;
    while (gspi->reg->GSPI_STATUS & GSPI_BUSY_F)
      ;
  }
  return ARM_DRIVER_OK;
}

/*==============================================*/
/**
 * @fn          int32_t GSPI_Transfer(const void *data_out,
 *								void *data_in,
 *								uint32_t num,
 *								const GSPI_RESOURCES *gspi,
 *								UDMA_RESOURCES *udma,
 *								UDMA_Channel_Info *chnl_info,
 *								RSI_UDMA_HANDLE_T udmaHandle)
 * @brief		     Start sending/receiving data to/from GSPI transmitter/receiver.
 * @param[in]    data_out    : Pointer to buffer with data to send to GSPI transmitter
 * @param[in]    data_in     : Pointer to buffer for data to receive from GSPI receiver
 * @param[in]    num         : Number of data items to transfer
 * @param[in]	   gspi        : Pointer to the GSPI resources
 * @param[in]	   udma        : Pointer to the UDMA resources
 * @param[in]	   CHNL_INFO   : Pointer to the UDMA channel info
 * @param[in]    udmaHandle  : Pointer to UDMA Handle
 * @return 		   excecution status
 */
int32_t GSPI_Transfer(const void *data_out,
                      void *data_in,
                      uint32_t num,
                      const GSPI_RESOURCES *gspi,
                      UDMA_RESOURCES *udma,
                      UDMA_Channel_Info *chnl_info,
                      RSI_UDMA_HANDLE_T udmaHandle)
{
#ifdef SL_SI91X_GSPI_DMA
  //Added to suppress unused variable warning
  (void)udma;
  (void)udmaHandle;
  (void)chnl_info;
  sl_status_t status;
#else
  volatile int32_t stat = 0;
#endif
  RSI_UDMA_CHA_CONFIG_DATA_T control = { 0 };
  uint16_t data_bits                 = 0;
  uint16_t data_16bit;

  if ((data_out == NULL) || (data_in == NULL) || (num == 0U)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }
  if (!(gspi->info->state & SPI_CONFIGURED)) {
    return ARM_DRIVER_ERROR;
  }
  if (gspi->info->status.busy) {
    return ARM_DRIVER_ERROR_BUSY;
  }
  gspi->info->status.busy       = 1U;
  gspi->info->status.data_lost  = 0U;
  gspi->info->status.mode_fault = 0U;

  gspi->xfer->rx_buf = (uint8_t *)data_in;
  ;
  gspi->xfer->tx_buf = (uint8_t *)data_out;

  gspi->xfer->num    = num;
  gspi->xfer->rx_cnt = 0U;
  gspi->xfer->tx_cnt = 0U;

  // Enable Full duplex mode
  gspi->reg->GSPI_CONFIG1_b.SPI_FULL_DUPLEX_EN = 0x1;

  data_bits = GSPI0->GSPI_WRITE_DATA2_b.GSPI_MANUAL_WRITE_DATA2;
  if ((gspi->rx_dma) || (gspi->tx_dma)) {
    if (num > 10) {
      // Configure the FIFO thresholds
      gspi->reg->GSPI_FIFO_THRLD_b.FIFO_AEMPTY_THRLD = (unsigned int)(gspi->threshold->gspi_aempty_threshold & 0x0F);
      gspi->reg->GSPI_FIFO_THRLD_b.FIFO_AFULL_THRLD  = (unsigned int)(gspi->threshold->gspi_afull_threshold & 0x0F);
      if (gspi->tx_dma) {
        control.transferType = UDMA_MODE_BASIC;
        control.nextBurst    = 0;
        if (num < 1024) {
          control.totalNumOfDMATrans = (unsigned int)((num - 1) & 0x0F);
        } else {
          control.totalNumOfDMATrans = 0x3FF;
        }
        control.rPower      = (unsigned int)(gspi->threshold->txdma_arb_size & 0x0F);
        control.srcProtCtrl = 0x0;
        control.dstProtCtrl = 0x0;
        if ((data_bits <= 8) && (data_bits != 0)) {
          //8-bit data frame
          control.srcSize = SRC_SIZE_8;
          control.srcInc  = SRC_INC_8;
          control.dstSize = DST_SIZE_8;
          control.dstInc  = DST_INC_NONE;
        } else {
          //16-bit data frame
          control.srcSize = SRC_SIZE_16;
          control.srcInc  = SRC_INC_16;
          control.dstSize = DST_SIZE_16;
          control.dstInc  = DST_INC_NONE;
        }
#ifdef SL_SI91X_GSPI_DMA
        sl_dma_xfer_t dma_transfer_tx = { 0 };
        uint32_t channel              = gspi->tx_dma->channel + 1;
        uint32_t channel_priority     = gspi->tx_dma->chnl_cfg.channelPrioHigh;
        sl_dma_callback_t gspi_tx_callback;
        //Initialize sl_dma callback structure
        gspi_tx_callback.transfer_complete_cb = gspi_transfer_complete_callback;
        gspi_tx_callback.error_cb             = gspi_error_callback;
        //Initialize sl_dma transfer structure
        dma_transfer_tx.src_addr       = (uint32_t *)((uint32_t)(gspi->xfer->tx_buf));
        dma_transfer_tx.dest_addr      = (uint32_t *)((uint32_t) & (gspi->reg->GSPI_WRITE_FIFO));
        dma_transfer_tx.src_inc        = control.srcInc;
        dma_transfer_tx.dst_inc        = control.dstInc;
        dma_transfer_tx.xfer_size      = control.dstSize;
        dma_transfer_tx.transfer_count = num;
        dma_transfer_tx.transfer_type  = SL_DMA_MEMORY_TO_PERIPHERAL;
        dma_transfer_tx.dma_mode       = control.transferType;
        dma_transfer_tx.signal         = gspi->tx_dma->chnl_cfg.periAck;

        //Allocate DMA channel for Tx
        status = sl_si91x_dma_allocate_channel(DMA_INSTANCE, &channel, channel_priority);
        if (status && (status != SL_STATUS_DMA_CHANNEL_ALLOCATED)) {
          return ARM_DRIVER_ERROR;
        }
        //Register transfer complete and error callback
        if (sl_si91x_dma_register_callbacks(DMA_INSTANCE, channel, &gspi_tx_callback)) {
          return ARM_DRIVER_ERROR;
        }
        //Configure the channel for DMA transfer
        if (sl_si91x_dma_transfer(DMA_INSTANCE, channel, &dma_transfer_tx)) {
          return ARM_DRIVER_ERROR;
        }
#else
        stat = UDMAx_ChannelConfigure(udma,
                                      gspi->tx_dma->channel,
                                      (uint32_t)(gspi->xfer->tx_buf),
                                      (uint32_t) & (gspi->reg->GSPI_WRITE_FIFO),
                                      num,
                                      control,
                                      &gspi->tx_dma->chnl_cfg,
                                      gspi->tx_dma->cb_event,
                                      chnl_info,
                                      udmaHandle);
        if (stat == -1) {
          return ARM_DRIVER_ERROR;
        }
#endif
        GSPI0->GSPI_WRITE_DATA2_b.GSPI_MANUAL_WRITE_DATA2 = (unsigned int)(data_bits & 0x0F);
        gspi->reg->GSPI_CONFIG1_b.GSPI_MANUAL_WR          = 0x1; //write enable
        GSPI0->GSPI_WRITE_DATA2_b.USE_PREV_LENGTH         = 0x1;
      }
      if (gspi->rx_dma) {
        control.transferType = UDMA_MODE_BASIC;
        control.nextBurst    = 0;
        if (num < 1024) {
          control.totalNumOfDMATrans = (unsigned int)((num - 1) & 0x03FF);
        } else {
          control.totalNumOfDMATrans = 0x3FF;
        }
        control.rPower      = (unsigned int)(gspi->threshold->rxdma_arb_size & 0x0F);
        control.srcProtCtrl = 0x0;
        control.dstProtCtrl = 0x0;
        if ((data_bits <= 8) && (data_bits != 0)) {
          //8-bit data frame
          control.srcSize = SRC_SIZE_8;
          control.srcInc  = SRC_INC_NONE;
          control.dstSize = DST_SIZE_8;
          control.dstInc  = DST_INC_8;
        } else {
          //16-bit data frame
          control.srcSize = SRC_SIZE_16;
          control.srcInc  = SRC_INC_NONE;
          control.dstSize = DST_SIZE_16;
          control.dstInc  = DST_INC_16;
        }
#ifdef SL_SI91X_GSPI_DMA
        sl_dma_xfer_t dma_transfer_rx = { 0 };
        uint32_t channel              = gspi->rx_dma->channel + 1;
        uint32_t channel_priority     = gspi->rx_dma->chnl_cfg.channelPrioHigh;
        sl_dma_callback_t gspi_rx_callback;
        //Initialize sl_dma callback structure
        gspi_rx_callback.transfer_complete_cb = gspi_transfer_complete_callback;
        gspi_rx_callback.error_cb             = gspi_error_callback;
        //Initialize sl_dma transfer structure
        dma_transfer_rx.src_addr       = (uint32_t *)((uint32_t) & (gspi->reg->GSPI_READ_FIFO));
        dma_transfer_rx.dest_addr      = (uint32_t *)((uint32_t)(gspi->xfer->rx_buf));
        dma_transfer_rx.src_inc        = control.srcInc;
        dma_transfer_rx.dst_inc        = control.dstInc;
        dma_transfer_rx.xfer_size      = control.dstSize;
        dma_transfer_rx.transfer_count = num;
        dma_transfer_rx.transfer_type  = SL_DMA_PERIPHERAL_TO_MEMORY;
        dma_transfer_rx.dma_mode       = control.transferType;
        dma_transfer_rx.signal         = gspi->rx_dma->chnl_cfg.periAck;

        //Allocate DMA channel for Rx
        status = sl_si91x_dma_allocate_channel(DMA_INSTANCE, &channel, channel_priority);
        if (status && (status != SL_STATUS_DMA_CHANNEL_ALLOCATED)) {
          return ARM_DRIVER_ERROR;
        }
        //Register transfer complete and error callback
        if (sl_si91x_dma_register_callbacks(DMA_INSTANCE, channel, &gspi_rx_callback)) {
          return ARM_DRIVER_ERROR;
        }
        //Configure the channel for DMA transfer
        if (sl_si91x_dma_transfer(DMA_INSTANCE, channel, &dma_transfer_rx)) {
          return ARM_DRIVER_ERROR;
        }
#else
        stat = UDMAx_ChannelConfigure(udma,
                                      gspi->rx_dma->channel,
                                      (uint32_t) & (gspi->reg->GSPI_READ_FIFO),
                                      (uint32_t)(gspi->xfer->rx_buf),
                                      num,
                                      control,
                                      &gspi->rx_dma->chnl_cfg,
                                      gspi->rx_dma->cb_event,
                                      chnl_info,
                                      udmaHandle);
        if (stat == -1) {
          return ARM_DRIVER_ERROR;
        }
#endif
        gspi->reg->GSPI_CONFIG1_b.GSPI_MANUAL_RD = 0x1;
#ifdef SL_SI91X_GSPI_DMA
        sl_si91x_dma_channel_enable(DMA_INSTANCE, gspi->rx_dma->channel + 1);
        sl_si91x_dma_channel_enable(DMA_INSTANCE, gspi->tx_dma->channel + 1);
        sl_si91x_dma_enable(DMA_INSTANCE);
#else
        UDMAx_ChannelEnable(gspi->rx_dma->channel, udma, udmaHandle);
        UDMAx_ChannelEnable(gspi->tx_dma->channel, udma, udmaHandle);
        UDMAx_DMAEnable(udma, udmaHandle);
#endif
      }

    } else {
      // IO mode
      // Configure the FIFO thresholds in I/0 mode
      gspi->reg->GSPI_FIFO_THRLD_b.FIFO_AEMPTY_THRLD        = 0x1;
      gspi->reg->GSPI_FIFO_THRLD_b.FIFO_AFULL_THRLD         = 0xC;
      gspi->reg->GSPI_WRITE_DATA2_b.GSPI_MANUAL_WRITE_DATA2 = (unsigned int)(data_bits & 0x0F);
      gspi->reg->GSPI_CONFIG1_b.GSPI_MANUAL_WR              = 0x1; //write enable
      if ((data_bits <= 8U) && (data_bits != 0)) {
        (*(volatile uint8_t *)(0x45030000 + 0x80)) = *(gspi->xfer->tx_buf++); //rite first index data
      } else {
        data_16bit = *(gspi->xfer->tx_buf++);
        data_16bit |= (uint16_t)(*(gspi->xfer->tx_buf++) << 8U);
        (*(volatile uint16_t *)(0x45030000 + 0x80)) = data_16bit; //write first index data
      }
      while (gspi->reg->GSPI_STATUS & GSPI_BUSY_F)
        ;
      gspi->reg->GSPI_INTR_UNMASK |= GSPI_INTR_UNMASK_BIT;
    }
  } else {
    gspi->reg->GSPI_FIFO_THRLD_b.FIFO_AEMPTY_THRLD = 0x1;
    gspi->reg->GSPI_FIFO_THRLD_b.FIFO_AFULL_THRLD  = 0xC;
    gspi->reg->GSPI_INTR_UNMASK |= GSPI_INTR_UNMASK_BIT;
    gspi->reg->GSPI_WRITE_DATA2_b.GSPI_MANUAL_WRITE_DATA2 = (unsigned int)(data_bits & 0x0F);
    gspi->reg->GSPI_CONFIG1_b.GSPI_MANUAL_WR              = 0x1; //write enable
    if ((data_bits <= 8U) && (data_bits != 0)) {
      (*(volatile uint8_t *)(0x45030000 + 0x80)) = *(gspi->xfer->tx_buf++);
      ; //rite first index data
    } else {
      data_16bit = *(gspi->xfer->tx_buf++);
      data_16bit |= (uint16_t)(*(gspi->xfer->tx_buf++) << 8U);
      (*(volatile uint16_t *)(0x45030000 + 0x80)) = data_16bit; //write first index data
    }
    while (gspi->reg->GSPI_STATUS & GSPI_BUSY_F)
      ;
  }
  return ARM_DRIVER_OK;
}

/*==============================================*/
/**
 * @fn          uint32_t GSPI_GetDataCount(const GSPI_RESOURCES *gspi)
 * @brief		     Get transferred data count.
 * @param[in]	   gspi        : Pointer to the GSPI resources
 * @return 		   number of data items transferred
 */
uint32_t GSPI_GetDataCount(const GSPI_RESOURCES *gspi)
{
  if (!(gspi->info->state & SPI_CONFIGURED)) {
    return 0U;
  }

  return gspi->xfer->rx_cnt;
}
/*==============================================*/
/**
 * @fn          void GSPI_UDMA_Tx_Event(uint32_t event, uint8_t dmaCh, GSPI_RESOURCES *gspi)
 * @brief		     GSPI UDMA transfer event Handler
 * @param[in]    event       : UDMA Event
 * @param[in]    dmaCh       : UDMA channel
 * @param[in]	   gspi        : Pointer to the GSPI resources
 * @return 		   none
 */
void GSPI_UDMA_Tx_Event(uint32_t event, uint8_t dmaCh, GSPI_RESOURCES *gspi)
{
  (void)dmaCh;
  switch (event) {
    case UDMA_EVENT_XFER_DONE:
      //gspi->info->status.busy       = 0U;
      // Update TX buffer info
      gspi->xfer->tx_cnt = gspi->xfer->num;
      if (gspi->xfer->rx_buf == NULL) {
        if (gspi->info->cb_event != NULL) {
          gspi->info->cb_event(ARM_SPI_EVENT_TRANSFER_COMPLETE);
        }
      }
      break;
    case UDMA_EVENT_ERROR:
      break;
  }
}

/*==============================================*/
/**
 * @fn          void GSPI_UDMA_Rx_Event(uint32_t event, uint8_t dmaCh, GSPI_RESOURCES *gspi)
 * @brief		     GSPI UDMA rx event Handler
 * @param[in]    event       : UDMA Event
 * @param[in]    dmaCh       : UDMA channel
 * @param[in]	   gspi        : Pointer to the GSPI resources
 * @return 		   none
 */
void GSPI_UDMA_Rx_Event(uint32_t event, uint8_t dmaCh, GSPI_RESOURCES *gspi)
{
  (void)dmaCh;
  switch (event) {
    case UDMA_EVENT_XFER_DONE:
      //gspi->xfer->rx_cnt    = gspi->xfer->num;
      gspi->info->status.busy = 0U;
      break;
    case UDMA_EVENT_ERROR:
      break;
  }
  if (gspi->info->cb_event != NULL) {
    gspi->info->cb_event(ARM_SPI_EVENT_TRANSFER_COMPLETE);
  }
}

/*==============================================*/
/**
 * @fn          void GSPI_IRQHandler(const GSPI_RESOURCES *gspi)
 * @brief		     GSPI IRQ Handler
 * @param[in]	   gspi        : Pointer to the GSPI resources
 * @return 		   none
 */
void GSPI_IRQHandler(const GSPI_RESOURCES *gspi)
{
  uint16_t data_bits   = 0;
  uint32_t event       = 0;
  uint32_t gspi_status = 0;
  uint8_t data_8bit;
  uint16_t data_16bit;

  volatile uint32_t dummy_data_read = 0;

  gspi_status = gspi->reg->GSPI_STATUS;

  data_bits = gspi->reg->GSPI_WRITE_DATA2_b.GSPI_MANUAL_WRITE_DATA2;

  // wait for GSPI idle
  while (gspi->reg->GSPI_STATUS_b.GSPI_BUSY)
    ;

  if (gspi->reg->GSPI_STATUS_b.GSPI_BUSY != 1) {
    gspi->reg->GSPI_WRITE_DATA2 = data_bits;
  }
  gspi->reg->GSPI_INTR_ACK_b.GSPI_INTR_ACK = 0x1;

  if ((gspi_status & GSPI_FIFO_EMPTY_RFIFO_S) == 0) {
    if (gspi->xfer->rx_buf) {
      if ((data_bits <= 8U) && (data_bits != 0)) {
        //8bit
        data_8bit               = (*(volatile uint8_t *)(0x45030000 + 0x80));
        *(gspi->xfer->rx_buf++) = data_8bit;
      } else {
        //16bit
        data_16bit              = (*(volatile uint16_t *)(0x45030000 + 0x80));
        *(gspi->xfer->rx_buf++) = (uint8_t)data_16bit;
        *(gspi->xfer->rx_buf++) = (uint8_t)(data_16bit >> 8U);
      }
      gspi->xfer->rx_cnt++;
      // Trigger the Read of GSPI Controller
      if (gspi->info->receive_function) {
        if (gspi->xfer->num != gspi->xfer->rx_cnt) {
          gspi->reg->GSPI_CONFIG1_b.GSPI_MANUAL_RD           = 0x1;
          gspi->reg->GSPI_CONFIG3_b.SPI_MANUAL_RD_LNTH_TO_BC = 1;
        }
      }
    } else {
      // dummy read
      dummy_data_read = (*(volatile uint32_t *)(0x45030000 + 0x80));
      gspi->xfer->rx_cnt++;
    }
    if (gspi->xfer->num == gspi->xfer->rx_cnt) {
      gspi->reg->GSPI_INTR_MASK |= (GSPI_INTR_MASK_BIT);
      // Clear busy flag
      gspi->info->status.busy = 0U;
    }
  } else {
    gspi->xfer->rx_cnt++;
  }
  if ((gspi_status & BIT(2)) == 0) {
    if (gspi->xfer->tx_cnt < gspi->xfer->num) {
      gspi->xfer->tx_cnt++;
      if (gspi->xfer->num != gspi->xfer->tx_cnt) {
        if (gspi->xfer->tx_buf) {
          gspi->reg->GSPI_CONFIG1_b.GSPI_MANUAL_WR = 0x1;
          if ((data_bits <= 8U) && (data_bits != 0)) {
            data_8bit                                  = *(gspi->xfer->tx_buf++);
            (*(volatile uint8_t *)(0x45030000 + 0x80)) = data_8bit;
          } else {
            data_16bit = *(gspi->xfer->tx_buf++);
            data_16bit |= (uint16_t)(*(gspi->xfer->tx_buf++) << 8);
            (*(volatile uint16_t *)(0x45030000 + 0x80)) = data_16bit;
            (void)dummy_data_read;
          }
        }
      }
    }
  }
  if (gspi->xfer->num == gspi->xfer->rx_cnt) {
    // Transfer completed
    event |= ARM_SPI_EVENT_TRANSFER_COMPLETE;
    gspi->info->cb_event(event);
  }
  while (gspi->reg->GSPI_STATUS_b.GSPI_BUSY)
    ;
}
#endif //A11_ROM
