# SL CRC

## Table of Contents

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

 This application contains an example code to demonstrate CRC (Cyclic Redundancy Check) Generation on DMA transferred data.

## Prerequisites/Setup Requirements

 To use this application following Hardware, Software and the Project Setup is required.

### Hardware Requirements

- Windows PC
- Silicon Labs [Si917 Evaluation Kit WPK(BRD4002) + BRD4338A / BRD4342A / BRD4343A ]
- SiWx917 AC1 Module Explorer Kit (BRD2708A)
- Ezurio Veda SL917 Explorer Kit Board (BRD2911A)


### Software Requirements

- Simplicity Studio
- Serial console Setup
  - For Serial Console setup instructions, refer [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#console-input-and-output).
- Embedded Development Environment
  - For Silicon Labs Si91x, use the latest version of Simplicity Studio (refer **"Download and Install Simplicity Studio"** section in **getting-started-with-siwx917-soc** guide at [**release_package/docs/index.html**](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#install-simplicity-studio)

### Setup Diagram

> ![Figure: Introduction](resources/readme/setupdiagram.png)

## Getting Started

Refer instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) for the following tasks:

- Install Studio and WiSeConnect 3 extension
- Connect your device to the computer
- Upgrade your connectivity firmware
- Create a Studio project

For details on the project folder structure, see the [WiSeConnect_Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure/) page.

## Application Build Environment

### Application Configuration Parameters

- An application has two parameters configurable they are Polynomial value and Data width.

- Configure the following parameter in sl_si91x_crc.h file(path : components\device\silabs\si91x\mcu\drivers\unified_api\inc\sl_si91x_crc.h), update/modify following macro if required also below mentioned are default configurations.

  ```C
   #define SL_CRC_POLYNOMIAL      0x04C11DB7 /* Polynomial encryption value */
   #define SL_CRC_POLY_WIDTH       31        /* Polynomial data width */
  ```
> **Note:**
> While changing the data which CRC has to calculate and compare with Software CRC, need to update the data in file `sw_crc.c` variable  `input[]`  and in `crc_example.c` variable `gcrc_tx_Buf[]`.

## Test the Application

Refer instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#create-a-project) to:

- Compile and run the application. 

 - Console Output:

    ![Figure: Introduction](resources/readme/crc_result_console.png)



> **Note:** Interrupt handlers are implemented in the driver layer, and user callbacks are provided for custom code. If you want to write your own interrupt handler instead of using the default one, make the driver interrupt handler a weak handler. Then, copy the necessary code from the driver handler to your custom interrupt handler.


