# SL USART ASYNCHRONOUS

## Table of Contents

- [Purpose/Scope](#purposescope)
- [Overview](#overview)
- [About Example Code](#about-example-code)
- [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
  - [Hardware Requirements](#hardware-requirements)
  - [Software Requirements](#software-requirements)
  - [Setup Diagram](#setup-diagram)
- [Getting Started](#getting-started)
- [Application Build Environment](#application-build-environment)
- [Test the Application](#test-the-application)

## Purpose/Scope

This application demonstrates how to configure Universal Synchronous Asynchronous Receiver-Transmitter (USART) In Asynchronous mode. It will send and receive data from serial console.

## Overview

- USART is used in communication through wired medium in both Synchronous and Asynchronous fashion. It enables the device to communicate using serial protocols.
- This application is configured with following configurations:
  - Tx and Rx enabled
  - Asynchronous mode
  - 8 Bit data transfer
  - Stop bits 1
  - No Parity
  - No Auto Flow control
  - Baud Rates - 115200

## About Example Code

- \ref usart_example.c - This example code demonstrates how to configure the USART to send and receive data.
- In this example, first USART get initialized if it is not initialized already with clock and DMA configurations if DMA is   enabled using \ref sl_si91x_usart_init
- After USART initialization, the USART power mode is set using \ref sl_si91x_usart_set_power_mode(), and then USART is configured with the default configurations from UC along with the USART transmit and receive lines using \ref sl_si91x_usart_set_configuration().
- Then the register user event callback for send and receive complete notification is set using \ref sl_si91x_usart_register_event_callback()
- After setting the user event callback, the data send and receive can happen through \ref sl_si91x_usart_send_data() and \ref sl_si91x_usart_receive_data() respectively.
- Once the receive data event is triggered, both transmit and receive buffer data is compared to confirm if the received data is the same.

## Prerequisites/Setup Requirements

### Hardware Requirements

- Windows PC
- Silicon Labs Si917 Evaluation Kit [WPK(4002A) + BRD4338A / BRD4342A / BRD4343A ]
- SiWx917 AC1 Module Explorer Kit (BRD2708A)

### Software Requirements

- Simplicity Studio
- Serial console setup
  - For serial console setup instructions, refer [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#console-input-and-output).

### Setup Diagram

> ![Figure: Introduction](resources/readme/setupdiagram.png)

## Getting Started

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- [Install Simplicity Studio](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#install-simplicity-studio)
- [Install WiSeConnect 3 extension](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#install-the-wi-se-connect-3-extension)
- [Connect your device to the computer](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#connect-si-wx91x-to-computer)
- [Upgrade your connectivity firmware](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#update-si-wx91x-connectivity-firmware)
- [Create a Studio project](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#create-a-project)

For details on the project folder structure, see the [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure) page.

## Application Build Environment

**Configuration of USART at UC (Universal Configuration)**

- Configure UC from the slcp component.
- Open the **sl_si91x_usart_async.slcp** project file, select the **Software Component** tab, and search for **USART** in the search bar.
- You can use the configuration wizard to configure different parameters. The following configuration screen illustrates where the user can select as per their requirements.

  > ![Figure: Selecting UC](resources/uc_screen/usart_uc.png)

## Pin Configuration

  | USART PINS     | GPIO    | Breakout pin  | Explorer kit Breakout pin | UART-TTL cable |
  | -------------- | ------- | ------------- | -------------- | ------------- |
  | USART0_TX_PIN  | GPIO_30 |     P35       |      [RST]     | RX pin         |
  | USART0_RX_PIN  | GPIO_29 |     P33       |      [AN]      | TX Pin         |

 ![Figure: Build run and Debug](resources/readme/image513d.png)

## Flow Control Configuration

- Set the SL_USART_FLOW_CONTROL_TYPE parameter to SL_USART_FLOW_CONTROL_RTS_CTS to enable USART flow control.
- Make the following two macros in `RTE_Device_917.h` or `RTE_Device_915.h`  to '1' to map RTS and CTS pins to WSTK/WPK Main Board EXP header or breakout pins.

  ```C
  #define RTE_USART0_CTS_PORT_ID    1
  #define RTE_USART0_RTS_PORT_ID    1
  ```

  | USART PINS     | GPIO    | Breakout pin  | Explorer kit Breakout pin|
  | -------------- | ------- | ------------- | ------------------------ |
  | USART0_CTS_PIN | GPIO_26 |     P27       |           [MISO]         |
  | USART0_RTS_PIN | GPIO_28 |     P31       |           [CS]           |

## Test the Application

1. Connect TX pin (GPIO_30) to RX pin of UART to TTL cable and RX pin (GPIO_29) to TX pin of UART-TTL cable.
2. When the application runs, USART sends and receives data in full-duplex mode.
3. Observe the USART transmission status upon data transmission from USART master to slave and vice-versa on USART pins.
4. After running this application, the following console output can be observed.

    > ![Figure: expected result](resources/readme/output_console_usart_async.png)
>
> **Note**:
>
>- Add data_in buffer to watch window for checking receive data.

## Configuring higher clock

For baud rates higher than 2 million, change the clock source to USART_SOCPLLCLK in `RTE_Device_917.h` (/$project/config/RTE_Device_917.h):

    ```c
    #define RTE_USART0_CLK_SRC   
    ```

> **Note:**
>
> Interrupt handlers are implemented in the driver layer, and user callbacks are provided for custom code. If you want to write your own interrupt handler instead of using the default one, make the driver interrupt handler a weak handler. Then, copy the necessary code from the driver handler to your custom interrupt handler.
> By default, Request to Send (RTS) and Clear to Send (CTS) flow control signals are disabled in the UART driver UC, and their corresponding GPIO pins are not assigned in the Pintool. If you enable RTS/CTS in the Driver UC, you must manually configure and assign the appropriate GPIO pins in the Pintool to ensure proper hardware flow control functionality.
