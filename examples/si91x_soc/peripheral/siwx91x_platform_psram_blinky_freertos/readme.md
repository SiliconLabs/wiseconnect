# SiWx91x Platform PSRAM Blinky FreeRTOS

## Table of Contents

- [SiWx91x Platform PSRAM Blinky FreeRTOS](#siwx91x-platform-psram-blinky-freertos)
  - [Purpose/Scope](#purposescope)
  - [FreeRTOS Architecture](#freertos-architecture)
  - [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
    - [Hardware Requirements](#hardware-requirements)
    - [Software Requirements](#software-requirements)
    - [Setup Diagram](#setup-diagram)
  - [Getting Started](#getting-started)
  - [Application Build Environment](#application-build-environment)
  - [PSRAM Linker Component Installation](#psram-linker-component-installation)
  - [Test the Application](#test-the-application)
  - [Troubleshooting](#troubleshooting)
  - [Resources](#resources)
  - [Report Bugs/Support](#report-bugssupport)

## Purpose/Scope

This example project demonstrates blinky application (GPIO/LED toggle) executing from PSRAM under FreeRTOS environment. By default, the Text and Data segments are placed in PSRAM.

## FreeRTOS Architecture

- On startup, `main.c` calls `sl_main_second_stage_init()` to initialize SDK components, then calls `app_init()`, which invokes `psram_blinky_example_init()`. That creates the `psram_blinky` CMSIS-RTOS2 thread with `osThreadNew()` (**`2048`**-byte stack, **`osPriorityLow1`**).
- [`app_process_action()`] is a no-op; LED toggling runs entirely in the blink task.
- The task allocates a binary semaphore (**`BLINK_SEM_MAX`** **`1`**) and a periodic **`osTimer`** (**`psram_blnk`**). The timer callback releases the semaphore each period; the task blocks on **`osSemaphoreAcquire(..., osWaitForever)`**, then calls **`sl_si91x_led_toggle(LED_INSTANCE.pin)`**. Timer period comes from **`BLINK_DELAY_MS`** (converted to kernel ticks in **`blink_delay_ms_to_ticks()`**).

## Prerequisites/Setup Requirements

### Hardware Requirements

- Windows PC
- Silicon Labs SiWx91x Evaluation Kit [WPK(BRD4002) + BRD4342A]

### Software Requirements

- Simplicity Studio

### Setup Diagram

 ![Figure: setupdiagram](resources/readme/setupdiagram.png)

## Getting Started

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- [Install Simplicity Studio](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/using-the-simplicity-studio-ide#install-simplicity-studio)
- [Install WiSeConnect extension](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/using-the-simplicity-studio-ide#install-the-wiseconnect-3-extension)
- [Connect your device to the computer](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/using-the-simplicity-studio-ide#connect-siwx91x-to-computer)
- [Upgrade your connectivity firmware](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/using-the-simplicity-studio-ide#update-siwx91x-connectivity-firmware)
- [Create a Studio project](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/using-the-simplicity-studio-ide#create-a-project)

For details on the project folder structure, see the [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure) page.

## Application Build Environment

- Tune LED and delay macros in **psram_blinky_freertos.c** (compiler flags can also pre-define them).

  - `LED_INSTANCE`: Selects the LED instance used by the blinky thread. Defaults to **`led_led0`** if undefined.

    ```c
      #ifndef LED_INSTANCE
      #define LED_INSTANCE led_led0
      #endif
    ```

  - **`BLINK_DELAY_MS`**: This sets the **`osTimer`** repeat interval in milliseconds (blink rate).

    ```c
      #ifndef BLINK_DELAY_MS
      #define BLINK_DELAY_MS 500U
      #endif
    ```
- Sections can be included in PSRAM by installing components present under "PSRAM Linker Configurations" from "SOFTWARE COMPONENTS" GUI. Same can be removed from PSRAM and placed into default memory by uninstalling the respective component from software component selection GUI.

## PSRAM Linker Component Installation

1. Open **siwx91x_platform_psram_blinky_freertos.slcp** (double-click).
2. Navigate to the SOFTWARE COMPONENTS tab within the interface.
3. Utilize the search bar to look for "PSRAM."
4. Install the necessary component sections located within the PSRAM Linker Configurations.

> ![Figure: section_component_installation](resources/readme/section_component_installation.png)

> **Note**: For recommended settings, please refer the [recommendations guide](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-prog-recommended-settings/).

## Test the Application

1. Compile and run the application.
2. The **`psram_blinky`** task blocks on a semaphore; a periodic **`osTimer`** callback releases it every **`BLINK_DELAY_MS`**, then the task toggles **`LED_INSTANCE`** (**`LED0`** by default).
3. **`LED0`** blinks on the WPK base board when that instance is selected.

> **Note!**
>
> LED0 will continuously be turned on and off.



> **Note:**
>
> - Interrupt handlers are implemented in the driver layer, and user callbacks are provided for custom code. If you want to write your own interrupt handler instead of using the default one, make the driver interrupt handler a weak handler. Then, copy the necessary code from the driver handler to your custom interrupt handler.

## Troubleshooting

- If the project does not build, ensure Simplicity Studio and the WiSeConnect extension are installed and the board is connected.
- If the device is not detected, reinstall the connectivity firmware and check USB drivers.

## Resources

- [WiSeConnect Getting Started](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/)
- [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/)
- [SiWx91x SoC Documentation](https://docs.silabs.com/wiseconnect/latest/)

## Report Bugs/Support

For issues and support, use the Silicon Labs Community or your normal support channel.

