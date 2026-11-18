# SiWx91x Platform NVM3 Dual Flash

## Table of Contents

- [SiWx91x Platform NVM3 Dual Flash](#platform-siwx91x-nvm3-dual-flash)
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
    - [General Configuration](#general-configuration)
  - [Test the Application](#test-the-application)
  - [Troubleshooting](#troubleshooting)
  - [Resources](#resources)
  - [Report Bugs/Support](#report-bugssupport)
  - [Resources](#resources)

## Purpose/Scope

- This application demonstrates the use of Third Generation Non-Volatile Memory (NVM3) data storage in SiWx91x dual flash.

## Overview

The NVM3 provides a means to write and read data objects (key/value pairs) stored in Flash. Wear-leveling is applied to reduce erase and write cycles and maximize flash lifetime. The driver is resilient to power loss and reset events, ensuring that objects retrieved from the driver are always in a valid state. A single NVM3 instance can be shared among several wireless stacks and application code, making it well-suited for multiprotocol applications.

For more detailed information about NVM3, refer to [Third Generation NonVolatile Memory (NVM3) Data Storage](https://www.silabs.com/documents/public/application-notes/an1135-using-third-generation-nonvolatile-memory.pdf).

## About Example Code

- This example performs NVM3 init using nvm3_initDefault() API.
- Two counter objects are initialized using nvm3_writeCounter() API. One is used to track the number of writes and another is used for
  tracking number of deleted objects.
- After initializing counters, application indefinetly waits for user input.
- Refer to Test the Application section for more information on how to give input to application.

## Prerequisites/Setup Requirements

- To use this application following Hardware, Software and the Project Setup is required

### Hardware Requirements

- Windows PC
- Silicon Labs SiWx91x Evaluation Kit [WPK(BRD4002) + External Flash]

### Software Requirements

- Simplicity Studio
- Serial console Setup
  - For Serial Console setup instructions, refer to [link name](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/using-the-simplicity-studio-ide#console-input-and-output).

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

- Configure UC from the slcp component.

   ![Figure: Introduction](resources/uc_screen/si91x_nvm3_uc_screen.png)

- Open **sl_si91x_nvm3_dual_flash.slcp** project file select **software component** tab and search for **NVM3 for SiWx91x** in search bar.
- Using configuration wizard one can configure different parameters like:

### General Configuration

  Configure following macros from UC,
- NVM3_DEFAULT_CACHE_SIZE: Number of NVM3 objects to cache. To reduce access times this number should be equal to or higher than the number
  of NVM3 objects in the default NVM3 instance.
- NVM3_DEFAULT_MAX_OBJECT_SIZE: Max NVM3 object size that can be stored. This value should be greater than or equal to 204
- NVM3_DEFAULT_REPACK_HEADROOM: NVM3 Default Instance User Repack Headroom, Headroom determining how many bytes below the forced repack limit
  the user repack limit should be placed. The default is 0, which means the user and forced repack limits are equal.
- NVM3_DEFAULT_NVM_SIZE - Size of the NVM3 storage region in flash. This size should be aligned with the flash page size of the device.

- Configure the following application macros in `nvm3_app.c`:

- `MAX_OBJECT_COUNT`: Specifies the maximum number of data objects that can be stored in the NVM3 instance by the application. By default, it is set to 10.

  ```c
  #define MAX_OBJECT_COUNT  10                                  // Maximum number of data objects saved
  ```

- `CMD_INPUT`: Specifies the expected number of UART input arguments per command line entered by the user. By default, it is set to 3.

  ```c
  #define CMD_INPUT         3                                    // UART input command arguments
  ```

- `STRING_SIZE`: Specifies the maximum length (in bytes) of a single input string accepted by the application. By default, it is set to 260.

  ```c
  #define STRING_SIZE       260                                  // Maximum string size
  ```

> **Note**: For recommended settings, please refer the [recommendations guide](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-prog-recommended-settings/).

## Test the Application

- Users can give the following commands through VCOM:

  - For storing new key, type below syntax followed by enter
    write key string
    ex: write 1 SiliconLabs
  - For reading data from key, type below syntax followed by enter
    read key
    ex: read 1
  - For deleting key, type below syntax followed by enter
    delete key
    ex: delete 1
  - For displaying content stored and keys deleted, type below syntax followed by enter
    display
    ex: display
  - For deleting all data stored in NVM3, type below syntax followed by enter
    erase
    ex: erase
  - For repacking NVM3, type below syntax followed by enter
    repack
    ex: repack

- After successful program execution the prints in serial console looks as shown below.

    ![Figure: Introduction](resources/readme/output.png)

## Troubleshooting

- If the project does not build, ensure Simplicity Studio and the WiSeConnect extension are installed and the board is connected.
- If the device is not detected, reinstall the connectivity firmware and check USB drivers.
- If NVM3 operations fail, verify flash layout and that repack/erase commands are used as documented.

## Resources

- [WiSeConnect Getting Started](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/)
- [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/)
- [AN1135: Using Third Generation Non-Volatile Memory (NVM3) Data Storage](https://www.silabs.com/documents/public/application-notes/an1135-using-third-generation-nonvolatile-memory.pdf)

## Report Bugs/Support

For issues and support, use the Silicon Labs Community or your normal support channel.

