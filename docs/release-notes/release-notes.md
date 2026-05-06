**WiSeConnect3\_SDK\_3.4.2-hotfix.1 Release Notes**

This hotfix summarizes changes in branch `hotfix/3.4.2-hotfix.1`.

This SDK package supports SoC Mode and NCP Mode for B0:

- In SoC Mode: Customer applications can run on Cortex M4. Wireless, network, and security stacks run on the wireless subsystem.
- In NCP mode: Wireless, network, and security stacks run on the Wireless subsystem. Customer applications run on the external host MCU.
- Silicon Labs' SiWx917 includes an ultra-low power Wi-Fi 6 plus Bluetooth Low Energy (BLE) 5.1 wireless subsystem and an integrated micro-controller application subsystem.

**Note**: Align Simplicity Studio and GSDK versions with the parent **3.4.2** package you are hotfixing.

**Release Details**

|**Item**|**Details**|
| :- | :- |
|Release date|6th May, 2026|
|SDK Version|3.4.2|
|Firmware Version Standard|1711.2.13.5.2.0.6|
|Firmware Version Lite |1711.2.13.5.2.2.6|
|Package Name|WiSeConnect3\_SDK\_3.4.2|
|GSDK Version |SiSDK-2024.12.2|
|Supported RTOS|FreeRTOS|

**Changes and Fixes**

This hotfix contains **two** change themes (single integration commit):

1. Added a configurable option to disable SoC LDO voltage switching.

2. Updated the SDK timeout for DNS command from 20 seconds to 80 seconds.
