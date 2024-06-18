# SL PWM

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
  - [PWM Pin Configuration](#pwm-pin-configuration)
- [Test the Application](#test-the-application)

## Purpose/Scope

- This application demonstrate the PWM (Pulse Width Modulation) to generate a periodic pulse waveform, which is useful in motor control and power control applications.

## Overview

- Supports up to eight PWM outputs with four duty cycle generators. The output pins are grouped in pairs, to facilitate driving the low side and high side of a power half bridge. For more details on PWM outputs please refer block diagram in HRM.
- Complementary and Independent output modes are supported
- Dead time insertion in Complementary mode
- Manual override option for PWM output pins. Output pin polarity is programmable
- Supports generation of interrupt for different events
- Supports two hardware fault input pins

## About Example Code

- This example demonstrates the generation of periodic pulse waveform with 50% duty cycle and frequency of 25Khz.
- Four macros are present i.e., DEAD_TIME, OVERRIDE, FAULT.
- If **DEAD_TIME** is enabled:
  - DEAD_TIME: To perform correct status change of the power switches in the inverter leg, a PWM generator should insert small amount of time between required switching edges for top and bottom switch. This time is called dead-time.
  - Dead time mode is applied only in complementary mode.
    - Complementary mode: In complementary PWM mode, PWM waveform output incorporates dead time (anti-short periods) to prevent overlap between the positive and anti phases.
    - Independent Mode: In Independent PWM Output mode, the PWM outputs (PWMxH and PWMxL) are phase shifted relative to each other.
  - Initialize the PWM using \ref sl_si91x_pwm_init.
  - Set configuration using \ref sl_si91x_pwm_set_configuration() API.
  - Set duty cycle using \ref sl_si91x_pwm_set_duty_cycle() API.
  - Set base timer mode using \ref sl_si91x_pwm_set_base_timer_mode() API.
  - Set base time period control using \ref sl_si91x_pwm_control_period() API.
  - Register callbacks using \ref sl_si91x_pwm_register_callback() API.
  - Start PWM using \ref sl_si91x_pwm_start() API.
- If **OVERRIDE** is enabled:
  - OVERRIDE: While overriding PWM outputs, the channel counters continue to run, only the PWM outputs are forced to user defined values.
  - Initialize the PWM using \ref sl_si91x_pwm_init() API.
  - Set configuration using \ref sl_si91x_pwm_set_configuration() API.
  - Set base timer mode using \ref sl_si91x_pwm_set_base_timer_mode() API.
  - Set duty cycle using \ref sl_si91x_pwm_set_duty_cycle() API.
  - Set base time period control using \ref sl_si91x_pwm_control_period() API.
  - Register callbacks using \ref sl_si91x_pwm_register_callback() API.
  - Start PWM using \ref sl_si91x_pwm_start() API.
- If **FAULT** is enabled:
  - FAULT: There are two fault pins, FAULTxA and FAULTxB, associated with the MCPWM(Motor Control Pulse Width Modulation) module. When asserted, these pins can optionally drive each of the PWM I/O pins to a defined state.
  - Initialize the PWM using \ref sl_si91x_pwm_init() API, \ref sl_Si91x_pwm_fault_init() API.
  - Set configuration using \ref sl_si91x_pwm_set_configuration() API.
  - Set base timer mode using \ref sl_si91x_pwm_set_base_timer_mode() API.
  - Set duty cycle using \ref sl_si91x_pwm_set_duty_cycle() API.
  - Set base time period control using \ref sl_si91x_pwm_control_period() API.
  - Register callbacks using \ref sl_si91x_pwm_register_callback() API.
  - Change the event generated to fault A/B flags.
  - Start PWM using \ref sl_si91x_pwm_start() API.
- If user wants to work without UC configuration, one can use their own macros and structure configuration and pass it directly in application.
- If **SVT** is enabled:
  - Initialize the PWM using \ref sl_si91x_pwm_init() API. Initialize Special Event Trigger based GPIO pin.
  - Set configuration using \ref sl_si91x_pwm_set_configuration() API.
  - Enable special event trigger using \ref sl_si91x_pwm_control_special_event_trigger() API.
  - Set configuration for special event trigger using \ref sl_si91x_pwm_trigger_special_event() API.
  - Start PWM using \ref sl_si91x_pwm_start() API.
  
>**Note:**
>
>1. PWM has four channels. User can handle these channels using instances. Each channel has 2 PWM outputs (i.e PWM_L(PWM output low), PWM_H(PWM output high)).The usage of L,H depends on application how to use them. For example, when driving something in PUSH-PULL configuration, PWM_H can drive the high-side switch, whereas PWM_L drives the low side switch. This is only a reference of how L,H can be used. There might be other scenarios, which are dependent on settings done and how to achieve it. 
>2. channel_0, channel_1, channel_2 and channel_3 are the names pre-defined for the PWM channels.
>3. For user defined instances, one may have to define his hardware specific definitions in sl_si91x_pwm_init_channel_0_config.h file (path: /$project/config/sl_si91x_pwm_init_channel_0_config.h).
>4. User can directly use APIs in application by passing appropriate structure members, if user doesn't want to configure from UC.

## Prerequisites/Setup Requirements

### Hardware Requirements

- Windows PC
- Silicon Labs Si917 Evaluation Kit [WPK(BRD4002) + BRD4338A]

### Software Requirements

- Simplicity Studio
- Serial console Setup
  - The Serial Console setup instructions are provided below:
Refer [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#console-input-and-output)

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

### Application Configuration Parameters

- Configure the following macros in pwm_example.c file and update/modify following macros if required.

  ```C
    #define EVENT_COUNT       10    // Count of events that can generate
    #define PRESCALE_A        0x100 // PWM Prescale_A value
    #define DEADTIME_A        0x08  // PWM deadtime_A
    #define FAULT_A_ENABLE    0x11  // Fault A enable for channel 0
    #define DT_COUNTER_A      0x00  // Dead time counter A enable
    #define DT_ENABLE         0x01  // Dead time enable for channel 0
    #define INTR_EVENT        0x01  // Rise PWM time period match channel 0 event
    #define DUTY_CYCLE_UPDATE 0x01  // Enable duty cycle updating bit in register
  ```

- Configure UC from the slcp component.
- Open **sl_si91x_pwm.slcp** project file select **software component** tab and search for **PWM** in search bar.
- By default, PWM has channel 0 instance. If base timer selection is selected as Timer (All Channels), to select any other PWM channel one should have PWM channel-0 as reference. If separate PWM channels are to be considered change the base timer selection to Timer (Each Channel). 
- Using configuration wizard, one can configure different parameters:

  - **Common Configuration for all PWM Channels**
    - Base Timer Selection: There are two base timers. Timer for each channel and Timer for all channels. If Timer (All Channels) is selected, make sure PWM channel-0 is installed.

  - **General Configuration for individual PWM Channel**
    - Frequency: Frequency changes from 500Hz to 200Khz. By default 25Khz is considered.
    - Output Polarity Low: There are 2 polarities- polarity low, polarity high. The difference can be observed in waveforms, when polarity is changed. By default it is set to polarity high.
    - Output Polarity High: There are 2 polarities- polarity low, polarity high. The difference can be observed in waveforms, when polarity is changed. By default it is set to polarity high.
    - PWM Mode: There are 2 modes, independent mode and complementary mode. The definitions for these modes are covered in about example code. The difference can be seen in waveform, when modes are changed.
    - Timer Counter: This is initial base time counter value to set. By default it is set to 0.
    - Duty Cycle: By default 50% duty cycle is take. One can vary duty cycle from 0% to 100%.
    - Base Timer Mode: There are 6 different modes. By default free run mode is selected.
    - Ext Trigger: In order to enable fault A, fault B and other external triggers present, ext trigger parameter is used.

    - **Pin Configuration for individual PWM Channel**
      - The pin configuration for PWM channel can be configured under SL_PWM_CHANNEL0 section.

      ![Figure: UC image](resources/uc_screen/pwm_uc_screen.png)

### PWM Pin Configuration

Tested on WPK Base board - 4002A and Radio board - BRD4338A.

- PWM channel-0 pin configuration.

  | Description   | GPIO    | Connector    |
  | ------------- | ------- | ------------ |
  | PWM_H         | GPIO_7  | P20          |
  | PWM_L         | GPIO_6  | P19          |

- PWM channel-1 pin configuration.

  | Description   | GPIO    | Connector    |
  | ------------- | ------- | ------------ |
  | PWM_H         | GPIO_9  | F9           |
  | PWM_L         | GPIO_8  | F8           |

- PWM channel-2 pin configuration.

  | Description   | GPIO    | Connector    |
  | ------------- | ------- | ------------ |
  | PWM_H         | GPIO_11 | F13          |
  | PWM_L         | GPIO_10 | F11          |

- PWM channel-3 pin configuration.

  | Description   | GPIO        | Connector   |
  | ------------- | -------     | ----------- |
  | PWM_H         | ULP_GPIO_7  | P12         |
  | PWM_L         | ULP_GPIO_6  | P13         |

> **Note:** Make sure pin configuration in RTE_Device_917.h file.(path: /$project/config/RTE_Device_917.h)

## Test the Application

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

1. Compile and run the application.
2. When the application runs, it generates periodic pulse waveform with 50% duty cycle.
3. Connect oscilloscope to GPIO_6(P19) & GPIO_7(P20) and observe the PWM waveform.
4. After successful program execution the prints in serial console looks as shown below.

   ![Figure: Introduction](resources/readme/output1.png)

   ![Figure: Introduction](resources/readme/output2.png)


> **Note:**
>
> - Interrupt handlers are implemented in the driver layer, and user callbacks are provided for custom code. If you want to write your own interrupt handler instead of using the default one, make the driver interrupt handler a weak handler. Then, copy the necessary code from the driver handler to your custom interrupt handler.
