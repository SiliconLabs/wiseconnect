# PWM

## Introduction

- This application demonstrate the PWM (Pulse Width Modulation) to generate a periodic pulse waveform, which is useful in motor control and power control applications.
 The PWM acts as a timer to count up to a period count value. The time period and the duty cycle of the pulses are both programmable.

## Overview

- Supports up to eight PWM outputs with four duty cycle generators
- Complementary and Independent output modes are supported
- Dead time insertion in Complementary mode
- Manual override option for PWM output pins. Output pin polarity is programmable
- Supports generation of interrupt for different events
- Supports two hardware fault input pins
- Special event trigger for synchronizing analog-to-digital conversions

## About Example Code

- This example demonstrates the generation of periodic pulse waveform with 50% duty cycle and frequency of 25Khz. 
- Four macros are present i.e., SVT, DEAD_TIME, OVERRIDE, FAULT
- If **DEAD_TIME** is enabled:
  - Dead time mode is applied only in complementary mode.
  - Initialize the PWM using \ref sl_si91x_pwm_init.
  - Set configuration using \ref sl_si91x_pwm_set_configuration.
  - Set duty cycle using \ref sl_si91x_pwm_set_duty_cycle.
  - Set base timer mode using \ref sl_si91x_pwm_set_base_timer_mode.
  - Set base time period control using \ref sl_si91x_pwm_control_period.
  - Register callbacks using \ref sl_si91x_pwm_register_callback.
  - Start PWM using \ref sl_si91x_pwm_start.
- If **OVERRIDE** is enabled:
  - Initialize the PWM using \ref sl_si91x_pwm_init.
  - Set configuration using \ref sl_si91x_pwm_set_configuration.
  - Set base timer mode using \ref sl_si91x_pwm_set_base_timer_mode.
    - Set duty cycle using \ref sl_si91x_pwm_set_duty_cycle.
  - Set base time period control using \ref sl_si91x_pwm_control_period.
  - Register callbacks using \ref sl_si91x_pwm_register_callback.
  - Start PWM using \ref sl_si91x_pwm_start.
- If **FAULT** is enabled:
  - Initialize the PWM using \ref sl_si91x_pwm_init, \ref sl_Si91x_pwm_fault_init.
  - Set configuration using \ref sl_si91x_pwm_set_configuration.
  - Set base timer mode using \ref sl_si91x_pwm_set_base_timer_mode.
    - Set duty cycle using \ref sl_si91x_pwm_set_duty_cycle.
  - Set base time period control using \ref sl_si91x_pwm_control_period.
  - Register callbacks using \ref sl_si91x_pwm_register_callback.
    Change the event generated to fault A/B flags.
  - Start PWM using \ref sl_si91x_pwm_start.
- If **SVT** is enabled:
  - Initialize the PWM using \ref sl_si91x_pwm_init.
  - Set configuration using \ref sl_si91x_pwm_set_configuration.
  - Set base timer mode using \ref sl_si91x_pwm_set_base_timer_mode.
  - Set base time period control using \ref sl_si91x_pwm_control_period.
  - Register callbacks using \ref sl_si91x_pwm_register_callback.
  - Start PWM using \ref sl_si91x_pwm_start.
- If user wants to work without UC configuration, one can use their own macros and structure configuration and pass it directly in application.
  
**Note!** 

1. PWM has four channels. User can handle these channels using instances. 
2. channel_0, channel_1, channel_2 and channel_3 are the names pre-defined for the PWM channels.
3. For user defined instances, one may have to define his hardware specific definitions in config.h file.
4. User can directly use APIs in application by passing appropriate structure members, if user doesn't want to configure from UC.

## Running Example Code

- To use this application following Hardware, Software and the Project Setup is required. User Configuration(UC) is provided with instances to select multiple channels at a time.

### Hardware Requirements

- Windows PC
- Silicon Labs [Si917 Evaluation Kit WPK/WSTK + BRD4338A]

![Figure: Introduction](resources/readme/image515a.png)

### Software Requirements

- Si91x SDK
- Embedded Development Environment
  - For Silicon Labs Si91x, use the latest version of Simplicity Studio (refer **"Download and Install Simplicity Studio"** section in **getting-started-with-siwx917-soc** guide at **release_package/docs/index.html**)

## Project Setup

- **Silicon Labs Si91x** refer **"Download SDK"** section in **getting-started-with-siwx917-soc** guide at **release_package/docs/index.html** to work with Si91x and Simplicity Studio

## Loading Application on Simplicity Studio

1. With the product Si917 selected, navigate to the example projects by clicking on Example Projects & Demos
  in simplicity studio and click on to PWM Example application as shown below.

![Figure:](resources/readme/image515b.png)

## Configuration and Steps for Execution

- Configure the following macros in pwm_example.c file and update/modify following macros if required.

```C
#define ENABLE 1  // enable

```

## Build

1. Compile the application in Simplicity Studio using build icon

![Figure: Build run and Debug](resources/readme/image515c.png)

## Device Programming

- To program the device ,refer **"Burn M4 Binary"** section in **getting-started-with-siwx917-soc** guide at **release_package/docs/index.html** to work with Si91x and Simplicity Studio

## SIO SPI Pin Configuration

Tested on WSTK Base board - 40002A and Radio boards - BRD4325C, BRD4338A. Below GPIO pins are for SI917_RADIO_BOARD. These exposed pins like P19 etc., are specific to 2.0(BRD4338A) Radio board. These exposed pins may change on other boards. 

**Note!** When PWM channel is changed from default, take care GPIO pins assigned to PWM channel. Either look into GPIO pin mux from HRM (or) check RTE_Device_917.h file.

## Executing the Application

1. Compile and run the application.
2. When the application runs, it generates periodic pulse waveform.

## Expected Results

- PWM_1L and PWM_1H produces periodic pulse waveform with 50% duty cycle. Observe the waveform on GPIO_6, GPIO_7.
- Connect oscilloscope to Evaluation kit board's GPIO_6(P19) & GPIO_7(P20) and observe the PWM waveform.
