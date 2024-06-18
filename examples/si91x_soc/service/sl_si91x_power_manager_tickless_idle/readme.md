# SL POWER MANAGER TICKLESS IDLE

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
- [Test the Application](#test-the-application)

## Purpose/Scope

- This application demonstrates the power manager service APIs, state transitions(PS4 and PS3) and sleep-wakeup (PS4 and PS3 sleep with RAM retention).

## Overview

- According to the configuration in power_manager_example application, it updates the power state in the requirement table upon software timer expiration.It then determines the highest attainable operating state and begins the transition. When application gets the idle time, Power manager initiates the sleep.
- Sleep - Demonstrations of wakeup with RAM retention are showcased in both PS4 and PS3 states, automatically adjusting to the attainable state.

## About Example Code

- At initialization, a thread is created and the application_start() function is called along the thread.
- All the activities are handled in the application_start() function.
- Firstly wifi is initialized, M4-TA secure handshake is established to send commands to TA, TA is switched to STANDBY_WITH_RAM_RETENTION mode.
- Power Manager service is initialized, the processor is switched to PS4 state and the clock is 32 MHz (Power Save) using \ref sl_si91x_power_manager_init.
- According to the revised implementation, the PS4 and PS3 powersave modes will continue to use the same clock frequency (32MHz).
- All the possible events are ored and passed to the \ref sl_si91x_power_manager_subscribe_ps_transition_event along with the callback function address.
- RAM retention is enabled and configured using \ref sl_si91x_power_manager_configure_ram_retention.
- Wakeup Source is selected as SysRTC and wireless wakeup by default. Application will go to sleep with SysRTC as wakeup resource as per the idle time provided by scheduler and it will wakeup with wireless wakeup.

## Prerequisites/Setup Requirements

- To use this application following Hardware, Software and Project Setup is required.

### Hardware Requirements

- Windows PC
- Silicon Labs Si917 Evaluation Kit [WPK(BRD4002) + BRD4338A]
  - The Serial Console setup instructions are provided below:
    Refer instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/getting-started-with-soc-mode#perform-console-output-and-input-for-brd4338-a).

### Software Requirements

- Simplicity Studio
- Embedded Development Environment
  - For Silicon Labs Si91x, use the latest version of Simplicity Studio (refer **"Download and Install Simplicity Studio"** section in **getting-started-with-siwx917-soc** guide at **release_package/docs/index.html**)

### Setup Diagram

![Figure: Introduction](resources/readme/setupdiagram.png)

## Getting Started

Refer instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) for the following tasks:

- Install Studio and WiSeConnect 3 extension
- Connect your device to the computer
- Upgrade your connectivity firmware
- Create a Studio project

For details on the project folder structure, see the [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure) page.

## Application Build Environment

## Test the Application

Refer instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) for the following tasks:

1. Compile and run the application.
2. Enable macros whose functionality needs to be tested.
3. By default it initializes the wifi and switches TA to standby with RAM retention mode, power manager service is initialized and callback is subscribed.
4. After successful program execution, observe the prints in the serial console.
