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
