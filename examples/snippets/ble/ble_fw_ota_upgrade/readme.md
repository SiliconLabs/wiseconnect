# BLE - FW OTA Upgrade

## High-Level Overview

This application demonstrates how to update the SiWx91x module firmware over-the-air (OTA) by receiving a firmware image from a remote BLE central (e.g., smartphone or PC tool).

## Table of Contents

- [High-Level Overview](#high-level-overview)
- [Table of Contents](#table-of-contents)
- [Purpose/Scope](#purposescope)
- [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
  - [Hardware Requirements](#hardware-requirements)
  - [Software Requirements](#software-requirements)
  - [SLCP files to use based on product type](#slcp-files-to-use-based-on-product-type)
  - [Setup Diagram](#setup-diagram)
- [Steps to Run Demo](#steps-to-run-demo)
  - [Getting Started](#getting-started)
  - [Configuration and setup](#configuration-and-setup)
  - [Steps for execution](#steps-for-execution)
    - [Build and Run](#build-and-run)
    - [Firmware File Format](#firmware-file-format)
    - [Firmware Upgrade with Si Connect Mobile App](#firmware-upgrade-with-si-connect-mobile-app)
    - [Firmware Upgrade with Python Script](#firmware-upgrade-with-python-script)
    - [Steps to Create a Combined Image](#steps-to-create-a-combined-image)
    - [Appendix](#appendix)
- [Troubleshooting](#troubleshooting)
- [Resources](#resources)
- [Report Bugs and Get Support](#report-bugs-and-get-support)

## Purpose/Scope

This application demonstrates how to update the SiWx91x module firmware over-the-air (OTA) by receiving a firmware image from a remote BLE central (e.g., smartphone or PC tool). The SiWx91x device runs as a BLE **peripheral** with an OTA GATT server; a central device connects and sends **TA** (transceiver/NWP), **M4** (MCU application), or **combined TA+M4** firmware to program flash and reboot.

Supported upgrade methods:

- **Si Connect mobile app** (Android / iOS) — scan, connect, and upload a **`.gbl`** firmware file.
- **Python PC tool** (in `tools/Python_script/`) — scan, connect, and send **`.rps`** or **`.bin`** firmware in chunks.
- **Combined TA+M4 image** — build with Simplicity Commander (see [Steps to Create a Combined Image](#steps-to-create-a-combined-image)); set `FW_UPGRADE_TYPE` to `COMBINED_FW_UP` before building this example.

## Prerequisites/Setup Requirements

### Hardware Requirements

- **Development PC** — Windows or Linux with Simplicity Studio for build, flash, and debug.
- SiWx91x Wi-Fi Evaluation Kit. The SiWx91x supports multiple operating modes. See [Operating Modes](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) for details.
  - SoC Mode:
    - Silicon Labs [BRD4325A, BRD4325B, BRD4325C, BRD4338A, BRD4339B, BRD4343A](https://www.silabs.com/)
    - Kits: SiWx917 AC1 Module Explorer Kit (BRD2708A)
  - PSRAM Mode:
    - Silicon Labs [BRD4340A, BRD4342A, BRD4325G](https://www.silabs.com/)
  - NCP Mode:
    - Silicon Labs [BRD4180B](https://www.silabs.com/);
    - Host MCU Eval Kit. This example has been tested with:
      - Silicon Labs [WSTK + EFR32MG21](https://www.silabs.com/development-tools/wireless/efr32xg21-bluetooth-starter-kit)
    - NCP Expansion Kit with NCP Radio boards
      - (BRD4346A + BRD8045A) [SiWx917-EB4346A]
      - (BRD4357A + BRD8045A) [SiWx917-EB4357A]
    - Interface and Host MCU Supported
      - SPI — EFR32 host MCU
- BLE smartphone for mobile OTA testing (Si Connect app).
- Serial terminal tool (e.g., Tera Term, Docklight) for viewing logs on SoC/PSRAM (WSTK VCOM) or NCP host VCOM.

### Software Requirements

- Embedded Development Environment.
- Download and install [Simplicity Studio](https://www.silabs.com/developers/simplicity-studio) with WiSeConnect extension.
- Download and install the Silicon Labs [Si Connect (formerly Simplicity Connect / EFR Connect App)](https://www.silabs.com/developers/simplicity-connect-mobile-app) in Android or iOS smartphones for BLE OTA firmware upgrade. Users can also use their choice of BLE apps available in Android/iOS smartphones.
- For Python script based OTA: Python 3.7.9 or above.

> **Note:** The provided mobile screenshots are from the Si Connect app; it is recommended to use the latest version.

### SLCP files to use based on product type

| Mode | Host / target | Project file (this example folder) |
|------|----------------|-------------------------------------|
| SoC | Application runs on SiWx91x. | `ble_fw_ota_upgrade_soc.slcp` |
| PSRAM | Application runs on SiWx91x with PSRAM-capable radio board. | `ble_fw_ota_upgrade_psram.slcp` |
| NCP (SPI) | Application runs on **EFR32** host; SiWx917 is the network co-processor over **SPI**. | `ble_fw_ota_upgrade_ncp.slcp` |

The table above lists the SLCP source files in this example folder for each product type. These `.slcp` files cannot be opened directly from the file system. In **all modes (SoC, PSRAM, and NCP)**, the project must be created from **File → New → Silicon Labs Project → Examples and Demos** in Simplicity Studio (WiSeConnect SDK). Select **BLE - FW OTA Upgrade** for your target (**SoC**, **PSRAM**, or **NCP**) and board. For NCP, also follow [Getting started with NCP mode](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/getting-started-with-ncp-mode).

### Setup Diagram

<img src="resources/readme/ble_fw_upgrade_ota_soc_ncp.png" alt="Setup diagram - SiWx917 SoC and NCP OTA" width="700">

## Steps to Run Demo

### Getting Started

Refer to the instructions [Getting Started with the WiSeConnect](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- [Install Simplicity Studio](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/using-the-simplicity-studio-ide#install-simplicity-studio)
- [Install WiSeConnect extension](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/using-the-simplicity-studio-ide#install-the-wiseconnect-3-extension)
- [Connect your device to the computer](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/using-the-simplicity-studio-ide#connect-siwx91x-to-computer)
- [Upgrade your connectivity firmware](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/using-the-simplicity-studio-ide#update-siwx91x-connectivity-firmware)
- [Create a Studio project](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/using-the-simplicity-studio-ide#create-a-project) from **Examples and Demos** in Simplicity Studio. Choose **BLE - FW OTA Upgrade (SoC)**, **BLE - FW OTA Upgrade (PSRAM)**, or **BLE - FW OTA Upgrade (NCP)** as appropriate for your board.

> **Note:** Ensure the SiWx91x module is loaded with the latest connectivity firmware as described in [SiWx91x Firmware Update](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/using-the-simplicity-studio-ide#update-siwx91x-connectivity-firmware).

For details on the project folder structure, see the [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure) page.

### Configuration and setup

The application can be configured to suit your requirements and development environment. Read through the following sections and make any changes needed.

- Open `ble_config.h` file and update/modify following macros,

  ```c
    #define RSI_BLE_OTA_FWUP_PROFILE  "BLE_OTA_FWUP"        // Device name advertised during BLE scanning
    #define FW_UPGRADE_TYPE           TA_FW_UP              // TA_FW_UP, M4_FW_UP or COMBINED_FW_UP
    #define RSI_BLE_ADV_INT_MIN       0x100                 // Min advertising interval
    #define RSI_BLE_ADV_INT_MAX       0x200                 // Max advertising interval
    #define CONNECTION_INTERVAL_MIN   6                     // Min connection interval (7.5 ms units)
    #define CONNECTION_INTERVAL_MAX   6                     // Max connection interval (7.5 ms units)
    #define CONNECTION_LATENCY        1                     // Slave latency (number of events)
    #define SUPERVISION_TIMEOUT       800                   // Supervision timeout (10 ms units)
  ```

  > **Note:** `ble_config.h` files are already set with desired configuration in respective example folders; user need not change for each example.

> **Note:** For recommended settings, please refer the [recommendations guide](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-prog-recommended-settings/).

### Steps for execution

#### Build and Run

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Build the application in Studio.
- Flash, run, and debug the application.

Follow the steps below for successful execution of the application:

### Verify BLE FW OTA upgrade Application as a Server

1. After the program runs, the device starts **BLE advertising** as an OTA GATT peripheral (server).

2. Connect a serial terminal for debug prints.

3. Open the **Si Connect** app on a smartphone and start scanning.

4. The device advertises with the name configured in `RSI_BLE_OTA_FWUP_PROFILE` (default: **`BLE_OTA_FWUP`**).

![BLE advertising](resources/readme/bleadv.png)

### Firmware File Format

Firmware format depends on the OTA tool:

| Tool | Accepted format | Notes |
|------|-----------------|--------|
| **Si Connect mobile app** | **`.gbl`** | A `.rps` firmware image can be used as-is by simply renaming its extension from `.rps` to `.gbl`. |
| **Python PC tool** | **`.rps`** or **`.bin`** | Script rejects other extensions. |

### Firmware Upgrade with Si Connect Mobile App

1. Launch the Si Connect (or Simplicity Connect) app and enable BLE. Start scanning and connect to the device advertising as `BLE_OTA_FWUP` (or your configured name).

2. After successful connection, the app discovers the GATT services exposed by the SiWx91x device. Verify that the **Silicon Labs OTA** service is present in the list of services. This service is used to transfer the firmware image from the mobile app to the device.

3. Open the **OTA Firmware** option in the app.

<br><img src="resources/readme/mobilebleadv.png" alt="" width=250 height=500><img src="resources/readme/bleconnection.png" alt="" width=250 height=500><br>

#### TA Firmware Upgrade

> **Note:** On the **OTA device firmware update** page in the Si Connect app, keep **Type = Partial** and **Mode = Reliability** (the defaults). **Full** and **Speed** are not used.

1. Use **Select application .gbl file** to choose the TA firmware (e.g., `SiWG917-B.2.16.5.0.0.14.gbl`).

   > **Note:** The Si Connect app accepts only `.gbl` firmware files. The released SiWx91x TA firmware is provided as a `.rps` file in `<SDK>/connectivity_firmware/`; you can use it directly by renaming the extension from `.rps` to `.gbl` (the file contents remain unchanged).

2. Tap **Upload** to start the transfer. The app sends the firmware in chunks; the device programs flash and reboots when done.

<br><img src="resources/readme/taselectedfw.png" alt="" width=250 height=500><img src="resources/readme/tafwupcompleted.png" alt="" width=250 height=500><br>

3. Open the serial terminal (Tera Term / Docklight) connected to the device's VCOM and observe the OTA progress and success messages. A sample log is shown below for reference:

![TA OTA serial terminal output](resources/readme/bleotafwuptasocprints.png)

#### M4 Firmware Upgrade

> **Note:** On the **OTA device firmware update** page in the Si Connect app, keep **Type = Partial** and **Mode = Reliability** (the defaults). **Full** and **Speed** are not used.

1. Build an M4 application (e.g., **BLE - Heart Rate (SoC)**).

   > **Note:** The Si Connect app accepts only `.gbl` firmware files. The build output of the M4 application is a `.rps` file (e.g., `ble_heart_rate_profile_soc.rps`); you can use it directly by renaming the extension from `.rps` to `.gbl` (the file contents remain unchanged).

2. In Si Connect, use **Select application .gbl file** to choose the M4 **`.gbl`** file.

3. Tap **Upload** to start the transfer.
4. After completion, the device reboots and runs the new M4 application.

<br><img src="resources/readme/bleotaseltectedm4firmware.png" alt="" width=250 height=500><img src="resources/readme/m4fwupcompleted.png" alt="" width=250 height=500><br>

5. Open the serial terminal connected to the device's VCOM and observe the OTA progress and success messages. A sample log is shown below for reference:

![M4 OTA serial terminal output](resources/readme/bleotafwupm4socprints.png)

> **Note:** M4 firmware upgrade is supported in **SoC and PSRAM** modes only. **NCP supports TA firmware upgrade only.**

### Firmware Upgrade with Python Script

A Python-based OTA tool is provided to run on a PC. It uses BLE to scan for the SiWx91x device, connect to it, and transfer the TA or M4 firmware image in chunks.

#### Prerequisites

- **Python 3.7.9 or above** installed on the PC (Windows or Linux).
- A BLE-capable adapter on the PC (built-in or USB Bluetooth dongle).
- Install the required Python packages:

  ```sh
  pip install bleak pillow
  ```

  > **Note:** `tkinter` is part of the standard Python installation; on Linux you may need to install it separately (e.g., `sudo apt install python3-tk`).

#### Launch the Python Script

1. Navigate to the Python script directory in the SDK:

   ```
   <SDK>/examples/snippets/ble/ble_fw_ota_upgrade/tools/Python_script
   ```

   ![Python script directory](resources/readme/pythonscriptpath.png)

2. Run the script `Si917-OTA Firmware Update Python Script.py` from a command prompt / terminal opened in this folder:

   ```sh
   python "Si917-OTA Firmware Update Python Script.py"
   ```

   The OTA tool window opens with a **START** button.

   ![Python script main window](resources/readme/bleotafwuppythonscript.png)

3. Make sure the SiWx91x device is powered on and advertising as **`BLE_OTA_FWUP`**, then click **START** in the tool.

   The tool starts scanning for nearby BLE devices advertising `BLE_OTA_FWUP`.

   ![Python script scanning](resources/readme/bleotafwuppyscanning.png)

4. Once the device is found, the tool connects to it and reads the device info — name, BD address, and current firmware version — over BLE.

   ![Python script connected](resources/readme/bleotafwupconnected.png)

#### TA Firmware Upgrade (Python)

By default the tool is configured for **TA (NWP/connectivity) firmware** upgrade.

1. Click **Update Firmware**. A file browser dialog opens; navigate to and select the TA firmware **`.rps`** file (e.g., `SiWG917-B.2.16.5.0.0.14.rps` from `<SDK>/connectivity_firmware/`).

   ![Browse for TA firmware](resources/readme/bleotafwupfirmwarebrowse.png)

   > **Note:** The Python tool accepts only `.rps` or `.bin` files for TA firmware. Files with any other extension are rejected.

2. Click **Start Firmware Update** in the dialog to begin the transfer.

   The tool sends the firmware to the device in chunks; progress is displayed in the tool window and on the PC terminal.

3. Wait for the **Firmware Update Successful** message in the tool. The device programs flash and reboots automatically once the transfer is complete.

4. Open the serial terminal connected to the device's VCOM and verify the OTA progress / success logs. A sample log is shown below for reference:

   ![TA OTA Python script serial terminal output](resources/readme/bleotafwuptasocpyscriptprints.png)

#### M4 Firmware Upgrade (Python)

> **Note:** M4 firmware upgrade is supported in **SoC and PSRAM** modes only. It is not supported in NCP mode.

1. Build an M4 example (e.g., **BLE - Heart Rate (SoC)**). The build output is a `.rps` file (e.g., `ble_heart_rate_profile_soc.rps`); use it as-is — no conversion or rename is required for the Python tool.

2. In the Python tool, click **Update Firmware** and select the M4 **`.rps`** file from the file browser dialog.

3. Click **Start Firmware Update** to begin the transfer. The tool sends the M4 image in chunks; the device programs flash and reboots automatically on success.

4. Verify the OTA progress / success logs on the serial terminal. A sample log is shown below for reference:

   ![M4 OTA Python script serial terminal output](resources/readme/bleotafwupm4pyscriptprints.png)

> **Note:** The provided Python screenshots may differ slightly from the latest script version; the workflow remains the same.

### Steps to Create a Combined Image

#### Case: When Security is Disabled

1. Navigate to the **Simplicity Commander** directory (where the `commander` tool is installed).

2. Copy the **NWP (TA) firmware** `.rps` and **M4 application** `.rps` into that directory.

3. Create **`nwp_combined_image.rps`**:

   ```sh
   commander rps convert nwp_combined_image.rps --taapp <original non-encrypted TA rps> --combinedimage
   ```

   Example:

   ```sh
   commander rps convert nwp_combined_image.rps --taapp SiWG917-B.2.16.5.0.0.14.rps --combinedimage
   ```

4. Create **`m4_combined_image.rps`**:

   ```sh
   commander rps convert m4_combined_image.rps --app <original non-encrypted M4 rps> --combinedimage
   ```

   Example:

   ```sh
   commander rps convert m4_combined_image.rps --app ble_heart_rate_profile_soc.rps --combinedimage
   ```

5. Create the **final combined image**:

   ```sh
   commander rps convert combined_image.rps --app m4_combined_image.rps --taapp nwp_combined_image.rps
   ```

The screenshot below shows the full sequence in Simplicity Commander on Windows:

![Combined TA + M4 image generation using Simplicity Commander](resources/readme/TA_M4_combined_image_generation.png)

If your tool requires `.gbl` format (e.g., Si Connect), simply rename **`combined_image.rps`** to **`combined_image.gbl`** before OTA.

## Appendix

- **SiWx91x NCP** supports **TA firmware upgrade only**.
- **SiWx91x SoC / PSRAM** supports **TA**, **M4**, and **combined TA+M4** firmware upgrades.
- **Si Connect** requires **`.gbl`** firmware files.
- **Python tool** accepts **`.rps`** or **`.bin`** firmware files only.
- Ensure the TA/M4 firmware version matches your SiWx91x module.
- Intermittent disconnections may be observed while upgrading the firmware. If the upgrade fails or disconnects, retry the OTA process.
- If intermittent disconnections persist, change the **BLE connection request parameters** in `ble_config.h` — adjust `CONNECTION_INTERVAL_MIN` / `CONNECTION_INTERVAL_MAX`, `CONNECTION_LATENCY`, and `SUPERVISION_TIMEOUT` to values better suited to your BLE central (a longer supervision timeout and a stable connection interval generally improve OTA reliability). Ensure the BLE central honors the requested parameters.
- It is recommended to **disable power save** on the SiWx91x device during the OTA firmware upgrade. The OTA example is delivered with power save disabled by default; do not enable it for the duration of the OTA transfer.
- Anti-rollback feature is not supported for this application.
- During the firmware upgrade, the mobile device running the Si Connect app should not enter sleep mode. Keep the screen on or disable sleep/auto-lock for the duration of the OTA transfer to avoid interrupting the upgrade.
- This application supports only a **single BLE connection** (one GATT client to the OTA server).
- This application does **not** provide a **GATT profile-fetch (service discovery)** provision; the GATT client must use the predefined OTA service and characteristic UUIDs to communicate with the device.
- If the upgrade fails or you send the wrong firmware (e.g., wrong SiWx91x variant, wrong flash size, or a corrupted file), disconnect, reconnect, and retry with the correct file.

- The application **auto-detects the firmware image type** (TA or M4) from the received file and triggers the appropriate **soft reset** when the transfer completes. For TA and M4 upgrades the customer does **not** need to configure `FW_UPGRADE_TYPE` in `ble_config.h` — just send the appropriate firmware file from Si Connect or the Python tool:
  - **TA firmware (connectivity / NWP)** — **`.gbl`** (Si Connect) or **`.rps`/`.bin`** (Python).
  - **M4 firmware (host application)** — **`.gbl`** (Si Connect) or **`.rps`/`.bin`** (Python). *(SoC / PSRAM only; not supported on NCP.)*
  - **Combined TA+M4 firmware** — produced with the Commander steps in [Steps to Create a Combined Image](#steps-to-create-a-combined-image); send the resulting **`.gbl`** (Si Connect) or **`.rps`** (Python).

## Troubleshooting

| Symptom | Things to check |
|--------|------------------|
| No connection / scan issues | Confirm the central scans for the name in `RSI_BLE_OTA_FWUP_PROFILE` (default `BLE_OTA_FWUP`). |
| Slow OTA or lower-than-expected transfer speed | 1. Adjust `CONNECTION_INTERVAL_MIN` / `CONNECTION_INTERVAL_MAX` in `ble_config.h` and update `SUPERVISION_TIMEOUT` accordingly (ensure the central honors them).<br>2. On iOS centrals, the connection interval must follow the [Apple Bluetooth Accessory Design Guidelines](https://developer.apple.com/accessories/Accessory-Design-Guidelines.pdf). |
| NCP: no boot or no HCI traffic | Update SiWx917 connectivity firmware; verify SPI wiring per [NCP getting started](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/getting-started-with-ncp-mode). Create **BLE - FW OTA Upgrade (NCP)** from **Examples and Demos** in Simplicity Studio, build the project, and flash the generated image on the **EFR32** host. |
| Build errors | Create the example from **Examples and Demos** for the correct kit (SoC vs PSRAM vs NCP); verify matching SDK / WiSeConnect versions. Build failures can also be caused by **toolchain issues** — check the **toolchain** configuration in Simplicity Studio (GCC / IAR / Keil compiler version and project build options) and ensure the correct toolchain is installed and selected for the project. |
| Flash errors | On **SoC/PSRAM**, flash can fail when the **M4 is in sleep** (e.g., the previously running firmware had power save enabled). Put the WSTK in **ISP mode** (hold **Reset** and **ISP**, release **Reset** first, then **ISP**) and flash over SWD with Simplicity Commander — see [ISP mode](https://docs.silabs.com/wiseconnect/latest/wiseconnect-software-reference-manual-siwx91x/chip-module-programming/#isp-mode) in the SiWx91x software reference manual. On **NCP**, build and flash the **EFR32** host project created from **Examples and Demos**. |


## Resources

1. [WiSeConnect getting started](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/)
2. [WiSeConnect developers guide — developing for Silicon Labs hosts](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/)
3. [Programming recommended settings](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-prog-recommended-settings/)


## Report Bugs and Get Support

Report issues and get help from the Silicon Labs community:

- [Silicon Labs Community](https://www.silabs.com/community)
