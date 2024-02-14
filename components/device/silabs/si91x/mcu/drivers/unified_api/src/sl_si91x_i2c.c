/***************************************************************************/ /**
* @file  sl_si91x_i2c.c
* @brief I2C API implementation
*******************************************************************************
* # License
* <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* SPDX-License-Identifier: Zlib
*
* The licensor of this software is Silicon Laboratories Inc.
*
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software. If you use this software
*    in a product, an acknowledgment in the product documentation would be
*    appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*
******************************************************************************/
#include "sl_si91x_i2c.h"
#include "rsi_board.h"
#include "rsi_power_save.h"
#include "rsi_rom_clks.h"
#include "rsi_rom_egpio.h"
#include "rsi_rom_ulpss_clk.h"
#include "sl_si91x_dma.h"
#include "sl_si91x_peripheral_gpio.h"
#include "sl_si91x_peripheral_i2c.h"
#include "rsi_debug.h"

/*******************************************************************************
 ***************************  DEFINES / MACROS ********************************
 ******************************************************************************/
#define DMA_NUMBER                           0            // DMA number of DMA used for I2C1 and I2C0 instances
#define I2C2_DMA_NUMBER                      1            // DMA number of DMA used for I2C2 instance
#define ZERO_FLAG                            0            // Zero flag, No argument
#define PORT_ZERO                            0            // Port zero
#define HP_MAX_GPIO                          64           // High Power GPIO Maximum number
#define PINMUX_MODE                          6            // I2C pinmux mode
#define LAST_DATA_COUNT                      0            // Last read-write count
#define DATA_COUNT                           1            // Last second data count for verification
#define BIT_SET                              1            // Set bit
#define STOP_BIT                             9            // Bit to send stop command
#define MASK_READ_BIT                        8            // Bit to mask read and write
#define INTERNAL_PULLUP                      1            // Internal Pull-up enable
#define ULP_GPIO_SDA                         10           // SDA ULP GPIO Pin number
#define ULP_GPIO_SCL                         11           // SCL ULP GPIO Pin number
#define ULP_GPIO_SDA2                        6            // SDA ULP GPIO Pin number
#define ULP_GPIO_SCL2                        7            // SCL ULP GPIO Pin number
#define BUFFER_START_INDEX                   0            // Starting buffer index
#define MAX_7BIT_ADDRESS                     127          // Maximum 7-bit address
#define SIX                                  6            // Value 25
#define ZERO                                 0            // Value 0
#define ONE                                  1            // Value 1
#define TWO                                  2            // Value 2
#define SET                                  1            // to set a bit
#define CLEAR                                0            // to clear a bit
#define ACK_BYTE                             0x00000100   // to write ack
#define FOLLOWER_ADDR_10BIT_MAX              0x3FF        // maximum value for 10-bit address
#define REFERENCE_CLOCK_FREQUENCY            (32000000u)  // Reference clock frequency
#define HIGH_SPEED_REFERENCE_CLOCK_FREQUENCY (40000000u)  // Reference clock frequency
#define I2C_STANDARD_MODE_CLOCK_FREQUENCY    (32000000u)  // clock frequency for i2c standard mode
#define I2C_FAST_MODE_CLOCK_FREQUENCY        (32000000u)  // clock frequency for i2c fast mode
#define I2C_FAST_PLUS_MODE_CLOCK_FREQUENCY   (80000000u)  // clock frequency for i2c fast plus mode
#define I2C_HIGH_SPEED_MODE_CLOCK_FREQUENCY  (180000000u) // clock frequency for i2c high speed mode
#define ULP_PORT                             4            // GPIO ULP port
#define ULP_MODE                             6            // ULP GPIO mode
#define MAX_GPIO                             64           // maximum GPIO pins
#define OUTPUT                               1            // Output value set
#define HOST_MIN                             24           // GPIO host pad minimum pin number
#define HOST_MAX                             31           // GPIO host pad maximum pin number
#define I2C2_MUX                             4            // I2C2 MUX number
#define I2C2_PAD                             0            // I2C2 PAD number
#define ULP_CLOCK_DIV_FACTOR                 0            // division factor value for ULP clock
#define ULP_PRO_CLOCK_DIV_FACTOR             0            // division factor value for ULP pro clock
#define EVEN_DIVISION_FACTOR                 0            // ulp clock division factor type
#define DELAY_DISABLE                        0            // to disable delay function callback for ulp pro clock

/*******************************************************************************
 ***************************  Local TYPES  ********************************
 ******************************************************************************/
// @brief Structure to hold the parameters of i2c instance
typedef struct {
  sl_i2c_instance_t instance;             // i2c instance
  sl_i2c_mode_t mode;                     // Leader/Follower Mode
  sl_i2c_operating_mode_t operating_mode; // Operating mode
  sl_i2c_transfer_type_t transfer_type;   // Transfer type
  uint8_t *write_buffer;                  // write-buffer
  uint8_t *read_buffer;                   // read-buffer
  uint32_t write_buffer_length;           // write-buffer length
  uint32_t read_buffer_length;            // read-buffer length
  uint32_t write_buffer_current_index;    // write buffer current index
  uint32_t read_buffer_current_index;     // read buffer current index
} sl_i2c_instance_state_t;

/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
static sl_i2c_callback_t i2c_callback_function_ptr[] = { NULL, NULL, NULL };
// @note: For multiple threads user should protect following i2c-state structure
// variables
static sl_i2c_instance_state_t i2c0_instance;
static sl_i2c_instance_state_t i2c1_instance;
static sl_i2c_instance_state_t i2c2_instance;
uint8_t i2c_read_ack = ZERO;
static uint32_t *i2c_tx_last_byte_addr;
uint32_t write_ack[1] = { ACK_BYTE };

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/
static void i2c_clock_init(I2C_TypeDef *i2c);
static void i2c_clock_deinit(I2C_TypeDef *i2c);
static void sl_i2c_follower_transmit_handler(I2C_TypeDef *i2c);
static void sl_i2c_leader_transmit_handler(I2C_TypeDef *i2c);
static void sl_i2c_follower_receive_handler(I2C_TypeDef *i2c);
static void sl_i2c_leader_receive_handler(I2C_TypeDef *i2c);
static void i2c_dma_transfer_complete_callback(uint32_t channel, void *data);
static void i2c_dma_error_callback(uint32_t channel, void *data);
static void *i2c_addr(sl_i2c_instance_t i2c_instance);
static void i2c_handler(I2C_TypeDef *i2c);
static void wait_for_i2c_follower_ready(I2C_TypeDef *i2c);
static void wait_till_i2c_gets_idle(I2C_TypeDef *i2c);
static void i2c_dma_rx_config(I2C_TypeDef *i2c);
static void i2c_dma_tx_config(I2C_TypeDef *i2c);

/*******************************************************************************
 ***********************  Global function Definitions *************************
 ******************************************************************************/

/*****************************************************************************
 * This function initializes the I2C Module and clock. Sets I2C instance mode,
 * operating mode(bus-speed), frequency and transfer type (using Interrupt or
 *DMA). If transfer type is DMA then it initializes DMA also. Registers I2C
 *instance callback and clears pending interrupts. Configures SDL and SCL pins
 *as per instance Updates instance state structure values
 *******************************************************************************/
sl_i2c_status_t sl_i2c_driver_init(sl_i2c_instance_t i2c_instance, const sl_i2c_config_t *p_user_config)
{
  sl_i2c_status_t i2c_status;
  I2C0_Type *i2c;
  sl_i2c_init_params_t config;
  i2c_status = SL_I2C_SUCCESS;
  do {
    // To validate the structure pointer, if the parameters is NULL, it
    // will return an error code, also validating members of i2c-config
    // structure.
    if ((p_user_config == NULL) || (i2c_instance >= SL_I2C_LAST) || (p_user_config->mode > SL_I2C_FOLLOWER_MODE)
        || (p_user_config->operating_mode >= SL_I2C_OPERATING_MODE_LAST)
        || (p_user_config->transfer_type >= SL_I2C_TRANFER_TYPE_LAST)) {
      i2c_status = SL_I2C_INVALID_PARAMETER;
      break;
    }
    // Checking if callback already registered
    if (i2c_callback_function_ptr[i2c_instance] != NULL) {
      i2c_status = SL_I2C_CALLBACK_BUSY;
      break;
    }
    // Updating i2c structure as per inputs
    config.clhr = (sl_i2c_clock_hlr_t)(p_user_config->operating_mode);
    config.mode = p_user_config->mode;
    // Updating members of respective instance structure variables
    if (i2c_instance == SL_I2C0) {
      i2c                          = ((I2C0_Type *)I2C0_BASE);
      i2c0_instance.instance       = SL_I2C0;
      i2c0_instance.operating_mode = (sl_i2c_operating_mode_t)config.clhr;
      i2c0_instance.transfer_type  = p_user_config->transfer_type;
      if (config.mode == SL_I2C_LEADER_MODE) {
        i2c0_instance.mode = SL_I2C_LEADER_MODE;
      } else {
        i2c0_instance.mode = SL_I2C_FOLLOWER_MODE;
      }
    }
    // Updating members of respective instance structure variables
    if (i2c_instance == SL_I2C1) {
      i2c                          = ((I2C0_Type *)I2C1_BASE);
      i2c1_instance.instance       = SL_I2C1;
      i2c1_instance.operating_mode = (sl_i2c_operating_mode_t)config.clhr;
      i2c1_instance.transfer_type  = p_user_config->transfer_type;
      if (config.mode == SL_I2C_LEADER_MODE) {
        i2c1_instance.mode = SL_I2C_LEADER_MODE;
      } else {
        i2c1_instance.mode = SL_I2C_FOLLOWER_MODE;
      }
    }
    // Updating members of respective instance structure variables
    if (i2c_instance == SL_I2C2) {
      i2c                          = ((I2C0_Type *)I2C2_BASE);
      i2c2_instance.instance       = SL_I2C2;
      i2c2_instance.operating_mode = (sl_i2c_operating_mode_t)config.clhr;
      i2c2_instance.transfer_type  = p_user_config->transfer_type;
      if (config.mode == SL_I2C_LEADER_MODE) {
        i2c2_instance.mode = SL_I2C_LEADER_MODE;
      } else {
        i2c2_instance.mode = SL_I2C_FOLLOWER_MODE;
      }
    }
    // Initializing I2c clock
    i2c_clock_init(i2c);
    // Default keep M4 in reference clock
    // Configure the PLL frequency as per operating mode
    if (p_user_config->operating_mode == SL_I2C_STANDARD_MODE) {
      RSI_CLK_M4SocClkConfig(M4CLK, M4_ULPREFCLK, 0);
      RSI_CLK_SetSocPllFreq(M4CLK, I2C_STANDARD_MODE_CLOCK_FREQUENCY, REFERENCE_CLOCK_FREQUENCY);
      RSI_CLK_M4SocClkConfig(M4CLK, M4_SOCPLLCLK, 0);
      config.freq = I2C_STANDARD_MODE_CLOCK_FREQUENCY;
    }
    if (p_user_config->operating_mode == SL_I2C_FAST_MODE) {
      RSI_CLK_M4SocClkConfig(M4CLK, M4_ULPREFCLK, 0);
      RSI_CLK_SetSocPllFreq(M4CLK, I2C_FAST_MODE_CLOCK_FREQUENCY, REFERENCE_CLOCK_FREQUENCY);
      RSI_CLK_M4SocClkConfig(M4CLK, M4_SOCPLLCLK, 0);
      config.freq = I2C_FAST_MODE_CLOCK_FREQUENCY;
    }
    if (p_user_config->operating_mode == SL_I2C_FAST_PLUS_MODE) {
      RSI_CLK_M4SocClkConfig(M4CLK, M4_ULPREFCLK, 0);
      RSI_CLK_SetSocPllFreq(M4CLK, I2C_FAST_PLUS_MODE_CLOCK_FREQUENCY, REFERENCE_CLOCK_FREQUENCY);
      RSI_CLK_M4SocClkConfig(M4CLK, M4_SOCPLLCLK, 0);
      if (i2c_instance == SL_I2C2) {
        RSI_ULPSS_ClockConfig(M4CLK, ENABLE, ULP_CLOCK_DIV_FACTOR, EVEN_DIVISION_FACTOR);
        RSI_ULPSS_UlpProcClkConfig(ULPCLK, ULP_PROC_SOC_CLK, ULP_PRO_CLOCK_DIV_FACTOR, DELAY_DISABLE);
        DEBUGINIT();
      }
      config.freq = sl_si91x_i2c_get_frequency(i2c);
    }
    if ((p_user_config->operating_mode == SL_I2C_HIGH_SPEED_MODE)) {
      RSI_CLK_M4SocClkConfig(M4CLK, M4_ULPREFCLK, 0);
      RSI_CLK_SetSocPllFreq(M4CLK, I2C_HIGH_SPEED_MODE_CLOCK_FREQUENCY, HIGH_SPEED_REFERENCE_CLOCK_FREQUENCY);
      RSI_CLK_M4SocClkConfig(M4CLK, M4_SOCPLLCLK, 0);
      if (i2c_instance == SL_I2C2) {
        RSI_ULPSS_ClockConfig(M4CLK, ENABLE, ULP_CLOCK_DIV_FACTOR, EVEN_DIVISION_FACTOR);
        RSI_ULPSS_UlpProcClkConfig(ULPCLK, ULP_PROC_SOC_CLK, ULP_PRO_CLOCK_DIV_FACTOR, DELAY_DISABLE);
        DEBUGINIT();
      }
      config.freq = sl_si91x_i2c_get_frequency(i2c);
    }
    // Registering callback as per transfer type
    i2c_callback_function_ptr[i2c_instance] = p_user_config->i2c_callback;
    // Initializing I2C parameters
    sl_si91x_i2c_init(i2c, &config);
    // Checking tranfer type
    if (p_user_config->transfer_type == SL_I2C_USING_DMA) {
      // DMA initializtion
      sl_dma_init_t dma_init;
      if (i2c_instance == SL_I2C2) {
        dma_init.dma_number = I2C2_DMA_NUMBER;
      } else {
        dma_init.dma_number = DMA_NUMBER;
      }
      if (sl_si91x_dma_init(&dma_init)) {
        // return ARM_DRIVER_ERROR;
        i2c_status = SL_I2C_INVALID_PARAMETER;
        break;
      }
    }
  } while (false);
  return i2c_status;
}

/*****************************************************************************
 * This API configures the follower address of the I2C module.
 * Should be used only in Follower mode, after init call.
 ******************************************************************************/
sl_i2c_status_t sl_i2c_driver_set_follower_address(sl_i2c_instance_t i2c_instance, uint16_t follower_address)
{
  sl_i2c_status_t i2c_status;
  I2C0_Type *i2c;
  i2c_status = SL_I2C_SUCCESS;

  do {
    if ((i2c_instance >= SL_I2C_LAST) || (follower_address > FOLLOWER_ADDR_10BIT_MAX)) {
      i2c_status = SL_I2C_INVALID_PARAMETER;
      break;
    }
    // Updating pointer to i2c register block as per instance number
    i2c = (I2C0_Type *)i2c_addr(i2c_instance);
    // Disabling I2C instance
    sl_si91x_i2c_disable(i2c);
    // Setting the follower mask address, here leader behaves like follower.
    sl_si91x_i2c_set_follower_mask_address(i2c, follower_address);
    // Enabling I2C
    sl_si91x_i2c_enable(i2c);
  } while (false);
  return i2c_status;
}

/*****************************************************************************
 * This function is used to configure the TX and RX FIFO threshold values
 * The valid range is 0-255, with the additional restriction that hardware does
 * not allow this value to be set to a value larger than the depth of the
 *buffer. A value of 0 sets the threshold for 1 entry, and a value of 255 sets
 *the threshold for 256 entries. Should be called after init API.
 ******************************************************************************/
sl_i2c_status_t sl_i2c_driver_configure_fifo_threshold(sl_i2c_instance_t i2c_instance,
                                                       uint8_t tx_threshold_value,
                                                       uint8_t rx_threshold_value)
{
  sl_i2c_status_t i2c_status;
  I2C0_Type *i2c;
  i2c_status = SL_I2C_SUCCESS;
  do {
    // Validating I2C instance
    if (i2c_instance >= SL_I2C_LAST) {
      i2c_status = SL_I2C_INVALID_PARAMETER;
      break;
    }
    // Updating pointer to i2c register block as per instance number
    i2c = (I2C0_Type *)i2c_addr(i2c_instance);
    // Disabling I2C instance
    sl_si91x_i2c_disable(i2c);
    // Setting tx FIFO threshold
    sl_si91x_i2c_set_tx_threshold(i2c, tx_threshold_value);
    // Setting rx FIFO threshold
    sl_si91x_i2c_set_rx_threshold(i2c, rx_threshold_value);
    // Enabling I2C instance
    sl_si91x_i2c_enable(i2c);
  } while (false);
  return i2c_status;
}

/*******************************************************************************
 * This API gets the current frequency of I2C transfer in MHz,
 * by reading system core clock frequency
 ******************************************************************************/
sl_i2c_status_t sl_i2c_driver_get_frequency(sl_i2c_instance_t i2c_instance, uint32_t *frequency)
{
  sl_i2c_status_t i2c_status;
  I2C0_Type *i2c;
  i2c_status = SL_I2C_SUCCESS;
  do {
    // Validating I2C instance number and NULL pointer
    if ((frequency == NULL) || (i2c_instance >= SL_I2C_LAST)) {
      i2c_status = SL_I2C_INVALID_PARAMETER;
      break;
    }
    // Updating i2c pointer as per instance number
    i2c = (I2C0_Type *)i2c_addr(i2c_instance);
    // Reading System core clock frequency
    *frequency = sl_si91x_i2c_get_frequency(i2c);
  } while (false);
  return i2c_status;
}
/*****************************************************************************
 * This function sends the data in blocking mode (using interrupt)
 * Also sets follower address when used in Leader application.
 * Sets control direction.
 * Sets transmit empty interrupt and enable I2C interrupts
 ******************************************************************************/
sl_i2c_status_t sl_i2c_driver_send_data_blocking(sl_i2c_instance_t i2c_instance,
                                                 uint16_t address,
                                                 uint8_t *tx_buffer,
                                                 uint32_t tx_len)
{
  I2C0_Type *i2c;
  sl_i2c_status_t i2c_status;
  i2c_status         = SL_I2C_SUCCESS;
  bool is_10bit_addr = false;
  do {
    // To validate the structure pointer, if the parameters is NULL, it
    // will return an error code, also validating I2C instance number.
    if ((tx_buffer == NULL) || (i2c_instance >= SL_I2C_LAST) || (address > FOLLOWER_ADDR_10BIT_MAX)) {
      i2c_status = SL_I2C_INVALID_PARAMETER;
      break;
    }
    // Updating i2c pointer as per instance number
    i2c = (I2C0_Type *)i2c_addr(i2c_instance);
    // Checking is address is 7-bit or 10bit
    if (address > MAX_7BIT_ADDRESS) {
      is_10bit_addr = true;
    }
    // Disables the interrupts.
    sl_si91x_i2c_disable_interrupts(i2c, ZERO_FLAG);
    // Disables the I2C peripheral.
    sl_si91x_i2c_disable(i2c);
    if ((i2c_instance == SL_I2C0) && (i2c0_instance.mode == SL_I2C_LEADER_MODE)) {
      // Setting the follower address recevied in parameter structure.
      sl_si91x_i2c_set_follower_address(i2c, address, is_10bit_addr);
    }
    if ((i2c_instance == SL_I2C1) && (i2c1_instance.mode == SL_I2C_LEADER_MODE)) {
      // Setting the follower address recevied in parameter structure.
      sl_si91x_i2c_set_follower_address(i2c, address, is_10bit_addr);
    }
    if ((i2c_instance == SL_I2C2) && (i2c2_instance.mode == SL_I2C_LEADER_MODE)) {
      // Setting the follower address recevied in parameter structure.
      sl_si91x_i2c_set_follower_address(i2c, address, is_10bit_addr);
    }
    // Updates the instance variables which are required for transmission as per
    // instance
    if (i2c_instance == SL_I2C0) {
      i2c0_instance.write_buffer               = (uint8_t *)tx_buffer;
      i2c0_instance.write_buffer_current_index = BUFFER_START_INDEX;
      i2c0_instance.write_buffer_length        = tx_len;
    }
    if (i2c_instance == SL_I2C1) {
      i2c1_instance.write_buffer               = (uint8_t *)tx_buffer;
      i2c1_instance.write_buffer_current_index = BUFFER_START_INDEX;
      i2c1_instance.write_buffer_length        = tx_len;
    }
    if (i2c_instance == SL_I2C2) {
      i2c2_instance.write_buffer               = (uint8_t *)tx_buffer;
      i2c2_instance.write_buffer_current_index = BUFFER_START_INDEX;
      i2c2_instance.write_buffer_length        = tx_len;
    }
    // Enables the I2C peripheral.
    sl_si91x_i2c_enable(i2c);
    // Configures the transmit empty interrupt.
    sl_si91x_i2c_set_interrupts(i2c, SL_I2C_EVENT_TRANSMIT_EMPTY);
    // Enables the interrupt.
    sl_si91x_i2c_enable_interrupts(i2c, ZERO_FLAG);

  } while (false);
  return i2c_status;
}

/*****************************************************************************
 * This function receives the data in blocking mode (using interrupt),
 * also sets follower address when used in Leader application.
 * Sets receive full interrupt, while receiving data
 * Sets transmit empty interrupt, while sending data
 ******************************************************************************/
sl_i2c_status_t sl_i2c_driver_receive_data_blocking(sl_i2c_instance_t i2c_instance,
                                                    uint16_t address,
                                                    uint8_t *rx_buffer,
                                                    uint32_t rx_len)
{
  I2C0_Type *i2c;
  sl_i2c_status_t i2c_status;
  i2c_status         = SL_I2C_SUCCESS;
  bool is_10bit_addr = false;
  do {
    // To validate the structure pointer, if the parameters is NULL, it
    // will return an error code, also validating I2C instance number.
    if ((rx_buffer == NULL) || (i2c_instance >= SL_I2C_LAST) || (address > FOLLOWER_ADDR_10BIT_MAX)) {
      i2c_status = SL_I2C_INVALID_PARAMETER;
      break;
    }
    // Updating i2c pointer as per instance number
    i2c = (I2C0_Type *)i2c_addr(i2c_instance);
    // Checking is address is 7-bit or 10bit
    if (address > MAX_7BIT_ADDRESS) {
      is_10bit_addr = true;
    }
    // Disables the interrupts.
    sl_si91x_i2c_disable_interrupts(i2c, ZERO_FLAG);
    // Disables the I2C peripheral.
    sl_si91x_i2c_disable(i2c);
    // Updates the variables which are required for trasmission.
    if (i2c_instance == SL_I2C0) {
      i2c0_instance.read_buffer               = (uint8_t *)rx_buffer;
      i2c0_instance.read_buffer_current_index = BUFFER_START_INDEX;
      i2c0_instance.read_buffer_length        = rx_len;
    }
    if (i2c_instance == SL_I2C1) {
      i2c1_instance.read_buffer               = (uint8_t *)rx_buffer;
      i2c1_instance.read_buffer_current_index = BUFFER_START_INDEX;
      i2c1_instance.read_buffer_length        = rx_len;
    }
    if (i2c_instance == SL_I2C2) {
      i2c2_instance.read_buffer               = (uint8_t *)rx_buffer;
      i2c2_instance.read_buffer_current_index = BUFFER_START_INDEX;
      i2c2_instance.read_buffer_length        = rx_len;
    }
    if ((i2c_instance == SL_I2C0) && (i2c0_instance.mode == SL_I2C_LEADER_MODE)) {
      // Setting the follower address recevied in parameter structure.
      sl_si91x_i2c_set_follower_address(i2c, address, is_10bit_addr);
    }
    if ((i2c_instance == SL_I2C1) && (i2c1_instance.mode == SL_I2C_LEADER_MODE)) {
      // Setting the follower address recevied in parameter structure.
      sl_si91x_i2c_set_follower_address(i2c, address, is_10bit_addr);
    }
    if ((i2c_instance == SL_I2C2) && (i2c2_instance.mode == SL_I2C_LEADER_MODE)) {
      // Setting the follower address recevied in parameter structure.
      sl_si91x_i2c_set_follower_address(i2c, address, is_10bit_addr);
    }
    // Enables the I2C peripheral.
    sl_si91x_i2c_enable(i2c);
    // Sets the direction to read.
    sl_si91x_i2c_control_direction(i2c, SL_I2C_READ_MASK);
    // Clearing all interrupts
    uint32_t clear = i2c->IC_CLR_INTR;
    // Enables the interrupt.
    sl_si91x_i2c_enable_interrupts(i2c, ZERO_FLAG);
    // Configures the receive full interrupt.
    sl_si91x_i2c_set_interrupts(i2c, SL_I2C_EVENT_RECEIVE_FULL);
    // wait till follower gets ready after getting read request
    if (i2c0_instance.mode || i2c1_instance.mode || i2c2_instance.mode) {
      wait_for_i2c_follower_ready(i2c);
    }
    // to avoid unused variable warning
    (void)clear;
  } while (false);
  return i2c_status;
}

/*****************************************************************************
 * This function sends the data in non blocking mode (using DMA),
 * also sets follower address when used in Leader application.
 * Configures DMA rx and tx channels.
 * Registers DMA callback & allocates channel
 ******************************************************************************/
sl_i2c_status_t sl_i2c_driver_send_data_non_blocking(sl_i2c_instance_t i2c_instance,
                                                     uint16_t address,
                                                     uint32_t *tx_buffer,
                                                     uint32_t tx_len,
                                                     sl_i2c_dma_config_t *p_dma_config)
{
  sl_status_t status = SL_STATUS_OK;
  sl_i2c_status_t i2c_status;
  i2c_status = SL_I2C_SUCCESS;
  I2C0_Type *i2c;
  bool is_10bit_addr        = false;
  bool leader_mode          = false;
  uint32_t channel          = ZERO;
  uint32_t channel_priority = ZERO;
  uint32_t dma_number       = DMA_NUMBER;
  sl_dma_callback_t i2c_tx_callback;
  do {
    // Validating for NULL pointers and I2C instance number
    if ((tx_buffer == NULL) || (p_dma_config == NULL) || (i2c_instance >= SL_I2C_LAST)
        || (address > FOLLOWER_ADDR_10BIT_MAX)) {
      i2c_status = SL_I2C_INVALID_PARAMETER;
      break;
    }
    // Updating i2c pointer as per instance number
    i2c = (I2C0_Type *)i2c_addr(i2c_instance);
    // Checking is address is 7-bit or 10bit
    if (address > MAX_7BIT_ADDRESS) {
      is_10bit_addr = true;
    }
    // Disables the I2C peripheral.
    sl_si91x_i2c_disable(i2c);
    sl_si91x_i2c_disable_interrupts(i2c, ZERO_FLAG);
    if ((i2c_instance == SL_I2C0) && (i2c0_instance.mode == SL_I2C_LEADER_MODE)) {
      // Setting the follower address recevied in parameter structure.
      sl_si91x_i2c_set_follower_address(i2c, address, is_10bit_addr);
      leader_mode = true;
    }
    if ((i2c_instance == SL_I2C1) && (i2c1_instance.mode == SL_I2C_LEADER_MODE)) {
      // Setting the follower address recevied in parameter structure.
      sl_si91x_i2c_set_follower_address(i2c, address, is_10bit_addr);
      leader_mode = true;
    }
    if ((i2c_instance == SL_I2C2) && (i2c2_instance.mode == SL_I2C_LEADER_MODE)) {
      // Setting the follower address recevied in parameter structure.
      sl_si91x_i2c_set_follower_address(i2c, address, is_10bit_addr);
      leader_mode = true;
    }
    // Add stop bit to last data byte
    tx_buffer[tx_len - ONE] |= (BIT_SET << STOP_BIT);
    i2c_tx_last_byte_addr = (uint32_t *)(tx_buffer + (tx_len - ONE));
    // Enabling  transmit FIFO DMA channel and setting transmit data Level
    i2c_dma_tx_config(i2c);
    // Enables the I2C peripheral.
    sl_si91x_i2c_enable(i2c);
    sl_dma_xfer_t dma_transfer_tx = { ZERO };
    channel                       = p_dma_config->dma_tx_channel + ONE;
    channel_priority              = ONE;
    // Initialize sl_dma callback structure
    i2c_tx_callback.transfer_complete_cb = i2c_dma_transfer_complete_callback;
    i2c_tx_callback.error_cb             = i2c_dma_error_callback;
    // Initialize sl_dma transfer structure-
    dma_transfer_tx.src_addr  = (uint32_t *)((uint32_t)(tx_buffer));
    dma_transfer_tx.dest_addr = (uint32_t *)((uint32_t) & (i2c->IC_DATA_CMD));
    dma_transfer_tx.src_inc   = SL_TRANSFER_SRC_INC_32;
    dma_transfer_tx.dst_inc   = SL_TRANSFER_DST_INC_NONE;
    dma_transfer_tx.xfer_size = SL_TRANSFER_SIZE_32;
    if (leader_mode) {
      if (i2c_instance == SL_I2C2) {
        dma_transfer_tx.transfer_count = tx_len;
      } else {
        dma_transfer_tx.transfer_count = tx_len + ONE;
      }
    } else {
      dma_transfer_tx.transfer_count = tx_len;
    }
    dma_transfer_tx.transfer_type = SL_DMA_MEMORY_TO_PERIPHERAL;
    dma_transfer_tx.dma_mode      = SL_DMA_BASIC_MODE;
    dma_transfer_tx.signal        = SL_I2C_ACK;
    // Updating DMA number for I2C2 instance
    if (i2c_instance == SL_I2C2) {
      dma_number = I2C2_DMA_NUMBER;
    }
    // Allocate DMA channel for Tx
    status = sl_si91x_dma_allocate_channel(dma_number, &channel, channel_priority);
    // Allocate DMA channel for Tx
    if ((status != SL_STATUS_OK) && (status != SL_STATUS_DMA_CHANNEL_ALLOCATED)) {
      i2c_status = SL_I2C_DMA_TRANSFER_ERROR;
      break;
    }
    // Register transfer complete and error callback
    if (sl_si91x_dma_register_callbacks(dma_number, channel, &i2c_tx_callback)) {
      i2c_status = SL_I2C_DMA_TRANSFER_ERROR;
      break;
    }
    // Configure the channel for DMA transfer
    if (sl_si91x_dma_transfer(dma_number, channel, &dma_transfer_tx)) {
      i2c_status = SL_I2C_DMA_TRANSFER_ERROR;
      break;
    }
    sl_si91x_dma_channel_enable(dma_number, p_dma_config->dma_tx_channel + ONE);
    sl_si91x_dma_enable(dma_number);
  } while (false);
  return i2c_status;
}

/*****************************************************************************
 * This function receives the data in non blocking mode (using DMA),
 * also sets follower address when used in Leader application.
 * Configures DMA rx and tx channels.
 ******************************************************************************/
sl_i2c_status_t sl_i2c_driver_receive_data_non_blocking(sl_i2c_instance_t i2c_instance,
                                                        uint16_t address,
                                                        uint8_t *rx_buffer,
                                                        uint32_t rx_len,
                                                        sl_i2c_dma_config_t *p_dma_config)
{
  sl_status_t status = SL_STATUS_OK;
  I2C0_Type *i2c;
  sl_i2c_status_t i2c_status;
  i2c_status                = SL_I2C_SUCCESS;
  bool is_10bit_addr        = false;
  bool leader_mode          = false;
  uint32_t channel          = ZERO;
  uint32_t channel_priority = ZERO;
  uint32_t dma_number       = DMA_NUMBER;
  sl_dma_callback_t i2c_rx_callback;
  do {
    // Validating for NULL pointers and I2C instance number
    if ((rx_buffer == NULL) || (p_dma_config == NULL) || (i2c_instance >= SL_I2C_LAST)
        || (address > FOLLOWER_ADDR_10BIT_MAX)) {
      i2c_status = SL_I2C_INVALID_PARAMETER;
      break;
    }
    // Updating i2c pointer as per instance number
    i2c = (I2C0_Type *)i2c_addr(i2c_instance);
    // Checking is address is 7-bit or 10bit
    if (address > MAX_7BIT_ADDRESS) {
      is_10bit_addr = true;
    }
    // Disables the I2C peripheral.
    sl_si91x_i2c_disable(i2c);
    sl_si91x_i2c_disable_interrupts(i2c, ZERO_FLAG);
    if ((i2c_instance == SL_I2C0) && (i2c0_instance.mode == SL_I2C_LEADER_MODE)) {
      // Setting the follower address received in parameter structure.
      sl_si91x_i2c_set_follower_address(i2c, address, is_10bit_addr);
      leader_mode = true;
    }
    if ((i2c_instance == SL_I2C1) && (i2c1_instance.mode == SL_I2C_LEADER_MODE)) {
      // Setting the follower address received in parameter structure.
      sl_si91x_i2c_set_follower_address(i2c, address, is_10bit_addr);
      leader_mode = true;
    }
    if ((i2c_instance == SL_I2C2) && (i2c2_instance.mode == SL_I2C_LEADER_MODE)) {
      // Setting the follower address received in parameter structure.
      sl_si91x_i2c_set_follower_address(i2c, address, is_10bit_addr);
      leader_mode = true;
    }
    // Enabling  receive FIFO DMA channel and setting receive data Level
    i2c_dma_rx_config(i2c);
    // Enables the I2C peripheral.
    sl_si91x_i2c_enable(i2c);
    sl_dma_xfer_t dma_transfer_rx = { ZERO };
    channel                       = p_dma_config->dma_rx_channel + ONE;
    channel_priority              = ONE;
    // Initialize sl_dma callback structure
    i2c_rx_callback.transfer_complete_cb = i2c_dma_transfer_complete_callback;
    i2c_rx_callback.error_cb             = i2c_dma_error_callback;
    // Initialize sl_dma transfer structure
    dma_transfer_rx.src_addr       = (uint32_t *)((uint32_t) & (i2c->IC_DATA_CMD));
    dma_transfer_rx.dest_addr      = (uint32_t *)((uint32_t)(rx_buffer));
    dma_transfer_rx.src_inc        = SL_TRANSFER_SRC_INC_NONE;
    dma_transfer_rx.dst_inc        = SL_TRANSFER_DST_INC_8;
    dma_transfer_rx.xfer_size      = SL_TRANSFER_SIZE_8;
    dma_transfer_rx.transfer_count = rx_len;
    dma_transfer_rx.transfer_type  = SL_DMA_PERIPHERAL_TO_MEMORY;
    dma_transfer_rx.dma_mode       = SL_DMA_BASIC_MODE;
    dma_transfer_rx.signal         = SL_I2C_ACK;
    // Updating DMA number for I2C2 instance
    if (i2c_instance == SL_I2C2) {
      dma_number = I2C2_DMA_NUMBER;
    }
    status = sl_si91x_dma_allocate_channel(dma_number, &channel, channel_priority);
    // Allocate DMA channel for Tx
    if ((status != SL_STATUS_OK) && (status != SL_STATUS_DMA_CHANNEL_ALLOCATED)) {
      i2c_status = SL_I2C_DMA_TRANSFER_ERROR;
      break;
    }
    // Register transfer complete and error callback
    if (sl_si91x_dma_register_callbacks(dma_number, channel, &i2c_rx_callback)) {
      i2c_status = SL_I2C_DMA_TRANSFER_ERROR;
      break;
    }
    // Configure the channel for DMA transfer
    if (sl_si91x_dma_transfer(dma_number, channel, &dma_transfer_rx)) {
      i2c_status = SL_I2C_DMA_TRANSFER_ERROR;
      break;
    }
    sl_si91x_dma_channel_enable(dma_number, p_dma_config->dma_rx_channel + ONE);
    sl_si91x_dma_enable(dma_number);
    if (leader_mode == false) {
      // wait till follower gets ready after getting read request
      wait_for_i2c_follower_ready(i2c);
    }
    if (leader_mode) {
      // Enabling  transmit FIFO DMA channel and setting transmit data Level
      i2c_dma_tx_config(i2c);
      sl_dma_xfer_t dma_transfer_tx = { ZERO };
      channel                       = p_dma_config->dma_tx_channel + ONE;
      channel_priority              = 0;
      // Initialize sl_dma transfer structure
      dma_transfer_tx.src_addr       = (uint32_t *)((uint32_t)(write_ack));
      dma_transfer_tx.dest_addr      = (uint32_t *)((uint32_t) & (i2c->IC_DATA_CMD));
      dma_transfer_tx.src_inc        = SL_TRANSFER_SRC_INC_NONE;
      dma_transfer_tx.dst_inc        = SL_TRANSFER_DST_INC_NONE;
      dma_transfer_tx.xfer_size      = SL_TRANSFER_SIZE_32;
      dma_transfer_tx.transfer_count = rx_len + TWO;
      dma_transfer_tx.transfer_type  = SL_DMA_MEMORY_TO_PERIPHERAL;
      dma_transfer_tx.dma_mode       = SL_DMA_BASIC_MODE;
      dma_transfer_tx.signal         = SL_I2C_ACK;
      // Allocate DMA channel for Tx
      status = sl_si91x_dma_allocate_channel(dma_number, &channel, channel_priority);
      // Allocate DMA channel for Tx
      if ((status != SL_STATUS_OK) && (status != SL_STATUS_DMA_CHANNEL_ALLOCATED)) {
        i2c_status = SL_I2C_DMA_TRANSFER_ERROR;
        break;
      }
      // Configure the channel for DMA transfer
      if (sl_si91x_dma_transfer(dma_number, channel, &dma_transfer_tx)) {
        i2c_status = SL_I2C_DMA_TRANSFER_ERROR;
        break;
      }
      i2c_read_ack = SET;
      sl_si91x_dma_channel_enable(dma_number, p_dma_config->dma_tx_channel + ONE);
    }
  } while (false);
  return i2c_status;
}

/*****************************************************************************
 * This function first transmits data and then receives data, in blocking mode
 * also sets follower address when used in Leader application.
 * Sets receive-full interrupt and enable I2C interrupts
 ******************************************************************************/
sl_i2c_status_t sl_i2c_driver_transfer_data(sl_i2c_instance_t i2c_instance,
                                            sl_i2c_transfer_config_t const *p_transfer_config,
                                            uint16_t address)
{
  sl_i2c_status_t i2c_status;
  i2c_status = SL_I2C_SUCCESS;
  I2C0_Type *i2c;
  do {
    // To validate for NULL pointers and instance number
    if ((p_transfer_config == NULL) || (p_transfer_config->tx_buffer == NULL) || (p_transfer_config->rx_buffer == NULL)
        || (address > FOLLOWER_ADDR_10BIT_MAX) || (i2c_instance >= SL_I2C_LAST)) {
      i2c_status = SL_I2C_INVALID_PARAMETER;
      break;
    }
    // Updating i2c pointer as per instance number
    i2c = (I2C0_Type *)i2c_addr(i2c_instance);
    // sending data
    sl_i2c_driver_send_data_blocking(i2c_instance, address, p_transfer_config->tx_buffer, p_transfer_config->tx_len);
    // waiting for I2C instance to be in idle state
    wait_till_i2c_gets_idle(i2c);
    // receiving data
    sl_i2c_driver_receive_data_blocking(i2c_instance, address, p_transfer_config->rx_buffer, p_transfer_config->rx_len);
  } while (false);
  return i2c_status;
}

/*****************************************************************************
 * This API deinitializes I2C peripheral instance, also unregisters callback
 * Also deinits clock and power down the peripheral
 ******************************************************************************/
sl_i2c_status_t sl_i2c_driver_deinit(sl_i2c_instance_t i2c_instance)
{
  I2C0_Type *i2c;
  sl_i2c_status_t i2c_status;
  i2c_status = SL_I2C_SUCCESS;
  do {
    // Validating I2C instance
    if (i2c_instance >= SL_I2C_LAST) {
      i2c_status = SL_I2C_INVALID_PARAMETER;
      break;
    }
    // Updating i2c pointer as per instance number
    i2c = (I2C0_Type *)i2c_addr(i2c_instance);
    // disabling i2c instance
    sl_si91x_i2c_disable(i2c);
    // de-initializing i2c clock
    i2c_clock_deinit(i2c);
    // Unregistering Callback
    i2c_callback_function_ptr[i2c_instance] = NULL;
    // Resetting i2c
    sl_si91x_i2c_reset(i2c);
  } while (false);
  return i2c_status;
}

/*******************************************************************************
 * API to set the Pin configuration for I2C.
 * It configures the SDA and SCL pins.
 ******************************************************************************/
sl_i2c_status_t sl_si91x_i2c_pin_init(sl_i2c_pin_init_t *pin_init)
{
  sl_i2c_status_t status;
  do {
    // Validates the null pointer, if true returns error code
    if (pin_init == NULL) {
      status = SL_I2C_INVALID_PARAMETER;
      break;
    }
    // for I2C2
    if ((pin_init->sda_mux == I2C2_MUX) && (pin_init->sda_pad == I2C2_PAD)) {
      RSI_PS_UlpssPeriPowerUp(ULPSS_PWRGATE_ULP_I2C);
      // SCL
      sl_si91x_gpio_enable_ulp_pad_receiver((uint8_t)(pin_init->scl_pin));
      sl_gpio_set_pin_mode(ULP_PORT, (uint8_t)(pin_init->scl_pin), pin_init->scl_mux, OUTPUT);
#if defined(SLI_SI91X_MCU_MOV_ROM_API_TO_FLASH)
      egpio_ulp_pad_driver_disable_state(pin_init->scl_pin, INTERNAL_PULLUP);
#endif
      // SDA
      RSI_EGPIO_UlpPadReceiverEnable(pin_init->sda_pin);
      RSI_EGPIO_SetPinMux(EGPIO1, pin_init->sda_port, pin_init->sda_pin, pin_init->sda_mux);
#if defined(SLI_SI91X_MCU_MOV_ROM_API_TO_FLASH)
      egpio_ulp_pad_driver_disable_state(pin_init->sda_pin, INTERNAL_PULLUP);
#endif
    }
    // for I2C0 and I2C1
    else {
#if defined(SLI_SI917)
      RSI_PS_M4ssPeriPowerUp(M4SS_PWRGATE_ULP_EFUSE_PERI);
#else
      RSI_PS_M4ssPeriPowerUp(M4SS_PWRGATE_ULP_PERI1);
#endif
      if (pin_init->scl_pin >= MAX_GPIO) {
        sl_si91x_gpio_enable_ulp_pad_receiver((uint8_t)(pin_init->scl_pin - MAX_GPIO));
        sl_gpio_set_pin_mode(ULP_PORT, (uint8_t)(pin_init->scl_pin - MAX_GPIO), ULP_MODE, OUTPUT);
#if defined(SLI_SI91X_MCU_MOV_ROM_API_TO_FLASH)
        // Configuring internal pullup for follower mode
        egpio_ulp_pad_driver_disable_state(ULP_GPIO_SCL, INTERNAL_PULLUP);
#endif
      } else {
        sl_si91x_gpio_enable_pad_receiver(pin_init->scl_pin);
      }
      if (pin_init->scl_pin >= HOST_MIN && pin_init->scl_pin <= HOST_MAX) {
        sl_si91x_gpio_enable_pad_selection(pin_init->scl_pin);
      } else {
        sl_si91x_gpio_enable_pad_selection(pin_init->scl_pad);
      }
      sl_gpio_set_pin_mode(pin_init->scl_port, pin_init->scl_pin, pin_init->scl_mux, OUTPUT);
#if defined(SLI_SI91X_MCU_MOV_ROM_API_TO_FLASH)
      egpio_pad_driver_disable_state(pin_init->scl_pin, INTERNAL_PULLUP);
#endif
      //for sda
      if (pin_init->sda_pin >= MAX_GPIO) {
        sl_si91x_gpio_enable_ulp_pad_receiver((uint8_t)(pin_init->sda_pin - MAX_GPIO));
        sl_gpio_set_pin_mode(ULP_PORT, (uint8_t)(pin_init->sda_pin - MAX_GPIO), ULP_MODE, OUTPUT);
      } else {
        sl_si91x_gpio_enable_pad_receiver(pin_init->sda_pin);
      }
      if (pin_init->sda_pin >= HOST_MIN && pin_init->sda_pin <= HOST_MAX) {
        sl_si91x_gpio_enable_pad_selection(pin_init->sda_pin);
      } else {
        sl_si91x_gpio_enable_pad_selection(pin_init->sda_pad);
      }
      sl_gpio_set_pin_mode(pin_init->sda_port, pin_init->sda_pin, pin_init->sda_mux, OUTPUT);
#if defined(SLI_SI91X_MCU_MOV_ROM_API_TO_FLASH)
      egpio_pad_driver_disable_state(pin_init->sda_pin, INTERNAL_PULLUP);
#endif
    }
    status = SL_I2C_SUCCESS;
  } while (false);
  return status;
}

/*******************************************************************************
 * Function to wait till I2C leader/follower comes to idle state
 *
 * @param i2c (I2C_TypeDef) Pointer to the I2C instance base address.
 * @return none
 ******************************************************************************/
static void wait_till_i2c_gets_idle(I2C_TypeDef *i2c)
{
  // waiting for I2C instance to be in idle state
  while (i2c->IC_STATUS_b.ACTIVITY)
    ;
}

/*******************************************************************************
 * Function to return pointer to I2C instance base address as per I2C instance
 *number
 *
 * @param i2c_instance   I2C Instance \ref sl_i2c_instance_t
 * @return Pointer(void*) to i2c instance base address
 ******************************************************************************/
static void *i2c_addr(sl_i2c_instance_t i2c_instance)
{
  I2C0_Type *i2c;
  // Updating i2c pointer as per instance number
  if (i2c_instance == SL_I2C0) {
    i2c = ((I2C0_Type *)I2C0_BASE);
  }
  if (i2c_instance == SL_I2C1) {
    i2c = ((I2C0_Type *)I2C1_BASE);
  }
  if (i2c_instance == SL_I2C2) {
    i2c = ((I2C0_Type *)I2C2_BASE);
  }
  return i2c;
}

/*******************************************************************************
 * To configure the clock and power up the peripheral according to the
 * I2C instance.
 *
 * @param none
 * @return none
 ******************************************************************************/
static void i2c_clock_init(I2C_TypeDef *i2c)
{
  if ((uint32_t)i2c == I2C0_BASE) {
#if defined(SLI_SI917)
    // Powering up the peripheral.
    RSI_PS_M4ssPeriPowerUp(M4SS_PWRGATE_ULP_EFUSE_PERI);
#else
    // Powering up the peripheral.
    RSI_PS_M4ssPeriPowerUp(M4SS_PWRGATE_ULP_PERI1);
#endif
    // Initialize the I2C clock.
    RSI_CLK_I2CClkConfig(M4CLK, 1, I2C1_INSTAN);
  }
  if ((uint32_t)i2c == I2C1_BASE) {
#if defined(SLI_SI917)
    // Powering up the peripheral.
    RSI_PS_M4ssPeriPowerUp(M4SS_PWRGATE_ULP_EFUSE_PERI);
#else
    // Powering up the peripheral.
    RSI_PS_M4ssPeriPowerUp(M4SS_PWRGATE_ULP_PERI3);
#endif
    // Initialize the I2C clock.
    RSI_CLK_I2CClkConfig(M4CLK, 1, I2C2_INSTAN);
  }
  if ((uint32_t)i2c == I2C2_BASE) {
    // Powering up the peripheral.
    RSI_PS_UlpssPeriPowerUp(ULPSS_PWRGATE_ULP_I2C);
    // Enabling I2C clock.
    RSI_ULPSS_PeripheralEnable(ULPCLK, ULP_I2C_CLK, ENABLE_STATIC_CLK);
  }
}

/*******************************************************************************
 * To de-init the clock and power down the peripheral according to the
 * I2C instance.
 *
 * @param none
 * @return none
 ******************************************************************************/
static void i2c_clock_deinit(I2C_TypeDef *i2c)
{
  if ((uint32_t)i2c == I2C0_BASE) {
#if defined(SLI_SI917)
    // Powering up the peripheral.
    RSI_PS_M4ssPeriPowerDown(M4SS_PWRGATE_ULP_EFUSE_PERI);
#else
    // Powering up the peripheral.
    RSI_PS_M4ssPeriPowerDown(M4SS_PWRGATE_ULP_PERI1);
#endif
    // De-initialize the I2C clock.
    RSI_CLK_I2CClkConfig(M4CLK, 0, I2C1_INSTAN);
  }
  if ((uint32_t)i2c == I2C1_BASE) {
#if defined(SLI_SI917)
    // Powering up the peripheral.
    RSI_PS_M4ssPeriPowerDown(M4SS_PWRGATE_ULP_EFUSE_PERI);
#else
    // Powering up the peripheral.
    RSI_PS_M4ssPeriPowerDown(M4SS_PWRGATE_ULP_PERI3);
#endif
    // De-initialize the I2C clock.
    RSI_CLK_I2CClkConfig(M4CLK, 0, I2C2_INSTAN);
  }
  if ((uint32_t)i2c == I2C2_BASE) {
    // Powering up the peripheral.
    RSI_PS_UlpssPeriPowerDown(ULPSS_PWRGATE_ULP_I2C);
    // Disabling peripheral clock.
    RSI_ULPSS_PeripheralDisable(ULPCLK, ULP_I2C_CLK);
  }
}

/*******************************************************************************
 * Function to enable transmit FIFO DMA channel and sets transmit minimum data
 *level
 *
 * @param i2c (I2C_TypeDef) Pointer to the I2C instance base address.
 * @return none
 ******************************************************************************/
static void i2c_dma_tx_config(I2C_TypeDef *i2c)
{
  // Updating I2C DMA control and transmit Data Level Register values
  i2c->IC_DMA_CR_b.TDMAE    = SET;
  i2c->IC_DMA_TDLR_b.DMATDL = SET;
}
/*******************************************************************************
 * Function to enable receive FIFO DMA channel and sets receive minimum data
 *level
 *
 * @param i2c (I2C_TypeDef) Pointer to the I2C instance base address.
 * @return none
 ******************************************************************************/
static void i2c_dma_rx_config(I2C_TypeDef *i2c)
{
  // Updating I2C DMA control and Receive Data Level Register values
  i2c->IC_DMA_CR_b.RDMAE    = SET;
  i2c->IC_DMA_RDLR_b.DMARDL = ZERO;
}
/*******************************************************************************
 * Function to wait till follower releases SCL after getting read request
 *
 * @param i2c (I2C_TypeDef) Pointer to the I2C instance base address.
 * @return none
 ******************************************************************************/
static void wait_for_i2c_follower_ready(I2C_TypeDef *i2c)
{
  while (!i2c->IC_STATUS_b.SLV_HOLD_TX_FIFO_EMPTY)
    ;
}

/*******************************************************************************
 * I2C leader transmit handler.
 ******************************************************************************/
static void sl_i2c_leader_transmit_handler(I2C_TypeDef *i2c)
{
  uint32_t driver_status = 0;
  sl_i2c_instance_t i2c_instance;
  if (i2c == I2C0) {
    i2c_instance = SL_I2C0;
    if (i2c0_instance.write_buffer_length > LAST_DATA_COUNT) {
      // Adding STOP-bit after last data byte
      if (i2c0_instance.write_buffer_length == DATA_COUNT) {
        i2c->IC_DATA_CMD = (uint32_t)(i2c0_instance.write_buffer[i2c0_instance.write_buffer_current_index])
                           | (BIT_SET << STOP_BIT);
      } else {
        sl_si91x_i2c_tx(i2c, i2c0_instance.write_buffer[i2c0_instance.write_buffer_current_index]);
      }
      // After every byte transfer, incrementing buffer index and decrementing
      // length by one
      i2c0_instance.write_buffer_current_index++;
      i2c0_instance.write_buffer_length--;
    } else {
      sl_si91x_i2c_clear_interrupts(i2c, SL_I2C_EVENT_TRANSMIT_EMPTY);
      sl_si91x_i2c_disable_interrupts(i2c, ZERO_FLAG);
      // calling callback
      driver_status = 0;
      driver_status = SL_I2C_DATA_TRANSFER_COMPLETE;
      i2c_callback_function_ptr[i2c_instance](i2c_instance, driver_status);
    }
  }
  if (i2c == I2C1) {
    i2c_instance = SL_I2C1;
    if (i2c1_instance.write_buffer_length > LAST_DATA_COUNT) {
      // Adding STOP-bit after last data byte
      if (i2c1_instance.write_buffer_length == DATA_COUNT) {
        i2c->IC_DATA_CMD = (uint32_t)(i2c1_instance.write_buffer[i2c1_instance.write_buffer_current_index])
                           | (BIT_SET << STOP_BIT);
      } else {
        sl_si91x_i2c_tx(i2c, i2c1_instance.write_buffer[i2c1_instance.write_buffer_current_index]);
      }
      // After every byte transfer, incrementing buffer index and decrementing
      // length by one
      i2c1_instance.write_buffer_current_index++;
      i2c1_instance.write_buffer_length--;
    } else {
      sl_si91x_i2c_clear_interrupts(i2c, SL_I2C_EVENT_TRANSMIT_EMPTY);
      sl_si91x_i2c_disable_interrupts(i2c, ZERO_FLAG);
      // calling callback
      driver_status = 0;
      driver_status = SL_I2C_DATA_TRANSFER_COMPLETE;
      i2c_callback_function_ptr[i2c_instance](i2c_instance, driver_status);
    }
  }
  if (i2c == I2C2) {
    i2c_instance = SL_I2C2;
    if (i2c2_instance.write_buffer_length > LAST_DATA_COUNT) {
      // Adding STOP-bit after last data byte
      if (i2c2_instance.write_buffer_length == DATA_COUNT) {
        i2c->IC_DATA_CMD = (uint32_t)(i2c2_instance.write_buffer[i2c2_instance.write_buffer_current_index])
                           | (BIT_SET << STOP_BIT);
      } else {
        sl_si91x_i2c_tx(i2c, i2c2_instance.write_buffer[i2c2_instance.write_buffer_current_index]);
      }
      // After every byte transfer, incrementing buffer index and decrementing
      // length by one
      i2c2_instance.write_buffer_current_index++;
      i2c2_instance.write_buffer_length--;
    } else {
      // After last data byte reception clearing and disabling interrupts
      sl_si91x_i2c_clear_interrupts(i2c, SL_I2C_EVENT_TRANSMIT_EMPTY);
      sl_si91x_i2c_disable_interrupts(i2c, ZERO_FLAG);
      // calling callback
      driver_status = 0;
      driver_status = SL_I2C_DATA_TRANSFER_COMPLETE;
      i2c_callback_function_ptr[i2c_instance](i2c_instance, driver_status);
    }
  }
}

/*******************************************************************************
 * I2C leader receive handler.
 ******************************************************************************/
static void sl_i2c_leader_receive_handler(I2C_TypeDef *i2c)
{
  uint32_t driver_status = 0;
  sl_i2c_instance_t i2c_instance;
  if (i2c == I2C0) {
    i2c_instance = SL_I2C0;
    if (i2c0_instance.read_buffer_length > LAST_DATA_COUNT) {
      i2c0_instance.read_buffer[i2c0_instance.read_buffer_current_index] = i2c->IC_DATA_CMD_b.DAT;
      // After every byte reception, incrementing buffer index and decrementing
      // length by one
      i2c0_instance.read_buffer_current_index++;
      i2c0_instance.read_buffer_length--;
      if (i2c0_instance.read_buffer_length == DATA_COUNT) {
        // If the last byte is there to receive, and in leader mode, it needs to
        // send the stop byte.
        i2c->IC_DATA_CMD = (BIT_SET << MASK_READ_BIT) | (BIT_SET << STOP_BIT);
      }
      if (i2c0_instance.read_buffer_length > DATA_COUNT) {
        i2c->IC_DATA_CMD = (BIT_SET << MASK_READ_BIT);
      }
    }
    if (i2c0_instance.read_buffer_length == LAST_DATA_COUNT) {
      // After last data byte reception clearing and disabling interrupts
      sl_si91x_i2c_clear_interrupts(i2c, SL_I2C_EVENT_RECEIVE_FULL);
      sl_si91x_i2c_disable_interrupts(i2c, ZERO_FLAG);
      // calling callback
      driver_status = SL_I2C_DATA_TRANSFER_COMPLETE;
      i2c_callback_function_ptr[i2c_instance](i2c_instance, driver_status);
    }
  }
  if (i2c == I2C1) {
    i2c_instance = SL_I2C1;
    if (i2c1_instance.read_buffer_length > LAST_DATA_COUNT) {
      i2c1_instance.read_buffer[i2c1_instance.read_buffer_current_index] = i2c->IC_DATA_CMD_b.DAT;
      // After every byte reception, incrementing buffer index and decrementing
      // length by one
      i2c1_instance.read_buffer_current_index++;
      i2c1_instance.read_buffer_length--;
      if (i2c1_instance.read_buffer_length == DATA_COUNT) {
        // If the last byte is there to receive, and in leader mode, it needs to
        // send the stop byte.
        i2c->IC_DATA_CMD = (BIT_SET << MASK_READ_BIT) | (BIT_SET << STOP_BIT);
      }
      // Reading data byte
      if (i2c1_instance.read_buffer_length > DATA_COUNT) {
        i2c->IC_DATA_CMD = (BIT_SET << MASK_READ_BIT);
      }
    }
    if (i2c1_instance.read_buffer_length == LAST_DATA_COUNT) {
      // After last data byte reception clearing and disabling interrupts
      sl_si91x_i2c_clear_interrupts(i2c, SL_I2C_EVENT_RECEIVE_FULL);
      sl_si91x_i2c_disable_interrupts(i2c, ZERO_FLAG);
      // calling callback
      driver_status = SL_I2C_DATA_TRANSFER_COMPLETE;
      i2c_callback_function_ptr[i2c_instance](i2c_instance, driver_status);
    }
  }
  if (i2c == I2C2) {
    i2c_instance = SL_I2C2;
    if (i2c2_instance.read_buffer_length > LAST_DATA_COUNT) {
      i2c2_instance.read_buffer[i2c2_instance.read_buffer_current_index] = i2c->IC_DATA_CMD_b.DAT;
      // After every byte reception, incrementing buffer index and decrementing
      // length by one
      i2c2_instance.read_buffer_current_index++;
      i2c2_instance.read_buffer_length--;
      if (i2c2_instance.read_buffer_length == DATA_COUNT) {
        // If the last byte is there to receive, and in leader mode, it needs to
        // send the stop byte.
        i2c->IC_DATA_CMD = (BIT_SET << MASK_READ_BIT) | (BIT_SET << STOP_BIT);
      }
      // Reading data byte
      if (i2c2_instance.read_buffer_length > DATA_COUNT) {
        i2c->IC_DATA_CMD = (BIT_SET << MASK_READ_BIT);
      }
    }
    if (i2c2_instance.read_buffer_length == LAST_DATA_COUNT) {
      // After last data byte reception clearing and disabling interrupts
      sl_si91x_i2c_clear_interrupts(i2c, SL_I2C_EVENT_RECEIVE_FULL);
      sl_si91x_i2c_disable_interrupts(i2c, ZERO_FLAG);
      // calling callback
      driver_status = SL_I2C_DATA_TRANSFER_COMPLETE;
      i2c_callback_function_ptr[i2c_instance](i2c_instance, driver_status);
    }
  }
}

/*******************************************************************************
 * I2C follower receive handler.
 ******************************************************************************/
static void sl_i2c_follower_receive_handler(I2C_TypeDef *i2c)
{
  uint32_t driver_status = 0;
  sl_i2c_instance_t i2c_instance;
  if (i2c == I2C0) {
    i2c_instance = SL_I2C0;
    if (i2c0_instance.read_buffer_length > LAST_DATA_COUNT) {
      i2c0_instance.read_buffer[i2c0_instance.read_buffer_current_index] = i2c->IC_DATA_CMD_b.DAT;
      // After every byte reception, incrementing buffer index and decrementing
      // length by one
      i2c0_instance.read_buffer_current_index++;
      i2c0_instance.read_buffer_length--;
    }
    if (i2c0_instance.read_buffer_length == LAST_DATA_COUNT) {
      // After last data byte reception clearing and disabling interrupts
      sl_si91x_i2c_clear_interrupts(i2c, SL_I2C_EVENT_RECEIVE_FULL);
      sl_si91x_i2c_disable_interrupts(i2c, ZERO_FLAG);
      // calling callback and passing interrupt status
      driver_status = 0;
      driver_status = SL_I2C_DATA_TRANSFER_COMPLETE;
      i2c_callback_function_ptr[i2c_instance](i2c_instance, driver_status);
    }
  }
  if (i2c == I2C1) {
    i2c_instance = SL_I2C1;
    if (i2c1_instance.read_buffer_length > LAST_DATA_COUNT) {
      // reading data byte
      i2c1_instance.read_buffer[i2c1_instance.read_buffer_current_index] = i2c->IC_DATA_CMD_b.DAT;
      // After every byte reception, incrementing buffer index and decrementing
      // length by one
      i2c1_instance.read_buffer_current_index++;
      i2c1_instance.read_buffer_length--;
    }
    if (i2c1_instance.read_buffer_length == LAST_DATA_COUNT) {
      // After last data byte reception clearing and disabling interrupts
      sl_si91x_i2c_clear_interrupts(i2c, SL_I2C_EVENT_RECEIVE_FULL);
      sl_si91x_i2c_disable_interrupts(i2c, ZERO_FLAG);
      // calling callback and passing interrupt status
      driver_status = 0;
      driver_status = SL_I2C_DATA_TRANSFER_COMPLETE;
      i2c_callback_function_ptr[i2c_instance](i2c_instance, driver_status);
    }
  }
  if (i2c == I2C2) {
    i2c_instance = SL_I2C2;
    if (i2c2_instance.read_buffer_length > LAST_DATA_COUNT) {
      // reading data byte
      i2c2_instance.read_buffer[i2c2_instance.read_buffer_current_index] = i2c->IC_DATA_CMD_b.DAT;
      // After every byte reception, incrementing buffer index and decrementing
      // length by one
      i2c2_instance.read_buffer_current_index++;
      i2c2_instance.read_buffer_length--;
    }
    if (i2c2_instance.read_buffer_length == LAST_DATA_COUNT) {
      // After last data byte reception clearing and disabling interrupts
      sl_si91x_i2c_clear_interrupts(i2c, SL_I2C_EVENT_RECEIVE_FULL);
      sl_si91x_i2c_disable_interrupts(i2c, ZERO_FLAG);
      // calling callback and passing interrupt status
      driver_status = 0;
      driver_status = SL_I2C_DATA_TRANSFER_COMPLETE;
      i2c_callback_function_ptr[i2c_instance](i2c_instance, driver_status);
    }
  }
}
/*******************************************************************************
 * I2C follower transmit handler.
 ******************************************************************************/
static void sl_i2c_follower_transmit_handler(I2C_TypeDef *i2c)
{
  uint32_t driver_status = 0;
  sl_i2c_instance_t i2c_instance;
  if (i2c == I2C0) {
    i2c_instance = SL_I2C0;
    if (i2c0_instance.write_buffer_length > LAST_DATA_COUNT) {
      // Adding STOP-bit after last data byte
      if (i2c0_instance.write_buffer_length == DATA_COUNT) {
        i2c->IC_DATA_CMD = (uint32_t)(i2c0_instance.write_buffer[i2c0_instance.write_buffer_current_index])
                           | (BIT_SET << STOP_BIT);
      } else {
        // Writing data byte to FIFO
        sl_si91x_i2c_tx(i2c, i2c0_instance.write_buffer[i2c0_instance.write_buffer_current_index]);
      }
      // After every byte transfer, incrementing buffer index and decrementing
      // length by one
      i2c0_instance.write_buffer_current_index++;
      i2c0_instance.write_buffer_length--;
    } else {
      // After last data byte transfer clearing and disabling interrupts
      sl_si91x_i2c_clear_interrupts(i2c, SL_I2C_EVENT_TRANSMIT_EMPTY);
      sl_si91x_i2c_disable_interrupts(i2c, ZERO_FLAG);
      // calling callback and passing interrupt status
      driver_status = 0;
      driver_status = SL_I2C_DATA_TRANSFER_COMPLETE;
      i2c_callback_function_ptr[i2c_instance](i2c_instance, driver_status);
    }
  }
  if (i2c == I2C1) {
    i2c_instance = SL_I2C1;
    if (i2c1_instance.write_buffer_length > LAST_DATA_COUNT) {
      if (i2c1_instance.write_buffer_length == DATA_COUNT) {
        // Adding STOP-bit after last data byte
        i2c->IC_DATA_CMD = (uint32_t)(i2c1_instance.write_buffer[i2c1_instance.write_buffer_current_index])
                           | (BIT_SET << STOP_BIT);
      } else {
        // Writing data byte to FIFO
        sl_si91x_i2c_tx(i2c, i2c1_instance.write_buffer[i2c1_instance.write_buffer_current_index]);
      }
      // After every byte transfer, incrementing buffer index and decrementing
      // length by one
      i2c1_instance.write_buffer_current_index++;
      i2c1_instance.write_buffer_length--;
    } else {
      // After last data byte transfer clearing and disabling interrupts
      sl_si91x_i2c_clear_interrupts(i2c, SL_I2C_EVENT_TRANSMIT_EMPTY);
      sl_si91x_i2c_disable_interrupts(i2c, ZERO_FLAG);
      // calling callback and passing interrupt status
      driver_status = 0;
      driver_status = SL_I2C_DATA_TRANSFER_COMPLETE;
      i2c_callback_function_ptr[i2c_instance](i2c_instance, driver_status);
    }
  }
  if (i2c == I2C2) {
    i2c_instance = SL_I2C2;
    if (i2c2_instance.write_buffer_length > LAST_DATA_COUNT) {
      if (i2c2_instance.write_buffer_length == DATA_COUNT) {
        // Adding STOP-bit after last data byte
        i2c->IC_DATA_CMD = (uint32_t)(i2c2_instance.write_buffer[i2c2_instance.write_buffer_current_index])
                           | (BIT_SET << STOP_BIT);
      } else {
        // Writing data to FIFO
        sl_si91x_i2c_tx(i2c, i2c2_instance.write_buffer[i2c2_instance.write_buffer_current_index]);
      }
      // After every byte transfer, incrementing buffer index and decrementing
      // length by one
      i2c2_instance.write_buffer_current_index++;
      i2c2_instance.write_buffer_length--;
    } else {
      // After last data byte transfer clearing and disabling interrupts
      sl_si91x_i2c_clear_interrupts(i2c, SL_I2C_EVENT_TRANSMIT_EMPTY);
      sl_si91x_i2c_disable_interrupts(i2c, ZERO_FLAG);
      // calling callback and passing interrupt status
      driver_status = 0;
      driver_status = SL_I2C_DATA_TRANSFER_COMPLETE;
      i2c_callback_function_ptr[i2c_instance](i2c_instance, driver_status);
    }
  }
}

/*******************************************************************************
 * I2C handler function.
 ******************************************************************************/
static void i2c_handler(I2C_TypeDef *i2c)
{
  uint32_t status        = 0;
  uint32_t driver_status = 0;
  uint32_t clear         = 0;
  sl_i2c_instance_t i2c_instance;
  if (i2c == I2C0) {
    i2c_instance = SL_I2C0;
  }
  if (i2c == I2C1) {
    i2c_instance = SL_I2C1;
  }
  if (i2c == I2C2) {
    i2c_instance = SL_I2C2;
  }
  // Checking interrupt status
  status = i2c->IC_INTR_STAT;
  if (status & SL_I2C_EVENT_TRANSMIT_ABORT) {
    uint32_t tx_abrt = i2c->IC_TX_ABRT_SOURCE;
    if (tx_abrt & SL_I2C_ABORT_7B_ADDRESS_NOACK) {
      // Clearing interrupt by issuing stop, when 7-bit address not acknowledged
      i2c->IC_DATA_CMD_b.STOP = ONE;
      driver_status           = SL_I2C_7BIT_ADD;
      i2c_callback_function_ptr[i2c_instance](i2c_instance, driver_status);
    }
    if (tx_abrt & SL_I2C_ABORT_10B_ADDRESS1_NOACK) {
      // Clearing interrupt by issuing stop, when first byte of 10-bit address
      // not acknowledged
      i2c->IC_DATA_CMD_b.STOP = ONE;
      driver_status           = SL_I2C_10BIT_ADD;
      i2c_callback_function_ptr[i2c_instance](i2c_instance, driver_status);
    }
    if (tx_abrt & SL_I2C_ABORT_10B_ADDRESS2_NOACK) {
      // Clearing interrupt by issuing stop, when second byte of 10-bit address
      // not acknowledged
      i2c->IC_DATA_CMD_b.STOP = ONE;
      driver_status           = SL_I2C_10BIT_ADD;
      i2c_callback_function_ptr[i2c_instance](i2c_instance, driver_status);
    }
    if (tx_abrt & SL_I2C_ABORT_TX_DATA_NOACK) {
      // Clearing interrupt by issuing stop, when second byte of 10-bit address
      // not acknowledged
      i2c->IC_DATA_CMD_b.STOP = ONE;
      driver_status           = SL_I2C_NACK;
      i2c_callback_function_ptr[i2c_instance](i2c_instance, driver_status);
    }
    if (tx_abrt & SL_I2C_ABORT_GENERAL_CALL_NOACK) {
      // Clearing interrupt by reading the respective bit
      clear = i2c->IC_CLR_GEN_CALL_b.CLR_GEN_CALL;
    }
    if (tx_abrt & SL_I2C_ABORT_GENERAL_CALL_READ) {
      // Clearing interrupt by reading the respective bit
      clear = i2c->IC_CLR_GEN_CALL_b.CLR_GEN_CALL;
    }
    if (tx_abrt & SL_I2C_ABORT_HIGH_SPEED_ACK) {
    }
    if (tx_abrt & SL_I2C_ABORT_START_BYTE_ACK) {
    }
    if (tx_abrt & SL_I2C_ABORT_HIGH_SPEED_NO_RESTART) {
    }
    if (tx_abrt & SL_I2C_ABORT_START_BYTE_NO_RESTART) {
    }
    if (tx_abrt & SL_I2C_ABORT_10B_READ_NO_RESTART) {
    }
    if (tx_abrt & SL_I2C_ABORT_MASTER_DISABLED) {
    }
    if (tx_abrt & SL_I2C_ABORT_MASTER_ARBITRATION_LOST) {
      driver_status = SL_I2C_ARIBITRATION_LOST;
      i2c_callback_function_ptr[i2c_instance](i2c_instance, driver_status);
    }
    if (tx_abrt & SL_I2C_ABORT_SLAVE_ARBITRATION_LOST) {
      driver_status = SL_I2C_ARIBITRATION_LOST;
      i2c_callback_function_ptr[i2c_instance](i2c_instance, driver_status);
    }
    if (tx_abrt & SL_I2C_ABORT_SLAVE_FLUSH_TX_FIFO) {
      // Waits until the Tx FIFO has data to Transmit after read request, then
      // interrupt gets clear
      wait_for_i2c_follower_ready(i2c);
    }
    if (tx_abrt & SL_I2C_ABORT_SLAVE_READ_INTX) {
    }
    if (tx_abrt & SL_I2C_TX_TX_FLUSH_CNT) {
    }
    if (tx_abrt & SL_I2C_ABORT_USER_ABORT) {
    }
    if (tx_abrt & SL_I2C_ABORT_SDA_STUCK_AT_LOW) {
      // Initiating the SDA Recovery Mechanism
      i2c->IC_ENABLE_b.SDA_STUCK_RECOVERY_ENABLE = ONE;
      driver_status                              = SL_I2C_SDA_ERROR;
      i2c_callback_function_ptr[i2c_instance](i2c_instance, driver_status);
    }
    // Clearing all interrupts
    clear = i2c->IC_CLR_INTR;
    // Disables the interrupts.
    sl_si91x_i2c_disable_interrupts(i2c, SL_I2C_EVENT_TRANSMIT_EMPTY);
  }
  if (status & (SL_I2C_EVENT_SCL_STUCK_AT_LOW)) {
    // Clearing interrupt by reading the respective bit
    clear         = i2c->IC_CLR_INTR;
    driver_status = SL_I2C_SCL_ERROR;
    i2c_callback_function_ptr[i2c_instance](i2c_instance, driver_status);
    return;
  }
  if (status & (SL_I2C_EVENT_MST_ON_HOLD)) {
    // Clearing interrupt by reading the respective bit
    clear         = i2c->IC_CLR_INTR;
    driver_status = SL_I2C_BUS_HOLD;
    i2c_callback_function_ptr[i2c_instance](i2c_instance, driver_status);
    return;
  }
  if (status & (SL_I2C_EVENT_START_DETECT)) {
    // Clearing interrupt by reading the respective bit
    clear = i2c->IC_CLR_START_DET_b.CLR_START_DET;
    return;
  }
  if (status & (SL_I2C_EVENT_STOP_DETECT)) {
    // Clearing interrupt by reading the respective bit
    clear             = i2c->IC_CLR_STOP_DET_b.CLR_STOP_DET;
    uint32_t maskReg  = 0;
    maskReg           = i2c->IC_INTR_MASK;
    i2c->IC_INTR_MASK = (maskReg & (~SL_I2C_EVENT_RECEIVE_FULL));
    return;
  }
  if (status & (SL_I2C_EVENT_ACTIVITY_ON_BUS)) {
    // Clearing interrupt by reading the respective bit
    clear = i2c->IC_CLR_ACTIVITY_b.CLR_ACTIVITY;
    return;
  }
  if (status & SL_I2C_EVENT_RECEIVE_FULL) {
    if ((i2c0_instance.mode == SL_I2C_LEADER_MODE) || (i2c1_instance.mode == SL_I2C_LEADER_MODE)
        || (i2c2_instance.mode == SL_I2C_LEADER_MODE)) {
      sl_i2c_leader_receive_handler(i2c);
    } else {
      sl_i2c_follower_receive_handler(i2c);
    }
  }
  if (status & SL_I2C_EVENT_TRANSMIT_EMPTY) {
    if ((i2c0_instance.mode == SL_I2C_LEADER_MODE) || (i2c1_instance.mode == SL_I2C_LEADER_MODE)
        || (i2c2_instance.mode == SL_I2C_LEADER_MODE)) {
      sl_i2c_leader_transmit_handler(i2c);
    } else {
      sl_i2c_follower_transmit_handler(i2c);
    }
  }
  if (status & (SL_I2C_EVENT_READ_REQ)) {
    // waiting until the Tx FIFO has data to Transmit for the read request
    wait_for_i2c_follower_ready(i2c);
    // Clearing interrupt by reading the respective bit
    clear = i2c->IC_CLR_RD_REQ_b.CLR_RD_REQ;
    return;
  }
  if (status & (SL_I2C_EVENT_RECEIVE_UNDER)) {
    clear         = i2c->IC_CLR_RX_UNDER_b.CLR_RX_UNDER;
    driver_status = SL_I2C_BUS_ERROR;
    i2c_callback_function_ptr[i2c_instance](i2c_instance, driver_status);
    return;
  }
  if (status & (SL_I2C_EVENT_RECEIVE_OVER)) {
    clear         = i2c->IC_CLR_RX_OVER_b.CLR_RX_OVER;
    driver_status = SL_I2C_BUS_ERROR;
    i2c_callback_function_ptr[i2c_instance](i2c_instance, driver_status);
    return;
  }
  if (status & (SL_I2C_EVENT_RECEIVE_DONE)) {
    sl_si91x_i2c_clear_interrupts(i2c, SL_I2C_EVENT_RECEIVE_DONE);
    return;
  }
  if (status & (SL_I2C_EVENT_GENERAL_CALL)) {
    sl_si91x_i2c_clear_interrupts(i2c, SL_I2C_EVENT_GENERAL_CALL);
    return;
  }
  if (status & (SL_I2C_EVENT_RESTART_DET)) {
    sl_si91x_i2c_clear_interrupts(i2c, SL_I2C_EVENT_RESTART_DET);
    return;
  }
  // to avoid unused variable warning
  (void)clear;
}
/*******************************************************************************
 * IRQ handler for I2C 0.
 ******************************************************************************/
void I2C0_IRQHandler(void)
{
  i2c_handler(I2C0);
}

/*******************************************************************************
 * IRQ handler for I2C 1.
 ******************************************************************************/
void I2C1_IRQHandler(void)
{
  i2c_handler(I2C1);
}

/*******************************************************************************
 * IRQ handler for I2C 2.
 ******************************************************************************/
void I2C2_IRQHandler(void)
{
  i2c_handler(I2C2);
}

/*******************************************************************************
 I2C DMA transfer callback function
 ******************************************************************************/
static void i2c_dma_transfer_complete_callback(uint32_t channel, void *data)
{
  uint32_t driver_status;
  // to avoid unused variable warning
  (void)data;
  driver_status = SL_I2C_DATA_TRANSFER_COMPLETE;
  switch (channel) {
    case SL_I2C0_DMA_TX_CHANNEL:
      if (!i2c_read_ack) {
        *i2c_tx_last_byte_addr &= ~((uint32_t)BIT_SET << STOP_BIT);
        i2c_tx_last_byte_addr = ZERO;
      }
      if (i2c_read_ack) {
        i2c_read_ack            = CLEAR;
        I2C0->IC_ENABLE_b.ABORT = SET;
      } else {
        i2c_callback_function_ptr[ZERO](SL_I2C0, driver_status);
      }
      break;
    case SL_I2C0_DMA_RX_CHANNEL:
      i2c_callback_function_ptr[ZERO](SL_I2C0, driver_status);
      break;
    case SL_I2C1_DMA_TX_CHANNEL:
      if (!i2c_read_ack) {
        *i2c_tx_last_byte_addr &= ~((uint32_t)BIT_SET << STOP_BIT);
        i2c_tx_last_byte_addr = 0;
      }
      if (i2c_read_ack) {
        i2c_read_ack            = CLEAR;
        I2C1->IC_ENABLE_b.ABORT = SET;
      } else {
        i2c_callback_function_ptr[ONE](SL_I2C1, driver_status);
      }
      break;
    case SL_I2C1_DMA_RX_CHANNEL:
      i2c_callback_function_ptr[ONE](SL_I2C1, driver_status);
      break;
    case SL_I2C2_DMA_TX_CHANNEL:
      if (!i2c_read_ack) {
        *i2c_tx_last_byte_addr &= ~((uint32_t)BIT_SET << STOP_BIT);
        i2c_tx_last_byte_addr = 0;
      }
      if (i2c_read_ack) {
        i2c_read_ack            = CLEAR;
        I2C2->IC_ENABLE_b.ABORT = SET;
      } else {
        i2c_callback_function_ptr[TWO](SL_I2C2, driver_status);
      }
      break;
    case SL_I2C2_DMA_RX_CHANNEL:
      i2c_callback_function_ptr[TWO](SL_I2C2, driver_status);
      break;
    default:
      break;
  }
}

/*******************************************************************************
 I2C DMA error callback function
 ******************************************************************************/
static void i2c_dma_error_callback(uint32_t channel, void *data)
{
  uint32_t driver_status;
  // to avoid unused variable warning
  (void)data;
  driver_status = SL_I2C_DMA_TRANSFER_ERROR;
  // Calling i2c callback
  if (channel == 31) {
    i2c_callback_function_ptr[ZERO](SL_I2C0, driver_status);
  }
  if (channel == 3) {
    i2c_callback_function_ptr[ONE](SL_I2C1, driver_status);
  }
  if (channel == 5) {
    i2c_callback_function_ptr[TWO](SL_I2C2, driver_status);
  }
}
