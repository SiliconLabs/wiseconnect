# PSRAM Blinky

## Table of Contents

- [Purpose/Scope](#purposescope)
- [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
  - [Hardware Requirements](#hardware-requirements)
  - [Software Requirements](#software-requirements)
  - [Setup Diagram](#setup-diagram)
- [Getting Started](#getting-started)
- [Application Build Environment](#application-build-environment)
- [PSRAM Linker Component Installation](#psram-linker-component-installation)
- [Test the Application](#test-the-application) 

## Purpo/sescope

- This example project demonstrates blinky application (GPIO/LED toggle) executing from PSRAM. By default, the Text segment, Data segment, BSS segment, Stack and Heap are placed in PSRAM.

## Prerequisites/Setup Requirements

### Hardware Requirements

- Windows PC
- Silicon Labs Si917 Evaluation Kit [WPK(BRD4002) + BRD4342A]

### Software Requirements

- Simplicity Studio

### Setup Diagram

 ![Figure: Introduction](resources/readme/setupdiagram.png)

## Getting Started

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Install Studio and WiSeConnect 3 extension
- Connect your device to the computer
- Upgrade your connectivity firmware
- Create a Studio project
- Compile and run the application.

## Application Build Environment

- Configure the following parameter in main.c file, update/modify following macro if required

  ```C
    #define RSI_BLINK_RATE //  configured for 10 ticks per second   
  ```   
- Sections can be included in PSRAM by installing components present under "PSRAM Linker Configurations" from "SOFTWARE COMPONENTS" GUI. Same can be removed from PSRAM and placed into default memory by uninstalling the respective component from software component selection GUI. 

## PSRAM Linker Component Installation

1. Open the .slcp file generated for your specific project by double-clicking it.
2. Navigate to the SOFTWARE COMPONENTS tab within the interface.
3. Utilize the search bar to look for "PSRAM."
4. Install the necessary component sections located within the PSRAM Linker Configurations.

> ![Figure: Component Installation](resources/readme/section_component_installation.png)

## Test the Application

1. Compile and run the application.
2. Toggles the state of LED0 at configured blink rate and then device goes to sleep.
3. LED0 should blink on WPK base board.

> **Note!** 
>
> LED toggling and device sleep will be a continous process.

