# SL BUTTON BAREMETAL

## Table of Contents

- [Purpose/Scope](#purposescope)
- [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
  - [Hardware Requirements](#hardware-requirements)
  - [Software Requirements](#software-requirements)
  - [Setup Diagram](#setup-diagram)
- [Getting Started](#getting-started)
- [Application Build Environment](#application-build-environment)
- [Test the Application](#test-the-application)

## Purpose/Scope

- This example application demonstrates the use of BUTTON in a bare metal environment.The application toggles LED on each BUTTON press using the sl_si91x_led_toggle() function. 
- Additional LED and BUTTON instances could be added.

## Prerequisites/Setup Requirements

### Hardware Requirements

- Windows PC
- Silicon Labs Si917 Evaluation Kit [WPK(BRD4002) + BRD4338A]

### Software Requirements

- Simplicity Studio

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

- The application will toggle the selected LED on each selected button press.

### Configuration

- Button Configure Interrupt: The button interrupt type can be configured. By default, Rise and Fall edge interrupt type has been set. One of the following can be selected - 
  - Low level interrupt: Interrupt on low/pressed button state can be configured
  - High level interrupt: Interrupt on high/released button state
  - Low level and High level: Interrupt on low/pressed and high/released button state
  - Rise edge interrupt: Interrupt on rising edge of the button press
  - Fall edge interrupt: Interrupt on falling edge of the button press
  - Rise adge and fall edge interrupt: Interrupt on rising edge and falling edge of the button press

> ![Figure: Introduction](resources/readme/image600a.png)

## Test the Application
>
>- The project uses BTN0 by default. To change the button to BTN1, follow the below steps :
>
  >  1. Add btn1 instance by traversing to the below path and select "Add New Instances" :
    "Software Components -> WiSeConnect 3 SDK -> Device -> Si91x -> MCU -> Drivers -> BUTTON"
    >
  > ![Figure: Adding Button Instance](resources/readme/image600d.png)
  > 
  >  2. Change the definition of macro BUTTON_INSTANCE_0 in button_baremetal.c file to button_btn1.
  >  
  >  3. Build and test the output.

  - LED0 will toggle when Button0 pressed.