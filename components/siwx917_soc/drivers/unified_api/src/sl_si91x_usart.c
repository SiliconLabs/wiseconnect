/***************************************************************************/ /**
 * @file sl_si91x_usart.c
 * @brief USART API implementation
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

#include "sl_si91x_usart.h"
#include "sl_si91x_usart_config.h"

/*******************************************************************************
 ***************************  LOCAL MACROS   ********************************
 ******************************************************************************/
#define MAX_BAUDRATE          7372800 // Max baud rate supported
#define CONFIG_ENABLE         1       // Used to configure USART/UART enable should be one
#define USART_RELEASE_VERSION 0       // USART Release version
#define USART_SQA_VERSION     0       // USART SQA version
#define USART_DEV_VERSION     2       // USART Developer version
#define USART_UC \
  1 /*!< USART_UC is defined by default. when this macro (USART_UC) is defined, peripheral
                                             configuration is directly taken from the configuration set in the universal configuration (UC).
                                             if the application requires the configuration to be changed in run-time, undefined this macro
                                             and change the peripheral configuration through the \ref sl_si91x_usart_set_configuration API. */

/*******************************************************************************
 ***************************  Global  VARIABLES   ********************************
 ******************************************************************************/
extern ARM_DRIVER_USART Driver_USART0;
extern ARM_DRIVER_USART Driver_UART1;
extern ARM_DRIVER_USART Driver_ULP_UART;

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/
static sl_usart_signal_event_t user_callback = NULL;

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/
static uint32_t usart_get_baud_rate(uint8_t usart_module);
static sl_status_t usart_get_parity(uint8_t usart_module, usart_parity_typedef_t *parity);
static sl_status_t usart_get_stop_bit(uint8_t usart_module, usart_stopbit_typedef_t *stopbits);
static sl_status_t convert_arm_to_sl_error_code(int32_t error);
static sl_status_t validate_control_parameters(sl_si91x_usart_control_config_t *control_configuration,
                                               uint32_t *input_mode);
static sl_status_t usart_set_tx_rx_configuration(const void *usart_handle);
static void callback_event_handler(uint32_t event);
static sl_status_t usart_get_handle(usart_peripheral_t usart_instance, sl_usart_handle_t *usart_handle);
static boolean_t validate_usart_handle(sl_usart_handle_t usart_handle);

/*******************************************************************************
 **************************   Global function Definitions   *******************************
 ******************************************************************************/
/*******************************************************************************
 * @brief
 * Get the USART handle
 *
 * @details
 * This function update the usart handle based on usart instance if not valid usart
 * instance then returns the invalid parameter error code
 ******************************************************************************/
static sl_status_t usart_get_handle(usart_peripheral_t usart_instance, sl_usart_handle_t *usart_handle)
{
  sl_status_t status;
  do {
    // Check for USART instance valid
    if (usart_instance >= UARTLAST) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // If instance is USART0 then update the USART handle USART0
    if (usart_instance == USART_0) {
      *usart_handle = &Driver_USART0;
      status        = SL_STATUS_OK;
      break;
    }
    // If instance is UART1 then update the USART handle UART1
    if (usart_instance == UART_1) {
      *usart_handle = &Driver_UART1;
      status        = SL_STATUS_OK;
      break;
    }
    // If instance is ULP_UART then update the USART handle with ULP UART
    if (usart_instance == ULPUART) {
      *usart_handle = &Driver_ULP_UART;
      status        = SL_STATUS_OK;
      break;
    }
  } while (false);
  return status;
}

/*******************************************************************************
 * @brief
 * Validate the USART handle is valid
 *
 * @details
 * This function checks the usart handle is valid or not if it's valid handle then
 * returns the SL_STATUS_OK else returns the error code
 ******************************************************************************/
static boolean_t validate_usart_handle(sl_usart_handle_t usart_handle)
{
  boolean_t status;
  // Check for USART instance valid
  if ((usart_handle == &Driver_USART0) || (usart_handle == &Driver_UART1) || (usart_handle == &Driver_ULP_UART)) {
    status = true;
  }
  // if usart instance is ULP_UART then check usart_handle is ULP_UART
  else {
    status = false;
  }
  return status;
}

/*******************************************************************************
 * @brief
 * Initialization of USART/UART interface
 *
 * @details
 * Initializes and Configures the USART base clock
 * If DMA is enabled then Initializes the DMA .
 * It will take callback event as input argument and call the callback function
 * on usart data transfer abd reception
 ******************************************************************************/
sl_status_t sl_si91x_usart_init(usart_peripheral_t usart_instance, sl_usart_handle_t *usart_handle)
{
  sl_status_t status;
  int32_t error_status;
  sl_usart_handle_t usart_temp_handle = NULL;

  do {
    // Get the USART Init state
    if (USART_GetInitState(usart_instance)) {
      status = SL_STATUS_BUSY;
      break;
    }
    // Check the USART Handle for NUll
    if (usart_handle == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // To validate the structure pointer and usart handle, if the parameters is NULL, it
    // returns an error code
    if (*usart_handle != NULL) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // Get the USART Handle
    usart_get_handle(usart_instance, &usart_temp_handle);
    *usart_handle = usart_temp_handle;

    // CMSIS API for initialization is called with the function address of
    // callback function, and the arm error code returned from the API is
    // converted to SL error code via convert_arm_to_sl_error_code function.
    error_status = ((sl_usart_driver_t *)usart_temp_handle)->Initialize(callback_event_handler);
    status       = convert_arm_to_sl_error_code(error_status);
  } while (false);
  return status;
}

/*******************************************************************************
 * @brief
 * De-init the USART
 *
 * @details
 * This function will disable the clocks for USART/UART module and also
 * Deinit the DMA for UART/USART if DMA is enabled for data transfers.
 * Validates the error code returns return the status.
 *
 * @note
 *  To enable debug, DEBUGINIT() needs to be called after using this API.
 ******************************************************************************/
sl_status_t sl_si91x_usart_deinit(sl_usart_handle_t usart_handle)
{
  sl_status_t status;
  int32_t error_status;
  do {
    // Check for USART handle parameter, if NULL return from here
    if (usart_handle == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // Validate USART handle
    if (!validate_usart_handle(usart_handle)) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    //Deinit the USART/UART
    error_status = ((sl_usart_driver_t *)usart_handle)->Uninitialize();
    status       = convert_arm_to_sl_error_code(error_status);
  } while (false);
  return status;
}

/*******************************************************************************
 * @brief
 * To set the power mode of USART
 *
 * @details
 * It will set the power mode of usart
 *          - powerOff
 *          - powerLow
 *          - powerFull
 *
 * In poweroff power state will disable the interrupt , disable DMA channel if DMA mode
 * disable the power to the USART block.So no usart operations are possible when set
 * this mode
 *
 * powerLow power state is not supported
 *
 * In powerFull power state will enable the intterupt and set the flags initialization
 * and power on flag so that USART data teransfer can takes place
 *******************************************************************************/
sl_status_t sl_si91x_usart_set_power_mode(sl_usart_handle_t usart_handle, power_mode_typedef_t state)
{
  sl_status_t status;
  int32_t error_status;
  do {
    // Check for USART handle parameter, if NULL return from here
    if (usart_handle == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // Check for power state
    if (state >= SL_POWER_MODE_LAST) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // Validate USART handle
    if (!validate_usart_handle(usart_handle)) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // Set the USART power state
    error_status = ((sl_usart_driver_t *)usart_handle)->PowerControl((ARM_POWER_STATE)state);
    status       = convert_arm_to_sl_error_code(error_status);

  } while (false);
  return status;
}

/*******************************************************************************
 * To register the event callback
 * It registers the callback, i.e., stores the callback function address
 * and pass to the variable that is called in Interrupt Handler.
 * If another callback is registered without unregistering previous callback then, it
 * returns an error code, so it is mandatory to unregister the callback before registering
 * another callback.
 * It will returns error if any callback is already registered.
 ******************************************************************************/
sl_status_t sl_si91x_usart_register_event_callback(sl_usart_signal_event_t callback_event)
{
  sl_status_t status;
  do {
    // Validates the null pointer, if true returns error code
    if (callback_event == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // To validate the function pointer if the parameters is not NULL then, it
    // returns an error code
    if (user_callback != NULL) {
      status = SL_STATUS_BUSY;
      break;
    }
    // User callback address is passed to the static variable which is called at the time of
    // interrupt
    user_callback = callback_event;
    // Returns SL_STATUS_OK if callback is successfully registered
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * It unregisters the callback, i.e., clear the callback function address
 * and pass NULL value to the variable
 ******************************************************************************/
void sl_si91x_usart_unregister_event_callback(void)
{
  // Pass the NULL value to the static variable which is called at the time of
  // interrupt.
  // It is further validated in register callback API.
  user_callback = NULL;
}

/*******************************************************************************
 * @brief
 * To send the data when USART/UART is configured
 *
 * @details
 * This function sends the data on USART transmitter
 * It takes two arguments,
 *   - data: pointer to the data buffer which will store the data that needs to
 *           be sent
 *   - data_legth: The number of data items to be send
 ******************************************************************************/
sl_status_t sl_si91x_usart_send_data(sl_usart_handle_t usart_handle, const void *data, uint32_t data_length)
{
  sl_status_t status;
  int32_t error_status;
  do {
    // Check USART handle and data parameters, if NULL return from here
    if ((usart_handle == NULL) || (data == NULL)) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // Check for data length
    if (data_length == 0) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // Validate USART handle
    if (!validate_usart_handle(usart_handle)) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // Send the data available in data pointer parameter
    error_status = ((sl_usart_driver_t *)usart_handle)->Send(data, data_length);
    status       = convert_arm_to_sl_error_code(error_status);
  } while (false);
  return status;
}

/*******************************************************************************
 * @brief
 * To receive the data when USART/UART is configured
 *
 * @details
 * This function receives the data received on USART receiver
 * It takes two arguments,
 *   - data: pointer to the data buffer which stores the data received
 *   - data_legth: The number of data length to receive
 *
 * Stores the data received in data pointer
 ******************************************************************************/
sl_status_t sl_si91x_usart_receive_data(sl_usart_handle_t usart_handle, void *data, uint32_t data_length)
{
  sl_status_t status;
  int32_t error_status;
  do {
    // Check USART handle and data parameters, if NULL return from here
    if ((usart_handle == NULL) || (data == NULL)) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // Check for data length
    if (data_length == 0) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // Validate USART handle
    if (!validate_usart_handle(usart_handle)) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // Receive the data from USART receiver and point it the data pointer received as argument
    error_status = ((sl_usart_driver_t *)usart_handle)->Receive(data, data_length);
    status       = convert_arm_to_sl_error_code(error_status);
  } while (false);
  return status;
}

/*******************************************************************************
 * @brief
 * To send and receive the data when USART in Synchronous mode is enabled
 *
 * @details
 * This function sends and receives the data to and from usart transmitter and
 * receiver in synchronous mode of operation
 * It takes three arguments,
 *   - data_in: pointer to the data buffer which will store the received data
 *   - data_out: pointer to the data buffer which will store the data that needs to
 *               be sent
 *   - data_legth: The number of data items to be transferred
 *
 * It will store the received data in the data_in buffer
 *
 * @note
 * This function use only in syncronpus mode
 ******************************************************************************/
sl_status_t sl_si91x_usart_transfer_data(sl_usart_handle_t usart_handle,
                                         const void *data_out,
                                         void *data_in,
                                         uint32_t data_length)
{
  sl_status_t status;
  int32_t error_status;
  do {
    // Check USART handle data in and data out parameters, if NULL return from here
    if ((usart_handle == NULL) || (data_in == NULL) || (data_out == NULL)) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // Check for Data length
    if (data_length == 0) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // Validate USART handle
    if (!validate_usart_handle(usart_handle)) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // Transfers and Receives the data in USART synch mode
    error_status = ((sl_usart_driver_t *)usart_handle)->Transfer(data_out, data_in, data_length);
    status       = convert_arm_to_sl_error_code(error_status);

  } while (false);
  return status;
}

/*******************************************************************************
 * @brief
 * To get the TX data count of USART
 *
 * @details
 * It will return Transferred data bytes by USART/UART transmitter
 ******************************************************************************/
uint32_t sl_si91x_usart_get_tx_data_count(sl_usart_handle_t usart_handle)
{
  return ((sl_usart_driver_t *)usart_handle)->GetTxCount();
}

/*******************************************************************************
 * @brief
 * To get the RX data count of USART
 *
 * @details
 * It will return RX data bytes by USART/UART receiver
 ******************************************************************************/
uint32_t sl_si91x_usart_get_rx_data_count(sl_usart_handle_t usart_handle)
{
  return ((sl_usart_driver_t *)usart_handle)->GetRxCount();
}

/*******************************************************************************
 * @brief
 * To control and configure the USART
 *
 * @details
 * It takes argument of pointer to the structure of control configuration and
 * baud rate.
 * It will configure the different configurations such as  USART mode, Data Bits,
 * Parity , stop bits, flow control ,baud rate,
 * - USART mode - mode of operation
 *              - usartAsynchMode
 *              - usartSynchModeMaster
 *              - usartSynchModeSlave
 *              - usartModeSingleWire
 *              - usartModeIrda
 * - Data Bits - Set the no of data bits in frame format
 *             - usartDatabits5
 *             - usartDatabits6
 *             - usartDatabits7
 *             - usartDatabits8
 *             - usartDatabits9
 * - Parity   - Set the parity bit
 *             - usartNoParity
 *             - usartEvenParity
 *             - usartOddParity
 * - Stop bits - Set the stop bits
 *             - ARM_USART_STOP_BITS_1
 *             - ARM_USART_STOP_BITS_2
 *  @note
 *  ARM_USART_STOP_BITS_0_5 is not supported in driver
 *  Even we set ARM_USART_STOP_BITS_1_5 driver will consider the
 *          ARM_USART_STOP_BITS_2
 *
 * - Flow control -Set the usart flow control in synchronous mode
 *                - usartHwFlowControlNone
 *                - usartHwFlowControlCts
 *                - usartHwFlowControlRts
 *                - usartHwFlowControlCtsAndRts
 * - Baud rate -Set the usart baud rate
 *
 * @note
 * Call the sl_si91x_usart_set_tx_rx_configuration() to set the tx and rx
 * lines before this API
 ********************************************************************************/
sl_status_t sl_si91x_usart_set_configuration(sl_usart_handle_t usart_handle,
                                             sl_si91x_usart_control_config_t *control_configuration,
                                             uint32_t baud_rate)
{
  sl_status_t status;
  int32_t error_status;
  uint32_t input_mode = false;
  /* USART_UC is defined by default. when this macro (USART_UC) is defined, peripheral
   * configuration is directly taken from the configuration set in the universal configuration (UC).
   * if the application requires the configuration to be changed in run-time, undefined this macro
   * and change the peripheral configuration through the sl_si91x_usart_set_configuration API.
   */
#ifdef USART_UC
  control_configuration = &usart_configuration;
  baud_rate             = usart_configuration.baudrate;
#endif
  do {
    // Check USART handle and control_configuration parameter, if NULL return from here
    if ((usart_handle == NULL) || (control_configuration == NULL)) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // Check for Max Baud rate supported
    if (baud_rate > MAX_BAUDRATE) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // Validate USART handle
    if (!validate_usart_handle(usart_handle)) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // Configure the USART Transfer and receive line
    status = usart_set_tx_rx_configuration(usart_handle);
    if (status != SL_STATUS_OK) {
      break;
    }
    // Validate the control parameters and combine in single variable
    status = validate_control_parameters(control_configuration, &input_mode);
    if (status != SL_STATUS_OK) {
      break;
    }
    // Configure the USART parameters
    error_status = ((sl_usart_driver_t *)usart_handle)->Control(input_mode, baud_rate);
    status       = convert_arm_to_sl_error_code(error_status);
  } while (false);
  return status;
}

/*******************************************************************************
 * @brief
 * Configures the USART Tx and Rx lines
 *
 * @details
 * This functions configures the usart tx and rx lines for data transfer
 * Takes the argument config enable to decide Tx and Rx lines configure or not
 * Validate the error code return from drives and return SL_STATUS
 ********************************************************************************/
static sl_status_t usart_set_tx_rx_configuration(sl_usart_handle_t usart_handle)
{
  sl_status_t status;
  int32_t error_status;
  do {
    // Check USART handle parameter, if NULL return from here
    if (usart_handle == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // Set the USART Tx control line
    error_status = ((sl_usart_driver_t *)usart_handle)->Control(SL_USART_CONTROL_TX, CONFIG_ENABLE);
    status       = convert_arm_to_sl_error_code(error_status);
    if (status != SL_STATUS_OK) {
      break;
    }
    // Set the USART Rx control line
    error_status = ((sl_usart_driver_t *)usart_handle)->Control(SL_USART_CONTROL_RX, CONFIG_ENABLE);
    status       = convert_arm_to_sl_error_code(error_status);
  } while (false);
  return status;
}

/*******************************************************************************
 * @brief
 * To get the USART status
 *
 * @details
 * This function returns the sl_usart_status_t structure with usart status info about
 * tx_busy ,rx_busy, tx_inderfloe ,rx_overflow ,rx_break, rx_framing_error and
 * rx_parity_error
 ******************************************************************************/
sl_usart_status_t sl_si91x_usart_get_status(sl_usart_handle_t usart_handle)
{
  return ((sl_usart_driver_t *)usart_handle)->GetStatus();
}

/*******************************************************************************
 * @brief
 * To set the modem control line in USART/UART
 *
 * @details
 * It takes argument as structure of sl_usart_modem_control_t type
 * It validates the error returned from driver and return the status of operation.
 ******************************************************************************/
sl_status_t sl_si91x_usart_set_modem_control(sl_usart_handle_t usart_handle, sl_usart_modem_control_t control)
{
  sl_status_t status;
  int32_t error_status;
  do {
    // Check USART handle parameter, if NULL return from here
    if (usart_handle == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // Validate USART handle
    if (!validate_usart_handle(usart_handle)) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // Set the USART Modem control state received as argument
    error_status = ((sl_usart_driver_t *)usart_handle)->SetModemControl(control);
    status       = convert_arm_to_sl_error_code(error_status);
  } while (false);
  return status;
}

/*******************************************************************************
 * @brief
 * To get the modem control status of USART/UART
 *
 * @details
 * This function gets the usart modem control status such as modem control lines
 * are active or not.
 * It return the structure with information about status of modem lines
 ******************************************************************************/
sl_usart_modem_status_t sl_si91x_usart_get_modem_status(sl_usart_handle_t usart_handle)
{
  return ((sl_usart_driver_t *)usart_handle)->GetModemStatus();
}

/*******************************************************************************
 * @brief
 * To get the release, sqa and dev version of USART
 * It returns the structure for USART version.
 * Structure includes three members:
 * - Release version
 * - SQA version
 * - Dev version
 ******************************************************************************/
sl_usart_version_t sl_si91x_usart_get_version(void)
{
  sl_usart_version_t version;
  version.minor   = USART_DEV_VERSION;
  version.release = USART_RELEASE_VERSION;
  version.major   = USART_SQA_VERSION;
  return version;
}

/*******************************************************************************
 * @brief
 * Get the usart configurations set
 *
 * @details
 * Get the USART configurations set in the module such as baudrate ,parity bit
 * stop bits etc
 *
 * @note
 * This function should call after USART Initliazation ,configuration
 *******************************************************************************/
sl_status_t sl_si91x_usart_get_configurations(uint8_t usart_module, sl_si91x_usart_control_config_t *usart_config)
{
  sl_status_t status;
  do {
    if (usart_config == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // Get the usart buadate
    usart_config->baudrate = usart_get_baud_rate(usart_module);
    // Get the USART parity bit set
    status = usart_get_parity(usart_module, &usart_config->parity);
    if (status != SL_STATUS_OK) {
      break;
    }
    //Get the usart stop bit set
    status = usart_get_stop_bit(usart_module, &usart_config->stopbits);
    if (status != SL_STATUS_OK) {
      break;
    }
  } while (false);
  return status;
}

/*******************************************************************************
 * @brief
 * Get the UART/USART baud rate programmed
 *
 * @details
 * This function return the buadrate set in the module
 *
 * @note
 * This function should call after USART Initliazation ,configuration
 ******************************************************************************/
static uint32_t usart_get_baud_rate(uint8_t usart_module)
{
  return USART_GetBaudrate(usart_module);
}

/*******************************************************************************
 * @brief
 * Get the UART/USART parity bit set in the module
 *
 * @details
 * It takes argument as usart peripheral i.e USART0,UART1 or ULP_UART and
 * enum address to store the parity bit set.
 * if BIT(4) and BIT(3) are set
 *      00 - No parity
 *      01 - Odd Parity
 *      11 - Even Parity
 *
 * @note
 * This function should call after USART Initliazation ,configuration
 ******************************************************************************/
static sl_status_t usart_get_parity(uint8_t usart_module, usart_parity_typedef_t *parity)
{
  uint32_t parity_bit;
  sl_status_t status;
  do {
    if (usart_module >= UARTLAST) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // Get the parity bit data
    parity_bit = USART_GetParity_StopBit(usart_module);
    // Check the BIT(4) AND bit(3) to decide which parity is set
    if (parity_bit & BIT(3)) {
      ((parity_bit & BIT(4)) ? (*parity = SL_USART_EVEN_PARITY) : (*parity = SL_USART_ODD_PARITY));
    } else {
      // BIT(3) and BIT(4) both are 0 then No parity
      (*parity = SL_USART_NO_PARITY);
    }
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * @brief
 * Get USART stop bit set
 *
 * @details
 * This function is used to get the usart stop bit set ,It takes argument as usart
 * peipheral i.e USART0,UART1 or ULP_UART and enum address to store the stopbits
 *
 * @note
 * This function should call after USART Initliazation ,configuration
 ******************************************************************************/
static sl_status_t usart_get_stop_bit(uint8_t usart_module, usart_stopbit_typedef_t *stopbits)
{
  uint32_t stopbit;
  sl_status_t status;
  do {
    if (usart_module >= UARTLAST) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // Get the stop bit data from h/w
    stopbit = USART_GetParity_StopBit(usart_module);
    // if bit(2) is set then usartStopbits2 and bit(2) is 0 then usartStopbits1
    (stopbit & BIT(2)) ? (*stopbits = SL_USART_STOP_BITS_2) : (*stopbits = SL_USART_STOP_BITS_1);
    status = SL_STATUS_OK;
  } while (false);
  // Return status
  return status;
}

/*******************************************************************************
 * @brief
 * To validate the ARM error code
 *
 * @details
 * While calling the CMSIS APIs, it will return the ARM Error codes.
 * This function will convert the ARM error codes into SL error codes.
 * It takes argument as 32 bit signed integer and returns the SL error type.
 * It has a single switch statement which is mapped with the SL error code and
 * after successful conversion it will break the switch statement.
 * If the error code is not listed, by default it will be SL_STATUS_FAIL.
 ******************************************************************************/
static sl_status_t convert_arm_to_sl_error_code(int32_t error)
{
  sl_status_t status;
  switch (error) {
    //Driver Errors
    case ARM_DRIVER_OK:
      status = SL_STATUS_OK;
      break;
    case ARM_DRIVER_ERROR:
      status = SL_STATUS_FAIL;
      break;
    case ARM_DRIVER_ERROR_BUSY:
      status = SL_STATUS_BUSY;
      break;
    case ARM_DRIVER_ERROR_TIMEOUT:
      status = SL_STATUS_TIMEOUT;
      break;
    case ARM_DRIVER_ERROR_UNSUPPORTED:
      status = SL_STATUS_NOT_SUPPORTED;
      break;
    case ARM_DRIVER_ERROR_PARAMETER:
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    case ARM_DRIVER_ERROR_SPECIFIC:
      status = SL_STATUS_FAIL;
      break;

    case ARM_USART_ERROR_MODE:
      status = SL_STATUS_INVALID_MODE;
      break;
    case ARM_USART_ERROR_BAUDRATE:
      status = SL_STATUS_INVALID_RANGE;
      break;
    case ARM_USART_ERROR_DATA_BITS:
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    case ARM_USART_ERROR_PARITY:
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    case ARM_USART_ERROR_STOP_BITS:
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    case ARM_USART_ERROR_FLOW_CONTROL:
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    case ARM_USART_ERROR_CPOL:
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    case ARM_USART_ERROR_CPHA:
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    default:
      status = SL_STATUS_FAIL;
      break;
  }
  return status;
}

/*******************************************************************************
 * @brief
 * To validate the control parameters and build a 32 bit integer
 *
 * @details
 * It takes pointer to control configuration structure and pointer to the uint32_t
 * as argument and builds the integer using the validations and 'OR' operation.
 * According to the values in control configuration structure, it will 'OR' the values.
 ******************************************************************************/
static sl_status_t validate_control_parameters(sl_si91x_usart_control_config_t *control_configuration,
                                               uint32_t *input_mode)
{
  sl_status_t status;
  do {
    if ((control_configuration == NULL) || (input_mode == NULL)) {
      status = SL_STATUS_NULL_POINTER;
    }
    // Append all the input control parameters
    *input_mode =
      (*input_mode | control_configuration->mode | control_configuration->parity | control_configuration->stopbits
       | control_configuration->hwflowcontrol | control_configuration->databits | control_configuration->misc_control);
  } while (false);
  status = SL_STATUS_OK;
  return status;
}

/*******************************************************************************
 * Static callback function for handling the callbacks
 ******************************************************************************/
static void callback_event_handler(uint32_t event)
{
  switch (event) {
    case SL_USART_EVENT_SEND_COMPLETE:
      break;
    case SL_USART_EVENT_RECEIVE_COMPLETE:
      break;
    case SL_USART_EVENT_TRANSFER_COMPLETE:
      break;
  }
  user_callback(event);
}
