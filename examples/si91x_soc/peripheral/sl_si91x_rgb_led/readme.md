# SL RGB LED

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

- This application contains an example code to demonstrate the GPIO/LED toggle functionality

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

## Application Build Environment

- Configure the following parameters in rgb_led.c file, update/modify following macro if required.

  ```C
    #define RGB_COLOUR 0xFFFFFF//  configured for white colour by default 
  ```   
  ```C
    #define TICK_DELAY 1// configured delay for PWM simulation (1 TICK_DELAY = 30.5 us)
  ``` 
  ```C
    #define PULSE_PERIOD (TICK_DELAY * 0xFF)//  configured total delay for the PWM cycle (Dependant on TICK_DELAY)
  ``` 

## Test the Application

1. Sets the board state of RGB LED to ON and the colour is set according to the colour input.
 

> **Note:**
>
> - Interrupt handlers are implemented in the driver layer, and user callbacks are provided for custom code. If you want to write your own interrupt handler instead of using the default one, make the driver interrupt handler a weak handler. Then, copy the necessary code from the driver handler to your custom interrupt handler.
