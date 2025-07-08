# SL JOYSTICK

## Table of Contents

- [Purpose/Scope](#purposescope)
- [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
  - [Hardware Requirements](#hardware-requirements)
  - [Software Requirements](#software-requirements)
  - [Setup Diagram](#setup-diagram)
- [Getting Started](#getting-started)
- [Application Build Environment](#application-build-environment)
  - [Application Configuration Parameters](#application-configuration-parameters)
  - [Joystick on BRD4002A](#joystick-on-brd4002a)
- [Test the Application](#test-the-application)

## Purpose/Scope

This sample app demonstrates the use of the Joystick Driver. It prints the joystick position after every finite interval over serial console

## Prerequisites/Setup Requirements

### Hardware Requirements

- Windows PC
- Silicon Labs Si917 Evaluation Kit [WPK(BRD4002) + BRD4338A / BRD4342A / BRD4343A]

### Software Requirements

- Simplicity Studio
- Serial console Setup
  - For Serial Console setup instructions, refer [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#console-input-and-output).

### Setup Diagram

> ![Figure: Introduction](resources/readme/setupdiagram.png)

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
- Open **sl_si91x_joystick.slcp** project file select **software component** tab and search for **joystick** in search bar.
- Configure the UC as mentioned below.

Refer to   ![Figure: Introduction](resources/uc_screen/sl_joystick_uc_screen.png)

- Using configuration wizard one can configure different parameters like:
  - **Channel selection**
  - Joystick ADC channel: The selection of the ADC channel instance for the joystick can be adjusted between channels 1 and channel_16.
  > **Note:**
  - Make sure to install selected ADC channel/instance.

  - **Joystick Voltage value Configuration**
  - REFERENCE VOLTAGE: Vref magnitude expressed in millivolts. As per Joystick Hardware on Wireless Pro Kit, Vref = AVDD = 3300 mV.
  - CENTER POSITION: Center position value(mV).
  - NORTH POSITION: North/Up position value(mV).
  - SOUTH POSITION: South/Down position value(mV).
  - EAST POSITION: East/Right position value(mV).
  - WEST POSITION: West/Left position value(mV).
  - JOYSTICK_MV_ERR_CARDINAL_ONLY:Joystick error mV value for Cardinal Directions only.
### Pin Configuration

#### Pin Configuration of the WPK[BRD4002A] Base Board, and with radio board

The following table lists the mentioned pin numbers for the Si917 radio board. If you want to use a different radio board, refer to the board-specific user guide.

  | CHANNEL | PIN TO ADCP | 
  | --- | --- | 
  | 1 | ULP_GPIO_1 [P16] | 
  | 2 | GPIO_27 [P29] | 
  | 3 | ULP_GPIO_8 [P15] | 
  | 4 | GPIO_25 [P25] | 
  | 5 | ULP_GPIO_8 [P15] | 
  | 6 | ULP_GPIO_10 [P17] | 
  | 7 | GPIO_25 [P25] | 
  | 8 | GPIO_27 [P29] | 
  | 9 | GPIO_29 [P33] | 
  | 10 | GPIO_29 [P33] | 
  | 11 | ULP_GPIO_1 [P16] | 
  | 12 | ULP_GPIO_1 [P16] | 
  | 13 | ULP_GPIO_7 [EXP_HEADER-15] | 
  | 14 | GPIO_26 [P27] | 
  | 15 | GPIO_28 [P31] | 
  | 16 | GPIO_30 [P35] | 

### Joystick on BRD4002A

Image shows Joystick located on BRD4002A, position/direction to press the Joystick and pin for Joystick.

![Figure: Joystick Hardware](resources/readme/image520d.png)

## Test the Application

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

1. Compile and run the application.
2. Connect GPIO_26 [P36] (Joystick pin) to ULP_GPIO_1 [P16] for ADC channel input voltage.If SiWG915 connect ULP_GPIO_4 [P36] (Joystick pin) to GPIO_27 [P29] for ADC channel input voltage.
3. If the channel instance has changed, the ADC channel input pin will also change. Verify the pin that is configured on the channel configuration.
4. Press the Joystick on WPK at any direction (Center, North/Up, South/Down, East/Right, West/Left).
5. The application should print the pressed position/direction of joystick in console.
6. After successful program execution the prints in serial console looks as shown below.

    ![Figure: Introduction](resources/readme/output.png)

> **Note:**
>
> - Interrupt handlers are implemented in the driver layer, and user callbacks are provided for custom code. If you want to write your own interrupt handler instead of using the default one, make the driver interrupt handler a weak handler. Further, copy the necessary code from the driver handler to your custom interrupt handler.
