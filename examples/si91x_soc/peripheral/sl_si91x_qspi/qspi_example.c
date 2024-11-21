/***************************************************************************/ /**
 * @file qspi_example.c
 * @brief QSPI examples functions
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "rsi_debug.h"
#include "qspi_example.h"
#include "rsi_qspi.h"
#include "rsi_pll.h"
#include "rsi_rom_egpio.h"
#include "rsi_rom_clks.h"
#include "rsi_rom_qspi.h"
#include "UDMA.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
// Pin Mux Macros (Pad Selection)
#define PAD_SELECTION_ENABLE_CLK  10 // Pad number for clock pin
#define PAD_SELECTION_ENABLE_D0   11 // Pad number for D0 pin
#define PAD_SELECTION_ENABLE_D1   12 // Pad number for D1 pin
#define PAD_SELECTION_ENABLE_D2   14 // Pad number for D2 pin
#define PAD_SELECTION_ENABLE_D3   15 // Pad number for D3 pin
#define PAD_SELECTION_ENABLE_CSN0 13 // Pad number for CS0 pin
// Pin Mux Macros (Pin Selection)
#define M4SS_QSPI_CLK  46 // QSPI clock pin number
#define M4SS_QSPI_D0   47 // QSPI D0 pin number
#define M4SS_QSPI_D1   48 // QSPI D1 pin number
#define M4SS_QSPI_D2   50 // QSPI D2 pin number
#define M4SS_QSPI_D3   51 // QSPI D3 pin number
#define M4SS_QSPI_CSN0 49 // QSPI CS0 pin number
// Pin Mux Macros (Pinmux Selection)
#define QSPI_MODE 11 // QSPI pinmux mode

#define FLASH_ADDRESS        0xA000000 // QSPI Flash Address
#define QSPI_BUFFER_SIZE     1024      // Transfer buffer size
#define ENCRYPTION           0         // Encryption state is disabled
#define MAX_FREQUENCY        33000000  // Maximum frequency to set the clock division factor
#define WRITE_COMMAND        0x2       // Command value for write operation
#define PAGE_SIZE            256       // Page size for write operation
#define HW_CTRL              0         // HW ctrl disabled for write operation
#define WRITE_REGISTER_DELAY 0         // Register delay (in ms) for write operation
#define CHECK_STATE          0         // Check state is disabled
#define UDMA_STATE           0         // Udma state is disabled
#define UDMA_HANDLE          0         // Udma handle is kept as 0 becuase it is not used
#define RPDMA_HANDLE         0         // Rpdma handle is kept as 0 becuase it is not used
#define READ_HSIZE           0         // Hsize for read operation
#define UDMA_READ            0         // Udma read is disabled

#define MANUAL_READ \
  ENABLE // To read data in Manual mode, enable this macro. When enabled, data from the QSPI FIFO can be accessed through I/O reads.
#define QSPI_QUAD_MODE DISABLE // Enable this macro to read/write data in Quad mode via QSPI.

/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
static volatile uint8_t qspi_read_buffer[QSPI_BUFFER_SIZE];
static volatile uint8_t qspi_write_buffer[QSPI_BUFFER_SIZE];
static volatile uint8_t qspi_read_encrypt_data[QSPI_BUFFER_SIZE];
static spi_config_t qspi_config_init;

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void set_qspi_configs(spi_config_t *spi_config);
static void set_qspi_pin_mux(void);
static void write_qspi_data(void);
static void read_qspi_data(void);
static void compare_qspi_data(void);

/*******************************************************************************
 **************************   GLOBAL VARIABLES   *******************************
 ******************************************************************************/

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/*******************************************************************************
 * QSPI example Initialization function
 * It configures the pin muxing for the QSPI interface, sets the QSPI clock using
 * the clock division factor.
 * It also sets the qspi configurations and initializes the QSPI interface.
 * Note: Before enabling the QSPI interface, it is recommended to disable all
 * the interrupts.
 * Primarily it erases the sector using QSPI interface.
 * Then it writes the data to the defined flash address and reads it back.
 * If the write and read from the flash is successful, the comparision between
 * both the buffers is successful.
 ******************************************************************************/
void qspi_example_init(void)
{
  uint16_t clock_division_factor = 0;
  uint32_t system_clock_value    = system_clocks.ulpss_ref_clk;

  // Configures the pin MUX for QSPI  pins.
  set_qspi_pin_mux();
  DEBUGOUT("QSPI Flash Pins configured successfully \n");

  // Read ID command supports max frequency of 33MHz.
  if (system_clock_value > MAX_FREQUENCY) {
    clock_division_factor = (system_clock_value / MAX_FREQUENCY);
  }

  // Clock Initialization.
  RSI_CLK_Qspi2ClkConfig(M4CLK, QSPI_ULPREFCLK, 0, 0, clock_division_factor);

  // Genrates data buffer.
  for (uint32_t i = 0; i < QSPI_BUFFER_SIZE; i++) {
    qspi_write_buffer[i] = i;
  }

  // Set the QSPI configuration structures.
  set_qspi_configs(&qspi_config_init);

  // Disable all IRQ before accessing flash.
  __disable_irq();

  // Initializes QSPI
  RSI_QSPI_SpiInit((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS, &qspi_config_init, 1, 0, 0);
  DEBUGOUT("QSPI is initialized successfully \n");

  // Erases the SECTOR
  RSI_QSPI_SpiErase((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS, &qspi_config_init, SECTOR_ERASE, FLASH_ADDRESS, 1, 0);
  DEBUGOUT("QSPI Sector is erased successfully \n");

  // Write the data to flash using QSPI.
  write_qspi_data();

  // Enable all IRQ after accessing flash.
  __enable_irq();

  // Read the data to flash using QSPI.
  read_qspi_data();

  // Compare the write data and read data.
  compare_qspi_data();
}

/*******************************************************************************
 * This function is called in while loop. 
 * Empty function.
 ******************************************************************************/
void qspi_example_process_action(void)
{
}

/*******************************************************************************
 * Writes the data to the Flash Address using QSPI interface.
 * If the Manual Read is enabled, the data will be written after performing the encryption.
 * 
 * @param none
 * @return none
 ******************************************************************************/
static void write_qspi_data(void)
{
#if defined(MANUAL_READ) && (MANUAL_READ == DISABLE)
  qspi_standalone_config_t configs;
  uint32_t status;
  // Calculationg the flash offset from flash address by anding with offset mask
  uint32_t address  = (FLASH_ADDRESS & 0x0FFFFFF);
  configs.aes_mode  = CTR_MODE;
  configs.encrypt   = ENCRYPTION;
  configs.flip_data = false;
  configs.iv        = &address;
  configs.key1      = NULL;
  configs.key2      = NULL;
  configs.key_len   = 0;
  configs.kh_enable = true;

  // Encrypting the data
  status = RSI_QSPI_Aes_Encrypt_Decrypt_Standalone((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                                                   &configs,
                                                   (uint32_t *)&qspi_write_buffer,
                                                   (uint32_t *)&qspi_read_encrypt_data,
                                                   sizeof(qspi_write_buffer));
  if (status != RSI_OK)
    DEBUGOUT("QPSI Data encryption  failed \n");

#endif

  // Writes the data to required address using qspi
  RSI_QSPI_SpiWrite((qspi_reg_t *)M4_QSPI_2_BASE_ADDRESS,
                    &qspi_config_init,
                    WRITE_COMMAND,
                    FLASH_ADDRESS,
#if defined(MANUAL_READ) && (MANUAL_READ == ENABLE)
                    (uint8_t *)&qspi_write_buffer,
#else
                    (uint8_t *)&qspi_read_encrypt_data,
#endif
                    QSPI_BUFFER_SIZE,
                    PAGE_SIZE,
                    _1BYTE,
                    HW_CTRL,
                    WRITE_REGISTER_DELAY,
                    CHECK_STATE,
                    UDMA_STATE,
                    UDMA_HANDLE,
                    RPDMA_HANDLE);
  DEBUGOUT("Data is written to the flash successfully \n ");
}

/*******************************************************************************
 * Reads the data from the Flash Address using QSPI interface.
 * If the Manual Read is enabled, the data will be read in manual mode.
 * 
 * @param none
 * @return none
 ******************************************************************************/
static void read_qspi_data(void)
{
#if defined(MANUAL_READ) && (MANUAL_READ == ENABLE)
  // Reads from the address in manual mode
  RSI_QSPI_ManualRead((qspi_reg_t *)(M4_QSPI_2_BASE_ADDRESS),
                      &qspi_config_init,
                      FLASH_ADDRESS,
                      (uint8_t *)qspi_read_buffer,
                      READ_HSIZE /*_32BIT*/,
                      sizeof(qspi_write_buffer),
                      UDMA_READ,
                      UDMA_HANDLE,
                      RPDMA_HANDLE);
  DEBUGOUT("Data is read from the flash using manual mode successfully \n ");
#else
  uint8_t *flash_address_pointer = (uint8_t *)FLASH_ADDRESS;
  for (uint32_t data = 0; data < QSPI_BUFFER_SIZE; data++) {
    qspi_read_buffer[data] = flash_address_pointer[data];
  }
  DEBUGOUT("Data is read from the flash successfully \n ");
#endif
}

/*******************************************************************************
 * Function to compare the loop back data, i.e., after transfer it will compare
 * the send and receive data
 * 
 * @param none
 * @return none
 ******************************************************************************/
static void compare_qspi_data(void)
{
  uint32_t data_index;
  for (data_index = 0; data_index < QSPI_BUFFER_SIZE; data_index++) {
    // If the write buffer and read buffer are same, it will be continued else, the for
    // loop will be break.
    if (qspi_write_buffer[data_index] != qspi_read_buffer[data_index]) {
      break;
    }
  }
  if (data_index == QSPI_BUFFER_SIZE) {
    DEBUGOUT("QSPI data comparison success \n");
  } else {
    DEBUGOUT("QSPI data comparison failed \n");
  }
}

/*******************************************************************************
 * Sets the configuration for the QSPI interface
 * 
 * @param[in] spi_config (spi_config_t type structure) Stores the QPSI configurations
 * @return none
 ******************************************************************************/
static void set_qspi_configs(spi_config_t *spi_config)
{
  memset(spi_config, 0, sizeof(spi_config_t));

  spi_config->spi_config_1.inst_mode         = SINGLE_MODE;
  spi_config->spi_config_1.addr_mode         = SINGLE_MODE;
  spi_config->spi_config_1.data_mode         = SINGLE_MODE;
  spi_config->spi_config_1.dummy_mode        = SINGLE_MODE;
  spi_config->spi_config_1.extra_byte_mode   = SINGLE_MODE;
  spi_config->spi_config_1.read_cmd          = READ;
  spi_config->spi_config_1.no_of_dummy_bytes = 0;
  spi_config->spi_config_1.prefetch_en       = DIS_PREFETCH;
  spi_config->spi_config_1.dummy_W_or_R      = DUMMY_READS;
  spi_config->spi_config_1.extra_byte_en     = 0;
  spi_config->spi_config_1.d3d2_data         = 3;
  spi_config->spi_config_1.continuous        = DIS_CONTINUOUS;
  spi_config->spi_config_1.flash_type        = MX_QUAD_FLASH;

  spi_config->spi_config_2.auto_mode                   = EN_AUTO_MODE;
  spi_config->spi_config_2.cs_no                       = CHIP_ZERO;
  spi_config->spi_config_2.neg_edge_sampling           = NEG_EDGE_SAMPLING;
  spi_config->spi_config_2.qspi_clk_en                 = QSPI_FULL_TIME_CLK;
  spi_config->spi_config_2.protection                  = DNT_REM_WR_PROT;
  spi_config->spi_config_2.dma_mode                    = NO_DMA;
  spi_config->spi_config_2.swap_en                     = SWAP;
  spi_config->spi_config_2.full_duplex                 = IGNORE_FULL_DUPLEX;
  spi_config->spi_config_2.wrap_len_in_bytes           = NO_WRAP;
  spi_config->spi_config_2.addr_width_valid            = 0;
  spi_config->spi_config_2.addr_width                  = _24BIT_ADDR;
  spi_config->spi_config_2.pinset_valid                = 0;
  spi_config->spi_config_2.flash_pinset                = GPIO_46_TO_51;
  spi_config->spi_config_2.dummy_cycles_for_controller = 0;

  spi_config->spi_config_3.wr_cmd            = WRITE_COMMAND;
  spi_config->spi_config_3.wr_inst_mode      = SINGLE_MODE;
  spi_config->spi_config_3.wr_addr_mode      = SINGLE_MODE;
  spi_config->spi_config_3.wr_data_mode      = SINGLE_MODE;
  spi_config->spi_config_3.xip_mode          = 0;
  spi_config->spi_config_3._16bit_cmd_valid  = 0;
  spi_config->spi_config_3._16bit_rd_cmd_msb = 0;
  spi_config->spi_config_3.dummys_4_jump     = 1;

  spi_config->spi_config_4._16bit_wr_cmd_msb    = 0;
  spi_config->spi_config_4.high_perf_mode_en    = 0;
  spi_config->spi_config_4.dual_flash_mode      = 0;
  spi_config->spi_config_4.secondary_csn        = 1;
  spi_config->spi_config_4.polarity_mode        = 0;
  spi_config->spi_config_4.valid_prot_bits      = 4;
  spi_config->spi_config_4.no_of_ms_dummy_bytes = 0;
  spi_config->spi_config_4.continue_fetch_en    = 0;

  spi_config->spi_config_5.block_erase_cmd      = BLOCK_ERASE;
  spi_config->spi_config_5.busy_bit_pos         = 0;
  spi_config->spi_config_5.d7_d4_data           = 0xf;
  spi_config->spi_config_5.dummy_bytes_for_rdsr = 0x0;
  spi_config->spi_config_5.reset_type           = 0x0;

  spi_config->spi_config_6.chip_erase_cmd   = CHIP_ERASE;
  spi_config->spi_config_6.sector_erase_cmd = SECTOR_ERASE;

  spi_config->spi_config_7.status_reg_write_cmd = 0x1;
  spi_config->spi_config_7.status_reg_read_cmd  = 0x5;

#if defined(QSPI_QUAD_MODE) && (QSPI_QUAD_MODE == ENABLE)
  spi_config->spi_config_1.addr_mode         = QUAD_MODE;
  spi_config->spi_config_1.data_mode         = QUAD_MODE;
  spi_config->spi_config_1.dummy_mode        = QUAD_MODE;
  spi_config->spi_config_1.extra_byte_mode   = QUAD_MODE;
  spi_config->spi_config_1.read_cmd          = FREAD_QUAD_IO;
  spi_config->spi_config_1.no_of_dummy_bytes = 2;
  spi_config->spi_config_1.extra_byte_en     = 1;

  spi_config->spi_config_3.no_of_dummy_bytes_wrap = 0;

  spi_config->spi_config_4.prot_top_bottom = 1;
#endif
}

/*******************************************************************************
 * Configures the pin-muxing for the QSPI interface.
 * Pad selection is enabled.
 * Pad receiver is configured.
 * Pin muxing is set to the QSPI mode.
 * 
 * @param none
 * @return none
 ******************************************************************************/
static void set_qspi_pin_mux(void)
{
  // Pad selection enable
  RSI_EGPIO_PadSelectionEnable(PAD_SELECTION_ENABLE_CLK);
  RSI_EGPIO_PadSelectionEnable(PAD_SELECTION_ENABLE_D0);
  RSI_EGPIO_PadSelectionEnable(PAD_SELECTION_ENABLE_D1);
  RSI_EGPIO_PadSelectionEnable(PAD_SELECTION_ENABLE_CSN0);
  RSI_EGPIO_PadSelectionEnable(PAD_SELECTION_ENABLE_D2);
  RSI_EGPIO_PadSelectionEnable(PAD_SELECTION_ENABLE_D3);

  // Receive enable for QSPI GPIO
  RSI_EGPIO_PadReceiverEnable(M4SS_QSPI_CLK);
  RSI_EGPIO_PadReceiverEnable(M4SS_QSPI_CSN0);
  RSI_EGPIO_PadReceiverEnable(M4SS_QSPI_D0);
  RSI_EGPIO_PadReceiverEnable(M4SS_QSPI_D1);
  RSI_EGPIO_PadReceiverEnable(M4SS_QSPI_D2);
  RSI_EGPIO_PadReceiverEnable(M4SS_QSPI_D3);

  // Set GPIO pin MUX for QSPI
  RSI_EGPIO_SetPinMux(EGPIO, 0, M4SS_QSPI_CLK, QSPI_MODE);
  RSI_EGPIO_SetPinMux(EGPIO, 0, M4SS_QSPI_CSN0, QSPI_MODE);
  RSI_EGPIO_SetPinMux(EGPIO, 0, M4SS_QSPI_D0, QSPI_MODE);
  RSI_EGPIO_SetPinMux(EGPIO, 0, M4SS_QSPI_D1, QSPI_MODE);
  RSI_EGPIO_SetPinMux(EGPIO, 0, M4SS_QSPI_D2, QSPI_MODE);
  RSI_EGPIO_SetPinMux(EGPIO, 0, M4SS_QSPI_D3, QSPI_MODE);
}
