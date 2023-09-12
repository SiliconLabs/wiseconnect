# ULP USART

## Introduction

- Universal Synchronous Asynchronous Receiver-Transmitter (USART)
- This application demonstrates how to configure USART In asyncronous mode, it will send and receive data in loopback mode

## Overview

- USART is used in communication through wired medium in both Synchronous and Asynchronous fashion. It enables the device to
  communicate using serail protocols
- This application is configured with following configs
  - Tx and Rx enabled
  - Asynchronous mode
  - 8 Bit data transfer
  - Stop bits 1
  - No Parity
  - No Auto Flow control
  - Baud Rates - 115200

## About Example Code

- \ref usart_example.c this example code demonstates how to configure the usart to send and receive data in loopback mode
- In this example, first usart get initialized if it's not initialized already with clock and dma configurations if dma is
  enalbed using \ref sl_si91x_usart_init
- After USART initialization ,sets the usart power mode using \ref sl_si91x_usart_set_power_mode() and configures the USART
  with default configurations from UC and usart transmitt and receive lines using \ref sl_si91x_usart_set_configuration()
- Then register's user event callback for send ,recevie and transfer complete notification using
  \ref sl_si91x_usart_register_event_callback()
- After user event callback registered data send and receive can happen through \ref sl_si91x_usart_send_data() and
  \ref sl_si91x_usart_receive_data() respectively
- Once the receive data event triggered ,compares both transmitt and receive buffers to confirm the received data if data is
  same then it prints the Data comparison successful, Loop Back Test Passed on the usart console.

## Running Example code

- To use this application following Hardware, Software and the Project Setup is required.

### Hardware Setup

- Windows PC
- Silicon Labs [WSTK + BRD4338A]

![Figure: Introduction](resources/readme/image513a.png)

### Software Setup

- Si91x SDK
- Embedded Development Environment
  - For Silicon Labs Si91x, use the latest version of Simplicity Studio (refer **"Download and Install Simplicity Studio"** section in **getting-started-with-siwx917-soc** guide at **release_package/docs/index.html**)
### VCOM Setup
- The Docklight tool's setup instructions are provided below..

![Figure: VCOM_setup](resources/readme/vcom.png)

## Project Setup

- **Silicon Labs Si91x** refer **"Download SDK"** section in **getting-started-with-siwx917-soc** guide at **release_package/docs/index.html** to work with Si91x and Simplicity Studio

## Loading Application on Simplicity Studio

1. With the product Si917 selected, navigate to the example projects by clicking on Example Projects & Demos
   in simplicity studio and click on to SI91x - SoC USART Example application as shown below.

![Figure: Selecting Example project](resources/readme/image513b.png)

## Build

1. Compile the application in Simplicity Studio using build icon
   ![Figure: Build run and Debug](resources/readme/image513c.png)

Note:
To run the application usart master mode do the following changes

1. Enable RTE_USART_MODE and RTE_CONTINUOUS_CLOCK_MODE in RTE_Device_9117.h(path: /$project/wiseconnect_1.0.0/platforms/si91x/drivers/cmsis_driver/config/RTE_Device_9117.h)
2. Connect Master and slave as per pin configurations and ensure slave to be configured properly before executing master code

## Device Programming

- To program the device ,refer **"Burn M4 Binary"** section in **getting-started-with-siwx917-soc** guide at **release_package/docs/index.html** to work with Si91x and Simplicity Studio

## Pin Configuration 

|GPIO pin  | Description|
|----------| ---------- |
|ULP_GPIO_11 [F6]   |TX|
|ULP_GPIO_9  [F7]   |RX|
|PIN46       [P24]   |GPIO_Toggle|

![Figure: Build run and Debug](resources/readme/image513d.png)

## Executing the Application
1. When the application runs,USART sends and receives data in full duplex mode

## Expected Results 
 - When tx and rx data both are matching PIN46 should be toggled ,connect logic analyser to observe the toggle state. 
 - Here same PINS which are used to send and recive the data where used for data transfer so we cannot able to see prins 
 we can use GPIO toggling instead.
## Note
 - This applicatin is executed from RAM.
 - In this application while changing the MCU mode from PS4 to PS2, M4 flash will be turned off.
 - The debug feature of Simplicity Studio will not work after M4 flash is turned off.
## Expected Scenario:
 - After Flashing ULP examples as M4 flash will be turned off,flash erase does not work.
 - To Erase the chip follow the below procedure
   - Turn ON ISP switch and press the reset button → Turn OFF ISP Switch → Now perform Chip erase 
      through commander.
