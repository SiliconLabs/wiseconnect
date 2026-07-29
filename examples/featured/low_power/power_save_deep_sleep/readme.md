# Wi-Fi - Powersave Deep Sleep

## High-Level Overview

SiWx91x deep sleep example: Configure standby power save in unassociated mode without RAM retention and measure current consumption using Energy Profiler in SoC and NCP modes.

## Table of Contents

- [Wi-Fi - Powersave Deep Sleep](#wi-fi---powersave-deep-sleep)
  - [High-Level Overview](#high-level-overview)
  - [Table of Contents](#table-of-contents)
  - [Purpose/Scope](#purposescope)
  - [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
    - [Hardware Requirements](#hardware-requirements)
    - [Software Requirements](#software-requirements)
    - [Set up Diagram](#set-up-diagram)
  - [Getting Started](#getting-started)
    - [Instructions for Simplicity Studio IDE and Silicon Labs Devices (SoC and NCP Modes)](#instructions-for-simplicity-studio-ide-and-silicon-labs-devices-soc-and-ncp-modes)
    - [Instructions for Keil IDE and STM32F411RE MCU (NCP Mode)](#instructions-for-keil-ide-and-stm32f411re-mcu-ncp-mode)
  - [Application Build Environment](#application-build-environment)
  - [Test the Application](#test-the-application)
    - [Instructions for Simplicity Studio IDE and Silicon Labs Devices (SoC and NCP Modes)](#instructions-for-simplicity-studio-ide-and-silicon-labs-devices-soc-and-ncp-modes-1)
    - [Instructions for Keil IDE and STM32F411RE MCU](#instructions-for-keil-ide-and-stm32f411re-mcu)
  - [Application Output](#application-output)
    - [Using Simplicity Studio Energy Profiler for Current Measurement](#using-simplicity-studio-energy-profiler-for-current-measurement)
  - [Troubleshooting](#troubleshooting)
  - [Resources](#resources)
  - [Report Bugs and Get Support](#report-bugs-and-get-support)

## Purpose/Scope

This application demonstrates how to enable power save deep sleep profile with SiWx91x. This application configures the SiWx91x in standby power save mode (Unassociated mode (NWP)) without RAM retention to reduce the current consumption.

Refer to the datasheet for current consumption values without RAM retention.

## Prerequisites/Setup Requirements

### Hardware Requirements

- Windows PC
- (Optional) Power analyzer 
- **SoC Mode**:
  - Standalone
    - BRD4002A Wireless Pro Kit Mainboard [SI-MB4002A](https://www.silabs.com/development-tools/wireless/wireless-pro-kit-mainboard?tab=overview)
    - Radio Boards 
  	  - BRD4338A [SiWx917-RB4338A](https://www.silabs.com/development-tools/wireless/wi-fi/siwx917-rb4338a-wifi-6-bluetooth-le-soc-radio-board?tab=overview)
      - BRD4342A [SiWx917-RB4342A](https://www.silabs.com/development-tools/wireless/wi-fi/siwx91x-rb4342a-wifi-6-bluetooth-le-soc-radio-board?tab=overview)
      - BRD4339B [SiWx917-RB4339B](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/getting-started-with-at)
      - BRD4340A [SiWx917-RB4340A](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/getting-started-with-at)
      - BRD4343A [SiWx917-RB4343A](https://www.silabs.com/development-tools/wireless/wi-fi/siw917y-rb4343a-wi-fi-6-bluetooth-le-8mb-flash-radio-board-for-module?tab=overview)
      - BRD4343C [SiWx917-RB4343C](https://www.silabs.com/development-tools/wireless/wi-fi/siw917y-rb4343c-wi-fi-6-bluetooth-le-8mb-flash-radio-board-for-module?tab=overview)
  - Kits
  	- SiWG917 Dev Kit [BRD2605A](https://www.silabs.com/development-tools/wireless/wi-fi/siwx917-dk2605a-wifi-6-bluetooth-le-soc-dev-kit?tab=overview)
  	- SiWx917 Pro Kit [Si917-PK6031A](https://www.silabs.com/development-tools/wireless/wi-fi/siwx917-pro-kit?tab=overview)
  	
- **NCP Mode**:
  - Standalone
    - BRD4002A Wireless Pro Kit Mainboard [SI-MB4002A](https://www.silabs.com/development-tools/wireless/wireless-pro-kit-mainboard?tab=overview)
    - EFR32xG24 Wireless 2.4 GHz +10 dBm Radio Board [xG24-RB4186C](https://www.silabs.com/development-tools/wireless/xg24-rb4186c-efr32xg24-wireless-gecko-radio-board?tab=overview)
    - NCP Expansion Kit with NCP Radio Boards
      - [BRD4346A](https://www.silabs.com/development-tools/wireless/wi-fi/siwx917-rb4346a-wifi-6-bluetooth-le-soc-4mb-flash-radio-board?tab=overview) + [BRD8045A](https://www.silabs.com/development-tools/wireless/wi-fi/expansion-adapter-board-for-co-processor-radio-boards?tab=overview)
      - [BRD4357A](https://www.silabs.com/development-tools/wireless/wi-fi/siw917y-rb4357a-wi-fi-6-bluetooth-le-4mb-flash-radio-board-for-rcp-and-ncp-modules?tab=overview) + [BRD8045A](https://www.silabs.com/development-tools/wireless/wi-fi/expansion-adapter-board-for-co-processor-radio-boards?tab=overview)
      - [BRD4357C](https://www.silabs.com/development-tools/wireless/wi-fi/siw917y-rb4357c-wi-fi-6-bluetooth-le-4mb-flash-radio-board-for-rcp-and-ncp-modules?tab=overview) + [BRD8045A](https://www.silabs.com/development-tools/wireless/wi-fi/expansion-adapter-board-for-co-processor-radio-boards?tab=overview)
  - Kits
  	- EFR32xG24 Pro Kit +10 dBm [xG24-PK6009A](https://www.silabs.com/development-tools/wireless/efr32xg24-pro-kit-10-dbm?tab=overview)
  - STM32F411RE MCU
    - [STM32F411RE](https://www.st.com/en/microcontrollers-microprocessors/stm32f411re.html) MCU
    - NCP Expansion Kit with NCP Radio Boards
      - [BRD4346A](https://www.silabs.com/development-tools/wireless/wi-fi/siwx917-rb4346a-wifi-6-bluetooth-le-soc-4mb-flash-radio-board?tab=overview) + [BRD8045C](https://www.silabs.com/development-tools/wireless/wi-fi/shield-adapter-board-for-co-processor-radio-boards?tab=overview)
      - [BRD4357A](https://www.silabs.com/development-tools/wireless/wi-fi/siw917y-rb4357a-wi-fi-6-bluetooth-le-4mb-flash-radio-board-for-rcp-and-ncp-modules?tab=overview) + [BRD8045C](https://www.silabs.com/development-tools/wireless/wi-fi/shield-adapter-board-for-co-processor-radio-boards?tab=overview)
      - [BRD4357C](https://www.silabs.com/development-tools/wireless/wi-fi/siw917y-rb4357c-wi-fi-6-bluetooth-le-4mb-flash-radio-board-for-rcp-and-ncp-modules?tab=overview) + [BRD8045C](https://www.silabs.com/development-tools/wireless/wi-fi/shield-adapter-board-for-co-processor-radio-boards?tab=overview)
  - Interface and Host MCU Supported
    - SPI - EFR32 & STM32
    - UART - EFR32

### Software Requirements

- Simplicity Studio IDE - [Simplicity Studio IDE](https://www.silabs.com/developer-tools/simplicity-studio) (to be used with Silicon Labs MCU)
- Keil IDE - [Keil IDE](https://www.keil.com/) (to be used with STM32F411RE MCU)
- Serial Terminal - [Docklight](https://docklight.de/)/[Tera Term](https://ttssh2.osdn.jp/index.html.en) (to be used with Keil IDE)

### Set up Diagram

  ![Figure: Setup Diagram SOC and NCP Mode for Power Save Deep Sleep soc Example](resources/readme/setup_soc_ncp.png)

## Getting Started

### Instructions for Simplicity Studio IDE and Silicon Labs Devices (SoC and NCP Modes)

  Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- [Install Simplicity Studio](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#install-simplicity-studio).
- [Install WiSeConnect extension](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#install-the-wi-se-connect-extension).
- [Connect your device to the computer](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#connect-si-wx91x-to-computer).
- [Upgrade your connectivity firmware ](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#update-si-wx91x-connectivity-firmware).
- [Create a Studio project ](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#create-a-project).
  
For details on the project folder structure, see the [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure) page.

### Instructions for Keil IDE and STM32F411RE MCU (NCP Mode)

  - Install the [Keil IDE](https://www.keil.com/).
  - Download [WiSeConnect SDK](https://github.com/SiliconLabs/wiseconnect).
  - Update the device's connectivity firmware as mentioned [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/getting-started-with-ncp-mode).
  - Connect the SiWx91x NCP to the STM32F411RE Nucleo Board by following these steps:
   	- Connect the male Arduino compatible header on the carrier board to the female Arduino compatible header on the STM32F411RE Nucleo board.
    - Mount the NCP Radio board (BRD4346A/BRD4357A) onto the radio board socket available on the base board (BRD8045C).
    - After connecting all the boards, the setup should look like the image shown below:
      ![Figure: Setup](resources/readme/stm32_setup.png)
   	- Connect the setup to the computer.
  - Open the POWERSAVE DEEP SLEEP µVision project - **power_save_deep_sleep.uvprojx** by navigating to **WiSeConnect SDK → examples → featured → low_power → power_save_deep_sleep → keil_project**. 

## Application Build Environment

**Soc Mode**:

The M4 processor is set to sleep mode. To wakeup the M4 processor, use the method shown below:

- ALARM timer-based - In this method, an ALARM timer is run that wakes up the M4 processor periodically as configured in the Universal Configurator in 'Wakeup Source Configuration' under `WiSeConnect SDK v3.4.2 -> Device -> Si91x -> MCU -> Service -> Power Manager -> UULP Wakeup Sources -> PM Wakeup Source Configuration`, as shown below.

  ![Wakeup Source Configuration](resources/readme/wakeup_config_1.png)

  ![Alarm timer Configuration](resources/readme/wakeup_config_2.png)

- Configure the following parameter in **app.c**.

  - Power Profile

    ```c
    #define POWER_SAVE_PROFILE DEEP_SLEEP_WITHOUT_RAM_RETENTION
    ```

    > **Note** : By default DEEP_SLEEP_WITHOUT_RAM_RETENTION is enabled.

> **Note**: For recommended settings, please refer the [recommendations guide](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-prog-recommended-settings/).

## Test the Application

### Instructions for Simplicity Studio IDE and Silicon Labs Devices (SoC and NCP Modes)

Refer to the instructions [here](https://docs.silabs.com/simplicity-studio-5-users-guide/5.10.1/ss-5-users-guide-building-and-flashing/) to:

- Build the application.
- Flash, run, and debug the application.

### Instructions for Keil IDE and STM32F411RE MCU

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/getting-started-with-ncp-mode-with-stm32#build-an-application) to:

- Build the application.
- Set the Docklight up by connecting STM32's Serial COM port. This enables you to view the application prints.
- Flash, run, and debug the application.

## Application Output

**SoC Mode**:

  ![Application Prints Soc](resources/readme/application_prints_soc.png)
  
  ![Energy profiler Soc](resources/readme/power_meter_avg_current_consumption.png)

**NCP Mode**:

  ![Application Prints NCP](resources/readme/application_prints_ncp.png)

### Using Simplicity Studio Energy Profiler for Current Measurement
  
- After flashing the application code to the module, the Energy profiler can be used for current consumption measurements.

- From tools, choose Energy Profiler and click "OK".

    ![Figure: Energy Profiler Step 1](resources/readme/energy_profiler_step_1.png)

- From Quick Access, choose the Start Energy Capture option.

    ![Figure: Energy Profiler Step 2](resources/readme/energy_profiler_step_2.png)

    > **Note:** The target part and board name have to be reverted to default to flash application binary.
- Select board and click "OK".

  ![Figure: Energy Profiler Step 3](resources/readme/energy_profiler_step_3.png)
- Average current consumption measured in power-meter.

    ![output_prints](resources/readme/power_meter_avg_current_consumption.png)

> **Note:**
> - The reference images which are captured were measured in an isolated chamber and might vary in an open environment.
> - To achieve the lowest power numbers in unconnected sleep, in SoC mode, configure both NWP and M4 to sleep without RAM retention.
> - To achieve the datasheet number for the deepsleep with 352 k RAM retained, configure `RAM_LEVEL` to `SL_SI91X_RAM_LEVEL_NWP_BASIC_MCU_ADV` and M4 to sleep without RAM retention.
> - This can be configured by navigating to `WiSeConnect SDK v3.4.2 -> Device -> Si91x -> Memory Configuration`, as shown below:
>   ![set_ram_level_1](resources/readme/set_ram_level_1.png)
>   ![set_ram_level_2](resources/readme/set_ram_level_2.png)
> - A flash erase is required to flash any other application after the user runs the powersave application(s). If not, the module will not allow any application to be flashed.

## Troubleshooting

If you encounter issues while running the Firmware Update example, check the following:

- Verify that `DEFAULT_WIFI_CLIENT_PROFILE_SSID`, `DEFAULT_WIFI_CLIENT_CREDENTIAL`, and `DEFAULT_WIFI_CLIENT_SECURITY_TYPE` in `sl_net_default_values.h` match your access point (AP) settings.
- Confirm that `SERVER_IP_ADDRESS` and `SERVER_PORT` in `app.c` match the IP address and port of the TCP server on your PC.
- Start the TCP server on the PC before you flash and run the SiWx91x application so the firmware image is available when the device connects.
- Ensure the firmware image path you pass to the TCP server is correct and that the image version is compatible with the target device.
- Set `COMBINED_IMAGE` to `1` only when you use a combined network processor (NWP) and M4 host image from the same release package. Set it to `0` for NWP-only updates.
- On devices with 4 MB flash, do not use a combined image. Update the NWP image first, then update the M4 image separately.
- In Network Co-Processor (NCP) mode, update the NWP image first, then update the host image from the same release version.
- On Windows, use Cygwin to build the TCP server as described in [Build and Run the TCP Server (Windows PC)](#build-and-run-the-tcp-server-windows-pc).
- If the update become unresponsive after download, wait a few minutes while the device writes the new firmware to flash and reboots.

## Resources

- [WiSeConnect Getting Started Guide](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/)
- [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure)
- [WiSeConnect Recommended Settings Guide](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-prog-recommended-settings/)
- [Simplicity Studio Energy Profiler](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-tools-energy-profiler/)

## Report Bugs and Get Support

Report issues and get help from the Silicon Labs community:

- [Silicon Labs Community](https://www.silabs.com/community)
