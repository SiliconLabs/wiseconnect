# **WiSeConnect3\_SDK\_3.4.0 SoC Release Notes**

## **Release Details**

|**Item**|**Details**|
| :- | :- |
|Release date|16th December 2024|
|SDK Version|3\.4.0|
|Firmware Version|<p>Standard: 1711.2.13.3.0.0.11</p><p>Lite Wireless: 1711.2.13.3.0.2.11</p>|
|GSDK/SiSDK Version|SiSDK 2024.12.0|
|Studio Version|5\.10.0.0|
|Release Package Name|WiSeConnect3\_SDK\_3.4.0|
|Supported RTOS|FreeRTOS|
|Operating Modes Supported|Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP|

- SiWx917 release consists of two components:
  - SiWx91x Connectivity Firmware:
    - Standard Wireless Firmware - SiWx917 Firmware Binary available as SiWG917-B.2.13.3.0.0.11.rps
    - Lite Wireless Firmware - SiWx917 Firmware Binary is available as SiWG917-B.2.13.3.0.2.11.rps, this image is with reduced features for parts with SiWG917M110LGTBA OPN. 
  - Wiseconnect3 Library - Wiseconnect3 SDK library runs on internal Cortex M4

**Note:**

- Mandatory to upgrade the earlier version of boards (Si917-6031A Pro kit or BRD4338A boards) or 917 Silicon ICs with instructions as outlined in this document "SiWG917–TA\_Flash\_Memory\_Map\_ChangeGuide\_v1.3.pdf" for more details.  
- The release packages will have bug-fixes, enhancements, and new features in both 'SDK' and 'Firmware'. Customer shall update and use 'SDK' and 'Firmware' of same release package. SDK and FW combinations that are not released together are not supported.
- It is mandatory to update TA image first followed by M4 image and ensure application compatibility with firmware before OTA
- To use the Dev kit Demo, users need to have Simplicity Connect Mobile app version 2.9.3 or higher.

## **Supported Hardware**

|**Hardware**|**OPN (Ordering Part Number)**|
| :- | :- |
|QFN OPN|<p>QFN OPNs: SiWG917M111MGTBA, SiWG917M100MGTBA, SIWG917M110LGTBA(Lite OPN), SiWG917M111XGTBA, SiWG917M121XGTBA, SiWG917M141XGTBA</p><p>Module OPNs: SIWG917Y111MGNBA , SIWG917Y110LGNBA, SIWG917Y121MGNBA, SIWG917Y111MGABA, SIWG917Y110LGABA, SIWG917Y121MGABA</p>|
|Development Kits|<p>Pro Kit: SiWx917-PK6031A, Si917-PK6031A. </p><p>(Pro Kit includes Mother board "Si-MB4002A" + Radio board)</p><p>Radio boards: SiWx917-RB4338A, SiWx91x-RB4342A,</p><p>Dev Kit : SiWx917-DK2605A, </p><p>Module boards: SiW917Y-RB4343A</p><p>Module Explorer Kits: SiW917Y-EK2708A</p>|

## **Supported Features** 

<table><tr><th><b>Section</b></th><th><b>Sub-Section</b></th><th><b>Feature</b></th><th><p><b>Lite Wireless Firmware</b></p><p><b>(4MB flash OPN<br>OPN No: SiWG917M110LGTBA)</b></p></th><th><b>Standard Wireless Firmware (For other OPNs)</b></th></tr>
<tr><td rowspan="9">System</td><td rowspan="4">Operating modes</td><td>Wi-Fi STA (802.11ax, 802.11n)</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi 802.11n AP</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi STA (802.11ax, 802.11n) + BLE</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Security </td><td>Secure Boot, Secure Key storage and HW device identity with PUF, Secure Zone, Secure XIP (Execution in place) from flash, Secure Attestation, Anti Rollback, Debug Lock, Flash Protection</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Secure firmware upgrade options</td><td><p>- Firmware loading support by Commander Tool through Jlink Debugger. Jlink connected to Serial Wire Debug (SWD) </p><p>- Firmware loading via ISP using UART (Commander or Serial terminal), SPI Interface</p><p>- Secure Over the Air (OTA) Upgrade</p><p>- Firmware update via Bootloader</p></td><td>Supported</td><td>Supported</td></tr>
<tr><td>Combined Image update</td><td>-  Secure and non-secure combined Image update</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Crypto Support</td><td><p>- Crypto API's for Hardware Accelerators: Advanced Encryption Standard (AES) 128/256/192, Secure Hash Algorithm (SHA) 256/384/512, Hash Message Authentication Code (HMAC), Random Number Generator (RNG), SHA3, AES-Galois Counter Mode (GCM)/ Cipher based Message Authentication Code (CMAC), ChaCha-poly, True Random Number Generator (TRNG)</p><p>- Software Accelerators: RSA, ECC</p><p>- PSA Crypto APIs support for all crypto operations.</p><p>- Wrapping Secret keys (Symmetric crypto). </p><p>- ECDSA Sign and Verify APIs</p></td><td>Supported</td><td>Supported</td></tr>
<tr><td>System Power Save</td><td><p>- Deep Sleep with RAM retention and without RAM retention. </p><p>- Wireless Power Save: Connected Sleep (Wi-Fi Standby Associated), BLE Advertising with powersave, BLE Scan with powersave,  BLE connection with powersave. Only Max PSP power save mode is supported in BLE. </p></td><td>Supported</td><td>Supported</td></tr>
<tr><td rowspan="15">Wi-Fi</td><td>Wi-Fi Protocols</td><td>IEEE 802.11 b/g/n/ax (2.4GHz)</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Access Point (AP) Mode</td><td><p>- 4 Client Support, Hidden SSID Mode, Auto Channel Selection, Scan in AP mode (Alpha)</p><p>- Wi-Fi Security </p><p>- WPA2 Personal, WPA3 Personal (H2E method only) (Alpha), WPA Mixed mode (WPA/WPA2) </p></td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi Scan</td><td>Selective Scan, Active/Passive Scan</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi STA (Security Modes)</td><td>Open Mode, WPA2 Personal, WPA2 Enhancements, WPA3 Personal, Mixed Mode (WPA/WPA2), WPA3 Personal Transition Mode (WPA2/WPA3)</td><td>Supported</td><td>Supported</td></tr>
<tr><td>WPA2 Enterprise security (STA)</td><td>Method: PEAP/TTLS/TLS 1.0/TLS 1.2/FAST/LEAP</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>WPA3 Enterprise (STA)</td><td>Method: PEAP/TTLS/TLS 1.0/TLS 1.2/FAST/LEAP</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi STA Rejoin</td><td></td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi STA Roaming </td><td>BG Scan, OKC (Opportunistic Key caching), PMK (Pairwise Master Key) caching, Pre-Authentication</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi Protocol Power Save </td><td>Deep sleep (unconnected state), Max PSP, Enhanced Max PSP, Fast PSP, TWT</td><td>Supported</td><td>Supported</td></tr>
<tr><td>QoS</td><td>WMM-QoS</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi 6 Feature</td><td>MU-MIMO (DL), OFDMA (UL/DL), iTWT, TWT I-Frame & TWT Enhancements (Automatic TWT Configuration), BSS coloring, MBSSID</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi Concurrency </td><td>AP+STA (Same channel)</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi Band/Channels</td><td>2.4GHz CH1-11, 2.4GHz CH1-13, 2.4GHz CH1-14</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Known Security Vulnerabilities Handled</td><td>WPA2 KRACK Attacks, Fragment and Forge Vulnerability</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi Alliance </td><td>Wi-Fi Alliance Certification</td><td>Not Supported</td><td>Pre-Tested</td></tr>
<tr><td rowspan="6">Network stack</td><td rowspan="2">Core Networking Features</td><td><p>- IPv4/IPv6/UDP/TCP/ARP/ICMP/ICMPv6</p><p>- SSL client versions TLSV1.0, TLSV1.2, TLSV1.3 </p><p>- SSL server versions TLSV1.0 and TLSV1.2</p><p>- DHCPv4/DHCPv6 Client</p><p>- TCP/IP Bypass (LWIP as Hosted stack for reference)</p></td><td>Supported</td><td>Supported</td></tr>
<tr><td>- DHCPv4 Server, DHCPv6 Server</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td rowspan="2">Advanced Network Features</td><td>HTTP Client/HTTPS Client/DNS Client, Embedded MQTT/MQTT on host (AWS and AZURE) </td><td>Supported</td><td>Supported</td></tr>
<tr><td>SNTP Client, IGMP</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi IoT Cloud Integration</td><td><p>- AWS IOT Core</p><p>- Azure IoT</p></td><td>Supported</td><td>Supported</td></tr>
<tr><td>BSD and IoT sockets application programming interface(API)</td><td></td><td>Supported</td><td>Supported</td></tr>
<tr><td rowspan="2">BLE </td><td>Legacy features</td><td><p>- GAP(Advertising, Scanning, initiation, Connection and Bonding)</p><p>- Generic Attribute Protocol(GATT)</p><p>- Attribute protocol(ATT)</p><p>- Security</p><p>- LL Privacy 1.2</p><p>- Accept list</p><p>- Directed Advertising</p><p>- LE PHY(1Mbps, 2Mbps) & Coded PHY(125kbps, 500kbps)</p><p>- Simultaneous scanning on 1Mbps and Coded PHY</p><p>- LE dual role topology</p><p>- LE data packet length extensions(DLE)</p><p>- Asymmetric PHYs</p><p>- LE channel selection algorithm 2 (CSA#2)</p><p>- LE Secure connections</p></td><td>Supported</td><td>Supported</td></tr>
<tr><td>Advertising Extensions </td><td><p>- Extended Advertising</p><p>- Periodic Advertising</p><p>- Periodic Advertising scanning</p><p>- Extended Advertising scanning</p><p>- Periodic Advertising list</p><p>- LE periodic advertising synchronization</p></td><td>Supported </td><td>Supported</td></tr>
</table>

### **MCU**

- **Memory**
  - Common Flash: Single shared Flash for both Cortex-M4 and NWP (Network Wireless Processor)
  - Common Flash + External PSRAM
  - In-Package PSRAM + External Common Flash
- **Power States**
  - Active: PS4, PS3 and PS2
  - Standby: PS4, PS3, and PS2
  - Sleep: PS4, PS3 and PS2
  - Deep Sleep (Shutdown): PS0
- **Peripherals, Services and Hardware Drivers** 

|**HP Peripherals**|**Notes**|
| :- | :- |
|<p>- ADC</p><p>- Analog Comparator</p><p>- CRC</p><p>- DAC</p><p>- eFuse</p><p>- EGPIO</p><p>- GPDMA<sup>1</sup></p><p>- GSPI</p><p>- HRNG</p><p>- I2C</p><p>- I2S</p><p>- MCPWM</p><p>- PSRAM</p><p>- QEI</p><p>- SDIO Secondary</p><p>- SSI (Primary & Secondary)</p><p>- Temperature Sensor</p><p>- UART</p><p>- uDMA</p><p>- USART</p>||
|<p>- BoD<sup>1</sup></p><p>- Config Timer (CT)</p><p>- CTS (Touch Sensor)<sup>1</sup></p><p>- OPAMP<sup>1</sup></p>|Limited Support|
|**ULP Peripherals**||
|<p>- ULP\_ADC</p><p>- ULP\_DAC</p><p>- ULP\_GPIO</p><p>- ULP\_I2C</p><p>- ULP\_I2S</p><p>- ULP\_TIMER</p><p>- ULP\_UDMA</p><p>- ULP\_UART</p><p>- ULP\_SSI\_PRIMARY</p>||
|**UULP Peripherals**||
|<p>- RTC (Calendar)</p><p>- SYSRTC</p><p>- WDT</p>||
|**Services**||
|Sleep Timer||
|IOSTREAM||
|NVM3||
|LittleFS|Limited Support|
|Power Manager||
|Sensor Hub|Limited Support|
|Pin Tool||
|**Hardware Drivers**||
|LED, Button, MEMLCD, Joystick, Mic, Sensors (RHT, VEML, ICM)||

|The peripherals marked with superscript<sup>1</sup> are available through RSI APIs.  Support for SL APIs for user facing peripherals will be available in future releases.|
| :- |

|The flash write feature has been enhanced to support the NWP area, providing a 20k allocation within the NWP flash memory for storing user data. Moreover, a Read API has been introduced to retrieve data from the NWP flash region.|
| :- |

### **Developer Environment**

- Simplicity Studio IDE (SV5.10.0.0 version) and Debugger Integration. Refer to the latest version of the SoC "Getting-Started-with-SiWx917" guide for more details. 
- Recommended to install and use Silicon labs Simplicity SDK (Previously known as Gecko SDK), Git hub based version 2024.12.0.
- Simplicity Commander to support Flash loading, provision of MBR programming, security key management, and calibration support for crystal and gain offsets. Refer "siwx917-soc-manufacturing-utility-user-guide" for more details. 
- Advanced Energy Monitoring (AEM) to measure ultra-low power capability on Development boards (Pro Kit).
- PinTool for MCU pin configurations

### **BLE** 

- GAP(Advertising, Scanning, initiation, Connection and Bonding)
- Generic Attribute Protocol(GATT)
- Attribute protocol(ATT)
- Security
- LL Privacy 1.2
- Accept list
- Directed Advertising
- Extended Advertising
- Periodic Advertising
- Periodic Advertising scanning
- Extended Advertising scanning
- Periodic Advertising list
- LE periodic advertising synchronization
- LE PHY(1Mbps, 2Mbps) & Coded PHY(125kbps, 500kbps)
- Simultaneous scanning on 1Mbps and Coded PHY
- LE dual role topology
- LE data packet length extensions(DLE)
- Asymmetric PHYs
- LE channel selection algorithm 2 (CSA#2)
- LE Secure connections
- Bluetooth 5.4 Qualified

### **SDK**

- Simplified and Unified DX for Wi-Fi API and Platform APIs
- Simplifies application development and presents clean and standardized APIs
- UC (Universal Configurator) enables componentization, simplifying configuration of peripherals and examples
- BSD and ARM IoT-compliant socket API
- Available through Simplicity Studio and GitHub

### **Multi-protocol**

- Wi-Fi STA + BLE

**PTA/Coexistence**

- 3 wire CoEx acting as Wi-Fi with external Bluetooth
- 3 wire CoEx acting as Wi-Fi with external Zigbee/OT

## **Changes in this release compared to v3.3.4 Release**

### **System**

- **Enhancements / New features**
   - None
- **Fixed Issues**
  - Added SL\_SI91X\_EXT\_FEAT\_DISABLE\_XTAL\_CORRECTION bit to disable the automatic correction of the 40MHz crystal. This bit must be enabled during calibration or in end application when not using the recommended crystal part
- **Documentation**
  - None

### **MCU**

- **Enhancements / New features**
  - Added support for power manager in sensorhub 
  - Added support for clock scaling in Power manager for different M4 Power States 
  - Added support for Si917 Explorer Kit 
  - Added support for HRNG and QEI peripherals
  - Added support for External Host (CPC) on SDIO interface
  - Added support for programmable interrupts in icm40627 sensor
  - Added PSRAM execution support for NVM3 
  - Added support for Microphone driver as part of Dev-kit 
  - Added support for Pin-Annotation for LED driver 
  - Added Pintool changes for new peripheral and port additions 
  - Added UC support to configure UART instance and pins for debug logs 
  - Added API to disable the MEMLCD display
  - Added new M4 power state examples (PS4, PS3, PS2, PS1 and PS0) to obtain the datasheet numbers
  - Added support for PS2 State in Power Manager
  - Added support for configuring multiple SSI instances via UC
  - Updated the default M4 Reference Clock for from 32MHz RC to 40MHz XTAL and default M4 SoC clock from 32MHz to 180MHz
  - Updated the NWP reference clock to RC from XTAL in M4
  - Enhanced UC for I2C peripheral 
  - Removed clock selection options from the UC for SysRTC, WDT timer, ULP timer, and Calendar peripherals 

- **Fixed Issues**
  - Fixed ADC Init->Deinit issue after M4 sleep wakeup
  - Fixed issue with ADC sensors in the SensorHub example.
  - Fixed sampling duration deviation issue after state transitions in the ULP\_ADC example 
  - Implemented a timeout mechanism in I2C to avoid infinite waiting time in the sl\_i2c\_driver\_transfer\_data API. 
  - Fixed blocking mode issue in sl\_i2c\_driver\_send\_data\_non\_blocking API when sending data to a different address.
  - Fixed I2C functionality issues with different transfer types in leader and follower modes. 
  - Fixed ULP I2C DMA Send API issue in the sl\_si91x\_ulp\_i2c\_driver\_leader example when using DMA transfer type 
  - Resolved abnormal behavior when SL\_SI91X\_NPSS\_GPIO\_BTN\_HANDLER was not enabled during button-based wakeup functionality
  - Resolved multiple definitions of IRQ022\_Handler conflicts error during PM ULP SysRTC installation. 
  - Resolved low power reading issue in PS4 state for Performance and Power Save clock modes.
  - Fixed issue with WFI sleep when there is a  SysRTC pending interrupts
  - Resolved issue with bt\_stack\_bypass example  when UART1 instance used instead of USART0 for the HCI interface
  - Fixed timer application discrepancies on SiWG917Y111MGNBA BRD4343A. 
  - Fixed I2S loopback functionality for continuous TX and RX configuration use cases. 
  - Resolved incorrect GPIO configuration in the sl\_gpio\_set\_configuration API. 
  - Fixed app/SDK hang issue during HTTP POST data transfer alongside peripheral operations.
  - Resolved callback Un registration issue with the 32KHZ\_XTAL clock source in the ULP\_Timer. 
  - Removed the sl\_si91x\_soc\_soft\_reset() function from sl\_si91x\_hal\_soft\_reset.c. 
  - Corrected implementation of the sl\_si91x\_gpio\_clear\_uulp\_npss\_wakeup\_interrupt API. 
  - Fixed PWM mode functionality issue in the Config Timer application. 
  - Fixed application not entering PS0 state with ULP UART instance usage. 
  - Resolved NVM3 component installation issue when using memory pool buffers in wireless examples
  - Fixed I2S loopback failure in continuous data transfer with single TX/RX initialization.
  - Resolved issue with sl\_gpio\_set\_port\_output\_value() not clearing specific GPIO pins. 
  - Fixed timer callback triggering when counter's initial count exceeded the match value. 
  - Added NVIC reset functionality for module boards
  - Fixed UART/USART asynchronous Rx failure with flow control at a baud rate of 921600 and DMA disabled. 
  - Corrected SL\_GPIO\_INTERRUPT\_RISE\_FALL\_EDGE enum functionality 
  - Resolved missing GSPI DMA interrupt issue during context switches with high-priority tasks.
  - Resolved GSPI loopback issue where MSB 2 bits of the first byte on MISO were lost.
  - Addressed data comparison failures in I2S primary-secondary configuration with continuous data transfer.
  - Resolved ULP SSI Non-DMA transfer issue. 
  - Fixed clock drift issues when using XTAL as the clock source for calendar across sleep wakeups 

- **Documentation**
  - Addressed feedback on the guide for Developing with WiSeConnect™ SDK v3.x on the SiWx91x™ SoC for custom board implementations
  - Resolved Readme concerns in peripheral and service examples
  - Added documentation for initialization/deinitialization sequence
  - Updated documentation on using SWD to flash application and firmware binaries in ISP mode
  - Updated documentation about pin\_config.h for improved pin tool support
  - Updated the default clock configuration information in software reference manual 
  - Fixed inconsistencies and incorrect documentation in the UULP GPIO interrupt configuration

### **SDK**

- **Enhancements / New features**
  - Enhanced internal bus to handle multiple socket receive requests.  
  - Added new example named "Wi-Fi - Multithread Sockets (SoC)"   
  - Added option to configure azure MQTT Keepalive timeout in preprocessor settings.  
  - Added WPA3 security option on the login page for the concurrent\_http\_server\_provisioning application. 
  - Added support to configure default region configuration for AP and Concurrent mode. 
  - Updated socket option value TOS to use standard macros for both offload and hosted mode.  
  - Moved powersave\_standby\_associated application into low power example folder.  
  - Updated tls\_client example to use ALPN extension in TLS Protocol.
  - Added support for ALPN extension in TLS protocol and updated tls\_client example.
  - Added option to configure the event handler thread stack size using SL\_SI91X\_EVENT\_HANDLER\_STACK\_SIZE.  
  - Removed dependency of sl\_si91x\_internal\_stack component dependy on common flash. 
  - Updated enterprise\_client example with DHCP configuration. 
  - Added device\_need\_ram\_execution component to support all applications with no optimisation enabled.  
  - Added a new example named "Wi-Fi - Provisioning via Access Point (SoC)".  
  - Added SDK support for BRD2708A. 
  - Removed the dependency of the network manager component from the Wi-Fi component. 
  - Added support for Amazon root CA1 certificate to AWS applications. 
  - Added support for crypto PUF and AES.  
  - Added support for Wi-Fi commander demo binary.
  - Updated http\_otaf example to use multiple certificate indexes.  
  - Added support for handling remote termination in data\_transfer example.
  - Removed legacy power save sl\_si91x\_m4\_sleep\_wakeup API and replaced it with sl\_si91x\_power\_manager\_sleep in all Wi-Fi examples.  
  - Added PSRAM support for BRD4342a in m4\_fimrware\_update example.  
  - Added command support to configure 11ax, select region and BLE in CLI Demo
  - Removed the switch\_m4\_frequency () API to avoid clock frequency changes in wireless applications
  - New API sl\_si91x\_get\_nwp\_config added to get OPN part number 
  - The error codes for firmware upgrades have been modified, and new error codes have been introduced. Users who reference numeric status codes (e.g., 0x00) in their code will need to update their implementations. Please refer to the migration guide for detailed instructions. 
- **Fixed Issues**
  - Fixed internal memory deallocation when rejoining in aws\_mqtt application.
  - Fixed junk values when background scan is enabled. 
  - Fixed the issue where the parameter sl\_wifi\_advanced\_scan\_configuration\_t.enable\_instant\_scan was hardcoded to ENABLE\_INSTANT\_SCAN. Now, its value is dynamically taken as input from the user.  
  - Fixed memory alignment in sl\_wifi\_buffer\_t structure.  
  - Fixed Wi-Fi scan result shared with DUT terminal vs Silabs apps on 3rd party device while using wifi\_station\_ble\_provisioning and wifi\_station\_ble\_provisiong\_aws applications. 
  - Fixed BSD error EBADF when sending data in the concurrent HTTP server example.  
  - Fixed invalid credential ID checks for sl\_net\_get\_credential() API  
  - Fixed remote disconnection error while keeping the idle connection of Azure MQTT.  
  - Fixed the inability to change LI configuration without changing the performance profile.  
  - Fixed invalid command error for sl\_calib\_write command in the Wi-Fi calibration application.
  - Removed a check for interface status in sl\_wifi\_set\_listen\_interval() API.  
  - Fixed issue in setting data rate using sl\_wifi\_set\_transmit\_rate API in AP mode.  
  - Updated the PSK and SSID to use generic phrases in the CLI demo application, and reflected this change in the README file.  
  - Fixed issue in wifi\_power\_save\_deep\_sleep example which was failing with error 0x11 when enable\_hig\_performance() API is used. 
  - Fixed TCP/UDP send failure with error 55 (No Buffers available) in multithread\_sockets application. 
  - Fixed processing of space in HTTP header in sl\_http\_server\_get\_request\_headers() API. 
  - Fixed power save modes terminology as per the datasheet. 
  - Fixed thread local array ID overwrites when sl\_si91x\_wireless and sl\_iostream components are commonly used. 
  - Fixed handling socket close in case of  Wi-Fi disconnect or rejoin fail cases.
  - Fixed interface up failures in sl\_wifi\_set\_max\_tx\_power() API for AP mode.  
  - Fixed timeout error when secured combined Image is flashed through http\_otaf example. 
  - Fixed HTTP OTAF firmware upgrade failure with combined AWS certificate.  
  - Fixed si91x socket typedefs signature to align with standard naming conventions.
  - Fixed a timeout error that occurred when APUT was disconnecting a connected third-party client.  
  - Fixed issue where DUT is unable to receive more than 800 bytes of data from the azure cloud.  
  - Fixed issue where aws\_mqtt application gets struck when running in a loop.  
  - Renamed sl\_net\_dns\_get\_by\_hostname() to sl\_net\_dns\_resolve\_hostname() to user-friendly name.  
  - Fixed handling of flushing disconnected client socket connection in AP mode.  
  - sl\_si91x\_setsockopt\_async has been renamed to sl\_si91x\_setsockopt. sl\_si91x\_set\_custom\_sync\_sockopt() and sl\_si91x\_get\_custom\_sync\_sockopt() are replaced by setsockopt and getsockopt respectively.  
  - Fixed the issue with clock scaling feature in OOB example 
- **Documentation**
  - Updated the notes section regarding the certificate format to be used in the `sl_net_set_credential()` API. 
  - Updated all readmes with the usage of sl\_wifi\_region\_db\_config.h.
  - Updated usage note for SL\_WIFI\_MAX\_SCANNED\_AP define.   
  - Replaced all the macro references of SL\_WIFI\_STATS\_AYSNC\_EVENT with SL\_WIFI\_STATS\_ASYNC\_EVENT.
  - Added iperf description in the readme of all required examples.  
  - Updated wlan\_throughput example readme with corrected throughput images.  
  - Updated documentation for pre-signed URL in http\_otaf readme.
  - Added note for ADV\_SCAN support in AP mode in sl\_wifi\_start\_scan API.  
  - Updated documentation for BTR Tx/Rx application.  
  - Added TOS option -S 0xC8 in the README file for wlan\_throughput example.  
  - Added documentation option\_val for sl\_si91x\_set\_custom\_sync\_sockopt() API.  
  - Updated sl\_si91x\_select () and select() API descriptions.  
  - Updated documentation about non-support of  sl\_si91x\_set\_device\_region() for ACX boards.  
  - Updated note section of sl\_si91x\_setsockopt\_async()and sl\_si91x\_set\_custom\_sync\_sockopt() with limitation of API for sockets. 
  - Added documentation for BRD2708A support in WLAN & Coex example readme files. 
  - Updated HTTP\_OTAF to HTTP/HTTPS\_OTAF in titles of http\_otaf and http\_otad\_twt example.  
  - Updated the readme for wifi\_station\_ble\_provisioning\_aws with temperature sensor requirement.  
  - Updated wiseconnect source and header files with Zlib license.
  - Updated BSD socket documentation with non-supported features.  
  - Added readme to use higher baud rate 921600 by default in firmware\_flashing\_from\_host\_uart\_xmodem example.  
  - Updated limitation of OPEN security support for Wi-Fi credentials in sl\_net\_set\_credential() API.  
  - Updated the missing state and reason codes in 917 API documentation 
  - Updated the documentation for join\_tx\_power 
  - Moved API Structure and Data flow from Wireless to Summary section in API Reference Guide  
  - Added documentation for closing of sockets 
  - Updated the readme of TCP Tx on periodic wake up example 
  - Updated the readme with steps for configuring  mqtt\_hostname in OOB example 
  - Added readme for multi thread sockets example 
  - Update crypto PUF documentation 
  - Updated the WebSocket API documentation 

### **Wi-Fi/Network Stack**

- **Enhancements / New features**
  - Enhanced support for alternate certification chains for TLS has been implemented. With this update, only the peer certificate needs to validate against a trusted certificate, eliminating the need for full chain validation. This improvement allows the use of intermediate root CAs as trusted entities, enhancing flexibility in certificate management.
  - Added support to configure DHCP retry parameters (maximum retry count and retry interval) from the SDK
  - HMAC Support added for wrapped keys 
  - Added WPA3 Enterprise support in STA 
  - Added support for IPv6 address change notification.  
  - Added support for sl\_wifi\_get\_stored\_scan\_results() API.  
  - Added support for sl\_si91x\_debug\_log() API. 
  - Added support for sl\_net\_configure\_ip() and sl\_net\_get\_ip\_address() APIs. 
  - Added thread-safe support for socket select API in BSD, IoT and Si91x sockets. 
  - Added support for WebSocket client feature and example.  
  - Added support for ALPN extension in TLS protocol.

- **Fixed Issues**
  - Fixed issues in DNS when WLAN disconnect is issued from SDK.
  - The TCP maximum retry value can now be programmed to more than 31.
  - Fixed issues with the TCP retry in Power Save mode .
  - Fixed issues with TCP socket remote termination notifications during repeated socket open and close operations executed in a loop.
  - Fixed issue in concurrent mode where the STA's IPv6 configuration could fail in DHCP stateless mode if the STA initialized after the AP was already active.
  - Fixed an issue in concurrent mode where data transfer using a Link-local address always defaulted to the first IP interface created, regardless of the intended destination.
  - Fixed an issue where the STA in static IP configuration ignored the gateway address when connected to a third-party AP operating in stateless mode.
  - Stabilized the IPv6 DAD (Duplicate Address Detection) process during WLAN connection and disconnection sequences.
  - Resolved issues with socket data transfer in concurrent mode
  - Fixed an issue where DUT in concurrent - AP mode does not process the deauthentication frame from third party STA 
  - Fixed UDP traffic stall issue when 3rd party station connects and disconnects to APUT 
  - Fixed the issue of asynchronous events in concurrent mode, when host issues disconnect to the 3rd party station connected to APUT. 
  - Fixed issue where APUT responds to the Probe Request with DS IE containing a different channel than APUT operating channel 
  - Fixed issue with MQTT state mismatch in case of Wi-Fi disconnection. 
  - Fixed the issue of the device's hung state during Wi-Fi reconnections.  
  - Fixed handling of closing server socket to close all associated client sockets.  
  - Fixed AP start failure (0xf) using SL\_WIFI\_WPA2 security and SL\_WIFI\_DEFAULT\_ENCRYPTION encryption.
  - Fixed issue where DUT is not initiating authentication process and returned SL\_STATUS\_FAIL with PMK enabled.
  - Fixed error in MSS calculation for TCP sockets in sl\_si91x\_get\_socket\_mss() API.  
  - Fixed issue where HTTP headers aren't having proper carriage and line feed at the end of the request.  
  - Fixed issue where the net up and down of the AP DUT interface was causing UDP transfers to stop for the station DUT in concurrent mode. 
- **Documentation**
  - Updated the HTTP long URL documentation.
  - Updated the documentation for ICMP packets with large sizes.

### **BLE**

- **Enhancements / New features**
  - In BLE examples, the legacy power save API sl\_si91x\_m4\_sleep\_wakeup is removed and replaced with sl\_si91x\_power\_manager\_sleep API.
  - Added SiWx917 AC1 Module Explorer Kit Explorer Kit(EK2708A) support for all BLE examples. 
- **Fixed Issues**
  - Resolved the Just Works issue by removing the transmission of the passkey event to the Host as per specification.
  - Resolved DUT hang issue while BLE advertising and scanning with power save and encryption enabled.
  - Resolved issue with BLE AE periodic sync cancel and sync terminate API flow in the ble\_ae\_central application.
  - Fixed random hopping issue in PER mode.
  - Resolved SMP issue with 110L(Lite wireless firmware image) board. 
  - Fixed BLE indication and write with response data transfer issues with AE.
  - Resolved the issue where the DUT is not generating a passkey using the passkey entry method when attempting to connect with legacy paring. 
  - Fixed DUT hang issue when BLE TX/RX notifications transmitted continuously, with both TA and M4 power save and encryption are enabled in a dual role (1 central + 1 peripheral).
  - Resolved the BLE Set Local ATT Value API issue during BLE TX notifications with encryption enabled in the heart rate application.
- **Documentation**
  - Added a note that we removed the BLE packet transmission on channel-39 (2480MHz) at a 2Mbps data rate as per Specification in the  rsi\_ble\_per\_transmit\_s structure 
  - Segregated BLE macros and callbacks to groups with appropriate description. 
  - Updated sl\_wifi\_init API details in BLE files.  
  - Added Zlib license clause for Zephyr integrated BLE files

### **Multi-protocol**

- **Enhancements / New features**
  - None
- **Fixed Issues**
  - Resolved the BLE and WLAN connection failure with SMP when WLAN connect and HTTPS GET were called in a loop.
  - Resolved BLE bonding failure issue during continuous HTTPS download.
  - Fixed Si-Connect apk(2.9.2) hang issue with SIWG917Y110LGNBA. 
- **Documentation**
  - None

## **Recommendations**

### **System**

- The current revision of SiWx917 has:
  - RAM memory of 672k bytes which can be shared between TA and M4 processors in SoC mode. 
  - The below configurations are applicable in SoC mode and can be configured based on the application requirement. EXT\_FEAT\_352K\_M4SS\_320K is the default configuration, based on requirement EXT\_FEAT\_480K\_M4SS\_192K configuration is selected for SoC mode multi-protocol examples.
    - EXT\_FEAT\_480K\_M4SS\_192K - This mode configures TA with 480k and M4 with 192K bytes of memory
    - EXT\_FEAT\_416K\_M4SS\_256K - This mode configures TA with 416k and M4 with 256K bytes of memory
    - EXT\_FEAT\_352K\_M4SS\_320K - This mode configures TA with 352k and M4 with 320K bytes of memory
  - SoC mode should not use 672k\_M4SS\_0K memory configuration.
- Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
- There are 2 Versions of Pro-Kits/Radio boards. Si917-6031A based on Si917-4338A (Rev **A01 - A11**) and SiWx917-6031A based on SiWx917-4338A (Rev A12-A14). To get optimal power numbers, enable macro "SL\_SI91X\_ENABLE\_LOWPWR\_RET\_LDO" pre-processor define for ICs or while using SiWx917-6031A Pro-kit, SiWx917-4338A version of boards. This macro should be disabled for earlier variants of the board (Si917-6031A, Si917-4338A).
- With RAM configuration (EXT\_FEAT\_352K\_M4SS\_320K), only 352K memory is available to TA  which limits the features supported, Recommended to enable EXT\_FEAT\_416K\_M4SS\_256K in Wi-Fi + BLE Multi protocol mode to enable more Network features.
- For EXT\_FEAT\_416K\_M4SS\_256K  and EXT\_FEAT\_480K\_M4SS\_192K memory configurations, it is recommended to retain both TA and M4 RAMs in power save.

### **Wi-Fi/Network Stack**

- It is recommended to enable SL\_SI91X\_EXT\_TCP\_IP\_WAIT\_FOR\_SOCKET\_CLOSE BIT(16) of the 'Extended TCP IP Feature' bit map in the opermode command for all Wi-Fi Socket operations from the host to ensure graceful handling during asynchronous closures from the peer.
- For high throughputs,  it is recommended to enable BIT(2) - SL\_SI91X\_FEAT\_AGGREGATION  of feature\_bit\_map in opermode. 
- Users can enable SL\_SI91X\_EXT\_TCP\_IP\_SSL\_16K\_RECORD in 'Extended TCP IP Feature' bit map in opermode for (HTTPS server) supporting 16k record.
- **TWT**
  - Recommendation is to use sl\_wifi\_target\_wake\_time\_auto\_selection() API for all TWT applications. 
  - It is recommended to issue iTWT setup command once IP assignment, TCP connection, application specific socket connections are done.
  - When using sl\_wifi\_enable\_target\_wake\_time API, increase TCP / ARP Timeouts at the remote side depending upon the configured TWT interval configured. It's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() as an alternative.
  - In case of TWT in CoEx mode, when using sl\_wifi\_enable\_target\_wake\_time API, use TWT wake duration <= 16 ms and TWT wake interval >= 1 sec. If wake duration > 16 ms or TWT wake interval < 1sec, there might be performance issues.
  - For iTWT GTK interval in AP should be configured to max possible value or zero. If GTK interval is not configurable on AP side, recommended TWT interval (in case of sl\_wifi\_enable\_target\_wake\_time API) or RX Latency (in case of sl\_wifi\_target\_wake\_time\_auto\_selection API) is less than 4sec.
  - When sl\_wifi\_enable\_target\_wake\_time API is used, configuring TWT Wake interval beyond 1 min might lead to disconnections from the AP. Recommended to use TWT wake interval of less than or equal to 1 min.
  - When using sl\_wifi\_enable\_target\_wake\_time API, it is recommended to set missed\_beacon\_count of sl\_wifi\_set\_advanced\_client\_configuration API greater than 2 times of the configured TWT Interval.
  - DUT keepalive should be configured aligned with AP keepalive in TWT modes.
- Disable power save for high throughput applications or use FAST PSP power save mode as per application requirement.
- The application needs to ensure that it sets RTC with the correct timestamp before establishing the SSL/EAP connection.
- The minimum timeout value should not be less than 1 second for socket select and socket receive calls. 
- Please refer Keep alive intervals supported by MQTT broker and configure keep alive interval values accordingly.
- The minimum keep alive interval value recommended for embedded MQTT is 10 Seconds. 
- Disable power save and suspend any active TWT sessions before triggering HTTP OTAF.
- Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with the power save.
- Recommended to configure VAP\_ID properly for Si91x STA and AP using sl\_si91x\_setsockopt\_async(), in case of data transfer.
- Recommended to use valid length(<= 202 bytes) for topic to be published while using Embedded MQTT, else it leads to return wrong error code(0x21).
- In concurrent mode with dual IP, it is advised to bring up STA first (IP configuration) and AP later.
- It is recommended to configure Tx ,Rx , Global buffer pool ratio in the buffer config command based for all Wi-Fi Socket operations from the host
- It is recommended to use "TCP exponential backoff" configuration for congested channels.
- It is recommended is to disable broadcast filter during TCP connection to avoid ARP resolution issues
- To avoid IOP issues, it is recommended to disable power save before Wi-Fi connection.
- It is recommended to set region\_code as `IGNORE_REGION` in boot configurations for SIWG917Y module boards except for PER mode.
- The ICMP timeout should be set to more than 2 seconds when power save is enabled. In power save mode, packet drops are expected when the timeout is low.
- An intermediate CA should be a trusted CA, provided it is not a self-signed certificate. To be considered as a trusted CA, the KeyUsage field must be explicitly configured while generating the certificate
- It is recommended to enable RSI\_REJOIN\_FIRST\_TIME\_RETRY to minimize join failures, especially in scenarios such as abnormal connection termination caused by a module reset or power failures. 

### **BLE**

- In BLE, the recommended range of Connection Interval in
  - Power Save (BLE Only) - 100 ms to 1.28 s.
- In BLE, during Connection, the configuration of Scan Interval and Scan Window with the same value is not recommended. The suggested ratio of Scan Window to Scan Interval is 3:4.
- In BLE, if a device is acting as Central, the scan window (in set\_scan\_params and create\_connection commands) must be less than the existing Connection Interval. The suggested ratio of Scan Window to Connection Interval is 2:3.
- In BLE mode, if scanning and advertising are in progress on the SiWx91x module and it subsequently gets connected and moves to the central role, scanning stops else if it moves to the peripheral role, advertising stops. To further establish a connection to another peripheral device or to a central device, the application should give a command for starting advertising and scanning again.

### **Multi-protocol**

- For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
- Wi-Fi + BLE Advertising
  - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals may be observed.
  - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity.
- Wi-Fi + BLE scanning
  - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
  - BLE scanning will be stopped for intervals that collide with Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Connections
  - All standard connection intervals are supported.
  - For a stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Data Transfer
  - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout.
  - Ensure Wi-Fi activity consumes lower intervals.

### **MCU**

- The WDT manager is specifically meant for system reset recovery and should not be utilized for any other purpose. When interrupts are disabled, make sure to stop the WDT to avoid unintended resets. Once interrupts are re-enabled, restart the WDT to ensure system reliability
- It is strongly recommended to use `sl_si91x_soc_nvic_reset()` API for system soft reset
- For any wakeup source, ensure the corresponding peripheral component is installed in powersave applications
- Use both CTS and RTS for UART flow control
- PSRAM examples are not supposed to be used with Non-PSRAM OPNs
- SYSRTC and wake on wireless wakeup resources are enabled by default when tickeless idle mode is enabled. Avoid installing sleeptimer component in power save applications (where FreeRTOS tickless is enabled by default)
- Minimum idle-time to sleep is configured to100ms in FreeRTOS\_config.h when using FreeRTOS tickless mode
- Enable DMA to achieve better throughput numbers
- For I2C Fastplus and High speed modes, use high power instances with core clock frequencies between 80MHz and 180MHz
- Use sl\_si91x\_i2c\_receive\_blocking API instead of non-blocking API for receiving data via the I2C protocol.
- In RTOS environment, prefer signaling mechanisms (Semaphore/Mutex/EventFlag etc.) instead of "Variables/Flags" from user callbacks to detect "\*Transfer Complete\*" for high speed communication peripherals Refer Software Reference Manual for more details
- Install the 'device\_needs\_ram\_execution' component for OPN-based firmware update and powersave examples
- For IC or Module configurations, it is advised to use UULP\_GPIO\_3 when connecting to the 32kHz external oscillator
- For IC, the default LF\_FSM clock is configured to 32KHz XTAL. If the external oscillator connected to UULP\_GPIO\_3 is used as the 32kHz clock source, make the necessary code changes as detailed in the Software Reference Manual
- It is strongly recommended not to use RO clock in MCU
- When using ICs for BLE, CoEx, and/or high-accuracy MCU applications it is recommended to use external 32.768kHz crystal on XTAL\_32KHZ\_P and XTAL\_32KHZ\_N pins
- It is not advisable to use the sl\_si91x\_m4\_sleep\_wakeup() API for powersave applications, instead use sl\_si91x\_power\_manager\_sleep().
- CPC
  - Ensure that the Linux host is restarted and the secondary device is reset after installing the kernel module to ensure proper connectivity. 
  - Avoid exiting and re-running the daemon once the connection is established. Instead, restart the Linux host and reset the secondary device if any changes are needed. 
- Any existing users migrating to 3.4.0 and using SSI, need to install the new component for re-configuring the peripheral
- The default M4 SoC Clock is updated to 180MHz which configures M4 in PS4-Performance mode. Use Power Manager to change the clock as per the required Power State . For example, all wireless examples are configured to PS3-Powersave mode which configures the M4 SoC Clock to 40MHz
- When configuring the Memory sections to PSRAM, please ensure that the buffers part of these memory sections are not being updated by NWP or DMA. 
- Only the Text and Data segments are recommended to be configured for PSRAM when using Wireless Applications
- When executing applications from PSRAM, install PSRAM Core component and its respective device variant
- It is strongly recommended not to configure any M4 Clock from application context
- It is strongly recommended to migrate to new GPIO driver for configuring GPIO interrupt(s). IRQ Handlers are available in GPIO driver and it is recommended to use only registered callback functions in the application layer. Please follow the migration guide for more details.

## **Known Issues of WiSeConnect3\_SDK\_3.4.0 Release**

### **MCU**

**I2C**

- For I2C1, ULP\_GPIO6 and ULP\_GPIO7 pin combination is not working
- For I2C1, High-speed mode is not working with I2C Leader and Follower modes 

**SSI**

- Data lost and Modefault events are not working as expected 

**I2S**

- FIFO threshold configurations are not working

**SensorHub**

- Power transitions with AWS are not stable in SensorHub

**ULP Timer**

- Observing Timer drifts beyond acceptable range

**CPC**

- CPCd NETLINK-SDIO is stable on Linux kernel v4.x but may experience stability issues on Linux kernel v6.x

### **SDK**

- Observed Wi-Fi connection is successful even after deleting the stored network credentials using sl\_net\_delete\_credential and responding with SL\_NET\_INVALID\_CREDENTIAL\_TYPE for sl\_net\_get\_credential. 
- Enhanced sl\_wifi\_get\_firmware\_version() API to provide more details (ROM ID, chip ID, security version, etc) which is not backward compatible with firmware older than 1711.2.10.1.0.0.4. Firmware binary notation does not include the security version number.
- In PSRAM enabled demos, moving of text, data and stack segments to PSRAM is allowed. BSS and Heap should still be in SRAM.
- Asynchronous Azure MQTT is not supported, this will be addressed in upcoming release(s).
- mDNS with IPV6 is not supported.
- Bus thread stack may need to increase if local variables are used in user callback to avoid stack overflow.
- Low Power examples usage and documentation still under scope of improvement.
- Observed sl\_wifi\_get\_wireless\_info() API is giving wrong security type and PSK for WPA3 Transition supported client mode.
- Observed socket close is not working as expected for TLS socket when socket connect, send data and socket close are performing in a continuous loop.
- WMM-PS/UAPSD is not supported
- Extended header is not visible in sniffer captures while running http otaf example 
- Observed concurrent\_http\_server\_provisioning example readme is not updated as per application provided 

### **Wi-Fi/Network Stack**

**Wi-Fi STA**

- STA Connection with the WPA3 Hunting and Pecking algorithm takes about 3-4 seconds.
- Connection failures have been observed with certain APs in environments with high channel congestion (~50-60% occupancy in an open lab).
- Region selection based on country IE in the beacon is not supported in ICs
- Intermittent beacon reception from Access Point (beacon misses) occurs when channel congestion exceeds 85%.
- When scanning with low power mode enabled, a sensitivity degradation of 3-6dB is observed, which may prevent APs at longer ranges from appearing in the scan results.
- For ICs, the region codes DEFAULT\_REGION and IGNORE\_REGION are not supported
- For modules, the region codes DEFAULT\_REGION and IGNORE\_REGION are not supported in PER mode.
- Observed ~2% increase in listen current and ~1% increase in standby associated current.
- Tx max powers for EVM limited data rates (like MCS7, MCS6, 54M, etc) will be reduced by 0.5dB.

**Access Point (AP) Mode**

- Advance scan feature is not supported in AP mode. 

**WPA2 Enterprise security (STA)**

- Observed connection issue with configuring certificate key and programming 4096 bit key and SHA384/SHA512 certificates.

**Wi-Fi Concurrency (AP + STA in same channel)**

- Observed 3rd party STA association fail with 917 AP while 917 STA mode is connecting/reconnecting to configured 3rd party AP. Reconnect 3rd party STA to 917 AP in such scenarios. 

**OFDMA (UL/DL)**

- Less throughput observed in DL-OFDMA with some APs that enabled Low density parity check coding.

**MU-MIMO (DL)**

- For CoEx Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Performance, Interop issues with MU MIMO with certain APs. 
- Less throughput observed in MU-MIMO with some APs that enabled  Low density parity check coding

**MU-MIMO (UL)**

- UL MU-MIMO is not supported.

**TWT**

- When sl\_wifi\_enable\_target\_wake\_time() API is used, occasional MQTT disconnections may be observed if TWT is configured with longer TWT intervals (>30secs) with embedded MQTT + TWT.  As an alternative, it's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() API, where these dependencies are internally handled.

**Wi-Fi STA Rejoin**
 
- Observed that the post rejoin DUT is unable to publish the data to the cloud with the aws mqtt application 

**BSD Socket API**

- Every server socket created consumes a socket (maximum of 10 sockets supported) and every subsequent connection to server socket consumes an additional socket (from the same pool of 10 sockets), which limits the number of server connections supported.

**SSL Client/Server**

- Sometimes during SSL Handshake, ECC curve parameters generated are wrong, resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
- Secure SSL renegotiation is not supported in the Embedded Networking Stack

**HTTP Client/ HTTPS Client**

- Observed occasional HTTPS continuous download failures when power save is enabled. Recommended to disable it before performing HTTPS continuous downloads
- HTTPS file download is not working with simple\_https\_server/lighttpd server. 

**SNTP**

- Unable to get SNTP async events when CoEx mode and power save are enabled 

**Throughputs & Performance**

- Wi-Fi alone throughput is about SDK 3.x (42Mbps). SDK refinements are in progress to further improve Wi-Fi Standalone and CoEx Throughputs. 

**Wi-Fi IOT Cloud integration**

- **AWS IOT Core**
  - Observed AWS MQTT keepalive transmission is not happening at expected intervals with power save enabled.
  - Observing LAST\_WILL\_MESSAGE is random at every MQTT connection rather than the configured Message/Length

**Wi-Fi Interoperability (IOP)**

- Observed disconnections with Amplifi (AFI-INS-R) AP with powersave enable
- TWT session is failing due to disconnections observed in DUT if rx\_latency is set to 55 seconds and receive data is also set to 55 seconds on MI Xiaomi RA72 and Tplink AX53 AP's
- Observed less throughput(~1Mb) while running TCP RX with Max\_PSP powersave with DLink 810 AP
- Observed interop issue (random disconnections) with few APs (EERO 6+, EERO PRO 6E, Cisco Catalyst 9120AXID)
- Disconnections observed with Netgear RAX120 AP in WPA3 Security

### **BLE**  

**DLE**

- Removed the ble\_data\_length PSRAM example as it does not work with 121x and 141x OPN's. 

**GAP** 

- BLE start advertising API failed with error code 0xffffffe2 while sending BLE direct connectable advertising packet continuously. 
- DUT is not throwing MTU response timeout error after a timeout of 30 seconds. 
- BLE power save API failed with error code -1 (Host Timeout error) while running BLE scan using scan interval(5ms) and scan window(2.5ms) with 352K\_M4SS\_320K RAM configuration. 
- BLE indication API failed with error code 0xFFFFFFD9(ATT command in progress) while enabling and disabling indications continuously. 

**AE** 

- Observed DUT hang issue while running TX notifications in peripheral role.

**Throughput & performance** : 

- BLE throughput in LITE version is reduced compared to Standard Wireless Firmware
- The DUT hangs when the SRRC region is set in the ICs. However, this issue does not occur with the SiWG917Y module.



### **Multi-protocol**

- For CoEx Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Wi-Fi + BLE intermittent connection failures, disconnections, and data transfer stalls in the long run when power save is enabled.
- Observed DUT is not disconnecting to the AP when initiating disconnection from EFR connect app screen using wifi\_station\_ble\_provisioning\_aws example
- Observed DUT failed to load certificate with error "0x10026" (SL\_STATUS\_SI91X\_WRONG\_PARAMETERS) while running wifi\_https\_ble\_dual\_role\_v6 application
- Observed issue in displaying throughput for interval of 'TEST\_TIMEOUT' when CONTINUOUS\_THROUGHPUT enabled in wifi\_station\_ble\_throughput\_app demo
- In CoEx opermode, with memory config of TA - 480K and M4 - 192K Wi-Fi through is degrading by 10Mbps due less memory available for throughput.

### **System**

- Observed random hang issues with encrypted firmwares on some earlier variant of boards  (Si917-6031A, Si917-4338A) with powersave enable.
- This release addresses several issues, resulting in an additional 1K RAM usage in the NWP core. Consequently, this reduces the available heap size by 1K for the NWP core. Users with configurations that were already near the heap limit may experience either minor throughput issues OR functionality issues with this update.

On encountering a problem, it can be mitigated by considering the following options:

- Reduce the number of enabled features in the NWP core.
- Switch to a memory configuration that allocates more RAM to the NWP core while reducing RAM allocated to the Host processor.

### **Simplicity Studio and Commander** 

- Simplicity commander does not support options under "Debug Lock tools".
- All projects in the package are compatible with **GNU ARM V12.2.1** toolchain

## **Limitations and Unsupported Features**  

### **System**

- 16MB Flash is not supported
- 16MB PSRAM is not supported
- Dual-Host mode is not supported
- 1.8v Supply is not supported

### **SDK**

- Baremetal mode is not supported.
- Zephyr is not supported

### **Wi-Fi/Network Stack**

- TLS 1.3 Server is not supported.
- 40 MHz bandwidth for 2.4 GHz band is not supported.
- Max 3 SSL sockets are supported in Wi-Fi alone and CoEx modes. No.of  SSL Sockets in Wi-Fi + BLE based on RAM memory configuration selected. 
- In SSL ECC Curve ID supported is 23. SSL handshake with 3rd party clients depends on the SSL ECC Curve ID.
- The number of Non-Transmitting BSSIDs processed is limited by the beacon length that can be processed by the stack (which is 1024 bytes). Beacons greater than 1024 Bytes in length will not be processed.
- Multiprotocol (STA +BLE) + EAP Security modes supported only with Memory configurations EXT\_FEAT\_416K\_M4SS\_256K and EXT\_FEAT\_480K\_M4SS\_192K.
- UL-MU-MIMO is not supported.
- WPA3 AP supports only H2E algorithm.
- PMKSA caching is not supported in WPA3 AP mode.
- Maximum embedded MQTT Publish payload is 1 kByte.
- Timeout value for socket select and socket receive calls of less than 1 second is not currently supported.
- SA query procedure not supported in 11W AP mode.
- WPA3 AP transition mode is not supported.
- AP standalone mode does not support Tx aggregation. Rx aggregation is supported with limited number of BA sessions.
- In concurrent AP mode, aggregation (Tx/Rx) is not supported.
- Embedded HTTP Server is not supported.
- mDNS with IPV6 is not supported.
- Low power scan supports 1 Mbps packets reception only.
- Auto PAC Provisioning in EAP-FAST with TLSv1.2 is not supported.
- bTWT, Intra PPDU Power save, Spatial Re-Use, BSS coloring features not supported
- HTTPS server is not supported.
- Depending on the crypto engine in use, except HMAC and SHA/SHA3, the input message length limit must not exceed its defined maximum. Refer to the crypto engine's documentation or SDK specifications for exact limits and details. 
- For WPA3 Enterprise connection in station mode, 
  - EAP-192 bit mode is not supported
  - UOSC feature is not supported
  - PEAPV1 is not supported
  - PMK caching is not supported.
  - Explicitly configured server certificate, server domain(FQDN), server domain suffix configuration are not supported for Server Certificate Validation.
- Customers shall not use sl\_si91x\_calibration\_write and sl\_si91x\_evm\_write APIs in PER mode when using module OPNs / Boards / Explorer Kits. This can lead to malfunctioning of the module.

### **BLE**

- For BLE, if the connection is established with a small connection interval (less than 15 ms), simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported.
- BLE maximum two concurrent connections are supported, which can be either a connection to two peripheral devices, to one central and one peripheral device or two central devices.
- BLE Slave latency value is valid up to 32 only.
- Maximum supported AE data length is 200 bytes.
- Supports only two ADV\_EXT sets.
- Supports only two BLE connections (1 Central and 1 Peripheral) with AE.
- Advertising Extension feature is not supported in Coexistence.
- The  ae\_central & ae\_peripheral applications are not supported with TA\_352K\_M4\_320K RAM configuration.
- Two BLE connections are not supported with M4 powersave. It only supports a single connection. 
- Isochronous channels feature is not supported. 
- Connection subrating feature is not supported. 
- LE power controller feature is not supported.
- EATT feature is not supported.
- Periodic Advertising with a response feature is not supported. 
- BLE Audio is not supported.
- The feature of dynamically changing the TX power when extended advertising is active is not supported.
- EFR Connect mobile application doesn't have support to differentiate the BLE configurators based on the Bluetooth Device address.
- The maximum BLE power has been reduced by 2dB compared to the Datasheet Number, which will be addressed in the upcoming 3.3.1 patch release.
- ICs do not support DEFAULT\_REGION and IGNORE\_REGION region codes.
- Modules do not support DEFAULT\_REGION and IGNORE\_REGION region codes in PER mode.

### **MCU**

- FreeRTOS Tickless IDLE mode is not supported in sensor hub application
- LittleFS support is intended to be used only with External flash
- PS1 state is not fully implemented for all supported wakeup sources in the Power Manager
- Manual chip select option is not supported when using GSPI
- Mutli-slave mode is not supported in SSI Primary
- Dual and quad modes are not supported in SSI Primary
- Config timer doesn't support 32-bit timer
- Config timer features to trigger DMA and interrupts on events or counters are not supported
- UART instances does not support different FIFO Thresholds
- In UART Character Timeout feature is not supported
- Lower baud rate bit-width 9 is not supported in UART/USART
- RS485 Interface configuration is not supported
- In SDIO function2 to function5 are not supported
- Multichannel mode and external event based sampling are not supported in ADC
- I2S-PCM is not supported
- MPU is not supported
- Hardware Flow control for ULP UART is not supported
- HSPI Secondary is not supported
- Using a 32kHz external oscillator (connected to UULP GPIOs) will cause timer drift in the Calendar and WDT. This is the default configuration for SoC Modules.
- Sample app and API information (in the API reference guide) for RSI based peripherals is not present
- The DMA controller does not support timeout or threshold interrupts for half-full or full states. For asynchronous transactions, the application must implement handshaking mechanisms to ensure proper synchronization and data transfer control
- CPC (Co-Processor Communication):  
  - Once the kernel Module is installed , Reloading of the module is not supported
  - Once the Daemon connection is successful between the Linux Host and Secondary, exiting and Re-running of daemon is not supported
- PinTool is not enabled for IC and Module OPNs

### **Multi-protocol**

- Wi-Fi AP + BLE currently not supported. 
- EXT\_FEAT\_352K\_M4SS\_320K RAM configuration is not supported for CoEx mode with SSL

## **Removed/Deprecated Features**

- Removed IR, SIO, RO temp sensor and FIM
- Removed support for WDT reset upon processor lock up
- Removed I2C SMBUS feature
- Removed hardware\_setup() from all power save application that uses sllib\_m4\_power\_save.slcc component
- sl\_si91x\_m4\_sleep\_wakeup() will be deprecated from upcoming releases
- switch\_m4\_frequency() will be deprecated from upcoming releases
- RO\_32KHZ\_CLOCK and MCU\_FSM\_CLOCK macros are removed

> **Note:** 
> 
> The following BLE Synchronous API's will be deprecated soon and the equivalent Asynchronous API's will be used instead in all BLE applications :
> 
> |**S.NO**|**BLE Synchronous API's** |**BLE Asynchronous API's** |
> | :- | :- | :- |
> |1|rsi\_ble\_get\_profiles|rsi\_ble\_get\_profiles\_async|
> |2|rsi\_ble\_get\_profile|rsi\_ble\_get\_profile\_async|
> |3|rsi\_ble\_get\_char\_services|rsi\_ble\_get\_char\_services\_async|
> |4|rsi\_ble\_get\_inc\_services|rsi\_ble\_get\_inc\_services\_async|
> |5|rsi\_ble\_get\_char\_value\_by\_uuid|rsi\_ble\_get\_char\_value\_by\_uuid\_async|
> |6|rsi\_ble\_get\_att\_descriptors|rsi\_ble\_get\_att\_descriptors\_async|
> |7|rsi\_ble\_get\_att\_value|rsi\_ble\_get\_att\_value\_async|
> |8|rsi\_ble\_get\_multiple\_att\_values|rsi\_ble\_get\_multiple\_att\_values\_async|
> |9|rsi\_ble\_get\_long\_att\_value|rsi\_ble\_get\_long\_att\_value\_async|
> |10|rsi\_ble\_set\_att\_value|rsi\_ble\_set\_att\_value\_async|
> |11|rsi\_ble\_set\_long\_att\_value|NA|
> |12|rsi\_ble\_prepare\_write|rsi\_ble\_prepare\_write\_async|
> |13|rsi\_ble\_execute\_write|rsi\_ble\_execute\_write\_async|
> |14|rsi\_ble\_indicate\_value\_sync|rsi\_ble\_indicate\_value|

<br>

# **WiSeConnect3\_SDK\_3.3.4 SoC Release Notes**

## **Release Details**

|**Item**|**Details**|
| :- | :- |
|Release date|18th October 2024|
|SDK Version|3\.3.4|
|Firmware Version|<p>Standard: 1711.2.12.3.3.0.3</p><p>Lite Wireless: 1711.2.12.3.3.2.3</p>|
|GSDK/SiSDK Version|SiSDK 2024.6.2 |
|Studio Version|5\.9.3.0|
|Release Package Name|WiSeConnect3\_SDK\_3.3.4|
|Supported RTOS|FreeRTOS|
|Operating Modes Supported|Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP|

- SiWx917 release consists of two components:
  - SiWx91x Connectivity Firmware:
    - Standard Wireless Firmware - SiWx917 Firmware Binary available as SiWG917-B.2.12.3.3.0.3.rps
    - Lite Wireless Firmware - SiWx917 Firmware Binary is available as SiWG917-B.2.12.3.3.2.3.rps, this image is with reduced features for parts with SiWG917M110LGTBA OPN. 
  - Wiseconnect3 Library - Wiseconnect3 SDK library runs on internal Cortex M4

**Note:**

- Mandatory to upgrade the earlier version of boards (Si917-6031A Pro kit or BRD4338A boards) or 917 Silicon ICs with instructions as outlined in this document "SiWG917–TA\_Flash\_Memory\_Map\_ChangeGuide\_v1.3.pdf" for more details.  
- The release packages will have bug-fixes, enhancements, and new features in both 'SDK' and 'Firmware'. Customer shall update and use 'SDK' and 'Firmware' of same release package. SDK and FW combinations that are not released together are not supported.
- It is recommended to update TA image first followed by M4 image and ensure application compatibility with firmware before OTA
- To use the Dev kit Demo, users need to have Simplicity Connect version 2.9.3 or higher.

## **Supported Hardware**

|**Hardware**|**OPN (Ordering Part Number)**|
| :- | :- |
|IC OPN|<p>QFN OPNs: SiWG917M111MGTBA, SiWG917M100MGTBA, SIWG917M110LGTBA(Lite Wireless Firmware), SiWG917M111XGTBA, SiWG917M121XGTBA, SiWG917M141XGTBA</p><p>Module OPNs: SIWG917Y111MGNBA , SIWG917Y110LGNBA, SIWG917Y111XGNBA, SIWG917Y121MGNBA, SIWG917Y111MGABA, SIWG917Y110LGABA, SIWG917Y111XGABA, SIWG917Y121MGABA</p>|
|Development Kits|<p>Pro Kit: SiWx917-PK6031A, Si917-PK6031A. </p><p>(Pro Kit includes Mother board "Si-MB4002A" + Radio board)</p><p>Radio boards: SiWx917-RB4338A, SiWx91x-RB4342A, SiWx917-DK2605A</p><p>Module boards: SiW917Y-RB4343A</p>|

## **Supported Features** 

<table><tr><th><b>Section</b></th><th><b>Sub-Section</b></th><th><b>Feature</b></th><th><p><b>Lite Wireless Firmware</b></p><p><b>(4MB flash OPN<br>OPN No: SiWG917M110LGTBA)</b></p></th><th><b>Standard Wireless Firmware (For other OPNs)</b></th></tr>
<tr><td rowspan="8">System</td><td rowspan="4">Operating modes</td><td>Wi-Fi STA (802.11ax, 802.11n)</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi 802.11n AP</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi STA (802.11ax, 802.11n) + BLE</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Security </td><td>Secure Boot, Secure Key storage and HW device identity with PUF, Secure Zone, Secure XIP (Execution in place) from flash, Secure Attestation, Anti Rollback, Debug Lock, Flash Protection</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Secure firmware upgrade options</td><td><p>- Firmware loading support by Commander Tool through Jlink Debugger. Jlink connected to Serial Wire Debug (SWD) </p><p>- Firmware loading via ISP using UART (Commander or Serial terminal), SPI Interface</p><p>- Secure Over the Air (OTA) Upgrade</p><p>- Firmware update via Bootloader</p></td><td>Supported</td><td>Supported</td></tr>
<tr><td>Crypto Support</td><td><p>- Crypto API's for Hardware Accelerators: Advanced Encryption Standard (AES) 128/256/192, Secure Hash Algorithm (SHA) 256/384/512, Hash Message Authentication Code (HMAC), Random Number Generator (RNG), SHA3, AES-Galois Counter Mode (GCM)/ Cipher based Message Authentication Code (CMAC), ChaCha-poly, True Random Number Generator (TRNG)</p><p>- Software Accelerators: RSA, ECC</p><p>- PSA Crypto APIs support for all crypto operations.</p><p>- Wrapping Secret keys (Symmetric crypto). </p><p>- Added ECDSA Sign and Verify APIs</p></td><td>Supported</td><td>Supported</td></tr>
<tr><td>System Power Save</td><td><p>- Deep Sleep with RAM retention and without RAM retention. </p><p>- Wireless Power Save: Connected Sleep (Wi-Fi Standby Associated), BLE Advertising with powersave, BLE Scan with powersave,  BLE connection with powersave. Only Max PSP power save mode is supported in BLE. </p></td><td>Supported</td><td>Supported</td></tr>
<tr><td rowspan="13">Wi-Fi</td><td>Wi-Fi Protocols</td><td>IEEE 802.11 b/g/n/ax (2.4GHz)</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Access Point (AP) Mode</td><td><p>- 4 Client Support, Hidden SSID Mode, Auto Channel Selection, Scan in AP mode (Alpha)</p><p>- Wi-Fi Security </p><p>- WPA2 Personal, WPA3 Personal (H2E method only) (Alpha), WPA Mixed mode (WPA/WPA2) </p></td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi Scan</td><td>Selective Scan, Active/Passive Scan</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi STA (Security Modes)</td><td>Open Mode, WPA2 Personal, WPA2 Enhancements, WPA3 Personal, Mixed Mode (WPA/WPA2), WPA3 Personal Transition Mode (WPA2/WPA3)</td><td>Supported</td><td>Supported</td></tr>
<tr><td>WPA2 Enterprise security (STA)</td><td>Method: PEAP/TTLS/TLS 1.0/TLS 1.2/FAST/LEAP</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi STA Rejoin</td><td></td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi STA Roaming </td><td>BG Scan, OKC (Opportunistic Key caching), PMK (Pairwise Master Key) caching, Pre-Authentication</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi Protocol Power Save </td><td>Deep sleep (unconnected state), Max PSP, Enhanced Max PSP, Fast PSP, TWT</td><td>Supported</td><td>Supported</td></tr>
<tr><td>QoS</td><td>WMM-QoS</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi 6 Feature</td><td>MU-MIMO (DL), OFDMA (UL/DL), iTWT, TWT I-Frame & TWT Enhancements (Automatic TWT Configuration), BSS coloring, MBSSID</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi Concurrency </td><td>AP+STA (Same channel)</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi Band/Channels</td><td>2\.4GHz CH1-11, 2.4GHz CH1-13, 2.4GHz CH1-14</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Known Security Vulnerabilities Handled</td><td>WPA2 KRACK Attacks, Fragment and Forge Vulnerability</td><td>Supported</td><td>Supported</td></tr>
<tr><td rowspan="6">Network stack</td><td rowspan="2">Core Networking Features</td><td><p>- IPv4/IPv6/UDP/TCP/ARP/ICMP/ICMPv6</p><p>- SSL client versions TLSV1.0, TLSV1.2, TLSV1.3 </p><p>- SSL server versions TLSV1.0 and TLSV1.2</p><p>- DHCPv4/DHCPv6 Client</p><p>- TCP/IP Bypass (LWIP as Hosted stack for reference)</p></td><td>Supported</td><td>Supported</td></tr>
<tr><td>- DHCPv4 Server, DHCPv6 Server</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td rowspan="2">Advanced Network Features</td><td>HTTP Client/HTTPS Client/DNS Client, Embedded MQTT/MQTT on host (AWS and AZURE) </td><td>Supported</td><td>Supported</td></tr>
<tr><td>SNTP Client, IGMP</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi IoT Cloud Integration</td><td><p>- AWS IOT Core</p><p>- Azure IoT</p></td><td>Supported</td><td>Supported</td></tr>
<tr><td>BSD and IoT sockets application programming interface(API)</td><td></td><td>Supported</td><td>Supported</td></tr>
<tr><td rowspan="2">BLE </td><td>Legacy features</td><td><p>- GAP(Advertising, Scanning, initiation, Connection and Bonding)</p><p>- Generic Attribute Protocol(GATT)</p><p>- Attribute protocol(ATT)</p><p>- Security</p><p>- LL Privacy 1.2</p><p>- Accept list</p><p>- Directed Advertising</p><p>- LE PHY(1Mbps, 2Mbps) & Coded PHY(125kbps, 500kbps)</p><p>- Simultaneous scanning on 1Mbps and Coded PHY</p><p>- LE dual role topology</p><p>- LE data packet length extensions(DLE)</p><p>- Asymmetric PHYs</p><p>- LE channel selection algorithm 2 (CSA#2)</p><p>- LE Secure connections</p></td><td>Supported</td><td>Supported</td></tr>
<tr><td>Advertising Extensions </td><td><p>- Extended Advertising</p><p>- Periodic Advertising</p><p>- Periodic Advertising scanning</p><p>- Extended Advertising scanning</p><p>- Periodic Advertising list</p><p>- LE periodic advertising synchronization</p></td><td>Not Supported </td><td>Supported</td></tr>
</table>

### **MCU**

- **Memory**
  - Common Flash: Single shared Flash for both Cortex-M4 and NWP (Wireless Processor)
  - Common Flash + External PSRAM
  - Stacked PSRAM + External Common Flash
- **Power States**
  - Active: PS4, PS3, PS2, and PS1 
  - Standby: PS4, PS3, and PS2
  - Sleep: PS4, PS3 and PS2
  - Deep Sleep (Shutdown): PS0
- **Peripherals, Services and Hardware Drivers** 

|**HP Peripherals**|**List**|**Notes**|
| :- | :- | :- |
| |<p>- ADC</p><p>- Analog Comparator</p><p>- Config Timer (CT)</p><p>- CRC</p><p>- DAC</p><p>- eFuse</p><p>- EGPIO</p><p>- GPDMA<sup>1</sup></p><p>- GSPI</p><p>- I2C</p><p>- I2S</p><p>- MCPWM</p><p>- PSRAM</p><p>- RNG<sup>1</sup></p><p>- SDIO Secondary</p><p>- SSI (Primary & Secondary)</p><p>- Temperature Sensor</p><p>- UART</p><p>- uDMA</p><p>- USART</p>||
| |<p>- BoD<sup>1</sup></p><p>- CTS (Touch Sensor)<sup>1</sup></p><p>- OPAMP<sup>1</sup></p><p>- QSPI<sup>1</sup></p><p>- QEI<sup>1</sup></p>|Limited Support|
|**ULP Peripherals**|||
| |<p>- ULP\_ADC</p><p>- ULP\_DAC</p><p>- ULP\_GPIO</p><p>- ULP\_I2C</p><p>- ULP\_I2S</p><p>- ULP\_TIMER</p><p>- ULP\_UDMA</p><p>- ULP\_UART</p><p>- ULP\_SSI\_PRIMARY</p>||
|**UULP Peripherals**|||
| |<p>- RTC (Calendar)</p><p>- SYSRTC</p><p>- WDT</p>||
|**Services**| ||
| |Sleep Timer||
| |IOSTREAM||
| |NVM3||
| |LittleFS (for Dual Flash)|Limited Support|
| |Power Manager||
| |Sensor Hub|Limited Support|
| |Pin Tool|Limited Support|
|**Hardware Drivers**| ||
| |LED, Button, MEMLCD, Joystick, Sensors (RHT, VEML, ICM)||

|The peripherals marked with superscript<sup>1</sup> are available through RSI APIs.  Support for SL APIs for user facing peripherals will be available in future releases.|
| :- |

|The flash write feature has been enhanced to support the NWP area, providing a 20k allocation within the NWP flash memory for storing user data. Moreover, a Read API has been introduced to retrieve data from the NWP flash region.|
| :- |

### **Developer Environment**

- Simplicity Studio IDE (SV5.9.3.0 version) and Debugger Integration. Refer to the latest version of the SoC "Getting-Started-with-SiWx917" guide for more details. 
- Recommended to install and use Silicon labs Simplicity SDK (Previously known as Gecko SDK), Git hub based version 2024.6.2.
- Simplicity Commander to supports Flash loading, provision of MBR programming, security key management, and calibration support for crystal and gain offsets. refer "siwx917-soc-manufacturing-utility-user-guide" for more details. 
- Advanced Energy Monitoring (AEM) to measure ultra-low power capability on Development boards (Pro Kit).
- PinTool for MCU pin configurations

### **BLE** 

- GAP(Advertising, Scanning, initiation, Connection and Bonding)
- Generic Attribute Protocol(GATT)
- Attribute protocol(ATT)
- Security
- LL Privacy 1.2
- Accept list
- Directed Advertising
- Extended Advertising
- Periodic Advertising
- Periodic Advertising scanning
- Extended Advertising scanning
- Periodic Advertising list
- LE periodic advertising synchronization
- LE PHY(1Mbps, 2Mbps) & Coded PHY(125kbps, 500kbps)
- Simultaneous scanning on 1Mbps and Coded PHY
- LE dual role topology
- LE data packet length extensions(DLE)
- Asymmetric PHYs
- LE channel selection algorithm 2 (CSA#2)
- LE Secure connections
- Bluetooth 5.4 Qualified

### **SDK**

- Simplified and Unified DX for Wi-Fi API and Platform APIs
- Simplifies application development and presents clean and standardized APIs
- UC (Universal Configurator) enables componentization, simplifying configuration of peripherals and examples
- BSD and ARM IoT-compliant socket API
- Available through Simplicity Studio and GitHub

### **Multi-protocol**

- Wi-Fi STA + BLE

### **PTA/Coexistence**

- 3 wire CoEx acting as Wi-Fi with external Bluetooth
- 3 wire CoEx acting as Wi-Fi with external Zigbee/OT

## **Changes in this release compared to v3.3.3 Release**

### **System**

- **Enhancements / New features**
  - None
- **Fixed Issues**
  - None
- **Documentation**
  - None

### **MCU**

- **Enhancements / New features**
  - None
- **Fixed Issues**
  - Resolved the issue with Copy contents when creating projects in Simplicity Studio
- **Documentation**
  - None

### **SDK**

- **Enhancements / New features**
  - None
- **Fixed Issues**
  - None
- **Documentation**
  - None

### **Wi-Fi/Network Stack**

- **Enhancements / New features**
  - None
- **Fixed Issues**
  - None
- **Documentation**
  - None

### **BLE**

- **Enhancements / New features**
  - None 
- **Fixed Issues**
  - None
- **Documentation**
  - None

### **Multi-protocol**

- **Enhancements / New features**
  - None
- **Fixed Issues**
  - None
- **Documentation**
  - None

## **Recommendations**

### **System**

- The current revision of SiWx917 has:
  - RAM memory of 672k bytes which can be shared between TA and M4 processors in SoC mode. 
  - The below configurations are applicable in SoC mode and can be configured based on the application requirement. EXT\_FEAT\_352K\_M4SS\_320K is the default configuration, based on requirement EXT\_FEAT\_480K\_M4SS\_192K configuration is selected for SoC mode multi-protocol examples.
    - EXT\_FEAT\_480K\_M4SS\_192K - This mode configures TA with 480k and M4 with 192K bytes of memory
    - EXT\_FEAT\_416K\_M4SS\_256K - This mode configures TA with 416k and M4 with 256K bytes of memory
    - EXT\_FEAT\_352K\_M4SS\_320K - This mode configures TA with 352k and M4 with 320K bytes of memory
  - SoC mode should not use 672k\_M4SS\_0K memory configuration.
- Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
- There are 2 Versions of Pro-Kits/Radio boards. Si917-6031A based on Si917-4338A (Rev **A01 - A11**) and SiWx917-6031A based on SiWx917-4338A (Rev A12-A14). To get optimal power numbers, enable macro "SL\_SI91X\_ENABLE\_LOWPWR\_RET\_LDO" pre-processor define for ICs or while using SiWx917-6031A Pro-kit, SiWx917-4338A version of boards. This macro should be disabled for earlier variants of the board (Si917-6031A, Si917-4338A).
- With RAM configuration (EXT\_FEAT\_352K\_M4SS\_320K), only 352K memory is available to TA  which limits the features supported, Recommended to enable EXT\_FEAT\_416K\_M4SS\_256K in Wi-Fi + BLE Multi protocol mode to enable more Network features.
- For EXT\_FEAT\_416K\_M4SS\_256K  and EXT\_FEAT\_480K\_M4SS\_192K memory configurations, it is recommended to retain both TA and M4 RAMs in power save.

### **Wi-Fi/Network Stack**

- It is recommended to enable SL\_SI91X\_EXT\_TCP\_IP\_WAIT\_FOR\_SOCKET\_CLOSE BIT(16) of the 'Extended TCP IP Feature' bit map in the opermode command for all Wi-Fi Socket operations from the host to ensure graceful handling during asynchronous closures from the peer.
- For high throughputs,  it is recommended to enable BIT(2) - SL\_SI91X\_FEAT\_AGGREGATION  of feature\_bit\_map in opermode. 
- Users can enable SL\_SI91X\_EXT\_TCP\_IP\_SSL\_16K\_RECORD in 'Extended TCP IP Feature' bit map in opermode for (HTTPS server) supporting 16k record.
- **TWT**
  - Recommendation is to use sl\_wifi\_target\_wake\_time\_auto\_selection() API for all TWT applications. 
  - It is recommended to issue iTWT setup command once IP assignment, TCP connection, application specific socket connections are done.
  - When using sl\_wifi\_enable\_target\_wake\_time API, increase TCP / ARP Timeouts at the remote side depending upon the configured TWT interval configured. It's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() as an alternative.
  - In case of TWT in CoEx mode, when using sl\_wifi\_enable\_target\_wake\_time API, use TWT wake duration <= 16 ms and TWT wake interval >= 1 sec. If wake duration > 16 ms or TWT wake interval < 1sec, there might be performance issues.
  - For iTWT GTK interval in AP should be configured to max possible value or zero. If GTK interval is not configurable on AP side, recommended TWT interval (in case of sl\_wifi\_enable\_target\_wake\_time API) or RX Latency (in case of sl\_wifi\_target\_wake\_time\_auto\_selection API) is less than 4sec.
  - When sl\_wifi\_enable\_target\_wake\_time API is used, configuring TWT Wake interval beyond 1 min might lead to disconnections from the AP. Recommended to use TWT wake interval of less than or equal to 1 min.
  - When using sl\_wifi\_enable\_target\_wake\_time API, it is recommended to set missed\_beacon\_count of sl\_wifi\_set\_advanced\_client\_configuration API greater than 2 times of the configured TWT Interval.
  - DUT keepalive should be configured aligned with AP keepalive in TWT modes.
- Disable power save for high throughput applications or use FAST PSP power save mode as per application requirement.
- The application needs to ensure that it sets RTC with the correct timestamp before establishing the SSL/EAP connection.
- The minimum timeout value should not be less than 1 second for socket select and socket receive calls. 
- Please refer Keep alive intervals supported by MQTT broker and configure keep alive interval values accordingly.
- The minimum keep alive interval value recommended for embedded MQTT is 10 Seconds. 
- Disable power save and suspend any active TWT sessions before triggering HTTP OTAF.
- Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with the power save.
- Recommended to configure VAP\_ID properly for Si91x STA and AP using sl\_si91x\_setsockopt\_async(), in case of data transfer.
- Recommended to use valid length(<= 202 bytes) for topic to be published while using Embedded MQTT, else it leads to return wrong error code(0x21).
- In concurrent mode with dual IP, it is advised to bring up STA first (IP configuration) and AP later.
- It is recommended to configure Tx ,Rx , Global buffer pool ratio in the buffer config command based for all Wi-Fi Socket operations from the host
- It is recommended to use "TCP exponential backoff" configuration for congested channels.
- It is recommended is to disable broadcast filter during TCP connection to avoid ARP resolution issues
- To avoid IOP issues, it is recommended to disable power save before Wi-Fi connection.
- It is recommended to set region\_code as `IGNORE_REGION` in boot configurations for SIWG917Y module boards except for PER mode.

### **BLE**

- In BLE, the recommended range of Connection Interval in
  - Power Save (BLE Only) - 100 ms to 1.28 s.
- In BLE, during Connection, the configuration of Scan Interval and Scan Window with the same value is not recommended. The suggested ratio of Scan Window to Scan Interval is 3:4.
- In BLE, if a device is acting as Central, the scan window (in set\_scan\_params and create\_connection commands) must be less than the existing Connection Interval. The suggested ratio of Scan Window to Connection Interval is 2:3.
- In BLE mode, if scanning and advertising are in progress on the SiWx91x module and it subsequently gets connected and moves to the central role, scanning stops else if it moves to the peripheral role, advertising stops. To further establish a connection to another peripheral device or to a central device, the application should give a command for starting advertising and scanning again.

### **Multi-protocol**

- For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
- Wi-Fi + BLE Advertising
  - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals may be observed.
  - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity.
- Wi-Fi + BLE scanning
  - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
  - BLE scanning will be stopped for intervals that collide with Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Connections
  - All standard connection intervals are supported.
  - For a stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Data Transfer
  - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout.
  - Ensure Wi-Fi activity consumes lower intervals.

### **MCU**

- The WDT manager is specifically meant for system reset recovery and should not be utilized for any other purpose. When interrupts are disabled, make sure to stop the WDT to avoid unintended resets. Once interrupts are re-enabled, restart the WDT to ensure system reliability
- It is strongly recommended to use `sl_si91x_soc_nvic_reset()` API for system soft reset rather than the `sl_si91x_soc_soft_reset()` function, since this uses the WDT for soft reset, which is specifically intended for system reset recovery
- It is strongly recommended not use switch\_m4\_frequency() for clock scaling. Refer to the migration guide for more details
- For GPIO-based wakeup, ensure the GPIO component is installed in powersave applications
- Use both CTS and RTS for UART flow control
- PSRAM examples are not supposed to be used with Non-PSRAM OPNs
- SYSRTC and wake on wireless wakeup resources are enabled by default when tickeless idle mode is enabled. Avoid installing sleeptimer component in power save applications (where FreeRTOS tickless is enabled by default)
- Minimum idle-time to sleep is configured to 100ms in FreeRTOS\_config.h
- Enable DMA to achieve better throughput numbers
- For I2C Fastplus and High speed modes, use high power instances with core clock frequencies between 80MHz and 180MHz
- Use blocking calls instead of non-blocking calls for I2C leader in ULP\_I2C while receiving data
- In RTOS environment, prefer Signaling mechanisms (Semaphore/Mutex/EventFlag etc.) instead of "Variables/Flags" from user callbacks to detect "\*Transfer Complete\*" for high speed communication peripherals
  Refer Software Reference Manual for more details
- Install the 'device\_needs\_ram\_execution' component for OPN-based firmware update examples
- It is strongly recommended to use the FreeRTOS tickless based powersave instead of the sl\_si91x\_m4\_sleep\_wakeup() (legacy) API
- For ACx Modules, it is necessary to initialize the internal 32kHz RC clock specifically for the LF\_FSM domain. GPIO oscillator clock does not propagate to the LF\_FSM. This may lead to timer accuracy issues when using the 32kHz RC clock.
- If the external oscillator connected to UULP\_GPIO\_3 is used as the 32kHz clock source, make the necessary code changes as detailed in the Software Reference Manual
- It is strongly recommended not to use RO clock in MCU
- For BLE, CoEx, and high-accuracy MCU applications it is recommended to use external 32kHz crystal on XTAL\_32KHZ\_P and XTAL\_32KHZ\_N pins.

## **Known Issues of WiSeConnect3\_SDK\_3.3.4 Release**

### **MCU**

**GSPI**

- First 2 MSB bits of the first byte on MISO are garbled

**GPIO**

- By default, sl\_gpio\_set\_configuration() sets the GPIO to HIGH 

**SSI**

- In ULP SSI, non-DMA transfer is not working

**I2C**

- ULP\_I2C non-blocking receive in low power mode will not function as expected

**I2S**

- FIFO threshold configurations are not working

**Config Timers**

- PWM mode is not working

**ADC**

- ADC static mode sampling rate is supported up to 2.5Msps

**Analog comparator**

- opamp2, opamp3 & BOD configurable Inputs do not work

**Temperature Sensors**

- BJT Temperature readings are not accurate

**QSPI**

- QUAD mode not working

**Power Manager**

- BOD and Comparator wakeup sources not working
- PS2 -PS4 transition is not working in SiWG917Y110LGNBA part

**SensorHub**

- Power transitions with AWS are not stable in SensorHub
- SPI sensor is not working
- Button interrupt is not working

### **SDK**

- Observed Wi-Fi connection is successful even after deleting the stored network credentials using sl\_net\_delete\_credential and responding with SL\_NET\_INVALID\_CREDENTIAL\_TYPE for sl\_net\_get\_credential.
- Enhanced sl\_wifi\_get\_firmware\_version() API to provide more details (ROM ID, chip ID, security version, etc) which is not backward compatible with firmware older than 1711.2.10.1.0.0.4. Firmware binary notation does not include the security version number.
- In PSRAM enabled demos, moving of text, data and stack segments to PSRAM is allowed. BSS and Heap should still be in SRAM.
- Asynchronous Azure MQTT is not supported, this will be addressed in upcoming release(s).
- mDNS with IPV6 is not supported.
- Bus thread stack may need to increase if local variables are used in user callback to avoid stack overflow.
- Low Power examples usage and documentation still under scope of improvement.
- Observed sl\_wifi\_get\_wireless\_info() API is giving wrong security type and PSK for WPA3 Transition supported client mode.
- Observed socket close is not working as expected for TLS socket when socket connect, send data and socket close are performing in a continuous loop.
- Observed data not received simultaneously when two sockets call recv() from two different RTOS tasks.
- WMM-PS/UAPSD is not supported
- firmware\_flashing\_from\_host\_uart\_xmodem example fails to communicate over UART.

### **Wi-Fi/Network Stack**

**Wi-Fi STA**

- STA Connection with the WPA3 Hunting and Pecking algorithm takes about 3-4 seconds.
- Connection failures have been observed with certain APs in environments with high channel congestion (~50-60% occupancy in an open lab).
- Region selection based on country IE in the beacon is not supported in ICs
- Intermittent beacon reception from Access Point (beacon misses) occurs when channel congestion exceeds 85%.
- When scanning with low power mode enabled, a sensitivity degradation of 3-6dB is observed, which may prevent APs at longer ranges from appearing in the scan results.
- Passive scan is failing when DUT is configured in world domain.
- For ICs, the region codes DEFAULT\_REGION and IGNORE\_REGION are not supported
- For modules, the region codes DEFAULT\_REGION and IGNORE\_REGION are not supported in PER mode.
- Observed ~2% increase in listen current and ~1% increase in standby associated current.
- Tx max powers for EVM limited data rates (like MCS7, MCS6, 54M, etc) will be reduced by 0.5dB.

**Access Point (AP) Mode**

- Fixed rate configuration in AP mode using sl\_wifi\_set\_transmit\_rate API is not being set as expected. 

**WPA2 Enterprise security (STA)**

- Observed connection issue with configuring certificate key and programming 4096 bit key and SHA384/SHA512 certificates.

**Wi-Fi Concurrency (AP + STA in same channel)**

- Observed 3rd party STA association fail with 917 AP while 917 STA mode is connecting/reconnecting to configured 3rd party AP. Reconnect 3rd party STA to 917 AP in such scenarios. 
- In concurrent mode, 917 AP cannot process de-authentication frames sent by third-party STA if 917 STA is connected to WPA2+WPA3 enabled AP.

**OFDMA (UL/DL)**

- Less throughput observed in DL-OFDMA with some APs that enabled Low density parity check coding.

**MU-MIMO (DL)**

- For CoEx Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Performance, Interop issues with MU MIMO with certain APs. 
- Less throughput observed in MU-MIMO with some APs that enabled  Low density parity check coding

**MU-MIMO (UL)**

- UL MU-MIMO is not supported.

**TWT**

- When sl\_wifi\_enable\_target\_wake\_time() API is used, occasional MQTT disconnections may be observed if TWT is configured with longer TWT intervals (>30secs) with embedded MQTT + TWT.  As an alternative, it's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() API, where these dependencies are internally handled.

**Wi-Fi STA Rejoin**

- Observed Scanning (probe request) in all channels instead of the channels configured in selective channel(channel\_bitmap\_2g4) during rejoin process. 

**IPv4/IPv6**

- IP change notification is not indicated to the application. 
- In concurrent mode with dual IP, if the STA starts after AP is up, the STA IP configuration may fail for DHCP stateless mode.
- In concurrent mode, data transfer using the Link-local address will always use the first IP interface created by the application.

**BSD Socket API**

- Every server socket created consumes a socket (maximum of 10 sockets supported) and every subsequent connection to server socket consumes an additional socket (from the same pool of 10 sockets), which limits the number of server connections supported.
- Observing issues with TCP retries when power save mode is enabled, especially when the module is in idle state.
- TCP maximum retry value is valid upto 31

**SSL Client/Server**

- Sometimes during SSL Handshake, ECC curve parameters generated are wrong, resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
- Secure SSL renegotiation is not supported in the Embedded Networking Stack

**HTTP Client/ HTTPS Client**

- Observed occasional HTTPS continuous download failures when power save is enabled. Recommended to disable it before performing HTTPS continuous downloads

**SNTP**

- Unable to get SNTP async events when CoEx mode and power save are enabled 

**Throughputs & Performance**

- Wi-Fi alone throughput is about SDK 3.x (42Mbps). SDK refinements are in progress to further improve Wi-Fi Standalone and CoEx Throughputs. 

**Wi-Fi IOT Cloud integration**

- **AWS IOT Core**
  - Observed AWS MQTT keepalive transmission is not happening at expected intervals with power save enabled.
  - Observing LAST\_WILL\_MESSAGE is random at every MQTT connection rather than the configured Message/Length
- **AZURE IOT Core**
  - Observed DUT after sending the data, its not sending the MQTT keep alive packet due to this Azure HUB closing the connection when power save is enabled.

**Wi-Fi Interoperability (IOP)**

- Observed disconnections with Amplifi (AFI-INS-R) AP with powersave enable
- TWT session is failing due to disconnections observed in DUT if rx\_latency is set to 55 seconds and receive data is also set to 55 seconds on MI Xiaomi RA72 and Tplink AX53 AP's
- Observed less throughput(~1Mb) while running TCP RX with Max\_PSP powersave with DLink 810 AP
- Observed interop issue (random disconnections) with few APs (EERO 6+, EERO PRO 6E, Cisco Catalyst 9120AXID)
- Disconnections observed with Netgear RAX120 AP in WPA3 Security

### **BLE**  

**DTM/PER**

- Recommend to limit BLE Tx Maximum power to 18 dBm.  Please don't use for 127 power\_index for BLE HP chain with this release.

**DLE**

- Removed the ble\_data\_length PSRAM example as it does not work with 121x and 141x OPN's. 

**Privacy** 

- DUT hang at the rsi\_bt\_get\_local\_device\_address API in central role, when using PSRAM boards (121x, 141x, 111M) and enabling power save in the ble\_privacy application,

AE 

- Observed DUT hang issue while running TX notifications in peripheral role.

**SMP**  

- SMP is not working with 110L(Lite wireless firmware image) board. 

**Throughput & performance** 

- BLE throughput in LITE version is reduced compared to Standard Wireless Firmware
- The DUT hangs when the SRRC region is set in the ICs. However, this issue does not occur with the SiWG917Y module. 



### **Multi-protocol**

- For CoEx Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Wi-Fi + BLE intermittent connection failures, disconnections, and data transfer stalls in the long run when power save is enabled.
- Observed "DUT is not disconnecting to the AP when initiating disconnection from EFR connect app screen using wifi\_station\_ble\_provisioning\_aws example
- Observed DUT failed to load certificate with error "0x10026" (SL\_STATUS\_SI91X\_WRONG\_PARAMETERS) while running wifi\_https\_ble\_dual\_role\_v6 application
- Observed BLE bonding failure during continuous HTTPS download.
- Observed BLE and WLAN connection failure with SMP, when WLAN connect and HTTPS GET called in a loop.
- Observed issue in displaying throughput for interval of 'TEST\_TIMEOUT' when CONTINUOUS\_THROUGHPUT enabled in wifi\_station\_ble\_throughput\_app demo
- In CoEx opermode, with memory config of TA - 480K and M4 - 192K Wi-Fi through is degrading by 10Mbps due less memory available for throughput.

### **System**

- Observed random hang issues with encrypted firmwares on some earlier variant of boards  (Si917-6031A, Si917-4338A) with powersave enable.
- This release addresses several issues, resulting in an additional 1K RAM usage in the NWP core. Consequently, this reduces the available heap size by 1K for the NWP core. Users with configurations that were already near the heap limit may experience either minor throughput issues OR functionality issues with this update.

On encountering a problem, it can be mitigated by considering the following options:

- Reduce the number of enabled features in the NWP core.
- Switch to a memory configuration that allocates more RAM to the NWP core while reducing RAM allocated to the Host processor.

### **Simplicity Studio and Commander** 

- Simplicity commander does not support options under "Debug Lock tools".
- All projects in the package are compatible with **GNU ARM V12.2.1** toolchain

## **Limitations and Unsupported Features**  

### **System**

- 16MB Flash is not supported
- 16MB PSRAM is not supported
- Dual-Host mode is not supported

### **SDK**

- Baremetal mode is not supported.
- Zephyr is not supported

### **Wi-Fi/Network Stack**

- TLS 1.3 Server is not supported.
- 40 MHz bandwidth for 2.4 GHz band is not supported.
- Max 3 SSL sockets are supported in Wi-Fi alone and CoEx modes. No.of  SSL Sockets in Wi-Fi + BLE based on RAM memory configuration selected. 
- In SSL ECC Curve ID supported is 23. SSL handshake with 3rd party clients depends on the SSL ECC Curve ID.
- The number of Non-Transmitting BSSIDs processed is limited by the beacon length that can be processed by the stack (which is 1024 bytes). Beacons greater than 1024 Bytes in length will not be processed.
- Multiprotocol (STA +BLE) + EAP Security modes supported only with Memory configurations EXT\_FEAT\_416K\_M4SS\_256K and EXT\_FEAT\_480K\_M4SS\_192K.
- UL-MU-MIMO is not supported.
- WPA3 AP supports only H2E algorithm.
- PMKSA caching is not supported in WPA3 AP mode.
- Maximum embedded MQTT Publish payload is 1 kByte.
- Timeout value for socket select and socket receive calls of less than 1 second is not currently supported.
- SA query procedure not supported in 11W AP mode.
- WPA3 AP transition mode is not supported.
- AP standalone mode does not support Tx aggregation. Rx aggregation is supported with limited number of BA sessions.
- In concurrent AP mode, aggregation (Tx/Rx) is not supported.
- Embedded HTTP Server is not supported.
- mDNS with IPV6 is not supported.
- Low power scan supports 1 Mbps packets reception only.
- Auto PAC Provisioning in EAP-FAST with TLSv1.2 is not supported.
- bTWT , Intra PPDU Power save, Spatial Re-Use, BSS coloring features not supported
- HTTPS server is not supported.

### **BLE**

- For BLE, if the connection is established with a small connection interval (less than 15 ms), simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported.
- BLE maximum two concurrent connections are supported, which can be either a connection to two peripheral devices, to one central and one peripheral device or two central devices.
- BLE Slave latency value is valid up to 32 only.
- Maximum supported AE data length is 200 bytes.
- Supports only two ADV\_EXT sets.
- Supports only two BLE connections (1 Central and 1 Peripheral) with AE.
- Advertising Extension feature is not supported in Coexistence.
- The  ae\_central & ae\_peripheral applications are not supported with TA\_352K\_M4\_320K RAM configuration.
- Two BLE connections are not supported with M4 powersave. It only supports a single connection. 
- Isochronous channels feature is not supported. 
- Connection subrating feature is not supported. 
- LE power controller feature is not supported.
- EATT feature is not supported.
- Periodic Advertising with a response feature is not supported. 
- BLE Audio is not supported.
- The feature of dynamically changing the TX power when extended advertising is active is not supported.
- EFR Connect mobile application doesn't have support to differentiate the BLE configurators based on the Bluetooth Device address.
- The maximum BLE power has been reduced by 2dB compared to the Datasheet Number, which will be addressed in the upcoming 3.3.1 patch release.
- ICs do not support DEFAULT\_REGION and IGNORE\_REGION region codes.
- Modules do not support DEFAULT\_REGION and IGNORE\_REGION region codes in PER mode.

### **MCU**

- SensorHub supports PS1-power state with ADC sensor (FIFO mode not supported) . In this mode, the other sensor's operation is not supported.
- FreeRTOS Tickless IDLE mode is not supported in sensor hub application
- LittleFS support is intended to be used only with Dual flash
- PS1 state is not supported in Power Manager
- Power consumption is same for PS4 and PS3 states with powersave mode of power manager
- When using Button\_0 as wakeup source, it limits the button functionality at application layer. For using Button\_0 for application specific functionality, enable 'SL\_SI91X\_NPSS\_GPIO\_BTN\_HANDLER'
- Manual chip select option is not supported when using GSPI
- Mutli-slave mode is not supported in SSI Primary
- Dual and quad modes are not supported in SSI Primary
- Config timer doesn't support 32-bit timer
- Config timer features to trigger DMA and interrupts on events or counters are not supported
- UART instances does not support different FIFO Thresholds
- In UART Character Timeout feature is not supported
- Lower baud rates 110, 150, bit-width 1-4 and 9 are not supported in UART/USART
- RS485 Interface configuration is not supported
- In SDIO function2 to function5 are not supported
- Multichannel and external event based sampling are not supported in ADC
- Fast plus and High Speed modes are not supported in ULP\_I2C instance
- I2S-PCM is not supported
- MPU is not supported
- CPC is not supported
- Hardware Flow control for ULP UART is not supported
- HSPI Secondary is not supported
- Using a 32kHz external oscillator (connected to UULP GPIOs) may cause timer drift in the UULP peripherals
- Sample app and API information (in the API reference guide) for RSI based peripherals is not present

### **Multi-protocol**

- Wi-Fi AP + BLE currently not supported. 
- EXT\_FEAT\_352K\_M4SS\_320K RAM configuration is not supported for CoEx mode with SSL

## **Removed/Deprecated Features**

- Removed IR, SIO, RO temp sensor and FIM
- Removed support for WDT reset upon processor lock up
- Removed I2C SMBUS feature
- Removed hardware\_setup() from all power save application that uses sllib\_m4\_power\_save.slcc component
- sl\_si91x\_m4\_sleep\_wakeup() will be deprecated from upcoming releases
- switch\_m4\_frequency() will be deprecated from upcoming releases
- RO\_32KHZ\_CLOCK and MCU\_FSM\_\_CLOCK macros are removed

> **Note:** 
> 
> The following BLE Synchronous API's will be deprecated soon and the equivalent Asynchronous API's will be used instead in all BLE applications :
> 
> |**S.NO**|**BLE Synchronous API's** |**BLE Asynchronous API's** |
> | :- | :- | :- |
> |1|rsi\_ble\_get\_profiles|rsi\_ble\_get\_profiles\_async|
> |2|rsi\_ble\_get\_profile|rsi\_ble\_get\_profile\_async|
> |3|rsi\_ble\_get\_char\_services|rsi\_ble\_get\_char\_services\_async|
> |4|rsi\_ble\_get\_inc\_services|rsi\_ble\_get\_inc\_services\_async|
> |5|rsi\_ble\_get\_char\_value\_by\_uuid|rsi\_ble\_get\_char\_value\_by\_uuid\_async|
> |6|rsi\_ble\_get\_att\_descriptors|rsi\_ble\_get\_att\_descriptors\_async|
> |7|rsi\_ble\_get\_att\_value|rsi\_ble\_get\_att\_value\_async|
> |8|rsi\_ble\_get\_multiple\_att\_values|rsi\_ble\_get\_multiple\_att\_values\_async|
> |9|rsi\_ble\_get\_long\_att\_value|rsi\_ble\_get\_long\_att\_value\_async|
> |10|rsi\_ble\_set\_att\_value|rsi\_ble\_set\_att\_value\_async|
> |11|rsi\_ble\_set\_long\_att\_value|NA|
> |12|rsi\_ble\_prepare\_write|rsi\_ble\_prepare\_write\_async|
> |13|rsi\_ble\_execute\_write|rsi\_ble\_execute\_write\_async|
> |14|rsi\_ble\_indicate\_value\_sync|rsi\_ble\_indicate\_value|

<br>

# **WiSeConnect3\_SDK\_3.3.3 SoC Release Notes**

## **Release Details**

|**Item**|**Details**|
| :- | :- |
|Release date|10th October 2024|
|SDK Version|3\.3.3|
|Firmware Version|<p>Standard: 1711.2.12.3.3.0.3</p><p>Lite Wireless: 1711.2.12.3.3.2.3</p>|
|GSDK/SiSDK Version|SiSDK 2024.6.2 |
|Studio Version|5\.9.3.0|
|Release Package Name|WiSeConnect3\_SDK\_3.3.3|
|Supported RTOS|FreeRTOS|
|Operating Modes Supported|Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP|

- SiWx917 release consists of two components:
  - SiWx91x Connectivity Firmware:
    - Standard Wireless Firmware - SiWx917 Firmware Binary available as SiWG917-B.2.12.3.3.0.3.rps
    - Lite Wireless Firmware - SiWx917 Firmware Binary is available as SiWG917-B.2.12.3.3.2.3.rps, this image is with reduced features for parts with SiWG917M110LGTBA OPN. 
  - Wiseconnect3 Library - Wiseconnect3 SDK library runs on internal Cortex M4

**Note:**

- Mandatory to upgrade the earlier version of boards (Si917-6031A Pro kit or BRD4338A boards) or 917 Silicon ICs with instructions as outlined in this document "SiWG917–TA\_Flash\_Memory\_Map\_ChangeGuide\_v1.3.pdf" for more details.  
- The release packages will have bug-fixes, enhancements, and new features in both 'SDK' and 'Firmware'. Customer shall update and use 'SDK' and 'Firmware' of same release package. SDK and FW combinations that are not released together are not supported.
- It is recommended to update TA image first followed by M4 image and ensure application compatibility with firmware before OTA
- To use the Dev kit Demo, users need to have Simplicity Connect version 2.9.3 or higher.

## **Supported Hardware**

|**Hardware**|**OPN (Ordering Part Number)**|
| :- | :- |
|IC OPN|<p>QFN OPNs: SiWG917M111MGTBA, SiWG917M100MGTBA, SIWG917M110LGTBA(Lite Wireless Firmware), SiWG917M111XGTBA, SiWG917M121XGTBA, SiWG917M141XGTBA</p><p>Module OPNs: SIWG917Y111MGNBA , SIWG917Y110LGNBA, SIWG917Y111XGNBA, SIWG917Y121MGNBA, SIWG917Y111MGABA, SIWG917Y110LGABA, SIWG917Y111XGABA, SIWG917Y121MGABA</p>|
|Development Kits|<p>Pro Kit: SiWx917-PK6031A, Si917-PK6031A. </p><p>(Pro Kit includes Mother board "Si-MB4002A" + Radio board)</p><p>Radio boards: SiWx917-RB4338A, SiWx91x-RB4342A, SiWx917-DK2605A</p><p>Module boards: SiW917Y-RB4343A</p>|

## **Supported Features** 

<table><tr><th><b>Section</b></th><th><b>Sub-Section</b></th><th><b>Feature</b></th><th><p><b>Lite Wireless Firmware</b></p><p><b>(4MB flash OPN<br>OPN No: SiWG917M110LGTBA)</b></p></th><th><b>Standard Wireless Firmware (For other OPNs)</b></th></tr>
<tr><td rowspan="8">System</td><td rowspan="4">Operating modes</td><td>Wi-Fi STA (802.11ax, 802.11n)</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi 802.11n AP</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi STA (802.11ax, 802.11n) + BLE</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Security </td><td>Secure Boot, Secure Key storage and HW device identity with PUF, Secure Zone, Secure XIP (Execution in place) from flash, Secure Attestation, Anti Rollback, Debug Lock, Flash Protection</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Secure firmware upgrade options</td><td><p>- Firmware loading support by Commander Tool through Jlink Debugger. Jlink connected to Serial Wire Debug (SWD) </p><p>- Firmware loading via ISP using UART (Commander or Serial terminal), SPI Interface</p><p>- Secure Over the Air (OTA) Upgrade</p><p>- Firmware update via Bootloader</p></td><td>Supported</td><td>Supported</td></tr>
<tr><td>Crypto Support</td><td><p>- Crypto API's for Hardware Accelerators: Advanced Encryption Standard (AES) 128/256/192, Secure Hash Algorithm (SHA) 256/384/512, Hash Message Authentication Code (HMAC), Random Number Generator (RNG), SHA3, AES-Galois Counter Mode (GCM)/ Cipher based Message Authentication Code (CMAC), ChaCha-poly, True Random Number Generator (TRNG)</p><p>- Software Accelerators: RSA, ECC</p><p>- PSA Crypto APIs support for all crypto operations.</p><p>- Wrapping Secret keys (Symmetric crypto). </p><p>- Added ECDSA Sign and Verify APIs</p></td><td>Supported</td><td>Supported</td></tr>
<tr><td>System Power Save</td><td><p>- Deep Sleep with RAM retention and without RAM retention. </p><p>- Wireless Power Save: Connected Sleep (Wi-Fi Standby Associated), BLE Advertising with powersave, BLE Scan with powersave,  BLE connection with powersave. Only Max PSP power save mode is supported in BLE. </p></td><td>Supported</td><td>Supported</td></tr>
<tr><td rowspan="13">Wi-Fi</td><td>Wi-Fi Protocols</td><td>IEEE 802.11 b/g/n/ax (2.4GHz)</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Access Point (AP) Mode</td><td><p>- 4 Client Support, Hidden SSID Mode, Auto Channel Selection, Scan in AP mode (Alpha)</p><p>- Wi-Fi Security </p><p>- WPA2 Personal, WPA3 Personal (H2E method only) (Alpha), WPA Mixed mode (WPA/WPA2) </p></td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi Scan</td><td>Selective Scan, Active/Passive Scan</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi STA (Security Modes)</td><td>Open Mode, WPA2 Personal, WPA2 Enhancements, WPA3 Personal, Mixed Mode (WPA/WPA2), WPA3 Personal Transition Mode (WPA2/WPA3)</td><td>Supported</td><td>Supported</td></tr>
<tr><td>WPA2 Enterprise security (STA)</td><td>Method: PEAP/TTLS/TLS 1.0/TLS 1.2/FAST/LEAP</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi STA Rejoin</td><td></td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi STA Roaming </td><td>BG Scan, OKC (Opportunistic Key caching), PMK (Pairwise Master Key) caching, Pre-Authentication</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi Protocol Power Save </td><td>Deep sleep (unconnected state), Max PSP, Enhanced Max PSP, Fast PSP, TWT</td><td>Supported</td><td>Supported</td></tr>
<tr><td>QoS</td><td>WMM-QoS</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi 6 Feature</td><td>MU-MIMO (DL), OFDMA (UL/DL), iTWT, TWT I-Frame & TWT Enhancements (Automatic TWT Configuration), BSS coloring, MBSSID</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi Concurrency </td><td>AP+STA (Same channel)</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi Band/Channels</td><td>2\.4GHz CH1-11, 2.4GHz CH1-13, 2.4GHz CH1-14</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Known Security Vulnerabilities Handled</td><td>WPA2 KRACK Attacks, Fragment and Forge Vulnerability</td><td>Supported</td><td>Supported</td></tr>
<tr><td rowspan="6">Network stack</td><td rowspan="2">Core Networking Features</td><td><p>- IPv4/IPv6/UDP/TCP/ARP/ICMP/ICMPv6</p><p>- SSL client versions TLSV1.0, TLSV1.2, TLSV1.3 </p><p>- SSL server versions TLSV1.0 and TLSV1.2</p><p>- DHCPv4/DHCPv6 Client</p><p>- TCP/IP Bypass (LWIP as Hosted stack for reference)</p></td><td>Supported</td><td>Supported</td></tr>
<tr><td>- DHCPv4 Server, DHCPv6 Server</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td rowspan="2">Advanced Network Features</td><td>HTTP Client/HTTPS Client/DNS Client, Embedded MQTT/MQTT on host (AWS and AZURE) </td><td>Supported</td><td>Supported</td></tr>
<tr><td>SNTP Client, IGMP</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi IoT Cloud Integration</td><td><p>- AWS IOT Core</p><p>- Azure IoT</p></td><td>Supported</td><td>Supported</td></tr>
<tr><td>BSD and IoT sockets application programming interface(API)</td><td></td><td>Supported</td><td>Supported</td></tr>
<tr><td rowspan="2">BLE </td><td>Legacy features</td><td><p>- GAP(Advertising, Scanning, initiation, Connection and Bonding)</p><p>- Generic Attribute Protocol(GATT)</p><p>- Attribute protocol(ATT)</p><p>- Security</p><p>- LL Privacy 1.2</p><p>- Accept list</p><p>- Directed Advertising</p><p>- LE PHY(1Mbps, 2Mbps) & Coded PHY(125kbps, 500kbps)</p><p>- Simultaneous scanning on 1Mbps and Coded PHY</p><p>- LE dual role topology</p><p>- LE data packet length extensions(DLE)</p><p>- Asymmetric PHYs</p><p>- LE channel selection algorithm 2 (CSA#2)</p><p>- LE Secure connections</p></td><td>Supported</td><td>Supported</td></tr>
<tr><td>Advertising Extensions </td><td><p>- Extended Advertising</p><p>- Periodic Advertising</p><p>- Periodic Advertising scanning</p><p>- Extended Advertising scanning</p><p>- Periodic Advertising list</p><p>- LE periodic advertising synchronization</p></td><td>Not Supported </td><td>Supported</td></tr>
</table>

### **MCU**

- **Memory**
  - Common Flash: Single shared Flash for both Cortex-M4 and NWP (Wireless Processor)
  - Common Flash + External PSRAM
  - Stacked PSRAM + External Common Flash
- **Power States**
  - Active: PS4, PS3, PS2, and PS1 
  - Standby: PS4, PS3, and PS2
  - Sleep: PS4, PS3 and PS2
  - Deep Sleep (Shutdown): PS0
- **Peripherals, Services and Hardware Drivers** 

|**HP Peripherals**|**List**|**Notes**|
| :- | :- | :- |
| |<p>- ADC</p><p>- Analog Comparator</p><p>- Config Timer (CT)</p><p>- CRC</p><p>- DAC</p><p>- eFuse</p><p>- EGPIO</p><p>- GPDMA<sup>1</sup></p><p>- GSPI</p><p>- I2C</p><p>- I2S</p><p>- MCPWM</p><p>- PSRAM</p><p>- RNG<sup>1</sup></p><p>- SDIO Secondary</p><p>- SSI (Primary & Secondary)</p><p>- Temperature Sensor</p><p>- UART</p><p>- uDMA</p><p>- USART</p>||
| |<p>- BoD<sup>1</sup></p><p>- CTS (Touch Sensor)<sup>1</sup></p><p>- OPAMP<sup>1</sup></p><p>- QSPI<sup>1</sup></p><p>- QEI<sup>1</sup></p>|Limited Support|
|**ULP Peripherals**|||
| |<p>- ULP\_ADC</p><p>- ULP\_DAC</p><p>- ULP\_GPIO</p><p>- ULP\_I2C</p><p>- ULP\_I2S</p><p>- ULP\_TIMER</p><p>- ULP\_UDMA</p><p>- ULP\_UART</p><p>- ULP\_SSI\_PRIMARY</p>||
|**UULP Peripherals**|||
| |<p>- RTC (Calendar)</p><p>- SYSRTC</p><p>- WDT</p>||
|**Services**| ||
| |Sleep Timer||
| |IOSTREAM||
| |NVM3||
| |LittleFS (for Dual Flash)|Limited Support|
| |Power Manager||
| |Sensor Hub|Limited Support|
| |Pin Tool|Limited Support|
|**Hardware Drivers**| ||
| |LED, Button, MEMLCD, Joystick, Sensors (RHT, VEML, ICM)||

|The peripherals marked with superscript<sup>1</sup> are available through RSI APIs.  Support for SL APIs for user facing peripherals will be available in future releases.|
| :- |

|The flash write feature has been enhanced to support the NWP area, providing a 20k allocation within the NWP flash memory for storing user data. Moreover, a Read API has been introduced to retrieve data from the NWP flash region.|
| :- |

### **Developer Environment**

- Simplicity Studio IDE (SV5.9.3.0 version) and Debugger Integration. Refer to the latest version of the SoC "Getting-Started-with-SiWx917" guide for more details. 
- Recommended to install and use Silicon labs Simplicity SDK (Previously known as Gecko SDK), Git hub based version 2024.6.2.
- Simplicity Commander to supports Flash loading, provision of MBR programming, security key management, and calibration support for crystal and gain offsets. refer "siwx917-soc-manufacturing-utility-user-guide" for more details. 
- Advanced Energy Monitoring (AEM) to measure ultra-low power capability on Development boards (Pro Kit).
- PinTool for MCU pin configurations

### **BLE** 

- GAP(Advertising, Scanning, initiation, Connection and Bonding)
- Generic Attribute Protocol(GATT)
- Attribute protocol(ATT)
- Security
- LL Privacy 1.2
- Accept list
- Directed Advertising
- Extended Advertising
- Periodic Advertising
- Periodic Advertising scanning
- Extended Advertising scanning
- Periodic Advertising list
- LE periodic advertising synchronization
- LE PHY(1Mbps, 2Mbps) & Coded PHY(125kbps, 500kbps)
- Simultaneous scanning on 1Mbps and Coded PHY
- LE dual role topology
- LE data packet length extensions(DLE)
- Asymmetric PHYs
- LE channel selection algorithm 2 (CSA#2)
- LE Secure connections
- Bluetooth 5.4 Qualified

### **SDK**

- Simplified and Unified DX for Wi-Fi API and Platform APIs
- Simplifies application development and presents clean and standardized APIs
- UC (Universal Configurator) enables componentization, simplifying configuration of peripherals and examples
- BSD and ARM IoT-compliant socket API
- Available through Simplicity Studio and GitHub

### **Multi-protocol**

- Wi-Fi STA + BLE

### **PTA/Coexistence**

- 3 wire CoEx acting as Wi-Fi with external Bluetooth
- 3 wire CoEx acting as Wi-Fi with external Zigbee/OT

## **Changes in this release compared to v3.3.2 Release**

### **System**

- **Enhancements / New features**
  - Added limited anti-rollback feature for firmware update
- **Fixed Issues**
  - None
- **Documentation**
  - None

### **MCU**

- **Enhancements / New features**
  - Added platform examples support for below SiWG917Y Module OPNs,
    - SIWG917Y110LGNBA, SIWG917Y111XGNBA, SIWG917Y121MGNBA, SIWG917Y111MGABA, SIWG917Y110LGABA, SIWG917Y111XGABA and SIWG917Y121MGABA
  - Added Pin-Annotation support for the following components,
    - sl\_si91x\_button\_917, sl\_icm40627, sl\_si70xx, iostream\_si91x, memlcd\_917 and sl\_joystick
  - Added UC support to configure supported UART instance and baudrate for debug logs
  - Added support to convert Unix timestamp to Calendar datetime and vice-versa
  - Added provision to disable MEMLCD
  - Enhanced RGB LED driver by adding support for multiple instances
- **Fixed Issues**
  - Fixed the issue with high sleep current (~100 µA) in the powersave\_standby\_associated example
  - Fixed Calendar/RTC clock drift issue
  - Fixed the flashing/erasing issue when performing M4/TA firmware update from version 3.2.0 to 3.3.0 on custom platform with external oscillator
  - Fixed the issue when there is a NAK from the I2C follower
  - Fix for I2C Write failure across sleep-wakeup
  - Fixed the ADC deinitialization and initialization issue upon wakeup
  - Fixed SDIO secondary build errors
  - Fixed ADC channel instance configuration for Joystick
  - Fixed LED1 configuration for PWM
  - Fixed the default instance for RGB LED application on the Devkit
  - Fixed build errors when configuring BSS, Data and Stack segments to PSRAM
  - Fixed the false interrupt issue when using button-based wakeup
  - Fixed M4 Source Clock discrepancy issue 
- **Documentation**
  - Updated migration guide with host pad selection API change information
  - Added a generic peripheral API flow diagram w.r.t Sleep Wakeups in API Reference Guide
  - Updated documentation regarding ISP mode in trouble shooting guide 
  - Updated ULP UART readme for GPIO 8 and 9 configuration as Rx and Tx

### **SDK**

- **Enhancements / New features**
  - Updated aws\_starfield\_ca.pem.h certificate.(includes Starfield G2 Root CA).
  - Added support for combined firmware upgrade feature in the http\_otaf and http\_otaf\_twt example applications.
  - Moved powersave\_standby\_associated example from featured  to featured/low\_power  section.
  - Support for SL\_WIFI\_SCAN\_TYPE\_EXTENDED in sl\_wifi\_scan\_type\_t and sl\_wifi\_get\_stored\_scan\_results() API to retrieve extended scan results. 
  - Added OPN support for SiWG917Y110LGNBA board.
  - Updated the SiWx917 development kit demo to use provisioning over BLE.
- **Fixed Issues**
  - Fixed sl\_mqtt\_client\_connect() API to handle WLAN disconnection during MQTT connection.
  - Fixed SOFT\_AP\_PSK and SOFT\_AP\_SSID defines to have generic default PSK and SSID for AP in cli\_demo example.
  - Updated SOFT\_AP\_PSK and SOFT\_AP\_SSID defines to have generic default PSK and SSID for AP and updated example readme for the same of Cli Demo example application.
  - Fixed certificate loading errors when No Optimization  compiler option is enabled in example applications.
  - Fixed handling of sl\_calib\_write command in wifi\_calibration example application.
  - Fixed handling of Wi-Fi client disconnections in concurrent mode for socket connections.
  - Fixed the issue where the device not going to sleep immediately after power save API is called while handling multiple DNS queries.
  - Fixed the issue where the DUT takes 2-4 minutes to reconnect with the AP and the Si-connect app shuts down in mid-iterations.
- **Documentation**
  - Added usage note for SL\_WIFI\_MAX\_SCANNED\_AP.
  - Updated the documentation for sl\_wifi\_start\_scan API to add more information about SL\_WIFI\_SCAN\_TYPE\_EXTENDED.
  - Updated cli\_demo example readme with SOFT\_AP\_PSK and SOFT\_AP\_SSID defines.
  - Updated migration guide from v3.3.2

### **Wi-Fi/Network Stack**

- **Enhancements / New features**
  - Added support for ALPN extension for TLS protocol.
  - Support added for SRRC region configuration
  - CRYPTO\_HARDWARE is now supported during rejoin process.
  - Added region configuration restrictions for SiWG917Y modules
  - 11ax receive performance improvement with transmit ppm
  - Improved receive sensitivity performance in the presence of transmitter carrier frequency offset (CFO).
- **Fixed Issues**
  - Addressed  fixes for Netx Vulnerabilities (CVE-2023-48315 ,CVE-2023-48316 ,CVE-2023-48691 ,CVE-2023-48692 )
- **Documentation**
  - None

### **BLE**

- **Enhancements / New features**
  - Limited the Transmit power to 4dBm for WORLDSAFE and SRRC regions, and removed the option for users to modify BLE Gain tables for the SiWG917Y module
  - Added BLE examples support for SiWG917Y110LGNBA  OPN. 
- **Fixed Issues**
  - Removed the BLE packet transmission on channel-39 (2480MHz) at a 2Mbps data rate.
  - Fixed BLE HP chain RX issues across temperature on few parts through improvements in bootup calibration.
  - Improved BLE transmit modulation parameters and harmonic emissions on the 8dBm transmission path. As a result, the active current of BLE transmit (8dBm path) can increase by around 1mA.
  - Enhanced the packet reception performance in the BLE LP chain.
  - Enhanced the BLE TX power in HP chain to align with the datasheet numbers.
  - Fixed the DUT hang issue at the rsi\_bt\_get\_local\_dev\_address API in central role when using PSRAM boards (121x, 141x, 111M) and enabling power save in the ble\_privacy application
- **Documentation**
  - None

### **Multi-protocol**

- **Enhancements / New features**
  - None
- **Fixed Issues**
  - None
- **Documentation**
  - None

## **Recommendations**

### **System**

- The current revision of SiWx917 has:
  - RAM memory of 672k bytes which can be shared between TA and M4 processors in SoC mode. 
  - The below configurations are applicable in SoC mode and can be configured based on the application requirement. EXT\_FEAT\_352K\_M4SS\_320K is the default configuration, based on requirement EXT\_FEAT\_480K\_M4SS\_192K configuration is selected for SoC mode multi-protocol examples.
    - EXT\_FEAT\_480K\_M4SS\_192K - This mode configures TA with 480k and M4 with 192K bytes of memory
    - EXT\_FEAT\_416K\_M4SS\_256K - This mode configures TA with 416k and M4 with 256K bytes of memory
    - EXT\_FEAT\_352K\_M4SS\_320K - This mode configures TA with 352k and M4 with 320K bytes of memory
  - SoC mode should not use 672k\_M4SS\_0K memory configuration.
- Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
- There are 2 Versions of Pro-Kits/Radio boards. Si917-6031A based on Si917-4338A (Rev **A01 - A11**) and SiWx917-6031A based on SiWx917-4338A (Rev A12-A14). To get optimal power numbers, enable macro "SL\_SI91X\_ENABLE\_LOWPWR\_RET\_LDO" pre-processor define for ICs or while using SiWx917-6031A Pro-kit, SiWx917-4338A version of boards. This macro should be disabled for earlier variants of the board (Si917-6031A, Si917-4338A).
- With RAM configuration (EXT\_FEAT\_352K\_M4SS\_320K), only 352K memory is available to TA  which limits the features supported, Recommended to enable EXT\_FEAT\_416K\_M4SS\_256K in Wi-Fi + BLE Multi protocol mode to enable more Network features.
- For EXT\_FEAT\_416K\_M4SS\_256K  and EXT\_FEAT\_480K\_M4SS\_192K memory configurations, it is recommended to retain both TA and M4 RAMs in power save.

### **Wi-Fi/Network Stack**

- It is recommended to enable SL\_SI91X\_EXT\_TCP\_IP\_WAIT\_FOR\_SOCKET\_CLOSE BIT(16) of the 'Extended TCP IP Feature' bit map in the opermode command for all Wi-Fi Socket operations from the host to ensure graceful handling during asynchronous closures from the peer.
- For high throughputs,  it is recommended to enable BIT(2) - SL\_SI91X\_FEAT\_AGGREGATION  of feature\_bit\_map in opermode. 
- Users can enable SL\_SI91X\_EXT\_TCP\_IP\_SSL\_16K\_RECORD in 'Extended TCP IP Feature' bit map in opermode for (HTTPS server) supporting 16k record.
- **TWT**
  - Recommendation is to use sl\_wifi\_target\_wake\_time\_auto\_selection() API for all TWT applications. 
  - It is recommended to issue iTWT setup command once IP assignment, TCP connection, application specific socket connections are done.
  - When using sl\_wifi\_enable\_target\_wake\_time API, increase TCP / ARP Timeouts at the remote side depending upon the configured TWT interval configured. It's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() as an alternative.
  - In case of TWT in CoEx mode, when using sl\_wifi\_enable\_target\_wake\_time API, use TWT wake duration <= 16 ms and TWT wake interval >= 1 sec. If wake duration > 16 ms or TWT wake interval < 1sec, there might be performance issues.
  - For iTWT GTK interval in AP should be configured to max possible value or zero. If GTK interval is not configurable on AP side, recommended TWT interval (in case of sl\_wifi\_enable\_target\_wake\_time API) or RX Latency (in case of sl\_wifi\_target\_wake\_time\_auto\_selection API) is less than 4sec.
  - When sl\_wifi\_enable\_target\_wake\_time API is used, configuring TWT Wake interval beyond 1 min might lead to disconnections from the AP. Recommended to use TWT wake interval of less than or equal to 1 min.
  - When using sl\_wifi\_enable\_target\_wake\_time API, it is recommended to set missed\_beacon\_count of sl\_wifi\_set\_advanced\_client\_configuration API greater than 2 times of the configured TWT Interval.
  - DUT keepalive should be configured aligned with AP keepalive in TWT modes.
- Disable power save for high throughput applications or use FAST PSP power save mode as per application requirement.
- The application needs to ensure that it sets RTC with the correct timestamp before establishing the SSL/EAP connection.
- The minimum timeout value should not be less than 1 second for socket select and socket receive calls. 
- Please refer Keep alive intervals supported by MQTT broker and configure keep alive interval values accordingly.
- The minimum keep alive interval value recommended for embedded MQTT is 10 Seconds. 
- Disable power save and suspend any active TWT sessions before triggering HTTP OTAF.
- Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with the power save.
- Recommended to configure VAP\_ID properly for Si91x STA and AP using sl\_si91x\_setsockopt\_async(), in case of data transfer.
- Recommended to use valid length(<= 202 bytes) for topic to be published while using Embedded MQTT, else it leads to return wrong error code(0x21).
- In concurrent mode with dual IP, it is advised to bring up STA first (IP configuration) and AP later.
- It is recommended to configure Tx ,Rx , Global buffer pool ratio in the buffer config command based for all Wi-Fi Socket operations from the host
- It is recommended to use "TCP exponential backoff" configuration for congested channels.
- It is recommended is to disable broadcast filter during TCP connection to avoid ARP resolution issues
- To avoid IOP issues, it is recommended to disable power save before Wi-Fi connection.
- It is recommended to set region\_code as `IGNORE_REGION` in boot configurations for SIWG917Y module boards except for PER mode.

### **BLE**

- In BLE, the recommended range of Connection Interval in
  - Power Save (BLE Only) - 100 ms to 1.28 s.
- In BLE, during Connection, the configuration of Scan Interval and Scan Window with the same value is not recommended. The suggested ratio of Scan Window to Scan Interval is 3:4.
- In BLE, if a device is acting as Central, the scan window (in set\_scan\_params and create\_connection commands) must be less than the existing Connection Interval. The suggested ratio of Scan Window to Connection Interval is 2:3.
- In BLE mode, if scanning and advertising are in progress on the SiWx91x module and it subsequently gets connected and moves to the central role, scanning stops else if it moves to the peripheral role, advertising stops. To further establish a connection to another peripheral device or to a central device, the application should give a command for starting advertising and scanning again.

### **Multi-protocol**

- For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
- Wi-Fi + BLE Advertising
  - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals may be observed.
  - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity.
- Wi-Fi + BLE scanning
  - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
  - BLE scanning will be stopped for intervals that collide with Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Connections
  - All standard connection intervals are supported.
  - For a stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Data Transfer
  - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout.
  - Ensure Wi-Fi activity consumes lower intervals.

### **MCU**

- The WDT manager is specifically meant for system reset recovery and should not be utilized for any other purpose. When interrupts are disabled, make sure to stop the WDT to avoid unintended resets. Once interrupts are re-enabled, restart the WDT to ensure system reliability
- It is strongly recommended to use `sl_si91x_soc_nvic_reset()` API for system soft reset rather than the `sl_si91x_soc_soft_reset()` function, since this uses the WDT for soft reset, which is specifically intended for system reset recovery
- It is strongly recommended not use switch\_m4\_frequency() for clock scaling. Refer to the migration guide for more details
- For GPIO-based wakeup, ensure the GPIO component is installed in powersave applications
- Use both CTS and RTS for UART flow control
- PSRAM examples are not supposed to be used with Non-PSRAM OPNs
- SYSRTC and wake on wireless wakeup resources are enabled by default when tickeless idle mode is enabled. Avoid installing sleeptimer component in power save applications (where FreeRTOS tickless is enabled by default)
- Minimum idle-time to sleep is configured to 100ms in FreeRTOS\_config.h
- Enable DMA to achieve better throughput numbers
- For I2C Fastplus and High speed modes, use high power instances with core clock frequencies between 80MHz and 180MHz
- Use blocking calls instead of non-blocking calls for I2C leader in ULP\_I2C while receiving data
- In RTOS environment, prefer Signaling mechanisms (Semaphore/Mutex/EventFlag etc.) instead of "Variables/Flags" from user callbacks to detect "\*Transfer Complete\*" for high speed communication peripherals
  Refer Software Reference Manual for more details
- Install the 'device\_needs\_ram\_execution' component for OPN-based firmware update examples
- It is strongly recommended to use the FreeRTOS tickless based powersave instead of the sl\_si91x\_m4\_sleep\_wakeup() (legacy) API
- For ACx Modules, it is necessary to initialize the internal 32kHz RC clock specifically for the LF\_FSM domain. GPIO oscillator clock does not propagate to the LF\_FSM. This may lead to timer accuracy issues when using the 32kHz RC clock.
- If the external oscillator connected to UULP\_GPIO\_3 is used as the 32kHz clock source, make the necessary code changes as detailed in the Software Reference Manual
- It is strongly recommended not to use RO clock in MCU
- For BLE, CoEx, and high-accuracy MCU applications it is recommended to use external 32kHz crystal on XTAL\_32KHZ\_P and XTAL\_32KHZ\_N pins.

## **Known Issues of WiSeConnect3\_SDK\_3.3.3 Release**

### **MCU**

**GSPI**

- First 2 MSB bits of the first byte on MISO are garbled

**GPIO**

- By default, sl\_gpio\_set\_configuration() sets the GPIO to HIGH 

**SSI**

- In ULP SSI, non-DMA transfer is not working

**I2C**

- ULP\_I2C non-blocking receive in low power mode will not function as expected

**I2S**

- FIFO threshold configurations are not working

**Config Timers**

- PWM mode is not working

**ADC**

- ADC static mode sampling rate is supported up to 2.5Msps

**Analog comparator**

- opamp2, opamp3 & BOD configurable Inputs do not work

**Temperature Sensors**

- BJT Temperature readings are not accurate

**QSPI**

- QUAD mode not working

**Power Manager**

- BOD and Comparator wakeup sources not working
- PS2 -PS4 transition is not working in SiWG917Y110LGNBA part

**SensorHub**

- Power transitions with AWS are not stable in SensorHub
- SPI sensor is not working
- Button interrupt is not working

### **SDK**

- Observed Wi-Fi connection is successful even after deleting the stored network credentials using sl\_net\_delete\_credential and responding with SL\_NET\_INVALID\_CREDENTIAL\_TYPE for sl\_net\_get\_credential.
- Enhanced sl\_wifi\_get\_firmware\_version() API to provide more details (ROM ID, chip ID, security version, etc) which is not backward compatible with firmware older than 1711.2.10.1.0.0.4. Firmware binary notation does not include the security version number.
- In PSRAM enabled demos, moving of text, data and stack segments to PSRAM is allowed. BSS and Heap should still be in SRAM.
- Asynchronous Azure MQTT is not supported, this will be addressed in upcoming release(s).
- mDNS with IPV6 is not supported.
- Bus thread stack may need to increase if local variables are used in user callback to avoid stack overflow.
- Low Power examples usage and documentation still under scope of improvement.
- Observed sl\_wifi\_get\_wireless\_info() API is giving wrong security type and PSK for WPA3 Transition supported client mode.
- Observed socket close is not working as expected for TLS socket when socket connect, send data and socket close are performing in a continuous loop.
- Observed data not received simultaneously when two sockets call recv() from two different RTOS tasks.
- WMM-PS/UAPSD is not supported
- firmware\_flashing\_from\_host\_uart\_xmodem example fails to communicate over UART.

### **Wi-Fi/Network Stack**

**Wi-Fi STA**

- STA Connection with the WPA3 Hunting and Pecking algorithm takes about 3-4 seconds.
- Connection failures have been observed with certain APs in environments with high channel congestion (~50-60% occupancy in an open lab).
- Region selection based on country IE in the beacon is not supported in ICs
- Intermittent beacon reception from Access Point (beacon misses) occurs when channel congestion exceeds 85%.
- When scanning with low power mode enabled, a sensitivity degradation of 3-6dB is observed, which may prevent APs at longer ranges from appearing in the scan results.
- Passive scan is failing when DUT is configured in world domain.
- For ICs, the region codes DEFAULT\_REGION and IGNORE\_REGION are not supported
- For modules, the region codes DEFAULT\_REGION and IGNORE\_REGION are not supported in PER mode.
- Observed ~2% increase in listen current and ~1% increase in standby associated current.
- Tx max powers for EVM limited data rates (like MCS7, MCS6, 54M, etc) will be reduced by 0.5dB.

**Access Point (AP) Mode**

- Fixed rate configuration in AP mode using sl\_wifi\_set\_transmit\_rate API is not being set as expected. 

**WPA2 Enterprise security (STA)**

- Observed connection issue with configuring certificate key and programming 4096 bit key and SHA384/SHA512 certificates.

**Wi-Fi Concurrency (AP + STA in same channel)**

- Observed 3rd party STA association fail with 917 AP while 917 STA mode is connecting/reconnecting to configured 3rd party AP. Reconnect 3rd party STA to 917 AP in such scenarios. 
- In concurrent mode, 917 AP cannot process de-authentication frames sent by third-party STA if 917 STA is connected to WPA2+WPA3 enabled AP.

**OFDMA (UL/DL)**

- Less throughput observed in DL-OFDMA with some APs that enabled Low density parity check coding.

**MU-MIMO (DL)**

- For CoEx Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Performance, Interop issues with MU MIMO with certain APs. 
- Less throughput observed in MU-MIMO with some APs that enabled  Low density parity check coding

**MU-MIMO (UL)**

- UL MU-MIMO is not supported.

**TWT**

- When sl\_wifi\_enable\_target\_wake\_time() API is used, occasional MQTT disconnections may be observed if TWT is configured with longer TWT intervals (>30secs) with embedded MQTT + TWT.  As an alternative, it's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() API, where these dependencies are internally handled.

**Wi-Fi STA Rejoin**

- Observed Scanning (probe request) in all channels instead of the channels configured in selective channel(channel\_bitmap\_2g4) during rejoin process. 

**IPv4/IPv6**

- IP change notification is not indicated to the application. 
- In concurrent mode with dual IP, if the STA starts after AP is up, the STA IP configuration may fail for DHCP stateless mode.
- In concurrent mode, data transfer using the Link-local address will always use the first IP interface created by the application.

**BSD Socket API**

- Every server socket created consumes a socket (maximum of 10 sockets supported) and every subsequent connection to server socket consumes an additional socket (from the same pool of 10 sockets), which limits the number of server connections supported.
- Observing issues with TCP retries when power save mode is enabled, especially when the module is in idle state.
- TCP maximum retry value is valid upto 31

**SSL Client/Server**

- Sometimes during SSL Handshake, ECC curve parameters generated are wrong, resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
- Secure SSL renegotiation is not supported in the Embedded Networking Stack

**HTTP Client/ HTTPS Client**

- Observed occasional HTTPS continuous download failures when power save is enabled. Recommended to disable it before performing HTTPS continuous downloads

**SNTP**

- Unable to get SNTP async events when CoEx mode and power save are enabled 

**Throughputs & Performance**

- Wi-Fi alone throughput is about SDK 3.x (42Mbps). SDK refinements are in progress to further improve Wi-Fi Standalone and CoEx Throughputs. 

**Wi-Fi IOT Cloud integration**

- **AWS IOT Core**
  - Observed AWS MQTT keepalive transmission is not happening at expected intervals with power save enabled.
  - Observing LAST\_WILL\_MESSAGE is random at every MQTT connection rather than the configured Message/Length
- **AZURE IOT Core**
  - Observed DUT after sending the data, its not sending the MQTT keep alive packet due to this Azure HUB closing the connection when power save is enabled.

**Wi-Fi Interoperability (IOP)**

- Observed disconnections with Amplifi (AFI-INS-R) AP with powersave enable
- TWT session is failing due to disconnections observed in DUT if rx\_latency is set to 55 seconds and receive data is also set to 55 seconds on MI Xiaomi RA72 and Tplink AX53 AP's
- Observed less throughput(~1Mb) while running TCP RX with Max\_PSP powersave with DLink 810 AP
- Observed interop issue (random disconnections) with few APs (EERO 6+, EERO PRO 6E, Cisco Catalyst 9120AXID)
- Disconnections observed with Netgear RAX120 AP in WPA3 Security

### **BLE**  

**DTM/PER**

- Recommend to limit BLE Tx Maximum power to 18 dBm.  Please don't use for 127 power\_index for BLE HP chain with this release.

**DLE**

- Removed the ble\_data\_length PSRAM example as it does not work with 121x and 141x OPN's. 

**Privacy** 

- DUT hang at the rsi\_bt\_get\_local\_device\_address API in central role, when using PSRAM boards (121x, 141x, 111M) and enabling power save in the ble\_privacy application,

AE 

- Observed DUT hang issue while running TX notifications in peripheral role.

**SMP**  

- SMP is not working with 110L(Lite wireless firmware image) board. 

**Throughput & performance** 

- BLE throughput in LITE version is reduced compared to Standard Wireless Firmware
- The DUT hangs when the SRRC region is set in the ICs. However, this issue does not occur with the SiWG917Y module. 



### **Multi-protocol**

- For CoEx Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Wi-Fi + BLE intermittent connection failures, disconnections, and data transfer stalls in the long run when power save is enabled.
- Observed "DUT is not disconnecting to the AP when initiating disconnection from EFR connect app screen using wifi\_station\_ble\_provisioning\_aws example
- Observed DUT failed to load certificate with error "0x10026" (SL\_STATUS\_SI91X\_WRONG\_PARAMETERS) while running wifi\_https\_ble\_dual\_role\_v6 application
- Observed BLE bonding failure during continuous HTTPS download.
- Observed BLE and WLAN connection failure with SMP, when WLAN connect and HTTPS GET called in a loop.
- Observed issue in displaying throughput for interval of 'TEST\_TIMEOUT' when CONTINUOUS\_THROUGHPUT enabled in wifi\_station\_ble\_throughput\_app demo
- In CoEx opermode, with memory config of TA - 480K and M4 - 192K Wi-Fi through is degrading by 10Mbps due less memory available for throughput.

### **System**

- Observed random hang issues with encrypted firmwares on some earlier variant of boards  (Si917-6031A, Si917-4338A) with powersave enable.
- This release addresses several issues, resulting in an additional 1K RAM usage in the NWP core. Consequently, this reduces the available heap size by 1K for the NWP core. Users with configurations that were already near the heap limit may experience either minor throughput issues OR functionality issues with this update.

On encountering a problem, it can be mitigated by considering the following options:

- Reduce the number of enabled features in the NWP core.
- Switch to a memory configuration that allocates more RAM to the NWP core while reducing RAM allocated to the Host processor.

### **Simplicity Studio and Commander** 

- Simplicity commander does not support options under "Debug Lock tools".
- All projects in the package are compatible with **GNU ARM V12.2.1** toolchain

## **Limitations and Unsupported Features**  

### **System**

- 16MB Flash is not supported
- 16MB PSRAM is not supported
- Dual-Host mode is not supported

### **SDK**

- Baremetal mode is not supported.
- Zephyr is not supported

### **Wi-Fi/Network Stack**

- TLS 1.3 Server is not supported.
- 40 MHz bandwidth for 2.4 GHz band is not supported.
- Max 3 SSL sockets are supported in Wi-Fi alone and CoEx modes. No.of  SSL Sockets in Wi-Fi + BLE based on RAM memory configuration selected. 
- In SSL ECC Curve ID supported is 23. SSL handshake with 3rd party clients depends on the SSL ECC Curve ID.
- The number of Non-Transmitting BSSIDs processed is limited by the beacon length that can be processed by the stack (which is 1024 bytes). Beacons greater than 1024 Bytes in length will not be processed.
- Multiprotocol (STA +BLE) + EAP Security modes supported only with Memory configurations EXT\_FEAT\_416K\_M4SS\_256K and EXT\_FEAT\_480K\_M4SS\_192K.
- UL-MU-MIMO is not supported.
- WPA3 AP supports only H2E algorithm.
- PMKSA caching is not supported in WPA3 AP mode.
- Maximum embedded MQTT Publish payload is 1 kByte.
- Timeout value for socket select and socket receive calls of less than 1 second is not currently supported.
- SA query procedure not supported in 11W AP mode.
- WPA3 AP transition mode is not supported.
- AP standalone mode does not support Tx aggregation. Rx aggregation is supported with limited number of BA sessions.
- In concurrent AP mode, aggregation (Tx/Rx) is not supported.
- Embedded HTTP Server is not supported.
- mDNS with IPV6 is not supported.
- Low power scan supports 1 Mbps packets reception only.
- Auto PAC Provisioning in EAP-FAST with TLSv1.2 is not supported.
- bTWT , Intra PPDU Power save, Spatial Re-Use, BSS coloring features not supported
- HTTPS server is not supported.

### **BLE**

- For BLE, if the connection is established with a small connection interval (less than 15 ms), simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported.
- BLE maximum two concurrent connections are supported, which can be either a connection to two peripheral devices, to one central and one peripheral device or two central devices.
- BLE Slave latency value is valid up to 32 only.
- Maximum supported AE data length is 200 bytes.
- Supports only two ADV\_EXT sets.
- Supports only two BLE connections (1 Central and 1 Peripheral) with AE.
- Advertising Extension feature is not supported in Coexistence.
- The  ae\_central & ae\_peripheral applications are not supported with TA\_352K\_M4\_320K RAM configuration.
- Two BLE connections are not supported with M4 powersave. It only supports a single connection. 
- Isochronous channels feature is not supported. 
- Connection subrating feature is not supported. 
- LE power controller feature is not supported.
- EATT feature is not supported.
- Periodic Advertising with a response feature is not supported. 
- BLE Audio is not supported.
- The feature of dynamically changing the TX power when extended advertising is active is not supported.
- EFR Connect mobile application doesn't have support to differentiate the BLE configurators based on the Bluetooth Device address.
- The maximum BLE power has been reduced by 2dB compared to the Datasheet Number, which will be addressed in the upcoming 3.3.1 patch release.
- ICs do not support DEFAULT\_REGION and IGNORE\_REGION region codes.
- Modules do not support DEFAULT\_REGION and IGNORE\_REGION region codes in PER mode.

### **MCU**

- SensorHub supports PS1-power state with ADC sensor (FIFO mode not supported) . In this mode, the other sensor's operation is not supported.
- FreeRTOS Tickless IDLE mode is not supported in sensor hub application
- LittleFS support is intended to be used only with Dual flash
- PS1 state is not supported in Power Manager
- Power consumption is same for PS4 and PS3 states with powersave mode of power manager
- When using Button\_0 as wakeup source, it limits the button functionality at application layer. For using Button\_0 for application specific functionality, enable 'SL\_SI91X\_NPSS\_GPIO\_BTN\_HANDLER'
- Manual chip select option is not supported when using GSPI
- Mutli-slave mode is not supported in SSI Primary
- Dual and quad modes are not supported in SSI Primary
- Config timer doesn't support 32-bit timer
- Config timer features to trigger DMA and interrupts on events or counters are not supported
- UART instances does not support different FIFO Thresholds
- In UART Character Timeout feature is not supported
- Lower baud rates 110, 150, bit-width 1-4 and 9 are not supported in UART/USART
- RS485 Interface configuration is not supported
- In SDIO function2 to function5 are not supported
- Multichannel and external event based sampling are not supported in ADC
- Fast plus and High Speed modes are not supported in ULP\_I2C instance
- I2S-PCM is not supported
- MPU is not supported
- CPC is not supported
- Hardware Flow control for ULP UART is not supported
- HSPI Secondary is not supported
- Using a 32kHz external oscillator (connected to UULP GPIOs) may cause timer drift in the UULP peripherals
- Sample app and API information (in the API reference guide) for RSI based peripherals is not present

### **Multi-protocol**

- Wi-Fi AP + BLE currently not supported. 
- EXT\_FEAT\_352K\_M4SS\_320K RAM configuration is not supported for CoEx mode with SSL

## **Removed/Deprecated Features**

- Removed IR, SIO, RO temp sensor and FIM
- Removed support for WDT reset upon processor lock up
- Removed I2C SMBUS feature
- Removed hardware\_setup() from all power save application that uses sllib\_m4\_power\_save.slcc component
- sl\_si91x\_m4\_sleep\_wakeup() will be deprecated from upcoming releases
- switch\_m4\_frequency() will be deprecated from upcoming releases
- RO\_32KHZ\_CLOCK and MCU\_FSM\_\_CLOCK macros are removed

> **Note:** 
> 
> The following BLE Synchronous API's will be deprecated soon and the equivalent Asynchronous API's will be used instead in all BLE applications :
> 
> |**S.NO**|**BLE Synchronous API's** |**BLE Asynchronous API's** |
> | :- | :- | :- |
> |1|rsi\_ble\_get\_profiles|rsi\_ble\_get\_profiles\_async|
> |2|rsi\_ble\_get\_profile|rsi\_ble\_get\_profile\_async|
> |3|rsi\_ble\_get\_char\_services|rsi\_ble\_get\_char\_services\_async|
> |4|rsi\_ble\_get\_inc\_services|rsi\_ble\_get\_inc\_services\_async|
> |5|rsi\_ble\_get\_char\_value\_by\_uuid|rsi\_ble\_get\_char\_value\_by\_uuid\_async|
> |6|rsi\_ble\_get\_att\_descriptors|rsi\_ble\_get\_att\_descriptors\_async|
> |7|rsi\_ble\_get\_att\_value|rsi\_ble\_get\_att\_value\_async|
> |8|rsi\_ble\_get\_multiple\_att\_values|rsi\_ble\_get\_multiple\_att\_values\_async|
> |9|rsi\_ble\_get\_long\_att\_value|rsi\_ble\_get\_long\_att\_value\_async|
> |10|rsi\_ble\_set\_att\_value|rsi\_ble\_set\_att\_value\_async|
> |11|rsi\_ble\_set\_long\_att\_value|NA|
> |12|rsi\_ble\_prepare\_write|rsi\_ble\_prepare\_write\_async|
> |13|rsi\_ble\_execute\_write|rsi\_ble\_execute\_write\_async|
> |14|rsi\_ble\_indicate\_value\_sync|rsi\_ble\_indicate\_value|

<br>

# **WiSeConnect3\_SDK\_3.3.2 SoC Release Notes**

## **Release Details**

|**Item**|**Details**|
| :- | :- |
|Release date|5th September, 2024|
|SDK Version|3\.3.2|
|Firmware Version|<p>Standard: 1711.2.12.2.1.0.9</p><p>Lite: 1711.2.12.2.1.2.9</p>|
|GSDK/SiSDK Version|SiSDK 2024.6.1|
|Studio Version|5\.9.2.0|
|Release Package Name|WiSeConnect3\_SDK\_3.3.2|
|Supported RTOS|FreeRTOS|
|Operating Modes Supported|Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP|

- SiWx917 release consists of two components:
  - Standard Wireless Firmware - SiWx917 Firmware Binary available as SiWG917-B.2.12.2.1.0.9.rps
  - Lite Wireless Firmware - SiWx917 Firmware Binary is available as SiWG917-B.2.12.2.1.2.9.rps, this image is with reduced features for parts with SiWG917M110LGTBA OPN. 
  - Wiseconnect3 Library - Wiseconnect3 SDK library runs on internal Cortex M4

**NOTE:**

- Mandatory to upgrade the earlier version of boards (Si917-6031A Prokit or BRD4338A boards) or 917 Silicon ICs with instructions as outlined in this document "SiWG917–TA\_Flash\_Memory\_Map\_ChangeGuide\_v1.3.pdf" for more details.  
- The release packages will have bug-fixes, enhancements, and new features in both 'SDK' and 'Firmware'. Customer shall update and use 'SDK' and 'Firmware' of same release package. SDK and FW combinations that are not released together are not supported.
- It is recommended to update TA image first followed by M4 image and ensure application compatibility with firmware before OTA

## **Supported Hardware**

|**Hardware**|**OPN**|
| :- | :- |
|Silicon (IC OPN)|<p>QFN OPNs: SiWG917M111MGTBA, SiWG917M100MGTBA, SIWG917M110LGTBA(lite image), SiWG917M111XGTBA, SiWG917M121XGTBA, SiWG917M141XGTBA</p><p>Module OPNs: SIWG917Y111MGNBA</p>|
|Development Kits|<p>Pro Kit: SiWx917-PK6031A, Si917-PK6031A. </p><p>(Pro Kit includes Mother board "Si-MB4002A" + Radio board)</p><p>Radio boards: SiWx917-RB4338A, SiWx91x-RB4342A, Early Access for new board BRD2605A</p><p>Module boards: SiW917Y-RB4343A(AC0)</p>|

## **Supported Features** 

<table><tr><th><b>Section</b></th><th><b>Sub-Section</b></th><th><b>Feature</b></th><th><p><b>Lite Wireless Firmware</b></p><p><b>(4MB flash OPN<br>OPN No: SiWG917M110LGTBA)</b></p></th><th><b>Standard Wireless Firmware (For other OPNs)</b></th></tr>
<tr><td rowspan="8">System</td><td rowspan="4">Operating modes</td><td>Wi-Fi STA (802.11ax, 802.11n)</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi 802.11n AP</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi STA (802.11ax, 802.11n) + BLE</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Security </td><td>Secure Boot, Secure Key storage and HW device identity with PUF, Secure Zone, Secure XIP (Execution in place) from flash, Secure Attestation, Anti Rollback, Debug Lock, Flash Protection</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Secure firmware upgrade options</td><td><p>- Firmware loading support by Commander Tool through Jlink Debugger. Jlink connected to Serial Wire Debug (SWD) </p><p>- Firmware loading via ISP using UART (Commander or Serial terminal), SPI Interface</p><p>- Secure Over the Air (OTA) Upgrade</p><p>- Firmware update via Bootloader</p></td><td>Supported</td><td>Supported</td></tr>
<tr><td>Crypto Support</td><td><p>- Crypto API's for Hardware Accelerators: Advanced Encryption Standard (AES) 128/256/192, Secure Hash Algorithm (SHA) 256/384/512, Hash Message Authentication Code (HMAC), Random Number Generator (RNG), SHA3, AES-Galois Counter Mode (GCM)/ Cipher based Message Authentication Code (CMAC), ChaCha-poly, True Random Number Generator (TRNG)</p><p>- Software Accelerators: RSA, ECC</p><p>- PSA Crypto APIs support for all crypto operations.</p><p>- Wrapping Secret keys (Symmetric crypto). </p><p>- Added ECDSA Sign and Verify APIs</p></td><td>Supported</td><td>Supported</td></tr>
<tr><td>System Power Save</td><td><p>- Deep Sleep with RAM retention and without RAM retention. </p><p>- Wireless Power Save: Connected Sleep (Wi-Fi Standby Associated), BLE Advertising with powersave, BLE Scan with powersave,  BLE connection with powersave. Only Max PSP power save mode is supported in BLE. </p></td><td>Supported</td><td>Supported</td></tr>
<tr><td rowspan="13">Wi-Fi</td><td>Wi-Fi Protocols</td><td>IEEE 802.11 b/g/n/ax (2.4GHz)</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Access Point (AP) Mode</td><td><p>- 4 Client Support, Hidden SSID Mode, Auto Channel Selection, Scan in AP mode (Alpha)</p><p>- Wi-Fi Security </p><p>` `- WPA2 Personal, WPA3 Personal (H2E method only) (Alpha), WPA Mixed mode (WPA/WPA2) </p></td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi Scan</td><td>Selective Scan, Active/Passive Scan</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi STA (Security Modes)</td><td>Open Mode, WPA2 Personal, WPA2 Enhancements, WPA3 Personal, Mixed Mode (WPA/WPA2), WPA3 Personal Transition Mode (WPA2/WPA3)</td><td>Supported</td><td>Supported</td></tr>
<tr><td>WPA2 Enterprise security (STA)</td><td>Method: PEAP/TTLS/TLS 1.0/TLS 1.2/FAST/LEAP</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi STA Rejoin</td><td></td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi STA Roaming </td><td>BG Scan, OKC (Opportunistic Key caching), PMK (Pairwise Master Key) caching, Pre-Authentication</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi Protocol Power Save </td><td>Deep sleep (unconnected state), Max PSP, Enhanced Max PSP, Fast PSP, TWT</td><td>Supported</td><td>Supported</td></tr>
<tr><td>QoS</td><td>WMM-QoS</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi 6 Feature</td><td>MUMIMO (DL), OFDMA (UL/DL), iTWT, TWT I-Frame & TWT Enhancements (Automatic TWT Configuration), BSS coloring, MBSSID</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi Concurrency </td><td>AP+STA (Same channel)</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi Band/Channels</td><td>2\.4GHz CH1-11, 2.4GHz CH1-13, 2.4GHz CH1-14</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Known Security Vulnerabilities Handled</td><td>WPA2 KRACK Attacks, Fragment and Forge Vulnerability</td><td>Supported</td><td>Supported</td></tr>
<tr><td rowspan="6">Network stack</td><td rowspan="2">Core Networking Features</td><td><p>- IPv4/IPv6/UDP/TCP/ARP/ICMP/ICMPv6</p><p>- SSL client versions TLSV1.0, TLSV1.2, TLSV1.3 </p><p>- SSL server versions TLSV1.0 and TLSV1.2</p><p>- DHCPv4/DHCPv6 Client</p><p>- TCP/IP Bypass (LWIP as Hosted stack for reference)</p></td><td>Supported</td><td>Supported</td></tr>
<tr><td>- DHCPv4 Server, DHCPv6 Server</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td rowspan="2">Advanced Network Features</td><td>HTTP Client/HTTPS Client/DNS Client, Embedded MQTT/MQTT on host (AWS and AZURE) </td><td>Supported</td><td>Supported</td></tr>
<tr><td>SNTP Client, IGMP</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi IoT Cloud Integration</td><td><p>- AWS IOT Core</p><p>- Azure IoT</p></td><td>Supported</td><td>Supported</td></tr>
<tr><td>BSD and IoT sockets application programming interface(API)</td><td></td><td>Supported</td><td>Supported</td></tr>
<tr><td rowspan="2">BLE </td><td>Legacy features</td><td><p>- GAP(Advertising, Scanning, initiation, Connection and Bonding)</p><p>- Generic Attribute Protocol(GATT)</p><p>- Attribute protocol(ATT)</p><p>- Security</p><p>- LL Privacy 1.2</p><p>- Accept list</p><p>- Directed Advertising</p><p>- LE PHY(1Mbps, 2Mbps) & Coded PHY(125Kbps, 500kbps)</p><p>- Simultaneous scanning on 1Mbps and Coded PHY</p><p>- LE dual role topology</p><p>- LE data packet length extensions(DLE)</p><p>- Asymmetric PHYs</p><p>- LE channel selection algorithm 2 (CSA#2)</p><p>- LE Secure connections</p></td><td>Supported</td><td>Supported</td></tr>
<tr><td>Advertising Extensions </td><td><p>- Extended Advertising</p><p>- Periodic Advertising</p><p>- Periodic Advertising scanning</p><p>- Extended Advertising scanning</p><p>- Periodic Advertising list</p><p>- LE periodic advertising synchronization</p></td><td>Not Supported </td><td>Supported</td></tr>
</table>

### **MCU**

- **Memory**
  - Common Flash: Single shared Flash for both Cortex-M4 and NWP (Wireless Processor)
  - Common Flash + External PSRAM
- **Power States**
  - Active: PS4, PS3, PS2, and PS1 
  - Standby: PS4, PS3, and PS2
  - Sleep: PS4, PS3 and PS2
  - Deep Sleep(Shutdown): PS0
- **Peripherals**
  - High Performance (HP) - ADC, Calendar, Config Timer (CT), eFuse, EGPIO, GSPI, I2C, I2S, MCPWM, SDIO Secondary, SSI, SYSRTC, UART, uDMA, USART, WDT, Analog Comparator, DAC, BJT Temperature Sensors , OPAMP, QEI, RNG, (based on SQA feedback), QSPI GPDMA

PSRAM, CRC and RTC

- Ultra Low Power (ULP) - ULP\_ADC, ULP\_CALENDAR, ULP\_DAC, ULP\_GPIO, ULP\_I2C, ULP\_I2S, ULP\_TIMER, ULP\_UDMA, ULP\_UART, ULP\_SSI\_MASTER
- UC Supported Peripherals - CRC, EFUSE, EGPIO, SYSRTC,  BJT Temp sensor, I2C, Calendar, SPI, SSI, uDMA, UART/USART, WDT, Config Timers, ULP Timers, SDIO Secondary, PWM, ADC, DAC, I2S and Analog Comparator
- **Services**
  - Sleep Timer, IOSTREAM, Power Manager, SensorHub, NVM3, LittileFS, FreeRTOS Tickless-idle mode
  - The flash write feature has been enhanced to support the NWP area, providing a 20k allocation within the NWP flash memory for storing user data. Moreover, a Read API has been introduced to retrieve data from the NWP flash region.
- **Development Pro-Kit Peripheral Support**
  - LED, Button, MEMLCD, Joystick, RHT Sensor

### **Development Environment**

- Simplicity Studio IDE (SV5.9.2.0 version) and Debugger Integration. Refer to the latest version of the SoC "Getting-Started-with-SiWx917" guide for more details. 
- Recommended to install and use Silicon labs Simplicity SDK (Previously known as Gecko SDK), Git hub based version 2024.6.1.
- Simplicity Commander to supports Flash loading, provision of MBR programming, security key management, and calibration support for crystal and gain offsets. refer "siwx917-soc-manufacturing-utility-user-guide" for more details. 
- Advanced Energy Monitoring (AEM) to measure ultra-low power capability on Development boards (Pro Kit).
- PinTool Supported peripherals - I2C, GSPI, SSI, SIO, UART/USART, Config Timers, PWM, I2S, ADC, DAC and Analog comparator

### **BLE** 

- GAP(Advertising, Scanning, initiation, Connection and Bonding)
- Generic Attribute Protocol(GATT)
- Attribute protocol(ATT)
- Security
- LL Privacy 1.2
- Accept list
- Directed Advertising
- Extended Advertising
- Periodic Advertising
- Periodic Advertising scanning
- Extended Advertising scanning
- Periodic Advertising list
- LE periodic advertising synchronization
- LE PHY(1Mbps, 2Mbps) & Coded PHY(125Kbps, 500kbps)
- Simultaneous scanning on 1Mbps and Coded PHY
- LE dual role topology
- LE data packet length extensions(DLE)
- Asymmetric PHYs
- LE channel selection algorithm 2 (CSA#2)
- LE Secure connections
- Bluetooth 5.4 Qualified

### **SDK**

- Simplified and Unified DX for Wi-Fi API and Platform APIs
- Simplifies application development and presents clean and standardized APIs
- UC (Universal Configurator) enables componentization, simplifying configuration of peripherals and examples
- BSD and ARM IoT-compliant socket API
- Available through Simplicity Studio and GitHub

### **Multi-protocol**

- Wi-Fi STA + BLE

### **PTA/Coexistence**

- 3 wire coex acting as Wi-Fi with external Bluetooth
- 3 wire coex acting as Wi-Fi with external Zigbee/OT

## **Changes in this release compared to v3.3.1 Release**

### **System**

- **Enhancements / New features**
  - None
- **Fixed Issues**
  - None
- **Documentation**
  - None

### **MCU**

- **Enhancements / New features**
  - None
- **Fixed Issues**
  - None
- **Documentation**
  - Updated sl\_status\_t status code hyperlinks across all unified peripherals, hardware drivers, and service APIs
  - Updated documentation regarding flash loader error codes
  - Included supported stream information in the I/O Stream documentation
  - Added a disclaimer note for limiting usage of 32MHz RC clock for user applications.
  - Improved API documentation for all Platform peripherals, unified peripherals, hardware drivers, and services
  - Resolved readme concerns in sl\_si91x\_littlefs application and sl\_si91x\_power\_manager\_tickless\_idle

### **SDK**

- **Enhancements / New features**
  - Added Ozone configuration files.
- **Fixed Issues**
  - None
- **Documentation**
  - Enhanced description, limitations, exceptions of APIs, types, and constants across all modules of the API Reference Guide.
  - Replaced references of “TA” with “NWP” and “ThreadArch” with “Network Wireless Processor”
  - Updated readme with project creation steps and images for SiWG917 Dev Kit demo
  - Added steps to use Ozone debugger.

### **Wi-Fi/Network Stack**

- **Enhancements / New features**
  - None
- **Fixed Issues**
  - None

### **BLE**

- **Enhancements / New features**
  - None
- **Fixed Issues**
  - None
- **Documentation**
  - Updated BLE API documentation.
  - Removed unused BT parameters and updated BLE feature bitmap descriptions. 

### **Multi-protocol**

- **Enhancements / New features**
  - None
- **Fixed Issues**
  - None
- **Documentation**
  - None

## **Recommendations**

### **System**

- The current revision of SiWx917 has:
  - RAM memory of 672k bytes which can be shared between TA and M4 processors in SoC mode. 
  - The below configurations are applicable in SoC mode and can be configured based on the application requirement. EXT\_FEAT\_352K\_M4SS\_320K is the default configuration, based on requirement EXT\_FEAT\_480K\_M4SS\_192K configuration is selected for SoC mode multi-protocol examples.
    - EXT\_FEAT\_480K\_M4SS\_192K - This mode configures TA with 480k and M4 with 192K bytes of memory
    - EXT\_FEAT\_416K\_M4SS\_256K - This mode configures TA with 416k and M4 with 256K bytes of memory
    - EXT\_FEAT\_352K\_M4SS\_320K - This mode configures TA with 352k and M4 with 320K bytes of memory
  - SoC mode should not use 672k\_M4SS\_0K memory configuration.
- Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
- There are 2 Versions of Pro-Kits/Radio boards. Si917-6031A based on Si917-4338A (Rev **A01 - A11**) and SiWx917-6031A based on SiWx917-4338A (Rev A12-A14). To get optimal power numbers, enable macro "SL\_SI91X\_ENABLE\_LOWPWR\_RET\_LDO" pre-processor define for ICs or while using SiWx917-6031A Pro-kit, SiWx917-4338A version of boards. This macro should be disabled for earlier variants of the board (Si917-6031A, Si917-4338A).
- With RAM configuration (EXT\_FEAT\_352K\_M4SS\_320K), only 352K memory is available to TA  which limits the features supported, Recommended to enable EXT\_FEAT\_416K\_M4SS\_256K in Wi-Fi + BLE Multi protocol mode to enable more Network features.
- For EXT\_FEAT\_416K\_M4SS\_256K  and EXT\_FEAT\_480K\_M4SS\_192K memory configurations, it is recommended to retain both TA and M4 RAMs in power save.
- It is highly recommended to use connectivity firmware version 1711.2.12.2.1.0.9 with SDK v3.3.1 for improved stability in power save mode.

### **Wi-Fi/Network Stack**

- It is recommended to enable SL\_SI91X\_EXT\_TCP\_IP\_WAIT\_FOR\_SOCKET\_CLOSE BIT(16) of the 'Extended TCP IP Feature' bit map in the opermode command for all Wi-Fi Socket operations from the host to ensure graceful handling during asynchronous closures from the peer.
- For high throughput applications,  SL\_SI91X\_FEAT\_AGGREGATION BIT(2) of feature\_bit\_map is recommended to be enabled in opermode. 
- Users can enable SL\_SI91X\_EXT\_TCP\_IP\_SSL\_16K\_RECORD in 'Extended TCP IP Feature' bit map in opermode for (HTTPS server) supporting 16k record.
- **TWT**
  - Recommendation is to use sl\_wifi\_target\_wake\_time\_auto\_selection() API for all TWT applications. 
  - It is recommended to issue iTWT setup command once IP assignment, TCP connection, application specific socket connections are done.
  - When using sl\_wifi\_enable\_target\_wake\_time API, increase TCP / ARP Timeouts at the remote side depending upon the configured TWT interval configured. It's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() as an alternative.
  - In case of TWT in coex mode, when using sl\_wifi\_enable\_target\_wake\_time API, use TWT wake duration <= 16 ms and TWT wake interval >= 1 sec. If wake duration > 16 ms or TWT wake interval < 1sec, there might be performance issues.
  - For iTWT GTK interval in AP should be configured to max possible value or zero. If GTK interval is not configurable on AP side, recommended TWT interval (in case of sl\_wifi\_enable\_target\_wake\_time API) or RX Latency (in case of sl\_wifi\_target\_wake\_time\_auto\_selection API) is less than 4sec.
  - When sl\_wifi\_enable\_target\_wake\_time API is used, configuring TWT Wake interval beyond 1 min might lead to disconnections from the AP. Recommended to use TWT wake interval of less than or equal to 1 min.
  - When using sl\_wifi\_enable\_target\_wake\_time API, it is recommended to set missed\_beacon\_count of sl\_wifi\_set\_advanced\_client\_configuration API greater than 2 times of the configured TWT Interval.
  - DUT keepalive should be configured aligned with AP keepalive in TWT modes.
- Disable power save for high throughput applications or use FAST PSP power save mode as per application requirement.
- The application needs to ensure that it sets RTC with the correct timestamp before establishing the SSL/EAP connection.
- The minimum timeout value should not be less than 1 second for socket select and socket receive calls. 
- Please refer Keep alive intervals supported by MQTT broker and configure keep alive interval values accordingly.
- The minimum keep alive interval value recommended for embedded MQTT is 10 Seconds. 
- Disable power save and suspend any active TWT sessions before triggering HTTP OTAF.
- Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with the power save.
- Recommended to configure VAP\_ID properly for Si91x STA and AP using sl\_si91x\_setsockopt\_async(), in case of data transfer.
- Recommended to use valid length(<= 202 bytes) for topic to be published while using Embedded MQTT, else it leads to return wrong error code(0x21).
- In concurrent mode with dual IP, it is advised to bring up STA first (IP configuration) and AP later.
- It is recommended to configure Tx ,Rx , Global buffer pool ratio in the buffer config command based for all Wi-Fi Socket operations from the host
- It is recommended to use "TCP exponential backoff" configuration for congested channels.
- It is recommended is to disable broadcast filter during TCP connection to avoid ARP resolution issues
- To avoid IOP issues, it is recommended to disable power save before Wi-Fi connection.

### **BLE**

- In BLE, the recommended range of Connection Interval in
  - Power Save (BLE Only) - 100 ms to 1.28 s.
- In BLE, during Connection, the configuration of Scan Interval and Scan Window with the same value is not recommended. The suggested ratio of Scan Window to Scan Interval is 3:4.
- In BLE, if a device is acting as Central, the scan window (in set\_scan\_params and create\_connection commands) must be less than the existing Connection Interval. The suggested ratio of Scan Window to Connection Interval is 2:3.
- In BLE mode, if scanning and advertising are in progress on the SiWx91x module and it subsequently gets connected and moves to the central role, scanning stops else if it moves to the peripheral role, advertising stops. To further establish a connection to another peripheral device or to a central device, the application should give a command for starting advertising and scanning again.

### **Multi-protocol**

- For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
- Wi-Fi + BLE Advertising
  - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals may be observed.
  - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity.
- Wi-Fi + BLE scanning
  - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
  - BLE scanning will be stopped for intervals that collide with Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Connections
  - All standard connection intervals are supported.
  - For a stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Data Transfer
  - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout.
  - Ensure Wi-Fi activity consumes lower intervals.

### **MCU**

- It is not recommended to use switch\_m4\_frequency() for clock scaling. Refer to the migration guide for more details on this
- It is not recommended to use the RO clock for any Clock Configuration
- PSRAM examples are not supposed to be used with Non-PSRAM OPNs
- In tickless idle mode, SYSRTC and wake on wireless are configured as default wakeup sources. Recommended not to install these components explicitly
- It is recommended to enable DMA for supported peripherals to achieve better throughput
- For I2C Fast-Plus and High-speed modes, it is recommended to use High power instances (I2C0, I2C1) for core clocks operating between 80MHz and 180MHz
- For using GPIO-based wakeup, it is strongly recommended to install GPIO component in powersave applications
- It is recommended to use both CTS and RTS for UART flow control
- Powersave examples for the OPNs requires the installation of the device\_need\_ram\_execution component
- It is recommended to make a code change if the external oscillator connected to UULP\_GPIO\_3 is being used as the 32kHz clock source. Please refer to the SRM for code change.

## **Known Issues of WiSeConnect3\_SDK\_3.3.2 Release**

### **MCU**

**SensorHub**

- Power transitions with AWS are not stable in SensorHub
- Support for ADC FIFO Mode and Multi channel is open
- PS1-state supports only ADC sensor
- Tickless IDLE mode is not supported in sensor hub application

**SSI**

- SSI Dual and Quad mode are not functional as expected
- In ULP SSI application, non-dma transfer is not working as expected.

**I2C**

- ULP\_I2C in low power mode, when DMA is enabled will not function as expected

**I2S**

- FIFO threshold configurations are not working as expected

**Config Timers**

- PWM mode is not working as expected
- Config timer features to trigger DMA and interrupts on events or counters are not supported
- 32-bit mode is not working as expected

**UART**

- UART instances does not support different FIFO Thresholds
- Character Timeout feature is not supported
- UART does not support lower baud rates (110 and 115)
- Multi-drop RS485 interface is not supported

**ADC**

- ADC static mode sampling rate is supported up to 2.5Msps
- ADC should use deinit-init wakeup sequence after wakeup

**Analog comparator**

- Few configurable inputs (opamp2, opamp3 and BOD) are not working as expected

**Temperature Sensors**

- BJT Temperature readings are not accurate

**Power Manager**

- Few wakeup sources (BOD and Comparator) are not working as expected
- PS1-state is not supported

**Miscellaneous**

- Observed issues for flashing application/firmware binaries, if external oscillator connected to UULP\_GPIO\_3 is being used as the 32Khz clock source, a code change is required to prevent this issue.

### **SDK**

- Observed Wi-Fi connection is successful even after deleting the stored network credentials using sl\_net\_delete\_credential and responding with SL\_NET\_INVALID\_CREDENTIAL\_TYPE for sl\_net\_get\_credential.
- Enhanced sl\_wifi\_get\_firmware\_version() API to provide more details (ROM ID, chip ID, security version, etc) which is not backward compatible with firmware older than 1711.2.10.1.0.0.4. Firmware binary notation does not include the security version number.
- In PSRAM enabled demos, moving of text, data and stack segments to PSRAM is allowed. BSS and Heap should still be in SRAM.
- Asynchronous Azure MQTT is not supported, this will be addressed in upcoming release(s).
- mDNS with IPV6 is not supported.
- Bus thread stack may need to increase if local variables are used in user callback to avoid stack overflow.
- Low Power examples usage and documentation still under scope of improvement.
- Observed sl\_wifi\_get\_wireless\_info() API is giving wrong security type and PSK for WPA3 Transition supported client mode.
- Observed socket close is not working as expected for TLS socket when socket connect, send data and socket close are performing in a continuous loop.
- Observed issues with http\_otaf\_twt example.
- Observed data not received from last socket while sending from multiple TCP client sockets
- WMM-PS/UAPSD is not supported
- Application firmware\_flashing\_from\_host\_uart\_xmodem is not working as expected

### **Wi-Fi/Network Stack**

**Wi-Fi STA**

- STA Connection with the WPA3 Hunting and Pecking algorithm takes about 3-4 seconds.
- Observed connection failures with some AP's in more channel congestion ~50-60% occupancy open lab environment.
- Region selection based on country IE in the beacon is not supported
- Observed intermittent beacon reception from Access Point ( beacon misses) when channel congestion > 85%.
- While scanning with low power mode enable, sensitivity degradation of 3-6dB is observed. Due to this AP's that are at longer range might not be shown in scan results.
- Passive scan is failing when DUT is configured in world domain.

**Access Point (AP) Mode**

- Fixed rate configuration in AP mode using sl\_wifi\_set\_transmit\_rate API is not being set as expected. 

**WPA2 Enterprise security (STA)**

- Observed connection issue with configuring certificate key and programming 4096 bit key and SHA384/SHA512 certificates.

**Wi-Fi Concurrency (AP + STA in same channel)**

- Observed 3rd party STA association fail with 917 AP while 917 STA mode is connecting/reconnecting to configured 3rd party AP. Reconnect 3rd party STA to 917 AP in such scenarios. 

**OFDMA (UL/DL)**

- Less throughput observed in DL-OFDMA with some APs that enabled Low density parity check coding.

**MUMIMO (DL)**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Performance, Interop issues with MU MIMO with certain APs. 
- Less throughput observed in MU-MIMO with some APs that enabled  Low density parity check coding

**MU-MIMO (UL)**

- UL MU-MIMO is not supported.

**TWT**

- When sl\_wifi\_enable\_target\_wake\_time() API is used, occasional MQTT disconnections may be observed if TWT is configured with longer TWT intervals (>30secs) with embedded MQTT + TWT.  As an alternative, it's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() API, where these dependencies are internally handled.

**Wi-Fi STA Rejoin**

- Observed Scanning (probe request) in all channels instead of the channels configured in selective channel(channel\_bitmap\_2g4) during rejoin process. 

**IPv4/IPv6**

- IP change notification is not indicated to the application. 
- In concurrent mode with dual IP, if the STA starts after AP is up, the STA IP configuration may fail for DHCP stateless mode.
- In concurrent mode, data transfer using the Link-local address will always use the first IP interface created by the application.

**BSD Socket API**

- Every server socket created consumes a socket (maximum of 10 sockets supported) and every subsequent connection to server socket consumes an additional socket (from the same pool of 10 sockets), which limits the number of server connections supported.
- Observing issues with TCP retries when power save mode is enabled, especially when the module is in idle state.
- TCP maximum retry value is valid upto 31

**SSL Client/Server**

- Sometimes during SSL Handshake, ECC curve parameters generated are wrong, resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
- Secure SSL renegotiation is not supported in the Embedded Networking Stack

**HTTP Client/ HTTPS Client**

- Observed occasional HTTPS continuous download failures when power save is enabled. Recommended to disable it before performing HTTPS continuous downloads

**SNTP**

- Unable to get SNTP async events when Coex mode and power save are enabled 

**Throughputs & Performance**

- Wi-Fi alone throughput is about SDK 3.x (42Mbps). SDK refinements are in progress to further improve Wi-Fi Standalone and Coex Throughputs. 

**Wi-Fi IOT Cloud integration**

- **AWS IOT Core**
  - Observed AWS MQTT keepalive transmission is not happening at expected intervals with power save enabled.
  - Observing LAST\_WILL\_MESSAGE is random at every MQTT connection rather than the configured Message/Length
- **AZURE IOT Core**
  - Observed DUT after sending the data, its not sending the MQTT keep alive packet due to this Azure HUB closing the connection when power save is enabled.

**Wi-Fi Interoperability (IOP)**

- Observed disconnections with Amplifi (AFI-INS-R) AP with powersave enable
- TWT session is failing due to disconnections observed in DUT if rx\_latency is set to 55 seconds and receive data is also set to 55 seconds on MI Xiaomi RA72 and Tplink AX53 AP's
- Observed less throughput(~1Mb) while running TCP RX with Max\_PSP powersave with DLink 810 AP
- Observed interop issue (random disconnections) with few APs (EERO 6+, EERO PRO 6E, Cisco Catalyst 9120AXID)
- Disconnections observed with Netgear RAX120 AP in WPA3 Security

### **BLE**  

**DTM/PER**

- Recommend to limit BLE Tx Maximum power to 18 dBm.  Please don't use for 127 power\_index for BLE HP chain with this release.

**DLE**

- Removed the ble\_data\_length PSRAM example as it does not work with 121x and 141x OPN's. 

**Privacy** 

- DUT hang at the rsi\_bt\_get\_local\_device\_address API in central role, when using PSRAM boards (121x, 141x, 111M) and enabling power save in the ble\_privacy application,

**SMP**  

- SMP is not working with 110L(Lite wireless firmware image) board. 

### **Multi-protocol**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Wi-Fi + BLE intermittent connection failures, disconnections, and data transfer stalls in the long run when power save is enabled.
- Observed "DUT is not disconnecting to the AP when initiating disconnection from EFR connect app screen using wifi\_station\_ble\_provisioning\_aws example
- Observed DUT failed to load certificate with error "0x10026" (SL\_STATUS\_SI91X\_WRONG\_PARAMETERS) while running wifi\_https\_ble\_dual\_role\_v6 application
- Observed BLE bonding failure during continuous HTTPS download.
- Observed BLE and WLAN connection failure with SMP, when WLAN connect and HTTPS GET called in a loop.
- Observed issue in displaying throughput for interval of 'TEST\_TIMEOUT' when CONTINUOUS\_THROUGHPUT enabled in wifi\_station\_ble\_throughput\_app demo
- In Coex opermode, with memory config of TA - 480K and M4 - 192K Wi-Fi through is degrading by 10Mbps due less memory available for throughput.

### **System**

- Observed random hang issues with encrypted firmwares on some earlier variant of boards  (Si917-6031A, Si917-4338A) with powersave enable.
- This release addresses several issues, resulting in an additional 1K RAM usage in the NWP core. Consequently, this reduces the available heap size by 1K for the NWP core. Users with configurations that were already near the heap limit may experience either minor throughput issues OR functionality issues with this update.

On encountering a problem, it can be mitigated by considering the following options:

- Reduce the number of enabled features in the NWP core.
- Switch to a memory configuration that allocates more RAM to the NWP core while reducing RAM allocated to the Host processor.

### **Simplicity Studio and Commander** 

- Simplicity commander does not support options under "Debug Lock tools".
- All projects in the package are compatible with **GNU ARM V12.2.1** toolchain

## **Planned Roadmap Features**

### **SDK**

- WPS 2.0 PIN/PUSH, Network Manager Enhancements
- Websockets
- Matter integration with Embedded TCP/IP stack 
- Enhanced buffer management, Throughput optimization
- TLS over LWIP, Network layer applications (MQTT, HTTP, etc.) over LWIP.
- Debugging utilities, UC Configurator for Wireless configurations

### **System**

- Secure communication over CPC (UART & SDIO interfaces)

### **MCU**

- Dual Flash: Separate flash for Cortex M4 and Wireless Processor
- Peripherals: BOD, OPAMP, HSPI, GPDMA, QEI, RNG, IR decoder
- Software-configurable MCU application memory options (192 or 256 or 320 kB)
- RTOS support for MCU Peripheral examples
- SensorHub: Support for SDC\_ADC
- DMA flow control, self-loop mechanism and as a clock qualifier
- Support for I2S PCM functionality
- Power Manager service enhancements
- Pintool GA quality and support to any upcoming peripheral
- NVM3 application execution from PSRAM(text and data segments)

### **Wi-Fi/Network Stack**

- Spatial Re-Use, BSS Max Idle
- Provisioning using Wi-Fi AP

### **BLE**

- Support for 8 Peripheral + 2 Central connections 
- Support for BLE Mesh (4 nodes only) for limited Mesh use case

## **Limitations and Unsupported Features**  

### **System**

- None

### **SDK**

- Baremetal mode is not supported.

### **Wi-Fi/Network Stack**

- TLS 1.3 Server is not supported.
- 40 MHz bandwidth for 2.4 GHz band is not supported.
- Max 3 SSL sockets are supported in Wi-Fi alone and CoEx modes. No.of  SSL Sockets in Wi-Fi + BLE based on RAM memory configuration selected. 
- In SSL ECC Curve ID supported is 23. SSL handshake with 3rd party clients depends on the SSL ECC Curve ID.
- The number of Non-Transmitting BSSIDs processed is limited by the beacon length that can be processed by the stack (which is 1024 bytes). Beacons greater than 1024 Bytes in length will not be processed.
- Multiprotocol (STA +BLE) + EAP Security modes supported only with Memory configurations EXT\_FEAT\_416K\_M4SS\_256K and EXT\_FEAT\_480K\_M4SS\_192K.
- UL MUMIMO is not supported.
- WPA3 AP supports only H2E algorithm.
- PMKSA caching is not supported in WPA3 AP mode.
- Maximum embedded MQTT Publish payload is 1 kbyte.
- Timeout value for socket select and socket receive calls of less than 1 second is not currently supported.
- SA query procedure not supported in 11W AP mode.
- WPA3 AP transition mode is not supported.
- AP standalone mode does not support Tx aggregation. Rx aggregation is supported with limited number of BA sessions.
- In concurrent AP mode, aggregation (Tx/Rx) is not supported.
- Embedded HTTP Server is not supported.
- mDNS with IPV6 is not supported.
- Low power scan supports 1 mbps packets reception only.
- Auto PAC Provisioning in EAP-FAST with TLSv1.2 is not supported.

### **BLE**

- For BLE, if the connection is established with a small connection interval (less than 15 ms), simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported.
- BLE maximum two concurrent connections are supported, which can be either a connection to two peripheral devices, to one central and one peripheral device or two central devices.
- BLE Slave latency value is valid up to 32 only.
- Maximum supported AE data length is 200 bytes.
- Supports only two ADV\_EXT sets.
- Supports only two BLE connections (1 Central and 1 Peripheral) with AE.
- Advertising Extension feature is not supported in Coexistence.
- The  ae\_central & ae\_peripheral applications are not supported with TA\_352K\_M4\_320K RAM configuration.
- Two BLE connections are not supported with M4 powersave. It only supports a single connection. 
- Isochronous channels feature is not supported. 
- Connection subrating feature is not supported. 
- LE power controller feature is not supported.
- EATT feature is not supported.
- Periodic Advertising with a response feature is not supported. 
- BLE Audio is not supported.
- The feature of dynamically changing the TX power when extended advertising is active is not supported.
- EFR Connect mobile application doesn't have support to differentiate the BLE configurators based on the Bluetooth Device address.
- The maximum BLE power has been reduced by 2dB compared to the Datasheet Number, which will be addressed in the upcoming 3.3.1 patch release.

### **MCU**

- SensorHub supports PS1-power state with ADC sensor. In this mode, the other sensor's operation is not supported
- Manual chip select option is not supported when using GSPI
- Following peripherals/features are not functioning as intended
  - ADC Multichannel, SSI Dual and Quad mode, SIO and Temp sensors
- LittleFS support is intended to be used only with Dual flash
- PS1 state is not supported in Power Manager
- Power is same for PS4 and PS3 states with powersave mode of power manager
- When using Button\_0 as wakeup source, it limits the button functionality at application layer. For using Button\_0 for application specific functionality, enable 'SL\_SI91X\_NPSS\_GPIO\_BTN\_HANDLER'

### **Multi-protocol**

- Wi-Fi AP + BLE currently not supported. 
- EXT\_FEAT\_352K\_M4SS\_320K RAM configuration is not supported for coex mode with SSL

## **Removed/Deprecated Features**

- Hardware Reference Manual updates - Updated platform code and applications for the below peripherals to align with the latest HRM 
  - Removed Peripherals - SIO, RO temp sensor, FIM
  - Removed features - ADC (Multichannel, external event based sampling), UART/USART(lower baud rates110/150, bit width 1-4 and 9, IrDA support, Multi-drop RS485 support), DAC Reference voltage for ADC, Hardware Flow control for ULP UART
  - Removed USART\_RX\_CHAR\_TIMEOUT interrupt macro as the feature is not functional 
  - Removed Hardware setup API from all power save application that uses sllib\_m4\_power\_save.slcc component
  - Removed MVP support from SIWG917M110LGTBA component 
  - Removed hard-coded I2S0 channel value in rsi send and receive APIs

> **Note:** 
>
> The following BLE Synchronous API's will be deprecated soon and the equivalent Asynchronous API's will be used instead in all BLE applications :
>
> |**S.NO**|**BLE Synchronous API's** |**BLE Asynchronous API's** |
> | :- | :- | :- |
> |1|rsi\_ble\_get\_profiles|rsi\_ble\_get\_profiles\_async|
> |2|rsi\_ble\_get\_profile|rsi\_ble\_get\_profile\_async|
> |3|rsi\_ble\_get\_char\_services|rsi\_ble\_get\_char\_services\_async|
> |4|rsi\_ble\_get\_inc\_services|rsi\_ble\_get\_inc\_services\_async|
> |5|rsi\_ble\_get\_char\_value\_by\_uuid|rsi\_ble\_get\_char\_value\_by\_uuid\_async|
> |6|rsi\_ble\_get\_att\_descriptors|rsi\_ble\_get\_att\_descriptors\_async|
> |7|rsi\_ble\_get\_att\_value|rsi\_ble\_get\_att\_value\_async|
> |8|rsi\_ble\_get\_multiple\_att\_values|rsi\_ble\_get\_multiple\_att\_values\_async|
> |9|rsi\_ble\_get\_long\_att\_value|rsi\_ble\_get\_long\_att\_value\_async|
> |10|rsi\_ble\_set\_att\_value|rsi\_ble\_set\_att\_value\_async|
> |11|rsi\_ble\_set\_long\_att\_value|NA|
> |12|rsi\_ble\_prepare\_write|rsi\_ble\_prepare\_write\_async|
> |13|rsi\_ble\_execute\_write|rsi\_ble\_execute\_write\_async|
> |14|rsi\_ble\_indicate\_value\_sync|rsi\_ble\_indicate\_value|

<br>

# **WiSeConnect3\_SDK\_3.3.1 SoC Release Notes**

## **Release Details**

|**Item**|**Details**|
| :- | :- |
|Release date|14th August, 2024|
|SDK Version|3\.3.1|
|Firmware Version|<p>Standard: 1711.2.12.2.1.0.9</p><p>Lite: 1711.2.12.2.1.2.9</p>|
|GSDK/SiSDK Version|SiSDK 2024.6.1|
|Studio Version|5\.9.2.0|
|Release Package Name|WiSeConnect3\_SDK\_3.3.1|
|Supported RTOS|FreeRTOS|
|Operating Modes Supported|Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP|

- SiWx917 release consists of two components:
  - Standard Wireless Firmware - SiWx917 Firmware Binary available as SiWG917-B.2.12.2.1.0.9.rps
  - Lite Wireless Firmware - SiWx917 Firmware Binary is available as SiWG917-B.2.12.2.1.2.9.rps, this image is with reduced features for parts with SiWG917M110LGTBA OPN. 
  - Wiseconnect3 Library - Wiseconnect3 SDK library runs on internal Cortex M4

> **NOTE:** 
> 
> - Mandatory to upgrade the earlier version of boards (Si917-6031A Prokit or BRD4338A boards) or 917 Silicon ICs with instructions as outlined in this document "SiWG917–TA\_Flash\_Memory\_Map\_ChangeGuide\_v1.3.pdf" for more details.  
> - The latest releases will have bug-fixes, enhancements, and new features in both 'SDK' and 'Firmware'. Hence, it is recommended to update and use 'SDK' and 'Firmware' of same release package.
> - It is recommended to update TA image first followed by M4 image and ensure application compatibility with firmware before OTA

## **Supported Hardware**

|**Hardware**|**OPN**|
| :- | :- |
|Silicon (IC OPN)|<p>QFN OPNs: SiWG917M111MGTBA, SiWG917M100MGTBA, SIWG917M110LGTBA(lite image), SiWG917M111XGTBA, SiWG917M121XGTBA, SiWG917M141XGTBA</p><p>Module OPNs: SIWG917Y111MGNBA</p>|
|Development Kits|<p>Pro Kit: SiWx917-PK6031A, Si917-PK6031A. </p><p>(Pro Kit includes Mother board "Si-MB4002A" + Radio board)</p><p>Radio boards: SiWx917-RB4338A, SiWx91x-RB4342A, Early Access for new board BRD2605A</p><p>Module boards: SiW917Y-RB4343A(AC0)</p>|

## **Supported Features** 

<table><tr><th><b>Section</b></th><th><b>Sub-Section</b></th><th><b>Feature</b></th><th><p><b>Lite Wireless Firmware</b></p><p><b>(4MB flash OPN<br>OPN No: SiWG917M110LGTBA)</b></p></th><th><b>Standard Wireless Firmware (For other OPNs)</b></th></tr>
<tr><td rowspan="8">System</td><td rowspan="4">Operating modes</td><td>Wi-Fi STA (802.11ax, 802.11n)</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi 802.11n AP</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi STA (802.11ax, 802.11n) + BLE</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Security </td><td>Secure Boot, Secure Key storage and HW device identity with PUF, Secure Zone, Secure XIP (Execution in place) from flash, Secure Attestation, Anti Rollback, Debug Lock, Flash Protection</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Secure firmware upgrade options</td><td><p>- Firmware loading support by Commander Tool through Jlink Debugger. Jlink connected to Serial Wire Debug (SWD) </p><p>- Firmware loading via ISP using UART (Commander or Serial terminal), SPI Interface</p><p>- Secure Over the Air (OTA) Upgrade</p><p>- Firmware update via Bootloader</p></td><td>Supported</td><td>Supported</td></tr>
<tr><td>Crypto Support</td><td><p>- Crypto API's for Hardware Accelerators: Advanced Encryption Standard (AES) 128/256/192, Secure Hash Algorithm (SHA) 256/384/512, Hash Message Authentication Code (HMAC), Random Number Generator (RNG), SHA3, AES-Galois Counter Mode (GCM)/ Cipher based Message Authentication Code (CMAC), ChaCha-poly, True Random Number Generator (TRNG)</p><p>- Software Accelerators: RSA, ECC</p><p>- PSA Crypto APIs support for all crypto operations.</p><p>- Wrapping Secret keys (Symmetric crypto). </p><p>- Added ECDSA Sign and Verify APIs</p></td><td>Supported</td><td>Supported</td></tr>
<tr><td>System Power Save</td><td><p>- Deep Sleep with RAM retention and without RAM retention. </p><p>- Wireless Power Save: Connected Sleep (Wi-Fi Standby Associated), BLE Advertising with powersave, BLE Scan with powersave,  BLE connection with powersave. Only Max PSP power save mode is supported in BLE. </p></td><td>Supported</td><td>Supported</td></tr>
<tr><td rowspan="13">Wi-Fi</td><td>Wi-Fi Protocols</td><td>IEEE 802.11 b/g/n/ax (2.4GHz)</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Access Point (AP) Mode</td><td><p>- 4 Client Support, Hidden SSID Mode, Auto Channel Selection, Scan in AP mode (Alpha)</p><p>- Wi-Fi Security </p><p>&emsp;- WPA2 Personal, WPA3 Personal (H2E method only) (Alpha), WPA Mixed mode (WPA/WPA2) </p></td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi Scan</td><td>Selective Scan, Active/Passive Scan</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi STA (Security Modes)</td><td>Open Mode, WPA2 Personal, WPA2 Enhancements, WPA3 Personal, Mixed Mode (WPA/WPA2), WPA3 Personal Transition Mode (WPA2/WPA3)</td><td>Supported</td><td>Supported</td></tr>
<tr><td>WPA2 Enterprise security (STA)</td><td>Method: PEAP/TTLS/TLS 1.0/TLS 1.2/FAST/LEAP</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi STA Rejoin</td><td></td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi STA Roaming </td><td>BG Scan, OKC (Opportunistic Key caching), PMK (Pairwise Master Key) caching, Pre-Authentication</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi Protocol Power Save </td><td>Deep sleep (unconnected state), Max PSP, Enhanced Max PSP, Fast PSP, TWT</td><td>Supported</td><td>Supported</td></tr>
<tr><td>QoS</td><td>WMM-QoS</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi 6 Feature</td><td>MUMIMO (DL), OFDMA (UL/DL), iTWT, TWT I-Frame & TWT Enhancements (Automatic TWT Configuration), BSS coloring, MBSSID</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi Concurrency </td><td>AP+STA (Same channel)</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi Band/Channels</td><td>2\.4GHz CH1-11, 2.4GHz CH1-13, 2.4GHz CH1-14</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Known Security Vulnerabilities Handled</td><td>WPA2 KRACK Attacks, Fragment and Forge Vulnerability</td><td>Supported</td><td>Supported</td></tr>
<tr><td rowspan="6">Network stack</td><td rowspan="2">Core Networking Features</td><td><p>- IPv4/IPv6/UDP/TCP/ARP/ICMP/ICMPv6</p><p>- SSL client versions TLSV1.0, TLSV1.2, TLSV1.3 </p><p>- SSL server versions TLSV1.0 and TLSV1.2</p><p>- DHCPv4/DHCPv6 Client</p><p>- TCP/IP Bypass (LWIP as Hosted stack for reference)</p></td><td>Supported</td><td>Supported</td></tr>
<tr><td>- DHCPv4 Server, DHCPv6 Server</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td rowspan="2">Advanced Network Features</td><td>HTTP Client/HTTPS Client/DNS Client, Embedded MQTT/MQTT on host (AWS and AZURE) </td><td>Supported</td><td>Supported</td></tr>
<tr><td>SNTP Client, IGMP</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi IoT Cloud Integration</td><td><p>- AWS IOT Core</p><p>- Azure IoT</p></td><td>Supported</td><td>Supported</td></tr>
<tr><td>BSD and IoT sockets application programming interface(API)</td><td></td><td>Supported</td><td>Supported</td></tr>
<tr><td rowspan="2">BLE </td><td>Legacy features</td><td><p>- GAP(Advertising, Scanning, initiation, Connection and Bonding)</p><p>- Generic Attribute Protocol(GATT)</p><p>- Attribute protocol(ATT)</p><p>- Security</p><p>- LL Privacy 1.2</p><p>- Accept list</p><p>- Directed Advertising</p><p>- LE PHY(1Mbps, 2Mbps) & Coded PHY(125Kbps, 500kbps)</p><p>- Simultaneous scanning on 1Mbps and Coded PHY</p><p>- LE dual role topology</p><p>- LE data packet length extensions(DLE)</p><p>- Asymmetric PHYs</p><p>- LE channel selection algorithm 2 (CSA#2)</p><p>- LE Secure connections</p></td><td>Supported</td><td>Supported</td></tr>
<tr><td>Advertising Extensions </td><td><p>- Extended Advertising</p><p>- Periodic Advertising</p><p>- Periodic Advertising scanning</p><p>- Extended Advertising scanning</p><p>- Periodic Advertising list</p><p>- LE periodic advertising synchronization</p></td><td>Not Supported </td><td>Supported</td></tr>
</table>

### **MCU**

- **Memory**
  - Common Flash: Single shared Flash for both Cortex-M4 and NWP (Wireless Processor)
  - Common Flash + External PSRAM
- **Power States**
  - Active: PS4, PS3, PS2, and PS1 
  - Standby: PS4, PS3, and PS2
  - Sleep: PS4, PS3 and PS2
  - Deep Sleep(Shutdown): PS0
- **Peripherals**
  - High Performance (HP) - ADC, Calendar, Config Timer (CT), eFuse, EGPIO, GSPI, I2C, I2S, MCPWM, SDIO Secondary, SSI, SYSRTC, UART, uDMA, USART, WDT, Analog Comparator, DAC, BJT Temperature Sensors , OPAMP, QEI, RNG, (based on SQA feedback), QSPI GPDMA

PSRAM, CRC and RTC

- Ultra Low Power (ULP) - ULP\_ADC, ULP\_CALENDAR, ULP\_DAC, ULP\_GPIO, ULP\_I2C, ULP\_I2S, ULP\_TIMER, ULP\_UDMA, ULP\_UART, ULP\_SSI\_MASTER
- UC Supported Peripherals - CRC, EFUSE, EGPIO, SYSRTC,  BJT Temp sensor, I2C, Calendar, SPI, SSI, uDMA, UART/USART, WDT, Config Timers, ULP Timers, SDIO Secondary, PWM, ADC, DAC, I2S and Analog Comparator
- **Services**
  - Sleep Timer, IOSTREAM, Power Manager, SensorHub, NVM3, LittileFS, FreeRTOS Tickless-idle mode
  - The flash write feature has been enhanced to support the NWP area, providing a 20k allocation within the NWP flash memory for storing user data. Moreover, a Read API has been introduced to retrieve data from the NWP flash region.
- **Development Pro-Kit Peripheral Support**
  - LED, Button, MEMLCD, Joystick, RHT Sensor

### **Development Environment**

- Simplicity Studio IDE (SV5.9.2.0 version) and Debugger Integration. Refer to the latest version of the SoC "Getting-Started-with-SiWx917" guide for more details. 
- Recommended to install and use Silicon labs Simplicity SDK (Previously known as Gecko SDK), Git hub based version 2024.6.1.
- Simplicity Commander to supports Flash loading, provision of MBR programming, security key management, and calibration support for crystal and gain offsets. refer "siwx917-soc-manufacturing-utility-user-guide" for more details. 
- Advanced Energy Monitoring (AEM) to measure ultra-low power capability on Development boards (Pro Kit).
- PinTool Supported peripherals - I2C, GSPI, SSI, SIO, UART/USART, Config Timers, PWM, I2S, ADC, DAC and Analog comparator

### **BLE** 

- GAP(Advertising, Scanning, initiation, Connection and Bonding)
- Generic Attribute Protocol(GATT)
- Attribute protocol(ATT)
- Security
- LL Privacy 1.2
- Accept list
- Directed Advertising
- Extended Advertising
- Periodic Advertising
- Periodic Advertising scanning
- Extended Advertising scanning
- Periodic Advertising list
- LE periodic advertising synchronization
- LE PHY(1Mbps, 2Mbps) & Coded PHY(125Kbps, 500kbps)
- Simultaneous scanning on 1Mbps and Coded PHY
- LE dual role topology
- LE data packet length extensions(DLE)
- Asymmetric PHYs
- LE channel selection algorithm 2 (CSA#2)
- LE Secure connections
- Bluetooth 5.4 Qualified

### **SDK**

- Simplified and Unified DX for Wi-Fi API and Platform APIs
- Simplifies application development and presents clean and standardized APIs
- UC (Universal Configurator) enables componentization, simplifying configuration of peripherals and examples
- BSD and ARM IoT-compliant socket API
- Available through Simplicity Studio and GitHub

### **Multi-protocol**

- Wi-Fi STA + BLE

### **PTA/Coexistence**

- 3 wire coex acting as Wi-Fi with external Bluetooth
- 3 wire coex acting as Wi-Fi with external Zigbee/OT

## **Changes in this release compared to v3.3.0 Release**

### **System**

- **Enhancements / New features**
  - Implemented framework for XTAL usage on M4
  - Implemented a system restart feature to facilitate automatic restarts in the event of unexpected faults or lock-ups in the field. It is recommended to enable this feature only during mass production or rollout.
  - Fixed issue observed during erasing of NWP flash region.
  - Implemented the changes to improve power number across sleep wakeups.
  - Addressed the 25% increase in system-level power consumption observed in the 3.3.0 release
  - Improved performance in system-level power save based applications
- **Enhancements / New features**
  - Added provision for multiple callbacks for each instance in SSI 
  - Implemented is\_okay\_to\_sleep API in power manager
  - Added Separate linker file for PSRAM and NONPSRAM examples
  - Enabled different wakeup configurations in power manager 
  - Added Pin tool support for SiWG917 OPNs
  - Added back and resolved the ULP SSI example issue
  - Added new API for SOC reset
  - Replaced BTN1 with BTN0 for out\_of\_box\_demo application
  - Enabled RC clock for the module boards
  - Clock configuration are not supposed to be in driver, so removed clock trimming from ADC driver
  - Accelerometer and gyroscope register have been modified to the correct register
  - Updated Software Reference Manual about front end switch pins configuration usage 
  - Updated Max data size in NVM3 dual flash application
  - Added front switch control feature in wifi structure in the application

- **Fixed Issues**
  - Resolved SWD/JTAG pins configuration as GPIO
  - Battery voltage failure issue during power-down of dependency components was resolved by removing the sl_si91x_hardware_setup() API.
  - The PAD Configuration address in the HRM and Peripherals tab of Studio has been corrected
  - Resolved application build errors while placing BSS, Data, Stack components in PSRAM 
  - Resolved M4 sleep issue with ULP\_GPIO\_BASED\_WKP wakeup and low polarity(NPSS\_GPIO\_INTR\_LOW) 
  - Fixed issues with VEML6035 example 
  - Resolved issue with sl\_gpio\_set\_configuration API, when using pins from port 2 and port 3 
  - Fixed issue with ULP\_GPIO group interrupt in sl\_si91x\_gpio\_group example
  - Removed unnecessary code in ULP IRQ's 
  - Changed USART and UART clock source from INTFPLL to ULPREF clock to resolve custom board print issue
  - Resolved data transmission issue while configuring the pins using pintool file in USART0 and UART1 examples 
  - Resolved UULP GPIOs wakeup issue in Wi-Fi Client BLE Provisioning with AWS
  - Resolved BTN1 issue in SiWG917 Dev Kit (BRD2605A)
  - Fixed incorrect RAM bank configuration in sl\_si91x\_configure\_ram\_retention API
  - The issue where LCD and LED0 could not be used simultaneously has been resolved
  - Removed reconfiguration of Ram retention in Application to configure different RAM retentions
  - Fixed logic error in accelerometer sensor 
  - Modified the ulp timer logic in combo application 
  - Fixed Soft reset issue with M4 firmware update application
  - Resolved Applications build errors with SIWG917Y111MGNBA OPN

### **SDK**

- **Enhancements / New features**
  - Added support for both IPv4 and IPv6 config in sl\_net\_up()
  - Added TCP and UDP data transfer for WLAN concurrent mode example
  - Added I2c changes back in wifi\_station\_ble\_provisioning\_aws example
  - Added sl\_si91x\_get\_firmware\_version and sl\_si91x\_get\_firmware\_size API's
  - Refactored LWIP porting layer and examples
  - Added SiWG917 Dev Kit (BRD2605A) example
  - Added Embedded Sensor demo tiles for SiWG917 dev kit board
  - Renamed "Additional Status Codes" as "WiSeConnect Status Codes" in documentation
  - Added support in the Azure IoT example to publish messages continuously
  - Added support for static ip address in LWIP port
  - Added tickless idle powersave for wifi\_azure\_iot\_soc example
  - Added tickless idle in TCP\_Tx\_periodic\_SoC example, moved examples from si91x\_soc/wlan to snippet folder
  - Added azureiotconfig KEEP\_ALIVE\_TIMEOUT\_SECONDS in preprocessor settings
  - Updated SL\_SI91X\_EXT\_FEAT\_XTAL\_CLK\_ENABLE for module and related boards e.g. BRD4343
  - Added tiles for Out of Box demo , CLI demo
  - Added new API sl\_net\_set\_dns\_server()
  - Added documentation changes for sl\_net\_get\_credential API.
  - Added new low power example powersave\_standby\_associated\_tcp\_client
  - Added the missing implementation for LOAD\_DEFAULT\_NWP\_FW\_ACTIVE\_LOW
  - Added Wi-Fi Coex - Wi-Fi Client HTTPS BLE Dual Role Readme file corrections
  - Added readme sections which are not clear for NCP or SOC
  - Added new API sl\_si91x\_is\_device\_initialized
  - Moved the applications to examples/featured/low\_power folder - power\_save\_deep\_sleep, twt\_tcp\_client
  - Updated all power save examples with Enhanced Max PSP feature

- **Fixed Issues**
  - Updated readme files for power\_save\_deep\_sleep, twt\_tcp\_client, powersave\_standby\_associated applications
  - Removed the unused files from sl\_net/utilities folder
  - updated the description of the output parameter "fw\_image\_size" in sl\_wifi\_get\_firmware\_size API
  - Updated image in readme changes in power\_save\_deep\_sleep application
  - Enhanced the clarity of several API descriptions for improved comprehension
  - Fixed issue with simultaneous operation of accept and recv
  - Fixed stack overflow of ble\_central task by increasing stack size in wifi\_https\_ble\_dual\_role\_v6 Application
  - Updated documentation with missing error codes
  - Updated TWT TCP client app and Readme documentation
  - Added support to the SDK to recover the module from a stuck state caused by the "valid firmware not present" error.

### **Wi-Fi/Network Stack**

- **Enhancements / New features**
  - Reduced Tx power variation across the channels 
  - Updated WLAN 11B temperature compensation Lookup table for modules
  - Added support for optional Customer Gain offset on Channel 14
  - Added support for Quick Scan Feature
  - Updated MQTT Documentation with topic length, password and username
  - Added support for IPV6 in MQTT
  - Updated data\_callback signature for async sockets
  - Added remote terminate support for fixing the Socket reconnection delay issue in twt\_use\_case\_remote\_app demo
  - Added certificate index and TLS configuration support for MQTTS
  - Enhanced documentation for sl\_wifi\_init and sl\_wifi\_deinit functions
  - Updated mDNS documentation
  - Update Doxygen documentation for SI91X\_TCP\_IP\_FEATURE\_BITMAP macro definitions
  - Added Aggregation and WPS disable bits to improve AP mode throughput 
  - Added front end switch bitmap in boot config for access point example
  - Updated ap\_throughput demo README
  - Added support for TCP\_ACK\_INDICATION call back feature 
  - Added socket\_bitmap to update the TCP\_ACK\_INDICATION bitmap
  - Added missing certificate index handling in http\_otaf application
  - Increased broadcast wait time from 14ms to 70ms and beacon wait time from 10ms to 25ms during powersave if broadcast filter is disabled.
- **Fixed Issues**
  - Added changes to control AP mode ACS survey based on the channel list in user provided region.
  - Fixed the issue observed with scanning in AP mode. 
  - The minimum supported beacon missed count value has been updated to 40.
  - Fixed wrong values for wlan\_state and channel\_number in sl\_wifi\_get\_wireless\_info structure
  - Fixed the publish failure issue in AWS\_MQTT example
  - Fixed Issue in multiple sockets with accept calls
  - Fixed hang issue when WLAN disconnection occurs while HTTP post is in progress
  - To maintain  consistency and uniformity with other socket  APIs ,  recvfrom() call in sl\_si91x\_recv() has been replaced with sl\_si91x\_recvfrom()
  - Fixed documentation issues in sl\_si91x\_http\_otaf API
  - Fixed issue with sl\_si91x\_select API when callback is NULL
  - Modified the http\_client application to give failure response when HTTP(s) GET requested file is not present on the server
  - Fixed MQTT connection failure with error code 0x10065 (SL\_STATUS\_SI91X\_ERROR\_WITH\_MQTT\_COMMAND)
  - Fixed an issue where the MQTT publish API failed to return an error when there was no internet connection at the AP (no MQTT cloud  connection)
  - Fixed issues with MQTT state machine in case of Wi-Fi disconnection
  - Fixed issue with sl\_si91x\_select API when configure socket\_select timeout set to '0'
  - Fixed issue with HTTPS download which is getting terminated with wifi\_https\_ble\_dual\_role\_v6 application
  - Fixed sl\_wifi\_init() failure with status 0x7
  - Fixed issue with MSS(Maximum Segment Size) error handling mechanism
  - Fixed the issue with client\_socket\_id when callback is NULL for accept\_async API
  - Implemented periodic TCP retries for HTTP sockets instead of exponential backoff.
  - Added configuration settings for the certificate index and TLS version in the MQTTs socket.
  - Resolved the SSL connection issues with  TLS13\_CHACHA20\_POLY1305\_SHA256 cipher
  - Resolved issues with the socket shutdown when running socket create->data\_trasfer→shutdown in a loop
  - Resolved instability issues with the DHCP server in IPv6.
  - Fixed data transfer issues on the link-local address when a unicast global IPv6 address is not configured.
  - Addressed instability issues with concurrent mode Dual IP.
  - Resolved an issue in AP mode where probe responses/beacons were incorrectly advertising WPA IE alongside RSN IE.
  - Added support for channel 14 in PER mode.
  - Fixed issue where Wrap key functionality gives incorrect output for 192-bit keys in AES and GCM.
  - Fixed LP chain DC calibration issue.

### **BLE**

- **Enhancements / New features**
  - Added a few additional details to the readme of "ble\_ae\_peripheral", ble\_ae\_central, ble\_central, ble\_heart\_rate\_profile, ble\_longrange\_2mbps and ble\_power\_save demos
  - Added support for permanently setting the customized BT MAC address in the SoC board.
  - Improved margin on emissions in FCC region accounting part-to-part variation with BLE High performance chain. 
  - Improved transmit power vs power index linearity in BLE High Performance chain
  - Added power manager tickless changes for PSRAM examples
- **Fixed Issues**
  - Resolved UART interface issue with bt\_stack\_bypass demo.
  - Resolved issue where the device stopped transmitting AE advertising packets after sometime , when configured 'Two ADV sets one handle with public address and another with Random address and Primary PHY =1M and Sec PHY =1M/2M". 
  - Resolved issue where the device was not transmitting AUX\_SCAN\_REQ packets to a remote device configured in scannable extended advertising mode.
  - Fixed the issue where device was unable to receive periodic advertising reports, when  "BLE\_AE\_PERODIC\_DUPLICATE\_FILTERING\_ENABLED" bit is enabled in AE option of the AE Periodic create sync API.
  - Fixed the issue where device was unable to receive periodic advertising reports, when  "BLE\_AE\_PERIODIC\_LIST\_USED" bit is enabled in the AE option of the AE Periodic create sync API.
  - Addressed the issue of high BLE power numbers. As a result, the power numbers now match the values specified in the data sheet.
  - Fixed BLE HP transmit power to power index mapping

### **Multi-protocol**

- **Enhancements / New features**
  - Implemented 5msec timeout for NULL DATA packet to reduce radio switch delay in dense environment.
- **Fixed Issues**
  - Fixed issue with DTIM beacon  time computation during coex without powersave , while releasing radio without receiving first beacon.

## **Recommendations**

### **System**

- The current revision of SiWx917 has:
  - RAM memory of 672k bytes which can be shared between TA and M4 processors in SoC mode. 
  - The below configurations are applicable in SoC mode and can be configured based on the application requirement. EXT\_FEAT\_352K\_M4SS\_320K is the default configuration, based on requirement EXT\_FEAT\_480K\_M4SS\_192K configuration is selected for SoC mode multi-protocol examples.
    - EXT\_FEAT\_480K\_M4SS\_192K - This mode configures TA with 480k and M4 with 192K bytes of memory
    - EXT\_FEAT\_416K\_M4SS\_256K - This mode configures TA with 416k and M4 with 256K bytes of memory
    - EXT\_FEAT\_352K\_M4SS\_320K - This mode configures TA with 352k and M4 with 320K bytes of memory
  - SoC mode should not use 672k\_M4SS\_0K memory configuration.
- Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
- There are 2 Versions of Pro-Kits/Radio boards. Si917-6031A based on Si917-4338A (Rev **A01 - A11**) and SiWx917-6031A based on SiWx917-4338A (Rev A12-A14). To get optimal power numbers, enable macro "SL\_SI91X\_ENABLE\_LOWPWR\_RET\_LDO" pre-processor define for ICs or while using SiWx917-6031A Pro-kit, SiWx917-4338A version of boards. This macro should be disabled for earlier variants of the board (Si917-6031A, Si917-4338A).
- With RAM configuration (EXT\_FEAT\_352K\_M4SS\_320K), only 352K memory is available to TA  which limits the features supported, Recommended to enable EXT\_FEAT\_416K\_M4SS\_256K in Wi-Fi + BLE Multi protocol mode to enable more Network features.
- For EXT\_FEAT\_416K\_M4SS\_256K  and EXT\_FEAT\_480K\_M4SS\_192K memory configurations, it is recommended to retain both TA and M4 RAMs in power save.
- It is highly recommended to use connectivity firmware version 1711.2.12.2.1.0.9 with SDK v3.3.1 for improved stability in power save mode.

### **Wi-Fi/Network Stack**

- It is recommended to enable SL\_SI91X\_EXT\_TCP\_IP\_WAIT\_FOR\_SOCKET\_CLOSE BIT(16) of the 'Extended TCP IP Feature' bit map in the opermode command for all Wi-Fi Socket operations from the host to ensure graceful handling during asynchronous closures from the peer.
- For high throughput applications,  SL\_SI91X\_FEAT\_AGGREGATION BIT(2) of feature\_bit\_map is recommended to be enabled in opermode. 
- Users can enable SL\_SI91X\_EXT\_TCP\_IP\_SSL\_16K\_RECORD in 'Extended TCP IP Feature' bit map in opermode for (HTTPS server) supporting 16k record.
- **TWT**
  - Recommendation is to use sl\_wifi\_target\_wake\_time\_auto\_selection() API for all TWT applications. 
  - It is recommended to issue iTWT setup command once IP assignment, TCP connection, application specific socket connections are done.
  - When using sl\_wifi\_enable\_target\_wake\_time API, increase TCP / ARP Timeouts at the remote side depending upon the configured TWT interval configured. It's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() as an alternative.
  - In case of TWT in coex mode, when using sl\_wifi\_enable\_target\_wake\_time API, use TWT wake duration <= 16 ms and TWT wake interval >= 1 sec. If wake duration > 16 ms or TWT wake interval < 1sec, there might be performance issues.
  - For iTWT GTK interval in AP should be configured to max possible value or zero. If GTK interval is not configurable on AP side, recommended TWT interval (in case of sl\_wifi\_enable\_target\_wake\_time API) or RX Latency (in case of sl\_wifi\_target\_wake\_time\_auto\_selection API) is less than 4sec.
  - When sl\_wifi\_enable\_target\_wake\_time API is used, configuring TWT Wake interval beyond 1 min might lead to disconnections from the AP. Recommended to use TWT wake interval of less than or equal to 1 min.
  - When using sl\_wifi\_enable\_target\_wake\_time API, it is recommended to set missed\_beacon\_count of sl\_wifi\_set\_advanced\_client\_configuration API greater than 2 times of the configured TWT Interval.
  - DUT keepalive should be configured aligned with AP keepalive in TWT modes.
- Disable power save for high throughput applications or use FAST PSP power save mode as per application requirement.
- The application needs to ensure that it sets RTC with the correct timestamp before establishing the SSL/EAP connection.
- The minimum timeout value should not be less than 1 second for socket select and socket receive calls. 
- Please refer Keep alive intervals supported by MQTT broker and configure keep alive interval values accordingly.
- The minimum keep alive interval value recommended for embedded MQTT is 10 Seconds. 
- Disable power save and suspend any active TWT sessions before triggering HTTP OTAF.
- Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with the power save.
- Recommended to configure VAP\_ID properly for Si91x STA and AP using sl\_si91x\_setsockopt\_async(), in case of data transfer.
- Recommended to use valid length(<= 202 bytes) for topic to be published while using Embedded MQTT, else it leads to return wrong error code(0x21).
- In concurrent mode with dual IP, it is advised to bring up STA first (IP configuration) and AP later.
- It is recommended to configure Tx ,Rx , Global buffer pool ratio in the buffer config command based for all Wi-Fi Socket operations from the host
- It is recommended to use "TCP exponential backoff" configuration for congested channels.
- It is recommended is to disable broadcast filter during TCP connection to avoid ARP resolution issues
- To avoid IOP issues, it is recommended to disable power save before Wi-Fi connection.

### **BLE**

- In BLE, the recommended range of Connection Interval in
  - Power Save (BLE Only) - 100 ms to 1.28 s.
- In BLE, during Connection, the configuration of Scan Interval and Scan Window with the same value is not recommended. The suggested ratio of Scan Window to Scan Interval is 3:4.
- In BLE, if a device is acting as Central, the scan window (in set\_scan\_params and create\_connection commands) must be less than the existing Connection Interval. The suggested ratio of Scan Window to Connection Interval is 2:3.
- In BLE mode, if scanning and advertising are in progress on the SiWx91x module and it subsequently gets connected and moves to the central role, scanning stops else if it moves to the peripheral role, advertising stops. To further establish a connection to another peripheral device or to a central device, the application should give a command for starting advertising and scanning again.

### **Multi-protocol**

- For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
- Wi-Fi + BLE Advertising
  - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals may be observed.
  - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity.
- Wi-Fi + BLE scanning
  - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
  - BLE scanning will be stopped for intervals that collide with Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Connections
  - All standard connection intervals are supported.
  - For a stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Data Transfer
  - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout.
  - Ensure Wi-Fi activity consumes lower intervals.

### **MCU**

- It is not recommended to use switch\_m4\_frequency() for clock scaling. Refer to the migration guide for more details on this
- It is not recommended to use the RO clock for any Clock Configuration
- PSRAM examples are not supposed to be used with Non-PSRAM OPNs
- In tickless idle mode, SYSRTC and wake on wireless are configured as default wakeup sources. Recommended not to install these components explicitly
- It is recommended to enable DMA for supported peripherals to achieve better throughput
- For I2C Fast-Plus and High-speed modes, it is recommended to use High power instances (I2C0, I2C1) for core clocks operating between 80MHz and 180MHz
- For using GPIO-based wakeup, it is strongly recommended to install GPIO component in powersave applications
- It is recommended to use both CTS and RTS for UART flow control
- Powersave examples for the OPNs requires the installation of the device\_need\_ram\_execution component
- It is recommended to make a code change if the external oscillator connected to UULP\_GPIO\_3 is being used as the 32kHz clock source. Please refer to the SRM for code change.

## **Known Issues of WiSeConnect3\_SDK\_3.3.1 Release**

### **MCU**

**SensorHub**

- Power transitions with AWS are not stable in SensorHub
- Support for ADC FIFO Mode and Multi channel is open
- PS1-state supports only ADC sensor
- Tickless IDLE mode is not supported in sensor hub application

**SSI**

- SSI Dual and Quad mode are not functional as expected
- In ULP SSI application, non-dma transfer is not working as expected.

**I2C**

- ULP\_I2C in low power mode, when DMA is enabled will not function as expected

**I2S**

- FIFO threshold configurations are not working as expected

**Config Timers**

- PWM mode is not working as expected
- Config timer features to trigger DMA and interrupts on events or counters are not supported
- 32-bit mode is not working as expected

**UART**

- UART instances does not support different FIFO Thresholds
- Character Timeout feature is not supported
- UART does not support lower baud rates (110 and 115)
- Multi-drop RS485 interface is not supported

**ADC**

- ADC static mode sampling rate is supported up to 2.5Msps
- ADC should use deinit-init wakeup sequence after wakeup

**Analog comparator**

- Few configurable inputs (opamp2, opamp3 and BOD) are not working as expected

**Temperature Sensors**

- BJT Temperature readings are not accurate

**Power Manager**

- Few wakeup sources (BOD and Comparator) are not working as expected
- PS1-state is not supported

**Miscellaneous**

- Observed issues for flashing application/firmware binaries, if external oscillator connected to UULP\_GPIO\_3 is being used as the 32Khz clock source, a code change is required to prevent this issue.

### **SDK**

- Observed Wi-Fi connection is successful even after deleting the stored network credentials using sl\_net\_delete\_credential and responding with SL\_NET\_INVALID\_CREDENTIAL\_TYPE for sl\_net\_get\_credential.
- Enhanced sl\_wifi\_get\_firmware\_version() API to provide more details (ROM ID, chip ID, security version, etc) which is not backward compatible with firmware older than 1711.2.10.1.0.0.4. Firmware binary notation does not include the security version number.
- In PSRAM enabled demos, moving of text, data and stack segments to PSRAM is allowed. BSS and Heap should still be in SRAM.
- Asynchronous Azure MQTT is not supported, this will be addressed in upcoming release(s).
- mDNS with IPV6 is not supported.
- Bus thread stack may need to increase if local variables are used in user callback to avoid stack overflow.
- Low Power examples usage and documentation still under scope of improvement.
- Observed sl\_wifi\_get\_wireless\_info() API is giving wrong security type and PSK for WPA3 Transition supported client mode.
- Observed socket close is not working as expected for TLS socket when socket connect, send data and socket close are performing in a continuous loop.
- Observed issues with http\_otaf\_twt example.
- Observed data not received from last socket while sending from multiple TCP client sockets
- WMM-PS/UAPSD is not supported
- Application firmware\_flashing\_from\_host\_uart\_xmodem is not working as expected

### **Wi-Fi/Network Stack**

**Wi-Fi STA**

- STA Connection with the WPA3 Hunting and Pecking algorithm takes about 3-4 seconds.
- Observed connection failures with some AP's in more channel congestion ~50-60% occupancy open lab environment.
- Region selection based on country IE in the beacon is not supported
- Observed intermittent beacon reception from Access Point ( beacon misses) when channel congestion > 85%.
- While scanning with low power mode enable, sensitivity degradation of 3-6dB is observed. Due to this AP's that are at longer range might not be shown in scan results.
- Passive scan is failing when DUT is configured in world domain.

**Access Point (AP) Mode**

- Fixed rate configuration in AP mode using sl\_wifi\_set\_transmit\_rate API is not being set as expected. 

**WPA2 Enterprise security (STA)**

- Observed connection issue with configuring certificate key and programming 4096 bit key and SHA384/SHA512 certificates.

**Wi-Fi Concurrency (AP + STA in same channel)**

- Observed 3rd party STA association fail with 917 AP while 917 STA mode is connecting/reconnecting to configured 3rd party AP. Reconnect 3rd party STA to 917 AP in such scenarios. 

**OFDMA (UL/DL)**

- Less throughput observed in DL-OFDMA with some APs that enabled Low density parity check coding.

**MUMIMO (DL)**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Performance, Interop issues with MU MIMO with certain APs. 
- Less throughput observed in MU-MIMO with some APs that enabled  Low density parity check coding

**MU-MIMO (UL)**

- UL MU-MIMO is not supported.

**TWT**

- When sl\_wifi\_enable\_target\_wake\_time() API is used, occasional MQTT disconnections may be observed if TWT is configured with longer TWT intervals (>30secs) with embedded MQTT + TWT.  As an alternative, it's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() API, where these dependencies are internally handled.

**Wi-Fi STA Rejoin**

- Observed Scanning (probe request) in all channels instead of the channels configured in selective channel(channel\_bitmap\_2g4) during rejoin process. 

**IPv4/IPv6**

- IP change notification is not indicated to the application. 
- In concurrent mode with dual IP, if the STA starts after AP is up, the STA IP configuration may fail for DHCP stateless mode.
- In concurrent mode, data transfer using the Link-local address will always use the first IP interface created by the application.

**BSD Socket API**

- Every server socket created consumes a socket (maximum of 10 sockets supported) and every subsequent connection to server socket consumes an additional socket (from the same pool of 10 sockets), which limits the number of server connections supported.
- Observing issues with TCP retries when power save mode is enabled, especially when the module is in idle state.
- TCP maximum retry value is valid upto 31

**SSL Client/Server**

- Sometimes during SSL Handshake, ECC curve parameters generated are wrong, resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
- Secure SSL renegotiation is not supported in the Embedded Networking Stack

**HTTP Client/ HTTPS Client**

- Observed occasional HTTPS continuous download failures when power save is enabled. Recommended to disable it before performing HTTPS continuous downloads

**SNTP**

- Unable to get SNTP async events when Coex mode and power save are enabled 

**Throughputs & Performance**

- Wi-Fi alone throughput is about SDK 3.x (42Mbps). SDK refinements are in progress to further improve Wi-Fi Standalone and Coex Throughputs. 

**Wi-Fi IOT Cloud integration**

- **AWS IOT Core**
  - Observed AWS MQTT keepalive transmission is not happening at expected intervals with power save enabled.
  - Observing LAST\_WILL\_MESSAGE is random at every MQTT connection rather than the configured Message/Length
- **AZURE IOT Core**
  - Observed DUT after sending the data, its not sending the MQTT keep alive packet due to this Azure HUB closing the connection when power save is enabled.

**Wi-Fi Interoperability (IOP)**

- Observed disconnections with Amplifi (AFI-INS-R) AP with powersave enable
- TWT session is failing due to disconnections observed in DUT if rx\_latency is set to 55 seconds and receive data is also set to 55 seconds on MI Xiaomi RA72 and Tplink AX53 AP's
- Observed less throughput(~1Mb) while running TCP RX with Max\_PSP powersave with DLink 810 AP
- Observed interop issue (random disconnections) with few APs (EERO 6+, EERO PRO 6E, Cisco Catalyst 9120AXID)
- Disconnections observed with Netgear RAX120 AP in WPA3 Security

### **BLE**  

**DTM/PER**

- Recommend to limit BLE Tx Maximum power to 18 dBm.  Please don't use for 127 power\_index for BLE HP chain with this release.

**DLE**

- Removed the ble\_data\_length PSRAM example as it does not work with 121x and 141x OPN's. 

**Privacy** 

- DUT hang at the rsi\_bt\_get\_local\_device\_address API in central role, when using PSRAM boards (121x, 141x, 111M) and enabling power save in the ble\_privacy application,

**SMP**  

- SMP is not working with 110L(Lite wireless firmware image) board. 

### **Multi-protocol**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Wi-Fi + BLE intermittent connection failures, disconnections, and data transfer stalls in the long run when power save is enabled.
- Observed "DUT is not disconnecting to the AP when initiating disconnection from EFR connect app screen using wifi\_station\_ble\_provisioning\_aws example
- Observed DUT failed to load certificate with error "0x10026" (SL\_STATUS\_SI91X\_WRONG\_PARAMETERS) while running wifi\_https\_ble\_dual\_role\_v6 application
- Observed BLE bonding failure during continuous HTTPS download.
- Observed BLE and WLAN connection failure with SMP, when WLAN connect and HTTPS GET called in a loop.
- Observed issue in displaying throughput for interval of 'TEST\_TIMEOUT' when CONTINUOUS\_THROUGHPUT enabled in wifi\_station\_ble\_throughput\_app demo
- In Coex opermode, with memory config of TA - 480K and M4 - 192K Wi-Fi through is degrading by 10Mbps due less memory available for throughput.

### **System**

- Observed random hang issues with encrypted firmwares on some earlier variant of boards  (Si917-6031A, Si917-4338A) with powersave enable.
- This release addresses several issues, resulting in an additional 1K RAM usage in the NWP core. Consequently, this reduces the available heap size by 1K for the NWP core. Users with configurations that were already near the heap limit may experience either minor throughput issues OR functionality issues with this update.

On encountering a problem, it can be mitigated by considering the following options:

- Reduce the number of enabled features in the NWP core.
- Switch to a memory configuration that allocates more RAM to the NWP core while reducing RAM allocated to the Host processor.

### **Simplicity Studio and Commander** 

- Simplicity commander does not support options under "Debug Lock tools".
- All projects in the package are compatible with **GNU ARM V12.2.1** toolchain

## **Planned Roadmap Features**

### **SDK**

- WPS 2.0 PIN/PUSH, Network Manager Enhancements
- Websockets
- Matter integration with Embedded TCP/IP stack 
- Enhanced buffer management, Throughput optimization
- TLS over LWIP, Network layer applications (MQTT, HTTP, etc.) over LWIP.
- Debugging utilities, UC Configurator for Wireless configurations

### **System**

- Secure communication over CPC (UART & SDIO interfaces)

### **MCU**

- Dual Flash: Separate flash for Cortex M4 and Wireless Processor
- Peripherals: BOD, OPAMP, HSPI, GPDMA, QEI, RNG, IR decoder
- Software-configurable MCU application memory options (192 or 256 or 320 kB)
- RTOS support for MCU Peripheral examples
- SensorHub: Support for SDC\_ADC
- DMA flow control, self-loop mechanism and as a clock qualifier
- Support for I2S PCM functionality
- Power Manager service enhancements
- Pintool GA quality and support to any upcoming peripheral
- NVM3 application execution from PSRAM(text and data segments)

### **Wi-Fi/Network Stack**

- Spatial Re-Use, BSS Max Idle
- Provisioning using Wi-Fi AP

### **BLE**

- Support for 8 Peripheral + 2 Central connections 
- Support for BLE Mesh (4 nodes only) for limited Mesh use case

## **Limitations and Unsupported Features**  

### **System**

- None

### **SDK**

- Baremetal mode is not supported.

### **Wi-Fi/Network Stack**

- TLS 1.3 Server is not supported.
- 40 MHz bandwidth for 2.4 GHz band is not supported.
- Max 3 SSL sockets are supported in Wi-Fi alone and CoEx modes. No.of  SSL Sockets in Wi-Fi + BLE based on RAM memory configuration selected. 
- In SSL ECC Curve ID supported is 23. SSL handshake with 3rd party clients depends on the SSL ECC Curve ID.
- The number of Non-Transmitting BSSIDs processed is limited by the beacon length that can be processed by the stack (which is 1024 bytes). Beacons greater than 1024 Bytes in length will not be processed.
- Multiprotocol (STA +BLE) + EAP Security modes supported only with Memory configurations EXT\_FEAT\_416K\_M4SS\_256K and EXT\_FEAT\_480K\_M4SS\_192K.
- UL MUMIMO is not supported.
- WPA3 AP supports only H2E algorithm.
- PMKSA caching is not supported in WPA3 AP mode.
- Maximum embedded MQTT Publish payload is 1 kbyte.
- Timeout value for socket select and socket receive calls of less than 1 second is not currently supported.
- SA query procedure not supported in 11W AP mode.
- WPA3 AP transition mode is not supported.
- AP standalone mode does not support Tx aggregation. Rx aggregation is supported with limited number of BA sessions.
- In concurrent AP mode, aggregation (Tx/Rx) is not supported.
- Embedded HTTP Server is not supported.
- mDNS with IPV6 is not supported.
- Low power scan supports 1 mbps packets reception only.
- Auto PAC Provisioning in EAP-FAST with TLSv1.2 is not supported.

### **BLE**

- For BLE, if the connection is established with a small connection interval (less than 15 ms), simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported.
- BLE maximum two concurrent connections are supported, which can be either a connection to two peripheral devices, to one central and one peripheral device or two central devices.
- BLE Slave latency value is valid up to 32 only.
- Maximum supported AE data length is 200 bytes.
- Supports only two ADV\_EXT sets.
- Supports only two BLE connections (1 Central and 1 Peripheral) with AE.
- Advertising Extension feature is not supported in Coexistence.
- The  ae\_central & ae\_peripheral applications are not supported with TA\_352K\_M4\_320K RAM configuration.
- Two BLE connections are not supported with M4 powersave. It only supports a single connection. 
- Isochronous channels feature is not supported. 
- Connection subrating feature is not supported. 
- LE power controller feature is not supported.
- EATT feature is not supported.
- Periodic Advertising with a response feature is not supported. 
- BLE Audio is not supported.
- The feature of dynamically changing the TX power when extended advertising is active is not supported.
- EFR Connect mobile application doesn't have support to differentiate the BLE configurators based on the Bluetooth Device address.
- The maximum BLE power has been reduced by 2dB compared to the Datasheet Number, which will be addressed in the upcoming 3.3.1 patch release.

### **MCU**

- SensorHub supports PS1-power state with ADC sensor. In this mode, the other sensor's operation is not supported
- Manual chip select option is not supported when using GSPI
- Following peripherals/features are not functioning as intended
  - ADC Multichannel, SSI Dual and Quad mode, SIO and Temp sensors
- LittleFS support is intended to be used only with Dual flash
- PS1 state is not supported in Power Manager
- Power is same for PS4 and PS3 states with powersave mode of power manager
- When using Button\_0 as wakeup source, it limits the button functionality at application layer. For using Button\_0 for application specific functionality, enable 'SL\_SI91X\_NPSS\_GPIO\_BTN\_HANDLER'

### **Multi-protocol**

- Wi-Fi AP + BLE currently not supported. 
- EXT\_FEAT\_352K\_M4SS\_320K RAM configuration is not supported for coex mode with SSL

## **Removed/Deprecated Features**

- Hardware Reference Manual updates - Updated platform code and applications for the below peripherals to align with the latest HRM 
  - Removed Peripherals - SIO, RO temp sensor, FIM
  - Removed features - ADC (Multichannel, external event based sampling), UART/USART(lower baud rates110/150, bit width 1-4 and 9, IrDA support, Multi-drop RS485 support), DAC Reference voltage for ADC, Hardware Flow control for ULP UART
  - Removed USART\_RX\_CHAR\_TIMEOUT interrupt macro as the feature is not functional 
  - Removed Hardware setup API from all power save application that uses sllib\_m4\_power\_save.slcc component
  - Removed MVP support from SIWG917M110LGTBA component 
  - Removed hard-coded I2S0 channel value in rsi send and receive APIs

> **Note:** 
> 
> The following BLE Synchronous API's will be deprecated soon and the equivalent Asynchronous API's will be used instead in all BLE applications :
> 
> |**S.NO**|**BLE Synchronous API's** |**BLE Asynchronous API's** |
> | :- | :- | :- |
> |1|rsi\_ble\_get\_profiles|rsi\_ble\_get\_profiles\_async|
> |2|rsi\_ble\_get\_profile|rsi\_ble\_get\_profile\_async|
> |3|rsi\_ble\_get\_char\_services|rsi\_ble\_get\_char\_services\_async|
> |4|rsi\_ble\_get\_inc\_services|rsi\_ble\_get\_inc\_services\_async|
> |5|rsi\_ble\_get\_char\_value\_by\_uuid|rsi\_ble\_get\_char\_value\_by\_uuid\_async|
> |6|rsi\_ble\_get\_att\_descriptors|rsi\_ble\_get\_att\_descriptors\_async|
> |7|rsi\_ble\_get\_att\_value|rsi\_ble\_get\_att\_value\_async|
> |8|rsi\_ble\_get\_multiple\_att\_values|rsi\_ble\_get\_multiple\_att\_values\_async|
> |9|rsi\_ble\_get\_long\_att\_value|rsi\_ble\_get\_long\_att\_value\_async|
> |10|rsi\_ble\_set\_att\_value|rsi\_ble\_set\_att\_value\_async|
> |11|rsi\_ble\_set\_long\_att\_value|NA|
> |12|rsi\_ble\_prepare\_write|rsi\_ble\_prepare\_write\_async|
> |13|rsi\_ble\_execute\_write|rsi\_ble\_execute\_write\_async|
> |14|rsi\_ble\_indicate\_value\_sync|rsi\_ble\_indicate\_value|

<br>

# WiSeConnect3\_SDK\_3.3.0 SoC Release Notes

## **Release Details**

|**Item**|**Details**|
| :- | :- |
|Release date|June 18th, 2024|
|SDK Version|3\.3.0|
|Firmware Version|<p>Standard: 1711.2.12.1.0.0.10</p><p>Lite: 1711.2.12.1.0.2.10</p>|
|GSDK/SiSDK Version|SiSDK 2024.6.0|
|Studio Version| 5.9.1.0|
|Release Package Name|WiSeConnect3\_SDK\_3.3.0|
|Supported RTOS|FreeRTOS|
|Operating Modes Supported|Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP|

- SiWx917 release consists of two components:
  - Standard Wireless Firmware - SiWx917 Firmware Binary available as SiWG917-B.2.12.1.0.0.10.rps
  - Lite Wireless Firmware - SiWx917 Firmware Binary available as SiWG917-B.2.12.1.0.2.10.rps, this image is with reduced features for parts with SiWG917M110LGTBA OPN. 
  - Wiseconnect3 Library - Wiseconnect3 SDK library runs on internal Cortex M4

> **Note:**
>
> - Mandatory to upgrade the earlier version of boards (Si917-6031A Prokit or BRD4338A boards) or 917 Silicon ICs with instructions as outlined in this document "SiWG917–TA\_Flash\_Memory\_Map\_ChangeGuide\_v1.3.pdf" for more details.  
> - The latest releases will have bug-fixes, enhancements, and new features in both 'SDK' and 'Firmware'. Hence, it is recommended to update and use 'SDK' and 'Firmware' of same release package.
> - It is recommended to update TA image first followed by M4 image.

## **Supported Hardware**

|**Hardware**|**OPN**|
| :- | :- |
|Silicon (IC OPN)|SiWG917M111MGTBA, SiWG917M100MGTBA, SIWG917M110LGTBA(lite image)|
|Development Kits|<p>Pro Kit: SiWx917-PK6031A, Si917-PK6031A. </p><p>(Pro Kit includes Mother board "SI-MB4002A" + Radio board)</p><p>Radio boards: SiWx917-RB4338A, SiWx91x-RB4342A</p>|

## **Supported Features** 

<table><tr><th><b>Section</b></th><th><b>Sub-Section</b></th><th><b>Feature</b></th><th><p><b>Lite Wireless Firmware</b></p><p><b>(4MB flash OPN<br>OPN No: SiWG917M110LGTBA)</b></p></th><th><b>Standard Wireless Firmware (For other OPNs)</b></th></tr>
<tr><td rowspan="8">System</td><td rowspan="4">Operating modes</td><td>Wi-Fi STA (802.11ax, 802.11n)</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi 802.11n AP</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi STA (802.11ax, 802.11n) + BLE</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Security </td><td>Secure Boot, Secure Key storage and HW device identity with PUF, Secure Zone, Secure XIP (Execution in place) from flash, Secure Attestation, Anti Rollback, Debug Lock, Flash Protection</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Secure firmware upgrade options</td><td><p>- Firmware loading support by Commander Tool through Jlink Debugger. Jlink connected to Serial Wire Debug (SWD) </p><p>- Firmware loading via ISP using UART (Commander or Serial terminal), SPI Interface</p><p>- Secure Over the Air (OTA) Upgrade</p><p>- Firmware update via Bootloader</p></td><td>Supported</td><td>Supported</td></tr>
<tr><td>Crypto Support</td><td><p>- Crypto API's for Hardware Accelerators: Advanced Encryption Standard (AES) 128/256/192, Secure Hash Algorithm (SHA) 256/384/512, Hash Message Authentication Code (HMAC), Random Number Generator (RNG), SHA3, AES-Galois Counter Mode (GCM)/ Cipher based Message Authentication Code (CMAC), ChaCha-poly, True Random Number Generator (TRNG)</p><p>- Software Accelerators: RSA, ECC</p><p>- PSA Crypto APIs support for all crypto operations.</p><p>- Wrapping Secret keys (Symmetric crypto). </p><p>- Added ECDSA Sign and Verify APIs</p></td><td>Supported</td><td>Supported</td></tr>
<tr><td>System Power Save</td><td><p>- Deep Sleep with RAM retention and without RAM retention. </p><p>- Wireless Power Save: Connected Sleep (Wi-Fi Standby Associated), BLE Advertising with powersave, BLE Scan with powersave ,  BLE connection with powersave. Only Max PSP power save mode is supported in BLE. </p></td><td>Supported</td><td>Supported</td></tr>
<tr><td rowspan="13">Wi-Fi</td><td>Wi-Fi Protocols</td><td>IEEE 802.11 b/g/n/ax (2.4GHz)</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Access Point (AP) Mode</td><td><p>- 4 Client Support, Hidden SSID Mode, Auto Channel Selection, Scan in AP mode (Alpha)</p><p>- Wi-Fi Security </p><p>&emsp;- WPA2 Personal, WPA3 Personal (H2E method only) (Alpha), WPA Mixed mode (WPA/WPA2) </p></td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi Scan</td><td>Selective Scan, Active/Passive Scan</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi STA (Security Modes)</td><td>Open Mode, WPA2 Personal, WPA2 Enhancements, WPA3 Personal, Mixed Mode (WPA/WPA2), WPA3 Personal Transition Mode (WPA2/WPA3)</td><td>Supported</td><td>Supported</td></tr>
<tr><td>WPA2 Enterprise security (STA)</td><td>Method: PEAP/TTLS/TLS 1.0/TLS 1.2/FAST/LEAP</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi STA Rejoin</td><td></td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi STA Roaming </td><td>BG Scan, OKC (Opportunistic Key caching), PMK (Pairwise Master Key) caching, Pre-Authentication</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi Protocol Power Save </td><td>Deep sleep (unconnected state), Max PSP, Enhanced Max PSP, Fast PSP, TWT</td><td>Supported</td><td>Supported</td></tr>
<tr><td>QoS</td><td>WMM-QoS</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi 6 Feature</td><td>MUMIMO (DL), OFDMA (UL/DL), iTWT, TWT I-Frame & TWT Enhancements (Automatic TWT Configuration), BSS coloring, MBSSID</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi Concurrency </td><td>AP+STA (Same channel)</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi Band/Channels</td><td>2\.4GHz CH1-11, 2.4GHz CH1-13, 2.4GHz CH1-14</td><td>Supported</td><td>Supported</td></tr>
<tr><td>Known Security Vulnerabilities Handled</td><td>WPA2 KRACK Attacks, Fragment and Forge Vulnerability</td><td>Supported</td><td>Supported</td></tr>
<tr><td rowspan="6">Network stack</td><td rowspan="2">Core Networking Features</td><td><p>- IPv4/IPv6/UDP/TCP/ARP/ICMP/ICMPv6</p><p>- SSL client versions TLSV1.0, TLSV1.2, TLSV1.3 </p><p>- SSL server versions TLSV1.0 and TLSV1.2</p><p>- DHCPv4/DHCPv6 Client</p><p>- TCP/IP Bypass (LWIP as Hosted stack for reference)</p></td><td>Supported</td><td>Supported</td></tr>
<tr><td>- DHCPv4 Server</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td rowspan="2">Advanced Network Features</td><td>HTTP Client/HTTPS Client/DNS Client, Embedded MQTT/MQTT on host (AWS and AZURE) </td><td>Supported</td><td>Supported</td></tr>
<tr><td>SNTP Client, IGMP</td><td>Not Supported</td><td>Supported</td></tr>
<tr><td>Wi-Fi IoT Cloud Integration</td><td><p>- AWS IOT Core</p><p>- Azure IoT</p></td><td>Supported</td><td>Supported</td></tr>
<tr><td>BSD and IoT sockets application programming interface(API)</td><td></td><td>Supported</td><td>Supported</td></tr>
<tr><td rowspan="2">BLE </td><td>Legacy features</td><td><p>- GAP(Advertising, Scanning, initiation, Connection and Bonding)</p><p>- Generic Attribute Protocol(GATT)</p><p>- Attribute protocol(ATT)</p><p>- Security</p><p>- LL Privacy 1.2</p><p>- Accept list</p><p>- Directed Advertising</p><p>- LE PHY(1Mbps, 2Mbps) & Coded PHY(125Kbps, 500kbps)</p><p>- Simultaneous scanning on 1Mbps and Coded PHY</p><p>- LE dual role topology</p><p>- LE data packet length extensions(DLE)</p><p>- Asymmetric PHYs</p><p>- LE channel selection algorithm 2 (CSA#2)</p><p>- LE Secure connections</p></td><td>Supported</td><td>Supported</td></tr>
<tr><td>Advertising Extensions </td><td><p>- Extended Advertising</p><p>- Periodic Advertising</p><p>- Periodic Advertising scanning</p><p>- Extended Advertising scanning</p><p>- Periodic Advertising list</p><p>- LE periodic advertising synchronization</p></td><td>Not Supported </td><td>Supported</td></tr>
</table>

### **MCU**

- **Memory**
  - Common Flash: Single shared Flash for both Cortex-M4 and NWP (Wireless Processor)
  - Common Flash + External PSRAM
- **Power States**
  - Active: PS4, PS3, PS2, and PS1 
  - Standby: PS4, PS3, and PS2
  - Sleep: PS4, PS3 and PS2
  - Deep Sleep(Shutdown): PS0
- **Peripherals**
  - High Performance (HP) - ADC, Calendar, Config Timer (CT), eFuse, EGPIO, GSPI, I2C, I2S, MCPWM, SDIO Secondary, SIO, SSI, SYSRTC, UART, uDMA, USART, WDT, Analog Comparator, DAC, Temperature Sensors (RO and BJT), CTS, PSRAM, CRC and RTC
  - Ultra Low Power (ULP) - ULP\_ADC, ULP\_CALENDAR, ULP\_DAC, ULP\_GPIO, ULP\_I2C, ULP\_I2S, ULP\_TIMER, ULP\_UDMA
  - UC Supported Peripherals - CRC, I2C, Calendar, SPI, SSI, SIO, uDMA, UART/USART, WDT, Config Timers, ULP Timers, SDIO Secondary, PWM, ADC, DAC, I2S and Analog Comparator
- **Services**
  - Sleep Timer, IOSTREAM, Power Manager, SensorHub, NVM3
  - The flash write feature has been enhanced to support the NWP area, providing a 20k allocation within the NWP flash memory for storing user data. Moreover, a Read API has been introduced to retrieve data from the NWP flash region.
- **Development Pro-Kit Peripheral Support**
  - LED, Button, MEMLCD, Joystick, RHT Sensor

### **Development Environment**

- Simplicity Studio IDE (SV5.9.1.0 version) and Debugger Integration. Refer to the latest version of the SoC "Getting-Started-with-SiWx917" guide for more details. 
- Recommended to install and use Silicon labs Simplicity SDK (Previously known as Gecko SDK), Git hub based version 2024.6.0.
- Simplicity Commander to supports Flash loading, provision of MBR programming, security key management, and calibration support for crystal and gain offsets. refer "siwx917-soc-manufacturing-utility-user-guide" for more details.** 
- Advanced Energy Monitoring (AEM) to measure ultra-low power capability on Development boards (Pro Kit).
- PinTool Supported peripherals - I2C, GSPI, SSI, SIO, UART/USART, Config Timers, PWM, I2S, ADC, DAC and Analog comparator

### **BLE** 

- GAP(Advertising, Scanning, initiation, Connection and Bonding)
- Generic Attribute Protocol(GATT)
- Attribute protocol(ATT)
- Security
- LL Privacy 1.2
- Accept list
- Directed Advertising
- Extended Advertising
- Periodic Advertising
- Periodic Advertising scanning
- Extended Advertising scanning
- Periodic Advertising list
- LE periodic advertising synchronization
- LE PHY(1Mbps, 2Mbps) & Coded PHY(125Kbps, 500kbps)
- Simultaneous scanning on 1Mbps and Coded PHY
- LE dual role topology
- LE data packet length extensions(DLE)
- Asymmetric PHYs
- LE channel selection algorithm 2 (CSA#2)
- LE Secure connections
- Bluetooth 5.4 Qualified

### **SDK**

- Simplified and Unified DX for Wi-Fi API and Platform APIs
- Simplifies application development and presents clean and standardized APIs
- UC (Universal Configurator) enables componentization, simplifying configuration of peripherals and examples
- BSD and ARM IoT-compliant socket API
- Available through Simplicity Studio and GitHub

### **Multi-protocol**

- Wi-Fi STA + BLE

### **PTA/Coexistence**

- 3 wire coex acting as Wi-Fi with external Bluetooth
- 3 wire coex acting as Wi-Fi with external Zigbee/OT

## **Changes in this release compared to v3.2.0 Release**

### **System**

- New API sl\_si91x\_fwup\_abort added to support aborting firmware upgrade
- The stability issues related to system-level power saving have been resolved, leading to an approximate 25% increase in power consumption in the WiFi & BLE standby cases in a Shield Room environment compared to the 3.2.0 release. This increase will be addressed in the upcoming 3.3.1 patch release. There is no power impact for active use cases, neither in Wireless nor in the Peripherals use cases

### **MCU**

- **Enhancements / New features**
  - New peripheral support -CRC
  - Clock configuration
    - Added APIs to configure M4 SoC and PLL clocks
    - Default clock configurations for si91x\_soc peripheral examples are updated to run in high-performance mode
  - Moved clock configurations from the driver layer to the application layer
  - Changed LED and button macro names instances as below,
    - button\_instance to simple\_button\_instance
    - button to simple\_button
    - led to simple led
    - led\_instance to simple\_led\_instance
  - Si917 Power Manager Enhancements
    - Added support for FreeRTOS Tickless-idle mode
    - Supports M4 PS4 and PS3 states with FreeRTOS Tickless-Idle
  - SIWG917M110LGTBA(lite image) support
  - For ease of use, the following peripheral examples have been separated as below:
    - UART: UART and USART
    - ADC: Static Mode and FIFO Mode
    - GPIO: Simple IO, Interrupt-based and Detailed IO
  - Added additional APIs for host pad selection in GPIO Driver

- **Fixed Issues**
  - Fix for timer drift with SYSRTC in tickless-idle mode
  - Resolved SIO\_UART baud rate issues
  - Resolved power state transitions issue when BTN0 is pressed
  - On 917 Pro-Kit, fixed spurious interrupts for Button\_0 when used as a wakeup source for M4
  - Resolved ULP\_ADC\_Multichannel pintool annotation issue
  - Macro ROM\_DRIVER\_PRESENT is replaced with peripheral-specific macros
  - Fixed GSPI performance issues
  - De-coupled Power down of efuse peripherals(RSI\_PS\_M4ssPeriPowerDown) from deinit API
  - Readme, API guide, and Software reference manual documentation have been updated

### **SDK**

- **Enhancements**
  - Enhanced M4 power save with M4 Tick-less idle power manager and added support for WLAN power save demos and updated readme files accordingly.
  - Added provision to few more ciphers support in the SDK.
  - Restructured connectivity\_firmware folder with standard and lite firmwares.
  - Added demo support for SIWG917M110LGTBA OPN.
  - Enhanced sl\_si91x\_set\_rtc\_time(), sl\_si91x\_host\_get\_buffer\_data() APIs with additional error handling checks.
  - Added support for parsing DHCP/IP notification events to Application.
  - Added support for parsing HTTP error codes to Application.
  - Moved eap\_flags from structure sl\_wifi\_advanced\_client\_configuration\_t to to sl\_net\_wifi\_client\_profile\_t
  - Added new API sl\_wifi\_get\_wireless\_info().
  - Updated documentation for SiSDK.
  - References to internal boards have been removed from all WSDK application readme files.
  - Enhanced socket API documentation and 'developing with sockets' guide.
  - Enhanced support for upgrading TA FW from simplicity studio launcher.

- **Fixed Issues**
  - Addressed the issue in the AWS MQTT demo that occurred when receiving continuous publish messages.
  - Fixed log prints issue in "Concurrent HTTP Server Provisioning" application.
  - Fixed issues in M4 power save in Wi-Fi alone and coex modes.
  - Fixed firmware upgrade failure issue in concurrent\_firmware\_update\_from\_host\_uart demo.
  - Fixed issues in Azure connectivity issues with symmetric key authentication.
  - Fixed issue with PLL mode in coex mode.
  - Fixed issues in MQTT remote terminate handler scenarios.
  - Fixed memory leak issues while performing continuous AES encryption and decryption 
  - Fixed Readme corrections for certificate name in HTTP Client application
  - Added Fix for FW upgrade failure due to invalid firmware.
  - Added fix for 0xb44 error observed in AP mode for sl\_wifi\_get\_mac\_address API.

### **Wi-Fi/Network Stack**

- **Enhancements**
  - WLAN transmit modulation performance improvements at 85C. ~~hot temperature.~~
  - Minor improvements in FCC band edge performance for 11ax.
  - Support added to trigger a passive and low power scan from host. Low power scan supports 1 mbps packets reception only.
  - Added STA-RX aggregation support in concurrent mode.
  - Added support to use user specific MAC address configured in EFUSE or flash.
  - Added feature to support Gain offset calibration in channel 14.
  - Added support to filter out multicast data packets.
- **Fixed Issues**
  - Fixed HTTP Post data 0xBBE2 error issue
  - Resolved DHCP address assignment issues in concurrent mode.
  - Enhanced the feature to skip the unicast global address assignment to stack if the static configuration is all f's
  - Fixed instability issues with concurrent mode dual IP feature
  - Added fix for CVE-2022-23303 (WPA3 SAE side channel attack)
  - Fixed flash write race condition observed intermittently during STA join/rejoin.
  - Added support to change sleep (listen) interval using power save command after join
  - TWT information frame related fixes
  - Fixed issues setting  TCP socket retry configuration through setsockopt()

### **BLE**

- **Enhancements**
  - Added BLE HP chain FCC margin improvements across corners.
  - Added BLE 2M receive max input power improvements.
  - Added BLE power backoff offsets implementation similar to WLAN.
  - Improved power vs index linearity in BLE HP chain.
  - Added log prints for BLE advertising, scanning, SMP connection, profile query and data transfer in ble\_multiconn\_gatt\_test application.
  - In the readme of the BLE and Multi protocol applications, the term 'EFR Connect App' has been replaced with 'Simplicity Connect App'.
- **Fixed Issues**
  - Added Power Manager Tickless changes in BLE applications.
  - Updated readme for bt\_stack\_bypass demo
  - Updated readme and images for ble\_throughput\_app, ble\_secureconnection, ble\_privacy, ble\_power\_save, ble\_multiconnection\_gatt\_test, ble\_longrange\_2mbps, ble\_ibeacon, ble\_heart\_rate\_profile, ble\_central, ble\_ae\_central and ble\_ae\_peripheral demos. 
  - Added options field in "rsi\_ble\_ae\_set\_periodic\_sync" API as per specification. 

### **Multi-protocol**

- None

## **Recommendations**

### **System**

- The current revision of SiWx917 has:
  - RAM memory of 672k bytes which can be shared between TA and M4 processors in SoC mode. 
  - The below configurations are applicable in SoC mode and can be configured based on the application requirement. EXT\_FEAT\_352K\_M4SS\_320K is the default configuration, based on requirement EXT\_FEAT\_480K\_M4SS\_192K configuration is selected for SoC mode multi-protocol examples.
    - EXT\_FEAT\_480K\_M4SS\_192K - This mode configures TA with 480k and M4 with 192K bytes of memory
    - EXT\_FEAT\_416K\_M4SS\_256K - This mode configures TA with 416k and M4 with 256K bytes of memory
    - EXT\_FEAT\_352K\_M4SS\_320K - This mode configures TA with 352k and M4 with 320K bytes of memory
  - SoC mode should not use 672k\_M4SS\_0K memory configuration.
- Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
- There are 2 Versions of Pro-Kits/Radio boards. Si917-6031A based on Si917-4338A (Rev **A01 - A11**) and SiWx917-6031A based on SiWx917-4338A (Rev A12). To get optimal power numbers, enable macro "SL\_SI91X\_ENABLE\_LOWPWR\_RET\_LDO" pre-processor define for ICs or while using SiWx917-6031A Pro-kit, SiWx917-4338A version of boards. This macro should be disabled for earlier variant of board (Si917-6031A, Si917-4338A).
- With RAM configuration (EXT\_FEAT\_352K\_M4SS\_320K), only 352K memory is available to TA  which limits the features supported, Recommended to enable EXT\_FEAT\_416K\_M4SS\_256K in Wi-Fi + BLE Multi protocol mode to enable more Network features.
- For EXT\_FEAT\_416K\_M4SS\_256K  and EXT\_FEAT\_480K\_M4SS\_192K memory configurations, it is recommended to retain both TA and M4 RAMs in power save.
- It is highly recommended to use connectivity firmware version 1711.2.12.1.0.0.10 with SDK v3.3.0 for improved stability in power save mode.

### **Wi-Fi/Network Stack**

- It is recommended to enable bit 16 of the 'Extended TCP IP Feature' bit map in the opermode command for all Wi-Fi Socket operations from the host to ensure graceful handling during asynchronous closures from the peer.
- For high throughput applications, aggregation (bit 2 of feature\_bit\_map) is recommended to be enabled in opermode. 
- Users can enable SL\_SI91X\_EXT\_TCP\_IP\_SSL\_16K\_RECORD in 'Extended TCP IP Feature' bit map in opermode for (HTTPS server) supporting 16k record.
- **TWT**
  - Recommendation is to use sl\_wifi\_target\_wake\_time\_auto\_selection() API for all TWT applications. 
  - It is recommended to issue iTWT setup command once IP assignment, TCP connection, application specific socket connections are done.
  - When using sl\_wifi\_enable\_target\_wake\_time API, increase TCP / ARP Timeouts at the remote side depending upon the configured TWT interval configured. It's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() as an alternative.
  - In case of TWT in coex mode, when using sl\_wifi\_enable\_target\_wake\_time API, use TWT wake duration <= 16 ms and TWT wake interval >= 1 sec. If wake duration > 16 ms or TWT wake interval < 1sec, there might be performance issues.
  - For iTWT GTK interval in AP should be configured to max possible value or zero. If GTK interval is not configurable on AP side, recommended TWT interval (in case of sl\_wifi\_enable\_target\_wake\_time API) or RX Latency (in case of sl\_wifi\_target\_wake\_time\_auto\_selection API) is less than 4sec.
  - When sl\_wifi\_enable\_target\_wake\_time API is used, configuring TWT Wake interval beyond 1 min might lead to disconnections from the AP. Recommended to use TWT wake interval of less than or equal to 1 min.
  - When using sl\_wifi\_enable\_target\_wake\_time API, it is recommended to set missed\_beacon\_count of sl\_wifi\_set\_advanced\_client\_configuration API greater than 2 times of the configured TWT Interval.
- Disable power save for high throughput applications or use FAST PSP power save mode as per application requirement.
- The application needs to ensure that it sets RTC with the correct timestamp before establishing the SSL/EAP connection.
- The minimum timeout value should not be less than 1 second for socket select and socket receive calls. 
- Please refer Keep alive intervals supported by MQTT broker and configure keep alive interval values accordingly.
- The minimum keep alive interval value recommended for embedded MQTT is 10 Seconds. 
- Disable power save and suspend any active TWT sessions before triggering HTTP OTAF.
- Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with the power save.
- Recommended to configure VAP\_ID properly for Si91x STA and AP using sl\_si91x\_setsockopt\_async(), in case of data transfer.
- Recommended to use valid length(<= 202 bytes) for topic to be published while using Embedded MQTT, else it leads to return wrong error code(0x21).
- In concurrent mode with dual IP, it is advised to bring up STA first (IP configuration) and AP later.
- It is recommended to enable bit 3 of the 'Extended TCP IP Feature' bit map in the opermode command for all Wi-Fi Socket operations from the host.
- It is recommended to configure Tx ,Rx , Global buffer pool ratio in the buffer config command based for all Wi-Fi Socket operations from the host

### **BLE**

- In BLE, the recommended range of Connection Interval in
  - Power Save (BLE Only) - 100 ms to 1.28 s.
- In BLE, during Connection, the configuration of Scan Interval and Scan Window with the same value is not recommended. The suggested ratio of Scan Window to Scan Interval is 3:4.
- In BLE, if a device is acting as Central, the scan window (in set\_scan\_params and create\_connection commands) must be less than the existing Connection Interval. The suggested ratio of Scan Window to Connection Interval is 2:3.
- In BLE mode, if scanning and advertising are in progress on the SiWx91x module and it subsequently gets connected and moves to the central role, scanning stops else if it moves to the peripheral role, advertising stops. To further establish a connection to another peripheral device or to a central device, the application should give a command for starting advertising and scanning again.

### **Multi-protocol**

- For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
- Wi-Fi + BLE Advertising
  - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals may be observed.
  - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity.
- Wi-Fi + BLE scanning
  - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
  - BLE scanning will be stopped for intervals that collide with Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Connections
  - All standard connection intervals are supported.
  - For a stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Data Transfer
  - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout.
  - Ensure Wi-Fi activity consumes lower intervals.

### **MCU**

- It is not recommended to use switch\_m4\_frequency() for clock scaling. Refer to the migration guide for more details on this
- It is not recommended to use the RO clock for any Clock Configurations in MCU
- PSRAM examples are not supposed to be used with Non-PSRAM OPNs
- In tickless idle mode, SYSRTC and wake on wireless are configured as default wakeup sources. Recommended not to reinstall them
- It is recommended to enable DMA for supported peripherals to achieve better throughput
- For I2C Fast-Plus and High-speed modes, it is recommended to use High power instances (I2C0, I2C1) for core clocks operating between 80MHz and 180MHz
- For using GPIO-based wakeup, it is strongly recommended to install GPIO component in powersave applications
- It is recommended to use both CTS and RTS for UART flow control

## **Known Issues of WiSeConnect3\_SDK\_3.3.0 Release**

### **MCU**

**SensorHub**

- Power transitions with AWS are not stable in SensorHub
- Support for ADC FIFO Mode and Multi channel is open
- PS1-state supports only ADC sensor
- Tickless IDLE mode is not supported in sensor hub application

**SSI**

- SSI Dual and Quad mode are not functional as expected
- Unable to use SSI Primary and SSI Secondary together in a single project

**I2C**

- ULP\_I2C in low power mode, when DMA is enabled will not function as expected

**I2S**

- FIFO threshold configurations are not working as expected

**Config Timers**

- PWM mode is not working as expected
- Config timer features to trigger DMA and interrupts on events or counters are not supported
- 32-bit mode is not working as expected

**UART**

- UART instances does not support different FIFO Thresholds
- Character Timeout feature is not supported
- UART does not support lower baud rates (110 and 115)
- Multi-drop RS485 interface is not supported

**ADC**

- ADC Multichannel mode is not functioning as expected
- ADC static mode sampling rate is supported up to 2.5Msps

**Analog comparator**

- Few configurable inputs (opamp2, opamp3 and BOD) are not working as expected

**Temperature Sensors**

- RO and BJT Temperature readings are not accurate

**Power Manager**

- Few wakeup sources (BOD and Comparator) are not working as expected
- PS1-state is not supported

**Miscellaneous**

- Naming convention of a few peripherals (UART/USART, I2C, GSPI, SSI, ANALOG COMPARATOR, I2S) is not aligned between the HRM/Datasheet and Code
- PAD Configuration address is wrongly mentioned in HRM and Peripherals tab of Studio

### **SDK**

- Observed Wi-Fi connection is successful even after deleting the stored network credentials using sl\_net\_delete\_credential and responding with SL\_NET\_INVALID\_CREDENTIAL\_TYPE for sl\_net\_get\_credential.
- Enhanced sl\_wifi\_get\_firmware\_version() API to provide more details (ROM ID, chip ID, security version, etc) which is not backward compatible with firmware older than 1711.2.10.1.0.0.4. Firmware binary notation does not include the security version number.
- In PSRAM enabled demos, moving of text, data and stack segments to PSRAM is allowed. BSS and Heap should still be in SRAM.
- Asynchronous Azure MQTT is not supported, this will be addressed in upcoming release(s).
- mDNS with IPV6 is not supported.
- Observing DUT is getting stuck while running MQTT publish with 2 different topics
- Observed MQTT RX is failing when continuous data sent every 1 second in aws\_mqtt application
- Observed DUT Failed to connect to AP with timeout error 0x7(SL\_STATUS\_TIMEOUT)  while single MQTT subscribe and publish in loop with embedded MQTT application
- Observing LAST\_WILL\_MESSAGE is random at every MQTT connection rather than the configured Message/Length with embedded MQTT application
- Observed DUT is not initiating the AWS connection post-rejoin failure indication after publishing 10-15 packets, turning OFF AP and trying to rejoin the network with aws\_mqtt application
- Observing MQTT disconnect API is getting failed with 0x7(SL\_STATUS\_TIMEOUT) error while doing publish & subscribe with embedded MQTT application
- Bus thread stack may need to increase if local variables are used in user callback to avoid stack overflow.
- Low Power examples usage and documentation still under scope of improvement.
- Observed not being able to receive UDP Rx TPUT print at Teraterm with IPV6 Throughput example
- Observed sl\_wifi\_get\_wireless\_info() API is giving wrong security type and PSK for WPA3 Transition supported client mode.
- Observed socket close is not working as expected for TLS socket when socket connect, send data and socket close are performing in a continuous loop.
- Observed MQTT Rx is not able to resume after rejoin in [wifi_station_ble_provisioning_aws](https://siliconlabsconnectivitysqa.testrail.io/index.php?/tests/view/863900) demo.
- Below are the list of applications which are not working as expected:
  - http\_otaf\_twt 

### **Wi-Fi/Network Stack**

**Wi-Fi STA**

- STA Connection with the WPA3 Hunting and Pecking algorithm takes about 3-4 seconds.
- Observed connection failures with some AP's in more channel congestion ~50-60% occupancy open lab environment.
- Region selection based on country IE in the beacon is not supported
- Observed intermittent beacon reception from Access Point ( beacon misses) when channel congestion > 85%.
- While scanning with low power mode enable, sensitivity degradation of 3-6dB is observed. Due to this AP's that are at longer range might not be shown in scan results.
- Observed TLS1.3 handshake failure with CHACHA20\_POLY1305\_SHA256 cipher

**Access Point (AP) Mode**

- Scan feature in AP mode not fully functional. 
- Fixed rate configuration in AP mode using sl\_wifi\_set\_transmit\_rate API is not being set as expected. 

**WPA2 Enterprise security (STA)**

- Observed connection issue with configuring certificate key and programming 4096 bit key and SHA384/SHA512 certificates.
- Observing DUT is throwing 0x1001c when configuring .data.certificate\_key as "123456789".
- Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.

**Wi-Fi Concurrency (AP + STA in same channel)**

- Observed 3rd party STA association fail with 917 AP while 917 STA mode is connecting/reconnecting to configured 3rd party AP. Reconnect 3rd party STA to 917 AP in such scenarios. 

**OFDMA (UL/DL)**

- Less throughput observed in DL-OFDMA with some APs that enabled Low density parity check coding.

**MUMIMO (DL)**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Performance, Interop issues with MU MIMO with certain APs. 
- Less throughput observed in MU-MIMO with some APs that enabled LDPC.

**MU-MIMO (UL)**

- UL MU-MIMO is not supported.

**TWT**

- When sl\_wifi\_enable\_target\_wake\_time() API is used, occasional MQTT disconnections may be observed if TWT is configured with longer TWT intervals (>30secs) with embedded MQTT + TWT.  As an alternative, it's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() API, where these dependencies are internally handled.

**Wi-Fi STA Rejoin**

- Observed Scanning (probe request) in all channels instead of the channels configured in selective channel(channel\_bitmap\_2g4) during rejoin process. 

**IPv4/IPv6**

- IP change notification is not indicated to the application. 
- In concurrent mode with dual IP, if the STA starts after AP is up, the STA IP configuration may fail for DHCP stateless mode.

**BSD Socket API**

- Every server socket created consumes a socket (maximum of 10 sockets supported) and every subsequent connection to server socket consumes an additional socket (from the same pool of 10 sockets), which limits the number of server connections supported.
- Observed socket shutdown is not happening and DUT is getting stuck at  "prvCheckTasksWaitingTermination" while doing socket connection-> data transfer ->  socket shut down for multiple iterations.

**SSL Client/Server**

- Sometimes during SSL Handshake, ECC curve parameters generated are wrong, resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
- Secure SSL renegotiation is not supported in the Embedded Networking Stack

**HTTP Client/ HTTPS Client**

- Observed occasional HTTPS continuous download failures when power save is enabled. Recommended to disable it before performing HTTPS continuous downloads

**SNTP**

- Unable to get SNTP async events when Coex mode and power save are enabled 

**Throughputs & Performance**

- Wi-Fi alone throughput is about SDK 3.x (42Mbps). SDK refinements are in progress to further improve Wi-Fi Standalone and Coex Throughputs. 

**Wi-Fi IOT Cloud integration**

- **AWS IOT Core**
  - Observed AWS MQTT keepalive transmission is not happening at expected intervals with power save enabled.
  - Observing LAST\_WILL\_MESSAGE is random at every MQTT connection rather than the configured Message/Length
- **AZURE IOT Core**
  - Observed DUT after sending the data, its not sending the MQTT keep alive packet due to this Azure HUB closing the connection when power save is enabled.

**Wi-Fi Interoperability (IOP)**

- Observed disconnections with Amplifi (AFI-INS-R) AP with powersave enable
- TWT session is failing due to disconnections observed in DUT if rx\_latency is set to 55 seconds and receive data is also set to 55 seconds on MI Xiaomi RA72 and Tplink AX53 AP's
- Observed less throughput(~1Mb) while running TCP RX with Max\_PSP powersave with DLink 810 AP
- Observed interop issue (random disconnections) with few APs (EERO 6+, EERO PRO 6E, Cisco Catalyst 9120AXID)
- Disconnections observed with Netgear RAX120 AP in WPA3 Security

### **BLE**  

**GAP** 

- BLE reconnection is not working in ble\_multiconnection\_gatt demo when power save is enabled in SoC. 
- When power save is enabled in the SoC, the DUT hangs with the ble\_multiconnection\_gatt demo while performing continuous Tx/Rx notifications.

**DTM/PER**

- Recommend to limit BLE Tx Maximum power to 18 dBm.  Please don't use for 127 power\_index for BLE HP chain with this release.

**IOP**

- BLE connection failure has been observed on a few mobile devices (Samsung S23& Oneplus 9pro) when the Si917 device is configured as central.

**AE**

- Device is not transmitting AUX\_SCAN\_REQ  packets, when remote device is configured with scannable extended advertising mode.
- Device stopped transmitting AE advertising packets after sometime, when configured 'Two ADV sets one handle with public address and another with Random address and Primary PHY =1M and Sec PHY =1M/2M". 
- Device is not able to receive periodic advertising reports when  "BLE\_AE\_PERIODIC\_LIST\_USED" bit is enabled in the AE option of the AE Periodic create sync API.
- Device is not able to receive periodic advertising report when  "BLE\_AE\_PERODIC\_DUPLICATE\_FILTERING\_ENABLED" bit is enabled in AE option of the AE Periodic create sync API.

**Power** 

- BLE power numbers increased up to 25% compared to targeted power numbers. 

### **Multi-protocol**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Wi-Fi + BLE intermittent connection failures, disconnections, and data transfer stalls in the long run when power save is enabled.
- Observed "DUT is not disconnecting to the AP when initiating disconnection from EFR connect app screen using wifi\_station\_ble\_provisioning\_aws example
- Observed DUT failed to load certificate with error "0x10026" (SL\_STATUS\_SI91X\_WRONG\_PARAMETERS) while running wifi\_https\_ble\_dual\_role\_v6 application
- Observed BLE bonding failure during continuous HTTPS download.
- Observed BLE and WLAN connection failure with SMP, when WLAN connect and HTTPS GET called in a loop.

### **System**

- Observed random hang issues with encrypted firmwares on some earlier variant of boards  (Si917-6031A, Si917-4338A) with powersave enable.

### **Simplicity Studio and Commander** 

- Simplicity commander does not support options under "Debug Lock tools".
- All projects in the package are compatible with **GNU ARM V12.2.1** toolchain

## **Planned Roadmap Features**

### **SDK**

- WPS 2.0 PIN/PUSH, Network Manager Enhancements
- Websockets
- Matter integration with Embedded TCP/IP stack 
- Enhanced buffer management, Throughput optimization
- TLS over LWIP, Network layer applications (MQTT, HTTP, etc.) over LWIP.
- Debugging utilities, UC Configurator for Wireless configurations

### **System**

- Secure communication over CPC (UART & SDIO interfaces)

### **MCU**

- Pin Tool for MCU - Pin tool for Analog Peripherals (ADC, DAC. Analog Comparator)
- Dual Flash: Separate flash for Cortex M4 and Wireless Processor
- Peripherals: BOD, OPAMP, HSPI, GPDMA, QEI, RNG, IR decoder
- Software-configurable MCU application memory options (192 or 256 or 320 kB)
- RTOS support for MCU Peripheral examples
- SensorHub: Support for SDC\_ADC
- DMA flow control, self-loop mechanism and as a clock qualifier
- Support for I2S PCM functionality
- Stacked and external PSRAM: Pseudo Static RAM for storing application data and executing SoC applications. 
- Power Manager service enhancements
- Pintool GA quality and support to any upcoming peripheral

### **Wi-Fi/Network Stack**

- Spatial Re-Use, BSS Max Idle
- Provisioning using Wi-Fi AP

### **BLE**

- Support for 8 Peripheral + 2 Central connections 
- Support for BLE Mesh (4 nodes only) for limited Mesh use case

## **Limitations and Unsupported Features**  

### **System**

- None

### **SDK**

- Baremetal mode is not supported.
- WiSeConnect3\_SDK\_3.1.3 and later versions are not compatible with firmware versions prior to 1711.2.10.1.2.0.4, due to enhancements in max transmit power configuration during Wi-Fi join/connection, need to be cautious while doing OTA firmware upgrade.
- Caller must memset/initialize the structure variable to 0 before calling the sl\_wifi\_get\_wireless\_info() API. This will be taken care in the next release.

### **Wi-Fi/Network Stack**

- TLS 1.3 Server is not supported.
- 40 MHz bandwidth for 2.4 GHz band is not supported.
- Max 3 SSL sockets are supported in Wi-Fi alone and CoEx modes. No.of  SSL Sockets in Wi-Fi + BLE based on RAM memory configuration selected. 
- In SSL ECC Curve ID supported is 23. SSL handshake with 3rd party clients depends on the SSL ECC Curve ID.
- The number of Non-Transmitting BSSIDs processed is limited by the beacon length that can be processed by the stack (which is 1024 bytes). Beacons greater than 1024 Bytes in length will not be processed.
- Multiprotocol (STA +BLE) + EAP Security modes supported only with Memory configurations EXT\_FEAT\_416K\_M4SS\_256K and EXT\_FEAT\_480K\_M4SS\_192K.
- UL MUMIMO is not supported.
- WPA3 AP supports only H2E algorithm.
- PMKSA caching is not supported in WPA3 AP mode.
- Maximum embedded MQTT Publish payload is 1 kbyte.
- Timeout value for socket select and socket receive calls of less than 1 second is not currently supported.
- SA query procedure not supported in 11W AP mode.
- WPA3 AP transition mode is not supported.
- AP standalone mode does not support Tx aggregation. Rx aggregation is supported with limited number of BA sessions.
- In concurrent AP mode, aggregation (Tx/Rx) is not supported.
- Embedded HTTP Server is not supported.
- mDNS with IPV6 is not supported.
- Low power scan supports 1 mbps packets reception only.

### **BLE**

- For BLE, if the connection is established with a small connection interval (less than 15 ms), simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported.
- BLE maximum two concurrent connections are supported, which can be either a connection to two peripheral devices, to one central and one peripheral device or two central devices.
- BLE Slave latency value is valid up to 32 only.
- Maximum supported AE data length is 200 bytes.
- Supports only two ADV\_EXT sets.
- Supports only two BLE connections (1 Central and 1 Peripheral) with AE.
- Advertising Extension feature is not supported in Coexistence.
- The  ae\_central & ae\_peripheral applications are not supported with TA\_352K\_M4\_320K RAM configuration.
- Two BLE connections are not supported with M4 powersave. It only supports a single connection. 
- Isochronous channels feature is not supported. 
- Connection subrating feature is not supported. 
- LE power controller feature is not supported.
- EATT feature is not supported.
- Periodic Advertising with a response feature is not supported. 
- BLE Audio is not supported.
- The feature of dynamically changing the TX power when extended advertising is active is not supported.
- EFR Connect mobile application doesn't have support to differentiate the BLE configurators based on the Bluetooth Device address.
- The maximum BLE power has been reduced by 2dB compared to the Datasheet Number, which will be addressed in the upcoming 3.3.1 patch release.

### **MCU**

- SensorHub supports PS1-power state with ADC sensor. In this mode, the other sensor's operation is not supported
- Manual chip select option is not supported when using GSPI
- Following peripherals/features are not functioning as intended
  - ADC Multichannel, SSI Dual and Quad mode, SIO and Temp sensors
- LittleFS support is intended to be used only with Dual flash
- PS1 state is not supported in Power Manager
- Power is same for PS4 and PS3 states with powersave mode of power manager
- When using Button\_0 as wakeup source, it limits the button functionality at application layer. For using Button\_0 for application specific functionality, enable 'SL\_SI91X\_NPSS\_GPIO\_BTN\_HANDLER'
- Sometimes TA processor hang occurs when M4 uses XTAL clock with sleep-enabled

### **Multi-protocol**

- Wi-Fi AP + BLE currently not supported. 
- EXT\_FEAT\_352K\_M4SS\_320K RAM configuration is not supported for coex mode with SSL

> **Note:** 
> 
> The following BLE Synchronous API's will be deprecated soon and the equivalent Asynchronous API's will be used instead in all BLE applications :
>
> |**S.NO**|**BLE Synchronous API's** |**BLE Asynchronous API's** |
> | :- | :- | :- |
> |1|rsi\_ble\_get\_profiles|rsi\_ble\_get\_profiles\_async|
> |2|rsi\_ble\_get\_profile|rsi\_ble\_get\_profile\_async|
> |3|rsi\_ble\_get\_char\_services|rsi\_ble\_get\_char\_services\_async|
> |4|rsi\_ble\_get\_inc\_services|rsi\_ble\_get\_inc\_services\_async|
> |5|rsi\_ble\_get\_char\_value\_by\_uuid|rsi\_ble\_get\_char\_value\_by\_uuid\_async|
> |6|rsi\_ble\_get\_att\_descriptors|rsi\_ble\_get\_att\_descriptors\_async|
> |7|rsi\_ble\_get\_att\_value|rsi\_ble\_get\_att\_value\_async|
> |8|rsi\_ble\_get\_multiple\_att\_values|rsi\_ble\_get\_multiple\_att\_values\_async|
> |9|rsi\_ble\_get\_long\_att\_value|rsi\_ble\_get\_long\_att\_value\_async|
> |10|rsi\_ble\_set\_att\_value|rsi\_ble\_set\_att\_value\_async|
> |11|rsi\_ble\_set\_long\_att\_value|NA|
> |12|rsi\_ble\_prepare\_write|rsi\_ble\_prepare\_write\_async|
> |13|rsi\_ble\_execute\_write|rsi\_ble\_execute\_write\_async|
> |14|rsi\_ble\_indicate\_value\_sync|rsi\_ble\_indicate\_value|

# **WiSeConnect3\_SDK\_3.2.0 SoC Release Notes**

## **Release Details**

| **Item** | **Details** |
| :- | :- |
| Release date | May 7th, 2024 |
| SDK Version | 3.2.0 |
| Firmware Version | 1711.2.11.1.0.0.7 |
| GSDK Version | 4.4.3 |
| Release Package Name | WiSeConnect3\_SDK\_3.2.0 |
| Supported RTOS | FreeRTOS |
| Operating Modes Supported | Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP |

- SiWx917 release consists of two components:
  - Wireless Firmware - SiWx917 Firmware Binary available as SiWG917-B.2.11.1.0.0.7.rps
  - Wiseconnect3 Library - Wiseconnect3 SDK library runs on internal Cortex M4

**NOTE:** Mandatory to upgrade the earlier version of boards (Si917-6031A Prokit or BRD4338A boards) or 917 Silicon ICs with instructions as outlined in this document "SiWG917–TA_Flash_Memory_Map_ChangeGuide_v1.3.pdf" for more details.  

## **Supported Hardware**

| **Hardware** | **OPN** |
| :- | :- |
| Silicon (IC OPN) | SiWG917M111MGTBA, SiWG917M100MGTBA |
| Development Kits |<p>Pro Kit: SiWx917-PK6031A, Si917-PK6031A, SiWx917-PK6032A. </p><p>(Pro Kit includes Mother board "SI-MB4002A" + Radio board)</p><p>Radio boards: SiWx917-RB4338A, SiWx917-RB4342A </p>|

## **Supported Features**

### **System**

- **Operating Modes**
  - Wi-Fi STA (802.11ax, 802.11n), Wi-Fi 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + BLE
- **Security**
  - Secure Boot, Secure Key storage and HW device identity with PUF, Secure Zone, Secure XIP (Execution in place) from flash, Secure Attestation, Anti Rollback, Secure Debug and Flash Protection.
  - Secure firmware upgrade options:
    - Firmware loading support by Commander Tool through Jlink Debugger. Jlink connected to Serial Wire Debug (SWD)
    - Firmware loading via ISP using UART (Commander or Serial terminal), SPI Interface
    - Secure Over the Air (OTA) Upgrade
    - Firmware update via Bootloader
- **Crypto Support**
  -  Crypto API's for Hardware Accelerators:
    - Advanced Encryption Standard (AES) 128/256/192, Secure Hash Algorithm (SHA) 256/384/512, Hash Message Authentication Code (HMAC), Random Number Generator (RNG), SHA3, AES-Galois Counter Mode (GCM)/ Cipher based Message Authentication Code (CMAC), ChaCha-poly, True Random Number Generator (TRNG)
  - Software Accelerators: RSA, ECC
  - PSA Crypto APIs support for all crypto operations.
  - Wrapping Secret keys (Symmetric crypto).
  - Added ECDSA Sign and Verify APIs
- **System Power Save**
  - Deep Sleep with RAM retention and without RAM retention.
  - Wireless Power Save: Connected Sleep (Wi-Fi Standby Associated), BLE Advertising with powersave, BLE Scan with powersave ,  BLE connection with powersave. Only Max PSP power save mode is supported in BLE.

### **MCU**

- **Memory**
  - Common Flash: Single shared Flash for both Cortex-M4 and NWP (Wireless Processor)
  - Common Flash + External PSRAM
- **Power States**
  - Active: PS4, PS3, PS2, and PS1
  - Standby: PS4, PS3, and PS2
  - Sleep: PS4, PS3 and PS2
  - Deep Sleep(Shutdown): PS0
- **Peripherals**
  - High Performance (HP) - ADC, Calendar, Config Timer (CT), eFuse, EGPIO, GSPI, I2C, I2S, MCPWM, SDIO Secondary, SIO, SSI, SYSRTC, UART, uDMA, USART, WDT, Analog Comparator, DAC, Temperature Sensors (RO and BJT), Bod, CTS, OPAMP, PSRAM, QEI and RTC
  - Ultra Low Power (ULP) - ULP_TIMER, ULP_GPIO and ULP_CALENDAR,
  - UC Supported Peripherals - I2C, Calendar, SPI, SSI, SIO, uDMA, UART/USART, WDT, Config Timers, ULP Timers, SDIO Secondary, PWM, ADC, DAC, I2S and Analog Comparator
- **Services**
  - Sleep Timer, IOSTREAM, Power Manager, SensorHub, NVM3
  - The flash write feature has been enhanced to support the NWP area, providing a 20k allocation within the NWP flash memory for storing user data. Moreover, a Read API has been introduced to retrieve data from the NWP flash region.
- **Development Pro-Kit Peripheral Support**
  - LED, Button, MEMLCD, Joystick, RHT Sensor

### **Development Environment**

- Simplicity Studio IDE (SV5.8.0 version) and Debugger Integration. Refer to the latest version of the SoC "Getting-Started-with-SiWx917" guide for more details.
- Recommended to install and use Silicon labs Simplicity SDK(formerly Gecko SDK), Git hub based version 4.4.3.
- Simplicity Commander to supports Flash loading, provision of MBR programming, security key management, and calibration support for crystal and gain offsets. refer "siwx917-soc-manufacturing-utility-user-guide" for more details.
- Advanced Energy Monitoring (AEM) to measure ultra-low power capability on Development boards (Pro Kit).
- PinTool  Supported peripherals - I2C, GSPI, SSI, SIO, UART/USART, Config Timers, PWM, I2S, ADC, DAC and Analog comparator

### **Wi-Fi**

- **Wi-Fi Protocols**
  - IEEE 802.11 b/g/n/ax (2.4GHz)
- **Access Point (AP) Mode**
  - 4 Client Support, Hidden SSID Mode, Auto Channel Selection, Scan in AP mode (Alpha)
  - Wi-Fi Security  
    - WPA2 Personal, WPA3 Personal (H2E method only) (Alpha), WPA Mixed mode (WPA/WPA2)
- **Wi-Fi Scan**
  - Selective Scan, Active/Passive Scan (world domain)
- **Wi-Fi STA (Security Modes)**
  - Open Mode, WPA2 Personal, WPA2 Enhancements, WPA3 Personal, Mixed Mode (WPA/WPA2), WPA3 Personal Transition Mode (WPA2/WPA3)
- **WPA2 Enterprise security (STA)**
  - Method
    - PEAP/TTLS/TLS 1.0/TLS 1.2/FAST/LEAP
- **Wi-Fi STA Rejoin**
- **Wi-Fi STA Roaming**
  - BG Scan, OKC (Opportunistic Key caching), PMK (Pairwise Master Key) caching, Pre-Authentication
- **Wi-Fi Protocol Power Save**
  - Deep sleep (unconnected state), Max PSP, Enhanced Max PSP, Fast PSP, TWT
- **QoS**
  - WMM-QoS
- **Wi-Fi 6 Feature**
  - MUMIMO (DL), OFDMA (UL/DL), iTWT, TWT I-Frame & TWT Enhancements (Automatic TWT Configuration), BSS coloring, MBSSID
- **Wi-Fi Concurrency**
  - AP+STA (Same channel)
- **Wi-Fi Band/Channels**
  - 2.4GHz CH1-11, 2.4GHz CH1-13, 2.4GHz CH1-14
- **Known Security Vulnerabilities Handled**
  - WPA2 KRACK Attacks, Fragment and Forge Vulnerability

### **Network Stack**

- **Core Networking Features**
  - IPv4/IPv6/UDP/TCP/ARP/ICMP/ICMPv6
  - SSL client versions TLSV1.0, TLSV1.2, TLSV1.3
  - SSL server versions TLSV1.0 and TLSV1.2
  - DHCPv4 (Client/Server)/ DHCPv6 Client
  - TCP/IP Bypass (LWIP as Hosted stack for reference),
- **Advanced Network Features**
  - HTTP Client/HTTPS Client//DNS Client/SNTP Client, Embedded MQTT/MQTT on host (AWS and AZURE)
- **Wi-Fi IoT Cloud Integration**
  - AWS IoT Core
  - Azure IoT
- BSD and IoT sockets application programming interface(API)

### **BLE**

- GAP(Advertising, Scanning, initiation, Connection and Bonding)
- Generic Attribute Protocol(GATT)
- Attribute protocol(ATT)
- Security
- LL Privacy 1.2
- Accept list
- Directed Advertising
- Extended Advertising
- Periodic Advertising
- Periodic Advertising scanning
- Extended Advertising scanning
- Periodic Advertising list
- LE periodic advertising synchronization
- LE PHY(1Mbps, 2Mbps) & Coded PHY(125Kbps, 500kbps)
- Simultaneous scanning on 1Mbps and Coded PHY
- LE dual role topology
- LE data packet length extensions(DLE)
- Asymmetric PHYs
- LE channel selection algorithm 2 (CSA#2)
- LE Secure connections
- Bluetooth 5.4 Qualified

### **SDK**

- Simplified and Unified DX for Wi-Fi API and Platform APIs
- Simplifies application development and presents clean and standardized APIs
- UC (Universal Configurator) enables componentization, simplifying configuration of peripherals and examples
- BSD and ARM IoT-compliant socket API
- Available through Simplicity Studio and GitHub

### **Multi-protocol**

- Wi-Fi STA + BLE

### **PTA/Coexistence**

- 3 wire coex acting as Wi-Fi with external Bluetooth
- 3 wire coex acting as Wi-Fi with external Zigbee/OT

## **Changes in this release compared to v3.1.4 Release**

### **System**

- Fixed M4 FW update(OTAF) issue for 1.8MBR

### **MCU**

- **Enhancements**
  - Implemented I2C 7 or 10-bit combined format transfers
  - Integrated SSI with SL DMA
  - Updated documentation and UC for Consistency - API Documentation, Readme, SRM and UC
  - Added an empty C project for SoC
  - Renamed below UART macro's in RTE_Device_917.h file
    - RTE_UART1_FRAC_DIV_EN changed to RTE_UART1_FRAC_DIV_SEL
    - RTE_USART0_FRAC_DIV_EN changed to RTE_USART0_FRAC_DIV_SEL
- **Fixed Issues**
  - Fixed SYSRTC application serial console prints documentation issue in readme
  - The issue with separate channel selection in the pwm application has been resolved and updated the associated readme documentation.
  - The issue with pwm_start not working after pwm_stop in pwm application has been resolved.
  - Resolved PWM toggles stopping issue when BTN0 is pressed in Combo app
  - Fixed the transfer for ULP SSI Master DMA issue
  - Resolved SSI DMA issue when buffer size is 10000
  - Fixed SSI functionality issue when GSPI component is added to project
  - Fixed UART Fractional clock divisor value issue
  - Resolved build error issue while adding IOSTREAM components in DAC example
  - Resolved UDMA and RAM memory overlapping issue in default examples
  - Resolved issues observed with 1 byte I2C read
  - Resolved RAM retention issues in Smart Lock application with Power Manager
  - Fixed Power Manager component issues observed in SDK v3.1.4
  - Resolved problems in peripheral UC configuration.
  - Fixed peripheral documentation links

### **SDK**

- **Enhancements**
  - Added sl_si91x_mem_pool_buffers_with_quota component
  - API reference guide enhancement with a callback usage note
  - Readme enhancements of the example application
  - Added support for wifi_data_transfer_application_soc, wifi_ap_throughput_soc, wifi_https_ble_dual_role_soc, flash_read_write  example application
- **Fixed Issues**
  - Fixed TCP RX failure in concurrent mode in multithreading_application example
  - Fixed RPS header corruption during firmware update
  - Fixed the issue of DUT returning SL_STATUS_TIMEOUT after multiple iterations during wifi init and deinit sequence
  - Fixed handling of EAP-FAST PAC file loading manually.

### **Wi-Fi/Network Stack**

- **Enhancements**
  - Added long url support (max 2048 bytes) for HTTP OTAF.
  - Removed a limitation in Embedded where MQTT keep alive interval should be either 0 or a value greater than 35 seconds. Now allowing keep alive interval less than 35 seconds also.
  - Added support for LEAP security in EAP mode
  - For Flash less parts support added to update customer gain offset and xo_ctune offset to external flash.
- **Fixed Issues**
  - Fixed issues with Embedded MQTT state machine, when remote terminate and WLAN disconnection is triggered
  - Fixed  issues with MDNS services not responding to the queries from other devices
  - Fixed same MAC address being generated for AP and Station address incase of concurrent mode.
  - Fixed the issue where DUT is getting hang while running HTTP_PUT Method in http client application
  - Fixed the issue where sl_net_set_credential API is failed with 0x10026 (SL_STATUS_SI91X_WRONG_PARAMETERS) error when trying to load HTTP certificate(SL_NET_HTTP_CLIENT_CREDENTIAL_ID) for HTTPs
  - Added hostname support to HTTP client request
  - Fixed issue where http otaf firmware update failed with "BBE1(SL_STATUS_SI91X_HTTP_TIMEOUT) error" for HTTP and BBD6(SL_STATUS_SI91X_SSL_TLS_HANDSHAKE_TIMEOUT_OR_FTP_FILE_NOT_CLOSED) for HTTPS otaf
  - Fixed issue where failure/no response during the 2nd iteration of HTTP Server response
  - Fixed DUT hang issue which is caused because of HW calibrations.
  - Fixed xo captune calibration issue. Issue was xo captune getting overwritten with default value.
  - Fixed the issue of module not entering into deep sleep after AP stop API call.
  - Fixed an issue of AP VAP initialization in the scenario of AP-start, AP-stop and AP-start again.
  - Fixed issues with static IPV4 assignment to DUT.
  - Fixed issues with ECDSA ciphers.

### **BLE**

- **Enhancements**
  - Added "bt_stack_bypass" demo for validating Direct test mode in SOC.
  - Added change to obtain 6 bytes Bluetooth device address when the "rsi_bt_get_local_device_address" API is called in all BLE applications.
- **Fixed Issues**
  - Fixed BLE disconnection issue while running wifi_https_ble_dual_role application when powersave is disabled.
  - Resolved the issue with BLE PER stats cmd API.
  - Fixed reconnection issue in case of pairing failure.
  - Fixed invalid connection request(CVE-2019-19193) vulnerability.
  - Updated readme files for "ble_accept_list" and "ble_testmodes" applications.
  - Updated "tx_power" description for HP mode in "rsi-ble-per-transmit-s" structure.
  - Fixed issue where WLAN power save API is failing with error code: SL_STATUS_TIMEOUT while executing the ble_ae_central application

### **Multi-protocol**

- **Enhancements**
  - NA
- **Fixed Issues**
  - NA

## **Recommendations**

### **System**

- The current revision of SiWx917 has:
  - RAM memory of 672k bytes which can be shared between TA and M4 processors in SoC mode.
  - The below configurations are applicable in SoC mode and can be configured based on the application requirement. EXT_FEAT_352K_M4SS_320K is the default configuration, based on requirement EXT_FEAT_480K_M4SS_192K configuration is selected for SoC mode multi-protocol examples.
    - EXT_FEAT_480K_M4SS_192K - This mode configures TA with 480k and M4 with 192K bytes of memory
    - EXT_FEAT_416K_M4SS_256K - This mode configures TA with 416k and M4 with 256K bytes of memory
    - EXT_FEAT_352K_M4SS_320K - This mode configures TA with 352k and M4 with 320K bytes of memory
  - SoC mode should not use 672k_M4SS_0K memory configuration.
- Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
- There are 2 Versions of Pro-Kits/Radio boards. Si917-6031A based on Si917-4338A (Rev **A01 - A11**) and SiWx917-6031A based on SiWx917-4338A (Rev A12). To get optimal power numbers, enable macro "SL_SI91X_ENABLE_LOWPWR_RET_LDO" pre-processor define for ICs or while using SiWx917-6031A Pro-kit, SiWx917-4338A version of boards. This macro should be disabled for earlier variant of board (Si917-6031A, Si917-4338A).
- With RAM configuration (EXT_FEAT_352K_M4SS_320K), only 352K memory is available to TA  which limits the features supported, Recommended to enable EXT_FEAT_416K_M4SS_256K in Wi-Fi + BLE Multi protocol mode to enable more Network features.
- For EXT_FEAT_416K_M4SS_256K  and EXT_FEAT_480K_M4SS_192K memory configurations, it is recommended to retain both TA and M4 RAMs in power save.

### **Wi-Fi/Network Stack**

- It is recommended to enable bit 16 of the 'Extended TCP IP Feature' bit map in the opermode command for all Wi-Fi Socket operations from the host to ensure graceful handling during asynchronous closures from the peer.
- For high throughput applications, aggregation (bit 2 of feature_bit_map) is recommended to be enabled in opermode.
- Users can enable SL_SI91X_EXT_TCP_IP_SSL_16K_RECORD in 'Extended TCP IP Feature' bit map in opermode for (HTTPS server) supporting 16k record.
- **TWT**
  - Recommendation is to use sl_wifi_target_wake_time_auto_selection() API for all TWT applications.
  - It is recommended to issue iTWT setup command once IP assignment, TCP connection, application specific socket connections are done.
  - When using sl_wifi_enable_target_wake_time API, increase TCP / ARP Timeouts at the remote side depending upon the configured TWT interval configured. It's highly recommended to use sl_wifi_target_wake_time_auto_selection() as an alternative.
  - In case of TWT in coex mode, when using sl_wifi_enable_target_wake_time API, use TWT wake duration &lt;= 16 ms and TWT wake interval &gt;= 1 sec. If wake duration > 16 ms or TWT wake interval < 1sec, there might be performance issues.
  - For iTWT GTK interval in AP should be configured to max possible value or zero. If GTK interval is not configurable on AP side, recommended TWT interval (in case of sl_wifi_enable_target_wake_time API) or RX Latency (in case of sl_wifi_target_wake_time_auto_selection API) is less than 4sec.
  - When sl_wifi_enable_target_wake_time API is used, configuring TWT Wake interval beyond 1 min might lead to disconnections from the AP. Recommended to use TWT wake interval of less than or equal to 1 min.
  - When using sl_wifi_enable_target_wake_time API, it is recommended to set missed_beacon_count of sl_wifi_set_advanced_client_configuration API greater than 2 times of the configured TWT Interval.
- Disable power save for high throughput applications or use FAST PSP power save mode as per application requirement.
- The application needs to ensure that it sets RTC with the correct timestamp before establishing the SSL/EAP connection.
- The minimum timeout value should not be less than 1 second for socket select and socket receive calls.
- Please refer Keep alive intervals supported by MQTT broker and configure keep alive interval values accordingly.
- The minimum keep alive interval value recommended for embedded MQTT is 10 Seconds.
- Disable power save and suspend any active TWT sessions before triggering HTTP OTAF.
- Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with the power save.
- Recommended to configure VAP_ID properly for Si91x STA and AP using sl_si91x_setsockopt_async(), in case of data transfer.
- Recommended to use valid length(<= 202 bytes) for topic to be published while using Embedded MQTT, else it leads to return wrong error code(0x21).

### **BLE**

- In BLE, the recommended range of Connection Interval in
  - Power Save (BLE Only) - 100 ms to 1.28 s.
- In BLE, during Connection, the configuration of Scan Interval and Scan Window with the same value is not recommended. The suggested ratio of Scan Window to Scan Interval is 3:4.
- In BLE, if a device is acting as Central, the scan window (in set_scan_params and create_connection commands) must be less than the existing Connection Interval. The suggested ratio of Scan Window to Connection Interval is 2:3.
- In BLE mode, if scanning and advertising are in progress on the SiWx91x module and it subsequently gets connected and moves to the central role, scanning stops else if it moves to the peripheral role, advertising stops. To further establish a connection to another peripheral device or to a central device, the application should give a command for starting advertising and scanning again.

### **Multi-protocol**

- For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
- Wi-Fi + BLE Advertising
  - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals may be observed.
  - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity.
- Wi-Fi + BLE scanning
  - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
  - BLE scanning will be stopped for intervals that collide with Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Connections
  - All standard connection intervals are supported.
  - For a stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Data Transfer
  - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout.
  - Ensure Wi-Fi activity consumes lower intervals.

## **Known Issues of WiSeConnect3_SDK_3.2.0 Release**

### **MCU**

**SensorHub**

- Power transitions with AWS are not stable in SensorHub
- Support for ADC FIFO Mode and Multi channel is open
- PS1-state supports only ADC senso

**SSI**

- SSI Dual and Quad mode are not functional
- SSI Primary: Mode3 and TI are not working as expected
- SSI Primary will not be operable at 40 MHz
- SSI Full Duplex in DMA mode is not working as expected

**I2C**

- ULP I2C High speed and Fast plus modes are not working at specified speed

**UART**

- Flow control is not supported in ULP UART.

**ADC**

- In ADC Multichannel mode, sampling rate is limited to less than 15Ksps. Number of samples is less than 500 and with a maximum of 4 channel
- In ADC Multichannel mode, when using different sampling rates for different channels behavior is not as expected

**CT (Config Timer)**

- Config Timer(CT) support as an external event, in 32-bit mode, and DMA are not working as expected

**Power Manager**

- The wakeup sources BOD, Comparator, and SYSRTC are not working in the Power Manager service

**RO Clock**

- Few peripherals like WDT, SYSRTC when configured with RO clock is not working as expected. No impact on the feature but the clocks are not accurate.

**ULP Peripheral examples**

- ULP Peripheral examples (ULP_ADC, ULP_ADC_MULTICHANNEL, ULP_I2S, ULP_SSI_MASTER, ULP_UART, ULP_DMA, ULP_I2C_LEADER, ULP_DAC, and ULP_I2C_DRIVER_LEADER) are being integrated with the Power Manager Service, ULP Peripheral examples without Power Manager are removed from SDK and available through Apps repo for [reference](https://github.com/SiliconLabs/wifi_combo_applications/tree/master/SiWx917).

### **SDK**

- Observed Wi-Fi connection is successful even after deleting the stored network credentials using sl_net_delete_credential and responding with SL_NET_INVALID_CREDENTIAL_TYPE for sl_net_get_credential.
- Enhanced sl_wifi_get_firmware_version() API to provide more details (ROM ID, chip ID, security version, etc) which is not backward compatible with firmware older than 1711.2.10.1.0.0.4. Firmware binary notation does not include the security version number.
- In PSRAM enabled demos, moving of text, data and stack segments to PSRAM is allowed. BSS and Heap should still be in SRAM.
- Asynchronous Azure MQTT is not supported, this will be addressed in upcoming release(s).
- mDNS with IPV6 is not supported.
- Observing DUT is getting stuck while running MQTT publish with 2 different topics
- Observed MQTT RX is failing when continuous data sent every 1 second in aws_mqtt application
- Observed DUT Failed to connect to AP with timeout error 0x7(SL_STATUS_TIMEOUT)  while single MQTT subscribe and publish in loop with embedded MQTT application
- Observing LAST_WILL_MESSAGE is random at every MQTT connection rather than the configured Message/Length with embedded MQTT application
- Observed DUT is not initiating the AWS connection post-rejoin failure indication after publishing 10-15 packets, turning OFF AP and trying to rejoin the network with aws_mqtt application
- Observing MQTT disconnect API is getting failed with 0x7(SL_STATUS_TIMEOUT) error while doing publish & subscribe with embedded MQTT application
- Bus thread stack may need to increase if local variables are used in user callback to avoid stack overflow.
- Low Power examples usage and documentation still under scope of improvement.

### **Wi-Fi/Network Stack**

**Wi-Fi STA**

- STA Connection with the WPA3 Hunting and Pecking algorithm takes about 3-4 seconds.
- Observed connection failures with some AP's in more channel congestion ~50-60% occupancy open lab environment.
- Region selection based on country IE in the beacon is not supported
- Observed intermittent beacon reception from Access Point ( beacon misses) when channel congestion > 85%.

**Access Point (AP) Mode**

- Scan feature in AP mode not fully functional.
- Fixed rate configuration in AP mode using sl_wifi_set_transmit_rate API is not being set as expected.

**WPA2 Enterprise security (STA)**

- Observed connection issue with configuring certificate key and programming 4096 bit key and SHA384/SHA512 certificates.
- Observing DUT is throwing 0x1001c when configuring .data.certificate_key as "123456789".
- Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.

**Wi-Fi Concurrency (AP + STA in same channel)**

- Observed 3rd party STA association fail with 917 AP while 917 STA mode is connecting/reconnecting to configured 3rd party AP. Reconnect 3rd party STA to 917 AP in such scenarios.

**OFDMA (UL/DL)**

- Less throughput observed in DL-OFDMA with some APs that enabled Low density parity check coding.

**MUMIMO (DL)**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test.
- Observed Performance, Interop issues with MU MIMO with certain APs.
- Less throughput observed in MU-MIMO with some APs that enabled LDPC.

**MU-MIMO (UL)**

- UL MU-MIMO is not supported.

**TWT**

- When sl_wifi_enable_target_wake_time() API is used, occasional MQTT disconnections may be observed if TWT is configured with longer TWT intervals (>30secs) with embedded MQTT + TWT.  As an alternative, it's highly recommended to use sl_wifi_target_wake_time_auto_selection() API, where these dependencies are internally handled.

**Wi-Fi STA Rejoin**

- Observed Scanning (probe request) in all channels instead of the channels configured in selective channel(channel_bitmap_2g4) during rejoin process.

**IPv4/IPv6**

- IP change notification is not indicated to the application.

**BSD Socket API**

- Every server socket created consumes a socket (maximum of 10 sockets supported) and every subsequent connection to server socket consumes an additional socket (from the same pool of 10 sockets), which limits the number of server connections supported.
- Observed socket shutdown is not happening and DUT is getting stuck at  "prvCheckTasksWaitingTermination" while doing socket connection-> data transfer ->  socket shut down for multiple iterations.
- Observed DUT is not sending TCP retry packets on air when configured through setsockopt().

**SSL Client/Server**

- Sometimes during SSL Handshake, ECC curve parameters generated are wrong, resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
- Secure SSL renegotiation is not supported in the Embedded Networking Stack

**HTTP Client/ HTTPS Client**

- Observed occasional HTTPS continuous download failures when power save is enabled. Recommended to disable it before performing HTTPS continuous downloads
- Observed HTTPS post data is failing with error code: BBE2 (HTTP Failed).

**SNTP**

- Unable to get SNTP async events when Coex mode and power save are enabled

**Throughputs & Performance**

- Wi-Fi alone throughput is about SDK 3.x (42Mbps). SDK refinements are in progress to further improve Wi-Fi Standalone and Coex Throughputs.

**Wi-Fi IOT Cloud integration**

-  **AWS IOT Core**
  - Observed AWS MQTT keepalive transmission is not happening at expected intervals with power save enabled.
- **AZURE IOT Core**
  - Observed DUT after sending the data, its not sending the MQTT keep alive packet due to this Azure HUB closing the connection.

**Wi-Fi Interoperability (IOP)**

- Observed disconnections with Amplifi (AFI-INS-R) AP with powersave enable
- TWT session is failing due to disconnections observed in DUT if rx_latency is set to 55 seconds and receive data is also set to 55 seconds on MI Xiaomi RA72 and Tplink AX53 AP's
- Observed less throughput(~1Mb) while running TCP RX with Max_PSP powersave with DLink 810 AP
- Observed interop issue (random disconnections) with few APs (EERO 6+, EERO PRO 6E, Cisco Catalyst 9120AXID)
- Disconnections observed with Netgear RAX120 AP in WPA3 Security

### **BLE**

**GAP**

- BLE reconnection is not working in ble_multiconnection_gatt demo when power save is enabled in SoC.
- When power save is enabled in the SoC, the DUT hangs with the ble_multiconnection_gatt demo while performing continuous Tx/Rx notifications.

**DTM/PER**

- Recommend to limit BLE Tx Maximum power to 18 dBm.  Please don't use for 127 power_index for BLE HP chain with this release.

**IOP**

- BLE connection failure has been observed on a few mobile devices (Samsung S23& Oneplus 9pro) when the Si917 device is configured as central.

### **Multi-protocol**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test.
- Observed Wi-Fi + BLE intermittent connection failures, disconnections, and data transfer stalls in the long run when power save is enabled.
- Observed "DUT is not disconnecting to the AP when initiating disconnection from EFR connect app screen using wifi_station_ble_provisioning_aws example
- Observed DUT failed to load certificate with error "0x10026" (SL_STATUS_SI91X_WRONG_PARAMETERS) while running wifi_https_ble_dual_role_v6 application

### **System**

- Observed random hang issues with encrypted firmwares on some earlier variant of boards  (Si917-6031A, Si917-4338A) with powersave enable.

### **Simplicity Studio and Commander**

- Simplicity commander does not support options under "Debug Lock tools".
- All projects in the package are compatible with **GNU ARM V12.2.1** toolchain

## **Planned Roadmap Features**

### **SDK**

- WPS 2.0 PIN/PUSH, Network Manager Enhancements
- Websockets
- Matter integration with Embedded TCP/IP stack
- Enhanced buffer management, Throughput optimization
- TLS over LWIP, Network layer applications (MQTT, HTTP, etc.) over LWIP.
- Debugging utilities, UC Configurator for Wireless configurations

### **System**

- Secure communication over CPC (UART & SDIO interfaces)

### **MCU**

- Pin Tool for MCU - Pin tool for Analog Peripherals (ADC, DAC. Analog Comparator)
- Dual Flash: Separate flash for Cortex M4 and Wireless Processor
- Peripherals: Capacitive Touch Sensor, OPAMP, HSPI, GPDMA, QEI, RNG, IR decoder, CRC and BOD
- Software-configurable MCU application memory options (192 or 256 or 320 kB)
- RTOS support for MCU Peripheral examples
- SensorHub: Support for SDC_ADC and CLOUD connectivity
- SIO support for features Pattern matching, DMA flow control, self-loop mechanism and as a clock qualifier
- Support for I2S PCM functionality
- Stacked and external PSRAM: Pseudo Static RAM for storing application data and executing SoC applications.

### **Wi-Fi/Network Stack**

- Spatial Re-Use, BSS Max Idle
- Provisioning using Wi-Fi AP

### **BLE**

- Support for 8 Peripheral + 2 Central connections
- Support for BLE Mesh (4 nodes only) for limited Mesh use case

## **Limitations and Unsupported Features**  

### **System**

- None

### **SDK**

- Baremetal mode is not supported.
- WiSeConnect3_SDK_3.1.3 and later versions are not compatible with firmware versions prior to 1711.2.10.1.2.0.4, due to enhancements in max transmit power configuration during Wi-Fi join/connection, need to be cautious while doing OTA firmware upgrade.

### **Wi-Fi/Network Stack**

- TLS 1.3 Server is not supported.
- 40 MHz bandwidth for 2.4 GHz and 5 GHz band is not supported.
- Max 3 SSL sockets are supported in Wi-Fi alone and CoEx modes. No.of  SSL Sockets in Wi-Fi + BLE based on RAM memory configuration selected.
- In SSL ECC Curve ID supported is 23. SSL handshake with 3rd party clients depends on the SSL ECC Curve ID.
- The number of Non-Transmitting BSSIDs processed is limited by the beacon length that can be processed by the stack (which is 1024 bytes). Beacons greater than 1024 Bytes in length will not be processed.
- Multiprotocol (STA +BLE) + EAP Security modes supported only with Memory configurations EXT_FEAT_416K_M4SS_256K and EXT_FEAT_480K_M4SS_192K.
- UL MUMIMO is not supported.
- WPA3 AP supports only H2E algorithm.
- PMKSA caching is not supported in WPA3 AP mode.
- Maximum embedded MQTT Publish payload is 1 kbyte.
- Timeout value for socket select and socket receive calls of less than 1 second is not currently supported.
- SA query procedure not supported in 11W AP mode.
- WPA3 AP transition mode is not supported.
- In Concurrent mode (AP/STA) aggregation (Tx and Rx) is not supported.
- AP standalone mode does not support Tx aggregation. Rx aggregation is supported with limited number of BA sessions.
- Embedded HTTP Server is not supported.
- mDNS with IPV6 is not supported.

### **BLE**

- For BLE, if the connection is established with a small connection interval (less than 15 ms), simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported.
- BLE maximum two concurrent connections are supported, which can be either a connection to two peripheral devices, to one central and one peripheral device or two central devices.
- BLE Slave latency value is valid up to 32 only.
- Maximum supported AE data length is 200 bytes.
- Supports only two ADV_EXT sets.
- Supports only two BLE connections (1 Central and 1 Peripheral) with AE.
- Advertising Extension feature is not supported in Coexistence.
- The  ae_central & ae_peripheral applications are not supported with TA_352K_M4_320K RAM configuration.
- Two BLE connections are not supported with M4 powersave. It only supports a single connection.
- Isochronous channels feature is not supported.
- Connection subrating feature is not supported.
- LE power controller feature is not supported.
- EATT feature is not supported.
- Periodic Advertising with a response feature is not supported.
- BLE Audio is not supported.
- The feature of dynamically changing the TX power when extended advertising is active is not supported.
- EFR Connect mobile application doesn't have support to differentiate the BLE configurators based on the Bluetooth Device address.

### **MCU**

- SensorHub supports PS1-power state with ADC sensor. In this mode, other sensors operation is not supported.

### **Multi-protocol**

- Wi-Fi AP + BLE currently not supported.
- EXT_FEAT_352K_M4SS_320K RAM configuration is not supported for coex mode with SSL

<br>

# **WiSeConnect3\_SDK\_3.1.4 SoC Release Notes**

## **Release Details**

|**Item**|**Details**|
| :- | :- |
|Release date|Mar 14th, 2024|
|SDK Version|3\.1.4|
|Firmware Version|1711\.2.10.1.3.0.7|
|Release Package Name|WiSeConnect3\_SDK\_3.1.4|
|Supported RTOS|FreeRTOS|
|Operating Modes Supported|Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP|

- SiWx917 release consists of two components:
  - Wireless Firmware - SiWx917 Firmware Binary available as SiWG917-B.2.10.1.3.0.7.rps
  - Wiseconnect3 Library - Wiseconnect3 SDK library runs on internal Cortex M4

**NOTE:** Mandatory to upgrade the earlier version of boards (Si917-6031A Prokit or BRD4338A boards) or 917 Silicon ICs with instructions as outlined in this document "SiWG917–TA\_Flash\_Memory\_Map\_ChangeGuide\_v1.3.pdf" for more details.  

## **Supported Hardware**

|**Hardware**|**OPN**|
| :- | :- |
|Silicon (IC OPN)|SiWG917M111MGTBA, SiWG917M111XGTBA, SiWG917M141XGTBA, SiWG917M121XGTBA, SiWG917M110LGTBA, SiWG917M100MGTBA|
|Development Kits|<p>Pro Kit: SiWx917-PK6031A, Si917-PK6031A, SiWx917-PK6032A. </p><p>(Pro Kit includes Mother board "SI-MB4002A" + Radio board)</p><p>Radio boards: SiWx917-RB4338A, SiWx917-RB4342A </p>|

## **Supported Features** 

### **System**

- **Operating Modes** 
  - Wi-Fi STA (802.11ax, 802.11n), Wi-Fi 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + BLE
- **Security** 
  - Secure Boot, Secure Key storage and HW device identity with PUF, Secure Zone, Secure XIP (Execution in place) from flash, Secure Attestation, Anti Rollback, Secure Debug and Flash Protection. 
  - Secure firmware upgrade options:
    - Firmware loading support by Commander Tool through Jlink Debugger. Jlink connected to Serial Wire Debug (SWD) 
    - Firmware loading via ISP using UART (Commander or Serial terminal), SPI Interface
    - Secure Over the Air (OTA) Upgrade
- **Crypto Support**
  - Crypto API's for Hardware Accelerators:
    - Advanced Encryption Standard (AES) 128/256/192, Secure Hash Algorithm (SHA) 256/384/512, Hash Message Authentication Code (HMAC), Random Number Generator (RNG), SHA3, AES-Galois Counter Mode (GCM)/ Cipher based Message Authentication Code (CMAC), ChaCha-poly, True Random Number Generator (TRNG)
  - Software Accelerators: RSA, ECC 
  - PSA Crypto APIs support for all crypto operations.
  - Wrapping Secret keys (Symmetric crypto). 
- **System Power Save**
  - Deep Sleep with RAM retention and without RAM retention. 
  - Wireless Power Save: Connected Sleep (Wi-Fi Standby Associated), BLE Advertising with powersave, BLE Scan with powersave ,  BLE connection with powersave. Only Max PSP power save mode is supported in BLE. 

### **MCU**

- **Memory**
  - Common Flash: Single shared Flash for both Cortex-M4 and NWP (Wireless Processor)
  - Common Flash + External PSRAM
- **Power States**
  - Active: PS4, PS3, PS2, and PS1 
  - Standby: PS4, PS3, and PS2
  - Sleep: PS4, PS3 and PS2
  - Deep Sleep(Shutdown): PS0
- **Peripherals**
  - High Performance (HP)- ADC, Calendar, Config Timer(CT), eFuse, EGPIO, GSPI, I2C, I2S, MCPWM, SDIO Secondary, SIO, SSI, SYSRTC, UART, uDMA, USART, WDT, Analog Comparator, DAC and Temperature Sensors(RO and BJT), Bod, CTS, OPAMP, PSRAM, QEI and RTC** 
  - Ultra Low Power (ULP) - ULP\_TIMER, ULP\_UART, ULP\_I2C, ULP\_I2S, ULP\_uDMA, ULP\_SSI, ULP\_GPIO, ULP\_ADC, ULP\_IR\_DECODER and ULP\_SPI
  - UC Supported Peripherals - I2C, Calendar, SPI, SSI, SIO, uDMA, UART/USART, WDT, Config Timers, ULP Timers, SDIO Secondary, PWM, ADC, DAC, I2S and Analog Comparator
- **Services**
  - Sleep Timer, IOSTREAM, Power Manager, SensorHub, NVM3
  - The flash write feature has been enhanced to support the NWP area, providing a 20k allocation within the NWP flash memory for storing user data. Moreover, a Read API has been introduced to retrieve data from the NWP flash region.
- **Development Pro-Kit Peripheral Support**
  - LED, Button, MEMLCD, Joystick, RHT Sensor

### **Development Environment**

- Simplicity Studio IDE (SV5.8.0 version) and Debugger Integration.   Refer to the latest version of the SoC "Getting-Started-with-SiWx917" guide for more details.
- Recommended to install and use Silicon labs Simplicity SDK(formerly Gecko SDK), Git hub based version 4.4.1. 
- Simplicity Commander to supports Flash loading, provision of MBR programming, security key management, and calibration support for crystal and gain offsets. refer "siwx917-soc-manufacturing-utility-user-guide" for more details.** 
- Advanced Energy Monitoring (AEM) to measure ultra-low power capability on Development boards (Pro Kit).
- PinTool  Supported peripherals - I2C, GSPI, SSI, SIO, UART/USART, Config Timers, PWM, I2S

### **Wi-Fi**

- **Wi-Fi Protocols**
  - IEEE 802.11 b/g/n/ax (2.4GHz)
- **Access Point (AP) Mode**
  - 4 Client Support, Hidden SSID Mode, Auto Channel Selection, Scan in AP mode (Alpha Version)
  - Wi-Fi Security  
    - WPA2 Personal, WPA3 Personal (H2E method only) (Alpha Version), WPA Mixed mode (WPA/WPA2) 
- **Wi-Fi Scan**
  - Selective Scan, Active/Passive Scan
- **Wi-Fi STA (Security Modes)**
  - Open Mode, WPA2 Personal, WPA2 Enhancements, WPA3 Personal, Mixed Mode (WPA/WPA2), WPA3 Personal Transition Mode (WPA2/WPA3)
- **WPA2 Enterprise security (STA)**
  - Method
    - PEAP/TTLS/TLS 1.0/TLS 1.2/FAST
- **Wi-Fi STA Rejoin**
- **Wi-Fi STA Roaming**
  - BG Scan, OKC (Opportunistic Key caching), PMK (Pairwise Master Key) caching, Pre-Authentication
- **Wi-Fi Protocol Power Save** 
  - Deep sleep (unconnected state), Max PSP, Enhanced Max PSP, Fast PSP,** TWT
- **QoS**
  - WMM-QoS
- **Wi-Fi 6 Feature**
  - MUMIMO (DL), OFDMA (UL/DL), iTWT, TWT I-Frame & TWT Enhancements, BSS coloring, MBSSID
- **Wi-Fi Concurrency** 
  - AP+STA (Same channel)
- **Wi-Fi Band/Channels**
  - 2.4GHz CH1-11, 2.4GHz CH1-13, 2.4GHz CH1-14 (Japan)
- **Known Security Vulnerabilities Handled**
  - WPA2 KRACK Attacks, Fragment and Forge Vulnerability

### **Network Stack**

- **Core Networking Features**
  - TCP/IP Bypass (LWIP as Hosted stack for reference), IPv4/IPv6/UDP/TCP/ARP/ICMP/ICMPv6
  - SSL client versions TLSV1.0, TLSV1.2, TLSV1.3 
  - SSL server versions TLSV1.0 and TLSV1.2
  - DHCPv4 (Client/Server)/ DHCPv6 Client
- **Advanced Network Features**
  - HTTP Client/HTTPS Client//DNS Client/SNTP Client, Embedded MQTT/MQTT on host
- **Wi-Fi IoT Cloud Integration**
  - AWS IOT Core
- BSD and IoT sockets application programming interface(API)

### **BLE** 

- GAP(Advertising, Scanning, initiation, Connection and Bonding)
- Generic Attribute Protocol(GATT)
- Attribute protocol(ATT)
- Security
- LL Privacy 1.2
- Accept list
- Directed Advertising
- Extended Advertising
- Periodic Advertising
- Periodic Advertising scanning
- Extended Advertising scanning
- Periodic Advertising list
- LE periodic advertising synchronization
- LE PHY(1Mbps, 2Mbps) & Coded PHY(125Kbps, 500kbps)
- Simultaneous scanning on 1Mbps and Coded PHY
- LE dual role topology
- LE data packet length extensions(DLE)
- Asymmetric PHYs
- LE channel selection algorithm 2 (CSA#2)
- LE Secure connections
- Bluetooth 5.4 Qualified

### **SDK**

- Simplified and Unified DX for Wi-Fi API and Platform APIs
- Simplifies application development and presents clean and standardized APIs
- UC (Universal Configurator) enables componentization, simplifying configuration of peripherals and examples
- BSD and ARM IoT-compliant socket API
- Available through Simplicity Studio and GitHub

### **Multi-protocol**

- Wi-Fi STA + BLE

### **PTA/Coexistence**

- 3 wire coex acting as Wi-Fi with external Bluetooth
- 3 wire coex acting as Wi-Fi with external Zigbee/OT

## **Changes in this release compared to v3.1.3 Release**

### **System**

- **Enhancements**
  - The flash write feature has been enhanced to support the NWP area, providing a 20k allocation within the NWP flash memory for storing user data. Moreover, a Read API has been introduced to retrieve data from the NWP flash region.
  - Added combined image support (NWP and M4) over-the-air update in a single instance. 
- **Fixed Issues**
  - None

### **MCU**

- **Enhancements**
  - The LF clock source has been switched from RO (32kHz) to RC. Adjustments were made in power save applications to reflect this switch
- **Fixed Issues**
  - SensorHub now supports support ADC multi-channel
  - PWM Special event trigger for synchronizing analog-to-digital conversions is working now
  - The wakeup source ULP-SSI is working in the Power Manager service now
  - ADC, ULP\_ADC and ULP\_ADC Multichannel applications are printing float values in serial console

### **SDK**

- **Enhancements**
  - Added support for HTTP Server and provided demo application.
  - Added Support for OPN Based Project Creation.
  - Added Mem pool component support for SoC demos.
  - Added multithreaded support for the Concurrent mode demo.
  - Added IPv6 support in AWS IoT MQTT Client application.
  - Added API support to get socket information from firmware.
  - Enhancements in Wi-Fi throughputs.
- **Fixed Issues**
  - Updated the missing error codes in the documentation.
  - Fixed SNTP client stop issue.

### **Wi-Fi/Network Stack**

- **Enhancements**
  - Added support to configure IPv6 link local address from the host while skipping the configuration of IPv6 unique global address for the network interface.
  - Added support for configuration of SU Beamforming capability
  - Added DNS-SD record in the MDNS query response to allow discovery of DUT's MDNS service with consistency.
- **Fixed Issues**
  - Fixed the server-side dependency that was causing the repetition of the firmware binary header (64 bytes) during OTA process
  - Fixed MBSSID feature Interop issue with WiFi7 Access Points.
### **BLE**

- **Enhancements**
  - Added BLE error code details in 917 documentation.
  - Added external PSRAM support for ble\_accept\_list, ble\_ae\_central, ble\_ae\_peripheral, ble\_central, ble\_datalength, ble\_heart\_rate\_profile, ble\_hid\_on\_gatt, ble\_ibeacon, ble\_longrange\_2mbps, ble\_multiconnection\_gatt\_test, ble\_privacy, ble\_secureconnection, ble\_testmodes, ble\_unified\_ae\_coex\_app, ble\_update\_gain\_table gatt\_long\_read and ble\_datalength applications.
- **Fixed Issues**
  - Resolved the BLE hang issue when DUT was trying to connect to the nRf dongle with I/O capability as 0x00.
  - Fixed BLE start advertising API failed within five minutes of continuously doing ble start, stop, and clear the address resolution API, with an error code 0xffffffe2 (RSI\_ERROR\_RESPONSE\_TIMEOUT).
### **Multi-protocol**

- **Enhancements**
  - Added new macro BLE\_ADVERTISE\_ONLY for wifi\_ble\_power\_save application.
  - Enabled support for 3 SSL sockets in COEX mode.
- **Fixed Issues**
  - None.

## **Recommendations**

### **System**

- The current revision of SiWx917 has:
  - RAM memory of 672k bytes which can be shared between TA and M4 processors in SoC mode. 
  - The below configurations are applicable in SoC mode and can be configured based on the application requirement. EXT\_FEAT\_352K\_M4SS\_320K is the default configuration, based on requirement EXT\_FEAT\_480K\_M4SS\_192K configuration is selected for SoC mode multi-protocol examples.
    - EXT\_FEAT\_480K\_M4SS\_192K - This mode configures TA with 480k and M4 with 192K bytes of memory
    - EXT\_FEAT\_416K\_M4SS\_256K - This mode configures TA with 416k and M4 with 256K bytes of memory
    - EXT\_FEAT\_352K\_M4SS\_320K - This mode configures TA with 352k and M4 with 320K bytes of memory
  - SoC mode should not use 672k\_M4SS\_0K memory configuration.
- Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
- There are 2 Versions of Pro-Kits/Radio boards. Si917-6031A based on Si917-4338A(Rev: A01 - A11) and SiWx917-6031A based on SiWx917-4338A(Rev: A12). To get optimal power numbers, enable macro "SL\_SI91X\_ENABLE\_LOWPWR\_RET\_LDO" pre-processor define for ICs or while using SiWx917-6031A Pro-kit, SiWx917-4338A version of boards. This macro should be disabled for earlier variant of board (Si917-6031A, Si917-4338A).
- With RAM configuration (EXT\_FEAT\_352K\_M4SS\_320K), only 352K memory is available to TA  which limits the features supported, Recommended to enable EXT\_FEAT\_416K\_M4SS\_256K in Wi-Fi + BLE Multi protocol mode to enable more Network features.
- For EXT\_FEAT\_416K\_M4SS\_256K  and EXT\_FEAT\_480K\_M4SS\_192K memory configurations, it is recommended to retain both TA and M4 RAMs in power save.

### **Wi-Fi/Network Stack**

- It is recommended to enable bit 16 of the 'Extended TCP IP Feature' bit map in the opermode command for all Wi-Fi Socket operations from the host to ensure graceful handling during asynchronous closures from the peer.
- For high throughput applications, aggregation (bit 2 of feature\_bit\_map) is recommended to be enabled in opermode. 
- To reset TA (NWP), the application needs to call sl\_wifi\_deinit() followed by sl\_wifi\_init(). Users must ensure that no execution happens from flash at this point of time. (Refer to example applications) 
- Users can enable SL\_SI91X\_EXT\_TCP\_IP\_SSL\_16K\_RECORD in 'Extended TCP IP Feature' bit map in opermode for (HTTPS server) supporting 16k record.
- **TWT**
  - Recommendation is to use sl\_wifi\_target\_wake\_time\_auto\_selection() API for all TWT applications. 
  - It is recommended to issue iTWT setup command once IP assignment, TCP connection, application specific socket connections are done.
  - When using sl\_wifi\_enable\_target\_wake\_time API, increase TCP / ARP Timeouts at the remote side depending upon the configured TWT interval configured. It's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() as an alternative.
  - In case of TWT in coex mode, when using sl\_wifi\_enable\_target\_wake\_time API, use TWT wake duration <= 16 ms and TWT wake interval >= 1 sec. If wake duration > 16 ms or TWT wake interval < 1sec, there might be performance issues.
  - For iTWT GTK interval in AP should be configured to max possible value or zero. If GTK interval is not configurable on AP side, recommended TWT interval (in case of sl\_wifi\_enable\_target\_wake\_time API) or RX Latency (in case of sl\_wifi\_target\_wake\_time\_auto\_selection API) is less than 4sec.
  - When sl\_wifi\_enable\_target\_wake\_time API is used, configuring TWT Wake interval beyond 1 min might lead to disconnections from the AP. Recommended to use TWT wake interval of less than or equal to 1 min.
  - When using sl\_wifi\_enable\_target\_wake\_time API, it is recommended to set missed\_beacon\_count of sl\_wifi\_set\_advanced\_client\_configuration API greater than 2 times of the configured TWT Interval.
- Disable power save for high throughput applications or use FAST PSP power save mode as per application requirement.
- The application needs to ensure that it sets RTC with the correct timestamp before establishing the SSL/EAP connection.
- The minimum timeout value should not be less than 1 second for socket select and socket receive calls. 
- Embedded MQTT keep alive interval should be either 0 or a value greater than 35 seconds.
- Disable power save and suspend any active TWT sessions before triggering HTTP OTAF.
- Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with the power save.

### **BLE**

- In BLE, the recommended range of Connection Interval in
  - Power Save (BLE Only) - 100 ms to 1.28 s.
- In BLE, during Connection, the configuration of Scan Interval and Scan Window with the same value is not recommended. The suggested ratio of Scan Window to Scan Interval is 3:4.
- In BLE, if a device is acting as Central, the scan window (in set\_scan\_params and create\_connection commands) must be less than the existing Connection Interval. The suggested ratio of Scan Window to Connection Interval is 2:3.
- In BLE mode, if scanning and advertising are in progress on the SiWx91x module and it subsequently gets connected and moves to the central role, scanning stops else if it moves to the peripheral role, advertising stops. To further establish a connection to another peripheral device or to a central device, the application should give a command for starting advertising and scanning again.

### **Multi-protocol**

- For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
- Wi-Fi + BLE Advertising
  - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals may be observed.
  - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity.
- Wi-Fi + BLE scanning
  - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
  - BLE scanning will be stopped for intervals that collide with Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Connections
  - All standard connection intervals are supported.
  - For a stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Data Transfer
  - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout.
  - Ensure Wi-Fi activity consumes lower intervals.

## **Known Issues of WiSeConnect3\_SDK\_3.1.4 Release**

### **MCU**

**SensorHub**
 
- Power transitions with AWS are not stable in SensorHub.

**SSI**

- SSI Dual and Quad mode are not functional. 
- SSI primary: Mode3 and TI is not working as expected. 
- SSI Primary will not be operable at 40 MHz 
- SSI Full Duplex in DMA mode is not working as expected
- SSI in DMA mode is limited to only 1024 bytes of buffer size

**I2C**

- ULP I2C High speed and Fast plus modes are not working at specified speed

**UART**

- Flow control is not supported in ULP UART.

**ADC**

- In ADC Multichannel mode, sampling rate is limited to less than 15Ksps. Number of samples is less than 500 and with a maximum of 4 channel
- In ADC Multichannel mode, when using different sampling rates for different channels behavior is not as expected.

**CT (Config Timer)**

- Config Timer(CT) support as an external event, in 32-bit mode, and DMA are not working as expected

**SYSRTC**

- SYSRTC with RO clock is not working as expected.

**Power Manager**

- The wakeup sources BOD, Comparator, and SYSRTC are not working in the Power Manager service

### **SDK**

- Observed Wi-Fi connection is successful even after deleting the stored network credentials using sl\_net\_delete\_credential and responding with SL\_NET\_INVALID\_CREDENTIAL\_TYPE for sl\_net\_get\_credential.
- Enhanced sl\_wifi\_get\_firmware\_version() API to provide more details (ROM ID, chip ID, security version, etc) which is not backward compatible with firmwares older than 1711.2.10.1.0.0.4. Firmware binary notation does not include security version number.
- Matter extension based applications are experiencing compatibility issues with WiseConnect SDK 3.1.4. It is recommended to use WiseConnect SDK 3.1.1 for matter-related applications. This will be addressed in up coming release(s).
- In PSRAM enabled demos, moving of text, data and stack segments to PSRAM is allowed. BSS and Heap should still be in SRAM.
- Asynchronous Azure MQTT is not supported, this This will be addressed in up coming release(s).
- Recommended to configure VAP\_ID properly for Si91x STA and AP using sl\_si91x\_setsockopt\_async(), in case of data transfer.
- Recommended to use valid length(<= 202 bytes) for topic to be published in the "Embedded MQTT" demo, else it leads to return wrong error code(0x21).

### **Wi-Fi/Network Stack**

**Wi-Fi STA**

- STA Connection with the WPA3 Hunting and Pecking algorithm takes about 3-4 seconds.
- Observed connection failures with some AP's in more channel congestion ~50-60% occupancy open lab environment.
- Region selection based on country IE in the beacon is not supported
- Observed intermittent beacon reception from Access Point ( beacon misses) in an extremely congested environment

**Access Point (AP) Mode**

- Scan feature in AP mode not fully functional. 
- Fixed rate configuration in AP mode using sl\_wifi\_set\_transmit\_rate API is not being set as expected. 

**WPA2 Enterprise security (STA)**

- Observed connection issue with configuring certificate key and programming 4096 bit key and SHA384/SHA512 certificates.
- Observing DUT is throwing 0x1001c when configuring .data.certificate\_key as "123456789".
- Configuration issue to program PAC file for EAP FAST Manual mode.
- Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.

**Wi-Fi Concurrency (AP + STA in same channel)**

- Observed 3rd party STA association fail with 917 AP while 917 STA mode is connecting/reconnecting to configured 3rd party AP. Reconnect 3rd party STA to 917 AP in such scenarios. 

**OFDMA (UL/DL)**

- Less throughput observed in DL-OFDMA with some APs that enabled LDPC.

**MUMIMO (DL)**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Performance, Interop issues with MU MIMO with certain APs. 
- Less throughput observed in MU-MIMO with some APs that enabled LDPC.

**MU-MIMO (UL)**

- UL MU-MIMO is not supported.

**TWT**

- Variability with MQTT keep alive interval based on TWT wake up interval.  
- When sl\_wifi\_enable\_target\_wake\_time() API is used, occasional MQTT disconnections may be observed if TWT is configured with longer TWT intervals (>30secs) with embedded MQTT + TWT.  As an alternative, it's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() API, where these dependencies are internally handled.

**Wi-Fi STA Rejoin**

- Observed Scanning (probe request) in all channels instead of the channels configured in selective channel(channel\_bitmap\_2g4) during rejoin process. 

**IPv4/IPv6**

- Observed issue with assigning static IP to DUT
- IP change notification is not indicated to the application. 

**BSD Socket API**

- Configuration issue to program TCP retry count (default 10) through setsockopt
- Every server socket created consumes a socket (maximum of 10 sockets supported) and every subsequent connection to server socket consumes an additional socket (from the same pool of 10 sockets), which limits the number of server connections supported.

**SSL Client/Server**

- Sometimes during SSL Handshake, ECC curve parameters generated are wrong, resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
- Secure SSL renegotiation is not supported in the Embedded Networking Stack
- Observed connection failures with ECDSA ciphers.

**HTTP Client/ HTTPS Client**

- Observed occasional HTTPS continuous download failures when power save is enabled. Recommended to disable it before performing HTTPS continuous downloads
- Observed HTTPS post data is failing with error code: BBE2 (HTTP Failed).
- Unable to get HTTP GET response when http length is exceeding 1024 bytes when SL\_SI91X\_FEAT\_LONG\_HTTP\_URL bit is disabled.

**SNTP**

- Unable to get SNTP async events when Coex mode and power save are enabled 

**Throughputs & Performance**

- Wi-Fi alone throughput is about SDK 3.x (42Mbps). SDK refinements are in progress to further improve Wi-Fi Standalone and Coex Throughputs. 

**Secure Over the Air (OTA) Upgrade**

- None. 

**Wi-Fi IOT Cloud integration (AWS IOT Core)**

- Observed AWS MQTT keepalive transmission is not happening at expected intervals with power save enabled.

**Wi-Fi Interoperability (IOP)**

- Observed disconnections with Amplifi (AFI-INS-R) AP with powersave enable
- TWT session is failing due to disconnections observed in DUT if rx\_latency is set to 55 seconds and receive data is also set to 55 seconds on MI Xiaomi RA72 and Tplink AX53 AP's
- Observed less throughput(~1Mb) while running TCP RX with Max\_PSP powersave with DLink 810 AP
- Observed interop issue (random disconnections) with few APs (EERO 6+, EERO PRO 6E, Cisco Catalyst 9120AXID)
- Disconnections observed with Netgear RAX120 AP in WPA3 Security
- Disconnections observed while running SSL traffic in power save and CoEx mode with Asus RT-AX3000 AP.

### **BLE**

**GAP** 

- BLE reconnection is not working in ble\_multiconnection\_gatt demo when power save is enabled in SoC. 
- When power save is enabled in the SoC, the DUT hangs with the ble\_multiconnection\_gatt demo while performing continuous Tx/Rx notifications.

**DTM/PER**

- Recommend to limit BLE Tx Maximum power to 18 dBm.  Please don't use for 127 power\_index for BLE HP chain with this release.

**IOP**

- BLE connection failure has been observed on a few mobile devices (Samsung S23& Oneplus 9pro) when the Si917 device is configured as central.

### **Multi-protocol**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Wi-Fi + BLE intermittent connection failures, disconnections, and data transfer stalls in the long run when power save is enabled.
- Observed "DUT is not disconnecting to the AP when initiating disconnection from EFR connect app screen using wifi\_station\_ble\_provisioning\_aws example

### **System**

- Observed random hang issues with encrypted firmwares on some earlier variant of boards  (Si917-6031A, Si917-4338A) with powersave enable.

### **Simplicity Studio and Commander** 

- Simplicity commander does not support options under "Debug Lock tools".
- All projects in the package are compatible with **GNU ARM V12.2.1** toolchain

## **Planned Roadmap Features**

### **SDK**

- WPS 2.0 PIN/PUSH, Network Manager Enhancements
- MDNS, HTTP Server, Azure MQTT Client, Websockets
- Matter integration with Embedded TCP/IP stack 
- Enhanced buffer management, Throughput optimization
- TLS over LWIP, Network layer applications (MQTT, HTTP, etc.) over LWIP.
- Debugging utilities, UC Configurator for Wireless configurations

### **System**

- Secure communication over CPC (UART & SDIO interfaces)

### **MCU**

- Pin Tool for MCU - Pin tool for Analog Peripherals (ADC, DAC. Analog Comparator)
- Dual Flash: Separate flash for Cortex M4 and Wireless Processor
- Peripherals: Capacitive Touch Sensor, OPAMP, HSPI, GPDMA, QEI, RNG, IR decoder, CRC and BOD
- Software-configurable MCU application memory options (192 or 256 or 320 kB)
- RTOS support for MCU Peripheral examples
- SensorHub: Support for SDC\_ADC and CLOUD connectivity
- SIO support for features Pattern matching, DMA flow control, self-loop mechanism and as a clock qualifier
- Support for I2S PCM functionality
- Stacked and external PSRAM: Pseudo Static RAM for storing application data and executing SoC applications. 

### **Wi-Fi/Network Stack**

- Spatial Re-Use, BSS Max Idle
- Provisioning using Wi-Fi AP

### **BLE**

- Support for 8 Peripheral + 2 Central connections 
- Support for BLE Mesh (4 nodes only) for limited Mesh use case

## **Limitations and Unsupported Features**  

### **System**

- None

### **SDK**

- Baremetal mode is not supported.
- WiSeConnect3\_SDK\_3.1.3 and later versions are not compatible with firmware versions prior to 1711.2.10.1.2.0.4, due to enhancements in max transmit power configuration during Wi-Fi join/connection, need to be cautious while doing OTA firmware upgrade.

### **Wi-Fi/Network Stack**

- TLS 1.3 Server is not supported.
- 40 MHz bandwidth for 2.4 GHz band and 5 GHz is not supported.
- Max 3 SSL sockets are supported in Wi-Fi alone and CoEx modes. No.of SSL Sockets in Wi-Fi + BLE based on RAM memory configuration selected. 
- In SSL ECC Curve ID supported is 23. SSL handshake with 3rd party clients depends on the SSL ECC Curve ID.
- The number of Non-Transmitting BSSIDs processed is limited by the beacon length that can be processed by the stack (which is 1024 bytes). Beacons greater than 1024 Bytes in length will not be processed.
- Multiprotocol (STA +BLE) + EAP Security modes supported only with Memory configurations EXT\_FEAT\_416K\_M4SS\_256K and EXT\_FEAT\_480K\_M4SS\_192K.
- UL MUMIMO is not supported.
- WPA3 AP supports only H2E algorithm.
- PMKSA caching is not supported in WPA3 AP mode.
- Maximum embedded MQTT Publish payload is 1 kbyte.
- Timeout value for socket select and socket receive calls of less than 1 second is not currently supported.
- SA query procedure not supported in 11W AP mode.
- WPA3 AP transition mode is not supported.

### **BLE**

- For BLE, if the connection is established with a small connection interval (less than 15 ms), simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported.
- BLE maximum two concurrent connections are supported, which can be either a connection to two peripheral devices, to one central and one peripheral device or two central devices.
- BLE Slave latency value is valid up to 32 only.
- Maximum supported AE data length is 200 bytes.
- Supports only two ADV\_EXT sets.
- Supports only two BLE connections (1 Central and 1 Peripheral) with AE.
- Advertising Extension feature is not supported in Coexistence.
- The  ae\_central & ae\_peripheral applications are not supported with TA\_352K\_M4\_320K RAM configuration.
- Two BLE connections are not supported with M4 powersave. It only supports a single connection. 
- Isochronous channels feature is not supported. 
- Connection subrating feature is not supported. 
- LE power controller feature is not supported.
- EATT feature is not supported.
- Periodic Advertising with a response feature is not supported. 
- BLE Audio is not supported.
- The feature of dynamically changing the TX power when extended advertising is active is not supported.

### **MCU**

- SensorHub supports PS1-power state with ADC sensor. In this mode, other sensors operation is not supported. 

### **Multi-protocol**

- Wi-Fi AP + BLE currently not supported. 
- EXT\_FEAT\_352K\_M4SS\_320K RAM configuration is not supported for coex mode with SSL

<br>

# **WiSeConnect3\_SDK\_3.1.3 SoC Release Notes**

## **Release Details**

|**Item**|**Details**|
| :- | :- |
|Release date|Feb 14th, 2024|
|SDK Version|3\.1.3|
|Firmware Version|1711\.2.10.1.2.0.4|
|Release Package Name|WiSeConnect3\_SDK\_3.1.3|
|Supported RTOS|FreeRTOS|
|Operating Modes Supported|Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP|

- SiWx917 release consists of two components:
  - Wireless Firmware - SiWx917 Firmware Binary available as SiWG917-B.2.10.1.2.0.4.rps
  - Wiseconnect3 Library - Wiseconnect3 SDK library runs on internal Cortex M4

**NOTE:** Mandatory to upgrade the earlier version of boards (Si917-6031A Prokit or BRD4338A boards) or 917 Silicon ICs with instructions as outlined in this document "SiWG917–TA\_Flash\_Memory\_Map\_ChangeGuide\_v1.3.pdf" for more details.  

## **Supported Hardware**

|**Hardware**|**OPN**|
| :- | :- |
|Silicon (IC OPN)|SiWG917M111MGTBA|
|Development Kits|<p>Pro Kit: SiWx917-PK6031A, Si917-PK6031A, SiWx917-PK6032A. </p><p>(Pro Kit includes Mother board "SI-MB4002A" + Radio board)</p><p>Radio boards: SiWx917-RB4338A, Si917-RB4338A, SiWx917-RB4342A </p>|

## **Supported Features** 

### **System**

- **Operating Modes** 
  - Wi-Fi STA (802.11ax, 802.11n), Wi-Fi 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + BLE
- **Security** 
  - Secure Boot, Secure Key storage and HW device identity with PUF, Secure Zone, Secure XIP (Execution in place) from flash, Secure Attestation, Anti Rollback, Secure Debug and Flash Protection. 
  - Secure firmware upgrade options:
    - Firmware loading support by Commander Tool through Jlink Debugger. Jlink connected to Serial Wire Debug (SWD) 
    - Firmware loading via ISP using UART (Commander or Serial terminal), SPI Interface
    - Secure Over the Air (OTA) Upgrade
- **Crypto Support**
  - ` `Crypto API's for Hardware Accelerators:
    - Advanced Encryption Standard (AES) 128/256/192, Secure Hash Algorithm (SHA) 256/384/512, Hash Message Authentication Code (HMAC), Random Number Generator (RNG), SHA3, AES-Galois Counter Mode (GCM)/ Cipher based Message Authentication Code (CMAC), ChaCha-poly, True Random Number Generator (TRNG)
  - Software Accelerators: RSA, ECC 
  - Integrated with mbed TLS.
  - TRNG, AES, SHA, HMAC, GCM, Chacha-poly, ECDH and AES-CCM hardware crypto engine is integrated with PSA crypto.
- **System Power Save**
  - Deep Sleep with RAM retention and without RAM retention. 
  - Wireless Power Save: Connected Sleep (Wi-Fi Standby Associated), BLE Advertising with powersave, BLE Scan with powersave ,  BLE connection with powersave. Only Max PSP power save mode is supported in BLE. 

### **MCU**

- **Memory**
  - Common Flash: Single shared Flash for both Cortex-M4 and NWP (Wireless Processor)
  - Common Flash + External PSRAM
- **Power States**
  - Active: PS4, PS3, PS2, and PS1 
  - Standby: PS4, PS3, and PS2
  - Sleep: PS4, PS3 and PS2
  - Deep Sleep(Shutdown): PS0
- **Peripherals**
  - High Performance (HP)- ADC, CALENDAR, CONFIG TIMER(CT), EFUSE, EGPIO, GSPI, I2C, I2S, MCPWM, SDIO Secondary, SIO, SSI, SYSRTC, UART, uDMA, USART, WDT, Analog Comparator, DAC and Temperature Sensor(RO and BJT)** 
  - Ultra Low Power (ULP) - ULP\_TIMER, ULP\_UART, ULP\_I2C, ULP\_I2S, ULP\_uDMA and ULP\_SSI
- **Services**
  - Sleep Timer, IOSTREAM, Power Manager, SensorHub, NVM3
- **Development Pro-Kit Peripheral Support**
  - LED, Button, MEMLCD, Joystick, RHT Sensor

### **Development Environment**

- Simplicity Studio IDE (SV5.8.0 version) and Debugger Integration.   Refer to the latest version of the SoC "Getting-Started-with-SiWx917" guide for more details. 
- Simplicity Commander to supports Flash loading, provision of MBR programming, security key management, and calibration support for crystal and gain offsets. refer "siwx917-soc-manufacturing-utility-user-guide" for more details.** 
- Advanced Energy Monitoring (AEM) to measure ultra-low power capability on Development boards (Pro Kit).

### **Wi-Fi**

- **Wi-Fi Protocols**
  - IEEE 802.11 b/g/n/ax (2.4GHz)
- **Access Point (AP) Mode**
  - 4 Client Support, Hidden SSID Mode, Auto Channel Selection, Scan in AP mode (Alpha Version)
  - Wi-Fi Security  
    - WPA2 Personal, WPA3 Personal (H2E method only) (Alpha Version), WPA Mixed mode (WPA/WPA2) 
- **Wi-Fi Scan**
  - Selective Scan, Active/Passive Scan
- **Wi-Fi STA (Security Modes)**
  - Open Mode, WPA2 Personal, WPA2 Enhancements, WPA3 Personal, Mixed Mode (WPA/WPA2), WPA3 Personal Transition Mode (WPA2/WPA3)
- **WPA2 Enterprise security (STA)**
  - Method
    - PEAP/TTLS/TLS 1.0/TLS 1.2/FAST
- **Wi-Fi STA Rejoin**
- **Wi-Fi STA Roaming**
  - BG Scan, OKC (Opportunistic Key caching), PMK (Pairwise Master Key) caching, Pre-Authentication
- **Wi-Fi Protocol Power Save** 
  - Deep sleep (unconnected state), Max PSP, Enhanced Max PSP, Fast PSP,** TWT
- **QoS**
  - WMM-QoS
- **Wi-Fi 6 Feature**
  - MUMIMO (DL), OFDMA (UL/DL), iTWT, TWT I-Frame & TWT Enhancements, BSS coloring, MBSSID
- **Wi-Fi Concurrency** 
  - AP+STA (Same channel)
- **Wi-Fi Band/Channels**
  - 2.4GHz CH1-11, 2.4GHz CH1-13, 2.4GHz CH1-14 (Japan)
- **Known Security Vulnerabilities Handled**
  - WPA2 KRACK Attacks, Fragment and Forge Vulnerability

### **Network Stack**

- **Core Networking Features**
  - TCP/IP Bypass (LWIP as Hosted stack for reference), IPv4/IPv6/UDP/TCP/ARP/ICMP/ICMPv6
  - SSL client versions TLSV1.0, TLSV1.2, TLSV1.3 
  - SSL server versions TLSV1.0 and TLSV1.2
  - DHCPv4 (Client/Server)/ DHCPv6 Client
- **Advanced Network Features**
  - HTTP Client/HTTPS Client//DNS Client/SNTP Client, Embedded MQTT/MQTT on host
- **Wi-Fi IoT Cloud Integration**
  - AWS IOT Core
- BSD and IoT sockets application programming interface(API)

### **BLE** 

- GAP(Advertising, Scanning, initiation, Connection and Bonding)
- Generic Attribute Protocol(GATT)
- Attribute protocol(ATT)
- Security
- LL Privacy 1.2
- Accept list
- Directed Advertising
- Extended Advertising
- Periodic Advertising
- Periodic Advertising scanning
- Extended Advertising scanning
- Periodic Advertising list
- LE periodic advertising synchronization
- LE PHY(1Mbps, 2Mbps) & Coded PHY(125Kbps, 500kbps)
- Simultaneous scanning on 1Mbps and Coded PHY
- LE dual role topology
- LE data packet length extensions(DLE)
- Asymmetric PHYs
- LE channel selection algorithm 2 (CSA#2)
- LE Secure connections
- Bluetooth 5.4 Qualified

### **SDK**

- Simplified and Unified DX for Wi-Fi API and Platform APIs
- Simplifies application development and presents clean and standardized APIs
- UC (Universal Configurator) enables componentization, simplifying configuration of peripherals and examples
- BSD and ARM IoT-compliant socket API
- Available through Simplicity Studio and GitHub

### **Multi-protocol**

- Wi-Fi STA + BLE

### **PTA/Coexistence**

- 3 wire coex acting as Wi-Fi with external Bluetooth
- 3 wire coex acting as Wi-Fi with external Zigbee/OT

## **Changes in this release compared to v3.1.2 Release**

### **System**

- **Enhancements**
  - Enhancements done in power save module to address a few stability issues
  - Enchancements done in deinit mechanism 
- **Fixed Issues**
  - Fixed De-init issue for the system which do not support XTAL.

### **MCU**

- **Enhancements**
  - Added support for SDC with ADC in SensorHub
  - Provided support for UART multiple instance
  - Provided support for M4 Power transitions (PS4-PS2-PS4) in Power Manager servicepin.
- **Fixed Issues**
  - Fix for SPI sensor issue in SensorHub
  - Fix for ADC stuck in powersave initialization

### **SDK**

- **Enhancements**
  - Added mDNS feature support (Alpha Version).
  - Added Azure Freertos middleware SDK Support (Alpha Version).
  - Added support for External PSRAM (SiWx917-RB4342A), for featured demos.
  - Added "wifi_http_otaf_twt" demo.
  - Added "Wi-Fi - Azure IoT MQTT Client" demo.
  - Added sl\_si91x\_set\_fast\_fw\_up() and sl\_si91x\_upgrade\_firmware() APIs  for firmware upgrade.
  - Added callback support for handling TCP remote terminate event.
  - Added support for wild cards in MQTT subscription.
  - Added sl\_sntp\_client\_get\_time() API to get epoch time for SNTP.
  - Replaced licensed third party certificates with prototype certs for AWS and Azure.
  - It is recommended to enable bit 12 of the 'Feature bit map' in the opermode command for enabling long sized ICMP packet support.
  - Added support in SDK to set absolute power value in scan and join commands.
  - Added wifi6 support in PER mode for CLI demo.
  - Implemented a handshake mechanism between NWP and M4 for Wi-Fi deinit to overcome RAM-based execution dependency.
- **Fixed Issues**
  - Added RAM execution for calibration example.
  - Fixed MQTT returning an error when the incoming publish packet length is greater than 64 bytes.
  - Fixed Http client de-init hang issue when wlan disconnect occurs.
  - Fixed issues with Asynchronous socket select.
  - Updated Korea region configuration.
  - Fixed issues in OOB demo.
  - Fixed issues in standard curve parameters for SECP192R1 curve.
  - Modified RSI\_HT\_CAPS\_BIT\_MAP to support 20MHz channel width only. 

### **Wi-Fi/Network Stack**

- **Enhancements**
  - Added Long URL support of 2048 bytes for HTTP Get Command 
  - Added support for Transition Disable Indication (TDI) in AP mode. It is an indication from an Si91x AP to WPA2/3 STA, that the STA is to disable certain transition modes for subsequent connections to the AP's network.
  - Implemented beacon stop feature in AP mode.
  - Enabled HT Caps in AP mode for high throughputs.
  - Increased the topic length and username length size for Embedded MQTT Publish and Subscribe.
- **Fixed Issues**
  - Added support for wrapping functionality for unaligned key length.

### **BLE**

- **Enhancements**
  - None
- **Fixed Issues**
  - Fixed the issue where connectable directed advertising exited without establishing a connection with the central device.
  - Fixed issue where device is throwing an error when it tried to re-enabling AE ADV after establishing an AE connection with two adverting sets.
  - Fixed AE connection fail issue when remote device transmitting AUX ADV packets with Coded PHY rates.

### **Multi-protocol**

- **Enhancements**
  - None
- **Fixed Issues**
  - Fixed issue where device sometimes failed to respond to connection update request in CoEx which is causing BLE disconnection.

## **Recommendations**

### **System**

- The current revision of SiWx917 has:
  - RAM memory of 672k bytes which can be shared between TA and M4 processors in SoC mode. 
  - The below configurations are applicable in SoC mode and can be configured based on the application requirement. EXT\_FEAT\_352K\_M4SS\_320K is the default configuration, based on requirement EXT\_FEAT\_480K\_M4SS\_192K configuration is selected for SoC mode multi-protocol examples.
    - EXT\_FEAT\_480K\_M4SS\_192K - This mode configures TA with 480k and M4 with 192K bytes of memory
    - EXT\_FEAT\_416K\_M4SS\_256K - This mode configures TA with 416k and M4 with 256K bytes of memory
    - EXT\_FEAT\_352K\_M4SS\_320K - This mode configures TA with 352k and M4 with 320K bytes of memory
  - SoC mode should not use 672k\_M4SS\_0K memory configuration.
- Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
- There are 2 Versions of Pro-Kits/Radio boards.  Si917-6031A based on Si917-4338A and SiWx917-6031A based on SiWx917-4338A. To get optimal power numbers, enable macro "SL\_SI91X\_ENABLE\_LOWPWR\_RET\_LDO" pre-processor define for ICs or while using SiWx917-6031A Pro-kit, SiWx917-4338A version of boards. This macro should be disabled for earlier variant of board (Si917-6031A, Si917-4338A).
- With RAM configuration (EXT\_FEAT\_352K\_M4SS\_320K), only 352K memory is available to TA  which limits the features supported, Recommended to enable EXT\_FEAT\_416K\_M4SS\_256K in Wi-Fi + BLE Multi protocol mode to enable more Network features.
- For EXT\_FEAT\_416K\_M4SS\_256K  and EXT\_FEAT\_480K\_M4SS\_192K memory configurations, it is recommended to retain both TA and M4 RAMs in power save.

### **Wi-Fi/Network Stack**

- It is recommended to enable bit 16 of the 'Extended TCP IP Feature' bit map in the opermode command for all Wi-Fi Socket operations from the host to ensure graceful handling during asynchronous closures from the peer.
- For high throughput applications, aggregation (bit 2 of feature\_bit\_map) is recommended to be enabled in opermode. 
- To reset TA (NWP), the application needs to call sl\_wifi\_deinit() followed by sl\_wifi\_init(). Users must ensure that no execution happens from flash at this point of time. (Refer to example applications) 
- Users can enable SL\_SI91X\_EXT\_TCP\_IP\_SSL\_16K\_RECORD in 'Extended TCP IP Feature' bit map in opermode for (HTTPS server) supporting 16k record.
- **TWT**
  - Recommendation is to use sl\_wifi\_target\_wake\_time\_auto\_selection() API for all TWT applications. 
  - It is recommended to issue iTWT setup command once IP assignment, TCP connection, application specific socket connections are done.
  - When using sl\_wifi\_enable\_target\_wake\_time API, increase TCP / ARP Timeouts at the remote side depending upon the configured TWT interval configured. It's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() as an alternative.
  - In case of TWT in coex mode, when using sl\_wifi\_enable\_target\_wake\_time API, use TWT wake duration <= 16 ms and TWT wake interval >= 1 sec. If wake duration > 16 ms or TWT wake interval < 1sec, there might be performance issues.
  - For iTWT GTK interval in AP should be configured to max possible value or zero. If GTK interval is not configurable on AP side, recommended TWT interval (in case of sl\_wifi\_enable\_target\_wake\_time API) or RX Latency (in case of sl\_wifi\_target\_wake\_time\_auto\_selection API) is less than 4sec.
  - When sl\_wifi\_enable\_target\_wake\_time API is used, configuring TWT Wake interval beyond 1 min might lead to disconnections from the AP. Recommended to use TWT wake interval of less than or equal to 1 min.
  - When using sl\_wifi\_enable\_target\_wake\_time API, it is recommended to set missed\_beacon\_count of sl\_wifi\_set\_advanced\_client\_configuration API greater than 2 times of the configured TWT Interval.
- Disable power save for high throughput applications or use FAST PSP power save mode as per application requirement.
- The application needs to ensure that it sets RTC with the correct timestamp before establishing the SSL/EAP connection.
- The minimum timeout value should not be less than 1 second for socket select and socket receive calls. 
- Embedded MQTT keep alive interval should be either 0 or a value greater than 35 seconds.
- Disable power save and suspend any active TWT sessions before triggering HTTP OTAF.
- Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with the power save.

### **BLE**

- In BLE, the recommended range of Connection Interval in
  - Power Save (BLE Only) - 100 ms to 1.28 s.
- In BLE, during Connection, the configuration of Scan Interval and Scan Window with the same value is not recommended. The suggested ratio of Scan Window to Scan Interval is 3:4.
- In BLE, if a device is acting as Central, the scan window (in set\_scan\_params and create\_connection commands) must be less than the existing Connection Interval. The suggested ratio of Scan Window to Connection Interval is 2:3.
- In BLE mode, if scanning and advertising are in progress on the SiWx91x module and it subsequently gets connected and moves to the central role, scanning stops else if it moves to the peripheral role, advertising stops. To further establish a connection to another peripheral device or to a central device, the application should give a command for starting advertising and scanning again.

### **Multi-protocol**

- For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
- Wi-Fi + BLE Advertising
  - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals may be observed.
  - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity.
- Wi-Fi + BLE scanning
  - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
  - BLE scanning will be stopped for intervals that collide with Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Connections
  - All standard connection intervals are supported.
  - For a stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Data Transfer
  - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout.
  - Ensure Wi-Fi activity consumes lower intervals.

## **Known Issues of WiSeConnect3\_SDK\_3.1.3 Release**

### **MCU**

**SensorHub**

- SensorHub does not support ADC multi-channel. 
- Power transitions with AWS are not stable in SensorHub.

**SSI**

- SSI Dual and Quad mode are not functional. 
- SSI primary: mode3(TI) is not working as expected. 
- SSI Primary will not be operable at 40 MHz 

**I2C**

- I2C High speed and Fastplus modes are not working at specification speed

**UART**

- Flow control is not supported in USART and IOSTREAM applications

**ADC**

- In ADC Multichannel, sampling rate is limited to less than 15Ksps. Number of samples is less than 500 and with a maximum of 4 channel
- In ADC Multichannel, when using different sampling rates for different channels behavior is not as expected
- ADC, ULP_ADC and ULP_ADC Multichannel applications are not printing float values in serial console.

**CT (Config Timer)**

- Config Timer(CT) support as an external event, in 32-bit mode, and DMA are not working as expected

**PWM**

- PWM Special event trigger for synchronizing analog-to-digital conversions is not working

**SYSRTC**

- SYSRTC with RO clock is not functioning

**Power Manager**

- The wakeup sources ULP-SSI, BOD, Comparator, and SysRTC are not working in the Power Manager service

### **SDK**

- Observed Wi-Fi connection is successful even after deleting the stored network credentials using sl\_net\_delete\_credential and responding with SL\_NET\_INVALID\_CREDENTIAL\_TYPE for sl\_net\_get\_credential.
- Enhanced sl\_wifi\_get\_firmware\_version() API to provide more details (ROM ID, chip ID, security version, etc) which is not backward compatible with firmwares older than 1711.2.10.1.0.0.4. Firmware binary notation does not include security version number.
- Matter applications are experiencing compatibility issues with WiseConnect SDK 3.1.2. It is recommended to use WiseConnect SDK 3.1.1 for matter-related applications. This will be addressed in up coming release(s).
- In PSRAM enabled demos, moving of text, data and stack segments to PSRAM is allowed. BSS and Heap should still be in SRAM.
- Asynchronous Azure MQTT is not supported, this This will be addressed in up coming release(s).

### **Wi-Fi/Network Stack**

**Wi-Fi STA**

- STA Connection with the WPA3 Hunting and Pecking algorithm takes about 3-4 seconds.
- Observed connection failures with some AP's in more channel congestion ~50-60% occupancy open lab environment.
- Region selection based on country IE in the beacon is not supported
- Observed intermittent beacon reception from Access Point ( beacon misses) in an extremely congested environment

**Access Point (AP) Mode**

- Scan feature in AP mode not fully functional. 
- Fixed rate configuration in AP mode using sl\_wifi\_set\_transmit\_rate API is not being set as expected. 

**WPA2 Enterprise security (STA)**

- Observed connection issue with configuring certificate key and programming 4096 bit key and SHA384/SHA512 certificates.
- Observing DUT is throwing 0x1001c when configuring .data.certificate\_key as "123456789".
- Configuration issue to program PAC file for EAP FAST Manual mode.
- Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.

**Wi-Fi Concurrency (AP + STA in same channel)**

- Observed 3rd party STA association fail with 917 AP while 917 STA mode is connecting/reconnecting to configured 3rd party AP. Reconnect 3rd party STA to 917 AP in such scenarios. 

**OFDMA (UL/DL)**

- Less throughput observed in DL-OFDMA with some APs that enabled LDPC.

**MUMIMO (DL)**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Performance, Interop issues with MU MIMO with certain APs. 
- Less throughput observed in MU-MIMO with some APs that enabled LDPC.

**MU-MIMO (UL)**

- UL MU-MIMO is not supported.

**TWT**

- Variability with MQTT keep alive interval based on TWT wake up interval.  
- When sl\_wifi\_enable\_target\_wake\_time() API is used, occasional MQTT disconnections may be observed if TWT is configured with longer TWT intervals (>30secs) with embedded MQTT + TWT.  As an alternative, it's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() API, where these dependencies are internally handled.

**Wi-Fi STA Rejoin**

- Observed Scanning (probe request) in all channels instead of the channels configured in selective channel(channel\_bitmap\_2g4) during rejoin process. 

**IPv4/IPv6**

- Observed issue with assigning static IP to DUT
- IP change notification is not indicated to the application. 

**BSD Socket API**

- Configuration issue to program TCP retry count (default 10) through setsockopt
- Every server socket created consumes a socket (maximum of 10 sockets supported) and every subsequent connection to server socket consumes an additional socket (from the same pool of 10 sockets), which limits the number of server connections supported.

**SSL Client/Server**

- Sometimes during SSL Handshake, ECC curve parameters generated are wrong, resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
- Secure SSL renegotiation is not supported in the Embedded Networking Stack
- Observed connection failures with ECDSA ciphers.

**HTTP Client/ HTTPS Client**

- Observed occasional HTTPS continuous download failures when power save is enabled. Recommended to disable it before performing HTTPS continuous downloads
- Observed HTTPS post data is failing with error code: BBE2 (HTTP Failed).
- Unable to get HTTP GET response when http length is exceeding 1024 bytes when SL\_SI91X\_FEAT\_LONG\_HTTP\_URL bit is disabled.

**SNTP**

- Unable to get SNTP async events when Coex mode and power save are enabled 

**Throughputs & Performance**

- Wi-Fi alone throughput is about SDK 3.x (42Mbps). SDK refinements are in progress to further improve Wi-Fi Standalone and Coex Throughputs. 

**Secure Over the Air (OTA) Upgrade**

- Observed firmware upgrade failures after multiple iterations.
- Observed OTA failures with powersave enable, So recommended to disable powersave during OTA. 
- Observed "0xffffffff" error at the remote end while doing firmware upgrade via TCP server using (featured) example with ubuntu 21.0x. 

**Wi-Fi IOT Cloud integration (AWS IOT Core)**

- Observed AWS MQTT keepalive transmission is not happening at expected intervals with power save enabled.

**Wi-Fi Interoperability (IOP)**

- Observed disconnections with Amplifi (AFI-INS-R) AP with powersave enable
- TWT session is failing due to disconnections observed in DUT if rx\_latency is set to 55 seconds and receive data is also set to 55 seconds on MI Xiaomi RA72 and Tplink AX53 AP's
- Observed less throughput(~1Mb) while running TCP RX with Max\_PSP powersave with DLink 810 AP
- Observed interop issue (random disconnections) with few APs (EERO 6+, EERO PRO 6E, Cisco Catalyst 9120AXID)
- Disconnections observed with Netgear RAX120 AP in WPA3 Security

### **BLE**

**GAP** 

- BLE reconnection is not working in ble\_multiconnection\_gatt demo when power save is enabled in SoC. 
- When power save is enabled in the SoC, the DUT hangs with the ble\_multiconnection\_gatt demo while performing continuous Tx/Rx notifications.
- BLE start advertising API failed with an error code 0xffffffe2(RSI_ERROR_RESPONSE_TIMEOUT) within 5minutes while executing ble start ,stop and clear the address resolution API's are called in a loop continuously.

**DTM/PER**

- Recommend to limit BLE Tx Maximum power to 18 dBm.  Please don't use for 127 power\_index for BLE HP chain with this release.

**IOP**

- BLE connection failure has been observed on a few mobile devices (Samsung S23& Oneplus 9pro) when the Si917 device is configured as central.
- Hang issue is observed when DUT is trying to connect to nRF dongle with I/O capability set to 0.

### **Multi-protocol**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Wi-Fi + BLE intermittent connection failures, disconnections, and data transfer stalls in the long run when power save is enabled.
- Observed "DUT is not disconnecting to the AP when initiating disconnection from EFR connect app screen using wifi\_station\_ble\_provisioning\_aws example

### **System**

- Observed random hang issues with encrypted firmwares on some earlier variant of boards  (Si917-6031A, Si917-4338A) with powersave enable.

### **Simplicity Studio and Commander** 

- Simplicity commander does not support options under "Debug Lock tools".
- All projects in the package are compatible with **GNU ARM V12.2.1** toolchain

## **Planned Roadmap Features**

### **SDK**

- WPS 2.0 PIN/PUSH, Network Manager Enhancements
- MDNS, HTTP Server, Azure MQTT Client, Websockets
- Matter integration with Embedded TCP/IP stack 
- Enhanced buffer management, Throughput optimization
- TLS over LWIP, Network layer applications (MQTT, HTTP, etc.) over LWIP.
- Debugging utilities, UC Configurator for Wireless configurations

### **System**

- Secure communication over CPC (UART & SDIO interfaces)

### **MCU**

- Pin Tool for MCU 
- Dual Flash: Separate flash for Cortex M4 and Wireless Processor
- Peripherals: Capacitive Touch Sensor, OPAMP, HSPI, GPDMA, QEI, RNG, IR decoder, CRC and BOD
- Software-configurable MCU application memory options (192 or 256 or 320 kB)
- RTOS support for MCU Peripheral examples
- SensorHub: Support for SDC\_ADC and CLOUD connectivity
- SIO support for features Pattern matching, DMA flow control, self-loop mechanism and as a clock qualifier
- Support for I2S PCM functionality
- Stacked and external PSRAM: Pseudo Static RAM for storing application data and executing SoC applications. 

### **Wi-Fi/Network Stack**

- Spatial Re-Use, BSS Max Idle
- Provisioning using Wi-Fi AP

### **BLE**

- Support for 8 Peripheral + 2 Central connections 
- Support for BLE Mesh (4 nodes only) for limited Mesh use case

## **Limitations and Unsupported Features**  

### **System**

- None

### **SDK**

- Baremetal mode is not supported.
- WiSeConnect3_SDK_3.1.3 and later versions are not compatible with firmware versions prior to 1711.2.10.1.2.0.4, due to enhancements in max transmit power configuration during Wi-Fi join/connection, need to be cautious while doing OTA firmware upgrade.

### **Wi-Fi/Network Stack**

- TLS 1.3 Server is not supported.
- 40 MHz bandwidth for 2.4 GHz band and 5 GHz is not supported.
- Max 3 SSL sockets in Wi-Fi only mode, 2 SSL Socket in Wi-Fi + BLE based on RAM memory configuration selected. 
- In SSL ECC Curve ID supported is 23. SSL handshake with 3rd party clients depends on the SSL ECC Curve ID.
- The number of Non-Transmitting BSSIDs processed is limited by the beacon length that can be processed by the stack (which is 1024 bytes). Beacons greater than 1024 Bytes in length will not be processed.
- Multiprotocol (STA +BLE) + EAP Security modes supported only with Memory configurations EXT\_FEAT\_416K\_M4SS\_256K and EXT\_FEAT\_480K\_M4SS\_192K.
- UL MUMIMO is not supported.
- WPA3 AP supports only H2E algorithm.
- PMKSA caching is not supported in WPA3 AP mode.
- Maximum embedded MQTT Publish payload is 1 kbyte.
- Timeout value for socket select and socket receive calls of less than 1 second is not currently supported.
- SA query procedure not supported in 11W AP mode.
- WPA3 AP transition mode is not supported.

### **BLE**

- For BLE, if the connection is established with a small connection interval (less than 15 ms), simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported.
- BLE maximum two concurrent connections are supported, which can be either a connection to two peripheral devices, to one central and one peripheral device or two central devices.
- BLE Slave latency value is valid up to 32 only.
- Maximum supported AE data length is 200 bytes.
- Supports only two ADV\_EXT sets.
- Supports only two BLE connections (1 Central and 1 Peripheral) with AE.
- Advertising Extension feature is not supported in Coexistence.
- The  ae\_central & ae\_peripheral applications are not supported with TA\_352K\_M4\_320K RAM configuration.
- Two BLE connections are not supported with M4 powersave. It only supports a single connection. 
- Isochronous channels feature is not supported. 
- Connection subrating feature is not supported. 
- LE power controller feature is not supported.
- EATT feature is not supported.
- Periodic Advertising with a response feature is not supported. 
- BLE Audio is not supported.
- The feature of dynamically changing the TX power when extended advertising is active is not supported.

### **MCU**

- SensorHub supports PS1-power state with ADC sensor. In this mode, other sensors operation is not supported. 

### **Multi-protocol**

- Wi-Fi AP + BLE currently not supported. 
- EXT\_FEAT\_416K\_M4SS\_256K  is the minimum RAM configuration to support coex mode with SSL


<br>

# **WiSeConnect3\_SDK\_3.1.2 SoC Release Notes**

## **Release Details**

|**Item**|**Details**|
| :- | :- |
|Release date|Jan 17th, 2024|
|SDK Version|3\.1.2|
|Firmware Version|1711\.2.10.1.1.0.2|
|Release Package Name|WiSeConnect3\_SDK\_3.1.2|
|Supported RTOS|FreeRTOS|
|Operating Modes Supported|Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP|

- SiWx917 release consists of two components:
  - Wireless Firmware - SiWx917 Firmware Binary available as SiWG917-B.2.10.1.1.0.2.rps
  - Wiseconnect3 Library - Wiseconnect3 SDK library runs on internal Cortex M4

**NOTE:** Mandatory to upgrade the earlier version of boards (Si917-6031A Prokit or 4338A boards) or 917 Silicon ICs with instructions as outlined in this document "SiWG917–TA\_Flash\_Memory\_Map\_ChangeGuide\_v1.3.pdf" for more details.  

## **Supported Hardware**

|**Hardware**|**OPN**|
| :- | :- |
|Silicon (IC OPN)|SiWG917M111MGTBA|
|Development Kits|<p>Pro Kit: SiWx917-PK6031A, Si917-PK6031A. </p><p>(Pro Kit includes Mother board "SI-MB4002A" + Radio board)</p><p>Radio boards: SiWx917-RB4338A, Si917-RB4338A </p>|

## **Supported Features** 

### **System**

- **Operating Modes** 
  - Wi-Fi STA (802.11ax, 802.11n), Wi-Fi 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + BLE
- **Security** 
  - Secure Boot, Secure Key storage and HW device identity with PUF, Secure Zone, Secure XIP (Execution in place) from flash, Secure Attestation, Anti Rollback, Secure Debug. Flash Protection. 
  - Secure firmware upgrade options:
    - Firmware loading support by Commander Tool through Jlink Debugger. Jlink connected to Serial Wire Debug (SWD) 
    - Firmware loading via ISP using UART (Commander or Serial terminal), SPI Interface
    - Secure Over the Air (OTA) Upgrade
- **Crypto Support**
  - ` `Crypto API's for Hardware Accelerators:
    - Advanced Encryption Standard (AES) 128/256/192, Secure Hash Algorithm (SHA) 256/384/512, Hash Message Authentication Code (HMAC), Random Number Generator (RNG), SHA3, AES-Galois Counter Mode (GCM)/ Cipher based Message Authentication Code (CMAC), ChaCha-poly, True Random Number Generator (TRNG)
  - Software Accelerators: RSA, ECC 
  - Integrated with mbed TLS.
  - TRNG, AES, SHA, HMAC, GCM, Chacha-poly, ECDH and AES-CCM hardware crypto engine is integrated with PSA crypto.
- **System Power Save**
  - Deep Sleep with RAM retention and without RAM retention. 
  - Wireless Power Save: Connected Sleep (Wi-Fi Standby Associated), BLE Advertising with powersave, BLE Scan with powersave ,  BLE connection with powersave. Only Max PSP power save mode is supported in BLE. 

### **MCU**

- **Memory**
  - Common Flash: Single shared Flash for both Cortex-M4 and NWP (Wireless Processor)
- **Power States**
  - Active: PS4, PS3, PS2, and PS1 
  - Standby: PS4, PS3, and PS2
  - Sleep: PS4, PS3 and PS2
  - Deep Sleep(Shutdown): PS0
- **Peripherals**
  - High Performance (HP)- ADC, CALENDAR, CONFIG TIMER(CT), EFUSE, EGPIO, GSPI, I2C, I2S, MCPWM, SDIO Secondary, SIO, SSI, SYSRTC, UART, uDMA, USART, WDT** 
  - Ultra Low Power (ULP) - ULP\_TIMER, ULP\_UART, ULP\_I2C, ULP\_I2S, ULP\_uDMA and ULP\_SSI
- **Services**
  - Sleep Timer, IOSTREAM, Power Manager, SensorHub, NVM3
- **Development Pro-Kit Peripheral Support**
  - LED, Button, MEMLCD, Joystick, RHT Sensor

### **Development Environment**

- Simplicity Studio IDE (SV5.8.0 version) and Debugger Integration.   Refer to the latest version of the SoC "Getting-Started-with-SiWx917" guide for more details. 
- Simplicity Commander to supports Flash loading, provision of MBR programming, security key management, and calibration support for crystal and gain offsets. refer "siwx917-soc-manufacturing-utility-user-guide" for more details.** 
- Advanced Energy Monitoring (AEM) to measure ultra-low power capability on Development boards (Pro Kit).

### **Wi-Fi**

- **Wi-Fi Protocols**
  - IEEE 802.11 b/g/n/ax (2.4GHz)
- **Access Point (AP) Mode**
  - 4 Client Support, Hidden SSID Mode, Auto Channel Selection, Scan in AP mode
  - Wi-Fi Security  
    - WPA2 Personal, WPA3 Personal (H2E method only), WPA Mixed mode (WPA/WPA2) 
- **Wi-Fi Scan**
  - Selective Scan, Active/Passive Scan
- **Wi-Fi STA (Security Modes)**
  - Open Mode, WPA2 Personal, WPA2 Enhancements, WPA3 Personal, Mixed Mode (WPA/WPA2), WPA3 Personal Transition Mode (WPA2/WPA3)
- **WPA2 Enterprise security (STA)**
  - Method
    - PEAP/TTLS/TLS 1.0/TLS 1.2/FAST
- **Wi-Fi STA Rejoin**
- **Wi-Fi STA Roaming**
  - BG Scan, OKC (Opportunistic Key caching), PMK (Pairwise Master Key) caching, Pre-Authentication
- **Wi-Fi Protocol Power Save** 
  - Deep sleep (unconnected state), Max PSP, Enhanced Max PSP, Fast PSP,** TWT
- **QoS**
  - WMM-QoS
- **Wi-Fi 6 Feature**
  - MUMIMO (DL), OFDMA (UL/DL), iTWT, TWT I-Frame & TWT Enhancements, BSS coloring, MBSSID
- **Wi-Fi Concurrency** 
  - AP+STA (Same channel)
- **Wi-Fi Band/Channels**
  - 2.4GHz CH1-11, 2.4GHz CH1-13, 2.4GHz CH1-14 (Japan)
- **Known Security Vulnerabilities Handled**
  - WPA2 KRACK Attacks, Fragment and Forge Vulnerability

### **Network Stack**

- **Core Networking Features**
  - TCP/IP Bypass (LWIP as Hosted stack for reference), IPv4/IPv6/UDP/TCP/ARP/ICMP/ICMPv6
  - SSL client versions TLSV1.0, TLSV1.2, TLSV1.3 
  - SSL server versions TLSV1.0 and TLSV1.2
  - DHCP (Client/Server)/ DHCPv6 Client
- **Advanced Network Features**
  - HTTP Client/HTTPS Client//DNS Client/SNTP Client, Embedded MQTT/MQTT on host
- **Wi-Fi IoT Cloud Integration**
  - AWS IOT Core
- BSD and IoT sockets application programming interface(API)

### **BLE** 

- GAP(Advertising, Scanning, initiation, Connection and Bonding)
- Generic Attribute Protocol(GATT)
- Attribute protocol(ATT)
- Security
- LL Privacy 1.2
- Accept list
- Directed Advertising
- Extended Advertising
- Periodic Advertising
- Periodic Advertising scanning
- Extended Advertising scanning
- Periodic Advertising list
- LE periodic advertising synchronization
- LE PHY(1Mbps, 2Mbps) & Coded PHY(125Kbps, 500kbps)
- Simultaneous scanning on 1Mbps and Coded PHY
- LE dual role topology
- LE data packet length extensions(DLE)
- Asymmetric PHYs
- LE channel selection algorithm 2 (CSA#2)
- LE Secure connections
- Bluetooth 5.4 Qualified

### **SDK**

- Simplified and Unified DX for Wi-Fi API and Platform APIs
- Simplifies application development and presents clean and standardized APIs
- UC (Universal Configurator) enables componentization, simplifying configuration of peripherals and examples
- BSD and ARM IoT-compliant socket API
- Available through Simplicity Studio and GitHub

### **Multi-protocol**

- Wi-Fi STA + BLE

### **PTA/Coexistence**

- 3 wire coex acting as Wi-Fi with external Bluetooth
- 3 wire coex acting as Wi-Fi with external Zigbee/OT

## **Changes in this release compared to v3.1.1-Doc Release**

### **System**

None

### **MCU**

- **Enhancements**
  - Provided API for configuring ULP GPIO pin functionality on SoC GPIO pin.
- **Fixed Issues**
  - Improved Joystick interface noise filtering.
  - Provided fix for SYSRTC callback unregister for Capture channel.
  - Provided resolution for SI70xx RHT sensor no\_hold\_read feature.
  - Provided unregister callback API in all the Peripheral deinit API.

### **SDK**

- **Enhancements**
  - Resolved warnings in the featured and snippet demos.
  - sl\_wifi\_get\_sta\_tsf() API added to get the station TSF time which is synchronized with connected AP beacon TSF.
  - Added “update gain” demo for BLE.
  - Added an option to test user gain table feature in "ble\_per" demo by enabling the “GAIN\_TABLE\_AND\_MAX\_POWER\_UPDATE\_ENABLE” macro.
  - Removed unused input parameters in rsi\_ble\_set\_ble\_tx\_power() API.
- **Fixed Issues**
  - Fixed issue with BSSID based connection.
  - Fixed issue with the file descriptors set handling for socket select.
  - Fixed issue with authentication and association timeout is not reflecting as per timeout configuration using sl\_si91x\_configure\_timeout() API
  - Fixed profile id corruption issue for IPV6 demos. 
  - Fixed application hang issue at the AE Clear Periodic Adv list API when used "ble\_ae\_central" example. 
  - Fixed issue where BLE connect API is failing because of 0xFFFFFFE2(RSI\_ERROR\_RESPONSE\_TIMEOUT) error with ble\_central application.
  - Fixed HID data transfer issue when power save is enabled. 

### **Wi-Fi/Network Stack**

- **Enhancements**
  - Added support for TSF time which is synchronized with connected AP beacon TSF.
  - Improve PER performance at max input power levels for 11b.
  - Improved ppm error across channels at room temperature.
  - Added Wi-Fi regulatory table for ACx.
  - Updated regulatory tables of WLAN.
- **Fixed Issues**
  - Increased supported length for Embedded MQTT username up to 120 bytes and topic length up to 200 bytes.
  - Added support to process long ICMP ping response upto packet size of 1472 Bytes for IPv4 and 1452 Bytes for IPv6.
  - Issue with configuration of TCP MSS through setsockopt() has been fixed.
  - Fixed Listen interval based power save issue when IPV6 mode is enabled
  - Added fix for missing RSN IE in first few AP beacons.

### **BLE**

- **Enhancements**
  - Added Region based maximum power index support for both 0dBm & 10dDm LP chains.
- **Fixed Issues**
  - Fixed regulatory output power violations with default region based settings in HP chain.
  - Fixed an issue that caused the Set local name API to fail with the error code 0x4e66[Invalid Name length] when a local name given with 16 bytes of length.

### **Multi-protocol**

- **Enhancements**
  - None
- **Fixed Issues**
  - None

## **Recommendations**

### **System**

- The current revision of SiWx917 has:
  - RAM memory of 672k bytes which can be shared between TA and M4 processors in SoC mode. 
  - The below configurations are applicable in SoC mode and can be configured based on the application requirement. EXT\_FEAT\_352K\_M4SS\_320K is the default configuration, based on requirement EXT\_FEAT\_480K\_M4SS\_192K configuration is selected for SoC mode multi-protocol examples.
    - EXT\_FEAT\_480K\_M4SS\_192K - This mode configures TA with 480k and M4 with 192K bytes of memory
    - EXT\_FEAT\_416K\_M4SS\_256K - This mode configures TA with 416k and M4 with 256K bytes of memory
    - EXT\_FEAT\_352K\_M4SS\_320K - This mode configures TA with 352k and M4 with 320K bytes of memory
  - SoC mode should not use 672k\_M4SS\_0K memory configuration.
- Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
- There are 2 Versions of Pro-Kits/Radio boards.  Si917-6031A based on Si917-4338A and SiWx917-6031A based on SiWx917-4338A. To get optimal power numbers, enable macro "SL\_SI91X\_ENABLE\_LOWPWR\_RET\_LDO" pre-processor define for ICs or while using SiWx917-6031A Pro-kit, SiWx917-4338A version of boards. This macro should be disabled for earlier variant of board (Si917-6031A, Si917-4338A).
- With RAM configuration (EXT\_FEAT\_352K\_M4SS\_320K), only 352K memory is available to TA  which limits the features supported, Recommended to enable EXT\_FEAT\_416K\_M4SS\_256K in Wi-Fi + BLE Multi protocol mode to enable more Network features.
- For EXT\_FEAT\_416K\_M4SS\_256K  and EXT\_FEAT\_480K\_M4SS\_192K memory configurations, it is recommended to retain both TA and M4 RAMs in power save.

### **Wi-Fi/Network Stack**

- It is recommended to enable bit 16 of the 'Extended TCP IP Feature' bit map in the opermode command for all Wi-Fi Socket operations from the host to ensure graceful handling during asynchronous closures from the peer.
- For high throughput applications, aggregation (bit 2 of feature\_bit\_map) is recommended to be enabled in opermode. 
- To reset TA (NWP), the application needs to call sl\_wifi\_deinit() followed by sl\_wifi\_init(). Users must ensure that no execution happens from flash at this point of time. (Refer to example applications) 
- Users can enable SL\_SI91X\_EXT\_TCP\_IP\_SSL\_16K\_RECORD in 'Extended TCP IP Feature' bit map in opermode for (HTTPS server) supporting 16k record.
- **TWT**
  - Recommendation is to use sl\_wifi\_target\_wake\_time\_auto\_selection() API for all TWT applications. 
  - It is recommended to issue iTWT setup command once IP assignment, TCP connection, application specific socket connections are done.
  - When using sl\_wifi\_enable\_target\_wake\_time API, increase TCP / ARP Timeouts at the remote side depending upon the configured TWT interval configured. It's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() as an alternative.
  - In case of TWT in coex mode, when using sl\_wifi\_enable\_target\_wake\_time API, use TWT wake duration <= 16 ms and TWT wake interval >= 1 sec. If wake duration > 16 ms or TWT wake interval < 1sec, there might be performance issues.
  - For iTWT GTK interval in AP should be configured to max possible value or zero. If GTK interval is not configurable on AP side, recommended TWT interval (in case of sl\_wifi\_enable\_target\_wake\_time API) or RX Latency (in case of sl\_wifi\_target\_wake\_time\_auto\_selection API) is less than 4sec.
  - When sl\_wifi\_enable\_target\_wake\_time API is used, configuring TWT Wake interval beyond 1 min might lead to disconnections from the AP. Recommended to use TWT wake interval of less than or equal to 1 min.
  - When using sl\_wifi\_enable\_target\_wake\_time API, it is recommended to set missed\_beacon\_count of sl\_wifi\_set\_advanced\_client\_configuration API greater than 2 times of the configured TWT Interval.
- Disable power save for high throughput applications or use FAST PSP power save mode as per application requirement.
- The application needs to ensure that it sets RTC with the correct timestamp before establishing the SSL/EAP connection.
- The minimum timeout value should not be less than 1 second for socket select and socket receive calls. 
- Embedded MQTT keep alive interval should be either 0 or a value greater than 35 seconds.
- Disable power save and suspend any active TWT sessions before triggering HTTP OTAF.
- Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with the power save.

### **BLE**

- In BLE, the recommended range of Connection Interval in
  - Power Save (BLE Only) - 100 ms to 1.28 s.
- In BLE, during Connection, the configuration of Scan Interval and Scan Window with the same value is not recommended. The suggested ratio of Scan Window to Scan Interval is 3:4.
- In BLE, if a device is acting as Central, the scan window (in set\_scan\_params and create\_connection commands) must be less than the existing Connection Interval. The suggested ratio of Scan Window to Connection Interval is 2:3.
- In BLE mode, if scanning and advertising are in progress on the SiWx91x module and it subsequently gets connected and moves to the central role, scanning stops else if it moves to the peripheral role, advertising stops. To further establish a connection to another peripheral device or to a central device, the application should give a command for starting advertising and scanning again.

### **Multi-protocol**

- For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
- Wi-Fi + BLE Advertising
  - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals may be observed.
  - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity.
- Wi-Fi + BLE scanning
  - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
  - BLE scanning will be stopped for intervals that collide with Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Connections
  - All standard connection intervals are supported.
  - For a stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Data Transfer
  - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout.
  - Ensure Wi-Fi activity consumes lower intervals.

## **Known Issues of WiSeConnect3\_SDK\_3.1.2 Release**

### **MCU**

**SensorHub**

- SensorHub does not support, SPI-based sensors and ADC multi-channel. 

**SSI**

- SSI Dual and Quad mode are not functional. 
- SSI primary: mode3(TI) is not working as expected. 
- SSI Primary will not be operable at 40 MHz 

**I2C**

- I2C High speed and Fastplus modes are not working at specification speed

**UART**

- Flow control is not supported in USART and IOSTREAM applications

**ADC**

- In ADC Multichannel, sampling rate is limited to less than 15Ksps. Number of samples is less than 500 and with a maximum of 4 channel
- In ADC Multichannel, when using different sampling rates for different channels behavior is not as expected

**CT (Config Timer)**

- Config Timer(CT) support as an external event, in 32-bit mode, and DMA are not working as expected

**PWM**

- PWM Special event trigger for synchronizing analog-to-digital conversions is not working

**SYSRTC**

- SYSRTC with RO clock is not functioning

**Power Manager**

- The wakeup sources ULP-SSI, BOD, Comparator, and SysRTC are not working in the Power Manager service

### **SDK**

- Observed Wi-Fi connection is successful even after deleting the stored network credentials using sl\_net\_delete\_credential and responding with SL\_NET\_INVALID\_CREDENTIAL\_TYPE for sl\_net\_get\_credential.
- Observed sl\_net\_deinit is not returning any response for both "SL\_NET\_WIFI\_AP\_INTERFACE" and "SL\_NET\_WIFI\_CLIENT\_INTERFACE" interfaces, which requires to execute deinit code path to RAM in case of common flash.
- Enhanced sl\_wifi\_get\_firmware\_version() API to provide more details (ROM ID, chip ID, security version, etc) which is not backward compatible with firmwares older than 1711.2.10.1.0.0.4. Firmware binary notation does not include security version number.
- Matter applications are experiencing compatibility issues with WiseConnect SDK 3.1.2. It is recommended to use WiseConnect SDK 3.1.1 for matter-related applications. This will be addressed in up coming release(s).

### **Wi-Fi/Network Stack**

**Wi-Fi STA**

- STA Connection with the WPA3 Hunting and Pecking algorithm takes about 3-4 seconds.
- Observed connection failures with some AP's in more channel congestion ~50-60% occupancy open lab environment.
- Region selection based on country IE in the beacon is not supported
- Observed intermittent beacon reception from Access Point ( beacon misses) in an extremely congested environment
- Note section in API documentation for sl\_wifi\_get\_sta\_tsf  needs to be corrected to "Returns error if station is not connected or at least one beacon is not received."

**Access Point (AP) Mode**

- Scan feature in AP mode not fully functional. 
- Fixed rate configuration in AP mode using sl\_wifi\_set\_transmit\_rate API is not being set as expected. 

**WPA2 Enterprise security (STA)**

- Observed connection issue with configuring certificate key and programming 4096 bit key and SHA384/SHA512 certificates.
- Observing DUT is throwing 0x1001c when configuring .data.certificate\_key as "123456789".
- Configuration issue to program PAC file for EAP FAST Manual mode.
- Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.

**Wi-Fi Concurrency (AP + STA in same channel)**

- Observed 3rd party STA association fail with 917 AP while 917 STA mode is connecting/reconnecting to configured 3rd party AP. Reconnect 3rd party STA to 917 AP in such scenarios. 

**OFDMA (UL/DL)**

- Less throughput observed in DL-OFDMA with some APs that enabled LDPC.

**MUMIMO (DL)**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Performance, Interop issues with MU MIMO with certain APs. 
- Less throughput observed in MU-MIMO with some APs that enabled LDPC.

**TWT**

- Variability with MQTT keep alive interval based on TWT wake up interval.  
- When sl\_wifi\_enable\_target\_wake\_time() API is used, occasional MQTT disconnections may be observed if TWT is configured with longer TWT intervals (>30secs) with embedded MQTT + TWT.  As an alternative, it's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() API, where these dependencies are internally handled.

**Wi-Fi STA Rejoin**

- Observed Scanning (probe request) in all channels instead of the channels configured in selective channel(channel\_bitmap\_2g4) during rejoin process. 

**IPv4/IPv6**

- Observed issue with assigning static IP to DUT
- IP change notification is not indicated to the application. 

**BSD Socket API**

- Configuration issue to program TCP retry count (default 10) through setsockopt
- Every server socket created consumes a socket (maximum of 10 sockets supported) and every subsequent connection to server socket consumes an additional socket (from the same pool of 10 sockets), which limits the number of server connections supported.

**SSL Client/Server**

- Sometimes during SSL Handshake, ECC curve parameters generated are wrong, resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
- Secure SSL renegotiation is not supported in the Embedded Networking Stack
- Observed connection failures with ECDSA ciphers.

**HTTP Client/ HTTPS Client**

- Observed occasional HTTPS continuous download failures when power save is enabled. Recommended to disable it before performing HTTPS continuous downloads
- Observed HTTPS post data is failing with error code: BBE2 (HTTP Failed).

**SNTP**

- Unable to get SNTP async events when Coex mode and power save are enabled 

**Throughputs & Performance**

- Wi-Fi alone throughput is about SDK 3.x (42Mbps). SDK refinements are in progress to further improve Wi-Fi Standalone and Coex Throughputs. 

**Secure Over the Air (OTA) Upgrade**

- Observed firmware upgrade failures after multiple iterations.
- Observed OTA failures with powersave enable, So recommended to disable powersave during OTA. 
- Observed "0xffffffff" error at the remote end while doing firmware upgrade via TCP server using (featured) example with ubuntu 21.0x. 

**Wi-Fi IOT Cloud integration (AWS IOT Core)**

- Observed AWS MQTT keepalive transmission is not happening at expected intervals with power save enabled.

**Wi-Fi Interoperability (IOP)**

- Observed disconnections with Amplifi (AFI-INS-R) AP with powersave enable
- TWT session is failing due to disconnections observed in DUT if rx\_latency is set to 55 seconds and receive data is also set to 55 seconds on MI Xiaomi RA72 and Tplink AX53 AP's
- Observed less throughput(~1Mb) while running TCP RX with Max\_PSP powersave with DLink 810 AP
- Observed interop issue (random disconnections) with few APs (EERO 6+, EERO PRO 6E, Cisco Catalyst 9120AXID)
- Disconnections observed with Netgear RAX120 AP in WPA3 Security

### **BLE**

**GAP** 

- BLE reconnection is not working in ble\_multiconnection\_gatt demo when power save is enabled in SoC. 
- When power save is enabled in the SoC, the DUT hangs with the ble\_multiconnection\_gatt demo while performing continuous Tx/Rx notifications.

**DTM/PER**

- Recommend to limit BLE Tx Maximum power to 18 dBm.  Please don't use for 127 power\_index for BLE HP chain with this release.

**IOP**

- BLE connection failure has been observed on a few mobile devices (Samsung S23& Oneplus 9pro) when the Si917 device is configured as central.

### **Multi-protocol**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Wi-Fi + BLE intermittent connection failures, disconnections, and data transfer stalls in the long run when power save is enabled.
- Observed "DUT is not disconnecting to the AP when initiating disconnection from EFR connect app screen using wifi\_station\_ble\_provisioning\_aws example

### **System**

- Observed random hang issues with encrypted firmwares on some earlier variant of boards  (Si917-6031A, Si917-4338A) with powersave enable.

### **Simplicity Studio and Commander** 

- Simplicity commander does not support options under "Debug Lock tools".
- All projects in the package are compatible with **GNU ARM V12.2.1** toolchain

## **Planned Roadmap Features**

### **SDK**

- WPS 2.0 PIN/PUSH, Network Manager Enhancements
- MDNS, HTTP Server, Azure MQTT Client, Websockets
- Matter integration with Embedded TCP/IP stack 
- Enhanced buffer management, Throughput optimization
- TLS over LWIP, Network layer applications (MQTT, HTTP, etc.) over LWIP.
- Debugging utilities, UC Configurator for Wireless configurations

### **System**

- Secure communication over CPC (UART & SDIO interfaces)

### **MCU**

- Pin Tool for MCU 
- Dual Flash: Separate flash for Cortex M4 and Wireless Processor
- Peripherals: Analog Comparator, DAC, Temp Sensor, Capacitive Touch Sensor, OPAMP, HSPI, GPDMA, QEI, RNG, IR decoder, CRC and BOD
- Software-configurable MCU application memory options (192 or 256 or 320 kB)
- RTOS support for MCU Peripheral examples
- SensorHub: Support for SDC\_ADC and CLOUD connectivity
- SIO support for features Pattern matching, DMA flow control, self-loop mechanism and as a clock qualifier
- Support for I2S PCM functionality
- Stacked and external PSRAM: Pseudo Static RAM for storing application data and executing SoC applications. 

### **Wi-Fi/Network Stack**

- Spatial Re-Use, BSS Max Idle
- Provisioning using Wi-Fi AP

### **BLE**

- Support for 8 Peripheral + 2 Central connections 
- Support for BLE Mesh (4 nodes only) for limited Mesh use case

## **Limitations and Unsupported Features**  

### **System**

- None

### **SDK**

- Baremetal mode is not supported.

### **Wi-Fi/Network Stack**

- TLS 1.3 Server is not supported.
- 40 MHz bandwidth for 2.4 GHz band is not supported.
- Max 3 SSL sockets in Wi-Fi only mode, 2 SSL Socket in Wi-Fi + BLE based on RAM memory configuration selected. 
- In SSL ECC Curve ID supported is 23. SSL handshake with 3rd party clients depends on the SSL ECC Curve ID.
- The number of Non-Transmitting BSSIDs processed is limited by the beacon length that can be processed by the stack (which is 1024 bytes). Beacons greater than 1024 Bytes in length will not be processed.
- Multiprotocol (STA +BLE) + EAP Security modes supported only with Memory configurations EXT\_FEAT\_416K\_M4SS\_256K and EXT\_FEAT\_480K\_M4SS\_192K.
- UL MUMIMO is not supported.
- WPA3 AP supports only H2E algorithm.
- PMKSA caching is not supported in WPA3 AP mode.
- Maximum embedded MQTT Publish payload is 1 kbyte.
- Timeout value for socket select and socket receive calls of less than 1 second is not currently supported.

### **BLE**

- For BLE, if the connection is established with a small connection interval (less than 15 ms), simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported.
- BLE maximum two concurrent connections are supported, which can be either a connection to two peripheral devices, to one central and one peripheral device or two central devices.
- BLE Slave latency value is valid up to 32 only.
- Maximum supported AE data length is 200 bytes.
- Supports only two ADV\_EXT sets.
- Supports only two BLE connections (1 Central and 1 Peripheral) with AE.
- Advertising Extension feature is not supported in Coexistence.
- The  ae\_central & ae\_peripheral applications are not supported with TA\_352K\_M4\_320K RAM configuration.
- Two BLE connections are not supported with M4 powersave. It only supports a single connection. 
- Isochronous channels feature is not supported. 
- Connection subrating feature is not supported. 
- LE power controller feature is not supported.
- EATT feature is not supported.
- Periodic Advertising with a response feature is not supported. 
- BLE Audio is not supported.
- The feature of dynamically changing the TX power when extended advertising is active is not supported.

### **MCU**

- SensorHub supports PS1-power state with ADC sensor. In this mode, other sensors operation is not supported. 

### **Multi-protocol**

- Wi-Fi AP + BLE currently not supported. 

<br>

# **WiSeConnect3\_SDK\_3.1.1-Hotfix SoC Release Notes**

## **Release Details**

|**Item**|**Details**|
| :- | :- |
|Release date|Jan 2nd, 2024|
|SDK Version|3\.1.1-Hotfix|
|Firmware Version|1711\.2.10.1.0.0.7|
|Release Package Name|WiSeConnect3\_SDK\_3.1.1-Hotfix|
|Supported RTOS|FreeRTOS|
|Operating Modes Supported|Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP|

- SiWx917 release consists of two components:
  - Wireless Firmware - SiWx917 Firmware Binary available as SiWG917-B.2.10.1.0.0.x.rps
  - Wiseconnect3 Library - Wiseconnect3 SDK library runs on internal Cortex M4

**NOTE:** Mandatory to upgrade the earlier version of boards (Si917-6031A Prokit or 4338A boards) or 917 Silicon ICs with instructions as outlined in this document "SiWG917–TA\_Flash\_Memory\_Map\_Change\_Guide\_v1.3.pdf" for more details.  

## **Supported Hardware**


|**Hardware**|**OPN**|
| :- | :- |
|Silicon (IC OPN)|SiWG917M111MGTBAR|
|Development Kits|<p>Pro Kit: SiWx917-PK6031A, Si917-PK6031A. </p><p>(Pro Kit includes Mother board "SI-MB4002A" + Radio board)</p><p>Radio boards: SiWx917-RB4338A, Si917-RB4338A </p>|

## **Supported Features** 

### **System**

- **Operating Modes** 
  - Wi-Fi STA (802.11ax, 802.11n), Wi-Fi 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + BLE
- **Security** 
  - Secure Boot, Secure Key storage and HW device identity with PUF, Secure Zone, Secure XIP (Execution in place) from flash, Secure Attestation, Anti Rollback, Secure Debug. Flash Protection. 
  - Secure firmware upgrade options:
    - Firmware loading support by Commander Tool through Jlink Debugger. Jlink connected to Serial Wire Debug (SWD) 
    - Firmware loading via ISP using UART (Commander or Serial terminal), SPI Interface
    - Secure Over the Air (OTA) Upgrade
- **Crypto Support**
  - Crypto API's for Hardware Accelerators:
    - Advanced Encryption Standard (AES) 128/256/192, Secure Hash Algorithm (SHA) 256/384/512, Hash Message Authentication Code (HMAC), Random Number Generator (RNG), SHA3, AES-Galois Counter Mode (GCM)/ Cipher based Message Authentication Code (CMAC), ChaCha-poly, True Random Number Generator (TRNG)
  - Software Accelerators: RSA, ECC 
  - Integrated with mbed TLS.
  - TRNG, AES, SHA, HMAC, GCM, Chacha-poly, ECDH and AES-CCM hardware crypto engine is integrated with PSA crypto.
- **System Power Save**
  - Deep Sleep with RAM retention and without RAM retention. 
  - Wireless Power Save: Connected Sleep (Wi-Fi Standby Associated), BLE Advertising with powersave, BLE Scan with powersave ,  BLE connection with powersave. Only Max PSP power save mode is supported in BLE. 

### **MCU**

- **Memory**
  - Common Flash: Single shared Flash for both Cortex-M4 and NWP (Wireless Processor)
- **Power States**
  - Active: PS4, PS3, PS2, and PS1 
  - Standby: PS4, PS3, and PS2
  - Sleep: PS4, PS3 and PS2
  - Deep Sleep(Shutdown): PS0
- **Peripherals**
  - High Performance (HP)- ADC, CALENDAR, CONFIG TIMER(CT), EFUSE, EGPIO, GSPI, I2C, I2S, MCPWM, SDIO Secondary, SIO, SSI, SYSRTC, UART, uDMA, USART, WDT** 
  - Ultra Low Power (ULP) - ULP\_TIMER, ULP\_UART, ULP\_I2C, ULP\_I2S, ULP\_uDMA and ULP\_SSI
- **Services**
  - Sleep Timer, IOSTREAM, Power Manager, SensorHub, NVM3
- **Development Pro-Kit Peripheral Support**
  - LED, Button, MEMLCD, Joystick, RHT Sensor

### **Development Environment**

- Simplicity Studio IDE (SV5.8.0 version) and Debugger Integration.   Refer to the latest version of the SoC "Getting-Started-with-SiWx917" guide for more details. 
- Simplicity Commander to supports Flash loading, provision of MBR programming, security key management, and calibration support for crystal and gain offsets. refer "siwx917-soc-manufacturing-utility-user-guide" for more details.** 
- Advanced energy measurement for ultra-low power capability available on Development boards (Pro Kit)

### **Wi-Fi**

- **Wi-Fi Protocols**
  - IEEE 802.11 b/g/n/ax (2.4GHz)
- **Access Point (AP) Mode**
  - 4 Client Support, Hidden SSID Mode, Auto Channel Selection, Scan in AP mode
  - Wi-Fi Security  
    - WPA2 Personal, WPA3 Personal (H2E method only), WPA Mixed mode (WPA/WPA2) 
- **Wi-Fi Scan**
  - Selective Scan, Active/Passive Scan
- **Wi-Fi STA (Security Modes)**
  - Open Mode, WPA2 Personal, WPA2 Enhancements, WPA3 Personal, Mixed Mode (WPA/WPA2), WPA3 Personal Transition Mode (WPA2/WPA3)
- **WPA2 Enterprise security (STA)**
  - Method
    - PEAP/TTLS/TLS 1.0/TLS 1.2/FAST
- **Wi-Fi STA Rejoin**
- **Wi-Fi STA Roaming**
  - BG Scan, OKC (Opportunistic Key caching), PMK (Pairwise Master Key) caching, Pre-Authentication
- **Wi-Fi Protocol Power Save** 
  - Deep sleep (unconnected state), Max PSP, Enhanced Max PSP, Fast PSP,** TWT
- **QoS**
  - WMM-QoS
- **Wi-Fi 6 Feature**
  - MUMIMO (DL), OFDMA (UL/DL), iTWT, TWT I-Frame & TWT Enhancements, BSS coloring, MBSSID
- **Wi-Fi Concurrency** 
  - AP+STA (Same channel)
- **Wi-Fi Band/Channels**
  - 2.4GHz CH1-11, 2.4GHz CH1-13, 2.4GHz CH1-14 (Japan)
- **Known Security Vulnerabilities Handled**
  - WPA2 KRACK Attacks, Fragment and Forge Vulnerability

### **Network Stack**

- **Core Networking Features**
  - TCP/IP Bypass (LWIP as Hosted stack for reference), IPv4/IPv6/UDP/TCP/ARP/ICMP/ICMPv6
  - SSL client versions TLSV1.0, TLSV1.2, TLSV1.3 
  - SSL server versions TLSV1.0 and TLSV1.2
  - DHCP (Client/Server)/ DHCPv6 Client
- **Advanced Network Features**
  - HTTP Client/HTTPS Client//DNS Client/SNTP Client, Embedded MQTT/MQTT on host
- **Wi-Fi IoT Cloud Integration**
  - AWS IOT Core
- BSD and IoT sockets application programming interface(API)

### **BLE** 

- GAP(Advertising, Scanning, initiation, Connection and Bonding)
- Generic Attribute Protocol(GATT)
- Attribute protocol(ATT)
- Security
- LL Privacy 1.2
- Accept list
- Directed Advertising
- Extended Advertising
- Periodic Advertising
- Periodic Advertising scanning
- Extended Advertising scanning
- Periodic Advertising list
- LE periodic advertising synchronization
- LE PHY(1Mbps, 2Mbps) & Coded PHY(125Kbps, 500kbps)
- Simultaneous scanning on 1Mbps and Coded PHY
- LE dual role topology
- LE data packet length extensions(DLE)
- Asymmetric PHYs
- LE channel selection algorithm 2 (CSA#2)
- LE Secure connections
- Bluetooth 5.4 Qualified

### **SDK**

- Simplified and Unified DX for Wi-Fi API and Platform APIs
- Simplifies application development and presents clean and standardized APIs
- UC (Universal Configurator) enables componentization, simplifying configuration of peripherals and examples
- BSD and ARM IoT-compliant socket API
- Available through Simplicity Studio and GitHub

### **Multi-protocol**

- Wi-Fi STA + BLE

### **PTA/Coexistence**

- 3 wire coex acting as Wi-Fi with external Bluetooth
- 3 wire coex acting as Wi-Fi with external Zigbee/OT

## **Changes in this release compared to v3.1.1-Doc Release**

### **System**

- None

### **MCU**

- None

### **SDK**

- None

### **Wi-Fi/Network Stack**

- **Fixed Issues**
  - Updated WLAN regulatory power tables.

### **BLE**

- **Fixed Issues**
  - Updated BLE regulatory power tables.

### **Multi-protocol**

- None

## **Recommendations**

### **System**

- The current revision of SiWx917 has:
  - RAM memory of 672k bytes which can be shared between TA and M4 processors in SoC mode. 
  - The below configurations are applicable in SoC mode and can be configured based on the application requirement. EXT\_FEAT\_352K\_M4SS\_320K is the default configuration, based on requirement EXT\_FEAT\_480K\_M4SS\_192K configuration is selected for SoC mode multi-protocol examples.
    - EXT\_FEAT\_480K\_M4SS\_192K - This mode configures TA with 480k and M4 with 192K bytes of memory
    - EXT\_FEAT\_416K\_M4SS\_256K - This mode configures TA with 416k and M4 with 256K bytes of memory
    - EXT\_FEAT\_352K\_M4SS\_320K - This mode configures TA with 352k and M4 with 320K bytes of memory
  - SoC mode should not use 672k\_M4SS\_0K memory configuration.
- Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
- There are 2 Versions of Pro-Kits/Radio boards.  Si917-6031A based on Si917-4338A and SiWx917-6031A based on SiWx917-4338A. To get optimal power numbers, enable macro "SL\_SI91X\_ENABLE\_LOWPWR\_RET\_LDO" pre-processor define for ICs or while using SiWx917-6031A Pro-kit, SiWx917-4338A version of boards. This macro should be disabled for earlier variant of board (Si917-6031A, Si917-4338A).
- With RAM configuration (EXT\_FEAT\_352K\_M4SS\_320K), only 352K memory is available to TA  which limits the features supported, Recommended to enable EXT\_FEAT\_416K\_M4SS\_256K in Wi-Fi + BLE Multi protocol mode to enable more Network features.
- For EXT\_FEAT\_416K\_M4SS\_256K and EXT\_FEAT\_480K\_M4SS\_192K memory configurations, it is recommended to retain both TA and M4 RAMs in power save.

### **Wi-Fi/Network Stack**

- It is recommended to enable bit 16 of the 'Extended TCP IP Feature' bit map in the opermode command for all Wi-Fi Socket operations from the host to ensure graceful handling during asynchronous closures from the peer.
- For high throughput applications, aggregation (bit 2 of feature\_bit\_map) is recommended to be enabled in opermode. 
- To reset TA (NWP), the application needs to call sl\_wifi\_deinit() followed by sl\_wifi\_init(). Users must ensure that no execution happens from flash at this point of time. (Refer to example applications) 
- Users can enable SL\_SI91X\_EXT\_TCP\_IP\_SSL\_16K\_RECORD in 'Extended TCP IP Feature' bit map in opermode for (HTTPS server) supporting 16k record.
- **TWT**
  - Recommendation is to use sl\_wifi\_target\_wake\_time\_auto\_selection() API for all TWT applications. 
  - It is recommended to issue iTWT setup command once IP assignment, TCP connection, application specific socket connections are done.
  - When using sl\_wifi\_enable\_target\_wake\_time API, increase TCP / ARP Timeouts at the remote side depending upon the configured TWT interval configured. It's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() as an alternative.
  - In case of TWT in coex mode, when using sl\_wifi\_enable\_target\_wake\_time API, use TWT wake duration <= 16 ms and TWT wake interval >= 1 sec. If wake duration > 16 ms or TWT wake interval < 1sec, there might be performance issues.
  - For iTWT GTK interval in AP should be configured to max possible value or zero. If GTK interval is not configurable on AP side, recommended TWT interval (in case of sl\_wifi\_enable\_target\_wake\_time API) or RX Latency (in case of sl\_wifi\_target\_wake\_time\_auto\_selection API) is less than 4sec.
  - When sl\_wifi\_enable\_target\_wake\_time API is used, configuring TWT Wake interval beyond 1 min might lead to disconnections from the AP. Recommended to use TWT wake interval of less than or equal to 1 min.
  - When using sl\_wifi\_enable\_target\_wake\_time API, it is recommended to set missed\_beacon\_count of sl\_wifi\_set\_advanced\_client\_configuration API greater than 2 times of the configured TWT Interval.
- Disable power save for high throughput applications or use FAST PSP power save mode as per application requirement.
- The application needs to ensure that it sets RTC with the correct timestamp before establishing the SSL/EAP connection.
- The minimum timeout value should not be less than 1 second for socket select and socket receive calls. 
- Embedded MQTT keep alive interval should be either 0 or a value greater than 35 seconds.
- Disable power save and suspend any active TWT sessions before triggering HTTP OTAF.
- Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with the power save.

### **BLE**

- In BLE, the recommended range of Connection Interval in
  - Power Save (BLE Only) - 100 ms to 1.28 s.
- In BLE, during Connection, the configuration of Scan Interval and Scan Window with the same value is not recommended. The suggested ratio of Scan Window to Scan Interval is 3:4.
- In BLE, if a device is acting as Central, the scan window (in set\_scan\_params and create\_connection commands) must be less than the existing Connection Interval. The suggested ratio of Scan Window to Connection Interval is 2:3.
- In BLE mode, if scanning and advertising are in progress on the SiWx91x module and it subsequently gets connected and moves to the central role, scanning stops else if it moves to the peripheral role, advertising stops. To further establish a connection to another peripheral device or to a central device, the application should give a command for starting advertising and scanning again.

### **Multi-protocol**

- For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
- Wi-Fi + BLE Advertising
  - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals may be observed.
  - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity.
- Wi-Fi + BLE scanning
  - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
  - BLE scanning will be stopped for intervals that collide with Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Connections
  - All standard connection intervals are supported.
  - For a stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Data Transfer
  - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout.
  - Ensure Wi-Fi activity consumes lower intervals.

## **Known Issues of WiSeConnect3\_SDK\_3.1.1 Release**

### **MCU**

**SensorHub**

- SensorHub does not support, SPI-based sensors and ADC multi-channel. 

**SSI**

- SSI Dual and Quad mode are not functional. 
- SSI primary: mode3(TI) is not working as expected. 
- SSI Primary will not be operable at 40 MHz. 

**I2C**

- I2C High speed and Fastplus modes are not working at specification speed

**NVM3**

- When installing Sleeptimer and NVM3 in Studio, is not working as expected. As a workaround these components must be added to project SLCP

**UART**

- Flow control is not supported in USART and IOSTREAM applications

**ADC**

- In ADC Multichannel, sampling rate is limited to less than 15Ksps. Number of samples is less than 500 and with a maximum of 4 channel
- In ADC Multichannel, when using different sampling rates for different channels behavior is not as expected

**CT (Config Timer)**

- Config Timer(CT) support as an external event, in 32-bit mode, and DMA are not working as expected

**PWM**

- PWM Special event trigger for synchronizing analog-to-digital conversions is not working

**SYSRTC**

- SYSRTC with RO clock is not functioning

**Power Manager**

- The wakeup sources ULP-SSI, BOD, Comparator, and SysRTC are not working in the Power Manager service

### **SDK**

- Observed Wi-Fi connection is successful even after deleting the stored network credentials using sl\_net\_delete\_credential and responding with SL\_NET\_INVALID\_CREDENTIAL\_TYPE for sl\_net\_get\_credential.
- Observed sl\_net\_deinit is not returning any response for both "SL\_NET\_WIFI\_AP\_INTERFACE" and "SL\_NET\_WIFI\_CLIENT\_INTERFACE" interfaces, which requires to execute deinit code path to RAM in case of common flash.
- Observed issue when NULL is passed for readfds, writefds, excepfds to socket select. 
- Enhanced sl\_wifi\_get\_firmware\_version() API to provide more details (ROM ID, chip ID, security version, etc) which is not backward compatible with firmwares older than 1711.2.10.1.0.0.4. Firmware binary notation does not include security version number.

### **Wi-Fi/Network Stack**

**Wi-Fi STA**

- STA Connection with the WPA3 Hunting and Pecking algorithm takes about 3-4 seconds.
- Observed BSSID-based connection configuration is not reflecting to make a connection with specific access point.
- Observed connection failures with some AP's in more channel congestion ~50-60% occupancy open lab environment.
- Region selection based on country IE in the beacon is not supported
- Observed auth/assoc timeout is not reflecting as per timeout configuration using sl\_si91x\_configure\_timeout() API 
- Observed intermittent beacon reception from Access Point ( beacon misses) in an extremely congested environment

**Access Point (AP) Mode**

- Scan feature in AP mode not fully functional. 
- Fixed rate configuration in AP mode using sl\_wifi\_set\_transmit\_rate API is not being set as expected. 
- First few beacons (~ 10) don't include RSN IE

**WPA2 Enterprise security (STA)**

- Observed connection issue with configuring certificate key and programming 4096 bit key and SHA384/SHA512 certificates.
- Observing DUT is throwing 0x1001c when configuring .data.certificate\_key as "123456789".
- Configuration issue to program PAC file for EAP FAST Manual mode.
- Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.

**Wi-Fi Concurrency (AP + STA in same channel)**

- Observed 3rd party STA association fail with 917 AP while 917 STA mode is connecting/reconnecting to configured 3rd party AP. Reconnect 3rd party STA to 917 AP in such scenarios. 

**OFDMA (UL/DL)**

- Less throughput observed in DL-OFDMA with some APs that enabled LDPC.

**MUMIMO (DL)**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Performance, Interop issues with MU MIMO with certain APs. 
- Less throughput observed in MU-MIMO with some APs that enabled LDPC.

**TWT**

- Variability with MQTT keep alive interval based on TWT wake up interval.  
- When sl\_wifi\_enable\_target\_wake\_time() API is used, occasional MQTT disconnections may be observed if TWT is configured with longer TWT intervals (>30secs) with embedded MQTT + TWT.  As an alternative, it's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() API, where these dependencies are internally handled.

**Wi-Fi STA Rejoin**

- Observed Scanning (probe request) in all channels instead of the channels configured in selective channel(channel\_bitmap\_2g4) during rejoin process. 

**IPv4/IPv6**

- Observed issue with assigning static IP to DUT
- IP change notification is not indicated to the application. 
- Observed sl\_net\_up returning 0x27 error code due to an issue in the handling of IPv6 address. 

**BSD Socket API**

- Configuration issue to program MSS (default 1460), TCP retry count (default 10) through setsockopt
- Every server socket created consumes a socket (maximum of 10 sockets supported) and every subsequent connection to server socket consumes an additional socket (from the same pool of 10 sockets), which limits the number of server connections supported.

**SSL Client/Server**

- Sometimes during SSL Handshake, ECC curve parameters generated are wrong, resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
- Secure SSL renegotiation is not supported in the Embedded Networking Stack
- Observed connection failures with ECDSA ciphers.

**HTTP Client/ HTTPS Client**

- Observed occasional HTTPS continuous download failures when power save is enabled. Recommended to disable it before performing HTTPS continuous downloads
- Observed HTTPS post data is failing with error code: BBE2 (HTTP Failed).

**SNTP**

- Unable to get SNTP async events when Coex mode and power save are enabled 

**Throughputs & Performance**

- Wi-Fi alone throughput is about SDK 3.x (42Mbps). SDK refinements are in progress to further improve Wi-Fi Standalone and Coex Throughputs. 

**Secure Over the Air (OTA) Upgrade**

- Observed firmware upgrade failures after multiple iterations.
- Observed OTA failures with powersave enable, So recommended to disable powersave during OTA. 
- Observed "0xffffffff" error at the remote end while doing firmware upgrade via TCP server using (featured) example with ubuntu 21.0x. 

**Wi-Fi IOT Cloud integration (AWS IOT Core)**

- Observed AWS MQTT keepalive transmission is not happening at expected intervals with power save enabled.

**Wi-Fi Interoperability (IOP)**

- Observed disconnections with Amplifi (AFI-INS-R) AP with powersave enable
- TWT session is failing due to disconnections observed in DUT if rx\_latency is set to 55 seconds and receive data is also set to 55 seconds on MI Xiaomi RA72 and Tplink AX53 AP's
- Observed less throughput(~1Mb) while running TCP RX with Max\_PSP powersave with DLink 810 AP
- Observed interop issue (random disconnections) with few APs (EERO 6+, EERO PRO 6E, Cisco Catalyst 9120AXID)
- Disconnections observed with Netgear RAX120 AP in WPA3 Security

### **BLE**

**GATT Profile**

- HID profile data transfer is not working when power save is enabled in SoC. 

**GAP** 

- BLE reconnection is not working in ble\_multiconnection\_gatt demo when power save is enabled in SoC. 
- When power save is enabled in the SoC, the DUT hangs with the ble\_multiconnection\_gatt demo while performing continuous Tx/Rx notifications.
- When running the ble\_central application with power save enabled, it was observed that the BLE connect API is failing because of 0xFFFFFFE2(RSI\_ERROR\_RESPONSE\_TIMEOUT) error.
- When BLE local name is provided with a 16-byte length, the Set local name API fails with error code: 0x4e66[Invalid Name length].

**Advertising Extension**

- When running the ble\_ae\_central example, observed an application hang issue at the AE Clear Periodic Adv list API. 

**DTM/PER**

- Recommend to limit BLE Tx Maximum power to 16 dBm.  Please don't use for 127 power\_index for BLE HP release with this release.

**IOP**

- BLE connection failure has been observed on a few mobile devices (Samsung S23& Oneplus 9pro) when the Si917 device is configured as central.

### **Multi-protocol**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Wi-Fi + BLE intermittent connection failures, disconnections, and data transfer stalls in the long run when power save is enabled.
- Observed "DUT is not disconnecting to the AP when initiating disconnection from EFR connect app screen using wifi\_station\_ble\_provisioning\_aws example

### **System**

- Observed random hang issues with encrypted firmwares on some earlier variant of boards  (Si917-6031A, Si917-4338A) with powersave enable.

### **Simplicity Studio and Commander** 

- Simplicity commander does not support options under "Debug Lock tools".
- All projects in the package are compatible with **GNU ARM V12.2.1** toolchain

## **Planned Roadmap Features**

### **SDK**

- WPS 2.0 PIN/PUSH, Network Manager Enhancements
- MDNS, HTTP Server, Azure MQTT Client, Websockets
- Matter integration with Embedded TCP/IP stack 
- Enhanced buffer management, Throughput optimization
- TLS over LWIP, Network layer applications (MQTT, HTTP, etc.) over LWIP.
- Debugging utilities, UC Configurator for Wireless configurations

### **System**

- Secure communication over CPC (UART & SDIO interfaces)

### **MCU**

- Pin Tool for MCU 
- Dual Flash: Separate flash for Cortex M4 and Wireless Processor
- Peripherals: Analog Comparator, DAC, Temp Sensor, Capacitive Touch Sensor, OPAMP, HSPI, GPDMA, QEI, RNG, IR decoder, CRC and BOD
- Software-configurable MCU application memory options (192 or 256 or 320 kB)
- RTOS support for MCU Peripheral examples
- SensorHub: Support for SDC\_ADC and CLOUD connectivity
- SIO support for features Pattern matching, DMA flow control, self-loop mechanism and as a clock qualifier
- Support for I2S PCM functionality
- Stacked and external PSRAM: Pseudo Static RAM for storing application data and executing SoC applications. 

### **Wi-Fi/Network Stack**

- Spatial Re-Use, BSS Max Idle
- Provisioning using Wi-Fi AP

### **BLE**

- Support for 8 Peripheral + 2 Central connections 
- Support for BLE Mesh (4 nodes only) for limited Mesh use case

## **Limitations and Unsupported Features**  

### **System**

- None

### **SDK**

- Baremetal mode is not supported.

### **Wi-Fi/Network Stack**

- TLS 1.3 Server is not supported.
- 40 MHz bandwidth for 2.4 GHz band is not supported.
- Max 3 SSL sockets in Wi-Fi only mode, 2 SSL Socket in Wi-Fi + BLE based on RAM memory configuration selected. 
- In SSL ECC Curve ID supported is 23. SSL handshake with 3rd party clients depends on the SSL ECC Curve ID.
- The number of Non-Transmitting BSSIDs processed is limited by the beacon length that can be processed by the stack (which is 1024 bytes). Beacons greater than 1024 Bytes in length will not be processed.
- Multiprotocol (STA +BLE) + EAP Security modes supported only with Memory configurations EXT\_FEAT\_416K\_M4SS\_256K and EXT\_FEAT\_480K\_M4SS\_192K.
- UL MUMIMO is not supported.
- WPA3 AP supports only H2E algorithm.
- PMKSA caching is not supported in WPA3 AP mode.
- Maximum embedded MQTT Publish payload is 1 kbyte.
- Timeout value for socket select and socket receive calls of less than 1 second is not currently supported.

### **BLE**

- For BLE, if the connection is established with a small connection interval (less than 15 ms), simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported.
- BLE maximum two concurrent connections are supported, which can be either a connection to two peripheral devices, to one central and one peripheral device or two central devices.
- BLE Slave latency value is valid up to 32 only.
- Maximum supported AE data length is 200 bytes.
- Supports only two ADV\_EXT sets.
- Supports only two BLE connections (1 Central and 1 Peripheral) with AE.
- Advertising Extension feature is not supported in Coexistence.
- The  ae\_central & ae\_peripheral applications are not supported with TA\_352K\_M4\_320K RAM configuration.
- Two BLE connections are not supported with M4 powersave. It only supports a single connection. 
- Isochronous channels feature is not supported. 
- Connection subrating feature is not supported. 
- LE power controller feature is not supported.
- EATT feature is not supported.
- Periodic Advertising with a response feature is not supported. 
- BLE Audio is not supported.
- The feature of dynamically changing the TX power when extended advertising is active is not supported.

### **MCU**

- SensorHub supports PS1-power state with ADC sensor. In this mode, other sensors operation is not supported. 

### **Multi-protocol**

- Wi-Fi AP + BLE currently not supported. 

<br>

# **WiSeConnect3\_SDK\_3.1.1-Doc SoC Release Notes**

## **Release Details**

|**Item**|**Details**|
| :- | :- |
|Release date|Dec 20th, 2023|
|SDK Version|3\.1.1-Doc|
|Firmware Version|1711\.2.10.1.0.0.4|
|Release Package Name|WiSeConnect3\_SDK\_3.1.1-Doc|
|Supported RTOS|FreeRTOS|
|Operating Modes Supported|Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP|

- SiWx917 release consists of two components:
  - Wireless Firmware - SiWx917 Firmware Binary available as SiWG917-B.2.10.0.0.4.x.rps
  - Wiseconnect3 Library - Wiseconnect3 SDK library runs on internal Cortex M4

**NOTE:** Mandatory to upgrade the earlier version of boards (Si917-6031A Prokit or 4338A boards) or 917 Silicon ICs with instructions as outlined in this document "SiWG917–TA\_Flash\_Memory\_Map\_Change\_Guide\_v1.3.pdf" for more details.  

## **Supported Hardware**


|**Hardware**|**OPN**|
| :- | :- |
|Silicon (IC OPN)|SiWG917M111MGTBAR|
|Development Kits|<p>Pro Kit: SiWx917-PK6031A, Si917-PK6031A. </p><p>(Pro Kit includes Mother board "SI-MB4002A" + Radio board)</p><p>Radio boards: SiWx917-RB4338A, Si917-RB4338A </p>|

## **Supported Features** 

### **System**

- **Operating Modes** 
  - Wi-Fi STA (802.11ax, 802.11n), Wi-Fi 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + BLE
- **Security** 
  - Secure Boot, Secure Key storage and HW device identity with PUF, Secure Zone, Secure XIP (Execution in place) from flash, Secure Attestation, Anti Rollback, Secure Debug. Flash Protection. 
  - Secure firmware upgrade options:
    - Firmware loading support by Commander Tool through Jlink Debugger. Jlink connected to Serial Wire Debug (SWD) 
    - Firmware loading via ISP using UART (Commander or Serial terminal), SPI Interface
    - Secure Over the Air (OTA) Upgrade
- **Crypto Support**
  - Crypto API's for Hardware Accelerators:
    - Advanced Encryption Standard (AES) 128/256/192, Secure Hash Algorithm (SHA) 256/384/512, Hash Message Authentication Code (HMAC), Random Number Generator (RNG), SHA3, AES-Galois Counter Mode (GCM)/ Cipher based Message Authentication Code (CMAC), ChaCha-poly, True Random Number Generator (TRNG)
  - Software Accelerators: RSA, ECC 
  - Integrated with mbed TLS.
  - TRNG, AES, SHA, HMAC, GCM, Chacha-poly, ECDH and AES-CCM hardware crypto engine is integrated with PSA crypto.
- **System Power Save**
  - Deep Sleep with RAM retention and without RAM retention. 
  - Wireless Power Save: Connected Sleep (Wi-Fi Standby Associated), BLE Advertising with powersave, BLE Scan with powersave ,  BLE connection with powersave. Only Max PSP power save mode is supported in BLE. 

### **MCU**

- **Memory**
  - Common Flash: Single shared Flash for both Cortex-M4 and NWP (Wireless Processor)
- **Power States**
  - Active: PS4, PS3, PS2, and PS1 
  - Standby: PS4, PS3, and PS2
  - Sleep: PS4, PS3 and PS2
  - Deep Sleep(Shutdown): PS0
- **Peripherals**
  - High Performance (HP)- ADC, CALENDAR, CONFIG TIMER(CT), EFUSE, EGPIO, GSPI, I2C, I2S, MCPWM, SDIO Secondary, SIO, SSI, SYSRTC, UART, uDMA, USART, WDT** 
  - Ultra Low Power (ULP) - ULP\_TIMER, ULP\_UART, ULP\_I2C, ULP\_I2S, ULP\_uDMA and ULP\_SSI
- **Services**
  - Sleep Timer, IOSTREAM, Power Manager, SensorHub, NVM3
- **Development Pro-Kit Peripheral Support**
  - LED, Button, MEMLCD, Joystick, RHT Sensor

### **Development Environment**

- Simplicity Studio IDE (SV5.8.0 version) and Debugger Integration.   Refer to the latest version of the SoC "Getting-Started-with-SiWx917" guide for more details. 
- Simplicity Commander to supports Flash loading, provision of MBR programming, security key management, and calibration support for crystal and gain offsets. refer "siwx917-soc-manufacturing-utility-user-guide" for more details.** 
- Advanced energy measurement for ultra-low power capability available on Development boards (Pro Kit)

### **Wi-Fi**

- **Wi-Fi Protocols**
  - IEEE 802.11 b/g/n/ax (2.4GHz)
- **Access Point (AP) Mode**
  - 4 Client Support, Hidden SSID Mode, Auto Channel Selection, Scan in AP mode
  - Wi-Fi Security  
    - WPA2 Personal, WPA3 Personal (H2E method only), WPA Mixed mode (WPA/WPA2) 
- **Wi-Fi Scan**
  - Selective Scan, Active/Passive Scan
- **Wi-Fi STA (Security Modes)**
  - Open Mode, WPA2 Personal, WPA2 Enhancements, WPA3 Personal, Mixed Mode (WPA/WPA2), WPA3 Personal Transition Mode (WPA2/WPA3)
- **WPA2 Enterprise security (STA)**
  - Method
    - PEAP/TTLS/TLS 1.0/TLS 1.2/FAST
- **Wi-Fi STA Rejoin**
- **Wi-Fi STA Roaming**
  - BG Scan, OKC (Opportunistic Key caching), PMK (Pairwise Master Key) caching, Pre-Authentication
- **Wi-Fi Protocol Power Save** 
  - Deep sleep (unconnected state), Max PSP, Enhanced Max PSP, Fast PSP,** TWT
- **QoS**
  - WMM-QoS
- **Wi-Fi 6 Feature**
  - MUMIMO (DL), OFDMA (UL/DL), iTWT, TWT I-Frame & TWT Enhancements, BSS coloring, MBSSID
- **Wi-Fi Concurrency** 
  - AP+STA (Same channel)
- **Wi-Fi Band/Channels**
  - 2.4GHz CH1-11, 2.4GHz CH1-13, 2.4GHz CH1-14 (Japan)
- **Known Security Vulnerabilities Handled**
  - WPA2 KRACK Attacks, Fragment and Forge Vulnerability

### **Network Stack**

- **Core Networking Features**
  - TCP/IP Bypass (LWIP as Hosted stack for reference), IPv4/IPv6/UDP/TCP/ARP/ICMP/ICMPv6
  - SSL client versions TLSV1.0, TLSV1.2, TLSV1.3 
  - SSL server versions TLSV1.0 and TLSV1.2
  - DHCP (Client/Server)/ DHCPv6 Client
- **Advanced Network Features**
  - HTTP Client/HTTPS Client//DNS Client/SNTP Client, Embedded MQTT/MQTT on host
- **Wi-Fi IoT Cloud Integration**
  - AWS IOT Core
- BSD and IoT sockets application programming interface(API)

### **BLE** 

- GAP(Advertising, Scanning, initiation, Connection and Bonding)
- Generic Attribute Protocol(GATT)
- Attribute protocol(ATT)
- Security
- LL Privacy 1.2
- Accept list
- Directed Advertising
- Extended Advertising
- Periodic Advertising
- Periodic Advertising scanning
- Extended Advertising scanning
- Periodic Advertising list
- LE periodic advertising synchronization
- LE PHY(1Mbps, 2Mbps) & Coded PHY(125Kbps, 500kbps)
- Simultaneous scanning on 1Mbps and Coded PHY
- LE dual role topology
- LE data packet length extensions(DLE)
- Asymmetric PHYs
- LE channel selection algorithm 2 (CSA#2)
- LE Secure connections
- Bluetooth 5.4 Qualified

### **SDK**

- Simplified and Unified DX for Wi-Fi API and Platform APIs
- Simplifies application development and presents clean and standardized APIs
- UC (Universal Configurator) enables componentization, simplifying configuration of peripherals and examples
- BSD and ARM IoT-compliant socket API
- Available through Simplicity Studio and GitHub

### **Multi-protocol**

- Wi-Fi STA + BLE

### **PTA/Coexistence**

- 3 wire coex acting as Wi-Fi with external Bluetooth
- 3 wire coex acting as Wi-Fi with external Zigbee/OT

## **Changes in this release compared to v3.1.0 Release**

### **System**

- Moved MCU starting location to increase flash space for NWP. 

**NOTE:** Mandatory to upgrade the earlier version of boards (Si917-6031A Prokit or 4338A boards) or 917 Silicon ICs with instructions as outlined in this document "SiWG917–TA\_Flash\_Memory\_Map\_ChangeGuide\_v1.3.pdf" for more details.

### **MCU**

- **Enhancements**
  - Support for SensorHub Service
  - Support for Power Manager Service
  - Support for Joystick and RHT sensor examples
  - Support for software chip selection in SSI
  - Support for 10-bit addressing mode in I2C
  - Startup file and linker file enhancements. Realigned memory regions and updated IVT table.
  - Default interrupt priority configuration for all MCU interrupts when using RTOS
- **Fixed Issues**
  - Fixed implicit placement of uDMA section in SRAM used for placing uDMA descriptors
  - Fixed NVM3 corruption issue in multi-threaded environment

### **SDK**

- **Enhancements**
  - Folder restructuring, renamed SLC features and components as part of componentization. (Refer Migration Guide)
  - Standardized build parameters, enum, structures, typedef, and API signatures. (Refer Migration Guide)
  - Added support for TA(NWP) buffer configuration in sl\_wifi\_init.
  - Added support for sl\_wifi\_device\_context\_t in sl\_wifi\_init.
  - Replaced sl\_tls with sl\_net credential API.
  - Updated certificate handling APIs.
  - Updated and standardized README files for featured examples, MQTT, TWT, and more.
  - Generic API reference documentation updation.
  - Removed duplicate instances of EXECUTION\_FROM\_RAM in examples.
  - Added readme files for ble\_per and ble\_accept\_list.
  - Updated BLE API reference documentation with descriptions and navigation links.
  - Replaced printf statements with LOG\_PRINT in all Coex example applications.
  - Removal of wifi\_https\_ble\_dual\_role and wifi\_station\_ble\_throughput\_app examples application support.

### **Wi-Fi/Network Stack**

- **Enhancements**
  - Added support for WPA3 personal security (with H2E method only) in AP mode
  - Added support for timeout configuration API 
  - Added support for Server Name Indication (SNI).
  - Added support for Extensible Authentication Protocol (EAP) encrypted certificates.
- **Fixed Issues**
  - Resolved issues with CW modes in RF test example.
  - Fixed the issue where the application hangs during SHA operation on larger data exceeding 1400 bytes in TA (NWP).
  - Fixed the issue where the IOT socket connect/bind failed with -3 error in the CLI demo app.

### **BLE**

- **Enhancements**
  - Added support for BLE data transfer process when TA & M4 power save is enabled.
  - Added support for M4 sleep and wake-up for BLE alone applications.
  - Added support for standby sleep with RAM retention in Wi-Fi + BLE coex mode.
  - Removed support for unused rsi\_ble\_set\_prop\_protocol\_ble\_bandedge\_tx\_power(), rsi\_bt\_ber\_enable\_or\_disable(), and rsi\_bt\_per\_cw\_mode() APIs. (Refer Migration Guide)
  - Added support for rsi\_ble\_prepare\_write\_async, rsi\_ble\_execute\_write\_async, and rsi\_ble\_get\_inc\_services in gatt\_test application.
- **Fixed Issues**
  - DUT transmitting 240 bytes MTU request packet regardless of programmed MTU length.
  - DUT is unable to transmit complete periodic advertising data.
  - Hang issue when transmitting BLE notification with coded PHY rates of 125kbps and 500kbps.
  - DUT not transmitting BLE scan requests after 20sec when power save is enabled.
  - DUT not transmitting packets continuously over the air in BLE test modes.
  - BLE set BD address API failing with error code 0xfffffffd (Command given in the wrong state).
  - rsi\_ble\_set\_local\_att\_value() API fails with error code 0xFFFFFFE2 (RSI\_ERROR\_RESPONSE\_TIMEOUT) during BLE notification.
  - BLE per stats cmd API fails with error code 0xFFFFFFE2 (RSI\_ERROR\_RESPONSE\_TIMEOUT).
  - Synchronization issue in the host where the device was not able to transfer BLE notification packets when TA & M4 power save is enabled.
  - BLE scanning stops within ~4-5 minutes when power save is enabled.
  - Hang issues during multiple iterations of BLE connect and disconnect tests with TA and M4 power save enabled.

### **Multi-protocol**

- **Enhancements**
  - Increased throughputs in Wi-Fi station + BLE mode upto 30Mbps.
  - Added support for alarm-based power save mode in wlan\_station\_ble\_provisioning\_with\_aws cloud example application.
  - Increased priority of Wi-Fi handling of data transmission in coex mode.
- **Fixed Issues**
  - Abrupt termination of HTTP download in the application when power save is enabled.
  - Firmware update fails with 0x7 error code when power save is enabled in wifi\_throughput\_ble\_dual\_role application.
  - Handling of PLL value in coex mode.
  - BLE disconnection issue after ~25-30 min while running Wi-Fi HTTP/HTTPs download along with BLE data transfer with power save enabled.

## **Recommendations**

### **System**

- The current revision of SiWx917 has:
  - RAM memory of 672k bytes which can be shared between TA and M4 processors in SoC mode. 
  - The below configurations are applicable in SoC mode and can be configured based on the application requirement. EXT\_FEAT\_352K\_M4SS\_320K is the default configuration, based on requirement EXT\_FEAT\_480K\_M4SS\_192K configuration is selected for SoC mode multi-protocol examples.
    - EXT\_FEAT\_480K\_M4SS\_192K - This mode configures TA with 480k and M4 with 192K bytes of memory
    - EXT\_FEAT\_416K\_M4SS\_256K - This mode configures TA with 416k and M4 with 256K bytes of memory
    - EXT\_FEAT\_352K\_M4SS\_320K - This mode configures TA with 352k and M4 with 320K bytes of memory
  - SoC mode should not use 672k\_M4SS\_0K memory configuration.
- Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
- There are 2 Versions of Pro-Kits/Radio boards.  Si917-6031A based on Si917-4338A and SiWx917-6031A based on SiWx917-4338A. To get optimal power numbers, enable macro "SL\_SI91X\_ENABLE\_LOWPWR\_RET\_LDO" pre-processor define for ICs or while using SiWx917-6031A Pro-kit, SiWx917-4338A version of boards. This macro should be disabled for earlier variant of board (Si917-6031A, Si917-4338A).
- With RAM configuration (EXT\_FEAT\_352K\_M4SS\_320K), only 352K memory is available to TA  which limits the features supported, Recommended to enable EXT\_FEAT\_416K\_M4SS\_256K in Wi-Fi + BLE Multi protocol mode to enable more Network features.
- For EXT\_FEAT\_416K\_M4SS\_256K and EXT\_FEAT\_480K\_M4SS\_192K memory configurations, it is recommended to retain both TA and M4 RAMs in power save.

### **Wi-Fi/Network Stack**

- It is recommended to enable bit 16 of the 'Extended TCP IP Feature' bit map in the opermode command for all Wi-Fi Socket operations from the host to ensure graceful handling during asynchronous closures from the peer.
- For high throughput applications, aggregation (bit 2 of feature\_bit\_map) is recommended to be enabled in opermode. 
- To reset TA (NWP), the application needs to call sl\_wifi\_deinit() followed by sl\_wifi\_init(). Users must ensure that no execution happens from flash at this point of time. (Refer to example applications) 
- Users can enable SL\_SI91X\_EXT\_TCP\_IP\_SSL\_16K\_RECORD in 'Extended TCP IP Feature' bit map in opermode for (HTTPS server) supporting 16k record.
- **TWT**
  - Recommendation is to use sl\_wifi\_target\_wake\_time\_auto\_selection() API for all TWT applications. 
  - It is recommended to issue iTWT setup command once IP assignment, TCP connection, application specific socket connections are done.
  - When using sl\_wifi\_enable\_target\_wake\_time API, increase TCP / ARP Timeouts at the remote side depending upon the configured TWT interval configured. It's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() as an alternative.
  - In case of TWT in coex mode, when using sl\_wifi\_enable\_target\_wake\_time API, use TWT wake duration <= 16 ms and TWT wake interval >= 1 sec. If wake duration > 16 ms or TWT wake interval < 1sec, there might be performance issues.
  - For iTWT GTK interval in AP should be configured to max possible value or zero. If GTK interval is not configurable on AP side, recommended TWT interval (in case of sl\_wifi\_enable\_target\_wake\_time API) or RX Latency (in case of sl\_wifi\_target\_wake\_time\_auto\_selection API) is less than 4sec.
  - When sl\_wifi\_enable\_target\_wake\_time API is used, configuring TWT Wake interval beyond 1 min might lead to disconnections from the AP. Recommended to use TWT wake interval of less than or equal to 1 min.
  - When using sl\_wifi\_enable\_target\_wake\_time API, it is recommended to set missed\_beacon\_count of sl\_wifi\_set\_advanced\_client\_configuration API greater than 2 times of the configured TWT Interval.
- Disable power save for high throughput applications or use FAST PSP power save mode as per application requirement.
- The application needs to ensure that it sets RTC with the correct timestamp before establishing the SSL/EAP connection.
- The minimum timeout value should not be less than 1 second for socket select and socket receive calls. 
- Embedded MQTT keep alive interval should be either 0 or a value greater than 35 seconds.
- Disable power save and suspend any active TWT sessions before triggering HTTP OTAF.
- Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with the power save.

### **BLE**

- In BLE, the recommended range of Connection Interval in
  - Power Save (BLE Only) - 100 ms to 1.28 s.
- In BLE, during Connection, the configuration of Scan Interval and Scan Window with the same value is not recommended. The suggested ratio of Scan Window to Scan Interval is 3:4.
- In BLE, if a device is acting as Central, the scan window (in set\_scan\_params and create\_connection commands) must be less than the existing Connection Interval. The suggested ratio of Scan Window to Connection Interval is 2:3.
- In BLE mode, if scanning and advertising are in progress on the SiWx91x module and it subsequently gets connected and moves to the central role, scanning stops else if it moves to the peripheral role, advertising stops. To further establish a connection to another peripheral device or to a central device, the application should give a command for starting advertising and scanning again.

### **Multi-protocol**

- For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
- Wi-Fi + BLE Advertising
  - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals may be observed.
  - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity.
- Wi-Fi + BLE scanning
  - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
  - BLE scanning will be stopped for intervals that collide with Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Connections
  - All standard connection intervals are supported.
  - For a stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Data Transfer
  - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout.
  - Ensure Wi-Fi activity consumes lower intervals.

## **Known Issues of WiSeConnect3\_SDK\_3.1.1 Release**

### **MCU**

**SensorHub**

- SensorHub does not support, SPI-based sensors and ADC multi-channel. 

**SSI**

- SSI Dual and Quad mode are not functional. 
- SSI primary: mode3(TI) is not working as expected. 
- SSI Primary will not be operable at 40 MHz. 

**I2C**

- I2C High speed and Fastplus modes are not working at specification speed

**NVM3**

- When installing Sleeptimer and NVM3 in Studio, is not working as expected. As a workaround these components must be added to project SLCP

**UART**

- Flow control is not supported in USART and IOSTREAM applications

**ADC**

- In ADC Multichannel, sampling rate is limited to less than 15Ksps. Number of samples is less than 500 and with a maximum of 4 channel
- In ADC Multichannel, when using different sampling rates for different channels behavior is not as expected

**CT (Config Timer)**

- Config Timer(CT) support as an external event, in 32-bit mode, and DMA are not working as expected

**PWM**

- PWM Special event trigger for synchronizing analog-to-digital conversions is not working

**SYSRTC**

- SYSRTC with RO clock is not functioning

**Power Manager**

- The wakeup sources ULP-SSI, BOD, Comparator, and SysRTC are not working in the Power Manager service

### **SDK**

- Observed Wi-Fi connection is successful even after deleting the stored network credentials using sl\_net\_delete\_credential and responding with SL\_NET\_INVALID\_CREDENTIAL\_TYPE for sl\_net\_get\_credential.
- Observed sl\_net\_deinit is not returning any response for both "SL\_NET\_WIFI\_AP\_INTERFACE" and "SL\_NET\_WIFI\_CLIENT\_INTERFACE" interfaces, which requires to execute deinit code path to RAM in case of common flash.
- Observed issue when NULL is passed for readfds, writefds, excepfds to socket select. 
- Enhanced sl\_wifi\_get\_firmware\_version() API to provide more details (ROM ID, chip ID, security version, etc) which is not backward compatible with firmwares older than 1711.2.10.1.0.0.4. Firmware binary notation does not include security version number.

### **Wi-Fi/Network Stack**

**Wi-Fi STA**

- STA Connection with the WPA3 Hunting and Pecking algorithm takes about 3-4 seconds.
- Observed BSSID-based connection configuration is not reflecting to make a connection with specific access point.
- Observed connection failures with some AP's in more channel congestion ~50-60% occupancy open lab environment.
- Region selection based on country IE in the beacon is not supported
- Observed auth/assoc timeout is not reflecting as per timeout configuration using sl\_si91x\_configure\_timeout() API 
- Observed intermittent beacon reception from Access Point ( beacon misses) in an extremely congested environment

**Access Point (AP) Mode**

- Scan feature in AP mode not fully functional. 
- Fixed rate configuration in AP mode using sl\_wifi\_set\_transmit\_rate API is not being set as expected. 
- First few beacons (~ 10) don't include RSN IE

**WPA2 Enterprise security (STA)**

- Observed connection issue with configuring certificate key and programming 4096 bit key and SHA384/SHA512 certificates.
- Observing DUT is throwing 0x1001c when configuring .data.certificate\_key as "123456789".
- Configuration issue to program PAC file for EAP FAST Manual mode.
- Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.

**Wi-Fi Concurrency (AP + STA in same channel)**

- Observed 3rd party STA association fail with 917 AP while 917 STA mode is connecting/reconnecting to configured 3rd party AP. Reconnect 3rd party STA to 917 AP in such scenarios. 

**OFDMA (UL/DL)**

- Less throughput observed in DL-OFDMA with some APs that enabled LDPC.

**MUMIMO (DL)**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Performance, Interop issues with MU MIMO with certain APs. 
- Less throughput observed in MU-MIMO with some APs that enabled LDPC.

**TWT**

- Variability with MQTT keep alive interval based on TWT wake up interval.  
- When sl\_wifi\_enable\_target\_wake\_time() API is used, occasional MQTT disconnections may be observed if TWT is configured with longer TWT intervals (>30secs) with embedded MQTT + TWT.  As an alternative, it's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() API, where these dependencies are internally handled.

**Wi-Fi STA Rejoin**

- Observed Scanning (probe request) in all channels instead of the channels configured in selective channel(channel\_bitmap\_2g4) during rejoin process. 

**IPv4/IPv6**

- Observed issue with assigning static IP to DUT
- IP change notification is not indicated to the application. 
- Observed sl\_net\_up returning 0x27 error code due to an issue in the handling of IPv6 address. 

**BSD Socket API**

- Configuration issue to program MSS (default 1460), TCP retry count (default 10) through setsockopt
- Every server socket created consumes a socket (maximum of 10 sockets supported) and every subsequent connection to server socket consumes an additional socket (from the same pool of 10 sockets), which limits the number of server connections supported.

**SSL Client/Server**

- Sometimes during SSL Handshake, ECC curve parameters generated are wrong, resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
- Secure SSL renegotiation is not supported in the Embedded Networking Stack
- Observed connection failures with ECDSA ciphers.

**HTTP Client/ HTTPS Client**

- Observed occasional HTTPS continuous download failures when power save is enabled. Recommended to disable it before performing HTTPS continuous downloads
- Observed HTTPS post data is failing with error code: BBE2 (HTTP Failed).

**SNTP**

- Unable to get SNTP async events when Coex mode and power save are enabled 

**Throughputs & Performance**

- Wi-Fi alone throughput is about SDK 3.x (42Mbps). SDK refinements are in progress to further improve Wi-Fi Standalone and Coex Throughputs. 

**Secure Over the Air (OTA) Upgrade**

- Observed firmware upgrade failures after multiple iterations.
- Observed OTA failures with powersave enable, So recommended to disable powersave during OTA. 
- Observed "0xffffffff" error at the remote end while doing firmware upgrade via TCP server using (featured) example with ubuntu 21.0x. 

**Wi-Fi IOT Cloud integration (AWS IOT Core)**

- Observed AWS MQTT keepalive transmission is not happening at expected intervals with power save enabled.

**Wi-Fi Interoperability (IOP)**

- Observed disconnections with Amplifi (AFI-INS-R) AP with powersave enable
- TWT session is failing due to disconnections observed in DUT if rx\_latency is set to 55 seconds and receive data is also set to 55 seconds on MI Xiaomi RA72 and Tplink AX53 AP's
- Observed less throughput(~1Mb) while running TCP RX with Max\_PSP powersave with DLink 810 AP
- Observed interop issue (random disconnections) with few APs (EERO 6+, EERO PRO 6E, Cisco Catalyst 9120AXID)
- Disconnections observed with Netgear RAX120 AP in WPA3 Security

### **BLE**

**GATT Profile**

- HID profile data transfer is not working when power save is enabled in SoC. 

**GAP** 

- BLE reconnection is not working in ble\_multiconnection\_gatt demo when power save is enabled in SoC. 
- When power save is enabled in the SoC, the DUT hangs with the ble\_multiconnection\_gatt demo while performing continuous Tx/Rx notifications.
- When running the ble\_central application with power save enabled, it was observed that the BLE connect API is failing because of 0xFFFFFFE2(RSI\_ERROR\_RESPONSE\_TIMEOUT) error.
- When BLE local name is provided with a 16-byte length, the Set local name API fails with error code: 0x4e66[Invalid Name length].

**Advertising Extension**

- When running the ble\_ae\_central example, observed an application hang issue at the AE Clear Periodic Adv list API. 

**DTM/PER**

- Recommend to limit BLE Tx Maximum power to 16 dBm.  Please don't use for 127 power\_index for BLE HP release with this release.

**IOP**

- BLE connection failure has been observed on a few mobile devices (Samsung S23& Oneplus 9pro) when the Si917 device is configured as central.

### **Multi-protocol**

- For Coex Scenario Wi-Fi + BLE, BLE Data transfer, MU retries (~50-60%) observed while running DL MU-MIMO test. 
- Observed Wi-Fi + BLE intermittent connection failures, disconnections, and data transfer stalls in the long run when power save is enabled.
- Observed "DUT is not disconnecting to the AP when initiating disconnection from EFR connect app screen using wifi\_station\_ble\_provisioning\_aws example

### **System**

- Observed random hang issues with encrypted firmwares on some earlier variant of boards  (Si917-6031A, Si917-4338A) with powersave enable.

### **Simplicity Studio and Commander** 

- Simplicity commander does not support options under "Debug Lock tools".
- All projects in the package are compatible with **GNU ARM V12.2.1** toolchain

## **Planned Roadmap Features**

### **SDK**

- WPS 2.0 PIN/PUSH, Network Manager Enhancements
- MDNS, HTTP Server, Azure MQTT Client, Websockets
- Matter integration with Embedded TCP/IP stack 
- Enhanced buffer management, Throughput optimization
- TLS over LWIP, Network layer applications (MQTT, HTTP, etc.) over LWIP.
- Debugging utilities, UC Configurator for Wireless configurations

### **System**

- Secure communication over CPC (UART & SDIO interfaces)

### **MCU**

- Pin Tool for MCU 
- Dual Flash: Separate flash for Cortex M4 and Wireless Processor
- Peripherals: Analog Comparator, DAC, Temp Sensor, Capacitive Touch Sensor, OPAMP, HSPI, GPDMA, QEI, RNG, IR decoder, CRC and BOD
- Software-configurable MCU application memory options (192 or 256 or 320 kB)
- RTOS support for MCU Peripheral examples
- SensorHub: Support for SDC\_ADC and CLOUD connectivity
- SIO support for features Pattern matching, DMA flow control, self-loop mechanism and as a clock qualifier
- Support for I2S PCM functionality
- Stacked and external PSRAM: Pseudo Static RAM for storing application data and executing SoC applications. 

### **Wi-Fi/Network Stack**

- Spatial Re-Use, BSS Max Idle
- Provisioning using Wi-Fi AP

### **BLE**

- Support for 8 Peripheral + 2 Central connections 
- Support for BLE Mesh (4 nodes only) for limited Mesh use case

## **Limitations and Unsupported Features**  

### **System**

- None

### **SDK**

- Baremetal mode is not supported.

### **Wi-Fi/Network Stack**

- TLS 1.3 Server is not supported.
- 40 MHz bandwidth for 2.4 GHz band is not supported.
- Max 3 SSL sockets in Wi-Fi only mode, 2 SSL Socket in Wi-Fi + BLE based on RAM memory configuration selected. 
- In SSL ECC Curve ID supported is 23. SSL handshake with 3rd party clients depends on the SSL ECC Curve ID.
- The number of Non-Transmitting BSSIDs processed is limited by the beacon length that can be processed by the stack (which is 1024 bytes). Beacons greater than 1024 Bytes in length will not be processed.
- Multiprotocol (STA +BLE) + EAP Security modes supported only with Memory configurations EXT\_FEAT\_416K\_M4SS\_256K and EXT\_FEAT\_480K\_M4SS\_192K.
- UL MUMIMO is not supported.
- WPA3 AP supports only H2E algorithm.
- PMKSA caching is not supported in WPA3 AP mode.
- Maximum embedded MQTT Publish payload is 1 kbyte.
- Timeout value for socket select and socket receive calls of less than 1 second is not currently supported.

### **BLE**

- For BLE, if the connection is established with a small connection interval (less than 15 ms), simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported.
- BLE maximum two concurrent connections are supported, which can be either a connection to two peripheral devices, to one central and one peripheral device or two central devices.
- BLE Slave latency value is valid up to 32 only.
- Maximum supported AE data length is 200 bytes.
- Supports only two ADV\_EXT sets.
- Supports only two BLE connections (1 Central and 1 Peripheral) with AE.
- Advertising Extension feature is not supported in Coexistence.
- The  ae\_central & ae\_peripheral applications are not supported with TA\_352K\_M4\_320K RAM configuration.
- Two BLE connections are not supported with M4 powersave. It only supports a single connection. 
- Isochronous channels feature is not supported. 
- Connection subrating feature is not supported. 
- LE power controller feature is not supported.
- EATT feature is not supported.
- Periodic Advertising with a response feature is not supported. 
- BLE Audio is not supported.
- The feature of dynamically changing the TX power when extended advertising is active is not supported.

### **MCU**

- SensorHub supports PS1-power state with ADC sensor. In this mode, other sensors operation is not supported. 

### **Multi-protocol**

- Wi-Fi AP + BLE currently not supported. 

<br>

# WiSeConnect3\_SDK\_3.1.1 SoC Release Notes
## Release Details

|<h3>**Item**</h3>|<h3>**Details**</h3>|
| :- | :- |
|Release date|Dec 13th, 2023|
|SDK Version|3\.1.1|
|Firmware Version|1711\.2.10.0.0.4|
|Package Name|WiSeConnect3\_SDK\_3.1.1|
|Supported RTOS|FreeRTOS|
|Operating Modes Supported|Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP|
|Build Quality|GA|

- SiWx917 release consists of two components
  - Wireless Firmware - SiWx917 Firmware Binaries
  - Wiseconnect3 Library - Wiseconnect3 SDK library runs on internal Cortex M4
- This release is meant only for use with designs based on SiWx917 Silicon
- The firmware to load in your design are available in the following format:
  - SiWG917-B.2.10.0.0.4.x.rps -  Sutlej B0 Full-featured firmware binary
## Supported Setups
- BRD4002A Wireless pro kit mainboard SI-MB4002A
- SiWx917 Radio boards

|**Product Mode**|**Radio Board**|
| :- | :- |
|Common Flash|SiWx917-RB4338A|
## Features Supported
### **System**
- #### **Operating Modes**
  - Wi-Fi STA (802.11ax, 802.11n)
  - Wi-Fi 802.11n AP
  - Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP
  - Wi-Fi STA (802.11ax, 802.11n) + BLE
- #### **Security** 
  - Secure Boot
  - Secure firmware upgrade over
    - ISP using UART, SPI interfaces
    - Secure OTA 
  - Secure Key storage and HW device identity with PUF
  - Secure Zone 
  - Secure XIP (Execution in place) from flash
  - Secure Attestation
  - Anti Rollback
  - Secure Debug
  - Flash Protection
- #### **Crypto Support**
  - Crypto API's for Hardware Accelerators:
    - Advanced Encryption Standard (AES) 128/256/192
    - Secure Hash Algorithm (SHA) 256/384/512
    - Hash Message Authentication Code (HMAC)
    - Random Number Generator (RNG)
    - SHA3
    - AES-Galois Counter Mode (GCM)/ Cipher based Message Authentication Code (CMAC)
    - ChaCha-poly
    - True Random Number Generator (TRNG)
  - Software Accelerators: RSA, ECC 
  - Integrated with mbed TLS 
  - TRNG, AES, SHA, HMAC, GCM, Chacha-poly, ECDH and AES-CCM hardware crypto engine is integrated with PSA crypto.
- #### **System Power save**
  - Deep Sleep 
    - With RAM retention
    - Without RAM retention
  - Connected Sleep
### **MCU**
- #### **Memory**
  - PSRAM (Alpha)
- #### **Peripherals**
  - **HP** - ADC, CALENDAR, CONFIG TIMER(CT), EFUSE, EGPIO, GSPI, I2C, I2S, MCPWM, SDIO Secondary, SIO, SSI, SYSRTC, UART, uDMA, USART, WDT
  - **ULP** - ULP\_TIMER, ULP\_UART, ULP\_I2C, ULP\_I2S, ULP\_uDMA and ULP\_SSI
- #### **Service**
  - Sleep Timer
  - IOSTREAM
  - Power Manager
  - Sensor Hub
  - NVM3
- #### **SiWx917-PK6030A Pro-Kit Peripheral Support**
  - LED
  - Button
  - MEMLCD
  - Joystick
  - RHT Sensor
### **Wi-Fi**
- #### **Wi-Fi protocols**
  - IEEE 802.11 b/g/n/ax (2.4GHz)
- #### **Access Point (AP) mode**
  - 4 Client Support 
  - Wi-Fi Security  
    - WPA2 Personal
    - WPA3 Personal (H2E method only)
    - WPA Mixed mode (WPA/WPA2) 
  - Hidden SSID Mode
  - Auto Channel Selection
  - Scan in AP mode
- #### **Wi-Fi Scan**
  - Selective Scan
  - Active/Passive Scan
- #### **Wi-Fi STA (Security Modes)**
  - Open Mode
  - WPA2 Personal, WPA2 Enhancements
  - WPA3 Personal 
  - Mixed Mode (WPA/WPA2)
  - WPA3 Personal Transition Mode (WPA2/WPA3)
- #### **WPA2 Enterprise security (STA)**
  - Method
    - PEAP/TTLS/TLS 1.0/TLS 1.2/FAST
- #### **Wi-Fi STA Rejoin**
- #### **Wi-Fi STA Roaming**
  - BG Scan
  - OKC (Opportunistic Key caching)
  - PMK (Pairwise Master Key) caching
  - Pre-Authentication
- #### **Wi-Fi Protocol Power save** 
  - Deep sleep (unconnected state)
  - Max PSP
  - Enhanced Max PSP
  - Fast PSP
  - TWT
- #### **QoS**
  - WMM-QoS
- #### **Wi-Fi 6 Feature**
  - MUMIMO (DL)
  - OFDMA (UL/DL)
  - iTWT, TWT I-Frame & TWT Enhancements
  - BSS coloring
  - MBSSID
- #### **Wi-Fi Concurrency** 
  - AP+STA (Same channel)
- #### **Wi-Fi Band/Channels**
  - 2.4GHz CH1-11
  - 2.4GHz CH1-13
  - 2.4GHz CH1-14 (Japan)
- #### **Known Security vulnerabilities handled**
  - WPA2 KRACK Attacks
  - Fragment and Forge Vulnerability
### **Networking stack**
- #### **Core Networking Features**
  - TCP/IP Bypass (LWIP as Hosted stack for reference)
  - IPv4/IPv6/UDP/TCP/ARP/ICMP/ICMPv6
  - SSL client versions TLSV1.0, TLSV1.2, TLSV1.3 
  - SSL server versions TLSV1.0 and TLSV1.2
  - DHCP (Client/Server)/ DHCPv6 Client
- #### **Advanced Network Features**
  - HTTP Client/HTTPS Client//DNS Client/SNTP Client
  - Embedded MQTT/MQTT on host
- #### **Wi-Fi IoT Cloud Integration**
  - AWS IOT Core
- BSD and IoT sockets application programming interface(API)
### **BLE**
- GAP(Advertising, Scanning, initiation, Connection and Bonding)
- Generic Attribute Protocol(GATT)
- Attribute protocol(ATT)
- Security
- LL Privacy 1.2
- Accept list
- Directed Advertising
- Extended Advertising
- Periodic Advertising
- Periodic Advertising scanning
- Extended Advertising scanning
- Periodic Advertising list
- LE periodic advertising synchronization
- LE PHY(1Mbps, 2Mbps) & Coded PHY(125Kbps, 500kbps)
- Simultaneous scanning on 1Mbps and Coded PHY
- LE dual role topology
- LE data packet length extensions(DLE)
- Asymmetric PHYs
- LE channel selection algorithm 2 (CSA#2)
- LE Secure connections
- Bluetooth 5.4 Qualified(Clerical work pending)
### **Multi-protocol Mode**
- Wi-Fi STA + BLE
### **PTA/Coexistence**
- 3 wire coex acting as Wi-Fi with external Bluetooth
- 3 wire coex acting as Wi-Fi with external Zigbee/OT
## Changes and Fixes
### **System**
- Support for new flash map changes with 'NWP-1.8MB + MCU-2MB'
### **MCU**
- #### **Enhancements**
  - Support for SensorHub Service
  - Support for Power Manager Service
  - Support for Joystick and RHT sensor examples
  - Support for External PSRAM - Alpha\*
  - Support for software chip selection in SSI
  - Support for 10-bit addressing mode in I2C
  - Support for interrupt mode and PS1 for ADC sensor
  - Support for PSRAM Half-Sleep
  - Support for GUVA\_12D\_UV(ADC-based) sensor in sensor hub framework
  - Support for ADC Multichannel
  - Startup file and linker file enhancements
  - Default interrupt priority configuration for all MCU interrupts when using RTOS
- #### **Fixed Issues**
  - Fixed implicit placement of uDMA section in SRAM used for placing uDMA descriptors
  - Fixed NVM3 corruption issue
### **Wi-Fi**
- #### **Enhancements**
  - Folder restructuring, renamed SLC features and components as part of componentization. (Refer Migration Guide)
  - Standardized build parameters, enum, structures, typedef, and API signatures. (Refer Migration Guide)
  - Added support for TA(NWP) buffer configuration in sl\_wifi\_init.
  - Added support for sl\_wifi\_device\_context\_t in sl\_wifi\_init.
  - Added support for WPA3 personal security (with H2E method only) in AP mode
  - Added support for timeout configuration API 
  - Added support for Server Name Indication (SNI).
  - Added support for Extensible Authentication Protocol (EAP) encrypted certificates.
  - Replaced sl\_tls with sl\_net credential API.
  - Updated certificate handling APIs.
  - Removed duplicate instances of EXECUTION\_FROM\_RAM in examples.
  - Updated and standardized README files for featured examples, MQTT, TWT, and more.
  - Generic API reference documentation updation.
- #### **Fixed Issues**
  - Resolved issues with CW modes in RF test example.
  - Fixed the issue where the application hangs issue during SHA operation on larger data exceeding 1400 bytes in TA (NWP).
  - Fixed the issue where DUT sent non-aggregated packets with protected ADDBA response for improving throughput.
  - Fixed the issue where the IOT socket connect/bind failed with -3 error in the CLI demo app.
  - "Failed to keep module in active mode: FFFFFFFF" issue when disabling power save after enabling enhanced max psp power save.
### **BLE**
- #### **Enhancements**
  - Readme files for ble\_per and ble\_accept\_list.
  - BLE API reference documentation with descriptions and navigation links.
  - Added support for BLE data transfer process when TA & M4 power save is enabled.
  - Added support for M4 sleep and wake-up for BLE alone applications.
  - Added support for standby sleep with RAM retention in Wi-Fi + BLE coex mode.
  - Removed support for unused rsi\_ble\_set\_prop\_protocol\_ble\_bandedge\_tx\_power(), rsi\_bt\_ber\_enable\_or\_disable(), and rsi\_bt\_per\_cw\_mode() APIs. (Refer Migration Guide)
  - Added support for rsi\_ble\_prepare\_write\_async, rsi\_ble\_execute\_write\_async, and rsi\_ble\_get\_inc\_services in gatt\_test application.
- #### **Fixed issues**
  - DUT transmitting 240 bytes MTU request packet regardless of programmed MTU length.
  - DUT is unable to transmit complete periodic advertising data.
  - Hang issue when transmitting BLE notification with coded PHY rates of 125kbps and 500kbps.
  - DUT not transmitting BLE scan requests after 20sec when power save is enabled.
  - DUT not transmitting packets continuously over the air in BLE test modes.
  - BLE set BD address API failing with error code 0xfffffffd (Command given in the wrong state).
  - rsi\_ble\_set\_local\_att\_value() API fails with error code 0xFFFFFFE2 (RSI\_ERROR\_RESPONSE\_TIMEOUT) during BLE notification.
  - BLE per stats cmd API fails with error code 0xFFFFFFE2 (RSI\_ERROR\_RESPONSE\_TIMEOUT).
  - Synchronization issue in the host where the device was not able to transfer BLE notification packets when TA & M4 power save is enabled.
  - BLE scanning stops within ~4-5 minutes when power save is enabled.
  - Hang issues during multiple iterations of BLE connect and disconnect tests with TA and M4 power save enabled.
### **Multi-protocol**
- #### **Enhancements**
  - Increased throughputs in Wi-Fi station + BLE mode upto 30Mbps.
  - Added support for alarm-based power save mode in wlan\_station\_ble\_provisioning\_with\_aws cloud example application.
  - Increased priority of Wi-Fi handling of data transmission in coex mode.
  - Replaced Printf statements with LOG\_PRINT in all Coex example applications.
- #### **Fixed issues**
  - Abrupt termination of HTTP download in the application when power save is enabled.
  - Firmware update fails with 0x7 error code when power save is enabled in wifi\_throughput\_ble\_dual\_role application.
  - Handling of PLL value in coex mode.
  - BLE disconnection issue after ~25-30 min while running Wi-Fi HTTP/HTTPs download along with BLE data transfer with power save enabled.
## **Recommendations**
### **System**
- The current revision of SiWx917 B0 has:
  - RAM memory of 672k bytes which can be shared between TA and M4 processors in SoC mode. 
  - The below configurations are applicable in SoC mode and can be configured based on the application requirement By default EXT\_FEAT\_480K\_M4SS\_192K configuration will be selected in SoC mode

      - EXT\_FEAT\_480K\_M4SS\_192K - This mode configures TA with 480k and M4 with 192K bytes of memory
      - EXT\_FEAT\_416K\_M4SS\_256K - This mode configures TA with 416k and M4 with 256K bytes of memory
      - EXT\_FEAT\_352K\_M4SS\_320K - This mode configures TA with 352k and M4 with 320K bytes of memory
  - SoC mode should not use 672k\_M4SS\_0K memory configuration.
- Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
- To get optimal power numbers for launch parts, enable SL\_SI91X\_ENABLE\_LOWPWR\_RET\_LDO pre-processor define. 
- With RAM configuration (EXT\_FEAT\_352K\_M4SS\_320K), only 352K memory is available to TA to support Wi-Fi + BLE with SSL, Recommended to enable EXT\_FEAT\_416K\_M4SS\_256K in Wi-Fi + BLE Multi protocol mode to enable more Network features.
### **IDE**
- Simplicity Studio. This release is tested with SV5.8.0 version.
- Refer to the latest version of the SoC "Getting-Started-with-SiWx917" guide for more details
### **Wi-Fi/Network Stack**
- It is recommended to enable bit 16 of the 'Extended TCP IP Feature' bit map in the opermode command for all Wi-Fi Socket operations from the host to ensure graceful handling during asynchronous closures from the peer.
- Aggregation (bit 2 of feature\_bit\_map) is recommended to be enabled in opermode. This enables software to aggregate multiple packets to achieve better throughput.
- TCP retransmission count value is recommended to be 30 or higher so that the TCP does not disconnect while the rejoin happens.
- To reset TA (NWP), the application needs to call sl\_wifi\_deinit() followed by sl\_wifi\_init(). Users must ensure that no execution happens from flash at this point of time. (Refer to example applications) 
- Users can enable SL\_SI91X\_EXT\_TCP\_IP\_SSL\_16K\_RECORD in 'Extended TCP IP Feature' bit map in opermode for (HTTPS server) supporting 16k record.
- #### **TWT**
  - It is recommended to issue iTWT setup command once IP assignment, TCP connection, application specific socket connections are done.
  - When using sl\_wifi\_enable\_target\_wake\_time API, increase TCP / ARP Timeouts at the remote side depending upon the configured TWT interval configured. It's highly recommended to use sl\_wifi\_target\_wake\_time\_auto\_selection() as an alternative.
  - In case of TWT in coex mode, when using sl\_wifi\_enable\_target\_wake\_time API, use TWT wake duration <= 16 ms and TWT wake interval >= 1 sec. If wake duration > 16 ms or TWT wake interval < 1sec, there might be performance issues.
  - For iTWT GTK interval in AP should be configured to max possible value or zero. If GTK interval is not configurable on AP side, recommended TWT interval (in case of sl\_wifi\_enable\_target\_wake\_time API) or RX Latency (in case of sl\_wifi\_target\_wake\_time\_auto\_selection API) is less than 4sec.
  - When sl\_wifi\_enable\_target\_wake\_time API is used, configuring TWT Wake interval beyond 1 min might lead to disconnections from the AP. Recommended to use TWT wake interval of less than or equal to 1 min.
  - When using sl\_wifi\_enable\_target\_wake\_time API, it is recommended to set missed\_beacon\_count of sl\_wifi\_set\_advanced\_client\_configuration API greater than 2 times of the configured TWT Interval.
- Disable power save for high throughput applications or use FAST PSP powersave mode as per application requirement.
- The application needs to ensure that it sets RTC with the correct timestamp when the feature is enabled before establishing the SSL/EAP connection.
- The minimum timeout value should not be less than 1 second for socket select and socket receive calls. 
- Embedded MQTT keep alive interval should be either 0 or a value greater than 35 seconds.
- Disable power save and suspend any active TWT sessions before triggering HTTP OTAF.
- Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with the power save.
### **BLE**
- In BLE, the recommended range of Connection Interval in
  - Power Save (BLE Only) - 100 ms to 1.28 s.
- In BLE, during Connection, the configuration of Scan Interval and Scan Window with the same value is not recommended. The suggested ratio of Scan Window to Scan Interval is 3:4.
- In BLE, if a device is acting as Central, the scan window (in set\_scan\_params and create\_connection commands) must be less than the existing Connection Interval. The suggested ratio of Scan Window to Connection Interval is 2:3.
- In BLE mode, if scanning and advertising are in progress on the SiWx91x module and it subsequently gets connected and moves to the central role, scanning stops else if it moves to the peripheral role, advertising stops. To further establish a connection to another peripheral device or to a central device, the application should give a command for starting advertising and scanning again.
### **Multi-protocol**
- For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
- Wi-Fi + BLE Advertising
  - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals may be observed.
  - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity.
- Wi-Fi + BLE scanning
  - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
  - BLE scanning will be stopped for intervals that collide with Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Connections
  - All standard connection intervals are supported.
  - For a stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Data Transfer
  - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout.
  - Ensure Wi-Fi activity consumes lower intervals.
## Known Issues
### **Wi-Fi/Network Stack**
- Sometimes during SSL Handshake, ECC curve parameters generated are wrong, resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
- Secure SSL renegotiation not supported in Embedded Networking Stack.
- Every server socket created consumes a socket and every subsequent connection consumes an additional socket.
- Observed failure with BIT\_TLS\_ECDHE\_ECDSA\_WITH\_AES\_256\_CBC\_SHA384, BIT\_TLS\_ECDHE\_ECDSA\_WITH\_AES\_128\_CBC\_SHA256 ciphers
- Configuration issue to program MSS, TCP retry count 
- Observed inconsistency with MQTT keep alive when TWT is enabled
- TWT device (STA) may not go to sleep in between TWT intervals in case of burst data traffic where data transfer happens over multiple TWT service periods.
- When sl\_wifi\_enable\_target\_wake\_time() API is used, observed occasional MQTT disconnections with longer TWT intervals (>30secs) with embedded MQTT + TWT.
- There are performance and IOP issues with MU MIMO.
- STA Connection with the WPA3 Hunting and Pecking algorithm takes about 3-4 seconds.
- Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.
- Observed HTTPS continuous download issues with TA Power save
- Planned performance improvements are in progress to get optimal performance in Wi-Fi/Wi-Fi+BLE 
- Observed interop issue (connectivity failure) with few AP's(EERO 6+, EERO PRO 6E, Cisco Catalyst 9120AXID) when device reset/reboot happens on active connection. 
- Unable to get SNTP async events when coex mode and power save are enabled and not getting any response for sl\_net\_up API for 2nd Iteration
- Disconnections observed with Netgear RAX120 AP in WPA3 security
- Observed connection failures with some AP's in more channel congestion ~50-60% occupancy open lab environment. 
- In AP mode, the first few beacons don't include RSN IE.
- Issue with fixed rate configuration in AP mode
- Configuration issue to program PAC file for EAP FAST Manual mode
- Optional LDPC feature is not enabled, So observed less throughput in DL-OFDMA with some APs.
- Observed issue with configuring certificate key and programming 4096 bit key and SHA384/SHA512 certificates.
- Observed DUT is unable to receive data when using wifi6\_twt\_use\_case\_demo\_soc application with encrypted rps.
- Observed Wi-Fi connection is successful even after deleting the stored network credentials using sl\_net\_delete\_credential and responding with SL\_NET\_INVALID\_CREDENTIAL\_TYPE for sl\_net\_get\_credential.
- Observed HTTPS post data is failing with error code: BBE2 (HTTP Failed). 
- Observed BSSID-based connection is not working.
- Observed DUT is not resuming the data transfer after successful rejoin when DUT is connected to AWS cloud and continuously publishing data with Qos1 priority.
- Observed "Failed to bring Wi-Fi client interface up: 0x100025(SL\_STATUS\_SI91X\_COMMAND\_ISSUED\_IN\_REJOIN\_STATE)" while running HTTP OTAF in the long run.
- Observed Wi-Fi alone throughput with SDK 3.x(42Mbps) compared with the expected target (50Mbps).
- Observed auth/assoc timeout is not reflecting as per timeout configuration.
- Observed sl\_net\_deinit is not returning any response for both "SL\_NET\_WIFI\_AP\_INTERFACE" and "SL\_NET\_WIFI\_CLIENT\_INTERFACE" interfaces. 
- Observed AWS MQTT keepalive transmission is not happening at expected intervals with power save enabled.
- WoWLAN feature not supported.
- Removal of wifi\_https\_ble\_dual\_role and wifi\_station\_ble\_throughput\_app examples application support.
### **BLE**
- HID profile data transfer is not working when power save is enabled in SoC.
- BLE reconnection is not working in ble\_multiconnection\_gatt demo when power save is enabled in SoC. 
- When power save is enabled in the SoC, the DUT hangs with the ble\_multiconnection\_gatt demo while performing continuous Tx/Rx notifications.
- When running the ble\_central application with power save enabled, it was observed that the BLE connect API is failing because of 0xFFFFFFE2(RSI\_ERROR\_RESPONSE\_TIMEOUT) error.
- When running the ble\_ae\_central example, observed an application hang issue at the AE Clear Periodic Adv list API. 
- BLE connection failure has been observed on a few mobile devices (Samsung S23& Oneplus 9pro) when the device is configured as central.
- The maximum power that BLE can support is 16 dBm. It is not permitted for users to program more than 16dBm. 127 power_index is not supported in this release.
### **Multi-protocol**
- MU retries in DL MUMIMO are more in CoEx.
- Observed TCP\_RX is failing while running WLAN continuous throughput and BLE adv/scan or connection activities in power save mode.
- Observed TCP socket creation is failing while running TCP Tx data by disabling the WLAN\_SYNC\_REQ in power save mode.
- Observed data stalls in the remote machine during continuous TCP TX along with BLE TX in SMP in the long run.
- Observed DUT hang while doing periodic(every 30s) M4 wakeup, SSL Tx, BLE Tx, and M4 sleep activity in the long run.
- Observed SSL connection failure while running wifi\_ble\_power\_save application. 
- Observed "DUT is not disconnecting to the AP when initiating disconnection from EFR connect app screen using wifi\_station\_ble\_provisioning\_aws example
- BLE disconnected with reason 'instant passed'/'Connection timeout' while BLE scan and BLE Tx are executed where Wi-Fi is in idle connection.
### **System**
- None
### **MCU**
- SensorHub does not support SPI-based sensors.
- SensorHub does not support ADC multi-channel.
- SSI Dual and Quad mode are not functional.
- I2C High speed and Fastplus mode not working at specifications speed.
- ULP SSI wakeup source is not working.
- The wakeup sources BOD, Comparator, and SysRTC are not working in the Power manager service.
- To use Sleeptimer and NVM3, these components must be added to project slcp before the project is launched in studio.
- SSI Primary will not be operable till 40 MHz.
- Flow control is not supported in USART and IOSTREAM applications.
- In SSI primary mode3(TI) is not working.
## Limitations and Unsupported Features
### **System**
- None
### **Wi-Fi/Network Stack**
- TLS 1.3 Server is not supported.
- Broadcast TWT is not supported.
- AMSDU TX is not supported.
- Fragmentation is not supported.
- AMSDUs within AMPDU is not supported.
- Currently, the module does not support Radio Measurement Requests feature of CCX V2
- 802.11k is not supported
- 40 MHz bandwidth for 2.4 GHz band is not supported.
- Max 3 SSL sockets in Wi-Fi only mode, 2 SSL Socket in Wi-Fi + BLE based on RAM memory configuration selected
- In SSL ECC Curve ID supported is 23. SSL handshake with 3rd party clients depends on the SSL ECC Curve ID.
- Legacy TA(NWP) LP mode is not supported.
- In iTWT mode, downlink data traffic should be limited. Disconnections may be observed if AP fails to send all the buffered data in the next TWT Service Period.
- The number of Non-Transmitting BSSIDs processed is limited by the beacon length that can be processed by the stack (which is 1024 bytes). Beacons greater than 1024 Bytes in length will not be processed.
- CoEx + EAP Security in 352K/320k is not feasible. This issue is observed due to a lack of memory in the 352k/320k Memory Configuration. It is supported with other EXT\_FEAT\_416K\_M4SS\_256K and EXT\_FEAT\_480K\_M4SS\_192K memory configurations.
- UL MUMIMO is not supported.
- The embedded HTTP server is not supported.
- SDK does not support baremetal mode.
- WPA3 AP supports only H2E algorithm.
- PMKSA caching is not supported in WPA3 AP mode.
- Wi-Fi Mesh not supported
- Apple HomeKit not supported
- Cellular coexistence not supported
- The maximum embedded MQTT Publish payload is 1 kbyte.
- Timeout value for socket select and socket receive calls of less than 1 second is not currently supported.
### **BLE**
- For BLE, if the connection is established with a small connection interval (less than 15 ms), simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported.
- BLE maximum two concurrent connections are supported, which can be either a connection to two peripheral devices, to one central and one peripheral device or two central devices.
- BLE Slave latency value is valid up to 32 only.
- BLE TX/RX throughput is less when tested with EFM as compared to EFR
- Maximum supported AE data length is 200 bytes.
- Supports only two ADV\_EXT sets.
- Supports only two BLE connections (1 Central and 1 Peripheral) with AE.
- Advertising Extension feature is not supported in Coexistence.
- The  ae\_central & ae\_peripheral applications are not supported with TA\_352K\_M4\_320K RAM configuration.
- Two BLE connections are not supported with M4 powersave. It only supports a single connection. 
- Isochronous channels feature is not supported. 
- Connection subrating feature is not supported. 
- LE power controller feature is not supported.
- EATT feature is not supported.
- Periodic Advertising with a response feature is not supported. 
- BLE Audio is not supported.
- The feature of dynamically changing the TX power when extended advertising is active is not supported.
### **MCU**
- On BRD4338a board, two I2C instances(I2C1 and I2C2) are using the same GPIOs, so support is not there for using two I2C instances simultaneously.
- SensorHub supports PS1-power state with ADC sensor. In this mode, other sensors operation is not supported. 
- PinTool is not supported.
- RTOS is not enabled for MCU peripheral examples.
## **Simplicity Studio and Commander**
- Simplicity commander does not support options under "Debug Lock tools".
- All projects in the package are compatible with GCC 12.2.1 version.


<br>

# WiSeConnect3\_SDK\_3.1.0 Release Notes
This SDK package supports SoC Mode and NCP Mode for A0 and B0:

- In SoC Mode: Customer applications can run on Cortex M4. Wireless, network, and security stacks run on the wireless subsystem.
- In NCP mode: Wireless, network, and security stacks run on the Wireless subsystem. Customer applications run on the external host MCU.
- Silicon Labs' SiWx917 includes an ultra-low power Wi-Fi 6 plus Bluetooth Low Energy (BLE) 5.1 wireless subsystem and an integrated micro-controller application subsystem.

**Note**: Use Simplicity Studio version SV5.7.0.1 onwards
## **SoC Highlights**
- **Operating Modes** : Wi-Fi STA (802.11ax, 802.11n), Wi-Fi 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + BLE
- **Wireless Protocols** : IEEE 802.11b, 802.11g, 802.11n, 802.11ax, BLE 5.1
- **Multiple Ultra Low Power Modes** : Support ultra low power Wi-Fi standby associated current in Wi-Fi 4 mode, Ultra low power Wi-Fi Standby associated current in Wi-Fi 6 mode leveraging TWT, Always on Cloud connected power, BLE connected power, and, deep sleep ULP mode
- **MCU examples** : GPIO, I2C, Calendar,  SPI, SSI, SIO, Micro DMA, UART/USART, Watchdog Timer, Timers, Config Timers, ULP (Ultra Low Power) Timers, Sleeptimer, IOSTREAM, NVM3, MEMLCD, SDIO Secondary, PWM, ADC, EFUSE, I2S, PSRAM, ML Blinky, Sensorhub
- **Wi-Fi** : 2.4GHz Wi-Fi Support - WPA2/WPA3 Personal, Wi-Fi 6 OFDMA/MU-MIMO Support that enhances network capacity and reduces latency
- **Cloud** : Amazon Cloud Connectivity Support, Amazon FreeRTOS
- **BLE** : BLE 5.1, BLE dual role (Central and Peripheral Support), Advertising extensions, Data length extensions, LL privacy, BLE whitelisting, and BLE long Range. 
- **Integrated Stacks and Profiles** : Wi-Fi Stack, BLE Stack, All GAP and GATT based Profiles & Services are Supported, TCP/IP stack with TLS 1.3, HTTP/HTTPS, DHCP, MQTT Client, DNS client
- **IDE** : Simplicity Studio with Windows 10(64-bit), Linux, Mac OS
- **TA/M4 Firmware Update Tool** : Simplicity Commander
- **Hardware kits to evaluate SoC Mode** :
  - Si-MB4002A Main board
  - SiWx917 Radio boards

|**RadioBoard Type**|**SiWx917A0 with Board Version**|**SiWx917B0 with Board Version**|
| :-: | :-: | :-: |
|Common flash|Si917-BRD4325A(1.0 /1.1 /1.2)|-|
|Common flash|-|Si917-BRD4338A(2.0)|
|Dual flash|Si917-BRD4325B(1.0 /1.1 /1.2)|-|
|External PSRAM|-|Si917-BRD4340A(2.0)|

## **NCP Highlights**
- **Host Interface** : SPI
- **Operating Modes** : Wi-Fi STA (802.11ax, 802.11n), Wi-Fi 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + BLE
- **Wireless Protocols** : IEEE 802.11b, 802.11g, 802.11n, 802.11ax, BLE 5.1
- **Multiple Ultra Low Power Modes** : Support Ultra low power Wi-Fi Standby associated current in Wi-Fi 4 mode, Ultra low power Wi-Fi Standby associated current in Wi-Fi 6 mode leveraging TWT, Always on Cloud connected power, BLE connected power, and, deep sleep ULP mode.
- **Wi-Fi** : 2.4GHz Wi-Fi Support, WPA2/WPA3 Personal, Wi-Fi 6 OFDMA/MU-MIMO Support that enhances network capacity and reduces latency
- **BLE** : BLE 5.1, BLE dual role (Central and Peripheral Support), Advertising extensions, Data length extensions, LL privacy, BLE Whitelisting and BLE long Range. 
- **Integrated Stacks and Profiles** : Wi-Fi Stack , BLE Stack , All GAP and GATT based Profiles & Services are Supported, TCP/IP stack with TLS 1.3, HTTP/HTTPS, DHCP, MQTT Client, DNS Client.
- **IDE** : Simplicity Studio with Windows 10(64-bit)
- **Hardware kits to evaluate NCP Mode**
  - Si917 B0 1.2 expansion board BRD8036B 
  - Wireless Pro kit Mainboard BRD4002A Rev A06
  - EFR32xG21 2.4 GHz 20 dBm Radio Board (BRD4180B Rev A02)
## **Release Details**

|**Item**|**Details**|
| :-: | :-: |
|Release date|Oct 9th, 2023|
|SDK Version|3\.1.0|
|Firmware Version|1711\.2.9.0.0.21|
|Package Name|WiSeConnect3\_SDK\_3.1.0|
|Supported RTOS|FreeRTOS|
|Operating Modes Supported|Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP|
|Build Quality|Beta|

- SiWx917 release consists of two components
  - Wireless Firmware - SiWx917 Firmware Binaries
  - Wiseconnect3 Library - Wiseconnect3 SDK library runs on the external host in NCP mode and internal Cortex M4 in SoC Mode.
- This release is meant only for use with designs based on SiWx917 Silicon
- The firmware to load in your design are available in the following format:
  - SiWG917-A.2.9.0.0.x.rps - A0 Full-featured firmware binary
  - SiWG917-B.2.9.0.0.x.rps - B0 Full-featured firmware binary
## **Features Supported**
**MCU**

- ADC Single Channel, ADC Static Mode, Analog Comparator, Blinky, Bod, CTS, DAC FIFO, DAC Static, GPIO, I2C, I2S, NVM3, OPAMP, PSRAM, PWM, QEI, RTC, SDIO Secondary, SPI Master and Slave, UART, Micro DMA, USART, Watchdog Timer, USART Master and Slave
- UC Supported Peripherals - GPIO, I2C, Calendar,  SPI, SSI, SIO, Micro DMA, UART/USART, Watchdog Timer, Timers , Config Timers, ULP (Ultra Low Power) Timers, SDIO Secondary, PWM, ADC, EFUSE, I2S, PSRAM, Sleeptimer, IOSTREAM, MEMLCD.
- ULP (Ultra Low Power) Peripherals - ulp\_gpio, ulp\_adc ulp\_i2c, ulp\_i2s, ulp\_ir\_decoder, ulp\_spi, ulp\_timer, ulp\_uart, ulp\_udma

**Wi-Fi**

- WPA2/WPA3 Personal, WPS and WPA2-EAP Support for Station Mode
- TCP/IP stack with IPv4, IPv6, TCP, UDP, TLS, HTTP/HTTPS, DHCPv4, DHCPv6, MQTT Client, DNS Client, SNTP client.
- Support for SSL client versions TLSV1.0, TLSV1.2, TLSV1.3 and SSL server versions TLSV1.0 and TLSV1.2
- Wi-Fi STA (802.11n/802.11ax) + 802.11n AP Support
- HE (802.11ax) and Individual TWT Feature Support
- Multiple BSSID feature support as an STA
- Secure OTA support on B0 radio boards.
- Support for LWIP network stack
- Support for AWS MQTT library
- Support for BSD and IoT sockets application programming interface(API)

**Bluetooth Low Energy**

- BLE central & peripheral connections are supported.
- Ultra-low power save is supported.

**Power Save**

- The power save profiles supported are Max PSP, enhanced MAX PSP, Fast PSP, and 802.11ax TWT. 

**Crypto Support**

- AES, SHA, AES-CCM, HMAC, and TRNG FW Crypto API support added.
- PSA enabled Mbed TLS support added
- NVM support is added for key storage
- Symmetric key storage support added for Volatile and transparent key.
- TRNG, AES, SHA, and AES-CCM hardware crypto engine is integrated with PSA crypto.
## **Changes and Fixes**
**MCU**

- Added Ultra Low Power(ULP) examples support for SSI Master, USART, Micro DMA, Calendar, GPIO, I2C Leader, Timer.

**Wi-Fi**

- Added support for Efuse read, User Gain tables, Calibration Software APIs
- Added support for M4 soft reset
- Added example support tcp\_tx\_on\_periodic\_wakeup\_soc, wlan\_throughput\_psram, twt\_use\_case\_remote\_app  
- Enhancement of SNTP example to support async notification
- Updation of Readme files to have a unified "Project Environment" section
- Added support for sl\_wifi\_enable\_target\_wake\_time() API
- Added M4 power save support for embedded\_mqtt\_client\_twt and twt\_tcp\_client, twt\_use\_case\_demo, aws\_device\_shadow\_logging\_stats examples 
- Fixed security setting failure to apply properly while enabling the APUT in DUT
- Removal of file duplicates sl\_net\_default\_values.h
- Readme enhancement to fix setup diagram images
- Removal of all UAPSD references
- RTC timer get and set APIs
- Added PSRAM board support for applications
- Added TA and M4 power save for lwip\_tcp\_client example
- Updation of wlan\_throughput example SI91x asynchronous sockets for high throughputs
- Added support to handle transmit test opermode
- Updated print\_sl\_ipv6\_address() utility to print IP address in new format.
- Added M4 power save support for tcp\_tx\_on\_periodic\_wakeup example
- Added thread-safe support for malloc and free functions
- Added SNI option for sockets
- Added support for AP stop feature
- Fixed issue stability issues in firmware update
- Added support for WPA3
- Added API support for configuration of listen interval
- Added NCP support for AWS MQTT, twt\_tcp\_client, power\_save\_deep\_sleep, enterprise\_client examples
- Added crypto examples - HMAC, PSA SHA, si91x\_psa\_asymmetric\_key\_storage, si91x\_psa\_symmetric\_key\_storage, si91x\_psa\_aes, ECDH, PSA GCM, PSA HMAC.
- Fixed async event handling in the aws\_device\_shadow application
- Updated readme of cli\_demo with steps to add new command handlers

**BLE**

- Fixed high power numbers issue in BLE PER application with Tx and Rx in SoC mode
- Added  ble\_testmodes demo in SoC & NCP
- Added gatt\_long\_read, ble\_ae\_central and ble\_ae\_peripheral applications in SoC
- Renaming of all references of whitelist to acceptlist in all BLE examples
- Added support for test agent application for BLE commands
- Added support for rsi\_ble\_req\_hci\_raw  command 
- Added support for  352K\_320K memory configuration in immediate\_alert\_client example application

**CoEx** 

- Added wifi\_station\_ble\_throughput\_app coex application
- Added studio support and logging stats feature for wifi\_station\_ble\_provisioning\_aws\_logging\_stats application
- Fixed  BLE notifications issue in wifi\_throughput\_ble\_dual\_role application
- Fixed 0 Mbps data samples in WLAN throughput test in wifi\_throughput\_ble\_dual\_role application

**System**

- None
## **Recommendations**
**System**

- The current revision of SiWx917 B0 has:
  - RAM memory of 672k bytes which can be shared between TA and M4 processors in NCP/SoC mode. The following memory configuration modes are supported.
    - EXT\_FEAT\_672K\_M4SS\_0K- This mode configures TA with 672k and M4 with 0K bytes of memory which is applicable only in the case of NCP mode
  - The below configurations are applicable in SoC mode and can be configured based on the application requirement By default EXT\_FEAT\_352K\_M4SS\_320K configuration will be selected in SoC mode
    - EXT\_FEAT\_480K\_M4SS\_192K - This mode configures TA with 480k and M4 with 192K bytes of memory
    - EXT\_FEAT\_416K\_M4SS\_256K - This mode configures TA with 416k and M4 with 256K bytes of memory
    - EXT\_FEAT\_352K\_M4SS\_320K - This mode configures TA with 352k and M4 with 320K bytes of memory
  - The currently supported memory configuration for NCP mode is 672K\_M4SS\_0K.
  - SoC mode should not use 672k\_M4SS\_0k memory configuration.
- Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.

**IDE**

- Download the latest Simplicity Studio (SV5.7.0.1)
- Refer to the latest version of the SoC/NCP "Getting-Started-with-SiWx917" guide for more details

**Network Stack**

- The recommended embedded MQTT Publish payload is 1 kBytes.

**Wi-Fi**

- It is recommended to enable bit 16 of the 'Extended TCP IP Feature' bit map in the opermode command for all Wi-Fi Socket operations from the host to ensure graceful handling during asynchronous closures from the peer.
- Aggregation (bit 2 of feature\_bit\_map) is recommended to be enabled in opermode. This enables software to aggregate multiple packets to achieve better throughput
- TCP retransmission count value is recommended to be 30 or higher so that the TCP does not disconnect while the rejoin happens.
- To restart the module, the application needs to call sl\_wifi\_deinit() followed by sl\_wifi\_init().
- Register 'Join Fail Callback' function every time when join is called, as in the rejoin failure path the callback is deregistered. If not done, this might result in a scan not functioning on rejoin failure.
- User can enable SL\_SI91X\_EXT\_TCP\_IP\_SSL\_16K\_RECORD in 'Extended TCP IP Feature'bit map in opermode, if HTTPS download is failing after receiving HTTP headers..
- iTWT setup is recommended after IP assignment/TCP connection/application specific socket connections.
- When using sl\_wifi\_enable\_target\_wakeup\_time API, increase TCP / ARP Timeouts at the remote side depending upon the configured TWT interval configured.
- In case of TWT+coex, when using sl\_wifi\_enable\_target\_wakeup\_time API, use TWT wake duration <= 16 ms and TWT wake interval >= 1 sec. If wake duration > 16 ms or TWT wake interval < 1sec, there might be performance issues.
- For iTWT GTK Interval Should be kept max possible value or zero. If GTK interval is not configurable, recommended TWT interval (in case of sl\_wifi\_enable\_target\_wakeup\_time API) or RX Latency (in case of sl\_wifi\_target\_wake\_time\_auto\_selection API) is less than 4sec.
- Disable Power Save for High Throughput Applications.
- The application needs to ensure that it sets RTC with the correct timestamp when the feature is enabled before establishing the SSL connection.
- The timeout value should not be less than 1 second for socket select and socket receive calls. Timeout value of less than 1 second is not currently supported.
- When sl\_wifi\_enable\_target\_wakeup\_time API is used, configuring TWT Wake interval beyond 1 min might lead to disconnections from the AP. Recommended to use TWT wakeup interval of less than or equal to 1 min.
- Embedded MQTT keep alive interval should be either 0 or a value greater than 35 seconds.
- When using sl\_wifi\_enable\_target\_wakeup\_time API, it is recommended to set missed\_beacon\_count of sl\_wifi\_set\_advanced\_client\_configuration API greater than 2 times of the configured TWT Interval.

**BLE**

- In BLE, the recommended range of Connection Interval in
  - Power Save (BLE Only) - 100 ms to 1.28 s.
- In BLE, during Connection, the configuration of Scan Interval and Scan Window with the same value is not recommended. The suggested ratio of Scan Window to Scan Interval is 3:4.
- In BLE, if a device is acting as Central, the scan window (in set\_scan\_params and create\_connection commands) must be less than the existing Connection Interval. The suggested ratio of Scan Window to Connection Interval is 2:3.
- In BLE mode, if scanning and advertising are in progress on the SiWx91x module and it subsequently gets connected and moves to the central role, scanning stops else if it moves to the peripheral role, advertising stops. To further establish a connection to another peripheral device or to a central device, the application should give a command for starting advertising and scanning again.

**Co-Existence**

- For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
- Wi-Fi + BLE Advertising
  - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals may be observed.
  - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity.
- Wi-Fi + BLE scanning
  - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
  - BLE scanning will be stopped for intervals that collide with Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Connections
  - All standard connection intervals are supported.
  - For a stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Data Transfer
  - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout.
  - Ensure Wi-Fi activity consumes lower intervals.
## **Known Issues**
**Network Stack**

- Sometimes during SSL Handshake, ECC curve parameters generated are wrong, resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
- Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with the power save.
- Secure SSL renegotiation not supported in Embedded Networking Stack.

**Wi-Fi**

- TWT device (STA) may not go to sleep in between TWT intervals in case of burst data traffics with data size more than the configured 'average tx throughput', until completion of data transfer. 
- There are performance and IOP issues with MU MIMO.
- WPA3 Connection with the Hunting and Pecking algorithm takes about 3-4 seconds.
- Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.
- Low Wi-Fi+BLE throughput
- TCP/UDP TX/RX Data stall observed in CoEx scenarios
- Observed HTTPS Continuous download fails with TA Power save
- Performance improvements are in progress for lower performance metrics.
- Observed interop issue (connectivity failure) with few AP's when device reset/reboot happens on active connection.
- In concurrent mode, AP restart fails after stopping it with ap\_stop command. 
- DUT deep sleep will not work after stopping AP with ap\_stop command.
- Unable to get SNTP async events when coex mode and Power save is enabled
- Fine Tuning 802.11ax Spectral mask in Progress
- Fine Tuning 11b mask in Ch11 in progress
- DUT is unable to connect to the AP with WPA3 security when we enable coex mode (WLAN+BLE) in opermode
- Socket listen API is failing from the second iteration
- Disconnections observed with Netgear RAX120 AP in WPA3 security 
- Observed connection failures with some AP's in more channel congestion open lab environment

**BLE**

- M4 Power save with RTOS is not supported for BLE scan and data transfer in SoC Mode.
- BLE anchor points will experience a miss rate of approximately 10%, when the slave latency is enabled.
- When power save is enabled, the device stops sending BLE scan requests after 20 seconds.
- DUT hangs when transmitting BLE notification with codded PHY rates of 125kbps and 500kbps, when DLE is enabled. 
- DUT is not transmitting packets continuously over the air in BLE test modes.
- After connecting to the remote device using gatt\_test applications, GATT APIs could not be triggered. 
- DUT hangs during multiple iterations of BLE connect and disconnect tests with TA and M4 power enabled.

**Co-Existence**

- Observed HTTP continuous download is not happening in Powersave mode after BLE connection established using wifi\_https\_ble\_dual\_role application
- Observed DUT hang while Wi-Fi UDP Tx and BLE continuous scan run for long time.
- sl\_wifi\_init throws insufficient memory(0x2C) error, when low\_power\_mode feature is enabled in CoEx application with "TA\_352K\_M4SS\_320K" Memory configuration.

**System**

- None
### **MCU**
- Refer to the respective Radio board user guides for specific pinout details. Examples are targeted to a specific version mentioned in the respective readme.
- Few peripherals like WDT when configured with RO clock is not working as expected. No impact on the feature but the clocks are not accurate.
## **Limitations and Unsupported Features**
**Wi-Fi/Network Stack**

- TLS 1.3 Server is not supported
- In AP mode, IPv6 Router Advertisements are not supported. Although STA connected to the APUT can transfer IPv6 data to the peers by using SLAAC for address configuration.
- When sl\_wifi\_enable\_target\_wake\_time() API is used, observed occasional MQTT disconnections with longer TWT intervals (>30secs) with embedded MQTT + TWT.
- AMSDU TX is not supported.
- Fragmentation is not supported.
- AMSDUs within AMPDU is not supported.
- Currently, the module does not support Radio Measurement Requests feature of CCX V2
- 802.11k is not supported
- 40 MHz bandwidth for 2.4 GHz band is not supported.
- 802.11J channel 14 is not supported.
- MQTT versions 3 and 4 are supported.
- Maximum of 3 SSL connections are supported in Wi-Fi alone mode.
- In SSL ECC Curve ID supported is 23. SSL handshake with 3rd party clients depends on the SSL ECC Curve ID.
- LP mode is not supported.
- In iTWT mode, downlink data traffic should be limited. Disconnections may be observed if AP fails to send all the buffered data in the next TWT Service Period.
- Number of MBSSIDs processed is limited by the beacon length that can be processed by the stack (which is limited to 1024 bytes). Beacons greater than 1024 Bytes in length will not be processed.
- In A0 SoC mode, M4 power save will work with 384k TA and 320 M4 memory configuration only. It is not supported for "448K TA and 256k M4" and "512k TA and 192k M4" memory configurations
- CoEx + EAP Security in 352K/320k is not feasible. This issue is observed due to a lack of memory in the 352k/320k Memory Configuration. It is supported with other EXT\_FEAT\_416K\_M4SS\_256K and EXT\_FEAT\_480K\_M4SS\_192K memory configurations.
- UL MUMIMO is not supported.
- The embedded HTTP server is not supported.
- Baremetal is not supported
- Compared to SoC mode, NCP mode has fewer examples, but more will be enabled in upcoming releases.

**BLE**

- For BLE, if the connection is established with a small connection interval (less than 15 ms), simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported.
- BLE maximum two concurrent connections are supported, which can be either a connection to two peripheral devices, to one central and one peripheral device or two central devices.
- BLE Slave latency value is valid up to 32 only.
- BLE TX/RX throughput is less when tested with EFM as compared to EFR
- Maximum supported AE data length is 200 bytes.
- Supports only two ADV\_EXT sets.
- Supports only two BLE connections (1 Central and 1 Peripheral) with AE.
- Advertising Extension feature is not supported in Coexistence.
- The demo's ae\_central & ae\_peripheral are not supported with TA\_352K\_M4\_320K RAM configuration. 

**MCU**

- ULP Peripherals examples configurations are for RAM-based execution, as flash is powered off in ULP Mode.
- On 4338a board, two I2C instances(I2C1 and I2C2) are using same GPIOs, so support is not there for using two I2C instances simultaneously.
- I2C High speed mode not supported.
- Manual chip select option is not supported when using GSPI and SSI.

**Simplicity Studio and Commander**

- Default GDB debug configuration is not supported for SoC projects, work around details provided in "Getting-Started-with-SiWx917"
- Observing flash download failures(ERROR 108) for a few image sizes
- While creating project through Simplicity Studio, "copy contents" option not supported.
- Simplicity commander does not support options under "Debug Lock tools" . 
## **Terminology**

|**Term**|**Description**|
| :-: | :-: |
|**New Features**|These items are new to this release|
|**Changes/Issues Fixed**|Changes made to existing features found in previous software releases.|
||Enhancements to existing product flow and feature set.|
||Bug fixes done in the release|
|**Deprecated Items**|Features or functions or APIs that are removed from the distributed release to align with the software roadmap|
|**Known Issues**|Features or functions that do not work as planned at the time of release. Workarounds may be offered to meet short-term development goals, but longer-term solutions will be added in future software releases|
|**Limitations/Recommendations**|Describes the limitations on product usage and recommendations for optimal use cases|
## **Note**
- Change SPI frequency to 12.5Mhz while measuring BLE throughput with EFR32

<br>

**WiSeConnect3\_SDK\_3.0.13 Release Notes**

This SDK package supports SoC Mode and NCP Mode for A0 and B0:

- In SoC Mode: Customer Applications can run on Cortex M4. Wireless, network, and security stacks run on the Wireless subsystem.
- In NCP mode: Wireless, network, and security stacks run on the Wireless subsystem. Customer applications run on the external host MCU.
- Silicon Labs' SiWx917 includes an ultra-low power Wi-Fi 6 plus Bluetooth Low Energy (BLE) 5.1 wireless subsystem and an integrated micro-controller application subsystem.

Note: Use Simplicity Studio version SV5.7.0.1 onwards

**SoC Highlights**

- **Operating Modes** : Wi-Fi STA (802.11ax, 802.11n), Wi-Fi 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + BLE
- **Wireless Protocols** : IEEE 802.11b, 802.11g, 802.11n, 802.11ax, BLE 5.1
- **Multiple Ultra Low Power Modes** : Support Ultra low power Wi-Fi Standby associated current in Wi-Fi 4 mode, Ultra low power Wi-Fi Standby associated current in Wi-Fi 6 mode leveraging TWT, Always on Cloud connected power, BLE connected power, and, deep sleep ULP mode
- **MCU examples** :  GPIO, I2C, Calendar,  SPI, SSI, SIO, UDMA, UART/USART, WDT, Timers, Config Timers, ULP (Ultra Low Power) Timers, Sleeptimer, IOSTREAM, NVM3, MEMLCD.
- **Wi-Fi** : 2.4GHz Wi-Fi Support - WPA2/WPA3 Personal, Wi-Fi 6 OFDMA/MU-MIMO Support that enhances network capacity and reduces latency
- **Cloud** : Amazon Cloud Connectivity Support, Amazon FreeRTOS
- **BLE** : BLE 5.1, BLE dual role (Central and Peripheral Support)
- **Integrated Stacks and Profiles** : Wi-Fi Stack, BLE Stack, and Profiles, TCP/IP stack with TLS 1.3, HTTP/HTTPS, DHCP, MQTT Client, DNS client
- **IDE** : Simplicity Studio with Windows 10(64-bit), Linux, Mac OS
- **TA/M4 Firmware Update Tool** : Simplicity Commander
- **Hardware kits to evaluate SoC Mode** :
  - Si-MB4002A Main board
  - SiWx917 Radio boards

|**RadioBoard Type**|**SiWx917A0 with Board Version**|**SiWx917B0 with Board Version**|
| :-: | :-: | :-: |
|Common flash|Si917-BRD4325A(1.0 /1.1 /1.2)|-|
|Common flash|-|Si917-BRD4338A(2.0)|
|Dual flash|Si917-BRD4325B(1.0 /1.1 /1.2)|-|

**NCP Highlights**

- **Host Interface** : SPI
- **Operating Modes** : Wi-Fi STA (802.11ax, 802.11n), Wi-Fi 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + BLE
- **Wireless Protocols** : IEEE 802.11b, 802.11g, 802.11n, 802.11ax, BLE 5.1
- **Multiple Ultra Low Power Modes** : Support Ultra low power Wi-Fi Standby associated current in Wi-Fi 4 mode, Ultra low power Wi-Fi Standby associated current in Wi-Fi 6 mode leveraging TWT, Always on Cloud connected power, BLE connected power, and, deep sleep ULP mode.
- **Wi-Fi** : 2.4GHz Wi-Fi Support, WPA2/WPA3 Personal, Wi-Fi 6 OFDMA/MU-MIMO Support that enhances network capacity and reduces latency
- **BLE** : BLE 5.1, BLE dual role (Central and Peripheral Support)
- **Integrated Stacks and Profiles** : Wi-Fi Stack , BLE Stack and Profiles, TCP/IP stack with TLS 1.3, HTTP/HTTPS, DHCP, MQTT Client, DNS Client.
- **IDE** : Simplicity Studio with Windows 10(64-bit)
- **Hardware kits to evaluate NCP Mode**
  - Si917 B0 Single Band Radio Board (BRD4338A)
  - Wireless Pro kit Mainboard BRD4002A Rev A06
  - EFR32xG21 2.4 GHz 20 dBm Radio Board (BRD4180B Rev A02)

**Release Details**

|**Item**|**Details**|
| :-: | :-: |
|Release date|Aug 18th, 2023|
|SDK Version|3\.0.13|
|Firmware Version|1711\.2.9.0.0.19|
|Package Name|WiSeConnect3\_SDK\_3.0.13|
|Supported RTOS|FreeRTOS|
|Operating Modes Supported|Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP|
|Build Quality|Beta(NCP)|

**Updating to this Release**

- SiWx917 release consists of two components
  - Wireless Firmware - SiWx917 Firmware Binaries
  - Wiseconnect3 Library - Wiseconnect3 SDK Library runs on the external host in NCP mode and internal Cortex M4 in SoC Mode.
- This release is meant only for use with designs based on SiWx917 Silicon
- The firmware to load in your design are available in the following format:
  - SiWG917-A.2.9.0.0.x.rps - A0 Full-featured firmware binary
  - SiWG917-B.2.9.0.0.x.rps - B0 Full-featured firmware binary

**Features Supported**

**MCU**

- ADC Single Channel, ADC Static Mode, Analog Comparator, Blinky, Bod, CTS, DAC FIFO, DAC Static, GPIO, I2C, I2S, NVM3, OPAMP, PSRAM, PWM, QEI, RTC, SDIO Slave, SPI Master and Slave, UART, UDMA, USART, WDT, USART Master and Slave
- UC Supported Peripherals - GPIO, I2C, Calendar,  SPI, SSI, SIO, UDMA, UART/USART, WDT, Timers , Config Timers, ULP (Ultra Low Power) Timers, Sleeptimer, IOSTREAM, MEMLCD
- ULP (Ultra Low Power) Peripherals - ulp\_gpio, ulp\_adc ulp\_i2c, ulp\_i2s, ulp\_ir\_decoder, ulp\_spi, ulp\_timer, ulp\_uart, ulp\_udma

**Wi-Fi**

- WPA2/WPA3 Personal, WPS and WPA2-EAP Support for Station Mode
- TCP/IP stack with IPv4, IPv6, TCP, UDP, TLS, HTTP/HTTPS, DHCPv4, DHCPv6, MQTT Client, DNS Client, SNTP client.
- Support for SSL client versions TLSV1.0, TLSV1.2, TLSV1.3 and SSL server versions TLSV1.0 and TLSV1.2
- Wi-Fi STA (802.11n/802.11ax) + 802.11n AP Support
- HE (802.11ax) and Individual TWT Feature Support
- Multiple BSSID feature support as an STA
- Secure OTA support on B0 radio boards.
- Support for LWIP network stack
- Support for AWS MQTT library
- Support for BSD and IoT sockets application programming interface(API)

**Bluetooth Low Energy**

- BLE central & peripheral connections are supported.
- Ultra-low power save is supported in Test Mode to support Tx/Rx performance testing

**Power Save**

- The power save profiles supported are Max PSP, enhanced MAX PSP, and 802.11ax TWT. 

**Changes and Fixes**

**Wi-Fi**

- NCP support for brd4180b EFR
- Addition of featured examples wifi\_aws\_device\_shadow\_ncp, wifi\_ble\_per\_ncp, wifi\_firmware\_update\_ncp, wifi\_powersave\_standby\_associated\_ncp, wifi\_wlan\_throughput\_ncp 
- Addition of snippet examples wifi\_access\_point\_ncp, wifi\_concurrent\_mode\_ncp, wifi\_select\_app\_ncp, wifi\_station\_ping\_ncp, wifi\_lwip\_tcp\_client\_ncp, wifi\_sntp\_client\_ncp
- Adds M4 power save support for wifi\_station\_ble\_provisioning\_aws
- Asynchronous select support using Si91x sockets APIs
- Added fixes for WLAN data transfer(TCP Tx/UDP Tx) failures while starting BLE Notification in wifi\_throughput\_ble\_dual\_role example
- Added fixes for socket bind failures in cli\_demo while running in multiple iterations 
- Added fixes for HTTP GET and HTTP POST when the application is run for multiple iterations

**BLE**

- Fixes compilation errors of ble\_hid\_on\_gatt application when configured GATT\_ROLE as CLIENT
- Added support for  352K\_320K memory configuration in ble\_central example application

**System**

- None

**Recommendations**

**System**

- The current revision of SiWx917 A0 has:
  - RAM memory of 704k bytes which can be shared between TA and M4 processors in NCP/SoC mode. The following memory configuration modes are supported.
    - EXT\_FEAT\_704K\_M4SS\_0K- This mode configures TA with 704k and M4 with 0K bytes of memory which is applicable only in the case of NCP mode
  - The below configurations are applicable in SoC mode and can be configured based on the application requirement By Default EXT\_FEAT\_384K\_M4SS\_320K configuration will be selected in SoC mode
    - EXT\_FEAT\_512K\_M4SS\_192K - This mode configures TA with 512k and M4 with 192K bytes of memory
    - EXT\_FEAT\_448K\_M4SS\_256K - This mode configures TA with 448k and M4 with 256K bytes of memory
    - EXT\_FEAT\_384K\_M4SS\_320K - This mode configures TA with 384k and M4 with 320K bytes of memory
  - The currently supported memory configuration for NCP mode is 704K\_M4SS\_0K.
  - SoC mode should not use 704k\_M4SS\_0k Memory configuration.
  - In SoC mode, M4 power save will work with 384k TA and 320 M4 memory configuration only. It is not supported for "448K TA and 256k M4" and "512k TA and 192k M4" memory configurations.
- The current revision of SiWx917 B0 has:
  - RAM memory of 672k bytes which can be shared between TA and M4 processors in NCP/SoC mode. The following memory configuration modes are supported.
    - EXT\_FEAT\_672K\_M4SS\_0K- This mode configures TA with 672k and M4 with 0K bytes of memory which is applicable only in the case of NCP mode
  - The below configurations are applicable in SoC mode and can be configured based on the application requirement By Default EXT\_FEAT\_352K\_M4SS\_320K configuration will be selected in SoC mode
    - EXT\_FEAT\_480K\_M4SS\_192K - This mode configures TA with 480k and M4 with 192K bytes of memory
    - EXT\_FEAT\_416K\_M4SS\_256K - This mode configures TA with 416k and M4 with 256K bytes of memory
    - EXT\_FEAT\_352K\_M4SS\_320K - This mode configures TA with 352k and M4 with 320K bytes of memory
  - The currently supported memory configuration for NCP mode is 672K\_M4SS\_0K.
  - SoC mode should not use 672k\_M4SS\_0k Memory configuration.
  - In SoC mode, M4 power save will work with any memory configuration.
- Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.

**IDE**

- Download the latest Simplicity Studio (SV5.7.0.1)
- Refer to the latest version of the SoC/NCP "Getting-Started-with-SiWx917" guide for more details

**Wi-Fi**

- It is recommended to enable bit 16 of the 'Extended TCP IP Feature' bit map in the opermode command for all Wi-Fi Socket Operations from the host to ensure graceful handling during asynchronous closures from the peer.
- Aggregation (bit 2 of feature\_bit\_map) is recommended to be enabled in opermode. This enables sw to aggregate multiple packets to achieve better throughput
- TCP retransmission count value is recommended to be 30 or higher so that the TCP does not disconnect while the rejoin happens.
- To restart the module, the application needs to call sl\_wifi\_deinit() followed by sl\_wifi\_init().
- Register 'Join Fail Callback' function every time when join is called, as in the rejoin failure path the callback is deregistered. If not done, this might result in a scan not functioning on rejoin failure.
- User can enable SL\_SI91X\_EXT\_TCP\_IP\_SSL\_16K\_RECORD in 'Extended TCP IP Feature'bit map in opermode, if HTTPS download is failing after receiving HTTP headers..
- iTWT setup is recommended after IP assignment/TCP connection/application connection.
- When using sl\_wifi\_enable\_target\_wakeup\_time API, increase TCP / ARP Timeouts at the remote side depending upon the configured TWT interval configured.
- In case of TWT+coex, when using sl\_wifi\_enable\_target\_wakeup\_time API, use TWT wake duration <= 16 ms and TWT wake interval >= 1 sec. If wake duration > 16 ms or TWT wake interval < 1sec, there might be performance issues.
- For iTWT GTK Interval Should be kept max possible value or zero. If GTK interval is not configurable, recommended TWT interval (in case of sl\_wifi\_enable\_target\_wakeup\_time API) 
- Disable Power Save for High Throughput Applications.
- The application needs to ensure that it sets RTC with the correct timestamp when the feature is enabled before establishing the SSL connection.
- Timeout value should be minimum 1 second for socket select and socket receive calls. Timeout value of less than 1 second is not currently supported.
- When sl\_wifi\_enable\_target\_wakeup\_time API is used, configuring TWT Wake interval beyond 1 min might lead to disconnections from the AP. Recommended to use TWT wakeup interval of less than or equal to 1 min.
- MQTT keep alive interval should be either 0 or a value greater than 35 seconds.
- When using sl\_wifi\_enable\_target\_wakeup\_time API, it is recommended to set missed\_beacon\_count of sl\_wifi\_set\_advanced\_client\_configuration API greater than 2 times of the configured TWT Interval.
- Secure OTA: Keys need to be programmed before validating Secure OTA

**BLE**

- In BLE, recommended range of Connection Interval in
  - Power Save (BLE Only) - 100 ms to 1.28 s.
- In BLE, during Connection, the configuration of Scan Interval and Scan Window with the same value is not recommended. The suggested ratio of Scan Window to Scan Interval is 3:4.
- In BLE, if a device is acting as Central, the scan window (in set\_scan\_params and create\_connection commands) must be less than the existing Connection Interval. The suggested ratio of Scan Window to Connection Interval is 2:3.
- In BLE mode, if scanning and advertising are in progress on the SiWx91x module and it subsequently gets connected and moves to the central role, scanning stops else if it moves to the peripheral role, advertising stops. To further establish a connection to another peripheral device or to a central device, the application should give a command for starting advertising and scanning again.

**Co-Existence**

- For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
- Wi-Fi + BLE Advertising
  - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals may be observed.
  - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity.
- Wi-Fi + BLE scanning
  - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
  - BLE scanning will be stopped for intervals that collide with Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Connections
  - All standard connection intervals are supported.
  - For a stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Data Transfer
  - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout.
  - Ensure Wi-Fi activity consumes lower intervals.

**Known Issues**

**Network Stack**

- Sometimes during SSL Handshake, ECC curve parameters generated are wrong, resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
- Recommended MQTT Publish payload is 1 kBytes.
- Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with the power save.
- Secure SSL Renegotiation not supported in Embedded Networking Stack.

**Wi-Fi**

- Device with active TWT will not go to sleep when the buffer is full.
- There are performance and IOP issues with MU MIMO.
- WPA3 connection takes 2.5 seconds.
- Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.
- EAP connection with certificates of 4096-bit key length is not supported.
- Low Wi-Fi+BLE throughput
- Data stall and disconnection with TCP Tx data transfer when TA power save is enabled
- TCP/UDP TX/RX Data stall observed in CoEx scenarios
- HTTPS Continuous download fails with TA Power save
- Low-performance metrics.
- Data stall when WLAN TCP data transfer, BLE scanning, BLE advertising, and BLE notification are enabled

**BLE**

- BLE Range is less with A0/B0 dual flash Radio boards when compared to A0/B0 common flash Radio boards.
- BLE applications hang after disconnection due to Disable Power save API failure
- Observing data stall with TCP Tx/Rx when connecting BLE With wifi\_throughput\_ble\_dual\_role application.

**System**

- None

**MCU**

- Refer the respective Radio board user guides for specific pinout details. Example readme's currently refer to A0 board versions
- IOSTREAM by default works only with 115200 baud rate. To work with different baud rates , undefine the  USART\_UC macro in sl\_91x..usart.c.
- SSI bitrate above 10 Mbps is not working as expected.
- when using SL DMA, UDMA1 is not working as expected
- SIO-SPI few channels are not working as expected. (working of MOSI & MISO channel 4 and above is inconsistent) 
- Few peripherals like WDT when configured with RO clock is not working as expected. No impact on the feature but the clocks are not accurate.

**Limitations and Unsupported Features**

**Wi-Fi/Network Stack**

- TLS 1.3 Server is not supported
- In AP mode, IPv6 Router Advertisements are not supported. Although STA connected to the APUT can transfer IPv6 data to the peers by using SLAAC for address configuration.
- When sl\_wifi\_enable\_target\_wakeup\_time() API is used, observed occasional MQTT disconnections with longer TWT intervals (>30secs) with embedded MQTT + TWT.
- AMSDU TX is not supported.
- Fragmentation is not supported.
- AMSDUs within AMPDU is not supported.
- Currently, the module does not support Radio Measurement Requests feature of CCX V2
- 802.11k is not supported
- Short GI is not supported
- 40 MHz bandwidth for 2.4 GHz band is not supported.
- 802.11J channel 14 is not supported.
- MQTT versions 3 and 4 are supported.
- Maximum of 3 SSL connections are supported in Wi-Fi alone mode.
- In SSL ECC Curve ID supported is 23. SSL handshake with 3rd party clients depends on the SSL ECC Curve ID.
- LP mode is not supported.
- Fine Tuning 802.11ax Spectral mask in Progress
- Fine Tuning 11b mask in Ch11 in progress
- In iTWT mode, downlink data traffic should be limited. Disconnections may be observed if AP fails to send all the buffered data in the next TWT Service Period.
- Number of MBSSIDs processed is limited by the beacon length that can be processed by the stack (which is limited to 1024 bytes). Beacons greater than 1024 Bytes in length will not be processed.
- In A0 SoC mode, M4 power save will work with 384k TA and 320 M4 memory configuration only. It is not supported for "448K TA and 256k M4" and "512k TA and 192k M4" memory configurations
- Timeout value should be a minimum 1 second for socket select and socket receive calls. Timeout value less than 1 second is not currently supported.
- M4 OTA update on dual flash will work for same flash sizes only (internal and External).
- CoEx + EAP Security in 352K/320k is not feasible. This issue is observed due to a lack of memory in the 352k/320k Memory Configuration. It is supported with other EXT\_FEAT\_416K\_M4SS\_256K and EXT\_FEAT\_480K\_M4SS\_192K memory configurations.
- UL MUMIMO is not supported.
- The embedded HTTP server is not supported.
- SNI is not supported
- Baremetal is not supported

**BLE**

- For BLE, if the connection is established with a small connection interval (less than 15 ms), simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported.
- BLE maximum two concurrent connections are supported, which can be either a connection to two peripheral devices, to one central and one peripheral device or two central devices.
- BLE Slave latency value is valid up to 32 only.
- BLE TX/RX throughput is less when tested with EFM as compared to EFR
- Maximum supported AE data length is 200 bytes.
- Supports only two ADV\_EXT sets.
- Supports only two BLE connections( 1 Central and 1 Peripheral) with AE.
- Filtering policy is not supported for Periodic advertising.

**MCU**

- ULP Peripherals examples configurations are for RAM-based execution, as flash is powered off in ULP Mode
- On 4338a board, two I2C instances(I2C1 and I2C2) are using same GPIOs ,so support is not there for using two I2C instances simultaneously
- I2C Fast and High speed mode not supported
- when using GSPI and SSI, manual chip select option not supported

**Simplicity Studio and Commander**

- Default GDB debug configuration is not supported for SoC projects, work around details provided in "Getting-Started-with-SiWx917"
- Observing flash download failures(ERROR 108) for a few image sizes
- While creating project through Simplicity Studio, "copy contents" option not supported.
- Simplicity commander does not support options under "Debug Lock tools" . 

**Terminology**

|**Term**|**Description**|
| :-: | :-: |
|**New Features**|These items are new to this release|
|**Changes/Issues Fixed**|Changes made to existing features found in previous software releases.|
||Enhancements to existing product flow and feature set.|
||Bug fixes done in the Release|
|**Deprecated Items**|Features or functions or APIs that are removed from the distributed release to align with the software roadmap|
|**Known Issues**|Features or functions that do not work as planned at the time of release. Workarounds may be offered to meet short-term development goals, but longer-term solutions will be added in future software releases|
|**Limitations/Recommendations**|Describes the limitations on product usage and recommendations for optimal use cases|

**Note**

- NA

<br>

# **Wi-Fi SDK v3.0.12**

# **WiSeConnect3\_SDK\_3.0.12 Release Notes**

This SDK package supports SoC Mode and NCP Mode for A0 and B0:

- In SoC Mode: Customer Applications can run on Cortex M4. Wireless, network, and security stacks run on the Wireless subsystem.
- In NCP mode: Wireless, network, and security stacks run on the Wireless subsystem. Customer applications run on the external host MCU.
- Silicon Labs' SiWx917 includes an ultra-low power Wi-Fi 6 plus Bluetooth Low Energy (BLE) 5.1 wireless subsystem and an integrated micro-controller application subsystem.

Note: Use Simplicity Studio version SV5.7.0.1 onwards

## **SoC Highlights**

- **Operating Modes** : Wi-Fi STA (802.11ax, 802.11n), Wi-Fi 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + 802.11n AP, Wi-Fi STA (802.11ax, 802.11n) + BLE
- **Wireless Protocols** : IEEE 802.11b, 802.11g, 802.11n, 802.11ax, BLE 5.1
- **Multiple Ultra Low Power Modes** : Support Ultra low power Wi-Fi Standby associated current in Wi-Fi 4 mode, Ultra low power Wi-Fi Standby associated current in Wi-Fi 6 mode leveraging TWT, Always on Cloud connected power, BLE connected power, and, deep sleep ULP mode
- **MCU examples** :  GPIO, I2C, Calendar,  SPI, SSI, SIO, UDMA, UART/USART, WDT, Timers, Config Timers, ULP (Ultra Low Power) Timers, Sleeptimer, IOSTREAM, NVM3, MEMLCD.
- **Wi-Fi** : 2.4GHz Wi-Fi Support - WPA2/WPA3 Personal, Wi-Fi 6 OFDMA/MU-MIMO Support that enhances network capacity and reduces latency
- **Cloud** : Amazon Cloud Connectivity Support, Amazon FreeRTOS
- **BLE** : BLE 5.1, BLE dual role (Central and Peripheral Support)
- **Integrated Stacks and Profiles** : Wi-Fi Stack, BLE Stack, and Profiles, TCP/IP stack with TLS 1.3, HTTP/HTTPS, DHCP, MQTT Client, DNS client
- **IDE** : Simplicity Studio with Windows 10(64-bit), Linux, Mac OS
- **TA/M4 Firmware Update Tool** : Simplicity Commander
- **Hardware kits to evaluate SoC Mode** : 

o   Si-MB4002A Main board

o   SiWx917 Radio boards

|**RadioBoard Type**|**SiWx917A0 with Board Version**|**SiWx917B0 with Board Version**|
| :-: | :-: | :-: |
|Common flash|Si917-BRD4325A(1.0 /1.1 /1.2)|-|
|Common flash|-|Si917-BRD4338A(2.0)|
|Dual flash|Si917-BRD4325B(1.0 /1.1 /1.2)|-|

## **Release Details**

|**Item**|**Details**|
| :-: | :-: |
|Release date|Aug 10th, 2023|
|SDK Version|3\.0.12|
|Firmware Version|1711\.2.9.0.0.19|
|Package Name|WiSeConnect3\_SDK\_3.0.12|
|Supported RTOS|FreeRTOS|
|Operating Modes Supported|Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP|
|Build Quality|Beta|

## **Updating to this Release**

- SiWx917 release consists of two components 
  - Wireless Firmware - SiWx917 Firmware Binaries
  - Wiseconnect3 Library - Wiseconnect3 SDK Library runs on internal Cortex M4 in SoC Mode.
- This release is meant only for use with designs based on SiWx917 Silicon
- The firmware to load in your design are available in the following format: 
  - SiWG917-A.2.9.0.0.x.rps - A0 Full-featured firmware binary
  - SiWG917-B.2.9.0.0.x.rps - B0 Full-featured firmware binary

## **Features Supported**

**MCU**

- ADC Single Channel, ADC Static Mode, Analog Comparator, Blinky, Bod, CTS, DAC FIFO, DAC Static, GPIO, I2C, I2S, NVM3, OPAMP, PSRAM, PWM, QEI, RTC, SDIO Slave, SPI Master and Slave, UART, UDMA, USART, WDT, USART Master and Slave
- UC Supported Peripherals - GPIO, I2C, Calendar,  SPI, SSI, SIO, UDMA, UART/USART, WDT, Timers , Config Timers, ULP (Ultra Low Power) Timers, Sleeptimer, IOSTREAM, MEMLCD
- ULP (Ultra Low Power) Peripherals - ulp\_gpio, ulp\_adc ulp\_i2c, ulp\_i2s, ulp\_ir\_decoder, ulp\_spi, ulp\_timer, ulp\_uart, ulp\_udma

**Wi-Fi**

- WPA2/WPA3 Personal, WPS and WPA2-EAP Support for Station Mode
- TCP/IP stack with IPv4, IPv6, TCP, UDP, TLS, HTTP/HTTPS, DHCPv4, DHCPv6, MQTT Client, DNS Client, SNTP client.
- Support for SSL client versions TLSV1.0, TLSV1.2, TLSV1.3 and SSL server versions TLSV1.0 and TLSV1.2
- Wi-Fi STA (802.11n/802.11ax) + 802.11n AP Support
- HE (802.11ax) and Individual TWT Feature Support
- Multiple BSSID feature support as an STA
- Secure OTA support on B0 radio boards.
- Support for LWIP network stack
- Support for AWS MQTT library
- Support for BSD and IoT sockets application programming interface(API)

**Bluetooth Low Energy**

- BLE central & peripheral connections are supported.
- Ultra-low power save is supported in Test Mode to support Tx/Rx performance testing

**Power Save**

- The powersave profiles supported are Max PSP, enhanced MAX PSP, and 802.11ax TWT. 

## **Changes and Fixes**

**MCU**

- Added project support for 1.2 dual flash and 2.0 boards
- SIO related bug fixes reported on SIO-USART and SIO-SPI
- Updated Readme files to provide pin configuration details
- WDT Example related bug fix

**Wi-Fi**

- TWT power number improvements.
- Added support for Si91X asynchronous sockets
- Added support for SNTP documentation
- Updated documentation for SL\_SI91X\_EXT\_FEAT\_XTAL\_CLK\_ENABLE
- Replacement of convert\_string\_to\_sl\_ipv4\_address() with sl\_net\_inet\_addr()
- Updation of sl\_net\_interface\_t enums
- Added Frontend switch selection in SRM
- Added timeout fixes for RF test example
- Added fixes related to socket bind
- Bug fixes HTTP/HTTPS file download in COEX mode
- M4 soft reset support in http\_otaf example
- Bug fixes related to certificate loading in COEX mode
- Bug fixes related to BLE connection in powersave mode
- Application bug fixes in wifi\_wlan\_throughput\_v6, ble\_multiconnection\_gatt example
- Bug fixes related to Wi-Fi rejoins in powersave mode
- Stability fixes for M4 sleep
- Support to SiWx917B0 2.0 radio boards
- Support for Wi-Fi transmission rate for the given 802.11b, 802.11g, and 802.11n protocols.
- Renaming of access\_point, concurrent\_mode, aws\_mqtt, powersave\_standby\_associated, select\_app, station\_ping, lwip\_tcp\_client, sntp\_client application with \_soc suffix
- Fixes MQTT username and password length checks in requests and asynchronous MQTT requests handling
- Support for functional OFDMA DL/UL.

**BLE**

- Fixes application timeouts in the ble\_throughput application
- Support to BLE PER mode

**System**

- None

## **Recommendations**

**System**

- The current revision of SiWx917 A0 has: 
  - RAM memory of 704k bytes which can be shared between TA and M4 processors in NCP/SoC mode. The following memory configuration modes are supported. 
    - EXT\_FEAT\_704K\_M4SS\_0K- This mode configures TA with 704k and M4 with 0K bytes of memory which is applicable only in the case of NCP mode
  - The below configurations are applicable in SoC mode and can be configured based on the application requirement By Default EXT\_FEAT\_384K\_M4SS\_320K configuration will be selected in SoC mode 
    - EXT\_FEAT\_512K\_M4SS\_192K - This mode configures TA with 512k and M4 with 192K bytes of memory
    - EXT\_FEAT\_448K\_M4SS\_256K - This mode configures TA with 448k and M4 with 256K bytes of memory
    - EXT\_FEAT\_384K\_M4SS\_320K - This mode configures TA with 384k and M4 with 320K bytes of memory
  - The currently supported memory configuration for NCP mode is 704K\_M4SS\_0K.
  - SoC mode should not use 704k\_M4SS\_0k Memory configuration.
  - In SoC mode, M4 power save will work with 384k TA and 320 M4 memory configuration only. It is not supported for "448K TA and 256k M4" and "512k TA and 192k M4" memory configurations.
- The current revision of SiWx917 B0 has: 
  - RAM memory of 672k bytes which can be shared between TA and M4 processors in NCP/SoC mode. The following memory configuration modes are supported. 
    - EXT\_FEAT\_672K\_M4SS\_0K- This mode configures TA with 672k and M4 with 0K bytes of memory which is applicable only in the case of NCP mode
  - The below configurations are applicable in SoC mode and can be configured based on the application requirement By Default EXT\_FEAT\_352K\_M4SS\_320K configuration will be selected in SoC mode 
    - EXT\_FEAT\_480K\_M4SS\_192K - This mode configures TA with 480k and M4 with 192K bytes of memory
    - EXT\_FEAT\_416K\_M4SS\_256K - This mode configures TA with 416k and M4 with 256K bytes of memory
    - EXT\_FEAT\_352K\_M4SS\_320K - This mode configures TA with 352k and M4 with 320K bytes of memory
  - The currently supported memory configuration for NCP mode is 672K\_M4SS\_0K.
  - SoC mode should not use 672k\_M4SS\_0k Memory configuration.
  - In SoC mode, M4 power save will work with any memory configuration.
- Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.

**IDE**

- Download the latest Simplicity Studio (SV5.7.0.1)
- Refer to the latest version of the SoC "Getting-Started-with-SiWx917" guide for more details

**Wi-Fi**

- It is recommended to enable bit 16 of the 'Extended TCP IP Feature' bit map in the opermode command for all Wi-Fi Socket Operations from the host to ensure graceful handling during asynchronous closures from the peer.
- Aggregation (bit 2 of feature\_bit\_map) is recommended to be enabled in opermode. This enables sw to aggregate multiple packets to achieve better throughput
- TCP retransmission count value is recommended to be 30 or higher so that the TCP does not disconnect while the rejoin happens.
- To restart the module, the application needs to call sl\_wifi\_deinit() followed by sl\_wifi\_init().
- Register 'Join Fail Callback' function every time when join is called, as in the rejoin failure path the callback is deregistered. If not done, this might result in a scan not functioning on rejoin failure.
- User can enable SL\_SI91X\_EXT\_TCP\_IP\_SSL\_16K\_RECORD in 'Extended TCP IP Feature'bit map in opermode, if HTTPS download is failing after receiving HTTP headers..
- iTWT setup is recommended after IP assignment/TCP connection/application connection.
- When using sl\_wifi\_enable\_target\_wakeup\_time API, increase TCP / ARP Timeouts at the remote side depending upon the configured TWT interval configured.
- In case of TWT+coex, when using sl\_wifi\_enable\_target\_wakeup\_time API, use TWT wake duration <= 16 ms and TWT wake interval >= 1 sec. If wake duration > 16 ms or TWT wake interval < 1sec, there might be performance issues.
- For iTWT GTK Interval Should be kept max possible value or zero. If GTK interval is not configurable, recommended TWT interval (in case of sl\_wifi\_enable\_target\_wakeup\_time API) 
- Disable Power Save for High Throughput Applications.
- The application needs to ensure that it sets RTC with the correct timestamp when the feature is enabled before establishing the SSL connection.
- Timeout value should be minimum 1 second for socket select and socket receive calls. Timeout value of less than 1 second is not currently supported.
- When sl\_wifi\_enable\_target\_wakeup\_time API is used, configuring TWT Wake interval beyond 1 min might lead to disconnections from the AP. Recommended to use TWT wakeup interval of less than or equal to 1 min.
- MQTT keep alive interval should be either 0 or a value greater than 35 seconds.
- When using sl\_wifi\_enable\_target\_wakeup\_time API, it is recommended to set missed\_beacon\_count of sl\_wifi\_set\_advanced\_client\_configuration API greater than 2 times of the configured TWT Interval.
- Secure OTA: Keys need to be programmed before validating Secure OTA

**BLE**

- In BLE, recommended range of Connection Interval in 
  - Power Save (BLE Only) - 100 ms to 1.28 s.
- In BLE, during Connection, the configuration of Scan Interval and Scan Window with the same value is not recommended. The suggested ratio of Scan Window to Scan Interval is 3:4.
- In BLE, if a device is acting as Central, the scan window (in set\_scan\_params and create\_connection commands) must be less than the existing Connection Interval. The suggested ratio of Scan Window to Connection Interval is 2:3.
- In BLE mode, if scanning and advertising are in progress on the SiWx91x module and it subsequently gets connected and moves to the central role, scanning stops else if it moves to the peripheral role, advertising stops. To further establish a connection to another peripheral device or to a central device, the application should give a command for starting advertising and scanning again.

**Co-Existence**

- For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
- Wi-Fi + BLE Advertising 
  - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals may be observed.
  - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity.
- Wi-Fi + BLE scanning 
  - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
  - BLE scanning will be stopped for intervals that collide with Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Connections 
  - All standard connection intervals are supported.
  - For a stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity.
- Wi-Fi + BLE Central/Peripheral Data Transfer 
  - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout.
  - Ensure Wi-Fi activity consumes lower intervals.

## **Known Issues**

**Network Stack**

- Sometimes during SSL Handshake, ECC curve parameters generated are wrong, resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
- Recommended MQTT Publish payload is 1 kBytes.
- Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with the power save.
- Secure SSL Renegotiation not supported in Embedded Networking Stack.

**Wi-Fi**

- Device with active TWT will not go to sleep when the buffer is full.
- There are performance and IOP issues with MU MIMO.
- WPA3 connection takes 2.5 seconds.
- Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.
- EAP connection with certificates of 4096-bit key length is not supported.
- Low Wi-Fi+BLE throughput
- Data stall and disconnection with TCP Tx data transfer when TA power save is enabled
- TCP/UDP TX/RX Data stall observed in CoEx scenarios
- HTTPS Continuous download fails with TA Power save
- HTTP get and post are not working from the second iteration i.e., upon WLAN reconnection
- Low-performance metrics.

**BLE**

- BLE Range is less with A0/B0 dual flash Radio boards when compared to A0/B0 common flash Radio boards.
- Wireless Init failure observed in BLE Central application with (352k,320k) configuration
- BLE applications hang after disconnection due to Disable Power save API failure

**System**

- None

### **MCU**

- Refer the respective Radio board user guides for specific pinout details. Example readme's currently refer to A0 board versions
- IOSTREAM by default works only with 115200 baud rate. To work with different baud rates , undefine the  USART\_UC macro in sl\_91x..usart.c.
- SSI bitrate above 10 Mbps is not working as expected.
- when using SL DMA, UDMA1 is not working as expected
- SIO-SPI few channels are not working as expected. (working of MOSI & MISO channel 4 and above is inconsistent) 
- Few peripherals like WDT when configured with RO clock is not working as expected. No impact on the feature but the clocks are not accurate.

## **Limitations and Unsupported Features**

**Wi-Fi/Network Stack**

- TLS 1.3 Server is not supported
- In AP mode, IPv6 Router Advertisements are not supported. Although STA connected to the APUT can transfer IPv6 data to the peers by using SLAAC for address configuration.
- When sl\_wifi\_enable\_target\_wakeup\_time() API is used, observed occasional MQTT disconnections with longer TWT intervals (>30secs) with embedded MQTT + TWT.
- AMSDU TX is not supported.
- Fragmentation is not supported.
- AMSDUs within AMPDU is not supported.
- Currently, the module does not support Radio Measurement Requests feature of CCX V2
- 802.11k is not supported
- Short GI is not supported
- 40 MHz bandwidth for 2.4 GHz band is not supported.
- 802.11J channel 14 is not supported.
- MQTT versions 3 and 4 are supported.
- Maximum of 3 SSL connections are supported in Wi-Fi alone mode.
- In SSL ECC Curve ID supported is 23. SSL handshake with 3rd party clients depends on the SSL ECC Curve ID.
- LP mode is not supported.
- Fine Tuning 802.11ax Spectral mask in Progress
- Fine Tuning 11b mask in Ch11 in progress
- In iTWT mode, downlink data traffic should be limited. Disconnections may be observed if AP fails to send all the buffered data in the next TWT Service Period.
- Number of MBSSIDs processed is limited by the beacon length that can be processed by the stack (which is limited to 1024 bytes). Beacons greater than 1024 Bytes in length will not be processed.
- In A0 SoC mode, M4 power save will work with 384k TA and 320 M4 memory configuration only. It is not supported for "448K TA and 256k M4" and "512k TA and 192k M4" memory configurations
- Timeout value should be a minimum 1 second for socket select and socket receive calls. Timeout value less than 1 second is not currently supported.
- M4 OTA update on dual flash will work for same flash sizes only (internal and External).
- CoEx + EAP Security in 352K/320k is not feasible. This issue is observed due to a lack of memory in the 352k/320k Memory Configuration. It is supported with other EXT\_FEAT\_416K\_M4SS\_256K and EXT\_FEAT\_480K\_M4SS\_192K memory configurations.
- UL MUMIMO is not supported.
- The embedded HTTP server is not supported.
- SNI is not supported
- Baremetal is not supported

**BLE**

- For BLE, if the connection is established with a small connection interval (less than 15 ms), simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported.
- BLE maximum two concurrent connections are supported, which can be either a connection to two peripheral devices, to one central and one peripheral device or two central devices.
- BLE Slave latency value is valid up to 32 only.
- BLE TX/RX throughput is less when tested with EFM as compared to EFR
- Maximum supported AE data length is 200 bytes.
- Supports only two ADV\_EXT sets.
- Supports only two BLE connections( 1 Central and 1 Peripheral) with AE.
- Filtering policy is not supported for Periodic advertising.

**MCU**

- ULP Peripherals examples configurations are for RAM-based execution, as flash is powered off in ULP Mode
- On 4338a board, two I2C instances(I2C1 and I2C2) are using same GPIOs ,so support is not there for using two I2C instances simultaneously
- I2C Fast and High speed mode not supported
- when using GSPI and SSI, manual chip select option not supported

**Simplicity Studio and Commander**

- Default GDB debug configuration is not supported for SoC projects, work around details provided in "Getting-Started-with-SiWx917"
- Observing flash download failures(ERROR 108) for a few image sizes
- While creating project through Simplicity Studio, "copy contents" option not supported.
- Simplicity commander does not support options under "Debug Lock tools" . 

## **Terminology**

|**Term**|**Description**|
| :-: | :-: |
|**New Features**|These items are new to this release|
|**Changes/Issues Fixed**|Changes made to existing features found in previous software releases.|
||Enhancements to existing product flow and feature set.|
||Bug fixes done in the Release|
|**Deprecated Items**|Features or functions or APIs that are removed from the distributed release to align with the software roadmap|
|**Known Issues**|Features or functions that do not work as planned at the time of release. Workarounds may be offered to meet short-term development goals, but longer-term solutions will be added in future software releases|
|**Limitations/Recommendations**|Describes the limitations on product usage and recommendations for optimal use cases|

## **Note**

- NA

<br>

# WiSeConnect3_SDK_3.0.10 Release Notes

This SDK package supports SoC Mode only.

- In SoC Mode: Customer Applications can run on Cortex M4 and Wireless stacks, Networking stacks run on the Wireless subsystem.
- Silicon Labs SiWx917 includes an ultra-low power Wi-Fi 6 plus Bluetooth Low Energy (BLE) 5.4 wireless subsystem and an integrated micro-controller application subsystem.

> Note: The release supports only Simplicity Studio 5.7 & above versions.

## Highlights

- **Operating Modes**: Wi-Fi STA, Wi-Fi AP, Concurrent (AP + STA), Wi-Fi STA+BLE (CoEx)
- **Wireless Protocols**: IEEE 802.11b, 802.11g, 802.11n, 802.11ax, BLE 5.4
- **MCU Peripherals**: ADC Static Mode, Analog Comparator, Bod, CTS, DAC FIFO, DAC Static, OPAMP, GPIO, I2C, I2S, PWM, RTC, QEI, SPI, UART, USART, UDMA, WDT
  - ULP (Ultra Low Power) Peripherals: ulp_gpio, ulp_i2c, ulp_i2s, ulp_ir_decoder, ulp_spi, ulp_timer, ulp_uart, ulp_udma
  - UART and I2C Deep Sleep Wakeup Without RAM Retention
- **Multiple Ultra Low Power Modes**: Wi-Fi 6 TWT (Target Wake Time) - for improved network efficiency and device battery life
- **Wi-Fi**: 2.4GHz Wi-Fi Support - WPA2/WPA3 Personal, Wi-Fi 6 OFDMA/MU-MIMO Support that enhances network capacity and latency
- **Cloud**: Amazon Cloud Connectivity Support
- **BLE**: BLE 5.4, BLE dual role (Central and Peripheral Support)
- **Integrated Stacks and Profiles**: Wi-Fi Stack, BLE Stack and Profiles, TCP/IP stack with TLS 1.3, HTTP/HTTPS, DHCP, MQTT Client, SNTP Client, DNS Client
- **IDE**: Simplicity Studio with Windows 10 (64-bit), Linux, MacOS
- **TA/M4 Firmware Update Tool**: Simplicity Commander
- **Hardware kits to evaluate SoC Mode**: Si917-BRD4325A (Common Flash Radio board), Si917-BRD4325B (Dual Flash Radio board), Si917-BRD4325G (Common Flash Radio board + extended PSRAM)

## Release Details

| Item                      | Details                                                        |
| ------------------------- | -------------------------------------------------------------- |
| Release date              | July 27th, 2023                                                |
| SDK Version               | 3.0.10(Build 1)                                                |
| Firmware Version          | 2.9.0.0.15                                                     |
| Package Name              | WiSeConnect3_SDK_3.0.10                                        |
| Supported RTOS            | FreeRTOS                                                       |
| Operating Modes Supported | Wi-Fi STA, Wi-Fi AP, Concurrent(AP + STA), Wi-Fi STA+BLE(CoEx) |
| Build Quality             | pre-beta                                                       |

## Updating to this Release

This release includes the Wireless Library, which operates on the internal Cortex M4 in SoC Mode.

## Features and Bug Fixes

- Added support for command line interface application (cli_demo).
- IOSTREAM support has been integrated into GSDK 4.3.1.

## Known Issues

- I2C High-Speed mode is not working as expected.
- SSI bitrate above 10 Mbps is not working as expected.
- DUT is getting hanged while continuously publishing data in the embedded MQTT application during power save mode.
- BLE notification and Continuous BG scan are not working as expected.
- BLE radio's range of 4-5m for BRD4325B A02 boards, .
- Intermittent Bus thread hang issue with LWIP external stack.
- The "Browser File Viewer" plugin has issues. It can't render the last image included in the markdown files and has observed issues accessing some hyperlinks.
- Known issues in ble per and wlan rf features
- Data rates are not effecting on the dut with 11b, 11g, 11n modes.
- BLE connection is not happening when power save is enabled in BLE-Central application

### SOC

- In SoC mode, M4 power save will only work with the 384k TA and 320 M4 memory configuration. It is not supported for "448K TA and 256k M4" and "512k TA and 192k M4" memory configurations.

### Wi-Fi

- WPA3 connection takes 2.5 seconds more than WPA2 connection.
- An issue has been observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.

### Network Stack

### BLE

- For BLE, dual role scenario issues might occur when connecting as central if the advertising is also going out for the peripheral role. Central connections can be made after the peripheral connections are established and advertising is stopped.
- LMP timeout BLE disconnection has been observed with the latest Realme mobile handsets.

## Limitations and Unsupported Features

### Wi-Fi/Network Stack Limitations

- Throughput is not on par with WiSeConnect 2 in the present release.
- A maximum of 3 SSL connections is supported in WiFi alone mode.
- TWT is not verified in Coex mode.
- BGscan is not verified with the TWT feature.

### SoC Limitations

- SIO currently supports SPI and UART.
- Peripherals currently use UDMA CMSIS driver.

### BLE Limitations

- For BLE, simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported if the connection is established with a small connection interval (less than 15 ms).
- BLE supports a maximum of two concurrent connections, which can be either a connection to two peripheral devices, one central and one peripheral device, or two central devices.
- The BLE Slave latency value is valid up to 32 only.

#### Unsupported Features

- SNI (HTTPS, TLS/SSL).
- Keep Alive configurability.
- Console application for customers.
- Multi-threaded APIs.
- 917-specific features (debugging utilities).
- APIs for Crypto Hardware (ECDH, DH, SHA, AES), Calibration software & stats.
- SoC Support (Hosted mode): support for switching between internal & external stack in SS.
- Protocols Offload mode: HTTP Server (917 specific), DHCPv6 Server, SNMP, Socket reads up to 64k, POP3, DTLS, mDNS, Web Socket, DNS Server.
- Power save support (Message-based).
- PTA Coexistence with other wireless.
- Baremetal support.
- Asynchronous socket APIs.
- NCP support (SPI, SDIO, UART, Linux, 3rd-party hosts).

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

# WiSeConnect3_SDK_3.0.9 Release Notes

This SDK package supports SoC Mode only.

- In SoC Mode: Customer Applications can run on Cortex M4 and Wireless stacks and Networking stacks run on the Wireless subsystem.
- Silicon Labs SiWx917 includes an ultra-low power Wi-Fi 6 plus Bluetooth Low Energy (BLE) 5.1 wireless subsystem and an integrated micro-controller application subsystem.

> Note: To run the application, please install the GSDK patch following the instructions below:
>
> - Install GSDK version 4.3.0.
> - Open your terminal application.
> - Go to your GSDK path and enter the following command:
      git apply --ignore-whitespace --ignore-space-change "extension/wiseconnect3/utilities/gsdk_service_patch/gsdk_service.patch"

> Note: The `git apply` command needs to be run to apply a GSDK patch not available with the current WiSeConnect 3 release.
>
> - Now you should be able to build and run Dx sample applications.

> Note: The release supports only Simplicity Studio 5.7 & above versions.

## SoC Highlights

- **Operating Modes**: Wi-Fi STA, Wi-Fi AP, Concurrent (AP + STA), Wi-Fi STA+BLE (CoEx)
- **Wireless Protocols**: IEEE 802.11b, 802.11g, 802.11n, 802.11ax, BLE 5.1
- **MCU Peripherals**: ADC Static Mode, Analog Comparator, Blinky, Bod, CTS, DAC FIFO, DAC Static, OPAMP, GPIO, I2C, I2S, PWM, RTC, QEI, SPI, UART, USART, UDMA, WDT, Combined Peripheral App
  - ULP (Ultra Low Power) Peripherals: ulp_gpio, ulp_i2c, ulp_i2s, ulp_ir_decoder, ulp_spi, ulp_timer, ulp_uart, ulp_udma
  - UART and I2C Deep Sleep Wakeup Without RAM Retention
- **Multiple Ultra Low Power Modes**: Wi-Fi 6 TWT (Target Wake Time) - for improved network efficiency and device battery life
- **Wi-Fi**: 2.4GHz Wi-Fi Support - WPA2/WPA3 Personal, Wi-Fi 6 OFDMA/MU-MIMO Support that enhances network capacity and latency
- **Cloud**: Amazon Cloud Connectivity Support
- **BLE**: BLE 5.1, BLE dual role (Central and Peripheral Support)
- **Integrated Stacks and Profiles**: Wi-Fi Stack, BLE Stack and Profiles, TCP/IP stack with TLS 1.3, HTTP/HTTPS, DHCP, MQTT Client, SNTP Client
- **IDE**: Simplicity Studio with Windows 10 (64-bit)
- **TA/M4 Firmware Update Tool**: Simplicity Commander
- **Hardware kits to evaluate SoC Mode**: Si917-BRD4325A (Common flash Radio board), Si917-BRD4325B (Dual flash Radio board), Si917-BRD4325G (Common Flash Radio board + extended PSRAM)

## Release Details

| Item                      | Details                                                        |
| ------------------------- | -------------------------------------------------------------- |
| Release date              | July 10th, 2023                                                |
| API Version               | 3.0.9(Build 1)                                                 |
| Firmware Version          | 2.9.0.0.8                                                      |
| Package Name              | WiSeConnect3_SDK_3.0.9                                         |
| Supported RTOS            | FreeRTOS                                                       |
| Operating Modes Supported | Wi-Fi STA, Wi-Fi AP, Concurrent(AP + STA), Wi-Fi STA+BLE(CoEx) |

## Updating to this Release

This release includes the Wireless Library, which operates on the internal Cortex M4 in SoC Mode.

## Features and Bug Fixes

- Supported DX Peripherals: Calendar, ULP Timer, GSPI, SSI, GPIO, I2C, Watchdog Timer, DMA, USART, Config Timer, SIO.
- Supported Services: Sleeptimer, IOStream, NVM3.
- Added support for IPv6.
- Added support for FreeRTOS.
- Added support for LWIP stack.
- Added support for AWS MQTT library.
- Added support for EAP-FAST and EAP-PEAP.
- Added support for 802.11ax protocol & TWT.
- Added support for TCP, UDP, TLS/SSL sockets.
- Added support for TA & M4 Powersave profiles.
- Added Support for Common & dual Flash radio boards.
- Added support for multi-threaded application development.
- Added support for WPA2/WPA3 Personal, Enterprise security in station Mode.
- Added support for BSD and IoT socket application programming interface (API).
- Added support for Network Application protocol for Offloaded mode (TCP/IP internal stack) with HTTP/HTTPS, DHCP, MQTT Client, DNS Client, SNTP Client.
- Added support for Bluetooth Low Energy (BLE) central & peripheral connections.
- Added support for Wireless firmware upgradation (OTA).
- Added support for Platform-agnostic Wi-Fi, Network APIs.
- Added API documentation, migration guide, Getting Started Guide.

## Known Issues

- B0 (V2.0) support will be provided in subsequent releases.
- I2C High-Speed mode is not working as expected.
- SSI bitrate above 10 Msps is not working as expected.
- Powersave Deepsleep is not working as expected on Common Flash B0 Board.
- DUT is getting hanged while continuously publishing data in the embedded MQTT application during power save mode.
- BLE notification and Continuous BG scan are not working as expected.
- Ongoing efforts to update the readme file are in progress.
- The "Browser File Viewer" plugin has issues. It can't render the last image included in the markdown files and has observed issues accessing some hyperlinks.

### SOC

- In SoC mode, M4 power save will only work with the 384k TA and 320 M4 memory configuration. It is not supported for "448K TA and 256k M4" and "512k TA and 192k M4" memory configurations.

### Wi-Fi

- WPA3 connection takes 2.5 seconds more than WPA2 connection.
- An issue has been observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.

### Network Stack

### BLE

- For BLE, dual role scenario issues might occur when connecting as central if the advertising is also going out for the peripheral role. Central connections can be made after the peripheral connections are established and advertising is stopped.
- LMP timeout BLE disconnection has been observed with the latest Realme mobile handsets.

## Limitations and Unsupported Features

### Wi-Fi/Network Stack Limitations

- Throughput is not on par with WiSeConnect 2 in the present release.
- A maximum of 3 SSL connections is supported in WiFi alone mode.
- TWT is not verified in Coex mode.
- BGscan is not verified with the TWT feature.

### SoC Limitations

- SIO currently supports SPI and UART.
- Peripherals currently use UDMA CMSIS driver.

### BLE Limitations

- For BLE, simultaneous roles (i.e., Central + Scanning and Peripheral + Advertising) are not supported if the connection is established with a small connection interval (less than 15 ms).
- BLE supports a maximum of two concurrent connections, which can be either a connection to two peripheral devices, one central and one peripheral device, or two central devices.
- The BLE Slave latency value is valid up to 32 only.

#### Unsupported Features

- SNI (HTTPS, TLS/SSL).
- Keep Alive configurability.
- Console application for customers.
- Multi-threading support for relevant APIs.
- 917-specific features (debugging utilities).
- APIs for Crypto Hardware (ECDH, DH, SHA, AES), Calibration software & stats.
- SoC Support (Hosted mode): support for switching between internal & external stack in SS.
- Protocols Offload mode: HTTP Server (917 specific), DHCPv6 Server, SNMP, Socket reads up to 64k, POP3, DTLS, mDNS, Web Socket, DNS Server.
- Power save support (Message-based).
- PTA Coexistence with other wireless.
- Baremetal support.
- Asynchronous socket APIs.
- NCP support (SPI, SDIO, UART, Linux, 3rd-party hosts).

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