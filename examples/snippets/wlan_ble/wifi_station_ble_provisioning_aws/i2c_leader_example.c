/***************************************************************************/ /**
 * @file i2c_example.c
 * @brief I2C examples functions
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
#include "sl_si91x_peripheral_i2c.h"
#include "i2c_leader_example.h"
#include "rsi_debug.h"
#include "rsi_rom_egpio.h"
#include "rsi_power_save.h"
#include "rsi_rom_clks.h"
#include "rsi_rom_ulpss_clk.h"
#include "cmsis_os2.h"
#include "app.h"
#include "sl_si91x_peripheral_i2c.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define SIZE_BUFFERS      2    // Size of buffer
#define FOLLOWER_I2C_ADDR 0x48 // LM75 Temperature sensor I2C address
#define RX_LENGTH         16   // Number of bytes to receive
#define TX_LENGTH         16   // Number of bytes to send
#define OFFSET_LENGTH     1    // Offset length
#define FIFO_THRESHOLD    0x0  // FIFO threshold
#define ZERO_FLAG         0    // Zero flag, No argument
#define PORT_ZERO         0    // Port zero
#define HP_MAX_GPIO       64   // High Power GPIO Maximum number
#define LAST_DATA_COUNT   0    // Last read-write count
#define DATA_COUNT        1    // Last second data count for verification
#define BIT_SET           1    // Set bit
#define STOP_BIT          9    // Bit to send stop command
#define RW_MASK_BIT       8    // Bit to mask read and write
#define MAX_7BIT_ADDRESS  127  // Maximum 7-bit address

/*******************************************************************************
 ******************************  Data Types  ***********************************
 ******************************************************************************/
// Structure to hold the pin configuration
typedef const struct {
  uint8_t port;    // GPIO port
  uint8_t pin;     // GPIO pin
  uint8_t mode;    // GPIO mode
  uint8_t pad_sel; // GPIO pad selection
} I2C_PIN_;

/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
static I2C_PIN scl = { RTE_I2C2_SCL_PORT, RTE_I2C2_SCL_PIN, RTE_I2C2_SCL_MUX, 0 };
static I2C_PIN sda = { RTE_I2C2_SDA_PORT, RTE_I2C2_SDA_PIN, RTE_I2C2_SDA_MUX, 0 };
//static i2c_mode_enum_t current_mode = SEND_DATA;
volatile uint8_t i2c_send_complete    = 0;
volatile uint8_t i2c_receive_complete = 0;
volatile uint8_t send_data_flag       = 0;
volatile uint8_t receive_data_flag    = 0;

static uint32_t write_number = 0;
static uint32_t write_count  = 0;
static uint32_t read_number  = 0;
static uint32_t read_count   = 0;
static uint8_t *write_data;
static uint8_t *read_data;

float sensor_data = 0.0;
float info        = 0;

volatile uint8_t read_buffer[SIZE_BUFFERS];
static uint8_t write_buffer[SIZE_BUFFERS];

extern osSemaphoreId_t i2c_sem;
extern osSemaphoreId_t rsi_mqtt_sem;
static uint8_t *write_data;
static uint8_t *read_data;

extern uint8_t I2C0_TRANSFER;

void i2c_init(void)
{
  /*I2C Initialization */
  i2c_leader_example_init();
}

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void pin_configurations(void);
static void i2c_send_data(const uint8_t *data, uint32_t data_length, uint16_t follower_address);
static void i2c_receive_data(uint8_t *data, uint32_t data_length, uint16_t follower_address);
static void i2c_clock_init(I2C_TypeDef *i2c);
static void handle_leader_transmit_irq(void);
static void handle_leader_receive_irq(void);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
/*******************************************************************************
 * I2C example initialization function
 ******************************************************************************/
void i2c_leader_example_init(void)
{
  i2c_clock_init(ULP_I2C);
  // For aborting, I2C instance should be enabled.
  sl_si91x_i2c_enable(ULP_I2C);
  // It aborts if any existing activity is there.
  sl_si91x_i2c_abort_transfer(ULP_I2C);
  sl_si91x_i2c_disable(ULP_I2C);
  NVIC_SetPriority(I2C2_IRQn, 15);
  sl_i2c_init_params_t config;
  // Filling the structure with default values.
  config.clhr = SL_I2C_STANDARD_BUS_SPEED;
  config.freq = sl_si91x_i2c_get_frequency(ULP_I2C);
  config.mode = SL_I2C_LEADER_MODE;
  // Passing the structure and i2c instance for the initialization.
  sl_si91x_i2c_init(ULP_I2C, &config);
  // Pin is configured here.
  pin_configurations();
  // Generating a buffer with values that needs to be sent.
  for (uint8_t loop = 0; loop < SIZE_BUFFERS; loop++) {
    write_buffer[loop] = (loop + 0x1);
    read_buffer[loop]  = 0;
  }
  send_data_flag = true;
}
/*******************************************************************************

* Function to provide 1 ms Delay

*******************************************************************************/

void Delay(uint32_t idelay)
{

  for (uint32_t x = 0; x < 4600 * idelay; x++) //1.002ms delay

  {
    __NOP();
  }
}
/*******************************************************************************
 * Function will run continuously in while loop
 ******************************************************************************/
void i2c_leader_example_process_action(void)
{
  if (send_data_flag) {
    uint8_t a[5] = "\0";
    sl_si91x_i2c_set_follower_address(ULP_I2C, 0x48, 0);
    // Validation for executing the API only once.
    a[0] = (uint8_t)(0x00 & 0xFF);
    a[1] = (uint8_t)(0x01 & 0xFF);
    i2c_send_data(a, 2, FOLLOWER_I2C_ADDR);
    while (!i2c_send_complete)
      ;
    // It waits till i2c_send_complete is true in IRQ handler.
    DEBUGOUT("Data is transferred to Follower successfully \n");

    // Validation for executing the API only once.
    i2c_receive_data((uint8_t *)read_buffer, 2, FOLLOWER_I2C_ADDR);
    Delay(10);
    while (!i2c_receive_complete)
      ;
    // To convert into negative value and converting into floating points
    if ((read_buffer[0] >= 128) && (read_buffer[1] < 128)) {
      read_buffer[0] = ~(read_buffer[0]);
      sensor_data    = (read_buffer[0] + 1);
      info           = sensor_data;
      info           = -(info);
      DEBUGOUT(" Temperature in Celsius %f \t \n", info);
    } else if ((read_buffer[0] >= 128) && (read_buffer[1] >= 128)) {
      read_buffer[0] = ~(read_buffer[0]);
      sensor_data    = read_buffer[0];
      info           = sensor_data + 0.5;
      info           = -(info);
      DEBUGOUT(" Temperature in Celsius %f \t \n", info);
      // Convert into floating value
    } else if ((read_buffer[0] < 128) && (read_buffer[1] >= 128)) {
      sensor_data = read_buffer[0];
      info        = sensor_data + 0.5;
      DEBUGOUT(" Temperature in Celsius %f \t \n", info);
    } else if ((read_buffer[0] < 128) && (read_buffer[1] < 128)) {
      sensor_data = read_buffer[0];
      info        = sensor_data;
      DEBUGOUT(" Temperature in Celsius  %f \t \n", info);
    }

    receive_data_flag = 0;
    sensor_data       = info;
    info              = 0;
    read_buffer[0]    = 0;
    read_buffer[1]    = 0;
    if (i2c_receive_complete) {
      // It waits till i2c_receive_complete is true in IRQ handler.
      DEBUGOUT("Data is received from Follower successfully \n");
      i2c_receive_complete = 0;
      send_data_flag       = 1;
    }
  }
}

/*******************************************************************************
 * Function to send the data using I2C.
 * Here the FIFO threshold, direction and interrupts are configured.
 * 
 * @param[in] data (uint8_t) Constant pointer to the data which needs to be transferred.
 * @param[in] data_length (uint32_t) Length of the data that needs to be received.
 * @return none
 ******************************************************************************/
static void i2c_send_data(const uint8_t *data, uint32_t data_length, uint16_t follower_address)
{
  bool is_10bit_addr = false;
  // Disables the interrupts.
  sl_si91x_i2c_disable_interrupts(ULP_I2C, ZERO_FLAG);
  // Updates the variables which are required for trasmission.
  write_data   = (uint8_t *)data;
  write_count  = 0;
  write_number = data_length;
  // Disables the I2C peripheral.
  sl_si91x_i2c_disable(ULP_I2C);
  // Checking is address is 7-bit or 10bit
  if (follower_address > MAX_7BIT_ADDRESS) {
    is_10bit_addr = true;
  }
  // Setting the follower address recevied in parameter structure.
  sl_si91x_i2c_set_follower_address(ULP_I2C, follower_address, is_10bit_addr);
  // Configures the FIFO threshold.
  sl_si91x_i2c_set_tx_threshold(ULP_I2C, FIFO_THRESHOLD);
  // Enables the I2C peripheral.
  sl_si91x_i2c_enable(ULP_I2C);
  // Sets the direction to write.
  // Configures the transmit empty interrupt.
  sl_si91x_i2c_set_interrupts(ULP_I2C, SL_I2C_EVENT_TRANSMIT_EMPTY);
  // Enables the interrupt.
  sl_si91x_i2c_enable_interrupts(ULP_I2C, ZERO_FLAG);
}

/*******************************************************************************
 * Function to receive the data using I2C.
 * Here the FIFO threshold, direction and interrupts are configured.
 * 
 * @param[in] data (uint8_t) Constant pointer to the data which needs to be transferred.
 * @param[in] data_length (uint32_t) Length of the data that needs to be received.
 * @return none
 ******************************************************************************/
static void i2c_receive_data(uint8_t *data, uint32_t data_length, uint16_t follower_address)
{
  bool is_10bit_addr = false;
  // Disables the interrupts.
  sl_si91x_i2c_disable_interrupts(ULP_I2C, ZERO_FLAG);
  // Updates the variables which are required for trasmission.
  read_data   = (uint8_t *)data;
  read_count  = 0;
  read_number = data_length;
  // Disables the I2C peripheral.
  sl_si91x_i2c_disable(ULP_I2C);
  // Configures the FIFO threshold.
  // Checking is address is 7-bit or 10bit
  if (follower_address > MAX_7BIT_ADDRESS) {
    is_10bit_addr = true;
  }
  // Setting the follower address recevied in parameter structure.
  sl_si91x_i2c_set_follower_address(ULP_I2C, follower_address, is_10bit_addr);
  sl_si91x_i2c_set_rx_threshold(ULP_I2C, FIFO_THRESHOLD);
  // Enables the I2C peripheral.
  sl_si91x_i2c_enable(ULP_I2C);
  // Sets the direction to read.
  sl_si91x_i2c_control_direction(ULP_I2C, SL_I2C_READ_MASK);
  // Configures the receive full interrupt.
  sl_si91x_i2c_set_interrupts(ULP_I2C, SL_I2C_EVENT_RECEIVE_FULL);
  // Enables the interrupt.
  sl_si91x_i2c_enable_interrupts(ULP_I2C, ZERO_FLAG);
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
  if ((uint32_t)i2c == I2C2_BASE) {
#if defined(SLI_SI917) || defined(SLI_SI915)
    // Powering up the peripheral.
    RSI_PS_M4ssPeriPowerUp(M4SS_PWRGATE_ULP_EFUSE_PERI);
#else
    // Powering up the peripheral.
    RSI_PS_M4ssPeriPowerUp(M4SS_PWRGATE_ULP_PERI1);
#endif
    // Initialize the I2C clock.
    RSI_CLK_I2CClkConfig(M4CLK, 1, I2C1_INSTAN);
  }
  if ((uint32_t)i2c == I2C2_BASE) {
#if defined(SLI_SI917) || defined(SLI_SI915)
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
    // Initialize the I2C clock.
    RSI_ULPSS_PeripheralEnable(ULPCLK, ULP_I2C_CLK, ENABLE_STATIC_CLK);
  }
}

/*******************************************************************************
 * Function to set the Pin configuration for I2C.
 * It configures the SDA and SCL pins.
 * 
 * @param none
 * @return none
 ******************************************************************************/
static void pin_configurations(void)
{
  // SCL
  RSI_EGPIO_UlpPadReceiverEnable((uint8_t)(scl.pin - HP_MAX_GPIO));
  RSI_EGPIO_SetPinMux(EGPIO1, scl.port, (uint8_t)(scl.pin - HP_MAX_GPIO), scl.mode);
  // SDA
  RSI_EGPIO_UlpPadReceiverEnable((uint8_t)(sda.pin - HP_MAX_GPIO));
  RSI_EGPIO_SetPinMux(EGPIO1, sda.port, (uint8_t)(sda.pin - HP_MAX_GPIO), scl.mode);
}

/*******************************************************************************
 * Function to handle the transmit IRQ.
 * Transmit empty interrupt is monitored and byte by byte data is transmitted.
 * If the data transmission is completed, it clears and disables the interrupt.
 * 
 * @param none
 * @return none
 ******************************************************************************/
static void handle_leader_transmit_irq(void)
{
  if (write_number > LAST_DATA_COUNT) {
    if (write_number == DATA_COUNT) {
      ULP_I2C->IC_DATA_CMD = (uint32_t)write_data[write_count] | (BIT_SET << STOP_BIT);
    } else {
      sl_si91x_i2c_tx(ULP_I2C, write_data[write_count]);
    }
    write_count++;
    write_number--;
  } else {
    sl_si91x_i2c_clear_interrupts(ULP_I2C, SL_I2C_EVENT_TRANSMIT_EMPTY);
    sl_si91x_i2c_disable_interrupts(ULP_I2C, ZERO_FLAG);
    i2c_send_complete = 1;
  }
}

/*******************************************************************************
 * Function to handle the receive IRQ.
 * Receive full interrupt is monitored and byte by byte data is received.
 * If the data receive is completed, it clears and disables the interrupt.
 * 
 * @param none
 * @return none
 ******************************************************************************/
static void handle_leader_receive_irq(void)
{
  if (read_number > LAST_DATA_COUNT) {
    read_data[read_count] = ULP_I2C->IC_DATA_CMD_b.DAT;
    read_count++;
    read_number--;
    if (read_number == DATA_COUNT) {
      // If the last byte is there to receive, and in leader mode, it needs to send
      // the stop byte.
      ULP_I2C->IC_DATA_CMD = (BIT_SET << RW_MASK_BIT) | (BIT_SET << STOP_BIT);
    }
    if (read_number > DATA_COUNT) {
      ULP_I2C->IC_DATA_CMD = (BIT_SET << RW_MASK_BIT);
    }
  }
  if (read_number == LAST_DATA_COUNT) {
    sl_si91x_i2c_clear_interrupts(ULP_I2C, SL_I2C_EVENT_RECEIVE_FULL);
    sl_si91x_i2c_disable_interrupts(ULP_I2C, ZERO_FLAG);
    i2c_receive_complete = 1;
  }
}

/*******************************************************************************
 * IRQ handler for I2C2 (ULP_I2C).
 ******************************************************************************/
void I2C2_IRQHandler(void)
{
  uint32_t status = 0;
  status          = ULP_I2C->IC_INTR_STAT;
  if (status & SL_I2C_EVENT_TRANSMIT_EMPTY) {
    handle_leader_transmit_irq();
  }
  if (status & SL_I2C_EVENT_RECEIVE_FULL) {
    handle_leader_receive_irq();
  }
}
