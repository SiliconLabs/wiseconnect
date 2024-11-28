/***************************************************************************/ /**
 * @file i2c_follower_example.c
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
#include "app.h"
#include "sl_si91x_peripheral_i2c.h"
#include "i2c_follower_app.h"
#include "rsi_debug.h"
#include "rsi_rom_egpio.h"
#include "rsi_power_save.h"
#include "rsi_rom_clks.h"
#include "rsi_rom_ulpss_clk.h"
#include "cmsis_os2.h"
#include "sl_si91x_gpio.h"
#include "sl_si91x_driver_gpio.h"
#include "sl_si91x_clock_manager.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define OWN_I2C_ADDR                         0x50        // Own I2C address
#define RX_LENGTH                            BUFFER_SIZE // Number of bytes to receive
#define TX_LENGTH                            BUFFER_SIZE // Number of bytes to send
#define FIFO_THRESHOLD                       0x0         // FIFO threshold
#define ZERO_FLAG                            0           // Zero flag, No argument
#define PINMUX_MODE                          6           // I2C pinmux mode
#define PORT_ZERO                            0           // Port zero
#define HP_MAX_GPIO                          64          // High Power GPIO Maximum number
#define LAST_DATA_COUNT                      0           // Last read-write count
#define DATA_COUNT                           1           // Last second data count for verification
#define BIT_SET                              1           // Set bit
#define STOP_BIT                             9           // Bit to send stop command
#define RW_MASK_BIT                          8           // Bit to mask read and write
#define ULP_GPIO_SDA                         10          // SDA ULP GPIO Pin number
#define ULP_GPIO_SCL                         11          // SCL ULP GPIO Pin number
#define INTERNAL_PULLUP                      1           // Internal Pull-up enable
#define I2C_INSTANCE                         2           // I2C Instance for Pin configuration
#define I2C                                  ULP_I2C     // I2C Instance
#define MAX_7BIT_ADDRESS                     127         // Maximum 7-bit address
#define REFERENCE_CLOCK_FREQUENCY            (32000000u) // Reference clock frequency
#define HIGH_SPEED_REFERENCE_CLOCK_FREQUENCY (40000000u) // High speed mode reference clock frequency
#define I2C_STANDARD_MODE_CLOCK_FREQUENCY    (32000000u) // clock frequency for i2c standard mode
#define I2C_FAST_MODE_CLOCK_FREQUENCY        (32000000u) // clock frequency for i2c Fast mode
#define I2C_FAST_PLUS_MODE_CLOCK_FREQUENCY   (80000000u) // clock frequency for i2c fast plus mode
#define I2C_HIGH_SPEED_MODE_CLOCK_FREQUENCY  (80000000u) // clock frequency for i2c high speed mode
#define ULP_PORT                             4           // GPIO ULP port
#define ULP_MODE                             6           // ULP GPIO mode
#define MAX_GPIO                             64          // maximum GPIO pins
#define OUTPUT                               1           // Output value set
#define HOST_MIN                             24          // GPIO host pad minimum pin number
#define HOST_MAX                             31          // GPIO host pad maximum pin number
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

// Enum for different transmission scenarios
typedef enum {
  SEND_DATA,              // Send mode
  RECEIVE_DATA,           // Receive mode
  TRANSMISSION_COMPLETED, // Transmission completed mode
} i2c_mode_enum_t;

/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
#if (I2C_INSTANCE == 0)
// SCL, GPIO: 75, Exapansion Header: 5
// SDA, GPIO: 74, Exapansion Header: 3
static I2C_PIN scl = { RTE_I2C0_SCL_PORT, RTE_I2C0_SCL_PIN, RTE_I2C0_SCL_MUX, RTE_I2C0_SCL_PAD };
static I2C_PIN sda = { RTE_I2C0_SDA_PORT, RTE_I2C0_SDA_PIN, RTE_I2C0_SDA_MUX, RTE_I2C0_SDA_PAD };
#endif

#if (I2C_INSTANCE == 1)
// SCL, GPIO: 50, Board Pin: P19
// SDA, GPIO: 51, Board Pin: P20
static I2C_PIN scl = { RTE_I2C1_SCL_PORT, RTE_I2C1_SCL_PIN, RTE_I2C1_SCL_MUX, RTE_I2C1_SCL_PAD };
static I2C_PIN sda = { RTE_I2C1_SDA_PORT, RTE_I2C1_SDA_PIN, RTE_I2C1_SDA_MUX, RTE_I2C1_SDA_PAD };
#endif

#if (I2C_INSTANCE == 2)
// SCL, ULP_GPIO: 5, Exapansion Header: 13
// SDA, ULP_GPIO: 4, Exapansion Header: 11
static I2C_PIN scl = { RTE_I2C2_SCL_PORT, RTE_I2C2_SCL_PIN, RTE_I2C2_SCL_MUX, 0 };
static I2C_PIN sda = { RTE_I2C2_SDA_PORT, RTE_I2C2_SDA_PIN, RTE_I2C2_SDA_MUX, 0 };
#endif

static i2c_mode_enum_t current_mode = RECEIVE_DATA;
boolean_t i2c_send_complete         = false;
boolean_t i2c_receive_complete      = false;
boolean_t send_data_flag            = false;
boolean_t receive_data_flag         = false;
static uint32_t write_number        = 0;
static uint32_t write_count         = 0;
static uint32_t read_number         = 0;
static uint32_t read_count          = 0;
static uint8_t *write_data;
static uint8_t *read_data;
static uint8_t i2c_write_buffer[BUFFER_SIZE];
static uint8_t i2c_read_buffer[BUFFER_SIZE];

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void pin_configurations(void);
static void i2c_send_data(const uint8_t *data, uint32_t data_length, uint16_t follower_address);
static void i2c_receive_data(uint8_t *data, uint32_t data_length, uint16_t follower_address);
static void i2c_clock_init(I2C_TypeDef *i2c);
static void compare_data(void);
static void handle_follower_transmit_irq(void);
static void handle_follower_receive_irq(void);

/*******************************************************************************
 *************************** GLOBAL VARIABLES   *******************************
 ******************************************************************************/
osMessageQueueId_t mid_i2c_msg_queue;          // message queue id
extern osMessageQueueId_t mid_usart_msg_queue; // message queue id
/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
/*******************************************************************************
 * I2C example initialization function
 ******************************************************************************/
void i2c_follower_example_init(void)
{
  i2c_clock_init(I2C);
  sl_i2c_init_params_t config;

  // Filling the structure with default values.
  config.clhr = SL_I2C_STANDARD_BUS_SPEED;
  sl_si91x_clock_manager_m4_get_core_clk_src_freq(&config.freq);
  config.mode = SL_I2C_FOLLOWER_MODE;
  // Passing the structure and i2c instance for the initialization.
  sl_si91x_i2c_init(I2C, &config);
  DEBUGOUT("I2C follower is initialized successfully \n");
  // Pin is configured here.
  pin_configurations();
  DEBUGOUT("Pins are configured successfully \n");
  // Generating a buffer with values that needs to be sent.
  for (uint32_t loop = 0; loop < BUFFER_SIZE; loop++) {
    //    i2c_write_buffer[loop] = (loop + 0x1);
    i2c_read_buffer[loop] = 0;
  }
  receive_data_flag = true;

  // create and initialize message queue object for I2C Rx msgs
  mid_i2c_msg_queue = osMessageQueueNew(MSGQUEUE_OBJECTS, BUFFER_SIZE, NULL);
  if (mid_i2c_msg_queue == NULL) {
    ; // Message Queue object not created, handle failure
  }
}

/*******************************************************************************
 * Function will run continuously in while loop
 ******************************************************************************/
void i2c_follower_example_process_action(void)
{
  osStatus_t os_status;

  while (1) {
    // In switch case, according to the current mode, the transmission is executed.
    // First EEPROM write is executes i.e., I2C send.
    // After that offset address is sent to the follower device.
    // Now data is read from the EEPROM using I2C receive.
    switch (current_mode) {
      case RECEIVE_DATA:
        if (receive_data_flag) {
          // Validation for executing the API only once.
          i2c_receive_data(i2c_read_buffer, RX_LENGTH, OWN_I2C_ADDR);
          receive_data_flag = false;
        }
        if (i2c_receive_complete) {
          // It waits till i2c_receive_complete is true in IRQ handler.
          current_mode         = SEND_DATA;
          i2c_receive_complete = false;
          send_data_flag       = true;

          DEBUGOUT("I2C_Task: Data read from Leader successfully and writing the same into I2C_Message_Queue\n");

          // put the received msg into MessageQueue which will be taken by UART
          os_status = osMessageQueuePut(mid_i2c_msg_queue, i2c_read_buffer, 0U, 0U);
          if (os_status != osOK) {
            DEBUGOUT("I2C Message Queue write failed\n");
            break;
          }
        }
        break;
      case SEND_DATA:
        if (send_data_flag) {
          // wait for the i2c_write_buffer buffer to be filled from uart reception
          os_status = osMessageQueueGet(mid_usart_msg_queue, i2c_write_buffer, NULL, osWaitForever); // wait for message
          if (os_status != osOK) {
            DEBUGOUT("UART Message Queue read failed\n");
            break;
          }
          DEBUGOUT("I2C_Task: Data read from UART_Message_Queue successfully and forwarding to Leader\n");

          // Validation for executing the API only once.
          i2c_send_data(i2c_write_buffer, TX_LENGTH, OWN_I2C_ADDR);
          send_data_flag = false;
        }
        if (i2c_send_complete) {
          // It waits till i2c_send_complete is true in IRQ handler.
          current_mode      = TRANSMISSION_COMPLETED;
          send_data_flag    = true;
          i2c_send_complete = false;
          // After the receive is completed, input and output data is compared and
          // output is printed on console.
          compare_data();
        }
        break;
      case TRANSMISSION_COMPLETED:
        break;
      default:
        break;
    }

    osDelay(100);
  }
}

/*******************************************************************************
 * Function to send the data using I2C.
 * Here the FIFO threshold, direction and interrupts are configured.
 * 
 * @param[in] data (uint8_t) Constant pointer to the data which needs to be transferred.
 * @param[in] data_length (uint32_t) Length of the data that needs to be received.
 * @param[in] I2C device own address
 * @return none
 ******************************************************************************/
static void i2c_send_data(const uint8_t *data, uint32_t data_length, uint16_t follower_address)
{
  // Disables the interrupts.
  sl_si91x_i2c_disable_interrupts(I2C, ZERO_FLAG);
  // Updates the variables which are required for trasmission.
  write_data   = (uint8_t *)data;
  write_count  = 0;
  write_number = data_length;
  // Disables the I2C peripheral.
  sl_si91x_i2c_disable(I2C);
  // Setting the follower mask address, here leader behaves like follower.
  sl_si91x_i2c_set_follower_mask_address(I2C, follower_address);
  // Configures the FIFO threshold.
  sl_si91x_i2c_set_tx_threshold(I2C, FIFO_THRESHOLD);
  // Enables the I2C peripheral.
  sl_si91x_i2c_enable(I2C);
  // Configures the transmit empty interrupt.
  sl_si91x_i2c_set_interrupts(I2C, SL_I2C_EVENT_TRANSMIT_EMPTY);
  // Enables the interrupt.
  sl_si91x_i2c_enable_interrupts(I2C, ZERO_FLAG);
}

/*******************************************************************************
* Function to receive the data using I2C.
 * Here the FIFO threshold, direction and interrupts are configured.
 * 
 * @param[in] data (uint8_t) Constant pointer to the data which needs to be received.
 * @param[in] data_length (uint32_t) Length of the data that needs to be received.
 * @param[in] I2C device own address
 * @return none
 ******************************************************************************/
static void i2c_receive_data(uint8_t *data, uint32_t data_length, uint16_t follower_address)
{
  // Disables the interrupts.
  sl_si91x_i2c_disable_interrupts(I2C, ZERO_FLAG);
  // Updates the variables which are required for trasmission.
  read_data   = (uint8_t *)data;
  read_count  = 0;
  read_number = data_length;
  // Disables the I2C peripheral.
  sl_si91x_i2c_disable(I2C);
  // Setting the follower mask address, here leader behaves like follower.
  sl_si91x_i2c_set_follower_mask_address(I2C, follower_address);
  // Configures the FIFO threshold.
  sl_si91x_i2c_set_rx_threshold(I2C, FIFO_THRESHOLD);
  // Enables the I2C peripheral.
  sl_si91x_i2c_enable(I2C);
  // Sets the direction to read.
  sl_si91x_i2c_control_direction(I2C, SL_I2C_READ_MASK);
  // Configures the receive full interrupt.
  sl_si91x_i2c_set_interrupts(I2C, SL_I2C_EVENT_RECEIVE_FULL);
  // Enables the interrupt.
  sl_si91x_i2c_enable_interrupts(I2C, ZERO_FLAG);
}

/*******************************************************************************
 * Function to compare the input and output data
 * 
 * @param none
 * @return none
 ******************************************************************************/
static void compare_data(void)
{
  uint32_t data_index = 0;
  for (data_index = 0; data_index < BUFFER_SIZE; data_index++) {
    // If the i2c_read_buffer and buffer are same, it will be continued else, the for
    // loop will be break.
    if (i2c_write_buffer[data_index] != i2c_read_buffer[data_index]) {
      break;
    }
  }
  if (data_index == BUFFER_SIZE) {
    DEBUGOUT("Leader-follower read-write data comparison is successful, Test Case Passed \n");
  } else {
    DEBUGOUT("Leader-follower read-write data comparison is not successful, Test Case Failed \n");
  }
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
  if ((uint32_t)i2c == I2C1_BASE) {
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
#if (I2C_INSTANCE == 0) || (I2C_INSTANCE == 1)
  if (scl.pin >= MAX_GPIO) {
    sl_si91x_gpio_enable_ulp_pad_receiver((uint8_t)(scl.pin - MAX_GPIO));
    sl_gpio_set_pin_mode(ULP_PORT, (uint8_t)(scl.pin - MAX_GPIO), ULP_MODE, OUTPUT);
    sl_si91x_gpio_select_ulp_pad_driver_disable_state((scl.pin) - MAX_GPIO, GPIO_PULLUP);
  } else {
    sl_si91x_gpio_enable_pad_receiver(scl.pin);
    sl_si91x_gpio_select_pad_driver_disable_state((uint8_t)(scl.pin), GPIO_PULLUP);
  }
  if (scl.pin >= HOST_MIN && scl.pin <= HOST_MAX) {
    sl_si91x_gpio_enable_pad_selection(scl.pin);
  } else {
    sl_si91x_gpio_enable_pad_selection(scl.pad_sel);
  }
  sl_gpio_set_pin_mode(scl.port, scl.pin, scl.mode, OUTPUT);
  // for sda
  if (sda.pin >= MAX_GPIO) {
    sl_si91x_gpio_enable_ulp_pad_receiver((uint8_t)(sda.pin - MAX_GPIO));
    sl_gpio_set_pin_mode(ULP_PORT, (uint8_t)(sda.pin - MAX_GPIO), ULP_MODE, OUTPUT);
  } else {
    sl_si91x_gpio_enable_pad_receiver(sda.pin);
    sl_gpio_set_pin_mode(sda.port, sda.pin, sda.mode, OUTPUT);
  }
  if (sda.pin >= HOST_MIN && sda.pin <= HOST_MAX) {
    sl_si91x_gpio_enable_pad_selection(sda.pin);
  } else {
    sl_si91x_gpio_enable_pad_selection(sda.pad_sel);
  }
  sl_si91x_gpio_select_pad_driver_disable_state((uint8_t)(sda.pin), GPIO_PULLUP);
#endif

#if (I2C_INSTANCE == 2)
  // SCL
  sl_si91x_gpio_enable_ulp_pad_receiver((uint8_t)(scl.pin));
  sl_gpio_set_pin_mode(ULP_PORT, (uint8_t)(scl.pin), scl.mode, OUTPUT);
  sl_si91x_gpio_select_ulp_pad_driver_disable_state((uint8_t)(scl.pin), GPIO_PULLUP);
  // SDA
  RSI_EGPIO_UlpPadReceiverEnable(sda.pin);
  RSI_EGPIO_SetPinMux(EGPIO1, sda.port, sda.pin, sda.mode);
  sl_si91x_gpio_select_ulp_pad_driver_disable_state((uint8_t)(sda.pin), GPIO_PULLUP);

#endif // I2C_INSTANCE 2
}

/*******************************************************************************
 * Function to handle the transmit IRQ.
 * Transmit empty interrupt is monitored and byte by byte data is transmitted.
 * If the data transmission is completed, it clears and disables the interrupt.
 * 
 * @param none
 * @return none
 ******************************************************************************/
static void handle_follower_transmit_irq(void)
{
  if (write_number > LAST_DATA_COUNT) {
    if (write_number == DATA_COUNT) {
      I2C->IC_DATA_CMD = (uint32_t)write_data[write_count] | (BIT_SET << STOP_BIT);
    } else {
      // waiting for Slave to release SCL
      while (!I2C->IC_STATUS_b.SLV_HOLD_TX_FIFO_EMPTY)
        ;
      sl_si91x_i2c_tx(I2C, write_data[write_count]);
    }
    write_count++;
    write_number--;
  } else {
    sl_si91x_i2c_clear_interrupts(I2C, SL_I2C_EVENT_TRANSMIT_EMPTY);
    sl_si91x_i2c_disable_interrupts(I2C, ZERO_FLAG);
    i2c_send_complete = true;
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
static void handle_follower_receive_irq(void)
{
  if (read_number > LAST_DATA_COUNT) {
    read_data[read_count] = I2C->IC_DATA_CMD_b.DAT;
    read_count++;
    read_number--;
  }
  if (read_number == LAST_DATA_COUNT) {
    sl_si91x_i2c_clear_interrupts(I2C, SL_I2C_EVENT_RECEIVE_FULL);
    sl_si91x_i2c_disable_interrupts(I2C, ZERO_FLAG);
    i2c_receive_complete = true;
  }
}

/*******************************************************************************
 * IRQ handler for I2C0.
 ******************************************************************************/
void I2C0_IRQHandler(void)
{
  uint32_t status = 0;
  uint32_t clear  = 0;
  // Checking interrupt status
  status = I2C0->IC_INTR_STAT;
  if (status & SL_I2C_EVENT_TRANSMIT_ABORT) {
    uint32_t tx_abrt = I2C0->IC_TX_ABRT_SOURCE;
    if (tx_abrt & SL_I2C_ABORT_GENERAL_CALL_NOACK) {
      // Clearing interrupt by reading the respective bit
      clear = I2C0->IC_CLR_GEN_CALL_b.CLR_GEN_CALL;
    }
    if (tx_abrt & SL_I2C_ABORT_GENERAL_CALL_READ) {
      // Clearing interrupt by reading the respective bit
      clear = I2C0->IC_CLR_GEN_CALL_b.CLR_GEN_CALL;
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
    }
    if (tx_abrt & SL_I2C_ABORT_SLAVE_ARBITRATION_LOST) {
    }
    if (tx_abrt & SL_I2C_ABORT_SLAVE_FLUSH_TX_FIFO) {
    }
    if (tx_abrt & SL_I2C_ABORT_SLAVE_READ_INTX) {
    }
    if (tx_abrt & SL_I2C_TX_TX_FLUSH_CNT) {
    }
    if (tx_abrt & SL_I2C_ABORT_USER_ABORT) {
    }
    if (tx_abrt & SL_I2C_ABORT_SDA_STUCK_AT_LOW) {
      I2C0->IC_ENABLE_b.SDA_STUCK_RECOVERY_ENABLE = 0x1;
    }
    // Clearing all interrupts
    clear = I2C0->IC_CLR_INTR;
    // Disables the interrupts.
    sl_si91x_i2c_disable_interrupts(I2C0, SL_I2C_EVENT_TRANSMIT_EMPTY);
  }
  if (status & (SL_I2C_EVENT_SCL_STUCK_AT_LOW)) {
    // Clearing interrupt by reading the respective bit
    clear                = I2C0->IC_CLR_INTR;
    I2C0->IC_ENABLE_b.EN = 0;
    return;
  }
  if (status & (SL_I2C_EVENT_MST_ON_HOLD)) {
    // Clearing interrupt by reading the respective bit
    clear = I2C0->IC_CLR_INTR;
    return;
  }
  if (status & (SL_I2C_EVENT_START_DETECT)) {
    // Clearing interrupt by reading the respective bit
    clear = I2C0->IC_CLR_START_DET_b.CLR_START_DET;
    return;
  }
  if (status & (SL_I2C_EVENT_STOP_DETECT)) {
    // Clearing interrupt by reading the respective bit
    clear              = I2C0->IC_CLR_STOP_DET_b.CLR_STOP_DET;
    uint32_t maskReg   = 0;
    maskReg            = I2C0->IC_INTR_MASK;
    I2C0->IC_INTR_MASK = (maskReg & (~SL_I2C_EVENT_RECEIVE_FULL));
    return;
  }
  if (status & (SL_I2C_EVENT_ACTIVITY_ON_BUS)) {
    // Clearing interrupt by reading the respective bit
    clear = I2C0->IC_CLR_ACTIVITY_b.CLR_ACTIVITY;
    return;
  }
  if (status & (SL_I2C_EVENT_READ_REQ)) {
    // Clearing interrupt by reading the respective bit
    clear = I2C0->IC_CLR_RD_REQ_b.CLR_RD_REQ;
    return;
  }
  if (status & SL_I2C_EVENT_TRANSMIT_EMPTY) {
    handle_follower_transmit_irq();
  }
  if (status & SL_I2C_EVENT_RECEIVE_FULL) {
    handle_follower_receive_irq();
  }
  if (status & (SL_I2C_EVENT_RECEIVE_UNDER)) {
    clear = I2C0->IC_CLR_RX_UNDER_b.CLR_RX_UNDER;
    return;
  }
  if (status & (SL_I2C_EVENT_RECEIVE_OVER)) {
    clear = I2C0->IC_CLR_RX_OVER_b.CLR_RX_OVER;
    return;
  }
  if (status & (SL_I2C_EVENT_RECEIVE_DONE)) {
    sl_si91x_i2c_clear_interrupts(I2C0, SL_I2C_EVENT_RECEIVE_DONE);
    return;
  }
  if (status & (SL_I2C_EVENT_GENERAL_CALL)) {
    sl_si91x_i2c_clear_interrupts(I2C0, SL_I2C_EVENT_GENERAL_CALL);
    return;
  }
  if (status & (SL_I2C_EVENT_RESTART_DET)) {
    sl_si91x_i2c_clear_interrupts(I2C0, SL_I2C_EVENT_RESTART_DET);
    return;
  }
  // to avoid unused variable warning
  (void)clear;
}

/*******************************************************************************
 * IRQ handler for I2C1.
 ******************************************************************************/
void I2C1_IRQHandler(void)
{
  uint32_t status = 0;
  uint32_t clear  = 0;
  // Checking interrupt status
  status = I2C1->IC_INTR_STAT;
  if (status & SL_I2C_EVENT_TRANSMIT_ABORT) {
    uint32_t tx_abrt = I2C1->IC_TX_ABRT_SOURCE;
    if (tx_abrt & SL_I2C_ABORT_GENERAL_CALL_NOACK) {
      // Clearing interrupt by reading the respective bit
      clear = I2C1->IC_CLR_GEN_CALL_b.CLR_GEN_CALL;
    }
    if (tx_abrt & SL_I2C_ABORT_GENERAL_CALL_READ) {
      // Clearing interrupt by reading the respective bit
      clear = I2C1->IC_CLR_GEN_CALL_b.CLR_GEN_CALL;
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
    }
    if (tx_abrt & SL_I2C_ABORT_SLAVE_ARBITRATION_LOST) {
    }
    if (tx_abrt & SL_I2C_ABORT_SLAVE_FLUSH_TX_FIFO) {
    }
    if (tx_abrt & SL_I2C_ABORT_SLAVE_READ_INTX) {
    }
    if (tx_abrt & SL_I2C_TX_TX_FLUSH_CNT) {
    }
    if (tx_abrt & SL_I2C_ABORT_USER_ABORT) {
    }
    if (tx_abrt & SL_I2C_ABORT_SDA_STUCK_AT_LOW) {
      I2C1->IC_ENABLE_b.SDA_STUCK_RECOVERY_ENABLE = 0x1;
    }
    // Clearing all interrupts
    clear = I2C1->IC_CLR_INTR;
    // Disables the interrupts.
    sl_si91x_i2c_disable_interrupts(I2C1, SL_I2C_EVENT_TRANSMIT_EMPTY);
  }
  if (status & (SL_I2C_EVENT_SCL_STUCK_AT_LOW)) {
    // Clearing interrupt by reading the respective bit
    clear                = I2C1->IC_CLR_INTR;
    I2C1->IC_ENABLE_b.EN = 0;
    return;
  }
  if (status & (SL_I2C_EVENT_MST_ON_HOLD)) {
    // Clearing interrupt by reading the respective bit
    clear = I2C1->IC_CLR_INTR;
    return;
  }
  if (status & (SL_I2C_EVENT_START_DETECT)) {
    // Clearing interrupt by reading the respective bit
    clear = I2C1->IC_CLR_START_DET_b.CLR_START_DET;
    return;
  }
  if (status & (SL_I2C_EVENT_STOP_DETECT)) {
    // Clearing interrupt by reading the respective bit
    clear              = I2C1->IC_CLR_STOP_DET_b.CLR_STOP_DET;
    uint32_t maskReg   = 0;
    maskReg            = I2C1->IC_INTR_MASK;
    I2C0->IC_INTR_MASK = (maskReg & (~SL_I2C_EVENT_RECEIVE_FULL));
    return;
  }
  if (status & (SL_I2C_EVENT_ACTIVITY_ON_BUS)) {
    // Clearing interrupt by reading the respective bit
    clear = I2C1->IC_CLR_ACTIVITY_b.CLR_ACTIVITY;
    return;
  }
  if (status & (SL_I2C_EVENT_READ_REQ)) {
    // Clearing interrupt by reading the respective bit
    clear = I2C1->IC_CLR_RD_REQ_b.CLR_RD_REQ;
    return;
  }
  if (status & SL_I2C_EVENT_TRANSMIT_EMPTY) {
    handle_follower_transmit_irq();
  }
  if (status & SL_I2C_EVENT_RECEIVE_FULL) {
    handle_follower_receive_irq();
  }
  if (status & (SL_I2C_EVENT_RECEIVE_UNDER)) {
    clear = I2C1->IC_CLR_RX_UNDER_b.CLR_RX_UNDER;
    return;
  }
  if (status & (SL_I2C_EVENT_RECEIVE_OVER)) {
    clear = I2C1->IC_CLR_RX_OVER_b.CLR_RX_OVER;
    return;
  }
  if (status & (SL_I2C_EVENT_RECEIVE_DONE)) {
    sl_si91x_i2c_clear_interrupts(I2C1, SL_I2C_EVENT_RECEIVE_DONE);
    return;
  }
  if (status & (SL_I2C_EVENT_GENERAL_CALL)) {
    sl_si91x_i2c_clear_interrupts(I2C1, SL_I2C_EVENT_GENERAL_CALL);
    return;
  }
  if (status & (SL_I2C_EVENT_RESTART_DET)) {
    sl_si91x_i2c_clear_interrupts(I2C1, SL_I2C_EVENT_RESTART_DET);
    return;
  }
  // to avoid unused variable warning
  (void)clear;
}

/*******************************************************************************
 * IRQ handler for I2C2 (ULP_I2C).
 ******************************************************************************/
void I2C2_IRQHandler(void)
{
  uint32_t status = 0;
  uint32_t clear  = 0;
  // Checking interrupt status
  status = ULP_I2C->IC_INTR_STAT;
  if (status & SL_I2C_EVENT_TRANSMIT_ABORT) {
    uint32_t tx_abrt = ULP_I2C->IC_TX_ABRT_SOURCE;
    if (tx_abrt & SL_I2C_ABORT_GENERAL_CALL_NOACK) {
      // Clearing interrupt by reading the respective bit
      clear = ULP_I2C->IC_CLR_GEN_CALL_b.CLR_GEN_CALL;
    }
    if (tx_abrt & SL_I2C_ABORT_GENERAL_CALL_READ) {
      // Clearing interrupt by reading the respective bit
      clear = ULP_I2C->IC_CLR_GEN_CALL_b.CLR_GEN_CALL;
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
    }
    if (tx_abrt & SL_I2C_ABORT_SLAVE_ARBITRATION_LOST) {
    }
    if (tx_abrt & SL_I2C_ABORT_SLAVE_FLUSH_TX_FIFO) {
    }
    if (tx_abrt & SL_I2C_ABORT_SLAVE_READ_INTX) {
    }
    if (tx_abrt & SL_I2C_TX_TX_FLUSH_CNT) {
    }
    if (tx_abrt & SL_I2C_ABORT_USER_ABORT) {
    }
    if (tx_abrt & SL_I2C_ABORT_SDA_STUCK_AT_LOW) {
      ULP_I2C->IC_ENABLE_b.SDA_STUCK_RECOVERY_ENABLE = 0x1;
    }
    // Clearing all interrupts
    clear = ULP_I2C->IC_CLR_INTR;
    // Disables the interrupts.
    sl_si91x_i2c_disable_interrupts(ULP_I2C, SL_I2C_EVENT_TRANSMIT_EMPTY);
  }
  if (status & (SL_I2C_EVENT_SCL_STUCK_AT_LOW)) {
    // Clearing interrupt by reading the respective bit
    clear                   = ULP_I2C->IC_CLR_INTR;
    ULP_I2C->IC_ENABLE_b.EN = 0;
    return;
  }
  if (status & (SL_I2C_EVENT_MST_ON_HOLD)) {
    // Clearing interrupt by reading the respective bit
    clear = ULP_I2C->IC_CLR_INTR;
    return;
  }
  if (status & (SL_I2C_EVENT_START_DETECT)) {
    // Clearing interrupt by reading the respective bit
    clear = ULP_I2C->IC_CLR_START_DET_b.CLR_START_DET;
    return;
  }
  if (status & (SL_I2C_EVENT_STOP_DETECT)) {
    // Clearing interrupt by reading the respective bit
    clear              = ULP_I2C->IC_CLR_STOP_DET_b.CLR_STOP_DET;
    uint32_t maskReg   = 0;
    maskReg            = ULP_I2C->IC_INTR_MASK;
    I2C0->IC_INTR_MASK = (maskReg & (~SL_I2C_EVENT_RECEIVE_FULL));
    return;
  }
  if (status & (SL_I2C_EVENT_ACTIVITY_ON_BUS)) {
    // Clearing interrupt by reading the respective bit
    clear = ULP_I2C->IC_CLR_ACTIVITY_b.CLR_ACTIVITY;
    return;
  }
  if (status & (SL_I2C_EVENT_READ_REQ)) {
    // Clearing interrupt by reading the respective bit
    clear = ULP_I2C->IC_CLR_RD_REQ_b.CLR_RD_REQ;
    return;
  }
  if (status & SL_I2C_EVENT_TRANSMIT_EMPTY) {
    handle_follower_transmit_irq();
  }
  if (status & SL_I2C_EVENT_RECEIVE_FULL) {
    handle_follower_receive_irq();
  }
  if (status & (SL_I2C_EVENT_RECEIVE_UNDER)) {
    clear = ULP_I2C->IC_CLR_RX_UNDER_b.CLR_RX_UNDER;
    return;
  }
  if (status & (SL_I2C_EVENT_RECEIVE_OVER)) {
    clear = ULP_I2C->IC_CLR_RX_OVER_b.CLR_RX_OVER;
    return;
  }
  if (status & (SL_I2C_EVENT_RECEIVE_DONE)) {
    sl_si91x_i2c_clear_interrupts(ULP_I2C, SL_I2C_EVENT_RECEIVE_DONE);
    return;
  }
  if (status & (SL_I2C_EVENT_GENERAL_CALL)) {
    sl_si91x_i2c_clear_interrupts(ULP_I2C, SL_I2C_EVENT_GENERAL_CALL);
    return;
  }
  if (status & (SL_I2C_EVENT_RESTART_DET)) {
    sl_si91x_i2c_clear_interrupts(ULP_I2C, SL_I2C_EVENT_RESTART_DET);
    return;
  }
  // to avoid unused variable warning
  (void)clear;
}
