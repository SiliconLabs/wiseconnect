# SL I2C FOLLOWER

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
  - [Pin Configuration](#pin-configuration)
- [Test the Application](#test-the-application)

## Purpose/Scope

- This application deals with low level driver example and demonstrates the I2C will be configured in follower mode. The SCL and SDA lines of leader controller are connected to Follower's SCL and SDA pins.
After transmission the data is compared and result is printed on the console.

> **Note:** 
>
>- The master-slave terminology is now replaced with leader-follower. Master is now recognized as Leader and slave is now recognized as Follower.
>- Here the SCL and SDA lines of follower are configured as internal pull-up.
>- Data is transferred from leader to follower and follower to leader. 

## Overview

- There are three configurable I2C Master/Slave controllers in M4 - two in the MCU HP peripherals (I2C1, I2C2) and one in the MCU ULP subsystem (ULP_I2C).
- The I2C interface allows the processor to serve as a leader or follower on the I2C bus.
- I2C can be configured with following features
  - I2C standard compliant bus interface with open-drain pins
  - Configurable as Leader or Follower
  - Four speed modes: Standard Mode (100 kbps), Fast Mode (400 kbps), Fast Mode Plus (1Mbps) and High-Speed Mode (3.4 Mbps)
  - 7 or 10-bit addressing and combined format transfers
  - Support for Clock synchronization and Bus Clear

## About Example Code

- This example code demonstrates I2C data transfer between leader and follower
- In example code, firstly clock is initialized using static function.
- \ref sl_si91x_i2c_init API is called to configure various parameters of \ref sl_i2c_init_params_t structure. This structure includes:
  - \ref sl_i2c_clock_hlr_t bus speed, Standard, Fast, Fast plus or High speed.
  - Frequency which needs to be configured.
  - \ref sl_i2c_mode_t mode, either leader or follower, here follower is selected.
- After initialization of the parameters, pin configuration is performed using static function.
  - Here in pin configuration, the pins are configured as internal pull-up.
  - To configure the pins as internal pull-up, it is necessary to disable ROM APIs.
  - It validates the SLI_SI91X_MCU_MOV_ROM_API_TO_FLASH macro to configure the gpio as internal pull-up.
- Now write_buffer is filled with some data which needs to be sent to the leader.
- Current_mode enum is set to RECEIVE_DATA, so here receive_data is called which is a static function, that internally calls the APIs which needs to be configured before sending data.
  - Disable the interrupt using \ref sl_si91x_i2c_disable_interrupts API.
  - Disable I2C, sets slave address and rx thresholds using \ref sl_si91x_i2c_set_rx_threshold API and enable the I2C.
  - Set the direction of transfer using \ref sl_si91x_i2c_control_direction.
  - Set the \ref SL_I2C_EVENT_RECEIVE_FULL interrupt using \ref sl_si91x_i2c_set_interrupts API.
  - Enable the interrupt using \ref sl_si91x_i2c_enable_interrupts API.
- After calling receive_data, it will wait till all the data is received from the leader device.
- Once the irq handler generates send_complete flag, it changes current_mode enum to SEND_DATA.
- Current_mode enum is set to SEND_DATA, so here send_data is called which is a static function, that internally calls the APIs which needs to be configured before sending data.
  - Disable the interrupt using \ref sl_si91x_i2c_disable_interrupts API, fill the write_data variable with the data.
  - Disable I2C, sets slave address and tx thresholds using \ref sl_si91x_i2c_set_tx_threshold API and enable the I2C.
  - Set the \ref SL_I2C_EVENT_TRANSMIT_EMPTY interrupt using \ref sl_si91x_i2c_set_interrupts API.
  - Enable the interrupt using \ref sl_si91x_i2c_enable_interrupts API.
- After calling send_data, it will wait till all the data is transferred to the leader device.
- Once the irq handler generates receive_complete flag, it changes current_mode enum to TRANSMISSION_COMPLETED.
- Now it compares the data which is received from the leader device to the data which it has sent.
- If the data is same, it will print Test Case Passed on the console.

## Prerequisites/Setup Requirements

- To use this application following Hardware, Software and the Project Setup is required

### Hardware Requirements

- Windows PC
- Silicon Labs [WPK(BRD4002) + BRD4338A]. It will act as a leader 
- Silicon Labs [WPK(BRD4002) + BRD4338A]. It will act as a follower 

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

For details on the project folder structure, see the [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure) page.

## Application Build Environment

- Configure the following macros in i2c_follower_example.c file and update/modify following macros if required.

  ```C
    #define I2C_INSTANCE    2    // I2C Instance for Pin configuration
    #define I2C             I2C2 // I2C Instance 
  ```

- `Available Configurations`: Apart from I2C0, other configurations are given below.
 
  ```C
    #define I2C_INSTANCE    1    // I2C Instance for Pin configuration
    #define I2C             I2C1 // I2C Instance 
    #define I2C_INSTANCE    0    // I2C Instance for Pin configuration
    #define I2C             I2C0 // I2C Instance 
  ```

- Change the value of following macros in config/RTE_Device_917.h for I2C0

  ```c
    #define RTE_I2C0_SCL_PORT_ID 0   // SCL pin port id
    #define RTE_I2C0_SDA_PORT_ID 0   //SDA pin port id
  ```

### Pin Configuration

**I2C0:**

| PIN |   ULP GPIO PIN     |   Description             |
| --- | ------------------ | ------------------------- |
| SCL |   GPIO_7 [P20]     | Connect to Leader SCL pin |
| SDA |   GPIO_6 [P19]     | Connect to Leader SDA pin |

**I2C1:**

| PIN |   GPIO PIN        |    Description            |
| --- | ----------------- | ------------------------- |
| SCL |   GPIO_50 [P32]   | Connect to Leader SCL pin |
| SDA |   GPIO_51 [P34]   | Connect to Leader SDA pin |

**I2C2:**

| PIN |   ULP GPIO PIN             |   Description             |
| --- | -------------------------- | ------------------------- |
| SCL | ULP_GPIO_7 [EXP_HEADER-15] | Connect to Leader SCL pin |
| SDA | ULP_GPIO_6 [EXP_HEADER-16] | Connect to Leader SDA pin |

![Figure: Pin Configuration I2C](resources/readme/image506d.png)

![Figure: Pin Configuration I2C](resources/readme/image506e.png)

## Test the Application

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

1. Compile and run the application.
2. Connect ULP_GPIO_6 and ULP_GPIO_7 with the leader device for I2C2.
3. When the application runs, it receives and sends data.
4. After the transfer is completed, it validates the data and prints on the console.
5. Will get "Test Case Pass" print on console.
6. After successful program execution the prints in serial console looks as shown below.

    ![Figure: Output](resources/readme/output.png)