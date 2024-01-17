# SL ULP SSI MASTER

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
- [Pin Configuration](#pin-configuration-of-the-wpkbrd4002a-base-board)
- [Test the Application](#test-the-application)

## Purpose/Scope

- This application demonstrate the use of ULP SSI MASTER for data transfer in full duplex as well as half duplex mode in master mode.
- This application can run in synchronous mode with full-duplex operation
  - Master transmits data on MOSI pin and receives the same data on MISO pin
- This also supports send and receive data with any SSI slave, additionally it also supports DMA and non-DMA transfer.
- For half duplex communication, i.e., send and receive, master / slave connection is required.

## Overview

- SSI (Synchronous Serial Interface) is a synchronous, point-to-point, serial communication channel for digital data transmission.
- Synchronous data transmission is one in which the data is transmitted by synchronizing the transmission at the receiving and sending ends using a common clock signal.
- SSI is a synchronous four-wire interface consisting of two data pins(MOSI, MISO), a device select pin (CSN) and a gated clock pin(SCLK).
- With the two data pins, it allows for full-duplex operation with other SSI compatible devices.
- It supports full duplex Single-bit SPI master mode.
- It supports 6 modes:  
  - Mode 0: Clock Polarity is zero and Clock Phase is zero.
  - Mode 1: Clock Polarity is zero, Clock Phase is one.
  - Mode 2: Clock Polarity is one and Clock Phase is zero.
  - Mode 3: Clock Polarity is one and Clock Phase is one.
  - Mode-4: TEXAS_INSTRUMENTS SSI.
  - Mode-5: NATIONAL_SEMICONDUCTORS_MICROWIRE.
- The SPI clock is programmable to meet required baud rates
- It can generates interrupts for different events like transfer complete, data lost, mode fault.
- It supports up to 32K bytes of read data from a SSI device in a single read operation.
- It has support for DMA (Dynamic Memory Access).
- The ULP_SSI_MST in the MCU ULP peripherals supports Single-bit mode and can be connected to only one slave.

## About Example Code

- This example demonstrates SSI transfer i.e., full duplex communication and SSI send, SSI receive i.e., half duplex communication.
- Various parameters like SSI clock mode, bit-width, manual cs pin and SSI baudrate can be configured using UC. Also, Master or Slave or ULP Master DMA can be configured using UC.
- sl_si91x_ssi_config.h file contains the control configurations and sl_si91x_ssi_common_config.h contains DMA configuration selection.
- In the example code, first the output buffer is filled with some data which is transferred to the slave.
- Firmware version of API is fetched using \ref sl_si91x_ssi_get_version which includes release version, major version and minor version \ref sl_ssi_version_t.
- A static function is called to fill in the \ref sl_ssi_clock_config_t structure, which is passed in \ref sl_si91x_ssi_configure_clock API to configure the clock.
- \ref sl_si91x_ssi_init is used to initialize the peripheral, that includes pin configuration and it powers up the module.
- SSI instance must be passed in init to get the instance handle \ref sl_ssi_instance_t, which is used in other APIs.
- After initialization \ref sl_si91x_ssi_configure_power_mode is called to set the power mode \ref sl_ssi_power_state_t.
- All the necessary parameters are configured using \ref sl_si91x_ssi_set_configuration API, it expects a structure with required parameters \ref sl_ssi_control_config_t.
- After configuration, a callback register API is called to register the callback at the time of events \ref sl_si91x_ssi_register_event_callback.
- The State machine code is implemented for transfer, send and receive data, the current mode is determined by ssi_mode_enum_t which is declared in ulp_ssi_master_example.c file.
- According to the macro which is enabled, the example code executes the transfer of data:

- If **ULP_SSI_MASTER_TRANSFER** macro is enabled, it will transfer the data, i.e. send and receive data in full duplex mode.

  - The current_mode enum is set to ULP_SSI_MASTER_TRANSFER_DATA and calls the \ref sl_si91x_ssi_transfer_data API which expects data_out, data_in and number of data bytes to be transferred for sending and receiving data simultaneously (full duplex).
  - This test can also be performed in loopback state, i.e. connect MISO and MOSI pins.
  - The example code waits till the transfer is completed, when the transfer complete event is generated, it compares the sent and received data.
  - The result is printed on the console.
  - Now the current_mode enum is updated as per the macros enabled i.e., either ULP_SSI_MASTER_SEND or ULP_SSI_MASTER_RECEIVE.
  - If no other macros are enabled, the current_mode is updated to ULP_SSI_MASTER_TRANSMISSION_COMPLETED.

- If **ULP_SSI_MASTER_RECEIVE** macro is enabled, it only receives the data from slave - SPI slave must be connected, it cannot be tested in loopback mode.

  - The current_mode is set to the ULP_SSI_MASTER_RECEIVE_DATA and calls the \ref sl_si91x_ssi_receive_data API which expects data_in (empty buffer) and number of data bytes to be received.
  - It waits till the receive is completed i.e., transfer complete event is generated.
  - Now the current_mode enum is updated as per the macros enabled i.e., ULP_SSI_MASTER_SEND.
  - If no other macros are enabled, the current_mode is updated to ULP_SSI_MASTER_TRANSMISSION_COMPLETED.

- If **ULP_SSI_MASTER_SEND** macro is enabled, it only sends the data to slave, SPI slave must be connected, it cannot be tested in loopback mode.
  - The current_mode enum is set to ULP_SSI_MASTER_SEND_DATA and calls the \ref sl_si91x_ssi_send_data API which expects data_out (data buffer that needs to be sent) and number of bytes to send.
  - It waits till the send is completed i.e., transfer complete event is generated.
  - Now the current_mode enum is updated to ULP_SSI_MASTER_TRANSMISSION_COMPLETED.

## Prerequisites/Setup Requirements

### Hardware Requirements

- Windows PC
- Silicon Labs Si917 Evaluation Kit [WPK(BRD4002) + BRD4338A]

### Software Requirements

- Si91x
- Simplicity Studio
- Serial console Setup
  - The Serial Console setup instructions are provided below:
Refer [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/getting-started-with-soc-mode#perform-console-output-and-input-for-brd4338-a).

### Setup Diagram

 > ![Figure: Introduction](resources/readme/setupdiagram.png)

## Getting Started

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Install Studio and WiSeConnect 3 extension
- Connect your device to the computer
- Upgrade your connectivity firmware
- Create a Studio project

## Application Build Environment

- Configure UC from the slcp component.
- Open **sl_si91x_ulp_ssi_master.slcp** project file select **software component** tab and search for **SSI** in search bar.
- Using configuration wizard one can configure different parameters like:

   ![Figure: Introduction](resources/uc_screen/ssi_uc_screen.png)

  - **SSI Configuration**
    - Frame Format: SSI Frame Format can be configured, i.e.,
      - Mode 0: Clock Polarity is zero and Clock Phase is zero.
      - Mode 1: Clock Polarity is zero, Clock Phase is one.
      - Mode 2: Clock Polarity is one and Clock Phase is zero.
      - Mode 3: Clock Polarity is one and Clock Phase is one.
      - Mode-4: TEXAS_INSTRUMENTS SSI.
      - Mode-5: NATIONAL_SEMICONDUCTORS_MICROWIRE.
    - Bit Rate: The speed of transfer is configurable. The configuration range is from 500Kbps to 10Mbps in low power mode.
    - Data Width: The size of data packet. The configuration range from 4 to 16.
    - Mode: SSI mode/instance can be configurable, it can be configured Master/SLave/ULP Master.
    - Rx Sample Delay: Receive Data (rxd) Sample Delay, this to delay the sample of the rxd input signal. Each value represents a single SSI clock delay on the sample of the rxd signal. the configuration range from 0 to 63.
  - **DMA Configuration**
    - Master DMA: DMA enable for SSI master mode. it will interface with a DMA Controller using an optional set of DMA signals.
    - Slave DMA: DMA enable for SSI slave mode. it will interface with a DMA Controller using an optional set of DMA signals.
    - ULP Master DMA: DMA enable for ULP SSI master mode. it will interface with a DMA Controller using an optional set of DMA signals.
    - Tx FIFO Threshold: Transmit FIFO Threshold. Controls the level of entries (or below) at which the transmit FIFO controller triggers an interrupt. The configuration range from 0 to 15.
    - Rx FIFO Threshold: Receive FIFO Threshold. Controls the level of entries (or below) at which the receive FIFO controller triggers an interrupt. The configuration range from 0 to 15.
- Configuration files are generated in **config folder**, if not changed then the code will run on default UC values.

- Configure the following macros in ulp_ssi_master_example.h file and update/modify following macros if required.

```C
#define ULP_SSI_MASTER_TRANSFER ENABLE    // To use the transfer API
#define ULP_SSI_MASTER_SEND     DISABLE   // To use the send API
#define ULP_SSI_MASTER_RECEIVE  DISABLE   // To use the receive API
```

## Pin Configuration of the WPK[BRD4002A] Base Board

| GPIO pin           | Description              |
| ------------------ | ------------------------ |
| ULP_GPIO_8  [P15]  |RTE_SSI_ULP_MASTER_SCK_PIN|
| ULP_GPIO_10 [P17]  |RTE_SSI_ULP_MASTER_CS0_PIN|
| ULP_GPIO_1  [P16]  | ULP_SSI_MASTER_MOSI_PIN  |
| ULP_GPIO_2  [F10]  | ULP_SSI_MASTER_MISO_PIN  |

Note: Make sure pin configuration in RTE_Device_917.h file.(path: /$project/config/RTE_Device_917.h)

## Test the Application

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Build the SL ULP SSI MASTER example in Studio.
- Flash, run and debug the application

Follow the steps below for successful execution of the application:

1. Connect ULP SSI Master SCK, CS0, MOSI, MISO pins with the SSI Slave device.
2. When the application runs, it receives and send data.
3. After the transfer is completed, it validates the data and prints "Test Case Passed" on the console.
3. If ULP_SSI_MASTER_RECEIVE or ULP_SSI_MASTER_SEND is enabled, SSI slave will receive and send data respectively.
4. After successful program execution the prints in serial console looks as shown below.

- Master output:

   > ![output](resources/readme/output_ulp_ssi.png)

- Slave output:

   > ![output](resources/readme/output_ssi_slave.png)

> **Note:**
>
>- After Flashing ULP examples as M4 flash will be turned off,flash erase does not work.
>
- To Erase the chip follow the below procedure
  - **Press ISP and RESET button at same time and then release, now perform Chip erase through commander.**
