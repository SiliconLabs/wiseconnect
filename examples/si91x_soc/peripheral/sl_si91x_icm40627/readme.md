# SL ICM40627

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

- This application demonstrate the 6-axis inertial sensor (ICM-40627), which measures the motion parameters and temperature every 2 seconds.

## About Example Code

- This example demonstrates the measurement of acceleration in 3 axes, gyroscope in 3 axes, and temperature every 2 seconds. It also shows how to use various APIs available via the SPI interface.

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

## Application Build Environment

- Configure the following macros in icm40627_example.c file and update/modify following macros if required.

- `DELAY_PERIODIC_MS1`: Select the delay for data display. By default, the delay is kept as 2 seconds.

  ```C
    #define DELAY_PERIODIC_MS1  2000      //sleeptimer1 periodic timeout in ms
  ```

## Test the Application

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

1. Compile and run the application.
2. When the application runs, it measures accelerometer, gyroscope and temperature data for every 2 seconds.
3. After successful program execution the prints in serial console looks as shown below.

   ![Figure: Introduction](resources/readme/output1.png)