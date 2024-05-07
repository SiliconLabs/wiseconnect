# SL SI70XX

## Table of Contents

- [Purpose/Scope](#purposescope)
- [About Example Code](#about-example-code)
- [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
  - [Hardware Requirements](#hardware-requirements)
  - [Software Requirements](#software-requirements)
  - [Setup Diagram](#setup-diagram)
- [Getting Started](#getting-started)
- [Application Build Environment](#application-build-environment)
  - [Pin Configuration](#pin-configuration)
- [Pin Configuration](#pin-configuration)
- [Test the Application](#test-the-application)

## Purpose/Scope

- This application demonstrate the si70xx RHT sensor, which measures the relative humidity and temperature via I2C interface for every 2 seconds.

## About Example Code

- This example demonstrates the measurement of relative humidity and temperature for every 2 seconds. Also shows how to use different APIs present via I2C interface.

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

> ![Figure: Introduction](resources/readme/setupdiagram.png)

## Getting Started

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Install Studio and WiSeConnect 3 extension
- Connect your device to the computer
- Upgrade your connectivity firmware
- Create a Studio project

For details on the project folder structure, see the [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure) page.

## Application Build Environment

- Configure the following macros in si70xx_example.c file and update/modify following macros if required.

- `I2C instance`: Select I2C instance for communication. By default I2C2 is selected.

  ```C
    #define I2C SL_I2C2 // I2C 2 instance
  ```

### Pin Configuration

Tested on WPK Base board - 4002A and Radio board - BRD4338A.

| Description  | GPIO       | Connector |
| -------------| -----------| ----------|
| I2C_SDA      | ULP_GPIO_6 | EXP_16    |
| I2C_SCL      | ULP_GPIO_7 | EXP_15    |

> **Note:** Make sure pin configuration in RTE_Device_917.h file.(path: /$project/config/RTE_Device_917.h)

## Test the Application

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

1. Compile and run the application.
2. When the application runs, it measures relative humidity and temperature for every 2 seconds.
3. Connect oscilloscope to Evaluation kit board's ULP_GPIO_6(EXP16), ULP_GPIO_7(EXP15) and observe the temperature and humidity data on I2C line.
4. After successful program execution the prints in serial console looks as shown below.

   ![Figure: Introduction](resources/readme/output1.png)

   ![Figure: Introduction](resources/readme/output2.png)
