/***************************************************************************/ /**
 * @file i2s_example.c
 * @brief I2S examples functions
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
#include "sl_si91x_i2s.h"
#include "sl_si91x_i2s_config.h"
#include "i2s_loopback_example.h"
#include "rsi_board.h"
#include "rsi_chip.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define SOC_PLL_REF_FREQUENCY 40000000  // PLL input REFERENCE clock 40MHZ
#define PS4_SOC_FREQ          180000000 // PLL out clock 180MHz
#define BUFFER_SIZE           1024      // Transmit/Receive buffer size
#define I2S_INSTANCE          0         // I2S instance

/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
uint16_t data_in[BUFFER_SIZE];
uint16_t data_out[BUFFER_SIZE];
static sl_i2s_handle_t i2s_driver_handle    = NULL;
static uint8_t send_complete                = 0;
static uint8_t receive_complete             = 0;
static sl_i2s_xfer_config_t i2s_xfer_config = { 0 };

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static int32_t clock_configuration_pll(void);
static void callback_event(uint32_t event);
static void compare_loop_back_data(void);
/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
/*******************************************************************************
 * I2S example initialization function
 ******************************************************************************/
void i2s_example_init(void)
{
  sl_status_t status;
  sl_i2s_version_t i2s_version;
  sl_i2s_status_t i2s_status; //Initialize I2S transfer structure
  i2s_xfer_config.mode          = SL_I2S_MASTER;
  i2s_xfer_config.protocol      = SL_I2S_PROTOCOL;
  i2s_xfer_config.resolution    = SL_I2S_RESOLUTION;
  i2s_xfer_config.sampling_rate = SL_I2S_SAMPLING_RATE;
  i2s_xfer_config.sync          = SL_I2S_ASYNC;
  i2s_xfer_config.data_size     = SL_I2S_DATA_SIZE32;

  // Filling the data out array with integer values
  for (uint32_t i = 0; i < BUFFER_SIZE; i++) {
    data_out[i] = i;
  }
  do {
    //Fetch I2S driver version
    i2s_version = sl_si91x_i2s_get_version();
    DEBUGOUT("I2S version is fetched successfully \n");
    DEBUGOUT("API version is %d.%d.%d\n", i2s_version.release, i2s_version.major, i2s_version.minor);
    // Configure PLL and switch M4 clock to PLL clock for speed operations
    if (clock_configuration_pll()) {
      DEBUGOUT("PLL configuration fail\r\n");
      break;
    }
    DEBUGOUT("PLL configuration success\r\n");
    //Initialize I2S peripheral and store driver handle in i2s_driver_handle
    status = sl_si91x_i2s_init(I2S_INSTANCE, &i2s_driver_handle);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("I2S Initialization fail\r\n");
      break;
    }
    DEBUGOUT("I2S Initialization success\r\n");
    //Get the status of I2S peripheral
    i2s_status = sl_si91x_i2s_get_status(i2s_driver_handle);
    DEBUGOUT("I2S status is fetched successfully \n");
    DEBUGOUT("Frame error: %d\n", i2s_status.frame_error);
    DEBUGOUT("Rx status: %d\n", i2s_status.rx_busy);
    DEBUGOUT("Rx overflow status: %d\n", i2s_status.rx_overflow);
    DEBUGOUT("Tx status: %d\n", i2s_status.tx_busy);
    DEBUGOUT("Tx underflow status: %d\n", i2s_status.tx_underflow);
    //Configure ARM full power mode
    status = sl_si91x_i2s_configure_power_mode(i2s_driver_handle, SL_I2S_FULL_POWER);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("I2S power mode config fail\r\n");
      break;
    }
    DEBUGOUT("I2S power mode config success\r\n");
    //Register user callback handler
    status = sl_si91x_i2s_register_event_callback(i2s_driver_handle, callback_event);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("I2S user callback register fail\r\n");
      break;
    }
    DEBUGOUT("I2S user callback register success\r\n");
    i2s_xfer_config.transfer_type = SL_I2S_TRANSMIT;
    //Configure transmitter parameters for i2s transfer
    if (sl_si91x_i2s_config_transmit_receive(i2s_driver_handle, &i2s_xfer_config)) {
      DEBUGOUT("I2S transmit config fail\r\n");
      break;
    }
    DEBUGOUT("I2S transmit config success\r\n");
    i2s_xfer_config.transfer_type = SL_I2S_RECEIVE;
    //Configure receiver parameters for i2s transfer
    if (sl_si91x_i2s_config_transmit_receive(i2s_driver_handle, &i2s_xfer_config)) {
      DEBUGOUT("I2S receive config fail\r\n");
      break;
    }
    DEBUGOUT("I2S receive config success\r\n");
    //Configure I2S receive DMA channel
    if (sl_si91x_i2s_receive_data(i2s_driver_handle, data_in, BUFFER_SIZE)) {
      DEBUGOUT("I2S receive start fail\r\n");
      break;
    }
    DEBUGOUT("I2S receive start success\r\n");
    //Configure I2S transmit DMA channel
    if (sl_si91x_i2s_transmit_data(i2s_driver_handle, data_out, BUFFER_SIZE)) {
      DEBUGOUT("I2S transmit start fail\r\n");
      break;
    }
    DEBUGOUT("I2S transmit start success\r\n");
  } while (false);
}
/*******************************************************************************
 * Function will run continuously in while loop
 ******************************************************************************/
void i2s_example_process_action(void)
{
  if ((send_complete && receive_complete)) {
    //Data has been transferred and received successfully
    //Validate the transmit and receive data count
    if ((sl_si91x_i2s_get_transmit_data_count(i2s_driver_handle) == BUFFER_SIZE)
        && (sl_si91x_i2s_get_receive_data_count(i2s_driver_handle) == BUFFER_SIZE)) {
      //I2S transfer completed
      DEBUGOUT("I2S transfer complete\r\n");
      //Compare transmit data and receive data
      compare_loop_back_data();
    }
    //reset send and receive complete status flags
    send_complete    = 0;
    receive_complete = 0;
  }
}

/*******************************************************************************
 * Function to configure PLL for high speed operations
 *
 * @param none
 * @return none
 ******************************************************************************/
static int32_t clock_configuration_pll(void)
{
  int32 status;
  do {
    // Configure the PLL frequency
    status = RSI_CLK_SetSocPllFreq(M4CLK, PS4_SOC_FREQ, SOC_PLL_REF_FREQUENCY);
    if (status != RSI_OK) {
      DEBUGOUT("\r\nFailed to configure SOC PLL Clock to 180Mhz,Error Code : %ld\r\n", status);
      break;
    }
    // Switch M4 clock to PLL clock for speed operations
    status = RSI_CLK_M4SocClkConfig(M4CLK, M4_SOCPLLCLK, 0);
    if (status != RSI_OK) {
      DEBUGOUT("\r\nFailed to Set M4 Clock as SOC PLL clock,Error Code : %ld\r\n", status);
      break;
    }
    status = ROMAPI_M4SS_CLK_API->clk_qspi_clk_config(M4CLK, QSPI_SOCPLLCLK, 0, 0, 0);
    if (status != RSI_OK) {
      DEBUGOUT("\r\nFailed to Set QSPI Clock as SOC PLL clock,Error Code : %ld\r\n", status);
      break;
    }
  } while (false);
  return status;
}

/*******************************************************************************
 * Function to compare the loop back data, i.e., after transfer it will compare
 * the send and receive data
 *
 * @param none
 * @return none
 ******************************************************************************/
static void compare_loop_back_data(void)
{
  uint16_t data_index = 0;
  for (data_index = 0; data_index < BUFFER_SIZE; data_index++) {
    // If the data in and data out are same, it will be continued else, the for
    // loop will be break.
    if (data_in[data_index] != data_out[data_index]) {
      break;
    }
  }
  if (data_index == BUFFER_SIZE) {
    DEBUGOUT("Data comparison successful, Loop Back Test Passed \n");
  } else {
    DEBUGOUT("Data comparison failed, Loop Back Test failed \n");
  }
}

/*******************************************************************************
 * Callback event function
 * It is responsible for the event which are triggered by GSPI interface
 * It updates the respective member of the structure as the event is triggered.
 * @param event - interrupt trigger event
 * @return none
 ******************************************************************************/
static void callback_event(uint32_t event)
{
  switch (event) {
    case SL_I2S_SEND_COMPLETE:
      send_complete = 1;
      break;
    case SL_I2S_RECEIVE_COMPLETE:
      receive_complete = 1;
      break;
    case SL_I2S_TX_UNDERFLOW:
      break;
    case SL_I2S_RX_OVERFLOW:
      break;
    case SL_I2S_FRAME_ERROR:
      break;
    default:
      break;
  }
}
