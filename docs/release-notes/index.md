# WiSeConnect3_SDK_3.0.9 Release Notes

This SDK package supports SoC Mode

- In SoC Mode: Customer Applications can run on Cortex M4. Wireless stacks and Networking stacks run on the Wireless subsystem.
- Silicon Labs' SiWx917 includes an ultra-low power Wi-Fi 6 plus Bluetooth Low Energy (BLE) 5.1 wireless subsystem and an integrated micro-controller application subsystem.
- Support for Phase 1 platform DX peripherals is added.
  > Note: To run the application please install the GSDK patch following below instructions:
  > - Install GSDK version 4.3.0.
  > - Open your terminal application.
  > - Go to your GSDK path and enter the following command.
      git apply --ignore-whitespace --ignore-space-change "extension/wiseconnect3/utilities/gsdk_service_patch/gsdk_service.patch"

> Note: The git apply command needs to be run to apply a GSDK patch not available with the current WiSeConnect 3 release.
> - Now you should be able to build and run Dx sample applications.

> Note: Release supports only Simplicity Studio 5.5 & above versions

## SoC Highlights

- **Operating Modes** : Wi-Fi STA, Wi-Fi AP, Concurrent(AP + STA), Wi-Fi STA+BLE(CoEx),
- **Wireless Protocols** : IEEE 802.11b, 802.11g, 802.11n , 802.11ax, BLE 5.1
- **MCU Peripherals** : ADC Static Mode, Analog Comparator, Blinky, Bod, CTS, DAC FIFO, DAC Static, OPAMP, GPIO, I2C, I2S, PWM, RTC, QEI, SPI, UART, USART, UDMA, WDT, Combined Peripheral App
  : ULP (Ultra Low Power) Peripherals - ulp_gpio, ulp_i2c, ulp_i2s, ulp_ir_decoder, ulp_spi, ulp_timer, ulp_uart, ulp_udma
  : UART and I2C Deepsleep Wakeup Without RAM Retention
- **Multiple Ultra Low Power Modes** : Wi-Fi 6 TWT (Target Wake Time) - for improved network efficiency and device battery life
- **Wi-Fi** : 2.4GHz Wi-Fi Support - WPA2/WPA3 Personal, Wi-Fi 6 OFDMA/MU-MIMO Support that enhances network capacity and latency
- **Cloud** : Amazon Cloud Connectivity Support, Amazon FreeRTOS
- **BLE** : BLE 5.1, BLE dual role (Central and Peripheral Support)
- **Integrated Stacks and Profiles** : Wi-Fi Stack, BLE Stack and Profiles, TCP/IP stack with TLS 1.3, HTTP/HTTPS, DHCP, MQTT Client, SNTP Client
- **IDE** : Simplicity Studio with Windows 10(64-bit)
- **TA/M4 Firmware Update Tool** : Simplicity Commander
- **Hardware kits to evaluate SoC Mode** : Si917-BRD4325B (Dual flash Radio board), Si917-BRD4325A, Si917-BRD4325G (Common Flash Radio board), Si917-BRD4338A (Common Flash Radio Board V2.0)

## Release Details

| Item                      | Details                                                        |
| ------------------------- | -------------------------------------------------------------- |
| Release date              | July 10th, 2023                                                 |
| API Version               | 3.0.9(Build 1)                                                 |
| Firmware Version          | 2.9.0.0.8                                                      |
| Package Name              | WiSeConnect3_SDK_3.0.9                                         |
| Supported RTOS            | FreeRTOS                                                       |
| Operating Modes Supported | Wi-Fi STA, Wi-Fi AP, Concurrent(AP + STA), Wi-Fi STA+BLE(CoEx) |

## Updating to this Release

This release consists of Wireless Library - The Library runs on internal Cortex M4 in SoC Mode and supports 45 sample examples.

## Features and Bug Fixes
- Added B0(V1.2) chipset support for platform examples.
- Addressed Matter issues
- SL DEINIT issue Resolved
- Implemented station_ping_v6 example

## Known Issues

- The current chipset support for all the DX peripherals is A0 and B0(V1.2). B0(V2.0) support shall be provided in subsequent releases.
- I2C High Speed mode is not working as expected.
- SSI bitrate above 10 Msps is not working as expected.
- Powersave Deepsleep issue with Common Flash B0 Board.
- DUT is getting hanged while continuously publishing data in embedded mqtt application during power save mode
- BLE notification and Continuous BG scan are not working as expected
- Ongoing efforts to update the readme file are in progress

### SoC

- The "Browser File Viewer" plugin has issues. It can't render the last image included in the markdown files and observed issues accessing some hyperlinks
- In SoC mode, M4 power save will work with 384k TA and 320 M4 memory configuration only. It is not supported for "448K TA and 256k M4" and "512k TA and 192k M4" memory configurations

### Network Stack

### Wi-Fi

- WPA3 connection takes 2.5 sec more than WPA2 connection
- Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.

### BLE

- For BLE, dual role scenario issues might occur when connecting as central if the advertising is also going out for the peripheral role. Central connections can be made after the peripheral connections are established and advertising is stopped.
- LMP timeout BLE disconnection observed with the latest Realme mobile handsets.

## Limitations and Unsupported Features

### Wi-Fi/Network Stack

#### Limitations

- Throughput not at par with WiSeConnect 2 in the present release
- Maximum of 3 SSL connections are supported in WiFi alone mode.
- TWT is not verified in Coex mode.
- BGscan is not verified with TWT Feature.
- SIO currently supports SPI and UART.
- Peripherals currently uses UDMA CMSIS driver.

#### Not supported

- DTIM skip feature in power save.
- NCP Support(SDIO, UART,Linux, 3rd party hosts)
- 917 Specific features(degugging utilities)
- Calibration SW APIs
- Keep Alive Functionality
- SNI
- DNS Clients
- Asynchronous API's(TCP,UDP,SSL),
- APIs for Crypto Hardware(ECDH, DH, SHA, AES)
- Stats APIs
- Multi-threading support for relevant APIs
- SoC Support (Hosted mode): support for switching between LWIP & internal stack in SS
- IPV6 Support (for network application protocols)
- Console application for customers
- Protocols Offload mode: HTTP Server(917 specific), DHCPv6 Server, SNMP, Socket reads up to 64k, POP3, DTLS, mDNS, Web Socket
- Power save support (Message based)
- Baremetal support (for EFR)
- PTA Coexistence with other wireless
- SL Net Hosted mode (SoftAP over LWIP)

### BLE

- For BLE, if the connection is established with a small connection interval (less than 15 ms), simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported.
- BLE maximum two concurrent connections are supported, which can be either a connection to two peripheral devices, to one central and one peripheral device or two central devices.
- BLE Slave latency value is valid up to 32 only.

## Terminology

| Term                            | Description                                                                                                                                                                                                    |
| ------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **New Features**                | These items are new to this release                                                                                                                                                                            |
| **Changes/Issues Fixed**        | Changes made to existing features found in previous software releases.                                                                                                                                         |
|                                 | Enhancements to existing product flow and feature set.                                                                                                                                                         |
|                                 | Bug fixes done in the Release                                                                                                                                                                                  |
| **Deprecated Items**            | Features or functions or APIs that are removed from the distributed release to align with the software roadmap                                                                                                 |
| **Known Issues**                | Features or functions that do not work as planned at the time of release. Workarounds may be offered to meet short-term development goals, but longer-term solutions will be added in future software releases |
| **Limitations/Recommendations** | Describes the limitations on product usage and recommendations for optimal use cases                                                                                                                           |

<br />

## Notes

This release supports 917 Common Flash (A0 - BRD4325A, B0 - BRD4325G and B0 - BRD4338A(V 2.0)) and Dual Flash (A0 - BRD4325B).
