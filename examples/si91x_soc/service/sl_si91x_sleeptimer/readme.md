# SL SLEEPTIMER

## Table of Contents

- [SL SLEEPTIMER](#sl-sleeptimer)
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

- This application contains an example code to demonstrate one shot and periodic SLEEPTIMER with LED toggle functionality.
      - LED0 for ACx Module boards and LED1 for ICs

## Prerequisites/Setup Requirements

- To use this application following Hardware, Software and the Project Setup is required.

### Hardware Requirements

- Windows PC
- Silicon Labs [Si917 Evaluation Kit WPK(BRD4002) + BRD4338A / BRD4342A / BRD4343A ]
- SiWx917 AC1 Module Explorer Kit (BRD2708A)

### Software Requirements

- Simplicity Studio
- Serial console Setup
  - For Serial Console setup instructions, refer [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#console-input-and-output).
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

For details on the project folder structure, see the [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure) page.

## Application Build Environment

### Application Configuration Parameters

- Si91x only uses si91x SYSRTC peripheral for sleeptimer. No UC is present for si91x sleeptimer component.

- Configure the following parameter in [`sleeptimer.c`](https://github.com/SiliconLabs/wiseconnect/blob/master/examples/si91x_soc/service/sl_si91x_sleeptimer/sleeptimer.c) file, update/modify following macro if required

  ```C
    #define TOOGLE_DELAY_MS1_ONESHOT //  configured for 5000 ms
    #define TOOGLE_DELAY_MS1_PERIODIC // configured for 400 ms
  ```

> **Note**: For recommended settings, see the [recommendations guide](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-prog-recommended-settings/).

## Test the Application

Refer instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

1. Compile and run the application.
2. Sets the board state of LED to on and off after 5 sec and then configured for periodic blink rate of 400 ms.
3. LED should blink on WPK base board and prints should come on console. 

   ![Figure: Introduction](resources/readme/output.png)
