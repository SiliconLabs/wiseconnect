# SL NVM3 DUAL FLASH

## Table of Contents

- [Purpose/Scope](#purposescope)
- [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
  - [Hardware Requirements](#hardware-requirements)
  - [Software Requirements](#software-requirements)
  - [Setup Diagram](#setup-diagram)
- [Getting Started](#getting-started)
- [Application Build Environment](#application-build-environment)
  - [Code modifications for printing on the uart console](#code-modifications-for-printing-on-the-uart-console)
  - [Pinout on WPK for uart console](#pinout-on-wpk-for-uart-console)
- [Test the Application](#test-the-application)
- [Resources](#resources)

## Purpose/Scope

- This application demonstrates the use of Third Generation Non-Volatile Memory (NVM3) data storage in Si91x dual flash.

## Prerequisites/Setup Requirements

- To use this application following Hardware, Software and the Project Setup is required

### Hardware Requirements

- Windows PC
- Silicon Labs [Si917 Evaluation Kit WPK(BRD4002) + BRD4325B]
  - The Serial Console setup instructions are provided below,
refer instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/getting-started-with-soc-mode#perform-console-output-and-input-for-brd4338-a).

### Software Requirements

- Simplicity Studio
- Embedded Development Environment
  - For Silicon Labs Si91x, use the latest version of Simplicity Studio (refer **"Download and Install Simplicity Studio"** section in **getting-started-with-siwx917-soc** guide at **release_package/docs/index.html**)

### Setup Diagram

> ![Figure: Introduction](resources/readme/setupdiagram.png)

## Getting Started

Refer instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) for the following tasks:

- Install Studio and WiSeConnect 3 extension
- Connect your device to the computer
- Upgrade your connectivity firmware
- Create a Studio project

## Application Build Environment

### Code modifications for printing on the uart console

- In rsi_board.c file make M4_UART1_INSTANCE as 1 and M4_UART2_INSTANCE as 0

### Pinout on WPK for uart console

| GPIO pin                | Description             |
| ----------------------- | ----------------------- |
| GPIO_30 [EXP_HEADER-4] | UART Tx   |
| GPIO_29 [EXP_HEADER-6] | UART Rx  |
| [EXP_HEADER-1]  | GND    |

## Test the Application

- Users can give the following commands:

  - For storing new key, type below syntax followed by enter
    write key string
    ex: write 1 SiliconLabs
  - For reading data from key, type below syntax followed by enter
    read key
    ex: read 1
  - For deleting key, type below syntax followed by enter
    delete key
    ex: delete 1

- After successful program execution the prints in serial console looks as shown below.

  ![Figure: Introduction](resources/readme/output.png)

## Resources

- [AN1135: Using Third Generation Non-Volatile Memory (NVM3) Data Storage](https://www.silabs.com/documents/public/application-notes/an1135-using-third-generation-nonvolatile-memory.pdf)
