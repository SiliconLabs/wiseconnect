/*******************************************************************************
 * @file  rsi_psram.c
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

#include "rsi_psram.h"
#include "rsi_psram_config.h"
#include "UDMA.h"
#include "rsi_pll.h"
#include "rsi_qspi.h"
#include "rsi_rom_egpio.h"
#include "rsi_rom_udma_wrapper.h"

extern uint16_t psram_security_aesKeySize;

extern uint32_t psram_security_aesKey[8];

extern PSRAMSecureSegmentType PSRAMSecureSegments[MAX_SEC_SEGMENTS];

static PSRAMReturnType autoModeState = PSRAM_UNKNOWN;

spi_config_t spi_psram_default_config = {

  .spi_config_1 = { .inst_mode         = SINGLE_MODE,
                    .addr_mode         = SINGLE_MODE,
                    .dummy_mode        = SINGLE_MODE,
                    .data_mode         = SINGLE_MODE,
                    .extra_byte_mode   = SINGLE_MODE,
                    .prefetch_en       = DIS_PREFETCH,
                    .dummy_W_or_R      = DUMMY_READS,
                    .d3d2_data         = 3,
                    .continuous        = DIS_CONTINUOUS,
                    .read_cmd          = 0x03,
                    .flash_type        = 255, // APS_QUAD_PSRAM
                    .no_of_dummy_bytes = 0,
                    .extra_byte_en     = 0 },

  .spi_config_2 = { .auto_mode                   = EN_MANUAL_MODE,
                    .cs_no                       = PSRAM_CHIP_SELECT,
                    .neg_edge_sampling           = NEG_EDGE_SAMPLING,
                    .qspi_clk_en                 = QSPI_FULL_TIME_CLK,
                    .protection                  = DNT_REM_WR_PROT,
                    .dma_mode                    = NO_DMA,
                    .swap_en                     = NO_SWAP,
                    .full_duplex                 = IGNORE_FULL_DUPLEX,
                    .wrap_len_in_bytes           = NO_WRAP,
                    .addr_width_valid            = 0,
                    .addr_width                  = _24BIT_ADDR,
                    .pinset_valid                = 0,
                    .dummy_cycles_for_controller = 0 },

  .spi_config_3 = { .xip_mode          = 0,
                    ._16bit_cmd_valid  = 0,
                    ._16bit_rd_cmd_msb = 0,
                    .reserved          = 0,
                    .wr_cmd            = 0x2,
                    .wr_inst_mode      = SINGLE_MODE,
                    .wr_addr_mode      = SINGLE_MODE,
                    .wr_data_mode      = SINGLE_MODE,
                    .dummys_4_jump     = 1 },

  .spi_config_4 = { ._16bit_wr_cmd_msb    = 0,
                    .dual_flash_mode      = 0,
                    .secondary_csn        = 1,
                    .polarity_mode        = 0,
                    .valid_prot_bits      = 4,
                    .no_of_ms_dummy_bytes = 0,
                    .continue_fetch_en    = 0 },

};

static PSRAMStatusType PSRAMStatus = { .state               = unknown,
                                       .ID                  = MAX_SUPPORTED_PSRAM_DEVICES,
                                       .spiMode             = SINGLE_MODE,
                                       .burstSize           = 0,
                                       .secureModeEnable    = false,
                                       .secureSegmentNumber = 0,
                                       .spi_config          = NULL };

PSRAMPinConfigType PSRAMPinConfig[NUM_OF_PSRAM_PINS] = {
  { M4SS_PSRAM_CLK_PORT, M4SS_PSRAM_CLK_PIN, M4SS_PSRAM_CLK_MUX, M4SS_PSRAM_CLK_PAD },
  { M4SS_PSRAM_CSN_PORT, M4SS_PSRAM_CSN_PIN, M4SS_PSRAM_CSN_MUX, M4SS_PSRAM_CSN_PAD

  },
  { M4SS_PSRAM_D0_PORT, M4SS_PSRAM_D0_PIN, M4SS_PSRAM_D0_MUX, M4SS_PSRAM_D0_PAD

  },
  { M4SS_PSRAM_D1_PORT, M4SS_PSRAM_D1_PIN, M4SS_PSRAM_D1_MUX, M4SS_PSRAM_D1_PAD

  },
  { M4SS_PSRAM_D2_PORT, M4SS_PSRAM_D2_PIN, M4SS_PSRAM_D2_MUX, M4SS_PSRAM_D2_PAD

  },
  { M4SS_PSRAM_D3_PORT, M4SS_PSRAM_D3_PIN, M4SS_PSRAM_D3_MUX, M4SS_PSRAM_D3_PAD

  }

#if defined(PSRAM_GPIO_PIN_SET_46_TO_57_CS_0) || defined(PSRAM_GPIO_PIN_SET_46_TO_57_CS_1)
  ,
  { M4SS_PSRAM_D4_PORT, M4SS_PSRAM_D4_PIN, M4SS_PSRAM_D4_MUX, M4SS_PSRAM_D4_PAD

  },
  { M4SS_PSRAM_D5_PORT, M4SS_PSRAM_D5_PIN, M4SS_PSRAM_D5_MUX, M4SS_PSRAM_D5_PAD

  },
  { M4SS_PSRAM_D6_PORT, M4SS_PSRAM_D6_PIN, M4SS_PSRAM_D6_MUX, M4SS_PSRAM_D6_PAD

  },
  { M4SS_PSRAM_D7_PORT, M4SS_PSRAM_D7_PIN, M4SS_PSRAM_D7_MUX, M4SS_PSRAM_D7_PAD

  }

#endif

};

/* DMA descriptors must be aligned to 16 bytes */
#if defined(__CC_ARM)
extern RSI_UDMA_DESC_T UDMA0_Table[CONTROL_STRUCT0];
extern RSI_UDMA_DESC_T UDMA1_Table[CONTROL_STRUCT1];
#endif /* defined (__CC_ARM) */

#if defined(__GNUC__)
extern RSI_UDMA_DESC_T __attribute__((section(".udma_addr0"))) UDMA0_Table[CONTROL_STRUCT0];
extern RSI_UDMA_DESC_T __attribute__((section(".udma_addr1"))) UDMA1_Table[CONTROL_STRUCT1];
#endif /* defined (__GNUC__) */

extern UDMA_Channel_Info udma0_chnl_info[32];

/* UDMA0 Resources */
extern UDMA_RESOURCES UDMA0_Resources;

/* UDMA1 Resources */
extern UDMA_RESOURCES UDMA1_Resources;

extern RSI_UDMA_HANDLE_T udmaHandle0;

extern uint32_t dma_rom_buff0[30];

PSRAMReturnType PRSAM_ToggleBurstLength(void);

void QSPI_Transmit(qspi_reg_t *qspi_reg,
                   uint8_t hSize,
                   void *data,
                   uint32_t length,
                   uint8_t chipSelect,
                   uint8_t chipSelectContinue);

void QSPI_Receive(qspi_reg_t *qspi_reg,
                  uint8_t hSize,
                  void *data,
                  uint32_t length,
                  uint8_t chipSelect,
                  uint8_t chipSelectContinue);

void QSPI_Deassert_CSN(qspi_reg_t *qspi_reg);

void QSPI_SRAM_Auto_Init(qspi_reg_t *qspi_reg, spi_config_t *spi_config);

void QSPI_SRAM_WRAP_Init(qspi_reg_t *qspi_reg, spi_config_t *spi_config);

void QSPI_Set_Swap_Mode(qspi_reg_t *qspi_reg, spi_config_t *spi_config);

void qspi_qspiload_key_ext(qspi_reg_t *qspi_reg, uint16_t key_size);

void qspi_qspiunload_key_ext(qspi_reg_t *qspi_reg);

#define QSPI_AUTO_WR_BUSY 0x8000

static volatile xferContextType ctx;
volatile uint32_t ret;
volatile PSRAMDMAStatusType dma_done;
RSI_UDMA_CHA_CONFIG_DATA_T control;
RSI_UDMA_CHA_CFG_T config;

void wait_state_manual()
{
  qspi_reg_t *qspi_reg = (qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS;
  // wait till QSPI becomes idle
  while (qspi_reg->QSPI_STATUS_REG & 1)
    ;
}

void wait_state()
{
  qspi_reg_t *qspi_reg = (qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS;
  while (qspi_reg->QSPI_STATUS_REG & QSPI_AUTO_WR_BUSY)
    ;
}

/**
 * @brief      UDMA  controller transfer descriptor chain complete callback
 * @param[in]  event dma transfer events
 * @param[in]  ch    dma channel number
 * @return     None
 */
void udmaTransferComplete(uint32_t event, uint8_t ch)
{
  if (event == UDMA_EVENT_XFER_DONE) {
    if (ch == 5 || ch == 4) {

      wait_state_manual();
      QSPI_Deassert_CSN((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS);

      RSI_UDMA_DeInit(udmaHandle0, &config);
      if ((ctx.xferRemLegth > 0) && (ctx.xferStatus == TX_RUNNING)) {
        if (PSRAM_ManualWriteInDMAMode(ctx.xferNextAddress,
                                       ctx.xferNextSourceAddress,
                                       ctx.xferHsize,
                                       ctx.xferRemLegth,
                                       ctx.done)
            != PSRAM_SUCCESS) {

#if (PSRAM_MODEL_WRAP == 1)
          PRSAM_SetWrapSize(PSRAM_Devices[PSRAMStatus.ID].toggleBurstWrapSize);
          PSRAMStatus.burstSize = PSRAM_Devices[PSRAMStatus.ID].toggleBurstWrapSize;
#endif
          if (PSRAM_MANUAL_MODE != autoModeState) {
            PSRAM_Enable_AutoReadWrite();
          }
          ctx.xferStatus = FAILED;
          /*Notify the Failure*/
          *ctx.done = DMA_FAIL;
          dma_done  = DMA_FAIL;
        }
      } else if ((ctx.xferRemLegth > 0) && (ctx.xferStatus == RX_RUNNING)) {
        if (PSRAM_ManualReadInDMAMode(ctx.xferNextAddress,
                                      ctx.xferNextSourceAddress,
                                      ctx.xferHsize,
                                      ctx.xferRemLegth,
                                      ctx.done)
            != PSRAM_SUCCESS) {
#if (PSRAM_MODEL_WRAP == 1)
          PRSAM_SetWrapSize(PSRAM_Devices[PSRAMStatus.ID].toggleBurstWrapSize);
          PSRAMStatus.burstSize = PSRAM_Devices[PSRAMStatus.ID].toggleBurstWrapSize;
#endif
          if (PSRAM_MANUAL_MODE != autoModeState) {
            PSRAM_Enable_AutoReadWrite();
          }

          ctx.xferStatus = FAILED;
          /*Notify the Failure*/
          *ctx.done = DMA_FAIL;
          dma_done  = DMA_FAIL;
        }
      } else {

        /*Clear the xfer Context*/
        ctx.xferStatus            = IDLE;
        ctx.xferHsize             = 0;
        ctx.xferNextAddress       = 0;
        ctx.xferNextSourceAddress = 0;
        ctx.xferRemLegth          = 0;

#if (PSRAM_MODEL_WRAP == 1)
        PRSAM_SetWrapSize(PSRAM_Devices[PSRAMStatus.ID].toggleBurstWrapSize);
        PSRAMStatus.burstSize = PSRAM_Devices[PSRAMStatus.ID].toggleBurstWrapSize;
#endif
        if (PSRAM_MANUAL_MODE != autoModeState) {
          PSRAM_Enable_AutoReadWrite();
        }

        /*Notify the completion*/
        *ctx.done = DMA_DONE;
        dma_done  = DMA_DONE;
      }
    }
  }
}

PSRAMReturnType PSRAM_Init(PSRAMDevType device)
{
  PSRAMReturnType PSRAM_Status = PSRAM_UNKNOWN;
  error_t clkStatus            = RSI_FAIL;
  uint8_t pinIndex             = 0;
#ifdef D_CACHE_ENABLE
  uint32_t dCacheInitStatus = 0;
#endif

  if (device >= MAX_SUPPORTED_PSRAM_DEVICES) {
    PSRAMStatus.state = uninitialised;
    return PSRAM_UNKNOWN_DEVICE;
  }

  /*Clock Initialization*/

  /*TODO: Review comments: Clock configuration to provide, calculate the Div
   * values, Check for proper clock*/
  clkStatus = RSI_CLK_Qspi2ClkConfig(M4CLK, QSPI_INTFPLLCLK, 0, 0, 0);

  if (RSI_OK != clkStatus) {
    return PSRAM_CLOCK_INIT_FAILURE;
  }

  /*PinMux Initialization*/
  for (pinIndex = 0; pinIndex < NUM_OF_PSRAM_PINS; pinIndex++) {
    RSI_EGPIO_PadSelectionEnable(PSRAMPinConfig[pinIndex].pad);

    RSI_EGPIO_PadReceiverEnable((PSRAMPinConfig[pinIndex].port * 16) + PSRAMPinConfig[pinIndex].pin);

    /*Set the GPIO pin MUX */
    RSI_EGPIO_SetPinMux(EGPIO,
                        PSRAMPinConfig[pinIndex].port,
                        PSRAMPinConfig[pinIndex].pin,
                        PSRAMPinConfig[pinIndex].mux);
  }

  /*QSPI Initialization*/
  PSRAM_Status = PRSAM_DetectDevice(device);

  if (PSRAM_SUPPORTED_DEVICE == PSRAM_Status) {

    PSRAMStatus.spi_config = (spi_config_t *)&PSRAM_Devices[device].spi_config;

    if (PSRAM_SPI_MODE == QUAD_MODE) {
      /*Set the PSRAM device to QPI mode*/
      PRSAM_Enter_QPIMode();

      PSRAMStatus.spiMode = QUAD_MODE;
    }

    /*Initialize the QSPI controller as per PSRAM device configuration*/
    qspi_spi_init((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS, PSRAMStatus.spi_config, 0, 0, 0);

#if (PSRAM_MODEL_WRAP == 1)
    QSPI_SRAM_WRAP_Init((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS, PSRAMStatus.spi_config);
#endif

    QSPI_SRAM_Auto_Init((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS, PSRAMStatus.spi_config);

    QSPI_Set_Swap_Mode((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS, PSRAMStatus.spi_config);

    PSRAMStatus.state = initialised;

    PSRAMStatus.ID = device;

    PSRAMStatus.secureSegmentNumber = 0;

    PSRAMStatus.secureModeEnable = false;

#if (PSRAM_MODEL_WRAP == 1)
    PRSAM_SetWrapSize(_WRAP32);

    PSRAMStatus.burstSize = _WRAP32;
#endif

    /*Clear the xfer Context*/
    ctx.xferStatus            = IDLE;
    ctx.xferHsize             = 0;
    ctx.xferNextAddress       = 0;
    ctx.xferNextSourceAddress = 0;
    ctx.xferRemLegth          = 0;

    /*Return success*/
    PSRAM_Status = PSRAM_SUCCESS;

  } else {

    /*UnInitialize QSPI Controller*/

    /*UnInitialize PinMux*/
    for (pinIndex = 0; pinIndex < NUM_OF_PSRAM_PINS; pinIndex++) {

      /*Set the GPIO pin MUX */
      RSI_EGPIO_SetPinMux(EGPIO, PSRAMPinConfig[pinIndex].port, PSRAMPinConfig[pinIndex].pin, EGPIO_PIN_MUX_MODE0);

      /* Set output direction */
      RSI_EGPIO_SetDir(EGPIO, PSRAMPinConfig[pinIndex].port, PSRAMPinConfig[pinIndex].pin, EGPIO_CONFIG_DIR_INPUT);
    }

    /*UnInitialize clock*/
    clk_peripheral_clk_disable(M4CLK, QSPI_2_CLK);

    PSRAMStatus.state = uninitialised;

    PSRAMStatus.ID = MAX_SUPPORTED_PSRAM_DEVICES;

    PSRAMStatus.burstSize = 0;

    PSRAMStatus.secureSegmentNumber = 0;

    PSRAMStatus.secureModeEnable = false;

    /*Return success*/
    PSRAM_Status = PSRAM_FAILURE;
  }

  /*D-cache Initialization*/
#ifdef D_CACHE_ENABLE
  DCACHE_REG_CTRL |= 0x3;
  dCacheInitStatus = DCACHE_REG_MAINT_STATUS;
  while (dCacheInitStatus != 0x101) {
    dCacheInitStatus = DCACHE_REG_MAINT_STATUS;
  }
#endif

  return PSRAM_Status;
}

PSRAMReturnType PSRAM_UnInit(void)
{
  uint8_t pinIndex = 0;
#ifdef D_CACHE_ENABLE
  uint32_t dCacheDeInitStatus = 0;
#endif

  /*TODO: First Check init*/
  if (PSRAM_SPI_MODE == QUAD_MODE) {
    /*Set the PSRAM device to QPI mode*/
    PRSAM_Exit_QPIMode();

    PSRAMStatus.spiMode = SINGLE_MODE;
  }

  /*Initialize the QSPI controller to PSRAM default mode configuration*/
  qspi_spi_init((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS, (spi_config_t *)&spi_psram_default_config, 0, 0, 0);

  /*disable cache*/
#ifdef D_CACHE_ENABLE
  DCACHE_REG_CTRL &= 0xFFFFFFFE;
  while ((DCACHE_REG_MAINT_STATUS & 0x3) != 0x0)
    ;
#endif

  /*UnInitialize PinMux*/
  for (pinIndex = 0; pinIndex < NUM_OF_PSRAM_PINS; pinIndex++) {

    /*Set the GPIO pin MUX */
    RSI_EGPIO_SetPinMux(EGPIO, PSRAMPinConfig[pinIndex].port, PSRAMPinConfig[pinIndex].pin, EGPIO_PIN_MUX_MODE0);

    /* Set output direction */
    RSI_EGPIO_SetDir(EGPIO, PSRAMPinConfig[pinIndex].port, PSRAMPinConfig[pinIndex].pin, EGPIO_CONFIG_DIR_INPUT);
  }

  clk_peripheral_clk_disable(M4CLK, QSPI_2_CLK);

  PSRAMStatus.state = uninitialised;

  PSRAMStatus.ID = MAX_SUPPORTED_PSRAM_DEVICES;

  PSRAMStatus.burstSize = 0;

  PSRAMStatus.secureSegmentNumber = 0;

  PSRAMStatus.secureModeEnable = false;

  /*Return success*/
  return PSRAM_SUCCESS;
}

PSRAMReturnType PRSAM_DetectDevice(const PSRAMDevType device)
{
  PSRMIDType PSRAM_ID;
  uint8_t command[4];
  static volatile uint8_t dummy_read;

  spi_config_t *spi_config = &spi_psram_default_config;

  if (device >= MAX_SUPPORTED_PSRAM_DEVICES) {
    return PSRAM_UNKNOWN_DEVICE;
  }

  /*Initialize the QSPI controller to PSRAM default mode configuration*/
  qspi_spi_init((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS, (spi_config_t *)&spi_psram_default_config, 0, 0, 0);

  //TODO: To check the start up time after boot exit, should be >150us or else implement wait here

  /*Perform dummy read before Read ID*/

  /*Dummy read in case if read id is not called after power on*/
  dummy_read = (*(uint8_t *)PSRAM_BASE_ADDRESS);

  /*Read ID*/
  command[0] = PSRAM_READ_ID;
  command[1] = 0x00;
  command[2] = 0x00;
  command[3] = 0x00;

  QSPI_Transmit((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                sizeof(uint8_t),
                &command[0],
                sizeof(command),
                spi_config->spi_config_2.cs_no,
                1); // Do not de-assert the CS

  QSPI_Receive((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
               sizeof(uint8_t),
               ((uint8_t *)&PSRAM_ID),
               sizeof(PSRAM_ID),
               spi_config->spi_config_2.cs_no,
               0); // de-assert the CS

  /* Find the device and get the device specific configuration*/

  if (PSRAM_Devices[device].deviceID.MFID == PSRAM_ID.MFID && PSRAM_Devices[device].deviceID.KGD == PSRAM_ID.KGD) {
    return PSRAM_SUPPORTED_DEVICE;
  } else {

    return PSRAM_DEVICE_MISMATCH;
  }
}

PSRAMReturnType PSRAM_ManualWriteInBlockingMode(uint32_t addr, void *SourceBuf, uint8_t hSize, uint32_t length)
{
  uint32_t prev_bus_mode;
  uint16_t writeLength;
  uint8_t psramXferBuf[64];
  uint32_t xferAddr;
  static uint32_t rbxOffset;
  uint32_t lengthInBytes = 0;

  if (PSRAMStatus.state != initialised) {
    return PSRAM_NOT_INITIALIZED;
  }

  if (((hSize != sizeof(uint8_t)) && (hSize != sizeof(uint16_t)) && (hSize != sizeof(uint32_t)))) {
    return PSRAM_INVALID_HSIZE;
  }

  if (NULL == SourceBuf) {
    return PSRAM_NULL_ADDRESS;
  }

  if ((!(addr >= PSRAM_BASE_ADDRESS && addr < (PSRAM_BASE_ADDRESS + (PSRAM_Devices[PSRAMStatus.ID].devDensity / 8))))
      || ((addr + (length * hSize)) > (PSRAM_BASE_ADDRESS + (PSRAM_Devices[PSRAMStatus.ID].devDensity / 8)))) {
    return PSRAM_INVALID_ADDRESS_LENGTH;
  }
  if (PSRAM_Devices[PSRAMStatus.ID].defaultBurstWrapSize) {
    rbxOffset = addr % PSRAM_Devices[PSRAMStatus.ID].defaultBurstWrapSize;
  }

  autoModeState = PSRAM_Disable_AutoReadWrite();

#if (PSRAM_MODEL_WRAP == 1)
  PRSAM_SetWrapSize(PSRAM_Devices[PSRAMStatus.ID].defaultBurstWrapSize);
  PSRAMStatus.burstSize = PSRAM_Devices[PSRAMStatus.ID].defaultBurstWrapSize;
#endif

  while (length) {
    lengthInBytes = (length * hSize);

    if (lengthInBytes >= PSRAM_READ_WRITE_PAGE_SIZE) {
      writeLength = PSRAM_READ_WRITE_PAGE_SIZE;
    } else {
      writeLength = lengthInBytes;
    }

    if (0 != rbxOffset) {
      rbxOffset += writeLength;
      if (rbxOffset > PSRAM_Devices[PSRAMStatus.ID].defaultBurstWrapSize) {
        writeLength = writeLength - (rbxOffset - PSRAM_Devices[PSRAMStatus.ID].defaultBurstWrapSize);
        rbxOffset   = 0;
      }
    }

    if (PSRAMStatus.spi_config->spi_config_1.inst_mode != PSRAMStatus.spi_config->spi_config_1.addr_mode) {

      if (PSRAMStatus.spi_config->spi_config_2.swap_en == SWAP) {
        xferAddr = ((addr >> 16) & 0xff);
        xferAddr |= (addr & 0xff00);
        xferAddr |= ((addr & 0xff) << 16);
        addr = xferAddr;
      }

      // switch QSPI to command mode
      qspi_switch_qspi2((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                        PSRAMStatus.spi_config->spi_config_1.inst_mode,
                        PSRAMStatus.spi_config->spi_config_2.cs_no);

      prev_bus_mode = PSRAMStatus.spi_config->spi_config_1.inst_mode;

      /*Send the write command to PSRAM*/
      if (PSRAMStatus.spi_config->spi_config_3._16bit_cmd_valid) {
        qspi_write_to_flash(
          (qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
          QSPI_16BIT_LEN,
          ((PSRAMStatus.spi_config->spi_config_3.wr_cmd << 8) | PSRAMStatus.spi_config->spi_config_3._16bit_rd_cmd_msb),
          PSRAMStatus.spi_config->spi_config_2.cs_no);
      } else {
        qspi_write_to_flash(
          (qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
          QSPI_8BIT_LEN,
          (PSRAMStatus.spi_config->spi_config_3.wr_cmd
           | ((PSRAMStatus.spi_config->spi_config_2.addr_width == _9BIT_ADDR) ? ((addr & BIT(8)) << 3) : 0)),
          PSRAMStatus.spi_config->spi_config_2.cs_no);
      }

      // if addr mode is not same as prev bus mode, switch qspi to addr mode
      if (PSRAMStatus.spi_config->spi_config_1.addr_mode != prev_bus_mode) {
        qspi_switch_qspi2((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                          PSRAMStatus.spi_config->spi_config_1.addr_mode,
                          PSRAMStatus.spi_config->spi_config_2.cs_no);
        prev_bus_mode = PSRAMStatus.spi_config->spi_config_1.addr_mode;
      }

      /* Send write Address to PSRAM  */
      qspi_write_to_flash(
        (qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
        (PSRAMStatus.spi_config->spi_config_2.addr_width ? (PSRAMStatus.spi_config->spi_config_2.addr_width * 8) : 8),
        addr,
        PSRAMStatus.spi_config->spi_config_2.cs_no);

      // if extra byte is enabled send that to PSRAM
      if (PSRAMStatus.spi_config->spi_config_1.extra_byte_en) {

        if (PSRAMStatus.spi_config->spi_config_1.extra_byte_mode != prev_bus_mode) {
          qspi_switch_qspi2((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                            PSRAMStatus.spi_config->spi_config_1.extra_byte_mode,
                            PSRAMStatus.spi_config->spi_config_2.cs_no);
          prev_bus_mode = PSRAMStatus.spi_config->spi_config_1.extra_byte_mode;
        }

        qspi_write_to_flash((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                            QSPI_8BIT_LEN,
                            0x00,
                            PSRAMStatus.spi_config->spi_config_2.cs_no);
      }

      if (PSRAMStatus.spi_config->spi_config_1.data_mode != prev_bus_mode) {
        qspi_switch_qspi2((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                          PSRAMStatus.spi_config->spi_config_1.data_mode,
                          PSRAMStatus.spi_config->spi_config_2.cs_no);
      }
    } else {
      addr            = addr & 0xffffff;
      psramXferBuf[0] = PSRAMStatus.spi_config->spi_config_3.wr_cmd;
      psramXferBuf[1] = (addr >> 16) & 0xff;
      psramXferBuf[2] = (addr >> 8) & 0xff;
      psramXferBuf[3] = addr & 0xff;

      QSPI_Transmit((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                    sizeof(uint8_t),
                    psramXferBuf,
                    4,
                    PSRAMStatus.spi_config->spi_config_2.cs_no,
                    1); // do not deassert the chip
    }

    QSPI_Transmit((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                  hSize,
                  SourceBuf,
                  (writeLength / hSize),
                  PSRAMStatus.spi_config->spi_config_2.cs_no,
                  0); // deassert the chip select at end of data.

    length = length - (writeLength / hSize);

    if (length) {
      ((uint8_t *)SourceBuf) = (((uint8_t *)SourceBuf) + writeLength);
      addr                   = addr + writeLength;
    }
  }

#if (PSRAM_MODEL_WRAP == 1)
  PRSAM_SetWrapSize(PSRAM_Devices[PSRAMStatus.ID].toggleBurstWrapSize);
  PSRAMStatus.burstSize = PSRAM_Devices[PSRAMStatus.ID].toggleBurstWrapSize;
#endif
  if (PSRAM_MANUAL_MODE != autoModeState) {
    PSRAM_Enable_AutoReadWrite();
  }

  return PSRAM_SUCCESS;
}

PSRAMReturnType PSRAM_ManualReadInBlockingMode(uint32_t addr, void *DestBuf, uint8_t hSize, uint32_t length)
{
  uint32_t prev_bus_mode;
  uint32_t dummy_cnt, tmp_dummy;
  uint16_t readLength;
  uint8_t psramXferBuf[64];
  static uint32_t xferAddr;
  static uint32_t rbxOffset;
  uint32_t lengthInBytes = 0;

  if (PSRAMStatus.state != initialised) {
    return PSRAM_NOT_INITIALIZED;
  }

  if (((hSize != sizeof(uint8_t)) && (hSize != sizeof(uint16_t)) && (hSize != sizeof(uint32_t)))) {
    return PSRAM_INVALID_HSIZE;
  }

  if (NULL == DestBuf) {
    return PSRAM_NULL_ADDRESS;
  }

  if ((!(addr >= PSRAM_BASE_ADDRESS && addr < (PSRAM_BASE_ADDRESS + (PSRAM_Devices[PSRAMStatus.ID].devDensity / 8))))
      || ((addr + (length * hSize)) > (PSRAM_BASE_ADDRESS + (PSRAM_Devices[PSRAMStatus.ID].devDensity / 8)))) {
    return PSRAM_INVALID_ADDRESS_LENGTH;
  }

  if (PSRAM_Devices[PSRAMStatus.ID].defaultBurstWrapSize) {
    rbxOffset = addr % PSRAM_Devices[PSRAMStatus.ID].defaultBurstWrapSize;
  }

  autoModeState = PSRAM_Disable_AutoReadWrite();

#if (PSRAM_MODEL_WRAP == 1)
  PRSAM_SetWrapSize(PSRAM_Devices[PSRAMStatus.ID].defaultBurstWrapSize);
  PSRAMStatus.burstSize = PSRAM_Devices[PSRAMStatus.ID].defaultBurstWrapSize;
#endif

  while (length) {

    lengthInBytes = (length * hSize);

    if (lengthInBytes >= PSRAM_READ_WRITE_PAGE_SIZE) {
      readLength = PSRAM_READ_WRITE_PAGE_SIZE;
    } else {
      readLength = lengthInBytes;
    }

    if (0 != rbxOffset) {
      rbxOffset += readLength;
      if (rbxOffset > PSRAM_Devices[PSRAMStatus.ID].defaultBurstWrapSize) {
        readLength = readLength - (rbxOffset - PSRAM_Devices[PSRAMStatus.ID].defaultBurstWrapSize);
        rbxOffset  = 0;
      }
    }

    if (PSRAMStatus.spi_config->spi_config_1.inst_mode != PSRAMStatus.spi_config->spi_config_1.addr_mode) {

      if (PSRAMStatus.spi_config->spi_config_2.swap_en == SWAP) {
        xferAddr = ((addr >> 16) & 0xff);
        xferAddr |= (addr & 0xff00);
        xferAddr |= ((addr & 0xff) << 16);
        addr = xferAddr;
      }
      // switch QSPI to command mode
      qspi_switch_qspi2((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                        PSRAMStatus.spi_config->spi_config_1.inst_mode,
                        PSRAMStatus.spi_config->spi_config_2.cs_no);

      prev_bus_mode = PSRAMStatus.spi_config->spi_config_1.inst_mode;

      /*Send the Read command to PSRAM*/
      if (PSRAMStatus.spi_config->spi_config_3._16bit_cmd_valid) {
        qspi_write_to_flash((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                            QSPI_16BIT_LEN,
                            ((PSRAMStatus.spi_config->spi_config_1.read_cmd << 8)
                             | PSRAMStatus.spi_config->spi_config_3._16bit_rd_cmd_msb),
                            PSRAMStatus.spi_config->spi_config_2.cs_no);
      } else {
        qspi_write_to_flash(
          (qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
          QSPI_8BIT_LEN,
          (PSRAMStatus.spi_config->spi_config_1.read_cmd
           | ((PSRAMStatus.spi_config->spi_config_2.addr_width == _9BIT_ADDR) ? ((addr & BIT(8)) << 3) : 0)),
          PSRAMStatus.spi_config->spi_config_2.cs_no);
      }

      // if addr mode is not same as prev bus mode, switch qspi to addr mode
      if (PSRAMStatus.spi_config->spi_config_1.addr_mode != prev_bus_mode) {
        qspi_switch_qspi2((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                          PSRAMStatus.spi_config->spi_config_1.addr_mode,
                          PSRAMStatus.spi_config->spi_config_2.cs_no);
        prev_bus_mode = PSRAMStatus.spi_config->spi_config_1.addr_mode;
      }

      /* Send Read Address to PSRAM  */
      qspi_write_to_flash(
        (qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
        (PSRAMStatus.spi_config->spi_config_2.addr_width ? (PSRAMStatus.spi_config->spi_config_2.addr_width * 8) : 8),
        addr,
        PSRAMStatus.spi_config->spi_config_2.cs_no);

      // if extra byte is enabled send that to PSRAM
      if (PSRAMStatus.spi_config->spi_config_1.extra_byte_en) {

        if (PSRAMStatus.spi_config->spi_config_1.extra_byte_mode != prev_bus_mode) {
          qspi_switch_qspi2((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                            PSRAMStatus.spi_config->spi_config_1.extra_byte_mode,
                            PSRAMStatus.spi_config->spi_config_2.cs_no);
          prev_bus_mode = PSRAMStatus.spi_config->spi_config_1.extra_byte_mode;
        }

        qspi_write_to_flash((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                            QSPI_8BIT_LEN,
                            0x00,
                            PSRAMStatus.spi_config->spi_config_2.cs_no);
      }

      // if dummy bytes are configured, send them to PSRAM
      if ((PSRAMStatus.spi_config->spi_config_1.no_of_dummy_bytes)
          || (PSRAMStatus.spi_config->spi_config_4.no_of_ms_dummy_bytes)) {
        dummy_cnt = PSRAMStatus.spi_config->spi_config_1.no_of_dummy_bytes;
        dummy_cnt |= (PSRAMStatus.spi_config->spi_config_4.no_of_ms_dummy_bytes << 4);

        if (PSRAMStatus.spi_config->spi_config_1.dummy_mode != prev_bus_mode) {
          qspi_switch_qspi2((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                            PSRAMStatus.spi_config->spi_config_1.dummy_mode,
                            PSRAMStatus.spi_config->spi_config_2.cs_no);
          prev_bus_mode = PSRAMStatus.spi_config->spi_config_1.dummy_mode;
        }

        // dummy write

        do {

          tmp_dummy = (dummy_cnt & 0x3) ? (dummy_cnt & 3) : 4;

          qspi_write_to_flash((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                              (QSPI_8BIT_LEN * tmp_dummy),
                              0x00,
                              PSRAMStatus.spi_config->spi_config_2.cs_no);
          dummy_cnt -= tmp_dummy;

        } while (dummy_cnt);
      }

      if (PSRAMStatus.spi_config->spi_config_1.data_mode != prev_bus_mode) {
        qspi_switch_qspi2((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                          PSRAMStatus.spi_config->spi_config_1.data_mode,
                          PSRAMStatus.spi_config->spi_config_2.cs_no);
      }
    } else {
      addr            = addr & 0xffffff;
      psramXferBuf[0] = PSRAMStatus.spi_config->spi_config_1.read_cmd;
      psramXferBuf[1] = (addr >> 16) & 0xff;
      psramXferBuf[2] = (addr >> 8) & 0xff;
      psramXferBuf[3] = addr & 0xff;
      psramXferBuf[4] = 0x00;
      psramXferBuf[5] = 0x00;
      psramXferBuf[6] = 0x00;

      QSPI_Transmit((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                    sizeof(uint8_t),
                    psramXferBuf,
                    7,
                    PSRAMStatus.spi_config->spi_config_2.cs_no,
                    1); // do not deassert the chip
    }

    QSPI_Receive((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                 hSize,
                 DestBuf,
                 (readLength / hSize),
                 PSRAMStatus.spi_config->spi_config_2.cs_no,
                 0);

    length = length - (readLength / hSize);

    if (length) {
      ((uint8_t *)DestBuf) = (((uint8_t *)DestBuf) + readLength);
      addr                 = addr + readLength;
    }
  }

#if (PSRAM_MODEL_WRAP == 1)
  PRSAM_SetWrapSize(PSRAM_Devices[PSRAMStatus.ID].toggleBurstWrapSize);
  PSRAMStatus.burstSize = PSRAM_Devices[PSRAMStatus.ID].toggleBurstWrapSize;
#endif

  if (PSRAM_MANUAL_MODE != autoModeState) {
    PSRAM_Enable_AutoReadWrite();
  }

  return PSRAM_SUCCESS;
}

PSRAMReturnType PSRAM_ManualWriteInDMAMode(uint32_t addr,
                                           void *SourceBuf,
                                           uint8_t hSize,
                                           uint32_t length,
                                           PSRAMDMAStatusType *dmastatus)
{
  uint32_t qspi_manual_config_reg;
  uint32_t prev_bus_mode;
  int32_t status       = RSI_OK;
  qspi_reg_t *qspi_reg = (qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS;
  uint16_t xferLength  = 0;
  uint32_t splitLength = PSRAM_READ_WRITE_PAGE_SIZE;
  uint32_t lengthInBytes;
  uint8_t psramXferBuf[64];
  uint32_t xferAddr;
  static uint32_t rbxOffset;

  if (PSRAMStatus.state != initialised) {
    return PSRAM_NOT_INITIALIZED;
  }

  if (((hSize != sizeof(uint8_t)) && (hSize != sizeof(uint16_t)) && (hSize != sizeof(uint32_t)))) {
    return PSRAM_INVALID_HSIZE;
  }

  if (NULL == SourceBuf) {
    return PSRAM_NULL_ADDRESS;
  }

  if ((!(addr >= PSRAM_BASE_ADDRESS && addr < (PSRAM_BASE_ADDRESS + (PSRAM_Devices[PSRAMStatus.ID].devDensity / 8))))
      || ((addr + (length * hSize)) > (PSRAM_BASE_ADDRESS + (PSRAM_Devices[PSRAMStatus.ID].devDensity / 8)))) {
    return PSRAM_INVALID_ADDRESS_LENGTH;
  }

  lengthInBytes = (length * hSize);

  if (lengthInBytes < splitLength) {
    splitLength = lengthInBytes;
  }

  if ((splitLength / hSize) >= 1024) {
    xferLength = 1024;
  } else {
    xferLength = splitLength / hSize;
  }

  if ((PSRAM_Devices[PSRAMStatus.ID].defaultBurstWrapSize) && (TX_RUNNING != ctx.xferStatus)) {
    rbxOffset = addr % PSRAM_Devices[PSRAMStatus.ID].defaultBurstWrapSize;
  }

  if (0 != rbxOffset) {
    rbxOffset += splitLength;
    if (rbxOffset > PSRAM_Devices[PSRAMStatus.ID].defaultBurstWrapSize) {
      splitLength = splitLength - (rbxOffset - PSRAM_Devices[PSRAMStatus.ID].defaultBurstWrapSize);
      xferLength  = splitLength / hSize;
      rbxOffset   = 0;
    }
  }
  ctx.xferHsize             = hSize;
  ctx.xferNextAddress       = addr + splitLength;
  ctx.xferNextSourceAddress = ((uint8_t *)SourceBuf + splitLength);
  ctx.xferRemLegth          = length - xferLength;
  ctx.xferStatus            = TX_RUNNING;
  ctx.done                  = dmastatus;

  memset(&control, 0, sizeof(RSI_UDMA_CHA_CONFIG_DATA_T));
  memset(&config, 0, sizeof(RSI_UDMA_CHA_CFG_T));

#define QSPI_DEST_CHANNEL 5
#define SIZE_BUFFERS      1024

  config.altStruct       = 0;
  config.burstReq        = 1;
  config.channelPrioHigh = 0;
  config.dmaCh           = QSPI_DEST_CHANNEL;
  config.periAck         = 0;
  config.periphReq       = 0;
  config.reqMask         = 0;

  control.transferType       = UDMA_MODE_BASIC;
  control.nextBurst          = 0;
  control.totalNumOfDMATrans = (xferLength - 1);
  control.rPower             = ARBSIZE_1024;
  control.srcProtCtrl        = 0x000;
  control.dstProtCtrl        = 0x000;

  switch (hSize) {
    case sizeof(uint32_t):
      control.srcSize = SRC_SIZE_32;
      control.srcInc  = SRC_INC_32;
      control.dstSize = DST_SIZE_32;
      break;
    case sizeof(uint16_t):
      control.srcSize = SRC_SIZE_16;
      control.srcInc  = SRC_INC_16;
      control.dstSize = DST_SIZE_16;
      break;
    default:
      control.srcSize = SRC_SIZE_8;
      control.srcInc  = SRC_INC_8;
      control.dstSize = DST_SIZE_8;
  }

  control.dstInc = DST_INC_NONE;

  /* Initialize dma */
  udmaHandle0 = UDMAx_Initialize(&UDMA0_Resources, UDMA0_Table, udmaHandle0, dma_rom_buff0);
  if (udmaHandle0 == NULL) {
    return PSRAM_FAILURE;
  }

  /* Configure dma channel */
  status = UDMAx_ChannelConfigure(&UDMA0_Resources,
                                  config.dmaCh,
                                  (uint32_t)SourceBuf,
                                  (uint32_t)(M4_QSPI_2_BASE_ADDRESS + 0x40),
                                  splitLength / hSize,
                                  control,
                                  &config,
                                  udmaTransferComplete,
                                  udma0_chnl_info,
                                  udmaHandle0);
  if (status != RSI_OK) {
    return PSRAM_FAILURE;
  }

  /* Enable dma channel */
  status = UDMAx_ChannelEnable(QSPI_DEST_CHANNEL, &UDMA0_Resources, udmaHandle0);
  if (status != RSI_OK) {
    RSI_UDMA_DeInit(udmaHandle0, &config);
    return PSRAM_FAILURE;
  }

  autoModeState = PSRAM_Disable_AutoReadWrite();

#if (PSRAM_MODEL_WRAP == 1)
  PRSAM_SetWrapSize(PSRAM_Devices[PSRAMStatus.ID].defaultBurstWrapSize);
  PSRAMStatus.burstSize = PSRAM_Devices[PSRAMStatus.ID].defaultBurstWrapSize;
#endif

  if (PSRAMStatus.spi_config->spi_config_1.inst_mode != PSRAMStatus.spi_config->spi_config_1.addr_mode) {

    if (PSRAMStatus.spi_config->spi_config_2.swap_en == SWAP) {
      xferAddr = ((addr >> 16) & 0xff);
      xferAddr |= (addr & 0xff00);
      xferAddr |= ((addr & 0xff) << 16);
      addr = xferAddr;
    }

    // switch QSPI to command mode
    qspi_switch_qspi2((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                      PSRAMStatus.spi_config->spi_config_1.inst_mode,
                      PSRAMStatus.spi_config->spi_config_2.cs_no);

    prev_bus_mode = PSRAMStatus.spi_config->spi_config_1.inst_mode;

    /*Send the write command to PSRAM*/
    if (PSRAMStatus.spi_config->spi_config_3._16bit_cmd_valid) {
      qspi_write_to_flash(
        (qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
        QSPI_16BIT_LEN,
        ((PSRAMStatus.spi_config->spi_config_3.wr_cmd << 8) | PSRAMStatus.spi_config->spi_config_3._16bit_rd_cmd_msb),
        PSRAMStatus.spi_config->spi_config_2.cs_no);
    } else {
      qspi_write_to_flash(
        (qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
        QSPI_8BIT_LEN,
        (PSRAMStatus.spi_config->spi_config_3.wr_cmd
         | ((PSRAMStatus.spi_config->spi_config_2.addr_width == _9BIT_ADDR) ? ((addr & BIT(8)) << 3) : 0)),
        PSRAMStatus.spi_config->spi_config_2.cs_no);
    }

    // if addr mode is not same as prev bus mode, switch qspi to addr mode
    if (PSRAMStatus.spi_config->spi_config_1.addr_mode != prev_bus_mode) {
      qspi_switch_qspi2((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                        PSRAMStatus.spi_config->spi_config_1.addr_mode,
                        PSRAMStatus.spi_config->spi_config_2.cs_no);
      prev_bus_mode = PSRAMStatus.spi_config->spi_config_1.addr_mode;
    }

    /* Send write Address to PSRAM  */
    qspi_write_to_flash(
      (qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
      (PSRAMStatus.spi_config->spi_config_2.addr_width ? (PSRAMStatus.spi_config->spi_config_2.addr_width * 8) : 8),
      addr,
      PSRAMStatus.spi_config->spi_config_2.cs_no);

    // if extra byte is enabled send that to PSRAM
    if (PSRAMStatus.spi_config->spi_config_1.extra_byte_en) {

      if (PSRAMStatus.spi_config->spi_config_1.extra_byte_mode != prev_bus_mode) {
        qspi_switch_qspi2((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                          PSRAMStatus.spi_config->spi_config_1.extra_byte_mode,
                          PSRAMStatus.spi_config->spi_config_2.cs_no);
        prev_bus_mode = PSRAMStatus.spi_config->spi_config_1.extra_byte_mode;
      }

      qspi_write_to_flash((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                          QSPI_8BIT_LEN,
                          0x00,
                          PSRAMStatus.spi_config->spi_config_2.cs_no);
    }

    if (PSRAMStatus.spi_config->spi_config_1.data_mode != prev_bus_mode) {
      qspi_switch_qspi2((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                        PSRAMStatus.spi_config->spi_config_1.data_mode,
                        PSRAMStatus.spi_config->spi_config_2.cs_no);
    }

  } else {
    addr            = addr & 0xffffff;
    psramXferBuf[0] = PSRAMStatus.spi_config->spi_config_3.wr_cmd;
    psramXferBuf[1] = (addr >> 16) & 0xff;
    psramXferBuf[2] = (addr >> 8) & 0xff;
    psramXferBuf[3] = addr & 0xff;

    QSPI_Transmit((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                  sizeof(uint8_t),
                  psramXferBuf,
                  4,
                  PSRAMStatus.spi_config->spi_config_2.cs_no,
                  1); // do not deassert the chip
  }

  // configure qspi for read
  qspi_manual_config_reg = (qspi_reg->QSPI_MANUAL_CONFIG_REG & ~0xF8387FFF);
  qspi_manual_config_reg |= (PSRAMStatus.spi_config->spi_config_2.cs_no << 13) | ((hSize - 1) << 19) | TAKE_LEN_FRM_REG;

  // trigger write
  qspi_reg->QSPI_MANUAL_CONFIG_REG = qspi_manual_config_reg | WRITE_TRIGGER | ((splitLength & 0x3FF) << 3)
                                     | (((splitLength >> 10) & 0x1F) << 27);

  /* Enable dma controller */
  status = UDMAx_DMAEnable(&UDMA0_Resources, udmaHandle0);
  if (status != RSI_OK) {
    return PSRAM_FAILURE;
  }

  status = RSI_UDMA_ChannelSoftwareTrigger(udmaHandle0, QSPI_DEST_CHANNEL);
  if (status != RSI_OK) {
    return PSRAM_FAILURE;
  }
  return PSRAM_SUCCESS;
}

PSRAMReturnType PSRAM_ManualReadInDMAMode(uint32_t addr,
                                          void *DestBuf,
                                          uint8_t hSize,
                                          uint32_t length,
                                          PSRAMDMAStatusType *dmastatus)
{
  uint32_t qspi_manual_config_reg;
  uint32_t prev_bus_mode;
  uint32_t dummy_cnt, tmp_dummy;
  int32_t status       = RSI_OK;
  qspi_reg_t *qspi_reg = (qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS;
  uint16_t xferLength  = 0;
  uint32_t splitLength = PSRAM_READ_WRITE_PAGE_SIZE;
  uint32_t lengthInBytes;
  uint8_t psramXferBuf[64];
  uint32_t xferAddr;
  static uint32_t rbxOffset;

  if (PSRAMStatus.state != initialised) {
    return PSRAM_NOT_INITIALIZED;
  }

  if (((hSize != sizeof(uint8_t)) && (hSize != sizeof(uint16_t)) && (hSize != sizeof(uint32_t)))) {
    return PSRAM_INVALID_HSIZE;
  }

  if (NULL == DestBuf) {
    return PSRAM_NULL_ADDRESS;
  }

  if ((!(addr >= PSRAM_BASE_ADDRESS && addr < (PSRAM_BASE_ADDRESS + (PSRAM_Devices[PSRAMStatus.ID].devDensity / 8))))
      || ((addr + (length * hSize)) > (PSRAM_BASE_ADDRESS + (PSRAM_Devices[PSRAMStatus.ID].devDensity / 8)))) {
    return PSRAM_INVALID_ADDRESS_LENGTH;
  }

  lengthInBytes = (length * hSize);

  if (lengthInBytes < splitLength) {
    splitLength = lengthInBytes;
  }

  if ((splitLength / hSize) >= 1024) {
    xferLength = 1024;
  } else {
    xferLength = splitLength / hSize;
  }

  if ((PSRAM_Devices[PSRAMStatus.ID].defaultBurstWrapSize) && (RX_RUNNING != ctx.xferStatus)) {
    rbxOffset = addr % PSRAM_Devices[PSRAMStatus.ID].defaultBurstWrapSize;
  }

  if (0 != rbxOffset) {
    rbxOffset += splitLength;
    if (rbxOffset > PSRAM_Devices[PSRAMStatus.ID].defaultBurstWrapSize) {
      splitLength = splitLength - (rbxOffset - PSRAM_Devices[PSRAMStatus.ID].defaultBurstWrapSize);
      xferLength  = splitLength / hSize;
      rbxOffset   = 0;
    }
  }

  ctx.xferHsize             = hSize;
  ctx.xferNextAddress       = addr + splitLength;
  ctx.xferNextSourceAddress = ((uint8_t *)DestBuf + splitLength);
  ctx.xferRemLegth          = length - xferLength;
  ctx.xferStatus            = RX_RUNNING;
  ctx.done                  = dmastatus;

  memset(&control, 0, sizeof(RSI_UDMA_CHA_CONFIG_DATA_T));
  memset(&config, 0, sizeof(RSI_UDMA_CHA_CFG_T));

#define QSPI_SRC_CHANNEL 4
#define SIZE_BUFFERS     1024

  config.altStruct       = 0;
  config.burstReq        = 1;
  config.channelPrioHigh = 0;
  config.dmaCh           = QSPI_SRC_CHANNEL;
  config.periAck         = 0;
  config.periphReq       = 0;
  config.reqMask         = 0;

  control.transferType       = UDMA_MODE_BASIC;
  control.nextBurst          = 0;
  control.totalNumOfDMATrans = (xferLength - 1);
  control.rPower             = ARBSIZE_1024;
  control.srcProtCtrl        = 0x000;
  control.dstProtCtrl        = 0x000;

  switch (hSize) {
    case sizeof(uint32_t):
      control.srcSize = SRC_SIZE_32;
      control.dstInc  = SRC_INC_32;
      control.dstSize = DST_SIZE_32;
      break;
    case sizeof(uint16_t):
      control.srcSize = SRC_SIZE_16;
      control.dstInc  = SRC_INC_16;
      control.dstSize = DST_SIZE_16;
      break;
    default:
      control.srcSize = SRC_SIZE_8;
      control.dstInc  = SRC_INC_8;
      control.dstSize = DST_SIZE_8;
  }

  control.srcInc = SRC_INC_NONE;

  /* Initialize dma */
  udmaHandle0 = UDMAx_Initialize(&UDMA0_Resources, UDMA0_Table, udmaHandle0, dma_rom_buff0);
  if (udmaHandle0 == NULL) {
    return PSRAM_FAILURE;
  }

  /* Configure dma channel */
  status = UDMAx_ChannelConfigure(&UDMA0_Resources,
                                  config.dmaCh,
                                  (uint32_t)(M4_QSPI_2_BASE_ADDRESS + 0x40),
                                  (uint32_t)DestBuf,
                                  splitLength / hSize,
                                  control,
                                  &config,
                                  udmaTransferComplete,
                                  udma0_chnl_info,
                                  udmaHandle0);
  if (status != RSI_OK) {
    return PSRAM_FAILURE;
  }

  /* Enable dma channel */
  status = UDMAx_ChannelEnable(QSPI_SRC_CHANNEL, &UDMA0_Resources, udmaHandle0);
  if (status != RSI_OK) {
    RSI_UDMA_DeInit(udmaHandle0, &config);
    return PSRAM_FAILURE;
  }

  autoModeState = PSRAM_Disable_AutoReadWrite();

#if (PSRAM_MODEL_WRAP == 1)
  PRSAM_SetWrapSize(PSRAM_Devices[PSRAMStatus.ID].defaultBurstWrapSize);
  PSRAMStatus.burstSize = PSRAM_Devices[PSRAMStatus.ID].defaultBurstWrapSize;
#endif

  if (PSRAMStatus.spi_config->spi_config_1.inst_mode != PSRAMStatus.spi_config->spi_config_1.addr_mode) {

    if (PSRAMStatus.spi_config->spi_config_2.swap_en == SWAP) {
      xferAddr = ((addr >> 16) & 0xff);
      xferAddr |= (addr & 0xff00);
      xferAddr |= ((addr & 0xff) << 16);
      addr = xferAddr;
    }

    // switch QSPI to command mode
    qspi_switch_qspi2((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                      PSRAMStatus.spi_config->spi_config_1.inst_mode,
                      PSRAMStatus.spi_config->spi_config_2.cs_no);

    prev_bus_mode = PSRAMStatus.spi_config->spi_config_1.inst_mode;

    /*Send the Read command to PSRAM*/
    if (PSRAMStatus.spi_config->spi_config_3._16bit_cmd_valid) {
      qspi_write_to_flash(
        (qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
        QSPI_16BIT_LEN,
        ((PSRAMStatus.spi_config->spi_config_1.read_cmd << 8) | PSRAMStatus.spi_config->spi_config_3._16bit_rd_cmd_msb),
        PSRAMStatus.spi_config->spi_config_2.cs_no);
    } else {
      qspi_write_to_flash(
        (qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
        QSPI_8BIT_LEN,
        (PSRAMStatus.spi_config->spi_config_1.read_cmd
         | ((PSRAMStatus.spi_config->spi_config_2.addr_width == _9BIT_ADDR) ? ((addr & BIT(8)) << 3) : 0)),
        PSRAMStatus.spi_config->spi_config_2.cs_no);
    }

    // if addr mode is not same as prev bus mode, switch qspi to addr mode
    if (PSRAMStatus.spi_config->spi_config_1.addr_mode != prev_bus_mode) {
      qspi_switch_qspi2((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                        PSRAMStatus.spi_config->spi_config_1.addr_mode,
                        PSRAMStatus.spi_config->spi_config_2.cs_no);
      prev_bus_mode = PSRAMStatus.spi_config->spi_config_1.addr_mode;
    }

    /* Send Read Address to PSRAM  */
    qspi_write_to_flash(
      (qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
      (PSRAMStatus.spi_config->spi_config_2.addr_width ? (PSRAMStatus.spi_config->spi_config_2.addr_width * 8) : 8),
      addr,
      PSRAMStatus.spi_config->spi_config_2.cs_no);

    // if extra byte is enabled send that to PSRAM
    if (PSRAMStatus.spi_config->spi_config_1.extra_byte_en) {

      if (PSRAMStatus.spi_config->spi_config_1.extra_byte_mode != prev_bus_mode) {
        qspi_switch_qspi2((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                          PSRAMStatus.spi_config->spi_config_1.extra_byte_mode,
                          PSRAMStatus.spi_config->spi_config_2.cs_no);
        prev_bus_mode = PSRAMStatus.spi_config->spi_config_1.extra_byte_mode;
      }

      qspi_write_to_flash((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                          QSPI_8BIT_LEN,
                          0x00,
                          PSRAMStatus.spi_config->spi_config_2.cs_no);
    }

    // if dummy bytes are configured, send them to PSRAM
    if ((PSRAMStatus.spi_config->spi_config_1.no_of_dummy_bytes)
        || (PSRAMStatus.spi_config->spi_config_4.no_of_ms_dummy_bytes)) {
      dummy_cnt = PSRAMStatus.spi_config->spi_config_1.no_of_dummy_bytes;
      dummy_cnt |= (PSRAMStatus.spi_config->spi_config_4.no_of_ms_dummy_bytes << 4);

      if (PSRAMStatus.spi_config->spi_config_1.dummy_mode != prev_bus_mode) {
        qspi_switch_qspi2((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                          PSRAMStatus.spi_config->spi_config_1.dummy_mode,
                          PSRAMStatus.spi_config->spi_config_2.cs_no);
        prev_bus_mode = PSRAMStatus.spi_config->spi_config_1.dummy_mode;
      }

      // dummy write

      do {

        tmp_dummy = (dummy_cnt & 0x3) ? (dummy_cnt & 3) : 4;

        qspi_write_to_flash((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                            (QSPI_8BIT_LEN * tmp_dummy),
                            0x00,
                            PSRAMStatus.spi_config->spi_config_2.cs_no);
        dummy_cnt -= tmp_dummy;

      } while (dummy_cnt);
    }

    if (PSRAMStatus.spi_config->spi_config_1.data_mode != prev_bus_mode) {
      qspi_switch_qspi2((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                        PSRAMStatus.spi_config->spi_config_1.data_mode,
                        PSRAMStatus.spi_config->spi_config_2.cs_no);
    }
  } else {
    addr            = addr & 0xffffff;
    psramXferBuf[0] = PSRAMStatus.spi_config->spi_config_1.read_cmd;
    psramXferBuf[1] = (addr >> 16) & 0xff;
    psramXferBuf[2] = (addr >> 8) & 0xff;
    psramXferBuf[3] = addr & 0xff;
    psramXferBuf[4] = 0x00;
    psramXferBuf[5] = 0x00;
    psramXferBuf[6] = 0x00;

    QSPI_Transmit((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                  sizeof(uint8_t),
                  psramXferBuf,
                  7,
                  PSRAMStatus.spi_config->spi_config_2.cs_no,
                  1); // do not deassert the chip
  }

  // configure qspi for read
  qspi_manual_config_reg = (qspi_reg->QSPI_MANUAL_CONFIG_REG & ~0xF8387FFF);
  qspi_manual_config_reg |= (PSRAMStatus.spi_config->spi_config_2.cs_no << 13) | ((hSize - 1) << 19) | TAKE_LEN_FRM_REG;

  // trigger write
  qspi_reg->QSPI_MANUAL_CONFIG_REG = qspi_manual_config_reg | READ_TRIGGER | ((splitLength & 0x3FF) << 3)
                                     | (((splitLength >> 10) & 0x1F) << 27);

  /* Enable dma controller */
  status = UDMAx_DMAEnable(&UDMA0_Resources, udmaHandle0);
  if (status != RSI_OK) {
    return PSRAM_FAILURE;
  }

  status = RSI_UDMA_ChannelSoftwareTrigger(udmaHandle0, QSPI_SRC_CHANNEL);
  if (status != RSI_OK) {
    return PSRAM_FAILURE;
  }
  return PSRAM_SUCCESS;
}

PSRAMReturnType PRSAM_Enter_QPIMode(void)
{

  uint8_t command[2];

  /*Set the PSRAM device to QPI mode*/
  command[0] = PSRAM_ENTER_QPI;

  QSPI_Transmit((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                sizeof(uint8_t),
                command,
                1,
                PSRAMStatus.spi_config->spi_config_2.cs_no,
                0);

  return PSRAM_SUCCESS;
}

PSRAMReturnType PRSAM_Exit_QPIMode(void)
{

  uint8_t command[2];

  /*Reset the PSRAM device to SPI mode*/
  command[0] = PSRAM_EXIT_QPI;

  QSPI_Transmit((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                sizeof(uint8_t),
                command,
                1,
                PSRAMStatus.spi_config->spi_config_2.cs_no,
                0);

  return PSRAM_SUCCESS;
}

PSRAMReturnType PSRAM_Enable_AutoReadWrite(void)
{

  qspi_reg_t *qspi_reg = (qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS;

  /*Wait till QSPI Auto becomes Idle */
  while (!(qspi_reg->QSPI_STATUS_REG & AUTO_MODE_FSM_IDLE_SCLK)) {
    // wait till auto mode becomes idle
  }

  /*Check, If already is in Auto mode */
  if ((qspi_reg->QSPI_STATUS_REG & HW_CTRLD_QSPI_MODE_CTRL_SCLK)) {
    return PSRAM_SUCCESS;
  }

  if (qspi_reg->QSPI_BUS_MODE_REG & AUTO_MODE) {
    return PSRAM_AUTO_MODE;
  }

  if (!(qspi_reg->QSPI_STATUS_REG & HW_CTRLD_QSPI_MODE_CTRL_SCLK)) {
    while (!(qspi_reg->QSPI_MANUAL_CONFIG_REG & 0x1)) {
      // wait till manual becomes idle
    }
    qspi_reg->QSPI_BUS_MODE_REG |= AUTO_MODE;
  }

  return PSRAM_SUCCESS;
}

PSRAMReturnType PSRAM_Disable_AutoReadWrite(void)
{

  qspi_reg_t *qspi_reg = (qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS;

  while (!(qspi_reg->QSPI_STATUS_REG & AUTO_MODE_FSM_IDLE_SCLK)) {
    // wait till auto mode becomes idle
  }

  // Check if already not in auto mode

  if ((qspi_reg->QSPI_STATUS_REG & HW_CTRLD_QSPI_MODE_CTRL_SCLK)) {
    return PSRAM_SUCCESS;
  }

  if (!(qspi_reg->QSPI_BUS_MODE_REG & AUTO_MODE)) {
    return PSRAM_MANUAL_MODE;
  }

  qspi_reg->QSPI_BUS_MODE_REG &= ~AUTO_MODE;
  while (qspi_reg->QSPI_STATUS_REG & AUTO_MODE_ENABLED)
    ;

  return PSRAM_SUCCESS;
}

PSRAMReturnType PRSAM_Reset(void)
{
  uint8_t command;
  if (PSRAMStatus.spi_config == NULL) {
    return PSRAM_NOT_INITIALIZED;
  }

  /*Send the RESET ENable command first*/
  command = PSRAM_RESET_EN;
  // switch QSPI to command mode
  qspi_switch_qspi2((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                    PSRAMStatus.spi_config->spi_config_1.inst_mode,
                    PSRAMStatus.spi_config->spi_config_2.cs_no);

  QSPI_Transmit((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                sizeof(uint8_t),
                &command,
                1,
                PSRAMStatus.spi_config->spi_config_2.cs_no,
                0);

  /*Send the RESET command*/
  command = PSRAM_RESET;
  QSPI_Transmit((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                sizeof(uint8_t),
                &command,
                1,
                PSRAMStatus.spi_config->spi_config_2.cs_no,
                0);

  return PSRAM_SUCCESS;
}

PSRAMReturnType PRSAM_Sleep(void)
{
  uint8_t command;
  uint32_t tDelay;

  if (PSRAMStatus.spi_config == NULL) {
    return PSRAM_NOT_INITIALIZED;
  }

  if (PSRAMStatus.spiMode == QUAD_MODE) {
    PRSAM_Exit_QPIMode();
  }

  /*Initialize the QSPI controller to PSRAM default mode configuration*/
  qspi_spi_init((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS, (spi_config_t *)&spi_psram_default_config, 0, 0, 0);

  command = PSRAM_HALF_SLEEP;
  QSPI_Transmit((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                sizeof(uint8_t),
                &command,
                1,
                PSRAMStatus.spi_config->spi_config_2.cs_no,
                0);

  /*Make sure tHS is not violated*/
  tDelay = ((tHS_US * 1000000) / (796 * (SystemCoreClock / 1000000)));
  for (int i = 0; i < tDelay; i++) {
    __asm("nop");
  }
  return PSRAM_SUCCESS;
}

PSRAMReturnType PRSAM_Wakeup(void)
{
  uint32_t tDelay      = 0;
  qspi_reg_t *qspi_reg = (qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS;

  qspi_reg->QSPI_MANUAL_CONFIG_REG = (qspi_reg->QSPI_MANUAL_CONFIG_REG & 0xFFFFFFFE);

  /*Make sure tXPHS is not violated*/

  //	200 counts = 12.75us at 80 Mhz
  //	1 count = 0.063us at 80 Mhz
  //	12750000 / (796 * 80) = count

  tDelay = ((tXPHS_US * 1000000) / (796 * (SystemCoreClock / 1000000)));
  for (int i = 0; i < tDelay; i++) {
    __asm("nop");
  }

  qspi_reg->QSPI_MANUAL_CONFIG_REG = (qspi_reg->QSPI_MANUAL_CONFIG_REG | 0x1);

  /*Make sure tXHS is not violated*/
  tDelay = ((tXHS_US * 1000000) / (796 * (SystemCoreClock / 1000000)));
  for (int i = 0; i < tDelay; i++) {
    __asm("nop");
  }

  return PSRAM_Init(PSRAMStatus.ID);
}

PSRAMReturnType PRSAM_ToggleBurstLength(void)
{

  if (PSRAMStatus.state != initialised) {
    return PSRAM_NOT_INITIALIZED;
  }

  if (PSRAMStatus.spi_config == NULL) {
    return PSRAM_NOT_INITIALIZED;
  }

  /*Reset the PSRAM device to SPI mode*/

  // switch QSPI to command mode
  qspi_switch_qspi2((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                    PSRAMStatus.spi_config->spi_config_1.inst_mode,
                    PSRAMStatus.spi_config->spi_config_2.cs_no);

  QSPI_Transmit((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                sizeof(uint8_t),
                (uint8_t *)PSRAM_BURST_LEN,
                1,
                PSRAMStatus.spi_config->spi_config_2.cs_no,
                0);

  if (PSRAMStatus.burstSize == PSRAM_Devices[PSRAMStatus.ID].defaultBurstWrapSize) {
    PSRAMStatus.burstSize = PSRAM_Devices[PSRAMStatus.ID].toggleBurstWrapSize;
  } else {
    PSRAMStatus.burstSize = PSRAM_Devices[PSRAMStatus.ID].defaultBurstWrapSize;
  }

  return PSRAM_SUCCESS;
}

PSRAMReturnType PRSAM_SetWrapSize(PSRAMBurstSizeType PSRAMBurstSize)
{

  uint8_t command[8];
  uint8_t Response;

  if (PSRAMStatus.state != initialised) {
    return PSRAM_NOT_INITIALIZED;
  }

  if (PSRAMStatus.spi_config == NULL) {
    return PSRAM_NOT_INITIALIZED;
  }

  // switch QSPI to command mode
  qspi_switch_qspi2((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                    PSRAMStatus.spi_config->spi_config_1.inst_mode,
                    PSRAMStatus.spi_config->spi_config_2.cs_no);

  /*Read Mode Resgister*/
  command[0] = PSRAM_MODE_REG_READ;
  command[1] = 0x00;
  command[2] = 0x00;
  command[3] = 0x00;
  command[4] = 0x00;
  command[5] = 0x00;
  command[6] = 0x00;

  QSPI_Transmit((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                sizeof(uint8_t),
                &command[0],
                7,
                PSRAMStatus.spi_config->spi_config_2.cs_no,
                1); // Do not de-assert the CS

  QSPI_Receive((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
               sizeof(uint8_t),
               ((uint8_t *)&Response),
               1,
               PSRAMStatus.spi_config->spi_config_2.cs_no,
               0); // de-assert the CS

  Response &= 0x9F;
  Response |= (PSRAMBurstSize << 5);

  /*Write Mode Resgister*/
  command[0] = PSRAM_MODE_REG_WRITE;
  command[1] = 0x00;
  command[2] = 0x00;
  command[3] = 0x00;
  command[4] = Response;

  QSPI_Transmit((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                sizeof(uint8_t),
                &command[0],
                5,
                PSRAMStatus.spi_config->spi_config_2.cs_no,
                0); // Do not de-assert the CS

  /*Read Mode Resgister*/
  command[0] = PSRAM_MODE_REG_READ;
  command[1] = 0x00;
  command[2] = 0x00;
  command[3] = 0x00;
  command[4] = 0x00;
  command[5] = 0x00;
  command[6] = 0x00;

  QSPI_Transmit((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                sizeof(uint8_t),
                &command[0],
                7,
                PSRAMStatus.spi_config->spi_config_2.cs_no,
                1); // Do not de-assert the CS

  QSPI_Receive((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
               sizeof(uint8_t),
               ((uint8_t *)&Response),
               1,
               PSRAMStatus.spi_config->spi_config_2.cs_no,
               0); // de-assert the CS

  return PSRAM_SUCCESS;
}

PSRAMReturnType PSRAM_Enable_Encry_Decry(uint16_t keySize)
{

  if (PSRAMStatus.state != initialised) {
    return PSRAM_NOT_INITIALIZED;
  }

  if (PSRAMStatus.ID >= MAX_SUPPORTED_PSRAM_DEVICES) {
    return PSRAM_UNKNOWN_DEVICE;
  }

  qspi_qspiload_key_ext((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS, keySize);

  PSRAMStatus.secureModeEnable = true;

  return PSRAM_SUCCESS;
}

void QSPI_Transmit(qspi_reg_t *qspi_reg,
                   uint8_t hSize,
                   void *data,
                   uint32_t length,
                   uint8_t chipSelect,
                   uint8_t chipSelectContinue)
{

  uint8_t noOfElelments;

  if (((hSize != sizeof(uint8_t)) && (hSize != sizeof(uint16_t)) && (hSize != sizeof(uint32_t)))) {
    return;
  }

  hSize = hSize - 1;

  while (length) {

    if (length >= 16) {
      noOfElelments = 16;
    } else {
      noOfElelments = (length % 16);
    }

    for (uint32_t fifIndex = 0; fifIndex < noOfElelments; fifIndex++) {

      if (_32BIT == hSize) {
        // length of the word to be programmed
        qspi_reg->QSPI_MANUAL_WRITE_DATA_2_REG = 32;
        // write the data
        qspi_reg->QSPI_MANUAL_RD_WR_DATA_REG = *((uint32_t *)data);
        data                                 = ((uint32_t *)data) + 1;
      } else if (_16BIT == hSize) {
        // length of the word to be programmed
        qspi_reg->QSPI_MANUAL_WRITE_DATA_2_REG = 16;
        // write the data
        qspi_reg->QSPI_MANUAL_RD_WR_DATA_REG = *((uint16_t *)data);
        data                                 = ((uint16_t *)data) + 1;
      } else if (_8BIT == hSize) {
        // length of the word to be programmed
        qspi_reg->QSPI_MANUAL_WRITE_DATA_2_REG = 8;
        // write the data
        qspi_reg->QSPI_MANUAL_RD_WR_DATA_REG = *((uint8_t *)data);
        data                                 = ((uint8_t *)data) + 1;
      }

      length -= 1;
    }
    qspi_reg->QSPI_MANUAL_CONFIG_REG =
      (((qspi_reg->QSPI_MANUAL_CONFIG_REG & ~0x6007) & ~TAKE_LEN_FRM_REG) | WRITE_TRIGGER | (chipSelect << 13));

    // wait till QSPI becomes idle
    while (qspi_reg->QSPI_STATUS_REG & 1)
      ;
  }

  if (!chipSelectContinue) {
    DEASSERT_CSN;
  }
}

void QSPI_Receive(qspi_reg_t *qspi_reg,
                  uint8_t hSize,
                  void *data,
                  uint32_t length,
                  uint8_t chipSelect,
                  uint8_t chipSelectContinue)
{

  uint32_t qspi_manual_config_reg;
  uint32_t readCount;
  uint32_t lengthInBytes;
  uint16_t *data_16bit;
  uint32_t *data_32bit;

  if (((hSize != sizeof(uint8_t)) && (hSize != sizeof(uint16_t)) && (hSize != sizeof(uint32_t)))) {
    return;
  }

  data_16bit = (uint16_t *)data;
  data_32bit = (uint32_t *)data;

  lengthInBytes = length * hSize;

  hSize = hSize - 1;

  while (lengthInBytes) {

    if (lengthInBytes > ((32 * 1024) - 4)) // 32kbytes is a max allowed size in QSPI read length
    {
      readCount = ((32 * 1024) - 4);
    } else {
      readCount = lengthInBytes;
    }

    lengthInBytes = lengthInBytes - readCount;

    // configure qspi for read
    qspi_manual_config_reg = (qspi_reg->QSPI_MANUAL_CONFIG_REG & ~0xF8387FFF);
    qspi_manual_config_reg |= (chipSelect << 13) | ((hSize) << 19) | TAKE_LEN_FRM_REG;

    // trigger qspi to read loop_count bytes from flash
    qspi_reg->QSPI_MANUAL_CONFIG_REG = qspi_manual_config_reg | READ_TRIGGER | ((readCount & 0x3FF) << 3)
                                       | (((readCount >> 10) & 0x1F) << 27);

    // reads

    if (hSize == _32BIT) {
      do {
        while (qspi_reg->QSPI_STATUS_REG & QSPI_FIFO_EMPTY_RFIFO_S) {
          // wait till the fifo empty is deasserted
        }
        *(data_32bit) = (uint32_t)qspi_reg->QSPI_MANUAL_RD_WR_DATA_REG;
        data_32bit++;
        readCount -= 4;
      } while (readCount);
    } else if (hSize == _16BIT) {
      do {
        while (qspi_reg->QSPI_STATUS_REG & QSPI_FIFO_EMPTY_RFIFO_S) {
          // wait till the fifo empty is deasserted
        }
        *(data_16bit) = (uint16_t)qspi_reg->QSPI_MANUAL_RD_WR_DATA_REG;
        data_16bit++;
        readCount -= 2;
      } while (readCount);
    } else if (hSize == _8BIT) {
      do {
        while (qspi_reg->QSPI_STATUS_REG & QSPI_FIFO_EMPTY_RFIFO_S) {
          // wait till the fifo empty is deasserted
        }
        *(uint8_t *)(data) = (uint8_t)qspi_reg->QSPI_MANUAL_RD_WR_DATA_REG;
        ((uint8_t *)(data))++;
        readCount -= 1;
      } while (readCount);
    }
  }

  if (!chipSelectContinue) {
    DEASSERT_CSN;
  }
}

void QSPI_Deassert_CSN(qspi_reg_t *qspi_reg)
{
  DEASSERT_CSN;
}

void QSPI_SRAM_Auto_Init(qspi_reg_t *qspi_reg, spi_config_t *spi_config)
{
  uint32_t *sram_ctrl_csn_ptr;

  switch (spi_config->spi_config_2.cs_no) {
    case CHIP_ONE:
      sram_ctrl_csn_ptr = ((uint32_t *)&qspi_reg->QSPI_SRAM_CTRL_CSN1_REG);
      break;
    case CHIP_TWO:
      sram_ctrl_csn_ptr = ((uint32_t *)&qspi_reg->QSPI_SRAM_CTRL_CSN2_REG);
      break;
    case CHIP_THREE:
      sram_ctrl_csn_ptr = ((uint32_t *)&qspi_reg->QSPI_SRAM_CTRL_CSN3_REG);
      break;
    default:
      sram_ctrl_csn_ptr = ((uint32_t *)&qspi_reg->QSPI_SRAM_CTRL_CSN0_REG);
      break;
  }

  *sram_ctrl_csn_ptr = ((spi_config->spi_config_3.wr_inst_mode << 8)    //< cmd mode
                        | (spi_config->spi_config_3.wr_addr_mode << 10) //< addr mode
                        | (spi_config->spi_config_3.wr_data_mode << 12) //< data mode
                        | (spi_config->spi_config_3.wr_cmd << 16));

  // enable write data swapping for auto mode
  if (spi_config->spi_config_2.swap_en == SWAP) {
    *sram_ctrl_csn_ptr |= (SWAP << 14);
  }
}

void QSPI_SRAM_WRAP_Init(qspi_reg_t *qspi_reg, spi_config_t *spi_config)
{
  uint32_t offset;
  uint32_t *auto_1_ptr;
  uint32_t *auto_2_ptr;
  // address difference between auto config reg for csn 0 and 1 is 12regs
  offset     = 12 * spi_config->spi_config_2.cs_no;
  auto_1_ptr = ((uint32_t *)&qspi_reg->QSPI_AUTO_CTRL_CONFIG_1_REG + offset);
  auto_2_ptr = ((uint32_t *)&qspi_reg->QSPI_AUTO_CTRL_CONFIG_2_REG + offset);

  // Enable burst mode read in qspi
  qspi_reg->QSPI_BUS_MODE_REG |= QSPI_WRAP_EN;

  if (PSRAM_SPI_MODE == SINGLE_MODE) {
    // no. of dummy bytes for wrap
    *auto_1_ptr |= 4 << 28;
    // read cmd for wrap mode
    *auto_2_ptr |= 0x8B << 16;

  } else if (PSRAM_SPI_MODE == QUAD_MODE) {
    // no. of dummy bytes for wrap
    *auto_1_ptr |= 3 << 28;
    // read cmd for wrap mode
    *auto_2_ptr |= 0x8B << 16;
  } else {
  }
}

void QSPI_Set_Swap_Mode(qspi_reg_t *qspi_reg, spi_config_t *spi_config)
{
  /*Set Swap for Manual transactions */
  if (spi_config->spi_config_2.swap_en) {
    // Manual Write Swap
    qspi_reg->QSPI_MANUAL_CONFIG_2_REG |= (SWAP << spi_config->spi_config_2.cs_no);
    // Manual Read Swap
    qspi_reg->QSPI_MANUAL_CONFIG_2_REG |= (SWAP << (4 + spi_config->spi_config_2.cs_no));
  } else {
    // Manual Write Swap
    qspi_reg->QSPI_MANUAL_CONFIG_2_REG &= (~(SWAP << spi_config->spi_config_2.cs_no));
    // Manual Read Swap
    qspi_reg->QSPI_MANUAL_CONFIG_2_REG &= (~(SWAP << (4 + spi_config->spi_config_2.cs_no)));
  }
}

void qspi_qspiload_key_ext(qspi_reg_t *qspi_reg, uint16_t key_size)
{
  uint8_t segIndex;
  uint32_t aes_config;
  uint32_t aes_octa_spi_controller_2;

  aes_config                = qspi_reg->QSPI_AES_CONFIG;
  aes_octa_spi_controller_2 = qspi_reg->OCTA_SPI_BUS_CONTROLLER2;
  aes_config                = CTR_MODE | KEY_FLIP_FOR_KH_INTF;

  if (256 == key_size) {
    aes_config |= KEY_SIZE_256;
    qspi_reg->QSPI_AES_KEY_IV_VALID = ((0xF << 8) | (0xF << 4) | 0xF);
  } else {
    aes_config &= (~KEY_SIZE_256);
    qspi_reg->QSPI_AES_KEY_IV_VALID = ((0xF << 8) | (0xF << 4));
  }

  qspi_reg->QSPI_AES_CONFIG = aes_config;
  qspi_reg->QSPI_AES_SEC_KEY_FRM_KH |= LOAD_SEC_KEY_FRM_KH;
  while (qspi_reg->QSPI_AES_SEC_KEY_FRM_KH & LOAD_SEC_KEY_FRM_KH)
    ;

  for (segIndex = 0; segIndex < MAX_SEC_SEGMENTS; segIndex++) {
    if (PSRAMSecureSegments[segIndex].segmentEnable) {
      qspi_reg->QSPI_AES_SEC_SEG_ADDR[segIndex * 2]     = PSRAMSecureSegments[segIndex].lowerBoundary;
      qspi_reg->QSPI_AES_SEC_SEG_ADDR[segIndex * 2 + 1] = PSRAMSecureSegments[segIndex].higherBoundary;
      aes_octa_spi_controller_2 |= (BIT(segIndex) << EN_SEG_SEC); // enabling security for segment
    }
  }

  aes_octa_spi_controller_2 |= (EN_KH_KEY | EN_SECURITY);

  qspi_reg->OCTA_SPI_BUS_CONTROLLER2 = aes_octa_spi_controller_2; // enabling security ;
}

void qspi_qspiunload_key_ext(qspi_reg_t *qspi_reg)
{

  qspi_reg->OCTA_SPI_BUS_CONTROLLER2 &= ~(EN_KH_KEY | EN_SECURITY);

  qspi_reg->QSPI_AES_CONFIG = DEFAULT_AES_CONFIG;

  qspi_reg->QSPI_AES_KEY_IV_VALID = DEFAULT_AES_KEY_IV_VALID;

  qspi_reg->QSPI_AES_SEC_KEY_FRM_KH = DEFAULT_AES_SEC_KEY_FRM_KH;
}
