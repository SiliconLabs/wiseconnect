# SL CRC

## Table of Contents

- [SL CRC](#sl-crc)
  - [Table of Contents](#table-of-contents)
  - [Purpose/Scope](#purposescope)
  - [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
    - [Hardware Requirements](#hardware-requirements)
    - [Software Requirements](#software-requirements)
    - [Setup Diagram](#setup-diagram)
  - [Getting Started](#getting-started)
  - [Application Build Environment](#application-build-environment)
    - [Application Configuration Parameters](#application-configuration-parameters)
  - [Test the Application](#test-the-application)

## Purpose/Scope

 This application contains example code to demonstrate CRC (Cyclic Redundancy Check) Generation on DMA-transferred data.

## Prerequisites/Setup Requirements

 To use this application, the following hardware, software, and project setup is required.

### Hardware Requirements

- Windows PC
- Silicon Labs [Si917 Evaluation Kit WPK(BRD4002) + BRD4338A / BRD4342A / BRD4343A ]
- SiWx917 AC1 Module Explorer Kit (BRD2708A)

### Software Requirements

- Simplicity Studio
- Serial console setup
  - For serial console setup instructions, see the [Console Input and Output](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#console-input-and-output) section of the *WiSeConnect Developer's Guide*.
- Embedded Development Environment
  - For Silicon Labs Si91x, use the latest version of Simplicity Studio (refer to the [Download and Install Simplicity Studio](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#install-simplicity-studio) section in the Developing with *WiSeConnect™ SDK v3.x with SiWx91x™ Boards Guide*).

### Setup Diagram

> ![Figure: setupdiagram](resources/readme/setupdiagram.png)

## Getting Started

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) for the following tasks:

- Install Studio and WiSeConnect 3 extension
- Connect your device to the computer
- Upgrade your connectivity firmware
- Create a Studio project

For details on the project folder structure, see the [WiSeConnect_Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure/) page.

## Application Build Environment

### Application Configuration Parameters

- The application has two configurable parameters:  Polynomial value and Data width.

- Configure the following parameter in the [`sl_si91x_crc.h`](https://github.com/SiliconLabs/wiseconnect/blob/master/components/device/silabs/si91x/mcu/drivers/unified_api/inc/sl_si91x_crc.h) file. Update or modify the following macro, if required. The code illustrates the default configurations.

  ```C
   #define SL_CRC_POLYNOMIAL      0x04C11DB7 /* Polynomial encryption value */
   #define SL_CRC_POLY_WIDTH       31        /* Polynomial data width */
  ```

> **Note:**
> While changing the data which CRC has to calculate and compare with Software CRC, you need to update the data in file `sw_crc.c` variable  `input[]`  and in `crc_example.c` variable `gcrc_tx_Buf[]`.

> **Note**: For recommended settings, see the [recommendations guide](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-prog-recommended-settings/).

## Test the Application

Refer to the instructions in the [Create a Project](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#create-a-project) section on the *Developing with WiSeConnect™ SDK v3.x with SiWx91x™ Boards* page to:

- Compile and run the application.

  - Console Output:

    ![Figure: crc_result_console](resources/readme/crc_result_console.png)

> **Note:** Interrupt handlers are implemented in the driver layer, and user callbacks are provided for custom code. If you want to write your own interrupt handler instead of using the default one, make the driver interrupt handler a weak handler. Then, copy the necessary code from the driver handler to your custom interrupt handler.
