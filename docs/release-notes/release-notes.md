**WiSeConnect3\_SDK\_3.5.3-hotfix.1 Release Notes**

This hotfix summarizes changes in branch `hotfix/3.5.3-hotfix.1`.

This SDK package supports SoC Mode and NCP Mode for B0:

- In SoC Mode: Customer applications can run on Cortex M4. Wireless, network, and security stacks run on the wireless subsystem.
- In NCP mode: Wireless, network, and security stacks run on the Wireless subsystem. Customer applications run on the external host MCU.
- Silicon Labs' SiWx917 includes an ultra-low power Wi-Fi 6 plus Bluetooth Low Energy (BLE) 5.1 wireless subsystem and an integrated micro-controller application subsystem.

**Note**: Align Simplicity Studio and GSDK versions with the parent **3.5.3** package you are hotfixing.

**Release Details**

|**Item**|**Details**|
| :- | :- |
|Release date|21st April, 2026|
|SDK Version|3.5.3|
|Firmware Version Standard|1711.2.14.5.3.0.6|
|Firmware Version Lite |1711.2.14.5.3.2.6|
|Package Name|WiSeConnect3\_SDK\_3.5.3|
|GSDK Version |SiSDK-2025.6.2|
|Supported RTOS|FreeRTOS|

**Changes and Fixes**

This hotfix contains **two** change themes (single integration commit):

1. **Wi‑Fi host buffer pool and driver path** — Rework host buffer allocation against quota under critical sections, adjust default control/TX buffer quotas, initialize driver command-path locals safely, and correct async response handling so buffer metadata (`id`) matches the outstanding command.

2. **BLE, firmware crypto, and AWS IoT TLS port** — Improve BLE command send and TX-completion handling (including status from the host–NWP bus on failures); use stack-backed request buffers for GCM/TRNG and add TRNG locking on non-multithread builds; in the AWS IoT TLS wrapper, yield briefly before retrying `send` when the stack returns `ENOBUFS`.
