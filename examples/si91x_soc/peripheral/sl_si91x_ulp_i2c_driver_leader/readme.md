# SL ULP I2C DRIVER LEADER

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
  - [Application Configuration Parameters](#application-configuration-parameters)
  - [Pin Configuration](#pin-configuration)
- [Test the Application](#test-the-application)

## Purpose/Scope

- The application demonstrates the data transfer from leader to follower and then follower to leader in high power & low power states. After transmission the data is compared and result is printed on the console.

> **Note:** The master-slave terminology is now replaced with leader-follower. Master is now recognized as Leader and slave is now recognized as Follower.

## Overview

- The I2C will be configured in leader mode. The SCL and SDA lines of leader controller are connected to Follower's SCL and SDA pins.
- There are three configurable I2C controllers in M4 - two in the MCU HP peripherals (I2C1, I2C2) and one in the MCU ULP subsystem (ULP_I2C). For I2C follower all instances will in MCU HP mode.
- The I2C interface allows the processor to serve as a leader or follower on the I2C bus.
- I2C can be configured with following features
  - I2C standard compliant bus interface with open-drain pins
  - Configurable as Leader or Follower
  - Four speed modes: Standard Mode (100 kbps), Fast Mode (400 kbps), Fast Mode Plus (1Mbps) and High-Speed Mode (3.4 Mbps)
  - 7 or 10-bit addressing and combined format transfers
  - Support for Clock synchronization and Bus Clear

## About Example Code

- This example code demonstrates I2C data transfer between leader and follower either using interrupt(Blocking application) or using DMA (Non-blocking application).
- In this example i2c instance is first initialized using \ref sl_i2c_driver_init to configure various init structure parameters
- This structure includes:
  - \ref sl_i2c_operating_mode_t bus speed, it can be Standard, Fast, Fast plus or High speed.
  - \ref sl_i2c_mode_t mode, it should be leader mode for leader application.
  - \ref sl_i2c_transfer_type_t, using interrupt or DMA (for using non-blocking send/transfer API it should be set as DMA type and for blocking API
     it should be interrupt type)
  - \ref sl_i2c_callback_t , I2C callback
- It also initializes I2C clock and configures I2C SDA & SCL pins.
- It also initializes DMA, if transfer type is 'Using DMA'.
- Now transmit and receive FIFO threshold values are configured using \ref sl_i2c_driver_configure_fifo_threshold API.
- Now write_buffer is filled with some data which needs to be sent to the follower.
- Current_mode enum is set to I2C_SEND_DATA and it calls send_data API to send data to follower & configures follower address through \ref sl_i2c_driver_send_data_blocking (for blocking Application) or through \ref sl_i2c_driver_send_data_non_blocking (for Non-blocking Application).
- For Blocking usecase : When all bytes are sent then mode changes to I2C_RECEIVE_DATA (Blocking API won't update any transfer complete flag, as control will be blocked untill all bytes are sent).
- For Non-Blocking usecase : After that it will wait till all the data is transferred to the follower device & once the i2c callback function sets transfer_complete flag, it changes current_mode enum to I2C_RECEIVE_DATA.
- Then it receives data from follower through \ref sl_i2c_driver_receive_data_blocking (for blocking Application) or through \ref sl_i2c_driver_receive_data_non_blocking (for Non-blocking Application).
- For Blocking usecase : When all bytes are recieved then mode changes to I2C_TRANSMISSION_COMPLETED (Blocking API won't update any transfer complete flag, as control will be blocked untill all bytes are received).
- For Non Blocking usecase : After calling receive_data, it will wait till all the data is received from the follower device & once the i2c callback function sets transfer_complete flag, it changes current_mode enum to I2C_TRANSMISSION_COMPLETED.
- Now it compares the data which is received from the follower device to the data which it has sent.
- If the data is same, it will print Test Case Passed on the console.
- After this first cycle of data transfer application switches to ULP mode using sl_si91x_power_manager_add_ps_requirement API and reconfig I2C leader params as per ULP mode through \ref sl_i2c_driver_leader_reconfig_on_power_mode_change API.
- Then current_mode enum changed to I2C_SEND_DATA & application waits for 10 seconds so that user can reset follower application
- Once Follower gets reset then whole data transfer cycle repeats once again and compares data again.
- After second cycle of data transfer application switches to HP mode using sl_si91x_power_manager_add_ps_requirement API and reconfig I2C leader params as per HP mode using \ref sl_i2c_driver_leader_reconfig_on_power_mode_change API.
- Then current_mode enum changed to I2C_SEND_DATA & application waits for 10 seconds so that user can reset follower application
- Once Follower gets reset then whole data transfer cycle repeats once again and compares data again.
- After third cycle of data tranfer I2c gets de-initialized.


> **Note:**
>
>- I2C has three instances. User can handle these i2c-instances by adding their instances.
>- I2C0, I2C1 & I2C2 are the names pre-defined for the I2C instances. But ULP I2C supports only I2C2 and instance 2.
>- For user defined instances, one may have to define his hardware specific definitions in config.h file.
>- User can directly use APIs in application by passing appropriate structure members, if user doesn't want to configure from UC.

## Prerequisites/Setup Requirements

### Hardware Requirements

- Windows PC
- Silicon Labs Si917 Evaluation Kit [WPK(BRD4002) + BRD4338A]

### Software Requirements

- Simplicity Studio
- Serial console Setup
  - The Serial Console setup instructions are provided below:
Refer [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/getting-started-with-soc-mode#perform-console-output-and-input-for-brd4338-a).

### Setup Diagram

![Figure: Introduction](resources/readme/setupdiagram.png)

## Getting Started

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Install Studio and WiSeConnect 3 extension
- Connect your device to the computer
- Upgrade your connectivity firmware
- Create a Studio project

## Application Build Environment

### Application Configuration Parameters

- Open sl_si91x_i2c_driver_leader.slcp project file select software component tab and search for i2c in search bar.
- Click on **I2C2** and configure the I2C2 instance as per configuration parameters given in wizard.
- For ULP application instance used should be 2 only
- After creation of instances separate configuration files are get generated in **config folder**.
- If project built without selecting configurations, it will take default values from UC.
- Configure mode, operating-mode and transfer-type of I2C instance using respective instance UC.
- Change 'Operating Mode' as per bus-speed requirement.
- Change 'Transfer Type' to 'Using Interrupt' for Blocking Application or to 'Using DMA' for NON-Blocking Application.
- After above UC configurations also configure following macros in i2c_leader_example.c file and update/modify following macros if required.

  ```C
    #define BLOCKING_APPLICATION     // Enable it for enabling I2C transfer using interrupt Application
    #define NON_BLOCKING_APPLICATION // Enable it for enabling I2C transfer using interrupt Application
    #define FOLLOWER_I2C_ADDR        // Update I2C follower address
    #define I2C_SIZE_BUFFERS             // To change the number of bytes to send and receive.Its value should be less than maximum buffer size macro value.
  ```

- Configure the UC as mentioned below.

  ![Figure: Introduction](resources/uc_screen/i2c_uc_screen.png)

### Pin Configuration

**I2C2:**

| PIN |   ULP GPIO PIN             |   Description             |
| --- | -------------------------- | ------------------------- |
| SCL | ULP_GPIO_7 [EXP_HEADER-15] | Connect to Follower SCL pin |
| SDA | ULP_GPIO_6 [EXP_HEADER-16] | Connect to Follower SDA pin |

![Figure: Pin Configuration I2C](resources/readme/image507e.png)

## Test the Application

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

1. Compile and run the application.
2. Connect SCL(ULP_GPIO_7) and SDA(ULP_GPIO_6) pins with the follower device.
3. When the application runs, it receives and send data.
4. After the transfer is completed, it validates the data and prints "Test Case Passed" on the console.
5. Will get "Test Case Passed" print on console.
6. Then again reset follower once application switches to ULP mode and observe "Test Case Passed" print on console.
7. After this again reset follower once application switches to HP mode back and observe "Test Case Passed" print on console.
6. Connect Analyzer channels to respective I2C instance SDA & SCA pins to observe the data on lines.
7. After successful program execution the prints in serial console looks as shown below.For follower also 3 times testcase pass will appear on console.

   ![Figure: Output](resources/readme/output.png)
