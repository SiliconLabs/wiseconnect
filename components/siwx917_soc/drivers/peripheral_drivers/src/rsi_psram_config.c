/*******************************************************************************
 * @file  rsi_psram_config.c
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

PSRAMInfoType PSRAM_Devices[MAX_SUPPORTED_PSRAM_DEVICES] = {

    /* AP Memory APS1604M_SQR PSRAM device Info*/
    {
        .devDensity = Mbit_(16),
        .deviceName = APS1604M_SQR,
        .deviceID = {.MFID = 0x0d,
                     .KGD = 0x5d,
                     .EID = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0}},
        .normalReadMAXFrequency = MHz_(33),
        .fastReadMAXFrequency = MHz_(144),
        .defaultBurstWrapSize = 1024,
        .toggleBurstWrapSize = 32,
        .spi_config = {.spi_config_1 = {.inst_mode = PSRAM_SPI_MODE,
                                        .addr_mode = QUAD_MODE,
                                        .dummy_mode = QUAD_MODE,
                                        .data_mode = QUAD_MODE,
                                        .extra_byte_mode = QUAD_MODE,
                                        .prefetch_en = DIS_PREFETCH,
                                        .dummy_W_or_R = DUMMY_READS,
                                        .d3d2_data = 3,
                                        .continuous = DIS_CONTINUOUS,
                                        .read_cmd = 0xEB,
                                        .flash_type = 255, // APS_QUAD_PSRAM
                                        .no_of_dummy_bytes = 3,
                                        .extra_byte_en = 0},

                       .spi_config_2 = {.auto_mode = EN_AUTO_MODE,
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
                                        .dummy_cycles_for_controller = 0},

                       .spi_config_3 = {.xip_mode = 0,
                                        ._16bit_cmd_valid = 0,
                                        ._16bit_rd_cmd_msb = 0,
                                        .reserved = 0,
                                        .wr_cmd = 0x38,
                                        .wr_inst_mode = PSRAM_SPI_MODE,
                                        .wr_addr_mode = QUAD_MODE,
                                        .wr_data_mode = QUAD_MODE,
                                        .dummys_4_jump = 1},

                       .spi_config_4 = {._16bit_wr_cmd_msb = 0,
                                        .dual_flash_mode = 0,
                                        .secondary_csn = 1,
                                        .polarity_mode = 0,
                                        .valid_prot_bits = 4,
                                        .no_of_ms_dummy_bytes = 0,
                                        .continue_fetch_en = 0},

                       .spi_config_5 =
                           {
                               .busy_bit_pos = 0,
                               .d7_d4_data = 0xf,
                               .dummy_bytes_for_rdsr = 0x0,
                               .reset_type = 0x0,
                           }}},
		
    /* AP Memory APS6404L_SQRH PSRAM device Info*/
    {
        .devDensity = Mbit_(64),
        .deviceName = APS6404L_SQRH,
        .deviceID = {.MFID = 0x0d,
                     .KGD = 0x5d,
                     .EID = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0}},
        .normalReadMAXFrequency = MHz_(33),
        .fastReadMAXFrequency = MHz_(84),
        .defaultBurstWrapSize = 0,
        .toggleBurstWrapSize = 0,
        .spi_config = {.spi_config_1 = {.inst_mode = PSRAM_SPI_MODE,
                                        .addr_mode = QUAD_MODE,
                                        .dummy_mode = QUAD_MODE,
                                        .data_mode = QUAD_MODE,
                                        .extra_byte_mode = QUAD_MODE,
                                        .prefetch_en = DIS_PREFETCH,
                                        .dummy_W_or_R = DUMMY_READS,
                                        .d3d2_data = 3,
                                        .continuous = DIS_CONTINUOUS,
                                        .read_cmd = 0xEB,
                                        .flash_type = 255, // APS_QUAD_PSRAM
                                        .no_of_dummy_bytes = 3,
                                        .extra_byte_en = 0},

                       .spi_config_2 = {.auto_mode = EN_AUTO_MODE,
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
                                        .dummy_cycles_for_controller = 0},

                       .spi_config_3 = {.xip_mode = 0,
                                        ._16bit_cmd_valid = 0,
                                        ._16bit_rd_cmd_msb = 0,
                                        .reserved = 0,
                                        .wr_cmd = 0x38,
                                        .wr_inst_mode = PSRAM_SPI_MODE,
                                        .wr_addr_mode = QUAD_MODE,
                                        .wr_data_mode = QUAD_MODE,
                                        .dummys_4_jump = 1},

                       .spi_config_4 = {._16bit_wr_cmd_msb = 0,
                                        .dual_flash_mode = 0,
                                        .secondary_csn = 1,
                                        .polarity_mode = 0,
                                        .valid_prot_bits = 4,
                                        .no_of_ms_dummy_bytes = 0,
                                        .continue_fetch_en = 0},

                       .spi_config_5 =
                           {
                               .busy_bit_pos = 0,
                               .d7_d4_data = 0xf,
                               .dummy_bytes_for_rdsr = 0x0,
                               .reset_type = 0x0,
                           }}},
		
    /* AP Memory APS6404L_SQH PSRAM device Info*/
    {
        .devDensity = Mbit_(64),
        .deviceName = APS6404L_SQH,
        .deviceID = {.MFID = 0x0d,
                     .KGD = 0x5d,
                     .EID = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0}},
        .normalReadMAXFrequency = MHz_(33),
        .fastReadMAXFrequency = MHz_(144),
        .defaultBurstWrapSize = 1024,
        .toggleBurstWrapSize = 0,
        .spi_config = {.spi_config_1 = {.inst_mode = PSRAM_SPI_MODE,
                                        .addr_mode = QUAD_MODE,
                                        .dummy_mode = QUAD_MODE,
                                        .data_mode = QUAD_MODE,
                                        .extra_byte_mode = QUAD_MODE,
                                        .prefetch_en = DIS_PREFETCH,
                                        .dummy_W_or_R = DUMMY_READS,
                                        .d3d2_data = 3,
                                        .continuous = DIS_CONTINUOUS,
                                        .read_cmd = 0xEB,
                                        .flash_type = 255, // APS_QUAD_PSRAM
                                        .no_of_dummy_bytes = 3,
                                        .extra_byte_en = 0},

                       .spi_config_2 = {.auto_mode = EN_AUTO_MODE,
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
                                        .dummy_cycles_for_controller = 0},

                       .spi_config_3 = {.xip_mode = 0,
                                        ._16bit_cmd_valid = 0,
                                        ._16bit_rd_cmd_msb = 0,
                                        .reserved = 0,
                                        .wr_cmd = 0x38,
                                        .wr_inst_mode = PSRAM_SPI_MODE,
                                        .wr_addr_mode = QUAD_MODE,
                                        .wr_data_mode = QUAD_MODE,
                                        .dummys_4_jump = 1},

                       .spi_config_4 = {._16bit_wr_cmd_msb = 0,
                                        .dual_flash_mode = 0,
                                        .secondary_csn = 1,
                                        .polarity_mode = 0,
                                        .valid_prot_bits = 4,
                                        .no_of_ms_dummy_bytes = 0,
                                        .continue_fetch_en = 0},

                       .spi_config_5 =
                           {
                               .busy_bit_pos = 0,
                               .d7_d4_data = 0xf,
                               .dummy_bytes_for_rdsr = 0x0,
                               .reset_type = 0x0,
                           }}},													 
		
    /* ISSI IS66WVS4M8ALL PSRAM device Info*/
    {.devDensity = Mbit_(32),
     .deviceName = IS66WVS4M8ALL,
     .deviceID = {.MFID = 0x9d,
                  .KGD = 0x5d,
                  .EID = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0}},
     .normalReadMAXFrequency = MHz_(33),
     .fastReadMAXFrequency = MHz_(104),
     .defaultBurstWrapSize = 512,
     .toggleBurstWrapSize = 32,
     .spi_config = {.spi_config_1 = {.inst_mode = QUAD_MODE,
                                     .addr_mode = QUAD_MODE,
                                     .dummy_mode = QUAD_MODE,
                                     .data_mode = QUAD_MODE,
                                     .extra_byte_mode = QUAD_MODE,
                                     .prefetch_en = DIS_PREFETCH,
                                     .dummy_W_or_R = DUMMY_READS,
                                     .d3d2_data = 3,
                                     .continuous = DIS_CONTINUOUS,
                                     .read_cmd = 0xEB,
                                     .flash_type = 255, // APS_QUAD_PSRAM
                                     .no_of_dummy_bytes = 2,
                                     .extra_byte_en = 0},

                    .spi_config_2 = {.auto_mode = EN_AUTO_MODE,
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
                                     .dummy_cycles_for_controller = 0},

                    .spi_config_3 = {.xip_mode = 0,
                                     ._16bit_cmd_valid = 0,
                                     ._16bit_rd_cmd_msb = 0,
                                     .reserved = 0,
                                     .wr_cmd = 0x38,
                                     .wr_inst_mode = QUAD_MODE,
                                     .wr_addr_mode = QUAD_MODE,
                                     .wr_data_mode = QUAD_MODE,
                                     .dummys_4_jump = 1},

                    .spi_config_4 = {._16bit_wr_cmd_msb = 0,
                                     .dual_flash_mode = 0,
                                     .secondary_csn = 1,
                                     .polarity_mode = 0,
                                     .valid_prot_bits = 4,
                                     .no_of_ms_dummy_bytes = 0,
                                     .continue_fetch_en = 0},

                    .spi_config_5 = {
                        .busy_bit_pos = 0,
                        .d7_d4_data = 0xf,
                        .dummy_bytes_for_rdsr = 0x0,
                        .reset_type = 0x0,
                    }}}};

uint16_t psram_security_aesKeySize = PSRAM_AES_KEY_SIZE;

PSRAMSecureSegmentType PSRAMSecureSegments[MAX_SEC_SEGMENTS] = {
  { .segmentEnable = 1, .lowerBoundary = 0x0000, .higherBoundary = 0x0ffff },
  { .segmentEnable = 0, .lowerBoundary = 0x0000, .higherBoundary = 0x0000 },
  { .segmentEnable = 0, .lowerBoundary = 0xf0001, .higherBoundary = 0xfffff },
  { .segmentEnable = 0, .lowerBoundary = 0x0000, .higherBoundary = 0x0000 }
};

uint32_t psram_security_aesKey[8] = { 0x11111111, 0x22222222, 0x33333333, 0x44444444,
                                      0xe774f309, 0xfcc45ece, 0xbb28c73d, 0xed8245f4 };
