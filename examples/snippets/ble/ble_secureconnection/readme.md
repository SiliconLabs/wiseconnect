# BLE - Secure Connection

## High-Level Overview

This application demonstrates how to configure SiWx91x device in peripheral role and how to connect it to a remote device. By default, our module has enable the SMP secure connection is enabled in...

## Table of Contents

- [High-Level Overview](#high-level-overview)
- [Table of Contents](#table-of-contents)
- [Purpose/Scope](#purposescope)
- [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
  - [Hardware Requirements](#hardware-requirements)
  - [Software Requirements](#software-requirements)
  - [NCP mode: host application and project files](#ncp-mode-host-application-and-project-files)
  - [Setup Diagram](#setup-diagram)
- [Steps to Run Demo](#steps-to-run-demo)
  - [Getting Started](#getting-started)
  - [Configuration and setup](#configuration-and-setup)
  - [Steps for execution](#steps-for-execution)
- [Troubleshooting](#troubleshooting)
- [Resources](#resources)
- [Report Bugs and Get Support](#report-bugs-and-get-support)

## Purpose/Scope

This application demonstrates how to configure SiWx91x device in peripheral role and how to connect it to a remote device. By default, our module has enable the SMP secure connection is enabled in the module.

In this application, module connects with remote device and initiates SMP pairing process.  After successful SMP pairing, SMP encryption will be enabled in both Central and Peripheral device.

## Prerequisites/Setup Requirements

### Hardware Requirements

- Windows PC with Host interface(UART/ SPI/ SDIO).
  - SiWx91x Wi-Fi Evaluation Kit.
  - SoC Mode:
    - Silicon Labs [[BRD4002A](https://www.silabs.com/development-tools/wireless/wireless-pro-kit-mainboard?tab=overview) + [BRD4338A](https://www.silabs.com/development-tools/wireless/wi-fi/siwx917-rb4338a-wifi-6-bluetooth-le-soc-radio-board?tab=overview) / [BRD4343A](https://www.silabs.com/development-tools/wireless/wi-fi/siw917y-rb4343a-wi-fi-6-bluetooth-le-8mb-flash-radio-board-for-module?tab=overview) / [BRD4343C](https://www.silabs.com/development-tools/wireless/wi-fi/siw917y-rb4343c-wi-fi-6-bluetooth-le-8mb-flash-radio-board-for-module?tab=overview)]
    - Kits
      - SiWx917 AC1 Module Explorer Kit [BRD2708A](https://www.silabs.com/development-tools/wireless/wi-fi/siw917y-ek2708a-explorer-kit)
  - PSRAM Mode:  
    - Silicon Labs [[BRD4342A](https://www.silabs.com/development-tools/wireless/wi-fi/siwx91x-rb4342a-wifi-6-bluetooth-le-soc-radio-board?tab=overview)]
  - NCP Mode:
    - Silicon Labs [BRD4186C](https://www.silabs.com/development-tools/wireless/xg24-rb4186c-efr32xg24-wireless-gecko-radio-board?tab=overview)
    - Host MCU Eval Kit. This example has been tested with:
      - Silicon Labs [WSTK + EFR32MG21](https://www.silabs.com/development-tools/wireless/efr32xg21-bluetooth-starter-kit)
      - NCP Expansion Kit with NCP Radio boards
        - [[BRD8045A](https://www.silabs.com/development-tools/wireless/wi-fi/expansion-adapter-board-for-co-processor-radio-boards?tab=overview) + [BRD4346A](https://www.silabs.com/development-tools/wireless/wi-fi/siwx917-rb4346a-wifi-6-bluetooth-le-soc-4mb-flash-radio-board?tab=overview) / [BRD4357A](https://www.silabs.com/development-tools/wireless/wi-fi/siw917y-rb4357a-wi-fi-6-bluetooth-le-4mb-flash-radio-board-for-rcp-and-ncp-modules?tab=overview) / [BRD4357C](https://www.silabs.com/development-tools/wireless/wi-fi/siw917y-rb4357c-wi-fi-6-bluetooth-le-4mb-flash-radio-board-for-rcp-and-ncp-modules?tab=overview)]
  - Interface and Host MCU Supported
    - SPI - EFR32 
- BLE peripheral device with privacy and SMP pairing support.

### Software Requirements

- Embedded Development Environment
- Download and install the Silicon Labs [Simplicity Connect App(formerly EFR Connect App) or other BLE Central/Peripheral app.](https://www.silabs.com/developers/simplicity-connect-mobile-app ) in the android smart phones for testing BLE applications. Users can also use their choice of BLE apps available in Android/iOS smart phones.

> **Note:** The provided mobile screenshots are from the 2.8.1 version of the Simplicity Connect App(formerly EFR Connect App), it is recommended to use the latest version.

### NCP mode: host application and project files

| Mode | Host / target | Project file (this example folder) |
|------|----------------|-------------------------------------|
| SoC | Application runs on SiWx91x. | `ble_secureconnection_soc.slcp` |
| PSRAM | Application runs on SiWx91x with PSRAM-capable radio board. | `ble_secureconnection_psram.slcp` |
| NCP (SPI) | Application runs on **EFR32** host; SiWx917 is the network co-processor over **SPI**. | `ble_secureconnection_ncp.slcp` |

Open the `.slcp` for your kit from **`examples/snippets/ble/ble_secureconnection/`** in Simplicity Studio. For NCP, follow [Getting started with NCP mode](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/getting-started-with-ncp-mode).

### Setup Diagram

 ![Figure: Setup Diagram for BLE Secureconnection example](resources/readme/ble_secureconnection_soc_ncp.png)

## Steps to Run Demo

### Getting Started

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- [Install Simplicity Studio](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#install-simplicity-studio)
- [Install WiSeConnect extension](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#install-the-wi-se-connect-extension)
- [Connect your device to the computer](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#connect-si-wx91x-to-computer)
- [Upgrade your connectivity firmware ](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#update-si-wx91x-connectivity-firmware)
- [Create a Studio project ](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#create-a-project)

For details on the project folder structure, see the [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure) page.

### Configuration and setup

The application can be configured to suit your requirements and development environment. Read through the following sections and make any changes needed.

- Open `ble_sc.c` file and update/modify the following parameters:

  - `RSI_BLE_DEVICE_NAME` refers the name of the WiSeConnect device to appear during scanning by remote devices.

      ```c
      #define RSI_BLE_DEVICE_NAME                              "BLE_SMP_SC"
      ```

  - `RSI_BLE_SMP_IO_CAPABILITY` refers IO capability.

      ```c
      #define RSI_BLE_SMP_IO_CAPABILITY                        0x00
      ```

  - `RSI_BLE_SMP_PASSKEY` refers SMP Passkey

      ```c
      #define RSI_BLE_SMP_PASSKEY                              0
      ```

  - Following are the non-configurable macros in the application.

      ```c
      #define RSI_BLE_CONN_EVENT                               0x01
      #define RSI_BLE_DISCONN_EVENT                            0x02
      #define RSI_BLE_SMP_REQ_EVENT                            0x03
      #define RSI_BLE_SMP_RESP_EVENT                           0x04
      #define RSI_BLE_SMP_PASSKEY_EVENT                        0x05
      #define RSI_BLE_SMP_FAILED_EVENT                         0x06
      #define RSI_BLE_ENCRYPT_STARTED_EVENT                    0x07
      #define RSI_BLE_SMP_PASSKEY_DISPLAY_EVENT                0x08
      #define RSI_BLE_SC_PASSKEY_EVENT                         0X09
      #define RSI_BLE_LTK_REQ_EVENT                            0x0A
      ```
  - Power save configuration

    By default, the application is configured without power save.

      ```c
      #define ENABLE_NWP_POWER_SAVE 0
      ```

  - If user wants to run the application in power save, modify the following configuration.

      ```c
      #define ENABLE_NWP_POWER_SAVE 1
      ```

> **Note**: For recommended settings, please refer the [recommendations guide](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-prog-recommended-settings/).

### Steps for execution

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Build the application in Studio.
- Flash, run and debug the application.

Follow the steps for successful execution of the program:

1. After the program gets executed, Silicon Labs device will be in advertising state.

2. Connect any serial console for prints.

3. Open a Simplicity Connect App(formerly EFR Connect App) in the Smartphone and do the scan.Ensure that the device is not bonded prior . Open the bonded tab and if the application name appears then click on the three dots beside the name and select delete bond information.

4. In the App, Silicon Labs  will appear with the name configured in the macro "BLE_SMP_SC" or sometimes observed as Silicon Labs device as internal name "SimpleBLEPeripheral".

 ![](resources/readme/Scan.png)

5. Initiate connection from the App.

 ![](resources/readme/Connect.png)

6. Observe that the connection is established between the desired device and Silicon Labs device.When application sends a smp request accept it on remote side by clicking ok(pair) and after smp passkey display event .

 ![](resources/readme/Pairing_Request.png)

 ![](resources/readme/Pair.png)

7. Enter the passkey displayed on the console (host logs) on the remote mobile side.

 ![](resources/readme/output_1.png)

 ![](resources/readme/passkey.png)

8. SiWx91x will initiate SMP Procedure by giving a security request . The remote device will give a SMP Pair request and SiWx91x responds with a SMP pair response . SMP Passkey event , encryption event and security keys exchange event follow after it .

 ![](resources/readme/DUT_Passkey.png)
 
9. If SMP is successful, device sends SMP encrypt started event to host. If not success, device sends SMP failure event to host.

10. Refer the following images for console prints:
 
    ![](resources/readme/output.png)

## Troubleshooting

| Symptom | Things to check |
|--------|------------------|
| No connection / scan issues | Confirm the peer address type and `RSI_BLE_DEV_ADDR` / `RSI_REMOTE_DEVICE_NAME` match the peripheral; phones often use random addresses. |
| Pairing fails / SC behavior not observed | **Verify the peer supports LE Secure Connections** (Secure Connections pairing per Bluetooth Core Specification). This example expects SMP **Secure Connections** (ECDH-based SC); a peer that only supports **Legacy** pairing cannot exercise the same path. Use a central or peripheral stack that advertises SC support and completes SC pairing—typical on current phones and Silicon Labs BLE devices. |
| NCP: no boot or no HCI traffic | Update SiWx917 connectivity firmware; verify SPI/UART wiring per [NCP getting started](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/getting-started-with-ncp-mode). Flash the correct `*_ncp.slcp` (or `*_uart_ncp.slcp`) on the **EFR32** host. |
| Power save anomalies on NCP expansion board | See the power-save note under **Configuration and setup** and the *Getting started with SiWx91x NCP* guide. |
| Build or flash errors | Open the `.slcp` that matches your kit (SoC vs PSRAM vs NCP) and matching SDK / WiSeConnect versions. |


## Resources

1. [WiSeConnect getting started](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/)
2. [WiSeConnect developers guide — developing for Silicon Labs hosts](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/)
3. [Programming recommended settings](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-prog-recommended-settings/)


## Report Bugs and Get Support

Report issues and get help from the Silicon Labs community:

- [Silicon Labs Community](https://www.silabs.com/community)
