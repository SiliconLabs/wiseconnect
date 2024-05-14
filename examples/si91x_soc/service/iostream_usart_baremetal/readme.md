# IOSTREAM USART BAREMETAL

## Table of Contents

- [Purpose/Scope](#purposescope)
- [Overview](#overview)
- [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
  - [Hardware Requirements](#hardware-requirements)
  - [Software Requirements](#software-requirements)
- [Getting Started](#getting-started)
- [Application Build Environment](#application-build-environment)
  - [Pin Configuration](#pin-configuration)
- [Test the Application](#test-the-application)

## Purpose/Scope

  This application demonstrates how to configure IOSTREAM USART In asyncronous mode, it will send and receive data.

## Overview

This example project demonstrates the use of UART communication over the virtual COM port (VCOM) in a bare metal environment using I/O stream service. The example will echo back any charaters it receives over the serial connection. The VCOM serial port can be used over USB.

This application is configured with the following configs:
  - Tx and Rx enabled
  - Synchronous Master mode
  - 8 Bit data transfer
  - Stop bits 1
  - No Parity
  - No Auto Flow control
  - Baud Rates - 115200

## Prerequisites/Setup Requirements

### Hardware Requirements

- Windows PC
- Silicon Labs [Si917 Evaluation Kit WPK(BRD4002) + BRD4338A]

### Software Requirements

- Simplicity Studio
- Serial console Setup
  - The Serial Console setup instructions are provided [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/getting-started-with-soc-mode#perform-console-output-and-input-for-brd4338-a).

### Setup Diagram

![Figure: Introduction](resources/readme/setupdiagram.png)

> **Note** 
>- The power port will acts as a VCOM port.

## Getting Started

Refer instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) for the follwoing tasks:

- Install Studio and WiSeConnect 3 extension
- Connect your device to the computer
- Upgrade your connectivity firmware
- Create a Studio project

For details on the project folder structure, see the [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure) page.

## Application Build Environment

### Pin Configuration

- Pin configurations for external UART to TTL cable, When VCOM enable is Turned off.

  | USART PINS     | GPIO    | Connector(B0) |
  | -------------- | ------- | ------------- |
  | USART0_TX_PIN  | GPIO_30 |     P35       |
  | USART0_RX_PIN  | GPIO_29 |     P33       |

![Figure: Selecting Example project](resources/readme/image501b.png)

- Pin configurations for external UART to TTL cable, When VCOM enable is Turned ON.

  | USART PINS       | GPIO        | Connector(B0) |
  | ---------------- | ----------- | ------------- |
  | ULP_GPIO_TX_PIN  | ULP_GPIO_11 |     F6        |
  | ULP_GPIO_RX_PIN  | ULP_GPIO_9  |     F7        |

## Test the Application

Refer instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) for the following task:

 1. Make sure VCOM enable switch is turned on as shown below

     > ![Figure: UC2](resources/readme/image501e.png)

    > **Note** 
    >- Please note VCOM has fixed configuration for data transfer ( data bits = 8, stop bits = 1, parity = No parity, flow control = None). 
    >- Only Buad rate(Default buad rate is 115200) can be changed for VCOM form admin console using below command 
       ```c
    serial vcom config speed 115200
    ```
    > ![Figure: UC2](resources/readme/studio_admin_console.png)

    >- To work with different configuations than ( data bits = 8, stop bits = 1, parity = No parity, flow control = None), it's recommended to connect external TTL cable as shown below
    
    > ![Figure: UC2](resources/readme/ttl_module_connection.png)


2. Build the SI91x - iostream usart baremetal example in Studio.

3. Flash, run and debug the application.Follow the steps for successful execution of the application:
 
4. In softwate components, search for iostream and open the configuration window.

    > ![Figure: UC1](resources/readme/image501d.png)
  
   6. Can see the prints on UART VCOM and echos back the data what we send to usart.

   > ![Figure: output screen](resources/readme/outputConsoleI_IOSTREAM.png)

   > **Note** 
   >- To add iostream for debug prints in any examples, install below components **Wiseconnect_3_SDK/IOSTREAM Si91x**, **SERVICES/IO Stream: Si91x UART  - Give instance name vcom**
   >- When iostream components installed for debug prints then make sure DEBUG_UART macro is not present in preprocessor window.
