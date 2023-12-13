/*******************************************************************************
 * @file  sl_si91x_psram_handle.c
 * @brief
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
#include "sl_si91x_psram_handle.h"

sl_psram_info_type_t PSRAM_Device = {

    .devDensity = Mbit_(PSRAM_DEVICE_DENSITY_MBIT),
    .deviceName = PSRAM_DEVICE_NAME,
    .deviceID = {.MFID = PSRAM_DEVICE_MFID,
                 .KGD = PSRAM_DEVICE_KGD,
                 .EID = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0}},
    .normalReadMAXFrequency = MHz_(PSRAM_NORMAL_READ_MAX_FREQ_MHZ),
    .fastReadMAXFrequency = MHz_(PSRAM_FAST_READ_MAX_FREQ_MHZ),
    .rwType = PSRAM_RW_TYPE,
    .defaultBurstWrapSize = PSRAM_DEFAULT_BURST_WRAP_SIZE,
    .toggleBurstWrapSize = PSRAM_TOGGLE_BURST_WRAP_SIZE,

    .spi_config = {

        .spi_config_1 = {

            .inst_mode = PSRAM_INST_MODE,
            .addr_mode = PSRAM_ADDR_MODE,
            .dummy_mode = PSRAM_DUMMY_MODE,
            .data_mode = PSRAM_DIO_MODE, 
            .extra_byte_mode = PSRAM_EXTRA_BYTE_MODE,
            .prefetch_en = DIS_PREFETCH,
            .dummy_W_or_R = DUMMY_READS,
            .d3d2_data = 0x11,
            .continuous = DIS_CONTINUOUS,
            .read_cmd = PSRAM_READ_CMD_CODE,
            .flash_type = 255,
            .no_of_dummy_bytes = (PSRAM_RD_DUMMY_BITS/8),
            .extra_byte_en = 0,
        },

       .spi_config_2 = {

            .auto_mode = EN_AUTO_MODE,
            .cs_no = PSRAM_CHIP_SELECT,
            .neg_edge_sampling = NEG_EDGE_SAMPLING,
            .qspi_clk_en = QSPI_FULL_TIME_CLK,
            .protection = DNT_REM_WR_PROT,
            .dma_mode = NO_DMA,
            .swap_en = SWAP,
            .full_duplex = IGNORE_FULL_DUPLEX,
            .wrap_len_in_bytes = NO_WRAP,
            .addr_width_valid = 0,
            .addr_width = _24BIT_ADDR,
            .pinset_valid = 0,
            .dummy_cycles_for_controller = 0,
        },

       .spi_config_3 = {

            .xip_mode = 0,
            ._16bit_cmd_valid = 0,
            ._16bit_rd_cmd_msb = 0,
            .reserved = 0, 
            .wr_cmd = PSRAM_WRITE_CMD_CODE,
            .wr_inst_mode = PSRAM_INST_MODE,
            .wr_addr_mode = PSRAM_ADDR_MODE,
            .wr_data_mode = PSRAM_DIO_MODE,
            .dummys_4_jump = 1,
        },

       .spi_config_4 = {

            ._16bit_wr_cmd_msb = 0,
            .dual_flash_mode = 0,
            .secondary_csn = 1,
            .polarity_mode = 0,
            .valid_prot_bits = 4,
            .no_of_ms_dummy_bytes = 0,
            .continue_fetch_en = 0,
        },

       .spi_config_5 = {

           .busy_bit_pos = 0,
           .d7_d4_data = 0xf,
           .dummy_bytes_for_rdsr = 0x0,
           .reset_type = 0x0,
        }
    }
};

PSRAMSecureSegmentType PSRAMSecureSegments[MAX_SEC_SEGMENTS] = {
  { .segmentEnable = 1, .lowerBoundary = 0x0000, .higherBoundary = 0x0ffff },
  { .segmentEnable = 0, .lowerBoundary = 0x0000, .higherBoundary = 0x0000 },
  { .segmentEnable = 0, .lowerBoundary = 0xf0001, .higherBoundary = 0xfffff },
  { .segmentEnable = 0, .lowerBoundary = 0x0000, .higherBoundary = 0x0000 }
};