# SL QEI

## Table of Contents

- [SL QEI](#sl-qei)
  - [Table of Contents](#table-of-contents)
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
    - [Pin Configuration](#pin-configuration)
  - [Test the Application](#test-the-application)

## Purpose/Scope

- This application demonstrates the velocity, position count, index count, and direction measurement using the QEI module.

## Overview

- The QEI module provides the ability to measure the position and speed of a rotating encoder, which is essential for applications requiring precise motion control.
- It utilizes two input signals, Phase A and Phase B, to determine the direction of rotation, and an Index signal to mark specific positions.
- This example showcases how to configure the QEI module for real-time data acquisition, allowing for effective monitoring and control of motion systems.
- The QEI can process both incremental and absolute encoders, making it versatile for various applications in robotics, industrial automation, and consumer electronics.
- By leveraging the QEI’s high-resolution counting capabilities, users can achieve accurate position tracking and velocity measurement, enabling enhanced performance in dynamic environments.

## About Example Code

- This example demonstrates the use of the Quadrature Encoder Interface (QEI) to measure velocity, position, direction, and index counts from a quadrature encoder.
- Various parameters like phase A and phase B signals, index signal, and configuration settings can be managed using \ref sl_qei_config_t.
- The example code emulates the behavior of a quadrature encoder by toggling GPIO pins for phase and index signals, simulating real-world encoder outputs.
- Dummy pulses are generated on gpio's and fed to the QEI pins using ulp timer.
- The firmware version of the QEI API is fetched using sl_si91x_qei_get_version, which includes the release version, major version, and minor version, represented by  \ref sl_qei_version_t.
- A static function initializes GPIO pins using \ref sl_si91x_qei_stimulus_pin_mux_init, configuring the necessary pins for QEI operation.
- \ref sl_si91x_qei_init is called to initialize the QEI peripheral.
- After initialization, \ref sl_si91x_qei_set_configuration is called to set up the QEI parameters using a configuration structure \ref sl_qei_config_t.
- A callback function is registered using \ref sl_si91x_qei_register_callback, allowing the application to respond to QEI interrupt events.
- The current position, index count, and direction are retrieved during processing through \ref sl_si91x_qei_get_position_counter, \ref sl_si91x_qei_get_index_counter, and  \ref sl_si91x_qei_get_direction.
- If ENCODER_MODE macro is enabled, the application prints the current position, index, and direction of the encoder.
- If VELOCITY macro is enabled,the velocity is fetched using \ref sl_si91x_qei_get_velocity and printed to the console when the velocity computation is completed.
- The application continuously runs in a loop, simulating the encoder’s output and processing the data to provide real-time feedback on the encoder's state.

## Prerequisites/Setup Requirements

### Hardware Requirements

- Windows PC
- Silicon Labs Si917 Evaluation Kit [WPK(BRD4002) + BRD4338A]

### Software Requirements

- Si91x
- Simplicity Studio
- Serial console Setup
  - The Serial Console setup instructions are provided below:
Refer [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#console-input-and-output)

### Setup Diagram

![Figure: setupdiagram](resources/readme/setupdiagram.png)

## Getting Started

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- [Install Simplicity Studio](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#install-simplicity-studio)
- [Install WiSeConnect 3 extension](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#install-the-wi-se-connect-3-extension)
- [Connect your device to the computer](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#connect-si-wx91x-to-computer)
- [Upgrade your connectivity firmware ](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#update-si-wx91x-connectivity-firmware)
- [Create a Studio project ](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#create-a-project)

For details on the project folder structure, see the [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure) page.

## Application Build Environment

### Application Configuration Parameters

- Configure UC from the slcp component.
- Open **sl_si91x_qei.slcp** project file, select **Software Component** tab and search for **QEI** in search bar.
- Using configuration wizard one can configure different parameters like:

  ![Figure: UC screen](resources/uc_screen/qei_uc_screen.png)

- **QEI Configuration**
  - Quadrature Encoder Mode: This setting controls the operation of the Quadrature Encoder.  This setting enables or disables the Quadrature Encoder mode. When enabled, the system utilizes the QEI for position tracking based on the encoder's output.
    - 0: Enable - Activates the Quadrature Encoder Mode.
    - 1: Disable - Deactivates the Quadrature Encoder Mode and user can configure for Timer Mode instead.
  - QEI Encoding Mode: Configures the position counting method of the QEI.
    - SL_QEI_ENCODE_MODE_1X: 1x Counting 
    - SL_QEI_ENCODE_MODE_2X: 2x Counting 
    - SL_QEI_ENCODE_MODE_4X: 4x Counting 
  - QEI Position Counter Mode: Sets the size of the position counter.
    - SL_QEI_POS_CNT_32: 32-bit position counter.
    - SL_QEI_POS_CNT_16: 16-bit position counter.
  - QEI Digital Filter Bypass: Configures the digital filter for QEI signals.
    - SL_QEI_DIGITAL_FILTER: The digital filter is active.
    - SL_QEI_BYPASS_FILTER: The digital filter is bypassed.
  - QEI Velocity Start: Enables or disables velocity calculation.
    - 0: Disable - Disables velocity calculation.
    - 1: Enable - Activates velocity calculation.
  - Delta Time: Defines the time interval used for velocity calculations.

- Configure the following macros in [`qei_example.h`](https://github.com/SiliconLabs/wiseconnect/blob/master/examples/si91x_soc/peripheral/sl_si91x_qei/qei_example.h) file and update/modify following macros if required.

  ```C
    # define BLINK_RATE       1000     // 1000 ticks per second
    # define QEI_OPERATING_FREQ    30000000  // 30MHz is QEI module frequency
    # define QEI_IDX_REVOLUTIONS  10    // Number of index revolutions for simulation
    #define SL_TIMER_MATCH_VALUE 400000 // Timer match value for down-counter type
    # define ULP_TIMER_INSTANCE   0 // ULP Timer Instance

  ```

### Pin Configuration

|GPIO pin  | Description|
|--- | --- | 
|GPIO_7  [P20]   |Phase A Signal|
|GPIO_8  [F8]    |Phase B Signal|
|GPIO_9  [F9]    |Index Signal  |
|GPIO_65 [P16]   |Phase A Signal|
|GPIO_74 [P17]   |Phase B Signal|
|GPIO_72 [P15]   |Index Signal  |

> **Note**: For recommended settings, see the [recommendations guide](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-prog-recommended-settings/).

## Test the Application

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

1. Compile and run the application.
2. - Connect GPIO_7 to GPIO_65. This is phase A signal
   - Connect GPIO_8 to GPIO_74. This is phase B signal
   - Connect GPIO_9 to GPIO_72. This is index signal
3. After successful program execution the prints in serial console looks as shown below.

   > ![output](resources/readme/output_qei.png)

> **Note:**
>
> - Interrupt handlers are implemented in the driver layer, and user callbacks are provided for custom code. If you want to write your own interrupt handler instead of using the default one, make the driver interrupt handler a weak handler. Then, copy the necessary code from the driver handler to your custom interrupt handler.
