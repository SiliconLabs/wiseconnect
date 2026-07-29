# SiWx91x Platform Simple Button FreeRTOS

## Table of Contents

- [SiWx91x Platform Simple Button FreeRTOS](#platform-siwx91x-simple-button-freertos)
  - [Table of Contents](#table-of-contents)
  - [Purpose/Scope](#purposescope)
  - [About Example Code](#about-example-code)
  - [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
    - [Hardware Requirements](#hardware-requirements)
    - [Software Requirements](#software-requirements)
    - [Setup Diagram](#setup-diagram)
  - [Getting Started](#getting-started)
  - [Application Build Environment](#application-build-environment)
    - [Configuration](#configuration)
  - [Test the Application](#test-the-application)
  - [Troubleshooting](#troubleshooting)
  - [Resources](#resources)
  - [Report Bugs / Support](#report-bugs--support)

## Purpose/Scope

This example application demonstrates the use of a BUTTON in a **FreeRTOS** environment. The application toggles the LED on each BUTTON press using the [sl_si91x_led_toggle()](https://docs.silabs.com/wiseconnect/latest/wiseconnect-api-reference-guide-si91x-peripherals/led#sl-si91x-led-toggle) function, with synchronization handled by a CMSIS-RTOS2 semaphore.

## About Example Code

- `simple_button_freertos.c` — Contains the FreeRTOS task and button ISR callback.
- On startup, `app_init()` calls `button_init()`, which creates the FreeRTOS task (`simple_button_task`) using `osThreadNew()`.
- The task performs the following steps:
  1. **Semaphore creation** — Creates a binary semaphore (`button_sem`, initial count 0) that the button ISR will release.
  2. **Wait loop** — Blocks on `osSemaphoreAcquire(button_sem, osWaitForever)`, sleeping until a button press occurs.
  3. **LED toggle** — When the semaphore is released, toggles the LED via `sl_si91x_led_toggle()` and loops back to wait.
- The button ISR (`sl_si91x_button_isr`) fires on a press event (`BUTTON_PRESSED`) and releases the semaphore to unblock the task.
- If semaphore creation fails, the task calls `osThreadExit()` to cleanly terminate.
- This approach uses RTOS-friendly blocking, allowing other tasks to run while waiting for input.

## Prerequisites/Setup Requirements

### Hardware Requirements

- Windows PC
- Silicon Labs SiWx917 Evaluation Kit [[BRD4002](https://www.silabs.com/development-tools/wireless/wireless-pro-kit-mainboard?tab=overview) + [BRD4338A](https://www.silabs.com/development-tools/wireless/wi-fi/siwx917-rb4338a-wifi-6-bluetooth-le-soc-radio-board?tab=overview) / [BRD4342A](https://www.silabs.com/development-tools/wireless/wi-fi/siwx91x-rb4342a-wifi-6-bluetooth-le-soc-radio-board?tab=overview) / [BRD4343A](https://www.silabs.com/development-tools/wireless/wi-fi/siw917y-rb4343a-wi-fi-6-bluetooth-le-8mb-flash-radio-board-for-module?tab=overview) / [BRD4343C](https://www.silabs.com/development-tools/wireless/wi-fi/siw917y-rb4343c-wi-fi-6-bluetooth-le-8mb-flash-radio-board-for-module?tab=overview)]
- SiWx917 AC1 Module Explorer Kit [BRD2708A](https://www.silabs.com/development-tools/wireless/wi-fi/siw917y-ek2708a-explorer-kit)

### Software Requirements

- Simplicity Studio

### Setup Diagram

![Figure: Introduction](resources/readme/setupdiagram.png)

## Getting Started

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- [Install Simplicity Studio](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/using-the-simplicity-studio-ide#install-simplicity-studio)
- [Install WiSeConnect extension](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/using-the-simplicity-studio-ide#install-the-wiseconnect-3-extension)
- [Connect your device to the computer](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/using-the-simplicity-studio-ide#connect-siwx91x-to-computer)
- [Upgrade your connectivity firmware](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/using-the-simplicity-studio-ide#update-siwx91x-connectivity-firmware)
- [Create a Studio project](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/using-the-simplicity-studio-ide#create-a-project)

For details on the project folder structure, see the [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure) page.

## Application Build Environment

The application toggles the selected LED on each selected button press.

### Configuration

- Open **siwx91x_platform_simple_button_freertos.slcp** project file, select the **Software Component** tab, and search for **BUTTON** in the search bar.
- Button Configure Interrupt: The button interrupt type can be configured. By default, Rise and Fall edge interrupt type has been set. One of the following can be selected:
  - Low level interrupt: Interrupt on low/pressed button state can be configured
  - High level interrupt: Interrupt on high/released button state
  - Low level and high level: Interrupt on low/pressed and high/released button state
  - Rise edge interrupt: Interrupt on rising edge of the button press
  - Fall edge interrupt: Interrupt on falling edge of the button press
  - Rise edge and fall edge interrupt: Interrupt on rising edge and falling edge of the button press

> ![Figure: Introduction](resources/readme/image600a.png)

- Configure the following macros in [`simple_button_freertos.c`](https://github.com/SiliconLabs/wiseconnect/blob/v4.1.1-content-for-docs/examples/si91x_soc/peripheral/platform_siwx91x_simple_button_freertos/simple_button_freertos.c) if required (each is guarded with `#ifndef` so you can also override them from the build):

  - `BUTTON_INSTANCE`: Selects the button instance whose press events unblock the FreeRTOS task and toggle the LED. By default, it is set to `button_btn0`.

    ```c
      #define BUTTON_INSTANCE button_btn0 // Button instance (e.g. button_btn1 when added in UC)
    ```

  - `LED_INSTANCE`: Selects the LED instance that is toggled on each button press. By default, it is set to `led_led0`.

    ```c
      #define LED_INSTANCE led_led0 // LED instance toggled on button press
    ```

## Test the Application

The project uses BTN0 by default. To change the button to BTN1, follow the below steps:

1. Add btn1 instance by traversing to the below path and select "Add New Instances":
    "Software Components -> WiSeConnect SDK -> Device -> Si91x -> MCU -> Drivers -> BUTTON"

   ![Figure: Adding Button Instance](resources/readme/image600d.png)

2. Change the definition of macro `BUTTON_INSTANCE` in the `simple_button_freertos.c` file to `button_btn1`.

3. Build and test the output.

   - LED0 will toggle when Button0 is pressed.

> **Note:**
>
> - In case of DEVKIT board [BRD2605A](https://www.silabs.com/development-tools/wireless/wi-fi/siwx917-dk2605a-wifi-6-bluetooth-le-soc-dev-kit?tab=overview), the blue LED toggles on button press. No other LED instance works for this board. To further use the DEVKIT LED, refer to the RGB LED application.
>
> - Interrupt handlers are implemented in the driver layer, and user callbacks are provided for custom code. If you want to write your own interrupt handler instead of using the default one, make the driver interrupt handler a weak handler. Then, copy the necessary code from the driver handler to your custom interrupt handler.
>
> - A maximum of 8 HP GPIO buttons, 8 ULP GPIO buttons, and 5 UULP GPIO buttons can be used simultaneously. This limitation arises from the number of available GPIO interrupts allocated for each GPIO type.
>
> - By default, the `SL_SI91X_BUTTON_DEBOUNCE` macro is disabled. If debouncing is needed, enable the `SL_SI91X_BUTTON_DEBOUNCE` macro in studio preprocessor settings.
>
> - In case of sleep-wakeup, call `button_init_instances()` and `led_init_instances()` after wakeup before handling button events or toggling LEDs so the configured button and LED instances are restored.

## Troubleshooting

- If the project does not build, ensure Simplicity Studio and the WiSeConnect extension are installed and the board is connected.
- If the device is not detected, reinstall the connectivity firmware and check USB drivers.

## Resources

- [WiSeConnect Getting Started](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/)
- [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/)
- [Si91x SoC Documentation](https://docs.silabs.com/wiseconnect/latest/)

## Report Bugs / Support

For issues and support, use the Silicon Labs Community or your normal support channel.


